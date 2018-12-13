/*
 * exp.h
 *
 *  Created on: 10 дек. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_ARM_NEON_D32_PMATH_EXP_H_
#define DSP_ARCH_ARM_NEON_D32_PMATH_EXP_H_

#ifndef DSP_ARCH_ARM_NEON_32_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_ARM_NEON_32_IMPL */

namespace neon_d32
{
#define X4VEC(x)    x, x, x, x

IF_ARCH_ARM(
    static const uint32_t EXP2_CONST[] __lsp_aligned16 =
    {
        X4VEC(0x7f),       // 0x7f
        X4VEC(0x3f317218), // ln(2)
        X4VEC(0x40e00000), // C5 = 7!/6! = 7.0000000000000000
        X4VEC(0x42280000), // C4 = 7!/5! = 42.0000000000000000
        X4VEC(0x43520000), // C3 = 7!/4! = 210.0000000000000000
        X4VEC(0x44520000), // C2 = 7!/3! = 840.0000000000000000
        X4VEC(0x451d8000), // C1 = 7!/2! = 2520.0000000000000000
        X4VEC(0x459d8000), // C0 = 7!/1! = 5040.0000000000000000
        X4VEC(0x39500d01), // 1/7! = 0.0001984127011383
        X4VEC(0x3f800000), // 1.0
    };

    static const float EXP_LOG2E[] __lsp_aligned16 =
    {
        X4VEC(M_LOG2E)
    };
)

#undef X4VEC

#define POW2_CORE_X8 \
    __ASM_EMIT("vldm            %[E2C]!, {q8-q13}")             /* q8 = 0x7f, q9 = ln(2), q10 = C5, q11 = C4, q12 = C3, q13 = C2, E2C += 6*4 */ \
    /* q0 = x */ \
    __ASM_EMIT("vshr.s32        q2, q0, $31")                   /* q2   = [ x < 0 ] */ \
    __ASM_EMIT("vshr.s32        q3, q1, $31") \
    __ASM_EMIT("vabs.f32        q0, q0")                        /* q0   = XP = fabs(x) */ \
    __ASM_EMIT("vabs.f32        q1, q1") \
    __ASM_EMIT("vcvt.u32.f32    q4, q0")                        /* q4   = R = int(x) */ \
    __ASM_EMIT("vcvt.u32.f32    q5, q1") \
    __ASM_EMIT("vadd.u32        q6, q4, q8")                    /* q6   = R + 127 */ \
    __ASM_EMIT("vadd.u32        q7, q5, q8") \
    __ASM_EMIT("vcvt.f32.u32    q4, q4")                        /* q4   = float(R) */ \
    __ASM_EMIT("vcvt.f32.u32    q5, q5") \
    __ASM_EMIT("vshl.u32        q6, q6, $23")                   /* q6   = 1 << (R + 127) */ \
    __ASM_EMIT("vshl.u32        q7, q7, $23") \
    __ASM_EMIT("vsub.f32        q0, q0, q4")                    /* q0   = XP - float(R) */ \
    __ASM_EMIT("vsub.f32        q1, q1, q5") \
    __ASM_EMIT("vmul.f32        q0, q0, q9")                    /* q0   = X = ln(2) * (XP - float(R)) */ \
    __ASM_EMIT("vmul.f32        q1, q1, q9") \
    /* q0 = X, q2 = [ x < 0 ], q6 = 1 << R */ \
    __ASM_EMIT("vadd.f32        q4, q0, q10")                   /* q4   = C5+X */ \
    __ASM_EMIT("vadd.f32        q5, q1, q10") \
    __ASM_EMIT("vmul.f32        q4, q4, q0")                    /* q4   = X*(C5+X) */ \
    __ASM_EMIT("vmul.f32        q5, q5, q1") \
    __ASM_EMIT("vadd.f32        q4, q4, q11")                   /* q4   = C4+X*(C5+X) */ \
    __ASM_EMIT("vadd.f32        q5, q5, q11") \
    __ASM_EMIT("vmul.f32        q4, q4, q0")                    /* q4   = X*(C4+X*(C5+X)) */ \
    __ASM_EMIT("vmul.f32        q5, q5, q1") \
    __ASM_EMIT("vadd.f32        q4, q4, q12")                   /* q4   = C3+X*(C4+X*(C5+X)) */ \
    __ASM_EMIT("vadd.f32        q5, q5, q12") \
    __ASM_EMIT("vmul.f32        q4, q4, q0")                    /* q4   = X*(C3+X*(C4+X*(C5+X))) */ \
    __ASM_EMIT("vmul.f32        q5, q5, q1") \
    __ASM_EMIT("vldm            %[E2C], {q8-q11}")              /* q8   = C1, q9 = C0, q10 = 1/7!, q11 = 1.0 */ \
    __ASM_EMIT("vadd.f32        q4, q4, q13")                   /* q4   = C2+X*(C3+X*(C4+X*(C5+X))) */ \
    __ASM_EMIT("vadd.f32        q5, q5, q13") \
    __ASM_EMIT("vmul.f32        q4, q4, q0")                    /* q4   = X*(C2+X*(C3+X*(C4+X*(C5+X)))) */ \
    __ASM_EMIT("vmul.f32        q5, q5, q1") \
    __ASM_EMIT("vadd.f32        q4, q4, q8")                    /* q4   = C1+X*(C2+X*(C3+X*(C4+X*(C5+X)))) */ \
    __ASM_EMIT("vadd.f32        q5, q5, q8") \
    __ASM_EMIT("vmul.f32        q4, q4, q0")                    /* q4   = X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X))))) */ \
    __ASM_EMIT("vmul.f32        q5, q5, q1") \
    __ASM_EMIT("vadd.f32        q4, q4, q9")                    /* q4   = C0+X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X))))) */ \
    __ASM_EMIT("vadd.f32        q5, q5, q9") \
    __ASM_EMIT("vmul.f32        q4, q4, q0")                    /* q4   = X*(C0+X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X)))))) */ \
    __ASM_EMIT("vmul.f32        q5, q5, q1") \
    __ASM_EMIT("vmul.f32        q4, q4, q10")                   /* q4   = 1/7! * X*(C0+X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X)))))) */ \
    __ASM_EMIT("vmul.f32        q5, q5, q10") \
    __ASM_EMIT("vadd.f32        q4, q4, q11")                   /* q4   = 1 + 1/7! * X*(C0+X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X)))))) */ \
    __ASM_EMIT("vadd.f32        q5, q5, q11") \
    __ASM_EMIT("vmul.f32        q4, q4, q6")                    /* q4   = E = (1 << R) * (1 + 1/7! * X*(C0+X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X))))))) */ \
    __ASM_EMIT("vmul.f32        q5, q5, q7") \
    __ASM_EMIT("sub             %[E2C], $0x60")                 /* E2C -= 6*4 */ \
    /* Calc reciprocals into q0 */ \
    __ASM_EMIT("vrecpe.f32      q8, q4")                        /* q8   = e2 */ \
    __ASM_EMIT("vrecpe.f32      q9, q5") \
    __ASM_EMIT("vrecps.f32      q10, q8, q4")                   /* q10  = (2 - E*e2) */ \
    __ASM_EMIT("vrecps.f32      q11, q9, q5") \
    __ASM_EMIT("vmul.f32        q8, q10, q8")                   /* q8   = e2' = e2 * (2 - E*e2) */ \
    __ASM_EMIT("vmul.f32        q9, q11, q9") \
    __ASM_EMIT("vrecps.f32      q10, q8, q4")                   /* q10  = (2 - E*e2') */ \
    __ASM_EMIT("vrecps.f32      q11, q9, q5") \
    __ASM_EMIT("vmul.f32        q0, q10, q8")                   /* q0   = 1/E = e2" = e2' * (2 - E*e2) */  \
    __ASM_EMIT("vmul.f32        q1, q11, q9") \
    /* Perform conditional output */ \
    __ASM_EMIT("vbif            q0, q4, q2")                    /* q0   = ((1/E) & [ x < 0 ]) | (E & [x >= 0]) */ \
    __ASM_EMIT("vbif            q1, q5, q3")

#define POW2_CORE_X4 \
    __ASM_EMIT("vldm            %[E2C]!, {q8-q13}")             /* q8 = 0x7f, q9 = ln(2), q10 = C5, q11 = C4, q12 = C3, q13 = C2, E2C += 6*4 */ \
    /* q0 = x */ \
    __ASM_EMIT("vshr.s32        q2, q0, $31")                   /* q2   = [ x < 0 ] */ \
    __ASM_EMIT("vabs.f32        q0, q0")                        /* q0   = XP = fabs(x) */ \
    __ASM_EMIT("vcvt.u32.f32    q4, q0")                        /* q4   = R = int(x) */ \
    __ASM_EMIT("vadd.u32        q6, q4, q8")                    /* q6   = R + 127 */ \
    __ASM_EMIT("vcvt.f32.u32    q4, q4")                        /* q4   = float(R) */ \
    __ASM_EMIT("vshl.u32        q6, q6, $23")                   /* q6   = 1 << (R + 127) */ \
    __ASM_EMIT("vsub.f32        q0, q0, q4")                    /* q0   = XP - float(R) */ \
    __ASM_EMIT("vmul.f32        q0, q0, q9")                    /* q0   = X = ln(2) * (XP - float(R)) */ \
    /* q0 = X, q2 = [ x < 0 ], q6 = 1 << R */ \
    __ASM_EMIT("vadd.f32        q4, q0, q10")                   /* q4   = C5+X */ \
    __ASM_EMIT("vmul.f32        q4, q4, q0")                    /* q4   = X*(C5+X) */ \
    __ASM_EMIT("vadd.f32        q4, q4, q11")                   /* q4   = C4+X*(C5+X) */ \
    __ASM_EMIT("vmul.f32        q4, q4, q0")                    /* q4   = X*(C4+X*(C5+X)) */ \
    __ASM_EMIT("vadd.f32        q4, q4, q12")                   /* q4   = C3+X*(C4+X*(C5+X)) */ \
    __ASM_EMIT("vmul.f32        q4, q4, q0")                    /* q4   = X*(C3+X*(C4+X*(C5+X))) */ \
    __ASM_EMIT("vldm            %[E2C], {q8-q11}")              /* q8   = C1, q9 = C0, q10 = 1/7!, q11 = 1.0 */ \
    __ASM_EMIT("vadd.f32        q4, q4, q13")                   /* q4   = C2+X*(C3+X*(C4+X*(C5+X))) */ \
    __ASM_EMIT("vmul.f32        q4, q4, q0")                    /* q4   = X*(C2+X*(C3+X*(C4+X*(C5+X)))) */ \
    __ASM_EMIT("vadd.f32        q4, q4, q8")                    /* q4   = C1+X*(C2+X*(C3+X*(C4+X*(C5+X)))) */ \
    __ASM_EMIT("vmul.f32        q4, q4, q0")                    /* q4   = X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X))))) */ \
    __ASM_EMIT("vadd.f32        q4, q4, q9")                    /* q4   = C0+X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X))))) */ \
    __ASM_EMIT("vmul.f32        q4, q4, q0")                    /* q4   = X*(C0+X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X)))))) */ \
    __ASM_EMIT("vmul.f32        q4, q4, q10")                   /* q4   = 1/7! * X*(C0+X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X)))))) */ \
    __ASM_EMIT("vadd.f32        q4, q4, q11")                   /* q4   = 1 + 1/7! * X*(C0+X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X)))))) */ \
    __ASM_EMIT("vmul.f32        q4, q4, q6")                    /* q4   = E = (1 << R) * (1 + 1/7! * X*(C0+X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X))))))) */ \
    __ASM_EMIT("sub             %[E2C], $0x60")                 /* E2C -= 6*4 */ \
    /* Calc reciprocals into q0 */ \
    __ASM_EMIT("vrecpe.f32      q8, q4")                        /* q8   = e2 */ \
    __ASM_EMIT("vrecps.f32      q10, q8, q4")                   /* q10  = (2 - E*e2) */ \
    __ASM_EMIT("vmul.f32        q8, q10, q8")                   /* q8   = e2' = e2 * (2 - E*e2) */ \
    __ASM_EMIT("vrecps.f32      q10, q8, q4")                   /* q10  = (2 - E*e2') */ \
    __ASM_EMIT("vmul.f32        q0, q10, q8")                   /* q0   = 1/E = e2" = e2' * (2 - E*e2) */  \
    /* Perform conditional output */ \
    __ASM_EMIT("vbif            q0, q4, q2")                    /* q0   = ((1/E) & [ x < 0 ]) | (E & [x >= 0]) */

    void exp2(float *dst, const float *src, size_t count)
    {
        ARCH_ARM_ASM(
            __ASM_EMIT("subs            %[count], $8")
            __ASM_EMIT("vldm            %[LOG2E], {q15}")
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
            : [E2C] "r" (&EXP2_CONST[0]),
              [LOG2E] "r" (&EXP_LOG2E[0])
            : "cc", "memory",
              "q0", "q1", "q2", "q3",
              "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11",
              "q12", "q13", "q14", "q15"
        );
    }

    void exp1(float *dst, size_t count)
    {
        IF_ARCH_ARM( float *src = dst );

        ARCH_ARM_ASM(
            __ASM_EMIT("subs            %[count], $8")
            __ASM_EMIT("vldm            %[LOG2E], {q15}")
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
            : [E2C] "r" (&EXP2_CONST[0]),
              [LOG2E] "r" (&EXP_LOG2E[0])
            : "cc", "memory",
              "q0", "q1", "q2", "q3",
              "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11",
              "q12", "q13", "q14", "q15"
        );
    }

}

#endif /* DSP_ARCH_ARM_NEON_D32_PMATH_EXP_H_ */
