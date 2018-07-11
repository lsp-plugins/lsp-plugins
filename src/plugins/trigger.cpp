/*
 * trigger.cpp
 *
 *  Created on: 05 мая 2016 г.
 *      Author: sadko
 */

#include <math.h>

#include <core/dsp.h>
#include <core/debug.h>
#include <core/midi.h>
#include <core/colors.h>
#include <core/Color.h>

#include <plugins/trigger.h>

#define TRACE_PORT(p) lsp_trace("  port id=%s", (p)->metadata()->id);

namespace lsp
{
    //-------------------------------------------------------------------------
    inline void trigger_base::update_counters()
    {
        if (fSampleRate <= 0)
            return;

        nDetectCounter      = millis_to_samples(fSampleRate, fDetectTime);
        nReleaseCounter     = millis_to_samples(fSampleRate, fReleaseTime);
    }

    inline void trigger_base::update_reactivity()
    {
        if (fSampleRate <= 0)
            return;

        nReactivity     = millis_to_samples(fSampleRate, fReactivity);
        fTau            = 1.0f - expf(logf(1.0f - M_SQRT1_2) / (nReactivity)); // Tau is based on seconds
        nRefresh        = BUFFER_SIZE; // Force the function to be refreshed
    }

    inline float trigger_base::get_sample(const float **data, size_t idx)
    {
        if (nChannels == 1)
            return data[0][idx];
        else if (nChannels < 2)
            return 0.0f;

        switch (nSource)
        {
            case S_LEFT:
                return data[0][idx];
            case S_RIGHT:
                return data[1][idx];
            case S_MIDDLE:
                return (data[0][idx] + data[1][idx]) * 0.5f;
            case S_SIDE:
                return (data[0][idx] - data[1][idx]) * 0.5f;
            default:
                return data[0][idx];
        }
        return 0.0f;
    }

    inline float trigger_base::process_sample(float sample)
    {
        // This is periodically called to fix floating-point rounding errors
        if ((nRefresh++) >= BUFFER_SIZE)
        {
            refresh_processing();
            nRefresh    = 0;
        }

        // Add new sample to buffer
        sBuffer.append(sample);

        // Calculate function
        switch (nMode)
        {
            case M_PEAK:
                break;

            case M_LPF:
                if (sample < 0.0f)
                    sample = - sample;
                fRmsValue  += fTau * (sample - fRmsValue);
                sample      = fRmsValue;
                break;

            case M_UNIFORM:
                if (sample < 0.0f)
                    sample      = -sample;
                if (nReactivity > 0)
                {
                    float first = sBuffer.last(nReactivity + 1);
                    if (first < 0.0f)
                        first       = -first;

                    fRmsValue  += sample - first;
                    sample      = fRmsValue / float(nReactivity + 1);
                }
                break;

            case M_RMS:
//                fRmsValue       = dsp::h_sqr_sum(sBuffer.tail(nReactivity + 1), nReactivity + 1);
                if (nReactivity > 0)
                {
                    float last      = sBuffer.last(nReactivity + 1);
                    fRmsValue      += sample * sample - last * last;
                    if (fRmsValue < 0.0f)
                        sample          = 0.0f;
                    else
                        sample          = sqrtf(fRmsValue / float(nReactivity));
                }
                break;

            default:
                break;
        }

        // Remove old sample
        sBuffer.shift();

        return (sample >= 0.0f) ? sample : -sample;
    }

    void trigger_base::refresh_processing()
    {
        switch (nMode)
        {
            case M_PEAK:
                fRmsValue       = 0.0f;
                break;

            case M_UNIFORM:
                fRmsValue           = dsp::h_abs_sum(sBuffer.tail(nReactivity), nReactivity);
                break;

            case M_RMS:
                fRmsValue           = dsp::h_sqr_sum(sBuffer.tail(nReactivity), nReactivity);
                break;

            default:
                break;
        }
    }

