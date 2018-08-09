/*
 * dsp.hpp
 *
 *  Created on: 02 окт. 2015 г.
 *      Author: sadko
 */

#ifndef CORE_DSP_H_
#define CORE_DSP_H_

#include <core/types.h>
#include <core/debug.h>
#include <stddef.h>
#include <math.h>
#include <string.h>

#define __CORE_DSP_DEFS

#include <core/dsp/context.h>
#include <core/dsp/filters.h>
#include <core/dsp/resampling.h>
#include <core/dsp/3dmath.h>

#undef __CORE_DSP_DEFS

namespace lsp
{
    // Namespace containing function
    namespace dsp
    {
        /** Copy data: dst[i] = src[i]
         *
         * @param dst destination pointer
         * @param src source pointer
         * @param count number of elements
         */
        extern void (*copy)(float *dst, const float *src, size_t count);

        /** Copy data: dst[i] = saturate(src[i])
         *
         * @param dst destination pointer
         * @param src source pointer
         * @param count number of elements
         */
        extern void (*copy_saturated)(float *dst, const float *src, size_t count);

        /** Saturate data: dst[i] = saturate(src[i])
         *
         * @param dst destination pointer
         * @param src source pointer
         * @param count number of elements
         */
        extern void (* saturate)(float *dst, size_t count);

        /** Move data: dst[i] = src[i]
         *
         * @param dst destination pointer
         * @param src source pointer
         * @param count number of elements
         */
        extern void (* move)(float *dst, const float *src, size_t count);

        extern void (* fill)(float *dst, float value, size_t count);
        extern void (* fill_zero)(float *dst, size_t count);
        extern void (* fill_one)(float *dst, size_t count);
        extern void (* fill_minus_one)(float *dst, size_t count);

        /** Power of floating-point value by integer constant
         *
         * @param x value to power
         * @param deg the power degree
         * @return result of x^deg calculation
         */
        extern float (* ipowf)(float x, int deg);

        /** Calculate the integer root of value
         *
         * @param x the value to calculate
         * @param deg the root degree, should be positive
         * @return the deg'th root of x
         */
        extern float (* irootf)(float x, int deg);

        /** Calculate absolute values: dst[i] = abs(dst[i])
         *
         * @param dst destination vector
         * @param count number of elements
         */
        extern void (* abs1)(float *dst, size_t count);

        /** Calculate absolute values: dst[i] = abs(src[i])
         *
         * @param dst destination vector
         * @param src source vector
         * @param count number of elements
         */
        extern void (* abs2)(float *dst, const float *src, size_t count);

        /** Calculate absolute values: dst[i] = dst[i] + abs(src[i])
         *
         * @param dst destination vector
         * @param src source vector
         * @param count number of elements
         */
        extern void (* abs_add2)(float *dst, const float *src, size_t count);

        /** Calculate absolute values: dst[i] = dst[i] - abs(src[i])
         *
         * @param dst destination vector
         * @param src source vector
         * @param count number of elements
         */
        extern void (* abs_sub2)(float *dst, const float *src, size_t count);

        /** Calculate absolute values: dst[i] = dst[i] * abs(src[i])
         *
         * @param dst destination vector
         * @param src source vector
         * @param count number of elements
         */
        extern void (* abs_mul2)(float *dst, const float *src, size_t count);

        /** Calculate absolute values: dst[i] = dst[i] / abs(src[i])
         *
         * @param dst destination vector
         * @param src source vector
         * @param count number of elements
         */
        extern void (* abs_div2)(float *dst, const float *src, size_t count);

        /** Calculate absolute normalized values: dst[i] = abs(src[i]) / max { abs(src) }
         *
         * @param dst destination vector
         * @param src source vector
         * @param count number of elements
         */
        extern void (* abs_normalized)(float *dst, const float *src, size_t count);

        /** Calculate normalized values: dst[i] = src[i] / (max { abs(src) })
         *
         * @param dst destination vector
         * @param src source vector
         * @param count number of elements
         */
        extern void (* normalize)(float *dst, const float *src, size_t count);

        /** Calculate min { src }
         *
         * @param src source vector
         * @param count number of elements
         * @return minimum value
         */
        extern float (* min)(const float *src, size_t count);

        /** Calculate max { src }
         *
         * @param src source vector
         * @param count number of elements
         * @return maximum value
         */
        extern float (* max)(const float *src, size_t count);

