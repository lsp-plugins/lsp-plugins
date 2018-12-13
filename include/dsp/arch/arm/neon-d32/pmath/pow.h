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
            __ASM_EMIT("vldm            %[LOGC], {q15}")        // 2 * log2(E)
            __ASM_EMIT("vld1.32         {d0[], d1[]}, [%[C]]")
            LOGN_CORE_X4
            __ASM_EMIT("vmul.f32        q0, q0, q15")           // q0   = 2*y*L*log2(E)
            __ASM_EMIT("vadd.f32        q15, q0, q2")           // q15  = 2*y*L*log2(E) + R = log(c)*log2(E)

            __ASM_EMIT("subs            %[count], $8")
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

            : [dst] "+r" (dst), [src] "+r" (v), [count] "+r" (count)
            : [L2C] "r" (&LOG2_CONST[0]),
              [LOGC] "r" (&LOGB_C[0]),
              [E2C] "r" (&EXP2_CONST[0]),
              [C] "r" (&c)
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
            __ASM_EMIT("vldm            %[LOGC], {q15}")
            __ASM_EMIT("vld1.32         {d0[], d1[]}, [%[C]]")
            LOGN_CORE_X4
            __ASM_EMIT("vmul.f32        q0, q0, q15")           // q0   = 2*y*L*log2(E)
            __ASM_EMIT("vadd.f32        q15, q0, q2")           // q15  = 2*y*L*log2(E) + R = log(c)*log2(E)

            __ASM_EMIT("subs            %[count], $8")
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
              [LOGC] "r" (&LOGB_C[0]),
              [E2C] "r" (&EXP2_CONST[0]),
              [C] "r" (&c)
            : "cc", "memory",
              "q0", "q1", "q2", "q3",
              "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11",
              "q12", "q13", "q14", "q15"
        );
    }

    void powvc2(float *dst, const float *c, float v, size_t count)
    {
//        for (size_t i=0; i<count; ++i)
//            dst[i] = expf(v * logf(c[i]));

        ARCH_ARM_ASM(
            __ASM_EMIT("subs            %[count], $8")
            __ASM_EMIT("vldm            %[LOGC], {q14}")        // q14 = 2*log2(E)
            __ASM_EMIT("vld1.32         {d30[], d31[]}, [%[V]]")// q15 = v
            __ASM_EMIT("blo             2f")

            // x8 blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("vld1.32         {q0-q1}, [%[src]]!")
            // log(c[i])
            LOGN_CORE_X8
            __ASM_EMIT("vmla.f32        q2, q0, q14")           // q2 = 2*y*L*log2(E) + R
            __ASM_EMIT("vmla.f32        q3, q1, q14")
            __ASM_EMIT("vmul.f32        q0, q2, q15")           // q0 = (2*y*L*log2(E) + R)*v = log(c[i])*log2(E)*v
            __ASM_EMIT("vmul.f32        q1, q3, q15")
            POW2_CORE_X8
            __ASM_EMIT("subs            %[count], $8")
            __ASM_EMIT("vst1.32         {q0-q1}, [%[dst]]!")
            __ASM_EMIT("bhs             1b")

            __ASM_EMIT("2:")
            __ASM_EMIT("adds            %[count], $4")
            __ASM_EMIT("blt             4f")

            // x4 block
            __ASM_EMIT("vld1.32         {q0}, [%[src]]!")
            // log(c[i])
            LOGN_CORE_X4
            __ASM_EMIT("vmla.f32        q2, q0, q14")           // q2 = 2*y*L*log2(E) + R
            __ASM_EMIT("vmul.f32        q0, q2, q15")           // q0 = (2*y*L*log2(E) + R)*v = log(c[i])*log2(E)*v
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

            LOGN_CORE_X4
            __ASM_EMIT("vmla.f32        q2, q0, q14")           // q2 = 2*y*L*log2(E) + R
            __ASM_EMIT("vmul.f32        q0, q2, q15")           // q0 = (2*y*L*log2(E) + R)*v = log(c[i])*log2(E)*v
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

            : [dst] "+r" (dst), [src] "+r" (c), [count] "+r" (count)
            : [L2C] "r" (&LOG2_CONST[0]),
              [LOGC] "r" (&LOGB_C[0]),
              [E2C] "r" (&EXP2_CONST[0]),
              [V] "r" (&v)
            : "cc", "memory",
              "q0", "q1", "q2", "q3",
              "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11",
              "q12", "q13", "q14", "q15"
        );
    }

    void powvc1(float *c, float v, size_t count)
    {
//        for (size_t i=0; i<count; ++i)
//            c[i] = expf(v * logf(c[i]));
        IF_ARCH_ARM(float *dst = c);

        ARCH_ARM_ASM(
            __ASM_EMIT("subs            %[count], $8")
            __ASM_EMIT("vldm            %[LOGC], {q14}")        // q14 = 2*log2(E)
            __ASM_EMIT("vld1.32         {d30[], d31[]}, [%[V]]")// q15 = v
            __ASM_EMIT("blo             2f")

            // x8 blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("vld1.32         {q0-q1}, [%[src]]!")
            // log(c[i])
            LOGN_CORE_X8
            __ASM_EMIT("vmla.f32        q2, q0, q14")           // q2 = 2*y*L*log2(E) + R
            __ASM_EMIT("vmla.f32        q3, q1, q14")
            __ASM_EMIT("vmul.f32        q0, q2, q15")           // q0 = (2*y*L*log2(E) + R)*v = log(c[i])*log2(E)*v
            __ASM_EMIT("vmul.f32        q1, q3, q15")
            POW2_CORE_X8
            __ASM_EMIT("subs            %[count], $8")
            __ASM_EMIT("vst1.32         {q0-q1}, [%[dst]]!")
            __ASM_EMIT("bhs             1b")

            __ASM_EMIT("2:")
            __ASM_EMIT("adds            %[count], $4")
            __ASM_EMIT("blt             4f")

            // x4 block
            __ASM_EMIT("vld1.32         {q0}, [%[src]]!")
            // log(c[i])
            LOGN_CORE_X4
            __ASM_EMIT("vmla.f32        q2, q0, q14")           // q2 = 2*y*L*log2(E) + R
            __ASM_EMIT("vmul.f32        q0, q2, q15")           // q0 = (2*y*L*log2(E) + R)*v = log(c[i])*log2(E)*v

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

            LOGN_CORE_X4
            __ASM_EMIT("vmla.f32        q2, q0, q14")           // q2 = 2*y*L*log2(E) + R
            __ASM_EMIT("vmul.f32        q0, q2, q15")           // q0 = (2*y*L*log2(E) + R)*v = log(c[i])*log2(E)*v
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

            : [dst] "+r" (dst), [src] "+r" (c), [count] "+r" (count)
            : [L2C] "r" (&LOG2_CONST[0]),
              [LOGC] "r" (&LOGB_C[0]),
              [E2C] "r" (&EXP2_CONST[0]),
              [V] "r" (&v)
            : "cc", "memory",
              "q0", "q1", "q2", "q3",
              "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11",
              "q12", "q13", "q14", "q15"
        );
    }

    void powvx2(float *dst, const float *v, const float *x, size_t count)
    {
//        for (size_t i=0; i<count; ++i)
//            dst[i] = expf(x[i] * logf(v[i]));

        ARCH_ARM_ASM(
            __ASM_EMIT("subs            %[count], $8")
            __ASM_EMIT("vldm            %[LOGC], {q14}")
            __ASM_EMIT("blo             2f")

            // x8 blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("vld1.32         {q0-q1}, [%[src]]!")
            // log(v[i])
            LOGN_CORE_X8
            __ASM_EMIT("vld1.32         {q4-q5}, [%[x]]!")
            __ASM_EMIT("vmla.f32        q2, q0, q14")           // q2 = 2*y*L*log2(E)+R
            __ASM_EMIT("vmla.f32        q3, q1, q14")
            __ASM_EMIT("vmul.f32        q0, q2, q4")            // q0 = (2*y*L*log2(E)+R)*x = log(v[i])*log2(E) * x
            __ASM_EMIT("vmul.f32        q1, q3, q5")
            POW2_CORE_X8
            __ASM_EMIT("subs            %[count], $8")
            __ASM_EMIT("vst1.32         {q0-q1}, [%[dst]]!")
            __ASM_EMIT("bhs             1b")

            __ASM_EMIT("2:")
            __ASM_EMIT("adds            %[count], $4")
            __ASM_EMIT("blt             4f")

            // x4 block
            __ASM_EMIT("vld1.32         {q0}, [%[src]]!")
            // log(v[i])
            LOGN_CORE_X4
            __ASM_EMIT("vld1.32         {q4}, [%[x]]!")
            __ASM_EMIT("vmla.f32        q2, q0, q14")           // q2 = 2*y*L*log2(E)+R
            __ASM_EMIT("vmul.f32        q0, q2, q4")            // q0 = (2*y*L*log2(E)+R)*x = log(v[i])*log2(E) * x
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
            __ASM_EMIT("vldm            %[x]!, {s6}")
            __ASM_EMIT("6:")
            __ASM_EMIT("tst             %[count], $2")
            __ASM_EMIT("beq             8f")
            __ASM_EMIT("vldm            %[src], {d0}")
            __ASM_EMIT("vldm            %[x], {d2}")
            __ASM_EMIT("8:")

            LOGN_CORE_X4
            __ASM_EMIT("vmla.f32        q2, q0, q14")           // q2 = 2*y*L*log2(E)+R
            __ASM_EMIT("vmul.f32        q0, q2, q1")            // q0 = (2*y*L*log2(E)+R)*x = log(v[i])*log2(E) * x
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

            : [dst] "+r" (dst), [src] "+r" (v), [x] "+r" (x), [count] "+r" (count)
            : [L2C] "r" (&LOG2_CONST[0]),
              [LOGC] "r" (&LOGB_C[0]),
              [E2C] "r" (&EXP2_CONST[0]),
              [V] "r" (&v)
            : "cc", "memory",
              "q0", "q1", "q2", "q3",
              "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11",
              "q12", "q13", "q14", "q15"
        );
    }

    void powvx1(float *v, const float *x, size_t count)
    {
//        for (size_t i=0; i<count; ++i)
//            dst[i] = expf(x[i] * logf(v[i]));
        IF_ARCH_ARM(float *dst = v);

        ARCH_ARM_ASM(
            __ASM_EMIT("subs            %[count], $8")
            __ASM_EMIT("vldm            %[LOGC], {q14}")
            __ASM_EMIT("blo             2f")

            // x8 blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("vld1.32         {q0-q1}, [%[src]]!")
            // log(v[i])
            LOGN_CORE_X8
            __ASM_EMIT("vld1.32         {q4-q5}, [%[x]]!")
            __ASM_EMIT("vmla.f32        q2, q0, q14")           // q2 = 2*y*L*log2(E)+R
            __ASM_EMIT("vmla.f32        q3, q1, q14")
            __ASM_EMIT("vmul.f32        q0, q2, q4")            // q0 = (2*y*L*log2(E)+R)*x = log(v[i])*log2(E) * x
            __ASM_EMIT("vmul.f32        q1, q3, q5")
            POW2_CORE_X8
            __ASM_EMIT("subs            %[count], $8")
            __ASM_EMIT("vst1.32         {q0-q1}, [%[dst]]!")
            __ASM_EMIT("bhs             1b")

            __ASM_EMIT("2:")
            __ASM_EMIT("adds            %[count], $4")
            __ASM_EMIT("blt             4f")

            // x4 block
            __ASM_EMIT("vld1.32         {q0}, [%[src]]!")
            // log(v[i])
            LOGN_CORE_X4
            __ASM_EMIT("vld1.32         {q4}, [%[x]]!")
            __ASM_EMIT("vmla.f32        q2, q0, q14")           // q2 = 2*y*L*log2(E)+R
            __ASM_EMIT("vmul.f32        q0, q2, q4")            // q0 = (2*y*L*log2(E)+R)*x = log(v[i])*log2(E) * x
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
            __ASM_EMIT("vldm            %[x]!, {s6}")
            __ASM_EMIT("6:")
            __ASM_EMIT("tst             %[count], $2")
            __ASM_EMIT("beq             8f")
            __ASM_EMIT("vldm            %[src], {d0}")
            __ASM_EMIT("vldm            %[x], {d2}")
            __ASM_EMIT("8:")

            LOGN_CORE_X4
            __ASM_EMIT("vmla.f32        q2, q0, q14")           // q2 = 2*y*L*log2(E)+R
            __ASM_EMIT("vmul.f32        q0, q2, q1")            // q0 = (2*y*L*log2(E)+R)*x = log(v[i])*log2(E) * x
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

            : [dst] "+r" (dst), [src] "+r" (v), [x] "+r" (x), [count] "+r" (count)
            : [L2C] "r" (&LOG2_CONST[0]),
              [LOGC] "r" (&LOGB_C[0]),
              [E2C] "r" (&EXP2_CONST[0]),
              [V] "r" (&v)
            : "cc", "memory",
              "q0", "q1", "q2", "q3",
              "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11",
              "q12", "q13", "q14", "q15"
        );
    }
}


#endif /* DSP_ARCH_ARM_NEON_D32_PMATH_POW_H_ */
