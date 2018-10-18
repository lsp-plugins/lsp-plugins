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
            // q1 = b[0,0] b[0,1] b[0,2] b[0,3]
            // q2 = t[1,0] t[1,1] t[1,2] t[1,3]
            // q3 = b[1,0] b[1,1] b[1,2] b[1,3]
            // q4 = t[2,0] t[2,1] t[2,2] t[2,3]
            // q5 = b[2,0] b[2,1] b[2,2] b[2,3]
            // q6 = t[3,0] t[3,1] t[3,2] t[3,3]
            // q7 = b[3,0] b[3,1] b[3,2] b[3,3]
            __ASM_EMIT("vtrn.32         q0, q2")                // q0   = t[0,0] t[1,0] t[0,2] t[1,2], q2 = t[0,1] t[1,1] t[0,3] t[1,3]
            __ASM_EMIT("vtrn.32         q1, q3")                // q1   = b[0,0] b[1,0] b[0,2] b[1,2], q3 = b[0,1] b[1,1] b[0,3] b[1,3]
            __ASM_EMIT("vtrn.32         q4, q6")                // q4   = t[2,0] t[3,0] t[2,2] t[3,2], q6 = t[2,1] t[3,1] t[2,3] t[3,3]
            __ASM_EMIT("vtrn.32         q5, q7")                // q5   = b[2,0] b[3,0] b[2,2] b[3,2], q7 = b[2,1] b[3,1] b[2,3] b[3,3]
            __ASM_EMIT("vswp            d8, d1")
            __ASM_EMIT("vswp            d10, d3")
            __ASM_EMIT("vswp            d12, d5")
            __ASM_EMIT("vswp            d14, d7")
            // q0 = t[0,0] t[1,0] t[2,0] t[3,0] = t[0] = T[0]
            // q2 = t[0,1] t[1,1] t[2,1] t[3,1] = t[1]
            // q4 = t[0,2] t[1,2] t[2,2] t[3,2] = t[2]
            // q6 = t[0,3] t[1,3] t[2,3] t[3,3]                 -- not used
            // q1 = b[0,0] b[1,0] b[2,0] b[3,0] = b[0] = B[0]
            // q3 = b[0,1] b[1,1] b[2,1] b[3,1] = b[1]
            // q5 = b[0,2] b[1,2] b[2,2] b[3,2] = b[2]
            // q6 = b[0,3] b[1,3] b[2,3] b[3,3]                 -- not used
            __ASM_EMIT("vmul.f32        q2, q14")               // q2   = t[1]*kf = T[1]
            __ASM_EMIT("vmul.f32        q4, q15")               // q4   = t[2]*kf2 = T[2]
            __ASM_EMIT("vmul.f32        q3, q14")               // q3   = b[1]*kf = B[1]
            __ASM_EMIT("vmul.f32        q5, q15")               // q5   = b[2]*kf2 = B[2]

            __ASM_EMIT("vadd.f32        q7, q1, q5")            // q7   = B[0] + B[2]
            __ASM_EMIT("vsub.f32        q6, q0, q2")            // q6   = T[0] - T[1]
            __ASM_EMIT("vsub.f32        q1, q5, q1")            // q1   = B[2] - B[0]
            __ASM_EMIT("vsub.f32        q8, q0, q4")            // q8   = T[0] - T[2]
            __ASM_EMIT("vadd.f32        q9, q9, q3")            // q9   = B[0] + B[1] + B[2] = B
            __ASM_EMIT("vadd.f32        q6, q6, q4")            // q6   = T[0] - T[1] + T[2]
            __ASM_EMIT("vrecpe.f32      q10, q9")               // q10  = R, q9 = B
            __ASM_EMIT("vadd.f32        q1, q1, q1")            // q1   = 2 * (B[2] - B[0])
            __ASM_EMIT("vrecps.f32      q11, q10, q9")          // q11  = (2 - R*B)
            __ASM_EMIT("vadd.f32        q0, q0, q2")            // q0   = T[0] + T[1]
            __ASM_EMIT("vmul.f32        q9, q11, q10")          // q9   = B' = B * (2 - R*B)
            __ASM_EMIT("vsub.f32        q3, q3, q7")            // q3   = B[1] - B[0] - B[2]
            __ASM_EMIT("vrecps.f32      q11, q10, q9")          // q11  = (2 - R*B')
            __ASM_EMIT("vadd.f32        q8, q8, q8")            // q8   = 2 * (T[0] - T[2])
            __ASM_EMIT("vmul.f32        q9, q11, q10")          // q9   = B" = B' * (2 - R*B) = 1/B = N
            __ASM_EMIT("vadd.f32        q0, q0, q4")            // q0   = T[0] + T[1] + T[2]
            __ASM_EMIT("veor            q5, q5")                // q5   = 0
            __ASM_EMIT("veor            q7, q7")                // q7   = 0
            __ASM_EMIT("vmul.f32        q0, q0, q9")            // q0   = (T[0] + T[1] + T[2]) * N = A0
            __ASM_EMIT("vmul.f32        q4, q8, q9")            // q4   = 2 * (T[0] - T[2]) * N = A1
            __ASM_EMIT("vmov            q2, q0")                // q2   = A0
            __ASM_EMIT("vmul.f32        q6, q6, q9")            // q6   = (T[0] - T[1] + T[2]) * N = A2
            __ASM_EMIT("vmul.f32        q1, q1, q9")            // q1   = 2 * (B[2] - B[0]) * N = B1
            __ASM_EMIT("vmul.f32        q3, q3, q9")            // q3   = (B[1] - B[0] - B[2]) * N = B2

            // Transpose back
            __ASM_EMIT("vtrn.32         q0, q2")
            __ASM_EMIT("vtrn.32         q1, q3")
            __ASM_EMIT("vtrn.32         q4, q6")
            __ASM_EMIT("vtrn.32         q5, q7")
            __ASM_EMIT("vswp            d8, d1")
            __ASM_EMIT("vswp            d10, d3")
            __ASM_EMIT("vswp            d12, d5")
            __ASM_EMIT("vswp            d14, d7")

            __ASM_EMIT("subs            %[count], $4")
            __ASM_EMIT("vstm            %[bf]!, {q0-q7}")
            __ASM_EMIT("bhs             1b")

            __ASM_EMIT("2:")
            __ASM_EMIT("add             %[count], $3")
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
            __ASM_EMIT("vmul.f32        d6, d8, d7")            // d6  = B' = B * (2 - R*B)
            __ASM_EMIT("vrecps.f32      d8, d7, d6")            // d8  = (2 - R*B')
            __ASM_EMIT("vmul.f32        d7, d8, d7")            // d7  = B" = B' * (2 - R*B) = 1/B = N
            __ASM_EMIT("vdup.32         d7, d7[1]")             // d7  = N

            __ASM_EMIT("vmul.f32        d0, d6, d7")            // d0  = (T[0]+T[1]+T[2])*N, (B[0]+B[1]+B[2])*N = A0, 1
            __ASM_EMIT("vmul.f32        d1, d5, d7")            // d1  = 2*(T[0]-T[2])*N, 2*(B[0]-B[2])*N = A1, -B1
            __ASM_EMIT("vmul.f32        d2, d4, d7")            // d2  = (T[0]-T[1]+T[2])*N, (B[0]-B[1]+B[2])*N = A2, -B2
            __ASM_EMIT("vtrn.f32        d1, d2")                // d1  = A1 A2, d2 = -B1, -B2
            __ASM_EMIT("vdup.32         d0, d0[0]")             // d0  = A0 A0
            __ASM_EMIT("veor            d3, d3")                // d3  = 0 0
            __ASM_EMIT("vneg.f32        d2, d2")                // d2  = B1, B2

            __ASM_EMIT("subs            %[count], $1")
            __ASM_EMIT("vst1.32         {q0-q1}, [%[bf]]!")
            __ASM_EMIT("bge             3b")

            __ASM_EMIT("4:")
            : [bf] "+r" (bf), [bc] "+r" (bc), [count] "+r" (count)
            : [kf] "r" (&kf)
            : "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }
}

#endif /* DSP_ARCH_ARM_NEON_D32_FILTERS_TRANSFORM_H_ */
