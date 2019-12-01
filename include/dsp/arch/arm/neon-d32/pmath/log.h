/*
 * log.h
 *
 *  Created on: 12 дек. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_ARM_NEON_D32_PMATH_LOG_H_
#define DSP_ARCH_ARM_NEON_D32_PMATH_LOG_H_

#ifndef DSP_ARCH_ARM_NEON_32_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_ARM_NEON_32_IMPL */

namespace neon_d32
{
#define X4VEC(x)    x, x, x, x

IF_ARCH_ARM(
    static const uint32_t LOG2_CONST[] __lsp_aligned16 =
    {
        X4VEC(0x007fffff), // frac
        X4VEC(0x0000007f), // 127
        X4VEC(0x3d888889), // C0 = 1/15 = 0.0666666701436043
        X4VEC(0x3d9d89d9), // C1 = 1/13 = 0.0769230797886848
        X4VEC(0x3dba2e8c), // C2 = 1/11 = 0.0909090936183929
        X4VEC(0x3f800000), // 1.0f = C7

        X4VEC(0x3de38e39), // C3 = 1/9 = 0.1111111119389534
        X4VEC(0x3e124925), // C4 = 1/7 = 0.1428571492433548
        X4VEC(0x3e4ccccd), // C5 = 1/5 = 0.2000000029802322
        X4VEC(0x3eaaaaab), // C6 = 1/3 = 0.3333333432674408
    };

    static const float LOGB_C[] __lsp_aligned16 =
    {
        X4VEC(2.0f * M_LOG2E),
        X4VEC(2.0f * M_LOG2E)
    };

    static const float LOGE_C[] __lsp_aligned16 =
    {
        X4VEC(1.0f / M_LOG2E),
        X4VEC(1.0f / M_LOG2E)
    };

    static const float LOGD_C[] __lsp_aligned16 =
    {
        X4VEC(2.0f * M_LOG10E),
        X4VEC(0.301029995663981f) // 1/log2(10)
    };
)

#undef X4VEC

#define LOGN_CORE_X8 \
    /* q0 = x */ \
    __ASM_EMIT("vldm            %[L2C]!, {q8-q13}")             /* q8   = F_MASK, q9 = 127, q10 = C0, q11=C1, q12=C2, q13=1.0f */ \
    __ASM_EMIT("vshr.u32        q2, q0, $23")                   /* q2   = ilog2(x) + 127 */ \
    __ASM_EMIT("vshr.u32        q3, q1, $23")                    \
    __ASM_EMIT("vand            q0, q0, q8")                    /* q0   = x & F_MASK */ \
    __ASM_EMIT("vand            q1, q1, q8")                     \
    __ASM_EMIT("vsub.i32        q2, q2, q9")                    /* q2   = r = ilog2(x) */ \
    __ASM_EMIT("vsub.i32        q3, q3, q9")                     \
    __ASM_EMIT("vorr            q0, q0, q13")                   /* q0   = X = (x & F_MASK) | 1.0f */ \
    __ASM_EMIT("vorr            q1, q1, q13")                    \
    __ASM_EMIT("vcvt.f32.s32    q2, q2")                        /* q2   = R = float(r) */ \
    __ASM_EMIT("vcvt.f32.s32    q3, q3")                         \
    __ASM_EMIT("vadd.f32        q4, q0, q13")                   /* q4   = XB = X + 1 */ \
    __ASM_EMIT("vadd.f32        q5, q1, q13")                    \
    __ASM_EMIT("vsub.f32        q0, q0, q13")                   /* q0   = XT = X - 1 */ \
    __ASM_EMIT("vsub.f32        q1, q1, q13")                    \
    __ASM_EMIT("vrecpe.f32      q6, q4")                        /* q6   = xb */ \
    __ASM_EMIT("vrecpe.f32      q7, q5")                         \
    __ASM_EMIT("vrecps.f32      q8, q6, q4")                    /* q8   = (2 - XB*xb) */ \
    __ASM_EMIT("vrecps.f32      q9, q7, q5")                     \
    __ASM_EMIT("vmul.f32        q6, q8, q6")                    /* q6   = xb' = xb * (2 - XB*xb) */ \
    __ASM_EMIT("vmul.f32        q7, q9, q7")                     \
    __ASM_EMIT("vrecps.f32      q8, q6, q4")                    /* q8   = (2 - XB*xb') */ \
    __ASM_EMIT("vrecps.f32      q9, q7, q5")                     \
    __ASM_EMIT("vmul.f32        q4, q8, q6")                    /* q4   = 1/(X+1) = xb" = xb' * (2 - XB*xb) */  \
    __ASM_EMIT("vmul.f32        q5, q9, q7")                     \
    __ASM_EMIT("vmul.f32        q0, q0, q4")                    /* q0   = y = (X-1)/(X+1) */ \
    __ASM_EMIT("vmul.f32        q1, q1, q5")                     \
    __ASM_EMIT("vmul.f32        q4, q0, q0")                    /* q4   = Y = y*y */ \
    __ASM_EMIT("vmul.f32        q5, q1, q1")                     \
    \
    /* q0 = y, q2 = R, q4 = Y */ \
    __ASM_EMIT("vmul.f32        q6, q4, q10")                   /* q6   = C0*Y */ \
    __ASM_EMIT("vmul.f32        q7, q5, q10")                    \
    __ASM_EMIT("vadd.f32        q6, q6, q11")                   /* q6   = C1+C0*Y */ \
    __ASM_EMIT("vadd.f32        q7, q7, q11")                    \
    __ASM_EMIT("vmul.f32        q6, q6, q4")                    /* q6   = Y*(C1+C0*Y) */ \
    __ASM_EMIT("vmul.f32        q7, q7, q5")                     \
    __ASM_EMIT("vadd.f32        q6, q6, q12")                   /* q6   = C2+Y*(C1+C0*Y) */ \
    __ASM_EMIT("vadd.f32        q7, q7, q12")                    \
    __ASM_EMIT("vldm            %[L2C], {q8-q11}")              /* q8   = C3, q9 = C4, q10 = C5, q11 = C6, q13 = C7 */ \
    __ASM_EMIT("vmul.f32        q6, q6, q4")                    /* q6   = Y*(C2+Y*(C1+C0*Y)) */ \
    __ASM_EMIT("vmul.f32        q7, q7, q5")                     \
    __ASM_EMIT("vadd.f32        q6, q6, q8")                    /* q6   = C3+Y*(C2+Y*(C1+C0*Y)) */ \
    __ASM_EMIT("vadd.f32        q7, q7, q8")                     \
    __ASM_EMIT("vmul.f32        q6, q6, q4")                    /* q6   = Y*(C3+Y*(C2+Y*(C1+C0*Y))) */ \
    __ASM_EMIT("vmul.f32        q7, q7, q5")                     \
    __ASM_EMIT("vadd.f32        q6, q6, q9")                    /* q6   = C4+Y*(C3+Y*(C2+Y*(C1+C0*Y))) */ \
    __ASM_EMIT("vadd.f32        q7, q7, q9")                     \
    __ASM_EMIT("vmul.f32        q6, q6, q4")                    /* q6   = Y*(C4+Y*(C3+Y*(C2+Y*(C1+C0*Y)))) */ \
    __ASM_EMIT("vmul.f32        q7, q7, q5")                     \
    __ASM_EMIT("vadd.f32        q6, q6, q10")                   /* q6   = C5+Y*(C4+Y*(C3+Y*(C2+Y*(C1+C0*Y)))) */ \
    __ASM_EMIT("vadd.f32        q7, q7, q10")                    \
    __ASM_EMIT("vmul.f32        q6, q6, q4")                    /* q6   = Y*(C5+Y*(C4+Y*(C3+Y*(C2+Y*(C1+C0*Y))))) */ \
    __ASM_EMIT("vmul.f32        q7, q7, q5")                     \
    __ASM_EMIT("vadd.f32        q6, q6, q11")                   /* q6   = C6+Y*(C5+Y*(C4+Y*(C3+Y*(C2+Y*(C1+C0*Y))))) */ \
    __ASM_EMIT("vadd.f32        q7, q7, q11")                    \
    __ASM_EMIT("vmul.f32        q6, q6, q4")                    /* q6   = Y*(C6+Y*(C5+Y*(C4+Y*(C3+Y*(C2+Y*(C1+C0*Y)))))) */ \
    __ASM_EMIT("vmul.f32        q7, q7, q5")                     \
    __ASM_EMIT("vadd.f32        q6, q6, q13")                   /* q6   = C7+Y*(C6+Y*(C5+Y*(C4+Y*(C3+Y*(C2+Y*(C1+C0*Y)))))) */ \
    __ASM_EMIT("vadd.f32        q7, q7, q13")                    \
    __ASM_EMIT("sub             %[L2C], $0x60")                  \
    __ASM_EMIT("vmul.f32        q0, q0, q6")                    /* q0   = y*(C7+Y*(C6+Y*(C5+Y*(C4+Y*(C3+Y*(C2+Y*(C1+C0*Y))))))) */ \
    __ASM_EMIT("vmul.f32        q1, q1, q7")                     \
    /* q0 = y*L, q2 = R */

#define LOGN_CORE_X4 \
    /* q0 = x */ \
    __ASM_EMIT("vldm            %[L2C]!, {q8-q13}")             /* q8   = F_MASK, q9 = 127, q10 = C0, q11=C1, q12=C2, q13=1.0f */ \
    __ASM_EMIT("vshr.u32        q2, q0, $23")                   /* q2   = ilog2(x) + 127 */ \
    __ASM_EMIT("vand            q0, q0, q8")                    /* q0   = x & F_MASK */ \
    __ASM_EMIT("vsub.i32        q2, q2, q9")                    /* q2   = r = ilog2(x) */ \
    __ASM_EMIT("vorr            q0, q0, q13")                   /* q0   = X = (x & F_MASK) | 1.0f */ \
    __ASM_EMIT("vcvt.f32.s32    q2, q2")                        /* q2   = R = float(r) */ \
    __ASM_EMIT("vadd.f32        q4, q0, q13")                   /* q4   = XB = X + 1 */ \
    __ASM_EMIT("vsub.f32        q0, q0, q13")                   /* q0   = XT = X - 1 */ \
    __ASM_EMIT("vrecpe.f32      q6, q4")                        /* q6   = xb */ \
    __ASM_EMIT("vrecps.f32      q8, q6, q4")                    /* q8   = (2 - XB*xb) */ \
    __ASM_EMIT("vmul.f32        q6, q8, q6")                    /* q6   = xb' = xb * (2 - XB*xb) */ \
    __ASM_EMIT("vrecps.f32      q8, q6, q4")                    /* q8   = (2 - XB*xb') */ \
    __ASM_EMIT("vmul.f32        q4, q8, q6")                    /* q4   = 1/(X+1) = xb" = xb' * (2 - XB*xb) */  \
    __ASM_EMIT("vmul.f32        q0, q0, q4")                    /* q0   = y = (X-1)/(X+1) */ \
    __ASM_EMIT("vmul.f32        q4, q0, q0")                    /* q4   = Y = y*y */ \
    \
    /* q0 = y, q2 = R, q4 = Y */ \
    __ASM_EMIT("vmul.f32        q6, q4, q10")                   /* q6   = C0*Y */ \
    __ASM_EMIT("vadd.f32        q6, q6, q11")                   /* q6   = C1+C0*Y */ \
    __ASM_EMIT("vmul.f32        q6, q6, q4")                    /* q6   = Y*(C1+C0*Y) */ \
    __ASM_EMIT("vadd.f32        q6, q6, q12")                   /* q6   = C2+Y*(C1+C0*Y) */ \
    __ASM_EMIT("vldm            %[L2C], {q8-q11}")              /* q8   = C3, q9 = C4, q10 = C5, q11 = C6, q13 = C7 */ \
    __ASM_EMIT("vmul.f32        q6, q6, q4")                    /* q6   = Y*(C2+Y*(C1+C0*Y)) */ \
    __ASM_EMIT("vadd.f32        q6, q6, q8")                    /* q6   = C3+Y*(C2+Y*(C1+C0*Y)) */ \
    __ASM_EMIT("vmul.f32        q6, q6, q4")                    /* q6   = Y*(C3+Y*(C2+Y*(C1+C0*Y))) */ \
    __ASM_EMIT("vadd.f32        q6, q6, q9")                    /* q6   = C4+Y*(C3+Y*(C2+Y*(C1+C0*Y))) */ \
    __ASM_EMIT("vmul.f32        q6, q6, q4")                    /* q6   = Y*(C4+Y*(C3+Y*(C2+Y*(C1+C0*Y)))) */ \
    __ASM_EMIT("vadd.f32        q6, q6, q10")                   /* q6   = C5+Y*(C4+Y*(C3+Y*(C2+Y*(C1+C0*Y)))) */ \
    __ASM_EMIT("vmul.f32        q6, q6, q4")                    /* q6   = Y*(C5+Y*(C4+Y*(C3+Y*(C2+Y*(C1+C0*Y))))) */ \
    __ASM_EMIT("vadd.f32        q6, q6, q11")                   /* q6   = C6+Y*(C5+Y*(C4+Y*(C3+Y*(C2+Y*(C1+C0*Y))))) */ \
    __ASM_EMIT("vmul.f32        q6, q6, q4")                    /* q6   = Y*(C6+Y*(C5+Y*(C4+Y*(C3+Y*(C2+Y*(C1+C0*Y)))))) */ \
    __ASM_EMIT("vadd.f32        q6, q6, q13")                   /* q6   = C7+Y*(C6+Y*(C5+Y*(C4+Y*(C3+Y*(C2+Y*(C1+C0*Y)))))) */ \
    __ASM_EMIT("sub             %[L2C], $0x60")                  \
    __ASM_EMIT("vmul.f32        q0, q0, q6")                    /* q0   = y*(C7+Y*(C6+Y*(C5+Y*(C4+Y*(C3+Y*(C2+Y*(C1+C0*Y))))))) */ \
    /* q0 = y*L, q2 = R */

