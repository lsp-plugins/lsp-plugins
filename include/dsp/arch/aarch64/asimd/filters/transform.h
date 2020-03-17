/*
 * transform.h
 *
 *  Created on: 11 февр. 2020 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_AARCH64_ASIMD_FILTERS_TRANSFORM_H_
#define DSP_ARCH_AARCH64_ASIMD_FILTERS_TRANSFORM_H_

#ifndef DSP_ARCH_AARCH64_ASIMD_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_AARCH64_ASIMD_IMPL */

namespace asimd
{
    void bilinear_transform_x1(biquad_x1_t *bf, const f_cascade_t *bc, float kf, size_t count)
    {
        ARCH_AARCH64_ASM
        (
            __ASM_EMIT("ld1r            {v22.4s}, [%[kf]]")             // v14  = kf
            __ASM_EMIT("subs            %[count], %[count], #4")
            __ASM_EMIT("fmul            v23.4s, v22.4s, v22.4s")        // v15  = kf*kf = kf2
            __ASM_EMIT("b.lo            2f")

            // x4 loop
            __ASM_EMIT("1:")
            __ASM_EMIT("ldp             q0, q1, [%[bc], #0x00]")
            __ASM_EMIT("ldp             q2, q3, [%[bc], #0x20]")
            __ASM_EMIT("ldp             q4, q5, [%[bc], #0x40]")
            __ASM_EMIT("ldp             q6, q7, [%[bc], #0x60]")
            // v0 = t00 t01 t02 t03
            // v2 = t10 t11 t12 t13
            // v4 = t20 t21 t22 t23
            // v6 = t30 t31 t32 t33
            // v1 = b00 b01 b02 b03
            // v3 = b10 b11 b12 b13
            // v5 = b20 b21 b22 b23
            // v7 = b30 b31 b32 b33
            __ASM_EMIT("zip1            v16.4s, v0.4s, v4.4s")          // v16 = t00 t20 t01 t21
            __ASM_EMIT("zip1            v18.4s, v2.4s, v6.4s")          // v18 = t10 t30 t11 t31
            __ASM_EMIT("zip2            v17.4s, v0.4s, v4.4s")          // v17 = t02 t22 t03 t23
            __ASM_EMIT("zip2            v19.4s, v2.4s, v6.4s")          // v19 = t12 t32 t13 t33
            __ASM_EMIT("zip1            v0.4s, v16.4s, v18.4s")         // v0  = t00 t10 t20 t30
            __ASM_EMIT("zip1            v4.4s, v17.4s, v19.4s")         // v4  = t02 t12 t22 t32
            __ASM_EMIT("zip2            v2.4s, v16.4s, v18.4s")         // v2  = t01 t11 t21 t31

            __ASM_EMIT("zip1            v16.4s, v1.4s, v5.4s")          // v16 = b00 b20 b01 b21
            __ASM_EMIT("zip1            v18.4s, v3.4s, v7.4s")          // v18 = b10 b30 b11 b31
            __ASM_EMIT("zip2            v17.4s, v1.4s, v5.4s")          // v17 = b02 b22 b03 b23
            __ASM_EMIT("zip2            v19.4s, v3.4s, v7.4s")          // v19 = b12 b32 b13 b33
            __ASM_EMIT("zip1            v1.4s, v16.4s, v18.4s")         // v1  = b00 b10 b20 b30
            __ASM_EMIT("zip1            v5.4s, v17.4s, v19.4s")         // v5  = b02 b12 b22 b32
            __ASM_EMIT("zip2            v3.4s, v16.4s, v18.4s")         // v3  = b01 b11 b21 b31

            // v0 = t[0] = T[0]
            // v2 = t[1]
            // v4 = t[2]
            // v6 = not used
            // q1 = b[0] = B[0]
            // q3 = b[1]
            // q5 = b[2]
            // q7 = not used
            __ASM_EMIT("fmul            v2.4s, v2.4s, v22.4s")          // v2   = t[1]*kf = T[1]
            __ASM_EMIT("fmul            v4.4s, v4.4s, v23.4s")          // v4   = t[2]*kf2 = T[2]
            __ASM_EMIT("fmul            v3.4s, v3.4s, v22.4s")          // v3   = b[1]*kf = B[1]
            __ASM_EMIT("fmul            v5.4s, v5.4s, v23.4s")          // v5   = b[2]*kf2 = B[2]

            __ASM_EMIT("fadd            v7.4s, v1.4s, v5.4s")           // v7   = B[0] + B[2]
            __ASM_EMIT("fsub            v6.4s, v0.4s, v2.4s")           // v6   = T[0] - T[1]
            __ASM_EMIT("fsub            v1.4s, v5.4s, v1.4s")           // v1   = B[2] - B[0]
            __ASM_EMIT("fsub            v16.4s, v0.4s, v4.4s")          // v16  = T[0] - T[2]
            __ASM_EMIT("fadd            v17.4s, v7.4s, v3.4s")          // v17  = B[0] + B[1] + B[2] = B
            __ASM_EMIT("fadd            v6.4s, v6.4s, v4.4s")           // v6   = T[0] - T[1] + T[2]
            __ASM_EMIT("frecpe          v18.4s, v17.4s")                // v18  = R, v17 = B
            __ASM_EMIT("fadd            v0.4s, v0.4s, v2.4s")           // v0   = T[0] + T[1]
            __ASM_EMIT("frecps          v19.4s, v18.4s, v17.4s")        // v19  = (2 - R*B)
            __ASM_EMIT("fadd            v16.4s, v16.4s, v16.4s")        // v16  = 2 * (T[0] - T[2])
            __ASM_EMIT("fmul            v18.4s, v19.4s, v18.4s")        // v18  = B' = B * (2 - R*B)
            __ASM_EMIT("fadd            v0.4s, v0.4s, v4.4s")           // v0   = T[0] + T[1] + T[2]
            __ASM_EMIT("frecps          v19.4s, v18.4s, v17.4s")        // v19  = (2 - R*B')
            __ASM_EMIT("fadd            v1.4s, v1.4s, v1.4s")           // v1   = 2 * (B[2] - B[0])
            __ASM_EMIT("fmul            v17.4s, v19.4s, v18.4s")        // v17  = B" = B' * (2 - R*B) = 1/B = N
            __ASM_EMIT("fsub            v3.4s, v3.4s, v7.4s")           // v3   = B[1] - B[0] - B[2]
            __ASM_EMIT("fmul            v0.4s, v0.4s, v17.4s")          // v0   = (T[0] + T[1] + T[2]) * N = A0
            __ASM_EMIT("fmul            v2.4s, v16.4s, v17.4s")         // v2   = 2 * (T[0] - T[2]) * N = A1
            __ASM_EMIT("fmul            v4.4s, v6.4s, v17.4s")          // v4   = (T[0] - T[1] + T[2]) * N = A2
            __ASM_EMIT("eor             v5.16b, v5.16b, v5.16b")        // v5   = 0
            __ASM_EMIT("fmul            v6.4s, v1.4s, v17.4s")          // v6   = 2 * (B[2] - B[0]) * N = B1
            __ASM_EMIT("eor             v7.16b, v7.16b, v7.16b")        // v7   = 0
            __ASM_EMIT("fmul            v1.4s, v3.4s, v17.4s")          // v1   = (B[1] - B[0] - B[2]) * N = B2
            __ASM_EMIT("eor             v3.16b, v3.16b, v3.16b")        // v3   = 0

            // Transpose back
            __ASM_EMIT("zip1            v16.4s, v0.4s, v4.4s")
            __ASM_EMIT("zip1            v18.4s, v2.4s, v6.4s")
            __ASM_EMIT("zip2            v17.4s, v0.4s, v4.4s")
            __ASM_EMIT("zip2            v19.4s, v2.4s, v6.4s")
            __ASM_EMIT("zip1            v0.4s, v16.4s, v18.4s")
            __ASM_EMIT("zip1            v4.4s, v17.4s, v19.4s")
            __ASM_EMIT("zip2            v2.4s, v16.4s, v18.4s")
            __ASM_EMIT("zip2            v6.4s, v17.4s, v19.4s")

            __ASM_EMIT("zip1            v16.4s, v1.4s, v5.4s")
            __ASM_EMIT("zip1            v18.4s, v3.4s, v7.4s")
            __ASM_EMIT("zip2            v17.4s, v1.4s, v5.4s")
            __ASM_EMIT("zip2            v19.4s, v3.4s, v7.4s")
            __ASM_EMIT("zip1            v1.4s, v16.4s, v18.4s")
            __ASM_EMIT("zip1            v5.4s, v17.4s, v19.4s")
            __ASM_EMIT("zip2            v3.4s, v16.4s, v18.4s")
            __ASM_EMIT("zip2            v7.4s, v17.4s, v19.4s")

            __ASM_EMIT("stp             q0, q1, [%[bf], #0x00]")
            __ASM_EMIT("stp             q2, q3, [%[bf], #0x20]")
            __ASM_EMIT("stp             q4, q5, [%[bf], #0x40]")
            __ASM_EMIT("stp             q6, q7, [%[bf], #0x60]")

            __ASM_EMIT("subs            %[count], %[count], #4")
            __ASM_EMIT("add             %[bc], %[bc], #0x80")
            __ASM_EMIT("add             %[bf], %[bf], #0x80")
            __ASM_EMIT("b.hs            1b")
            __ASM_EMIT("2:")

            // 1x blocks
            __ASM_EMIT("adds            %[count], %[count], #3")
            __ASM_EMIT("b.lt            4f")
            __ASM_EMIT("3:")
            __ASM_EMIT("ld4             {v0.2s, v1.2s, v2.2s, v3.2s}, [%[bc]]")
            // v0 = t[0] b[0] = T[0] B[0]
            // v1 = t[1] b[1]
            // v2 = t[2] b[2]
            // v3 = 0 0
            __ASM_EMIT("fmul            v1.2s, v1.2s, v22.2s")          // v1   = t[1]*kf b[1]*kf = T[1] B[1]
            __ASM_EMIT("fmul            v2.2s, v2.2s, v23.2s")          // v2   = t[2]*kf2 b[2]*kf2 = T[2] B[2]

            __ASM_EMIT("fadd            v4.2s, v0.2s, v2.2s")           // v4   = T[0]+T[2], B[0]+B[2]
            __ASM_EMIT("fsub            v5.2s, v0.2s, v2.2s")           // v5   = T[0]-T[2], B[0]-B[2]
            __ASM_EMIT("fadd            v6.2s, v4.2s, v1.2s")           // v6   = T[0]+T[1]+T[2], B[0]+B[1]+B[2]
            __ASM_EMIT("fadd            v5.2s, v5.2s, v5.2s")           // v5   = 2*(T[0]-T[2]), 2*(B[0]-B[2])
            __ASM_EMIT("fsub            v4.2s, v4.2s, v1.2s")           // v4   = T[0]-T[1]+T[2], B[0]-B[1]+B[2]

            __ASM_EMIT("frecpe          v7.2s, v6.2s")                  // v7   = R, v6 = B
            __ASM_EMIT("frecps          v8.2s, v7.2s, v6.2s")           // v8   = (2 - R*B)
            __ASM_EMIT("fmul            v7.2s, v8.2s, v7.2s")           // v7   = B' = B * (2 - R*B)
            __ASM_EMIT("frecps          v8.2s, v7.2s, v6.2s")           // v8   = (2 - R*B')
            __ASM_EMIT("fmul            v16.2s, v8.2s, v7.2s")          // v16  = B" = B' * (2 - R*B) = 1/B = ? N
            __ASM_EMIT("fneg            v7.2s, v7.2s")                  // v16  = ? -N
            __ASM_EMIT("mov             v7.s[0], v16.s[1]")             // v7   = N -N

            __ASM_EMIT("fmul            v0.2s, v6.2s, v7.2s")           // v0   = (T[0]+T[1]+T[2])*N, (B[0]+B[1]+B[2])*N = A0, 1
            __ASM_EMIT("fmul            v1.2s, v5.2s, v7.2s")           // v1   = 2*(T[0]-T[2])*N, 2*(B[0]-B[2])*N = A1, B1
            __ASM_EMIT("fmul            v2.2s, v4.2s, v7.2s")           // v2   = (T[0]-T[1]+T[2])*N, (B[0]-B[1]+B[2])*N = A2, B2
            __ASM_EMIT("eor             v3.16b, v3.16b, v3.16b")        // v3   = 0 0 0 0
            __ASM_EMIT("mov             v0.s[1], v2.s[0]")              // v0   = A0 A2
            __ASM_EMIT("mov             v3.s[0], v2.s[1]")              // v3   = B2 0 0 0
            __ASM_EMIT("zip1            v2.4s, v0.4s, v1.4s")           // v2   = A0 A1 A2 B1
            __ASM_EMIT("stp             q2, q3, [%[bf]]")

            __ASM_EMIT("subs            %[count], %[count], #1")
            __ASM_EMIT("add             %[bc], %[bc], #0x20")
            __ASM_EMIT("add             %[bf], %[bf], #0x20")
            __ASM_EMIT("b.ge            3b")

            __ASM_EMIT("4:")
            : [bf] "+r" (bf), [bc] "+r" (bc), [count] "+r" (count)
            : [kf] "r" (&kf)
            : "cc", "memory",
              "q0", "q1", "q2", "q3",
              "q4", "q5", "q6", "q7",
              "q16", "q17", "q18", "q19",
              "q20", "q21", "q22", "q23"
        );
    }