    void trigger_base::process_samples(const float **data, size_t samples)
    {
        float max_level     = 0.0f, max_velocity  = 0.0f;

        // Process input data
        for (size_t i=0; i<samples; ++i)
        {
            // Get sample and log to function
            float level         = get_sample(data, i);
            level               = process_sample(level);
            if (level > max_level)
                max_level           = level;
            sFunction.process(level);

            // Check trigger state
            switch (nState)
            {
                case T_OFF: // Trigger is closed
                    if (level >= fDetectLevel) // Signal is growing, open trigger
                    {
                        // Mark trigger open
                        nCounter    = nDetectCounter;
                        nState      = T_DETECT;
                    }
                    break;
                case T_DETECT:
                    if (level < fDetectLevel)
                        nState      = T_OFF;
                    else if ((nCounter--) <= 0)
                    {
                        // Calculate the velocity
                        fVelocity   = 0.5f * expf(fDynamics * logf(level/fDetectLevel));
                        float vel   = fVelocity;
                        if (vel >= fDynaTop) // Saturate to maximum
                            vel         = 1.0f;
                        else if (vel <= fDynaBottom) // Saturate to minimum
                            vel         = 0.0f;
                        else // Calculate the velocity based on logarithmic scale
                            vel         = logf(vel/fDynaBottom) / logf(fDynaTop/fDynaBottom);

                        // Trigger state ON
                        trigger_on(i, vel);
                        nState      = T_ON;

                        // Indicate that trigger is active
                        sActive.blink();
                    }
                    break;
                case T_ON: // Trigger is active
                    if (level <= fReleaseLevel) // Signal is in peak
                    {
                        nCounter    = nReleaseCounter;
                        nState      = T_RELEASE;
                    }
                    break;
                case T_RELEASE:
                    if (level > fReleaseLevel)
                        nState      = T_ON;
                    else if ((nCounter--) <= 0)
                    {
                        trigger_off(i, 0.0f);
                        nState      = T_OFF;
                        fVelocity   = 0.0f;
                    }
                    break;

                default:
                    break;
            }

            // Log the velocity value
            sVelocity.process(fVelocity);
            if (fVelocity > max_velocity)
                max_velocity        = fVelocity;
        }

        // Output meter value
        if (pActive != NULL)
            pActive->setValue(sActive.process(samples));

        pFunctionLevel->setValue(max_level);
        pVelocityLevel->setValue(max_velocity);
    }


    trigger_base::trigger_base(const plugin_metadata_t &metadata, size_t files, size_t channels, bool midi):
        plugin_t(metadata)
    {
        // Instantiation parameters
        nFiles              = files;
        nChannels           = channels;
        bMidiPorts          = midi;

        // Processors and buffers
        vTimePoints         = NULL;

        // Processing variables
        nCounter            = 0;
        nState              = T_OFF;
        nReactivity         = 0;
        fTau                = 0.0f;
        fVelocity           = 0.0f;
        bFunctionActive     = true;
        bVelocityActive     = true;
        fRmsValue           = 0.0f;

        // Parameters
        nNote               = trigger_midi_metadata::NOTE_DFL + trigger_midi_metadata::OCTAVE_DFL * 12;
        nChannel            = trigger_midi_metadata::CHANNEL_DFL;
        fDry                = 1.0f;
        fWet                = 1.0f;
        bPause              = false;
        bClear              = false;
        fPreamp             = 1.0f;
        nRefresh            = BUFFER_SIZE;

        nSource             = S_MIDDLE;
        nMode               = M_RMS;
        nDetectCounter      = 0;
        nReleaseCounter     = 0;
        fDetectLevel        = DETECT_LEVEL_DFL;
        fDetectTime         = DETECT_TIME_DFL;
        fReleaseLevel       = RELEASE_LEVEL_DFL;
        fReleaseTime        = RELEASE_TIME_DFL;
        fDynamics           = 0.0f;
        fDynaTop            = 1.0f;
        fDynaBottom         = 0.0f;
        fReactivity         = REACTIVITY_DFL;
        pIDisplay           = NULL;

        // Control ports
        pFunction           = NULL;
        pFunctionLevel      = NULL;
        pFunctionActive     = NULL;
        pVelocity           = NULL;
        pVelocityLevel      = NULL;
        pVelocityActive     = NULL;
        pActive             = NULL;

        pMidiIn             = NULL;
        pMidiOut            = NULL;
        pChannel            = NULL;
        pNote               = NULL;
        pOctave             = NULL;
        pMidiNote           = NULL;

        pBypass             = NULL;
        pDry                = NULL;
        pWet                = NULL;
        pGain               = NULL;
        pPreamp             = NULL;

        pSource             = NULL;
        pMode               = NULL;
        pPause              = NULL;
        pClear              = NULL;
        pPreamp             = NULL;
        pDetectLevel        = NULL;
        pDetectTime         = NULL;
        pReleaseLevel       = NULL;
        pReleaseTime        = NULL;
        pDynamics           = NULL;
        pDynaRange1         = NULL;
        pDynaRange2         = NULL;
        pReactivity         = NULL;
        pReleaseValue       = NULL;
    }

