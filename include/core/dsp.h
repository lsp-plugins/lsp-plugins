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

#define DSP_OPTION_CPU_UNKNOWN          0UL
#define DSP_OPTION_CPU_INTEL            1UL
#define DSP_OPTION_CPU_AMD              2UL
#define DSP_OPTION_CPU_MASK             0x3UL
#define DSP_OPTION_FPU                  (1UL << 2)
#define DSP_OPTION_CMOV                 (1UL << 3)
#define DSP_OPTION_MMX                  (1UL << 4)
#define DSP_OPTION_SSE                  (1UL << 5)
#define DSP_OPTION_SSE2                 (1UL << 6)
#define DSP_OPTION_SSE3                 (1UL << 7)
#define DSP_OPTION_SSSE3                (1UL << 8)
#define DSP_OPTION_SSE4_1               (1UL << 9)
#define DSP_OPTION_SSE4_2               (1UL << 10)
#define DSP_OPTION_SSE4A                (1UL << 11)
#define DSP_OPTION_FMA3                 (1UL << 12)
#define DSP_OPTION_FMA4                 (1UL << 13)
#define DSP_OPTION_AVX                  (1UL << 14)
#define DSP_OPTION_AVX2                 (1UL << 15)

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

        /** Calculate dst[i] = dst[i] + src[i]
         *
         * @param dst destination array
         * @param src source array
         * @param count number of elements
         */
        extern void (* add)(float *dst, const float *src, size_t count);

        /** Calculate dst[i] = dst[i] - src[i]
         *
         * @param dst destination array
         * @param src source array
         * @param count number of elements
         */
        extern void (* sub)(float *dst, const float *src, size_t count);

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

        /** Calculate convolution for the current sample
         *
         * @param src source data
         * @param conv REVERSED convolution data
         * @param count length of convolution
         * @return sample value
         */
        extern float   (* convolve_single)(const float *src, const float *conv, size_t length);

        /** Apply direct convolution: dst[i] = sum from j=0 to j=length { src[i + j] * conv[i] }
         *
         * @param dst destination buffer
         * @param src source buffer
         * @param conv REVERSED convolution data
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

        /** Convert stereo signal to mid-side signal
         *
         * @param m mid signal
         * @param s side signal
         * @param l left signal
         * @param r right signal
         * @param count number of samples to process
         */
        extern void (* lr_to_ms)(float *m, float *s, const float *l, const float *r, size_t count);

        /** Convert mid-side signal to left-right signal
         *
         * @param l left signal
         * @param r right signal
         * @param m mid signal
         * @param s side signal
         * @param count number of samples to process
         */
        extern void (* ms_to_lr)(float *l, float *r, const float *m, const float *s, size_t count);

        /** Process biquad filter
         *
         * @param buf filter delay buffer
         * @param ir impulse response part of filter
         * @param sample sample to process
         * @return processed sample
         */
        extern float (* biquad_process)(float *buf, const float *ir, float sample);

        /** Process biquad filter for multiple samples
         *
         * @param dst destination samples
         * @param src source samples
         * @param count number of samples to process
         * @param buf filter delay buffer
         * @param ir impulse response part of filter
         * @return processed sample
         */
        extern void (* biquad_process_multi)(float *dst, const float *src, size_t count, float *buf, const float *ir);

        /** Do scalar mul for 4-element vector:
         * Restriction: vector has to be 16-byte aligned
         * result = sum {i=0..3} a[i] * b[i]
         *
         * @param a vector a
         * @param b vector b
         * @return scalar multiplication result
         *
         */
        extern float (* vec4_scalar_mul)(const float *a, const float *b);

        /** Push data to scalar vector at end
         * Restriction: vector has to be 16-byte aligned
         * result = v[0]
         * v      = { v[1], v[2], v[3], value }
         *
         * @param v vector to modify
         * @return the value that was removed from vector
         *
         */
        extern float (* vec4_push)(float *v, float value);

        /** Push data to scalar vector at start
         * Restriction: vector has to be 16-byte aligned
         * result = v[3]
         * v      = { value, v[0], v[1], v[2] }
         *
         * @param v vector to modify
         * @return the value that was removed from vector
         *
         */
        extern float (* vec4_unshift)(float *v, float value);

        /** Initialize vector with zero values
         *
         * @param v vector to initialize
         */
        extern void (* vec4_zero)(float *v);

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

        /** Do linear vector apply
         *  x[i] = x[i] + norm_x * v[i]
         *
         * @param x
         * @param y
         * @param v
         * @param norm_x
         * @param norm_y
         * @param count
         */
        extern void (* axis_apply_lin)(float *x, float *y, float norm_x, float norm_y, size_t count);
    }

} /* namespace forzee */

#endif /* CORE_DSP_HPP_ */
