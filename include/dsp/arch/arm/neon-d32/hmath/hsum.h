/*
 * hsum.h
 *
 *  Created on: 29 нояб. 2019 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_ARM_NEON_D32_HMATH_HSUM_H_
#define DSP_ARCH_ARM_NEON_D32_HMATH_HSUM_H_


#ifndef DSP_ARCH_ARM_NEON_32_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_ARM_NEON_32_IMPL */

namespace neon_d32
{
    float h_sum(const float *src, size_t count)
    {
        IF_ARCH_ARM(float result);
        ARCH_ARM_ASM
        (
            __ASM_EMIT("veor            q0, q0")
            __ASM_EMIT("veor            q1, q1")
            __ASM_EMIT("subs            %[count], $24") // 6*4 floats
            __ASM_EMIT("blo             2f")

            /* x24 Blocks */
            __ASM_EMIT("1:")
            __ASM_EMIT("vldm            %[src]!, {q2-q7}")
            __ASM_EMIT("vadd.f32        q0, q2")
            __ASM_EMIT("vadd.f32        q1, q3")
            __ASM_EMIT("vadd.f32        q0, q4")
            __ASM_EMIT("vadd.f32        q1, q5")
            __ASM_EMIT("vadd.f32        q0, q6")
            __ASM_EMIT("vadd.f32        q1, q7")
            __ASM_EMIT("subs            %[count], $24")
            __ASM_EMIT("bhs             1b")
            /* x16 Block */
            __ASM_EMIT("2:")
            __ASM_EMIT("adds            %[count], $8")
            __ASM_EMIT("blt             4f")
            __ASM_EMIT("vldm            %[src]!, {q2-q5}")
            __ASM_EMIT("vadd.f32        q0, q2")
            __ASM_EMIT("vadd.f32        q1, q3")
            __ASM_EMIT("vadd.f32        q0, q4")
            __ASM_EMIT("vadd.f32        q1, q5")
            __ASM_EMIT("sub             %[count], $16")
            /* x8 Block */
            __ASM_EMIT("4:")
            __ASM_EMIT("adds            %[count], $8")
            __ASM_EMIT("blt             6f")
            __ASM_EMIT("vldm            %[src]!, {q2-q3}")
            __ASM_EMIT("vadd.f32        q0, q2")
            __ASM_EMIT("vadd.f32        q1, q3")
            __ASM_EMIT("sub             %[count], $8")
            /* x4 Block */
            __ASM_EMIT("6:")
            __ASM_EMIT("vadd.f32        q0, q1")
            __ASM_EMIT("adds            %[count], $4")
            __ASM_EMIT("blt             8f")
            __ASM_EMIT("vldm            %[src]!, {q2}")
            __ASM_EMIT("vadd.f32        q0, q2")
            __ASM_EMIT("sub             %[count], $4")
            /* x1 Blocks */
            __ASM_EMIT("8:")
            __ASM_EMIT("adds            %[count], $3")
            __ASM_EMIT("blt             10f")
            __ASM_EMIT("9:")
            __ASM_EMIT("vld1.32         {d4[], d5[]}, [%[src]]!")
            __ASM_EMIT("vadd.f32        s0, s8")
            __ASM_EMIT("subs            %[count], $1")
            __ASM_EMIT("bge             9b")
            /* end */
            __ASM_EMIT("10:")
            __ASM_EMIT("vadd.f32        d0, d1")
            __ASM_EMIT("vadd.f32        %[res], s0, s1")

            : [res] "=t" (result),
              [src] "+r" (src), [count] "+r" (count)
            :
            : "cc", "memory",
              "q1", "q2", "q3", "q4", "q5", "q6", "q7"
        );

        return result;
    }

    float h_sqr_sum(const float *src, size_t count)
    {
        IF_ARCH_ARM(float result);
        ARCH_ARM_ASM
        (
            __ASM_EMIT("veor            q0, q0")
            __ASM_EMIT("veor            q1, q1")
            __ASM_EMIT("subs            %[count], $24") // 6*4 floats
            __ASM_EMIT("blo             2f")

            /* x24 Blocks */
            __ASM_EMIT("1:")
            __ASM_EMIT("vldm            %[src]!, {q2-q7}")
            __ASM_EMIT("vfma.f32        q0, q2, q2")
            __ASM_EMIT("vfma.f32        q1, q3, q3")
            __ASM_EMIT("vfma.f32        q0, q4, q4")
            __ASM_EMIT("vfma.f32        q1, q5, q5")
            __ASM_EMIT("vfma.f32        q0, q6, q6")
            __ASM_EMIT("vfma.f32        q1, q7, q7")
            __ASM_EMIT("subs            %[count], $24")
            __ASM_EMIT("bhs             1b")
            /* x16 Block */
            __ASM_EMIT("2:")
            __ASM_EMIT("adds            %[count], $8")
            __ASM_EMIT("blt             4f")
            __ASM_EMIT("vldm            %[src]!, {q2-q5}")
            __ASM_EMIT("vfma.f32        q0, q2, q2")
            __ASM_EMIT("vfma.f32        q1, q3, q3")
            __ASM_EMIT("vfma.f32        q0, q4, q4")
            __ASM_EMIT("vfma.f32        q1, q5, q5")
            __ASM_EMIT("sub             %[count], $16")
            /* x8 Block */
            __ASM_EMIT("4:")
            __ASM_EMIT("adds            %[count], $8")
            __ASM_EMIT("blt             6f")
            __ASM_EMIT("vldm            %[src]!, {q2-q3}")
            __ASM_EMIT("vfma.f32        q0, q2, q2")
            __ASM_EMIT("vfma.f32        q1, q3, q3")
            __ASM_EMIT("sub             %[count], $8")
            /* x4 Block */
            __ASM_EMIT("6:")
            __ASM_EMIT("vadd.f32        q0, q1")
            __ASM_EMIT("adds            %[count], $4")
            __ASM_EMIT("blt             8f")
            __ASM_EMIT("vldm            %[src]!, {q2}")
            __ASM_EMIT("vfma.f32        q0, q2, q2")
            __ASM_EMIT("sub             %[count], $4")
            /* x1 Blocks */
            __ASM_EMIT("8:")
            __ASM_EMIT("adds            %[count], $3")
            __ASM_EMIT("blt             10f")
            __ASM_EMIT("9:")
            __ASM_EMIT("vld1.32         {d4[], d5[]}, [%[src]]!")
            __ASM_EMIT("vfma.f32        s0, s8, s8")
            __ASM_EMIT("subs            %[count], $1")
            __ASM_EMIT("bge             9b")
            /* end */
            __ASM_EMIT("10:")
            __ASM_EMIT("vadd.f32        d0, d1")
            __ASM_EMIT("vadd.f32        %[res], s0, s1")

            : [res] "=t" (result),
              [src] "+r" (src), [count] "+r" (count)
            :
            : "cc", "memory",
              "q1", "q2", "q3", "q4", "q5", "q6", "q7"
        );

        return result;
    }

