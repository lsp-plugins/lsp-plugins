/*
 * graph_equalizer.cpp
 *
 *  Created on: 03 авг. 2016 г.
 *      Author: sadko
 */

#include <core/debug.h>
#include <core/colors.h>
#include <core/util/Color.h>
#include <plugins/graph_equalizer.h>

#define EQ_BUFFER_SIZE          0x1000
#define TRACE_PORT(p)           lsp_trace("  port id=%s", (p)->metadata()->id);

namespace lsp
{
    const float graph_equalizer_base::band_frequencies[] =
    {
        16.0f, 20.0f, 25.0f, 31.5f, 40.0f, 50.0f, 63.0f, 80.0f, 100.0f, 125.0f,
        160.0f, 200.0f, 250.0f, 315.0f, 400.0f, 500.0f, 630.0f, 800.0f, 1000.0f, 1250.0f,
        1600.0f, 2000.0f, 2500.0f, 3150.0f, 4000.0f, 5000.0f, 6300.0f, 8000.0f, 10000.0f, 12500.0f,
        16000.0f, 20000.0f
    };

    graph_equalizer_base::graph_equalizer_base(const plugin_metadata_t &metadata, size_t bands, size_t mode): plugin_t(metadata)
    {
        vChannels       = NULL;
        nBands          = bands;
        nMode           = mode;
        nFftPosition    = FFTP_NONE;
        nSlope          = -1;
        bListen         = false;
        bMatched        = false;
        fInGain         = 1.0f;
        fZoom           = 1.0f;
        vFreqs          = NULL;
        vIndexes        = NULL;
        pIDisplay       = NULL;

        pEqMode         = NULL;
        pSlope          = NULL;
        pListen         = NULL;
        pInGain         = NULL;
        pOutGain        = NULL;
        pBypass         = NULL;
        pFftMode        = NULL;
        pReactivity     = NULL;
        pShiftGain      = NULL;
        pZoom           = NULL;
        pBalance        = NULL;
    }

    graph_equalizer_base::~graph_equalizer_base()
    {
        destroy();
    }

