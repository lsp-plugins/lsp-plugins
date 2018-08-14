/*
 * complex.h
 *
 *  Created on: 17 февр. 2017 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_SSE3_COMPLEX_H_
#define DSP_ARCH_X86_SSE3_COMPLEX_H_

namespace sse3
{
    void packed_complex_mul(float *dst, const float *src1, const float *src2, size_t count)
    {
        #define complex_core(MV_DST, MV_SRC1, MV_SRC2) \
            __ASM_EMIT("2:") \
            /* Process vectorized data */ \
            __ASM_EMIT(MV_SRC1 "    0x00(%[src1]), %%xmm0")     /* xmm0 = ar0 ai0 ar1 ai1 */ \
            __ASM_EMIT(MV_SRC1 "    0x10(%[src1]), %%xmm4")     /* xmm4 = ar2 ai2 ar3 ai3 */ \
            __ASM_EMIT(MV_SRC2 "    0x00(%[src2]), %%xmm1")     /* xmm1 = br0 bi0 br1 bi1 */ \
            __ASM_EMIT(MV_SRC2 "    0x10(%[src2]), %%xmm5")     /* xmm5 = br2 bi2 br3 bi3 */ \
            /* Calc multiplication */ \
            __ASM_EMIT("movsldup    %%xmm0, %%xmm2")            /* xmm2 = ar0 ar0 ar1, ar1 */ \
            __ASM_EMIT("movsldup    %%xmm4, %%xmm6")            /* xmm6 = ar2 ar2 ar3, ar3 */ \
            __ASM_EMIT("movshdup    %%xmm0, %%xmm0")            /* xmm0 = ai0 ai0 ai1, ai1 */ \
            __ASM_EMIT("movshdup    %%xmm4, %%xmm4")            /* xmm4 = ai2 ai2 ai3, ai3 */ \
            __ASM_EMIT("mulps       %%xmm1, %%xmm0")            /* xmm0 = ai0*br0 ai0*bi0 ai1*br1 ai1*bi1 */ \
            __ASM_EMIT("mulps       %%xmm5, %%xmm4")            /* xmm4 = ai2*br2 ai2*bi2 ai3*br3 ai3*bi3 */ \
            __ASM_EMIT("mulps       %%xmm1, %%xmm2")            /* xmm2 = ar0*br0 ar0*bi0 ar1*br1 ar1*bi1 */ \
            __ASM_EMIT("mulps       %%xmm5, %%xmm6")            /* xmm6 = ar2*br2 ar2*bi2 ar3*br3 ar3*bi3 */ \
            __ASM_EMIT("shufps      $0xb1, %%xmm0, %%xmm0")     /* xmm0 = ai0*bi0 ai0*br0 ai1*bi1 ai1*br1 */ \
            __ASM_EMIT("shufps      $0xb1, %%xmm4, %%xmm4")     /* xmm4 = ai2*bi2 ai2*br2 ai3*bi3 ai3*br3 */ \
            __ASM_EMIT("addsubps    %%xmm0, %%xmm2")            /* xmm2 = ar0*br0 - ai0*bi0 ar0*bi0 + ai0*br0 ar1*br1 - ai1*bi1 ar1*bi1 + ai1*br1 */ \
            __ASM_EMIT("addsubps    %%xmm4, %%xmm6")            /* xmm6 = ar2*br2 - ai2*bi2 ar2*bi2 + ai2*br2 ar3*br3 - ai3*bi3 ar3*bi3 + ai3*br3 */ \
            /* Store */ \
            __ASM_EMIT(MV_DST "     %%xmm2, 0x00(%[dst])") \
            __ASM_EMIT(MV_DST "     %%xmm6, 0x10(%[dst])") \
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

