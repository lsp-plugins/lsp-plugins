/*
 * hsum.h
 *
 *  Created on: 29 нояб. 2019 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_AARCH64_ASIMD_HMATH_HSUM_H_
#define DSP_ARCH_AARCH64_ASIMD_HMATH_HSUM_H_

#ifndef DSP_ARCH_AARCH64_ASIMD_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_AARCH64_ASIMD_IMPL */

namespace asimd
{
    float h_sum(const float *src, size_t count)
    {
        IF_ARCH_AARCH64(float res);
        ARCH_AARCH64_ASM
        (
            __ASM_EMIT("eor         v0.16b, v0.16b, v0.16b")
            __ASM_EMIT("eor         v1.16b, v1.16b, v1.16b")
            __ASM_EMIT("subs        %[count], %[count], #24")
            __ASM_EMIT("b.lt        2f")
            /* 24x block */
            __ASM_EMIT("1:")
            __ASM_EMIT("ldp         q2, q3, [%[src], #0x00]")
            __ASM_EMIT("ldp         q4, q5, [%[src], #0x20]")
            __ASM_EMIT("ldp         q6, q7, [%[src], #0x40]")
            __ASM_EMIT("fadd        v0.4s, v0.4s, v2.4s")
            __ASM_EMIT("fadd        v1.4s, v1.4s, v3.4s")
            __ASM_EMIT("fadd        v0.4s, v0.4s, v4.4s")
            __ASM_EMIT("fadd        v1.4s, v1.4s, v5.4s")
            __ASM_EMIT("fadd        v0.4s, v0.4s, v6.4s")
            __ASM_EMIT("fadd        v1.4s, v1.4s, v7.4s")
            __ASM_EMIT("subs        %[count], %[count], #24")
            __ASM_EMIT("add         %[src], %[src], #0x60")
            __ASM_EMIT("b.hs        1b")
            /* 16x block */
            __ASM_EMIT("2:")
            __ASM_EMIT("adds        %[count], %[count], #8")
            __ASM_EMIT("b.lt        4f")
            __ASM_EMIT("ldp         q2, q3, [%[src], #0x00]")
            __ASM_EMIT("ldp         q4, q5, [%[src], #0x20]")
            __ASM_EMIT("fadd        v0.4s, v0.4s, v2.4s")
            __ASM_EMIT("fadd        v1.4s, v1.4s, v3.4s")
            __ASM_EMIT("fadd        v0.4s, v0.4s, v4.4s")
            __ASM_EMIT("fadd        v1.4s, v1.4s, v5.4s")
            __ASM_EMIT("sub         %[count], %[count], #16")
            __ASM_EMIT("add         %[src], %[src], #0x40")
            /* 8x block */
            __ASM_EMIT("4:")
            __ASM_EMIT("adds        %[count], %[count], #8")
            __ASM_EMIT("b.lt        6f")
            __ASM_EMIT("ldp         q2, q3, [%[src], #0x00]")
            __ASM_EMIT("fadd        v0.4s, v0.4s, v2.4s")
            __ASM_EMIT("fadd        v1.4s, v1.4s, v3.4s")
            __ASM_EMIT("sub         %[count], %[count], #8")
            __ASM_EMIT("add         %[src], %[src], #0x20")
            /* 4x block */
            __ASM_EMIT("6:")
            __ASM_EMIT("adds        %[count], %[count], #4")
            __ASM_EMIT("fadd        v0.4s, v0.4s, v1.4s")
            __ASM_EMIT("b.lt        8f")
            __ASM_EMIT("ldr         q2, [%[src], #0x00]")
            __ASM_EMIT("fadd        v0.4s, v0.4s, v2.4s")
            __ASM_EMIT("sub         %[count], %[count], #4")
            __ASM_EMIT("add         %[src], %[src], #0x10")
            /* 1x block */
            __ASM_EMIT("8:")
            __ASM_EMIT("adds        %[count], %[count], #3")
            __ASM_EMIT("b.lt        10f")
            __ASM_EMIT("eor         v2.16b, v2.16b, v2.16b")
            __ASM_EMIT("9:")
            __ASM_EMIT("ld1         {v2.s}[0], [%[src]]")
            __ASM_EMIT("fadd        v0.4s, v0.4s, v2.4s")
            __ASM_EMIT("subs        %[count], %[count], #1")
            __ASM_EMIT("add         %[src], %[src], #0x04")
            __ASM_EMIT("b.ge        9b")
            /* end of sum */
            __ASM_EMIT("10:")
            __ASM_EMIT("ext         v1.16b, v0.16b, v0.16b, #8")    /* v0 = a0 a1 a2 a3, v1 = a2 a3 a0 a1 */
            __ASM_EMIT("fadd        v0.4s, v0.4s, v1.4s")           /* v0 = a0+a2 a1+a3 a0+a2 a1+a3 */
            __ASM_EMIT("ext         v1.16b, v0.16b, v0.16b, #4")    /* v1 = a1+a3 a0+a2 a1+a3 a0+a2 */
            __ASM_EMIT("fadd        %S[res].4s, v0.4s, v1.4s")      /* v0 = a0+a1+a2+a3 */
            : [res] "=w" (res),
              [src] "+r" (src), [count] "+r" (count)
            :
            : "cc", "memory",
              "v1", "v2", "v3", "v4", "v5", "v6", "v7"
        );

        return res;
    }

