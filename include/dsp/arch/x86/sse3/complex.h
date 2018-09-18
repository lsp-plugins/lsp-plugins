/*
 * complex.h
 *
 *  Created on: 17 февр. 2017 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_SSE3_COMPLEX_H_
#define DSP_ARCH_X86_SSE3_COMPLEX_H_

#ifndef DSP_ARCH_X86_SSE3_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_SSE3_IMPL */

namespace sse3
{
    void pcomplex_mul3(float *dst, const float *src1, const float *src2, size_t count)
    {
        size_t off;

        ARCH_X86_ASM
        (
            /* Check count */
            __ASM_EMIT("xor         %[off], %[off]")
            __ASM_EMIT("sub         $4, %[count]")
            __ASM_EMIT("jb          3f")

            __ASM_EMIT(".align 16")
            __ASM_EMIT("2:")
            /* Process vectorized data */
            __ASM_EMIT("lddqu       0x00(%[src1], %[off]), %%xmm0")     /* xmm0 = ar0 ai0 ar1 ai1 */
            __ASM_EMIT("lddqu       0x10(%[src1], %[off]), %%xmm4")     /* xmm4 = ar2 ai2 ar3 ai3 */
            __ASM_EMIT("lddqu       0x00(%[src2], %[off]), %%xmm1")     /* xmm1 = br0 bi0 br1 bi1 */
            __ASM_EMIT("lddqu       0x10(%[src2], %[off]), %%xmm5")     /* xmm5 = br2 bi2 br3 bi3 */
            /* Calc multiplication */
            __ASM_EMIT("movsldup    %%xmm0, %%xmm2")            /* xmm2 = ar0 ar0 ar1, ar1 */
            __ASM_EMIT("movsldup    %%xmm4, %%xmm6")            /* xmm6 = ar2 ar2 ar3, ar3 */
            __ASM_EMIT("movshdup    %%xmm0, %%xmm0")            /* xmm0 = ai0 ai0 ai1, ai1 */
            __ASM_EMIT("movshdup    %%xmm4, %%xmm4")            /* xmm4 = ai2 ai2 ai3, ai3 */
            __ASM_EMIT("mulps       %%xmm1, %%xmm0")            /* xmm0 = ai0*br0 ai0*bi0 ai1*br1 ai1*bi1 */
            __ASM_EMIT("mulps       %%xmm5, %%xmm4")            /* xmm4 = ai2*br2 ai2*bi2 ai3*br3 ai3*bi3 */
            __ASM_EMIT("mulps       %%xmm1, %%xmm2")            /* xmm2 = ar0*br0 ar0*bi0 ar1*br1 ar1*bi1 */
            __ASM_EMIT("mulps       %%xmm5, %%xmm6")            /* xmm6 = ar2*br2 ar2*bi2 ar3*br3 ar3*bi3 */
            __ASM_EMIT("shufps      $0xb1, %%xmm0, %%xmm0")     /* xmm0 = ai0*bi0 ai0*br0 ai1*bi1 ai1*br1 */
            __ASM_EMIT("shufps      $0xb1, %%xmm4, %%xmm4")     /* xmm4 = ai2*bi2 ai2*br2 ai3*bi3 ai3*br3 */
            __ASM_EMIT("addsubps    %%xmm0, %%xmm2")            /* xmm2 = ar0*br0 - ai0*bi0 ar0*bi0 + ai0*br0 ar1*br1 - ai1*bi1 ar1*bi1 + ai1*br1 */
            __ASM_EMIT("addsubps    %%xmm4, %%xmm6")            /* xmm6 = ar2*br2 - ai2*bi2 ar2*bi2 + ai2*br2 ar3*br3 - ai3*bi3 ar3*bi3 + ai3*br3 */
            /* Store */
            __ASM_EMIT("movdqu      %%xmm2, 0x00(%[dst], %[off])")
            __ASM_EMIT("movdqu      %%xmm6, 0x10(%[dst], %[off])")
            /* Repeat loop */
            __ASM_EMIT("add         $0x20, %[off]")
            __ASM_EMIT("sub         $4, %[count]")
            __ASM_EMIT("jae         2b")

            /* Check count again */
            __ASM_EMIT("3:")
            __ASM_EMIT("add         $4, %[count]")
            __ASM_EMIT("jle         5f")

            /* Process scalar data */
            __ASM_EMIT("4:")
            /* Load */
            __ASM_EMIT("movss       0x00(%[src1], %[off]), %%xmm0")     /* xmm0 = ar */
            __ASM_EMIT("movss       0x04(%[src1], %[off]), %%xmm2")     /* xmm2 = ai */
            __ASM_EMIT("movss       0x00(%[src2], %[off]), %%xmm4")     /* xmm4 = br */
            __ASM_EMIT("movss       0x04(%[src2], %[off]), %%xmm6")     /* xmm6 = bi */
            /* Calculate multiplication */
            __ASM_EMIT("movaps      %%xmm0, %%xmm1")            /* xmm1 = ar */
            __ASM_EMIT("mulss       %%xmm4, %%xmm0")            /* xmm0 = ar*br */
            __ASM_EMIT("mulss       %%xmm6, %%xmm1")            /* xmm1 = ar*bi */
            __ASM_EMIT("mulss       %%xmm2, %%xmm6")            /* xmm6 = ai*bi */
            __ASM_EMIT("mulss       %%xmm2, %%xmm4")            /* xmm4 = ai*br */
            __ASM_EMIT("addss       %%xmm4, %%xmm1")            /* xmm1 = ar*bi + ai*br = i */
            __ASM_EMIT("subss       %%xmm6, %%xmm0")            /* xmm0 = ar*br - ai*bi = r */
            /* Store */
            __ASM_EMIT("movss       %%xmm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("movss       %%xmm1, 0x04(%[dst], %[off])")
            /* Repeat loop */
            __ASM_EMIT("add         $0x08, %[off]")
            __ASM_EMIT("dec         %[count]")
            __ASM_EMIT("jnz         4b")

            /* Exit */
            __ASM_EMIT("5:")

            : [dst] "+r" (dst), [src1] "+r" (src1), [src2] "+r" (src2),
              [off] "=&r" (off),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm4", "%xmm5", "%xmm6"
        );

        #undef complex_core
    }

    void pcomplex_mod(float *dst, const float *src, size_t count)
    {
        size_t off;

        ARCH_X86_ASM
        (
            __ASM_EMIT("xor         %[off], %[off]")
            __ASM_EMIT("sub         $16, %[count]")
            __ASM_EMIT("jb          2f")

            // 16x blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("lddqu       0x00(%[src], %[off], 2), %%xmm0")
            __ASM_EMIT("lddqu       0x10(%[src], %[off], 2), %%xmm1")
            __ASM_EMIT("lddqu       0x20(%[src], %[off], 2), %%xmm2")
            __ASM_EMIT("lddqu       0x30(%[src], %[off], 2), %%xmm3")
            __ASM_EMIT("lddqu       0x40(%[src], %[off], 2), %%xmm4")
            __ASM_EMIT("lddqu       0x50(%[src], %[off], 2), %%xmm5")
            __ASM_EMIT("lddqu       0x60(%[src], %[off], 2), %%xmm6")
            __ASM_EMIT("lddqu       0x70(%[src], %[off], 2), %%xmm7")

            __ASM_EMIT("mulps       %%xmm0, %%xmm0")
            __ASM_EMIT("mulps       %%xmm1, %%xmm1")
            __ASM_EMIT("mulps       %%xmm2, %%xmm2")
            __ASM_EMIT("mulps       %%xmm3, %%xmm3")
            __ASM_EMIT("mulps       %%xmm4, %%xmm4")
            __ASM_EMIT("mulps       %%xmm5, %%xmm5")
            __ASM_EMIT("mulps       %%xmm6, %%xmm6")
            __ASM_EMIT("mulps       %%xmm7, %%xmm7")
            __ASM_EMIT("haddps      %%xmm1, %%xmm0")
            __ASM_EMIT("haddps      %%xmm3, %%xmm2")
            __ASM_EMIT("haddps      %%xmm5, %%xmm4")
            __ASM_EMIT("haddps      %%xmm7, %%xmm6")
            __ASM_EMIT("sqrtps      %%xmm0, %%xmm0")
            __ASM_EMIT("sqrtps      %%xmm2, %%xmm2")
            __ASM_EMIT("sqrtps      %%xmm4, %%xmm4")
            __ASM_EMIT("sqrtps      %%xmm6, %%xmm6")

            __ASM_EMIT("movdqu      %%xmm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("movdqu      %%xmm2, 0x10(%[dst], %[off])")
            __ASM_EMIT("movdqu      %%xmm4, 0x20(%[dst], %[off])")
            __ASM_EMIT("movdqu      %%xmm6, 0x30(%[dst], %[off])")
            __ASM_EMIT("add         $0x40, %[off]")
            __ASM_EMIT("sub         $16, %[count]")
            __ASM_EMIT("jae         1b")

            // 4x blocks
            __ASM_EMIT("2:")
            __ASM_EMIT("add         $12, %[count]")
            __ASM_EMIT("jl          4f")
            __ASM_EMIT("3:")
            __ASM_EMIT("lddqu       0x00(%[src], %[off], 2), %%xmm0")
            __ASM_EMIT("lddqu       0x10(%[src], %[off], 2), %%xmm1")
            __ASM_EMIT("mulps       %%xmm0, %%xmm0")
            __ASM_EMIT("mulps       %%xmm1, %%xmm1")
            __ASM_EMIT("haddps      %%xmm1, %%xmm0")
            __ASM_EMIT("sqrtps      %%xmm0, %%xmm0")
            __ASM_EMIT("movdqu      %%xmm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("add         $0x10, %[off]")
            __ASM_EMIT("sub         $4, %[count]")
            __ASM_EMIT("jae         3b")

            // 1x blocks
            __ASM_EMIT("4:")
            __ASM_EMIT("add         $3, %[count]")
            __ASM_EMIT("jl          6f")
            __ASM_EMIT("5:")
            __ASM_EMIT("movlps      0x00(%[src], %[off], 2), %%xmm0")
            __ASM_EMIT("mulps       %%xmm0, %%xmm0")
            __ASM_EMIT("haddps      %%xmm0, %%xmm0")
            __ASM_EMIT("sqrtss      %%xmm0, %%xmm0")
            __ASM_EMIT("movss       %%xmm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("add         $0x4, %[off]")
            __ASM_EMIT("dec         %[count]")
            __ASM_EMIT("jge         5b")

            // End
            __ASM_EMIT("6:")
            : [dst] "+r" (dst), [src] "+r" (src),
              [count] "+r" (count), [off] "=&r" (off)
            :
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void x64_pcomplex_mul3(float *dst, const float *src1, const float *src2, size_t count)
    {
        IF_ARCH_X86_64(size_t off);

        ARCH_X86_64_ASM
        (
            /* Do block processing */
            __ASM_EMIT("xor         %[off], %[off]")
            __ASM_EMIT("sub         $8, %[count]")
            __ASM_EMIT("jb          3f")

            __ASM_EMIT(".align 16")
            __ASM_EMIT("2:")
            /* Process vectorized data */
            __ASM_EMIT("lddqu       0x00(%[src1], %[off]), %%xmm0")     /* xmm0 = ar0 ai0 ar1 ai1 */
            __ASM_EMIT("lddqu       0x10(%[src1], %[off]), %%xmm4")     /* xmm4 = ar2 ai2 ar3 ai3 */
            __ASM_EMIT("lddqu       0x20(%[src1], %[off]), %%xmm8")
            __ASM_EMIT("lddqu       0x30(%[src1], %[off]), %%xmm12")
            __ASM_EMIT("lddqu       0x00(%[src2], %[off]), %%xmm1")     /* xmm1 = br0 bi0 br1 bi1 */
            __ASM_EMIT("lddqu       0x10(%[src2], %[off]), %%xmm5")     /* xmm5 = br2 bi2 br3 bi3 */
            __ASM_EMIT("lddqu       0x20(%[src2], %[off]), %%xmm9")
            __ASM_EMIT("lddqu       0x30(%[src2], %[off]), %%xmm13")
            /* Calc multiplication */
            __ASM_EMIT("movsldup    %%xmm0, %%xmm2")            /* xmm2 = ar0 ar0 ar1, ar1 */
            __ASM_EMIT("movsldup    %%xmm4, %%xmm6")            /* xmm6 = ar2 ar2 ar3, ar3 */
            __ASM_EMIT("movsldup    %%xmm8, %%xmm10")
            __ASM_EMIT("movsldup    %%xmm12, %%xmm14")
            __ASM_EMIT("movshdup    %%xmm0, %%xmm0")            /* xmm0 = ai0 ai0 ai1, ai1 */
            __ASM_EMIT("movshdup    %%xmm4, %%xmm4")            /* xmm4 = ai2 ai2 ai3, ai3 */
            __ASM_EMIT("movshdup    %%xmm8, %%xmm8")
            __ASM_EMIT("movshdup    %%xmm12, %%xmm12")
            __ASM_EMIT("mulps       %%xmm1, %%xmm0")            /* xmm0 = ai0*br0 ai0*bi0 ai1*br1 ai1*bi1 */
            __ASM_EMIT("mulps       %%xmm5, %%xmm4")            /* xmm4 = ai2*br2 ai2*bi2 ai3*br3 ai3*bi3 */
            __ASM_EMIT("mulps       %%xmm9, %%xmm8")
            __ASM_EMIT("mulps       %%xmm13, %%xmm12")
            __ASM_EMIT("mulps       %%xmm1, %%xmm2")            /* xmm2 = ar0*br0 ar0*bi0 ar1*br1 ar1*bi1 */
            __ASM_EMIT("mulps       %%xmm5, %%xmm6")            /* xmm6 = ar2*br2 ar2*bi2 ar3*br3 ar3*bi3 */
            __ASM_EMIT("mulps       %%xmm9, %%xmm10")
            __ASM_EMIT("mulps       %%xmm13, %%xmm14")
            __ASM_EMIT("shufps      $0xb1, %%xmm0, %%xmm0")     /* xmm0 = ai0*bi0 ai0*br0 ai1*bi1 ai1*br1 */
            __ASM_EMIT("shufps      $0xb1, %%xmm4, %%xmm4")     /* xmm4 = ai2*bi2 ai2*br2 ai3*bi3 ai3*br3 */
            __ASM_EMIT("shufps      $0xb1, %%xmm8, %%xmm8")
            __ASM_EMIT("shufps      $0xb1, %%xmm12, %%xmm12")
            __ASM_EMIT("addsubps    %%xmm0, %%xmm2")            /* xmm2 = ar0*br0 - ai0*bi0 ar0*bi0 + ai0*br0 ar1*br1 - ai1*bi1 ar1*bi1 + ai1*br1 */
            __ASM_EMIT("addsubps    %%xmm4, %%xmm6")            /* xmm6 = ar2*br2 - ai2*bi2 ar2*bi2 + ai2*br2 ar3*br3 - ai3*bi3 ar3*bi3 + ai3*br3 */
            __ASM_EMIT("addsubps    %%xmm8, %%xmm10")
            __ASM_EMIT("addsubps    %%xmm12, %%xmm14")
            /* Store */
            __ASM_EMIT("movdqu      %%xmm2, 0x00(%[dst], %[off])")
            __ASM_EMIT("movdqu      %%xmm6, 0x10(%[dst], %[off])")
            __ASM_EMIT("movdqu      %%xmm10, 0x20(%[dst], %[off])")
            __ASM_EMIT("movdqu      %%xmm14, 0x30(%[dst], %[off])")
            /* Repeat loop */
            __ASM_EMIT("add         $0x40, %[off]")
            __ASM_EMIT("sub         $8, %[count]")
            __ASM_EMIT("jae         2b")

            /* 4-element block */
            __ASM_EMIT("3:")
            __ASM_EMIT("add         $4, %[count]")
            __ASM_EMIT("jl          4f")
            /* Process vectorized data */
            __ASM_EMIT("lddqu       0x00(%[src1], %[off]), %%xmm0")     /* xmm0 = ar0 ai0 ar1 ai1 */
            __ASM_EMIT("lddqu       0x10(%[src1], %[off]), %%xmm4")     /* xmm4 = ar2 ai2 ar3 ai3 */
            __ASM_EMIT("lddqu       0x00(%[src2], %[off]), %%xmm1")     /* xmm1 = br0 bi0 br1 bi1 */
            __ASM_EMIT("lddqu       0x10(%[src2], %[off]), %%xmm5")     /* xmm5 = br2 bi2 br3 bi3 */
            /* Calc multiplication */
            __ASM_EMIT("movsldup    %%xmm0, %%xmm2")            /* xmm2 = ar0 ar0 ar1, ar1 */
            __ASM_EMIT("movsldup    %%xmm4, %%xmm6")            /* xmm6 = ar2 ar2 ar3, ar3 */
            __ASM_EMIT("movshdup    %%xmm0, %%xmm0")            /* xmm0 = ai0 ai0 ai1, ai1 */
            __ASM_EMIT("movshdup    %%xmm4, %%xmm4")            /* xmm4 = ai2 ai2 ai3, ai3 */
            __ASM_EMIT("mulps       %%xmm1, %%xmm0")            /* xmm0 = ai0*br0 ai0*bi0 ai1*br1 ai1*bi1 */
            __ASM_EMIT("mulps       %%xmm5, %%xmm4")            /* xmm4 = ai2*br2 ai2*bi2 ai3*br3 ai3*bi3 */
            __ASM_EMIT("mulps       %%xmm1, %%xmm2")            /* xmm2 = ar0*br0 ar0*bi0 ar1*br1 ar1*bi1 */
            __ASM_EMIT("mulps       %%xmm5, %%xmm6")            /* xmm6 = ar2*br2 ar2*bi2 ar3*br3 ar3*bi3 */
            __ASM_EMIT("shufps      $0xb1, %%xmm0, %%xmm0")     /* xmm0 = ai0*bi0 ai0*br0 ai1*bi1 ai1*br1 */
            __ASM_EMIT("shufps      $0xb1, %%xmm4, %%xmm4")     /* xmm4 = ai2*bi2 ai2*br2 ai3*bi3 ai3*br3 */
            __ASM_EMIT("addsubps    %%xmm0, %%xmm2")            /* xmm2 = ar0*br0 - ai0*bi0 ar0*bi0 + ai0*br0 ar1*br1 - ai1*bi1 ar1*bi1 + ai1*br1 */
            __ASM_EMIT("addsubps    %%xmm4, %%xmm6")            /* xmm6 = ar2*br2 - ai2*bi2 ar2*bi2 + ai2*br2 ar3*br3 - ai3*bi3 ar3*bi3 + ai3*br3 */
            /* Store */
            __ASM_EMIT("movdqu      %%xmm2, 0x00(%[dst], %[off])")
            __ASM_EMIT("movdqu      %%xmm6, 0x10(%[dst], %[off])")
            /* Repeat loop */
            __ASM_EMIT("add         $0x20, %[off]")
            __ASM_EMIT("sub         $4, %[count]")

            /* Check count again */
            __ASM_EMIT("4:")
            __ASM_EMIT("add         $4, %[count]")
            __ASM_EMIT("jle         6f")
            /* Process scalar data */
            __ASM_EMIT("5:")
            /* Load */
            __ASM_EMIT("movss       0x00(%[src1], %[off]), %%xmm0")     /* xmm0 = ar */
            __ASM_EMIT("movss       0x04(%[src1], %[off]), %%xmm2")     /* xmm2 = ai */
            __ASM_EMIT("movss       0x00(%[src2], %[off]), %%xmm4")     /* xmm4 = br */
            __ASM_EMIT("movss       0x04(%[src2], %[off]), %%xmm6")     /* xmm6 = bi */
            /* Calculate multiplication */
            __ASM_EMIT("movaps      %%xmm0, %%xmm1")            /* xmm1 = ar */
            __ASM_EMIT("mulss       %%xmm4, %%xmm0")            /* xmm0 = ar*br */
            __ASM_EMIT("mulss       %%xmm6, %%xmm1")            /* xmm1 = ar*bi */
            __ASM_EMIT("mulss       %%xmm2, %%xmm6")            /* xmm6 = ai*bi */
            __ASM_EMIT("mulss       %%xmm2, %%xmm4")            /* xmm4 = ai*br */
            __ASM_EMIT("addss       %%xmm4, %%xmm1")            /* xmm1 = ar*bi + ai*br = i */
            __ASM_EMIT("subss       %%xmm6, %%xmm0")            /* xmm0 = ar*br - ai*bi = r */
            /* Store */
            __ASM_EMIT("movss       %%xmm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("movss       %%xmm1, 0x04(%[dst], %[off])")
            /* Repeat loop */
            __ASM_EMIT("add         $0x08, %[off]")
            __ASM_EMIT("dec         %[count]")
            __ASM_EMIT("jnz         5b")

            /* Exit */
            __ASM_EMIT("6:")

            : [dst] "+r" (dst), [src1] "+r" (src1), [src2] "+r" (src2),
              [off] "=&r" (off),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm4", "%xmm5", "%xmm6",
              "%xmm8", "%xmm9", "%xmm10", "%xmm12", "%xmm13", "%xmm14"
        );
    }

    void x64_pcomplex_mod(float *dst, const float *src, size_t count)
    {
        IF_ARCH_X86_64(size_t off);

        ARCH_X86_64_ASM
        (
            __ASM_EMIT("xor         %[off], %[off]")
            __ASM_EMIT("sub         $32, %[count]")
            __ASM_EMIT("jb          2f")

            // 32x blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("lddqu       0x00(%[src], %[off], 2), %%xmm0")
            __ASM_EMIT("lddqu       0x10(%[src], %[off], 2), %%xmm1")
            __ASM_EMIT("lddqu       0x20(%[src], %[off], 2), %%xmm2")
            __ASM_EMIT("lddqu       0x30(%[src], %[off], 2), %%xmm3")
            __ASM_EMIT("lddqu       0x40(%[src], %[off], 2), %%xmm4")
            __ASM_EMIT("lddqu       0x50(%[src], %[off], 2), %%xmm5")
            __ASM_EMIT("lddqu       0x60(%[src], %[off], 2), %%xmm6")
            __ASM_EMIT("lddqu       0x70(%[src], %[off], 2), %%xmm7")
            __ASM_EMIT("lddqu       0x80(%[src], %[off], 2), %%xmm8")
            __ASM_EMIT("lddqu       0x90(%[src], %[off], 2), %%xmm9")
            __ASM_EMIT("lddqu       0xa0(%[src], %[off], 2), %%xmm10")
            __ASM_EMIT("lddqu       0xb0(%[src], %[off], 2), %%xmm11")
            __ASM_EMIT("lddqu       0xc0(%[src], %[off], 2), %%xmm12")
            __ASM_EMIT("lddqu       0xd0(%[src], %[off], 2), %%xmm13")
            __ASM_EMIT("lddqu       0xe0(%[src], %[off], 2), %%xmm14")
            __ASM_EMIT("lddqu       0xf0(%[src], %[off], 2), %%xmm15")

            __ASM_EMIT("mulps       %%xmm0, %%xmm0")
            __ASM_EMIT("mulps       %%xmm1, %%xmm1")
            __ASM_EMIT("mulps       %%xmm2, %%xmm2")
            __ASM_EMIT("mulps       %%xmm3, %%xmm3")
            __ASM_EMIT("mulps       %%xmm4, %%xmm4")
            __ASM_EMIT("mulps       %%xmm5, %%xmm5")
            __ASM_EMIT("mulps       %%xmm6, %%xmm6")
            __ASM_EMIT("mulps       %%xmm7, %%xmm7")
            __ASM_EMIT("mulps       %%xmm8, %%xmm8")
            __ASM_EMIT("mulps       %%xmm9, %%xmm9")
            __ASM_EMIT("mulps       %%xmm10, %%xmm10")
            __ASM_EMIT("mulps       %%xmm11, %%xmm11")
            __ASM_EMIT("mulps       %%xmm12, %%xmm12")
            __ASM_EMIT("mulps       %%xmm13, %%xmm13")
            __ASM_EMIT("mulps       %%xmm14, %%xmm14")
            __ASM_EMIT("mulps       %%xmm15, %%xmm15")

            __ASM_EMIT("haddps      %%xmm1, %%xmm0")
            __ASM_EMIT("haddps      %%xmm3, %%xmm2")
            __ASM_EMIT("haddps      %%xmm5, %%xmm4")
            __ASM_EMIT("haddps      %%xmm7, %%xmm6")
            __ASM_EMIT("haddps      %%xmm9, %%xmm8")
            __ASM_EMIT("haddps      %%xmm11, %%xmm10")
            __ASM_EMIT("haddps      %%xmm13, %%xmm12")
            __ASM_EMIT("haddps      %%xmm15, %%xmm14")

            __ASM_EMIT("sqrtps      %%xmm0, %%xmm0")
            __ASM_EMIT("sqrtps      %%xmm2, %%xmm2")
            __ASM_EMIT("sqrtps      %%xmm4, %%xmm4")
            __ASM_EMIT("sqrtps      %%xmm6, %%xmm6")
            __ASM_EMIT("sqrtps      %%xmm8, %%xmm8")
            __ASM_EMIT("sqrtps      %%xmm10, %%xmm10")
            __ASM_EMIT("sqrtps      %%xmm12, %%xmm12")
            __ASM_EMIT("sqrtps      %%xmm14, %%xmm14")

            __ASM_EMIT("movdqu      %%xmm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("movdqu      %%xmm2, 0x10(%[dst], %[off])")
            __ASM_EMIT("movdqu      %%xmm4, 0x20(%[dst], %[off])")
            __ASM_EMIT("movdqu      %%xmm6, 0x30(%[dst], %[off])")
            __ASM_EMIT("movdqu      %%xmm8, 0x40(%[dst], %[off])")
            __ASM_EMIT("movdqu      %%xmm10, 0x50(%[dst], %[off])")
            __ASM_EMIT("movdqu      %%xmm12, 0x60(%[dst], %[off])")
            __ASM_EMIT("movdqu      %%xmm14, 0x70(%[dst], %[off])")

            __ASM_EMIT("add         $0x80, %[off]")
            __ASM_EMIT("sub         $32, %[count]")
            __ASM_EMIT("jae         1b")

            // 16x blocks
            __ASM_EMIT("2:")
            __ASM_EMIT("add         $16, %[count]")
            __ASM_EMIT("jl          4f")

            __ASM_EMIT("3:")
            __ASM_EMIT("lddqu       0x00(%[src], %[off], 2), %%xmm0")
            __ASM_EMIT("lddqu       0x10(%[src], %[off], 2), %%xmm1")
            __ASM_EMIT("lddqu       0x20(%[src], %[off], 2), %%xmm2")
            __ASM_EMIT("lddqu       0x30(%[src], %[off], 2), %%xmm3")
            __ASM_EMIT("lddqu       0x40(%[src], %[off], 2), %%xmm4")
            __ASM_EMIT("lddqu       0x50(%[src], %[off], 2), %%xmm5")
            __ASM_EMIT("lddqu       0x60(%[src], %[off], 2), %%xmm6")
            __ASM_EMIT("lddqu       0x70(%[src], %[off], 2), %%xmm7")

            __ASM_EMIT("mulps       %%xmm0, %%xmm0")
            __ASM_EMIT("mulps       %%xmm1, %%xmm1")
            __ASM_EMIT("mulps       %%xmm2, %%xmm2")
            __ASM_EMIT("mulps       %%xmm3, %%xmm3")
            __ASM_EMIT("mulps       %%xmm4, %%xmm4")
            __ASM_EMIT("mulps       %%xmm5, %%xmm5")
            __ASM_EMIT("mulps       %%xmm6, %%xmm6")
            __ASM_EMIT("mulps       %%xmm7, %%xmm7")
            __ASM_EMIT("haddps      %%xmm1, %%xmm0")
            __ASM_EMIT("haddps      %%xmm3, %%xmm2")
            __ASM_EMIT("haddps      %%xmm5, %%xmm4")
            __ASM_EMIT("haddps      %%xmm7, %%xmm6")
            __ASM_EMIT("sqrtps      %%xmm0, %%xmm0")
            __ASM_EMIT("sqrtps      %%xmm2, %%xmm2")
            __ASM_EMIT("sqrtps      %%xmm4, %%xmm4")
            __ASM_EMIT("sqrtps      %%xmm6, %%xmm6")

            __ASM_EMIT("movdqu      %%xmm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("movdqu      %%xmm2, 0x10(%[dst], %[off])")
            __ASM_EMIT("movdqu      %%xmm4, 0x20(%[dst], %[off])")
            __ASM_EMIT("movdqu      %%xmm6, 0x30(%[dst], %[off])")
            __ASM_EMIT("add         $0x40, %[off]")
            __ASM_EMIT("sub         $16, %[count]")
            __ASM_EMIT("jae         3b")

            // 4x blocks
            __ASM_EMIT("4:")
            __ASM_EMIT("add         $12, %[count]")
            __ASM_EMIT("jl          6f")
            __ASM_EMIT("5:")
            __ASM_EMIT("lddqu       0x00(%[src], %[off], 2), %%xmm0")
            __ASM_EMIT("lddqu       0x10(%[src], %[off], 2), %%xmm1")
            __ASM_EMIT("mulps       %%xmm0, %%xmm0")
            __ASM_EMIT("mulps       %%xmm1, %%xmm1")
            __ASM_EMIT("haddps      %%xmm1, %%xmm0")
            __ASM_EMIT("sqrtps      %%xmm0, %%xmm0")
            __ASM_EMIT("movdqu      %%xmm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("add         $0x10, %[off]")
            __ASM_EMIT("sub         $4, %[count]")
            __ASM_EMIT("jge         5b")

            // 1x blocks
            __ASM_EMIT("6:")
            __ASM_EMIT("add         $3, %[count]")
            __ASM_EMIT("jl          8f")
            __ASM_EMIT("7:")
            __ASM_EMIT("movlps      0x00(%[src], %[off], 2), %%xmm0")
            __ASM_EMIT("mulps       %%xmm0, %%xmm0")
            __ASM_EMIT("haddps      %%xmm0, %%xmm0")
            __ASM_EMIT("sqrtss      %%xmm0, %%xmm0")
            __ASM_EMIT("movss       %%xmm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("add         $0x4, %[off]")
            __ASM_EMIT("dec         %[count]")
            __ASM_EMIT("jge         7b")

            // End
            __ASM_EMIT("8:")
            : [dst] "+r" (dst), [src] "+r" (src),
              [count] "+r" (count), [off] "=&r" (off)
            :
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5", "%xmm6", "%xmm7",
              "%xmm8", "%xmm9", "%xmm10", "%xmm12", "%xmm13", "%xmm14"
        );
    }
}


#endif /* DSP_ARCH_X86_SSE3_COMPLEX_H_ */
