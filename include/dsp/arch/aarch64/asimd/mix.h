/*
 * mix.h
 *
 *  Created on: 2 дек. 2019 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_AARCH64_ASIMD_MIX_H_
#define DSP_ARCH_AARCH64_ASIMD_MIX_H_

#ifndef DSP_ARCH_AARCH64_ASIMD_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_AARCH64_ASIMD_IMPL */

namespace asimd
{
    void mix2(float *a, const float *b, float k1, float k2, size_t count)
    {
        ARCH_AARCH64_ASM(
            __ASM_EMIT("dup         v28.4s, %S[k1].s[0]")
            __ASM_EMIT("dup         v29.4s, %S[k2].s[0]")
            __ASM_EMIT("subs        %[count], %[count], #16")
            __ASM_EMIT("b.lt        2f")
            // x16 block
            __ASM_EMIT("1:")
            __ASM_EMIT("ldp         q0, q1, [%[a], #0x00]")
            __ASM_EMIT("ldp         q2, q3, [%[a], #0x20]")
            __ASM_EMIT("ldp         q4, q5, [%[b], #0x00]")
            __ASM_EMIT("ldp         q6, q7, [%[b], #0x20]")
            __ASM_EMIT("fmul        v16.4s, v0.4s, v28.4s")
            __ASM_EMIT("fmul        v17.4s, v1.4s, v28.4s")
            __ASM_EMIT("fmul        v18.4s, v2.4s, v28.4s")
            __ASM_EMIT("fmul        v19.4s, v3.4s, v28.4s")
            __ASM_EMIT("fmla        v16.4s, v4.4s, v29.4s")
            __ASM_EMIT("fmla        v17.4s, v5.4s, v29.4s")
            __ASM_EMIT("fmla        v18.4s, v6.4s, v29.4s")
            __ASM_EMIT("fmla        v19.4s, v7.4s, v29.4s")
            __ASM_EMIT("stp         q16, q17, [%[a], #0x00]")
            __ASM_EMIT("stp         q18, q19, [%[a], #0x20]")
            __ASM_EMIT("subs        %[count], %[count], #16")
            __ASM_EMIT("add         %[a], %[a], #0x40")
            __ASM_EMIT("add         %[b], %[b], #0x40")
            __ASM_EMIT("b.hs        1b")
            // x8 block
            __ASM_EMIT("2:")
            __ASM_EMIT("adds        %[count], %[count], #8")
            __ASM_EMIT("b.lt        4f")
            __ASM_EMIT("ldp         q0, q1, [%[a], #0x00]")
            __ASM_EMIT("ldp         q4, q5, [%[b], #0x00]")
            __ASM_EMIT("fmul        v16.4s, v0.4s, v28.4s")
            __ASM_EMIT("fmul        v17.4s, v1.4s, v28.4s")
            __ASM_EMIT("fmla        v16.4s, v4.4s, v29.4s")
            __ASM_EMIT("fmla        v17.4s, v5.4s, v29.4s")
            __ASM_EMIT("stp         q16, q17, [%[a], #0x00]")
            __ASM_EMIT("sub         %[count], %[count], #8")
            __ASM_EMIT("add         %[a], %[a], #0x20")
            __ASM_EMIT("add         %[b], %[b], #0x20")
            // x4 block
            __ASM_EMIT("4:")
            __ASM_EMIT("adds        %[count], %[count], #4")
            __ASM_EMIT("b.lt        6f")
            __ASM_EMIT("ldr         q0, [%[a], #0x00]")
            __ASM_EMIT("ldr         q4, [%[b], #0x00]")
            __ASM_EMIT("fmul        v16.4s, v0.4s, v28.4s")
            __ASM_EMIT("fmla        v16.4s, v4.4s, v29.4s")
            __ASM_EMIT("str         q16, [%[a], #0x00]")
            __ASM_EMIT("sub         %[count], %[count], #4")
            __ASM_EMIT("add         %[a], %[a], #0x10")
            __ASM_EMIT("add         %[b], %[b], #0x10")
            // x1 blocks
            __ASM_EMIT("6:")
            __ASM_EMIT("adds        %[count], %[count], #3")
            __ASM_EMIT("b.lt        8f")
            __ASM_EMIT("7:")
            __ASM_EMIT("ld1r        {v0.4s}, [%[a]]")
            __ASM_EMIT("ld1r        {v4.4s}, [%[b]]")
            __ASM_EMIT("fmul        v16.4s, v0.4s, v28.4s")
            __ASM_EMIT("fmla        v16.4s, v4.4s, v29.4s")
            __ASM_EMIT("st1         {v16.s}[0], [%[a]]")
            __ASM_EMIT("subs        %[count], %[count], #1")
            __ASM_EMIT("add         %[a], %[a], #0x04")
            __ASM_EMIT("add         %[b], %[b], #0x04")
            __ASM_EMIT("b.ge        7b")
            // End
            __ASM_EMIT("8:")

            : [a] "+r" (a), [b] "+r" (b),
              [count] "+r" (count),
              [k1] "+w" (k1), [k2] "+w" (k2)
            :
            : "cc", "memory",
              "v2", "v3",
              "v4", "v5", "v6", "v7",
              "v16", "v17", "v18", "v19",
              "v28", "v29"
        );
    }

