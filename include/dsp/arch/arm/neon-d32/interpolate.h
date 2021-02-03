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

#ifndef DSP_ARCH_ARM_NEON_D32_INTERPOLATE_H_
#define DSP_ARCH_ARM_NEON_D32_INTERPOLATE_H_

namespace neon_d32
{
    static const float lin_inter_const[] __lsp_aligned16 =
    {
        0.0f, 1.0f,  2.0f, 3.0f,  4.0f,  5.0f,  6.0f,  7.0f,    // Initial values 0..7
        8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f,   // Initial values 8..15
        16.0f, 16.0f, 16.0f, 16.0f, 16.0f, 16.0f, 16.0f, 16.0f  // Step
    };

    void lin_inter_set(float *dst, int32_t x0, float y0, int32_t x1, float y1, int32_t x, uint32_t n)
    {
        IF_ARCH_ARM(
            float v[4];
            v[0]            = x1 - x0;
            v[1]            = x - x0;
            v[2]            = y1 - y0;
            v[3]            = y0;
            const float *CC = lin_inter_const;
        )

        ARCH_ARM_ASM(
            // Prepare parameters
            __ASM_EMIT("vld4.32         {d24[], d26[], d28[], d30[]}, [%[v]]")
            __ASM_EMIT("vldm            %[CC]!, {q8-q11}")                  // q8  = 0..3, q9=4..7, q10=8..11, q11=12..15
            __ASM_EMIT("vld4.32         {d25[], d27[], d29[], d31[]}, [%[v]]")  // q12 = (x1-x0), q13 = (x-x0), q14 = (y1-y0), q15 = y0
            __ASM_EMIT("vrecpe.f32      q0, q12")                           // q0  = s2
            __ASM_EMIT("vadd.f32        q8, q8, q13")                       // q8  = x[0]
            __ASM_EMIT("vrecps.f32      q1, q0, q12")                       // q1  = (2 - R*s2)
            __ASM_EMIT("vadd.f32        q9, q9, q13")                       // q9  = x[1]
            __ASM_EMIT("vmul.f32        q0, q1, q0")                        // q0  = s2' = s2 * (2 - R*s2)
            __ASM_EMIT("vadd.f32        q10, q10, q13")                     // q10 = x[2]
            __ASM_EMIT("vrecps.f32      q1, q0, q12")                       // q1  = (2 - R*s2')
            __ASM_EMIT("vadd.f32        q11, q11, q13")                     // q11 = x[3]
            __ASM_EMIT("vmul.f32        q12, q1, q0")                       // q14 = s2" = s2' * (2 - R*s2) = 1/s2
            __ASM_EMIT("vmul.f32        q14, q14, q12")                     // q14 = k = (y1-y0)/(x1-x0)
            __ASM_EMIT("vldm            %[CC], {q12-q13}")                  // q12 = d[0], q13=d[1]

            // Register allocation:
            // q8  = x[0]
            // q9  = x[1]
            // q10 = x[2]
            // q11 = x[3]
            // q12 = d[0]
            // q13 = d[1]
            // q14 = k
            // q15 = y0
            __ASM_EMIT("subs            %[n], %[n], $16")
            __ASM_EMIT("blo             2f")

            // 16x blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("vmul.f32        q4, q8, q14")                       // q4  = k*x[0]
            __ASM_EMIT("vmul.f32        q5, q9, q14")                       // q5  = k*x[1]
            __ASM_EMIT("vmul.f32        q6, q10, q14")                      // q6  = k*x[2]
            __ASM_EMIT("vmul.f32        q7, q11, q14")                      // q7  = k*x[3]
            __ASM_EMIT("vadd.f32        q0, q4, q15")                       // q0  = k*x[0] + y0
            __ASM_EMIT("vadd.f32        q1, q5, q15")                       // q1  = k*x[1] + y0
            __ASM_EMIT("vadd.f32        q2, q6, q15")                       // q2  = k*x[2] + y0
            __ASM_EMIT("vadd.f32        q3, q7, q15")                       // q3  = k*x[3] + y0
            __ASM_EMIT("vadd.f32        q8, q8, q12")                       // q8  = x[0]' = x[0]+d[0]
            __ASM_EMIT("vadd.f32        q9, q9, q13")                       // q9  = x[1]' = x[1]+d[1]
            __ASM_EMIT("vadd.f32        q10, q10, q12")                     // q10 = x[2]' = x[2]+d[0]
            __ASM_EMIT("vadd.f32        q11, q11, q13")                     // q11 = x[3]' = x[3]+d[1]
            __ASM_EMIT("vstm            %[dst]!, {q0-q3}")
            __ASM_EMIT("subs            %[n], %[n], $16")
            __ASM_EMIT("bhs             1b")

            // 8x block
            __ASM_EMIT("2:")
            __ASM_EMIT("adds            %[n], %[n], $8")
            __ASM_EMIT("blt             4f")
            __ASM_EMIT("vmul.f32        q4, q8, q14")                       // q4  = k*x[0]
            __ASM_EMIT("vmul.f32        q5, q9, q14")                       // q5  = k*x[1]
            __ASM_EMIT("vadd.f32        q0, q4, q15")                       // q0  = k*x[0] + y0
            __ASM_EMIT("vadd.f32        q1, q5, q15")                       // q1  = k*x[1] + y0
            __ASM_EMIT("vmov            q8, q10")                           // q8  = x[0]'
            __ASM_EMIT("vmov            q9, q11")                           // q9  = x[1]'
            __ASM_EMIT("vstm            %[dst]!, {q0-q1}")
            __ASM_EMIT("sub             %[n], %[n], $8")

            // 4x block
            __ASM_EMIT("4:")
            __ASM_EMIT("adds            %[n], %[n], $4")
            __ASM_EMIT("blt             6f")
            __ASM_EMIT("vmul.f32        q4, q8, q14")                       // q4  = k*x[0]
            __ASM_EMIT("vadd.f32        q0, q4, q15")                       // q0  = k*x[0] + y0
            __ASM_EMIT("vmov            q8, q9")                            // q8  = x[0]'
            __ASM_EMIT("vstm            %[dst]!, {q0}")
            __ASM_EMIT("sub             %[n], %[n], $4")

            // 1x blocks
            __ASM_EMIT("6:")
            __ASM_EMIT("adds            %[n], %[n], $3")
            __ASM_EMIT("blt             8f")
            __ASM_EMIT("7:")
            __ASM_EMIT("vmul.f32        q4, q8, q14")                       // q4  = k*x[0]
            __ASM_EMIT("vadd.f32        q0, q4, q15")                       // q0  = k*x[0] + y0
            __ASM_EMIT("vext.32         q8, q8, $1")                        // q8  = x[0]'
            __ASM_EMIT("vst1.32         {d0[0]}, [%[dst]]!")
            __ASM_EMIT("subs            %[n], %[n], $1")
            __ASM_EMIT("bge             7b")

            __ASM_EMIT("8:")
            : [dst] "+r" (dst),
              [n] "+r" (n),
              [CC] "+r" (CC)
            : [v] "r" (&v[0])
            : "cc", "memory",
              "q0", "q1", "q2", "q3",
              "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11",
              "q12", "q13", "q14", "q15"
        );
    }

