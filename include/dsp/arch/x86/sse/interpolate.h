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
        4.0f, 5.0f, 6.0f, 7.0f, // Initial values 4..7
        8.0f, 8.0f, 8.0f, 8.0f  // Step
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
            __ASM_EMIT("xor         %[off], %[off]")
            __ASM_EMIT("movss       %[dy], %%xmm7")                     // xmm7     = dy
            __ASM_EMIT("movss       %[dx], %%xmm0")                     // xmm0     = dx
            __ASM_EMIT("movss       %[y0], %%xmm6")                     // xmm6     = y0
            __ASM_EMIT("movss       %[xx], %%xmm3")                     // xmm3     = xx
            __ASM_EMIT("divss       %%xmm0, %%xmm7")                    // xmm7     = k = dy / dx
            __ASM_EMIT("shufps      $0x00, %%xmm6, %%xmm6")             // xmm6     = y0
            __ASM_EMIT("shufps      $0x00, %%xmm3, %%xmm3")             // xmm3     = xx
            __ASM_EMIT("shufps      $0x00, %%xmm7, %%xmm7")             // xmm7     = k
            __ASM_EMIT("movaps      %%xmm3, %%xmm4")                    // xmm4     = xx
            __ASM_EMIT("addps       0x00(%[CC]), %%xmm3")               // xmm3     = x[0]
            __ASM_EMIT("addps       0x10(%[CC]), %%xmm4")               // xmm4     = x[1]
            __ASM_EMIT("movaps      0x20(%[CC]), %%xmm5")               // xmm5     = d

            __ASM_EMIT("sub         $8, %[n]")
            __ASM_EMIT("jb          2f")
            // 8x blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("movaps      %%xmm3, %%xmm0")                    // xmm0     = x[0]
            __ASM_EMIT("movaps      %%xmm4, %%xmm1")                    // xmm1     = x[1]
            __ASM_EMIT("mulps       %%xmm7, %%xmm0")                    // xmm0     = k*x[0]
            __ASM_EMIT("addps       %%xmm5, %%xmm3")                    // xmm3     = x[0]' = x[0] + d
            __ASM_EMIT("mulps       %%xmm7, %%xmm1")                    // xmm1     = k*x[1]
            __ASM_EMIT("addps       %%xmm5, %%xmm4")                    // xmm4     = x[1]' = x[1] + d
            __ASM_EMIT("addps       %%xmm6, %%xmm0")                    // xmm0     = k*x[0] + y0
            __ASM_EMIT("addps       %%xmm6, %%xmm1")                    // xmm1     = k*x[1] + y0
            __ASM_EMIT("movups      %%xmm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("movups      %%xmm1, 0x10(%[dst], %[off])")
            __ASM_EMIT("add         $0x20, %[off]")
            __ASM_EMIT("sub         $8, %[n]")
            __ASM_EMIT("jae         1b")

            // 4x block
            __ASM_EMIT("2:")
            __ASM_EMIT("add         $4, %[n]")
            __ASM_EMIT("jl          4f")
            __ASM_EMIT("movaps      %%xmm3, %%xmm0")                    // xmm0     = x[0]
            __ASM_EMIT("mulps       %%xmm7, %%xmm0")                    // xmm0     = k*x[0]
            __ASM_EMIT("movaps      %%xmm4, %%xmm3")                    // xmm3     = x[0]' = x[1]
            __ASM_EMIT("addps       %%xmm6, %%xmm0")                    // xmm0     = k*x[0] + y0
            __ASM_EMIT("movups      %%xmm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("add         $0x10, %[off]")
            __ASM_EMIT("sub         $4, %[n]")

            // 1x blocks
            __ASM_EMIT("4:")
            __ASM_EMIT("add         $3, %[n]")
            __ASM_EMIT("jl          6f")
            __ASM_EMIT("5:")
            __ASM_EMIT("movss       %%xmm3, %%xmm0")                    // xmm0     = x
            __ASM_EMIT("mulss       %%xmm7, %%xmm0")                    // xmm0     = k*x
            __ASM_EMIT("shufps      $0x39, %%xmm3, %%xmm3")             // xmm3     = x' = x + d
            __ASM_EMIT("addss       %%xmm6, %%xmm0")                    // xmm0     = k*x + y0
            __ASM_EMIT("movss       %%xmm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("add         $0x04, %[off]")
            __ASM_EMIT("dec         %[n]")
            __ASM_EMIT("jge         5b")

            __ASM_EMIT("6:")
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
            __ASM_EMIT("xor         %[off], %[off]")
            __ASM_EMIT("movss       %[dy], %%xmm7")                     // xmm7     = dy
            __ASM_EMIT("movss       %[dx], %%xmm0")                     // xmm0     = dx
            __ASM_EMIT("movss       %[y0], %%xmm6")                     // xmm6     = y0
            __ASM_EMIT("movss       %[xx], %%xmm4")                     // xmm4     = xx
            __ASM_EMIT("divss       %%xmm0, %%xmm7")                    // xmm7     = k = dy / dx
            __ASM_EMIT("shufps      $0x00, %%xmm6, %%xmm6")             // xmm6     = y0
            __ASM_EMIT("shufps      $0x00, %%xmm4, %%xmm4")             // xmm4     = xx
            __ASM_EMIT("shufps      $0x00, %%xmm7, %%xmm7")             // xmm7     = k
            __ASM_EMIT("movaps      %%xmm4, %%xmm5")                    // xmm5     = xx
            __ASM_EMIT("addps       0x00(%[CC]), %%xmm4")               // xmm4     = x[0]
            __ASM_EMIT("addps       0x10(%[CC]), %%xmm5")               // xmm5     = x[1]

            __ASM_EMIT("sub         $8, %[n]")
            __ASM_EMIT("jb          2f")
            // 8x blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("movaps      %%xmm4, %%xmm0")                    // xmm0     = x[0]
            __ASM_EMIT("movaps      %%xmm5, %%xmm1")                    // xmm1     = x[1]
            __ASM_EMIT("movups      0x00(%[dst], %[off]), %%xmm2")      // xmm2     = s0[0]
            __ASM_EMIT("movups      0x10(%[dst], %[off]), %%xmm3")      // xmm3     = x0[1]
            __ASM_EMIT("mulps       %%xmm7, %%xmm0")                    // xmm0     = k*x[0]
            __ASM_EMIT("addps       0x20(%[CC]), %%xmm4")               // xmm4     = x[0]' = x[0] + d
            __ASM_EMIT("mulps       %%xmm7, %%xmm1")                    // xmm1     = k*x[1]
            __ASM_EMIT("addps       0x20(%[CC]), %%xmm5")               // xmm5     = x[1]' = x[1] + d
            __ASM_EMIT("addps       %%xmm6, %%xmm0")                    // xmm0     = k*x[0] + y0
            __ASM_EMIT("addps       %%xmm6, %%xmm1")                    // xmm1     = k*x[1] + y0
            __ASM_EMIT("mulps       %%xmm2, %%xmm0")                    // xmm0     = s0[0]*(k*x[0] + y0)
            __ASM_EMIT("mulps       %%xmm3, %%xmm1")                    // xmm1     = s0[1]*(k*x[1] + y0)
            __ASM_EMIT("movups      %%xmm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("movups      %%xmm1, 0x10(%[dst], %[off])")
            __ASM_EMIT("add         $0x20, %[off]")
            __ASM_EMIT("sub         $8, %[n]")
            __ASM_EMIT("jae         1b")

            // 4x block
            __ASM_EMIT("2:")
            __ASM_EMIT("add         $4, %[n]")
            __ASM_EMIT("jl          4f")
            __ASM_EMIT("movaps      %%xmm4, %%xmm0")                    // xmm0     = x[0]
            __ASM_EMIT("movups      0x00(%[dst], %[off]), %%xmm2")      // xmm2     = s0[0]
            __ASM_EMIT("mulps       %%xmm7, %%xmm0")                    // xmm0     = k*x[0]
            __ASM_EMIT("movaps      %%xmm5, %%xmm4")                    // xmm4     = x[0]' = x[1]
            __ASM_EMIT("addps       %%xmm6, %%xmm0")                    // xmm0     = k*x[0] + y0
            __ASM_EMIT("mulps       %%xmm2, %%xmm0")                    // xmm0     = s0[0]*(k*x[0] + y0)
            __ASM_EMIT("movups      %%xmm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("add         $0x10, %[off]")
            __ASM_EMIT("sub         $4, %[n]")

            // 1x blocks
            __ASM_EMIT("4:")
            __ASM_EMIT("add         $3, %[n]")
            __ASM_EMIT("jl          6f")
            __ASM_EMIT("5:")
            __ASM_EMIT("movss       %%xmm4, %%xmm0")                    // xmm0     = x
            __ASM_EMIT("movss       0x00(%[dst], %[off]), %%xmm2")      // xmm2     = s0
            __ASM_EMIT("mulss       %%xmm7, %%xmm0")                    // xmm0     = k*x
            __ASM_EMIT("shufps      $0x39, %%xmm4, %%xmm4")             // xmm4     = x' = x + d
            __ASM_EMIT("addss       %%xmm6, %%xmm0")                    // xmm0     = k*x + y0
            __ASM_EMIT("mulss       %%xmm2, %%xmm0")                    // xmm0     = s0*(k*x + y0)
            __ASM_EMIT("movss       %%xmm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("add         $0x04, %[off]")
            __ASM_EMIT("dec         %[n]")
            __ASM_EMIT("jge         5b")

            __ASM_EMIT("6:")
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
            __ASM_EMIT("xor         %[off], %[off]")
            __ASM_EMIT("movss       %[dy], %%xmm7")                     // xmm7     = dy
            __ASM_EMIT("movss       %[dx], %%xmm0")                     // xmm0     = dx
            __ASM_EMIT("movss       %[y0], %%xmm6")                     // xmm6     = y0
            __ASM_EMIT("movss       %[xx], %%xmm4")                     // xmm4     = xx
            __ASM_EMIT("divss       %%xmm0, %%xmm7")                    // xmm7     = k = dy / dx
            __ASM_EMIT("shufps      $0x00, %%xmm6, %%xmm6")             // xmm6     = y0
            __ASM_EMIT("shufps      $0x00, %%xmm4, %%xmm4")             // xmm4     = xx
            __ASM_EMIT("shufps      $0x00, %%xmm7, %%xmm7")             // xmm7     = k
            __ASM_EMIT("movaps      %%xmm4, %%xmm5")                    // xmm5     = xx
            __ASM_EMIT("addps       0x00(%[CC]), %%xmm4")               // xmm4     = x[0]
            __ASM_EMIT("addps       0x10(%[CC]), %%xmm5")               // xmm5     = x[1]

            __ASM_EMIT("sub         $8, %[n]")
            __ASM_EMIT("jb          2f")
            // 8x blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("movaps      %%xmm4, %%xmm0")                    // xmm0     = x[0]
            __ASM_EMIT("movaps      %%xmm5, %%xmm1")                    // xmm1     = x[1]
            __ASM_EMIT("movups      0x00(%[src], %[off]), %%xmm2")      // xmm2     = s0[0]
            __ASM_EMIT("movups      0x10(%[src], %[off]), %%xmm3")      // xmm3     = x0[1]
            __ASM_EMIT("mulps       %%xmm7, %%xmm0")                    // xmm0     = k*x[0]
            __ASM_EMIT("addps       0x20(%[CC]), %%xmm4")               // xmm4     = x[0]' = x[0] + d
            __ASM_EMIT("mulps       %%xmm7, %%xmm1")                    // xmm1     = k*x[1]
            __ASM_EMIT("addps       0x20(%[CC]), %%xmm5")               // xmm5     = x[1]' = x[1] + d
            __ASM_EMIT("addps       %%xmm6, %%xmm0")                    // xmm0     = k*x[0] + y0
            __ASM_EMIT("addps       %%xmm6, %%xmm1")                    // xmm1     = k*x[1] + y0
            __ASM_EMIT("mulps       %%xmm2, %%xmm0")                    // xmm0     = s0[0]*(k*x[0] + y0)
            __ASM_EMIT("mulps       %%xmm3, %%xmm1")                    // xmm1     = s0[1]*(k*x[1] + y0)
            __ASM_EMIT("movups      %%xmm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("movups      %%xmm1, 0x10(%[dst], %[off])")
            __ASM_EMIT("add         $0x20, %[off]")
            __ASM_EMIT("sub         $8, %[n]")
            __ASM_EMIT("jae         1b")

            // 4x block
            __ASM_EMIT("2:")
            __ASM_EMIT("add         $4, %[n]")
            __ASM_EMIT("jl          4f")
            __ASM_EMIT("movaps      %%xmm4, %%xmm0")                    // xmm0     = x[0]
            __ASM_EMIT("movups      0x00(%[src], %[off]), %%xmm2")      // xmm2     = s0[0]
            __ASM_EMIT("mulps       %%xmm7, %%xmm0")                    // xmm0     = k*x[0]
            __ASM_EMIT("movaps      %%xmm5, %%xmm4")                    // xmm4     = x[0]' = x[1]
            __ASM_EMIT("addps       %%xmm6, %%xmm0")                    // xmm0     = k*x[0] + y0
            __ASM_EMIT("mulps       %%xmm2, %%xmm0")                    // xmm0     = s0[0]*(k*x[0] + y0)
            __ASM_EMIT("movups      %%xmm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("add         $0x10, %[off]")
            __ASM_EMIT("sub         $4, %[n]")

            // 1x blocks
            __ASM_EMIT("4:")
            __ASM_EMIT("add         $3, %[n]")
            __ASM_EMIT("jl          6f")
            __ASM_EMIT("5:")
            __ASM_EMIT("movss       %%xmm4, %%xmm0")                    // xmm0     = x
            __ASM_EMIT("movss       0x00(%[src], %[off]), %%xmm2")      // xmm2     = s0
            __ASM_EMIT("mulss       %%xmm7, %%xmm0")                    // xmm0     = k*x
            __ASM_EMIT("shufps      $0x39, %%xmm4, %%xmm4")             // xmm4     = x' = x + d
            __ASM_EMIT("addss       %%xmm6, %%xmm0")                    // xmm0     = k*x + y0
            __ASM_EMIT("mulss       %%xmm2, %%xmm0")                    // xmm0     = s0*(k*x + y0)
            __ASM_EMIT("movss       %%xmm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("add         $0x04, %[off]")
            __ASM_EMIT("dec         %[n]")
            __ASM_EMIT("jge         5b")

            __ASM_EMIT("6:")
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
            __ASM_EMIT("xor         %[off], %[off]")
            __ASM_EMIT("movss       %[dy], %%xmm7")                     // xmm7     = dy
            __ASM_EMIT("movss       %[dx], %%xmm0")                     // xmm0     = dx
            __ASM_EMIT("movss       %[y0], %%xmm6")                     // xmm6     = y0
            __ASM_EMIT("movss       %[xx], %%xmm4")                     // xmm4     = xx
            __ASM_EMIT("divss       %%xmm0, %%xmm7")                    // xmm7     = k = dy / dx
            __ASM_EMIT("shufps      $0x00, %%xmm6, %%xmm6")             // xmm6     = y0
            __ASM_EMIT("shufps      $0x00, %%xmm4, %%xmm4")             // xmm4     = xx
            __ASM_EMIT("shufps      $0x00, %%xmm7, %%xmm7")             // xmm7     = k
            __ASM_EMIT("movaps      %%xmm4, %%xmm5")                    // xmm5     = xx
            __ASM_EMIT("addps       0x00(%[CC]), %%xmm4")               // xmm4     = x[0]
            __ASM_EMIT("addps       0x10(%[CC]), %%xmm5")               // xmm5     = x[1]

            __ASM_EMIT("sub         $8, %[n]")
            __ASM_EMIT("jb          2f")
            // 8x blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("movaps      %%xmm4, %%xmm0")                    // xmm0     = x[0]
            __ASM_EMIT("movaps      %%xmm5, %%xmm1")                    // xmm1     = x[1]
            __ASM_EMIT("movups      0x00(%[src], %[off]), %%xmm2")      // xmm2     = s0[0]
            __ASM_EMIT("movups      0x10(%[src], %[off]), %%xmm3")      // xmm3     = x0[1]
            __ASM_EMIT("mulps       %%xmm7, %%xmm0")                    // xmm0     = k*x[0]
            __ASM_EMIT("addps       0x20(%[CC]), %%xmm4")               // xmm4     = x[0]' = x[0] + d
            __ASM_EMIT("mulps       %%xmm7, %%xmm1")                    // xmm1     = k*x[1]
            __ASM_EMIT("addps       0x20(%[CC]), %%xmm5")               // xmm5     = x[1]' = x[1] + d
            __ASM_EMIT("addps       %%xmm6, %%xmm0")                    // xmm0     = k*x[0] + y0
            __ASM_EMIT("addps       %%xmm6, %%xmm1")                    // xmm1     = k*x[1] + y0
            __ASM_EMIT("mulps       %%xmm2, %%xmm0")                    // xmm0     = s0[0]*(k*x[0] + y0)
            __ASM_EMIT("mulps       %%xmm3, %%xmm1")                    // xmm1     = s0[1]*(k*x[1] + y0)
            __ASM_EMIT("movups      0x00(%[dst], %[off]), %%xmm2")      // xmm2     = d0[0]
            __ASM_EMIT("movups      0x10(%[dst], %[off]), %%xmm3")      // xmm3     = d0[1]
            __ASM_EMIT("addps       %%xmm2, %%xmm0")                    // xmm0     = d0[0] + s0[0]*(k*x[0] + y0)
            __ASM_EMIT("addps       %%xmm3, %%xmm1")                    // xmm1     = d0[0] + s0[1]*(k*x[1] + y0)
            __ASM_EMIT("movups      %%xmm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("movups      %%xmm1, 0x10(%[dst], %[off])")
            __ASM_EMIT("add         $0x20, %[off]")
            __ASM_EMIT("sub         $8, %[n]")
            __ASM_EMIT("jae         1b")

            // 4x block
            __ASM_EMIT("2:")
            __ASM_EMIT("add         $4, %[n]")
            __ASM_EMIT("jl          4f")
            __ASM_EMIT("movaps      %%xmm4, %%xmm0")                    // xmm0     = x[0]
            __ASM_EMIT("movups      0x00(%[src], %[off]), %%xmm2")      // xmm2     = s0[0]
            __ASM_EMIT("movups      0x00(%[dst], %[off]), %%xmm3")      // xmm3     = d0[0]
            __ASM_EMIT("mulps       %%xmm7, %%xmm0")                    // xmm0     = k*x[0]
            __ASM_EMIT("movaps      %%xmm5, %%xmm4")                    // xmm4     = x[0]' = x[1]
            __ASM_EMIT("addps       %%xmm6, %%xmm0")                    // xmm0     = k*x[0] + y0
            __ASM_EMIT("mulps       %%xmm2, %%xmm0")                    // xmm0     = s0[0]*(k*x[0] + y0)
            __ASM_EMIT("addps       %%xmm3, %%xmm0")                    // xmm0     = d0[0] + s0[0]*(k*x[0] + y0)
            __ASM_EMIT("movups      %%xmm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("add         $0x10, %[off]")
            __ASM_EMIT("sub         $4, %[n]")

            // 1x blocks
            __ASM_EMIT("4:")
            __ASM_EMIT("add         $3, %[n]")
            __ASM_EMIT("jl          6f")
            __ASM_EMIT("5:")
            __ASM_EMIT("movss       %%xmm4, %%xmm0")                    // xmm0     = x
            __ASM_EMIT("movss       0x00(%[src], %[off]), %%xmm2")      // xmm2     = s0
            __ASM_EMIT("movss       0x00(%[dst], %[off]), %%xmm3")      // xmm3     = d0
            __ASM_EMIT("mulss       %%xmm7, %%xmm0")                    // xmm0     = k*x
            __ASM_EMIT("shufps      $0x39, %%xmm4, %%xmm4")             // xmm4     = x' = x + d
            __ASM_EMIT("addss       %%xmm6, %%xmm0")                    // xmm0     = k*x + y0
            __ASM_EMIT("mulss       %%xmm2, %%xmm0")                    // xmm0     = s0*(k*x + y0)
            __ASM_EMIT("addss       %%xmm3, %%xmm0")                    // xmm0     = d0 + s0*(k*x + y0)
            __ASM_EMIT("movss       %%xmm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("add         $0x04, %[off]")
            __ASM_EMIT("dec         %[n]")
            __ASM_EMIT("jge         5b")

            __ASM_EMIT("6:")
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
            __ASM_EMIT("xor         %[off], %[off]")
            __ASM_EMIT("movss       %[dy], %%xmm7")                     // xmm7     = dy
            __ASM_EMIT("movss       %[dx], %%xmm0")                     // xmm0     = dx
            __ASM_EMIT("movss       %[y0], %%xmm6")                     // xmm6     = y0
            __ASM_EMIT("movss       %[xx], %%xmm4")                     // xmm4     = xx
            __ASM_EMIT("divss       %%xmm0, %%xmm7")                    // xmm7     = k = dy / dx
            __ASM_EMIT("shufps      $0x00, %%xmm6, %%xmm6")             // xmm6     = y0
            __ASM_EMIT("shufps      $0x00, %%xmm4, %%xmm4")             // xmm4     = xx
            __ASM_EMIT("shufps      $0x00, %%xmm7, %%xmm7")             // xmm7     = k
            __ASM_EMIT("movaps      %%xmm4, %%xmm5")                    // xmm5     = xx
            __ASM_EMIT("addps       0x00(%[CC]), %%xmm4")               // xmm4     = x[0]
            __ASM_EMIT("addps       0x10(%[CC]), %%xmm5")               // xmm5     = x[1]

            __ASM_EMIT("sub         $8, %[n]")
            __ASM_EMIT("jb          2f")
            // 8x blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("movaps      %%xmm4, %%xmm0")                    // xmm0     = x[0]
            __ASM_EMIT("movaps      %%xmm5, %%xmm1")                    // xmm1     = x[1]
            __ASM_EMIT("movups      0x00(%[dst], %[off]), %%xmm2")      // xmm2     = s0[0]
            __ASM_EMIT("movups      0x10(%[dst], %[off]), %%xmm3")      // xmm3     = x0[1]
            __ASM_EMIT("mulps       %%xmm7, %%xmm0")                    // xmm0     = k*x[0]
            __ASM_EMIT("addps       0x20(%[CC]), %%xmm4")               // xmm4     = x[0]' = x[0] + d
            __ASM_EMIT("mulps       %%xmm7, %%xmm1")                    // xmm1     = k*x[1]
            __ASM_EMIT("addps       0x20(%[CC]), %%xmm5")               // xmm5     = x[1]' = x[1] + d
            __ASM_EMIT("addps       %%xmm6, %%xmm0")                    // xmm0     = k*x[0] + y0
            __ASM_EMIT("addps       %%xmm6, %%xmm1")                    // xmm1     = k*x[1] + y0
            __ASM_EMIT("mulps       %%xmm2, %%xmm0")                    // xmm0     = s0[0]*(k*x[0] + y0)
            __ASM_EMIT("mulps       %%xmm3, %%xmm1")                    // xmm1     = s0[1]*(k*x[1] + y0)
            __ASM_EMIT("movups      0x00(%[src], %[off]), %%xmm2")      // xmm2     = d0[0]
            __ASM_EMIT("movups      0x10(%[src], %[off]), %%xmm3")      // xmm3     = d0[1]
            __ASM_EMIT("addps       %%xmm2, %%xmm0")                    // xmm0     = d0[0] + s0[0]*(k*x[0] + y0)
            __ASM_EMIT("addps       %%xmm3, %%xmm1")                    // xmm1     = d0[0] + s0[1]*(k*x[1] + y0)
            __ASM_EMIT("movups      %%xmm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("movups      %%xmm1, 0x10(%[dst], %[off])")
            __ASM_EMIT("add         $0x20, %[off]")
            __ASM_EMIT("sub         $8, %[n]")
            __ASM_EMIT("jae         1b")

            // 4x block
            __ASM_EMIT("2:")
            __ASM_EMIT("add         $4, %[n]")
            __ASM_EMIT("jl          4f")
            __ASM_EMIT("movaps      %%xmm4, %%xmm0")                    // xmm0     = x[0]
            __ASM_EMIT("movups      0x00(%[dst], %[off]), %%xmm2")      // xmm2     = s0[0]
            __ASM_EMIT("movups      0x00(%[src], %[off]), %%xmm3")      // xmm3     = d0[0]
            __ASM_EMIT("mulps       %%xmm7, %%xmm0")                    // xmm0     = k*x[0]
            __ASM_EMIT("movaps      %%xmm5, %%xmm4")                    // xmm4     = x[0]' = x[1]
            __ASM_EMIT("addps       %%xmm6, %%xmm0")                    // xmm0     = k*x[0] + y0
            __ASM_EMIT("mulps       %%xmm2, %%xmm0")                    // xmm0     = s0[0]*(k*x[0] + y0)
            __ASM_EMIT("addps       %%xmm3, %%xmm0")                    // xmm0     = d0[0] + s0[0]*(k*x[0] + y0)
            __ASM_EMIT("movups      %%xmm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("add         $0x10, %[off]")
            __ASM_EMIT("sub         $4, %[n]")

            // 1x blocks
            __ASM_EMIT("4:")
            __ASM_EMIT("add         $3, %[n]")
            __ASM_EMIT("jl          6f")
            __ASM_EMIT("5:")
            __ASM_EMIT("movss       %%xmm4, %%xmm0")                    // xmm0     = x
            __ASM_EMIT("movss       0x00(%[dst], %[off]), %%xmm2")      // xmm2     = s0
            __ASM_EMIT("movss       0x00(%[src], %[off]), %%xmm3")      // xmm3     = d0
            __ASM_EMIT("mulss       %%xmm7, %%xmm0")                    // xmm0     = k*x
            __ASM_EMIT("shufps      $0x39, %%xmm4, %%xmm4")             // xmm4     = x' = x + d
            __ASM_EMIT("addss       %%xmm6, %%xmm0")                    // xmm0     = k*x + y0
            __ASM_EMIT("mulss       %%xmm2, %%xmm0")                    // xmm0     = s0*(k*x + y0)
            __ASM_EMIT("addss       %%xmm3, %%xmm0")                    // xmm0     = d0 + s0*(k*x + y0)
            __ASM_EMIT("movss       %%xmm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("add         $0x04, %[off]")
            __ASM_EMIT("dec         %[n]")
            __ASM_EMIT("jge         5b")

            __ASM_EMIT("6:")
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
            __ASM_EMIT("movss       %[dy], %%xmm7")                     // xmm7     = dy
            __ASM_EMIT("movss       %[dx], %%xmm0")                     // xmm0     = dx
            __ASM_EMIT("movss       %[y0], %%xmm6")                     // xmm6     = y0
            __ASM_EMIT("movss       %[xx], %%xmm4")                     // xmm4     = xx
            __ASM_EMIT("divss       %%xmm0, %%xmm7")                    // xmm7     = k = dy / dx
            __ASM_EMIT("shufps      $0x00, %%xmm6, %%xmm6")             // xmm6     = y0
            __ASM_EMIT("shufps      $0x00, %%xmm4, %%xmm4")             // xmm4     = xx
            __ASM_EMIT("shufps      $0x00, %%xmm7, %%xmm7")             // xmm7     = k
            __ASM_EMIT("movaps      %%xmm4, %%xmm5")                    // xmm5     = xx
            __ASM_EMIT("addps       0x00(%[CC]), %%xmm4")               // xmm4     = x[0]
            __ASM_EMIT("addps       0x10(%[CC]), %%xmm5")               // xmm5     = x[1]

            __ASM_EMIT("sub         $8, %[n]")
            __ASM_EMIT("jb          2f")
            // 8x blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("movaps      %%xmm4, %%xmm0")                    // xmm0     = x[0]
            __ASM_EMIT("movaps      %%xmm5, %%xmm1")                    // xmm1     = x[1]
            __ASM_EMIT("movups      0x00(%[src1]), %%xmm2")             // xmm2     = s0[0]
            __ASM_EMIT("movups      0x10(%[src1]), %%xmm3")             // xmm3     = x0[1]
            __ASM_EMIT("mulps       %%xmm7, %%xmm0")                    // xmm0     = k*x[0]
            __ASM_EMIT("addps       0x20(%[CC]), %%xmm4")               // xmm4     = x[0]' = x[0] + d
            __ASM_EMIT("mulps       %%xmm7, %%xmm1")                    // xmm1     = k*x[1]
            __ASM_EMIT("addps       0x20(%[CC]), %%xmm5")               // xmm5     = x[1]' = x[1] + d
            __ASM_EMIT("addps       %%xmm6, %%xmm0")                    // xmm0     = k*x[0] + y0
            __ASM_EMIT("addps       %%xmm6, %%xmm1")                    // xmm1     = k*x[1] + y0
            __ASM_EMIT("mulps       %%xmm2, %%xmm0")                    // xmm0     = s0[0]*(k*x[0] + y0)
            __ASM_EMIT("mulps       %%xmm3, %%xmm1")                    // xmm1     = s0[1]*(k*x[1] + y0)
            __ASM_EMIT("movups      0x00(%[src2]), %%xmm2")             // xmm2     = d0[0]
            __ASM_EMIT("movups      0x10(%[src2]), %%xmm3")             // xmm3     = d0[1]
            __ASM_EMIT("addps       %%xmm2, %%xmm0")                    // xmm0     = d0[0] + s0[0]*(k*x[0] + y0)
            __ASM_EMIT("addps       %%xmm3, %%xmm1")                    // xmm1     = d0[0] + s0[1]*(k*x[1] + y0)
            __ASM_EMIT("movups      %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("movups      %%xmm1, 0x10(%[dst])")
            __ASM_EMIT("add         $0x20, %[src1]")
            __ASM_EMIT("add         $0x20, %[src2]")
            __ASM_EMIT("add         $0x20, %[dst]")
            __ASM_EMIT("sub         $8, %[n]")
            __ASM_EMIT("jae         1b")

            // 4x block
            __ASM_EMIT("2:")
            __ASM_EMIT("add         $4, %[n]")
            __ASM_EMIT("jl          4f")
            __ASM_EMIT("movaps      %%xmm4, %%xmm0")                    // xmm0     = x[0]
            __ASM_EMIT("movups      0x00(%[src1]), %%xmm2")             // xmm2     = s0[0]
            __ASM_EMIT("movups      0x00(%[src2]), %%xmm3")             // xmm3     = d0[0]
            __ASM_EMIT("mulps       %%xmm7, %%xmm0")                    // xmm0     = k*x[0]
            __ASM_EMIT("movaps      %%xmm5, %%xmm4")                    // xmm4     = x[0]' = x[1]
            __ASM_EMIT("addps       %%xmm6, %%xmm0")                    // xmm0     = k*x[0] + y0
            __ASM_EMIT("mulps       %%xmm2, %%xmm0")                    // xmm0     = s0[0]*(k*x[0] + y0)
            __ASM_EMIT("addps       %%xmm3, %%xmm0")                    // xmm0     = d0[0] + s0[0]*(k*x[0] + y0)
            __ASM_EMIT("movups      %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("add         $0x10, %[src1]")
            __ASM_EMIT("add         $0x10, %[src2]")
            __ASM_EMIT("add         $0x10, %[dst]")
            __ASM_EMIT("sub         $4, %[n]")

            // 1x blocks
            __ASM_EMIT("4:")
            __ASM_EMIT("add         $3, %[n]")
            __ASM_EMIT("jl          6f")
            __ASM_EMIT("5:")
            __ASM_EMIT("movss       %%xmm4, %%xmm0")                    // xmm0     = x
            __ASM_EMIT("movss       0x00(%[src1]), %%xmm2")             // xmm2     = s0
            __ASM_EMIT("movss       0x00(%[src2]), %%xmm3")             // xmm3     = d0
            __ASM_EMIT("mulss       %%xmm7, %%xmm0")                    // xmm0     = k*x
            __ASM_EMIT("shufps      $0x39, %%xmm4, %%xmm4")             // xmm4     = x' = x + d
            __ASM_EMIT("addss       %%xmm6, %%xmm0")                    // xmm0     = k*x + y0
            __ASM_EMIT("mulss       %%xmm2, %%xmm0")                    // xmm0     = s0*(k*x + y0)
            __ASM_EMIT("addss       %%xmm3, %%xmm0")                    // xmm0     = d0 + s0*(k*x + y0)
            __ASM_EMIT("movss       %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("add         $0x04, %[src1]")
            __ASM_EMIT("add         $0x04, %[src2]")
            __ASM_EMIT("add         $0x04, %[dst]")
            __ASM_EMIT("dec         %[n]")
            __ASM_EMIT("jge         5b")

            __ASM_EMIT("6:")
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



#endif /* DSP_ARCH_X86_SSE_INTERPOLATE_H_ */
