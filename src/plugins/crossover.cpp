/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 8 сент. 2020 г.
 *
 * lsp-plugins is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-plugins is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-plugins. If not, see <https://www.gnu.org/licenses/>.
 */

#include <core/debug.h>
#include <core/colors.h>
#include <core/util/Color.h>
#include <plugins/crossover.h>

#define BUFFER_SIZE             0x1000
#define TRACE_PORT(p)           lsp_trace("  port id=%s", (p)->metadata()->id);

namespace lsp
{
    crossover_base::crossover_base(const plugin_metadata_t &metadata, size_t mode): plugin_t(metadata)
    {
        nMode           = 0;
        bEnvUpdate      = true;
        vChannels       = NULL;
        fInGain         = GAIN_AMP_0_DB;
        fOutGain        = GAIN_AMP_0_DB;
        fZoom           = GAIN_AMP_0_DB;

        pData           = NULL;
        vTr             = NULL;
        vBFc            = NULL;
        vFreqs          = NULL;
        vCurve          = NULL;
        vIndexes        = NULL;
        pIDisplay       = NULL;

        pBypass         = NULL;
        pInGain         = NULL;
        pOutGain        = NULL;
        pReactivity     = NULL;
        pShiftGain      = NULL;
        pZoom           = NULL;
        pMSOut          = NULL;
    }

    crossover_base::~crossover_base()
    {
    }

