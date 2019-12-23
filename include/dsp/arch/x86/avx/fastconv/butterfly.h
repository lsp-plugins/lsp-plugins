/*
 * butterfly.h
 *
 *  Created on: 13 дек. 2019 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_AVX_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_AVX_IMPL */

#define __SKIP(x)

// c    = a - b
// a'   = a + b
// b'   = c * w
#define FASTCONV_DIRECT_BUTTERFLY_BODY8(add_re, add_im, FMA_SEL) \
    ARCH_X86_ASM \
    ( \
        /* Prepare angle */ \
        __ASM_EMIT("vmovaps         0x00(%[ak]), %%ymm6")               /* ymm6 = x_re */ \
        __ASM_EMIT("vmovaps         0x20(%[ak]), %%ymm7")               /* ymm7 = x_im */ \
        /* Start loop */ \
        __ASM_EMIT("1:") \
            __ASM_EMIT("vmovups         0x00(%[dst], %[off1]), %%ymm0")     /* ymm0 = a_re */ \
            __ASM_EMIT("vmovups         0x20(%[dst], %[off1]), %%ymm1")     /* ymm1 = a_im */ \
            __ASM_EMIT("vmovups         0x00(%[dst], %[off2]), %%ymm2")     /* ymm2 = b_re */ \
            __ASM_EMIT("vmovups         0x20(%[dst], %[off2]), %%ymm3")     /* ymm3 = b_im */ \
            /* Perform butterfly */ \
            __ASM_EMIT("vsubps          %%ymm2, %%ymm0, %%ymm4")                /* ymm4 = c_re  = a_re - b_re */ \
            __ASM_EMIT("vsubps          %%ymm3, %%ymm1, %%ymm5")                /* ymm5 = c_im  = a_im - b_im */ \
            __ASM_EMIT("vaddps          %%ymm2, %%ymm0, %%ymm0")                /* ymm0 = a_re' = a_re + b_re */ \
            __ASM_EMIT("vaddps          %%ymm3, %%ymm1, %%ymm1")                /* ymm1 = a_im' = a_im + b_im */ \
            __ASM_EMIT("vmulps          %%ymm7, %%ymm4, %%ymm2")                /* ymm2 = x_im * c_re */ \
            __ASM_EMIT("vmulps          %%ymm7, %%ymm5, %%ymm3")                /* ymm3 = x_im * c_im */ \
            __ASM_EMIT(FMA_SEL("vmulps  %%ymm6, %%ymm4, %%ymm4", ""))           /* ymm4 = x_re * c_re */ \
            __ASM_EMIT(FMA_SEL("vmulps  %%ymm6, %%ymm5, %%ymm5", ""))           /* ymm5 = x_re * c_im */ \
            __ASM_EMIT(FMA_SEL(add_re "  %%ymm3, %%ymm4, %%ymm4", add_re " %%ymm6, %%ymm3, %%ymm4")) /* ymm4 = b_re = x_re * c_re +- x_im * c_im */ \
            __ASM_EMIT(FMA_SEL(add_im "  %%ymm2, %%ymm5, %%ymm5", add_im " %%ymm6, %%ymm2, %%ymm5")) /* ymm5 = b_im = x_re * c_im -+ x_im * c_re */ \
            /* Store values */ \
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst], %[off1])") \
            __ASM_EMIT("vmovups         %%ymm1, 0x20(%[dst], %[off1])") \
            __ASM_EMIT("vmovups         %%ymm4, 0x00(%[dst], %[off2])") \
            __ASM_EMIT("vmovups         %%ymm5, 0x20(%[dst], %[off2])") \
            __ASM_EMIT("add             $0x40, %[off1]") \
            __ASM_EMIT("add             $0x40, %[off2]") \
            __ASM_EMIT32("subl          $8, %[np]") \
            __ASM_EMIT64("subq          $8, %[np]") \
            __ASM_EMIT("jz              2f") \
            /* Rotate angle */ \
            __ASM_EMIT("vmovaps         0x00(%[wk]), %%ymm4")               /* xmm4 = w_re */ \
            __ASM_EMIT("vmovaps         0x20(%[wk]), %%ymm5")               /* xmm5 = w_im */ \
            __ASM_EMIT("vmulps          %%ymm5, %%ymm6, %%ymm2")            /* ymm2 = w_im * x_re */ \
            __ASM_EMIT("vmulps          %%ymm5, %%ymm7, %%ymm3")            /* ymm3 = w_im * x_im */ \
            __ASM_EMIT(FMA_SEL("vmulps  %%ymm4, %%ymm6, %%ymm6", ""))       /* ymm6 = w_re * x_re */ \
            __ASM_EMIT(FMA_SEL("vmulps  %%ymm4, %%ymm7, %%ymm7", ""))       /* ymm7 = w_re * x_im */ \
            __ASM_EMIT(FMA_SEL("vsubps  %%ymm3, %%ymm6, %%ymm6", "vfmsub132ps %%ymm4, %%ymm3, %%ymm6")) /* ymm6 = x_re' = w_re * x_re - w_im * x_im */ \
            __ASM_EMIT(FMA_SEL("vaddps  %%ymm2, %%ymm7, %%ymm7", "vfmadd132ps %%ymm4, %%ymm2, %%ymm7")) /* ymm7 = x_im' = w_re * x_im + w_im * x_re */ \
            /* Repeat loop */ \
        __ASM_EMIT("jmp             1b") \
        __ASM_EMIT("2:") \
        \
        : [off1] "+r" (off1), [off2] "+r" (off2), [np] __ASM_ARG_RW(np) \
        : [dst] "r" (dst), [ak] "r" (ak), [wk] "r" (wk) \
        : "cc", "memory",  \
        "%xmm0", "%xmm1", "%xmm2", "%xmm3", \
        "%xmm4", "%xmm5", "%xmm6", "%xmm7" \
    );


