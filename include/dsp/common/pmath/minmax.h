/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 5 июл. 2020 г.
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

#ifndef DSP_COMMON_PMATH_MINMAX_H_
#define DSP_COMMON_PMATH_MINMAX_H_

namespace dsp
{
    /**
     * Compute minimum values between two arrays:
     *   dst[i] = min(dst[i], src[i])
     * @param dst destination array
     * @param src source array
     * @param count number of elements in each array
     */
    extern void (* pmin2)(float *dst, const float *src, size_t count);

    /**
     * Compute minimum values between two arrays with disregarded sign:
     *   dst[i] = (abs(dst[i]) < abs(src[i])) ? dst[i] : src[i]
     * @param dst destination array
     * @param src source array
     * @param count number of elements in each array
     */
    extern void (* psmin2)(float *dst, const float *src, size_t count);

    /**
     * Compute absolute minimum values between two arrays:
     *   dst[i] = (abs(dst[i]) < abs(src[i])) ? dst[i] : src[i]
     * @param dst destination array
     * @param src source array
     * @param count number of elements in each array
     */
    extern void (* pamin2)(float *dst, const float *src, size_t count);

    /**
     * Compute maximum values between two arrays:
     *   dst[i] = max(dst[i], src[i])
     * @param dst destination array
     * @param src source array
     * @param count number of elements in each array
     */
    extern void (* pmax2)(float *dst, const float *src, size_t count);

    /**
     * Compute maximum values between two arrays with disregarded sign:
     *   dst[i] = (abs(dst[i]) < abs(src[i])) ? src[i] : dst[i]
     * @param dst destination array
     * @param src source array
     * @param count number of elements in each array
     */
    extern void (* psmax2)(float *dst, const float *src, size_t count);

    /**
     * Compute absolute maximum values between two arrays:
     *   dst[i] = (abs(dst[i]) < abs(src[i])) ? src[i] : dst[i]
     * @param dst destination array
     * @param src source array
     * @param count number of elements in each array
     */
    extern void (* pamax2)(float *dst, const float *src, size_t count);

    /**
     * Compute minimum values between two arrays:
     *   dst[i] = min(a[i], b[i])
     * @param dst destination array
     * @param src source array
     * @param count number of elements in each array
     */
    extern void (* pmin3)(float *dst, const float *a, const float *b, size_t count);

    /**
     * Compute minimum values between two arrays with disregarded sign:
     *   dst[i] = (abs(a[i]) < abs(b[i])) ? a[i] : b[i]
     * @param dst destination array
     * @param src source array
     * @param count number of elements in each array
     */
    extern void (* psmin3)(float *dst, const float *a, const float *b, size_t count);

    /**
     * Compute absolute minimum values between two arrays:
     *   dst[i] = (abs(a[i]) < abs(b[i])) ? a[i] : b[i]
     * @param dst destination array
     * @param src source array
     * @param count number of elements in each array
     */
    extern void (* pamin3)(float *dst, const float *a, const float *b, size_t count);

    /**
     * Compute maximum values between two arrays:
     *   dst[i] = max(a[i], b[i])
     * @param dst destination array
     * @param src source array
     * @param count number of elements in each array
     */
    extern void (* pmax3)(float *dst, const float *a, const float *b, size_t count);

    /**
     * Compute maximum values between two arrays with disregarded sign:
     *   dst[i] = (abs(a[i]) < b(src[i])) ? b[i] : a[i]
     * @param dst destination array
     * @param src source array
     * @param count number of elements in each array
     */
    extern void (* psmax3)(float *dst, const float *a, const float *b, size_t count);

    /**
     * Compute absolute minimum values between two arrays:
     *   dst[i] = (abs(a[i]) < abs(b[i])) ? b[i] : a[i]
     * @param dst destination array
     * @param src source array
     * @param count number of elements in each array
     */
    extern void (* pamax3)(float *dst, const float *a, const float *b, size_t count);
}



#endif /* DSP_COMMON_PMATH_MINMAX_H_ */
