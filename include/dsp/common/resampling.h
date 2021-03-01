/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 9 апр. 2017 г.
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

#ifndef DSP_COMMON_RESAMPLING_H_
#define DSP_COMMON_RESAMPLING_H_

#ifndef __DSP_DSP_DEFS
    #error "This header should not be included directly"
#endif /* __DSP_DSP_DEFS */

#define RESAMPLING_RESERVED_SAMPLES         64

/** Resampling/oversampling funtion type.
 * Remember that destination buffer must be times greater and have additional gap (>=64 samples) at
 * the tail to contain complete convolution after resampling
 *
 * @param dst destination buffer
 * @param src source buffer
 * @param count number of samples to process
 */
typedef void (* resampling_function_t)(float *dst, const float *src, size_t count);

//-----------------------------------------------------------------------
// DSP resampling functions
namespace dsp
{
    /** Perform lanczos resampling, destination buffer must be cleared and contain only
     * resampling tail from previous resampling
     *
     * @param dst destination buffer of count*2 samples + RESAMPLING_RESERVED_SAMPLES samples for convolution tail
     * @param src source buffer of count samples
     * @param count number of samples
     */
    extern void (* lanczos_resample_2x2)(float *dst, const float *src, size_t count);

    /** Perform lanczos resampling, destination buffer must be cleared and contain only
     * resampling tail from previous resampling
     *
     * @param dst destination buffer of count*2 samples + RESAMPLING_RESERVED_SAMPLES samples for convolution tail
     * @param src source buffer of count samples
     * @param count number of samples
     */
    extern void (* lanczos_resample_2x3)(float *dst, const float *src, size_t count);

    /** Perform lanczos resampling, destination buffer must be cleared and contain only
     * resampling tail from previous resampling
     *
     * @param dst destination buffer of count*2 samples + RESAMPLING_RESERVED_SAMPLES samples for convolution tail
     * @param src source buffer of count samples
     * @param count number of samples
     */
    extern void (* lanczos_resample_2x4)(float *dst, const float *src, size_t count);

    /** Perform lanczos resampling, destination buffer must be cleared and contain only
     * resampling tail from previous resampling
     *
     * @param dst destination buffer of count*3 samples + RESAMPLING_RESERVED_SAMPLES samples for convolution tail
     * @param src source buffer of count samples
     * @param count number of samples
     */
    extern void (* lanczos_resample_3x2)(float *dst, const float *src, size_t count);

    /** Perform lanczos resampling, destination buffer must be cleared and contain only
     * resampling tail from previous resampling
     *
     * @param dst destination buffer of count*3 samples + RESAMPLING_RESERVED_SAMPLES samples for convolution tail
     * @param src source buffer of count samples
     * @param count number of samples
     */
    extern void (* lanczos_resample_3x3)(float *dst, const float *src, size_t count);

    /** Perform lanczos resampling, destination buffer must be cleared and contain only
     * resampling tail from previous resampling
     *
     * @param dst destination buffer of count*3 samples + RESAMPLING_RESERVED_SAMPLES samples for convolution tail
     * @param src source buffer of count samples
     * @param count number of samples
     */
    extern void (* lanczos_resample_3x4)(float *dst, const float *src, size_t count);

    /** Perform lanczos resampling, destination buffer must be cleared and contain only
     * resampling tail from previous resampling
     *
     * @param dst destination buffer of count*4 samples + RESAMPLING_RESERVED_SAMPLES samples for convolution tail
     * @param src source buffer of count samples
     * @param count number of samples
     */
    extern void (* lanczos_resample_4x2)(float *dst, const float *src, size_t count);

    /** Perform lanczos resampling, destination buffer must be cleared and contain only
     * resampling tail from previous resampling
     *
     * @param dst destination buffer of count*4 samples + RESAMPLING_RESERVED_SAMPLES samples for convolution tail
     * @param src source buffer of count samples
     * @param count number of samples
     */
    extern void (* lanczos_resample_4x3)(float *dst, const float *src, size_t count);

