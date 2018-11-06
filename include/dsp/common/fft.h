/*
 * fft.h
 *
 *  Created on: 10 авг. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_COMMON_FFT_H_
#define DSP_COMMON_FFT_H_

#ifndef __DSP_DSP_DEFS
    #error "This header should not be included directly"
#endif /* __DSP_DSP_DEFS */

//-----------------------------------------------------------------------
// DSP fast fourier transform (FFT) routines
namespace dsp
{
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
    extern void (* normalize_fft3)(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);

    /** Normalize FFT coefficients
     *
     * @param re target array for real part of signal
     * @param im target array for imaginary part of signal
     * @param rank the rank of FFT
     */
    extern void (* normalize_fft2)(float *re, float *im, size_t rank);

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

}

#endif /* DSP_COMMON_FFT_H_ */