    trigger_base::~trigger_base()
    {
        destroy();
    }

    void trigger_base::destroy()
    {
        // Destroy objects
        sKernel.destroy();

        // Remove time points buffer
        if (vTimePoints != NULL)
        {
            delete [] vTimePoints;
            vTimePoints     = NULL;
        }

        for (size_t i=0; i<trigger_base_metadata::TRACKS_MAX; ++i)
        {
            channel_t *tc   = &vChannels[i];
            tc->vCtl        = NULL;
            tc->pIn         = NULL;
            tc->pOut        = NULL;
        }

        if (pIDisplay != NULL)
        {
            pIDisplay->detroy();
            pIDisplay   = NULL;
        }
    }

    void trigger_base::init(IWrapper *wrapper)
    {
        // Pass wrapper
        plugin_t::init(wrapper);

        // Get executor
        IExecutor *executor = wrapper->get_executor();

        // Initialize audio channels
        for (size_t i=0; i<trigger_base_metadata::TRACKS_MAX; ++i)
        {
            channel_t *c        = &vChannels[i];
            c->vCtl             = NULL;
            c->pIn              = NULL;
            c->pOut             = NULL;
            c->pGraph           = NULL;
            c->pMeter           = NULL;
            c->pVisible         = NULL;
        }

        // Allocate buffer for time coordinates
        vTimePoints         = new float[HISTORY_MESH_SIZE + BUFFER_SIZE * nChannels];
        if (vTimePoints == NULL)
            return;
        float *ctlbuf       = &vTimePoints[HISTORY_MESH_SIZE];

        // Fill time dots with values
        float step          = HISTORY_TIME / HISTORY_MESH_SIZE;
        for (size_t i=0; i < HISTORY_MESH_SIZE; ++i)
            vTimePoints[i]      = (HISTORY_MESH_SIZE - i - 1) * step;

        // Initialize trigger
        sKernel.init(executor, nFiles, nChannels);

        // Now we are ready to bind ports
        size_t port_id          = 0;

        // Bind audio inputs
        lsp_trace("Binding audio inputs...");
        for (size_t i=0; i<nChannels; ++i)
        {
            TRACE_PORT(vPorts[port_id]);
            vChannels[i].pIn        = vPorts[port_id++];
            vChannels[i].vCtl       = ctlbuf;

            dsp::fill_zero(ctlbuf, BUFFER_SIZE);
            ctlbuf                 += BUFFER_SIZE;
        }

        // Bind audio outputs
        lsp_trace("Binding audio outputs...");
        for (size_t i=0; i<nChannels; ++i)
        {
            TRACE_PORT(vPorts[port_id]);
            vChannels[i].pOut       = vPorts[port_id++];
        }

        // Bind meters
        if (nChannels > 1)
        {
            lsp_trace("Binding source switch port...");
            TRACE_PORT(vPorts[port_id]);
            pSource             = vPorts[port_id++];
        }

        lsp_trace("Binding audio meters...");
        for (size_t i=0; i<nChannels; ++i)
        {
            TRACE_PORT(vPorts[port_id]);
            vChannels[i].pGraph     = vPorts[port_id++];
        }
        for (size_t i=0; i<nChannels; ++i)
        {
            TRACE_PORT(vPorts[port_id]);
            vChannels[i].pMeter     = vPorts[port_id++];
        }
        for (size_t i=0; i<nChannels; ++i)
        {
            TRACE_PORT(vPorts[port_id]);
            vChannels[i].pVisible   = vPorts[port_id++];
        }

        // Bind MIDI ports
        if (bMidiPorts)
        {
            lsp_trace("Binding MIDI ports...");
            TRACE_PORT(vPorts[port_id]);
            pMidiIn     = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            pMidiOut    = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            pChannel    = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            pNote       = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            pOctave     = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            pMidiNote   = vPorts[port_id++];
        }

        // Skip area selector
        lsp_trace("Skipping Area selector...");
        TRACE_PORT(vPorts[port_id]);
        port_id     ++;

        // Bind ports
        lsp_trace("Binding Global ports...");
        TRACE_PORT(vPorts[port_id]);
        pBypass     = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pDry        = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pWet        = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pGain       = vPorts[port_id++];

        lsp_trace("Binding mode port...");
        TRACE_PORT(vPorts[port_id]);
        pMode               = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pPause              = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pClear              = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pPreamp             = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pDetectLevel        = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pDetectTime         = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pReleaseLevel       = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pReleaseTime        = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pDynamics           = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pDynaRange1         = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pDynaRange2         = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pReactivity         = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pReleaseValue       = vPorts[port_id++];

        lsp_trace("Binding meters...");
        TRACE_PORT(vPorts[port_id]);
        pFunction           = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pFunctionLevel      = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pFunctionActive     = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pActive             = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pVelocity           = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pVelocityLevel      = vPorts[port_id++];
        TRACE_PORT(vPorts[port_id]);
        pVelocityActive     = vPorts[port_id++];

        // Bind kernel
        lsp_trace("Binding kernel ports...");
        port_id     = sKernel.bind(vPorts, port_id, false);

        // Call for initial settings update
        lsp_trace("Calling settings update");
        update_settings();
    }

