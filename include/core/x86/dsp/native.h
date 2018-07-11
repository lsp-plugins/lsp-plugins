/*
 * native.h
 *
 *  Created on: 05 окт. 2015 г.
 *      Author: sadko
 */

#ifndef CORE_X86_DSP_NATIVE_H_
#define CORE_X86_DSP_NATIVE_H_

namespace lsp
{
    namespace sse
    {
        static void copy(float *dst, const float *src, size_t count)
        {
            if ((count == 0) || (dst == src))
                return;
            copy_forward(dst, src, count);
        }

        static void move(float *dst, const float *src, size_t count)
        {
            if (count == 0)
                return;
            if (dst < src)
                copy_forward(dst, src, count);
            else if (dst > src)
                copy_backward(dst, src, count);
        }

        static void normalize(float *dst, const float *src, size_t count)
        {
            // Find minimum and maximum
            float max = 0.0f, min = 0.0f;
            minmax(src, count, &min, &max);

            // Determine maximum possible value
            if (max < 0.0f)
                max     = - max;
            if (min < 0.0f)
                min     = -min;
            if (max < min)
                max = min;

            // Normalize OR copy
            if (max > 0.0f)
                scale(dst, src, 1.0f / max, count);
            else
                copy(dst, src, count);
        }

        static void fill(float *dst, float value, size_t count)
        {
            if (count > 0)
                fill_forward(dst, value, count);
        }

        static void fill_zero(float *dst, size_t count)
        {
            if (count > 0)
                fill_forward(dst, 0.0f, count);
        }

        static void fill_one(float *dst, size_t count)
        {
            if (count > 0)
                fill_forward(dst, 1.0f, count);
        }

        static void fill_minus_one(float *dst, size_t count)
        {
            if (count > 0)
                fill_forward(dst, -1.0f, count);
        }

        static void sub_multiplied(float *dst, const float *src, float k, size_t count)
        {
            add_multiplied(dst, src, -k, count);
        }

        static void integrate(float *dst, const float *src, float k, size_t count)
        {
            // dst[i] = dst[i] + (src[i] - dst[i]) * k =
            // = dst[i] - dst[i] * k + src[i] * k =
            // = dst[i] * (1-k) + src[i] * k

            return mix(dst, dst, src, 1.0f - k, k, count);
        }

        static float abs_max(const float *src, size_t count)
        {
            if (count == 0)
                return 0.0f;

            float min, max;
            minmax(src, count, &min, &max);
            if (min < 0.0f)
                min     = - min;
            if (max < 0.0f)
                max     = - max;
            if (max < min)
                max     = min;

            return max;
        }

        static void convolve(float *dst, const float *src, const float *conv, size_t length, size_t count)
        {
            if (length == 0)
            {
                sse::fill_zero(dst, count);
                return;
            }

            while ((count--) > 0)
            {
                *dst        = convolve_single(src, conv, length);
                dst++;
                src++;
            }
        }

        static void normalize_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank)
        {
            rank            = 1 << rank;
            float k         = 1.0f / rank;
            scale(dst_re, src_re, k, rank);
            scale(dst_im, src_im, k, rank);
        }
    }
}

#endif /* CORE_X86_DSP_NATIVE_H_ */
