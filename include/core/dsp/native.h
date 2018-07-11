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
    class native_dsp: public dsp
    {
        public:
            native_dsp() {};

            static dsp *instance()
            {
                return new native_dsp();
            }

        public:
            virtual void copy(float *dst, const float *src, size_t count)
            {
                if (dst == src)
                    return;
                memcpy(dst, src, count * sizeof(float));
            }

            virtual void copy_multiplied(float *dst, const float *src, float k, size_t count)
            {
                while (count--)
                    *(dst++) = *(src++) * k;
            }

            virtual void fill(float *dst, float value, size_t count)
            {
                while (count--)
                    *(dst++) = value;
            }

            virtual void fill_zero(float *dst, size_t count)
            {
                while (count--)
                    *(dst++) = 0.0f;
            }

            virtual void fill_one(float *dst, size_t count)
            {
                while (count--)
                    *(dst++) = 1.0f;
            }

            virtual void fill_minus_one(float *dst, size_t count)
            {
                while (count--)
                    *(dst++) = -1.0f;
            }

            virtual float scalar_mul(const float *a, const float *b, size_t count)
            {
                float result = 0;
                while (count--)
                    result += *(a++) * *(b++);
                return result;
            }

            virtual void abs(float *dst, const float *src, size_t count)
            {
                while (count--)
                    *(dst++) = fabs(*(src++));
            }

            virtual void abs_normalized(float *dst, const float *src, size_t count)
            {
                // Calculate absolute values
                abs(dst, src, count);

                // Find the maximum value
                float max = 0.0f;
                for (size_t i=0; i<count; ++i)
                    if (dst[i] > max)
                        max = dst[i];

                // Divide if it is possible
                if (max != 0.0f)
                    multiply(dst, dst, 1.0f / max, count);
            }

            virtual void normalize(float *dst, const float *src, size_t count)
            {
                // Find minimum and maximum
                float max = 0.0f, min = 0.0f;
                minmax(src, count, &min, &max);

                // Determine maximum possible value
                max = (max > 0) ? max : -max;
                min = (min > 0) ? min : -min;
                if (max < min)
                    max = min;

                // Normalize OR copy
                if (max > 0.0f)
                    multiply(dst, src, 1.0f / max, count);
                else
                    copy(dst, src, count);
            }

            virtual float min(const float *src, size_t count)
            {
                float min = src[0];
                for (size_t i=0; i<count; ++i)
                    if (src[i] < min)
                        min = src[i];
                return min;
            }

            virtual float max(const float *src, size_t count)
            {
                float max = src[0];
                for (size_t i=0; i<count; ++i)
                    if (src[i] > max)
                        max = src[i];
                return max;
            }

            virtual void minmax(const float *src, size_t count, float *min, float *max)
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

            virtual size_t min_index(const float *src, size_t count)
            {
                size_t index = 0;
                for (size_t i=0; i<count; ++i)
                    if (src[i] < src[index])
                        index = i;
                return index;
            }

            virtual size_t max_index(const float *src, size_t count)
            {
                size_t index = 0;
                for (size_t i=0; i<count; ++i)
                    if (src[i] > src[index])
                        index = i;
                return index;
            }

            virtual void multiply(float *dst, const float *src, float k, size_t count)
            {
                while (count--)
                    *(dst++) = *(src++) * k;
            };

            virtual void accumulate(float *dst, const float *src, float k, float p, size_t count)
            {
                while (count--)
                {
                    *dst = (*dst) * k + (*src) * p;
                    dst++;
                    src++;
                }
            }

            virtual void add_multiplied(float *dst, const float *src, float k, size_t count)
            {
                while (count--)
                    *(dst++) += (*(src++)) * k;
            }

            virtual void sub_multiplied(float *dst, const float *src, float k, size_t count)
            {
                while (count--)
                    *(dst++) -= (*(src++)) * k;
            }

            virtual void integrate(float *dst, const float *src, float k, size_t count)
            {
                while (count--)
                {
                    *dst = *dst + (*src - *dst) * k;
                    dst++;
                    src++;
                }
            }

            virtual void mix(float *dst, const float *src1, const float *src2, float k1, float k2, size_t count)
            {
                while (count--)
                {
                    *dst = (*src1) * k1 + (*src2) * k2;
                    dst     ++;
                    src1    ++;
                    src2    ++;
                }
            }
    };

}


#endif /* CORE_DSP_NATIVE_H_ */
