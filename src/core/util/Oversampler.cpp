/*
 * Oversampler.cpp
 *
 *  Created on: 19 нояб. 2016 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <core/debug.h>

#include <core/util/Oversampler.h>

#define OS_UP_BUFFER_SIZE       (12 * 1024)   /* Multiple of 3 and 4 */
#define OS_DOWN_BUFFER_SIZE     (12 * 1024)   /* Multiple of 3 and 4 */
#define OS_CUTOFF               21000.0f

namespace lsp
{
    IOversamplerCallback::~IOversamplerCallback()
    {
    }

    void IOversamplerCallback::process(float *out, const float *in, size_t samples)
    {
        dsp::copy(out, in, samples);
    }

    Oversampler::Oversampler()
    {
        pCallback   = NULL;
        fUpBuffer   = NULL;
        fDownBuffer = NULL;
        nUpHead     = 0;
        nMode       = OM_NONE;
        nSampleRate = 0;
        nUpdate     = UP_ALL;
        bData       = NULL;
        bFilter     = true;
    }
    
    Oversampler::~Oversampler()
    {
    }

    bool Oversampler::init()
    {
        if (!sFilter.init(NULL))
            return false;

        if (bData == NULL)
        {
            size_t samples  = OS_UP_BUFFER_SIZE + OS_DOWN_BUFFER_SIZE + RESAMPLING_RESERVED_SAMPLES;
            bData           = new uint8_t[samples * sizeof(float) + DEFAULT_ALIGN];
            if (bData == NULL)
                return false;
            float *ptr      = reinterpret_cast<float *>(ALIGN_PTR(bData, DEFAULT_ALIGN));
            fDownBuffer     = ptr;
            ptr            += OS_DOWN_BUFFER_SIZE;
            fUpBuffer       = reinterpret_cast<float *>(ptr);
            ptr            += OS_UP_BUFFER_SIZE + RESAMPLING_RESERVED_SAMPLES;

            lsp_assert(reinterpret_cast<uint8_t *>(ptr) <= &bData[samples * sizeof(float) + DEFAULT_ALIGN]);
        }

        // Clear buffer
        dsp::fill_zero(fUpBuffer, OS_UP_BUFFER_SIZE + RESAMPLING_RESERVED_SAMPLES);
        dsp::fill_zero(fDownBuffer, OS_DOWN_BUFFER_SIZE);
        nUpHead       = 0;

        return true;
    }

    void Oversampler::destroy()
    {
        sFilter.destroy();
        if (bData != NULL)
        {
            delete [] bData;
            fUpBuffer   = NULL;
            fDownBuffer = NULL;
        }
        pCallback = NULL;
    }

    void Oversampler::set_sample_rate(size_t sr)
    {
        if (sr == nSampleRate)
            return;
        nSampleRate     = sr;
        nUpdate        |= UP_SAMPLE_RATE;
        size_t os       = get_oversampling();

        // Update filter parameters
        filter_params_t fp;
        fp.fFreq        = OS_CUTOFF;        // Calculate cutoff frequency
        fp.fFreq2       = fp.fFreq;
        fp.fGain        = 1.0f;
        fp.fQuality     = 0.5f;
        fp.nSlope       = 30;               // 30 poles = 30 * 3db/oct = 90 db/Oct
        fp.nType        = FLT_BT_BWC_LOPASS;// Chebyshev filter

        sFilter.update(nSampleRate * os, &fp);
    }

    void Oversampler::update_settings()
    {
        if (nUpdate & (UP_MODE | UP_SAMPLE_RATE))
        {
            dsp::fill_zero(fUpBuffer, OS_UP_BUFFER_SIZE + RESAMPLING_RESERVED_SAMPLES);
            nUpHead       = 0;
            sFilter.clear();
        }

        size_t os       = get_oversampling();
        filter_params_t fp;
        sFilter.get_params(&fp);
        sFilter.update(nSampleRate * os, &fp);

        nUpdate = 0;
        return;
    }