    void logb2(float *dst, const float *src, size_t count)
    {
        ARCH_ARM_ASM(
            __ASM_EMIT("subs            %[count], $8")
            __ASM_EMIT("vldm            %[LOGC], {q14-q15}")
            __ASM_EMIT("blo             2f")
            // x8 blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("vld1.32         {q0-q1}, [%[src]]!")
            LOGN_CORE_X8
            __ASM_EMIT("vmla.f32        q2, q0, q14")           // q2 = R + 2*y*L*M_LOG2E
            __ASM_EMIT("vmla.f32        q3, q1, q15")
            __ASM_EMIT("subs            %[count], $8")
            __ASM_EMIT("vst1.32         {q2-q3}, [%[dst]]!")
            __ASM_EMIT("bhs             1b")
            // x4 block
            __ASM_EMIT("2:")
            __ASM_EMIT("adds            %[count], $4")
            __ASM_EMIT("blt             4f")
            __ASM_EMIT("vld1.32         {q0}, [%[src]]!")
            LOGN_CORE_X4
            __ASM_EMIT("vmla.f32        q2, q0, q14")           // q2 = R + 2*y*L*M_LOG2E
            __ASM_EMIT("sub             %[count], $4")
            __ASM_EMIT("vst1.32         {q2}, [%[dst]]!")
            // Tail: 1x-3x block
            __ASM_EMIT("4:")
            __ASM_EMIT("adds            %[count], $4")
            __ASM_EMIT("bls             12f")
            __ASM_EMIT("tst             %[count], $1")
            __ASM_EMIT("beq             6f")
            __ASM_EMIT("vld1.32         {d0[0]}, [%[src]]!")
            __ASM_EMIT("6:")
            __ASM_EMIT("tst             %[count], $2")
            __ASM_EMIT("beq             8f")
            __ASM_EMIT("vld1.32         {d1}, [%[src]]")
            __ASM_EMIT("8:")
            LOGN_CORE_X4
            __ASM_EMIT("vmla.f32        q2, q0, q14")           // q2 = R + 2*y*L*M_LOG2E
            __ASM_EMIT("tst             %[count], $1")
            __ASM_EMIT("beq             10f")
            __ASM_EMIT("vst1.32         {d4[0]}, [%[dst]]!")
            __ASM_EMIT("10:")
            __ASM_EMIT("tst             %[count], $2")
            __ASM_EMIT("beq             12f")
            __ASM_EMIT("vst1.32         {d5}, [%[dst]]")
            // End
            __ASM_EMIT("12:")

            : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
            : [L2C] "r" (&LOG2_CONST[0]),
              [LOGC] "r" (&LOGB_C[0])
            : "cc", "memory",
              "q0", "q1", "q2", "q3",
              "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11",
              "q12", "q13", "q14", "q15"
        );
    }

