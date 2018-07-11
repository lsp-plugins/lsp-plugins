/*
 * graphics.h
 *
 *  Created on: 07 авг. 2016 г.
 *      Author: sadko
 */

#ifndef CORE_NATIVE_GRAPHICS_H_
#define CORE_NATIVE_GRAPHICS_H_

namespace lsp
{
    namespace native
    {
        static void axis_apply_log(float *x, float *y, const float *v, float zero, float norm_x, float norm_y, size_t count)
        {
            for (size_t i=0; i<count; ++i)
            {
                float vec    = v[i];
                if (vec < 0.0f)
                    vec     = -vec;
                if (vec < AMPLIFICATION_THRESH)
                    vec     = AMPLIFICATION_THRESH;
                float k     = logf(vec * zero);
                x[i]       += norm_x * k;
                y[i]       += norm_y * k;
            }
        }
    }
}

#endif /* CORE_NATIVE_GRAPHICS_H_ */
