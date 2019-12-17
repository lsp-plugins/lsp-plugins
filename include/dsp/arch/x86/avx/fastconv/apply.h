/*
 * apply.h
 *
 *  Created on: 16 дек. 2019 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_AVX_FASTCONV_APPLY_H_
#define DSP_ARCH_X86_AVX_FASTCONV_APPLY_H_

#ifndef DSP_ARCH_X86_AVX_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_AVX_IMPL */

namespace avx
{
    #define FMA_OFF(a, b)       a
    #define FMA_ON(a, b)        b
    #define FMA_SEL1(a, b)      a

    #define FASTCONV_APPLY_PREPARE_CORE(FMA_SEL) \
        size_t off; \
        ARCH_X86_ASM( \
            /* 2x blocks of 4x-butterfly loop */ \
            __ASM_EMIT("xor             %[off], %[off]") \
            __ASM_EMIT("sub             $2, %[nb]") \
            __ASM_EMIT("jb              2f") \
            __ASM_EMIT("1:") \
                /* Load data and apply convolution*/ \
                __ASM_EMIT("vmovups         0x00(%[c1], %[off]), %%ymm0")                   /* ymm0 = r0 = r0  r4  */ \
                __ASM_EMIT("vmovups         0x20(%[c1], %[off]), %%ymm2")                   /* ymm2 = i0 = i0  i4  */ \
                __ASM_EMIT("vmovups         0x40(%[c1], %[off]), %%ymm4")                   /* xmm4 = r4 = r8  r12 */ \
                __ASM_EMIT("vmovups         0x60(%[c1], %[off]), %%ymm6")                   /* xmm6 = i4 = i8  i12 */ \
                /* Apply convolution */ \
                __ASM_EMIT("vmulps          0x20(%[c2], %[off]), %%ymm0, %%ymm1")           /* ymm1 = r0*I */ \
                __ASM_EMIT("vmulps          0x60(%[c2], %[off]), %%ymm4, %%ymm5")           /* ymm1 = r4*I */ \
                __ASM_EMIT("vmulps          0x20(%[c2], %[off]), %%ymm2, %%ymm3")           /* ymm3 = i0*I */ \
                __ASM_EMIT("vmulps          0x60(%[c2], %[off]), %%ymm6, %%ymm7")           /* ymm3 = i4*I */ \
                __ASM_EMIT(FMA_SEL("vmulps  0x00(%[c2], %[off]), %%ymm0, %%ymm0", ""))      /* ymm0 = r0*R */ \
                __ASM_EMIT(FMA_SEL("vmulps  0x40(%[c2], %[off]), %%ymm4, %%ymm4", ""))      /* ymm4 = r4*R */ \
                __ASM_EMIT(FMA_SEL("vmulps  0x00(%[c2], %[off]), %%ymm2, %%ymm2", ""))      /* ymm2 = i0*R */ \
                __ASM_EMIT(FMA_SEL("vmulps  0x40(%[c2], %[off]), %%ymm6, %%ymm6", ""))      /* ymm6 = i4*R */ \
                __ASM_EMIT(FMA_SEL("vsubps  %%ymm3, %%ymm0, %%ymm0", "vfmsub132ps 0x00(%[c2], %[off]), %%ymm3, %%ymm0")) /* ymm0 = r0*R - i0*I */ \
                __ASM_EMIT(FMA_SEL("vsubps  %%ymm7, %%ymm4, %%ymm4", "vfmsub132ps 0x40(%[c2], %[off]), %%ymm7, %%ymm4")) /* ymm4 = r4*R - i4*I */ \
                __ASM_EMIT(FMA_SEL("vaddps  %%ymm1, %%ymm2, %%ymm2", "vfmadd132ps 0x00(%[c2], %[off]), %%ymm1, %%ymm2")) /* ymm2 = r0*I + i0*R */ \
                __ASM_EMIT(FMA_SEL("vaddps  %%ymm5, %%ymm6, %%ymm6", "vfmadd132ps 0x40(%[c2], %[off]), %%ymm5, %%ymm6")) /* ymm2 = r0*I + i0*R */ \
                /* Reshuffle */ \
                __ASM_EMIT("vextractf128    $1, %%ymm0, %%xmm5")                    /* xmm5 = r4 */ \
                __ASM_EMIT("vextractf128    $1, %%ymm2, %%xmm7")                    /* xmm7 = i4 */ \
                __ASM_EMIT("vinsertf128     $1, %%xmm4, %%ymm0, %%ymm0")            /* ymm0 = r0  r8  */ \
                __ASM_EMIT("vinsertf128     $1, %%xmm6, %%ymm2, %%ymm2")            /* ymm2 = i0  i8  */ \
                __ASM_EMIT("vinsertf128     $0, %%xmm5, %%ymm4, %%ymm4")            /* ymm4 = r4  r12 */ \
                __ASM_EMIT("vinsertf128     $0, %%xmm7, %%ymm6, %%ymm6")            /* ymm6 = i4  i12 */ \
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
                __ASM_EMIT("vmovups         %%xmm2, 0x00(%[dst], %[off])") \
                __ASM_EMIT("vmovups         %%xmm0, 0x10(%[dst], %[off])") \
                __ASM_EMIT("vmovups         %%xmm3, 0x20(%[dst], %[off])") \
                __ASM_EMIT("vmovups         %%xmm1, 0x30(%[dst], %[off])") \
                __ASM_EMIT("vextractf128    $1, %%ymm2, 0x40(%[dst], %[off])") \
                __ASM_EMIT("vextractf128    $1, %%ymm0, 0x50(%[dst], %[off])") \
                __ASM_EMIT("vextractf128    $1, %%ymm3, 0x60(%[dst], %[off])") \
                __ASM_EMIT("vextractf128    $1, %%ymm1, 0x70(%[dst], %[off])") \
            __ASM_EMIT("add             $0x80, %[off]") \
            __ASM_EMIT("sub             $2, %[nb]") \
            __ASM_EMIT("jae             1b") \
            /* 1x block of 4-butterfly */ \
            __ASM_EMIT("2:") \
            __ASM_EMIT("add             $1, %[nb]") \
            __ASM_EMIT("jl              4f") \
                __ASM_EMIT("vmovups         0x00(%[c1], %[off]), %%ymm0")                   /* ymm0 = r0 = r0  r4  */ \
                __ASM_EMIT("vmovups         0x20(%[c1], %[off]), %%ymm2")                   /* ymm2 = i0 = i0  i4  */ \
                /* Apply convolution */ \
                __ASM_EMIT("vmulps          0x20(%[c2], %[off]), %%ymm0, %%ymm1")           /* ymm1 = r0*I */ \
                __ASM_EMIT("vmulps          0x20(%[c2], %[off]), %%ymm2, %%ymm3")           /* ymm3 = i0*I */ \
                __ASM_EMIT(FMA_SEL("vmulps  0x00(%[c2], %[off]), %%ymm0, %%ymm0", ""))      /* ymm0 = r0*R */ \
                __ASM_EMIT(FMA_SEL("vmulps  0x00(%[c2], %[off]), %%ymm2, %%ymm2", ""))      /* ymm2 = i0*R */ \
                __ASM_EMIT(FMA_SEL("vsubps  %%ymm3, %%ymm0, %%ymm0", "vfmsub132ps 0x00(%[c2], %[off]), %%ymm3, %%ymm0")) /* ymm0 = r0*R - i0*I */ \
                __ASM_EMIT(FMA_SEL("vaddps  %%ymm1, %%ymm2, %%ymm2", "vfmadd132ps 0x00(%[c2], %[off]), %%ymm1, %%ymm2")) /* ymm2 = r0*I + i0*R */ \
                /* Reshuffle */ \
                __ASM_EMIT("vextractf128    $1, %%ymm0, %%xmm4")                    /* xmm4 = r4 */ \
                __ASM_EMIT("vextractf128    $1, %%ymm2, %%xmm6")                    /* xmm6 = i4 */ \
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
                __ASM_EMIT("vmovups         %%xmm2, 0x00(%[dst], %[off])") \
                __ASM_EMIT("vmovups         %%xmm0, 0x10(%[dst], %[off])") \
                __ASM_EMIT("vmovups         %%xmm3, 0x20(%[dst], %[off])") \
                __ASM_EMIT("vmovups         %%xmm1, 0x30(%[dst], %[off])") \
            __ASM_EMIT("4:")  \
            : [off] "=&r" (off), [nb] "+r" (nb) \
            : [dst] "r" (dst), [c1] "r" (c1), [c2] "r" (c2), \
              [FFT_A] "o" (FFT_A) \
            : "cc", "memory", \
              "%xmm0", "%xmm1", "%xmm2", "%xmm3", \
              "%xmm4", "%xmm5", "%xmm6", "%xmm7" \
        )

