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

#include <plugins/trigger.h>

#define TRACE_PORT(p) lsp_trace("  port id=%s", (p)->metadata()->id);

namespace lsp
{
#ifndef LSP_NO_EXPERIMENTAL
    //-------------------------------------------------------------------------
    audio_trigger_kernel::audio_trigger_kernel()
    {
        pHandler            = NULL;
        nCounter            = 0;
        nSampleRate         = 0;
        nChannels           = 0;
        nState              = T_OFF;
        nReactivity         = 0;
        fTau                = 0.0f;
        fRmsValue           = 0.0f;

        nSource             = S_MIDDLE;
        nMode               = M_RMS;
        fDetectLevel        = DETECT_LEVEL_DFL;
        fDetectTime         = DETECT_TIME_DFL;
        fReleaseLevel       = RELEASE_LEVEL_DFL;
        fReleaseTime        = RELEASE_TIME_DFL;
        fDynamics           = 0.0f;
        fReactivity         = REACTIVITY_DFL;

        pSource             = NULL;
        pMode               = NULL;
        pDetectLevel        = NULL;
        pDetectTime         = NULL;
        pReleaseLevel       = NULL;
        pReleaseTime        = NULL;
        pDynamics           = NULL;
        pReactivity         = NULL;
        pMeter              = NULL;
        pActive             = NULL;
        #ifdef LSP_DEBUG
            pDebug              = NULL;
        #endif /* LSP_DEBUG */
    }

    audio_trigger_kernel::~audio_trigger_kernel()
    {
        destroy();
    }

    bool audio_trigger_kernel::init(ITrigger *handler, size_t channels)
    {
        pHandler            = handler;
        nChannels           = channels;
        return true;
    }

    size_t audio_trigger_kernel::bind(cvector<IPort> &ports, size_t port_id)
    {
        if (nChannels > 1)
        {
            lsp_trace("Binding source port...");
            TRACE_PORT(ports[port_id]);
            pSource             = ports[port_id++];
        }

        lsp_trace("Binding mode port...");
        TRACE_PORT(ports[port_id]);
        pMode               = ports[port_id++];

        lsp_trace("Binding ports...");
        TRACE_PORT(ports[port_id]);
        pDetectLevel        = ports[port_id++];
        TRACE_PORT(ports[port_id]);
        pDetectTime         = ports[port_id++];
        TRACE_PORT(ports[port_id]);
        pReleaseLevel       = ports[port_id++];
        TRACE_PORT(ports[port_id]);
        pReleaseTime        = ports[port_id++];
        TRACE_PORT(ports[port_id]);
        pDynamics           = ports[port_id++];
        TRACE_PORT(ports[port_id]);
        pReactivity         = ports[port_id++];

        lsp_trace("Binding meters...");
        TRACE_PORT(ports[port_id]);
        pMeter              = ports[port_id++];
        TRACE_PORT(ports[port_id]);
        pActive             = ports[port_id++];

        return port_id;
    }

#ifdef LSP_DEBUG
    void audio_trigger_kernel::bind_debug(IPort *debug)
    {
        pDebug              = debug;
    }
#endif /* LSP_DEBUG */

    void audio_trigger_kernel::destroy()
    {
    }

    inline void audio_trigger_kernel::update_reactivity(size_t old_mode)
    {
        if (nSampleRate <= 0)
            return;

        nReactivity     = millis_to_samples(nSampleRate, fReactivity);
        fTau            = 1.0f - expf(logf(1.0f - M_SQRT1_2) / nReactivity);

        switch (nMode)
        {
            case M_PEAK:
                if (old_mode != M_PEAK)
                    fRmsValue       = 0.0f;
                break;

            case M_RMS:
            case M_ABS_RMS:
                if ((old_mode != M_RMS) && (old_mode != M_ABS_RMS))
                    fRmsValue       = 0.0f;
                break;

            case M_FILTERED:
                fRmsValue           = dsp::h_sum(sBuffer.tail(nReactivity), nReactivity);
                break;

            case M_SQR:
                fRmsValue           = dsp::h_sqr_sum(sBuffer.tail(nReactivity), nReactivity);
                break;

            default:
                break;
        }
    }

    void audio_trigger_kernel::update_settings()
    {
        // Update settings
        size_t old_mode = nMode;

        nSource         = (pSource != NULL) ? size_t(pSource->getValue()) : S_MIDDLE;
        nMode           = (pMode != NULL) ? size_t(pMode->getValue()) : M_RMS;
        fDetectLevel    = pDetectLevel->getValue();
        fDetectTime     = pDetectTime->getValue();
        fReleaseLevel   = pDetectLevel->getValue();
        fReleaseTime    = pDetectTime->getValue();
        fDynamics       = pDynamics->getValue();
        fReactivity     = pReactivity->getValue();

        // Update reactivity
        update_reactivity(old_mode);
    }