    size_t Oversampler::get_oversampling() const
    {
        switch (nMode)
        {
            case OM_LANCZOS_2X2:
            case OM_LANCZOS_2X3:
                return 2;

            case OM_LANCZOS_3X2:
            case OM_LANCZOS_3X3:
                return 3;

            case OM_LANCZOS_4X2:
            case OM_LANCZOS_4X3:
                return 4;

            case OM_LANCZOS_6X2:
            case OM_LANCZOS_6X3:
                return 6;

            case OM_LANCZOS_8X2:
            case OM_LANCZOS_8X3:
                return 8;

            default:
                break;
        }

        return 1;
    }

    void Oversampler::upsample(float *dst, const float *src, size_t samples)
    {
        switch (nMode)
        {
            case OM_LANCZOS_2X2:
            case OM_LANCZOS_2X3:
            {
                while (samples > 0)
                {
                    // Check that there is enough space in buffer
                    size_t can_do   = (OS_UP_BUFFER_SIZE - nUpHead) >> 1;
                    if (can_do <= 0)
                    {
                        dsp::move(fUpBuffer, &fUpBuffer[nUpHead], RESAMPLING_RESERVED_SAMPLES);
                        dsp::fill_zero(&fUpBuffer[RESAMPLING_RESERVED_SAMPLES], OS_UP_BUFFER_SIZE);
                        nUpHead         = 0;
                        can_do          = OS_UP_BUFFER_SIZE >> 1;
                    }

                    size_t to_do    = (samples > can_do) ? can_do : samples;

                    // Do oversampling
                    if (nMode == OM_LANCZOS_2X2)
                        dsp::lanczos_resample_2x2(&fUpBuffer[nUpHead], src, to_do);
                    else
                        dsp::lanczos_resample_2x3(&fUpBuffer[nUpHead], src, to_do);
                    dsp::copy(dst, &fUpBuffer[nUpHead], to_do << 1);

                    // Update pointers
                    nUpHead        += to_do << 1;
                    dst            += to_do << 1;
                    src            += to_do;
                    samples        -= to_do;
                }
                break;
            }

            case OM_LANCZOS_3X2:
            case OM_LANCZOS_3X3:
            {
                while (samples > 0)
                {
                    // Check that there is enough space in buffer
                    size_t can_do   = (OS_UP_BUFFER_SIZE - nUpHead) / 3;
                    if (can_do <= 0)
                    {
                        dsp::move(fUpBuffer, &fUpBuffer[nUpHead], RESAMPLING_RESERVED_SAMPLES);
                        dsp::fill_zero(&fUpBuffer[RESAMPLING_RESERVED_SAMPLES], OS_UP_BUFFER_SIZE);
                        nUpHead         = 0;
                        can_do          = OS_UP_BUFFER_SIZE / 3;
                    }

                    size_t to_do    = (samples > can_do) ? can_do : samples;

                    // Do oversampling
                    if (nMode == OM_LANCZOS_3X2)
                        dsp::lanczos_resample_3x2(&fUpBuffer[nUpHead], src, to_do);
                    else
                        dsp::lanczos_resample_3x3(&fUpBuffer[nUpHead], src, to_do);
                    dsp::copy(dst, &fUpBuffer[nUpHead], to_do * 3);

                    // Update pointers
                    nUpHead        += to_do * 3;
                    dst            += to_do * 3;
                    src            += to_do;
                    samples        -= to_do;
                }
                break;
            }

            case OM_LANCZOS_4X2:
            case OM_LANCZOS_4X3:
            {
                while (samples > 0)
                {
                    // Check that there is enough space in buffer
                    size_t can_do   = (OS_UP_BUFFER_SIZE - nUpHead) >> 2;
                    if (can_do <= 0)
                    {
                        dsp::move(fUpBuffer, &fUpBuffer[nUpHead], RESAMPLING_RESERVED_SAMPLES);
                        dsp::fill_zero(&fUpBuffer[RESAMPLING_RESERVED_SAMPLES], OS_UP_BUFFER_SIZE);
                        nUpHead         = 0;
                        can_do          = OS_UP_BUFFER_SIZE >> 2;
                    }

                    size_t to_do    = (samples > can_do) ? can_do : samples;

                    // Do oversampling
                    if (nMode == OM_LANCZOS_4X2)
                        dsp::lanczos_resample_4x2(&fUpBuffer[nUpHead], src, to_do);
                    else
                        dsp::lanczos_resample_4x3(&fUpBuffer[nUpHead], src, to_do);
                    dsp::copy(dst, &fUpBuffer[nUpHead], to_do << 2);

                    // Update pointers
                    nUpHead        += to_do << 2;
                    dst            += to_do << 2;
                    src            += to_do;
                    samples        -= to_do;
                }
                break;
            }

            case OM_LANCZOS_6X2:
            case OM_LANCZOS_6X3:
            {
                while (samples > 0)
                {
                    // Check that there is enough space in buffer
                    size_t can_do   = (OS_UP_BUFFER_SIZE - nUpHead) / 6;
                    if (can_do <= 0)
                    {
                        dsp::move(fUpBuffer, &fUpBuffer[nUpHead], RESAMPLING_RESERVED_SAMPLES);
                        dsp::fill_zero(&fUpBuffer[RESAMPLING_RESERVED_SAMPLES], OS_UP_BUFFER_SIZE);
                        nUpHead         = 0;
                        can_do          = OS_UP_BUFFER_SIZE / 6;
                    }

                    size_t to_do    = (samples > can_do) ? can_do : samples;

                    // Do oversampling
                    if (nMode == OM_LANCZOS_6X2)
                        dsp::lanczos_resample_6x2(&fUpBuffer[nUpHead], src, to_do);
                    else
                        dsp::lanczos_resample_6x3(&fUpBuffer[nUpHead], src, to_do);
                    dsp::copy(dst, &fUpBuffer[nUpHead], to_do * 6);

                    // Update pointers
                    nUpHead        += to_do * 6;
                    dst            += to_do * 6;
                    src            += to_do;
                    samples        -= to_do;
                }
                break;
            }

            case OM_LANCZOS_8X2:
            case OM_LANCZOS_8X3:
            {
                while (samples > 0)
                {
                    // Check that there is enough space in buffer
                    size_t can_do   = (OS_UP_BUFFER_SIZE - nUpHead) >> 3;
                    if (can_do <= 0)
                    {
                        dsp::move(fUpBuffer, &fUpBuffer[nUpHead], RESAMPLING_RESERVED_SAMPLES);
                        dsp::fill_zero(&fUpBuffer[RESAMPLING_RESERVED_SAMPLES], OS_UP_BUFFER_SIZE);
                        nUpHead         = 0;
                        can_do          = OS_UP_BUFFER_SIZE >> 3;
                    }

                    size_t to_do    = (samples > can_do) ? can_do : samples;

                    // Do oversampling
                    if (nMode == OM_LANCZOS_8X2)
                        dsp::lanczos_resample_8x2(&fUpBuffer[nUpHead], src, to_do);
                    else
                        dsp::lanczos_resample_8x3(&fUpBuffer[nUpHead], src, to_do);
                    dsp::copy(dst, &fUpBuffer[nUpHead], to_do << 3);

                    // Update pointers
                    nUpHead        += to_do << 3;
                    dst            += to_do << 3;
                    src            += to_do;
                    samples        -= to_do;
                }
                break;
            }


            case OM_NONE:
            default:
                dsp::copy(dst, src, samples);
                break;
        }
    }