    void trigger_base::update_settings()
    {
        // Update settings for notes
        if (bMidiPorts)
        {
            nNote       = (pOctave->getValue() * 12) + pNote->getValue();
            lsp_trace("trigger note=%d", int(nNote));
        }

        // Update settings
        nSource         = (pSource != NULL) ? size_t(pSource->getValue()) : S_MIDDLE;
        size_t old_mode = nMode;
        nMode           = (pMode != NULL) ? size_t(pMode->getValue()) : M_PEAK;
        fDetectLevel    = pDetectLevel->getValue();
        fDetectTime     = pDetectTime->getValue();
        fReleaseLevel   = fDetectLevel * pReleaseLevel->getValue();
        fReleaseTime    = pReleaseTime->getValue();
        fDynamics       = pDynamics->getValue() * 0.01f; // Percents
        fDynaTop        = pDynaRange1->getValue();
        fDynaBottom     = pDynaRange2->getValue();
        fReactivity     = pReactivity->getValue();
        fPreamp         = pPreamp->getValue();

        float out_gain  = pGain->getValue();
        fDry            = pDry->getValue() * out_gain;
        fWet            = pWet->getValue() * out_gain;
        bFunctionActive = pFunctionActive->getValue() >= 0.5f;
        bVelocityActive = pVelocityActive->getValue() >= 0.5f;

        // Update dynamics
        if (fDynaTop < 1e-6f)
            fDynaTop    = 1e-6f;
        if (fDynaBottom < 1e-6f)
            fDynaBottom = 1e-6f;
        if (fDynaTop < fDynaBottom)
        {
            float tmp   = fDynaTop;
            fDynaTop    = fDynaBottom;
            fDynaBottom = tmp;
        }

        // Update reactivity
        update_reactivity();
        if (old_mode != nMode)
            fRmsValue       = 0.0f;

        // Update sampler settings
        sKernel.update_settings();

        // Update bypass
        bool bypass     = pBypass->getValue() >= 0.5f;
        for (size_t i=0; i<nChannels; ++i)
        {
            if (vChannels[i].sBypass.set_bypass(bypass))
                pWrapper->query_display_draw();
            vChannels[i].bVisible   = vChannels[i].pVisible->getValue() >= 0.5f;
        }

        // Update pause
        bPause          = pPause->getValue() >= 0.5f;
        bClear          = pClear->getValue() >= 0.5f;

        // Update counters
        update_counters();
    }

