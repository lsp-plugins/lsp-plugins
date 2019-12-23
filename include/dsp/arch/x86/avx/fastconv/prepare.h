/*
 * prepare.h
 *
 *  Created on: 13 дек. 2019 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_AVX_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_AVX_IMPL */

#define FASTCONV_DIRECT_PREPARE_BODY(FMA_SEL) \
        size_t off; \
        \
        ARCH_X86_ASM( \
            __ASM_EMIT("lea                 (,%[np], 8), %[off]") \
            __ASM_EMIT("vmovups             0x00(%[ak]), %%ymm6")               /* ymm6 = x_re */ \
            __ASM_EMIT("vmovups             0x20(%[ak]), %%ymm7")               /* ymm7 = x_im */ \
            __ASM_EMIT("vmovups             0x00(%[wk]), %%ymm4")               /* ymm4 = w_re */ \
            __ASM_EMIT("vmovups             0x20(%[wk]), %%ymm5")               /* ymm5 = w_im */ \
            __ASM_EMIT("vxorps              %%ymm1, %%ymm1, %%ymm1")            /* ymm1 = a_im = 0 */ \
            /* x8 blocks */ \
            __ASM_EMIT32("subl              $8, %[np]") \
            __ASM_EMIT64("sub               $8, %[np]") \
            __ASM_EMIT64("jb                2f") \
            __ASM_EMIT("1:") \
            __ASM_EMIT("vmovups             0x00(%[src]), %%ymm0")              /* ymm0 = a_re = re */ \
            __ASM_EMIT("vmulps              %%ymm0, %%ymm7, %%ymm3")            /* ymm3 = x_im * re */ \
            __ASM_EMIT("vmulps              %%ymm0, %%ymm6, %%ymm2")            /* ymm2 = b_re = x_re * re */ \
            __ASM_EMIT("vsubps              %%ymm3, %%ymm1, %%ymm3")            /* ymm3 = b_im = -x_im * re */ \
            __ASM_EMIT("vmovups             %%ymm0, 0x00(%[dst])") \
            __ASM_EMIT("vmovups             %%ymm1, 0x20(%[dst])") \
            __ASM_EMIT("vmovups             %%ymm2, 0x00(%[dst], %[off])") \
            __ASM_EMIT("vmovups             %%ymm3, 0x20(%[dst], %[off])") \
            __ASM_EMIT("add                 $0x20, %[src]") \
            __ASM_EMIT("add                 $0x40, %[dst]") \
            __ASM_EMIT32("subl              $8, %[np]") \
            __ASM_EMIT64("sub               $8, %[np]") \
            __ASM_EMIT("jb                  2f") \
            /* Rotate angle */ \
            __ASM_EMIT("vmulps              %%ymm5, %%ymm6, %%ymm2")            /* ymm2 = w_im * x_re */ \
            __ASM_EMIT("vmulps              %%ymm5, %%ymm7, %%ymm3")            /* ymm3 = w_im * x_im */ \
            __ASM_EMIT(FMA_SEL("vmulps      %%ymm4, %%ymm6, %%ymm6", ""))       /* ymm6 = w_re * x_re */ \
            __ASM_EMIT(FMA_SEL("vmulps      %%ymm4, %%ymm7, %%ymm7", ""))       /* ymm7 = w_re * x_im */ \
            __ASM_EMIT(FMA_SEL("vsubps      %%ymm3, %%ymm6, %%ymm6", "vfmsub132ps %%ymm4, %%ymm3, %%ymm6")) /* ymm6 = x_re' = w_re * x_re - w_im * x_im */ \
            __ASM_EMIT(FMA_SEL("vaddps      %%ymm2, %%ymm7, %%ymm7", "vfmadd132ps %%ymm4, %%ymm2, %%ymm7")) /* ymm7 = x_im' = w_re * x_im + w_im * x_re */ \
            __ASM_EMIT("jmp                 1b") \
            __ASM_EMIT("2:") \
            : [dst] "+r" (dst), [src] "+r" (src), \
              [off] "=&r" (off), [np] __ASM_ARG_RW(np) \
            : [ak] "r" (ak), [wk] "r" (wk) \
            : "%xmm0", "%xmm1", "%xmm2", "%xmm3", \
              "%xmm4", "%xmm5", "%xmm6", "%xmm7" \
        )