    #define FASTCONV_APPLY_CORE(FMA_SEL) \
        size_t off; \
        ARCH_X86_ASM( \
            /* 2x blocks of 4x-butterfly loop */ \
            __ASM_EMIT("xor             %[off], %[off]") \
            __ASM_EMIT("sub             $2, %[nb]") \
            __ASM_EMIT("jb              2f") \
            __ASM_EMIT("1:") \
                /* Load data to registers */ \
                __ASM_EMIT("vmovups         0x00(%[dst], %[off]), %%xmm0")              /* xmm0 = r0  r1  r2  r3 */ \
                __ASM_EMIT("vmovups         0x10(%[dst], %[off]), %%xmm2")              /* xmm2 = r4  r5  r6  r7 */ \
                __ASM_EMIT("vmovups         0x20(%[dst], %[off]), %%xmm1")              /* xmm1 = i0  i1  i2  i3 */ \
                __ASM_EMIT("vmovups         0x30(%[dst], %[off]), %%xmm3")              /* xmm3 = i4  i5  i6  i7 */ \
                __ASM_EMIT("vinsertf128     $1, 0x40(%[dst], %[off]), %%ymm0, %%ymm0")  /* ymm0 = a_re = r0  r1  r2  r3  r8  r9  r10 r11 */ \
                __ASM_EMIT("vinsertf128     $1, 0x50(%[dst], %[off]), %%ymm2, %%ymm2")  /* ymm2 = b_re = r4  r5  r6  r7  r12 r13 r14 r15 */ \
                __ASM_EMIT("vinsertf128     $1, 0x60(%[dst], %[off]), %%ymm1, %%ymm1")  /* ymm1 = a_im = i0  i1  i2  i3  i8  i9  i10 i11 */ \
                __ASM_EMIT("vinsertf128     $1, 0x70(%[dst], %[off]), %%ymm3, %%ymm3")  /* ymm3 = b_im = i4  i5  i6  i7  i12 i13 i14 i15 */ \
                /* Perform 3rd-order butterflies */ \
                __ASM_EMIT("vsubps          %%ymm2, %%ymm0, %%ymm4")                /* ymm4 = c_re  = a_re - b_re */ \
                __ASM_EMIT("vsubps          %%ymm3, %%ymm1, %%ymm5")                /* ymm5 = c_im  = a_im - b_im */ \
                __ASM_EMIT("vaddps          %%ymm2, %%ymm0, %%ymm0")                /* ymm0 = a_re' = a_re + b_re */ \
                __ASM_EMIT("vaddps          %%ymm3, %%ymm1, %%ymm1")                /* ymm1 = a_im' = a_im + b_im */ \
                __ASM_EMIT("vmulps          0x20 + %[FFT_A], %%ymm4, %%ymm2")       /* ymm2 = x_im * c_re */ \
                __ASM_EMIT("vmulps          0x20 + %[FFT_A], %%ymm5, %%ymm3")       /* ymm3 = x_im * c_im */ \
                __ASM_EMIT(FMA_SEL("vmulps  0x00 + %[FFT_A], %%ymm4, %%ymm4", ""))  /* ymm4 = x_re * c_re */ \
                __ASM_EMIT(FMA_SEL("vmulps  0x00 + %[FFT_A], %%ymm5, %%ymm5", ""))  /* ymm5 = x_re * c_im */ \
                __ASM_EMIT(FMA_SEL("vaddps  %%ymm3, %%ymm4, %%ymm4", "vfmadd132ps 0x00 + %[FFT_A], %%ymm3, %%ymm4")) /* ymm4 = b_re = x_re * c_re + x_im * c_im */ \
                __ASM_EMIT(FMA_SEL("vsubps  %%ymm2, %%ymm5, %%ymm5", "vfmsub132ps 0x00 + %[FFT_A], %%ymm2, %%ymm5")) /* ymm5 = b_im = x_re * c_im - x_im * c_re */ \
                /* 2nd-order butterflies */ \
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
                __ASM_EMIT("vunpckhps       %%ymm1, %%ymm0, %%ymm2")                /* ymm2 = i0" i1" i2" i3" */ \
                __ASM_EMIT("vunpckhps       %%ymm5, %%ymm4, %%ymm6") \
                __ASM_EMIT("vunpcklps       %%ymm1, %%ymm0, %%ymm0")                /* ymm0 = r0" r1" r2" r3" */ \
                __ASM_EMIT("vunpcklps       %%ymm5, %%ymm4, %%ymm4") \
                /* Reshuffle */ \
                __ASM_EMIT("vextractf128    $1, %%ymm0, %%xmm5")                    /* xmm5 = r4 */ \
                __ASM_EMIT("vextractf128    $1, %%ymm2, %%xmm7")                    /* xmm7 = i4 */ \
                __ASM_EMIT("vinsertf128     $1, %%xmm4, %%ymm0, %%ymm0")            /* ymm0 = r0  r8  */ \
                __ASM_EMIT("vinsertf128     $1, %%xmm6, %%ymm2, %%ymm2")            /* ymm2 = i0  i8  */ \
                __ASM_EMIT("vinsertf128     $0, %%xmm5, %%ymm4, %%ymm4")            /* ymm4 = r4  r12 */ \
                __ASM_EMIT("vinsertf128     $0, %%xmm7, %%ymm6, %%ymm6")            /* ymm6 = i4  i12 */ \
                /* Apply convolution */ \
                __ASM_EMIT("vmulps          0x20(%[conv], %[off]), %%ymm0, %%ymm1")             /* ymm1 = r0*I */ \
                __ASM_EMIT("vmulps          0x60(%[conv], %[off]), %%ymm4, %%ymm5")             /* ymm1 = r4*I */ \
                __ASM_EMIT("vmulps          0x20(%[conv], %[off]), %%ymm2, %%ymm3")             /* ymm3 = i0*I */ \
                __ASM_EMIT("vmulps          0x60(%[conv], %[off]), %%ymm6, %%ymm7")             /* ymm3 = i4*I */ \
                __ASM_EMIT(FMA_SEL("vmulps  0x00(%[conv], %[off]), %%ymm0, %%ymm0", ""))        /* ymm0 = r0*R */ \
                __ASM_EMIT(FMA_SEL("vmulps  0x40(%[conv], %[off]), %%ymm4, %%ymm4", ""))        /* ymm4 = r4*R */ \
                __ASM_EMIT(FMA_SEL("vmulps  0x00(%[conv], %[off]), %%ymm2, %%ymm2", ""))        /* ymm2 = i0*R */ \
                __ASM_EMIT(FMA_SEL("vmulps  0x40(%[conv], %[off]), %%ymm6, %%ymm6", ""))        /* ymm6 = i4*R */ \
                __ASM_EMIT(FMA_SEL("vsubps  %%ymm3, %%ymm0, %%ymm0", "vfmsub132ps 0x00(%[conv], %[off]), %%ymm3, %%ymm0")) /* ymm0 = r0*R - i0*I */ \
                __ASM_EMIT(FMA_SEL("vsubps  %%ymm7, %%ymm4, %%ymm4", "vfmsub132ps 0x40(%[conv], %[off]), %%ymm7, %%ymm4")) /* ymm4 = r4*R - i4*I */ \
                __ASM_EMIT(FMA_SEL("vaddps  %%ymm1, %%ymm2, %%ymm2", "vfmadd132ps 0x00(%[conv], %[off]), %%ymm1, %%ymm2")) /* ymm2 = r0*I + i0*R */ \
                __ASM_EMIT(FMA_SEL("vaddps  %%ymm5, %%ymm6, %%ymm6", "vfmadd132ps 0x40(%[conv], %[off]), %%ymm5, %%ymm6")) /* ymm2 = r0*I + i0*R */ \
                /* Reshuffle */ \
                __ASM_EMIT("vextractf128    $1, %%ymm0, %%xmm5")                    /* xmm5 = r4 */ \
                __ASM_EMIT("vextractf128    $1, %%ymm2, %%xmm7")                    /* xmm7 = i4 */ \
                __ASM_EMIT("vinsertf128     $1, %%xmm4, %%ymm0, %%ymm0")            /* ymm0 = r0  r8  */ \
                __ASM_EMIT("vinsertf128     $1, %%xmm6, %%ymm2, %%ymm2")            /* ymm2 = i0  i8  */ \
                __ASM_EMIT("vinsertf128     $0, %%xmm5, %%ymm4, %%ymm4")            /* ymm4 = r4  r12 */ \
                __ASM_EMIT("vinsertf128     $0, %%xmm7, %%ymm6, %%ymm6")            /* ymm6 = i4  i12 */ \
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
                __ASM_EMIT("vmovups         %%xmm2, 0x00(%[dst], %[off])") \
                __ASM_EMIT("vmovups         %%xmm0, 0x10(%[dst], %[off])") \
                __ASM_EMIT("vmovups         %%xmm3, 0x20(%[dst], %[off])") \
                __ASM_EMIT("vmovups         %%xmm1, 0x30(%[dst], %[off])") \
                __ASM_EMIT("vextractf128    $1, %%ymm2, 0x40(%[dst], %[off])") \
                __ASM_EMIT("vextractf128    $1, %%ymm0, 0x50(%[dst], %[off])") \
                __ASM_EMIT("vextractf128    $1, %%ymm3, 0x60(%[dst], %[off])") \
                __ASM_EMIT("vextractf128    $1, %%ymm1, 0x70(%[dst], %[off])") \
            __ASM_EMIT("add             $0x80, %[off]") \
            __ASM_EMIT("sub             $2, %[nb]") \
            __ASM_EMIT("jae             1b") \
            /* 1x block of 4-butterfly */ \
            __ASM_EMIT("2:") \
            __ASM_EMIT("add             $1, %[nb]") \
            __ASM_EMIT("jl              4f") \
                /* Load data to registers */ \
                __ASM_EMIT("vmovups         0x00(%[dst], %[off]), %%xmm0")          /* xmm0 = r0  r1  r2  r3 */ \
                __ASM_EMIT("vmovups         0x10(%[dst], %[off]), %%xmm2")          /* xmm2 = r4  r5  r6  r7 */ \
                __ASM_EMIT("vmovups         0x20(%[dst], %[off]), %%xmm1")          /* xmm1 = i0  i1  i2  i3 */ \
                __ASM_EMIT("vmovups         0x30(%[dst], %[off]), %%xmm3")          /* xmm3 = i4  i5  i6  i7 */ \
                /* Perform 3rd-order butterflies */ \
                __ASM_EMIT("vsubps          %%xmm2, %%xmm0, %%xmm4")                /* xmm4 = c_re  = a_re - b_re */ \
                __ASM_EMIT("vsubps          %%xmm3, %%xmm1, %%xmm5")                /* xmm5 = c_im  = a_im - b_im */ \
                __ASM_EMIT("vaddps          %%xmm2, %%xmm0, %%xmm0")                /* xmm0 = a_re' = a_re + b_re */ \
                __ASM_EMIT("vaddps          %%xmm3, %%xmm1, %%xmm1")                /* xmm1 = a_im' = a_im + b_im */ \
                __ASM_EMIT("vmulps          0x20 + %[FFT_A], %%xmm4, %%xmm2")       /* xmm2 = x_im * c_re */ \
                __ASM_EMIT("vmulps          0x20 + %[FFT_A], %%xmm5, %%xmm3")       /* xmm3 = x_im * c_im */ \
                __ASM_EMIT(FMA_SEL("vmulps  0x00 + %[FFT_A], %%xmm4, %%xmm4", ""))  /* xmm4 = x_re * c_re */ \
                __ASM_EMIT(FMA_SEL("vmulps  0x00 + %[FFT_A], %%xmm5, %%xmm5", ""))  /* xmm5 = x_re * c_im */ \
                __ASM_EMIT(FMA_SEL("vaddps  %%xmm3, %%xmm4, %%xmm4", "vfmadd132ps 0x00 + %[FFT_A], %%xmm3, %%xmm4")) /* xmm4 = b_re = x_re * c_re + x_im * c_im */ \
                __ASM_EMIT(FMA_SEL("vsubps  %%xmm2, %%xmm5, %%xmm5", "vfmsub132ps 0x00 + %[FFT_A], %%xmm2, %%xmm5")) /* xmm5 = b_im = x_re * c_im - x_im * c_re */ \
                /* 2nd-order butterflies */ \
                __ASM_EMIT("vshufps         $0xd8, %%xmm0, %%xmm0, %%xmm0")         /* xmm0 = r0 r2 r1 r3 */ \
                __ASM_EMIT("vshufps         $0xd8, %%xmm1, %%xmm1, %%xmm1")         /* xmm1 = i0 i2 i1 i3 */ \
                __ASM_EMIT("vshufps         $0xd8, %%xmm4, %%xmm4, %%xmm4") \
                __ASM_EMIT("vshufps         $0xd8, %%xmm5, %%xmm5, %%xmm5") \
                __ASM_EMIT("vhsubps         %%xmm1, %%xmm0, %%xmm2")                /* xmm2 = r0-r2 r1-r3 i0-i2 i1-i3 = r1' r3' i1' i3' */ \
                __ASM_EMIT("vhsubps         %%xmm5, %%xmm4, %%xmm3") \
                __ASM_EMIT("vhaddps         %%xmm1, %%xmm0, %%xmm0")                /* xmm0 = r0+r2 r1+r3 i0+i2 i1+i3 = r0' r2' i0' i2' */ \
                __ASM_EMIT("vhaddps         %%xmm5, %%xmm4, %%xmm4") \
                /* 1st-order 8x butterfly */ \
                __ASM_EMIT("vshufps         $0x6e, %%xmm2, %%xmm0, %%xmm1")         /* xmm0 = i0' i2' i1' r3' */ \
                __ASM_EMIT("vshufps         $0x6e, %%xmm3, %%xmm4, %%xmm5") \
                __ASM_EMIT("vshufps         $0xc4, %%xmm2, %%xmm0, %%xmm0")         /* xmm1 = r0' r2' r1' i3' */ \
                __ASM_EMIT("vshufps         $0xc4, %%xmm3, %%xmm4, %%xmm4") \
                __ASM_EMIT("vhsubps         %%xmm1, %%xmm0, %%xmm2")                /* xmm2 = r0'-r2' r1'-i3' i0'-i2' i1'-r3' = r1" r3" i1" i2" */ \
                __ASM_EMIT("vhsubps         %%xmm5, %%xmm4, %%xmm3") \
                __ASM_EMIT("vhaddps         %%xmm1, %%xmm0, %%xmm0")                /* xmm0 = r0'+r2' r1'+i3' i0'+i2' i1'+r3' = r0" r2" i0" i3" */ \
                __ASM_EMIT("vhaddps         %%xmm5, %%xmm4, %%xmm4") \
                __ASM_EMIT("vblendps        $0x88, %%xmm0, %%xmm2, %%xmm1")         /* xmm1 = r1" r3" i1" i3" */ \
                __ASM_EMIT("vblendps        $0x88, %%xmm4, %%xmm3, %%xmm5") \
                __ASM_EMIT("vblendps        $0x88, %%xmm2, %%xmm0, %%xmm0")         /* xmm0 = r0" r2" i0" i2" */ \
                __ASM_EMIT("vblendps        $0x88, %%xmm3, %%xmm4, %%xmm4") \
                __ASM_EMIT("vunpckhps       %%xmm1, %%xmm0, %%xmm2")                /* xmm2 = i0" i1" i2" i3" */ \
                __ASM_EMIT("vunpckhps       %%xmm5, %%xmm4, %%xmm6") \
                __ASM_EMIT("vunpcklps       %%xmm1, %%xmm0, %%xmm0")                /* xmm0 = r0" r1" r2" r3" */ \
                __ASM_EMIT("vunpcklps       %%xmm5, %%xmm4, %%xmm4") \
                /* Reshuffle */ \
                __ASM_EMIT("vinsertf128     $1, %%xmm4, %%ymm0, %%ymm0")            /* ymm0 = r0 r4 */ \
                __ASM_EMIT("vinsertf128     $1, %%xmm6, %%ymm2, %%ymm2")            /* ymm2 = i0 i4 */ \
                /* Apply convolution */ \
                __ASM_EMIT("vmulps          0x20(%[conv], %[off]), %%ymm0, %%ymm1")             /* ymm1 = r0*I */ \
                __ASM_EMIT("vmulps          0x20(%[conv], %[off]), %%ymm2, %%ymm3")             /* ymm3 = i0*I */ \
                __ASM_EMIT(FMA_SEL("vmulps  0x00(%[conv], %[off]), %%ymm0, %%ymm0", ""))        /* ymm0 = r0*R */ \
                __ASM_EMIT(FMA_SEL("vmulps  0x00(%[conv], %[off]), %%ymm2, %%ymm2", ""))        /* ymm2 = i0*R */ \
                __ASM_EMIT(FMA_SEL("vsubps  %%ymm3, %%ymm0, %%ymm0", "vfmsub132ps 0x00(%[conv], %[off]), %%ymm3, %%ymm0"))  /* ymm0 = r0*R - i0*I */ \
                __ASM_EMIT(FMA_SEL("vaddps  %%ymm1, %%ymm2, %%ymm2", "vfmadd132ps 0x00(%[conv], %[off]), %%ymm1, %%ymm2"))  /* ymm2 = r0*I + i0*R */ \
                /* Reshuffle */ \
                __ASM_EMIT("vextractf128    $1, %%ymm0, %%xmm4")                    /* xmm4 = r4 */ \
                __ASM_EMIT("vextractf128    $1, %%ymm2, %%xmm6")                    /* xmm6 = i4 */ \
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
                __ASM_EMIT("vmovups         %%xmm2, 0x00(%[dst], %[off])") \
                __ASM_EMIT("vmovups         %%xmm0, 0x10(%[dst], %[off])") \
                __ASM_EMIT("vmovups         %%xmm3, 0x20(%[dst], %[off])") \
                __ASM_EMIT("vmovups         %%xmm1, 0x30(%[dst], %[off])") \
            __ASM_EMIT("4:")  \
            : [off] "=&r" (off), [nb] "+r" (nb) \
            : [dst] "r" (dst), [conv] "r" (conv), \
              [FFT_A] "o" (FFT_A) \
            : "cc", "memory", \
              "%xmm0", "%xmm1", "%xmm2", "%xmm3", \
              "%xmm4", "%xmm5", "%xmm6", "%xmm7" \
        )

    static inline void fastconv_apply_prepare(float *dst, const float *c1, const float *c2, size_t nb)
    {
        FASTCONV_APPLY_PREPARE_CORE(FMA_OFF);
    }

    static inline void fastconv_apply_prepare_fma3(float *dst, const float *c1, const float *c2, size_t nb)
    {
        FASTCONV_APPLY_PREPARE_CORE(FMA_ON);
    }

    static inline void fastconv_apply_internal(float *dst, const float *conv, size_t nb)
    {
        FASTCONV_APPLY_CORE(FMA_OFF);
    }

    static inline void fastconv_apply_internal_fma3(float *dst, const float *conv, size_t nb)
    {
        FASTCONV_APPLY_CORE(FMA_ON);
    }

    #undef FASTCONV_APPLY_PREPARE_CORE
    #undef FASTCONV_APPLY_CORE
    #undef FMA_ON
    #undef FMA_OFF
}

#endif /* DSP_ARCH_X86_AVX_FASTCONV_APPLY_H_ */