    void trigger_base::update_sample_rate(long sr)
    {
        // Calculate number of samples per dot for shift buffer and initialize buffers
        size_t samples_per_dot  = seconds_to_samples(sr, HISTORY_TIME / HISTORY_MESH_SIZE);

        // Update sample rate for bypass
        for (size_t i=0; i<nChannels; ++i)
        {
            vChannels[i].sBypass.init(sr);
            vChannels[i].sGraph.init(HISTORY_MESH_SIZE, samples_per_dot);
        }
        sFunction.init(HISTORY_MESH_SIZE, samples_per_dot);
        sVelocity.init(HISTORY_MESH_SIZE, samples_per_dot);

        // Update settings on all samplers
        sKernel.update_sample_rate(sr);

        // Update trigger buffer
        size_t gap              = millis_to_samples(sr, REACTIVITY_MAX);
        sBuffer.init(gap * 4, gap);

        // Update activity blink
        sActive.init(sr);

        // Update reactivity
        update_reactivity();

        // Update counters
        update_counters();
    }

    void trigger_base::trigger_on(size_t timestamp, float level)
    {
        lsp_trace("midi_out = %p", pMidiOut);
        if (pMidiOut != NULL)
        {
            // We need to emit the NoteOn event
            midi_t *midi    = pMidiOut->getBuffer<midi_t>();
            lsp_trace("midi buffer = %p", midi);
            if (midi != NULL)
            {
                // Create event
                midi_event_t ev;
                ev.timestamp    = timestamp;
                ev.type         = MIDI_MSG_NOTE_ON;
                ev.channel      = nChannel;
                ev.note.pitch   = nNote;
                ev.note.velocity= uint32_t(1 + (level * 126));

                // Store event in MIDI buffer
                midi->push(ev);
            }
        }

        // Handle Note On event
        sKernel.trigger_on(timestamp, level);
    }

    void trigger_base::trigger_off(size_t timestamp, float level)
    {
        if (pMidiOut != NULL)
        {
            // We need to emit the NoteOff event
            midi_t *midi    = pMidiOut->getBuffer<midi_t>();
            if (midi != NULL)
            {
                // Create event
                midi_event_t ev;
                ev.timestamp    = timestamp;
                ev.type         = MIDI_MSG_NOTE_OFF;
                ev.channel      = nChannel;
                ev.note.pitch   = nNote;
                ev.note.velocity= 0;                        // Velocity is zero now

                // Store event in MIDI buffer
                midi->push(ev);
            }
        }

        // Handle Note Off event
        sKernel.trigger_off(timestamp, level);
    }