#define FASTCONV_REVERSE_PREPARE_BODY(FMA_SEL) \
        ARCH_X86_ASM( \
            /* 2x blocks of 4x-butterfly loop */ \
            __ASM_EMIT("sub             $2, %[nb]") \
            __ASM_EMIT("jb              2f") \
            __ASM_EMIT("1:") \
                __ASM_EMIT("vmovups         0x00(%[dst]), %%xmm0")                  /* xmm0 = r0  r1  r2  r3 */ \
                __ASM_EMIT("vmovups         0x10(%[dst]), %%xmm4")                  /* xmm4 = r4  r5  r6  r7 */ \
                __ASM_EMIT("vmovups         0x20(%[dst]), %%xmm2")                  /* xmm2 = i0  i1  i2  i3 */ \
                __ASM_EMIT("vmovups         0x30(%[dst]), %%xmm6")                  /* xmm6 = i4  i5  i6  i7 */ \
                __ASM_EMIT("vinsertf128     $1, 0x40(%[dst]), %%ymm0, %%ymm0")      /* ymm0 = r0  r1  r2  r3  r8  r9  r10 r11   */ \
                __ASM_EMIT("vinsertf128     $1, 0x50(%[dst]), %%ymm4, %%ymm4")      /* ymm4 = r4  r5  r6  r7  r12 r13 r14 r15   */ \
                __ASM_EMIT("vinsertf128     $1, 0x60(%[dst]), %%ymm2, %%ymm2")      /* ymm2 = i0  i1  i2  i3  i8  i9  i10 i11   */ \
                __ASM_EMIT("vinsertf128     $1, 0x70(%[dst]), %%ymm6, %%ymm6")      /* ymm6 = i4  i5  i6  i7  i12 i13 i14 i15   */ \
                /* 1st-order 4x butterfly */ \
                __ASM_EMIT("vhsubps         %%ymm4, %%ymm0, %%ymm1")                /* ymm1 = r0-r1 r2-r3 r4-r5 r6-r7 = r1' r3' r5' r7' */ \
                __ASM_EMIT("vhsubps         %%ymm6, %%ymm2, %%ymm3")                /* ymm3 = i0-i1 i2-i3 i4-i5 i6-i7 = i1' i3' i5' i7' */ \
                __ASM_EMIT("vhaddps         %%ymm4, %%ymm0, %%ymm0")                /* ymm0 = r0+r1 r2+r3 r4+r5 r6+r7 = r0' r2' r4' r6' */ \
                __ASM_EMIT("vhaddps         %%ymm6, %%ymm2, %%ymm2")                /* ymm2 = i0+i1 i2+i3 i4+i5 i6+i7 = i0' i2' i4' i6' */ \
                /* 2nd-order 4x butterfly */ \
                __ASM_EMIT("vblendps        $0xaa, %%ymm3, %%ymm1, %%ymm4")         /* ymm4 = r1' i3' r5' i7' */ \
                __ASM_EMIT("vblendps        $0xaa, %%ymm1, %%ymm3, %%ymm5")         /* ymm5 = i1' r3' i5' r7' */ \
                __ASM_EMIT("vhsubps         %%ymm4, %%ymm0, %%ymm1")                /* ymm1 = r0'-r2' r4'-r6' r1'-i3' r5'-i7' = r2" r6" r1" r5" */ \
                __ASM_EMIT("vhsubps         %%ymm5, %%ymm2, %%ymm3")                /* ymm3 = i0'-i2' i4'-i6' i1'-r3' i5'-r7' = i2" i6" i3" i7" */ \
                __ASM_EMIT("vhaddps         %%ymm4, %%ymm0, %%ymm0")                /* ymm0 = r0'+r2' r4'+r6' r1'+i3' r5'+i7' = r0" r4" r3" r7" */ \
                __ASM_EMIT("vhaddps         %%ymm5, %%ymm2, %%ymm2")                /* ymm2 = i0'+i2' i4'+i6' i1'+r3' i5'+r7' = i0" i4" i1" i5" */ \
                __ASM_EMIT("vblendps        $0xcc, %%ymm1, %%ymm0, %%ymm4")         /* ymm4 = r0" i4" r1" r5" */ \
                __ASM_EMIT("vblendps        $0xcc, %%ymm0, %%ymm1, %%ymm5")         /* ymm5 = r2" r6" r3" r7" */ \
                __ASM_EMIT("vshufps         $0x88, %%ymm3, %%ymm2, %%ymm6")         /* ymm6 = i0" i1" i2" i3" */ \
                __ASM_EMIT("vshufps         $0xdd, %%ymm3, %%ymm2, %%ymm7")         /* ymm7 = i4" i5" i6" i7" */ \
                __ASM_EMIT("vshufps         $0x88, %%ymm5, %%ymm4, %%ymm2")         /* ymm2 = r0" r1" r2" r3" */ \
                __ASM_EMIT("vshufps         $0xdd, %%ymm5, %%ymm4, %%ymm3")         /* ymm3 = r4" r5" r6" r7" */ \
                /* 3rd-order 8x butterfly */ \
                __ASM_EMIT("vmulps          0x20 + %[FFT_A], %%ymm3, %%ymm4")       /* ymm4 = x_im * b_re */ \
                __ASM_EMIT("vmulps          0x20 + %[FFT_A], %%ymm7, %%ymm5")       /* ymm5 = x_im * b_im */ \
                __ASM_EMIT(FMA_SEL("vmulps  0x00 + %[FFT_A], %%ymm3, %%ymm3", ""))  /* ymm3 = x_re * b_re */ \
                __ASM_EMIT(FMA_SEL("vmulps  0x00 + %[FFT_A], %%ymm7, %%ymm7", ""))  /* ymm7 = x_re * b_im */ \
                __ASM_EMIT(FMA_SEL("vsubps  %%ymm5, %%ymm3, %%ymm5", "vfmsub231ps  0x00 + %[FFT_A], %%ymm3, %%ymm5"))       /* ymm5 = c_re = x_re * b_re - x_im * b_im */ \
                __ASM_EMIT(FMA_SEL("vaddps  %%ymm4, %%ymm7, %%ymm4", "vfmadd231ps  0x00 + %[FFT_A], %%ymm7, %%ymm4"))       /* ymm4 = c_im = x_re * b_im + x_im * b_re */ \
                __ASM_EMIT("vsubps          %%ymm5, %%ymm2, %%ymm0")                /* ymm0 = a_re - c_re */ \
                __ASM_EMIT("vsubps          %%ymm4, %%ymm6, %%ymm1")                /* ymm1 = a_im - c_im */ \
                __ASM_EMIT("vaddps          %%ymm5, %%ymm2, %%ymm2")                /* ymm2 = a_re + c_re */ \
                __ASM_EMIT("vaddps          %%ymm4, %%ymm6, %%ymm3")                /* ymm3 = a_im + c_im */ \
                /* Store */ \
                __ASM_EMIT("vmovups         %%xmm2, 0x00(%[dst])") \
                __ASM_EMIT("vmovups         %%xmm0, 0x10(%[dst])") \
                __ASM_EMIT("vmovups         %%xmm3, 0x20(%[dst])") \
                __ASM_EMIT("vmovups         %%xmm1, 0x30(%[dst])") \
                __ASM_EMIT("vextractf128    $1, %%ymm2, 0x40(%[dst])") \
                __ASM_EMIT("vextractf128    $1, %%ymm0, 0x50(%[dst])") \
                __ASM_EMIT("vextractf128    $1, %%ymm3, 0x60(%[dst])") \
                __ASM_EMIT("vextractf128    $1, %%ymm1, 0x70(%[dst])") \
            __ASM_EMIT("add             $0x80, %[dst]") \
            __ASM_EMIT("sub             $2, %[nb]") \
            __ASM_EMIT("jae             1b") \
            /* 1x block of 4-butterfly */ \
            __ASM_EMIT("2:") \
            __ASM_EMIT("add             $1, %[nb]") \
            __ASM_EMIT("jl              4f") \
                __ASM_EMIT("vmovups         0x00(%[dst]), %%xmm0")                  /* xmm0 = r0  r1  r2  r3 */ \
                __ASM_EMIT("vmovups         0x10(%[dst]), %%xmm4")                  /* xmm4 = r4  r5  r6  r7 */ \
                __ASM_EMIT("vmovups         0x20(%[dst]), %%xmm2")                  /* xmm2 = i0  i1  i2  i3 */ \
                __ASM_EMIT("vmovups         0x30(%[dst]), %%xmm6")                  /* xmm6 = i4  i5  i6  i7 */ \
                /* 1st-order 4x butterfly */ \
                __ASM_EMIT("vhsubps         %%xmm4, %%xmm0, %%xmm1")                /* xmm1 = r0-r1 r2-r3 r4-r5 r6-r7 = r1' r3' r5' r7' */ \
                __ASM_EMIT("vhsubps         %%xmm6, %%xmm2, %%xmm3")                /* xmm3 = i0-i1 i2-i3 i4-i5 i6-i7 = i1' i3' i5' i7' */ \
                __ASM_EMIT("vhaddps         %%xmm4, %%xmm0, %%xmm0")                /* xmm0 = r0+r1 r2+r3 r4+r5 r6+r7 = r0' r2' r4' r6' */ \
                __ASM_EMIT("vhaddps         %%xmm6, %%xmm2, %%xmm2")                /* xmm2 = i0+i1 i2+i3 i4+i5 i6+i7 = i0' i2' i4' i6' */ \
                /* 2nd-order 4x butterfly */ \
                __ASM_EMIT("vblendps        $0xaa, %%xmm3, %%xmm1, %%xmm4")         /* xmm4 = r1' i3' r5' i7' */ \
                __ASM_EMIT("vblendps        $0xaa, %%xmm1, %%xmm3, %%xmm5")         /* xmm5 = i1' r3' i5' r7' */ \
                __ASM_EMIT("vhsubps         %%xmm4, %%xmm0, %%xmm1")                /* xmm1 = r0'-r2' r4'-r6' r1'-i3' r5'-i7' = r2" r6" r1" r5" */ \
                __ASM_EMIT("vhsubps         %%xmm5, %%xmm2, %%xmm3")                /* xmm3 = i0'-i2' i4'-i6' i1'-r3' i5'-r7' = i2" i6" i3" i7" */ \
                __ASM_EMIT("vhaddps         %%xmm4, %%xmm0, %%xmm0")                /* xmm0 = r0'+r2' r4'+r6' r1'+i3' r5'+i7' = r0" r4" r3" r7" */ \
                __ASM_EMIT("vhaddps         %%xmm5, %%xmm2, %%xmm2")                /* xmm2 = i0'+i2' i4'+i6' i1'+r3' i5'+r7' = i0" i4" i1" i5" */ \
                __ASM_EMIT("vblendps        $0xcc, %%xmm1, %%xmm0, %%xmm4")         /* xmm4 = r0" i4" r1" r5" */ \
                __ASM_EMIT("vblendps        $0xcc, %%xmm0, %%xmm1, %%xmm5")         /* xmm5 = r2" r6" r3" r7" */ \
                __ASM_EMIT("vshufps         $0x88, %%xmm3, %%xmm2, %%xmm6")         /* xmm6 = i0" i1" i2" i3" */ \
                __ASM_EMIT("vshufps         $0xdd, %%xmm3, %%xmm2, %%xmm7")         /* xmm7 = i4" i5" i6" i7" */ \
                __ASM_EMIT("vshufps         $0x88, %%xmm5, %%xmm4, %%xmm2")         /* xmm2 = r0" r1" r2" r3" */ \
                __ASM_EMIT("vshufps         $0xdd, %%xmm5, %%xmm4, %%xmm3")         /* xmm3 = r4" r5" r6" r7" */ \
                /* 3rd-order 8x butterfly */ \
                __ASM_EMIT("vmulps          0x20 + %[FFT_A], %%xmm3, %%xmm4")       /* xmm4 = x_im * b_re */ \
                __ASM_EMIT("vmulps          0x20 + %[FFT_A], %%xmm7, %%xmm5")       /* xmm5 = x_im * b_im */ \
                __ASM_EMIT(FMA_SEL("vmulps  0x00 + %[FFT_A], %%xmm3, %%xmm3", ""))  /* xmm3 = x_re * b_re */ \
                __ASM_EMIT(FMA_SEL("vmulps  0x00 + %[FFT_A], %%xmm7, %%xmm7", ""))  /* xmm7 = x_re * b_im */ \
                __ASM_EMIT(FMA_SEL("vsubps  %%xmm5, %%xmm3, %%xmm5", "vfmsub231ps  0x00 + %[FFT_A], %%xmm3, %%xmm5"))       /* xmm5 = c_re = x_re * b_re - x_im * b_im */ \
                __ASM_EMIT(FMA_SEL("vaddps  %%xmm4, %%xmm7, %%xmm4", "vfmadd231ps  0x00 + %[FFT_A], %%xmm7, %%xmm4"))       /* xmm4 = c_im = x_re * b_im + x_im * b_re */ \
                __ASM_EMIT("vsubps          %%xmm5, %%xmm2, %%xmm0")                /* xmm0 = a_re - c_re */ \
                __ASM_EMIT("vsubps          %%xmm4, %%xmm6, %%xmm1")                /* xmm1 = a_im - c_im */ \
                __ASM_EMIT("vaddps          %%xmm5, %%xmm2, %%xmm2")                /* xmm2 = a_re + c_re */ \
                __ASM_EMIT("vaddps          %%xmm4, %%xmm6, %%xmm3")                /* xmm3 = a_im + c_im */ \
                /* Store */ \
                __ASM_EMIT("vmovups         %%xmm2, 0x00(%[dst])") \
                __ASM_EMIT("vmovups         %%xmm0, 0x10(%[dst])") \
                __ASM_EMIT("vmovups         %%xmm3, 0x20(%[dst])") \
                __ASM_EMIT("vmovups         %%xmm1, 0x30(%[dst])") \
            __ASM_EMIT("4:")  \
            : [dst] "+r" (dst), [nb] "+r" (nb) \
            : [FFT_A] "o" (FFT_A) \
            : "cc", "memory", \
              "%xmm0", "%xmm1", "%xmm2", "%xmm3", \
              "%xmm4", "%xmm5", "%xmm6", "%xmm7" \
        )

