/*
 * complex.h
 *
 *  Created on: 05 окт. 2015 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_SSE_COMPLEX_H_
#define DSP_ARCH_X86_SSE_COMPLEX_H_

#ifndef DSP_ARCH_X86_SSE_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_SSE_IMPL */

namespace sse
{
    void complex_mul2(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t count)
    {
        ARCH_X86_ASM
        (
            /* Check count */
            __ASM_EMIT("sub         $4, %[count]")
            __ASM_EMIT("jb          3f")

            __ASM_EMIT(".align 16")
            __ASM_EMIT("2:")
            /* Process vectorized data */
            __ASM_EMIT("movups      0x00(%[dst_re]), %%xmm0")           /* xmm0 = ar0 ar1 ar2 ar3 */
            __ASM_EMIT("movups      0x00(%[src_re]), %%xmm4")           /* xmm4 = br0 br1 br2 br3 */
            __ASM_EMIT("movups      0x00(%[dst_im]), %%xmm2")           /* xmm2 = ai0 ai1 ai2 ai3 */
            __ASM_EMIT("movups      0x00(%[src_im]), %%xmm6")           /* xmm6 = bi0 bi1 bi2 bi3 */
            /* Calc multiplication */
            __ASM_EMIT("movaps      %%xmm0, %%xmm1")                    /* xmm1 = ar0 ar1 ar2 ar3 */
            __ASM_EMIT("mulps       %%xmm4, %%xmm0")                    /* xmm0 = ar0*br0 ar1*br1 ar2*br2 ar3*br3 */
            __ASM_EMIT("mulps       %%xmm6, %%xmm1")                    /* xmm1 = ar0*bi0 ar1*bi1 ar2*bi2 ar3*bi3 */
            __ASM_EMIT("mulps       %%xmm2, %%xmm6")                    /* xmm6 = ai0*bi0 ai1*bi1 ai2*bi2 ai3*bi3 */
            __ASM_EMIT("mulps       %%xmm2, %%xmm4")                    /* xmm4 = ai0*br0 ai1*br1 ai2*br2 ai3*br3 */
            __ASM_EMIT("addps       %%xmm4, %%xmm1")                    /* xmm1 = ar[i]*bi[i] + ai[i]*br[i] = i0 i1 i2 i3 */
            __ASM_EMIT("subps       %%xmm6, %%xmm0")                    /* xmm0 = ar[i]*br[i] - ai[i]*bi[i] = r0 r1 r2 r3 */
            __ASM_EMIT("movups      %%xmm0, 0x00(%[dst_re])")
            __ASM_EMIT("movups      %%xmm1, 0x00(%[dst_im])")
            /* Repeat loop */
            __ASM_EMIT("add         $0x10, %[src_re]")
            __ASM_EMIT("add         $0x10, %[src_im]")
            __ASM_EMIT("add         $0x10, %[dst_re]")
            __ASM_EMIT("add         $0x10, %[dst_im]")
            __ASM_EMIT("sub         $4, %[count]")
            __ASM_EMIT("jae         2b")

            __ASM_EMIT("3:")
            __ASM_EMIT("add         $3, %[count]")
            __ASM_EMIT("jl          5f")

            /* Process scalar data */
            __ASM_EMIT("4:")
            __ASM_EMIT("movss       0x00(%[dst_re]), %%xmm0")           /* xmm0 = ar */
            __ASM_EMIT("movss       0x00(%[src_re]), %%xmm4")           /* xmm4 = br */
            __ASM_EMIT("movss       0x00(%[dst_im]), %%xmm2")           /* xmm2 = ai */
            __ASM_EMIT("movss       0x00(%[src_im]), %%xmm6")           /* xmm6 = bi */
            __ASM_EMIT("movaps      %%xmm0, %%xmm1")                    /* xmm1 = ar */
            __ASM_EMIT("mulss       %%xmm4, %%xmm0")                    /* xmm0 = ar*br */
            __ASM_EMIT("mulss       %%xmm6, %%xmm1")                    /* xmm1 = ar*bi */
            __ASM_EMIT("mulss       %%xmm2, %%xmm6")                    /* xmm6 = ai*bi */
            __ASM_EMIT("mulss       %%xmm2, %%xmm4")                    /* xmm4 = ai*br */
            __ASM_EMIT("addss       %%xmm4, %%xmm1")                    /* xmm1 = ar*bi + ai*br = i */
            __ASM_EMIT("subss       %%xmm6, %%xmm0")                    /* xmm0 = ar*br - ai*bi = r */
            __ASM_EMIT("movss       %%xmm0, 0x00(%[dst_re])")
            __ASM_EMIT("movss       %%xmm1, 0x00(%[dst_im])")
            __ASM_EMIT("add         $0x04, %[src_re]")
            __ASM_EMIT("add         $0x04, %[src_im]")
            __ASM_EMIT("add         $0x04, %[dst_re]")
            __ASM_EMIT("add         $0x04, %[dst_im]")
            __ASM_EMIT("dec         %[count]")
            __ASM_EMIT("jge         4b")

            /* Exit */
            __ASM_EMIT("5:")

            : [dst_re] "+r" (dst_re), [dst_im] "+r" (dst_im),
              [src_re] "+r" (src_re), [src_im] "+r" (src_im),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm4", "%xmm5", "%xmm6"
        );
    }

