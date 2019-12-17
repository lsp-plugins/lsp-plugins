/*
 * slap_delay.cpp
 *
 *  Created on: 30 янв. 2017 г.
 *      Author: sadko
 */

#include <plugins/slap_delay.h>
#include <core/debug.h>

#define BUFFER_SIZE     4096
#define CONV_RANK       10

#define TRACE_PORT(p)           lsp_trace("  port id=%s", (p)->metadata()->id);

namespace lsp
{
    static float band_freqs[] =
    {
        60.0f,
        300.0f,
        1000.0f,
        6000.0f
    };

    //-------------------------------------------------------------------------
    // Slap-back delay base class

    slap_delay_base::slap_delay_base(const plugin_metadata_t &mdata, bool stereo_in): plugin_t(mdata)
    {
        nInputs         = (stereo_in) ? 2 : 1;
        vInputs         = NULL;

        vTemp           = NULL;
        bMono           = false;

        pBypass         = NULL;
        pTemp           = NULL;
        pDry            = NULL;
        pWet            = NULL;
        pDryMute        = NULL;
        pWetMute        = NULL;
        pOutGain        = NULL;
        pMono           = NULL;
        pPred           = NULL;
        pStretch        = NULL;
        pTempo          = NULL;
        pSync           = NULL;
        pRamping        = NULL;

        vData           = NULL;
    }

    slap_delay_base::~slap_delay_base()
    {
        destroy();
    }

    void slap_delay_base::init(IWrapper *wrapper)
    {
        plugin_t::init(wrapper);

        // Allocate inputs
        vInputs         = new input_t[nInputs];
        if (vInputs == NULL)
            return;

        // Allocate buffers
        size_t alloc    = BUFFER_SIZE * 3;
        vData           = new uint8_t[alloc * sizeof(float) + DEFAULT_ALIGN];
        if (vData == NULL)
            return;
        float *ptr      = reinterpret_cast<float *>(ALIGN_PTR(vData, DEFAULT_ALIGN));

        // Remember pointers
        vTemp           = ptr;
        ptr            += BUFFER_SIZE;

        // Initialize inputs
        for (size_t i=0; i<nInputs; ++i)
        {
            vInputs[i].vIn      = NULL;
            vInputs[i].pIn      = NULL;
            vInputs[i].pPan     = NULL;
        }

        // Initialize channels
        for (size_t i=0; i<2; ++i)
        {
            channel_t *c        = &vChannels[i];

            c->vOut             = NULL;
            c->pOut             = NULL;
            c->vRender          = ptr;
            ptr                += BUFFER_SIZE;
        }

        for (size_t i=0; i<slap_delay_base_metadata::MAX_PROCESSORS; ++i)
        {
            processor_t    *p   = &vProcessors[i];

            p->nDelay           = 0;
            p->nNewDelay        = 0;
            p->nMode            = M_OFF;

            p->pMode            = NULL;
            p->pTime            = NULL;
            p->pDistance        = NULL;
            p->pPan[0]          = NULL;
            p->pPan[1]          = NULL;
            p->pGain            = NULL;
            p->pLowCut          = NULL;
            p->pLowFreq         = NULL;
            p->pHighCut         = NULL;
            p->pHighFreq        = NULL;
            p->pSolo            = NULL;
            p->pMute            = NULL;
            p->pPhase           = NULL;

            for (size_t j=0; j<slap_delay_base_metadata::EQ_BANDS; ++j)
                p->pFreqGain[j]     = NULL;

            for (size_t j=0; j<2; ++j)
            {
                p->vDelay[j].sEqualizer.init(slap_delay_base_metadata::EQ_BANDS + 2, CONV_RANK);
                p->vDelay[j].sEqualizer.set_mode(EQM_IIR);
            }
        }

        lsp_assert(ptr <= reinterpret_cast<float *>(&vData[alloc * sizeof(float) + DEFAULT_ALIGN]));

        // Bind ports
        size_t port_id = 0;

        lsp_trace("Binding audio ports");
        for (size_t i=0; i<nInputs; ++i)
        {
            TRACE_PORT(vPorts[port_id]);
            vInputs[i].pIn  = vPorts[port_id++];
        }
        for (size_t i=0; i<2; ++i)
        {
            TRACE_PORT(vPorts[port_id]);
            vChannels[i].pOut   = vPorts[port_id++];
        }

        // Bind common ports
        lsp_trace("Binding common ports");
        TRACE_PORT(vPorts[port_id]);
        pBypass         = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        port_id ++;     // Skip delay selector
        TRACE_PORT(vPorts[port_id]);
        pTemp           = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pPred           = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pStretch        = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pTempo          = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pSync           = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pRamping        = vPorts[port_id++];

        for (size_t i=0; i<nInputs; ++i)
        {
            TRACE_PORT(vPorts[port_id]);
            vInputs[i].pPan     = vPorts[port_id++];
        }

        TRACE_PORT(vPorts[port_id]);
        pDry            = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pDryMute        = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pWet            = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pWetMute        = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pMono           = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pOutGain        = vPorts[port_id++];

        // Bind processor ports
        for (size_t i=0; i<slap_delay_base_metadata::MAX_PROCESSORS; ++i)
        {
            processor_t *p      = &vProcessors[i];

            TRACE_PORT(vPorts[port_id]);
            p->pMode            = vPorts[port_id++];
            for (size_t j=0; j<nInputs; ++j)
            {
                TRACE_PORT(vPorts[port_id]);
                p->pPan[j]          = vPorts[port_id++];
            }
            TRACE_PORT(vPorts[port_id]);
            p->pSolo            = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            p->pMute            = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            p->pPhase           = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            p->pTime            = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            p->pDistance        = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            p->pFrac            = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            p->pDenom           = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            p->pEq              = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            p->pLowCut          = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            p->pLowFreq         = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            p->pHighCut         = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            p->pHighFreq        = vPorts[port_id++];

            for (size_t j=0; j<slap_delay_base_metadata::EQ_BANDS; ++j)
            {
                TRACE_PORT(vPorts[port_id]);
                p->pFreqGain[j]     = vPorts[port_id++];
            }

            TRACE_PORT(vPorts[port_id]);
            p->pGain            = vPorts[port_id++];
        }
    }