namespace avx
{
#define FMA_OFF(a, b)       a
#define FMA_ON(a, b)        b

    static inline void fastconv_direct_prepare(float *dst, const float *src, const float *ak, const float *wk, size_t np)
    {
        FASTCONV_DIRECT_PREPARE_BODY(FMA_OFF);
    }

    static inline void fastconv_reverse_prepare(float *dst, size_t nb)
    {
        FASTCONV_REVERSE_PREPARE_BODY(FMA_OFF);
    }

    static inline void fastconv_direct_prepare_fma3(float *dst, const float *src, const float *ak, const float *wk, size_t np)
    {
        FASTCONV_DIRECT_PREPARE_BODY(FMA_OFF);
    }

    static inline void fastconv_reverse_prepare_fma3(float *dst, size_t nb)
    {
        FASTCONV_REVERSE_PREPARE_BODY(FMA_OFF);
    }

    static inline void fastconv_direct_unpack(float *dst, const float *src)
    {
        ARCH_X86_ASM(
            __ASM_EMIT("vmovups         (%[src]), %%xmm0")
            __ASM_EMIT("vxorps          %%ymm1, %%ymm1, %%ymm1")
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
            __ASM_EMIT("vmovups         %%ymm1, 0x20(%[dst])")
            :
            : [dst] "r" (dst), [src] "r" (src)
            : "%xmm0", "%xmm1"
        );
    }

