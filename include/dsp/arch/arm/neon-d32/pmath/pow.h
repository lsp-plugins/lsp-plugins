/*
 * pow.h
 *
 *  Created on: 13 дек. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_ARM_NEON_D32_PMATH_POW_H_
#define DSP_ARCH_ARM_NEON_D32_PMATH_POW_H_

#ifndef DSP_ARCH_ARM_NEON_32_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_ARM_NEON_32_IMPL */

#include <dsp/arch/arm/neon-d32/pmath/exp.h>
#include <dsp/arch/arm/neon-d32/pmath/log.h>

namespace neon_d32
{
    void powcv2(float *dst, const float *v, float c, size_t count)
    {
//        float C = logf(c);
//        for (size_t i=0; i<count; ++i)
//            v[i] = expf(v[i] * C);

        ARCH_ARM_ASM(
            // log(c)
            __ASM_EMIT("vldm            %[LOGC], {q14}")
            __ASM_EMIT("vld1.32         {d0[], d1[]}, [%[C]]")
            LOGN_CORE_X4
            __ASM_EMIT("vadd.f32        q0, q0, q0")            // q0   = 2*y*L
            __ASM_EMIT("vmla.f32        q0, q2, q14")           // q0   = 2*y*L + R/log2(E)

            __ASM_EMIT("subs            %[count], $8")
            __ASM_EMIT("vmov            q14, q0")               // q14  = log(c)
            __ASM_EMIT("vldm            %[LOG2E], {q15}")
            __ASM_EMIT("vmul.f32        q15, q15, q14")         // q15  = log(c)*log2(E)
            __ASM_EMIT("blo             2f")

            // x8 blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("vld1.32         {q0-q1}, [%[src]]!")
            __ASM_EMIT("vmul.f32        q0, q0, q15")
            __ASM_EMIT("vmul.f32        q1, q1, q15")
            POW2_CORE_X8
            __ASM_EMIT("subs            %[count], $8")
            __ASM_EMIT("vst1.32         {q0-q1}, [%[dst]]!")
            __ASM_EMIT("bhs             1b")

            __ASM_EMIT("2:")
            __ASM_EMIT("adds            %[count], $4")
            __ASM_EMIT("blt             4f")

            // x4 block
            __ASM_EMIT("vld1.32         {q0}, [%[src]]!")
            __ASM_EMIT("vmul.f32        q0, q0, q15")
            POW2_CORE_X4
            __ASM_EMIT("sub             %[count], $4")
            __ASM_EMIT("vst1.32         {q0}, [%[dst]]!")

            __ASM_EMIT("4:")
            __ASM_EMIT("adds            %[count], $4")
            __ASM_EMIT("bls             12f")

            // Tail: 1x-3x block
            __ASM_EMIT("tst             %[count], $1")
            __ASM_EMIT("beq             6f")
            __ASM_EMIT("vldm            %[src]!, {s2}")
            __ASM_EMIT("6:")
            __ASM_EMIT("tst             %[count], $2")
            __ASM_EMIT("beq             8f")
            __ASM_EMIT("vldm            %[src], {d0}")
            __ASM_EMIT("8:")

            __ASM_EMIT("vmul.f32        q0, q0, q15")
            POW2_CORE_X4

            __ASM_EMIT("tst             %[count], $1")
            __ASM_EMIT("beq             10f")
            __ASM_EMIT("vstm            %[dst]!, {s2}")
            __ASM_EMIT("10:")
            __ASM_EMIT("tst             %[count], $2")
            __ASM_EMIT("beq             12f")
            __ASM_EMIT("vstm            %[dst], {d0}")

            // End
            __ASM_EMIT("12:")

            : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
            : [L2C] "r" (&LOG2_CONST[0]),
              [LOGC] "r" (&LOGE_C[0]),
              [E2C] "r" (&EXP2_CONST[0]),
              [LOG2E] "r" (&EXP_LOG2E[0]),
              [C] "r" [&c]
            : "cc", "memory",
              "q0", "q1", "q2", "q3",
              "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11",
              "q12", "q13", "q14", "q15"
        );
    }

    void powcv1(float *dst, float c, size_t count)
    {
//        float C = logf(c);
//        for (size_t i=0; i<count; ++i)
//            v[i] = expf(v[i] * C);
        IF_ARCH_ARM(float *src = dst);

        ARCH_ARM_ASM(
            // log(c)
            __ASM_EMIT("vldm            %[LOGC], {q14}")
            __ASM_EMIT("vld1.32         {d0[], d1[]}, [%[C]]")
            LOGN_CORE_X4
            __ASM_EMIT("vadd.f32        q0, q0, q0")            // q0   = 2*y*L
            __ASM_EMIT("vmla.f32        q0, q2, q14")           // q0   = 2*y*L + R/log2(E)

            __ASM_EMIT("subs            %[count], $8")
            __ASM_EMIT("vmov            q14, q0")               // q14  = log(c)
            __ASM_EMIT("vldm            %[LOG2E], {q15}")
            __ASM_EMIT("vmul.f32        q15, q15, q14")         // q15  = log(c)*log2(E)
            __ASM_EMIT("blo             2f")

            // x8 blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("vld1.32         {q0-q1}, [%[src]]!")
            __ASM_EMIT("vmul.f32        q0, q0, q15")
            __ASM_EMIT("vmul.f32        q1, q1, q15")
            POW2_CORE_X8
            __ASM_EMIT("subs            %[count], $8")
            __ASM_EMIT("vst1.32         {q0-q1}, [%[dst]]!")
            __ASM_EMIT("bhs             1b")

            __ASM_EMIT("2:")
            __ASM_EMIT("adds            %[count], $4")
            __ASM_EMIT("blt             4f")

            // x4 block
            __ASM_EMIT("vld1.32         {q0}, [%[src]]!")
            __ASM_EMIT("vmul.f32        q0, q0, q15")
            POW2_CORE_X4
            __ASM_EMIT("sub             %[count], $4")
            __ASM_EMIT("vst1.32         {q0}, [%[dst]]!")

            __ASM_EMIT("4:")
            __ASM_EMIT("adds            %[count], $4")
            __ASM_EMIT("bls             12f")

            // Tail: 1x-3x block
            __ASM_EMIT("tst             %[count], $1")
            __ASM_EMIT("beq             6f")
            __ASM_EMIT("vldm            %[src]!, {s2}")
            __ASM_EMIT("6:")
            __ASM_EMIT("tst             %[count], $2")
            __ASM_EMIT("beq             8f")
            __ASM_EMIT("vldm            %[src], {d0}")
            __ASM_EMIT("8:")

            __ASM_EMIT("vmul.f32        q0, q0, q15")
            POW2_CORE_X4

            __ASM_EMIT("tst             %[count], $1")
            __ASM_EMIT("beq             10f")
            __ASM_EMIT("vstm            %[dst]!, {s2}")
            __ASM_EMIT("10:")
            __ASM_EMIT("tst             %[count], $2")
            __ASM_EMIT("beq             12f")
            __ASM_EMIT("vstm            %[dst], {d0}")

            // End
            __ASM_EMIT("12:")

            : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
            : [L2C] "r" (&LOG2_CONST[0]),
              [LOGC] "r" (&LOGE_C[0]),
              [E2C] "r" (&EXP2_CONST[0]),
              [LOG2E] "r" (&EXP_LOG2E[0]),
              [C] "r" [&c]
            : "cc", "memory",
              "q0", "q1", "q2", "q3",
              "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11",
              "q12", "q13", "q14", "q15"
        );
    }
}


#endif /* DSP_ARCH_ARM_NEON_D32_PMATH_POW_H_ */
