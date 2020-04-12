/*
 * hdotp.h
 *
 *  Created on: 30 нояб. 2019 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_AARCH64_ASIMD_HMATH_HDOTP_H_
#define DSP_ARCH_AARCH64_ASIMD_HMATH_HDOTP_H_

#ifndef DSP_ARCH_AARCH64_ASIMD_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_AARCH64_ASIMD_IMPL */

namespace asimd
{
    float h_dotp(const float *a, const float *b, size_t count)
    {
        float res;
        ARCH_AARCH64_ASM
        (
            __ASM_EMIT("eor         v16.16b, v16.16b, v16.16b")
            __ASM_EMIT("eor         v17.16b, v17.16b, v17.16b")
            __ASM_EMIT("subs        %[count], %[count], #16")
            __ASM_EMIT("b.lt        2f")
            /* 16x block */
            __ASM_EMIT("1:")
            __ASM_EMIT("ldp         q0, q1, [%[a], #0x00]")
            __ASM_EMIT("ldp         q2, q3, [%[a], #0x20]")
            __ASM_EMIT("ldp         q4, q5, [%[b], #0x00]")
            __ASM_EMIT("ldp         q6, q7, [%[b], #0x20]")
            __ASM_EMIT("fmla        v16.4s, v0.4s, v4.4s")
            __ASM_EMIT("fmla        v17.4s, v1.4s, v5.4s")
            __ASM_EMIT("fmla        v16.4s, v2.4s, v6.4s")
            __ASM_EMIT("fmla        v17.4s, v3.4s, v7.4s")
            __ASM_EMIT("subs        %[count], %[count], #16")
            __ASM_EMIT("add         %[a], %[a], #0x40")
            __ASM_EMIT("add         %[b], %[b], #0x40")
            __ASM_EMIT("b.hs        1b")
            /* 8x block */
            __ASM_EMIT("2:")
            __ASM_EMIT("adds        %[count], %[count], #8")
            __ASM_EMIT("b.lt        4f")
            __ASM_EMIT("ldp         q0, q1, [%[a], #0x00]")
            __ASM_EMIT("ldp         q4, q5, [%[b], #0x00]")
            __ASM_EMIT("fmla        v16.4s, v0.4s, v4.4s")
            __ASM_EMIT("fmla        v17.4s, v1.4s, v5.4s")
            __ASM_EMIT("sub         %[count], %[count], #8")
            __ASM_EMIT("add         %[a], %[a], #0x20")
            __ASM_EMIT("add         %[b], %[b], #0x20")
            /* 4x block */
            __ASM_EMIT("4:")
            __ASM_EMIT("adds        %[count], %[count], #4")
            __ASM_EMIT("fadd        v16.4s, v16.4s, v17.4s")
            __ASM_EMIT("b.lt        6f")
            __ASM_EMIT("ldr         q0, [%[a], #0x00]")
            __ASM_EMIT("ldr         q4, [%[b], #0x00]")
            __ASM_EMIT("fmla        v16.4s, v0.4s, v4.4s")
            __ASM_EMIT("sub         %[count], %[count], #4")
            __ASM_EMIT("add         %[a], %[a], #0x10")
            __ASM_EMIT("add         %[b], %[b], #0x10")
            /* 1x block */
            __ASM_EMIT("6:")
            __ASM_EMIT("ext         v17.16b, v16.16b, v16.16b, #8")
            __ASM_EMIT("fadd        v16.4s, v16.4s, v17.4s")
            __ASM_EMIT("ext         v17.16b, v16.16b, v16.16b, #4")
            __ASM_EMIT("fadd        %S[res].4s, v16.4s, v17.4s")
            __ASM_EMIT("adds        %[count], %[count], #3")
            __ASM_EMIT("b.lt        8f")
            __ASM_EMIT("7:")
            __ASM_EMIT("ld1         {v1.s}[0], [%[a]]")
            __ASM_EMIT("ld1         {v4.s}[0], [%[b]]")
            __ASM_EMIT("fmla        %S[res].4s, v1.4s, v4.4s")
            __ASM_EMIT("subs        %[count], %[count], #1")
            __ASM_EMIT("add         %[a], %[a], #0x04")
            __ASM_EMIT("add         %[b], %[b], #0x04")
            __ASM_EMIT("b.ge        7b")
            /* end of sum */
            __ASM_EMIT("8:")
            : [res] "=w" (res),
              [a] "+r" (a), [b] "+r" (b), [count] "+r" (count)
            :
            : "cc", "memory",
              "v1", "v2", "v3", "v4", "v5", "v6", "v7",
              "v16", "v17"
        );

        return res;
    }

