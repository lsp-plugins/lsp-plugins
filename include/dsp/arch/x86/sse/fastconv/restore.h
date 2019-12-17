/*
 * restore.h
 *
 *  Created on: 6 мар. 2017 г.
 *      Author: sadko
 */
#ifndef DSP_ARCH_X86_SSE_FASTCONV_H_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_SSE_FASTCONV_H_IMPL */

namespace sse
{
    void fastconv_restore(float *dst, float *tmp, size_t rank)
    {
        // Prepare for butterflies
        size_t last     = size_t(1) << rank;
        size_t items    = last;
        float *ptr      = tmp;

        ARCH_X86_ASM
        (
    //                __ASM_EMIT(".align 16")
            __ASM_EMIT("1:")

            // Load data
            __ASM_EMIT("movups      0x00(%[ptr]), %%xmm1")
            __ASM_EMIT("movups      0x10(%[ptr]), %%xmm3")
            __ASM_EMIT("movups      0x20(%[ptr]), %%xmm5")
            __ASM_EMIT("movups      0x30(%[ptr]), %%xmm7")

            // Do x4 reverse butterflies
            // xmm1 = r0 r1 r2 r3
            // xmm3 = i0 i1 i2 i3
            // xmm5 = r4 r5 r6 r7
            // xmm7 = i4 i5 i6 i7
            __ASM_EMIT("movaps      %%xmm1, %%xmm0")            /* xmm0 = r0 r1 r2 r3 */
            __ASM_EMIT("movaps      %%xmm5, %%xmm4")
            __ASM_EMIT("shufps      $0x88, %%xmm3, %%xmm0")     /* xmm0 = r0 r2 i0 i2 */
            __ASM_EMIT("shufps      $0x88, %%xmm7, %%xmm4")
            __ASM_EMIT("shufps      $0xdd, %%xmm3, %%xmm1")     /* xmm1 = r1 r3 i1 i3 */
            __ASM_EMIT("shufps      $0xdd, %%xmm7, %%xmm5")
            __ASM_EMIT("movaps      %%xmm0, %%xmm2")            /* xmm2 = r0 r2 i0 i2 */
            __ASM_EMIT("movaps      %%xmm4, %%xmm6")
            __ASM_EMIT("addps       %%xmm1, %%xmm0")            /* xmm0 = r0+r1 r2+r3 i0+i1 i2+i3 = r0k r2k i0k i2k */
            __ASM_EMIT("addps       %%xmm5, %%xmm4")
            __ASM_EMIT("subps       %%xmm1, %%xmm2")            /* xmm2 = r0-r1 r2-r3 i0-i1 i2-i3 = r1k r3k i1k i3k */
            __ASM_EMIT("subps       %%xmm5, %%xmm6")

            __ASM_EMIT("movaps      %%xmm0, %%xmm1")            /* xmm1 = r0k r2k i0k i2k */
            __ASM_EMIT("movaps      %%xmm4, %%xmm5")
            __ASM_EMIT("shufps      $0x88, %%xmm2, %%xmm0")     /* xmm0 = r0k i0k r1k i1k */
            __ASM_EMIT("shufps      $0x88, %%xmm6, %%xmm4")
            __ASM_EMIT("shufps      $0x7d, %%xmm2, %%xmm1")     /* xmm1 = r2k i2k i3k r3k */
            __ASM_EMIT("shufps      $0x7d, %%xmm6, %%xmm5")
            __ASM_EMIT("movaps      %%xmm0, %%xmm2")            /* xmm2 = r0k i0k r1k i1k */
            __ASM_EMIT("movaps      %%xmm4, %%xmm6")
            __ASM_EMIT("addps       %%xmm1, %%xmm0")            /* xmm0 = r0k+r2k i0k+i2k r1k+i3k i1k+r3k = d0 d4 d3 d5 */
            __ASM_EMIT("addps       %%xmm5, %%xmm4")
            __ASM_EMIT("subps       %%xmm1, %%xmm2")            /* xmm2 = r0k-r2k i0k-i2k r1k-i3k i1k-r3k = d2 d6 d1 d7  */
            __ASM_EMIT("subps       %%xmm5, %%xmm6")

            __ASM_EMIT("movaps      %%xmm0, %%xmm1")            /* xmm1 = d0 d4 d3 d5 */
            __ASM_EMIT("movaps      %%xmm4, %%xmm5")
            __ASM_EMIT("shufps      $0x88, %%xmm2, %%xmm0")     /* xmm0 = d0 d3 d2 d1 */
            __ASM_EMIT("shufps      $0x88, %%xmm6, %%xmm4")
            __ASM_EMIT("shufps      $0xdd, %%xmm2, %%xmm1")     /* xmm1 = d4 d5 d6 d7 */
            __ASM_EMIT("shufps      $0xdd, %%xmm6, %%xmm5")
            __ASM_EMIT("shufps      $0x6c, %%xmm0, %%xmm0")     /* xmm0 = d0 d1 d2 d3 */
            __ASM_EMIT("shufps      $0x6c, %%xmm4, %%xmm4")

            // Finally store result
            __ASM_EMIT("movups      %%xmm0, 0x00(%[ptr])")
            __ASM_EMIT("movups      %%xmm1, 0x10(%[ptr])")
            __ASM_EMIT("movups      %%xmm4, 0x20(%[ptr])")
            __ASM_EMIT("movups      %%xmm5, 0x30(%[ptr])")

            __ASM_EMIT("add         $0x40, %[ptr]")
            __ASM_EMIT("sub         $8, %[k]")
            __ASM_EMIT("jnz         1b")

             : [ptr] "+r" (ptr), [k] "+r" (items)
             :
             : "cc", "memory",
               "%xmm0", "%xmm1", "%xmm2", "%xmm3",
               "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );

        items           = last << 1;
        size_t n        = 8;
        size_t bs       = n << 1;

        const float *wk     = XFFT_W;
        const float *ak     = XFFT_A;

        // Iterate butterflies
        while (n < last)
        {
            for (size_t p=0; p<items; p += bs)
            {
                // Set initial values of pointers
                float *a        = &tmp[p];
                float *b        = &a[n];
                size_t k        = n;

                ARCH_X86_ASM
                (
                    __ASM_EMIT("movaps      0x00(%[ak]), %%xmm6")       /* xmm6 = rak[i] */
                    __ASM_EMIT("movaps      0x10(%[ak]), %%xmm7")       /* xmm7 = iak[i] */
                    :
                    : [ak] "r"(ak)
                    : "%xmm6", "%xmm7"
                );

                ARCH_X86_ASM
                (
    //                        __ASM_EMIT(".align 16")
                    __ASM_EMIT("1:")

                    __ASM_EMIT("movups      0x00(%[a]), %%xmm0")        /* xmm0 = ra[i] */
                    __ASM_EMIT("movups      0x10(%[a]), %%xmm1")        /* xmm1 = ia[i] */
                    __ASM_EMIT("movups      0x00(%[b]), %%xmm2")        /* xmm2 = rb[i] */
                    __ASM_EMIT("movups      0x10(%[b]), %%xmm3")        /* xmm3 = ib[i] */

                    __ASM_EMIT("movaps      %%xmm2, %%xmm4")            /* xmm4 = rb[i] */
                    __ASM_EMIT("movaps      %%xmm3, %%xmm5")            /* xmm5 = ib[i] */
                    __ASM_EMIT("mulps       %%xmm6, %%xmm2")            /* xmm2 = rak[i]*rb[i] */
                    __ASM_EMIT("mulps       %%xmm7, %%xmm4")            /* xmm4 = iak[i]*rb[i] */
                    __ASM_EMIT("mulps       %%xmm6, %%xmm3")            /* xmm3 = rak[i]*ib[i] */
                    __ASM_EMIT("mulps       %%xmm7, %%xmm5")            /* xmm5 = iak[i]*ib[i] */
                    __ASM_EMIT("addps       %%xmm4, %%xmm3")            /* xmm3 = rak[i]*ib[i] + iak[i]*rb[i] = ic[i] */
                    __ASM_EMIT("subps       %%xmm5, %%xmm2")            /* xmm2 = rak[i]*rb[i] - iak[i]*ib[i] = rc[i] */
                    __ASM_EMIT("movaps      %%xmm0, %%xmm4")            /* xmm4 = ra[i] */
                    __ASM_EMIT("movaps      %%xmm1, %%xmm5")            /* xmm5 = ia[i] */
                    __ASM_EMIT("subps       %%xmm2, %%xmm0")            /* xmm0 = ra[i] - rc[i] */
                    __ASM_EMIT("subps       %%xmm3, %%xmm1")            /* xmm1 = ia[i] - ic[i] */
                    __ASM_EMIT("addps       %%xmm4, %%xmm2")            /* xmm2 = ra[i] + rc[i] */
                    __ASM_EMIT("addps       %%xmm5, %%xmm3")            /* xmm3 = ia[i] + ic[i] */

                    __ASM_EMIT("movups      %%xmm2, 0x00(%[a])")
                    __ASM_EMIT("movups      %%xmm3, 0x10(%[a])")
                    __ASM_EMIT("movups      %%xmm0, 0x00(%[b])")
                    __ASM_EMIT("movups      %%xmm1, 0x10(%[b])")

                    __ASM_EMIT("add         $0x20, %[a]")
                    __ASM_EMIT("add         $0x20, %[b]")
                    __ASM_EMIT("sub         $8, %[k]")
                    __ASM_EMIT("jz          2f")

                    /* Rotate angle */
                    __ASM_EMIT("movaps      0x00(%[wk]), %%xmm0")       /* xmm0 = rw */
                    __ASM_EMIT("movaps      0x10(%[wk]), %%xmm1")       /* xmm1 = iw */
                    __ASM_EMIT("movaps      %%xmm0, %%xmm2")            /* xmm2 = rw */
                    __ASM_EMIT("movaps      %%xmm1, %%xmm3")            /* xmm3 = iw */
                    __ASM_EMIT("mulps       %%xmm6, %%xmm3")            /* xmm3 = ra * iw */
                    __ASM_EMIT("mulps       %%xmm7, %%xmm1")            /* xmm1 = ia * iw */
                    __ASM_EMIT("mulps       %%xmm0, %%xmm6")            /* xmm6 = ra * rw */
                    __ASM_EMIT("mulps       %%xmm2, %%xmm7")            /* xmm7 = ia * rw */
                    __ASM_EMIT("subps       %%xmm1, %%xmm6")            /* xmm6 = ra * rw - ia * iw */
                    __ASM_EMIT("addps       %%xmm3, %%xmm7")            /* xmm7 = ra * iw + ia * rw */
                    __ASM_EMIT("jmp         1b")

                    __ASM_EMIT("2:")

                    : [a] "+r"(a), [b] "+r"(b), [k] "+r" (k)
                    : [wk] "r"(wk)
                    : "cc", "memory",
                      "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                      "%xmm4", "%xmm5", "%xmm6", "%xmm7"
                );
            }

            wk     += 8;
            ak     += 8;
            n     <<= 1;
            bs    <<= 1;
        }

        if (n < items)
        {
            // ONE LARGE CYCLE
            // Set initial values of pointers
            float kn            = 1.0f / last;
            size_t k            = n;

            ARCH_X86_ASM
            (
                __ASM_EMIT("movaps      0x00(%[ak]), %%xmm6")           /* xmm6 = rak[i] */
                __ASM_EMIT("movaps      0x10(%[ak]), %%xmm7")           /* xmm7 = iak[i] */
                __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0")         /* xmm0 = kn */
                __ASM_EMIT("movaps      %%xmm0, %%xmm1")                /* xmm1 = kn */
                : "+Yz" (kn)
                : [ak] "r" (ak)
                : "%xmm1", "%xmm6", "%xmm7"
            );

            ARCH_X86_ASM
            (
    //                    __ASM_EMIT(".align 16")
                __ASM_EMIT("1:")

                __ASM_EMIT("movups      0x00(%[tmp]), %%xmm4")          /* xmm4 = ra[i] */
                __ASM_EMIT("movups      0x00(%[tmp],%[n],4), %%xmm2")   /* xmm2 = rb[i] */
                __ASM_EMIT("movups      0x10(%[tmp],%[n],4), %%xmm3")   /* xmm3 = ib[i] */

                __ASM_EMIT("mulps       %%xmm6, %%xmm2")                /* xmm2 = rak[i]*rb[i] */
                __ASM_EMIT("mulps       %%xmm7, %%xmm3")                /* xmm3 = iak[i]*ib[i] */
                __ASM_EMIT("movaps      %%xmm4, %%xmm5")                /* xmm5 = ra[i] */
                __ASM_EMIT("subps       %%xmm3, %%xmm2")                /* xmm2 = rak[i]*rb[i] - iak[i]*ib[i] = rc[i] */
                __ASM_EMIT("addps       %%xmm2, %%xmm4")                /* xmm4 = ra[i] + rc[i] */
                __ASM_EMIT("subps       %%xmm2, %%xmm5")                /* xmm5 = ra[i] - rc[i] */
                __ASM_EMIT("mulps       %%xmm0, %%xmm4")                /* xmm4 = kn*(ra[i] + rc[i]) */
                __ASM_EMIT("mulps       %%xmm1, %%xmm5")                /* xmm5 = kn*(ra[i] - rc[i]) */

                __ASM_EMIT("movups      %%xmm4, 0x00(%[dst])")
                __ASM_EMIT("movups      %%xmm5, 0x00(%[dst],%[n],2)")

                __ASM_EMIT("add         $0x20, %[tmp]")
                __ASM_EMIT("add         $0x10, %[dst]")
                __ASM_EMIT("sub         $8, %[k]")
                __ASM_EMIT("jz          2f")

                /* Rotate angle */
                __ASM_EMIT("movaps      0x00(%[wk]), %%xmm4")           /* xmm4 = rw */
                __ASM_EMIT("movaps      0x10(%[wk]), %%xmm5")           /* xmm5 = iw */
                __ASM_EMIT("movaps      %%xmm4, %%xmm2")                /* xmm2 = rw */
                __ASM_EMIT("movaps      %%xmm5, %%xmm3")                /* xmm3 = iw */
                __ASM_EMIT("mulps       %%xmm6, %%xmm3")                /* xmm3 = ra * iw */
                __ASM_EMIT("mulps       %%xmm7, %%xmm5")                /* xmm5 = ia * iw */
                __ASM_EMIT("mulps       %%xmm4, %%xmm6")                /* xmm6 = ra * rw */
                __ASM_EMIT("mulps       %%xmm2, %%xmm7")                /* xmm7 = ia * rw */
                __ASM_EMIT("subps       %%xmm5, %%xmm6")                /* xmm6 = ra * rw - ia * iw */
                __ASM_EMIT("addps       %%xmm3, %%xmm7")                /* xmm7 = ra * iw + ia * rw */
                __ASM_EMIT("jmp         1b")

                __ASM_EMIT("2:")

                : [tmp] "+r"(tmp), [dst] "+r"(dst), [n] "+r"(n), [k] "+r" (k)
                : [wk] "r"(wk)
                : "cc", "memory",
                  "%xmm0", "%xmm5", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }
        else
        {
            // Add real result to the target (ignore complex result)
            float kn    = 1.0f / last;

            // Unpack 4x split complex to 4x real
            ARCH_X86_ASM
            (
                __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0")         /* xmm0 = kn */
                __ASM_EMIT("movups      0x00(%[tmp]), %%xmm1")          /* xmm0 = s[i] */
                __ASM_EMIT("movups      0x00(%[dst]), %%xmm2")
                __ASM_EMIT("mulps       %%xmm0, %%xmm1")
                __ASM_EMIT("addps       %%xmm1, %%xmm2")
                __ASM_EMIT("movups      %%xmm2, 0x00(%[dst])")
                : "+Yz" (kn)
                : [tmp] "r" (tmp), [dst] "r" (dst)
                : "cc", "memory",
                  "%xmm1", "%xmm2"
            );
        }

    } // RESTORE_IMPL
}