    void trigger_base::process(size_t samples)
    {
        // Bypass MIDI events (additionally to the triggered events)
        if ((pMidiIn != NULL) && (pMidiOut != NULL))
        {
            midi_t *in  = pMidiIn->getBuffer<midi_t>();
            midi_t *out = pMidiOut->getBuffer<midi_t>();

            // Bypass MIDI events from input to output
            if ((in != NULL) && (out != NULL))
                out->push_all(in);

            // Output midi note number
            if (pMidiNote != NULL)
                pMidiNote->setValue(nNote);
        }

        // Get pointers to channel buffers
        const float *ins[TRACKS_MAX];
        float *outs[TRACKS_MAX], *ctls[TRACKS_MAX];

        for (size_t i=0; i<nChannels; ++i)
        {
            channel_t *c        = &vChannels[i];
            ins[i]              = (c->pIn != NULL)  ? c->pIn->getBuffer<float>() : NULL;
            outs[i]             = (c->pOut != NULL) ? c->pOut->getBuffer<float>() : NULL;

            // Update meter
            if ((ins[i] != NULL) && (c->pMeter != NULL))
            {
                float level = (c->bVisible) ? dsp::abs_max(ins[i], samples) * fPreamp : 0.0f;
                c->pMeter->setValue(level);
            }
        }
        pReleaseValue->setValue(fReleaseLevel);

        // Process samples
        for (size_t offset = 0; offset < samples; )
        {
            // Calculate amount of samples to process
            size_t to_process = samples - offset;
            if (to_process > BUFFER_SIZE)
                to_process  = BUFFER_SIZE;

            // Prepare the control chain
            for (size_t i=0; i<nChannels; ++i)
            {
                channel_t *c        = &vChannels[i];
                dsp::scale(c->vCtl, ins[i], fPreamp, to_process);
                c->sGraph.process(c->vCtl, samples);
                ctls[i]             = c->vCtl;
            }

            // Now we have to process data
            process_samples(const_cast<const float **>(ctls), to_process);

            // Call sampler kernel for processing
            sKernel.process(ctls, NULL, to_process);

            // Now mix dry/wet signals and pass thru bypass switch
            for (size_t i=0; i<nChannels; ++i)
            {
                dsp::mix(ctls[i], ins[i], ctls[i], fDry, fWet, to_process);
                vChannels[i].sBypass.process(outs[i], ins[i], ctls[i], to_process);
            }

            // Update pointers
            for (size_t i=0; i<nChannels; ++i)
            {
                ins[i]         += to_process;
                outs[i]        += to_process;
            }
            offset         += to_process;
        }

        if ((!bPause) || (bClear))
        {
            // Process mesh requests
            for (size_t i=0; i<nChannels; ++i)
            {
                // Get channel
                channel_t *c        = &vChannels[i];
                if (c->pGraph == NULL)
                    continue;

                // Clear data if requested
                if (bClear)
                    dsp::fill_zero(c->sGraph.data(), HISTORY_MESH_SIZE);

                // Get mesh
                mesh_t *mesh    = c->pGraph->getBuffer<mesh_t>();
                if ((mesh != NULL) && (mesh->isEmpty()))
                {
                    // Fill mesh with new values
                    dsp::copy(mesh->pvData[0], vTimePoints, HISTORY_MESH_SIZE);
                    dsp::copy(mesh->pvData[1], c->sGraph.data(), HISTORY_MESH_SIZE);
                    mesh->data(2, HISTORY_MESH_SIZE);
                }
            }

            // Trigger function
            if (pFunction != NULL)
            {
                // Clear data if requested
                if (bClear)
                    dsp::fill_zero(sFunction.data(), HISTORY_MESH_SIZE);

                // Fill mesh if needed
                mesh_t *mesh = pFunction->getBuffer<mesh_t>();
                if ((mesh != NULL) && (mesh->isEmpty()))
                {
                    dsp::copy(mesh->pvData[0], vTimePoints, HISTORY_MESH_SIZE);
                    dsp::copy(mesh->pvData[1], sFunction.data(), HISTORY_MESH_SIZE);
                    mesh->data(2, HISTORY_MESH_SIZE);
                }
            }

            // Trigger velocity
            if (pVelocity != NULL)
            {
                // Clear data if requested
                if (bClear)
                    dsp::fill_zero(sVelocity.data(), HISTORY_MESH_SIZE);

                // Fill mesh if needed
                mesh_t *mesh = pVelocity->getBuffer<mesh_t>();
                if ((mesh != NULL) && (mesh->isEmpty()))
                {
                    dsp::copy(mesh->pvData[0], vTimePoints, HISTORY_MESH_SIZE);
                    dsp::copy(mesh->pvData[1], sVelocity.data(), HISTORY_MESH_SIZE);
                    mesh->data(2, HISTORY_MESH_SIZE);
                }
            }
        }

        // Always query for draawing
        pWrapper->query_display_draw();
    }

    bool trigger_base::inline_display(ICanvas *cv, size_t width, size_t height)
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
        cv->set_color_rgb(CV_BACKGROUND);
        cv->paint();

        // Calc axis params
        float zy    = 1.0f/GAIN_AMP_M_72_DB;
        float dx    = -width/HISTORY_TIME;
        float dy    = -height/(logf(GAIN_AMP_P_24_DB)-logf(GAIN_AMP_M_72_DB));

        // Draw axis
        cv->set_line_width(1.0);

        // Draw vertical lines
        cv->set_color_rgb(CV_YELLOW, 0.5f);
        for (float i=1.0; i < (HISTORY_TIME-0.1); i += 1.0f)
        {
            float ax = width + dx*i;
            cv->line(ax, 0, ax, height);
        }

        // Draw horizontal lines
        cv->set_color_rgb(CV_WHITE, 0.5f);
        for (float i=GAIN_AMP_M_48_DB; i<GAIN_AMP_P_24_DB; i *= GAIN_AMP_P_24_DB)
        {
            float ay = height + dy*(logf(i*zy));
            cv->line(0, ay, width, ay);
        }

