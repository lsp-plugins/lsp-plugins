/*
 * transfer.h
 *
 *  Created on: 3 янв. 2020 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_ARM_NEON_D32_FILTERS_TRANSFER_H_
#define DSP_ARCH_ARM_NEON_D32_FILTERS_TRANSFER_H_

#ifndef DSP_ARCH_ARM_NEON_32_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_ARM_NEON_32_IMPL */

namespace neon_d32
{
    void filter_transfer_calc_ri(float *re, float *im, const f_cascade_t *c, const float *freq, size_t count)
    {
        ARCH_ARM_ASM(
            // Unpack filter params
            __ASM_EMIT("vld3.f32            {d20[], d22[], d24[]}, [%[c]]")
            __ASM_EMIT("vld3.f32            {d21[], d23[], d25[]}, [%[c]]")
            __ASM_EMIT("add                 %[c], $0x10")
            __ASM_EMIT("vld3.f32            {d26[], d28[], d30[]}, [%[c]]")
            __ASM_EMIT("vld3.f32            {d27[], d29[], d31[]}, [%[c]]")
            // x8 blocks
            __ASM_EMIT("subs                %[count], $8")
            __ASM_EMIT("blo                 2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vldm                %[f]!, {q6-q7}")                    // q6   = f
            __ASM_EMIT("vmul.f32            q8, q6, q6")                        // q8   = f2 = f*f
            __ASM_EMIT("vmul.f32            q9, q7, q7")
            __ASM_EMIT("vmul.f32            q3, q11, q6")                       // q3   = t_im = t1*f
            __ASM_EMIT("vmul.f32            q5, q11, q7")
            __ASM_EMIT("vmul.f32            q6, q14, q6")                       // q6   = b_im = b1*f
            __ASM_EMIT("vmul.f32            q7, q14, q7")
            __ASM_EMIT("vmul.f32            q0, q12, q8")                       // q0   = t2*f2
            __ASM_EMIT("vmul.f32            q1, q12, q9")
            __ASM_EMIT("vmul.f32            q8, q15, q8")                       // q8   = b2*f2
            __ASM_EMIT("vmul.f32            q9, q15, q9")
            __ASM_EMIT("vsub.f32            q2, q10, q0")                       // q2   = t_re = t0 - t2*f2
            __ASM_EMIT("vsub.f32            q4, q10, q1")
            __ASM_EMIT("vsub.f32            q8, q13, q8")                       // q8   = b_re = b0 - b2*f2
            __ASM_EMIT("vsub.f32            q9, q13, q9")
            __ASM_EMIT("vmul.f32            q0, q2, q8")                        // q0   = t_re*b_re
            __ASM_EMIT("vmul.f32            q1, q3, q8")                        // q1   = t_im*b_re
            __ASM_EMIT("vmla.f32            q0, q3, q6")                        // q0   = t_re*b_re + t_im*b_im
            __ASM_EMIT("vmls.f32            q1, q2, q6")                        // q1   = t_im*b_re - t_re*b_im
            __ASM_EMIT("vmul.f32            q2, q4, q9")
            __ASM_EMIT("vmul.f32            q3, q5, q9")
            __ASM_EMIT("vmla.f32            q2, q5, q7")
            __ASM_EMIT("vmls.f32            q3, q4, q7")
            __ASM_EMIT("vmul.f32            q4, q8, q8")                        // q4   = b_re*b_re
            __ASM_EMIT("vmul.f32            q5, q9, q9")
            __ASM_EMIT("vmla.f32            q4, q6, q6")                        // q4   = W = b_re*b_re + b_im*b_im
            __ASM_EMIT("vmla.f32            q5, q7, q7")
            __ASM_EMIT("vrecpe.f32          q6, q4")                            // q6   = s2
            __ASM_EMIT("vrecpe.f32          q7, q5")
            __ASM_EMIT("vrecps.f32          q8, q6, q4")                        // q8   = (2 - R*s2)
            __ASM_EMIT("vrecps.f32          q9, q7, q5")
            __ASM_EMIT("vmul.f32            q6, q8, q6")                        // q6   = s2' = s2 * (2 - R*s2)
            __ASM_EMIT("vmul.f32            q7, q9, q7")
            __ASM_EMIT("vrecps.f32          q8, q6, q4")                        // q8   = (2 - R*s2')
            __ASM_EMIT("vrecps.f32          q9, q7, q5")
            __ASM_EMIT("vmul.f32            q6, q8, q6")                        // q6   = 1/W = s2" = s2' * (2 - R*s2')
            __ASM_EMIT("vmul.f32            q7, q9, q7")
            __ASM_EMIT("vmul.f32            q0, q0, q6")                        // q0   = a_re = t_re / W
            __ASM_EMIT("vmul.f32            q2, q2, q7")
            __ASM_EMIT("vmul.f32            q1, q1, q6")                        // q1   = a_im = t_im / W
            __ASM_EMIT("vmul.f32            q3, q3, q7")
            // Store data
            __ASM_EMIT("vswp                q1, q2")
            __ASM_EMIT("vstm                %[re]!, {q0-q1}")
            __ASM_EMIT("vstm                %[im]!, {q2-q3}")
            __ASM_EMIT("subs                %[count], $8")
            __ASM_EMIT("bhs                 1b")
            __ASM_EMIT("2:")
            // x4 blocks
            __ASM_EMIT("adds                %[count], $4")
            __ASM_EMIT("blt                 4f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vldm                %[f]!, {q6}")                       // q6   = f
            __ASM_EMIT("vmul.f32            q8, q6, q6")                        // q8   = f2 = f*f
            __ASM_EMIT("vmul.f32            q3, q11, q6")                       // q3   = t_im = t1*f
            __ASM_EMIT("vmul.f32            q6, q14, q6")                       // q6   = b_im = b1*f
            __ASM_EMIT("vmul.f32            q0, q12, q8")                       // q0   = t2*f2
            __ASM_EMIT("vmul.f32            q8, q15, q8")                       // q8   = b2*f2
            __ASM_EMIT("vsub.f32            q2, q10, q0")                       // q2   = t_re = t0 - t2*f2
            __ASM_EMIT("vsub.f32            q8, q13, q8")                       // q8   = b_re = b0 - b2*f2
            __ASM_EMIT("vmul.f32            q0, q2, q8")                        // q0   = t_re*b_re
            __ASM_EMIT("vmul.f32            q1, q3, q8")                        // q1   = t_im*b_re
            __ASM_EMIT("vmla.f32            q0, q3, q6")                        // q0   = t_re*b_re + t_im*b_im
            __ASM_EMIT("vmls.f32            q1, q2, q6")                        // q1   = t_im*b_re - t_re*b_im
            __ASM_EMIT("vmul.f32            q4, q8, q8")                        // q4   = b_re*b_re
            __ASM_EMIT("vmla.f32            q4, q6, q6")                        // q4   = W = b_re*b_re + b_im*b_im
            __ASM_EMIT("vrecpe.f32          q6, q4")                            // q6   = s2
            __ASM_EMIT("vrecps.f32          q8, q6, q4")                        // q8   = (2 - R*s2)
            __ASM_EMIT("vmul.f32            q6, q8, q6")                        // q6   = s2' = s2 * (2 - R*s2)
            __ASM_EMIT("vrecps.f32          q8, q6, q4")                        // q8   = (2 - R*s2')
            __ASM_EMIT("vmul.f32            q6, q8, q6")                        // q6   = 1/W = s2" = s2' * (2 - R*s2')
            __ASM_EMIT("vmul.f32            q0, q0, q6")                        // q0   = a_re = t_re / W
            __ASM_EMIT("vmul.f32            q1, q1, q6")                        // q1   = a_im = t_im / W
            // Store data
            __ASM_EMIT("vstm                %[re]!, {q0}")
            __ASM_EMIT("vstm                %[im]!, {q1}")
            __ASM_EMIT("sub                 %[count], $4")
            __ASM_EMIT("4:")
            // x2 blocks
            __ASM_EMIT("adds                %[count], $2")
            __ASM_EMIT("blt                 6f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vldm                %[f]!, {d12}")                      // q6   = f
            __ASM_EMIT("vmul.f32            q8, q6, q6")                        // q8   = f2 = f*f
            __ASM_EMIT("vmul.f32            q3, q11, q6")                       // q3   = t_im = t1*f
            __ASM_EMIT("vmul.f32            q6, q14, q6")                       // q6   = b_im = b1*f
            __ASM_EMIT("vmul.f32            q0, q12, q8")                       // q0   = t2*f2
            __ASM_EMIT("vmul.f32            q8, q15, q8")                       // q8   = b2*f2
            __ASM_EMIT("vsub.f32            q2, q10, q0")                       // q2   = t_re = t0 - t2*f2
            __ASM_EMIT("vsub.f32            q8, q13, q8")                       // q8   = b_re = b0 - b2*f2
            __ASM_EMIT("vmul.f32            q0, q2, q8")                        // q0   = t_re*b_re
            __ASM_EMIT("vmul.f32            q1, q3, q8")                        // q1   = t_im*b_re
            __ASM_EMIT("vmla.f32            q0, q3, q6")                        // q0   = t_re*b_re + t_im*b_im
            __ASM_EMIT("vmls.f32            q1, q2, q6")                        // q1   = t_im*b_re - t_re*b_im
            __ASM_EMIT("vmul.f32            q4, q8, q8")                        // q4   = b_re*b_re
            __ASM_EMIT("vmla.f32            q4, q6, q6")                        // q4   = W = b_re*b_re + b_im*b_im
            __ASM_EMIT("vrecpe.f32          q6, q4")                            // q6   = s2
            __ASM_EMIT("vrecps.f32          q8, q6, q4")                        // q8   = (2 - R*s2)
            __ASM_EMIT("vmul.f32            q6, q8, q6")                        // q6   = s2' = s2 * (2 - R*s2)
            __ASM_EMIT("vrecps.f32          q8, q6, q4")                        // q8   = (2 - R*s2')
            __ASM_EMIT("vmul.f32            q6, q8, q6")                        // q6   = 1/W = s2" = s2' * (2 - R*s2')
            __ASM_EMIT("vmul.f32            q0, q0, q6")                        // q0   = a_re = t_re / W
            __ASM_EMIT("vmul.f32            q1, q1, q6")                        // q1   = a_im = t_im / W
            // Store data
            __ASM_EMIT("vstm                %[re]!, {d0}")
            __ASM_EMIT("vstm                %[im]!, {d2}")
            __ASM_EMIT("sub                 %[count], $2")
            __ASM_EMIT("6:")
            // x1 blocks
            __ASM_EMIT("adds                %[count], $1")
            __ASM_EMIT("blt                 8f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vld1.32             {d12[]}, [%[f]]")                   // q6   = f
            __ASM_EMIT("vmul.f32            q8, q6, q6")                        // q8   = f2 = f*f
            __ASM_EMIT("vmul.f32            q3, q11, q6")                       // q3   = t_im = t1*f
            __ASM_EMIT("vmul.f32            q6, q14, q6")                       // q6   = b_im = b1*f
            __ASM_EMIT("vmul.f32            q0, q12, q8")                       // q0   = t2*f2
            __ASM_EMIT("vmul.f32            q8, q15, q8")                       // q8   = b2*f2
            __ASM_EMIT("vsub.f32            q2, q10, q0")                       // q2   = t_re = t0 - t2*f2
            __ASM_EMIT("vsub.f32            q8, q13, q8")                       // q8   = b_re = b0 - b2*f2
            __ASM_EMIT("vmul.f32            q0, q2, q8")                        // q0   = t_re*b_re
            __ASM_EMIT("vmul.f32            q1, q3, q8")                        // q1   = t_im*b_re
            __ASM_EMIT("vmla.f32            q0, q3, q6")                        // q0   = t_re*b_re + t_im*b_im
            __ASM_EMIT("vmls.f32            q1, q2, q6")                        // q1   = t_im*b_re - t_re*b_im
            __ASM_EMIT("vmul.f32            q4, q8, q8")                        // q4   = b_re*b_re
            __ASM_EMIT("vmla.f32            q4, q6, q6")                        // q4   = W = b_re*b_re + b_im*b_im
            __ASM_EMIT("vrecpe.f32          q6, q4")                            // q6   = s2
            __ASM_EMIT("vrecps.f32          q8, q6, q4")                        // q8   = (2 - R*s2)
            __ASM_EMIT("vmul.f32            q6, q8, q6")                        // q6   = s2' = s2 * (2 - R*s2)
            __ASM_EMIT("vrecps.f32          q8, q6, q4")                        // q8   = (2 - R*s2')
            __ASM_EMIT("vmul.f32            q6, q8, q6")                        // q6   = 1/W = s2" = s2' * (2 - R*s2')
            __ASM_EMIT("vmul.f32            q0, q0, q6")                        // q0   = a_re = t_re / W
            __ASM_EMIT("vmul.f32            q1, q1, q6")                        // q1   = a_im = t_im / W
            // Update data
            __ASM_EMIT("vst1.32             {d0[0]}, [%[re]]")
            __ASM_EMIT("vst1.32             {d2[0]}, [%[im]]")
            __ASM_EMIT("8:")

            : [re] "+r" (re), [im] "+r" (im), [f] "+r" (freq), [count] "+r" (count), [c] "+r" (c)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3",
              "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11",
              "q12", "q13", "q14", "q15"
        );
    }