    void slap_delay_base::destroy()
    {
        if (vInputs != NULL)
        {
            // Destroy shift buffers
            for (size_t i=0; i<nInputs; ++i)
                vInputs[i].sBuffer.destroy();

            delete [] vInputs;
            vInputs = NULL;
        }

        for (size_t i=0; i<slap_delay_base_metadata::MAX_PROCESSORS; ++i)
        {
            processor_t *c      = &vProcessors[i];
            c->vDelay[0].sEqualizer.destroy();
            c->vDelay[1].sEqualizer.destroy();
        }

        if (vData != NULL)
        {
            delete [] vData;
            vData       = NULL;
        }

        vTemp       = NULL;
    }

    bool slap_delay_base::set_position(const position_t *pos)
    {
        return pos->beatsPerMinute != pWrapper->position()->beatsPerMinute;
    }

    void slap_delay_base::update_settings()
    {
        float out_gain      = pOutGain->getValue();
        float dry_gain      = (pDryMute->getValue() >= 0.5f) ? 0.0f : pDry->getValue() * out_gain;
        float wet_gain      = (pWetMute->getValue() >= 0.5f) ? 0.0f : pWet->getValue() * out_gain;
        float d_delay       = 1.0f / sound_speed(pTemp->getValue()); // 1 / ss [m/s] = d_delay [s/m]
        float p_delay       = pPred->getValue(); // Pre-delay value
        float stretch       = pStretch->getValue() * 0.01;
        bool bypass         = pBypass->getValue() >= 0.5f;
        bool has_solo       = false;
        bMono               = pMono->getValue() >= 0.5f;
        bool ramping        = pRamping->getValue() >= 0.5f;

        for (size_t i=0; i<2; ++i)
            vChannels[i].sBypass.set_bypass(bypass);

        // Check that solo is enabled
        for (size_t i=0; i<slap_delay_base_metadata::MAX_PROCESSORS; ++i)
            if (vProcessors[i].pSolo->getValue() >= 0.5f)
            {
                has_solo        = true;
                break;
            }

        if (nInputs == 1)
        {
            float pan               = vInputs[0].pPan->getValue();
            vChannels[0].fGain[0]   = (100.0f - pan) * 0.005f * dry_gain;
            vChannels[0].fGain[1]   = 0.0f;
            vChannels[1].fGain[0]   = (100.0f + pan) * 0.005f * dry_gain;
            vChannels[1].fGain[1]   = 0.0f;
        }
        else
        {
            float pan_l             = vInputs[0].pPan->getValue();
            float pan_r             = vInputs[1].pPan->getValue();

            vChannels[0].fGain[0]   = (100.0f - pan_l) * 0.005f * dry_gain;
            vChannels[0].fGain[1]   = (100.0f - pan_r) * 0.005f * dry_gain;
            vChannels[1].fGain[0]   = (100.0f + pan_l) * 0.005f * dry_gain;
            vChannels[1].fGain[1]   = (100.0f + pan_r) * 0.005f * dry_gain;
        }

        for (size_t i=0; i<slap_delay_base_metadata::MAX_PROCESSORS; ++i)
        {
            processor_t *p      = &vProcessors[i];

            // Calculate delay gain
            float delay_gain    = (p->pMute->getValue() >= 0.5f) ? 0.0f : p->pGain->getValue() * wet_gain;
            if ((has_solo) && (p->pSolo->getValue() < 0.5f))
                delay_gain          = 0.0f;
            if (p->pPhase->getValue() >= 0.5f)
                delay_gain          = -delay_gain;

            // Apply panning parameters
            if (nInputs == 1)
            {
                float pan               = p->pPan[0]->getValue();
                p->vDelay[0].fGain[0]   = ((100.0f - pan) * 0.005f) * delay_gain;
                p->vDelay[0].fGain[1]   = 0.0f;
                p->vDelay[1].fGain[0]   = ((100.0f + pan) * 0.005f) * delay_gain;
                p->vDelay[1].fGain[1]   = 0.0f;
            }
            else
            {
                float pan_l             = p->pPan[0]->getValue();
                float pan_r             = p->pPan[1]->getValue();

                p->vDelay[0].fGain[0]   = (100.0f - pan_l) * 0.005f * delay_gain;
                p->vDelay[0].fGain[1]   = (100.0f - pan_r) * 0.005f * delay_gain;
                p->vDelay[1].fGain[0]   = (100.0f + pan_l) * 0.005f * delay_gain;
                p->vDelay[1].fGain[1]   = (100.0f + pan_r) * 0.005f * delay_gain;
            }

            // Determine mode
            bool eq_on          = p->pEq->getValue() >= 0.5f;
            bool low_on         = p->pLowCut->getValue() >= 0.5f;
            bool high_on        = p->pHighCut->getValue() >= 0.5f;
            equalizer_mode_t eq_mode = (eq_on || low_on || high_on) ? EQM_IIR : EQM_BYPASS;
            p->nMode            = p->pMode->getValue();

            if (p->nMode == slap_delay_base_metadata::OP_MODE_TIME)
                p->nNewDelay        = millis_to_samples(fSampleRate, p->pTime->getValue() * stretch + p_delay);
            else if (p->nMode == slap_delay_base_metadata::OP_MODE_DISTANCE)
                p->nNewDelay        = seconds_to_samples(fSampleRate, p->pDistance->getValue() * d_delay * stretch + p_delay * 0.001f);
            else if (p->nMode == slap_delay_base_metadata::OP_MODE_NOTE)
            {
                float tempo         = (pSync->getValue() >= 0.5f) ? pWrapper->position()->beatsPerMinute : pTempo->getValue();
                if (tempo < slap_delay_base_metadata::TEMPO_MIN)
                    tempo               = slap_delay_base_metadata::TEMPO_MIN;
                else if (tempo > slap_delay_base_metadata::TEMPO_MAX)
                    tempo               = slap_delay_base_metadata::TEMPO_MAX;

                float delay         = (240.0f * p->pFrac->getValue()) / tempo;
                p->nNewDelay        = seconds_to_samples(fSampleRate, delay * stretch + p_delay * 0.001f);
            }
            else
                p->nNewDelay        = 0;

            if (!ramping)
                p->nDelay           = p->nNewDelay;

            lsp_trace("p[%d].nDelay     = %d", int(i), int(p->nDelay));
            lsp_trace("p[%d].nNewDelay  = %d", int(i), int(p->nNewDelay));

            // Update equalizer settings
            for (size_t j=0; j<2; ++j)
            {
                // Update equalizer
                Equalizer *eq   = &p->vDelay[j].sEqualizer;
                eq->set_mode(eq_mode);

                if (eq_mode == EQM_BYPASS)
                    continue;

                filter_params_t fp;
                size_t band     = 0;

                // Set-up parametric equalizer
                while (band < slap_delay_base_metadata::EQ_BANDS)
                {
                    if (band == 0)
                    {
                        fp.fFreq        = band_freqs[band];
                        fp.fFreq2       = fp.fFreq;
                        fp.nType        = (eq_on) ? FLT_MT_LRX_LOSHELF : FLT_NONE;
                    }
                    else if (band == (slap_delay_base_metadata::EQ_BANDS - 1))
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

                    fp.fGain        = p->pFreqGain[band]->getValue();
                    fp.nSlope       = 2;
                    fp.fQuality     = 0.0f;

                    // Update filter parameters
                    eq->set_params(band++, &fp);
                }

                // Setup hi-pass filter
                fp.nType        = (low_on) ? FLT_BT_BWC_HIPASS : FLT_NONE;
                fp.fFreq        = p->pLowFreq->getValue();
                fp.fFreq2       = fp.fFreq;
                fp.fGain        = 1.0f;
                fp.nSlope       = 4;
                fp.fQuality     = 0.0f;
                eq->set_params(band++, &fp);

                // Setup low-pass filter
                fp.nType        = (high_on) ? FLT_BT_BWC_LOPASS : FLT_NONE;
                fp.fFreq        = p->pHighFreq->getValue();
                fp.fFreq2       = fp.fFreq;
                fp.fGain        = 1.0f;
                fp.nSlope       = 4;
                fp.fQuality     = 0.0f;
                eq->set_params(band++, &fp);
            }
        }
    }

