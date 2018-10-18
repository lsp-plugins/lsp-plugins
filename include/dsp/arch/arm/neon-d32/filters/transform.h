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
        ARCH_ARM_ASM(

            __ASM_EMIT("vldm            %[bc]!, {q0-q7}")   // {q0, q2, q4, q6} = t[x,0] t[x,1] t[x,2] t[x,3], {q1, q3, q5, q7} = b[x,0] b[x,1] b[x,2] b[x,3]
            // q0 = t[0,0] t[0,1] t[0,2] t[0,3]
            // q1 = b[0,0] b[0,1] b[0,2] b[0,3]
            // q2 = t[1,0] t[1,1] t[1,2] t[1,3]
            // q3 = b[1,0] b[1,1] b[1,2] b[1,3]
            // q4 = t[2,0] t[2,1] t[2,2] t[2,3]
            // q5 = b[2,0] b[2,1] b[2,2] b[2,3]
            // q6 = t[3,0] t[3,1] t[3,2] t[3,3]
            // q7 = b[3,0] b[3,1] b[3,2] b[3,3]
            __ASM_EMIT("vtrn.32         q0, q2")            // q0   = t[0,0] t[1,0] t[0,2] t[1,2], q2 = t[0,1] t[1,1] t[0,3] t[1,3]
            __ASM_EMIT("vtrn.32         q1, q3")            // q1   = b[0,0] b[1,0] b[0,2] b[1,2], q3 = b[0,1] b[1,1] b[0,3] b[1,3]
            __ASM_EMIT("vtrn.32         q4, q6")            // q4   = t[2,0] t[3,0] t[2,2] t[3,2], q6 = t[2,1] t[3,1] t[2,3] t[3,3]
            __ASM_EMIT("vtrn.32         q5, q7")            // q5   = b[2,0] b[3,0] b[2,2] b[3,2], q7 = b[2,1] b[3,1] b[2,3] b[3,3]
            __ASM_EMIT("vswp            d8, d1")
            __ASM_EMIT("vswp            d10, d3")
            __ASM_EMIT("vswp            d12, d5")
            __ASM_EMIT("vswp            d14, d7")
            // q0 = t[0,0] t[1,0] t[2,0] t[3,0]
            // q2 = t[0,1] t[1,1] t[2,1] t[3,1]
            // q4 = t[0,2] t[1,2] t[2,2] t[3,2]
            // q6 = t[0,3] t[1,3] t[2,3] t[3,3]
            // q1 = b[0,0] b[1,0] b[2,0] b[3,0]
            // q3 = b[0,1] b[1,1] b[2,1] b[3,1]
            // q5 = b[0,2] b[1,2] b[2,2] b[3,2]
            // q6 = b[0,3] b[1,3] b[2,3] b[3,3]
        )
    }
}

#endif /* DSP_ARCH_ARM_NEON_D32_FILTERS_TRANSFORM_H_ */