    static inline void fastconv_reverse_unpack(float *dst, const float *src, size_t rank)
    {
        size_t blocks = 1 << rank;
        float norm = 1.0f / float(blocks);

        // Perform 4-element butterflies
        ARCH_X86_ASM
        (
            __ASM_EMIT("vbroadcastss    %[norm], %%ymm0")
            // 16x blocks
            __ASM_EMIT("sub             $16, %[blocks]")
            __ASM_EMIT("jb              2f")
            __ASM_EMIT("vmovaps         %%ymm0, %%ymm1")
            __ASM_EMIT("1:")
            __ASM_EMIT("vmulps          0x00(%[src]), %%ymm0, %%ymm2")  /* ymm2 = r0  r1  r2  r3  r4  r5  r6  r7  */
            __ASM_EMIT("vmulps          0x40(%[src]), %%ymm1, %%ymm3")  /* ymm3 = r8  r9  r10 r11 r12 r13 r14 r15 */
            __ASM_EMIT("vmovups         %%ymm2, 0x00(%[dst])")
            __ASM_EMIT("vmovups         %%ymm3, 0x20(%[dst])")
            __ASM_EMIT("add             $0x80, %[dst]")
            __ASM_EMIT("add             $0x40, %[src]")
            __ASM_EMIT("sub             $16, %[blocks]")
            __ASM_EMIT("jae             1b")
            __ASM_EMIT("2:")
            // 8x block
            __ASM_EMIT("add             $8, %[blocks]")
            __ASM_EMIT("jl              4f")
            __ASM_EMIT("vmulps          0x00(%[src]), %%ymm0, %%ymm2")  /* ymm2 = r0  r1  r2  r3  r4  r5  r6  r7  */
            __ASM_EMIT("vmovups         %%ymm2, 0x00(%[dst])")
            __ASM_EMIT("4:")

            : [dst] "+r"(dst), [src] "+r" (src), [blocks] "+r" (blocks)
            : [norm] "o" (norm)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    static inline void fastconv_reverse_unpack_adding(float *dst, const float *src, size_t rank)
    {
        size_t blocks = 1 << rank;
        float norm = 1.0f / float(blocks);

        // Perform 4-element butterflies
        ARCH_X86_ASM
        (
            __ASM_EMIT("vbroadcastss    %[norm], %%ymm0")
            // 16x blocks
            __ASM_EMIT("sub             $16, %[blocks]")
            __ASM_EMIT("jb              2f")
            __ASM_EMIT("vmovaps         %%ymm0, %%ymm1")
            __ASM_EMIT("1:")
            __ASM_EMIT("vmulps          0x00(%[src]), %%ymm0, %%ymm2")  /* ymm2 = r0  r1  r2  r3  r4  r5  r6  r7  */
            __ASM_EMIT("vmulps          0x40(%[src]), %%ymm1, %%ymm3")  /* ymm3 = r8  r9  r10 r11 r12 r13 r14 r15 */
            __ASM_EMIT("vaddps          0x00(%[dst]), %%ymm2, %%ymm2")
            __ASM_EMIT("vaddps          0x20(%[dst]), %%ymm3, %%ymm3")
            __ASM_EMIT("vmovups         %%ymm2, 0x00(%[dst])")
            __ASM_EMIT("vmovups         %%ymm3, 0x20(%[dst])")
            __ASM_EMIT("add             $0x80, %[dst]")
            __ASM_EMIT("add             $0x40, %[src]")
            __ASM_EMIT("sub             $16, %[blocks]")
            __ASM_EMIT("jae             1b")
            __ASM_EMIT("2:")
            // 8x block
            __ASM_EMIT("add             $8, %[blocks]")
            __ASM_EMIT("jl              4f")
            __ASM_EMIT("vmulps          0x00(%[src]), %%ymm0, %%ymm2")  /* ymm2 = r0  r1  r2  r3  r4  r5  r6  r7  */
            __ASM_EMIT("vaddps          0x00(%[dst]), %%ymm2, %%ymm2")
            __ASM_EMIT("vmovups         %%ymm2, 0x00(%[dst])")
            __ASM_EMIT("4:")

            : [dst] "+r"(dst), [src] "+r" (src), [blocks] "+r" (blocks)
            : [norm] "o" (norm)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

#undef FASTCONV_DIRECT_PREPARE_BODY
#undef FASTCONV_REVERSE_PREPARE_BODY
#undef FMA_OFF
#undef FMA_ON
}


