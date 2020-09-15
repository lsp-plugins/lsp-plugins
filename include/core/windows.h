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

#ifndef CORE_WINDOWS_H_
#define CORE_WINDOWS_H_

#include <core/types.h>

namespace lsp
{
    namespace windows
    {
        extern const char *windows[];

        enum window_t
        {
            HANN,
            HAMMING,
            BLACKMAN,
            LANCZOS,
            GAUSSIAN,
            POISSON,
            PARZEN,
            TUKEY,
            WELCH,
            NUTTALL,
            BLACKMAN_NUTTALL,
            BLACKMAN_HARRIS,
            HANN_POISSON,
            BARTLETT_HANN,
            BARTLETT_FEJER,
            TRIANGULAR,
            RECTANGULAR,
            FLAT_TOP,
            COSINE,
            SQR_COSINE,
            CUBIC,

            // Special variables
            TOTAL,
            FIRST = HANN,
            LAST = TOTAL - 1
        };

        void window(float *dst, size_t n, window_t type);

        void rectangular(float *dst, size_t n);

        void triangular_general(float *dst, size_t n, int dn);

        void triangular(float *dst, size_t n);

        void bartlett_fejer(float *dst, size_t n);

        void parzen(float *dst, size_t n);

        void welch(float *dst, size_t n);

        void hamming_general(float *dst, size_t n, float a, float b);

        void hann(float *dst, size_t n);

        void hamming(float *dst, size_t n);

        void blackman_general(float *dst, size_t n, float a);

        void blackman(float *dst, size_t n);

        void nutall_general(float *dst, size_t n, float a0, float a1, float a2, float a3);

        void nuttall(float *dst, size_t n);

        void blackman_nuttall(float *dst, size_t n);

        void blackman_harris(float *dst, size_t n);

        void flat_top_general(float *dst, size_t n, float a0, float a1, float a2, float a3, float a4);

        void flat_top(float *dst, size_t n);

        void cosine(float *dst, size_t n);

        void sqr_cosine(float *dst, size_t n);

        void cubic(float *dst, size_t n);

        void gaussian_general(float *dst, size_t n, float s);

        void gaussian(float *dst, size_t n);

        void poisson_general(float *dst, size_t n, float t);

        void poisson(float *dst, size_t n);

        void lanczos(float *dst, size_t n);

        void bartlett_hann_general(float *dst, size_t n, float a0, float a1, float a2);

        void hann_poisson_general(float *dst, size_t n, float a);

        void hann_poisson(float *dst, size_t n);

        void bartlett_hann(float *dst, size_t n);

        void tukey_general(float *dst, size_t n, float a);

        void tukey(float *dst, size_t n);
    }
}

#endif /* CORE_WINDOWS_H_ */
