/*
 * copy.h
 *
 *  Created on: 4 сент. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_AVX_COPY_H_
#define DSP_ARCH_X86_AVX_COPY_H_

namespace avx
{
    void copy(float *dst, const float *src, size_t count)
    {
        size_t off;

        ARCH_X86_ASM
        (
            __ASM_EMIT("cmp         %[dst], %[src]")
            __ASM_EMIT("je          10f")

            /* Destination is aligned */
            __ASM_EMIT("xor         %[off], %[off]")
            __ASM_EMIT("sub         $0x40, %[count]")
            __ASM_EMIT("jb          2f")

            /* Block of 64 items */
            __ASM_EMIT("1:")
            __ASM_EMIT("vlddqu      0x00(%[src], %[off]), %%ymm0")
            __ASM_EMIT("vlddqu      0x20(%[src], %[off]), %%ymm1")
            __ASM_EMIT("vlddqu      0x40(%[src], %[off]), %%ymm2")
            __ASM_EMIT("vlddqu      0x60(%[src], %[off]), %%ymm3")
            __ASM_EMIT("vlddqu      0x80(%[src], %[off]), %%ymm4")
            __ASM_EMIT("vlddqu      0xa0(%[src], %[off]), %%ymm5")
            __ASM_EMIT("vlddqu      0xc0(%[src], %[off]), %%ymm6")
            __ASM_EMIT("vlddqu      0xe0(%[src], %[off]), %%ymm7")
            __ASM_EMIT("vmovdqu     %%ymm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("vmovdqu     %%ymm1, 0x20(%[dst], %[off])")
            __ASM_EMIT("vmovdqu     %%ymm2, 0x40(%[dst], %[off])")
            __ASM_EMIT("vmovdqu     %%ymm3, 0x60(%[dst], %[off])")
            __ASM_EMIT("vmovdqu     %%ymm4, 0x80(%[dst], %[off])")
            __ASM_EMIT("vmovdqu     %%ymm5, 0xa0(%[dst], %[off])")
            __ASM_EMIT("vmovdqu     %%ymm6, 0xc0(%[dst], %[off])")
            __ASM_EMIT("vmovdqu     %%ymm7, 0xe0(%[dst], %[off])")
            __ASM_EMIT("add         $0x100, %[off]")
            __ASM_EMIT("sub         $0x40, %[count]")
            __ASM_EMIT("jae         1b")
            /* Block of 32 items */
            __ASM_EMIT("2:")
            __ASM_EMIT("add         $0x20, %[count]")
            __ASM_EMIT("jl          4f")
            __ASM_EMIT("vlddqu      0x00(%[src], %[off]), %%ymm0")
            __ASM_EMIT("vlddqu      0x20(%[src], %[off]), %%ymm1")
            __ASM_EMIT("vlddqu      0x40(%[src], %[off]), %%ymm2")
            __ASM_EMIT("vlddqu      0x60(%[src], %[off]), %%ymm3")
            __ASM_EMIT("sub         $0x20, %[count]")
            __ASM_EMIT("vmovdqu     %%ymm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("vmovdqu     %%ymm1, 0x20(%[dst], %[off])")
            __ASM_EMIT("vmovdqu     %%ymm2, 0x40(%[dst], %[off])")
            __ASM_EMIT("vmovdqu     %%ymm3, 0x60(%[dst], %[off])")
            __ASM_EMIT("add         $0x80, %[off]")
            /* Block of 16 items */
            __ASM_EMIT("4:")
            __ASM_EMIT("add         $0x10, %[count]")
            __ASM_EMIT("jl          6f")
            __ASM_EMIT("vlddqu      0x00(%[src], %[off]), %%ymm0")
            __ASM_EMIT("vlddqu      0x20(%[src], %[off]), %%ymm1")
            __ASM_EMIT("sub         $0x10, %[count]")
            __ASM_EMIT("vmovdqu     %%ymm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("vmovdqu     %%ymm1, 0x20(%[dst], %[off])")
            __ASM_EMIT("add         $0x40, %[off]")
            /* Block of 8 items */
            __ASM_EMIT("6:")
            __ASM_EMIT("add         $0x8, %[count]")
            __ASM_EMIT("jl          8f")
            __ASM_EMIT("vlddqu      0x00(%[src], %[off]), %%ymm0")
            __ASM_EMIT("sub         $0x8, %[count]")
            __ASM_EMIT("vmovdqu     %%ymm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("add         $0x20, %[off]")
            /* 1x items */
            __ASM_EMIT("8:")
            __ASM_EMIT("add         $0x07, %[count]")
            __ASM_EMIT("jl          10f")
            __ASM_EMIT("9:")
            __ASM_EMIT("vmovd       0x00(%[src], %[off]), %%xmm0")
            __ASM_EMIT("vmovd       %%xmm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("add         $0x04, %[off]")
            __ASM_EMIT("dec         %[count]")
            __ASM_EMIT("jge         9b")

            __ASM_EMIT("10:")

            : [src] "+r" (src), [dst] "+r"(dst),
              [count] "+r" (count), [off] "=&r" (off)
            :
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void reverse1(float *dst, size_t count)
    {
        IF_ARCH_X86(float *src);

        ARCH_X86_ASM
        (
            __ASM_EMIT("lea         (%[dst],%[count],4), %[src]")
            __ASM_EMIT("shr         $1, %[count]")
            __ASM_EMIT("sub         $32, %[count]")
            __ASM_EMIT("jb          2f")

            /* 32x blocks */
            __ASM_EMIT("1:")
            __ASM_EMIT("sub         $0x80, %[src]")
            __ASM_EMIT("vperm2f128  $0x23, 0x00(%[dst]), %%ymm4, %%ymm4")
            __ASM_EMIT("vperm2f128  $0x23, 0x20(%[dst]), %%ymm5, %%ymm5")
            __ASM_EMIT("vperm2f128  $0x23, 0x40(%[dst]), %%ymm6, %%ymm6")
            __ASM_EMIT("vperm2f128  $0x23, 0x60(%[dst]), %%ymm7, %%ymm7")
            __ASM_EMIT("vperm2f128  $0x23, 0x00(%[src]), %%ymm0, %%ymm0")
            __ASM_EMIT("vperm2f128  $0x23, 0x20(%[src]), %%ymm1, %%ymm1")
            __ASM_EMIT("vperm2f128  $0x23, 0x40(%[src]), %%ymm2, %%ymm2")
            __ASM_EMIT("vperm2f128  $0x23, 0x60(%[src]), %%ymm3, %%ymm3")
            __ASM_EMIT("vshufps     $0x1b, %%ymm0, %%ymm0, %%ymm0")
            __ASM_EMIT("vshufps     $0x1b, %%ymm1, %%ymm1, %%ymm1")
            __ASM_EMIT("vshufps     $0x1b, %%ymm2, %%ymm2, %%ymm2")
            __ASM_EMIT("vshufps     $0x1b, %%ymm3, %%ymm3, %%ymm3")
            __ASM_EMIT("vshufps     $0x1b, %%ymm4, %%ymm4, %%ymm4")
            __ASM_EMIT("vshufps     $0x1b, %%ymm5, %%ymm5, %%ymm5")
            __ASM_EMIT("vshufps     $0x1b, %%ymm6, %%ymm6, %%ymm6")
            __ASM_EMIT("vshufps     $0x1b, %%ymm7, %%ymm7, %%ymm7")
            __ASM_EMIT("vmovups     %%ymm3, 0x00(%[dst])")
            __ASM_EMIT("vmovups     %%ymm2, 0x20(%[dst])")
            __ASM_EMIT("vmovups     %%ymm1, 0x40(%[dst])")
            __ASM_EMIT("vmovups     %%ymm0, 0x60(%[dst])")
            __ASM_EMIT("vmovups     %%ymm7, 0x00(%[src])")
            __ASM_EMIT("vmovups     %%ymm6, 0x20(%[src])")
            __ASM_EMIT("vmovups     %%ymm5, 0x40(%[src])")
            __ASM_EMIT("vmovups     %%ymm4, 0x60(%[src])")
            __ASM_EMIT("add         $0x80, %[dst]")
            __ASM_EMIT("sub         $32, %[count]")
            __ASM_EMIT("jae         1b")
            /* 16x block */
            __ASM_EMIT("2:")
            __ASM_EMIT("add         $16, %[count]")
            __ASM_EMIT("jl          4f")
            __ASM_EMIT("sub         $0x40, %[src]")
            __ASM_EMIT("vperm2f128  $0x23, 0x00(%[dst]), %%ymm4, %%ymm4")
            __ASM_EMIT("vperm2f128  $0x23, 0x20(%[dst]), %%ymm5, %%ymm5")
            __ASM_EMIT("vperm2f128  $0x23, 0x00(%[src]), %%ymm0, %%ymm0")
            __ASM_EMIT("vperm2f128  $0x23, 0x20(%[src]), %%ymm1, %%ymm1")
            __ASM_EMIT("vshufps     $0x1b, %%ymm0, %%ymm0, %%ymm0")
            __ASM_EMIT("vshufps     $0x1b, %%ymm1, %%ymm1, %%ymm1")
            __ASM_EMIT("vshufps     $0x1b, %%ymm4, %%ymm4, %%ymm4")
            __ASM_EMIT("vshufps     $0x1b, %%ymm5, %%ymm5, %%ymm5")
            __ASM_EMIT("vmovups     %%ymm1, 0x00(%[dst])")
            __ASM_EMIT("vmovups     %%ymm0, 0x20(%[dst])")
            __ASM_EMIT("vmovups     %%ymm5, 0x00(%[src])")
            __ASM_EMIT("vmovups     %%ymm4, 0x20(%[src])")
            __ASM_EMIT("sub         $16, %[count]")
            __ASM_EMIT("add         $0x40, %[dst]")
            /* 8x block */
            __ASM_EMIT("4:")
            __ASM_EMIT("add         $8, %[count]")
            __ASM_EMIT("jl          6f")
            __ASM_EMIT("sub         $0x20, %[src]")
            __ASM_EMIT("vperm2f128  $0x23, 0x00(%[dst]), %%ymm4, %%ymm4")
            __ASM_EMIT("vperm2f128  $0x23, 0x00(%[src]), %%ymm0, %%ymm0")
            __ASM_EMIT("vshufps     $0x1b, %%ymm0, %%ymm0, %%ymm0")
            __ASM_EMIT("vshufps     $0x1b, %%ymm4, %%ymm4, %%ymm4")
            __ASM_EMIT("vmovups     %%ymm0, 0x00(%[dst])")
            __ASM_EMIT("vmovups     %%ymm4, 0x00(%[src])")
            __ASM_EMIT("sub         $8, %[count]")
            __ASM_EMIT("add         $0x20, %[dst]")
            /* 4x block */
            __ASM_EMIT("6:")
            __ASM_EMIT("add         $4, %[count]")
            __ASM_EMIT("jl          8f")
            __ASM_EMIT("sub         $0x10, %[src]")
            __ASM_EMIT("vmovups     0x00(%[dst]), %%xmm4")
            __ASM_EMIT("vmovups     0x00(%[src]), %%xmm0")
            __ASM_EMIT("vshufps     $0x1b, %%xmm0, %%xmm0, %%xmm0")
            __ASM_EMIT("vshufps     $0x1b, %%xmm4, %%xmm4, %%xmm4")
            __ASM_EMIT("vmovups     %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("vmovups     %%xmm4, 0x00(%[src])")
            __ASM_EMIT("sub         $4, %[count]")
            __ASM_EMIT("add         $0x10, %[dst]")
            /* 1x blocks */
            __ASM_EMIT("8:")
            __ASM_EMIT("add         $3, %[count]")
            __ASM_EMIT("jl          10f")
            __ASM_EMIT("9:")
            __ASM_EMIT("sub         $0x4, %[src]")
            __ASM_EMIT("vmovss      0x00(%[dst]), %%xmm4")
            __ASM_EMIT("vmovss      0x00(%[src]), %%xmm0")
            __ASM_EMIT("vmovss      %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("vmovss      %%xmm4, 0x00(%[src])")
            __ASM_EMIT("add         $0x04, %[dst]")
            __ASM_EMIT("dec         %[count]")
            __ASM_EMIT("jge         9b")
            /* end of reverse */
            __ASM_EMIT("10:")
            : [dst] "+r"(dst), [src] "=&r" (src),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void reverse2(float *dst, const float *src, size_t count)
    {
        ARCH_X86_ASM
        (
            __ASM_EMIT("lea         (%[dst], %[count], 4), %[dst]")
            __ASM_EMIT("sub         $64, %[count]")
            __ASM_EMIT("jb          2f")

            /* 64x blocks */
            __ASM_EMIT("1:")
            __ASM_EMIT("sub         $0x100, %[dst]")
            __ASM_EMIT("vperm2f128  $0x23, 0x00(%[src]), %%ymm0, %%ymm0")
            __ASM_EMIT("vperm2f128  $0x23, 0x20(%[src]), %%ymm1, %%ymm1")
            __ASM_EMIT("vperm2f128  $0x23, 0x40(%[src]), %%ymm2, %%ymm2")
            __ASM_EMIT("vperm2f128  $0x23, 0x60(%[src]), %%ymm3, %%ymm3")
            __ASM_EMIT("vperm2f128  $0x23, 0x80(%[src]), %%ymm4, %%ymm4")
            __ASM_EMIT("vperm2f128  $0x23, 0xa0(%[src]), %%ymm5, %%ymm5")
            __ASM_EMIT("vperm2f128  $0x23, 0xc0(%[src]), %%ymm6, %%ymm6")
            __ASM_EMIT("vperm2f128  $0x23, 0xe0(%[src]), %%ymm7, %%ymm7")
            __ASM_EMIT("vshufps     $0x1b, %%ymm0, %%ymm0, %%ymm0")
            __ASM_EMIT("vshufps     $0x1b, %%ymm1, %%ymm1, %%ymm1")
            __ASM_EMIT("vshufps     $0x1b, %%ymm2, %%ymm2, %%ymm2")
            __ASM_EMIT("vshufps     $0x1b, %%ymm3, %%ymm3, %%ymm3")
            __ASM_EMIT("vshufps     $0x1b, %%ymm4, %%ymm4, %%ymm4")
            __ASM_EMIT("vshufps     $0x1b, %%ymm5, %%ymm5, %%ymm5")
            __ASM_EMIT("vshufps     $0x1b, %%ymm6, %%ymm6, %%ymm6")
            __ASM_EMIT("vshufps     $0x1b, %%ymm7, %%ymm7, %%ymm7")
            __ASM_EMIT("vmovups     %%ymm7, 0x00(%[dst])")
            __ASM_EMIT("vmovups     %%ymm6, 0x20(%[dst])")
            __ASM_EMIT("vmovups     %%ymm5, 0x40(%[dst])")
            __ASM_EMIT("vmovups     %%ymm4, 0x60(%[dst])")
            __ASM_EMIT("vmovups     %%ymm3, 0x80(%[dst])")
            __ASM_EMIT("vmovups     %%ymm2, 0xa0(%[dst])")
            __ASM_EMIT("vmovups     %%ymm1, 0xc0(%[dst])")
            __ASM_EMIT("vmovups     %%ymm0, 0xe0(%[dst])")
            __ASM_EMIT("add         $0x100, %[src]")
            __ASM_EMIT("sub         $64, %[count]")
            __ASM_EMIT("jae         1b")
            /* 32x block */
            __ASM_EMIT("2:")
            __ASM_EMIT("add         $32, %[count]")
            __ASM_EMIT("jl          4f")
            __ASM_EMIT("sub         $0x80, %[dst]")
            __ASM_EMIT("vperm2f128  $0x23, 0x00(%[src]), %%ymm0, %%ymm0")
            __ASM_EMIT("vperm2f128  $0x23, 0x20(%[src]), %%ymm1, %%ymm1")
            __ASM_EMIT("vperm2f128  $0x23, 0x40(%[src]), %%ymm2, %%ymm2")
            __ASM_EMIT("vperm2f128  $0x23, 0x60(%[src]), %%ymm3, %%ymm3")
            __ASM_EMIT("vshufps     $0x1b, %%ymm0, %%ymm0, %%ymm0")
            __ASM_EMIT("vshufps     $0x1b, %%ymm1, %%ymm1, %%ymm1")
            __ASM_EMIT("vshufps     $0x1b, %%ymm2, %%ymm2, %%ymm2")
            __ASM_EMIT("vshufps     $0x1b, %%ymm3, %%ymm3, %%ymm3")
            __ASM_EMIT("vmovups     %%ymm3, 0x00(%[dst])")
            __ASM_EMIT("vmovups     %%ymm2, 0x20(%[dst])")
            __ASM_EMIT("vmovups     %%ymm1, 0x40(%[dst])")
            __ASM_EMIT("vmovups     %%ymm0, 0x60(%[dst])")
            __ASM_EMIT("sub         $32, %[count]")
            __ASM_EMIT("add         $0x80, %[src]")
            /* 16x block */
            __ASM_EMIT("4:")
            __ASM_EMIT("add         $16, %[count]")
            __ASM_EMIT("jl          6f")
            __ASM_EMIT("sub         $0x40, %[dst]")
            __ASM_EMIT("vperm2f128  $0x23, 0x00(%[src]), %%ymm0, %%ymm0")
            __ASM_EMIT("vperm2f128  $0x23, 0x20(%[src]), %%ymm1, %%ymm1")
            __ASM_EMIT("vshufps     $0x1b, %%ymm0, %%ymm0, %%ymm0")
            __ASM_EMIT("vshufps     $0x1b, %%ymm1, %%ymm1, %%ymm1")
            __ASM_EMIT("vmovups     %%ymm1, 0x00(%[dst])")
            __ASM_EMIT("vmovups     %%ymm0, 0x20(%[dst])")
            __ASM_EMIT("sub         $16, %[count]")
            __ASM_EMIT("add         $0x40, %[src]")
            /* 8x block */
            __ASM_EMIT("6:")
            __ASM_EMIT("add         $8, %[count]")
            __ASM_EMIT("jl          8f")
            __ASM_EMIT("sub         $0x20, %[dst]")
            __ASM_EMIT("vperm2f128  $0x23, 0x00(%[src]), %%ymm0, %%ymm0")
            __ASM_EMIT("vshufps     $0x1b, %%ymm0, %%ymm0, %%ymm0")
            __ASM_EMIT("vmovups     %%ymm0, 0x00(%[dst])")
            __ASM_EMIT("sub         $8, %[count]")
            __ASM_EMIT("add         $0x20, %[src]")
            /* 4x block */
            __ASM_EMIT("8:")
            __ASM_EMIT("add         $4, %[count]")
            __ASM_EMIT("jl          10f")
            __ASM_EMIT("sub         $0x10, %[dst]")
            __ASM_EMIT("vmovups     0x00(%[src]), %%xmm0")
            __ASM_EMIT("vshufps     $0x1b, %%xmm0, %%xmm0, %%xmm0")
            __ASM_EMIT("vmovups     %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("sub         $4, %[count]")
            __ASM_EMIT("add         $0x10, %[src]")
            /* 1x blocks */
            __ASM_EMIT("10:")
            __ASM_EMIT("add         $3, %[count]")
            __ASM_EMIT("jl          12f")
            __ASM_EMIT("11:")
            __ASM_EMIT("sub         $0x04, %[dst]")
            __ASM_EMIT("vmovss      0x00(%[src]), %%xmm0")
            __ASM_EMIT("vmovss      %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("add         $0x04, %[src]")
            __ASM_EMIT("dec         %[count]")
            __ASM_EMIT("jge         11b")
            /* end of reverse */
            __ASM_EMIT("12:")
            : [dst] "+r"(dst), [src] "+r" (src),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }
}

#endif /* DSP_ARCH_X86_AVX_COPY_H_ */
