/*
 * float.h
 *
 *  Created on: 05 авг. 2016 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_NATIVE_FLOAT_H_
#define DSP_ARCH_NATIVE_FLOAT_H_

#ifndef __DSP_NATIVE_IMPL
    #error "This header should not be included directly"
#endif /* __DSP_NATIVE_IMPL */

namespace native
{
    void copy_saturated(float *dst, const float *src, size_t count)
    {
        while (count--)
        {
            float v = *(src++);
            if (isnanf(v))
                v       =   FLOAT_SAT_P_NAN;
            else if (isinff(v))
                v       =   (v < 0.0f) ? FLOAT_SAT_N_INF : FLOAT_SAT_P_INF;

            *(dst++)    = v;
        }
    }

    void saturate(float *dst, size_t count)
    {
        while (count--)
        {
            float v = *dst;
            if (isnanf(v))
                *(dst++)    = FLOAT_SAT_P_NAN;
            else if (isinff(v))
                *(dst++)    = (v < 0.0f) ? FLOAT_SAT_N_INF : FLOAT_SAT_P_INF;
            else
                dst++;
        }
    }

    void avoid_denormals(float *dst, const float *src, size_t count)
    {
        const uint32_t *si  = reinterpret_cast<const uint32_t *>(src);
        uint32_t *di        = reinterpret_cast<uint32_t *>(dst);

        while (count--)
        {
            uint32_t s          = *(si++);
            *(di++)             = ((s & 0x80000000) < 0x00800000) ? 0 : s;
        }
    }
}

#endif /* DSP_ARCH_NATIVE_FLOAT_H_ */