    /** Perform lanczos resampling, destination buffer must be cleared and contain only
     * resampling tail from previous resampling
     *
     * @param dst destination buffer of count*4 samples + RESAMPLING_RESERVED_SAMPLES samples for convolution tail
     * @param src source buffer of count samples
     * @param count number of samples
     */
    extern void (* lanczos_resample_4x4)(float *dst, const float *src, size_t count);

    /** Perform lanczos resampling, destination buffer must be cleared and contain only
     * resampling tail from previous resampling
     *
     * @param dst destination buffer of count*6 samples + RESAMPLING_RESERVED_SAMPLES samples for convolution tail
     * @param src source buffer of count samples
     * @param count number of samples
     */
    extern void (* lanczos_resample_6x2)(float *dst, const float *src, size_t count);

    /** Perform lanczos resampling, destination buffer must be cleared and contain only
     * resampling tail from previous resampling
     *
     * @param dst destination buffer of count*6 samples + RESAMPLING_RESERVED_SAMPLES samples for convolution tail
     * @param src source buffer of count samples
     * @param count number of samples
     */
    extern void (* lanczos_resample_6x3)(float *dst, const float *src, size_t count);

    /** Perform lanczos resampling, destination buffer must be cleared and contain only
     * resampling tail from previous resampling
     *
     * @param dst destination buffer of count*6 samples + RESAMPLING_RESERVED_SAMPLES samples for convolution tail
     * @param src source buffer of count samples
     * @param count number of samples
     */
    extern void (* lanczos_resample_6x4)(float *dst, const float *src, size_t count);

    /** Perform lanczos resampling, destination buffer must be cleared and contain only
     * resampling tail from previous resampling
     *
     * @param dst destination buffer of count*8 samples + RESAMPLING_RESERVED_SAMPLES samples for convolution tail
     * @param src source buffer of count samples
     * @param count number of samples
     */
    extern void (* lanczos_resample_8x2)(float *dst, const float *src, size_t count);

    /** Perform lanczos resampling, destination buffer must be cleared and contain only
     * resampling tail from previous resampling
     *
     * @param dst destination buffer of count*8 samples + RESAMPLING_RESERVED_SAMPLES samples for convolution tail
     * @param src source buffer of count samples
     * @param count number of samples
     */
    extern void (* lanczos_resample_8x3)(float *dst, const float *src, size_t count);

    /** Perform lanczos resampling, destination buffer must be cleared and contain only
     * resampling tail from previous resampling
     *
     * @param dst destination buffer of count*8 samples + RESAMPLING_RESERVED_SAMPLES samples for convolution tail
     * @param src source buffer of count samples
     * @param count number of samples
     */
    extern void (* lanczos_resample_8x4)(float *dst, const float *src, size_t count);

    /** Copy each even sample to output buffer
     *
     * @param dst destination buffer
     * @param src source buffer
     * @param count number of samples to process
     */
    extern void (* downsample_2x)(float *dst, const float *src, size_t count);

    /** Copy each 3rd sample to output buffer
     *
     * @param dst destination buffer
     * @param src source buffer
     * @param count number of samples to process
     */
    extern void (* downsample_3x)(float *dst, const float *src, size_t count);

    /** Copy each 4th sample to output buffer
     *
     * @param dst destination buffer
     * @param src source buffer
     * @param count number of samples to process
     */
    extern void (* downsample_4x)(float *dst, const float *src, size_t count);

    /** Copy each 6th sample to output buffer
     *
     * @param dst destination buffer
     * @param src source buffer
     * @param count number of samples to process
     */
    extern void (* downsample_6x)(float *dst, const float *src, size_t count);

    /** Copy each 8th sample to output buffer
     *
     * @param dst destination buffer
     * @param src source buffer
     * @param count number of samples to process
     */
    extern void (* downsample_8x)(float *dst, const float *src, size_t count);
}

#endif /* DSP_COMMON_RESAMPLING_H_ */