    inline float audio_trigger_kernel::get_sample(const float **data, size_t idx)
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

    inline float audio_trigger_kernel::process_sample(float sample)
    {
        // Add new sample
        sBuffer.append(sample);

        switch (nMode)
        {
            case M_PEAK:
                if (sample < 0.0f)
                    sample = -sample;
                break;

            case M_RMS:
                fRmsValue  += fTau * (sample - fRmsValue);
                sample      = fRmsValue;
                if (sample < 0.0f)
                    sample      = -sample;
                break;

            case M_ABS_RMS:
                if (sample < 0.0f)
                    sample = - sample;
                fRmsValue  += fTau * (sample - fRmsValue);
                sample      = fRmsValue;
                if (sample < 0.0f)
                    sample      = 0.0f;
                break;

            case M_FILTERED:
                if (nReactivity > 0)
                {
                    fRmsValue  += sample - sBuffer.first();
                    sample      = fRmsValue / float(nReactivity + 1);
                }
                if (sample < 0.0f)
                    sample      = -sample;
                break;

            case M_SQR:
                if (nReactivity > 0)
                {
                    float first = sBuffer.first();
                    fRmsValue  += sample*sample - first * first;
                    sample      = sqrtf(fRmsValue) / float(nReactivity + 1);
                }
                else if (sample < 0.0f)
                    sample      = -sample;
                break;

            default:
                break;
        }

        // Remove old sample
        sBuffer.shift();

        return sample;
    }

    void audio_trigger_kernel::update_sample_rate(long sr)
    {
        nSampleRate     = sr;

        // Update trigger buffer
        size_t gap      = millis_to_samples(sr, DETECT_TIME_MAX);
        sBuffer.init(gap * 2, gap);

        // Update activity blink
        sActive.init(sr);

        // Update reactivity
        update_reactivity(nMode);
    }

    void audio_trigger_kernel::process(const float **data, size_t samples)
    {
        // Do nothing if handler not defined
        if (pHandler == NULL)
            return;

        float meter         = 0.0f;

        // Process input data
        for (size_t i=0; i<samples; ++i)
        {
            // Get sample
            float level         = get_sample(data, i);
            level               = process_sample(level);
            if ((i == 0) || (level > meter))
                meter           = level;

            #ifdef LSP_DEBUG
                if (pDebug != NULL)
                {
                    float *buf = pDebug->getBuffer<float>();
                    if (buf != NULL)
                        buf[i]      = level;
                }
            #endif /* LSP_DEBUG */

            // Check trigger state
            switch (nState)
            {
                case T_OFF: // Trigger is closed
                    if (level >= fDetectLevel) // Signal is growing, open trigger
                    {
                        // Mark trigger open
                        nCounter    = millis_to_samples(nSampleRate, fDetectTime);
                        nState      = T_DETECT;
                    }
                    break;
                case T_DETECT:
                    if (level < fDetectLevel)
                        nState      = T_DETECT;
                    else if ((nCounter--) <= 0)
                    {
                        if (pHandler != NULL)
                        {
                            // Calculate the velocity
                            float velocity  = 0.5f + fDynamics * (level - fDetectLevel) / float(nReactivity + 1);
                            if (velocity > 1.0f) // Saturate velocity
                                velocity        = 1.0f;

                            pHandler->trigger_on(i, velocity);
                        }

                        // Indicate that trigger is active
                        sActive.blink();
                    }
                    break;
                case T_ON: // Trigger is active
                    if (level <= fReleaseLevel) // Signal is in peak
                    {
                        nCounter    = millis_to_samples(nSampleRate, fReleaseTime);
                        nState      = T_RELEASE;
                    }
                    break;
                case T_RELEASE:
                    if (level > fReleaseLevel)
                        nState      = T_DETECT;
                    else if ((nCounter--) <= 0)
                    {
                        if (pHandler != NULL)
                            pHandler->trigger_off(i, 0.0f);
                    }
                    break;
                default:
                    break;
            }
        }

        // Output meter value
        if (pMeter != NULL)
            pMeter->setValue(meter);
        if (pActive != NULL)
            pActive->setValue(sActive.process(samples));
    }

