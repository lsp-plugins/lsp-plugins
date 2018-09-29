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
    void x64_pcomplex_mul3(float *dst, const float *src1, const float *src2, size_t count)
    {
        IF_ARCH_X86_64(size_t off);

        ARCH_X86_64_ASM
        (
            /* Check count */
            __ASM_EMIT("xor         %[off], %[off]")
            __ASM_EMIT("sub         $8, %[count]")
            __ASM_EMIT("jb          2f")

            __ASM_EMIT(".align 16")
            __ASM_EMIT("1:")
            /* Process vectorized data */
            __ASM_EMIT("vlddqu      0x00(%[src1], %[off]), %%ymm0")     /* ymm0 = ar0 ai0 ar1 ai1 ar2 ai2 ar3 ai3 */
            __ASM_EMIT("vlddqu      0x20(%[src1], %[off]), %%ymm4")
            __ASM_EMIT("vlddqu      0x00(%[src2], %[off]), %%ymm1")     /* ymm1 = br0 bi0 br1 bi1 br2 bi2 br3 bi3 */
            __ASM_EMIT("vlddqu      0x20(%[src2], %[off]), %%ymm5")
            /* Calc multiplication */
            __ASM_EMIT("vmovshdup   %%ymm0, %%ymm2")                    /* ymm2 = ai0 ai0 ai1 ai1 ai2 ai2 ai3 ai3 */
            __ASM_EMIT("vmovshdup   %%ymm4, %%ymm6")
            __ASM_EMIT("vmovsldup   %%ymm0, %%ymm0")                    /* ymm0 = ar0 ar0 ar1 ar1 ar2 ar2 ar3 ar3 */
            __ASM_EMIT("vmovsldup   %%ymm4, %%ymm4")
            __ASM_EMIT("vshufps     $0xb1, %%ymm1, %%ymm1, %%ymm3")     /* ymm3 = bi0 br0 bi1 br1 bi2 br2 bi3 br3 */
            __ASM_EMIT("vshufps     $0xb1, %%ymm5, %%ymm5, %%ymm7")
            __ASM_EMIT("vmulps      %%ymm1, %%ymm0, %%ymm0")            /* ymm0 = ar0*br0 ar0*bi0 ... */
            __ASM_EMIT("vmulps      %%ymm5, %%ymm4, %%ymm4")
            __ASM_EMIT("vmulps      %%ymm2, %%ymm3, %%ymm2")            /* ymm2 = ai0*bi0 ai0*br0 ... */
            __ASM_EMIT("vmulps      %%ymm6, %%ymm7, %%ymm6")
            __ASM_EMIT("vaddsubps   %%ymm2, %%ymm0, %%ymm0")            /* ymm0 = ar0*br0-ai0*bi0 ar0*bi0+ai0*br0 ... */
            __ASM_EMIT("vaddsubps   %%ymm6, %%ymm4, %%ymm4")
            /* Store */
            __ASM_EMIT("vmovdqu     %%ymm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("vmovdqu     %%ymm4, 0x20(%[dst], %[off])")
            /* Repeat loop */
            __ASM_EMIT("add         $0x40, %[off]")
            __ASM_EMIT("sub         $8, %[count]")
            __ASM_EMIT("jae         1b")

            /* Check count again */
            __ASM_EMIT("2:")
            __ASM_EMIT("add         $4, %[count]")
            __ASM_EMIT("jl          4f")

            /* Process vectorized data */
            __ASM_EMIT("vlddqu      0x00(%[src1], %[off]), %%ymm0")     /* ymm0 = ar0 ai0 ar1 ai1 ar2 ai2 ar3 ai3 */
            __ASM_EMIT("vlddqu      0x00(%[src2], %[off]), %%ymm1")     /* ymm1 = br0 bi0 br1 bi1 br2 bi2 br3 bi3 */
            /* Calc multiplication */
            __ASM_EMIT("vmovshdup   %%ymm0, %%ymm2")                    /* ymm2 = ai0 ai0 ai1 ai1 ai2 ai2 ai3 ai3 */
            __ASM_EMIT("vmovsldup   %%ymm0, %%ymm0")                    /* ymm0 = ar0 ar0 ar1 ar1 ar2 ar2 ar3 ar3 */
            __ASM_EMIT("vshufps     $0xb1, %%ymm1, %%ymm1, %%ymm3")     /* ymm3 = bi0 br0 bi1 br1 bi2 br2 bi3 br3 */
            __ASM_EMIT("vmulps      %%ymm1, %%ymm0, %%ymm0")            /* ymm0 = ar0*br0 ar0*bi0 ... */
            __ASM_EMIT("vmulps      %%ymm2, %%ymm3, %%ymm2")            /* ymm2 = ai0*bi0 ai0*br0 ... */
            __ASM_EMIT("vaddsubps   %%ymm2, %%ymm0, %%ymm0")            /* ymm0 = ar0*br0-ai0*bi0 ar0*bi0+ai0*br0 ... */
            /* Store */
            __ASM_EMIT("vmovdqu     %%ymm0, 0x00(%[dst], %[off])")
            /* Repeat loop */
            __ASM_EMIT("add         $0x20, %[off]")
            __ASM_EMIT("sub         $4, %[count]")

            /* Process scalar data */
            __ASM_EMIT("4:")
            __ASM_EMIT("add         $4, %[count]")
            __ASM_EMIT("jle         6f")

            __ASM_EMIT("5:")
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
            __ASM_EMIT("jnz         5b")

            /* Exit */
            __ASM_EMIT("6:")
            __ASM_EMIT("vzeroupper")

            : [dst] "+r" (dst), [src1] "+r" (src1), [src2] "+r" (src2),
              [off] "=&r" (off),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm4", "%xmm5", "%xmm6"
        );
    }

    void x64_pcomplex_mul3_fma3(float *dst, const float *src1, const float *src2, size_t count)
    {
        IF_ARCH_X86_64(size_t off);

        ARCH_X86_64_ASM
        (
            /* Check count */
            __ASM_EMIT("xor         %[off], %[off]")
            __ASM_EMIT("sub         $8, %[count]")
            __ASM_EMIT("jb          2f")

            __ASM_EMIT(".align 16")
            __ASM_EMIT("1:")
            /* Process vectorized data */
            __ASM_EMIT("vlddqu      0x00(%[src1], %[off]), %%ymm0")     /* ymm0 = ar0 ai0 ar1 ai1 ar2 ai2 ar3 ai3 */
            __ASM_EMIT("vlddqu      0x20(%[src1], %[off]), %%ymm4")
            __ASM_EMIT("vlddqu      0x00(%[src2], %[off]), %%ymm1")     /* ymm1 = br0 bi0 br1 bi1 br2 bi2 br3 bi3 */
            __ASM_EMIT("vlddqu      0x20(%[src2], %[off]), %%ymm5")
            /* Calc multiplication */
            __ASM_EMIT("vmovshdup   %%ymm0, %%ymm2")                    /* ymm2 = ai0 ai0 ai1 ai1 ai2 ai2 ai3 ai3 */
            __ASM_EMIT("vmovshdup   %%ymm4, %%ymm6")
            __ASM_EMIT("vmovsldup   %%ymm0, %%ymm0")                    /* ymm0 = ar0 ar0 ar1 ar1 ar2 ar2 ar3 ar3 */
            __ASM_EMIT("vmovsldup   %%ymm4, %%ymm4")
            __ASM_EMIT("vshufps     $0xb1, %%ymm1, %%ymm1, %%ymm3")     /* ymm3 = bi0 br0 ... */
            __ASM_EMIT("vshufps     $0xb1, %%ymm5, %%ymm5, %%ymm7")
            __ASM_EMIT("vmulps      %%ymm2, %%ymm3, %%ymm2")            /* ymm2 = ai0*bi0 ai0*br0 ... */
            __ASM_EMIT("vmulps      %%ymm6, %%ymm7, %%ymm6")
            __ASM_EMIT("vfmaddsub132ps %%ymm1, %%ymm2, %%ymm0")         /* ymm0 = ar0*br0-ai0*bi0 ar0*bi0+ai0*br0 ... */
            __ASM_EMIT("vfmaddsub132ps %%ymm5, %%ymm6, %%ymm4")
            /* Store */
            __ASM_EMIT("vmovdqu     %%ymm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("vmovdqu     %%ymm4, 0x20(%[dst], %[off])")
            /* Repeat loop */
            __ASM_EMIT("add         $0x40, %[off]")
            __ASM_EMIT("sub         $8, %[count]")
            __ASM_EMIT("jae         1b")

            /* Check count again */
            __ASM_EMIT("2:")
            __ASM_EMIT("add         $4, %[count]")
            __ASM_EMIT("jl          4f")

            /* Process vectorized data */
            __ASM_EMIT("vlddqu      0x00(%[src1], %[off]), %%ymm0")     /* ymm0 = ar0 ai0 ar1 ai1 ar2 ai2 ar3 ai3 */
            __ASM_EMIT("vlddqu      0x00(%[src2], %[off]), %%ymm1")     /* ymm1 = br0 bi0 br1 bi1 br2 bi2 br3 bi3 */
            /* Calc multiplication */
            __ASM_EMIT("vmovshdup   %%ymm0, %%ymm2")                    /* ymm2 = ai0 ai0 ai1 ai1 ai2 ai2 ai3 ai3 */
            __ASM_EMIT("vmovsldup   %%ymm0, %%ymm0")                    /* ymm0 = ar0 ar0 ar1 ar1 ar2 ar2 ar3 ar3 */
            __ASM_EMIT("vshufps     $0xb1, %%ymm1, %%ymm1, %%ymm3")     /* ymm3 = bi0 br0 ... */
            __ASM_EMIT("vmulps      %%ymm2, %%ymm3, %%ymm2")            /* ymm2 = ai0*bi0 ai0*br0 ... */
            __ASM_EMIT("vfmaddsub132ps %%ymm1, %%ymm2, %%ymm0")            /* ymm0 = ar0*br0-ai0*bi0 ar0*bi0+ai0*br0 ... */
            /* Store */
            __ASM_EMIT("vmovdqu     %%ymm0, 0x00(%[dst], %[off])")
            /* Repeat loop */
            __ASM_EMIT("add         $0x20, %[off]")
            __ASM_EMIT("sub         $4, %[count]")

            /* Process scalar data */
            __ASM_EMIT("4:")
            __ASM_EMIT("add         $4, %[count]")
            __ASM_EMIT("jle         6f")

            __ASM_EMIT("5:")
            /* Load */
            __ASM_EMIT("vmovss      0x00(%[src1], %[off]), %%xmm0")     /* xmm0 = ar */
            __ASM_EMIT("vmovss      0x04(%[src1], %[off]), %%xmm1")     /* xmm1 = ai */
            __ASM_EMIT("vmovss      0x00(%[src2], %[off]), %%xmm2")     /* xmm2 = br */
            __ASM_EMIT("vmovss      0x04(%[src2], %[off]), %%xmm3")     /* xmm3 = bi */
            /* Calculate multiplication */
            __ASM_EMIT("vmulss      %%xmm1, %%xmm2, %%xmm4")            /* xmm4 = ai*br */
            __ASM_EMIT("vmulss      %%xmm1, %%xmm3, %%xmm1")            /* xmm1 = ai*bi */
            __ASM_EMIT("vfmadd132ss %%xmm0, %%xmm4, %%xmm3")            /* xmm3 = ar*bi + ai*br = i */
            __ASM_EMIT("vfmsub132ss %%xmm0, %%xmm1, %%xmm2")            /* xmm2 = ar*br - ai*bi = r */
            /* Store */
            __ASM_EMIT("vmovss      %%xmm2, 0x00(%[dst], %[off])")
            __ASM_EMIT("vmovss      %%xmm3, 0x04(%[dst], %[off])")
            /* Repeat loop */
            __ASM_EMIT("add         $0x08, %[off]")
            __ASM_EMIT("dec         %[count]")
            __ASM_EMIT("jnz         5b")

            /* Exit */
            __ASM_EMIT("6:")
            __ASM_EMIT("vzeroupper")

            : [dst] "+r" (dst), [src1] "+r" (src1), [src2] "+r" (src2),
              [off] "=&r" (off),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm4", "%xmm5", "%xmm6"
        );
    }

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
            __ASM_EMIT("vzeroupper")

            : [dst_re] "+r" (dst_re), [dst_im] "+r" (dst_im),
              [src1_re] "+r" (src1_re), [src1_im] "+r" (src1_im),
              [src2_re] "+r" (src2_re), [src2_im] "+r" (src2_im),
              [count] "+r" (count),
              [off] "=&r" (off)
            :
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
            __ASM_EMIT("vzeroupper")

            : [dst_re] "+r" (dst_re), [dst_im] "+r" (dst_im),
              [src1_re] "+r" (src1_re), [src1_im] "+r" (src1_im),
              [src2_re] "+r" (src2_re), [src2_im] "+r" (src2_im),
              [count] "+r" (count),
              [off] "=&r" (off)
            :
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm4", "%xmm5", "%xmm6"
        );
    }

    void x64_pcomplex_mod(float *dst, const float *src, size_t count)
    {
        IF_ARCH_X86_64(size_t off);

        ARCH_X86_64_ASM
        (
            __ASM_EMIT("xor             %[off], %[off]")
            __ASM_EMIT("sub             $32, %[count]")
            __ASM_EMIT("jb              2f")

            // 32x blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("vlddqu          0x000(%[src], %[off], 2), %%ymm0")
            __ASM_EMIT("vlddqu          0x020(%[src], %[off], 2), %%ymm1")
            __ASM_EMIT("vlddqu          0x040(%[src], %[off], 2), %%ymm2")
            __ASM_EMIT("vlddqu          0x060(%[src], %[off], 2), %%ymm3")
            __ASM_EMIT("vlddqu          0x080(%[src], %[off], 2), %%ymm4")
            __ASM_EMIT("vlddqu          0x0a0(%[src], %[off], 2), %%ymm5")
            __ASM_EMIT("vlddqu          0x0c0(%[src], %[off], 2), %%ymm6")
            __ASM_EMIT("vlddqu          0x0e0(%[src], %[off], 2), %%ymm7")
            __ASM_EMIT("vextractf128    $0x01, %%ymm0, %%xmm8")
            __ASM_EMIT("vextractf128    $0x01, %%ymm2, %%xmm10")
            __ASM_EMIT("vextractf128    $0x01, %%ymm4, %%xmm12")
            __ASM_EMIT("vextractf128    $0x01, %%ymm6, %%xmm14")
            __ASM_EMIT("vinsertf128     $0x01, %%xmm1, %%ymm0, %%ymm0")
            __ASM_EMIT("vinsertf128     $0x01, %%xmm3, %%ymm2, %%ymm2")
            __ASM_EMIT("vinsertf128     $0x01, %%xmm5, %%ymm4, %%ymm4")
            __ASM_EMIT("vinsertf128     $0x01, %%xmm7, %%ymm6, %%ymm6")
            __ASM_EMIT("vinsertf128     $0x00, %%xmm8,  %%ymm1, %%ymm1")
            __ASM_EMIT("vinsertf128     $0x00, %%xmm10, %%ymm3, %%ymm3")
            __ASM_EMIT("vinsertf128     $0x00, %%xmm12, %%ymm5, %%ymm5")
            __ASM_EMIT("vinsertf128     $0x00, %%xmm14, %%ymm7, %%ymm7")
            __ASM_EMIT("vmulps          %%ymm0, %%ymm0, %%ymm0")
            __ASM_EMIT("vmulps          %%ymm1, %%ymm1, %%ymm1")
            __ASM_EMIT("vmulps          %%ymm2, %%ymm2, %%ymm2")
            __ASM_EMIT("vmulps          %%ymm3, %%ymm3, %%ymm3")
            __ASM_EMIT("vmulps          %%ymm4, %%ymm4, %%ymm4")
            __ASM_EMIT("vmulps          %%ymm5, %%ymm5, %%ymm5")
            __ASM_EMIT("vmulps          %%ymm6, %%ymm6, %%ymm6")
            __ASM_EMIT("vmulps          %%ymm7, %%ymm7, %%ymm7")
            __ASM_EMIT("vhaddps         %%ymm1, %%ymm0, %%ymm0")
            __ASM_EMIT("vhaddps         %%ymm3, %%ymm2, %%ymm2")
            __ASM_EMIT("vhaddps         %%ymm5, %%ymm4, %%ymm4")
            __ASM_EMIT("vhaddps         %%ymm7, %%ymm6, %%ymm6")
            __ASM_EMIT("vsqrtps         %%ymm0, %%ymm0")
            __ASM_EMIT("vsqrtps         %%ymm2, %%ymm2")
            __ASM_EMIT("vsqrtps         %%ymm4, %%ymm4")
            __ASM_EMIT("vsqrtps         %%ymm6, %%ymm6")
            __ASM_EMIT("vmovdqu         %%ymm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("vmovdqu         %%ymm2, 0x20(%[dst], %[off])")
            __ASM_EMIT("vmovdqu         %%ymm4, 0x40(%[dst], %[off])")
            __ASM_EMIT("vmovdqu         %%ymm6, 0x60(%[dst], %[off])")
            __ASM_EMIT("add             $0x80, %[off]")
            __ASM_EMIT("sub             $32, %[count]")
            __ASM_EMIT("jae             1b")

            // 16x blocks
            __ASM_EMIT("2:")
            __ASM_EMIT("add             $16, %[count]")
            __ASM_EMIT("jl              4f")
            __ASM_EMIT("vlddqu          0x000(%[src], %[off], 2), %%ymm0")
            __ASM_EMIT("vlddqu          0x020(%[src], %[off], 2), %%ymm1")
            __ASM_EMIT("vlddqu          0x040(%[src], %[off], 2), %%ymm2")
            __ASM_EMIT("vlddqu          0x060(%[src], %[off], 2), %%ymm3")
            __ASM_EMIT("vextractf128    $0x01, %%ymm0, %%xmm4")
            __ASM_EMIT("vextractf128    $0x01, %%ymm2, %%xmm6")
            __ASM_EMIT("vinsertf128     $0x01, %%xmm1, %%ymm0, %%ymm0")
            __ASM_EMIT("vinsertf128     $0x01, %%xmm3, %%ymm2, %%ymm2")
            __ASM_EMIT("vinsertf128     $0x00, %%xmm4, %%ymm1, %%ymm1")
            __ASM_EMIT("vinsertf128     $0x00, %%xmm6, %%ymm3, %%ymm3")
            __ASM_EMIT("vmulps          %%ymm0, %%ymm0, %%ymm0")
            __ASM_EMIT("vmulps          %%ymm1, %%ymm1, %%ymm1")
            __ASM_EMIT("vmulps          %%ymm2, %%ymm2, %%ymm2")
            __ASM_EMIT("vmulps          %%ymm3, %%ymm3, %%ymm3")
            __ASM_EMIT("vhaddps         %%ymm1, %%ymm0, %%ymm0")
            __ASM_EMIT("vhaddps         %%ymm3, %%ymm2, %%ymm2")
            __ASM_EMIT("vsqrtps         %%ymm0, %%ymm0")
            __ASM_EMIT("vsqrtps         %%ymm2, %%ymm2")
            __ASM_EMIT("vmovdqu         %%ymm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("vmovdqu         %%ymm2, 0x20(%[dst], %[off])")
            __ASM_EMIT("sub             $16, %[count]")
            __ASM_EMIT("add             $0x40, %[off]")

            // 4x blocks
            __ASM_EMIT("4:")
            __ASM_EMIT("add             $12, %[count]")
            __ASM_EMIT("jl              6f")
            __ASM_EMIT("5:")
            __ASM_EMIT("vlddqu          0x00(%[src], %[off], 2), %%xmm0")
            __ASM_EMIT("vlddqu          0x10(%[src], %[off], 2), %%xmm1")
            __ASM_EMIT("vmulps          %%xmm0, %%xmm0, %%xmm0")
            __ASM_EMIT("vmulps          %%xmm1, %%xmm1, %%xmm1")
            __ASM_EMIT("vhaddps         %%xmm1, %%xmm0, %%xmm0")
            __ASM_EMIT("vsqrtps         %%xmm0, %%xmm0")
            __ASM_EMIT("vmovdqu         %%xmm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("add             $0x10, %[off]")
            __ASM_EMIT("sub             $4, %[count]")
            __ASM_EMIT("jge             5b")

            // 1x blocks
            __ASM_EMIT("6:")
            __ASM_EMIT("add             $3, %[count]")
            __ASM_EMIT("jl              8f")
            __ASM_EMIT("7:")
            __ASM_EMIT("vmovss          0x00(%[src], %[off], 2), %%xmm0")
            __ASM_EMIT("vmovss          0x04(%[src], %[off], 2), %%xmm1")
            __ASM_EMIT("vmulss          %%xmm0, %%xmm0, %%xmm0")
            __ASM_EMIT("vmulss          %%xmm1, %%xmm1, %%xmm1")
            __ASM_EMIT("vaddss          %%xmm1, %%xmm0, %%xmm0")
            __ASM_EMIT("vsqrtss         %%xmm0, %%xmm1, %%xmm0")
            __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("add             $0x4, %[off]")
            __ASM_EMIT("dec             %[count]")
            __ASM_EMIT("jge             7b")

            // End
            __ASM_EMIT("8:")
            __ASM_EMIT("vzeroupper")

            : [dst] "+r" (dst), [src] "+r" (src),
              [count] "+r" (count), [off] "=&r" (off)
            :
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5", "%xmm6", "%xmm7",
              "%xmm8", "%xmm9", "%xmm10", "%xmm12", "%xmm13", "%xmm14", "%xmm15"
        );
    }
}

#endif /* INCLUDE_DSP_ARCH_X86_AVX_COMPLEX_H_ */
