/*
 * butterfly.h
 *
 *  Created on: 29 февр. 2016 г.
 *      Author: sadko
 */

// This is the SSE implementation of the scrambling functions for self data

#define _SSE_FFT_NAME(id, mode) id ## _ ## mode
#define _SSE_CALL_NAME(id, bits, mode) id ## bits ## _ ## mode

#define SSE_FFT_NAME(id, mode) _SSE_FFT_NAME(id, mode)
#define SSE_CALL_NAME(id, bits, mode) _SSE_CALL_NAME(id, bits, mode)(dst_re, dst_im, src_re, src_im, rank)

static inline void SSE_FFT_NAME(FFT_SCRAMBLE_DIRECT_NAME, FFT_MODE)(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank)
{
    // Scramble the order of samples
    if ((dst_re == src_re) || (dst_im == src_im))
    {
        if (rank <= 8)
            SSE_CALL_NAME(FFT_SCRAMBLE_SELF_DIRECT_NAME, 8, FFT_MODE);
        else //if (rank <= 16)
            SSE_CALL_NAME(FFT_SCRAMBLE_SELF_DIRECT_NAME, 16, FFT_MODE);
//        else
//        {
//            #ifdef __x86_64__
//            if (rank <= 32)
//                SSE_CALL_NAME(FFT_SCRAMBLE_SELF_DIRECT_NAME, 32, FFT_MODE);
//            else
//                SSE_CALL_NAME(FFT_SCRAMBLE_SELF_DIRECT_NAME, 64, FFT_MODE);
//            #else
//                SSE_CALL_NAME(FFT_SCRAMBLE_SELF_DIRECT_NAME, 32, FFT_MODE);
//            #endif /* __x86_64 */
//        }
    }
    else
    {
        rank -= 3;

        if (rank <= 8)
            SSE_CALL_NAME(FFT_SCRAMBLE_COPY_DIRECT_NAME, 8, FFT_MODE);
        else //if (rank <= 16)
            SSE_CALL_NAME(FFT_SCRAMBLE_COPY_DIRECT_NAME, 16, FFT_MODE);
//        else
//        {
//            #ifdef __x86_64__
//            if (rank <= 32)
//                SSE_CALL_NAME(FFT_SCRAMBLE_COPY_DIRECT_NAME, 32, FFT_MODE);
//            else
//                SSE_CALL_NAME(FFT_SCRAMBLE_COPY_DIRECT_NAME, 64, FFT_MODE);
//            #else
//                SSE_CALL_NAME(FFT_SCRAMBLE_COPY_DIRECT_NAME, 32, FFT_MODE);
//            #endif /* __x86_64 */
//        }
    }
}

static inline void SSE_FFT_NAME(FFT_SCRAMBLE_REVERSE_NAME, FFT_MODE)(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank)
{
    // Scramble the order of samples
    if ((dst_re == src_re) || (dst_im == src_im))
    {
        if (rank <= 8)
            SSE_CALL_NAME(FFT_SCRAMBLE_SELF_REVERSE_NAME, 8, FFT_MODE);
        else //if (rank <= 16)
            SSE_CALL_NAME(FFT_SCRAMBLE_SELF_REVERSE_NAME, 16, FFT_MODE);
//        else
//        {
//            #ifdef __x86_64__
//            if (rank <= 32)
//                SSE_CALL_NAME(FFT_SCRAMBLE_SELF_REVERSE_NAME, 32, FFT_MODE);
//            else
//                SSE_CALL_NAME(FFT_SCRAMBLE_SELF_REVERSE_NAME, 64, FFT_MODE);
//            #else
//                SSE_CALL_NAME(FFT_SCRAMBLE_SELF_REVERSE_NAME, 32, FFT_MODE);
//            #endif /* __x86_64 */
//        }
    }
    else
    {
        rank -= 3;

        if (rank <= 8)
            SSE_CALL_NAME(FFT_SCRAMBLE_COPY_REVERSE_NAME, 8, FFT_MODE);
        else //if (rank <= 16)
            SSE_CALL_NAME(FFT_SCRAMBLE_COPY_REVERSE_NAME, 16, FFT_MODE);
//        else
//        {
//            #ifdef __x86_64__
//            if (rank <= 32)
//                SSE_CALL_NAME(FFT_SCRAMBLE_COPY_REVERSE_NAME, 32, FFT_MODE);
//            else
//                SSE_CALL_NAME(FFT_SCRAMBLE_COPY_REVERSE_NAME, 64, FFT_MODE);
//            #else
//                SSE_CALL_NAME(FFT_SCRAMBLE_COPY_REVERSE_NAME, 32, FFT_MODE);
//            #endif /* __x86_64 */
//        }
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
