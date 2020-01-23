/*
 * convolution.h
 *
 *  Created on: 22 янв. 2020 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_AARCH64_ASIMD_CONVOLUTION_H_
#define DSP_ARCH_AARCH64_ASIMD_CONVOLUTION_H_

#ifndef DSP_ARCH_AARCH64_ASIMD_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_AARCH64_ASIMD_IMPL */

namespace asimd
{
    void convolve(float *dst, const float *src, const float *conv, size_t length, size_t count)
    {
        IF_ARCH_AARCH64(
            const float *c;
            float *d;
            size_t clen;
        )

        ARCH_AARCH64_ASM(
            // 4x blocks
            __ASM_EMIT("subs        %[count], %[count], #4")
            __ASM_EMIT("b.lo         200f")
                __ASM_EMIT("eor         v17.16b, v17.16b, v17.16b")             // v17 = 0 0 0 0
                __ASM_EMIT("100:")
                __ASM_EMIT("mov         %[d], %[dst]")
                __ASM_EMIT("mov         %[c], %[conv]")
                __ASM_EMIT("ld4r        {v4.4s, v5.4s, v6.4s, v7.4s}, [%[k]]")
                __ASM_EMIT("eor         v16.16b, v16.16b, v16.16b")             // v16 = p0 p0 p0 p0 = 0 0 0 0 (history)
                    // v4 = k0 k0 k0 k0
                    // v5 = k1 k1 k1 k1
                    // v6 = k2 k2 k2 k2
                    // v7 = k3 k3 k3 k3
                    // 8x convolution
                    __ASM_EMIT("subs        %[clen], %[length], #8")
                    __ASM_EMIT("b.lo        10f")
                        __ASM_EMIT("11:")
                        __ASM_EMIT("ldp         q8, q9, [%[c]]")                    // v8   = c0 c1 c2 c3, v9 = c4 c5 c6 c7
                        __ASM_EMIT("ldp         q0, q1, [%[d]]")                    // v0   = d0 d1 d2 d3, v1 = d4 d5 d6 d7
                        __ASM_EMIT("ext         v10.16b, v16.16b, v8.16b, #12")     // v10  = p3 c0 c1 c2
                        __ASM_EMIT("ext         v11.16b, v8.16b, v9.16b, #12")      // v11  = c3 c4 c5 c6
                        __ASM_EMIT("ext         v12.16b, v16.16b, v8.16b, #8")      // v12  = p2 p3 c0 c1
                        __ASM_EMIT("ext         v13.16b, v8.16b, v9.16b, #8")       // v13  = c2 c3 c4 c5
                        __ASM_EMIT("ext         v14.16b, v16.16b, v8.16b, #4")      // v14  = p1 p2 p3 c0
                        __ASM_EMIT("ext         v15.16b, v8.16b, v9.16b, #4")       // v15  = c1 c2 c3 c4
                        __ASM_EMIT("fmla        v0.4s, v4.4s, v8.4s")               // v0   = d0+k0*c0 d1+k0*c1 d2+k0*c2 d3+k0*c3
                        __ASM_EMIT("fmla        v1.4s, v4.4s, v9.4s")               // v1   = d4+k0*c4 d5+k0*c5 d6+k0*c6 d7+k0*c7
                        __ASM_EMIT("fmla        v0.4s, v5.4s, v10.4s")              // v0   = d0 + k0*c0 + k1*p3 ...
                        __ASM_EMIT("fmla        v1.4s, v5.4s, v11.4s")              // v1   = d1 + k0*c4 + k1*c3 ...
                        __ASM_EMIT("fmla        v0.4s, v6.4s, v12.4s")              // v0   = d0 + k0*c0 + k1*p3 + k2*p2 ...
                        __ASM_EMIT("fmla        v1.4s, v6.4s, v13.4s")              // v1   = d1 + k0*c4 + k1*c3 + k2*c2 ...
                        __ASM_EMIT("fmla        v0.4s, v7.4s, v14.4s")              // v0   = d0 + k0*c0 + k1*p3 + k2*p2 + k3*p1 ...
                        __ASM_EMIT("fmla        v1.4s, v7.4s, v15.4s")              // v1   = d1 + k0*c4 + k1*c3 + k2*c2 + k3*c1 ...
                        __ASM_EMIT("mov         v16.16b, v9.16b")                   // v16  = c4 c5 c6 c7
                        __ASM_EMIT("stp         q0, q1, [%[d]]")                    // v0   = d0 d1 d2 d3, v1 = d4 d5 d6 d7
                        __ASM_EMIT("subs        %[clen], %[clen], #8")
                        __ASM_EMIT("add         %[c], %[c], #0x20")                 // c   += 8
                        __ASM_EMIT("add         %[d], %[d], #0x20")                 // d   += 8
                        __ASM_EMIT("b.hs        11b")
                    __ASM_EMIT("10:")
                    // 4x convolution
                    __ASM_EMIT("adds        %[clen], %[clen], #4")
                    __ASM_EMIT("b.lo        12f")
                    __ASM_EMIT("ldr         q8, [%[c]]")                        // v8   = c0 c1 c2 c3
                    __ASM_EMIT("ldr         q0, [%[d]]")                        // v0   = d0 d1 d2 d3
                    __ASM_EMIT("ext         v10.16b, v16.16b, v8.16b, #12")     // v10  = p3 c0 c1 c2
                    __ASM_EMIT("ext         v12.16b, v16.16b, v8.16b, #8")      // v12  = p2 p3 c0 c1
                    __ASM_EMIT("ext         v14.16b, v16.16b, v8.16b, #4")      // v14  = p1 p2 p3 c0
                    __ASM_EMIT("fmla        v0.4s, v4.4s, v8.4s")               // v0   = d0+k0*c0 d1+k0*c1 d2+k0*c2 d3+k0*c3
                    __ASM_EMIT("fmla        v0.4s, v5.4s, v10.4s")              // v0   = d0 + k0*c0 + k1*p3 ...
                    __ASM_EMIT("fmla        v0.4s, v6.4s, v12.4s")              // v0   = d0 + k0*c0 + k1*p3 + k2*p2 ...
                    __ASM_EMIT("fmla        v0.4s, v7.4s, v14.4s")              // v0   = d0 + k0*c0 + k1*p3 + k2*p2 + k3*p1 ...
                    __ASM_EMIT("mov         v16.16b, v8.16b")                   // v16  = c0 c1 c2 c3
                    __ASM_EMIT("str         q0, [%[d]]")
                    __ASM_EMIT("sub         %[clen], %[clen], #4")
                    __ASM_EMIT("add         %[c], %[c], #0x10")                 // c   += 4
                    __ASM_EMIT("add         %[d], %[d], #0x10")                 // d   += 4
                    __ASM_EMIT("12:")
                    // Apply tail: v16 = p0 p1 p2 p3
                    __ASM_EMIT("ldr         d0, [%[d], #0x00]")                 // v0   = d0 d1
                    __ASM_EMIT("ldr         s1, [%[d], #0x08]")                 // v1   = d2
                    __ASM_EMIT("ext         v10.16b, v16.16b, v17.16b, #12")    // v10  = p3  0  0  0
                    __ASM_EMIT("mov         v0.s[2], v1.s[0]")                  // v0   = d0 d1 d2
                    __ASM_EMIT("ext         v12.16b, v16.16b, v17.16b, #8")     // v12  = p2 p3  0  0
                    __ASM_EMIT("ext         v14.16b, v16.16b, v17.16b, #4")     // v14  = p1 p2 p3  0
                    __ASM_EMIT("fmla        v0.4s, v5.4s, v10.4s")              // v0   = d0 + k1*p3 ...
                    __ASM_EMIT("fmla        v0.4s, v6.4s, v12.4s")              // v0   = d0 + k1*p3 + k2*p2 ...
                    __ASM_EMIT("fmla        v0.4s, v7.4s, v14.4s")              // v0   = d0 + k1*p3 + k2*p2 + k3*p1 ...
                    __ASM_EMIT("str         d0, [%[d], #0x00]")
                    __ASM_EMIT("mov         v1.s[0], v0.s[2]")
                    __ASM_EMIT("str         s1, [%[d], #0x08]")
                    // 1x convolution
                    __ASM_EMIT("adds        %[clen], %[clen], #3")
                    __ASM_EMIT("b.lo        14f")
                        __ASM_EMIT("ldr         q4, [%[k]]")                        // q4   = k0 k1 k2 k3
                        __ASM_EMIT("13:")
                        __ASM_EMIT("ld1r        {v8.4s}, [%[c]]")                   // v8   = c0 c0 c0 c0
                        __ASM_EMIT("ldr         q0, [%[d]]")                        // v0   = d0 d1 d2 d3
                        __ASM_EMIT("fmla        v0.4s, v4.4s, v8.4s")               // v0   = d0+k0*c0 d1+k1*c0 d2+k2*c0 d3+k3*c0
                        __ASM_EMIT("str         q0, [%[d]]")
                        __ASM_EMIT("subs        %[clen], %[clen], #1")
                        __ASM_EMIT("add         %[c], %[c], #0x04")
                        __ASM_EMIT("add         %[d], %[d], #0x04")
                        __ASM_EMIT("b.ge        13b")
                    __ASM_EMIT("14:")
                __ASM_EMIT("subs        %[count], %[count], #4")
                __ASM_EMIT("add         %[dst], %[dst], #0x10")
                __ASM_EMIT("add         %[k], %[k], #0x10")
                __ASM_EMIT("b.ge        100b")

            // 1x blocks
            __ASM_EMIT("200:")
            __ASM_EMIT("adds        %[count], %[count], #3")
            __ASM_EMIT("b.lt        400f")
                __ASM_EMIT("300:")
                __ASM_EMIT("ld1r        {v8.4s}, [%[k]]")           // v8 = k0 k0 k0 k0
                __ASM_EMIT("mov         %[d], %[dst]")
                __ASM_EMIT("mov         v9.16b, v8.16b")            // v9 = k0 k0 k0 k0
                __ASM_EMIT("mov         %[c], %[conv]")
                    // 16x convolution
                    __ASM_EMIT("subs        %[clen], %[length], #16")
                    __ASM_EMIT("b.lo        20f")
                    __ASM_EMIT("21:")
                    __ASM_EMIT("ldp         q0, q1, [%[d], #0x00]")
                    __ASM_EMIT("ldp         q2, q3, [%[d], #0x20]")
                    __ASM_EMIT("ldp         q4, q5, [%[c], #0x00]")
                    __ASM_EMIT("ldp         q6, q7, [%[c], #0x20]")
                    __ASM_EMIT("fmla        v0.4s, v4.4s, v8.4s")
                    __ASM_EMIT("fmla        v1.4s, v5.4s, v9.4s")
                    __ASM_EMIT("fmla        v2.4s, v6.4s, v8.4s")
                    __ASM_EMIT("fmla        v3.4s, v7.4s, v9.4s")
                    __ASM_EMIT("stp         q0, q1, [%[d], #0x00]")
                    __ASM_EMIT("stp         q2, q3, [%[d], #0x20]")
                    __ASM_EMIT("subs        %[clen], %[clen], #16")
                    __ASM_EMIT("add         %[c], %[c], #0x40")
                    __ASM_EMIT("add         %[d], %[d], #0x40")
                    __ASM_EMIT("b.hs        21b")
                    __ASM_EMIT("20:")
                    // 8x convolution
                    __ASM_EMIT("adds       %[clen], %[clen], #8")
                    __ASM_EMIT("b.lo        22f")
                    __ASM_EMIT("ldp         q0, q1, [%[d], #0x00]")
                    __ASM_EMIT("ldp         q4, q5, [%[c], #0x00]")
                    __ASM_EMIT("fmla        v0.4s, v4.4s, v8.4s")
                    __ASM_EMIT("fmla        v1.4s, v5.4s, v9.4s")
                    __ASM_EMIT("stp         q0, q1, [%[d], #0x00]")
                    __ASM_EMIT("sub         %[clen], %[clen], #8")
                    __ASM_EMIT("add         %[c], %[c], #0x20")
                    __ASM_EMIT("add         %[d], %[d], #0x20")
                    __ASM_EMIT("22:")
                    // 4x convolution
                    __ASM_EMIT("adds       %[clen], %[clen], #4")
                    __ASM_EMIT("b.lo        24f")
                    __ASM_EMIT("ldr         q0, [%[d], #0x00]")
                    __ASM_EMIT("ldr         q4, [%[c], #0x00]")
                    __ASM_EMIT("fmla        v0.4s, v4.4s, v8.4s")
                    __ASM_EMIT("str         q0, [%[d], #0x00]")
                    __ASM_EMIT("sub         %[clen], %[clen], #4")
                    __ASM_EMIT("add         %[c], %[c], #0x10")
                    __ASM_EMIT("add         %[d], %[d], #0x10")
                    __ASM_EMIT("24:")
                    // 1x tail convolution
                    __ASM_EMIT("adds       %[clen], %[clen], #3")
                    __ASM_EMIT("b.lo        26f")
                    __ASM_EMIT("25:")
                    __ASM_EMIT("ld1r        {v0.4s}, [%[d]]")
                    __ASM_EMIT("ld1r        {v4.4s}, [%[c]]")
                    __ASM_EMIT("fmla        v0.4s, v4.4s, v8.4s")
                    __ASM_EMIT("st1         {v0.s}[0], [%[d]]")
                    __ASM_EMIT("subs        %[clen], %[clen], #1")
                    __ASM_EMIT("add         %[c], %[c], #0x04")
                    __ASM_EMIT("add         %[d], %[d], #0x04")
                    __ASM_EMIT("b.ge        25b")
                    __ASM_EMIT("26:")
                __ASM_EMIT("subs        %[count], %[count], #1")
                __ASM_EMIT("add         %[dst], %[dst], #0x04")
                __ASM_EMIT("add         %[k], %[k], #0x04")
                __ASM_EMIT("b.ge        300b")

            __ASM_EMIT("400:")
            : [dst] "+r" (dst),
              [k] "+r" (src), [count] "+r" (count),
              [c] "=&r" (c), [d] "=&r" (d),
              [clen] "=&r" (clen)
            : [conv] "r" (conv), [length] "r" (length)
            : "cc", "memory",
              "q0", "q1", "q2", "q3",
              "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11",
              "q12", "q13", "q14", "q15",
              "q16", "q17"
        );
    }
}

#endif /* DSP_ARCH_AARCH64_ASIMD_CONVOLUTION_H_ */
