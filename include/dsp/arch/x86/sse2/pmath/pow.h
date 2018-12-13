/*
 * pow.h
 *
 *  Created on: 12 дек. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_SSE2_PMATH_POW_H_
#define DSP_ARCH_X86_SSE2_PMATH_POW_H_

#ifndef DSP_ARCH_X86_SSE2_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_SSE2_IMPL */

#include <dsp/arch/x86/sse2/pmath/exp.h>
#include <dsp/arch/x86/sse2/pmath/log.h>

namespace sse2
{
    void powcv1(float *v, float c, size_t count)
    {
//        float C = logf(c);
//        for (size_t i=0; i<count; ++i)
//            v[i] = expf(v[i] * C);

        IF_ARCH_X86(
            float *src;
            float C[4] __lsp_aligned16;
        );

        ARCH_X86_ASM(
            // Calc C = logf(c)
            __ASM_EMIT("shufps          $0x00, %%xmm0, %%xmm0")
            LOGN_CORE_X4
            __ASM_EMIT("mulps           0x00 + %[LOGC], %%xmm0")        // xmm0 = 2*y*L*log2(E)
            __ASM_EMIT("addps           %%xmm1, %%xmm0")                // xmm0 = 2*y*L*log2(E) + R = log(c)*log2(E)
            __ASM_EMIT("movaps          %%xmm0, %[C]")

            // Analyze size
            __ASM_EMIT("sub             $8, %[count]")
            __ASM_EMIT("jb              2f")

            // x8 blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("movups          0x00(%[dst]), %%xmm0")
            __ASM_EMIT("movups          0x10(%[dst]), %%xmm4")
            __ASM_EMIT("mulps           %[C], %%xmm0")
            __ASM_EMIT("mulps           %[C], %%xmm4")
            POW2_CORE_X8
            __ASM_EMIT("movups          %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("movups          %%xmm4, 0x10(%[dst])")
            __ASM_EMIT("add             $0x20, %[dst]")
            __ASM_EMIT("sub             $8, %[count]")
            __ASM_EMIT("jae             1b")

            __ASM_EMIT("2:")
            __ASM_EMIT("add             $4, %[count]")
            __ASM_EMIT("jl              4f")

            // x4 block
            __ASM_EMIT("movups          0x00(%[dst]), %%xmm0")
            __ASM_EMIT("mulps           %[C], %%xmm0")
            POW2_CORE_X4
            __ASM_EMIT("movups          %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("add             $0x10, %[dst]")
            __ASM_EMIT("sub             $4, %[count]")

            __ASM_EMIT("4:")
            __ASM_EMIT("add             $4, %[count]")
            __ASM_EMIT("jle             12f")

            // Tail: 1x-3x block
            __ASM_EMIT("mov             %[dst], %[src]")
            __ASM_EMIT("test            $1, %[count]")
            __ASM_EMIT("jz              6f")
            __ASM_EMIT("movss           0x00(%[src]), %%xmm0")
            __ASM_EMIT("add             $4, %[src]")
            __ASM_EMIT("6:")
            __ASM_EMIT("test            $2, %[count]")
            __ASM_EMIT("jz              8f")
            __ASM_EMIT("movhps          0x00(%[src]), %%xmm0")
            __ASM_EMIT("8:")

            __ASM_EMIT("mulps           %[C], %%xmm0")
            POW2_CORE_X4

            __ASM_EMIT("test            $1, %[count]")
            __ASM_EMIT("jz              10f")
            __ASM_EMIT("movss           %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("add             $4, %[dst]")
            __ASM_EMIT("10:")
            __ASM_EMIT("test            $2, %[count]")
            __ASM_EMIT("jz              12f")
            __ASM_EMIT("movhps          %%xmm0, 0x00(%[dst])")

            // End
            __ASM_EMIT("12:")

            : [dst] "+r" (v), [src] "=&r" (src), [count] "+r" (count),
              "+Yz" (c)
            : [E2C] "o" (EXP2_CONST),
              [L2C] "o" (LOG2_CONST),
              [LOGC] "o" (LOGB_C),
              [C] "m" (C)
            : "cc", "memory",
              "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void powcv2(float *dst, const float *v, float c, size_t count)
    {
//        float C = logf(c);
//        for (size_t i=0; i<count; ++i)
//            dst[i] = expf(v[i] * C);

        IF_ARCH_X86(
            float C[4] __lsp_aligned16;
        );

        ARCH_X86_ASM(
            // Calc C = logf(c)
            __ASM_EMIT("shufps          $0x00, %%xmm0, %%xmm0")
            LOGN_CORE_X4
            __ASM_EMIT("mulps           0x00 + %[LOGC], %%xmm0")        // xmm0 = 2*y*L*log2(E)
            __ASM_EMIT("addps           %%xmm1, %%xmm0")                // xmm0 = 2*y*L*log2(E) + R = log(c)*log2(E)
            __ASM_EMIT("movaps          %%xmm0, %[C]")

            // Analyze size
            __ASM_EMIT("sub             $8, %[count]")
            __ASM_EMIT("jb              2f")

            // x8 blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("movups          0x00(%[src]), %%xmm0")
            __ASM_EMIT("movups          0x10(%[src]), %%xmm4")
            __ASM_EMIT("mulps           %[C], %%xmm0")
            __ASM_EMIT("mulps           %[C], %%xmm4")
            POW2_CORE_X8
            __ASM_EMIT("movups          %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("movups          %%xmm4, 0x10(%[dst])")
            __ASM_EMIT("add             $0x20, %[src]")
            __ASM_EMIT("add             $0x20, %[dst]")
            __ASM_EMIT("sub             $8, %[count]")
            __ASM_EMIT("jae             1b")

            __ASM_EMIT("2:")
            __ASM_EMIT("add             $4, %[count]")
            __ASM_EMIT("jl              4f")

            // x4 block
            __ASM_EMIT("movups          0x00(%[src]), %%xmm0")
            __ASM_EMIT("mulps           %[C], %%xmm0")
            POW2_CORE_X4
            __ASM_EMIT("movups          %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("add             $0x10, %[src]")
            __ASM_EMIT("add             $0x10, %[dst]")
            __ASM_EMIT("sub             $4, %[count]")

            __ASM_EMIT("4:")
            __ASM_EMIT("add             $4, %[count]")
            __ASM_EMIT("jle             12f")

            // Tail: 1x-3x block
            __ASM_EMIT("test            $1, %[count]")
            __ASM_EMIT("jz              6f")
            __ASM_EMIT("movss           0x00(%[src]), %%xmm0")
            __ASM_EMIT("add             $4, %[src]")
            __ASM_EMIT("6:")
            __ASM_EMIT("test            $2, %[count]")
            __ASM_EMIT("jz              8f")
            __ASM_EMIT("movhps          0x00(%[src]), %%xmm0")
            __ASM_EMIT("8:")

            __ASM_EMIT("mulps           %[C], %%xmm0")
            POW2_CORE_X4

            __ASM_EMIT("test            $1, %[count]")
            __ASM_EMIT("jz              10f")
            __ASM_EMIT("movss           %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("add             $4, %[dst]")
            __ASM_EMIT("10:")
            __ASM_EMIT("test            $2, %[count]")
            __ASM_EMIT("jz              12f")
            __ASM_EMIT("movhps          %%xmm0, 0x00(%[dst])")

            // End
            __ASM_EMIT("12:")

            : [dst] "+r" (dst), [src] "+r" (v), [count] "+r" (count),
              "+Yz" (c)
            : [E2C] "o" (EXP2_CONST),
              [L2C] "o" (LOG2_CONST),
              [LOGC] "o" (LOGB_C),
              [C] "m" (C)
            : "cc", "memory",
              "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void powvc1(float *c, float v, size_t count)
    {
//        for (size_t i=0; i<count; ++i)
//            c[i] = expf(v * logf(c[i]));

        IF_ARCH_X86(
            float *src;
            float V[4] __lsp_aligned16;
        );

        ARCH_X86_ASM(
            __ASM_EMIT("shufps          $0x00, %%xmm0, %%xmm0")
            __ASM_EMIT("movaps          %%xmm0, %[V]")

            // Analyze size
            __ASM_EMIT("sub             $8, %[count]")
            __ASM_EMIT("jb              2f")

            // x8 blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("movups          0x00(%[dst]), %%xmm0")
            __ASM_EMIT("movups          0x10(%[dst]), %%xmm4")
            // logf(c)
            LOGN_CORE_X8
            __ASM_EMIT("mulps           0x00 + %[LOGC], %%xmm0")        // xmm0 = 2*y*L*log2(E)
            __ASM_EMIT("mulps           0x00 + %[LOGC], %%xmm4")
            __ASM_EMIT("addps           %%xmm1, %%xmm0")                // xmm0 = 2*y*L*log2(E)+R
            __ASM_EMIT("addps           %%xmm5, %%xmm4")
            __ASM_EMIT("mulps           %[V], %%xmm0")                  // xmm0 = (2*y*L*log2(E)+R)*v
            __ASM_EMIT("mulps           %[V], %%xmm4")
            // expf(v * logf(c))
            POW2_CORE_X8
            __ASM_EMIT("movups          %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("movups          %%xmm4, 0x10(%[dst])")
            __ASM_EMIT("add             $0x20, %[dst]")
            __ASM_EMIT("sub             $8, %[count]")
            __ASM_EMIT("jae             1b")

            __ASM_EMIT("2:")
            __ASM_EMIT("add             $4, %[count]")
            __ASM_EMIT("jl              4f")

            // x4 block
            __ASM_EMIT("movups          0x00(%[dst]), %%xmm0")
            // logf(c)
            LOGN_CORE_X4
            __ASM_EMIT("mulps           0x00 + %[LOGC], %%xmm0")        // xmm0 = 2*y*L*log2(E)
            __ASM_EMIT("addps           %%xmm1, %%xmm0")                // xmm0 = 2*y*L*log2(E)+R
            __ASM_EMIT("mulps           %[V], %%xmm0")                  // xmm0 = (2*y*L*log2(E)+R)*v
            // expf(v * logf(c))
            POW2_CORE_X4
            __ASM_EMIT("movups          %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("add             $0x10, %[dst]")
            __ASM_EMIT("sub             $4, %[count]")

            __ASM_EMIT("4:")
            __ASM_EMIT("add             $4, %[count]")
            __ASM_EMIT("jle             12f")

            // Tail: 1x-3x block
            __ASM_EMIT("mov             %[dst], %[src]")
            __ASM_EMIT("test            $1, %[count]")
            __ASM_EMIT("jz              6f")
            __ASM_EMIT("movss           0x00(%[src]), %%xmm0")
            __ASM_EMIT("add             $4, %[src]")
            __ASM_EMIT("6:")
            __ASM_EMIT("test            $2, %[count]")
            __ASM_EMIT("jz              8f")
            __ASM_EMIT("movhps          0x00(%[src]), %%xmm0")
            __ASM_EMIT("8:")

            // logf(c)
            LOGN_CORE_X4
            __ASM_EMIT("mulps           0x00 + %[LOGC], %%xmm0")        // xmm0 = 2*y*L*log2(E)
            __ASM_EMIT("addps           %%xmm1, %%xmm0")                // xmm0 = 2*y*L*log2(E)+R
            __ASM_EMIT("mulps           %[V], %%xmm0")                  // xmm0 = (2*y*L*log2(E)+R)*v
            // expf(v * logf(c))
            POW2_CORE_X4

            __ASM_EMIT("test            $1, %[count]")
            __ASM_EMIT("jz              10f")
            __ASM_EMIT("movss           %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("add             $4, %[dst]")
            __ASM_EMIT("10:")
            __ASM_EMIT("test            $2, %[count]")
            __ASM_EMIT("jz              12f")
            __ASM_EMIT("movhps          %%xmm0, 0x00(%[dst])")

            // End
            __ASM_EMIT("12:")

            : [dst] "+r" (c), [src] "=&r" (src), [count] "+r" (count),
              "+Yz" (v)
            : [E2C] "o" (EXP2_CONST),
              [L2C] "o" (LOG2_CONST),
              [LOGC] "o" (LOGB_C),
              [V] "m" (V)
            : "cc", "memory",
              "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void powvc2(float *dst, const float *c, float v, size_t count)
    {
//        for (size_t i=0; i<count; ++i)
//            dst[i] = expf(v * logf(c[i]));

        IF_ARCH_X86(
            float V[4] __lsp_aligned16;
        );

        ARCH_X86_ASM(
            __ASM_EMIT("shufps          $0x00, %%xmm0, %%xmm0")
            __ASM_EMIT("movaps          %%xmm0, %[V]")

            // Analyze size
            __ASM_EMIT("sub             $8, %[count]")
            __ASM_EMIT("jb              2f")

            // x8 blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("movups          0x00(%[src]), %%xmm0")
            __ASM_EMIT("movups          0x10(%[src]), %%xmm4")
            // logf(c)
            LOGN_CORE_X8
            __ASM_EMIT("mulps           0x00 + %[LOGC], %%xmm0")        // xmm0 = 2*y*L*log2(E)
            __ASM_EMIT("mulps           0x00 + %[LOGC], %%xmm4")
            __ASM_EMIT("addps           %%xmm1, %%xmm0")                // xmm0 = 2*y*L*log2(E)+R
            __ASM_EMIT("addps           %%xmm5, %%xmm4")
            __ASM_EMIT("mulps           %[V], %%xmm0")                  // xmm0 = (2*y*L*log2(E)+R)*v
            __ASM_EMIT("mulps           %[V], %%xmm4")
            // expf(v * logf(c))
            POW2_CORE_X8
            __ASM_EMIT("movups          %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("movups          %%xmm4, 0x10(%[dst])")
            __ASM_EMIT("add             $0x20, %[src]")
            __ASM_EMIT("add             $0x20, %[dst]")
            __ASM_EMIT("sub             $8, %[count]")
            __ASM_EMIT("jae             1b")

            __ASM_EMIT("2:")
            __ASM_EMIT("add             $4, %[count]")
            __ASM_EMIT("jl              4f")

            // x4 block
            __ASM_EMIT("movups          0x00(%[src]), %%xmm0")
            // logf(c)
            LOGN_CORE_X4
            __ASM_EMIT("mulps           0x00 + %[LOGC], %%xmm0")        // xmm0 = 2*y*L*log2(E)
            __ASM_EMIT("addps           %%xmm1, %%xmm0")                // xmm0 = 2*y*L*log2(E)+R
            __ASM_EMIT("mulps           %[V], %%xmm0")                  // xmm0 = (2*y*L*log2(E)+R)*v
            // expf(v * logf(c))
            POW2_CORE_X4
            __ASM_EMIT("movups          %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("add             $0x10, %[src]")
            __ASM_EMIT("add             $0x10, %[dst]")
            __ASM_EMIT("sub             $4, %[count]")

            __ASM_EMIT("4:")
            __ASM_EMIT("add             $4, %[count]")
            __ASM_EMIT("jle             12f")

            // Tail: 1x-3x block
            __ASM_EMIT("test            $1, %[count]")
            __ASM_EMIT("jz              6f")
            __ASM_EMIT("movss           0x00(%[src]), %%xmm0")
            __ASM_EMIT("add             $4, %[src]")
            __ASM_EMIT("6:")
            __ASM_EMIT("test            $2, %[count]")
            __ASM_EMIT("jz              8f")
            __ASM_EMIT("movhps          0x00(%[src]), %%xmm0")
            __ASM_EMIT("8:")

            // logf(c)
            LOGN_CORE_X4
            __ASM_EMIT("mulps           0x00 + %[LOGC], %%xmm0")        // xmm0 = 2*y*L*log2(E)
            __ASM_EMIT("addps           %%xmm1, %%xmm0")                // xmm0 = 2*y*L*log2(E)+R
            __ASM_EMIT("mulps           %[V], %%xmm0")                  // xmm0 = (2*y*L*log2(E)+R)*v
            // expf(v * logf(c))
            POW2_CORE_X4

            __ASM_EMIT("test            $1, %[count]")
            __ASM_EMIT("jz              10f")
            __ASM_EMIT("movss           %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("add             $4, %[dst]")
            __ASM_EMIT("10:")
            __ASM_EMIT("test            $2, %[count]")
            __ASM_EMIT("jz              12f")
            __ASM_EMIT("movhps          %%xmm0, 0x00(%[dst])")

            // End
            __ASM_EMIT("12:")

            : [dst] "+r" (dst), [src] "+r" (c), [count] "+r" (count),
              "+Yz" (v)
            : [E2C] "o" (EXP2_CONST),
              [L2C] "o" (LOG2_CONST),
              [LOGC] "o" (LOGB_C),
              [V] "m" (V)
            : "cc", "memory",
              "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void powvx1(float *v, const float *x, size_t count)
    {
//        for (size_t i=0; i<count; ++i)
//            v[i] = expf(x[i] * logf(v[i]));

        IF_ARCH_X86(float *src;);

        ARCH_X86_ASM(
            // Analyze size
            __ASM_EMIT("sub             $8, %[count]")
            __ASM_EMIT("jb              2f")

            // x8 blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("movups          0x00(%[dst]), %%xmm0")
            __ASM_EMIT("movups          0x10(%[dst]), %%xmm4")
            // logf(v)
            LOGN_CORE_X8
            __ASM_EMIT("mulps           0x00 + %[LOGC], %%xmm0")        // xmm0 = 2*y*L*log2(E)
            __ASM_EMIT("mulps           0x00 + %[LOGC], %%xmm4")
            __ASM_EMIT("movups          0x00(%[x]), %%xmm2")            // xmm2 = x
            __ASM_EMIT("movups          0x10(%[x]), %%xmm6")
            __ASM_EMIT("addps           %%xmm1, %%xmm0")                // xmm0 = 2*y*L*log2(E) + R
            __ASM_EMIT("addps           %%xmm5, %%xmm4")
            __ASM_EMIT("mulps           %%xmm2, %%xmm0")                // xmm0 = (2*y*L*log2(E) + R)*x
            __ASM_EMIT("mulps           %%xmm6, %%xmm4")
            // expf(x * logf(v))
            POW2_CORE_X8
            __ASM_EMIT("movups          %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("movups          %%xmm4, 0x10(%[dst])")
            __ASM_EMIT("add             $0x20, %[x]")
            __ASM_EMIT("add             $0x20, %[dst]")
            __ASM_EMIT("sub             $8, %[count]")
            __ASM_EMIT("jae             1b")

            __ASM_EMIT("2:")
            __ASM_EMIT("add             $4, %[count]")
            __ASM_EMIT("jl              4f")

            // x4 block
            __ASM_EMIT("movups          0x00(%[dst]), %%xmm0")
            // logf(v)
            LOGN_CORE_X4
            __ASM_EMIT("mulps           0x00 + %[LOGC], %%xmm0")        // xmm0 = 2*y*L*log2(E)
            __ASM_EMIT("movups          0x00(%[x]), %%xmm2")            // xmm2 = x
            __ASM_EMIT("addps           %%xmm1, %%xmm0")                // xmm0 = 2*y*L*log2(E) + R
            __ASM_EMIT("mulps           %%xmm2, %%xmm0")                // xmm0 = (2*y*L*log2(E) + R)*x
            // expf(x * logf(x))
            POW2_CORE_X8
            __ASM_EMIT("movups          %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("add             $0x10, %[x]")
            __ASM_EMIT("add             $0x10, %[dst]")
            __ASM_EMIT("sub             $4, %[count]")

            __ASM_EMIT("4:")
            __ASM_EMIT("add             $4, %[count]")
            __ASM_EMIT("jle             12f")

            // Tail: 1x-3x block
            __ASM_EMIT("mov             %[dst], %[src]")
            __ASM_EMIT("test            $1, %[count]")
            __ASM_EMIT("jz              6f")
            __ASM_EMIT("movss           0x00(%[src]), %%xmm0")
            __ASM_EMIT("movss           0x00(%[x]), %%xmm7")
            __ASM_EMIT("add             $4, %[src]")
            __ASM_EMIT("add             $4, %[x]")
            __ASM_EMIT("6:")
            __ASM_EMIT("test            $2, %[count]")
            __ASM_EMIT("jz              8f")
            __ASM_EMIT("movhps          0x00(%[src]), %%xmm0")
            __ASM_EMIT("movhps          0x00(%[x]), %%xmm7")
            __ASM_EMIT("8:")

            // logf(x)
            LOGN_CORE_X4
            __ASM_EMIT("mulps           0x00 + %[LOGC], %%xmm0")        // xmm0 = 2*y*L*log2(E)
            __ASM_EMIT("addps           %%xmm1, %%xmm0")                // xmm0 = 2*y*L*log2(E) + R
            __ASM_EMIT("mulps           %%xmm7, %%xmm0")                // xmm0 = (2*y*L*log2(E) + R)*x
            // expf(v * logf(x))
            POW2_CORE_X8

            __ASM_EMIT("test            $1, %[count]")
            __ASM_EMIT("jz              10f")
            __ASM_EMIT("movss           %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("add             $4, %[dst]")
            __ASM_EMIT("10:")
            __ASM_EMIT("test            $2, %[count]")
            __ASM_EMIT("jz              12f")
            __ASM_EMIT("movhps          %%xmm0, 0x00(%[dst])")

            // End
            __ASM_EMIT("12:")

            : [dst] "+r" (v), [x] "+r" (x), [src] "=&r" (src), [count] "+r" (count)
            : [E2C] "o" (EXP2_CONST),
              [L2C] "o" (LOG2_CONST),
              [LOGC] "o" (LOGB_C)
            : "cc", "memory",
              "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void powvx2(float *dst, const float *v, const float *x, size_t count)
    {
//        for (size_t i=0; i<count; ++i)
//            dst[i] = expf(x[i] * logf(v[i]));

        ARCH_X86_ASM(
            // Analyze size
            __ASM_EMIT("sub             $8, %[count]")
            __ASM_EMIT("jb              2f")

            // x8 blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("movups          0x00(%[src]), %%xmm0")
            __ASM_EMIT("movups          0x10(%[src]), %%xmm4")
            // logf(v)
            LOGN_CORE_X8
            __ASM_EMIT("mulps           0x00 + %[LOGC], %%xmm0")        // xmm0 = 2*y*L*log2(E)
            __ASM_EMIT("mulps           0x00 + %[LOGC], %%xmm4")
            __ASM_EMIT("movups          0x00(%[x]), %%xmm2")            // xmm2 = x
            __ASM_EMIT("movups          0x10(%[x]), %%xmm6")
            __ASM_EMIT("addps           %%xmm1, %%xmm0")                // xmm0 = 2*y*L*log2(E) + R
            __ASM_EMIT("addps           %%xmm5, %%xmm4")
            __ASM_EMIT("mulps           %%xmm2, %%xmm0")                // xmm0 = (2*y*L*log2(E) + R)*x
            __ASM_EMIT("mulps           %%xmm6, %%xmm4")
            // expf(x * logf(v))
            POW2_CORE_X8
            __ASM_EMIT("movups          %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("movups          %%xmm4, 0x10(%[dst])")
            __ASM_EMIT("add             $0x20, %[src]")
            __ASM_EMIT("add             $0x20, %[dst]")
            __ASM_EMIT("add             $0x20, %[x]")
            __ASM_EMIT("sub             $8, %[count]")
            __ASM_EMIT("jae             1b")

            __ASM_EMIT("2:")
            __ASM_EMIT("add             $4, %[count]")
            __ASM_EMIT("jl              4f")

            // x4 block
            __ASM_EMIT("movups          0x00(%[src]), %%xmm0")
            // logf(x)
            LOGN_CORE_X4
            __ASM_EMIT("mulps           0x00 + %[LOGC], %%xmm0")        // xmm0 = 2*y*L*log2(E)
            __ASM_EMIT("movups          0x00(%[x]), %%xmm2")            // xmm2 = x
            __ASM_EMIT("addps           %%xmm1, %%xmm0")                // xmm0 = 2*y*L*log2(E) + R
            __ASM_EMIT("mulps           %%xmm2, %%xmm0")                // xmm0 = (2*y*L*log2(E) + R)*x
            // expf(x * logf(v))
            POW2_CORE_X8
            __ASM_EMIT("movups          %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("add             $0x10, %[src]")
            __ASM_EMIT("add             $0x10, %[dst]")
            __ASM_EMIT("add             $0x10, %[x]")
            __ASM_EMIT("sub             $4, %[count]")

            __ASM_EMIT("4:")
            __ASM_EMIT("add             $4, %[count]")
            __ASM_EMIT("jle             12f")

            // Tail: 1x-3x block
            __ASM_EMIT("test            $1, %[count]")
            __ASM_EMIT("jz              6f")
            __ASM_EMIT("movss           0x00(%[src]), %%xmm0")
            __ASM_EMIT("movss           0x00(%[x]), %%xmm7")
            __ASM_EMIT("add             $4, %[src]")
            __ASM_EMIT("add             $4, %[x]")
            __ASM_EMIT("6:")
            __ASM_EMIT("test            $2, %[count]")
            __ASM_EMIT("jz              8f")
            __ASM_EMIT("movhps          0x00(%[src]), %%xmm0")
            __ASM_EMIT("movhps          0x00(%[x]), %%xmm7")
            __ASM_EMIT("8:")

            // logf(v)
            LOGN_CORE_X4
            __ASM_EMIT("mulps           0x00 + %[LOGC], %%xmm0")        // xmm0 = 2*y*L*log2(E)
            __ASM_EMIT("addps           %%xmm1, %%xmm0")                // xmm0 = 2*y*L*log2(E) + R
            __ASM_EMIT("mulps           %%xmm7, %%xmm0")                // xmm0 = (2*y*L*log2(E) + R)*x
            // expf(x * logf(v))
            POW2_CORE_X8

            __ASM_EMIT("test            $1, %[count]")
            __ASM_EMIT("jz              10f")
            __ASM_EMIT("movss           %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("add             $4, %[dst]")
            __ASM_EMIT("10:")
            __ASM_EMIT("test            $2, %[count]")
            __ASM_EMIT("jz              12f")
            __ASM_EMIT("movhps          %%xmm0, 0x00(%[dst])")

            // End
            __ASM_EMIT("12:")

            : [dst] "+r" (dst), [x] "+r" (x), [src] "+r" (v), [count] "+r" (count)
            : [E2C] "o" (EXP2_CONST),
              [L2C] "o" (LOG2_CONST),
              [LOGC] "o" (LOGB_C)
            : "cc", "memory",
              "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }
}

#endif /* DSP_ARCH_X86_SSE2_PMATH_POW_H_ */