    void crossover_base::init(IWrapper *wrapper)
    {
        // Initialize plugin
        plugin_t::init(wrapper);

        // Determine number of channels
        size_t channels         = (nMode == XOVER_MONO) ? 1 : 2;
        size_t sz_channels      = ALIGN_SIZE(channels * sizeof(channel_t), DEFAULT_ALIGN);
        size_t filter_mesh_size = ALIGN_SIZE(crossover_base_metadata::FFT_MESH_POINTS * sizeof(float), DEFAULT_ALIGN);

        size_t to_alloc         = sz_channels +
                                  2 * filter_mesh_size + // vTr (both complex and real)
                                  2 * filter_mesh_size + // vBFc (both complex and real)
                                  crossover_base_metadata::FFT_MESH_POINTS * sizeof(float)    + // vFreqs
                                  crossover_base_metadata::FFT_MESH_POINTS * sizeof(uint32_t) + // vIndexes
                                  channels * (
                                      BUFFER_SIZE * sizeof(float)             +                   // vBuffer
                                      crossover_base_metadata::BANDS_MAX * filter_mesh_size * 2   // band.vTr
                                  );

        // Initialize analyzer
        size_t an_cid           = 0;
        if (!sAnalyzer.init(2*channels, crossover_base_metadata::FFT_RANK))
            return;

        sAnalyzer.set_rank(crossover_base_metadata::FFT_RANK);
        sAnalyzer.set_activity(false);
        sAnalyzer.set_envelope(envelope::WHITE_NOISE);
        sAnalyzer.set_window(crossover_base_metadata::FFT_WINDOW);
        sAnalyzer.set_rate(crossover_base_metadata::REFRESH_RATE);

        // Allocate memory
        uint8_t *ptr    = alloc_aligned<uint8_t>(pData, to_alloc);
        if (ptr == NULL)
            return;
        lsp_guard_assert(uint8_t *save   = ptr);

        // Assign pointers
        vChannels       = reinterpret_cast<channel_t *>(ptr);       // Audio channels
        ptr            += sz_channels;
        vTr             = reinterpret_cast<float *>(ptr);           // Transfer buffer
        ptr            += filter_mesh_size * 2;
        vBFc            = reinterpret_cast<float *>(ptr);           // Band filter characteristics buffer
        ptr            += filter_mesh_size * 2;
        vFreqs          = reinterpret_cast<float *>(ptr);           // Graph frequencies
        ptr            += crossover_base_metadata::FFT_MESH_POINTS * sizeof(float);
        vIndexes        = reinterpret_cast<uint32_t *>(ptr);
        ptr            += crossover_base_metadata::FFT_MESH_POINTS * sizeof(uint32_t);

        // Initialize channels
        for (size_t i=0; i<channels; ++i)
        {
            channel_t *c        = &vChannels[i];

            c->sBypass.construct();
            c->sXOver.construct();

            if (!c->sXOver.init(crossover_base_metadata::BANDS_MAX, BUFFER_SIZE))
                return;

            for (size_t i=0; i<crossover_base_metadata::BANDS_MAX; ++i)
            {
                xover_band_t *b     = &c->vBands[i];

                b->vBandOut         = NULL;
                b->vAllOut          = NULL;

                vTr                 = reinterpret_cast<float *>(ptr);           // Transfer buffer
                ptr                += filter_mesh_size * 2;

                b->fMakeup          = GAIN_AMP_0_DB;

                b->pSolo            = NULL;
                b->pMute            = NULL;
                b->pMakeup          = NULL;
                b->pFreqEnd         = NULL;
                b->pOut             = NULL;
            }

            for (size_t i=0; i<crossover_base_metadata::BANDS_MAX-1; ++i)
            {
                xover_split_t *s    = &c->vSplit[i];

                s->pSlope           = NULL;
                s->pFreq            = NULL;
            }

            c->vIn              = NULL;
            c->vOut             = NULL;
            c->vBuffer          = reinterpret_cast<float *>(ptr);
            ptr                += BUFFER_SIZE * sizeof(float);
            c->vTr              = NULL;

            c->nAnInChannel     = an_cid++;
            c->nAnOutChannel    = an_cid++;
            c->bInFft           = false;
            c->bOutFft          = false;

            c->pIn              = NULL;
            c->pOut             = NULL;
            c->pFftIn           = NULL;
            c->pFftInSw         = NULL;
            c->pFftOut          = NULL;
            c->pFftOutSw        = NULL;
            c->pAmpGraph        = NULL;
            c->pInLvl           = NULL;
            c->pOutLvl          = NULL;
        }

        lsp_assert(ptr <= &save[to_alloc]);

        // Bind ports
        size_t port_id              = 0;

        // Input ports
        lsp_trace("Binding input ports");
        for (size_t i=0; i<channels; ++i)
        {
            TRACE_PORT(vPorts[port_id]);
            vChannels[i].pIn        =   vPorts[port_id++];
        }

        // Input ports
        lsp_trace("Binding output ports");
        for (size_t i=0; i<channels; ++i)
        {
            TRACE_PORT(vPorts[port_id]);
            vChannels[i].pOut       =   vPorts[port_id++];
        }

        // Bind
        lsp_trace("Binding crossover outputs");
        if (channels < 2)
        {
            for (size_t i=0; i<crossover_base_metadata::BANDS_MAX; ++i)
            {
                TRACE_PORT(vPorts[port_id]);
                vChannels[0].vBands[i].pOut     =   vPorts[port_id++];
            }
        }
        else
        {
            for (size_t i=0; i<channels; ++i)
            {
                TRACE_PORT(vPorts[port_id]);
                vChannels[0].vBands[i].pOut     =   vPorts[port_id++];
                TRACE_PORT(vPorts[port_id]);
                vChannels[1].vBands[i].pOut     =   vPorts[port_id++];
            }
        }

        // Bind bypass
        TRACE_PORT(vPorts[port_id]);
        pBypass         =   vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pInGain         =   vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pOutGain        =   vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pReactivity     = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pShiftGain      = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pZoom           = vPorts[port_id++];

        if (nMode == XOVER_MS)
        {
            TRACE_PORT(vPorts[port_id]);
            pMSOut          = vPorts[port_id++];
        }

        // Bind channel ports
        lsp_trace("Binding channel ports");

        for (size_t i=0; i<channels; ++i)
        {
            channel_t *c    = &vChannels[i];

            if ((i > 0) && (nMode == XOVER_STEREO))
            {
                channel_t *sc           = &vChannels[0];
                c->pAmpGraph            = sc->pAmpGraph;
            }
            else
            {
                TRACE_PORT(vPorts[port_id]);
                port_id++;              // Skip filter curves switch
                TRACE_PORT(vPorts[port_id]);
                c->pAmpGraph            = vPorts[port_id++];
            }
        }

        lsp_trace("Binding meters");
        for (size_t i=0; i<channels; ++i)
        {
            channel_t *c    = &vChannels[i];

            TRACE_PORT(vPorts[port_id]);
            c->pFftInSw             = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            c->pFftOutSw            = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            c->pFftIn               = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            c->pFftOut              = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            c->pInLvl               = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            c->pOutLvl              = vPorts[port_id++];
        }

        // Split frequencies
        lsp_trace("Binding split frequencies");
        for (size_t i=0; i<channels; ++i)
        {
            for (size_t j=0; j<crossover_base_metadata::BANDS_MAX-1; ++j)
            {
                xover_split_t *s    = &vChannels[i].vSplit[j];

                if ((i > 0) && (nMode == XOVER_STEREO))
                {
                    xover_split_t *sc   = &vChannels[0].vSplit[j];
                    s->pSlope           = sc->pSlope;
                    s->pFreq            = sc->pFreq;
                }
                else
                {
                    TRACE_PORT(vPorts[port_id]);
                    s->pSlope           = vPorts[port_id++];
                    TRACE_PORT(vPorts[port_id]);
                    s->pFreq            = vPorts[port_id++];
                }
            }
        }

        // Bands
        lsp_trace("Binding band controllers");
        for (size_t i=0; i<channels; ++i)
        {
            for (size_t j=0; j<crossover_base_metadata::BANDS_MAX-1; ++j)
            {
                xover_band_t *b     = &vChannels[i].vBands[j];

                if ((i > 0) && (nMode == XOVER_STEREO))
                {
                    xover_band_t *sb    = &vChannels[0].vBands[j];
                    b->pSolo            = sb->pSolo;
                    b->pMute            = sb->pMute;
                    b->pMakeup          = sb->pMakeup;
                    b->pFreqEnd         = sb->pFreqEnd;
                }
                else
                {
                    TRACE_PORT(vPorts[port_id]);
                    b->pSolo            = vPorts[port_id++];
                    TRACE_PORT(vPorts[port_id]);
                    b->pMute            = vPorts[port_id++];
                    TRACE_PORT(vPorts[port_id]);
                    b->pMakeup          = vPorts[port_id++];
                    TRACE_PORT(vPorts[port_id]);
                    ++port_id; // Skip hue
                    TRACE_PORT(vPorts[port_id]);
                    b->pFreqEnd         = vPorts[port_id++];
                }
            }
        }
    }