    void mix_copy2(float *dst, const float *a, const float *b, float k1, float k2, size_t count)
    {
        ARCH_AARCH64_ASM(
            __ASM_EMIT("dup         v28.4s, %S[k1].s[0]")
            __ASM_EMIT("dup         v29.4s, %S[k2].s[0]")
            __ASM_EMIT("subs        %[count], %[count], #16")
            __ASM_EMIT("b.lt        2f")
            // x16 block
            __ASM_EMIT("1:")
            __ASM_EMIT("ldp         q0, q1, [%[a], #0x00]")
            __ASM_EMIT("ldp         q2, q3, [%[a], #0x20]")
            __ASM_EMIT("ldp         q4, q5, [%[b], #0x00]")
            __ASM_EMIT("ldp         q6, q7, [%[b], #0x20]")
            __ASM_EMIT("fmul        v16.4s, v0.4s, v28.4s")
            __ASM_EMIT("fmul        v17.4s, v1.4s, v28.4s")
            __ASM_EMIT("fmul        v18.4s, v2.4s, v28.4s")
            __ASM_EMIT("fmul        v19.4s, v3.4s, v28.4s")
            __ASM_EMIT("fmla        v16.4s, v4.4s, v29.4s")
            __ASM_EMIT("fmla        v17.4s, v5.4s, v29.4s")
            __ASM_EMIT("fmla        v18.4s, v6.4s, v29.4s")
            __ASM_EMIT("fmla        v19.4s, v7.4s, v29.4s")
            __ASM_EMIT("stp         q16, q17, [%[dst], #0x00]")
            __ASM_EMIT("stp         q18, q19, [%[dst], #0x20]")
            __ASM_EMIT("subs        %[count], %[count], #16")
            __ASM_EMIT("add         %[a], %[a], #0x40")
            __ASM_EMIT("add         %[b], %[b], #0x40")
            __ASM_EMIT("add         %[dst], %[dst], #0x40")
            __ASM_EMIT("b.hs        1b")
            // x8 block
            __ASM_EMIT("2:")
            __ASM_EMIT("adds        %[count], %[count], #8")
            __ASM_EMIT("b.lt        4f")
            __ASM_EMIT("ldp         q0, q1, [%[a], #0x00]")
            __ASM_EMIT("ldp         q4, q5, [%[b], #0x00]")
            __ASM_EMIT("fmul        v16.4s, v0.4s, v28.4s")
            __ASM_EMIT("fmul        v17.4s, v1.4s, v28.4s")
            __ASM_EMIT("fmla        v16.4s, v4.4s, v29.4s")
            __ASM_EMIT("fmla        v17.4s, v5.4s, v29.4s")
            __ASM_EMIT("stp         q16, q17, [%[dst], #0x00]")
            __ASM_EMIT("sub         %[count], %[count], #8")
            __ASM_EMIT("add         %[a], %[a], #0x20")
            __ASM_EMIT("add         %[b], %[b], #0x20")
            __ASM_EMIT("add         %[dst], %[dst], #0x20")
            // x4 block
            __ASM_EMIT("4:")
            __ASM_EMIT("adds        %[count], %[count], #4")
            __ASM_EMIT("b.lt        6f")
            __ASM_EMIT("ldr         q0, [%[a], #0x00]")
            __ASM_EMIT("ldr         q4, [%[b], #0x00]")
            __ASM_EMIT("fmul        v16.4s, v0.4s, v28.4s")
            __ASM_EMIT("fmla        v16.4s, v4.4s, v29.4s")
            __ASM_EMIT("str         q16, [%[dst], #0x00]")
            __ASM_EMIT("sub         %[count], %[count], #4")
            __ASM_EMIT("add         %[a], %[a], #0x10")
            __ASM_EMIT("add         %[b], %[b], #0x10")
            __ASM_EMIT("add         %[dst], %[dst], #0x10")
            // x1 blocks
            __ASM_EMIT("6:")
            __ASM_EMIT("adds        %[count], %[count], #3")
            __ASM_EMIT("b.lt        8f")
            __ASM_EMIT("7:")
            __ASM_EMIT("ld1r        {v0.4s}, [%[a]]")
            __ASM_EMIT("ld1r        {v4.4s}, [%[b]]")
            __ASM_EMIT("fmul        v16.4s, v0.4s, v28.4s")
            __ASM_EMIT("fmla        v16.4s, v4.4s, v29.4s")
            __ASM_EMIT("st1         {v16.s}[0], [%[dst]]")
            __ASM_EMIT("subs        %[count], %[count], #1")
            __ASM_EMIT("add         %[a], %[a], #0x04")
            __ASM_EMIT("add         %[b], %[b], #0x04")
            __ASM_EMIT("add         %[dst], %[dst], #0x04")
            __ASM_EMIT("b.ge        7b")
            // End
            __ASM_EMIT("8:")

            : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b),
              [count] "+r" (count),
              [k1] "+w" (k1), [k2] "+w" (k2)
            :
            : "cc", "memory",
              "v2", "v3",
              "v4", "v5", "v6", "v7",
              "v16", "v17", "v18", "v19",
              "v28", "v29"
        );
    }

    void mix_add2(float *dst, const float *a, const float *b, float k1, float k2, size_t count)
    {
        ARCH_AARCH64_ASM(
            __ASM_EMIT("dup         v28.4s, %S[k1].s[0]")
            __ASM_EMIT("dup         v29.4s, %S[k2].s[0]")
            __ASM_EMIT("subs        %[count], %[count], #16")
            __ASM_EMIT("b.lt        2f")
            // x16 block
            __ASM_EMIT("1:")
            __ASM_EMIT("ldp         q16, q17, [%[dst], #0x00]")
            __ASM_EMIT("ldp         q18, q19, [%[dst], #0x20]")
            __ASM_EMIT("ldp         q0, q1, [%[a], #0x00]")
            __ASM_EMIT("ldp         q2, q3, [%[a], #0x20]")
            __ASM_EMIT("ldp         q4, q5, [%[b], #0x00]")
            __ASM_EMIT("ldp         q6, q7, [%[b], #0x20]")
            __ASM_EMIT("fmla        v16.4s, v0.4s, v28.4s")
            __ASM_EMIT("fmla        v17.4s, v1.4s, v28.4s")
            __ASM_EMIT("fmla        v18.4s, v2.4s, v28.4s")
            __ASM_EMIT("fmla        v19.4s, v3.4s, v28.4s")
            __ASM_EMIT("fmla        v16.4s, v4.4s, v29.4s")
            __ASM_EMIT("fmla        v17.4s, v5.4s, v29.4s")
            __ASM_EMIT("fmla        v18.4s, v6.4s, v29.4s")
            __ASM_EMIT("fmla        v19.4s, v7.4s, v29.4s")
            __ASM_EMIT("stp         q16, q17, [%[dst], #0x00]")
            __ASM_EMIT("stp         q18, q19, [%[dst], #0x20]")
            __ASM_EMIT("subs        %[count], %[count], #16")
            __ASM_EMIT("add         %[a], %[a], #0x40")
            __ASM_EMIT("add         %[b], %[b], #0x40")
            __ASM_EMIT("add         %[dst], %[dst], #0x40")
            __ASM_EMIT("b.hs        1b")
            // x8 block
            __ASM_EMIT("2:")
            __ASM_EMIT("adds        %[count], %[count], #8")
            __ASM_EMIT("b.lt        4f")
            __ASM_EMIT("ldp         q16, q17, [%[dst], #0x00]")
            __ASM_EMIT("ldp         q0, q1, [%[a], #0x00]")
            __ASM_EMIT("ldp         q4, q5, [%[b], #0x00]")
            __ASM_EMIT("fmla        v16.4s, v0.4s, v28.4s")
            __ASM_EMIT("fmla        v17.4s, v1.4s, v28.4s")
            __ASM_EMIT("fmla        v16.4s, v4.4s, v29.4s")
            __ASM_EMIT("fmla        v17.4s, v5.4s, v29.4s")
            __ASM_EMIT("stp         q16, q17, [%[dst], #0x00]")
            __ASM_EMIT("sub         %[count], %[count], #8")
            __ASM_EMIT("add         %[a], %[a], #0x20")
            __ASM_EMIT("add         %[b], %[b], #0x20")
            __ASM_EMIT("add         %[dst], %[dst], #0x20")
            // x4 block
            __ASM_EMIT("4:")
            __ASM_EMIT("adds        %[count], %[count], #4")
            __ASM_EMIT("b.lt        6f")
            __ASM_EMIT("ldr         q16, [%[dst], #0x00]")
            __ASM_EMIT("ldr         q0, [%[a], #0x00]")
            __ASM_EMIT("ldr         q4, [%[b], #0x00]")
            __ASM_EMIT("fmla        v16.4s, v0.4s, v28.4s")
            __ASM_EMIT("fmla        v16.4s, v4.4s, v29.4s")
            __ASM_EMIT("str         q16, [%[dst], #0x00]")
            __ASM_EMIT("sub         %[count], %[count], #4")
            __ASM_EMIT("add         %[a], %[a], #0x10")
            __ASM_EMIT("add         %[b], %[b], #0x10")
            __ASM_EMIT("add         %[dst], %[dst], #0x10")
            // x1 blocks
            __ASM_EMIT("6:")
            __ASM_EMIT("adds        %[count], %[count], #3")
            __ASM_EMIT("b.lt        8f")
            __ASM_EMIT("7:")
            __ASM_EMIT("ld1r        {v16.4s}, [%[dst]]")
            __ASM_EMIT("ld1r        {v0.4s}, [%[a]]")
            __ASM_EMIT("ld1r        {v4.4s}, [%[b]]")
            __ASM_EMIT("fmla        v16.4s, v0.4s, v28.4s")
            __ASM_EMIT("fmla        v16.4s, v4.4s, v29.4s")
            __ASM_EMIT("st1         {v16.s}[0], [%[dst]]")
            __ASM_EMIT("subs        %[count], %[count], #1")
            __ASM_EMIT("add         %[a], %[a], #0x04")
            __ASM_EMIT("add         %[b], %[b], #0x04")
            __ASM_EMIT("add         %[dst], %[dst], #0x04")
            __ASM_EMIT("b.ge        7b")
            // End
            __ASM_EMIT("8:")

            : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b),
              [count] "+r" (count),
              [k1] "+w" (k1), [k2] "+w" (k2)
            :
            : "cc", "memory",
              "v2", "v3",
              "v4", "v5", "v6", "v7",
              "v16", "v17", "v18", "v19",
              "v28", "v29"
        );
    }

    void mix3(float *a, const float *b, const float *c, float k1, float k2, float k3, size_t count)
    {
        ARCH_AARCH64_ASM(
            __ASM_EMIT("dup         v28.4s, %S[k1].s[0]")
            __ASM_EMIT("dup         v29.4s, %S[k2].s[0]")
            __ASM_EMIT("dup         v30.4s, %S[k3].s[0]")
            __ASM_EMIT("subs        %[count], %[count], #16")
            __ASM_EMIT("b.lt        2f")
            // x16 block
            __ASM_EMIT("1:")
            __ASM_EMIT("ldp         q0, q1, [%[a], #0x00]")
            __ASM_EMIT("ldp         q2, q3, [%[a], #0x20]")
            __ASM_EMIT("ldp         q4, q5, [%[b], #0x00]")
            __ASM_EMIT("ldp         q6, q7, [%[b], #0x20]")
            __ASM_EMIT("ldp         q8, q9, [%[c], #0x00]")
            __ASM_EMIT("ldp         q10, q11, [%[c], #0x20]")
            __ASM_EMIT("fmul        v16.4s, v0.4s, v28.4s")
            __ASM_EMIT("fmul        v17.4s, v1.4s, v28.4s")
            __ASM_EMIT("fmul        v18.4s, v2.4s, v28.4s")
            __ASM_EMIT("fmul        v19.4s, v3.4s, v28.4s")
            __ASM_EMIT("fmla        v16.4s, v4.4s, v29.4s")
            __ASM_EMIT("fmla        v17.4s, v5.4s, v29.4s")
            __ASM_EMIT("fmla        v18.4s, v6.4s, v29.4s")
            __ASM_EMIT("fmla        v19.4s, v7.4s, v29.4s")
            __ASM_EMIT("fmla        v16.4s, v8.4s, v30.4s")
            __ASM_EMIT("fmla        v17.4s, v9.4s, v30.4s")
            __ASM_EMIT("fmla        v18.4s, v10.4s, v30.4s")
            __ASM_EMIT("fmla        v19.4s, v11.4s, v30.4s")
            __ASM_EMIT("stp         q16, q17, [%[a], #0x00]")
            __ASM_EMIT("stp         q18, q19, [%[a], #0x20]")
            __ASM_EMIT("subs        %[count], %[count], #16")
            __ASM_EMIT("add         %[a], %[a], #0x40")
            __ASM_EMIT("add         %[b], %[b], #0x40")
            __ASM_EMIT("add         %[c], %[c], #0x40")
            __ASM_EMIT("b.hs        1b")
            // x8 block
            __ASM_EMIT("2:")
            __ASM_EMIT("adds        %[count], %[count], #8")
            __ASM_EMIT("b.lt        4f")
            __ASM_EMIT("ldp         q0, q1, [%[a], #0x00]")
            __ASM_EMIT("ldp         q4, q5, [%[b], #0x00]")
            __ASM_EMIT("ldp         q8, q9, [%[c], #0x00]")
            __ASM_EMIT("fmul        v16.4s, v0.4s, v28.4s")
            __ASM_EMIT("fmul        v17.4s, v1.4s, v28.4s")
            __ASM_EMIT("fmla        v16.4s, v4.4s, v29.4s")
            __ASM_EMIT("fmla        v17.4s, v5.4s, v29.4s")
            __ASM_EMIT("fmla        v16.4s, v8.4s, v30.4s")
            __ASM_EMIT("fmla        v17.4s, v9.4s, v30.4s")
            __ASM_EMIT("stp         q16, q17, [%[a], #0x00]")
            __ASM_EMIT("sub         %[count], %[count], #8")
            __ASM_EMIT("add         %[a], %[a], #0x20")
            __ASM_EMIT("add         %[b], %[b], #0x20")
            __ASM_EMIT("add         %[c], %[c], #0x20")
            // x4 block
            __ASM_EMIT("4:")
            __ASM_EMIT("adds        %[count], %[count], #4")
            __ASM_EMIT("b.lt        6f")
            __ASM_EMIT("ldr         q0, [%[a], #0x00]")
            __ASM_EMIT("ldr         q4, [%[b], #0x00]")
            __ASM_EMIT("ldr         q8, [%[c], #0x00]")
            __ASM_EMIT("fmul        v16.4s, v0.4s, v28.4s")
            __ASM_EMIT("fmla        v16.4s, v4.4s, v29.4s")
            __ASM_EMIT("fmla        v16.4s, v8.4s, v30.4s")
            __ASM_EMIT("str         q16, [%[a], #0x00]")
            __ASM_EMIT("sub         %[count], %[count], #4")
            __ASM_EMIT("add         %[a], %[a], #0x10")
            __ASM_EMIT("add         %[b], %[b], #0x10")
            __ASM_EMIT("add         %[c], %[c], #0x10")
            // x1 blocks
            __ASM_EMIT("6:")
            __ASM_EMIT("adds        %[count], %[count], #3")
            __ASM_EMIT("b.lt        8f")
            __ASM_EMIT("7:")
            __ASM_EMIT("ld1r        {v0.4s}, [%[a]]")
            __ASM_EMIT("ld1r        {v4.4s}, [%[b]]")
            __ASM_EMIT("ld1r        {v8.4s}, [%[c]]")
            __ASM_EMIT("fmul        v16.4s, v0.4s, v28.4s")
            __ASM_EMIT("fmla        v16.4s, v4.4s, v29.4s")
            __ASM_EMIT("fmla        v16.4s, v8.4s, v30.4s")
            __ASM_EMIT("st1         {v16.s}[0], [%[a]]")
            __ASM_EMIT("subs        %[count], %[count], #1")
            __ASM_EMIT("add         %[a], %[a], #0x04")
            __ASM_EMIT("add         %[b], %[b], #0x04")
            __ASM_EMIT("add         %[c], %[c], #0x04")
            __ASM_EMIT("b.ge        7b")
            // End
            __ASM_EMIT("8:")

            : [a] "+r" (a), [b] "+r" (b), [c] "+r" (c),
              [count] "+r" (count),
              [k1] "+w" (k1), [k2] "+w" (k2), [k3] "+w" (k3)
            :
            : "cc", "memory",
              "v3",
              "v4", "v5", "v6", "v7",
              "v8", "v9", "v10", "v11",
              "v16", "v17", "v18", "v19",
              "v28", "v29", "v30"
        );
    }

    void mix_copy3(float *dst, const float *a, const float *b, const float *c, float k1, float k2, float k3, size_t count)
    {
        ARCH_AARCH64_ASM(
            __ASM_EMIT("dup         v28.4s, %S[k1].s[0]")
            __ASM_EMIT("dup         v29.4s, %S[k2].s[0]")
            __ASM_EMIT("dup         v30.4s, %S[k3].s[0]")
            __ASM_EMIT("subs        %[count], %[count], #16")
            __ASM_EMIT("b.lt        2f")
            // x16 block
            __ASM_EMIT("1:")
            __ASM_EMIT("ldp         q0, q1, [%[a], #0x00]")
            __ASM_EMIT("ldp         q2, q3, [%[a], #0x20]")
            __ASM_EMIT("ldp         q4, q5, [%[b], #0x00]")
            __ASM_EMIT("ldp         q6, q7, [%[b], #0x20]")
            __ASM_EMIT("ldp         q8, q9, [%[c], #0x00]")
            __ASM_EMIT("ldp         q10, q11, [%[c], #0x20]")
            __ASM_EMIT("fmul        v16.4s, v0.4s, v28.4s")
            __ASM_EMIT("fmul        v17.4s, v1.4s, v28.4s")
            __ASM_EMIT("fmul        v18.4s, v2.4s, v28.4s")
            __ASM_EMIT("fmul        v19.4s, v3.4s, v28.4s")
            __ASM_EMIT("fmla        v16.4s, v4.4s, v29.4s")
            __ASM_EMIT("fmla        v17.4s, v5.4s, v29.4s")
            __ASM_EMIT("fmla        v18.4s, v6.4s, v29.4s")
            __ASM_EMIT("fmla        v19.4s, v7.4s, v29.4s")
            __ASM_EMIT("fmla        v16.4s, v8.4s, v30.4s")
            __ASM_EMIT("fmla        v17.4s, v9.4s, v30.4s")
            __ASM_EMIT("fmla        v18.4s, v10.4s, v30.4s")
            __ASM_EMIT("fmla        v19.4s, v11.4s, v30.4s")
            __ASM_EMIT("stp         q16, q17, [%[dst], #0x00]")
            __ASM_EMIT("stp         q18, q19, [%[dst], #0x20]")
            __ASM_EMIT("subs        %[count], %[count], #16")
            __ASM_EMIT("add         %[a], %[a], #0x40")
            __ASM_EMIT("add         %[b], %[b], #0x40")
            __ASM_EMIT("add         %[c], %[c], #0x40")
            __ASM_EMIT("add         %[dst], %[dst], #0x40")
            __ASM_EMIT("b.hs        1b")
            // x8 block
            __ASM_EMIT("2:")
            __ASM_EMIT("adds        %[count], %[count], #8")
            __ASM_EMIT("b.lt        4f")
            __ASM_EMIT("ldp         q0, q1, [%[a], #0x00]")
            __ASM_EMIT("ldp         q4, q5, [%[b], #0x00]")
            __ASM_EMIT("ldp         q8, q9, [%[c], #0x00]")
            __ASM_EMIT("fmul        v16.4s, v0.4s, v28.4s")
            __ASM_EMIT("fmul        v17.4s, v1.4s, v28.4s")
            __ASM_EMIT("fmla        v16.4s, v4.4s, v29.4s")
            __ASM_EMIT("fmla        v17.4s, v5.4s, v29.4s")
            __ASM_EMIT("fmla        v16.4s, v8.4s, v30.4s")
            __ASM_EMIT("fmla        v17.4s, v9.4s, v30.4s")
            __ASM_EMIT("stp         q16, q17, [%[dst], #0x00]")
            __ASM_EMIT("sub         %[count], %[count], #8")
            __ASM_EMIT("add         %[a], %[a], #0x20")
            __ASM_EMIT("add         %[b], %[b], #0x20")
            __ASM_EMIT("add         %[c], %[c], #0x20")
            __ASM_EMIT("add         %[dst], %[dst], #0x20")
            // x4 block
            __ASM_EMIT("4:")
            __ASM_EMIT("adds        %[count], %[count], #4")
            __ASM_EMIT("b.lt        6f")
            __ASM_EMIT("ldr         q0, [%[a], #0x00]")
            __ASM_EMIT("ldr         q4, [%[b], #0x00]")
            __ASM_EMIT("ldr         q8, [%[c], #0x00]")
            __ASM_EMIT("fmul        v16.4s, v0.4s, v28.4s")
            __ASM_EMIT("fmla        v16.4s, v4.4s, v29.4s")
            __ASM_EMIT("fmla        v16.4s, v8.4s, v30.4s")
            __ASM_EMIT("str         q16, [%[dst], #0x00]")
            __ASM_EMIT("sub         %[count], %[count], #4")
            __ASM_EMIT("add         %[a], %[a], #0x10")
            __ASM_EMIT("add         %[b], %[b], #0x10")
            __ASM_EMIT("add         %[c], %[c], #0x10")
            __ASM_EMIT("add         %[dst], %[dst], #0x10")
            // x1 blocks
            __ASM_EMIT("6:")
            __ASM_EMIT("adds        %[count], %[count], #3")
            __ASM_EMIT("b.lt        8f")
            __ASM_EMIT("7:")
            __ASM_EMIT("ld1r        {v0.4s}, [%[a]]")
            __ASM_EMIT("ld1r        {v4.4s}, [%[b]]")
            __ASM_EMIT("ld1r        {v8.4s}, [%[c]]")
            __ASM_EMIT("fmul        v16.4s, v0.4s, v28.4s")
            __ASM_EMIT("fmla        v16.4s, v4.4s, v29.4s")
            __ASM_EMIT("fmla        v16.4s, v8.4s, v30.4s")
            __ASM_EMIT("st1         {v16.s}[0], [%[dst]]")
            __ASM_EMIT("subs        %[count], %[count], #1")
            __ASM_EMIT("add         %[a], %[a], #0x04")
            __ASM_EMIT("add         %[b], %[b], #0x04")
            __ASM_EMIT("add         %[c], %[c], #0x04")
            __ASM_EMIT("add         %[dst], %[dst], #0x04")
            __ASM_EMIT("b.ge        7b")
            // End
            __ASM_EMIT("8:")

            : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b), [c] "+r" (c),
              [count] "+r" (count),
              [k1] "+w" (k1), [k2] "+w" (k2), [k3] "+w" (k3)
            :
            : "cc", "memory",
              "v3",
              "v4", "v5", "v6", "v7",
              "v8", "v9", "v10", "v11",
              "v16", "v17", "v18", "v19",
              "v28", "v29", "v30"
        );
    }

    void mix_add3(float *dst, const float *a, const float *b, const float *c, float k1, float k2, float k3, size_t count)
    {
        ARCH_AARCH64_ASM(
            __ASM_EMIT("dup         v28.4s, %S[k1].s[0]")
            __ASM_EMIT("dup         v29.4s, %S[k2].s[0]")
            __ASM_EMIT("dup         v30.4s, %S[k3].s[0]")
            __ASM_EMIT("subs        %[count], %[count], #16")
            __ASM_EMIT("b.lt        2f")
            // x16 block
            __ASM_EMIT("1:")
            __ASM_EMIT("ldp         q16, q17, [%[dst], #0x00]")
            __ASM_EMIT("ldp         q18, q19, [%[dst], #0x20]")
            __ASM_EMIT("ldp         q0, q1, [%[a], #0x00]")
            __ASM_EMIT("ldp         q2, q3, [%[a], #0x20]")
            __ASM_EMIT("ldp         q4, q5, [%[b], #0x00]")
            __ASM_EMIT("ldp         q6, q7, [%[b], #0x20]")
            __ASM_EMIT("ldp         q8, q9, [%[c], #0x00]")
            __ASM_EMIT("ldp         q10, q11, [%[c], #0x20]")
            __ASM_EMIT("fmla        v16.4s, v0.4s, v28.4s")
            __ASM_EMIT("fmla        v17.4s, v1.4s, v28.4s")
            __ASM_EMIT("fmla        v18.4s, v2.4s, v28.4s")
            __ASM_EMIT("fmla        v19.4s, v3.4s, v28.4s")
            __ASM_EMIT("fmla        v16.4s, v4.4s, v29.4s")
            __ASM_EMIT("fmla        v17.4s, v5.4s, v29.4s")
            __ASM_EMIT("fmla        v18.4s, v6.4s, v29.4s")
            __ASM_EMIT("fmla        v19.4s, v7.4s, v29.4s")
            __ASM_EMIT("fmla        v16.4s, v8.4s, v30.4s")
            __ASM_EMIT("fmla        v17.4s, v9.4s, v30.4s")
            __ASM_EMIT("fmla        v18.4s, v10.4s, v30.4s")
            __ASM_EMIT("fmla        v19.4s, v11.4s, v30.4s")
            __ASM_EMIT("stp         q16, q17, [%[dst], #0x00]")
            __ASM_EMIT("stp         q18, q19, [%[dst], #0x20]")
            __ASM_EMIT("subs        %[count], %[count], #16")
            __ASM_EMIT("add         %[a], %[a], #0x40")
            __ASM_EMIT("add         %[b], %[b], #0x40")
            __ASM_EMIT("add         %[c], %[c], #0x40")
            __ASM_EMIT("add         %[dst], %[dst], #0x40")
            __ASM_EMIT("b.hs        1b")
            // x8 block
            __ASM_EMIT("2:")
            __ASM_EMIT("adds        %[count], %[count], #8")
            __ASM_EMIT("b.lt        4f")
            __ASM_EMIT("ldp         q16, q17, [%[dst], #0x00]")
            __ASM_EMIT("ldp         q0, q1, [%[a], #0x00]")
            __ASM_EMIT("ldp         q4, q5, [%[b], #0x00]")
            __ASM_EMIT("ldp         q8, q9, [%[c], #0x00]")
            __ASM_EMIT("fmla        v16.4s, v0.4s, v28.4s")
            __ASM_EMIT("fmla        v17.4s, v1.4s, v28.4s")
            __ASM_EMIT("fmla        v16.4s, v4.4s, v29.4s")
            __ASM_EMIT("fmla        v17.4s, v5.4s, v29.4s")
            __ASM_EMIT("fmla        v16.4s, v8.4s, v30.4s")
            __ASM_EMIT("fmla        v17.4s, v9.4s, v30.4s")
            __ASM_EMIT("stp         q16, q17, [%[dst], #0x00]")
            __ASM_EMIT("sub         %[count], %[count], #8")
            __ASM_EMIT("add         %[a], %[a], #0x20")
            __ASM_EMIT("add         %[b], %[b], #0x20")
            __ASM_EMIT("add         %[c], %[c], #0x20")
            __ASM_EMIT("add         %[dst], %[dst], #0x20")
            // x4 block
            __ASM_EMIT("4:")
            __ASM_EMIT("adds        %[count], %[count], #4")
            __ASM_EMIT("b.lt        6f")
            __ASM_EMIT("ldr         q16, [%[dst], #0x00]")
            __ASM_EMIT("ldr         q0, [%[a], #0x00]")
            __ASM_EMIT("ldr         q4, [%[b], #0x00]")
            __ASM_EMIT("ldr         q8, [%[c], #0x00]")
            __ASM_EMIT("fmla        v16.4s, v0.4s, v28.4s")
            __ASM_EMIT("fmla        v16.4s, v4.4s, v29.4s")
            __ASM_EMIT("fmla        v16.4s, v8.4s, v30.4s")
            __ASM_EMIT("str         q16, [%[dst], #0x00]")
            __ASM_EMIT("sub         %[count], %[count], #4")
            __ASM_EMIT("add         %[a], %[a], #0x10")
            __ASM_EMIT("add         %[b], %[b], #0x10")
            __ASM_EMIT("add         %[c], %[c], #0x10")
            __ASM_EMIT("add         %[dst], %[dst], #0x10")
            // x1 blocks
            __ASM_EMIT("6:")
            __ASM_EMIT("adds        %[count], %[count], #3")
            __ASM_EMIT("b.lt        8f")
            __ASM_EMIT("7:")
            __ASM_EMIT("ld1r        {v16.4s}, [%[dst]]")
            __ASM_EMIT("ld1r        {v0.4s}, [%[a]]")
            __ASM_EMIT("ld1r        {v4.4s}, [%[b]]")
            __ASM_EMIT("ld1r        {v8.4s}, [%[c]]")
            __ASM_EMIT("fmla        v16.4s, v0.4s, v28.4s")
            __ASM_EMIT("fmla        v16.4s, v4.4s, v29.4s")
            __ASM_EMIT("fmla        v16.4s, v8.4s, v30.4s")
            __ASM_EMIT("st1         {v16.s}[0], [%[dst]]")
            __ASM_EMIT("subs        %[count], %[count], #1")
            __ASM_EMIT("add         %[a], %[a], #0x04")
            __ASM_EMIT("add         %[b], %[b], #0x04")
            __ASM_EMIT("add         %[c], %[c], #0x04")
            __ASM_EMIT("add         %[dst], %[dst], #0x04")
            __ASM_EMIT("b.ge        7b")
            // End
            __ASM_EMIT("8:")

            : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b), [c] "+r" (c),
              [count] "+r" (count),
              [k1] "+w" (k1), [k2] "+w" (k2), [k3] "+w" (k3)
            :
            : "cc", "memory",
              "v3",
              "v4", "v5", "v6", "v7",
              "v8", "v9", "v10", "v11",
              "v16", "v17", "v18", "v19",
              "v28", "v29", "v30"
        );
    }

    void mix4(float *a, const float *b, const float *c, const float *d,
            float k1, float k2, float k3, float k4, size_t count)
    {
        ARCH_AARCH64_ASM(
            __ASM_EMIT("dup         v28.4s, %S[k1].s[0]")
            __ASM_EMIT("dup         v29.4s, %S[k2].s[0]")
            __ASM_EMIT("dup         v30.4s, %S[k3].s[0]")
            __ASM_EMIT("dup         v31.4s, %S[k4].s[0]")
            __ASM_EMIT("subs        %[count], %[count], #16")
            __ASM_EMIT("b.lt        2f")
            // x16 block
            __ASM_EMIT("1:")
            __ASM_EMIT("ldp         q0, q1, [%[a], #0x00]")
            __ASM_EMIT("ldp         q2, q3, [%[a], #0x20]")
            __ASM_EMIT("ldp         q4, q5, [%[b], #0x00]")
            __ASM_EMIT("ldp         q6, q7, [%[b], #0x20]")
            __ASM_EMIT("ldp         q8, q9, [%[c], #0x00]")
            __ASM_EMIT("ldp         q10, q11, [%[c], #0x20]")
            __ASM_EMIT("ldp         q12, q13, [%[d], #0x00]")
            __ASM_EMIT("ldp         q14, q15, [%[d], #0x20]")
            __ASM_EMIT("fmul        v16.4s, v0.4s, v28.4s")
            __ASM_EMIT("fmul        v17.4s, v1.4s, v28.4s")
            __ASM_EMIT("fmul        v18.4s, v2.4s, v28.4s")
            __ASM_EMIT("fmul        v19.4s, v3.4s, v28.4s")
            __ASM_EMIT("fmla        v16.4s, v4.4s, v29.4s")
            __ASM_EMIT("fmla        v17.4s, v5.4s, v29.4s")
            __ASM_EMIT("fmla        v18.4s, v6.4s, v29.4s")
            __ASM_EMIT("fmla        v19.4s, v7.4s, v29.4s")
            __ASM_EMIT("fmla        v16.4s, v8.4s, v30.4s")
            __ASM_EMIT("fmla        v17.4s, v9.4s, v30.4s")
            __ASM_EMIT("fmla        v18.4s, v10.4s, v30.4s")
            __ASM_EMIT("fmla        v19.4s, v11.4s, v30.4s")
            __ASM_EMIT("fmla        v16.4s, v12.4s, v31.4s")
            __ASM_EMIT("fmla        v17.4s, v13.4s, v31.4s")
            __ASM_EMIT("fmla        v18.4s, v14.4s, v31.4s")
            __ASM_EMIT("fmla        v19.4s, v15.4s, v31.4s")
            __ASM_EMIT("stp         q16, q17, [%[a], #0x00]")
            __ASM_EMIT("stp         q18, q19, [%[a], #0x20]")
            __ASM_EMIT("subs        %[count], %[count], #16")
            __ASM_EMIT("add         %[a], %[a], #0x40")
            __ASM_EMIT("add         %[b], %[b], #0x40")
            __ASM_EMIT("add         %[c], %[c], #0x40")
            __ASM_EMIT("add         %[d], %[d], #0x40")
            __ASM_EMIT("b.hs        1b")
            // x8 block
            __ASM_EMIT("2:")
            __ASM_EMIT("adds        %[count], %[count], #8")
            __ASM_EMIT("b.lt        4f")
            __ASM_EMIT("ldp         q0, q1, [%[a], #0x00]")
            __ASM_EMIT("ldp         q4, q5, [%[b], #0x00]")
            __ASM_EMIT("ldp         q8, q9, [%[c], #0x00]")
            __ASM_EMIT("ldp         q12, q13, [%[d], #0x00]")
            __ASM_EMIT("fmul        v16.4s, v0.4s, v28.4s")
            __ASM_EMIT("fmul        v17.4s, v1.4s, v28.4s")
            __ASM_EMIT("fmla        v16.4s, v4.4s, v29.4s")
            __ASM_EMIT("fmla        v17.4s, v5.4s, v29.4s")
            __ASM_EMIT("fmla        v16.4s, v8.4s, v30.4s")
            __ASM_EMIT("fmla        v17.4s, v9.4s, v30.4s")
            __ASM_EMIT("fmla        v16.4s, v12.4s, v31.4s")
            __ASM_EMIT("fmla        v17.4s, v13.4s, v31.4s")
            __ASM_EMIT("stp         q16, q17, [%[a], #0x00]")
            __ASM_EMIT("sub         %[count], %[count], #8")
            __ASM_EMIT("add         %[a], %[a], #0x20")
            __ASM_EMIT("add         %[b], %[b], #0x20")
            __ASM_EMIT("add         %[c], %[c], #0x20")
            __ASM_EMIT("add         %[d], %[d], #0x20")
            // x4 block
            __ASM_EMIT("4:")
            __ASM_EMIT("adds        %[count], %[count], #4")
            __ASM_EMIT("b.lt        6f")
            __ASM_EMIT("ldr         q0, [%[a], #0x00]")
            __ASM_EMIT("ldr         q4, [%[b], #0x00]")
            __ASM_EMIT("ldr         q8, [%[c], #0x00]")
            __ASM_EMIT("ldr         q12, [%[d], #0x00]")
            __ASM_EMIT("fmul        v16.4s, v0.4s, v28.4s")
            __ASM_EMIT("fmla        v16.4s, v4.4s, v29.4s")
            __ASM_EMIT("fmla        v16.4s, v8.4s, v30.4s")
            __ASM_EMIT("fmla        v16.4s, v12.4s, v31.4s")
            __ASM_EMIT("str         q16, [%[a], #0x00]")
            __ASM_EMIT("sub         %[count], %[count], #4")
            __ASM_EMIT("add         %[a], %[a], #0x10")
            __ASM_EMIT("add         %[b], %[b], #0x10")
            __ASM_EMIT("add         %[c], %[c], #0x10")
            __ASM_EMIT("add         %[d], %[d], #0x10")
            // x1 blocks
            __ASM_EMIT("6:")
            __ASM_EMIT("adds        %[count], %[count], #3")
            __ASM_EMIT("b.lt        8f")
            __ASM_EMIT("7:")
            __ASM_EMIT("ld1r        {v0.4s}, [%[a]]")
            __ASM_EMIT("ld1r        {v4.4s}, [%[b]]")
            __ASM_EMIT("ld1r        {v8.4s}, [%[c]]")
            __ASM_EMIT("ld1r        {v12.4s}, [%[d]]")
            __ASM_EMIT("fmul        v16.4s, v0.4s, v28.4s")
            __ASM_EMIT("fmla        v16.4s, v4.4s, v29.4s")
            __ASM_EMIT("fmla        v16.4s, v8.4s, v30.4s")
            __ASM_EMIT("fmla        v16.4s, v12.4s, v31.4s")
            __ASM_EMIT("st1         {v16.s}[0], [%[a]]")
            __ASM_EMIT("subs        %[count], %[count], #1")
            __ASM_EMIT("add         %[a], %[a], #0x04")
            __ASM_EMIT("add         %[b], %[b], #0x04")
            __ASM_EMIT("add         %[c], %[c], #0x04")
            __ASM_EMIT("add         %[d], %[d], #0x04")
            __ASM_EMIT("b.ge        7b")
            // End
            __ASM_EMIT("8:")

            : [a] "+r" (a), [b] "+r" (b), [c] "+r" (c), [d] "+r" (d),
              [count] "+r" (count),
              [k1] "+w" (k1), [k2] "+w" (k2), [k3] "+w" (k3), [k4] "+w" (k4)
            :
            : "cc", "memory",
              "v4", "v5", "v6", "v7",
              "v8", "v9", "v10", "v11",
              "v12", "v13", "v14", "v15",
              "v16", "v17", "v18", "v19",
              "v28", "v29", "v30", "v31"
        );
    }

    void mix_copy4(float *dst, const float *a, const float *b, const float *c, const float *d,
            float k1, float k2, float k3, float k4, size_t count)
    {
        ARCH_AARCH64_ASM(
            __ASM_EMIT("dup         v28.4s, %S[k1].s[0]")
            __ASM_EMIT("dup         v29.4s, %S[k2].s[0]")
            __ASM_EMIT("dup         v30.4s, %S[k3].s[0]")
            __ASM_EMIT("dup         v31.4s, %S[k4].s[0]")
            __ASM_EMIT("subs        %[count], %[count], #16")
            __ASM_EMIT("b.lt        2f")
            // x16 block
            __ASM_EMIT("1:")
            __ASM_EMIT("ldp         q0, q1, [%[a], #0x00]")
            __ASM_EMIT("ldp         q2, q3, [%[a], #0x20]")
            __ASM_EMIT("ldp         q4, q5, [%[b], #0x00]")
            __ASM_EMIT("ldp         q6, q7, [%[b], #0x20]")
            __ASM_EMIT("ldp         q8, q9, [%[c], #0x00]")
            __ASM_EMIT("ldp         q10, q11, [%[c], #0x20]")
            __ASM_EMIT("ldp         q12, q13, [%[d], #0x00]")
            __ASM_EMIT("ldp         q14, q15, [%[d], #0x20]")
            __ASM_EMIT("fmul        v16.4s, v0.4s, v28.4s")
            __ASM_EMIT("fmul        v17.4s, v1.4s, v28.4s")
            __ASM_EMIT("fmul        v18.4s, v2.4s, v28.4s")
            __ASM_EMIT("fmul        v19.4s, v3.4s, v28.4s")
            __ASM_EMIT("fmla        v16.4s, v4.4s, v29.4s")
            __ASM_EMIT("fmla        v17.4s, v5.4s, v29.4s")
            __ASM_EMIT("fmla        v18.4s, v6.4s, v29.4s")
            __ASM_EMIT("fmla        v19.4s, v7.4s, v29.4s")
            __ASM_EMIT("fmla        v16.4s, v8.4s, v30.4s")
            __ASM_EMIT("fmla        v17.4s, v9.4s, v30.4s")
            __ASM_EMIT("fmla        v18.4s, v10.4s, v30.4s")
            __ASM_EMIT("fmla        v19.4s, v11.4s, v30.4s")
            __ASM_EMIT("fmla        v16.4s, v12.4s, v31.4s")
            __ASM_EMIT("fmla        v17.4s, v13.4s, v31.4s")
            __ASM_EMIT("fmla        v18.4s, v14.4s, v31.4s")
            __ASM_EMIT("fmla        v19.4s, v15.4s, v31.4s")
            __ASM_EMIT("stp         q16, q17, [%[dst], #0x00]")
            __ASM_EMIT("stp         q18, q19, [%[dst], #0x20]")
            __ASM_EMIT("subs        %[count], %[count], #16")
            __ASM_EMIT("add         %[a], %[a], #0x40")
            __ASM_EMIT("add         %[b], %[b], #0x40")
            __ASM_EMIT("add         %[c], %[c], #0x40")
            __ASM_EMIT("add         %[d], %[d], #0x40")
            __ASM_EMIT("add         %[dst], %[dst], #0x40")
            __ASM_EMIT("b.hs        1b")
            // x8 block
            __ASM_EMIT("2:")
            __ASM_EMIT("adds        %[count], %[count], #8")
            __ASM_EMIT("b.lt        4f")
            __ASM_EMIT("ldp         q0, q1, [%[a], #0x00]")
            __ASM_EMIT("ldp         q4, q5, [%[b], #0x00]")
            __ASM_EMIT("ldp         q8, q9, [%[c], #0x00]")
            __ASM_EMIT("ldp         q12, q13, [%[d], #0x00]")
            __ASM_EMIT("fmul        v16.4s, v0.4s, v28.4s")
            __ASM_EMIT("fmul        v17.4s, v1.4s, v28.4s")
            __ASM_EMIT("fmla        v16.4s, v4.4s, v29.4s")
            __ASM_EMIT("fmla        v17.4s, v5.4s, v29.4s")
            __ASM_EMIT("fmla        v16.4s, v8.4s, v30.4s")
            __ASM_EMIT("fmla        v17.4s, v9.4s, v30.4s")
            __ASM_EMIT("fmla        v16.4s, v12.4s, v31.4s")
            __ASM_EMIT("fmla        v17.4s, v13.4s, v31.4s")
            __ASM_EMIT("stp         q16, q17, [%[dst], #0x00]")
            __ASM_EMIT("sub         %[count], %[count], #8")
            __ASM_EMIT("add         %[a], %[a], #0x20")
            __ASM_EMIT("add         %[b], %[b], #0x20")
            __ASM_EMIT("add         %[c], %[c], #0x20")
            __ASM_EMIT("add         %[d], %[d], #0x20")
            __ASM_EMIT("add         %[dst], %[dst], #0x20")
            // x4 block
            __ASM_EMIT("4:")
            __ASM_EMIT("adds        %[count], %[count], #4")
            __ASM_EMIT("b.lt        6f")
            __ASM_EMIT("ldr         q0, [%[a], #0x00]")
            __ASM_EMIT("ldr         q4, [%[b], #0x00]")
            __ASM_EMIT("ldr         q8, [%[c], #0x00]")
            __ASM_EMIT("ldr         q12, [%[d], #0x00]")
            __ASM_EMIT("fmul        v16.4s, v0.4s, v28.4s")
            __ASM_EMIT("fmla        v16.4s, v4.4s, v29.4s")
            __ASM_EMIT("fmla        v16.4s, v8.4s, v30.4s")
            __ASM_EMIT("fmla        v16.4s, v12.4s, v31.4s")
            __ASM_EMIT("str         q16, [%[dst], #0x00]")
            __ASM_EMIT("sub         %[count], %[count], #4")
            __ASM_EMIT("add         %[a], %[a], #0x10")
            __ASM_EMIT("add         %[b], %[b], #0x10")
            __ASM_EMIT("add         %[c], %[c], #0x10")
            __ASM_EMIT("add         %[d], %[d], #0x10")
            __ASM_EMIT("add         %[dst], %[dst], #0x10")
            // x1 blocks
            __ASM_EMIT("6:")
            __ASM_EMIT("adds        %[count], %[count], #3")
            __ASM_EMIT("b.lt        8f")
            __ASM_EMIT("7:")
            __ASM_EMIT("ld1r        {v0.4s}, [%[a]]")
            __ASM_EMIT("ld1r        {v4.4s}, [%[b]]")
            __ASM_EMIT("ld1r        {v8.4s}, [%[c]]")
            __ASM_EMIT("ld1r        {v12.4s}, [%[d]]")
            __ASM_EMIT("fmul        v16.4s, v0.4s, v28.4s")
            __ASM_EMIT("fmla        v16.4s, v4.4s, v29.4s")
            __ASM_EMIT("fmla        v16.4s, v8.4s, v30.4s")
            __ASM_EMIT("fmla        v16.4s, v12.4s, v31.4s")
            __ASM_EMIT("st1         {v16.s}[0], [%[dst]]")
            __ASM_EMIT("subs        %[count], %[count], #1")
            __ASM_EMIT("add         %[a], %[a], #0x04")
            __ASM_EMIT("add         %[b], %[b], #0x04")
            __ASM_EMIT("add         %[c], %[c], #0x04")
            __ASM_EMIT("add         %[d], %[d], #0x04")
            __ASM_EMIT("add         %[dst], %[dst], #0x04")
            __ASM_EMIT("b.ge        7b")
            // End
            __ASM_EMIT("8:")

            : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b), [c] "+r" (c), [d] "+r" (d),
              [count] "+r" (count),
              [k1] "+w" (k1), [k2] "+w" (k2), [k3] "+w" (k3), [k4] "+w" (k4)
            :
            : "cc", "memory",
              "v4", "v5", "v6", "v7",
              "v8", "v9", "v10", "v11",
              "v12", "v13", "v14", "v15",
              "v16", "v17", "v18", "v19",
              "v28", "v29", "v30", "v31"
        );
    }

    void mix_add4(float *dst, const float *a, const float *b, const float *c, const float *d,
            float k1, float k2, float k3, float k4, size_t count)
    {
        ARCH_AARCH64_ASM(
            __ASM_EMIT("dup         v28.4s, %S[k1].s[0]")
            __ASM_EMIT("dup         v29.4s, %S[k2].s[0]")
            __ASM_EMIT("dup         v30.4s, %S[k3].s[0]")
            __ASM_EMIT("dup         v31.4s, %S[k4].s[0]")
            __ASM_EMIT("subs        %[count], %[count], #16")
            __ASM_EMIT("b.lt        2f")
            // x16 block
            __ASM_EMIT("1:")
            __ASM_EMIT("ldp         q16, q17, [%[dst], #0x00]")
            __ASM_EMIT("ldp         q18, q19, [%[dst], #0x20]")
            __ASM_EMIT("ldp         q0, q1, [%[a], #0x00]")
            __ASM_EMIT("ldp         q2, q3, [%[a], #0x20]")
            __ASM_EMIT("ldp         q4, q5, [%[b], #0x00]")
            __ASM_EMIT("ldp         q6, q7, [%[b], #0x20]")
            __ASM_EMIT("ldp         q8, q9, [%[c], #0x00]")
            __ASM_EMIT("ldp         q10, q11, [%[c], #0x20]")
            __ASM_EMIT("ldp         q12, q13, [%[d], #0x00]")
            __ASM_EMIT("ldp         q14, q15, [%[d], #0x20]")
            __ASM_EMIT("fmla        v16.4s, v0.4s, v28.4s")
            __ASM_EMIT("fmla        v17.4s, v1.4s, v28.4s")
            __ASM_EMIT("fmla        v18.4s, v2.4s, v28.4s")
            __ASM_EMIT("fmla        v19.4s, v3.4s, v28.4s")
            __ASM_EMIT("fmla        v16.4s, v4.4s, v29.4s")
            __ASM_EMIT("fmla        v17.4s, v5.4s, v29.4s")
            __ASM_EMIT("fmla        v18.4s, v6.4s, v29.4s")
            __ASM_EMIT("fmla        v19.4s, v7.4s, v29.4s")
            __ASM_EMIT("fmla        v16.4s, v8.4s, v30.4s")
            __ASM_EMIT("fmla        v17.4s, v9.4s, v30.4s")
            __ASM_EMIT("fmla        v18.4s, v10.4s, v30.4s")
            __ASM_EMIT("fmla        v19.4s, v11.4s, v30.4s")
            __ASM_EMIT("fmla        v16.4s, v12.4s, v31.4s")
            __ASM_EMIT("fmla        v17.4s, v13.4s, v31.4s")
            __ASM_EMIT("fmla        v18.4s, v14.4s, v31.4s")
            __ASM_EMIT("fmla        v19.4s, v15.4s, v31.4s")
            __ASM_EMIT("stp         q16, q17, [%[dst], #0x00]")
            __ASM_EMIT("stp         q18, q19, [%[dst], #0x20]")
            __ASM_EMIT("subs        %[count], %[count], #16")
            __ASM_EMIT("add         %[a], %[a], #0x40")
            __ASM_EMIT("add         %[b], %[b], #0x40")
            __ASM_EMIT("add         %[c], %[c], #0x40")
            __ASM_EMIT("add         %[d], %[d], #0x40")
            __ASM_EMIT("add         %[dst], %[dst], #0x40")
            __ASM_EMIT("b.hs        1b")
            // x8 block
            __ASM_EMIT("2:")
            __ASM_EMIT("adds        %[count], %[count], #8")
            __ASM_EMIT("b.lt        4f")
            __ASM_EMIT("ldp         q16, q17, [%[dst], #0x00]")
            __ASM_EMIT("ldp         q0, q1, [%[a], #0x00]")
            __ASM_EMIT("ldp         q4, q5, [%[b], #0x00]")
            __ASM_EMIT("ldp         q8, q9, [%[c], #0x00]")
            __ASM_EMIT("ldp         q12, q13, [%[d], #0x00]")
            __ASM_EMIT("fmla        v16.4s, v0.4s, v28.4s")
            __ASM_EMIT("fmla        v17.4s, v1.4s, v28.4s")
            __ASM_EMIT("fmla        v16.4s, v4.4s, v29.4s")
            __ASM_EMIT("fmla        v17.4s, v5.4s, v29.4s")
            __ASM_EMIT("fmla        v16.4s, v8.4s, v30.4s")
            __ASM_EMIT("fmla        v17.4s, v9.4s, v30.4s")
            __ASM_EMIT("fmla        v16.4s, v12.4s, v31.4s")
            __ASM_EMIT("fmla        v17.4s, v13.4s, v31.4s")
            __ASM_EMIT("stp         q16, q17, [%[dst], #0x00]")
            __ASM_EMIT("sub         %[count], %[count], #8")
            __ASM_EMIT("add         %[a], %[a], #0x20")
            __ASM_EMIT("add         %[b], %[b], #0x20")
            __ASM_EMIT("add         %[c], %[c], #0x20")
            __ASM_EMIT("add         %[d], %[d], #0x20")
            __ASM_EMIT("add         %[dst], %[dst], #0x20")
            // x4 block
            __ASM_EMIT("4:")
            __ASM_EMIT("adds        %[count], %[count], #4")
            __ASM_EMIT("b.lt        6f")
            __ASM_EMIT("ldr         q16, [%[dst], #0x00]")
            __ASM_EMIT("ldr         q0, [%[a], #0x00]")
            __ASM_EMIT("ldr         q4, [%[b], #0x00]")
            __ASM_EMIT("ldr         q8, [%[c], #0x00]")
            __ASM_EMIT("ldr         q12, [%[d], #0x00]")
            __ASM_EMIT("fmla        v16.4s, v0.4s, v28.4s")
            __ASM_EMIT("fmla        v16.4s, v4.4s, v29.4s")
            __ASM_EMIT("fmla        v16.4s, v8.4s, v30.4s")
            __ASM_EMIT("fmla        v16.4s, v12.4s, v31.4s")
            __ASM_EMIT("str         q16, [%[dst], #0x00]")
            __ASM_EMIT("sub         %[count], %[count], #4")
            __ASM_EMIT("add         %[a], %[a], #0x10")
            __ASM_EMIT("add         %[b], %[b], #0x10")
            __ASM_EMIT("add         %[c], %[c], #0x10")
            __ASM_EMIT("add         %[d], %[d], #0x10")
            __ASM_EMIT("add         %[dst], %[dst], #0x10")
            // x1 blocks
            __ASM_EMIT("6:")
            __ASM_EMIT("adds        %[count], %[count], #3")
            __ASM_EMIT("b.lt        8f")
            __ASM_EMIT("7:")
            __ASM_EMIT("ld1r        {v16.4s}, [%[dst]]")
            __ASM_EMIT("ld1r        {v0.4s}, [%[a]]")
            __ASM_EMIT("ld1r        {v4.4s}, [%[b]]")
            __ASM_EMIT("ld1r        {v8.4s}, [%[c]]")
            __ASM_EMIT("ld1r        {v12.4s}, [%[d]]")
            __ASM_EMIT("fmla        v16.4s, v0.4s, v28.4s")
            __ASM_EMIT("fmla        v16.4s, v4.4s, v29.4s")
            __ASM_EMIT("fmla        v16.4s, v8.4s, v30.4s")
            __ASM_EMIT("fmla        v16.4s, v12.4s, v31.4s")
            __ASM_EMIT("st1         {v16.s}[0], [%[dst]]")
            __ASM_EMIT("subs        %[count], %[count], #1")
            __ASM_EMIT("add         %[a], %[a], #0x04")
            __ASM_EMIT("add         %[b], %[b], #0x04")
            __ASM_EMIT("add         %[c], %[c], #0x04")
            __ASM_EMIT("add         %[d], %[d], #0x04")
            __ASM_EMIT("add         %[dst], %[dst], #0x04")
            __ASM_EMIT("b.ge        7b")
            // End
            __ASM_EMIT("8:")

            : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b), [c] "+r" (c), [d] "+r" (d),
              [count] "+r" (count),
              [k1] "+w" (k1), [k2] "+w" (k2), [k3] "+w" (k3), [k4] "+w" (k4)
            :
            : "cc", "memory",
              "v4", "v5", "v6", "v7",
              "v8", "v9", "v10", "v11",
              "v12", "v13", "v14", "v15",
              "v16", "v17", "v18", "v19",
              "v28", "v29", "v30", "v31"
        );
    }
}

#endif /* DSP_ARCH_AARCH64_ASIMD_MIX_H_ */
