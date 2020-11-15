/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 18 дек. 2019 г.
 *
 * lsp-plugins is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-plugins is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-plugins. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef DSP_ARCH_X86_SSE_FFT_NORMALIZE_H_
#define DSP_ARCH_X86_SSE_FFT_NORMALIZE_H_

#ifndef DSP_ARCH_X86_SSE_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_SSE_IMPL */

namespace sse
{
    void normalize_fft3(float *dre, float *dim, const float *re, const float *im, size_t rank)
    {
        IF_ARCH_X86(
            float k = 1.0f/(1 << rank);
            size_t count = 1 << rank, off = 0;
        );
        ARCH_X86_ASM(
            // x8 blocks
            __ASM_EMIT  ("shufps        $0x00, %%xmm0, %%xmm0")                 // xmm0   = k
            __ASM_EMIT32("subl          $8, %[count]")
            __ASM_EMIT64("sub           $8, %[count]")
            __ASM_EMIT  ("movaps        %%xmm0, %%xmm1")
            __ASM_EMIT  ("jb            2f")
            __ASM_EMIT  ("1:")
            __ASM_EMIT  ("movups        0x00(%[s_re], %[off]), %%xmm4")
            __ASM_EMIT  ("movups        0x10(%[s_re], %[off]), %%xmm5")
            __ASM_EMIT  ("movups        0x00(%[s_im], %[off]), %%xmm6")
            __ASM_EMIT  ("movups        0x10(%[s_im], %[off]), %%xmm7")
            __ASM_EMIT  ("mulps         %%xmm0, %%xmm4")
            __ASM_EMIT  ("mulps         %%xmm1, %%xmm5")
            __ASM_EMIT  ("mulps         %%xmm0, %%xmm6")
            __ASM_EMIT  ("mulps         %%xmm1, %%xmm7")
            __ASM_EMIT  ("movups        %%xmm4, 0x00(%[d_re], %[off])")
            __ASM_EMIT  ("movups        %%xmm5, 0x10(%[d_re], %[off])")
            __ASM_EMIT  ("movups        %%xmm6, 0x00(%[d_im], %[off])")
            __ASM_EMIT  ("movups        %%xmm7, 0x10(%[d_im], %[off])")
            __ASM_EMIT  ("add           $0x20, %[off]")
            __ASM_EMIT32("subl          $8, %[count]")
            __ASM_EMIT64("sub           $8, %[count]")
            __ASM_EMIT  ("jae           1b")
            __ASM_EMIT  ("2:")
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
            // x8 blocks
            __ASM_EMIT  ("shufps        $0x00, %%xmm0, %%xmm0")                 // xmm0   = k
            __ASM_EMIT  ("sub           $8, %[count]")
            __ASM_EMIT  ("movaps        %%xmm0, %%xmm1")
            __ASM_EMIT  ("jb            2f")
            __ASM_EMIT  ("1:")
            __ASM_EMIT  ("movups        0x00(%[d_re], %[off]), %%xmm4")
            __ASM_EMIT  ("movups        0x10(%[d_re], %[off]), %%xmm5")
            __ASM_EMIT  ("movups        0x00(%[d_im], %[off]), %%xmm6")
            __ASM_EMIT  ("movups        0x10(%[d_im], %[off]), %%xmm7")
            __ASM_EMIT  ("mulps         %%xmm0, %%xmm4")
            __ASM_EMIT  ("mulps         %%xmm1, %%xmm5")
            __ASM_EMIT  ("mulps         %%xmm0, %%xmm6")
            __ASM_EMIT  ("mulps         %%xmm1, %%xmm7")
            __ASM_EMIT  ("movups        %%xmm4, 0x00(%[d_re], %[off])")
            __ASM_EMIT  ("movups        %%xmm5, 0x10(%[d_re], %[off])")
            __ASM_EMIT  ("movups        %%xmm6, 0x00(%[d_im], %[off])")
            __ASM_EMIT  ("movups        %%xmm7, 0x10(%[d_im], %[off])")
            __ASM_EMIT  ("add           $0x20, %[off]")
            __ASM_EMIT  ("sub           $8, %[count]")
            __ASM_EMIT  ("jae           1b")
            __ASM_EMIT  ("2:")
            : [off] "+r" (off), [count] "+r" (count),
              [k] "+Yz" (k)
            : [d_re] "r" (re), [d_im] "r" (im)
            : "cc", "memory",
              "%xmm1",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }
}

#endif /* DSP_ARCH_X86_SSE_FFT_NORMALIZE_H_ */