    void Oversampler::downsample(float *dst, const float *src, size_t samples)
    {
        switch (nMode)
        {
            case OM_LANCZOS_2X2:
            case OM_LANCZOS_2X3:
            {
                while (samples > 0)
                {
                    // Perform filtering
                    size_t can_do   = OS_DOWN_BUFFER_SIZE >> 1;
                    size_t to_do    = (samples > can_do) ? can_do : samples;

                    if (bFilter)
                    {
                        sFilter.process(fDownBuffer, src, to_do << 1);
                        dsp::downsample_2x(dst, fDownBuffer, to_do);
                    }
                    else
                        dsp::downsample_2x(dst, src, to_do);

                    // Update pointers
                    src            += to_do << 1;
                    dst            += to_do;
                    samples        -= to_do;
                }
                break;
            }

            case OM_LANCZOS_3X2:
            case OM_LANCZOS_3X3:
            {
                while (samples > 0)
                {
                    // Perform filtering
                    size_t can_do   = OS_DOWN_BUFFER_SIZE / 3;
                    size_t to_do    = (samples > can_do) ? can_do : samples;

                    if (bFilter)
                    {
                        sFilter.process(fDownBuffer, src, to_do * 3);
                        dsp::downsample_3x(dst, fDownBuffer, to_do);
                    }
                    else
                        dsp::downsample_3x(dst, src, to_do);

                    // Update pointers
                    src            += to_do * 3;
                    dst            += to_do;
                    samples        -= to_do;
                }
                break;
            }

            case OM_LANCZOS_4X2:
            case OM_LANCZOS_4X3:
            {
                while (samples > 0)
                {
                    // Perform filtering
                    size_t can_do   = OS_DOWN_BUFFER_SIZE >> 2;
                    size_t to_do    = (samples > can_do) ? can_do : samples;

                    if (bFilter)
                    {
                        sFilter.process(fDownBuffer, src, to_do << 2);
                        dsp::downsample_4x(dst, fDownBuffer, to_do);
                    }
                    else
                        dsp::downsample_4x(dst, src, to_do);

                    // Update pointers
                    src            += to_do << 2;
                    dst            += to_do;
                    samples        -= to_do;
                }
                break;
            }

            case OM_LANCZOS_6X2:
            case OM_LANCZOS_6X3:
            {
                while (samples > 0)
                {
                    // Perform filtering
                    size_t can_do   = OS_DOWN_BUFFER_SIZE / 6;
                    size_t to_do    = (samples > can_do) ? can_do : samples;

                    // Pack samples to dst
                    if (bFilter)
                    {
                        sFilter.process(fDownBuffer, src, to_do * 6);
                        dsp::downsample_6x(dst, fDownBuffer, to_do);
                    }
                    else
                        dsp::downsample_6x(dst, src, to_do);

                    // Update pointers
                    src            += to_do * 6;
                    dst            += to_do;
                    samples        -= to_do;
                }
                break;
            }

            case OM_LANCZOS_8X2:
            case OM_LANCZOS_8X3:
            {
                while (samples > 0)
                {
                    // Perform filtering
                    size_t can_do   = OS_DOWN_BUFFER_SIZE >> 3;
                    size_t to_do    = (samples > can_do) ? can_do : samples;

                    // Pack samples to dst
                    if (bFilter)
                    {
                        sFilter.process(fDownBuffer, src, to_do << 3);
                        dsp::downsample_8x(dst, fDownBuffer, to_do);
                    }
                    else
                        dsp::downsample_8x(dst, src, to_do);

                    // Update pointers
                    src            += to_do << 3;
                    dst            += to_do;
                    samples        -= to_do;
                }
                break;
            }

            case OM_NONE:
            default:
                dsp::copy(dst, src, samples);
                break;
        }
    }

