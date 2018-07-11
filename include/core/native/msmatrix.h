/*
 * dsp.h
 *
 *  Created on: 05 окт. 2015 г.
 *      Author: sadko
 */

#ifndef CORE_NATIVE_MSMATRIX_H_
#define CORE_NATIVE_MSMATRIX_H_

namespace lsp
{
    namespace native
    {
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

        static void lr_to_mid(float *m, const float *l, const float *r, size_t count)
        {
            while (count--)
            {
                float lv        = *(l++);
                float rv        = *(r++);
                *(m++)          = (lv + rv) * 0.5f;
            }
        }

        static void lr_to_side(float *s, const float *l, const float *r, size_t count)
        {
            while (count--)
            {
                float lv        = *(l++);
                float rv        = *(r++);
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

        static void ms_to_left(float *l, const float *m, const float *s, size_t count)
        {
            while (count--)
                *(l++)          = *(m++) + *(s++);
        }

        static void ms_to_right(float *r, const float *m, const float *s, size_t count)
        {
            while (count--)
                *(r++)          = *(m++) - *(s++);
        }

    }

}

#endif /* CORE_NATIVE_MSMATRIX_H_ */
