/*
 * Oscilloscope.cpp
 *
 *  Created on: 27 Jan 2020
 *      Author: crocoduck
 */

#include <core/util/Oscilloscope.h>

#define BUFFER_LIM_SIZE 1000 // 196608 // Multiple of 3 4 6 and 8

namespace lsp
{
    Oscilloscope::Oscilloscope()
    {
        nSampleRate                 = -1;
        nOverSampleRate             = -1;

        enTriggerType               = TRG_TYPE_NONE;

        enState                     = OSC_STATE_ACQUIRING;

        sSweepParams.fPreTrigger    = 0.0f;
        sSweepParams.fPostTrigger   = 0.0f;
        sSweepParams.nPreTrigger    = 0;
        sSweepParams.nPostTrigger   = 0;
        sSweepParams.fVertOffset    = 0.0f;
        sSweepParams.fHorOffset     = 0.0f;
        sSweepParams.nSweepLength   = 0;
        sSweepParams.nHead          = 0;
        sSweepParams.bSweepComplete = false;

        sBufferParams.nHead         = 0;
        sBufferParams.nTriggerAt    = 0;
        sBufferParams.nCopyHead     = 0;

        enOverMode                  = OM_NONE;
        nOversampling               = 0;

        enOutputMode                = OSC_OUTPUT_MODE_MUTED;

        vCaptureBuffer              = NULL;
        vSweepBuffer                = NULL;
        pData                       = NULL;

        bSync                       = true;
    }

    Oscilloscope::~Oscilloscope()
    {
    }

    bool Oscilloscope::init()
    {
        // 1X Capture Buffer + 1X Sync Buffer
        size_t samples = 2 * BUFFER_LIM_SIZE;

        float *ptr = alloc_aligned<float>(pData, samples);
        if (ptr == NULL)
            return false;

        lsp_guard_assert(float *save = ptr);

        vCaptureBuffer = ptr;
        ptr += BUFFER_LIM_SIZE;

        vSweepBuffer = ptr;
        ptr += BUFFER_LIM_SIZE;

        lsp_assert(ptr <= &save[samples]);

        return sOver.init() && sTrigger.init();
    }

    void Oscilloscope::destroy()
    {
        free_aligned(pData);
        pData = NULL;
        vCaptureBuffer = NULL;
        vSweepBuffer = NULL;

        sOver.destroy();
    }

    void Oscilloscope::update_settings()
    {
        if (!bSync)
            return;

        sOver.set_sample_rate(nSampleRate);
        sOver.set_mode(enOverMode);
        if (sOver.modified())
            sOver.update_settings();
        nOversampling = sOver.get_oversampling();
        nOverSampleRate = nOversampling * nSampleRate;

//        size_t maxTriggerSide = (BUFFER_LIM_SIZE / 2) - 1;
//
//        sSweepParams.nPreTrigger = seconds_to_samples(nOverSampleRate, sSweepParams.fPreTrigger);
//        sSweepParams.nPreTrigger = (sSweepParams.nPreTrigger < maxTriggerSide) ? sSweepParams.nPreTrigger : maxTriggerSide;
//        sSweepParams.fPreTrigger = samples_to_seconds(nOverSampleRate, sSweepParams.nPreTrigger);
//
//        sSweepParams.nPostTrigger = seconds_to_samples(nOverSampleRate, sSweepParams.fPostTrigger);
//        sSweepParams.nPostTrigger = (sSweepParams.nPostTrigger < maxTriggerSide) ? sSweepParams.nPostTrigger : maxTriggerSide;
//        sSweepParams.fPostTrigger = samples_to_seconds(nOverSampleRate, sSweepParams.nPostTrigger);

        sSweepParams.nPreTrigger = seconds_to_samples(nOverSampleRate, sSweepParams.fPreTrigger);
        sSweepParams.nPreTrigger = (sSweepParams.nPreTrigger < BUFFER_LIM_SIZE) ? sSweepParams.nPreTrigger : BUFFER_LIM_SIZE;
        sSweepParams.fPreTrigger = samples_to_seconds(nOverSampleRate, sSweepParams.nPreTrigger);

        size_t availableForPost = (BUFFER_LIM_SIZE > sSweepParams.nPreTrigger) ? BUFFER_LIM_SIZE - sSweepParams.nPreTrigger - 1 : 0;
        sSweepParams.nPostTrigger = seconds_to_samples(nOverSampleRate, sSweepParams.fPostTrigger);
        sSweepParams.nPostTrigger = (sSweepParams.nPostTrigger < availableForPost) ? sSweepParams.nPostTrigger : availableForPost;
        sSweepParams.fPostTrigger = samples_to_seconds(nOverSampleRate, sSweepParams.nPostTrigger);

        sSweepParams.nSweepLength = sSweepParams.nPreTrigger + sSweepParams.nPostTrigger + 1;
        sSweepParams.nHead = 0;
        sSweepParams.bSweepComplete = false;

        sBufferParams.nHead = 0;
        sBufferParams.nTriggerAt = 0;

        sTrigger.set_post_trigger_samples(sSweepParams.nSweepLength);
        sTrigger.set_trigger_type(enTriggerType);
        sTrigger.update_settings();

        bSync = false;
    }

