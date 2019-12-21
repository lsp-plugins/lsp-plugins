/*
 * exp.h
 *
 *  Created on: 2 дек. 2019 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_NATIVE_PMATH_EXP_H_
#define DSP_ARCH_NATIVE_PMATH_EXP_H_

#ifndef __DSP_NATIVE_IMPL
    #error "This header should not be included directly"
#endif /* __DSP_NATIVE_IMPL */

namespace native
{
    void exp1(float *dst, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i]  = ::expf(dst[i]);
    }

    void exp2(float *dst, const float *src, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i]  = ::expf(src[i]);
    }
}

#endif /* DSP_ARCH_NATIVE_PMATH_EXP_H_ */
