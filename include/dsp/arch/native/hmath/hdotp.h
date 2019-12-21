/*
 * hdotp.h
 *
 *  Created on: 29 нояб. 2019 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_NATIVE_HMATH_HDOTP_H_
#define DSP_ARCH_NATIVE_HMATH_HDOTP_H_

#ifndef __DSP_NATIVE_IMPL
    #error "This header should not be included directly"
#endif /* __DSP_NATIVE_IMPL */

namespace native
{
    float h_dotp(const float *a, const float *b, size_t count)
    {
        float result = 0;
        while (count--)
            result += *(a++) * *(b++);
        return result;
    }

    float h_abs_dotp(const float *a, const float *b, size_t count)
    {
        float result = 0;
        while (count--)
            result += ::fabs(*(a++)) * ::fabs(*(b++));
        return result;
    }

    float h_sqr_dotp(const float *a, const float *b, size_t count)
    {
        float result = 0;
        while (count--)
        {
            float xa = *a++, xb = *b++;
            result += xa*xa * xb*xb;
        }
        return result;
    }
}




#endif /* DSP_ARCH_NATIVE_HMATH_HDOTP_H_ */
