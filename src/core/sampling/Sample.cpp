/*
 * Sample.cpp
 *
 *  Created on: 12 мая 2017 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <core/sampling/Sample.h>

namespace lsp
{
    Sample::Sample()
    {
        vBuffer     = NULL;
        nLength     = 0;
        nMaxLength  = 0;
        nChannels   = 0;
    }

    Sample::~Sample()
    {
        destroy();
    }

    bool Sample::init(size_t channels, size_t max_length, size_t length)
    {
        if (channels <= 0)
            return false;

        // Destroy previous data
        destroy();

        // Allocate new data
        max_length      = ALIGN_SIZE(max_length, DEFAULT_ALIGN);    // Make multiple of 4
        float *buf      = new float[max_length * channels];
        if (buf == NULL)
            return false;
        dsp::fill_zero(buf, max_length * channels);

        vBuffer         = buf;
        nLength         = length;
        nMaxLength      = max_length;
        nChannels       = channels;
        return true;
    }

    bool Sample::resize(size_t channels, size_t max_length, size_t length)
    {
        if (channels <= 0)
            return false;

        // Allocate new data
        max_length      = ALIGN_SIZE(max_length, DEFAULT_ALIGN);    // Make multiple of 4
        float *buf      = new float[max_length * channels];
        if (buf == NULL)
            return false;
        lsp_trace("Allocated buffer=%p of %d floats", buf, int(max_length * channels));

        // Copy previously allocated data
        if (vBuffer != NULL)
        {
            float *dptr         = buf;
            const float *sptr   = vBuffer;
            size_t to_copy      = (nMaxLength > max_length) ? max_length : nMaxLength;

            // Copy channels
            for (size_t ch=0; ch < channels; ++ch)
            {
                if (ch >= nChannels)
                {
                    dsp::fill_zero(dptr, max_length);
                    dptr           += max_length;
                    continue;
                }

                // Copy data and clear data
                dsp::copy(dptr, sptr, to_copy);
                dsp::fill_zero(&dptr[to_copy], max_length - to_copy);

                // Update pointers
                dptr           += max_length;
                sptr           += nMaxLength;
            }

            // Destroy previously allocated data
            destroy();
        }
        else
            dsp::fill_zero(buf, max_length * channels);

        vBuffer         = buf;
        nLength         = length;
        nMaxLength      = max_length;
        nChannels       = channels;
        return true;
    }

    void Sample::destroy()
    {
//        lsp_trace("Sample::destroy this=%p", this);
        if (vBuffer != NULL)
        {
//            lsp_trace("delete [] vBuffer=%p", vBuffer);
            delete [] vBuffer;
            vBuffer     = NULL;
        }
        nMaxLength      = 0;
        nLength         = 0;
        nChannels       = 0;
    }

} /* namespace lsp */
