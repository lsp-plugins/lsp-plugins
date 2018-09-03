/*
 * complex.h
 *
 *  Created on: 2 сент. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_ARM_NEON_D32_COMPLEX_H_
#define DSP_ARCH_ARM_NEON_D32_COMPLEX_H_

#ifndef DSP_ARCH_ARM_NEON_32_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_ARM_NEON_32_IMPL */

namespace neon_d32
{
    void complex_mul2(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t count)
    {
        ARCH_ARM_ASM
        (
            __ASM_EMIT("subs        %[count], $8")
            __ASM_EMIT("blo         2f")

            // x8 blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("vld1.32     {d0-d3}, [%[src_re]]!")         // q0, q1 = sr
            __ASM_EMIT("vld1.32     {d4-d7}, [%[src_im]]!")         // q2, q3 = si
            __ASM_EMIT("vld1.32     {d8-d11}, [%[dst_re]]")         // q4, q5 = dr
            __ASM_EMIT("vld1.32     {d12-d15}, [%[dst_im]]")        // q6, q7 = di
            __ASM_EMIT("vmul.f32    q8, q0, q4")                    // q8 = sr*dr
            __ASM_EMIT("vmul.f32    q9, q1, q5")
            __ASM_EMIT("vmul.f32    q10, q2, q4")                   // q10 = si * dr
            __ASM_EMIT("vmul.f32    q11, q3, q5")
            __ASM_EMIT("vmls.f32    q8, q2, q6")                    // q8 = sr*dr - si*di
            __ASM_EMIT("vmls.f32    q9, q3, q7")
            __ASM_EMIT("vmla.f32    q10, q0, q6")                   // q10 = si*dr + sr*di
            __ASM_EMIT("vmla.f32    q11, q1, q7")
            __ASM_EMIT("vst1.32     {d16-d19}, [%[dst_re]]!")
            __ASM_EMIT("vst1.32     {d20-d23}, [%[dst_im]]!")
            __ASM_EMIT("subs        %[count], $8")
            __ASM_EMIT("bhs         1b")

            // x4 blocks
            __ASM_EMIT("2:")
            __ASM_EMIT("adds        %[count], $4")
            __ASM_EMIT("blt         4f")
            __ASM_EMIT("vld1.32     {d0-d1}, [%[src_re]]!")         // q0 = sr
            __ASM_EMIT("vld1.32     {d4-d5}, [%[src_im]]!")         // q2 = si
            __ASM_EMIT("vld1.32     {d8-d9}, [%[dst_re]]")          // q4 = dr
            __ASM_EMIT("vld1.32     {d12-d13}, [%[dst_im]]")        // q6 = di
            __ASM_EMIT("vmul.f32    q8, q0, q4")                    // q8 = sr*dr
            __ASM_EMIT("vmul.f32    q10, q2, q4")                   // q10 = si * dr
            __ASM_EMIT("vmls.f32    q8, q2, q6")                    // q8 = sr*dr - si*di
            __ASM_EMIT("vmla.f32    q10, q0, q6")                   // q10 = si*dr + sr*di
            __ASM_EMIT("vst1.32     {d16-d17}, [%[dst_re]]!")
            __ASM_EMIT("vst1.32     {d20-d21}, [%[dst_im]]!")
            __ASM_EMIT("sub         %[count], $4")

            // x1 blocks
            __ASM_EMIT("4:")
            __ASM_EMIT("adds        %[count], $3")
            __ASM_EMIT("blt         6f")
            __ASM_EMIT("5:")
            __ASM_EMIT("vldr        s0, [%[src_re]]")               // s0 = sr
            __ASM_EMIT("vldr        s1, [%[src_im]]")               // s1 = si
            __ASM_EMIT("vldr        s2, [%[dst_re]]")               // s2 = dr
            __ASM_EMIT("vldr        s3, [%[dst_im]]")               // s3 = di
            __ASM_EMIT("vmul.f32    s4, s0, s2")                    // s4 = sr * dr
            __ASM_EMIT("vmul.f32    s5, s1, s2")                    // s5 = si * dr
            __ASM_EMIT("vmls.f32    s4, s1, s3")                    // s4 = sr*dr - si*di
            __ASM_EMIT("vmla.f32    s5, s0, s3")                    // q10 = si * dr + sr * di
            __ASM_EMIT("vstr        s4, [%[dst_re]]")
            __ASM_EMIT("vstr        s5, [%[dst_im]]")
            __ASM_EMIT("add         %[src_re], $4")
            __ASM_EMIT("add         %[src_im], $4")
            __ASM_EMIT("add         %[dst_re], $4")
            __ASM_EMIT("add         %[dst_im], $4")
            __ASM_EMIT("subs        %[count], $1")
            __ASM_EMIT("bge         5b")

            __ASM_EMIT("6:")

            : [dst_re] "+r" (dst_re), [dst_im] "+r" (dst_im),
              [src_re] "+r" (src_re), [src_im] "+r" (src_im),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11"
        );
    }

