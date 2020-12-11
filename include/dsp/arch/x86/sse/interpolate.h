/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 11 дек. 2020 г.
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

#ifndef DSP_ARCH_X86_SSE_INTERPOLATE_H_
#define DSP_ARCH_X86_SSE_INTERPOLATE_H_

namespace sse
{
    static const float lin_inter_const[] __lsp_aligned16 =
    {
        0.0f, 1.0f, 2.0f, 3.0f, // Initial values 0..3
        4.0f, 4.0f, 4.0f, 4.0f  // Step
    };

    void lin_inter_set(float *dst, int32_t x0, float y0, int32_t x1, float y1, int32_t x, uint32_t n)
    {
        float dy    = (y1 - y0) / float(x1 - x0);
        x          -= x0;
        for (size_t i=0; i<n; ++i, ++x)
            dst[i]      = (x * dy + y0);
    }

    void lin_inter_mul2(float *dst, int32_t x0, float y0, int32_t x1, float y1, int32_t x, uint32_t n)
    {
        float dy    = (y1 - y0) / float(x1 - x0);
        x          -= x0;
        for (size_t i=0; i<n; ++i, ++x)
            dst[i]     *= (x * dy + y0);
    }

    void lin_inter_mul3(float *dst, const float *src, int32_t x0, float y0, int32_t x1, float y1, int32_t x, uint32_t n)
    {
        float dy    = (y1 - y0) / float(x1 - x0);
        x          -= x0;
        for (size_t i=0; i<n; ++i, ++x)
            dst[i]      = src[i] * (x * dy + y0);
    }

    void lin_inter_fmadd2(float *dst, const float *src, int32_t x0, float y0, int32_t x1, float y1, int32_t x, uint32_t n)
    {
        float dy    = (y1 - y0) / float(x1 - x0);
        x          -= x0;
        for (size_t i=0; i<n; ++i, ++x)
            dst[i]     += src[i]*(x * dy + y0);
    }

    void lin_inter_frmadd2(float *dst, const float *src, int32_t x0, float y0, int32_t x1, float y1, int32_t x, uint32_t n)
    {
        float dy    = (y1 - y0) / float(x1 - x0);
        x          -= x0;
        for (size_t i=0; i<n; ++i, ++x)
            dst[i]     = dst[i]*(x * dy + y0) + src[i];
    }

    void lin_inter_fmadd3(float *dst, const float *src1, const float *src2, int32_t x0, float y0, int32_t x1, float y1, int32_t x, uint32_t n)
    {
        IF_ARCH_X86(
            float dx    = x1 - x0;
            float dy    = y1 - y0;
            float xx    = x - x0;
        )

        ARCH_X86_ASM(
            __ASM_EMIT("movss       %[dx], %%xmm7")                     // xmm7     = dx
            __ASM_EMIT("movss       %[dy], %%xmm6")                     // xmm6     = dy
            __ASM_EMIT("movss       %[xx], %%xmm5")                     // xmm5     = x
            __ASM_EMIT("divss       %%xmm7, %%xmm6")                    // xmm6     = dy / dx
            __ASM_EMIT("movss       %[y0], %%xmm4")                     // xmm4     = y0
            __ASM_EMIT("shufps      $0x00, %%xmm6, %%xmm6")             // xmm6     = dy / dx
            __ASM_EMIT("shufps      $0x00, %%xmm5, %%xmm5")             // xmm5     = x
            __ASM_EMIT("mulps       0x00(%[CC]), %%xmm6")               // xmm6     = k = xi * dy / dx
            __ASM_EMIT("movaps      0x10(%[CC]), %%xmm7")               // xmm7     = step

            __ASM_EMIT("sub         $4, %[n]")
            __ASM_EMIT("jl          2f")
            // 4x blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("movaps      %%xmm5, %%xmm2")                    // xmm2     = x
            __ASM_EMIT("movups      (%[src1], %[off]), %%xmm0")         // xmm0     = s1
            __ASM_EMIT("mulps       %%xmm6, %%xmm2")                    // xmm2     = k*x
            __ASM_EMIT("movups      (%[src2], %[off]), %%xmm1")         // xmm1     = s2
            __ASM_EMIT("addps       %%xmm4, %%xmm2")                    // xmm2     = k*x + y0
            __ASM_EMIT("addps       %%xmm7, %%xmm5")                    // xmm5     = x' = x + step
            __ASM_EMIT("mulps       %%xmm2, %%xmm0")                    // xmm0     = s1*(k*x + y0)
            __ASM_EMIT("addps       %%xmm1, %%xmm0")                    // xmm0     = s1*(k*x + y0) + s2
            __ASM_EMIT("sub         $4, %[n]")                          // n       -= 4
            __ASM_EMIT("movups      %%xmm0, %[dst]")
        );

        x          -= x0;
        for (size_t i=0; i<n; ++i, ++x)
            dst[i]  = src1[i]*(x * dy + y0) + src2[i];
    }
}



#endif /* DSP_ARCH_X86_SSE_INTERPOLATE_H_ */
