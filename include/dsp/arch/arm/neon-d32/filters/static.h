/*
 * static.h
 *
 *  Created on: 12 окт. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_ARM_NEON_D32_FILTERS_STATIC_H_
#define DSP_ARCH_ARM_NEON_D32_FILTERS_STATIC_H_

namespace neon_d32
{
    void biquad_process_x1(float *dst, const float *src, size_t count, biquad_t *f)
    {
        ARCH_ARM_ASM
        (
            // Check count
            __ASM_EMIT("tst         %[count], %[count]")
            __ASM_EMIT("beq         2f")

            // Load permanent data
            __ASM_EMIT("vld1.32     {q1}, [%[FD]]")                         // q1   = d0 d1 0 0
            __ASM_EMIT("vld1.32     {q2-q3}, [%[FX1]]")                     // q2   = a0 a0 a1 a2, q3 = b1 b2 0 0

            // Start loop
            __ASM_EMIT("1:")
            __ASM_EMIT("vmov        s6, s5")                                // q1   = d0 d1 d1 0
            __ASM_EMIT("vld1.32     {d0[], d1[]}, [%[src]]!")               // q0   = s s s s
            __ASM_EMIT("vmov        s5, s4")                                // q1   = d0 d0 d1 0
            __ASM_EMIT("vmla.f32    q1, q0, q2")                            // q1   = s*a0+d0 s*a0+d0 s*a1+d1 s*a2
            __ASM_EMIT("vstm        %[dst]!, {s4}")                         // *dst = s*a0+d0
            __ASM_EMIT("vmul.f32    q4, q1, q3")                            // q4   = (s*a0+d0)*b1 (s*a0+d0)*b2 0 0
            __ASM_EMIT("vadd.f32    d8, d3")                                // q4   = (s*a0+d0)*b1+s*a1+d1 (s*a0+d0)*b2+s*a2 0 0
            __ASM_EMIT("subs        %[count], $1")
            __ASM_EMIT("vmov        q1, q4")                                // q1   = (s*a0+d0)*b1+s*a1+d1 (s*a0+d0)*b2+s*a2 0 0
            __ASM_EMIT("bne         1b")

            // Store the updated buffer state
            __ASM_EMIT("vst1.32     {q1}, [%[FD]]")
            __ASM_EMIT("2:")

            : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
            : [FD] "r" (&f->d[0]), [FX1] "r" (&f->x1)
            : "cc", "memory",
              "q0", "q1", "q2", "q3", "q4"
        );
    }

    void biquad_process_x2(float *dst, const float *src, size_t count, biquad_t *f)
    {
        IF_ARCH_ARM(biquad_x2_t *fx2 = &f->x2);

        ARCH_ARM_ASM
        (
            // Check count
            __ASM_EMIT("tst         %[count], %[count]")
            __ASM_EMIT("beq         4f")

            // Load permanent data
            __ASM_EMIT("vldm        %[FX2], {q4-q7}")                       // q4-q7 = { a0 a0 a1 a2, i0 i0 i1 i2, b1 b2 0 0, j1 j2 0 0 }
            __ASM_EMIT("vld1.32     {q2-q3}, [%[FD]]")                      // q2-q3 = { d0 d1 0 0, e0 e1 0 0 }

            // Peform 1x single A filter processing
            __ASM_EMIT("vmov        s10, s9")                               // q2   = d0 d1 d1 0
            __ASM_EMIT("vld1.32     {d0[], d1[]}, [%[src]]!")               // q0   = s s s s
            __ASM_EMIT("vmov        s9, s8")                                // q2   = d0 d0 d1 0
            __ASM_EMIT("vmla.f32    q2, q0, q4")                            // q2   = s*a0+d0 s*a0+d0 s*a1+d1 s*a2
            __ASM_EMIT("vmov        s4, s8")                                // q1   = s*a0+d0 ? ? ? = r ? ? ?
            __ASM_EMIT("vmul.f32    q8, q2, q6")                            // q8   = (s*a0+d0)*b1 (s*a0+d0)*b2 0 0
            __ASM_EMIT("vadd.f32    d16, d5")                               // q8   = (s*a0+d0)*b1+s*a1+d1 (s*a0+d0)*b2+s*a2 0 0
            __ASM_EMIT("vmov        q2, q8")                                // q2   = (s*a0+d0)*b1+s*a1+d1 (s*a0+d0)*b2+s*a2 0 0
            __ASM_EMIT("subs        %[count], $1")
            __ASM_EMIT("bls         2f")

            // Perform 2x A+B filter processing
            __ASM_EMIT("1:")
            __ASM_EMIT("vmov        s5, s4")                                // q1   = r r ? ?
            __ASM_EMIT("vmov        s10, s9")                               // q2   = d0 d1 d1 0
            __ASM_EMIT("vmov        s14, s13")                              // q3   = j0 j1 j1 0
            __ASM_EMIT("vmov        d3, d2")                                // q1   = r r r r
            __ASM_EMIT("vld1.32     {d0[], d1[]}, [%[src]]!")               // q0   = s s s s
            __ASM_EMIT("vmov        s9, s8")                                // q2   = d0 d0 d1 0
            __ASM_EMIT("vmov        s13, s12")                              // q3   = j0 j0 j1 0
            __ASM_EMIT("vmla.f32    q2, q0, q4")                            // q2   = s*a0+d0 s*a0+d0 s*a1+d1 s*a2
            __ASM_EMIT("vmla.f32    q3, q1, q5")                            // q3   = r*i0+e0 r*i0+e0 r*i1+e1 r*i2
            __ASM_EMIT("vmov        s4, s8")                                // q1   = s*a0+d0 ? ? ? = r ? ? ?
            __ASM_EMIT("vstm        %[dst]!, {s12}")                        // *dst = r*i0+e0
            __ASM_EMIT("vmul.f32    q8, q2, q6")                            // q8   = (s*a0+d0)*b1 (s*a0+d0)*b2 0 0
            __ASM_EMIT("vmul.f32    q9, q3, q7")                            // q9   = (r*i0+e0)*j1 (r*i0+e0)*j2 0 0
            __ASM_EMIT("vadd.f32    d16, d5")                               // q8   = (s*a0+d0)*b1+s*a1+d1 (s*a0+d0)*b2+s*a2 0 0
            __ASM_EMIT("vadd.f32    d18, d7")                               // q9   = (r*i0+e0)*j1+r*i1+e1 (r*i0+e0)*j2+r*i2 0 0
            __ASM_EMIT("vmov        q2, q8")                                // q2   = (s*a0+d0)*b1+s*a1+d1 (s*a0+d0)*b2+s*a2 0 0
            __ASM_EMIT("vmov        q3, q9")                                // q3   = (r*i0+e0)*j1+r*i1+e1 (r*i0+e0)*j2+r*i2 0 0
            __ASM_EMIT("subs        %[count], $1")
            __ASM_EMIT("bhi         1b")

            // Peform 1x single B filter processing
            __ASM_EMIT("2:")
            __ASM_EMIT("vmov        s5, s4")                                // q1   = r r ? ?
            __ASM_EMIT("vmov        s14, s13")                              // q3   = j0 j1 j1 0
            __ASM_EMIT("vmov        d3, d2")                                // q1   = r r r r
            __ASM_EMIT("vmov        s13, s12")                              // q3   = j0 j0 j1 0
            __ASM_EMIT("vmla.f32    q3, q1, q5")                            // q3   = r*i0+e0 r*i0+e0 r*i1+e1 r*i2
            __ASM_EMIT("vstm        %[dst]!, {s12}")                        // *dst = r*i0+e0
            __ASM_EMIT("vmul.f32    q9, q3, q7")                            // q9   = (r*i0+e0)*j1 (r*i0+e0)*j2 0 0
            __ASM_EMIT("vadd.f32    d18, d7")                               // q9   = (r*i0+e0)*j1+r*i1+e1 (r*i0+e0)*j2+r*i2 0 0
            __ASM_EMIT("vmov        q3, q9")                                // q3   = (r*i0+e0)*j1+r*i1+e1 (r*i0+e0)*j2+r*i2 0 0

            // Store the updated buffer state
            __ASM_EMIT("vst1.32     {q2-q3}, [%[FD]]")
            __ASM_EMIT("4:")

            : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
            : [FD] "r" (&f->d[0]), [FX2] "r" (fx2)
            : "cc", "memory",
              "q0", "q1", "q2", "q3", "q4"
        );
    }

    static const uint32_t biquad_x4_mask[8] __lsp_aligned16 =
    {
        0xffffffff, 0x00000000, 0x00000000, 0x00000000,
        0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff
    };

    void biquad_process_x4(float *dst, const float *src, size_t count, biquad_t *f)
    {
        IF_ARCH_ARM(
            biquad_x4_t *fx4 = &f->x4;
            size_t mask;
        );

        ARCH_ARM_ASM
        (
            __ASM_EMIT("tst         %[count], %[count]")
            __ASM_EMIT("beq         8f")

            // Prepare
            __ASM_EMIT("vldm        %[FD], {q1-q2}")                        // q1-q2 = { d0, d1 }
            __ASM_EMIT("vldm        %[FX4], {q3-q7}")                       // q3-q7 = { a0, a1, a2, b1, b2 }
            __ASM_EMIT("vldm        %[X_MASK], {q8-q9}")                    // q8-q9 = { vmask, 1 }
            __ASM_EMIT("mov         %[mask], $1")                           // mask  = 1

            // Do pre-loop
            __ASM_EMIT("1:")
            __ASM_EMIT("vldm        %[src]!, {s0}")                         // q0    = s
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
            __ASM_EMIT("vldm        %[src]!, {s0}")                         // q0    = s
            __ASM_EMIT("vmul.f32    q10, q3, q0")                           // q10   = a0*s
            __ASM_EMIT("vmul.f32    q11, q4, q0")                           // q11   = a1*s
            __ASM_EMIT("vadd.f32    q10, q1")                               // q10   = a0*s + d0 = s2
            __ASM_EMIT("vmul.f32    q12, q5, q0")                           // q12   = a2*s
            __ASM_EMIT("vmla.f32    q11, q6, q10")                          // q11   = a1*s + b1*s2
            __ASM_EMIT("vmla.f32    q12, q7, q10")                          // q12   = d1' = a2*s + b2*s2
            __ASM_EMIT("vadd.f32    q11, q2")                               // q11   = d0' = d1 + a1*s + b1*s2
            __ASM_EMIT("vext.32     q0, q10, q10, $3")                      // q0    = s2[3] s2[0] s2[1] s2[2]
            __ASM_EMIT("vmov        q2, q12")                               // q12   = d1'
            __ASM_EMIT("vmov        q1, q11")                               // q11   = d0'
            __ASM_EMIT("vstm        %[dst]!, {s0}")
            __ASM_EMIT("subs        %[count], $1")
            __ASM_EMIT("bne         3b")

            // Do post-loop
            __ASM_EMIT("4:")
            __ASM_EMIT("veor        q9, q9")                                // q9    = 0
            __ASM_EMIT("lsl         %[mask], $1")                           // mask  = mask << 1
            __ASM_EMIT("vext.32     q8, q9, q8, $3")                        // q8    = (vmask << 1) | 0

            __ASM_EMIT("5:")
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
            __ASM_EMIT("vstm        %[dst]!, {s0}")
            __ASM_EMIT("6:")
            __ASM_EMIT("vbit        q2, q12, q8")                           // q2    = (d1 & ~vmask) | (d1' & vmask)
            __ASM_EMIT("vbit        q1, q11, q8")                           // q1    = (d0 & ~vmask) | (d0' & vmask)
            __ASM_EMIT("lsl         %[mask], $1")                           // mask  = mask << 1
            __ASM_EMIT("vext.32     q8, q9, q8, $3")                        // q8    = vmask << 1
            __ASM_EMIT("tst         %[mask], $0x0f")                        // mask  == 0 ?
            __ASM_EMIT("bne         5b")

            // Store memory
            __ASM_EMIT("6:")
            __ASM_EMIT("vstm        %[FD], {q1-q2}")
            __ASM_EMIT("8:")

            : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count),
              [mask] "=&r" (mask)
            : [FD] "r" (&f->d[0]), [FX4] "r" (fx4),
              [X_MASK] "r" (&biquad_x4_mask[0])
            : "cc", "memory",
              "q0", "q1", "q2", "q3", "q4"
        );
    }
}

#endif /* DSP_ARCH_ARM_NEON_D32_FILTERS_STATIC_H_ */