    void lin_inter_mul2(float *dst, int32_t x0, float y0, int32_t x1, float y1, int32_t x, uint32_t n)
    {
        IF_ARCH_ARM(
            float v[4];
            v[0]            = x1 - x0;
            v[1]            = x - x0;
            v[2]            = y1 - y0;
            v[3]            = y0;
            const float *CC = lin_inter_const;
        )

        ARCH_ARM_ASM(
            // Prepare parameters
            __ASM_EMIT("vld4.32         {d24[], d26[], d28[], d30[]}, [%[v]]")
            __ASM_EMIT("vldm            %[CC]!, {q8-q11}")                  // q8  = 0..3, q9=4..7, q10=8..11, q11=12..15
            __ASM_EMIT("vld4.32         {d25[], d27[], d29[], d31[]}, [%[v]]")  // q12 = (x1-x0), q13 = (x-x0), q14 = (y1-y0), q15 = y0
            __ASM_EMIT("vrecpe.f32      q0, q12")                           // q0  = s2
            __ASM_EMIT("vadd.f32        q8, q8, q13")                       // q8  = x[0]
            __ASM_EMIT("vrecps.f32      q1, q0, q12")                       // q1  = (2 - R*s2)
            __ASM_EMIT("vadd.f32        q9, q9, q13")                       // q9  = x[1]
            __ASM_EMIT("vmul.f32        q0, q1, q0")                        // q0  = s2' = s2 * (2 - R*s2)
            __ASM_EMIT("vadd.f32        q10, q10, q13")                     // q10 = x[2]
            __ASM_EMIT("vrecps.f32      q1, q0, q12")                       // q1  = (2 - R*s2')
            __ASM_EMIT("vadd.f32        q11, q11, q13")                     // q11 = x[3]
            __ASM_EMIT("vmul.f32        q12, q1, q0")                       // q14 = s2" = s2' * (2 - R*s2) = 1/s2
            __ASM_EMIT("vmul.f32        q14, q14, q12")                     // q14 = k = (y1-y0)/(x1-x0)
            __ASM_EMIT("vldm            %[CC], {q12-q13}")                  // q12 = d[0], q13=d[1]

            // Register allocation:
            // q8  = x[0]
            // q9  = x[1]
            // q10 = x[2]
            // q11 = x[3]
            // q12 = d[0]
            // q13 = d[1]
            // q14 = k
            // q15 = y0
            __ASM_EMIT("subs            %[n], %[n], $16")
            __ASM_EMIT("blo             2f")

            // 16x blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("vldm            %[dst], {q0-q3}")
            __ASM_EMIT("vmul.f32        q4, q8, q14")                       // q4  = k*x[0]
            __ASM_EMIT("vmul.f32        q5, q9, q14")                       // q5  = k*x[1]
            __ASM_EMIT("vmul.f32        q6, q10, q14")                      // q6  = k*x[2]
            __ASM_EMIT("vmul.f32        q7, q11, q14")                      // q7  = k*x[3]
            __ASM_EMIT("vadd.f32        q4, q4, q15")                       // q0  = k*x[0] + y0
            __ASM_EMIT("vadd.f32        q5, q5, q15")                       // q1  = k*x[1] + y0
            __ASM_EMIT("vadd.f32        q6, q6, q15")                       // q2  = k*x[2] + y0
            __ASM_EMIT("vadd.f32        q7, q7, q15")                       // q3  = k*x[3] + y0
            __ASM_EMIT("vmul.f32        q0, q0, q4")                        // q0  = s0[0]*(k*x[0] + y0)
            __ASM_EMIT("vmul.f32        q1, q1, q5")                        // q1  = s0[1]*(k*x[1] + y0)
            __ASM_EMIT("vmul.f32        q2, q2, q6")                        // q2  = s0[2]*(k*x[2] + y0)
            __ASM_EMIT("vmul.f32        q3, q3, q7")                        // q3  = s0[3]*(k*x[3] + y0)
            __ASM_EMIT("vadd.f32        q8, q8, q12")                       // q8  = x[0]' = x[0]+d[0]
            __ASM_EMIT("vadd.f32        q9, q9, q13")                       // q9  = x[1]' = x[1]+d[1]
            __ASM_EMIT("vadd.f32        q10, q10, q12")                     // q10 = x[2]' = x[2]+d[0]
            __ASM_EMIT("vadd.f32        q11, q11, q13")                     // q11 = x[3]' = x[3]+d[1]
            __ASM_EMIT("vstm            %[dst]!, {q0-q3}")
            __ASM_EMIT("subs            %[n], %[n], $16")
            __ASM_EMIT("bhs             1b")

            // 8x block
            __ASM_EMIT("2:")
            __ASM_EMIT("adds            %[n], %[n], $8")
            __ASM_EMIT("blt             4f")
            __ASM_EMIT("vldm            %[dst], {q0-q1}")
            __ASM_EMIT("vmul.f32        q4, q8, q14")                       // q4  = k*x[0]
            __ASM_EMIT("vmul.f32        q5, q9, q14")                       // q5  = k*x[1]
            __ASM_EMIT("vadd.f32        q4, q4, q15")                       // q0  = k*x[0] + y0
            __ASM_EMIT("vadd.f32        q5, q5, q15")                       // q1  = k*x[1] + y0
            __ASM_EMIT("vmul.f32        q0, q0, q4")                        // q0  = s0[0]*(k*x[0] + y0)
            __ASM_EMIT("vmul.f32        q1, q1, q5")                        // q1  = s0[1]*(k*x[1] + y0)
            __ASM_EMIT("vmov            q8, q10")                           // q8  = x[0]'
            __ASM_EMIT("vmov            q9, q11")                           // q9  = x[1]'
            __ASM_EMIT("vstm            %[dst]!, {q0-q1}")
            __ASM_EMIT("sub             %[n], %[n], $8")

            // 4x block
            __ASM_EMIT("4:")
            __ASM_EMIT("adds            %[n], %[n], $4")
            __ASM_EMIT("blt             6f")
            __ASM_EMIT("vldm            %[dst], {q0}")
            __ASM_EMIT("vmul.f32        q4, q8, q14")                       // q4  = k*x[0]
            __ASM_EMIT("vadd.f32        q4, q4, q15")                       // q0  = k*x[0] + y0
            __ASM_EMIT("vmul.f32        q0, q0, q4")                        // q0  = s0[0]*(k*x[0] + y0)
            __ASM_EMIT("vmov            q8, q9")                            // q8  = x[0]'
            __ASM_EMIT("vstm            %[dst]!, {q0}")
            __ASM_EMIT("sub             %[n], %[n], $4")

            // 1x blocks
            __ASM_EMIT("6:")
            __ASM_EMIT("adds            %[n], %[n], $3")
            __ASM_EMIT("blt             8f")
            __ASM_EMIT("7:")
            __ASM_EMIT("vld1.32         {d0[], d1[]}, [%[dst]]")
            __ASM_EMIT("vmul.f32        q4, q8, q14")                       // q4  = k*x[0]
            __ASM_EMIT("vadd.f32        q4, q4, q15")                       // q0  = k*x[0] + y0
            __ASM_EMIT("vext.32         q8, q8, $1")                        // q8  = x[0]'
            __ASM_EMIT("vmul.f32        q0, q0, q4")                        // q0  = s0[0]*(k*x[0] + y0)
            __ASM_EMIT("vst1.32         {d0[0]}, [%[dst]]!")
            __ASM_EMIT("subs            %[n], %[n], $1")
            __ASM_EMIT("bge             7b")

            __ASM_EMIT("8:")
            : [dst] "+r" (dst),
              [n] "+r" (n),
              [CC] "+r" (CC)
            : [v] "r" (&v[0])
            : "cc", "memory",
              "q0", "q1", "q2", "q3",
              "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11",
              "q12", "q13", "q14", "q15"
        );
    }

