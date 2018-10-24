/*
 * parse.h
 *
 *  Created on: 5 мар. 2017 г.
 *      Author: sadko
 */
#ifndef DSP_ARCH_X86_SSE_FASTCONV_H_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_SSE_FASTCONV_H_IMPL */

static inline void PARSE_IMPL(float *dst, const float *src, size_t rank)
{
    // Prepare for butterflies
    const float *wk     = &XFFT_W[(rank-3) << 3];
    const float *ak     = &XFFT_A[(rank-3) << 3];
    size_t items        = size_t(1) << (rank + 1);
    size_t bs           = items;
    size_t n            = bs >> 1;

    // Iterate first cycle
    if (n > 4)
    {
        // ONE LARGE CYCLE
        // Set initial values of pointers
        float *a            = dst;
        float *b            = &a[n];
        size_t k            = n;

        ARCH_X86_ASM
        (
            __ASM_EMIT("movaps      0x00(%[ak]), %%xmm6")       /* xmm6 = rA[i] */
            __ASM_EMIT("movaps      0x10(%[ak]), %%xmm7")       /* xmm7 = iA[i] */
            __ASM_EMIT("xorps       %%xmm5, %%xmm5")            /* xmm5 = 0 */
            :
            : [ak] "r"(ak)
            :
              "%xmm5", "%xmm6", "%xmm7"
        );

        ARCH_X86_ASM
        (
//                    __ASM_EMIT(".align 16")
            __ASM_EMIT("1:")

            __ASM_EMIT(MV_SRC "     0x00(%[src]), %%xmm0")      /* xmm0 = s[i] */
            __ASM_EMIT("xorps       %%xmm3, %%xmm3")            /* xmm3 = 0 */
            __ASM_EMIT("movaps      %%xmm0, %%xmm4")            /* xmm4 = s[i] */
            __ASM_EMIT("movaps      %%xmm0, %%xmm2")            /* xmm2 = s[i] */
            __ASM_EMIT("mulps       %%xmm7, %%xmm4")            /* xmm4 = s[i]*iA[i] */
            __ASM_EMIT("mulps       %%xmm6, %%xmm2")            /* xmm2 = s[i]*rA[i] */
            __ASM_EMIT("subps       %%xmm4, %%xmm3")            /* xmm3 = -s[i]*iA[i] */

            __ASM_EMIT(MV_DST "     %%xmm0, 0x00(%[a])")
            __ASM_EMIT(MV_DST "     %%xmm5, 0x10(%[a])")
            __ASM_EMIT(MV_DST "     %%xmm2, 0x00(%[b])")
            __ASM_EMIT(MV_DST "     %%xmm3, 0x10(%[b])")

            __ASM_EMIT("add         $0x10, %[src]")
            __ASM_EMIT("add         $0x20, %[a]")
            __ASM_EMIT("add         $0x20, %[b]")

            /* Repeat loop */
            __ASM_EMIT("sub         $8, %[k]")
            __ASM_EMIT("jz          2f")

            /* Rotate angle */
            __ASM_EMIT("movaps      0x00(%[wk]), %%xmm0")       /* xmm0 = rw[i] */
            __ASM_EMIT("movaps      0x10(%[wk]), %%xmm1")       /* xmm1 = iw[i] */
            __ASM_EMIT("movaps      %%xmm0, %%xmm2")            /* xmm2 = rw[i] */
            __ASM_EMIT("movaps      %%xmm1, %%xmm3")            /* xmm3 = iw[i] */
            __ASM_EMIT("mulps       %%xmm6, %%xmm3")            /* xmm3 = rA[i] * iw[i] */
            __ASM_EMIT("mulps       %%xmm7, %%xmm1")            /* xmm1 = iA[i] * iw[i] */
            __ASM_EMIT("mulps       %%xmm0, %%xmm6")            /* xmm6 = rA[i] * rw[i] */
            __ASM_EMIT("mulps       %%xmm2, %%xmm7")            /* xmm7 = iA[i] * rw[i] */
            __ASM_EMIT("subps       %%xmm1, %%xmm6")            /* xmm6 = rA[i] * rw[i] - iA[i] * iw[i] */
            __ASM_EMIT("addps       %%xmm3, %%xmm7")            /* xmm7 = rA[i] * iw[i] + iA[i] * rw[i] */
            __ASM_EMIT("jmp         1b")

            __ASM_EMIT("2:")

            : [a] "+r" (a), [b] "+r" (b), [src] "+r" (src), [k] "+r" (k)
            : [wk] "r"(wk)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );

        wk     -= 8;
        ak     -= 8;
        n     >>= 1;
        bs    >>= 1;
    }
    else
    {
        // Unpack 4x real to 4x split complex
        ARCH_X86_ASM
        (
            __ASM_EMIT(MV_SRC "     0x00(%[src]), %%xmm0")      /* xmm0 = s[i] */
            __ASM_EMIT("xorps       %%xmm1, %%xmm1")            /* xmm1 = 0 */
            __ASM_EMIT(MV_DST "     %%xmm0, 0x00(%[dst])")
            __ASM_EMIT(MV_DST "     %%xmm1, 0x10(%[dst])")
            :
            : [items] "r" (items), [src] "r" (src), [dst] "r" (dst)
            : "cc", "memory",
              "%xmm0", "%xmm1"
        );
    }

    // Iterate butterflies
    for (; n > 4; n >>= 1, bs >>= 1)
    {
        for (size_t p=0; p<items; p += bs)
        {
            // Set initial values of pointers
            float *a            = &dst[p];
            float *b            = &a[n];
            size_t k            = n;

            ARCH_X86_ASM
            (
                __ASM_EMIT("movaps      0x00(%[ak]), %%xmm6")       /* xmm6 = rA[i] */
                __ASM_EMIT("movaps      0x10(%[ak]), %%xmm7")       /* xmm7 = iA[i] */
                :
                : [ak] "r"(ak)
                : "%xmm6", "%xmm7"
            );

            ARCH_X86_ASM
            (
//                        __ASM_EMIT(".align 16")
                __ASM_EMIT("1:")

                __ASM_EMIT(MV_DST "     0x00(%[a]), %%xmm0")        /* xmm0 = ra[i] */
                __ASM_EMIT(MV_DST "     0x10(%[a]), %%xmm1")        /* xmm1 = ia[i] */
                __ASM_EMIT(MV_DST "     0x00(%[b]), %%xmm2")        /* xmm2 = rb[i] */
                __ASM_EMIT(MV_DST "     0x10(%[b]), %%xmm3")        /* xmm3 = ib[i] */

                __ASM_EMIT("movaps      %%xmm0, %%xmm4")            /* xmm4 = ra[i] */
                __ASM_EMIT("movaps      %%xmm1, %%xmm5")            /* xmm5 = ia[i] */
                __ASM_EMIT("addps       %%xmm2, %%xmm0")            /* xmm0 = ra[i]+rb[i] */
                __ASM_EMIT("addps       %%xmm3, %%xmm1")            /* xmm1 = ia[i]+ib[i] */
                __ASM_EMIT("subps       %%xmm2, %%xmm4")            /* xmm4 = ra[i]-rb[i] = rc[i] */
                __ASM_EMIT("subps       %%xmm3, %%xmm5")            /* xmm5 = ia[i]-ib[i] = ic[i] */
                __ASM_EMIT("movaps      %%xmm4, %%xmm2")            /* xmm2 = rc[i] */
                __ASM_EMIT("movaps      %%xmm5, %%xmm3")            /* xmm3 = ic[i] */
                __ASM_EMIT("mulps       %%xmm6, %%xmm4")            /* xmm4 = rA[i]*rc[i] */
                __ASM_EMIT("mulps       %%xmm7, %%xmm5")            /* xmm5 = iA[i]*ic[i] */
                __ASM_EMIT("mulps       %%xmm6, %%xmm3")            /* xmm3 = rA[i]*ic[i] */
                __ASM_EMIT("mulps       %%xmm7, %%xmm2")            /* xmm2 = iA[i]*rc[i] */
                __ASM_EMIT("addps       %%xmm5, %%xmm4")            /* xmm4 = rA[i]*rc[i] + iA[i]*ic[i] */
                __ASM_EMIT("subps       %%xmm2, %%xmm3")            /* xmm3 = rA[i]*ic[i] - iA[i]*rc[i] */

                __ASM_EMIT(MV_DST "     %%xmm0, 0x00(%[a])")
                __ASM_EMIT(MV_DST "     %%xmm1, 0x10(%[a])")
                __ASM_EMIT(MV_DST "     %%xmm4, 0x00(%[b])")
                __ASM_EMIT(MV_DST "     %%xmm3, 0x10(%[b])")

                /* Repeat loop */
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

                : [a] "+r" (a), [b] "+r" (b), [k] "+r" (k)
                : [wk] "r"(wk)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        ak     -= 8;
        wk     -= 8;
    }

    ARCH_X86_ASM
    (
//                __ASM_EMIT(".align 16")
        __ASM_EMIT("1:")

        __ASM_EMIT(MV_DST "     0x00(%[dst]), %%xmm0")      /* xmm0 = r0 r1 r2 r3 */
        __ASM_EMIT(MV_DST "     0x10(%[dst]), %%xmm1")      /* xmm1 = i0 i1 i2 i3 */
        __ASM_EMIT(MV_DST "     0x20(%[dst]), %%xmm4")
        __ASM_EMIT(MV_DST "     0x30(%[dst]), %%xmm5")

        __ASM_EMIT("movaps      %%xmm0, %%xmm2")            /* xmm2 = r0 r1 r2 r3 */
        __ASM_EMIT("movaps      %%xmm4, %%xmm6")
        __ASM_EMIT("unpcklps    %%xmm1, %%xmm0")            /* xmm0 = r0 i0 r1 i1 */
        __ASM_EMIT("unpcklps    %%xmm5, %%xmm4")
        __ASM_EMIT("unpckhps    %%xmm1, %%xmm2")            /* xmm2 = r2 i2 r3 i3 */
        __ASM_EMIT("unpckhps    %%xmm5, %%xmm6")
        __ASM_EMIT("movaps      %%xmm0, %%xmm1")            /* xmm1 = r0 i0 r1 i1 */
        __ASM_EMIT("movaps      %%xmm4, %%xmm5")

        __ASM_EMIT("addps       %%xmm2, %%xmm0")            /* xmm0 = r0+r2 i0+i2 r1+r3 i1+i3 = r0k i0k r2k i2k */
        __ASM_EMIT("addps       %%xmm6, %%xmm4")
        __ASM_EMIT("subps       %%xmm2, %%xmm1")            /* xmm1 = r0-r2 i0-i2 r1-r3 i1-i3 = r1k i1k r3k i3k */
        __ASM_EMIT("subps       %%xmm6, %%xmm5")
        __ASM_EMIT("movaps      %%xmm0, %%xmm2")            /* xmm2 = r0k i0k r2k i2k */
        __ASM_EMIT("movaps      %%xmm4, %%xmm6")
        __ASM_EMIT("unpcklps    %%xmm1, %%xmm0")            /* xmm0 = r0k r1k i0k i1k */
        __ASM_EMIT("unpcklps    %%xmm5, %%xmm4")
        __ASM_EMIT("unpckhps    %%xmm1, %%xmm2")            /* xmm2 = r2k r3k i2k i3k */
        __ASM_EMIT("unpckhps    %%xmm5, %%xmm6")
        __ASM_EMIT("movaps      %%xmm0, %%xmm1")            /* xmm1 = r0k r1k i0k i1k */
        __ASM_EMIT("movaps      %%xmm4, %%xmm5")
        __ASM_EMIT("shufps      $0x6c, %%xmm2, %%xmm2")     /* xmm2 = r2k i3k i2k r3k */
        __ASM_EMIT("shufps      $0x6c, %%xmm6, %%xmm6")

        __ASM_EMIT("addps       %%xmm2, %%xmm0")            /* xmm0 = r0k+r2k r1k+i3k i0k+i2k i1k+r3k */
        __ASM_EMIT("addps       %%xmm6, %%xmm4")
        __ASM_EMIT("subps       %%xmm2, %%xmm1")            /* xmm1 = r0k-r2k r1k-i3k i0k-i2k i1k-r3k */
        __ASM_EMIT("subps       %%xmm6, %%xmm5")
        __ASM_EMIT("movaps      %%xmm0, %%xmm2")            /* xmm2 = r0k+r2k r1k+i3k i0k+i2k i1k+r3k */
        __ASM_EMIT("movaps      %%xmm4, %%xmm6")
        __ASM_EMIT("unpcklps    %%xmm1, %%xmm0")            /* xmm0 = r0k+r2k r0k-r2k r1k+i3k r1k-i3k */
        __ASM_EMIT("unpcklps    %%xmm5, %%xmm4")
        __ASM_EMIT("unpckhps    %%xmm1, %%xmm2")            /* xmm2 = i0k+i2k i0k-i2k i1k+r3k i1k-r3k */
        __ASM_EMIT("unpckhps    %%xmm5, %%xmm6")
        __ASM_EMIT("shufps      $0xb4, %%xmm2, %%xmm2")     /* xmm2 = i0k+i2k i0k-i2k i1k-r3k i1k+r3k */
        __ASM_EMIT("shufps      $0xb4, %%xmm6, %%xmm6")

        __ASM_EMIT(MV_DST "     %%xmm0, 0x00(%[dst])")
        __ASM_EMIT(MV_DST "     %%xmm2, 0x10(%[dst])")
        __ASM_EMIT(MV_DST "     %%xmm4, 0x20(%[dst])")
        __ASM_EMIT(MV_DST "     %%xmm6, 0x30(%[dst])")

        __ASM_EMIT("add         $0x40, %[dst]")
        __ASM_EMIT("sub         $16, %[k]")
        __ASM_EMIT("jnz         1b")

        : [dst] "+r" (dst), [k] "+r" (items)
        :
        : "cc", "memory",
          "%xmm0", "%xmm1", "%xmm2",
          "%xmm4", "%xmm5", "%xmm6"
    );
} // PARSE_IMPL

static inline void PARSE_INTERNAL_IMPL(float *dst, const float *src, size_t rank)
{
    // Prepare for butterflies
    const float *wk     = &XFFT_W[(rank-3) << 3];
    const float *ak     = &XFFT_A[(rank-3) << 3];
    size_t items        = size_t(1) << (rank + 1);
    size_t bs           = items;
    size_t n            = bs >> 1;

    // Iterate first cycle
    if (n > 4)
    {
        // ONE LARGE CYCLE
        // Set initial values of pointers
        float *a            = dst;
        float *b            = &a[n];
        size_t k            = n;

        ARCH_X86_ASM
        (
            __ASM_EMIT("movaps      0x00(%[ak]), %%xmm6")       /* xmm6 = rA[i] */
            __ASM_EMIT("movaps      0x10(%[ak]), %%xmm7")       /* xmm7 = iA[i] */
            __ASM_EMIT("xorps       %%xmm5, %%xmm5")            /* xmm5 = 0 */
            :
            : [ak] "r"(ak)
            :
              "%xmm5", "%xmm6", "%xmm7"
        );

        ARCH_X86_ASM
        (
//                    __ASM_EMIT(".align 16")
            __ASM_EMIT("1:")

            __ASM_EMIT(MV_SRC "     0x00(%[src]), %%xmm0")      /* xmm0 = s[i] */

            __ASM_EMIT("xorps       %%xmm3, %%xmm3")            /* xmm3 = 0 */
            __ASM_EMIT("movaps      %%xmm0, %%xmm4")            /* xmm4 = s[i] */
            __ASM_EMIT("movaps      %%xmm0, %%xmm2")            /* xmm2 = s[i] */
            __ASM_EMIT("mulps       %%xmm7, %%xmm4")            /* xmm4 = s[i]*iA[i] */
            __ASM_EMIT("mulps       %%xmm6, %%xmm2")            /* xmm2 = s[i]*rA[i] */
            __ASM_EMIT("subps       %%xmm4, %%xmm3")            /* xmm3 = -s[i]*iA[i] */

            __ASM_EMIT(MV_DST "     %%xmm0, 0x00(%[a])")
            __ASM_EMIT(MV_DST "     %%xmm5, 0x10(%[a])")
            __ASM_EMIT(MV_DST "     %%xmm2, 0x00(%[b])")
            __ASM_EMIT(MV_DST "     %%xmm3, 0x10(%[b])")

            __ASM_EMIT("add         $0x10, %[src]")
            __ASM_EMIT("add         $0x20, %[a]")
            __ASM_EMIT("add         $0x20, %[b]")

            /* Repeat loop */
            __ASM_EMIT32("subl      $8, %[k]")
            __ASM_EMIT64("sub       $8, %[k]")
            __ASM_EMIT("jz          2f")

            /* Rotate angle */
            __ASM_EMIT("movaps      0x00(%[wk]), %%xmm0")       /* xmm0 = rw[i] */
            __ASM_EMIT("movaps      0x10(%[wk]), %%xmm1")       /* xmm1 = iw[i] */
            __ASM_EMIT("movaps      %%xmm0, %%xmm2")            /* xmm2 = rw[i] */
            __ASM_EMIT("movaps      %%xmm1, %%xmm3")            /* xmm3 = iw[i] */
            __ASM_EMIT("mulps       %%xmm6, %%xmm3")            /* xmm3 = rA[i] * iw[i] */
            __ASM_EMIT("mulps       %%xmm7, %%xmm1")            /* xmm1 = iA[i] * iw[i] */
            __ASM_EMIT("mulps       %%xmm0, %%xmm6")            /* xmm6 = rA[i] * rw[i] */
            __ASM_EMIT("mulps       %%xmm2, %%xmm7")            /* xmm7 = iA[i] * rw[i] */
            __ASM_EMIT("subps       %%xmm1, %%xmm6")            /* xmm6 = rA[i] * rw[i] - iA[i] * iw[i] */
            __ASM_EMIT("addps       %%xmm3, %%xmm7")            /* xmm7 = rA[i] * iw[i] + iA[i] * rw[i] */
            __ASM_EMIT("jmp         1b")

            __ASM_EMIT("2:")

            : [a] "+r" (a), [b] "+r" (b), [src] "+r" (src), [k] "+r" (k)
            : [wk] "r" (wk)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );

        wk     -= 8;
        ak     -= 8;
        n     >>= 1;
        bs    >>= 1;
    }
    else
    {
        // Unpack 4x real to 4x split complex
        ARCH_X86_ASM
        (
            __ASM_EMIT(MV_SRC "     0x00(%[src]), %%xmm0")      /* xmm0 = s[i] */
            __ASM_EMIT("xorps       %%xmm1, %%xmm1")            /* xmm1 = 0 */
            __ASM_EMIT(MV_DST "     %%xmm0, 0x00(%[dst])")
            __ASM_EMIT(MV_DST "     %%xmm1, 0x10(%[dst])")
            :
            : [src] "r" (src), [dst] "r" (dst)
            : "cc", "memory",
              "%xmm0", "%xmm1"
        );
    }

    // Iterate butterflies
    for (; n > 4; n >>= 1, bs >>= 1)
    {
        for (size_t p=0; p<items; p += bs)
        {
            // Set initial values of pointers
            float *a            = &dst[p];
            float *b            = &a[n];
            size_t k            = n;

            ARCH_X86_ASM
            (
                __ASM_EMIT("movaps      0x00(%[ak]), %%xmm6")       /* xmm6 = rA[i] */
                __ASM_EMIT("movaps      0x10(%[ak]), %%xmm7")       /* xmm7 = iA[i] */
                :
                : [ak] "r"(ak)
                : "%xmm6", "%xmm7"
            );

            ARCH_X86_ASM
            (
//                        __ASM_EMIT(".align 16")
                __ASM_EMIT("1:")

                __ASM_EMIT(MV_DST "     0x00(%[a]), %%xmm0")        /* xmm0 = ra[i] */
                __ASM_EMIT(MV_DST "     0x10(%[a]), %%xmm1")        /* xmm1 = ia[i] */
                __ASM_EMIT(MV_DST "     0x00(%[b]), %%xmm2")        /* xmm2 = rb[i] */
                __ASM_EMIT(MV_DST "     0x10(%[b]), %%xmm3")        /* xmm3 = ib[i] */

                __ASM_EMIT("movaps      %%xmm0, %%xmm4")            /* xmm4 = ra[i] */
                __ASM_EMIT("movaps      %%xmm1, %%xmm5")            /* xmm5 = ia[i] */
                __ASM_EMIT("addps       %%xmm2, %%xmm0")            /* xmm0 = ra[i]+rb[i] */
                __ASM_EMIT("addps       %%xmm3, %%xmm1")            /* xmm1 = ia[i]+ib[i] */
                __ASM_EMIT("subps       %%xmm2, %%xmm4")            /* xmm4 = ra[i]-rb[i] = rc[i] */
                __ASM_EMIT("subps       %%xmm3, %%xmm5")            /* xmm5 = ia[i]-ib[i] = ic[i] */
                __ASM_EMIT("movaps      %%xmm4, %%xmm2")            /* xmm2 = rc[i] */
                __ASM_EMIT("movaps      %%xmm5, %%xmm3")            /* xmm3 = ic[i] */
                __ASM_EMIT("mulps       %%xmm6, %%xmm4")            /* xmm4 = rA[i]*rc[i] */
                __ASM_EMIT("mulps       %%xmm7, %%xmm5")            /* xmm5 = iA[i]*ic[i] */
                __ASM_EMIT("mulps       %%xmm6, %%xmm3")            /* xmm3 = rA[i]*ic[i] */
                __ASM_EMIT("mulps       %%xmm7, %%xmm2")            /* xmm2 = iA[i]*rc[i] */
                __ASM_EMIT("addps       %%xmm5, %%xmm4")            /* xmm4 = rA[i]*rc[i] + iA[i]*ic[i] */
                __ASM_EMIT("subps       %%xmm2, %%xmm3")            /* xmm3 = rA[i]*ic[i] - iA[i]*rc[i] */

                __ASM_EMIT(MV_DST "     %%xmm0, 0x00(%[a])")
                __ASM_EMIT(MV_DST "     %%xmm1, 0x10(%[a])")
                __ASM_EMIT(MV_DST "     %%xmm4, 0x00(%[b])")
                __ASM_EMIT(MV_DST "     %%xmm3, 0x10(%[b])")

                /* Repeat loop */
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

                : [a] "+r" (a), [b] "+r" (b), [k] "+r" (k)
                : [wk] "r"(wk)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        ak     -= 8;
        wk     -= 8;
    }
} // PARSE_INTERNAL_IMPL

#undef PARSE_IMPL
#undef PARSE_INTERNAL_IMPL
#undef MV_SRC
#undef MV_DST

