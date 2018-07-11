/*
 * complex.h
 *
 *  Created on: 05 окт. 2015 г.
 *      Author: sadko
 */

#ifndef CORE_X86_SSE_COMPLEX_H_
#define CORE_X86_SSE_COMPLEX_H_

namespace lsp
{
    namespace sse
    {

        void packed_complex_mul(float *dst, const float *src1, const float *src2, size_t count)
        {
            #define complex_core(MV_DST, MV_SRC1, MV_SRC2) \
                __ASM_EMIT("2:") \
                /* Process vectorized data */ \
                __ASM_EMIT(MV_SRC1 "    0x00(%[src1]), %%xmm0")     /* xmm0 = ar0 ai0 ar1 ai1 */ \
                __ASM_EMIT(MV_SRC1 "    0x10(%[src1]), %%xmm1")     /* xmm1 = ar2 ai2 ar3 ai3 */ \
                __ASM_EMIT(MV_SRC2 "    0x00(%[src2]), %%xmm4")     /* xmm4 = br0 bi0 br1 bi1 */ \
                __ASM_EMIT(MV_SRC2 "    0x10(%[src2]), %%xmm5")     /* xmm5 = br2 bi2 br3 bi3 */ \
                /* Do shuffle */ \
                __ASM_EMIT("movaps      %%xmm0, %%xmm2")            /* xmm2 = ar0 ai0 ar1 ai1 */ \
                __ASM_EMIT("movaps      %%xmm4, %%xmm6")            /* xmm6 = br0 bi0 br1 bi1 */ \
                __ASM_EMIT("shufps      $0x88, %%xmm1, %%xmm0")     /* xmm0 = ar0 ar1 ar2 ar3 */ \
                __ASM_EMIT("shufps      $0x88, %%xmm5, %%xmm4")     /* xmm4 = br0 br1 br2 br3 */ \
                __ASM_EMIT("shufps      $0xdd, %%xmm1, %%xmm2")     /* xmm2 = ai0 ai1 ai2 ai3 */ \
                __ASM_EMIT("shufps      $0xdd, %%xmm5, %%xmm6")     /* xmm6 = bi0 bi1 bi2 bi3 */ \
                /* Calc multiplication */ \
                __ASM_EMIT("movaps      %%xmm0, %%xmm1")            /* xmm1 = ar0 ar1 ar2 ar3 */ \
                __ASM_EMIT("mulps       %%xmm4, %%xmm0")            /* xmm0 = ar0*br0 ar1*br1 ar2*br2 ar3*br3 */ \
                __ASM_EMIT("mulps       %%xmm6, %%xmm1")            /* xmm1 = ar0*bi0 ar1*bi1 ar2*bi2 ar3*bi3 */ \
                __ASM_EMIT("mulps       %%xmm2, %%xmm6")            /* xmm6 = ai0*bi0 ai1*bi1 ai2*bi2 ai3*bi3 */ \
                __ASM_EMIT("mulps       %%xmm2, %%xmm4")            /* xmm4 = ai0*br0 ai1*br1 ai2*br2 ai3*br3 */ \
                __ASM_EMIT("addps       %%xmm4, %%xmm1")            /* xmm1 = ar[i]*bi[i] + ai[i]*br[i] = i0 i1 i2 i3 */ \
                __ASM_EMIT("subps       %%xmm6, %%xmm0")            /* xmm0 = ar[i]*br[i] - ai[i]*bi[i] = r0 r1 r2 r3 */ \
                \
                /* Re-shuffle */ \
                __ASM_EMIT("movaps      %%xmm0, %%xmm2")            /* xmm2 = r0 r1 r2 r3 */ \
                __ASM_EMIT("unpcklps    %%xmm1, %%xmm0")            /* xmm0 = r0 i0 r1 i1 */ \
                __ASM_EMIT("unpckhps    %%xmm1, %%xmm2")            /* xmm2 = r2 i2 r3 i3 */ \
                \
                /* Store */ \
                __ASM_EMIT(MV_DST "     %%xmm0, 0x00(%[dst])") \
                __ASM_EMIT(MV_DST "     %%xmm2, 0x10(%[dst])") \
                /* Repeat loop */ \
                __ASM_EMIT("sub         $4, %[count]") \
                __ASM_EMIT("add         $0x20, %[src1]") \
                __ASM_EMIT("add         $0x20, %[src2]") \
                __ASM_EMIT("add         $0x20, %[dst]") \
                __ASM_EMIT("cmp         $4, %[count]") \
                __ASM_EMIT("jae         2b") \
                __ASM_EMIT("jmp         3f")


            __asm__ __volatile__
            (
                /* Check count */
                __ASM_EMIT("1:")
                __ASM_EMIT("cmp         $4, %[count]")
                __ASM_EMIT("jb          3f")

                /* Do block processing */
                __ASM_EMIT("test $0x0f, %[dst]")
                __ASM_EMIT("jnz 1100f")
                    __ASM_EMIT("test $0x0f, %[src1]")
                    __ASM_EMIT("jnz 1010f")
                        __ASM_EMIT("test $0x0f, %[src2]")
                        __ASM_EMIT("jnz 1001f")
                            __ASM_EMIT(".align 16")
                            complex_core("movaps", "movaps", "movaps")
                        __ASM_EMIT(".align 16")
                        __ASM_EMIT("1001:")
                            complex_core("movaps", "movaps", "movups")
                    __ASM_EMIT("1010:")
                        __ASM_EMIT("test $0x0f, %[src2]")
                        __ASM_EMIT("jnz 1011f")
                            __ASM_EMIT(".align 16")
                            complex_core("movaps", "movups", "movaps")
                        __ASM_EMIT(".align 16")
                        __ASM_EMIT("1011:")
                            complex_core("movaps", "movups", "movups")

                __ASM_EMIT("1100:")
                    __ASM_EMIT("test $0x0f, %[src1]")
                    __ASM_EMIT("jnz 1110f")
                        __ASM_EMIT("test $0x0f, %[src2]")
                        __ASM_EMIT("jnz 1101f")
                            __ASM_EMIT(".align 16")
                            complex_core("movups", "movaps", "movaps")
                        __ASM_EMIT(".align 16")
                        __ASM_EMIT("1101:")
                            complex_core("movups", "movaps", "movups")
                    __ASM_EMIT("1110:")
                        __ASM_EMIT("test $0x0f, %[src2]")
                        __ASM_EMIT("jnz 1111f")
                            __ASM_EMIT(".align 16")
                            complex_core("movups", "movups", "movaps")
                        __ASM_EMIT(".align 16")
                        __ASM_EMIT("1111:")
                            complex_core("movups", "movups", "movups")

                /* Check count again */
                __ASM_EMIT("3:")
                __ASM_EMIT("test        %[count], %[count]")
                __ASM_EMIT("jz          5f")

                /* Process scalar data */
                __ASM_EMIT("4:")
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
                __ASM_EMIT("jnz         4b")

                /* Exit */
                __ASM_EMIT("5:")

                : [dst] "+r" (dst), [src1] "+r" (src1), [src2] "+r" (src2), [count] "+r" (count)
                :
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm4", "%xmm5", "%xmm6"
            );

            #undef complex_core
        }