    void Oversampler::process(float *dst, const float *src, size_t samples, IOversamplerCallback *callback)
    {
        switch (nMode)
        {
            case OM_LANCZOS_2X2:
            case OM_LANCZOS_2X3:
            {
                while (samples > 0)
                {
                    // Check that there is enough space in buffer
                    size_t can_do   = (OS_UP_BUFFER_SIZE - nUpHead) >> 1;
                    if (can_do <= 0)
                    {
                        dsp::move(fUpBuffer, &fUpBuffer[nUpHead], RESAMPLING_RESERVED_SAMPLES);
                        dsp::fill_zero(&fUpBuffer[RESAMPLING_RESERVED_SAMPLES], OS_UP_BUFFER_SIZE);
                        nUpHead         = 0;
                        can_do          = OS_UP_BUFFER_SIZE >> 1;
                    }

                    size_t to_do    = (samples > can_do) ? can_do : samples;

                    // Do oversampling
                    if (nMode == OM_LANCZOS_2X2)
                        dsp::lanczos_resample_2x2(&fUpBuffer[nUpHead], src, to_do);
                    else
                        dsp::lanczos_resample_2x3(&fUpBuffer[nUpHead], src, to_do);

                    // Call handler
                    if (callback != NULL)
                        callback->process(&fUpBuffer[nUpHead], &fUpBuffer[nUpHead], to_do << 1);

                    // Do downsampling
                    if (bFilter)
                        sFilter.process(&fUpBuffer[nUpHead], &fUpBuffer[nUpHead], to_do << 1);
                    dsp::downsample_2x(dst, &fUpBuffer[nUpHead], to_do);

                    // Update pointers
                    nUpHead        += to_do << 1;
                    dst            += to_do << 1;
                    src            += to_do;
                    samples        -= to_do;
                }
                break;
            }

            case OM_LANCZOS_3X2:
            case OM_LANCZOS_3X3:
            {
                while (samples > 0)
                {
                    // Check that there is enough space in buffer
                    size_t can_do   = (OS_UP_BUFFER_SIZE - nUpHead) / 3;
                    if (can_do <= 0)
                    {
                        dsp::move(fUpBuffer, &fUpBuffer[nUpHead], RESAMPLING_RESERVED_SAMPLES);
                        dsp::fill_zero(&fUpBuffer[RESAMPLING_RESERVED_SAMPLES], OS_UP_BUFFER_SIZE);
                        nUpHead         = 0;
                        can_do          = OS_UP_BUFFER_SIZE / 3;
                    }

                    size_t to_do    = (samples > can_do) ? can_do : samples;

                    // Do oversampling
                    if (nMode == OM_LANCZOS_3X2)
                        dsp::lanczos_resample_3x2(&fUpBuffer[nUpHead], src, to_do);
                    else
                        dsp::lanczos_resample_3x3(&fUpBuffer[nUpHead], src, to_do);

                    // Call handler
                    if (callback != NULL)
                        callback->process(&fUpBuffer[nUpHead], &fUpBuffer[nUpHead], to_do * 3);

                    // Do downsampling
                    if (bFilter)
                        sFilter.process(&fUpBuffer[nUpHead], &fUpBuffer[nUpHead], to_do * 3);
                    dsp::downsample_3x(dst, &fUpBuffer[nUpHead], to_do);

                    // Update pointers
                    nUpHead        += to_do * 3;
                    dst            += to_do;
                    src            += to_do;
                    samples        -= to_do;
                }
                break;
            }

            case OM_LANCZOS_4X2:
            case OM_LANCZOS_4X3:
            {
                while (samples > 0)
                {
                    // Check that there is enough space in buffer
                    size_t can_do   = (OS_UP_BUFFER_SIZE - nUpHead) >> 2;
                    if (can_do <= 0)
                    {
                        dsp::move(fUpBuffer, &fUpBuffer[nUpHead], RESAMPLING_RESERVED_SAMPLES);
                        dsp::fill_zero(&fUpBuffer[RESAMPLING_RESERVED_SAMPLES], OS_UP_BUFFER_SIZE);
                        nUpHead         = 0;
                        can_do          = OS_UP_BUFFER_SIZE >> 2;
                    }

                    size_t to_do    = (samples > can_do) ? can_do : samples;

                    // Do oversampling
                    if (nMode == OM_LANCZOS_4X2)
                        dsp::lanczos_resample_4x2(&fUpBuffer[nUpHead], src, to_do);
                    else
                        dsp::lanczos_resample_4x3(&fUpBuffer[nUpHead], src, to_do);

                    // Call handler
                    if (callback != NULL)
                        callback->process(&fUpBuffer[nUpHead], &fUpBuffer[nUpHead], to_do << 2);

                    // Do downsampling
                    if (bFilter)
                        sFilter.process(&fUpBuffer[nUpHead], &fUpBuffer[nUpHead], to_do << 2);
                    dsp::downsample_4x(dst, &fUpBuffer[nUpHead], to_do);

                    // Update pointers
                    nUpHead        += to_do << 2;
                    dst            += to_do;
                    src            += to_do;
                    samples        -= to_do;
                }
                break;
            }

            case OM_LANCZOS_6X2:
            case OM_LANCZOS_6X3:
            {
                while (samples > 0)
                {
                    // Check that there is enough space in buffer
                    size_t can_do   = (OS_UP_BUFFER_SIZE - nUpHead) / 6;
                    if (can_do <= 0)
                    {
                        dsp::move(fUpBuffer, &fUpBuffer[nUpHead], RESAMPLING_RESERVED_SAMPLES);
                        dsp::fill_zero(&fUpBuffer[RESAMPLING_RESERVED_SAMPLES], OS_UP_BUFFER_SIZE);
                        nUpHead         = 0;
                        can_do          = OS_UP_BUFFER_SIZE / 6;
                    }

                    size_t to_do    = (samples > can_do) ? can_do : samples;

                    // Do oversampling
                    if (nMode == OM_LANCZOS_6X2)
                        dsp::lanczos_resample_6x2(&fUpBuffer[nUpHead], src, to_do);
                    else
                        dsp::lanczos_resample_6x3(&fUpBuffer[nUpHead], src, to_do);

                    // Call handler
                    if (callback != NULL)
                        callback->process(&fUpBuffer[nUpHead], &fUpBuffer[nUpHead], to_do * 6);

                    // Do downsampling
                    if (bFilter)
                        sFilter.process(&fUpBuffer[nUpHead], &fUpBuffer[nUpHead], to_do * 6);
                    dsp::downsample_6x(dst, &fUpBuffer[nUpHead], to_do);

                    // Update pointers
                    nUpHead        += to_do * 6;
                    dst            += to_do;
                    src            += to_do;
                    samples        -= to_do;
                }
                break;
            }

            case OM_LANCZOS_8X2:
            case OM_LANCZOS_8X3:
            {
                while (samples > 0)
                {
                    // Check that there is enough space in buffer
                    size_t can_do   = (OS_UP_BUFFER_SIZE - nUpHead) >> 3;
                    if (can_do <= 0)
                    {
                        dsp::move(fUpBuffer, &fUpBuffer[nUpHead], RESAMPLING_RESERVED_SAMPLES);
                        dsp::fill_zero(&fUpBuffer[RESAMPLING_RESERVED_SAMPLES], OS_UP_BUFFER_SIZE);
                        nUpHead         = 0;
                        can_do          = OS_UP_BUFFER_SIZE >> 3;
                    }

                    size_t to_do    = (samples > can_do) ? can_do : samples;

                    // Do oversampling
                    if (nMode == OM_LANCZOS_8X2)
                        dsp::lanczos_resample_8x2(&fUpBuffer[nUpHead], src, to_do);
                    else
                        dsp::lanczos_resample_8x3(&fUpBuffer[nUpHead], src, to_do);

                    // Call handler
                    if (callback != NULL)
                        callback->process(&fUpBuffer[nUpHead], &fUpBuffer[nUpHead], to_do << 3);

                    // Do downsampling
                    if (bFilter)
                        sFilter.process(&fUpBuffer[nUpHead], &fUpBuffer[nUpHead], to_do << 3);
                    dsp::downsample_8x(dst, &fUpBuffer[nUpHead], to_do);

                    // Update pointers
                    nUpHead        += to_do << 3;
                    dst            += to_do;
                    src            += to_do;
                    samples        -= to_do;
                }
                break;
            }

            case OM_NONE:
            default:
                if (callback != NULL)
                    callback->process(dst, src, samples);
                else
                    dsp::copy(dst, src, samples);
                break;
        }
    }

    size_t Oversampler::latency() const
    {
        switch (nMode)
        {
            case OM_LANCZOS_2X2:
            case OM_LANCZOS_3X2:
            case OM_LANCZOS_4X2:
            case OM_LANCZOS_6X2:
            case OM_LANCZOS_8X2:
                return 2;

            case OM_LANCZOS_2X3:
            case OM_LANCZOS_3X3:
            case OM_LANCZOS_4X3:
            case OM_LANCZOS_6X3:
            case OM_LANCZOS_8X3:
                return 3;

            default:
                break;
        }

        return 0;
    }

} /* namespace lsp */
