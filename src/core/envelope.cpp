/*
 * envelope.cpp
 *
 *  Created on: 20 февр. 2016 г.
 *      Author: sadko
 */

#include <core/envelope.h>
#include <math.h>

namespace lsp
{
    namespace envelope
    {
        const char *envelopes[] =
        {
            "Purple noise",
            "Blue noise",
            "White noise",
            "Pink noise",
            "Brown noise",
            NULL
        };

        void noise(float *dst, size_t n, envelope_t type)
        {
            switch (type)
            {
                case WHITE_NOISE:   white_noise(dst, n);    return;
                case PINK_NOISE:    pink_noise(dst, n);     return;
                case BROWN_NOISE:   brown_noise(dst, n);    return;
                case BLUE_NOISE:    blue_noise(dst, n);     return;
                case PURPLE_NOISE:  purple_noise(dst, n);   return;
                default:
                    return;
            }
        }

        void reverse_noise(float *dst, size_t n, envelope_t type)
        {
            switch (type)
            {
                case WHITE_NOISE:   white_noise(dst, n);    return;
                case PINK_NOISE:    blue_noise(dst, n);     return;
                case BROWN_NOISE:   purple_noise(dst, n);   return;
                case BLUE_NOISE:    pink_noise(dst, n);     return;
                case PURPLE_NOISE:  brown_noise(dst, n);    return;
                default:
                    return;
            }
        }

        void white_noise(float *dst, size_t n)
        {
            while (n--)
                *(dst++)        = 1.0f;
        }

        static void basic_noise(float *dst, size_t n, float k)
        {
            if (n == 0)
                return;

            float step  = logf(SPEC_FREQ_MAX / n);
            dst[0]      = 1.0f;
            for (size_t i=1; i < n; ++i)
                dst[i]      = expf(k * (logf(i) + step));
        }

        void pink_noise(float *dst, size_t n)
        {
            basic_noise(dst, n, -0.6f);
        }

        void brown_noise(float *dst, size_t n)
        {
            basic_noise(dst, n, -1.2f);
        }

        void blue_noise(float *dst, size_t n)
        {
            basic_noise(dst, n, 0.6f);
        }

        void purple_noise(float *dst, size_t n)
        {
            basic_noise(dst, n, 1.2f);
        }

    }
}

