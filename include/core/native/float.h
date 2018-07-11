/*
 * float.h
 *
 *  Created on: 05 авг. 2016 г.
 *      Author: sadko
 */

#ifndef CORE_NATIVE_FLOAT_H_
#define CORE_NATIVE_FLOAT_H_

#include <core/types.h>

namespace lsp
{
    namespace native
    {
        static void copy_saturated(float *dst, const float *src, size_t count)
        {
            while (count--)
            {
                float v = *(src++);
                if (isnanf(v))
                    v       =   FLOAT_SAT_P_NAN;
                else if (isinff(v))
                    v       =   (v < 0.0f) ? FLOAT_SAT_N_INF : FLOAT_SAT_P_INF;

                *(dst++)    = v;
            }
        }

        static void saturate(float *dst, size_t count)
        {
            while (count--)
            {
                float v = *dst;
                if (isnanf(v))
                    *(dst++)    = FLOAT_SAT_P_NAN;
                else if (isinff(v))
                    *(dst++)    = (v < 0.0f) ? FLOAT_SAT_N_INF : FLOAT_SAT_P_INF;
                else
                    dst++;
            }
        }
    }
}

#endif /* CORE_NATIVE_FLOAT_H_ */