    void complex_mul3(float *dst_re, float *dst_im, const float *src1_re, const float *src1_im, const float *src2_re, const float *src2_im, size_t count)
    {
        size_t off;
        __IF_32( size_t temp_re, temp_im );

        ARCH_X86_ASM
        (
            /* Check count */
            __ASM_EMIT("xor         %[off], %[off]")
            __ASM_EMIT64("sub       $4, %[count]")
            __ASM_EMIT32("subl      $4, %[count]")
            __ASM_EMIT("jb          3f")

            __ASM_EMIT(".align 16")
            __ASM_EMIT("2:")
            /* Process vectorized data */
            __ASM_EMIT("movups      0x00(%[src1_re], %[off]), %%xmm0")  /* xmm0 = ar0 ar1 ar2 ar3 */
            __ASM_EMIT("movups      0x00(%[src2_re], %[off]), %%xmm4")  /* xmm4 = br0 br1 br2 br3 */
            __ASM_EMIT("movups      0x00(%[src1_im], %[off]), %%xmm2")  /* xmm2 = ai0 ai1 ai2 ai3 */
            __ASM_EMIT("movups      0x00(%[src2_im], %[off]), %%xmm6")  /* xmm6 = bi0 bi1 bi2 bi3 */
            /* Calc multiplication */
            __ASM_EMIT("movaps      %%xmm0, %%xmm1")            /* xmm1 = ar0 ar1 ar2 ar3 */
            __ASM_EMIT("mulps       %%xmm4, %%xmm0")            /* xmm0 = ar0*br0 ar1*br1 ar2*br2 ar3*br3 */
            __ASM_EMIT("mulps       %%xmm6, %%xmm1")            /* xmm1 = ar0*bi0 ar1*bi1 ar2*bi2 ar3*bi3 */
            __ASM_EMIT("mulps       %%xmm2, %%xmm6")            /* xmm6 = ai0*bi0 ai1*bi1 ai2*bi2 ai3*bi3 */
            __ASM_EMIT("mulps       %%xmm2, %%xmm4")            /* xmm4 = ai0*br0 ai1*br1 ai2*br2 ai3*br3 */
            __ASM_EMIT("addps       %%xmm4, %%xmm1")            /* xmm1 = ar[i]*bi[i] + ai[i]*br[i] = i0 i1 i2 i3 */
            __ASM_EMIT("subps       %%xmm6, %%xmm0")            /* xmm0 = ar[i]*br[i] - ai[i]*bi[i] = r0 r1 r2 r3 */
            /* Store (64-bit code) */
            __ASM_EMIT64("movups    %%xmm0, 0x00(%[dst_re], %[off])")
            __ASM_EMIT64("movups    %%xmm1, 0x00(%[dst_im], %[off])")
            /* Store (32-bit code) */
            __ASM_EMIT32("mov       %[src1_re], %[temp_re]")
            __ASM_EMIT32("mov       %[src1_im], %[temp_im]")
            __ASM_EMIT32("mov       %[dst_re], %[src1_re]")
            __ASM_EMIT32("mov       %[dst_im], %[src1_im]")
            __ASM_EMIT32("movups    %%xmm0, 0x00(%[src1_re], %[off])")
            __ASM_EMIT32("movups    %%xmm1, 0x00(%[src1_im], %[off])")
            __ASM_EMIT32("mov       %[temp_re], %[src1_re]")
            __ASM_EMIT32("mov       %[temp_im], %[src1_im]")
            /* Repeat loop */
            __ASM_EMIT("add         $0x10, %[off]")
            __ASM_EMIT64("sub       $4, %[count]")
            __ASM_EMIT32("subl      $4, %[count]")
            __ASM_EMIT("jae         2b")

            __ASM_EMIT("3:")
            __ASM_EMIT64("add       $4, %[count]")
            __ASM_EMIT32("addl      $4, %[count]")
            __ASM_EMIT("jle         5f")

            /* Process scalar data */
            __ASM_EMIT("4:")
            __ASM_EMIT("movss       0x00(%[src1_re], %[off]), %%xmm0")  /* xmm0 = ar */
            __ASM_EMIT("movss       0x00(%[src2_re], %[off]), %%xmm4")  /* xmm4 = br */
            __ASM_EMIT("movss       0x00(%[src1_im], %[off]), %%xmm2")  /* xmm2 = ai */
            __ASM_EMIT("movss       0x00(%[src2_im], %[off]), %%xmm6")  /* xmm6 = bi */
            /* Calculate multiplication */
            __ASM_EMIT("movaps      %%xmm0, %%xmm1")            /* xmm1 = ar */
            __ASM_EMIT("mulss       %%xmm4, %%xmm0")            /* xmm0 = ar*br */
            __ASM_EMIT("mulss       %%xmm6, %%xmm1")            /* xmm1 = ar*bi */
            __ASM_EMIT("mulss       %%xmm2, %%xmm6")            /* xmm6 = ai*bi */
            __ASM_EMIT("mulss       %%xmm2, %%xmm4")            /* xmm4 = ai*br */
            __ASM_EMIT("addss       %%xmm4, %%xmm1")            /* xmm1 = ar*bi + ai*br = i */
            __ASM_EMIT("subss       %%xmm6, %%xmm0")            /* xmm0 = ar*br - ai*bi = r */
            /* Store (64-bit code) */
            __ASM_EMIT64("movss     %%xmm0, 0x00(%[dst_re], %[off])")
            __ASM_EMIT64("movss     %%xmm1, 0x00(%[dst_im], %[off])")
            /* Store (32-bit code) */
            __ASM_EMIT32("mov       %[src1_re], %[temp_re]")
            __ASM_EMIT32("mov       %[src1_im], %[temp_im]")
            __ASM_EMIT32("mov       %[dst_re], %[src1_re]")
            __ASM_EMIT32("mov       %[dst_im], %[src1_im]")
            __ASM_EMIT32("movss     %%xmm0, 0x00(%[src1_re], %[off])")
            __ASM_EMIT32("movss     %%xmm1, 0x00(%[src1_im], %[off])")
            __ASM_EMIT32("mov       %[temp_re], %[src1_re]")
            __ASM_EMIT32("mov       %[temp_im], %[src1_im]")
            /* Repeat loop */
            __ASM_EMIT("add         $0x04, %[off]")
            __ASM_EMIT64("dec       %[count]")
            __ASM_EMIT32("decl      %[count]")
            __ASM_EMIT("jnz         4b")

            /* Exit */
            __ASM_EMIT("5:")

            : [dst_re] __IF_32_64("+m", "+r") (dst_re), [dst_im] __IF_32_64("+m", "+r") (dst_im),
              [src1_re] "+r" (src1_re), [src1_im] "+r" (src1_im),
              [src2_re] "+r" (src2_re), [src2_im] "+r" (src2_im),
              [count] __IF_32_64("+m", "+r") (count),
              __IF_32( [temp_re] "+m"  (temp_re), [temp_im] "+m"  (temp_im), )
              [off] "=&r" (off)
            :
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm4", "%xmm5", "%xmm6"
        );
    }

