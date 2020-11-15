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

#ifndef DSP_COMMON_COPY_H_
#define DSP_COMMON_COPY_H_

#ifndef __DSP_DSP_DEFS
    #error "This header should not be included directly"
#endif /* __DSP_DSP_DEFS */

//-----------------------------------------------------------------------
// DSP copying functions
namespace dsp
{
    /** Copy data: dst[i] = src[i]
     *
     * @param dst destination pointer
     * @param src source pointer
     * @param count number of elements
     */
    extern void (*copy)(float *dst, const float *src, size_t count);

    /** Move data: dst[i] = src[i]
     *
     * @param dst destination pointer
     * @param src source pointer
     * @param count number of elements
     */
    extern void (* move)(float *dst, const float *src, size_t count);

    /** Fill data: dst[i] = value
     *
     * @param dst destination pointer
     * @param value filling value
     * @param count number of elements
     */
    extern void (* fill)(float *dst, float value, size_t count);

    /** Fill data with zeros: dst[i] = 0.0f
     *
     * @param dst destination pointer
     * @param count number of elements
     */
    extern void (* fill_zero)(float *dst, size_t count);

    /** Fill data with ones: dst[i] = 1.0f
     *
     * @param dst destination pointer
     * @param count number of elements
     */
    extern void (* fill_one)(float *dst, size_t count);

    /** Fill data with negative ones: dst[i] = -1.0f
     *
     * @param dst destination pointer
     * @param count number of elements
     */
    extern void (* fill_minus_one)(float *dst, size_t count);

    /** Reverse the order of samples: dst[i] <=> dst[count - i - 1]
     *
     * @param dst the buffer to reverse
     * @param count number of samples in buffer
     */
    extern void (* reverse1)(float *dst, size_t count);

    /** Reverse the order of samples: dst[i] <=> src[count - i - 1]
     *
     * @param dst destination buffer to reverse
     * @param src source buffer to reverse
     * @param count number of samples in buffer
     */
    extern void (* reverse2)(float *dst, const float *src, size_t count);
}

#endif /* DSP_COMMON_COPY_H_ */