    void graph_equalizer_base::init(IWrapper *wrapper)
    {
        // Pass wrapper
        plugin_t::init(wrapper);

        // Determine number of channels
        size_t channels     = (nMode == EQ_MONO) ? 1 : 2;

        // Initialize analyzer
        if (!sAnalyzer.init(channels, graph_equalizer_base_metadata::FFT_RANK))
            return;

        sAnalyzer.set_rank(graph_equalizer_base_metadata::FFT_RANK);
        sAnalyzer.set_activity(false);
        sAnalyzer.set_envelope(graph_equalizer_base_metadata::FFT_ENVELOPE);
        sAnalyzer.set_window(graph_equalizer_base_metadata::FFT_WINDOW);
        sAnalyzer.set_rate(graph_equalizer_base_metadata::REFRESH_RATE);

        // Allocate channels
        vChannels           = new eq_channel_t[channels];
        if (vChannels == NULL)
            return;
            
        // Initialize global parameters
        fInGain             = 1.0f;
        bListen             = false;
        nFftPosition        = FFTP_NONE;

        // Allocate indexes
        vIndexes            = new uint32_t[graph_equalizer_base_metadata::MESH_POINTS];
        if (vIndexes == NULL)
            return;

        // Allocate buffer
        size_t allocate     = (EQ_BUFFER_SIZE + (nBands + 1)*graph_equalizer_base_metadata::MESH_POINTS*2) * channels + graph_equalizer_base_metadata::MESH_POINTS;
        float *abuf         = new float[allocate];
        if (abuf == NULL)
            return;

        // Clear all floating-point buffers
        dsp::fill_zero(abuf, allocate);

        vFreqs              = abuf;
        abuf               += graph_equalizer_base_metadata::MESH_POINTS;

        // Allocate channel data
        for (size_t i=0; i<channels; ++i)
        {
            // Allocate data
            eq_channel_t *c     = &vChannels[i];
            c->nSync            = CS_UPDATE;
            c->fInGain          = 1.0f;
            c->fOutGain         = 1.0f;
            c->vBands           = new eq_band_t[nBands];
            if (c->vBands == NULL)
                return;

            c->vIn              = NULL;
            c->vOut             = NULL;
            c->vBuffer          = abuf;
            abuf               += EQ_BUFFER_SIZE;
            c->vTrRe            = abuf;
            abuf               += graph_equalizer_base_metadata::MESH_POINTS;
            c->vTrIm            = abuf;
            abuf               += graph_equalizer_base_metadata::MESH_POINTS;

            c->pIn              = NULL;
            c->pOut             = NULL;
            c->pInGain          = NULL;
            c->pTrAmp           = NULL;
            c->pFft             = NULL;
            c->pVisible         = NULL;
            c->pMeter           = NULL;

            // Initialize equalizer
            c->sEqualizer.init(nBands, graph_equalizer_base_metadata::FFT_RANK);

            for (size_t j=0; j<nBands; ++j)
            {
                eq_band_t *b    = &c->vBands[j];

                b->bSolo        = false;
                b->nSync        = CS_UPDATE;
                b->vTrRe        = abuf;
                abuf           += graph_equalizer_base_metadata::MESH_POINTS;
                b->vTrIm        = abuf;
                abuf           += graph_equalizer_base_metadata::MESH_POINTS;

                b->pGain        = NULL;
                b->pSolo        = NULL;
                b->pMute        = NULL;
                b->pEnable      = NULL;
                b->pVisibility  = NULL;
            }
        }

        // Bind ports
        size_t port_id          = 0;

        // Bind audio ports
        lsp_trace("Binding audio ports");
        for (size_t i=0; i<channels; ++i)
        {
            TRACE_PORT(vPorts[port_id]);
            vChannels[i].pIn        =   vPorts[port_id++];
        }
        for (size_t i=0; i<channels; ++i)
        {
            TRACE_PORT(vPorts[port_id]);
            vChannels[i].pOut       =   vPorts[port_id++];
        }

        // Bind common ports
        lsp_trace("Binding common ports");
        TRACE_PORT(vPorts[port_id]);
        pBypass                 = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pInGain                 = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pOutGain                = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pEqMode                 = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pSlope                  = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pFftMode                = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pReactivity             = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pShiftGain              = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pZoom                   = vPorts[port_id++];
        // Skip band select port
        if (nBands > 16)
        {
            TRACE_PORT(vPorts[port_id]);
            port_id++;
        }
        else if ((nMode != EQ_MONO) && (nMode != EQ_STEREO))
        {
            TRACE_PORT(vPorts[port_id]);
            port_id++;
        }

        // Balance
        if (channels > 1)
        {
            TRACE_PORT(vPorts[port_id]);
            pBalance                = vPorts[port_id++];
        }

        // Listen port
        if (nMode == EQ_MID_SIDE)
        {
            TRACE_PORT(vPorts[port_id]);
            pListen                 = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            vChannels[0].pInGain    = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            vChannels[1].pInGain    = vPorts[port_id++];
        }

        for (size_t i=0; i<channels; ++i)
        {
            if ((nMode == EQ_STEREO) && (i > 0))
            {
                vChannels[i].pTrAmp     =   NULL;
            }
            else
            {
                TRACE_PORT(vPorts[port_id]);
                vChannels[i].pTrAmp     =   vPorts[port_id++];
            }
            TRACE_PORT(vPorts[port_id]);
            vChannels[i].pMeter     =   vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            vChannels[i].pFft       =   vPorts[port_id++];
            if (channels > 1)
            {
                TRACE_PORT(vPorts[port_id]);
                vChannels[i].pVisible       =   vPorts[port_id++];
                if ((nMode == EQ_MONO) || (nMode == EQ_STEREO))
                    vChannels[i].pVisible       = NULL;
            }
        }

        // Bind filters
        lsp_trace("Binding filter ports");

        for (size_t i=0; i<nBands; ++i)
        {
            for (size_t j=0; j<channels; ++j)
            {
                eq_band_t *b        = &vChannels[j].vBands[i];

                if ((nMode == EQ_STEREO) && (j > 0))
                {
                    // 1 port controls 2 filters
                    eq_band_t *sb       = &vChannels[0].vBands[i];

                    b->pGain            = sb->pGain;
                    b->pSolo            = sb->pSolo;
                    b->pMute            = sb->pMute;
                    b->pEnable          = sb->pEnable;
                    b->pVisibility      = sb->pVisibility;
                }
                else
                {
                    // 1 port controls 1 band
                    TRACE_PORT(vPorts[port_id]);
                    b->pSolo            = vPorts[port_id++];
                    TRACE_PORT(vPorts[port_id]);
                    b->pMute            = vPorts[port_id++];
                    TRACE_PORT(vPorts[port_id]);
                    b->pEnable          = vPorts[port_id++];
                    TRACE_PORT(vPorts[port_id]);
                    b->pVisibility      = vPorts[port_id++];
                    TRACE_PORT(vPorts[port_id]);
                    b->pGain            = vPorts[port_id++];
                }
            }
        }
    }