        // Allocate buffer: t, f1(t), x, y
        pIDisplay           = float_buffer_t::reuse(pIDisplay, 4, width);
        float_buffer_t *b   = pIDisplay;
        if (b == NULL)
            return false;

        // Draw input signal
        static uint32_t c_colors[] = {
                CV_MIDDLE_CHANNEL, CV_MIDDLE_CHANNEL,
                CV_LEFT_CHANNEL, CV_RIGHT_CHANNEL
               };
        bool bypass         = vChannels[0].sBypass.bypassing();
        float r             = HISTORY_MESH_SIZE/float(width);

        for (size_t j=0; j<width; ++j)
        {
            size_t k        = r*j;
            b->v[0][j]      = vTimePoints[k];
        }

        cv->set_line_width(2.0f);
        for (size_t i=0; i<nChannels; ++i)
        {
            channel_t *c    = &vChannels[i];
            if (!c->bVisible)
                continue;

            // Initialize values
            float *ft       = c->sGraph.data();
            for (size_t j=0; j<width; ++j)
                b->v[1][j]      = ft[size_t(r*j)];

            // Initialize coords
            dsp::fill(b->v[2], width, width);
            dsp::fill(b->v[3], height, width);
            dsp::add_multiplied(b->v[2], b->v[0], dx, width);
            dsp::axis_apply_log(b->v[2], b->v[3], b->v[1], zy, 0.0f, dy, width);

            // Draw channel
            cv->set_color_rgb((bypass) ? CV_SILVER : c_colors[(nChannels-1)*2 + i]);
            cv->draw_lines(b->v[2], b->v[3], width);
        }

        // Draw function (if present)
        if (bFunctionActive)
        {
            float *ft       = sFunction.data();
            for (size_t j=0; j<width; ++j)
                b->v[1][j]      = ft[size_t(r*j)];

            // Initialize coords
            dsp::fill(b->v[2], width, width);
            dsp::fill(b->v[3], height, width);
            dsp::add_multiplied(b->v[2], b->v[0], dx, width);
            dsp::axis_apply_log(b->v[2], b->v[3], b->v[1], zy, 0.0f, dy, width);

            // Draw channel
            cv->set_color_rgb((bypass) ? CV_SILVER : CV_GREEN);
            cv->draw_lines(b->v[2], b->v[3], width);
        }

        // Draw events (if present)
        if (bVelocityActive)
        {
            float *ft       = sVelocity.data();
            for (size_t j=0; j<width; ++j)
                b->v[1][j]      = ft[size_t(r*j)];

            // Initialize coords
            dsp::fill(b->v[2], width, width);
            dsp::fill(b->v[3], height, width);
            dsp::add_multiplied(b->v[2], b->v[0], dx, width);
            dsp::axis_apply_log(b->v[2], b->v[3], b->v[1], zy, 0.0f, dy, width);

            // Draw channel
            cv->set_color_rgb((bypass) ? CV_SILVER : CV_MEDIUM_GREEN);
            cv->draw_lines(b->v[2], b->v[3], width);
        }

        // Draw boundaries
        cv->set_color_rgb(CV_MAGENTA, 0.5f);
        cv->set_line_width(1.0);
        {
            float ay = height + dy*(logf(fDetectLevel*zy));
            cv->line(0, ay, width, ay);
            ay = height + dy*(logf(fReleaseLevel*zy));
            cv->line(0, ay, width, ay);
        }

        return true;
    }

    //-------------------------------------------------------------------------
    trigger_mono::trigger_mono(): trigger_base(metadata, SAMPLE_FILES, 1, false)
    {
    }

    trigger_mono::~trigger_mono()
    {
    }

    trigger_stereo::trigger_stereo(): trigger_base(metadata, SAMPLE_FILES, 2, false)
    {
    }

    trigger_stereo::~trigger_stereo()
    {
    }

    trigger_midi_mono::trigger_midi_mono(): trigger_base(metadata, SAMPLE_FILES, 1, true)
    {
    }

    trigger_midi_mono::~trigger_midi_mono()
    {
    }

    trigger_midi_stereo::trigger_midi_stereo(): trigger_base(metadata, SAMPLE_FILES, 2, true)
    {
    }

    trigger_midi_stereo::~trigger_midi_stereo()
    {
    }

}

