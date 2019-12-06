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
    void x64_complex_mul3(float *dst_re, float *dst_im, const float *src1_re, const float *src1_im, const float *src2_re, const float *src2_im, size_t count)
    {
        IF_ARCH_X86_64(size_t off);

        ARCH_X86_64_ASM
        (
            /* Check count */
            __ASM_EMIT("xor         %[off], %[off]")
            __ASM_EMIT("sub         $8, %[count]")
            __ASM_EMIT("jb          3f")

            __ASM_EMIT(".align 16")
            __ASM_EMIT("2:")
            /* Process vectorized data */
            __ASM_EMIT("vmovdqu      0x00(%[src1_re], %[off]), %%ymm0")  /* ymm0 = ar */
            __ASM_EMIT("vmovdqu      0x00(%[src1_im], %[off]), %%ymm2")  /* ymm2 = ai */
            __ASM_EMIT("vmovdqu      0x00(%[src2_re], %[off]), %%ymm4")  /* ymm4 = br */
            __ASM_EMIT("vmovdqu      0x00(%[src2_im], %[off]), %%ymm6")  /* ymm6 = bi */
            /* Calc multiplication */
            __ASM_EMIT("vmulps      %%ymm4, %%ymm0, %%ymm1")             /* ymm1 = ar*br */
            __ASM_EMIT("vmulps      %%ymm6, %%ymm0, %%ymm0")             /* ymm0 = ar*bi */
            __ASM_EMIT("vmulps      %%ymm2, %%ymm6, %%ymm6")             /* ymm6 = ai*bi */
            __ASM_EMIT("vmulps      %%ymm2, %%ymm4, %%ymm4")             /* ymm4 = ai*br */
            __ASM_EMIT("vaddps      %%ymm4, %%ymm0, %%ymm0")             /* ymm0 = ar*bi + ai*br = i */
            __ASM_EMIT("vsubps      %%ymm6, %%ymm1, %%ymm1")             /* ymm1 = ar*br - ai*bi = r */
            /* Store (64-bit code) */
            __ASM_EMIT("vmovups     %%ymm1, 0x00(%[dst_re], %[off])")
            __ASM_EMIT("vmovups     %%ymm0, 0x00(%[dst_im], %[off])")
            /* Repeat loop */
            __ASM_EMIT("add         $0x20, %[off]")
            __ASM_EMIT("sub         $8, %[count]")
            __ASM_EMIT("jae         2b")

            __ASM_EMIT("3:")
            __ASM_EMIT("add         $8, %[count]")
            __ASM_EMIT("jle         5f")

            /* Process scalar data */
            __ASM_EMIT("4:")
            __ASM_EMIT("vmovss      0x00(%[src1_re], %[off]), %%xmm0")  /* ymm0 = ar */
            __ASM_EMIT("vmovss      0x00(%[src1_im], %[off]), %%xmm2")  /* ymm2 = ai */
            __ASM_EMIT("vmovss      0x00(%[src2_re], %[off]), %%xmm4")  /* ymm4 = br */
            __ASM_EMIT("vmovss      0x00(%[src2_im], %[off]), %%xmm6")  /* ymm6 = bi */
            /* Calculate multiplication */
            __ASM_EMIT("vmulss      %%xmm4, %%xmm0, %%xmm1")            /* xmm1 = ar*br */
            __ASM_EMIT("vmulss      %%xmm6, %%xmm0, %%xmm0")            /* xmm0 = ar*bi */
            __ASM_EMIT("vmulss      %%xmm2, %%xmm6, %%xmm6")            /* xmm6 = ai*bi */
            __ASM_EMIT("vmulss      %%xmm2, %%xmm4, %%xmm4")            /* xmm4 = ai*br */
            __ASM_EMIT("vaddss      %%xmm4, %%xmm0, %%xmm0")            /* xmm0 = ar*bi + ai*br = i */
            __ASM_EMIT("vsubss      %%xmm6, %%xmm1, %%xmm1")            /* xmm1 = ar*br - ai*bi = r */
            /* Store */
            __ASM_EMIT("vmovss      %%xmm1, 0x00(%[dst_re], %[off])")
            __ASM_EMIT("vmovss      %%xmm0, 0x00(%[dst_im], %[off])")
            /* Repeat loop */
            __ASM_EMIT("add         $0x04, %[off]")
            __ASM_EMIT("dec         %[count]")
            __ASM_EMIT("jnz         4b")

            /* Exit */
            __ASM_EMIT("5:")

            : [count] "+r" (count),
              [off] "=&r" (off)
            : [dst_re] "r" (dst_re), [dst_im] "r" (dst_im),
              [src1_re] "r" (src1_re), [src1_im] "r" (src1_im),
              [src2_re] "r" (src2_re), [src2_im] "r" (src2_im)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm4", "%xmm5", "%xmm6"
        );
    }

    void x64_complex_mul3_fma3(float *dst_re, float *dst_im, const float *src1_re, const float *src1_im, const float *src2_re, const float *src2_im, size_t count)
    {
        IF_ARCH_X86_64(size_t off);

        ARCH_X86_64_ASM
        (
            /* Check count */
            __ASM_EMIT("xor         %[off], %[off]")
            __ASM_EMIT("sub         $8, %[count]")
            __ASM_EMIT("jb          3f")

            __ASM_EMIT(".align 16")
            __ASM_EMIT("2:")
            /* Process vectorized data */
            __ASM_EMIT("vmovdqu     0x00(%[src1_re], %[off]), %%ymm0")     /* ymm0 = ar */
            __ASM_EMIT("vmovdqu     0x00(%[src1_im], %[off]), %%ymm1")     /* ymm1 = ai */
            __ASM_EMIT("vmovdqu     0x00(%[src2_re], %[off]), %%ymm2")     /* ymm2 = br */
            __ASM_EMIT("vmovdqu     0x00(%[src2_im], %[off]), %%ymm3")     /* ymm3 = bi */
            /* Calculate multiplication */
            __ASM_EMIT("vmulps      %%ymm1, %%ymm2, %%ymm4")            /* ymm4 = ai*br */
            __ASM_EMIT("vmulps      %%ymm1, %%ymm3, %%ymm1")            /* ymm1 = ai*bi */
            __ASM_EMIT("vfmadd132ps %%ymm0, %%ymm4, %%ymm3")            /* ymm3 = ar*bi + ai*br = i */
            __ASM_EMIT("vfmsub132ps %%ymm0, %%ymm1, %%ymm2")            /* ymm2 = ar*br - ai*bi = r */
            /* Store */
            __ASM_EMIT("vmovups     %%ymm2, 0x00(%[dst_re], %[off])")
            __ASM_EMIT("vmovups     %%ymm3, 0x00(%[dst_im], %[off])")
            /* Repeat loop */
            __ASM_EMIT("add         $0x20, %[off]")
            __ASM_EMIT("sub         $8, %[count]")
            __ASM_EMIT("jae         2b")

            __ASM_EMIT("3:")
            __ASM_EMIT("add         $8, %[count]")
            __ASM_EMIT("jle         5f")

            /* Process scalar data */
            __ASM_EMIT("4:")
            __ASM_EMIT("vmovss      0x00(%[src1_re], %[off]), %%xmm0")     /* xmm0 = ar */
            __ASM_EMIT("vmovss      0x00(%[src1_im], %[off]), %%xmm1")     /* xmm1 = ai */
            __ASM_EMIT("vmovss      0x00(%[src2_re], %[off]), %%xmm2")     /* xmm2 = br */
            __ASM_EMIT("vmovss      0x00(%[src2_im], %[off]), %%xmm3")     /* xmm3 = bi */
            /* Calculate multiplication */
            __ASM_EMIT("vmulss      %%xmm1, %%xmm2, %%xmm4")            /* xmm4 = ai*br */
            __ASM_EMIT("vmulss      %%xmm1, %%xmm3, %%xmm1")            /* xmm1 = ai*bi */
            __ASM_EMIT("vfmadd132ss %%xmm0, %%xmm4, %%xmm3")            /* xmm3 = ar*bi + ai*br = i */
            __ASM_EMIT("vfmsub132ss %%xmm0, %%xmm1, %%xmm2")            /* xmm2 = ar*br - ai*bi = r */
            /* Store */
            __ASM_EMIT("vmovss      %%xmm2, 0x00(%[dst_re], %[off])")
            __ASM_EMIT("vmovss      %%xmm3, 0x00(%[dst_im], %[off])")
            /* Repeat loop */
            __ASM_EMIT("add         $0x04, %[off]")
            __ASM_EMIT("dec         %[count]")
            __ASM_EMIT("jnz         4b")

            /* Exit */
            __ASM_EMIT("5:")

            : [count] "+r" (count),
              [off] "=&r" (off)
            : [dst_re] "r" (dst_re), [dst_im] "r" (dst_im),
              [src1_re] "r" (src1_re), [src1_im] "r" (src1_im),
              [src2_re] "r" (src2_re), [src2_im] "r" (src2_im)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm4", "%xmm5", "%xmm6"
        );
    }

}

#endif /* INCLUDE_DSP_ARCH_X86_AVX_COMPLEX_H_ */
