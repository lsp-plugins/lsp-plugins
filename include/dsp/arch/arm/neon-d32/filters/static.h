/*
 * static.h
 *
 *  Created on: 12 окт. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_ARM_NEON_D32_FILTERS_STATIC_H_
#define DSP_ARCH_ARM_NEON_D32_FILTERS_STATIC_H_

#ifndef DSP_ARCH_ARM_NEON_32_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_ARM_NEON_32_IMPL */

namespace neon_d32
{
    void biquad_process_x1(float *dst, const float *src, size_t count, biquad_t *f)
    {
        ARCH_ARM_ASM
        (
            // Check count
            __ASM_EMIT("tst         %[count], %[count]")
            __ASM_EMIT("beq         6f")

            // Load permanent data
            __ASM_EMIT("vld1.32     {q1}, [%[FD]]")                         // q1       = d0 d1 0 0
            __ASM_EMIT("vld1.32     {q2-q3}, [%[FX1]]")                     // q2       = a0 a0 a1 a2, q3 = b1 b2 0 0

            // X2 loop (unrolled)
            __ASM_EMIT("subs        %[count], $2")
            __ASM_EMIT("blt         2f")

            __ASM_EMIT("1:")
            __ASM_EMIT("vext.f32    q4, q1, q1, $3")                        // q4       = 0 d0 d1 0
            __ASM_EMIT("vld1.32     {d0[], d1[]}, [%[src]]!")               // q0       = s s s s
            __ASM_EMIT("vdup.32     d8, d8[1]")                             // q4       = d0 d0 d1 0
            __ASM_EMIT("vmla.f32    q4, q0, q2")                            // q4       = s*a0+d0 s*a0+d0 s*a1+d1 s*a2
            __ASM_EMIT("vst1.32     d8[0], [%[dst]]!")                      // *dst++   = s*a0+d0
            __ASM_EMIT("vmul.f32    q1, q4, q3")                            // q1       = (s*a0+d0)*b1 (s*a0+d0)*b2 0 0
            __ASM_EMIT("vadd.f32    d2, d9")                                // q1       = (s*a0+d0)*b1+s*a1+d1 (s*a0+d0)*b2+s*a2 0 0

            __ASM_EMIT("vext.f32    q4, q1, q1, $3")                        // q4       = 0 d0 d1 0
            __ASM_EMIT("vld1.32     {d0[], d1[]}, [%[src]]!")               // q0       = s s s s
            __ASM_EMIT("vdup.32     d8, d8[1]")                             // q4       = d0 d0 d1 0
            __ASM_EMIT("vmla.f32    q4, q0, q2")                            // q4       = s*a0+d0 s*a0+d0 s*a1+d1 s*a2
            __ASM_EMIT("vst1.32     d8[0], [%[dst]]!")                      // *dst++   = s*a0+d0
            __ASM_EMIT("vmul.f32    q1, q4, q3")                            // q1       = (s*a0+d0)*b1 (s*a0+d0)*b2 0 0
            __ASM_EMIT("vadd.f32    d2, d9")                                // q1       = (s*a0+d0)*b1+s*a1+d1 (s*a0+d0)*b2+s*a2 0 0

            __ASM_EMIT("subs        %[count], $2")
            __ASM_EMIT("bge         1b")

            // X1 operation:
            __ASM_EMIT("2:")
            __ASM_EMIT("adds        %[count], $1")
            __ASM_EMIT("blt         4f")

            __ASM_EMIT("vext.f32    q4, q1, q1, $3")                        // q4       = 0 d0 d1 0
            __ASM_EMIT("vld1.32     {d0[], d1[]}, [%[src]]")                // q0       = s s s s
            __ASM_EMIT("vdup.32     d8, d8[1]")                             // q4       = d0 d0 d1 0
            __ASM_EMIT("vmla.f32    q4, q0, q2")                            // q4       = s*a0+d0 s*a0+d0 s*a1+d1 s*a2
            __ASM_EMIT("vst1.32     d8[0], [%[dst]]")                       // *dst++   = s*a0+d0
            __ASM_EMIT("vmul.f32    q1, q4, q3")                            // q1       = (s*a0+d0)*b1 (s*a0+d0)*b2 0 0
            __ASM_EMIT("vadd.f32    d2, d9")                                // q1       = (s*a0+d0)*b1+s*a1+d1 (s*a0+d0)*b2+s*a2 0 0

            // Store the updated buffer state
            __ASM_EMIT("4:")
            __ASM_EMIT("vst1.32     {q1}, [%[FD]]")
            __ASM_EMIT("6:")

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
            __ASM_EMIT("beq         6f")

            // Load permanent data
            __ASM_EMIT("vldm        %[FX2], {q4-q7}")                       // q4-q7    = { a0 a0 a1 a2, i0 i0 i1 i2, b1 b2 0 0, j1 j2 0 0 }
            __ASM_EMIT("vld1.32     {q2-q3}, [%[FD]]")                      // q2-q3    = { d0 d1 0 0, e0 e1 0 0 }

            // Peform 1x single A filter processing
            __ASM_EMIT("vext.f32    q8, q2, q2, $3")                        // q8       = 0 d0 d1 0
            __ASM_EMIT("vld1.32     {d0[], d1[]}, [%[src]]!")               // q0       = s s s s
            __ASM_EMIT("vdup.32     d16, d16[1]")                           // q8       = d0 d0 d1 0
            __ASM_EMIT("vmla.f32    q8, q0, q4")                            // q8       = s*a0+d0 s*a0+d0 s*a1+d1 s*a2
            __ASM_EMIT("vdup.32     q1, d16[0]")                            // q1       = s*a0+d0 = r
            __ASM_EMIT("vmul.f32    q2, q8, q6")                            // q2       = (s*a0+d0)*b1 (s*a0+d0)*b2 0 0
            __ASM_EMIT("vadd.f32    d4, d17")                               // q2       = (s*a0+d0)*b1+s*a1+d1 (s*a0+d0)*b2+s*a2 0 0
            __ASM_EMIT("subs        %[count], $1")
            __ASM_EMIT("bls         2f")

            // Perform 2x A+B filter processing (loop unrolling)
            __ASM_EMIT("subs        %[count], $2")
            __ASM_EMIT("blt         2f")
            __ASM_EMIT("1:")

            __ASM_EMIT("vext.f32    q8, q2, q2, $3")                        // q8       = 0 d0 d1 0
            __ASM_EMIT("vext.f32    q9, q3, q3, $3")                        // q9       = 0 j0 j1 0
            __ASM_EMIT("vld1.32     {d0[], d1[]}, [%[src]]!")               // q0       = s s s s
            __ASM_EMIT("vdup.32     d16, d16[1]")                           // q8       = d0 d0 d1 0
            __ASM_EMIT("vdup.32     d18, d18[1]")                           // q9       = j0 j0 j1 0
            __ASM_EMIT("vmla.f32    q8, q0, q4")                            // q8       = s*a0+d0 s*a0+d0 s*a1+d1 s*a2
            __ASM_EMIT("vmla.f32    q9, q1, q5")                            // q9       = r*i0+e0 r*i0+e0 r*i1+e1 r*i2
            __ASM_EMIT("vdup.32     q1, d16[0]")                            // q1       = s*a0+d0 = r
            __ASM_EMIT("vst1.32     d18[0], [%[dst]]!")                     // *dst++   = r*i0+e0
            __ASM_EMIT("vmul.f32    q2, q8, q6")                            // q2       = (s*a0+d0)*b1 (s*a0+d0)*b2 0 0
            __ASM_EMIT("vmul.f32    q3, q9, q7")                            // q3       = (r*i0+e0)*j1 (r*i0+e0)*j2 0 0
            __ASM_EMIT("vadd.f32    d4, d17")                               // q2       = (s*a0+d0)*b1+s*a1+d1 (s*a0+d0)*b2+s*a2 0 0
            __ASM_EMIT("vadd.f32    d6, d19")                               // q3       = (r*i0+e0)*j1+r*i1+e1 (r*i0+e0)*j2+r*i2 0 0

            __ASM_EMIT("vext.f32    q8, q2, q2, $3")                        // q8       = 0 d0 d1 0
            __ASM_EMIT("vext.f32    q9, q3, q3, $3")                        // q9       = 0 j0 j1 0
            __ASM_EMIT("vld1.32     {d0[], d1[]}, [%[src]]!")               // q0       = s s s s
            __ASM_EMIT("vdup.32     d16, d16[1]")                           // q8       = d0 d0 d1 0
            __ASM_EMIT("vdup.32     d18, d18[1]")                           // q9       = j0 j0 j1 0
            __ASM_EMIT("vmla.f32    q8, q0, q4")                            // q8       = s*a0+d0 s*a0+d0 s*a1+d1 s*a2
            __ASM_EMIT("vmla.f32    q9, q1, q5")                            // q9       = r*i0+e0 r*i0+e0 r*i1+e1 r*i2
            __ASM_EMIT("vdup.32     q1, d16[0]")                            // q1       = s*a0+d0 = r
            __ASM_EMIT("vst1.32     d18[0], [%[dst]]!")                     // *dst++   = r*i0+e0
            __ASM_EMIT("vmul.f32    q2, q8, q6")                            // q2       = (s*a0+d0)*b1 (s*a0+d0)*b2 0 0
            __ASM_EMIT("vmul.f32    q3, q9, q7")                            // q3       = (r*i0+e0)*j1 (r*i0+e0)*j2 0 0
            __ASM_EMIT("vadd.f32    d4, d17")                               // q2       = (s*a0+d0)*b1+s*a1+d1 (s*a0+d0)*b2+s*a2 0 0
            __ASM_EMIT("vadd.f32    d6, d19")                               // q3       = (r*i0+e0)*j1+r*i1+e1 (r*i0+e0)*j2+r*i2 0 0

            __ASM_EMIT("subs        %[count], $2")
            __ASM_EMIT("bge         1b")

            // Perform A+B filter processing
            __ASM_EMIT("2:")
            __ASM_EMIT("adds        %[count], $1")
            __ASM_EMIT("blt         4f")
            __ASM_EMIT("vext.f32    q8, q2, q2, $3")                        // q8       = 0 d0 d1 0
            __ASM_EMIT("vext.f32    q9, q3, q3, $3")                        // q9       = 0 j0 j1 0
            __ASM_EMIT("vld1.32     {d0[], d1[]}, [%[src]]!")               // q0       = s s s s
            __ASM_EMIT("vdup.32     d16, d16[1]")                           // q8       = d0 d0 d1 0
            __ASM_EMIT("vdup.32     d18, d18[1]")                           // q9       = j0 j0 j1 0
            __ASM_EMIT("vmla.f32    q8, q0, q4")                            // q8       = s*a0+d0 s*a0+d0 s*a1+d1 s*a2
            __ASM_EMIT("vmla.f32    q9, q1, q5")                            // q9       = r*i0+e0 r*i0+e0 r*i1+e1 r*i2
            __ASM_EMIT("vdup.32     q1, d16[0]")                            // q1       = s*a0+d0 = r
            __ASM_EMIT("vst1.32     d18[0], [%[dst]]!")                     // *dst++   = r*i0+e0
            __ASM_EMIT("vmul.f32    q2, q8, q6")                            // q2       = (s*a0+d0)*b1 (s*a0+d0)*b2 0 0
            __ASM_EMIT("vmul.f32    q3, q9, q7")                            // q3       = (r*i0+e0)*j1 (r*i0+e0)*j2 0 0
            __ASM_EMIT("vadd.f32    d4, d17")                               // q2       = (s*a0+d0)*b1+s*a1+d1 (s*a0+d0)*b2+s*a2 0 0
            __ASM_EMIT("vadd.f32    d6, d19")                               // q3       = (r*i0+e0)*j1+r*i1+e1 (r*i0+e0)*j2+r*i2 0 0

            // Peform 1x single B filter processing
            __ASM_EMIT("4:")
            __ASM_EMIT("vext.f32    q9, q3, q3, $3")                        // q9       = 0 j0 j1 0
            __ASM_EMIT("vdup.32     d18, d18[1]")                           // q9       = j0 j0 j1 0
            __ASM_EMIT("vmla.f32    q9, q1, q5")                            // q9       = r*i0+e0 r*i0+e0 r*i1+e1 r*i2
            __ASM_EMIT("vst1.32     d18[0], [%[dst]]!")                     // *dst++   = r*i0+e0
            __ASM_EMIT("vmul.f32    q3, q9, q7")                            // q3       = (r*i0+e0)*j1 (r*i0+e0)*j2 0 0
            __ASM_EMIT("vadd.f32    d6, d19")                               // q3       = (r*i0+e0)*j1+r*i1+e1 (r*i0+e0)*j2+r*i2 0 0

            // Store the updated buffer state
            __ASM_EMIT("vst1.32     {q2-q3}, [%[FD]]")
            __ASM_EMIT("6:")

            : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
            : [FD] "r" (&f->d[0]), [FX2] "r" (fx2)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12"
        );
    }

