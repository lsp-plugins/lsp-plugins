/*
 * dynamic.h
 *
 *  Created on: 27 янв. 2020 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_AARCH64_ASIMD_FILTERS_DYNAMIC_H_
#define DSP_ARCH_AARCH64_ASIMD_FILTERS_DYNAMIC_H_


#ifndef DSP_ARCH_AARCH64_ASIMD_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_AARCH64_ASIMD_IMPL */

namespace asimd
{
    void dyn_biquad_process_x1(float *dst, const float *src, float *d, size_t count, const biquad_x1_t *f)
    {
        // s'    = a0*s + d0;
        // d0'   = d1 + a1*s + b1*s';
        // d1'   = a2*s + b2*s';
        ARCH_AARCH64_ASM(
            __ASM_EMIT("ldp             s22, s23, [%[d]]")                      // v22  = d0, v23 = d1
            // x2 blocks
            __ASM_EMIT("subs            %[count], %[count], #2")
            __ASM_EMIT("b.lt            2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("ldp             s0, s1, [%[src]]")                      // v0   = s0, v1 = s1
            __ASM_EMIT("ld3             {v16.s, v17.s, v18.s}[0], [%[f]]")      // v16  = a0, v17 = a1, v18 = a2
            __ASM_EMIT("ldp             s19, s20, [%[f], #0x0c]")               // v19  = b1, v20 = b2
            __ASM_EMIT("fmadd           s2, s16, s0, s22")                      // v2   = s' = a0*s0+d0
            __ASM_EMIT("fmul            s4, s17, s0")                           // v4   = a1*s0
            __ASM_EMIT("fmadd           s6, s19, s2, s23")                      // v6   = b1*s' + d1
            __ASM_EMIT("fmul            s0, s18, s0")                           // v0   = a2*s0
            __ASM_EMIT("fadd            s22, s4, s6")                           // v22  = d0' = d1 + a1*s0 + b1*s'
            __ASM_EMIT("fmadd           s23, s20, s2, s0")                      // v23  = d1' = a2*s0 + b2*s'
            __ASM_EMIT("add             %[f], %[f], #0x20")
            __ASM_EMIT("fmadd           s3, s16, s1, s22")                      // v3   = s' = a0*s1+d0
            __ASM_EMIT("fmul            s5, s17, s1")                           // v5   = a1*s1
            __ASM_EMIT("fmadd           s7, s19, s3, s23")                      // v7   = b1*s3 + d1
            __ASM_EMIT("fmul            s1, s18, s1")                           // v1   = a2*s1
            __ASM_EMIT("fadd            s22, s5, s7")                           // v22  = d0' = d1 + a1*s1 + b1*s3
            __ASM_EMIT("fmadd           s23, s20, s3, s1")                      // v23  = d1' = a2*s1 + b2*s0
            __ASM_EMIT("subs            %[count], %[count], #2")
            __ASM_EMIT("stp             s2, s3, [%[dst]]")
            __ASM_EMIT("add             %[src], %[src], #0x08")
            __ASM_EMIT("add             %[dst], %[dst], #0x08")
            __ASM_EMIT("b.ge            1b")
            __ASM_EMIT("2:")
            // X1 block:
            __ASM_EMIT("adds            %[count], %[count], #1")
            __ASM_EMIT("b.lt            4f")
            __ASM_EMIT("ldr             s0, [%[src]]")                          // v0   = s0
            __ASM_EMIT("ld3             {v16.s, v17.s, v18.s}[0], [%[f]]")      // v16  = a0, v17 = a1, v18 = a2
            __ASM_EMIT("ldp             s19, s20, [%[f], #0x0c]")               // v19  = b1, v20 = b2
            __ASM_EMIT("fmadd           s2, s16, s0, s22")                      // v2   = s' = a0*s0+d0
            __ASM_EMIT("fmul            s4, s17, s0")                           // v4   = a1*s0
            __ASM_EMIT("fmadd           s6, s19, s2, s23")                      // v6   = b1*s' + d1
            __ASM_EMIT("fmul            s0, s18, s0")                           // v0   = a2*s0
            __ASM_EMIT("fadd            s22, s4, s6")                           // v22  = d0' = d1 + a1*s0 + b1*s'
            __ASM_EMIT("fmadd           s23, s20, s2, s0")                      // v23  = d1' = a2*s0 + b2*s'
            __ASM_EMIT("str             s2, [%[dst]]")
            __ASM_EMIT("4:")
            // Store the updated buffer state
            __ASM_EMIT("stp             s22, s23, [%[d]]")

            : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count),
              [f] "+r" (f)
            : [d] "r" (d)
            : "cc", "memory",
              "q0", "q1", "q2", "q3",
              "q4", "q5", "q6", "q7",
              "q16", "q17", "q18", "q19",
              "q20", "q21", "q22", "q23"
        );
    }

