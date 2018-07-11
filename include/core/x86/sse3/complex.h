/*
 * complex.h
 *
 *  Created on: 17 февр. 2017 г.
 *      Author: sadko
 */

#ifndef CORE_X86_SSE3_COMPLEX_H_
#define CORE_X86_SSE3_COMPLEX_H_

namespace lsp
{
    namespace sse3
    {
#ifdef ARCH_X86_64
        void x64_packed_complex_mul(float *dst, const float *src1, const float *src2, size_t count)
        {
            #define complex_core(MV_DST, MV_SRC1, MV_SRC2) \
                __ASM_EMIT("cmp         $8, %[count]") \
                __ASM_EMIT("jb          3f") \
                __ASM_EMIT(".align 16") \
                __ASM_EMIT("2:") \
                /* Process vectorized data */ \
                __ASM_EMIT(MV_SRC1 "    0x00(%[src1]), %%xmm0")     /* xmm0  = ar0 ai0 ar1 ai1 */ \
                __ASM_EMIT(MV_SRC1 "    0x10(%[src1]), %%xmm1")     /* xmm1  = ar2 ai2 ar3 ai3 */ \
                __ASM_EMIT(MV_SRC1 "    0x20(%[src1]), %%xmm8")     /* xmm8  = ar4 ai4 ar5 ai5 */ \
                __ASM_EMIT(MV_SRC1 "    0x30(%[src1]), %%xmm9")     /* xmm9  = ar6 ai6 ar7 ai7 */ \
                \
                __ASM_EMIT(MV_SRC2 "    0x00(%[src2]), %%xmm4")     /* xmm4  = br0 bi0 br1 bi1 */ \
                __ASM_EMIT(MV_SRC2 "    0x10(%[src2]), %%xmm5")     /* xmm5  = br2 bi2 br3 bi3 */ \
                __ASM_EMIT(MV_SRC2 "    0x20(%[src2]), %%xmm12")    /* xmm12 = br4 bi4 br5 bi5 */ \
                __ASM_EMIT(MV_SRC2 "    0x30(%[src2]), %%xmm13")    /* xmm13 = br6 bi6 br7 bi7 */ \
                /* Do shuffle */ \
                __ASM_EMIT("movaps      %%xmm0, %%xmm2")            /* xmm2  = ar0 ai0 ar1 ai1 */ \
                __ASM_EMIT("movaps      %%xmm4, %%xmm6")            /* xmm6  = br0 bi0 br1 bi1 */ \
                __ASM_EMIT("movaps      %%xmm8, %%xmm10")           /* xmm10 = ar4 ai4 ar5 ai5 */ \
                __ASM_EMIT("movaps      %%xmm12, %%xmm14")          /* xmm14 = br4 bi4 br5 bi5 */ \
                \
                __ASM_EMIT("shufps      $0x88, %%xmm1, %%xmm0")     /* xmm0  = ar0 ar1 ar2 ar3 */ \
                __ASM_EMIT("shufps      $0x88, %%xmm5, %%xmm4")     /* xmm4  = br0 br1 br2 br3 */ \
                __ASM_EMIT("shufps      $0x88, %%xmm9, %%xmm8")     /* xmm8  = ar4 ar5 ar6 ar7 */ \
                __ASM_EMIT("shufps      $0x88, %%xmm13, %%xmm12")   /* xmm12 = br4 br5 br6 br7 */ \
                __ASM_EMIT("shufps      $0xdd, %%xmm1, %%xmm2")     /* xmm2  = ai0 ai1 ai2 ai3 */ \
                __ASM_EMIT("shufps      $0xdd, %%xmm5, %%xmm6")     /* xmm6  = bi0 bi1 bi2 bi3 */ \
                __ASM_EMIT("shufps      $0xdd, %%xmm9, %%xmm10")    /* xmm10 = ai4 ai5 ai6 ai7 */ \
                __ASM_EMIT("shufps      $0xdd, %%xmm13, %%xmm14")   /* xmm14 = bi4 bi5 bi6 bi7 */ \
                /* Calc multiplication */ \
                __ASM_EMIT("movaps      %%xmm0, %%xmm1")            /* xmm1  = ar0 ar1 ar2 ar3 */ \
                __ASM_EMIT("movaps      %%xmm8, %%xmm9")            /* xmm9  = ar4 ar5 ar6 ar7 */ \
                \
                __ASM_EMIT("mulps       %%xmm4, %%xmm0")            /* xmm0  = ar0*br0 ar1*br1 ar2*br2 ar3*br3 */ \
                __ASM_EMIT("mulps       %%xmm12, %%xmm8")           /* xmm8  = ar4*br4 ar5*br5 ar6*br6 ar3*br7 */ \
                __ASM_EMIT("mulps       %%xmm6, %%xmm1")            /* xmm1  = ar0*bi0 ar1*bi1 ar2*bi2 ar3*bi3 */ \
                __ASM_EMIT("mulps       %%xmm14, %%xmm9")           /* xmm9  = ar4*bi4 ar5*bi5 ar6*bi6 ar3*bi7 */ \
                __ASM_EMIT("mulps       %%xmm2, %%xmm6")            /* xmm6  = ai0*bi0 ai1*bi1 ai2*bi2 ai3*bi3 */ \
                __ASM_EMIT("mulps       %%xmm10, %%xmm14")          /* xmm14 = ai4*bi4 ai5*bi5 ai6*bi6 ai3*bi7 */ \
                __ASM_EMIT("mulps       %%xmm2, %%xmm4")            /* xmm4  = ai0*br0 ai1*br1 ai2*br2 ai3*br3 */ \
                __ASM_EMIT("mulps       %%xmm10, %%xmm12")          /* xmm12 = ai4*br4 ai5*br5 ai6*br6 ai3*br7 */ \
                __ASM_EMIT("addps       %%xmm4, %%xmm1")            /* xmm1  = ar[i]*bi[i] + ai[i]*br[i] = i0 i1 i2 i3 */ \
                __ASM_EMIT("addps       %%xmm12, %%xmm9")           /* xmm9  = ar[i]*bi[i] + ai[i]*br[i] = i4 i5 i6 i7 */ \
                __ASM_EMIT("subps       %%xmm6, %%xmm0")            /* xmm0  = ar[i]*br[i] - ai[i]*bi[i] = r0 r1 r2 r3 */ \
                __ASM_EMIT("subps       %%xmm14, %%xmm8")           /* xmm8  = ar[i]*br[i] - ai[i]*bi[i] = r4 r5 r6 r7 */ \
                \
                /* Re-shuffle */ \
                __ASM_EMIT("movaps      %%xmm0, %%xmm2")            /* xmm2  = r0 r1 r2 r3 */ \
                __ASM_EMIT("movaps      %%xmm8, %%xmm10")           /* xmm10 = r4 r5 r6 r7 */ \
                \
                __ASM_EMIT("unpcklps    %%xmm1, %%xmm0")            /* xmm0  = r0 i0 r1 i1 */ \
                __ASM_EMIT("unpcklps    %%xmm9, %%xmm8")            /* xmm8  = r4 i4 r5 i5 */ \
                __ASM_EMIT("unpckhps    %%xmm1, %%xmm2")            /* xmm2  = r2 i2 r3 i3 */ \
                __ASM_EMIT("unpckhps    %%xmm9, %%xmm10")           /* xmm10 = r6 i6 r7 i7 */ \
                \
                /* Store */ \
                __ASM_EMIT(MV_DST "     %%xmm0, 0x00(%[dst])") \
                __ASM_EMIT(MV_DST "     %%xmm2, 0x10(%[dst])") \
                __ASM_EMIT(MV_DST "     %%xmm8, 0x20(%[dst])") \
                __ASM_EMIT(MV_DST "     %%xmm10, 0x30(%[dst])") \
                /* Repeat loop */ \
                __ASM_EMIT("sub         $8, %[count]") \
                __ASM_EMIT("add         $0x40, %[src1]") \
                __ASM_EMIT("add         $0x40, %[src2]") \
                __ASM_EMIT("add         $0x40, %[dst]") \
                __ASM_EMIT("cmp         $8, %[count]") \
                __ASM_EMIT("jae         2b") \
                /* 4-element block */ \
                __ASM_EMIT("3:") \
                __ASM_EMIT("cmp         $4, %[count]") \
                __ASM_EMIT("jb          4f") \
                /* Process vectorized data */ \
                __ASM_EMIT(MV_SRC1 "    0x00(%[src1]), %%xmm0")     /* xmm0  = ar0 ai0 ar1 ai1 */ \
                __ASM_EMIT(MV_SRC1 "    0x10(%[src1]), %%xmm1")     /* xmm1  = ar2 ai2 ar3 ai3 */ \
                __ASM_EMIT(MV_SRC2 "    0x00(%[src2]), %%xmm4")     /* xmm4  = br0 bi0 br1 bi1 */ \
                __ASM_EMIT(MV_SRC2 "    0x10(%[src2]), %%xmm5")     /* xmm5  = br2 bi2 br3 bi3 */ \
                /* Do shuffle */ \
                __ASM_EMIT("movaps      %%xmm0, %%xmm2")            /* xmm2  = ar0 ai0 ar1 ai1 */ \
                __ASM_EMIT("movaps      %%xmm4, %%xmm6")            /* xmm6  = br0 bi0 br1 bi1 */ \
                __ASM_EMIT("shufps      $0x88, %%xmm1, %%xmm0")     /* xmm0  = ar0 ar1 ar2 ar3 */ \
                __ASM_EMIT("shufps      $0x88, %%xmm5, %%xmm4")     /* xmm4  = br0 br1 br2 br3 */ \
                __ASM_EMIT("shufps      $0xdd, %%xmm1, %%xmm2")     /* xmm2  = ai0 ai1 ai2 ai3 */ \
                __ASM_EMIT("shufps      $0xdd, %%xmm5, %%xmm6")     /* xmm6  = bi0 bi1 bi2 bi3 */ \
                /* Calc multiplication */ \
                __ASM_EMIT("movaps      %%xmm0, %%xmm1")            /* xmm1  = ar0 ar1 ar2 ar3 */ \
                __ASM_EMIT("mulps       %%xmm4, %%xmm0")            /* xmm0  = ar0*br0 ar1*br1 ar2*br2 ar3*br3 */ \
                __ASM_EMIT("mulps       %%xmm6, %%xmm1")            /* xmm1  = ar0*bi0 ar1*bi1 ar2*bi2 ar3*bi3 */ \
                __ASM_EMIT("mulps       %%xmm2, %%xmm6")            /* xmm6  = ai0*bi0 ai1*bi1 ai2*bi2 ai3*bi3 */ \
                __ASM_EMIT("mulps       %%xmm2, %%xmm4")            /* xmm4  = ai0*br0 ai1*br1 ai2*br2 ai3*br3 */ \
                __ASM_EMIT("addps       %%xmm4, %%xmm1")            /* xmm1  = ar[i]*bi[i] + ai[i]*br[i] = i0 i1 i2 i3 */ \
                __ASM_EMIT("subps       %%xmm6, %%xmm0")            /* xmm0  = ar[i]*br[i] - ai[i]*bi[i] = r0 r1 r2 r3 */ \
                \
                /* Re-shuffle */ \
                __ASM_EMIT("movaps      %%xmm0, %%xmm2")            /* xmm2  = r0 r1 r2 r3 */ \
                __ASM_EMIT("unpcklps    %%xmm1, %%xmm0")            /* xmm0  = r0 i0 r1 i1 */ \
                __ASM_EMIT("unpckhps    %%xmm1, %%xmm2")            /* xmm2  = r2 i2 r3 i3 */ \
                /* Store */ \
                __ASM_EMIT(MV_DST "     %%xmm0, 0x00(%[dst])") \
                __ASM_EMIT(MV_DST "     %%xmm2, 0x10(%[dst])") \
                /* Repeat loop */ \
                __ASM_EMIT("sub         $4, %[count]") \
                __ASM_EMIT("add         $0x20, %[src1]") \
                __ASM_EMIT("add         $0x20, %[src2]") \
                __ASM_EMIT("add         $0x20, %[dst]") \
                __ASM_EMIT("jmp         4f")


            __asm__ __volatile__
            (
                /* Check count */
                __ASM_EMIT("1:")

                /* Do block processing */
                __ASM_EMIT("test $0x0f, %[dst]")
                __ASM_EMIT("jnz 1100f")
                    __ASM_EMIT("test $0x0f, %[src1]")
                    __ASM_EMIT("jnz 1010f")
                        __ASM_EMIT("test $0x0f, %[src2]")
                        __ASM_EMIT("jnz 1001f")
                            complex_core("movaps", "movaps", "movaps")
                        __ASM_EMIT("1001:")
                            complex_core("movaps", "movaps", "movups")
                    __ASM_EMIT("1010:")
                        __ASM_EMIT("test $0x0f, %[src2]")
                        __ASM_EMIT("jnz 1011f")
                            complex_core("movaps", "movups", "movaps")
                        __ASM_EMIT("1011:")
                            complex_core("movaps", "movups", "movups")

                __ASM_EMIT("1100:")
                    __ASM_EMIT("test $0x0f, %[src1]")
                    __ASM_EMIT("jnz 1110f")
                        __ASM_EMIT("test $0x0f, %[src2]")
                        __ASM_EMIT("jnz 1101f")
                            complex_core("movups", "movaps", "movaps")
                        __ASM_EMIT("1101:")
                            complex_core("movups", "movaps", "movups")
                    __ASM_EMIT("1110:")
                        __ASM_EMIT("test $0x0f, %[src2]")
                        __ASM_EMIT("jnz 1111f")
                            complex_core("movups", "movups", "movaps")
                        __ASM_EMIT("1111:")
                            complex_core("movups", "movups", "movups")

                /* Check count again */
                __ASM_EMIT("4:")
                __ASM_EMIT("test        %[count], %[count]")
                __ASM_EMIT("jz          6f")
                /* Process scalar data */
                __ASM_EMIT("5:")
                /* Load */
                __ASM_EMIT("movss       0x00(%[src1]), %%xmm0")     /* xmm0 = ar */
                __ASM_EMIT("movss       0x04(%[src1]), %%xmm2")     /* xmm2 = ai */
                __ASM_EMIT("movss       0x00(%[src2]), %%xmm4")     /* xmm4 = br */
                __ASM_EMIT("movss       0x04(%[src2]), %%xmm6")     /* xmm6 = bi */
                /* Calculate multiplication */
                __ASM_EMIT("movaps      %%xmm0, %%xmm1")            /* xmm1 = ar */
                __ASM_EMIT("mulss       %%xmm4, %%xmm0")            /* xmm0 = ar*br */
                __ASM_EMIT("mulss       %%xmm6, %%xmm1")            /* xmm1 = ar*bi */
                __ASM_EMIT("mulss       %%xmm2, %%xmm6")            /* xmm6 = ai*bi */
                __ASM_EMIT("mulss       %%xmm2, %%xmm4")            /* xmm4 = ai*br */
                __ASM_EMIT("addss       %%xmm4, %%xmm1")            /* xmm1 = ar*bi + ai*br = i */
                __ASM_EMIT("subss       %%xmm6, %%xmm0")            /* xmm0 = ar*br - ai*bi = r */
                /* Store */
                __ASM_EMIT("movss       %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("movss       %%xmm1, 0x04(%[dst])")
                /* Repeat loop */
                __ASM_EMIT("add         $0x08, %[src1]")
                __ASM_EMIT("add         $0x08, %[src2]")
                __ASM_EMIT("add         $0x08, %[dst]")
                __ASM_EMIT("dec         %[count]")
                __ASM_EMIT("jnz         5b")

                /* Exit */
                __ASM_EMIT("6:")

                : [dst] "+r" (dst), [src1] "+r" (src1), [src2] "+r" (src2), [count] "+r" (count)
                :
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm4", "%xmm5", "%xmm6",
                  "%xmm8", "%xmm9", "%xmm10", "%xmm12", "%xmm13", "%xmm14"
            );

            #undef complex_core
        }

#endif /* ARCH_X86_64 */
    }
}


#endif /* CORE_X86_SSE3_COMPLEX_H_ */