    void logb1(float *dst, size_t count)
    {
        ARCH_ARM_ASM(
            __ASM_EMIT("subs            %[count], $8")
            __ASM_EMIT("vldm            %[LOGC], {q14-q15}")
            __ASM_EMIT("blo             2f")
            // x8 blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("vld1.32         {q0-q1}, [%[dst]]")
            LOGN_CORE_X8
            __ASM_EMIT("vmla.f32        q2, q0, q14")           // q2 = R + 2*y*L*log2(E)
            __ASM_EMIT("vmla.f32        q3, q1, q15")
            __ASM_EMIT("subs            %[count], $8")
            __ASM_EMIT("vst1.32         {q2-q3}, [%[dst]]!")
            __ASM_EMIT("bhs             1b")
            // x4 block
            __ASM_EMIT("2:")
            __ASM_EMIT("adds            %[count], $4")
            __ASM_EMIT("blt             4f")
            __ASM_EMIT("vld1.32         {q0}, [%[dst]]")
            LOGN_CORE_X4
            __ASM_EMIT("vmla.f32        q2, q0, q14")           // q2 = R + 2*y*L*log2(E)
            __ASM_EMIT("sub             %[count], $4")
            __ASM_EMIT("vst1.32         {q2}, [%[dst]]!")
            // Tail: 1x-3x block
            __ASM_EMIT("4:")
            __ASM_EMIT("adds            %[count], $4")
            __ASM_EMIT("bls             12f")
            __ASM_EMIT("tst             %[count], $1")
            __ASM_EMIT("beq             6f")
            __ASM_EMIT("vld1.32         {d0[0]}, [%[dst]]!")
            __ASM_EMIT("6:")
            __ASM_EMIT("tst             %[count], $2")
            __ASM_EMIT("beq             8f")
            __ASM_EMIT("vld1.32         {d1}, [%[dst]]")
            __ASM_EMIT("8:")
            LOGN_CORE_X4
            __ASM_EMIT("vmla.f32        q2, q0, q14")           // q2 = R + 2*y*L*log2(E)
            __ASM_EMIT("tst             %[count], $1")
            __ASM_EMIT("beq             10f")
            __ASM_EMIT("sub             %[dst], $0x04")
            __ASM_EMIT("vst1.32         {d4[0]}, [%[dst]]!")
            __ASM_EMIT("10:")
            __ASM_EMIT("tst             %[count], $2")
            __ASM_EMIT("beq             12f")
            __ASM_EMIT("vst1.32         {d5}, [%[dst]]")
            // End
            __ASM_EMIT("12:")

            : [dst] "+r" (dst), [count] "+r" (count)
            : [L2C] "r" (&LOG2_CONST[0]),
              [LOGC] "r" (&LOGB_C[0])
            : "cc", "memory",
              "q0", "q1", "q2", "q3",
              "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11",
              "q12", "q13", "q14", "q15"
        );
    }

