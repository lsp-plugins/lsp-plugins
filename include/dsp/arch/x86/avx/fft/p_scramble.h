/*
 * p_scramble.h
 *
 *  Created on: 11 дек. 2019 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_AVX_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_AVX_IMPL */

namespace avx
{
    static inline void FFT_PSCRAMBLE_SELF_DIRECT_NAME(float *dst, size_t rank)
    {
        // Calculate number of items
        size_t items    = (1 << rank) - 1;

        for (size_t i = 1; i < items; ++i)
        {
            size_t j = reverse_bits(FFT_TYPE(i), rank);    /* Reverse the order of the bits */
            if (i >= j)
                continue;

            /* Copy the values from the reversed position */
            /* Copy the values from the reversed position */
            ARCH_X86_ASM
            (
                __ASM_EMIT("vmovlps     (%[dst], %[i], 8), %%xmm0, %%xmm0")
                __ASM_EMIT("vmovlps     (%[dst], %[j], 8), %%xmm1, %%xmm1")
                __ASM_EMIT("vmovlps     %%xmm0, (%[dst], %[j], 8)")
                __ASM_EMIT("vmovlps     %%xmm1, (%[dst], %[i], 8)")
                :
                : [dst] "r"(dst), [i] "r"(i), [j] "r"(j)
                : "memory",
                  "%xmm0", "%xmm1"
            );
        }

        // Perform butterfly 8x
        items = 1 << (rank - 3);

        // Perform 4-element butterflies
        ARCH_X86_ASM
        (
            /* Loop 2x 4-element butterflies */
            __ASM_EMIT("sub             $2, %[items]")
            __ASM_EMIT("jb              2f")
            __ASM_EMIT("1:")
                /* Load data to registers */
                __ASM_EMIT("vmovups         0x00(%[dst]), %%xmm0")                  /* xmm0 = r0  i0  r1  i1 */
                __ASM_EMIT("vmovups         0x10(%[dst]), %%xmm1")                  /* xmm1 = r2  i2  r3  i3 */
                __ASM_EMIT("vmovups         0x20(%[dst]), %%xmm4")                  /* xmm4 = r4  i4  r5  i5 */
                __ASM_EMIT("vmovups         0x30(%[dst]), %%xmm5")                  /* xmm5 = r6  i6  r7  i7 */
                __ASM_EMIT("vinsertf128     $1, 0x40(%[dst]), %%ymm0, %%ymm0")      /* ymm0 = r0  i0  r1  i1  r8  i8  r9  i9    */
                __ASM_EMIT("vinsertf128     $1, 0x50(%[dst]), %%ymm1, %%ymm1")      /* ymm1 = r2  i2  r3  i3  r10 i10 r11 i11   */
                __ASM_EMIT("vinsertf128     $1, 0x60(%[dst]), %%ymm4, %%ymm4")      /* ymm4 = r4  i4  r5  i5  r12 i12 r13 i13   */
                __ASM_EMIT("vinsertf128     $1, 0x70(%[dst]), %%ymm5, %%ymm5")      /* ymm5 = r6  i6  r7  i7  r14 i14 r15 i15   */
                __ASM_EMIT("vshufps         $0xdd, %%ymm1, %%ymm0, %%ymm2")         /* ymm2 = i0  i1  i2  i3  i8  i9  i10 i11   */
                __ASM_EMIT("vshufps         $0x88, %%ymm1, %%ymm0, %%ymm0")         /* ymm0 = r0  r1  r2  r3  r8  r9  r10 r11   */
                __ASM_EMIT("vshufps         $0xdd, %%ymm5, %%ymm4, %%ymm6")         /* ymm6 = i4  i5  i6  i7  i12 i13 i14 i15   */
                __ASM_EMIT("vshufps         $0x88, %%ymm5, %%ymm4, %%ymm4")         /* ymm4 = r4  r5  r6  r7  r12 r13 r14 r15   */
                /* 1st-order 4x butterfly */
                __ASM_EMIT("vhsubps         %%ymm4, %%ymm0, %%ymm1")                /* ymm1 = r0-r1 r2-r3 r4-r5 r6-r7 = r1' r3' r5' r7' */
                __ASM_EMIT("vhsubps         %%ymm6, %%ymm2, %%ymm3")                /* ymm3 = i0-i1 i2-i3 i4-i5 i6-i7 = i1' i3' i5' i7' */
                __ASM_EMIT("vhaddps         %%ymm4, %%ymm0, %%ymm0")                /* ymm0 = r0+r1 r2+r3 r4+r5 r6+r7 = r0' r2' r4' r6' */
                __ASM_EMIT("vhaddps         %%ymm6, %%ymm2, %%ymm2")                /* ymm2 = i0+i1 i2+i3 i4+i5 i6+i7 = i0' i2' i4' i6' */
                /* 2nd-order 4x butterfly */
                __ASM_EMIT("vblendps        $0xaa, %%ymm3, %%ymm1, %%ymm4")         /* ymm4 = r1' i3' r5' i7' */
                __ASM_EMIT("vblendps        $0xaa, %%ymm1, %%ymm3, %%ymm5")         /* ymm5 = i1' r3' i5' r7' */
                __ASM_EMIT("vhsubps         %%ymm4, %%ymm0, %%ymm1")                /* ymm1 = r0'-r2' r4'-r6' r1'-i3' r5'-i7' = r2" r6" r3" r7" */
                __ASM_EMIT("vhsubps         %%ymm5, %%ymm2, %%ymm3")                /* ymm3 = i0'-i2' i4'-i6' i1'-r3' i5'-r7' = i2" i6" i1" i5" */
                __ASM_EMIT("vhaddps         %%ymm4, %%ymm0, %%ymm0")                /* ymm0 = r0'+r2' r4'+r6' r1'+i3' r5'+i7' = r0" r4" r1" r5" */
                __ASM_EMIT("vhaddps         %%ymm5, %%ymm2, %%ymm2")                /* ymm2 = i0'+i2' i4'+i6' i1'+r3' i5'+r7' = i0" i4" i3" i7" */
                __ASM_EMIT("vblendps        $0xcc, %%ymm3, %%ymm2, %%ymm4")         /* ymm4 = i0" i4" i1" i5" */
                __ASM_EMIT("vblendps        $0xcc, %%ymm2, %%ymm3, %%ymm5")         /* ymm5 = i2" i6" i3" i7" */
                __ASM_EMIT("vshufps         $0x88, %%ymm1, %%ymm0, %%ymm2")         /* ymm2 = r0" r1" r2" r3" */
                __ASM_EMIT("vshufps         $0xdd, %%ymm1, %%ymm0, %%ymm3")         /* ymm3 = r4" r5" r6" r7" */
                __ASM_EMIT("vshufps         $0x88, %%ymm5, %%ymm4, %%ymm6")         /* ymm6 = i0" i1" i2" i3" */
                __ASM_EMIT("vshufps         $0xdd, %%ymm5, %%ymm4, %%ymm7")         /* ymm7 = i4" i5" i6" i7" */
                /* 3rd-order 8x butterfly */
                __ASM_EMIT("vmulps          0x20 + %[FFT_A], %%ymm3, %%ymm4")       /* ymm4 = x_im * b_re */ \
                __ASM_EMIT("vmulps          0x20 + %[FFT_A], %%ymm7, %%ymm5")       /* ymm5 = x_im * b_im */ \
                __ASM_EMIT(FFT_FMA("vmulps  0x00 + %[FFT_A], %%ymm3, %%ymm3", ""))  /* ymm3 = x_re * b_re */ \
                __ASM_EMIT(FFT_FMA("vmulps  0x00 + %[FFT_A], %%ymm7, %%ymm7", ""))  /* ymm7 = x_re * b_im */ \
                __ASM_EMIT(FFT_FMA("vaddps  %%ymm5, %%ymm3, %%ymm5", "vfmadd231ps  0x00 + %[FFT_A], %%ymm3, %%ymm5"))       /* ymm5 = c_re = x_re * b_re + x_im * b_im */ \
                __ASM_EMIT(FFT_FMA("vsubps  %%ymm4, %%ymm7, %%ymm4", "vfmsub231ps  0x00 + %[FFT_A], %%ymm7, %%ymm4"))       /* ymm4 = c_im = x_re * b_im - x_im * b_re */ \
                __ASM_EMIT("vsubps          %%ymm5, %%ymm2, %%ymm0")                /* ymm0 = a_re - c_re */ \
                __ASM_EMIT("vsubps          %%ymm4, %%ymm6, %%ymm1")                /* ymm1 = a_im - c_im */ \
                __ASM_EMIT("vaddps          %%ymm5, %%ymm2, %%ymm2")                /* ymm2 = a_re + c_re */ \
                __ASM_EMIT("vaddps          %%ymm4, %%ymm6, %%ymm3")                /* ymm3 = a_im + c_im */ \
                /* Store */
                __ASM_EMIT("vmovups         %%xmm2, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%xmm0, 0x10(%[dst])")
                __ASM_EMIT("vmovups         %%xmm3, 0x20(%[dst])")
                __ASM_EMIT("vmovups         %%xmm1, 0x30(%[dst])")
                __ASM_EMIT("vextractf128    $1, %%ymm2, 0x40(%[dst])")
                __ASM_EMIT("vextractf128    $1, %%ymm0, 0x50(%[dst])")
                __ASM_EMIT("vextractf128    $1, %%ymm3, 0x60(%[dst])")
                __ASM_EMIT("vextractf128    $1, %%ymm1, 0x70(%[dst])")
                /* Move pointers and repeat*/
                __ASM_EMIT("add             $0x80, %[dst]")
                __ASM_EMIT("sub             $2, %[items]")
                __ASM_EMIT("jae             1b")
            __ASM_EMIT("2:")
            /* x4 scramble block */
            __ASM_EMIT("add             $1, %[items]")
            __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovups         0x00(%[dst]), %%xmm0")                  /* xmm0 = r0  i0  r1  i1 */
                __ASM_EMIT("vmovups         0x10(%[dst]), %%xmm1")                  /* xmm1 = r2  i2  r3  i3 */
                __ASM_EMIT("vmovups         0x20(%[dst]), %%xmm4")                  /* xmm4 = r4  i4  r5  i5 */
                __ASM_EMIT("vmovups         0x30(%[dst]), %%xmm5")                  /* xmm5 = r6  i6  r7  i7 */
                __ASM_EMIT("vshufps         $0xdd, %%xmm1, %%xmm0, %%xmm2")         /* xmm2 = i0  i1  i2  i3 */
                __ASM_EMIT("vshufps         $0x88, %%xmm1, %%xmm0, %%xmm0")         /* xmm0 = r0  r1  r2  r3 */
                __ASM_EMIT("vshufps         $0xdd, %%xmm5, %%xmm4, %%xmm6")         /* xmm4 = i4  i5  i6  i7 */
                __ASM_EMIT("vshufps         $0x88, %%xmm5, %%xmm4, %%xmm4")         /* xmm6 = r4  r5  r6  r7 */
                /* 1st-order 4x butterfly */
                __ASM_EMIT("vhsubps         %%xmm4, %%xmm0, %%xmm1")                /* xmm1 = r0-r1 r2-r3 r4-r5 r6-r7 = r1' r3' r5' r7' */
                __ASM_EMIT("vhsubps         %%xmm6, %%xmm2, %%xmm3")                /* xmm3 = i0-i1 i2-i3 i4-i5 i6-i7 = i1' i3' i5' i7' */
                __ASM_EMIT("vhaddps         %%xmm4, %%xmm0, %%xmm0")                /* xmm0 = r0+r1 r2+r3 r4+r5 r6+r7 = r0' r2' r4' r6' */
                __ASM_EMIT("vhaddps         %%xmm6, %%xmm2, %%xmm2")                /* xmm2 = i0+i1 i2+i3 i4+i5 i6+i7 = i0' i2' i4' i6' */
                /* 2nd-order 4x butterfly */
                __ASM_EMIT("vblendps        $0xaa, %%xmm3, %%xmm1, %%xmm4")         /* xmm4 = r1' i3' r5' i7' */
                __ASM_EMIT("vblendps        $0xaa, %%xmm1, %%xmm3, %%xmm5")         /* xmm5 = i1' r3' i5' r7' */
                __ASM_EMIT("vhsubps         %%xmm4, %%xmm0, %%xmm1")                /* xmm1 = r0'-r2' r4'-r6' r1'-i3' r5'-i7' = r2" r6" r3" r7" */
                __ASM_EMIT("vhsubps         %%xmm5, %%xmm2, %%xmm3")                /* xmm3 = i0'-i2' i4'-i6' i1'-r3' i5'-r7' = i2" i6" i1" i5" */
                __ASM_EMIT("vhaddps         %%xmm4, %%xmm0, %%xmm0")                /* xmm0 = r0'+r2' r4'+r6' r1'+i3' r5'+i7' = r0" r4" r1" r5" */
                __ASM_EMIT("vhaddps         %%xmm5, %%xmm2, %%xmm2")                /* xmm2 = i0'+i2' i4'+i6' i1'+r3' i5'+r7' = i0" i4" i3" i7" */
                __ASM_EMIT("vblendps        $0xcc, %%xmm3, %%xmm2, %%xmm4")         /* xmm4 = i0" i4" i1" i5" */
                __ASM_EMIT("vblendps        $0xcc, %%xmm2, %%xmm3, %%xmm5")         /* xmm5 = i2" i6" i3" i7" */
                __ASM_EMIT("vshufps         $0x88, %%xmm1, %%xmm0, %%xmm2")         /* xmm2 = r0" r1" r2" r3" */
                __ASM_EMIT("vshufps         $0xdd, %%xmm1, %%xmm0, %%xmm3")         /* xmm3 = r4" r5" r6" r7" */
                __ASM_EMIT("vshufps         $0x88, %%xmm5, %%xmm4, %%xmm6")         /* xmm6 = i0" i1" i2" i3" */
                __ASM_EMIT("vshufps         $0xdd, %%xmm5, %%xmm4, %%xmm7")         /* xmm7 = i4" i5" i6" i7" */
                /* 3rd-order 8x butterfly */
                __ASM_EMIT("vmulps          0x20 + %[FFT_A], %%xmm3, %%xmm4")       /* xmm4 = x_im * b_re */ \
                __ASM_EMIT("vmulps          0x20 + %[FFT_A], %%xmm7, %%xmm5")       /* xmm5 = x_im * b_im */ \
                __ASM_EMIT(FFT_FMA("vmulps  0x00 + %[FFT_A], %%xmm3, %%xmm3", ""))  /* xmm3 = x_re * b_re */ \
                __ASM_EMIT(FFT_FMA("vmulps  0x00 + %[FFT_A], %%xmm7, %%xmm7", ""))  /* xmm7 = x_re * b_im */ \
                __ASM_EMIT(FFT_FMA("vaddps  %%xmm5, %%xmm3, %%xmm5", "vfmadd231ps 0x00 + %[FFT_A], %%xmm3, %%xmm5"))        /* xmm5 = c_re = x_re * b_re + x_im * b_im */ \
                __ASM_EMIT(FFT_FMA("vsubps  %%xmm4, %%xmm7, %%xmm4", "vfmsub231ps 0x00 + %[FFT_A], %%xmm7, %%xmm4"))        /* xmm4 = c_im = x_re * b_im - x_im * b_re */ \
                __ASM_EMIT("vsubps          %%xmm5, %%xmm2, %%xmm0")                /* xmm0 = a_re - c_re */ \
                __ASM_EMIT("vsubps          %%xmm4, %%xmm6, %%xmm1")                /* xmm1 = a_im - c_im */ \
                __ASM_EMIT("vaddps          %%xmm5, %%xmm2, %%xmm2")                /* xmm2 = a_re + c_re */ \
                __ASM_EMIT("vaddps          %%xmm4, %%xmm6, %%xmm3")                /* xmm3 = a_im + c_im */ \
                /* Store */
                __ASM_EMIT("vmovups         %%xmm2, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%xmm0, 0x10(%[dst])")
                __ASM_EMIT("vmovups         %%xmm3, 0x20(%[dst])")
                __ASM_EMIT("vmovups         %%xmm1, 0x30(%[dst])")
            __ASM_EMIT("4:")

            : [dst] "+r" (dst), [items] "+r" (items)
            : [FFT_A] "o" (FFT_A)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    static inline void FFT_PSCRAMBLE_SELF_REVERSE_NAME(float *dst, size_t rank)
    {
        // Calculate number of items
        size_t items    = (1 << rank) - 1;

        for (size_t i = 1; i < items; ++i)
        {
            size_t j = reverse_bits(FFT_TYPE(i), rank);    /* Reverse the order of the bits */
            if (i >= j)
                continue;

            /* Copy the values from the reversed position */
            /* Copy the values from the reversed position */
            ARCH_X86_ASM
            (
                __ASM_EMIT("vmovlps     (%[dst], %[i], 8), %%xmm0, %%xmm0")
                __ASM_EMIT("vmovlps     (%[dst], %[j], 8), %%xmm1, %%xmm1")
                __ASM_EMIT("vmovlps     %%xmm0, (%[dst], %[j], 8)")
                __ASM_EMIT("vmovlps     %%xmm1, (%[dst], %[i], 8)")
                :
                : [dst] "r"(dst), [i] "r"(i), [j] "r"(j)
                : "memory",
                  "%xmm0", "%xmm1"
            );
        }

        // Perform butterfly 8x
        items = 1 << (rank - 3);

        // Perform 4-element butterflies
        ARCH_X86_ASM
        (
            /* Loop 2x 4-element butterflies */
            __ASM_EMIT("sub             $2, %[items]")
            __ASM_EMIT("jb              2f")
            __ASM_EMIT("1:")
                /* Load data to registers */
                __ASM_EMIT("vmovups         0x00(%[dst]), %%xmm0")                  /* xmm0 = r0  i0  r1  i1 */
                __ASM_EMIT("vmovups         0x10(%[dst]), %%xmm1")                  /* xmm1 = r2  i2  r3  i3 */
                __ASM_EMIT("vmovups         0x20(%[dst]), %%xmm4")                  /* xmm4 = r4  i4  r5  i5 */
                __ASM_EMIT("vmovups         0x30(%[dst]), %%xmm5")                  /* xmm5 = r6  i6  r7  i7 */
                __ASM_EMIT("vinsertf128     $1, 0x40(%[dst]), %%ymm0, %%ymm0")      /* ymm0 = r0  i0  r1  i1  r8  i8  r9  i9    */
                __ASM_EMIT("vinsertf128     $1, 0x50(%[dst]), %%ymm1, %%ymm1")      /* ymm1 = r2  i2  r3  i3  r10 i10 r11 i11   */
                __ASM_EMIT("vinsertf128     $1, 0x60(%[dst]), %%ymm4, %%ymm4")      /* ymm4 = r4  i4  r5  i5  r12 i12 r13 i13   */
                __ASM_EMIT("vinsertf128     $1, 0x70(%[dst]), %%ymm5, %%ymm5")      /* ymm5 = r6  i6  r7  i7  r14 i14 r15 i15   */
                __ASM_EMIT("vshufps         $0xdd, %%ymm1, %%ymm0, %%ymm2")         /* ymm2 = i0  i1  i2  i3  i8  i9  i10 i11   */
                __ASM_EMIT("vshufps         $0x88, %%ymm1, %%ymm0, %%ymm0")         /* ymm0 = r0  r1  r2  r3  r8  r9  r10 r11   */
                __ASM_EMIT("vshufps         $0xdd, %%ymm5, %%ymm4, %%ymm6")         /* ymm6 = i4  i5  i6  i7  i12 i13 i14 i15   */
                __ASM_EMIT("vshufps         $0x88, %%ymm5, %%ymm4, %%ymm4")         /* ymm4 = r4  r5  r6  r7  r12 r13 r14 r15   */
                /* 1st-order 4x butterfly */
                __ASM_EMIT("vhsubps         %%ymm4, %%ymm0, %%ymm1")                /* ymm1 = r0-r1 r2-r3 r4-r5 r6-r7 = r1' r3' r5' r7' */
                __ASM_EMIT("vhsubps         %%ymm6, %%ymm2, %%ymm3")                /* ymm3 = i0-i1 i2-i3 i4-i5 i6-i7 = i1' i3' i5' i7' */
                __ASM_EMIT("vhaddps         %%ymm4, %%ymm0, %%ymm0")                /* ymm0 = r0+r1 r2+r3 r4+r5 r6+r7 = r0' r2' r4' r6' */
                __ASM_EMIT("vhaddps         %%ymm6, %%ymm2, %%ymm2")                /* ymm2 = i0+i1 i2+i3 i4+i5 i6+i7 = i0' i2' i4' i6' */
                /* 2nd-order 4x butterfly */
                __ASM_EMIT("vblendps        $0xaa, %%ymm3, %%ymm1, %%ymm4")         /* ymm4 = r1' i3' r5' i7' */
                __ASM_EMIT("vblendps        $0xaa, %%ymm1, %%ymm3, %%ymm5")         /* ymm5 = i1' r3' i5' r7' */
                __ASM_EMIT("vhsubps         %%ymm4, %%ymm0, %%ymm1")                /* ymm1 = r0'-r2' r4'-r6' r1'-i3' r5'-i7' = r2" r6" r1" r5" */
                __ASM_EMIT("vhsubps         %%ymm5, %%ymm2, %%ymm3")                /* ymm3 = i0'-i2' i4'-i6' i1'-r3' i5'-r7' = i2" i6" i3" i7" */
                __ASM_EMIT("vhaddps         %%ymm4, %%ymm0, %%ymm0")                /* ymm0 = r0'+r2' r4'+r6' r1'+i3' r5'+i7' = r0" r4" r3" r7" */
                __ASM_EMIT("vhaddps         %%ymm5, %%ymm2, %%ymm2")                /* ymm2 = i0'+i2' i4'+i6' i1'+r3' i5'+r7' = i0" i4" i1" i5" */
                __ASM_EMIT("vblendps        $0xcc, %%ymm1, %%ymm0, %%ymm4")         /* ymm4 = r0" i4" r1" r5" */
                __ASM_EMIT("vblendps        $0xcc, %%ymm0, %%ymm1, %%ymm5")         /* ymm5 = r2" r6" r3" r7" */
                __ASM_EMIT("vshufps         $0x88, %%ymm3, %%ymm2, %%ymm6")         /* ymm6 = i0" i1" i2" i3" */
                __ASM_EMIT("vshufps         $0xdd, %%ymm3, %%ymm2, %%ymm7")         /* ymm7 = i4" i5" i6" i7" */
                __ASM_EMIT("vshufps         $0x88, %%ymm5, %%ymm4, %%ymm2")         /* ymm2 = r0" r1" r2" r3" */
                __ASM_EMIT("vshufps         $0xdd, %%ymm5, %%ymm4, %%ymm3")         /* ymm3 = r4" r5" r6" r7" */
                /* 3rd-order 8x butterfly */
                __ASM_EMIT("vmulps          0x20 + %[FFT_A], %%ymm3, %%ymm4")       /* ymm4 = x_im * b_re */ \
                __ASM_EMIT("vmulps          0x20 + %[FFT_A], %%ymm7, %%ymm5")       /* ymm5 = x_im * b_im */ \
                __ASM_EMIT(FFT_FMA("vmulps  0x00 + %[FFT_A], %%ymm3, %%ymm3", ""))  /* ymm3 = x_re * b_re */ \
                __ASM_EMIT(FFT_FMA("vmulps  0x00 + %[FFT_A], %%ymm7, %%ymm7", ""))  /* ymm7 = x_re * b_im */ \
                __ASM_EMIT(FFT_FMA("vsubps  %%ymm5, %%ymm3, %%ymm5", "vfmsub231ps  0x00 + %[FFT_A], %%ymm3, %%ymm5"))       /* ymm5 = c_re = x_re * b_re - x_im * b_im */ \
                __ASM_EMIT(FFT_FMA("vaddps  %%ymm4, %%ymm7, %%ymm4", "vfmadd231ps  0x00 + %[FFT_A], %%ymm7, %%ymm4"))       /* ymm4 = c_im = x_re * b_im + x_im * b_re */ \
                __ASM_EMIT("vsubps          %%ymm5, %%ymm2, %%ymm0")                /* ymm0 = a_re - c_re */ \
                __ASM_EMIT("vsubps          %%ymm4, %%ymm6, %%ymm1")                /* ymm1 = a_im - c_im */ \
                __ASM_EMIT("vaddps          %%ymm5, %%ymm2, %%ymm2")                /* ymm2 = a_re + c_re */ \
                __ASM_EMIT("vaddps          %%ymm4, %%ymm6, %%ymm3")                /* ymm3 = a_im + c_im */ \
                /* Store */
                __ASM_EMIT("vmovups         %%xmm2, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%xmm0, 0x10(%[dst])")
                __ASM_EMIT("vmovups         %%xmm3, 0x20(%[dst])")
                __ASM_EMIT("vmovups         %%xmm1, 0x30(%[dst])")
                __ASM_EMIT("vextractf128    $1, %%ymm2, 0x40(%[dst])")
                __ASM_EMIT("vextractf128    $1, %%ymm0, 0x50(%[dst])")
                __ASM_EMIT("vextractf128    $1, %%ymm3, 0x60(%[dst])")
                __ASM_EMIT("vextractf128    $1, %%ymm1, 0x70(%[dst])")
                /* Move pointers and repeat*/
                __ASM_EMIT("add             $0x80, %[dst]")
                __ASM_EMIT("sub             $2, %[items]")
                __ASM_EMIT("jae             1b")
            __ASM_EMIT("2:")
            /* x4 scramble block */
            __ASM_EMIT("add             $1, %[items]")
            __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovups         0x00(%[dst]), %%xmm0")                  /* xmm0 = r0  i0  r1  i1 */
                __ASM_EMIT("vmovups         0x10(%[dst]), %%xmm1")                  /* xmm1 = r2  i2  r3  i3 */
                __ASM_EMIT("vmovups         0x20(%[dst]), %%xmm4")                  /* xmm4 = r4  i4  r5  i5 */
                __ASM_EMIT("vmovups         0x30(%[dst]), %%xmm5")                  /* xmm5 = r6  i6  r7  i7 */
                __ASM_EMIT("vshufps         $0xdd, %%xmm1, %%xmm0, %%xmm2")         /* xmm2 = i0  i1  i2  i3 */
                __ASM_EMIT("vshufps         $0x88, %%xmm1, %%xmm0, %%xmm0")         /* xmm0 = r0  r1  r2  r3 */
                __ASM_EMIT("vshufps         $0xdd, %%xmm5, %%xmm4, %%xmm6")         /* xmm4 = i4  i5  i6  i7 */
                __ASM_EMIT("vshufps         $0x88, %%xmm5, %%xmm4, %%xmm4")         /* xmm6 = r4  r5  r6  r7 */
                /* 1st-order 4x butterfly */
                __ASM_EMIT("vhsubps         %%xmm4, %%xmm0, %%xmm1")                /* xmm1 = r0-r1 r2-r3 r4-r5 r6-r7 = r1' r3' r5' r7' */
                __ASM_EMIT("vhsubps         %%xmm6, %%xmm2, %%xmm3")                /* xmm3 = i0-i1 i2-i3 i4-i5 i6-i7 = i1' i3' i5' i7' */
                __ASM_EMIT("vhaddps         %%xmm4, %%xmm0, %%xmm0")                /* xmm0 = r0+r1 r2+r3 r4+r5 r6+r7 = r0' r2' r4' r6' */
                __ASM_EMIT("vhaddps         %%xmm6, %%xmm2, %%xmm2")                /* xmm2 = i0+i1 i2+i3 i4+i5 i6+i7 = i0' i2' i4' i6' */
                /* 2nd-order 4x butterfly */
                __ASM_EMIT("vblendps        $0xaa, %%xmm3, %%xmm1, %%xmm4")         /* xmm4 = r1' i3' r5' i7' */
                __ASM_EMIT("vblendps        $0xaa, %%xmm1, %%xmm3, %%xmm5")         /* xmm5 = i1' r3' i5' r7' */
                __ASM_EMIT("vhsubps         %%xmm4, %%xmm0, %%xmm1")                /* xmm1 = r0'-r2' r4'-r6' r1'-i3' r5'-i7' = r2" r6" r1" r5" */
                __ASM_EMIT("vhsubps         %%xmm5, %%xmm2, %%xmm3")                /* xmm3 = i0'-i2' i4'-i6' i1'-r3' i5'-r7' = i2" i6" i3" i7" */
                __ASM_EMIT("vhaddps         %%xmm4, %%xmm0, %%xmm0")                /* xmm0 = r0'+r2' r4'+r6' r1'+i3' r5'+i7' = r0" r4" r3" r7" */
                __ASM_EMIT("vhaddps         %%xmm5, %%xmm2, %%xmm2")                /* xmm2 = i0'+i2' i4'+i6' i1'+r3' i5'+r7' = i0" i4" i1" i5" */
                __ASM_EMIT("vblendps        $0xcc, %%xmm1, %%xmm0, %%xmm4")         /* xmm4 = r0" i4" r1" r5" */
                __ASM_EMIT("vblendps        $0xcc, %%xmm0, %%xmm1, %%xmm5")         /* xmm5 = r2" r6" r3" r7" */
                __ASM_EMIT("vshufps         $0x88, %%xmm3, %%xmm2, %%xmm6")         /* xmm6 = i0" i1" i2" i3" */
                __ASM_EMIT("vshufps         $0xdd, %%xmm3, %%xmm2, %%xmm7")         /* xmm7 = i4" i5" i6" i7" */
                __ASM_EMIT("vshufps         $0x88, %%xmm5, %%xmm4, %%xmm2")         /* xmm2 = r0" r1" r2" r3" */
                __ASM_EMIT("vshufps         $0xdd, %%xmm5, %%xmm4, %%xmm3")         /* xmm3 = r4" r5" r6" r7" */
                /* 3rd-order 8x butterfly */
                __ASM_EMIT("vmulps          0x20 + %[FFT_A], %%xmm3, %%xmm4")       /* xmm4 = x_im * b_re */ \
                __ASM_EMIT("vmulps          0x20 + %[FFT_A], %%xmm7, %%xmm5")       /* xmm5 = x_im * b_im */ \
                __ASM_EMIT(FFT_FMA("vmulps  0x00 + %[FFT_A], %%xmm3, %%xmm3", ""))  /* xmm3 = x_re * b_re */ \
                __ASM_EMIT(FFT_FMA("vmulps  0x00 + %[FFT_A], %%xmm7, %%xmm7", ""))  /* xmm7 = x_re * b_im */ \
                __ASM_EMIT(FFT_FMA("vsubps  %%xmm5, %%xmm3, %%xmm5", "vfmsub231ps  0x00 + %[FFT_A], %%xmm3, %%xmm5"))       /* xmm5 = c_re = x_re * b_re - x_im * b_im */ \
                __ASM_EMIT(FFT_FMA("vaddps  %%xmm4, %%xmm7, %%xmm4", "vfmadd231ps  0x00 + %[FFT_A], %%xmm7, %%xmm4"))       /* xmm4 = c_im = x_re * b_im + x_im * b_re */ \
                __ASM_EMIT("vsubps          %%xmm5, %%xmm2, %%xmm0")                /* xmm0 = a_re - c_re */ \
                __ASM_EMIT("vsubps          %%xmm4, %%xmm6, %%xmm1")                /* xmm1 = a_im - c_im */ \
                __ASM_EMIT("vaddps          %%xmm5, %%xmm2, %%xmm2")                /* xmm2 = a_re + c_re */ \
                __ASM_EMIT("vaddps          %%xmm4, %%xmm6, %%xmm3")                /* xmm3 = a_im + c_im */ \
                /* Store */
                __ASM_EMIT("vmovups         %%xmm2, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%xmm0, 0x10(%[dst])")
                __ASM_EMIT("vmovups         %%xmm3, 0x20(%[dst])")
                __ASM_EMIT("vmovups         %%xmm1, 0x30(%[dst])")
            __ASM_EMIT("4:")

            : [dst] "+r" (dst), [items] "+r" (items)
            : [FFT_A] "o" (FFT_A)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    static inline void FFT_PSCRAMBLE_COPY_DIRECT_NAME(float *dst, const float *src, size_t rank)
    {
        size_t regs     = 1 << rank;

        for (size_t i=0; i<regs; ++i)
        {
            size_t index    = reverse_bits(FFT_TYPE(i), rank);

            ARCH_X86_ASM
            (
                /* Load scalar values */
                __ASM_EMIT("vmovlps         (%[src], %[index], 8), %%xmm0, %%xmm0")                 /* xmm0 = r0  i0  x   x     */
                __ASM_EMIT("add             %[regs], %[index]")
                __ASM_EMIT("vmovlps         (%[src], %[index], 8), %%xmm2, %%xmm2")                 /* xmm2 = r8  i8  x   x     */
                __ASM_EMIT("add             %[regs], %[index]")
                __ASM_EMIT("vmovlps         (%[src], %[index], 8), %%xmm4, %%xmm4")                 /* xmm4 = r4  i4  x   x     */
                __ASM_EMIT("add             %[regs], %[index]")
                __ASM_EMIT("vmovlps         (%[src], %[index], 8), %%xmm6, %%xmm6")                 /* xmm6 = r12 i12 x   x     */
                __ASM_EMIT("add             %[regs], %[index]")
                __ASM_EMIT("vmovlps         (%[src], %[index], 8), %%xmm1, %%xmm1")                 /* xmm1 = r2  i2  x   x     */
                __ASM_EMIT("add             %[regs], %[index]")
                __ASM_EMIT("vmovlps         (%[src], %[index], 8), %%xmm3, %%xmm3")                 /* xmm3 = r10 i10 x   x     */
                __ASM_EMIT("add             %[regs], %[index]")
                __ASM_EMIT("vmovlps         (%[src], %[index], 8), %%xmm5, %%xmm5")                 /* xmm5 = r6  i6  x   x     */
                __ASM_EMIT("add             %[regs], %[index]")
                __ASM_EMIT("vmovlps         (%[src], %[index], 8), %%xmm7, %%xmm7")                 /* xmm7 = r14 i14 x   x     */
                __ASM_EMIT("add             %[regs], %[index]")

                __ASM_EMIT("vmovhps         (%[src], %[index], 8), %%xmm0, %%xmm0")                 /* xmm0 = r0  i0  r1  i1    */
                __ASM_EMIT("add             %[regs], %[index]")
                __ASM_EMIT("vmovhps         (%[src], %[index], 8), %%xmm2, %%xmm2")                 /* xmm2 = r8  i8  r9  i9    */
                __ASM_EMIT("add             %[regs], %[index]")
                __ASM_EMIT("vmovhps         (%[src], %[index], 8), %%xmm4, %%xmm4")                 /* xmm4 = r4  i4  r5  i5    */
                __ASM_EMIT("add             %[regs], %[index]")
                __ASM_EMIT("vmovhps         (%[src], %[index], 8), %%xmm6, %%xmm6")                 /* xmm6 = r12 i12 r13 r14   */
                __ASM_EMIT("add             %[regs], %[index]")
                __ASM_EMIT("vmovhps         (%[src], %[index], 8), %%xmm1, %%xmm1")                 /* xmm1 = r2  i2  r3  i3    */
                __ASM_EMIT("add             %[regs], %[index]")
                __ASM_EMIT("vmovhps         (%[src], %[index], 8), %%xmm3, %%xmm3")                 /* xmm3 = r10 i10 r11 i11   */
                __ASM_EMIT("add             %[regs], %[index]")
                __ASM_EMIT("vmovhps         (%[src], %[index], 8), %%xmm5, %%xmm5")                 /* xmm5 = r6  i6  r7  i7    */
                __ASM_EMIT("add             %[regs], %[index]")
                __ASM_EMIT("vmovhps         (%[src], %[index], 8), %%xmm7, %%xmm7")                 /* xmm7 = r14 i14 r15 i15   */
                __ASM_EMIT("add             %[regs], %[index]")

                __ASM_EMIT("vinsertf128     $1, %%xmm2, %%ymm0, %%ymm0")                            /* ymm0 = r0  i0  r1  i1  r8  i8  r9  i9  */
                __ASM_EMIT("vinsertf128     $1, %%xmm3, %%ymm1, %%ymm1")                            /* ymm1 = r2  i2  r3  i3  r10 i10 r11 i11 */
                __ASM_EMIT("vinsertf128     $1, %%xmm6, %%ymm4, %%ymm4")                            /* ymm4 = r4  i4  r5  i5  r12 i12 r13 r14 */
                __ASM_EMIT("vinsertf128     $1, %%xmm7, %%ymm5, %%ymm5")                            /* ymm5 = r6  i6  r7  i7  r14 i14 r15 i15 */
                __ASM_EMIT("vshufps         $0xdd, %%ymm1, %%ymm0, %%ymm2")         /* ymm2 = i0  i1  i2  i3  i8  i9  i10 i11   */
                __ASM_EMIT("vshufps         $0x88, %%ymm1, %%ymm0, %%ymm0")         /* ymm0 = r0  r1  r2  r3  r8  r9  r10 r11   */
                __ASM_EMIT("vshufps         $0xdd, %%ymm5, %%ymm4, %%ymm6")         /* ymm6 = i4  i5  i6  i7  i12 i13 i14 i15   */
                __ASM_EMIT("vshufps         $0x88, %%ymm5, %%ymm4, %%ymm4")         /* ymm4 = r4  r5  r6  r7  r12 r13 r14 r15   */
                /* 1st-order 4x butterfly */
                __ASM_EMIT("vhsubps         %%ymm4, %%ymm0, %%ymm1")                /* ymm1 = r0-r1 r2-r3 r4-r5 r6-r7 = r1' r3' r5' r7' */
                __ASM_EMIT("vhsubps         %%ymm6, %%ymm2, %%ymm3")                /* ymm3 = i0-i1 i2-i3 i4-i5 i6-i7 = i1' i3' i5' i7' */
                __ASM_EMIT("vhaddps         %%ymm4, %%ymm0, %%ymm0")                /* ymm0 = r0+r1 r2+r3 r4+r5 r6+r7 = r0' r2' r4' r6' */
                __ASM_EMIT("vhaddps         %%ymm6, %%ymm2, %%ymm2")                /* ymm2 = i0+i1 i2+i3 i4+i5 i6+i7 = i0' i2' i4' i6' */
                /* 2nd-order 4x butterfly */
                __ASM_EMIT("vblendps        $0xaa, %%ymm3, %%ymm1, %%ymm4")         /* ymm4 = r1' i3' r5' i7' */
                __ASM_EMIT("vblendps        $0xaa, %%ymm1, %%ymm3, %%ymm5")         /* ymm5 = i1' r3' i5' r7' */
                __ASM_EMIT("vhsubps         %%ymm4, %%ymm0, %%ymm1")                /* ymm1 = r0'-r2' r4'-r6' r1'-i3' r5'-i7' = r2" r6" r3" r7" */
                __ASM_EMIT("vhsubps         %%ymm5, %%ymm2, %%ymm3")                /* ymm3 = i0'-i2' i4'-i6' i1'-r3' i5'-r7' = i2" i6" i1" i5" */
                __ASM_EMIT("vhaddps         %%ymm4, %%ymm0, %%ymm0")                /* ymm0 = r0'+r2' r4'+r6' r1'+i3' r5'+i7' = r0" r4" r1" r5" */
                __ASM_EMIT("vhaddps         %%ymm5, %%ymm2, %%ymm2")                /* ymm2 = i0'+i2' i4'+i6' i1'+r3' i5'+r7' = i0" i4" i3" i7" */
                __ASM_EMIT("vblendps        $0xcc, %%ymm3, %%ymm2, %%ymm4")         /* ymm4 = i0" i4" i1" i5" */
                __ASM_EMIT("vblendps        $0xcc, %%ymm2, %%ymm3, %%ymm5")         /* ymm5 = i2" i6" i3" i7" */
                __ASM_EMIT("vshufps         $0x88, %%ymm1, %%ymm0, %%ymm2")         /* ymm2 = r0" r1" r2" r3" */
                __ASM_EMIT("vshufps         $0xdd, %%ymm1, %%ymm0, %%ymm3")         /* ymm3 = r4" r5" r6" r7" */
                __ASM_EMIT("vshufps         $0x88, %%ymm5, %%ymm4, %%ymm6")         /* ymm6 = i0" i1" i2" i3" */
                __ASM_EMIT("vshufps         $0xdd, %%ymm5, %%ymm4, %%ymm7")         /* ymm7 = i4" i5" i6" i7" */
                /* 3rd-order 8x butterfly */
                __ASM_EMIT("vmulps          0x20 + %[FFT_A], %%ymm3, %%ymm4")       /* ymm4 = x_im * b_re */ \
                __ASM_EMIT("vmulps          0x20 + %[FFT_A], %%ymm7, %%ymm5")       /* ymm5 = x_im * b_im */ \
                __ASM_EMIT(FFT_FMA("vmulps  0x00 + %[FFT_A], %%ymm3, %%ymm3", ""))  /* ymm3 = x_re * b_re */ \
                __ASM_EMIT(FFT_FMA("vmulps  0x00 + %[FFT_A], %%ymm7, %%ymm7", ""))  /* ymm7 = x_re * b_im */ \
                __ASM_EMIT(FFT_FMA("vaddps  %%ymm5, %%ymm3, %%ymm5", "vfmadd231ps  0x00 + %[FFT_A], %%ymm3, %%ymm5"))       /* ymm5 = c_re = x_re * b_re + x_im * b_im */ \
                __ASM_EMIT(FFT_FMA("vsubps  %%ymm4, %%ymm7, %%ymm4", "vfmsub231ps  0x00 + %[FFT_A], %%ymm7, %%ymm4"))       /* ymm4 = c_im = x_re * b_im - x_im * b_re */ \
                __ASM_EMIT("vsubps          %%ymm5, %%ymm2, %%ymm0")                /* ymm0 = a_re - c_re */ \
                __ASM_EMIT("vsubps          %%ymm4, %%ymm6, %%ymm1")                /* ymm1 = a_im - c_im */ \
                __ASM_EMIT("vaddps          %%ymm5, %%ymm2, %%ymm2")                /* ymm2 = a_re + c_re */ \
                __ASM_EMIT("vaddps          %%ymm4, %%ymm6, %%ymm3")                /* ymm3 = a_im + c_im */ \
                /* Store */
                __ASM_EMIT("vmovups         %%xmm2, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%xmm0, 0x10(%[dst])")
                __ASM_EMIT("vmovups         %%xmm3, 0x20(%[dst])")
                __ASM_EMIT("vmovups         %%xmm1, 0x30(%[dst])")
                __ASM_EMIT("vextractf128    $1, %%ymm2, 0x40(%[dst])")
                __ASM_EMIT("vextractf128    $1, %%ymm0, 0x50(%[dst])")
                __ASM_EMIT("vextractf128    $1, %%ymm3, 0x60(%[dst])")
                __ASM_EMIT("vextractf128    $1, %%ymm1, 0x70(%[dst])")
                __ASM_EMIT("add             $0x80, %[dst]")

                : [dst] "+r" (dst), [index] "+r"(index)
                : [src] "r" (src), [regs] "r" (regs),
                  [FFT_A] "o" (FFT_A)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }
    }

    static inline void FFT_PSCRAMBLE_COPY_REVERSE_NAME(float *dst, const float *src, size_t rank)
    {
        size_t regs     = 1 << rank;

        for (size_t i=0; i<regs; ++i)
        {
            size_t index    = reverse_bits(FFT_TYPE(i), rank);

            ARCH_X86_ASM
            (
                /* Load scalar values */
                __ASM_EMIT("vmovlps         (%[src], %[index], 8), %%xmm0, %%xmm0")                 /* xmm0 = r0  i0  x   x     */
                __ASM_EMIT("add             %[regs], %[index]")
                __ASM_EMIT("vmovlps         (%[src], %[index], 8), %%xmm2, %%xmm2")                 /* xmm2 = r8  i8  x   x     */
                __ASM_EMIT("add             %[regs], %[index]")
                __ASM_EMIT("vmovlps         (%[src], %[index], 8), %%xmm4, %%xmm4")                 /* xmm4 = r4  i4  x   x     */
                __ASM_EMIT("add             %[regs], %[index]")
                __ASM_EMIT("vmovlps         (%[src], %[index], 8), %%xmm6, %%xmm6")                 /* xmm6 = r12 i12 x   x     */
                __ASM_EMIT("add             %[regs], %[index]")
                __ASM_EMIT("vmovlps         (%[src], %[index], 8), %%xmm1, %%xmm1")                 /* xmm1 = r2  i2  x   x     */
                __ASM_EMIT("add             %[regs], %[index]")
                __ASM_EMIT("vmovlps         (%[src], %[index], 8), %%xmm3, %%xmm3")                 /* xmm3 = r10 i10 x   x     */
                __ASM_EMIT("add             %[regs], %[index]")
                __ASM_EMIT("vmovlps         (%[src], %[index], 8), %%xmm5, %%xmm5")                 /* xmm5 = r6  i6  x   x     */
                __ASM_EMIT("add             %[regs], %[index]")
                __ASM_EMIT("vmovlps         (%[src], %[index], 8), %%xmm7, %%xmm7")                 /* xmm7 = r14 i14 x   x     */
                __ASM_EMIT("add             %[regs], %[index]")

                __ASM_EMIT("vmovhps         (%[src], %[index], 8), %%xmm0, %%xmm0")                 /* xmm0 = r0  i0  r1  i1    */
                __ASM_EMIT("add             %[regs], %[index]")
                __ASM_EMIT("vmovhps         (%[src], %[index], 8), %%xmm2, %%xmm2")                 /* xmm2 = r8  i8  r9  i9    */
                __ASM_EMIT("add             %[regs], %[index]")
                __ASM_EMIT("vmovhps         (%[src], %[index], 8), %%xmm4, %%xmm4")                 /* xmm4 = r4  i4  r5  i5    */
                __ASM_EMIT("add             %[regs], %[index]")
                __ASM_EMIT("vmovhps         (%[src], %[index], 8), %%xmm6, %%xmm6")                 /* xmm6 = r12 i12 r13 r14   */
                __ASM_EMIT("add             %[regs], %[index]")
                __ASM_EMIT("vmovhps         (%[src], %[index], 8), %%xmm1, %%xmm1")                 /* xmm1 = r2  i2  r3  i3    */
                __ASM_EMIT("add             %[regs], %[index]")
                __ASM_EMIT("vmovhps         (%[src], %[index], 8), %%xmm3, %%xmm3")                 /* xmm3 = r10 i10 r11 i11   */
                __ASM_EMIT("add             %[regs], %[index]")
                __ASM_EMIT("vmovhps         (%[src], %[index], 8), %%xmm5, %%xmm5")                 /* xmm5 = r6  i6  r7  i7    */
                __ASM_EMIT("add             %[regs], %[index]")
                __ASM_EMIT("vmovhps         (%[src], %[index], 8), %%xmm7, %%xmm7")                 /* xmm7 = r14 i14 r15 i15   */
                __ASM_EMIT("add             %[regs], %[index]")

                __ASM_EMIT("vinsertf128     $1, %%xmm2, %%ymm0, %%ymm0")                            /* ymm0 = r0  i0  r1  i1  r8  i8  r9  i9  */
                __ASM_EMIT("vinsertf128     $1, %%xmm3, %%ymm1, %%ymm1")                            /* ymm1 = r2  i2  r3  i3  r10 i10 r11 i11 */
                __ASM_EMIT("vinsertf128     $1, %%xmm6, %%ymm4, %%ymm4")                            /* ymm4 = r4  i4  r5  i5  r12 i12 r13 r14 */
                __ASM_EMIT("vinsertf128     $1, %%xmm7, %%ymm5, %%ymm5")                            /* ymm5 = r6  i6  r7  i7  r14 i14 r15 i15 */
                __ASM_EMIT("vshufps         $0xdd, %%ymm1, %%ymm0, %%ymm2")         /* ymm2 = i0  i1  i2  i3  i8  i9  i10 i11   */
                __ASM_EMIT("vshufps         $0x88, %%ymm1, %%ymm0, %%ymm0")         /* ymm0 = r0  r1  r2  r3  r8  r9  r10 r11   */
                __ASM_EMIT("vshufps         $0xdd, %%ymm5, %%ymm4, %%ymm6")         /* ymm6 = i4  i5  i6  i7  i12 i13 i14 i15   */
                __ASM_EMIT("vshufps         $0x88, %%ymm5, %%ymm4, %%ymm4")         /* ymm4 = r4  r5  r6  r7  r12 r13 r14 r15   */
                /* 1st-order 4x butterfly */
                __ASM_EMIT("vhsubps         %%ymm4, %%ymm0, %%ymm1")                /* ymm1 = r0-r1 r2-r3 r4-r5 r6-r7 = r1' r3' r5' r7' */
                __ASM_EMIT("vhsubps         %%ymm6, %%ymm2, %%ymm3")                /* ymm3 = i0-i1 i2-i3 i4-i5 i6-i7 = i1' i3' i5' i7' */
                __ASM_EMIT("vhaddps         %%ymm4, %%ymm0, %%ymm0")                /* ymm0 = r0+r1 r2+r3 r4+r5 r6+r7 = r0' r2' r4' r6' */
                __ASM_EMIT("vhaddps         %%ymm6, %%ymm2, %%ymm2")                /* ymm2 = i0+i1 i2+i3 i4+i5 i6+i7 = i0' i2' i4' i6' */
                /* 2nd-order 4x butterfly */
                __ASM_EMIT("vblendps        $0xaa, %%ymm3, %%ymm1, %%ymm4")         /* ymm4 = r1' i3' r5' i7' */
                __ASM_EMIT("vblendps        $0xaa, %%ymm1, %%ymm3, %%ymm5")         /* ymm5 = i1' r3' i5' r7' */
                __ASM_EMIT("vhsubps         %%ymm4, %%ymm0, %%ymm1")                /* ymm1 = r0'-r2' r4'-r6' r1'-i3' r5'-i7' = r2" r6" r1" r5" */
                __ASM_EMIT("vhsubps         %%ymm5, %%ymm2, %%ymm3")                /* ymm3 = i0'-i2' i4'-i6' i1'-r3' i5'-r7' = i2" i6" i3" i7" */
                __ASM_EMIT("vhaddps         %%ymm4, %%ymm0, %%ymm0")                /* ymm0 = r0'+r2' r4'+r6' r1'+i3' r5'+i7' = r0" r4" r3" r7" */
                __ASM_EMIT("vhaddps         %%ymm5, %%ymm2, %%ymm2")                /* ymm2 = i0'+i2' i4'+i6' i1'+r3' i5'+r7' = i0" i4" i1" i5" */
                __ASM_EMIT("vblendps        $0xcc, %%ymm1, %%ymm0, %%ymm4")         /* ymm4 = r0" i4" r1" r5" */
                __ASM_EMIT("vblendps        $0xcc, %%ymm0, %%ymm1, %%ymm5")         /* ymm5 = r2" r6" r3" r7" */
                __ASM_EMIT("vshufps         $0x88, %%ymm3, %%ymm2, %%ymm6")         /* ymm6 = i0" i1" i2" i3" */
                __ASM_EMIT("vshufps         $0xdd, %%ymm3, %%ymm2, %%ymm7")         /* ymm7 = i4" i5" i6" i7" */
                __ASM_EMIT("vshufps         $0x88, %%ymm5, %%ymm4, %%ymm2")         /* ymm2 = r0" r1" r2" r3" */
                __ASM_EMIT("vshufps         $0xdd, %%ymm5, %%ymm4, %%ymm3")         /* ymm3 = r4" r5" r6" r7" */
                /* 3rd-order 8x butterfly */
                __ASM_EMIT("vmulps          0x20 + %[FFT_A], %%ymm3, %%ymm4")       /* ymm4 = x_im * b_re */ \
                __ASM_EMIT("vmulps          0x20 + %[FFT_A], %%ymm7, %%ymm5")       /* ymm5 = x_im * b_im */ \
                __ASM_EMIT(FFT_FMA("vmulps  0x00 + %[FFT_A], %%ymm3, %%ymm3", ""))  /* ymm3 = x_re * b_re */ \
                __ASM_EMIT(FFT_FMA("vmulps  0x00 + %[FFT_A], %%ymm7, %%ymm7", ""))  /* ymm7 = x_re * b_im */ \
                __ASM_EMIT(FFT_FMA("vsubps  %%ymm5, %%ymm3, %%ymm5", "vfmsub231ps  0x00 + %[FFT_A], %%ymm3, %%ymm5"))       /* ymm5 = c_re = x_re * b_re - x_im * b_im */ \
                __ASM_EMIT(FFT_FMA("vaddps  %%ymm4, %%ymm7, %%ymm4", "vfmadd231ps  0x00 + %[FFT_A], %%ymm7, %%ymm4"))       /* ymm4 = c_im = x_re * b_im + x_im * b_re */ \
                __ASM_EMIT("vsubps          %%ymm5, %%ymm2, %%ymm0")                /* ymm0 = a_re - c_re */ \
                __ASM_EMIT("vsubps          %%ymm4, %%ymm6, %%ymm1")                /* ymm1 = a_im - c_im */ \
                __ASM_EMIT("vaddps          %%ymm5, %%ymm2, %%ymm2")                /* ymm2 = a_re + c_re */ \
                __ASM_EMIT("vaddps          %%ymm4, %%ymm6, %%ymm3")                /* ymm3 = a_im + c_im */ \
                /* Store */
                __ASM_EMIT("vmovups         %%xmm2, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%xmm0, 0x10(%[dst])")
                __ASM_EMIT("vmovups         %%xmm3, 0x20(%[dst])")
                __ASM_EMIT("vmovups         %%xmm1, 0x30(%[dst])")
                __ASM_EMIT("vextractf128    $1, %%ymm2, 0x40(%[dst])")
                __ASM_EMIT("vextractf128    $1, %%ymm0, 0x50(%[dst])")
                __ASM_EMIT("vextractf128    $1, %%ymm3, 0x60(%[dst])")
                __ASM_EMIT("vextractf128    $1, %%ymm1, 0x70(%[dst])")
                __ASM_EMIT("add             $0x80, %[dst]")

                : [dst] "+r" (dst), [index] "+r"(index)
                : [src] "r" (src), [regs] "r" (regs),
                  [FFT_A] "o" (FFT_A)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }
    }
}

#undef FFT_PSCRAMBLE_SELF_DIRECT_NAME
#undef FFT_PSCRAMBLE_SELF_REVERSE_NAME
#undef FFT_PSCRAMBLE_COPY_DIRECT_NAME
#undef FFT_PSCRAMBLE_COPY_REVERSE_NAME
#undef FFT_TYPE
#undef FFT_FMA

