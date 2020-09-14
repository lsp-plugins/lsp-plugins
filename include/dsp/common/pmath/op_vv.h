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

#ifndef DSP_COMMON_PMATH_OP_VV_H_
#define DSP_COMMON_PMATH_OP_VV_H_

#ifndef __DSP_DSP_DEFS
    #error "This header should not be included directly"
#endif /* __DSP_DSP_DEFS */

namespace dsp
{
    /** Calculate dst[i] = dst[i] + src[i]
     *
     * @param dst destination array
     * @param src source array
     * @param count number of elements
     */
    extern void (* add2)(float *dst, const float *src, size_t count);

    /** Calculate dst[i] = dst[i] - src[i]
     *
     * @param dst destination array
     * @param src source array
     * @param count number of elements
     */
    extern void (* sub2)(float *dst, const float *src, size_t count);

    /** Calculate dst[i] = src[i] - dst[i]
     *
     * @param dst destination array
     * @param src source array
     * @param count number of elements
     */
    extern void (* rsub2)(float *dst, const float *src, size_t count);

    /** Multiply: dst[i] = dst[i] * src[i]
     *
     * @param dst destination
     * @param src first source
     * @param count number of elements
     */
    extern void (* mul2)(float *dst, const float *src, size_t count);

    /** Divide: dst[i] = dst[i] / src[i]
     *
     * @param dst destination
     * @param src first source
     * @param count number of elements
     */
    extern void (* div2)(float *dst, const float *src, size_t count);

    /** Divide: dst[i] = src[i] / dst[i]
     *
     * @param dst destination
     * @param src first source
     * @param count number of elements
     */
    extern void (* rdiv2)(float *dst, const float *src, size_t count);

    /** Compute remainder: dst[i] = dst[i] - src[i] * int(dst[i]/src[i])
     *
     * @param dst destination
     * @param src source
     * @param count number of elements
     */
    extern void (* mod2)(float *dst, const float *src, size_t count);

    /** Compute reverse remainder: dst[i] = src[i] - dst[i] * int(src[i]/dst[i])
     *
     * @param dst destination
     * @param src source
     * @param count number of elements
     */
    extern void (* rmod2)(float *dst, const float *src, size_t count);

    /** Calculate dst[i] = src1[i] + src2[i]
     *
     * @param dst destination array
     * @param src source array
     * @param count number of elements
     */
    extern void (* add3)(float *dst, const float *src1, const float *src2, size_t count);

    /** Calculate dst[i] = src1[i] - src2[i]
     *
     * @param dst destination array
     * @param src source array
     * @param count number of elements
     */
    extern void (* sub3)(float *dst, const float *src1, const float *src2, size_t count);

    /** Multiply: dst[i] = src1[i] * src2[i]
     *
     * @param dst destination
     * @param src1 first source
     * @param src2 second source
     * @param count number of elements
     */
    extern void (* mul3)(float *dst, const float *src1, const float *src2, size_t count);

    /** Divide: dst[i] = src1[i] / src2[i]
     *
     * @param dst destination
     * @param src1 first source
     * @param src2 second source
     * @param count number of elements
     */
    extern void (* div3)(float *dst, const float *src1, const float *src2, size_t count);

    /** Remainder: dst[i] = src1[i] - src2[i]*int(src1[i]/src2[i]);
     *
     * @param dst destination
     * @param src1 first source
     * @param src2 second source
     * @param count number of elements
     */
    extern void (* mod3)(float *dst, const float *src1, const float *src2, size_t count);

}

#endif /* DSP_COMMON_PMATH_OP_VV_H_ */
