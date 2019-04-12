/*
 * copy.h
 *
 *  Created on: 11 апр. 2019 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_AARCH64_ASIMD_COPY_H_
#define DSP_ARCH_AARCH64_ASIMD_COPY_H_

#ifndef DSP_ARCH_AARCH64_ASIMD_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_AARCH64_ASIMD_IMPL */

namespace asimd
{
    void copy(float *dst, const float *src, size_t count)
    {
        float *dst2;
        const float *src2;

        ARCH_AARCH64_ASM
        (
            __ASM_EMIT("cmp         %[dst1], %[src1]")
            __ASM_EMIT("b.eq        16f")
            __ASM_EMIT("subs        %[count], %[count], #0x10")
            __ASM_EMIT("b.lt        8f")
            __ASM_EMIT("add         %[dst2], %[dst1], #0x20")
            __ASM_EMIT("add         %[src2], %[src1], #0x20")

            /* 96x block */
            __ASM_EMIT("subs        %[count], %[count], #0x50") // 0x60 - 0x10
            __ASM_EMIT("b.lt        2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("ldp         q0, q1, [%[src1]], #0x40")
            __ASM_EMIT("ldp         q2, q3, [%[src2]], #0x40")
            __ASM_EMIT("ldp         q4, q5, [%[src1]], #0x40")
            __ASM_EMIT("ldp         q6, q7, [%[src2]], #0x40")
            __ASM_EMIT("ldp         q16, q17, [%[src1]], #0x40")
            __ASM_EMIT("ldp         q18, q19, [%[src2]], #0x40")
            __ASM_EMIT("ldp         q20, q21, [%[src1]], #0x40")
            __ASM_EMIT("ldp         q22, q23, [%[src2]], #0x40")
            __ASM_EMIT("ldp         q24, q25, [%[src1]], #0x40")
            __ASM_EMIT("ldp         q26, q27, [%[src2]], #0x40")
            __ASM_EMIT("ldp         q28, q29, [%[src1]], #0x40")
            __ASM_EMIT("ldp         q30, q31, [%[src2]], #0x40")

            __ASM_EMIT("subs        %[count], %[count], #0x60")

            __ASM_EMIT("stp         q0, q1, [%[dst1]], #0x40")
            __ASM_EMIT("stp         q2, q3, [%[dst2]], #0x40")
            __ASM_EMIT("stp         q4, q5, [%[dst1]], #0x40")
            __ASM_EMIT("stp         q6, q7, [%[dst2]], #0x40")
            __ASM_EMIT("stp         q16, q17, [%[dst1]], #0x40")
            __ASM_EMIT("stp         q18, q19, [%[dst2]], #0x40")
            __ASM_EMIT("stp         q20, q21, [%[dst1]], #0x40")
            __ASM_EMIT("stp         q22, q23, [%[dst2]], #0x40")
            __ASM_EMIT("stp         q24, q25, [%[dst1]], #0x40")
            __ASM_EMIT("stp         q26, q27, [%[dst2]], #0x40")
            __ASM_EMIT("stp         q28, q29, [%[dst1]], #0x40")
            __ASM_EMIT("stp         q30, q31, [%[dst2]], #0x40")

            __ASM_EMIT("b.hs         1b")

            /* 64x block */
            __ASM_EMIT("2:")
            __ASM_EMIT("adds        %[count], %[count], #0x20") // + 0x60 - 0x40
            __ASM_EMIT("b.lt        4f")

            __ASM_EMIT("ldp         q0, q1, [%[src1]], #0x40")
            __ASM_EMIT("ldp         q2, q3, [%[src2]], #0x40")
            __ASM_EMIT("ldp         q4, q5, [%[src1]], #0x40")
            __ASM_EMIT("ldp         q6, q7, [%[src2]], #0x40")
            __ASM_EMIT("ldp         q16, q17, [%[src1]], #0x40")
            __ASM_EMIT("ldp         q18, q19, [%[src2]], #0x40")
            __ASM_EMIT("ldp         q20, q21, [%[src1]], #0x40")
            __ASM_EMIT("ldp         q22, q23, [%[src2]], #0x40")

            __ASM_EMIT("sub         %[count], %[count], #0x40")

            __ASM_EMIT("stp         q0, q1, [%[dst1]], #0x40")
            __ASM_EMIT("stp         q2, q3, [%[dst2]], #0x40")
            __ASM_EMIT("stp         q4, q5, [%[dst1]], #0x40")
            __ASM_EMIT("stp         q6, q7, [%[dst2]], #0x40")
            __ASM_EMIT("stp         q16, q17, [%[dst1]], #0x40")
            __ASM_EMIT("stp         q18, q19, [%[dst2]], #0x40")
            __ASM_EMIT("stp         q20, q21, [%[dst1]], #0x40")
            __ASM_EMIT("stp         q22, q23, [%[dst2]], #0x40")

            /* 32x block */
            __ASM_EMIT("4:")
            __ASM_EMIT("adds        %[count], %[count], #0x20") // + 0x40 - 0x20
            __ASM_EMIT("b.lt        6f")

            __ASM_EMIT("ldp         q0, q1, [%[src1]], #0x40")
            __ASM_EMIT("ldp         q2, q3, [%[src2]], #0x40")
            __ASM_EMIT("ldp         q4, q5, [%[src1]], #0x40")
            __ASM_EMIT("ldp         q6, q7, [%[src2]], #0x40")

            __ASM_EMIT("sub         %[count], %[count], #0x20")

            __ASM_EMIT("stp         q0, q1, [%[dst1]], #0x40")
            __ASM_EMIT("stp         q2, q3, [%[dst2]], #0x40")
            __ASM_EMIT("stp         q4, q5, [%[dst1]], #0x40")
            __ASM_EMIT("stp         q6, q7, [%[dst2]], #0x40")

            /* 16x block */
            __ASM_EMIT("6:")
            __ASM_EMIT("adds        %[count], %[count], #0x10") // + 0x20 - 0x10
            __ASM_EMIT("b.lt        8f")

            __ASM_EMIT("ldp         q0, q1, [%[src1]], #0x40")
            __ASM_EMIT("ldp         q2, q3, [%[src2]], #0x40")

            __ASM_EMIT("sub         %[count], %[count], #0x10")

            __ASM_EMIT("stp         q0, q1, [%[dst1]], #0x40")
            __ASM_EMIT("stp         q2, q3, [%[dst2]], #0x40")

            /* 8x block */
            __ASM_EMIT("8:")
            __ASM_EMIT("adds        %[count], %[count], #0x08") // + 0x10 - 0x08
            __ASM_EMIT("b.lt        10f")
            __ASM_EMIT("ldp         q0, q1, [%[src1]], #0x20")
            __ASM_EMIT("sub         %[count], %[count], #0x08")
            __ASM_EMIT("stp         q0, q1, [%[dst1]], #0x20")

            /* 4x block */
            __ASM_EMIT("10:")
            __ASM_EMIT("adds        %[count], %[count], #0x04") // + 0x08 - 0x04
            __ASM_EMIT("b.lt        12f")
            __ASM_EMIT("ldr         q0, [%[src1]], #0x10")
            __ASM_EMIT("sub         %[count], %[count], #0x04")
            __ASM_EMIT("str         q0, [%[dst1]], #0x10")

            /* 2x block */
            __ASM_EMIT("12:")
            __ASM_EMIT("adds        %[count], %[count], #0x02") // + 0x04 - 0x02
            __ASM_EMIT("b.lt        14f")
            __ASM_EMIT("ldr         d0, [%[src1]], #0x08")
            __ASM_EMIT("sub         %[count], %[count], #0x02")
            __ASM_EMIT("str         d0, [%[dst1]], #0x08")

            /* 1x block */
            __ASM_EMIT("14:")
            __ASM_EMIT("adds        %[count], %[count], #0x01") // + 0x02 - 0x01
            __ASM_EMIT("b.lt        16f")
            __ASM_EMIT("ldr         s0, [%[src1]]")
            __ASM_EMIT("str         s0, [%[dst1]]")

            /* End of copy */
            __ASM_EMIT("16:")

            : [src1] "+r" (src), [src2] "=&r" (src2),
              [dst1] "+r"(dst), [dst2] "=&r"(dst2),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              //"q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15", // These should be avoided if possible
              "q16", "q17", "q18", "q19" , "q20", "q21", "q22", "q23",
              "q24", "q25", "q26", "q27" , "q28", "q29", "q30", "q31"
        );
    }

    void move(float *dst, const float *src, size_t count)
    {
        float *dst2;
        const float *src2;

        ARCH_AARCH64_ASM
        (
            __ASM_EMIT("cmp         %[dst1], %[src1]")
            __ASM_EMIT("b.eq        2000f")
            __ASM_EMIT("b.gt        1000f")

            //---------------------------------------------
            // Address-increasing copy
            __ASM_EMIT("subs        %[count], %[count], #0x10")
            __ASM_EMIT("b.lt        8f")
            __ASM_EMIT("add         %[dst2], %[dst1], #0x20")
            __ASM_EMIT("add         %[src2], %[src1], #0x20")

            /* 96x block */
            __ASM_EMIT("subs        %[count], %[count], #0x50") // 0x60 - 0x10
            __ASM_EMIT("b.lt        2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("ldp         q0, q1, [%[src1]], #0x40")
            __ASM_EMIT("ldp         q2, q3, [%[src2]], #0x40")
            __ASM_EMIT("ldp         q4, q5, [%[src1]], #0x40")
            __ASM_EMIT("ldp         q6, q7, [%[src2]], #0x40")
            __ASM_EMIT("ldp         q16, q17, [%[src1]], #0x40")
            __ASM_EMIT("ldp         q18, q19, [%[src2]], #0x40")
            __ASM_EMIT("ldp         q20, q21, [%[src1]], #0x40")
            __ASM_EMIT("ldp         q22, q23, [%[src2]], #0x40")
            __ASM_EMIT("ldp         q24, q25, [%[src1]], #0x40")
            __ASM_EMIT("ldp         q26, q27, [%[src2]], #0x40")
            __ASM_EMIT("ldp         q28, q29, [%[src1]], #0x40")
            __ASM_EMIT("ldp         q30, q31, [%[src2]], #0x40")

            __ASM_EMIT("subs        %[count], %[count], #0x60")

            __ASM_EMIT("stp         q0, q1, [%[dst1]], #0x40")
            __ASM_EMIT("stp         q2, q3, [%[dst2]], #0x40")
            __ASM_EMIT("stp         q4, q5, [%[dst1]], #0x40")
            __ASM_EMIT("stp         q6, q7, [%[dst2]], #0x40")
            __ASM_EMIT("stp         q16, q17, [%[dst1]], #0x40")
            __ASM_EMIT("stp         q18, q19, [%[dst2]], #0x40")
            __ASM_EMIT("stp         q20, q21, [%[dst1]], #0x40")
            __ASM_EMIT("stp         q22, q23, [%[dst2]], #0x40")
            __ASM_EMIT("stp         q24, q25, [%[dst1]], #0x40")
            __ASM_EMIT("stp         q26, q27, [%[dst2]], #0x40")
            __ASM_EMIT("stp         q28, q29, [%[dst1]], #0x40")
            __ASM_EMIT("stp         q30, q31, [%[dst2]], #0x40")

            __ASM_EMIT("b.hs         1b")

            /* 64x block */
            __ASM_EMIT("2:")
            __ASM_EMIT("adds        %[count], %[count], #0x20") // + 0x60 - 0x40
            __ASM_EMIT("b.lt        4f")

            __ASM_EMIT("ldp         q0, q1, [%[src1]], #0x40")
            __ASM_EMIT("ldp         q2, q3, [%[src2]], #0x40")
            __ASM_EMIT("ldp         q4, q5, [%[src1]], #0x40")
            __ASM_EMIT("ldp         q6, q7, [%[src2]], #0x40")
            __ASM_EMIT("ldp         q16, q17, [%[src1]], #0x40")
            __ASM_EMIT("ldp         q18, q19, [%[src2]], #0x40")
            __ASM_EMIT("ldp         q20, q21, [%[src1]], #0x40")
            __ASM_EMIT("ldp         q22, q23, [%[src2]], #0x40")

            __ASM_EMIT("sub         %[count], %[count], #0x40")

            __ASM_EMIT("stp         q0, q1, [%[dst1]], #0x40")
            __ASM_EMIT("stp         q2, q3, [%[dst2]], #0x40")
            __ASM_EMIT("stp         q4, q5, [%[dst1]], #0x40")
            __ASM_EMIT("stp         q6, q7, [%[dst2]], #0x40")
            __ASM_EMIT("stp         q16, q17, [%[dst1]], #0x40")
            __ASM_EMIT("stp         q18, q19, [%[dst2]], #0x40")
            __ASM_EMIT("stp         q20, q21, [%[dst1]], #0x40")
            __ASM_EMIT("stp         q22, q23, [%[dst2]], #0x40")

            /* 32x block */
            __ASM_EMIT("4:")
            __ASM_EMIT("adds        %[count], %[count], #0x20") // + 0x40 - 0x20
            __ASM_EMIT("b.lt        6f")

            __ASM_EMIT("ldp         q0, q1, [%[src1]], #0x40")
            __ASM_EMIT("ldp         q2, q3, [%[src2]], #0x40")
            __ASM_EMIT("ldp         q4, q5, [%[src1]], #0x40")
            __ASM_EMIT("ldp         q6, q7, [%[src2]], #0x40")

            __ASM_EMIT("sub         %[count], %[count], #0x20")

            __ASM_EMIT("stp         q0, q1, [%[dst1]], #0x40")
            __ASM_EMIT("stp         q2, q3, [%[dst2]], #0x40")
            __ASM_EMIT("stp         q4, q5, [%[dst1]], #0x40")
            __ASM_EMIT("stp         q6, q7, [%[dst2]], #0x40")

            /* 16x block */
            __ASM_EMIT("6:")
            __ASM_EMIT("adds        %[count], %[count], #0x10") // + 0x20 - 0x10
            __ASM_EMIT("b.lt        8f")

            __ASM_EMIT("ldp         q0, q1, [%[src1]], #0x40")
            __ASM_EMIT("ldp         q2, q3, [%[src2]], #0x40")

            __ASM_EMIT("sub         %[count], %[count], #0x10")

            __ASM_EMIT("stp         q0, q1, [%[dst1]], #0x40")
            __ASM_EMIT("stp         q2, q3, [%[dst2]], #0x40")

            /* 8x block */
            __ASM_EMIT("8:")
            __ASM_EMIT("adds        %[count], %[count], #0x08") // + 0x10 - 0x08
            __ASM_EMIT("b.lt        10f")
            __ASM_EMIT("ldp         q0, q1, [%[src1]], #0x20")
            __ASM_EMIT("sub         %[count], %[count], #0x08")
            __ASM_EMIT("stp         q0, q1, [%[dst1]], #0x20")

            /* 4x block */
            __ASM_EMIT("10:")
            __ASM_EMIT("adds        %[count], %[count], #0x04") // + 0x08 - 0x04
            __ASM_EMIT("b.lt        12f")
            __ASM_EMIT("ldr         q0, [%[src1]], #0x10")
            __ASM_EMIT("sub         %[count], %[count], #0x04")
            __ASM_EMIT("str         q0, [%[dst1]], #0x10")

            /* 2x block */
            __ASM_EMIT("12:")
            __ASM_EMIT("adds        %[count], %[count], #0x02") // + 0x04 - 0x02
            __ASM_EMIT("b.lt        14f")
            __ASM_EMIT("ldr         d0, [%[src1]], #0x08")
            __ASM_EMIT("sub         %[count], %[count], #0x02")
            __ASM_EMIT("str         d0, [%[dst1]], #0x08")

            /* 1x block */
            __ASM_EMIT("14:")
            __ASM_EMIT("adds        %[count], %[count], #0x01") // + 0x02 - 0x01
            __ASM_EMIT("b.lt        2000f")
            __ASM_EMIT("ldr         s0, [%[src1]]")
            __ASM_EMIT("str         s0, [%[dst1]]")
            __ASM_EMIT("b           2000f")

            //---------------------------------------------
            // Address-decreasing copy
            __ASM_EMIT("1000:")
            __ASM_EMIT("add         %[src1], %[src1], %[count], lsl #2")    // src1 += count
            __ASM_EMIT("add         %[dst1], %[dst1], %[count], lsl #2")    // dst1 += count
            __ASM_EMIT("subs        %[count], %[count], #0x10")
            __ASM_EMIT("sub         %[dst1], %[dst1], #0x40")
            __ASM_EMIT("sub         %[src1], %[src1], #0x40")
            __ASM_EMIT("b.lt        8f")

            __ASM_EMIT("add         %[dst2], %[dst1], #0x20")
            __ASM_EMIT("add         %[src2], %[src1], #0x20")

            /* 96x block */
            __ASM_EMIT("subs        %[count], %[count], #0x50") // 0x60 - 0x10
            __ASM_EMIT("b.lt        2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("ldp         q0, q1, [%[src1]], #-0x40")
            __ASM_EMIT("ldp         q2, q3, [%[src2]], #-0x40")
            __ASM_EMIT("ldp         q4, q5, [%[src1]], #-0x40")
            __ASM_EMIT("ldp         q6, q7, [%[src2]], #-0x40")
            __ASM_EMIT("ldp         q16, q17, [%[src1]], #-0x40")
            __ASM_EMIT("ldp         q18, q19, [%[src2]], #-0x40")
            __ASM_EMIT("ldp         q20, q21, [%[src1]], #-0x40")
            __ASM_EMIT("ldp         q22, q23, [%[src2]], #-0x40")
            __ASM_EMIT("ldp         q24, q25, [%[src1]], #-0x40")
            __ASM_EMIT("ldp         q26, q27, [%[src2]], #-0x40")
            __ASM_EMIT("ldp         q28, q29, [%[src1]], #-0x40")
            __ASM_EMIT("ldp         q30, q31, [%[src2]], #-0x40")

            __ASM_EMIT("subs        %[count], %[count], #0x60")

            __ASM_EMIT("stp         q0, q1, [%[dst1]], #-0x40")
            __ASM_EMIT("stp         q2, q3, [%[dst2]], #-0x40")
            __ASM_EMIT("stp         q4, q5, [%[dst1]], #-0x40")
            __ASM_EMIT("stp         q6, q7, [%[dst2]], #-0x40")
            __ASM_EMIT("stp         q16, q17, [%[dst1]], #-0x40")
            __ASM_EMIT("stp         q18, q19, [%[dst2]], #-0x40")
            __ASM_EMIT("stp         q20, q21, [%[dst1]], #-0x40")
            __ASM_EMIT("stp         q22, q23, [%[dst2]], #-0x40")
            __ASM_EMIT("stp         q24, q25, [%[dst1]], #-0x40")
            __ASM_EMIT("stp         q26, q27, [%[dst2]], #-0x40")
            __ASM_EMIT("stp         q28, q29, [%[dst1]], #-0x40")
            __ASM_EMIT("stp         q30, q31, [%[dst2]], #-0x40")

            __ASM_EMIT("b.hs         1b")

            /* 64x block */
            __ASM_EMIT("2:")
            __ASM_EMIT("adds        %[count], %[count], #0x20") // + 0x60 - 0x40
            __ASM_EMIT("b.lt        4f")

            __ASM_EMIT("ldp         q0, q1, [%[src1]], #0x40")
            __ASM_EMIT("ldp         q2, q3, [%[src2]], #0x40")
            __ASM_EMIT("ldp         q4, q5, [%[src1]], #0x40")
            __ASM_EMIT("ldp         q6, q7, [%[src2]], #0x40")
            __ASM_EMIT("ldp         q16, q17, [%[src1]], #0x40")
            __ASM_EMIT("ldp         q18, q19, [%[src2]], #0x40")
            __ASM_EMIT("ldp         q20, q21, [%[src1]], #0x40")
            __ASM_EMIT("ldp         q22, q23, [%[src2]], #0x40")

            __ASM_EMIT("sub         %[count], %[count], #0x40")

            __ASM_EMIT("stp         q0, q1, [%[dst1]], #0x40")
            __ASM_EMIT("stp         q2, q3, [%[dst2]], #0x40")
            __ASM_EMIT("stp         q4, q5, [%[dst1]], #0x40")
            __ASM_EMIT("stp         q6, q7, [%[dst2]], #0x40")
            __ASM_EMIT("stp         q16, q17, [%[dst1]], #0x40")
            __ASM_EMIT("stp         q18, q19, [%[dst2]], #0x40")
            __ASM_EMIT("stp         q20, q21, [%[dst1]], #0x40")
            __ASM_EMIT("stp         q22, q23, [%[dst2]], #0x40")

            /* 32x block */
            __ASM_EMIT("4:")
            __ASM_EMIT("adds        %[count], %[count], #0x20") // + 0x40 - 0x20
            __ASM_EMIT("b.lt        6f")

            __ASM_EMIT("ldp         q0, q1, [%[src1]], #0x40")
            __ASM_EMIT("ldp         q2, q3, [%[src2]], #0x40")
            __ASM_EMIT("ldp         q4, q5, [%[src1]], #0x40")
            __ASM_EMIT("ldp         q6, q7, [%[src2]], #0x40")

            __ASM_EMIT("sub         %[count], %[count], #0x20")

            __ASM_EMIT("stp         q0, q1, [%[dst1]], #0x40")
            __ASM_EMIT("stp         q2, q3, [%[dst2]], #0x40")
            __ASM_EMIT("stp         q4, q5, [%[dst1]], #0x40")
            __ASM_EMIT("stp         q6, q7, [%[dst2]], #0x40")

            /* 16x block */
            __ASM_EMIT("6:")
            __ASM_EMIT("adds        %[count], %[count], #0x10") // + 0x20 - 0x10
            __ASM_EMIT("b.lt        9f")

            __ASM_EMIT("ldp         q0, q1, [%[src1]], #0x40")
            __ASM_EMIT("ldp         q2, q3, [%[src2]], #0x40")

            __ASM_EMIT("sub         %[count], %[count], #0x10")

            __ASM_EMIT("stp         q0, q1, [%[dst1]], #0x40")
            __ASM_EMIT("stp         q2, q3, [%[dst2]], #0x40")

            /* 8x block */
            __ASM_EMIT("8:")
            __ASM_EMIT("adds        %[count], %[count], #0x08") // + 0x10 - 0x08
            __ASM_EMIT("add         %[dst1], %[dst1], #0x20")
            __ASM_EMIT("add         %[src1], %[src1], #0x20")
            __ASM_EMIT("b.lt        10f")
            __ASM_EMIT("ldp         q0, q1, [%[src1]], #-0x20")
            __ASM_EMIT("sub         %[count], %[count], #0x08")
            __ASM_EMIT("stp         q0, q1, [%[dst1]], #-0x20")

            /* 4x block */
            __ASM_EMIT("10:")
            __ASM_EMIT("adds        %[count], %[count], #0x04") // + 0x08 - 0x04
            __ASM_EMIT("add         %[dst1], %[dst1], #0x10")
            __ASM_EMIT("add         %[src1], %[src1], #0x10")
            __ASM_EMIT("b.lt        12f")
            __ASM_EMIT("ldr         q0, [%[src1]], #-0x10")
            __ASM_EMIT("sub         %[count], %[count], #0x04")
            __ASM_EMIT("str         q0, [%[dst1]], #-0x10")

            /* 2x block */
            __ASM_EMIT("12:")
            __ASM_EMIT("adds        %[count], %[count], #0x02") // + 0x04 - 0x02
            __ASM_EMIT("add         %[dst1], %[dst1], #0x08")
            __ASM_EMIT("add         %[src1], %[src1], #0x08")
            __ASM_EMIT("b.lt        14f")
            __ASM_EMIT("ldr         d0, [%[src1]], #-0x08")
            __ASM_EMIT("sub         %[count], %[count], #0x02")
            __ASM_EMIT("str         d0, [%[dst1]], #-0x08")

            /* 1x block */
            __ASM_EMIT("14:")
            __ASM_EMIT("adds        %[count], %[count], #0x01") // + 0x02 - 0x01
            __ASM_EMIT("add         %[dst1], %[dst1], #0x04")
            __ASM_EMIT("add         %[src1], %[src1], #0x04")
            __ASM_EMIT("b.lt        2000f")
            __ASM_EMIT("ldr         s0, [%[src1]]")
            __ASM_EMIT("str         s0, [%[dst1]]")

            /* End of move */
            __ASM_EMIT("2000:")

            : [src1] "+r" (src), [src2] "=&r" (src2),
              [dst1] "+r"(dst), [dst2] "=&r"(dst2),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              //"q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15", // These should be avoided if possible
              "q16", "q17", "q18", "q19" , "q20", "q21", "q22", "q23",
              "q24", "q25", "q26", "q27" , "q28", "q29", "q30", "q31"
        );
    }
}

#endif /* DSP_ARCH_AARCH64_ASIMD_COPY_H_ */