    void lin_inter_mul3(float *dst, const float *src, int32_t x0, float y0, int32_t x1, float y1, int32_t x, uint32_t n)
    {
        IF_ARCH_ARM(
            float v[4];
            v[0]            = x1 - x0;
            v[1]            = x - x0;
            v[2]            = y1 - y0;
            v[3]            = y0;
            const float *CC = lin_inter_const;
        )

        ARCH_ARM_ASM(
            // Prepare parameters
            __ASM_EMIT("vld4.32         {d24[], d26[], d28[], d30[]}, [%[v]]")
            __ASM_EMIT("vldm            %[CC]!, {q8-q11}")                  // q8  = 0..3, q9=4..7, q10=8..11, q11=12..15
            __ASM_EMIT("vld4.32         {d25[], d27[], d29[], d31[]}, [%[v]]")  // q12 = (x1-x0), q13 = (x-x0), q14 = (y1-y0), q15 = y0
            __ASM_EMIT("vrecpe.f32      q0, q12")                           // q0  = s2
            __ASM_EMIT("vadd.f32        q8, q8, q13")                       // q8  = x[0]
            __ASM_EMIT("vrecps.f32      q1, q0, q12")                       // q1  = (2 - R*s2)
            __ASM_EMIT("vadd.f32        q9, q9, q13")                       // q9  = x[1]
            __ASM_EMIT("vmul.f32        q0, q1, q0")                        // q0  = s2' = s2 * (2 - R*s2)
            __ASM_EMIT("vadd.f32        q10, q10, q13")                     // q10 = x[2]
            __ASM_EMIT("vrecps.f32      q1, q0, q12")                       // q1  = (2 - R*s2')
            __ASM_EMIT("vadd.f32        q11, q11, q13")                     // q11 = x[3]
            __ASM_EMIT("vmul.f32        q12, q1, q0")                       // q14 = s2" = s2' * (2 - R*s2) = 1/s2
            __ASM_EMIT("vmul.f32        q14, q14, q12")                     // q14 = k = (y1-y0)/(x1-x0)
            __ASM_EMIT("vldm            %[CC], {q12-q13}")                  // q12 = d[0], q13=d[1]

            // Register allocation:
            // q8  = x[0]
            // q9  = x[1]
            // q10 = x[2]
            // q11 = x[3]
            // q12 = d[0]
            // q13 = d[1]
            // q14 = k
            // q15 = y0
            __ASM_EMIT("subs            %[n], %[n], $16")
            __ASM_EMIT("blo             2f")

            // 16x blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("vldm            %[src]!, {q0-q3}")
            __ASM_EMIT("vmul.f32        q4, q8, q14")                       // q4  = k*x[0]
            __ASM_EMIT("vmul.f32        q5, q9, q14")                       // q5  = k*x[1]
            __ASM_EMIT("vmul.f32        q6, q10, q14")                      // q6  = k*x[2]
            __ASM_EMIT("vmul.f32        q7, q11, q14")                      // q7  = k*x[3]
            __ASM_EMIT("vadd.f32        q4, q4, q15")                       // q0  = k*x[0] + y0
            __ASM_EMIT("vadd.f32        q5, q5, q15")                       // q1  = k*x[1] + y0
            __ASM_EMIT("vadd.f32        q6, q6, q15")                       // q2  = k*x[2] + y0
            __ASM_EMIT("vadd.f32        q7, q7, q15")                       // q3  = k*x[3] + y0
            __ASM_EMIT("vmul.f32        q0, q0, q4")                        // q0  = s0[0]*(k*x[0] + y0)
            __ASM_EMIT("vmul.f32        q1, q1, q5")                        // q1  = s0[1]*(k*x[1] + y0)
            __ASM_EMIT("vmul.f32        q2, q2, q6")                        // q2  = s0[2]*(k*x[2] + y0)
            __ASM_EMIT("vmul.f32        q3, q3, q7")                        // q3  = s0[3]*(k*x[3] + y0)
            __ASM_EMIT("vadd.f32        q8, q8, q12")                       // q8  = x[0]' = x[0]+d[0]
            __ASM_EMIT("vadd.f32        q9, q9, q13")                       // q9  = x[1]' = x[1]+d[1]
            __ASM_EMIT("vadd.f32        q10, q10, q12")                     // q10 = x[2]' = x[2]+d[0]
            __ASM_EMIT("vadd.f32        q11, q11, q13")                     // q11 = x[3]' = x[3]+d[1]
            __ASM_EMIT("vstm            %[dst]!, {q0-q3}")
            __ASM_EMIT("subs            %[n], %[n], $16")
            __ASM_EMIT("bhs             1b")

            // 8x block
            __ASM_EMIT("2:")
            __ASM_EMIT("adds            %[n], %[n], $8")
            __ASM_EMIT("blt             4f")
            __ASM_EMIT("vldm            %[src]!, {q0-q1}")
            __ASM_EMIT("vmul.f32        q4, q8, q14")                       // q4  = k*x[0]
            __ASM_EMIT("vmul.f32        q5, q9, q14")                       // q5  = k*x[1]
            __ASM_EMIT("vadd.f32        q4, q4, q15")                       // q0  = k*x[0] + y0
            __ASM_EMIT("vadd.f32        q5, q5, q15")                       // q1  = k*x[1] + y0
            __ASM_EMIT("vmul.f32        q0, q0, q4")                        // q0  = s0[0]*(k*x[0] + y0)
            __ASM_EMIT("vmul.f32        q1, q1, q5")                        // q1  = s0[1]*(k*x[1] + y0)
            __ASM_EMIT("vmov            q8, q10")                           // q8  = x[0]'
            __ASM_EMIT("vmov            q9, q11")                           // q9  = x[1]'
            __ASM_EMIT("vstm            %[dst]!, {q0-q1}")
            __ASM_EMIT("sub             %[n], %[n], $8")

            // 4x block
            __ASM_EMIT("4:")
            __ASM_EMIT("adds            %[n], %[n], $4")
            __ASM_EMIT("blt             6f")
            __ASM_EMIT("vldm            %[src]!, {q0}")
            __ASM_EMIT("vmul.f32        q4, q8, q14")                       // q4  = k*x[0]
            __ASM_EMIT("vadd.f32        q4, q4, q15")                       // q0  = k*x[0] + y0
            __ASM_EMIT("vmul.f32        q0, q0, q4")                        // q0  = s0[0]*(k*x[0] + y0)
            __ASM_EMIT("vmov            q8, q9")                            // q8  = x[0]'
            __ASM_EMIT("vstm            %[dst]!, {q0}")
            __ASM_EMIT("sub             %[n], %[n], $4")

            // 1x blocks
            __ASM_EMIT("6:")
            __ASM_EMIT("adds            %[n], %[n], $3")
            __ASM_EMIT("blt             8f")
            __ASM_EMIT("7:")
            __ASM_EMIT("vld1.32         {d0[], d1[]}, [%[src]]!")
            __ASM_EMIT("vmul.f32        q4, q8, q14")                       // q4  = k*x[0]
            __ASM_EMIT("vadd.f32        q4, q4, q15")                       // q0  = k*x[0] + y0
            __ASM_EMIT("vext.32         q8, q8, $1")                        // q8  = x[0]'
            __ASM_EMIT("vmul.f32        q0, q0, q4")                        // q0  = s0[0]*(k*x[0] + y0)
            __ASM_EMIT("vst1.32         {d0[0]}, [%[dst]]!")
            __ASM_EMIT("subs            %[n], %[n], $1")
            __ASM_EMIT("bge             7b")

            __ASM_EMIT("8:")
            : [dst] "+r" (dst), [src] "+r" (src),
              [n] "+r" (n),
              [CC] "+r" (CC)
            : [v] "r" (&v[0])
            : "cc", "memory",
              "q0", "q1", "q2", "q3",
              "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11",
              "q12", "q13", "q14", "q15"
        );
    }

