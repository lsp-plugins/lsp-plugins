/*
 * butterfly.h
 *
 *  Created on: 29 февр. 2016 г.
 *      Author: sadko
 */

/* This is the SSE implementation of the function:
    static void butterfly_direct_fft(float *dst_re, float *dst_im, size_t pairs, size_t blocks)
    {
        float w[4], w_re[4], w_im[4], c_re[4], c_im[4];
        size_t stride = pairs << 1;

        for (size_t p=0; p < pairs; p += 4)
        {
            // Calculate sines and cosines
            float l             = M_PI / pairs;
            w[0]                = p * l;
            w[1]                = (p + 1) * l;
            w[2]                = (p + 2) * l;
            w[3]                = (p + 3) * l;

            w_re[0]             = cosf(w[0]);
            w_re[1]             = cosf(w[1]);
            w_re[2]             = cosf(w[2]);
            w_re[3]             = cosf(w[3]);

            w_im[0]             = sinf(w[0]);
            w_im[1]             = sinf(w[1]);
            w_im[2]             = sinf(w[2]);
            w_im[3]             = sinf(w[3]);

            // Init pointers
            float *a_re         = &dst_re[p];
            float *a_im         = &dst_im[p];
            float *b_re         = &a_re[pairs];
            float *b_im         = &a_im[pairs];

            for (size_t b=0; b < blocks; ++b)
            {
                // Perform 4x butterflies
                // Calculate complex c[i] = w[i] * b[i]
                c_re[0]             = w_re[0] * b_re[0] + w_im[0] * b_im[0];
                c_re[1]             = w_re[1] * b_re[1] + w_im[1] * b_im[1];
                c_re[2]             = w_re[2] * b_re[2] + w_im[2] * b_im[2];
                c_re[3]             = w_re[3] * b_re[3] + w_im[3] * b_im[3];

                c_im[0]             = w_re[0] * b_im[0] - w_im[0] * b_re[0];
                c_im[1]             = w_re[1] * b_im[1] - w_im[1] * b_re[1];
                c_im[2]             = w_re[2] * b_im[2] - w_im[2] * b_re[2];
                c_im[3]             = w_re[3] * b_im[3] - w_im[3] * b_re[3];

                // Calculate the output values:
                // a[i]'   = a[i] + c[i]
                // b[i]'   = a[i] - c[i]
                b_re[0]             = a_re[0] - c_re[0];
                b_re[1]             = a_re[1] - c_re[1];
                b_re[2]             = a_re[2] - c_re[2];
                b_re[3]             = a_re[3] - c_re[3];

                b_im[0]             = a_im[0] - c_im[0];
                b_im[1]             = a_im[1] - c_im[1];
                b_im[2]             = a_im[2] - c_im[2];
                b_im[3]             = a_im[3] - c_im[3];

                a_re[0]             = a_re[0] + c_re[0];
                a_re[1]             = a_re[1] + c_re[1];
                a_re[2]             = a_re[2] + c_re[2];
                a_re[3]             = a_re[3] + c_re[3];

                a_im[0]             = a_im[0] + c_im[0];
                a_im[1]             = a_im[1] + c_im[1];
                a_im[2]             = a_im[2] + c_im[2];
                a_im[3]             = a_im[3] + c_im[3];

                // Move pointers
                a_re               += stride;
                a_im               += stride;
                b_re               += stride;
                b_im               += stride;
            }
        }
    }
 */

