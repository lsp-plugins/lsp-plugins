/*
 * fade.cpp
 *
 *  Created on: 24 марта 2016 г.
 *      Author: sadko
 */

#include <core/fade.h>

namespace lsp
{
    void fade_in(float *dst, const float *src, size_t fade_len, size_t buf_len)
    {
        if ((fade_len <= 0) || (buf_len <= 0))
            return;

        float k = 1.0f / fade_len;
        if (fade_len > buf_len)
            fade_len = buf_len;

        for (size_t i=0; i < fade_len; ++i)
            dst[i] = src[i] * i * k;
    }

    void fade_out(float *dst, const float *src, size_t fade_len, size_t buf_len)
    {
        if ((fade_len <= 0) || (buf_len <= 0))
            return;

        float k = 1.0f / fade_len;
        if (fade_len > buf_len)
            fade_len = buf_len;
        else
        {
            src     = &src[buf_len - fade_len];
            dst     = &dst[buf_len - fade_len];
        }

        for (size_t i=fade_len; i > 0; )
            *(dst++) = *(src++) * ((--i) * k);
    }
}

