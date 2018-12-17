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
            if (isnan(v))
                v       =   FLOAT_SAT_P_NAN;
            else if (isinf(v))
                v       =   (v < 0.0f) ? FLOAT_SAT_N_INF : FLOAT_SAT_P_INF;

            *(dst++)    = v;
        }
    }

    void saturate(float *dst, size_t count)
    {
        while (count--)
        {
            float v = *dst;
            if (isnan(v))
                *(dst++)    = FLOAT_SAT_P_NAN;
            else if (isinf(v))
                *(dst++)    = (v < 0.0f) ? FLOAT_SAT_N_INF : FLOAT_SAT_P_INF;
            else
                dst++;
        }
    }

    void limit_saturate1(float *dst, size_t count)
    {
        while (count--)
        {
            float v     = *dst;
            if (isnan(v))
                v       =   FLOAT_SAT_P_NAN;
            else if (isinf(v))
                v       =   (v < 0.0f) ? -1.0f : 1.0f;
            else if (v > 1.0f)
                v       =   1.0f;
            else if (v < -1.0f)
                v       =  -1.0f;

            *(dst++)    = v;
        }
    }

    void limit_saturate2(float *dst, const float *src, size_t count)
    {
        while (count--)
        {
            float v     = *(src++);
            if (isnan(v))
                v       =   FLOAT_SAT_P_NAN;
            else if (isinf(v))
                v       =   (v < 0.0f) ? -1.0f : 1.0f;
            else if (v > 1.0f)
                v       =   1.0f;
            else if (v < -1.0f)
                v       =  -1.0f;

            *(dst++)    = v;
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

    void limit1(float *dst, float min, float max, size_t count)
    {
        while (count--)
        {
            float v     = *dst;
            if (isnan(v))
                v       =   min;
            else if (isinf(v))
                v       =   (v < 0.0f) ? min : max;
            else if (v > max)
                v       =   max;
            else if (v < min)
                v       =  min;

            *(dst++)    = v;
        }
    }

    void limit2(float *dst, const float *src, float min, float max, size_t count)
    {
        while (count--)
        {
            float v     = *(src++);
            if (isnan(v))
                v       =   min;
            else if (isinf(v))
                v       =   (v < 0.0f) ? min : max;
            else if (v > max)
                v       =   max;
            else if (v < min)
                v       =  min;

            *(dst++)    = v;
        }
    }
}

#endif /* DSP_ARCH_NATIVE_FLOAT_H_ */
