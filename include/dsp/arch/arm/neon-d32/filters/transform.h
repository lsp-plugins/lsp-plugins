/*
 * transform.h
 *
 *  Created on: 18 окт. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_ARM_NEON_D32_FILTERS_TRANSFORM_H_
#define DSP_ARCH_ARM_NEON_D32_FILTERS_TRANSFORM_H_

#ifndef DSP_ARCH_ARM_NEON_32_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_ARM_NEON_32_IMPL */

//    if (count <= 0)
//        return;
//
//    float T[4], B[4], N;
//    float kf2       = kf * kf;
//
//    while (count--)
//    {
//        // Calculate bottom coefficients
//        B[0]            = bc->b[0];
//        B[1]            = bc->b[1]*kf;
//        B[2]            = bc->b[2]*kf2;
//
//        // Calculate the convolution
//        N               = 1.0 / (B[0] + B[1] + B[2]);
//
//        bf->b[0]        = 2.0 * (B[2] - B[0]) * N;  // Sign negated
//        bf->b[1]        = (B[1] - B[2] - B[0]) * N; // Sign negated
//        bf->b[2]        = 0.0f;
//        bf->b[3]        = 0.0f;
//
//        // Calculate top coefficients
//        T[0]            = bc->t[0];
//        T[1]            = bc->t[1]*kf;
//        T[2]            = bc->t[2]*kf2;
//
//        bf->a[0]        = (T[0] + T[1] + T[2]) * N;
//        bf->a[1]        = bf->a[0];
//        bf->a[2]        = 2.0 * (T[0] - T[2]) * N;
//        bf->a[3]        = (T[0] - T[1] + T[2]) * N;
//
//        // Increment pointers
//        bc              ++;
//        bf              ++;
//    }