    //-------------------------------------------------------------------------
    trigger_kernel::trigger_kernel()
    {
        nNote               = 0;
        nChannel            = 0;

        pChannel            = NULL;
        pNote               = NULL;
        pOctave             = NULL;
        pMidiNote           = NULL;
        pMidiIn             = NULL;
        pMidiOut            = NULL;
    }

    trigger_kernel::~trigger_kernel()
    {
        pMidiOut            = NULL;
    }

    size_t trigger_kernel::bind(cvector<IPort> &ports, size_t port_id)
    {
        lsp_trace("Binding ports...\n");
        TRACE_PORT(ports[port_id]);
        pChannel        = ports[port_id++];
        TRACE_PORT(ports[port_id]);
        pNote           = ports[port_id++];
        TRACE_PORT(ports[port_id]);
        pOctave         = ports[port_id++];
        TRACE_PORT(ports[port_id]);
        pMidiNote       = ports[port_id++];

        return sampler_kernel::bind(ports, port_id, false);
    }

    void trigger_kernel::trigger_on(size_t timestamp, float level)
    {
        if (pMidiOut != NULL)
        {
            // We need to emit the NoteOn event
            midi_t *midi    = pMidiOut->getBuffer<midi_t>();
            if (midi != NULL)
            {
                // Create event
                midi_event_t ev;
                ev.timestamp    = timestamp;
                ev.type         = MIDI_MSG_NOTE_ON;
                ev.channel      = nChannel;
                ev.note.pitch   = nNote;
                ev.note.velocity= uint32_t(level * 127);

                // Store event in MIDI buffer
                midi->push(ev);
            }
        }

        // Handle Note On event
        sampler_kernel::trigger_on(timestamp, level);
    }