    void bilinear_transform_x2(biquad_x2_t *bf, const f_cascade_t *bc, float kf, size_t count)
    {
        ARCH_AARCH64_ASM
        (
            __ASM_EMIT("ld1r            {v22.4s}, [%[kf]]")             // v14  = kf
            __ASM_EMIT("subs            %[count], %[count], #2")
            __ASM_EMIT("fmul            v23.4s, v22.4s, v22.4s")        // v15  = kf*kf = kf2
            __ASM_EMIT("b.lo            2f")

            // x4 loop
            __ASM_EMIT("1:")
            __ASM_EMIT("ldp             q0, q1, [%[bc], #0x00]")
            __ASM_EMIT("ldp             q2, q3, [%[bc], #0x20]")
            __ASM_EMIT("ldp             q4, q5, [%[bc], #0x40]")
            __ASM_EMIT("ldp             q6, q7, [%[bc], #0x60]")
            // v0 = t00 t01 t02 t03
            // v2 = t10 t11 t12 t13
            // v4 = t20 t21 t22 t23
            // v6 = t30 t31 t32 t33
            // v1 = b00 b01 b02 b03
            // v3 = b10 b11 b12 b13
            // v5 = b20 b21 b22 b23
            // v7 = b30 b31 b32 b33
            __ASM_EMIT("zip1            v16.4s, v0.4s, v4.4s")          // v16 = t00 t20 t01 t21
            __ASM_EMIT("zip1            v18.4s, v2.4s, v6.4s")          // v18 = t10 t30 t11 t31
            __ASM_EMIT("zip2            v17.4s, v0.4s, v4.4s")          // v17 = t02 t22 t03 t23
            __ASM_EMIT("zip2            v19.4s, v2.4s, v6.4s")          // v19 = t12 t32 t13 t33
            __ASM_EMIT("zip1            v0.4s, v16.4s, v18.4s")         // v0  = t00 t10 t20 t30
            __ASM_EMIT("zip1            v4.4s, v17.4s, v19.4s")         // v4  = t02 t12 t22 t32
            __ASM_EMIT("zip2            v2.4s, v16.4s, v18.4s")         // v2  = t01 t11 t21 t31
            __ASM_EMIT("fmul            v4.4s, v4.4s, v23.4s")          // v4   = t[2]*kf2 = T[2]
            __ASM_EMIT("fmul            v2.4s, v2.4s, v22.4s")          // v2   = t[1]*kf = T[1]

            __ASM_EMIT("zip1            v16.4s, v1.4s, v5.4s")          // v16 = b00 b20 b01 b21
            __ASM_EMIT("zip1            v18.4s, v3.4s, v7.4s")          // v18 = b10 b30 b11 b31
            __ASM_EMIT("zip2            v17.4s, v1.4s, v5.4s")          // v17 = b02 b22 b03 b23
            __ASM_EMIT("zip2            v19.4s, v3.4s, v7.4s")          // v19 = b12 b32 b13 b33
            __ASM_EMIT("zip1            v1.4s, v16.4s, v18.4s")         // v1  = b00 b10 b20 b30
            __ASM_EMIT("zip1            v5.4s, v17.4s, v19.4s")         // v5  = b02 b12 b22 b32
            __ASM_EMIT("zip2            v3.4s, v16.4s, v18.4s")         // v3  = b01 b11 b21 b31
            __ASM_EMIT("fmul            v5.4s, v5.4s, v23.4s")          // v5   = b[2]*kf2 = B[2]
            __ASM_EMIT("fmul            v3.4s, v3.4s, v22.4s")          // v3   = b[1]*kf = B[1]

            // v0 = T[0]
            // v2 = T[1]
            // v4 = T[2]
            // v6 = not used
            // q1 = B[0]
            // q3 = B[1]
            // q5 = B[2]
            // q7 = not used
            __ASM_EMIT("fadd            v7.4s, v1.4s, v5.4s")           // v7   = B[0] + B[2]
            __ASM_EMIT("fsub            v6.4s, v0.4s, v2.4s")           // v6   = T[0] - T[1]
            __ASM_EMIT("fsub            v1.4s, v5.4s, v1.4s")           // v1   = B[2] - B[0]
            __ASM_EMIT("fsub            v16.4s, v0.4s, v4.4s")          // v16  = T[0] - T[2]
            __ASM_EMIT("fadd            v17.4s, v7.4s, v3.4s")          // v17  = B[0] + B[1] + B[2] = B
            __ASM_EMIT("fadd            v6.4s, v6.4s, v4.4s")           // v6   = T[0] - T[1] + T[2]
            __ASM_EMIT("frecpe          v18.4s, v17.4s")                // v18  = R, v17 = B
            __ASM_EMIT("fadd            v0.4s, v0.4s, v2.4s")           // v0   = T[0] + T[1]
            __ASM_EMIT("frecps          v19.4s, v18.4s, v17.4s")        // v19  = (2 - R*B)
            __ASM_EMIT("fadd            v16.4s, v16.4s, v16.4s")        // v16  = 2 * (T[0] - T[2])
            __ASM_EMIT("fmul            v18.4s, v19.4s, v18.4s")        // v18  = B' = B * (2 - R*B)
            __ASM_EMIT("fadd            v0.4s, v0.4s, v4.4s")           // v0   = T[0] + T[1] + T[2]
            __ASM_EMIT("frecps          v19.4s, v18.4s, v17.4s")        // v19  = (2 - R*B')
            __ASM_EMIT("fadd            v1.4s, v1.4s, v1.4s")           // v1   = 2 * (B[2] - B[0])
            __ASM_EMIT("fmul            v17.4s, v19.4s, v18.4s")        // v17  = B" = B' * (2 - R*B) = 1/B = N
            __ASM_EMIT("fsub            v3.4s, v3.4s, v7.4s")           // v3   = B[1] - B[0] - B[2]
            __ASM_EMIT("fmul            v0.4s, v0.4s, v17.4s")          // v0   = (T[0] + T[1] + T[2]) * N = A0
            __ASM_EMIT("fmul            v2.4s, v16.4s, v17.4s")         // v2   = 2 * (T[0] - T[2]) * N = A1
            __ASM_EMIT("fmul            v4.4s, v6.4s, v17.4s")          // v4   = (T[0] - T[1] + T[2]) * N = A2
            __ASM_EMIT("fmul            v6.4s, v1.4s, v17.4s")          // v6   = 2 * (B[2] - B[0]) * N = B1
            __ASM_EMIT("fmul            v7.4s, v3.4s, v17.4s")          // v7   = (B[1] - B[0] - B[2]) * N = B2

            // Transpose back
            __ASM_EMIT("trn2            v1.2d, v0.2d, v2.2d")           // v1   = A01 A11
            __ASM_EMIT("trn1            v0.2d, v0.2d, v2.2d")           // v0   = A00 A01
            __ASM_EMIT("trn2            v3.2d, v4.2d, v6.2d")           // v3   = A21 B11
            __ASM_EMIT("trn1            v2.2d, v4.2d, v6.2d")           // v2   = A20 B10
            __ASM_EMIT("eor             v5.16b, v5.16b, v5.16b")        // v5   = 0 0
            __ASM_EMIT("trn1            v4.2d, v7.2d, v5.2d")           // v4   = B20 0
            __ASM_EMIT("trn2            v5.2d, v7.2d, v5.2d")           // v5   = B21 0

            __ASM_EMIT("stp             q0, q2, [%[bf], #0x00]")
            __ASM_EMIT("stp             q4, q1, [%[bf], #0x20]")
            __ASM_EMIT("stp             q3, q5, [%[bf], #0x40]")

            __ASM_EMIT("subs            %[count], %[count], #2")
            __ASM_EMIT("add             %[bc], %[bc], #0x80")
            __ASM_EMIT("add             %[bf], %[bf], #0x60")
            __ASM_EMIT("b.hs            1b")
            __ASM_EMIT("2:")

            // 1x blocks
            __ASM_EMIT("adds            %[count], %[count], #1")
            __ASM_EMIT("b.lt            4f")
            __ASM_EMIT("ld4             {v0.4s, v1.4s, v2.4s, v3.4s}, [%[bc]]")
            // v0 = t[0] b[0] = T[0] B[0]
            // v1 = t[1] b[1]
            // v2 = t[2] b[2]
            // v3 = 0 0
            __ASM_EMIT("fmul            v1.4s, v1.4s, v22.4s")          // v1   = t[1]*kf b[1]*kf = T[1] B[1]
            __ASM_EMIT("fmul            v2.4s, v2.4s, v23.4s")          // v2   = t[2]*kf2 b[2]*kf2 = T[2] B[2]

            __ASM_EMIT("fadd            v4.4s, v0.4s, v2.4s")           // v4   = T[0]+T[2], B[0]+B[2]
            __ASM_EMIT("fsub            v5.4s, v0.4s, v2.4s")           // v5   = T[0]-T[2], B[0]-B[2]
            __ASM_EMIT("fadd            v6.4s, v4.4s, v1.4s")           // v6   = T[0]+T[1]+T[2], B[0]+B[1]+B[2]
            __ASM_EMIT("fadd            v5.4s, v5.4s, v5.4s")           // v5   = 2*(T[0]-T[2]), 2*(B[0]-B[2])
            __ASM_EMIT("fsub            v4.4s, v4.4s, v1.4s")           // v4   = T[0]-T[1]+T[2], B[0]-B[1]+B[2]

            __ASM_EMIT("frecpe          v7.4s, v6.4s")                  // v7   = R, v6 = B
            __ASM_EMIT("frecps          v8.4s, v7.4s, v6.4s")           // v8   = (2 - R*B)
            __ASM_EMIT("fmul            v7.4s, v8.4s, v7.4s")           // v7   = B' = B * (2 - R*B)
            __ASM_EMIT("frecps          v8.4s, v7.4s, v6.4s")           // v8   = (2 - R*B')
            __ASM_EMIT("fmul            v16.4s, v8.4s, v7.4s")          // v16  = B" = B' * (2 - R*B) = 1/B = ? N
            __ASM_EMIT("fneg            v7.4s, v7.4s")                  // v7   = ? -N
            __ASM_EMIT("trn2            v7.4s, v16.4s, v7.4s")          // v7   = N -N

            __ASM_EMIT("fmul            v0.4s, v6.4s, v7.4s")           // v0   = (T[0]+T[1]+T[2])*N, (B[0]+B[1]+B[2])*N = A0, 1
            __ASM_EMIT("fmul            v1.4s, v5.4s, v7.4s")           // v1   = 2*(T[0]-T[2])*N, 2*(B[0]-B[2])*N = A1, B1
            __ASM_EMIT("fmul            v2.4s, v4.4s, v7.4s")           // v2   = (T[0]-T[1]+T[2])*N, (B[0]-B[1]+B[2])*N = A2, B2
            __ASM_EMIT("ext             v3.16b, v1.16b, v1.16b, #4")    // v3   = B10 A11 B11 A10
            // v0 = A00 1   A01 1
            // v1 = A10 B10 A11 B11
            // v2 = A20 B20 A21 B21
            // v3 = B10 A11 B11 A10
            __ASM_EMIT("uzp1            v4.4s, v0.4s, v1.4s")           // v4   = A00 A01 A10 A11
            __ASM_EMIT("uzp1            v5.4s, v2.4s, v3.4s")           // v5   = A20 A21 B10 B11
            __ASM_EMIT("eor             v0.16b, v0.16b, v0.16b")        // v0   = 0 0 0 0
            __ASM_EMIT("uzp2            v6.4s, v2.4s, v0.4s")           // v6   = B20 B21 0 0

            __ASM_EMIT("stp             q4, q5, [%[bf], #0x00]")
            __ASM_EMIT("str             q6, [%[bf], #0x20]")

            __ASM_EMIT("4:")
            : [bf] "+r" (bf), [bc] "+r" (bc), [count] "+r" (count)
            : [kf] "r" (&kf)
            : "cc", "memory",
              "q0", "q1", "q2", "q3",
              "q4", "q5", "q6", "q7",
              "q16", "q17", "q18", "q19",
              "q20", "q21", "q22", "q23"
        );
    }

