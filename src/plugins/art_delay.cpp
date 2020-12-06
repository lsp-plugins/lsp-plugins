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

#define BUFFER_SIZE             0x1000U
#define DELAY_REF_NONE          -1

#define TRACE_PORT(p)           lsp_trace("  port id=%s", (p)->metadata()->id);

namespace lsp
{
    static float art_delay_ratio[] =
    {
        1.0f / 1.0f,
        1.0f / 2.0f,
        1.0f / 3.0f,
        2.0f / 1.0f,
        2.0f / 3.0f,
        3.0f / 1.0f,
        3.0f / 2.0f
    };

    static const float band_freqs[] =
    {
        60.0f,
        300.0f,
        1000.0f,
        6000.0f
    };

    static uint8_t art_delay_max[] =
    {
        1, 2, 4, 8,
        16, 24, 32, 40, 48, 56, 64
    };

    art_delay_base::DelayAllocator::DelayAllocator(art_delay_t *delay)
    {
        pDelay      = delay;
        nSize       = 0;
    }

    art_delay_base::DelayAllocator::~DelayAllocator()
    {
    }

    status_t art_delay_base::DelayAllocator::run()
    {
        DynamicDelay *d;
        size_t channels = (pDelay->bStereo) ? 2 : 1;

        // Drop garbage
        for (size_t i=0; i<channels; ++i)
        {
            if ((d = pDelay->pGDelay[i]) != NULL)
            {
                pDelay->pGDelay[i] = NULL;
                d->destroy();
                delete d;
            }

            if ((d = pDelay->pPDelay[i]) != NULL)
            {
                pDelay->pPDelay[i] = NULL;
                d->destroy();
                delete d;
            }
        }

        if (nSize < 0)
            return STATUS_OK;

        // Allocate delays
        for (size_t i=0; i<channels; ++i)
        {
            d = pDelay->pCDelay[i];
            if ((d != NULL) && (d->max_delay() == size_t(nSize)))
                continue;

            // Allocate delay
            d = new DynamicDelay();
            if (d == NULL)
                return STATUS_NO_MEM;

            // Reserve space for delay
            status_t res = d->init(nSize);
            if (res != STATUS_OK)
            {
                d->destroy();
                delete d;
                return res;
            }

            // Add delay to list of pending
            pDelay->pPDelay[i]  = d;
        }

        return STATUS_OK;
    }

