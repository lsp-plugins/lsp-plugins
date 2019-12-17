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
    /*
     * v0, v1   = dr
     * v2, v3   = di
     * v16, v17 = ar
     * v18, v19 = ai
     * v20, v21 = br
     * v22, v23 = bi
     */
    void complex_mul2(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t count)
    {
        ARCH_AARCH64_ASM
        (
            // x8 blocks
            __ASM_EMIT("subs        %[count], %[count], #8")
            __ASM_EMIT("b.lo        2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("ldp         q16, q17, [%[src_re]]")                 // v16-v19  = ar
            __ASM_EMIT("ldp         q18, q19, [%[src_im]]")                 // v18-v19  = ai
            __ASM_EMIT("ldp         q20, q21, [%[dst_re]]")                 // v20-v21  = br
            __ASM_EMIT("ldp         q22, q23, [%[dst_im]]")                 // v22-v23  = bi
            __ASM_EMIT("fmul        v0.4s, v16.4s, v20.4s")                 // v0   = ar*br
            __ASM_EMIT("fmul        v1.4s, v17.4s, v21.4s")
            __ASM_EMIT("add         %[src_re], %[src_re], #0x20")
            __ASM_EMIT("fmul        v2.4s, v18.4s, v20.4s")                 // v2   = ai*br
            __ASM_EMIT("fmul        v3.4s, v19.4s, v21.4s")
            __ASM_EMIT("add         %[src_im], %[src_im], #0x20")
            __ASM_EMIT("fmls        v0.4s, v18.4s, v22.4s")                 // v0   = ar*br - ai*bi
            __ASM_EMIT("fmls        v1.4s, v19.4s, v23.4s")
            __ASM_EMIT("fmla        v2.4s, v16.4s, v22.4s")                 // v2   = ai*br + ar*bi
            __ASM_EMIT("fmla        v3.4s, v17.4s, v23.4s")
            __ASM_EMIT("stp         q0, q1, [%[dst_re]]")
            __ASM_EMIT("stp         q2, q3, [%[dst_im]]")
            __ASM_EMIT("subs        %[count], %[count], #8")
            __ASM_EMIT("add         %[dst_re], %[dst_re], #0x20")
            __ASM_EMIT("add         %[dst_im], %[dst_im], #0x20")
            __ASM_EMIT("b.hs        1b")
            __ASM_EMIT("2:")
            // x4 blocks
            __ASM_EMIT("adds        %[count], %[count], #4")
            __ASM_EMIT("b.lo        4f")
            __ASM_EMIT("ldr         q16, [%[src_re]]")                      // v16  = ar
            __ASM_EMIT("ldr         q18, [%[src_im]]")                      // v18  = ai
            __ASM_EMIT("ldr         q20, [%[dst_re]]")                      // v20  = br
            __ASM_EMIT("ldr         q22, [%[dst_im]]")                      // v22  = bi
            __ASM_EMIT("fmul        v0.4s, v16.4s, v20.4s")                 // v0   = ar*br
            __ASM_EMIT("add         %[src_re], %[src_re], #0x10")
            __ASM_EMIT("fmul        v2.4s, v18.4s, v20.4s")                 // v2   = ai*br
            __ASM_EMIT("add         %[src_im], %[src_im], #0x10")
            __ASM_EMIT("fmls        v0.4s, v18.4s, v22.4s")                 // v0   = ar*br - ai*bi
            __ASM_EMIT("fmla        v2.4s, v16.4s, v22.4s")                 // v2   = ai*br + ar*bi
            __ASM_EMIT("str         q0, [%[dst_re]]")
            __ASM_EMIT("str         q2, [%[dst_im]]")
            __ASM_EMIT("sub         %[count], %[count], #4")
            __ASM_EMIT("add         %[dst_re], %[dst_re], #0x10")
            __ASM_EMIT("add         %[dst_im], %[dst_im], #0x10")
            __ASM_EMIT("4:")
            // x1 blocks
            __ASM_EMIT("adds        %[count], %[count], #3")
            __ASM_EMIT("b.lt        6f")
            __ASM_EMIT("5:")
            __ASM_EMIT("ld1r        {v16.4s}, [%[src_re]]")                 // v16  = ar
            __ASM_EMIT("ld1r        {v18.4s}, [%[src_im]]")                 // v18  = ai
            __ASM_EMIT("ld1r        {v20.4s}, [%[dst_re]]")                 // v20  = br
            __ASM_EMIT("ld1r        {v22.4s}, [%[dst_im]]")                 // v22  = bi
            __ASM_EMIT("fmul        v0.4s, v16.4s, v20.4s")                 // v0   = ar*br
            __ASM_EMIT("add         %[src_re], %[src_re], #0x04")
            __ASM_EMIT("fmul        v2.4s, v18.4s, v20.4s")                 // v2   = ai*br
            __ASM_EMIT("add         %[src_im], %[src_im], #0x04")
            __ASM_EMIT("fmls        v0.4s, v18.4s, v22.4s")                 // v0   = ar*br - ai*bi
            __ASM_EMIT("fmla        v2.4s, v16.4s, v22.4s")                 // v2   = ai*br + ar*bi
            __ASM_EMIT("st1         {v0.s}[0], [%[dst_re]]")
            __ASM_EMIT("st1         {v2.s}[0], [%[dst_im]]")
            __ASM_EMIT("subs        %[count], %[count], #1")
            __ASM_EMIT("add         %[dst_re], %[dst_re], #0x04")
            __ASM_EMIT("add         %[dst_im], %[dst_im], #0x04")
            __ASM_EMIT("b.ge        5b")
            __ASM_EMIT("6:")
            /* end */
            : [dst_re] "+r" (dst_re), [dst_im] "+r" (dst_im),
              [src_re] "+r" (src_re), [src_im] "+r" (src_im),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "v0", "v1",
              "v16", "v17", "v18", "v19",
              "v20", "v21", "v22", "v23"
        );
    }

    void complex_mul3(float *dst_re, float *dst_im, const float *src1_re, const float *src1_im, const float *src2_re, const float *src2_im, size_t count)
    {
        ARCH_AARCH64_ASM
        (
            // x16 blocks
            __ASM_EMIT("subs        %[count], %[count], #16")
            __ASM_EMIT("b.lo        2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("ld1         {v16.4s-v19.4s}, [%[src1_re]]")             // v16-v19  = sr
            __ASM_EMIT("ld1         {v20.4s-v23.4s}, [%[src1_im]]")             // v20-v23  = si
            __ASM_EMIT("ld1         {v24.4s-v27.4s}, [%[src2_re]]")             // v24-v27  = dr
            __ASM_EMIT("ld1         {v28.4s-v31.4s}, [%[src2_im]]")             // v28-v31  = di
            __ASM_EMIT("add         %[src1_re], %[src1_re], #0x40")
            __ASM_EMIT("fmul        v0.4s, v16.4s, v24.4s")                     // v0   = sr*dr
            __ASM_EMIT("fmul        v1.4s, v17.4s, v25.4s")
            __ASM_EMIT("fmul        v2.4s, v18.4s, v26.4s")
            __ASM_EMIT("fmul        v3.4s, v19.4s, v27.4s")
            __ASM_EMIT("add         %[src1_im], %[src1_im], #0x40")
            __ASM_EMIT("fmul        v24.4s, v24.4s, v20.4s")                    // v24  = si*dr
            __ASM_EMIT("fmul        v25.4s, v25.4s, v21.4s")
            __ASM_EMIT("fmul        v26.4s, v26.4s, v22.4s")
            __ASM_EMIT("fmul        v27.4s, v27.4s, v23.4s")
            __ASM_EMIT("add         %[src2_re], %[src2_re], #0x40")
            __ASM_EMIT("fmls        v0.4s, v20.4s, v28.4s")                     // v0   = sr*dr - si*di
            __ASM_EMIT("fmls        v1.4s, v21.4s, v29.4s")
            __ASM_EMIT("fmls        v2.4s, v22.4s, v30.4s")
            __ASM_EMIT("fmls        v3.4s, v23.4s, v31.4s")
            __ASM_EMIT("add         %[src2_im], %[src2_im], #0x40")
            __ASM_EMIT("fmla        v24.4s, v16.4s, v28.4s")                    // v24  = si*dr + sr*di
            __ASM_EMIT("fmla        v25.4s, v17.4s, v29.4s")
            __ASM_EMIT("fmla        v26.4s, v18.4s, v30.4s")
            __ASM_EMIT("fmla        v27.4s, v19.4s, v31.4s")
            __ASM_EMIT("st1         {v0.4s-v3.4s}, [%[dst_re]]")
            __ASM_EMIT("st1         {v24.4s-v27.4s}, [%[dst_im]]")
            __ASM_EMIT("subs        %[count], %[count], #16")
            __ASM_EMIT("add         %[dst_re], %[dst_re], #0x40")
            __ASM_EMIT("add         %[dst_im], %[dst_im], #0x40")
            __ASM_EMIT("b.hs        1b")
            __ASM_EMIT("2:")
            // x8 blocks
            __ASM_EMIT("adds        %[count], %[count], #8")
            __ASM_EMIT("b.lt        4f")
            __ASM_EMIT("ld1         {v16.4s, v17.4s}, [%[src1_re]]")            // v16-v17  = sr
            __ASM_EMIT("ld1         {v20.4s, v21.4s}, [%[src1_im]]")            // v20-v21  = si
            __ASM_EMIT("ld1         {v24.4s, v25.4s}, [%[src2_re]]")            // v24-v25  = dr
            __ASM_EMIT("ld1         {v28.4s, v29.4s}, [%[src2_im]]")            // v28-v29  = di
            __ASM_EMIT("add         %[src1_re], %[src1_re], #0x20")
            __ASM_EMIT("fmul        v0.4s, v16.4s, v24.4s")                     // v0   = sr*dr
            __ASM_EMIT("fmul        v1.4s, v17.4s, v25.4s")
            __ASM_EMIT("add         %[src1_im], %[src1_im], #0x20")
            __ASM_EMIT("fmul        v24.4s, v24.4s, v20.4s")                    // v24  = si*dr
            __ASM_EMIT("fmul        v25.4s, v25.4s, v21.4s")
            __ASM_EMIT("add         %[src2_re], %[src2_re], #0x20")
            __ASM_EMIT("fmls        v0.4s, v20.4s, v28.4s")                     // v0   = sr*dr - si*di
            __ASM_EMIT("fmls        v1.4s, v21.4s, v29.4s")
            __ASM_EMIT("add         %[src2_im], %[src2_im], #0x20")
            __ASM_EMIT("fmla        v24.4s, v16.4s, v28.4s")                    // v24  = si*dr + sr*di
            __ASM_EMIT("fmla        v25.4s, v17.4s, v29.4s")
            __ASM_EMIT("sub         %[count], %[count], #8")
            __ASM_EMIT("st1         {v0.4s, v1.4s}, [%[dst_re]]")
            __ASM_EMIT("st1         {v24.4s, v25.4s}, [%[dst_im]]")
            __ASM_EMIT("add         %[dst_re], %[dst_re], #0x20")
            __ASM_EMIT("add         %[dst_im], %[dst_im], #0x20")
            __ASM_EMIT("4:")
            // x4 blocks
            __ASM_EMIT("adds        %[count], %[count], #4")
            __ASM_EMIT("b.lt        6f")
            __ASM_EMIT("ld1         {v16.4s}, [%[src1_re]]")                    // v16-v17  = sr
            __ASM_EMIT("ld1         {v20.4s}, [%[src1_im]]")                    // v20-v21  = si
            __ASM_EMIT("ld1         {v24.4s}, [%[src2_re]]")                    // v24-v25  = dr
            __ASM_EMIT("ld1         {v28.4s}, [%[src2_im]]")                    // v28-v29  = di
            __ASM_EMIT("add         %[src1_re], %[src1_re], #0x10")
            __ASM_EMIT("fmul        v0.4s, v16.4s, v24.4s")                     // v0   = sr*dr
            __ASM_EMIT("add         %[src1_im], %[src1_im], #0x10")
            __ASM_EMIT("fmul        v24.4s, v24.4s, v20.4s")                    // v24  = si*dr
            __ASM_EMIT("add         %[src2_re], %[src2_re], #0x10")
            __ASM_EMIT("fmls        v0.4s, v20.4s, v28.4s")                     // v0   = sr*dr - si*di
            __ASM_EMIT("add         %[src2_im], %[src2_im], #0x10")
            __ASM_EMIT("fmla        v24.4s, v16.4s, v28.4s")                    // v24  = si*dr + sr*di
            __ASM_EMIT("sub         %[count], %[count], #4")
            __ASM_EMIT("st1         {v0.4s}, [%[dst_re]]")
            __ASM_EMIT("st1         {v24.4s}, [%[dst_im]]")
            __ASM_EMIT("add         %[dst_re], %[dst_re], #0x10")
            __ASM_EMIT("add         %[dst_im], %[dst_im], #0x10")
            __ASM_EMIT("6:")
            // x1 blocks
            __ASM_EMIT("adds        %[count], %[count], #3")
            __ASM_EMIT("b.lt        8f")
            __ASM_EMIT("7:")
            __ASM_EMIT("ld1r        {v16.4s}, [%[src1_re]]")                    // v16  = sr
            __ASM_EMIT("ld1r        {v20.4s}, [%[src1_im]]")                    // v20  = si
            __ASM_EMIT("ld1r        {v24.4s}, [%[src2_re]]")                    // v24  = dr
            __ASM_EMIT("ld1r        {v28.4s}, [%[src2_im]]")                    // v28  = di
            __ASM_EMIT("add         %[src1_re], %[src1_re], #0x04")
            __ASM_EMIT("fmul        v0.4s, v16.4s, v24.4s")                     // v0   = sr*dr
            __ASM_EMIT("add         %[src1_im], %[src1_im], #0x04")
            __ASM_EMIT("fmul        v24.4s, v24.4s, v20.4s")                    // v24  = si*dr
            __ASM_EMIT("add         %[src2_re], %[src2_re], #0x04")
            __ASM_EMIT("fmls        v0.4s, v20.4s, v28.4s")                     // v0   = sr*dr - si*di
            __ASM_EMIT("add         %[src2_im], %[src2_im], #0x04")
            __ASM_EMIT("fmla        v24.4s, v16.4s, v28.4s")                    // v24  = si*dr + sr*di
            __ASM_EMIT("st1         {v0.s}[0], [%[dst_re]]")
            __ASM_EMIT("st1         {v24.s}[0], [%[dst_im]]")
            __ASM_EMIT("add         %[dst_re], %[dst_re], #0x04")
            __ASM_EMIT("add         %[dst_im], %[dst_im], #0x04")
            __ASM_EMIT("subs        %[count], %[count], #1")
            __ASM_EMIT("b.ge        7b")
            __ASM_EMIT("8:")

            : [dst_re] "+r" (dst_re), [dst_im] "+r" (dst_im),
              [src1_re] "+r" (src1_re), [src1_im] "+r" (src1_im),
              [src2_re] "+r" (src2_re), [src2_im] "+r" (src2_im),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "v0", "v1", "v2", "v3",
              // "v8", "v9", "v10", "v11", "v12", "v13", "v14", "v15", // Avoid usage if possible
              "v16", "v17", "v18", "v19", "v20", "v21", "v22", "v23",
              "v24", "v25", "v26", "v27", "v28", "v29", "v30", "v31"
        );
    }

    void complex_div2(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t count)
    {
        ARCH_AARCH64_ASM
        (
            __ASM_EMIT("subs        %[count], %[count], #16")
            __ASM_EMIT("b.lo        2f")

            // x16 blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("ld1         {v16.4s-v19.4s}, [%[src_re]], #0x40")   // v16-v19  = sr
            __ASM_EMIT("ld1         {v20.4s-v23.4s}, [%[src_im]], #0x40")   // v20-v23  = si
            __ASM_EMIT("ld1         {v24.4s-v27.4s}, [%[dst_re]]")          // v24-v27  = dr
            __ASM_EMIT("ld1         {v28.4s-v31.4s}, [%[dst_im]]")          // v28-v31  = di

            __ASM_EMIT("fmul        v0.4s, v16.4s, v24.4s")                 // v0   = sr*dr
            __ASM_EMIT("fmul        v1.4s, v17.4s, v25.4s")
            __ASM_EMIT("fmul        v2.4s, v18.4s, v26.4s")
            __ASM_EMIT("fmul        v3.4s, v19.4s, v27.4s")
            __ASM_EMIT("fmul        v4.4s, v16.4s, v28.4s")                 // v4   = sr*di
            __ASM_EMIT("fmul        v5.4s, v17.4s, v29.4s")
            __ASM_EMIT("fmul        v6.4s, v18.4s, v30.4s")
            __ASM_EMIT("fmul        v7.4s, v19.4s, v31.4s")
            __ASM_EMIT("fmla        v0.4s, v20.4s, v28.4s")                 // v0   = sr*dr + si*di
            __ASM_EMIT("fmla        v1.4s, v21.4s, v29.4s")
            __ASM_EMIT("fmla        v2.4s, v22.4s, v30.4s")
            __ASM_EMIT("fmla        v3.4s, v23.4s, v31.4s")
            __ASM_EMIT("fmul        v16.4s, v16.4s, v16.4s")                // v16  = sr*sr
            __ASM_EMIT("fmul        v17.4s, v17.4s, v17.4s")
            __ASM_EMIT("fmul        v18.4s, v18.4s, v18.4s")
            __ASM_EMIT("fmul        v19.4s, v19.4s, v19.4s")
            __ASM_EMIT("fmla        v4.4s, v20.4s, v24.4s")                 // v4   = sr*di + si*dr
            __ASM_EMIT("fmla        v5.4s, v21.4s, v25.4s")
            __ASM_EMIT("fmla        v6.4s, v22.4s, v26.4s")
            __ASM_EMIT("fmla        v7.4s, v23.4s, v27.4s")
            __ASM_EMIT("fmla        v16.4s, v20.4s, v20.4s")                // v16  = sr*sr + si*si
            __ASM_EMIT("fmla        v17.4s, v21.4s, v21.4s")
            __ASM_EMIT("fmla        v18.4s, v22.4s, v22.4s")
            __ASM_EMIT("fmla        v19.4s, v23.4s, v23.4s")
            __ASM_EMIT("fneg        v4.4s, v4.4s")                          // v4   = -(sr*di + si*dr)
            __ASM_EMIT("fneg        v5.4s, v5.4s")
            __ASM_EMIT("fneg        v6.4s, v6.4s")
            __ASM_EMIT("fneg        v7.4s, v7.4s")
            __ASM_EMIT("fdiv        v0.4s, v0.4s, v16.4s")                  // v0   = (sr*dr + si*di)/(sr*sr + si*si)
            __ASM_EMIT("fdiv        v1.4s, v1.4s, v17.4s")
            __ASM_EMIT("fdiv        v2.4s, v2.4s, v18.4s")
            __ASM_EMIT("fdiv        v3.4s, v3.4s, v19.4s")
            __ASM_EMIT("fdiv        v4.4s, v4.4s, v16.4s")                  // v4   = -(sr*di + si*dr)/(sr*sr + si*si)
            __ASM_EMIT("fdiv        v5.4s, v5.4s, v17.4s")
            __ASM_EMIT("fdiv        v6.4s, v6.4s, v18.4s")
            __ASM_EMIT("fdiv        v7.4s, v7.4s, v19.4s")

            __ASM_EMIT("subs        %[count], %[count], #16")
            __ASM_EMIT("st1         {v0.4s-v3.4s}, [%[dst_re]], #0x40")
            __ASM_EMIT("st1         {v4.4s-v7.4s}, [%[dst_im]], #0x40")
            __ASM_EMIT("b.hs        1b")

            // x8 blocks
            __ASM_EMIT("2:")
            __ASM_EMIT("adds        %[count], %[count], #8")
            __ASM_EMIT("b.lt        4f")
            __ASM_EMIT("ld1         {v16.4s, v17.4s}, [%[src_re]], #0x20")  // v16-v17  = sr
            __ASM_EMIT("ld1         {v20.4s, v21.4s}, [%[src_im]], #0x20")  // v20-v21  = si
            __ASM_EMIT("ld1         {v24.4s, v25.4s}, [%[dst_re]]")         // v24-v25  = dr
            __ASM_EMIT("ld1         {v28.4s, v29.4s}, [%[dst_im]]")         // v28-v29  = di
            __ASM_EMIT("fmul        v0.4s, v16.4s, v24.4s")                 // v0   = sr*dr
            __ASM_EMIT("fmul        v1.4s, v17.4s, v25.4s")
            __ASM_EMIT("fmul        v4.4s, v16.4s, v28.4s")                 // v4   = sr*di
            __ASM_EMIT("fmul        v5.4s, v17.4s, v29.4s")
            __ASM_EMIT("fmla        v0.4s, v20.4s, v28.4s")                 // v0   = sr*dr + si*di
            __ASM_EMIT("fmla        v1.4s, v21.4s, v29.4s")
            __ASM_EMIT("fmul        v16.4s, v16.4s, v16.4s")                // v16  = sr*sr
            __ASM_EMIT("fmul        v17.4s, v17.4s, v17.4s")
            __ASM_EMIT("fmla        v4.4s, v20.4s, v24.4s")                 // v4   = sr*di + si*dr
            __ASM_EMIT("fmla        v5.4s, v21.4s, v25.4s")
            __ASM_EMIT("fmla        v16.4s, v20.4s, v20.4s")                // v16  = sr*sr + si*si
            __ASM_EMIT("fmla        v17.4s, v21.4s, v21.4s")
            __ASM_EMIT("fneg        v4.4s, v4.4s")                          // v4   = -(sr*di + si*dr)
            __ASM_EMIT("fneg        v5.4s, v5.4s")
            __ASM_EMIT("fdiv        v0.4s, v0.4s, v16.4s")                  // v0   = (sr*dr + si*di)/(sr*sr + si*si)
            __ASM_EMIT("fdiv        v1.4s, v1.4s, v17.4s")
            __ASM_EMIT("fdiv        v4.4s, v4.4s, v16.4s")                  // v4   = -(sr*di + si*dr)/(sr*sr + si*si)
            __ASM_EMIT("fdiv        v5.4s, v5.4s, v17.4s")
            __ASM_EMIT("sub         %[count], %[count], #8")
            __ASM_EMIT("st1         {v0.4s, v1.4s}, [%[dst_re]], #0x20")
            __ASM_EMIT("st1         {v4.4s, v5.4s}, [%[dst_im]], #0x20")

            // x4 blocks
            __ASM_EMIT("4:")
            __ASM_EMIT("adds        %[count], %[count], #4")
            __ASM_EMIT("b.lt        6f")
            __ASM_EMIT("ld1         {v16.4s}, [%[src_re]], #0x10")          // v16-v17  = sr
            __ASM_EMIT("ld1         {v20.4s}, [%[src_im]], #0x10")          // v20-v21  = si
            __ASM_EMIT("ld1         {v24.4s}, [%[dst_re]]")                 // v24-v25  = dr
            __ASM_EMIT("ld1         {v28.4s}, [%[dst_im]]")                 // v28-v29  = di
            __ASM_EMIT("fmul        v0.4s, v16.4s, v24.4s")                 // v0   = sr*dr
            __ASM_EMIT("fmul        v4.4s, v16.4s, v28.4s")                 // v4   = sr*di
            __ASM_EMIT("fmla        v0.4s, v20.4s, v28.4s")                 // v0   = sr*dr + si*di
            __ASM_EMIT("fmul        v16.4s, v16.4s, v16.4s")                // v16  = sr*sr
            __ASM_EMIT("fmla        v4.4s, v20.4s, v24.4s")                 // v4   = sr*di + si*dr
            __ASM_EMIT("fmla        v16.4s, v20.4s, v20.4s")                // v16  = sr*sr + si*si
            __ASM_EMIT("fneg        v4.4s, v4.4s")                          // v4   = -(sr*di + si*dr)
            __ASM_EMIT("fdiv        v0.4s, v0.4s, v16.4s")                  // v0   = (sr*dr + si*di)/(sr*sr + si*si)
            __ASM_EMIT("fdiv        v4.4s, v4.4s, v16.4s")                  // v4   = -(sr*di + si*dr)/(sr*sr + si*si)
            __ASM_EMIT("sub         %[count], %[count], #4")
            __ASM_EMIT("st1         {v0.4s}, [%[dst_re]], #0x10")
            __ASM_EMIT("st1         {v4.4s}, [%[dst_im]], #0x10")

            // x1 blocks
            __ASM_EMIT("6:")
            __ASM_EMIT("adds        %[count], %[count], #3")
            __ASM_EMIT("b.lt        8f")
            __ASM_EMIT("7:")
            __ASM_EMIT("ld1r        {v16.4s}, [%[src_re]], #0x04")          // v16  = sr
            __ASM_EMIT("ld1r        {v20.4s}, [%[src_im]], #0x04")          // v20  = si
            __ASM_EMIT("ld1r        {v24.4s}, [%[dst_re]]")                 // v24  = dr
            __ASM_EMIT("ld1r        {v28.4s}, [%[dst_im]]")                 // v28  = di
            __ASM_EMIT("fmul        v0.4s, v16.4s, v24.4s")                 // v0   = sr*dr
            __ASM_EMIT("fmul        v4.4s, v16.4s, v28.4s")                 // v4   = sr*di
            __ASM_EMIT("fmla        v0.4s, v20.4s, v28.4s")                 // v0   = sr*dr + si*di
            __ASM_EMIT("fmul        v16.4s, v16.4s, v16.4s")                // v16  = sr*sr
            __ASM_EMIT("fmla        v4.4s, v20.4s, v24.4s")                 // v4   = sr*di + si*dr
            __ASM_EMIT("fmla        v16.4s, v20.4s, v20.4s")                // v16  = sr*sr + si*si
            __ASM_EMIT("fneg        v4.4s, v4.4s")                          // v4   = -(sr*di + si*dr)
            __ASM_EMIT("fdiv        v0.4s, v0.4s, v16.4s")                  // v0   = (sr*dr + si*di)/(sr*sr + si*si)
            __ASM_EMIT("fdiv        v4.4s, v4.4s, v16.4s")                  // v4   = -(sr*di + si*dr)/(sr*sr + si*si)
            __ASM_EMIT("str         s0, [%[dst_re]], #0x04")
            __ASM_EMIT("str         s4, [%[dst_im]], #0x04")
            __ASM_EMIT("subs        %[count], %[count], #1")
            __ASM_EMIT("b.ge        7b")

            __ASM_EMIT("8:")

            : [dst_re] "+r" (dst_re), [dst_im] "+r" (dst_im),
              [src_re] "+r" (src_re), [src_im] "+r" (src_im),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7",
              // "v8", "v9", "v10", "v11", "v12", "v13", "v14", "v15", // Avoid usage if possible
              "v16", "v17", "v18", "v19", "v20", "v21", "v22", "v23",
              "v24", "v25", "v26", "v27", "v28", "v29", "v30", "v31"
        );
    }

    void complex_rdiv2(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t count)
    {
        ARCH_AARCH64_ASM
        (
            __ASM_EMIT("subs        %[count], %[count], #16")
            __ASM_EMIT("b.lo        2f")

            // x16 blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("ld1         {v24.4s-v27.4s}, [%[src_re]], #0x40")   // v24-v27  = sr
            __ASM_EMIT("ld1         {v28.4s-v31.4s}, [%[src_im]], #0x40")   // v28-v31  = si
            __ASM_EMIT("ld1         {v16.4s-v19.4s}, [%[dst_re]]")          // v16-v19  = dr
            __ASM_EMIT("ld1         {v20.4s-v23.4s}, [%[dst_im]]")          // v20-v23  = di

            __ASM_EMIT("fmul        v0.4s, v16.4s, v24.4s")                 // v0   = dr*sr
            __ASM_EMIT("fmul        v1.4s, v17.4s, v25.4s")
            __ASM_EMIT("fmul        v2.4s, v18.4s, v26.4s")
            __ASM_EMIT("fmul        v3.4s, v19.4s, v27.4s")
            __ASM_EMIT("fmul        v4.4s, v16.4s, v28.4s")                 // v4   = dr*si
            __ASM_EMIT("fmul        v5.4s, v17.4s, v29.4s")
            __ASM_EMIT("fmul        v6.4s, v18.4s, v30.4s")
            __ASM_EMIT("fmul        v7.4s, v19.4s, v31.4s")
            __ASM_EMIT("fmla        v0.4s, v20.4s, v28.4s")                 // v0   = dr*sr + di*si
            __ASM_EMIT("fmla        v1.4s, v21.4s, v29.4s")
            __ASM_EMIT("fmla        v2.4s, v22.4s, v30.4s")
            __ASM_EMIT("fmla        v3.4s, v23.4s, v31.4s")
            __ASM_EMIT("fmul        v16.4s, v16.4s, v16.4s")                // v16  = dr*dr
            __ASM_EMIT("fmul        v17.4s, v17.4s, v17.4s")
            __ASM_EMIT("fmul        v18.4s, v18.4s, v18.4s")
            __ASM_EMIT("fmul        v19.4s, v19.4s, v19.4s")
            __ASM_EMIT("fmla        v4.4s, v20.4s, v24.4s")                 // v4   = dr*si + di*sr
            __ASM_EMIT("fmla        v5.4s, v21.4s, v25.4s")
            __ASM_EMIT("fmla        v6.4s, v22.4s, v26.4s")
            __ASM_EMIT("fmla        v7.4s, v23.4s, v27.4s")
            __ASM_EMIT("fmla        v16.4s, v20.4s, v20.4s")                // v16  = dr*dr + di*di
            __ASM_EMIT("fmla        v17.4s, v21.4s, v21.4s")
            __ASM_EMIT("fmla        v18.4s, v22.4s, v22.4s")
            __ASM_EMIT("fmla        v19.4s, v23.4s, v23.4s")
            __ASM_EMIT("fneg        v4.4s, v4.4s")                          // v4   = -(dr*si + di*sr)
            __ASM_EMIT("fneg        v5.4s, v5.4s")
            __ASM_EMIT("fneg        v6.4s, v6.4s")
            __ASM_EMIT("fneg        v7.4s, v7.4s")
            __ASM_EMIT("fdiv        v0.4s, v0.4s, v16.4s")                  // v0   = (dr*sr + di*si)/(dr*dr + di*di)
            __ASM_EMIT("fdiv        v1.4s, v1.4s, v17.4s")
            __ASM_EMIT("fdiv        v2.4s, v2.4s, v18.4s")
            __ASM_EMIT("fdiv        v3.4s, v3.4s, v19.4s")
            __ASM_EMIT("fdiv        v4.4s, v4.4s, v16.4s")                  // v4   = -(dr*si + di*sr)/(dr*dr + di*di)
            __ASM_EMIT("fdiv        v5.4s, v5.4s, v17.4s")
            __ASM_EMIT("fdiv        v6.4s, v6.4s, v18.4s")
            __ASM_EMIT("fdiv        v7.4s, v7.4s, v19.4s")

            __ASM_EMIT("subs        %[count], %[count], #16")
            __ASM_EMIT("st1         {v0.4s-v3.4s}, [%[dst_re]], #0x40")
            __ASM_EMIT("st1         {v4.4s-v7.4s}, [%[dst_im]], #0x40")
            __ASM_EMIT("b.hs        1b")

            // x8 blocks
            __ASM_EMIT("2:")
            __ASM_EMIT("adds        %[count], %[count], #8")
            __ASM_EMIT("b.lt        4f")
            __ASM_EMIT("ld1         {v24.4s, v25.4s}, [%[src_re]], #0x20")  // v24-v25  = sr
            __ASM_EMIT("ld1         {v28.4s, v29.4s}, [%[src_im]], #0x20")  // v28-v29  = si
            __ASM_EMIT("ld1         {v16.4s, v17.4s}, [%[dst_re]]")         // v16-v17  = dr
            __ASM_EMIT("ld1         {v20.4s, v21.4s}, [%[dst_im]]")         // v20-v21  = di
            __ASM_EMIT("fmul        v0.4s, v16.4s, v24.4s")                 // v0   = dr*sr
            __ASM_EMIT("fmul        v1.4s, v17.4s, v25.4s")
            __ASM_EMIT("fmul        v4.4s, v16.4s, v28.4s")                 // v4   = dr*si
            __ASM_EMIT("fmul        v5.4s, v17.4s, v29.4s")
            __ASM_EMIT("fmla        v0.4s, v20.4s, v28.4s")                 // v0   = dr*sr + di*si
            __ASM_EMIT("fmla        v1.4s, v21.4s, v29.4s")
            __ASM_EMIT("fmul        v16.4s, v16.4s, v16.4s")                // v16  = dr*dr
            __ASM_EMIT("fmul        v17.4s, v17.4s, v17.4s")
            __ASM_EMIT("fmla        v4.4s, v20.4s, v24.4s")                 // v4   = dr*si + di*sr
            __ASM_EMIT("fmla        v5.4s, v21.4s, v25.4s")
            __ASM_EMIT("fmla        v16.4s, v20.4s, v20.4s")                // v16  = dr*dr + di*di
            __ASM_EMIT("fmla        v17.4s, v21.4s, v21.4s")
            __ASM_EMIT("fneg        v4.4s, v4.4s")                          // v4   = -(dr*si + di*sr)
            __ASM_EMIT("fneg        v5.4s, v5.4s")
            __ASM_EMIT("fdiv        v0.4s, v0.4s, v16.4s")                  // v0   = (dr*sr + di*si)/(dr*dr + di*di)
            __ASM_EMIT("fdiv        v1.4s, v1.4s, v17.4s")
            __ASM_EMIT("fdiv        v4.4s, v4.4s, v16.4s")                  // v4   = -(dr*si + di*sr)/(dr*dr + di*di)
            __ASM_EMIT("fdiv        v5.4s, v5.4s, v17.4s")
            __ASM_EMIT("sub         %[count], %[count], #8")
            __ASM_EMIT("st1         {v0.4s, v1.4s}, [%[dst_re]], #0x20")
            __ASM_EMIT("st1         {v4.4s, v5.4s}, [%[dst_im]], #0x20")

            // x4 blocks
            __ASM_EMIT("4:")
            __ASM_EMIT("adds        %[count], %[count], #4")
            __ASM_EMIT("b.lt        6f")
            __ASM_EMIT("ld1         {v24.4s}, [%[src_re]], #0x10")          // v24-v25  = sr
            __ASM_EMIT("ld1         {v28.4s}, [%[src_im]], #0x10")          // v28-v29  = si
            __ASM_EMIT("ld1         {v16.4s}, [%[dst_re]]")                 // v16-v17  = dr
            __ASM_EMIT("ld1         {v20.4s}, [%[dst_im]]")                 // v20-v21  = di
            __ASM_EMIT("fmul        v0.4s, v16.4s, v24.4s")                 // v0   = dr*sr
            __ASM_EMIT("fmul        v4.4s, v16.4s, v28.4s")                 // v4   = dr*si
            __ASM_EMIT("fmla        v0.4s, v20.4s, v28.4s")                 // v0   = dr*sr + di*si
            __ASM_EMIT("fmul        v16.4s, v16.4s, v16.4s")                // v16  = dr*dr
            __ASM_EMIT("fmla        v4.4s, v20.4s, v24.4s")                 // v4   = dr*si + di*sr
            __ASM_EMIT("fmla        v16.4s, v20.4s, v20.4s")                // v16  = dr*dr + di*di
            __ASM_EMIT("fneg        v4.4s, v4.4s")                          // v4   = -(dr*si + di*sr)
            __ASM_EMIT("fdiv        v0.4s, v0.4s, v16.4s")                  // v0   = (dr*sr + di*si)/(dr*dr + di*di)
            __ASM_EMIT("fdiv        v4.4s, v4.4s, v16.4s")                  // v4   = -(dr*si + di*sr)/(dr*dr + di*di)
            __ASM_EMIT("sub         %[count], %[count], #4")
            __ASM_EMIT("st1         {v0.4s}, [%[dst_re]], #0x10")
            __ASM_EMIT("st1         {v4.4s}, [%[dst_im]], #0x10")

            // x1 blocks
            __ASM_EMIT("6:")
            __ASM_EMIT("adds        %[count], %[count], #3")
            __ASM_EMIT("b.lt        8f")
            __ASM_EMIT("7:")
            __ASM_EMIT("ld1r        {v24.4s}, [%[src_re]], #0x04")          // v24  = sr
            __ASM_EMIT("ld1r        {v28.4s}, [%[src_im]], #0x04")          // v28  = si
            __ASM_EMIT("ld1r        {v16.4s}, [%[dst_re]]")                 // v16  = dr
            __ASM_EMIT("ld1r        {v20.4s}, [%[dst_im]]")                 // v20  = di
            __ASM_EMIT("fmul        v0.4s, v16.4s, v24.4s")                 // v0   = dr*sr
            __ASM_EMIT("fmul        v4.4s, v16.4s, v28.4s")                 // v4   = dr*si
            __ASM_EMIT("fmla        v0.4s, v20.4s, v28.4s")                 // v0   = dr*sr + di*si
            __ASM_EMIT("fmul        v16.4s, v16.4s, v16.4s")                // v16  = dr*dr
            __ASM_EMIT("fmla        v4.4s, v20.4s, v24.4s")                 // v4   = dr*si + di*sr
            __ASM_EMIT("fmla        v16.4s, v20.4s, v20.4s")                // v16  = dr*dr + di*di
            __ASM_EMIT("fneg        v4.4s, v4.4s")                          // v4   = -(dr*si + di*sr)
            __ASM_EMIT("fdiv        v0.4s, v0.4s, v16.4s")                  // v0   = (dr*sr + di*si)/(dr*dr + di*di)
            __ASM_EMIT("fdiv        v4.4s, v4.4s, v16.4s")                  // v4   = -(dr*si + di*sr)/(dr*dr + di*di)
            __ASM_EMIT("str         s0, [%[dst_re]], #0x04")
            __ASM_EMIT("str         s4, [%[dst_im]], #0x04")
            __ASM_EMIT("subs        %[count], %[count], #1")
            __ASM_EMIT("b.ge        7b")

            __ASM_EMIT("8:")

            : [dst_re] "+r" (dst_re), [dst_im] "+r" (dst_im),
              [src_re] "+r" (src_re), [src_im] "+r" (src_im),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7",
              // "v8", "v9", "v10", "v11", "v12", "v13", "v14", "v15", // Avoid usage if possible
              "v16", "v17", "v18", "v19", "v20", "v21", "v22", "v23",
              "v24", "v25", "v26", "v27", "v28", "v29", "v30", "v31"
        );
    }

    void complex_div3(float *dst_re, float *dst_im, const float *t_re, const float *t_im, const float *b_re, const float *b_im, size_t count)
    {
        ARCH_AARCH64_ASM
        (
            __ASM_EMIT("subs        %[count], %[count], #16")
            __ASM_EMIT("b.lo        2f")

            // x16 blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("ld1         {v16.4s-v19.4s}, [%[b_re]], #0x40")     // v16-v19  = sr
            __ASM_EMIT("ld1         {v20.4s-v23.4s}, [%[b_im]], #0x40")     // v20-v23  = si
            __ASM_EMIT("ld1         {v24.4s-v27.4s}, [%[t_re]], #0x40")     // v24-v27  = dr
            __ASM_EMIT("ld1         {v28.4s-v31.4s}, [%[t_im]], #0x40")     // v28-v31  = di

            __ASM_EMIT("fmul        v0.4s, v16.4s, v24.4s")                 // v0   = sr*dr
            __ASM_EMIT("fmul        v1.4s, v17.4s, v25.4s")
            __ASM_EMIT("fmul        v2.4s, v18.4s, v26.4s")
            __ASM_EMIT("fmul        v3.4s, v19.4s, v27.4s")
            __ASM_EMIT("fmul        v4.4s, v16.4s, v28.4s")                 // v4   = sr*di
            __ASM_EMIT("fmul        v5.4s, v17.4s, v29.4s")
            __ASM_EMIT("fmul        v6.4s, v18.4s, v30.4s")
            __ASM_EMIT("fmul        v7.4s, v19.4s, v31.4s")
            __ASM_EMIT("fmla        v0.4s, v20.4s, v28.4s")                 // v0   = sr*dr + si*di
            __ASM_EMIT("fmla        v1.4s, v21.4s, v29.4s")
            __ASM_EMIT("fmla        v2.4s, v22.4s, v30.4s")
            __ASM_EMIT("fmla        v3.4s, v23.4s, v31.4s")
            __ASM_EMIT("fmul        v16.4s, v16.4s, v16.4s")                // v16  = sr*sr
            __ASM_EMIT("fmul        v17.4s, v17.4s, v17.4s")
            __ASM_EMIT("fmul        v18.4s, v18.4s, v18.4s")
            __ASM_EMIT("fmul        v19.4s, v19.4s, v19.4s")
            __ASM_EMIT("fmla        v4.4s, v20.4s, v24.4s")                 // v4   = sr*di + si*dr
            __ASM_EMIT("fmla        v5.4s, v21.4s, v25.4s")
            __ASM_EMIT("fmla        v6.4s, v22.4s, v26.4s")
            __ASM_EMIT("fmla        v7.4s, v23.4s, v27.4s")
            __ASM_EMIT("fmla        v16.4s, v20.4s, v20.4s")                // v16  = sr*sr + si*si
            __ASM_EMIT("fmla        v17.4s, v21.4s, v21.4s")
            __ASM_EMIT("fmla        v18.4s, v22.4s, v22.4s")
            __ASM_EMIT("fmla        v19.4s, v23.4s, v23.4s")
            __ASM_EMIT("fneg        v4.4s, v4.4s")                          // v4   = -(sr*di + si*dr)
            __ASM_EMIT("fneg        v5.4s, v5.4s")
            __ASM_EMIT("fneg        v6.4s, v6.4s")
            __ASM_EMIT("fneg        v7.4s, v7.4s")
            __ASM_EMIT("fdiv        v0.4s, v0.4s, v16.4s")                  // v0   = (sr*dr + si*di)/(sr*sr + si*si)
            __ASM_EMIT("fdiv        v1.4s, v1.4s, v17.4s")
            __ASM_EMIT("fdiv        v2.4s, v2.4s, v18.4s")
            __ASM_EMIT("fdiv        v3.4s, v3.4s, v19.4s")
            __ASM_EMIT("fdiv        v4.4s, v4.4s, v16.4s")                  // v4   = -(sr*di + si*dr)/(sr*sr + si*si)
            __ASM_EMIT("fdiv        v5.4s, v5.4s, v17.4s")
            __ASM_EMIT("fdiv        v6.4s, v6.4s, v18.4s")
            __ASM_EMIT("fdiv        v7.4s, v7.4s, v19.4s")

            __ASM_EMIT("subs        %[count], %[count], #16")
            __ASM_EMIT("st1         {v0.4s-v3.4s}, [%[dst_re]], #0x40")
            __ASM_EMIT("st1         {v4.4s-v7.4s}, [%[dst_im]], #0x40")
            __ASM_EMIT("b.hs        1b")

            // x8 blocks
            __ASM_EMIT("2:")
            __ASM_EMIT("adds        %[count], %[count], #8")
            __ASM_EMIT("b.lt        4f")
            __ASM_EMIT("ld1         {v16.4s, v17.4s}, [%[b_re]], #0x20")    // v16-v17  = sr
            __ASM_EMIT("ld1         {v20.4s, v21.4s}, [%[b_im]], #0x20")    // v20-v21  = si
            __ASM_EMIT("ld1         {v24.4s, v25.4s}, [%[t_re]], #0x20")    // v24-v25  = dr
            __ASM_EMIT("ld1         {v28.4s, v29.4s}, [%[t_im]], #0x20")    // v28-v29  = di
            __ASM_EMIT("fmul        v0.4s, v16.4s, v24.4s")                 // v0   = sr*dr
            __ASM_EMIT("fmul        v1.4s, v17.4s, v25.4s")
            __ASM_EMIT("fmul        v4.4s, v16.4s, v28.4s")                 // v4   = sr*di
            __ASM_EMIT("fmul        v5.4s, v17.4s, v29.4s")
            __ASM_EMIT("fmla        v0.4s, v20.4s, v28.4s")                 // v0   = sr*dr + si*di
            __ASM_EMIT("fmla        v1.4s, v21.4s, v29.4s")
            __ASM_EMIT("fmul        v16.4s, v16.4s, v16.4s")                // v16  = sr*sr
            __ASM_EMIT("fmul        v17.4s, v17.4s, v17.4s")
            __ASM_EMIT("fmla        v4.4s, v20.4s, v24.4s")                 // v4   = sr*di + si*dr
            __ASM_EMIT("fmla        v5.4s, v21.4s, v25.4s")
            __ASM_EMIT("fmla        v16.4s, v20.4s, v20.4s")                // v16  = sr*sr + si*si
            __ASM_EMIT("fmla        v17.4s, v21.4s, v21.4s")
            __ASM_EMIT("fneg        v4.4s, v4.4s")                          // v4   = -(sr*di + si*dr)
            __ASM_EMIT("fneg        v5.4s, v5.4s")
            __ASM_EMIT("fdiv        v0.4s, v0.4s, v16.4s")                  // v0   = (sr*dr + si*di)/(sr*sr + si*si)
            __ASM_EMIT("fdiv        v1.4s, v1.4s, v17.4s")
            __ASM_EMIT("fdiv        v4.4s, v4.4s, v16.4s")                  // v4   = -(sr*di + si*dr)/(sr*sr + si*si)
            __ASM_EMIT("fdiv        v5.4s, v5.4s, v17.4s")
            __ASM_EMIT("sub         %[count], %[count], #8")
            __ASM_EMIT("st1         {v0.4s, v1.4s}, [%[dst_re]], #0x20")
            __ASM_EMIT("st1         {v4.4s, v5.4s}, [%[dst_im]], #0x20")

            // x4 blocks
            __ASM_EMIT("4:")
            __ASM_EMIT("adds        %[count], %[count], #4")
            __ASM_EMIT("b.lt        6f")
            __ASM_EMIT("ld1         {v16.4s}, [%[b_re]], #0x10")            // v16-v17  = sr
            __ASM_EMIT("ld1         {v20.4s}, [%[b_im]], #0x10")            // v20-v21  = si
            __ASM_EMIT("ld1         {v24.4s}, [%[t_re]], #0x10")            // v24-v25  = dr
            __ASM_EMIT("ld1         {v28.4s}, [%[t_im]], #0x10")            // v28-v29  = di
            __ASM_EMIT("fmul        v0.4s, v16.4s, v24.4s")                 // v0   = sr*dr
            __ASM_EMIT("fmul        v4.4s, v16.4s, v28.4s")                 // v4   = sr*di
            __ASM_EMIT("fmla        v0.4s, v20.4s, v28.4s")                 // v0   = sr*dr + si*di
            __ASM_EMIT("fmul        v16.4s, v16.4s, v16.4s")                // v16  = sr*sr
            __ASM_EMIT("fmla        v4.4s, v20.4s, v24.4s")                 // v4   = sr*di + si*dr
            __ASM_EMIT("fmla        v16.4s, v20.4s, v20.4s")                // v16  = sr*sr + si*si
            __ASM_EMIT("fneg        v4.4s, v4.4s")                          // v4   = -(sr*di + si*dr)
            __ASM_EMIT("fdiv        v0.4s, v0.4s, v16.4s")                  // v0   = (sr*dr + si*di)/(sr*sr + si*si)
            __ASM_EMIT("fdiv        v4.4s, v4.4s, v16.4s")                  // v4   = -(sr*di + si*dr)/(sr*sr + si*si)
            __ASM_EMIT("sub         %[count], %[count], #4")
            __ASM_EMIT("st1         {v0.4s}, [%[dst_re]], #0x10")
            __ASM_EMIT("st1         {v4.4s}, [%[dst_im]], #0x10")

            // x1 blocks
            __ASM_EMIT("6:")
            __ASM_EMIT("adds        %[count], %[count], #3")
            __ASM_EMIT("b.lt        8f")
            __ASM_EMIT("7:")
            __ASM_EMIT("ld1r        {v16.4s}, [%[b_re]], #0x04")            // v16  = sr
            __ASM_EMIT("ld1r        {v20.4s}, [%[b_im]], #0x04")            // v20  = si
            __ASM_EMIT("ld1r        {v24.4s}, [%[t_re]], #0x04")            // v24  = dr
            __ASM_EMIT("ld1r        {v28.4s}, [%[t_im]], #0x04")            // v28  = di
            __ASM_EMIT("fmul        v0.4s, v16.4s, v24.4s")                 // v0   = sr*dr
            __ASM_EMIT("fmul        v4.4s, v16.4s, v28.4s")                 // v4   = sr*di
            __ASM_EMIT("fmla        v0.4s, v20.4s, v28.4s")                 // v0   = sr*dr + si*di
            __ASM_EMIT("fmul        v16.4s, v16.4s, v16.4s")                // v16  = sr*sr
            __ASM_EMIT("fmla        v4.4s, v20.4s, v24.4s")                 // v4   = sr*di + si*dr
            __ASM_EMIT("fmla        v16.4s, v20.4s, v20.4s")                // v16  = sr*sr + si*si
            __ASM_EMIT("fneg        v4.4s, v4.4s")                          // v4   = -(sr*di + si*dr)
            __ASM_EMIT("fdiv        v0.4s, v0.4s, v16.4s")                  // v0   = (sr*dr + si*di)/(sr*sr + si*si)
            __ASM_EMIT("fdiv        v4.4s, v4.4s, v16.4s")                  // v4   = -(sr*di + si*dr)/(sr*sr + si*si)
            __ASM_EMIT("str         s0, [%[dst_re]], #0x04")
            __ASM_EMIT("str         s4, [%[dst_im]], #0x04")
            __ASM_EMIT("subs        %[count], %[count], #1")
            __ASM_EMIT("b.ge        7b")

            __ASM_EMIT("8:")

            : [dst_re] "+r" (dst_re), [dst_im] "+r" (dst_im),
              [t_re] "+r" (t_re), [t_im] "+r" (t_im),
              [b_re] "+r" (b_re), [b_im] "+r" (b_im),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7",
              // "v8", "v9", "v10", "v11", "v12", "v13", "v14", "v15", // Avoid usage if possible
              "v16", "v17", "v18", "v19", "v20", "v21", "v22", "v23",
              "v24", "v25", "v26", "v27", "v28", "v29", "v30", "v31"
        );
    }

    void complex_mod(float *dst, const float *src_re, const float *src_im, size_t count)
    {
        ARCH_AARCH64_ASM
        (
            __ASM_EMIT("subs        %[count], %[count], #16")
            __ASM_EMIT("b.lo        2f")

            // x16 blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("ld1         {v16.4s-v19.4s}, [%[src_re]], #0x40")   // v16-v19  = r
            __ASM_EMIT("ld1         {v24.4s-v27.4s}, [%[src_im]], #0x40")   // v24-v27  = i

            __ASM_EMIT("fmul        v16.4s, v16.4s, v16.4s")                // v16  = r*r
            __ASM_EMIT("fmul        v17.4s, v17.4s, v17.4s")
            __ASM_EMIT("fmul        v18.4s, v18.4s, v18.4s")
            __ASM_EMIT("fmul        v19.4s, v19.4s, v19.4s")
            __ASM_EMIT("fmla        v16.4s, v24.4s, v24.4s")                // v16  = R = r*r + i*i
            __ASM_EMIT("fmla        v17.4s, v25.4s, v25.4s")
            __ASM_EMIT("fmla        v18.4s, v26.4s, v26.4s")
            __ASM_EMIT("fmla        v19.4s, v27.4s, v27.4s")
            __ASM_EMIT("fsqrt       v16.4s, v16.4s")                        // v16  = sqrt(R)
            __ASM_EMIT("fsqrt       v17.4s, v17.4s")
            __ASM_EMIT("fsqrt       v18.4s, v18.4s")
            __ASM_EMIT("fsqrt       v19.4s, v19.4s")

            __ASM_EMIT("subs        %[count], %[count], #16")
            __ASM_EMIT("st1         {v16.4s-v19.4s}, [%[dst]], #0x40")
            __ASM_EMIT("b.hs        1b")

            // x8 blocks
            __ASM_EMIT("2:")
            __ASM_EMIT("adds        %[count], %[count], #8")
            __ASM_EMIT("b.lt        4f")

            __ASM_EMIT("ld1         {v16.4s, v17.4s}, [%[src_re]], #0x20")  // v16-v17  = r
            __ASM_EMIT("ld1         {v24.4s, v25.4s}, [%[src_im]], #0x20")  // v24-v25  = i
            __ASM_EMIT("fmul        v16.4s, v16.4s, v16.4s")                // v16  = r*r
            __ASM_EMIT("fmul        v17.4s, v17.4s, v17.4s")
            __ASM_EMIT("fmla        v16.4s, v24.4s, v24.4s")                // v16  = R = r*r + i*i
            __ASM_EMIT("fmla        v17.4s, v25.4s, v25.4s")
            __ASM_EMIT("fsqrt       v16.4s, v16.4s")                        // v16  = sqrt(R)
            __ASM_EMIT("fsqrt       v17.4s, v17.4s")
            __ASM_EMIT("sub         %[count], %[count], #8")
            __ASM_EMIT("st1         {v16.4s, v17.4s}, [%[dst]], #0x20")

            // x4 blocks
            __ASM_EMIT("4:")
            __ASM_EMIT("adds        %[count], %[count], #4")
            __ASM_EMIT("b.lt        6f")

            __ASM_EMIT("ld1         {v16.4s}, [%[src_re]], #0x10")          // v16  = r
            __ASM_EMIT("ld1         {v24.4s}, [%[src_im]], #0x10")          // v24  = i
            __ASM_EMIT("fmul        v16.4s, v16.4s, v16.4s")                // v16  = r*r
            __ASM_EMIT("fmla        v16.4s, v24.4s, v24.4s")                // v16  = R = r*r + i*i
            __ASM_EMIT("fsqrt       v16.4s, v16.4s")                        // v16  = sqrt(R)
            __ASM_EMIT("sub         %[count], %[count], #4")
            __ASM_EMIT("st1         {v16.4s}, [%[dst]], #0x10")

            // x1 blocks
            __ASM_EMIT("6:")
            __ASM_EMIT("adds        %[count], %[count], #3")
            __ASM_EMIT("b.lt        8f")
            __ASM_EMIT("7:")

            __ASM_EMIT("ld1r        {v16.4s}, [%[src_re]], #0x04")          // v16  = r
            __ASM_EMIT("ld1r        {v24.4s}, [%[src_im]], #0x04")          // v24  = i
            __ASM_EMIT("fmul        v16.4s, v16.4s, v16.4s")                // v16  = r*r
            __ASM_EMIT("fmla        v16.4s, v24.4s, v24.4s")                // v16  = R = r*r + i*i
            __ASM_EMIT("fsqrt       v16.4s, v16.4s")                        // v16  = sqrt(R)
            __ASM_EMIT("str         s16, [%[dst]], #0x04")
            __ASM_EMIT("subs        %[count], %[count], #1")
            __ASM_EMIT("b.ge        7b")

            __ASM_EMIT("8:")

            : [dst] "+r" (dst), [src_re] "+r" (src_re), [src_im] "+r" (src_im),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7",
              // "v8", "v9", "v10", "v11", "v12", "v13", "v14", "v15", // Avoid usage if possible
              "v16", "v17", "v18", "v19", "v20", "v21", "v22", "v23",
              "v24", "v25", "v26", "v27", "v28", "v29", "v30", "v31"
        );
    }

    void complex_rcp1(float *dst_re, float *dst_im, size_t count)
    {
        ARCH_AARCH64_ASM
        (
            __ASM_EMIT("subs        %[count], %[count], #16")
            __ASM_EMIT("b.lo        2f")

            // x16 blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("ld1         {v16.4s-v19.4s}, [%[dst_re]]")          // v16-v19  = dr
            __ASM_EMIT("ld1         {v20.4s-v23.4s}, [%[dst_im]]")          // v20-v23  = di

            __ASM_EMIT("fmul        v0.4s, v16.4s, v16.4s")                 // v0   = dr*dr
            __ASM_EMIT("fmul        v1.4s, v17.4s, v17.4s")
            __ASM_EMIT("fmul        v2.4s, v18.4s, v18.4s")
            __ASM_EMIT("fmul        v3.4s, v19.4s, v19.4s")
            __ASM_EMIT("fmla        v0.4s, v20.4s, v20.4s")                 // v0   = dr*dr + di*di
            __ASM_EMIT("fmla        v1.4s, v21.4s, v21.4s")
            __ASM_EMIT("fmla        v2.4s, v22.4s, v22.4s")
            __ASM_EMIT("fmla        v3.4s, v23.4s, v23.4s")
            __ASM_EMIT("fneg        v20.4s, v20.4s")                        // v20  = -di
            __ASM_EMIT("fneg        v21.4s, v21.4s")
            __ASM_EMIT("fneg        v22.4s, v22.4s")
            __ASM_EMIT("fneg        v23.4s, v23.4s")
            __ASM_EMIT("fdiv        v16.4s, v16.4s, v0.4s")                 // v16  = dr/(dr*dr + di*di)
            __ASM_EMIT("fdiv        v17.4s, v17.4s, v1.4s")
            __ASM_EMIT("fdiv        v18.4s, v18.4s, v2.4s")
            __ASM_EMIT("fdiv        v19.4s, v19.4s, v3.4s")
            __ASM_EMIT("fdiv        v20.4s, v20.4s, v0.4s")                 // v20  = -di/(dr*dr + di*di)
            __ASM_EMIT("fdiv        v21.4s, v21.4s, v1.4s")
            __ASM_EMIT("fdiv        v22.4s, v22.4s, v2.4s")
            __ASM_EMIT("fdiv        v23.4s, v23.4s, v3.4s")

            __ASM_EMIT("subs        %[count], %[count], #16")
            __ASM_EMIT("st1         {v16.4s-v19.4s}, [%[dst_re]], #0x40")
            __ASM_EMIT("st1         {v20.4s-v23.4s}, [%[dst_im]], #0x40")
            __ASM_EMIT("b.hs        1b")

            // x8 blocks
            __ASM_EMIT("2:")
            __ASM_EMIT("adds        %[count], %[count], #8")
            __ASM_EMIT("b.lt        4f")
            __ASM_EMIT("ld1         {v16.4s, v17.4s}, [%[dst_re]]")         // v16-v17  = dr
            __ASM_EMIT("ld1         {v20.4s, v21.4s}, [%[dst_im]]")         // v20-v21  = di
            __ASM_EMIT("fmul        v0.4s, v16.4s, v16.4s")                 // v0   = dr*dr
            __ASM_EMIT("fmul        v1.4s, v17.4s, v17.4s")
            __ASM_EMIT("fmla        v0.4s, v20.4s, v20.4s")                 // v0   = dr*dr + di*di
            __ASM_EMIT("fmla        v1.4s, v21.4s, v21.4s")
            __ASM_EMIT("fneg        v20.4s, v20.4s")                        // v20  = -di
            __ASM_EMIT("fneg        v21.4s, v21.4s")
            __ASM_EMIT("fdiv        v16.4s, v16.4s, v0.4s")                 // v16  = dr/(dr*dr + di*di)
            __ASM_EMIT("fdiv        v17.4s, v17.4s, v1.4s")
            __ASM_EMIT("fdiv        v20.4s, v20.4s, v0.4s")                 // v20  = -di/(dr*dr + di*di)
            __ASM_EMIT("fdiv        v21.4s, v21.4s, v1.4s")
            __ASM_EMIT("sub         %[count], %[count], #8")
            __ASM_EMIT("st1         {v16.4s, v17.4s}, [%[dst_re]], #0x20")
            __ASM_EMIT("st1         {v20.4s, v21.4s}, [%[dst_im]], #0x20")

            // x4 blocks
            __ASM_EMIT("4:")
            __ASM_EMIT("adds        %[count], %[count], #4")
            __ASM_EMIT("b.lt        6f")
            __ASM_EMIT("ld1         {v16.4s}, [%[dst_re]]")                 // v16-v17  = dr
            __ASM_EMIT("ld1         {v20.4s}, [%[dst_im]]")                 // v20-v21  = di
            __ASM_EMIT("fmul        v0.4s, v16.4s, v16.4s")                 // v0   = dr*dr
            __ASM_EMIT("fmla        v0.4s, v20.4s, v20.4s")                 // v0   = dr*dr + di*di
            __ASM_EMIT("fneg        v20.4s, v20.4s")                        // v20  = -di
            __ASM_EMIT("fdiv        v16.4s, v16.4s, v0.4s")                 // v16  = dr/(dr*dr + di*di)
            __ASM_EMIT("fdiv        v20.4s, v20.4s, v0.4s")                 // v20  = -di/(dr*dr + di*di)
            __ASM_EMIT("sub         %[count], %[count], #4")
            __ASM_EMIT("st1         {v16.4s}, [%[dst_re]], #0x10")
            __ASM_EMIT("st1         {v20.4s}, [%[dst_im]], #0x10")

            // x1 blocks
            __ASM_EMIT("6:")
            __ASM_EMIT("adds        %[count], %[count], #3")
            __ASM_EMIT("b.lt        8f")
            __ASM_EMIT("7:")
            __ASM_EMIT("ld1r        {v16.4s}, [%[dst_re]]")                 // v16  = dr
            __ASM_EMIT("ld1r        {v20.4s}, [%[dst_im]]")                 // v20  = di
            __ASM_EMIT("fmul        v0.4s, v16.4s, v16.4s")                 // v0   = dr*dr
            __ASM_EMIT("fmla        v0.4s, v20.4s, v20.4s")                 // v0   = dr*dr + di*di
            __ASM_EMIT("fneg        v20.4s, v20.4s")                        // v20  = -di
            __ASM_EMIT("fdiv        v16.4s, v16.4s, v0.4s")                 // v16  = dr/(dr*dr + di*di)
            __ASM_EMIT("fdiv        v20.4s, v20.4s, v0.4s")                 // v20  = -di/(dr*dr + di*di)
            __ASM_EMIT("str         s16, [%[dst_re]], #0x04")
            __ASM_EMIT("str         s20, [%[dst_im]], #0x04")
            __ASM_EMIT("subs        %[count], %[count], #1")
            __ASM_EMIT("b.ge        7b")

            __ASM_EMIT("8:")

            : [dst_re] "+r" (dst_re), [dst_im] "+r" (dst_im),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "v0", "v1", "v2", "v3",
              // "v8", "v9", "v10", "v11", "v12", "v13", "v14", "v15", // Avoid usage if possible
              "v16", "v17", "v18", "v19", "v20", "v21", "v22", "v23"
        );
    }

    void complex_rcp2(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t count)
    {
        ARCH_AARCH64_ASM
        (
            __ASM_EMIT("subs        %[count], %[count], #16")
            __ASM_EMIT("b.lo        2f")

            // x16 blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("ld1         {v16.4s-v19.4s}, [%[src_re]], #0x40")   // v16-v19  = dr
            __ASM_EMIT("ld1         {v20.4s-v23.4s}, [%[src_im]], #0x40")   // v20-v23  = di

            __ASM_EMIT("fmul        v0.4s, v16.4s, v16.4s")                 // v0   = dr*dr
            __ASM_EMIT("fmul        v1.4s, v17.4s, v17.4s")
            __ASM_EMIT("fmul        v2.4s, v18.4s, v18.4s")
            __ASM_EMIT("fmul        v3.4s, v19.4s, v19.4s")
            __ASM_EMIT("fmla        v0.4s, v20.4s, v20.4s")                 // v0   = dr*dr + di*di
            __ASM_EMIT("fmla        v1.4s, v21.4s, v21.4s")
            __ASM_EMIT("fmla        v2.4s, v22.4s, v22.4s")
            __ASM_EMIT("fmla        v3.4s, v23.4s, v23.4s")
            __ASM_EMIT("fneg        v20.4s, v20.4s")                        // v20  = -di
            __ASM_EMIT("fneg        v21.4s, v21.4s")
            __ASM_EMIT("fneg        v22.4s, v22.4s")
            __ASM_EMIT("fneg        v23.4s, v23.4s")
            __ASM_EMIT("fdiv        v16.4s, v16.4s, v0.4s")                 // v16  = dr/(dr*dr + di*di)
            __ASM_EMIT("fdiv        v17.4s, v17.4s, v1.4s")
            __ASM_EMIT("fdiv        v18.4s, v18.4s, v2.4s")
            __ASM_EMIT("fdiv        v19.4s, v19.4s, v3.4s")
            __ASM_EMIT("fdiv        v20.4s, v20.4s, v0.4s")                 // v20  = -di/(dr*dr + di*di)
            __ASM_EMIT("fdiv        v21.4s, v21.4s, v1.4s")
            __ASM_EMIT("fdiv        v22.4s, v22.4s, v2.4s")
            __ASM_EMIT("fdiv        v23.4s, v23.4s, v3.4s")

            __ASM_EMIT("subs        %[count], %[count], #16")
            __ASM_EMIT("st1         {v16.4s-v19.4s}, [%[dst_re]], #0x40")
            __ASM_EMIT("st1         {v20.4s-v23.4s}, [%[dst_im]], #0x40")
            __ASM_EMIT("b.hs        1b")

            // x8 blocks
            __ASM_EMIT("2:")
            __ASM_EMIT("adds        %[count], %[count], #8")
            __ASM_EMIT("b.lt        4f")
            __ASM_EMIT("ld1         {v16.4s, v17.4s}, [%[src_re]], #0x20")  // v16-v17  = dr
            __ASM_EMIT("ld1         {v20.4s, v21.4s}, [%[src_im]], #0x20")  // v20-v21  = di
            __ASM_EMIT("fmul        v0.4s, v16.4s, v16.4s")                 // v0   = dr*dr
            __ASM_EMIT("fmul        v1.4s, v17.4s, v17.4s")
            __ASM_EMIT("fmla        v0.4s, v20.4s, v20.4s")                 // v0   = dr*dr + di*di
            __ASM_EMIT("fmla        v1.4s, v21.4s, v21.4s")
            __ASM_EMIT("fneg        v20.4s, v20.4s")                        // v20  = -di
            __ASM_EMIT("fneg        v21.4s, v21.4s")
            __ASM_EMIT("fdiv        v16.4s, v16.4s, v0.4s")                 // v16  = dr/(dr*dr + di*di)
            __ASM_EMIT("fdiv        v17.4s, v17.4s, v1.4s")
            __ASM_EMIT("fdiv        v20.4s, v20.4s, v0.4s")                 // v20  = -di/(dr*dr + di*di)
            __ASM_EMIT("fdiv        v21.4s, v21.4s, v1.4s")
            __ASM_EMIT("sub         %[count], %[count], #8")
            __ASM_EMIT("st1         {v16.4s, v17.4s}, [%[dst_re]], #0x20")
            __ASM_EMIT("st1         {v20.4s, v21.4s}, [%[dst_im]], #0x20")

            // x4 blocks
            __ASM_EMIT("4:")
            __ASM_EMIT("adds        %[count], %[count], #4")
            __ASM_EMIT("b.lt        6f")
            __ASM_EMIT("ld1         {v16.4s}, [%[src_re]], #0x10")          // v16-v17  = dr
            __ASM_EMIT("ld1         {v20.4s}, [%[src_im]], #0x10")          // v20-v21  = di
            __ASM_EMIT("fmul        v0.4s, v16.4s, v16.4s")                 // v0   = dr*dr
            __ASM_EMIT("fmla        v0.4s, v20.4s, v20.4s")                 // v0   = dr*dr + di*di
            __ASM_EMIT("fneg        v20.4s, v20.4s")                        // v20  = -di
            __ASM_EMIT("fdiv        v16.4s, v16.4s, v0.4s")                 // v16  = dr/(dr*dr + di*di)
            __ASM_EMIT("fdiv        v20.4s, v20.4s, v0.4s")                 // v20  = -di/(dr*dr + di*di)
            __ASM_EMIT("sub         %[count], %[count], #4")
            __ASM_EMIT("st1         {v16.4s}, [%[dst_re]], #0x10")
            __ASM_EMIT("st1         {v20.4s}, [%[dst_im]], #0x10")

            // x1 blocks
            __ASM_EMIT("6:")
            __ASM_EMIT("adds        %[count], %[count], #3")
            __ASM_EMIT("b.lt        8f")
            __ASM_EMIT("7:")
            __ASM_EMIT("ld1r        {v16.4s}, [%[src_re]], #0x04")          // v16  = dr
            __ASM_EMIT("ld1r        {v20.4s}, [%[src_im]], #0x04")          // v20  = di
            __ASM_EMIT("fmul        v0.4s, v16.4s, v16.4s")                 // v0   = dr*dr
            __ASM_EMIT("fmla        v0.4s, v20.4s, v20.4s")                 // v0   = dr*dr + di*di
            __ASM_EMIT("fneg        v20.4s, v20.4s")                        // v20  = -di
            __ASM_EMIT("fdiv        v16.4s, v16.4s, v0.4s")                 // v16  = dr/(dr*dr + di*di)
            __ASM_EMIT("fdiv        v20.4s, v20.4s, v0.4s")                 // v20  = -di/(dr*dr + di*di)
            __ASM_EMIT("str         s16, [%[dst_re]], #0x04")
            __ASM_EMIT("str         s20, [%[dst_im]], #0x04")
            __ASM_EMIT("subs        %[count], %[count], #1")
            __ASM_EMIT("b.ge        7b")

            __ASM_EMIT("8:")

            : [dst_re] "+r" (dst_re), [dst_im] "+r" (dst_im),
              [src_re] "+r" (src_re), [src_im] "+r" (src_im),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "v0", "v1", "v2", "v3",
              // "v8", "v9", "v10", "v11", "v12", "v13", "v14", "v15", // Avoid usage if possible
              "v16", "v17", "v18", "v19", "v20", "v21", "v22", "v23"
        );
    }
}

#endif /* DSP_ARCH_AARCH64_ASIMD_COMPLEX_H_ */