    void dyn_biquad_process_x2(float *dst, const float *src, float *d, size_t count, const biquad_x2_t *f)
    {
        // s'    = a0*s + d0;
        // d0'   = d1 + a1*s + b1*s';
        // d1'   = a2*s + b2*s';
        ARCH_AARCH64_ASM(
            // Check count
            __ASM_EMIT("cbz             %[count], 6f")
            __ASM_EMIT("ldp             d22, d23, [%[d], #0x00]")               // v22  = d0 e0, v23 = d1 e1
            // x1 head block
            __ASM_EMIT("ld1             {v0.s}[0], [%[src]]")                   // v0   = s0
            __ASM_EMIT("ldp             d16, d18, [%[f], #0x00]")               // v16  = a0, v18 = a1
            __ASM_EMIT("ldp             d19, d20, [%[f], #0x10]")               // v19  = a2, v20 = b1
            __ASM_EMIT("ldr             d21, [%[f], #0x20]")                    // v21  = b2
            __ASM_EMIT("fmul            v1.2s, v16.2s, v0.2s")                  // v1   = a0*s0
            __ASM_EMIT("fadd            v4.2s, v22.2s, v1.2s")                  // v4   = s' = d0+a0*s0
            __ASM_EMIT("fmul            v2.2s, v18.2s, v0.2s")                  // v2   = a1*s0
            __ASM_EMIT("fadd            v5.2s, v23.2s, v2.2s")                  // v5   = d1+a1*s0
            __ASM_EMIT("fmul            v6.2s, v19.2s, v0.2s")                  // v6   = a2*s0
            __ASM_EMIT("fmla            v5.2s, v20.2s, v4.2s")                  // v5   = d0' = d1+a1*s0+b1*s'
            __ASM_EMIT("fmla            v6.2s, v21.2s, v4.2s")                  // v6   = d1' = a2*s0+b2*s'
            __ASM_EMIT("add             %[src], %[src], #0x04")
            __ASM_EMIT("add             %[f], %[f], #0x30")
            __ASM_EMIT("mov             v0.s[1], v4.s[0]")                      // shift
            __ASM_EMIT("mov             v22.s[0], v5.s[0]")                     // update d0
            __ASM_EMIT("mov             v23.s[0], v6.s[0]")                     // update d1
            // x2 blocks
            __ASM_EMIT("subs            %[count], %[count], #1")
            __ASM_EMIT("b.ls            2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("ld1             {v0.s}[0], [%[src]]")                   // v0   = s0 j0
            __ASM_EMIT("ldp             d16, d18, [%[f], #0x00]")               // v16  = a0, v18 = a1
            __ASM_EMIT("ldp             d19, d20, [%[f], #0x10]")               // v19  = a2, v20 = b1
            __ASM_EMIT("ldr             d21, [%[f], #0x20]")                    // v21  = b2
            __ASM_EMIT("fmul            v1.2s, v16.2s, v0.2s")                  // v1   = a0*s0 a0*j0
            __ASM_EMIT("fadd            v4.2s, v22.2s, v1.2s")                  // v4   = s' j' = d0+a0*s0 e0+a0*j0
            __ASM_EMIT("fmul            v2.2s, v18.2s, v0.2s")                  // v2   = a1*s0 a1*j0
            __ASM_EMIT("st1             {v4.s}[1], [%[dst]]")
            __ASM_EMIT("fadd            v22.2s, v23.2s, v2.2s")                 // v22  = d1+a1*s0 e1+a1*j0
            __ASM_EMIT("fmul            v23.2s, v19.2s, v0.2s")                 // v23  = a2*s0 a2*j0
            __ASM_EMIT("fmla            v22.2s, v20.2s, v4.2s")                 // v22  = d0' e0' = d1+a1*s0+b1*s' e1+a1*j0+b1*j'
            __ASM_EMIT("fmla            v23.2s, v21.2s, v4.2s")                 // v23  = d1' e1' = a2*s0+b2*s' a2*j0 b2*j'
            __ASM_EMIT("subs            %[count], %[count], #1")
            __ASM_EMIT("mov             v0.s[1], v4.s[0]")                      // shift
            __ASM_EMIT("add             %[f], %[f], #0x30")
            __ASM_EMIT("add             %[src], %[src], #0x04")
            __ASM_EMIT("add             %[dst], %[dst], #0x04")
            __ASM_EMIT("b.hi            1b")
            __ASM_EMIT("2:")
            // x1 tail block:
            __ASM_EMIT("ldp             d16, d18, [%[f], #0x00]")               // v16  = a0, v18 = a1
            __ASM_EMIT("ldp             d19, d20, [%[f], #0x10]")               // v19  = a2, v20 = b1
            __ASM_EMIT("ldr             d21, [%[f], #0x20]")                    // v21  = b2
            __ASM_EMIT("fmul            v1.2s, v16.2s, v0.2s")                  // v1   = a0*j0
            __ASM_EMIT("fadd            v4.2s, v22.2s, v1.2s")                  // v4   = s' = e0 + a0*j0
            __ASM_EMIT("fmul            v2.2s, v18.2s, v0.2s")                  // v2   = a1*j0
            __ASM_EMIT("fadd            v5.2s, v23.2s, v2.2s")                  // v5   = e1 + a1*j0
            __ASM_EMIT("st1             {v4.s}[1], [%[dst]]")
            __ASM_EMIT("fmul            v6.2s, v19.2s, v0.2s")                  // v6   = a2*j0
            __ASM_EMIT("fmla            v5.2s, v20.2s, v4.2s")                  // v5   = d0' = e1 + a1*j0 + b1*j'
            __ASM_EMIT("fmla            v6.2s, v21.2s, v4.2s")                  // v6   = d1' = a2*j0 + b2*j'
            __ASM_EMIT("mov             v22.s[1], v5.s[1]")
            __ASM_EMIT("mov             v23.s[1], v6.s[1]")
            __ASM_EMIT("stp             d22, d23, [%[d], #0x00]")
            __ASM_EMIT("6:")

            : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count),
              [f] "+r" (f)
            : [d] "r" (d)
            : "cc", "memory",
              "q0", "q1", "q2", "q3",
              "q4", "q5", "q6", "q7",
              "q16", "q17", "q18", "q19",
              "q20", "q21", "q22", "q23"
        );
    }