    float h_abs_sum(const float *src, size_t count)
    {
        IF_ARCH_ARM(float result);
        ARCH_ARM_ASM
        (
            __ASM_EMIT("veor            q0, q0")
            __ASM_EMIT("veor            q1, q1")
            __ASM_EMIT("subs            %[count], $24") // 6*4 floats
            __ASM_EMIT("blo             2f")

            /* x24 Blocks */
            __ASM_EMIT("1:")
            __ASM_EMIT("vldm            %[src]!, {q2-q7}")
            __ASM_EMIT("vabs.f32        q2, q2")
            __ASM_EMIT("vabs.f32        q3, q3")
            __ASM_EMIT("vabs.f32        q4, q4")
            __ASM_EMIT("vabs.f32        q5, q5")
            __ASM_EMIT("vabs.f32        q6, q6")
            __ASM_EMIT("vabs.f32        q7, q7")
            __ASM_EMIT("vadd.f32        q0, q2")
            __ASM_EMIT("vadd.f32        q1, q3")
            __ASM_EMIT("vadd.f32        q0, q4")
            __ASM_EMIT("vadd.f32        q1, q5")
            __ASM_EMIT("vadd.f32        q0, q6")
            __ASM_EMIT("vadd.f32        q1, q7")
            __ASM_EMIT("subs            %[count], $24")
            __ASM_EMIT("bhs             1b")
            /* x16 Block */
            __ASM_EMIT("2:")
            __ASM_EMIT("adds            %[count], $8")
            __ASM_EMIT("blt             4f")
            __ASM_EMIT("vldm            %[src]!, {q2-q5}")
            __ASM_EMIT("vabs.f32        q2, q2")
            __ASM_EMIT("vabs.f32        q3, q3")
            __ASM_EMIT("vabs.f32        q4, q4")
            __ASM_EMIT("vabs.f32        q5, q5")
            __ASM_EMIT("vadd.f32        q0, q2")
            __ASM_EMIT("vadd.f32        q1, q3")
            __ASM_EMIT("vadd.f32        q0, q4")
            __ASM_EMIT("vadd.f32        q1, q5")
            __ASM_EMIT("sub             %[count], $16")
            /* x8 Block */
            __ASM_EMIT("4:")
            __ASM_EMIT("adds            %[count], $8")
            __ASM_EMIT("blt             6f")
            __ASM_EMIT("vldm            %[src]!, {q2-q3}")
            __ASM_EMIT("vabs.f32        q2, q2")
            __ASM_EMIT("vabs.f32        q3, q3")
            __ASM_EMIT("vadd.f32        q0, q2")
            __ASM_EMIT("vadd.f32        q1, q3")
            __ASM_EMIT("sub             %[count], $8")
            /* x4 Block */
            __ASM_EMIT("6:")
            __ASM_EMIT("vadd.f32        q0, q1")
            __ASM_EMIT("adds            %[count], $4")
            __ASM_EMIT("blt             8f")
            __ASM_EMIT("vldm            %[src]!, {q2}")
            __ASM_EMIT("vabs.f32        q2, q2")
            __ASM_EMIT("vadd.f32        q0, q2")
            __ASM_EMIT("sub             %[count], $4")
            /* x1 Blocks */
            __ASM_EMIT("8:")
            __ASM_EMIT("adds            %[count], $3")
            __ASM_EMIT("blt             10f")
            __ASM_EMIT("9:")
            __ASM_EMIT("vld1.32         {d4[], d5[]}, [%[src]]!")
            __ASM_EMIT("vabs.f32        q2, q2")
            __ASM_EMIT("vadd.f32        s0, s8")
            __ASM_EMIT("subs            %[count], $1")
            __ASM_EMIT("bge             9b")
            /* end */
            __ASM_EMIT("10:")
            __ASM_EMIT("vadd.f32        d0, d1")
            __ASM_EMIT("vadd.f32        %[res], s0, s1")

            : [res] "=t" (result),
              [src] "+r" (src), [count] "+r" (count)
            :
            : "cc", "memory",
              "q1", "q2", "q3", "q4", "q5", "q6", "q7"
        );

        return result;
    }
}

#endif /* DSP_ARCH_ARM_NEON_D32_HMATH_HSUM_H_ */
