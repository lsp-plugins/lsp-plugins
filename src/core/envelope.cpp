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
            "Violet noise",
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
                case VIOLET_NOISE:  violet_noise(dst, n);   return;
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
                case BROWN_NOISE:   violet_noise(dst, n);   return;
                case BLUE_NOISE:    pink_noise(dst, n);     return;
                case VIOLET_NOISE:  brown_noise(dst, n);    return;
                default:
                    return;
            }
        }

        static void basic_noise(float *dst, size_t n, float k)
        {
            if (n == 0)
                return;

            dst[0]      = 1.0f;
            float kd    = (SPEC_FREQ_MAX / SPEC_FREQ_MIN) / n;
            for (size_t i=1; i < n; ++i)
                dst[i]      = expf(k * logf(i * kd));
        }

        void white_noise(float *dst, size_t n)
        {
            while (n--)
                *(dst++)        = 1.0f;
        }

        void pink_noise(float *dst, size_t n)
        {
            basic_noise(dst, n, logf(0.5) / logf(4));
        }

        void brown_noise(float *dst, size_t n)
        {
            basic_noise(dst, n, -1);
        }

        void blue_noise(float *dst, size_t n)
        {
            basic_noise(dst, n, logf(2) / logf(4));
        }

        void violet_noise(float *dst, size_t n)
        {
            basic_noise(dst, n, 1);
        }

    }
}