        void packed_real_to_complex(float *dst, const float *src, size_t count)
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


                __asm__ __volatile__
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


                __asm__ __volatile__
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

        void packed_complex_to_real(float *dst, const float *src, size_t count)
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


            __asm__ __volatile__
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

        void packed_complex_add_to_real(float *dst, const float *src, size_t count)
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
                /* Add and store data */ \
                __ASM_EMIT(MV_DST "     0x00(%[dst]), %%xmm1") \
                __ASM_EMIT(MV_DST "     0x10(%[dst]), %%xmm3") \
                __ASM_EMIT(MV_DST "     0x20(%[dst]), %%xmm5") \
                __ASM_EMIT(MV_DST "     0x30(%[dst]), %%xmm7") \
                __ASM_EMIT("addps       %%xmm1, %%xmm0") \
                __ASM_EMIT("addps       %%xmm3, %%xmm2") \
                __ASM_EMIT("addps       %%xmm5, %%xmm4") \
                __ASM_EMIT("addps       %%xmm7, %%xmm6") \
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


            __asm__ __volatile__
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

        void complex_mul(float *dst_re, float *dst_im, const float *src1_re, const float *src1_im, const float *src2_re, const float *src2_im, size_t count)
        {
            if (count == 0)
                return;

            __asm__ __volatile__
            (
                __ASM_EMIT("1:")

                // Check conditions
                #ifndef __x86_64__
                __ASM_EMIT("testb $0x0f, %0")
                #else
                __ASM_EMIT("test $0x0f, %0")
                #endif
                __ASM_EMIT("jz 2f")

                // Process data
                __ASM_EMIT("movss   (%2), %%xmm0")    // xmm0 = *src1_re
                __ASM_EMIT("movss   (%3), %%xmm1")    // xmm1 = *src1_im
                __ASM_EMIT("movss   (%4), %%xmm2")    // xmm2 = *src2_re
                __ASM_EMIT("movss   (%5), %%xmm3")    // xmm3 = *src2_im
                __ASM_EMIT("movaps  %%xmm0, %%xmm4")  // xmm4 = *src1_re
                __ASM_EMIT("movaps  %%xmm1, %%xmm5")  // xmm5 = *src1_im
                __ASM_EMIT("mulss   %%xmm2, %%xmm0")  // xmm0 = *src1_re * *src2_re
                __ASM_EMIT("mulss   %%xmm3, %%xmm1")  // xmm1 = *src1_im * *src2_im
                __ASM_EMIT("mulss   %%xmm5, %%xmm2")  // xmm2 = *src2_re * *src1_im
                __ASM_EMIT("mulss   %%xmm4, %%xmm3")  // xmm3 = *src2_im * *src1_re
                __ASM_EMIT("subss   %%xmm1, %%xmm0")  // xmm0 = *src1_re * *src2_re - *src1_im * *src2_im
                __ASM_EMIT("addss   %%xmm3, %%xmm2")  // xmm2 = *src2_re * *src1_im + *src2_im * *src1_re
                #ifndef __x86_64__
                __ASM_EMIT("xchg    %0, %2")
                __ASM_EMIT("xchg    %1, %3")
                __ASM_EMIT("movss   %%xmm0, (%2)")
                __ASM_EMIT("movss   %%xmm2, (%3)")
                #else
                __ASM_EMIT("movss   %%xmm0, (%0)")
                __ASM_EMIT("movss   %%xmm2, (%1)")
                #endif

                // Move pointers
                __ASM_EMIT("add     $0x4, %2")
                __ASM_EMIT("add     $0x4, %3")
                __ASM_EMIT("add     $0x4, %4")
                __ASM_EMIT("add     $0x4, %5")
                #ifndef __x86_64__
                __ASM_EMIT("xchg    %0, %2")
                __ASM_EMIT("xchg    %1, %3")
                __ASM_EMIT("add     $0x4, %2")
                __ASM_EMIT("add     $0x4, %3")
                #else
                __ASM_EMIT("add     $0x4, %0")
                __ASM_EMIT("add     $0x4, %1")
                #endif

                // Repeat loop
                __ASM_EMIT("dec %6")
                __ASM_EMIT("jnz 1b")
                __ASM_EMIT("2:")

                :
                  #ifndef __x86_64__
                  "+m" (dst_re), "+m" (dst_im),
                  #else
                  "+r" (dst_re), "+r" (dst_im),
                  #endif
                  "+r" (src1_re), "+r" (src1_im),
                  "+r" (src2_re), "+r" (src2_im),
                  "+r" (count)
                :
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5"
            );

            if (count >= SSE_MULTIPLE)
            {
            #ifdef __i386__
                #define cplx_mul4x(s_im, r_re1, r_im1, r_re2, r_im2) \
                        __ASM_EMIT("100:") \
                        \
                        /* Prefetch data */ \
                        __ASM_EMIT("prefetchnta 0x20(%2)") \
                        __ASM_EMIT("prefetchnta 0x20(%3)") \
                        __ASM_EMIT("prefetchnta 0x20(%4)") \
                        __ASM_EMIT("prefetchnta 0x20(%5)") \
                        /* Process data */ \
                        __ASM_EMIT(r_re1 "  (%2), %%xmm0") \
                        __ASM_EMIT(r_im1 "  (%3), %%xmm1") \
                        __ASM_EMIT(r_re2 "  (%4), %%xmm2") \
                        __ASM_EMIT(r_im2 "  (%5), %%xmm3") \
                        __ASM_EMIT("movaps  %%xmm0, %%xmm4") \
                        __ASM_EMIT("movaps  %%xmm1, %%xmm5") \
                        __ASM_EMIT("mulps   %%xmm2, %%xmm0") \
                        __ASM_EMIT("mulps   %%xmm3, %%xmm1") \
                        __ASM_EMIT("mulps   %%xmm5, %%xmm2") \
                        __ASM_EMIT("mulps   %%xmm4, %%xmm3") \
                        __ASM_EMIT("subps   %%xmm1, %%xmm0") \
                        __ASM_EMIT("addps   %%xmm3, %%xmm2") \
                        __ASM_EMIT("xchg    %0, %2") \
                        __ASM_EMIT("xchg    %1, %3") \
                        __ASM_EMIT(MOVNTPS " %%xmm0, (%2)") \
                        __ASM_EMIT(s_im "   %%xmm2, (%3)") \
                        \
                        /* Move pointers */ \
                        __ASM_EMIT("sub     $0x4, %6") \
                        __ASM_EMIT("add     $0x10, %2") \
                        __ASM_EMIT("add     $0x10, %3") \
                        __ASM_EMIT("add     $0x10, %4") \
                        __ASM_EMIT("add     $0x10, %5") \
                        __ASM_EMIT("xchg    %0, %2") \
                        __ASM_EMIT("xchg    %1, %3") \
                        __ASM_EMIT("add     $0x10, %2") \
                        __ASM_EMIT("add     $0x10, %3") \
                        \
                        /* Repeat loop */ \
                        __ASM_EMIT("test $-4, %6") \
                        __ASM_EMIT("jnz 100b")  \
                        __ASM_EMIT("jmp 32f")
            #else // __x86_64__
                register size_t offset = 0;

                #define cplx_mul4x(s_im, r_re1, r_im1, r_re2, r_im2) \
                        __ASM_EMIT("100:") \
                        \
                        /* Prefetch data */ \
                        __ASM_EMIT("prefetchnta 0x20(%2,%7)") \
                        __ASM_EMIT("prefetchnta 0x20(%3,%7)") \
                        __ASM_EMIT("prefetchnta 0x20(%4,%7)") \
                        __ASM_EMIT("prefetchnta 0x20(%5,%7)") \
                        /* Process data */ \
                        __ASM_EMIT(r_re1 "  (%2,%7), %%xmm0") \
                        __ASM_EMIT(r_im1 "  (%3,%7), %%xmm1") \
                        __ASM_EMIT(r_re2 "  (%4,%7), %%xmm2") \
                        __ASM_EMIT(r_im2 "  (%5,%7), %%xmm3") \
                        __ASM_EMIT("movaps  %%xmm0, %%xmm4") \
                        __ASM_EMIT("movaps  %%xmm1, %%xmm5") \
                        __ASM_EMIT("mulps   %%xmm2, %%xmm0") \
                        __ASM_EMIT("mulps   %%xmm3, %%xmm1") \
                        __ASM_EMIT("mulps   %%xmm5, %%xmm2") \
                        __ASM_EMIT("mulps   %%xmm4, %%xmm3") \
                        __ASM_EMIT("subps   %%xmm1, %%xmm0") \
                        __ASM_EMIT("addps   %%xmm3, %%xmm2") \
                        __ASM_EMIT(MOVNTPS " %%xmm0, (%0,%7)") \
                        __ASM_EMIT(s_im "   %%xmm2, (%1,%7)") \
                        \
                        /* Move pointers */ \
                        __ASM_EMIT("sub $0x4, %6") \
                        __ASM_EMIT("add $0x10, %7") \
                        \
                        /* Repeat loop */ \
                        __ASM_EMIT("test $-4, %6") \
                        __ASM_EMIT("jnz 100b")  \
                        __ASM_EMIT("jmp 32f")

            #endif /* __i386__ */

                __asm__ __volatile__
                (
                    __ASM_EMIT("prefetchnta (%0)")
                    __ASM_EMIT("prefetchnta (%1)")
                    __ASM_EMIT("prefetchnta (%2)")
                    __ASM_EMIT("prefetchnta (%3)")
                    : : "r" (src1_re), "r" (src1_im), "r" (src2_re), "r" (src2_im) :
                );

                __asm__ __volatile__
                (
                    #ifdef __x86_64__
                    __ASM_EMIT("test $0x0f, %1") // sse_aligned(dst_im)
                    #else
                    __ASM_EMIT("testb $0x0f, %1") // sse_aligned(dst_im)
                    #endif /* __x86_64__ */
                    __ASM_EMIT("jnz 16f")
                        __ASM_EMIT("test $0x0f, %2") // sse_aligned(src1_re)
                        __ASM_EMIT("jnz 8f")
                            __ASM_EMIT("test $0x0f, %3") // sse_aligned(src1_im)
                            __ASM_EMIT("jnz 4f")
                                __ASM_EMIT("test $0x0f, %4") // sse_aligned(src2_re)
                                __ASM_EMIT("jnz 2f")
                                    __ASM_EMIT("test $0x0f, %5") // sse_aligned(src2_im)
                                    __ASM_EMIT("jnz 1f")
                                        cplx_mul4x(MOVNTPS, "movaps", "movaps", "movaps", "movaps")
                                    __ASM_EMIT("1:") // !sse_aligned(src2_re)
                                        cplx_mul4x(MOVNTPS, "movaps", "movaps", "movaps", "movups")
                                __ASM_EMIT("2:") // !sse_aligned(src2_re)
                                    __ASM_EMIT("test $0x0f, %5") // sse_aligned(src2_im)
                                    __ASM_EMIT("jnz 3f")
                                        cplx_mul4x(MOVNTPS, "movaps", "movaps", "movups", "movaps")
                                    __ASM_EMIT("3:") // !sse_aligned(src2_re)
                                        cplx_mul4x(MOVNTPS, "movaps", "movaps", "movups", "movups")
                            __ASM_EMIT("4:") // !sse_aligned(src1_im)
                                __ASM_EMIT("test $0x0f, %4") // sse_aligned(src2_re)
                                __ASM_EMIT("jnz 6f")
                                    __ASM_EMIT("test $0x0f, %5") // sse_aligned(src2_im)
                                    __ASM_EMIT("jnz 5f")
                                        cplx_mul4x(MOVNTPS, "movaps", "movups", "movaps", "movaps")
                                    __ASM_EMIT("5:") // !sse_aligned(src2_re)
                                        cplx_mul4x(MOVNTPS, "movaps", "movups", "movaps", "movups")
                                __ASM_EMIT("6:") // !sse_aligned(src2_re)
                                    __ASM_EMIT("test $0x0f, %5") // sse_aligned(src2_im)
                                    __ASM_EMIT("jnz 7f")
                                        cplx_mul4x(MOVNTPS, "movaps", "movups", "movups", "movaps")
                                    __ASM_EMIT("7:") // !sse_aligned(src2_re)
                                        cplx_mul4x(MOVNTPS, "movaps", "movups", "movups", "movups")
                        __ASM_EMIT("8:") // !sse_aligned(src1_re)
                        __ASM_EMIT("test $0x0f, %3") // sse_aligned(src1_im)
                            __ASM_EMIT("jnz 12f")
                                __ASM_EMIT("test $0x0f, %4") // sse_aligned(src2_re)
                                __ASM_EMIT("jnz 10f")
                                    __ASM_EMIT("test $0x0f, %5") // sse_aligned(src2_im)
                                    __ASM_EMIT("jnz 9f")
                                        cplx_mul4x(MOVNTPS, "movups", "movaps", "movaps", "movaps")
                                    __ASM_EMIT("9:") // !sse_aligned(src2_re)
                                        cplx_mul4x(MOVNTPS, "movups", "movaps", "movaps", "movups")
                                __ASM_EMIT("10:") // !sse_aligned(src2_re)
                                    __ASM_EMIT("test $0x0f, %5") // sse_aligned(src2_im)
                                    __ASM_EMIT("jnz 11f")
                                        cplx_mul4x(MOVNTPS, "movups", "movaps", "movups", "movaps")
                                    __ASM_EMIT("11:") // !sse_aligned(src2_re)
                                        cplx_mul4x(MOVNTPS, "movups", "movaps", "movups", "movups")
                            __ASM_EMIT("12:") // !sse_aligned(src1_im)
                                __ASM_EMIT("test $0x0f, %4") // sse_aligned(src2_re)
                                __ASM_EMIT("jnz 14f")
                                    __ASM_EMIT("test $0x0f, %5") // sse_aligned(src2_im)
                                    __ASM_EMIT("jnz 13f")
                                        cplx_mul4x(MOVNTPS, "movups", "movups", "movaps", "movaps")
                                    __ASM_EMIT("13:") // !sse_aligned(src2_re)
                                        cplx_mul4x(MOVNTPS, "movups", "movups", "movaps", "movups")
                                __ASM_EMIT("14:") // !sse_aligned(src2_re)
                                    __ASM_EMIT("test $0x0f, %5") // sse_aligned(src2_im)
                                    __ASM_EMIT("jnz 15f")
                                        cplx_mul4x(MOVNTPS, "movups", "movups", "movups", "movaps")
                                    __ASM_EMIT("15:") // !sse_aligned(src2_re)
                                        cplx_mul4x(MOVNTPS, "movups", "movups", "movups", "movups")

                    __ASM_EMIT("16:") // !sse_aligned(dst_im)
                        __ASM_EMIT("test $0x0f, %2") // sse_aligned(src1_re)
                            __ASM_EMIT("jnz 24f")
                                __ASM_EMIT("test $0x0f, %3") // sse_aligned(src1_im)
                                __ASM_EMIT("jnz 20f")
                                    __ASM_EMIT("test $0x0f, %4") // sse_aligned(src2_re)
                                    __ASM_EMIT("jnz 18f")
                                        __ASM_EMIT("test $0x0f, %5") // sse_aligned(src2_im)
                                        __ASM_EMIT("jnz 17f")
                                            cplx_mul4x("movups",  "movaps", "movaps", "movaps", "movaps")
                                        __ASM_EMIT("17:") // !sse_aligned(src2_re)
                                            cplx_mul4x("movups",  "movaps", "movaps", "movaps", "movups")
                                    __ASM_EMIT("18:") // !sse_aligned(src2_re)
                                        __ASM_EMIT("test $0x0f, %5") // sse_aligned(src2_im)
                                        __ASM_EMIT("jnz 19f")
                                            cplx_mul4x("movups",  "movaps", "movaps", "movups", "movaps")
                                        __ASM_EMIT("19:") // !sse_aligned(src2_re)
                                            cplx_mul4x("movups",  "movaps", "movaps", "movups", "movups")
                                __ASM_EMIT("20:") // !sse_aligned(src1_im)
                                    __ASM_EMIT("test $0x0f, %4") // sse_aligned(src2_re)
                                    __ASM_EMIT("jnz 22f")
                                        __ASM_EMIT("test $0x0f, %5") // sse_aligned(src2_im)
                                        __ASM_EMIT("jnz 21f")
                                            cplx_mul4x("movups",  "movaps", "movups", "movaps", "movaps")
                                        __ASM_EMIT("21:") // !sse_aligned(src2_re)
                                            cplx_mul4x("movups",  "movaps", "movups", "movaps", "movups")
                                    __ASM_EMIT("22:") // !sse_aligned(src2_re)
                                        __ASM_EMIT("test $0x0f, %5") // sse_aligned(src2_im)
                                        __ASM_EMIT("jnz 23f")
                                            cplx_mul4x("movups",  "movaps", "movups", "movups", "movaps")
                                        __ASM_EMIT("23:") // !sse_aligned(src2_re)
                                            cplx_mul4x("movups",  "movaps", "movups", "movups", "movups")
                            __ASM_EMIT("24:") // !sse_aligned(src1_re)
                            __ASM_EMIT("test $0x0f, %3") // sse_aligned(src1_im)
                                __ASM_EMIT("jnz 28f")
                                    __ASM_EMIT("test $0x0f, %4") // sse_aligned(src2_re)
                                    __ASM_EMIT("jnz 26f")
                                        __ASM_EMIT("test $0x0f, %5") // sse_aligned(src2_im)
                                        __ASM_EMIT("jnz 25f")
                                            cplx_mul4x("movups",  "movups", "movaps", "movaps", "movaps")
                                        __ASM_EMIT("25:") // !sse_aligned(src2_re)
                                            cplx_mul4x("movups",  "movups", "movaps", "movaps", "movups")
                                    __ASM_EMIT("26:") // !sse_aligned(src2_re)
                                        __ASM_EMIT("test $0x0f, %5") // sse_aligned(src2_im)
                                        __ASM_EMIT("jnz 27f")
                                            cplx_mul4x("movups",  "movups", "movaps", "movups", "movaps")
                                        __ASM_EMIT("27:") // !sse_aligned(src2_re)
                                            cplx_mul4x("movups",  "movups", "movaps", "movups", "movups")
                                __ASM_EMIT("28:") // !sse_aligned(src1_im)
                                    __ASM_EMIT("test $0x0f, %4") // sse_aligned(src2_re)
                                    __ASM_EMIT("jnz 30f")
                                        __ASM_EMIT("test $0x0f, %5") // sse_aligned(src2_im)
                                        __ASM_EMIT("jnz 29f")
                                            cplx_mul4x("movups",  "movups", "movups", "movaps", "movaps")
                                        __ASM_EMIT("29:") // !sse_aligned(src2_re)
                                            cplx_mul4x("movups",  "movups", "movups", "movaps", "movups")
                                    __ASM_EMIT("30:") // !sse_aligned(src2_re)
                                        __ASM_EMIT("test $0x0f, %5") // sse_aligned(src2_im)
                                        __ASM_EMIT("jnz 31f")
                                            cplx_mul4x("movups",  "movups", "movups", "movups", "movaps")
                                        __ASM_EMIT("31:") // !sse_aligned(src2_re)
                                            cplx_mul4x("movups",  "movups", "movups", "movups", "movups")
                    __ASM_EMIT("32:")

                    :
                      #ifndef __x86_64__
                      "+m" (dst_re), "+m" (dst_im),
                      #else
                      "+r" (dst_re), "+r" (dst_im),
                      #endif
                      "+r" (src1_re), "+r" (src1_im),
                      "+r" (src2_re), "+r" (src2_im),
                      "+r" (count)
                      #ifdef __x86_64__
                      ,"+r"(offset)
                      #endif /* __x86_64__ */
                    :
                    : "cc", "memory",
                      "%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5"
                );

                #undef cplx_mul4x

                #ifdef __x86_64__
                    __asm__ __volatile__
                    (
                        // Update pointers
                        __ASM_EMIT("add %6, %0")
                        __ASM_EMIT("add %6, %1")
                        __ASM_EMIT("add %6, %2")
                        __ASM_EMIT("add %6, %3")
                        __ASM_EMIT("add %6, %4")
                        __ASM_EMIT("add %6, %5")

                        : "+r" (dst_re), "+r" (dst_im),
                          "+r" (src1_re), "+r" (src1_im),
                          "+r" (src2_re), "+r" (src2_im),
                          "+r" (offset)
                        :
                        : "cc",
                          "%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5"
                    );
                #endif /* __x86_64__ */
            }

            if (count > 0)
            {
                __asm__ __volatile__
                (
                    __ASM_EMIT("1:")

                    // Process data
                    __ASM_EMIT("movss   (%2), %%xmm0")
                    __ASM_EMIT("movss   (%3), %%xmm1")
                    __ASM_EMIT("movss   (%4), %%xmm2")
                    __ASM_EMIT("movss   (%5), %%xmm3")
                    __ASM_EMIT("movaps  %%xmm0, %%xmm4")
                    __ASM_EMIT("movaps  %%xmm1, %%xmm5")
                    __ASM_EMIT("mulss   %%xmm2, %%xmm0")
                    __ASM_EMIT("mulss   %%xmm3, %%xmm1")
                    __ASM_EMIT("mulss   %%xmm5, %%xmm2")
                    __ASM_EMIT("mulss   %%xmm4, %%xmm3")
                    __ASM_EMIT("subss   %%xmm1, %%xmm0")
                    __ASM_EMIT("addss   %%xmm3, %%xmm2")
                    #ifndef __x86_64__
                    __ASM_EMIT("xchg    %0, %2")
                    __ASM_EMIT("xchg    %1, %3")
                    __ASM_EMIT("movss   %%xmm0, (%2)")
                    __ASM_EMIT("movss   %%xmm2, (%3)")
                    #else
                    __ASM_EMIT("movss   %%xmm0, (%0)")
                    __ASM_EMIT("movss   %%xmm2, (%1)")
                    #endif

                    // Move pointers
                    __ASM_EMIT("add     $0x4, %2")
                    __ASM_EMIT("add     $0x4, %3")
                    __ASM_EMIT("add     $0x4, %4")
                    __ASM_EMIT("add     $0x4, %5")
                    #ifndef __x86_64__
                    __ASM_EMIT("xchg    %0, %2")
                    __ASM_EMIT("xchg    %1, %3")
                    __ASM_EMIT("add     $0x4, %2")
                    __ASM_EMIT("add     $0x4, %3")
                    #else
                    __ASM_EMIT("add     $0x4, %0")
                    __ASM_EMIT("add     $0x4, %1")
                    #endif

                    // Repeat loop
                    __ASM_EMIT("dec %6")
                    __ASM_EMIT("jnz 1b")
                    :
                      #ifndef __x86_64__
                      "+m" (dst_re), "+m" (dst_im),
                      #else
                      "+r" (dst_re), "+r" (dst_im),
                      #endif
                      "+r" (src1_re), "+r" (src1_im),
                      "+r" (src2_re), "+r" (src2_im),
                      "+r" (count)
                    :
                    : "cc", "memory",
                      "%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5"
                );
            }

            SFENCE;
        }

