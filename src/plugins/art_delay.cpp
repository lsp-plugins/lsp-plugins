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
#include <dsp/atomic.h>

#define BUFFER_SIZE             0x1000U
#define DELAY_REF_NONE          -1

#define TRACE_PORT(p)           lsp_trace("  port id=%s", (p)->metadata()->id);

namespace lsp
{
    static const float art_delay_ratio[] =
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

    static const uint16_t art_delay_max[] =
    {
        1, 2, 4, 8,
        16, 24, 32, 40,
        48, 56, 64, 96,
        128, 160, 192, 224,
        256
    };

    art_delay_base::DelayAllocator::DelayAllocator(art_delay_base *base, art_delay_t *delay)
    {
        pBase       = base;
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
                int32_t used        = d->capacity();
                pDelay->pGDelay[i]  = NULL;
                d->destroy();
                delete d;
                atomic_add(&pBase->nMemUsed, -used); // Decrement the overall memory usage
            }

            if ((d = pDelay->pPDelay[i]) != NULL)
            {
                int32_t used        = d->capacity();
                pDelay->pPDelay[i] = NULL;
                d->destroy();
                delete d;
                atomic_add(&pBase->nMemUsed, -used); // Decrement the overall memory usage
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
            atomic_add(&pBase->nMemUsed, d->capacity()); // Increment the overall memory usage
        }