#ifdef ARCH_X86_64
    void x64_packed_complex_mul(float *dst, const float *src1, const float *src2, size_t count)
    {
        #define complex_core(MV_DST, MV_SRC1, MV_SRC2) \
            __ASM_EMIT("cmp         $8, %[count]") \
            __ASM_EMIT("jb          3f") \
            __ASM_EMIT(".align 16") \
            __ASM_EMIT("2:") \
            /* Process vectorized data */ \
            __ASM_EMIT(MV_SRC1 "    0x00(%[src1]), %%xmm0")     /* xmm0 = ar0 ai0 ar1 ai1 */ \
            __ASM_EMIT(MV_SRC1 "    0x10(%[src1]), %%xmm4")     /* xmm4 = ar2 ai2 ar3 ai3 */ \
            __ASM_EMIT(MV_SRC1 "    0x20(%[src1]), %%xmm8") \
            __ASM_EMIT(MV_SRC1 "    0x30(%[src1]), %%xmm12") \
            __ASM_EMIT(MV_SRC2 "    0x00(%[src2]), %%xmm1")     /* xmm1 = br0 bi0 br1 bi1 */ \
            __ASM_EMIT(MV_SRC2 "    0x10(%[src2]), %%xmm5")     /* xmm5 = br2 bi2 br3 bi3 */ \
            __ASM_EMIT(MV_SRC2 "    0x20(%[src2]), %%xmm9") \
            __ASM_EMIT(MV_SRC2 "    0x30(%[src2]), %%xmm13") \
            /* Calc multiplication */ \
            __ASM_EMIT("movsldup    %%xmm0, %%xmm2")            /* xmm2 = ar0 ar0 ar1, ar1 */ \
            __ASM_EMIT("movsldup    %%xmm4, %%xmm6")            /* xmm6 = ar2 ar2 ar3, ar3 */ \
            __ASM_EMIT("movsldup    %%xmm8, %%xmm10") \
            __ASM_EMIT("movsldup    %%xmm12, %%xmm14") \
            __ASM_EMIT("movshdup    %%xmm0, %%xmm0")            /* xmm0 = ai0 ai0 ai1, ai1 */ \
            __ASM_EMIT("movshdup    %%xmm4, %%xmm4")            /* xmm4 = ai2 ai2 ai3, ai3 */ \
            __ASM_EMIT("movshdup    %%xmm8, %%xmm8") \
            __ASM_EMIT("movshdup    %%xmm12, %%xmm12") \
            __ASM_EMIT("mulps       %%xmm1, %%xmm0")            /* xmm0 = ai0*br0 ai0*bi0 ai1*br1 ai1*bi1 */ \
            __ASM_EMIT("mulps       %%xmm5, %%xmm4")            /* xmm4 = ai2*br2 ai2*bi2 ai3*br3 ai3*bi3 */ \
            __ASM_EMIT("mulps       %%xmm9, %%xmm8") \
            __ASM_EMIT("mulps       %%xmm13, %%xmm12") \
            __ASM_EMIT("mulps       %%xmm1, %%xmm2")            /* xmm2 = ar0*br0 ar0*bi0 ar1*br1 ar1*bi1 */ \
            __ASM_EMIT("mulps       %%xmm5, %%xmm6")            /* xmm6 = ar2*br2 ar2*bi2 ar3*br3 ar3*bi3 */ \
            __ASM_EMIT("mulps       %%xmm9, %%xmm10") \
            __ASM_EMIT("mulps       %%xmm13, %%xmm14") \
            __ASM_EMIT("shufps      $0xb1, %%xmm0, %%xmm0")     /* xmm0 = ai0*bi0 ai0*br0 ai1*bi1 ai1*br1 */ \
            __ASM_EMIT("shufps      $0xb1, %%xmm4, %%xmm4")     /* xmm4 = ai2*bi2 ai2*br2 ai3*bi3 ai3*br3 */ \
            __ASM_EMIT("shufps      $0xb1, %%xmm8, %%xmm8") \
            __ASM_EMIT("shufps      $0xb1, %%xmm12, %%xmm12") \
            __ASM_EMIT("addsubps    %%xmm0, %%xmm2")            /* xmm2 = ar0*br0 - ai0*bi0 ar0*bi0 + ai0*br0 ar1*br1 - ai1*bi1 ar1*bi1 + ai1*br1 */ \
            __ASM_EMIT("addsubps    %%xmm4, %%xmm6")            /* xmm6 = ar2*br2 - ai2*bi2 ar2*bi2 + ai2*br2 ar3*br3 - ai3*bi3 ar3*bi3 + ai3*br3 */ \
            __ASM_EMIT("addsubps    %%xmm8, %%xmm10") \
            __ASM_EMIT("addsubps    %%xmm12, %%xmm14") \
            /* Store */ \
            __ASM_EMIT(MV_DST "     %%xmm2, 0x00(%[dst])") \
            __ASM_EMIT(MV_DST "     %%xmm6, 0x10(%[dst])") \
            __ASM_EMIT(MV_DST "     %%xmm10, 0x20(%[dst])") \
            __ASM_EMIT(MV_DST "     %%xmm14, 0x30(%[dst])") \
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
            __ASM_EMIT(MV_SRC1 "    0x00(%[src1]), %%xmm0")     /* xmm0 = ar0 ai0 ar1 ai1 */ \
            __ASM_EMIT(MV_SRC1 "    0x10(%[src1]), %%xmm4")     /* xmm4 = ar2 ai2 ar3 ai3 */ \
            __ASM_EMIT(MV_SRC2 "    0x00(%[src2]), %%xmm1")     /* xmm1 = br0 bi0 br1 bi1 */ \
            __ASM_EMIT(MV_SRC2 "    0x10(%[src2]), %%xmm5")     /* xmm5 = br2 bi2 br3 bi3 */ \
            /* Calc multiplication */ \
            __ASM_EMIT("movsldup    %%xmm0, %%xmm2")            /* xmm2 = ar0 ar0 ar1, ar1 */ \
            __ASM_EMIT("movsldup    %%xmm4, %%xmm6")            /* xmm6 = ar2 ar2 ar3, ar3 */ \
            __ASM_EMIT("movshdup    %%xmm0, %%xmm0")            /* xmm0 = ai0 ai0 ai1, ai1 */ \
            __ASM_EMIT("movshdup    %%xmm4, %%xmm4")            /* xmm4 = ai2 ai2 ai3, ai3 */ \
            __ASM_EMIT("mulps       %%xmm1, %%xmm0")            /* xmm0 = ai0*br0 ai0*bi0 ai1*br1 ai1*bi1 */ \
            __ASM_EMIT("mulps       %%xmm5, %%xmm4")            /* xmm4 = ai2*br2 ai2*bi2 ai3*br3 ai3*bi3 */ \
            __ASM_EMIT("mulps       %%xmm1, %%xmm2")            /* xmm2 = ar0*br0 ar0*bi0 ar1*br1 ar1*bi1 */ \
            __ASM_EMIT("mulps       %%xmm5, %%xmm6")            /* xmm6 = ar2*br2 ar2*bi2 ar3*br3 ar3*bi3 */ \
            __ASM_EMIT("shufps      $0xb1, %%xmm0, %%xmm0")     /* xmm0 = ai0*bi0 ai0*br0 ai1*bi1 ai1*br1 */ \
            __ASM_EMIT("shufps      $0xb1, %%xmm4, %%xmm4")     /* xmm4 = ai2*bi2 ai2*br2 ai3*bi3 ai3*br3 */ \
            __ASM_EMIT("addsubps    %%xmm0, %%xmm2")            /* xmm2 = ar0*br0 - ai0*bi0 ar0*bi0 + ai0*br0 ar1*br1 - ai1*bi1 ar1*bi1 + ai1*br1 */ \
            __ASM_EMIT("addsubps    %%xmm4, %%xmm6")            /* xmm6 = ar2*br2 - ai2*bi2 ar2*bi2 + ai2*br2 ar3*br3 - ai3*bi3 ar3*bi3 + ai3*br3 */ \
            /* Store */ \
            __ASM_EMIT(MV_DST "     %%xmm2, 0x00(%[dst])") \
            __ASM_EMIT(MV_DST "     %%xmm6, 0x10(%[dst])") \
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


#endif /* DSP_ARCH_X86_SSE3_COMPLEX_H_ */