    float h_sqr_sum(const float *src, size_t count)
    {
        IF_ARCH_AARCH64(float res);
        ARCH_AARCH64_ASM
        (
            __ASM_EMIT("eor         v0.16b, v0.16b, v0.16b")
            __ASM_EMIT("eor         v1.16b, v1.16b, v1.16b")
            __ASM_EMIT("subs        %[count], %[count], #24")
            __ASM_EMIT("b.lt        2f")
            /* 24x block */
            __ASM_EMIT("1:")
            __ASM_EMIT("ldp         q2, q3, [%[src], #0x00]")
            __ASM_EMIT("ldp         q4, q5, [%[src], #0x20]")
            __ASM_EMIT("ldp         q6, q7, [%[src], #0x40]")
            __ASM_EMIT("fmla        v0.4s, v2.4s, v2.4s")
            __ASM_EMIT("fmla        v1.4s, v3.4s, v3.4s")
            __ASM_EMIT("fmla        v0.4s, v4.4s, v4.4s")
            __ASM_EMIT("fmla        v1.4s, v5.4s, v5.4s")
            __ASM_EMIT("fmla        v0.4s, v6.4s, v6.4s")
            __ASM_EMIT("fmla        v1.4s, v7.4s, v7.4s")
            __ASM_EMIT("subs        %[count], %[count], #24")
            __ASM_EMIT("add         %[src], %[src], #0x60")
            __ASM_EMIT("b.hs        1b")
            /* 16x block */
            __ASM_EMIT("2:")
            __ASM_EMIT("adds        %[count], %[count], #8")
            __ASM_EMIT("b.lt        4f")
            __ASM_EMIT("ldp         q2, q3, [%[src], #0x00]")
            __ASM_EMIT("ldp         q4, q5, [%[src], #0x20]")
            __ASM_EMIT("fmla        v0.4s, v2.4s, v2.4s")
            __ASM_EMIT("fmla        v1.4s, v3.4s, v3.4s")
            __ASM_EMIT("fmla        v0.4s, v4.4s, v4.4s")
            __ASM_EMIT("fmla        v1.4s, v5.4s, v5.4s")
            __ASM_EMIT("sub         %[count], %[count], #16")
            __ASM_EMIT("add         %[src], %[src], #0x40")
            /* 8x block */
            __ASM_EMIT("4:")
            __ASM_EMIT("adds        %[count], %[count], #8")
            __ASM_EMIT("b.lt        6f")
            __ASM_EMIT("ldp         q2, q3, [%[src], #0x00]")
            __ASM_EMIT("fmla        v0.4s, v2.4s, v2.4s")
            __ASM_EMIT("fmla        v1.4s, v3.4s, v3.4s")
            __ASM_EMIT("sub         %[count], %[count], #8")
            __ASM_EMIT("add         %[src], %[src], #0x20")
            /* 4x block */
            __ASM_EMIT("6:")
            __ASM_EMIT("adds        %[count], %[count], #4")
            __ASM_EMIT("fadd        v0.4s, v0.4s, v1.4s")
            __ASM_EMIT("b.lt        8f")
            __ASM_EMIT("ldr         q2, [%[src], #0x00]")
            __ASM_EMIT("fmla        v0.4s, v2.4s, v2.4s")
            __ASM_EMIT("sub         %[count], %[count], #4")
            __ASM_EMIT("add         %[src], %[src], #0x10")
            /* 1x block */
            __ASM_EMIT("8:")
            __ASM_EMIT("adds        %[count], %[count], #3")
            __ASM_EMIT("b.lt        10f")
            __ASM_EMIT("eor         v2.16b, v2.16b, v2.16b")
            __ASM_EMIT("9:")
            __ASM_EMIT("ld1         {v2.s}[0], [%[src]]")
            __ASM_EMIT("fmla        v0.4s, v2.4s, v2.4s")
            __ASM_EMIT("subs        %[count], %[count], #1")
            __ASM_EMIT("add         %[src], %[src], #0x04")
            __ASM_EMIT("b.ge        9b")
            /* end of sum */
            __ASM_EMIT("10:")
            __ASM_EMIT("ext         v1.16b, v0.16b, v0.16b, #8")    /* v0 = a0 a1 a2 a3, v1 = a2 a3 a0 a1 */
            __ASM_EMIT("fadd        v0.4s, v0.4s, v1.4s")           /* v0 = a0+a2 a1+a3 a0+a2 a1+a3 */
            __ASM_EMIT("ext         v1.16b, v0.16b, v0.16b, #4")    /* v1 = a1+a3 a0+a2 a1+a3 a0+a2 */
            __ASM_EMIT("fadd        %S[res].4s, v0.4s, v1.4s")      /* v0 = a0+a1+a2+a3 */
            : [res] "=w" (res),
              [src] "+r" (src), [count] "+r" (count)
            :
            : "cc", "memory",
              "v1", "v2", "v3", "v4", "v5", "v6", "v7"
        );

        return res;
    }

