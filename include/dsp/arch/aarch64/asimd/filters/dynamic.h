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
            // Check count
            __ASM_EMIT("cbz             %[count], 6f")
            __ASM_EMIT("ldp             s22, s23, [%[d]]")                      // v22  = d0, v23 = d1
            // x2 blocks
            __ASM_EMIT("subs            %[count], %[count], #2")
            __ASM_EMIT("b.lt            2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("ldp             s0, s1, [%[src]]")                      // v0   = s0, v1 = s1
            __ASM_EMIT("ldp             s16, s17, [%[f], #0x00]")               // v16  = a0, v17 = a0
            __ASM_EMIT("ldp             s18, s19, [%[f], #0x08]")               // v18  = a1, v19 = a2
            __ASM_EMIT("ldp             s20, s21, [%[f], #0x10]")               // v20  = b1, v21 = b2
            __ASM_EMIT("fmadd           s2, s16, s0, s22")                      // v2   = s' = a0*s0+d0
            __ASM_EMIT("fmul            s4, s18, s0")                           // v4   = a1*s0
            __ASM_EMIT("fmadd           s6, s20, s2, s23")                      // v6   = b1*s' + d1
            __ASM_EMIT("fmul            s0, s19, s0")                           // v0   = a2*s0
            __ASM_EMIT("fadd            s22, s4, s6")                           // v22  = d0' = d1 + a1*s0 + b1*s'
            __ASM_EMIT("fmadd           s23, s21, s2, s0")                      // v23  = d1' = a2*s0 + b2*s'
            __ASM_EMIT("add             %[f], %[f], #0x20")
            __ASM_EMIT("fmadd           s3, s16, s1, s22")                      // v3   = s' = a0*s1+d0
            __ASM_EMIT("fmul            s5, s18, s1")                           // v5   = a1*s1
            __ASM_EMIT("fmadd           s7, s20, s3, s23")                      // v7   = b1*s3 + d1
            __ASM_EMIT("fmul            s1, s19, s1")                           // v1   = a2*s1
            __ASM_EMIT("fadd            s22, s5, s7")                           // v22  = d0' = d1 + a1*s1 + b1*s3
            __ASM_EMIT("fmadd           s23, s21, s3, s1")                      // v23  = d1' = a2*s1 + b2*s0
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
            __ASM_EMIT("ldp             s16, s17, [%[f], #0x00]")               // v16  = a0, v17 = a0
            __ASM_EMIT("ldp             s18, s19, [%[f], #0x08]")               // v18  = a1, v19 = a2
            __ASM_EMIT("ldp             s20, s21, [%[f], #0x10]")               // v20  = b1, v21 = b2
            __ASM_EMIT("fmadd           s2, s16, s0, s22")                      // v2   = s' = a0*s0+d0
            __ASM_EMIT("fmul            s4, s18, s0")                           // v4   = a1*s0
            __ASM_EMIT("fmadd           s6, s20, s2, s23")                      // v6   = b1*s' + d1
            __ASM_EMIT("fmul            s0, s19, s0")                           // v0   = a2*s0
            __ASM_EMIT("fadd            s22, s4, s6")                           // v22  = d0' = d1 + a1*s0 + b1*s'
            __ASM_EMIT("fmadd           s23, s21, s2, s0")                      // v23  = d1' = a2*s0 + b2*s'
            __ASM_EMIT("str             s2, [%[dst]]")
            __ASM_EMIT("4:")
            // Store the updated buffer state
            __ASM_EMIT("stp             s22, s23, [%[d]]")
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

    void dyn_biquad_process_x2(float *dst, const float *src, float *d, size_t count, const biquad_x2_t *f)
    {
        // s'    = a0*s + d0;
        // d0'   = d1 + a1*s + b1*s';
        // d1'   = a2*s + b2*s';
        ARCH_AARCH64_ASM(
            // Check count
            __ASM_EMIT("cbz             %[count], 6f")
            __ASM_EMIT("ldp             q0, q1, [%[d], #0x00]")                             // v0   = d0 d1 0 0, v1 = e0 e1 0 0
            __ASM_EMIT("trn1            v24.4s, v0.4s, v1.4s")                              // v24  = d0 e0 0 0
            __ASM_EMIT("trn2            v25.4s, v0.4s, v1.4s")                              // v25  = d1 e1 0 0
            // x1 head block
            __ASM_EMIT("ld4             {v16.2s, v17.2s, v18.2s, v19.2s}, [%[f]], #0x20")   // v16  = a0, v17 = a0, v18  = a1, v19 = a2
            __ASM_EMIT("ld1             {v0.s}[0], [%[src]]")                   // v0   = s0
            __ASM_EMIT("fmul            v1.2s, v16.2s, v0.2s")                  // v1   = a0*s0
            __ASM_EMIT("fadd            v4.2s, v24.2s, v1.2s")                  // v4   = s' = d0+a0*s0
            __ASM_EMIT("fmul            v2.2s, v18.2s, v0.2s")                  // v2   = a1*s0
            __ASM_EMIT("ld4             {v20.2s, v21.2s, v22.2s, v23.2s}, [%[f]], #0x20")   // v20  = b1, v21 = b2, v22 = 0, v23 = 0
            __ASM_EMIT("fadd            v5.2s, v25.2s, v2.2s")                  // v5   = d1+a1*s0
            __ASM_EMIT("fmul            v6.2s, v19.2s, v0.2s")                  // v6   = a2*s0
            __ASM_EMIT("fmla            v5.2s, v20.2s, v4.2s")                  // v5   = d0' = d1+a1*s0+b1*s'
            __ASM_EMIT("fmla            v6.2s, v21.2s, v4.2s")                  // v6   = d1' = a2*s0+b2*s'
            __ASM_EMIT("add             %[src], %[src], #0x04")
            __ASM_EMIT("mov             v0.s[1], v4.s[0]")                      // shift
            __ASM_EMIT("mov             v24.s[0], v5.s[0]")                     // update d0
            __ASM_EMIT("mov             v25.s[0], v6.s[0]")                     // update d1
            // x2 blocks
            __ASM_EMIT("subs            %[count], %[count], #1")
            __ASM_EMIT("b.ls            2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("ld4             {v16.2s, v17.2s, v18.2s, v19.2s}, [%[f]], #0x20")   // v16  = a0, v17 = a0, v18  = a1, v19 = a2
            __ASM_EMIT("ld1             {v0.s}[0], [%[src]]")                   // v0   = s0 j0
            __ASM_EMIT("fmul            v1.2s, v16.2s, v0.2s")                  // v1   = a0*s0 a0*j0
            __ASM_EMIT("fadd            v4.2s, v24.2s, v1.2s")                  // v4   = s' j' = d0+a0*s0 e0+a0*j0
            __ASM_EMIT("fmul            v2.2s, v18.2s, v0.2s")                  // v2   = a1*s0 a1*j0
            __ASM_EMIT("ld4             {v20.2s, v21.2s, v22.2s, v23.2s}, [%[f]], #0x20")   // v20  = b1, v21 = b2, v22 = 0, v23 = 0
            __ASM_EMIT("fadd            v24.2s, v25.2s, v2.2s")                 // v24  = d1+a1*s0 e1+a1*j0
            __ASM_EMIT("st1             {v4.s}[1], [%[dst]]")
            __ASM_EMIT("fmul            v25.2s, v19.2s, v0.2s")                 // v25  = a2*s0 a2*j0
            __ASM_EMIT("fmla            v24.2s, v20.2s, v4.2s")                 // v24  = d0' e0' = d1+a1*s0+b1*s' e1+a1*j0+b1*j'
            __ASM_EMIT("fmla            v25.2s, v21.2s, v4.2s")                 // v25  = d1' e1' = a2*s0+b2*s' a2*j0 b2*j'
            __ASM_EMIT("mov             v0.s[1], v4.s[0]")                      // shift
            __ASM_EMIT("subs            %[count], %[count], #1")
            __ASM_EMIT("add             %[src], %[src], #0x04")
            __ASM_EMIT("add             %[dst], %[dst], #0x04")
            __ASM_EMIT("b.hi            1b")
            __ASM_EMIT("2:")
            // x1 tail block:
            __ASM_EMIT("ld4             {v16.2s, v17.2s, v18.2s, v19.2s}, [%[f]], #0x20")   // v16  = a0, v17 = a0, v18  = a1, v19 = a2
            __ASM_EMIT("trn1            v6.4s, v24.4s, v25.4s")                 // v24  = d0 d1 0 0
            __ASM_EMIT("fmul            v1.2s, v16.2s, v0.2s")                  // v1   = a0*j0
            __ASM_EMIT("str             d6, [%[d], #0x00]")
            __ASM_EMIT("fadd            v4.2s, v24.2s, v1.2s")                  // v4   = s' = e0 + a0*j0
            __ASM_EMIT("fmul            v2.2s, v18.2s, v0.2s")                  // v2   = a1*j0
            __ASM_EMIT("ld4             {v20.2s, v21.2s, v22.2s, v23.2s}, [%[f]], #0x20")   // v20  = b1, v21 = b2, v22 = 0, v23 = 0
            __ASM_EMIT("fadd            v24.2s, v25.2s, v2.2s")                 // v24  = e1 + a1*j0
            __ASM_EMIT("st1             {v4.s}[1], [%[dst]]")
            __ASM_EMIT("fmul            v25.2s, v19.2s, v0.2s")                 // v25  = a2*j0
            __ASM_EMIT("fmla            v24.2s, v20.2s, v4.2s")                 // v24  = d0' = e1 + a1*j0 + b1*j'
            __ASM_EMIT("fmla            v25.2s, v21.2s, v4.2s")                 // v25  = d1' = a2*j0 + b2*j'
            __ASM_EMIT("trn2            v6.4s, v24.4s, v25.4s")                 // v24  = e0 e1 0 0
            __ASM_EMIT("str             d6, [%[d], #0x10]")
            __ASM_EMIT("6:")

            : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count),
              [f] "+r" (f)
            : [d] "r" (d)
            : "cc", "memory",
              "q0", "q1", "q2", "q3",
              "q4", "q5", "q6", "q7",
              "q16", "q17", "q18", "q19",
              "q20", "q21", "q22", "q23",
              "q24", "q25"
        );
    }
}


#endif /* DSP_ARCH_AARCH64_ASIMD_FILTERS_DYNAMIC_H_ */
