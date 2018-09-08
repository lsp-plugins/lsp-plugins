/*
 * convolution.h
 *
 *  Created on: 7 сент. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_COMMON_CONVOLUTION_H_
#define DSP_COMMON_CONVOLUTION_H_

namespace dsp
{
    /**
     * Calculate convolution of source signal and convolution and add to destination buffer
     * @param dst destination buffer to add result of convolution
     * @param src source signal
     * @param conv convolution
     * @param length length of convolution
     * @param count the number of samples in source signal to process
     */
    extern void (* convolve)(float *dst, const float *src, const float *conv, size_t length, size_t count);

}



#endif /* DSP_COMMON_CONVOLUTION_H_ */
