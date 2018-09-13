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
            __ASM_EMIT("vzeroupper")

            : [src] "+r" (src), [dst] "+r"(dst),
              [count] "+r" (count), [off] "=&r" (off)
            :
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }
}

#endif /* DSP_ARCH_X86_AVX_COPY_H_ */