#define FASTCONV_DIRECT_BUTTERFLY_LAST(add_re, add_im, FMA_SEL) \
    ARCH_X86_ASM( \
        /* Loop 2x 4-element butterflies */ \
        __ASM_EMIT("vmovaps         0x00 + %[FFT_A], %%ymm6")       /* ymm6 = x_re */ \
        __ASM_EMIT("vmovaps         0x20 + %[FFT_A], %%ymm7")       /* ymm7 = x_im */ \
        __ASM_EMIT("sub             $2, %[nb]") \
        __ASM_EMIT("jb              2f") \
        __ASM_EMIT("1:") \
            /* Load data to registers */ \
            __ASM_EMIT("vmovups         0x00(%[dst]), %%xmm0")                  /* xmm0 = r0  r1  r2  r3 */ \
            __ASM_EMIT("vmovups         0x10(%[dst]), %%xmm2")                  /* xmm2 = r4  r5  r6  r7 */ \
            __ASM_EMIT("vmovups         0x20(%[dst]), %%xmm1")                  /* xmm1 = i0  i1  i2  i3 */ \
            __ASM_EMIT("vmovups         0x30(%[dst]), %%xmm3")                  /* xmm3 = i4  i5  i6  i7 */ \
            __ASM_EMIT("vinsertf128     $1, 0x40(%[dst]), %%ymm0, %%ymm0")      /* ymm0 = a_re = r0  r1  r2  r3  r8  r9  r10 r11 */ \
            __ASM_EMIT("vinsertf128     $1, 0x50(%[dst]), %%ymm2, %%ymm2")      /* ymm2 = b_re = r4  r5  r6  r7  r12 r13 r14 r15 */ \
            __ASM_EMIT("vinsertf128     $1, 0x60(%[dst]), %%ymm1, %%ymm1")      /* ymm1 = a_im = i0  i1  i2  i3  i8  i9  i10 i11 */ \
            __ASM_EMIT("vinsertf128     $1, 0x70(%[dst]), %%ymm3, %%ymm3")      /* ymm3 = b_im = i4  i5  i6  i7  i12 i13 i14 i15 */ \
            /* Perform 3rd-order butterflies */ \
            __ASM_EMIT("vsubps          %%ymm2, %%ymm0, %%ymm4")                /* ymm4 = c_re  = a_re - b_re */ \
            __ASM_EMIT("vsubps          %%ymm3, %%ymm1, %%ymm5")                /* ymm5 = c_im  = a_im - b_im */ \
            __ASM_EMIT("vaddps          %%ymm2, %%ymm0, %%ymm0")                /* ymm0 = a_re' = a_re + b_re */ \
            __ASM_EMIT("vaddps          %%ymm3, %%ymm1, %%ymm1")                /* ymm1 = a_im' = a_im + b_im */ \
            __ASM_EMIT("vmulps          %%ymm7, %%ymm4, %%ymm2")                /* ymm2 = x_im * c_re */ \
            __ASM_EMIT("vmulps          %%ymm7, %%ymm5, %%ymm3")                /* ymm3 = x_im * c_im */ \
            __ASM_EMIT(FMA_SEL("vmulps  %%ymm6, %%ymm4, %%ymm4", ""))           /* ymm4 = x_re * c_re */ \
            __ASM_EMIT(FMA_SEL("vmulps  %%ymm6, %%ymm5, %%ymm5", ""))           /* ymm5 = x_re * c_im */ \
            __ASM_EMIT(FMA_SEL(add_re "  %%ymm3, %%ymm4, %%ymm4", add_re " %%ymm6, %%ymm3, %%ymm4")) /* ymm4 = b_re = x_re * c_re +- x_im * c_im */ \
            __ASM_EMIT(FMA_SEL(add_im "  %%ymm2, %%ymm5, %%ymm5", add_im " %%ymm6, %%ymm2, %%ymm5")) /* ymm5 = b_im = x_re * c_im -+ x_im * c_re */ \
            /* 2nd-order butterflies */ \
            /* s0" = (r0 + r2) + j*(i0 + i2) + (r1 + r3) + j*(i1 + i3) */ \
            /* s1" = (r0 + r2) + j*(i0 + i2) - (r1 + r3) - j*(i1 + i3) */ \
            /* s2" = (r0 - r2) + j*(i0 - i2) + (i1 - i3) - j*(r1 - r3) */ \
            /* s3" = (r0 - r2) + j*(i0 - i2) - (i1 - i3) + j*(r1 - r3) */ \
            /* ymm0         = r0  r1  r2  r3 ... */ \
            /* ymm1         = i0  i1  i2  i3 ... */ \
            /* ymm4         = r4  r5  r6  r7 ... */ \
            /* ymm5         = i4  i5  i6  i7 ... */ \
            __ASM_EMIT("vshufps         $0xd8, %%ymm0, %%ymm0, %%ymm0")         /* ymm0 = r0 r2 r1 r3 */ \
            __ASM_EMIT("vshufps         $0xd8, %%ymm1, %%ymm1, %%ymm1")         /* ymm1 = i0 i2 i1 i3 */ \
            __ASM_EMIT("vshufps         $0xd8, %%ymm4, %%ymm4, %%ymm4") \
            __ASM_EMIT("vshufps         $0xd8, %%ymm5, %%ymm5, %%ymm5") \
            __ASM_EMIT("vhsubps         %%ymm1, %%ymm0, %%ymm2")                /* ymm2 = r0-r2 r1-r3 i0-i2 i1-i3 = r1' r3' i1' i3' */ \
            __ASM_EMIT("vhsubps         %%ymm5, %%ymm4, %%ymm3") \
            __ASM_EMIT("vhaddps         %%ymm1, %%ymm0, %%ymm0")                /* ymm0 = r0+r2 r1+r3 i0+i2 i1+i3 = r0' r2' i0' i2' */ \
            __ASM_EMIT("vhaddps         %%ymm5, %%ymm4, %%ymm4") \
            /* 1st-order 8x butterfly */ \
            __ASM_EMIT("vshufps         $0x6e, %%ymm2, %%ymm0, %%ymm1")         /* ymm0 = i0' i2' i1' r3' */ \
            __ASM_EMIT("vshufps         $0x6e, %%ymm3, %%ymm4, %%ymm5") \
            __ASM_EMIT("vshufps         $0xc4, %%ymm2, %%ymm0, %%ymm0")         /* ymm1 = r0' r2' r1' i3' */ \
            __ASM_EMIT("vshufps         $0xc4, %%ymm3, %%ymm4, %%ymm4") \
            __ASM_EMIT("vhsubps         %%ymm1, %%ymm0, %%ymm2")                /* ymm2 = r0'-r2' r1'-i3' i0'-i2' i1'-r3' = r1" r3" i1" i2" */ \
            __ASM_EMIT("vhsubps         %%ymm5, %%ymm4, %%ymm3") \
            __ASM_EMIT("vhaddps         %%ymm1, %%ymm0, %%ymm0")                /* ymm0 = r0'+r2' r1'+i3' i0'+i2' i1'+r3' = r0" r2" i0" i3" */ \
            __ASM_EMIT("vhaddps         %%ymm5, %%ymm4, %%ymm4") \
            __ASM_EMIT("vblendps        $0x88, %%ymm0, %%ymm2, %%ymm1")         /* ymm1 = r1" r3" i1" i3" */ \
            __ASM_EMIT("vblendps        $0x88, %%ymm4, %%ymm3, %%ymm5") \
            __ASM_EMIT("vblendps        $0x88, %%ymm2, %%ymm0, %%ymm0")         /* ymm0 = r0" r2" i0" i2" */ \
            __ASM_EMIT("vblendps        $0x88, %%ymm3, %%ymm4, %%ymm4") \
            __ASM_EMIT("vunpckhps       %%ymm1, %%ymm0, %%ymm3")                /* ymm3 = r0" r1" r2" r3" */ \
            __ASM_EMIT("vunpcklps       %%ymm1, %%ymm0, %%ymm2")                /* ymm2 = i0" i1" i2" i3" */ \
            __ASM_EMIT("vunpckhps       %%ymm5, %%ymm4, %%ymm1") \
            __ASM_EMIT("vunpcklps       %%ymm5, %%ymm4, %%ymm0") \
            /* Store */ \
            __ASM_EMIT("vmovups         %%xmm2, 0x00(%[dst])") \
            __ASM_EMIT("vmovups         %%xmm0, 0x10(%[dst])") \
            __ASM_EMIT("vmovups         %%xmm3, 0x20(%[dst])") \
            __ASM_EMIT("vmovups         %%xmm1, 0x30(%[dst])") \
            __ASM_EMIT("vextractf128    $1, %%ymm2, 0x40(%[dst])") \
            __ASM_EMIT("vextractf128    $1, %%ymm0, 0x50(%[dst])") \
            __ASM_EMIT("vextractf128    $1, %%ymm3, 0x60(%[dst])") \
            __ASM_EMIT("vextractf128    $1, %%ymm1, 0x70(%[dst])") \
            /* Move pointers and repeat*/ \
            __ASM_EMIT("add             $0x80, %[dst]") \
            __ASM_EMIT("sub             $2, %[nb]") \
            __ASM_EMIT("jae             1b") \
        __ASM_EMIT("2:") \
        /* 1x 4-element butterflies */ \
        __ASM_EMIT("add             $1, %[nb]") \
        __ASM_EMIT("jl              4f") \
            __ASM_EMIT("vmovups         0x00(%[dst]), %%xmm0")                  /* xmm0 = r0  r1  r2  r3 */ \
            __ASM_EMIT("vmovups         0x10(%[dst]), %%xmm2")                  /* xmm2 = r4  r5  r6  r7 */ \
            __ASM_EMIT("vmovups         0x20(%[dst]), %%xmm1")                  /* xmm1 = i0  i1  i2  i3 */ \
            __ASM_EMIT("vmovups         0x30(%[dst]), %%xmm3")                  /* xmm3 = i4  i5  i6  i7 */ \
            /* Perform 3rd-order 8x butterfly */ \
            __ASM_EMIT("vsubps          %%xmm2, %%xmm0, %%xmm4")                /* xmm4 = c_re  = a_re - b_re */ \
            __ASM_EMIT("vsubps          %%xmm3, %%xmm1, %%xmm5")                /* xmm5 = c_im  = a_im - b_im */ \
            __ASM_EMIT("vaddps          %%xmm2, %%xmm0, %%xmm0")                /* xmm0 = a_re' = a_re + b_re */ \
            __ASM_EMIT("vaddps          %%xmm3, %%xmm1, %%xmm1")                /* xmm1 = a_im' = a_im + b_im */ \
            __ASM_EMIT("vmulps          %%xmm7, %%xmm4, %%xmm2")                /* xmm2 = x_im * c_re */ \
            __ASM_EMIT("vmulps          %%xmm7, %%xmm5, %%xmm3")                /* xmm3 = x_im * c_im */ \
            __ASM_EMIT(FMA_SEL("vmulps  %%xmm6, %%xmm4, %%xmm4", ""))           /* xmm4 = x_re * c_re */ \
            __ASM_EMIT(FMA_SEL("vmulps  %%xmm6, %%xmm5, %%xmm5", ""))           /* xmm5 = x_re * c_im */ \
            __ASM_EMIT(FMA_SEL(add_re "  %%xmm3, %%xmm4, %%xmm4", add_re " %%xmm6, %%xmm3, %%xmm4")) /* xmm4 = b_re = x_re * c_re +- x_im * c_im */ \
            __ASM_EMIT(FMA_SEL(add_im "  %%xmm2, %%xmm5, %%xmm5", add_im " %%xmm6, %%xmm2, %%xmm5")) /* xmm5 = b_im = x_re * c_im -+ x_im * c_re */ \
            /* 2nd-order butterflies */ \
            /* s0" = (r0 + r2) + j*(i0 + i2) + (r1 + r3) + j*(i1 + i3) */ \
            /* s1" = (r0 + r2) + j*(i0 + i2) - (r1 + r3) - j*(i1 + i3) */ \
            /* s2" = (r0 - r2) + j*(i0 - i2) + (i1 - i3) - j*(r1 - r3) */ \
            /* s3" = (r0 - r2) + j*(i0 - i2) - (i1 - i3) + j*(r1 - r3) */ \
            /* xmm0         = r0  r1  r2  r3 ... */ \
            /* xmm1         = i0  i1  i2  i3 ... */ \
            /* xmm4         = r4  r5  r6  r7 ... */ \
            /* xmm5         = i4  i5  i6  i7 ... */ \
            __ASM_EMIT("vshufps         $0xd8, %%xmm0, %%xmm0, %%xmm0")         /* xmm0 = r0 r2 r1 r3 */ \
            __ASM_EMIT("vshufps         $0xd8, %%xmm1, %%xmm1, %%xmm1")         /* xmm1 = i0 i2 i1 i3 */ \
            __ASM_EMIT("vshufps         $0xd8, %%xmm4, %%xmm4, %%xmm4") \
            __ASM_EMIT("vshufps         $0xd8, %%xmm5, %%xmm5, %%xmm5") \
            __ASM_EMIT("vhsubps         %%xmm1, %%xmm0, %%xmm2")                /* xmm2 = r0-r2 r1-r3 i0-i2 i1-i3 = r1' r3' i1' i3' */ \
            __ASM_EMIT("vhsubps         %%xmm5, %%xmm4, %%xmm3") \
            __ASM_EMIT("vhaddps         %%xmm1, %%xmm0, %%xmm0")                /* xmm0 = r0+r2 r1+r3 i0+i2 i1+i3 = r0' r2' i0' i2' */ \
            __ASM_EMIT("vhaddps         %%xmm5, %%xmm4, %%xmm4") \
            /* 1st-order butterflies */ \
            __ASM_EMIT("vshufps         $0x6e, %%xmm2, %%xmm0, %%xmm1")         /* xmm0 = i0' i2' i1' r3' */ \
            __ASM_EMIT("vshufps         $0x6e, %%xmm3, %%xmm4, %%xmm5") \
            __ASM_EMIT("vshufps         $0xc4, %%xmm2, %%xmm0, %%xmm0")         /* xmm1 = r0' r2' r1' i3' */ \
            __ASM_EMIT("vshufps         $0xc4, %%xmm3, %%xmm4, %%xmm4") \
            __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst])") \
            __ASM_EMIT("vmovups         %%xmm1, 0x10(%[dst])") \
            __ASM_EMIT("vmovups         %%xmm4, 0x20(%[dst])") \
            __ASM_EMIT("vmovups         %%xmm5, 0x30(%[dst])") \
            __ASM_EMIT("vhsubps         %%xmm1, %%xmm0, %%xmm2")                /* xmm2 = r0'-r2' r1'-i3' i0'-i2' i1'-r3' = r1" r3" i1" i2" */ \
            __ASM_EMIT("vhsubps         %%xmm5, %%xmm4, %%xmm3") \
            __ASM_EMIT("vhaddps         %%xmm1, %%xmm0, %%xmm0")                /* xmm0 = r0'+r2' r1'+i3' i0'+i2' i1'+r3' = r0" r2" i0" i3" */ \
            __ASM_EMIT("vhaddps         %%xmm5, %%xmm4, %%xmm4") \
            __ASM_EMIT("vblendps        $0x88, %%xmm0, %%xmm2, %%xmm1")         /* xmm1 = r1" r3" i1" i3" */ \
            __ASM_EMIT("vblendps        $0x88, %%xmm4, %%xmm3, %%xmm5") \
            __ASM_EMIT("vblendps        $0x88, %%xmm2, %%xmm0, %%xmm0")         /* xmm0 = r0" r2" i0" i2" */ \
            __ASM_EMIT("vblendps        $0x88, %%xmm3, %%xmm4, %%xmm4") \
            __ASM_EMIT("vunpckhps       %%xmm1, %%xmm0, %%xmm3")                /* xmm3 = i0" i1" i2" i3" */ \
            __ASM_EMIT("vunpcklps       %%xmm1, %%xmm0, %%xmm2")                /* xmm2 = r0" r1" r2" r3" */ \
            __ASM_EMIT("vunpckhps       %%xmm5, %%xmm4, %%xmm1") \
            __ASM_EMIT("vunpcklps       %%xmm5, %%xmm4, %%xmm0") \
            /* Store */ \
            __ASM_EMIT("vmovups         %%xmm2, 0x00(%[dst])") \
            __ASM_EMIT("vmovups         %%xmm0, 0x10(%[dst])") \
            __ASM_EMIT("vmovups         %%xmm3, 0x20(%[dst])") \
            __ASM_EMIT("vmovups         %%xmm1, 0x30(%[dst])") \
        __ASM_EMIT("4:") \
        \
        : [dst] "+r" (dst), [nb] "+r" (nb) \
        : [FFT_A] "o" (FFT_A) \
        : "cc", "memory", \
          "%xmm0", "%xmm1", "%xmm2", "%xmm3", \
          "%xmm4", "%xmm5", "%xmm6", "%xmm7" \
    );