        static void complex_mod(float *dst_mod, const float *src_re, const float *src_im, size_t count)
        {
            if (count == 0)
                return;

            __asm__ __volatile__
            (
                __ASM_EMIT("1:")

                // Check conditions
                __ASM_EMIT("test $0x0f, %2")
                __ASM_EMIT("jz 2f")

                // Process data
                __ASM_EMIT("movss (%0), %%xmm0")
                __ASM_EMIT("movss (%1), %%xmm1")
                __ASM_EMIT("mulss %%xmm0, %%xmm0")
                __ASM_EMIT("mulss %%xmm1, %%xmm1")
                __ASM_EMIT("addss %%xmm1, %%xmm0")
                __ASM_EMIT("sqrtss %%xmm0, %%xmm0")
                __ASM_EMIT("movss %%xmm0, (%2)")

                // Move pointers
                __ASM_EMIT("add $0x4, %0")
                __ASM_EMIT("add $0x4, %1")
                __ASM_EMIT("add $0x4, %2")

                // Repeat loop
                __ASM_EMIT("dec %3")
                __ASM_EMIT("jnz 1b")
                __ASM_EMIT("2:")

                : "+r" (src_re), "+r"(src_im), "+r"(dst_mod), "+r" (count) :
                : "cc", "memory",
                  "%xmm0", "%xmm1"
            );

            size_t regs     = count / SSE_MULTIPLE;
            count          %= SSE_MULTIPLE;

            if (regs > 0)
            {
                size_t blocks   = regs / 4;
                regs           %= 4;

                // Prefetch data for iteration
                __asm__ __volatile__
                (
                    __ASM_EMIT("prefetchnta  0x00(%0)")
                    __ASM_EMIT("prefetchnta  0x20(%0)")
                    __ASM_EMIT("prefetchnta  0x00(%1)")
                    __ASM_EMIT("prefetchnta  0x20(%1)")
                    : : "r" (src_re), "r" (src_im)
                );

                #define cplx_mod4(l_re, l_im)   \
                { \
                    if (blocks > 0) \
                    { \
                        __asm__ __volatile__ \
                        ( \
                            __ASM_EMIT("1:") \
                            \
                            __ASM_EMIT("prefetchnta  0x40(%0)") \
                            __ASM_EMIT("prefetchnta  0x60(%0)") \
                            __ASM_EMIT("prefetchnta  0x40(%1)") \
                            __ASM_EMIT("prefetchnta  0x60(%1)") \
                            /* Process data */ \
                            __ASM_EMIT(l_re " 0x00(%0), %%xmm0") \
                            __ASM_EMIT(l_re " 0x10(%0), %%xmm1") \
                            __ASM_EMIT(l_re " 0x20(%0), %%xmm2") \
                            __ASM_EMIT(l_re " 0x30(%0), %%xmm3") \
                            __ASM_EMIT(l_im " 0x00(%1), %%xmm4") \
                            __ASM_EMIT(l_im " 0x10(%1), %%xmm5") \
                            __ASM_EMIT(l_im " 0x20(%1), %%xmm6") \
                            __ASM_EMIT(l_im " 0x30(%1), %%xmm7") \
                            \
                            __ASM_EMIT("mulps %%xmm0, %%xmm0") \
                            __ASM_EMIT("mulps %%xmm1, %%xmm1") \
                            __ASM_EMIT("mulps %%xmm2, %%xmm2") \
                            __ASM_EMIT("mulps %%xmm3, %%xmm3") \
                            __ASM_EMIT("mulps %%xmm4, %%xmm4") \
                            __ASM_EMIT("mulps %%xmm5, %%xmm5") \
                            __ASM_EMIT("mulps %%xmm6, %%xmm6") \
                            __ASM_EMIT("mulps %%xmm7, %%xmm7") \
                            \
                            __ASM_EMIT("addps %%xmm4, %%xmm0") \
                            __ASM_EMIT("addps %%xmm5, %%xmm1") \
                            __ASM_EMIT("addps %%xmm6, %%xmm2") \
                            __ASM_EMIT("addps %%xmm7, %%xmm3") \
                            \
                            __ASM_EMIT("sqrtps %%xmm0, %%xmm0") \
                            __ASM_EMIT("sqrtps %%xmm1, %%xmm1") \
                            __ASM_EMIT("sqrtps %%xmm2, %%xmm2") \
                            __ASM_EMIT("sqrtps %%xmm3, %%xmm3") \
                            \
                            __ASM_EMIT(MOVNTPS " %%xmm0, 0x00(%2)") \
                            __ASM_EMIT(MOVNTPS " %%xmm1, 0x10(%2)") \
                            __ASM_EMIT(MOVNTPS " %%xmm2, 0x20(%2)") \
                            __ASM_EMIT(MOVNTPS " %%xmm3, 0x30(%2)") \
                            \
                            /* Move pointers */ \
                            __ASM_EMIT("add $0x40, %0") \
                            __ASM_EMIT("add $0x40, %1") \
                            __ASM_EMIT("add $0x40, %2") \
                            \
                            /* Repeat loop */ \
                            __ASM_EMIT("dec %3") \
                            __ASM_EMIT("jnz 1b") \
                            __ASM_EMIT("2:") \
                            \
                            : "+r" (src_re), "+r"(src_im), "+r"(dst_mod), "+r" (blocks) : \
                            : "cc", "memory", \
                              "%xmm0", "%xmm1", "%xmm2", "%xmm3", \
                              "%xmm4", "%xmm5", "%xmm6", "%xmm7" \
                        ); \
                    } \
                    \
                    if (regs > 0) \
                    { \
                        __asm__ __volatile__ \
                        ( \
                            __ASM_EMIT("1:") \
                            \
                            /* Process data */ \
                            __ASM_EMIT(l_re " (%0), %%xmm0") \
                            __ASM_EMIT(l_im " (%1), %%xmm1") \
                            __ASM_EMIT("mulps %%xmm0, %%xmm0") \
                            __ASM_EMIT("mulps %%xmm1, %%xmm1") \
                            __ASM_EMIT("addps %%xmm1, %%xmm0") \
                            __ASM_EMIT("sqrtps %%xmm0, %%xmm0") \
                            __ASM_EMIT(MOVNTPS " %%xmm0, (%2)") \
                            \
                            /* Move pointers */ \
                            __ASM_EMIT("add $0x10, %0") \
                            __ASM_EMIT("add $0x10, %1") \
                            __ASM_EMIT("add $0x10, %2") \
                            \
                            /* Repeat loop */ \
                            __ASM_EMIT("dec %3") \
                            __ASM_EMIT("jnz 1b") \
                            __ASM_EMIT("2:") \
                            \
                            : "+r" (src_re), "+r"(src_im), "+r"(dst_mod), "+r" (regs) : \
                            : "cc", "memory", \
                              "%xmm0", "%xmm1" \
                        ); \
                    } \
                }

                if (sse_aligned(src_re))
                {
                    if (sse_aligned(src_im))
                        cplx_mod4("movaps", "movaps")
                    else
                        cplx_mod4("movaps", "movups")
                }
                else
                {
                    if (sse_aligned(src_im))
                        cplx_mod4("movups", "movaps")
                    else
                        cplx_mod4("movups", "movups")
                }

                #undef cplx_mod4
            }

            if (count > 0)
            {
                __asm__ __volatile__
                (
                    __ASM_EMIT("1:")

                    // Process data
                    __ASM_EMIT("movss (%0), %%xmm0")
                    __ASM_EMIT("movss (%1), %%xmm1")
                    __ASM_EMIT("mulss %%xmm0, %%xmm0")
                    __ASM_EMIT("mulss %%xmm1, %%xmm1")
                    __ASM_EMIT("addss %%xmm1, %%xmm0")
                    __ASM_EMIT("sqrtss %%xmm0, %%xmm0")
                    __ASM_EMIT("movss %%xmm0, (%2)")

                    // Move pointers
                    __ASM_EMIT("add $0x4, %0")
                    __ASM_EMIT("add $0x4, %1")
                    __ASM_EMIT("add $0x4, %2")

                    // Repeat loop
                    __ASM_EMIT("dec %3")
                    __ASM_EMIT("jnz 1b")
                    __ASM_EMIT("2:")

                    : "+r" (src_re), "+r"(src_im), "+r"(dst_mod), "+r" (count) :
                    : "cc", "memory",
                      "%xmm0", "%xmm1"
                );
            }

            SFENCE;
        }