    void bilinear_transform_x4(biquad_x4_t *bf, const f_cascade_t *bc, float kf, size_t count)
    {
        ARCH_AARCH64_ASM
        (
            __ASM_EMIT("ld1r            {v22.4s}, [%[kf]]")             // v14  = kf
            __ASM_EMIT("subs            %[count], %[count], #1")
            __ASM_EMIT("fmul            v23.4s, v22.4s, v22.4s")        // v15  = kf*kf = kf2
            __ASM_EMIT("b.lo            2f")

            // x4 loop
            __ASM_EMIT("1:")
            __ASM_EMIT("ldp             q0, q1, [%[bc], #0x00]")
            __ASM_EMIT("ldp             q2, q3, [%[bc], #0x20]")
            __ASM_EMIT("ldp             q4, q5, [%[bc], #0x40]")
            __ASM_EMIT("ldp             q6, q7, [%[bc], #0x60]")
            // v0 = t00 t01 t02 t03
            // v2 = t10 t11 t12 t13
            // v4 = t20 t21 t22 t23
            // v6 = t30 t31 t32 t33
            // v1 = b00 b01 b02 b03
            // v3 = b10 b11 b12 b13
            // v5 = b20 b21 b22 b23
            // v7 = b30 b31 b32 b33
            __ASM_EMIT("zip1            v16.4s, v0.4s, v4.4s")          // v16 = t00 t20 t01 t21
            __ASM_EMIT("zip1            v18.4s, v2.4s, v6.4s")          // v18 = t10 t30 t11 t31
            __ASM_EMIT("zip2            v17.4s, v0.4s, v4.4s")          // v17 = t02 t22 t03 t23
            __ASM_EMIT("zip2            v19.4s, v2.4s, v6.4s")          // v19 = t12 t32 t13 t33
            __ASM_EMIT("zip1            v0.4s, v16.4s, v18.4s")         // v0  = t00 t10 t20 t30
            __ASM_EMIT("zip1            v4.4s, v17.4s, v19.4s")         // v4  = t02 t12 t22 t32
            __ASM_EMIT("zip2            v2.4s, v16.4s, v18.4s")         // v2  = t01 t11 t21 t31
            __ASM_EMIT("fmul            v4.4s, v4.4s, v23.4s")          // v4   = t[2]*kf2 = T[2]
            __ASM_EMIT("fmul            v2.4s, v2.4s, v22.4s")          // v2   = t[1]*kf = T[1]

            __ASM_EMIT("zip1            v16.4s, v1.4s, v5.4s")          // v16 = b00 b20 b01 b21
            __ASM_EMIT("zip1            v18.4s, v3.4s, v7.4s")          // v18 = b10 b30 b11 b31
            __ASM_EMIT("zip2            v17.4s, v1.4s, v5.4s")          // v17 = b02 b22 b03 b23
            __ASM_EMIT("zip2            v19.4s, v3.4s, v7.4s")          // v19 = b12 b32 b13 b33
            __ASM_EMIT("zip1            v1.4s, v16.4s, v18.4s")         // v1  = b00 b10 b20 b30
            __ASM_EMIT("zip1            v5.4s, v17.4s, v19.4s")         // v5  = b02 b12 b22 b32
            __ASM_EMIT("zip2            v3.4s, v16.4s, v18.4s")         // v3  = b01 b11 b21 b31
            __ASM_EMIT("fmul            v5.4s, v5.4s, v23.4s")          // v5   = b[2]*kf2 = B[2]
            __ASM_EMIT("fmul            v3.4s, v3.4s, v22.4s")          // v3   = b[1]*kf = B[1]

            // v0 = T[0]
            // v2 = T[1]
            // v4 = T[2]
            // v6 = not used
            // q1 = B[0]
            // q3 = B[1]
            // q5 = B[2]
            // q7 = not used
            __ASM_EMIT("fadd            v7.4s, v1.4s, v5.4s")           // v7   = B[0] + B[2]
            __ASM_EMIT("fsub            v6.4s, v0.4s, v2.4s")           // v6   = T[0] - T[1]
            __ASM_EMIT("fsub            v1.4s, v5.4s, v1.4s")           // v1   = B[2] - B[0]
            __ASM_EMIT("fsub            v16.4s, v0.4s, v4.4s")          // v16  = T[0] - T[2]
            __ASM_EMIT("fadd            v17.4s, v7.4s, v3.4s")          // v17  = B[0] + B[1] + B[2] = B
            __ASM_EMIT("fadd            v6.4s, v6.4s, v4.4s")           // v6   = T[0] - T[1] + T[2]
            __ASM_EMIT("frecpe          v18.4s, v17.4s")                // v18  = R, v17 = B
            __ASM_EMIT("fadd            v0.4s, v0.4s, v2.4s")           // v0   = T[0] + T[1]
            __ASM_EMIT("frecps          v19.4s, v18.4s, v17.4s")        // v19  = (2 - R*B)
            __ASM_EMIT("fadd            v16.4s, v16.4s, v16.4s")        // v16  = 2 * (T[0] - T[2])
            __ASM_EMIT("fmul            v18.4s, v19.4s, v18.4s")        // v18  = B' = B * (2 - R*B)
            __ASM_EMIT("fadd            v0.4s, v0.4s, v4.4s")           // v0   = T[0] + T[1] + T[2]
            __ASM_EMIT("frecps          v19.4s, v18.4s, v17.4s")        // v19  = (2 - R*B')
            __ASM_EMIT("fadd            v1.4s, v1.4s, v1.4s")           // v1   = 2 * (B[2] - B[0])
            __ASM_EMIT("fmul            v17.4s, v19.4s, v18.4s")        // v17  = B" = B' * (2 - R*B) = 1/B = N
            __ASM_EMIT("fsub            v3.4s, v3.4s, v7.4s")           // v3   = B[1] - B[0] - B[2]
            __ASM_EMIT("fmul            v0.4s, v0.4s, v17.4s")          // v0   = (T[0] + T[1] + T[2]) * N = A0
            __ASM_EMIT("fmul            v2.4s, v16.4s, v17.4s")         // v2   = 2 * (T[0] - T[2]) * N = A1
            __ASM_EMIT("fmul            v4.4s, v6.4s, v17.4s")          // v4   = (T[0] - T[1] + T[2]) * N = A2
            __ASM_EMIT("fmul            v6.4s, v1.4s, v17.4s")          // v6   = 2 * (B[2] - B[0]) * N = B1
            __ASM_EMIT("fmul            v7.4s, v3.4s, v17.4s")          // v7   = (B[1] - B[0] - B[2]) * N = B2

            __ASM_EMIT("stp             q0, q2, [%[bf], #0x00]")
            __ASM_EMIT("stp             q4, q6, [%[bf], #0x20]")
            __ASM_EMIT("str             q7, [%[bf], #0x40]")

            __ASM_EMIT("subs            %[count], %[count], #1")
            __ASM_EMIT("add             %[bc], %[bc], #0x80")
            __ASM_EMIT("add             %[bf], %[bf], #0x50")
            __ASM_EMIT("b.hs            1b")
            __ASM_EMIT("2:")

            : [bf] "+r" (bf), [bc] "+r" (bc), [count] "+r" (count)
            : [kf] "r" (&kf)
            : "cc", "memory",
              "q0", "q1", "q2", "q3",
              "q4", "q5", "q6", "q7",
              "q16", "q17", "q18", "q19",
              "q20", "q21", "q22", "q23"
        );
    }

