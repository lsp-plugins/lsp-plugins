/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 21 нояб. 2019 г.
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

#ifndef DSP_COMMON_PMATH_FMOP_VV_H_
#define DSP_COMMON_PMATH_FMOP_VV_H_

#ifndef __DSP_DSP_DEFS
    #error "This header should not be included directly"
#endif /* __DSP_DSP_DEFS */

namespace dsp
{
    /** Calculate dst[i] = dst[i] + a[i] * b[i]
     *
     * @param dst destination array
     * @param a source array
     * @param b source array
     * @param count number of elements
     */
    extern void (* fmadd3)(float *dst, const float *a, const float *b, size_t count);

    /** Calculate dst[i] = dst[i] - a[i] * b[i]
     *
     * @param dst destination array
     * @param a source array
     * @param b source array
     * @param count number of elements
     */
    extern void (* fmsub3)(float *dst, const float *a, const float *b, size_t count);

    /** Calculate dst[i] = a[i] * b[i] - dst[i]
     *
     * @param dst destination array
     * @param a source array
     * @param b source array
     * @param count number of elements
     */
    extern void (* fmrsub3)(float *dst, const float *a, const float *b, size_t count);

    /** Calculate dst[i] = dst[i] * a[i] * b[i]
     *
     * @param dst destination array
     * @param a source array
     * @param b source array
     * @param count number of elements
     */
    extern void (* fmmul3)(float *dst, const float *a, const float *b, size_t count);

    /** Calculate dst[i] = dst[i] / (a[i] * b[i])
     *
     * @param dst destination array
     * @param a source array
     * @param b source array
     * @param count number of elements
     */
    extern void (* fmdiv3)(float *dst, const float *a, const float *b, size_t count);

    /** Calculate dst[i] = (a[i] * b[i]) / dst[i]
     *
     * @param dst destination array
     * @param a source array
     * @param b source array
     * @param count number of elements
     */
    extern void (* fmrdiv3)(float *dst, const float *a, const float *b, size_t count);

    /**
     * Calculate remainder: dst[i] = dst[i] - (a[i]*b[i]) * int(dst[i] / (a[i] * b[i]))
     * @param dst destination array
     * @param a first argument array
     * @param b second argument array
     * @param count number of elements to process
     */
    extern void (* fmmod3)(float *dst, const float *a, const float *b, size_t count);

    /**
     * Calculate reverse remainder: dst[i] = a[i]*b[i] - dst[i] * int((a[i] * b[i]) / dst[i])
     * @param dst destination array
     * @param a first argument array
     * @param b second argument array
     * @param count number of elements to process
     */
    extern void (* fmrmod3)(float *dst, const float *a, const float *b, size_t count);

    /** Calculate dst[i] = a[i] + b[i] * c[i]
     *
     * @param dst destination array
     * @param a source array
     * @param b source array
     * @param c source array
     * @param count number of elements
     */
    extern void (* fmadd4)(float *dst, const float *a, const float *b, const float *c, size_t count);

    /** Calculate dst[i] = a[i] - b[i] * c[i]
     *
     * @param dst destination array
     * @param a source array
     * @param b source array
     * @param c source array
     * @param count number of elements
     */
    extern void (* fmsub4)(float *dst, const float *a, const float *b, const float *c, size_t count);

    /** Calculate dst[i] = b[i] * c[i] - a[i]
     *
     * @param dst destination array
     * @param a source array
     * @param b source array
     * @param c source array
     * @param count number of elements
     */
    extern void (* fmrsub4)(float *dst, const float *a, const float *b, const float *c, size_t count);

    /** Calculate dst[i] = a[i] * b[i] * c[i]
     *
     * @param dst destination array
     * @param a source array
     * @param b source array
     * @param c source array
     * @param count number of elements
     */
    extern void (* fmmul4)(float *dst, const float *a, const float *b, const float *c, size_t count);

    /** Calculate dst[i] = a[i] / (b[i] * c[i])
     *
     * @param dst destination array
     * @param a source array
     * @param b source array
     * @param c source array
     * @param count number of elements
     */
    extern void (* fmdiv4)(float *dst, const float *a, const float *b, const float *c, size_t count);

    /** Calculate dst[i] = (b[i] * c[i]) / a[i]
     *
     * @param dst destination array
     * @param a source array
     * @param b source array
     * @param c source array
     * @param count number of elements
     */
    extern void (* fmrdiv4)(float *dst, const float *a, const float *b, const float *c, size_t count);

    /**
     * Calculate remainder: dst[i] = a[i] - (b[i]*c[i]) * int(a[i] / (b[i] * c[i]))
     * @param dst destination array
     * @param a first argument array
     * @param b second argument array
     * @param c third argument array
     * @param count number of elements to process
     */
    extern void (* fmmod4)(float *dst, const float *a, const float *b, const float *c, size_t count);

    /**
     * Calculate reverse remainder: dst[i] = (b[i]*c[i]) - a[i] * int((b[i] * c[i]) / a[i])
     * @param dst destination array
     * @param a first argument array
     * @param b second argument array
     * @param c third argument array
     * @param count number of elements to process
     */
    extern void (* fmrmod4)(float *dst, const float *a, const float *b, const float *c, size_t count);
}



#endif /* DSP_COMMON_PMATH_FMOP_VV_H_ */
