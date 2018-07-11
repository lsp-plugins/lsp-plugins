/*
 * dsp.hpp
 *
 *  Created on: 02 окт. 2015 г.
 *      Author: sadko
 */

#ifndef CORE_DSP_H_
#define CORE_DSP_H_

#include <core/types.h>
#include <stddef.h>
#include <math.h>
#include <string.h>

namespace lsp
{
    namespace dsp
    {
        /** Initialize DSP
         *
         */
        void init();

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
        extern void (*move)(float *dst, const float *src, size_t count);

        extern void (* fill)(float *dst, float value, size_t count);
        extern void (* fill_zero)(float *dst, size_t count);
        extern void (* fill_one)(float *dst, size_t count);
        extern void (* fill_minus_one)(float *dst, size_t count);

        /** Calculate absolute values: dst[i] = abs(src[i])
         *
         * @param dst destination vector
         * @param src source vector
         * @param count number of elements
         */
        extern void (* abs)(float *dst, const float *src, size_t count);

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

        /** Calculate min { src }, max { src }
         *
         * @param src source vector
         * @param count number of elements
         * @return maximum value
         */
        extern void (* minmax)(const float *src, size_t count, float *min, float *max);

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

        /** Multiply: dst[i] = src[i] * k
         *
         * @param dst destination
         * @param src source
         * @param k multiplier
         * @param count number of elements
         */
        extern void (* scale)(float *dst, const float *src, float k, size_t count);

        /** Multiply: dst[i] = src1[i] * src2[i]
         *
         * @param dst destination
         * @param src1 first source
         * @param src2 second source
         * @param count number of elements
         */
        extern void (* multiply)(float *dst, const float *src1, const float *src2, size_t count);

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

        /** Calculate dst[i] = dst[i] * k + src[i] * p
         *
         * @param dst accumulator list
         * @param src change list
         * @param k keep value
         * @param p push value
         * @param count number of elements
         */
        extern void (* accumulate)(float *dst, const float *src, float k, float p, size_t count);

        /** Calculate dst[i] = dst[i] + src[i] * k
         *
         * @param dst destination array
         * @param src source array
         * @param k multiplier
         * @param count number of elements
         */
        extern void (* add_multiplied)(float *dst, const float *src, float k, size_t count);

        /** Calculate dst[i] = dst[i] - src[i] * k
         *
         * @param dst destination array
         * @param src source array
         * @param k multiplier
         * @param count number of elements
         */
        extern void (* sub_multiplied)(float *dst, const float *src, float k, size_t count);

        /** Calculate dst[i] = dst[i] * (1 - k) + src[i] * k = dst[i] + (src[i] - dst[i]) * k
         *
         * @param dst destination
         * @param src function value to integrate
         * @param k time factor
         * @param count
         */
        extern void (* integrate)(float *dst, const float *src, float k, size_t count);

        /** Calculate dst[i] = src1[i] * k1 + src2[i] * k2
         *
         * @param dst destination buffer
         * @param src1 source buffer 1
         * @param src2 source buffer 2
         * @param k1 multiplier 1
         * @param k2 multiplier 2
         */
        extern void (* mix)(float *dst, const float *src1, const float *src2, float k1, float k2, size_t count);

        /** Calculate dst[i] = dst[i] + src1[i] * k1 + src2[i] * k2
         *
         * @param dst destination buffer
         * @param src1 source buffer 1
         * @param src2 source buffer 2
         * @param k1 multiplier 1
         * @param k2 multiplier 2
         */
        extern void (* mix_add)(float *dst, const float *src1, const float *src2, float k1, float k2, size_t count);

        /** Apply direct convolution: dst[i] = sum from j=0 to j=length { src[i + j] * conv[i] }
         *
         * @param dst destination buffer
         * @param src source buffer
         * @param conv REVERSE convolution data
         * @param length convolution length
         * @param count number of samples to process
         */
        extern void (* convolve)(float *dst, const float *src, const float *conv, size_t length, size_t count);

        /** Reverse the order of samples: dst[i] <=> dst[count - i - 1]
         *
         * @param dst the buffer to reverse
         * @param count number of samples in buffer
         */
        extern void (* reverse)(float *dst, size_t count);

        /** Direct Fast Fourier Transform
         * @param dst_re real part of spectrum
         * @param dst_im imaginary part of spectrum
         * @param src_re real part of signal
         * @param src_im imaginary part of signal
         * @param rank the rank of FFT
         */
        extern void (* direct_fft)(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);

        /** Reverse Fast Fourier transform
         * @param dst_re real part of signal
         * @param dst_im imaginary part of signal
         * @param src_re real part of spectrum
         * @param src_im imaginary part of spectrum
         * @param rank the rank of FFT
         */
        extern void (* reverse_fft)(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);

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
    }

} /* namespace forzee */

#endif /* CORE_DSP_HPP_ */
