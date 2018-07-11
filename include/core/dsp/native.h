/*
 * native_dsp.hpp
 *
 *  Created on: 05 окт. 2015 г.
 *      Author: sadko
 */

#ifndef CORE_DSP_NATIVE_H_
#define CORE_DSP_NATIVE_H_

#include <core/dsp.h>

namespace lsp
{
    namespace native
    {
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

        void abs(float *dst, const float *src, size_t count)
        {
            while (count--)
                *(dst++) = fabs(*(src++));
        }

        void abs_normalized(float *dst, const float *src, size_t count)
        {
            // Calculate absolute values
            dsp::abs(dst, src, count);

            // Find the maximum value
            float max = 0.0f;
            for (size_t i=0; i<count; ++i)
                if (dst[i] > max)
                    max = dst[i];

            // Divide if it is possible
            if (max != 0.0f)
                dsp::multiply(dst, dst, 1.0f / max, count);
        }

        void normalize(float *dst, const float *src, size_t count)
        {
            // Find minimum and maximum
            float max = 0.0f, min = 0.0f;
            dsp::minmax(src, count, &min, &max);

            // Determine maximum possible value
            max = (max > 0) ? max : -max;
            min = (min > 0) ? min : -min;
            if (max < min)
                max = min;

            // Normalize OR copy
            if (max > 0.0f)
                dsp::multiply(dst, src, 1.0f / max, count);
            else
                dsp::copy(dst, src, count);
        }

        float min(const float *src, size_t count)
        {
            float min = src[0];
            for (size_t i=0; i<count; ++i)
                if (src[i] < min)
                    min = src[i];
            return min;
        }

        float max(const float *src, size_t count)
        {
            float max = src[0];
            for (size_t i=0; i<count; ++i)
                if (src[i] > max)
                    max = src[i];
            return max;
        }

        void minmax(const float *src, size_t count, float *min, float *max)
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

        size_t min_index(const float *src, size_t count)
        {
            size_t index = 0;
            for (size_t i=0; i<count; ++i)
                if (src[i] < src[index])
                    index = i;
            return index;
        }

        size_t max_index(const float *src, size_t count)
        {
            size_t index = 0;
            for (size_t i=0; i<count; ++i)
                if (src[i] > src[index])
                    index = i;
            return index;
        }

        void multiply(float *dst, const float *src, float k, size_t count)
        {
            while (count--)
                *(dst++) = *(src++) * k;
        };

        void accumulate(float *dst, const float *src, float k, float p, size_t count)
        {
            while (count--)
            {
                *dst = (*dst) * k + (*src) * p;
                dst++;
                src++;
            }
        }

        void add_multiplied(float *dst, const float *src, float k, size_t count)
        {
            while (count--)
                *(dst++) += (*(src++)) * k;
        }

        void sub_multiplied(float *dst, const float *src, float k, size_t count)
        {
            while (count--)
                *(dst++) -= (*(src++)) * k;
        }

        void integrate(float *dst, const float *src, float k, size_t count)
        {
            while (count--)
            {
                *dst = *dst + (*src - *dst) * k;
                dst++;
                src++;
            }
        }

        void mix(float *dst, const float *src1, const float *src2, float k1, float k2, size_t count)
        {
            while (count--)
            {
                *dst = (*src1) * k1 + (*src2) * k2;
                dst     ++;
                src1    ++;
                src2    ++;
            }
        }

        void mix_add(float *dst, const float *src1, const float *src2, float k1, float k2, size_t count)
        {
            while (count--)
            {
                *dst += (*src1) * k1 + (*src2) * k2;
                dst     ++;
                src1    ++;
                src2    ++;
            }
        }

        void dsp_init()
        {
            lsp_trace("Initializing DSP");

            dsp::copy                       = native::copy;
            dsp::move                       = native::move;
            dsp::fill                       = native::fill;
            dsp::fill_one                   = native::fill_one;
            dsp::fill_zero                  = native::fill_zero;
            lsp_trace("dsp::fill_zero = %p", native::fill_zero);
            dsp::fill_minus_one             = native::fill_minus_one;
            dsp::abs                        = native::abs;
            dsp::abs_normalized             = native::abs_normalized;
            dsp::normalize                  = native::normalize;
            dsp::min                        = native::min;
            dsp::max                        = native::max;
            dsp::minmax                     = native::minmax;
            dsp::min_index                  = native::min_index;
            dsp::max_index                  = native::max_index;
            dsp::multiply                   = native::multiply;
            dsp::scalar_mul                 = native::scalar_mul;
            dsp::accumulate                 = native::accumulate;
            dsp::add_multiplied             = native::add_multiplied;
            dsp::sub_multiplied             = native::sub_multiplied;
            dsp::integrate                  = native::integrate;
            dsp::mix                        = native::mix;
            dsp::mix_add                    = native::mix_add;
        }
    }

}


#endif /* CORE_DSP_NATIVE_H_ */
