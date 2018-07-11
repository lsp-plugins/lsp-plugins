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

        static void copy(float *dst, const float *src, size_t count)
        {
            if (dst == src)
                return;
            memcpy(dst, src, count * sizeof(float));
        }

        static void move(float *dst, const float *src, size_t count)
        {
            if (dst == src)
                return;
            memmove(dst, src, count * sizeof(float));
        }

        static void fill(float *dst, float value, size_t count)
        {
            while (count--)
                *(dst++) = value;
        }

        static void fill_zero(float *dst, size_t count)
        {
            while (count--)
                *(dst++) = 0.0f;
        }

        static void fill_one(float *dst, size_t count)
        {
            while (count--)
                *(dst++) = 1.0f;
        }

        static void fill_minus_one(float *dst, size_t count)
        {
            while (count--)
                *(dst++) = -1.0f;
        }

        static float scalar_mul(const float *a, const float *b, size_t count)
        {
            float result = 0;
            while (count--)
                result += *(a++) * *(b++);
            return result;
        }

        static void abs(float *dst, const float *src, size_t count)
        {
            while (count--)
                *(dst++) = fabs(*(src++));
        }

        static void abs_normalized(float *dst, const float *src, size_t count)
        {
            // Calculate absolute values
            dsp::abs(dst, src, count);

            // Find the maximum value
            float max = dsp::max(dst, count);

            // Divide if it is possible
            if (max != 0.0f)
                dsp::scale(dst, dst, 1.0f / max, count);
        }

        static void normalize(float *dst, const float *src, size_t count)
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
                dsp::scale(dst, src, 1.0f / max, count);
            else
                dsp::copy(dst, src, count);
        }

        static float min(const float *src, size_t count)
        {
            if (count == 0)
                return 0.0f;

            float min = src[0];
            for (size_t i=0; i<count; ++i)
                if (src[i] < min)
                    min = src[i];
            return min;
        }

        static float max(const float *src, size_t count)
        {
            if (count == 0)
                return 0.0f;

            float max = src[0];
            for (size_t i=0; i<count; ++i)
                if (src[i] > max)
                    max = src[i];
            return max;
        }

        static float abs_max(const float *src, size_t count)
        {
            if (count == 0)
                return 0.0f;

            float max = fabs(src[0]);
            for (size_t i=0; i<count; ++i)
            {
                float tmp = fabs(src[i]);
                if (tmp > max)
                    max = tmp;
            }
            return max;
        }

        static float abs_min(const float *src, size_t count)
        {
            if (count == 0)
                return 0.0f;

            float min = fabs(src[0]);
            for (size_t i=0; i<count; ++i)
            {
                float tmp = fabs(src[i]);
                if (tmp < min)
                    min = tmp;
            }
            return min;
        }

        static void minmax(const float *src, size_t count, float *min, float *max)
        {
            float a_min = src[0], a_max = src[0];
            for (size_t i=0; i<count; ++i)
            {
                if (src[i] < a_min)
                    a_min = src[i];
                if (src[i] > a_max)
                    a_max = src[i];
            }
            *min    = a_min;
            *max    = a_max;
        }

        static size_t min_index(const float *src, size_t count)
        {
            size_t index = 0;
            for (size_t i=0; i<count; ++i)
                if (src[i] < src[index])
                    index = i;
            return index;
        }

        static size_t max_index(const float *src, size_t count)
        {
            size_t index = 0;
            for (size_t i=0; i<count; ++i)
                if (src[i] > src[index])
                    index = i;
            return index;
        }

        static size_t abs_max_index(const float *src, size_t count)
        {
            if (count == 0)
                return 0;

            size_t index = 0;
            float s = fabs(src[0]);
            for (size_t i=1; i<count; ++i)
            {
                float d = fabs(src[i]);
                if (d > s)
                {
                    index   = i;
                    s       = d;
                }
            }
            return index;
        }

        static size_t abs_min_index(const float *src, size_t count)
        {
            if (count == 0)
                return 0;

            size_t index = 0;
            float s = fabs(src[0]);
            for (size_t i=1; i<count; ++i)
            {
                float d = fabs(src[i]);
                if (d < s)
                {
                    index   = i;
                    s       = d;
                }
            }
            return index;
        }

        static void scale(float *dst, const float *src, float k, size_t count)
        {
            while (count--)
                *(dst++) = *(src++) * k;
        };

        static void multiply(float *dst, const float *src1, const float *src2, size_t count)
        {
            while (count--)                *(dst++) = *(src1++) * *(src2++);
        }

        static float h_sum(const float *src, size_t count)
        {
            float result    = 0;
            while (count--)
                result         += *(src++);
            return result;
        }

        static float h_sqr_sum(const float *src, size_t count)
        {
            float result    = 0;
            while (count--)
            {
                float tmp       = *(src++);
                result         += tmp * tmp;
            }
            return result;
        }

        static float h_abs_sum(const float *src, size_t count)
        {
            float result    = 0;
            while (count--)
            {
                float tmp       = *(src++);
                if (tmp >= 0)
                    result         += tmp;
                else
                    result         -= tmp;
            }
            return result;
        }

        static void accumulate(float *dst, const float *src, float k, float p, size_t count)
        {
            while (count--)
            {
                *dst = (*dst) * k + (*src) * p;
                dst++;
                src++;
            }
        }

        static void add_multiplied(float *dst, const float *src, float k, size_t count)
        {
            while (count--)
                *(dst++) += (*(src++)) * k;
        }

        static void sub_multiplied(float *dst, const float *src, float k, size_t count)
        {
            while (count--)
                *(dst++) -= (*(src++)) * k;
        }

        static void add(float *dst, const float *src, size_t count)
        {
            while (count--)
                *(dst++) += *(src++);
        }

        static void sub(float *dst, const float *src, size_t count)
        {
            while (count--)
                *(dst++) -= *(src++);
        }

        static void integrate(float *dst, const float *src, float k, size_t count)
        {
            while (count--)
            {
                *dst = *dst + (*src - *dst) * k;
                dst++;
                src++;
            }
        }

        static void mix(float *dst, const float *src1, const float *src2, float k1, float k2, size_t count)
        {
            while (count--)
            {
                *dst = (*src1) * k1 + (*src2) * k2;
                dst     ++;
                src1    ++;
                src2    ++;
            }
        }

        static void mix_add(float *dst, const float *src1, const float *src2, float k1, float k2, size_t count)
        {
            while (count--)
            {
                *dst += (*src1) * k1 + (*src2) * k2;
                dst     ++;
                src1    ++;
                src2    ++;
            }
        }

        static float convolve_single(const float *src, const float *conv, size_t length)
        {
            float result = 0.0f;
            while (length--)
                result     += *(src++) * *(conv++);
            return result;
        }

        static void convolve(float *dst, const float *src, const float *conv, size_t length, size_t count)
        {
            for (size_t i=0; i < count; ++i)
            {
                const float *s      = &src[i];
                const float *c      = conv;
                float result        = 0;
                size_t n            = length;

                // Process with long chunks (4 samples)
                while (n >= 4)
                {
                    // Convolution (c) is reversed
                    result         +=
                        (s[0] * c[0]) +
                        (s[1] * c[1]) +
                        (s[2] * c[2]) +
                        (s[3] * c[3]);

                    // Increment pointers and counters
                    s              += 4;
                    c              += 4;
                    n              -= 4;
                }

                // Process with short chunks (1 sample)
                while (n > 0)
                {
                    // Convolution (c) is reversed
                    result         += (*s) * (*c);
                    s              ++;
                    c              ++;
                    n              --;
                }

                // Store result
                *(dst++)        = result;
            }
        }

        static void reverse(float *dst, size_t count)
        {
            if (count < 2)
                return;
            float *src      = &dst[count];
            count >>= 1;
            while (count--)
            {
                float   tmp = *dst;
                *(dst++)    = *(--src);
                *src        = tmp;
            }
        }

        static void complex_mul(float *dst_re, float *dst_im, const float *src1_re, const float *src1_im, const float *src2_re, const float *src2_im, size_t count)
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
    }

}

#endif /* CORE_NATIVE_DSP_H_ */
