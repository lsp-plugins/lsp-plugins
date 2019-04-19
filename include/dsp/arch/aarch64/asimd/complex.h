/*
 * complex.h
 *
 *  Created on: 19 апр. 2019 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_AARCH64_ASIMD_COMPLEX_H_
#define DSP_ARCH_AARCH64_ASIMD_COMPLEX_H_

#ifndef DSP_ARCH_AARCH64_ASIMD_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_AARCH64_ASIMD_IMPL */

namespace asimd
{
    void complex_mul2(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t count)
    {
        ARCH_AARCH64_ASM
        (
            __ASM_EMIT("subs        %[count], #16")
            __ASM_EMIT("b.lo        2f")

            // x16 blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("ld1         {q0.4s, q1.4s, q2.4s, q3.4s}, [%[src_re]], #0x40")  // q0-q3   = sr
            __ASM_EMIT("ld1         {q4.4s, q5.4s, q6.4s, q7.4s}, [%[src_im]], #0x40")  // q4-q7   = si
            __ASM_EMIT("ld1         {q8.4s, q9.4s, q10.4s, q11.4s}, [%[dst_re]]")       // q8-q11  = dr
            __ASM_EMIT("ld1         {q12.4s, q13.4s, q14.4s, q15.4s}, [%[dst_im]]")     // q12-q15 = di
            __ASM_EMIT("fmul        q16.4s, q0.4s, q8.4s")              // q16 = sr*dr
            __ASM_EMIT("fmul        q17.4s, q1.4s, q9.4s")
            __ASM_EMIT("fmul        q18.4s, q2.4s, q10.4s")
            __ASM_EMIT("fmul        q19.4s, q3.4s, q11.4s")
            __ASM_EMIT("fmul        q4.4s, q8.4s, q4.4s")               // q4 = si*dr
            __ASM_EMIT("fmul        q5.4s, q9.4s, q5.4s")
            __ASM_EMIT("fmul        q6.4s, q10.4s, q6.4s")
            __ASM_EMIT("fmul        q7.4s, q11.4s, q7.4s")
            __ASM_EMIT("fmls        q16.4s, q16.4s, q4.4s, q12.4s")     // q16 = sr*dr - si*di
            __ASM_EMIT("fmls        q17.4s, q17.4s, q5.4s, q13.4s")
            __ASM_EMIT("fmls        q18.4s, q18.4s, q6.4s, q14.4s")
            __ASM_EMIT("fmls        q19.4s, q19.4s, q7.4s, q15.4s")
            __ASM_EMIT("fmla        q4.4s, q4.4s, q0.4s, q12.4s")       // q4 = si*dr + sr*di
            __ASM_EMIT("fmla        q5.4s, q5.4s, q1.4s, q13.4s")
            __ASM_EMIT("fmla        q6.4s, q6.4s, q2.4s, q14.4s")
            __ASM_EMIT("fmla        q7.4s, q7.4s, q3.4s, q15.4s")
            __ASM_EMIT("subs        %[count], #16")
            __ASM_EMIT("st1         {q16.4s, q17.4s, q18.4s, q19.4s}, %[dst_re], #0x40")
            __ASM_EMIT("st1         {q4.4s, q5.4s, q6.4s, q7.4s}, %[dst_im], #0x40")
            __ASM_EMIT("b.hs        1b")

            // x8 blocks
            __ASM_EMIT("2:")
            __ASM_EMIT("adds        %[count], #8")
            __ASM_EMIT("b.lt        4f")
            __ASM_EMIT("ld1         {q0.4s, q1.4s}, [%[src_re]], #0x20")// q0-q1   = sr
            __ASM_EMIT("ld1         {q4.4s, q5.4s}, [%[src_im]], #0x20")// q4-q5   = si
            __ASM_EMIT("ld1         {q8.4s, q9.4s}, [%[dst_re]]")       // q8-q9   = dr
            __ASM_EMIT("ld1         {q12.4s, q13.4s}, [%[dst_im]]")     // q12-q13 = di
            __ASM_EMIT("fmul        q16.4s, q0.4s, q8.4s")              // q16 = sr*dr
            __ASM_EMIT("fmul        q17.4s, q1.4s, q9.4s")
            __ASM_EMIT("fmul        q4.4s, q8.4s, q4.4s")               // q4 = si*dr
            __ASM_EMIT("fmul        q5.4s, q9.4s, q5.4s")
            __ASM_EMIT("fmls        q16.4s, q16.4s, q4.4s, q12.4s")     // q16 = sr*dr - si*di
            __ASM_EMIT("fmls        q17.4s, q17.4s, q5.4s, q13.4s")
            __ASM_EMIT("fmla        q4.4s, q4.4s, q0.4s, q12.4s")       // q4 = si*dr + sr*di
            __ASM_EMIT("fmla        q5.4s, q5.4s, q1.4s, q13.4s")
            __ASM_EMIT("sub         %[count], #8")
            __ASM_EMIT("st1         {q16.4s, q17.4s}, %[dst_re], #0x20")
            __ASM_EMIT("st1         {q4.4s, q5.4s}, %[dst_im], #0x20")

            // x4 blocks
            __ASM_EMIT("4:")
            __ASM_EMIT("adds        %[count], #4")
            __ASM_EMIT("b.lt        6f")
            __ASM_EMIT("ld1         {q0.4s}, [%[src_re]], #0x10")   // q0-q1   = sr
            __ASM_EMIT("ld1         {q4.4s}, [%[src_im]], #0x10")   // q4-q5   = si
            __ASM_EMIT("ld1         {q8.4s}, [%[dst_re]]")          // q8-q9   = dr
            __ASM_EMIT("ld1         {q12.4s}, [%[dst_im]]")         // q12-q13 = di
            __ASM_EMIT("fmul        q16.4s, q0.4s, q4.4s")          // q16 = sr*dr
            __ASM_EMIT("fmul        q4.4s, q2.4s, q4.4s")           // q4 = si*dr
            __ASM_EMIT("fmls        q16.4s, q16.4s, q2.4s, q6.4s")  // q16 = sr*dr - si*di
            __ASM_EMIT("fmla        q4.4s, q4.4s, q0.4s, q6.4s")    // q4 = si*dr + sr*di
            __ASM_EMIT("sub         %[count], #4")
            __ASM_EMIT("st1         {q16.4s}, %[dst_re], #0x10")
            __ASM_EMIT("st1         {q4.4s}, %[dst_im], #0x10")

            // x1 blocks
            __ASM_EMIT("6:")
            __ASM_EMIT("adds        %[count], #3")
            __ASM_EMIT("b.lt         8f")
            __ASM_EMIT("7:")
            __ASM_EMIT("ld1r        q0, %[src_re], 0x04")           // s0 = sr
            __ASM_EMIT("ld1r        q4, %[src_im], 0x04")           // s1 = si
            __ASM_EMIT("ld1r        q8, %[dst_re], 0x04")           // s2 = dr
            __ASM_EMIT("ld1r        q12, %[dst_im], 0x04")          // s3 = di
            __ASM_EMIT("fmul        q16.4s, q0.4s, q4.4s")          // q16 = sr*dr
            __ASM_EMIT("fmul        q4.4s, q2.4s, q4.4s")           // q4 = si*dr
            __ASM_EMIT("fmls        q16.4s, q16.4s, q2.4s, q6.4s")  // q16 = sr*dr - si*di
            __ASM_EMIT("fmla        q4.4s, q4.4s, q0.4s, q6.4s")    // q4 = si*dr + sr*di
            __ASM_EMIT("str         s16, %[dst_re]")
            __ASM_EMIT("str         s4, %[dst_im]")
            __ASM_EMIT("subs        %[count], #1")
            __ASM_EMIT("add         %[dst_re], %[dst_re], #0x04")
            __ASM_EMIT("add         %[dst_im], %[dst_im], #0x04")
            __ASM_EMIT("b.ge        7b")

            __ASM_EMIT("8:")

            : [dst_re] "+r" (dst_re), [dst_im] "+r" (dst_im),
              [src_re] "+r" (src_re), [src_im] "+r" (src_im),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15",
              "q16", "q17", "q18", "q19"
        );
    }
}

#endif /* DSP_ARCH_AARCH64_ASIMD_COMPLEX_H_ */
