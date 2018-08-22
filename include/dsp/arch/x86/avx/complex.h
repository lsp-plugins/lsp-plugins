/*
 * complex.h
 *
 *  Created on: 22 авг. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_AVX_COMPLEX_H_
#define DSP_ARCH_X86_AVX_COMPLEX_H_

namespace avx
{
    void x64_packed_complex_mul(float *dst, const float *src1, const float *src2, size_t count)
    {
        size_t off;

        __asm__ __volatile__
        (
            /* Check count */
            __ASM_EMIT("xor         %[off], %[off]")
            __ASM_EMIT("sub         $8, %[count]")
            __ASM_EMIT("jb          3f")

            __ASM_EMIT(".align 16")
            __ASM_EMIT("2:")
            /* Process vectorized data */
            __ASM_EMIT("vlddqu      0x00(%[src1], %[off]), %%ymm0")     /* ymm0 = ar0 ai0 ar1 ai1 ar2 ai2 ar3 ai3 */
            __ASM_EMIT("vlddqu      0x20(%[src1], %[off]), %%ymm1")     /* ymm1 = ar4 ai4 ar5 ai5 ar6 ai6 ar7 ai7 */
            __ASM_EMIT("vlddqu      0x00(%[src2], %[off]), %%ymm4")     /* ymm4 = br0 bi0 br1 bi1 br2 bi2 br3 bi3 */
            __ASM_EMIT("vlddqu      0x20(%[src2], %[off]), %%ymm5")     /* ymm5 = br4 bi4 br5 bi5 br6 bi6 br7 bi7 */
            /* Calc multiplication */
            __ASM_EMIT("vmovsldup   %%ymm0, %%ymm2")                    /* ymm2 = ar0 ar0 ar1, ar1 */
            __ASM_EMIT("vmovsldup   %%ymm4, %%ymm6")                    /* ymm6 = ar2 ar2 ar3, ar3 */
            __ASM_EMIT("vmovshdup   %%ymm0, %%ymm0")                    /* ymm0 = ai0 ai0 ai1, ai1 */
            __ASM_EMIT("vmovshdup   %%ymm4, %%ymm4")                    /* ymm4 = ai2 ai2 ai3, ai3 */
            __ASM_EMIT("vmulps      %%ymm1, %%ymm0, %%ymm0")                    /* ymm0 = ai0*br0 ai0*bi0 ai1*br1 ai1*bi1 */
            __ASM_EMIT("vmulps      %%ymm5, %%ymm4, %%ymm4")                    /* ymm4 = ai2*br2 ai2*bi2 ai3*br3 ai3*bi3 */
            __ASM_EMIT("vmulps      %%ymm1, %%ymm2, %%ymm2")                    /* ymm2 = ar0*br0 ar0*bi0 ar1*br1 ar1*bi1 */
            __ASM_EMIT("vmulps      %%ymm5, %%ymm6, %%ymm6")                    /* ymm6 = ar2*br2 ar2*bi2 ar3*br3 ar3*bi3 */
            __ASM_EMIT("vshufps     $0xb1, %%ymm0, %%ymm0, %%ymm0")             /* ymm0 = ai0*bi0 ai0*br0 ai1*bi1 ai1*br1 */
            __ASM_EMIT("vshufps     $0xb1, %%ymm4, %%ymm4, %%ymm4")             /* ymm4 = ai2*bi2 ai2*br2 ai3*bi3 ai3*br3 */
            __ASM_EMIT("vaddsubps   %%ymm0, %%ymm2, %%ymm2")                    /* ymm2 = ar0*br0 - ai0*bi0 ar0*bi0 + ai0*br0 ar1*br1 - ai1*bi1 ar1*bi1 + ai1*br1 */
            __ASM_EMIT("vaddsubps   %%ymm4, %%ymm6, %%ymm6")                    /* ymm6 = ar2*br2 - ai2*bi2 ar2*bi2 + ai2*br2 ar3*br3 - ai3*bi3 ar3*bi3 + ai3*br3 */
            /* Store */
            __ASM_EMIT("vmovdqu     %%ymm2, 0x00(%[dst], %[off])")
            __ASM_EMIT("vmovdqu     %%ymm6, 0x20(%[dst], %[off])")
            /* Repeat loop */
            __ASM_EMIT("add         $0x40, %[off]")
            __ASM_EMIT("sub         $8, %[count]")
            __ASM_EMIT("jae         2b")

            /* Check count again */
            __ASM_EMIT("add         $8, %[count]")
            __ASM_EMIT("jle         5f")

            /* Process scalar data */
            __ASM_EMIT("4:")
            /* Load */
            __ASM_EMIT("vmovss      0x00(%[src1], %[off]), %%xmm0")     /* xmm0 = ar */
            __ASM_EMIT("vmovss      0x04(%[src1], %[off]), %%xmm2")     /* xmm2 = ai */
            __ASM_EMIT("vmovss      0x00(%[src2], %[off]), %%xmm4")     /* xmm4 = br */
            __ASM_EMIT("vmovss      0x04(%[src2], %[off]), %%xmm6")     /* xmm6 = bi */
            /* Calculate multiplication */
            __ASM_EMIT("vmulss      %%xmm4, %%xmm0, %%xmm1")            /* xmm1 = ar*br */
            __ASM_EMIT("vmulss      %%xmm6, %%xmm0, %%xmm0")            /* xmm0 = ar*bi */
            __ASM_EMIT("vmulss      %%xmm2, %%xmm6, %%xmm6")            /* xmm6 = ai*bi */
            __ASM_EMIT("vmulss      %%xmm2, %%xmm4, %%xmm4")            /* xmm4 = ai*br */
            __ASM_EMIT("vaddss      %%xmm4, %%xmm0, %%xmm0")            /* xmm0 = ar*bi + ai*br = i */
            __ASM_EMIT("vsubss      %%xmm6, %%xmm1, %%xmm1")            /* xmm1 = ar*br - ai*bi = r */
            /* Store */
            __ASM_EMIT("vmovss      %%xmm1, 0x00(%[dst], %[off])")
            __ASM_EMIT("vmovss      %%xmm0, 0x04(%[dst], %[off])")
            /* Repeat loop */
            __ASM_EMIT("add         $0x08, %[off]")
            __ASM_EMIT("dec         %[count]")
            __ASM_EMIT("jnz         4b")

            /* Exit */
            __ASM_EMIT("5:")
            __ASM_EMIT("vzeroupper")

            : [dst] "+r" (dst), [src1] "+r" (src1), [src2] "+r" (src2),
              [off] "=&r" (off),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm4", "%xmm5", "%xmm6"
        );
    }

    void x64_packed_complex_mul_fma3(float *dst, const float *src1, const float *src2, size_t count)
    {
        size_t off;

        __asm__ __volatile__
        (
            /* Check count */
            __ASM_EMIT("xor         %[off], %[off]")
            __ASM_EMIT("sub         $8, %[count]")
            __ASM_EMIT("jb          3f")

            __ASM_EMIT(".align 16")
            __ASM_EMIT("2:")
            /* Process vectorized data */
            __ASM_EMIT("vlddqu      0x00(%[src1], %[off]), %%ymm0")     /* ymm0 = ar0 ai0 ar1 ai1 ar2 ai2 ar3 ai3 */
            __ASM_EMIT("vlddqu      0x20(%[src1], %[off]), %%ymm1")     /* ymm1 = ar4 ai4 ar5 ai5 ar6 ai6 ar7 ai7 */
            __ASM_EMIT("vlddqu      0x00(%[src2], %[off]), %%ymm4")     /* ymm4 = br0 bi0 br1 bi1 br2 bi2 br3 bi3 */
            __ASM_EMIT("vlddqu      0x20(%[src2], %[off]), %%ymm5")     /* ymm5 = br4 bi4 br5 bi5 br6 bi6 br7 bi7 */
            /* Calc multiplication */
            __ASM_EMIT("vmovsldup   %%ymm0, %%ymm2")                    /* ymm2 = ar0 ar0 ar1, ar1 */
            __ASM_EMIT("vmovsldup   %%ymm4, %%ymm6")                    /* ymm6 = ar2 ar2 ar3, ar3 */
            __ASM_EMIT("vmovshdup   %%ymm0, %%ymm0")                    /* ymm0 = ai0 ai0 ai1, ai1 */
            __ASM_EMIT("vmovshdup   %%ymm4, %%ymm4")                    /* ymm4 = ai2 ai2 ai3, ai3 */
            __ASM_EMIT("vshufps     $0xb1, %%ymm0, %%ymm0, %%ymm0")             /* ymm0 = ai0*bi0 ai0*br0 ai1*bi1 ai1*br1 */
            __ASM_EMIT("vshufps     $0xb1, %%ymm4, %%ymm4, %%ymm4")             /* ymm4 = ai2*bi2 ai2*br2 ai3*bi3 ai3*br3 */
            __ASM_EMIT("vmulps      %%ymm1, %%ymm0, %%ymm0")                    /* ymm0 = ai0*br0 ai0*bi0 ai1*br1 ai1*bi1 */
            __ASM_EMIT("vmulps      %%ymm5, %%ymm4, %%ymm4")                    /* ymm4 = ai2*br2 ai2*bi2 ai3*br3 ai3*bi3 */
            __ASM_EMIT("vfmaddsub132ps   %%ymm1, %%ymm0, %%ymm2")               /* ymm2 = ar0*br0 - ai0*bi0 ar0*bi0 + ai0*br0 ar1*br1 - ai1*bi1 ar1*bi1 + ai1*br1 */
            __ASM_EMIT("vfmaddsub132ps   %%ymm5, %%ymm4, %%ymm6")               /* ymm6 = ar2*br2 - ai2*bi2 ar2*bi2 + ai2*br2 ar3*br3 - ai3*bi3 ar3*bi3 + ai3*br3 */
            /* Store */
            __ASM_EMIT("vmovdqu     %%ymm2, 0x00(%[dst], %[off])")
            __ASM_EMIT("vmovdqu     %%ymm6, 0x20(%[dst], %[off])")
            /* Repeat loop */
            __ASM_EMIT("add         $0x40, %[off]")
            __ASM_EMIT("sub         $8, %[count]")
            __ASM_EMIT("jae         2b")

            /* Check count again */
            __ASM_EMIT("add         $8, %[count]")
            __ASM_EMIT("jle         5f")

            /* Process scalar data */
            __ASM_EMIT("4:")
            /* Load */
            __ASM_EMIT("vmovss      0x00(%[src1], %[off]), %%xmm0")     /* xmm0 = ar */
            __ASM_EMIT("vmovss      0x04(%[src1], %[off]), %%xmm1")     /* xmm1 = ai */
            __ASM_EMIT("vmovss      0x00(%[src2], %[off]), %%xmm2")     /* xmm2 = br */
            __ASM_EMIT("vmovss      0x04(%[src2], %[off]), %%xmm3")     /* xmm3 = bi */
            /* Calculate multiplication */
            __ASM_EMIT("vmulss      %%xmm1, %%xmm2, %%xmm4")            /* xmm4 = ai*br */
            __ASM_EMIT("vmulss      %%xmm1, %%xmm3, %%xmm5")            /* xmm5 = ai*bi */
            __ASM_EMIT("vfmadd132ss %%xmm0, %%xmm4, %%xmm3")            /* xmm0 = ar*bi + ai*br = i */
            __ASM_EMIT("vfmsub132ss %%xmm2, %%xmm5, %%xmm0")            /* xmm1 = ar*br - ai*bi = r */
            /* Store */
            __ASM_EMIT("vmovss      %%xmm1, 0x00(%[dst], %[off])")
            __ASM_EMIT("vmovss      %%xmm0, 0x04(%[dst], %[off])")
            /* Repeat loop */
            __ASM_EMIT("add         $0x08, %[off]")
            __ASM_EMIT("dec         %[count]")
            __ASM_EMIT("jnz         4b")

            /* Exit */
            __ASM_EMIT("5:")
            __ASM_EMIT("vzeroupper")

            : [dst] "+r" (dst), [src1] "+r" (src1), [src2] "+r" (src2),
              [off] "=&r" (off),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm4", "%xmm5", "%xmm6"
        );
    }
}

#endif /* INCLUDE_DSP_ARCH_X86_AVX_COMPLEX_H_ */
