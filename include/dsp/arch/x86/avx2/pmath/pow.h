/*
 * pow.h
 *
 *  Created on: 18 дек. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_AVX2_PMATH_POW_H_
#define DSP_ARCH_X86_AVX2_PMATH_POW_H_

#include <dsp/arch/x86/avx2/pmath/exp.h>
#include <dsp/arch/x86/avx2/pmath/log.h>

#ifdef ARCH_X86_64

namespace avx2
{
    void x64_powcv1(float *v, float c, size_t count)
    {
//        float C = logf(c);
//        for (size_t i=0; i<count; ++i)
//            v[i] = expf(v[i] * C);

        IF_ARCH_X86(
            float *src;
            float C[8] __lsp_aligned32;
        );

        ARCH_X86_ASM(
            // Calc C = logf(c)
            __ASM_EMIT("vbroadcastss    %%xmm0, %%ymm0")
            LOGN_CORE_X8
            __ASM_EMIT("vmulps          0x00 + %[LOGC], %%ymm0, %%ymm0")    // ymm0 = 2*y*L*log2(E)
            __ASM_EMIT("vaddps          %%ymm1, %%ymm0, %%ymm0")            // ymm0 = 2*y*L*log2(E) + R = log(c)*log2(E)
            __ASM_EMIT("vmovaps         %%ymm0, %[C]")

            // x32 blocks
            __ASM_EMIT("sub             $32, %[count]")
            __ASM_EMIT("jb              2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vmovups         0x00(%[dst]), %%ymm0")
            __ASM_EMIT("vmovups         0x20(%[dst]), %%ymm4")
            __ASM_EMIT("vmovups         0x40(%[dst]), %%ymm8")
            __ASM_EMIT("vmovups         0x60(%[dst]), %%ymm12")
            __ASM_EMIT("vmulps          %[C], %%ymm0, %%ymm0")
            __ASM_EMIT("vmulps          %[C], %%ymm4, %%ymm4")
            __ASM_EMIT("vmulps          %[C], %%ymm8, %%ymm8")
            __ASM_EMIT("vmulps          %[C], %%ymm12, %%ymm12")
            POW2_CORE_X32
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
            __ASM_EMIT("vmovups         %%ymm4, 0x20(%[dst])")
            __ASM_EMIT("vmovups         %%ymm8, 0x40(%[dst])")
            __ASM_EMIT("vmovups         %%ymm12, 0x60(%[dst])")
            __ASM_EMIT("add             $0x80, %[dst]")
            __ASM_EMIT("sub             $32, %[count]")
            __ASM_EMIT("jae             1b")

            // x16 block
            __ASM_EMIT("2:")
            __ASM_EMIT("add             $16, %[count]")
            __ASM_EMIT("jl              4f")
            __ASM_EMIT("vmovups         0x00(%[dst]), %%ymm0")
            __ASM_EMIT("vmovups         0x20(%[dst]), %%ymm4")
            __ASM_EMIT("vmulps          %[C], %%ymm0, %%ymm0")
            __ASM_EMIT("vmulps          %[C], %%ymm4, %%ymm4")
            POW2_CORE_X16
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
            __ASM_EMIT("vmovups         %%ymm4, 0x20(%[dst])")
            __ASM_EMIT("sub             $16, %[count]")
            __ASM_EMIT("add             $0x40, %[dst]")

            // x8 block
            __ASM_EMIT("4:")
            __ASM_EMIT("add             $8, %[count]")
            __ASM_EMIT("jl              6f")
            __ASM_EMIT("vmovups         0x00(%[dst]), %%ymm0")
            __ASM_EMIT("vmulps          %[C], %%ymm0, %%ymm0")
            POW2_CORE_X8
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
            __ASM_EMIT("sub             $8, %[count]")
            __ASM_EMIT("add             $0x20, %[dst]")

            // x4 block
            __ASM_EMIT("6:")
            __ASM_EMIT("add             $4, %[count]")
            __ASM_EMIT("jl              8f")
            __ASM_EMIT("vmovups         0x00(%[dst]), %%xmm0")
            __ASM_EMIT("vmulps          %[C], %%xmm0, %%xmm0")
            POW2_CORE_X4
            __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("sub             $4, %[count]")
            __ASM_EMIT("add             $0x10, %[dst]")

            // Tail: 1x-3x block
            __ASM_EMIT("8:")
            __ASM_EMIT("add             $4, %[count]")
            __ASM_EMIT("jle             16f")
            __ASM_EMIT("mov             %[dst], %[src]")
            __ASM_EMIT("test            $1, %[count]")
            __ASM_EMIT("jz              10f")
            __ASM_EMIT("vmovss          0x00(%[src]), %%xmm0")
            __ASM_EMIT("add             $4, %[src]")
            __ASM_EMIT("10:")
            __ASM_EMIT("test            $2, %[count]")
            __ASM_EMIT("jz              12f")
            __ASM_EMIT("vmovhps         0x00(%[src]), %%xmm0, %%xmm0")
            __ASM_EMIT("12:")

            __ASM_EMIT("vmulps          %[C], %%xmm0, %%xmm0")
            POW2_CORE_X4

            __ASM_EMIT("test            $1, %[count]")
            __ASM_EMIT("jz              14f")
            __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("add             $4, %[dst]")
            __ASM_EMIT("14:")
            __ASM_EMIT("test            $2, %[count]")
            __ASM_EMIT("jz              16f")
            __ASM_EMIT("vmovhps         %%xmm0, 0x00(%[dst])")

            // End
            __ASM_EMIT("16:")
            __ASM_EMIT("vzeroupper")

            : [dst] "+r" (v), [src] "=&r" (src), [count] "+r" (count),
              "+Yz" (c)
            : [E2C] "o" (EXP2_CONST),
              [L2C] "o" (LOG2_CONST),
              [LOGC] "o" (LOGB_C),
              [C] "m" (C)
            : "cc", "memory",
              "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5", "%xmm6", "%xmm7",
              "%xmm8", "%xmm9", "%xmm10", "%xmm11", "%xmm12", "%xmm13", "%xmm14", "%xmm15"
        );
    }

    void x64_powcv2(float *dst, const float *v, float c, size_t count)
    {
//        float C = logf(c);
//        for (size_t i=0; i<count; ++i)
//            dst[i] = expf(v[i] * C);

        IF_ARCH_X86(
            float C[8] __lsp_aligned32;
        );

        ARCH_X86_ASM(
            // Calc C = logf(c)
            __ASM_EMIT("vbroadcastss    %%xmm0, %%ymm0")
            LOGN_CORE_X8
            __ASM_EMIT("vmulps          0x00 + %[LOGC], %%ymm0, %%ymm0")    // ymm0 = 2*y*L*log2(E)
            __ASM_EMIT("vaddps          %%ymm1, %%ymm0, %%ymm0")            // ymm0 = 2*y*L*log2(E) + R = log(c)*log2(E)
            __ASM_EMIT("vmovaps         %%ymm0, %[C]")

            // x32 blocks
            __ASM_EMIT("sub             $32, %[count]")
            __ASM_EMIT("jb              2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
            __ASM_EMIT("vmovups         0x20(%[src]), %%ymm4")
            __ASM_EMIT("vmovups         0x40(%[src]), %%ymm8")
            __ASM_EMIT("vmovups         0x60(%[src]), %%ymm12")
            __ASM_EMIT("vmulps          %[C], %%ymm0, %%ymm0")
            __ASM_EMIT("vmulps          %[C], %%ymm4, %%ymm4")
            __ASM_EMIT("vmulps          %[C], %%ymm8, %%ymm8")
            __ASM_EMIT("vmulps          %[C], %%ymm12, %%ymm12")
            POW2_CORE_X32
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
            __ASM_EMIT("vmovups         %%ymm4, 0x20(%[dst])")
            __ASM_EMIT("vmovups         %%ymm8, 0x40(%[dst])")
            __ASM_EMIT("vmovups         %%ymm12, 0x60(%[dst])")
            __ASM_EMIT("add             $0x80, %[src]")
            __ASM_EMIT("add             $0x80, %[dst]")
            __ASM_EMIT("sub             $32, %[count]")
            __ASM_EMIT("jae             1b")

            // x16 block
            __ASM_EMIT("2:")
            __ASM_EMIT("add             $16, %[count]")
            __ASM_EMIT("jl              4f")
            __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
            __ASM_EMIT("vmovups         0x20(%[src]), %%ymm4")
            __ASM_EMIT("vmulps          %[C], %%ymm0, %%ymm0")
            __ASM_EMIT("vmulps          %[C], %%ymm4, %%ymm4")
            POW2_CORE_X16
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
            __ASM_EMIT("vmovups         %%ymm4, 0x20(%[dst])")
            __ASM_EMIT("sub             $16, %[count]")
            __ASM_EMIT("add             $0x40, %[src]")
            __ASM_EMIT("add             $0x40, %[dst]")

            // x8 block
            __ASM_EMIT("4:")
            __ASM_EMIT("add             $8, %[count]")
            __ASM_EMIT("jl              6f")
            __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
            __ASM_EMIT("vmulps          %[C], %%ymm0, %%ymm0")
            POW2_CORE_X8
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
            __ASM_EMIT("sub             $8, %[count]")
            __ASM_EMIT("add             $0x20, %[src]")
            __ASM_EMIT("add             $0x20, %[dst]")

            // x4 block
            __ASM_EMIT("6:")
            __ASM_EMIT("add             $4, %[count]")
            __ASM_EMIT("jl              8f")
            __ASM_EMIT("vmovups         0x00(%[src]), %%xmm0")
            __ASM_EMIT("vmulps          %[C], %%xmm0, %%xmm0")
            POW2_CORE_X4
            __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("sub             $4, %[count]")
            __ASM_EMIT("add             $0x10, %[src]")
            __ASM_EMIT("add             $0x10, %[dst]")

            // Tail: 1x-3x block
            __ASM_EMIT("8:")
            __ASM_EMIT("add             $4, %[count]")
            __ASM_EMIT("jle             16f")
            __ASM_EMIT("test            $1, %[count]")
            __ASM_EMIT("jz              10f")
            __ASM_EMIT("vmovss          0x00(%[src]), %%xmm0")
            __ASM_EMIT("add             $4, %[src]")
            __ASM_EMIT("10:")
            __ASM_EMIT("test            $2, %[count]")
            __ASM_EMIT("jz              12f")
            __ASM_EMIT("vmovhps         0x00(%[src]), %%xmm0, %%xmm0")
            __ASM_EMIT("12:")

            __ASM_EMIT("vmulps          %[C], %%xmm0, %%xmm0")
            POW2_CORE_X4

            __ASM_EMIT("test            $1, %[count]")
            __ASM_EMIT("jz              14f")
            __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("add             $4, %[dst]")
            __ASM_EMIT("14:")
            __ASM_EMIT("test            $2, %[count]")
            __ASM_EMIT("jz              16f")
            __ASM_EMIT("vmovhps         %%xmm0, 0x00(%[dst])")

            // End
            __ASM_EMIT("16:")
            __ASM_EMIT("vzeroupper")

            : [dst] "+r" (dst), [src] "+r" (v), [count] "+r" (count),
              "+Yz" (c)
            : [E2C] "o" (EXP2_CONST),
              [L2C] "o" (LOG2_CONST),
              [LOGC] "o" (LOGB_C),
              [C] "m" (C)
            : "cc", "memory",
              "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5", "%xmm6", "%xmm7",
              "%xmm8", "%xmm9", "%xmm10", "%xmm11", "%xmm12", "%xmm13", "%xmm14", "%xmm15"
        );
    }

    void x64_powvc1(float *c, float v, size_t count)
    {
//        for (size_t i=0; i<count; ++i)
//            c[i] = expf(v * logf(c[i]));

        IF_ARCH_X86(
            float *src;
            float V[8] __lsp_aligned32;
        );

        ARCH_X86_ASM(
            __ASM_EMIT("vbroadcastss    %%xmm0, %%ymm0")
            __ASM_EMIT("vmovaps         %%ymm0, %[V]")

            // x32 blocks
            __ASM_EMIT("sub             $32, %[count]")
            __ASM_EMIT("jb              2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vmovups         0x00(%[dst]), %%ymm0")
            __ASM_EMIT("vmovups         0x20(%[dst]), %%ymm4")
            __ASM_EMIT("vmovups         0x40(%[dst]), %%ymm8")
            __ASM_EMIT("vmovups         0x60(%[dst]), %%ymm12")
            // logf(c)
            LOGN_CORE_X32
            __ASM_EMIT("vmulps          0x00 + %[LOGC], %%ymm0, %%ymm0")        // ymm0 = 2*y*L*log2(E)
            __ASM_EMIT("vmulps          0x00 + %[LOGC], %%ymm4, %%ymm4")
            __ASM_EMIT("vmulps          0x00 + %[LOGC], %%ymm8, %%ymm8")
            __ASM_EMIT("vmulps          0x00 + %[LOGC], %%ymm12, %%ymm12")
            __ASM_EMIT("vaddps          %%ymm1, %%ymm0, %%ymm0")                // ymm0 = 2*y*L*log2(E)+R
            __ASM_EMIT("vaddps          %%ymm5, %%ymm4, %%ymm4")
            __ASM_EMIT("vaddps          %%ymm9, %%ymm8, %%ymm8")
            __ASM_EMIT("vaddps          %%ymm13, %%ymm12, %%ymm12")
            __ASM_EMIT("vmulps          %[V], %%ymm0, %%ymm0")                  // ymm0 = (2*y*L*log2(E)+R)*v
            __ASM_EMIT("vmulps          %[V], %%ymm4, %%ymm4")
            __ASM_EMIT("vmulps          %[V], %%ymm8, %%ymm8")
            __ASM_EMIT("vmulps          %[V], %%ymm12, %%ymm12")
            // expf(v * logf(c))
            POW2_CORE_X32
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
            __ASM_EMIT("vmovups         %%ymm4, 0x20(%[dst])")
            __ASM_EMIT("vmovups         %%ymm8, 0x40(%[dst])")
            __ASM_EMIT("vmovups         %%ymm12, 0x60(%[dst])")
            __ASM_EMIT("add             $0x80, %[dst]")
            __ASM_EMIT("sub             $32, %[count]")
            __ASM_EMIT("jae             1b")

            // x16 block
            __ASM_EMIT("2:")
            __ASM_EMIT("add             $16, %[count]")
            __ASM_EMIT("jl              4f")
            __ASM_EMIT("vmovups         0x00(%[dst]), %%ymm0")
            __ASM_EMIT("vmovups         0x20(%[dst]), %%ymm4")
            // logf(c)
            LOGN_CORE_X16
            __ASM_EMIT("vmulps          0x00 + %[LOGC], %%ymm0, %%ymm0")        // ymm0 = 2*y*L*log2(E)
            __ASM_EMIT("vmulps          0x00 + %[LOGC], %%ymm4, %%ymm4")
            __ASM_EMIT("vaddps          %%ymm1, %%ymm0, %%ymm0")                // ymm0 = 2*y*L*log2(E)+R
            __ASM_EMIT("vaddps          %%ymm5, %%ymm4, %%ymm4")
            __ASM_EMIT("vmulps          %[V], %%ymm0, %%ymm0")                  // ymm0 = (2*y*L*log2(E)+R)*v
            __ASM_EMIT("vmulps          %[V], %%ymm4, %%ymm4")
            // expf(v * logf(c))
            POW2_CORE_X16
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
            __ASM_EMIT("vmovups         %%ymm4, 0x20(%[dst])")
            __ASM_EMIT("sub             $16, %[count]")
            __ASM_EMIT("add             $0x40, %[dst]")

            // x8 block
            __ASM_EMIT("4:")
            __ASM_EMIT("add             $8, %[count]")
            __ASM_EMIT("jl              6f")
            __ASM_EMIT("vmovups         0x00(%[dst]), %%ymm0")
            // logf(c)
            LOGN_CORE_X8
            __ASM_EMIT("vmulps          0x00 + %[LOGC], %%ymm0, %%ymm0")        // ymm0 = 2*y*L*log2(E)
            __ASM_EMIT("vaddps          %%ymm1, %%ymm0, %%ymm0")                // ymm0 = 2*y*L*log2(E)+R
            __ASM_EMIT("vmulps          %[V], %%ymm0, %%ymm0")                  // ymm0 = (2*y*L*log2(E)+R)*v
            // expf(v * logf(c))
            POW2_CORE_X8
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
            __ASM_EMIT("sub             $8, %[count]")
            __ASM_EMIT("add             $0x20, %[dst]")

            // x4 block
            __ASM_EMIT("6:")
            __ASM_EMIT("add             $4, %[count]")
            __ASM_EMIT("jl              8f")
            __ASM_EMIT("vmovups         0x00(%[dst]), %%xmm0")
            // logf(c)
            LOGN_CORE_X4
            __ASM_EMIT("vmulps          0x00 + %[LOGC], %%xmm0, %%xmm0")        // xmm0 = 2*y*L*log2(E)
            __ASM_EMIT("vaddps          %%xmm1, %%xmm0, %%xmm0")                // xmm0 = 2*y*L*log2(E)+R
            __ASM_EMIT("vmulps          %[V], %%xmm0, %%xmm0")                  // xmm0 = (2*y*L*log2(E)+R)*v
            // expf(v * logf(c))
            POW2_CORE_X4
            __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("sub             $4, %[count]")
            __ASM_EMIT("add             $0x10, %[dst]")

            // Tail: 1x-3x block
            __ASM_EMIT("8:")
            __ASM_EMIT("add             $4, %[count]")
            __ASM_EMIT("jle             16f")
            __ASM_EMIT("mov             %[dst], %[src]")
            __ASM_EMIT("test            $1, %[count]")
            __ASM_EMIT("jz              10f")
            __ASM_EMIT("vmovss          0x00(%[src]), %%xmm0")
            __ASM_EMIT("add             $4, %[src]")
            __ASM_EMIT("10:")
            __ASM_EMIT("test            $2, %[count]")
            __ASM_EMIT("jz              12f")
            __ASM_EMIT("vmovhps         0x00(%[src]), %%xmm0, %%xmm0")
            __ASM_EMIT("12:")

            // logf(c)
            LOGN_CORE_X4
            __ASM_EMIT("vmulps          0x00 + %[LOGC], %%xmm0, %%xmm0")        // xmm0 = 2*y*L*log2(E)
            __ASM_EMIT("vaddps          %%xmm1, %%xmm0, %%xmm0")                // xmm0 = 2*y*L*log2(E)+R
            __ASM_EMIT("vmulps          %[V], %%xmm0, %%xmm0")                  // xmm0 = (2*y*L*log2(E)+R)*v
            // expf(v * logf(c))
            POW2_CORE_X4

            __ASM_EMIT("test            $1, %[count]")
            __ASM_EMIT("jz              14f")
            __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("add             $4, %[dst]")
            __ASM_EMIT("14:")
            __ASM_EMIT("test            $2, %[count]")
            __ASM_EMIT("jz              16f")
            __ASM_EMIT("vmovhps         %%xmm0, 0x00(%[dst])")

            // End
            __ASM_EMIT("16:")
            __ASM_EMIT("vzeroupper")

            : [dst] "+r" (c), [src] "=&r" (src), [count] "+r" (count),
              "+Yz" (v)
            : [E2C] "o" (EXP2_CONST),
              [L2C] "o" (LOG2_CONST),
              [LOGC] "o" (LOGB_C),
              [V] "m" (V)
            : "cc", "memory",
              "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5", "%xmm6", "%xmm7",
              "%xmm8", "%xmm9", "%xmm10", "%xmm11", "%xmm12", "%xmm13", "%xmm14", "%xmm15"
        );
    }

    void x64_powvc2(float *dst, const float *c, float v, size_t count)
    {
//        for (size_t i=0; i<count; ++i)
//            dst[i] = expf(v * logf(c[i]));

        IF_ARCH_X86(
            float V[8] __lsp_aligned32;
        );

        ARCH_X86_ASM(
            __ASM_EMIT("vbroadcastss    %%xmm0, %%ymm0")
            __ASM_EMIT("vmovaps         %%ymm0, %[V]")

            // x32 blocks
            __ASM_EMIT("sub             $32, %[count]")
            __ASM_EMIT("jb              2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
            __ASM_EMIT("vmovups         0x20(%[src]), %%ymm4")
            __ASM_EMIT("vmovups         0x40(%[src]), %%ymm8")
            __ASM_EMIT("vmovups         0x60(%[src]), %%ymm12")
            // logf(c)
            LOGN_CORE_X32
            __ASM_EMIT("vmulps          0x00 + %[LOGC], %%ymm0, %%ymm0")        // ymm0 = 2*y*L*log2(E)
            __ASM_EMIT("vmulps          0x00 + %[LOGC], %%ymm4, %%ymm4")
            __ASM_EMIT("vmulps          0x00 + %[LOGC], %%ymm8, %%ymm8")
            __ASM_EMIT("vmulps          0x00 + %[LOGC], %%ymm12, %%ymm12")
            __ASM_EMIT("vaddps          %%ymm1, %%ymm0, %%ymm0")                // ymm0 = 2*y*L*log2(E)+R
            __ASM_EMIT("vaddps          %%ymm5, %%ymm4, %%ymm4")
            __ASM_EMIT("vaddps          %%ymm9, %%ymm8, %%ymm8")
            __ASM_EMIT("vaddps          %%ymm13, %%ymm12, %%ymm12")
            __ASM_EMIT("vmulps          %[V], %%ymm0, %%ymm0")                  // ymm0 = (2*y*L*log2(E)+R)*v
            __ASM_EMIT("vmulps          %[V], %%ymm4, %%ymm4")
            __ASM_EMIT("vmulps          %[V], %%ymm8, %%ymm8")
            __ASM_EMIT("vmulps          %[V], %%ymm12, %%ymm12")
            // expf(v * logf(c))
            POW2_CORE_X32
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
            __ASM_EMIT("vmovups         %%ymm4, 0x20(%[dst])")
            __ASM_EMIT("vmovups         %%ymm8, 0x40(%[dst])")
            __ASM_EMIT("vmovups         %%ymm12, 0x60(%[dst])")
            __ASM_EMIT("add             $0x80, %[src]")
            __ASM_EMIT("add             $0x80, %[dst]")
            __ASM_EMIT("sub             $32, %[count]")
            __ASM_EMIT("jae             1b")

            // x16 block
            __ASM_EMIT("2:")
            __ASM_EMIT("add             $16, %[count]")
            __ASM_EMIT("jl              4f")
            __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
            __ASM_EMIT("vmovups         0x20(%[src]), %%ymm4")
            // logf(c)
            LOGN_CORE_X16
            __ASM_EMIT("vmulps          0x00 + %[LOGC], %%ymm0, %%ymm0")        // ymm0 = 2*y*L*log2(E)
            __ASM_EMIT("vmulps          0x00 + %[LOGC], %%ymm4, %%ymm4")
            __ASM_EMIT("vaddps          %%ymm1, %%ymm0, %%ymm0")                // ymm0 = 2*y*L*log2(E)+R
            __ASM_EMIT("vaddps          %%ymm5, %%ymm4, %%ymm4")
            __ASM_EMIT("vmulps          %[V], %%ymm0, %%ymm0")                  // ymm0 = (2*y*L*log2(E)+R)*v
            __ASM_EMIT("vmulps          %[V], %%ymm4, %%ymm4")
            // expf(v * logf(c))
            POW2_CORE_X16
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
            __ASM_EMIT("vmovups         %%ymm4, 0x20(%[dst])")
            __ASM_EMIT("sub             $16, %[count]")
            __ASM_EMIT("add             $0x40, %[src]")
            __ASM_EMIT("add             $0x40, %[dst]")

            // x8 block
            __ASM_EMIT("4:")
            __ASM_EMIT("add             $8, %[count]")
            __ASM_EMIT("jl              6f")
            __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
            // logf(c)
            LOGN_CORE_X8
            __ASM_EMIT("vmulps          0x00 + %[LOGC], %%ymm0, %%ymm0")        // ymm0 = 2*y*L*log2(E)
            __ASM_EMIT("vaddps          %%ymm1, %%ymm0, %%ymm0")                // ymm0 = 2*y*L*log2(E)+R
            __ASM_EMIT("vmulps          %[V], %%ymm0, %%ymm0")                  // ymm0 = (2*y*L*log2(E)+R)*v
            // expf(v * logf(c))
            POW2_CORE_X8
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
            __ASM_EMIT("sub             $8, %[count]")
            __ASM_EMIT("add             $0x20, %[src]")
            __ASM_EMIT("add             $0x20, %[dst]")

            // x4 block
            __ASM_EMIT("6:")
            __ASM_EMIT("add             $4, %[count]")
            __ASM_EMIT("jl              8f")
            __ASM_EMIT("vmovups         0x00(%[src]), %%xmm0")
            // logf(c)
            LOGN_CORE_X4
            __ASM_EMIT("vmulps          0x00 + %[LOGC], %%xmm0, %%xmm0")        // xmm0 = 2*y*L*log2(E)
            __ASM_EMIT("vaddps          %%xmm1, %%xmm0, %%xmm0")                // xmm0 = 2*y*L*log2(E)+R
            __ASM_EMIT("vmulps          %[V], %%xmm0, %%xmm0")                  // xmm0 = (2*y*L*log2(E)+R)*v
            // expf(v * logf(c))
            POW2_CORE_X4
            __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("sub             $4, %[count]")
            __ASM_EMIT("add             $0x10, %[src]")
            __ASM_EMIT("add             $0x10, %[dst]")

            // Tail: 1x-3x block
            __ASM_EMIT("8:")
            __ASM_EMIT("add             $4, %[count]")
            __ASM_EMIT("jle             16f")
            __ASM_EMIT("test            $1, %[count]")
            __ASM_EMIT("jz              10f")
            __ASM_EMIT("vmovss          0x00(%[src]), %%xmm0")
            __ASM_EMIT("add             $4, %[src]")
            __ASM_EMIT("10:")
            __ASM_EMIT("test            $2, %[count]")
            __ASM_EMIT("jz              12f")
            __ASM_EMIT("vmovhps         0x00(%[src]), %%xmm0, %%xmm0")
            __ASM_EMIT("12:")

            // logf(c)
            LOGN_CORE_X4
            __ASM_EMIT("vmulps          0x00 + %[LOGC], %%xmm0, %%xmm0")        // xmm0 = 2*y*L*log2(E)
            __ASM_EMIT("vaddps          %%xmm1, %%xmm0, %%xmm0")                // xmm0 = 2*y*L*log2(E)+R
            __ASM_EMIT("vmulps          %[V], %%xmm0, %%xmm0")                  // xmm0 = (2*y*L*log2(E)+R)*v
            // expf(v * logf(c))
            POW2_CORE_X4

            __ASM_EMIT("test            $1, %[count]")
            __ASM_EMIT("jz              14f")
            __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("add             $4, %[dst]")
            __ASM_EMIT("14:")
            __ASM_EMIT("test            $2, %[count]")
            __ASM_EMIT("jz              16f")
            __ASM_EMIT("vmovhps         %%xmm0, 0x00(%[dst])")

            // End
            __ASM_EMIT("16:")
            __ASM_EMIT("vzeroupper")

            : [dst] "+r" (dst), [src] "+r" (c), [count] "+r" (count),
              "+Yz" (v)
            : [E2C] "o" (EXP2_CONST),
              [L2C] "o" (LOG2_CONST),
              [LOGC] "o" (LOGB_C),
              [V] "m" (V)
            : "cc", "memory",
              "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5", "%xmm6", "%xmm7",
              "%xmm8", "%xmm9", "%xmm10", "%xmm11", "%xmm12", "%xmm13", "%xmm14", "%xmm15"
        );
    }

    void x64_powvx1(float *v, const float *x, size_t count)
    {
//        for (size_t i=0; i<count; ++i)
//            v[i] = expf(x[i] * logf(v[i]));
        IF_ARCH_X86(float *src);

        ARCH_X86_ASM(
            // x32 blocks
            __ASM_EMIT("sub             $32, %[count]")
            __ASM_EMIT("jb              2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vmovups         0x00(%[dst]), %%ymm0")
            __ASM_EMIT("vmovups         0x20(%[dst]), %%ymm4")
            __ASM_EMIT("vmovups         0x40(%[dst]), %%ymm8")
            __ASM_EMIT("vmovups         0x60(%[dst]), %%ymm12")
            // logf(v)
            LOGN_CORE_X32
            __ASM_EMIT("vmulps          0x00 + %[LOGC], %%ymm0, %%ymm0")        // ymm0 = 2*y*L*log2(E)
            __ASM_EMIT("vmulps          0x00 + %[LOGC], %%ymm4, %%ymm4")
            __ASM_EMIT("vmulps          0x00 + %[LOGC], %%ymm8, %%ymm8")
            __ASM_EMIT("vmulps          0x00 + %[LOGC], %%ymm12, %%ymm12")
            __ASM_EMIT("vmovups         0x00(%[x]), %%ymm2")                    // ymm2 = x
            __ASM_EMIT("vmovups         0x20(%[x]), %%ymm6")
            __ASM_EMIT("vmovups         0x40(%[x]), %%ymm10")
            __ASM_EMIT("vmovups         0x60(%[x]), %%ymm14")
            __ASM_EMIT("vaddps          %%ymm1, %%ymm0, %%ymm0")                // ymm0 = 2*y*L*log2(E) + R
            __ASM_EMIT("vaddps          %%ymm5, %%ymm4, %%ymm4")
            __ASM_EMIT("vaddps          %%ymm9, %%ymm8, %%ymm8")
            __ASM_EMIT("vaddps          %%ymm13, %%ymm12, %%ymm12")
            __ASM_EMIT("vmulps          %%ymm2, %%ymm0, %%ymm0")                // ymm0 = (2*y*L*log2(E) + R)*x
            __ASM_EMIT("vmulps          %%ymm6, %%ymm4, %%ymm4")
            __ASM_EMIT("vmulps          %%ymm10, %%ymm8, %%ymm8")
            __ASM_EMIT("vmulps          %%ymm14, %%ymm12, %%ymm12")
            // expf(x * logf(v))
            POW2_CORE_X32
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
            __ASM_EMIT("vmovups         %%ymm4, 0x20(%[dst])")
            __ASM_EMIT("vmovups         %%ymm8, 0x40(%[dst])")
            __ASM_EMIT("vmovups         %%ymm12, 0x60(%[dst])")
            __ASM_EMIT("add             $0x80, %[x]")
            __ASM_EMIT("add             $0x80, %[dst]")
            __ASM_EMIT("sub             $32, %[count]")
            __ASM_EMIT("jae             1b")

            // x16 block
            __ASM_EMIT("2:")
            __ASM_EMIT("add             $16, %[count]")
            __ASM_EMIT("jl              4f")
            __ASM_EMIT("vmovups         0x00(%[dst]), %%ymm0")
            __ASM_EMIT("vmovups         0x20(%[dst]), %%ymm4")
            // logf(v)
            LOGN_CORE_X16
            __ASM_EMIT("vmulps          0x00 + %[LOGC], %%ymm0, %%ymm0")        // ymm0 = 2*y*L*log2(E)
            __ASM_EMIT("vmulps          0x00 + %[LOGC], %%ymm4, %%ymm4")
            __ASM_EMIT("vmovups         0x00(%[x]), %%ymm2")                    // ymm2 = x
            __ASM_EMIT("vmovups         0x20(%[x]), %%ymm6")
            __ASM_EMIT("vaddps          %%ymm1, %%ymm0, %%ymm0")                // ymm0 = 2*y*L*log2(E) + R
            __ASM_EMIT("vaddps          %%ymm5, %%ymm4, %%ymm4")
            __ASM_EMIT("vmulps          %%ymm2, %%ymm0, %%ymm0")                // ymm0 = (2*y*L*log2(E) + R)*x
            __ASM_EMIT("vmulps          %%ymm6, %%ymm4, %%ymm4")
            // expf(x * logf(v))
            POW2_CORE_X16
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
            __ASM_EMIT("vmovups         %%ymm4, 0x20(%[dst])")
            __ASM_EMIT("sub             $16, %[count]")
            __ASM_EMIT("add             $0x40, %[x]")
            __ASM_EMIT("add             $0x40, %[dst]")

            // x8 block
            __ASM_EMIT("4:")
            __ASM_EMIT("add             $8, %[count]")
            __ASM_EMIT("jl              6f")
            __ASM_EMIT("vmovups         0x00(%[dst]), %%ymm0")
            // logf(v)
            LOGN_CORE_X8
            __ASM_EMIT("vmulps          0x00 + %[LOGC], %%ymm0, %%ymm0")        // ymm0 = 2*y*L*log2(E)
            __ASM_EMIT("vmovups         0x00(%[x]), %%ymm2")                    // ymm2 = x
            __ASM_EMIT("vaddps          %%ymm1, %%ymm0, %%ymm0")                // ymm0 = 2*y*L*log2(E) + R
            __ASM_EMIT("vmulps          %%ymm2, %%ymm0, %%ymm0")                // ymm0 = (2*y*L*log2(E) + R)*x
            // expf(x * logf(v))
            POW2_CORE_X8
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
            __ASM_EMIT("sub             $8, %[count]")
            __ASM_EMIT("add             $0x20, %[x]")
            __ASM_EMIT("add             $0x20, %[dst]")

            // x4 block
            __ASM_EMIT("6:")
            __ASM_EMIT("add             $4, %[count]")
            __ASM_EMIT("jl              8f")
            __ASM_EMIT("vmovups         0x00(%[dst]), %%xmm0")
            // logf(v)
            LOGN_CORE_X4
            __ASM_EMIT("vmulps          0x00 + %[LOGC], %%xmm0, %%xmm0")        // xmm0 = 2*y*L*log2(E)
            __ASM_EMIT("vmovups         0x00(%[x]), %%xmm2")                    // xmm2 = x
            __ASM_EMIT("vaddps          %%xmm1, %%xmm0, %%xmm0")                // xmm0 = 2*y*L*log2(E) + R
            __ASM_EMIT("vmulps          %%xmm2, %%xmm0, %%xmm0")                // xmm0 = (2*y*L*log2(E) + R)*x
            // expf(x * logf(v))
            POW2_CORE_X4
            __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("sub             $4, %[count]")
            __ASM_EMIT("add             $0x10, %[x]")
            __ASM_EMIT("add             $0x10, %[dst]")

            // Tail: 1x-3x block
            __ASM_EMIT("8:")
            __ASM_EMIT("add             $4, %[count]")
            __ASM_EMIT("jle             16f")
            __ASM_EMIT("mov             %[dst], %[src]")
            __ASM_EMIT("test            $1, %[count]")
            __ASM_EMIT("jz              10f")
            __ASM_EMIT("vmovss          0x00(%[src]), %%xmm0")
            __ASM_EMIT("vmovss          0x00(%[x]), %%xmm7")
            __ASM_EMIT("add             $4, %[src]")
            __ASM_EMIT("add             $4, %[x]")
            __ASM_EMIT("10:")
            __ASM_EMIT("test            $2, %[count]")
            __ASM_EMIT("jz              12f")
            __ASM_EMIT("vmovhps         0x00(%[src]), %%xmm0, %%xmm0")
            __ASM_EMIT("vmovhps         0x00(%[x]), %%xmm7, %%xmm7")
            __ASM_EMIT("12:")

            // logf(v)
            LOGN_CORE_X4
            __ASM_EMIT("vmulps          0x00 + %[LOGC], %%xmm0, %%xmm0")        // xmm0 = 2*y*L*log2(E)
            __ASM_EMIT("vaddps          %%xmm1, %%xmm0, %%xmm0")                // xmm0 = 2*y*L*log2(E) + R
            __ASM_EMIT("vmulps          %%xmm7, %%xmm0, %%xmm0")                // xmm0 = (2*y*L*log2(E) + R)*x
            // expf(x * logf(v))
            POW2_CORE_X4

            __ASM_EMIT("test            $1, %[count]")
            __ASM_EMIT("jz              14f")
            __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("add             $4, %[dst]")
            __ASM_EMIT("14:")
            __ASM_EMIT("test            $2, %[count]")
            __ASM_EMIT("jz              16f")
            __ASM_EMIT("vmovhps         %%xmm0, 0x00(%[dst])")

            // End
            __ASM_EMIT("16:")
            __ASM_EMIT("vzeroupper")

            : [dst] "+r" (v), [x] "+r" (x), [src] "=&r" (src), [count] "+r" (count)
            : [E2C] "o" (EXP2_CONST),
              [L2C] "o" (LOG2_CONST),
              [LOGC] "o" (LOGB_C)
            : "cc", "memory",
              "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5", "%xmm6", "%xmm7",
              "%xmm8", "%xmm9", "%xmm10", "%xmm11", "%xmm12", "%xmm13", "%xmm14", "%xmm15"
        );
    }

    void x64_powvx2(float *dst, const float *v, const float *x, size_t count)
    {
//        for (size_t i=0; i<count; ++i)
//            dst[i] = expf(x[i] * logf(v[i]));
        ARCH_X86_ASM(
            // x32 blocks
            __ASM_EMIT("sub             $32, %[count]")
            __ASM_EMIT("jb              2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
            __ASM_EMIT("vmovups         0x20(%[src]), %%ymm4")
            __ASM_EMIT("vmovups         0x40(%[src]), %%ymm8")
            __ASM_EMIT("vmovups         0x60(%[src]), %%ymm12")
            // logf(v)
            LOGN_CORE_X32
            __ASM_EMIT("vmulps          0x00 + %[LOGC], %%ymm0, %%ymm0")        // ymm0 = 2*y*L*log2(E)
            __ASM_EMIT("vmulps          0x00 + %[LOGC], %%ymm4, %%ymm4")
            __ASM_EMIT("vmulps          0x00 + %[LOGC], %%ymm8, %%ymm8")
            __ASM_EMIT("vmulps          0x00 + %[LOGC], %%ymm12, %%ymm12")
            __ASM_EMIT("vmovups         0x00(%[x]), %%ymm2")                    // ymm2 = x
            __ASM_EMIT("vmovups         0x20(%[x]), %%ymm6")
            __ASM_EMIT("vmovups         0x40(%[x]), %%ymm10")
            __ASM_EMIT("vmovups         0x60(%[x]), %%ymm14")
            __ASM_EMIT("vaddps          %%ymm1, %%ymm0, %%ymm0")                // ymm0 = 2*y*L*log2(E) + R
            __ASM_EMIT("vaddps          %%ymm5, %%ymm4, %%ymm4")
            __ASM_EMIT("vaddps          %%ymm9, %%ymm8, %%ymm8")
            __ASM_EMIT("vaddps          %%ymm13, %%ymm12, %%ymm12")
            __ASM_EMIT("vmulps          %%ymm2, %%ymm0, %%ymm0")                // ymm0 = (2*y*L*log2(E) + R)*x
            __ASM_EMIT("vmulps          %%ymm6, %%ymm4, %%ymm4")
            __ASM_EMIT("vmulps          %%ymm10, %%ymm8, %%ymm8")
            __ASM_EMIT("vmulps          %%ymm14, %%ymm12, %%ymm12")
            // expf(x * logf(v))
            POW2_CORE_X32
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
            __ASM_EMIT("vmovups         %%ymm4, 0x20(%[dst])")
            __ASM_EMIT("vmovups         %%ymm8, 0x40(%[dst])")
            __ASM_EMIT("vmovups         %%ymm12, 0x60(%[dst])")
            __ASM_EMIT("add             $0x80, %[x]")
            __ASM_EMIT("add             $0x80, %[src]")
            __ASM_EMIT("add             $0x80, %[dst]")
            __ASM_EMIT("sub             $32, %[count]")
            __ASM_EMIT("jae             1b")

            // x16 block
            __ASM_EMIT("2:")
            __ASM_EMIT("add             $16, %[count]")
            __ASM_EMIT("jl              4f")
            __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
            __ASM_EMIT("vmovups         0x20(%[src]), %%ymm4")
            // logf(v)
            LOGN_CORE_X16
            __ASM_EMIT("vmulps          0x00 + %[LOGC], %%ymm0, %%ymm0")        // ymm0 = 2*y*L*log2(E)
            __ASM_EMIT("vmulps          0x00 + %[LOGC], %%ymm4, %%ymm4")
            __ASM_EMIT("vmovups         0x00(%[x]), %%ymm2")                    // ymm2 = x
            __ASM_EMIT("vmovups         0x20(%[x]), %%ymm6")
            __ASM_EMIT("vaddps          %%ymm1, %%ymm0, %%ymm0")                // ymm0 = 2*y*L*log2(E) + R
            __ASM_EMIT("vaddps          %%ymm5, %%ymm4, %%ymm4")
            __ASM_EMIT("vmulps          %%ymm2, %%ymm0, %%ymm0")                // ymm0 = (2*y*L*log2(E) + R)*x
            __ASM_EMIT("vmulps          %%ymm6, %%ymm4, %%ymm4")
            // expf(x * logf(v))
            POW2_CORE_X16
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
            __ASM_EMIT("vmovups         %%ymm4, 0x20(%[dst])")
            __ASM_EMIT("sub             $16, %[count]")
            __ASM_EMIT("add             $0x40, %[x]")
            __ASM_EMIT("add             $0x40, %[src]")
            __ASM_EMIT("add             $0x40, %[dst]")

            // x8 block
            __ASM_EMIT("4:")
            __ASM_EMIT("add             $8, %[count]")
            __ASM_EMIT("jl              6f")
            __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
            // logf(v)
            LOGN_CORE_X8
            __ASM_EMIT("vmulps          0x00 + %[LOGC], %%ymm0, %%ymm0")        // ymm0 = 2*y*L*log2(E)
            __ASM_EMIT("vmovups         0x00(%[x]), %%ymm2")                    // ymm2 = x
            __ASM_EMIT("vaddps          %%ymm1, %%ymm0, %%ymm0")                // ymm0 = 2*y*L*log2(E) + R
            __ASM_EMIT("vmulps          %%ymm2, %%ymm0, %%ymm0")                // ymm0 = (2*y*L*log2(E) + R)*x
            // expf(x * logf(v))
            POW2_CORE_X8
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
            __ASM_EMIT("sub             $8, %[count]")
            __ASM_EMIT("add             $0x20, %[x]")
            __ASM_EMIT("add             $0x20, %[src]")
            __ASM_EMIT("add             $0x20, %[dst]")

            // x4 block
            __ASM_EMIT("6:")
            __ASM_EMIT("add             $4, %[count]")
            __ASM_EMIT("jl              8f")
            __ASM_EMIT("vmovups         0x00(%[src]), %%xmm0")
            // logf(v)
            LOGN_CORE_X4
            __ASM_EMIT("vmulps          0x00 + %[LOGC], %%xmm0, %%xmm0")        // xmm0 = 2*y*L*log2(E)
            __ASM_EMIT("vmovups         0x00(%[x]), %%xmm2")                    // xmm2 = x
            __ASM_EMIT("vaddps          %%xmm1, %%xmm0, %%xmm0")                // xmm0 = 2*y*L*log2(E) + R
            __ASM_EMIT("vmulps          %%xmm2, %%xmm0, %%xmm0")                // xmm0 = (2*y*L*log2(E) + R)*x
            // expf(x * logf(v))
            POW2_CORE_X4
            __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("sub             $4, %[count]")
            __ASM_EMIT("add             $0x10, %[x]")
            __ASM_EMIT("add             $0x10, %[src]")
            __ASM_EMIT("add             $0x10, %[dst]")

            // Tail: 1x-3x block
            __ASM_EMIT("8:")
            __ASM_EMIT("add             $4, %[count]")
            __ASM_EMIT("jle             16f")
            __ASM_EMIT("test            $1, %[count]")
            __ASM_EMIT("jz              10f")
            __ASM_EMIT("vmovss          0x00(%[src]), %%xmm0")
            __ASM_EMIT("vmovss          0x00(%[x]), %%xmm7")
            __ASM_EMIT("add             $4, %[src]")
            __ASM_EMIT("add             $4, %[x]")
            __ASM_EMIT("10:")
            __ASM_EMIT("test            $2, %[count]")
            __ASM_EMIT("jz              12f")
            __ASM_EMIT("vmovhps         0x00(%[src]), %%xmm0, %%xmm0")
            __ASM_EMIT("vmovhps         0x00(%[x]), %%xmm7, %%xmm7")
            __ASM_EMIT("12:")

            // logf(v)
            LOGN_CORE_X4
            __ASM_EMIT("vmulps          0x00 + %[LOGC], %%xmm0, %%xmm0")        // xmm0 = 2*y*L*log2(E)
            __ASM_EMIT("vaddps          %%xmm1, %%xmm0, %%xmm0")                // xmm0 = 2*y*L*log2(E) + R
            __ASM_EMIT("vmulps          %%xmm7, %%xmm0, %%xmm0")                // xmm0 = (2*y*L*log2(E) + R)*x
            // expf(x * logf(v))
            POW2_CORE_X4

            __ASM_EMIT("test            $1, %[count]")
            __ASM_EMIT("jz              14f")
            __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("add             $4, %[dst]")
            __ASM_EMIT("14:")
            __ASM_EMIT("test            $2, %[count]")
            __ASM_EMIT("jz              16f")
            __ASM_EMIT("vmovhps         %%xmm0, 0x00(%[dst])")

            // End
            __ASM_EMIT("16:")
            __ASM_EMIT("vzeroupper")

            : [dst] "+r" (dst), [x] "+r" (x), [src] "+r" (v), [count] "+r" (count)
            : [E2C] "o" (EXP2_CONST),
              [L2C] "o" (LOG2_CONST),
              [LOGC] "o" (LOGB_C)
            : "cc", "memory",
              "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5", "%xmm6", "%xmm7",
              "%xmm8", "%xmm9", "%xmm10", "%xmm11", "%xmm12", "%xmm13", "%xmm14", "%xmm15"
        );
    }








    void x64_powcv1_fma3(float *v, float c, size_t count)
    {
//        float C = logf(c);
//        for (size_t i=0; i<count; ++i)
//            v[i] = expf(v[i] * C);

        IF_ARCH_X86(
            float *src;
            float C[8] __lsp_aligned32;
        );

        ARCH_X86_ASM(
            // Calc C = logf(c)
            __ASM_EMIT("vbroadcastss    %%xmm0, %%ymm0")
            LOGN_FMA3_CORE_X8
            __ASM_EMIT("vmulps          0x00 + %[LOGC], %%ymm0, %%ymm0")    // ymm0 = 2*y*L*log2(E)
            __ASM_EMIT("vaddps          %%ymm1, %%ymm0, %%ymm0")            // ymm0 = 2*y*L*log2(E) + R = log(c)*log2(E)
            __ASM_EMIT("vmovaps         %%ymm0, %[C]")

            // x32 blocks
            __ASM_EMIT("sub             $32, %[count]")
            __ASM_EMIT("jb              2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vmovups         0x00(%[dst]), %%ymm0")
            __ASM_EMIT("vmovups         0x20(%[dst]), %%ymm4")
            __ASM_EMIT("vmovups         0x40(%[dst]), %%ymm8")
            __ASM_EMIT("vmovups         0x60(%[dst]), %%ymm12")
            __ASM_EMIT("vmulps          %[C], %%ymm0, %%ymm0")
            __ASM_EMIT("vmulps          %[C], %%ymm4, %%ymm4")
            __ASM_EMIT("vmulps          %[C], %%ymm8, %%ymm8")
            __ASM_EMIT("vmulps          %[C], %%ymm12, %%ymm12")
            POW2_FMA3_CORE_X32
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
            __ASM_EMIT("vmovups         %%ymm4, 0x20(%[dst])")
            __ASM_EMIT("vmovups         %%ymm8, 0x40(%[dst])")
            __ASM_EMIT("vmovups         %%ymm12, 0x60(%[dst])")
            __ASM_EMIT("add             $0x80, %[dst]")
            __ASM_EMIT("sub             $32, %[count]")
            __ASM_EMIT("jae             1b")

            // x16 block
            __ASM_EMIT("2:")
            __ASM_EMIT("add             $16, %[count]")
            __ASM_EMIT("jl              4f")
            __ASM_EMIT("vmovups         0x00(%[dst]), %%ymm0")
            __ASM_EMIT("vmovups         0x20(%[dst]), %%ymm4")
            __ASM_EMIT("vmulps          %[C], %%ymm0, %%ymm0")
            __ASM_EMIT("vmulps          %[C], %%ymm4, %%ymm4")
            POW2_FMA3_CORE_X16
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
            __ASM_EMIT("vmovups         %%ymm4, 0x20(%[dst])")
            __ASM_EMIT("sub             $16, %[count]")
            __ASM_EMIT("add             $0x40, %[dst]")

            // x8 block
            __ASM_EMIT("4:")
            __ASM_EMIT("add             $8, %[count]")
            __ASM_EMIT("jl              6f")
            __ASM_EMIT("vmovups         0x00(%[dst]), %%ymm0")
            __ASM_EMIT("vmulps          %[C], %%ymm0, %%ymm0")
            POW2_FMA3_CORE_X8
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
            __ASM_EMIT("sub             $8, %[count]")
            __ASM_EMIT("add             $0x20, %[dst]")

            // x4 block
            __ASM_EMIT("6:")
            __ASM_EMIT("add             $4, %[count]")
            __ASM_EMIT("jl              8f")
            __ASM_EMIT("vmovups         0x00(%[dst]), %%xmm0")
            __ASM_EMIT("vmulps          %[C], %%xmm0, %%xmm0")
            POW2_FMA3_CORE_X4
            __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("sub             $4, %[count]")
            __ASM_EMIT("add             $0x10, %[dst]")

            // Tail: 1x-3x block
            __ASM_EMIT("8:")
            __ASM_EMIT("add             $4, %[count]")
            __ASM_EMIT("jle             16f")
            __ASM_EMIT("mov             %[dst], %[src]")
            __ASM_EMIT("test            $1, %[count]")
            __ASM_EMIT("jz              10f")
            __ASM_EMIT("vmovss          0x00(%[src]), %%xmm0")
            __ASM_EMIT("add             $4, %[src]")
            __ASM_EMIT("10:")
            __ASM_EMIT("test            $2, %[count]")
            __ASM_EMIT("jz              12f")
            __ASM_EMIT("vmovhps         0x00(%[src]), %%xmm0, %%xmm0")
            __ASM_EMIT("12:")

            __ASM_EMIT("vmulps          %[C], %%xmm0, %%xmm0")
            POW2_FMA3_CORE_X4

            __ASM_EMIT("test            $1, %[count]")
            __ASM_EMIT("jz              14f")
            __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("add             $4, %[dst]")
            __ASM_EMIT("14:")
            __ASM_EMIT("test            $2, %[count]")
            __ASM_EMIT("jz              16f")
            __ASM_EMIT("vmovhps         %%xmm0, 0x00(%[dst])")

            // End
            __ASM_EMIT("16:")
            __ASM_EMIT("vzeroupper")

            : [dst] "+r" (v), [src] "=&r" (src), [count] "+r" (count),
              "+Yz" (c)
            : [E2C] "o" (EXP2_CONST),
              [L2C] "o" (LOG2_CONST),
              [LOGC] "o" (LOGB_C),
              [C] "m" (C)
            : "cc", "memory",
              "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5", "%xmm6", "%xmm7",
              "%xmm8", "%xmm9", "%xmm10", "%xmm11", "%xmm12", "%xmm13", "%xmm14", "%xmm15"
        );
    }

    void x64_powcv2_fma3(float *dst, const float *v, float c, size_t count)
    {
//        float C = logf(c);
//        for (size_t i=0; i<count; ++i)
//            dst[i] = expf(v[i] * C);

        IF_ARCH_X86(
            float C[8] __lsp_aligned32;
        );

        ARCH_X86_ASM(
            // Calc C = logf(c)
            __ASM_EMIT("vbroadcastss    %%xmm0, %%ymm0")
            LOGN_FMA3_CORE_X8
            __ASM_EMIT("vmulps          0x00 + %[LOGC], %%ymm0, %%ymm0")    // ymm0 = 2*y*L*log2(E)
            __ASM_EMIT("vaddps          %%ymm1, %%ymm0, %%ymm0")            // ymm0 = 2*y*L*log2(E) + R = log(c)*log2(E)
            __ASM_EMIT("vmovaps         %%ymm0, %[C]")

            // x32 blocks
            __ASM_EMIT("sub             $32, %[count]")
            __ASM_EMIT("jb              2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
            __ASM_EMIT("vmovups         0x20(%[src]), %%ymm4")
            __ASM_EMIT("vmovups         0x40(%[src]), %%ymm8")
            __ASM_EMIT("vmovups         0x60(%[src]), %%ymm12")
            __ASM_EMIT("vmulps          %[C], %%ymm0, %%ymm0")
            __ASM_EMIT("vmulps          %[C], %%ymm4, %%ymm4")
            __ASM_EMIT("vmulps          %[C], %%ymm8, %%ymm8")
            __ASM_EMIT("vmulps          %[C], %%ymm12, %%ymm12")
            POW2_FMA3_CORE_X32
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
            __ASM_EMIT("vmovups         %%ymm4, 0x20(%[dst])")
            __ASM_EMIT("vmovups         %%ymm8, 0x40(%[dst])")
            __ASM_EMIT("vmovups         %%ymm12, 0x60(%[dst])")
            __ASM_EMIT("add             $0x80, %[src]")
            __ASM_EMIT("add             $0x80, %[dst]")
            __ASM_EMIT("sub             $32, %[count]")
            __ASM_EMIT("jae             1b")

            // x16 block
            __ASM_EMIT("2:")
            __ASM_EMIT("add             $16, %[count]")
            __ASM_EMIT("jl              4f")
            __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
            __ASM_EMIT("vmovups         0x20(%[src]), %%ymm4")
            __ASM_EMIT("vmulps          %[C], %%ymm0, %%ymm0")
            __ASM_EMIT("vmulps          %[C], %%ymm4, %%ymm4")
            POW2_FMA3_CORE_X16
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
            __ASM_EMIT("vmovups         %%ymm4, 0x20(%[dst])")
            __ASM_EMIT("sub             $16, %[count]")
            __ASM_EMIT("add             $0x40, %[src]")
            __ASM_EMIT("add             $0x40, %[dst]")

            // x8 block
            __ASM_EMIT("4:")
            __ASM_EMIT("add             $8, %[count]")
            __ASM_EMIT("jl              6f")
            __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
            __ASM_EMIT("vmulps          %[C], %%ymm0, %%ymm0")
            POW2_FMA3_CORE_X8
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
            __ASM_EMIT("sub             $8, %[count]")
            __ASM_EMIT("add             $0x20, %[src]")
            __ASM_EMIT("add             $0x20, %[dst]")

            // x4 block
            __ASM_EMIT("6:")
            __ASM_EMIT("add             $4, %[count]")
            __ASM_EMIT("jl              8f")
            __ASM_EMIT("vmovups         0x00(%[src]), %%xmm0")
            __ASM_EMIT("vmulps          %[C], %%xmm0, %%xmm0")
            POW2_FMA3_CORE_X4
            __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("sub             $4, %[count]")
            __ASM_EMIT("add             $0x10, %[src]")
            __ASM_EMIT("add             $0x10, %[dst]")

            // Tail: 1x-3x block
            __ASM_EMIT("8:")
            __ASM_EMIT("add             $4, %[count]")
            __ASM_EMIT("jle             16f")
            __ASM_EMIT("test            $1, %[count]")
            __ASM_EMIT("jz              10f")
            __ASM_EMIT("vmovss          0x00(%[src]), %%xmm0")
            __ASM_EMIT("add             $4, %[src]")
            __ASM_EMIT("10:")
            __ASM_EMIT("test            $2, %[count]")
            __ASM_EMIT("jz              12f")
            __ASM_EMIT("vmovhps         0x00(%[src]), %%xmm0, %%xmm0")
            __ASM_EMIT("12:")

            __ASM_EMIT("vmulps          %[C], %%xmm0, %%xmm0")
            POW2_FMA3_CORE_X4

            __ASM_EMIT("test            $1, %[count]")
            __ASM_EMIT("jz              14f")
            __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("add             $4, %[dst]")
            __ASM_EMIT("14:")
            __ASM_EMIT("test            $2, %[count]")
            __ASM_EMIT("jz              16f")
            __ASM_EMIT("vmovhps         %%xmm0, 0x00(%[dst])")

            // End
            __ASM_EMIT("16:")
            __ASM_EMIT("vzeroupper")

            : [dst] "+r" (dst), [src] "+r" (v), [count] "+r" (count),
              "+Yz" (c)
            : [E2C] "o" (EXP2_CONST),
              [L2C] "o" (LOG2_CONST),
              [LOGC] "o" (LOGB_C),
              [C] "m" (C)
            : "cc", "memory",
              "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5", "%xmm6", "%xmm7",
              "%xmm8", "%xmm9", "%xmm10", "%xmm11", "%xmm12", "%xmm13", "%xmm14", "%xmm15"
        );
    }

    void x64_powvc1_fma3(float *c, float v, size_t count)
    {
//        for (size_t i=0; i<count; ++i)
//            c[i] = expf(v * logf(c[i]));

        IF_ARCH_X86(
            float *src;
            float V[8] __lsp_aligned32;
        );

        ARCH_X86_ASM(
            __ASM_EMIT("vbroadcastss    %%xmm0, %%ymm0")
            __ASM_EMIT("vmovaps         %%ymm0, %[V]")

            // x32 blocks
            __ASM_EMIT("sub             $32, %[count]")
            __ASM_EMIT("jb              2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vmovups         0x00(%[dst]), %%ymm0")
            __ASM_EMIT("vmovups         0x20(%[dst]), %%ymm4")
            __ASM_EMIT("vmovups         0x40(%[dst]), %%ymm8")
            __ASM_EMIT("vmovups         0x60(%[dst]), %%ymm12")
            // logf(c)
            LOGN_FMA3_CORE_X32
            __ASM_EMIT("vfmadd132ps     0x00 + %[LOGC], %%ymm1, %%ymm0")        // ymm0 = 2*y*L*log2(E)+R
            __ASM_EMIT("vfmadd132ps     0x00 + %[LOGC], %%ymm5, %%ymm4")
            __ASM_EMIT("vfmadd132ps     0x00 + %[LOGC], %%ymm9, %%ymm8")
            __ASM_EMIT("vfmadd132ps     0x00 + %[LOGC], %%ymm13, %%ymm12")
            __ASM_EMIT("vmulps          %[V], %%ymm0, %%ymm0")                  // ymm0 = (2*y*L*log2(E)+R)*v
            __ASM_EMIT("vmulps          %[V], %%ymm4, %%ymm4")
            __ASM_EMIT("vmulps          %[V], %%ymm8, %%ymm8")
            __ASM_EMIT("vmulps          %[V], %%ymm12, %%ymm12")
            // expf(v * logf(c))
            POW2_FMA3_CORE_X32
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
            __ASM_EMIT("vmovups         %%ymm4, 0x20(%[dst])")
            __ASM_EMIT("vmovups         %%ymm8, 0x40(%[dst])")
            __ASM_EMIT("vmovups         %%ymm12, 0x60(%[dst])")
            __ASM_EMIT("add             $0x80, %[dst]")
            __ASM_EMIT("sub             $32, %[count]")
            __ASM_EMIT("jae             1b")

            // x16 block
            __ASM_EMIT("2:")
            __ASM_EMIT("add             $16, %[count]")
            __ASM_EMIT("jl              4f")
            __ASM_EMIT("vmovups         0x00(%[dst]), %%ymm0")
            __ASM_EMIT("vmovups         0x20(%[dst]), %%ymm4")
            // logf(c)
            LOGN_FMA3_CORE_X16
            __ASM_EMIT("vfmadd132ps     0x00 + %[LOGC], %%ymm1, %%ymm0")        // ymm0 = 2*y*L*log2(E)+R
            __ASM_EMIT("vfmadd132ps     0x00 + %[LOGC], %%ymm5, %%ymm4")
            __ASM_EMIT("vmulps          %[V], %%ymm0, %%ymm0")                  // ymm0 = (2*y*L*log2(E)+R)*v
            __ASM_EMIT("vmulps          %[V], %%ymm4, %%ymm4")
            // expf(v * logf(c))
            POW2_FMA3_CORE_X16
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
            __ASM_EMIT("vmovups         %%ymm4, 0x20(%[dst])")
            __ASM_EMIT("sub             $16, %[count]")
            __ASM_EMIT("add             $0x40, %[dst]")

            // x8 block
            __ASM_EMIT("4:")
            __ASM_EMIT("add             $8, %[count]")
            __ASM_EMIT("jl              6f")
            __ASM_EMIT("vmovups         0x00(%[dst]), %%ymm0")
            // logf(c)
            LOGN_FMA3_CORE_X8
            __ASM_EMIT("vfmadd132ps     0x00 + %[LOGC], %%ymm1, %%ymm0")        // ymm0 = 2*y*L*log2(E)+R
            __ASM_EMIT("vmulps          %[V], %%ymm0, %%ymm0")                  // ymm0 = (2*y*L*log2(E)+R)*v
            // expf(v * logf(c))
            POW2_FMA3_CORE_X8
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
            __ASM_EMIT("sub             $8, %[count]")
            __ASM_EMIT("add             $0x20, %[dst]")

            // x4 block
            __ASM_EMIT("6:")
            __ASM_EMIT("add             $4, %[count]")
            __ASM_EMIT("jl              8f")
            __ASM_EMIT("vmovups         0x00(%[dst]), %%xmm0")
            // logf(c)
            LOGN_FMA3_CORE_X4
            __ASM_EMIT("vfmadd132ps     0x00 + %[LOGC], %%xmm1, %%xmm0")        // xmm0 = 2*y*L*log2(E)+R
            __ASM_EMIT("vmulps          %[V], %%xmm0, %%xmm0")                  // xmm0 = (2*y*L*log2(E)+R)*v
            // expf(v * logf(c))
            POW2_FMA3_CORE_X4
            __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("sub             $4, %[count]")
            __ASM_EMIT("add             $0x10, %[dst]")

            // Tail: 1x-3x block
            __ASM_EMIT("8:")
            __ASM_EMIT("add             $4, %[count]")
            __ASM_EMIT("jle             16f")
            __ASM_EMIT("mov             %[dst], %[src]")
            __ASM_EMIT("test            $1, %[count]")
            __ASM_EMIT("jz              10f")
            __ASM_EMIT("vmovss          0x00(%[src]), %%xmm0")
            __ASM_EMIT("add             $4, %[src]")
            __ASM_EMIT("10:")
            __ASM_EMIT("test            $2, %[count]")
            __ASM_EMIT("jz              12f")
            __ASM_EMIT("vmovhps         0x00(%[src]), %%xmm0, %%xmm0")
            __ASM_EMIT("12:")

            // logf(c)
            LOGN_FMA3_CORE_X4
            __ASM_EMIT("vfmadd132ps     0x00 + %[LOGC], %%xmm1, %%xmm0")        // xmm0 = 2*y*L*log2(E)+R
            __ASM_EMIT("vmulps          %[V], %%xmm0, %%xmm0")                  // xmm0 = (2*y*L*log2(E)+R)*v
            // expf(v * logf(c))
            POW2_FMA3_CORE_X4

            __ASM_EMIT("test            $1, %[count]")
            __ASM_EMIT("jz              14f")
            __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("add             $4, %[dst]")
            __ASM_EMIT("14:")
            __ASM_EMIT("test            $2, %[count]")
            __ASM_EMIT("jz              16f")
            __ASM_EMIT("vmovhps         %%xmm0, 0x00(%[dst])")

            // End
            __ASM_EMIT("16:")
            __ASM_EMIT("vzeroupper")

            : [dst] "+r" (c), [src] "=&r" (src), [count] "+r" (count),
              "+Yz" (v)
            : [E2C] "o" (EXP2_CONST),
              [L2C] "o" (LOG2_CONST),
              [LOGC] "o" (LOGB_C),
              [V] "m" (V)
            : "cc", "memory",
              "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5", "%xmm6", "%xmm7",
              "%xmm8", "%xmm9", "%xmm10", "%xmm11", "%xmm12", "%xmm13", "%xmm14", "%xmm15"
        );
    }

    void x64_powvc2_fma3(float *dst, const float *c, float v, size_t count)
    {
//        for (size_t i=0; i<count; ++i)
//            dst[i] = expf(v * logf(c[i]));

        IF_ARCH_X86(
            float V[8] __lsp_aligned32;
        );

        ARCH_X86_ASM(
            __ASM_EMIT("vbroadcastss    %%xmm0, %%ymm0")
            __ASM_EMIT("vmovaps         %%ymm0, %[V]")

            // x32 blocks
            __ASM_EMIT("sub             $32, %[count]")
            __ASM_EMIT("jb              2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
            __ASM_EMIT("vmovups         0x20(%[src]), %%ymm4")
            __ASM_EMIT("vmovups         0x40(%[src]), %%ymm8")
            __ASM_EMIT("vmovups         0x60(%[src]), %%ymm12")
            // logf(c)
            LOGN_FMA3_CORE_X32
            __ASM_EMIT("vfmadd132ps     0x00 + %[LOGC], %%ymm1, %%ymm0")        // ymm0 = 2*y*L*log2(E)+R
            __ASM_EMIT("vfmadd132ps     0x00 + %[LOGC], %%ymm5, %%ymm4")
            __ASM_EMIT("vfmadd132ps     0x00 + %[LOGC], %%ymm9, %%ymm8")
            __ASM_EMIT("vfmadd132ps     0x00 + %[LOGC], %%ymm13, %%ymm12")
            __ASM_EMIT("vmulps          %[V], %%ymm0, %%ymm0")                  // ymm0 = (2*y*L*log2(E)+R)*v
            __ASM_EMIT("vmulps          %[V], %%ymm4, %%ymm4")
            __ASM_EMIT("vmulps          %[V], %%ymm8, %%ymm8")
            __ASM_EMIT("vmulps          %[V], %%ymm12, %%ymm12")
            // expf(v * logf(c))
            POW2_FMA3_CORE_X32
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
            __ASM_EMIT("vmovups         %%ymm4, 0x20(%[dst])")
            __ASM_EMIT("vmovups         %%ymm8, 0x40(%[dst])")
            __ASM_EMIT("vmovups         %%ymm12, 0x60(%[dst])")
            __ASM_EMIT("add             $0x80, %[src]")
            __ASM_EMIT("add             $0x80, %[dst]")
            __ASM_EMIT("sub             $32, %[count]")
            __ASM_EMIT("jae             1b")

            // x16 block
            __ASM_EMIT("2:")
            __ASM_EMIT("add             $16, %[count]")
            __ASM_EMIT("jl              4f")
            __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
            __ASM_EMIT("vmovups         0x20(%[src]), %%ymm4")
            // logf(c)
            LOGN_FMA3_CORE_X16
            __ASM_EMIT("vfmadd132ps     0x00 + %[LOGC], %%ymm1, %%ymm0")        // ymm0 = 2*y*L*log2(E)+R
            __ASM_EMIT("vfmadd132ps     0x00 + %[LOGC], %%ymm5, %%ymm4")
            __ASM_EMIT("vmulps          %[V], %%ymm0, %%ymm0")                  // ymm0 = (2*y*L*log2(E)+R)*v
            __ASM_EMIT("vmulps          %[V], %%ymm4, %%ymm4")
            // expf(v * logf(c))
            POW2_FMA3_CORE_X16
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
            __ASM_EMIT("vmovups         %%ymm4, 0x20(%[dst])")
            __ASM_EMIT("sub             $16, %[count]")
            __ASM_EMIT("add             $0x40, %[src]")
            __ASM_EMIT("add             $0x40, %[dst]")

            // x8 block
            __ASM_EMIT("4:")
            __ASM_EMIT("add             $8, %[count]")
            __ASM_EMIT("jl              6f")
            __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
            // logf(c)
            LOGN_FMA3_CORE_X8
            __ASM_EMIT("vfmadd132ps     0x00 + %[LOGC], %%ymm1, %%ymm0")        // ymm0 = 2*y*L*log2(E)+R
            __ASM_EMIT("vmulps          %[V], %%ymm0, %%ymm0")                  // ymm0 = (2*y*L*log2(E)+R)*v
            // expf(v * logf(c))
            POW2_FMA3_CORE_X8
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
            __ASM_EMIT("sub             $8, %[count]")
            __ASM_EMIT("add             $0x20, %[src]")
            __ASM_EMIT("add             $0x20, %[dst]")

            // x4 block
            __ASM_EMIT("6:")
            __ASM_EMIT("add             $4, %[count]")
            __ASM_EMIT("jl              8f")
            __ASM_EMIT("vmovups         0x00(%[src]), %%xmm0")
            // logf(c)
            LOGN_FMA3_CORE_X4
            __ASM_EMIT("vfmadd132ps     0x00 + %[LOGC], %%xmm1, %%xmm0")        // ymm0 = 2*y*L*log2(E)+R
            __ASM_EMIT("vmulps          %[V], %%xmm0, %%xmm0")                  // xmm0 = (2*y*L*log2(E)+R)*v
            // expf(v * logf(c))
            POW2_FMA3_CORE_X4
            __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("sub             $4, %[count]")
            __ASM_EMIT("add             $0x10, %[src]")
            __ASM_EMIT("add             $0x10, %[dst]")

            // Tail: 1x-3x block
            __ASM_EMIT("8:")
            __ASM_EMIT("add             $4, %[count]")
            __ASM_EMIT("jle             16f")
            __ASM_EMIT("test            $1, %[count]")
            __ASM_EMIT("jz              10f")
            __ASM_EMIT("vmovss          0x00(%[src]), %%xmm0")
            __ASM_EMIT("add             $4, %[src]")
            __ASM_EMIT("10:")
            __ASM_EMIT("test            $2, %[count]")
            __ASM_EMIT("jz              12f")
            __ASM_EMIT("vmovhps         0x00(%[src]), %%xmm0, %%xmm0")
            __ASM_EMIT("12:")

            // logf(c)
            LOGN_FMA3_CORE_X4
            __ASM_EMIT("vfmadd132ps     0x00 + %[LOGC], %%xmm1, %%xmm0")        // xmm0 = 2*y*L*log2(E)+R
            __ASM_EMIT("vmulps          %[V], %%xmm0, %%xmm0")                  // xmm0 = (2*y*L*log2(E)+R)*v
            // expf(v * logf(c))
            POW2_FMA3_CORE_X4

            __ASM_EMIT("test            $1, %[count]")
            __ASM_EMIT("jz              14f")
            __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("add             $4, %[dst]")
            __ASM_EMIT("14:")
            __ASM_EMIT("test            $2, %[count]")
            __ASM_EMIT("jz              16f")
            __ASM_EMIT("vmovhps         %%xmm0, 0x00(%[dst])")

            // End
            __ASM_EMIT("16:")
            __ASM_EMIT("vzeroupper")

            : [dst] "+r" (dst), [src] "+r" (c), [count] "+r" (count),
              "+Yz" (v)
            : [E2C] "o" (EXP2_CONST),
              [L2C] "o" (LOG2_CONST),
              [LOGC] "o" (LOGB_C),
              [V] "m" (V)
            : "cc", "memory",
              "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5", "%xmm6", "%xmm7",
              "%xmm8", "%xmm9", "%xmm10", "%xmm11", "%xmm12", "%xmm13", "%xmm14", "%xmm15"
        );
    }

    void x64_powvx1_fma3(float *v, const float *x, size_t count)
    {
//        for (size_t i=0; i<count; ++i)
//            v[i] = expf(x[i] * logf(v[i]));
        IF_ARCH_X86(float *src);

        ARCH_X86_ASM(
            // x32 blocks
            __ASM_EMIT("sub             $32, %[count]")
            __ASM_EMIT("jb              2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vmovups         0x00(%[dst]), %%ymm0")
            __ASM_EMIT("vmovups         0x20(%[dst]), %%ymm4")
            __ASM_EMIT("vmovups         0x40(%[dst]), %%ymm8")
            __ASM_EMIT("vmovups         0x60(%[dst]), %%ymm12")
            // logf(v)
            LOGN_FMA3_CORE_X32
            __ASM_EMIT("vmovups         0x00(%[x]), %%ymm2")                    // ymm2 = x
            __ASM_EMIT("vmovups         0x20(%[x]), %%ymm6")
            __ASM_EMIT("vmovups         0x40(%[x]), %%ymm10")
            __ASM_EMIT("vmovups         0x60(%[x]), %%ymm14")
            __ASM_EMIT("vfmadd132ps     0x00 + %[LOGC], %%ymm1, %%ymm0")        // ymm0 = 2*y*L*log2(E)+R
            __ASM_EMIT("vfmadd132ps     0x00 + %[LOGC], %%ymm5, %%ymm4")
            __ASM_EMIT("vfmadd132ps     0x00 + %[LOGC], %%ymm9, %%ymm8")
            __ASM_EMIT("vfmadd132ps     0x00 + %[LOGC], %%ymm13, %%ymm12")
            __ASM_EMIT("vmulps          %%ymm2, %%ymm0, %%ymm0")                // ymm0 = (2*y*L*log2(E) + R)*x
            __ASM_EMIT("vmulps          %%ymm6, %%ymm4, %%ymm4")
            __ASM_EMIT("vmulps          %%ymm10, %%ymm8, %%ymm8")
            __ASM_EMIT("vmulps          %%ymm14, %%ymm12, %%ymm12")
            // expf(x * logf(v))
            POW2_FMA3_CORE_X32
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
            __ASM_EMIT("vmovups         %%ymm4, 0x20(%[dst])")
            __ASM_EMIT("vmovups         %%ymm8, 0x40(%[dst])")
            __ASM_EMIT("vmovups         %%ymm12, 0x60(%[dst])")
            __ASM_EMIT("add             $0x80, %[x]")
            __ASM_EMIT("add             $0x80, %[dst]")
            __ASM_EMIT("sub             $32, %[count]")
            __ASM_EMIT("jae             1b")

            // x16 block
            __ASM_EMIT("2:")
            __ASM_EMIT("add             $16, %[count]")
            __ASM_EMIT("jl              4f")
            __ASM_EMIT("vmovups         0x00(%[dst]), %%ymm0")
            __ASM_EMIT("vmovups         0x20(%[dst]), %%ymm4")
            // logf(v)
            LOGN_FMA3_CORE_X16
            __ASM_EMIT("vmovups         0x00(%[x]), %%ymm2")                    // ymm2 = x
            __ASM_EMIT("vmovups         0x20(%[x]), %%ymm6")
            __ASM_EMIT("vfmadd132ps     0x00 + %[LOGC], %%ymm1, %%ymm0")        // ymm0 = 2*y*L*log2(E)+R
            __ASM_EMIT("vfmadd132ps     0x00 + %[LOGC], %%ymm5, %%ymm4")
            __ASM_EMIT("vmulps          %%ymm2, %%ymm0, %%ymm0")                // ymm0 = (2*y*L*log2(E) + R)*x
            __ASM_EMIT("vmulps          %%ymm6, %%ymm4, %%ymm4")
            // expf(x * logf(v))
            POW2_FMA3_CORE_X16
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
            __ASM_EMIT("vmovups         %%ymm4, 0x20(%[dst])")
            __ASM_EMIT("sub             $16, %[count]")
            __ASM_EMIT("add             $0x40, %[x]")
            __ASM_EMIT("add             $0x40, %[dst]")

            // x8 block
            __ASM_EMIT("4:")
            __ASM_EMIT("add             $8, %[count]")
            __ASM_EMIT("jl              6f")
            __ASM_EMIT("vmovups         0x00(%[dst]), %%ymm0")
            // logf(v)
            LOGN_FMA3_CORE_X8
            __ASM_EMIT("vmovups         0x00(%[x]), %%ymm2")                    // ymm2 = x
            __ASM_EMIT("vfmadd132ps     0x00 + %[LOGC], %%ymm1, %%ymm0")        // ymm0 = 2*y*L*log2(E)+R
            __ASM_EMIT("vmulps          %%ymm2, %%ymm0, %%ymm0")                // ymm0 = (2*y*L*log2(E) + R)*x
            // expf(x * logf(v))
            POW2_FMA3_CORE_X8
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
            __ASM_EMIT("sub             $8, %[count]")
            __ASM_EMIT("add             $0x20, %[x]")
            __ASM_EMIT("add             $0x20, %[dst]")

            // x4 block
            __ASM_EMIT("6:")
            __ASM_EMIT("add             $4, %[count]")
            __ASM_EMIT("jl              8f")
            __ASM_EMIT("vmovups         0x00(%[dst]), %%xmm0")
            // logf(v)
            LOGN_FMA3_CORE_X4
            __ASM_EMIT("vmovups         0x00(%[x]), %%xmm2")                    // xmm2 = x
            __ASM_EMIT("vfmadd132ps     0x00 + %[LOGC], %%xmm1, %%xmm0")        // xmm0 = 2*y*L*log2(E)+R
            __ASM_EMIT("vmulps          %%xmm2, %%xmm0, %%xmm0")                // xmm0 = (2*y*L*log2(E) + R)*x
            // expf(x * logf(v))
            POW2_FMA3_CORE_X4
            __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("sub             $4, %[count]")
            __ASM_EMIT("add             $0x10, %[x]")
            __ASM_EMIT("add             $0x10, %[dst]")

            // Tail: 1x-3x block
            __ASM_EMIT("8:")
            __ASM_EMIT("add             $4, %[count]")
            __ASM_EMIT("jle             16f")
            __ASM_EMIT("mov             %[dst], %[src]")
            __ASM_EMIT("test            $1, %[count]")
            __ASM_EMIT("jz              10f")
            __ASM_EMIT("vmovss          0x00(%[src]), %%xmm0")
            __ASM_EMIT("vmovss          0x00(%[x]), %%xmm7")
            __ASM_EMIT("add             $4, %[src]")
            __ASM_EMIT("add             $4, %[x]")
            __ASM_EMIT("10:")
            __ASM_EMIT("test            $2, %[count]")
            __ASM_EMIT("jz              12f")
            __ASM_EMIT("vmovhps         0x00(%[src]), %%xmm0, %%xmm0")
            __ASM_EMIT("vmovhps         0x00(%[x]), %%xmm7, %%xmm7")
            __ASM_EMIT("12:")

            // logf(v)
            LOGN_FMA3_CORE_X4
            __ASM_EMIT("vfmadd132ps     0x00 + %[LOGC], %%xmm1, %%xmm0")        // xmm0 = 2*y*L*log2(E)+R
            __ASM_EMIT("vmulps          %%xmm7, %%xmm0, %%xmm0")                // xmm0 = (2*y*L*log2(E) + R)*x
            // expf(x * logf(v))
            POW2_FMA3_CORE_X4

            __ASM_EMIT("test            $1, %[count]")
            __ASM_EMIT("jz              14f")
            __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("add             $4, %[dst]")
            __ASM_EMIT("14:")
            __ASM_EMIT("test            $2, %[count]")
            __ASM_EMIT("jz              16f")
            __ASM_EMIT("vmovhps         %%xmm0, 0x00(%[dst])")

            // End
            __ASM_EMIT("16:")
            __ASM_EMIT("vzeroupper")

            : [dst] "+r" (v), [x] "+r" (x), [src] "=&r" (src), [count] "+r" (count)
            : [E2C] "o" (EXP2_CONST),
              [L2C] "o" (LOG2_CONST),
              [LOGC] "o" (LOGB_C)
            : "cc", "memory",
              "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5", "%xmm6", "%xmm7",
              "%xmm8", "%xmm9", "%xmm10", "%xmm11", "%xmm12", "%xmm13", "%xmm14", "%xmm15"
        );
    }

    void x64_powvx2_fma3(float *dst, const float *v, const float *x, size_t count)
    {
//        for (size_t i=0; i<count; ++i)
//            dst[i] = expf(x[i] * logf(v[i]));
        ARCH_X86_ASM(
            // x32 blocks
            __ASM_EMIT("sub             $32, %[count]")
            __ASM_EMIT("jb              2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
            __ASM_EMIT("vmovups         0x20(%[src]), %%ymm4")
            __ASM_EMIT("vmovups         0x40(%[src]), %%ymm8")
            __ASM_EMIT("vmovups         0x60(%[src]), %%ymm12")
            // logf(v)
            LOGN_FMA3_CORE_X32
            __ASM_EMIT("vmovups         0x00(%[x]), %%ymm2")                    // ymm2 = x
            __ASM_EMIT("vmovups         0x20(%[x]), %%ymm6")
            __ASM_EMIT("vmovups         0x40(%[x]), %%ymm10")
            __ASM_EMIT("vmovups         0x60(%[x]), %%ymm14")
            __ASM_EMIT("vfmadd132ps     0x00 + %[LOGC], %%ymm1, %%ymm0")        // ymm0 = 2*y*L*log2(E)+R
            __ASM_EMIT("vfmadd132ps     0x00 + %[LOGC], %%ymm5, %%ymm4")
            __ASM_EMIT("vfmadd132ps     0x00 + %[LOGC], %%ymm9, %%ymm8")
            __ASM_EMIT("vfmadd132ps     0x00 + %[LOGC], %%ymm13, %%ymm12")
            __ASM_EMIT("vmulps          %%ymm2, %%ymm0, %%ymm0")                // ymm0 = (2*y*L*log2(E) + R)*x
            __ASM_EMIT("vmulps          %%ymm6, %%ymm4, %%ymm4")
            __ASM_EMIT("vmulps          %%ymm10, %%ymm8, %%ymm8")
            __ASM_EMIT("vmulps          %%ymm14, %%ymm12, %%ymm12")
            // expf(x * logf(v))
            POW2_FMA3_CORE_X32
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
            __ASM_EMIT("vmovups         %%ymm4, 0x20(%[dst])")
            __ASM_EMIT("vmovups         %%ymm8, 0x40(%[dst])")
            __ASM_EMIT("vmovups         %%ymm12, 0x60(%[dst])")
            __ASM_EMIT("add             $0x80, %[x]")
            __ASM_EMIT("add             $0x80, %[src]")
            __ASM_EMIT("add             $0x80, %[dst]")
            __ASM_EMIT("sub             $32, %[count]")
            __ASM_EMIT("jae             1b")

            // x16 block
            __ASM_EMIT("2:")
            __ASM_EMIT("add             $16, %[count]")
            __ASM_EMIT("jl              4f")
            __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
            __ASM_EMIT("vmovups         0x20(%[src]), %%ymm4")
            // logf(v)
            LOGN_FMA3_CORE_X16
            __ASM_EMIT("vmovups         0x00(%[x]), %%ymm2")                    // ymm2 = x
            __ASM_EMIT("vmovups         0x20(%[x]), %%ymm6")
            __ASM_EMIT("vfmadd132ps     0x00 + %[LOGC], %%ymm1, %%ymm0")        // ymm0 = 2*y*L*log2(E)+R
            __ASM_EMIT("vfmadd132ps     0x00 + %[LOGC], %%ymm5, %%ymm4")
            __ASM_EMIT("vmulps          %%ymm2, %%ymm0, %%ymm0")                // ymm0 = (2*y*L*log2(E) + R)*x
            __ASM_EMIT("vmulps          %%ymm6, %%ymm4, %%ymm4")
            // expf(x * logf(v))
            POW2_FMA3_CORE_X16
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
            __ASM_EMIT("vmovups         %%ymm4, 0x20(%[dst])")
            __ASM_EMIT("sub             $16, %[count]")
            __ASM_EMIT("add             $0x40, %[x]")
            __ASM_EMIT("add             $0x40, %[src]")
            __ASM_EMIT("add             $0x40, %[dst]")

            // x8 block
            __ASM_EMIT("4:")
            __ASM_EMIT("add             $8, %[count]")
            __ASM_EMIT("jl              6f")
            __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
            // logf(v)
            LOGN_FMA3_CORE_X8
            __ASM_EMIT("vmovups         0x00(%[x]), %%ymm2")                    // ymm2 = x
            __ASM_EMIT("vfmadd132ps     0x00 + %[LOGC], %%ymm1, %%ymm0")        // ymm0 = 2*y*L*log2(E)+R
            __ASM_EMIT("vmulps          %%ymm2, %%ymm0, %%ymm0")                // ymm0 = (2*y*L*log2(E) + R)*x
            // expf(x * logf(v))
            POW2_FMA3_CORE_X8
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
            __ASM_EMIT("sub             $8, %[count]")
            __ASM_EMIT("add             $0x20, %[x]")
            __ASM_EMIT("add             $0x20, %[src]")
            __ASM_EMIT("add             $0x20, %[dst]")

            // x4 block
            __ASM_EMIT("6:")
            __ASM_EMIT("add             $4, %[count]")
            __ASM_EMIT("jl              8f")
            __ASM_EMIT("vmovups         0x00(%[src]), %%xmm0")
            // logf(v)
            LOGN_FMA3_CORE_X4
            __ASM_EMIT("vmovups         0x00(%[x]), %%xmm2")                    // xmm2 = x
            __ASM_EMIT("vfmadd132ps     0x00 + %[LOGC], %%xmm1, %%xmm0")        // xmm0 = 2*y*L*log2(E)+R
            __ASM_EMIT("vmulps          %%xmm2, %%xmm0, %%xmm0")                // xmm0 = (2*y*L*log2(E) + R)*x
            // expf(x * logf(v))
            POW2_FMA3_CORE_X4
            __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("sub             $4, %[count]")
            __ASM_EMIT("add             $0x10, %[x]")
            __ASM_EMIT("add             $0x10, %[src]")
            __ASM_EMIT("add             $0x10, %[dst]")

            // Tail: 1x-3x block
            __ASM_EMIT("8:")
            __ASM_EMIT("add             $4, %[count]")
            __ASM_EMIT("jle             16f")
            __ASM_EMIT("test            $1, %[count]")
            __ASM_EMIT("jz              10f")
            __ASM_EMIT("vmovss          0x00(%[src]), %%xmm0")
            __ASM_EMIT("vmovss          0x00(%[x]), %%xmm7")
            __ASM_EMIT("add             $4, %[src]")
            __ASM_EMIT("add             $4, %[x]")
            __ASM_EMIT("10:")
            __ASM_EMIT("test            $2, %[count]")
            __ASM_EMIT("jz              12f")
            __ASM_EMIT("vmovhps         0x00(%[src]), %%xmm0, %%xmm0")
            __ASM_EMIT("vmovhps         0x00(%[x]), %%xmm7, %%xmm7")
            __ASM_EMIT("12:")

            // logf(v)
            LOGN_FMA3_CORE_X4
            __ASM_EMIT("vfmadd132ps     0x00 + %[LOGC], %%xmm1, %%xmm0")        // xmm0 = 2*y*L*log2(E)+R
            __ASM_EMIT("vmulps          %%xmm7, %%xmm0, %%xmm0")                // xmm0 = (2*y*L*log2(E) + R)*x
            // expf(x * logf(v))
            POW2_FMA3_CORE_X4

            __ASM_EMIT("test            $1, %[count]")
            __ASM_EMIT("jz              14f")
            __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("add             $4, %[dst]")
            __ASM_EMIT("14:")
            __ASM_EMIT("test            $2, %[count]")
            __ASM_EMIT("jz              16f")
            __ASM_EMIT("vmovhps         %%xmm0, 0x00(%[dst])")

            // End
            __ASM_EMIT("16:")
            __ASM_EMIT("vzeroupper")

            : [dst] "+r" (dst), [x] "+r" (x), [src] "+r" (v), [count] "+r" (count)
            : [E2C] "o" (EXP2_CONST),
              [L2C] "o" (LOG2_CONST),
              [LOGC] "o" (LOGB_C)
            : "cc", "memory",
              "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5", "%xmm6", "%xmm7",
              "%xmm8", "%xmm9", "%xmm10", "%xmm11", "%xmm12", "%xmm13", "%xmm14", "%xmm15"
        );
    }
}

#endif /* ARCH_X86_64 */

#endif /* DSP_ARCH_X86_AVX2_PMATH_POW_H_ */
