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

        static inline void scramble_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank)
        {
            size_t items    = 1 << rank;

            // Scramble the order of samples
            if ((dst_re != src_re) && (dst_im != src_im))
            {
                #define SC_COPY(type)   \
                    for (size_t i = 0; i < items; ++i) \
                    { \
                        size_t j = reverse_bits(type(i), rank);    /* Reverse the order of the bits */ \
                        /* Copy the values from the reversed position */ \
                        dst_re[i]   = src_re[j]; \
                        dst_im[i]   = src_im[j]; \
                    }

                // Just copy data from calculated positions
                if (rank <= (sizeof(int16_t) * 8))
                {
                    if (rank <= 8)
                        SC_COPY(uint8_t)
                    else
                        SC_COPY(uint16_t)
                }
                else
                {
                    if (rank <= 32)
                        SC_COPY(uint32_t)
                    else
                        SC_COPY(uint64_t)
                }

                #undef SC_COPY
            }
            else
            {
                // More general algorithm: first copy data
                dsp::move(dst_re, src_re, items);
                dsp::move(dst_im, src_im, items);

                #define SC_COPY(type)   \
                    for (size_t i = 1; i < (items - 1); ++i) \
                    { \
                        size_t j = reverse_bits(type(i), rank);    /* Reverse the order of the bits */ \
                        if (i >= j) \
                            continue; \
                        \
                        /* Copy the values from the reversed position */ \
                        float re    = dst_re[i]; \
                        float im    = dst_im[i]; \
                        dst_re[i]   = dst_re[j]; \
                        dst_im[i]   = dst_im[j]; \
                        dst_re[j]   = re; \
                        dst_im[j]   = im; \
                    }

                // Reverse the source arrays
                if (rank <= (sizeof(int16_t) * 8))
                {
                    if (rank <= 8)
                        SC_COPY(uint8_t)
                    else
                        SC_COPY(uint16_t)
                }
                else
                {
                    if (rank <= 32)
                        SC_COPY(uint32_t)
                    else
                        SC_COPY(uint64_t)
                }

                #undef SC_COPY
            }
        }

        static void start_direct_fft(float *dst_re, float *dst_im, size_t rank)
        {
            size_t iterations    = 1 << (rank - 2);
            while (iterations--)
            {
                // Perform 4-calculations
                // s0' = s0 + s1
                // s1' = s0 - s1
                // s2' = s2 + s3
                // s3' = s2 - s3
                // s0'' = s0' + s2'
                // s1'' = s1' - j * s3'
                // s2'' = s0' - s2'
                // s3'' = s1' + j * s3'
                float s0_re     = dst_re[0] + dst_re[1];
                float s0_im     = dst_im[0] + dst_im[1];
                float s1_re     = dst_re[0] - dst_re[1];
                float s1_im     = dst_im[0] - dst_im[1];
                float s2_re     = dst_re[2] + dst_re[3];
                float s2_im     = dst_im[2] + dst_im[3];
                float s3_re     = dst_re[2] - dst_re[3];
                float s3_im     = dst_im[2] - dst_im[3];

                dst_re[0]       = s0_re + s2_re;
                dst_im[0]       = s0_im + s2_im;
                dst_re[1]       = s1_re + s3_im;
                dst_im[1]       = s1_im - s3_re;
                dst_re[2]       = s0_re - s2_re;
                dst_im[2]       = s0_im - s2_im;
                dst_re[3]       = s1_re - s3_im;
                dst_im[3]       = s1_im + s3_re;

                // Move pointers
                dst_re      += 4;
                dst_im      += 4;
            }
        }

        static void start_reverse_fft(float *dst_re, float *dst_im, size_t rank)
        {
            size_t iterations    = 1 << (rank - 2);
            while (iterations--)
            {
                // Perform 4-calculations
                // s0' = s0 + s1
                // s1' = s0 - s1
                // s2' = s2 + s3
                // s3' = s2 - s3
                // s0'' = s0' + s2'
                // s1'' = s1' + j * s3'
                // s2'' = s0' - s2'
                // s3'' = s1' - j * s3'
                float s0_re     = dst_re[0] + dst_re[1];
                float s0_im     = dst_im[0] + dst_im[1];
                float s1_re     = dst_re[0] - dst_re[1];
                float s1_im     = dst_im[0] - dst_im[1];
                float s2_re     = dst_re[2] + dst_re[3];
                float s2_im     = dst_im[2] + dst_im[3];
                float s3_re     = dst_re[2] - dst_re[3];
                float s3_im     = dst_im[2] - dst_im[3];

                dst_re[0]       = s0_re + s2_re;
                dst_im[0]       = s0_im + s2_im;
                dst_re[1]       = s1_re - s3_im;
                dst_im[1]       = s1_im + s3_re;
                dst_re[2]       = s0_re - s2_re;
                dst_im[2]       = s0_im - s2_im;
                dst_re[3]       = s1_re + s3_im;
                dst_im[3]       = s1_im - s3_re;

                // Move pointers
                dst_re      += 4;
                dst_im      += 4;
            }
        }

        static void direct_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank)
        {
            // Check bounds
            if (rank <= 1)
            {
                if (rank == 1)
                {
                    // s0' = s0 + s1
                    // s1' = s0 - s1
                    float s1_re     = dst_re[1];
                    float s1_im     = dst_im[1];
                    dst_re[1]       = dst_re[0] - s1_re;
                    dst_im[1]       = dst_im[0] - s1_im;
                    dst_re[0]       = dst_re[0] + s1_re;
                    dst_im[0]       = dst_im[0] + s1_im;
                }
                else
                {
                    dst_re[0]       = src_re[0];
                    dst_im[0]       = src_im[0];
                }
                return;
            }

            // Scramble the order of samples
            scramble_fft(dst_re, dst_im, src_re, src_im, rank);

            // Perform the lowest kernel calculations
            start_direct_fft(dst_re, dst_im, rank);

            // Prepare for butterflies
            size_t items    = 1 << rank;

            // Iterate butterflies
            for (size_t n=4, bs=n << 1; n < items; n <<= 1, bs <<= 1)
            {
                // Process the X[k] and X[k + n] pairs
                for (size_t k=0; k < n; ++k)
                {
                    // Calculate the rotation coefficient
                    float w             = (M_PI * k) / n;
                    float w_re          = cosf(w);
                    float w_im          = sinf(w);

                    // Set initial values of pointers
                    float *a_re         = &dst_re[k];
                    float *a_im         = &dst_im[k];
                    float *b_re         = &a_re[n];
                    float *b_im         = &a_im[n];

                    for (size_t p=k; p<items; p += bs)
                    {
                        // Calculate complex c = w * b
                        float c_re          = w_re * (*b_re) + w_im * (*b_im);
                        float c_im          = w_re * (*b_im) - w_im * (*b_re);

                        // Calculate the output values:
                        // a'   = a + c
                        // b'   = a - c
                        *b_re               = *a_re - c_re;
                        *b_im               = *a_im - c_im;
                        *a_re               = *a_re + c_re;
                        *a_im               = *a_im + c_im;

                        // Update pointers
                        a_re               += bs;
                        a_im               += bs;
                        b_re               += bs;
                        b_im               += bs;
                    }
                }
            }
        }

        static void reverse_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank)
        {
            // Check bounds
            if (rank <= 1)
            {
                if (rank == 1)
                {
                    // s0' = s0 + s1
                    // s1' = s0 - s1
                    float s1_re     = dst_re[1];
                    float s1_im     = dst_im[1];
                    dst_re[1]       = (dst_re[0] - s1_re) * 0.5f;
                    dst_im[1]       = (dst_im[0] - s1_im) * 0.5f;
                    dst_re[0]       = (dst_re[0] + s1_re) * 0.5f;
                    dst_im[0]       = (dst_im[0] + s1_im) * 0.5f;
                }
                else
                {
                    dst_re[0]       = src_re[0];
                    dst_im[0]       = src_im[0];
                }
                return;
            }

            // Scramble the order of samples
            scramble_fft(dst_re, dst_im, src_re, src_im, rank);

            // Perform the lowest kernel calculations
            start_reverse_fft(dst_re, dst_im, rank);

            // Prepare for butterflies
            size_t items    = 1 << rank;

            // Iterate butterflies
            for (size_t n=4, bs=n << 1; n < items; n <<= 1, bs <<= 1)
            {
                // Process the X[k] and X[k + n] pairs
                for (size_t k=0; k < n; ++k)
                {
                    // Calculate the rotation coefficient
                    float w_re          = cosf((M_PI * k) / n);
                    float w_im          = sinf((M_PI * k) / n);

                    // Set initial values of pointers
                    float *a_re         = &dst_re[k];
                    float *a_im         = &dst_im[k];
                    float *b_re         = &a_re[n];
                    float *b_im         = &a_im[n];

                    for (size_t p=k; p<items; p += bs)
                    {
                        // Calculate complex c = w * b
                        float c_re          = w_re * (*b_re) - w_im * (*b_im);
                        float c_im          = w_re * (*b_im) + w_im * (*b_re);

                        // Calculate the output values:
                        // a'   = a + c
                        // b'   = a - c
                        *b_re               = *a_re - c_re;
                        *b_im               = *a_im - c_im;
                        *a_re               = *a_re + c_re;
                        *a_im               = *a_im + c_im;

                        // Update pointers
                        a_re               += bs;
                        a_im               += bs;
                        b_re               += bs;
                        b_im               += bs;
                    }
                }
            }

            // Update amplitudes
            dsp::normalize_fft(dst_re, dst_im, dst_re, dst_im, rank);
        }

        static void normalize_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank)
        {
            size_t items    = 1 << rank;
            float k         = 1.0f / items;
            while (items--)
            {
                *(dst_re++)     = *(src_re++) * k;
                *(dst_im++)     = *(src_im++) * k;
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

        static void center_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank)
        {
            if (rank == 0)
                return;

            size_t center    = 1 << (rank - 1);
            for (size_t i=0; i < center; ++i)
            {
                float v1_re         =   src_re[i];
                float v1_im         =   src_im[i];
                float v2_re         =   src_re[center + i];
                float v2_im         =   src_im[center + i];

                dst_re[i]           =   v2_re;
                dst_im[i]           =   v2_im;
                dst_re[center + i]  =   v1_re;
                dst_im[center + i]  =   v1_im;
            }
        }

        static void combine_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank)
        {
            if (rank < 2)
                return;
            ssize_t  count  = 1 << rank;
            const float *tail_re = &src_re[count];
            const float *tail_im = &src_im[count];
            count >>= 1;

            for (ssize_t i=1; i<count; ++i)
            {
                dst_re[i]       = src_re[i] + tail_re[-i];
                dst_im[i]       = src_im[i] - tail_im[-i];
            }

            dsp::fill_zero(&dst_re[count+1], count-1);
            dsp::fill_zero(&dst_im[count+1], count-1);
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

        static void lr_to_ms(float *m, float *s, const float *l, const float *r, size_t count)
        {
            while (count--)
            {
                float lv        = *(l++);
                float rv        = *(r++);
                *(m++)          = (lv + rv) * 0.5f;
                *(s++)          = (lv - rv) * 0.5f;
            }
        }

        static void ms_to_lr(float *l, float *r, const float *m, const float *s, size_t count)
        {
            while (count--)
            {
                float mv        = *(m++);
                float sv        = *(s++);
                *(l++)          = mv + sv;
                *(r++)          = mv - sv;
            }
        }

        static float biquad_process(float *buf, const float *ir, float sample)
        {
            // Calculate sample
            float result    =
                buf[0] * ir[0] +
                buf[1] * ir[1] +
                buf[2] * ir[2] +
                buf[3] * ir[3] +
                sample * ir[4];

            // Shift buffer
            buf[3]  = buf[1];
            buf[2]  = buf[0];
            buf[1]  = sample;
            buf[0]  = result;

            return result;
        }

        static void biquad_process_multi(float *dst, const float *src, size_t count, float *buf, const float *ir)
        {
            for (size_t i=0; i<count; ++i)
            {
                // Calculate sample
                float result    =
                    buf[0] * ir[0] +
                    buf[1] * ir[1] +
                    buf[2] * ir[2] +
                    buf[3] * ir[3] +
                    src[i] * ir[4];

                // Shift buffer
                buf[3]  = buf[1];
                buf[2]  = buf[0];
                buf[1]  = src[i];
                buf[0]  = result;

                // Store sample
                dst[i]  = result;
            }
        }

        static float vec4_scalar_mul(const float *a, const float *b)
        {
            return
                a[0] * b[0] +
                a[1] * b[1] +
                a[2] * b[2] +
                a[3] * b[3];
        }

        static float vec4_push(float *v, float value)
        {
            float result = v[0];
            v[0]    = v[1];
            v[1]    = v[2];
            v[2]    = v[3];
            v[3]    = value;
            return result;
        }

        static float vec4_unshift(float *v, float value)
        {
            float result = v[3];
            v[3]    = v[2];
            v[2]    = v[1];
            v[1]    = v[0];
            v[0]    = value;
            return result;
        }

        static void vec4_zero(float *v)
        {
            v[0]    = 0.0f;
            v[1]    = 0.0f;
            v[2]    = 0.0f;
            v[3]    = 0.0f;
        }

//        static float poly_calc(float x, const float *poly, size_t count)
//        {
//            if (count == 0)
//                return 0;
//            float t = 1.0f, r = *(poly++);
//            while (--count)
//            {
//                r   +=  t * (*(poly++));
//                t   *=  x;
//            }
//
//            return r;
//        }
//
//        static void complex_poly_calc(float *p_re, float *p_im, float x_re, float x_im, const float *poly, size_t count)
//        {
//            float r_re  = 0.0f, r_im = 0.0f;
//            if (count > 0)
//            {
//                float t_re  = 1.0f, t_im  = 0.0f;
//                while (count--)
//                {
//                    // Update result
//                    float k     = *(poly++);
//                    r_re       += t_re * k;
//                    r_im       += t_im * k;
//
//                    // Update coefficients
//                    k           = t_re * x_re - t_im * x_im;
//                    t_im        = t_re * x_im + t_im * x_re;
//                    t_re        = k;
//                    poly       ++;
//                }
//            }
//
//            *p_re       = r_re;
//            *p_im       = r_im;
//        }
    }

}


#endif /* CORE_NATIVE_DSP_H_ */