    void crossover_base::destroy()
    {
        // Determine number of channels
        size_t channels     = (nMode == XOVER_MONO) ? 1 : 2;

        // Destroy channels
        if (vChannels != NULL)
        {
            for (size_t i=0; i<channels; ++i)
            {
                channel_t *c    = &vChannels[i];

                c->sXOver.destroy();
                c->vBuffer      = NULL;
                c->vTr          = NULL;
            }

            vChannels       = NULL;
        }

        if (pIDisplay != NULL)
        {
            pIDisplay->detroy();
            pIDisplay   = NULL;
        }

        // Destroy data
        if (pData != NULL)
            free_aligned(pData);

        if (pIDisplay != NULL)
        {
            pIDisplay->detroy();
            pIDisplay   = NULL;
        }

        // Destroy analyzer
        sAnalyzer.destroy();

        // Destroy plugin
        plugin_t::destroy();
    }

    void crossover_base::update_settings()
    {
        // TODO
    }

    void crossover_base::update_sample_rate(long sr)
    {
        // TODO
    }

    void crossover_base::ui_activated()
    {
        // TODO
    }

    void crossover_base::process(size_t samples)
    {
        // TODO
    }

    bool crossover_base::inline_display(ICanvas *cv, size_t width, size_t height)
    {
        // TODO
        return false;
    }

    void crossover_base::dump(IStateDumper *v) const
    {
        // TODO
    }

    //-------------------------------------------------------------------------
    // Crossover derivatives
    crossover_mono::crossover_mono(): crossover_base(metadata, XOVER_MONO)
    {
    }

    crossover_stereo::crossover_stereo(): crossover_base(metadata, XOVER_STEREO)
    {
    }

    crossover_lr::crossover_lr(): crossover_base(metadata, XOVER_LR)
    {
    }

    crossover_ms::crossover_ms(): crossover_base(metadata, XOVER_MS)
    {
    }
}


