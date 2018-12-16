/*
 * interpolation.h
 *
 *  Created on: 26 нояб. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_NATIVE_GRAPHICS_INTERPOLATION_H_
#define DSP_ARCH_NATIVE_GRAPHICS_INTERPOLATION_H_

namespace native
{
    void smooth_cubic_linear(float *dst, float start, float stop, size_t count)
    {
        float dy = stop - start;
        float nx = 1.0f / (count + 1); // Normalizing x

        for (size_t i=0; i<count; ++i)
        {
            float x = i * nx;
            *(dst++) = start + dy * x*x * (3.0f - 2.0f * x);
        }
    }

    void smooth_cubic_log(float *dst, float start, float stop, size_t count)
    {
        float dy = logf(stop/start);
        float nx = 1.0f / (count + 1); // Normalizing x

        for (size_t i=0; i<count; ++i)
        {
            float x = i * nx;
            *(dst++) = start * expf(dy * x*x * (3.0f - 2.0f * x));
        }
    }
}

#endif /* DSP_ARCH_NATIVE_GRAPHICS_INTERPOLATION_H_ */
