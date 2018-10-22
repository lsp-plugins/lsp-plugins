/*
 * hmath.h
 *
 *  Created on: 12 авг. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_NATIVE_HMATH_H_
#define DSP_ARCH_NATIVE_HMATH_H_

#ifndef __DSP_NATIVE_IMPL
    #error "This header should not be included directly"
#endif /* __DSP_NATIVE_IMPL */

namespace native
{
    float scalar_mul(const float *a, const float *b, size_t count)
    {
        float result = 0;
        while (count--)
            result += *(a++) * *(b++);
        return result;
    }

    float h_sum(const float *src, size_t count)
    {
        float result    = 0.0f;
        while (count--)
            result         += *(src++);
        return result;
    }

    float h_sqr_sum(const float *src, size_t count)
    {
        float result    = 0.0f;
        while (count--)
        {
            float tmp       = *(src++);
            result         += tmp * tmp;
        }
        return result;
    }

    float h_abs_sum(const float *src, size_t count)
    {
        float result    = 0.0f;
        while (count--)
        {
            float tmp       = *(src++);
            if (tmp < 0.0f)
                result         -= tmp;
            else
                result         += tmp;
        }
        return result;
    }
}

#endif /* DSP_ARCH_NATIVE_HMATH_H_ */