#define FASTCONV_REVERSE_BUTTERFLY_BODY8(add_re, add_im, FMA_SEL) \
    ARCH_X86_ASM \
    ( \
        /* Prepare angle */ \
        __ASM_EMIT("vmovaps         0x00(%[ak]), %%ymm6")               /* ymm6 = x_re */ \
        __ASM_EMIT("vmovaps         0x20(%[ak]), %%ymm7")               /* ymm7 = x_im */ \
        /* Start loop */ \
        __ASM_EMIT("1:") \
            __ASM_EMIT("vmovups         0x00(%[dst], %[off1]), %%ymm0")     /* ymm0 = a_re */ \
            __ASM_EMIT("vmovups         0x20(%[dst], %[off1]), %%ymm1")     /* ymm1 = a_im */ \
            __ASM_EMIT("vmovups         0x00(%[dst], %[off2]), %%ymm2")     /* ymm2 = b_re */ \
            __ASM_EMIT("vmovups         0x20(%[dst], %[off2]), %%ymm3")     /* ymm3 = b_im */ \
            /* Calculate complex multiplication */ \
            __ASM_EMIT("vmulps          %%ymm7, %%ymm2, %%ymm4")            /* ymm4 = x_im * b_re */ \
            __ASM_EMIT("vmulps          %%ymm7, %%ymm3, %%ymm5")            /* ymm5 = x_im * b_im */ \
            __ASM_EMIT(FMA_SEL("vmulps  %%ymm6, %%ymm2, %%ymm2", ""))       /* ymm2 = x_re * b_re */ \
            __ASM_EMIT(FMA_SEL("vmulps  %%ymm6, %%ymm3, %%ymm3", ""))       /* ymm3 = x_re * b_im */ \
            __ASM_EMIT(FMA_SEL(add_re "  %%ymm5, %%ymm2, %%ymm5", add_re " %%ymm6, %%ymm2, %%ymm5")) /* ymm5 = c_re = x_re * b_re +- x_im * b_im */ \
            __ASM_EMIT(FMA_SEL(add_im "  %%ymm4, %%ymm3, %%ymm4", add_im " %%ymm6, %%ymm3, %%ymm4")) /* ymm4 = c_im = x_re * b_im -+ x_im * b_re */ \
            /* Perform butterfly */ \
            __ASM_EMIT("vsubps          %%ymm5, %%ymm0, %%ymm2")            /* ymm2 = a_re - c_re */ \
            __ASM_EMIT("vsubps          %%ymm4, %%ymm1, %%ymm3")            /* ymm3 = a_im - c_im */ \
            __ASM_EMIT("vaddps          %%ymm5, %%ymm0, %%ymm0")            /* ymm0 = a_re + c_re */ \
            __ASM_EMIT("vaddps          %%ymm4, %%ymm1, %%ymm1")            /* ymm1 = a_im + c_im */ \
            /* Store values */ \
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst], %[off1])") \
            __ASM_EMIT("vmovups         %%ymm1, 0x20(%[dst], %[off1])") \
            __ASM_EMIT("vmovups         %%ymm2, 0x00(%[dst], %[off2])") \
            __ASM_EMIT("vmovups         %%ymm3, 0x20(%[dst], %[off2])") \
            __ASM_EMIT("add             $0x40, %[off1]") \
            __ASM_EMIT("add             $0x40, %[off2]") \
            __ASM_EMIT32("subl          $8, %[np]") \
            __ASM_EMIT64("subq          $8, %[np]") \
            __ASM_EMIT("jz              2f") \
            /* Rotate angle */ \
            __ASM_EMIT("vmovaps         0x00(%[wk]), %%ymm4")               /* xmm4 = w_re */ \
            __ASM_EMIT("vmovaps         0x20(%[wk]), %%ymm5")               /* xmm5 = w_im */ \
            __ASM_EMIT("vmulps          %%ymm5, %%ymm6, %%ymm2")            /* ymm2 = w_im * x_re */ \
            __ASM_EMIT("vmulps          %%ymm5, %%ymm7, %%ymm3")            /* ymm3 = w_im * x_im */ \
            __ASM_EMIT(FMA_SEL("vmulps  %%ymm4, %%ymm6, %%ymm6", ""))       /* ymm6 = w_re * x_re */ \
            __ASM_EMIT(FMA_SEL("vmulps  %%ymm4, %%ymm7, %%ymm7", ""))       /* ymm7 = w_re * x_im */ \
            __ASM_EMIT(FMA_SEL("vsubps  %%ymm3, %%ymm6, %%ymm6", "vfmsub132ps %%ymm4, %%ymm3, %%ymm6")) /* ymm6 = x_re' = w_re * x_re - w_im * x_im */ \
            __ASM_EMIT(FMA_SEL("vaddps  %%ymm2, %%ymm7, %%ymm7", "vfmadd132ps %%ymm4, %%ymm2, %%ymm7")) /* ymm7 = x_im' = w_re * x_im + w_im * x_re */ \
            /* Repeat loop */ \
        __ASM_EMIT("jmp             1b") \
        __ASM_EMIT("2:") \
        \
        : [off1] "+r" (off1), [off2] "+r" (off2), [np] __ASM_ARG_RW(np) \
        : [dst] "r" (dst), [ak] "r" (ak), [wk] "r" (wk) \
        : "cc", "memory",  \
        "%xmm0", "%xmm1", "%xmm2", "%xmm3", \
        "%xmm4", "%xmm5", "%xmm6", "%xmm7"  \
    );

