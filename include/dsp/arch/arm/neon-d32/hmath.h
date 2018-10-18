/*
 * hmath.h
 *
 *  Created on: 1 окт. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_ARM_NEON_D32_HMATH_H_
#define DSP_ARCH_ARM_NEON_D32_HMATH_H_

#ifndef DSP_ARCH_ARM_NEON_32_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_ARM_NEON_32_IMPL */

namespace neon_d32
{
    float h_sum(const float *src, size_t count)
    {
        float result = 0.0f;
        IF_ARCH_ARM(
            float *presult = &result;
        );

        ARCH_ARM_ASM
        (
            __ASM_EMIT("veor            q0, q0")
            __ASM_EMIT("veor            q1, q1")
            __ASM_EMIT("subs            %[count], $56") // 14*4 floats
            __ASM_EMIT("blo             2f")

            /* x56 Blocks */
            __ASM_EMIT("1:")
            __ASM_EMIT("vld1.32         {q2-q3}, [%[src]]!")
            __ASM_EMIT("vld1.32         {q4-q5}, [%[src]]!")
            __ASM_EMIT("vadd.f32        q0, q2")
            __ASM_EMIT("vadd.f32        q1, q3")
            __ASM_EMIT("vld1.32         {q6-q7}, [%[src]]!")
            __ASM_EMIT("vadd.f32        q0, q4")
            __ASM_EMIT("vadd.f32        q1, q5")
            __ASM_EMIT("vld1.32         {q8-q9}, [%[src]]!")
            __ASM_EMIT("vadd.f32        q0, q6")
            __ASM_EMIT("vadd.f32        q1, q7")
            __ASM_EMIT("vld1.32         {q10-q11}, [%[src]]!")
            __ASM_EMIT("vadd.f32        q0, q8")
            __ASM_EMIT("vadd.f32        q1, q9")
            __ASM_EMIT("vld1.32         {q12-q13}, [%[src]]!")
            __ASM_EMIT("vadd.f32        q0, q10")
            __ASM_EMIT("vadd.f32        q1, q11")
            __ASM_EMIT("vld1.32         {q14-q15}, [%[src]]!")
            __ASM_EMIT("vadd.f32        q0, q12")
            __ASM_EMIT("vadd.f32        q1, q13")
            __ASM_EMIT("vadd.f32        q0, q14")
            __ASM_EMIT("vadd.f32        q1, q15")
            __ASM_EMIT("subs            %[count], $56")
            __ASM_EMIT("bhs             1b")
            /* x32 Block */
            __ASM_EMIT("2:")
            __ASM_EMIT("adds            %[count], $24") // 56-32
            __ASM_EMIT("blt             4f")
            __ASM_EMIT("vld1.32         {q2-q3}, [%[src]]!")
            __ASM_EMIT("vld1.32         {q4-q5}, [%[src]]!")
            __ASM_EMIT("vadd.f32        q0, q2")
            __ASM_EMIT("vadd.f32        q1, q3")
            __ASM_EMIT("vld1.32         {q6-q7}, [%[src]]!")
            __ASM_EMIT("vadd.f32        q0, q4")
            __ASM_EMIT("vadd.f32        q1, q5")
            __ASM_EMIT("vld1.32         {q8-q9}, [%[src]]!")
            __ASM_EMIT("vadd.f32        q0, q6")
            __ASM_EMIT("vadd.f32        q1, q7")
            __ASM_EMIT("sub             %[count], $32")
            __ASM_EMIT("vadd.f32        q0, q8")
            __ASM_EMIT("vadd.f32        q1, q9")
            /* x16 Block */
            __ASM_EMIT("4:")
            __ASM_EMIT("adds            %[count], $16")
            __ASM_EMIT("blt             6f")
            __ASM_EMIT("vld1.32         {q2-q3}, [%[src]]!")
            __ASM_EMIT("vld1.32         {q4-q5}, [%[src]]!")
            __ASM_EMIT("vadd.f32        q0, q2")
            __ASM_EMIT("vadd.f32        q1, q3")
            __ASM_EMIT("sub             %[count], $16")
            __ASM_EMIT("vadd.f32        q0, q4")
            __ASM_EMIT("vadd.f32        q1, q5")
            /* x8 Block */
            __ASM_EMIT("6:")
            __ASM_EMIT("adds            %[count], $8")
            __ASM_EMIT("blt             8f")
            __ASM_EMIT("vld1.32         {q2-q3}, [%[src]]!")
            __ASM_EMIT("vadd.f32        q0, q2")
            __ASM_EMIT("sub             %[count], $8")
            __ASM_EMIT("vadd.f32        q1, q3")
            /* x4 Block */
            __ASM_EMIT("8:")
            __ASM_EMIT("adds            %[count], $4")
            __ASM_EMIT("blt             10f")
            __ASM_EMIT("vld1.32         {q2}, [%[src]]!")
            __ASM_EMIT("sub             %[count], $4")
            __ASM_EMIT("vadd.f32        q0, q2")
            /* x1 Blocks */
            __ASM_EMIT("10:")
            __ASM_EMIT("vadd.f32        q0, q1")
            __ASM_EMIT("vadd.f32        d0, d1")
            __ASM_EMIT("vadd.f32        s0, s1")
            __ASM_EMIT("adds            %[count], $3")
            __ASM_EMIT("blt             12f")
            __ASM_EMIT("11:")
            __ASM_EMIT("vldm            %[src]!, {s1}")
            __ASM_EMIT("vadd.f32        s0, s1")
            __ASM_EMIT("subs            %[count], $1")
            __ASM_EMIT("bge             11b")

            __ASM_EMIT("12:")
            __ASM_EMIT("vstm            %[dst], {s0}")

            : [src] "+r" (src), [count] "+r" (count)
            : [dst] "r" (presult)
            : "cc", "memory",
              "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );

        return result;
    }

    float h_sqr_sum(const float *src, size_t count)
    {
        float result = 0.0f;
        IF_ARCH_ARM(
            float *presult = &result;
        );

        ARCH_ARM_ASM
        (
            __ASM_EMIT("veor            q0, q0")
            __ASM_EMIT("veor            q1, q1")
            __ASM_EMIT("subs            %[count], $56") // 14*4 floats
            __ASM_EMIT("blo             2f")

            /* x56 Blocks */
            __ASM_EMIT("1:")
            __ASM_EMIT("vld1.32         {q2-q3}, [%[src]]!")
            __ASM_EMIT("vld1.32         {q4-q5}, [%[src]]!")
            __ASM_EMIT("vmla.f32        q0, q2, q2")
            __ASM_EMIT("vmla.f32        q1, q3, q3")
            __ASM_EMIT("vld1.32         {q6-q7}, [%[src]]!")
            __ASM_EMIT("vmla.f32        q0, q4, q4")
            __ASM_EMIT("vmla.f32        q1, q5, q5")
            __ASM_EMIT("vld1.32         {q8-q9}, [%[src]]!")
            __ASM_EMIT("vmla.f32        q0, q6, q6")
            __ASM_EMIT("vmla.f32        q1, q7, q7")
            __ASM_EMIT("vld1.32         {q10-q11}, [%[src]]!")
            __ASM_EMIT("vmla.f32        q0, q8, q8")
            __ASM_EMIT("vmla.f32        q1, q9, q9")
            __ASM_EMIT("vld1.32         {q12-q13}, [%[src]]!")
            __ASM_EMIT("vmla.f32        q0, q10, q10")
            __ASM_EMIT("vmla.f32        q1, q11, q11")
            __ASM_EMIT("vld1.32         {q14-q15}, [%[src]]!")
            __ASM_EMIT("vmla.f32        q0, q12, q12")
            __ASM_EMIT("vmla.f32        q1, q13, q13")
            __ASM_EMIT("vmla.f32        q0, q14, q14")
            __ASM_EMIT("vmla.f32        q1, q15, q15")
            __ASM_EMIT("subs            %[count], $56")
            __ASM_EMIT("bhs             1b")
            /* x32 Block */
            __ASM_EMIT("2:")
            __ASM_EMIT("adds            %[count], $24") // 56-32
            __ASM_EMIT("blt             4f")
            __ASM_EMIT("vld1.32         {q2-q3}, [%[src]]!")
            __ASM_EMIT("vld1.32         {q4-q5}, [%[src]]!")
            __ASM_EMIT("vmla.f32        q0, q2, q2")
            __ASM_EMIT("vmla.f32        q1, q3, q3")
            __ASM_EMIT("vld1.32         {q6-q7}, [%[src]]!")
            __ASM_EMIT("vmla.f32        q0, q4, q4")
            __ASM_EMIT("vmla.f32        q1, q5, q5")
            __ASM_EMIT("vld1.32         {q8-q9}, [%[src]]!")
            __ASM_EMIT("vmla.f32        q0, q6, q6")
            __ASM_EMIT("vmla.f32        q1, q7, q7")
            __ASM_EMIT("sub             %[count], $32")
            __ASM_EMIT("vmla.f32        q0, q8, q8")
            __ASM_EMIT("vmla.f32        q1, q9, q9")
            /* x16 Block */
            __ASM_EMIT("4:")
            __ASM_EMIT("adds            %[count], $16")
            __ASM_EMIT("blt             6f")
            __ASM_EMIT("vld1.32         {q2-q3}, [%[src]]!")
            __ASM_EMIT("vld1.32         {q4-q5}, [%[src]]!")
            __ASM_EMIT("vmla.f32        q0, q2, q2")
            __ASM_EMIT("vmla.f32        q1, q3, q3")
            __ASM_EMIT("sub             %[count], $16")
            __ASM_EMIT("vmla.f32        q0, q4, q4")
            __ASM_EMIT("vmla.f32        q1, q5, q5")
            /* x8 Block */
            __ASM_EMIT("6:")
            __ASM_EMIT("adds            %[count], $8")
            __ASM_EMIT("blt             8f")
            __ASM_EMIT("vld1.32         {q2-q3}, [%[src]]!")
            __ASM_EMIT("vmla.f32        q0, q2, q2")
            __ASM_EMIT("sub             %[count], $8")
            __ASM_EMIT("vmla.f32        q1, q3, q3")
            /* x4 Block */
            __ASM_EMIT("8:")
            __ASM_EMIT("adds            %[count], $4")
            __ASM_EMIT("blt             10f")
            __ASM_EMIT("vld1.32         {q2}, [%[src]]!")
            __ASM_EMIT("sub             %[count], $4")
            __ASM_EMIT("vmla.f32        q0, q2, q2")
            /* x1 Blocks */
            __ASM_EMIT("10:")
            __ASM_EMIT("vadd.f32        q0, q1")
            __ASM_EMIT("vadd.f32        d0, d1")
            __ASM_EMIT("vadd.f32        s0, s1")
            __ASM_EMIT("adds            %[count], $3")
            __ASM_EMIT("blt             12f")
            __ASM_EMIT("11:")
            __ASM_EMIT("vldm            %[src]!, {s1}")
            __ASM_EMIT("vmla.f32        s0, s1, s1")
            __ASM_EMIT("subs            %[count], $1")
            __ASM_EMIT("bge             11b")

            __ASM_EMIT("12:")
            __ASM_EMIT("vstm            %[dst], {s0}")

            : [src] "+r" (src), [count] "+r" (count)
            : [dst] "r" (presult)
            : "cc", "memory",
              "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );

        return result;
    }

    float h_abs_sum(const float *src, size_t count)
    {
        float result = 0.0f;
        IF_ARCH_ARM(
            float *presult = &result;
        );

        ARCH_ARM_ASM
        (
            __ASM_EMIT("veor            q0, q0")
            __ASM_EMIT("veor            q1, q1")
            __ASM_EMIT("subs            %[count], $56") // 14*4 floats
            __ASM_EMIT("blo             2f")

            /* x56 Blocks */
            __ASM_EMIT("1:")
            __ASM_EMIT("vld1.32         {q2-q3}, [%[src]]!")
            __ASM_EMIT("vld1.32         {q4-q5}, [%[src]]!")
            __ASM_EMIT("vabs.f32        q2, q2")
            __ASM_EMIT("vabs.f32        q3, q3")
            __ASM_EMIT("vld1.32         {q6-q7}, [%[src]]!")
            __ASM_EMIT("vabs.f32        q4, q4")
            __ASM_EMIT("vabs.f32        q5, q5")
            __ASM_EMIT("vld1.32         {q8-q9}, [%[src]]!")
            __ASM_EMIT("vabs.f32        q6, q6")
            __ASM_EMIT("vabs.f32        q7, q7")
            __ASM_EMIT("vld1.32         {q10-q11}, [%[src]]!")
            __ASM_EMIT("vabs.f32        q8, q8")
            __ASM_EMIT("vabs.f32        q9, q9")
            __ASM_EMIT("vld1.32         {q12-q13}, [%[src]]!")
            __ASM_EMIT("vabs.f32        q10, q10")
            __ASM_EMIT("vabs.f32        q11, q11")
            __ASM_EMIT("vld1.32         {q14-q15}, [%[src]]!")
            __ASM_EMIT("vabs.f32        q12, q12")
            __ASM_EMIT("vabs.f32        q13, q13")
            __ASM_EMIT("vabs.f32        q14, q14")
            __ASM_EMIT("vabs.f32        q15, q15")
            __ASM_EMIT("vadd.f32        q0, q2")
            __ASM_EMIT("vadd.f32        q1, q3")
            __ASM_EMIT("vadd.f32        q0, q4")
            __ASM_EMIT("vadd.f32        q1, q5")
            __ASM_EMIT("vadd.f32        q0, q6")
            __ASM_EMIT("vadd.f32        q1, q7")
            __ASM_EMIT("vadd.f32        q0, q8")
            __ASM_EMIT("vadd.f32        q1, q9")
            __ASM_EMIT("vadd.f32        q0, q10")
            __ASM_EMIT("vadd.f32        q1, q11")
            __ASM_EMIT("vadd.f32        q0, q12")
            __ASM_EMIT("vadd.f32        q1, q13")
            __ASM_EMIT("vadd.f32        q0, q14")
            __ASM_EMIT("vadd.f32        q1, q15")
            __ASM_EMIT("subs            %[count], $56")
            __ASM_EMIT("bhs             1b")
            /* x32 Block */
            __ASM_EMIT("2:")
            __ASM_EMIT("adds            %[count], $24") // 56-32
            __ASM_EMIT("blt             4f")
            __ASM_EMIT("vld1.32         {q2-q3}, [%[src]]!")
            __ASM_EMIT("vld1.32         {q4-q5}, [%[src]]!")
            __ASM_EMIT("vabs.f32        q2, q2")
            __ASM_EMIT("vabs.f32        q3, q3")
            __ASM_EMIT("vld1.32         {q6-q7}, [%[src]]!")
            __ASM_EMIT("vabs.f32        q4, q4")
            __ASM_EMIT("vabs.f32        q5, q5")
            __ASM_EMIT("vld1.32         {q8-q9}, [%[src]]!")
            __ASM_EMIT("vabs.f32        q6, q6")
            __ASM_EMIT("vabs.f32        q7, q7")
            __ASM_EMIT("vabs.f32        q8, q8")
            __ASM_EMIT("vabs.f32        q9, q9")
            __ASM_EMIT("vadd.f32        q0, q2")
            __ASM_EMIT("vadd.f32        q1, q3")
            __ASM_EMIT("vadd.f32        q0, q4")
            __ASM_EMIT("vadd.f32        q1, q5")
            __ASM_EMIT("vadd.f32        q0, q6")
            __ASM_EMIT("vadd.f32        q1, q7")
            __ASM_EMIT("sub             %[count], $32")
            __ASM_EMIT("vadd.f32        q0, q8")
            __ASM_EMIT("vadd.f32        q1, q9")
            /* x16 Block */
            __ASM_EMIT("4:")
            __ASM_EMIT("adds            %[count], $16")
            __ASM_EMIT("blt             6f")
            __ASM_EMIT("vld1.32         {q2-q3}, [%[src]]!")
            __ASM_EMIT("vld1.32         {q4-q5}, [%[src]]!")
            __ASM_EMIT("vabs.f32        q2, q2")
            __ASM_EMIT("vabs.f32        q3, q3")
            __ASM_EMIT("vabs.f32        q4, q4")
            __ASM_EMIT("vabs.f32        q5, q5")
            __ASM_EMIT("vadd.f32        q0, q2")
            __ASM_EMIT("vadd.f32        q1, q3")
            __ASM_EMIT("sub             %[count], $16")
            __ASM_EMIT("vadd.f32        q0, q4")
            __ASM_EMIT("vadd.f32        q1, q5")
            /* x8 Block */
            __ASM_EMIT("6:")
            __ASM_EMIT("adds            %[count], $8")
            __ASM_EMIT("blt             8f")
            __ASM_EMIT("vld1.32         {q2-q3}, [%[src]]!")
            __ASM_EMIT("vabs.f32        q2, q2")
            __ASM_EMIT("vabs.f32        q3, q3")
            __ASM_EMIT("vadd.f32        q0, q2")
            __ASM_EMIT("sub             %[count], $8")
            __ASM_EMIT("vadd.f32        q1, q3")
            /* x4 Block */
            __ASM_EMIT("8:")
            __ASM_EMIT("adds            %[count], $4")
            __ASM_EMIT("blt             10f")
            __ASM_EMIT("vld1.32         {q2}, [%[src]]!")
            __ASM_EMIT("vabs.f32        q2, q2")
            __ASM_EMIT("sub             %[count], $4")
            __ASM_EMIT("vadd.f32        q0, q2")
            /* x1 Blocks */
            __ASM_EMIT("10:")
            __ASM_EMIT("vadd.f32        q0, q1")
            __ASM_EMIT("vadd.f32        d0, d1")
            __ASM_EMIT("vadd.f32        s0, s1")
            __ASM_EMIT("adds            %[count], $3")
            __ASM_EMIT("blt             12f")
            __ASM_EMIT("11:")
            __ASM_EMIT("vldm            %[src]!, {s2}")
            __ASM_EMIT("vabs.f32        d1, d1")
            __ASM_EMIT("vadd.f32        s0, s2")
            __ASM_EMIT("subs            %[count], $1")
            __ASM_EMIT("bge             11b")

            __ASM_EMIT("12:")
            __ASM_EMIT("vstm            %[dst], {s0}")

            : [src] "+r" (src), [count] "+r" (count)
            : [dst] "r" (presult)
            : "cc", "memory",
              "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );

        return result;
    }
}

#endif /* DSP_ARCH_ARM_NEON_D32_HMATH_H_ */
