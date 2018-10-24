/*
 * mix.h
 *
 *  Created on: 12 авг. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_NATIVE_MIX_H_
#define DSP_ARCH_NATIVE_MIX_H_

namespace native
{
    void mix2(float *dst, const float *src, float k1, float k2, size_t count)
    {
        while (count--)
        {
            *dst = *(dst) * k1 + *(src++) * k2;
            dst     ++;
        }
    }

    void mix_copy2(float *dst, const float *src1, const float *src2, float k1, float k2, size_t count)
    {
        while (count--)
            *(dst++) = *(src1++) * k1 + *(src2++) * k2;
    }

    void mix_add2(float *dst, const float *src1, const float *src2, float k1, float k2, size_t count)
    {
        while (count--)
            *(dst++) += *(src1++) * k1 + *(src2++) * k2;
    }

    void mix3(float *dst, const float *src1, const float *src2, float k1, float k2, float k3, size_t count)
    {
        while (count--)
        {
            *dst = *(dst) * k1 + *(src1++) * k2 + *(src2++) * k3;
            dst     ++;
        }
    }

    void mix_copy3(float *dst, const float *src1, const float *src2, const float *src3, float k1, float k2, float k3, size_t count)
    {
        while (count--)
            *(dst++) = *(src1++) * k1 + *(src2++) * k2 + *(src3++) * k3;
    }

    void mix_add3(float *dst, const float *src1, const float *src2, const float *src3, float k1, float k2, float k3, size_t count)
    {
        while (count--)
            *(dst++) += *(src1++) * k1 + *(src2++) * k2 + *(src3++) * k3;
    }

    void mix4(float *dst, const float *src1, const float *src2, const float *src3, float k1, float k2, float k3, float k4, size_t count)
    {
        while (count--)
        {
            *dst = *(dst) * k1 + *(src1++) * k2 + *(src2++) * k3 + *(src3++) * k4;
            dst     ++;
        }
    }

    void mix_copy4(float *dst, const float *src1, const float *src2, const float *src3, const float *src4, float k1, float k2, float k3, float k4, size_t count)
    {
        while (count--)
            *(dst++) = *(src1++) * k1 + *(src2++) * k2 + *(src3++) * k3 + *(src4++) * k4;
    }

    void mix_add4(float *dst, const float *src1, const float *src2, const float *src3, const float *src4, float k1, float k2, float k3, float k4, size_t count)
    {
        while (count--)
            *(dst++) += *(src1++) * k1 + *(src2++) * k2 + *(src3++) * k3 + *(src4++) * k4;
    }
}

#endif /* DSP_ARCH_NATIVE_MIX_H_ */
