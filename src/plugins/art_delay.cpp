/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 3 дек. 2020 г.
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

#include <plugins/art_delay.h>

#define BUFFER_SIZE             0x1000

#define TRACE_PORT(p)           lsp_trace("  port id=%s", (p)->metadata()->id);

namespace lsp
{
    art_delay_base::art_delay_base(const plugin_metadata_t &mdata, bool stereo_in): plugin_t(mdata)
    {
        bStereo         = stereo_in;
        vDataBuf[0]     = NULL;
        vDataBuf[1]     = NULL;
        vOutBuf[0]      = NULL;
        vOutBuf[1]      = NULL;
        vTempo          = NULL;
        vDelays         = NULL;
        pExecutor       = NULL;

        pIn[0]          = NULL;
        pIn[1]          = NULL;
        pOut[0]         = NULL;
        pOut[1]         = NULL;
        pBypass         = NULL;
        pMaxDelay       = NULL;
        pPan[0]         = NULL;
        pPan[1]         = NULL;
        pDryGain        = NULL;
        pWetGain        = NULL;
        pMono           = NULL;
        pFeedback       = NULL;

        pData           = NULL;
    }

    art_delay_base::~art_delay_base()
    {
        destroy();
    }

    void art_delay_base::init(IWrapper *wrapper)
    {
        plugin_t::init(wrapper);

        size_t sz_buf           = BUFFER_SIZE * sizeof(float);
        size_t sz_tempo         = ALIGN_SIZE(sizeof(art_tempo_t) * MAX_TEMPOS, ALIGN64);
        size_t sz_proc          = ALIGN_SIZE(sizeof(art_delay_t), ALIGN64);
        size_t sz_alloc         = sz_tempo + sz_proc + sz_buf * 4;

        uint8_t *ptr            = alloc_aligned<uint8_t>(pData, sz_alloc, ALIGN64);
        if (ptr == NULL)
            return;

        // Allocate data buffers
        for (size_t i=0; i<2; ++i)
        {
            vDataBuf[i]             = reinterpret_cast<float *>(ptr);
            ptr                    += sz_buf;
            vOutBuf[i]              = reinterpret_cast<float *>(ptr);
            ptr                    += sz_buf;
        }

        vTempo                  = reinterpret_cast<art_delay_t *>(ptr);
        ptr                    += sz_tempo;
        vDelays                 = reinterpret_cast<art_delay_t *>(ptr);
        ptr                    += sz_proc;

        // Initialize tempos
        for (size_t i=0; i<MAX_PROCESSORS; ++i)
        {
            art_tempo_t *at         = &vTempo[i];

            at->fTempo              = BPM_DEFAULT;
            at->fRatio              = 1.0f;
            at->bSync               = false;
        }

        // Initialize delays
        for (size_t i=0; i<MAX_PROCESSORS; ++i)
        {
            art_delay_t *ad         = &vDelays[i];

            ad->pPDelay[0]          = NULL;
            ad->pPDelay[1]          = NULL;
            ad->pCDelay[0]          = NULL;
            ad->pCDelay[1]          = NULL;
            ad->pGDelay[0]          = NULL;
            ad->pGDelay[1]          = NULL;

            ad->sEq[0].construct();
            ad->sEq[1].construct();
            ad->sBypass[0].construct();
            ad->sBypass[1].construct();

            ad->pAllocator          = new DelayAllocator(ad);
            if (ad->pAllocator == NULL)
                return;

            ad->bStereo             = bStereo;
            ad->bOn                 = false;
            ad->bSolo               = false;
            ad->bValid              = false;

            ad->sOld.fDelay         = 0.0f;
            ad->sOld.fFeedback      = 0.0f;
            if (bStereo)
            {
                ad->sOld.fPan[0]        = 0.0f;
                ad->sOld.fPan[1]        = 1.0f;
            }
            else
            {
                ad->sOld.fPan[0]        = 0.5f;
                ad->sOld.fPan[1]        = 0.5f;
            }

            ad->sNew                = ad->sOld;

            ad->pOn                 = NULL;
            ad->pTempo              = NULL;
            ad->pPan[0]             = NULL;
            ad->pPan[1]             = NULL;
            ad->pSolo               = NULL;
            ad->pMute               = NULL;
            ad->pDelayRef           = NULL;
            ad->pDelayMul           = NULL;
            ad->pBarFrac            = NULL;
            ad->pBarDenom           = NULL;
            ad->pBarMul             = NULL;
            ad->pFrac               = NULL;
            ad->pDenom              = NULL;
            ad->pDelay              = NULL;
            ad->pEqOn               = NULL;
            ad->pLcfOn              = NULL;
            ad->pLcfFreq            = NULL;
            ad->pHcfOn              = NULL;
            ad->pHcfFreq            = NULL;
            for (size_t j=0; j<EQ_BANDS; ++j)
                ad->pBandGain[j]        = NULL;
            ad->pFeedback           = NULL;
            ad->pGain               = NULL;
            ad->pOutDelay           = NULL;
            ad->pOutOfRange         = NULL;
            ad->pOutLoop            = NULL;
        }

        // Get executor service
        pExecutor               = wrapper->get_executor();
        size_t port_id          = 0;

        // Bind in/out ports
        lsp_trace("Binding audio ports");

        TRACE_PORT(vPorts[port_id]);
        pIn[0]          = vPorts[port_id++];
        if (bStereo)
        {
            TRACE_PORT(vPorts[port_id]);
            pIn[1]          = vPorts[port_id++];
        }

        TRACE_PORT(vPorts[port_id]);
        pOut[0]         = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pOut[1]         = vPorts[port_id++];

        // Bind common ports
        lsp_trace("Binding common ports");
        TRACE_PORT(vPorts[port_id]);
        pBypass         = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        port_id++; // Skip delay line selector
        TRACE_PORT(vPorts[port_id]);
        pMaxDelay       = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pPan[0]         = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pPan[1]         = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pDryGain        = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pWetGain        = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pMono           = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pFeedback       = vPorts[port_id++];

        // Bind delay ports
        lsp_trace("Binding tempo ports");
        for (size_t i=0; i<MAX_TEMPOS; ++i)
        {
            art_tempo_t *at         = &vTempo[i];
            TRACE_PORT(vPorts[port_id]);
            at->pTempo              = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            at->pRatio              = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            at->pSync               = vPorts[port_id++];
        }

        // Bind delay ports
        lsp_trace("Binding delay ports");
        for (size_t i=0; i<MAX_PROCESSORS; ++i)
        {
            art_delay_t *ad         = &vDelays[i];

            TRACE_PORT(vPorts[port_id]);
            ad->pOn                 = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            ad->pTempo              = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            ad->pPan[0]             = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            ad->pPan[1]             = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            ad->pSolo               = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            ad->pMute               = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            ad->pDelayRef           = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            ad->pDelayMul           = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            ad->pBarFrac            = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            ad->pBarDenom           = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            ad->pBarMul             = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            ad->pFrac               = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            ad->pDenom              = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            ad->pDelay              = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            ad->pEqOn               = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            ad->pLcfOn              = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            ad->pLcfFreq            = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            ad->pHcfOn              = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            ad->pHcfFreq            = vPorts[port_id++];
            for (size_t j=0; j<EQ_BANDS; ++j)
            {
                TRACE_PORT(vPorts[port_id]);
                ad->pBandGain[j]        = vPorts[port_id++];
            }
            TRACE_PORT(vPorts[port_id]);
            ad->pFeedback           = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            ad->pGain               = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            ad->pOutDelay           = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            ad->pOutOfRange         = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            ad->pOutLoop            = vPorts[port_id++];
        }
    }