namespace neon_d32
{
    void bilinear_transform_x1(biquad_x1_t *bf, const f_cascade_t *bc, float kf, size_t count)
    {
        IF_ARCH_ARM(float xkf = kf);

        ARCH_ARM_ASM
        (
            __ASM_EMIT("vld1.32         {d28[], d29[]}, [%[kf]]") // q14  = kf
            __ASM_EMIT("subs            %[count], $4")
            __ASM_EMIT("vmul.f32        q15, q14, q14")         // q15  = kf*kf = kf2
            __ASM_EMIT("blo             2f")

            // x4 loop
            __ASM_EMIT("1:")
            __ASM_EMIT("vldm            %[bc]!, {q0-q7}")       // {q0, q2, q4, q6} = t[x,0] t[x,1] t[x,2] t[x,3], {q1, q3, q5, q7} = b[x,0] b[x,1] b[x,2] b[x,3]
            // q0 = t[0,0] t[0,1] t[0,2] t[0,3]
            // q2 = t[1,0] t[1,1] t[1,2] t[1,3]
            // q4 = t[2,0] t[2,1] t[2,2] t[2,3]
            // q6 = t[3,0] t[3,1] t[3,2] t[3,3]
            // q1 = b[0,0] b[0,1] b[0,2] b[0,3]
            // q3 = b[1,0] b[1,1] b[1,2] b[1,3]
            // q5 = b[2,0] b[2,1] b[2,2] b[2,3]
            // q7 = b[3,0] b[3,1] b[3,2] b[3,3]
            __ASM_EMIT("vzip.32         q0, q4")                // q0   = t00 t20 t01 t21, q4 = t02 t22 t03 t23
            __ASM_EMIT("vzip.32         q2, q6")                // q2   = t10 t30 t11 t31, q6 = t12 t32 t13 t33
            __ASM_EMIT("vzip.32         q1, q5")                // q1   = b00 b20 b01 b21, q5 = b02 b22 b03 b23
            __ASM_EMIT("vzip.32         q3, q7")                // q3   = b10 b30 b11 b31, q7 = b12 b32 b13 b33
            __ASM_EMIT("vzip.32         q0, q2")                // q0   = t00 t10 t20 t30, q2 = t01 t11 t21 t31
            __ASM_EMIT("vzip.32         q4, q6")                // q4   = t02 t12 t22 t32, q6 = t03 t13 t23 t33
            __ASM_EMIT("vzip.32         q1, q3")                // q1   = b00 b10 b20 b30, q5 = b01 b11 b21 b31
            __ASM_EMIT("vzip.32         q5, q7")                // q3   = b02 b12 b22 b32, q7 = b03 b13 b23 b33
            // q0 = t00 t10 t20 t30         = T[0]
            // q2 = t01 t11 t21 t31         = T[1]
            // q4 = t02 t12 t22 t32         = T[2]
            // q6 = t03 t13 t23 t33         = not used
            // q1 = b00 b10 b20 b30         = B[0]
            // q3 = b01 b11 b21 b31         = B[1]
            // q5 = b02 b12 b22 b32         = B[2]
            // q7 = b03 b13 b23 b33         = not used
            __ASM_EMIT("vmul.f32        q2, q14")               // q2   = t[1]*kf = T[1]
            __ASM_EMIT("vmul.f32        q4, q15")               // q4   = t[2]*kf2 = T[2]
            __ASM_EMIT("vmul.f32        q3, q14")               // q3   = b[1]*kf = B[1]
            __ASM_EMIT("vmul.f32        q5, q15")               // q5   = b[2]*kf2 = B[2]

            __ASM_EMIT("vadd.f32        q7, q1, q5")            // q7   = B[0] + B[2]
            __ASM_EMIT("vsub.f32        q6, q0, q2")            // q6   = T[0] - T[1]
            __ASM_EMIT("vsub.f32        q1, q5, q1")            // q1   = B[2] - B[0]
            __ASM_EMIT("vsub.f32        q8, q0, q4")            // q8   = T[0] - T[2]
            __ASM_EMIT("vadd.f32        q9, q7, q3")            // q9   = B[0] + B[1] + B[2] = B
            __ASM_EMIT("vadd.f32        q6, q6, q4")            // q6   = T[0] - T[1] + T[2]
            __ASM_EMIT("vrecpe.f32      q10, q9")               // q10  = R, q9 = B
            __ASM_EMIT("vadd.f32        q0, q0, q2")            // q0   = T[0] + T[1]
            __ASM_EMIT("vrecps.f32      q11, q10, q9")          // q11  = (2 - R*B)
            __ASM_EMIT("vadd.f32        q8, q8, q8")            // q8   = 2 * (T[0] - T[2])
            __ASM_EMIT("vmul.f32        q10, q11, q10")         // q10  = B' = B * (2 - R*B)
            __ASM_EMIT("vadd.f32        q0, q0, q4")            // q0   = T[0] + T[1] + T[2]
            __ASM_EMIT("vrecps.f32      q11, q10, q9")          // q11  = (2 - R*B')
            __ASM_EMIT("vadd.f32        q1, q1, q1")            // q1   = 2 * (B[2] - B[0])
            __ASM_EMIT("vmul.f32        q9, q11, q10")          // q9   = B" = B' * (2 - R*B) = 1/B = N
            __ASM_EMIT("vsub.f32        q3, q3, q7")            // q3   = B[1] - B[0] - B[2]
            __ASM_EMIT("vmul.f32        q0, q0, q9")            // q0   = (T[0] + T[1] + T[2]) * N = A0
            __ASM_EMIT("vmul.f32        q2, q8, q9")            // q2   = 2 * (T[0] - T[2]) * N = A1
            __ASM_EMIT("vmul.f32        q4, q6, q9")            // q4   = (T[0] - T[1] + T[2]) * N = A2
            __ASM_EMIT("veor            q5, q5")                // q5   = 0
            __ASM_EMIT("vmul.f32        q6, q1, q9")            // q6   = 2 * (B[2] - B[0]) * N = B1
            __ASM_EMIT("veor            q7, q7")                // q7   = 0
            __ASM_EMIT("vmul.f32        q1, q3, q9")            // q1   = (B[1] - B[0] - B[2]) * N = B2
            __ASM_EMIT("veor            q3, q3")                // q3   = 0

            // Transpose back
            __ASM_EMIT("vzip.32         q0, q4")
            __ASM_EMIT("vzip.32         q2, q6")
            __ASM_EMIT("vzip.32         q1, q5")
            __ASM_EMIT("vzip.32         q3, q7")
            __ASM_EMIT("vzip.32         q0, q2")
            __ASM_EMIT("vzip.32         q4, q6")
            __ASM_EMIT("vzip.32         q1, q3")
            __ASM_EMIT("vzip.32         q5, q7")

            __ASM_EMIT("subs            %[count], $4")
            __ASM_EMIT("vstm            %[bf]!, {q0-q7}")
            __ASM_EMIT("bhs             1b")

            __ASM_EMIT("2:")
            __ASM_EMIT("adds            %[count], $3")
            __ASM_EMIT("blt             4f")

            // 1x blocks
            __ASM_EMIT("vmov            q6, q14")               // q6 = kf
            __ASM_EMIT("vmov            q7, q15")               // q7 = kf2
            __ASM_EMIT("3:")
            __ASM_EMIT("vld4.32         {q0-q1}, [%[bc]]!")     // d0 = t[0] b[0] = T[0] B[0], d1 = t[1] b[1], d2 = t[2] b[2], d3 = 0 0
            __ASM_EMIT("vmul.f32        d1, d1, d12")           // d1 = t[1]*kf b[1]*kf = T[1] B[1]
            __ASM_EMIT("vmul.f32        d2, d2, d14")           // d2 = t[2]*kf b[2]*kf = T[2] B[2]

            __ASM_EMIT("vadd.f32        d4, d0, d2")            // d4 = T[0]+T[2], B[0]+B[2]
            __ASM_EMIT("vsub.f32        d5, d0, d2")            // d5 = T[0]-T[2], B[0]-B[2]
            __ASM_EMIT("vadd.f32        d6, d4, d1")            // d6 = T[0]+T[1]+T[2], B[0]+B[1]+B[2]
            __ASM_EMIT("vadd.f32        d5, d5, d5")            // d5 = 2*(T[0]-T[2]), 2*(B[0]-B[2])
            __ASM_EMIT("vsub.f32        d4, d4, d1")            // d4 = T[0]-T[1]+T[2], B[0]-B[1]+B[2]

            __ASM_EMIT("vrecpe.f32      d7, d6")                // d7  = R, d6 = B
            __ASM_EMIT("vrecps.f32      d8, d7, d6")            // d8  = (2 - R*B)
            __ASM_EMIT("vmul.f32        d7, d8, d7")            // d7  = B' = B * (2 - R*B)
            __ASM_EMIT("vrecps.f32      d8, d7, d6")            // d8  = (2 - R*B')
            __ASM_EMIT("vmul.f32        d7, d8, d7")            // d7  = B" = B' * (2 - R*B) = 1/B = N
            __ASM_EMIT("vdup.32         d7, d7[1]")             // d7  = N

            __ASM_EMIT("vmul.f32        d0, d6, d7")            // d0  = (T[0]+T[1]+T[2])*N, (B[0]+B[1]+B[2])*N = A0, 1
            __ASM_EMIT("vmul.f32        d1, d5, d7")            // d1  = 2*(T[0]-T[2])*N, 2*(B[0]-B[2])*N = A1, -B1
            __ASM_EMIT("vmul.f32        d2, d4, d7")            // d2  = (T[0]-T[1]+T[2])*N, (B[0]-B[1]+B[2])*N = A2, -B2
            __ASM_EMIT("vmov            s1, s2")                // d0  = A0 A1
            __ASM_EMIT("vtrn.f32        d1, d2")                // d1  = A1 A2, d2 = -B1, -B2
            __ASM_EMIT("veor            d3, d3")                // d3  = 0 0
            __ASM_EMIT("vneg.f32        d2, d2")                // d2  = B1, B2
            __ASM_EMIT("vext.32         d1, d1, d2, $1")        // d1  = A2 B1
            __ASM_EMIT("vext.32         d2, d2, d3, $1")        // d1  = B2 0

            __ASM_EMIT("subs            %[count], $1")
            __ASM_EMIT("vst1.32         {d0-d3}, [%[bf]]!")
            __ASM_EMIT("bge             3b")

            __ASM_EMIT("4:")
            : [bf] "+r" (bf), [bc] "+r" (bc), [count] "+r" (count)
            : [kf] "r" (&xkf)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    void bilinear_transform_x2(biquad_x2_t *bf, const f_cascade_t *bc, float kf, size_t count)
    {
        IF_ARCH_ARM(float xkf = kf);

        ARCH_ARM_ASM
        (
            __ASM_EMIT("tst             %[count], %[count]")
            __ASM_EMIT("beq             4f")

            __ASM_EMIT("vld1.32         {d28[], d29[]}, [%[kf]]")   // q14  = kf
            __ASM_EMIT("subs            %[count], $2")
            __ASM_EMIT("vmul.f32        q15, q14, q14")             // q15  = kf*kf = kf2
            __ASM_EMIT("blo             2f")

            // 2 x2 blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("vldm            %[bc]!, {q0-q7}")       // {q0, q2, q4, q6} = t[x,0] t[x,1] t[x,2] t[x,3], {q1, q3, q5, q7} = b[x,0] b[x,1] b[x,2] b[x,3]
            __ASM_EMIT("vzip.32         q0, q4")                // q0   = t00 t20 t01 t21, q4 = t02 t22 t03 t23
            __ASM_EMIT("vzip.32         q2, q6")                // q2   = t10 t30 t11 t31, q6 = t12 t32 t13 t33
            __ASM_EMIT("vzip.32         q1, q5")                // q1   = b00 b20 b01 b21, q5 = b02 b22 b03 b23
            __ASM_EMIT("vzip.32         q3, q7")                // q3   = b10 b30 b11 b31, q7 = b12 b32 b13 b33
            __ASM_EMIT("vzip.32         q0, q2")                // q0   = t00 t10 t20 t30, q2 = t01 t11 t21 t31
            __ASM_EMIT("vzip.32         q4, q6")                // q4   = t02 t12 t22 t32, q6 = t03 t13 t23 t33
            __ASM_EMIT("vzip.32         q1, q3")                // q1   = b00 b10 b20 b30, q5 = b01 b11 b21 b31
            __ASM_EMIT("vzip.32         q5, q7")                // q3   = b02 b12 b22 b32, q7 = b03 b13 b23 b33
            // q0 = T[0]
            // q2 = t[1]
            // q4 = t[2]
            // q6 = zero, not used
            // q1 = B[0]
            // q3 = b[1]
            // q5 = b[2]
            // q6 = zero, not used
            __ASM_EMIT("vmul.f32        q2, q14")               // q2   = t[1]*kf = T[1]
            __ASM_EMIT("vmul.f32        q4, q15")               // q4   = t[2]*kf2 = T[2]
            __ASM_EMIT("vmul.f32        q3, q14")               // q3   = b[1]*kf = B[1]
            __ASM_EMIT("vmul.f32        q5, q15")               // q5   = b[2]*kf2 = B[2]

            __ASM_EMIT("vadd.f32        q7, q1, q5")            // q7   = B[0] + B[2]
            __ASM_EMIT("vsub.f32        q6, q0, q2")            // q6   = T[0] - T[1]
            __ASM_EMIT("vsub.f32        q1, q5, q1")            // q1   = B[2] - B[0]
            __ASM_EMIT("vsub.f32        q8, q0, q4")            // q8   = T[0] - T[2]
            __ASM_EMIT("vadd.f32        q9, q7, q3")            // q9   = B[0] + B[1] + B[2] = B
            __ASM_EMIT("vadd.f32        q6, q6, q4")            // q6   = T[0] - T[1] + T[2]
            __ASM_EMIT("vrecpe.f32      q10, q9")               // q10  = R, q9 = B
            __ASM_EMIT("vadd.f32        q0, q0, q2")            // q0   = T[0] + T[1]
            __ASM_EMIT("vrecps.f32      q11, q10, q9")          // q11  = (2 - R*B)
            __ASM_EMIT("vadd.f32        q8, q8, q8")            // q8   = 2 * (T[0] - T[2])
            __ASM_EMIT("vmul.f32        q10, q11, q10")         // q10  = B' = B * (2 - R*B)
            __ASM_EMIT("vadd.f32        q0, q0, q4")            // q0   = T[0] + T[1] + T[2]
            __ASM_EMIT("vrecps.f32      q11, q10, q9")          // q11  = (2 - R*B')
            __ASM_EMIT("vadd.f32        q1, q1, q1")            // q1   = 2 * (B[2] - B[0])
            __ASM_EMIT("vmul.f32        q9, q11, q10")          // q9   = B" = B' * (2 - R*B) = 1/B = N
            __ASM_EMIT("vsub.f32        q3, q3, q7")            // q3   = B[1] - B[0] - B[2]

            __ASM_EMIT("vmul.f32        q4, q1, q9")            // q8   = 2 * (B[2] - B[0]) * N = B1
            __ASM_EMIT("vmul.f32        q2, q3, q9")            // q9   = (B[1] - B[0] - B[2]) * N = B2
            __ASM_EMIT("vmul.f32        q0, q0, q9")            // q0   = (T[0] + T[1] + T[2]) * N = A0
            __ASM_EMIT("vmul.f32        q3, q8, q9")            // q4   = 2 * (T[0] - T[2]) * N = A1
            __ASM_EMIT("vmul.f32        q1, q6, q9")            // q6   = (T[0] - T[1] + T[2]) * N = A2
            __ASM_EMIT("veor            q5, q5, q5")            // q5   = 0

            // Register states:
            // q0  = A00 A01 A02 A03
            // q1  = A20 A21 A22 A23
            // q2  = B10 B11 B12 B13
            // q3  = A10 A11 A12 A13
            // q4  = B20 B21 B22 B23
            // q5  = 0   0   0   0
            __ASM_EMIT("vswp            d1, d6")                // q0  = A00 A01 A10 A11, q3 = A02 A03 A12 A13
            __ASM_EMIT("vswp            d3, d8")                // q1  = A20 A21 B20 B21, q4 = A22 A23 B22 B23
            __ASM_EMIT("vswp            d5, d10")               // q2  = B20 B21 0   0  , q5 = B22 B23 0   0

            __ASM_EMIT("subs            %[count], $2")
            __ASM_EMIT("vstm            %[bf]!, {q0-q5}")
            __ASM_EMIT("bhs             1b")

            __ASM_EMIT("2:")
            __ASM_EMIT("adds            %[count], $1")
            __ASM_EMIT("blt             4f")

            // 1 x2 block
            __ASM_EMIT("vld4.32         {q0-q1}, [%[bc]]!")     // d0 = t[0] b[0] = T[0] B[0], d1 = t[1] b[1], d2 = t[2] b[2], d3 = 0 0
            __ASM_EMIT("vmul.f32        d1, d1, d28")           // d1 = t[1]*kf b[1]*kf = T[1] B[1]
            __ASM_EMIT("vmul.f32        d2, d2, d30")           // d2 = t[2]*kf b[2]*kf = T[2] B[2]
            __ASM_EMIT("vld4.32         {q8-q9}, [%[bc]]!")
            __ASM_EMIT("vmul.f32        d17, d17, d28")
            __ASM_EMIT("vmul.f32        d18, d18, d30")

            __ASM_EMIT("vadd.f32        d4, d0, d2")            // d4 = T[0]+T[2], B[0]+B[2]
            __ASM_EMIT("vadd.f32        d20, d16, d18")
            __ASM_EMIT("vsub.f32        d5, d0, d2")            // d5 = T[0]-T[2], B[0]-B[2]
            __ASM_EMIT("vsub.f32        d21, d16, d18")
            __ASM_EMIT("vadd.f32        d6, d4, d1")            // d6 = T[0]+T[1]+T[2], B[0]+B[1]+B[2]
            __ASM_EMIT("vadd.f32        d22, d20, d17")
            __ASM_EMIT("vadd.f32        d5, d5, d5")            // d5 = 2*(T[0]-T[2]), 2*(B[0]-B[2])
            __ASM_EMIT("vadd.f32        d21, d21, d21")
            __ASM_EMIT("vsub.f32        d4, d4, d1")            // d4 = T[0]-T[1]+T[2], B[0]-B[1]+B[2]
            __ASM_EMIT("vsub.f32        d20, d20, d17")

            __ASM_EMIT("vrecpe.f32      d7, d6")                // d7  = R, d6 = B
            __ASM_EMIT("vrecpe.f32      d23, d22")
            __ASM_EMIT("vrecps.f32      d8, d7, d6")            // d8  = (2 - R*B)
            __ASM_EMIT("vrecps.f32      d24, d23, d22")
            __ASM_EMIT("vmul.f32        d7, d8, d7")            // d7  = B' = B * (2 - R*B)
            __ASM_EMIT("vmul.f32        d23, d24, d23")
            __ASM_EMIT("vrecps.f32      d8, d7, d6")            // d8  = (2 - R*B')
            __ASM_EMIT("vrecps.f32      d24, d23, d22")
            __ASM_EMIT("vmul.f32        d7, d8, d7")            // d7  = B" = B' * (2 - R*B) = 1/B = N
            __ASM_EMIT("vmul.f32        d23, d24, d23")
            __ASM_EMIT("vdup.32         d7, d7[1]")             // d7  = N
            __ASM_EMIT("vdup.32         d23, d23[1]")

            __ASM_EMIT("vmul.f32        d0, d6, d7")            // d0  = (T[0]+T[1]+T[2])*N, (B[0]+B[1]+B[2])*N = A0, 1
            __ASM_EMIT("vmul.f32        d1, d22, d23")
            __ASM_EMIT("vmul.f32        d2, d5, d7")            // d2  = 2*(T[0]-T[2])*N, 2*(B[0]-B[2])*N = A1, -B1
            __ASM_EMIT("vmul.f32        d3, d21, d23")
            __ASM_EMIT("vmul.f32        d4, d4, d7")            // d4  = (T[0]-T[1]+T[2])*N, (B[0]-B[1]+B[2])*N = A2, -B2
            __ASM_EMIT("vmul.f32        d5, d20, d23")

            __ASM_EMIT("vtrn.f32        d0, d1")                // d0  = A00 A01, d1 = 1 1
            __ASM_EMIT("vtrn.f32        d2, d3")                // d2  = A10 A11, d3 = -B10 -B11
            __ASM_EMIT("vtrn.f32        d4, d5")                // d4  = A20 A01, d5 = -B20 -B21
            __ASM_EMIT("veor            d6, d6")                // d6  = 0 0
            __ASM_EMIT("vswp            d0, d1")                // d0  = 1 1, d1 = A00 A01
            __ASM_EMIT("vswp            d3, d4")                // d3  = A20 A01, d4 = -B10 -B11
            __ASM_EMIT("vneg.f32        d5, d5")                // d5  = B20 B21
            __ASM_EMIT("vneg.f32        d4, d4")                // d4  = B10 B11

            __ASM_EMIT("vstm            %[bf]!, {d1-d6}")

            __ASM_EMIT("4:")
            : [bf] "+r" (bf), [bc] "+r" (bc), [count] "+r" (count)
            : [kf] "r" (&xkf)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    void bilinear_transform_x4(biquad_x4_t *bf, const f_cascade_t *bc, float kf, size_t count)
    {
        IF_ARCH_ARM(float xkf = kf);

        ARCH_ARM_ASM
        (
            __ASM_EMIT("subs            %[count], $1")
            __ASM_EMIT("blo             2f")

            __ASM_EMIT("vld1.32         {d28[], d29[]}, [%[kf]]")   // q14  = kf
            __ASM_EMIT("vmul.f32        q15, q14, q14")             // q15  = kf*kf = kf2

            // 1 x4 blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("vldm            %[bc]!, {q0-q7}")       // {q0, q2, q4, q6} = t[x,0] t[x,1] t[x,2] t[x,3], {q1, q3, q5, q7} = b[x,0] b[x,1] b[x,2] b[x,3]
            __ASM_EMIT("vtrn.32         q0, q2")
            __ASM_EMIT("vtrn.32         q1, q3")
            __ASM_EMIT("vtrn.32         q4, q6")
            __ASM_EMIT("vtrn.32         q5, q7")
            __ASM_EMIT("vswp            d8, d1")
            __ASM_EMIT("vswp            d10, d3")
            __ASM_EMIT("vswp            d12, d5")
            __ASM_EMIT("vswp            d14, d7")
            __ASM_EMIT("vmul.f32        q2, q14")               // q2   = t[1]*kf = T[1]
            __ASM_EMIT("vmul.f32        q4, q15")               // q4   = t[2]*kf2 = T[2]
            __ASM_EMIT("vmul.f32        q3, q14")               // q3   = b[1]*kf = B[1]
            __ASM_EMIT("vmul.f32        q5, q15")               // q5   = b[2]*kf2 = B[2]
            // q0 = T[0]
            // q1 = B[0]
            // q2 = T[1]
            // q3 = B[1]
            // q4 = T[2]
            // q5 = B[2]
            // q6 = zero, not used
            // q7 = zero, not used
            __ASM_EMIT("vadd.f32        q6, q0, q4")            // q6   = T[0] + T[2]
            __ASM_EMIT("vadd.f32        q7, q1, q5")            // q7   = B[0] + B[2]
            __ASM_EMIT("vsub.f32        q8, q0, q4")            // q8   = T[0] - T[2]
            __ASM_EMIT("vsub.f32        q9, q5, q1")            // q9   = B[2] - T[0]
            __ASM_EMIT("vsub.f32        q4, q3, q7")            // q4   = B[1] - B[2] - B[0]
            __ASM_EMIT("vadd.f32        q0, q6, q2")            // q0   = T[0] + T[1] + T[2]
            __ASM_EMIT("vadd.f32        q7, q3, q7")            // q7   = B[0] + B[1] + B[2]
            __ASM_EMIT("vsub.f32        q2, q6, q2")            // q2   = T[0] - T[1] + T[2]
            __ASM_EMIT("vrecpe.f32      q10, q7")               // q10  = R, q7 = B
            __ASM_EMIT("vadd.f32        q1, q8, q8")            // q1   = 2*(T[0] - T[2])
            __ASM_EMIT("vrecps.f32      q11, q10, q7")          // q11  = (2 - R*B)
            __ASM_EMIT("vadd.f32        q3, q9, q9")            // q3   = 2*(B[2] - T[0])
            __ASM_EMIT("vmul.f32        q10, q11, q10")         // q10  = B' = B * (2 - R*B)
            __ASM_EMIT("vrecps.f32      q11, q10, q7")          // q11  = (2 - R*B')
            __ASM_EMIT("vmul.f32        q7, q11, q10")          // q9   = B" = B' * (2 - R*B) = 1/B = N

            __ASM_EMIT("vmul.f32        q0, q0, q7")            // q0   = (T[0] + T[1] + T[2]) * N = A0
            __ASM_EMIT("vmul.f32        q1, q1, q7")            // q1   = 2*(T[0] - T[2]) * N = A1
            __ASM_EMIT("vmul.f32        q2, q2, q7")            // q2   = (T[0] - T[1] + T[2]) * N = A2
            __ASM_EMIT("vmul.f32        q3, q3, q7")            // q3   = 2*(B[2] - T[0]) = B1
            __ASM_EMIT("vmul.f32        q4, q4, q7")            // q4   = (B[1] - B[2] - B[0]) * N = B2

            __ASM_EMIT("subs            %[count], $1")
            __ASM_EMIT("vstm            %[bf]!, {q0-q4}")
            __ASM_EMIT("bhs             1b")

            __ASM_EMIT("2:")
            : [bf] "+r" (bf), [bc] "+r" (bc), [count] "+r" (count)
            : [kf] "r" (&xkf)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    void bilinear_transform_x8(biquad_x8_t *bf, const f_cascade_t *bc, float kf, size_t count)
    {
        IF_ARCH_ARM(
                float xkf = kf;
                float KF[8] __lsp_aligned16;
            );

        ARCH_ARM_ASM
        (
            __ASM_EMIT("subs            %[count], $1")
            __ASM_EMIT("blo             2f")

            __ASM_EMIT("vld1.32         {d28[], d29[]}, [%[kf]]")   // q14  = kf
            __ASM_EMIT("vmul.f32        q15, q14, q14")             // q15  = kf*kf = kf2
            __ASM_EMIT("vstm            %[KF], {q14-q15}")

            // 1 x8 blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("vldm            %[bc]!, {q0-q7}")
            __ASM_EMIT("vtrn.32         q0, q2")
            __ASM_EMIT("vtrn.32         q1, q3")
            __ASM_EMIT("vtrn.32         q4, q6")
            __ASM_EMIT("vtrn.32         q5, q7")
            __ASM_EMIT("vswp            d8, d1")
            __ASM_EMIT("vswp            d10, d3")
            __ASM_EMIT("vswp            d12, d5")
            __ASM_EMIT("vswp            d14, d7")

            __ASM_EMIT("vldm            %[bc]!, {q8-q15}")
            __ASM_EMIT("vtrn.32         q8, q10")
            __ASM_EMIT("vtrn.32         q9, q11")
            __ASM_EMIT("vtrn.32         q12, q14")
            __ASM_EMIT("vtrn.32         q13, q15")
            __ASM_EMIT("vswp            d24, d17")
            __ASM_EMIT("vswp            d26, d19")
            __ASM_EMIT("vswp            d28, d21")
            __ASM_EMIT("vswp            d30, d23")

            __ASM_EMIT("vldm            %[KF], {q14-q15}")
            __ASM_EMIT("vmul.f32        q2, q14")               // q2   = t[1]*kf  = T[1]
            __ASM_EMIT("vmul.f32        q4, q15")               // q4   = t[2]*kf2 = T[2]
            __ASM_EMIT("vmul.f32        q3, q14")               // q3   = b[1]*kf  = B[1]
            __ASM_EMIT("vmul.f32        q5, q15")               // q5   = b[2]*kf2 = B[2]
            __ASM_EMIT("vmul.f32        q10, q14")              // q10  = v[1]*kf  = V[1]
            __ASM_EMIT("vmul.f32        q12, q15")              // q12  = v[2]*kf2 = V[2]
            __ASM_EMIT("vmul.f32        q11, q14")              // q11  = w[1]*kf  = W[1]
            __ASM_EMIT("vmul.f32        q13, q15")              // q13  = w[2]*kf2 = W[2]

            // q0  = T[0]   q8  =  V[0]
            // q1  = B[0]   q9  =  W[0]
            // q2  = T[1]   q10 =  V[1]
            // q3  = B[1]   q11 =  W[1]
            // q4  = T[2]   q12 =  V[2]
            // q5  = B[2]   q13 =  W[2]
            // q6  = 0      q14 =  kf
            // q7  = 0      q15 =  kf2
            __ASM_EMIT("vsub.f32        q6, q5, q1")            // q6   = B[2] - B[0]
            __ASM_EMIT("vadd.f32        q7, q5, q1")            // q7   = B[2] + B[0]
            __ASM_EMIT("vadd.f32        q6, q6, q6")            // q6   = 2*(B[2] - B[0])
            __ASM_EMIT("vadd.f32        q1, q3, q7")            // q1   = B[0] + B[1] + B[2]
            __ASM_EMIT("vsub.f32        q5, q3, q7")            // q5   = B[1] - B[2] - B[0]
            __ASM_EMIT("vrecpe.f32      q14, q1")               // q14  = R, q7 = B0
            __ASM_EMIT("vrecps.f32      q15, q14, q1")          // q15  = (2 - R*B0)
            __ASM_EMIT("vmul.f32        q14, q15, q14")         // q14  = B0' = B0 * (2 - R*B0)
            __ASM_EMIT("vrecps.f32      q15, q14, q1")          // q15  = (2 - R*B0')
            __ASM_EMIT("vmul.f32        q1, q15, q14")          // q1   = B0" = B0' * (2 - R*B0) = 1/B0 = N
            __ASM_EMIT("vmul.f32        q5, q5, q1")            // q5   = (B[1] - B[2] - B[0]) * N = B2
            __ASM_EMIT("vmul.f32        q6, q6, q1")            // q6   = 2*(B[2] - B[0]) * N = B1

            __ASM_EMIT("vsub.f32        q7, q13, q9")           // q7   = W[2] - W[0]
            __ASM_EMIT("vadd.f32        q3, q13, q9")           // q3   = W[2] + W[0]
            __ASM_EMIT("vadd.f32        q7, q7, q7")            // q7   = 2*(W[2] - W[0])
            __ASM_EMIT("vsub.f32        q9, q11, q3")           // q9   = W[1] - W[2] - W[0]
            __ASM_EMIT("vadd.f32        q3, q11, q3")           // q1   = W[0] + W[1] + W[2]
            __ASM_EMIT("vrecpe.f32      q14, q3")               // q14  = R, q7 = W0
            __ASM_EMIT("vrecps.f32      q15, q14, q3")          // q15  = (2 - R*W0)
            __ASM_EMIT("vmul.f32        q14, q15, q14")         // q14  = W0' = W0 * (2 - R*W0)
            __ASM_EMIT("vrecps.f32      q15, q14, q3")          // q15  = (2 - R*W0')
            __ASM_EMIT("vmul.f32        q3, q15, q14")          // q3   = W0" = W0' * (2 - R*W0) = 1/W0 = M
            __ASM_EMIT("vmul.f32        q7, q7, q3")            // q7   = 2*(W[2] - W[0])*M = W1
            __ASM_EMIT("vmul.f32        q9, q9, q3")            // q9   = (W[1] - W[2] - W[0])*M = W2

            // q0  = T[0]   q8  =  V[0]
            // q1  = N      q9  =  W2
            // q2  = T[1]   q10 =  V[1]
            // q3  = M      q11 =  ?
            // q4  = T[2]   q12 =  V[2]
            // q5  = B2     q13 =  ?
            // q6  = B1     q14 =  ?
            // q7  = W1     q15 =  ?
            __ASM_EMIT("vadd.f32        q11, q0, q4")           // q11  = T[0] + T[2]
            __ASM_EMIT("vadd.f32        q14, q8, q12")          // q14  = V[0] + V[2]
            __ASM_EMIT("vsub.f32        q13, q0, q4")           // q13  = T[0] - T[2]
            __ASM_EMIT("vsub.f32        q15, q8, q12")          // q15  = V[0] - V[2]
            __ASM_EMIT("vadd.f32        q0, q11, q2")           // q0   = T[0] + T[1] + T[2]
            __ASM_EMIT("vadd.f32        q15, q15, q15")         // q15  = 2*(V[0] - V[2])
            __ASM_EMIT("vsub.f32        q4, q11, q2")           // q4   = T[0] - T[1] + T[2]
            __ASM_EMIT("vadd.f32        q2, q13, q13")          // q2   = 2*(T[0] - T[2])
            __ASM_EMIT("vadd.f32        q12, q14, q10")         // q12  = V[0] + V[1] + V[2]
            __ASM_EMIT("vsub.f32        q10, q14, q10")         // q10  = V[0] - V[1] + V[2]
            __ASM_EMIT("vmul.f32        q0, q0, q1")            // q0   = (T[0] + T[1] + T[2])*N = A0
            __ASM_EMIT("vmul.f32        q2, q2, q1")            // q2   = 2*(T[0] - T[2])*N = A1
            __ASM_EMIT("vmul.f32        q4, q4, q1")            // q4   = (T[0] - T[1] + T[2])*N = A2
            __ASM_EMIT("vmul.f32        q1, q12, q3")           // q1   = (V[0] + V[1] + V[2])*M = V0
            __ASM_EMIT("vmov.f32        q8, q5")                // q8   = B2
            __ASM_EMIT("vmul.f32        q5, q10, q3")           // q5   = (V[0] - V[1] + V[2])*M = V2
            __ASM_EMIT("vmul.f32        q3, q15, q3")           // q3   = 2*(V[0] - V[2])*M = V1

            // q0  = A0     q8  =  B2
            // q1  = V0     q9  =  W2
            // q2  = A1     q10 =  ?
            // q3  = V1     q11 =  ?
            // q4  = A2     q12 =  ?
            // q5  = V2     q13 =  ?
            // q6  = B1     q14 =  ?
            // q7  = W1     q15 =  ?
            __ASM_EMIT("vstm            %[bf]!, {q0-q5}")
            __ASM_EMIT("subs            %[count], $1")
            __ASM_EMIT("vstm            %[bf]!, {q6-q9}")
            __ASM_EMIT("bhs             1b")

            __ASM_EMIT("2:")
            : [bf] "+r" (bf), [bc] "+r" (bc), [count] "+r" (count)
            : [kf] "r" (&xkf), [KF] "r" (&KF[0])
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }
}

#endif /* DSP_ARCH_ARM_NEON_D32_FILTERS_TRANSFORM_H_ */
