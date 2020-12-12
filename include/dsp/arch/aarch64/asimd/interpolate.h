/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 13 дек. 2020 г.
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

#ifndef DSP_ARCH_AARCH64_ASIMD_INTERPOLATE_H_
#define DSP_ARCH_AARCH64_ASIMD_INTERPOLATE_H_

namespace asimd
{
    static const float lin_inter_const[] __lsp_aligned16 =
    {
        0.0f, 1.0f,  2.0f, 3.0f,  4.0f,  5.0f,  6.0f,  7.0f,    // Initial values 0..7
        8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f,   // Initial values 8..15
        16.0f, 16.0f, 16.0f, 16.0f, 16.0f, 16.0f, 16.0f, 16.0f  // Step
    };

    void lin_inter_set(float *dst, int32_t x0, float y0, int32_t x1, float y1, int32_t x, uint32_t n)
    {
        IF_ARCH_AARCH64(
            float v[4];
            v[0]            = x1 - x0;
            v[1]            = x - x0;
            v[2]            = y1 - y0;
            v[3]            = y0;
        )

        ARCH_AARCH64_ASM(
            // Prepare parameters
            __ASM_EMIT("ld4r            {v12.4s, v13.4s, v14.4s, v15.4s}, [%[v]]")  // v12 = (x1-x0), v13 = (x-x0), v14 = (y1-y0), v15 = y0
            __ASM_EMIT("ldp             q8, q9, [%[CC], #0x00]")                    // v8  = 0..3, v9=4..7
            __ASM_EMIT("ldp             q10, q11, [%[CC], #0x20]")                  // v10 = 8..11, v11=12..15
            __ASM_EMIT("frecpe          v0.4s, v12.4s")                             // v0  = s2
            __ASM_EMIT("fadd            v8.4s, v8.4s, v13.4s")                      // v8  = x[0]
            __ASM_EMIT("frecps          v1.4s, v0.4s, v12.4s")                      // v1  = (2 - R*s2)
            __ASM_EMIT("fadd            v9.4s, v9.4s, v13.4s")                      // v9  = x[1]
            __ASM_EMIT("fmul            v0.4s, v1.4s, v0.4s")                       // v0  = s2' = s2 * (2 - R*s2)
            __ASM_EMIT("fadd            v10.4s, v10.4s, v13.4s")                    // v10 = x[2]
            __ASM_EMIT("frecps          v1.4s, v0.4s, v12.4s")                      // v1  = (2 - R*s2')
            __ASM_EMIT("fadd            v11.4s, v11.4s, v13.4s")                    // v11 = x[3]
            __ASM_EMIT("fmul            v12.4s, v1.4s, v0.4s")                      // v14 = s2" = s2' * (2 - R*s2) = 1/s2
            __ASM_EMIT("fmul            v14.4s, v14.4s, v12.4s")                    // v14 = k = (y1-y0)/(x1-x0)
            __ASM_EMIT("ldp             q12, q13, [%[CC], #0x40]")                  // v12 = d[0], v13=d[1]

            // Register allocation:
            // v8  = x[0]
            // v9  = x[1]
            // v10 = x[2]
            // v11 = x[3]
            // v12 = d[0]
            // v13 = d[1]
            // v14 = k
            // v15 = y0
            __ASM_EMIT("subs            %[n], %[n], #16")
            __ASM_EMIT("b.lo            2f")

            // 16x blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("fmul            v4.4s, v8.4s, v14.4s")                      // v4  = k*x[0]
            __ASM_EMIT("fmul            v5.4s, v9.4s, v14.4s")                      // v5  = k*x[1]
            __ASM_EMIT("fmul            v6.4s, v10.4s, v14.4s")                     // v6  = k*x[2]
            __ASM_EMIT("fmul            v7.4s, v11.4s, v14.4s")                     // v7  = k*x[3]
            __ASM_EMIT("fadd            v0.4s, v4.4s, v15.4s")                      // v0  = k*x[0] + y0
            __ASM_EMIT("fadd            v1.4s, v5.4s, v15.4s")                      // v1  = k*x[1] + y0
            __ASM_EMIT("fadd            v2.4s, v6.4s, v15.4s")                      // v2  = k*x[2] + y0
            __ASM_EMIT("fadd            v3.4s, v7.4s, v15.4s")                      // v3  = k*x[3] + y0
            __ASM_EMIT("fadd            v8.4s, v8.4s, v12.4s")                      // v8  = x[0]' = x[0]+d[0]
            __ASM_EMIT("fadd            v9.4s, v9.4s, v13.4s")                      // v9  = x[1]' = x[1]+d[1]
            __ASM_EMIT("fadd            v10.4s, v10.4s, v12.4s")                    // v10 = x[2]' = x[2]+d[0]
            __ASM_EMIT("fadd            v11.4s, v11.4s, v13.4s")                    // v11 = x[3]' = x[3]+d[1]
            __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("stp             q2, q3, [%[dst], #0x20]")
            __ASM_EMIT("subs            %[n], %[n], #16")
            __ASM_EMIT("add             %[dst], %[dst], #0x40")
            __ASM_EMIT("b.hs            1b")

            // 8x block
            __ASM_EMIT("2:")
            __ASM_EMIT("adds            %[n], %[n], #8")
            __ASM_EMIT("b.lt            4f")
            __ASM_EMIT("fmul            v4.4s, v8.4s, v14.4s")                      // v4  = k*x[0]
            __ASM_EMIT("fmul            v5.4s, v9.4s, v14.4s")                      // v5  = k*x[1]
            __ASM_EMIT("fadd            v0.4s, v4.4s, v15.4s")                      // v0  = k*x[0] + y0
            __ASM_EMIT("fadd            v1.4s, v5.4s, v15.4s")                      // v1  = k*x[1] + y0
            __ASM_EMIT("mov             v8.16b, v10.16b")                           // v8  = x[0]'
            __ASM_EMIT("mov             v9.16b, v11.16b")                           // v9  = x[1]'
            __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("sub             %[n], %[n], #8")
            __ASM_EMIT("add             %[dst], %[dst], #0x20")

            // 4x block
            __ASM_EMIT("4:")
            __ASM_EMIT("adds            %[n], %[n], #4")
            __ASM_EMIT("b.lt            6f")
            __ASM_EMIT("fmul            v4.4s, v8.4s, v14.4s")                      // v4  = k*x[0]
            __ASM_EMIT("fadd            v0.4s, v4.4s, v15.4s")                      // v0  = k*x[0] + y0
            __ASM_EMIT("mov             v8.16b, v9.16b")                            // v8  = x[0]'
            __ASM_EMIT("str             q0, [%[dst], #0x00]")
            __ASM_EMIT("sub             %[n], %[n], #4")
            __ASM_EMIT("add             %[dst], %[dst], #0x10")

            // 1x blocks
            __ASM_EMIT("6:")
            __ASM_EMIT("adds            %[n], %[n], #3")
            __ASM_EMIT("b.lt            8f")
            __ASM_EMIT("7:")
            __ASM_EMIT("fmul            v4.4s, v8.4s, v14.4s")                      // v4  = k*x[0]
            __ASM_EMIT("fadd            v0.4s, v4.4s, v15.4s")                      // v0  = k*x[0] + y0
            __ASM_EMIT("ext             v8.16b, v8.16b, v8.16b, #4")                // v8  = x[0]'
            __ASM_EMIT("st1             {v0.s}[0], [%[dst]]")
            __ASM_EMIT("subs            %[n], %[n], #1")
            __ASM_EMIT("add             %[dst], %[dst], #0x04")
            __ASM_EMIT("b.ge            7b")

            __ASM_EMIT("8:")
            : [dst] "+r" (dst),
              [n] "+r" (n)
            : [v] "r" (&v[0]),
              [CC] "r" (&lin_inter_const[0])
            : "cc", "memory",
              "v0", "v1", "v2", "v3",
              "v4", "v5", "v6", "v7",
              "v8", "v9", "v10", "v11",
              "v12", "v13", "v14", "v15"
        );
    }

