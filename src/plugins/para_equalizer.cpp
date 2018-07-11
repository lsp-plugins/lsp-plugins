/*
 * equalizer.cpp
 *
 *  Created on: 30 июня 2016 г.
 *      Author: sadko
 */

#include <core/types.h>
#include <core/debug.h>

#include <plugins/para_equalizer.h>

namespace lsp
{
    #define EQ_BUFFER_SIZE          0x1000
    #define EQ_RANK                 12
    #define TRACE_PORT(p)           lsp_trace("  port id=%s", (p)->metadata()->id);

    //-------------------------------------------------------------------------
    // para_equalizer core
    
    para_equalizer_base::para_equalizer_base(const plugin_metadata_t &metadata, size_t filters, size_t mode): plugin_t(metadata)
    {
        nFilters    = filters;
        nMode       = mode;
        nEqMode     = EQM_BYPASS;
        vChannels   = NULL;
        vFreqs      = NULL;
        fGainIn     = 1.0f;
        fGainOut    = 1.0f;
        bListen     = false;
        nFftPeriod  = 0;
        fTau        = 1.0f;
        nFftPosition= FFTP_NONE;
        fShiftGain  = 1.0f;

        vSigRe      = NULL;
        vSigIm      = NULL;
        vFftRe      = NULL;
        vFftIm      = NULL;
        vFftWindow  = NULL;
        vFftEnvelope= NULL;
        vIndexes    = NULL;

        pBypass     = NULL;
        pGainIn     = NULL;
        pGainOut    = NULL;
        pFftMode    = NULL;
        pReactivity = NULL;
        pListen     = NULL;
        pShiftGain  = NULL;
        pEqMode     = NULL;
    }
    
    para_equalizer_base::~para_equalizer_base()
    {
        destroy_state();
    }

    inline void para_equalizer_base::decode_filter(size_t *ftype, size_t *slope)
    {
        #define EQF(x, y, ks) \
            case para_equalizer_base_metadata::EQF_ ## x: \
                *ftype = y; \
                *slope = ks * *slope; \
                return;

        switch (*ftype)
        {
            EQF(NONE, FLT_NONE, 1)

            EQF(BT_RLC_LOPASS, FLT_BT_RLC_LOPASS, 2)
            EQF(MT_RLC_LOPASS, FLT_MT_RLC_LOPASS, 2)

            EQF(BT_BWC_LOPASS, FLT_BT_BWC_LOPASS, 2)
            EQF(MT_BWC_LOPASS, FLT_MT_BWC_LOPASS, 2)

            EQF(BT_LRX_LOPASS, FLT_BT_LRX_LOPASS, 1)
            EQF(MT_LRX_LOPASS, FLT_MT_LRX_LOPASS, 1)

            EQF(BT_RLC_HIPASS, FLT_BT_RLC_HIPASS, 2)
            EQF(MT_RLC_HIPASS, FLT_MT_RLC_HIPASS, 2)

            EQF(BT_BWC_HIPASS, FLT_BT_BWC_HIPASS, 2)
            EQF(MT_BWC_HIPASS, FLT_MT_BWC_HIPASS, 2)

            EQF(BT_LRX_HIPASS, FLT_BT_LRX_HIPASS, 1)
            EQF(MT_LRX_HIPASS, FLT_MT_LRX_HIPASS, 1)

            EQF(BT_RLC_LOSHELF, FLT_BT_RLC_LOSHELF, 1)
            EQF(MT_RLC_LOSHELF, FLT_MT_RLC_LOSHELF, 1)

            EQF(BT_BWC_LOSHELF, FLT_BT_BWC_LOSHELF, 1)
            EQF(MT_BWC_LOSHELF, FLT_MT_BWC_LOSHELF, 1)

            EQF(BT_LRX_LOSHELF, FLT_BT_LRX_LOSHELF, 1)
            EQF(MT_LRX_LOSHELF, FLT_MT_LRX_LOSHELF, 1)

            EQF(BT_RLC_HISHELF, FLT_BT_RLC_HISHELF, 1)
            EQF(MT_RLC_HISHELF, FLT_MT_RLC_HISHELF, 1)

            EQF(BT_BWC_HISHELF, FLT_BT_BWC_HISHELF, 1)
            EQF(MT_BWC_HISHELF, FLT_MT_BWC_HISHELF, 1)

            EQF(BT_LRX_HISHELF, FLT_BT_LRX_HISHELF, 1)
            EQF(MT_LRX_HISHELF, FLT_MT_LRX_HISHELF, 1)

            EQF(BT_RLC_BELL, FLT_BT_RLC_BELL, 1)
            EQF(MT_RLC_BELL, FLT_MT_RLC_BELL, 1)

            EQF(BT_BWC_BELL, FLT_BT_BWC_BELL, 1)
            EQF(MT_BWC_BELL, FLT_MT_BWC_BELL, 1)

            EQF(BT_LRX_BELL, FLT_BT_LRX_BELL, 1)
            EQF(MT_LRX_BELL, FLT_MT_LRX_BELL, 1)

            EQF(BT_RESONANCE, FLT_BT_RLC_RESONANCE, 1)
            EQF(MT_RESONANCE, FLT_MT_RLC_RESONANCE, 1)

            EQF(BT_NOTCH, FLT_BT_RLC_NOTCH, 1)
            EQF(MT_NOTCH, FLT_MT_RLC_NOTCH, 1)

            default:
                *ftype = FLT_NONE;
                *slope = 1;
                return;
        }
        #undef EQF
    }

