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

//#define NO_SINCOS4
#ifdef NO_SINCOS4
    #define FFT_BUTTERFLY_SINCOS  \
        float w_re[4] __lsp_aligned16, w_im[4] __lsp_aligned16; \
        \
        float l             = M_PI / pairs; \
        float w             = p * l; \
        w_re[0]             = cosf(w); \
        w_im[0]             = sinf(w); \
        w                  += l; \
        w_re[1]             = cosf(w); \
        w_im[1]             = sinf(w); \
        w                  += l; \
        w_re[2]             = cosf(w); \
        w_im[2]             = sinf(w); \
        w                  += l; \
        w_re[3]             = cosf(w); \
        w_im[3]             = sinf(w); \
        \
        ARCH_X86_ASM \
        ( \
            __ASM_EMIT("movaps     %0, %%xmm6") \
            __ASM_EMIT("movaps     %1, %%xmm7") \
            : \
            : "m" (w_re), "m"(w_im) \
            : "%xmm6", "%xmm7" \
        );

#else
    // Enhanced-precision sines and cosines
    #define FFT_BUTTERFLY_SINCOS_PREC \
        ARCH_X86_ASM \
        ( \
            /* Prepare angles for calculation */ \
            __ASM_EMIT("cvtsi2ss    %1, %%xmm0")                /* xmm0 = p */ \
            __ASM_EMIT("cvtsi2ss    %0, %%xmm3")                /* xmm3 = pairs */ \
            __ASM_EMIT("movaps      %[PI], %%xmm1")             /* xmm1 = pi */ \
            __ASM_EMIT("movaps      %[PI_2], %%xmm4")           /* xmm4 = pi/2 */ \
            __ASM_EMIT("movaps      %%xmm1, %%xmm2")            /* xmm2 = pi */ \
            __ASM_EMIT("divss       %%xmm3, %%xmm2")            /* xmm2 = pi / pairs */ \
            __ASM_EMIT("mulss       %%xmm2, %%xmm0")            /* xmm0 = p * pi / pairs */ \
            __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0")     /* xmm0 = p * pi / pairs */ \
            __ASM_EMIT("addss       %%xmm2, %%xmm0") \
            __ASM_EMIT("shufps      $0x90, %%xmm0, %%xmm0") \
            __ASM_EMIT("addss       %%xmm2, %%xmm0") \
            __ASM_EMIT("shufps      $0x90, %%xmm0, %%xmm0") \
            __ASM_EMIT("addss       %%xmm2, %%xmm0") \
            __ASM_EMIT("shufps      $0x1b, %%xmm0, %%xmm0")     /* xmm0 = X */ \
            \
            /* We consider that the values are positive \
             * xmm0 = X, xmm1 = pi, xmm4 = pi/2 \
             */ \
            __ASM_EMIT("cmpltps     %%xmm0, %%xmm4")            /* xmm4 = X > pi */ \
            __ASM_EMIT("subps       %%xmm0, %%xmm1")            /* xmm1 = pi - X */ \
            __ASM_EMIT("movaps      %%xmm4, %%xmm2")            /* xmm2 = S */ \
            __ASM_EMIT("andps       %%xmm4, %%xmm1")            /* xmm1 = pi - X */ \
            __ASM_EMIT("andnps      %%xmm0, %%xmm4")            /* xmm4 = pi */ \
            __ASM_EMIT("orps        %%xmm4, %%xmm1")            /* xmm1 = x */ \
            __ASM_EMIT("pslld       $31, %%xmm2")               /* xmm2 = s */ \
            \
            /* Angles are in xmm0, calculate sines and cosines */ \
            /* Prepare data \
             * Predicates: \
             *   xmm1 = x, \
             *   xmm2 = s \
             * */ \
            __ASM_EMIT("movaps      %[S5], %%xmm7")             /* xmm2 = S5 */ \
            __ASM_EMIT("movaps      %[C5], %%xmm6")             /* xmm3 = C5 */ \
            __ASM_EMIT("movaps      %%xmm1, %%xmm0")            /* xmm0 = x */ \
            __ASM_EMIT("mulps       %%xmm1, %%xmm1")            /* xmm1 = x^2 */ \
            \
            /* Calculate */ \
            __ASM_EMIT("movaps      %[S4], %%xmm4")             /* xmm4 = S4 */ \
            __ASM_EMIT("movaps      %[C4], %%xmm5")             /* xmm5 = C4 */ \
            __ASM_EMIT("mulps       %%xmm1, %%xmm6")            /* xmm6 = C5 * x^2 */ \
            __ASM_EMIT("mulps       %%xmm1, %%xmm7")            /* xmm7 = S5 * x^2 */ \
            __ASM_EMIT("addps       %%xmm5, %%xmm6")            /* xmm6 = C4 + C5 * x^2 */ \
            __ASM_EMIT("addps       %%xmm4, %%xmm7")            /* xmm7 = S4 + S5 * x^2 */ \
            __ASM_EMIT("movaps      %[S3], %%xmm4")             /* xmm4 = S3 */ \
            __ASM_EMIT("movaps      %[C3], %%xmm5")             /* xmm5 = C3 */ \
            __ASM_EMIT("mulps       %%xmm1, %%xmm6")            /* xmm6 = x^2 * (C4 + C5 * x^2) */ \
            __ASM_EMIT("mulps       %%xmm1, %%xmm7")            /* xmm7 = x^2 * (S4 + S5 * x^2) */ \
            __ASM_EMIT("addps       %%xmm5, %%xmm6")            /* xmm6 = C3 + x^2 * (C4 + C5 * x^2) */ \
            __ASM_EMIT("addps       %%xmm4, %%xmm7")            /* xmm7 = S3 + x^2 * (S4 + S5 * x^2) */ \
            __ASM_EMIT("movaps      %[S2], %%xmm4")             /* xmm4 = S2 */ \
            __ASM_EMIT("movaps      %[C2], %%xmm5")             /* xmm5 = C2 */ \
            __ASM_EMIT("mulps       %%xmm1, %%xmm6")            /* xmm6 = x^2 * (C3 + x^2 * (C4 + C5 * x^2)) */ \
            __ASM_EMIT("mulps       %%xmm1, %%xmm7")            /* xmm7 = x^2 * (S3 + x^2 * (S4 + S5 * x^2)) */ \
            __ASM_EMIT("addps       %%xmm5, %%xmm6")            /* xmm6 = C2 + x^2 * (C3 + x^2 * (C4 + C5 * x^2)) */ \
            __ASM_EMIT("addps       %%xmm4, %%xmm7")            /* xmm7 = S2 + x^2 * (S3 + x^2 * (S4 + S5 * x^2)) */ \
            __ASM_EMIT("movaps      %[S0], %%xmm4")             /* xmm4 = S0 */ \
            __ASM_EMIT("movaps      %[C1], %%xmm5")             /* xmm5 = C1 */ \
            __ASM_EMIT("mulps       %%xmm1, %%xmm6")            /* xmm6 = x^2 * (C2 + x^2 * (C3 + x^2 * (C4 + C5 * x^2))) */ \
            __ASM_EMIT("mulps       %%xmm1, %%xmm7")            /* xmm7 = x^2 * (S2 + x^2 * (S3 + x^2 * (S4 + S5 * x^2))) */ \
            __ASM_EMIT("addps       %%xmm5, %%xmm6")            /* xmm6 = C1 + x^2 * (C2 + x^2 * (C3 + x^2 * (C4 + C5 * x^2))) */ \
            __ASM_EMIT("addps       %%xmm4, %%xmm7")            /* xmm7 = S0 + x^2 * (S2 + x^2 * (S3 + x^2 * (S4 + S5 * x^2))) */ \
            __ASM_EMIT("movaps      %[S0], %%xmm4")             /* xmm4 = S0 */ \
            __ASM_EMIT("movaps      %[C0], %%xmm5")             /* xmm5 = C0 */ \
            __ASM_EMIT("mulps       %%xmm1, %%xmm6")            /* xmm6 = x^2 * (C1 + x^2 * (C2 + x^2 * (C3 + x^2 * (C4 + C5 * x^2)))) */ \
            __ASM_EMIT("mulps       %%xmm1, %%xmm7")            /* xmm7 = x^2 * (S0 + x^2 * (S2 + x^2 * (S3 + x^2 * (S4 + S5 * x^2)))) */ \
            __ASM_EMIT("addps       %%xmm5, %%xmm6")            /* xmm6 = C0 + x^2 * (C1 + x^2 * (C2 + x^2 * (C3 + x^2 * (C4 + C5 * x^2)))) */ \
            __ASM_EMIT("addps       %%xmm4, %%xmm7")            /* xmm7 = S0 + x^2 * (S0 + x^2 * (S2 + x^2 * (S3 + x^2 * (S4 + S5 * x^2)))) */ \
            __ASM_EMIT("xorps       %%xmm2, %%xmm6")            /* xmm6 = s * (C0 + x^2 * (C1 + x^2 * (C2 + x^2 * (C3 + x^2 * (C4 + C5 * x^2))))) */ \
            __ASM_EMIT("mulps       %%xmm0, %%xmm7")            /* xmm7 = x * (S0 + x^2 * (S0 + x^2 * (S2 + x^2 * (S3 + x^2 * (S4 + S5 * x^2))))) */ \
            \
            /* Now cosines (w_re) are in xmm6, sines (w_im) in xmm7 */ \
            : \
            : "r"(pairs), "r"(p), \
                [S0] "m"(SIN_C0), \
                [S0] "m"(SIN_C0), \
                [S2] "m"(SIN_C2), \
                [S3] "m"(SIN_C3), \
                [S4] "m"(SIN_C4), \
                [S5] "m"(SIN_C5), \
                [C0] "m"(COS_C0), \
                [C1] "m"(COS_C1), \
                [C2] "m"(COS_C2), \
                [C3] "m"(COS_C3), \
                [C4] "m"(COS_C4), \
                [C5] "m"(COS_C5), \
                [PI] "m"(PI), \
                [PI_2] "m"(PI_2) \
            : \
            "%xmm0", "%xmm1", "%xmm2", "%xmm3", \
            "%xmm4", "%xmm5", "%xmm6", "%xmm7" \
        );

    // Normal-precision sines and cosines
    #define FFT_BUTTERFLY_SINCOS  \
        ARCH_X86_ASM \
        ( \
            /* Prepare angles for calculation */ \
            __ASM_EMIT("cvtsi2ss    %1, %%xmm0")                /* xmm0 = p */ \
            __ASM_EMIT("cvtsi2ss    %0, %%xmm3")                /* xmm3 = pairs */ \
            __ASM_EMIT("movaps      %[PI], %%xmm1")             /* xmm1 = pi */ \
            __ASM_EMIT("movaps      %[PI_2], %%xmm4")           /* xmm4 = pi/2 */ \
            __ASM_EMIT("movaps      %%xmm1, %%xmm2")            /* xmm2 = pi */ \
            __ASM_EMIT("divss       %%xmm3, %%xmm2")            /* xmm2 = D = pi / pairs */ \
            __ASM_EMIT("mulss       %%xmm2, %%xmm0")            /* xmm0 = A = p * pi / pairs */ \
            __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0")     /* xmm0 = A A A A */ \
            __ASM_EMIT("addss       %%xmm2, %%xmm0")            /* xmm0 = A+D A A A */ \
            __ASM_EMIT("shufps      $0x90, %%xmm0, %%xmm0")     /* xmm0 = A+D A+D A A */ \
            __ASM_EMIT("addss       %%xmm2, %%xmm0")            /* xmm0 = A+2D A+D A A */ \
            __ASM_EMIT("shufps      $0x90, %%xmm0, %%xmm0")     /* xmm0 = A+2D A+2D A+D A */ \
            __ASM_EMIT("addss       %%xmm2, %%xmm0")            /* xmm0 = A+3D A+2D A+D A */ \
            __ASM_EMIT("shufps      $0x1b, %%xmm0, %%xmm0")     /* xmm0 = X = A A+D A+2D A+3D */ \
            \
            /* We consider that the values are positive \
             * xmm0 = X, xmm1 = pi, xmm4 = pi/2 \
             */ \
            __ASM_EMIT("cmpltps     %%xmm0, %%xmm4")            /* xmm4 = X > pi */ \
            __ASM_EMIT("subps       %%xmm0, %%xmm1")            /* xmm1 = pi - X */ \
            __ASM_EMIT("movaps      %%xmm4, %%xmm2")            /* xmm2 = S */ \
            __ASM_EMIT("andps       %%xmm4, %%xmm1")            /* xmm1 = pi - X */ \
            __ASM_EMIT("andnps      %%xmm0, %%xmm4")            /* xmm4 = pi */ \
            __ASM_EMIT("orps        %%xmm4, %%xmm1")            /* xmm1 = x */ \
            __ASM_EMIT("pslld       $31, %%xmm2")               /* xmm2 = s */ \
            \
            /* Angles are in xmm0, calculate sines and cosines */ \
            /* Prepare data \
             * Predicates: \
             *   xmm1 = x, \
             *   xmm2 = s \
             * */ \
            __ASM_EMIT("movaps      %[S4], %%xmm7")             /* xmm2 = S4 */ \
            __ASM_EMIT("movaps      %[C4], %%xmm6")             /* xmm3 = C4 */ \
            __ASM_EMIT("movaps      %%xmm1, %%xmm0")            /* xmm0 = x */ \
            __ASM_EMIT("mulps       %%xmm1, %%xmm1")            /* xmm1 = x^2 */ \
            \
            /* Calculate */ \
            __ASM_EMIT("movaps      %[S3], %%xmm4")             /* xmm4 = S3 */ \
            __ASM_EMIT("movaps      %[C3], %%xmm5")             /* xmm5 = C3 */ \
            __ASM_EMIT("mulps       %%xmm1, %%xmm6")            /* xmm6 = C4 * x^2 */ \
            __ASM_EMIT("mulps       %%xmm1, %%xmm7")            /* xmm7 = S4 * x^2 */ \
            __ASM_EMIT("addps       %%xmm5, %%xmm6")            /* xmm6 = C3 + C4 * x^2 */ \
            __ASM_EMIT("addps       %%xmm4, %%xmm7")            /* xmm7 = S3 + S4 * x^2 */ \
            __ASM_EMIT("movaps      %[S2], %%xmm4")             /* xmm4 = S2 */ \
            __ASM_EMIT("movaps      %[C2], %%xmm5")             /* xmm5 = C2 */ \
            __ASM_EMIT("mulps       %%xmm1, %%xmm6")            /* xmm6 = x^2 * (C3 + C4 * x^2) */ \
            __ASM_EMIT("mulps       %%xmm1, %%xmm7")            /* xmm7 = x^2 * (S3 + S4 * x^2) */ \
            __ASM_EMIT("addps       %%xmm5, %%xmm6")            /* xmm6 = C2 + x^2 * (C3 + C4 * x^2) */ \
            __ASM_EMIT("addps       %%xmm4, %%xmm7")            /* xmm7 = S2 + x^2 * (S3 + S4 * x^2) */ \
            __ASM_EMIT("movaps      %[S1], %%xmm4")             /* xmm4 = S1 */ \
            __ASM_EMIT("movaps      %[C1], %%xmm5")             /* xmm5 = C1 */ \
            __ASM_EMIT("mulps       %%xmm1, %%xmm6")            /* xmm6 = x^2 * (C2 + x^2 * (C3 + C4 * x^2)) */ \
            __ASM_EMIT("mulps       %%xmm1, %%xmm7")            /* xmm7 = x^2 * (S2 + x^2 * (S3 + S4 * x^2)) */ \
            __ASM_EMIT("addps       %%xmm5, %%xmm6")            /* xmm6 = C1 + x^2 * (C2 + x^2 * (C3 + C4 * x^2)) */ \
            __ASM_EMIT("addps       %%xmm4, %%xmm7")            /* xmm7 = S1 + x^2 * (S2 + x^2 * (S3 + S4 * x^2)) */ \
            __ASM_EMIT("movaps      %[S0], %%xmm4")             /* xmm4 = S0 */ \
            __ASM_EMIT("movaps      %[C0], %%xmm5")             /* xmm5 = C0 */ \
            __ASM_EMIT("mulps       %%xmm1, %%xmm6")            /* xmm6 = x^2 * (C1 + x^2 * (C2 + x^2 * (C3 + C4 * x^2))) */ \
            __ASM_EMIT("mulps       %%xmm1, %%xmm7")            /* xmm7 = x^2 * (S1 + x^2 * (S2 + x^2 * (S3 + S4 * x^2))) */ \
            __ASM_EMIT("addps       %%xmm5, %%xmm6")            /* xmm6 = C0 + x^2 * (C1 + x^2 * (C2 + x^2 * (C3 + C4 * x^2))) */ \
            __ASM_EMIT("addps       %%xmm4, %%xmm7")            /* xmm7 = S0 + x^2 * (S1 + x^2 * (S2 + x^2 * (S3 + S4 * x^2))) */ \
            __ASM_EMIT("xorps       %%xmm2, %%xmm6")            /* xmm6 = s * (C0 + x^2 * (C1 + x^2 * (C2 + x^2 * (C3 + C4 * x^2)))) */ \
            __ASM_EMIT("mulps       %%xmm0, %%xmm7")            /* xmm7 = x * (S0 + x^2 * (S1 + x^2 * (S2 + x^2 * (S3 + S4 * x^2)))) */ \
            \
            /* Now cosines (w_re) are in xmm6, sines (w_im) in xmm7 */ \
            : \
            : "r"(pairs), "r"(p), \
                [S0] "m"(S0), \
                [S1] "m"(S1), \
                [S2] "m"(S2), \
                [S3] "m"(S3), \
                [S4] "m"(S4), \
                [C0] "m"(C0), \
                [C1] "m"(C1), \
                [C2] "m"(C2), \
                [C3] "m"(C3), \
                [C4] "m"(C4), \
                [PI] "m"(PI), \
                [PI_2] "m"(PI_2) \
            : \
            "%xmm0", "%xmm1", "%xmm2", "%xmm3", \
            "%xmm4", "%xmm5", "%xmm6", "%xmm7" \
        );
#endif /* NO_SINCOS4 */

#define FFT_BUTTERFLY_BODY(add_b, add_a) \
    /* Init pointers */ \
    register float *a_re    = &dst_re[p]; \
    register float *a_im    = &dst_im[p]; \
    register float *b_re    = &a_re[pairs]; \
    register float *b_im    = &a_im[pairs]; \
    register size_t b       = blocks; \
    \
    ARCH_X86_ASM \
    ( \
        /* Prefetch data */ \
        /*__ASM_EMIT("prefetchnta (%[a_re])") */ \
        /* __ASM_EMIT("prefetchnta (%[a_im])") */ \
        /* __ASM_EMIT("prefetchnta (%[b_re])") */ \
        /* __ASM_EMIT("prefetchnta (%[b_im])") */ \
        \
        /* Start loop */ \
        __ASM_EMIT("1:") \
        \
        /* Prefetch next data */ \
        /* __ASM_EMIT("prefetchnta (%[a_re], %[pairs], 8)") */ \
        /* __ASM_EMIT("prefetchnta (%[a_im], %[pairs], 8)") */ \
        /* __ASM_EMIT("prefetchnta (%[b_re], %[pairs], 8)") */ \
        /* __ASM_EMIT("prefetchnta (%[b_im], %[pairs], 8)") */ \
        \
        /* Load complex values */ \
        /* predicate: xmm6 = w_re[0..3] */ \
        /* predicate: xmm7 = w_im[0..3] */ \
        __ASM_EMIT(LS_RE "     (%[a_re]), %%xmm0")   /* xmm0 = a_re[0..3] */ \
        __ASM_EMIT(LS_IM "     (%[a_im]), %%xmm1")   /* xmm1 = a_im[0..3] */ \
        __ASM_EMIT(LS_RE "     (%[b_re]), %%xmm2")   /* xmm2 = b_re[0..3] */ \
        __ASM_EMIT(LS_IM "     (%[b_im]), %%xmm3")   /* xmm3 = b_im[0..3] */ \
        \
        /* Calculate complex multiplication */ \
        __ASM_EMIT("movaps     %%xmm2, %%xmm4") /* xmm4 = b_re[0..3] */ \
        __ASM_EMIT("movaps     %%xmm3, %%xmm5") /* xmm5 = b_im[0..3] */ \
        __ASM_EMIT("mulps      %%xmm6, %%xmm2") /* xmm2 = w_re[0..3] * b_re[0..3] */ \
        __ASM_EMIT("mulps      %%xmm6, %%xmm3") /* xmm3 = w_re[0..3] * b_im[0..3] */ \
        __ASM_EMIT("mulps      %%xmm7, %%xmm4") /* xmm4 = w_im[0..3] * b_re[0..3] */ \
        __ASM_EMIT("mulps      %%xmm7, %%xmm5") /* xmm5 = w_im[0..3] * b_im[0..3] */ \
        __ASM_EMIT(add_b "     %%xmm5, %%xmm2") /* xmm2 = c_re[0..3] = w_re[0..3] * b_re[0..3] +- w_im[0..3] * b_im[0..3] */ \
        __ASM_EMIT(add_a "     %%xmm4, %%xmm3") /* xmm3 = c_im[0..3] = w_re[0..3] * b_im[0..3] -+ w_im[0..3] * b_re[0..3] */ \
        \
        /* Perform butterfly */ \
        __ASM_EMIT("movaps     %%xmm0, %%xmm4") /* xmm4 = a_re[0..3] */ \
        __ASM_EMIT("movaps     %%xmm1, %%xmm5") /* xmm5 = a_im[0..3] */ \
        __ASM_EMIT("subps      %%xmm2, %%xmm0") /* xmm0 = a_re[0..3] - c_re[0..3] */ \
        __ASM_EMIT("subps      %%xmm3, %%xmm1") /* xmm1 = a_im[0..3] - c_im[0..3] */ \
        __ASM_EMIT("addps      %%xmm4, %%xmm2") /* xmm2 = a_re[0..3] + c_re[0..3] */ \
        __ASM_EMIT("addps      %%xmm5, %%xmm3") /* xmm3 = a_im[0..3] + c_im[0..3] */ \
        \
        /* Store values */ \
        __ASM_EMIT(LS_RE "     %%xmm2, (%[a_re])") \
        __ASM_EMIT(LS_IM "     %%xmm3, (%[a_im])") \
        __ASM_EMIT(LS_RE "     %%xmm0, (%[b_re])") \
        __ASM_EMIT(LS_IM "     %%xmm1, (%[b_im])") \
        \
        /* Update pointers */ \
        __ASM_EMIT("lea        (%[a_re], %[pairs], 8), %[a_re]") \
        __ASM_EMIT("lea        (%[a_im], %[pairs], 8), %[a_im]") \
        __ASM_EMIT("lea        (%[b_re], %[pairs], 8), %[b_re]") \
        __ASM_EMIT("lea        (%[b_im], %[pairs], 8), %[b_im]") \
        \
        /* Repeat loop */ \
        __ASM_EMIT32("decl      %[b]") \
        __ASM_EMIT64("decq      %[b]") \
        __ASM_EMIT("jnz         1b") \
        \
        : [a_re] "+r"(a_re), [a_im] "+r"(a_im), [b_re] "+r"(b_re), [b_im] "+r"(b_im), [b] __ASM_ARG_RW(b) \
        : [pairs] "r"(pairs) \
        : "cc", "memory",  \
        "%xmm0", "%xmm1", "%xmm2", "%xmm3", \
        "%xmm4", "%xmm5" \
    );

#define FFT_BUTTERFLY_BODY2(add_b, add_a) \
    /* Init pointers */ \
    register float *a_re    = &dst_re[b*pairs*2]; \
    register float *a_im    = &dst_im[b*pairs*2]; \
    register float *b_re    = &a_re[pairs]; \
    register float *b_im    = &a_im[pairs]; \
    register size_t p       = pairs; \
    __IF_32(size_t tmp_re, tmp_im); \
    \
    ARCH_X86_ASM \
    ( \
        /* Prefetch data */ \
        /*__ASM_EMIT("prefetchnta (%[a_re])") */ \
        /* __ASM_EMIT("prefetchnta (%[a_im])") */ \
        /* __ASM_EMIT("prefetchnta (%[b_re])") */ \
        /* __ASM_EMIT("prefetchnta (%[b_im])") */ \
        \
        /* Prepare angle */ \
        __ASM_EMIT32("mov       %[a_re], %[tmp_re]") \
        __ASM_EMIT32("mov       %[a_im], %[tmp_im]") \
        __ASM_EMIT32("mov       %[XFFT_A_RE], %[a_re]") \
        __ASM_EMIT32("mov       %[XFFT_A_IM], %[a_im]") \
        __ASM_EMIT32("movaps    (%[a_re], %[rank]), %%xmm6")        /* xmm6 = angle_re[0..3] */ \
        __ASM_EMIT32("movaps    (%[a_im], %[rank]), %%xmm7")        /* xmm7 = angle_im[0..3] */ \
        __ASM_EMIT32("mov       %[tmp_re], %[a_re]") \
        __ASM_EMIT32("mov       %[tmp_im], %[a_im]") \
        __ASM_EMIT64("movaps    (%[XFFT_A_RE], %[rank]), %%xmm6")   /* xmm6 = angle_re[0..3] */ \
        __ASM_EMIT64("movaps    (%[XFFT_A_IM], %[rank]), %%xmm7")   /* xmm7 = angle_im[0..3] */ \
        /* Start loop */ \
        __ASM_EMIT(".align 16") \
        __ASM_EMIT("1:") \
        \
        /* Prefetch next data */ \
        /* __ASM_EMIT("prefetchnta (%[a_re], %[pairs], 8)") */ \
        /* __ASM_EMIT("prefetchnta (%[a_im], %[pairs], 8)") */ \
        /* __ASM_EMIT("prefetchnta (%[b_re], %[pairs], 8)") */ \
        /* __ASM_EMIT("prefetchnta (%[b_im], %[pairs], 8)") */ \
        \
        /* Load complex values */ \
        /* predicate: xmm6 = w_re[0..3] */ \
        /* predicate: xmm7 = w_im[0..3] */ \
        __ASM_EMIT(LS_RE "      (%[a_re]), %%xmm0")   /* xmm0 = a_re[0..3] */ \
        __ASM_EMIT(LS_IM "      (%[a_im]), %%xmm1")   /* xmm1 = a_im[0..3] */ \
        __ASM_EMIT(LS_RE "      (%[b_re]), %%xmm2")   /* xmm2 = b_re[0..3] */ \
        __ASM_EMIT(LS_IM "      (%[b_im]), %%xmm3")   /* xmm3 = b_im[0..3] */ \
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
        __ASM_EMIT(LS_RE "      %%xmm2, (%[a_re])") \
        __ASM_EMIT(LS_IM "      %%xmm3, (%[a_im])") \
        __ASM_EMIT(LS_RE "      %%xmm0, (%[b_re])") \
        __ASM_EMIT(LS_IM "      %%xmm1, (%[b_im])") \
        \
        /* Update pointers */ \
        __ASM_EMIT("add         $0x10, %[a_re]") \
        __ASM_EMIT("add         $0x10, %[b_re]") \
        __ASM_EMIT("add         $0x10, %[a_im]") \
        __ASM_EMIT("add         $0x10, %[b_im]") \
        \
        /* Repeat loop */ \
        __ASM_EMIT32("subl      $4, %[p]") \
        __ASM_EMIT64("subq      $4, %[p]") \
        __ASM_EMIT("jz          2f") \
        \
        /* Rotate angle */ \
        __ASM_EMIT32("mov       %[a_re], %[tmp_re]") \
        __ASM_EMIT32("mov       %[a_im], %[tmp_im]") \
        __ASM_EMIT32("mov       %[XFFT_W_RE], %[a_re]") \
        __ASM_EMIT32("mov       %[XFFT_W_IM], %[a_im]") \
        __ASM_EMIT32("movaps    (%[a_re], %[rank]), %%xmm0")            /* xmm0 = w_re[0..3] */ \
        __ASM_EMIT32("movaps    (%[a_im], %[rank]), %%xmm1")            /* xmm1 = w_im[0..3] */ \
        __ASM_EMIT32("mov       %[tmp_re], %[a_re]") \
        __ASM_EMIT32("mov       %[tmp_im], %[a_im]") \
        __ASM_EMIT64("movaps    (%[XFFT_W_IM], %[rank]), %%xmm1")       /* xmm1 = w_im[0..3] */ \
        __ASM_EMIT64("movaps    (%[XFFT_W_RE], %[rank]), %%xmm0")       /* xmm0 = w_re[0..3] */ \
        \
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
        : [a_re] "+r"(a_re), [a_im] "+r"(a_im), [b_re] "+r"(b_re), [b_im] "+r"(b_im), [p] __ASM_ARG_RW(p) \
        : [rank] "r" (rank), \
          __IF_64([XFFT_A_RE] "r"(XFFT_A_RE), [XFFT_A_IM] "r"(XFFT_A_IM), [XFFT_W_RE] "r"(XFFT_W_RE), [XFFT_W_IM] "r"(XFFT_W_IM)) \
          __IF_32([XFFT_A_RE] "g"(XFFT_A_RE), [XFFT_A_IM] "g"(XFFT_A_IM), [XFFT_W_RE] "g"(XFFT_W_RE), [XFFT_W_IM] "g"(XFFT_W_IM), [tmp_re] "g"(&tmp_re), [tmp_im] "g"(&tmp_im)) \
        : "cc", "memory",  \
        "%xmm0", "%xmm1", "%xmm2", "%xmm3", \
        "%xmm4", "%xmm5" \
    );

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

#if 0
static void FFT_BUTTERFLY_DIRECT_NAME(float *dst_re, float *dst_im, size_t rank, size_t blocks)
{
    size_t pairs = 1 << rank;

    FFT_ANGLE_INIT;

    size_t p = 0;
//            for (size_t p=0; p < pairs; p += 4)
    while (1)
    {
        // Calculate sines and cosines The result is in xmm6 (cosine) and xmm7 (sine)
//                FFT_BUTTERFLY_SINCOS;

        // Do the butterfly body
        FFT_BUTTERFLY_BODY("addps", "subps");

        p += 4;
        if (p >= pairs)
            break;

        FFT_ANGLE_ROTATE;
    }
}

static void FFT_BUTTERFLY_REVERSE_NAME(float *dst_re, float *dst_im, size_t rank, size_t blocks)
{
    size_t pairs = 1 << rank;

    FFT_ANGLE_INIT;

//            for (size_t p=0; p < pairs; p += 4)
    size_t p = 0;
    while (1)
    {
        // Calculate sines and cosines The result is in xmm6 (cosine) and xmm7 (sine)
//                FFT_BUTTERFLY_SINCOS;

        // Do the butterfly body
        FFT_BUTTERFLY_BODY("subps", "addps");

        p += 4;
        if (p >= pairs)
            break;

        FFT_ANGLE_ROTATE;
    }
}

#else

static inline void FFT_BUTTERFLY_DIRECT_NAME(float *dst_re, float *dst_im, size_t rank, size_t blocks)
{
    size_t pairs = 1 << rank;
    rank = (rank - 2) << 4;

    for (size_t b=0; b<blocks; ++b)
    {
        FFT_BUTTERFLY_BODY2("addps", "subps");
    }
}

static inline void FFT_BUTTERFLY_REVERSE_NAME(float *dst_re, float *dst_im, size_t rank, size_t blocks)
{
    size_t pairs = 1 << rank;
    rank = (rank - 2) << 4;

    for (size_t b=0; b<blocks; ++b)
    {
        FFT_BUTTERFLY_BODY2("subps", "addps");
    }
}
#endif


#undef FFT_ANGLE_INIT
#undef FFT_ANGLE_ROTATE
#undef FFT_BUTTERFLY_SINCOS_PREC
#undef FFT_BUTTERFLY_SINCOS
#undef FFT_BUTTERFLY_BODY
#undef FFT_BUTTERFLY_BODY2

#undef FFT_BUTTERFLY_DIRECT_NAME
#undef FFT_BUTTERFLY_REVERSE_NAME
#undef LS_RE
#undef LS_IM