    void loge2(float *dst, const float *src, size_t count)
    {
        ARCH_ARM_ASM(
            __ASM_EMIT("subs            %[count], $8")
            __ASM_EMIT("vldm            %[LOGC], {q14-q15}")
            __ASM_EMIT("blo             2f")
            // x8 blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("vld1.32         {q0-q1}, [%[src]]!")
            LOGN_CORE_X8
            __ASM_EMIT("vadd.f32        q0, q0, q0")            // q0 = 2*y*L
            __ASM_EMIT("vadd.f32        q1, q1, q1")
            __ASM_EMIT("vmla.f32        q0, q2, q14")           // q0 = 2*y*L + R/log2(E)
            __ASM_EMIT("vmla.f32        q1, q3, q15")
            __ASM_EMIT("subs            %[count], $8")
            __ASM_EMIT("vst1.32         {q0-q1}, [%[dst]]!")
            __ASM_EMIT("bhs             1b")
            // x4 block
            __ASM_EMIT("2:")
            __ASM_EMIT("adds            %[count], $4")
            __ASM_EMIT("blt             4f")
            __ASM_EMIT("vld1.32         {q0}, [%[src]]!")
            LOGN_CORE_X4
            __ASM_EMIT("vadd.f32        q0, q0, q0")            // q0 = 2*y*L
            __ASM_EMIT("vmla.f32        q0, q2, q14")           // q0 = 2*y*L + R/log2(E)
            __ASM_EMIT("sub             %[count], $4")
            __ASM_EMIT("vst1.32         {q0}, [%[dst]]!")
            // Tail: 1x-3x block
            __ASM_EMIT("4:")
            __ASM_EMIT("adds            %[count], $4")
            __ASM_EMIT("bls             12f")
            __ASM_EMIT("tst             %[count], $1")
            __ASM_EMIT("beq             6f")
            __ASM_EMIT("vld1.32         {d0[0]}, [%[src]]!")
            __ASM_EMIT("6:")
            __ASM_EMIT("tst             %[count], $2")
            __ASM_EMIT("beq             8f")
            __ASM_EMIT("vld1.32         {d1}, [%[src]]")
            __ASM_EMIT("8:")
            LOGN_CORE_X4
            __ASM_EMIT("vadd.f32        q0, q0, q0")            // q0 = 2*y*L
            __ASM_EMIT("vmla.f32        q0, q2, q14")           // q0 = 2*y*L + R/log2(E)
            __ASM_EMIT("tst             %[count], $1")
            __ASM_EMIT("beq             10f")
            __ASM_EMIT("vst1.32         {d0[0]}, [%[dst]]!")
            __ASM_EMIT("10:")
            __ASM_EMIT("tst             %[count], $2")
            __ASM_EMIT("beq             12f")
            __ASM_EMIT("vst1.32         {d1}, [%[dst]]")
            // End
            __ASM_EMIT("12:")

            : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
            : [L2C] "r" (&LOG2_CONST[0]),
              [LOGC] "r" (&LOGE_C[0])
            : "cc", "memory",
              "q0", "q1", "q2", "q3",
              "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11",
              "q12", "q13", "q14", "q15"
        );
    }