        /** Get absolute maximum: result = max { abs(src[i]) }
         *
         * @param src source array
         * @param count number of elements
         * @return result
         */
        extern float (* abs_max)(const float *src, size_t count);

        /** Get absolute minimum: result = min { abs(src[i]) }
         *
         * @param src source array
         * @param count number of elements
         * @return result
         */
        extern float (* abs_min)(const float *src, size_t count);

        /** Calculate min { src }, max { src }
         *
         * @param src source vector
         * @param count number of elements
         * @return maximum value
         */
        extern void (* minmax)(const float *src, size_t count, float *min, float *max);

        /** Calculate min { abs(src) }, max { abs(src) }
         *
         * @param src source vector
         * @param count number of elements
         * @return maximum value
         */
        extern void (* abs_minmax)(const float *src, size_t count, float *min, float *max);

        /** Calculate @ min { src }
         *
         * @param src source vector
         * @param count number of elements
         * @return minimum value index
         */
        extern size_t (* min_index)(const float *src, size_t count);

        /** Calculate @ max { src }
         *
         * @param src source vector
         * @param count number of elements
         * @return maximum value
         */
        extern size_t (* max_index)(const float *src, size_t count);

        /** Calculate @ minmax { src }
         *
         * @param src source vector
         * @param count number of elements
         * @param min pointer to store minimum value index
         * @param max pointer to store maximum value index
         */
        extern void (* minmax_index)(const float *src, size_t count, size_t *min, size_t *max);

        /** Calculate @ max { abs(src) }
         *
         * @param src source
         * @param count number of samples
         * @return index of maximum element
         */
        extern size_t  (* abs_max_index)(const float *src, size_t count);

        /** Calculate @ min { abs(src) }
         *
         * @param src source
         * @param count number of samples
         * @return index of maximum element
         */
        extern size_t  (* abs_min_index)(const float *src, size_t count);

        /** Calculate @ minmax { abs(src) }
         *
         * @param src source vector
         * @param count number of elements
         * @param min pointer to store absolute minimum value index
         * @param max pointer to store absolute maximum value index
         */
        extern void (* abs_minmax_index)(const float *src, size_t count, size_t *min, size_t *max);

        /** Calculate horizontal sum: result = sum (i) from 0 to count-1 src[i]
         *
         * @param src vector to summarize
         * @param count number of elements
         * @return status of operation
         */
        extern float (* h_sum)(const float *src, size_t count);

        /** Calculate horizontal sum: result = sum (i) from 0 to count-1 sqr(src[i])
         *
         * @param src vector to summarize
         * @param count number of elements
         * @return status of operation
         */
        extern float (* h_sqr_sum)(const float *src, size_t count);

        /** Calculate horizontal sum of absolute values: result = sum (i) from 0 to count-1 abs(src[i])
         *
         * @param src vector to summarize
         * @param count number of elements
         * @return status of operation
         */
        extern float (* h_abs_sum)(const float *src, size_t count);

        /** Calculate sum {from 0 to count-1} (a[i] * b[i])
         *
         * @param a first vector
         * @param b second vector
         * @param count number of elements
         * @return scalar multiplication
         */
        extern float (* scalar_mul)(const float *a, const float *b, size_t count);

        /** Calculate dst[i] = dst[i] + src[i] * k
         *
         * @param dst destination array
         * @param src source array
         * @param k multiplier
         * @param count number of elements
         */
        extern void (* scale_add3)(float *dst, const float *src, float k, size_t count);

        /** Calculate dst[i] = dst[i] - src[i] * k
         *
         * @param dst destination array
         * @param src source array
         * @param k multiplier
         * @param count number of elements
         */
        extern void (* scale_sub3)(float *dst, const float *src, float k, size_t count);

        /** Calculate dst[i] = dst[i] * src[i] * k
         *
         * @param dst destination array
         * @param src source array
         * @param k multiplier
         * @param count number of elements
         */
        extern void (* scale_mul3)(float *dst, const float *src, float k, size_t count);

        /** Calculate dst[i] = dst[i] / (src[i] * k)
         *
         * @param dst destination array
         * @param src source array
         * @param k multiplier
         * @param count number of elements
         */
        extern void (* scale_div3)(float *dst, const float *src, float k, size_t count);

