/*
 * Crossfade.cpp
 *
 *  Created on: 22 янв. 2020 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <core/util/Crossfade.h>

namespace lsp
{
    
    Crossfade::Crossfade()
    {
        nSamples    = 0;
        nCounter    = 0;
        fDelta      = 0.0f;
        fGain       = 1.0f;
    }

    Crossfade::~Crossfade()
    {
    }

    void Crossfade::init(int sample_rate, float time)
    {
        ssize_t samples     = sample_rate * time;
        nSamples            = (samples < 1) ? 1 : samples;
    }

    bool Crossfade::toggle()
    {
        if (nCounter > 0)
            return false;

        fDelta      = 1.0f / nSamples;
        fGain       = 0.0f;
        nCounter    = nSamples;

        return true;
    }

    void Crossfade::reset()
    {
        nCounter    = 0;
        fDelta      = 0.0f;
        fGain       = 0.0f;
    }

    void Crossfade::process(float *dst, const float *fade_out, const float *fade_in, size_t count)
    {
        // Skip empty buffers
        if (count == 0)
            return;

        if (fade_out == NULL)
        {
            if (fade_in == NULL)
            {
                size_t delta    = (nCounter < count) ? nCounter : count;
                nCounter       -= delta;
                fGain          += fDelta * delta;

                dsp::fill_zero(dst, count);
                return;
            }

            // Perform crossfade
            while (nCounter > 0)
            {
                *dst            = *fade_in * fGain;
                fGain          += fDelta;

                --nCounter;
                ++fade_in;
                ++dst;
                if ((--count) <= 0)
                    return;
            }

            // Just bypass fade_in to output
            if (fGain > 0.0f)
                dsp::copy(dst, fade_in, count);
            else
                dsp::fill_zero(dst, count);
        }
        else
        {
            if (fade_in == NULL)
            {
                // Perform crossfade
                while (nCounter > 0)
                {
                    *dst            = *fade_out * (1.0f - fGain);
                    fGain          += fDelta;

                    --nCounter;
                    ++fade_out;
                    ++dst;
                    if ((--count) <= 0)
                        return;
                }

                // Fill output
                if (fGain > 0.0f)
                    dsp::fill_zero(dst, count);
                else
                    dsp::copy(dst, fade_out, count);
            }
            else
            {
                // Perform crossfade
                while (nCounter > 0)
                {
                    *dst            = *fade_out + (*fade_in - *fade_out) * fGain;
                    fGain          += fDelta;

                    --nCounter;
                    ++fade_in;
                    ++fade_out;
                    ++dst;
                    if ((--count) <= 0)
                        return;
                }

                // Just bypass fade_in to output
                dsp::copy(dst, (fGain > 0.0f) ? fade_in : fade_out, count);
            }
        }
    }


} /* namespace lsp */
