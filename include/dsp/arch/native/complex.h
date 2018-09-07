/*
 * complex.h
 *
 *  Created on: 15 февр. 2017 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_NATIVE_COMPLEX_H_
#define DSP_ARCH_NATIVE_COMPLEX_H_

#ifndef __DSP_NATIVE_IMPL
    #error "This header should not be included directly"
#endif /* __DSP_NATIVE_IMPL */

namespace native
{
    void complex_mul2(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t count)
    {
        for (size_t i=0; i<count; ++i)
        {
            float re            = (dst_re[i]) * (src_re[i]) - (dst_im[i]) * (src_im[i]);
            float im            = (dst_re[i]) * (src_im[i]) + (dst_im[i]) * (src_re[i]);
            dst_re[i]           = re;
            dst_im[i]           = im;
        }
    }

    void complex_mul3(float *dst_re, float *dst_im, const float *src1_re, const float *src1_im, const float *src2_re, const float *src2_im, size_t count)
    {
        for (size_t i=0; i<count; ++i)
        {
            float re            = (src1_re[i]) * (src2_re[i]) - (src1_im[i]) * (src2_im[i]);
            float im            = (src1_re[i]) * (src2_im[i]) + (src1_im[i]) * (src2_re[i]);
            dst_re[i]           = re;
            dst_im[i]           = im;
        }
    }

    void complex_rcp1(float *dst_re, float *dst_im, size_t count)
    {
        while (count--)
        {
            float re            = *dst_re;
            float im            = *dst_im;
            float mag           = 1.0f / (re * re + im * im);

            *(dst_re++)         = re * mag;
            *(dst_im++)         = -im * mag;
        }
    }

    void complex_rcp2(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t count)
    {
        while (count--)
        {
            float re            = *(src_re++);
            float im            = *(src_im++);
            float mag           = 1.0f / (re * re + im * im);

            *(dst_re++)         = re * mag;
            *(dst_im++)         = -im * mag;
        }
    }

    void complex_cvt2modarg(float *dst_mod, float *dst_arg, const float *src_re, const float *src_im, size_t count)
    {
        while (count--)
        {
            float re        = *(src_re++);
            float im        = *(src_im++);
            float re2       = re * re;
            float im2       = im * im;
            float mod       = sqrtf(re2 + im2);
            float arg;

            if (re2 > im2)
            {
                if (im >= 0)
                    arg = acos(re / mod);
                else
                    arg = 2 * M_PI - acos(re / mod);
            }
            else
            {
                if (re > 0)
                {
                    if (im >= 0)
                        arg = asin(im / mod);
                    else
                        arg = 2*M_PI + asin(im / mod);
                }
                else
                    arg = M_PI - asin(im / mod);
            }

            *(dst_mod++)    = mod;
            *(dst_arg++)    = arg;
        }
    }

    void complex_cvt2reim(float *dst_re, float *dst_im, const float *src_mod, const float *src_arg, size_t count)
    {
        while (count--)
        {
            float mod       = *(src_mod++);
            float arg       = *(src_arg++);
            *(dst_re++)     = mod * cosf(arg);
            *(dst_im++)     = mod * sinf(arg);
        }
    }

    void complex_mod(float *dst_mod, const float *src_re, const float *src_im, size_t count)
    {
        while (count--)
        {
            float re        = *(src_re++);
            float im        = *(src_im++);
            *(dst_mod++)    = sqrtf(re*re + im*im);
        }
    }

    void complex_div2(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t count)
    {
        for (size_t i=0; i<count; ++i)
        {
            float re        = src_re[i] * dst_re[i] + src_im[i] * dst_im[i];
            float im        = src_re[i] * dst_im[i] + src_im[i] * dst_re[i];
            float n         = 1.0f / (src_re[i] * src_re[i] + src_im[i] * src_im[i]);

            dst_re[i]       = re * n;
            dst_im[i]       = -im * n;
        }
    }

    void complex_rdiv2(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t count)
    {
        for (size_t i=0; i<count; ++i)
        {
            float re        = src_re[i] * dst_re[i] + src_im[i] * dst_im[i];
            float im        = src_re[i] * dst_im[i] + src_im[i] * dst_re[i];
            float n         = 1.0f / (dst_re[i] * dst_re[i] + dst_im[i] * dst_im[i]);

            dst_re[i]       = re * n;
            dst_im[i]       = -im * n;
        }
    }

    void complex_div3(float *dst_re, float *dst_im, const float *t_re, const float *t_im, const float *b_re, const float *b_im, size_t count)
    {
        for (size_t i=0; i<count; ++i)
        {
            float re        = t_re[i] * b_re[i] + t_im[i] * b_im[i];
            float im        = t_re[i] * b_im[i] + t_im[i] * b_re[i];
            float n         = 1.0f / (b_re[i] * b_re[i] + b_im[i] * b_im[i]);

            dst_re[i]       = re * n;
            dst_im[i]       = -im * n;
        }
    }

}

#endif /* DSP_ARCH_NATIVE_COMPLEX_H_ */