        /** Calculate dst[i] = src1[i] + src2[i] * k
         *
         * @param dst destination array
         * @param src1 source array 1
         * @param src2 source array 2
         * @param k multiplier for elements of array 2
         * @param count number of elements
         */
        extern void (* scale_add4)(float *dst, const float *src1, const float *src2, float k, size_t count);

        /** Calculate dst[i] = src1[i] - src2[i] * k
         *
         * @param dst destination array
         * @param src1 source array 1
         * @param src2 source array 2
         * @param k multiplier for elements of array 2
         * @param count number of elements
         */
        extern void (* scale_sub4)(float *dst, const float *src1, const float *src2, float k, size_t count);

        /** Calculate dst[i] = src1[i] * src2[i] * k
         *
         * @param dst destination array
         * @param src1 source array 1
         * @param src2 source array 2
         * @param k multiplier for elements of array 2
         * @param count number of elements
         */
        extern void (* scale_mul4)(float *dst, const float *src1, const float *src2, float k, size_t count);

        /** Calculate dst[i] = src1[i] / (src2[i] * k)
         *
         * @param dst destination array
         * @param src1 source array 1
         * @param src2 source array 2
         * @param k multiplier for elements of array 2
         * @param count number of elements
         */
        extern void (* scale_div4)(float *dst, const float *src1, const float *src2, float k, size_t count);

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

        /** Scale: dst[i] = dst[i] * k
         *
         * @param dst destination
         * @param src source
         * @param k multiplier
         * @param count number of elements
         */
        extern void (* scale2)(float *dst, float k, size_t count);

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

        /** Scale: dst[i] = src[i] * k
         *
         * @param dst destination
         * @param src source
         * @param k multiplier
         * @param count number of elements
         */
        extern void (* scale3)(float *dst, const float *src, float k, size_t count);

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

        /** Direct Fast Fourier Transform
         * @param dst_re real part of spectrum
         * @param dst_im imaginary part of spectrum
         * @param src_re real part of signal
         * @param src_im imaginary part of signal
         * @param rank the rank of FFT
         */
        extern void (* direct_fft)(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);

        /** Direct Fast Fourier Transform with packed complex data
         * @param dst complex spectrum [re, im, re, im ...]
         * @param src complex signal [re, im, re, im ...]
         * @param rank the rank of FFT
         */
        extern void (* packed_direct_fft)(float *dst, const float *src, size_t rank);

        /** Direct Fast Fourier Transform for convolution
         * @param dst complex spectrum [re, im, re, im ...]
         * @param src real signal, buffer is twice lower size than output (in elements)
         * @param rank the rank of FFT
         */
        extern void (* conv_direct_fft)(float *dst, const float *src, size_t rank);

        /** Reverse Fast Fourier transform
         * @param dst_re real part of signal
         * @param dst_im imaginary part of signal
         * @param src_re real part of spectrum
         * @param src_im imaginary part of spectrum
         * @param rank the rank of FFT
         */
        extern void (* reverse_fft)(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);

        /** Reverse Fast Fourier transform with packed complex data
         * @param dst complex signal [re, im, re, im ...]
         * @param src complex spectrum [re, im, re, im ...]
         * @param rank the rank of FFT
         */
        extern void (* packed_reverse_fft)(float *dst, const float *src, size_t rank);

        /** Normalize FFT coefficients
         *
         * @param dst_re target array for real part of signal
         * @param dst_im target array for imaginary part of signal
         * @param src_re real part of spectrum
         * @param src_im imaginary part of spectrum;
         * @param rank the rank of FFT
         */
        extern void (* normalize_fft)(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);

        /** Center FFT coefficients
         *
         * @param dst_re target array for real part of signal
         * @param dst_im target array for imaginary part of signal
         * @param src_re source array for real part of signal
         * @param src_im source array for imaginary part of signal
         * @param rank rank of FFT
         */
        extern void (* center_fft)(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);

        /** Leave only harmonics with positive frequencies
         *
         * @param dst_re target array for real part of signal
         * @param dst_im target array for imaginary part of signal
         * @param src_re source array for real part of signal
         * @param src_im source array for imaginary part of signal
         * @param rank rank of FFT
         */
        extern void (* combine_fft)(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);

        /** Leave only harmonics with positive frequencies
         *
         * @param dst target array for complex data
         * @param src source array for complex data
         * @param rank rank of FFT
         */
        extern void (* packed_combine_fft)(float *dst, const float *src, size_t rank);

