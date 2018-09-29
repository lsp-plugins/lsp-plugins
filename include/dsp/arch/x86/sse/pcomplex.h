/*
 * pcomplex.h
 *
 *  Created on: 6 сент. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_SSE_PCOMPLEX_H_
#define DSP_ARCH_X86_SSE_PCOMPLEX_H_

namespace sse
{
    void pcomplex_mul2(float *dst, const float *src, size_t count)
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
            __ASM_EMIT("movups      0x00(%[dst], %[off]), %%xmm0")     /* xmm0 = ar0 ai0 ar1 ai1 */
            __ASM_EMIT("movups      0x10(%[dst], %[off]), %%xmm1")     /* xmm1 = ar2 ai2 ar3 ai3 */
            __ASM_EMIT("movups      0x00(%[src], %[off]), %%xmm4")     /* xmm4 = br0 bi0 br1 bi1 */
            __ASM_EMIT("movups      0x10(%[src], %[off]), %%xmm5")     /* xmm5 = br2 bi2 br3 bi3 */
            /* Do shuffle */
            __ASM_EMIT("movaps      %%xmm0, %%xmm2")            /* xmm2 = ar0 ai0 ar1 ai1 */
            __ASM_EMIT("movaps      %%xmm4, %%xmm6")            /* xmm6 = br0 bi0 br1 bi1 */
            __ASM_EMIT("shufps      $0x88, %%xmm1, %%xmm0")     /* xmm0 = ar0 ar1 ar2 ar3 */
            __ASM_EMIT("shufps      $0x88, %%xmm5, %%xmm4")     /* xmm4 = br0 br1 br2 br3 */
            __ASM_EMIT("shufps      $0xdd, %%xmm1, %%xmm2")     /* xmm2 = ai0 ai1 ai2 ai3 */
            __ASM_EMIT("shufps      $0xdd, %%xmm5, %%xmm6")     /* xmm6 = bi0 bi1 bi2 bi3 */
            /* Calc multiplication */
            __ASM_EMIT("movaps      %%xmm0, %%xmm1")            /* xmm1 = ar0 ar1 ar2 ar3 */
            __ASM_EMIT("mulps       %%xmm4, %%xmm0")            /* xmm0 = ar0*br0 ar1*br1 ar2*br2 ar3*br3 */
            __ASM_EMIT("mulps       %%xmm6, %%xmm1")            /* xmm1 = ar0*bi0 ar1*bi1 ar2*bi2 ar3*bi3 */
            __ASM_EMIT("mulps       %%xmm2, %%xmm6")            /* xmm6 = ai0*bi0 ai1*bi1 ai2*bi2 ai3*bi3 */
            __ASM_EMIT("mulps       %%xmm2, %%xmm4")            /* xmm4 = ai0*br0 ai1*br1 ai2*br2 ai3*br3 */
            __ASM_EMIT("addps       %%xmm4, %%xmm1")            /* xmm1 = ar[i]*bi[i] + ai[i]*br[i] = i0 i1 i2 i3 */
            __ASM_EMIT("subps       %%xmm6, %%xmm0")            /* xmm0 = ar[i]*br[i] - ai[i]*bi[i] = r0 r1 r2 r3 */
            /* Re-shuffle */
            __ASM_EMIT("movaps      %%xmm0, %%xmm2")            /* xmm2 = r0 r1 r2 r3 */
            __ASM_EMIT("unpcklps    %%xmm1, %%xmm0")            /* xmm0 = r0 i0 r1 i1 */
            __ASM_EMIT("unpckhps    %%xmm1, %%xmm2")            /* xmm2 = r2 i2 r3 i3 */
            /* Store */
            __ASM_EMIT("movups       %%xmm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("movups       %%xmm2, 0x10(%[dst], %[off])")
            /* Repeat loop */
            __ASM_EMIT("add         $0x20, %[off]")
            __ASM_EMIT("sub         $4, %[count]")
            __ASM_EMIT("jae         2b")

            __ASM_EMIT("3:")
            __ASM_EMIT("add         $4, %[count]")
            __ASM_EMIT("jle         5f")
            /* Process scalar data */
            __ASM_EMIT("4:")
            /* Load */
            __ASM_EMIT("movss       0x00(%[dst], %[off]), %%xmm0")     /* xmm0 = ar */
            __ASM_EMIT("movss       0x04(%[dst], %[off]), %%xmm2")     /* xmm2 = ai */
            __ASM_EMIT("movss       0x00(%[src], %[off]), %%xmm4")     /* xmm4 = br */
            __ASM_EMIT("movss       0x04(%[src], %[off]), %%xmm6")     /* xmm6 = bi */
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

            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r" (dst), [src] "r" (src)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm4", "%xmm5", "%xmm6"
        );
    }

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
            __ASM_EMIT("movups      0x00(%[src1], %[off]), %%xmm0")     /* xmm0 = ar0 ai0 ar1 ai1 */
            __ASM_EMIT("movups      0x10(%[src1], %[off]), %%xmm1")     /* xmm1 = ar2 ai2 ar3 ai3 */
            __ASM_EMIT("movups      0x00(%[src2], %[off]), %%xmm4")     /* xmm4 = br0 bi0 br1 bi1 */
            __ASM_EMIT("movups      0x10(%[src2], %[off]), %%xmm5")     /* xmm5 = br2 bi2 br3 bi3 */
            /* Do shuffle */
            __ASM_EMIT("movaps      %%xmm0, %%xmm2")            /* xmm2 = ar0 ai0 ar1 ai1 */
            __ASM_EMIT("movaps      %%xmm4, %%xmm6")            /* xmm6 = br0 bi0 br1 bi1 */
            __ASM_EMIT("shufps      $0x88, %%xmm1, %%xmm0")     /* xmm0 = ar0 ar1 ar2 ar3 */
            __ASM_EMIT("shufps      $0x88, %%xmm5, %%xmm4")     /* xmm4 = br0 br1 br2 br3 */
            __ASM_EMIT("shufps      $0xdd, %%xmm1, %%xmm2")     /* xmm2 = ai0 ai1 ai2 ai3 */
            __ASM_EMIT("shufps      $0xdd, %%xmm5, %%xmm6")     /* xmm6 = bi0 bi1 bi2 bi3 */
            /* Calc multiplication */
            __ASM_EMIT("movaps      %%xmm0, %%xmm1")            /* xmm1 = ar0 ar1 ar2 ar3 */
            __ASM_EMIT("mulps       %%xmm4, %%xmm0")            /* xmm0 = ar0*br0 ar1*br1 ar2*br2 ar3*br3 */
            __ASM_EMIT("mulps       %%xmm6, %%xmm1")            /* xmm1 = ar0*bi0 ar1*bi1 ar2*bi2 ar3*bi3 */
            __ASM_EMIT("mulps       %%xmm2, %%xmm6")            /* xmm6 = ai0*bi0 ai1*bi1 ai2*bi2 ai3*bi3 */
            __ASM_EMIT("mulps       %%xmm2, %%xmm4")            /* xmm4 = ai0*br0 ai1*br1 ai2*br2 ai3*br3 */
            __ASM_EMIT("addps       %%xmm4, %%xmm1")            /* xmm1 = ar[i]*bi[i] + ai[i]*br[i] = i0 i1 i2 i3 */
            __ASM_EMIT("subps       %%xmm6, %%xmm0")            /* xmm0 = ar[i]*br[i] - ai[i]*bi[i] = r0 r1 r2 r3 */
            /* Re-shuffle */
            __ASM_EMIT("movaps      %%xmm0, %%xmm2")            /* xmm2 = r0 r1 r2 r3 */
            __ASM_EMIT("unpcklps    %%xmm1, %%xmm0")            /* xmm0 = r0 i0 r1 i1 */
            __ASM_EMIT("unpckhps    %%xmm1, %%xmm2")            /* xmm2 = r2 i2 r3 i3 */
            /* Store */
            __ASM_EMIT("movups       %%xmm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("movups       %%xmm2, 0x10(%[dst], %[off])")
            /* Repeat loop */
            __ASM_EMIT("add         $0x20, %[off]")
            __ASM_EMIT("sub         $4, %[count]")
            __ASM_EMIT("jae         2b")

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

            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r" (dst), [src1] "r" (src1), [src2] "r" (src2)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm4", "%xmm5", "%xmm6"
        );
    }

    void pcomplex_r2c(float *dst, const float *src, size_t count)
    {
        if (dst == src)
        {
            // Need to perform reverse copy
            #define real_to_complex_core(MV_DST, MV_SRC) \
                __ASM_EMIT("cmp         $16, %[count]") \
                __ASM_EMIT("jb          2f") \
                /* Do with 6x blocks */ \
                __ASM_EMIT(".align 16") \
                __ASM_EMIT("1:") \
                __ASM_EMIT("sub         $16, %[count]") \
                __ASM_EMIT("sub         $0x40, %[src]") \
                __ASM_EMIT("sub         $0x80, %[dst]") \
                __ASM_EMIT(MV_SRC "     0x00(%[src]), %%xmm0")  /* xmm0  = r0  r1  r2  r3  */ \
                __ASM_EMIT(MV_SRC "     0x10(%[src]), %%xmm1")  /* xmm1  = r4  r5  r6  r7  */ \
                __ASM_EMIT(MV_SRC "     0x20(%[src]), %%xmm2")  /* xmm2  = r8  r9  r10 r11 */ \
                __ASM_EMIT(MV_SRC "     0x30(%[src]), %%xmm3")  /* xmm3  = r12 r13 r14 r15 */ \
                /* Do conversion */ \
                __ASM_EMIT("movaps      %%xmm0, %%xmm4") \
                __ASM_EMIT("movaps      %%xmm1, %%xmm5") \
                __ASM_EMIT("unpcklps    %%xmm6, %%xmm0") \
                __ASM_EMIT("unpcklps    %%xmm7, %%xmm1") \
                __ASM_EMIT("unpckhps    %%xmm6, %%xmm4") \
                __ASM_EMIT("unpckhps    %%xmm7, %%xmm5") \
                __ASM_EMIT(MV_DST "     %%xmm0, 0x00(%[dst])") \
                __ASM_EMIT(MV_DST "     %%xmm4, 0x10(%[dst])") \
                __ASM_EMIT(MV_DST "     %%xmm1, 0x20(%[dst])") \
                __ASM_EMIT(MV_DST "     %%xmm5, 0x30(%[dst])") \
                __ASM_EMIT("movaps      %%xmm2, %%xmm4") \
                __ASM_EMIT("movaps      %%xmm3, %%xmm5") \
                __ASM_EMIT("unpcklps    %%xmm6, %%xmm2") \
                __ASM_EMIT("unpcklps    %%xmm7, %%xmm3") \
                __ASM_EMIT("unpckhps    %%xmm6, %%xmm4") \
                __ASM_EMIT("unpckhps    %%xmm7, %%xmm5") \
                __ASM_EMIT(MV_DST "     %%xmm2, 0x40(%[dst])") \
                __ASM_EMIT(MV_DST "     %%xmm4, 0x50(%[dst])") \
                __ASM_EMIT(MV_DST "     %%xmm3, 0x60(%[dst])") \
                __ASM_EMIT(MV_DST "     %%xmm5, 0x70(%[dst])") \
                /* Repeat loop */ \
                __ASM_EMIT("cmp         $16, %[count]") \
                __ASM_EMIT("jae         1b") \
                /* 4x iterations */ \
                __ASM_EMIT("2:") \
                __ASM_EMIT("cmp         $4, %[count]") \
                __ASM_EMIT("jb          4f") \
                \
                __ASM_EMIT("3:") \
                __ASM_EMIT("sub         $4, %[count]") \
                __ASM_EMIT("sub         $0x10, %[src]") \
                __ASM_EMIT("sub         $0x20, %[dst]") \
                __ASM_EMIT(MV_SRC "     0x00(%[src]), %%xmm0") \
                __ASM_EMIT("movaps      %%xmm0, %%xmm4") \
                __ASM_EMIT("unpcklps    %%xmm6, %%xmm0") \
                __ASM_EMIT("unpckhps    %%xmm7, %%xmm4") \
                __ASM_EMIT(MV_DST "     %%xmm0, 0x00(%[dst])") \
                __ASM_EMIT(MV_DST "     %%xmm4, 0x10(%[dst])") \
                \
                __ASM_EMIT("cmp         $4, %[count]") \
                __ASM_EMIT("jae         3b") \
                __ASM_EMIT("jmp         4f")


            ARCH_X86_ASM
            (
                __ASM_EMIT("xorps       %%xmm6, %%xmm6")
                __ASM_EMIT("xorps       %%xmm7, %%xmm7")
                __ASM_EMIT("lea         (%[src], %[count], 4), %[src]")
                __ASM_EMIT("lea         (%[dst], %[count], 8), %[dst]")

                /* Do block processing */
                __ASM_EMIT("test $0x0f, %[dst]")
                __ASM_EMIT("jnz 110f")
                    __ASM_EMIT("test $0x0f, %[src]")
                    __ASM_EMIT("jnz 101f")
                        real_to_complex_core("movaps", "movaps")
                    __ASM_EMIT("101:")
                        real_to_complex_core("movaps", "movups")

                __ASM_EMIT("110:")
                    __ASM_EMIT("test $0x0f, %[src]")
                    __ASM_EMIT("jnz 111f")
                        real_to_complex_core("movups", "movaps")
                    __ASM_EMIT("111:")
                        real_to_complex_core("movups", "movups")

                // 1x iterations
                __ASM_EMIT("4:")
                __ASM_EMIT("test        %[count], %[count]")
                __ASM_EMIT("jz          6f")

                __ASM_EMIT("5:")
                __ASM_EMIT("sub         $0x4, %[src]")
                __ASM_EMIT("sub         $0x8, %[dst]")
                __ASM_EMIT("movss       0x00(%[src]), %%xmm0")
                __ASM_EMIT("movlps      %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("dec         %[count]")
                __ASM_EMIT("jnz         5b")

                // End of routine
                __ASM_EMIT("6:")

                : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
                :
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );

            #undef real_to_complex_core
        }
        else
        {
            #define real_to_complex_core(MV_DST, MV_SRC) \
                __ASM_EMIT("cmp         $16, %[count]") \
                __ASM_EMIT("jb          2f") \
                /* Do with 6x blocks */ \
                __ASM_EMIT(".align 16") \
                __ASM_EMIT("1:") \
                __ASM_EMIT(MV_SRC "     0x00(%[src]), %%xmm0")  /* xmm0  = r0  r1  r2  r3  */ \
                __ASM_EMIT(MV_SRC "     0x10(%[src]), %%xmm1")  /* xmm1  = r4  r5  r6  r7  */ \
                __ASM_EMIT(MV_SRC "     0x20(%[src]), %%xmm2")  /* xmm2  = r8  r9  r10 r11 */ \
                __ASM_EMIT(MV_SRC "     0x30(%[src]), %%xmm3")  /* xmm3  = r12 r13 r14 r15 */ \
                /* Do conversion */ \
                __ASM_EMIT("movaps      %%xmm0, %%xmm4") \
                __ASM_EMIT("movaps      %%xmm1, %%xmm5") \
                __ASM_EMIT("unpcklps    %%xmm6, %%xmm0") \
                __ASM_EMIT("unpcklps    %%xmm7, %%xmm1") \
                __ASM_EMIT("unpckhps    %%xmm6, %%xmm4") \
                __ASM_EMIT("unpckhps    %%xmm7, %%xmm5") \
                __ASM_EMIT(MV_DST "     %%xmm0, 0x00(%[dst])") \
                __ASM_EMIT(MV_DST "     %%xmm4, 0x10(%[dst])") \
                __ASM_EMIT(MV_DST "     %%xmm1, 0x20(%[dst])") \
                __ASM_EMIT(MV_DST "     %%xmm5, 0x30(%[dst])") \
                __ASM_EMIT("movaps      %%xmm2, %%xmm4") \
                __ASM_EMIT("movaps      %%xmm3, %%xmm5") \
                __ASM_EMIT("unpcklps    %%xmm6, %%xmm2") \
                __ASM_EMIT("unpcklps    %%xmm7, %%xmm3") \
                __ASM_EMIT("unpckhps    %%xmm6, %%xmm4") \
                __ASM_EMIT("unpckhps    %%xmm7, %%xmm5") \
                __ASM_EMIT(MV_DST "     %%xmm2, 0x40(%[dst])") \
                __ASM_EMIT(MV_DST "     %%xmm4, 0x50(%[dst])") \
                __ASM_EMIT(MV_DST "     %%xmm3, 0x60(%[dst])") \
                __ASM_EMIT(MV_DST "     %%xmm5, 0x70(%[dst])") \
                /* Repeat loop */ \
                __ASM_EMIT("sub         $16, %[count]") \
                __ASM_EMIT("add         $0x40, %[src]") \
                __ASM_EMIT("add         $0x80, %[dst]") \
                __ASM_EMIT("cmp         $16, %[count]") \
                __ASM_EMIT("jae         1b") \
                /* 4x iterations */ \
                __ASM_EMIT("2:") \
                __ASM_EMIT("cmp         $4, %[count]") \
                __ASM_EMIT("jb          4f") \
                \
                __ASM_EMIT("3:") \
                __ASM_EMIT(MV_SRC "     0x00(%[src]), %%xmm0") \
                __ASM_EMIT("movaps      %%xmm0, %%xmm4") \
                __ASM_EMIT("unpcklps    %%xmm6, %%xmm0") \
                __ASM_EMIT("unpckhps    %%xmm7, %%xmm4") \
                __ASM_EMIT(MV_DST "     %%xmm0, 0x00(%[dst])") \
                __ASM_EMIT(MV_DST "     %%xmm4, 0x10(%[dst])") \
                \
                __ASM_EMIT("sub         $4, %[count]") \
                __ASM_EMIT("add         $0x10, %[src]") \
                __ASM_EMIT("add         $0x20, %[dst]") \
                __ASM_EMIT("cmp         $4, %[count]") \
                __ASM_EMIT("jae         3b") \
                __ASM_EMIT("jmp         4f")


            ARCH_X86_ASM
            (
                __ASM_EMIT("xorps       %%xmm6, %%xmm6")
                __ASM_EMIT("xorps       %%xmm7, %%xmm7")

                /* Do block processing */
                __ASM_EMIT("test $0x0f, %[dst]")
                __ASM_EMIT("jnz 110f")
                    __ASM_EMIT("test $0x0f, %[src]")
                    __ASM_EMIT("jnz 101f")
                        real_to_complex_core("movaps", "movaps")
                    __ASM_EMIT("101:")
                        real_to_complex_core("movaps", "movups")

                __ASM_EMIT("110:")
                    __ASM_EMIT("test $0x0f, %[src]")
                    __ASM_EMIT("jnz 111f")
                        real_to_complex_core("movups", "movaps")
                    __ASM_EMIT("111:")
                        real_to_complex_core("movups", "movups")

                // 1x iterations
                __ASM_EMIT("4:")
                __ASM_EMIT("test        %[count], %[count]")
                __ASM_EMIT("jz          6f")

                __ASM_EMIT("5:")
                __ASM_EMIT("movss       0x00(%[src]), %%xmm0")
                __ASM_EMIT("movlps      %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add         $0x4, %[src]")
                __ASM_EMIT("add         $0x8, %[dst]")
                __ASM_EMIT("dec         %[count]")
                __ASM_EMIT("jnz         5b")

                // End of routine
                __ASM_EMIT("6:")

                : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
                :
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );

            #undef real_to_complex_core
        }
    }

    void pcomplex_c2r(float *dst, const float *src, size_t count)
    {
        #define complex_to_real_core(MV_DST, MV_SRC) \
            __ASM_EMIT("cmp         $16, %[count]") \
            __ASM_EMIT("jb          2f") \
            /* Do with 16x blocks */ \
            __ASM_EMIT(".align 16") \
            __ASM_EMIT("1:") \
            /* Load data */ \
            __ASM_EMIT(MV_SRC "     0x00(%[src]), %%xmm0")  /* xmm0  = r0 i0 r1 i1 */ \
            __ASM_EMIT(MV_SRC "     0x10(%[src]), %%xmm1")  /* xmm1  = r2 i2 r3 i3 */ \
            __ASM_EMIT(MV_SRC "     0x20(%[src]), %%xmm2")  \
            __ASM_EMIT(MV_SRC "     0x30(%[src]), %%xmm3")  \
            __ASM_EMIT(MV_SRC "     0x40(%[src]), %%xmm4")  \
            __ASM_EMIT(MV_SRC "     0x50(%[src]), %%xmm5")  \
            __ASM_EMIT(MV_SRC "     0x60(%[src]), %%xmm6")  \
            __ASM_EMIT(MV_SRC "     0x70(%[src]), %%xmm7")  \
            /* Pack data */ \
            __ASM_EMIT("shufps      $0x88, %%xmm1, %%xmm0") /* xmm0  = r0 r1 r2 r3 */ \
            __ASM_EMIT("shufps      $0x88, %%xmm3, %%xmm2") \
            __ASM_EMIT("shufps      $0x88, %%xmm5, %%xmm4") \
            __ASM_EMIT("shufps      $0x88, %%xmm7, %%xmm6") \
            /* Store data */ \
            __ASM_EMIT(MV_DST "     %%xmm0, 0x00(%[dst])") \
            __ASM_EMIT(MV_DST "     %%xmm2, 0x10(%[dst])") \
            __ASM_EMIT(MV_DST "     %%xmm4, 0x20(%[dst])") \
            __ASM_EMIT(MV_DST "     %%xmm6, 0x30(%[dst])") \
            /* Repeat loop */ \
            __ASM_EMIT("sub         $16, %[count]") \
            __ASM_EMIT("add         $0x80, %[src]") \
            __ASM_EMIT("add         $0x40, %[dst]") \
            __ASM_EMIT("cmp         $16, %[count]") \
            __ASM_EMIT("jae         1b") \
            /* 4x iterations */ \
            __ASM_EMIT("2:") \
            __ASM_EMIT("cmp         $4, %[count]") \
            __ASM_EMIT("jb          4f") \
            \
            __ASM_EMIT("3:") \
            __ASM_EMIT(MV_SRC "     0x00(%[src]), %%xmm0") \
            __ASM_EMIT(MV_SRC "     0x10(%[src]), %%xmm1") \
            __ASM_EMIT("shufps      $0x88, %%xmm1, %%xmm0") \
            __ASM_EMIT(MV_DST "     %%xmm0, 0x00(%[dst])") \
            \
            __ASM_EMIT("sub         $4, %[count]") \
            __ASM_EMIT("add         $0x20, %[src]") \
            __ASM_EMIT("add         $0x10, %[dst]") \
            __ASM_EMIT("cmp         $4, %[count]") \
            __ASM_EMIT("jae         3b") \
            __ASM_EMIT("jmp         4f")


        ARCH_X86_ASM
        (
            /* Do block processing */
            __ASM_EMIT("test $0x0f, %[dst]")
            __ASM_EMIT("jnz 110f")
                __ASM_EMIT("test $0x0f, %[src]")
                __ASM_EMIT("jnz 101f")
                    complex_to_real_core("movaps", "movaps")
                __ASM_EMIT("101:")
                    complex_to_real_core("movaps", "movups")

            __ASM_EMIT("110:")
                __ASM_EMIT("test $0x0f, %[src]")
                __ASM_EMIT("jnz 111f")
                    complex_to_real_core("movups", "movaps")
                __ASM_EMIT("111:")
                    complex_to_real_core("movups", "movups")

            // 1x iterations
            __ASM_EMIT("4:")
            __ASM_EMIT("test        %[count], %[count]")
            __ASM_EMIT("jz          6f")

            __ASM_EMIT("5:")
            __ASM_EMIT("movss       0x00(%[src]), %%xmm0")
            __ASM_EMIT("movss       %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("add         $0x8, %[src]")
            __ASM_EMIT("add         $0x4, %[dst]")
            __ASM_EMIT("dec         %[count]")
            __ASM_EMIT("jnz         5b")

            // End of routine
            __ASM_EMIT("6:")

            : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
            :
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );

        #undef complex_to_real_core
    }

    #define complex_rop_core(OP) \
        __ASM_EMIT("xor         %[off], %[off]") \
        __ASM_EMIT("sub         $8, %[count]") \
        __ASM_EMIT("jb          2f") \
        /* Do with 8x blocks */ \
        __ASM_EMIT("1:") \
        __ASM_EMIT("movups      0x00(%[src], %[off]), %%xmm4") \
        __ASM_EMIT("movups      0x10(%[src], %[off]), %%xmm6") \
        __ASM_EMIT("movaps      %%xmm4, %%xmm5") \
        __ASM_EMIT("movaps      %%xmm6, %%xmm7") \
        __ASM_EMIT("xorps       %%xmm2, %%xmm2") \
        __ASM_EMIT("xorps       %%xmm3, %%xmm3") \
        __ASM_EMIT("unpcklps    %%xmm2, %%xmm4") \
        __ASM_EMIT("unpckhps    %%xmm3, %%xmm5") \
        __ASM_EMIT("unpcklps    %%xmm2, %%xmm6") \
        __ASM_EMIT("unpckhps    %%xmm3, %%xmm7") \
        __ASM_EMIT("movups      0x00(%[dst], %[off], 2), %%xmm0") \
        __ASM_EMIT("movups      0x10(%[dst], %[off], 2), %%xmm1") \
        __ASM_EMIT("movups      0x20(%[dst], %[off], 2), %%xmm2") \
        __ASM_EMIT("movups      0x30(%[dst], %[off], 2), %%xmm3") \
        __ASM_EMIT(OP "ps       %%xmm4, %%xmm0") \
        __ASM_EMIT(OP "ps       %%xmm5, %%xmm1") \
        __ASM_EMIT(OP "ps       %%xmm6, %%xmm2") \
        __ASM_EMIT(OP "ps       %%xmm7, %%xmm3") \
        __ASM_EMIT("movups      %%xmm0, 0x00(%[dst], %[off], 2)") \
        __ASM_EMIT("movups      %%xmm1, 0x10(%[dst], %[off], 2)") \
        __ASM_EMIT("movups      %%xmm2, 0x20(%[dst], %[off], 2)") \
        __ASM_EMIT("movups      %%xmm3, 0x30(%[dst], %[off], 2)") \
        __ASM_EMIT("add         $0x20, %[off]") \
        __ASM_EMIT("sub         $8, %[count]") \
        __ASM_EMIT("jae         1b") \
        /* 4x block */ \
        __ASM_EMIT("2:") \
        __ASM_EMIT("add         $4, %[count]") \
        __ASM_EMIT("jl          4f") \
        __ASM_EMIT("movups      0x00(%[src], %[off]), %%xmm4") \
        __ASM_EMIT("movups      0x00(%[dst], %[off], 2), %%xmm0") \
        __ASM_EMIT("movups      0x10(%[dst], %[off], 2), %%xmm1") \
        __ASM_EMIT("movaps      %%xmm4, %%xmm5") \
        __ASM_EMIT("movaps      %%xmm6, %%xmm7") \
        __ASM_EMIT("xorps       %%xmm2, %%xmm2") \
        __ASM_EMIT("xorps       %%xmm3, %%xmm3") \
        __ASM_EMIT("unpcklps    %%xmm2, %%xmm4") \
        __ASM_EMIT("unpckhps    %%xmm3, %%xmm5") \
        __ASM_EMIT(OP "ps       %%xmm4, %%xmm0") \
        __ASM_EMIT(OP "ps       %%xmm5, %%xmm1") \
        __ASM_EMIT("movups      %%xmm0, 0x00(%[dst], %[off], 2)") \
        __ASM_EMIT("movups      %%xmm1, 0x10(%[dst], %[off], 2)") \
        __ASM_EMIT("sub         $4, %[count]") \
        __ASM_EMIT("add         $0x10, %[off]") \
        \
        /* 1x blocks */ \
        __ASM_EMIT("4:") \
        __ASM_EMIT("add         $3, %[count]") \
        __ASM_EMIT("jl          6f") \
        __ASM_EMIT("3:") \
        __ASM_EMIT("movss       0x00(%[src], %[off]), %%xmm2") \
        __ASM_EMIT("movss       0x00(%[dst], %[off], 2), %%xmm0") \
        __ASM_EMIT(OP "ss       %%xmm2, %%xmm0") \
        __ASM_EMIT("movss       %%xmm0, 0x00(%[dst], %[off], 2)") \
        __ASM_EMIT("add         $0x04, %[off]") \
        __ASM_EMIT("dec         %[count]") \
        __ASM_EMIT("jge         3b") \
        __ASM_EMIT("6:")

    void pcomplex_add_r(float *dst, const float *src, size_t count)
    {
        IF_ARCH_X86(size_t off);

        ARCH_X86_ASM
        (
            complex_rop_core("add")
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r" (dst), [src] "r" (src)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );

    }

    #undef complex_rop_core

    void pcomplex_mod(float *dst, const float *src, size_t count)
    {
        size_t off;

        ARCH_X86_ASM
        (
            __ASM_EMIT("xor         %[off], %[off]")
            __ASM_EMIT("sub         $12, %[count]")
            __ASM_EMIT("jb          2f")

            // 12x blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("movups      0x00(%[src], %[off], 2), %%xmm0")
            __ASM_EMIT("movups      0x10(%[src], %[off], 2), %%xmm5")
            __ASM_EMIT("movups      0x20(%[src], %[off], 2), %%xmm1")
            __ASM_EMIT("movups      0x30(%[src], %[off], 2), %%xmm6")
            __ASM_EMIT("movups      0x40(%[src], %[off], 2), %%xmm2")
            __ASM_EMIT("movups      0x50(%[src], %[off], 2), %%xmm7")

            __ASM_EMIT("movaps      %%xmm0, %%xmm3")
            __ASM_EMIT("movaps      %%xmm1, %%xmm4")
            __ASM_EMIT("shufps      $0x88, %%xmm5, %%xmm0") /* xmm0 = re[0..3] */
            __ASM_EMIT("shufps      $0xdd, %%xmm5, %%xmm3") /* xmm3 = im[0..3] */
            __ASM_EMIT("movaps      %%xmm2, %%xmm5")
            __ASM_EMIT("shufps      $0x88, %%xmm6, %%xmm1") /* xmm1 = re[4..7] */
            __ASM_EMIT("shufps      $0xdd, %%xmm6, %%xmm4") /* xmm4 = im[4..7] */
            __ASM_EMIT("shufps      $0x88, %%xmm7, %%xmm2") /* xmm2 = re[8..11] */
            __ASM_EMIT("shufps      $0xdd, %%xmm7, %%xmm5") /* xmm5 = im[8..11] */

            __ASM_EMIT("mulps       %%xmm0, %%xmm0")
            __ASM_EMIT("mulps       %%xmm1, %%xmm1")
            __ASM_EMIT("mulps       %%xmm2, %%xmm2")
            __ASM_EMIT("mulps       %%xmm3, %%xmm3")
            __ASM_EMIT("mulps       %%xmm4, %%xmm4")
            __ASM_EMIT("mulps       %%xmm5, %%xmm5")
            __ASM_EMIT("addps       %%xmm3, %%xmm0")
            __ASM_EMIT("addps       %%xmm4, %%xmm1")
            __ASM_EMIT("addps       %%xmm5, %%xmm2")
            __ASM_EMIT("sqrtps      %%xmm0, %%xmm0")
            __ASM_EMIT("sqrtps      %%xmm1, %%xmm1")
            __ASM_EMIT("sqrtps      %%xmm2, %%xmm2")
            __ASM_EMIT("movups      %%xmm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("movups      %%xmm1, 0x10(%[dst], %[off])")
            __ASM_EMIT("movups      %%xmm2, 0x20(%[dst], %[off])")
            __ASM_EMIT("add         $0x30, %[off]")
            __ASM_EMIT("sub         $12, %[count]")
            __ASM_EMIT("jae         1b")

            // 4x blocks
            __ASM_EMIT("2:")
            __ASM_EMIT("add         $8, %[count]")
            __ASM_EMIT("jl          4f")
            __ASM_EMIT("3:")
            __ASM_EMIT("movups      0x00(%[src], %[off], 2), %%xmm0")
            __ASM_EMIT("movups      0x10(%[src], %[off], 2), %%xmm5")
            __ASM_EMIT("movaps      %%xmm0, %%xmm3")
            __ASM_EMIT("shufps      $0x88, %%xmm5, %%xmm0") /* xmm0 = re[0..3] */
            __ASM_EMIT("shufps      $0xdd, %%xmm5, %%xmm3") /* xmm3 = im[0..3] */
            __ASM_EMIT("mulps       %%xmm0, %%xmm0")
            __ASM_EMIT("mulps       %%xmm3, %%xmm3")
            __ASM_EMIT("addps       %%xmm3, %%xmm0")
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
            __ASM_EMIT("movss       0x00(%[src], %[off], 2), %%xmm0")
            __ASM_EMIT("movss       0x04(%[src], %[off], 2), %%xmm3")
            __ASM_EMIT("mulss       %%xmm0, %%xmm0")
            __ASM_EMIT("mulss       %%xmm3, %%xmm3")
            __ASM_EMIT("addss       %%xmm3, %%xmm0")
            __ASM_EMIT("sqrtss      %%xmm0, %%xmm0")
            __ASM_EMIT("movss       %%xmm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("add         $0x4, %[off]")
            __ASM_EMIT("dec         %[count]")
            __ASM_EMIT("jge         5b")

            // End
            __ASM_EMIT("6:")
            : [count] "+r" (count), [off] "=&r" (off)
            : [dst] "r" (dst), [src] "r" (src)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void pcomplex_rcp2(float *dst, const float *src, size_t count)
    {
        ARCH_X86_ASM (
            /* Do block processing */
            __ASM_EMIT("test        %[count], %[count]")
            __ASM_EMIT("jz          50f")

            __ASM_EMIT("sub         $8, %[count]")              /* count -= 8 */
            __ASM_EMIT("jb          2f")
            __ASM_EMIT("1:")
            /* Perform 8x RCP */
            __ASM_EMIT("movups      0x00(%[src]),  %%xmm0")     /* xmm0  = r0 i0 r1 i1 */
            __ASM_EMIT("movups      0x10(%[src]),  %%xmm4")     /* xmm4  = r2 i2 r3 i3 */
            __ASM_EMIT("movups      0x20(%[src]),  %%xmm2")     /* xmm2  = r4 i4 r5 i5 */
            __ASM_EMIT("movups      0x30(%[src]),  %%xmm5")     /* xmm5  = r6 i6 r7 i7 */
            /* Do shuffle */
            __ASM_EMIT("movaps      %%xmm0, %%xmm1")            /* xmm1  = r0 i0 r1 i1 */
            __ASM_EMIT("movaps      %%xmm2, %%xmm3")            /* xmm3  = r4 i4 r5 i5 */
            __ASM_EMIT("shufps      $0x88, %%xmm4, %%xmm0")     /* xmm0  = r0 r1 r2 r3 */
            __ASM_EMIT("shufps      $0x88, %%xmm5, %%xmm2")     /* xmm2  = r4 r5 r6 r7 */
            __ASM_EMIT("shufps      $0xdd, %%xmm4, %%xmm1")     /* xmm1  = i0 i1 i2 i3 */
            __ASM_EMIT("shufps      $0xdd, %%xmm5, %%xmm3")     /* xmm3  = i4 i5 i6 i7 */
            /* Perform operations */
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
            /* Do shuffle back */
            __ASM_EMIT("movaps      %%xmm0, %%xmm4")            /* xmm4 = r0' r1' r2' r3' */
            __ASM_EMIT("movaps      %%xmm2, %%xmm5")            /* xmm5 = r4' r5' r6' r7' */
            __ASM_EMIT("unpcklps    %%xmm1, %%xmm0")            /* xmm0 = r0' i0' r1' i1' */
            __ASM_EMIT("unpcklps    %%xmm3, %%xmm2")            /* xmm2 = r4' i4' r5' i5' */
            __ASM_EMIT("unpckhps    %%xmm1, %%xmm4")            /* xmm4 = r2' i2' r3' i3' */
            __ASM_EMIT("unpckhps    %%xmm3, %%xmm5")            /* xmm5 = r6' i6' r7' i7' */

            __ASM_EMIT("movups      %%xmm0, 0x00(%[dst])")      /* dst[0]  = r0' i0' r1' i1' */
            __ASM_EMIT("movups      %%xmm4, 0x10(%[dst])")      /* dst[1]  = r2' i2' r3' i3' */
            __ASM_EMIT("movups      %%xmm2, 0x20(%[dst])")      /* dst[2]  = r4' i4' r5' i5' */
            __ASM_EMIT("movups      %%xmm5, 0x30(%[dst])")      /* dst[3]  = r6' i6' r7' i7' */
            __ASM_EMIT("add         $0x40, %[src]")             /* src += 16 */
            __ASM_EMIT("add         $0x40, %[dst]")             /* dst += 16 */
            __ASM_EMIT("sub         $8, %[count]")              /* count -= 8 */
            __ASM_EMIT("jae         1b")
            /* Next step */
            __ASM_EMIT("2:")
            __ASM_EMIT("add         $4, %[count]")              /* count += 4 */
            __ASM_EMIT("jl          10f")
            /* Perform 4x RCP */
            __ASM_EMIT("movups      0x00(%[src]),  %%xmm0")     /* xmm0  = r0 i0 r1 i1 */
            __ASM_EMIT("movups      0x10(%[src]),  %%xmm4")     /* xmm4  = r2 i2 r3 i3 */
            /* Do shuffle */
            __ASM_EMIT("movaps      %%xmm0, %%xmm1")            /* xmm1  = r0 i0 r1 i1 */
            __ASM_EMIT("shufps      $0x88, %%xmm4, %%xmm0")     /* xmm0  = r0 r1 r2 r3 */
            __ASM_EMIT("shufps      $0xdd, %%xmm4, %%xmm1")     /* xmm1  = i0 i1 i2 i3 */
            /* Perform operations */
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
            /* Do shuffle back */
            __ASM_EMIT("movaps      %%xmm0, %%xmm4")            /* xmm4 = r0' r1' r2' r3' */
            __ASM_EMIT("unpcklps    %%xmm1, %%xmm0")            /* xmm0 = r0' i0' r1' i1' */
            __ASM_EMIT("unpckhps    %%xmm1, %%xmm4")            /* xmm4 = r2' i2' r3' i3' */

            __ASM_EMIT("movups      %%xmm0, 0x00(%[dst])")      /* dst[0]  = r0' i0' r1' i1' */
            __ASM_EMIT("movups      %%xmm4, 0x10(%[dst])")      /* dst[1]  = r2' i2' r3' i3'  */
            __ASM_EMIT("add         $0x20, %[src]")             /* src += 8 */
            __ASM_EMIT("add         $0x20, %[dst]")             /* dst += 8 */
            __ASM_EMIT("sub         $4, %[count]")              /* count -= 4 */

            /* 1x RCP */
            __ASM_EMIT("10:")
            __ASM_EMIT("add         $4, %[count]")              /* count += 4 */
            __ASM_EMIT("jle         50f")
            __ASM_EMIT("40:")
            __ASM_EMIT("movss       0x00(%[src]),  %%xmm0")     /* xmm0  = r0 */
            __ASM_EMIT("movss       0x04(%[src]),  %%xmm1")     /* xmm1  = i0 */
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
            __ASM_EMIT("movss       %%xmm0, 0x00(%[dst])")      /* dst[0]  = r0' */
            __ASM_EMIT("movss       %%xmm1, 0x04(%[dst])")      /* dst[1]  = i0' */
            __ASM_EMIT("add         $0x08, %[src]")             /* src += 2 */
            __ASM_EMIT("add         $0x08, %[dst]")             /* dst += 2 */
            __ASM_EMIT("dec         %[count]")                  /* count-- */
            __ASM_EMIT("jnz         40b")

            __ASM_EMIT("50:")
            : [dst] "+r" (dst), [src] "+r" (src),
              [count] "+r" (count)
            : [X_ONE] "m" (ONE),
              [X_ISIGN] "m" (X_ISIGN)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void pcomplex_rcp1(float *dst, size_t count)
    {
        ARCH_X86_ASM (
            /* Do block processing */
            __ASM_EMIT("test        %[count], %[count]")
            __ASM_EMIT("jz          50f")

            __ASM_EMIT("sub         $8, %[count]")              /* count -= 8 */
            __ASM_EMIT("jb          2f")
            __ASM_EMIT("1:")
            /* Perform 8x RCP */
            __ASM_EMIT("movups      0x00(%[dst]),  %%xmm0")     /* xmm0  = r0 i0 r1 i1 */
            __ASM_EMIT("movups      0x10(%[dst]),  %%xmm4")     /* xmm4  = r2 i2 r3 i3 */
            __ASM_EMIT("movups      0x20(%[dst]),  %%xmm2")     /* xmm2  = r4 i4 r5 i5 */
            __ASM_EMIT("movups      0x30(%[dst]),  %%xmm5")     /* xmm5  = r6 i6 r7 i7 */
            /* Do shuffle */
            __ASM_EMIT("movaps      %%xmm0, %%xmm1")            /* xmm1  = r0 i0 r1 i1 */
            __ASM_EMIT("movaps      %%xmm2, %%xmm3")            /* xmm3  = r4 i4 r5 i5 */
            __ASM_EMIT("shufps      $0x88, %%xmm4, %%xmm0")     /* xmm0  = r0 r1 r2 r3 */
            __ASM_EMIT("shufps      $0x88, %%xmm5, %%xmm2")     /* xmm2  = r4 r5 r6 r7 */
            __ASM_EMIT("shufps      $0xdd, %%xmm4, %%xmm1")     /* xmm1  = i0 i1 i2 i3 */
            __ASM_EMIT("shufps      $0xdd, %%xmm5, %%xmm3")     /* xmm3  = i4 i5 i6 i7 */
            /* Perform operations */
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
            /* Do shuffle back */
            __ASM_EMIT("movaps      %%xmm0, %%xmm4")            /* xmm4 = r0' r1' r2' r3' */
            __ASM_EMIT("movaps      %%xmm2, %%xmm5")            /* xmm5 = r4' r5' r6' r7' */
            __ASM_EMIT("unpcklps    %%xmm1, %%xmm0")            /* xmm0 = r0' i0' r1' i1' */
            __ASM_EMIT("unpcklps    %%xmm3, %%xmm2")            /* xmm2 = r4' i4' r5' i5' */
            __ASM_EMIT("unpckhps    %%xmm1, %%xmm4")            /* xmm4 = r2' i2' r3' i3' */
            __ASM_EMIT("unpckhps    %%xmm3, %%xmm5")            /* xmm5 = r6' i6' r7' i7' */

            __ASM_EMIT("movups      %%xmm0, 0x00(%[dst])")      /* dst[0]  = r0' i0' r1' i1' */
            __ASM_EMIT("movups      %%xmm4, 0x10(%[dst])")      /* dst[1]  = r2' i2' r3' i3' */
            __ASM_EMIT("movups      %%xmm2, 0x20(%[dst])")      /* dst[2]  = r4' i4' r5' i5' */
            __ASM_EMIT("movups      %%xmm5, 0x30(%[dst])")      /* dst[3]  = r6' i6' r7' i7' */
            __ASM_EMIT("add         $0x40, %[dst]")             /* dst += 16 */
            __ASM_EMIT("sub         $8, %[count]")              /* count -= 8 */
            __ASM_EMIT("jae         1b")
            /* Next step */
            __ASM_EMIT("2:")
            __ASM_EMIT("add         $4, %[count]")              /* count += 4 */
            __ASM_EMIT("jl          10f")
            /* Perform 4x RCP */
            __ASM_EMIT("movups      0x00(%[dst]),  %%xmm0")     /* xmm0  = r0 i0 r1 i1 */
            __ASM_EMIT("movups      0x10(%[dst]),  %%xmm4")     /* xmm4  = r2 i2 r3 i3 */
            /* Do shuffle */
            __ASM_EMIT("movaps      %%xmm0, %%xmm1")            /* xmm1  = r0 i0 r1 i1 */
            __ASM_EMIT("shufps      $0x88, %%xmm4, %%xmm0")     /* xmm0  = r0 r1 r2 r3 */
            __ASM_EMIT("shufps      $0xdd, %%xmm4, %%xmm1")     /* xmm1  = i0 i1 i2 i3 */
            /* Perform operations */
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
            /* Do shuffle back */
            __ASM_EMIT("movaps      %%xmm0, %%xmm4")            /* xmm4 = r0' r1' r2' r3' */
            __ASM_EMIT("unpcklps    %%xmm1, %%xmm0")            /* xmm0 = r0' i0' r1' i1' */
            __ASM_EMIT("unpckhps    %%xmm1, %%xmm4")            /* xmm4 = r2' i2' r3' i3' */

            __ASM_EMIT("movups      %%xmm0, 0x00(%[dst])")      /* dst[0]  = r0' i0' r1' i1' */
            __ASM_EMIT("movups      %%xmm4, 0x10(%[dst])")      /* dst[1]  = r2' i2' r3' i3'  */
            __ASM_EMIT("add         $0x20, %[dst]")             /* dst += 8 */
            __ASM_EMIT("sub         $4, %[count]")              /* count -= 4 */
            /* 1x RCP */
            __ASM_EMIT("10:")
            __ASM_EMIT("add         $4, %[count]")              /* count += 4 */
            __ASM_EMIT("jle         50f")
            __ASM_EMIT("40:")
            __ASM_EMIT("movss       0x00(%[dst]),  %%xmm0")     /* xmm0  = r0 */
            __ASM_EMIT("movss       0x04(%[dst]),  %%xmm1")     /* xmm1  = i0 */
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
            __ASM_EMIT("movss       %%xmm0, 0x00(%[dst])")      /* dst[0]  = r0' */
            __ASM_EMIT("movss       %%xmm1, 0x04(%[dst])")      /* dst[1]  = i0' */
            __ASM_EMIT("add         $0x08, %[dst]")             /* dst += 2 */
            __ASM_EMIT("dec         %[count]")                  /* count-- */
            __ASM_EMIT("jnz         40b")

            __ASM_EMIT("50:")
            : [dst] "+r" (dst),
              [count] "+r" (count)
            : [X_ONE] "m" (ONE),
              [X_ISIGN] "m" (X_ISIGN)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void pcomplex_div2(float *dst, const float *src, size_t count)
    {
        size_t off;

        ARCH_X86_ASM
        (
            __ASM_EMIT("xor             %[off], %[off]")
            __ASM_EMIT("sub             $4, %[count]")
            __ASM_EMIT("jb              2f")

            // x4 blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("movups          0x00(%[src], %[off]), %%xmm0")  // xmm0 = sr1 si1
            __ASM_EMIT("movups          0x10(%[src], %[off]), %%xmm4")  // xmm4 = sr2 si2
            __ASM_EMIT("movups          0x00(%[dst], %[off]), %%xmm2")  // xmm2 = dr1, di1
            __ASM_EMIT("movups          0x10(%[dst], %[off]), %%xmm6")  // xmm6 = dr2, di2
            __ASM_EMIT("movaps          %%xmm0, %%xmm1")
            __ASM_EMIT("movaps          %%xmm2, %%xmm3")
            __ASM_EMIT("shufps          $0x88, %%xmm4, %%xmm0")         // xmm0 = sr
            __ASM_EMIT("shufps          $0x88, %%xmm6, %%xmm2")         // xmm2 = dr
            __ASM_EMIT("shufps          $0xdd, %%xmm4, %%xmm1")         // xmm1 = si
            __ASM_EMIT("shufps          $0xdd, %%xmm6, %%xmm3")         // xmm3 = di
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
            __ASM_EMIT("movaps          %%xmm0, %%xmm2")
            __ASM_EMIT("unpcklps        %%xmm1, %%xmm0")
            __ASM_EMIT("unpckhps        %%xmm1, %%xmm2")
            __ASM_EMIT("movups          %%xmm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("movups          %%xmm2, 0x10(%[dst], %[off])")
            __ASM_EMIT("add             $0x20, %[off]")
            __ASM_EMIT("sub             $4, %[count]")
            __ASM_EMIT("jae             1b")

            // x1 blocks
            __ASM_EMIT("2:")
            __ASM_EMIT("add             $3, %[count]")
            __ASM_EMIT("jl              4f")
            __ASM_EMIT("3:")
            __ASM_EMIT("movss           0x00(%[src], %[off]), %%xmm0")  // xmm0 = sr
            __ASM_EMIT("movss           0x04(%[src], %[off]), %%xmm1")  // xmm1 = si
            __ASM_EMIT("movss           0x00(%[dst], %[off]), %%xmm2")  // xmm2 = dr
            __ASM_EMIT("movss           0x04(%[dst], %[off]), %%xmm3")  // xmm3 = di
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
            __ASM_EMIT("movss           %%xmm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("movss           %%xmm1, 0x04(%[dst], %[off])")
            __ASM_EMIT("add             $0x08, %[off]")
            __ASM_EMIT("dec             %[count]")
            __ASM_EMIT("jge             3b")

            __ASM_EMIT("4:")

            : [count] "+r" (count), [off] "=&r" (off)
            : [dst] "r" (dst), [src] "r" (src)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void pcomplex_rdiv2(float *dst, const float *src, size_t count)
    {
        size_t off;

        ARCH_X86_ASM
        (
            __ASM_EMIT("xor             %[off], %[off]")
            __ASM_EMIT("sub             $4, %[count]")
            __ASM_EMIT("jb              2f")

            // x4 blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("movups          0x00(%[dst], %[off]), %%xmm0")  // xmm0 = sr1 si1
            __ASM_EMIT("movups          0x10(%[dst], %[off]), %%xmm4")  // xmm4 = sr2 si2
            __ASM_EMIT("movups          0x00(%[src], %[off]), %%xmm2")  // xmm2 = dr1, di1
            __ASM_EMIT("movups          0x10(%[src], %[off]), %%xmm6")  // xmm6 = dr2, di2
            __ASM_EMIT("movaps          %%xmm0, %%xmm1")
            __ASM_EMIT("movaps          %%xmm2, %%xmm3")
            __ASM_EMIT("shufps          $0x88, %%xmm4, %%xmm0")         // xmm0 = sr
            __ASM_EMIT("shufps          $0x88, %%xmm6, %%xmm2")         // xmm2 = dr
            __ASM_EMIT("shufps          $0xdd, %%xmm4, %%xmm1")         // xmm1 = si
            __ASM_EMIT("shufps          $0xdd, %%xmm6, %%xmm3")         // xmm3 = di
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
            __ASM_EMIT("movaps          %%xmm0, %%xmm2")
            __ASM_EMIT("unpcklps        %%xmm1, %%xmm0")
            __ASM_EMIT("unpckhps        %%xmm1, %%xmm2")
            __ASM_EMIT("movups          %%xmm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("movups          %%xmm2, 0x10(%[dst], %[off])")
            __ASM_EMIT("add             $0x20, %[off]")
            __ASM_EMIT("sub             $4, %[count]")
            __ASM_EMIT("jae             1b")

            // x1 blocks
            __ASM_EMIT("2:")
            __ASM_EMIT("add             $3, %[count]")
            __ASM_EMIT("jl              4f")
            __ASM_EMIT("3:")
            __ASM_EMIT("movss           0x00(%[dst], %[off]), %%xmm0")  // xmm0 = sr
            __ASM_EMIT("movss           0x04(%[dst], %[off]), %%xmm1")  // xmm1 = si
            __ASM_EMIT("movss           0x00(%[src], %[off]), %%xmm2")  // xmm2 = dr
            __ASM_EMIT("movss           0x04(%[src], %[off]), %%xmm3")  // xmm3 = di
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
            __ASM_EMIT("movss           %%xmm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("movss           %%xmm1, 0x04(%[dst], %[off])")
            __ASM_EMIT("add             $0x08, %[off]")
            __ASM_EMIT("dec             %[count]")
            __ASM_EMIT("jge             3b")

            __ASM_EMIT("4:")

            : [count] "+r" (count), [off] "=&r" (off)
            : [dst] "r" (dst), [src] "r" (src)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void pcomplex_div3(float *dst, const float *t, const float *b, size_t count)
    {
        size_t off;

        ARCH_X86_ASM
        (
            __ASM_EMIT("xor             %[off], %[off]")
            __ASM_EMIT("sub             $4, %[count]")
            __ASM_EMIT("jb              2f")

            // x4 blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("movups          0x00(%[b], %[off]), %%xmm0")    // xmm0 = sr1 si1
            __ASM_EMIT("movups          0x10(%[b], %[off]), %%xmm4")    // xmm4 = sr2 si2
            __ASM_EMIT("movups          0x00(%[t], %[off]), %%xmm2")    // xmm2 = dr1, di1
            __ASM_EMIT("movups          0x10(%[t], %[off]), %%xmm6")    // xmm6 = dr2, di2
            __ASM_EMIT("movaps          %%xmm0, %%xmm1")
            __ASM_EMIT("movaps          %%xmm2, %%xmm3")
            __ASM_EMIT("shufps          $0x88, %%xmm4, %%xmm0")         // xmm0 = sr
            __ASM_EMIT("shufps          $0x88, %%xmm6, %%xmm2")         // xmm2 = dr
            __ASM_EMIT("shufps          $0xdd, %%xmm4, %%xmm1")         // xmm1 = si
            __ASM_EMIT("shufps          $0xdd, %%xmm6, %%xmm3")         // xmm3 = di
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
            __ASM_EMIT("movaps          %%xmm0, %%xmm2")
            __ASM_EMIT("unpcklps        %%xmm1, %%xmm0")
            __ASM_EMIT("unpckhps        %%xmm1, %%xmm2")
            __ASM_EMIT("movups          %%xmm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("movups          %%xmm2, 0x10(%[dst], %[off])")
            __ASM_EMIT("add             $0x20, %[off]")
            __ASM_EMIT("sub             $4, %[count]")
            __ASM_EMIT("jae             1b")

            // x1 blocks
            __ASM_EMIT("2:")
            __ASM_EMIT("add             $3, %[count]")
            __ASM_EMIT("jl              4f")
            __ASM_EMIT("3:")
            __ASM_EMIT("movss           0x00(%[b], %[off]), %%xmm0")    // xmm0 = sr
            __ASM_EMIT("movss           0x04(%[b], %[off]), %%xmm1")    // xmm1 = si
            __ASM_EMIT("movss           0x00(%[t], %[off]), %%xmm2")    // xmm2 = dr
            __ASM_EMIT("movss           0x04(%[t], %[off]), %%xmm3")    // xmm3 = di
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
            __ASM_EMIT("movss           %%xmm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("movss           %%xmm1, 0x04(%[dst], %[off])")
            __ASM_EMIT("add             $0x08, %[off]")
            __ASM_EMIT("dec             %[count]")
            __ASM_EMIT("jge             3b")

            __ASM_EMIT("4:")

            : [count] "+r" (count), [off] "=&r" (off)
            : [dst] "r" (dst), [t] "r" (t), [b] "r" (b)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }
}

#endif /* DSP_ARCH_X86_SSE_PCOMPLEX_H_ */