    void dyn_biquad_process_x4(float *dst, const float *src, float *d, size_t count, const biquad_x4_t *f)
    {
        IF_ARCH_AARCH64( size_t mask; );

        ARCH_AARCH64_ASM
        (
            __ASM_EMIT("cbz         %[count], 10f")
            // Prepare
            __ASM_EMIT("ldp         q16, q17, [%[d]]")                      // q16  = d0, q17 = d1
            __ASM_EMIT("ldp         q24, q25, [%[X_MASK]]")                 // q24  = vmask, q25 = 1
            __ASM_EMIT("mov         %[mask], #1")                           // mask = 0

            // Do pre-loop
            __ASM_EMIT("1:")
            __ASM_EMIT("orr         %[mask], %[mask], %[mask], LSL #1")     // mask = (mask << 1) | 1
            __ASM_EMIT("ld1         {v0.s}[0], [%[src]]")                   // v0   = s
            __ASM_EMIT("ext         v24.16b, v25.16b, v24.16b, #12")        // v24  = (vmask << 1) | 1
            __ASM_EMIT("ldp         q18, q19, [%[f], #0x00]")               // q18  = a0, q19 = a1
            __ASM_EMIT("ldp         q20, q21, [%[f], #0x20]")               // q20  = a1, q21 = b1
            __ASM_EMIT("ldr         q22, [%[f], #0x40]")                    // q22  = b2
            __ASM_EMIT("fmul        v1.4s, v18.4s, v0.4s")                  // v1   = a0*s
            __ASM_EMIT("fmul        v2.4s, v19.4s, v0.4s")                  // v2   = a1*s
            __ASM_EMIT("fmul        v3.4s, v20.4s, v0.4s")                  // v3   = a2*s
            __ASM_EMIT("fadd        v0.4s, v1.4s, v16.4s")                  // v0   = a0*s + d0 = s'
            __ASM_EMIT("fmla        v2.4s, v21.4s, v0.4s")                  // v2   = a1*s + b1*s'
            __ASM_EMIT("fmla        v3.4s, v22.4s, v0.4s")                  // v3   = d1' = a2*s + b2*s'
            __ASM_EMIT("fadd        v2.4s, v2.4s, v17.4s")                  // v2   = d0' = d1 + a1*s + b1*s'
            __ASM_EMIT("bit         v17.16b, v3.16b, v24.16b")              // q9   = (d1 & ~vmask) | (d1' & vmask)
            __ASM_EMIT("bit         v16.16b, v2.16b, v24.16b")              // q8   = (d0 & ~vmask) | (d0' & vmask)
            __ASM_EMIT("ext         v0.16b, v0.16b, v0.16b, #12")           // v0   = s' = s[3] s[0] s[1] s[2]
            __ASM_EMIT("subs        %[count], %[count], #1")
            __ASM_EMIT("add         %[src], %[src], #0x04")
            __ASM_EMIT("add         %[f], %[f], #0x50")
            __ASM_EMIT("b.eq        6f")
            __ASM_EMIT("cmp         %[mask], #0x0f")
            __ASM_EMIT("b.ne        1b")

            // Do main loop
            __ASM_EMIT("5:")
            __ASM_EMIT("ldp         q18, q19, [%[f], #0x00]")               // q18  = a0, q19 = a1
            __ASM_EMIT("ldp         q20, q21, [%[f], #0x20]")               // q20  = a1, q21 = b1
            __ASM_EMIT("ldr         q22, [%[f], #0x40]")                    // q22  = b2
            __ASM_EMIT("ld1         {v0.s}[0], [%[src]]")
            __ASM_EMIT("fmul        v1.4s, v18.4s, v0.4s")                  // v1   = a0*s
            __ASM_EMIT("fmul        v2.4s, v19.4s, v0.4s")                  // v2   = a1*s
            __ASM_EMIT("fmul        v3.4s, v20.4s, v0.4s")                  // v3   = a2*s
            __ASM_EMIT("fadd        v0.4s, v1.4s, v16.4s")                  // v0   = a0*s + d0 = s'
            __ASM_EMIT("fmla        v2.4s, v21.4s, v0.4s")                  // v2   = a1*s + b1*s'
            __ASM_EMIT("fmla        v3.4s, v22.4s, v0.4s")                  // v3   = d1' = a2*s + b2*s'
            __ASM_EMIT("ext         v0.16b, v0.16b, v0.16b, #12")           // v0   = s' = s[3] s[0] s[1] s[2]
            __ASM_EMIT("fadd        v16.4s, v2.4s, v17.4s")                 // v16  = d0' = d1 + a1*s + b1*s'
            __ASM_EMIT("mov         v17.16b, v3.16b")                       // v17  = d1'
            __ASM_EMIT("st1         {v0.s}[0], [%[dst]]")
            __ASM_EMIT("subs        %[count], %[count], #1")
            __ASM_EMIT("add         %[f], %[f], #0x50")
            __ASM_EMIT("add         %[src], %[src], #0x04")
            __ASM_EMIT("add         %[dst], %[dst], #0x04")
            __ASM_EMIT("b.ne        5b")

            // Do post-loop
            __ASM_EMIT("6:")
            __ASM_EMIT("eor         %[mask], %[mask], #1")                  // reset bit
            __ASM_EMIT("eor         v25.16b, v25.16b, v25.16b")             // v25  = 0
            __ASM_EMIT("7:")
            __ASM_EMIT("ldp         q18, q19, [%[f], #0x00]")               // q18  = a0, q19 = a1
            __ASM_EMIT("ldp         q20, q21, [%[f], #0x20]")               // q20  = a1, q21 = b1
            __ASM_EMIT("ldr         q22, [%[f], #0x40]")                    // q22  = b2
            __ASM_EMIT("lsl         %[mask], %[mask], #1")                  // mask  = mask << 1
            __ASM_EMIT("ext         v24.16b, v25.16b, v24.16b, #12")        // v24  = (vmask << 1) | 0
            __ASM_EMIT("fmul        v1.4s, v18.4s, v0.4s")                  // v1   = a0*s
            __ASM_EMIT("fmul        v2.4s, v19.4s, v0.4s")                  // v2   = a1*s
            __ASM_EMIT("fmul        v3.4s, v20.4s, v0.4s")                  // v3   = a2*s
            __ASM_EMIT("fadd        v0.4s, v1.4s, v16.4s")                  // v0   = a0*s + d0 = s'
            __ASM_EMIT("fmla        v2.4s, v21.4s, v0.4s")                  // v2   = a1*s + b1*s'
            __ASM_EMIT("fmla        v3.4s, v22.4s, v0.4s")                  // v3   = d1' = a2*s + b2*s'
            __ASM_EMIT("fadd        v2.4s, v2.4s, v17.4s")                  // v2   = d0' = d1 + a1*s + b1*s'
            __ASM_EMIT("bit         v17.16b, v3.16b, v24.16b")              // q9   = (d1 & ~vmask) | (d1' & vmask)
            __ASM_EMIT("bit         v16.16b, v2.16b, v24.16b")              // q8   = (d0 & ~vmask) | (d0' & vmask)
            __ASM_EMIT("ext         v0.16b, v0.16b, v0.16b, #12")           // v0   = s' = s[3] s[0] s[1] s[2]
            __ASM_EMIT("add         %[f], %[f], #0x50")
            __ASM_EMIT("tst         %[mask], #0x10")                        // Need to emit?
            __ASM_EMIT("b.eq        8f")
            __ASM_EMIT("st1         {v0.s}[0], [%[dst]]")
            __ASM_EMIT("add         %[dst], %[dst], #0x04")
            __ASM_EMIT("8:")
            __ASM_EMIT("tst         %[mask], #0x0e")
            __ASM_EMIT("b.ne        7b")

            // Store memory
            __ASM_EMIT("stp         q16, q17, [%[d]]")
            __ASM_EMIT("10:")

            : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count),
              [f] "+r" (f),
              [mask] "=&r" (mask)
            : [d] "r" (d),
              [X_MASK] "r" (&biquad_x4_mask[0])
            : "cc", "memory",
              "q0", "q1", "q2", "q3",
              "q4", "q5", "q6", "q7",
              "q16", "q17", "q18", "q19",
              "q20", "q21", "q22", "q23",
              "q24", "q25"
        );
    }

    void dyn_biquad_process_x8(float *dst, const float *src, float *d, size_t count, const biquad_x8_t *f)
    {
        IF_ARCH_AARCH64(
            size_t mask;
        );

        ARCH_AARCH64_ASM
        (
            // Prepare
            __ASM_EMIT("cbz             %[count], 8f")
            __ASM_EMIT("ldp             q8, q9, [%[d], #0x00]")                     // v8-v9    = d0
            __ASM_EMIT("ldp             q10, q11, [%[d], #0x20]")                   // v10-v11  = d1
            __ASM_EMIT("mov             %[mask], #1")                               // mask     = 1
            __ASM_EMIT("ldp             q26, q27, [%[X_MASK], #0x00]")              // v26-v27  = vmask
            __ASM_EMIT("ldp             q28, q29, [%[X_MASK], #0x20]")              // v28-v29  = vshift = 1
            // Do pre-loop
            __ASM_EMIT("1:")
            __ASM_EMIT("ldp             q16, q17, [%[f], #0x00]")                   // q16-q17  = a0
            __ASM_EMIT("ldp             q18, q19, [%[f], #0x20]")                   // q18-q19  = a1
            __ASM_EMIT("ldp             q20, q21, [%[f], #0x40]")                   // q20-q21  = a2
            __ASM_EMIT("ldp             q22, q23, [%[f], #0x60]")                   // q22-q23  = b1
            __ASM_EMIT("ldp             q24, q25, [%[f], #0x80]")                   // q24-q25  = b2
            __ASM_EMIT("ld1             {v0.s}[0], [%[src]]")                       // q0       = s
            __ASM_EMIT("fmul            v2.4s, v16.4s, v0.4s")                      // v2       = a0*s
            __ASM_EMIT("fmul            v3.4s, v17.4s, v1.4s")
            __ASM_EMIT("fmul            v4.4s, v18.4s, v0.4s")                      // v4       = a1*s
            __ASM_EMIT("fmul            v5.4s, v19.4s, v1.4s")
            __ASM_EMIT("fmul            v6.4s, v20.4s, v0.4s")                      // v6       = a2*s
            __ASM_EMIT("fmul            v7.4s, v21.4s, v1.4s")
            __ASM_EMIT("fadd            v0.4s, v2.4s, v8.4s")                       // v0       = a0*s + d0 = s'
            __ASM_EMIT("fadd            v1.4s, v3.4s, v9.4s")
            __ASM_EMIT("fmla            v4.4s, v22.4s, v0.4s")                      // v4       = a1*s + b1*s'
            __ASM_EMIT("fmla            v5.4s, v23.4s, v1.4s")
            __ASM_EMIT("fmla            v6.4s, v24.4s, v0.4s")                      // v6       = d1' = a2*s + b2*s'
            __ASM_EMIT("fmla            v7.4s, v25.4s, v1.4s")
            __ASM_EMIT("fadd            v4.4s, v4.4s, v10.4s")                      // v4       = d0' = d1 + a1*s + b1*s'
            __ASM_EMIT("fadd            v5.4s, v5.4s, v11.4s")

            __ASM_EMIT("ext             v1.16b, v0.16b, v1.16b, #12")               // v1       = s2[3] s2[4] s2[5] s2[6]
            __ASM_EMIT("ext             v0.16b, v0.16b, v0.16b, #12")               // v0       = s2[3] s2[0] s2[1] s2[2]
            __ASM_EMIT("bit             v8.16b, v4.16b, v26.16b")                   // v8       = (d0 & ~vmask) | (d0' & vmask)
            __ASM_EMIT("bit             v9.16b, v5.16b, v27.16b")
            __ASM_EMIT("bit             v10.16b, v6.16b, v26.16b")                  // v9       = (d1 & ~vmask) | (d1' & vmask)
            __ASM_EMIT("bit             v11.16b, v7.16b, v27.16b")
            __ASM_EMIT("add             %[src], %[src], #0x04")                     // ++src
            __ASM_EMIT("add             %[f], %[f], #0xa0")                         // ++f
            __ASM_EMIT("subs            %[count], %[count], #1")
            __ASM_EMIT("b.eq            4f")
            __ASM_EMIT("ext             v27.16b, v26.16b, v27.16b, #12")            // vmask    = vmask << 1
            __ASM_EMIT("orr             %[mask], %[mask], %[mask], LSL #1")         // mask     = (mask << 1) | 1
            __ASM_EMIT("ext             v26.16b, v28.16b, v26.16b, #12")            // vmask    = (vmask << 1) | 1
            __ASM_EMIT("cmp             %[mask], #0xff")
            __ASM_EMIT("b.ne            1b")

            // Do main loop
            __ASM_EMIT("3:")
            __ASM_EMIT("ldp             q16, q17, [%[f], #0x00]")                   // q16-q17  = a0
            __ASM_EMIT("ldp             q18, q19, [%[f], #0x20]")                   // q18-q19  = a1
            __ASM_EMIT("ldp             q20, q21, [%[f], #0x40]")                   // q20-q21  = a2
            __ASM_EMIT("ldp             q22, q23, [%[f], #0x60]")                   // q22-q23  = b1
            __ASM_EMIT("ldp             q24, q25, [%[f], #0x80]")                   // q24-q25  = b2
            __ASM_EMIT("ld1             {v0.s}[0], [%[src]]")                       // q0       = s
            __ASM_EMIT("fmul            v2.4s, v16.4s, v0.4s")                      // v2       = a0*s
            __ASM_EMIT("fmul            v3.4s, v17.4s, v1.4s")
            __ASM_EMIT("fmul            v4.4s, v18.4s, v0.4s")                      // v4       = a1*s
            __ASM_EMIT("fmul            v5.4s, v19.4s, v1.4s")
            __ASM_EMIT("fmul            v6.4s, v20.4s, v0.4s")                      // v6       = a2*s
            __ASM_EMIT("fmul            v7.4s, v21.4s, v1.4s")
            __ASM_EMIT("fadd            v0.4s, v2.4s, v8.4s")                       // v0       = a0*s + d0 = s'
            __ASM_EMIT("fadd            v1.4s, v3.4s, v9.4s")
            __ASM_EMIT("fmla            v4.4s, v22.4s, v0.4s")                      // v4       = a1*s + b1*s'
            __ASM_EMIT("fmla            v5.4s, v23.4s, v1.4s")
            __ASM_EMIT("fmla            v6.4s, v24.4s, v0.4s")                      // v6       = d1' = a2*s + b2*s'
            __ASM_EMIT("fmla            v7.4s, v25.4s, v1.4s")
            __ASM_EMIT("fadd            v8.4s, v4.4s, v10.4s")                      // v8       = d0' = d1 + a1*s + b1*s'
            __ASM_EMIT("fadd            v9.4s, v5.4s, v11.4s")
            __ASM_EMIT("st1             {v1.s}[3], [%[dst]]")                       // *dst     = s'[7]
            __ASM_EMIT("mov             v10.16b, v6.16b")                           // v10      = d1'
            __ASM_EMIT("mov             v11.16b, v7.16b")
            __ASM_EMIT("ext             v1.16b, v0.16b, v1.16b, #12")               // v1       = s2[3] s2[4] s2[5] s2[6]
            __ASM_EMIT("ext             v0.16b, v0.16b, v0.16b, #12")               // v0       = s2[3] s2[0] s2[1] s2[2]
            __ASM_EMIT("subs            %[count], %[count], #1")
            __ASM_EMIT("add             %[f], %[f], #0xa0")                         // ++f
            __ASM_EMIT("add             %[src], %[src], #0x04")                     // ++src
            __ASM_EMIT("add             %[dst], %[dst], #0x04")                     // ++dst
            __ASM_EMIT("b.ne            3b")

            // Do post-loop
            __ASM_EMIT("4:")
            __ASM_EMIT("ext             v27.16b, v26.16b, v27.16b, #12")            // vmask    = vmask << 1
            __ASM_EMIT("lsl             %[mask], %[mask], #1")                      // mask     = mask << 1
            __ASM_EMIT("ext             v26.16b, v29.16b, v26.16b, #12")            // vmask    = vmask << 1

            __ASM_EMIT("5:")
            __ASM_EMIT("ldp             q16, q17, [%[f], #0x00]")                   // q16-q17  = a0
            __ASM_EMIT("ldp             q18, q19, [%[f], #0x20]")                   // q18-q19  = a1
            __ASM_EMIT("ldp             q20, q21, [%[f], #0x40]")                   // q20-q21  = a2
            __ASM_EMIT("ldp             q22, q23, [%[f], #0x60]")                   // q22-q23  = b1
            __ASM_EMIT("ldp             q24, q25, [%[f], #0x80]")                   // q24-q25  = b2
            __ASM_EMIT("fmul            v2.4s, v16.4s, v0.4s")                      // v2       = a0*s
            __ASM_EMIT("fmul            v3.4s, v17.4s, v1.4s")
            __ASM_EMIT("fmul            v4.4s, v18.4s, v0.4s")                      // v4       = a1*s
            __ASM_EMIT("fmul            v5.4s, v19.4s, v1.4s")
            __ASM_EMIT("fmul            v6.4s, v20.4s, v0.4s")                      // v6       = a2*s
            __ASM_EMIT("fmul            v7.4s, v21.4s, v1.4s")
            __ASM_EMIT("fadd            v0.4s, v2.4s, v8.4s")                       // v0       = a0*s + d0 = s'
            __ASM_EMIT("fadd            v1.4s, v3.4s, v9.4s")
            __ASM_EMIT("fmla            v4.4s, v22.4s, v0.4s")                      // v4       = a1*s + b1*s'
            __ASM_EMIT("fmla            v5.4s, v23.4s, v1.4s")
            __ASM_EMIT("fmla            v6.4s, v24.4s, v0.4s")                      // v6       = d1' = a2*s + b2*s'
            __ASM_EMIT("fmla            v7.4s, v25.4s, v1.4s")
            __ASM_EMIT("fadd            v4.4s, v4.4s, v10.4s")                      // v4       = d0' = d1 + a1*s + b1*s'
            __ASM_EMIT("fadd            v5.4s, v5.4s, v11.4s")

            __ASM_EMIT("add             %[f], %[f], #0xa0")                         // ++f
            __ASM_EMIT("tst             %[mask], #0x80")
            __ASM_EMIT("b.eq            6f")
            __ASM_EMIT("st1             {v1.s}[3], [%[dst]]")                       // *dst     = s'[7]
            __ASM_EMIT("add             %[dst], %[dst], #0x04")                     // ++dst
            __ASM_EMIT("6:")
            __ASM_EMIT("ext             v1.16b, v0.16b, v1.16b, #12")               // v1       = s2[3] s2[4] s2[5] s2[6]
            __ASM_EMIT("ext             v0.16b, v0.16b, v0.16b, #12")               // v0       = s2[3] s2[0] s2[1] s2[2]
            __ASM_EMIT("bit             v8.16b, v4.16b, v26.16b")                   // v8       = (d0 & ~vmask) | (d0' & vmask)
            __ASM_EMIT("bit             v9.16b, v5.16b, v27.16b")
            __ASM_EMIT("bit             v10.16b, v6.16b, v26.16b")                  // v9       = (d1 & ~vmask) | (d1' & vmask)
            __ASM_EMIT("bit             v11.16b, v7.16b, v27.16b")
            __ASM_EMIT("ext             v27.16b, v26.16b, v27.16b, #12")            // vmask    = vmask << 1
            __ASM_EMIT("lsl             %[mask], %[mask], #1")                      // mask     = mask << 1
            __ASM_EMIT("ext             v26.16b, v29.16b, v26.16b, #12")            // vmask    = vmask << 1
            __ASM_EMIT("tst             %[mask], #0xff")                            // mask  == 0 ?
            __ASM_EMIT("b.ne            5b")

            // Store memory
            __ASM_EMIT("6:")
            __ASM_EMIT("stp             q8, q9, [%[d], #0x00]")                     // v8-v9    = d0
            __ASM_EMIT("stp             q10, q11, [%[d], #0x20]")                   // v10-v11  = d1
            __ASM_EMIT("8:")

            : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count),
              [f] "+r" (f), [mask] "=&r" (mask)
            : [d] "r" (d),
              [X_MASK] "r" (&biquad_x8_mask[0])
            : "cc", "memory",
              "q0", "q1", "q2", "q3",
              "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11",
              "q16", "q17", "q18", "q19",
              "q20", "q21", "q22", "q23",
              "q24", "q25", "q26", "q27",
              "q28", "q29"
        );
    }
}


#endif /* DSP_ARCH_AARCH64_ASIMD_FILTERS_DYNAMIC_H_ */
