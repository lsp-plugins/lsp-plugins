/*
 * ResponeTaker.cpp
 *
 *  Created on: 30 Jul 2017
 *      Author: crocoduck
 */

#include <dsp/dsp.h>
#include <core/util/ResponseTaker.h>

#define DFL_GAIN    1.0f
#define DFL_FADE    0.01f
#define DFL_PAUSE   0.5f
#define MAX_TAIL    10.0f
#define DFL_TAIL    1.0f    // Make < MAX_TAIL

namespace lsp
{
    ResponseTaker::ResponseTaker()
    {
        nSampleRate = -1;

        sInputProcessor.nState          = IP_BYPASS;
        sInputProcessor.ig_time         = 0;
        sInputProcessor.ig_start        = 0;
        sInputProcessor.ig_stop         = -1;

        sInputProcessor.fAcquire        = 0.0f;
        sInputProcessor.nAcquire        = 0;
        sInputProcessor.nAcquireTime    = 0;

        sOutputProcessor.nState         = OP_BYPASS;
        sOutputProcessor.og_time        = 0;
        sOutputProcessor.og_start       = 0;
        sOutputProcessor.fGain          = DFL_GAIN;
        sOutputProcessor.fGainDelta     = 0.0f;

        sOutputProcessor.fFade          = DFL_FADE;
        sOutputProcessor.nFade          = 0;

        sOutputProcessor.fPause         = DFL_PAUSE;
        sOutputProcessor.nPause         = 0;
        sOutputProcessor.nPauseTime     = 0;

        sOutputProcessor.fTail          = DFL_TAIL;
        sOutputProcessor.nTail          = 0;
        sOutputProcessor.nTailTime      = 0;

        sOutputProcessor.fTestSig       = 0.0f;
        sOutputProcessor.nTestSig       = 0;
        sOutputProcessor.nTestSigTime   = 0;

        pTestSig                        = NULL;
        pCapture                        = NULL;

        nLatency                        = 0;
        nTimeWarp                       = 0;
        nCaptureStart                   = 0;

        bCycleComplete                  = false;

        bSync                           = true;
    }

    ResponseTaker::~ResponseTaker()
    {
    }

    status_t ResponseTaker::reconfigure(Sample *testsig)
    {
        if (bSync)
            update_settings();

        if ((testsig == NULL) || (!testsig->valid()))
            return STATUS_NO_DATA;

        pTestSig                        = testsig;
        size_t nCaptureLength           = testsig->length() + sOutputProcessor.nTail + nLatency;
        size_t nChannels                = testsig->channels();

        bool bReAllocate                = false;

        if ((pCapture == NULL) || (!pCapture->valid()))
        {
            bReAllocate                 = true;
        }
        else if ((pCapture->length() != nCaptureLength) || (pCapture->channels() != nChannels))
        {
            bReAllocate                 = true;
        }

        if (bReAllocate)
        {
            delete pCapture;
            pCapture                    = NULL;

            Sample *s                   = new Sample();

            if (s == NULL)
                return STATUS_NO_MEM;

            if (!s->init(nChannels, nCaptureLength, nCaptureLength))
                return STATUS_NO_MEM;

            pCapture                    = s;
        }

        return STATUS_OK;
    }

    void ResponseTaker::init()
    {
        pCapture = new Sample();
    }

    void ResponseTaker::destroy()
    {
        if (pCapture != NULL)
        {
            delete pCapture;
            pCapture = NULL;
        }
    }

    void ResponseTaker::update_settings()
    {
        if (!bSync)
            return;

        // Processors parameters:
        sOutputProcessor.nFade          = seconds_to_samples(nSampleRate, sOutputProcessor.fFade);

        sOutputProcessor.fGainDelta     = sOutputProcessor.fGain / (sOutputProcessor.nFade + 1);

        sOutputProcessor.nPause         = seconds_to_samples(nSampleRate, sOutputProcessor.fPause);

        if (sOutputProcessor.fTail < 0.0f)
            sOutputProcessor.fTail      = DFL_TAIL;
        sOutputProcessor.fTail          = (sOutputProcessor.fTail < MAX_TAIL) ? sOutputProcessor.fTail : MAX_TAIL;
        sOutputProcessor.nTail          = seconds_to_samples(nSampleRate, sOutputProcessor.fTail);

        bSync                           = false;
    }

