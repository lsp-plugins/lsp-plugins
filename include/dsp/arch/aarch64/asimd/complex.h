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
            __ASM_EMIT("subs        %[count], %[count], #16")
            __ASM_EMIT("b.lo        2f")

            // x16 blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("ld1         {v0.4s-v3.4s}, [%[src_re]], #0x40") // v0-v3   = sr
            __ASM_EMIT("ld1         {v4.4s-v7.4s}, [%[src_im]], #0x40") // v4-v7   = si
            __ASM_EMIT("ld1         {v8.4s-v11.4s}, [%[dst_re]]")       // v8-v11  = dr
            __ASM_EMIT("ld1         {v12.4s-v15.4s}, [%[dst_im]]")      // v12-v15 = di
            __ASM_EMIT("fmul        v16.4s, v0.4s, v8.4s")              // v16 = sr*dr
            __ASM_EMIT("fmul        v17.4s, v1.4s, v9.4s")
            __ASM_EMIT("fmul        v18.4s, v2.4s, v10.4s")
            __ASM_EMIT("fmul        v19.4s, v3.4s, v11.4s")
            __ASM_EMIT("fmul        v8.4s, v8.4s, v4.4s")               // v8 = si*dr
            __ASM_EMIT("fmul        v9.4s, v9.4s, v5.4s")
            __ASM_EMIT("fmul        v10.4s, v10.4s, v6.4s")
            __ASM_EMIT("fmul        v11.4s, v11.4s, v7.4s")
            __ASM_EMIT("fmls        v16.4s, v4.4s, v12.4s")             // v16 = sr*dr - si*di
            __ASM_EMIT("fmls        v17.4s, v5.4s, v13.4s")
            __ASM_EMIT("fmls        v18.4s, v6.4s, v14.4s")
            __ASM_EMIT("fmls        v19.4s, v7.4s, v15.4s")
            __ASM_EMIT("fmla        v8.4s, v0.4s, v12.4s")              // v8 = si*dr + sr*di
            __ASM_EMIT("fmla        v9.4s, v1.4s, v13.4s")
            __ASM_EMIT("fmla        v10.4s, v2.4s, v14.4s")
            __ASM_EMIT("fmla        v11.4s, v3.4s, v15.4s")
            __ASM_EMIT("subs        %[count], %[count], #16")
            __ASM_EMIT("st1         {v16.4s-v19.4s}, [%[dst_re]], #0x40")
            __ASM_EMIT("st1         {v8.4s-v11.4s}, [%[dst_im]], #0x40")
            __ASM_EMIT("b.hs        1b")

            // x8 blocks
            __ASM_EMIT("2:")
            __ASM_EMIT("adds        %[count], %[count], #8")
            __ASM_EMIT("b.lt        4f")
            __ASM_EMIT("ld1         {v0.4s, v1.4s}, [%[src_re]], #0x20")// v0-v1    = sr
            __ASM_EMIT("ld1         {v4.4s, v5.4s}, [%[src_im]], #0x20")// v4-v5    = si
            __ASM_EMIT("ld1         {v8.4s, v9.4s}, [%[dst_re]]")       // v8-v9    = dr
            __ASM_EMIT("ld1         {v12.4s, v13.4s}, [%[dst_im]]")     // v12-v13  = di
            __ASM_EMIT("fmul        v16.4s, v0.4s, v8.4s")              // v16 = sr*dr
            __ASM_EMIT("fmul        v17.4s, v1.4s, v9.4s")
            __ASM_EMIT("fmul        v8.4s, v8.4s, v4.4s")               // v8 = si*dr
            __ASM_EMIT("fmul        v9.4s, v9.4s, v5.4s")
            __ASM_EMIT("fmls        v16.4s, v4.4s, v12.4s")             // v16 = sr*dr - si*di
            __ASM_EMIT("fmls        v17.4s, v5.4s, v13.4s")
            __ASM_EMIT("fmla        v8.4s, v0.4s, v12.4s")              // v8 = si*dr + sr*di
            __ASM_EMIT("fmla        v9.4s, v1.4s, v13.4s")
            __ASM_EMIT("sub         %[count], %[count], #8")
            __ASM_EMIT("st1         {v16.4s, v17.4s}, [%[dst_re]], #0x20")
            __ASM_EMIT("st1         {v8.4s, v9.4s}, [%[dst_im]], #0x20")

            // x4 blocks
            __ASM_EMIT("4:")
            __ASM_EMIT("adds        %[count], %[count], #4")
            __ASM_EMIT("b.lt        6f")
            __ASM_EMIT("ld1         {v0.4s}, [%[src_re]], #0x10")       // v0-v1    = sr
            __ASM_EMIT("ld1         {v4.4s}, [%[src_im]], #0x10")       // v4-v5    = si
            __ASM_EMIT("ld1         {v8.4s}, [%[dst_re]]")              // v8-v9    = dr
            __ASM_EMIT("ld1         {v12.4s}, [%[dst_im]]")             // v12-v13  = di
            __ASM_EMIT("fmul        v16.4s, v0.4s, v8.4s")              // v16 = sr*dr
            __ASM_EMIT("fmul        v8.4s, v8.4s, v4.4s")               // v8 = si*dr
            __ASM_EMIT("fmls        v16.4s, v4.4s, v12.4s")             // v16 = sr*dr - si*di
            __ASM_EMIT("fmla        v8.4s, v0.4s, v12.4s")              // v8 = si*dr + sr*di
            __ASM_EMIT("sub         %[count], %[count], #4")
            __ASM_EMIT("st1         {v16.4s}, [%[dst_re]], #0x10")
            __ASM_EMIT("st1         {v8.4s}, [%[dst_im]], #0x10")

            // x1 blocks
            __ASM_EMIT("6:")
            __ASM_EMIT("adds        %[count], %[count], #3")
            __ASM_EMIT("b.lt        8f")
            __ASM_EMIT("7:")
            __ASM_EMIT("ld1r        {v0.4s}, [%[src_re]], #0x04")       // v0   = sr
            __ASM_EMIT("ld1r        {v4.4s}, [%[src_im]], #0x04")       // v4   = si
            __ASM_EMIT("ld1r        {v8.4s}, [%[dst_re]]")              // v8   = dr
            __ASM_EMIT("ld1r        {v12.4s}, [%[dst_im]]")             // v12  = di
            __ASM_EMIT("fmul        v16.4s, v0.4s, v8.4s")              // v16 = sr*dr
            __ASM_EMIT("fmul        v8.4s, v8.4s, v4.4s")               // v8 = si*dr
            __ASM_EMIT("fmls        v16.4s, v4.4s, v12.4s")             // v16 = sr*dr - si*di
            __ASM_EMIT("fmla        v8.4s, v0.4s, v12.4s")              // v8 = si*dr + sr*di
            __ASM_EMIT("str         s16, [%[dst_re]]")
            __ASM_EMIT("str         s8, [%[dst_im]]")
            __ASM_EMIT("subs        %[count], %[count], #1")
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
