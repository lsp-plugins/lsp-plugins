/*
 * complex.h
 *
 *  Created on: 2 сент. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_ARM_NEON_D32_COMPLEX_H_
#define DSP_ARCH_ARM_NEON_D32_COMPLEX_H_

namespace neon_d32
{
    void complex_mul(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t count)
    {
        __asm__ __volatile__
        (
            __ASM_EMIT("subs        %[count], $8")
            __ASM_EMIT("blo         2f")

            __ASM_EMIT("1:")
            __ASM_EMIT("vld1.32     {d0-d3}, [%[src_re]]!") // q0, q1 = sr
            __ASM_EMIT("vld1.32     {d4-d7}, [%[src_im]]!") // q2, q3 = si
            __ASM_EMIT("vld1.32     {d8-d11}, [%[dst_re]]") // q4, q5 = dr
            __ASM_EMIT("vld1.32     {d12-d15}, [%[dst_im]]") // q6, q7 = di
            __ASM_EMIT("vmul.f32    q8, q0, q4") // q8 = sr * si
            __ASM_EMIT("vmul.f32    q9, q1, q5")
            __ASM_EMIT("vmul.f32    q10, q2, q4") // q10 = si * dr
            __ASM_EMIT("vmul.f32    q11, q3, q5")
            __ASM_EMIT("vmls.f32    q8, q4, q6") // q8 = sr*si - dr*di
            __ASM_EMIT("vmls.f32    q9, q5, q7")
            __ASM_EMIT("vmla.f32    q10, q0, q6") // q10 = si * dr + sr * di
            __ASM_EMIT("vmla.f32    q11, q1, q7")
            __ASM_EMIT("vst1.32     {d16-d19}, [%[dst_re]]!")
            __ASM_EMIT("vst1.32     {d20-d23}, [%[dst_im]]!")

            __ASM_EMIT("subs        %[count], $8")
            __ASM_EMIT("bhs         1b")

            __ASM_EMIT("2:")
            __ASM_EMIT("adds        %[count], $7")
            __ASM_EMIT("blt         4f")

            __ASM_EMIT("3:")
            __ASM_EMIT("vldr        s0, [%[src_re]]") // s0 = sr
            __ASM_EMIT("vldr        s1, [%[src_im]]") // s1 = si
            __ASM_EMIT("vldr        s2, [%[dst_re]]") // s2 = dr
            __ASM_EMIT("vldr        s3, [%[dst_im]]") // s3 = di
            __ASM_EMIT("vmul.f32    s4, s0, s1") // s4 = sr * si
            __ASM_EMIT("vmul.f32    s5, s1, s2") // s5 = si * dr
            __ASM_EMIT("vmls.f32    s4, s2, s3") // s4 = sr*si - dr*di
            __ASM_EMIT("vmla.f32    s5, s0, s3") // q10 = si * dr + sr * di
            __ASM_EMIT("vstr        s4, [%[dst_re]]")
            __ASM_EMIT("vstr        s5, [%[dst_im]]")

            __ASM_EMIT("add         %[src_re], $4")
            __ASM_EMIT("add         %[src_im], $4")
            __ASM_EMIT("add         %[dst_re], $4")
            __ASM_EMIT("add         %[dst_im], $4")
            __ASM_EMIT("subs        %[count], $1")
            __ASM_EMIT("bge         1b")

            __ASM_EMIT("4:")

            : [dst_re] "+r" (dst_re), [dst_im] "+r" (dst_im),
              [src_re] "+r" (src_re), [src_im] "+r" (src_im),
              [count] "+r" (count)
            :
        );
    }
}


#endif /* DSP_ARCH_ARM_NEON_D32_COMPLEX_H_ */
