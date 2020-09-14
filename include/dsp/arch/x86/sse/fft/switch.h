/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 29 февр. 2016 г.
 *
 * lsp-plugins is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-plugins is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-plugins. If not, see <https://www.gnu.org/licenses/>.
 */

// This is the SSE implementation of the scrambling functions for self data

#define _SSE_FFT_NAME(id) id
#define _SSE_CALL_NAME(id, bits) id ## bits

#define SSE_FFT_NAME(id) _SSE_FFT_NAME(id)
#define SSE_CALL_NAME(id, bits) _SSE_CALL_NAME(id, bits)(dst_re, dst_im, src_re, src_im, rank)

namespace sse
{
    static inline void SSE_FFT_NAME(FFT_SCRAMBLE_DIRECT_NAME)(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank)
    {
        // Scramble the order of samples
        if ((dst_re == src_re) || (dst_im == src_im))
        {
            if (rank <= 8)
                SSE_CALL_NAME(FFT_SCRAMBLE_SELF_DIRECT_NAME, 8);
            else //if (rank <= 16)
                SSE_CALL_NAME(FFT_SCRAMBLE_SELF_DIRECT_NAME, 16);
        }
        else
        {
            rank -= 3;

            if (rank <= 8)
                SSE_CALL_NAME(FFT_SCRAMBLE_COPY_DIRECT_NAME, 8);
            else //if (rank <= 16)
                SSE_CALL_NAME(FFT_SCRAMBLE_COPY_DIRECT_NAME, 16);
        }
    }

    static inline void SSE_FFT_NAME(FFT_SCRAMBLE_REVERSE_NAME)(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank)
    {
        // Scramble the order of samples
        if ((dst_re == src_re) || (dst_im == src_im))
        {
            if (rank <= 8)
                SSE_CALL_NAME(FFT_SCRAMBLE_SELF_REVERSE_NAME, 8);
            else //if (rank <= 16)
                SSE_CALL_NAME(FFT_SCRAMBLE_SELF_REVERSE_NAME, 16);
        }
        else
        {
            rank -= 3;

            if (rank <= 8)
                SSE_CALL_NAME(FFT_SCRAMBLE_COPY_REVERSE_NAME, 8);
            else //if (rank <= 16)
                SSE_CALL_NAME(FFT_SCRAMBLE_COPY_REVERSE_NAME, 16);
        }
    }
}

#undef SSE_FFT_NAME
#undef SSE_CALL_NAME
#undef _SSE_FFT_NAME
#undef _SSE_CALL_NAME

#undef FFT_SCRAMBLE_SELF_DIRECT_NAME
#undef FFT_SCRAMBLE_SELF_REVERSE_NAME
#undef FFT_SCRAMBLE_COPY_DIRECT_NAME
#undef FFT_SCRAMBLE_COPY_REVERSE_NAME

#undef FFT_SCRAMBLE_DIRECT_NAME
#undef FFT_SCRAMBLE_REVERSE_NAME
#undef FFT_MODE