    void complex_div2(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t count)
    {
        ARCH_X86_ASM
        (
            __ASM_EMIT("sub             $4, %[count]")
            __ASM_EMIT("jb              2f")

            // x4 blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("movups          0x00(%[src_re]), %%xmm0")       // xmm0 = sr
            __ASM_EMIT("movups          0x00(%[src_im]), %%xmm1")       // xmm1 = si
            __ASM_EMIT("movups          0x00(%[dst_re]), %%xmm2")       // xmm2 = dr
            __ASM_EMIT("movups          0x00(%[dst_im]), %%xmm3")       // xmm3 = di
            __ASM_EMIT("movaps          %%xmm0, %%xmm4")                // xmm4 = sr
            __ASM_EMIT("movaps          %%xmm1, %%xmm5")                // xmm5 = si
            __ASM_EMIT("movaps          %%xmm0, %%xmm6")                // xmm6 = sr
            __ASM_EMIT("movaps          %%xmm1, %%xmm7")                // xmm7 = si
            __ASM_EMIT("mulps           %%xmm2, %%xmm0")                // xmm0 = sr*dr
            __ASM_EMIT("mulps           %%xmm3, %%xmm4")                // xmm4 = sr*di
            __ASM_EMIT("mulps           %%xmm2, %%xmm1")                // xmm1 = si*dr
            __ASM_EMIT("mulps           %%xmm3, %%xmm5")                // xmm5 = si*di
            __ASM_EMIT("mulps           %%xmm6, %%xmm6")                // xmm6 = sr*sr
            __ASM_EMIT("mulps           %%xmm7, %%xmm7")                // xmm7 = si*si
            __ASM_EMIT("addps           %%xmm5, %%xmm0")                // xmm0 = sr*dr + si*di
            __ASM_EMIT("addps           %%xmm1, %%xmm4")                // xmm4 = si*dr + sr*di
            __ASM_EMIT("addps           %%xmm7, %%xmm6")                // xmm6 = R = sr*sr + si*si
            __ASM_EMIT("divps           %%xmm6, %%xmm4")                // xmm4 = (sr*dr + si*di) / R
            __ASM_EMIT("xorps           %%xmm1, %%xmm1")                // xmm1 = 0
            __ASM_EMIT("divps           %%xmm6, %%xmm0")                // xmm0 = (sr*dr + si*di) / R
            __ASM_EMIT("subps           %%xmm4, %%xmm1")                // xmm1 = - (sr*dr + si*di) / R
            __ASM_EMIT("movups          %%xmm0, 0x00(%[dst_re])")
            __ASM_EMIT("movups          %%xmm1, 0x00(%[dst_im])")
            __ASM_EMIT("add             $0x10, %[src_re]")
            __ASM_EMIT("add             $0x10, %[src_im]")
            __ASM_EMIT("add             $0x10, %[dst_re]")
            __ASM_EMIT("add             $0x10, %[dst_im]")
            __ASM_EMIT("sub             $4, %[count]")
            __ASM_EMIT("jae             1b")

            // x1 blocks
            __ASM_EMIT("2:")
            __ASM_EMIT("add             $3, %[count]")
            __ASM_EMIT("jl              4f")
            __ASM_EMIT("3:")
            __ASM_EMIT("movss           0x00(%[src_re]), %%xmm0")       // xmm0 = sr
            __ASM_EMIT("movss           0x00(%[src_im]), %%xmm1")       // xmm1 = si
            __ASM_EMIT("movss           0x00(%[dst_re]), %%xmm2")       // xmm2 = dr
            __ASM_EMIT("movss           0x00(%[dst_im]), %%xmm3")       // xmm3 = di
            __ASM_EMIT("movaps          %%xmm0, %%xmm4")                // xmm4 = sr
            __ASM_EMIT("movaps          %%xmm1, %%xmm5")                // xmm5 = si
            __ASM_EMIT("movaps          %%xmm0, %%xmm6")                // xmm6 = sr
            __ASM_EMIT("movaps          %%xmm1, %%xmm7")                // xmm7 = si
            __ASM_EMIT("mulss           %%xmm2, %%xmm0")                // xmm0 = sr*dr
            __ASM_EMIT("mulss           %%xmm3, %%xmm4")                // xmm4 = sr*di
            __ASM_EMIT("mulss           %%xmm2, %%xmm1")                // xmm1 = si*dr
            __ASM_EMIT("mulss           %%xmm3, %%xmm5")                // xmm5 = si*di
            __ASM_EMIT("mulss           %%xmm6, %%xmm6")                // xmm6 = sr*sr
            __ASM_EMIT("mulss           %%xmm7, %%xmm7")                // xmm7 = si*si
            __ASM_EMIT("addss           %%xmm5, %%xmm0")                // xmm0 = sr*dr + si*di
            __ASM_EMIT("addss           %%xmm1, %%xmm4")                // xmm4 = si*dr + sr*di
            __ASM_EMIT("addss           %%xmm7, %%xmm6")                // xmm6 = R = sr*sr + si*si
            __ASM_EMIT("divss           %%xmm6, %%xmm4")                // xmm4 = (sr*dr + si*di) / R
            __ASM_EMIT("xorps           %%xmm1, %%xmm1")                // xmm1 = 0
            __ASM_EMIT("divss           %%xmm6, %%xmm0")                // xmm0 = (sr*dr + si*di) / R
            __ASM_EMIT("subss           %%xmm4, %%xmm1")                // xmm1 = - (sr*dr + si*di) / R
            __ASM_EMIT("movss           %%xmm0, 0x00(%[dst_re])")
            __ASM_EMIT("movss           %%xmm1, 0x00(%[dst_im])")
            __ASM_EMIT("add             $0x04, %[src_re]")
            __ASM_EMIT("add             $0x04, %[src_im]")
            __ASM_EMIT("add             $0x04, %[dst_re]")
            __ASM_EMIT("add             $0x04, %[dst_im]")
            __ASM_EMIT("dec             %[count]")
            __ASM_EMIT("jge             3b")

            __ASM_EMIT("4:")

            : [dst_re] "+r" (dst_re), [dst_im] "+r" (dst_im),
              [src_re] "+r" (src_re), [src_im] "+r" (src_im),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void complex_rdiv2(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t count)
    {
        ARCH_X86_ASM
        (
            __ASM_EMIT("sub             $4, %[count]")
            __ASM_EMIT("jb              2f")

            // x4 blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("movups          0x00(%[src_re]), %%xmm0")       // xmm0 = sr
            __ASM_EMIT("movups          0x00(%[src_im]), %%xmm1")       // xmm1 = si
            __ASM_EMIT("movups          0x00(%[dst_re]), %%xmm2")       // xmm2 = dr
            __ASM_EMIT("movups          0x00(%[dst_im]), %%xmm3")       // xmm3 = di
            __ASM_EMIT("movaps          %%xmm0, %%xmm4")                // xmm4 = sr
            __ASM_EMIT("movaps          %%xmm1, %%xmm5")                // xmm5 = si
            __ASM_EMIT("movaps          %%xmm2, %%xmm6")                // xmm6 = dr
            __ASM_EMIT("movaps          %%xmm3, %%xmm7")                // xmm7 = di
            __ASM_EMIT("mulps           %%xmm2, %%xmm0")                // xmm0 = sr*dr
            __ASM_EMIT("mulps           %%xmm3, %%xmm4")                // xmm4 = sr*di
            __ASM_EMIT("mulps           %%xmm2, %%xmm1")                // xmm1 = si*dr
            __ASM_EMIT("mulps           %%xmm3, %%xmm5")                // xmm5 = si*di
            __ASM_EMIT("mulps           %%xmm6, %%xmm6")                // xmm6 = dr*dr
            __ASM_EMIT("mulps           %%xmm7, %%xmm7")                // xmm7 = di*di
            __ASM_EMIT("addps           %%xmm5, %%xmm0")                // xmm0 = sr*dr + si*di
            __ASM_EMIT("addps           %%xmm1, %%xmm4")                // xmm4 = si*dr + sr*di
            __ASM_EMIT("addps           %%xmm7, %%xmm6")                // xmm6 = R = dr*dr + di*di
            __ASM_EMIT("divps           %%xmm6, %%xmm4")                // xmm4 = (sr*dr + si*di) / R
            __ASM_EMIT("xorps           %%xmm1, %%xmm1")                // xmm1 = 0
            __ASM_EMIT("divps           %%xmm6, %%xmm0")                // xmm0 = (sr*dr + si*di) / R
            __ASM_EMIT("subps           %%xmm4, %%xmm1")                // xmm1 = - (sr*dr + si*di) / R
            __ASM_EMIT("movups          %%xmm0, 0x00(%[dst_re])")
            __ASM_EMIT("movups          %%xmm1, 0x00(%[dst_im])")
            __ASM_EMIT("add             $0x10, %[src_re]")
            __ASM_EMIT("add             $0x10, %[src_im]")
            __ASM_EMIT("add             $0x10, %[dst_re]")
            __ASM_EMIT("add             $0x10, %[dst_im]")
            __ASM_EMIT("sub             $4, %[count]")
            __ASM_EMIT("jae             1b")

            // x1 blocks
            __ASM_EMIT("2:")
            __ASM_EMIT("add             $3, %[count]")
            __ASM_EMIT("jl              4f")
            __ASM_EMIT("3:")
            __ASM_EMIT("movss           0x00(%[src_re]), %%xmm0")       // xmm0 = sr
            __ASM_EMIT("movss           0x00(%[src_im]), %%xmm1")       // xmm1 = si
            __ASM_EMIT("movss           0x00(%[dst_re]), %%xmm2")       // xmm2 = dr
            __ASM_EMIT("movss           0x00(%[dst_im]), %%xmm3")       // xmm3 = di
            __ASM_EMIT("movaps          %%xmm0, %%xmm4")                // xmm4 = sr
            __ASM_EMIT("movaps          %%xmm1, %%xmm5")                // xmm5 = si
            __ASM_EMIT("movaps          %%xmm2, %%xmm6")                // xmm6 = dr
            __ASM_EMIT("movaps          %%xmm3, %%xmm7")                // xmm7 = di
            __ASM_EMIT("mulss           %%xmm2, %%xmm0")                // xmm0 = sr*dr
            __ASM_EMIT("mulss           %%xmm3, %%xmm4")                // xmm4 = sr*di
            __ASM_EMIT("mulss           %%xmm2, %%xmm1")                // xmm1 = si*dr
            __ASM_EMIT("mulss           %%xmm3, %%xmm5")                // xmm5 = si*di
            __ASM_EMIT("mulss           %%xmm6, %%xmm6")                // xmm6 = dr*dr
            __ASM_EMIT("mulss           %%xmm7, %%xmm7")                // xmm7 = di*di
            __ASM_EMIT("addss           %%xmm5, %%xmm0")                // xmm0 = sr*dr + si*di
            __ASM_EMIT("addss           %%xmm1, %%xmm4")                // xmm4 = si*dr + sr*di
            __ASM_EMIT("addss           %%xmm7, %%xmm6")                // xmm6 = R = dr*dr + di*di
            __ASM_EMIT("divss           %%xmm6, %%xmm4")                // xmm4 = (sr*dr + si*di) / R
            __ASM_EMIT("xorps           %%xmm1, %%xmm1")                // xmm1 = 0
            __ASM_EMIT("divss           %%xmm6, %%xmm0")                // xmm0 = (sr*dr + si*di) / R
            __ASM_EMIT("subss           %%xmm4, %%xmm1")                // xmm1 = - (sr*dr + si*di) / R
            __ASM_EMIT("movss           %%xmm0, 0x00(%[dst_re])")
            __ASM_EMIT("movss           %%xmm1, 0x00(%[dst_im])")
            __ASM_EMIT("add             $0x04, %[src_re]")
            __ASM_EMIT("add             $0x04, %[src_im]")
            __ASM_EMIT("add             $0x04, %[dst_re]")
            __ASM_EMIT("add             $0x04, %[dst_im]")
            __ASM_EMIT("dec             %[count]")
            __ASM_EMIT("jge             3b")

            __ASM_EMIT("4:")

            : [dst_re] "+r" (dst_re), [dst_im] "+r" (dst_im),
              [src_re] "+r" (src_re), [src_im] "+r" (src_im),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void complex_div3(float *dst_re, float *dst_im, const float *t_re, const float *t_im, const float *b_re, const float *b_im, size_t count)
    {
        IF_ARCH_I386(size_t tmp1, tmp2);

        ARCH_X86_ASM
        (
            __ASM_EMIT("sub             $4, %[count]")
            __ASM_EMIT("jb              2f")

            // x4 blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("movups          0x00(%[t_re]), %%xmm0")         // xmm0 = tr
            __ASM_EMIT("movups          0x00(%[t_im]), %%xmm1")         // xmm1 = ti
            __ASM_EMIT("movups          0x00(%[b_re]), %%xmm2")         // xmm2 = br
            __ASM_EMIT("movups          0x00(%[b_im]), %%xmm3")         // xmm3 = bi
            __ASM_EMIT("movaps          %%xmm0, %%xmm4")                // xmm4 = tr
            __ASM_EMIT("movaps          %%xmm1, %%xmm5")                // xmm5 = ti
            __ASM_EMIT("movaps          %%xmm2, %%xmm6")                // xmm6 = br
            __ASM_EMIT("movaps          %%xmm3, %%xmm7")                // xmm7 = bi
            __ASM_EMIT("add             $0x10, %[t_re]")
            __ASM_EMIT("add             $0x10, %[t_im]")
            __ASM_EMIT("add             $0x10, %[b_re]")
            __ASM_EMIT("add             $0x10, %[b_im]")
            __ASM_EMIT("mulps           %%xmm2, %%xmm0")                // xmm0 = tr*br
            __ASM_EMIT("mulps           %%xmm3, %%xmm4")                // xmm4 = tr*bi
            __ASM_EMIT("mulps           %%xmm2, %%xmm1")                // xmm1 = ti*br
            __ASM_EMIT("mulps           %%xmm3, %%xmm5")                // xmm5 = ti*bi
            __ASM_EMIT("mulps           %%xmm6, %%xmm6")                // xmm6 = br*br
            __ASM_EMIT("mulps           %%xmm7, %%xmm7")                // xmm7 = bi*bi
            __ASM_EMIT("addps           %%xmm5, %%xmm0")                // xmm0 = tr*br + ti*bi
            __ASM_EMIT("addps           %%xmm1, %%xmm4")                // xmm4 = ti*br + tr*bi
            __ASM_EMIT("addps           %%xmm7, %%xmm6")                // xmm6 = R = br*br + bi*bi
            __ASM_EMIT("divps           %%xmm6, %%xmm4")                // xmm4 = (tr*br + ti*bi) / R
            __ASM_EMIT("xorps           %%xmm1, %%xmm1")                // xmm1 = 0
            __ASM_EMIT("divps           %%xmm6, %%xmm0")                // xmm0 = (tr*br + ti*bi) / R
            __ASM_EMIT("subps           %%xmm4, %%xmm1")                // xmm1 = - (tr*br + ti*bi) / R

            __ASM_EMIT64("movups        %%xmm0, 0x00(%[dst_re])")
            __ASM_EMIT64("movups        %%xmm1, 0x00(%[dst_im])")
            __ASM_EMIT64("add           $0x10, %[dst_re]")
            __ASM_EMIT64("add           $0x10, %[dst_im]")
            __ASM_EMIT32("mov           %[b_re], %[tmp1]")
            __ASM_EMIT32("mov           %[b_im], %[tmp2]")
            __ASM_EMIT32("mov           %[dst_re], %[b_re]")
            __ASM_EMIT32("mov           %[dst_im], %[b_im]")
            __ASM_EMIT32("movups        %%xmm0, 0x00(%[b_re])")
            __ASM_EMIT32("movups        %%xmm1, 0x00(%[b_im])")
            __ASM_EMIT32("addl          $0x10, %[dst_re]")
            __ASM_EMIT32("addl          $0x10, %[dst_im]")
            __ASM_EMIT32("mov           %[tmp1], %[b_re]")
            __ASM_EMIT32("mov           %[tmp2], %[b_im]")

            __ASM_EMIT("sub             $4, %[count]")
            __ASM_EMIT("jae             1b")

            // x1 blocks
            __ASM_EMIT("2:")
            __ASM_EMIT("add             $3, %[count]")
            __ASM_EMIT("jl              4f")
            __ASM_EMIT("3:")
            __ASM_EMIT("movss           0x00(%[t_re]), %%xmm0")         // xmm0 = tr
            __ASM_EMIT("movss           0x00(%[t_im]), %%xmm1")         // xmm1 = ti
            __ASM_EMIT("movss           0x00(%[b_re]), %%xmm2")         // xmm2 = br
            __ASM_EMIT("movss           0x00(%[b_im]), %%xmm3")         // xmm3 = bi
            __ASM_EMIT("movaps          %%xmm0, %%xmm4")                // xmm4 = tr
            __ASM_EMIT("movaps          %%xmm1, %%xmm5")                // xmm5 = ti
            __ASM_EMIT("movaps          %%xmm2, %%xmm6")                // xmm6 = br
            __ASM_EMIT("movaps          %%xmm3, %%xmm7")                // xmm7 = bi
            __ASM_EMIT("add             $0x04, %[t_re]")
            __ASM_EMIT("add             $0x04, %[t_im]")
            __ASM_EMIT("add             $0x04, %[b_re]")
            __ASM_EMIT("add             $0x04, %[b_im]")
            __ASM_EMIT("mulss           %%xmm2, %%xmm0")                // xmm0 = tr*br
            __ASM_EMIT("mulss           %%xmm3, %%xmm4")                // xmm4 = tr*bi
            __ASM_EMIT("mulss           %%xmm2, %%xmm1")                // xmm1 = ti*br
            __ASM_EMIT("mulss           %%xmm3, %%xmm5")                // xmm5 = ti*bi
            __ASM_EMIT("mulss           %%xmm6, %%xmm6")                // xmm6 = br*br
            __ASM_EMIT("mulss           %%xmm7, %%xmm7")                // xmm7 = bi*bi
            __ASM_EMIT("addss           %%xmm5, %%xmm0")                // xmm0 = tr*br + ti*bi
            __ASM_EMIT("addss           %%xmm1, %%xmm4")                // xmm4 = ti*br + tr*bi
            __ASM_EMIT("addss           %%xmm7, %%xmm6")                // xmm6 = R = br*br + bi*bi
            __ASM_EMIT("divss           %%xmm6, %%xmm4")                // xmm4 = (tr*br + ti*bi) / R
            __ASM_EMIT("xorps           %%xmm1, %%xmm1")                // xmm1 = 0
            __ASM_EMIT("divss           %%xmm6, %%xmm0")                // xmm0 = (tr*br + ti*bi) / R
            __ASM_EMIT("subss           %%xmm4, %%xmm1")                // xmm1 = - (tr*br + ti*bi) / R

            __ASM_EMIT64("movss         %%xmm0, 0x00(%[dst_re])")
            __ASM_EMIT64("movss         %%xmm1, 0x00(%[dst_im])")
            __ASM_EMIT64("add           $0x04, %[dst_re]")
            __ASM_EMIT64("add           $0x04, %[dst_im]")
            __ASM_EMIT32("mov           %[b_re], %[tmp1]")
            __ASM_EMIT32("mov           %[b_im], %[tmp2]")
            __ASM_EMIT32("mov           %[dst_re], %[b_re]")
            __ASM_EMIT32("mov           %[dst_im], %[b_im]")
            __ASM_EMIT32("movss         %%xmm0, 0x00(%[b_re])")
            __ASM_EMIT32("movss         %%xmm1, 0x00(%[b_im])")
            __ASM_EMIT32("addl          $0x04, %[dst_re]")
            __ASM_EMIT32("addl          $0x04, %[dst_im]")
            __ASM_EMIT32("mov           %[tmp1], %[b_re]")
            __ASM_EMIT32("mov           %[tmp2], %[b_im]")

            __ASM_EMIT("dec             %[count]")
            __ASM_EMIT("jge             3b")

            __ASM_EMIT("4:")

            : IF_ARCH_X86_64([dst_re] "+r" (dst_re), [dst_im] "+r" (dst_im),)
              IF_ARCH_I386([dst_re] "+m" (dst_re), [dst_im] "+m" (dst_im), [tmp1] "+m" (tmp1), [tmp2] "+m" (tmp2), )
              [t_re] "+r" (t_re), [t_im] "+r" (t_im),
              [b_re] "+r" (b_re), [b_im] "+r" (b_im),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void complex_mod(float *dst, const float *src_re, const float *src_im, size_t count)
    {
        size_t off;

        ARCH_X86_ASM
        (
            __ASM_EMIT("xor         %[off], %[off]")
            __ASM_EMIT("sub         $16, %[count]")
            __ASM_EMIT("jb          2f")

            // 16x blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("movups      0x00(%[src_re], %[off]), %%xmm0")
            __ASM_EMIT("movups      0x10(%[src_re], %[off]), %%xmm1")
            __ASM_EMIT("movups      0x20(%[src_re], %[off]), %%xmm2")
            __ASM_EMIT("movups      0x30(%[src_re], %[off]), %%xmm3")
            __ASM_EMIT("movups      0x00(%[src_im], %[off]), %%xmm4")
            __ASM_EMIT("movups      0x10(%[src_im], %[off]), %%xmm5")
            __ASM_EMIT("movups      0x20(%[src_im], %[off]), %%xmm6")
            __ASM_EMIT("movups      0x30(%[src_im], %[off]), %%xmm7")
            __ASM_EMIT("mulps       %%xmm0, %%xmm0")
            __ASM_EMIT("mulps       %%xmm1, %%xmm1")
            __ASM_EMIT("mulps       %%xmm2, %%xmm2")
            __ASM_EMIT("mulps       %%xmm3, %%xmm3")
            __ASM_EMIT("mulps       %%xmm4, %%xmm4")
            __ASM_EMIT("mulps       %%xmm5, %%xmm5")
            __ASM_EMIT("mulps       %%xmm6, %%xmm6")
            __ASM_EMIT("mulps       %%xmm7, %%xmm7")
            __ASM_EMIT("addps       %%xmm4, %%xmm0")
            __ASM_EMIT("addps       %%xmm5, %%xmm1")
            __ASM_EMIT("addps       %%xmm6, %%xmm2")
            __ASM_EMIT("addps       %%xmm7, %%xmm3")
            __ASM_EMIT("sqrtps      %%xmm0, %%xmm0")
            __ASM_EMIT("sqrtps      %%xmm1, %%xmm1")
            __ASM_EMIT("sqrtps      %%xmm2, %%xmm2")
            __ASM_EMIT("sqrtps      %%xmm3, %%xmm3")
            __ASM_EMIT("movups      %%xmm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("movups      %%xmm1, 0x10(%[dst], %[off])")
            __ASM_EMIT("movups      %%xmm2, 0x20(%[dst], %[off])")
            __ASM_EMIT("movups      %%xmm3, 0x30(%[dst], %[off])")
            __ASM_EMIT("add         $0x40, %[off]")
            __ASM_EMIT("sub         $16, %[count]")
            __ASM_EMIT("jae         1b")

            // 4x blocks
            __ASM_EMIT("2:")
            __ASM_EMIT("add         $12, %[count]")
            __ASM_EMIT("jl          4f")
            __ASM_EMIT("3:")
            __ASM_EMIT("movups      0x00(%[src_re], %[off]), %%xmm0")
            __ASM_EMIT("movups      0x00(%[src_im], %[off]), %%xmm4")
            __ASM_EMIT("mulps       %%xmm0, %%xmm0")
            __ASM_EMIT("mulps       %%xmm4, %%xmm4")
            __ASM_EMIT("addps       %%xmm4, %%xmm0")
            __ASM_EMIT("sqrtps      %%xmm0, %%xmm0")
            __ASM_EMIT("movups      %%xmm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("add         $0x10, %[off]")
            __ASM_EMIT("sub         $4, %[count]")
            __ASM_EMIT("jae         3b")

            // 1x blocks
            __ASM_EMIT("4:")
            __ASM_EMIT("add         $3, %[count]")
            __ASM_EMIT("jl          6f")
            __ASM_EMIT("5:")
            __ASM_EMIT("movss       0x00(%[src_re], %[off]), %%xmm0")
            __ASM_EMIT("movss       0x00(%[src_im], %[off]), %%xmm4")
            __ASM_EMIT("mulss       %%xmm0, %%xmm0")
            __ASM_EMIT("mulss       %%xmm4, %%xmm4")
            __ASM_EMIT("addss       %%xmm4, %%xmm0")
            __ASM_EMIT("sqrtss      %%xmm0, %%xmm0")
            __ASM_EMIT("movss       %%xmm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("add         $0x4, %[off]")
            __ASM_EMIT("dec         %[count]")
            __ASM_EMIT("jge         5b")

            // End
            __ASM_EMIT("6:")
            : [dst] "+r" (dst), [src_re] "+r" (src_re), [src_im] "+r"(src_im),
              [count] "+r" (count), [off] "=&r" (off)
            :
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }


    void complex_rcp2(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t count)
    {
        ARCH_X86_ASM (
            /* Do block processing */
            __ASM_EMIT("test        %[count], %[count]")
            __ASM_EMIT("jz          50f")

            __ASM_EMIT("sub         $8, %[count]")              /* count -= 8 */
            __ASM_EMIT("jb          2f")
            __ASM_EMIT("1:")
            /* Perform 8x RCP */
            __ASM_EMIT("movups      0x00(%[src_re]),  %%xmm0")  /* xmm0  = r0 */
            __ASM_EMIT("movups      0x10(%[src_re]),  %%xmm2")  /* xmm2  = r1 */
            __ASM_EMIT("movups      0x00(%[src_im]),  %%xmm1")  /* xmm1  = i0 */
            __ASM_EMIT("movups      0x10(%[src_im]),  %%xmm3")  /* xmm3  = i1 */
            __ASM_EMIT("movaps      %%xmm0, %%xmm4")            /* xmm4  = r0 */
            __ASM_EMIT("movaps      %%xmm1, %%xmm5")            /* xmm5  = i0 */
            __ASM_EMIT("movaps      %%xmm2, %%xmm6")            /* xmm6  = r1 */
            __ASM_EMIT("movaps      %%xmm3, %%xmm7")            /* xmm7  = i1 */
            __ASM_EMIT("mulps       %%xmm0, %%xmm4")            /* xmm4  = r0*r0 */
            __ASM_EMIT("mulps       %%xmm1, %%xmm5")            /* xmm5  = i0*i0 */
            __ASM_EMIT("mulps       %%xmm2, %%xmm6")            /* xmm6  = r1*r1 */
            __ASM_EMIT("mulps       %%xmm3, %%xmm7")            /* xmm7  = i1*i1 */
            __ASM_EMIT("addps       %%xmm5, %%xmm4")            /* xmm4  = r0*r0 + i0*i0 */
            __ASM_EMIT("addps       %%xmm7, %%xmm6")            /* xmm6  = r1*r1 + i1*i1 */
            __ASM_EMIT("movaps      %[X_ONE], %%xmm5")          /* xmm5  = 1 */
            __ASM_EMIT("xorps       %[X_ISIGN], %%xmm1")        /* xmm1  = -i0 */
            __ASM_EMIT("movaps      %%xmm5, %%xmm7")            /* xmm7  = 1 */
            __ASM_EMIT("xorps       %[X_ISIGN], %%xmm3")        /* xmm3  = -i1 */
            __ASM_EMIT("divps       %%xmm4, %%xmm5")            /* xmm5  = 1 / (r0*r0 + i0*i0) */
            __ASM_EMIT("divps       %%xmm6, %%xmm7")            /* xmm7  = 1 / (r1*r1 + i1*i1) */
            __ASM_EMIT("mulps       %%xmm5, %%xmm0")            /* xmm0  = r0' = r0 / (r0*r0 + i0*i0) */
            __ASM_EMIT("mulps       %%xmm7, %%xmm2")            /* xmm2  = r1' = r1 / (r1*r1 + i1*i1) */
            __ASM_EMIT("mulps       %%xmm5, %%xmm1")            /* xmm1  = i0' = -i0 / (r0*r0 + i0*i0) */
            __ASM_EMIT("mulps       %%xmm7, %%xmm3")            /* xmm3  = i1' = -i1 / (r1*r1 + i1*i1) */
            __ASM_EMIT("movups      %%xmm0, 0x00(%[dst_re])")   /* dst_re[0]  = r0' */
            __ASM_EMIT("movups      %%xmm2, 0x10(%[dst_re])")   /* dst_re[1]  = r1' */
            __ASM_EMIT("movups      %%xmm1, 0x00(%[dst_im])")   /* dst_im[0]  = i0' */
            __ASM_EMIT("movups       %%xmm3, 0x10(%[dst_im])")   /* dst_im[1]  = i1' */
            __ASM_EMIT("add         $0x20, %[src_re]")          /* src_re += 8 */
            __ASM_EMIT("add         $0x20, %[src_im]")          /* src_im += 8 */
            __ASM_EMIT("add         $0x20, %[dst_re]")          /* dst_re += 8 */
            __ASM_EMIT("add         $0x20, %[dst_im]")          /* dst_im += 8 */
            __ASM_EMIT("sub         $8, %[count]")              /* count -= 8 */
            __ASM_EMIT("jae         1b")
            /* 4x RCP */
            __ASM_EMIT("2:")
            __ASM_EMIT("add         $4, %[count]")              /* count += 4 */
            __ASM_EMIT("jl          10f")
            __ASM_EMIT("movups      0x00(%[src_re]),  %%xmm0")  /* xmm0  = r0 */
            __ASM_EMIT("movups      0x00(%[src_im]),  %%xmm1")  /* xmm1  = i0 */
            __ASM_EMIT("movaps      %%xmm0, %%xmm4")            /* xmm4  = r0 */
            __ASM_EMIT("movaps      %%xmm1, %%xmm5")            /* xmm5  = i0 */
            __ASM_EMIT("mulps       %%xmm0, %%xmm4")            /* xmm4  = r0*r0 */
            __ASM_EMIT("mulps       %%xmm1, %%xmm5")            /* xmm5  = i0*i0 */
            __ASM_EMIT("addps       %%xmm5, %%xmm4")            /* xmm4  = r0*r0 + i0*i0 */
            __ASM_EMIT("movaps      %[X_ONE], %%xmm5")          /* xmm5  = 1 */
            __ASM_EMIT("xorps       %[X_ISIGN], %%xmm1")        /* xmm1  = -i0 */
            __ASM_EMIT("divps       %%xmm4, %%xmm5")            /* xmm5  = 1 / (r0*r0 + i0*i0) */
            __ASM_EMIT("mulps       %%xmm5, %%xmm0")            /* xmm0  = r0' = r0 / (r0*r0 + i0*i0) */
            __ASM_EMIT("mulps       %%xmm5, %%xmm1")            /* xmm1  = i0' = -i0 / (r0*r0 + i0*i0) */
            __ASM_EMIT("movups      %%xmm0, 0x00(%[dst_re])")   /* dst_re[0]  = r0' */
            __ASM_EMIT("movups      %%xmm1, 0x00(%[dst_im])")   /* dst_im[0]  = i0' */
            __ASM_EMIT("add         $0x10, %[src_re]")          /* src_re += 4 */
            __ASM_EMIT("add         $0x10, %[src_im]")          /* src_im += 4 */
            __ASM_EMIT("add         $0x10, %[dst_re]")          /* dst_re += 4 */
            __ASM_EMIT("add         $0x10, %[dst_im]")          /* dst_im += 4 */
            __ASM_EMIT("sub         $4, %[count]")              /* count -= 4 */
            /* 1x RCP */
            __ASM_EMIT("10:")
            __ASM_EMIT("add         $4, %[count]")              /* count += 4 */
            __ASM_EMIT("jle         50f")
            __ASM_EMIT("40:")
            __ASM_EMIT("movss       0x00(%[src_re]), %%xmm0")   /* xmm0  = r0 */
            __ASM_EMIT("movss       0x00(%[src_im]), %%xmm1")   /* xmm1  = i0 */
            __ASM_EMIT("movaps      %%xmm0, %%xmm4")            /* xmm4  = r0 */
            __ASM_EMIT("movaps      %%xmm1, %%xmm5")            /* xmm5  = i0 */
            __ASM_EMIT("mulss       %%xmm0, %%xmm4")            /* xmm4  = r0*r0 */
            __ASM_EMIT("mulss       %%xmm1, %%xmm5")            /* xmm5  = i0*i0 */
            __ASM_EMIT("addss       %%xmm5, %%xmm4")            /* xmm4  = r0*r0 + i0*i0 */
            __ASM_EMIT("movaps      %[X_ONE], %%xmm5")          /* xmm5  = 1 */
            __ASM_EMIT("xorps       %[X_ISIGN], %%xmm1")        /* xmm1  = -i0 */
            __ASM_EMIT("divss       %%xmm4, %%xmm5")            /* xmm5  = 1 / (r0*r0 + i0*i0) */
            __ASM_EMIT("mulss       %%xmm5, %%xmm0")            /* xmm0  = r0' = r0 / (r0*r0 + i0*i0) */
            __ASM_EMIT("mulss       %%xmm5, %%xmm1")            /* xmm1  = i0' = -i0 / (r0*r0 + i0*i0) */
            __ASM_EMIT("movss       %%xmm0, 0x00(%[dst_re])")   /* dst_re[0]  = r0' */
            __ASM_EMIT("movss       %%xmm1, 0x00(%[dst_im])")   /* dst_im[0]  = i0' */
            __ASM_EMIT("add         $0x04, %[src_re]")          /* src_re++ */
            __ASM_EMIT("add         $0x04, %[src_im]")          /* src_im++ */
            __ASM_EMIT("add         $0x04, %[dst_re]")          /* dst_re++ */
            __ASM_EMIT("add         $0x04, %[dst_im]")          /* dst_im++ */
            __ASM_EMIT("dec         %[count]")                  /* count-- */
            __ASM_EMIT("jnz         40b")

            __ASM_EMIT("50:")
            : [dst_re] "+r" (dst_re), [dst_im] "+r" (dst_im),
              [src_re] "+r" (src_re), [src_im] "+r" (src_im),
              [count] "+r" (count)
            : [X_ONE] "m" (ONE),
              [X_ISIGN] "m" (X_ISIGN)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm4", "%xmm5", "%xmm6"
        );
    }

    void complex_rcp1(float *dst_re, float *dst_im, size_t count)
    {
        ARCH_X86_ASM (
            /* Do block processing */
            __ASM_EMIT("test        %[count], %[count]")
            __ASM_EMIT("jz          50f")

            __ASM_EMIT("sub         $8, %[count]")              /* count -= 8 */
            __ASM_EMIT("jb          2f")
            __ASM_EMIT("1:")
            /* Perform 8x RCP */
            __ASM_EMIT("movups      0x00(%[dst_re]), %%xmm0")   /* xmm0  = r0 */
            __ASM_EMIT("movups      0x10(%[dst_re]), %%xmm2")   /* xmm2  = r1 */
            __ASM_EMIT("movups      0x00(%[dst_im]), %%xmm1")   /* xmm1  = i0 */
            __ASM_EMIT("movups      0x10(%[dst_im]), %%xmm3")   /* xmm3  = i1 */
            __ASM_EMIT("movaps      %%xmm0, %%xmm4")            /* xmm4  = r0 */
            __ASM_EMIT("movaps      %%xmm1, %%xmm5")            /* xmm5  = i0 */
            __ASM_EMIT("movaps      %%xmm2, %%xmm6")            /* xmm6  = r1 */
            __ASM_EMIT("movaps      %%xmm3, %%xmm7")            /* xmm7  = i1 */
            __ASM_EMIT("mulps       %%xmm0, %%xmm4")            /* xmm4  = r0*r0 */
            __ASM_EMIT("mulps       %%xmm1, %%xmm5")            /* xmm5  = i0*i0 */
            __ASM_EMIT("mulps       %%xmm2, %%xmm6")            /* xmm6  = r1*r1 */
            __ASM_EMIT("mulps       %%xmm3, %%xmm7")            /* xmm7  = i1*i1 */
            __ASM_EMIT("addps       %%xmm5, %%xmm4")            /* xmm4  = r0*r0 + i0*i0 */
            __ASM_EMIT("addps       %%xmm7, %%xmm6")            /* xmm6  = r1*r1 + i1*i1 */
            __ASM_EMIT("movaps      %[X_ONE], %%xmm5")          /* xmm5  = 1 */
            __ASM_EMIT("xorps       %[X_ISIGN], %%xmm1")        /* xmm1  = -i0 */
            __ASM_EMIT("movaps      %%xmm5, %%xmm7")            /* xmm7  = 1 */
            __ASM_EMIT("xorps       %[X_ISIGN], %%xmm3")        /* xmm3  = -i1 */
            __ASM_EMIT("divps       %%xmm4, %%xmm5")            /* xmm5  = 1 / (r0*r0 + i0*i0) */
            __ASM_EMIT("divps       %%xmm6, %%xmm7")            /* xmm7  = 1 / (r1*r1 + i1*i1) */
            __ASM_EMIT("mulps       %%xmm5, %%xmm0")            /* xmm0  = r0' = r0 / (r0*r0 + i0*i0) */
            __ASM_EMIT("mulps       %%xmm7, %%xmm2")            /* xmm2  = r1' = r1 / (r1*r1 + i1*i1) */
            __ASM_EMIT("mulps       %%xmm5, %%xmm1")            /* xmm1  = i0' = -i0 / (r0*r0 + i0*i0) */
            __ASM_EMIT("mulps       %%xmm7, %%xmm3")            /* xmm3  = i1' = -i1 / (r1*r1 + i1*i1) */
            __ASM_EMIT("movups      %%xmm0, 0x00(%[dst_re])")   /* dst_re[0]  = r0' */
            __ASM_EMIT("movups      %%xmm2, 0x10(%[dst_re])")   /* dst_re[1]  = r1' */
            __ASM_EMIT("movups      %%xmm1, 0x00(%[dst_im])")   /* dst_im[0]  = i0' */
            __ASM_EMIT("movups      %%xmm3, 0x10(%[dst_im])")   /* dst_im[1]  = i1' */
            __ASM_EMIT("add         $0x20, %[dst_re]")          /* dst_re += 8 */
            __ASM_EMIT("add         $0x20, %[dst_im]")          /* dst_im += 8 */
            __ASM_EMIT("sub         $8, %[count]")              /* count -= 8 */
            __ASM_EMIT("jae         1b")
            /* 4x RCP */
            __ASM_EMIT("2:")
            __ASM_EMIT("add         $4, %[count]")              /* count += 4 */
            __ASM_EMIT("jl          10f")
            __ASM_EMIT("movups      0x00(%[dst_re]), %%xmm0")   /* xmm0  = r0 */
            __ASM_EMIT("movups      0x00(%[dst_im]), %%xmm1")   /* xmm1  = i0 */
            __ASM_EMIT("movaps      %%xmm0, %%xmm4")            /* xmm4  = r0 */
            __ASM_EMIT("movaps      %%xmm1, %%xmm5")            /* xmm5  = i0 */
            __ASM_EMIT("mulps       %%xmm0, %%xmm4")            /* xmm4  = r0*r0 */
            __ASM_EMIT("mulps       %%xmm1, %%xmm5")            /* xmm5  = i0*i0 */
            __ASM_EMIT("addps       %%xmm5, %%xmm4")            /* xmm4  = r0*r0 + i0*i0 */
            __ASM_EMIT("movaps      %[X_ONE], %%xmm5")          /* xmm5  = 1 */
            __ASM_EMIT("xorps       %[X_ISIGN], %%xmm1")        /* xmm1  = -i0 */
            __ASM_EMIT("divps       %%xmm4, %%xmm5")            /* xmm5  = 1 / (r0*r0 + i0*i0) */
            __ASM_EMIT("mulps       %%xmm5, %%xmm0")            /* xmm0  = r0' = r0 / (r0*r0 + i0*i0) */
            __ASM_EMIT("mulps       %%xmm5, %%xmm1")            /* xmm1  = i0' = -i0 / (r0*r0 + i0*i0) */
            __ASM_EMIT("movups      %%xmm0, 0x00(%[dst_re])")   /* dst_re[0]  = r0' */
            __ASM_EMIT("movups      %%xmm1, 0x00(%[dst_im])")   /* dst_im[0]  = i0' */
            __ASM_EMIT("add         $0x10, %[dst_re]")          /* dst_re += 4 */
            __ASM_EMIT("add         $0x10, %[dst_im]")          /* dst_im += 4 */
            __ASM_EMIT("sub         $4, %[count]")              /* count -= 4 */

            /* 1x RCP */
            __ASM_EMIT("10:")
            __ASM_EMIT("add         $4, %[count]")              /* count += 4 */
            __ASM_EMIT("jle         50f")
            __ASM_EMIT("40:")
            __ASM_EMIT("movss       0x00(%[dst_re]), %%xmm0")   /* xmm0  = r0 */
            __ASM_EMIT("movss       0x00(%[dst_im]), %%xmm1")   /* xmm1  = i0 */
            __ASM_EMIT("movaps      %%xmm0, %%xmm4")            /* xmm4  = r0 */
            __ASM_EMIT("movaps      %%xmm1, %%xmm5")            /* xmm5  = i0 */
            __ASM_EMIT("mulss       %%xmm0, %%xmm4")            /* xmm4  = r0*r0 */
            __ASM_EMIT("mulss       %%xmm1, %%xmm5")            /* xmm5  = i0*i0 */
            __ASM_EMIT("addss       %%xmm5, %%xmm4")            /* xmm4  = r0*r0 + i0*i0 */
            __ASM_EMIT("movaps      %[X_ONE], %%xmm5")          /* xmm5  = 1 */
            __ASM_EMIT("xorps       %[X_ISIGN], %%xmm1")        /* xmm1  = -i0 */
            __ASM_EMIT("divss       %%xmm4, %%xmm5")            /* xmm5  = 1 / (r0*r0 + i0*i0) */
            __ASM_EMIT("mulss       %%xmm5, %%xmm0")            /* xmm0  = r0' = r0 / (r0*r0 + i0*i0) */
            __ASM_EMIT("mulss       %%xmm5, %%xmm1")            /* xmm1  = i0' = -i0 / (r0*r0 + i0*i0) */
            __ASM_EMIT("movss       %%xmm0, 0x00(%[dst_re])")   /* dst_re[0]  = r0' */
            __ASM_EMIT("movss       %%xmm1, 0x00(%[dst_im])")   /* dst_im[0]  = i0' */
            __ASM_EMIT("add         $0x04, %[dst_re]")          /* dst_re++ */
            __ASM_EMIT("add         $0x04, %[dst_im]")          /* dst_im++ */
            __ASM_EMIT("dec         %[count]")                  /* count-- */
            __ASM_EMIT("jnz         40b")

            __ASM_EMIT("50:")
            : [dst_re] "+r" (dst_re), [dst_im] "+r" (dst_im),
              [count] "+r" (count)
            : [X_ONE] "m" (ONE),
              [X_ISIGN] "m" (X_ISIGN)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }
}

#endif /* DSP_ARCH_X86_SSE_COMPLEX_H_ */
