/*
 * dsp.h
 *
 *  Created on: 05 окт. 2015 г.
 *      Author: sadko
 */

#ifndef CORE_NATIVE_DSP_H_
#define CORE_NATIVE_DSP_H_

namespace lsp
{
    namespace native
    {
        void start(dsp_context_t *ctx)
        {
            ctx->top        = 0;
        }

        void finish(dsp_context_t *ctx)
        {
        }

        void copy(float *dst, const float *src, size_t count)
        {
            if (dst == src)
                return;
            memcpy(dst, src, count * sizeof(float));
        }

        void move(float *dst, const float *src, size_t count)
        {
            if (dst == src)
                return;
            memmove(dst, src, count * sizeof(float));
        }

        void fill(float *dst, float value, size_t count)
        {
            while (count--)
                *(dst++) = value;
        }

        void fill_zero(float *dst, size_t count)
        {
            while (count--)
                *(dst++) = 0.0f;
        }

        void fill_one(float *dst, size_t count)
        {
            while (count--)
                *(dst++) = 1.0f;
        }

        void fill_minus_one(float *dst, size_t count)
        {
            while (count--)
                *(dst++) = -1.0f;
        }

        float scalar_mul(const float *a, const float *b, size_t count)
        {
            float result = 0;
            while (count--)
                result += *(a++) * *(b++);
            return result;
        }

        void abs1(float *dst, size_t count)
        {
            while (count--)
            {
                float tmp   = *dst;
                *(dst++)    = (tmp < 0.0f) ? -tmp : tmp;
            }
        }

        void abs2(float *dst, const float *src, size_t count)
        {
            while (count--)
            {
                float tmp   = *(src++);
                *(dst++)    = (tmp < 0.0f) ? -tmp : tmp;
            }
        }

        void abs_add2(float *dst, const float *src, size_t count)
        {
            while (count--)
            {
                float tmp   = *(src++);
                *(dst++)   += (tmp < 0.0f) ? -tmp : tmp;
            }
        }

        void abs_sub2(float *dst, const float *src, size_t count)
        {
            while (count--)
            {
                float tmp   = *(src++);
                *(dst++)   -= (tmp < 0.0f) ? -tmp : tmp;
            }
        }

        void abs_mul2(float *dst, const float *src, size_t count)
        {
            while (count--)
            {
                float tmp   = *(src++);
                *(dst++)   *= (tmp < 0.0f) ? -tmp : tmp;
            }
        }

        void abs_div2(float *dst, const float *src, size_t count)
        {
            while (count--)
            {
                float tmp   = *(src++);
                *(dst++)   /= (tmp < 0.0f) ? -tmp : tmp;
            }
        }

        void abs_normalized(float *dst, const float *src, size_t count)
        {
            // Calculate absolute values
            dsp::abs2(dst, src, count);

            // Find the maximum value
            float max = dsp::max(dst, count);

            // Divide if it is possible
            if (max != 0.0f)
                dsp::scale2(dst, 1.0f / max, count);
        }

        void normalize(float *dst, const float *src, size_t count)
        {
            // Find minimum and maximum
            float max = 0.0f, min = 0.0f;
            dsp::minmax(src, count, &min, &max);

            // Determine maximum possible value
            max = (max > 0.0f) ? max : -max;
            min = (min > 0.0f) ? min : -min;
            if (max < min)
                max = min;

            // Normalize OR copy
            if (max > 0.0f)
                dsp::scale3(dst, src, 1.0f / max, count);
            else
                dsp::copy(dst, src, count);
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

        void accumulate(float *dst, const float *src, float k, float p, size_t count)
        {
            while (count--)
            {
                *dst = (*dst) * k + (*src) * p;
                dst++;
                src++;
            }
        }

        void scale_add3(float *dst, const float *src, float k, size_t count)
        {
            while (count--)
                *(dst++) += *(src++) * k;
        }

        void scale_sub3(float *dst, const float *src, float k, size_t count)
        {
            while (count--)
                *(dst++) -= *(src++) * k;
        }

        void scale_mul3(float *dst, const float *src, float k, size_t count)
        {
            while (count--)
                *(dst++) *= *(src++) * k;
        }

        void scale_div3(float *dst, const float *src, float k, size_t count)
        {
            while (count--)
                *(dst++) /= *(src++) * k;
        }

        void scale_add4(float *dst, const float *src1, const float *src2, float k, size_t count)
        {
            while (count--)
                *(dst++) = *(src1++) + *(src2++) * k;
        }

        void scale_sub4(float *dst, const float *src1, const float *src2, float k, size_t count)
        {
            while (count--)
                *(dst++) = *(src1++) - *(src2++) * k;
        }

        void scale_mul4(float *dst, const float *src1, const float *src2, float k, size_t count)
        {
            while (count--)
                *(dst++) = *(src1++) * *(src2++) * k;
        }

        void scale_div4(float *dst, const float *src1, const float *src2, float k, size_t count)
        {
            while (count--)
                *(dst++) = *(src1++) / (*(src2++) * k);
        }

        void add2(float *dst, const float *src, size_t count)
        {
            while (count--)
                *(dst++) += *(src++);
        }

        void sub2(float *dst, const float *src, size_t count)
        {
            while (count--)
                *(dst++) -= *(src++);
        }

        void mul2(float *dst, const float *src, size_t count)
        {
            while (count--)
                *(dst++) *= *(src++);
        }

        void div2(float *dst, const float *src, size_t count)
        {
            while (count--)
                *(dst++) /= *(src++);
        }

        void scale2(float *dst, float k, size_t count)
        {
            while (count--)
                *(dst++) *= k;
        };

        void add3(float *dst, const float *src1, const float *src2, size_t count)
        {
            while (count--)
                *(dst++) = *(src1++) + *(src2++);
        }

        void sub3(float *dst, const float *src1, const float *src2, size_t count)
        {
            while (count--)
                *(dst++) = *(src1++) - *(src2++);
        }

        void mul3(float *dst, const float *src1, const float *src2, size_t count)
        {
            while (count--)
                *(dst++) = *(src1++) * *(src2++);
        }

        void div3(float *dst, const float *src1, const float *src2, size_t count)
        {
            while (count--)
                *(dst++) = *(src1++) / *(src2++);
        }

        void scale3(float *dst, const float *src, float k, size_t count)
        {
            while (count--)
                *(dst++) = *(src++) * k;
        };

        void integrate(float *dst, const float *src, float k, size_t count)
        {
            while (count--)
            {
                *dst = *dst + (*src - *dst) * k;
                dst++;
                src++;
            }
        }

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

        void reverse1(float *dst, size_t count)
        {
            float *src      = &dst[count];
            count >>= 1;
            while (count--)
            {
                float   tmp = *dst;
                *(dst++)    = *(--src);
                *src        = tmp;
            }
        }

        void reverse2(float *dst, const float *src, size_t count)
        {
            if (dst == src)
            {
                reverse1(dst, count);
                return;
            }

            src         = &src[count];
            while (count--)
                *(dst++)    = *(--src);
        }
    }

}

#endif /* CORE_NATIVE_DSP_H_ */