    void filter_transfer_apply_ri(float *re, float *im, const f_cascade_t *c, const float *freq, size_t count)
    {
        ARCH_ARM_ASM(
            // Unpack filter params
            __ASM_EMIT("vld3.f32            {d20[], d22[], d24[]}, [%[c]]")
            __ASM_EMIT("vld3.f32            {d21[], d23[], d25[]}, [%[c]]")
            __ASM_EMIT("add                 %[c], $0x10")
            __ASM_EMIT("vld3.f32            {d26[], d28[], d30[]}, [%[c]]")
            __ASM_EMIT("vld3.f32            {d27[], d29[], d31[]}, [%[c]]")
            // x8 blocks
            __ASM_EMIT("subs                %[count], $8")
            __ASM_EMIT("blo                 2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vldm                %[f]!, {q6-q7}")                    // q6   = f
            __ASM_EMIT("vmul.f32            q8, q6, q6")                        // q8   = f2 = f*f
            __ASM_EMIT("vmul.f32            q9, q7, q7")
            __ASM_EMIT("vmul.f32            q3, q11, q6")                       // q3   = t_im = t1*f
            __ASM_EMIT("vmul.f32            q5, q11, q7")
            __ASM_EMIT("vmul.f32            q6, q14, q6")                       // q6   = b_im = b1*f
            __ASM_EMIT("vmul.f32            q7, q14, q7")
            __ASM_EMIT("vmul.f32            q0, q12, q8")                       // q0   = t2*f2
            __ASM_EMIT("vmul.f32            q1, q12, q9")
            __ASM_EMIT("vmul.f32            q8, q15, q8")                       // q8   = b2*f2
            __ASM_EMIT("vmul.f32            q9, q15, q9")
            __ASM_EMIT("vsub.f32            q2, q10, q0")                       // q2   = t_re = t0 - t2*f2
            __ASM_EMIT("vsub.f32            q4, q10, q1")
            __ASM_EMIT("vsub.f32            q8, q13, q8")                       // q8   = b_re = b0 - b2*f2
            __ASM_EMIT("vsub.f32            q9, q13, q9")
            __ASM_EMIT("vmul.f32            q0, q2, q8")                        // q0   = t_re*b_re
            __ASM_EMIT("vmul.f32            q1, q3, q8")                        // q1   = t_im*b_re
            __ASM_EMIT("vmla.f32            q0, q3, q6")                        // q0   = t_re*b_re + t_im*b_im
            __ASM_EMIT("vmls.f32            q1, q2, q6")                        // q1   = t_im*b_re - t_re*b_im
            __ASM_EMIT("vmul.f32            q2, q4, q9")
            __ASM_EMIT("vmul.f32            q3, q5, q9")
            __ASM_EMIT("vmla.f32            q2, q5, q7")
            __ASM_EMIT("vmls.f32            q3, q4, q7")
            __ASM_EMIT("vmul.f32            q4, q8, q8")                        // q4   = b_re*b_re
            __ASM_EMIT("vmul.f32            q5, q9, q9")
            __ASM_EMIT("vmla.f32            q4, q6, q6")                        // q4   = W = b_re*b_re + b_im*b_im
            __ASM_EMIT("vmla.f32            q5, q7, q7")
            __ASM_EMIT("vrecpe.f32          q6, q4")                            // q6   = s2
            __ASM_EMIT("vrecpe.f32          q7, q5")
            __ASM_EMIT("vrecps.f32          q8, q6, q4")                        // q8   = (2 - R*s2)
            __ASM_EMIT("vrecps.f32          q9, q7, q5")
            __ASM_EMIT("vmul.f32            q6, q8, q6")                        // q6   = s2' = s2 * (2 - R*s2)
            __ASM_EMIT("vmul.f32            q7, q9, q7")
            __ASM_EMIT("vrecps.f32          q8, q6, q4")                        // q8   = (2 - R*s2')
            __ASM_EMIT("vrecps.f32          q9, q7, q5")
            __ASM_EMIT("vmul.f32            q6, q8, q6")                        // q6   = 1/W = s2" = s2' * (2 - R*s2')
            __ASM_EMIT("vmul.f32            q7, q9, q7")
            __ASM_EMIT("vmul.f32            q0, q0, q6")                        // q0   = a_re = t_re / W
            __ASM_EMIT("vmul.f32            q2, q2, q7")
            __ASM_EMIT("vmul.f32            q1, q1, q6")                        // q1   = a_im = t_im / W
            __ASM_EMIT("vmul.f32            q3, q3, q7")
            // Update data
            __ASM_EMIT("vldm                %[re], {q6-q7}")                    // q6   = b_re
            __ASM_EMIT("vldm                %[im], {q8-q9}")                    // q8   = b_im
            __ASM_EMIT("vmul.f32            q4, q0, q6")                        // q4   = a_re*b_re
            __ASM_EMIT("vmul.f32            q5, q1, q6")                        // q5   = a_im*b_re
            __ASM_EMIT("vmls.f32            q4, q1, q8")                        // q4   = a_re*b_re - a_im*b_im
            __ASM_EMIT("vmla.f32            q5, q0, q8")                        // q5   = a_im*b_re + a_re*b_im
            __ASM_EMIT("vmul.f32            q6, q2, q7")
            __ASM_EMIT("vmul.f32            q7, q3, q7")
            __ASM_EMIT("vmls.f32            q6, q3, q9")
            __ASM_EMIT("vmla.f32            q7, q2, q9")
            __ASM_EMIT("vswp                q5, q6")
            __ASM_EMIT("vstm                %[re]!, {q4-q5}")
            __ASM_EMIT("vstm                %[im]!, {q6-q7}")
            __ASM_EMIT("subs                %[count], $8")
            __ASM_EMIT("bhs                 1b")
            __ASM_EMIT("2:")
            // x4 blocks
            __ASM_EMIT("adds                %[count], $4")
            __ASM_EMIT("blt                 4f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vldm                %[f]!, {q6}")                       // q6   = f
            __ASM_EMIT("vmul.f32            q8, q6, q6")                        // q8   = f2 = f*f
            __ASM_EMIT("vmul.f32            q3, q11, q6")                       // q3   = t_im = t1*f
            __ASM_EMIT("vmul.f32            q6, q14, q6")                       // q6   = b_im = b1*f
            __ASM_EMIT("vmul.f32            q0, q12, q8")                       // q0   = t2*f2
            __ASM_EMIT("vmul.f32            q8, q15, q8")                       // q8   = b2*f2
            __ASM_EMIT("vsub.f32            q2, q10, q0")                       // q2   = t_re = t0 - t2*f2
            __ASM_EMIT("vsub.f32            q8, q13, q8")                       // q8   = b_re = b0 - b2*f2
            __ASM_EMIT("vmul.f32            q0, q2, q8")                        // q0   = t_re*b_re
            __ASM_EMIT("vmul.f32            q1, q3, q8")                        // q1   = t_im*b_re
            __ASM_EMIT("vmla.f32            q0, q3, q6")                        // q0   = t_re*b_re + t_im*b_im
            __ASM_EMIT("vmls.f32            q1, q2, q6")                        // q1   = t_im*b_re - t_re*b_im
            __ASM_EMIT("vmul.f32            q4, q8, q8")                        // q4   = b_re*b_re
            __ASM_EMIT("vmla.f32            q4, q6, q6")                        // q4   = W = b_re*b_re + b_im*b_im
            __ASM_EMIT("vrecpe.f32          q6, q4")                            // q6   = s2
            __ASM_EMIT("vrecps.f32          q8, q6, q4")                        // q8   = (2 - R*s2)
            __ASM_EMIT("vmul.f32            q6, q8, q6")                        // q6   = s2' = s2 * (2 - R*s2)
            __ASM_EMIT("vrecps.f32          q8, q6, q4")                        // q8   = (2 - R*s2')
            __ASM_EMIT("vmul.f32            q6, q8, q6")                        // q6   = 1/W = s2" = s2' * (2 - R*s2')
            __ASM_EMIT("vmul.f32            q0, q0, q6")                        // q0   = a_re = t_re / W
            __ASM_EMIT("vmul.f32            q1, q1, q6")                        // q1   = a_im = t_im / W
            // Update data
            __ASM_EMIT("vldm                %[re], {q6}")                       // q6   = b_re
            __ASM_EMIT("vldm                %[im], {q8}")                       // q8   = b_im
            __ASM_EMIT("vmul.f32            q4, q0, q6")                        // q4   = a_re*b_re
            __ASM_EMIT("vmul.f32            q5, q1, q6")                        // q5   = a_im*b_re
            __ASM_EMIT("vmls.f32            q4, q1, q8")                        // q4   = a_re*b_re - a_im*b_im
            __ASM_EMIT("vmla.f32            q5, q0, q8")                        // q5   = a_im*b_re + a_re*b_im
            __ASM_EMIT("vstm                %[re]!, {q4}")
            __ASM_EMIT("vstm                %[im]!, {q5}")
            __ASM_EMIT("sub                 %[count], $4")
            __ASM_EMIT("4:")
            // x2 blocks
            __ASM_EMIT("adds                %[count], $2")
            __ASM_EMIT("blt                 6f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vldm                %[f]!, {d12}")                      // q6   = f
            __ASM_EMIT("vmul.f32            q8, q6, q6")                        // q8   = f2 = f*f
            __ASM_EMIT("vmul.f32            q3, q11, q6")                       // q3   = t_im = t1*f
            __ASM_EMIT("vmul.f32            q6, q14, q6")                       // q6   = b_im = b1*f
            __ASM_EMIT("vmul.f32            q0, q12, q8")                       // q0   = t2*f2
            __ASM_EMIT("vmul.f32            q8, q15, q8")                       // q8   = b2*f2
            __ASM_EMIT("vsub.f32            q2, q10, q0")                       // q2   = t_re = t0 - t2*f2
            __ASM_EMIT("vsub.f32            q8, q13, q8")                       // q8   = b_re = b0 - b2*f2
            __ASM_EMIT("vmul.f32            q0, q2, q8")                        // q0   = t_re*b_re
            __ASM_EMIT("vmul.f32            q1, q3, q8")                        // q1   = t_im*b_re
            __ASM_EMIT("vmla.f32            q0, q3, q6")                        // q0   = t_re*b_re + t_im*b_im
            __ASM_EMIT("vmls.f32            q1, q2, q6")                        // q1   = t_im*b_re - t_re*b_im
            __ASM_EMIT("vmul.f32            q4, q8, q8")                        // q4   = b_re*b_re
            __ASM_EMIT("vmla.f32            q4, q6, q6")                        // q4   = W = b_re*b_re + b_im*b_im
            __ASM_EMIT("vrecpe.f32          q6, q4")                            // q6   = s2
            __ASM_EMIT("vrecps.f32          q8, q6, q4")                        // q8   = (2 - R*s2)
            __ASM_EMIT("vmul.f32            q6, q8, q6")                        // q6   = s2' = s2 * (2 - R*s2)
            __ASM_EMIT("vrecps.f32          q8, q6, q4")                        // q8   = (2 - R*s2')
            __ASM_EMIT("vmul.f32            q6, q8, q6")                        // q6   = 1/W = s2" = s2' * (2 - R*s2')
            __ASM_EMIT("vmul.f32            q0, q0, q6")                        // q0   = a_re = t_re / W
            __ASM_EMIT("vmul.f32            q1, q1, q6")                        // q1   = a_im = t_im / W
            // Update data
            __ASM_EMIT("vldm                %[re], {d12}")                      // q6   = b_re
            __ASM_EMIT("vldm                %[im], {d16}")                      // q8   = b_im
            __ASM_EMIT("vmul.f32            q4, q0, q6")                        // q4   = a_re*b_re
            __ASM_EMIT("vmul.f32            q5, q1, q6")                        // q5   = a_im*b_re
            __ASM_EMIT("vmls.f32            q4, q1, q8")                        // q4   = a_re*b_re - a_im*b_im
            __ASM_EMIT("vmla.f32            q5, q0, q8")                        // q5   = a_im*b_re + a_re*b_im
            __ASM_EMIT("vstm                %[re]!, {d8}")
            __ASM_EMIT("vstm                %[im]!, {d10}")
            __ASM_EMIT("sub                 %[count], $2")
            __ASM_EMIT("6:")
            // x1 blocks
            __ASM_EMIT("adds                %[count], $1")
            __ASM_EMIT("blt                 8f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vld1.32             {d12[]}, [%[f]]")                   // q6   = f
            __ASM_EMIT("vmul.f32            q8, q6, q6")                        // q8   = f2 = f*f
            __ASM_EMIT("vmul.f32            q3, q11, q6")                       // q3   = t_im = t1*f
            __ASM_EMIT("vmul.f32            q6, q14, q6")                       // q6   = b_im = b1*f
            __ASM_EMIT("vmul.f32            q0, q12, q8")                       // q0   = t2*f2
            __ASM_EMIT("vmul.f32            q8, q15, q8")                       // q8   = b2*f2
            __ASM_EMIT("vsub.f32            q2, q10, q0")                       // q2   = t_re = t0 - t2*f2
            __ASM_EMIT("vsub.f32            q8, q13, q8")                       // q8   = b_re = b0 - b2*f2
            __ASM_EMIT("vmul.f32            q0, q2, q8")                        // q0   = t_re*b_re
            __ASM_EMIT("vmul.f32            q1, q3, q8")                        // q1   = t_im*b_re
            __ASM_EMIT("vmla.f32            q0, q3, q6")                        // q0   = t_re*b_re + t_im*b_im
            __ASM_EMIT("vmls.f32            q1, q2, q6")                        // q1   = t_im*b_re - t_re*b_im
            __ASM_EMIT("vmul.f32            q4, q8, q8")                        // q4   = b_re*b_re
            __ASM_EMIT("vmla.f32            q4, q6, q6")                        // q4   = W = b_re*b_re + b_im*b_im
            __ASM_EMIT("vrecpe.f32          q6, q4")                            // q6   = s2
            __ASM_EMIT("vrecps.f32          q8, q6, q4")                        // q8   = (2 - R*s2)
            __ASM_EMIT("vmul.f32            q6, q8, q6")                        // q6   = s2' = s2 * (2 - R*s2)
            __ASM_EMIT("vrecps.f32          q8, q6, q4")                        // q8   = (2 - R*s2')
            __ASM_EMIT("vmul.f32            q6, q8, q6")                        // q6   = 1/W = s2" = s2' * (2 - R*s2')
            __ASM_EMIT("vmul.f32            q0, q0, q6")                        // q0   = a_re = t_re / W
            __ASM_EMIT("vmul.f32            q1, q1, q6")                        // q1   = a_im = t_im / W
            // Update data
            __ASM_EMIT("vld1.32             {d12[]}, [%[re]]")                  // q6   = b_re
            __ASM_EMIT("vld1.32             {d16[]}, [%[im]]")                  // q8   = b_im
            __ASM_EMIT("vmul.f32            q4, q0, q6")                        // q4   = a_re*b_re
            __ASM_EMIT("vmul.f32            q5, q1, q6")                        // q5   = a_im*b_re
            __ASM_EMIT("vmls.f32            q4, q1, q8")                        // q4   = a_re*b_re - a_im*b_im
            __ASM_EMIT("vmla.f32            q5, q0, q8")                        // q5   = a_im*b_re + a_re*b_im
            __ASM_EMIT("vst1.32             {d8[0]}, [%[re]]")
            __ASM_EMIT("vst1.32             {d10[0]}, [%[im]]")
            __ASM_EMIT("8:")

            : [re] "+r" (re), [im] "+r" (im), [f] "+r" (freq), [count] "+r" (count), [c] "+r" (c)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3",
              "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11",
              "q12", "q13", "q14", "q15"
        );
    }

