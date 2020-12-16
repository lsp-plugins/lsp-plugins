/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 12 дек. 2020 г.
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

#ifndef DSP_ARCH_X86_AVX_INTERPOLATE_H_
#define DSP_ARCH_X86_AVX_INTERPOLATE_H_

namespace avx
{
    static const float lin_inter_const[] __lsp_aligned32 =
    {
        0.0f, 1.0f,  2.0f, 3.0f,  4.0f,  5.0f,  6.0f,  7.0f,    // Initial values 0..7
        8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f,   // Initial values 8..15
        16.0f, 16.0f, 16.0f, 16.0f, 16.0f, 16.0f, 16.0f, 16.0f  // Step
    };

    void lin_inter_set(float *dst, int32_t x0, float y0, int32_t x1, float y1, int32_t x, uint32_t n)
    {
        IF_ARCH_X86(
            float dx    = x1 - x0;
            float dy    = y1 - y0;
            float xx    = x - x0;
            ssize_t off;
        )

        ARCH_X86_ASM(
            __ASM_EMIT("xor                 %[off], %[off]")
            __ASM_EMIT("vbroadcastss        %[xx], %%ymm4")                             // ymm4     = xx
            __ASM_EMIT("vbroadcastss        %[dy], %%ymm7")                             // ymm7     = dy
            __ASM_EMIT("vbroadcastss        %[dx], %%ymm0")                             // ymm0     = dx
            __ASM_EMIT("vaddps              0x20(%[CC]), %%ymm4, %%ymm5")               // ymm5     = x[1]
            __ASM_EMIT("vbroadcastss        %[y0], %%ymm6")                             // ymm6     = y0
            __ASM_EMIT("vdivps              %%ymm0, %%ymm7, %%ymm7")                    // ymm7     = k = dy / dx
            __ASM_EMIT("vaddps              0x00(%[CC]), %%ymm4, %%ymm4")               // ymm4     = x[0]
            __ASM_EMIT("vmovaps             0x40(%[CC]), %%ymm3")                       // ymm3     = d
            __ASM_EMIT("sub                 $16, %[n]")
            __ASM_EMIT("jb                  2f")

            // 16x blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("vmulps              %%ymm7, %%ymm4, %%ymm0")                    // ymm0     = k*x[0]
            __ASM_EMIT("vmulps              %%ymm7, %%ymm5, %%ymm1")                    // ymm1     = k*x[1]
            __ASM_EMIT("vaddps              %%ymm3, %%ymm4, %%ymm4")                    // ymm4     = x[0]' = x[0] + d
            __ASM_EMIT("vaddps              %%ymm6, %%ymm0, %%ymm0")                    // ymm0     = k*x[0]+y0
            __ASM_EMIT("vaddps              %%ymm3, %%ymm5, %%ymm5")                    // ymm5     = x[1]' = x[1] + d
            __ASM_EMIT("vaddps              %%ymm6, %%ymm1, %%ymm1")                    // ymm1     = k*x[1]+y0
            __ASM_EMIT("vmovups             %%ymm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("vmovups             %%ymm1, 0x20(%[dst], %[off])")
            __ASM_EMIT("add                 $0x40, %[off]")
            __ASM_EMIT("sub                 $16, %[n]")
            __ASM_EMIT("jae                 1b")

            // 8x block
            __ASM_EMIT("2:")
            __ASM_EMIT("add                 $8, %[n]")
            __ASM_EMIT("jl                  4f")
            __ASM_EMIT("vextractf128        $1, %%ymm4, %%xmm3")                        // xmm3     = x[1]
            __ASM_EMIT("vmulps              %%xmm7, %%xmm4, %%xmm0")                    // xmm0     = k*x[0]
            __ASM_EMIT("vmulps              %%xmm7, %%xmm3, %%xmm1")                    // xmm1     = k*x[1]
            __ASM_EMIT("vmovaps             %%ymm5, %%ymm4")                            // ymm4     = x[0]
            __ASM_EMIT("vaddps              %%xmm6, %%xmm0, %%xmm0")                    // xmm0     = k*x[0]+y0
            __ASM_EMIT("vaddps              %%xmm6, %%xmm1, %%xmm1")                    // xmm1     = k*x[1]+y0
            __ASM_EMIT("vmovups             %%xmm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("vmovups             %%xmm1, 0x10(%[dst], %[off])")
            __ASM_EMIT("add                 $0x20, %[off]")
            __ASM_EMIT("sub                 $8, %[n]")

            // 4x block
            __ASM_EMIT("4:")
            __ASM_EMIT("add                 $4, %[n]")
            __ASM_EMIT("jl                  6f")
            __ASM_EMIT("vmulps              %%xmm7, %%xmm4, %%xmm0")                    // xmm0     = k*x[0]
            __ASM_EMIT("vextractf128        $1, %%ymm4, %%xmm4")                        // xmm4     = x[0]
            __ASM_EMIT("vaddps              %%xmm6, %%xmm0, %%xmm0")                    // xmm0     = k*x[0]+y0
            __ASM_EMIT("vmovups             %%xmm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("add                 $0x10, %[off]")
            __ASM_EMIT("sub                 $4, %[n]")

            // 1x blocks
            __ASM_EMIT("6:")
            __ASM_EMIT("add                 $3, %[n]")
            __ASM_EMIT("jl                  8f")
            __ASM_EMIT("7:")
            __ASM_EMIT("vmulss              %%xmm7, %%xmm4, %%xmm0")                    // xmm0     = k*x[0]
            __ASM_EMIT("vshufps             $0x39, %%xmm4, %%xmm4, %%xmm4")             // xmm4     = x[0]'
            __ASM_EMIT("vaddss              %%xmm6, %%xmm0, %%xmm0")                    // xmm0     = k*x[0]+y0
            __ASM_EMIT("vmovss              %%xmm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("add                 $0x04, %[off]")
            __ASM_EMIT("dec                 %[n]")
            __ASM_EMIT("jge                 7b")

            __ASM_EMIT("8:")
            : [n] "+r" (n), [off] "=&r" (off)
            : [dst] "r" (dst),
              [CC] "r" (lin_inter_const),
              [dx] "m" (dx),
              [dy] "m" (dy),
              [xx] "m" (xx),
              [y0] "m" (y0)
            : "cc", "memory",
              "xmm0", "xmm1", "xmm2", "xmm3",
              "xmm4", "xmm5", "xmm6", "xmm7"
        );
    }

    void lin_inter_mul2(float *dst, int32_t x0, float y0, int32_t x1, float y1, int32_t x, uint32_t n)
    {
        IF_ARCH_X86(
            float dx    = x1 - x0;
            float dy    = y1 - y0;
            float xx    = x - x0;
            ssize_t off;
        )

        ARCH_X86_ASM(
            __ASM_EMIT("xor                 %[off], %[off]")
            __ASM_EMIT("vbroadcastss        %[xx], %%ymm4")                             // ymm4     = xx
            __ASM_EMIT("vbroadcastss        %[dy], %%ymm7")                             // ymm7     = dy
            __ASM_EMIT("vbroadcastss        %[dx], %%ymm0")                             // ymm0     = dx
            __ASM_EMIT("vaddps              0x20(%[CC]), %%ymm4, %%ymm5")               // ymm5     = x[1]
            __ASM_EMIT("vbroadcastss        %[y0], %%ymm6")                             // ymm6     = y0
            __ASM_EMIT("vdivps              %%ymm0, %%ymm7, %%ymm7")                    // ymm7     = k = dy / dx
            __ASM_EMIT("vaddps              0x00(%[CC]), %%ymm4, %%ymm4")               // ymm4     = x[0]
            __ASM_EMIT("vmovaps             0x40(%[CC]), %%ymm3")                       // ymm3     = d
            __ASM_EMIT("sub                 $16, %[n]")
            __ASM_EMIT("jb                  2f")

            // 16x blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("vmulps              %%ymm7, %%ymm4, %%ymm0")                    // ymm0     = k*x[0]
            __ASM_EMIT("vmulps              %%ymm7, %%ymm5, %%ymm1")                    // ymm1     = k*x[1]
            __ASM_EMIT("vaddps              %%ymm3, %%ymm4, %%ymm4")                    // ymm4     = x[0]' = x[0] + d
            __ASM_EMIT("vaddps              %%ymm6, %%ymm0, %%ymm0")                    // ymm0     = k*x[0]+y0
            __ASM_EMIT("vaddps              %%ymm3, %%ymm5, %%ymm5")                    // ymm5     = x[1]' = x[1] + d
            __ASM_EMIT("vaddps              %%ymm6, %%ymm1, %%ymm1")                    // ymm1     = k*x[1]+y0
            __ASM_EMIT("vmulps              0x00(%[dst], %[off]), %%ymm0, %%ymm0")      // ymm0     = s0[0]*(k*x[0]+y0)
            __ASM_EMIT("vmulps              0x20(%[dst], %[off]), %%ymm1, %%ymm1")      // ymm1     = s0[1]*(k*x[1]+y0)
            __ASM_EMIT("vmovups             %%ymm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("vmovups             %%ymm1, 0x20(%[dst], %[off])")
            __ASM_EMIT("add                 $0x40, %[off]")
            __ASM_EMIT("sub                 $16, %[n]")
            __ASM_EMIT("jae                 1b")

            // 8x block
            __ASM_EMIT("2:")
            __ASM_EMIT("add                 $8, %[n]")
            __ASM_EMIT("jl                  4f")
            __ASM_EMIT("vextractf128        $1, %%ymm4, %%xmm3")                        // xmm3     = x[1]
            __ASM_EMIT("vmulps              %%xmm7, %%xmm4, %%xmm0")                    // xmm0     = k*x[0]
            __ASM_EMIT("vmulps              %%xmm7, %%xmm3, %%xmm1")                    // xmm1     = k*x[1]
            __ASM_EMIT("vmovaps             %%ymm5, %%ymm4")                            // ymm4     = x[0]
            __ASM_EMIT("vaddps              %%xmm6, %%xmm0, %%xmm0")                    // xmm0     = k*x[0]+y0
            __ASM_EMIT("vaddps              %%xmm6, %%xmm1, %%xmm1")                    // xmm1     = k*x[1]+y0
            __ASM_EMIT("vmulps              0x00(%[dst], %[off]), %%xmm0, %%xmm0")      // xmm0     = s0[0]*(k*x[0]+y0)
            __ASM_EMIT("vmulps              0x10(%[dst], %[off]), %%xmm1, %%xmm1")      // xmm1     = s0[1]*(k*x[1]+y0)
            __ASM_EMIT("vmovups             %%xmm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("vmovups             %%xmm1, 0x10(%[dst], %[off])")
            __ASM_EMIT("add                 $0x20, %[off]")
            __ASM_EMIT("sub                 $8, %[n]")

            // 4x block
            __ASM_EMIT("4:")
            __ASM_EMIT("add                 $4, %[n]")
            __ASM_EMIT("jl                  6f")
            __ASM_EMIT("vmulps              %%xmm7, %%xmm4, %%xmm0")                    // xmm0     = k*x[0]
            __ASM_EMIT("vextractf128        $1, %%ymm4, %%xmm4")                        // xmm4     = x[0]
            __ASM_EMIT("vaddps              %%xmm6, %%xmm0, %%xmm0")                    // xmm0     = k*x[0]+y0
            __ASM_EMIT("vmulps              0x00(%[dst], %[off]), %%xmm0, %%xmm0")      // xmm0     = s0[0]*(k*x[0]+y0)
            __ASM_EMIT("vmovups             %%xmm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("add                 $0x10, %[off]")
            __ASM_EMIT("sub                 $4, %[n]")

            // 1x blocks
            __ASM_EMIT("6:")
            __ASM_EMIT("add                 $3, %[n]")
            __ASM_EMIT("jl                  8f")
            __ASM_EMIT("7:")
            __ASM_EMIT("vmulss              %%xmm7, %%xmm4, %%xmm0")                    // xmm0     = k*x[0]
            __ASM_EMIT("vshufps             $0x39, %%xmm4, %%xmm4, %%xmm4")             // xmm4     = x[0]'
            __ASM_EMIT("vaddss              %%xmm6, %%xmm0, %%xmm0")                    // xmm0     = k*x[0]+y0
            __ASM_EMIT("vmulss              0x00(%[dst], %[off]), %%xmm0, %%xmm0")      // xmm0     = s0[0]*(k*x[0]+y0)
            __ASM_EMIT("vmovss              %%xmm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("add                 $0x04, %[off]")
            __ASM_EMIT("dec                 %[n]")
            __ASM_EMIT("jge                 7b")

            __ASM_EMIT("8:")
            : [n] "+r" (n), [off] "=&r" (off)
            : [dst] "r" (dst),
              [CC] "r" (lin_inter_const),
              [dx] "m" (dx),
              [dy] "m" (dy),
              [xx] "m" (xx),
              [y0] "m" (y0)
            : "cc", "memory",
              "xmm0", "xmm1", "xmm2", "xmm3",
              "xmm4", "xmm5", "xmm6", "xmm7"
        );
    }

    void lin_inter_mul3(float *dst, const float *src, int32_t x0, float y0, int32_t x1, float y1, int32_t x, uint32_t n)
    {
        IF_ARCH_X86(
            float dx    = x1 - x0;
            float dy    = y1 - y0;
            float xx    = x - x0;
            ssize_t off;
        )

        ARCH_X86_ASM(
            __ASM_EMIT("xor                 %[off], %[off]")
            __ASM_EMIT("vbroadcastss        %[xx], %%ymm4")                             // ymm4     = xx
            __ASM_EMIT("vbroadcastss        %[dy], %%ymm7")                             // ymm7     = dy
            __ASM_EMIT("vbroadcastss        %[dx], %%ymm0")                             // ymm0     = dx
            __ASM_EMIT("vaddps              0x20(%[CC]), %%ymm4, %%ymm5")               // ymm5     = x[1]
            __ASM_EMIT("vbroadcastss        %[y0], %%ymm6")                             // ymm6     = y0
            __ASM_EMIT("vdivps              %%ymm0, %%ymm7, %%ymm7")                    // ymm7     = k = dy / dx
            __ASM_EMIT("vaddps              0x00(%[CC]), %%ymm4, %%ymm4")               // ymm4     = x[0]
            __ASM_EMIT("vmovaps             0x40(%[CC]), %%ymm3")                       // ymm3     = d
            __ASM_EMIT("sub                 $16, %[n]")
            __ASM_EMIT("jb                  2f")

            // 16x blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("vmulps              %%ymm7, %%ymm4, %%ymm0")                    // ymm0     = k*x[0]
            __ASM_EMIT("vmulps              %%ymm7, %%ymm5, %%ymm1")                    // ymm1     = k*x[1]
            __ASM_EMIT("vaddps              %%ymm3, %%ymm4, %%ymm4")                    // ymm4     = x[0]' = x[0] + d
            __ASM_EMIT("vaddps              %%ymm6, %%ymm0, %%ymm0")                    // ymm0     = k*x[0]+y0
            __ASM_EMIT("vaddps              %%ymm3, %%ymm5, %%ymm5")                    // ymm5     = x[1]' = x[1] + d
            __ASM_EMIT("vaddps              %%ymm6, %%ymm1, %%ymm1")                    // ymm1     = k*x[1]+y0
            __ASM_EMIT("vmulps              0x00(%[src], %[off]), %%ymm0, %%ymm0")      // ymm0     = s0[0]*(k*x[0]+y0)
            __ASM_EMIT("vmulps              0x20(%[src], %[off]), %%ymm1, %%ymm1")      // ymm1     = s0[1]*(k*x[1]+y0)
            __ASM_EMIT("vmovups             %%ymm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("vmovups             %%ymm1, 0x20(%[dst], %[off])")
            __ASM_EMIT("add                 $0x40, %[off]")
            __ASM_EMIT("sub                 $16, %[n]")
            __ASM_EMIT("jae                 1b")

            // 8x block
            __ASM_EMIT("2:")
            __ASM_EMIT("add                 $8, %[n]")
            __ASM_EMIT("jl                  4f")
            __ASM_EMIT("vextractf128        $1, %%ymm4, %%xmm3")                        // xmm3     = x[1]
            __ASM_EMIT("vmulps              %%xmm7, %%xmm4, %%xmm0")                    // xmm0     = k*x[0]
            __ASM_EMIT("vmulps              %%xmm7, %%xmm3, %%xmm1")                    // xmm1     = k*x[1]
            __ASM_EMIT("vmovaps             %%ymm5, %%ymm4")                            // ymm4     = x[0]
            __ASM_EMIT("vaddps              %%xmm6, %%xmm0, %%xmm0")                    // xmm0     = k*x[0]+y0
            __ASM_EMIT("vaddps              %%xmm6, %%xmm1, %%xmm1")                    // xmm1     = k*x[1]+y0
            __ASM_EMIT("vmulps              0x00(%[src], %[off]), %%xmm0, %%xmm0")      // xmm0     = s0[0]*(k*x[0]+y0)
            __ASM_EMIT("vmulps              0x10(%[src], %[off]), %%xmm1, %%xmm1")      // xmm1     = s0[1]*(k*x[1]+y0)
            __ASM_EMIT("vmovups             %%xmm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("vmovups             %%xmm1, 0x10(%[dst], %[off])")
            __ASM_EMIT("add                 $0x20, %[off]")
            __ASM_EMIT("sub                 $8, %[n]")

            // 4x block
            __ASM_EMIT("4:")
            __ASM_EMIT("add                 $4, %[n]")
            __ASM_EMIT("jl                  6f")
            __ASM_EMIT("vmulps              %%xmm7, %%xmm4, %%xmm0")                    // xmm0     = k*x[0]
            __ASM_EMIT("vextractf128        $1, %%ymm4, %%xmm4")                        // xmm4     = x[0]
            __ASM_EMIT("vaddps              %%xmm6, %%xmm0, %%xmm0")                    // xmm0     = k*x[0]+y0
            __ASM_EMIT("vmulps              0x00(%[src], %[off]), %%xmm0, %%xmm0")      // xmm0     = s0[0]*(k*x[0]+y0)
            __ASM_EMIT("vmovups             %%xmm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("add                 $0x10, %[off]")
            __ASM_EMIT("sub                 $4, %[n]")

            // 1x blocks
            __ASM_EMIT("6:")
            __ASM_EMIT("add                 $3, %[n]")
            __ASM_EMIT("jl                  8f")
            __ASM_EMIT("7:")
            __ASM_EMIT("vmulss              %%xmm7, %%xmm4, %%xmm0")                    // xmm0     = k*x[0]
            __ASM_EMIT("vshufps             $0x39, %%xmm4, %%xmm4, %%xmm4")             // xmm4     = x[0]'
            __ASM_EMIT("vaddss              %%xmm6, %%xmm0, %%xmm0")                    // xmm0     = k*x[0]+y0
            __ASM_EMIT("vmulss              0x00(%[src], %[off]), %%xmm0, %%xmm0")      // xmm0     = s0[0]*(k*x[0]+y0)
            __ASM_EMIT("vmovss              %%xmm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("add                 $0x04, %[off]")
            __ASM_EMIT("dec                 %[n]")
            __ASM_EMIT("jge                 7b")

            __ASM_EMIT("8:")
            : [n] "+r" (n), [off] "=&r" (off)
            : [dst] "r" (dst), [src] "r" (src),
              [CC] "r" (lin_inter_const),
              [dx] "m" (dx),
              [dy] "m" (dy),
              [xx] "m" (xx),
              [y0] "m" (y0)
            : "cc", "memory",
              "xmm0", "xmm1", "xmm2", "xmm3",
              "xmm4", "xmm5", "xmm6", "xmm7"
        );
    }

    void lin_inter_fmadd2(float *dst, const float *src, int32_t x0, float y0, int32_t x1, float y1, int32_t x, uint32_t n)
    {
        IF_ARCH_X86(
            float dx    = x1 - x0;
            float dy    = y1 - y0;
            float xx    = x - x0;
            ssize_t off;
        )

        ARCH_X86_ASM(
            __ASM_EMIT("xor                 %[off], %[off]")
            __ASM_EMIT("vbroadcastss        %[xx], %%ymm4")                             // ymm4     = xx
            __ASM_EMIT("vbroadcastss        %[dy], %%ymm7")                             // ymm7     = dy
            __ASM_EMIT("vbroadcastss        %[dx], %%ymm0")                             // ymm0     = dx
            __ASM_EMIT("vaddps              0x20(%[CC]), %%ymm4, %%ymm5")               // ymm5     = x[1]
            __ASM_EMIT("vbroadcastss        %[y0], %%ymm6")                             // ymm6     = y0
            __ASM_EMIT("vdivps              %%ymm0, %%ymm7, %%ymm7")                    // ymm7     = k = dy / dx
            __ASM_EMIT("vaddps              0x00(%[CC]), %%ymm4, %%ymm4")               // ymm4     = x[0]
            __ASM_EMIT("vmovaps             0x40(%[CC]), %%ymm3")                       // ymm3     = d
            __ASM_EMIT("sub                 $16, %[n]")
            __ASM_EMIT("jb                  2f")

            // 16x blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("vmulps              %%ymm7, %%ymm4, %%ymm0")                    // ymm0     = k*x[0]
            __ASM_EMIT("vmulps              %%ymm7, %%ymm5, %%ymm1")                    // ymm1     = k*x[1]
            __ASM_EMIT("vaddps              %%ymm3, %%ymm4, %%ymm4")                    // ymm4     = x[0]' = x[0] + d
            __ASM_EMIT("vaddps              %%ymm6, %%ymm0, %%ymm0")                    // ymm0     = k*x[0]+y0
            __ASM_EMIT("vaddps              %%ymm3, %%ymm5, %%ymm5")                    // ymm5     = x[1]' = x[1] + d
            __ASM_EMIT("vaddps              %%ymm6, %%ymm1, %%ymm1")                    // ymm1     = k*x[1]+y0
            __ASM_EMIT("vmulps              0x00(%[src], %[off]), %%ymm0, %%ymm0")      // ymm0     = s0[0]*(k*x[0]+y0)
            __ASM_EMIT("vmulps              0x20(%[src], %[off]), %%ymm1, %%ymm1")      // ymm1     = s0[1]*(k*x[1]+y0)
            __ASM_EMIT("vaddps              0x00(%[dst], %[off]), %%ymm0, %%ymm0")      // ymm0     = s0[0]*(k*x[0]+y0) + d0[0]
            __ASM_EMIT("vaddps              0x20(%[dst], %[off]), %%ymm1, %%ymm1")      // ymm1     = s0[1]*(k*x[1]+y0) + d0[1]
            __ASM_EMIT("vmovups             %%ymm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("vmovups             %%ymm1, 0x20(%[dst], %[off])")
            __ASM_EMIT("add                 $0x40, %[off]")
            __ASM_EMIT("sub                 $16, %[n]")
            __ASM_EMIT("jae                 1b")

            // 8x block
            __ASM_EMIT("2:")
            __ASM_EMIT("add                 $8, %[n]")
            __ASM_EMIT("jl                  4f")
            __ASM_EMIT("vextractf128        $1, %%ymm4, %%xmm3")                        // xmm3     = x[1]
            __ASM_EMIT("vmulps              %%xmm7, %%xmm4, %%xmm0")                    // xmm0     = k*x[0]
            __ASM_EMIT("vmulps              %%xmm7, %%xmm3, %%xmm1")                    // xmm1     = k*x[1]
            __ASM_EMIT("vmovaps             %%ymm5, %%ymm4")                            // ymm4     = x[0]
            __ASM_EMIT("vaddps              %%xmm6, %%xmm0, %%xmm0")                    // xmm0     = k*x[0]+y0
            __ASM_EMIT("vaddps              %%xmm6, %%xmm1, %%xmm1")                    // xmm1     = k*x[1]+y0
            __ASM_EMIT("vmulps              0x00(%[src], %[off]), %%xmm0, %%xmm0")      // xmm0     = s0[0]*(k*x[0]+y0)
            __ASM_EMIT("vmulps              0x10(%[src], %[off]), %%xmm1, %%xmm1")      // xmm1     = s0[1]*(k*x[1]+y0)
            __ASM_EMIT("vaddps              0x00(%[dst], %[off]), %%xmm0, %%xmm0")      // xmm0     = s0[0]*(k*x[0]+y0) + d0[0]
            __ASM_EMIT("vaddps              0x10(%[dst], %[off]), %%xmm1, %%xmm1")      // xmm1     = s0[1]*(k*x[1]+y0) + d0[1]
            __ASM_EMIT("vmovups             %%xmm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("vmovups             %%xmm1, 0x10(%[dst], %[off])")
            __ASM_EMIT("add                 $0x20, %[off]")
            __ASM_EMIT("sub                 $8, %[n]")

            // 4x block
            __ASM_EMIT("4:")
            __ASM_EMIT("add                 $4, %[n]")
            __ASM_EMIT("jl                  6f")
            __ASM_EMIT("vmulps              %%xmm7, %%xmm4, %%xmm0")                    // xmm0     = k*x[0]
            __ASM_EMIT("vextractf128        $1, %%ymm4, %%xmm4")                        // xmm4     = x[0]
            __ASM_EMIT("vaddps              %%xmm6, %%xmm0, %%xmm0")                    // xmm0     = k*x[0]+y0
            __ASM_EMIT("vmulps              0x00(%[src], %[off]), %%xmm0, %%xmm0")      // xmm0     = s0[0]*(k*x[0]+y0)
            __ASM_EMIT("vaddps              0x00(%[dst], %[off]), %%xmm0, %%xmm0")      // xmm0     = s0[0]*(k*x[0]+y0) + d0[0]
            __ASM_EMIT("vmovups             %%xmm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("add                 $0x10, %[off]")
            __ASM_EMIT("sub                 $4, %[n]")

            // 1x blocks
            __ASM_EMIT("6:")
            __ASM_EMIT("add                 $3, %[n]")
            __ASM_EMIT("jl                  8f")
            __ASM_EMIT("7:")
            __ASM_EMIT("vmulss              %%xmm7, %%xmm4, %%xmm0")                    // xmm0     = k*x[0]
            __ASM_EMIT("vshufps             $0x39, %%xmm4, %%xmm4, %%xmm4")             // xmm4     = x[0]'
            __ASM_EMIT("vaddss              %%xmm6, %%xmm0, %%xmm0")                    // xmm0     = k*x[0]+y0
            __ASM_EMIT("vmulss              0x00(%[src], %[off]), %%xmm0, %%xmm0")      // ymm0     = s0[0]*(k*x[0]+y0)
            __ASM_EMIT("vaddss              0x00(%[dst], %[off]), %%xmm0, %%xmm0")      // xmm0     = s0[0]*(k*x[0]+y0) + d0[0]
            __ASM_EMIT("vmovss              %%xmm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("add                 $0x04, %[off]")
            __ASM_EMIT("dec                 %[n]")
            __ASM_EMIT("jge                 7b")

            __ASM_EMIT("8:")
            : [n] "+r" (n), [off] "=&r" (off)
            : [dst] "r" (dst), [src] "r" (src),
              [CC] "r" (lin_inter_const),
              [dx] "m" (dx),
              [dy] "m" (dy),
              [xx] "m" (xx),
              [y0] "m" (y0)
            : "cc", "memory",
              "xmm0", "xmm1", "xmm2", "xmm3",
              "xmm4", "xmm5", "xmm6", "xmm7"
        );
    }

    void lin_inter_frmadd2(float *dst, const float *src, int32_t x0, float y0, int32_t x1, float y1, int32_t x, uint32_t n)
    {
        IF_ARCH_X86(
            float dx    = x1 - x0;
            float dy    = y1 - y0;
            float xx    = x - x0;
            ssize_t off;
        )

        ARCH_X86_ASM(
            __ASM_EMIT("xor                 %[off], %[off]")
            __ASM_EMIT("vbroadcastss        %[xx], %%ymm4")                             // ymm4     = xx
            __ASM_EMIT("vbroadcastss        %[dy], %%ymm7")                             // ymm7     = dy
            __ASM_EMIT("vbroadcastss        %[dx], %%ymm0")                             // ymm0     = dx
            __ASM_EMIT("vaddps              0x20(%[CC]), %%ymm4, %%ymm5")               // ymm5     = x[1]
            __ASM_EMIT("vbroadcastss        %[y0], %%ymm6")                             // ymm6     = y0
            __ASM_EMIT("vdivps              %%ymm0, %%ymm7, %%ymm7")                    // ymm7     = k = dy / dx
            __ASM_EMIT("vaddps              0x00(%[CC]), %%ymm4, %%ymm4")               // ymm4     = x[0]
            __ASM_EMIT("vmovaps             0x40(%[CC]), %%ymm3")                       // ymm3     = d
            __ASM_EMIT("sub                 $16, %[n]")
            __ASM_EMIT("jb                  2f")

            // 16x blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("vmulps              %%ymm7, %%ymm4, %%ymm0")                    // ymm0     = k*x[0]
            __ASM_EMIT("vmulps              %%ymm7, %%ymm5, %%ymm1")                    // ymm1     = k*x[1]
            __ASM_EMIT("vaddps              %%ymm3, %%ymm4, %%ymm4")                    // ymm4     = x[0]' = x[0] + d
            __ASM_EMIT("vaddps              %%ymm6, %%ymm0, %%ymm0")                    // ymm0     = k*x[0]+y0
            __ASM_EMIT("vaddps              %%ymm3, %%ymm5, %%ymm5")                    // ymm5     = x[1]' = x[1] + d
            __ASM_EMIT("vaddps              %%ymm6, %%ymm1, %%ymm1")                    // ymm1     = k*x[1]+y0
            __ASM_EMIT("vmulps              0x00(%[dst], %[off]), %%ymm0, %%ymm0")      // ymm0     = s0[0]*(k*x[0]+y0)
            __ASM_EMIT("vmulps              0x20(%[dst], %[off]), %%ymm1, %%ymm1")      // ymm1     = s0[1]*(k*x[1]+y0)
            __ASM_EMIT("vaddps              0x00(%[src], %[off]), %%ymm0, %%ymm0")      // ymm0     = s0[0]*(k*x[0]+y0) + d0[0]
            __ASM_EMIT("vaddps              0x20(%[src], %[off]), %%ymm1, %%ymm1")      // ymm1     = s0[1]*(k*x[1]+y0) + d0[1]
            __ASM_EMIT("vmovups             %%ymm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("vmovups             %%ymm1, 0x20(%[dst], %[off])")
            __ASM_EMIT("add                 $0x40, %[off]")
            __ASM_EMIT("sub                 $16, %[n]")
            __ASM_EMIT("jae                 1b")

            // 8x block
            __ASM_EMIT("2:")
            __ASM_EMIT("add                 $8, %[n]")
            __ASM_EMIT("jl                  4f")
            __ASM_EMIT("vextractf128        $1, %%ymm4, %%xmm3")                        // xmm3     = x[1]
            __ASM_EMIT("vmulps              %%xmm7, %%xmm4, %%xmm0")                    // xmm0     = k*x[0]
            __ASM_EMIT("vmulps              %%xmm7, %%xmm3, %%xmm1")                    // xmm1     = k*x[1]
            __ASM_EMIT("vmovaps             %%ymm5, %%ymm4")                            // ymm4     = x[0]
            __ASM_EMIT("vaddps              %%xmm6, %%xmm0, %%xmm0")                    // xmm0     = k*x[0]+y0
            __ASM_EMIT("vaddps              %%xmm6, %%xmm1, %%xmm1")                    // xmm1     = k*x[1]+y0
            __ASM_EMIT("vmulps              0x00(%[dst], %[off]), %%xmm0, %%xmm0")      // xmm0     = s0[0]*(k*x[0]+y0)
            __ASM_EMIT("vmulps              0x10(%[dst], %[off]), %%xmm1, %%xmm1")      // xmm1     = s0[1]*(k*x[1]+y0)
            __ASM_EMIT("vaddps              0x00(%[src], %[off]), %%xmm0, %%xmm0")      // xmm0     = s0[0]*(k*x[0]+y0) + d0[0]
            __ASM_EMIT("vaddps              0x10(%[src], %[off]), %%xmm1, %%xmm1")      // xmm1     = s0[1]*(k*x[1]+y0) + d0[1]
            __ASM_EMIT("vmovups             %%xmm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("vmovups             %%xmm1, 0x10(%[dst], %[off])")
            __ASM_EMIT("add                 $0x20, %[off]")
            __ASM_EMIT("sub                 $8, %[n]")

            // 4x block
            __ASM_EMIT("4:")
            __ASM_EMIT("add                 $4, %[n]")
            __ASM_EMIT("jl                  6f")
            __ASM_EMIT("vmulps              %%xmm7, %%xmm4, %%xmm0")                    // xmm0     = k*x[0]
            __ASM_EMIT("vextractf128        $1, %%ymm4, %%xmm4")                        // xmm4     = x[0]
            __ASM_EMIT("vaddps              %%xmm6, %%xmm0, %%xmm0")                    // xmm0     = k*x[0]+y0
            __ASM_EMIT("vmulps              0x00(%[dst], %[off]), %%xmm0, %%xmm0")      // xmm0     = s0[0]*(k*x[0]+y0)
            __ASM_EMIT("vaddps              0x00(%[src], %[off]), %%xmm0, %%xmm0")      // xmm0     = s0[0]*(k*x[0]+y0) + d0[0]
            __ASM_EMIT("vmovups             %%xmm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("add                 $0x10, %[off]")
            __ASM_EMIT("sub                 $4, %[n]")

            // 1x blocks
            __ASM_EMIT("6:")
            __ASM_EMIT("add                 $3, %[n]")
            __ASM_EMIT("jl                  8f")
            __ASM_EMIT("7:")
            __ASM_EMIT("vmulss              %%xmm7, %%xmm4, %%xmm0")                    // xmm0     = k*x[0]
            __ASM_EMIT("vshufps             $0x39, %%xmm4, %%xmm4, %%xmm4")             // xmm4     = x[0]'
            __ASM_EMIT("vaddss              %%xmm6, %%xmm0, %%xmm0")                    // xmm0     = k*x[0]+y0
            __ASM_EMIT("vmulss              0x00(%[dst], %[off]), %%xmm0, %%xmm0")      // ymm0     = s0[0]*(k*x[0]+y0)
            __ASM_EMIT("vaddss              0x00(%[src], %[off]), %%xmm0, %%xmm0")      // xmm0     = s0[0]*(k*x[0]+y0) + d0[0]
            __ASM_EMIT("vmovss              %%xmm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("add                 $0x04, %[off]")
            __ASM_EMIT("dec                 %[n]")
            __ASM_EMIT("jge                 7b")

            __ASM_EMIT("8:")
            : [n] "+r" (n), [off] "=&r" (off)
            : [dst] "r" (dst), [src] "r" (src),
              [CC] "r" (lin_inter_const),
              [dx] "m" (dx),
              [dy] "m" (dy),
              [xx] "m" (xx),
              [y0] "m" (y0)
            : "cc", "memory",
              "xmm0", "xmm1", "xmm2", "xmm3",
              "xmm4", "xmm5", "xmm6", "xmm7"
        );
    }

    void lin_inter_fmadd3(float *dst, const float *src1, const float *src2, int32_t x0, float y0, int32_t x1, float y1, int32_t x, uint32_t n)
    {
        IF_ARCH_X86(
            float dx    = x1 - x0;
            float dy    = y1 - y0;
            float xx    = x - x0;
        )

        ARCH_X86_ASM(
            __ASM_EMIT("vbroadcastss        %[xx], %%ymm4")                             // ymm4     = xx
            __ASM_EMIT("vbroadcastss        %[dy], %%ymm7")                             // ymm7     = dy
            __ASM_EMIT("vbroadcastss        %[dx], %%ymm0")                             // ymm0     = dx
            __ASM_EMIT("vaddps              0x20(%[CC]), %%ymm4, %%ymm5")               // ymm5     = x[1]
            __ASM_EMIT("vbroadcastss        %[y0], %%ymm6")                             // ymm6     = y0
            __ASM_EMIT("vdivps              %%ymm0, %%ymm7, %%ymm7")                    // ymm7     = k = dy / dx
            __ASM_EMIT("vaddps              0x00(%[CC]), %%ymm4, %%ymm4")               // ymm4     = x[0]
            __ASM_EMIT("vmovaps             0x40(%[CC]), %%ymm3")                       // ymm3     = d
            __ASM_EMIT("sub                 $16, %[n]")
            __ASM_EMIT("jb                  2f")

            // 16x blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("vmulps              %%ymm7, %%ymm4, %%ymm0")                    // ymm0     = k*x[0]
            __ASM_EMIT("vmulps              %%ymm7, %%ymm5, %%ymm1")                    // ymm1     = k*x[1]
            __ASM_EMIT("vaddps              %%ymm3, %%ymm4, %%ymm4")                    // ymm4     = x[0]' = x[0] + d
            __ASM_EMIT("vaddps              %%ymm6, %%ymm0, %%ymm0")                    // ymm0     = k*x[0]+y0
            __ASM_EMIT("vaddps              %%ymm3, %%ymm5, %%ymm5")                    // ymm5     = x[1]' = x[1] + d
            __ASM_EMIT("vaddps              %%ymm6, %%ymm1, %%ymm1")                    // ymm1     = k*x[1]+y0
            __ASM_EMIT("vmulps              0x00(%[src1]), %%ymm0, %%ymm0")             // ymm0     = s0[0]*(k*x[0]+y0)
            __ASM_EMIT("vmulps              0x20(%[src1]), %%ymm1, %%ymm1")             // ymm1     = s0[1]*(k*x[1]+y0)
            __ASM_EMIT("vaddps              0x00(%[src2]), %%ymm0, %%ymm0")             // ymm0     = s0[0]*(k*x[0]+y0) + d0[0]
            __ASM_EMIT("vaddps              0x20(%[src2]), %%ymm1, %%ymm1")             // ymm1     = s0[1]*(k*x[1]+y0) + d0[1]
            __ASM_EMIT("vmovups             %%ymm0, 0x00(%[dst])")
            __ASM_EMIT("vmovups             %%ymm1, 0x20(%[dst])")
            __ASM_EMIT("add                 $0x40, %[src1]")
            __ASM_EMIT("add                 $0x40, %[src2]")
            __ASM_EMIT("add                 $0x40, %[dst]")
            __ASM_EMIT("sub                 $16, %[n]")
            __ASM_EMIT("jae                 1b")

            // 8x block
            __ASM_EMIT("2:")
            __ASM_EMIT("add                 $8, %[n]")
            __ASM_EMIT("jl                  4f")
            __ASM_EMIT("vextractf128        $1, %%ymm4, %%xmm3")                        // xmm3     = x[1]
            __ASM_EMIT("vmulps              %%xmm7, %%xmm4, %%xmm0")                    // xmm0     = k*x[0]
            __ASM_EMIT("vmulps              %%xmm7, %%xmm3, %%xmm1")                    // xmm1     = k*x[1]
            __ASM_EMIT("vmovaps             %%ymm5, %%ymm4")                            // ymm4     = x[0]
            __ASM_EMIT("vaddps              %%xmm6, %%xmm0, %%xmm0")                    // xmm0     = k*x[0]+y0
            __ASM_EMIT("vaddps              %%xmm6, %%xmm1, %%xmm1")                    // xmm1     = k*x[1]+y0
            __ASM_EMIT("vmulps              0x00(%[src1]), %%xmm0, %%xmm0")             // xmm0     = s0[0]*(k*x[0]+y0)
            __ASM_EMIT("vmulps              0x10(%[src1]), %%xmm1, %%xmm1")             // xmm1     = s0[1]*(k*x[1]+y0)
            __ASM_EMIT("vaddps              0x00(%[src2]), %%xmm0, %%xmm0")             // xmm0     = s0[0]*(k*x[0]+y0) + d0[0]
            __ASM_EMIT("vaddps              0x10(%[src2]), %%xmm1, %%xmm1")             // xmm1     = s0[1]*(k*x[1]+y0) + d0[1]
            __ASM_EMIT("vmovups             %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("vmovups             %%xmm1, 0x10(%[dst])")
            __ASM_EMIT("add                 $0x20, %[src1]")
            __ASM_EMIT("add                 $0x20, %[src2]")
            __ASM_EMIT("add                 $0x20, %[dst]")
            __ASM_EMIT("sub                 $8, %[n]")

            // 4x block
            __ASM_EMIT("4:")
            __ASM_EMIT("add                 $4, %[n]")
            __ASM_EMIT("jl                  6f")
            __ASM_EMIT("vmulps              %%xmm7, %%xmm4, %%xmm0")                    // xmm0     = k*x[0]
            __ASM_EMIT("vextractf128        $1, %%ymm4, %%xmm4")                        // xmm4     = x[0]
            __ASM_EMIT("vaddps              %%xmm6, %%xmm0, %%xmm0")                    // xmm0     = k*x[0]+y0
            __ASM_EMIT("vmulps              0x00(%[src1]), %%xmm0, %%xmm0")             // xmm0     = s0[0]*(k*x[0]+y0)
            __ASM_EMIT("vaddps              0x00(%[src2]), %%xmm0, %%xmm0")             // xmm0     = s0[0]*(k*x[0]+y0) + d0[0]
            __ASM_EMIT("vmovups             %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("add                 $0x10, %[src1]")
            __ASM_EMIT("add                 $0x10, %[src2]")
            __ASM_EMIT("add                 $0x10, %[dst]")
            __ASM_EMIT("sub                 $4, %[n]")

            // 1x blocks
            __ASM_EMIT("6:")
            __ASM_EMIT("add                 $3, %[n]")
            __ASM_EMIT("jl                  8f")
            __ASM_EMIT("7:")
            __ASM_EMIT("vmulss              %%xmm7, %%xmm4, %%xmm0")                    // xmm0     = k*x[0]
            __ASM_EMIT("vshufps             $0x39, %%xmm4, %%xmm4, %%xmm4")             // xmm4     = x[0]'
            __ASM_EMIT("vaddss              %%xmm6, %%xmm0, %%xmm0")                    // xmm0     = k*x[0]+y0
            __ASM_EMIT("vmulss              0x00(%[src1]), %%xmm0, %%xmm0")             // ymm0     = s0[0]*(k*x[0]+y0)
            __ASM_EMIT("vaddss              0x00(%[src2]), %%xmm0, %%xmm0")             // xmm0     = s0[0]*(k*x[0]+y0) + d0[0]
            __ASM_EMIT("vmovss              %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("add                 $0x04, %[src1]")
            __ASM_EMIT("add                 $0x04, %[src2]")
            __ASM_EMIT("add                 $0x04, %[dst]")
            __ASM_EMIT("dec                 %[n]")
            __ASM_EMIT("jge                 7b")

            __ASM_EMIT("8:")
            : [n] "+r" (n), [dst] "+r" (dst), [src1] "+r" (src1), [src2] "+r" (src2)
            : [CC] "r" (lin_inter_const),
              [dx] "m" (dx),
              [dy] "m" (dy),
              [xx] "m" (xx),
              [y0] "m" (y0)
            : "cc", "memory",
              "xmm0", "xmm1", "xmm2", "xmm3",
              "xmm4", "xmm5", "xmm6", "xmm7"
        );
    }
}

#endif /* DSP_ARCH_X86_AVX_INTERPOLATE_H_ */
