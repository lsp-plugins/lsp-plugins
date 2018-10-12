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
            __ASM_EMIT("cmp         %[count], $0")
            __ASM_EMIT("beq         2f")

            // Load permanent data
            __ASM_EMIT("vld1.32     {q1}, [%[FD]]")                         // q1   = d0 d1 0 0
            __ASM_EMIT("vld1.32     {q2-q3}, [%[FX1]]")                     // q2   = a0 a0 a1 a2, q3 = b1 b2 0 0

            // Start loop
            __ASM_EMIT("1:")
            __ASM_EMIT("vmov        s6, s5")                                // q1   = d0 d1 d1 0
            __ASM_EMIT("vld1.32     {d0[], d1[]}, [%[src]]!")               // q0   = s s s s
            __ASM_EMIT("vmov        s5, s4")                                // q1   = d0 d0 d1 0
            __ASM_EMIT("vmla.f32    q4, q0, q2")                            // q4   = s*a0+d0 s*a0+d0 s*a1+d1 s*a2
            __ASM_EMIT("vstr        s16, %[dst]!")                          // *dst = s*a0+d0
            __ASM_EMIT("vmul.f32    q1, q4, d3")                            // q1   = (s*a0+d0)*b1 (s*a0+d0)*b2 0 0
            __ASM_EMIT("vadd.f32    d2, d9")                                // q1   = (s*a0+d0)*b1+s*a1+d1 (s*a0+d0)*b2+s*a2 0 0
            __ASM_EMIT("subs        %[count], $1")
            __ASM_EMIT("bne         1b")

            // Store the updated buffer state
            __ASM_EMIT("vst1.32     {d2}, [%[FD]]")
            __ASM_EMIT("2:")

            : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
            : [FD] "r" (f->d), [FX1] "r" (&f->x1)
            : "cc", "memory",
              "q0", "q1", "q2", "q3", "q4"
        );
    }
}

#endif /* DSP_ARCH_ARM_NEON_D32_FILTERS_STATIC_H_ */
