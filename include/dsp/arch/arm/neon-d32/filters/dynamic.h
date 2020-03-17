/*
 * dynamic.h
 *
 *  Created on: 17 окт. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_ARM_NEON_D32_FILTERS_DYNAMIC_H_
#define DSP_ARCH_ARM_NEON_D32_FILTERS_DYNAMIC_H_

#ifndef DSP_ARCH_ARM_NEON_32_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_ARM_NEON_32_IMPL */

namespace neon_d32
{
    void dyn_biquad_process_x1(float *dst, const float *src, float *d, size_t count, const biquad_x1_t *f)
    {
        ARCH_ARM_ASM
        (
            // Check count
            __ASM_EMIT("vldm            %[d],  {s14-s15}")
            // s8   = a0
            // s9   = a1
            // s10  = a2
            // s11  = b1
            // s12  = b1
            // s14  = d0
            // s15  = d1
            // x2 blocks
            __ASM_EMIT("subs            %[count], %[count], $2")
            __ASM_EMIT("blo             2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vldm            %[f], {s8-s12}")
            __ASM_EMIT("vldm            %[src]!, {s0-s1}")                      // s0   = s0, s1 = s1
            __ASM_EMIT("add             %[f], %[f], $0x20")
            __ASM_EMIT("vmul.f32        s2, s0, s8")                            // s2   = a0*s0
            __ASM_EMIT("vmul.f32        s4, s0, s9")                            // s4   = a1*s0
            __ASM_EMIT("vmul.f32        s6, s0, s10")                           // s6   = a2*s0
            __ASM_EMIT("vadd.f32        s0, s2, s14")                           // s0   = s0' = d0 + a0*s0
            __ASM_EMIT("vmla.f32        s4, s0, s11")                           // s4   = a1*s0 + b1*s0'
            __ASM_EMIT("vmla.f32        s6, s0, s12")                           // s6   = d1' = a2*s0 + b2*s0'
            __ASM_EMIT("vadd.f32        s4, s15, s4")                           // s4   = d0' = d1 + a1*s0 + b1*s0'
            __ASM_EMIT("vldm            %[f], {s8-s12}")
            __ASM_EMIT("add             %[f], %[f], $0x20")
            __ASM_EMIT("vmul.f32        s3, s1, s8")                            // s3   = a0*s1
            __ASM_EMIT("vmul.f32        s5, s1, s9")                            // s5   = a1*s1
            __ASM_EMIT("vmul.f32        s7, s1, s10")                           // s7   = a2*s1
            __ASM_EMIT("vadd.f32        s1, s3, s4")                            // s1   = s1' = d0' + a0*s1
            __ASM_EMIT("vmla.f32        s5, s1, s11")                           // s5   = a1*s1 + b1*s1'
            __ASM_EMIT("vmla.f32        s7, s1, s12")                           // s7   = d1" = a2*s1 + b2*s1'
            __ASM_EMIT("vadd.f32        s14, s6, s5")                           // s14  = d0" = d1' + a1*s1 + b1*s1'
            __ASM_EMIT("vmov            s15, s7")
            __ASM_EMIT("vstm            %[dst]!, {s0-s1}")
            __ASM_EMIT("subs            %[count], %[count], $2")
            __ASM_EMIT("bhs             1b")
            __ASM_EMIT("2:")
            // x1 block:
            __ASM_EMIT("adds            %[count], %[count], $1")
            __ASM_EMIT("blt             4f")
            __ASM_EMIT("vldm            %[f], {s8-s12}")
            __ASM_EMIT("vldr            s0, [%[src]]")                          // v0   = s0
            __ASM_EMIT("vmul.f32        s2, s0, s8")                            // s2   = a0*s0
            __ASM_EMIT("vmul.f32        s4, s0, s9")                            // s4   = a1*s0
            __ASM_EMIT("vmul.f32        s6, s0, s10")                           // s6   = a2*s0
            __ASM_EMIT("vadd.f32        s0, s2, s14")                           // s0   = s0' = d0 + a0*s0
            __ASM_EMIT("vmla.f32        s4, s0, s11")                           // s4   = a1*s0 + b1*s0'
            __ASM_EMIT("vmla.f32        s6, s0, s12")                           // s6   = d1' = a2*s0 + b2*s0'
            __ASM_EMIT("vadd.f32        s14, s15, s4")                          // s14  = d0' = d1 + a1*s0 + b1*s0'
            __ASM_EMIT("vmov            s15, s6")
            __ASM_EMIT("vstr            s0, [%[dst]]")
            __ASM_EMIT("4:")
            // Store the updated buffer state
            __ASM_EMIT("vstm            %[d],  {s14-s15}")

            : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count),
              [f] "+r" (f)
            : [d] "r" (d)
            : "cc", "memory",
              "q0", "q1", "q2", "q3"
        );
    }

    void dyn_biquad_process_x2(float *dst, const float *src, float *d, size_t count, const biquad_x2_t *f)
    {
        ARCH_ARM_ASM
        (
            // Check count
            __ASM_EMIT("tst             %[count], %[count]")
            __ASM_EMIT("beq             6f")
            // x1 head block
            __ASM_EMIT("vldm            %[d], {d14-d15}")                       // d14  = d0 e0, d15 = d1 e1
            __ASM_EMIT("vldm            %[f]!, {d8-d13}")                       // d8   = a0, d9 = a1, d10 = a2, d11 = b1, d12 = b2, d13 = 0
            __ASM_EMIT("vld1.32         {d0[0]}, [%[src]]!")                    // d0   = s0
            __ASM_EMIT("vmul.f32        d1, d8, d0")                            // d1   = a0*s0
            __ASM_EMIT("vadd.f32        d4, d14, d1")                           // d4   = s' = d0+a0*s0
            __ASM_EMIT("vmul.f32        d2, d9, d0")                            // d2   = a1*s0
            __ASM_EMIT("vadd.f32        d5, d15, d2")                           // d5   = d1+a1*s0
            __ASM_EMIT("vmul.f32        d6, d10, d0")                           // d6   = a2*s0
            __ASM_EMIT("vmla.f32        d5, d11, d4")                           // d5   = d0' = d1+a1*s0+b1*s'
            __ASM_EMIT("vmla.f32        d6, d12, d4")                           // d6   = d1' = a2*s0+b2*s'
            __ASM_EMIT("vext.32         d0, d0, d4, $1")                        // shift
            __ASM_EMIT("vmov            s28, s10")                              // update d0
            __ASM_EMIT("vmov            s30, s12")                              // update d1
            // x2 blocks
            __ASM_EMIT("subs            %[count], %[count], $1")
            __ASM_EMIT("bls             2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vldm            %[f]!, {d8-d13}")                       // d8   = a0, d9 = a1, d10 = a2, d11 = b1, d12 = b2, d13 = 0
            __ASM_EMIT("vld1.32         {d0[0]}, [%[src]]!")                    // d0   = s0 j0
            __ASM_EMIT("vmul.f32        d1, d8, d0")                            // d1   = a0*s0 a0*j0
            __ASM_EMIT("vadd.f32        d4, d14, d1")                           // d4   = s' j' = d0+a0*s0 e0+a0*j0
            __ASM_EMIT("vmul.f32        d2, d9, d0")                            // d2   = a1*s0 a1*j0
            __ASM_EMIT("vst1.32         {d4[1]}, [%[dst]]!")
            __ASM_EMIT("vadd.f32        d14, d15, d2")                          // d15  = d1+a1*s0 e1+a1*j0
            __ASM_EMIT("vmul.f32        d15, d10, d0")                          // d23  = a2*s0 a2*j0
            __ASM_EMIT("vmla.f32        d14, d11, d4")                          // d22  = d0' e0' = d1+a1*s0+b1*s' e1+a1*j0+b1*j'
            __ASM_EMIT("vmla.f32        d15, d12, d4")                          // d23  = d1' e1' = a2*s0+b2*s' a2*j0 b2*j'
            __ASM_EMIT("subs            %[count], %[count], $1")
            __ASM_EMIT("vext.32         d0, d0, d4, $1")                        // shift
            __ASM_EMIT("bhi             1b")
            __ASM_EMIT("2:")
            // x1 tail block
            __ASM_EMIT("vldm            %[f], {d8-d13}")                        // d8   = a0, d9 = a1, d10 = a2, d11 = b1, d12 = b2, d13 = 0
            __ASM_EMIT("vmul.f32        d1, d8, d0")                            // v1   = a0*j0
            __ASM_EMIT("vadd.f32        d4, d14, d1")                           // v4   = s' = e0 + a0*j0
            __ASM_EMIT("vmul.f32        d2, d9, d0")                            // v2   = a1*j0
            __ASM_EMIT("vadd.f32        d5, d15, d2")                           // v5   = e1 + a1*j0
            __ASM_EMIT("vst1.32         {d4[1]}, [%[dst]]")
            __ASM_EMIT("vmul.f32        d6, d10, d0")                           // v6   = a2*j0
            __ASM_EMIT("vmla.f32        d5, d11, d4")                           // v5   = d0' = e1 + a1*j0 + b1*j'
            __ASM_EMIT("vmla.f32        d6, d12, d4")                           // v6   = d1' = a2*j0 + b2*j'
            __ASM_EMIT("vmov            s29, s11")                              // update d0
            __ASM_EMIT("vmov            s31, s13")                              // update d1
            // Store the updated buffer state
            __ASM_EMIT("vstm            %[d], {d14-d15}")
            __ASM_EMIT("6:")

            : [dst] "+r" (dst), [src] "+r" (src),
              [count] "+r" (count), [f] "+r" (f)
            : [d] "r" (d)
            : "cc", "memory",
              "q0", "q1", "q2", "q3",
              "q4", "q5", "q6", "q7"
        );
    }

    static const uint32_t dyn_biquad_x4_mask[8] __lsp_aligned16 =
    {
        0xffffffff, 0x00000000, 0x00000000, 0x00000000,
        0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff
    };

    void dyn_biquad_process_x4(float *dst, const float *src, float *d, size_t count, const biquad_x4_t *f)
    {
        IF_ARCH_ARM(
            size_t mask;
        );

        ARCH_ARM_ASM
        (
            __ASM_EMIT("tst         %[count], %[count]")
            __ASM_EMIT("beq         8f")

            // Prepare
            __ASM_EMIT("vldm        %[d], {q1-q2}")                         // q1-q2 = { d0, d1 }
            __ASM_EMIT("vldm        %[X_MASK], {q8-q9}")                    // q8-q9 = { vmask, 1 }
            __ASM_EMIT("mov         %[mask], $1")                           // mask  = 1

            // Do pre-loop
            __ASM_EMIT("1:")
            __ASM_EMIT("vld1.32     d0[0], [%[src]]!")                      // q0    = s
            __ASM_EMIT("vldm        %[f]!, {q3-q7}")                        // q3-q7 = { a0, a1, a2, b1, b2 }
            __ASM_EMIT("vmul.f32    q10, q3, q0")                           // q10   = a0*s
            __ASM_EMIT("vmul.f32    q11, q4, q0")                           // q11   = a1*s
            __ASM_EMIT("vadd.f32    q10, q1")                               // q10   = a0*s + d0 = s2
            __ASM_EMIT("vmul.f32    q12, q5, q0")                           // q12   = a2*s
            __ASM_EMIT("vmla.f32    q11, q6, q10")                          // q11   = a1*s + b1*s2
            __ASM_EMIT("vmla.f32    q12, q7, q10")                          // q12   = d1' = a2*s + b2*s2
            __ASM_EMIT("vadd.f32    q11, q2")                               // q11   = d0' = d1 + a1*s + b1*s2
            __ASM_EMIT("vext.32     q0, q10, q10, $3")                      // q0    = s2[3] s2[0] s2[1] s2[2]
            __ASM_EMIT("vbit        q2, q12, q8")                           // q2    = (d1 & ~vmask) | (d1' & vmask)
            __ASM_EMIT("vbit        q1, q11, q8")                           // q1    = (d0 & ~vmask) | (d0' & vmask)

            __ASM_EMIT("subs        %[count], $1")
            __ASM_EMIT("beq         4f")
            __ASM_EMIT("orr         %[mask], %[mask], LSL $1")              // mask  = (mask << 1) | 1
            __ASM_EMIT("vext.32     q8, q9, q8, $3")                        // q8    = (vmask << 1) | 1
            __ASM_EMIT("cmp         %[mask], $0x0f")
            __ASM_EMIT("bne         1b")

            // Do main loop
            __ASM_EMIT("3:")
            __ASM_EMIT("vld1.32     d0[0], [%[src]]!")                      // q0    = s
            __ASM_EMIT("vldm        %[f]!, {q3-q7}")                        // q3-q7 = { a0, a1, a2, b1, b2 }
            __ASM_EMIT("vmul.f32    q10, q3, q0")                           // q10   = a0*s
            __ASM_EMIT("vmul.f32    q11, q4, q0")                           // q11   = a1*s
            __ASM_EMIT("vadd.f32    q10, q1")                               // q10   = a0*s + d0 = s2
            __ASM_EMIT("vmul.f32    q12, q5, q0")                           // q12   = a2*s
            __ASM_EMIT("vmla.f32    q11, q6, q10")                          // q11   = a1*s + b1*s2
            __ASM_EMIT("vmla.f32    q12, q7, q10")                          // q12   = d1' = a2*s + b2*s2
            __ASM_EMIT("vadd.f32    q1, q11, q2")                           // q1    = d0' = d1 + a1*s + b1*s2
            __ASM_EMIT("vext.32     q0, q10, q10, $3")                      // q0    = s2[3] s2[0] s2[1] s2[2]
            __ASM_EMIT("vmov        q2, q12")
            __ASM_EMIT("vst1.32     d0[0], [%[dst]]!")
            __ASM_EMIT("subs        %[count], $1")
            __ASM_EMIT("bne         3b")

            // Do post-loop
            __ASM_EMIT("4:")
            __ASM_EMIT("veor        q9, q9")                                // q9    = 0
            __ASM_EMIT("lsl         %[mask], $1")                           // mask  = mask << 1
            __ASM_EMIT("vext.32     q8, q9, q8, $3")                        // q8    = (vmask << 1) | 0

            __ASM_EMIT("5:")
            __ASM_EMIT("vldm        %[f]!, {q3-q7}")                        // q3-q7 = { a0, a1, a2, b1, b2 }
            __ASM_EMIT("vmul.f32    q10, q3, q0")                           // q10   = a0*s
            __ASM_EMIT("vmul.f32    q11, q4, q0")                           // q11   = a1*s
            __ASM_EMIT("vadd.f32    q10, q1")                               // q10   = a0*s + d0 = s2
            __ASM_EMIT("vmul.f32    q12, q5, q0")                           // q12   = a2*s
            __ASM_EMIT("vmla.f32    q11, q6, q10")                          // q11   = a1*s + b1*s2
            __ASM_EMIT("vmla.f32    q12, q7, q10")                          // q12   = d1' = a2*s + b2*s2
            __ASM_EMIT("vadd.f32    q11, q2")                               // q11   = d0' = d1 + a1*s + b1*s2
            __ASM_EMIT("vext.32     q0, q10, q10, $3")                      // q0    = s2[3] s2[0] s2[1] s2[2]
            __ASM_EMIT("tst         %[mask], $0x08")
            __ASM_EMIT("beq         6f")
            __ASM_EMIT("vst1.32     d0[0], [%[dst]]!")
            __ASM_EMIT("6:")
            __ASM_EMIT("vbit        q2, q12, q8")                           // q2    = (d1 & ~vmask) | (d1' & vmask)
            __ASM_EMIT("vbit        q1, q11, q8")                           // q1    = (d0 & ~vmask) | (d0' & vmask)
            __ASM_EMIT("lsl         %[mask], $1")                           // mask  = mask << 1
            __ASM_EMIT("vext.32     q8, q9, q8, $3")                        // q8    = vmask << 1
            __ASM_EMIT("tst         %[mask], $0x0f")                        // mask  == 0 ?
            __ASM_EMIT("bne         5b")

            // Store memory
            __ASM_EMIT("6:")
            __ASM_EMIT("vstm        %[d], {q1-q2}")
            __ASM_EMIT("8:")

            : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count), [f] "+r" (f),
              [mask] "=&r" (mask)
            : [d] "r" (d),
              [X_MASK] "r" (&dyn_biquad_x4_mask[0])
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12"
        );
    }

    static const uint32_t dyn_biquad_x8_mask[16] __lsp_aligned16 =
    {
        0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
        0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
        0xffffffff, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
    };

    void dyn_biquad_process_x8(float *dst, const float *src, float *d, size_t count, const biquad_x8_t *f)
    {
        IF_ARCH_ARM(
            float vmask[16] __lsp_aligned16;
            size_t mask;
        );

        ARCH_ARM_ASM
        (
            __ASM_EMIT("tst         %[count], %[count]")
            __ASM_EMIT("beq         8f")

            // Prepare
            __ASM_EMIT("vldm        %[d], {q2-q5}")                         // q2-q3 = d0, q4-q5 = d1
            __ASM_EMIT("vldm        %[X_MASK], {q12-q15}")                  // q12-q15 = vmask
            __ASM_EMIT("mov         %[mask], $1")                           // mask  = 1
            __ASM_EMIT("vstm        %[vmask], {q12-q15}")

            // Do pre-loop
            __ASM_EMIT("1:")
            __ASM_EMIT("vldm        %[f]!, {q6-q11}")                       // q6-q7 = a0, q8-q9 = a1, q10-q11 = a2
            __ASM_EMIT("vld1.32     d0[0], [%[src]]!")                      // q0    = s
            __ASM_EMIT("vldm        %[f]!, {q12-q15}")                      // q12-q13 = b1, q14-q15 = b2

            __ASM_EMIT("vmul.f32    q6, q6, q0")                            // q6    = a0*s
            __ASM_EMIT("vmul.f32    q7, q7, q1")
            __ASM_EMIT("vmul.f32    q8, q8, q0")                            // q8    = a1*s
            __ASM_EMIT("vmul.f32    q9, q9, q1")
            __ASM_EMIT("vadd.f32    q6, q6, q2")                            // q6    = a0*s + d0 = s2
            __ASM_EMIT("vadd.f32    q7, q7, q3")
            __ASM_EMIT("vmul.f32    q10, q10, q0")                          // q10   = a2*s
            __ASM_EMIT("vmul.f32    q11, q11, q1")
            __ASM_EMIT("vmla.f32    q8, q12, q6")                           // q8    = a1*s + b1*s2
            __ASM_EMIT("vmla.f32    q9, q13, q7")
            __ASM_EMIT("vmla.f32    q10, q14, q6")                          // q10   = a2*s + b2*s2 = d1'
            __ASM_EMIT("vmla.f32    q11, q15, q7")
            __ASM_EMIT("vadd.f32    q8, q8, q4")                            // q8    = a1*s + b1*s2 + d1 = d0'
            __ASM_EMIT("vadd.f32    q9, q9, q5")

            __ASM_EMIT("vext.32     q1, q6, q7, $3")                        // q1    = s2[3] s2[4] s2[5] s2[6]
            __ASM_EMIT("vldm        %[vmask], {q12-q15}")                   // q12-q15 = vmask
            __ASM_EMIT("vext.32     q0, q6, q6, $3")                        // q0    = s2[3] s2[0] s2[1] s2[2]
            __ASM_EMIT("vbit        q2, q8, q14")                           // q2    = (d0 & ~vmask) | (d0' & vmask)
            __ASM_EMIT("vbit        q3, q9, q15")
            __ASM_EMIT("vbit        q4, q10, q14")                          // q4    = (d1 & ~vmask) | (d1' & vmask)
            __ASM_EMIT("vbit        q5, q11, q15")

            __ASM_EMIT("subs        %[count], $1")
            __ASM_EMIT("beq         4f")
            __ASM_EMIT("vext.32     q15, q14, q15, $3")                     // vmask = vmask << 1
            __ASM_EMIT("orr         %[mask], %[mask], LSL $1")              // mask  = (mask << 1) | 1
            __ASM_EMIT("vext.32     q14, q13, q14, $3")                     // vmask = (vmask << 1) | 1
            __ASM_EMIT("cmp         %[mask], $0xff")
            __ASM_EMIT("vstm        %[vmask], {q12-q15}")
            __ASM_EMIT("bne         1b")

            // Do main loop
            __ASM_EMIT("3:")
            __ASM_EMIT("vldm        %[f]!, {q6-q11}")                       // q6-q7 = a0, q8-q9 = a1, q10-q11 = a2
            __ASM_EMIT("vld1.32     d0[0], [%[src]]!")                      // q0    = s
            __ASM_EMIT("vldm        %[f]!, {q12-q15}")                      // q12-q13 = b1, q14-q15 = b2

            __ASM_EMIT("vmul.f32    q6, q6, q0")                            // q6    = a0*s
            __ASM_EMIT("vmul.f32    q7, q7, q1")
            __ASM_EMIT("vmul.f32    q8, q8, q0")                            // q8    = a1*s
            __ASM_EMIT("vmul.f32    q9, q9, q1")
            __ASM_EMIT("vadd.f32    q6, q6, q2")                            // q6    = a0*s + d0 = s2
            __ASM_EMIT("vadd.f32    q7, q7, q3")
            __ASM_EMIT("vmul.f32    q10, q10, q0")                          // q10   = a2*s
            __ASM_EMIT("vmul.f32    q11, q11, q1")
            __ASM_EMIT("vmla.f32    q8, q12, q6")                           // q8    = a1*s + b1*s2
            __ASM_EMIT("vmla.f32    q9, q13, q7")
            __ASM_EMIT("vmla.f32    q10, q14, q6")                          // q10   = a2*s + b2*s2 = d1'
            __ASM_EMIT("vmla.f32    q11, q15, q7")
            __ASM_EMIT("vadd.f32    q2, q8, q4")                            // q2    = a1*s + b1*s2 + d1 = d0'
            __ASM_EMIT("vadd.f32    q3, q9, q5")
            __ASM_EMIT("vmov        q4, q10")                               // q4    = d1'
            __ASM_EMIT("vmov        q5, q11")

            __ASM_EMIT("vst1.32     d15[1], [%[dst]]!")                     // *dst++= s2[7]
            __ASM_EMIT("vext.32     q1, q6, q7, $3")                        // q1    = s2[3] s2[4] s2[5] s2[6]
            __ASM_EMIT("vext.32     q0, q6, q6, $3")                        // q0    = s2[3] s2[0] s2[1] s2[2]

            __ASM_EMIT("subs        %[count], $1")
            __ASM_EMIT("bne         3b")

            // Do post-loop
            __ASM_EMIT("4:")
            __ASM_EMIT("vldm        %[vmask], {q12-q15}")                   // q12-q15 = vmask
            __ASM_EMIT("veor        q13, q13")                              // q13   = 0
            __ASM_EMIT("vext.32     q15, q14, q15, $3")                     // vmask = vmask << 1
            __ASM_EMIT("lsl         %[mask], $1")                           // mask  = mask << 1
            __ASM_EMIT("vext.32     q14, q13, q14, $3")                     // vmask = vmask << 1
            __ASM_EMIT("vstm        %[vmask], {q12-q15}")

            __ASM_EMIT("5:")
            __ASM_EMIT("vldm        %[f]!, {q6-q11}")                       // q6-q7 = a0, q8-q9 = a1, q10-q11 = a2
            __ASM_EMIT("vldm        %[f]!, {q12-q15}")                      // q12-q13 = b1, q14-q15 = b2
            __ASM_EMIT("vmul.f32    q6, q6, q0")                            // q6    = a0*s
            __ASM_EMIT("vmul.f32    q7, q7, q1")
            __ASM_EMIT("vmul.f32    q8, q8, q0")                            // q8    = a1*s
            __ASM_EMIT("vmul.f32    q9, q9, q1")
            __ASM_EMIT("vadd.f32    q6, q6, q2")                            // q6    = a0*s + d0 = s2
            __ASM_EMIT("vadd.f32    q7, q7, q3")
            __ASM_EMIT("vmul.f32    q10, q10, q0")                          // q10   = a2*s
            __ASM_EMIT("vmul.f32    q11, q11, q1")
            __ASM_EMIT("vmla.f32    q8, q12, q6")                           // q8    = a1*s + b1*s2
            __ASM_EMIT("vmla.f32    q9, q13, q7")
            __ASM_EMIT("vmla.f32    q10, q14, q6")                          // q10   = a2*s + b2*s2 = d1'
            __ASM_EMIT("vmla.f32    q11, q15, q7")
            __ASM_EMIT("vadd.f32    q8, q8, q4")                            // q8    = a1*s + b1*s2 + d1 = d0'
            __ASM_EMIT("vadd.f32    q9, q9, q5")

            __ASM_EMIT("tst         %[mask], $0x80")
            __ASM_EMIT("beq         6f")
            __ASM_EMIT("vst1.32     d15[1], [%[dst]]!")
            __ASM_EMIT("6:")
            __ASM_EMIT("vext.32     q1, q6, q7, $3")                        // q1    = s2[3] s2[4] s2[5] s2[6]
            __ASM_EMIT("vldm        %[vmask], {q12-q15}")                   // q12-q15 = vmask
            __ASM_EMIT("vext.32     q0, q6, q6, $3")                        // q0    = s2[3] s2[0] s2[1] s2[2]
            __ASM_EMIT("vbit        q2, q8, q14")                           // q2    = (d0 & ~vmask) | (d0' & vmask)
            __ASM_EMIT("vbit        q3, q9, q15")
            __ASM_EMIT("vbit        q4, q10, q14")                          // q4    = (d1 & ~vmask) | (d1' & vmask)
            __ASM_EMIT("vbit        q5, q11, q15")

            __ASM_EMIT("vext.32     q15, q14, q15, $3")                     // vmask = vmask << 1
            __ASM_EMIT("lsl         %[mask], $1")                           // mask  = mask << 1
            __ASM_EMIT("vext.32     q14, q13, q14, $3")                     // vmask = vmask << 1
            __ASM_EMIT("tst         %[mask], $0xff")                        // mask  == 0 ?
            __ASM_EMIT("vstm        %[vmask], {q12-q15}")
            __ASM_EMIT("bne         5b")

            // Store memory
            __ASM_EMIT("6:")
            __ASM_EMIT("vstm        %[d], {q2-q5}")
            __ASM_EMIT("8:")

            : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count),
              [mask] "=&r" (mask), [f] "+r" (f)
            : [d] "r" (d),
              [vmask] "r" (&vmask[0]),
              [X_MASK] "r" (&dyn_biquad_x8_mask[0])
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }
}

#endif /* DSP_ARCH_ARM_NEON_D32_FILTERS_DYNAMIC_H_ */