    // Copying the samples from the past (with respect to the trigger) into the sweep buffer. Excluding trigger sample.
    void Oscilloscope::sweep_from_the_past()
    {
        size_t copyhead; // = wrap_index(sBufferParams.nTriggerAt - sSweepParams.nPreTrigger, CAPTURE_BUFFER_LIMIT_SIZE);

        if (sBufferParams.nTriggerAt >= sSweepParams.nPreTrigger)
            copyhead = sBufferParams.nTriggerAt - sSweepParams.nPreTrigger;
        else
            copyhead = BUFFER_LIM_SIZE - sSweepParams.nPreTrigger + sBufferParams.nTriggerAt;

        if (copyhead <= sBufferParams.nTriggerAt)
        {
            dsp::copy(&vSweepBuffer[sSweepParams.nHead], &vCaptureBuffer[copyhead], sSweepParams.nPreTrigger);
            sSweepParams.nHead += sSweepParams.nPreTrigger;
        }
        else
        {
            dsp::copy(&vSweepBuffer[sSweepParams.nHead], &vCaptureBuffer[copyhead], BUFFER_LIM_SIZE - copyhead);
            sSweepParams.nHead += BUFFER_LIM_SIZE - copyhead;
            dsp::copy(&vSweepBuffer[sSweepParams.nHead], vCaptureBuffer, sBufferParams.nTriggerAt);
            sSweepParams.nHead += sBufferParams.nTriggerAt;
        }
    }

    void Oscilloscope::process(float *dst, float *src, size_t count)
    {
        switch (enOutputMode)
        {
            case OSC_OUTPUT_MODE_COPY:
                dsp::copy(dst, src, count);
                break;
            case OSC_OUTPUT_MODE_MUTED:
            default:
                dsp::fill_zero(dst, count);
                break;
        }

        while (count > 0)
        {
            switch (enState)
            {
                case OSC_STATE_ACQUIRING:
                {
                    size_t to_process = nOversampling * count;
                    size_t remaining_in_buffer = BUFFER_LIM_SIZE - sBufferParams.nHead;
                    size_t to_store = to_process > remaining_in_buffer ? remaining_in_buffer : to_process;
                    to_store = (to_store < sSweepParams.nPostTrigger) ? to_store : sSweepParams.nPostTrigger;
                    size_t to_do = to_store / nOversampling;

                    float* bufptr = &vCaptureBuffer[sBufferParams.nHead];

                    sOver.upsample(bufptr, src, to_do);

                    for (size_t n = 0; n < to_store; ++n)
                    {
                        sTrigger.single_sample_processor(bufptr[n]);

                        trg_state_t tState = sTrigger.get_trigger_state();

                        if (tState == TRG_STATE_FIRED)
                        {
                            enState = OSC_STATE_SWEEPING;
                            sBufferParams.nTriggerAt = sBufferParams.nHead + n;
                            sSweepParams.nHead = 0;
                            sSweepParams.bSweepComplete = false;
                            sweep_from_the_past();
                            sBufferParams.nCopyHead = sBufferParams.nTriggerAt;
                        }
                    }

                    sBufferParams.nHead = (sBufferParams.nHead + to_store) % BUFFER_LIM_SIZE;

                    src     += to_do;
                    count   -= to_do;

                    break;
                }
                case OSC_STATE_SWEEPING:
                {
                    size_t available_in_buffer;
                    size_t remaining_in_sweep;
                    size_t copy_now;

                    if (sBufferParams.nHead <= sBufferParams.nCopyHead)
                    {
                        available_in_buffer = BUFFER_LIM_SIZE - sBufferParams.nCopyHead;
                        remaining_in_sweep = sSweepParams.nSweepLength - sSweepParams.nHead;
                        copy_now = (remaining_in_sweep < available_in_buffer) ? remaining_in_sweep : available_in_buffer;

                        dsp::copy(&vSweepBuffer[sSweepParams.nHead], &vCaptureBuffer[sBufferParams.nCopyHead], copy_now);
                        sSweepParams.nHead += copy_now;
                        sBufferParams.nCopyHead = 0; // (sBufferParams.nCopyHead + CAPTURE_BUFFER_LIMIT_SIZE - sBufferParams.nCopyHead) % CAPTURE_BUFFER_LIMIT_SIZE;
                    }

                    available_in_buffer = sBufferParams.nHead - sBufferParams.nCopyHead;
                    remaining_in_sweep = sSweepParams.nSweepLength - sSweepParams.nHead;
                    copy_now = (remaining_in_sweep < available_in_buffer) ? remaining_in_sweep : available_in_buffer;

                    dsp::copy(&vSweepBuffer[sSweepParams.nHead], &vCaptureBuffer[sBufferParams.nCopyHead], copy_now);
                    sSweepParams.nHead += copy_now;
                    sBufferParams.nCopyHead = (sBufferParams.nCopyHead + copy_now) % BUFFER_LIM_SIZE;

                    size_t to_process = nOversampling * count;
                    size_t remaining = BUFFER_LIM_SIZE - sBufferParams.nHead;
                    size_t to_store = to_process > remaining ? remaining : to_process;
                    to_store = (to_store < sSweepParams.nPostTrigger) ? to_store : sSweepParams.nPostTrigger;
                    size_t to_do = to_store / nOversampling;

                    sOver.upsample(&vCaptureBuffer[sBufferParams.nHead], src, to_do);

                    sBufferParams.nHead = (sBufferParams.nHead + to_store) % BUFFER_LIM_SIZE;

                    src     += to_do;
                    count   -= to_do;

                    if (sSweepParams.nHead >= sSweepParams.nSweepLength - 1)
                    {
                        enState = OSC_STATE_ACQUIRING;
                        sSweepParams.nHead = 0;
                        sSweepParams.bSweepComplete = true;
                    }

                    break;
                }
            }
        }
    }
}