    void filter_transfer_calc_pc(float *dst, const f_cascade_t *c, const float *freq, size_t count)
    {
        ARCH_ARM_ASM(
            // Unpack filter params
            __ASM_EMIT("vld3.f32            {d20[], d22[], d24[]}, [%[c]]")
            __ASM_EMIT("vld3.f32            {d21[], d23[], d25[]}, [%[c]]")
            __ASM_EMIT("add                 %[c], $0x10")
            __ASM_EMIT("vld3.f32            {d26[], d28[], d30[]}, [%[c]]")
            __ASM_EMIT("vld3.f32            {d27[], d29[], d31[]}, [%[c]]")
            // x8 blocks
            __ASM_EMIT("subs                %[count], $8")
            __ASM_EMIT("blo                 2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vldm                %[f]!, {q6-q7}")                    // q6   = f
            __ASM_EMIT("vmul.f32            q8, q6, q6")                        // q8   = f2 = f*f
            __ASM_EMIT("vmul.f32            q9, q7, q7")
            __ASM_EMIT("vmul.f32            q3, q11, q6")                       // q3   = t_im = t1*f
            __ASM_EMIT("vmul.f32            q5, q11, q7")
            __ASM_EMIT("vmul.f32            q6, q14, q6")                       // q6   = b_im = b1*f
            __ASM_EMIT("vmul.f32            q7, q14, q7")
            __ASM_EMIT("vmul.f32            q0, q12, q8")                       // q0   = t2*f2
            __ASM_EMIT("vmul.f32            q1, q12, q9")
            __ASM_EMIT("vmul.f32            q8, q15, q8")                       // q8   = b2*f2
            __ASM_EMIT("vmul.f32            q9, q15, q9")
            __ASM_EMIT("vsub.f32            q2, q10, q0")                       // q2   = t_re = t0 - t2*f2
            __ASM_EMIT("vsub.f32            q4, q10, q1")
            __ASM_EMIT("vsub.f32            q8, q13, q8")                       // q8   = b_re = b0 - b2*f2
            __ASM_EMIT("vsub.f32            q9, q13, q9")
            __ASM_EMIT("vmul.f32            q0, q2, q8")                        // q0   = t_re*b_re
            __ASM_EMIT("vmul.f32            q1, q3, q8")                        // q1   = t_im*b_re
            __ASM_EMIT("vmla.f32            q0, q3, q6")                        // q0   = t_re*b_re + t_im*b_im
            __ASM_EMIT("vmls.f32            q1, q2, q6")                        // q1   = t_im*b_re - t_re*b_im
            __ASM_EMIT("vmul.f32            q2, q4, q9")
            __ASM_EMIT("vmul.f32            q3, q5, q9")
            __ASM_EMIT("vmla.f32            q2, q5, q7")
            __ASM_EMIT("vmls.f32            q3, q4, q7")
            __ASM_EMIT("vmul.f32            q4, q8, q8")                        // q4   = b_re*b_re
            __ASM_EMIT("vmul.f32            q5, q9, q9")
            __ASM_EMIT("vmla.f32            q4, q6, q6")                        // q4   = W = b_re*b_re + b_im*b_im
            __ASM_EMIT("vmla.f32            q5, q7, q7")
            __ASM_EMIT("vrecpe.f32          q6, q4")                            // q6   = s2
            __ASM_EMIT("vrecpe.f32          q7, q5")
            __ASM_EMIT("vrecps.f32          q8, q6, q4")                        // q8   = (2 - R*s2)
            __ASM_EMIT("vrecps.f32          q9, q7, q5")
            __ASM_EMIT("vmul.f32            q6, q8, q6")                        // q6   = s2' = s2 * (2 - R*s2)
            __ASM_EMIT("vmul.f32            q7, q9, q7")
            __ASM_EMIT("vrecps.f32          q8, q6, q4")                        // q8   = (2 - R*s2')
            __ASM_EMIT("vrecps.f32          q9, q7, q5")
            __ASM_EMIT("vmul.f32            q6, q8, q6")                        // q6   = 1/W = s2" = s2' * (2 - R*s2')
            __ASM_EMIT("vmul.f32            q7, q9, q7")
            __ASM_EMIT("vmul.f32            q0, q0, q6")                        // q0   = a_re = t_re / W
            __ASM_EMIT("vmul.f32            q2, q2, q7")
            __ASM_EMIT("vmul.f32            q1, q1, q6")                        // q1   = a_im = t_im / W
            __ASM_EMIT("vmul.f32            q3, q3, q7")
            // Update data
            __ASM_EMIT("vzip.32             q0, q1")
            __ASM_EMIT("vzip.32             q2, q3")
            __ASM_EMIT("vstm                %[dst]!, {q0-q3}")
            __ASM_EMIT("subs                %[count], $8")
            __ASM_EMIT("bhs                 1b")
            __ASM_EMIT("2:")
            // x4 blocks
            __ASM_EMIT("adds                %[count], $4")
            __ASM_EMIT("blt                 4f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vldm                %[f]!, {q6}")                       // q6   = f
            __ASM_EMIT("vmul.f32            q8, q6, q6")                        // q8   = f2 = f*f
            __ASM_EMIT("vmul.f32            q3, q11, q6")                       // q3   = t_im = t1*f
            __ASM_EMIT("vmul.f32            q6, q14, q6")                       // q6   = b_im = b1*f
            __ASM_EMIT("vmul.f32            q0, q12, q8")                       // q0   = t2*f2
            __ASM_EMIT("vmul.f32            q8, q15, q8")                       // q8   = b2*f2
            __ASM_EMIT("vsub.f32            q2, q10, q0")                       // q2   = t_re = t0 - t2*f2
            __ASM_EMIT("vsub.f32            q8, q13, q8")                       // q8   = b_re = b0 - b2*f2
            __ASM_EMIT("vmul.f32            q0, q2, q8")                        // q0   = t_re*b_re
            __ASM_EMIT("vmul.f32            q1, q3, q8")                        // q1   = t_im*b_re
            __ASM_EMIT("vmla.f32            q0, q3, q6")                        // q0   = t_re*b_re + t_im*b_im
            __ASM_EMIT("vmls.f32            q1, q2, q6")                        // q1   = t_im*b_re - t_re*b_im
            __ASM_EMIT("vmul.f32            q4, q8, q8")                        // q4   = b_re*b_re
            __ASM_EMIT("vmla.f32            q4, q6, q6")                        // q4   = W = b_re*b_re + b_im*b_im
            __ASM_EMIT("vrecpe.f32          q6, q4")                            // q6   = s2
            __ASM_EMIT("vrecps.f32          q8, q6, q4")                        // q8   = (2 - R*s2)
            __ASM_EMIT("vmul.f32            q6, q8, q6")                        // q6   = s2' = s2 * (2 - R*s2)
            __ASM_EMIT("vrecps.f32          q8, q6, q4")                        // q8   = (2 - R*s2')
            __ASM_EMIT("vmul.f32            q6, q8, q6")                        // q6   = 1/W = s2" = s2' * (2 - R*s2')
            __ASM_EMIT("vmul.f32            q0, q0, q6")                        // q0   = a_re = t_re / W
            __ASM_EMIT("vmul.f32            q1, q1, q6")                        // q1   = a_im = t_im / W
            // Store data
            __ASM_EMIT("vst2.32             {q0-q1}, [%[dst]]!")
            __ASM_EMIT("sub                 %[count], $4")
            __ASM_EMIT("4:")
            // x2 blocks
            __ASM_EMIT("adds                %[count], $2")
            __ASM_EMIT("blt                 6f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vldm                %[f]!, {d12}")                      // q6   = f
            __ASM_EMIT("vmul.f32            q8, q6, q6")                        // q8   = f2 = f*f
            __ASM_EMIT("vmul.f32            q3, q11, q6")                       // q3   = t_im = t1*f
            __ASM_EMIT("vmul.f32            q6, q14, q6")                       // q6   = b_im = b1*f
            __ASM_EMIT("vmul.f32            q0, q12, q8")                       // q0   = t2*f2
            __ASM_EMIT("vmul.f32            q8, q15, q8")                       // q8   = b2*f2
            __ASM_EMIT("vsub.f32            q2, q10, q0")                       // q2   = t_re = t0 - t2*f2
            __ASM_EMIT("vsub.f32            q8, q13, q8")                       // q8   = b_re = b0 - b2*f2
            __ASM_EMIT("vmul.f32            q0, q2, q8")                        // q0   = t_re*b_re
            __ASM_EMIT("vmul.f32            q1, q3, q8")                        // q1   = t_im*b_re
            __ASM_EMIT("vmla.f32            q0, q3, q6")                        // q0   = t_re*b_re + t_im*b_im
            __ASM_EMIT("vmls.f32            q1, q2, q6")                        // q1   = t_im*b_re - t_re*b_im
            __ASM_EMIT("vmul.f32            q4, q8, q8")                        // q4   = b_re*b_re
            __ASM_EMIT("vmla.f32            q4, q6, q6")                        // q4   = W = b_re*b_re + b_im*b_im
            __ASM_EMIT("vrecpe.f32          q6, q4")                            // q6   = s2
            __ASM_EMIT("vrecps.f32          q8, q6, q4")                        // q8   = (2 - R*s2)
            __ASM_EMIT("vmul.f32            q6, q8, q6")                        // q6   = s2' = s2 * (2 - R*s2)
            __ASM_EMIT("vrecps.f32          q8, q6, q4")                        // q8   = (2 - R*s2')
            __ASM_EMIT("vmul.f32            q6, q8, q6")                        // q6   = 1/W = s2" = s2' * (2 - R*s2')
            __ASM_EMIT("vmul.f32            d0, d0, d12")                       // d0   = a_re = t_re / W
            __ASM_EMIT("vmul.f32            d1, d2, d12")                       // d1   = a_im = t_im / W
            // Store data
            __ASM_EMIT("vst2.32             {d0-d1}, [%[dst]]!")
            __ASM_EMIT("sub                 %[count], $2")
            __ASM_EMIT("6:")
            // x1 blocks
            __ASM_EMIT("adds                %[count], $1")
            __ASM_EMIT("blt                 8f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vld1.32             {d12[]}, [%[f]]")                   // q6   = f
            __ASM_EMIT("vmul.f32            q8, q6, q6")                        // q8   = f2 = f*f
            __ASM_EMIT("vmul.f32            q3, q11, q6")                       // q3   = t_im = t1*f
            __ASM_EMIT("vmul.f32            q6, q14, q6")                       // q6   = b_im = b1*f
            __ASM_EMIT("vmul.f32            q0, q12, q8")                       // q0   = t2*f2
            __ASM_EMIT("vmul.f32            q8, q15, q8")                       // q8   = b2*f2
            __ASM_EMIT("vsub.f32            q2, q10, q0")                       // q2   = t_re = t0 - t2*f2
            __ASM_EMIT("vsub.f32            q8, q13, q8")                       // q8   = b_re = b0 - b2*f2
            __ASM_EMIT("vmul.f32            q0, q2, q8")                        // q0   = t_re*b_re
            __ASM_EMIT("vmul.f32            q1, q3, q8")                        // q1   = t_im*b_re
            __ASM_EMIT("vmla.f32            q0, q3, q6")                        // q0   = t_re*b_re + t_im*b_im
            __ASM_EMIT("vmls.f32            q1, q2, q6")                        // q1   = t_im*b_re - t_re*b_im
            __ASM_EMIT("vmul.f32            q4, q8, q8")                        // q4   = b_re*b_re
            __ASM_EMIT("vmla.f32            q4, q6, q6")                        // q4   = W = b_re*b_re + b_im*b_im
            __ASM_EMIT("vrecpe.f32          q6, q4")                            // q6   = s2
            __ASM_EMIT("vrecps.f32          q8, q6, q4")                        // q8   = (2 - R*s2)
            __ASM_EMIT("vmul.f32            q6, q8, q6")                        // q6   = s2' = s2 * (2 - R*s2)
            __ASM_EMIT("vrecps.f32          q8, q6, q4")                        // q8   = (2 - R*s2')
            __ASM_EMIT("vmul.f32            q6, q8, q6")                        // q6   = 1/W = s2" = s2' * (2 - R*s2')
            __ASM_EMIT("vmul.f32            d0, d0, d12")                       // d0   = a_re = t_re / W
            __ASM_EMIT("vmul.f32            d1, d2, d12")                       // d1   = a_im = t_im / W
            // Store data
            __ASM_EMIT("vst2.32             {d0[0], d1[0]}, [%[dst]]")
            __ASM_EMIT("8:")

            : [dst] "+r" (dst), [f] "+r" (freq), [count] "+r" (count), [c] "+r" (c)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3",
              "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11",
              "q12", "q13", "q14", "q15"
        );
    }