    void graph_equalizer_base::destroy()
    {
        size_t channels     = (nMode == EQ_MONO) ? 1 : 2;

        if (vChannels != NULL)
        {
            // Destroy channels
            for (size_t i=0; i<channels; ++i)
            {
                eq_channel_t *c = &vChannels[i];
                c->sEqualizer.destroy();

                if (c->vBands != NULL)
                {
                    delete [] c->vBands;
                    c->vBands   = NULL;
                }
            }

            delete[] vChannels;
            vChannels       = NULL;
        }

        if (vFreqs != NULL)
        {
            delete [] vFreqs;
            vFreqs      = NULL;
        }

        if (pIDisplay != NULL)
        {
            pIDisplay->detroy();
            pIDisplay   = NULL;
        }

        // Destroy analyzer
        sAnalyzer.destroy();
    }

    inline equalizer_mode_t graph_equalizer_base::get_eq_mode()
    {
        switch (size_t(pEqMode->getValue()))
        {
            case para_equalizer_base_metadata::PEM_IIR: return EQM_IIR;
            case para_equalizer_base_metadata::PEM_FIR: return EQM_FIR;
            case para_equalizer_base_metadata::PEM_FFT: return EQM_FFT;
            default:
                break;
        }
        return EQM_BYPASS;
    }

