/*
 * normalize.h
 *
 *  Created on: 23 дек. 2019 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_AVX2_FFT_NORMALIZE_H_
#define DSP_ARCH_X86_AVX2_FFT_NORMALIZE_H_

#ifndef DSP_ARCH_X86_AVX2_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_AVX_IMPL */

namespace avx2
{
    void normalize_fft3(float *dre, float *dim, const float *re, const float *im, size_t rank)
    {
        IF_ARCH_X86(
            float k = 1.0f/(1 << rank);
            size_t count = 1 << rank, off = 0;
        );
        ARCH_X86_ASM(
            // x16 blocks
            __ASM_EMIT  ("vbroadcastss  %%xmm0, %%ymm0")                 // ymm0   = k
            __ASM_EMIT32("subl          $16, %[count]")
            __ASM_EMIT64("sub           $16, %[count]")
            __ASM_EMIT  ("vmovaps       %%ymm0, %%ymm1")
            __ASM_EMIT  ("jb            2f")
            __ASM_EMIT  ("1:")
            __ASM_EMIT  ("vmulps        0x00(%[s_re], %[off]), %%ymm0, %%ymm4")
            __ASM_EMIT  ("vmulps        0x20(%[s_re], %[off]), %%ymm1, %%ymm5")
            __ASM_EMIT  ("vmulps        0x00(%[s_im], %[off]), %%ymm0, %%ymm6")
            __ASM_EMIT  ("vmulps        0x20(%[s_im], %[off]), %%ymm1, %%ymm7")
            __ASM_EMIT  ("vmovups       %%ymm4, 0x00(%[d_re], %[off])")
            __ASM_EMIT  ("vmovups       %%ymm5, 0x20(%[d_re], %[off])")
            __ASM_EMIT  ("vmovups       %%ymm6, 0x00(%[d_im], %[off])")
            __ASM_EMIT  ("vmovups       %%ymm7, 0x20(%[d_im], %[off])")
            __ASM_EMIT  ("add           $0x40, %[off]")
            __ASM_EMIT32("subl          $16, %[count]")
            __ASM_EMIT64("sub           $16, %[count]")
            __ASM_EMIT  ("jae           1b")
            __ASM_EMIT  ("2:")
            // x8 block
            __ASM_EMIT32("addl          $8, %[count]")
            __ASM_EMIT64("add           $8, %[count]")
            __ASM_EMIT  ("jl            4f")
            __ASM_EMIT  ("vmulps        0x00(%[s_re], %[off]), %%ymm0, %%ymm4")
            __ASM_EMIT  ("vmulps        0x00(%[s_im], %[off]), %%ymm1, %%ymm6")
            __ASM_EMIT  ("vmovups       %%ymm4, 0x00(%[d_re], %[off])")
            __ASM_EMIT  ("vmovups       %%ymm6, 0x00(%[d_im], %[off])")
            __ASM_EMIT  ("4:")
            : [off] "+r" (off), [count] __ASM_ARG_RW(count),
              [k] "+Yz" (k)
            : [s_re] "r" (re), [s_im] "r" (im),
              [d_re] "r" (dre), [d_im] "r" (dim)
            : "cc", "memory",
              "%xmm1",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void normalize_fft2(float *re, float *im, size_t rank)
    {
        IF_ARCH_X86(
            float k = 1.0f/(1 << rank);
            size_t count = 1 << rank, off = 0;
        );
        ARCH_X86_ASM(
            // x16 blocks
            __ASM_EMIT  ("vbroadcastss  %%xmm0, %%ymm0")                 // ymm0   = k
            __ASM_EMIT32("subl          $16, %[count]")
            __ASM_EMIT64("sub           $16, %[count]")
            __ASM_EMIT  ("vmovaps       %%ymm0, %%ymm1")
            __ASM_EMIT  ("jb            2f")
            __ASM_EMIT  ("1:")
            __ASM_EMIT  ("vmulps        0x00(%[d_re], %[off]), %%ymm0, %%ymm4")
            __ASM_EMIT  ("vmulps        0x20(%[d_re], %[off]), %%ymm1, %%ymm5")
            __ASM_EMIT  ("vmulps        0x00(%[d_im], %[off]), %%ymm0, %%ymm6")
            __ASM_EMIT  ("vmulps        0x20(%[d_im], %[off]), %%ymm1, %%ymm7")
            __ASM_EMIT  ("vmovups       %%ymm4, 0x00(%[d_re], %[off])")
            __ASM_EMIT  ("vmovups       %%ymm5, 0x20(%[d_re], %[off])")
            __ASM_EMIT  ("vmovups       %%ymm6, 0x00(%[d_im], %[off])")
            __ASM_EMIT  ("vmovups       %%ymm7, 0x20(%[d_im], %[off])")
            __ASM_EMIT  ("add           $0x40, %[off]")
            __ASM_EMIT32("subl          $16, %[count]")
            __ASM_EMIT64("sub           $16, %[count]")
            __ASM_EMIT  ("jae           1b")
            __ASM_EMIT  ("2:")
            // x8 block
            __ASM_EMIT32("addl          $8, %[count]")
            __ASM_EMIT64("add           $8, %[count]")
            __ASM_EMIT  ("jl            4f")
            __ASM_EMIT  ("vmulps        0x00(%[d_re], %[off]), %%ymm0, %%ymm4")
            __ASM_EMIT  ("vmulps        0x00(%[d_im], %[off]), %%ymm1, %%ymm6")
            __ASM_EMIT  ("vmovups       %%ymm4, 0x00(%[d_re], %[off])")
            __ASM_EMIT  ("vmovups       %%ymm6, 0x00(%[d_im], %[off])")
            __ASM_EMIT  ("4:")
            : [off] "+r" (off), [count] "+r" (count),
              [k] "+Yz" (k)
            : [d_re] "r" (re), [d_im] "r" (im)
            : "cc", "memory",
              "%xmm1",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }
}


#endif /* DSP_ARCH_X86_AVX2_FFT_NORMALIZE_H_ */