#ifdef ARCH_I386
    #define FFT_BUTTERFLY_BODY(add_b, add_a) \
        /* Init pointers */ \
        float *a_re    = &dst_re[b*pairs*2]; \
        float *a_im    = &dst_im[b*pairs*2]; \
        float *b_re    = &a_re[pairs]; \
        float *b_im    = &a_im[pairs]; \
        size_t p       = pairs; \
        size_t off     = 0; \
        float *tmp_re, *tmp_im; \
        \
        ARCH_X86_ASM \
        ( \
            /* Prepare angle */ \
            __ASM_EMIT("mov         %[XFFT_A_RE], %[tmp_re]") \
            __ASM_EMIT("mov         %[XFFT_A_IM], %[tmp_im]") \
            __ASM_EMIT("movaps      (%[tmp_re], %[rank]), %%xmm6")      /* xmm6 = angle_re[0..3] */ \
            __ASM_EMIT("movaps      (%[tmp_im], %[rank]), %%xmm7")      /* xmm7 = angle_im[0..3] */ \
            /* Start loop */ \
            __ASM_EMIT(".align 16") \
            __ASM_EMIT("1:") \
            \
            /* Load complex values */ \
            /* predicate: xmm6 = w_re[0..3] */ \
            /* predicate: xmm7 = w_im[0..3] */ \
            __ASM_EMIT("mov         %[a_re], %[tmp_re]") \
            __ASM_EMIT("mov         %[a_im], %[tmp_im]") \
            __ASM_EMIT("movups      (%[tmp_re], %[off]), %%xmm0")       /* xmm0 = a_re[0..3] */ \
            __ASM_EMIT("movups      (%[tmp_im], %[off]), %%xmm1")       /* xmm1 = a_im[0..3] */ \
            __ASM_EMIT("mov         %[b_re], %[tmp_re]") \
            __ASM_EMIT("mov         %[b_im], %[tmp_im]") \
            __ASM_EMIT("movups      (%[tmp_re], %[off]), %%xmm2")       /* xmm2 = b_re[0..3] */ \
            __ASM_EMIT("movups      (%[tmp_im], %[off]), %%xmm3")       /* xmm3 = b_im[0..3] */ \
            \
            /* Calculate complex multiplication */ \
            __ASM_EMIT("movaps      %%xmm2, %%xmm4") /* xmm4 = b_re[0..3] */ \
            __ASM_EMIT("movaps      %%xmm3, %%xmm5") /* xmm5 = b_im[0..3] */ \
            __ASM_EMIT("mulps       %%xmm6, %%xmm2") /* xmm2 = w_re[0..3] * b_re[0..3] */ \
            __ASM_EMIT("mulps       %%xmm6, %%xmm3") /* xmm3 = w_re[0..3] * b_im[0..3] */ \
            __ASM_EMIT("mulps       %%xmm7, %%xmm4") /* xmm4 = w_im[0..3] * b_re[0..3] */ \
            __ASM_EMIT("mulps       %%xmm7, %%xmm5") /* xmm5 = w_im[0..3] * b_im[0..3] */ \
            __ASM_EMIT(add_b "      %%xmm5, %%xmm2") /* xmm2 = c_re[0..3] = w_re[0..3] * b_re[0..3] +- w_im[0..3] * b_im[0..3] */ \
            __ASM_EMIT(add_a "      %%xmm4, %%xmm3") /* xmm3 = c_im[0..3] = w_re[0..3] * b_im[0..3] -+ w_im[0..3] * b_re[0..3] */ \
            \
            /* Perform butterfly */ \
            __ASM_EMIT("movaps      %%xmm0, %%xmm4") /* xmm4 = a_re[0..3] */ \
            __ASM_EMIT("movaps      %%xmm1, %%xmm5") /* xmm5 = a_im[0..3] */ \
            __ASM_EMIT("subps       %%xmm2, %%xmm0") /* xmm0 = a_re[0..3] - c_re[0..3] */ \
            __ASM_EMIT("subps       %%xmm3, %%xmm1") /* xmm1 = a_im[0..3] - c_im[0..3] */ \
            __ASM_EMIT("addps       %%xmm4, %%xmm2") /* xmm2 = a_re[0..3] + c_re[0..3] */ \
            __ASM_EMIT("addps       %%xmm5, %%xmm3") /* xmm3 = a_im[0..3] + c_im[0..3] */ \
            \
            /* Store values */ \
            __ASM_EMIT("movups      %%xmm0, (%[tmp_re], %[off])") \
            __ASM_EMIT("movups      %%xmm1, (%[tmp_im], %[off])") \
            __ASM_EMIT("mov         %[a_re], %[tmp_re]") \
            __ASM_EMIT("mov         %[a_im], %[tmp_im]") \
            __ASM_EMIT("movups      %%xmm2, (%[tmp_re], %[off])") \
            __ASM_EMIT("movups      %%xmm3, (%[tmp_im], %[off])") \
            __ASM_EMIT("add         $0x10, %[off]") \
            \
            /* Repeat loop */ \
            __ASM_EMIT("subl        $4, %[p]") \
            __ASM_EMIT("jz          2f") \
            \
            /* Rotate angle */ \
            __ASM_EMIT("mov         %[XFFT_W_RE], %[tmp_re]") \
            __ASM_EMIT("mov         %[XFFT_W_IM], %[tmp_im]") \
            __ASM_EMIT("movaps      (%[tmp_re], %[rank]), %%xmm0")          /* xmm0 = w_re[0..3] */ \
            __ASM_EMIT("movaps      (%[tmp_im], %[rank]), %%xmm1")          /* xmm1 = w_im[0..3] */ \
            __ASM_EMIT("movaps      %%xmm1, %%xmm3")                        /* xmm3 = w_im[0..3] */ \
            __ASM_EMIT("movaps      %%xmm0, %%xmm2")                        /* xmm2 = w_re[0..3] */ \
            \
            __ASM_EMIT("mulps       %%xmm6, %%xmm3")                        /* xmm3 = a_re[0..3] * w_im[0..3] */ \
            __ASM_EMIT("mulps       %%xmm7, %%xmm1")                        /* xmm1 = a_im[0..3] * w_im[0..3] */ \
            __ASM_EMIT("mulps       %%xmm0, %%xmm6")                        /* xmm6 = a_re[0..3] * w_re[0..3] */ \
            __ASM_EMIT("mulps       %%xmm2, %%xmm7")                        /* xmm7 = a_im[0..3] * w_re[0..3] */ \
            __ASM_EMIT("subps       %%xmm1, %%xmm6")                        /* xmm6 = a_re[0..3] * w_re[0..3] + a_im[0..3] * w_im[0..3] */ \
            __ASM_EMIT("addps       %%xmm3, %%xmm7")                        /* xmm7 = a_im[0..3] * w_re[0..3] - a_re[0..3] * w_im[0..3] */ \
            \
            /* Repeat loop */ \
            __ASM_EMIT("jmp         1b") \
            __ASM_EMIT("2:") \
            \
            : [tmp_re] "=&r" (tmp_re), [tmp_im] "=&r" (tmp_im), \
              [off] "+r" (off), [p] "+g" (p) \
            : [rank] "r" (rank), \
              [a_re] "m" (a_re), [a_im] "m" (a_im), \
              [b_re] "m" (b_re), [b_im] "m" (b_im), \
              [XFFT_A_RE] "g" (&XFFT_A_RE[0]), [XFFT_A_IM] "g" (&XFFT_A_IM[0]), \
              [XFFT_W_RE] "g" (&XFFT_W_RE[0]), [XFFT_W_IM] "g" (&XFFT_W_IM[0]) \
            : "cc", "memory",  \
              "%xmm0", "%xmm1", "%xmm2", "%xmm3", \
              "%xmm4", "%xmm5", "%xmm6", "%xmm7" \
        );
