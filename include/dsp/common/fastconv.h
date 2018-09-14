/*
 * fastconv.h
 *
 *  Created on: 10 авг. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_COMMON_FASTCONV_H_
#define DSP_COMMON_FASTCONV_H_

#ifndef __DSP_DSP_DEFS
    #error "This header should not be included directly"
#endif /* __DSP_DSP_DEFS */

//-----------------------------------------------------------------------
// DSP fast convolution routines
namespace dsp
{
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

    /** Convolve two convolutions and restore data to real data
     * and add to output buffer
     *
     * @param dst target real data of 2^rank floats to apply convolved data
     * @param tmp temporary buffer of 2^(rank+1) floats to store intermediate data
     * @param c1 fast convolution data of 2^(rank+1) floats
     * @param c2 fast convolution data of 2^(rank+1) floats
     * @param rank the convolution rank
     */
    extern void (* fastconv_apply)(float *dst, float *tmp, const float *c1, const float *c2, size_t rank);
}

#endif /* DSP_COMMON_FASTCONV_H_ */