        // TODO: test it
        void packed_complex_mod(float *dst, const float *src, size_t count)
        {
            #define packed_complex_mod_core(MV_DST, MV_SRC) \
                __ASM_EMIT("sub         $12, %[count]") \
                __ASM_EMIT("jb          2f") \
                /* Do with 12x blocks */ \
                __ASM_EMIT(".align 16") \
                __ASM_EMIT("1:") \
                /* Load data */ \
                __ASM_EMIT(MV_SRC "     0x00(%[src]), %%xmm0")  /* xmm0  = r0 i0 r1 i1 */ \
                __ASM_EMIT(MV_SRC "     0x10(%[src]), %%xmm5")  /* xmm5  = r2 i2 r3 i3 */ \
                __ASM_EMIT(MV_SRC "     0x20(%[src]), %%xmm2")  /* xmm2  = r4 i4 r5 i5 */ \
                __ASM_EMIT(MV_SRC "     0x30(%[src]), %%xmm6")  /* xmm6  = r6 i6 r7 i7 */ \
                __ASM_EMIT(MV_SRC "     0x40(%[src]), %%xmm4")  /* xmm4  = r8 i8 r9 i9 */ \
                __ASM_EMIT(MV_SRC "     0x50(%[src]), %%xmm7")  /* xmm7  = r10 i10 r11 i11 */ \
                __ASM_EMIT("movaps      %%xmm0, %%xmm1")        /* xmm1  = r0 i0 r1 i1 */ \
                __ASM_EMIT("movaps      %%xmm2, %%xmm3")        /* xmm3  = r4 i4 r5 i5 */ \
                /* Pack data and calculate mod */ \
                __ASM_EMIT("shufps      $0x88, %%xmm5, %%xmm0") /* xmm0  = r0 r1 r2 r3 */ \
                __ASM_EMIT("shufps      $0x88, %%xmm6, %%xmm2") /* xmm2  = r4 r5 r6 r7 */ \
                __ASM_EMIT("shufps      $0xdd, %%xmm5, %%xmm1") /* xmm1  = i0 i1 i2 i3 */ \
                __ASM_EMIT("mulps       %%xmm0, %%xmm0")        /* xmm0  = r0*r0 r1*r1 r2*r2 r3*r3 */ \
                __ASM_EMIT("movaps      %%xmm4, %%xmm5")        /* xmm5  = r8 i8 r9 i9 */ \
                __ASM_EMIT("mulps       %%xmm1, %%xmm1")        /* xmm1  = i0*i0 i1*i1 i2*i2 i3*i3 */ \
                __ASM_EMIT("shufps      $0xdd, %%xmm6, %%xmm3") /* xmm3  = i4 i5 i6 i7 */ \
                __ASM_EMIT("mulps       %%xmm2, %%xmm2")    \
                __ASM_EMIT("shufps      $0x88, %%xmm7, %%xmm4") /* xmm4  = r8 r9 r10 r11 */ \
                __ASM_EMIT("mulps       %%xmm3, %%xmm3") \
                __ASM_EMIT("shufps      $0xdd, %%xmm7, %%xmm5") /* xmm5  = i8 i9 i10 i11 */ \
                __ASM_EMIT("mulps       %%xmm4, %%xmm4") \
                __ASM_EMIT("mulps       %%xmm5, %%xmm5") \
                __ASM_EMIT("addps       %%xmm1, %%xmm0") \
                __ASM_EMIT("addps       %%xmm3, %%xmm2") \
                __ASM_EMIT("addps       %%xmm5, %%xmm4") \
                __ASM_EMIT("sqrtps      %%xmm0, %%xmm0") \
                __ASM_EMIT("sqrtps      %%xmm2, %%xmm2") \
                __ASM_EMIT("sqrtps      %%xmm4, %%xmm4") \
                /* Store data */ \
                __ASM_EMIT(MV_DST "     %%xmm0, 0x00(%[dst])") \
                __ASM_EMIT(MV_DST "     %%xmm2, 0x10(%[dst])") \
                __ASM_EMIT(MV_DST "     %%xmm4, 0x20(%[dst])") \
                /* Repeat loop */ \
                __ASM_EMIT("add         $0x60, %[src]") \
                __ASM_EMIT("add         $0x30, %[dst]") \
                __ASM_EMIT("sub         $12, %[count]") \
                __ASM_EMIT("jae         1b") \
                /* 4x iterations */ \
                __ASM_EMIT("2:") \
                __ASM_EMIT("add         $8, %[count]") \
                __ASM_EMIT("jl          4f") \
                __ASM_EMIT("3:") \
                /* Load data */ \
                __ASM_EMIT(MV_SRC "     0x00(%[src]), %%xmm0")  /* xmm0  = r0 i0 r1 i1 */ \
                __ASM_EMIT(MV_SRC "     0x10(%[src]), %%xmm2")  /* xmm2  = r2 i2 r3 i3 */ \
                __ASM_EMIT("movaps      %%xmm0, %%xmm1")        /* xmm1  = r0 i0 r1 i1 */ \
                /* Pack data and calculate mod */ \
                __ASM_EMIT("shufps      $0x88, %%xmm2, %%xmm0") /* xmm0  = r0 r1 r2 r3 */ \
                __ASM_EMIT("shufps      $0xdd, %%xmm2, %%xmm1") /* xmm1  = i0 i1 i2 i3 */ \
                __ASM_EMIT("mulps       %%xmm0, %%xmm0")        /* xmm0  = r0*r0 r1*r1 r2*r2 r3*r3 */ \
                __ASM_EMIT("mulps       %%xmm1, %%xmm1")        /* xmm1  = i0*i0 i1*i1 i2*i2 i3*i3 */ \
                __ASM_EMIT("addps       %%xmm1, %%xmm0") \
                __ASM_EMIT("sqrtps      %%xmm0, %%xmm0") \
                /* Store data */ \
                __ASM_EMIT(MV_DST "     %%xmm0, 0x00(%[dst])") \
                /* Repeat loop */ \
                __ASM_EMIT("add         $0x20, %[src]") \
                __ASM_EMIT("add         $0x10, %[dst]") \
                __ASM_EMIT("sub         $4, %[count]") \
                __ASM_EMIT("jae         3b") \
                /* 1x iterations */ \
                __ASM_EMIT("4:") \
                __ASM_EMIT("add         $4, %[count]") \
                __ASM_EMIT("jle         1000f")

            __asm__ __volatile__
            (
                /* Do block processing */
                __ASM_EMIT("test %[count], %[count]")
                __ASM_EMIT("jz 1000f")
                __ASM_EMIT("test $0x0f, %[dst]")
                __ASM_EMIT("jnz 110f")
                    __ASM_EMIT("test $0x0f, %[src]")
                    __ASM_EMIT("jnz 101f")
                        packed_complex_mod_core("movaps", "movaps")
                        __ASM_EMIT("jmp 5f")
                    __ASM_EMIT("101:")
                        packed_complex_mod_core("movaps", "movups")
                        __ASM_EMIT("jmp 5f")

                __ASM_EMIT("110:")
                    __ASM_EMIT("test $0x0f, %[src]")
                    __ASM_EMIT("jnz 111f")
                        packed_complex_mod_core("movups", "movaps")
                        __ASM_EMIT("jmp 5f")
                    __ASM_EMIT("111:")
                        packed_complex_mod_core("movups", "movups")

                // End of routine
                __ASM_EMIT("5:")
                /* Load data */
                __ASM_EMIT("movss       0x00(%[src]), %%xmm0")  /* xmm0  = r0 */
                __ASM_EMIT("movss       0x04(%[src]), %%xmm1")  /* xmm1  = i0 */
                __ASM_EMIT("mulss       %%xmm0, %%xmm0")        /* xmm0  = r0*r0 */
                __ASM_EMIT("mulss       %%xmm1, %%xmm1")        /* xmm1  = i0*i0 */
                __ASM_EMIT("addps       %%xmm1, %%xmm0")        /* xmm0  = r0*r0 + i0*i0 */
                __ASM_EMIT("sqrtss      %%xmm0, %%xmm0")        /* xmm0  = sqrt(r0*r0 + i0*i0) */
                /* Store data */
                __ASM_EMIT("movss       %%xmm0, 0x00(%[dst])")
                /* Repeat loop */
                __ASM_EMIT("add         $0x8, %[src]")
                __ASM_EMIT("add         $0x4, %[dst]")
                __ASM_EMIT("dec         %[count]")
                __ASM_EMIT("jnz         5b")

                __ASM_EMIT("1000:")

                : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
                :
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );

            #undef packed_complex_mod_core
        }
    }
}

#endif /* CORE_X86_SSE_COMPLEX_H_ */