#else /* ARCH_X86_64 */

    #define FFT_BUTTERFLY_BODY(add_b, add_a) \
        /* Init pointers */ \
        float *a_re    = &dst_re[b*pairs*2]; \
        float *a_im    = &dst_im[b*pairs*2]; \
        float *b_re    = &a_re[pairs]; \
        float *b_im    = &a_im[pairs]; \
        size_t p       = pairs; \
        size_t off     = 0; \
        \
        ARCH_X86_ASM \
        ( \
            /* Prepare angle */ \
            __ASM_EMIT("movaps      (%[XFFT_A_RE], %[rank]), %%xmm6")   /* xmm6 = angle_re[0..3] */ \
            __ASM_EMIT("movaps      (%[XFFT_A_IM], %[rank]), %%xmm7")   /* xmm7 = angle_im[0..3] */ \
            /* Start loop */ \
            __ASM_EMIT(".align 16") \
            __ASM_EMIT("1:") \
            \
            /* Load complex values */ \
            /* predicate: xmm6 = w_re[0..3] */ \
            /* predicate: xmm7 = w_im[0..3] */ \
            __ASM_EMIT("movups      (%[a_re], %[off]), %%xmm0")     /* xmm0 = a_re[0..3] */ \
            __ASM_EMIT("movups      (%[a_im], %[off]), %%xmm1")     /* xmm1 = a_im[0..3] */ \
            __ASM_EMIT("movups      (%[b_re], %[off]), %%xmm2")     /* xmm2 = b_re[0..3] */ \
            __ASM_EMIT("movups      (%[b_im], %[off]), %%xmm3")     /* xmm3 = b_im[0..3] */ \
            \
            /* Calculate complex multiplication */ \
            __ASM_EMIT("movaps      %%xmm2, %%xmm4") /* xmm4 = b_re[0..3] */ \
            __ASM_EMIT("movaps      %%xmm3, %%xmm5") /* xmm5 = b_im[0..3] */ \
            __ASM_EMIT("mulps       %%xmm6, %%xmm2") /* xmm2 = w_re[0..3] * b_re[0..3] */ \
            __ASM_EMIT("mulps       %%xmm6, %%xmm3") /* xmm3 = w_re[0..3] * b_im[0..3] */ \
            __ASM_EMIT("mulps       %%xmm7, %%xmm4") /* xmm4 = w_im[0..3] * b_re[0..3] */ \
            __ASM_EMIT("mulps       %%xmm7, %%xmm5") /* xmm5 = w_im[0..3] * b_im[0..3] */ \
            __ASM_EMIT(add_b "      %%xmm5, %%xmm2") /* xmm2 = c_re[0..3] = w_re[0..3] * b_re[0..3] +- w_im[0..3] * b_im[0..3] */ \
            __ASM_EMIT(add_a "      %%xmm4, %%xmm3") /* xmm3 = c_im[0..3] = w_re[0..3] * b_im[0..3] -+ w_im[0..3] * b_re[0..3] */ \
            \
            /* Perform butterfly */ \
            __ASM_EMIT("movaps      %%xmm0, %%xmm4") /* xmm4 = a_re[0..3] */ \
            __ASM_EMIT("movaps      %%xmm1, %%xmm5") /* xmm5 = a_im[0..3] */ \
            __ASM_EMIT("subps       %%xmm2, %%xmm0") /* xmm0 = a_re[0..3] - c_re[0..3] */ \
            __ASM_EMIT("subps       %%xmm3, %%xmm1") /* xmm1 = a_im[0..3] - c_im[0..3] */ \
            __ASM_EMIT("addps       %%xmm4, %%xmm2") /* xmm2 = a_re[0..3] + c_re[0..3] */ \
            __ASM_EMIT("addps       %%xmm5, %%xmm3") /* xmm3 = a_im[0..3] + c_im[0..3] */ \
            \
            /* Store values */ \
            __ASM_EMIT("movups      %%xmm2, (%[a_re], %[off])") \
            __ASM_EMIT("movups      %%xmm3, (%[a_im], %[off])") \
            __ASM_EMIT("movups      %%xmm0, (%[b_re], %[off])") \
            __ASM_EMIT("movups      %%xmm1, (%[b_im], %[off])") \
            /* Update pointers */ \
            __ASM_EMIT("add         $0x10, %[off]") \
            /* Repeat loop */ \
            __ASM_EMIT("sub         $4, %[p]") \
            __ASM_EMIT("jz          2f") \
            \
            /* Rotate angle */ \
            __ASM_EMIT("movaps      (%[XFFT_W_IM], %[rank]), %%xmm1")       /* xmm1 = w_im[0..3] */ \
            __ASM_EMIT("movaps      (%[XFFT_W_RE], %[rank]), %%xmm0")       /* xmm0 = w_re[0..3] */ \
            __ASM_EMIT("movaps      %%xmm1, %%xmm3")                        /* xmm3 = w_im[0..3] */ \
            __ASM_EMIT("movaps      %%xmm0, %%xmm2")                        /* xmm2 = w_re[0..3] */ \
            __ASM_EMIT("mulps       %%xmm6, %%xmm3")                        /* xmm3 = a_re[0..3] * w_im[0..3] */ \
            __ASM_EMIT("mulps       %%xmm7, %%xmm1")                        /* xmm1 = a_im[0..3] * w_im[0..3] */ \
            __ASM_EMIT("mulps       %%xmm0, %%xmm6")                        /* xmm6 = a_re[0..3] * w_re[0..3] */ \
            __ASM_EMIT("mulps       %%xmm2, %%xmm7")                        /* xmm7 = a_im[0..3] * w_re[0..3] */ \
            __ASM_EMIT("subps       %%xmm1, %%xmm6")                        /* xmm6 = a_re[0..3] * w_re[0..3] + a_im[0..3] * w_im[0..3] */ \
            __ASM_EMIT("addps       %%xmm3, %%xmm7")                        /* xmm7 = a_im[0..3] * w_re[0..3] - a_re[0..3] * w_im[0..3] */ \
            \
            /* Repeat loop */ \
            __ASM_EMIT("jmp         1b") \
            __ASM_EMIT("2:") \
            \
            : [off] "+r" (off), [p] "+r" (p) \
            : [a_re] "r"(a_re), [a_im] "r"(a_im), \
              [b_re] "r"(b_re), [b_im] "r"(b_im), \
              [rank] "r" (rank), \
              [XFFT_A_RE] "r" (XFFT_A_RE), [XFFT_A_IM] "r" (XFFT_A_IM), \
              [XFFT_W_RE] "r" (XFFT_W_RE), [XFFT_W_IM] "r"(XFFT_W_IM) \
            : "cc", "memory", \
              "%xmm0", "%xmm1", "%xmm2", "%xmm3", \
              "%xmm4", "%xmm5", "%xmm6", "%xmm7" \
        );