    void art_delay_base::destroy()
    {
        plugin_t::destroy();

        if (vDelays != NULL)
        {
            for (size_t i=0; i<MAX_PROCESSORS; ++i)
            {
                art_delay_t *ad         = &vDelays[i];

                for (size_t j=0; j < 2; ++j)
                {
                    if (ad->pPDelay[j] != NULL)
                        delete ad->pPDelay[j];
                    if (ad->pCDelay[j] != NULL)
                        delete ad->pCDelay[j];
                    if (ad->pGDelay[j] != NULL)
                        delete ad->pGDelay[j];

                    ad->sEq[j].destroy();
                }

                if (ad->pAllocator != NULL)
                {
                    delete ad->pAllocator;
                    ad->pAllocator = NULL;
                }
            }

            vDelays     = NULL;
        }

        if (pData != NULL)
        {
            free_aligned(pData);
            pData       = NULL;
        }
    }

    bool art_delay_base::set_position(const position_t *pos)
    {
        for (size_t i=0; i<MAX_TEMPOS; ++i)
        {
            art_tempo_t *at         = &vTempo[i];
            if (at->bSync)
                return pos->beatsPerMinute != pWrapper->position()->beatsPerMinute;
        }

        return false;
    }

    void art_delay_base::update_settings()
    {
    }

    void art_delay_base::update_sample_rate(long sr)
    {
    }

    void art_delay_base::process(size_t samples)
    {
    }

    void art_delay_base::dump(IStateDumper *v) const
    {
    }

    art_delay_mono::art_delay_mono(): art_delay_base(metadata, false)
    {
    }

    art_delay_stereo::art_delay_stereo(): art_delay_base(metadata, true)
    {
    }
}