    void lin_inter_fmadd2(float *dst, const float *src, int32_t x0, float y0, int32_t x1, float y1, int32_t x, uint32_t n)
    {
        IF_ARCH_ARM(
            float v[4];
            v[0]            = x1 - x0;
            v[1]            = x - x0;
            v[2]            = y1 - y0;
            v[3]            = y0;
            const float *CC = lin_inter_const;
        )

        ARCH_ARM_ASM(
            // Prepare parameters
            __ASM_EMIT("vld4.32         {d24[], d26[], d28[], d30[]}, [%[v]]")
            __ASM_EMIT("vldm            %[CC]!, {q8-q11}")                  // q8  = 0..3, q9=4..7, q10=8..11, q11=12..15
            __ASM_EMIT("vld4.32         {d25[], d27[], d29[], d31[]}, [%[v]]")  // q12 = (x1-x0), q13 = (x-x0), q14 = (y1-y0), q15 = y0
            __ASM_EMIT("vrecpe.f32      q0, q12")                           // q0  = s2
            __ASM_EMIT("vadd.f32        q8, q8, q13")                       // q8  = x[0]
            __ASM_EMIT("vrecps.f32      q1, q0, q12")                       // q1  = (2 - R*s2)
            __ASM_EMIT("vadd.f32        q9, q9, q13")                       // q9  = x[1]
            __ASM_EMIT("vmul.f32        q0, q1, q0")                        // q0  = s2' = s2 * (2 - R*s2)
            __ASM_EMIT("vadd.f32        q10, q10, q13")                     // q10 = x[2]
            __ASM_EMIT("vrecps.f32      q1, q0, q12")                       // q1  = (2 - R*s2')
            __ASM_EMIT("vadd.f32        q11, q11, q13")                     // q11 = x[3]
            __ASM_EMIT("vmul.f32        q12, q1, q0")                       // q14 = s2" = s2' * (2 - R*s2) = 1/s2
            __ASM_EMIT("vmul.f32        q14, q14, q12")                     // q14 = k = (y1-y0)/(x1-x0)
            __ASM_EMIT("vldm            %[CC], {q12-q13}")                  // q12 = d[0], q13=d[1]

            // Register allocation:
            // q8  = x[0]
            // q9  = x[1]
            // q10 = x[2]
            // q11 = x[3]
            // q12 = d[0]
            // q13 = d[1]
            // q14 = k
            // q15 = y0
            __ASM_EMIT("subs            %[n], %[n], $16")
            __ASM_EMIT("blo             2f")

            // 16x blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("vldm            %[src]!, {q0-q3}")
            __ASM_EMIT("vmul.f32        q4, q8, q14")                       // q4  = k*x[0]
            __ASM_EMIT("vmul.f32        q5, q9, q14")                       // q5  = k*x[1]
            __ASM_EMIT("vmul.f32        q6, q10, q14")                      // q6  = k*x[2]
            __ASM_EMIT("vmul.f32        q7, q11, q14")                      // q7  = k*x[3]
            __ASM_EMIT("vadd.f32        q4, q4, q15")                       // q0  = k*x[0] + y0
            __ASM_EMIT("vadd.f32        q5, q5, q15")                       // q1  = k*x[1] + y0
            __ASM_EMIT("vadd.f32        q6, q6, q15")                       // q2  = k*x[2] + y0
            __ASM_EMIT("vadd.f32        q7, q7, q15")                       // q3  = k*x[3] + y0
            __ASM_EMIT("vmul.f32        q0, q0, q4")                        // q0  = s0[0]*(k*x[0] + y0)
            __ASM_EMIT("vmul.f32        q1, q1, q5")                        // q1  = s0[1]*(k*x[1] + y0)
            __ASM_EMIT("vmul.f32        q2, q2, q6")                        // q2  = s0[2]*(k*x[2] + y0)
            __ASM_EMIT("vmul.f32        q3, q3, q7")                        // q3  = s0[3]*(k*x[3] + y0)
            __ASM_EMIT("vadd.f32        q8, q8, q12")                       // q8  = x[0]' = x[0]+d[0]
            __ASM_EMIT("vadd.f32        q9, q9, q13")                       // q9  = x[1]' = x[1]+d[1]
            __ASM_EMIT("vadd.f32        q10, q10, q12")                     // q10 = x[2]' = x[2]+d[0]
            __ASM_EMIT("vadd.f32        q11, q11, q13")                     // q11 = x[3]' = x[3]+d[1]
            __ASM_EMIT("vldm            %[dst], {q4-q7}")
            __ASM_EMIT("vadd.f32        q0, q0, q4")                        // q0  = s0[0]*(k*x[0] + y0) + s1[0]
            __ASM_EMIT("vadd.f32        q1, q1, q5")                        // q1  = s0[1]*(k*x[1] + y0) + s1[1]
            __ASM_EMIT("vadd.f32        q2, q2, q6")                        // q2  = s0[2]*(k*x[2] + y0) + s1[2]
            __ASM_EMIT("vadd.f32        q3, q3, q7")                        // q3  = s0[3]*(k*x[3] + y0) + s1[3]
            __ASM_EMIT("vstm            %[dst]!, {q0-q3}")
            __ASM_EMIT("subs            %[n], %[n], $16")
            __ASM_EMIT("bhs             1b")

            // 8x block
            __ASM_EMIT("2:")
            __ASM_EMIT("adds            %[n], %[n], $8")
            __ASM_EMIT("blt             4f")
            __ASM_EMIT("vldm            %[src]!, {q0-q1}")
            __ASM_EMIT("vmul.f32        q4, q8, q14")                       // q4  = k*x[0]
            __ASM_EMIT("vmul.f32        q5, q9, q14")                       // q5  = k*x[1]
            __ASM_EMIT("vadd.f32        q4, q4, q15")                       // q0  = k*x[0] + y0
            __ASM_EMIT("vadd.f32        q5, q5, q15")                       // q1  = k*x[1] + y0
            __ASM_EMIT("vmul.f32        q0, q0, q4")                        // q0  = s0[0]*(k*x[0] + y0)
            __ASM_EMIT("vmul.f32        q1, q1, q5")                        // q1  = s0[1]*(k*x[1] + y0)
            __ASM_EMIT("vmov            q8, q10")                           // q8  = x[0]'
            __ASM_EMIT("vmov            q9, q11")                           // q9  = x[1]'
            __ASM_EMIT("vldm            %[dst], {q4-q5}")
            __ASM_EMIT("vadd.f32        q0, q0, q4")                        // q0  = s0[0]*(k*x[0] + y0) + s1[0]
            __ASM_EMIT("vadd.f32        q1, q1, q5")                        // q1  = s0[1]*(k*x[1] + y0) + s1[1]
            __ASM_EMIT("vstm            %[dst]!, {q0-q1}")
            __ASM_EMIT("sub             %[n], %[n], $8")

            // 4x block
            __ASM_EMIT("4:")
            __ASM_EMIT("adds            %[n], %[n], $4")
            __ASM_EMIT("blt             6f")
            __ASM_EMIT("vldm            %[src]!, {q0}")
            __ASM_EMIT("vmul.f32        q4, q8, q14")                       // q4  = k*x[0]
            __ASM_EMIT("vadd.f32        q4, q4, q15")                       // q0  = k*x[0] + y0
            __ASM_EMIT("vmul.f32        q0, q0, q4")                        // q0  = s0[0]*(k*x[0] + y0)
            __ASM_EMIT("vmov            q8, q9")                            // q8  = x[0]'
            __ASM_EMIT("vldm            %[dst], {q4}")
            __ASM_EMIT("vadd.f32        q0, q0, q4")                        // q0  = s0[0]*(k*x[0] + y0) + s1[0]
            __ASM_EMIT("vstm            %[dst]!, {q0}")
            __ASM_EMIT("sub             %[n], %[n], $4")

            // 1x blocks
            __ASM_EMIT("6:")
            __ASM_EMIT("adds            %[n], %[n], $3")
            __ASM_EMIT("blt             8f")
            __ASM_EMIT("7:")
            __ASM_EMIT("vld1.32         {d0[], d1[]}, [%[src]]!")
            __ASM_EMIT("vmul.f32        q4, q8, q14")                       // q4  = k*x[0]
            __ASM_EMIT("vadd.f32        q4, q4, q15")                       // q0  = k*x[0] + y0
            __ASM_EMIT("vmul.f32        q0, q0, q4")                        // q0  = s0[0]*(k*x[0] + y0)
            __ASM_EMIT("vext.32         q8, q8, $1")                        // q8  = x[0]'
            __ASM_EMIT("vld1.32         {d8[], d9[]}, [%[dst]]")
            __ASM_EMIT("vadd.f32        q0, q0, q4")                        // q0  = s0[0]*(k*x[0] + y0) + s1[0]
            __ASM_EMIT("vst1.32         {d0[0]}, [%[dst]]!")
            __ASM_EMIT("subs            %[n], %[n], $1")
            __ASM_EMIT("bge             7b")

            __ASM_EMIT("8:")
            : [dst] "+r" (dst), [src] "+r" (src),
              [n] "+r" (n),
              [CC] "+r" (CC)
            : [v] "r" (&v[0])
            : "cc", "memory",
              "q0", "q1", "q2", "q3",
              "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11",
              "q12", "q13", "q14", "q15"
        );
    }