    inline bool para_equalizer_base::adjust_gain(size_t filter_type)
    {
        switch (filter_type)
        {
            case FLT_BT_RLC_LOPASS:
            case FLT_MT_RLC_LOPASS:
            case FLT_BT_RLC_HIPASS:
            case FLT_MT_RLC_HIPASS:
            case FLT_BT_RLC_NOTCH:
            case FLT_MT_RLC_NOTCH:
            case FLT_BT_BWC_LOPASS:
            case FLT_MT_BWC_LOPASS:
            case FLT_BT_BWC_HIPASS:
            case FLT_MT_BWC_HIPASS:
            case FLT_BT_LRX_LOPASS:
            case FLT_MT_LRX_LOPASS:
            case FLT_BT_LRX_HIPASS:
            case FLT_MT_LRX_HIPASS:
                return false;
            default:
                break;
        }
        return true;
    }

    inline equalizer_mode_t para_equalizer_base::get_eq_mode()
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

    void para_equalizer_base::init(IWrapper *wrapper)
    {
        // Determine number of channels
        size_t channels     = (nMode == EQ_MONO) ? 1 : 2;
        vChannels           = new eq_channel_t[channels];
        if (vChannels == NULL)
            return;

        // Initialize global parameters
        fGainIn             = 1.0f;
        fGainOut            = 1.0f;
        bListen             = false;
        nFftPosition        = FFTP_NONE;

        // Allocate indexes
        vIndexes            = new uint32_t[para_equalizer_base_metadata::MESH_POINTS];
        if (vIndexes == NULL)
            return;

        // Calculate amount of bulk data to allocate
        size_t allocate     = (3 * para_equalizer_base_metadata::MESH_POINTS * (nFilters + 1) + EQ_BUFFER_SIZE) * channels + para_equalizer_base_metadata::MESH_POINTS;
        allocate           += (para_equalizer_base_metadata::FFT_ITEMS * channels * 2) + para_equalizer_base_metadata::FFT_ITEMS * 6;
        float *abuf         = new float[allocate];
        if (abuf == NULL)
            return;

        // Clear all floating-point buffers
        dsp::fill_zero(abuf, allocate);

        // Frequency list buffer
        vFreqs              = abuf;
        abuf               += para_equalizer_base_metadata::MESH_POINTS;

        // FFT buffers
        vSigRe              = abuf;
        abuf               += para_equalizer_base_metadata::FFT_ITEMS;
        vSigIm              = abuf;
        abuf               += para_equalizer_base_metadata::FFT_ITEMS;
        vFftRe              = abuf;
        abuf               += para_equalizer_base_metadata::FFT_ITEMS;
        vFftIm              = abuf;
        abuf               += para_equalizer_base_metadata::FFT_ITEMS;
        vFftWindow          = abuf;
        abuf               += para_equalizer_base_metadata::FFT_ITEMS;
        vFftEnvelope        = abuf;
        abuf               += para_equalizer_base_metadata::FFT_ITEMS;

        // Initialize each channel
        for (size_t i=0; i<channels; ++i)
        {
            eq_channel_t *c     = &vChannels[i];

            c->nLatency         = 0;
            c->vFilters         = NULL;
            c->vBuffer          = abuf;
            abuf               += EQ_BUFFER_SIZE;
            c->vTrRe            = abuf;
            abuf               += para_equalizer_base_metadata::MESH_POINTS;
            c->vTrIm            = abuf;
            abuf               += para_equalizer_base_metadata::MESH_POINTS;
            c->vTrAmp           = abuf;
            abuf               += para_equalizer_base_metadata::MESH_POINTS;

            // FFT buffers
            c->vFftBuffer       = abuf;
            abuf               += para_equalizer_base_metadata::FFT_ITEMS;
            c->vFftAmp          = abuf;
            abuf               += para_equalizer_base_metadata::FFT_ITEMS;

            // Input and output ports
            c->vIn              = NULL;
            c->vOut             = NULL;

            // Ports
            c->pIn              = NULL;
            c->pOut             = NULL;
            c->pTrAmp           = NULL;
            c->pFft             = NULL;
        }

        // Allocate data
        for (size_t i=0; i<channels; ++i)
        {
            // Allocate data
            eq_channel_t *c     = &vChannels[i];
            c->nSync            = CS_UPDATE;
            c->vFilters         = new eq_filter_t[nFilters];
            c->nFftCounter      = 0;
            if (c->vFilters == NULL)
                return;

            c->sEqualizer.init(nFilters, EQ_RANK);

            // Initialize filters
            for (size_t j=0; j<nFilters; ++j)
            {
                eq_filter_t *f      = &c->vFilters[j];

                // Filter characteristics
                f->vTrRe            = abuf;
                abuf               += para_equalizer_base_metadata::MESH_POINTS;
                f->vTrIm            = abuf;
                abuf               += para_equalizer_base_metadata::MESH_POINTS;
                f->vTrAmp           = abuf;
                abuf               += para_equalizer_base_metadata::MESH_POINTS;
                f->nSync            = CS_UPDATE;

                // Additional parameters
                f->pType            = NULL;
                f->pFreq            = NULL;
                f->pGain            = NULL;
                f->pQuality         = NULL;
                f->pActivity        = NULL;
                f->pTrAmp           = NULL;
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
        pGainIn                 = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pGainOut                = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pEqMode                = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pFftMode                = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pReactivity             = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pShiftGain              = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        port_id++; // Skip filter selector
        if (nMode == EQ_MID_SIDE)
        {
            TRACE_PORT(vPorts[port_id]);
            pListen                 = vPorts[port_id++];
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
            vChannels[i].pFft       =   vPorts[port_id++];
            if (channels > 1)
            {
                TRACE_PORT(vPorts[port_id]);
                vChannels[i].pVisible       =   vPorts[port_id++];
            }
        }

        // Bind filters
        lsp_trace("Binding filter ports");

        for (size_t i=0; i<nFilters; ++i)
        {
            for (size_t j=0; j<channels; ++j)
            {
                eq_filter_t *f      = &vChannels[j].vFilters[i];

                if ((nMode == EQ_STEREO) && (j > 0))
                {
                    // 1 port controls 2 filters
                    eq_filter_t *sf     = &vChannels[0].vFilters[i];
                    f->pType            = sf->pType;
                    f->pSlope           = sf->pSlope;
                    f->pSolo            = sf->pSolo;
                    f->pMute            = sf->pMute;
                    f->pFreq            = sf->pFreq;
                    f->pGain            = sf->pGain;
                    f->pQuality         = sf->pQuality;
                    f->pActivity        = sf->pActivity;
                    f->pTrAmp           = NULL;
                }
                else
                {
                    // 1 port controls 1 filter
                    TRACE_PORT(vPorts[port_id]);
                    f->pType        = vPorts[port_id++];
                    TRACE_PORT(vPorts[port_id]);
                    f->pSlope       = vPorts[port_id++];
                    TRACE_PORT(vPorts[port_id]);
                    f->pSolo        = vPorts[port_id++];
                    TRACE_PORT(vPorts[port_id]);
                    f->pMute        = vPorts[port_id++];
                    TRACE_PORT(vPorts[port_id]);
                    f->pFreq        = vPorts[port_id++];
                    TRACE_PORT(vPorts[port_id]);
                    f->pGain        = vPorts[port_id++];
                    TRACE_PORT(vPorts[port_id]);
                    f->pQuality     = vPorts[port_id++];
                    TRACE_PORT(vPorts[port_id]);
                    port_id++; // Skip hue
                    TRACE_PORT(vPorts[port_id]);
                    f->pActivity    = vPorts[port_id++];
                    TRACE_PORT(vPorts[port_id]);
                    f->pTrAmp       = vPorts[port_id++];
                }
            }
        }

        init_envelope();
        init_window();

        // Force settings to be update
        update_settings();
    }

    void para_equalizer_base::ui_activated()
    {
        size_t channels     = ((nMode == EQ_MONO) || (nMode == EQ_STEREO)) ? 1 : 2;
        for (size_t i=0; i<channels; ++i)
            for (size_t j=0; j<nFilters; ++j)
                vChannels[i].vFilters[j].nSync = CS_UPDATE;
    }

    void para_equalizer_base::destroy()
    {
        destroy_state();
    }

    void para_equalizer_base::init_window()
    {
        // Initialize window
        windows::window(vFftWindow, para_equalizer_base_metadata::FFT_ITEMS, windows::window_t(para_equalizer_base_metadata::FFT_WINDOW));
    }

    void para_equalizer_base::init_envelope()
    {
        // Initialize envelope
        envelope::reverse_noise(vFftEnvelope, para_equalizer_base_metadata::FFT_ITEMS, envelope::envelope_t(para_equalizer_base_metadata::FFT_ENVELOPE));

        // Adjust envelope
        dsp::scale(vFftEnvelope, vFftEnvelope, 100.0f * fShiftGain / para_equalizer_base_metadata::FFT_ITEMS, para_equalizer_base_metadata::FFT_ITEMS); // Add extra + 20dB to the signal
    }

    void para_equalizer_base::destroy_state()
    {
        size_t channels     = (nMode == EQ_MONO) ? 1 : 2;

        // Delete channels
        if (vChannels != NULL)
        {
            for (size_t i=0; i<channels; ++i)
            {
                eq_channel_t *c     = &vChannels[i];
                if (c->vFilters != NULL)
                {
                    delete [] c->vFilters;
                    c->vFilters         = NULL;
                }
            }

            delete [] vChannels;
            vChannels = NULL;
        }

        if (vIndexes != NULL)
        {
            delete [] vIndexes;
            vIndexes    = NULL;
        }

        // Delete frequencies
        if (vFreqs != NULL)
        {
            delete [] vFreqs;
            vFreqs = NULL;
        }
    }

    void para_equalizer_base::update_settings()
    {
        // Check sample rate
        if (fSampleRate <= 0)
            return;

        // Update common settings
        if (pGainIn != NULL)
            fGainIn     = pGainIn->getValue();
        if (pGainOut != NULL)
            fGainOut    = pGainOut->getValue();
        if (pListen != NULL)
            bListen     = pListen->getValue() >= 0.5f;

        size_t channels     = (nMode == EQ_MONO) ? 1 : 2;
        size_t latency      = 0;

        if (pFftMode != NULL)
        {
            fft_position_t pos = fft_position_t(pFftMode->getValue());
            if (pos != nFftPosition)
            {
                nFftPosition    = pos;
                for (size_t i=0; i<channels; ++i)
                    dsp::fill_zero(vChannels[i].vFftAmp, para_equalizer_base_metadata::FFT_ITEMS);
            }
        }

        // Update reactivity
        float reactivity            = pReactivity->getValue();
        fTau                        = 1.0f - expf(logf(1.0f - M_SQRT1_2) / seconds_to_samples(fSampleRate / nFftPeriod, reactivity));

        // Update shift gain
        if (pShiftGain != NULL)
        {
            float gain                  = pShiftGain->getValue();
            if (gain != fShiftGain)
            {
                fShiftGain              = gain;
                init_envelope();
            }
        }

        // Update equalizer mode
        nEqMode         = get_eq_mode();
        bool bypass     = pBypass->getValue() >= 0.5f;

        // For each channel
        for (size_t i=0; i<channels; ++i)
        {
            eq_channel_t *c     = &vChannels[i];
            bool solo           = false;
            bool modified       = nEqMode != c->sEqualizer.get_mode();
            bool visible        = ((nMode == EQ_MONO) || (nMode == EQ_STEREO) || (c->pVisible == NULL)) ?
                                    true : (c->pVisible->getValue() >= 0.5f);

            c->sBypass.set_bypass(bypass);

            // Update each filter configuration (step 1)
            for (size_t j=0; j<nFilters; ++j)
            {
                eq_filter_t *f      = &c->vFilters[j];
                f->bSolo            = f->pSolo->getValue() >= 0.5f;
                if (f->bSolo)
                    solo                = true;
            }

            // Update each filter configuration (step 2, depending on solo)
            for (size_t j=0; j<nFilters; ++j)
            {
                filter_params_t fp;
                eq_filter_t *f      = &c->vFilters[j];

                // Check if need to update parameters
                size_t ft   = FLT_NONE, slope = 1;
                bool mute   = f->pMute->getValue() >= 0.5f;
                if ((!mute) && ((solo) ^ (!f->bSolo)))
                {
                    ft          = f->pType->getValue();
                    slope       = f->pSlope->getValue() + 1;
                    decode_filter(&ft, &slope);
                }

                // Fetch filter params
                c->sEqualizer.get_params(j, &fp);
                bool update         =
                        (fp.nType != ft) ||
                        (fp.fFreq != f->pFreq->getValue()) ||
                        (fp.fGain != f->pGain->getValue()) ||
                        (fp.nSlope != slope) ||
                        (fp.fQuality != f->pQuality->getValue());

                // Update filter parameters
                if (update)
                {
                    modified            = true;
                    fp.nType            = ft;
                    fp.fFreq            = f->pFreq->getValue();
                    fp.fFreq2           = 2.0f * fp.fFreq;
                    fp.fGain            = (adjust_gain(ft)) ? f->pGain->getValue() : 1.0f;
                    fp.nSlope           = slope;
                    fp.fQuality         = f->pQuality->getValue();

                    c->sEqualizer.set_params(j, &fp);
                    f->nSync            = CS_UPDATE;
                }

                // Output filter activity
                if (f->pActivity != NULL)
                    f->pActivity->setValue(((visible) && (ft != FLT_NONE)) ? 1.0f : 0.0f);
            }

            // Update equalizer mode and get latency
            if (modified)
            {
                c->sEqualizer.set_mode(nEqMode);
                c->sEqualizer.reconfigure();
            }
            if (latency < c->sEqualizer.get_latency())
                latency         = c->sEqualizer.get_latency();
        }

        // Update para_equalizer latency
        set_latency(latency);
    }

    void para_equalizer_base::update_sample_rate(long sr)
    {
        size_t channels     = (nMode == EQ_MONO) ? 1 : 2;

        // Calculate FFT period
        nFftPeriod          = float(fSampleRate) / float(para_equalizer_base_metadata::REFRESH_RATE);

        for (size_t i=0; i<channels; ++i)
        {
            eq_channel_t *c     = &vChannels[i];
            c->sBypass.init(sr);
            c->sEqualizer.set_sample_rate(sr);

            c->nFftCounter = ((i * nFftPeriod) / channels);
        }

        update_frequencies();
        update_settings();
    }

    void para_equalizer_base::update_frequencies()
    {
        size_t fft_csize    = (para_equalizer_base_metadata::FFT_ITEMS >> 1) + 1;
        float scale         = float(para_equalizer_base_metadata::FFT_ITEMS) / float(fSampleRate);

        // Initialize list of frequencies
        float step = 1.0f / (para_equalizer_base_metadata::MESH_POINTS - 1);
        float norm = logf(para_equalizer_base_metadata::FREQ_MAX/para_equalizer_base_metadata::FREQ_MIN);

        for (size_t i=0; i<para_equalizer_base_metadata::MESH_POINTS; ++i)
        {
            float f         = para_equalizer_base_metadata::FREQ_MIN * expf(i * step * norm);
            size_t idx      = scale * f;
            if (idx > fft_csize)
                idx                 = fft_csize + 1;

            vFreqs[i]       = f;
            vIndexes[i]     = idx;
        }
    }

    void para_equalizer_base::do_fft(eq_channel_t *channel, size_t samples)
    {
        const float *src    = channel->vBuffer;
        size_t fft_csize    = (para_equalizer_base_metadata::FFT_ITEMS >> 1) + 1;

        while (samples > 0)
        {
            // Determine the amount of samples to process
            ssize_t to_process          = nFftPeriod - channel->nFftCounter;

            if (to_process <= 0)
            {
                if (ui_active())
                {
                    // Apply window to the temporary buffer
                    dsp::multiply(vSigRe, channel->vFftBuffer, vFftWindow, para_equalizer_base_metadata::FFT_ITEMS);
                    // Do FFT
                    dsp::direct_fft(vFftRe, vFftIm, vSigRe, vSigIm, para_equalizer_base_metadata::FFT_RANK);
                    // Leave only positive frequencies
                    dsp::combine_fft(vFftRe, vFftIm, vFftRe, vFftIm, para_equalizer_base_metadata::FFT_RANK);
                    // Get complex argument
                    dsp::complex_mod(vFftRe, vFftRe, vFftIm, fft_csize);
                    // Mix with the previous value
                    dsp::mix(channel->vFftAmp, channel->vFftAmp, vFftRe, 1.0 - fTau, fTau, fft_csize);
                }
                else
                {
                    // Mix with zero values (vSigIm is zero)
                    dsp::mix(channel->vFftAmp, channel->vFftAmp, vSigIm, 1.0 - fTau, fTau, fft_csize);
                }

                // Decrement FFT counter
                channel->nFftCounter       -= nFftPeriod;
            }
            else
            {
                // Limit number of samples to be processed
                if (to_process > ssize_t(samples))
                    to_process      = samples;

                // Move data in the buffer
                dsp::move(channel->vFftBuffer, &channel->vFftBuffer[to_process], para_equalizer_base_metadata::FFT_ITEMS - to_process);
                dsp::copy(&channel->vFftBuffer[para_equalizer_base_metadata::FFT_ITEMS - to_process], src, to_process);

                // Update counters
                channel->nFftCounter       += to_process;
                src                        += to_process;
                samples                    -= to_process;
            }
        }
    }

    void para_equalizer_base::process(size_t samples)
    {
        size_t channels     = (nMode == EQ_MONO) ? 1 : 2;

        // Initialize buffer pointers
        for (size_t i=0; i<channels; ++i)
        {
            eq_channel_t *c     = &vChannels[i];

            c->vIn              = c->pIn->getBuffer<float>();
            c->vOut             = c->pOut->getBuffer<float>();
        }

        while (samples > 0)
        {
            // Determine buffer size for processing
            size_t to_process   = (samples > EQ_BUFFER_SIZE) ? EQ_BUFFER_SIZE : samples;

            // Pre-process data
            if (nMode == EQ_MID_SIDE)
            {
                dsp::lr_to_ms(vChannels[0].vBuffer, vChannels[1].vBuffer, vChannels[0].vIn, vChannels[1].vIn, to_process);
                if (fGainIn != 1.0f)
                {
                    dsp::scale(vChannels[0].vBuffer, vChannels[0].vBuffer, fGainIn, to_process);
                    dsp::scale(vChannels[1].vBuffer, vChannels[1].vBuffer, fGainIn, to_process);
                }
            }
            else if (nMode == EQ_MONO)
            {
                if (fGainIn != 1.0f)
                    dsp::scale(vChannels[0].vBuffer, vChannels[0].vIn, fGainIn, to_process);
                else
                    dsp::copy(vChannels[0].vBuffer, vChannels[0].vIn, to_process);
            }
            else
            {
                if (fGainIn != 1.0f)
                {
                    dsp::scale(vChannels[0].vBuffer, vChannels[0].vIn, fGainIn, to_process);
                    dsp::scale(vChannels[1].vBuffer, vChannels[1].vIn, fGainIn, to_process);
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
                if (nFftPosition == FFTP_PRE)
                    do_fft(c, to_process);

                // Process the signal by the equalizer
                c->sEqualizer.process(c->vBuffer, c->vBuffer, to_process);

                // Apply output gain
                if (fGainOut != 1.0f)
                    dsp::scale(c->vBuffer, c->vBuffer, fGainOut, to_process);

                // Do FFT in 'POST'-position
                if (nFftPosition == FFTP_POST)
                    do_fft(c, to_process);
            }

            // Post-process data (if needed)
            if ((nMode == EQ_MID_SIDE) && (!bListen))
                dsp::ms_to_lr(vChannels[0].vBuffer, vChannels[1].vBuffer, vChannels[0].vBuffer, vChannels[1].vBuffer, to_process);

            // Process data via bypass
            for (size_t i=0; i<channels; ++i)
            {
                eq_channel_t *c     = &vChannels[i];
                c->sBypass.process(c->vOut, c->vIn, c->vBuffer, to_process);
            }

            // Update counters and pointers
            samples            -= to_process;
            for (size_t i=0; i<channels; ++i)
            {
                vChannels[i].vIn       += to_process;
                vChannels[i].vOut      += to_process;
            }
        }

        // Output FFT curves for each channel
        for (size_t i=0; i<channels; ++i)
        {
            eq_channel_t *c     = &vChannels[i];

            // Output FFT curve
            mesh_t *mesh            = c->pFft->getBuffer<mesh_t>();
            if ((mesh != NULL) && (mesh->isEmpty()))
            {
                if (nFftPosition != FFTP_NONE)
                {
                    // Copy frequency points
                    dsp::copy(mesh->pvData[0], vFreqs, para_equalizer_base_metadata::MESH_POINTS);

                    float *v        = mesh->pvData[1];
                    uint32_t *idx   = vIndexes;
                    for (size_t i=0; i<para_equalizer_base_metadata::MESH_POINTS; ++i)
                    {
                        size_t j        = idx[i];
                        *(v++)          = c->vFftAmp[j] * vFftEnvelope[j];
                    }

                    // Mark mesh containing data
                    mesh->data(2, para_equalizer_base_metadata::MESH_POINTS);
                }
                else
                    mesh->data(2, 0);
            }
        }

        // For Mono and Stereo channels only the first channel should be processed
        if (nMode == EQ_STEREO)
            channels        = 1;

        // Sync meshes
        for (size_t i=0; i<channels; ++i)
        {
            eq_channel_t *c     = &vChannels[i];

            // Synchronize filters
            for (size_t j=0; j<nFilters; ++j)
            {
                // Update transfer chart of the filter
                eq_filter_t *f  = &c->vFilters[j];
                if (f->nSync & CS_UPDATE)
                {
                    c->sEqualizer.freq_chart(j, f->vTrRe, f->vTrIm, vFreqs, para_equalizer_base_metadata::MESH_POINTS);
                    f->nSync    = CS_SYNC_AMP | CS_SYNC_PHASE;
                    c->nSync    = CS_UPDATE;
                }

                // Output amplification curve
                if ((f->pTrAmp != NULL) && (f->nSync & CS_SYNC_AMP))
                {
                    mesh_t *mesh        = f->pTrAmp->getBuffer<mesh_t>();
                    if ((mesh != NULL) && (mesh->isEmpty()))
                    {
                        if (c->sEqualizer.filter_active(j))
                        {
                            // Add extra points
                            mesh->pvData[0][0] = para_equalizer_base_metadata::FREQ_MIN*0.5f;
                            mesh->pvData[0][para_equalizer_base_metadata::MESH_POINTS+1] = para_equalizer_base_metadata::FREQ_MAX*1.5f;
                            mesh->pvData[1][0] = 1.0f;
                            mesh->pvData[1][para_equalizer_base_metadata::MESH_POINTS+1] = 1.0f;

                            // Fill mesh
                            dsp::copy(&mesh->pvData[0][1], vFreqs, para_equalizer_base_metadata::MESH_POINTS);
                            dsp::complex_mod(&mesh->pvData[1][1], f->vTrRe, f->vTrIm, para_equalizer_base_metadata::MESH_POINTS);
                            mesh->data(2, para_equalizer_base_metadata::FILTER_MESH_POINTS);
                        }
                        else
                            mesh->data(2, 0);

                        f->nSync           &= ~CS_SYNC_AMP;
                    }
                }
            }

            // Synchronize main transfer function of the channel
            if (c->nSync & CS_UPDATE)
            {
                // Initialize complex numbers for transfer function
                dsp::fill_one(c->vTrRe, para_equalizer_base_metadata::MESH_POINTS);
                dsp::fill_zero(c->vTrIm, para_equalizer_base_metadata::MESH_POINTS);

                for (size_t j=0; j<nFilters; ++j)
                {
                    eq_filter_t *f  = &c->vFilters[j];
                    dsp::complex_mul(c->vTrRe, c->vTrIm, c->vTrRe, c->vTrIm, f->vTrRe, f->vTrIm, para_equalizer_base_metadata::MESH_POINTS);
                }
                c->nSync    = CS_SYNC_AMP | CS_SYNC_PHASE;
            }

            // Output amplification curve
            if ((c->pTrAmp != NULL) && (c->nSync & CS_SYNC_AMP))
            {
                mesh_t *mesh        = c->pTrAmp->getBuffer<mesh_t>();
                if ((mesh != NULL) && (mesh->isEmpty()))
                {
                    dsp::copy(mesh->pvData[0], vFreqs, para_equalizer_base_metadata::MESH_POINTS);
                    dsp::complex_mod(mesh->pvData[1], c->vTrRe, c->vTrIm, para_equalizer_base_metadata::MESH_POINTS);
                    mesh->data(2, para_equalizer_base_metadata::MESH_POINTS);

                    c->nSync           &= ~CS_SYNC_AMP;
                }
            }
        }
    }

    //-------------------------------------------------------------------------
    para_equalizer_x16_mono::para_equalizer_x16_mono() : para_equalizer_base(metadata, 16, EQ_MONO)
    {
    }

    para_equalizer_x32_mono::para_equalizer_x32_mono() : para_equalizer_base(metadata, 32, EQ_MONO)
    {
    }

    para_equalizer_x16_stereo::para_equalizer_x16_stereo() : para_equalizer_base(metadata, 16, EQ_STEREO)
    {
    }

    para_equalizer_x32_stereo::para_equalizer_x32_stereo() : para_equalizer_base(metadata, 32, EQ_STEREO)
    {
    }

    para_equalizer_x16_lr::para_equalizer_x16_lr() : para_equalizer_base(metadata, 16, EQ_LEFT_RIGHT)
    {
    }

    para_equalizer_x32_lr::para_equalizer_x32_lr() : para_equalizer_base(metadata, 32, EQ_LEFT_RIGHT)
    {
    }

    para_equalizer_x16_ms::para_equalizer_x16_ms() : para_equalizer_base(metadata, 16, EQ_MID_SIDE)
    {
    }

    para_equalizer_x32_ms::para_equalizer_x32_ms() : para_equalizer_base(metadata, 32, EQ_MID_SIDE)
    {
    }

} /* namespace lsp */