    void loge1(float *dst, size_t count)
    {
        ARCH_ARM_ASM(
            __ASM_EMIT("subs            %[count], $8")
            __ASM_EMIT("vldm            %[LOGC], {q14-q15}")
            __ASM_EMIT("blo             2f")
            // x8 blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("vld1.32         {q0-q1}, [%[dst]]")
            LOGN_CORE_X8
            __ASM_EMIT("vadd.f32        q0, q0, q0")            // q0 = 2*y*L
            __ASM_EMIT("vadd.f32        q1, q1, q1")
            __ASM_EMIT("vmla.f32        q0, q2, q14")           // q0 = 2*y*L + R/log2(E)
            __ASM_EMIT("vmla.f32        q1, q3, q15")
            __ASM_EMIT("subs            %[count], $8")
            __ASM_EMIT("vst1.32         {q0-q1}, [%[dst]]!")
            __ASM_EMIT("bhs             1b")
            // x4 block
            __ASM_EMIT("2:")
            __ASM_EMIT("adds            %[count], $4")
            __ASM_EMIT("blt             4f")
            __ASM_EMIT("vld1.32         {q0}, [%[dst]]")
            LOGN_CORE_X4
            __ASM_EMIT("vadd.f32        q0, q0, q0")            // q0 = 2*y*L
            __ASM_EMIT("vmla.f32        q0, q2, q14")           // q0 = 2*y*L + R/log2(E)
            __ASM_EMIT("sub             %[count], $4")
            __ASM_EMIT("vst1.32         {q0}, [%[dst]]!")
            // Tail: 1x-3x block
            __ASM_EMIT("4:")
            __ASM_EMIT("adds            %[count], $4")
            __ASM_EMIT("bls             12f")
            __ASM_EMIT("tst             %[count], $1")
            __ASM_EMIT("beq             6f")
            __ASM_EMIT("vld1.32         {d0[0]}, [%[dst]]!")
            __ASM_EMIT("6:")
            __ASM_EMIT("tst             %[count], $2")
            __ASM_EMIT("beq             8f")
            __ASM_EMIT("vld1.32         {d1}, [%[dst]]")
            __ASM_EMIT("8:")
            LOGN_CORE_X4
            __ASM_EMIT("vadd.f32        q0, q0, q0")            // q0 = 2*y*L
            __ASM_EMIT("vmla.f32        q0, q2, q14")           // q0 = 2*y*L + R/log2(E)
            __ASM_EMIT("tst             %[count], $1")
            __ASM_EMIT("beq             10f")
            __ASM_EMIT("sub             %[dst], $0x04")
            __ASM_EMIT("vst1.32         {d0[0]}, [%[dst]]!")
            __ASM_EMIT("10:")
            __ASM_EMIT("tst             %[count], $2")
            __ASM_EMIT("beq             12f")
            __ASM_EMIT("vst1.32         {d1}, [%[dst]]")
            // End
            __ASM_EMIT("12:")

            : [dst] "+r" (dst), [count] "+r" (count)
            : [L2C] "r" (&LOG2_CONST[0]),
              [LOGC] "r" (&LOGE_C[0])
            : "cc", "memory",
              "q0", "q1", "q2", "q3",
              "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11",
              "q12", "q13", "q14", "q15"
        );
    }