    float h_abs_sum(const float *src, size_t count)
    {
        IF_ARCH_AARCH64(float res);
        ARCH_AARCH64_ASM
        (
            __ASM_EMIT("eor         v0.16b, v0.16b, v0.16b")
            __ASM_EMIT("eor         v1.16b, v1.16b, v1.16b")
            __ASM_EMIT("subs        %[count], %[count], #24")
            __ASM_EMIT("b.lt        2f")
            /* 24x block */
            __ASM_EMIT("1:")
            __ASM_EMIT("ldp         q2, q3, [%[src], #0x00]")
            __ASM_EMIT("ldp         q4, q5, [%[src], #0x20]")
            __ASM_EMIT("ldp         q6, q7, [%[src], #0x40]")
            __ASM_EMIT("fabs        v2.4s, v2.4s")
            __ASM_EMIT("fabs        v3.4s, v3.4s")
            __ASM_EMIT("fabs        v4.4s, v4.4s")
            __ASM_EMIT("fabs        v5.4s, v5.4s")
            __ASM_EMIT("fabs        v6.4s, v6.4s")
            __ASM_EMIT("fabs        v7.4s, v7.4s")
            __ASM_EMIT("fadd        v0.4s, v0.4s, v2.4s")
            __ASM_EMIT("fadd        v1.4s, v1.4s, v3.4s")
            __ASM_EMIT("fadd        v0.4s, v0.4s, v4.4s")
            __ASM_EMIT("fadd        v1.4s, v1.4s, v5.4s")
            __ASM_EMIT("fadd        v0.4s, v0.4s, v6.4s")
            __ASM_EMIT("fadd        v1.4s, v1.4s, v7.4s")
            __ASM_EMIT("subs        %[count], %[count], #24")
            __ASM_EMIT("add         %[src], %[src], #0x60")
            __ASM_EMIT("b.hs        1b")
            /* 16x block */
            __ASM_EMIT("2:")
            __ASM_EMIT("adds        %[count], %[count], #8")
            __ASM_EMIT("b.lt        4f")
            __ASM_EMIT("ldp         q2, q3, [%[src], #0x00]")
            __ASM_EMIT("ldp         q4, q5, [%[src], #0x20]")
            __ASM_EMIT("fabs        v2.4s, v2.4s")
            __ASM_EMIT("fabs        v3.4s, v3.4s")
            __ASM_EMIT("fabs        v4.4s, v4.4s")
            __ASM_EMIT("fabs        v5.4s, v5.4s")
            __ASM_EMIT("fadd        v0.4s, v0.4s, v2.4s")
            __ASM_EMIT("fadd        v1.4s, v1.4s, v3.4s")
            __ASM_EMIT("fadd        v0.4s, v0.4s, v4.4s")
            __ASM_EMIT("fadd        v1.4s, v1.4s, v5.4s")
            __ASM_EMIT("sub         %[count], %[count], #16")
            __ASM_EMIT("add         %[src], %[src], #0x40")
            /* 8x block */
            __ASM_EMIT("4:")
            __ASM_EMIT("adds        %[count], %[count], #8")
            __ASM_EMIT("b.lt        6f")
            __ASM_EMIT("ldp         q2, q3, [%[src], #0x00]")
            __ASM_EMIT("fabs        v2.4s, v2.4s")
            __ASM_EMIT("fabs        v3.4s, v3.4s")
            __ASM_EMIT("fadd        v0.4s, v0.4s, v2.4s")
            __ASM_EMIT("fadd        v1.4s, v1.4s, v3.4s")
            __ASM_EMIT("sub         %[count], %[count], #8")
            __ASM_EMIT("add         %[src], %[src], #0x20")
            /* 4x block */
            __ASM_EMIT("6:")
            __ASM_EMIT("adds        %[count], %[count], #4")
            __ASM_EMIT("fadd        v0.4s, v0.4s, v1.4s")
            __ASM_EMIT("b.lt        8f")
            __ASM_EMIT("ldr         q2, [%[src], #0x00]")
            __ASM_EMIT("fabs        v2.4s, v2.4s")
            __ASM_EMIT("fadd        v0.4s, v0.4s, v2.4s")
            __ASM_EMIT("sub         %[count], %[count], #4")
            __ASM_EMIT("add         %[src], %[src], #0x10")
            /* 1x block */
            __ASM_EMIT("8:")
            __ASM_EMIT("adds        %[count], %[count], #3")
            __ASM_EMIT("b.lt        10f")
            __ASM_EMIT("eor         v2.16b, v2.16b, v2.16b")
            __ASM_EMIT("9:")
            __ASM_EMIT("ld1         {v2.s}[0], [%[src]]")
            __ASM_EMIT("fabs        v2.4s, v2.4s")
            __ASM_EMIT("fadd        v0.4s, v0.4s, v2.4s")
            __ASM_EMIT("subs        %[count], %[count], #1")
            __ASM_EMIT("add         %[src], %[src], #0x04")
            __ASM_EMIT("b.ge        9b")
            /* end of sum */
            __ASM_EMIT("10:")
            __ASM_EMIT("ext         v1.16b, v0.16b, v0.16b, #8")    /* v0 = a0 a1 a2 a3, v1 = a2 a3 a0 a1 */
            __ASM_EMIT("fadd        v0.4s, v0.4s, v1.4s")           /* v0 = a0+a2 a1+a3 a0+a2 a1+a3 */
            __ASM_EMIT("ext         v1.16b, v0.16b, v0.16b, #4")    /* v1 = a1+a3 a0+a2 a1+a3 a0+a2 */
            __ASM_EMIT("fadd        %S[res].4s, v0.4s, v1.4s")      /* v0 = a0+a1+a2+a3 */
            : [res] "=w" (res),
              [src] "+r" (src), [count] "+r" (count)
            :
            : "cc", "memory",
              "v1", "v2", "v3", "v4", "v5", "v6", "v7"
        );

        return res;
    }
}

#endif /* DSP_ARCH_AARCH64_ASIMD_HMATH_HSUM_H_ */
