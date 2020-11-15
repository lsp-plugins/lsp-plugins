/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 20 февр. 2016 г.
 *
 * lsp-plugins is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-plugins is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-plugins. If not, see <https://www.gnu.org/licenses/>.
 */

#include <core/envelope.h>
#include <core/stdlib/math.h>

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
            "Fall-off 4.5dB/oct",
            "Raise 4.5dB/oct",
            NULL
        };

        static void basic_noise(float *dst, size_t n, float k)
        {
            if (n == 0)
                return;

            dst[0]      = 1.0f;
            float kd    = (SPEC_FREQ_MAX / SPEC_FREQ_MIN) / n;
            for (size_t i=1; i < n; ++i)
                dst[i]      = expf(k * logf(i * kd));
        }

        void noise(float *dst, size_t n, envelope_t type)
        {
            switch (type)
            {
                case WHITE_NOISE:   white_noise(dst, n);    return;
                case PINK_NOISE:    pink_noise(dst, n);     return;
                case BROWN_NOISE:   brown_noise(dst, n);    return;
                case BLUE_NOISE:    blue_noise(dst, n);     return;
                case VIOLET_NOISE:  violet_noise(dst, n);   return;
                case PLUS_4_5_DB:   basic_noise(dst, n, 4.5 / (20.0 * M_LOG10_2));  return;
                case MINUS_4_5_DB:  basic_noise(dst, n, -4.5 / (20.0 * M_LOG10_2));  return;
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
                case PLUS_4_5_DB:   basic_noise(dst, n, -4.5 / (20.0 * M_LOG10_2));  return;
                case MINUS_4_5_DB:  basic_noise(dst, n, 4.5 / (20.0 * M_LOG10_2));  return;
                default:
                    return;
            }
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

