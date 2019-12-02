/*
 * log.h
 *
 *  Created on: 2 дек. 2019 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_NATIVE_PMATH_LOG_H_
#define DSP_ARCH_NATIVE_PMATH_LOG_H_

#ifndef __DSP_NATIVE_IMPL
    #error "This header should not be included directly"
#endif /* __DSP_NATIVE_IMPL */

namespace native
{
    void logb1(float *dst, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i] = ::logf(dst[i]) * M_LOG2E;
    }

    void logb2(float *dst, const float *src, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i] = ::logf(src[i]) * M_LOG2E;
    }

    void loge1(float *dst, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i] = ::logf(dst[i]);
    }

    void loge2(float *dst, const float *src, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i] = ::logf(src[i]);
    }

    void logd1(float *dst, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i] = ::log10f(dst[i]);
    }

    void logd2(float *dst, const float *src, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i] = ::log10f(src[i]);
    }
}

#endif /* DSP_ARCH_NATIVE_PMATH_LOG_H_ */
