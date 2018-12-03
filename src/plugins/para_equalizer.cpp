/*
 * para_equalizer.cpp
 *
 *  Created on: 30 июня 2016 г.
 *      Author: sadko
 */

#include <core/types.h>
#include <core/debug.h>
#include <core/colors.h>
#include <core/util/Color.h>

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
        vChannels   = NULL;
        vFreqs      = NULL;
        vIndexes    = NULL;
        fGainIn     = 1.0f;
        fZoom       = 1.0f;
        bListen     = false;
        nFftPosition= FFTP_NONE;
        pIDisplay   = NULL;

        pBypass     = NULL;
        pGainIn     = NULL;
        pGainOut    = NULL;
        pFftMode    = NULL;
        pReactivity = NULL;
        pListen     = NULL;
        pShiftGain  = NULL;
        pZoom       = NULL;
        pEqMode     = NULL;
        pBalance    = NULL;
    }
    
    para_equalizer_base::~para_equalizer_base()
    {
        destroy_state();
    }

    inline void para_equalizer_base::decode_filter(size_t *ftype, size_t *slope, size_t mode)
    {
        #define EQF(x) para_equalizer_base_metadata::EQF_ ## x
        #define EQS(k, t, ks) case para_equalizer_base_metadata::EFM_ ## k:    \
                *ftype = t; \
                *slope = ks * *slope; \
                return;
        #define EQDFL  default: \
                *ftype = FLT_NONE; \
                *slope = 1; \
                return;

        switch (*ftype)
        {
            case EQF(BELL):
            {
                switch (mode)
                {
                    EQS(RLC_BT, FLT_BT_RLC_BELL, 1)
                    EQS(RLC_MT, FLT_MT_RLC_BELL, 1)
                    EQS(BWC_BT, FLT_BT_BWC_BELL, 1)
                    EQS(BWC_MT, FLT_MT_BWC_BELL, 1)
                    EQS(LRX_BT, FLT_BT_LRX_BELL, 1)
                    EQS(LRX_MT, FLT_MT_LRX_BELL, 1)
                    EQDFL
                }
                break;
            }

            case EQF(HIPASS):
            {
                switch (mode)
                {
                    EQS(RLC_BT, FLT_BT_RLC_HIPASS, 2)
                    EQS(RLC_MT, FLT_MT_RLC_HIPASS, 2)
                    EQS(BWC_BT, FLT_BT_BWC_HIPASS, 2)
                    EQS(BWC_MT, FLT_MT_BWC_HIPASS, 2)
                    EQS(LRX_BT, FLT_BT_LRX_HIPASS, 1)
                    EQS(LRX_MT, FLT_MT_LRX_HIPASS, 1)
                    EQDFL
                }
                break;
            }

            case EQF(HISHELF):
            {
                switch (mode)
                {
                    EQS(RLC_BT, FLT_BT_RLC_HISHELF, 1)
                    EQS(RLC_MT, FLT_MT_RLC_HISHELF, 1)
                    EQS(BWC_BT, FLT_BT_BWC_HISHELF, 1)
                    EQS(BWC_MT, FLT_MT_BWC_HISHELF, 1)
                    EQS(LRX_BT, FLT_BT_LRX_HISHELF, 1)
                    EQS(LRX_MT, FLT_MT_LRX_HISHELF, 1)
                    EQDFL
                }
                break;
            }

            case EQF(LOPASS):
            {
                switch (mode)
                {
                    EQS(RLC_BT, FLT_BT_RLC_LOPASS, 2)
                    EQS(RLC_MT, FLT_MT_RLC_LOPASS, 2)
                    EQS(BWC_BT, FLT_BT_BWC_LOPASS, 2)
                    EQS(BWC_MT, FLT_MT_BWC_LOPASS, 2)
                    EQS(LRX_BT, FLT_BT_LRX_LOPASS, 1)
                    EQS(LRX_MT, FLT_MT_LRX_LOPASS, 1)
                    EQDFL
                }
                break;
            }

            case EQF(LOSHELF):
            {
                switch (mode)
                {
                    EQS(RLC_BT, FLT_BT_RLC_LOSHELF, 1)
                    EQS(RLC_MT, FLT_MT_RLC_LOSHELF, 1)
                    EQS(BWC_BT, FLT_BT_BWC_LOSHELF, 1)
                    EQS(BWC_MT, FLT_MT_BWC_LOSHELF, 1)
                    EQS(LRX_BT, FLT_BT_LRX_LOSHELF, 1)
                    EQS(LRX_MT, FLT_MT_LRX_LOSHELF, 1)
                    EQDFL
                }
                break;
            }

            case EQF(NOTCH):
            {
                switch (mode)
                {
                    EQS(RLC_BT, FLT_BT_RLC_NOTCH, 1)
                    EQS(RLC_MT, FLT_MT_RLC_NOTCH, 1)
                    EQS(BWC_BT, FLT_BT_RLC_NOTCH, 1)
                    EQS(BWC_MT, FLT_MT_RLC_NOTCH, 1)
                    EQS(LRX_BT, FLT_BT_RLC_NOTCH, 1)
                    EQS(LRX_MT, FLT_MT_RLC_NOTCH, 1)
                    EQDFL
                }
                break;
            }

            case EQF(RESONANCE):
            {
                switch (mode)
                {
                    EQS(RLC_BT, FLT_BT_RLC_RESONANCE, 1)
                    EQS(RLC_MT, FLT_MT_RLC_RESONANCE, 1)
                    EQS(BWC_BT, FLT_BT_RLC_RESONANCE, 1)
                    EQS(BWC_MT, FLT_MT_RLC_RESONANCE, 1)
                    EQS(LRX_BT, FLT_BT_RLC_RESONANCE, 1)
                    EQS(LRX_MT, FLT_MT_RLC_RESONANCE, 1)
                    EQDFL
                }
                break;
            }

#ifndef LSP_NO_EXPERIMENTAL
            case EQF(LADDERPASS):
            {
                switch (mode)
                {
                    EQS(RLC_BT, FLT_BT_RLC_LADDERPASS, 1)
                    EQS(RLC_MT, FLT_MT_RLC_LADDERPASS, 1)
                    EQS(BWC_BT, FLT_BT_BWC_LADDERPASS, 1)
                    EQS(BWC_MT, FLT_MT_BWC_LADDERPASS, 1)
                    EQS(LRX_BT, FLT_BT_LRX_LADDERPASS, 1)
                    EQS(LRX_MT, FLT_MT_LRX_LADDERPASS, 1)
                    EQDFL
                }
                break;
            }

            case EQF(LADDERREJ):
            {
                switch (mode)
                {
                    EQS(RLC_BT, FLT_BT_RLC_LADDERREJ, 1)
                    EQS(RLC_MT, FLT_MT_RLC_LADDERREJ, 1)
                    EQS(BWC_BT, FLT_BT_BWC_LADDERREJ, 1)
                    EQS(BWC_MT, FLT_MT_BWC_LADDERREJ, 1)
                    EQS(LRX_BT, FLT_BT_LRX_LADDERREJ, 1)
                    EQS(LRX_MT, FLT_MT_LRX_LADDERREJ, 1)
                    EQDFL
                }
                break;
            }

            case EQF(ENVELOPE):
            {
                switch (mode)
                {
                    EQS(RLC_BT, FLT_BT_RLC_ENVELOPE, 1)
                    EQS(RLC_MT, FLT_MT_RLC_ENVELOPE, 1)
                    EQS(BWC_BT, FLT_BT_RLC_ENVELOPE, 1)
                    EQS(BWC_MT, FLT_MT_RLC_ENVELOPE, 1)
                    EQS(LRX_BT, FLT_BT_RLC_ENVELOPE, 1)
                    EQS(LRX_MT, FLT_MT_RLC_ENVELOPE, 1)
                    EQDFL
                }
                break;
            }

            case EQF(BANDPASS):
            {
                switch (mode)
                {
                    EQS(RLC_BT, FLT_BT_RLC_BANDPASS, 1)
                    EQS(RLC_MT, FLT_MT_RLC_BANDPASS, 1)
                    EQS(BWC_BT, FLT_BT_BWC_BANDPASS, 1)
                    EQS(BWC_MT, FLT_MT_BWC_BANDPASS, 1)
                    EQS(LRX_BT, FLT_BT_LRX_BANDPASS, 1)
                    EQS(LRX_MT, FLT_MT_LRX_BANDPASS, 1)
                    EQDFL
                }
                break;
            }
#endif /* LSP_NO_EXPERIMENTAL */

            case EQF(OFF):
                EQDFL;
        }
        #undef EQDFL
        #undef EQS
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
        // Pass wrapper
        plugin_t::init(wrapper);

        // Determine number of channels
        size_t channels     = (nMode == EQ_MONO) ? 1 : 2;

        // Initialize analyzer
        if (!sAnalyzer.init(channels, para_equalizer_base_metadata::FFT_RANK))
            return;

        sAnalyzer.set_rank(para_equalizer_base_metadata::FFT_RANK);
        sAnalyzer.set_activity(false);
        sAnalyzer.set_envelope(para_equalizer_base_metadata::FFT_ENVELOPE);
        sAnalyzer.set_window(para_equalizer_base_metadata::FFT_WINDOW);
        sAnalyzer.set_rate(para_equalizer_base_metadata::REFRESH_RATE);

        // Allocate channels
        vChannels           = new eq_channel_t[channels];
        if (vChannels == NULL)
            return;

        // Initialize global parameters
        fGainIn             = 1.0f;
        bListen             = false;
        nFftPosition        = FFTP_NONE;

        // Allocate indexes
        vIndexes            = new uint32_t[para_equalizer_base_metadata::MESH_POINTS];
        if (vIndexes == NULL)
            return;

        // Calculate amount of bulk data to allocate
        size_t allocate     = (2 * para_equalizer_base_metadata::MESH_POINTS * (nFilters + 1) + EQ_BUFFER_SIZE) * channels + para_equalizer_base_metadata::MESH_POINTS;
        float *abuf         = new float[allocate];
        if (abuf == NULL)
            return;

        // Clear all floating-point buffers
        dsp::fill_zero(abuf, allocate);

        // Frequency list buffer
        vFreqs              = abuf;
        abuf               += para_equalizer_base_metadata::MESH_POINTS;

        // Initialize each channel
        for (size_t i=0; i<channels; ++i)
        {
            eq_channel_t *c     = &vChannels[i];

            c->nLatency         = 0;
            c->fInGain          = 1.0f;
            c->fOutGain         = 1.0f;
            c->vFilters         = NULL;
            c->vBuffer          = abuf;
            abuf               += EQ_BUFFER_SIZE;
            c->vTrRe            = abuf;
            abuf               += para_equalizer_base_metadata::MESH_POINTS;
            c->vTrIm            = abuf;
            abuf               += para_equalizer_base_metadata::MESH_POINTS;

            // Input and output ports
            c->vIn              = NULL;
            c->vOut             = NULL;

            // Ports
            c->pIn              = NULL;
            c->pOut             = NULL;
            c->pInGain          = NULL;
            c->pTrAmp           = NULL;
            c->pFft             = NULL;
            c->pVisible         = NULL;
            c->pMeter           = NULL;
        }

        // Allocate data
        for (size_t i=0; i<channels; ++i)
        {
            // Allocate data
            eq_channel_t *c     = &vChannels[i];
            c->nSync            = CS_UPDATE;
            c->vFilters         = new eq_filter_t[nFilters];
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
                f->nSync            = CS_UPDATE;

                // Additional parameters
                f->pType            = NULL;
                f->pMode            = NULL;
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
        pEqMode                 = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pFftMode                = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pReactivity             = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pShiftGain              = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pZoom                   = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        port_id++; // Skip filter selector

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
                    f->pMode            = sf->pMode;
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
                    f->pMode        = vPorts[port_id++];
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

        if (pIDisplay != NULL)
        {
            pIDisplay->detroy();
            pIDisplay   = NULL;
        }

        // Destroy analyzer
        sAnalyzer.destroy();
    }

    void para_equalizer_base::update_settings()
    {
        // Check sample rate
        if (fSampleRate <= 0)
            return;

        // Update common settings
        if (pGainIn != NULL)
            fGainIn     = pGainIn->getValue();
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
        float bal[2] = { 1.0f, 1.0f };
        if (pBalance != NULL)
        {
            float xbal      = pBalance->getValue();
            bal[0]          = (100.0f - xbal) * 0.01f;
            bal[1]          = (xbal + 100.0f) * 0.01f;
        }
        if (pGainOut != NULL)
        {
            float out_gain  = pGainOut->getValue();
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

        // Update equalizer mode
        equalizer_mode_t eq_mode    = get_eq_mode();
        bool bypass                 = pBypass->getValue() >= 0.5f;

        // For each channel
        for (size_t i=0; i<channels; ++i)
        {
            filter_params_t fp;
            eq_channel_t *c     = &vChannels[i];
            bool solo           = false;
            bool visible        = (c->pVisible == NULL) ?  true : (c->pVisible->getValue() >= 0.5f);
            c->sEqualizer.set_mode(eq_mode);

            // Update settings
            if (c->sBypass.set_bypass(bypass))
                pWrapper->query_display_draw();
            c->fOutGain         = bal[i];
            if (c->pInGain != NULL)
                c->fInGain          = c->pInGain->getValue();

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
                eq_filter_t *f      = &c->vFilters[j];

                // Check if need to update parameters
                size_t ft   = FLT_NONE, slope = 1;
                bool mute   = f->pMute->getValue() >= 0.5f;
                if ((!mute) && ((solo) ^ (!f->bSolo)))
                {
                    ft          = f->pType->getValue();
                    slope       = f->pSlope->getValue() + 1;
                    decode_filter(&ft, &slope, f->pMode->getValue());
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
//                    modified            = true;
                    fp.nType            = ft;
                    fp.fFreq            = f->pFreq->getValue();
                    #ifdef LSP_NO_EXPERIMENTAL
                        fp.fFreq2           = fp.fFreq;
                    #else
                        fp.fFreq2           = 10.0f * fp.fFreq;
                    #endif /* LSP_NO_EXPERIMENTAL */
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
        }

        // Update analyzer
        if (sAnalyzer.needs_reconfiguration())
        {
            sAnalyzer.reconfigure();
            sAnalyzer.get_frequencies(vFreqs, vIndexes, SPEC_FREQ_MIN, SPEC_FREQ_MAX, para_equalizer_base_metadata::MESH_POINTS);
        }
    }

    void para_equalizer_base::update_sample_rate(long sr)
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

        size_t fft_pos          = (ui_active()) ? nFftPosition : FFTP_NONE;

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
                    dsp::scale2(vChannels[0].vBuffer, fGainIn, to_process);
                    dsp::scale2(vChannels[1].vBuffer, fGainIn, to_process);
                }
            }
            else if (nMode == EQ_MONO)
            {
                if (fGainIn != 1.0f)
                    dsp::scale3(vChannels[0].vBuffer, vChannels[0].vIn, fGainIn, to_process);
                else
                    dsp::copy(vChannels[0].vBuffer, vChannels[0].vIn, to_process);
            }
            else
            {
                if (fGainIn != 1.0f)
                {
                    dsp::scale3(vChannels[0].vBuffer, vChannels[0].vIn, fGainIn, to_process);
                    dsp::scale3(vChannels[1].vBuffer, vChannels[1].vIn, fGainIn, to_process);
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

            // Update counters and pointers
            samples            -= to_process;
        }

        // Output FFT curves for each channel and report latency
        size_t latency          = 0;

        for (size_t i=0; i<channels; ++i)
        {
            eq_channel_t *c     = &vChannels[i];

            if (latency < c->sEqualizer.get_latency())
                latency         = c->sEqualizer.get_latency();

            // Output FFT curve
            mesh_t *mesh            = c->pFft->getBuffer<mesh_t>();
            if ((mesh != NULL) && (mesh->isEmpty()))
            {
                if (nFftPosition != FFTP_NONE)
                {
                    // Copy frequency points
                    dsp::copy(mesh->pvData[0], vFreqs, para_equalizer_base_metadata::MESH_POINTS);
                    sAnalyzer.get_spectrum(i, mesh->pvData[1], vIndexes, para_equalizer_base_metadata::MESH_POINTS);

                    // Mark mesh containing data
                    mesh->data(2, para_equalizer_base_metadata::MESH_POINTS);
                }
                else
                    mesh->data(2, 0);
            }
        }

        set_latency(latency);

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
                    f->nSync    = CS_SYNC_AMP;
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
                            mesh->pvData[0][0] = SPEC_FREQ_MIN*0.5f;
                            mesh->pvData[0][para_equalizer_base_metadata::MESH_POINTS+1] = SPEC_FREQ_MAX*2.0;
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
                    dsp::complex_mul3(c->vTrRe, c->vTrIm, c->vTrRe, c->vTrIm, f->vTrRe, f->vTrIm, para_equalizer_base_metadata::MESH_POINTS);
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
                    dsp::copy(mesh->pvData[0], vFreqs, para_equalizer_base_metadata::MESH_POINTS);
                    dsp::complex_mod(mesh->pvData[1], c->vTrRe, c->vTrIm, para_equalizer_base_metadata::MESH_POINTS);
                    mesh->data(2, para_equalizer_base_metadata::MESH_POINTS);

                    c->nSync           &= ~CS_SYNC_AMP;
                }

                // Request for redraw
                if (pWrapper != NULL)
                    pWrapper->query_display_draw();
            }
        }
    }

    bool para_equalizer_base::inline_display(ICanvas *cv, size_t width, size_t height)
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
                size_t k        = (j*para_equalizer_base_metadata::MESH_POINTS)/width;
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
