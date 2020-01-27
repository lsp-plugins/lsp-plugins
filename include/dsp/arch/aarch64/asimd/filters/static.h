/*
 * static.h
 *
 *  Created on: 27 янв. 2020 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_AARCH64_ASIMD_FILTERS_STATIC_H_
#define DSP_ARCH_AARCH64_ASIMD_FILTERS_STATIC_H_

#ifndef DSP_ARCH_AARCH64_ASIMD_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_AARCH64_ASIMD_IMPL */

namespace asimd
{
    void biquad_process_x1(float *dst, const float *src, size_t count, biquad_t *f)
    {
        // s'    = a0*s + d0;
        // d0'   = d1 + a1*s + b1*s';
        // d1'   = a2*s + b2*s';
        ARCH_AARCH64_ASM(
            // Check count
            __ASM_EMIT("cbz             %[count], 6f")
            __ASM_EMIT("ldp             s16, s17, [%[FX1], #0x00]")             // v16  = a0, v17 = a0
            __ASM_EMIT("ldp             s18, s19, [%[FX1], #0x08]")             // v18  = a1, v19 = a2
            __ASM_EMIT("ldp             s20, s21, [%[FX1], #0x10]")             // v20  = b1, v21 = b2
            __ASM_EMIT("ldp             s22, s23, [%[FD]]")                     // v22  = d0, v23 = d1
            // x2 blocks
            __ASM_EMIT("subs            %[count], %[count], #2")
            __ASM_EMIT("b.lt            2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("ldp             s0, s1, [%[src]]")                      // v0   = s0, v1 = s1
            __ASM_EMIT("fmadd           s2, s16, s0, s22")                      // v2   = s' = a0*s0+d0
            __ASM_EMIT("fmul            s4, s18, s0")                           // v4   = a1*s0
            __ASM_EMIT("fmadd           s6, s20, s2, s23")                      // v6   = b1*s' + d1
            __ASM_EMIT("fmul            s0, s19, s0")                           // v0   = a2*s0
            __ASM_EMIT("fadd            s22, s4, s6")                           // v22  = d0' = d1 + a1*s0 + b1*s'
            __ASM_EMIT("fmadd           s23, s21, s2, s0")                      // v23  = d1' = a2*s0 + b2*s'

            __ASM_EMIT("fmadd           s3, s16, s1, s22")                      // v3   = s' = a0*s1+d0
            __ASM_EMIT("fmul            s5, s18, s1")                           // v5   = a1*s1
            __ASM_EMIT("fmadd           s7, s20, s3, s23")                      // v7   = b1*s3 + d1
            __ASM_EMIT("fmul            s1, s19, s1")                           // v1   = a2*s1
            __ASM_EMIT("fadd            s22, s5, s7")                           // v22  = d0' = d1 + a1*s1 + b1*s3
            __ASM_EMIT("fmadd           s23, s21, s3, s1")                      // v23  = d1' = a2*s1 + b2*s0

            __ASM_EMIT("stp             s2, s3, [%[dst]]")
            __ASM_EMIT("subs            %[count], %[count], #2")
            __ASM_EMIT("add             %[src], %[src], #0x08")
            __ASM_EMIT("add             %[dst], %[dst], #0x08")
            __ASM_EMIT("b.ge            1b")
            __ASM_EMIT("2:")
            // X1 block:
            __ASM_EMIT("adds            %[count], %[count], #1")
            __ASM_EMIT("b.lt            4f")
            __ASM_EMIT("ldr             s0, [%[src]]")                          // v0   = s0
            __ASM_EMIT("fmadd           s2, s16, s0, s22")                      // v2   = s' = a0*s0+d0
            __ASM_EMIT("fmul            s4, s18, s0")                           // v4   = a1*s0
            __ASM_EMIT("fmadd           s6, s20, s2, s23")                      // v6   = b1*s' + d1
            __ASM_EMIT("fmul            s0, s19, s0")                           // v0   = a2*s0
            __ASM_EMIT("fadd            s22, s4, s6")                           // v22  = d0' = d1 + a1*s0 + b1*s'
            __ASM_EMIT("fmadd           s23, s21, s2, s0")                      // v23  = d1' = a2*s0 + b2*s'
            __ASM_EMIT("str             s2, [%[dst]]")
            __ASM_EMIT("4:")
            // Store the updated buffer state
            __ASM_EMIT("stp             s22, s23, [%[FD]]")
            __ASM_EMIT("6:")

            : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
            : [FD] "r" (&f->d[0]), [FX1] "r" (&f->x1)
            : "cc", "memory",
              "q0", "q1", "q2", "q3",
              "q4", "q5", "q6", "q7",
              "q16", "q17", "q18", "q19",
              "q20", "q21", "q22", "q23"
        );
    }
}

#endif /* DSP_ARCH_AARCH64_ASIMD_FILTERS_STATIC_H_ */
