/*
 * complex.h
 *
 *  Created on: 15 февр. 2017 г.
 *      Author: sadko
 */

#ifndef CORE_NATIVE_COMPLEX_H_
#define CORE_NATIVE_COMPLEX_H_

#ifndef __DSP_NATIVE_IMPL
    #error "This header should not be included directly"
#endif /* __DSP_NATIVE_IMPL */

namespace lsp
{
    namespace native
    {
        void complex_mul(float *dst_re, float *dst_im, const float *src1_re, const float *src1_im, const float *src2_re, const float *src2_im, size_t count)
        {
            while (count--)
            {
                // Use temporaries to prevent dst_re and dst_im to be the same to one of the sources
                float res_re        = (*src1_re) * (*src2_re) - (*src1_im) * (*src2_im);
                float res_im        = (*src1_re) * (*src2_im) + (*src1_im) * (*src2_re);

                // Store values
                *dst_re             = res_re;
                *dst_im             = res_im;

                // Update pointers
                dst_re              ++;
                dst_im              ++;
                src1_re             ++;
                src1_im             ++;
                src2_re             ++;
                src2_im             ++;
            }
        }

        void packed_complex_mul(float *dst, const float *src1, const float *src2, size_t count)
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

        void packed_real_to_complex(float *dst, const float *src, size_t count)
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

        void packed_complex_to_real(float *dst, const float *src, size_t count)
        {
            while (count --)
            {
                *(dst++)    = *src;
                src        += 2;
            }
        }

        void packed_complex_add_to_real(float *dst, const float *src, size_t count)
        {
            while (count --)
            {
                *(dst++)   += *src;
                src        += 2;
            }
        }

        static void complex_cvt2modarg(float *dst_mod, float *dst_arg, const float *src_re, const float *src_im, size_t count)
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

        static void complex_cvt2reim(float *dst_re, float *dst_im, const float *src_mod, const float *src_arg, size_t count)
        {
            while (count--)
            {
                float mod       = *(src_mod++);
                float arg       = *(src_arg++);
                *(dst_re++)     = mod * cosf(arg);
                *(dst_im++)     = mod * sinf(arg);
            }
        }

        static void complex_mod(float *dst_mod, const float *src_re, const float *src_im, size_t count)
        {
            while (count--)
            {
                float re        = *(src_re++);
                float im        = *(src_im++);
                *(dst_mod++)    = sqrtf(re*re + im*im);
            }
        }

        void packed_complex_mod(float *dst_mod, const float *src, size_t count)
        {
            while (count--)
            {
                float re        = src[0];
                float im        = src[1];
                *(dst_mod++)    = sqrtf(re*re + im*im);
                src            += 2;
            }
        }
    }
}

#endif /* CORE_NATIVE_COMPLEX_H_ */