    void slap_delay_base::update_sample_rate(long sr)
    {
        // Calculate maximum possible delay
        float stretch_max   = slap_delay_base_metadata::STRETCH_MAX * 0.01f;
        float time_max      = slap_delay_base_metadata::TIME_MAX;
        float dist_max      = slap_delay_base_metadata::DISTANCE_MAX / sound_speed(slap_delay_base_metadata::TEMPERATURE_MIN);
        float tempo_max     = (240.0f * slap_delay_base_metadata::FRACTION_MAX) / slap_delay_base_metadata::TEMPO_MIN; // time per FRACTION_MAX whole notes

        size_t max_delay    = millis_to_samples(sr, time_max * stretch_max + slap_delay_base_metadata::PRED_TIME_MAX);
        size_t dist_delay   = seconds_to_samples(sr, dist_max * stretch_max + slap_delay_base_metadata::PRED_TIME_MAX * 0.001f);
        size_t tempo_delay  = seconds_to_samples(sr, tempo_max * stretch_max + slap_delay_base_metadata::PRED_TIME_MAX * 0.001f);
        lsp_trace("max_delay = %d, dist_delay=%d, tempo_delay=%d", int(max_delay), int(dist_delay), int(tempo_delay));
        if (max_delay < dist_delay)
            max_delay           = dist_delay;
        if (max_delay < tempo_delay)
            max_delay           = tempo_delay;
        lsp_trace("max_delay (final) = %d", int(max_delay));

        // Initialize buffers and fill them with zeros
        for (size_t i=0; i<nInputs; ++i)
        {
            vInputs[i].sBuffer.init(max_delay * 2, max_delay);
            vInputs[i].sBuffer.fill(0.0f);
        }

        for (size_t i=0; i<slap_delay_base_metadata::MAX_PROCESSORS; ++i)
        {
            processor_t *p      = &vProcessors[i];
            p->vDelay[0].sEqualizer.set_sample_rate(sr);
            p->vDelay[1].sEqualizer.set_sample_rate(sr);
        }

        // Initialize output channels
        for (size_t i=0; i<2; ++i)
            vChannels[i].sBypass.init(sr);
    }

