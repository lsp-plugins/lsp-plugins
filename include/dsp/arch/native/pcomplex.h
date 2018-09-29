/*
 * complex.h
 *
 *  Created on: 15 февр. 2017 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_NATIVE_PCOMPLEX_H_
#define DSP_ARCH_NATIVE_PCOMPLEX_H_

#ifndef __DSP_NATIVE_IMPL
    #error "This header should not be included directly"
#endif /* __DSP_NATIVE_IMPL */

namespace native
{
    void pcomplex_mul2(float *dst, const float *src, size_t count)
    {
        while (count--)
        {
            // Use temporaries to prevent dst_re and dst_im to be the same to one of the sources
            float res_re        = (dst[0]) * (src[0]) - (dst[1]) * (src[1]);
            float res_im        = (dst[0]) * (src[1]) + (dst[1]) * (src[0]);

            // Store values
            dst[0]              = res_re;
            dst[1]              = res_im;

            // Update pointers
            src                += 2;
            dst                += 2;
        }
    }

    void pcomplex_mul3(float *dst, const float *src1, const float *src2, size_t count)
    {
        while (count--)
        {
            // Use temporaries to prevent dst_re and dst_im to be the same to one of the sources
            float res_re        = (src1[0]) * (src2[0]) - (src1[1]) * (src2[1]);
            float res_im        = (src1[0]) * (src2[1]) + (src1[1]) * (src2[0]);

            // Store values
            dst[0]              = res_re;
            dst[1]              = res_im;

            // Update pointers
            src1               += 2;
            src2               += 2;
            dst                += 2;
        }
    }

    void pcomplex_rcp1(float *dst, size_t count)
    {
        while (count--)
        {
            float re            = dst[0];
            float im            = dst[1];
            float mag           = 1.0f / (re * re + im * im);

            *(dst++)            = re * mag;
            *(dst++)            = -im * mag;
        }
    }

    void pcomplex_rcp2(float *dst, const float *src, size_t count)
    {
        while (count--)
        {
            float re            = *(src++);
            float im            = *(src++);
            float mag           = 1.0f / (re * re + im * im);

            *(dst++)            = re * mag;
            *(dst++)            = -im * mag;
        }
    }

    void pcomplex_r2c(float *dst, const float *src, size_t count)
    {
        if (dst == src)
        {
            // Do backward copy
            dst        += (count-1) << 1;
            src        += (count-1);
            while (count--)
            {
                dst[0]      = *(src--);
                dst[1]      = 0.0f;
                dst        -= 2;
            }
        }
        else
        {
            // Do forward copy
            while (count--)
            {
                dst[0]      = *(src++);
                dst[1]      = 0.0f;
                dst        += 2;
            }
        }
    }

    void pcomplex_fill_ri(float *dst, float re, float im, size_t count)
    {
        while (count --)
        {
            dst[0]      = re;
            dst[1]      = im;
            dst        += 2;
        }
    }

    void pcomplex_c2r(float *dst, const float *src, size_t count)
    {
        while (count --)
        {
            *(dst++)    = *src;
            src        += 2;
        }
    }

    void pcomplex_c2r_add2(float *dst, const float *src, size_t count)
    {
        while (count --)
        {
            *(dst++)   += *src;
            src        += 2;
        }
    }

    void pcomplex_c2r_sub2(float *dst, const float *src, size_t count)
    {
        while (count --)
        {
            *(dst++)   -= *src;
            src        += 2;
        }
    }

    void pcomplex_c2r_rsub2(float *dst, const float *src, size_t count)
    {
        while (count --)
        {
            *dst        = *src - *dst;
            src        += 2;
            dst        ++;
        }
    }

    void pcomplex_c2r_mul2(float *dst, const float *src, size_t count)
    {
        while (count --)
        {
            *(dst++)   *= *src;
            src        += 2;
        }
    }

    void pcomplex_c2r_div2(float *dst, const float *src, size_t count)
    {
        while (count --)
        {
            *(dst++)   /= *src;
            src        += 2;
        }
    }

    void pcomplex_c2r_rdiv2(float *dst, const float *src, size_t count)
    {
        while (count --)
        {
            *dst        = *src / *dst;
            src        += 2;
            dst        ++;
        }
    }

    void pcomplex_add_r(float *dst, const float *src, size_t count)
    {
        while (count --)
        {
            *dst       += *(src++);
            dst        += 2;
        }
    }

    void pcomplex_mod(float *dst_mod, const float *src, size_t count)
    {
        while (count--)
        {
            float re        = src[0];
            float im        = src[1];
            *(dst_mod++)    = sqrtf(re*re + im*im);
            src            += 2;
        }
    }

    void pcomplex_div2(float *dst, const float *src, size_t count)
    {
        while (count--)
        {
            float re        = src[0] * dst[0] + src[1] * dst[1];
            float im        = src[0] * dst[1] + src[1] * dst[0];
            float n         = 1.0f / (src[0] * src[0] + src[1] * src[1]);

            dst[0]          = re * n;
            dst[1]          = -im * n;
            src            += 2;
            dst            += 2;
        }
    }

    void pcomplex_rdiv2(float *dst, const float *src, size_t count)
    {
        while (count--)
        {
            float re        = src[0] * dst[0] + src[1] * dst[1];
            float im        = src[0] * dst[1] + src[1] * dst[0];
            float n         = 1.0f / (dst[0] * dst[0] + dst[1] * dst[1]);

            dst[0]          = re * n;
            dst[1]          = -im * n;
            src            += 2;
            dst            += 2;
        }
    }

    void pcomplex_div3(float *dst, const float *t, const float *b, size_t count)
    {
        while (count--)
        {
            float re        = t[0] * b[0] + t[1] * b[1];
            float im        = t[0] * b[1] + t[1] * b[0];
            float n         = 1.0f / (b[0] * b[0] + b[1] * b[1]);

            dst[0]          = re * n;
            dst[1]          = -im * n;
            t              += 2;
            b              += 2;
            dst            += 2;
        }
    }

}

#endif /* DSP_ARCH_NATIVE_PCOMPLEX_H_ */