    void logd2(float *dst, const float *src, size_t count)
    {
        ARCH_ARM_ASM(
            __ASM_EMIT("subs            %[count], $8")
            __ASM_EMIT("vldm            %[LOGC], {q14-q15}")
            __ASM_EMIT("blo             2f")
            // x8 blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("vld1.32         {q0-q1}, [%[src]]!")
            LOGN_CORE_X8
            __ASM_EMIT("vmul.f32        q0, q0, q14")           // q0 = 2*y*L*log10(E)
            __ASM_EMIT("vmul.f32        q1, q1, q14")
            __ASM_EMIT("vmla.f32        q0, q2, q15")           // q0 = 2*y*L*log10(E) + R/log2(10)
            __ASM_EMIT("vmla.f32        q1, q3, q15")
            __ASM_EMIT("subs            %[count], $8")
            __ASM_EMIT("vst1.32         {q0-q1}, [%[dst]]!")
            __ASM_EMIT("bhs             1b")
            // x4 block
            __ASM_EMIT("2:")
            __ASM_EMIT("adds            %[count], $4")
            __ASM_EMIT("blt             4f")
            __ASM_EMIT("vld1.32         {q0}, [%[src]]!")
            LOGN_CORE_X4
            __ASM_EMIT("vmul.f32        q0, q0, q14")           // q0 = 2*y*L*log10(E)
            __ASM_EMIT("vmla.f32        q0, q2, q15")           // q0 = 2*y*L*log10(E) + R/log2(10)
            __ASM_EMIT("sub             %[count], $4")
            __ASM_EMIT("vst1.32         {q0}, [%[dst]]!")
            // Tail: 1x-3x block
            __ASM_EMIT("4:")
            __ASM_EMIT("adds            %[count], $4")
            __ASM_EMIT("bls             12f")
            __ASM_EMIT("tst             %[count], $1")
            __ASM_EMIT("beq             6f")
            __ASM_EMIT("vld1.32         {d0[0]}, [%[src]]!")
            __ASM_EMIT("6:")
            __ASM_EMIT("tst             %[count], $2")
            __ASM_EMIT("beq             8f")
            __ASM_EMIT("vld1.32         {d1}, [%[src]]")
            __ASM_EMIT("8:")
            LOGN_CORE_X4
            __ASM_EMIT("vmul.f32        q0, q0, q14")           // q0 = 2*y*L*log10(E)
            __ASM_EMIT("vmla.f32        q0, q2, q15")           // q0 = 2*y*L*log10(E) + R/log2(10)
            __ASM_EMIT("tst             %[count], $1")
            __ASM_EMIT("beq             10f")
            __ASM_EMIT("vst1.32         {d0[0]}, [%[dst]]!")
            __ASM_EMIT("10:")
            __ASM_EMIT("tst             %[count], $2")
            __ASM_EMIT("beq             12f")
            __ASM_EMIT("vst1.32         {d1}, [%[dst]]")
            // End
            __ASM_EMIT("12:")

            : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
            : [L2C] "r" (&LOG2_CONST[0]),
              [LOGC] "r" (&LOGD_C[0])
            : "cc", "memory",
              "q0", "q1", "q2", "q3",
              "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11",
              "q12", "q13", "q14", "q15"
        );
    }