    void trigger_kernel::trigger_off(size_t timestamp, float level)
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
        sampler_kernel::trigger_off(timestamp, level);
    }

    void trigger_kernel::bind_midi(IPort *midi_in, IPort *midi_out)
    {
        pMidiIn             = midi_in;
        pMidiOut            = midi_out;
    }

    void trigger_kernel::update_settings()
    {
        // Update settings for notes
        if (pMidiOut != NULL)
        {
            nNote       = (pOctave->getValue() * 12) + pNote->getValue();
            lsp_trace("trigger note=%d", int(nNote));
        }

        // Call the sampler kernel for settings update
        sampler_kernel::update_settings();
    }

    void trigger_kernel::process(float **outs, const float **ins, size_t samples)
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

        // Call sampler kernel
        sampler_kernel::process(outs, ins, samples);
    }

    //-------------------------------------------------------------------------
    trigger_base::trigger_base(const plugin_metadata_t &metadata, size_t triggers, size_t files, size_t channels, bool midi):
        plugin_t(metadata)
    {
        nTriggers       = triggers;
        nFiles          = files;
        nChannels       = channels;
        bMidiPorts      = midi;
        fDry            = 1.0f;
        fWet            = 1.0f;
        vTriggers       = NULL;
        pBuffer         = NULL;

        pMidiIn         = NULL;
        pMidiOut        = NULL;
        pBypass         = NULL;
        pDry            = NULL;
        pWet            = NULL;
        pGain           = NULL;
    }

    trigger_base::~trigger_base()
    {
        destroy();
    }

    void trigger_base::destroy()
    {
        if (vTriggers != NULL)
        {
            delete [] vTriggers;
            vTriggers       = NULL;
        }
        if (pBuffer != NULL)
        {
            delete [] pBuffer;
            pBuffer         = NULL;
        }
    }

    void trigger_base::init(IWrapper *wrapper)
    {
        vTriggers           = new trigger_t[nTriggers];
        if (vTriggers == NULL)
            return;

        // Get executor
        IExecutor *executor = wrapper->get_executor();

        // Initialize audio channels
        for (size_t i=0; i<trigger_base_metadata::TRACKS_MAX; ++i)
        {
            channel_t *c        = &vChannels[i];
            c->vIn              = NULL;
            c->vOut             = NULL;
            c->vTmpIn           = NULL;
            c->vTmpOut          = NULL;
            c->pIn              = NULL;
            c->pOut             = NULL;
        }

        // Initialize triggers
        for (size_t i=0; i<nTriggers; ++i)
        {
            trigger_t *trg      = &vTriggers[i];

            // Initialize trigger
            trg->sKernel.init(executor, nFiles, nChannels);
            trg->sTrigger.init(&trg->sKernel, nChannels);

            trg->fOutGain       = 1.0f;

            trg->pOutGain       = NULL;
            trg->pDry           = NULL;
            trg->pWet           = NULL;
            trg->pDryBypass     = NULL;
            trg->pMixBypass     = NULL;

            // Initialize channels
            for (size_t j=0; j<trigger_base_metadata::TRACKS_MAX; ++j)
            {
                trigger_channel_t  *tc  = &trg->vChannels[j];

                tc->vDry            = NULL;
                tc->fPan            = 1.0f;
                tc->fDry            = 0.0f;
                tc->fWet            = 1.0f;
                tc->pPan            = NULL;
            }
        }

        // Initialize temporary buffers
        size_t allocate         = trigger_base_metadata::BUFFER_SIZE * nChannels * 2; // vTmpIn + vTmpOut
        lsp_trace("Allocating temporary buffer of %d samples", int(allocate));
        pBuffer                 = new float[allocate];
        if (pBuffer == NULL)
            return;

        lsp_trace("Initializing temporary buffers");
        float *fptr             = pBuffer;
        for (size_t i=0; i<nChannels; ++i)
        {
            vChannels[i].vTmpIn     = fptr;
            fptr                   += trigger_base_metadata::BUFFER_SIZE;
            vChannels[i].vTmpOut    = fptr;
            fptr                   += trigger_base_metadata::BUFFER_SIZE;
        }

        // Now we are ready to bind ports
        size_t port_id          = 0;

        // Bind audio inputs
        lsp_trace("Binding audio inputs...");
        for (size_t i=0; i<nChannels; ++i)
        {
            TRACE_PORT(vPorts[port_id]);
            vChannels[i].pIn        = vPorts[port_id++];
            vChannels[i].vIn        = NULL;
        }

        // Bind audio outputs
        lsp_trace("Binding audio outputs...");
        for (size_t i=0; i<nChannels; ++i)
        {
            TRACE_PORT(vPorts[port_id]);
            vChannels[i].pOut       = vPorts[port_id++];
            vChannels[i].vOut       = NULL;
        }

        // Bind MIDI ports
        if (bMidiPorts)
        {
            lsp_trace("Binding MIDI ports...");
            TRACE_PORT(vPorts[port_id]);
            pMidiIn     = vPorts[port_id++];
            TRACE_PORT(vPorts[port_id]);
            pMidiOut    = vPorts[port_id++];
        }

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

        // Now process each trigger
        for (size_t i=0; i<nTriggers; ++i)
        {
            trigger_t *trg  = &vTriggers[i];

            // Bind trigger
            lsp_trace("Binding trigger #%d ports...", int(i));
            port_id     = trg->sTrigger.bind(vPorts, port_id);

            // Bind kernel
            lsp_trace("Binding kernel #%d ports...", int(i));
            port_id     = trg->sKernel.bind(vPorts, port_id);

            // Bind MIDI ports for kernel
            if (bMidiPorts)
                trg->sKernel.bind_midi(pMidiIn, pMidiOut);
        }

        // For more than one trigger bind mixing controls
        if (nTriggers > 1)
        {
            for (size_t i=0; i<nTriggers; ++i)
            {
                trigger_t *trg  = &vTriggers[i];

                // Bind Bypass port
                lsp_trace("Binding bypass ports...");
                TRACE_PORT(vPorts[port_id]);
                trg->pDryBypass     = vPorts[port_id++];
                trg->pMixBypass     = vPorts[port_id++];

                // Bind mixing gain port
                lsp_trace("Binding output gain port...");
                TRACE_PORT(vPorts[port_id]);
                trg->pOutGain       = vPorts[port_id++];

                // Bind panorama port
                if (nChannels > 1)
                {
                    lsp_trace("Binding panorama ports...");
                    for (size_t j=0; j<nChannels; ++j)
                    {
                        TRACE_PORT(vPorts[port_id]);
                        trg->vChannels[j].pPan    = vPorts[port_id++];
                    }
                }

                // Bind dry port if present
                lsp_trace("Binding dry ports...");
                TRACE_PORT(vPorts[port_id]);

                for (size_t j=0; j<nChannels; ++j)
                {
                    TRACE_PORT(vPorts[port_id]);
                    trg->vChannels[j].pDry    = vPorts[port_id++];
                }
            }
        }

        // Call for initial settings update
        lsp_trace("Calling settings update");
        update_settings();
    }

    void trigger_base::update_settings()
    {
        // TODO
    }

    void trigger_base::update_sample_rate(long sr)
    {
        // TODO
    }

    void trigger_base::process(size_t samples)
    {
        // TODO
    }
#endif
}