        /** Calculate complex multiplication
         *
         * @param dst_re destination real part
         * @param dst_im destination imaginary part
         * @param src1_re source 1 real part
         * @param src1_im source 1 imaginary part
         * @param src2_re source 2 real part
         * @param src2_im source 2 imaginary part
         * @param count number of multiplications
         */
        extern void (* complex_mul)(
                float *dst_re, float *dst_im,
                const float *src1_re, const float *src1_im,
                const float *src2_re, const float *src2_im,
                size_t count
            );
        
        /** Calculate complex reciprocal: 1 / (re + j * im)
         *
         * @param dst_re source, destination real part
         * @param dst_im source, destination imaginary part
         * @param count number of multiplications
         */
        extern void (* complex_rcp1)(
                float *dst_re, float *dst_im,
                size_t count
            );
        
        /** Calculate complex reciprocal: 1 / (re + j * im)
         *
         * @param dst_re destination real part
         * @param dst_im destination imaginary part
         * @param src_re source real part
         * @param src_im source imaginary part
         * @param count number of multiplications
         */
        extern void (* complex_rcp2)(
                float *dst_re, float *dst_im,
                const float *src_re, const float *src_im,
                size_t count
            );

        /** Calculate packed complex multiplication
         *
         * @param dst destination to store complex numbers
         * @param src1 source 1
         * @param src2 source 2
         * @param count number of multiplications
         */
        extern void (* packed_complex_mul)(float *dst, const float *src1, const float *src2, size_t count);
        
        /** Calculate packed complex reciprocal: 1 / (re + j * im)
         *
         * @param dst source, destination to store complex numbers
         * @param count number of multiplications
         */
        extern void (* packed_complex_rcp1)(float *dst, size_t count);
        
        /** Calculate packed complex reciprocal: 1 / (re + j * im)
         *
         * @param dst destination to store complex numbers
         * @param src source
         * @param count number of multiplications
         */
        extern void (* packed_complex_rcp2)(float *dst, const float *src, size_t count);

        /** Fill output array with same complex numbers
         *
         * @param dst target array to fill
         * @param re real part of complex number
         * @param im imaginary part of complex number
         * @param count number of elements to fill
         */
        extern void (* packed_complex_fill)(float *dst, float re, float im, size_t count);

        /** Convert real to packed complex
         *
         * @param dst destination packed complex data
         * @param src source real data
         * @param count number of items to convert
         */
        extern void (* packed_real_to_complex)(float *dst, const float *src, size_t count);

        /** Convert packed complex to real
         *
         * @param dst destination real data
         * @param src source packed complex data
         * @param count number of items to convert
         */
        extern void (* packed_complex_to_real)(float *dst, const float *src, size_t count);

        /** Convert packed complex to real and add to destination buffer
         *
         * @param dst destination real data
         * @param src source packed complex data
         * @param count number of items to convert
         */
        extern void (* packed_complex_add_to_real)(float *dst, const float *src, size_t count);

        /** Convert real+imaginary complex number to polar form
         *
         * @param dst_mod module of the complex number
         * @param dst_arg argument of the complex number
         * @param src_re real part of complex number
         * @param src_im imaginary part of complex number
         * @param count number of elements to process
         */
        extern void (* complex_cvt2modarg)(
                float *dst_mod, float *dst_arg,
                const float *src_re, const float *src_im,
                size_t count
            );

        /** Get module for complex numbers
         *
         * @param dst_mod array to sore module
         * @param src_re real part of complex number
         * @param src_im imaginary part of complex number
         * @param count number of elements to process
         */
        extern void (* complex_mod)(
                float *dst_mod,
                const float *src_re, const float *src_im,
                size_t count
            );

        /** Get module for complex numbers
         *
         * @param dst_mod array to sore module
         * @param src packed complex number data
         * @param count count number of elements to process
         */
        extern void (* packed_complex_mod)(float *dst_mod, const float *src, size_t count);

        /** Convert polar-form of complex number to real+imaginary
         *
         * @param dst_re real part of complex number
         * @param dst_im imaginary part of complex number
         * @param src_mod module of the complex number
         * @param src_arg argument of the complex number
         * @param count number of elements to process
         */
        extern void (* complex_cvt2reim)(
                float *dst_re, float *dst_im,
                const float *src_mod, const float *src_arg,
                size_t count
            );

