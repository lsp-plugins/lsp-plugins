/*
 * mix.h
 *
 *  Created on: 3 дек. 2019 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_AVX_MIX_H_
#define DSP_ARCH_X86_AVX_MIX_H_

#ifndef DSP_ARCH_X86_AVX_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_AVX_IMPL */

namespace avx
{
    void mix2(float *a, const float *b, float k1, float k2, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            __ASM_EMIT("vbroadcastss    %[k1], %%ymm6")
            __ASM_EMIT("vbroadcastss    %[k2], %%ymm7")
            __ASM_EMIT("xor             %[off], %[off]")
            // 16x blocks
            __ASM_EMIT("sub             $16, %[count]")
            __ASM_EMIT("jb              2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vmulps          0x00(%[a],%[off]), %%ymm6, %%ymm0")
            __ASM_EMIT("vmulps          0x20(%[a],%[off]), %%ymm6, %%ymm1")
            __ASM_EMIT("vmulps          0x00(%[b],%[off]), %%ymm7, %%ymm2")
            __ASM_EMIT("vmulps          0x20(%[b],%[off]), %%ymm7, %%ymm3")
            __ASM_EMIT("vaddps          %%ymm2, %%ymm0, %%ymm0")
            __ASM_EMIT("vaddps          %%ymm3, %%ymm1, %%ymm1")
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[a],%[off])")
            __ASM_EMIT("vmovups         %%ymm1, 0x20(%[a],%[off])")
            __ASM_EMIT("add             $0x40, %[off]")
            __ASM_EMIT("sub             $16, %[count]")
            __ASM_EMIT("jae             1b")
            // 8x block
            __ASM_EMIT("2:")
            __ASM_EMIT("add             $8, %[count]")
            __ASM_EMIT("jl              4f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vmulps          0x00(%[a],%[off]), %%ymm6, %%ymm0")
            __ASM_EMIT("vmulps          0x00(%[b],%[off]), %%ymm7, %%ymm2")
            __ASM_EMIT("vaddps          %%ymm2, %%ymm0, %%ymm0")
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[a],%[off])")
            __ASM_EMIT("add             $0x20, %[off]")
            __ASM_EMIT("sub             $8, %[count]")
            // 4x block
            __ASM_EMIT("4:")
            __ASM_EMIT("add             $4, %[count]")
            __ASM_EMIT("jl              6f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vmulps          0x00(%[a],%[off]), %%xmm6, %%xmm0")
            __ASM_EMIT("vmulps          0x00(%[b],%[off]), %%xmm7, %%xmm2")
            __ASM_EMIT("vaddps          %%xmm2, %%xmm0, %%xmm0")
            __ASM_EMIT("vmovups         %%xmm0, 0x00(%[a],%[off])")
            __ASM_EMIT("add             $0x10, %[off]")
            __ASM_EMIT("sub             $4, %[count]")
            // 1x blocks
            __ASM_EMIT("6:")
            __ASM_EMIT("add             $3, %[count]")
            __ASM_EMIT("jl              8f")
            __ASM_EMIT("7:")
            __ASM_EMIT("vmulss          0x00(%[a],%[off]), %%xmm6, %%xmm0")
            __ASM_EMIT("vmulss          0x00(%[b],%[off]), %%xmm7, %%xmm2")
            __ASM_EMIT("vaddss          %%xmm2, %%xmm0, %%xmm0")
            __ASM_EMIT("vmovss          %%xmm0, 0x00(%[a],%[off])")
            __ASM_EMIT("add             $0x04, %[off]")
            __ASM_EMIT("dec             %[count]")
            __ASM_EMIT("jge             7b")
            // End
            __ASM_EMIT("8:")
            : [count] "+r" (count),
              [off] "=&r" (off)
            : [a] "r" (a), [b] "r" (b),
              [k1] "m" (k1), [k2] "m" (k2)
            : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void mix_copy2(float *dst, const float *a, const float *b, float k1, float k2, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            __ASM_EMIT("vbroadcastss    %[k1], %%ymm6")
            __ASM_EMIT("vbroadcastss    %[k2], %%ymm7")
            __ASM_EMIT("xor             %[off], %[off]")
            // 16x blocks
            __ASM_EMIT("sub             $16, %[count]")
            __ASM_EMIT("jb              2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vmulps          0x00(%[a],%[off]), %%ymm6, %%ymm0")
            __ASM_EMIT("vmulps          0x20(%[a],%[off]), %%ymm6, %%ymm1")
            __ASM_EMIT("vmulps          0x00(%[b],%[off]), %%ymm7, %%ymm2")
            __ASM_EMIT("vmulps          0x20(%[b],%[off]), %%ymm7, %%ymm3")
            __ASM_EMIT("vaddps          %%ymm2, %%ymm0, %%ymm0")
            __ASM_EMIT("vaddps          %%ymm3, %%ymm1, %%ymm1")
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst],%[off])")
            __ASM_EMIT("vmovups         %%ymm1, 0x20(%[dst],%[off])")
            __ASM_EMIT("add             $0x40, %[off]")
            __ASM_EMIT("sub             $16, %[count]")
            __ASM_EMIT("jae             1b")
            // 8x block
            __ASM_EMIT("2:")
            __ASM_EMIT("add             $8, %[count]")
            __ASM_EMIT("jl              4f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vmulps          0x00(%[a],%[off]), %%ymm6, %%ymm0")
            __ASM_EMIT("vmulps          0x00(%[b],%[off]), %%ymm7, %%ymm2")
            __ASM_EMIT("vaddps          %%ymm2, %%ymm0, %%ymm0")
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst],%[off])")
            __ASM_EMIT("add             $0x20, %[off]")
            __ASM_EMIT("sub             $8, %[count]")
            // 4x block
            __ASM_EMIT("4:")
            __ASM_EMIT("add             $4, %[count]")
            __ASM_EMIT("jl              6f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vmulps          0x00(%[a],%[off]), %%xmm6, %%xmm0")
            __ASM_EMIT("vmulps          0x00(%[b],%[off]), %%xmm7, %%xmm2")
            __ASM_EMIT("vaddps          %%xmm2, %%xmm0, %%xmm0")
            __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst],%[off])")
            __ASM_EMIT("add             $0x10, %[off]")
            __ASM_EMIT("sub             $4, %[count]")
            // 1x blocks
            __ASM_EMIT("6:")
            __ASM_EMIT("add             $3, %[count]")
            __ASM_EMIT("jl              8f")
            __ASM_EMIT("7:")
            __ASM_EMIT("vmulss          0x00(%[a],%[off]), %%xmm6, %%xmm0")
            __ASM_EMIT("vmulss          0x00(%[b],%[off]), %%xmm7, %%xmm2")
            __ASM_EMIT("vaddss          %%xmm2, %%xmm0, %%xmm0")
            __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst],%[off])")
            __ASM_EMIT("add             $0x04, %[off]")
            __ASM_EMIT("dec             %[count]")
            __ASM_EMIT("jge             7b")
            // End
            __ASM_EMIT("8:")
            : [count] "+r" (count),
              [off] "=&r" (off)
            : [dst] "r" (dst), [a] "r" (a), [b] "r" (b),
              [k1] "m" (k1), [k2] "m" (k2)
            : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void mix_add2(float *dst, const float *a, const float *b, float k1, float k2, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            __ASM_EMIT("vbroadcastss    %[k1], %%ymm6")
            __ASM_EMIT("vbroadcastss    %[k2], %%ymm7")
            __ASM_EMIT("xor             %[off], %[off]")
            // 16x blocks
            __ASM_EMIT("sub             $16, %[count]")
            __ASM_EMIT("jb              2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vmulps          0x00(%[a],%[off]), %%ymm6, %%ymm0")
            __ASM_EMIT("vmulps          0x20(%[a],%[off]), %%ymm6, %%ymm1")
            __ASM_EMIT("vmulps          0x00(%[b],%[off]), %%ymm7, %%ymm2")
            __ASM_EMIT("vmulps          0x20(%[b],%[off]), %%ymm7, %%ymm3")
            __ASM_EMIT("vaddps          0x00(%[dst],%[off]), %%ymm0, %%ymm0")
            __ASM_EMIT("vaddps          0x20(%[dst],%[off]), %%ymm1, %%ymm1")
            __ASM_EMIT("vaddps          %%ymm2, %%ymm0, %%ymm0")
            __ASM_EMIT("vaddps          %%ymm3, %%ymm1, %%ymm1")
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst],%[off])")
            __ASM_EMIT("vmovups         %%ymm1, 0x20(%[dst],%[off])")
            __ASM_EMIT("add             $0x40, %[off]")
            __ASM_EMIT("sub             $16, %[count]")
            __ASM_EMIT("jae             1b")
            // 8x block
            __ASM_EMIT("2:")
            __ASM_EMIT("add             $8, %[count]")
            __ASM_EMIT("jl              4f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vmulps          0x00(%[a],%[off]), %%ymm6, %%ymm0")
            __ASM_EMIT("vmulps          0x00(%[b],%[off]), %%ymm7, %%ymm2")
            __ASM_EMIT("vaddps          0x00(%[dst],%[off]), %%ymm0, %%ymm0")
            __ASM_EMIT("vaddps          %%ymm2, %%ymm0, %%ymm0")
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst],%[off])")
            __ASM_EMIT("add             $0x20, %[off]")
            __ASM_EMIT("sub             $8, %[count]")
            // 4x block
            __ASM_EMIT("4:")
            __ASM_EMIT("add             $4, %[count]")
            __ASM_EMIT("jl              6f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vmulps          0x00(%[a],%[off]), %%xmm6, %%xmm0")
            __ASM_EMIT("vmulps          0x00(%[b],%[off]), %%xmm7, %%xmm2")
            __ASM_EMIT("vaddps          0x00(%[dst],%[off]), %%xmm0, %%xmm0")
            __ASM_EMIT("vaddps          %%xmm2, %%xmm0, %%xmm0")
            __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst],%[off])")
            __ASM_EMIT("add             $0x10, %[off]")
            __ASM_EMIT("sub             $4, %[count]")
            // 1x blocks
            __ASM_EMIT("6:")
            __ASM_EMIT("add             $3, %[count]")
            __ASM_EMIT("jl              8f")
            __ASM_EMIT("7:")
            __ASM_EMIT("vmulss          0x00(%[a],%[off]), %%xmm6, %%xmm0")
            __ASM_EMIT("vmulss          0x00(%[b],%[off]), %%xmm7, %%xmm2")
            __ASM_EMIT("vaddss          0x00(%[dst],%[off]), %%xmm0, %%xmm0")
            __ASM_EMIT("vaddss          %%xmm2, %%xmm0, %%xmm0")
            __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst],%[off])")
            __ASM_EMIT("add             $0x04, %[off]")
            __ASM_EMIT("dec             %[count]")
            __ASM_EMIT("jge             7b")
            // End
            __ASM_EMIT("8:")
            : [count] "+r" (count),
              [off] "=&r" (off)
            : [dst] "r" (dst), [a] "r" (a), [b] "r" (b),
              [k1] "m" (k1), [k2] "m" (k2)
            : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void mix3(float *a, const float *b, const float *c,
            float k1, float k2, float k3, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            __ASM_EMIT("vbroadcastss    %[k1], %%ymm5")
            __ASM_EMIT("vbroadcastss    %[k2], %%ymm6")
            __ASM_EMIT("vbroadcastss    %[k3], %%ymm7")
            __ASM_EMIT("xor             %[off], %[off]")
            // 16x blocks
            __ASM_EMIT("sub             $16, %[count]")
            __ASM_EMIT("jb              2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vmulps          0x00(%[a],%[off]), %%ymm5, %%ymm0")
            __ASM_EMIT("vmulps          0x20(%[a],%[off]), %%ymm5, %%ymm1")
            __ASM_EMIT("vmulps          0x00(%[b],%[off]), %%ymm6, %%ymm2")
            __ASM_EMIT("vmulps          0x20(%[b],%[off]), %%ymm6, %%ymm3")
            __ASM_EMIT("vaddps          %%ymm2, %%ymm0, %%ymm0")
            __ASM_EMIT("vaddps          %%ymm3, %%ymm1, %%ymm1")
            __ASM_EMIT("vmulps          0x00(%[c],%[off]), %%ymm7, %%ymm2")
            __ASM_EMIT("vmulps          0x20(%[c],%[off]), %%ymm7, %%ymm3")
            __ASM_EMIT("vaddps          %%ymm2, %%ymm0, %%ymm0")
            __ASM_EMIT("vaddps          %%ymm3, %%ymm1, %%ymm1")
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[a],%[off])")
            __ASM_EMIT("vmovups         %%ymm1, 0x20(%[a],%[off])")
            __ASM_EMIT("add             $0x40, %[off]")
            __ASM_EMIT("sub             $16, %[count]")
            __ASM_EMIT("jae             1b")
            // 8x block
            __ASM_EMIT("2:")
            __ASM_EMIT("add             $8, %[count]")
            __ASM_EMIT("jl              4f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vmulps          0x00(%[a],%[off]), %%ymm5, %%ymm0")
            __ASM_EMIT("vmulps          0x00(%[b],%[off]), %%ymm6, %%ymm2")
            __ASM_EMIT("vaddps          %%ymm2, %%ymm0, %%ymm0")
            __ASM_EMIT("vmulps          0x00(%[c],%[off]), %%ymm7, %%ymm2")
            __ASM_EMIT("vaddps          %%ymm2, %%ymm0, %%ymm0")
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[a],%[off])")
            __ASM_EMIT("add             $0x20, %[off]")
            __ASM_EMIT("sub             $8, %[count]")
            // 4x block
            __ASM_EMIT("4:")
            __ASM_EMIT("add             $4, %[count]")
            __ASM_EMIT("jl              6f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vmulps          0x00(%[a],%[off]), %%xmm5, %%xmm0")
            __ASM_EMIT("vmulps          0x00(%[b],%[off]), %%xmm6, %%xmm2")
            __ASM_EMIT("vaddps          %%xmm2, %%xmm0, %%xmm0")
            __ASM_EMIT("vmulps          0x00(%[c],%[off]), %%xmm7, %%xmm2")
            __ASM_EMIT("vaddps          %%xmm2, %%xmm0, %%xmm0")
            __ASM_EMIT("vmovups         %%xmm0, 0x00(%[a],%[off])")
            __ASM_EMIT("add             $0x10, %[off]")
            __ASM_EMIT("sub             $4, %[count]")
            // 1x blocks
            __ASM_EMIT("6:")
            __ASM_EMIT("add             $3, %[count]")
            __ASM_EMIT("jl              8f")
            __ASM_EMIT("7:")
            __ASM_EMIT("vmulss          0x00(%[a],%[off]), %%xmm5, %%xmm0")
            __ASM_EMIT("vmulss          0x00(%[b],%[off]), %%xmm6, %%xmm2")
            __ASM_EMIT("vaddss          %%xmm2, %%xmm0, %%xmm0")
            __ASM_EMIT("vmulss          0x00(%[c],%[off]), %%xmm7, %%xmm2")
            __ASM_EMIT("vaddss          %%xmm2, %%xmm0, %%xmm0")
            __ASM_EMIT("vmovss          %%xmm0, 0x00(%[a],%[off])")
            __ASM_EMIT("add             $0x04, %[off]")
            __ASM_EMIT("dec             %[count]")
            __ASM_EMIT("jge             7b")
            // End
            __ASM_EMIT("8:")
            : [count] "+r" (count),
              [off] "=&r" (off)
            : [a] "r" (a), [b] "r" (b), [c] "r" (c),
              [k1] "m" (k1), [k2] "m" (k2), [k3] "m" (k3)
            : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void mix_copy3(float *dst, const float *a, const float *b, const float *c,
            float k1, float k2, float k3, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            __ASM_EMIT("vbroadcastss    %[k1], %%ymm5")
            __ASM_EMIT("vbroadcastss    %[k2], %%ymm6")
            __ASM_EMIT("vbroadcastss    %[k3], %%ymm7")
            __ASM_EMIT("xor             %[off], %[off]")
            // 16x blocks
            __ASM_EMIT("sub             $16, %[count]")
            __ASM_EMIT("jb              2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vmulps          0x00(%[a],%[off]), %%ymm5, %%ymm0")
            __ASM_EMIT("vmulps          0x20(%[a],%[off]), %%ymm5, %%ymm1")
            __ASM_EMIT("vmulps          0x00(%[b],%[off]), %%ymm6, %%ymm2")
            __ASM_EMIT("vmulps          0x20(%[b],%[off]), %%ymm6, %%ymm3")
            __ASM_EMIT("vaddps          %%ymm2, %%ymm0, %%ymm0")
            __ASM_EMIT("vaddps          %%ymm3, %%ymm1, %%ymm1")
            __ASM_EMIT("vmulps          0x00(%[c],%[off]), %%ymm7, %%ymm2")
            __ASM_EMIT("vmulps          0x20(%[c],%[off]), %%ymm7, %%ymm3")
            __ASM_EMIT("vaddps          %%ymm2, %%ymm0, %%ymm0")
            __ASM_EMIT("vaddps          %%ymm3, %%ymm1, %%ymm1")
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst],%[off])")
            __ASM_EMIT("vmovups         %%ymm1, 0x20(%[dst],%[off])")
            __ASM_EMIT("add             $0x40, %[off]")
            __ASM_EMIT("sub             $16, %[count]")
            __ASM_EMIT("jae             1b")
            // 8x block
            __ASM_EMIT("2:")
            __ASM_EMIT("add             $8, %[count]")
            __ASM_EMIT("jl              4f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vmulps          0x00(%[a],%[off]), %%ymm5, %%ymm0")
            __ASM_EMIT("vmulps          0x00(%[b],%[off]), %%ymm6, %%ymm2")
            __ASM_EMIT("vaddps          %%ymm2, %%ymm0, %%ymm0")
            __ASM_EMIT("vmulps          0x00(%[c],%[off]), %%ymm7, %%ymm2")
            __ASM_EMIT("vaddps          %%ymm2, %%ymm0, %%ymm0")
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst],%[off])")
            __ASM_EMIT("add             $0x20, %[off]")
            __ASM_EMIT("sub             $8, %[count]")
            // 4x block
            __ASM_EMIT("4:")
            __ASM_EMIT("add             $4, %[count]")
            __ASM_EMIT("jl              6f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vmulps          0x00(%[a],%[off]), %%xmm5, %%xmm0")
            __ASM_EMIT("vmulps          0x00(%[b],%[off]), %%xmm6, %%xmm2")
            __ASM_EMIT("vaddps          %%xmm2, %%xmm0, %%xmm0")
            __ASM_EMIT("vmulps          0x00(%[c],%[off]), %%xmm7, %%xmm2")
            __ASM_EMIT("vaddps          %%xmm2, %%xmm0, %%xmm0")
            __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst],%[off])")
            __ASM_EMIT("add             $0x10, %[off]")
            __ASM_EMIT("sub             $4, %[count]")
            // 1x blocks
            __ASM_EMIT("6:")
            __ASM_EMIT("add             $3, %[count]")
            __ASM_EMIT("jl              8f")
            __ASM_EMIT("7:")
            __ASM_EMIT("vmulss          0x00(%[a],%[off]), %%xmm5, %%xmm0")
            __ASM_EMIT("vmulss          0x00(%[b],%[off]), %%xmm6, %%xmm2")
            __ASM_EMIT("vaddss          %%xmm2, %%xmm0, %%xmm0")
            __ASM_EMIT("vmulss          0x00(%[c],%[off]), %%xmm7, %%xmm2")
            __ASM_EMIT("vaddss          %%xmm2, %%xmm0, %%xmm0")
            __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst],%[off])")
            __ASM_EMIT("add             $0x04, %[off]")
            __ASM_EMIT("dec             %[count]")
            __ASM_EMIT("jge             7b")
            // End
            __ASM_EMIT("8:")
            : [count] "+r" (count),
              [off] "=&r" (off)
            : [dst] "r" (dst), [a] "r" (a), [b] "r" (b), [c] "r" (c),
              [k1] "m" (k1), [k2] "m" (k2), [k3] "m" (k3)
            : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void mix_add3(float *dst, const float *a, const float *b, const float *c,
            float k1, float k2, float k3, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            __ASM_EMIT("vbroadcastss    %[k1], %%ymm5")
            __ASM_EMIT("vbroadcastss    %[k2], %%ymm6")
            __ASM_EMIT("vbroadcastss    %[k3], %%ymm7")
            __ASM_EMIT("xor             %[off], %[off]")
            // 16x blocks
            __ASM_EMIT("sub             $16, %[count]")
            __ASM_EMIT("jb              2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vmulps          0x00(%[a],%[off]), %%ymm5, %%ymm0")
            __ASM_EMIT("vmulps          0x20(%[a],%[off]), %%ymm5, %%ymm1")
            __ASM_EMIT("vmulps          0x00(%[b],%[off]), %%ymm6, %%ymm2")
            __ASM_EMIT("vmulps          0x20(%[b],%[off]), %%ymm6, %%ymm3")
            __ASM_EMIT("vaddps          %%ymm2, %%ymm0, %%ymm0")
            __ASM_EMIT("vaddps          %%ymm3, %%ymm1, %%ymm1")
            __ASM_EMIT("vmulps          0x00(%[c],%[off]), %%ymm7, %%ymm2")
            __ASM_EMIT("vmulps          0x20(%[c],%[off]), %%ymm7, %%ymm3")
            __ASM_EMIT("vaddps          %%ymm2, %%ymm0, %%ymm0")
            __ASM_EMIT("vaddps          %%ymm3, %%ymm1, %%ymm1")
            __ASM_EMIT("vaddps          0x00(%[dst],%[off]), %%ymm0, %%ymm0")
            __ASM_EMIT("vaddps          0x20(%[dst],%[off]), %%ymm1, %%ymm1")
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst],%[off])")
            __ASM_EMIT("vmovups         %%ymm1, 0x20(%[dst],%[off])")
            __ASM_EMIT("add             $0x40, %[off]")
            __ASM_EMIT("sub             $16, %[count]")
            __ASM_EMIT("jae             1b")
            // 8x block
            __ASM_EMIT("2:")
            __ASM_EMIT("add             $8, %[count]")
            __ASM_EMIT("jl              4f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vmulps          0x00(%[a],%[off]), %%ymm5, %%ymm0")
            __ASM_EMIT("vmulps          0x00(%[b],%[off]), %%ymm6, %%ymm2")
            __ASM_EMIT("vaddps          %%ymm2, %%ymm0, %%ymm0")
            __ASM_EMIT("vmulps          0x00(%[c],%[off]), %%ymm7, %%ymm2")
            __ASM_EMIT("vaddps          %%ymm2, %%ymm0, %%ymm0")
            __ASM_EMIT("vaddps          0x00(%[dst],%[off]), %%ymm0, %%ymm0")
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst],%[off])")
            __ASM_EMIT("add             $0x20, %[off]")
            __ASM_EMIT("sub             $8, %[count]")
            // 4x block
            __ASM_EMIT("4:")
            __ASM_EMIT("add             $4, %[count]")
            __ASM_EMIT("jl              6f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vmulps          0x00(%[a],%[off]), %%xmm5, %%xmm0")
            __ASM_EMIT("vmulps          0x00(%[b],%[off]), %%xmm6, %%xmm2")
            __ASM_EMIT("vaddps          %%xmm2, %%xmm0, %%xmm0")
            __ASM_EMIT("vmulps          0x00(%[c],%[off]), %%xmm7, %%xmm2")
            __ASM_EMIT("vaddps          %%xmm2, %%xmm0, %%xmm0")
            __ASM_EMIT("vaddps          0x00(%[dst],%[off]), %%xmm0, %%xmm0")
            __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst],%[off])")
            __ASM_EMIT("add             $0x10, %[off]")
            __ASM_EMIT("sub             $4, %[count]")
            // 1x blocks
            __ASM_EMIT("6:")
            __ASM_EMIT("add             $3, %[count]")
            __ASM_EMIT("jl              8f")
            __ASM_EMIT("7:")
            __ASM_EMIT("vmulss          0x00(%[a],%[off]), %%xmm5, %%xmm0")
            __ASM_EMIT("vmulss          0x00(%[b],%[off]), %%xmm6, %%xmm2")
            __ASM_EMIT("vaddss          %%xmm2, %%xmm0, %%xmm0")
            __ASM_EMIT("vmulss          0x00(%[c],%[off]), %%xmm7, %%xmm2")
            __ASM_EMIT("vaddss          %%xmm2, %%xmm0, %%xmm0")
            __ASM_EMIT("vaddss          0x00(%[dst],%[off]), %%xmm0, %%xmm0")
            __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst],%[off])")
            __ASM_EMIT("add             $0x04, %[off]")
            __ASM_EMIT("dec             %[count]")
            __ASM_EMIT("jge             7b")
            // End
            __ASM_EMIT("8:")
            : [count] "+r" (count),
              [off] "=&r" (off)
            : [dst] "r" (dst), [a] "r" (a), [b] "r" (b), [c] "r" (c),
              [k1] "m" (k1), [k2] "m" (k2), [k3] "m" (k3)
            : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void mix4(float *a, const float *b, const float *c, const float *d,
            float k1, float k2, float k3, float k4, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            __ASM_EMIT("vbroadcastss    %[k1], %%ymm4")
            __ASM_EMIT("vbroadcastss    %[k2], %%ymm5")
            __ASM_EMIT("vbroadcastss    %[k3], %%ymm6")
            __ASM_EMIT("vbroadcastss    %[k4], %%ymm7")
            __ASM_EMIT("xor             %[off], %[off]")
            // 16x blocks
            __ASM_EMIT("sub             $16, %[count]")
            __ASM_EMIT("jb              2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vmulps          0x00(%[a],%[off]), %%ymm4, %%ymm0")
            __ASM_EMIT("vmulps          0x20(%[a],%[off]), %%ymm4, %%ymm1")
            __ASM_EMIT("vmulps          0x00(%[b],%[off]), %%ymm5, %%ymm2")
            __ASM_EMIT("vmulps          0x20(%[b],%[off]), %%ymm5, %%ymm3")
            __ASM_EMIT("vaddps          %%ymm2, %%ymm0, %%ymm0")
            __ASM_EMIT("vaddps          %%ymm3, %%ymm1, %%ymm1")
            __ASM_EMIT("vmulps          0x00(%[c],%[off]), %%ymm6, %%ymm2")
            __ASM_EMIT("vmulps          0x20(%[c],%[off]), %%ymm6, %%ymm3")
            __ASM_EMIT("vaddps          %%ymm2, %%ymm0, %%ymm0")
            __ASM_EMIT("vaddps          %%ymm3, %%ymm1, %%ymm1")
            __ASM_EMIT("vmulps          0x00(%[d],%[off]), %%ymm7, %%ymm2")
            __ASM_EMIT("vmulps          0x20(%[d],%[off]), %%ymm7, %%ymm3")
            __ASM_EMIT("vaddps          %%ymm2, %%ymm0, %%ymm0")
            __ASM_EMIT("vaddps          %%ymm3, %%ymm1, %%ymm1")
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[a],%[off])")
            __ASM_EMIT("vmovups         %%ymm1, 0x20(%[a],%[off])")
            __ASM_EMIT("add             $0x40, %[off]")
            __ASM_EMIT("sub             $16, %[count]")
            __ASM_EMIT("jae             1b")
            // 8x block
            __ASM_EMIT("2:")
            __ASM_EMIT("add             $8, %[count]")
            __ASM_EMIT("jl              4f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vmulps          0x00(%[a],%[off]), %%ymm4, %%ymm0")
            __ASM_EMIT("vmulps          0x00(%[b],%[off]), %%ymm5, %%ymm2")
            __ASM_EMIT("vaddps          %%ymm2, %%ymm0, %%ymm0")
            __ASM_EMIT("vmulps          0x00(%[c],%[off]), %%ymm6, %%ymm2")
            __ASM_EMIT("vaddps          %%ymm2, %%ymm0, %%ymm0")
            __ASM_EMIT("vmulps          0x00(%[d],%[off]), %%ymm7, %%ymm2")
            __ASM_EMIT("vaddps          %%ymm2, %%ymm0, %%ymm0")
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[a],%[off])")
            __ASM_EMIT("add             $0x20, %[off]")
            __ASM_EMIT("sub             $8, %[count]")
            // 4x block
            __ASM_EMIT("4:")
            __ASM_EMIT("add             $4, %[count]")
            __ASM_EMIT("jl              6f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vmulps          0x00(%[a],%[off]), %%xmm4, %%xmm0")
            __ASM_EMIT("vmulps          0x00(%[b],%[off]), %%xmm5, %%xmm2")
            __ASM_EMIT("vaddps          %%xmm2, %%xmm0, %%xmm0")
            __ASM_EMIT("vmulps          0x00(%[c],%[off]), %%xmm6, %%xmm2")
            __ASM_EMIT("vaddps          %%xmm2, %%xmm0, %%xmm0")
            __ASM_EMIT("vmulps          0x00(%[d],%[off]), %%xmm7, %%xmm2")
            __ASM_EMIT("vaddps          %%xmm2, %%xmm0, %%xmm0")
            __ASM_EMIT("vmovups         %%xmm0, 0x00(%[a],%[off])")
            __ASM_EMIT("add             $0x10, %[off]")
            __ASM_EMIT("sub             $4, %[count]")
            // 1x blocks
            __ASM_EMIT("6:")
            __ASM_EMIT("add             $3, %[count]")
            __ASM_EMIT("jl              8f")
            __ASM_EMIT("7:")
            __ASM_EMIT("vmulss          0x00(%[a],%[off]), %%xmm4, %%xmm0")
            __ASM_EMIT("vmulss          0x00(%[b],%[off]), %%xmm5, %%xmm2")
            __ASM_EMIT("vaddss          %%xmm2, %%xmm0, %%xmm0")
            __ASM_EMIT("vmulss          0x00(%[c],%[off]), %%xmm6, %%xmm2")
            __ASM_EMIT("vaddss          %%xmm2, %%xmm0, %%xmm0")
            __ASM_EMIT("vmulss          0x00(%[d],%[off]), %%xmm7, %%xmm2")
            __ASM_EMIT("vaddss          %%xmm2, %%xmm0, %%xmm0")
            __ASM_EMIT("vmovss          %%xmm0, 0x00(%[a],%[off])")
            __ASM_EMIT("add             $0x04, %[off]")
            __ASM_EMIT("dec             %[count]")
            __ASM_EMIT("jge             7b")
            // End
            __ASM_EMIT("8:")
            : [count] "+r" (count),
              [off] "=&r" (off)
            : [a] "r" (a), [b] "r" (b), [c] "r" (c), [d] "r" (d),
              [k1] "m" (k1), [k2] "m" (k2), [k3] "m" (k3), [k4] "m" (k4)
            : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

