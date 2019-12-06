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
        __ASM_EMIT("vmovups     0x00(%[" SRC1 "], %[off]), %%ymm4")     /* ymm4 = ar0 ai0 */ \
        __ASM_EMIT("vmovups     0x20(%[" SRC1 "], %[off]), %%ymm5") \
        __ASM_EMIT("vmovups     0x00(%[" SRC2 "], %[off]), %%ymm6")     /* ymm6 = br0 bi0 */ \
        __ASM_EMIT("vmovups     0x20(%[" SRC2 "], %[off]), %%ymm7") \
        __ASM_EMIT("vmovsldup   %%ymm4, %%ymm0")                        /* ymm0 = ar0 ar0 */ \
        __ASM_EMIT("vmovsldup   %%ymm5, %%ymm1") \
        __ASM_EMIT("vmovshdup   %%ymm4, %%ymm2")                        /* ymm2 = ai0 ai0 */ \
        __ASM_EMIT("vmovshdup   %%ymm5, %%ymm3") \
        __ASM_EMIT("vshufps     $0xb1, %%ymm6, %%ymm6, %%ymm4")         /* ymm4 = bi0 br0 */ \
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
        __ASM_EMIT("vmovups     0x00(%[" SRC1 "], %[off]), %%ymm4")     /* ymm4 = ar0 ai0 */ \
        __ASM_EMIT("vmovups     0x00(%[" SRC2 "], %[off]), %%ymm6")     /* ymm6 = br0 bi0 */ \
        __ASM_EMIT("vmovsldup   %%ymm4, %%ymm0")                        /* ymm0 = ar0 ar0 */ \
        __ASM_EMIT("vmovshdup   %%ymm4, %%ymm2")                        /* ymm2 = ai0 ai0 */ \
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
        __ASM_EMIT("vmovups     0x00(%[" SRC1 "], %[off]), %%xmm4")     /* xmm4 = ar0 ai0 */ \
        __ASM_EMIT("vmovups     0x00(%[" SRC2 "], %[off]), %%xmm6")     /* xmm6 = br0 bi0 */ \
        __ASM_EMIT("vmovsldup   %%xmm4, %%xmm0")                        /* xmm0 = ar0 ar0 */ \
        __ASM_EMIT("vmovshdup   %%xmm4, %%xmm2")                        /* xmm2 = ai0 ai0 */ \
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

    void x64_pcomplex_mod(float *dst, const float *src, size_t count)
    {
        IF_ARCH_X86_64(size_t off);

        ARCH_X86_64_ASM
        (
            __ASM_EMIT("xor             %[off], %[off]")
            __ASM_EMIT("sub             $32, %[count]")
            __ASM_EMIT("jb              2f")

            // 32x blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("vlddqu          0x000(%[src], %[off], 2), %%ymm0")
            __ASM_EMIT("vlddqu          0x020(%[src], %[off], 2), %%ymm1")
            __ASM_EMIT("vlddqu          0x040(%[src], %[off], 2), %%ymm2")
            __ASM_EMIT("vlddqu          0x060(%[src], %[off], 2), %%ymm3")
            __ASM_EMIT("vlddqu          0x080(%[src], %[off], 2), %%ymm4")
            __ASM_EMIT("vlddqu          0x0a0(%[src], %[off], 2), %%ymm5")
            __ASM_EMIT("vlddqu          0x0c0(%[src], %[off], 2), %%ymm6")
            __ASM_EMIT("vlddqu          0x0e0(%[src], %[off], 2), %%ymm7")
            __ASM_EMIT("vextractf128    $0x01, %%ymm0, %%xmm8")
            __ASM_EMIT("vextractf128    $0x01, %%ymm2, %%xmm10")
            __ASM_EMIT("vextractf128    $0x01, %%ymm4, %%xmm12")
            __ASM_EMIT("vextractf128    $0x01, %%ymm6, %%xmm14")
            __ASM_EMIT("vinsertf128     $0x01, %%xmm1, %%ymm0, %%ymm0")
            __ASM_EMIT("vinsertf128     $0x01, %%xmm3, %%ymm2, %%ymm2")
            __ASM_EMIT("vinsertf128     $0x01, %%xmm5, %%ymm4, %%ymm4")
            __ASM_EMIT("vinsertf128     $0x01, %%xmm7, %%ymm6, %%ymm6")
            __ASM_EMIT("vinsertf128     $0x00, %%xmm8,  %%ymm1, %%ymm1")
            __ASM_EMIT("vinsertf128     $0x00, %%xmm10, %%ymm3, %%ymm3")
            __ASM_EMIT("vinsertf128     $0x00, %%xmm12, %%ymm5, %%ymm5")
            __ASM_EMIT("vinsertf128     $0x00, %%xmm14, %%ymm7, %%ymm7")
            __ASM_EMIT("vmulps          %%ymm0, %%ymm0, %%ymm0")
            __ASM_EMIT("vmulps          %%ymm1, %%ymm1, %%ymm1")
            __ASM_EMIT("vmulps          %%ymm2, %%ymm2, %%ymm2")
            __ASM_EMIT("vmulps          %%ymm3, %%ymm3, %%ymm3")
            __ASM_EMIT("vmulps          %%ymm4, %%ymm4, %%ymm4")
            __ASM_EMIT("vmulps          %%ymm5, %%ymm5, %%ymm5")
            __ASM_EMIT("vmulps          %%ymm6, %%ymm6, %%ymm6")
            __ASM_EMIT("vmulps          %%ymm7, %%ymm7, %%ymm7")
            __ASM_EMIT("vhaddps         %%ymm1, %%ymm0, %%ymm0")
            __ASM_EMIT("vhaddps         %%ymm3, %%ymm2, %%ymm2")
            __ASM_EMIT("vhaddps         %%ymm5, %%ymm4, %%ymm4")
            __ASM_EMIT("vhaddps         %%ymm7, %%ymm6, %%ymm6")
            __ASM_EMIT("vsqrtps         %%ymm0, %%ymm0")
            __ASM_EMIT("vsqrtps         %%ymm2, %%ymm2")
            __ASM_EMIT("vsqrtps         %%ymm4, %%ymm4")
            __ASM_EMIT("vsqrtps         %%ymm6, %%ymm6")
            __ASM_EMIT("vmovdqu         %%ymm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("vmovdqu         %%ymm2, 0x20(%[dst], %[off])")
            __ASM_EMIT("vmovdqu         %%ymm4, 0x40(%[dst], %[off])")
            __ASM_EMIT("vmovdqu         %%ymm6, 0x60(%[dst], %[off])")
            __ASM_EMIT("add             $0x80, %[off]")
            __ASM_EMIT("sub             $32, %[count]")
            __ASM_EMIT("jae             1b")

            // 16x blocks
            __ASM_EMIT("2:")
            __ASM_EMIT("add             $16, %[count]")
            __ASM_EMIT("jl              4f")
            __ASM_EMIT("vlddqu          0x000(%[src], %[off], 2), %%ymm0")
            __ASM_EMIT("vlddqu          0x020(%[src], %[off], 2), %%ymm1")
            __ASM_EMIT("vlddqu          0x040(%[src], %[off], 2), %%ymm2")
            __ASM_EMIT("vlddqu          0x060(%[src], %[off], 2), %%ymm3")
            __ASM_EMIT("vextractf128    $0x01, %%ymm0, %%xmm4")
            __ASM_EMIT("vextractf128    $0x01, %%ymm2, %%xmm6")
            __ASM_EMIT("vinsertf128     $0x01, %%xmm1, %%ymm0, %%ymm0")
            __ASM_EMIT("vinsertf128     $0x01, %%xmm3, %%ymm2, %%ymm2")
            __ASM_EMIT("vinsertf128     $0x00, %%xmm4, %%ymm1, %%ymm1")
            __ASM_EMIT("vinsertf128     $0x00, %%xmm6, %%ymm3, %%ymm3")
            __ASM_EMIT("vmulps          %%ymm0, %%ymm0, %%ymm0")
            __ASM_EMIT("vmulps          %%ymm1, %%ymm1, %%ymm1")
            __ASM_EMIT("vmulps          %%ymm2, %%ymm2, %%ymm2")
            __ASM_EMIT("vmulps          %%ymm3, %%ymm3, %%ymm3")
            __ASM_EMIT("vhaddps         %%ymm1, %%ymm0, %%ymm0")
            __ASM_EMIT("vhaddps         %%ymm3, %%ymm2, %%ymm2")
            __ASM_EMIT("vsqrtps         %%ymm0, %%ymm0")
            __ASM_EMIT("vsqrtps         %%ymm2, %%ymm2")
            __ASM_EMIT("vmovdqu         %%ymm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("vmovdqu         %%ymm2, 0x20(%[dst], %[off])")
            __ASM_EMIT("sub             $16, %[count]")
            __ASM_EMIT("add             $0x40, %[off]")

            // 4x blocks
            __ASM_EMIT("4:")
            __ASM_EMIT("add             $12, %[count]")
            __ASM_EMIT("jl              6f")
            __ASM_EMIT("5:")
            __ASM_EMIT("vlddqu          0x00(%[src], %[off], 2), %%xmm0")
            __ASM_EMIT("vlddqu          0x10(%[src], %[off], 2), %%xmm1")
            __ASM_EMIT("vmulps          %%xmm0, %%xmm0, %%xmm0")
            __ASM_EMIT("vmulps          %%xmm1, %%xmm1, %%xmm1")
            __ASM_EMIT("vhaddps         %%xmm1, %%xmm0, %%xmm0")
            __ASM_EMIT("vsqrtps         %%xmm0, %%xmm0")
            __ASM_EMIT("vmovdqu         %%xmm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("add             $0x10, %[off]")
            __ASM_EMIT("sub             $4, %[count]")
            __ASM_EMIT("jge             5b")

            // 1x blocks
            __ASM_EMIT("6:")
            __ASM_EMIT("add             $3, %[count]")
            __ASM_EMIT("jl              8f")
            __ASM_EMIT("7:")
            __ASM_EMIT("vmovss          0x00(%[src], %[off], 2), %%xmm0")
            __ASM_EMIT("vmovss          0x04(%[src], %[off], 2), %%xmm1")
            __ASM_EMIT("vmulss          %%xmm0, %%xmm0, %%xmm0")
            __ASM_EMIT("vmulss          %%xmm1, %%xmm1, %%xmm1")
            __ASM_EMIT("vaddss          %%xmm1, %%xmm0, %%xmm0")
            __ASM_EMIT("vsqrtss         %%xmm0, %%xmm1, %%xmm0")
            __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("add             $0x4, %[off]")
            __ASM_EMIT("dec             %[count]")
            __ASM_EMIT("jge             7b")

            // End
            __ASM_EMIT("8:")

            : [count] "+r" (count), [off] "=&r" (off)
            : [dst] "r" (dst), [src] "r" (src)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5", "%xmm6", "%xmm7",
              "%xmm8", "%xmm9", "%xmm10", "%xmm12", "%xmm13", "%xmm14", "%xmm15"
        );
    }

    #undef FMA_OFF
    #undef FMA_ON
}

#endif /* DSP_ARCH_X86_AVX_PCOMPLEX_H_ */
