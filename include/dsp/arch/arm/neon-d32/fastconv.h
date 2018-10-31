/*
 * fastconv.h
 *
 *  Created on: 31 окт. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_ARM_NEON_D32_FASTCONV_H_
#define DSP_ARCH_ARM_NEON_D32_FASTCONV_H_

#ifndef DSP_ARCH_ARM_NEON_32_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_ARM_NEON_32_IMPL */

namespace neon_d32
{
    void fastconv_parse(float *dst, const float *src, size_t rank)
    {
        size_t items        = 1 << rank;

        IF_ARCH_ARM(
            const float *fdw    = &XFFT_DW[(rank - 3) << 3];
            const float *fw     = &XFFT_A[(rank - 3) << 4];
            const float *ps;
            size_t count;
            float *a, *b;
        );

        if (items >= 8)
        {
            ARCH_ARM_ASM(
                __ASM_EMIT("mov         %[a], %[dst]")                  // a    = dst
                __ASM_EMIT("mov         %[ps], %[src]")
                __ASM_EMIT("add         %[b], %[dst], %[rank], LSL $1") // b    = &dst[1 << (rank-1)]
                __ASM_EMIT("vldm        %[XFFT_A], {q8-q11}")           // q8   = wr0, q9 = wr1, q10 = wi0, q11 = wi1

                __ASM_EMIT("subs        %[count], %[items], $16")
                __ASM_EMIT("blo         2f")

                // 8x butterflies
                __ASM_EMIT("1:")
                __ASM_EMIT("vldm        %[ps]!, {q0-q1}")               // q0   = r0, q1 = r1
                __ASM_EMIT("veor        q2, q2")                        // q2   = 0
                __ASM_EMIT("vswp        q1, q2")                        // q1   = 0, q2 = r1
                __ASM_EMIT("vmul.f32    q1, q0, q10")                   // q1   = r0 * wi0
                __ASM_EMIT("vmul.f32    q0, q0, q8")                    // q0   = r0 * wr0
                __ASM_EMIT("vmvn.f32    q1, q1")                        // q1   = - r0 * wi0
                __ASM_EMIT("vmul.f32    q3, q2, q11")                   // q3   = r1 * wi1
                __ASM_EMIT("vmul.f32    q2, q2, q9")                    // q2   = r1 * wr1
                __ASM_EMIT("vmvn.f32    q3, q3")                        // q3   = - r1 * wi1
                __ASM_EMIT("vstm        %[a]!, {q0-q1}")
                __ASM_EMIT("vstm        %[b]!, {q2-q3}")
                // rotate angle
                __ASM_EMIT("vld1.32     {q0-q1}, [%[XFFT_DW]]")         // q0   = dr, q1 = di
                __ASM_EMIT("vmul.f32    q12, q8, q1")                   // q12  = wr0 * di
                __ASM_EMIT("vmul.f32    q13, q9, q1")                   // q13  = wr1 * di
                __ASM_EMIT("vmul.f32    q14, q10, q1")                  // q14  = wi0 * di
                __ASM_EMIT("vmul.f32    q15, q11, q1")                  // q15  = wi1 * di
                __ASM_EMIT("vmul.f32    q8, q8, q0")                    // q8   = wr0 * dr
                __ASM_EMIT("vmul.f32    q9, q9, q0")                    // q9   = wr1 * dr
                __ASM_EMIT("vmul.f32    q10, q10, q0")                  // q10  = wi0 * dr
                __ASM_EMIT("vmul.f32    q11, q11, q0")                  // q11  = wi1 * dr
                __ASM_EMIT("vsub.f32    q8, q8, q14")                   // q8   = wr0*dr - wi0*di
                __ASM_EMIT("vsub.f32    q9, q9, q15")                   // q9   = wr1*dr - wi1*di
                __ASM_EMIT("vadd.f32    q10, q10, q12")                 // q10  = wi0*dr + wr0*di
                __ASM_EMIT("vadd.f32    q11, q11, q13")                 // q11  = wi1*dr + wr1*di
                __ASM_EMIT("subs        %[count], $16")
                __ASM_EMIT("bhs         1b")

                // 4x butterfly
                __ASM_EMIT("adds        %[count], $8")
                __ASM_EMIT("blt         4f")

                __ASM_EMIT("vldm        %[ps]!, {q0}")                  // q0   = r0
                __ASM_EMIT("vmul.f32    q1, q0, q10")                   // q1   = r0 * wi0
                __ASM_EMIT("vmul.f32    q0, q0, q8")                    // q0   = r0 * wr0
                __ASM_EMIT("vmvn.f32    q1, q1")                        // q1   = - r0 * wi0
                __ASM_EMIT("vstm        %[a]!, {q0}")
                __ASM_EMIT("vstm        %[b]!, {q1}")

                __ASM_EMIT("4:")
                __ASM_EMIT("sub         %[XFFT_A], $0x40")              // fw  -= 16
                __ASM_EMIT("sub         %[XFFT_DW], $0x20")             // fdw -= 8

                : [ps] "=&r" (ps), [a] "=&r" (a),
                  [b] "=&r" (b),
                  [XFFT_A] "+r" (fw), [XFFT_DW] "+r" (fdw),
                  [count] "=&r" (count)
                : [src] "r" (src), [dst] "r" (dst), [rank] "r" (rank),
                  [items] "r" (items)
                : "cc", "memory",
                  "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
            );
        }
        else
        {
            ARCH_ARM_ASM(
                __ASM_EMIT("vld1.32     {q0}, [%[src]]")
                __ASM_EMIT("veor        q1, q1")
                __ASM_EMIT("vst1.32     {q0-q1}, [%[dst]]")
                :
                : [src] "r" (src), [dst] "r" (dst)
                : "memory",
                  "q0", "q1"
            )
        }
    }
}

#endif /* DSP_ARCH_ARM_NEON_D32_FASTCONV_H_ */