    static const uint32_t biquad_x4_mask[8] __lsp_aligned16 =
    {
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
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
            __ASM_EMIT("beq         10f")

            // Prepare
            __ASM_EMIT("vldm        %[FD], {q8-q9}")                        // q8-q9 = { d0, d1 }
            __ASM_EMIT("vldm        %[FX4], {q3-q7}")                       // q3-q7 = { a0, a1, a2, b1, b2 }
            __ASM_EMIT("vldm        %[X_MASK], {q10-q11}")                  // q10-q11 = { vmask, 1 }
            __ASM_EMIT("mov         %[mask], $1")                           // mask  = 1

            // Do pre-loop
            __ASM_EMIT("1:")
            __ASM_EMIT("orr         %[mask], %[mask], LSL $1")              // mask  = (mask << 1) | 1
            __ASM_EMIT("vext.32     q0, q0, q0, $3")                        // q0    = s' = s[3] s[0] s[1] s[2]
            __ASM_EMIT("vext.32     q10, q11, q10, $3")                     // q10   = (vmask << 1) | 1
            __ASM_EMIT("vld1.32     d0[0], [%[src]]!")                      // q0    = s
            __ASM_EMIT("vmul.f32    q13, q3, q0")                           // q13   = a0*s
            __ASM_EMIT("vmul.f32    q14, q4, q0")                           // q14   = a1*s
            __ASM_EMIT("vmul.f32    q15, q5, q0")                           // q15   = a2*s
            __ASM_EMIT("vadd.f32    q0, q13, q8")                           // q0    = a0*s + d0 = s2
            __ASM_EMIT("vmla.f32    q14, q6, q0")                           // q14   = a1*s + b1*s2
            __ASM_EMIT("vmla.f32    q15, q7, q0")                           // q15   = d1' = a2*s + b2*s2
            __ASM_EMIT("vadd.f32    q14, q9")                               // q14   = d0' = d1 + a1*s + b1*s2
            __ASM_EMIT("vbit        q9, q15, q10")                          // q9    = (d1 & ~vmask) | (d1' & vmask)
            __ASM_EMIT("vbit        q8, q14, q10")                          // q8    = (d0 & ~vmask) | (d0' & vmask)
            __ASM_EMIT("subs        %[count], $1")
            __ASM_EMIT("beq         6f")
            __ASM_EMIT("cmp         %[mask], $0x1f")
            __ASM_EMIT("bne         1b")

            // Do main loop
            __ASM_EMIT("5:")
            __ASM_EMIT("vext.32     q0, q0, q0, $3")                        // q0    = s' = s[3] s[0] s[1] s[2]
            __ASM_EMIT("vst1.32     d0[0], [%[dst]]!")
            __ASM_EMIT("vld1.32     d0[0], [%[src]]!")
            __ASM_EMIT("vmul.f32    q13, q3, q0")                           // q13   = a0*s
            __ASM_EMIT("vmul.f32    q14, q4, q0")                           // q14   = a1*s
            __ASM_EMIT("vmul.f32    q15, q5, q0")                           // q15   = a2*s
            __ASM_EMIT("vadd.f32    q0, q13, q8")                           // q0    = a0*s + d0 = s2
            __ASM_EMIT("vmla.f32    q14, q6, q0")                           // q14   = a1*s + b1*s2
            __ASM_EMIT("vmla.f32    q15, q7, q0")                           // q15   = d1' = a2*s + b2*s2
            __ASM_EMIT("vadd.f32    q8, q14, q9")                           // q8    = d0' = d1 + a1*s + b1*s2
            __ASM_EMIT("vmov        q9, q15")
            __ASM_EMIT("subs        %[count], $1")
            __ASM_EMIT("bne         5b")

            // Do post-loop
            __ASM_EMIT("6:")
            __ASM_EMIT("veor        q11, q11")                              // q11    = 0
            __ASM_EMIT("eor         %[mask], $1")                           // reset flag

            __ASM_EMIT("7:")
            __ASM_EMIT("lsl         %[mask], $1")                           // mask  = mask << 1
            __ASM_EMIT("vext.32     q0, q0, q0, $3")                        // q0    = s' = s[3] s[0] s[1] s[2]
            __ASM_EMIT("vext.32     q10, q11, q10, $3")                     // q10    = (vmask << 1) | 0
            __ASM_EMIT("tst         %[mask], $0x20")                        // Need to emit?
            __ASM_EMIT("beq         8f")
            __ASM_EMIT("vst1.32     d0[0], [%[dst]]!")
            __ASM_EMIT("8:")

            __ASM_EMIT("vmul.f32    q13, q3, q0")                           // q13   = a0*s
            __ASM_EMIT("vmul.f32    q14, q4, q0")                           // q14   = a1*s
            __ASM_EMIT("vmul.f32    q15, q5, q0")                           // q15   = a2*s
            __ASM_EMIT("vadd.f32    q0, q13, q8")                           // q0    = a0*s + d0 = s2
            __ASM_EMIT("vmla.f32    q14, q6, q0")                           // q14   = a1*s + b1*s2
            __ASM_EMIT("vmla.f32    q15, q7, q0")                           // q15   = d1' = a2*s + b2*s2
            __ASM_EMIT("vadd.f32    q14, q9")                               // q14   = d0' = d1 + a1*s + b1*s2
            __ASM_EMIT("vbit        q9, q15, q10")                          // q9    = (d1 & ~vmask) | (d1' & vmask)
            __ASM_EMIT("vbit        q8, q14, q10")                          // q8    = (d0 & ~vmask) | (d0' & vmask)

            __ASM_EMIT("tst         %[mask], $0x1e")
            __ASM_EMIT("bne         7b")

            // Store memory
            __ASM_EMIT("vstm        %[FD], {q8-q9}")
            __ASM_EMIT("10:")

            : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count),
              [mask] "=&r" (mask)
            : [FD] "r" (&f->d[0]), [FX4] "r" (fx4),
              [X_MASK] "r" (&biquad_x4_mask[0])
            : "cc", "memory",
              "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q13", "q14", "q15"
        );
    }

    static const uint32_t biquad_x8_mask[16] __lsp_aligned16 =
    {
        0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
        0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
        0xffffffff, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
    };

    void biquad_process_x8(float *dst, const float *src, size_t count, biquad_t *f)
    {
        IF_ARCH_ARM(
            float *fx8a = f->x8.a0;
            float *fx8b = f->x8.b1;
            float vmask[16] __lsp_aligned16;
            size_t mask;
        );

        ARCH_ARM_ASM
        (
            __ASM_EMIT("tst         %[count], %[count]")
            __ASM_EMIT("beq         8f")

            // Prepare
            __ASM_EMIT("vldm        %[FD], {q2-q5}")                        // q2-q3 = d0, q4-q5 = d1
            __ASM_EMIT("vldm        %[X_MASK], {q12-q15}")                  // q12-q15 = vmask
            __ASM_EMIT("mov         %[mask], $1")                           // mask  = 1
            __ASM_EMIT("vstm        %[vmask], {q12-q15}")

            // Do pre-loop
            __ASM_EMIT("1:")
            __ASM_EMIT("vldm        %[FX8A], {q6-q11}")                     // q6-q7 = a0, q8-q9 = a1, q10-q11 = a2
            __ASM_EMIT("vld1.32     d0[0], [%[src]]!")                      // q0    = s
            __ASM_EMIT("vldm        %[FX8B], {q12-q15}")                    // q12-q13 = b1, q14-q15 = b2

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
            __ASM_EMIT("vldm        %[FX8A], {q6-q11}")                     // q6-q7 = a0, q8-q9 = a1, q10-q11 = a2
            __ASM_EMIT("vld1.32     d0[0], [%[src]]!")                      // q0    = s
            __ASM_EMIT("vldm        %[FX8B], {q12-q15}")                    // q12-q13 = b1, q14-q15 = b2

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
            __ASM_EMIT("vldm        %[FX8A], {q6-q11}")                     // q6-q7 = a0, q8-q9 = a1, q10-q11 = a2
            __ASM_EMIT("vldm        %[FX8B], {q12-q15}")                    // q12-q13 = b1, q14-q15 = b2
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
            __ASM_EMIT("vstm        %[FD], {q2-q5}")
            __ASM_EMIT("8:")

            : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count),
              [mask] "=&r" (mask)
            : [FD] "r" (&f->d[0]), [FX8A] "r" (fx8a), [FX8B] "r" (fx8b),
              [vmask] "r" (&vmask[0]),
              [X_MASK] "r" (&biquad_x8_mask[0])
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }
}

#endif /* DSP_ARCH_ARM_NEON_D32_FILTERS_STATIC_H_ */