    void graph_equalizer_base::update_settings()
    {
        // Check sample rate
        if (fSampleRate <= 0)
            return;

        // Update common settings
        if (pInGain != NULL)
            fInGain     = pInGain->getValue();
        if (pZoom != NULL)
        {
            float zoom  = pZoom->getValue();
            if (zoom != fZoom)
            {
                fZoom       = zoom;
                pWrapper->query_display_draw();
            }
        }

        // Calculate balance
        float bal[2]    = { 1.0f, 1.0f };
        if (pBalance != NULL)
        {
            float xbal      = pBalance->getValue();
            bal[0]          = (100.0f - xbal) * 0.01f;
            bal[1]          = (xbal + 100.0f) * 0.01f;
        }
        if (pOutGain != NULL)
        {
            float out_gain  = pOutGain->getValue();
            bal[0]         *= out_gain;
            bal[1]         *= out_gain;
        }

        // Listen
        if (pListen != NULL)
            bListen     = pListen->getValue() >= 0.5f;

        size_t channels     = (nMode == EQ_MONO) ? 1 : 2;

        if (pFftMode != NULL)
        {
            fft_position_t pos = fft_position_t(pFftMode->getValue());
            if (pos != nFftPosition)
            {
                nFftPosition    = pos;
                sAnalyzer.reset();
            }
            sAnalyzer.set_activity(nFftPosition != FFTP_NONE);
        }

        // Update reactivity
        sAnalyzer.set_reactivity(pReactivity->getValue());

        // Update shift gain
        if (pShiftGain != NULL)
            sAnalyzer.set_shift(pShiftGain->getValue() * 100.0f);

        // Listen flag
        if (pListen != NULL)
            bListen         = pListen->getValue() >= 0.5f;

        size_t slope                = pSlope->getValue();
        bool bypass                 = pBypass->getValue() >= 0.5f;
        bool solo                   = false;
        bool matched_tr             = bMatched;
        size_t step                 = (nBands > 16) ? 1 : 2;

        bMatched                    = (slope & 1) != 0;
        fInGain                     = pInGain->getValue();
        equalizer_mode_t eq_mode    = get_eq_mode();
        slope                       = graph_equalizer_base_metadata::SLOPE_MIN + (slope >> 1);

        // Update channels
        for (size_t i=0; i<channels; ++i)
        {
            filter_params_t fp;
            eq_channel_t *c     = &vChannels[i];
            bool visible        = (c->pVisible == NULL) ? true : (c->pVisible->getValue() >= 0.5f);

            // Update settings
            c->sEqualizer.set_mode(eq_mode);
            if (c->sBypass.set_bypass(bypass))
                pWrapper->query_display_draw();
            c->fOutGain         = bal[i];
            if (c->pInGain != NULL)
                c->fInGain          = c->pInGain->getValue();

            // Update each band solo
            for (size_t j=0; j<nBands; ++j)
            {
                eq_band_t *b        = &c->vBands[j];
                b->bSolo            = b->pSolo->getValue() >= 0.5f;
                if (b->bSolo)
                    solo                = true;
            }

            // Update each band
            for (size_t j=0; j<nBands; ++j)
            {
                eq_band_t *b        = &c->vBands[j];
                bool enable         = b->pEnable->getValue() >= 0.5f;
                bool mute           = b->pMute->getValue() >= 0.5f;
                float gain          = graph_equalizer_base_metadata::BAND_GAIN_DFL;
                bool b_vis          = visible;

                // Calculate band gain
                if (enable)
                {
                    if (mute)
                    {
                        gain            = graph_equalizer_base_metadata::BAND_GAIN_MIN;
                        b_vis           = false;
                    }
                    else if (solo)
                    {
                        if (b->bSolo)
                            gain            = b->pGain->getValue();
                        else
                        {
                            gain            = graph_equalizer_base_metadata::BAND_GAIN_MIN;
                            b_vis           = false;
                        }
                    }
                    else
                        gain            = b->pGain->getValue();
                }
                else
                {
                    gain            = (solo) ? graph_equalizer_base_metadata::BAND_GAIN_MIN : graph_equalizer_base_metadata::BAND_GAIN_DFL;
                    b_vis           = false;
                }

                // Update visibility
                b->pVisibility->setValue((b_vis) ? 1.0f : 0.0f);

                // Fetch filter params
                c->sEqualizer.get_params(j, &fp);

                bool update         =
                    (fp.fGain != gain) ||
                    (fp.nSlope != slope) ||
                    (bMatched != matched_tr);

                if (update)
                {
                    if (j == 0)
                    {
                        fp.nType        = (bMatched) ? FLT_MT_LRX_LOSHELF : FLT_BT_LRX_LOSHELF;
                        fp.fFreq        = sqrtf(band_frequencies[0] * band_frequencies[step]);
                        fp.fFreq2       = fp.fFreq;
                    }
                    else if (j == (nBands-1))
                    {
                        fp.nType        = (bMatched) ? FLT_MT_LRX_HISHELF : FLT_BT_LRX_HISHELF;
                        fp.fFreq        = sqrtf(band_frequencies[(j-1)*step] * band_frequencies[j*step]);
                        fp.fFreq2       = fp.fFreq;
                    }
                    else
                    {
                        fp.nType        = (bMatched) ? FLT_MT_LRX_LADDERPASS : FLT_BT_LRX_LADDERPASS;
                        fp.fFreq        = sqrtf(band_frequencies[(j-1)*step] * band_frequencies[j*step]);
                        fp.fFreq2       = sqrtf(band_frequencies[j*step] * band_frequencies[(j+1)*step]);
                    }

                    fp.fGain            = gain;
                    fp.nSlope           = slope;
                    fp.fQuality         = 0.0f;

                    c->sEqualizer.set_params(j, &fp);
                    b->nSync           |= CS_UPDATE;
                }
            }
        }

        // Update analyzer
        if (sAnalyzer.needs_reconfiguration())
        {
            sAnalyzer.reconfigure();
            sAnalyzer.get_frequencies(vFreqs, vIndexes, SPEC_FREQ_MIN, SPEC_FREQ_MAX, graph_equalizer_base_metadata::MESH_POINTS);
        }
    }

    void graph_equalizer_base::update_sample_rate(long sr)
    {
        size_t channels     = (nMode == EQ_MONO) ? 1 : 2;

        sAnalyzer.set_sample_rate(sr);

        // Initialize channels
        for (size_t i=0; i<channels; ++i)
        {
            eq_channel_t *c     = &vChannels[i];
            c->sBypass.init(sr);
            c->sEqualizer.set_sample_rate(sr);
        }
    }

    void graph_equalizer_base::ui_activated()
    {
        size_t channels     = ((nMode == EQ_MONO) || (nMode == EQ_STEREO)) ? 1 : 2;
        for (size_t i=0; i<channels; ++i)
            vChannels[i].nSync     = CS_UPDATE;
    }