    void lin_inter_frmadd2(float *dst, const float *src, int32_t x0, float y0, int32_t x1, float y1, int32_t x, uint32_t n)
    {
        IF_ARCH_ARM(
            float v[4];
            v[0]            = x1 - x0;
            v[1]            = x - x0;
            v[2]            = y1 - y0;
            v[3]            = y0;
            const float *CC = lin_inter_const;
        )

        ARCH_ARM_ASM(
            // Prepare parameters
            __ASM_EMIT("vld4.32         {d24[], d26[], d28[], d30[]}, [%[v]]")
            __ASM_EMIT("vldm            %[CC]!, {q8-q11}")                  // q8  = 0..3, q9=4..7, q10=8..11, q11=12..15
            __ASM_EMIT("vld4.32         {d25[], d27[], d29[], d31[]}, [%[v]]")  // q12 = (x1-x0), q13 = (x-x0), q14 = (y1-y0), q15 = y0
            __ASM_EMIT("vrecpe.f32      q0, q12")                           // q0  = s2
            __ASM_EMIT("vadd.f32        q8, q8, q13")                       // q8  = x[0]
            __ASM_EMIT("vrecps.f32      q1, q0, q12")                       // q1  = (2 - R*s2)
            __ASM_EMIT("vadd.f32        q9, q9, q13")                       // q9  = x[1]
            __ASM_EMIT("vmul.f32        q0, q1, q0")                        // q0  = s2' = s2 * (2 - R*s2)
            __ASM_EMIT("vadd.f32        q10, q10, q13")                     // q10 = x[2]
            __ASM_EMIT("vrecps.f32      q1, q0, q12")                       // q1  = (2 - R*s2')
            __ASM_EMIT("vadd.f32        q11, q11, q13")                     // q11 = x[3]
            __ASM_EMIT("vmul.f32        q12, q1, q0")                       // q14 = s2" = s2' * (2 - R*s2) = 1/s2
            __ASM_EMIT("vmul.f32        q14, q14, q12")                     // q14 = k = (y1-y0)/(x1-x0)
            __ASM_EMIT("vldm            %[CC], {q12-q13}")                  // q12 = d[0], q13=d[1]

            // Register allocation:
            // q8  = x[0]
            // q9  = x[1]
            // q10 = x[2]
            // q11 = x[3]
            // q12 = d[0]
            // q13 = d[1]
            // q14 = k
            // q15 = y0
            __ASM_EMIT("subs            %[n], %[n], $16")
            __ASM_EMIT("blo             2f")

            // 16x blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("vldm            %[dst], {q0-q3}")
            __ASM_EMIT("vmul.f32        q4, q8, q14")                       // q4  = k*x[0]
            __ASM_EMIT("vmul.f32        q5, q9, q14")                       // q5  = k*x[1]
            __ASM_EMIT("vmul.f32        q6, q10, q14")                      // q6  = k*x[2]
            __ASM_EMIT("vmul.f32        q7, q11, q14")                      // q7  = k*x[3]
            __ASM_EMIT("vadd.f32        q4, q4, q15")                       // q0  = k*x[0] + y0
            __ASM_EMIT("vadd.f32        q5, q5, q15")                       // q1  = k*x[1] + y0
            __ASM_EMIT("vadd.f32        q6, q6, q15")                       // q2  = k*x[2] + y0
            __ASM_EMIT("vadd.f32        q7, q7, q15")                       // q3  = k*x[3] + y0
            __ASM_EMIT("vmul.f32        q0, q0, q4")                        // q0  = s0[0]*(k*x[0] + y0)
            __ASM_EMIT("vmul.f32        q1, q1, q5")                        // q1  = s0[1]*(k*x[1] + y0)
            __ASM_EMIT("vmul.f32        q2, q2, q6")                        // q2  = s0[2]*(k*x[2] + y0)
            __ASM_EMIT("vmul.f32        q3, q3, q7")                        // q3  = s0[3]*(k*x[3] + y0)
            __ASM_EMIT("vadd.f32        q8, q8, q12")                       // q8  = x[0]' = x[0]+d[0]
            __ASM_EMIT("vadd.f32        q9, q9, q13")                       // q9  = x[1]' = x[1]+d[1]
            __ASM_EMIT("vadd.f32        q10, q10, q12")                     // q10 = x[2]' = x[2]+d[0]
            __ASM_EMIT("vadd.f32        q11, q11, q13")                     // q11 = x[3]' = x[3]+d[1]
            __ASM_EMIT("vldm            %[src]!, {q4-q7}")
            __ASM_EMIT("vadd.f32        q0, q0, q4")                        // q0  = s0[0]*(k*x[0] + y0) + s1[0]
            __ASM_EMIT("vadd.f32        q1, q1, q5")                        // q1  = s0[1]*(k*x[1] + y0) + s1[1]
            __ASM_EMIT("vadd.f32        q2, q2, q6")                        // q2  = s0[2]*(k*x[2] + y0) + s1[2]
            __ASM_EMIT("vadd.f32        q3, q3, q7")                        // q3  = s0[3]*(k*x[3] + y0) + s1[3]
            __ASM_EMIT("vstm            %[dst]!, {q0-q3}")
            __ASM_EMIT("subs            %[n], %[n], $16")
            __ASM_EMIT("bhs             1b")

            // 8x block
            __ASM_EMIT("2:")
            __ASM_EMIT("adds            %[n], %[n], $8")
            __ASM_EMIT("blt             4f")
            __ASM_EMIT("vldm            %[dst], {q0-q1}")
            __ASM_EMIT("vmul.f32        q4, q8, q14")                       // q4  = k*x[0]
            __ASM_EMIT("vmul.f32        q5, q9, q14")                       // q5  = k*x[1]
            __ASM_EMIT("vadd.f32        q4, q4, q15")                       // q0  = k*x[0] + y0
            __ASM_EMIT("vadd.f32        q5, q5, q15")                       // q1  = k*x[1] + y0
            __ASM_EMIT("vmul.f32        q0, q0, q4")                        // q0  = s0[0]*(k*x[0] + y0)
            __ASM_EMIT("vmul.f32        q1, q1, q5")                        // q1  = s0[1]*(k*x[1] + y0)
            __ASM_EMIT("vmov            q8, q10")                           // q8  = x[0]'
            __ASM_EMIT("vmov            q9, q11")                           // q9  = x[1]'
            __ASM_EMIT("vldm            %[src]!, {q4-q5}")
            __ASM_EMIT("vadd.f32        q0, q0, q4")                        // q0  = s0[0]*(k*x[0] + y0) + s1[0]
            __ASM_EMIT("vadd.f32        q1, q1, q5")                        // q1  = s0[1]*(k*x[1] + y0) + s1[1]
            __ASM_EMIT("vstm            %[dst]!, {q0-q1}")
            __ASM_EMIT("sub             %[n], %[n], $8")

            // 4x block
            __ASM_EMIT("4:")
            __ASM_EMIT("adds            %[n], %[n], $4")
            __ASM_EMIT("blt             6f")
            __ASM_EMIT("vldm            %[dst], {q0}")
            __ASM_EMIT("vmul.f32        q4, q8, q14")                       // q4  = k*x[0]
            __ASM_EMIT("vadd.f32        q4, q4, q15")                       // q0  = k*x[0] + y0
            __ASM_EMIT("vmul.f32        q0, q0, q4")                        // q0  = s0[0]*(k*x[0] + y0)
            __ASM_EMIT("vmov            q8, q9")                            // q8  = x[0]'
            __ASM_EMIT("vldm            %[src]!, {q4}")
            __ASM_EMIT("vadd.f32        q0, q0, q4")                        // q0  = s0[0]*(k*x[0] + y0) + s1[0]
            __ASM_EMIT("vstm            %[dst]!, {q0}")
            __ASM_EMIT("sub             %[n], %[n], $4")

            // 1x blocks
            __ASM_EMIT("6:")
            __ASM_EMIT("adds            %[n], %[n], $3")
            __ASM_EMIT("blt             8f")
            __ASM_EMIT("7:")
            __ASM_EMIT("vld1.32         {d0[], d1[]}, [%[dst]]")
            __ASM_EMIT("vmul.f32        q4, q8, q14")                       // q4  = k*x[0]
            __ASM_EMIT("vadd.f32        q4, q4, q15")                       // q0  = k*x[0] + y0
            __ASM_EMIT("vmul.f32        q0, q0, q4")                        // q0  = s0[0]*(k*x[0] + y0)
            __ASM_EMIT("vext.32         q8, q8, $1")                        // q8  = x[0]'
            __ASM_EMIT("vld1.32         {d8[], d9[]}, [%[src]]!")
            __ASM_EMIT("vadd.f32        q0, q0, q4")                        // q0  = s0[0]*(k*x[0] + y0) + s1[0]
            __ASM_EMIT("vst1.32         {d0[0]}, [%[dst]]!")
            __ASM_EMIT("subs            %[n], %[n], $1")
            __ASM_EMIT("bge             7b")

            __ASM_EMIT("8:")
            : [dst] "+r" (dst), [src] "+r" (src),
              [n] "+r" (n),
              [CC] "+r" (CC)
            : [v] "r" (&v[0])
            : "cc", "memory",
              "q0", "q1", "q2", "q3",
              "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11",
              "q12", "q13", "q14", "q15"
        );
    }