#define FASTCONV_REVERSE_BUTTERFLY_BODY_LAST(add_re, add_im, FMA_SEL, IF_ADD) \
    size_t off; \
    float norm = 0.5f / np; \
    ARCH_X86_ASM \
    ( \
        /* Prepare angle */ \
        __ASM_EMIT("vbroadcastss    %[norm], %%ymm1")                   /* ymm1 = k */ \
        __ASM_EMIT("lea             (,%[np], 4), %[off]")               /* off  = np * 8 */ \
        __ASM_EMIT("vmovaps         0x00(%[ak]), %%ymm6")               /* ymm6 = x_re */ \
        __ASM_EMIT("vmovaps         0x20(%[ak]), %%ymm7")               /* ymm7 = x_im */ \
        __ASM_EMIT("vmovaps         0x00(%[wk]), %%ymm4")               /* xmm4 = w_re */ \
        __ASM_EMIT("vmovaps         0x20(%[wk]), %%ymm5")               /* xmm5 = w_im */ \
        /* Start loop */ \
        __ASM_EMIT("1:") \
            __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")              /* ymm0 = a_re */ \
            __ASM_EMIT("vmovups         0x00(%[src], %[off], 2), %%ymm2")   /* ymm2 = b_re */ \
            __ASM_EMIT("vmovups         0x20(%[src], %[off], 2), %%ymm3")   /* ymm3 = b_im */ \
            /* Calculate complex multiplication */ \
            __ASM_EMIT("vmulps          %%ymm7, %%ymm3, %%ymm3")            /* ymm3 = x_im * b_im */ \
            __ASM_EMIT(FMA_SEL("vmulps  %%ymm6, %%ymm2, %%ymm2", ""))       /* ymm2 = x_re * b_re */ \
            __ASM_EMIT(FMA_SEL(add_re "  %%ymm3, %%ymm2, %%ymm3", add_re " %%ymm6, %%ymm2, %%ymm3")) /* ymm3 = c_re = x_re * b_re +- x_im * b_im */ \
            /* Perform butterfly */ \
            __ASM_EMIT("vsubps          %%ymm3, %%ymm0, %%ymm2")            /* ymm2 = a_re - c_re */ \
            __ASM_EMIT("vaddps          %%ymm3, %%ymm0, %%ymm0")            /* ymm0 = a_re + c_re */ \
            __ASM_EMIT("vmulps          %%ymm1, %%ymm2, %%ymm2") \
            __ASM_EMIT("vmulps          %%ymm1, %%ymm0, %%ymm0") \
            /* Store values */ \
            __ASM_EMIT(IF_ADD("vaddps   0x00(%[dst]), %%ymm0, %%ymm0")) \
            __ASM_EMIT(IF_ADD("vaddps   0x00(%[dst], %[off]), %%ymm2, %%ymm2")) \
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])") \
            __ASM_EMIT("vmovups         %%ymm2, 0x00(%[dst], %[off])") \
            __ASM_EMIT("add             $0x40, %[src]") \
            __ASM_EMIT("add             $0x20, %[dst]") \
            __ASM_EMIT32("subl          $8, %[np]") \
            __ASM_EMIT64("subq          $8, %[np]") \
            __ASM_EMIT("jbe             2f") \
            /* Rotate angle */ \
            __ASM_EMIT("vmulps          %%ymm5, %%ymm6, %%ymm2")            /* ymm2 = w_im * x_re */ \
            __ASM_EMIT("vmulps          %%ymm5, %%ymm7, %%ymm3")            /* ymm3 = w_im * x_im */ \
            __ASM_EMIT(FMA_SEL("vmulps  %%ymm4, %%ymm6, %%ymm6", ""))       /* ymm6 = w_re * x_re */ \
            __ASM_EMIT(FMA_SEL("vmulps  %%ymm4, %%ymm7, %%ymm7", ""))       /* ymm7 = w_re * x_im */ \
            __ASM_EMIT(FMA_SEL("vsubps  %%ymm3, %%ymm6, %%ymm6", "vfmsub132ps %%ymm4, %%ymm3, %%ymm6")) /* ymm6 = x_re' = w_re * x_re - w_im * x_im */ \
            __ASM_EMIT(FMA_SEL("vaddps  %%ymm2, %%ymm7, %%ymm7", "vfmadd132ps %%ymm4, %%ymm2, %%ymm7")) /* ymm7 = x_im' = w_re * x_im + w_im * x_re */ \
            /* Repeat loop */ \
        __ASM_EMIT("jmp             1b") \
        __ASM_EMIT("2:") \
        \
        : [off] "=&r" (off), [np] __ASM_ARG_RW(np) \
        : [dst] "r" (dst), [src] "r" (src), [ak] "r" (ak), [wk] "r" (wk), \
          [norm] "o" (norm) \
        : "cc", "memory",  \
          "%xmm0", "%xmm1", "%xmm2", "%xmm3", \
          "%xmm4", "%xmm5", "%xmm6", "%xmm7"  \
    );