    void graph_equalizer_base::process(size_t samples)
    {
        size_t channels     = (nMode == EQ_MONO) ? 1 : 2;

        // Initialize buffer pointers
        for (size_t i=0; i<channels; ++i)
        {
            eq_channel_t *c     = &vChannels[i];
            c->vIn              = c->pIn->getBuffer<float>();
            c->vOut             = c->pOut->getBuffer<float>();
        }

        size_t fft_pos          = (ui_active()) ? nFftPosition : FFTP_NONE;

        while (samples > 0)
        {
            // Determine buffer size for processing
            size_t to_process   = (samples > EQ_BUFFER_SIZE) ? EQ_BUFFER_SIZE : samples;

            // Pre-process data
            if (nMode == EQ_MID_SIDE)
            {
                dsp::lr_to_ms(vChannels[0].vBuffer, vChannels[1].vBuffer, vChannels[0].vIn, vChannels[1].vIn, to_process);
                if (fInGain != 1.0f)
                {
                    dsp::scale2(vChannels[0].vBuffer, fInGain, to_process);
                    dsp::scale2(vChannels[1].vBuffer, fInGain, to_process);
                }
            }
            else if (nMode == EQ_MONO)
            {
                if (fInGain != 1.0f)
                    dsp::scale3(vChannels[0].vBuffer, vChannels[0].vIn, fInGain, to_process);
                else
                    dsp::copy(vChannels[0].vBuffer, vChannels[0].vIn, to_process);
            }
            else
            {
                if (fInGain != 1.0f)
                {
                    dsp::scale3(vChannels[0].vBuffer, vChannels[0].vIn, fInGain, to_process);
                    dsp::scale3(vChannels[1].vBuffer, vChannels[1].vIn, fInGain, to_process);
                }
                else
                {
                    dsp::copy(vChannels[0].vBuffer, vChannels[0].vIn, to_process);
                    dsp::copy(vChannels[1].vBuffer, vChannels[1].vIn, to_process);
                }
            }

            // Process each channel individually
            for (size_t i=0; i<channels; ++i)
            {
                eq_channel_t *c     = &vChannels[i];

                // Do FFT in 'PRE'-position
                if (fft_pos == FFTP_PRE)
                    sAnalyzer.process(i, c->vBuffer, to_process);

                // Process the signal by the equalizer
                c->sEqualizer.process(c->vBuffer, c->vBuffer, to_process);
                if (c->fInGain != 1.0f)
                    dsp::scale2(c->vBuffer, c->fInGain, to_process);

                // Do FFT in 'POST'-position
                if (fft_pos == FFTP_POST)
                    sAnalyzer.process(i, c->vBuffer, to_process);
            }

            // Post-process data (if needed)
            if ((nMode == EQ_MID_SIDE) && (!bListen))
                dsp::ms_to_lr(vChannels[0].vBuffer, vChannels[1].vBuffer, vChannels[0].vBuffer, vChannels[1].vBuffer, to_process);

            // Process data via bypass
            for (size_t i=0; i<channels; ++i)
            {
                eq_channel_t *c     = &vChannels[i];

                // Apply output gain
                if (c->fOutGain != 1.0f)
                    dsp::scale2(c->vBuffer, c->fOutGain, to_process);

                // Do metering
                if (c->pMeter != NULL)
                    c->pMeter->setValue(dsp::abs_max(c->vBuffer, to_process));

                // Process via bypass
                c->sBypass.process(c->vOut, c->vIn, c->vBuffer, to_process);

                c->vIn             += to_process;
                c->vOut            += to_process;
            }

            // Update counter
            samples            -= to_process;
        }

        // Output FFT curves for each channel and report latency
        size_t latency          = 0;

        for (size_t i=0; i<channels; ++i)
        {
            eq_channel_t *c     = &vChannels[i];

            // Calculate latency
            if (latency < c->sEqualizer.get_latency())
                latency         = c->sEqualizer.get_latency();

            // Output FFT curve
            mesh_t *mesh            = c->pFft->getBuffer<mesh_t>();
            if ((mesh != NULL) && (mesh->isEmpty()))
            {
                if (nFftPosition != FFTP_NONE)
                {
                    // Copy frequency points
                    dsp::copy(mesh->pvData[0], vFreqs, graph_equalizer_base_metadata::MESH_POINTS);
                    sAnalyzer.get_spectrum(i, mesh->pvData[1], vIndexes, graph_equalizer_base_metadata::MESH_POINTS);

                    // Mark mesh containing data
                    mesh->data(2, graph_equalizer_base_metadata::MESH_POINTS);
                }
                else
                    mesh->data(2, 0);
            }
        }

        // Update latency report
        set_latency(latency);

        // For Mono and Stereo channels only the first channel should be processed
        if (nMode == EQ_STEREO)
            channels        = 1;

        // Sync meshes
        for (size_t i=0; i<channels; ++i)
        {
            eq_channel_t *c     = &vChannels[i];

            // Synchronize bands
            for (size_t j=0; j<nBands; ++j)
            {
                // Update transfer chart of the filter
                eq_band_t *b  = &c->vBands[j];
                if (b->nSync & CS_UPDATE)
                {
                    c->sEqualizer.freq_chart(j, b->vTrRe, b->vTrIm, vFreqs, graph_equalizer_base_metadata::MESH_POINTS);
                    b->nSync    = 0;
                    c->nSync    = CS_UPDATE;
                }
            }

            // Synchronize main transfer function of the channel
            if (c->nSync & CS_UPDATE)
            {
                // Initialize complex numbers for transfer function
                dsp::fill_one(c->vTrRe, para_equalizer_base_metadata::MESH_POINTS);
                dsp::fill_zero(c->vTrIm, para_equalizer_base_metadata::MESH_POINTS);

                for (size_t j=0; j<nBands; ++j)
                {
                    eq_band_t *b  = &c->vBands[j];
                    dsp::complex_mul3(c->vTrRe, c->vTrIm, c->vTrRe, c->vTrIm, b->vTrRe, b->vTrIm, para_equalizer_base_metadata::MESH_POINTS);
                }
                c->nSync    = CS_SYNC_AMP;
            }

            // Output amplification curve
            if ((c->pTrAmp != NULL) && (c->nSync & CS_SYNC_AMP))
            {
                // Sync mesh
                mesh_t *mesh        = c->pTrAmp->getBuffer<mesh_t>();
                if ((mesh != NULL) && (mesh->isEmpty()))
                {
                    // Add extra points
                    mesh->pvData[0][0] = SPEC_FREQ_MIN*0.5f;
                    mesh->pvData[0][graph_equalizer_base_metadata::MESH_POINTS+1] = SPEC_FREQ_MAX*2.0;
                    mesh->pvData[1][0] = 1.0f;
                    mesh->pvData[1][graph_equalizer_base_metadata::MESH_POINTS+1] = 1.0f;

                    // Copy data
                    dsp::copy(&mesh->pvData[0][1], vFreqs, graph_equalizer_base_metadata::MESH_POINTS);
                    dsp::complex_mod(&mesh->pvData[1][1], c->vTrRe, c->vTrIm, graph_equalizer_base_metadata::MESH_POINTS);
                    mesh->data(2, graph_equalizer_base_metadata::FILTER_MESH_POINTS);

                    c->nSync           &= ~CS_SYNC_AMP;
                }

                // Request for redraw
                if (pWrapper != NULL)
                    pWrapper->query_display_draw();
            }
        }
    }