#if defined(LSP_PROFILING) && defined(ARCH_I386)
    void mix_copy4(float *dst, const float *a, const float *b, const float *c, const float *d,
            float k1, float k2, float k3, float k4, size_t count)
    {
        ARCH_X86_ASM
        (
            __ASM_EMIT("vbroadcastss    %[k1], %%ymm4")
            __ASM_EMIT("vbroadcastss    %[k2], %%ymm5")
            __ASM_EMIT("vbroadcastss    %[k3], %%ymm6")
            __ASM_EMIT("vbroadcastss    %[k4], %%ymm7")
            // 16x blocks
            __ASM_EMIT("subl            $16, %[count]")
            __ASM_EMIT("jb              2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vmulps          0x00(%[a]), %%ymm4, %%ymm0")
            __ASM_EMIT("vmulps          0x20(%[a]), %%ymm4, %%ymm1")
            __ASM_EMIT("vmulps          0x00(%[b]), %%ymm5, %%ymm2")
            __ASM_EMIT("vmulps          0x20(%[b]), %%ymm5, %%ymm3")
            __ASM_EMIT("vaddps          %%ymm2, %%ymm0, %%ymm0")
            __ASM_EMIT("vaddps          %%ymm3, %%ymm1, %%ymm1")
            __ASM_EMIT("vmulps          0x00(%[c]), %%ymm6, %%ymm2")
            __ASM_EMIT("vmulps          0x20(%[c]), %%ymm6, %%ymm3")
            __ASM_EMIT("vaddps          %%ymm2, %%ymm0, %%ymm0")
            __ASM_EMIT("vaddps          %%ymm3, %%ymm1, %%ymm1")
            __ASM_EMIT("vmulps          0x00(%[d]), %%ymm7, %%ymm2")
            __ASM_EMIT("vmulps          0x20(%[d]), %%ymm7, %%ymm3")
            __ASM_EMIT("vaddps          %%ymm2, %%ymm0, %%ymm0")
            __ASM_EMIT("vaddps          %%ymm3, %%ymm1, %%ymm1")
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
            __ASM_EMIT("vmovups         %%ymm1, 0x20(%[dst])")
            __ASM_EMIT("add             $0x40, %[a]")
            __ASM_EMIT("add             $0x40, %[b]")
            __ASM_EMIT("add             $0x40, %[c]")
            __ASM_EMIT("add             $0x40, %[d]")
            __ASM_EMIT("add             $0x40, %[dst]")
            __ASM_EMIT("subl            $16, %[count]")
            __ASM_EMIT("jae             1b")
            // 8x block
            __ASM_EMIT("2:")
            __ASM_EMIT("addl            $8, %[count]")
            __ASM_EMIT("jl              4f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vmulps          0x00(%[a]), %%ymm4, %%ymm0")
            __ASM_EMIT("vmulps          0x00(%[b]), %%ymm5, %%ymm2")
            __ASM_EMIT("vaddps          %%ymm2, %%ymm0, %%ymm0")
            __ASM_EMIT("vmulps          0x00(%[c]), %%ymm6, %%ymm2")
            __ASM_EMIT("vaddps          %%ymm2, %%ymm0, %%ymm0")
            __ASM_EMIT("vmulps          0x00(%[d]), %%ymm7, %%ymm2")
            __ASM_EMIT("vaddps          %%ymm2, %%ymm0, %%ymm0")
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
            __ASM_EMIT("add             $0x20, %[a]")
            __ASM_EMIT("add             $0x20, %[b]")
            __ASM_EMIT("add             $0x20, %[c]")
            __ASM_EMIT("add             $0x20, %[d]")
            __ASM_EMIT("add             $0x20, %[dst]")
            __ASM_EMIT("subl            $8, %[count]")
            // 4x block
            __ASM_EMIT("4:")
            __ASM_EMIT("addl            $4, %[count]")
            __ASM_EMIT("jl              6f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vmulps          0x00(%[a]), %%xmm4, %%xmm0")
            __ASM_EMIT("vmulps          0x00(%[b]), %%xmm5, %%xmm2")
            __ASM_EMIT("vaddps          %%xmm2, %%xmm0, %%xmm0")
            __ASM_EMIT("vmulps          0x00(%[c]), %%xmm6, %%xmm2")
            __ASM_EMIT("vaddps          %%xmm2, %%xmm0, %%xmm0")
            __ASM_EMIT("vmulps          0x00(%[d]), %%xmm7, %%xmm2")
            __ASM_EMIT("vaddps          %%xmm2, %%xmm0, %%xmm0")
            __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("add             $0x10, %[a]")
            __ASM_EMIT("add             $0x10, %[b]")
            __ASM_EMIT("add             $0x10, %[c]")
            __ASM_EMIT("add             $0x10, %[d]")
            __ASM_EMIT("add             $0x10, %[dst]")
            __ASM_EMIT("subl            $4, %[count]")
            // 1x blocks
            __ASM_EMIT("6:")
            __ASM_EMIT("addl            $3, %[count]")
            __ASM_EMIT("jl              8f")
            __ASM_EMIT("7:")
            __ASM_EMIT("vmulss          0x00(%[a]), %%xmm4, %%xmm0")
            __ASM_EMIT("vmulss          0x00(%[b]), %%xmm5, %%xmm2")
            __ASM_EMIT("vaddss          %%xmm2, %%xmm0, %%xmm0")
            __ASM_EMIT("vmulss          0x00(%[c]), %%xmm6, %%xmm2")
            __ASM_EMIT("vaddss          %%xmm2, %%xmm0, %%xmm0")
            __ASM_EMIT("vmulss          0x00(%[d]), %%xmm7, %%xmm2")
            __ASM_EMIT("vaddss          %%xmm2, %%xmm0, %%xmm0")
            __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("add             $0x04, %[a]")
            __ASM_EMIT("add             $0x04, %[b]")
            __ASM_EMIT("add             $0x04, %[c]")
            __ASM_EMIT("add             $0x04, %[d]")
            __ASM_EMIT("add             $0x04, %[dst]")
            __ASM_EMIT("decl            %[count]")
            __ASM_EMIT("jge             7b")
            // End
            __ASM_EMIT("8:")
            : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b), [c] "+r" (c), [d] "+r" (d),
              [count] "+r" (count)
            : [k1] "m" (k1), [k2] "m" (k2), [k3] "m" (k3), [k4] "m" (k4)
            : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void mix_add4(float *dst, const float *a, const float *b, const float *c, const float *d,
            float k1, float k2, float k3, float k4, size_t count)
    {
        ARCH_X86_ASM
        (
            __ASM_EMIT("vbroadcastss    %[k1], %%ymm4")
            __ASM_EMIT("vbroadcastss    %[k2], %%ymm5")
            __ASM_EMIT("vbroadcastss    %[k3], %%ymm6")
            __ASM_EMIT("vbroadcastss    %[k4], %%ymm7")
            // 16x blocks
            __ASM_EMIT("subl            $16, %[count]")
            __ASM_EMIT("jb              2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vmulps          0x00(%[a]), %%ymm4, %%ymm0")
            __ASM_EMIT("vmulps          0x20(%[a]), %%ymm4, %%ymm1")
            __ASM_EMIT("vmulps          0x00(%[b]), %%ymm5, %%ymm2")
            __ASM_EMIT("vmulps          0x20(%[b]), %%ymm5, %%ymm3")
            __ASM_EMIT("vaddps          %%ymm2, %%ymm0, %%ymm0")
            __ASM_EMIT("vaddps          %%ymm3, %%ymm1, %%ymm1")
            __ASM_EMIT("vmulps          0x00(%[c]), %%ymm6, %%ymm2")
            __ASM_EMIT("vmulps          0x20(%[c]), %%ymm6, %%ymm3")
            __ASM_EMIT("vaddps          %%ymm2, %%ymm0, %%ymm0")
            __ASM_EMIT("vaddps          %%ymm3, %%ymm1, %%ymm1")
            __ASM_EMIT("vmulps          0x00(%[d]), %%ymm7, %%ymm2")
            __ASM_EMIT("vmulps          0x20(%[d]), %%ymm7, %%ymm3")
            __ASM_EMIT("vaddps          %%ymm2, %%ymm0, %%ymm0")
            __ASM_EMIT("vaddps          %%ymm3, %%ymm1, %%ymm1")
            __ASM_EMIT("vaddps          0x00(%[dst]), %%ymm0, %%ymm0")
            __ASM_EMIT("vaddps          0x20(%[dst]), %%ymm1, %%ymm1")
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
            __ASM_EMIT("vmovups         %%ymm1, 0x20(%[dst])")
            __ASM_EMIT("add             $0x40, %[a]")
            __ASM_EMIT("add             $0x40, %[b]")
            __ASM_EMIT("add             $0x40, %[c]")
            __ASM_EMIT("add             $0x40, %[d]")
            __ASM_EMIT("add             $0x40, %[dst]")
            __ASM_EMIT("subl            $16, %[count]")
            __ASM_EMIT("jae             1b")
            // 8x block
            __ASM_EMIT("2:")
            __ASM_EMIT("addl            $8, %[count]")
            __ASM_EMIT("jl              4f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vmulps          0x00(%[a]), %%ymm4, %%ymm0")
            __ASM_EMIT("vmulps          0x00(%[b]), %%ymm5, %%ymm2")
            __ASM_EMIT("vaddps          %%ymm2, %%ymm0, %%ymm0")
            __ASM_EMIT("vmulps          0x00(%[c]), %%ymm6, %%ymm2")
            __ASM_EMIT("vaddps          %%ymm2, %%ymm0, %%ymm0")
            __ASM_EMIT("vmulps          0x00(%[d]), %%ymm7, %%ymm2")
            __ASM_EMIT("vaddps          %%ymm2, %%ymm0, %%ymm0")
            __ASM_EMIT("vaddps          0x00(%[dst]), %%ymm0, %%ymm0")
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
            __ASM_EMIT("add             $0x20, %[a]")
            __ASM_EMIT("add             $0x20, %[b]")
            __ASM_EMIT("add             $0x20, %[c]")
            __ASM_EMIT("add             $0x20, %[d]")
            __ASM_EMIT("add             $0x20, %[dst]")
            __ASM_EMIT("subl            $8, %[count]")
            // 4x block
            __ASM_EMIT("4:")
            __ASM_EMIT("addl            $4, %[count]")
            __ASM_EMIT("jl              6f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vmulps          0x00(%[a]), %%xmm4, %%xmm0")
            __ASM_EMIT("vmulps          0x00(%[b]), %%xmm5, %%xmm2")
            __ASM_EMIT("vaddps          %%xmm2, %%xmm0, %%xmm0")
            __ASM_EMIT("vmulps          0x00(%[c]), %%xmm6, %%xmm2")
            __ASM_EMIT("vaddps          %%xmm2, %%xmm0, %%xmm0")
            __ASM_EMIT("vmulps          0x00(%[d]), %%xmm7, %%xmm2")
            __ASM_EMIT("vaddps          %%xmm2, %%xmm0, %%xmm0")
            __ASM_EMIT("vaddps          0x00(%[dst]), %%xmm0, %%xmm0")
            __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("add             $0x10, %[a]")
            __ASM_EMIT("add             $0x10, %[b]")
            __ASM_EMIT("add             $0x10, %[c]")
            __ASM_EMIT("add             $0x10, %[d]")
            __ASM_EMIT("add             $0x10, %[dst]")
            __ASM_EMIT("subl            $4, %[count]")
            // 1x blocks
            __ASM_EMIT("6:")
            __ASM_EMIT("addl            $3, %[count]")
            __ASM_EMIT("jl              8f")
            __ASM_EMIT("7:")
            __ASM_EMIT("vmulss          0x00(%[a]), %%xmm4, %%xmm0")
            __ASM_EMIT("vmulss          0x00(%[b]), %%xmm5, %%xmm2")
            __ASM_EMIT("vaddss          %%xmm2, %%xmm0, %%xmm0")
            __ASM_EMIT("vmulss          0x00(%[c]), %%xmm6, %%xmm2")
            __ASM_EMIT("vaddss          %%xmm2, %%xmm0, %%xmm0")
            __ASM_EMIT("vmulss          0x00(%[d]), %%xmm7, %%xmm2")
            __ASM_EMIT("vaddss          %%xmm2, %%xmm0, %%xmm0")
            __ASM_EMIT("vaddss          0x00(%[dst]), %%xmm0, %%xmm0")
            __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("add             $0x04, %[a]")
            __ASM_EMIT("add             $0x04, %[b]")
            __ASM_EMIT("add             $0x04, %[c]")
            __ASM_EMIT("add             $0x04, %[d]")
            __ASM_EMIT("add             $0x04, %[dst]")
            __ASM_EMIT("decl            %[count]")
            __ASM_EMIT("jge             7b")
            // End
            __ASM_EMIT("8:")
            : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b), [c] "+r" (c), [d] "+r" (d),
              [count] "+r" (count)
            : [k1] "m" (k1), [k2] "m" (k2), [k3] "m" (k3), [k4] "m" (k4)
            : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }
}
#else
    void mix_copy4(float *dst, const float *a, const float *b, const float *c, const float *d,
            float k1, float k2, float k3, float k4, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            __ASM_EMIT("vbroadcastss    %[k1], %%ymm4")
            __ASM_EMIT("vbroadcastss    %[k2], %%ymm5")
            __ASM_EMIT("vbroadcastss    %[k3], %%ymm6")
            __ASM_EMIT("vbroadcastss    %[k4], %%ymm7")
            __ASM_EMIT("xor             %[off], %[off]")
            // 16x blocks
            __ASM_EMIT("sub             $16, %[count]")
            __ASM_EMIT("jb              2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vmulps          0x00(%[a],%[off]), %%ymm4, %%ymm0")
            __ASM_EMIT("vmulps          0x20(%[a],%[off]), %%ymm4, %%ymm1")
            __ASM_EMIT("vmulps          0x00(%[b],%[off]), %%ymm5, %%ymm2")
            __ASM_EMIT("vmulps          0x20(%[b],%[off]), %%ymm5, %%ymm3")
            __ASM_EMIT("vaddps          %%ymm2, %%ymm0, %%ymm0")
            __ASM_EMIT("vaddps          %%ymm3, %%ymm1, %%ymm1")
            __ASM_EMIT("vmulps          0x00(%[c],%[off]), %%ymm6, %%ymm2")
            __ASM_EMIT("vmulps          0x20(%[c],%[off]), %%ymm6, %%ymm3")
            __ASM_EMIT("vaddps          %%ymm2, %%ymm0, %%ymm0")
            __ASM_EMIT("vaddps          %%ymm3, %%ymm1, %%ymm1")
            __ASM_EMIT("vmulps          0x00(%[d],%[off]), %%ymm7, %%ymm2")
            __ASM_EMIT("vmulps          0x20(%[d],%[off]), %%ymm7, %%ymm3")
            __ASM_EMIT("vaddps          %%ymm2, %%ymm0, %%ymm0")
            __ASM_EMIT("vaddps          %%ymm3, %%ymm1, %%ymm1")
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst],%[off])")
            __ASM_EMIT("vmovups         %%ymm1, 0x20(%[dst],%[off])")
            __ASM_EMIT("add             $0x40, %[off]")
            __ASM_EMIT("sub             $16, %[count]")
            __ASM_EMIT("jae             1b")
            // 8x block
            __ASM_EMIT("2:")
            __ASM_EMIT("add             $8, %[count]")
            __ASM_EMIT("jl              4f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vmulps          0x00(%[a],%[off]), %%ymm4, %%ymm0")
            __ASM_EMIT("vmulps          0x00(%[b],%[off]), %%ymm5, %%ymm2")
            __ASM_EMIT("vaddps          %%ymm2, %%ymm0, %%ymm0")
            __ASM_EMIT("vmulps          0x00(%[c],%[off]), %%ymm6, %%ymm2")
            __ASM_EMIT("vaddps          %%ymm2, %%ymm0, %%ymm0")
            __ASM_EMIT("vmulps          0x00(%[d],%[off]), %%ymm7, %%ymm2")
            __ASM_EMIT("vaddps          %%ymm2, %%ymm0, %%ymm0")
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst],%[off])")
            __ASM_EMIT("add             $0x20, %[off]")
            __ASM_EMIT("sub             $8, %[count]")
            // 4x block
            __ASM_EMIT("4:")
            __ASM_EMIT("add             $4, %[count]")
            __ASM_EMIT("jl              6f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vmulps          0x00(%[a],%[off]), %%xmm4, %%xmm0")
            __ASM_EMIT("vmulps          0x00(%[b],%[off]), %%xmm5, %%xmm2")
            __ASM_EMIT("vaddps          %%xmm2, %%xmm0, %%xmm0")
            __ASM_EMIT("vmulps          0x00(%[c],%[off]), %%xmm6, %%xmm2")
            __ASM_EMIT("vaddps          %%xmm2, %%xmm0, %%xmm0")
            __ASM_EMIT("vmulps          0x00(%[d],%[off]), %%xmm7, %%xmm2")
            __ASM_EMIT("vaddps          %%xmm2, %%xmm0, %%xmm0")
            __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst],%[off])")
            __ASM_EMIT("add             $0x10, %[off]")
            __ASM_EMIT("sub             $4, %[count]")
            // 1x blocks
            __ASM_EMIT("6:")
            __ASM_EMIT("add             $3, %[count]")
            __ASM_EMIT("jl              8f")
            __ASM_EMIT("7:")
            __ASM_EMIT("vmulss          0x00(%[a],%[off]), %%xmm4, %%xmm0")
            __ASM_EMIT("vmulss          0x00(%[b],%[off]), %%xmm5, %%xmm2")
            __ASM_EMIT("vaddss          %%xmm2, %%xmm0, %%xmm0")
            __ASM_EMIT("vmulss          0x00(%[c],%[off]), %%xmm6, %%xmm2")
            __ASM_EMIT("vaddss          %%xmm2, %%xmm0, %%xmm0")
            __ASM_EMIT("vmulss          0x00(%[d],%[off]), %%xmm7, %%xmm2")
            __ASM_EMIT("vaddss          %%xmm2, %%xmm0, %%xmm0")
            __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst],%[off])")
            __ASM_EMIT("add             $0x04, %[off]")
            __ASM_EMIT("dec             %[count]")
            __ASM_EMIT("jge             7b")
            // End
            __ASM_EMIT("8:")
            : [count] "+r" (count),
              [off] "=&r" (off)
            : [dst] "r" (dst), [a] "r" (a), [b] "r" (b), [c] "r" (c), [d] "r" (d),
              [k1] "m" (k1), [k2] "m" (k2), [k3] "m" (k3), [k4] "m" (k4)
            : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void mix_add4(float *dst, const float *a, const float *b, const float *c, const float *d,
            float k1, float k2, float k3, float k4, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            __ASM_EMIT("vbroadcastss    %[k1], %%ymm4")
            __ASM_EMIT("vbroadcastss    %[k2], %%ymm5")
            __ASM_EMIT("vbroadcastss    %[k3], %%ymm6")
            __ASM_EMIT("vbroadcastss    %[k4], %%ymm7")
            __ASM_EMIT("xor             %[off], %[off]")
            // 16x blocks
            __ASM_EMITNP("sub           $16, %[count]")
            __ASM_EMIT("jb              2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vmulps          0x00(%[a],%[off]), %%ymm4, %%ymm0")
            __ASM_EMIT("vmulps          0x20(%[a],%[off]), %%ymm4, %%ymm1")
            __ASM_EMIT("vmulps          0x00(%[b],%[off]), %%ymm5, %%ymm2")
            __ASM_EMIT("vmulps          0x20(%[b],%[off]), %%ymm5, %%ymm3")
            __ASM_EMIT("vaddps          %%ymm2, %%ymm0, %%ymm0")
            __ASM_EMIT("vaddps          %%ymm3, %%ymm1, %%ymm1")
            __ASM_EMIT("vmulps          0x00(%[c],%[off]), %%ymm6, %%ymm2")
            __ASM_EMIT("vmulps          0x20(%[c],%[off]), %%ymm6, %%ymm3")
            __ASM_EMIT("vaddps          %%ymm2, %%ymm0, %%ymm0")
            __ASM_EMIT("vaddps          %%ymm3, %%ymm1, %%ymm1")
            __ASM_EMIT("vmulps          0x00(%[d],%[off]), %%ymm7, %%ymm2")
            __ASM_EMIT("vmulps          0x20(%[d],%[off]), %%ymm7, %%ymm3")
            __ASM_EMIT("vaddps          %%ymm2, %%ymm0, %%ymm0")
            __ASM_EMIT("vaddps          %%ymm3, %%ymm1, %%ymm1")
            __ASM_EMIT("vaddps          0x00(%[dst],%[off]), %%ymm0, %%ymm0")
            __ASM_EMIT("vaddps          0x20(%[dst],%[off]), %%ymm1, %%ymm1")
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst],%[off])")
            __ASM_EMIT("vmovups         %%ymm1, 0x20(%[dst],%[off])")
            __ASM_EMIT("add             $0x40, %[off]")
            __ASM_EMIT("sub             $16, %[count]")
            __ASM_EMIT("jae             1b")
            // 8x block
            __ASM_EMIT("2:")
            __ASM_EMIT("add             $8, %[count]")
            __ASM_EMIT("jl              4f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vmulps          0x00(%[a],%[off]), %%ymm4, %%ymm0")
            __ASM_EMIT("vmulps          0x00(%[b],%[off]), %%ymm5, %%ymm2")
            __ASM_EMIT("vaddps          %%ymm2, %%ymm0, %%ymm0")
            __ASM_EMIT("vmulps          0x00(%[c],%[off]), %%ymm6, %%ymm2")
            __ASM_EMIT("vaddps          %%ymm2, %%ymm0, %%ymm0")
            __ASM_EMIT("vmulps          0x00(%[d],%[off]), %%ymm7, %%ymm2")
            __ASM_EMIT("vaddps          %%ymm2, %%ymm0, %%ymm0")
            __ASM_EMIT("vaddps          0x00(%[dst],%[off]), %%ymm0, %%ymm0")
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst],%[off])")
            __ASM_EMIT("add             $0x20, %[off]")
            __ASM_EMIT("sub             $8, %[count]")
            // 4x block
            __ASM_EMIT("4:")
            __ASM_EMIT("add             $4, %[count]")
            __ASM_EMIT("jl              6f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vmulps          0x00(%[a],%[off]), %%xmm4, %%xmm0")
            __ASM_EMIT("vmulps          0x00(%[b],%[off]), %%xmm5, %%xmm2")
            __ASM_EMIT("vaddps          %%xmm2, %%xmm0, %%xmm0")
            __ASM_EMIT("vmulps          0x00(%[c],%[off]), %%xmm6, %%xmm2")
            __ASM_EMIT("vaddps          %%xmm2, %%xmm0, %%xmm0")
            __ASM_EMIT("vmulps          0x00(%[d],%[off]), %%xmm7, %%xmm2")
            __ASM_EMIT("vaddps          %%xmm2, %%xmm0, %%xmm0")
            __ASM_EMIT("vaddps          0x00(%[dst],%[off]), %%xmm0, %%xmm0")
            __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst],%[off])")
            __ASM_EMIT("add             $0x10, %[off]")
            __ASM_EMIT("sub             $4, %[count]")
            // 1x blocks
            __ASM_EMIT("6:")
            __ASM_EMIT("add             $3, %[count]")
            __ASM_EMIT("jl              8f")
            __ASM_EMIT("7:")
            __ASM_EMIT("vmulss          0x00(%[a],%[off]), %%xmm4, %%xmm0")
            __ASM_EMIT("vmulss          0x00(%[b],%[off]), %%xmm5, %%xmm2")
            __ASM_EMIT("vaddss          %%xmm2, %%xmm0, %%xmm0")
            __ASM_EMIT("vmulss          0x00(%[c],%[off]), %%xmm6, %%xmm2")
            __ASM_EMIT("vaddss          %%xmm2, %%xmm0, %%xmm0")
            __ASM_EMIT("vmulss          0x00(%[d],%[off]), %%xmm7, %%xmm2")
            __ASM_EMIT("vaddss          %%xmm2, %%xmm0, %%xmm0")
            __ASM_EMIT("vaddss          0x00(%[dst],%[off]), %%xmm0, %%xmm0")
            __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst],%[off])")
            __ASM_EMIT("add             $0x04, %[off]")
            __ASM_EMIT("dec             %[count]")
            __ASM_EMIT("jge             7b")
            // End
            __ASM_EMIT("8:")
            : [count] "+r" (count),
              [off] "=&r" (off)
            : [dst] "r" (dst), [a] "r" (a), [b] "r" (b), [c] "r" (c), [d] "r" (d),
              [k1] "m" (k1), [k2] "m" (k2), [k3] "m" (k3), [k4] "m" (k4)
            : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }
}
#endif

#endif /* DSP_ARCH_X86_AVX_MIX_H_ */