    art_delay_base::art_delay_base(const plugin_metadata_t &mdata, bool stereo_in): plugin_t(mdata)
    {
        bStereoIn       = stereo_in;
        bMono           = false;
        nMaxDelay       = 0;
        fOldDryGain     = 1.0f;
        fNewDryGain     = 1.0f;
        fOldPan[0]      = 1.0f;
        fOldPan[1]      = 1.0f;
        fNewPan[0]      = 1.0f;
        fNewPan[1]      = 1.0f;
        vDataBuf[0]     = NULL;
        vDataBuf[1]     = NULL;
        vOutBuf[0]      = NULL;
        vOutBuf[1]      = NULL;
        vGainBuf        = NULL;
        vDelayBuf       = NULL;
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
        size_t sz_alloc         = sz_tempo + sz_proc + sz_buf * 6;

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

        vGainBuf                = reinterpret_cast<float *>(ptr);
        ptr                    += sz_buf;
        vDelayBuf               = reinterpret_cast<float *>(ptr);
        ptr                    += sz_buf;

        vTempo                  = reinterpret_cast<art_tempo_t *>(ptr);
        ptr                    += sz_tempo;
        vDelays                 = reinterpret_cast<art_delay_t *>(ptr);
        ptr                    += sz_proc;

        // Initialize tempos
        for (size_t i=0; i<MAX_TEMPOS; ++i)
        {
            art_tempo_t *at         = &vTempo[i];

            at->fTempo              = BPM_DEFAULT;
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

            ad->bStereo             = bStereoIn;
            ad->bOn                 = false;
            ad->bSolo               = false;
            ad->bMute               = false;
            ad->bUpdated            = false;
            ad->bValidRef           = true;
            ad->nDelayRef           = DELAY_REF_NONE;

            ad->sOld.fDelay         = 0.0f;
            ad->sOld.fGain          = 0.0f;
            ad->sOld.fFeedback      = 0.0f;
            if (bStereoIn)
            {
                ad->sOld.fPan[0]        = 1.0f;
                ad->sOld.fPan[1]        = 1.0f;
            }
            else
            {
                ad->sOld.fPan[0]        = 0.5f;
                ad->sOld.fPan[1]        = 0.5f;
            }
            ad->sOld.nMaxDelay      = 0;

            ad->sNew                = ad->sOld;

            ad->pOn                 = NULL;
            ad->pTempoRef           = NULL;
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
            ad->pFeedOn             = NULL;
            ad->pFeedGain           = NULL;
            ad->pGain               = NULL;
            ad->pOutDelay           = NULL;
            ad->pOutOfRange         = NULL;
            ad->pOutLoop            = NULL;
        }

        // Initialize bypasses
        sBypass[0].construct();
        sBypass[1].construct();

        // Get executor service
        pExecutor               = wrapper->get_executor();
        size_t port_id          = 0;

        // Bind in/out ports
        lsp_trace("Binding audio ports");

        TRACE_PORT(vPorts[port_id]);
        pIn[0]          = vPorts[port_id++];
        if (bStereoIn)
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
            ad->pTempoRef           = vPorts[port_id++];
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
            ad->pFeedOn             = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            ad->pFeedGain           = vPorts[port_id++];
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

    void art_delay_base::update_sample_rate(long sr)
    {
        sBypass[0].init(sr);
        sBypass[1].init(sr);

        for (size_t i=0; i<MAX_PROCESSORS; ++i)
        {
            art_delay_t *ad         = &vDelays[i];

            // The length of each delay will be changed in offline mode
            ad->sEq[0].set_sample_rate(sr);
            ad->sEq[1].set_sample_rate(sr);
            ad->sBypass[0].init(sr);
            ad->sBypass[1].init(sr);
        }
    }

    float art_delay_base::decode_ratio(size_t v)
    {
        return (v < sizeof(art_delay_ratio)/sizeof(float)) ? art_delay_ratio[v] : 1.0f;
    }

    size_t art_delay_base::decode_max_delay_value(size_t v)
    {
        return (v < sizeof(art_delay_max)/sizeof(uint8_t)) ? art_delay_max[v] : 1.0f;
    }

    bool art_delay_base::check_delay_ref(art_delay_t *ad)
    {
        art_delay_t *list[MAX_PROCESSORS];
        size_t n = 0;
        list[n++] = ad;

        while (ad->nDelayRef >= 0)
        {
            // Get referenced delay
            ad  = &vDelays[ad->nDelayRef];

            // Check that there is no reference already
            for (size_t i=0; i<n; ++i)
                if (list[i] == ad)
                    return false;
        }

        return true;
    }

    void art_delay_base::update_settings()
    {
        bool bypass         = pBypass->getValue() >= 0.5f;
        float wet           = pWetGain->getValue();
        bool fback          = pFeedback->getValue();

        bMono               = pMono->getValue() >= 0.5f;
        fNewDryGain         = pDryGain->getValue();
        nMaxDelay           = decode_max_delay_value(pMaxDelay->getValue());

        fNewPan[0]          = (100.0f - pPan[0]->getValue()) * 0.005f * wet;
        fNewPan[1]          = (100.0f - pPan[1]->getValue()) * 0.005f * wet;

        sBypass[0].set_bypass(bypass);
        sBypass[1].set_bypass(bypass);

        // Sync state of tempos
        for (size_t i=0; i<MAX_TEMPOS; ++i)
        {
            art_tempo_t *at         = &vTempo[i];

            bool sync               = at->pSync->getValue() >= 0.5f;
            float ratio             = decode_ratio(at->pRatio->getValue());
            float tempo             = (sync) ? pWrapper->position()->beatsPerMinute : at->pTempo->getValue();

            at->bSync               = sync;
            at->fTempo              = ratio * tempo;
        }

        // Sync state of each delay
        for (size_t i=0; i<MAX_PROCESSORS; ++i)
        {
            art_delay_t *ad         = &vDelays[i];

            ad->bOn                 = ad->pOn->getValue() >= 0.5f;
            ad->bSolo               = ad->pSolo->getValue() >= 0.5f;
            ad->bMute               = ad->pMute->getValue() >= 0.5f;
            ad->bUpdated            = false;
            ad->nDelayRef           = ad->pDelayRef->getValue() - 1.0f;
        }

        // Validate state of delay
        bool has_solo           = false;
        for (size_t i=0; i<MAX_PROCESSORS; ++i)
        {
            art_delay_t *ad         = &vDelays[i];
            ad->bValidRef           = check_delay_ref(ad);
            if ((ad->bOn) && (ad->bSolo))
                has_solo                = true;
        }

        // Apply recursive settings to delays
        for (size_t i=0, nupd=0; nupd < MAX_PROCESSORS; i = (i % MAX_PROCESSORS))
        {
            // Get the delay
            art_delay_t *ad         = &vDelays[i];
            if (ad->bUpdated)
                continue;

            // Get parent delay reference and check that it is updated
            art_delay_t *p_ad       = ((ad->bValidRef) && (ad->nDelayRef >= 0)) ? &vDelays[ad->nDelayRef] : NULL;
            if ((p_ad != NULL) && (!p_ad->bUpdated))
                continue;

            bool pfback             = (fback) && (ad->pFeedOn->getValue() >= 0.5f);
            float delay             = ad->pDelay->getValue();

            // Tempo reference
            ssize_t tempo_ref       = ad->pTempoRef->getValue() - 1.0f;
            if (tempo_ref >= 0)
            {
                // Compute bar * multiplier + fraction
                art_tempo_t *at         = &vTempo[tempo_ref];
                float bdelay            = ad->pBarFrac->getValue() * ad->pBarMul->getValue() + ad->pFrac->getValue();
                delay                  += seconds_to_samples(fSampleRate, (240.0f * bdelay) / at->fTempo);
            }

            // Parent delay reference
            if (p_ad != NULL)
                delay                  += p_ad->sNew.fDelay * ad->pDelayMul->getValue();

            // Update delay settings
            ad->sNew.fDelay         = delay;
            ad->sNew.fGain          = ad->pGain->getValue();
            ad->sNew.fFeedback      = (pfback) ? ad->pFeedGain->getValue() : 0.0f;
            ad->sNew.fPan[0]        = (100.0f - pPan[0]->getValue()) * 0.005f;
            ad->sNew.fPan[1]        = (100.0f - pPan[1]->getValue()) * 0.005f;

            // Determine mode
            bool eq_on          = ad->pEqOn->getValue() >= 0.5f;
            bool low_on         = ad->pLcfOn->getValue() >= 0.5f;
            bool high_on        = ad->pHcfOn->getValue() >= 0.5f;
            bool xbypass        = (bypass) || (ad->bMute) || ((has_solo) && (!ad->bSolo));
            equalizer_mode_t eq_mode = (eq_on || low_on || high_on) ? EQM_IIR : EQM_BYPASS;

            // Update processor settings
            for (size_t j=0; j<2; ++j)
            {
                // Update bypass
                ad->sBypass[j].set_bypass(xbypass);

                // Update equalizer
                Equalizer *eq   = &ad->sEq[j];
                eq->set_mode(eq_mode);

                if (eq_mode == EQM_BYPASS)
                    continue;

                filter_params_t fp;
                size_t band     = 0;

                // Set-up parametric equalizer
                while (band < EQ_BANDS)
                {
                    if (band == 0)
                    {
                        fp.fFreq        = band_freqs[band];
                        fp.fFreq2       = fp.fFreq;
                        fp.nType        = (eq_on) ? FLT_MT_LRX_LOSHELF : FLT_NONE;
                    }
                    else if (band == (EQ_BANDS - 1))
                    {
                        fp.fFreq        = band_freqs[band-1];
                        fp.fFreq2       = fp.fFreq;
                        fp.nType        = (eq_on) ? FLT_MT_LRX_HISHELF : FLT_NONE;
                    }
                    else
                    {
                        fp.fFreq        = band_freqs[band-1];
                        fp.fFreq2       = band_freqs[band];
                        fp.nType        = (eq_on) ? FLT_MT_LRX_LADDERPASS : FLT_NONE;
                    }

                    fp.fGain        = ad->pBandGain[band]->getValue();
                    fp.nSlope       = 2;
                    fp.fQuality     = 0.0f;

                    // Update filter parameters
                    eq->set_params(band++, &fp);
                }

                // Setup hi-pass filter
                fp.nType        = (low_on) ? FLT_BT_BWC_HIPASS : FLT_NONE;
                fp.fFreq        = ad->pLcfFreq->getValue();
                fp.fFreq2       = fp.fFreq;
                fp.fGain        = 1.0f;
                fp.nSlope       = 4;
                fp.fQuality     = 0.0f;
                eq->set_params(band++, &fp);

                // Setup low-pass filter
                fp.nType        = (high_on) ? FLT_BT_BWC_LOPASS : FLT_NONE;
                fp.fFreq        = ad->pHcfFreq->getValue();
                fp.fFreq2       = fp.fFreq;
                fp.fGain        = 1.0f;
                fp.nSlope       = 4;
                fp.fQuality     = 0.0f;
                eq->set_params(band++, &fp);
            }
        }
    }

    void art_delay_base::sync_delay(art_delay_t *ad)
    {
        DelayAllocator *da = ad->pAllocator;
        size_t channels    = (ad->bStereo) ? 2 : 1;

        if (da->idle())
        {
            if (ad->bOn)
            {
                bool resize     = false;

                // Check that delay has to be resized
                for (size_t i=0; i < channels; ++i)
                {
                    if ((ad->pCDelay[i] == NULL) || (ad->pCDelay[i]->max_delay() != nMaxDelay))
                        resize      = true;
                }

                // Need to issue resize?
                if (resize)
                {
                    da->set_size(nMaxDelay);
                    pExecutor->submit(da);
                }
            }
            else
            {
                // Estimate the garbage cleanup flag
                bool gc = false;
                for (size_t i=0; i < channels; ++i)
                {
                    if ((ad->pGDelay[i] == NULL) && (ad->pCDelay[i] != NULL))
                    {
                        ad->pGDelay[i] = ad->pCDelay[i];
                        ad->pCDelay[i] = NULL;
                    }

                    gc = gc || (ad->pGDelay[i] != NULL) || (ad->pPDelay[i] != NULL);
                }

                // Need to clean the whole garbage ?
                if (gc)
                {
                    da->set_size(-1);
                    pExecutor->submit(da);
                }
            }
        }
        else if (da->completed())
        {
            // Update delay
            bool gc = false;

            for (size_t i=0; i < channels; ++i)
            {
                // There is data to commit?
                if (ad->pPDelay[i] == NULL)
                    continue;

                // Copy delay data
                ad->pPDelay[i]->copy(ad->pCDelay[i]);

                // Swap pointers
                ad->pGDelay[i] = ad->pCDelay[i];
                ad->pCDelay[i] = ad->pPDelay[i];
                ad->pPDelay[i] = NULL;

                // Update garbage flag
                gc = gc || (ad->pGDelay[i] != NULL);
            }

            // Reset task state
            da->reset();

            // Need to clean garbage?
            if (gc)
            {
                da->set_size(nMaxDelay);
                pExecutor->submit(da);
            }
        }
    }

    void art_delay_base::process_delay(art_delay_t *ad, float **out, const float * const *in, size_t samples, size_t i, size_t count)
    {
        // TODO
    }

    void art_delay_base::process(size_t samples)
    {
        // Sync delay lines
        for (size_t j=0; j<MAX_PROCESSORS; ++j)
        {
            art_delay_t *ad     = &vDelays[j];
            sync_delay(ad);
        }

        // Bind ports
        float *in[2], *out[2];

        in[0]   = pIn[0]->getBuffer<float>();
        in[1]   = (bStereoIn) ? pIn[1]->getBuffer<float>() : NULL;

        out[0]  = pOut[0]->getBuffer<float>();
        out[1]  = pOut[1]->getBuffer<float>();

        // Process audio signal
        for (size_t i=0; i<samples; )
        {
            // How many samples we can process at one time?
            size_t count        = lsp_min(samples - i, BUFFER_SIZE);

            // Fill output buffer with dry sound
            if (bStereoIn)
            {
                if (fOldDryGain == fNewDryGain)
                {
                    dsp::mul_k3(vOutBuf[0], in[0], fNewDryGain, count);
                    dsp::mul_k3(vOutBuf[1], in[1], fNewDryGain, count);
                }
                else
                {
                    dsp::lin_inter_mul3(vOutBuf[0], in[0], 0, fOldDryGain, samples, fNewDryGain, i, count);
                    dsp::lin_inter_mul3(vOutBuf[1], in[1], 0, fOldDryGain, samples, fNewDryGain, i, count);
                }
            }
            else
            {
                if (fOldDryGain == fNewDryGain)
                    dsp::mul_k3(vOutBuf[0], in[0], fNewDryGain, count);
                else
                    dsp::lin_inter_mul3(vOutBuf[0], in[0], 0, fOldDryGain, samples, fNewDryGain, i, count);
                dsp::copy(vOutBuf[1], vOutBuf[0], count);
            }

            // Process delay channels and store result to vDataBuf
            dsp::fill_zero(vDataBuf[0], count);
            dsp::fill_zero(vDataBuf[1], count);

            for (size_t j=0; j<MAX_PROCESSORS; ++j)
            {
                art_delay_t *ad     = &vDelays[j];
                if (ad->bOn)
                    process_delay(ad, vDataBuf, in, samples, i, count);
            }

            // Apply panning and add wet sound to output buffers
            for (size_t j=0; j<2; ++j)
            {
                size_t c1   = j, c2 = (j + 1) & 1;
                float pan   = fOldPan[j];
                float npan  = fNewPan[j];

                if (pan != npan)
                {
                    dsp::lin_inter_fmadd2(vOutBuf[j], vDataBuf[c1], 0, pan, samples, npan, i, count);
                    dsp::lin_inter_fmadd2(vOutBuf[j], vDataBuf[c2], 0, 1.0f - pan, samples, 1.0f - npan, i, count);
                }
                else
                    dsp::mix_add2(vOutBuf[j], vDataBuf[c1], vDataBuf[c2], pan, 1.0f - pan, count);
            }

            // Output internal buffer data to external outputs via applied bypass
            if (bStereoIn)
            {
                if (bMono)
                {
                    dsp::lr_to_mid(vOutBuf[0], vOutBuf[0], vOutBuf[1], count);
                    sBypass[0].process(out[0], in[0], vOutBuf[0], count);
                    sBypass[0].process(out[1], in[1], vOutBuf[0], count);
                }
                else
                {
                    sBypass[0].process(out[0], in[0], vOutBuf[0], count);
                    sBypass[0].process(out[1], in[1], vOutBuf[1], count);
                }
            }
            else
            {
                if (bMono)
                {
                    dsp::lr_to_mid(vOutBuf[0], vOutBuf[0], vOutBuf[1], count);
                    sBypass[0].process(out[0], in[0], vOutBuf[0], count);
                    sBypass[0].process(out[1], in[0], vOutBuf[0], count);
                }
                else
                {
                    sBypass[0].process(out[0], in[0], vOutBuf[0], count);
                    sBypass[0].process(out[1], in[0], vOutBuf[1], count);
                }
            }

            // Update positions
            in[0]              += count;
            if (bStereoIn)
                in[1]              += count;
            out[0]             += count;
            out[1]             += count;
            i                  += count;
        }

        // Commit dynamic settings
        fOldDryGain         = fNewDryGain;
        fOldPan[0]          = fNewPan[0];
        fOldPan[1]          = fNewPan[1];

        for (size_t j=0; j<MAX_PROCESSORS; ++j)
        {
            art_delay_t *ad     = &vDelays[j];
            ad->sOld            = ad->sNew;
        }
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