        /** Parse input real data to fast convolution data
         *
         * @param dst destination buffer of 2^(rank+1) floats
         * @param src source real data of 2^(rank-1) floats
         * @param rank the convolution rank
         */
        extern void (* fastconv_parse)(float *dst, const float *src, size_t rank);

        /** Parse input real data to fast convolution data,
         *  convolve with another convolution data,
         *  restore data to real data and add to output buffer
         *
         * @param dst target real data of 2^rank floats to store convolved data
         * @param tmp temporary buffer of 2^(rank+1) floats to store intermediate data
         * @param c fast convolution data of 2^(rank+1) floats to apply to the buffer
         * @param src source real data of 2^(rank-1) floats
         * @param rank the convolution rank
         */
        extern void (* fastconv_parse_apply)(float *dst, float *tmp, const float *c, const float *src, size_t rank);

        /** Restore convolution to real data,
         * modifies the source fast convolution data
         *
         * @param dst destination real data of 2^rank floats
         * @param src source fast convolution data of 2^(rank+1) floats
         * @param rank the convolution rank
         */
        extern void (* fastconv_restore)(float *dst, float *src, size_t rank);

        /** Convolve two convolutiona and restore data to real data
         * and add to output buffer
         *
         * @param dst target real data of 2%rank floats to store convolved data
         * @param tmp temporary buffer of 2^(rank+1) floats to store intermediate data
         * @param c1 fast convolution data of 2^(rank+1) floats
         * @param c2 fast convolution data of 2^(rank+1) floats
         * @param rank the convolution rank
         */
        extern void (* fastconv_apply)(float *dst, float *tmp, const float *c1, const float *c2, size_t rank);

        /** Convert stereo signal to mid-side signal
         *
         * @param m mid signal
         * @param s side signal
         * @param l left signal
         * @param r right signal
         * @param count number of samples to process
         */
        extern void (* lr_to_ms)(float *m, float *s, const float *l, const float *r, size_t count);

        /** Convert stereo signal to middle signal
         *
         * @param m mid signal
         * @param l left channel
         * @param r right channel
         * @param count number of samples to process
         */
        extern void (* lr_to_mid)(float *m, const float *l, const float *r, size_t count);

        /** Convert stereo signal to side signal
         *
         * @param s side signal
         * @param l left channel
         * @param r right channel
         * @param count number of samples to process
         */
        extern void (* lr_to_side)(float *s, const float *l, const float *r, size_t count);

        /** Convert mid-side signal to left-right signal
         *
         * @param l left signal
         * @param r right signal
         * @param m mid signal
         * @param s side signal
         * @param count number of samples to process
         */
        extern void (* ms_to_lr)(float *l, float *r, const float *m, const float *s, size_t count);

        /** Convert mid-side signal to left signal
         *
         * @param l left signal
         * @param m mid signal
         * @param s side signal
         * @param count number of samples to process
         */
        extern void (* ms_to_left)(float *l, const float *m, const float *s, size_t count);

        /** Convert mid-side signal to right signal
         *
         * @param r right signal
         * @param m mid signal
         * @param s side signal
         * @param count number of samples to process
         */
        extern void (* ms_to_right)(float *r, const float *m, const float *s, size_t count);

        /** Avoid denormal values
         *
         * @param dst destination buffer
         * @param src source buffer
         * @param count number of samples
         */
        extern void (* avoid_denormals)(float *dst, const float *src, size_t count);

        /** Do logarithmic vector apply:
         *  x[i] = x[i] + norm_x * logf(absf(v[i]*zero))
         *  y[i] = y[i] + norm_y * logf(absf(v[i]*zero))
         *
         * @param x destination vector for X coordinate
         * @param y destination vector for Y coordinate
         * @param v delta vector to apply
         * @param zero graphics zero point
         * @param norm_x X norming factor
         * @param norm_y Y norming factor
         */
        extern void (* axis_apply_log)(float *x, float *y, const float *v, float zero, float norm_x, float norm_y, size_t count);

        /** Convert RGBA32 -> BGRA32 color
         *
         * @param dst target buffer
         * @param src source buffer
         * @param count number of samples to process
         */
        extern void (* rgba32_to_bgra32)(void *dst, const void *src, size_t count);
    }

} /* namespace forzee */

#endif /* CORE_DSP_HPP_ */
