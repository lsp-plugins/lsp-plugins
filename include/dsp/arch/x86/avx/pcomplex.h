/*
 * pcomplex.h
 *
 *  Created on: 26 мая 2019 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_AVX_PCOMPLEX_H_
#define DSP_ARCH_X86_AVX_PCOMPLEX_H_

namespace avx
{
    #define FMA_OFF(a, b)       a
    #define FMA_ON(a, b)        b

    #define PCOMPLEX_MULX_CORE(DST, SRC1, SRC2, SEL) \
        /* Init */ \
        __ASM_EMIT("xor         %[off], %[off]") \
        /* x8 block */ \
        __ASM_EMIT("sub         $8, %[count]") \
        __ASM_EMIT("jb          2f") \
        __ASM_EMIT("1:") \
        __ASM_EMIT("vmovsldup   0x00(%[" SRC1 "], %[off]), %%ymm0")     /* ymm0 = ar0 ar0 */ \
        __ASM_EMIT("vmovsldup   0x20(%[" SRC1 "], %[off]), %%ymm1") \
        __ASM_EMIT("vmovups     0x00(%[" SRC2 "], %[off]), %%ymm6")     /* ymm6 = br0 bi0 */ \
        __ASM_EMIT("vmovups     0x20(%[" SRC2 "], %[off]), %%ymm7") \
        __ASM_EMIT("vshufps     $0xb1, %%ymm6, %%ymm6, %%ymm4")         /* ymm4 = bi0 br0 */ \
        __ASM_EMIT("vmovshdup   0x00(%[" SRC1 "], %[off]), %%ymm2")     /* ymm2 = ai0 ai0 */ \
        __ASM_EMIT("vmovshdup   0x20(%[" SRC1 "], %[off]), %%ymm3") \
        __ASM_EMIT("vshufps     $0xb1, %%ymm7, %%ymm7, %%ymm5") \
        __ASM_EMIT(SEL("vmulps      %%ymm6, %%ymm0, %%ymm0", ""))       /* ymm0 = ar0*br0 ar0*bi0 */ \
        __ASM_EMIT(SEL("vmulps      %%ymm7, %%ymm1, %%ymm1", "")) \
        __ASM_EMIT("vmulps      %%ymm4, %%ymm2, %%ymm2")                /* ymm2 = ai0*bi0 ai0*br0 */ \
        __ASM_EMIT("vmulps      %%ymm5, %%ymm3, %%ymm3") \
        __ASM_EMIT(SEL("vaddsubps   %%ymm2, %%ymm0, %%ymm0", "vfmaddsub132ps %%ymm6, %%ymm2, %%ymm0"))   /* ymm0 = ar0*br0-ai0*bi0 ar0*bi0+ai0*br0 ... */ \
        __ASM_EMIT(SEL("vaddsubps   %%ymm3, %%ymm1, %%ymm1", "vfmaddsub132ps %%ymm7, %%ymm3, %%ymm1")) \
        __ASM_EMIT("vmovups     %%ymm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%ymm1, 0x20(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x40, %[off]") \
        __ASM_EMIT("sub         $8, %[count]") \
        __ASM_EMIT("jae         1b") \
        __ASM_EMIT("2:") \
        /* x4 block */ \
        __ASM_EMIT("add         $4, %[count]") \
        __ASM_EMIT("jl          4f") \
        __ASM_EMIT("vmovups     0x00(%[" SRC2 "], %[off]), %%ymm6")     /* ymm6 = br0 bi0 */ \
        __ASM_EMIT("vmovsldup   0x00(%[" SRC1 "], %[off]), %%ymm0")     /* ymm0 = ar0 ar0 */ \
        __ASM_EMIT("vmovshdup   0x00(%[" SRC1 "], %[off]), %%ymm2")     /* ymm2 = ai0 ai0 */ \
        __ASM_EMIT("vshufps     $0xb1, %%ymm6, %%ymm6, %%ymm4")         /* xmm4 = bi0 br0 */ \
        __ASM_EMIT(SEL("vmulps      %%ymm6, %%ymm0, %%ymm0", ""))       /* ymm0 = ar0*br0 ar0*bi0 */ \
        __ASM_EMIT("vmulps      %%ymm4, %%ymm2, %%ymm2")                /* ymm2 = ai0*bi0 ai0*br0 */ \
        __ASM_EMIT(SEL("vaddsubps   %%ymm2, %%ymm0, %%ymm0", "vfmaddsub132ps %%ymm6, %%ymm2, %%ymm0"))   /* ymm0 = ar0*br0-ai0*bi0 ar0*bi0+ai0*br0 ... */ \
        __ASM_EMIT("vmovups     %%ymm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x20, %[off]") \
        __ASM_EMIT("sub         $4, %[count]") \
        __ASM_EMIT("4:") \
        /* x2 block */ \
        __ASM_EMIT("add         $2, %[count]") \
        __ASM_EMIT("jl          6f") \
        __ASM_EMIT("vmovups     0x00(%[" SRC2 "], %[off]), %%xmm6")     /* xmm6 = br0 bi0 */ \
        __ASM_EMIT("vmovsldup   0x00(%[" SRC1 "], %[off]), %%xmm0")     /* xmm0 = ar0 ar0 */ \
        __ASM_EMIT("vmovshdup   0x00(%[" SRC1 "], %[off]), %%xmm2")     /* xmm2 = ai0 ai0 */ \
        __ASM_EMIT("vshufps     $0xb1, %%xmm6, %%xmm6, %%xmm4")         /* xmm4 = bi0 br0 */ \
        __ASM_EMIT(SEL("vmulps      %%xmm6, %%xmm0, %%xmm0", ""))       /* xmm0 = ar0*br0 ar0*bi0 */ \
        __ASM_EMIT("vmulps      %%xmm4, %%xmm2, %%xmm2")                /* xmm2 = ai0*bi0 ai0*br0 */ \
        __ASM_EMIT(SEL("vaddsubps   %%xmm2, %%xmm0, %%xmm0", "vfmaddsub132ps %%xmm6, %%xmm2, %%xmm0"))   /* xmm0 = ar0*br0-ai0*bi0 ar0*bi0+ai0*br0 ... */ \
        __ASM_EMIT("vmovups     %%xmm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x10, %[off]") \
        __ASM_EMIT("sub         $2, %[count]") \
        __ASM_EMIT("6:") \
        /* x1 block */ \
        __ASM_EMIT("add         $1, %[count]") \
        __ASM_EMIT("jl          8f") \
        __ASM_EMIT("vmovlps     0x00(%[" SRC1 "], %[off]), %%xmm4, %%xmm4")     /* xmm4 = ar0 ai0 */ \
        __ASM_EMIT("vmovlps     0x00(%[" SRC2 "], %[off]), %%xmm6, %%xmm6")     /* xmm6 = br0 bi0 */ \
        __ASM_EMIT("vmovsldup   %%xmm4, %%xmm0")                        /* xmm0 = ar0 ar0 */ \
        __ASM_EMIT("vmovshdup   %%xmm4, %%xmm2")                        /* xmm2 = ai0 ai0 */ \
        __ASM_EMIT("vshufps     $0xb1, %%xmm6, %%xmm6, %%xmm4")         /* xmm4 = bi0 br0 */ \
        __ASM_EMIT(SEL("vmulps      %%xmm6, %%xmm0, %%xmm0", ""))       /* xmm0 = ar0*br0 ar0*bi0 */ \
        __ASM_EMIT("vmulps      %%xmm4, %%xmm2, %%xmm2")                /* xmm2 = ai0*bi0 ai0*br0 */ \
        __ASM_EMIT(SEL("vaddsubps   %%xmm2, %%xmm0, %%xmm0", "vfmaddsub132ps %%xmm6, %%xmm2, %%xmm0"))   /* xmm0 = ar0*br0-ai0*bi0 ar0*bi0+ai0*br0 ... */ \
        __ASM_EMIT("vmovlps     %%xmm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("8:") \
        /* end */

    /*
     * Register allocation:
     * ymm4     = a1
     * ymm5     = a2
     * ymm6     = b1
     * ymm7     = b2
     */
    void pcomplex_mul3(float *dst, const float *src1, const float *src2, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            PCOMPLEX_MULX_CORE("dst", "src1", "src2", FMA_OFF)
            : [off] "=&r" (off),
              [count] "+r" (count)
            : [dst] "r" (dst), [src1] "r" (src1), [src2] "r" (src2)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void pcomplex_mul2(float *dst, const float *src, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            PCOMPLEX_MULX_CORE("dst", "dst", "src", FMA_OFF)
            : [off] "=&r" (off),
              [count] "+r" (count)
            : [dst] "r" (dst), [src] "r" (src)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void pcomplex_mul3_fma3(float *dst, const float *src1, const float *src2, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            PCOMPLEX_MULX_CORE("dst", "src1", "src2", FMA_ON)
            : [off] "=&r" (off),
              [count] "+r" (count)
            : [dst] "r" (dst), [src1] "r" (src1), [src2] "r" (src2)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void pcomplex_mul2_fma3(float *dst, const float *src, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
                PCOMPLEX_MULX_CORE("dst", "dst", "src", FMA_ON)
            : [off] "=&r" (off),
              [count] "+r" (count)
            : [dst] "r" (dst), [src] "r" (src)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    #undef PCOMPLEX_MULX_CORE

    void pcomplex_mod(float *dst, const float *src, size_t count)
    {
        // Strided-load technique is used to improve performance
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            __ASM_EMIT("xor             %[off], %[off]")
            // 16x blocks
            __ASM_EMIT("sub             $16, %[count]")
            __ASM_EMIT("jb              2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vmovups         0x000(%[src], %[off], 2), %%xmm0")
            __ASM_EMIT("vmovups         0x010(%[src], %[off], 2), %%xmm2")
            __ASM_EMIT("vinsertf128     $1, 0x020(%[src], %[off], 2), %%ymm0, %%ymm0")
            __ASM_EMIT("vinsertf128     $1, 0x030(%[src], %[off], 2), %%ymm2, %%ymm2")
            __ASM_EMIT("vmovups         0x040(%[src], %[off], 2), %%xmm1")
            __ASM_EMIT("vmovups         0x050(%[src], %[off], 2), %%xmm3")
            __ASM_EMIT("vinsertf128     $1, 0x060(%[src], %[off], 2), %%ymm1, %%ymm1")
            __ASM_EMIT("vinsertf128     $1, 0x070(%[src], %[off], 2), %%ymm3, %%ymm3")
            __ASM_EMIT("vmulps          %%ymm0, %%ymm0, %%ymm0")
            __ASM_EMIT("vmulps          %%ymm2, %%ymm2, %%ymm2")
            __ASM_EMIT("vmulps          %%ymm1, %%ymm1, %%ymm1")
            __ASM_EMIT("vmulps          %%ymm3, %%ymm3, %%ymm3")
            __ASM_EMIT("vhaddps         %%ymm2, %%ymm0, %%ymm0")
            __ASM_EMIT("vhaddps         %%ymm3, %%ymm1, %%ymm1")
            __ASM_EMIT("vsqrtps         %%ymm0, %%ymm0")
            __ASM_EMIT("vsqrtps         %%ymm1, %%ymm1")
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("vmovups         %%ymm1, 0x20(%[dst], %[off])")
            __ASM_EMIT("add             $0x40, %[off]")
            __ASM_EMIT("sub             $16, %[count]")
            __ASM_EMIT("jae             1b")
            __ASM_EMIT("2:")
            // 8x block
            __ASM_EMIT("add             $8, %[count]")
            __ASM_EMIT("jl              4f")
            __ASM_EMIT("vmovups         0x000(%[src], %[off], 2), %%xmm0")
            __ASM_EMIT("vmovups         0x010(%[src], %[off], 2), %%xmm2")
            __ASM_EMIT("vinsertf128     $1, 0x020(%[src], %[off], 2), %%ymm0, %%ymm0")
            __ASM_EMIT("vinsertf128     $1, 0x030(%[src], %[off], 2), %%ymm2, %%ymm2")
            __ASM_EMIT("vmulps          %%ymm0, %%ymm0, %%ymm0")
            __ASM_EMIT("vmulps          %%ymm2, %%ymm2, %%ymm2")
            __ASM_EMIT("vhaddps         %%ymm2, %%ymm0, %%ymm0")
            __ASM_EMIT("vsqrtps         %%ymm0, %%ymm0")
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("sub             $8, %[count]")
            __ASM_EMIT("add             $0x20, %[off]")
            __ASM_EMIT("4:")
            // 4x block
            __ASM_EMIT("add             $4, %[count]")
            __ASM_EMIT("jl              6f")
            __ASM_EMIT("5:")
            __ASM_EMIT("vmovups         0x000(%[src], %[off], 2), %%xmm0")
            __ASM_EMIT("vmovups         0x010(%[src], %[off], 2), %%xmm2")
            __ASM_EMIT("vmulps          %%xmm0, %%xmm0, %%xmm0")
            __ASM_EMIT("vmulps          %%xmm2, %%xmm2, %%xmm2")
            __ASM_EMIT("vhaddps         %%xmm2, %%xmm0, %%xmm0")
            __ASM_EMIT("vsqrtps         %%xmm0, %%xmm0")
            __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("sub             $4, %[count]")
            __ASM_EMIT("add             $0x10, %[off]")
            __ASM_EMIT("6:")
            // 2x block
            __ASM_EMIT("add             $2, %[count]")
            __ASM_EMIT("jl              8f")
            __ASM_EMIT("vmovups         0x000(%[src], %[off], 2), %%xmm0")
            __ASM_EMIT("vmulps          %%xmm0, %%xmm0, %%xmm0")
            __ASM_EMIT("vhaddps         %%xmm0, %%xmm0, %%xmm0")
            __ASM_EMIT("vsqrtps         %%xmm0, %%xmm0")
            __ASM_EMIT("vmovlps         %%xmm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("sub             $2, %[count]")
            __ASM_EMIT("add             $0x08, %[off]")
            __ASM_EMIT("8:")
            // 1x block
            __ASM_EMIT("add             $1, %[count]")
            __ASM_EMIT("jl              10f")
            __ASM_EMIT("vmovlps         0x000(%[src], %[off], 2), %%xmm0, %%xmm0")
            __ASM_EMIT("vmulps          %%xmm0, %%xmm0, %%xmm0")
            __ASM_EMIT("vhaddps         %%xmm0, %%xmm0, %%xmm0")
            __ASM_EMIT("vsqrtps         %%xmm0, %%xmm0")
            __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("10:")
            // End

            : [count] "+r" (count), [off] "=&r" (off)
            : [dst] "r" (dst), [src] "r" (src)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    #define PCOMPLEX_DIV_CORE(DST, T, B, SEL) \
        /* Init */ \
        __ASM_EMIT("xor         %[off], %[off]") \
        /* x8 block */ \
        __ASM_EMIT("sub         $8, %[count]") \
        __ASM_EMIT("jb          2f") \
        __ASM_EMIT("1:") \
        __ASM_EMIT("vmovups     0x00(%[" B "], %[off]), %%ymm6")        /* ymm6 = br0 bi0 */ \
        __ASM_EMIT("vmovups     0x20(%[" B "], %[off]), %%ymm7") \
        __ASM_EMIT("vmovaps     %[ONE], %%ymm3")                        /* ymm3 = 1 */ \
        __ASM_EMIT("vmulps      %%ymm6, %%ymm6, %%ymm4")                /* ymm4 = br0*br0 bi0*bi0 */ \
        __ASM_EMIT("vmulps      %%ymm7, %%ymm7, %%ymm5") \
        __ASM_EMIT("vhaddps     %%ymm5, %%ymm4, %%ymm2")                /* ymm2 = R = br0*br0+bi0*bi0 */ \
        __ASM_EMIT("vmovsldup   0x00(%[" T "], %[off]), %%ymm0")        /* ymm0 = ar0 ar0 */ \
        __ASM_EMIT("vdivps      %%ymm2, %%ymm3, %%ymm3")                /* ymm3 = 1/R */ \
        __ASM_EMIT("vmovsldup   0x20(%[" T "], %[off]), %%ymm1") \
        __ASM_EMIT("vxorps      %[SIGN], %%ymm3, %%ymm5")               /* ymm5 = -1/R */ \
        __ASM_EMIT("vmovshdup   0x00(%[" T "], %[off]), %%ymm2")        /* ymm2 = ai0 ai0 */ \
        __ASM_EMIT("vunpcklps   %%ymm5, %%ymm3, %%ymm4")                /* ymm4 = 1/R -1/R */ \
        __ASM_EMIT("vmulps      %%ymm6, %%ymm0, %%ymm0")                /* ymm0 = ar0*br0 ar0*bi0 */ \
        __ASM_EMIT("vunpckhps   %%ymm5, %%ymm3, %%ymm5") \
        __ASM_EMIT("vmulps      %%ymm7, %%ymm1, %%ymm1") \
        __ASM_EMIT("vmovshdup   0x20(%[" T "], %[off]), %%ymm3") \
        __ASM_EMIT("vshufps     $0xb1, %%ymm6, %%ymm6, %%ymm6")         /* ymm6 = bi0 br0 */ \
        __ASM_EMIT("vshufps     $0xb1, %%ymm7, %%ymm7, %%ymm7") \
        __ASM_EMIT(SEL("vmulps  %%ymm6, %%ymm2, %%ymm2", ""))           /* ymm2 = ai0*bi0 ai0*br0 */ \
        __ASM_EMIT(SEL("vmulps  %%ymm7, %%ymm3, %%ymm3", "")) \
        __ASM_EMIT(SEL("vaddps  %%ymm2, %%ymm0, %%ymm0", "vfmadd231ps %%ymm6, %%ymm2, %%ymm0")) /* ymm0 = r i = ar0*br0+ai0*bi0 ar0*bi0+ai0*br0 ... */ \
        __ASM_EMIT(SEL("vaddps  %%ymm3, %%ymm1, %%ymm1", "vfmadd231ps %%ymm7, %%ymm3, %%ymm1")) \
        __ASM_EMIT("vmulps      %%ymm4, %%ymm0, %%ymm0")                /* ymm0 = r/R -i/R */ \
        __ASM_EMIT("vmulps      %%ymm5, %%ymm1, %%ymm1") \
        __ASM_EMIT("vmovups     %%ymm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%ymm1, 0x20(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x40, %[off]") \
        __ASM_EMIT("sub         $8, %[count]") \
        __ASM_EMIT("jae         1b") \
        __ASM_EMIT("2:") \
        /* x4 block */ \
        __ASM_EMIT("add         $4, %[count]") \
        __ASM_EMIT("jl          4f") \
        __ASM_EMIT("vmovups     0x00(%[" B "], %[off]), %%ymm6")        /* ymm6 = br0 bi0 */ \
        __ASM_EMIT("vmovaps     %[ONE], %%ymm3")                        /* ymm3 = 1 */ \
        __ASM_EMIT("vmulps      %%ymm6, %%ymm6, %%ymm4")                /* ymm4 = br0*br0 bi0*bi0 */ \
        __ASM_EMIT("vhaddps     %%ymm5, %%ymm4, %%ymm2")                /* ymm2 = R = br0*br0+bi0*bi0 */ \
        __ASM_EMIT("vdivps      %%ymm2, %%ymm3, %%ymm3")                /* ymm3 = 1/R */ \
        __ASM_EMIT("vmovsldup   0x00(%[" T "], %[off]), %%ymm0")        /* ymm0 = ar0 ar0 */ \
        __ASM_EMIT("vxorps      %[SIGN], %%ymm3, %%ymm5")               /* ymm5 = -1/R */ \
        __ASM_EMIT("vmovshdup   0x00(%[" T "], %[off]), %%ymm2")        /* ymm2 = ai0 ai0 */ \
        __ASM_EMIT("vunpcklps   %%ymm5, %%ymm3, %%ymm4")                /* ymm4 = 1/R -1/R */ \
        __ASM_EMIT("vmulps      %%ymm6, %%ymm0, %%ymm0")                /* ymm0 = ar0*br0 ar0*bi0 */ \
        __ASM_EMIT("vunpckhps   %%ymm5, %%ymm3, %%ymm5") \
        __ASM_EMIT("vshufps     $0xb1, %%ymm6, %%ymm6, %%ymm6")         /* ymm6 = bi0 br0 */ \
        __ASM_EMIT(SEL("vmulps  %%ymm6, %%ymm2, %%ymm2", ""))           /* ymm2 = ai0*bi0 ai0*br0 */ \
        __ASM_EMIT(SEL("vaddps  %%ymm2, %%ymm0, %%ymm0", "vfmadd231ps %%ymm6, %%ymm2, %%ymm0")) /* ymm0 = r i = ar0*br0+ai0*bi0 ar0*bi0+ai0*br0 ... */ \
        __ASM_EMIT("vmulps      %%ymm4, %%ymm0, %%ymm0")                /* ymm0 = r/R -i/R */ \
        __ASM_EMIT("vmovups     %%ymm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x20, %[off]") \
        __ASM_EMIT("sub         $4, %[count]") \
        __ASM_EMIT("4:") \
        /* x2 block */ \
        __ASM_EMIT("add         $2, %[count]") \
        __ASM_EMIT("jl          6f") \
        __ASM_EMIT("vmovups     0x00(%[" B "], %[off]), %%xmm6")        /* xmm6 = br0 bi0 */ \
        __ASM_EMIT("vmovaps     %[ONE], %%xmm3")                        /* xmm3 = 1 */ \
        __ASM_EMIT("vmulps      %%xmm6, %%xmm6, %%xmm4")                /* xmm4 = br0*br0 bi0*bi0 */ \
        __ASM_EMIT("vhaddps     %%xmm5, %%xmm4, %%xmm2")                /* xmm2 = R = br0*br0+bi0*bi0 */ \
        __ASM_EMIT("vmovsldup   0x00(%[" T "], %[off]), %%xmm0")        /* xmm0 = ar0 ar0 */ \
        __ASM_EMIT("vdivps      %%xmm2, %%xmm3, %%xmm3")                /* xmm3 = 1/R */ \
        __ASM_EMIT("vmovshdup   0x00(%[" T "], %[off]), %%xmm2")        /* xmm2 = ai0 ai0 */ \
        __ASM_EMIT("vxorps      %[SIGN], %%xmm3, %%xmm5")               /* xmm5 = -1/R */ \
        __ASM_EMIT("vmulps      %%xmm6, %%xmm0, %%xmm0")                /* xmm0 = ar0*br0 ar0*bi0 */ \
        __ASM_EMIT("vunpcklps   %%xmm5, %%xmm3, %%xmm4")                /* xmm4 = 1/R -1/R */ \
        __ASM_EMIT("vshufps     $0xb1, %%xmm6, %%xmm6, %%xmm6")         /* xmm6 = bi0 br0 */ \
        __ASM_EMIT(SEL("vmulps  %%xmm6, %%xmm2, %%xmm2", ""))           /* xmm2 = ai0*bi0 ai0*br0 */ \
        __ASM_EMIT(SEL("vaddps  %%xmm2, %%xmm0, %%xmm0", "vfmadd231ps %%xmm6, %%xmm2, %%xmm0")) /* xmm0 = r i = ar0*br0+ai0*bi0 ar0*bi0+ai0*br0 ... */ \
        __ASM_EMIT("vmulps      %%xmm4, %%xmm0, %%xmm0")                /* xmm0 = r/R -i/R */ \
        __ASM_EMIT("vmovups     %%xmm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x10, %[off]") \
        __ASM_EMIT("sub         $2, %[count]") \
        __ASM_EMIT("6:") \
        /* x1 block */ \
        __ASM_EMIT("add         $1, %[count]") \
        __ASM_EMIT("jl          8f") \
        __ASM_EMIT("vmovlps     0x00(%[" B "], %[off]), %%xmm6, %%xmm6")/* xmm6 = br0 bi0 */ \
        __ASM_EMIT("vmovaps     %[ONE], %%xmm3")                        /* xmm3 = 1 */ \
        __ASM_EMIT("vmulps      %%xmm6, %%xmm6, %%xmm4")                /* xmm4 = br0*br0 bi0*bi0 */ \
        __ASM_EMIT("vhaddps     %%xmm5, %%xmm4, %%xmm2")                /* xmm2 = R = br0*br0+bi0*bi0 */ \
        __ASM_EMIT("vdivps      %%xmm2, %%xmm3, %%xmm3")                /* xmm3 = 1/R */ \
        __ASM_EMIT("vxorps      %[SIGN], %%xmm3, %%xmm5")               /* xmm5 = -1/R */ \
        __ASM_EMIT("vmovlps     0x00(%[" T "], %[off]), %%xmm2, %%xmm2") \
        __ASM_EMIT("vunpcklps   %%xmm5, %%xmm3, %%xmm4")                /* xmm4 = 1/R -1/R */ \
        __ASM_EMIT("vmovsldup   %%xmm2, %%xmm0")                        /* xmm0 = ar0 ar0 */ \
        __ASM_EMIT("vmovshdup   %%xmm2, %%xmm2")                        /* xmm2 = ai0 ai0 */ \
        __ASM_EMIT("vmulps      %%xmm6, %%xmm0, %%xmm0")                /* xmm0 = ar0*br0 ar0*bi0 */ \
        __ASM_EMIT("vshufps     $0xb1, %%xmm6, %%xmm6, %%xmm6")         /* xmm6 = bi0 br0 */ \
        __ASM_EMIT(SEL("vmulps  %%xmm6, %%xmm2, %%xmm2", ""))           /* xmm2 = ai0*bi0 ai0*br0 */ \
        __ASM_EMIT(SEL("vaddps  %%xmm2, %%xmm0, %%xmm0", "vfmadd231ps %%xmm6, %%xmm2, %%xmm0")) /* xmm0 = r i = ar0*br0+ai0*bi0 ar0*bi0+ai0*br0 ... */ \
        __ASM_EMIT("vmulps      %%xmm4, %%xmm0, %%xmm0")                /* xmm0 = r/R -i/R */ \
        __ASM_EMIT("vmovlps     %%xmm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("8:") \
        /* end */

    void pcomplex_div3(float *dst, const float *t, const float *b, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            PCOMPLEX_DIV_CORE("dst", "t", "b", FMA_OFF)
            : [off] "=&r" (off),
              [count] "+r" (count)
            : [dst] "r" (dst), [t] "r" (t), [b] "r" (b),
              [ONE] "m" (ONE),
              [SIGN] "m" (R_SIGN)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void pcomplex_div2(float *dst, const float *src, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            PCOMPLEX_DIV_CORE("dst", "dst", "src", FMA_OFF)
            : [off] "=&r" (off),
              [count] "+r" (count)
            : [dst] "r" (dst), [src] "r" (src),
              [ONE] "m" (ONE),
              [SIGN] "m" (R_SIGN)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void pcomplex_rdiv2(float *dst, const float *src, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            PCOMPLEX_DIV_CORE("dst", "src", "dst", FMA_OFF)
            : [off] "=&r" (off),
              [count] "+r" (count)
            : [dst] "r" (dst), [src] "r" (src),
              [ONE] "m" (ONE),
              [SIGN] "m" (R_SIGN)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void pcomplex_div3_fma3(float *dst, const float *t, const float *b, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            PCOMPLEX_DIV_CORE("dst", "t", "b", FMA_ON)
            : [off] "=&r" (off),
              [count] "+r" (count)
            : [dst] "r" (dst), [t] "r" (t), [b] "r" (b),
              [ONE] "m" (ONE),
              [SIGN] "m" (R_SIGN)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void pcomplex_div2_fma3(float *dst, const float *src, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            PCOMPLEX_DIV_CORE("dst", "dst", "src", FMA_ON)
            : [off] "=&r" (off),
              [count] "+r" (count)
            : [dst] "r" (dst), [src] "r" (src),
              [ONE] "m" (ONE),
              [SIGN] "m" (R_SIGN)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void pcomplex_rdiv2_fma3(float *dst, const float *src, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            PCOMPLEX_DIV_CORE("dst", "src", "dst", FMA_ON)
            : [off] "=&r" (off),
              [count] "+r" (count)
            : [dst] "r" (dst), [src] "r" (src),
              [ONE] "m" (ONE),
              [SIGN] "m" (R_SIGN)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    #undef PCOMPLEX_DIV_CORE

    #define PCOMPLEX_RCP_CORE(DST, SRC) \
        /* Init */ \
        __ASM_EMIT("xor         %[off], %[off]") \
        /* x16 block */ \
        __ASM_EMIT("sub         $16, %[count]") \
        __ASM_EMIT("jb          2f") \
        __ASM_EMIT("1:") \
        __ASM_EMIT("vmovups     0x00(%[" SRC "], %[off]), %%ymm0")      /* ymm0 = r0 i0 */ \
        __ASM_EMIT("vmovups     0x20(%[" SRC "], %[off]), %%ymm1") \
        __ASM_EMIT("vmovups     0x40(%[" SRC "], %[off]), %%ymm2") \
        __ASM_EMIT("vmovups     0x60(%[" SRC "], %[off]), %%ymm3") \
        __ASM_EMIT("vmulps      %%ymm0, %%ymm0, %%ymm4")                /* ymm0 = r0*r0 + i0*i0 */ \
        __ASM_EMIT("vmulps      %%ymm1, %%ymm1, %%ymm5") \
        __ASM_EMIT("vmulps      %%ymm2, %%ymm2, %%ymm6") \
        __ASM_EMIT("vmulps      %%ymm3, %%ymm3, %%ymm7") \
        __ASM_EMIT("vhaddps     %%ymm5, %%ymm4, %%ymm4")                /* ymm4 = R = r0*r0+i0*i0 */ \
        __ASM_EMIT("vhaddps     %%ymm7, %%ymm6, %%ymm6") \
        __ASM_EMIT("vmovaps     %[ONE], %%ymm5")                        /* ymm5 = 1 */ \
        __ASM_EMIT("vdivps      %%ymm4, %%ymm5, %%ymm4")                /* ymm4 = 1/R */ \
        __ASM_EMIT("vdivps      %%ymm6, %%ymm5, %%ymm6") \
        __ASM_EMIT("vxorps      %[SIGN], %%ymm4, %%ymm5")               /* ymm5 = -1/R */ \
        __ASM_EMIT("vunpckhps   %%ymm5, %%ymm4, %%ymm7")                /* ymm7 = 1/R -1/R */ \
        __ASM_EMIT("vunpcklps   %%ymm5, %%ymm4, %%ymm5") \
        __ASM_EMIT("vmulps      %%ymm5, %%ymm0, %%ymm0")                /* ymm0 = r0/R -i0/R */ \
        __ASM_EMIT("vmulps      %%ymm7, %%ymm1, %%ymm1") \
        __ASM_EMIT("vxorps      %[SIGN], %%ymm6, %%ymm5")               /* ymm5 = -1/R2 */ \
        __ASM_EMIT("vunpckhps   %%ymm5, %%ymm6, %%ymm7")                /* ymm7 = 1/R2 -1/R2 */ \
        __ASM_EMIT("vunpcklps   %%ymm5, %%ymm6, %%ymm5") \
        __ASM_EMIT("vmulps      %%ymm5, %%ymm2, %%ymm2")                /* ymm0 = r1/R2 -i1/R2 */ \
        __ASM_EMIT("vmulps      %%ymm7, %%ymm3, %%ymm3") \
        __ASM_EMIT("vmovups     %%ymm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%ymm1, 0x20(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%ymm2, 0x40(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%ymm3, 0x60(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x80, %[off]") \
        __ASM_EMIT("sub         $16, %[count]") \
        __ASM_EMIT("jae         1b") \
        __ASM_EMIT("2:") \
        /* x8 block */ \
        __ASM_EMIT("add         $8, %[count]") \
        __ASM_EMIT("jl          4f") \
        __ASM_EMIT("vmovups     0x00(%[" SRC "], %[off]), %%ymm0")      /* ymm0 = r0 i0 */ \
        __ASM_EMIT("vmovups     0x20(%[" SRC "], %[off]), %%ymm1") \
        __ASM_EMIT("vmulps      %%ymm0, %%ymm0, %%ymm4")                /* ymm0 = r0*r0 + i0*i0 */ \
        __ASM_EMIT("vmulps      %%ymm1, %%ymm1, %%ymm5") \
        __ASM_EMIT("vhaddps     %%ymm5, %%ymm4, %%ymm4")                /* ymm4 = R = r0*r0+i0*i0 */ \
        __ASM_EMIT("vmovaps     %[ONE], %%ymm5")                        /* ymm5 = 1 */ \
        __ASM_EMIT("vdivps      %%ymm4, %%ymm5, %%ymm4")                /* ymm4 = 1/R */ \
        __ASM_EMIT("vxorps      %[SIGN], %%ymm4, %%ymm5")               /* ymm5 = -1/R */ \
        __ASM_EMIT("vunpckhps   %%ymm5, %%ymm4, %%ymm7")                /* ymm7 = 1/R -1/R */ \
        __ASM_EMIT("vunpcklps   %%ymm5, %%ymm4, %%ymm5") \
        __ASM_EMIT("vmulps      %%ymm5, %%ymm0, %%ymm0")                /* ymm0 = r0/R -i0/R */ \
        __ASM_EMIT("vmulps      %%ymm7, %%ymm1, %%ymm1") \
        __ASM_EMIT("vmovups     %%ymm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%ymm1, 0x20(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x40, %[off]") \
        __ASM_EMIT("sub         $8, %[count]") \
        __ASM_EMIT("4:") \
        /* x4 block */ \
        __ASM_EMIT("add         $4, %[count]") \
        __ASM_EMIT("jl          6f") \
        __ASM_EMIT("vmovups     0x00(%[" SRC "], %[off]), %%ymm0")      /* ymm0 = r0 i0 */ \
        __ASM_EMIT("vmulps      %%ymm0, %%ymm0, %%ymm4")                /* ymm0 = r0*r0 + i0*i0 */ \
        __ASM_EMIT("vhaddps     %%ymm4, %%ymm4, %%ymm4")                /* ymm4 = R = r0*r0+i0*i0 */ \
        __ASM_EMIT("vmovaps     %[ONE], %%ymm5")                        /* ymm5 = 1 */ \
        __ASM_EMIT("vdivps      %%ymm4, %%ymm5, %%ymm4")                /* ymm4 = 1/R */ \
        __ASM_EMIT("vxorps      %[SIGN], %%ymm4, %%ymm5")               /* ymm5 = -1/R */ \
        __ASM_EMIT("vunpcklps   %%ymm5, %%ymm4, %%ymm5")                /* ymm7 = 1/R -1/R */ \
        __ASM_EMIT("vmulps      %%ymm5, %%ymm0, %%ymm0")                /* ymm0 = r0/R -i0/R */ \
        __ASM_EMIT("vmovups     %%ymm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x20, %[off]") \
        __ASM_EMIT("sub         $4, %[count]") \
        __ASM_EMIT("6:") \
        /* x2 block */ \
        __ASM_EMIT("add         $2, %[count]") \
        __ASM_EMIT("jl          8f") \
        __ASM_EMIT("vmovups     0x00(%[" SRC "], %[off]), %%xmm0")      /* xmm0 = r0 i0 */ \
        __ASM_EMIT("vmulps      %%xmm0, %%xmm0, %%xmm4")                /* xmm0 = r0*r0 + i0*i0 */ \
        __ASM_EMIT("vhaddps     %%xmm4, %%xmm4, %%xmm4")                /* xmm4 = R = r0*r0+i0*i0 */ \
        __ASM_EMIT("vmovaps     %[ONE], %%xmm5")                        /* xmm5 = 1 */ \
        __ASM_EMIT("vdivps      %%xmm4, %%xmm5, %%xmm4")                /* xmm4 = 1/R */ \
        __ASM_EMIT("vxorps      %[SIGN], %%xmm4, %%xmm5")               /* xmm5 = -1/R */ \
        __ASM_EMIT("vunpcklps   %%xmm5, %%xmm4, %%xmm5")                /* xmm7 = 1/R -1/R */ \
        __ASM_EMIT("vmulps      %%xmm5, %%xmm0, %%xmm0")                /* xmm0 = r0/R -i0/R */ \
        __ASM_EMIT("vmovups     %%xmm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x10, %[off]") \
        __ASM_EMIT("sub         $2, %[count]") \
        __ASM_EMIT("8:") \
        /* x1 block */ \
        __ASM_EMIT("add         $1, %[count]") \
        __ASM_EMIT("jl          10f") \
        __ASM_EMIT("vmovlps     0x00(%[" SRC "], %[off]), %%xmm0, %%xmm0")  /* xmm0 = r0 i0 */ \
        __ASM_EMIT("vmulps      %%xmm0, %%xmm0, %%xmm4")                /* xmm0 = r0*r0 + i0*i0 */ \
        __ASM_EMIT("vhaddps     %%xmm4, %%xmm4, %%xmm4")                /* xmm4 = R = r0*r0+i0*i0 */ \
        __ASM_EMIT("vmovaps     %[ONE], %%xmm5")                        /* xmm5 = 1 */ \
        __ASM_EMIT("vdivps      %%xmm4, %%xmm5, %%xmm4")                /* xmm4 = 1/R */ \
        __ASM_EMIT("vxorps      %[SIGN], %%xmm4, %%xmm5")               /* xmm5 = -1/R */ \
        __ASM_EMIT("vunpcklps   %%xmm5, %%xmm4, %%xmm5")                /* xmm7 = 1/R -1/R */ \
        __ASM_EMIT("vmulps      %%xmm5, %%xmm0, %%xmm0")                /* xmm0 = r0/R -i0/R */ \
        __ASM_EMIT("vmovlps     %%xmm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("10:") \
        /* end */

    void pcomplex_rcp1(float *dst, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            PCOMPLEX_RCP_CORE("dst", "dst")
            : [off] "=&r" (off),
              [count] "+r" (count)
            : [dst] "r" (dst),
              [ONE] "m" (ONE),
              [SIGN] "m" (R_SIGN)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void pcomplex_rcp2(float *dst, const float *src, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            PCOMPLEX_RCP_CORE("dst", "src")
            : [off] "=&r" (off),
              [count] "+r" (count)
            : [dst] "r" (dst), [src] "r" (src),
              [ONE] "m" (ONE),
              [SIGN] "m" (R_SIGN)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    #undef PCOMPLEX_RCP_CORE

    #undef FMA_OFF
    #undef FMA_ON
}

#endif /* DSP_ARCH_X86_AVX_PCOMPLEX_H_ */