    void logd1(float *dst, size_t count)
    {
        ARCH_ARM_ASM(
            __ASM_EMIT("subs            %[count], $8")
            __ASM_EMIT("vldm            %[LOGC], {q14-q15}")
            __ASM_EMIT("blo             2f")
            // x8 blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("vld1.32         {q0-q1}, [%[dst]]")
            LOGN_CORE_X8
            __ASM_EMIT("vmul.f32        q0, q0, q14")           // q0 = 2*y*L*log10(E)
            __ASM_EMIT("vmul.f32        q1, q1, q14")
            __ASM_EMIT("vmla.f32        q0, q2, q15")           // q0 = 2*y*L*log10(E) + R/log2(10)
            __ASM_EMIT("vmla.f32        q1, q3, q15")
            __ASM_EMIT("subs            %[count], $8")
            __ASM_EMIT("vst1.32         {q0-q1}, [%[dst]]!")
            __ASM_EMIT("bhs             1b")
            // x4 block
            __ASM_EMIT("2:")
            __ASM_EMIT("adds            %[count], $4")
            __ASM_EMIT("blt             4f")
            __ASM_EMIT("vld1.32         {q0}, [%[dst]]")
            LOGN_CORE_X4
            __ASM_EMIT("vmul.f32        q0, q0, q14")           // q0 = 2*y*L*log10(E)
            __ASM_EMIT("vmla.f32        q0, q2, q15")           // q0 = 2*y*L*log10(E) + R/log2(10)
            __ASM_EMIT("sub             %[count], $4")
            __ASM_EMIT("vst1.32         {q0}, [%[dst]]!")
            // Tail: 1x-3x block
            __ASM_EMIT("4:")
            __ASM_EMIT("adds            %[count], $4")
            __ASM_EMIT("bls             12f")
            __ASM_EMIT("tst             %[count], $1")
            __ASM_EMIT("beq             6f")
            __ASM_EMIT("vld1.32         {d0[0]}, [%[dst]]!")
            __ASM_EMIT("6:")
            __ASM_EMIT("tst             %[count], $2")
            __ASM_EMIT("beq             8f")
            __ASM_EMIT("vld1.32         {d1}, [%[dst]]")
            __ASM_EMIT("8:")
            LOGN_CORE_X4
            __ASM_EMIT("vmul.f32        q0, q0, q14")           // q0 = 2*y*L*log10(E)
            __ASM_EMIT("vmla.f32        q0, q2, q15")           // q0 = 2*y*L*log10(E) + R/log2(10)
            __ASM_EMIT("tst             %[count], $1")
            __ASM_EMIT("beq             10f")
            __ASM_EMIT("sub             %[dst], $0x04")
            __ASM_EMIT("vst1.32         {d0[0]}, [%[dst]]!")
            __ASM_EMIT("10:")
            __ASM_EMIT("tst             %[count], $2")
            __ASM_EMIT("beq             12f")
            __ASM_EMIT("vst1.32         {d1}, [%[dst]]")
            // End
            __ASM_EMIT("12:")

            : [dst] "+r" (dst), [count] "+r" (count)
            : [L2C] "r" (&LOG2_CONST[0]),
              [LOGC] "r" (&LOGD_C[0])
            : "cc", "memory",
              "q0", "q1", "q2", "q3",
              "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11",
              "q12", "q13", "q14", "q15"
        );
    }
}

#endif /* DSP_ARCH_ARM_NEON_D32_PMATH_LOG_H_ */