    void lin_inter_mul2(float *dst, int32_t x0, float y0, int32_t x1, float y1, int32_t x, uint32_t n)
    {
        IF_ARCH_AARCH64(
            float v[4];
            v[0]            = x1 - x0;
            v[1]            = x - x0;
            v[2]            = y1 - y0;
            v[3]            = y0;
        )

        ARCH_AARCH64_ASM(
            // Prepare parameters
            __ASM_EMIT("ld4r            {v12.4s, v13.4s, v14.4s, v15.4s}, [%[v]]")  // v12 = (x1-x0), v13 = (x-x0), v14 = (y1-y0), v15 = y0
            __ASM_EMIT("ldp             q8, q9, [%[CC], #0x00]")                    // v8  = 0..3, v9=4..7
            __ASM_EMIT("ldp             q10, q11, [%[CC], #0x20]")                  // v10 = 8..11, v11=12..15
            __ASM_EMIT("frecpe          v0.4s, v12.4s")                             // v0  = s2
            __ASM_EMIT("fadd            v8.4s, v8.4s, v13.4s")                      // v8  = x[0]
            __ASM_EMIT("frecps          v1.4s, v0.4s, v12.4s")                      // v1  = (2 - R*s2)
            __ASM_EMIT("fadd            v9.4s, v9.4s, v13.4s")                      // v9  = x[1]
            __ASM_EMIT("fmul            v0.4s, v1.4s, v0.4s")                       // v0  = s2' = s2 * (2 - R*s2)
            __ASM_EMIT("fadd            v10.4s, v10.4s, v13.4s")                    // v10 = x[2]
            __ASM_EMIT("frecps          v1.4s, v0.4s, v12.4s")                      // v1  = (2 - R*s2')
            __ASM_EMIT("fadd            v11.4s, v11.4s, v13.4s")                    // v11 = x[3]
            __ASM_EMIT("fmul            v12.4s, v1.4s, v0.4s")                      // v14 = s2" = s2' * (2 - R*s2) = 1/s2
            __ASM_EMIT("fmul            v14.4s, v14.4s, v12.4s")                    // v14 = k = (y1-y0)/(x1-x0)
            __ASM_EMIT("ldp             q12, q13, [%[CC], #0x40]")                  // v12 = d[0], v13=d[1]

            // Register allocation:
            // v8  = x[0]
            // v9  = x[1]
            // v10 = x[2]
            // v11 = x[3]
            // v12 = d[0]
            // v13 = d[1]
            // v14 = k
            // v15 = y0
            __ASM_EMIT("subs            %[n], %[n], #16")
            __ASM_EMIT("b.lo            2f")

            // 16x blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("ldp             q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("ldp             q2, q3, [%[dst], #0x20]")
            __ASM_EMIT("fmul            v4.4s, v8.4s, v14.4s")                      // v4  = k*x[0]
            __ASM_EMIT("fmul            v5.4s, v9.4s, v14.4s")                      // v5  = k*x[1]
            __ASM_EMIT("fmul            v6.4s, v10.4s, v14.4s")                     // v6  = k*x[2]
            __ASM_EMIT("fmul            v7.4s, v11.4s, v14.4s")                     // v7  = k*x[3]
            __ASM_EMIT("fadd            v4.4s, v4.4s, v15.4s")                      // v0  = k*x[0] + y0
            __ASM_EMIT("fadd            v5.4s, v5.4s, v15.4s")                      // v1  = k*x[1] + y0
            __ASM_EMIT("fadd            v6.4s, v6.4s, v15.4s")                      // v2  = k*x[2] + y0
            __ASM_EMIT("fadd            v7.4s, v7.4s, v15.4s")                      // v3  = k*x[3] + y0
            __ASM_EMIT("fadd            v8.4s, v8.4s, v12.4s")                      // v8  = x[0]' = x[0]+d[0]
            __ASM_EMIT("fadd            v9.4s, v9.4s, v13.4s")                      // v9  = x[1]' = x[1]+d[1]
            __ASM_EMIT("fadd            v10.4s, v10.4s, v12.4s")                    // v10 = x[2]' = x[2]+d[0]
            __ASM_EMIT("fadd            v11.4s, v11.4s, v13.4s")                    // v11 = x[3]' = x[3]+d[1]
            __ASM_EMIT("fmul            v0.4s, v0.4s, v4.4s")                       // v0  = s0*(k*x[0] + y0)
            __ASM_EMIT("fmul            v1.4s, v1.4s, v5.4s")                       // v0  = s0*(k*x[0] + y0)
            __ASM_EMIT("fmul            v2.4s, v2.4s, v6.4s")                       // v0  = s0*(k*x[0] + y0)
            __ASM_EMIT("fmul            v3.4s, v3.4s, v7.4s")                       // v0  = s0*(k*x[0] + y0)
            __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("stp             q2, q3, [%[dst], #0x20]")
            __ASM_EMIT("subs            %[n], %[n], #16")
            __ASM_EMIT("add             %[dst], %[dst], #0x40")
            __ASM_EMIT("b.hs            1b")

            // 8x block
            __ASM_EMIT("2:")
            __ASM_EMIT("adds            %[n], %[n], #8")
            __ASM_EMIT("b.lt            4f")
            __ASM_EMIT("ldp             q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("fmul            v4.4s, v8.4s, v14.4s")                      // v4  = k*x[0]
            __ASM_EMIT("fmul            v5.4s, v9.4s, v14.4s")                      // v5  = k*x[1]
            __ASM_EMIT("fadd            v4.4s, v4.4s, v15.4s")                      // v0  = k*x[0] + y0
            __ASM_EMIT("fadd            v5.4s, v5.4s, v15.4s")                      // v1  = k*x[1] + y0
            __ASM_EMIT("mov             v8.16b, v10.16b")                           // v8  = x[0]'
            __ASM_EMIT("mov             v9.16b, v11.16b")                           // v9  = x[1]'
            __ASM_EMIT("fmul            v0.4s, v0.4s, v4.4s")                       // v0  = s0*(k*x[0] + y0)
            __ASM_EMIT("fmul            v1.4s, v1.4s, v5.4s")                       // v0  = s0*(k*x[0] + y0)
            __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("sub             %[n], %[n], #8")
            __ASM_EMIT("add             %[dst], %[dst], #0x20")

            // 4x block
            __ASM_EMIT("4:")
            __ASM_EMIT("adds            %[n], %[n], #4")
            __ASM_EMIT("b.lt            6f")
            __ASM_EMIT("ldr             q0, [%[dst], #0x00]")
            __ASM_EMIT("fmul            v4.4s, v8.4s, v14.4s")                      // v4  = k*x[0]
            __ASM_EMIT("fadd            v4.4s, v4.4s, v15.4s")                      // v0  = k*x[0] + y0
            __ASM_EMIT("mov             v8.16b, v9.16b")                            // v8  = x[0]'
            __ASM_EMIT("fmul            v0.4s, v0.4s, v4.4s")                       // v0  = s0*(k*x[0] + y0)
            __ASM_EMIT("str             q0, [%[dst], #0x00]")
            __ASM_EMIT("sub             %[n], %[n], #4")
            __ASM_EMIT("add             %[dst], %[dst], #0x10")

            // 1x blocks
            __ASM_EMIT("6:")
            __ASM_EMIT("adds            %[n], %[n], #3")
            __ASM_EMIT("b.lt            8f")
            __ASM_EMIT("7:")
            __ASM_EMIT("ld1r            {v0.4s}, [%[dst]]")
            __ASM_EMIT("fmul            v4.4s, v8.4s, v14.4s")                      // v4  = k*x[0]
            __ASM_EMIT("fadd            v4.4s, v4.4s, v15.4s")                      // v0  = k*x[0] + y0
            __ASM_EMIT("ext             v8.16b, v8.16b, v8.16b, #4")                // v8  = x[0]'
            __ASM_EMIT("fmul            v0.4s, v0.4s, v4.4s")                       // v0  = s0*(k*x[0] + y0)
            __ASM_EMIT("st1             {v0.s}[0], [%[dst]]")
            __ASM_EMIT("subs            %[n], %[n], #1")
            __ASM_EMIT("add             %[dst], %[dst], #0x04")
            __ASM_EMIT("b.ge            7b")

            __ASM_EMIT("8:")
            : [dst] "+r" (dst),
              [n] "+r" (n)
            : [v] "r" (&v[0]),
              [CC] "r" (&lin_inter_const[0])
            : "cc", "memory",
              "v0", "v1", "v2", "v3",
              "v4", "v5", "v6", "v7",
              "v8", "v9", "v10", "v11",
              "v12", "v13", "v14", "v15"
        );
    }

    void lin_inter_mul3(float *dst, const float *src, int32_t x0, float y0, int32_t x1, float y1, int32_t x, uint32_t n)
    {
        IF_ARCH_AARCH64(
            float v[4];
            v[0]            = x1 - x0;
            v[1]            = x - x0;
            v[2]            = y1 - y0;
            v[3]            = y0;
        )

        ARCH_AARCH64_ASM(
            // Prepare parameters
            __ASM_EMIT("ld4r            {v12.4s, v13.4s, v14.4s, v15.4s}, [%[v]]")  // v12 = (x1-x0), v13 = (x-x0), v14 = (y1-y0), v15 = y0
            __ASM_EMIT("ldp             q8, q9, [%[CC], #0x00]")                    // v8  = 0..3, v9=4..7
            __ASM_EMIT("ldp             q10, q11, [%[CC], #0x20]")                  // v10 = 8..11, v11=12..15
            __ASM_EMIT("frecpe          v0.4s, v12.4s")                             // v0  = s2
            __ASM_EMIT("fadd            v8.4s, v8.4s, v13.4s")                      // v8  = x[0]
            __ASM_EMIT("frecps          v1.4s, v0.4s, v12.4s")                      // v1  = (2 - R*s2)
            __ASM_EMIT("fadd            v9.4s, v9.4s, v13.4s")                      // v9  = x[1]
            __ASM_EMIT("fmul            v0.4s, v1.4s, v0.4s")                       // v0  = s2' = s2 * (2 - R*s2)
            __ASM_EMIT("fadd            v10.4s, v10.4s, v13.4s")                    // v10 = x[2]
            __ASM_EMIT("frecps          v1.4s, v0.4s, v12.4s")                      // v1  = (2 - R*s2')
            __ASM_EMIT("fadd            v11.4s, v11.4s, v13.4s")                    // v11 = x[3]
            __ASM_EMIT("fmul            v12.4s, v1.4s, v0.4s")                      // v14 = s2" = s2' * (2 - R*s2) = 1/s2
            __ASM_EMIT("fmul            v14.4s, v14.4s, v12.4s")                    // v14 = k = (y1-y0)/(x1-x0)
            __ASM_EMIT("ldp             q12, q13, [%[CC], #0x40]")                  // v12 = d[0], v13=d[1]

            // Register allocation:
            // v8  = x[0]
            // v9  = x[1]
            // v10 = x[2]
            // v11 = x[3]
            // v12 = d[0]
            // v13 = d[1]
            // v14 = k
            // v15 = y0
            __ASM_EMIT("subs            %[n], %[n], #16")
            __ASM_EMIT("b.lo            2f")

            // 16x blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("ldp             q0, q1, [%[src], #0x00]")
            __ASM_EMIT("ldp             q2, q3, [%[src], #0x20]")
            __ASM_EMIT("fmul            v4.4s, v8.4s, v14.4s")                      // v4  = k*x[0]
            __ASM_EMIT("fmul            v5.4s, v9.4s, v14.4s")                      // v5  = k*x[1]
            __ASM_EMIT("fmul            v6.4s, v10.4s, v14.4s")                     // v6  = k*x[2]
            __ASM_EMIT("fmul            v7.4s, v11.4s, v14.4s")                     // v7  = k*x[3]
            __ASM_EMIT("fadd            v4.4s, v4.4s, v15.4s")                      // v0  = k*x[0] + y0
            __ASM_EMIT("fadd            v5.4s, v5.4s, v15.4s")                      // v1  = k*x[1] + y0
            __ASM_EMIT("fadd            v6.4s, v6.4s, v15.4s")                      // v2  = k*x[2] + y0
            __ASM_EMIT("fadd            v7.4s, v7.4s, v15.4s")                      // v3  = k*x[3] + y0
            __ASM_EMIT("fadd            v8.4s, v8.4s, v12.4s")                      // v8  = x[0]' = x[0]+d[0]
            __ASM_EMIT("fadd            v9.4s, v9.4s, v13.4s")                      // v9  = x[1]' = x[1]+d[1]
            __ASM_EMIT("fadd            v10.4s, v10.4s, v12.4s")                    // v10 = x[2]' = x[2]+d[0]
            __ASM_EMIT("fadd            v11.4s, v11.4s, v13.4s")                    // v11 = x[3]' = x[3]+d[1]
            __ASM_EMIT("fmul            v0.4s, v0.4s, v4.4s")                       // v0  = s0*(k*x[0] + y0)
            __ASM_EMIT("fmul            v1.4s, v1.4s, v5.4s")                       // v0  = s0*(k*x[0] + y0)
            __ASM_EMIT("fmul            v2.4s, v2.4s, v6.4s")                       // v0  = s0*(k*x[0] + y0)
            __ASM_EMIT("fmul            v3.4s, v3.4s, v7.4s")                       // v0  = s0*(k*x[0] + y0)
            __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("stp             q2, q3, [%[dst], #0x20]")
            __ASM_EMIT("subs            %[n], %[n], #16")
            __ASM_EMIT("add             %[src], %[src], #0x40")
            __ASM_EMIT("add             %[dst], %[dst], #0x40")
            __ASM_EMIT("b.hs            1b")

            // 8x block
            __ASM_EMIT("2:")
            __ASM_EMIT("adds            %[n], %[n], #8")
            __ASM_EMIT("b.lt            4f")
            __ASM_EMIT("ldp             q0, q1, [%[src], #0x00]")
            __ASM_EMIT("fmul            v4.4s, v8.4s, v14.4s")                      // v4  = k*x[0]
            __ASM_EMIT("fmul            v5.4s, v9.4s, v14.4s")                      // v5  = k*x[1]
            __ASM_EMIT("fadd            v4.4s, v4.4s, v15.4s")                      // v0  = k*x[0] + y0
            __ASM_EMIT("fadd            v5.4s, v5.4s, v15.4s")                      // v1  = k*x[1] + y0
            __ASM_EMIT("mov             v8.16b, v10.16b")                           // v8  = x[0]'
            __ASM_EMIT("mov             v9.16b, v11.16b")                           // v9  = x[1]'
            __ASM_EMIT("fmul            v0.4s, v0.4s, v4.4s")                       // v0  = s0*(k*x[0] + y0)
            __ASM_EMIT("fmul            v1.4s, v1.4s, v5.4s")                       // v0  = s0*(k*x[0] + y0)
            __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("sub             %[n], %[n], #8")
            __ASM_EMIT("add             %[src], %[src], #0x20")
            __ASM_EMIT("add             %[dst], %[dst], #0x20")

            // 4x block
            __ASM_EMIT("4:")
            __ASM_EMIT("adds            %[n], %[n], #4")
            __ASM_EMIT("b.lt            6f")
            __ASM_EMIT("ldr             q0, [%[src], #0x00]")
            __ASM_EMIT("fmul            v4.4s, v8.4s, v14.4s")                      // v4  = k*x[0]
            __ASM_EMIT("fadd            v4.4s, v4.4s, v15.4s")                      // v0  = k*x[0] + y0
            __ASM_EMIT("mov             v8.16b, v9.16b")                            // v8  = x[0]'
            __ASM_EMIT("fmul            v0.4s, v0.4s, v4.4s")                       // v0  = s0*(k*x[0] + y0)
            __ASM_EMIT("str             q0, [%[dst], #0x00]")
            __ASM_EMIT("sub             %[n], %[n], #4")
            __ASM_EMIT("add             %[src], %[src], #0x10")
            __ASM_EMIT("add             %[dst], %[dst], #0x10")

            // 1x blocks
            __ASM_EMIT("6:")
            __ASM_EMIT("adds            %[n], %[n], #3")
            __ASM_EMIT("b.lt            8f")
            __ASM_EMIT("7:")
            __ASM_EMIT("ld1r            {v0.4s}, [%[src]]")
            __ASM_EMIT("fmul            v4.4s, v8.4s, v14.4s")                      // v4  = k*x[0]
            __ASM_EMIT("fadd            v4.4s, v4.4s, v15.4s")                      // v0  = k*x[0] + y0
            __ASM_EMIT("ext             v8.16b, v8.16b, v8.16b, #4")                // v8  = x[0]'
            __ASM_EMIT("fmul            v0.4s, v0.4s, v4.4s")                       // v0  = s0*(k*x[0] + y0)
            __ASM_EMIT("st1             {v0.s}[0], [%[dst]]")
            __ASM_EMIT("subs            %[n], %[n], #1")
            __ASM_EMIT("add             %[src], %[src], #0x04")
            __ASM_EMIT("add             %[dst], %[dst], #0x04")
            __ASM_EMIT("b.ge            7b")

            __ASM_EMIT("8:")
            : [dst] "+r" (dst), [src] "+r" (src),
              [n] "+r" (n)
            : [v] "r" (&v[0]),
              [CC] "r" (&lin_inter_const[0])
            : "cc", "memory",
              "v0", "v1", "v2", "v3",
              "v4", "v5", "v6", "v7",
              "v8", "v9", "v10", "v11",
              "v12", "v13", "v14", "v15"
        );
    }

    void lin_inter_fmadd2(float *dst, const float *src, int32_t x0, float y0, int32_t x1, float y1, int32_t x, uint32_t n)
    {
        IF_ARCH_AARCH64(
            float v[4];
            v[0]            = x1 - x0;
            v[1]            = x - x0;
            v[2]            = y1 - y0;
            v[3]            = y0;
        )

        ARCH_AARCH64_ASM(
            // Prepare parameters
            __ASM_EMIT("ld4r            {v12.4s, v13.4s, v14.4s, v15.4s}, [%[v]]")  // v12 = (x1-x0), v13 = (x-x0), v14 = (y1-y0), v15 = y0
            __ASM_EMIT("ldp             q8, q9, [%[CC], #0x00]")                    // v8  = 0..3, v9=4..7
            __ASM_EMIT("ldp             q10, q11, [%[CC], #0x20]")                  // v10 = 8..11, v11=12..15
            __ASM_EMIT("frecpe          v0.4s, v12.4s")                             // v0  = s2
            __ASM_EMIT("fadd            v8.4s, v8.4s, v13.4s")                      // v8  = x[0]
            __ASM_EMIT("frecps          v1.4s, v0.4s, v12.4s")                      // v1  = (2 - R*s2)
            __ASM_EMIT("fadd            v9.4s, v9.4s, v13.4s")                      // v9  = x[1]
            __ASM_EMIT("fmul            v0.4s, v1.4s, v0.4s")                       // v0  = s2' = s2 * (2 - R*s2)
            __ASM_EMIT("fadd            v10.4s, v10.4s, v13.4s")                    // v10 = x[2]
            __ASM_EMIT("frecps          v1.4s, v0.4s, v12.4s")                      // v1  = (2 - R*s2')
            __ASM_EMIT("fadd            v11.4s, v11.4s, v13.4s")                    // v11 = x[3]
            __ASM_EMIT("fmul            v12.4s, v1.4s, v0.4s")                      // v14 = s2" = s2' * (2 - R*s2) = 1/s2
            __ASM_EMIT("fmul            v14.4s, v14.4s, v12.4s")                    // v14 = k = (y1-y0)/(x1-x0)
            __ASM_EMIT("ldp             q12, q13, [%[CC], #0x40]")                  // v12 = d[0], v13=d[1]

            // Register allocation:
            // v8  = x[0]
            // v9  = x[1]
            // v10 = x[2]
            // v11 = x[3]
            // v12 = d[0]
            // v13 = d[1]
            // v14 = k
            // v15 = y0
            __ASM_EMIT("subs            %[n], %[n], #16")
            __ASM_EMIT("b.lo            2f")

            // 16x blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("ldp             q0, q1, [%[src], #0x00]")
            __ASM_EMIT("ldp             q2, q3, [%[src], #0x20]")
            __ASM_EMIT("fmul            v4.4s, v8.4s, v14.4s")                      // v4  = k*x[0]
            __ASM_EMIT("fmul            v5.4s, v9.4s, v14.4s")                      // v5  = k*x[1]
            __ASM_EMIT("fmul            v6.4s, v10.4s, v14.4s")                     // v6  = k*x[2]
            __ASM_EMIT("fmul            v7.4s, v11.4s, v14.4s")                     // v7  = k*x[3]
            __ASM_EMIT("fadd            v4.4s, v4.4s, v15.4s")                      // v0  = k*x[0] + y0
            __ASM_EMIT("fadd            v5.4s, v5.4s, v15.4s")                      // v1  = k*x[1] + y0
            __ASM_EMIT("fadd            v6.4s, v6.4s, v15.4s")                      // v2  = k*x[2] + y0
            __ASM_EMIT("fadd            v7.4s, v7.4s, v15.4s")                      // v3  = k*x[3] + y0
            __ASM_EMIT("fadd            v8.4s, v8.4s, v12.4s")                      // v8  = x[0]' = x[0]+d[0]
            __ASM_EMIT("fadd            v9.4s, v9.4s, v13.4s")                      // v9  = x[1]' = x[1]+d[1]
            __ASM_EMIT("fadd            v10.4s, v10.4s, v12.4s")                    // v10 = x[2]' = x[2]+d[0]
            __ASM_EMIT("fadd            v11.4s, v11.4s, v13.4s")                    // v11 = x[3]' = x[3]+d[1]
            __ASM_EMIT("fmul            v0.4s, v0.4s, v4.4s")                       // v0  = s0*(k*x[0] + y0)
            __ASM_EMIT("fmul            v1.4s, v1.4s, v5.4s")                       // v0  = s0*(k*x[0] + y0)
            __ASM_EMIT("fmul            v2.4s, v2.4s, v6.4s")                       // v0  = s0*(k*x[0] + y0)
            __ASM_EMIT("fmul            v3.4s, v3.4s, v7.4s")                       // v0  = s0*(k*x[0] + y0)
            __ASM_EMIT("ldp             q4, q5, [%[dst], #0x00]")
            __ASM_EMIT("ldp             q6, q7, [%[dst], #0x20]")
            __ASM_EMIT("fadd            v0.4s, v0.4s, v4.4s")                       // v0  = s0*(k*x[0] + y0) + d0[0]
            __ASM_EMIT("fadd            v1.4s, v1.4s, v5.4s")                       // v0  = s0*(k*x[0] + y0) + d0[1]
            __ASM_EMIT("fadd            v2.4s, v2.4s, v6.4s")                       // v0  = s0*(k*x[0] + y0) + d0[2]
            __ASM_EMIT("fadd            v3.4s, v3.4s, v7.4s")                       // v0  = s0*(k*x[0] + y0) + d0[3]
            __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("stp             q2, q3, [%[dst], #0x20]")
            __ASM_EMIT("subs            %[n], %[n], #16")
            __ASM_EMIT("add             %[src], %[src], #0x40")
            __ASM_EMIT("add             %[dst], %[dst], #0x40")
            __ASM_EMIT("b.hs            1b")

            // 8x block
            __ASM_EMIT("2:")
            __ASM_EMIT("adds            %[n], %[n], #8")
            __ASM_EMIT("b.lt            4f")
            __ASM_EMIT("ldp             q0, q1, [%[src], #0x00]")
            __ASM_EMIT("fmul            v4.4s, v8.4s, v14.4s")                      // v4  = k*x[0]
            __ASM_EMIT("fmul            v5.4s, v9.4s, v14.4s")                      // v5  = k*x[1]
            __ASM_EMIT("fadd            v4.4s, v4.4s, v15.4s")                      // v0  = k*x[0] + y0
            __ASM_EMIT("fadd            v5.4s, v5.4s, v15.4s")                      // v1  = k*x[1] + y0
            __ASM_EMIT("mov             v8.16b, v10.16b")                           // v8  = x[0]'
            __ASM_EMIT("mov             v9.16b, v11.16b")                           // v9  = x[1]'
            __ASM_EMIT("fmul            v0.4s, v0.4s, v4.4s")                       // v0  = s0*(k*x[0] + y0)
            __ASM_EMIT("fmul            v1.4s, v1.4s, v5.4s")                       // v0  = s0*(k*x[0] + y0)
            __ASM_EMIT("ldp             q4, q5, [%[dst], #0x00]")
            __ASM_EMIT("fadd            v0.4s, v0.4s, v4.4s")                       // v0  = s0*(k*x[0] + y0) + d0[0]
            __ASM_EMIT("fadd            v1.4s, v1.4s, v5.4s")                       // v0  = s0*(k*x[0] + y0) + d0[1]
            __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("sub             %[n], %[n], #8")
            __ASM_EMIT("add             %[src], %[src], #0x20")
            __ASM_EMIT("add             %[dst], %[dst], #0x20")

            // 4x block
            __ASM_EMIT("4:")
            __ASM_EMIT("adds            %[n], %[n], #4")
            __ASM_EMIT("b.lt            6f")
            __ASM_EMIT("ldr             q0, [%[src], #0x00]")
            __ASM_EMIT("fmul            v4.4s, v8.4s, v14.4s")                      // v4  = k*x[0]
            __ASM_EMIT("fadd            v4.4s, v4.4s, v15.4s")                      // v0  = k*x[0] + y0
            __ASM_EMIT("mov             v8.16b, v9.16b")                            // v8  = x[0]'
            __ASM_EMIT("fmul            v0.4s, v0.4s, v4.4s")                       // v0  = s0*(k*x[0] + y0)
            __ASM_EMIT("ldr             q4, [%[dst], #0x00]")
            __ASM_EMIT("fadd            v0.4s, v0.4s, v4.4s")                       // v0  = s0*(k*x[0] + y0) + d0[0]
            __ASM_EMIT("str             q0, [%[dst], #0x00]")
            __ASM_EMIT("sub             %[n], %[n], #4")
            __ASM_EMIT("add             %[src], %[src], #0x10")
            __ASM_EMIT("add             %[dst], %[dst], #0x10")

            // 1x blocks
            __ASM_EMIT("6:")
            __ASM_EMIT("adds            %[n], %[n], #3")
            __ASM_EMIT("b.lt            8f")
            __ASM_EMIT("7:")
            __ASM_EMIT("ld1r            {v0.4s}, [%[src]]")
            __ASM_EMIT("fmul            v4.4s, v8.4s, v14.4s")                      // v4  = k*x[0]
            __ASM_EMIT("fadd            v4.4s, v4.4s, v15.4s")                      // v0  = k*x[0] + y0
            __ASM_EMIT("ext             v8.16b, v8.16b, v8.16b, #4")                // v8  = x[0]'
            __ASM_EMIT("fmul            v0.4s, v0.4s, v4.4s")                       // v0  = s0*(k*x[0] + y0)
            __ASM_EMIT("ld1r            {v4.4s}, [%[dst]]")
            __ASM_EMIT("fadd            v0.4s, v0.4s, v4.4s")                       // v0  = s0*(k*x[0] + y0) + d0[0]
            __ASM_EMIT("st1             {v0.s}[0], [%[dst]]")
            __ASM_EMIT("subs            %[n], %[n], #1")
            __ASM_EMIT("add             %[src], %[src], #0x04")
            __ASM_EMIT("add             %[dst], %[dst], #0x04")
            __ASM_EMIT("b.ge            7b")

            __ASM_EMIT("8:")
            : [dst] "+r" (dst), [src] "+r" (src),
              [n] "+r" (n)
            : [v] "r" (&v[0]),
              [CC] "r" (&lin_inter_const[0])
            : "cc", "memory",
              "v0", "v1", "v2", "v3",
              "v4", "v5", "v6", "v7",
              "v8", "v9", "v10", "v11",
              "v12", "v13", "v14", "v15"
        );
    }

    void lin_inter_frmadd2(float *dst, const float *src, int32_t x0, float y0, int32_t x1, float y1, int32_t x, uint32_t n)
    {
        IF_ARCH_AARCH64(
            float v[4];
            v[0]            = x1 - x0;
            v[1]            = x - x0;
            v[2]            = y1 - y0;
            v[3]            = y0;
        )

        ARCH_AARCH64_ASM(
            // Prepare parameters
            __ASM_EMIT("ld4r            {v12.4s, v13.4s, v14.4s, v15.4s}, [%[v]]")  // v12 = (x1-x0), v13 = (x-x0), v14 = (y1-y0), v15 = y0
            __ASM_EMIT("ldp             q8, q9, [%[CC], #0x00]")                    // v8  = 0..3, v9=4..7
            __ASM_EMIT("ldp             q10, q11, [%[CC], #0x20]")                  // v10 = 8..11, v11=12..15
            __ASM_EMIT("frecpe          v0.4s, v12.4s")                             // v0  = s2
            __ASM_EMIT("fadd            v8.4s, v8.4s, v13.4s")                      // v8  = x[0]
            __ASM_EMIT("frecps          v1.4s, v0.4s, v12.4s")                      // v1  = (2 - R*s2)
            __ASM_EMIT("fadd            v9.4s, v9.4s, v13.4s")                      // v9  = x[1]
            __ASM_EMIT("fmul            v0.4s, v1.4s, v0.4s")                       // v0  = s2' = s2 * (2 - R*s2)
            __ASM_EMIT("fadd            v10.4s, v10.4s, v13.4s")                    // v10 = x[2]
            __ASM_EMIT("frecps          v1.4s, v0.4s, v12.4s")                      // v1  = (2 - R*s2')
            __ASM_EMIT("fadd            v11.4s, v11.4s, v13.4s")                    // v11 = x[3]
            __ASM_EMIT("fmul            v12.4s, v1.4s, v0.4s")                      // v14 = s2" = s2' * (2 - R*s2) = 1/s2
            __ASM_EMIT("fmul            v14.4s, v14.4s, v12.4s")                    // v14 = k = (y1-y0)/(x1-x0)
            __ASM_EMIT("ldp             q12, q13, [%[CC], #0x40]")                  // v12 = d[0], v13=d[1]

            // Register allocation:
            // v8  = x[0]
            // v9  = x[1]
            // v10 = x[2]
            // v11 = x[3]
            // v12 = d[0]
            // v13 = d[1]
            // v14 = k
            // v15 = y0
            __ASM_EMIT("subs            %[n], %[n], #16")
            __ASM_EMIT("b.lo            2f")

            // 16x blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("ldp             q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("ldp             q2, q3, [%[dst], #0x20]")
            __ASM_EMIT("fmul            v4.4s, v8.4s, v14.4s")                      // v4  = k*x[0]
            __ASM_EMIT("fmul            v5.4s, v9.4s, v14.4s")                      // v5  = k*x[1]
            __ASM_EMIT("fmul            v6.4s, v10.4s, v14.4s")                     // v6  = k*x[2]
            __ASM_EMIT("fmul            v7.4s, v11.4s, v14.4s")                     // v7  = k*x[3]
            __ASM_EMIT("fadd            v4.4s, v4.4s, v15.4s")                      // v0  = k*x[0] + y0
            __ASM_EMIT("fadd            v5.4s, v5.4s, v15.4s")                      // v1  = k*x[1] + y0
            __ASM_EMIT("fadd            v6.4s, v6.4s, v15.4s")                      // v2  = k*x[2] + y0
            __ASM_EMIT("fadd            v7.4s, v7.4s, v15.4s")                      // v3  = k*x[3] + y0
            __ASM_EMIT("fadd            v8.4s, v8.4s, v12.4s")                      // v8  = x[0]' = x[0]+d[0]
            __ASM_EMIT("fadd            v9.4s, v9.4s, v13.4s")                      // v9  = x[1]' = x[1]+d[1]
            __ASM_EMIT("fadd            v10.4s, v10.4s, v12.4s")                    // v10 = x[2]' = x[2]+d[0]
            __ASM_EMIT("fadd            v11.4s, v11.4s, v13.4s")                    // v11 = x[3]' = x[3]+d[1]
            __ASM_EMIT("fmul            v0.4s, v0.4s, v4.4s")                       // v0  = s0*(k*x[0] + y0)
            __ASM_EMIT("fmul            v1.4s, v1.4s, v5.4s")                       // v0  = s0*(k*x[0] + y0)
            __ASM_EMIT("fmul            v2.4s, v2.4s, v6.4s")                       // v0  = s0*(k*x[0] + y0)
            __ASM_EMIT("fmul            v3.4s, v3.4s, v7.4s")                       // v0  = s0*(k*x[0] + y0)
            __ASM_EMIT("ldp             q4, q5, [%[src], #0x00]")
            __ASM_EMIT("ldp             q6, q7, [%[src], #0x20]")
            __ASM_EMIT("fadd            v0.4s, v0.4s, v4.4s")                       // v0  = s0*(k*x[0] + y0) + d0[0]
            __ASM_EMIT("fadd            v1.4s, v1.4s, v5.4s")                       // v0  = s0*(k*x[0] + y0) + d0[1]
            __ASM_EMIT("fadd            v2.4s, v2.4s, v6.4s")                       // v0  = s0*(k*x[0] + y0) + d0[2]
            __ASM_EMIT("fadd            v3.4s, v3.4s, v7.4s")                       // v0  = s0*(k*x[0] + y0) + d0[3]
            __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("stp             q2, q3, [%[dst], #0x20]")
            __ASM_EMIT("subs            %[n], %[n], #16")
            __ASM_EMIT("add             %[src], %[src], #0x40")
            __ASM_EMIT("add             %[dst], %[dst], #0x40")
            __ASM_EMIT("b.hs            1b")

            // 8x block
            __ASM_EMIT("2:")
            __ASM_EMIT("adds            %[n], %[n], #8")
            __ASM_EMIT("b.lt            4f")
            __ASM_EMIT("ldp             q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("fmul            v4.4s, v8.4s, v14.4s")                      // v4  = k*x[0]
            __ASM_EMIT("fmul            v5.4s, v9.4s, v14.4s")                      // v5  = k*x[1]
            __ASM_EMIT("fadd            v4.4s, v4.4s, v15.4s")                      // v0  = k*x[0] + y0
            __ASM_EMIT("fadd            v5.4s, v5.4s, v15.4s")                      // v1  = k*x[1] + y0
            __ASM_EMIT("mov             v8.16b, v10.16b")                           // v8  = x[0]'
            __ASM_EMIT("mov             v9.16b, v11.16b")                           // v9  = x[1]'
            __ASM_EMIT("fmul            v0.4s, v0.4s, v4.4s")                       // v0  = s0*(k*x[0] + y0)
            __ASM_EMIT("fmul            v1.4s, v1.4s, v5.4s")                       // v0  = s0*(k*x[0] + y0)
            __ASM_EMIT("ldp             q4, q5, [%[src], #0x00]")
            __ASM_EMIT("fadd            v0.4s, v0.4s, v4.4s")                       // v0  = s0*(k*x[0] + y0) + d0[0]
            __ASM_EMIT("fadd            v1.4s, v1.4s, v5.4s")                       // v0  = s0*(k*x[0] + y0) + d0[1]
            __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("sub             %[n], %[n], #8")
            __ASM_EMIT("add             %[src], %[src], #0x20")
            __ASM_EMIT("add             %[dst], %[dst], #0x20")

            // 4x block
            __ASM_EMIT("4:")
            __ASM_EMIT("adds            %[n], %[n], #4")
            __ASM_EMIT("b.lt            6f")
            __ASM_EMIT("ldr             q0, [%[dst], #0x00]")
            __ASM_EMIT("fmul            v4.4s, v8.4s, v14.4s")                      // v4  = k*x[0]
            __ASM_EMIT("fadd            v4.4s, v4.4s, v15.4s")                      // v0  = k*x[0] + y0
            __ASM_EMIT("mov             v8.16b, v9.16b")                            // v8  = x[0]'
            __ASM_EMIT("fmul            v0.4s, v0.4s, v4.4s")                       // v0  = s0*(k*x[0] + y0)
            __ASM_EMIT("ldr             q4, [%[src], #0x00]")
            __ASM_EMIT("fadd            v0.4s, v0.4s, v4.4s")                       // v0  = s0*(k*x[0] + y0) + d0[0]
            __ASM_EMIT("str             q0, [%[dst], #0x00]")
            __ASM_EMIT("sub             %[n], %[n], #4")
            __ASM_EMIT("add             %[src], %[src], #0x10")
            __ASM_EMIT("add             %[dst], %[dst], #0x10")

            // 1x blocks
            __ASM_EMIT("6:")
            __ASM_EMIT("adds            %[n], %[n], #3")
            __ASM_EMIT("b.lt            8f")
            __ASM_EMIT("7:")
            __ASM_EMIT("ld1r            {v0.4s}, [%[dst]]")
            __ASM_EMIT("fmul            v4.4s, v8.4s, v14.4s")                      // v4  = k*x[0]
            __ASM_EMIT("fadd            v4.4s, v4.4s, v15.4s")                      // v0  = k*x[0] + y0
            __ASM_EMIT("ext             v8.16b, v8.16b, v8.16b, #4")                // v8  = x[0]'
            __ASM_EMIT("fmul            v0.4s, v0.4s, v4.4s")                       // v0  = s0*(k*x[0] + y0)
            __ASM_EMIT("ld1r            {v4.4s}, [%[src]]")
            __ASM_EMIT("fadd            v0.4s, v0.4s, v4.4s")                       // v0  = s0*(k*x[0] + y0) + d0[0]
            __ASM_EMIT("st1             {v0.s}[0], [%[dst]]")
            __ASM_EMIT("subs            %[n], %[n], #1")
            __ASM_EMIT("add             %[src], %[src], #0x04")
            __ASM_EMIT("add             %[dst], %[dst], #0x04")
            __ASM_EMIT("b.ge            7b")

            __ASM_EMIT("8:")
            : [dst] "+r" (dst), [src] "+r" (src),
              [n] "+r" (n)
            : [v] "r" (&v[0]),
              [CC] "r" (&lin_inter_const[0])
            : "cc", "memory",
              "v0", "v1", "v2", "v3",
              "v4", "v5", "v6", "v7",
              "v8", "v9", "v10", "v11",
              "v12", "v13", "v14", "v15"
        );
    }

    void lin_inter_fmadd3(float *dst, const float *src1, const float *src2, int32_t x0, float y0, int32_t x1, float y1, int32_t x, uint32_t n)
    {
        IF_ARCH_AARCH64(
            float v[4];
            v[0]            = x1 - x0;
            v[1]            = x - x0;
            v[2]            = y1 - y0;
            v[3]            = y0;
        )

        ARCH_AARCH64_ASM(
            // Prepare parameters
            __ASM_EMIT("ld4r            {v12.4s, v13.4s, v14.4s, v15.4s}, [%[v]]")  // v12 = (x1-x0), v13 = (x-x0), v14 = (y1-y0), v15 = y0
            __ASM_EMIT("ldp             q8, q9, [%[CC], #0x00]")                    // v8  = 0..3, v9=4..7
            __ASM_EMIT("ldp             q10, q11, [%[CC], #0x20]")                  // v10 = 8..11, v11=12..15
            __ASM_EMIT("frecpe          v0.4s, v12.4s")                             // v0  = s2
            __ASM_EMIT("fadd            v8.4s, v8.4s, v13.4s")                      // v8  = x[0]
            __ASM_EMIT("frecps          v1.4s, v0.4s, v12.4s")                      // v1  = (2 - R*s2)
            __ASM_EMIT("fadd            v9.4s, v9.4s, v13.4s")                      // v9  = x[1]
            __ASM_EMIT("fmul            v0.4s, v1.4s, v0.4s")                       // v0  = s2' = s2 * (2 - R*s2)
            __ASM_EMIT("fadd            v10.4s, v10.4s, v13.4s")                    // v10 = x[2]
            __ASM_EMIT("frecps          v1.4s, v0.4s, v12.4s")                      // v1  = (2 - R*s2')
            __ASM_EMIT("fadd            v11.4s, v11.4s, v13.4s")                    // v11 = x[3]
            __ASM_EMIT("fmul            v12.4s, v1.4s, v0.4s")                      // v14 = s2" = s2' * (2 - R*s2) = 1/s2
            __ASM_EMIT("fmul            v14.4s, v14.4s, v12.4s")                    // v14 = k = (y1-y0)/(x1-x0)
            __ASM_EMIT("ldp             q12, q13, [%[CC], #0x40]")                  // v12 = d[0], v13=d[1]

            // Register allocation:
            // v8  = x[0]
            // v9  = x[1]
            // v10 = x[2]
            // v11 = x[3]
            // v12 = d[0]
            // v13 = d[1]
            // v14 = k
            // v15 = y0
            __ASM_EMIT("subs            %[n], %[n], #16")
            __ASM_EMIT("b.lo            2f")

            // 16x blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("ldp             q0, q1, [%[src1], #0x00]")
            __ASM_EMIT("ldp             q2, q3, [%[src1], #0x20]")
            __ASM_EMIT("fmul            v4.4s, v8.4s, v14.4s")                      // v4  = k*x[0]
            __ASM_EMIT("fmul            v5.4s, v9.4s, v14.4s")                      // v5  = k*x[1]
            __ASM_EMIT("fmul            v6.4s, v10.4s, v14.4s")                     // v6  = k*x[2]
            __ASM_EMIT("fmul            v7.4s, v11.4s, v14.4s")                     // v7  = k*x[3]
            __ASM_EMIT("fadd            v4.4s, v4.4s, v15.4s")                      // v0  = k*x[0] + y0
            __ASM_EMIT("fadd            v5.4s, v5.4s, v15.4s")                      // v1  = k*x[1] + y0
            __ASM_EMIT("fadd            v6.4s, v6.4s, v15.4s")                      // v2  = k*x[2] + y0
            __ASM_EMIT("fadd            v7.4s, v7.4s, v15.4s")                      // v3  = k*x[3] + y0
            __ASM_EMIT("fadd            v8.4s, v8.4s, v12.4s")                      // v8  = x[0]' = x[0]+d[0]
            __ASM_EMIT("fadd            v9.4s, v9.4s, v13.4s")                      // v9  = x[1]' = x[1]+d[1]
            __ASM_EMIT("fadd            v10.4s, v10.4s, v12.4s")                    // v10 = x[2]' = x[2]+d[0]
            __ASM_EMIT("fadd            v11.4s, v11.4s, v13.4s")                    // v11 = x[3]' = x[3]+d[1]
            __ASM_EMIT("fmul            v0.4s, v0.4s, v4.4s")                       // v0  = s0*(k*x[0] + y0)
            __ASM_EMIT("fmul            v1.4s, v1.4s, v5.4s")                       // v0  = s0*(k*x[0] + y0)
            __ASM_EMIT("fmul            v2.4s, v2.4s, v6.4s")                       // v0  = s0*(k*x[0] + y0)
            __ASM_EMIT("fmul            v3.4s, v3.4s, v7.4s")                       // v0  = s0*(k*x[0] + y0)
            __ASM_EMIT("ldp             q4, q5, [%[src2], #0x00]")
            __ASM_EMIT("ldp             q6, q7, [%[src2], #0x20]")
            __ASM_EMIT("fadd            v0.4s, v0.4s, v4.4s")                       // v0  = s0*(k*x[0] + y0) + d0[0]
            __ASM_EMIT("fadd            v1.4s, v1.4s, v5.4s")                       // v0  = s0*(k*x[0] + y0) + d0[1]
            __ASM_EMIT("fadd            v2.4s, v2.4s, v6.4s")                       // v0  = s0*(k*x[0] + y0) + d0[2]
            __ASM_EMIT("fadd            v3.4s, v3.4s, v7.4s")                       // v0  = s0*(k*x[0] + y0) + d0[3]
            __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("stp             q2, q3, [%[dst], #0x20]")
            __ASM_EMIT("subs            %[n], %[n], #16")
            __ASM_EMIT("add             %[src1], %[src1], #0x40")
            __ASM_EMIT("add             %[src2], %[src2], #0x40")
            __ASM_EMIT("add             %[dst], %[dst], #0x40")
            __ASM_EMIT("b.hs            1b")

            // 8x block
            __ASM_EMIT("2:")
            __ASM_EMIT("adds            %[n], %[n], #8")
            __ASM_EMIT("b.lt            4f")
            __ASM_EMIT("ldp             q0, q1, [%[src1], #0x00]")
            __ASM_EMIT("fmul            v4.4s, v8.4s, v14.4s")                      // v4  = k*x[0]
            __ASM_EMIT("fmul            v5.4s, v9.4s, v14.4s")                      // v5  = k*x[1]
            __ASM_EMIT("fadd            v4.4s, v4.4s, v15.4s")                      // v0  = k*x[0] + y0
            __ASM_EMIT("fadd            v5.4s, v5.4s, v15.4s")                      // v1  = k*x[1] + y0
            __ASM_EMIT("mov             v8.16b, v10.16b")                           // v8  = x[0]'
            __ASM_EMIT("mov             v9.16b, v11.16b")                           // v9  = x[1]'
            __ASM_EMIT("fmul            v0.4s, v0.4s, v4.4s")                       // v0  = s0*(k*x[0] + y0)
            __ASM_EMIT("fmul            v1.4s, v1.4s, v5.4s")                       // v0  = s0*(k*x[0] + y0)
            __ASM_EMIT("ldp             q4, q5, [%[src2], #0x00]")
            __ASM_EMIT("fadd            v0.4s, v0.4s, v4.4s")                       // v0  = s0*(k*x[0] + y0) + d0[0]
            __ASM_EMIT("fadd            v1.4s, v1.4s, v5.4s")                       // v0  = s0*(k*x[0] + y0) + d0[1]
            __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("sub             %[n], %[n], #8")
            __ASM_EMIT("add             %[src1], %[src1], #0x20")
            __ASM_EMIT("add             %[src2], %[src2], #0x20")
            __ASM_EMIT("add             %[dst], %[dst], #0x20")

            // 4x block
            __ASM_EMIT("4:")
            __ASM_EMIT("adds            %[n], %[n], #4")
            __ASM_EMIT("b.lt            6f")
            __ASM_EMIT("ldr             q0, [%[src1], #0x00]")
            __ASM_EMIT("fmul            v4.4s, v8.4s, v14.4s")                      // v4  = k*x[0]
            __ASM_EMIT("fadd            v4.4s, v4.4s, v15.4s")                      // v0  = k*x[0] + y0
            __ASM_EMIT("mov             v8.16b, v9.16b")                            // v8  = x[0]'
            __ASM_EMIT("fmul            v0.4s, v0.4s, v4.4s")                       // v0  = s0*(k*x[0] + y0)
            __ASM_EMIT("ldr             q4, [%[src2], #0x00]")
            __ASM_EMIT("fadd            v0.4s, v0.4s, v4.4s")                       // v0  = s0*(k*x[0] + y0) + d0[0]
            __ASM_EMIT("str             q0, [%[dst], #0x00]")
            __ASM_EMIT("sub             %[n], %[n], #4")
            __ASM_EMIT("add             %[src1], %[src1], #0x10")
            __ASM_EMIT("add             %[src2], %[src2], #0x10")
            __ASM_EMIT("add             %[dst], %[dst], #0x10")

            // 1x blocks
            __ASM_EMIT("6:")
            __ASM_EMIT("adds            %[n], %[n], #3")
            __ASM_EMIT("b.lt            8f")
            __ASM_EMIT("7:")
            __ASM_EMIT("ld1r            {v0.4s}, [%[src1]]")
            __ASM_EMIT("fmul            v4.4s, v8.4s, v14.4s")                      // v4  = k*x[0]
            __ASM_EMIT("fadd            v4.4s, v4.4s, v15.4s")                      // v0  = k*x[0] + y0
            __ASM_EMIT("ext             v8.16b, v8.16b, v8.16b, #4")                // v8  = x[0]'
            __ASM_EMIT("fmul            v0.4s, v0.4s, v4.4s")                       // v0  = s0*(k*x[0] + y0)
            __ASM_EMIT("ld1r            {v4.4s}, [%[src2]]")
            __ASM_EMIT("fadd            v0.4s, v0.4s, v4.4s")                       // v0  = s0*(k*x[0] + y0) + d0[0]
            __ASM_EMIT("st1             {v0.s}[0], [%[dst]]")
            __ASM_EMIT("subs            %[n], %[n], #1")
            __ASM_EMIT("add             %[src1], %[src1], #0x04")
            __ASM_EMIT("add             %[src2], %[src2], #0x04")
            __ASM_EMIT("add             %[dst], %[dst], #0x04")
            __ASM_EMIT("b.ge            7b")

            __ASM_EMIT("8:")
            : [dst] "+r" (dst), [src1] "+r" (src1), [src2] "+r" (src2),
              [n] "+r" (n)
            : [v] "r" (&v[0]),
              [CC] "r" (&lin_inter_const[0])
            : "cc", "memory",
              "v0", "v1", "v2", "v3",
              "v4", "v5", "v6", "v7",
              "v8", "v9", "v10", "v11",
              "v12", "v13", "v14", "v15"
        );
    }
}

#endif /* DSP_ARCH_AARCH64_ASIMD_INTERPOLATE_H_ */
