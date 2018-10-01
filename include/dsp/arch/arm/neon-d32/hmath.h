/*
 * hmath.h
 *
 *  Created on: 1 окт. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_ARM_NEON_D32_HMATH_H_
#define DSP_ARCH_ARM_NEON_D32_HMATH_H_

namespace neon_d32
{
    float h_sum(const float *src, size_t count)
    {
        float result;

        ARCH_ARM_ASM
        (
            __ASM_EMIT("veor            q0, q0")
            __ASM_EMIT("veor            q1, q1")
            __ASM_EMIT("subs            %[count], $56") // 14*4 floats
            __ASM_EMIT("blo             2f")

            /* x56 Blocks */
            __ASM_EMIT("1:")
            __ASM_EMIT("vld1.32         {q2-q3}, %[src]!")
            __ASM_EMIT("vld1.32         {q4-q5}, %[src]!")
            __ASM_EMIT("vadd.f32        q0, q2")
            __ASM_EMIT("vadd.f32        q1, q3")
            __ASM_EMIT("vld1.32         {q6-q7}, %[src]!")
            __ASM_EMIT("vadd.f32        q0, q4")
            __ASM_EMIT("vadd.f32        q1, q5")
            __ASM_EMIT("vld1.32         {q8-q9}, %[src]!")
            __ASM_EMIT("vadd.f32        q0, q6")
            __ASM_EMIT("vadd.f32        q1, q7")
            __ASM_EMIT("vld1.32         {q10-q11}, %[src]!")
            __ASM_EMIT("vadd.f32        q0, q8")
            __ASM_EMIT("vadd.f32        q1, q9")
            __ASM_EMIT("vld1.32         {q12-q13}, %[src]!")
            __ASM_EMIT("vadd.f32        q0, q10")
            __ASM_EMIT("vadd.f32        q1, q11")
            __ASM_EMIT("vld1.32         {q14-q15}, %[src]!")
            __ASM_EMIT("vadd.f32        q0, q12")
            __ASM_EMIT("vadd.f32        q1, q13")
            __ASM_EMIT("vadd.f32        q0, q14")
            __ASM_EMIT("vadd.f32        q1, q15")
            __ASM_EMIT("subs            $56, %[count]")
            __ASM_EMIT("bhs             1b")
            /* x32 Block */
            __ASM_EMIT("2:")
            __ASM_EMIT("adds            %[count], 24") // 56-32
            __ASM_EMIT("blt             4f")
            __ASM_EMIT("vld1.32         {q2-q3}, %[src]!")
            __ASM_EMIT("vld1.32         {q4-q5}, %[src]!")
            __ASM_EMIT("vadd.f32        q0, q2")
            __ASM_EMIT("vadd.f32        q1, q3")
            __ASM_EMIT("vld1.32         {q6-q7}, %[src]!")
            __ASM_EMIT("vadd.f32        q0, q4")
            __ASM_EMIT("vadd.f32        q1, q5")
            __ASM_EMIT("vld1.32         {q8-q9}, %[src]!")
            __ASM_EMIT("vadd.f32        q0, q6")
            __ASM_EMIT("vadd.f32        q1, q7")
            __ASM_EMIT("sub             $32, %[count]")
            __ASM_EMIT("vadd.f32        q0, q8")
            __ASM_EMIT("vadd.f32        q1, q9")
            /* x16 Block */
            __ASM_EMIT("4:")
            __ASM_EMIT("adds            %[count], 16")
            __ASM_EMIT("blt             6f")
            __ASM_EMIT("vld1.32         {q2-q3}, %[src]!")
            __ASM_EMIT("vld1.32         {q4-q5}, %[src]!")
            __ASM_EMIT("vadd.f32        q0, q2")
            __ASM_EMIT("vadd.f32        q1, q3")
            __ASM_EMIT("sub             $16, %[count]")
            __ASM_EMIT("vadd.f32        q0, q4")
            __ASM_EMIT("vadd.f32        q1, q5")
            /* x8 Block */
            __ASM_EMIT("6:")
            __ASM_EMIT("adds            %[count], 8")
            __ASM_EMIT("blt             8f")
            __ASM_EMIT("vld1.32         {q2-q3}, %[src]!")
            __ASM_EMIT("vadd.f32        q0, q2")
            __ASM_EMIT("sub             $8, %[count]")
            __ASM_EMIT("vadd.f32        q1, q3")
            /* x4 Block */
            __ASM_EMIT("8:")
            __ASM_EMIT("adds            %[count], 4")
            __ASM_EMIT("blt             10f")
            __ASM_EMIT("vld1.32         {q2}, %[src]!")
            __ASM_EMIT("sub             $4, %[count]")
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

            : [src] "+r" (src), [count] "+r" (count),
              "=Yz" (result)
            :
            : "cc",
              "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );

        return result;
    }
}

#endif /* DSP_ARCH_ARM_NEON_D32_HMATH_H_ */
