/*
 * log.h
 *
 *  Created on: 1 дек. 2019 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_AARCH64_ASIMD_PMATH_LOG_H_
#define DSP_ARCH_AARCH64_ASIMD_PMATH_LOG_H_

#ifndef DSP_ARCH_AARCH64_ASIMD_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_AARCH64_ASIMD_IMPL */

namespace asimd
{
#define X4VEC(x)    x, x, x, x

IF_ARCH_AARCH64(
    static const uint32_t LOG2_CONST[] __lsp_aligned16 =
    {
        X4VEC(0x007fffff), // MM = frac
        X4VEC(0x0000007f), // ME = 127
        X4VEC(0x3d888889), // C0 = 1/15 = 0.0666666701436043
        X4VEC(0x3d9d89d9), // C1 = 1/13 = 0.0769230797886848
        X4VEC(0x3dba2e8c), // C2 = 1/11 = 0.0909090936183929
        X4VEC(0x3de38e39), // C3 = 1/9 = 0.1111111119389534
        X4VEC(0x3e124925), // C4 = 1/7 = 0.1428571492433548
        X4VEC(0x3e4ccccd), // C5 = 1/5 = 0.2000000029802322
        X4VEC(0x3eaaaaab), // C6 = 1/3 = 0.3333333432674408
        X4VEC(0x3f800000), // C7 = 1.0f
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

#define LOGN_CORE_X8(MM, ME, C0, C1, C2, C3, C4, C5, C6, C7) \
    /* v0 = x */ \
    __ASM_EMIT("ushr            v2.4s, v0.4s, #23")             /* v2   = ilog2(x) + 127 */ \
    __ASM_EMIT("ushr            v3.4s, v1.4s, #23") \
    __ASM_EMIT("and             v0.16b, v0.16b, " MM ".16b")    /* v0   = x & MM */ \
    __ASM_EMIT("and             v1.16b, v1.16b, " MM ".16b") \
    __ASM_EMIT("sub             v2.4s, v2.4s, " ME ".4s")       /* v2   = r - ME = ilog2(x) */ \
    __ASM_EMIT("sub             v3.4s, v3.4s, " ME ".4s") \
    __ASM_EMIT("orr             v0.16b, v0.16b, " C7 ".16b")    /* v0   = X = (x & MM) | (C7 = 1.0f) */ \
    __ASM_EMIT("orr             v1.16b, v1.16b, " C7 ".16b") \
    __ASM_EMIT("scvtf           v2.4s, v2.4s")                  /* v2   = R = float(r) */ \
    __ASM_EMIT("scvtf           v3.4s, v3.4s") \
    __ASM_EMIT("fadd            v4.4s, v0.4s, " C7 ".4s")       /* v4   = XB = X + (C7 = 1) */ \
    __ASM_EMIT("fadd            v5.4s, v1.4s, " C7 ".4s") \
    __ASM_EMIT("fsub            v0.4s, v0.4s, " C7 ".4s")       /* v0   = XT = X - (C7 = 1) */ \
    __ASM_EMIT("fsub            v1.4s, v1.4s, " C7 ".4s") \
    __ASM_EMIT("frecpe          v6.4s, v4.4s")                  /* v6   = xb */ \
    __ASM_EMIT("frecpe          v7.4s, v5.4s") \
    __ASM_EMIT("frecps          v8.4s, v6.4s, v4.4s")           /* v8   = (2 - XB*xb) */ \
    __ASM_EMIT("frecps          v9.4s, v7.4s, v5.4s") \
    __ASM_EMIT("fmul            v6.4s, v8.4s, v6.4s")           /* v6   = xb' = xb * (2 - XB*xb) */ \
    __ASM_EMIT("fmul            v7.4s, v9.4s, v7.4s") \
    __ASM_EMIT("frecps          v8.4s, v6.4s, v4.4s")           /* v8   = (2 - XB*xb') */ \
    __ASM_EMIT("frecps          v9.4s, v7.4s, v5.4s") \
    __ASM_EMIT("fmul            v4.4s, v8.4s, v6.4s")           /* v4   = 1/(X+1) = xb" = xb' * (2 - XB*xb) */  \
    __ASM_EMIT("fmul            v5.4s, v9.4s, v7.4s") \
    __ASM_EMIT("fmul            v0.4s, v0.4s, v4.4s")           /* v0   = y = (X-1)/(X+1) */ \
    __ASM_EMIT("fmul            v1.4s, v1.4s, v5.4s") \
    __ASM_EMIT("fmul            v4.4s, v0.4s, v0.4s")           /* v4   = Y = y*y */ \
    __ASM_EMIT("fmul            v5.4s, v1.4s, v1.4s") \
    /* v0 = y, v2 = R, v4 = Y */ \
    __ASM_EMIT("fmul            v6.4s, v4.4s, " C0 ".4s")       /* v6   = C0*Y */ \
    __ASM_EMIT("fmul            v7.4s, v5.4s, " C0 ".4s") \
    __ASM_EMIT("fadd            v6.4s, v6.4s, " C1 ".4s")       /* v6   = C1+C0*Y */ \
    __ASM_EMIT("fadd            v7.4s, v7.4s, " C1 ".4s") \
    __ASM_EMIT("fmul            v6.4s, v6.4s, v4.4s")           /* v6   = Y*(C1+C0*Y) */ \
    __ASM_EMIT("fmul            v7.4s, v7.4s, v5.4s") \
    __ASM_EMIT("fadd            v6.4s, v6.4s, " C2 ".4s")       /* v6   = C2+Y*(C1+C0*Y) */ \
    __ASM_EMIT("fadd            v7.4s, v7.4s, " C2 ".4s") \
    __ASM_EMIT("fmul            v6.4s, v6.4s, v4.4s")           /* v6   = Y*(C2+Y*(C1+C0*Y)) */ \
    __ASM_EMIT("fmul            v7.4s, v7.4s, v5.4s") \
    __ASM_EMIT("fadd            v6.4s, v6.4s, " C3 ".4s")       /* v6   = C3+Y*(C2+Y*(C1+C0*Y)) */ \
    __ASM_EMIT("fadd            v7.4s, v7.4s, " C3 ".4s") \
    __ASM_EMIT("fmul            v6.4s, v6.4s, v4.4s")           /* v6   = Y*(C3+Y*(C2+Y*(C1+C0*Y))) */ \
    __ASM_EMIT("fmul            v7.4s, v7.4s, v5.4s") \
    __ASM_EMIT("fadd            v6.4s, v6.4s, " C4 ".4s")       /* v6   = C4+Y*(C3+Y*(C2+Y*(C1+C0*Y))) */ \
    __ASM_EMIT("fadd            v7.4s, v7.4s, " C4 ".4s") \
    __ASM_EMIT("fmul            v6.4s, v6.4s, v4.4s")           /* v6   = Y*(C4+Y*(C3+Y*(C2+Y*(C1+C0*Y)))) */ \
    __ASM_EMIT("fmul            v7.4s, v7.4s, v5.4s") \
    __ASM_EMIT("fadd            v6.4s, v6.4s, " C5 ".4s")       /* v6   = C5+Y*(C4+Y*(C3+Y*(C2+Y*(C1+C0*Y)))) */ \
    __ASM_EMIT("fadd            v7.4s, v7.4s, " C5 ".4s") \
    __ASM_EMIT("fmul            v6.4s, v6.4s, v4.4s")           /* v6   = Y*(C5+Y*(C4+Y*(C3+Y*(C2+Y*(C1+C0*Y))))) */ \
    __ASM_EMIT("fmul            v7.4s, v7.4s, v5.4s") \
    __ASM_EMIT("fadd            v6.4s, v6.4s, " C6 ".4s")       /* v6   = C6+Y*(C5+Y*(C4+Y*(C3+Y*(C2+Y*(C1+C0*Y))))) */ \
    __ASM_EMIT("fadd            v7.4s, v7.4s, " C6 ".4s") \
    __ASM_EMIT("fmul            v6.4s, v6.4s, v4.4s")           /* v6   = Y*(C6+Y*(C5+Y*(C4+Y*(C3+Y*(C2+Y*(C1+C0*Y)))))) */ \
    __ASM_EMIT("fmul            v7.4s, v7.4s, v5.4s") \
    __ASM_EMIT("fadd            v6.4s, v6.4s, " C7 ".4s")       /* v6   = C7+Y*(C6+Y*(C5+Y*(C4+Y*(C3+Y*(C2+Y*(C1+C0*Y)))))) */ \
    __ASM_EMIT("fadd            v7.4s, v7.4s, " C7 ".4s") \
    __ASM_EMIT("fmul            v0.4s, v0.4s, v6.4s")           /* v0   = y*(C7+Y*(C6+Y*(C5+Y*(C4+Y*(C3+Y*(C2+Y*(C1+C0*Y))))))) */ \
    __ASM_EMIT("fmul            v1.4s, v1.4s, v7.4s") \
    /* v0 = y*L, v2 = R */

#define LOGN_CORE_X4(MM, ME, C0, C1, C2, C3, C4, C5, C6, C7) \
    /* v0 = x */ \
    __ASM_EMIT("ushr            v2.4s, v0.4s, #23")             /* v2   = ilog2(x) + 127 */ \
    __ASM_EMIT("and             v0.16b, v0.16b, " MM ".16b")    /* v0   = x & MM */ \
    __ASM_EMIT("sub             v2.4s, v2.4s, " ME ".4s")       /* v2   = r - ME = ilog2(x) */ \
    __ASM_EMIT("orr             v0.16b, v0.16b, " C7 ".16b")    /* v0   = X = (x & MM) | (C7 = 1.0f) */ \
    __ASM_EMIT("scvtf           v2.4s, v2.4s")                  /* v2   = R = float(r) */ \
    __ASM_EMIT("fadd            v4.4s, v0.4s, " C7 ".4s")       /* v4   = XB = X + (C7 = 1) */ \
    __ASM_EMIT("fsub            v0.4s, v0.4s, " C7 ".4s")       /* v0   = XT = X - (C7 = 1) */ \
    __ASM_EMIT("frecpe          v6.4s, v4.4s")                  /* v6   = xb */ \
    __ASM_EMIT("frecps          v8.4s, v6.4s, v4.4s")           /* v8   = (2 - XB*xb) */ \
    __ASM_EMIT("fmul            v6.4s, v8.4s, v6.4s")           /* v6   = xb' = xb * (2 - XB*xb) */ \
    __ASM_EMIT("frecps          v8.4s, v6.4s, v4.4s")           /* v8   = (2 - XB*xb') */ \
    __ASM_EMIT("fmul            v4.4s, v8.4s, v6.4s")           /* v4   = 1/(X+1) = xb" = xb' * (2 - XB*xb) */  \
    __ASM_EMIT("fmul            v0.4s, v0.4s, v4.4s")           /* v0   = y = (X-1)/(X+1) */ \
    __ASM_EMIT("fmul            v4.4s, v0.4s, v0.4s")           /* v4   = Y = y*y */ \
    /* v0 = y, v2 = R, v4 = Y */ \
    __ASM_EMIT("fmul            v6.4s, v4.4s, " C0 ".4s")       /* v6   = C0*Y */ \
    __ASM_EMIT("fadd            v6.4s, v6.4s, " C1 ".4s")       /* v6   = C1+C0*Y */ \
    __ASM_EMIT("fmul            v6.4s, v6.4s, v4.4s")           /* v6   = Y*(C1+C0*Y) */ \
    __ASM_EMIT("fadd            v6.4s, v6.4s, " C2 ".4s")       /* v6   = C2+Y*(C1+C0*Y) */ \
    __ASM_EMIT("fmul            v6.4s, v6.4s, v4.4s")           /* v6   = Y*(C2+Y*(C1+C0*Y)) */ \
    __ASM_EMIT("fadd            v6.4s, v6.4s, " C3 ".4s")       /* v6   = C3+Y*(C2+Y*(C1+C0*Y)) */ \
    __ASM_EMIT("fmul            v6.4s, v6.4s, v4.4s")           /* v6   = Y*(C3+Y*(C2+Y*(C1+C0*Y))) */ \
    __ASM_EMIT("fadd            v6.4s, v6.4s, " C4 ".4s")       /* v6   = C4+Y*(C3+Y*(C2+Y*(C1+C0*Y))) */ \
    __ASM_EMIT("fmul            v6.4s, v6.4s, v4.4s")           /* v6   = Y*(C4+Y*(C3+Y*(C2+Y*(C1+C0*Y)))) */ \
    __ASM_EMIT("fadd            v6.4s, v6.4s, " C5 ".4s")       /* v6   = C5+Y*(C4+Y*(C3+Y*(C2+Y*(C1+C0*Y)))) */ \
    __ASM_EMIT("fmul            v6.4s, v6.4s, v4.4s")           /* v6   = Y*(C5+Y*(C4+Y*(C3+Y*(C2+Y*(C1+C0*Y))))) */ \
    __ASM_EMIT("fadd            v6.4s, v6.4s, " C6 ".4s")       /* v6   = C6+Y*(C5+Y*(C4+Y*(C3+Y*(C2+Y*(C1+C0*Y))))) */ \
    __ASM_EMIT("fmul            v6.4s, v6.4s, v4.4s")           /* v6   = Y*(C6+Y*(C5+Y*(C4+Y*(C3+Y*(C2+Y*(C1+C0*Y)))))) */ \
    __ASM_EMIT("fadd            v6.4s, v6.4s, " C7 ".4s")       /* v6   = C7+Y*(C6+Y*(C5+Y*(C4+Y*(C3+Y*(C2+Y*(C1+C0*Y)))))) */ \
    __ASM_EMIT("fmul            v0.4s, v0.4s, v6.4s")           /* v0   = y*(C7+Y*(C6+Y*(C5+Y*(C4+Y*(C3+Y*(C2+Y*(C1+C0*Y))))))) */ \
    /* v0 = y*L, v2 = R */

    void logb2(float *dst, const float *src, size_t count)
    {
        ARCH_AARCH64_ASM(
            // prepare
            __ASM_EMIT("subs            %[count], %[count], #8")
            __ASM_EMIT("ldp             q30, q31, [%[LOGC]]")
            __ASM_EMIT("ldp             q16, q17, [%[L2C], #0x00]")     /* v16  = MM, v17 = ME */ \
            __ASM_EMIT("ldp             q18, q19, [%[L2C], #0x20]")     /* v18  = C0, v19 = C1 */ \
            __ASM_EMIT("ldp             q20, q21, [%[L2C], #0x40]")     /* v20  = C2, v21 = C3 */ \
            __ASM_EMIT("ldp             q22, q23, [%[L2C], #0x60]")     /* v22  = C4, v23 = C5 */ \
            __ASM_EMIT("ldp             q24, q25, [%[L2C], #0x80]")     /* v24  = C6, v25 = C7 */ \
            __ASM_EMIT("b.lo            2f")
            // x8 blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("ldp             q0, q1, [%[src]]")
            LOGN_CORE_X8("v16", "v17", "v18", "v19", "v20", "v21", "v22", "v23", "v24", "v25")
            __ASM_EMIT("fmla            v2.4s, v0.4s, v30.4s")          // v2   = R + 2*y*L*M_LOG2E
            __ASM_EMIT("fmla            v3.4s, v1.4s, v31.4s")
            __ASM_EMIT("subs            %[count], %[count], #8")
            __ASM_EMIT("stp             q2, q3, [%[dst]]")
            __ASM_EMIT("add             %[src], %[src], #0x20")
            __ASM_EMIT("add             %[dst], %[dst], #0x20")
            __ASM_EMIT("b.hs            1b")
            // x4 block
            __ASM_EMIT("2:")
            __ASM_EMIT("adds            %[count], %[count] , #4")
            __ASM_EMIT("b.lt            4f")
            __ASM_EMIT("ldr             q0, [%[src]]")
            LOGN_CORE_X4("v16", "v17", "v18", "v19", "v20", "v21", "v22", "v23", "v24", "v25")
            __ASM_EMIT("fmla            v2.4s, v0.4s, v30.4s")          // v2   = R + 2*y*L*M_LOG2E
            __ASM_EMIT("sub             %[count], %[count], #4")
            __ASM_EMIT("str             q2, [%[dst]]")
            __ASM_EMIT("add             %[src], %[src], #0x10")
            __ASM_EMIT("add             %[dst], %[dst], #0x10")
            // Tail: 1x-3x block
            __ASM_EMIT("4:")
            __ASM_EMIT("adds            %[count], %[count], #4")
            __ASM_EMIT("b.ls            12f")
            __ASM_EMIT("tst             %[count], #1")
            __ASM_EMIT("b.eq            6f")
            __ASM_EMIT("ld1             {v0.s}[0], [%[src]]")
            __ASM_EMIT("add             %[src], %[src], #0x04")
            __ASM_EMIT("6:")
            __ASM_EMIT("tst             %[count], #2")
            __ASM_EMIT("b.eq            8f")
            __ASM_EMIT("ld1             {v0.d}[1], [%[src]]")
            __ASM_EMIT("8:")
            LOGN_CORE_X4("v16", "v17", "v18", "v19", "v20", "v21", "v22", "v23", "v24", "v25")
            __ASM_EMIT("fmla            v2.4s, v0.4s, v30.4s")          // v2   = R + 2*y*L*M_LOG2E
            __ASM_EMIT("tst             %[count], #1")
            __ASM_EMIT("b.eq            10f")
            __ASM_EMIT("st1             {v2.s}[0], [%[dst]]")
            __ASM_EMIT("add             %[dst], %[dst], #0x04")
            __ASM_EMIT("10:")
            __ASM_EMIT("tst             %[count], #2")
            __ASM_EMIT("b.eq            12f")
            __ASM_EMIT("st1             {v2.d}[1], [%[dst]]")
            // End
            __ASM_EMIT("12:")

            : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
            : [L2C] "r" (&LOG2_CONST[0]),
              [LOGC] "r" (&LOGB_C[0])
            : "cc", "memory",
              "v0", "v1", "v2", "v3",
              "v4", "v5", "v6", "v7",
              "v8", "v9",
              "v16", "v17", "v18", "v19",
              "v20", "v21", "v22", "v23",
              "v24", "v25",
              "v30", "v31"
        );
    }

    void logb1(float *dst, size_t count)
    {
        ARCH_AARCH64_ASM(
            // prepare
            __ASM_EMIT("subs            %[count], %[count], #8")
            __ASM_EMIT("ldp             q30, q31, [%[LOGC]]")
            __ASM_EMIT("ldp             q16, q17, [%[L2C], #0x00]")     /* v16  = MM, v17 = ME */ \
            __ASM_EMIT("ldp             q18, q19, [%[L2C], #0x20]")     /* v18  = C0, v19 = C1 */ \
            __ASM_EMIT("ldp             q20, q21, [%[L2C], #0x40]")     /* v20  = C2, v21 = C3 */ \
            __ASM_EMIT("ldp             q22, q23, [%[L2C], #0x60]")     /* v22  = C4, v23 = C5 */ \
            __ASM_EMIT("ldp             q24, q25, [%[L2C], #0x80]")     /* v24  = C6, v25 = C7 */ \
            __ASM_EMIT("b.lo            2f")
            // x8 blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("ldp             q0, q1, [%[dst]]")
            LOGN_CORE_X8("v16", "v17", "v18", "v19", "v20", "v21", "v22", "v23", "v24", "v25")
            __ASM_EMIT("fmla            v2.4s, v0.4s, v30.4s")          // v2   = R + 2*y*L*M_LOG2E
            __ASM_EMIT("fmla            v3.4s, v1.4s, v31.4s")
            __ASM_EMIT("subs            %[count], %[count], #8")
            __ASM_EMIT("stp             q2, q3, [%[dst]]")
            __ASM_EMIT("add             %[dst], %[dst], #0x20")
            __ASM_EMIT("b.hs            1b")
            // x4 block
            __ASM_EMIT("2:")
            __ASM_EMIT("adds            %[count], %[count] , #4")
            __ASM_EMIT("b.lt            4f")
            __ASM_EMIT("ldr             q0, [%[dst]]")
            LOGN_CORE_X4("v16", "v17", "v18", "v19", "v20", "v21", "v22", "v23", "v24", "v25")
            __ASM_EMIT("fmla            v2.4s, v0.4s, v30.4s")          // v2   = R + 2*y*L*M_LOG2E
            __ASM_EMIT("sub             %[count], %[count], #4")
            __ASM_EMIT("str             q2, [%[dst]]")
            __ASM_EMIT("add             %[dst], %[dst], #0x10")
            // Tail: 1x-3x block
            __ASM_EMIT("4:")
            __ASM_EMIT("adds            %[count], %[count], #4")
            __ASM_EMIT("b.ls            12f")
            __ASM_EMIT("tst             %[count], #1")
            __ASM_EMIT("b.eq            6f")
            __ASM_EMIT("ld1             {v0.s}[0], [%[dst]]")
            __ASM_EMIT("add             %[dst], %[dst], #0x04")
            __ASM_EMIT("6:")
            __ASM_EMIT("tst             %[count], #2")
            __ASM_EMIT("b.eq            8f")
            __ASM_EMIT("ld1             {v0.d}[1], [%[dst]]")
            __ASM_EMIT("8:")
            LOGN_CORE_X4("v16", "v17", "v18", "v19", "v20", "v21", "v22", "v23", "v24", "v25")
            __ASM_EMIT("fmla            v2.4s, v0.4s, v30.4s")          // v2   = R + 2*y*L*M_LOG2E
            __ASM_EMIT("tst             %[count], #1")
            __ASM_EMIT("b.eq            10f")
            __ASM_EMIT("sub             %[dst], %[dst], #0x04")
            __ASM_EMIT("st1             {v2.s}[0], [%[dst]]")
            __ASM_EMIT("add             %[dst], %[dst], #0x04")
            __ASM_EMIT("10:")
            __ASM_EMIT("tst             %[count], #2")
            __ASM_EMIT("b.eq            12f")
            __ASM_EMIT("st1             {v2.d}[1], [%[dst]]")
            // End
            __ASM_EMIT("12:")

            : [dst] "+r" (dst), [count] "+r" (count)
            : [L2C] "r" (&LOG2_CONST[0]),
              [LOGC] "r" (&LOGB_C[0])
            : "cc", "memory",
              "v0", "v1", "v2", "v3",
              "v4", "v5", "v6", "v7",
              "v8", "v9",
              "v16", "v17", "v18", "v19",
              "v20", "v21", "v22", "v23",
              "v24", "v25",
              "v30", "v31"
        );
    }

    void loge2(float *dst, const float *src, size_t count)
    {
        ARCH_AARCH64_ASM(
            // prepare
            __ASM_EMIT("subs            %[count], %[count], #8")
            __ASM_EMIT("ldp             q30, q31, [%[LOGC]]")
            __ASM_EMIT("ldp             q16, q17, [%[L2C], #0x00]")     /* v16  = MM, v17 = ME */ \
            __ASM_EMIT("ldp             q18, q19, [%[L2C], #0x20]")     /* v18  = C0, v19 = C1 */ \
            __ASM_EMIT("ldp             q20, q21, [%[L2C], #0x40]")     /* v20  = C2, v21 = C3 */ \
            __ASM_EMIT("ldp             q22, q23, [%[L2C], #0x60]")     /* v22  = C4, v23 = C5 */ \
            __ASM_EMIT("ldp             q24, q25, [%[L2C], #0x80]")     /* v24  = C6, v25 = C7 */ \
            __ASM_EMIT("b.lo            2f")
            // x8 blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("ldp             q0, q1, [%[src]]")
            LOGN_CORE_X8("v16", "v17", "v18", "v19", "v20", "v21", "v22", "v23", "v24", "v25")
            __ASM_EMIT("fadd            v0.4s, v0.4s, v0.4s")           // v0 = 2*y*L
            __ASM_EMIT("fadd            v1.4s, v1.4s, v1.4s")
            __ASM_EMIT("fmla            v0.4s, v2.4s, v30.4s")          // v0 = 2*y*L + R/log2(E)
            __ASM_EMIT("fmla            v1.4s, v3.4s, v31.4s")
            __ASM_EMIT("subs            %[count], %[count], #8")
            __ASM_EMIT("stp             q0, q1, [%[dst]]")
            __ASM_EMIT("add             %[src], %[src], #0x20")
            __ASM_EMIT("add             %[dst], %[dst], #0x20")
            __ASM_EMIT("b.hs            1b")
            // x4 block
            __ASM_EMIT("2:")
            __ASM_EMIT("adds            %[count], %[count] , #4")
            __ASM_EMIT("b.lt            4f")
            __ASM_EMIT("ldr             q0, [%[src]]")
            LOGN_CORE_X4("v16", "v17", "v18", "v19", "v20", "v21", "v22", "v23", "v24", "v25")
            __ASM_EMIT("fadd            v0.4s, v0.4s, v0.4s")           // v0 = 2*y*L
            __ASM_EMIT("fmla            v0.4s, v2.4s, v30.4s")          // v0 = 2*y*L + R/log2(E)
            __ASM_EMIT("sub             %[count], %[count], #4")
            __ASM_EMIT("str             q0, [%[dst]]")
            __ASM_EMIT("add             %[src], %[src], #0x10")
            __ASM_EMIT("add             %[dst], %[dst], #0x10")
            // Tail: 1x-3x block
            __ASM_EMIT("4:")
            __ASM_EMIT("adds            %[count], %[count], #4")
            __ASM_EMIT("b.ls            12f")
            __ASM_EMIT("tst             %[count], #1")
            __ASM_EMIT("b.eq            6f")
            __ASM_EMIT("ld1             {v0.s}[0], [%[src]]")
            __ASM_EMIT("add             %[src], %[src], #0x04")
            __ASM_EMIT("6:")
            __ASM_EMIT("tst             %[count], #2")
            __ASM_EMIT("b.eq            8f")
            __ASM_EMIT("ld1             {v0.d}[1], [%[src]]")
            __ASM_EMIT("8:")
            LOGN_CORE_X4("v16", "v17", "v18", "v19", "v20", "v21", "v22", "v23", "v24", "v25")
            __ASM_EMIT("fadd            v0.4s, v0.4s, v0.4s")           // v0 = 2*y*L
            __ASM_EMIT("fmla            v0.4s, v2.4s, v30.4s")          // v0 = 2*y*L + R/log2(E)
            __ASM_EMIT("tst             %[count], #1")
            __ASM_EMIT("b.eq            10f")
            __ASM_EMIT("st1             {v0.s}[0], [%[dst]]")
            __ASM_EMIT("add             %[dst], %[dst], #0x04")
            __ASM_EMIT("10:")
            __ASM_EMIT("tst             %[count], #2")
            __ASM_EMIT("b.eq            12f")
            __ASM_EMIT("st1             {v0.d}[1], [%[dst]]")
            // End
            __ASM_EMIT("12:")

            : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
            : [L2C] "r" (&LOG2_CONST[0]),
              [LOGC] "r" (&LOGE_C[0])
            : "cc", "memory",
              "v0", "v1", "v2", "v3",
              "v4", "v5", "v6", "v7",
              "v8", "v9",
              "v16", "v17", "v18", "v19",
              "v20", "v21", "v22", "v23",
              "v24", "v25",
              "v30", "v31"
        );
    }

    void loge1(float *dst, size_t count)
    {
        ARCH_AARCH64_ASM(
            // prepare
            __ASM_EMIT("subs            %[count], %[count], #8")
            __ASM_EMIT("ldp             q30, q31, [%[LOGC]]")
            __ASM_EMIT("ldp             q16, q17, [%[L2C], #0x00]")     /* v16  = MM, v17 = ME */ \
            __ASM_EMIT("ldp             q18, q19, [%[L2C], #0x20]")     /* v18  = C0, v19 = C1 */ \
            __ASM_EMIT("ldp             q20, q21, [%[L2C], #0x40]")     /* v20  = C2, v21 = C3 */ \
            __ASM_EMIT("ldp             q22, q23, [%[L2C], #0x60]")     /* v22  = C4, v23 = C5 */ \
            __ASM_EMIT("ldp             q24, q25, [%[L2C], #0x80]")     /* v24  = C6, v25 = C7 */ \
            __ASM_EMIT("b.lo            2f")
            // x8 blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("ldp             q0, q1, [%[dst]]")
            LOGN_CORE_X8("v16", "v17", "v18", "v19", "v20", "v21", "v22", "v23", "v24", "v25")
            __ASM_EMIT("fadd            v0.4s, v0.4s, v0.4s")           // v0 = 2*y*L
            __ASM_EMIT("fadd            v1.4s, v1.4s, v1.4s")
            __ASM_EMIT("fmla            v0.4s, v2.4s, v30.4s")          // v0 = 2*y*L + R/log2(E)
            __ASM_EMIT("fmla            v1.4s, v3.4s, v31.4s")
            __ASM_EMIT("subs            %[count], %[count], #8")
            __ASM_EMIT("stp             q0, q1, [%[dst]]")
            __ASM_EMIT("add             %[dst], %[dst], #0x20")
            __ASM_EMIT("b.hs            1b")
            // x4 block
            __ASM_EMIT("2:")
            __ASM_EMIT("adds            %[count], %[count] , #4")
            __ASM_EMIT("b.lt            4f")
            __ASM_EMIT("ldr             q0, [%[dst]]")
            LOGN_CORE_X4("v16", "v17", "v18", "v19", "v20", "v21", "v22", "v23", "v24", "v25")
            __ASM_EMIT("fadd            v0.4s, v0.4s, v0.4s")           // v0 = 2*y*L
            __ASM_EMIT("fmla            v0.4s, v2.4s, v30.4s")          // v0 = 2*y*L + R/log2(E)
            __ASM_EMIT("sub             %[count], %[count], #4")
            __ASM_EMIT("str             q0, [%[dst]]")
            __ASM_EMIT("add             %[dst], %[dst], #0x10")
            // Tail: 1x-3x block
            __ASM_EMIT("4:")
            __ASM_EMIT("adds            %[count], %[count], #4")
            __ASM_EMIT("b.ls            12f")
            __ASM_EMIT("tst             %[count], #1")
            __ASM_EMIT("b.eq            6f")
            __ASM_EMIT("ld1             {v0.s}[0], [%[dst]]")
            __ASM_EMIT("add             %[dst], %[dst], #0x04")
            __ASM_EMIT("6:")
            __ASM_EMIT("tst             %[count], #2")
            __ASM_EMIT("b.eq            8f")
            __ASM_EMIT("ld1             {v0.d}[1], [%[dst]]")
            __ASM_EMIT("8:")
            LOGN_CORE_X4("v16", "v17", "v18", "v19", "v20", "v21", "v22", "v23", "v24", "v25")
            __ASM_EMIT("fadd            v0.4s, v0.4s, v0.4s")           // v0 = 2*y*L
            __ASM_EMIT("fmla            v0.4s, v2.4s, v30.4s")          // v0 = 2*y*L + R/log2(E)
            __ASM_EMIT("tst             %[count], #1")
            __ASM_EMIT("b.eq            10f")
            __ASM_EMIT("sub             %[dst], %[dst], #0x04")
            __ASM_EMIT("st1             {v0.s}[0], [%[dst]]")
            __ASM_EMIT("add             %[dst], %[dst], #0x04")
            __ASM_EMIT("10:")
            __ASM_EMIT("tst             %[count], #2")
            __ASM_EMIT("b.eq            12f")
            __ASM_EMIT("st1             {v0.d}[1], [%[dst]]")
            // End
            __ASM_EMIT("12:")

            : [dst] "+r" (dst), [count] "+r" (count)
            : [L2C] "r" (&LOG2_CONST[0]),
              [LOGC] "r" (&LOGE_C[0])
            : "cc", "memory",
              "v0", "v1", "v2", "v3",
              "v4", "v5", "v6", "v7",
              "v8", "v9",
              "v16", "v17", "v18", "v19",
              "v20", "v21", "v22", "v23",
              "v24", "v25",
              "v30", "v31"
        );
    }

    void logd2(float *dst, const float *src, size_t count)
    {
        ARCH_AARCH64_ASM(
            // prepare
            __ASM_EMIT("subs            %[count], %[count], #8")
            __ASM_EMIT("ldp             q30, q31, [%[LOGC]]")
            __ASM_EMIT("ldp             q16, q17, [%[L2C], #0x00]")     /* v16  = MM, v17 = ME */ \
            __ASM_EMIT("ldp             q18, q19, [%[L2C], #0x20]")     /* v18  = C0, v19 = C1 */ \
            __ASM_EMIT("ldp             q20, q21, [%[L2C], #0x40]")     /* v20  = C2, v21 = C3 */ \
            __ASM_EMIT("ldp             q22, q23, [%[L2C], #0x60]")     /* v22  = C4, v23 = C5 */ \
            __ASM_EMIT("ldp             q24, q25, [%[L2C], #0x80]")     /* v24  = C6, v25 = C7 */ \
            __ASM_EMIT("b.lo            2f")
            // x8 blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("ldp             q0, q1, [%[src]]")
            LOGN_CORE_X8("v16", "v17", "v18", "v19", "v20", "v21", "v22", "v23", "v24", "v25")
            __ASM_EMIT("fmul            v0.4s, v0.4s, v30.4s")          // v0 = 2*y*L*log10(E)
            __ASM_EMIT("fmul            v1.4s, v1.4s, v30.4s")
            __ASM_EMIT("fmla            v0.4s, v2.4s, v31.4s")          // v0 = 2*y*L*log10(E) + R/log2(10)
            __ASM_EMIT("fmla            v1.4s, v3.4s, v31.4s")
            __ASM_EMIT("subs            %[count], %[count], #8")
            __ASM_EMIT("stp             q0, q1, [%[dst]]")
            __ASM_EMIT("add             %[src], %[src], #0x20")
            __ASM_EMIT("add             %[dst], %[dst], #0x20")
            __ASM_EMIT("b.hs            1b")
            // x4 block
            __ASM_EMIT("2:")
            __ASM_EMIT("adds            %[count], %[count] , #4")
            __ASM_EMIT("b.lt            4f")
            __ASM_EMIT("ldr             q0, [%[src]]")
            LOGN_CORE_X4("v16", "v17", "v18", "v19", "v20", "v21", "v22", "v23", "v24", "v25")
            __ASM_EMIT("fmul            v0.4s, v0.4s, v30.4s")          // v0 = 2*y*L*log10(E)
            __ASM_EMIT("fmla            v0.4s, v2.4s, v31.4s")          // v0 = 2*y*L*log10(E) + R/log2(10)
            __ASM_EMIT("sub             %[count], %[count], #4")
            __ASM_EMIT("str             q0, [%[dst]]")
            __ASM_EMIT("add             %[src], %[src], #0x10")
            __ASM_EMIT("add             %[dst], %[dst], #0x10")
            // Tail: 1x-3x block
            __ASM_EMIT("4:")
            __ASM_EMIT("adds            %[count], %[count], #4")
            __ASM_EMIT("b.ls            12f")
            __ASM_EMIT("tst             %[count], #1")
            __ASM_EMIT("b.eq            6f")
            __ASM_EMIT("ld1             {v0.s}[0], [%[src]]")
            __ASM_EMIT("add             %[src], %[src], #0x04")
            __ASM_EMIT("6:")
            __ASM_EMIT("tst             %[count], #2")
            __ASM_EMIT("b.eq            8f")
            __ASM_EMIT("ld1             {v0.d}[1], [%[src]]")
            __ASM_EMIT("8:")
            LOGN_CORE_X4("v16", "v17", "v18", "v19", "v20", "v21", "v22", "v23", "v24", "v25")
            __ASM_EMIT("fmul            v0.4s, v0.4s, v30.4s")          // v0 = 2*y*L*log10(E)
            __ASM_EMIT("fmla            v0.4s, v2.4s, v31.4s")          // v0 = 2*y*L*log10(E) + R/log2(10)
            __ASM_EMIT("tst             %[count], #1")
            __ASM_EMIT("b.eq            10f")
            __ASM_EMIT("st1             {v0.s}[0], [%[dst]]")
            __ASM_EMIT("add             %[dst], %[dst], #0x04")
            __ASM_EMIT("10:")
            __ASM_EMIT("tst             %[count], #2")
            __ASM_EMIT("b.eq            12f")
            __ASM_EMIT("st1             {v0.d}[1], [%[dst]]")
            // End
            __ASM_EMIT("12:")

            : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
            : [L2C] "r" (&LOG2_CONST[0]),
              [LOGC] "r" (&LOGD_C[0])
            : "cc", "memory",
              "v0", "v1", "v2", "v3",
              "v4", "v5", "v6", "v7",
              "v8", "v9",
              "v16", "v17", "v18", "v19",
              "v20", "v21", "v22", "v23",
              "v24", "v25",
              "v30", "v31"
        );
    }

    void logd1(float *dst, size_t count)
    {
        ARCH_AARCH64_ASM(
            // prepare
            __ASM_EMIT("subs            %[count], %[count], #8")
            __ASM_EMIT("ldp             q30, q31, [%[LOGC]]")
            __ASM_EMIT("ldp             q16, q17, [%[L2C], #0x00]")     /* v16  = MM, v17 = ME */ \
            __ASM_EMIT("ldp             q18, q19, [%[L2C], #0x20]")     /* v18  = C0, v19 = C1 */ \
            __ASM_EMIT("ldp             q20, q21, [%[L2C], #0x40]")     /* v20  = C2, v21 = C3 */ \
            __ASM_EMIT("ldp             q22, q23, [%[L2C], #0x60]")     /* v22  = C4, v23 = C5 */ \
            __ASM_EMIT("ldp             q24, q25, [%[L2C], #0x80]")     /* v24  = C6, v25 = C7 */ \
            __ASM_EMIT("b.lo            2f")
            // x8 blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("ldp             q0, q1, [%[dst]]")
            LOGN_CORE_X8("v16", "v17", "v18", "v19", "v20", "v21", "v22", "v23", "v24", "v25")
            __ASM_EMIT("fmul            v0.4s, v0.4s, v30.4s")          // v0 = 2*y*L*log10(E)
            __ASM_EMIT("fmul            v1.4s, v1.4s, v30.4s")
            __ASM_EMIT("fmla            v0.4s, v2.4s, v31.4s")          // v0 = 2*y*L*log10(E) + R/log2(10)
            __ASM_EMIT("fmla            v1.4s, v3.4s, v31.4s")
            __ASM_EMIT("subs            %[count], %[count], #8")
            __ASM_EMIT("stp             q0, q1, [%[dst]]")
            __ASM_EMIT("add             %[dst], %[dst], #0x20")
            __ASM_EMIT("b.hs            1b")
            // x4 block
            __ASM_EMIT("2:")
            __ASM_EMIT("adds            %[count], %[count] , #4")
            __ASM_EMIT("b.lt            4f")
            __ASM_EMIT("ldr             q0, [%[dst]]")
            LOGN_CORE_X4("v16", "v17", "v18", "v19", "v20", "v21", "v22", "v23", "v24", "v25")
            __ASM_EMIT("fmul            v0.4s, v0.4s, v30.4s")          // v0 = 2*y*L*log10(E)
            __ASM_EMIT("fmla            v0.4s, v2.4s, v31.4s")          // v0 = 2*y*L*log10(E) + R/log2(10)
            __ASM_EMIT("sub             %[count], %[count], #4")
            __ASM_EMIT("str             q0, [%[dst]]")
            __ASM_EMIT("add             %[dst], %[dst], #0x10")
            // Tail: 1x-3x block
            __ASM_EMIT("4:")
            __ASM_EMIT("adds            %[count], %[count], #4")
            __ASM_EMIT("b.ls            12f")
            __ASM_EMIT("tst             %[count], #1")
            __ASM_EMIT("b.eq            6f")
            __ASM_EMIT("ld1             {v0.s}[0], [%[dst]]")
            __ASM_EMIT("add             %[dst], %[dst], #0x04")
            __ASM_EMIT("6:")
            __ASM_EMIT("tst             %[count], #2")
            __ASM_EMIT("b.eq            8f")
            __ASM_EMIT("ld1             {v0.d}[1], [%[dst]]")
            __ASM_EMIT("8:")
            LOGN_CORE_X4("v16", "v17", "v18", "v19", "v20", "v21", "v22", "v23", "v24", "v25")
            __ASM_EMIT("fmul            v0.4s, v0.4s, v30.4s")          // v0 = 2*y*L*log10(E)
            __ASM_EMIT("fmla            v0.4s, v2.4s, v31.4s")          // v0 = 2*y*L*log10(E) + R/log2(10)
            __ASM_EMIT("tst             %[count], #1")
            __ASM_EMIT("b.eq            10f")
            __ASM_EMIT("sub             %[dst], %[dst], #0x04")
            __ASM_EMIT("st1             {v0.s}[0], [%[dst]]")
            __ASM_EMIT("add             %[dst], %[dst], #0x04")
            __ASM_EMIT("10:")
            __ASM_EMIT("tst             %[count], #2")
            __ASM_EMIT("b.eq            12f")
            __ASM_EMIT("st1             {v0.d}[1], [%[dst]]")
            // End
            __ASM_EMIT("12:")

            : [dst] "+r" (dst), [count] "+r" (count)
            : [L2C] "r" (&LOG2_CONST[0]),
              [LOGC] "r" (&LOGD_C[0])
            : "cc", "memory",
              "v0", "v1", "v2", "v3",
              "v4", "v5", "v6", "v7",
              "v8", "v9",
              "v16", "v17", "v18", "v19",
              "v20", "v21", "v22", "v23",
              "v24", "v25",
              "v30", "v31"
        );
    }
}



#endif /* DSP_ARCH_AARCH64_ASIMD_PMATH_LOG_H_ */