    void ResponseTaker::process_in(float *dst, const float *src, size_t count)
    {
        if (bSync)
            update_settings();

        while (count > 0)
        {
            switch (sInputProcessor.nState)
            {
                case IP_ACQUIRE:
                {
                    size_t captureIdx   = sInputProcessor.nAcquireTime % sInputProcessor.nAcquire;
                    size_t to_do        = sInputProcessor.nAcquire - captureIdx;
                    if (to_do > count)
                        to_do = count;

                    dsp::copy(pCapture->getBuffer(0, captureIdx), src, to_do);

                    sInputProcessor.nAcquireTime    += to_do;
                    sInputProcessor.ig_time         += to_do;
                    dst                             += to_do;
                    src                             += to_do;
                    count                           -= to_do;

                    if (sInputProcessor.nAcquireTime >= sInputProcessor.nAcquire)
                    {
                        sInputProcessor.nState      = IP_BYPASS;
                        sOutputProcessor.nState     = OP_FADEIN;
                        sInputProcessor.ig_stop     = sInputProcessor.ig_time;
                        bCycleComplete              = true;
                    }

                    break;
                }
                case IP_WAIT:
                    sInputProcessor.ig_time += count;
                    dsp::copy(dst, src, count);
                    count = 0;
                    break;
                case IP_BYPASS:
                default:
                    dsp::copy(dst, src, count);
                    count = 0;
                    break;
            }
        }
    }

    void ResponseTaker::process_out(float *dst, const float *src, size_t count)
    {
        if (bSync)
            update_settings();

        while (count > 0)
        {
            switch (sOutputProcessor.nState)
            {
                case OP_FADEOUT:
                {
                    while (count > 0)
                    {
                        sOutputProcessor.fGain      -= sOutputProcessor.fGainDelta;

                        if (sOutputProcessor.fGain <= 0.0f)
                        {
                            sOutputProcessor.fGain      = 0.0f;
                            sOutputProcessor.nPauseTime = sOutputProcessor.nPause;
                            sOutputProcessor.nState     = OP_PAUSE;
                            break;
                        }

                        *(dst++) = *(src++) * sOutputProcessor.fGain;
                        count--;
                        sOutputProcessor.og_time++;
                    }
                    break;
                }
                case OP_PAUSE:
                {
                    size_t to_do                    = (sOutputProcessor.nPauseTime > count) ? count : sOutputProcessor.nPauseTime;
                    dsp::fill_zero(dst, to_do);

                    sOutputProcessor.nPauseTime    -= to_do;
                    sOutputProcessor.og_time       += to_do;
                    src                            += to_do;
                    dst                            += to_do;
                    count                          -= to_do;

                    if (sOutputProcessor.nPauseTime <= 0)
                    {
                        sOutputProcessor.nTestSigTime  = 0;
                        sOutputProcessor.nState     = OP_TEST_SIG_EMIT;
                        sInputProcessor.nState      = IP_ACQUIRE;
                        sInputProcessor.nAcquire    = pCapture->length();
                        sInputProcessor.fAcquire    = samples_to_seconds(nSampleRate, sInputProcessor.nAcquire);
                        sOutputProcessor.nTestSig   = pTestSig->length();
                        sOutputProcessor.fTestSig   = samples_to_seconds(nSampleRate, sOutputProcessor.nTestSig);
                        sOutputProcessor.og_start   = sOutputProcessor.og_time;
                        sInputProcessor.ig_start    = sInputProcessor.ig_time;
                        nTimeWarp                   = sInputProcessor.ig_start - sOutputProcessor.og_start;
                        nCaptureStart               = nLatency - nTimeWarp; // This should be the value at which the recorded chirp starts in vCapture, but not sure this formula will always work
                    }
                    break;
                }
                case OP_TEST_SIG_EMIT:
                {
                    size_t playbackIdx              = sOutputProcessor.nTestSigTime % sOutputProcessor.nTestSig;
                    size_t to_do                    = sOutputProcessor.nTestSig - playbackIdx;
                    if (to_do > count)
                        to_do = count;

                    dsp::copy(dst, pTestSig->getBuffer(0, playbackIdx), to_do);

                    sOutputProcessor.nTestSigTime  += to_do;
                    sOutputProcessor.og_time       += to_do;
                    dst                            += to_do;
                    src                            += to_do;
                    count                          -= to_do;

                    if (sOutputProcessor.nTestSigTime >= sOutputProcessor.nTestSig)
                    {
                        sOutputProcessor.nState     = OP_TAIL_EMIT;
                        sOutputProcessor.nTailTime  = 0;
                    }

                    break;
                }
                case OP_TAIL_EMIT:
                {
                    dsp::fill_zero(dst, count);

                    sOutputProcessor.nTailTime     += count;
                    sOutputProcessor.og_time       += count;
                    dst                            += count;
                    src                            += count;
                    count                           = 0;
                    break;
                }
                case OP_FADEIN:
                {
                    while (count > 0)
                    {
                        sOutputProcessor.fGain  += sOutputProcessor.fGainDelta;
                        if (sOutputProcessor.fGain >= 1.0f)
                        {
                            sOutputProcessor.fGain      = 1.0f;
                            sOutputProcessor.nState     = OP_BYPASS;
                            break;
                        }

                        *(dst++) = *(src++) * sOutputProcessor.fGain;
                        count--;
                        sOutputProcessor.og_time++;
                    }
                    break;
                }
                case OP_BYPASS:
                default:
                    dsp::copy(dst, src, count);
                    count = 0;
                    break;
            }
        }
    }

    void ResponseTaker::process(float *dst, const float *src, size_t count)
    {
        process_in(dst, src, count);
        process_out(dst, dst, count);
    }
}