#endif /* ARCH_X86_64 */

#define FFT_ANGLE_INIT   \
    ARCH_X86_ASM \
    ( \
        __ASM_EMIT("sub $2, %[rank]")                           /* rank -= 2 */ \
        __ASM_EMIT("shl $4, %[rank]")                           /* rank *= 16 */ \
        __ASM_EMIT("movaps (%[XFFT_A_RE], %[rank]), %%xmm6")    /* xmm6 = angle_re[0..3] */ \
        __ASM_EMIT("movaps (%[XFFT_A_IM], %[rank]), %%xmm7")    /* xmm7 = angle_im[0..3] */ \
        : [rank] "+r" (rank) \
        : \
            [XFFT_A_RE] "r"(XFFT_A_RE), \
            [XFFT_A_IM] "r"(XFFT_A_IM)  \
        : "cc", \
          "%xmm6", "%xmm7" \
    );

#define FFT_ANGLE_ROTATE   \
    ARCH_X86_ASM \
    ( \
        /* xmm6 = a_re[0..3] */ \
        /* xmm7 = a_im[0..3] */ \
        __ASM_EMIT("movaps (%[XFFT_W_IM], %[rank]), %%xmm1")    /* xmm1 = w_im[0..3] */ \
        __ASM_EMIT("movaps (%[XFFT_W_RE], %[rank]), %%xmm0")    /* xmm0 = w_re[0..3] */ \
        __ASM_EMIT("movaps %%xmm1, %%xmm3")                     /* xmm3 = w_im[0..3] */ \
        __ASM_EMIT("movaps %%xmm0, %%xmm2")                     /* xmm2 = w_re[0..3] */ \
        \
        __ASM_EMIT("mulps %%xmm6, %%xmm3")                      /* xmm3 = a_re[0..3] * w_im[0..3] */ \
        __ASM_EMIT("mulps %%xmm7, %%xmm1")                      /* xmm1 = a_im[0..3] * w_im[0..3] */ \
        __ASM_EMIT("mulps %%xmm0, %%xmm6")                      /* xmm6 = a_re[0..3] * w_re[0..3] */ \
        __ASM_EMIT("mulps %%xmm2, %%xmm7")                      /* xmm7 = a_im[0..3] * w_re[0..3] */ \
        __ASM_EMIT("subps %%xmm1, %%xmm6")                      /* xmm6 = a_re[0..3] * w_re[0..3] + a_im[0..3] * w_im[0..3] */ \
        __ASM_EMIT("addps %%xmm3, %%xmm7")                      /* xmm7 = a_im[0..3] * w_re[0..3] - a_re[0..3] * w_im[0..3] */ \
        : \
        : [rank] "r" (rank), \
            [XFFT_W_RE] "r"(XFFT_W_RE), \
            [XFFT_W_IM] "r"(XFFT_W_IM)  \
        : "cc", \
            "%xmm0", "%xmm1", "%xmm2", "%xmm3", \
            "%xmm6", "%xmm7" \
    );

namespace sse
{
    static inline void butterfly_direct(float *dst_re, float *dst_im, size_t rank, size_t blocks)
    {
        size_t pairs = 1 << rank;
        rank = (rank - 2) << 4;

        for (size_t b=0; b<blocks; ++b)
        {
            FFT_BUTTERFLY_BODY("addps", "subps");
        }
    }

    static inline void butterfly_reverse(float *dst_re, float *dst_im, size_t rank, size_t blocks)
    {
        size_t pairs = 1 << rank;
        rank = (rank - 2) << 4;

        for (size_t b=0; b<blocks; ++b)
        {
            FFT_BUTTERFLY_BODY("subps", "addps");
        }
    }
}

#undef FFT_ANGLE_INIT
#undef FFT_ANGLE_ROTATE
#undef FFT_BUTTERFLY_BODY