namespace avx
{
#define FMA_OFF(a, b)       a
#define FMA_ON(a, b)        b
#define FASTCONV_SET(x)
#define FASTCONV_ADD(x)     x

    static inline void fastconv_direct_butterfly(float *dst, const float *ak, const float *wk, size_t pairs, size_t nb)
    {
        size_t off1, off2, np;
        off1        = 0;
        size_t step = pairs << 3;
        for (size_t i=0; i<nb; ++i)
        {
            off2        = off1 + step;
            np          = pairs;

            FASTCONV_DIRECT_BUTTERFLY_BODY8("vaddps", "vsubps", FMA_OFF);

            off1        = off2;
        }
    }

    static inline void fastconv_direct_butterfly_last(float *dst, size_t nb)
    {
        FASTCONV_DIRECT_BUTTERFLY_LAST("vaddps", "vsubps", FMA_OFF);
    }

    static inline void fastconv_direct_butterfly_fma3(float *dst, const float *ak, const float *wk, size_t pairs, size_t nb)
    {
        size_t off1, off2, np;
        off1        = 0;
        size_t step = pairs << 3;
        for (size_t i=0; i<nb; ++i)
        {
            off2        = off1 + step;
            np          = pairs;

            FASTCONV_DIRECT_BUTTERFLY_BODY8("vfmadd132ps", "vfmsub132ps", FMA_ON);

            off1        = off2;
        }
    }