    void bilinear_transform_x8(biquad_x8_t *bf, const f_cascade_t *bc, float kf, size_t count)
    {
        ARCH_AARCH64_ASM
        (
            __ASM_EMIT("ld1r            {v22.4s}, [%[kf]]")             // v14  = kf
            __ASM_EMIT("subs            %[count], %[count], #1")
            __ASM_EMIT("fmul            v23.4s, v22.4s, v22.4s")        // v15  = kf*kf = kf2
            __ASM_EMIT("b.lo            2f")

            // x4 loop
            __ASM_EMIT("1:")
            __ASM_EMIT("ldp             q0, q1, [%[bc], #0x00]")
            __ASM_EMIT("ldp             q2, q3, [%[bc], #0x20]")
            __ASM_EMIT("ldp             q4, q5, [%[bc], #0x40]")
            __ASM_EMIT("ldp             q6, q7, [%[bc], #0x60]")
            __ASM_EMIT("ldp             q24, q25, [%[bc], #0x80]")
            __ASM_EMIT("ldp             q26, q27, [%[bc], #0xa0]")
            __ASM_EMIT("ldp             q28, q29, [%[bc], #0xc0]")
            __ASM_EMIT("ldp             q30, q31, [%[bc], #0xe0]")
            // v0 = t00 t01 t02 t03
            // v2 = t10 t11 t12 t13
            // v4 = t20 t21 t22 t23
            // v6 = t30 t31 t32 t33
            // v1 = b00 b01 b02 b03
            // v3 = b10 b11 b12 b13
            // v5 = b20 b21 b22 b23
            // v7 = b30 b31 b32 b33
            __ASM_EMIT("zip1            v16.4s, v0.4s, v4.4s")          // v16  = t00 t20 t01 t21
            __ASM_EMIT("zip1            v18.4s, v2.4s, v6.4s")          // v18  = t10 t30 t11 t31
            __ASM_EMIT("zip2            v17.4s, v0.4s, v4.4s")          // v17  = t02 t22 t03 t23
            __ASM_EMIT("zip2            v19.4s, v2.4s, v6.4s")          // v19  = t12 t32 t13 t33
            __ASM_EMIT("zip1            v0.4s, v16.4s, v18.4s")         // v0   = t00 t10 t20 t30
            __ASM_EMIT("zip1            v4.4s, v17.4s, v19.4s")         // v4   = t02 t12 t22 t32
            __ASM_EMIT("zip2            v2.4s, v16.4s, v18.4s")         // v2   = t01 t11 t21 t31
            __ASM_EMIT("fmul            v4.4s, v4.4s, v23.4s")          // v4   = t[2]*kf2 = T[2]
            __ASM_EMIT("fmul            v2.4s, v2.4s, v22.4s")          // v2   = t[1]*kf = T[1]

            __ASM_EMIT("zip1            v16.4s, v1.4s, v5.4s")          // v16  = b00 b20 b01 b21
            __ASM_EMIT("zip1            v18.4s, v3.4s, v7.4s")          // v18  = b10 b30 b11 b31
            __ASM_EMIT("zip2            v17.4s, v1.4s, v5.4s")          // v17  = b02 b22 b03 b23
            __ASM_EMIT("zip2            v19.4s, v3.4s, v7.4s")          // v19  = b12 b32 b13 b33
            __ASM_EMIT("zip1            v1.4s, v16.4s, v18.4s")         // v1   = b00 b10 b20 b30
            __ASM_EMIT("zip1            v5.4s, v17.4s, v19.4s")         // v5   = b02 b12 b22 b32
            __ASM_EMIT("zip2            v3.4s, v16.4s, v18.4s")         // v3   = b01 b11 b21 b31
            __ASM_EMIT("fmul            v5.4s, v5.4s, v23.4s")          // v5   = b[2]*kf2 = B[2]
            __ASM_EMIT("fmul            v3.4s, v3.4s, v22.4s")          // v3   = b[1]*kf = B[1]

            __ASM_EMIT("zip1            v16.4s, v24.4s, v28.4s")
            __ASM_EMIT("zip1            v18.4s, v26.4s, v30.4s")
            __ASM_EMIT("zip2            v17.4s, v24.4s, v28.4s")
            __ASM_EMIT("zip2            v19.4s, v26.4s, v30.4s")
            __ASM_EMIT("zip1            v24.4s, v16.4s, v18.4s")
            __ASM_EMIT("zip1            v28.4s, v17.4s, v19.4s")
            __ASM_EMIT("zip2            v26.4s, v16.4s, v18.4s")
            __ASM_EMIT("fmul            v28.4s, v28.4s, v23.4s")
            __ASM_EMIT("fmul            v26.4s, v26.4s, v22.4s")

            __ASM_EMIT("zip1            v16.4s, v25.4s, v29.4s")
            __ASM_EMIT("zip1            v18.4s, v27.4s, v31.4s")
            __ASM_EMIT("zip2            v17.4s, v25.4s, v29.4s")
            __ASM_EMIT("zip2            v19.4s, v27.4s, v31.4s")
            __ASM_EMIT("zip1            v25.4s, v16.4s, v18.4s")
            __ASM_EMIT("zip1            v29.4s, v17.4s, v19.4s")
            __ASM_EMIT("zip2            v27.4s, v16.4s, v18.4s")
            __ASM_EMIT("fmul            v29.4s, v29.4s, v23.4s")
            __ASM_EMIT("fmul            v27.4s, v27.4s, v22.4s")
            // v0 = T[0]
            // v2 = T[1]
            // v4 = T[2]
            // v6 = not used
            // q1 = B[0]
            // q3 = B[1]
            // q5 = B[2]
            // q7 = not used
            __ASM_EMIT("fadd            v7.4s, v1.4s, v5.4s")           // v7   = B[0] + B[2]
            __ASM_EMIT("fsub            v6.4s, v0.4s, v2.4s")           // v6   = T[0] - T[1]
            __ASM_EMIT("fsub            v1.4s, v5.4s, v1.4s")           // v1   = B[2] - B[0]
            __ASM_EMIT("fsub            v16.4s, v0.4s, v4.4s")          // v16  = T[0] - T[2]
            __ASM_EMIT("fadd            v17.4s, v7.4s, v3.4s")          // v17  = B[0] + B[1] + B[2] = B
            __ASM_EMIT("fadd            v6.4s, v6.4s, v4.4s")           // v6   = T[0] - T[1] + T[2]
            __ASM_EMIT("frecpe          v18.4s, v17.4s")                // v18  = R, v17 = B
            __ASM_EMIT("fadd            v0.4s, v0.4s, v2.4s")           // v0   = T[0] + T[1]
            __ASM_EMIT("frecps          v19.4s, v18.4s, v17.4s")        // v19  = (2 - R*B)
            __ASM_EMIT("fadd            v16.4s, v16.4s, v16.4s")        // v16  = 2 * (T[0] - T[2])
            __ASM_EMIT("fmul            v18.4s, v19.4s, v18.4s")        // v18  = B' = B * (2 - R*B)
            __ASM_EMIT("fadd            v0.4s, v0.4s, v4.4s")           // v0   = T[0] + T[1] + T[2]
            __ASM_EMIT("frecps          v19.4s, v18.4s, v17.4s")        // v19  = (2 - R*B')
            __ASM_EMIT("fadd            v1.4s, v1.4s, v1.4s")           // v1   = 2 * (B[2] - B[0])
            __ASM_EMIT("fmul            v17.4s, v19.4s, v18.4s")        // v17  = B" = B' * (2 - R*B) = 1/B = N
            __ASM_EMIT("fsub            v3.4s, v3.4s, v7.4s")           // v3   = B[1] - B[0] - B[2]
            __ASM_EMIT("fmul            v0.4s, v0.4s, v17.4s")          // v0   = (T[0] + T[1] + T[2]) * N = A0
            __ASM_EMIT("fmul            v2.4s, v16.4s, v17.4s")         // v2   = 2 * (T[0] - T[2]) * N = A1
            __ASM_EMIT("fmul            v4.4s, v6.4s, v17.4s")          // v4   = (T[0] - T[1] + T[2]) * N = A2
            __ASM_EMIT("fmul            v6.4s, v1.4s, v17.4s")          // v6   = 2 * (B[2] - B[0]) * N = B1
            __ASM_EMIT("fmul            v7.4s, v3.4s, v17.4s")          // v7   = (B[1] - B[0] - B[2]) * N = B2

            __ASM_EMIT("fadd            v31.4s, v25.4s, v29.4s")
            __ASM_EMIT("fsub            v30.4s, v24.4s, v26.4s")
            __ASM_EMIT("fsub            v25.4s, v29.4s, v25.4s")
            __ASM_EMIT("fsub            v16.4s, v24.4s, v28.4s")
            __ASM_EMIT("fadd            v17.4s, v31.4s, v27.4s")
            __ASM_EMIT("fadd            v30.4s, v30.4s, v28.4s")
            __ASM_EMIT("frecpe          v18.4s, v17.4s")
            __ASM_EMIT("fadd            v24.4s, v24.4s, v26.4s")
            __ASM_EMIT("frecps          v19.4s, v18.4s, v17.4s")
            __ASM_EMIT("fadd            v16.4s, v16.4s, v16.4s")
            __ASM_EMIT("fmul            v18.4s, v19.4s, v18.4s")
            __ASM_EMIT("fadd            v24.4s, v24.4s, v28.4s")
            __ASM_EMIT("frecps          v19.4s, v18.4s, v17.4s")
            __ASM_EMIT("fadd            v25.4s, v25.4s, v25.4s")
            __ASM_EMIT("fmul            v17.4s, v19.4s, v18.4s")
            __ASM_EMIT("fsub            v27.4s, v27.4s, v31.4s")
            __ASM_EMIT("fmul            v24.4s, v24.4s, v17.4s")
            __ASM_EMIT("fmul            v26.4s, v16.4s, v17.4s")
            __ASM_EMIT("fmul            v28.4s, v30.4s, v17.4s")
            __ASM_EMIT("fmul            v30.4s, v25.4s, v17.4s")
            __ASM_EMIT("fmul            v31.4s, v27.4s, v17.4s")

            __ASM_EMIT("stp             q0, q24, [%[bf], #0x00]")
            __ASM_EMIT("stp             q2, q26, [%[bf], #0x20]")
            __ASM_EMIT("stp             q4, q28, [%[bf], #0x40]")
            __ASM_EMIT("stp             q6, q30, [%[bf], #0x60]")
            __ASM_EMIT("stp             q7, q31, [%[bf], #0x80]")

            __ASM_EMIT("subs            %[count], %[count], #1")
            __ASM_EMIT("add             %[bc], %[bc], #0x100")
            __ASM_EMIT("add             %[bf], %[bf], #0xa0")
            __ASM_EMIT("b.hs            1b")
            __ASM_EMIT("2:")

            : [bf] "+r" (bf), [bc] "+r" (bc), [count] "+r" (count)
            : [kf] "r" (&kf)
            : "cc", "memory",
              "q0", "q1", "q2", "q3",
              "q4", "q5", "q6", "q7",
              "q16", "q17", "q18", "q19",
              "q20", "q21", "q22", "q23",
              "q24", "q25", "q26", "q27",
              "q28", "q29", "q30", "q31"
        );
    }
}

#endif /* DSP_ARCH_AARCH64_ASIMD_FILTERS_TRANSFORM_H_ */
