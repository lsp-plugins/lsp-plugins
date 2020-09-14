/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 10 авг. 2018 г.
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

#ifndef DSP_COMMON_MIX_H_
#define DSP_COMMON_MIX_H_

#ifndef __DSP_DSP_DEFS
    #error "This header should not be included directly"
#endif /* __DSP_DSP_DEFS */

//-----------------------------------------------------------------------
// DSP different mixing functions
namespace dsp
{
    /** Calculate dst[i] = dst[i] * k1 + src[i] * k2
     *
     */
    extern void (* mix2)(float *dst, const float *src, float k1, float k2, size_t count);

    /** Calculate dst[i] = src1[i] * k1 + src2[i] * k2
     *
     */
    extern void (* mix_copy2)(float *dst, const float *src1, const float *src2, float k1, float k2, size_t count);

    /** Calculate dst[i] = dst[i] + src1[i] * k1 + src2[i] * k2
     *
     */
    extern void (* mix_add2)(float *dst, const float *src1, const float *src2, float k1, float k2, size_t count);

    /** Calculate dst[i] = dst[i] * k1 + src1[i] * k2 + src2[i] * k3
     *
     */
    extern void (* mix3)(float *dst, const float *src1, const float *src2, float k1, float k2, float k3, size_t count);

    /** Calculate dst[i] = src1[i] * k1 + src2[i] * k2 + src3 * k3
     *
     */
    extern void (* mix_copy3)(float *dst, const float *src1, const float *src2, const float *src3, float k1, float k2, float k3, size_t count);

    /** Calculate dst[i] = dst[i] + src1[i] * k1 + src2[i] * k2 + src3 * k3
     *
     */
    extern void (* mix_add3)(float *dst, const float *src1, const float *src2, const float *src3, float k1, float k2, float k3, size_t count);

    /** Calculate dst[i] = dst[i] * k1 + src1[i] * k2 + src2[i] * k3 + src3[i] * k4
     *
     */
    extern void (* mix4)(float *dst, const float *src1, const float *src2, const float *src3, float k1, float k2, float k3, float k4, size_t count);

    /** Calculate dst[i] = src1[i] * k1 + src2[i] * k2 + src3 * k3 + src4 * k4
     *
     */
    extern void (* mix_copy4)(float *dst, const float *src1, const float *src2, const float *src3, const float *src4, float k1, float k2, float k3, float k4, size_t count);

    /** Calculate dst[i] = dst[i] + src1[i] * k1 + src2[i] * k2 + src3 * k3 + src4 * k4
     *
     */
    extern void (* mix_add4)(float *dst, const float *src1, const float *src2, const float *src3, const float *src4, float k1, float k2, float k3, float k4, size_t count);
}

#endif /* DSP_COMMON_MIX_H_ */