    static inline void fastconv_direct_butterfly_last_fma3(float *dst, size_t nb)
    {
        FASTCONV_DIRECT_BUTTERFLY_LAST("vfmadd132ps", "vfmsub132ps", FMA_ON);
    }

    static inline void fastconv_reverse_butterfly(float *dst, const float *ak, const float *wk, size_t pairs, size_t nb)
    {
        size_t off1, off2, np;
        off1        = 0;
        size_t step = pairs << 3;
        for (size_t i=0; i<nb; ++i)
        {
            off2        = off1 + step;
            np          = pairs;

            FASTCONV_REVERSE_BUTTERFLY_BODY8("vsubps", "vaddps", FMA_OFF);

            off1        = off2;
        }
    }

    static inline void fastconv_reverse_butterfly_last(float *dst, const float *src, const float *ak, const float *wk, size_t np)
    {
        FASTCONV_REVERSE_BUTTERFLY_BODY_LAST("vsubps", "vaddps", FMA_OFF, FASTCONV_SET);
    }

    static inline void fastconv_reverse_butterfly_last_adding(float *dst, const float *src, const float *ak, const float *wk, size_t np)
    {
        FASTCONV_REVERSE_BUTTERFLY_BODY_LAST("vsubps", "vaddps", FMA_OFF, FASTCONV_ADD);
    }

