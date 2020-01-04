/*
 * transfer.h
 *
 *  Created on: 4 янв. 2020 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_AARCH64_ASIMD_FILTERS_TRANSFER_H_
#define DSP_ARCH_AARCH64_ASIMD_FILTERS_TRANSFER_H_

#ifndef DSP_ARCH_AARCH64_ASIMD_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_AARCH64_ASIMD_IMPL */

namespace asimd
{
    void filter_transfer_calc_ri(float *re, float *im, const f_cascade_t *c, const float *freq, size_t count)
    {
        ARCH_AARCH64_ASM(
            // Unpack filter params
            __ASM_EMIT("ld3r                {v18.4s, v19.4s, v20.4s}, [%[c]]")
            __ASM_EMIT("add                 %[c], %[c], #0x10")
            __ASM_EMIT("ld3r                {v21.4s, v22.4s, v23.4s}, [%[c]]")
            // x8 blocks
            __ASM_EMIT("subs                %[count], %[count], #8")
            __ASM_EMIT("b.lo                2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("ldp                 q6, q7, [%[f]]")                        // v6   = f
            __ASM_EMIT("fmul                v16.4s, v6.4s, v6.4s")                  // v16  = f2 = f*f
            __ASM_EMIT("fmul                v17.4s, v7.4s, v7.4s")
            __ASM_EMIT("fmul                v3.4s, v19.4s, v6.4s")                  // v3   = t_im = t1*f
            __ASM_EMIT("fmul                v5.4s, v19.4s, v7.4s")
            __ASM_EMIT("fmul                v6.4s, v22.4s, v6.4s")                  // v6   = b_im = b1*f
            __ASM_EMIT("fmul                v7.4s, v22.4s, v7.4s")
            __ASM_EMIT("fmul                v0.4s, v20.4s, v16.4s")                 // v0   = t2*f2
            __ASM_EMIT("fmul                v1.4s, v20.4s, v17.4s")
            __ASM_EMIT("fmul                v16.4s, v23.4s, v16.4s")                // v16  = b2*f2
            __ASM_EMIT("fmul                v17.4s, v23.4s, v17.4s")
            __ASM_EMIT("fsub                v2.4s, v18.4s, v0.4s")                  // v2   = t_re = t0 - t2*f2
            __ASM_EMIT("fsub                v4.4s, v18.4s, v1.4s")
            __ASM_EMIT("fsub                v16.4s, v21.4s, v16.4s")                // v16  = b_re = b0 - b2*f2
            __ASM_EMIT("fsub                v17.4s, v21.4s, v17.4s")
            __ASM_EMIT("fmul                v0.4s, v2.4s, v16.4s")                  // v0   = t_re*b_re
            __ASM_EMIT("fmul                v1.4s, v3.4s, v16.4s")                  // v1   = t_im*b_re
            __ASM_EMIT("fmla                v0.4s, v3.4s, v6.4s")                   // v0   = t_re*b_re + t_im*b_im
            __ASM_EMIT("fmls                v1.4s, v2.4s, v6.4s")                   // v1   = t_im*b_re - t_re*b_im
            __ASM_EMIT("fmul                v2.4s, v4.4s, v17.4s")
            __ASM_EMIT("fmul                v3.4s, v5.4s, v17.4s")
            __ASM_EMIT("fmla                v2.4s, v5.4s, v7.4s")
            __ASM_EMIT("fmls                v3.4s, v4.4s, v7.4s")
            __ASM_EMIT("fmul                v4.4s, v16.4s, v16.4s")                 // v4   = b_re*b_re
            __ASM_EMIT("fmul                v5.4s, v17.4s, v17.4s")
            __ASM_EMIT("fmla                v4.4s, v6.4s, v6.4s")                   // v4   = W = b_re*b_re + b_im*b_im
            __ASM_EMIT("fmla                v5.4s, v7.4s, v7.4s")
            __ASM_EMIT("frecpe              v6.4s, v4.4s")                          // v6   = s2
            __ASM_EMIT("frecpe              v7.4s, v5.4s")
            __ASM_EMIT("frecps              v16.4s, v6.4s, v4.4s")                  // v16  = (2 - R*s2)
            __ASM_EMIT("frecps              v17.4s, v7.4s, v5.4s")
            __ASM_EMIT("fmul                v6.4s, v16.4s, v6.4s")                  // v6   = s2' = s2 * (2 - R*s2)
            __ASM_EMIT("fmul                v7.4s, v17.4s, v7.4s")
            __ASM_EMIT("frecps              v16.4s, v6.4s, v4.4s")                  // v16  = (2 - R*s2')
            __ASM_EMIT("frecps              v17.4s, v7.4s, v5.4s")
            __ASM_EMIT("fmul                v6.4s, v16.4s, v6.4s")                  // v6   = 1/W = s2" = s2' * (2 - R*s2')
            __ASM_EMIT("fmul                v7.4s, v17.4s, v7.4s")
            __ASM_EMIT("fmul                v0.4s, v0.4s, v6.4s")                   // v0   = a_re = t_re / W
            __ASM_EMIT("fmul                v2.4s, v2.4s, v7.4s")
            __ASM_EMIT("fmul                v1.4s, v1.4s, v6.4s")                   // v1   = a_im = t_im / W
            __ASM_EMIT("fmul                v3.4s, v3.4s, v7.4s")
            // Store data
            __ASM_EMIT("stp                 q0, q2, [%[re]]")
            __ASM_EMIT("stp                 q1, q3, [%[im]]")
            __ASM_EMIT("subs                %[count], %[count], #8")
            __ASM_EMIT("add                 %[f], %[f], #0x20")
            __ASM_EMIT("add                 %[re], %[re], #0x20")
            __ASM_EMIT("add                 %[im], %[im], #0x20")
            __ASM_EMIT("b.hs                1b")
            __ASM_EMIT("2:")
            // x4 blocks
            __ASM_EMIT("adds                %[count], %[count], #4")
            __ASM_EMIT("b.lt                4f")
            __ASM_EMIT("1:")
            __ASM_EMIT("ldr                 q6, [%[f]]")                            // v6   = f
            __ASM_EMIT("fmul                v16.4s, v6.4s, v6.4s")                  // v16  = f2 = f*f
            __ASM_EMIT("fmul                v3.4s, v19.4s, v6.4s")                  // v3   = t_im = t1*f
            __ASM_EMIT("fmul                v6.4s, v22.4s, v6.4s")                  // v6   = b_im = b1*f
            __ASM_EMIT("fmul                v0.4s, v20.4s, v16.4s")                 // v0   = t2*f2
            __ASM_EMIT("fmul                v16.4s, v23.4s, v16.4s")                // v16  = b2*f2
            __ASM_EMIT("fsub                v2.4s, v18.4s, v0.4s")                  // v2   = t_re = t0 - t2*f2
            __ASM_EMIT("fsub                v16.4s, v21.4s, v16.4s")                // v16  = b_re = b0 - b2*f2
            __ASM_EMIT("fmul                v0.4s, v2.4s, v16.4s")                  // v0   = t_re*b_re
            __ASM_EMIT("fmul                v1.4s, v3.4s, v16.4s")                  // v1   = t_im*b_re
            __ASM_EMIT("fmla                v0.4s, v3.4s, v6.4s")                   // v0   = t_re*b_re + t_im*b_im
            __ASM_EMIT("fmls                v1.4s, v2.4s, v6.4s")                   // v1   = t_im*b_re - t_re*b_im
            __ASM_EMIT("fmul                v4.4s, v16.4s, v16.4s")                 // v4   = b_re*b_re
            __ASM_EMIT("fmla                v4.4s, v6.4s, v6.4s")                   // v4   = W = b_re*b_re + b_im*b_im
            __ASM_EMIT("frecpe              v6.4s, v4.4s")                          // v6   = s2
            __ASM_EMIT("frecps              v16.4s, v6.4s, v4.4s")                  // v16  = (2 - R*s2)
            __ASM_EMIT("fmul                v6.4s, v16.4s, v6.4s")                  // v6   = s2' = s2 * (2 - R*s2)
            __ASM_EMIT("frecps              v16.4s, v6.4s, v4.4s")                  // v16  = (2 - R*s2')
            __ASM_EMIT("fmul                v6.4s, v16.4s, v6.4s")                  // v6   = 1/W = s2" = s2' * (2 - R*s2')
            __ASM_EMIT("fmul                v0.4s, v0.4s, v6.4s")                   // v0   = a_re = t_re / W
            __ASM_EMIT("fmul                v1.4s, v1.4s, v6.4s")                   // v1   = a_im = t_im / W
            // Update data
            __ASM_EMIT("str                 q0, [%[re]]")
            __ASM_EMIT("str                 q1, [%[im]]")
            __ASM_EMIT("sub                 %[count], %[count], #4")
            __ASM_EMIT("add                 %[f], %[f], #0x10")
            __ASM_EMIT("add                 %[re], %[re], #0x10")
            __ASM_EMIT("add                 %[im], %[im], #0x10")
            __ASM_EMIT("4:")
            // x2 blocks
            __ASM_EMIT("adds                %[count], %[count], #2")
            __ASM_EMIT("blt                 6f")
            __ASM_EMIT("1:")
            __ASM_EMIT("ldr                 d6, [%[f]]")                            // v6   = f
            __ASM_EMIT("fmul                v16.4s, v6.4s, v6.4s")                  // v16  = f2 = f*f
            __ASM_EMIT("fmul                v3.4s, v19.4s, v6.4s")                  // v3   = t_im = t1*f
            __ASM_EMIT("fmul                v6.4s, v22.4s, v6.4s")                  // v6   = b_im = b1*f
            __ASM_EMIT("fmul                v0.4s, v20.4s, v16.4s")                 // v0   = t2*f2
            __ASM_EMIT("fmul                v16.4s, v23.4s, v16.4s")                // v16  = b2*f2
            __ASM_EMIT("fsub                v2.4s, v18.4s, v0.4s")                  // v2   = t_re = t0 - t2*f2
            __ASM_EMIT("fsub                v16.4s, v21.4s, v16.4s")                // v16  = b_re = b0 - b2*f2
            __ASM_EMIT("fmul                v0.4s, v2.4s, v16.4s")                  // v0   = t_re*b_re
            __ASM_EMIT("fmul                v1.4s, v3.4s, v16.4s")                  // v1   = t_im*b_re
            __ASM_EMIT("fmla                v0.4s, v3.4s, v6.4s")                   // v0   = t_re*b_re + t_im*b_im
            __ASM_EMIT("fmls                v1.4s, v2.4s, v6.4s")                   // v1   = t_im*b_re - t_re*b_im
            __ASM_EMIT("fmul                v4.4s, v16.4s, v16.4s")                 // v4   = b_re*b_re
            __ASM_EMIT("fmla                v4.4s, v6.4s, v6.4s")                   // v4   = W = b_re*b_re + b_im*b_im
            __ASM_EMIT("frecpe              v6.4s, v4.4s")                          // v6   = s2
            __ASM_EMIT("frecps              v16.4s, v6.4s, v4.4s")                  // v16  = (2 - R*s2)
            __ASM_EMIT("fmul                v6.4s, v16.4s, v6.4s")                  // v6   = s2' = s2 * (2 - R*s2)
            __ASM_EMIT("frecps              v16.4s, v6.4s, v4.4s")                  // v16  = (2 - R*s2')
            __ASM_EMIT("fmul                v6.4s, v16.4s, v6.4s")                  // v6   = 1/W = s2" = s2' * (2 - R*s2')
            __ASM_EMIT("fmul                v0.4s, v0.4s, v6.4s")                   // v0   = a_re = t_re / W
            __ASM_EMIT("fmul                v1.4s, v1.4s, v6.4s")                   // v1   = a_im = t_im / W
            // Update data
            __ASM_EMIT("str                 d0, [%[re]]")
            __ASM_EMIT("str                 d1, [%[im]]")
            __ASM_EMIT("sub                 %[count], %[count], #2")
            __ASM_EMIT("add                 %[f], %[f], #0x08")
            __ASM_EMIT("add                 %[re], %[re], #0x08")
            __ASM_EMIT("add                 %[im], %[im], #0x08")
            __ASM_EMIT("6:")
            // x1 blocks
            __ASM_EMIT("adds                %[count], %[count], #1")
            __ASM_EMIT("b.lt                8f")
            __ASM_EMIT("1:")
            __ASM_EMIT("ld1r                {v6.4s}, [%[f]]")                       // v6   = f
            __ASM_EMIT("fmul                v16.4s, v6.4s, v6.4s")                  // v16  = f2 = f*f
            __ASM_EMIT("fmul                v3.4s, v19.4s, v6.4s")                  // v3   = t_im = t1*f
            __ASM_EMIT("fmul                v6.4s, v22.4s, v6.4s")                  // v6   = b_im = b1*f
            __ASM_EMIT("fmul                v0.4s, v20.4s, v16.4s")                 // v0   = t2*f2
            __ASM_EMIT("fmul                v16.4s, v23.4s, v16.4s")                // v16  = b2*f2
            __ASM_EMIT("fsub                v2.4s, v18.4s, v0.4s")                  // v2   = t_re = t0 - t2*f2
            __ASM_EMIT("fsub                v16.4s, v21.4s, v16.4s")                // v16  = b_re = b0 - b2*f2
            __ASM_EMIT("fmul                v0.4s, v2.4s, v16.4s")                  // v0   = t_re*b_re
            __ASM_EMIT("fmul                v1.4s, v3.4s, v16.4s")                  // v1   = t_im*b_re
            __ASM_EMIT("fmla                v0.4s, v3.4s, v6.4s")                   // v0   = t_re*b_re + t_im*b_im
            __ASM_EMIT("fmls                v1.4s, v2.4s, v6.4s")                   // v1   = t_im*b_re - t_re*b_im
            __ASM_EMIT("fmul                v4.4s, v16.4s, v16.4s")                 // v4   = b_re*b_re
            __ASM_EMIT("fmla                v4.4s, v6.4s, v6.4s")                   // v4   = W = b_re*b_re + b_im*b_im
            __ASM_EMIT("frecpe              v6.4s, v4.4s")                          // v6   = s2
            __ASM_EMIT("frecps              v16.4s, v6.4s, v4.4s")                  // v16  = (2 - R*s2)
            __ASM_EMIT("fmul                v6.4s, v16.4s, v6.4s")                  // v6   = s2' = s2 * (2 - R*s2)
            __ASM_EMIT("frecps              v16.4s, v6.4s, v4.4s")                  // v16  = (2 - R*s2')
            __ASM_EMIT("fmul                v6.4s, v16.4s, v6.4s")                  // v6   = 1/W = s2" = s2' * (2 - R*s2')
            __ASM_EMIT("fmul                v0.4s, v0.4s, v6.4s")                   // v0   = a_re = t_re / W
            __ASM_EMIT("fmul                v1.4s, v1.4s, v6.4s")                   // v1   = a_im = t_im / W
            // Update data
            __ASM_EMIT("st1                 {v0.s}[0], [%[re]]")
            __ASM_EMIT("st1                 {v1.s}[0], [%[im]]")
            __ASM_EMIT("8:")

            : [re] "+r" (re), [im] "+r" (im), [f] "+r" (freq),
              [count] "+r" (count), [c] "+r" (c)
            :
            : "cc", "memory",
              "v0", "v1", "v2", "v3",
              "v4", "v5", "v6", "v7",
              "v16", "v17", "v18", "v19",
              "v20", "v21", "v22", "v23"
        );
    }

    void filter_transfer_apply_ri(float *re, float *im, const f_cascade_t *c, const float *freq, size_t count)
    {
        ARCH_AARCH64_ASM(
            // Unpack filter params
            __ASM_EMIT("ld3r                {v18.4s, v19.4s, v20.4s}, [%[c]]")
            __ASM_EMIT("add                 %[c], %[c], #0x10")
            __ASM_EMIT("ld3r                {v21.4s, v22.4s, v23.4s}, [%[c]]")
            // x8 blocks
            __ASM_EMIT("subs                %[count], %[count], #8")
            __ASM_EMIT("b.lo                2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("ldp                 q6, q7, [%[f]]")                        // v6   = f
            __ASM_EMIT("fmul                v16.4s, v6.4s, v6.4s")                  // v16  = f2 = f*f
            __ASM_EMIT("fmul                v17.4s, v7.4s, v7.4s")
            __ASM_EMIT("fmul                v3.4s, v19.4s, v6.4s")                  // v3   = t_im = t1*f
            __ASM_EMIT("fmul                v5.4s, v19.4s, v7.4s")
            __ASM_EMIT("fmul                v6.4s, v22.4s, v6.4s")                  // v6   = b_im = b1*f
            __ASM_EMIT("fmul                v7.4s, v22.4s, v7.4s")
            __ASM_EMIT("fmul                v0.4s, v20.4s, v16.4s")                 // v0   = t2*f2
            __ASM_EMIT("fmul                v1.4s, v20.4s, v17.4s")
            __ASM_EMIT("fmul                v16.4s, v23.4s, v16.4s")                // v16  = b2*f2
            __ASM_EMIT("fmul                v17.4s, v23.4s, v17.4s")
            __ASM_EMIT("fsub                v2.4s, v18.4s, v0.4s")                  // v2   = t_re = t0 - t2*f2
            __ASM_EMIT("fsub                v4.4s, v18.4s, v1.4s")
            __ASM_EMIT("fsub                v16.4s, v21.4s, v16.4s")                // v16  = b_re = b0 - b2*f2
            __ASM_EMIT("fsub                v17.4s, v21.4s, v17.4s")
            __ASM_EMIT("fmul                v0.4s, v2.4s, v16.4s")                  // v0   = t_re*b_re
            __ASM_EMIT("fmul                v1.4s, v3.4s, v16.4s")                  // v1   = t_im*b_re
            __ASM_EMIT("fmla                v0.4s, v3.4s, v6.4s")                   // v0   = t_re*b_re + t_im*b_im
            __ASM_EMIT("fmls                v1.4s, v2.4s, v6.4s")                   // v1   = t_im*b_re - t_re*b_im
            __ASM_EMIT("fmul                v2.4s, v4.4s, v17.4s")
            __ASM_EMIT("fmul                v3.4s, v5.4s, v17.4s")
            __ASM_EMIT("fmla                v2.4s, v5.4s, v7.4s")
            __ASM_EMIT("fmls                v3.4s, v4.4s, v7.4s")
            __ASM_EMIT("fmul                v4.4s, v16.4s, v16.4s")                 // v4   = b_re*b_re
            __ASM_EMIT("fmul                v5.4s, v17.4s, v17.4s")
            __ASM_EMIT("fmla                v4.4s, v6.4s, v6.4s")                   // v4   = W = b_re*b_re + b_im*b_im
            __ASM_EMIT("fmla                v5.4s, v7.4s, v7.4s")
            __ASM_EMIT("frecpe              v6.4s, v4.4s")                          // v6   = s2
            __ASM_EMIT("frecpe              v7.4s, v5.4s")
            __ASM_EMIT("frecps              v16.4s, v6.4s, v4.4s")                  // v16  = (2 - R*s2)
            __ASM_EMIT("frecps              v17.4s, v7.4s, v5.4s")
            __ASM_EMIT("fmul                v6.4s, v16.4s, v6.4s")                  // v6   = s2' = s2 * (2 - R*s2)
            __ASM_EMIT("fmul                v7.4s, v17.4s, v7.4s")
            __ASM_EMIT("frecps              v16.4s, v6.4s, v4.4s")                  // v16  = (2 - R*s2')
            __ASM_EMIT("frecps              v17.4s, v7.4s, v5.4s")
            __ASM_EMIT("fmul                v6.4s, v16.4s, v6.4s")                  // v6   = 1/W = s2" = s2' * (2 - R*s2')
            __ASM_EMIT("fmul                v7.4s, v17.4s, v7.4s")
            __ASM_EMIT("fmul                v0.4s, v0.4s, v6.4s")                   // v0   = a_re = t_re / W
            __ASM_EMIT("fmul                v2.4s, v2.4s, v7.4s")
            __ASM_EMIT("fmul                v1.4s, v1.4s, v6.4s")                   // v1   = a_im = t_im / W
            __ASM_EMIT("fmul                v3.4s, v3.4s, v7.4s")
            // Update data
            __ASM_EMIT("ldp                 q6, q7, [%[re]]")                       // v6   = b_re
            __ASM_EMIT("ldp                 q16, q17, [%[im]]")                     // v16  = b_im
            __ASM_EMIT("fmul                v4.4s, v0.4s, v6.4s")                   // v4   = a_re*b_re
            __ASM_EMIT("fmul                v5.4s, v1.4s, v6.4s")                   // v5   = a_im*b_re
            __ASM_EMIT("fmls                v4.4s, v1.4s, v16.4s")                  // v4   = a_re*b_re - a_im*b_im
            __ASM_EMIT("fmla                v5.4s, v0.4s, v16.4s")                  // v5   = a_im*b_re + a_re*b_im
            __ASM_EMIT("fmul                v6.4s, v2.4s, v7.4s")
            __ASM_EMIT("fmul                v7.4s, v3.4s, v7.4s")
            __ASM_EMIT("fmls                v6.4s, v3.4s, v17.4s")
            __ASM_EMIT("fmla                v7.4s, v2.4s, v17.4s")
            __ASM_EMIT("stp                 q4, q6, [%[re]]")
            __ASM_EMIT("stp                 q5, q7, [%[im]]")
            __ASM_EMIT("subs                %[count], %[count], #8")
            __ASM_EMIT("add                 %[f], %[f], #0x20")
            __ASM_EMIT("add                 %[re], %[re], #0x20")
            __ASM_EMIT("add                 %[im], %[im], #0x20")
            __ASM_EMIT("b.hs                1b")
            __ASM_EMIT("2:")
            // x4 blocks
            __ASM_EMIT("adds                %[count], %[count], #4")
            __ASM_EMIT("b.lt                4f")
            __ASM_EMIT("1:")
            __ASM_EMIT("ldr                 q6, [%[f]]")                            // v6   = f
            __ASM_EMIT("fmul                v16.4s, v6.4s, v6.4s")                  // v16  = f2 = f*f
            __ASM_EMIT("fmul                v3.4s, v19.4s, v6.4s")                  // v3   = t_im = t1*f
            __ASM_EMIT("fmul                v6.4s, v22.4s, v6.4s")                  // v6   = b_im = b1*f
            __ASM_EMIT("fmul                v0.4s, v20.4s, v16.4s")                 // v0   = t2*f2
            __ASM_EMIT("fmul                v16.4s, v23.4s, v16.4s")                // v16  = b2*f2
            __ASM_EMIT("fsub                v2.4s, v18.4s, v0.4s")                  // v2   = t_re = t0 - t2*f2
            __ASM_EMIT("fsub                v16.4s, v21.4s, v16.4s")                // v16  = b_re = b0 - b2*f2
            __ASM_EMIT("fmul                v0.4s, v2.4s, v16.4s")                  // v0   = t_re*b_re
            __ASM_EMIT("fmul                v1.4s, v3.4s, v16.4s")                  // v1   = t_im*b_re
            __ASM_EMIT("fmla                v0.4s, v3.4s, v6.4s")                   // v0   = t_re*b_re + t_im*b_im
            __ASM_EMIT("fmls                v1.4s, v2.4s, v6.4s")                   // v1   = t_im*b_re - t_re*b_im
            __ASM_EMIT("fmul                v4.4s, v16.4s, v16.4s")                 // v4   = b_re*b_re
            __ASM_EMIT("fmla                v4.4s, v6.4s, v6.4s")                   // v4   = W = b_re*b_re + b_im*b_im
            __ASM_EMIT("frecpe              v6.4s, v4.4s")                          // v6   = s2
            __ASM_EMIT("frecps              v16.4s, v6.4s, v4.4s")                  // v16  = (2 - R*s2)
            __ASM_EMIT("fmul                v6.4s, v16.4s, v6.4s")                  // v6   = s2' = s2 * (2 - R*s2)
            __ASM_EMIT("frecps              v16.4s, v6.4s, v4.4s")                  // v16  = (2 - R*s2')
            __ASM_EMIT("fmul                v6.4s, v16.4s, v6.4s")                  // v6   = 1/W = s2" = s2' * (2 - R*s2')
            __ASM_EMIT("fmul                v0.4s, v0.4s, v6.4s")                   // v0   = a_re = t_re / W
            __ASM_EMIT("fmul                v1.4s, v1.4s, v6.4s")                   // v1   = a_im = t_im / W
            // Update data
            __ASM_EMIT("ldr                 q6, [%[re]]")                           // v6   = b_re
            __ASM_EMIT("ldr                 q16, [%[im]]")                          // v16  = b_im
            __ASM_EMIT("fmul                v4.4s, v0.4s, v6.4s")                   // v4   = a_re*b_re
            __ASM_EMIT("fmul                v5.4s, v1.4s, v6.4s")                   // v5   = a_im*b_re
            __ASM_EMIT("fmls                v4.4s, v1.4s, v16.4s")                  // v4   = a_re*b_re - a_im*b_im
            __ASM_EMIT("fmla                v5.4s, v0.4s, v16.4s")                  // v5   = a_im*b_re + a_re*b_im
            __ASM_EMIT("str                 q4, [%[re]]")
            __ASM_EMIT("str                 q5, [%[im]]")
            __ASM_EMIT("sub                 %[count], %[count], #4")
            __ASM_EMIT("add                 %[f], %[f], #0x10")
            __ASM_EMIT("add                 %[re], %[re], #0x10")
            __ASM_EMIT("add                 %[im], %[im], #0x10")
            __ASM_EMIT("4:")
            // x2 blocks
            __ASM_EMIT("adds                %[count], %[count], #2")
            __ASM_EMIT("blt                 6f")
            __ASM_EMIT("1:")
            __ASM_EMIT("ldr                 d6, [%[f]]")                            // v6   = f
            __ASM_EMIT("fmul                v16.4s, v6.4s, v6.4s")                  // v16  = f2 = f*f
            __ASM_EMIT("fmul                v3.4s, v19.4s, v6.4s")                  // v3   = t_im = t1*f
            __ASM_EMIT("fmul                v6.4s, v22.4s, v6.4s")                  // v6   = b_im = b1*f
            __ASM_EMIT("fmul                v0.4s, v20.4s, v16.4s")                 // v0   = t2*f2
            __ASM_EMIT("fmul                v16.4s, v23.4s, v16.4s")                // v16  = b2*f2
            __ASM_EMIT("fsub                v2.4s, v18.4s, v0.4s")                  // v2   = t_re = t0 - t2*f2
            __ASM_EMIT("fsub                v16.4s, v21.4s, v16.4s")                // v16  = b_re = b0 - b2*f2
            __ASM_EMIT("fmul                v0.4s, v2.4s, v16.4s")                  // v0   = t_re*b_re
            __ASM_EMIT("fmul                v1.4s, v3.4s, v16.4s")                  // v1   = t_im*b_re
            __ASM_EMIT("fmla                v0.4s, v3.4s, v6.4s")                   // v0   = t_re*b_re + t_im*b_im
            __ASM_EMIT("fmls                v1.4s, v2.4s, v6.4s")                   // v1   = t_im*b_re - t_re*b_im
            __ASM_EMIT("fmul                v4.4s, v16.4s, v16.4s")                 // v4   = b_re*b_re
            __ASM_EMIT("fmla                v4.4s, v6.4s, v6.4s")                   // v4   = W = b_re*b_re + b_im*b_im
            __ASM_EMIT("frecpe              v6.4s, v4.4s")                          // v6   = s2
            __ASM_EMIT("frecps              v16.4s, v6.4s, v4.4s")                  // v16  = (2 - R*s2)
            __ASM_EMIT("fmul                v6.4s, v16.4s, v6.4s")                  // v6   = s2' = s2 * (2 - R*s2)
            __ASM_EMIT("frecps              v16.4s, v6.4s, v4.4s")                  // v16  = (2 - R*s2')
            __ASM_EMIT("fmul                v6.4s, v16.4s, v6.4s")                  // v6   = 1/W = s2" = s2' * (2 - R*s2')
            __ASM_EMIT("fmul                v0.4s, v0.4s, v6.4s")                   // v0   = a_re = t_re / W
            __ASM_EMIT("fmul                v1.4s, v1.4s, v6.4s")                   // v1   = a_im = t_im / W
            // Update data
            __ASM_EMIT("ldr                 d6, [%[re]]")                           // v6   = b_re
            __ASM_EMIT("ldr                 d16, [%[im]]")                          // v16  = b_im
            __ASM_EMIT("fmul                v4.4s, v0.4s, v6.4s")                   // v4   = a_re*b_re
            __ASM_EMIT("fmul                v5.4s, v1.4s, v6.4s")                   // v5   = a_im*b_re
            __ASM_EMIT("fmls                v4.4s, v1.4s, v16.4s")                  // v4   = a_re*b_re - a_im*b_im
            __ASM_EMIT("fmla                v5.4s, v0.4s, v16.4s")                  // v5   = a_im*b_re + a_re*b_im
            __ASM_EMIT("str                 d4, [%[re]]")
            __ASM_EMIT("str                 d5, [%[im]]")
            __ASM_EMIT("sub                 %[count], %[count], #2")
            __ASM_EMIT("add                 %[f], %[f], #0x08")
            __ASM_EMIT("add                 %[re], %[re], #0x08")
            __ASM_EMIT("add                 %[im], %[im], #0x08")
            __ASM_EMIT("6:")
            // x1 blocks
            __ASM_EMIT("adds                %[count], %[count], #1")
            __ASM_EMIT("b.lt                8f")
            __ASM_EMIT("1:")
            __ASM_EMIT("ld1r                {v6.4s}, [%[f]]")                       // v6   = f
            __ASM_EMIT("fmul                v16.4s, v6.4s, v6.4s")                  // v16  = f2 = f*f
            __ASM_EMIT("fmul                v3.4s, v19.4s, v6.4s")                  // v3   = t_im = t1*f
            __ASM_EMIT("fmul                v6.4s, v22.4s, v6.4s")                  // v6   = b_im = b1*f
            __ASM_EMIT("fmul                v0.4s, v20.4s, v16.4s")                 // v0   = t2*f2
            __ASM_EMIT("fmul                v16.4s, v23.4s, v16.4s")                // v16  = b2*f2
            __ASM_EMIT("fsub                v2.4s, v18.4s, v0.4s")                  // v2   = t_re = t0 - t2*f2
            __ASM_EMIT("fsub                v16.4s, v21.4s, v16.4s")                // v16  = b_re = b0 - b2*f2
            __ASM_EMIT("fmul                v0.4s, v2.4s, v16.4s")                  // v0   = t_re*b_re
            __ASM_EMIT("fmul                v1.4s, v3.4s, v16.4s")                  // v1   = t_im*b_re
            __ASM_EMIT("fmla                v0.4s, v3.4s, v6.4s")                   // v0   = t_re*b_re + t_im*b_im
            __ASM_EMIT("fmls                v1.4s, v2.4s, v6.4s")                   // v1   = t_im*b_re - t_re*b_im
            __ASM_EMIT("fmul                v4.4s, v16.4s, v16.4s")                 // v4   = b_re*b_re
            __ASM_EMIT("fmla                v4.4s, v6.4s, v6.4s")                   // v4   = W = b_re*b_re + b_im*b_im
            __ASM_EMIT("frecpe              v6.4s, v4.4s")                          // v6   = s2
            __ASM_EMIT("frecps              v16.4s, v6.4s, v4.4s")                  // v16  = (2 - R*s2)
            __ASM_EMIT("fmul                v6.4s, v16.4s, v6.4s")                  // v6   = s2' = s2 * (2 - R*s2)
            __ASM_EMIT("frecps              v16.4s, v6.4s, v4.4s")                  // v16  = (2 - R*s2')
            __ASM_EMIT("fmul                v6.4s, v16.4s, v6.4s")                  // v6   = 1/W = s2" = s2' * (2 - R*s2')
            __ASM_EMIT("fmul                v0.4s, v0.4s, v6.4s")                   // v0   = a_re = t_re / W
            __ASM_EMIT("fmul                v1.4s, v1.4s, v6.4s")                   // v1   = a_im = t_im / W
            // Update data
            __ASM_EMIT("ld1r                {v6.4s}, [%[re]]")                      // v6   = b_re
            __ASM_EMIT("ld1r                {v16.4s}, [%[im]]")                     // v16  = b_im
            __ASM_EMIT("fmul                v4.4s, v0.4s, v6.4s")                   // v4   = a_re*b_re
            __ASM_EMIT("fmul                v5.4s, v1.4s, v6.4s")                   // v5   = a_im*b_re
            __ASM_EMIT("fmls                v4.4s, v1.4s, v16.4s")                  // v4   = a_re*b_re - a_im*b_im
            __ASM_EMIT("fmla                v5.4s, v0.4s, v16.4s")                  // v5   = a_im*b_re + a_re*b_im
            __ASM_EMIT("st1                 {v4.s}[0], [%[re]]")
            __ASM_EMIT("st1                 {v5.s}[0], [%[im]]")
            __ASM_EMIT("8:")

            : [re] "+r" (re), [im] "+r" (im), [f] "+r" (freq),
              [count] "+r" (count), [c] "+r" (c)
            :
            : "cc", "memory",
              "v0", "v1", "v2", "v3",
              "v4", "v5", "v6", "v7",
              "v16", "v17", "v18", "v19",
              "v20", "v21", "v22", "v23"
        );
    }

    void filter_transfer_calc_pc(float *dst, const f_cascade_t *c, const float *freq, size_t count)
    {
        ARCH_AARCH64_ASM(
            // Unpack filter params
            __ASM_EMIT("ld3r                {v18.4s, v19.4s, v20.4s}, [%[c]]")
            __ASM_EMIT("add                 %[c], %[c], #0x10")
            __ASM_EMIT("ld3r                {v21.4s, v22.4s, v23.4s}, [%[c]]")
            // x8 blocks
            __ASM_EMIT("subs                %[count], %[count], #8")
            __ASM_EMIT("b.lo                2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("ldp                 q6, q7, [%[f]]")                        // v6   = f
            __ASM_EMIT("fmul                v16.4s, v6.4s, v6.4s")                  // v16  = f2 = f*f
            __ASM_EMIT("fmul                v17.4s, v7.4s, v7.4s")
            __ASM_EMIT("fmul                v3.4s, v19.4s, v6.4s")                  // v3   = t_im = t1*f
            __ASM_EMIT("fmul                v5.4s, v19.4s, v7.4s")
            __ASM_EMIT("fmul                v6.4s, v22.4s, v6.4s")                  // v6   = b_im = b1*f
            __ASM_EMIT("fmul                v7.4s, v22.4s, v7.4s")
            __ASM_EMIT("fmul                v0.4s, v20.4s, v16.4s")                 // v0   = t2*f2
            __ASM_EMIT("fmul                v1.4s, v20.4s, v17.4s")
            __ASM_EMIT("fmul                v16.4s, v23.4s, v16.4s")                // v16  = b2*f2
            __ASM_EMIT("fmul                v17.4s, v23.4s, v17.4s")
            __ASM_EMIT("fsub                v2.4s, v18.4s, v0.4s")                  // v2   = t_re = t0 - t2*f2
            __ASM_EMIT("fsub                v4.4s, v18.4s, v1.4s")
            __ASM_EMIT("fsub                v16.4s, v21.4s, v16.4s")                // v16  = b_re = b0 - b2*f2
            __ASM_EMIT("fsub                v17.4s, v21.4s, v17.4s")
            __ASM_EMIT("fmul                v0.4s, v2.4s, v16.4s")                  // v0   = t_re*b_re
            __ASM_EMIT("fmul                v1.4s, v3.4s, v16.4s")                  // v1   = t_im*b_re
            __ASM_EMIT("fmla                v0.4s, v3.4s, v6.4s")                   // v0   = t_re*b_re + t_im*b_im
            __ASM_EMIT("fmls                v1.4s, v2.4s, v6.4s")                   // v1   = t_im*b_re - t_re*b_im
            __ASM_EMIT("fmul                v2.4s, v4.4s, v17.4s")
            __ASM_EMIT("fmul                v3.4s, v5.4s, v17.4s")
            __ASM_EMIT("fmla                v2.4s, v5.4s, v7.4s")
            __ASM_EMIT("fmls                v3.4s, v4.4s, v7.4s")
            __ASM_EMIT("fmul                v4.4s, v16.4s, v16.4s")                 // v4   = b_re*b_re
            __ASM_EMIT("fmul                v5.4s, v17.4s, v17.4s")
            __ASM_EMIT("fmla                v4.4s, v6.4s, v6.4s")                   // v4   = W = b_re*b_re + b_im*b_im
            __ASM_EMIT("fmla                v5.4s, v7.4s, v7.4s")
            __ASM_EMIT("frecpe              v6.4s, v4.4s")                          // v6   = s2
            __ASM_EMIT("frecpe              v7.4s, v5.4s")
            __ASM_EMIT("frecps              v16.4s, v6.4s, v4.4s")                  // v16  = (2 - R*s2)
            __ASM_EMIT("frecps              v17.4s, v7.4s, v5.4s")
            __ASM_EMIT("fmul                v6.4s, v16.4s, v6.4s")                  // v6   = s2' = s2 * (2 - R*s2)
            __ASM_EMIT("fmul                v7.4s, v17.4s, v7.4s")
            __ASM_EMIT("frecps              v16.4s, v6.4s, v4.4s")                  // v16  = (2 - R*s2')
            __ASM_EMIT("frecps              v17.4s, v7.4s, v5.4s")
            __ASM_EMIT("fmul                v6.4s, v16.4s, v6.4s")                  // v6   = 1/W = s2" = s2' * (2 - R*s2')
            __ASM_EMIT("fmul                v7.4s, v17.4s, v7.4s")
            __ASM_EMIT("fmul                v0.4s, v0.4s, v6.4s")                   // v0   = a_re = t_re / W
            __ASM_EMIT("fmul                v2.4s, v2.4s, v7.4s")
            __ASM_EMIT("fmul                v1.4s, v1.4s, v6.4s")                   // v1   = a_im = t_im / W
            __ASM_EMIT("fmul                v3.4s, v3.4s, v7.4s")
            // Store data
            __ASM_EMIT("st2                 {v0.4s, v1.4s}, [%[dst]]")
            __ASM_EMIT("add                 %[dst], %[dst], #0x20")
            __ASM_EMIT("st2                 {v2.4s, v3.4s}, [%[dst]]")
            __ASM_EMIT("add                 %[dst], %[dst], #0x20")
            __ASM_EMIT("subs                %[count], %[count], #8")
            __ASM_EMIT("add                 %[f], %[f], #0x20")
            __ASM_EMIT("b.hs                1b")
            __ASM_EMIT("2:")
            // x4 blocks
            __ASM_EMIT("adds                %[count], %[count], #4")
            __ASM_EMIT("b.lt                4f")
            __ASM_EMIT("1:")
            __ASM_EMIT("ldr                 q6, [%[f]]")                            // v6   = f
            __ASM_EMIT("fmul                v16.4s, v6.4s, v6.4s")                  // v16  = f2 = f*f
            __ASM_EMIT("fmul                v3.4s, v19.4s, v6.4s")                  // v3   = t_im = t1*f
            __ASM_EMIT("fmul                v6.4s, v22.4s, v6.4s")                  // v6   = b_im = b1*f
            __ASM_EMIT("fmul                v0.4s, v20.4s, v16.4s")                 // v0   = t2*f2
            __ASM_EMIT("fmul                v16.4s, v23.4s, v16.4s")                // v16  = b2*f2
            __ASM_EMIT("fsub                v2.4s, v18.4s, v0.4s")                  // v2   = t_re = t0 - t2*f2
            __ASM_EMIT("fsub                v16.4s, v21.4s, v16.4s")                // v16  = b_re = b0 - b2*f2
            __ASM_EMIT("fmul                v0.4s, v2.4s, v16.4s")                  // v0   = t_re*b_re
            __ASM_EMIT("fmul                v1.4s, v3.4s, v16.4s")                  // v1   = t_im*b_re
            __ASM_EMIT("fmla                v0.4s, v3.4s, v6.4s")                   // v0   = t_re*b_re + t_im*b_im
            __ASM_EMIT("fmls                v1.4s, v2.4s, v6.4s")                   // v1   = t_im*b_re - t_re*b_im
            __ASM_EMIT("fmul                v4.4s, v16.4s, v16.4s")                 // v4   = b_re*b_re
            __ASM_EMIT("fmla                v4.4s, v6.4s, v6.4s")                   // v4   = W = b_re*b_re + b_im*b_im
            __ASM_EMIT("frecpe              v6.4s, v4.4s")                          // v6   = s2
            __ASM_EMIT("frecps              v16.4s, v6.4s, v4.4s")                  // v16  = (2 - R*s2)
            __ASM_EMIT("fmul                v6.4s, v16.4s, v6.4s")                  // v6   = s2' = s2 * (2 - R*s2)
            __ASM_EMIT("frecps              v16.4s, v6.4s, v4.4s")                  // v16  = (2 - R*s2')
            __ASM_EMIT("fmul                v6.4s, v16.4s, v6.4s")                  // v6   = 1/W = s2" = s2' * (2 - R*s2')
            __ASM_EMIT("fmul                v0.4s, v0.4s, v6.4s")                   // v0   = a_re = t_re / W
            __ASM_EMIT("fmul                v1.4s, v1.4s, v6.4s")                   // v1   = a_im = t_im / W
            // Store data
            __ASM_EMIT("st2                 {v0.4s, v1.4s}, [%[dst]]")
            __ASM_EMIT("add                 %[dst], %[dst], #0x20")
            __ASM_EMIT("sub                 %[count], %[count], #4")
            __ASM_EMIT("add                 %[f], %[f], #0x10")
            __ASM_EMIT("4:")
            // x2 blocks
            __ASM_EMIT("adds                %[count], %[count], #2")
            __ASM_EMIT("blt                 6f")
            __ASM_EMIT("1:")
            __ASM_EMIT("ldr                 d6, [%[f]]")                            // v6   = f
            __ASM_EMIT("fmul                v16.4s, v6.4s, v6.4s")                  // v16  = f2 = f*f
            __ASM_EMIT("fmul                v3.4s, v19.4s, v6.4s")                  // v3   = t_im = t1*f
            __ASM_EMIT("fmul                v6.4s, v22.4s, v6.4s")                  // v6   = b_im = b1*f
            __ASM_EMIT("fmul                v0.4s, v20.4s, v16.4s")                 // v0   = t2*f2
            __ASM_EMIT("fmul                v16.4s, v23.4s, v16.4s")                // v16  = b2*f2
            __ASM_EMIT("fsub                v2.4s, v18.4s, v0.4s")                  // v2   = t_re = t0 - t2*f2
            __ASM_EMIT("fsub                v16.4s, v21.4s, v16.4s")                // v16  = b_re = b0 - b2*f2
            __ASM_EMIT("fmul                v0.4s, v2.4s, v16.4s")                  // v0   = t_re*b_re
            __ASM_EMIT("fmul                v1.4s, v3.4s, v16.4s")                  // v1   = t_im*b_re
            __ASM_EMIT("fmla                v0.4s, v3.4s, v6.4s")                   // v0   = t_re*b_re + t_im*b_im
            __ASM_EMIT("fmls                v1.4s, v2.4s, v6.4s")                   // v1   = t_im*b_re - t_re*b_im
            __ASM_EMIT("fmul                v4.4s, v16.4s, v16.4s")                 // v4   = b_re*b_re
            __ASM_EMIT("fmla                v4.4s, v6.4s, v6.4s")                   // v4   = W = b_re*b_re + b_im*b_im
            __ASM_EMIT("frecpe              v6.4s, v4.4s")                          // v6   = s2
            __ASM_EMIT("frecps              v16.4s, v6.4s, v4.4s")                  // v16  = (2 - R*s2)
            __ASM_EMIT("fmul                v6.4s, v16.4s, v6.4s")                  // v6   = s2' = s2 * (2 - R*s2)
            __ASM_EMIT("frecps              v16.4s, v6.4s, v4.4s")                  // v16  = (2 - R*s2')
            __ASM_EMIT("fmul                v6.4s, v16.4s, v6.4s")                  // v6   = 1/W = s2" = s2' * (2 - R*s2')
            __ASM_EMIT("fmul                v0.4s, v0.4s, v6.4s")                   // v0   = a_re = t_re / W
            __ASM_EMIT("fmul                v1.4s, v1.4s, v6.4s")                   // v1   = a_im = t_im / W
            // Update data
            __ASM_EMIT("st2                 {v0.2s, v1.2s}, [%[dst]]")
            __ASM_EMIT("add                 %[dst], %[dst], #0x10")
            __ASM_EMIT("sub                 %[count], %[count], #2")
            __ASM_EMIT("add                 %[f], %[f], #0x08")
            __ASM_EMIT("6:")
            // x1 blocks
            __ASM_EMIT("adds                %[count], %[count], #1")
            __ASM_EMIT("b.lt                8f")
            __ASM_EMIT("1:")
            __ASM_EMIT("ld1r                {v6.4s}, [%[f]]")                       // v6   = f
            __ASM_EMIT("fmul                v16.4s, v6.4s, v6.4s")                  // v16  = f2 = f*f
            __ASM_EMIT("fmul                v3.4s, v19.4s, v6.4s")                  // v3   = t_im = t1*f
            __ASM_EMIT("fmul                v6.4s, v22.4s, v6.4s")                  // v6   = b_im = b1*f
            __ASM_EMIT("fmul                v0.4s, v20.4s, v16.4s")                 // v0   = t2*f2
            __ASM_EMIT("fmul                v16.4s, v23.4s, v16.4s")                // v16  = b2*f2
            __ASM_EMIT("fsub                v2.4s, v18.4s, v0.4s")                  // v2   = t_re = t0 - t2*f2
            __ASM_EMIT("fsub                v16.4s, v21.4s, v16.4s")                // v16  = b_re = b0 - b2*f2
            __ASM_EMIT("fmul                v0.4s, v2.4s, v16.4s")                  // v0   = t_re*b_re
            __ASM_EMIT("fmul                v1.4s, v3.4s, v16.4s")                  // v1   = t_im*b_re
            __ASM_EMIT("fmla                v0.4s, v3.4s, v6.4s")                   // v0   = t_re*b_re + t_im*b_im
            __ASM_EMIT("fmls                v1.4s, v2.4s, v6.4s")                   // v1   = t_im*b_re - t_re*b_im
            __ASM_EMIT("fmul                v4.4s, v16.4s, v16.4s")                 // v4   = b_re*b_re
            __ASM_EMIT("fmla                v4.4s, v6.4s, v6.4s")                   // v4   = W = b_re*b_re + b_im*b_im
            __ASM_EMIT("frecpe              v6.4s, v4.4s")                          // v6   = s2
            __ASM_EMIT("frecps              v16.4s, v6.4s, v4.4s")                  // v16  = (2 - R*s2)
            __ASM_EMIT("fmul                v6.4s, v16.4s, v6.4s")                  // v6   = s2' = s2 * (2 - R*s2)
            __ASM_EMIT("frecps              v16.4s, v6.4s, v4.4s")                  // v16  = (2 - R*s2')
            __ASM_EMIT("fmul                v6.4s, v16.4s, v6.4s")                  // v6   = 1/W = s2" = s2' * (2 - R*s2')
            __ASM_EMIT("fmul                v0.4s, v0.4s, v6.4s")                   // v0   = a_re = t_re / W
            __ASM_EMIT("fmul                v1.4s, v1.4s, v6.4s")                   // v1   = a_im = t_im / W
            // Update data
            __ASM_EMIT("st2                 {v0.s, v1.s}[0], [%[dst]]")
            __ASM_EMIT("8:")

            : [dst] "+r" (dst), [f] "+r" (freq),
              [count] "+r" (count), [c] "+r" (c)
            :
            : "cc", "memory",
              "v0", "v1", "v2", "v3",
              "v4", "v5", "v6", "v7",
              "v16", "v17", "v18", "v19",
              "v20", "v21", "v22", "v23"
        );
    }

    void filter_transfer_apply_pc(float *dst, const f_cascade_t *c, const float *freq, size_t count)
    {
        ARCH_AARCH64_ASM(
            // Unpack filter params
            __ASM_EMIT("ld3r                {v18.4s, v19.4s, v20.4s}, [%[c]]")
            __ASM_EMIT("add                 %[c], %[c], #0x10")
            __ASM_EMIT("ld3r                {v21.4s, v22.4s, v23.4s}, [%[c]]")
            // x8 blocks
            __ASM_EMIT("subs                %[count], %[count], #8")
            __ASM_EMIT("b.lo                2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("ldp                 q6, q7, [%[f]]")                        // v6   = f
            __ASM_EMIT("fmul                v16.4s, v6.4s, v6.4s")                  // v16  = f2 = f*f
            __ASM_EMIT("fmul                v17.4s, v7.4s, v7.4s")
            __ASM_EMIT("fmul                v3.4s, v19.4s, v6.4s")                  // v3   = t_im = t1*f
            __ASM_EMIT("fmul                v5.4s, v19.4s, v7.4s")
            __ASM_EMIT("fmul                v6.4s, v22.4s, v6.4s")                  // v6   = b_im = b1*f
            __ASM_EMIT("fmul                v7.4s, v22.4s, v7.4s")
            __ASM_EMIT("fmul                v0.4s, v20.4s, v16.4s")                 // v0   = t2*f2
            __ASM_EMIT("fmul                v1.4s, v20.4s, v17.4s")
            __ASM_EMIT("fmul                v16.4s, v23.4s, v16.4s")                // v16  = b2*f2
            __ASM_EMIT("fmul                v17.4s, v23.4s, v17.4s")
            __ASM_EMIT("fsub                v2.4s, v18.4s, v0.4s")                  // v2   = t_re = t0 - t2*f2
            __ASM_EMIT("fsub                v4.4s, v18.4s, v1.4s")
            __ASM_EMIT("fsub                v16.4s, v21.4s, v16.4s")                // v16  = b_re = b0 - b2*f2
            __ASM_EMIT("fsub                v17.4s, v21.4s, v17.4s")
            __ASM_EMIT("fmul                v0.4s, v2.4s, v16.4s")                  // v0   = t_re*b_re
            __ASM_EMIT("fmul                v1.4s, v3.4s, v16.4s")                  // v1   = t_im*b_re
            __ASM_EMIT("fmla                v0.4s, v3.4s, v6.4s")                   // v0   = t_re*b_re + t_im*b_im
            __ASM_EMIT("fmls                v1.4s, v2.4s, v6.4s")                   // v1   = t_im*b_re - t_re*b_im
            __ASM_EMIT("fmul                v2.4s, v4.4s, v17.4s")
            __ASM_EMIT("fmul                v3.4s, v5.4s, v17.4s")
            __ASM_EMIT("fmla                v2.4s, v5.4s, v7.4s")
            __ASM_EMIT("fmls                v3.4s, v4.4s, v7.4s")
            __ASM_EMIT("fmul                v4.4s, v16.4s, v16.4s")                 // v4   = b_re*b_re
            __ASM_EMIT("fmul                v5.4s, v17.4s, v17.4s")
            __ASM_EMIT("fmla                v4.4s, v6.4s, v6.4s")                   // v4   = W = b_re*b_re + b_im*b_im
            __ASM_EMIT("fmla                v5.4s, v7.4s, v7.4s")
            __ASM_EMIT("frecpe              v6.4s, v4.4s")                          // v6   = s2
            __ASM_EMIT("frecpe              v7.4s, v5.4s")
            __ASM_EMIT("frecps              v16.4s, v6.4s, v4.4s")                  // v16  = (2 - R*s2)
            __ASM_EMIT("frecps              v17.4s, v7.4s, v5.4s")
            __ASM_EMIT("fmul                v6.4s, v16.4s, v6.4s")                  // v6   = s2' = s2 * (2 - R*s2)
            __ASM_EMIT("fmul                v7.4s, v17.4s, v7.4s")
            __ASM_EMIT("frecps              v16.4s, v6.4s, v4.4s")                  // v16  = (2 - R*s2')
            __ASM_EMIT("frecps              v17.4s, v7.4s, v5.4s")
            __ASM_EMIT("fmul                v6.4s, v16.4s, v6.4s")                  // v6   = 1/W = s2" = s2' * (2 - R*s2')
            __ASM_EMIT("fmul                v7.4s, v17.4s, v7.4s")
            __ASM_EMIT("fmul                v0.4s, v0.4s, v6.4s")                   // v0   = a_re = t_re / W
            __ASM_EMIT("fmul                v2.4s, v2.4s, v7.4s")
            __ASM_EMIT("fmul                v1.4s, v1.4s, v6.4s")                   // v1   = a_im = t_im / W
            __ASM_EMIT("fmul                v3.4s, v3.4s, v7.4s")
            // Update data
            __ASM_EMIT("ld2                 {v6.4s, v7.4s}, [%[dst]]")              // v6   = r0 r1 r2 r3, v7 = i0 i1 i2 i3
            __ASM_EMIT("fmul                v4.4s, v0.4s, v6.4s")                   // v4   = a_re*b_re
            __ASM_EMIT("fmul                v5.4s, v1.4s, v6.4s")                   // v5   = a_im*b_re
            __ASM_EMIT("fmls                v4.4s, v1.4s, v7.4s")                   // v4   = a_re*b_re - a_im*b_im
            __ASM_EMIT("fmla                v5.4s, v0.4s, v7.4s")                   // v5   = a_im*b_re + a_re*b_im
            __ASM_EMIT("st2                 {v4.4s, v5.4s}, [%[dst]]")
            __ASM_EMIT("add                 %[dst], %[dst], #0x20")
            __ASM_EMIT("ld2                 {v6.4s, v7.4s}, [%[dst]]")              // v6   = r4 r5 r6 r7, v7 = i4 i5 i6 i7
            __ASM_EMIT("fmul                v4.4s, v2.4s, v6.4s")
            __ASM_EMIT("fmul                v5.4s, v3.4s, v6.4s")
            __ASM_EMIT("fmls                v4.4s, v3.4s, v7.4s")
            __ASM_EMIT("fmla                v5.4s, v2.4s, v7.4s")
            __ASM_EMIT("st2                 {v4.4s, v5.4s}, [%[dst]]")
            __ASM_EMIT("add                 %[dst], %[dst], #0x20")
            __ASM_EMIT("subs                %[count], %[count], #8")
            __ASM_EMIT("add                 %[f], %[f], #0x20")
            __ASM_EMIT("b.hs                1b")
            __ASM_EMIT("2:")
            // x4 blocks
            __ASM_EMIT("adds                %[count], %[count], #4")
            __ASM_EMIT("b.lt                4f")
            __ASM_EMIT("1:")
            __ASM_EMIT("ldr                 q6, [%[f]]")                            // v6   = f
            __ASM_EMIT("fmul                v16.4s, v6.4s, v6.4s")                  // v16  = f2 = f*f
            __ASM_EMIT("fmul                v3.4s, v19.4s, v6.4s")                  // v3   = t_im = t1*f
            __ASM_EMIT("fmul                v6.4s, v22.4s, v6.4s")                  // v6   = b_im = b1*f
            __ASM_EMIT("fmul                v0.4s, v20.4s, v16.4s")                 // v0   = t2*f2
            __ASM_EMIT("fmul                v16.4s, v23.4s, v16.4s")                // v16  = b2*f2
            __ASM_EMIT("fsub                v2.4s, v18.4s, v0.4s")                  // v2   = t_re = t0 - t2*f2
            __ASM_EMIT("fsub                v16.4s, v21.4s, v16.4s")                // v16  = b_re = b0 - b2*f2
            __ASM_EMIT("fmul                v0.4s, v2.4s, v16.4s")                  // v0   = t_re*b_re
            __ASM_EMIT("fmul                v1.4s, v3.4s, v16.4s")                  // v1   = t_im*b_re
            __ASM_EMIT("fmla                v0.4s, v3.4s, v6.4s")                   // v0   = t_re*b_re + t_im*b_im
            __ASM_EMIT("fmls                v1.4s, v2.4s, v6.4s")                   // v1   = t_im*b_re - t_re*b_im
            __ASM_EMIT("fmul                v4.4s, v16.4s, v16.4s")                 // v4   = b_re*b_re
            __ASM_EMIT("fmla                v4.4s, v6.4s, v6.4s")                   // v4   = W = b_re*b_re + b_im*b_im
            __ASM_EMIT("frecpe              v6.4s, v4.4s")                          // v6   = s2
            __ASM_EMIT("frecps              v16.4s, v6.4s, v4.4s")                  // v16  = (2 - R*s2)
            __ASM_EMIT("fmul                v6.4s, v16.4s, v6.4s")                  // v6   = s2' = s2 * (2 - R*s2)
            __ASM_EMIT("frecps              v16.4s, v6.4s, v4.4s")                  // v16  = (2 - R*s2')
            __ASM_EMIT("fmul                v6.4s, v16.4s, v6.4s")                  // v6   = 1/W = s2" = s2' * (2 - R*s2')
            __ASM_EMIT("fmul                v0.4s, v0.4s, v6.4s")                   // v0   = a_re = t_re / W
            __ASM_EMIT("fmul                v1.4s, v1.4s, v6.4s")                   // v1   = a_im = t_im / W
            // Update data
            __ASM_EMIT("ld2                 {v6.4s, v7.4s}, [%[dst]]")              // v6   = r0 r1 r2 r3, v7 = i0 i1 i2 i3
            __ASM_EMIT("fmul                v4.4s, v0.4s, v6.4s")                   // v4   = a_re*b_re
            __ASM_EMIT("fmul                v5.4s, v1.4s, v6.4s")                   // v5   = a_im*b_re
            __ASM_EMIT("fmls                v4.4s, v1.4s, v7.4s")                   // v4   = a_re*b_re - a_im*b_im
            __ASM_EMIT("fmla                v5.4s, v0.4s, v7.4s")                   // v5   = a_im*b_re + a_re*b_im
            __ASM_EMIT("st2                 {v4.4s, v5.4s}, [%[dst]]")
            __ASM_EMIT("add                 %[dst], %[dst], #0x20")
            __ASM_EMIT("sub                 %[count], %[count], #4")
            __ASM_EMIT("add                 %[f], %[f], #0x10")
            __ASM_EMIT("4:")
            // x2 blocks
            __ASM_EMIT("adds                %[count], %[count], #2")
            __ASM_EMIT("blt                 6f")
            __ASM_EMIT("1:")
            __ASM_EMIT("ldr                 d6, [%[f]]")                            // v6   = f
            __ASM_EMIT("fmul                v16.4s, v6.4s, v6.4s")                  // v16  = f2 = f*f
            __ASM_EMIT("fmul                v3.4s, v19.4s, v6.4s")                  // v3   = t_im = t1*f
            __ASM_EMIT("fmul                v6.4s, v22.4s, v6.4s")                  // v6   = b_im = b1*f
            __ASM_EMIT("fmul                v0.4s, v20.4s, v16.4s")                 // v0   = t2*f2
            __ASM_EMIT("fmul                v16.4s, v23.4s, v16.4s")                // v16  = b2*f2
            __ASM_EMIT("fsub                v2.4s, v18.4s, v0.4s")                  // v2   = t_re = t0 - t2*f2
            __ASM_EMIT("fsub                v16.4s, v21.4s, v16.4s")                // v16  = b_re = b0 - b2*f2
            __ASM_EMIT("fmul                v0.4s, v2.4s, v16.4s")                  // v0   = t_re*b_re
            __ASM_EMIT("fmul                v1.4s, v3.4s, v16.4s")                  // v1   = t_im*b_re
            __ASM_EMIT("fmla                v0.4s, v3.4s, v6.4s")                   // v0   = t_re*b_re + t_im*b_im
            __ASM_EMIT("fmls                v1.4s, v2.4s, v6.4s")                   // v1   = t_im*b_re - t_re*b_im
            __ASM_EMIT("fmul                v4.4s, v16.4s, v16.4s")                 // v4   = b_re*b_re
            __ASM_EMIT("fmla                v4.4s, v6.4s, v6.4s")                   // v4   = W = b_re*b_re + b_im*b_im
            __ASM_EMIT("frecpe              v6.4s, v4.4s")                          // v6   = s2
            __ASM_EMIT("frecps              v16.4s, v6.4s, v4.4s")                  // v16  = (2 - R*s2)
            __ASM_EMIT("fmul                v6.4s, v16.4s, v6.4s")                  // v6   = s2' = s2 * (2 - R*s2)
            __ASM_EMIT("frecps              v16.4s, v6.4s, v4.4s")                  // v16  = (2 - R*s2')
            __ASM_EMIT("fmul                v6.4s, v16.4s, v6.4s")                  // v6   = 1/W = s2" = s2' * (2 - R*s2')
            __ASM_EMIT("fmul                v0.4s, v0.4s, v6.4s")                   // v0   = a_re = t_re / W
            __ASM_EMIT("fmul                v1.4s, v1.4s, v6.4s")                   // v1   = a_im = t_im / W
            // Update data
            __ASM_EMIT("ld2                 {v6.2s, v7.2s}, [%[dst]]")              // v6   = r0 r1 r2 r3, v7 = i0 i1 i2 i3
            __ASM_EMIT("fmul                v4.4s, v0.4s, v6.4s")                   // v4   = a_re*b_re
            __ASM_EMIT("fmul                v5.4s, v1.4s, v6.4s")                   // v5   = a_im*b_re
            __ASM_EMIT("fmls                v4.4s, v1.4s, v7.4s")                   // v4   = a_re*b_re - a_im*b_im
            __ASM_EMIT("fmla                v5.4s, v0.4s, v7.4s")                   // v5   = a_im*b_re + a_re*b_im
            __ASM_EMIT("st2                 {v4.2s, v5.2s}, [%[dst]]")
            __ASM_EMIT("add                 %[dst], %[dst], #0x10")
            __ASM_EMIT("sub                 %[count], %[count], #2")
            __ASM_EMIT("add                 %[f], %[f], #0x08")
            __ASM_EMIT("6:")
            // x1 blocks
            __ASM_EMIT("adds                %[count], %[count], #1")
            __ASM_EMIT("b.lt                8f")
            __ASM_EMIT("1:")
            __ASM_EMIT("ld1r                {v6.4s}, [%[f]]")                       // v6   = f
            __ASM_EMIT("fmul                v16.4s, v6.4s, v6.4s")                  // v16  = f2 = f*f
            __ASM_EMIT("fmul                v3.4s, v19.4s, v6.4s")                  // v3   = t_im = t1*f
            __ASM_EMIT("fmul                v6.4s, v22.4s, v6.4s")                  // v6   = b_im = b1*f
            __ASM_EMIT("fmul                v0.4s, v20.4s, v16.4s")                 // v0   = t2*f2
            __ASM_EMIT("fmul                v16.4s, v23.4s, v16.4s")                // v16  = b2*f2
            __ASM_EMIT("fsub                v2.4s, v18.4s, v0.4s")                  // v2   = t_re = t0 - t2*f2
            __ASM_EMIT("fsub                v16.4s, v21.4s, v16.4s")                // v16  = b_re = b0 - b2*f2
            __ASM_EMIT("fmul                v0.4s, v2.4s, v16.4s")                  // v0   = t_re*b_re
            __ASM_EMIT("fmul                v1.4s, v3.4s, v16.4s")                  // v1   = t_im*b_re
            __ASM_EMIT("fmla                v0.4s, v3.4s, v6.4s")                   // v0   = t_re*b_re + t_im*b_im
            __ASM_EMIT("fmls                v1.4s, v2.4s, v6.4s")                   // v1   = t_im*b_re - t_re*b_im
            __ASM_EMIT("fmul                v4.4s, v16.4s, v16.4s")                 // v4   = b_re*b_re
            __ASM_EMIT("fmla                v4.4s, v6.4s, v6.4s")                   // v4   = W = b_re*b_re + b_im*b_im
            __ASM_EMIT("frecpe              v6.4s, v4.4s")                          // v6   = s2
            __ASM_EMIT("frecps              v16.4s, v6.4s, v4.4s")                  // v16  = (2 - R*s2)
            __ASM_EMIT("fmul                v6.4s, v16.4s, v6.4s")                  // v6   = s2' = s2 * (2 - R*s2)
            __ASM_EMIT("frecps              v16.4s, v6.4s, v4.4s")                  // v16  = (2 - R*s2')
            __ASM_EMIT("fmul                v6.4s, v16.4s, v6.4s")                  // v6   = 1/W = s2" = s2' * (2 - R*s2')
            __ASM_EMIT("fmul                v0.4s, v0.4s, v6.4s")                   // v0   = a_re = t_re / W
            __ASM_EMIT("fmul                v1.4s, v1.4s, v6.4s")                   // v1   = a_im = t_im / W
            // Update data
            __ASM_EMIT("ld2r                {v6.4s, v7.4s}, [%[dst]]")              // v6   = r0, v7 =i0
            __ASM_EMIT("fmul                v4.4s, v0.4s, v6.4s")                   // v4   = a_re*b_re
            __ASM_EMIT("fmul                v5.4s, v1.4s, v6.4s")                   // v5   = a_im*b_re
            __ASM_EMIT("fmls                v4.4s, v1.4s, v7.4s")                   // v4   = a_re*b_re - a_im*b_im
            __ASM_EMIT("fmla                v5.4s, v0.4s, v7.4s")                   // v5   = a_im*b_re + a_re*b_im
            __ASM_EMIT("st2                 {v4.s, v5.s}[0], [%[dst]]")
            __ASM_EMIT("8:")

            : [dst] "+r" (dst), [f] "+r" (freq),
              [count] "+r" (count), [c] "+r" (c)
            :
            : "cc", "memory",
              "v0", "v1", "v2", "v3",
              "v4", "v5", "v6", "v7",
              "v16", "v17", "v18", "v19",
              "v20", "v21", "v22", "v23"
        );
    }
}

#endif /* DSP_ARCH_AARCH64_ASIMD_FILTERS_TRANSFER_H_ */