        return STATUS_OK;
    }

    art_delay_base::art_delay_base(const plugin_metadata_t &mdata, bool stereo_in): plugin_t(mdata)
    {
        bStereoIn       = stereo_in;
        bMono           = false;
        nMaxDelay       = 0;
        sOldDryPan[0].l = 0.0f;
        sOldDryPan[0].r = 0.0f;
        sOldDryPan[1].l = 0.0f;
        sOldDryPan[1].r = 0.0f;
        sNewDryPan[0].l = 0.0f;
        sNewDryPan[0].r = 0.0f;
        sNewDryPan[1].l = 0.0f;
        sNewDryPan[1].r = 0.0f;
        vOutBuf[0]      = NULL;
        vOutBuf[1]      = NULL;
        vGainBuf        = NULL;
        vDelayBuf       = NULL;
        vFeedBuf        = NULL;
        vTempBuf        = NULL;
        vTempo          = NULL;
        vDelays         = NULL;
        pExecutor       = NULL;
        nMemUsed        = 0;

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
        pDryOn          = NULL;
        pWetOn          = NULL;
        pMono           = NULL;
        pFeedback       = NULL;
        pFeedGain       = NULL;
        pOutGain        = NULL;
        pOutDMax        = NULL;
        pOutMemUse      = NULL;

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
        size_t sz_proc          = ALIGN_SIZE(sizeof(art_delay_t) * MAX_PROCESSORS, ALIGN64);
        size_t sz_alloc         = sz_tempo + sz_proc + sz_buf * 6;

        uint8_t *ptr            = alloc_aligned<uint8_t>(pData, sz_alloc, ALIGN64);
        if (ptr == NULL)
            return;

        // Allocate data buffers
        for (size_t i=0; i<2; ++i)
        {
            vOutBuf[i]              = reinterpret_cast<float *>(ptr);
            ptr                    += sz_buf;
        }

        vGainBuf                = reinterpret_cast<float *>(ptr);
        ptr                    += sz_buf;
        vDelayBuf               = reinterpret_cast<float *>(ptr);
        ptr                    += sz_buf;
        vFeedBuf                = reinterpret_cast<float *>(ptr);
        ptr                    += sz_buf;
        vTempBuf                = reinterpret_cast<float *>(ptr);
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

            at->pTempo              = NULL;
            at->pRatio              = NULL;
            at->pSync               = NULL;
            at->pOutTempo           = NULL;
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
            ad->sOutOfRange.construct();
            ad->sFeedOutRange.construct();

            ad->sEq[0].init(art_delay_base_metadata::EQ_BANDS + 2, 0);
            ad->sEq[1].init(art_delay_base_metadata::EQ_BANDS + 2, 0);
            ad->sEq[0].set_mode(EQM_IIR);
            ad->sEq[1].set_mode(EQM_IIR);

            ad->pAllocator          = new DelayAllocator(this, ad);
            if (ad->pAllocator == NULL)
                return;

            ad->bStereo             = bStereoIn;
            ad->bOn                 = false;
            ad->bSolo               = false;
            ad->bMute               = false;
            ad->bUpdated            = false;
            ad->bValidRef           = true;
            ad->nDelayRef           = DELAY_REF_NONE;
            ad->fOutDelay           = 0.0f;
            ad->fOutFeedback        = 0.0f;
            ad->fOutTempo           = 0.0f;
            ad->fOutDelayRef        = 0.0f;

            ad->sOld.fDelay         = 0.0f;
            ad->sOld.fFeedGain      = 0.0f;
            ad->sOld.fFeedLen       = 0.0f;
            if (bStereoIn)
            {
                ad->sOld.sPan[0].l      = 1.0f;
                ad->sOld.sPan[0].r      = 0.0f;
                ad->sOld.sPan[1].l      = 0.0f;
                ad->sOld.sPan[1].r      = 1.0f;
            }
            else
            {
                ad->sOld.sPan[0].l      = 0.5f;
                ad->sOld.sPan[0].r      = 0.5f;
                ad->sOld.sPan[1].l      = 0.5f;
                ad->sOld.sPan[1].r      = 0.5f;
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
            ad->pGain               = NULL;

            ad->pFeedOn             = NULL;
            ad->pFeedGain           = NULL;
            ad->pFeedTempoRef       = NULL;
            ad->pFeedBarFrac        = NULL;
            ad->pFeedBarDenom       = NULL;
            ad->pFeedBarMul         = NULL;
            ad->pFeedFrac           = NULL;
            ad->pFeedDenom          = NULL;
            ad->pFeedDelay          = NULL;

            ad->pOutDelay           = NULL;
            ad->pOutFeedback        = NULL;
            ad->pOutOfRange         = NULL;
            ad->pOutFeedRange       = NULL;
            ad->pOutLoop            = NULL;
            ad->pOutTempo           = NULL;
            ad->pOutDelayRef        = NULL;
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
        if (bStereoIn)
        {
            TRACE_PORT(vPorts[port_id]);
            pPan[1]         = vPorts[port_id++];
        }
        TRACE_PORT(vPorts[port_id]);
        pDryGain        = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pWetGain        = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pDryOn          = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pWetOn          = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pMono           = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pFeedback       = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pFeedGain       = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pOutGain        = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pOutDMax        = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pOutMemUse      = vPorts[port_id++];

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
            TRACE_PORT(vPorts[port_id]);
            at->pOutTempo           = vPorts[port_id++];
        }

        // Bind delay ports
        lsp_trace("Binding delay ports");
        for (size_t i=0; i<MAX_PROCESSORS; ++i)
        {
            art_delay_t *ad         = &vDelays[i];

            TRACE_PORT(vPorts[port_id]);
            ad->pOn                 = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            ad->pSolo               = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            ad->pMute               = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            ad->pDelayRef           = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            ad->pDelayMul           = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            ad->pTempoRef           = vPorts[port_id++];
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
            ad->pPan[0]             = vPorts[port_id++];
            if (ad->bStereo)
            {
                TRACE_PORT(vPorts[port_id]);
                ad->pPan[1]             = vPorts[port_id++];
            }
            TRACE_PORT(vPorts[port_id]);
            ad->pGain               = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            port_id++;              // Skip hue settings

            // Feedback
            TRACE_PORT(vPorts[port_id]);
            ad->pFeedOn             = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            ad->pFeedGain           = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            ad->pFeedTempoRef       = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            ad->pFeedBarFrac        = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            ad->pFeedBarDenom       = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            ad->pFeedBarMul         = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            ad->pFeedFrac           = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            ad->pFeedDenom          = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            ad->pFeedDelay          = vPorts[port_id++];

            TRACE_PORT(vPorts[port_id]);
            ad->pOutDelay           = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            ad->pOutFeedback        = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            ad->pOutOfRange         = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            ad->pOutFeedRange       = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            ad->pOutLoop            = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            ad->pOutTempo           = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            ad->pOutFeedTempo       = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            ad->pOutDelayRef        = vPorts[port_id++];
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
            ad->sOutOfRange.init(sr);
            ad->sFeedOutRange.init(sr);
        }
    }

    float art_delay_base::decode_ratio(size_t v)
    {
        return (v < sizeof(art_delay_ratio)/sizeof(float)) ? art_delay_ratio[v] : 1.0f;
    }

    size_t art_delay_base::decode_max_delay_value(size_t v)
    {
        float seconds = (v < sizeof(art_delay_max)/sizeof(uint8_t)) ? art_delay_max[v] : 1.0f;
        return seconds_to_samples(fSampleRate, seconds);
    }

    bool art_delay_base::check_delay_ref(art_delay_t *ad)
    {
        art_delay_t *list[MAX_PROCESSORS];
        size_t n  = 0;
        list[n++] = ad;

        while (ad->nDelayRef >= 0)
        {
            // Get referenced delay
            ad  = &vDelays[ad->nDelayRef];

            // Check that there is no reference already
            for (size_t i=0; i<n; ++i)
                if (list[i] == ad)
                    return false;

            // Append to list
            list[n++] = ad;
        }

        return true;
    }

    void art_delay_base::update_settings()
    {
        size_t channels     = (bStereoIn) ? 2 : 1;

        bool bypass         = pBypass->getValue() >= 0.5f;
        float g_out         = pOutGain->getValue();
        float dry           = (pDryOn->getValue() >= 0.5f) ? pDryGain->getValue() * g_out : 0.0f;
        float wet           = (pWetOn->getValue() >= 0.5f) ? pWetGain->getValue() * g_out : 0.0f;
        float fback         = (pFeedback->getValue() >= 0.5f) ? pFeedGain->getValue() : 0.0f;

        bMono               = pMono->getValue() >= 0.5f;
        nMaxDelay           = decode_max_delay_value(pMaxDelay->getValue());

        for (size_t i=0; i<channels; ++i)
        {
            sNewDryPan[i].l     = (100.0f - pPan[i]->getValue()) * 0.005f * dry;
            sNewDryPan[i].r     = (pPan[i]->getValue() + 100.0f) * 0.005f * dry;
        }

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
        for (size_t i=0, nupd=0; nupd < MAX_PROCESSORS; i = ((i+1) % MAX_PROCESSORS))
        {
            // Get the delay
            art_delay_t *ad         = &vDelays[i];
            if (ad->bUpdated)
                continue;

            // Get parent delay reference and check that it is updated
            art_delay_t *p_ad       = ((ad->bValidRef) && (ad->nDelayRef >= 0)) ? &vDelays[ad->nDelayRef] : NULL;
            if ((p_ad != NULL) && (!p_ad->bUpdated))
                continue;

            float delay             = seconds_to_samples(fSampleRate, ad->pDelay->getValue());
            float fbdelay           = seconds_to_samples(fSampleRate, ad->pFeedDelay->getValue());

            // Tempo reference
            ssize_t tempo_ref       = ad->pTempoRef->getValue() - 1.0f;
            if (tempo_ref >= 0)
            {
                // Compute bar * multiplier + fraction
                art_tempo_t *at         = &vTempo[tempo_ref];
                ad->fOutTempo           = at->fTempo;
                float bfrac             = ad->pBarFrac->getValue() * ad->pBarMul->getValue() + ad->pFrac->getValue();
                float bdelay            = (240.0f * bfrac) / ad->fOutTempo;
                delay                  += seconds_to_samples(fSampleRate, bdelay);
            }
            else
                ad->fOutTempo           = 0.0f;

            // Feedback tempo reference
            tempo_ref               = ad->pFeedTempoRef->getValue() - 1.0f;
            if (tempo_ref >= 0)
            {
                // Compute bar * multiplier + fraction
                art_tempo_t *at         = &vTempo[tempo_ref];
                ad->fOutFeedTempo       = at->fTempo;
                float bfrac             = ad->pFeedBarFrac->getValue() * ad->pFeedBarMul->getValue() + ad->pFeedFrac->getValue();
                float bdelay            = (240.0f * bfrac) / ad->fOutFeedTempo;
                fbdelay                += seconds_to_samples(fSampleRate, bdelay);
            }
            else
                ad->fOutFeedTempo       = 0.0f;

            // Parent delay reference
            if (p_ad != NULL)
            {
                ad->fOutDelayRef        = p_ad->sNew.fDelay;
                delay                  += ad->fOutDelayRef * ad->pDelayMul->getValue();
            }
            else
                ad->fOutDelayRef        = 0.0f;

            // Update delay settings
            float gain              = ad->pGain->getValue() * wet;
            ad->sNew.fDelay         = delay;
            ad->sNew.fFeedGain      = (ad->pFeedOn->getValue() >= 0.5f) ? fback * ad->pFeedGain->getValue() : 0.0f;
            ad->sNew.fFeedLen       = fbdelay;

            for (size_t j=0; j<channels; ++j)
            {
                ad->sNew.sPan[j].l      = (100.0f - ad->pPan[j]->getValue()) * 0.005f * gain;
                ad->sNew.sPan[j].r      = (ad->pPan[j]->getValue() + 100.0f) * 0.005f * gain;
            }

            ad->fOutDelay           = samples_to_seconds(fSampleRate, delay);

            // Determine mode
            bool eq_on          = ad->pEqOn->getValue() >= 0.5f;
            bool low_on         = ad->pLcfOn->getValue() >= 0.5f;
            bool high_on        = ad->pHcfOn->getValue() >= 0.5f;
            bool xbypass        = (bypass) || (ad->bMute) || ((has_solo) && (!ad->bSolo));
            equalizer_mode_t eq_mode = (eq_on || low_on || high_on) ? EQM_IIR : EQM_BYPASS;

            // Update processor settings
            for (size_t j=0; j<channels; ++j)
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

            // Mark delay as updated
            ad->bUpdated    = true;
            ++nupd;
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
            if (ad->bOn)
            {
                bool gc = false;

                for (size_t i=0; i < channels; ++i)
                {
                    // There is data to commit?
                    if (ad->pPDelay[i] == NULL)
                        continue;

                    // Copy delay data if it is present
                    if (ad->pCDelay[i] != NULL)
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
    }

    void art_delay_base::process_delay(art_delay_t *ad, float **out, const float * const *in, size_t samples, size_t off, size_t count)
    {
        float dmax, fbmax;

        // Create delay control signal if it is changing slowly
        if ((ad->sOld.fDelay != ad->sNew.fDelay) &&
            (fabs(ad->sOld.fDelay - ad->sNew.fDelay)*0.25f <= samples))
        {
            dsp::lin_inter_set(vDelayBuf, 0, ad->sOld.fDelay, samples, ad->sNew.fDelay, off, count);
            dmax = lsp_max(vDelayBuf[0], vDelayBuf[count-1]);
        }
        else
        {
            dsp::fill(vDelayBuf, ad->sNew.fDelay, count);
            dmax = ad->sNew.fDelay;
        }

        // Create feedback delay control signal if it is changing slowly
        if ((ad->sOld.fFeedLen != ad->sNew.fFeedLen) &&
            (fabs(ad->sOld.fFeedLen - ad->sNew.fFeedLen)*0.25f <= samples))
        {
            dsp::lin_inter_set(vFeedBuf, 0, ad->sOld.fFeedLen, samples, ad->sNew.fFeedLen, off, count);
            fbmax = lsp_max(vFeedBuf[0], vFeedBuf[count-1]);
        }
        else
        {
            dsp::fill(vFeedBuf, ad->sNew.fFeedLen, count);
            fbmax = ad->sNew.fFeedLen;
        }

        // Process the feedback signal and check that it is not out of range
        ad->fOutFeedback    = samples_to_seconds(fSampleRate, fbmax);
        if ((fbmax > nMaxDelay) || (fbmax > dmax))
            ad->sFeedOutRange.blink(); // Indicate out of range

        // Check if there is nothing to do
        if (!ad->bOn)
            return;
        size_t channels = (ad->bStereo) ? 2 : 1;
        for (size_t i=0; i<channels; ++i)
            if (ad->pCDelay[i] == NULL)
                return;

        // Create feedback gain control signal
        if (ad->sOld.fFeedGain != ad->sNew.fFeedGain)
            dsp::lin_inter_set(vGainBuf, 0, ad->sOld.fFeedGain, samples, ad->sNew.fFeedGain, off, count);
        else
            dsp::fill(vGainBuf, ad->sOld.fFeedGain, count);

        for (size_t i=0; i<channels; ++i)
        {
            // Process the delay -> eq -> bypass chain
            ad->pCDelay[i]->process(vTempBuf, in[i], vDelayBuf, vGainBuf, vFeedBuf, count);
            ad->sEq[i].process(vTempBuf, vTempBuf, count);
            ad->sBypass[i].process(vTempBuf, NULL, vTempBuf, count);

            // Pan the output
            if (ad->sOld.sPan[i].l != ad->sNew.sPan[i].l)
            {
                dsp::lin_inter_fmadd2(out[0], vTempBuf, 0, ad->sOld.sPan[i].l, samples, ad->sNew.sPan[i].l, off, count);
                dsp::lin_inter_fmadd2(out[1], vTempBuf, 0, ad->sOld.sPan[i].r, samples, ad->sNew.sPan[i].r, off, count);
            }
            else
            {
                dsp::fmadd_k3(out[0], vTempBuf, ad->sOld.sPan[i].l, count);
                dsp::fmadd_k3(out[1], vTempBuf, ad->sOld.sPan[i].r, count);
            }
        }
    }

    void art_delay_base::process(size_t samples)
    {
        // Estimate number of channels
        size_t channels = (bStereoIn) ? 2 : 1;

        // Sync delay lines
        for (size_t j=0; j<MAX_PROCESSORS; ++j)
        {
            art_delay_t *ad     = &vDelays[j];
            sync_delay(ad);
        }

        // Bind ports
        float *in[2], *out[2];

        in[0]   = pIn[0]->getBuffer<float>();
        in[1]   = (bStereoIn) ? pIn[1]->getBuffer<float>() : in[0];

        out[0]  = pOut[0]->getBuffer<float>();
        out[1]  = pOut[1]->getBuffer<float>();

        // Process audio signal
        for (size_t i=0; i<samples; )
        {
            // How many samples we can process at one time?
            size_t count        = lsp_min(samples - i, BUFFER_SIZE);

            // Process the dry sound (gain + pan)
            dsp::fill_zero(vOutBuf[0], count);
            dsp::fill_zero(vOutBuf[1], count);

            for (size_t j=0; j<channels; ++j)
            {
                if (sOldDryPan[j].l != sNewDryPan[j].l)
                {
                    dsp::lin_inter_fmadd2(vOutBuf[0], in[j], 0, sOldDryPan[j].l, samples, sNewDryPan[j].l, i, count);
                    dsp::lin_inter_fmadd2(vOutBuf[1], in[j], 0, sOldDryPan[j].r, samples, sNewDryPan[j].r, i, count);
                }
                else
                {
                    dsp::fmadd_k3(vOutBuf[0], in[j], sOldDryPan[j].l, count);
                    dsp::fmadd_k3(vOutBuf[1], in[j], sOldDryPan[j].r, count);
                }
            }

            // Process all delay channels and store result to vDataBuf
            for (size_t j=0; j<MAX_PROCESSORS; ++j)
                process_delay(&vDelays[j], vOutBuf, in, samples, i, count);

            // Output internal buffer data to external outputs via applied bypass

            if (bMono)
            {
                dsp::lr_to_mid(vOutBuf[0], vOutBuf[0], vOutBuf[1], count);
                sBypass[0].process(out[0], in[0], vOutBuf[0], count);
                sBypass[1].process(out[1], in[1], vOutBuf[0], count);
            }
            else
            {
                sBypass[0].process(out[0], in[0], vOutBuf[0], count);
                sBypass[1].process(out[1], in[1], vOutBuf[1], count);
            }

            // Update positions
            in[0]              += count;
            in[1]              += count;
            out[0]             += count;
            out[1]             += count;
            i                  += count;
        }

        // Commit dynamic settings and output values
        sOldDryPan[0]       = sNewDryPan[0];
        sOldDryPan[1]       = sNewDryPan[1];

        for (size_t j=0; j<MAX_TEMPOS; ++j)
        {
            art_tempo_t *at     = &vTempo[j];
            at->pOutTempo->setValue(at->fTempo);
        }

        for (size_t j=0; j<MAX_PROCESSORS; ++j)
        {
            art_delay_t *ad     = &vDelays[j];
            ad->sOld            = ad->sNew;

            // Update blink state
            if (ad->sNew.fDelay > nMaxDelay)
                ad->sOutOfRange.blink();

            // Output values
            ad->pOutDelay->setValue(ad->fOutDelay);
            ad->pOutFeedback->setValue(ad->fOutFeedback);
            ad->pOutDelayRef->setValue(samples_to_seconds(fSampleRate, ad->fOutDelayRef));
            ad->pOutTempo->setValue(ad->fOutTempo);
            ad->pOutFeedTempo->setValue(ad->fOutFeedTempo);
            ad->pOutOfRange->setValue(ad->sOutOfRange.value());
            ad->pOutFeedRange->setValue(ad->sFeedOutRange.value());
            ad->pOutLoop->setValue((ad->bValidRef) ? 0.0f : 1.0f);

            // Post-process blink
            ad->sOutOfRange.process(samples);
            ad->sFeedOutRange.process(samples);
        }

        float used = nMemUsed;
        pOutDMax->setValue(samples_to_seconds(fSampleRate, nMaxDelay));
        pOutMemUse->setValue((used / (1024.0f * 1024.0f)) * sizeof(float)); // Translate floats into megabytes
    }

    void art_delay_base::dump_pan(IStateDumper *v, const char *name, const pan_t *pan, size_t n)
    {
        v->begin_array(name, pan, n);
        {
            for (size_t i=0; i<n; ++i)
            {
                const pan_t *p = &pan[i];
                v->begin_object(p, sizeof(pan_t));
                {
                    v->write("l", p->l);
                    v->write("r", p->r);
                }
                v->end_object();
            }
        }
        v->end_array();
    }

    void art_delay_base::dump_art_settings(IStateDumper *v, const char *name, const art_settings_t *as)
    {
        v->begin_object(name, as, sizeof(art_settings_t));
        {
            v->write("fDelay", as->fDelay);
            v->write("fFeedGain", as->fFeedGain);
            v->write("fFeedLen", as->fFeedLen);
            dump_pan(v, "sPan", as->sPan, 2);
            v->write("nMaxDelay", as->nMaxDelay);
        }
        v->end_object();
    }

    void art_delay_base::dump_art_tempo(IStateDumper *v, const art_tempo_t *at)
    {
        v->begin_object(at, sizeof(art_tempo_t));
        {
            v->write("fTempo", at->fTempo);
            v->write("bSync", at->bSync);
            v->write("pTempo", at->pTempo);
            v->write("pRatio", at->pRatio);
            v->write("pSync", at->pSync);
            v->write("pOutTempo", at->pOutTempo);
        }
        v->end_object();
    }

    void art_delay_base::dump_art_delay(IStateDumper *v, const art_delay_t *ad)
    {
        v->begin_object(ad, sizeof(art_delay_t));
        {
            v->begin_array("pPDelay", ad->pPDelay, 2);
            {
                v->write_object(ad->pPDelay[0]);
                v->write_object(ad->pPDelay[1]);
            }
            v->end_array();

            v->begin_array("pCDelay", ad->pCDelay, 2);
            {
                v->write_object(ad->pCDelay[0]);
                v->write_object(ad->pCDelay[1]);
            }
            v->end_array();

            v->begin_array("pGDelay", ad->pGDelay, 2);
            {
                v->write_object(ad->pGDelay[0]);
                v->write_object(ad->pGDelay[1]);
            }
            v->end_array();

            v->begin_array("sEq", ad->sEq, 2);
            {
                v->write_object(&ad->sEq[0]);
                v->write_object(&ad->sEq[1]);
            }
            v->end_array();

            v->begin_array("sBypass", ad->sBypass, 2);
            {
                v->write_object(&ad->sBypass[0]);
                v->write_object(&ad->sBypass[1]);
            }
            v->end_array();

            v->write_object("sOutOfRange", &ad->sOutOfRange);
            v->write_object("sFeedOutRange", &ad->sFeedOutRange);
            v->write("pAllocator", &ad->pAllocator);

            v->write("bStereo", ad->bStereo);
            v->write("bOn", ad->bOn);
            v->write("bSolo", ad->bSolo);
            v->write("bMute", ad->bMute);
            v->write("bUpdated", ad->bUpdated);
            v->write("bValidRef", ad->bValidRef);
            v->write("nDelayRef", ad->nDelayRef);
            v->write("fOutDelay", ad->fOutDelay);
            v->write("fOutFeedback", ad->fOutFeedback);
            v->write("fOutTempo", ad->fOutTempo);
            v->write("fOutFeedTempo", ad->fOutFeedTempo);
            v->write("fOutDelayRef", ad->fOutDelayRef);

            dump_art_settings(v, "sOld", &ad->sOld);
            dump_art_settings(v, "sNew", &ad->sNew);

            v->write("pOn", ad->pOn);
            v->write("pTempoRef", ad->pTempoRef);
            v->writev("pPan", ad->pPan, 2);
            v->write("pSolo", ad->pSolo);
            v->write("pMute", ad->pMute);
            v->write("pDelayRef", ad->pDelayRef);
            v->write("pDelayMul", ad->pDelayMul);
            v->write("pBarFrac", ad->pBarFrac);
            v->write("pBarMul", ad->pBarMul);
            v->write("pFrac", ad->pFrac);
            v->write("pDenom", ad->pDenom);
            v->write("pDelay", ad->pDelay);
            v->write("pEqOn", ad->pEqOn);
            v->write("pLcfOn", ad->pLcfOn);
            v->write("pLcfFreq", ad->pLcfFreq);
            v->write("pHcfOn", ad->pHcfOn);
            v->write("pHcfFreq", ad->pHcfFreq);
            v->writev("pBandGain", ad->pBandGain, EQ_BANDS);
            v->write("pGain", ad->pGain);

            v->write("pFeedOn", ad->pFeedOn);
            v->write("pFeedGain", ad->pFeedGain);
            v->write("pFeedTempoRef", ad->pFeedTempoRef);
            v->write("pFeedBarFrac", ad->pFeedBarFrac);
            v->write("pFeedBarDenom", ad->pFeedBarDenom);
            v->write("pFeedBarMul", ad->pFeedBarMul);
            v->write("pFeedFrac", ad->pFeedFrac);
            v->write("pFeedDenom", ad->pFeedDenom);
            v->write("pFeedDelay", ad->pFeedDelay);

            v->write("pOutDelay", ad->pOutDelay);
            v->write("pOutFeedback", ad->pOutFeedback);
            v->write("pOutOfRange", ad->pOutOfRange);
            v->write("pOutFeedRange", ad->pOutFeedRange);
            v->write("pOutLoop", ad->pOutLoop);
            v->write("pOutTempo", ad->pOutTempo);
            v->write("pOutFeedTempo", ad->pOutFeedTempo);
            v->write("pOutDelayRef", ad->pOutDelayRef);
        }
        v->end_object();
    }

    void art_delay_base::dump(IStateDumper *v) const
    {
        v->write("bStereoIn", bStereoIn);
        v->write("bMono", bMono);
        v->write("nMaxDelay", nMaxDelay);
        dump_pan(v, "sOldDryPan", sOldDryPan, 2);
        dump_pan(v, "sNewDryPan", sNewDryPan, 2);
        v->writev("vOutBuf", vOutBuf, 2);
        v->write("vGainBuf", vGainBuf);
        v->write("vDelayBuf", vDelayBuf);
        v->write("vFeedBuf", vFeedBuf);
        v->write("vTempBuf", vTempBuf);
        v->begin_array("vTempo", vTempo, MAX_TEMPOS);
        {
            for (size_t i=0; i<MAX_TEMPOS; ++i)
                dump_art_tempo(v, &vTempo[i]);
        }
        v->end_array();
        v->begin_array("vDelays", vDelays, MAX_PROCESSORS);
        {
            for (size_t i=0; i<MAX_PROCESSORS; ++i)
                dump_art_delay(v, &vDelays[i]);
        }
        v->end_array();
        v->write("nMemUsed", nMemUsed);

        v->begin_array("sBypass", sBypass, 2);
        {
            v->write_object(&sBypass[0]);
            v->write_object(&sBypass[1]);
        }
        v->end_array();
        v->write("pExecutor", pExecutor);


        v->writev("pIn", pIn, 2);
        v->writev("pOut", pOut, 2);
        v->write("pBypass", pBypass);
        v->write("pMaxDelay", pMaxDelay);
        v->writev("pPan", pPan, 2);
        v->write("pDryGain", pDryGain);
        v->write("pWetGain", pWetGain);
        v->write("pDryOn", pDryOn);
        v->write("pWetOn", pWetOn);
        v->write("pMono", pMono);
        v->write("pFeedback", pFeedback);
        v->write("pFeedGain", pFeedGain);
        v->write("pOutGain", pOutGain);
        v->write("pOutDMax", pOutDMax);
        v->write("pOutMemUse", pOutMemUse);

        v->write("pData", pData);
    }

    art_delay_mono::art_delay_mono(): art_delay_base(metadata, false)
    {
    }

    art_delay_stereo::art_delay_stereo(): art_delay_base(metadata, true)
    {
    }
}