    bool graph_equalizer_base::inline_display(ICanvas *cv, size_t width, size_t height)
    {
        // Check proportions
        if (height > (R_GOLDEN_RATIO * width))
            height  = R_GOLDEN_RATIO * width;

        // Init canvas
        if (!cv->init(width, height))
            return false;
        width   = cv->width();
        height  = cv->height();

        // Clear background
        bool bypassing = vChannels[0].sBypass.bypassing();
        cv->set_color_rgb((bypassing) ? CV_DISABLED : CV_BACKGROUND);
        cv->paint();

        // Draw axis
        cv->set_line_width(1.0);

        float zx    = 1.0f/SPEC_FREQ_MIN;
        float zy    = fZoom/GAIN_AMP_M_48_DB;
        float dx    = width/(logf(SPEC_FREQ_MAX)-logf(SPEC_FREQ_MIN));
        float dy    = height/(logf(GAIN_AMP_M_48_DB/fZoom)-logf(GAIN_AMP_P_48_DB*fZoom));

        // Draw vertical lines
        cv->set_color_rgb(CV_YELLOW, 0.5f);
        for (float i=100.0f; i<SPEC_FREQ_MAX; i *= 10.0f)
        {
            float ax = dx*(logf(i*zx));
            cv->line(ax, 0, ax, height);
        }

        // Draw horizontal lines
        cv->set_color_rgb(CV_WHITE, 0.5f);
        for (float i=GAIN_AMP_M_48_DB; i<GAIN_AMP_P_48_DB; i *= GAIN_AMP_P_12_DB)
        {
            float ay = height + dy*(logf(i*zy));
            cv->line(0, ay, width, ay);
        }

        // Allocate buffer: f, x, y, re, im
        pIDisplay           = float_buffer_t::reuse(pIDisplay, 5, width+2);
        float_buffer_t *b   = pIDisplay;
        if (b == NULL)
            return false;

        // Initialize mesh
        b->v[0][0]          = SPEC_FREQ_MIN*0.5f;
        b->v[0][width+1]    = SPEC_FREQ_MAX*2.0f;
        b->v[3][0]          = 1.0f;
        b->v[3][width+1]    = 1.0f;
        b->v[4][0]          = 0.0f;
        b->v[4][width+1]    = 0.0f;

        size_t channels = ((nMode == EQ_MONO) || (nMode == EQ_STEREO)) ? 1 : 2;
        static uint32_t c_colors[] = {
                CV_MIDDLE_CHANNEL, CV_MIDDLE_CHANNEL,
                CV_MIDDLE_CHANNEL, CV_MIDDLE_CHANNEL,
                CV_LEFT_CHANNEL, CV_RIGHT_CHANNEL,
                CV_MIDDLE_CHANNEL, CV_SIDE_CHANNEL
               };

        bool aa = cv->set_anti_aliasing(true);
        cv->set_line_width(2);

        for (size_t i=0; i<channels; ++i)
        {
            eq_channel_t *c = &vChannels[i];

            for (size_t j=0; j<width; ++j)
            {
                size_t k        = (j*graph_equalizer_base_metadata::MESH_POINTS)/width;
                b->v[0][j+1]    = vFreqs[k];
                b->v[3][j+1]    = c->vTrRe[k];
                b->v[4][j+1]    = c->vTrIm[k];
            }

            dsp::complex_mod(b->v[3], b->v[3], b->v[4], width+2);
            dsp::fill(b->v[1], 0.0f, width+2);
            dsp::fill(b->v[2], height, width+2);
            dsp::axis_apply_log1(b->v[1], b->v[0], zx, dx, width+2);
            dsp::axis_apply_log1(b->v[2], b->v[3], zy, dy, width+2);

            // Draw mesh
            uint32_t color = (bypassing || !(active())) ? CV_SILVER : c_colors[nMode*2 + i];
            Color stroke(color), fill(color, 0.5f);
            cv->draw_poly(b->v[1], b->v[2], width+2, stroke, fill);
        }
        cv->set_anti_aliasing(aa);

        return true;
    }