    void slap_delay_base::process(size_t samples)
    {
        // Prepare inputs and outputs
        for (size_t i=0; i<nInputs; ++i)
            vInputs[i].vIn      = vInputs[i].pIn->getBuffer<float>();
        for (size_t i=0; i<2; ++i)
            vChannels[i].vOut   = vChannels[i].pOut->getBuffer<float>();

        // Do processing
        for (size_t k=0; k < samples; )
        {
            // Process input data
            size_t to_do        = samples - k;
            if (to_do > BUFFER_SIZE)
                to_do               = BUFFER_SIZE;
            to_do               = vInputs[0].sBuffer.append(vInputs[0].vIn, to_do);

            if (nInputs > 1)
                vInputs[1].sBuffer.append(vInputs[1].vIn, to_do); // Buffer has the same gap, nothing to worry about to_do

            // Process each channel individually
            for (size_t i=0; i<2; ++i)
            {
                channel_t *c        = &vChannels[i];

                // Copy dry data to rendering buffer
                if (nInputs == 1)
                    dsp::mul_k3(c->vRender, vInputs[0].vIn, c->fGain[0], to_do);
                else
                    dsp::mix_copy2(c->vRender, vInputs[0].vIn, vInputs[1].vIn, c->fGain[0], c->fGain[1], to_do);

                // Do job with processors
                for (size_t j=0; j<slap_delay_base_metadata::MAX_PROCESSORS; ++j)
                {
                    // Skip processor if it is turned off
                    processor_t *p      = &vProcessors[j];
                    if (p->nMode == slap_delay_base_metadata::OP_MODE_NONE)
                        continue;

                    if (p->nNewDelay == p->nDelay)
                    {
                        // Copy delayed signal to buffer and apply panoraming
                        size_t delay        = p->nDelay + to_do;
                        if (nInputs == 1)
                            dsp::mul_k3(vTemp, vInputs[0].sBuffer.tail(delay), p->vDelay[i].fGain[0], to_do);
                        else
                            dsp::mix_copy2(vTemp, vInputs[0].sBuffer.tail(delay), vInputs[1].sBuffer.tail(delay), p->vDelay[i].fGain[0], p->vDelay[i].fGain[1], to_do);
                    }
                    else
                    {
                        // More complicated algorithm with ramping
                        float delta = (float(p->nNewDelay) - float(p->nDelay))/float(samples);

                        if (nInputs == 1)
                        {
                            float g0 = p->vDelay[i].fGain[0];
                            const float *s0 = vInputs[0].sBuffer.tail(to_do);

                            for (size_t n=0; n < to_do; ++n, ++s0)
                            {
                                ssize_t d = p->nDelay + delta * (k + n);
                                vTemp[n] = s0[-d] * g0;
                            }
                        }
                        else
                        {
                            float g0 = p->vDelay[i].fGain[0];
                            float g1 = p->vDelay[i].fGain[1];

                            const float *s0 = vInputs[0].sBuffer.tail(to_do);
                            const float *s1 = vInputs[1].sBuffer.tail(to_do);

                            for (size_t n=0; n < to_do; ++n, ++s0, ++s1)
                            {
                                ssize_t d = p->nDelay + delta * (k + n);
                                vTemp[n] = s0[-d] * g0 + s1[-d] * g1;
                            }
                        }
                    }

                    // Process data with equalizer
                    p->vDelay[i].sEqualizer.process(vTemp, vTemp, to_do);

                    // Alright, append temporary buffer to render buffer
                    dsp::add2(c->vRender, vTemp, to_do);
                }
            }

            // Make output monophonic
            if (bMono)
            {
                dsp::lr_to_mid(vChannels[0].vRender, vChannels[0].vRender, vChannels[1].vRender, to_do);
                dsp::copy(vChannels[1].vRender, vChannels[0].vRender, to_do);
            }

            // Process each channel individually
            for (size_t i=0; i<2; ++i)
            {
                // Apply bypass
                channel_t *c        = &vChannels[i];
                c->sBypass.process(c->vOut, vInputs[i%nInputs].vIn, c->vRender, to_do);
            }

            // Adjust delay
            for (size_t j=0; j<slap_delay_base_metadata::MAX_PROCESSORS; ++j)
                vProcessors[j].nDelay   = vProcessors[j].nNewDelay;

            // Remove rare data from shift buffers
            vInputs[0].sBuffer.shift(to_do);
            if (nInputs > 1)
                vInputs[1].sBuffer.shift(to_do);

            // Update pointers
            for (size_t i=0; i<nInputs; ++i)
                vInputs[i].vIn     += to_do;
            for (size_t i=0; i<2; ++i)
                vChannels[i].vOut  += to_do;
            k   += to_do;
        }
    }

    //-------------------------------------------------------------------------
    // Slap-back delay instances
    slap_delay_mono::slap_delay_mono() : slap_delay_base(metadata, false)
    {
    }

    slap_delay_mono::~slap_delay_mono()
    {
    }

    slap_delay_stereo::slap_delay_stereo() : slap_delay_base(metadata, true)
    {
    }

    slap_delay_stereo::~slap_delay_stereo()
    {
    }

} /* namespace lsp */