    float h_sqr_dotp(const float *a, const float *b, size_t count)
    {
        float res;
        ARCH_AARCH64_ASM
        (
            __ASM_EMIT("eor         v16.16b, v16.16b, v16.16b")
            __ASM_EMIT("eor         v17.16b, v17.16b, v17.16b")
            __ASM_EMIT("subs        %[count], %[count], #16")
            __ASM_EMIT("b.lt        2f")
            /* 16x block */
            __ASM_EMIT("1:")
            __ASM_EMIT("ldp         q0, q1, [%[a], #0x00]")
            __ASM_EMIT("ldp         q2, q3, [%[a], #0x20]")
            __ASM_EMIT("ldp         q4, q5, [%[b], #0x00]")
            __ASM_EMIT("ldp         q6, q7, [%[b], #0x20]")
            __ASM_EMIT("fmul        v0.4s, v0.4s, v0.4s")
            __ASM_EMIT("fmul        v1.4s, v1.4s, v1.4s")
            __ASM_EMIT("fmul        v2.4s, v2.4s, v2.4s")
            __ASM_EMIT("fmul        v3.4s, v3.4s, v3.4s")
            __ASM_EMIT("fmul        v4.4s, v4.4s, v4.4s")
            __ASM_EMIT("fmul        v5.4s, v5.4s, v5.4s")
            __ASM_EMIT("fmul        v6.4s, v6.4s, v6.4s")
            __ASM_EMIT("fmul        v7.4s, v7.4s, v7.4s")
            __ASM_EMIT("fmla        v16.4s, v0.4s, v4.4s")
            __ASM_EMIT("fmla        v17.4s, v1.4s, v5.4s")
            __ASM_EMIT("fmla        v16.4s, v2.4s, v6.4s")
            __ASM_EMIT("fmla        v17.4s, v3.4s, v7.4s")
            __ASM_EMIT("subs        %[count], %[count], #16")
            __ASM_EMIT("add         %[a], %[a], #0x40")
            __ASM_EMIT("add         %[b], %[b], #0x40")
            __ASM_EMIT("b.hs        1b")
            /* 8x block */
            __ASM_EMIT("2:")
            __ASM_EMIT("adds        %[count], %[count], #8")
            __ASM_EMIT("b.lt        4f")
            __ASM_EMIT("ldp         q0, q1, [%[a], #0x00]")
            __ASM_EMIT("ldp         q4, q5, [%[b], #0x00]")
            __ASM_EMIT("fmul        v0.4s, v0.4s, v0.4s")
            __ASM_EMIT("fmul        v1.4s, v1.4s, v1.4s")
            __ASM_EMIT("fmul        v4.4s, v4.4s, v4.4s")
            __ASM_EMIT("fmul        v5.4s, v5.4s, v5.4s")
            __ASM_EMIT("fmla        v16.4s, v0.4s, v4.4s")
            __ASM_EMIT("fmla        v17.4s, v1.4s, v5.4s")
            __ASM_EMIT("sub         %[count], %[count], #8")
            __ASM_EMIT("add         %[a], %[a], #0x20")
            __ASM_EMIT("add         %[b], %[b], #0x20")
            /* 4x block */
            __ASM_EMIT("4:")
            __ASM_EMIT("adds        %[count], %[count], #4")
            __ASM_EMIT("fadd        v16.4s, v16.4s, v17.4s")
            __ASM_EMIT("b.lt        6f")
            __ASM_EMIT("ldr         q0, [%[a], #0x00]")
            __ASM_EMIT("ldr         q4, [%[b], #0x00]")
            __ASM_EMIT("fmul        v0.4s, v0.4s, v0.4s")
            __ASM_EMIT("fmul        v4.4s, v4.4s, v4.4s")
            __ASM_EMIT("fmla        v16.4s, v0.4s, v4.4s")
            __ASM_EMIT("sub         %[count], %[count], #4")
            __ASM_EMIT("add         %[a], %[a], #0x10")
            __ASM_EMIT("add         %[b], %[b], #0x10")
            /* 1x block */
            __ASM_EMIT("6:")
            __ASM_EMIT("ext         v17.16b, v16.16b, v16.16b, #8")
            __ASM_EMIT("fadd        v16.4s, v16.4s, v17.4s")
            __ASM_EMIT("ext         v17.16b, v16.16b, v16.16b, #4")
            __ASM_EMIT("fadd        %S[res].4s, v16.4s, v17.4s")
            __ASM_EMIT("adds        %[count], %[count], #3")
            __ASM_EMIT("b.lt        8f")
            __ASM_EMIT("7:")
            __ASM_EMIT("ld1         {v1.s}[0], [%[a]]")
            __ASM_EMIT("ld1         {v4.s}[0], [%[b]]")
            __ASM_EMIT("fmul        v1.4s, v1.4s, v1.4s")
            __ASM_EMIT("fmul        v4.4s, v4.4s, v4.4s")
            __ASM_EMIT("fmla        %S[res].4s, v1.4s, v4.4s")
            __ASM_EMIT("subs        %[count], %[count], #1")
            __ASM_EMIT("add         %[a], %[a], #0x04")
            __ASM_EMIT("add         %[b], %[b], #0x04")
            __ASM_EMIT("b.ge        7b")
            /* end of sum */
            __ASM_EMIT("8:")

            : [res] "=w" (res),
              [a] "+r" (a), [b] "+r" (b), [count] "+r" (count)
            :
            : "cc", "memory",
              "v1", "v2", "v3", "v4", "v5", "v6", "v7",
              "v16", "v17"
        );

        return res;
    }