    //-------------------------------------------------------------------------
    graph_equalizer_x16_mono::graph_equalizer_x16_mono() : graph_equalizer_base(metadata, 16, EQ_MONO)
    {
    }

    graph_equalizer_x32_mono::graph_equalizer_x32_mono() : graph_equalizer_base(metadata, 32, EQ_MONO)
    {
    }

    graph_equalizer_x16_stereo::graph_equalizer_x16_stereo() : graph_equalizer_base(metadata, 16, EQ_STEREO)
    {
    }

    graph_equalizer_x32_stereo::graph_equalizer_x32_stereo() : graph_equalizer_base(metadata, 32, EQ_STEREO)
    {
    }

    graph_equalizer_x16_lr::graph_equalizer_x16_lr() : graph_equalizer_base(metadata, 16, EQ_LEFT_RIGHT)
    {
    }

    graph_equalizer_x32_lr::graph_equalizer_x32_lr() : graph_equalizer_base(metadata, 32, EQ_LEFT_RIGHT)
    {
    }

    graph_equalizer_x16_ms::graph_equalizer_x16_ms() : graph_equalizer_base(metadata, 16, EQ_MID_SIDE)
    {
    }

    graph_equalizer_x32_ms::graph_equalizer_x32_ms() : graph_equalizer_base(metadata, 32, EQ_MID_SIDE)
    {
    }
} /* namespace lsp */