    void lin_inter_fmadd3(float *dst, const float *src1, const float *src2, int32_t x0, float y0, int32_t x1, float y1, int32_t x, uint32_t n)
    {
        IF_ARCH_ARM(
            float v[4];
            v[0]            = x1 - x0;
            v[1]            = x - x0;
            v[2]            = y1 - y0;
            v[3]            = y0;
            const float *CC = lin_inter_const;
        )

        ARCH_ARM_ASM(
            // Prepare parameters
            __ASM_EMIT("vld4.32         {d24[], d26[], d28[], d30[]}, [%[v]]")
            __ASM_EMIT("vldm            %[CC]!, {q8-q11}")                  // q8  = 0..3, q9=4..7, q10=8..11, q11=12..15
            __ASM_EMIT("vld4.32         {d25[], d27[], d29[], d31[]}, [%[v]]")  // q12 = (x1-x0), q13 = (x-x0), q14 = (y1-y0), q15 = y0
            __ASM_EMIT("vrecpe.f32      q0, q12")                           // q0  = s2
            __ASM_EMIT("vadd.f32        q8, q8, q13")                       // q8  = x[0]
            __ASM_EMIT("vrecps.f32      q1, q0, q12")                       // q1  = (2 - R*s2)
            __ASM_EMIT("vadd.f32        q9, q9, q13")                       // q9  = x[1]
            __ASM_EMIT("vmul.f32        q0, q1, q0")                        // q0  = s2' = s2 * (2 - R*s2)
            __ASM_EMIT("vadd.f32        q10, q10, q13")                     // q10 = x[2]
            __ASM_EMIT("vrecps.f32      q1, q0, q12")                       // q1  = (2 - R*s2')
            __ASM_EMIT("vadd.f32        q11, q11, q13")                     // q11 = x[3]
            __ASM_EMIT("vmul.f32        q12, q1, q0")                       // q14 = s2" = s2' * (2 - R*s2) = 1/s2
            __ASM_EMIT("vmul.f32        q14, q14, q12")                     // q14 = k = (y1-y0)/(x1-x0)
            __ASM_EMIT("vldm            %[CC], {q12-q13}")                  // q12 = d[0], q13=d[1]

            // Register allocation:
            // q8  = x[0]
            // q9  = x[1]
            // q10 = x[2]
            // q11 = x[3]
            // q12 = d[0]
            // q13 = d[1]
            // q14 = k
            // q15 = y0
            __ASM_EMIT("subs            %[n], %[n], $16")
            __ASM_EMIT("blo             2f")

            // 16x blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("vldm            %[src1]!, {q0-q3}")
            __ASM_EMIT("vmul.f32        q4, q8, q14")                       // q4  = k*x[0]
            __ASM_EMIT("vmul.f32        q5, q9, q14")                       // q5  = k*x[1]
            __ASM_EMIT("vmul.f32        q6, q10, q14")                      // q6  = k*x[2]
            __ASM_EMIT("vmul.f32        q7, q11, q14")                      // q7  = k*x[3]
            __ASM_EMIT("vadd.f32        q4, q4, q15")                       // q0  = k*x[0] + y0
            __ASM_EMIT("vadd.f32        q5, q5, q15")                       // q1  = k*x[1] + y0
            __ASM_EMIT("vadd.f32        q6, q6, q15")                       // q2  = k*x[2] + y0
            __ASM_EMIT("vadd.f32        q7, q7, q15")                       // q3  = k*x[3] + y0
            __ASM_EMIT("vmul.f32        q0, q0, q4")                        // q0  = s0[0]*(k*x[0] + y0)
            __ASM_EMIT("vmul.f32        q1, q1, q5")                        // q1  = s0[1]*(k*x[1] + y0)
            __ASM_EMIT("vmul.f32        q2, q2, q6")                        // q2  = s0[2]*(k*x[2] + y0)
            __ASM_EMIT("vmul.f32        q3, q3, q7")                        // q3  = s0[3]*(k*x[3] + y0)
            __ASM_EMIT("vadd.f32        q8, q8, q12")                       // q8  = x[0]' = x[0]+d[0]
            __ASM_EMIT("vadd.f32        q9, q9, q13")                       // q9  = x[1]' = x[1]+d[1]
            __ASM_EMIT("vadd.f32        q10, q10, q12")                     // q10 = x[2]' = x[2]+d[0]
            __ASM_EMIT("vadd.f32        q11, q11, q13")                     // q11 = x[3]' = x[3]+d[1]
            __ASM_EMIT("vldm            %[src2]!, {q4-q7}")
            __ASM_EMIT("vadd.f32        q0, q0, q4")                        // q0  = s0[0]*(k*x[0] + y0) + s1[0]
            __ASM_EMIT("vadd.f32        q1, q1, q5")                        // q1  = s0[1]*(k*x[1] + y0) + s1[1]
            __ASM_EMIT("vadd.f32        q2, q2, q6")                        // q2  = s0[2]*(k*x[2] + y0) + s1[2]
            __ASM_EMIT("vadd.f32        q3, q3, q7")                        // q3  = s0[3]*(k*x[3] + y0) + s1[3]
            __ASM_EMIT("vstm            %[dst]!, {q0-q3}")
            __ASM_EMIT("subs            %[n], %[n], $16")
            __ASM_EMIT("bhs             1b")

            // 8x block
            __ASM_EMIT("2:")
            __ASM_EMIT("adds            %[n], %[n], $8")
            __ASM_EMIT("blt             4f")
            __ASM_EMIT("vldm            %[src1]!, {q0-q1}")
            __ASM_EMIT("vmul.f32        q4, q8, q14")                       // q4  = k*x[0]
            __ASM_EMIT("vmul.f32        q5, q9, q14")                       // q5  = k*x[1]
            __ASM_EMIT("vadd.f32        q4, q4, q15")                       // q0  = k*x[0] + y0
            __ASM_EMIT("vadd.f32        q5, q5, q15")                       // q1  = k*x[1] + y0
            __ASM_EMIT("vmul.f32        q0, q0, q4")                        // q0  = s0[0]*(k*x[0] + y0)
            __ASM_EMIT("vmul.f32        q1, q1, q5")                        // q1  = s0[1]*(k*x[1] + y0)
            __ASM_EMIT("vmov            q8, q10")                           // q8  = x[0]'
            __ASM_EMIT("vmov            q9, q11")                           // q9  = x[1]'
            __ASM_EMIT("vldm            %[src2]!, {q4-q5}")
            __ASM_EMIT("vadd.f32        q0, q0, q4")                        // q0  = s0[0]*(k*x[0] + y0) + s1[0]
            __ASM_EMIT("vadd.f32        q1, q1, q5")                        // q1  = s0[1]*(k*x[1] + y0) + s1[1]
            __ASM_EMIT("vstm            %[dst]!, {q0-q1}")
            __ASM_EMIT("sub             %[n], %[n], $8")

            // 4x block
            __ASM_EMIT("4:")
            __ASM_EMIT("adds            %[n], %[n], $4")
            __ASM_EMIT("blt             6f")
            __ASM_EMIT("vldm            %[src1]!, {q0}")
            __ASM_EMIT("vmul.f32        q4, q8, q14")                       // q4  = k*x[0]
            __ASM_EMIT("vadd.f32        q4, q4, q15")                       // q0  = k*x[0] + y0
            __ASM_EMIT("vmul.f32        q0, q0, q4")                        // q0  = s0[0]*(k*x[0] + y0)
            __ASM_EMIT("vmov            q8, q9")                            // q8  = x[0]'
            __ASM_EMIT("vldm            %[src2]!, {q4}")
            __ASM_EMIT("vadd.f32        q0, q0, q4")                        // q0  = s0[0]*(k*x[0] + y0) + s1[0]
            __ASM_EMIT("vstm            %[dst]!, {q0}")
            __ASM_EMIT("sub             %[n], %[n], $4")

            // 1x blocks
            __ASM_EMIT("6:")
            __ASM_EMIT("adds            %[n], %[n], $3")
            __ASM_EMIT("blt             8f")
            __ASM_EMIT("7:")
            __ASM_EMIT("vld1.32         {d0[], d1[]}, [%[src1]]!")
            __ASM_EMIT("vmul.f32        q4, q8, q14")                       // q4  = k*x[0]
            __ASM_EMIT("vadd.f32        q4, q4, q15")                       // q0  = k*x[0] + y0
            __ASM_EMIT("vmul.f32        q0, q0, q4")                        // q0  = s0[0]*(k*x[0] + y0)
            __ASM_EMIT("vext.32         q8, q8, $1")                        // q8  = x[0]'
            __ASM_EMIT("vld1.32         {d8[], d9[]}, [%[src2]]!")
            __ASM_EMIT("vadd.f32        q0, q0, q4")                        // q0  = s0[0]*(k*x[0] + y0) + s1[0]
            __ASM_EMIT("vst1.32         {d0[0]}, [%[dst]]!")
            __ASM_EMIT("subs            %[n], %[n], $1")
            __ASM_EMIT("bge             7b")

            __ASM_EMIT("8:")
            : [dst] "+r" (dst), [src1] "+r" (src1), [src2] "+r" (src2),
              [n] "+r" (n),
              [CC] "+r" (CC)
            : [v] "r" (&v[0])
            : "cc", "memory",
              "q0", "q1", "q2", "q3",
              "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11",
              "q12", "q13", "q14", "q15"
        );
    }
}

#endif /* DSP_ARCH_ARM_NEON_D32_INTERPOLATE_H_ */