    float h_abs_dotp(const float *a, const float *b, size_t count)
    {
        IF_ARCH_AARCH64(float res);
        ARCH_AARCH64_ASM
        (
            __ASM_EMIT("eor         v16.16b, v16.16b, v16.16b")
            __ASM_EMIT("eor         v17.16b, v17.16b, v17.16b")
            __ASM_EMIT("subs        %[count], %[count], #16")
            __ASM_EMIT("b.lt        2f")
            /* 16x block */
            __ASM_EMIT("1:")
            __ASM_EMIT("ldp         q0, q1, [%[a], #0x00]")
            __ASM_EMIT("ldp         q2, q3, [%[a], #0x20]")
            __ASM_EMIT("ldp         q4, q5, [%[b], #0x00]")
            __ASM_EMIT("ldp         q6, q7, [%[b], #0x20]")
            __ASM_EMIT("fabs        v0.4s, v0.4s")
            __ASM_EMIT("fabs        v1.4s, v1.4s")
            __ASM_EMIT("fabs        v2.4s, v2.4s")
            __ASM_EMIT("fabs        v3.4s, v3.4s")
            __ASM_EMIT("fabs        v4.4s, v4.4s")
            __ASM_EMIT("fabs        v5.4s, v5.4s")
            __ASM_EMIT("fabs        v6.4s, v6.4s")
            __ASM_EMIT("fabs        v7.4s, v7.4s")
            __ASM_EMIT("fmla        v16.4s, v0.4s, v4.4s")
            __ASM_EMIT("fmla        v17.4s, v1.4s, v5.4s")
            __ASM_EMIT("fmla        v16.4s, v2.4s, v6.4s")
            __ASM_EMIT("fmla        v17.4s, v3.4s, v7.4s")
            __ASM_EMIT("subs        %[count], %[count], #16")
            __ASM_EMIT("add         %[a], %[a], #0x40")
            __ASM_EMIT("add         %[b], %[b], #0x40")
            __ASM_EMIT("b.hs        1b")
            /* 8x block */
            __ASM_EMIT("2:")
            __ASM_EMIT("adds        %[count], %[count], #8")
            __ASM_EMIT("b.lt        4f")
            __ASM_EMIT("ldp         q0, q1, [%[a], #0x00]")
            __ASM_EMIT("ldp         q4, q5, [%[b], #0x00]")
            __ASM_EMIT("fabs        v0.4s, v0.4s")
            __ASM_EMIT("fabs        v1.4s, v1.4s")
            __ASM_EMIT("fabs        v4.4s, v4.4s")
            __ASM_EMIT("fabs        v5.4s, v5.4s")
            __ASM_EMIT("fmla        v16.4s, v0.4s, v4.4s")
            __ASM_EMIT("fmla        v17.4s, v1.4s, v5.4s")
            __ASM_EMIT("sub         %[count], %[count], #8")
            __ASM_EMIT("add         %[a], %[a], #0x20")
            __ASM_EMIT("add         %[b], %[b], #0x20")
            /* 4x block */
            __ASM_EMIT("4:")
            __ASM_EMIT("adds        %[count], %[count], #4")
            __ASM_EMIT("fadd        v16.4s, v16.4s, v17.4s")
            __ASM_EMIT("b.lt        6f")
            __ASM_EMIT("ldr         q0, [%[a], #0x00]")
            __ASM_EMIT("ldr         q4, [%[b], #0x00]")
            __ASM_EMIT("fabs        v0.4s, v0.4s")
            __ASM_EMIT("fabs        v4.4s, v4.4s")
            __ASM_EMIT("fmla        v16.4s, v0.4s, v4.4s")
            __ASM_EMIT("sub         %[count], %[count], #4")
            __ASM_EMIT("add         %[a], %[a], #0x10")
            __ASM_EMIT("add         %[b], %[b], #0x10")
            /* 1x block */
            __ASM_EMIT("6:")
            __ASM_EMIT("ext         v17.16b, v16.16b, v16.16b, #8")
            __ASM_EMIT("fadd        v16.4s, v16.4s, v17.4s")
            __ASM_EMIT("ext         v17.16b, v16.16b, v16.16b, #4")
            __ASM_EMIT("fadd        %S[res].4s, v16.4s, v17.4s")
            __ASM_EMIT("adds        %[count], %[count], #3")
            __ASM_EMIT("b.lt        8f")
            __ASM_EMIT("7:")
            __ASM_EMIT("ld1         {v1.s}[0], [%[a]]")
            __ASM_EMIT("ld1         {v4.s}[0], [%[b]]")
            __ASM_EMIT("fabs        v1.4s, v1.4s")
            __ASM_EMIT("fabs        v4.4s, v4.4s")
            __ASM_EMIT("fmla        %S[res].4s, v1.4s, v4.4s")
            __ASM_EMIT("subs        %[count], %[count], #1")
            __ASM_EMIT("add         %[a], %[a], #0x04")
            __ASM_EMIT("add         %[b], %[b], #0x04")
            __ASM_EMIT("b.ge        7b")
            /* end of sum */
            __ASM_EMIT("8:")

            : [res] "=w" (res),
              [a] "+r" (a), [b] "+r" (b), [count] "+r" (count)
            :
            : "cc", "memory",
              "v1", "v2", "v3", "v4", "v5", "v6", "v7",
              "v16", "v17"
        );

        return res;
    }
}

#endif /* DSP_ARCH_AARCH64_ASIMD_HMATH_HDOTP_H_ */