    void filter_transfer_apply_pc(float *dst, const f_cascade_t *c, const float *freq, size_t count)
    {
        ARCH_ARM_ASM(
            // Unpack filter params
            __ASM_EMIT("vld3.f32            {d20[], d22[], d24[]}, [%[c]]")
            __ASM_EMIT("vld3.f32            {d21[], d23[], d25[]}, [%[c]]")
            __ASM_EMIT("add                 %[c], $0x10")
            __ASM_EMIT("vld3.f32            {d26[], d28[], d30[]}, [%[c]]")
            __ASM_EMIT("vld3.f32            {d27[], d29[], d31[]}, [%[c]]")
            // x8 blocks
            __ASM_EMIT("subs                %[count], $8")
            __ASM_EMIT("blo                 2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vldm                %[f]!, {q6-q7}")                    // q6   = f
            __ASM_EMIT("vmul.f32            q8, q6, q6")                        // q8   = f2 = f*f
            __ASM_EMIT("vmul.f32            q9, q7, q7")
            __ASM_EMIT("vmul.f32            q3, q11, q6")                       // q3   = t_im = t1*f
            __ASM_EMIT("vmul.f32            q5, q11, q7")
            __ASM_EMIT("vmul.f32            q6, q14, q6")                       // q6   = b_im = b1*f
            __ASM_EMIT("vmul.f32            q7, q14, q7")
            __ASM_EMIT("vmul.f32            q0, q12, q8")                       // q0   = t2*f2
            __ASM_EMIT("vmul.f32            q1, q12, q9")
            __ASM_EMIT("vmul.f32            q8, q15, q8")                       // q8   = b2*f2
            __ASM_EMIT("vmul.f32            q9, q15, q9")
            __ASM_EMIT("vsub.f32            q2, q10, q0")                       // q2   = t_re = t0 - t2*f2
            __ASM_EMIT("vsub.f32            q4, q10, q1")
            __ASM_EMIT("vsub.f32            q8, q13, q8")                       // q8   = b_re = b0 - b2*f2
            __ASM_EMIT("vsub.f32            q9, q13, q9")
            __ASM_EMIT("vmul.f32            q0, q2, q8")                        // q0   = t_re*b_re
            __ASM_EMIT("vmul.f32            q1, q3, q8")                        // q1   = t_im*b_re
            __ASM_EMIT("vmla.f32            q0, q3, q6")                        // q0   = t_re*b_re + t_im*b_im
            __ASM_EMIT("vmls.f32            q1, q2, q6")                        // q1   = t_im*b_re - t_re*b_im
            __ASM_EMIT("vmul.f32            q2, q4, q9")
            __ASM_EMIT("vmul.f32            q3, q5, q9")
            __ASM_EMIT("vmla.f32            q2, q5, q7")
            __ASM_EMIT("vmls.f32            q3, q4, q7")
            __ASM_EMIT("vmul.f32            q4, q8, q8")                        // q4   = b_re*b_re
            __ASM_EMIT("vmul.f32            q5, q9, q9")
            __ASM_EMIT("vmla.f32            q4, q6, q6")                        // q4   = W = b_re*b_re + b_im*b_im
            __ASM_EMIT("vmla.f32            q5, q7, q7")
            __ASM_EMIT("vrecpe.f32          q6, q4")                            // q6   = s2
            __ASM_EMIT("vrecpe.f32          q7, q5")
            __ASM_EMIT("vrecps.f32          q8, q6, q4")                        // q8   = (2 - R*s2)
            __ASM_EMIT("vrecps.f32          q9, q7, q5")
            __ASM_EMIT("vmul.f32            q6, q8, q6")                        // q6   = s2' = s2 * (2 - R*s2)
            __ASM_EMIT("vmul.f32            q7, q9, q7")
            __ASM_EMIT("vrecps.f32          q8, q6, q4")                        // q8   = (2 - R*s2')
            __ASM_EMIT("vrecps.f32          q9, q7, q5")
            __ASM_EMIT("vmul.f32            q6, q8, q6")                        // q6   = 1/W = s2" = s2' * (2 - R*s2')
            __ASM_EMIT("vmul.f32            q7, q9, q7")
            __ASM_EMIT("vmul.f32            q0, q0, q6")                        // q0   = a_re = t_re / W
            __ASM_EMIT("vmul.f32            q2, q2, q7")
            __ASM_EMIT("vmul.f32            q1, q1, q6")                        // q1   = a_im = t_im / W
            __ASM_EMIT("vmul.f32            q3, q3, q7")
            // Update data
            __ASM_EMIT("vldm                %[dst], {q6-q9}")                   // q6   = r0 i0 r1 i1, q7 = r2 i2 r3 i3 ...
            __ASM_EMIT("vuzp.32             q6, q7")                            // q6   = r0 r1 r2 r3, q7 = i0 i1 i2 i3
            __ASM_EMIT("vuzp.32             q8, q9")
            __ASM_EMIT("vmul.f32            q4, q0, q6")                        // q4   = a_re*b_re
            __ASM_EMIT("vmul.f32            q5, q1, q6")                        // q5   = a_im*b_re
            __ASM_EMIT("vmls.f32            q4, q1, q7")                        // q4   = a_re*b_re - a_im*b_im
            __ASM_EMIT("vmla.f32            q5, q0, q7")                        // q5   = a_im*b_re + a_re*b_im
            __ASM_EMIT("vmul.f32            q6, q2, q8")
            __ASM_EMIT("vmul.f32            q7, q3, q8")
            __ASM_EMIT("vmls.f32            q6, q3, q9")
            __ASM_EMIT("vmla.f32            q7, q2, q9")
            __ASM_EMIT("vzip.32             q4, q5")                            // q4   = r0 i0 r1 i1, q5 = r2 i2, r3 i3
            __ASM_EMIT("vzip.32             q6, q7")
            __ASM_EMIT("vstm                %[dst]!, {q4-q7}")
            __ASM_EMIT("subs                %[count], $8")
            __ASM_EMIT("bhs                 1b")
            __ASM_EMIT("2:")
            // x4 blocks
            __ASM_EMIT("adds                %[count], $4")
            __ASM_EMIT("blt                 4f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vldm                %[f]!, {q6}")                       // q6   = f
            __ASM_EMIT("vmul.f32            q8, q6, q6")                        // q8   = f2 = f*f
            __ASM_EMIT("vmul.f32            q3, q11, q6")                       // q3   = t_im = t1*f
            __ASM_EMIT("vmul.f32            q6, q14, q6")                       // q6   = b_im = b1*f
            __ASM_EMIT("vmul.f32            q0, q12, q8")                       // q0   = t2*f2
            __ASM_EMIT("vmul.f32            q8, q15, q8")                       // q8   = b2*f2
            __ASM_EMIT("vsub.f32            q2, q10, q0")                       // q2   = t_re = t0 - t2*f2
            __ASM_EMIT("vsub.f32            q8, q13, q8")                       // q8   = b_re = b0 - b2*f2
            __ASM_EMIT("vmul.f32            q0, q2, q8")                        // q0   = t_re*b_re
            __ASM_EMIT("vmul.f32            q1, q3, q8")                        // q1   = t_im*b_re
            __ASM_EMIT("vmla.f32            q0, q3, q6")                        // q0   = t_re*b_re + t_im*b_im
            __ASM_EMIT("vmls.f32            q1, q2, q6")                        // q1   = t_im*b_re - t_re*b_im
            __ASM_EMIT("vmul.f32            q4, q8, q8")                        // q4   = b_re*b_re
            __ASM_EMIT("vmla.f32            q4, q6, q6")                        // q4   = W = b_re*b_re + b_im*b_im
            __ASM_EMIT("vrecpe.f32          q6, q4")                            // q6   = s2
            __ASM_EMIT("vrecps.f32          q8, q6, q4")                        // q8   = (2 - R*s2)
            __ASM_EMIT("vmul.f32            q6, q8, q6")                        // q6   = s2' = s2 * (2 - R*s2)
            __ASM_EMIT("vrecps.f32          q8, q6, q4")                        // q8   = (2 - R*s2')
            __ASM_EMIT("vmul.f32            q6, q8, q6")                        // q6   = 1/W = s2" = s2' * (2 - R*s2')
            __ASM_EMIT("vmul.f32            q0, q0, q6")                        // q0   = a_re = t_re / W
            __ASM_EMIT("vmul.f32            q1, q1, q6")                        // q1   = a_im = t_im / W
            // Update data
            __ASM_EMIT("vld2.32             {q6-q7}, [%[dst]]")                 // q6   = b_re, q7 = b_im
            __ASM_EMIT("vmul.f32            q4, q0, q6")                        // q4   = a_re*b_re
            __ASM_EMIT("vmul.f32            q5, q1, q6")                        // q5   = a_im*b_re
            __ASM_EMIT("vmls.f32            q4, q1, q7")                        // q4   = a_re*b_re - a_im*b_im
            __ASM_EMIT("vmla.f32            q5, q0, q7")                        // q5   = a_im*b_re + a_re*b_im
            __ASM_EMIT("vst2.32             {q4-q5}, [%[dst]]!")
            __ASM_EMIT("sub                 %[count], $4")
            __ASM_EMIT("4:")
            // x2 blocks
            __ASM_EMIT("adds                %[count], $2")
            __ASM_EMIT("blt                 6f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vldm                %[f]!, {d12}")                      // q6   = f
            __ASM_EMIT("vmul.f32            q8, q6, q6")                        // q8   = f2 = f*f
            __ASM_EMIT("vmul.f32            q3, q11, q6")                       // q3   = t_im = t1*f
            __ASM_EMIT("vmul.f32            q6, q14, q6")                       // q6   = b_im = b1*f
            __ASM_EMIT("vmul.f32            q0, q12, q8")                       // q0   = t2*f2
            __ASM_EMIT("vmul.f32            q8, q15, q8")                       // q8   = b2*f2
            __ASM_EMIT("vsub.f32            q2, q10, q0")                       // q2   = t_re = t0 - t2*f2
            __ASM_EMIT("vsub.f32            q8, q13, q8")                       // q8   = b_re = b0 - b2*f2
            __ASM_EMIT("vmul.f32            q0, q2, q8")                        // q0   = t_re*b_re
            __ASM_EMIT("vmul.f32            q1, q3, q8")                        // q1   = t_im*b_re
            __ASM_EMIT("vmla.f32            q0, q3, q6")                        // q0   = t_re*b_re + t_im*b_im
            __ASM_EMIT("vmls.f32            q1, q2, q6")                        // q1   = t_im*b_re - t_re*b_im
            __ASM_EMIT("vmul.f32            q4, q8, q8")                        // q4   = b_re*b_re
            __ASM_EMIT("vmla.f32            q4, q6, q6")                        // q4   = W = b_re*b_re + b_im*b_im
            __ASM_EMIT("vrecpe.f32          q6, q4")                            // q6   = s2
            __ASM_EMIT("vrecps.f32          q8, q6, q4")                        // q8   = (2 - R*s2)
            __ASM_EMIT("vmul.f32            q6, q8, q6")                        // q6   = s2' = s2 * (2 - R*s2)
            __ASM_EMIT("vrecps.f32          q8, q6, q4")                        // q8   = (2 - R*s2')
            __ASM_EMIT("vmul.f32            q6, q8, q6")                        // q6   = 1/W = s2" = s2' * (2 - R*s2')
            __ASM_EMIT("vmul.f32            q0, q0, q6")                        // q0   = a_re = t_re / W
            __ASM_EMIT("vmul.f32            q1, q1, q6")                        // q1   = a_im = t_im / W
            // Update data
            __ASM_EMIT("vld2.32             {d12-d13}, [%[dst]]")               // d12  = b_re, q7 = b_im
            __ASM_EMIT("vmul.f32            d8, d0, d12")                       // d8   = a_re*b_re
            __ASM_EMIT("vmul.f32            d9, d2, d12")                       // d9  = a_im*b_re
            __ASM_EMIT("vmls.f32            d8, d2, d13")                       // d8   = a_re*b_re - a_im*b_im
            __ASM_EMIT("vmla.f32            d9, d0, d13")                       // d9  = a_im*b_re + a_re*b_im
            __ASM_EMIT("vst2.32             {d8-d9}, [%[dst]]!")
            __ASM_EMIT("sub                 %[count], $2")
            __ASM_EMIT("6:")
            // x1 blocks
            __ASM_EMIT("adds                %[count], $1")
            __ASM_EMIT("blt                 8f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vld1.32             {d12[]}, [%[f]]")                   // q6   = f
            __ASM_EMIT("vmul.f32            q8, q6, q6")                        // q8   = f2 = f*f
            __ASM_EMIT("vmul.f32            q3, q11, q6")                       // q3   = t_im = t1*f
            __ASM_EMIT("vmul.f32            q6, q14, q6")                       // q6   = b_im = b1*f
            __ASM_EMIT("vmul.f32            q0, q12, q8")                       // q0   = t2*f2
            __ASM_EMIT("vmul.f32            q8, q15, q8")                       // q8   = b2*f2
            __ASM_EMIT("vsub.f32            q2, q10, q0")                       // q2   = t_re = t0 - t2*f2
            __ASM_EMIT("vsub.f32            q8, q13, q8")                       // q8   = b_re = b0 - b2*f2
            __ASM_EMIT("vmul.f32            q0, q2, q8")                        // q0   = t_re*b_re
            __ASM_EMIT("vmul.f32            q1, q3, q8")                        // q1   = t_im*b_re
            __ASM_EMIT("vmla.f32            q0, q3, q6")                        // q0   = t_re*b_re + t_im*b_im
            __ASM_EMIT("vmls.f32            q1, q2, q6")                        // q1   = t_im*b_re - t_re*b_im
            __ASM_EMIT("vmul.f32            q4, q8, q8")                        // q4   = b_re*b_re
            __ASM_EMIT("vmla.f32            q4, q6, q6")                        // q4   = W = b_re*b_re + b_im*b_im
            __ASM_EMIT("vrecpe.f32          q6, q4")                            // q6   = s2
            __ASM_EMIT("vrecps.f32          q8, q6, q4")                        // q8   = (2 - R*s2)
            __ASM_EMIT("vmul.f32            q6, q8, q6")                        // q6   = s2' = s2 * (2 - R*s2)
            __ASM_EMIT("vrecps.f32          q8, q6, q4")                        // q8   = (2 - R*s2')
            __ASM_EMIT("vmul.f32            q6, q8, q6")                        // q6   = 1/W = s2" = s2' * (2 - R*s2')
            __ASM_EMIT("vmul.f32            q0, q0, q6")                        // q0   = a_re = t_re / W
            __ASM_EMIT("vmul.f32            q1, q1, q6")                        // q1   = a_im = t_im / W
            // Update data
            __ASM_EMIT("vld2.32             {d12[], d13[]}, [%[dst]]")          // q6   = b_re
            __ASM_EMIT("vmul.f32            d8, d0, d12")                       // d8   = a_re*b_re
            __ASM_EMIT("vmul.f32            d9, d2, d12")                       // d9  = a_im*b_re
            __ASM_EMIT("vmls.f32            d8, d2, d13")                       // d8   = a_re*b_re - a_im*b_im
            __ASM_EMIT("vmla.f32            d9, d0, d13")                       // d9  = a_im*b_re + a_re*b_im
            __ASM_EMIT("vst2.32             {d8[0], d9[0]}, [%[dst]]")
            __ASM_EMIT("8:")

            : [dst] "+r" (dst), [f] "+r" (freq), [count] "+r" (count), [c] "+r" (c)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3",
              "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11",
              "q12", "q13", "q14", "q15"
        );
    }
}

#endif /* DSP_ARCH_ARM_NEON_D32_FILTERS_TRANSFER_H_ */
