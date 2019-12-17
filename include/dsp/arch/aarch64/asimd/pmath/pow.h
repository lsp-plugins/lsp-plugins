/*
 * pow.h
 *
 *  Created on: 1 дек. 2019 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_AARCH64_ASIMD_PMATH_POW_H_
#define DSP_ARCH_AARCH64_ASIMD_PMATH_POW_H_

#include <dsp/arch/aarch64/asimd/pmath/exp.h>
#include <dsp/arch/aarch64/asimd/pmath/log.h>

namespace asimd
{
#define X4VEC(x)    x, x, x, x
    static const uint32_t POW_CONST[] __lsp_aligned16 =
    {
        X4VEC(0x3d888889), // LC0 = 1/15 = 0.0666666701436043
        X4VEC(0x3d9d89d9), // LC1 = 1/13 = 0.0769230797886848
        X4VEC(0x3dba2e8c), // LC2 = 1/11 = 0.0909090936183929
        X4VEC(0x3de38e39), // LC3 = 1/9 = 0.1111111119389534
        X4VEC(0x3e124925), // LC4 = 1/7 = 0.1428571492433548
        X4VEC(0x3e4ccccd), // LC5 = 1/5 = 0.2000000029802322
        X4VEC(0x3eaaaaab), // LC6 = 1/3 = 0.3333333432674408
        X4VEC(0x40e00000), // EC5 = 7!/6! = 7.0000000000000000

        X4VEC(0x42280000), // EC4 = 7!/5! = 42.0000000000000000
        X4VEC(0x43520000), // EC3 = 7!/4! = 210.0000000000000000
        X4VEC(0x44520000), // EC2 = 7!/3! = 840.0000000000000000
        X4VEC(0x451d8000), // EC1 = 7!/2! = 2520.0000000000000000
        X4VEC(0x459d8000), // EC0 = 7!/1! = 5040.0000000000000000
        X4VEC(0x39500d01), // EC6 = 1/7! = 0.0001984127011383
        X4VEC(0x007fffff), // MM  = frac
        X4VEC(0x0000007f), // ME  = 127

        X4VEC(0x3f800000), // C7  = 1.0f
        X4VEC(0x3f317218), // L2  = ln(2)
        X4VEC(0x4038aa3b)  // LE  = 2*log2(e)
    };
#undef X4VEC

    void powcv2(float *dst, const float *v, float c, size_t count)
    {
//        float C = logf(c);
//        for (size_t i=0; i<count; ++i)
//            v[i] = expf(v[i] * C);
        ARCH_AARCH64_ASM(
            __ASM_EMIT("dup             v0.4s, %S[C].s[0]")             // v15  = C
            __ASM_EMIT("ldr             q14, [%[P2C], #0x120]")         // v14  = LE
            __ASM_EMIT("ldp             q16, q17, [%[P2C], #0x000]")    // v16  = LC0, v17  = LC1
            __ASM_EMIT("ldp             q18, q19, [%[P2C], #0x020]")    // v18  = LC2, v19  = LC3
            __ASM_EMIT("ldp             q20, q21, [%[P2C], #0x040]")    // v20  = LC4, v21  = LC5
            __ASM_EMIT("ldp             q22, q23, [%[P2C], #0x060]")    // v22  = LC6, v23  = EC5
            __ASM_EMIT("ldp             q24, q25, [%[P2C], #0x080]")    // v24  = EC4, v25  = EC3
            __ASM_EMIT("ldp             q26, q27, [%[P2C], #0x0a0]")    // v26  = EC2, v27  = EC1
            __ASM_EMIT("ldp             q28, q29, [%[P2C], #0x0c0]")    // v28  = EC0, v29  = EC6
            __ASM_EMIT("ldp             q30, q31, [%[P2C], #0x0e0]")    // v30  = MM , v31  = ME
            __ASM_EMIT("ldp             q12, q13, [%[P2C], #0x100]")    // v12  = C7 , v13  = L2
            // log(c)
            LOGN_CORE_X4("v30", "v31", "v16", "v17", "v18", "v19", "v20", "v21", "v22", "v12")
            __ASM_EMIT("fmul            v0.4s, v0.4s, v14.4s")          // v0   = 2*y*L*log2(E)
            __ASM_EMIT("fadd            v15.4s, v0.4s, v2.4s")          // v15  = 2*y*L*log2(E) + R = log(c)*log2(E)
            __ASM_EMIT("subs            %[count], %[count], #8")
            __ASM_EMIT("b.lo            2f")
            // x8 blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("ldp             q0, q1, [%[src]]")
            __ASM_EMIT("fmul            v0.4s, v0.4s, v15.4s")
            __ASM_EMIT("fmul            v1.4s, v1.4s, v15.4s")
            POW2_CORE_X8("v31", "v13", "v23", "v24", "v25", "v26", "v27", "v28", "v29", "v12")
            __ASM_EMIT("subs            %[count], %[count], #8")
            __ASM_EMIT("stp             q0, q1, [%[dst]]")
            __ASM_EMIT("add             %[src], %[src], #0x20")
            __ASM_EMIT("add             %[dst], %[dst], #0x20")
            __ASM_EMIT("b.hs             1b")
            // x4 block
            __ASM_EMIT("2:")
            __ASM_EMIT("adds            %[count], %[count], #4")
            __ASM_EMIT("b.lt            4f")
            __ASM_EMIT("ldr             q0, [%[src]]")
            __ASM_EMIT("fmul            v0.4s, v0.4s, v15.4s")
            POW2_CORE_X4("v31", "v13", "v23", "v24", "v25", "v26", "v27", "v28", "v29", "v12")
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
            __ASM_EMIT("fmul            v0.4s, v0.4s, v15.4s")
            POW2_CORE_X4("v31", "v13", "v23", "v24", "v25", "v26", "v27", "v28", "v29", "v12")
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

            : [dst] "+r" (dst), [src] "+r" (v), [C] "+w" (c),
              [count] "+r" (count)
            : [P2C] "r" (&POW_CONST[0])
            : "cc", "memory",
              "v1", "v2", "v3",
              "v4", "v5", "v6", "v7",
              "v8", "v9", "v10", "v11",
              "v12", "v13", "v14", "v15",
              "v16", "v17", "v18", "v19",
              "v20", "v21", "v22", "v23",
              "v24", "v25", "v26", "v27",
              "v28", "v29", "v30", "v31"
        );
    }

    void powcv1(float *dst, float c, size_t count)
    {
//        float C = logf(c);
//        for (size_t i=0; i<count; ++i)
//            v[i] = expf(v[i] * C);
        ARCH_AARCH64_ASM(
            __ASM_EMIT("dup             v0.4s, %S[C].s[0]")             // v15  = C
            __ASM_EMIT("ldr             q14, [%[P2C], #0x120]")         // v14  = LE
            __ASM_EMIT("ldp             q16, q17, [%[P2C], #0x000]")    // v16  = LC0, v17  = LC1
            __ASM_EMIT("ldp             q18, q19, [%[P2C], #0x020]")    // v18  = LC2, v19  = LC3
            __ASM_EMIT("ldp             q20, q21, [%[P2C], #0x040]")    // v20  = LC4, v21  = LC5
            __ASM_EMIT("ldp             q22, q23, [%[P2C], #0x060]")    // v22  = LC6, v23  = EC5
            __ASM_EMIT("ldp             q24, q25, [%[P2C], #0x080]")    // v24  = EC4, v25  = EC3
            __ASM_EMIT("ldp             q26, q27, [%[P2C], #0x0a0]")    // v26  = EC2, v27  = EC1
            __ASM_EMIT("ldp             q28, q29, [%[P2C], #0x0c0]")    // v28  = EC0, v29  = EC6
            __ASM_EMIT("ldp             q30, q31, [%[P2C], #0x0e0]")    // v30  = MM , v31  = ME
            __ASM_EMIT("ldp             q12, q13, [%[P2C], #0x100]")    // v12  = C7 , v13  = L2
            // log(c)
            LOGN_CORE_X4("v30", "v31", "v16", "v17", "v18", "v19", "v20", "v21", "v22", "v12")
            __ASM_EMIT("fmul            v0.4s, v0.4s, v14.4s")          // v0   = 2*y*L*log2(E)
            __ASM_EMIT("fadd            v15.4s, v0.4s, v2.4s")          // v15  = 2*y*L*log2(E) + R = log(c)*log2(E)
            __ASM_EMIT("subs            %[count], %[count], #8")
            __ASM_EMIT("b.lo            2f")
            // x8 blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("ldp             q0, q1, [%[dst]]")
            __ASM_EMIT("fmul            v0.4s, v0.4s, v15.4s")
            __ASM_EMIT("fmul            v1.4s, v1.4s, v15.4s")
            POW2_CORE_X8("v31", "v13", "v23", "v24", "v25", "v26", "v27", "v28", "v29", "v12")
            __ASM_EMIT("subs            %[count], %[count], #8")
            __ASM_EMIT("stp             q0, q1, [%[dst]]")
            __ASM_EMIT("add             %[dst], %[dst], #0x20")
            __ASM_EMIT("b.hs             1b")
            // x4 block
            __ASM_EMIT("2:")
            __ASM_EMIT("adds            %[count], %[count], #4")
            __ASM_EMIT("b.lt            4f")
            __ASM_EMIT("ldr             q0, [%[dst]]")
            __ASM_EMIT("fmul            v0.4s, v0.4s, v15.4s")
            POW2_CORE_X4("v31", "v13", "v23", "v24", "v25", "v26", "v27", "v28", "v29", "v12")
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
            __ASM_EMIT("fmul            v0.4s, v0.4s, v15.4s")
            POW2_CORE_X4("v31", "v13", "v23", "v24", "v25", "v26", "v27", "v28", "v29", "v12")
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

            : [dst] "+r" (dst), [C] "+w" (c),
              [count] "+r" (count)
            : [P2C] "r" (&POW_CONST[0])
            : "cc", "memory",
              "v1", "v2", "v3",
              "v4", "v5", "v6", "v7",
              "v8", "v9", "v10", "v11",
              "v12", "v13", "v14", "v15",
              "v16", "v17", "v18", "v19",
              "v20", "v21", "v22", "v23",
              "v24", "v25", "v26", "v27",
              "v28", "v29", "v30", "v31"
        );
    }

    void powvc2(float *dst, const float *c, float v, size_t count)
    {
//        for (size_t i=0; i<count; ++i)
//            dst[i] = expf(v * logf(c[i]));
        ARCH_AARCH64_ASM(
            __ASM_EMIT("dup             v15.4s, %S[V].s[0]")            // v15  = V
            __ASM_EMIT("ldr             q14, [%[P2C], #0x120]")         // v14  = LE
            __ASM_EMIT("ldp             q16, q17, [%[P2C], #0x000]")    // v16  = LC0, v17  = LC1
            __ASM_EMIT("ldp             q18, q19, [%[P2C], #0x020]")    // v18  = LC2, v19  = LC3
            __ASM_EMIT("ldp             q20, q21, [%[P2C], #0x040]")    // v20  = LC4, v21  = LC5
            __ASM_EMIT("ldp             q22, q23, [%[P2C], #0x060]")    // v22  = LC6, v23  = EC5
            __ASM_EMIT("ldp             q24, q25, [%[P2C], #0x080]")    // v24  = EC4, v25  = EC3
            __ASM_EMIT("ldp             q26, q27, [%[P2C], #0x0a0]")    // v26  = EC2, v27  = EC1
            __ASM_EMIT("ldp             q28, q29, [%[P2C], #0x0c0]")    // v28  = EC0, v29  = EC6
            __ASM_EMIT("ldp             q30, q31, [%[P2C], #0x0e0]")    // v30  = MM , v31  = ME
            __ASM_EMIT("ldp             q12, q13, [%[P2C], #0x100]")    // v12  = C7 , v13  = L2
            __ASM_EMIT("subs            %[count], %[count], #8")
            __ASM_EMIT("b.lo            2f")
            // x8 blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("ldp             q0, q1, [%[src]]")
            LOGN_CORE_X8("v30", "v31", "v16", "v17", "v18", "v19", "v20", "v21", "v22", "v12")
            __ASM_EMIT("fmla            v2.4s, v0.4s, v14.4s")          // v2 = 2*y*L*log2(E) + R
            __ASM_EMIT("fmla            v3.4s, v1.4s, v14.4s")
            __ASM_EMIT("fmul            v0.4s, v2.4s, v15.4s")          // q0 = (2*y*L*log2(E) + R)*v = log(c[i])*v
            __ASM_EMIT("fmul            v1.4s, v3.4s, v15.4s")
            POW2_CORE_X8("v31", "v13", "v23", "v24", "v25", "v26", "v27", "v28", "v29", "v12")
            __ASM_EMIT("subs            %[count], %[count], #8")
            __ASM_EMIT("stp             q0, q1, [%[dst]]")
            __ASM_EMIT("add             %[src], %[src], #0x20")
            __ASM_EMIT("add             %[dst], %[dst], #0x20")
            __ASM_EMIT("b.hs            1b")
            // x4 block
            __ASM_EMIT("2:")
            __ASM_EMIT("adds            %[count], %[count], #4")
            __ASM_EMIT("b.lt            4f")
            __ASM_EMIT("ldr             q0, [%[src]]")
            LOGN_CORE_X4("v30", "v31", "v16", "v17", "v18", "v19", "v20", "v21", "v22", "v12")
            __ASM_EMIT("fmla            v2.4s, v0.4s, v14.4s")          // v2 = 2*y*L*log2(E) + R
            __ASM_EMIT("fmul            v0.4s, v2.4s, v15.4s")          // q0 = (2*y*L*log2(E) + R)*v = log(c[i])*v
            POW2_CORE_X4("v31", "v13", "v23", "v24", "v25", "v26", "v27", "v28", "v29", "v12")
            __ASM_EMIT("subs            %[count], %[count], #4")
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
            LOGN_CORE_X4("v30", "v31", "v16", "v17", "v18", "v19", "v20", "v21", "v22", "v12")
            __ASM_EMIT("fmla            v2.4s, v0.4s, v14.4s")          // v2 = 2*y*L*log2(E) + R
            __ASM_EMIT("fmul            v0.4s, v2.4s, v15.4s")          // q0 = (2*y*L*log2(E) + R)*v = log(c[i])*v
            POW2_CORE_X4("v31", "v13", "v23", "v24", "v25", "v26", "v27", "v28", "v29", "v12")
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

            : [dst] "+r" (dst), [src] "+r" (c), [V] "+w" (v),
              [count] "+r" (count)
            : [P2C] "r" (&POW_CONST[0])
            : "cc", "memory",
              "v1", "v2", "v3",
              "v4", "v5", "v6", "v7",
              "v8", "v9", "v10", "v11",
              "v12", "v13", "v14", "v15",
              "v16", "v17", "v18", "v19",
              "v20", "v21", "v22", "v23",
              "v24", "v25", "v26", "v27",
              "v28", "v29", "v30", "v31"
        );
    }

    void powvc1(float *c, float v, size_t count)
    {
//        for (size_t i=0; i<count; ++i)
//            c[i] = expf(v * logf(c[i]));
        ARCH_AARCH64_ASM(
            __ASM_EMIT("dup             v15.4s, %S[V].s[0]")            // v15  = V
            __ASM_EMIT("ldr             q14, [%[P2C], #0x120]")         // v14  = LE
            __ASM_EMIT("ldp             q16, q17, [%[P2C], #0x000]")    // v16  = LC0, v17  = LC1
            __ASM_EMIT("ldp             q18, q19, [%[P2C], #0x020]")    // v18  = LC2, v19  = LC3
            __ASM_EMIT("ldp             q20, q21, [%[P2C], #0x040]")    // v20  = LC4, v21  = LC5
            __ASM_EMIT("ldp             q22, q23, [%[P2C], #0x060]")    // v22  = LC6, v23  = EC5
            __ASM_EMIT("ldp             q24, q25, [%[P2C], #0x080]")    // v24  = EC4, v25  = EC3
            __ASM_EMIT("ldp             q26, q27, [%[P2C], #0x0a0]")    // v26  = EC2, v27  = EC1
            __ASM_EMIT("ldp             q28, q29, [%[P2C], #0x0c0]")    // v28  = EC0, v29  = EC6
            __ASM_EMIT("ldp             q30, q31, [%[P2C], #0x0e0]")    // v30  = MM , v31  = ME
            __ASM_EMIT("ldp             q12, q13, [%[P2C], #0x100]")    // v12  = C7 , v13  = L2
            __ASM_EMIT("subs            %[count], %[count], #8")
            __ASM_EMIT("b.lo            2f")
            // x8 blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("ldp             q0, q1, [%[dst]]")
            LOGN_CORE_X8("v30", "v31", "v16", "v17", "v18", "v19", "v20", "v21", "v22", "v12")
            __ASM_EMIT("fmla            v2.4s, v0.4s, v14.4s")          // v2 = 2*y*L*log2(E) + R
            __ASM_EMIT("fmla            v3.4s, v1.4s, v14.4s")
            __ASM_EMIT("fmul            v0.4s, v2.4s, v15.4s")          // q0 = (2*y*L*log2(E) + R)*v = log(c[i])*v
            __ASM_EMIT("fmul            v1.4s, v3.4s, v15.4s")
            POW2_CORE_X8("v31", "v13", "v23", "v24", "v25", "v26", "v27", "v28", "v29", "v12")
            __ASM_EMIT("subs            %[count], %[count], #8")
            __ASM_EMIT("stp             q0, q1, [%[dst]]")
            __ASM_EMIT("add             %[dst], %[dst], #0x20")
            __ASM_EMIT("b.hs            1b")
            // x4 block
            __ASM_EMIT("2:")
            __ASM_EMIT("adds            %[count], %[count], #4")
            __ASM_EMIT("b.lt            4f")
            __ASM_EMIT("ldr             q0, [%[dst]]")
            LOGN_CORE_X4("v30", "v31", "v16", "v17", "v18", "v19", "v20", "v21", "v22", "v12")
            __ASM_EMIT("fmla            v2.4s, v0.4s, v14.4s")          // v2 = 2*y*L*log2(E) + R
            __ASM_EMIT("fmul            v0.4s, v2.4s, v15.4s")          // q0 = (2*y*L*log2(E) + R)*v = log(c[i])*v
            POW2_CORE_X4("v31", "v13", "v23", "v24", "v25", "v26", "v27", "v28", "v29", "v12")
            __ASM_EMIT("subs            %[count], %[count], #4")
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
            LOGN_CORE_X4("v30", "v31", "v16", "v17", "v18", "v19", "v20", "v21", "v22", "v12")
            __ASM_EMIT("fmla            v2.4s, v0.4s, v14.4s")          // v2 = 2*y*L*log2(E) + R
            __ASM_EMIT("fmul            v0.4s, v2.4s, v15.4s")          // q0 = (2*y*L*log2(E) + R)*v = log(c[i])*v
            POW2_CORE_X4("v31", "v13", "v23", "v24", "v25", "v26", "v27", "v28", "v29", "v12")
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

            : [dst] "+r" (c), [V] "+w" (v),
              [count] "+r" (count)
            : [P2C] "r" (&POW_CONST[0])
            : "cc", "memory",
              "v1", "v2", "v3",
              "v4", "v5", "v6", "v7",
              "v8", "v9", "v10", "v11",
              "v12", "v13", "v14", "v15",
              "v16", "v17", "v18", "v19",
              "v20", "v21", "v22", "v23",
              "v24", "v25", "v26", "v27",
              "v28", "v29", "v30", "v31"
        );
    }

    void powvx2(float *dst, const float *v, const float *x, size_t count)
    {
//        for (size_t i=0; i<count; ++i)
//            dst[i] = expf(x[i] * logf(v[i]));
        ARCH_AARCH64_ASM(
            __ASM_EMIT("ldp             q16, q17, [%[P2C], #0x000]")    // v16  = LC0, v17  = LC1
            __ASM_EMIT("ldp             q18, q19, [%[P2C], #0x020]")    // v18  = LC2, v19  = LC3
            __ASM_EMIT("ldp             q20, q21, [%[P2C], #0x040]")    // v20  = LC4, v21  = LC5
            __ASM_EMIT("ldp             q22, q23, [%[P2C], #0x060]")    // v22  = LC6, v23  = EC5
            __ASM_EMIT("ldp             q24, q25, [%[P2C], #0x080]")    // v24  = EC4, v25  = EC3
            __ASM_EMIT("ldp             q26, q27, [%[P2C], #0x0a0]")    // v26  = EC2, v27  = EC1
            __ASM_EMIT("ldp             q28, q29, [%[P2C], #0x0c0]")    // v28  = EC0, v29  = EC6
            __ASM_EMIT("ldp             q30, q31, [%[P2C], #0x0e0]")    // v30  = MM , v31  = ME
            __ASM_EMIT("ldp             q12, q13, [%[P2C], #0x100]")    // v12  = C7 , v13  = L2
            __ASM_EMIT("ldr             q14, [%[P2C], #0x120]")         // v14  = LE
            __ASM_EMIT("subs            %[count], %[count], #8")
            __ASM_EMIT("b.lo            2f")
            // x8 blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("ldp             q0, q1, [%[src]]")
            LOGN_CORE_X8("v30", "v31", "v16", "v17", "v18", "v19", "v20", "v21", "v22", "v12")
            __ASM_EMIT("ldp             q4, q5, [%[x]]")                // q4 = x
            __ASM_EMIT("fmla            v2.4s, v0.4s, v14.4s")          // v2 = 2*y*L*log2(E) + R
            __ASM_EMIT("fmla            v3.4s, v1.4s, v14.4s")
            __ASM_EMIT("fmul            v0.4s, v2.4s, v4.4s")           // q0 = (2*y*L*log2(E) + R)*x = log(v[i])*x[i]
            __ASM_EMIT("fmul            v1.4s, v3.4s, v5.4s")
            POW2_CORE_X8("v31", "v13", "v23", "v24", "v25", "v26", "v27", "v28", "v29", "v12")
            __ASM_EMIT("subs            %[count], %[count], #8")
            __ASM_EMIT("stp             q0, q1, [%[dst]]")
            __ASM_EMIT("add             %[src], %[src], #0x20")
            __ASM_EMIT("add             %[dst], %[dst], #0x20")
            __ASM_EMIT("add             %[x], %[x], #0x20")
            __ASM_EMIT("b.hs            1b")
            // x4 block
            __ASM_EMIT("2:")
            __ASM_EMIT("adds            %[count], %[count], #4")
            __ASM_EMIT("b.lt            4f")
            __ASM_EMIT("ldr             q0, [%[src]]")
            LOGN_CORE_X8("v30", "v31", "v16", "v17", "v18", "v19", "v20", "v21", "v22", "v12")
            __ASM_EMIT("ldr             q4, [%[x]]")                    // q4 = x
            __ASM_EMIT("fmla            v2.4s, v0.4s, v14.4s")          // v2 = 2*y*L*log2(E) + R
            __ASM_EMIT("fmul            v0.4s, v2.4s, v4.4s")           // q0 = (2*y*L*log2(E) + R)*x = log(v[i])*x[i]
            POW2_CORE_X8("v31", "v13", "v23", "v24", "v25", "v26", "v27", "v28", "v29", "v12")
            __ASM_EMIT("subs            %[count], %[count], #4")
            __ASM_EMIT("str             q0, [%[dst]]")
            __ASM_EMIT("add             %[src], %[src], #0x10")
            __ASM_EMIT("add             %[dst], %[dst], #0x10")
            __ASM_EMIT("add             %[x], %[x], #0x10")
            // Tail: 1x-3x block
            __ASM_EMIT("4:")
            __ASM_EMIT("adds            %[count], %[count], #4")
            __ASM_EMIT("b.ls            12f")
            __ASM_EMIT("tst             %[count], #1")
            __ASM_EMIT("b.eq            6f")
            __ASM_EMIT("ld1             {v0.s}[0], [%[src]]")
            __ASM_EMIT("ld1             {v15.s}[0], [%[x]]")
            __ASM_EMIT("add             %[src], %[src], #0x04")
            __ASM_EMIT("add             %[x], %[x], #0x04")
            __ASM_EMIT("6:")
            __ASM_EMIT("tst             %[count], #2")
            __ASM_EMIT("b.eq            8f")
            __ASM_EMIT("ld1             {v0.d}[1], [%[src]]")
            __ASM_EMIT("ld1             {v15.d}[1], [%[x]]")
            __ASM_EMIT("8:")
            LOGN_CORE_X4("v30", "v31", "v16", "v17", "v18", "v19", "v20", "v21", "v22", "v12")
            __ASM_EMIT("fmla            v2.4s, v0.4s, v14.4s")          // v2 = 2*y*L*log2(E) + R
            __ASM_EMIT("fmul            v0.4s, v2.4s, v15.4s")          // q0 = (2*y*L*log2(E) + R)*v = log(c[i])*v
            POW2_CORE_X4("v31", "v13", "v23", "v24", "v25", "v26", "v27", "v28", "v29", "v12")
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

            : [dst] "+r" (dst), [src] "+r" (v), [x] "+r" (x),
              [count] "+r" (count)
            : [P2C] "r" (&POW_CONST[0])
            : "cc", "memory",
              "v0", "v1", "v2", "v3",
              "v4", "v5", "v6", "v7",
              "v8", "v9", "v10", "v11",
              "v12", "v13", "v14", "v15",
              "v16", "v17", "v18", "v19",
              "v20", "v21", "v22", "v23",
              "v24", "v25", "v26", "v27",
              "v28", "v29", "v30", "v31"
        );
    }

    void powvx1(float *v, const float *x, size_t count)
    {
//        for (size_t i=0; i<count; ++i)
//            dst[i] = expf(x[i] * logf(v[i]));
        ARCH_AARCH64_ASM(
            __ASM_EMIT("ldp             q16, q17, [%[P2C], #0x000]")    // v16  = LC0, v17  = LC1
            __ASM_EMIT("ldp             q18, q19, [%[P2C], #0x020]")    // v18  = LC2, v19  = LC3
            __ASM_EMIT("ldp             q20, q21, [%[P2C], #0x040]")    // v20  = LC4, v21  = LC5
            __ASM_EMIT("ldp             q22, q23, [%[P2C], #0x060]")    // v22  = LC6, v23  = EC5
            __ASM_EMIT("ldp             q24, q25, [%[P2C], #0x080]")    // v24  = EC4, v25  = EC3
            __ASM_EMIT("ldp             q26, q27, [%[P2C], #0x0a0]")    // v26  = EC2, v27  = EC1
            __ASM_EMIT("ldp             q28, q29, [%[P2C], #0x0c0]")    // v28  = EC0, v29  = EC6
            __ASM_EMIT("ldp             q30, q31, [%[P2C], #0x0e0]")    // v30  = MM , v31  = ME
            __ASM_EMIT("ldp             q12, q13, [%[P2C], #0x100]")    // v12  = C7 , v13  = L2
            __ASM_EMIT("ldr             q14, [%[P2C], #0x120]")         // v14  = LE
            __ASM_EMIT("subs            %[count], %[count], #8")
            __ASM_EMIT("b.lo            2f")
            // x8 blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("ldp             q0, q1, [%[dst]]")
            LOGN_CORE_X8("v30", "v31", "v16", "v17", "v18", "v19", "v20", "v21", "v22", "v12")
            __ASM_EMIT("ldp             q4, q5, [%[x]]")                // q4 = x
            __ASM_EMIT("fmla            v2.4s, v0.4s, v14.4s")          // v2 = 2*y*L*log2(E) + R
            __ASM_EMIT("fmla            v3.4s, v1.4s, v14.4s")
            __ASM_EMIT("fmul            v0.4s, v2.4s, v4.4s")           // q0 = (2*y*L*log2(E) + R)*x = log(v[i])*x[i]
            __ASM_EMIT("fmul            v1.4s, v3.4s, v5.4s")
            POW2_CORE_X8("v31", "v13", "v23", "v24", "v25", "v26", "v27", "v28", "v29", "v12")
            __ASM_EMIT("subs            %[count], %[count], #8")
            __ASM_EMIT("stp             q0, q1, [%[dst]]")
            __ASM_EMIT("add             %[dst], %[dst], #0x20")
            __ASM_EMIT("add             %[x], %[x], #0x20")
            __ASM_EMIT("b.hs            1b")
            // x4 block
            __ASM_EMIT("2:")
            __ASM_EMIT("adds            %[count], %[count], #4")
            __ASM_EMIT("b.lt            4f")
            __ASM_EMIT("ldr             q0, [%[dst]]")
            LOGN_CORE_X8("v30", "v31", "v16", "v17", "v18", "v19", "v20", "v21", "v22", "v12")
            __ASM_EMIT("ldr             q4, [%[x]]")                    // q4 = x
            __ASM_EMIT("fmla            v2.4s, v0.4s, v14.4s")          // v2 = 2*y*L*log2(E) + R
            __ASM_EMIT("fmul            v0.4s, v2.4s, v4.4s")           // q0 = (2*y*L*log2(E) + R)*x = log(v[i])*x[i]
            POW2_CORE_X8("v31", "v13", "v23", "v24", "v25", "v26", "v27", "v28", "v29", "v12")
            __ASM_EMIT("subs            %[count], %[count], #4")
            __ASM_EMIT("str             q0, [%[dst]]")
            __ASM_EMIT("add             %[dst], %[dst], #0x10")
            __ASM_EMIT("add             %[x], %[x], #0x10")
            // Tail: 1x-3x block
            __ASM_EMIT("4:")
            __ASM_EMIT("adds            %[count], %[count], #4")
            __ASM_EMIT("b.ls            12f")
            __ASM_EMIT("tst             %[count], #1")
            __ASM_EMIT("b.eq            6f")
            __ASM_EMIT("ld1             {v0.s}[0], [%[dst]]")
            __ASM_EMIT("ld1             {v15.s}[0], [%[x]]")
            __ASM_EMIT("add             %[dst], %[dst], #0x04")
            __ASM_EMIT("add             %[x], %[x], #0x04")
            __ASM_EMIT("6:")
            __ASM_EMIT("tst             %[count], #2")
            __ASM_EMIT("b.eq            8f")
            __ASM_EMIT("ld1             {v0.d}[1], [%[dst]]")
            __ASM_EMIT("ld1             {v15.d}[1], [%[x]]")
            __ASM_EMIT("8:")
            LOGN_CORE_X4("v30", "v31", "v16", "v17", "v18", "v19", "v20", "v21", "v22", "v12")
            __ASM_EMIT("fmla            v2.4s, v0.4s, v14.4s")          // v2 = 2*y*L*log2(E) + R
            __ASM_EMIT("fmul            v0.4s, v2.4s, v15.4s")          // q0 = (2*y*L*log2(E) + R)*v = log(c[i])*v
            POW2_CORE_X4("v31", "v13", "v23", "v24", "v25", "v26", "v27", "v28", "v29", "v12")
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

            : [dst] "+r" (v), [x] "+r" (x),
              [count] "+r" (count)
            : [P2C] "r" (&POW_CONST[0])
            : "cc", "memory",
              "v0", "v1", "v2", "v3",
              "v4", "v5", "v6", "v7",
              "v8", "v9", "v10", "v11",
              "v12", "v13", "v14", "v15",
              "v16", "v17", "v18", "v19",
              "v20", "v21", "v22", "v23",
              "v24", "v25", "v26", "v27",
              "v28", "v29", "v30", "v31"
        );
    }
}



#endif /* DSP_ARCH_AARCH64_ASIMD_PMATH_POW_H_ */