    void complex_mul3(float *dst_re, float *dst_im, const float *src1_re, const float *src1_im, const float *src2_re, const float *src2_im, size_t count)
    {
        ARCH_ARM_ASM
        (
            __ASM_EMIT("subs        %[count], $8")
            __ASM_EMIT("blo         2f")

            // x8 blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("vld1.32     {d0-d3}, [%[src1_re]]!")        // q0, q1 = sr
            __ASM_EMIT("vld1.32     {d4-d7}, [%[src1_im]]!")        // q2, q3 = si
            __ASM_EMIT("vld1.32     {d8-d11}, [%[src2_re]]!")       // q4, q5 = dr
            __ASM_EMIT("vld1.32     {d12-d15}, [%[src2_im]]!")      // q6, q7 = di
            __ASM_EMIT("vmul.f32    q8, q0, q4")                    // q8 = sr*dr
            __ASM_EMIT("vmul.f32    q9, q1, q5")
            __ASM_EMIT("vmul.f32    q10, q2, q4")                   // q10 = si * dr
            __ASM_EMIT("vmul.f32    q11, q3, q5")
            __ASM_EMIT("vmls.f32    q8, q2, q6")                    // q8 = sr*dr - si*di
            __ASM_EMIT("vmls.f32    q9, q3, q7")
            __ASM_EMIT("vmla.f32    q10, q0, q6")                   // q10 = si*dr + sr*di
            __ASM_EMIT("vmla.f32    q11, q1, q7")
            __ASM_EMIT("vst1.32     {d16-d19}, [%[dst_re]]!")
            __ASM_EMIT("vst1.32     {d20-d23}, [%[dst_im]]!")
            __ASM_EMIT("subs        %[count], $8")
            __ASM_EMIT("bhs         1b")

            // x4 blocks
            __ASM_EMIT("2:")
            __ASM_EMIT("adds        %[count], $4")
            __ASM_EMIT("blt         4f")
            __ASM_EMIT("vld1.32     {d0-d1}, [%[src1_re]]!")        // q0 = sr
            __ASM_EMIT("vld1.32     {d4-d5}, [%[src1_im]]!")        // q2 = si
            __ASM_EMIT("vld1.32     {d8-d9}, [%[src2_re]]!")        // q4 = dr
            __ASM_EMIT("vld1.32     {d12-d13}, [%[src2_im]]!")      // q6 = di
            __ASM_EMIT("vmul.f32    q8, q0, q4")                    // q8 = sr*dr
            __ASM_EMIT("vmul.f32    q10, q2, q4")                   // q10 = si * dr
            __ASM_EMIT("vmls.f32    q8, q2, q6")                    // q8 = sr*dr - si*di
            __ASM_EMIT("vmla.f32    q10, q0, q6")                   // q10 = si*dr + sr*di
            __ASM_EMIT("vst1.32     {d16-d17}, [%[dst_re]]!")
            __ASM_EMIT("vst1.32     {d20-d21}, [%[dst_im]]!")
            __ASM_EMIT("sub         %[count], $4")

            // x1 blocks
            __ASM_EMIT("4:")
            __ASM_EMIT("adds        %[count], $3")
            __ASM_EMIT("blt         6f")
            __ASM_EMIT("5:")
            __ASM_EMIT("vldr        s0, [%[src1_re]]")              // s0 = sr
            __ASM_EMIT("vldr        s1, [%[src1_im]]")              // s1 = si
            __ASM_EMIT("vldr        s2, [%[src2_re]]")              // s2 = dr
            __ASM_EMIT("vldr        s3, [%[src2_im]]")              // s3 = di
            __ASM_EMIT("vmul.f32    s4, s0, s2")                    // s4 = sr * dr
            __ASM_EMIT("vmul.f32    s5, s1, s2")                    // s5 = si * dr
            __ASM_EMIT("vmls.f32    s4, s1, s3")                    // s4 = sr*dr - si*di
            __ASM_EMIT("vmla.f32    s5, s0, s3")                    // q10 = si * dr + sr * di
            __ASM_EMIT("vstr        s4, [%[dst_re]]")
            __ASM_EMIT("vstr        s5, [%[dst_im]]")
            __ASM_EMIT("add         %[src1_re], $4")
            __ASM_EMIT("add         %[src1_im], $4")
            __ASM_EMIT("add         %[src2_re], $4")
            __ASM_EMIT("add         %[src2_im], $4")
            __ASM_EMIT("add         %[dst_re], $4")
            __ASM_EMIT("add         %[dst_im], $4")
            __ASM_EMIT("subs        %[count], $1")
            __ASM_EMIT("bge         5b")

            __ASM_EMIT("6:")

            : [dst_re] "+r" (dst_re), [dst_im] "+r" (dst_im),
              [src1_re] "+r" (src1_re), [src1_im] "+r" (src1_im),
              [src2_re] "+r" (src2_re), [src2_im] "+r" (src2_im),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11"
        );
    }
}


#endif /* DSP_ARCH_ARM_NEON_D32_COMPLEX_H_ */