    static inline void fastconv_reverse_butterfly_fma3(float *dst, const float *ak, const float *wk, size_t pairs, size_t nb)
    {
        size_t off1, off2, np;
        off1        = 0;
        size_t step = pairs << 3;
        for (size_t i=0; i<nb; ++i)
        {
            off2        = off1 + step;
            np          = pairs;

            FASTCONV_REVERSE_BUTTERFLY_BODY8("vfmsub231ps", "vfmadd231ps", FMA_ON);

            off1        = off2;
        }
    }

    static inline void fastconv_reverse_butterfly_last_fma3(float *dst, const float *src, const float *ak, const float *wk, size_t np)
    {
        FASTCONV_REVERSE_BUTTERFLY_BODY_LAST("vfmsub231ps", "vfmadd231ps", FMA_ON, FASTCONV_SET);
    }

    static inline void fastconv_reverse_butterfly_last_adding_fma3(float *dst, const float *src, const float *ak, const float *wk, size_t np)
    {
        FASTCONV_REVERSE_BUTTERFLY_BODY_LAST("vfmsub231ps", "vfmadd231ps", FMA_ON, FASTCONV_ADD);
    }

#undef FASTCONV_DIRECT_BUTTERFLY_BODY8
#undef FASTCONV_DIRECT_BUTTERFLY_LAST
#undef FASTCONV_REVERSE_BUTTERFLY_BODY8
#undef FASTCONV_REVERSE_BUTTERFLY_LAST
#undef FASTCONV_SET
#undef FASTCONV_ADD
#undef FMA_OFF
#undef FMA_ON
}
