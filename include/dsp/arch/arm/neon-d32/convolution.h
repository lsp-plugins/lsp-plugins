/*
 * convolve.h
 *
 *  Created on: 10 сент. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_ARM_NEON_D32_CONVOLVE_H_
#define DSP_ARCH_ARM_NEON_D32_CONVOLVE_H_

#ifndef DSP_ARCH_ARM_NEON_32_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_ARM_NEON_32_IMPL */

namespace neon_d32
{
    void convolve(float *dst, const float *src, const float *conv, size_t length, size_t count)
    {
        IF_ARCH_ARM(
            const float *c; // , *cc;
            float *d; //, *dd;
            size_t clen;
        )

        ARCH_ARM_ASM(
            __ASM_EMIT("subs        %[count], $4")
            __ASM_EMIT("blo         20f")

            __ASM_EMIT("10:")
                __ASM_EMIT("veor        q15, q15, q15")             // q15 = p0 p1 p2 p3 = 0 0 0 0 (history)
                __ASM_EMIT("mov         %[d], %[dst]")
                __ASM_EMIT("mov         %[c], %[conv]")
                __ASM_EMIT("subs        %[clen], %[length], $4")
                __ASM_EMIT("blo         12f")

                // Load convolution coefficients
                __ASM_EMIT("vld2.32     {d0[], d2[]}, [%[k]]")      // q0 = k0 k0 ? ?, q1 = k1 k1 ? ?
                __ASM_EMIT("vld2.32     {d1[], d3[]}, [%[k]]!")     // q0 = k0 k0 k0 k0, q1 = k1 k1 k1 k1, k += 8
                __ASM_EMIT("vld2.32     {d4[], d6[]}, [%[k]]")      // q2 = k2 k2 ? ?, q3 = k3 k3 ? ?
                __ASM_EMIT("vld2.32     {d5[], d7[]}, [%[k]]")      // q2 = k2 k2 k2 k2, q3 = k3 k3 k3 k3
                __ASM_EMIT("sub         %[k], $8")                  // k -= 8

                // Load convolution kernel
                __ASM_EMIT("11:")
                    __ASM_EMIT("vld1.32     {q4}, [%[c]]!")             // q4  = c0 c1 c2 c3, c += 4
                    __ASM_EMIT("vld1.32     {q8}, [%[d]]")              // q8  = d0 d1 d2 d3
                    __ASM_EMIT("vext.8      q5, q15, q4, $12")          // q5  = p3 c0 c1 c2
                    __ASM_EMIT("vext.8      q6, q15, q4, $8")           // q6  = p2 p3 c0 c1
                    __ASM_EMIT("vext.8      q7, q15, q4, $4")           // q7  = p1 p2 p3 c0
                    __ASM_EMIT("vmla.f32    q8, q5, q1")                // q8  = d0 + k0*c0 + k1*p3 ...
                    __ASM_EMIT("vmla.f32    q8, q4, q0")                // q8  = d0+k0*c0 d1+k0*c1 d2+k0*c2 d3+k0*c3
                    __ASM_EMIT("vmla.f32    q8, q6, q2")                // q8  = d0 + k0*c0 + k1*p3 + k2*p2 ...
                    __ASM_EMIT("vmla.f32    q8, q7, q3")                // q8  = d0 + k0*c0 + k1*p3 + k2*p2 + k3*p1 ...
                    __ASM_EMIT("vmov        q15, q4")                   // q15 = c0 c1 c2 c3
                    __ASM_EMIT("vst1.32     {q8}, [%[d]]!")             // d += 4
                    __ASM_EMIT("subs        %[clen], $4")               // clen -= 4
                    __ASM_EMIT("bhs         11b")

                // Apply tail: q15 = p0 p1 p2 p3
                __ASM_EMIT("vld1.32     {q1}, [%[k]]")              // s4 = k0, s5 = k1, s6 = k2, s7 = k3
                __ASM_EMIT("vmov        q7, q15")                   // s28 = p0, s29 = p1, s30 = p2, s31 = p3
                __ASM_EMIT("vldm.32     %[d], {s0-s2}")             // s0 = d0, s1 = d1, s2 = d2
                __ASM_EMIT("vmla.f32    s0, s7, s29")               // s0 = d0 + k3*p1
                __ASM_EMIT("vmla.f32    s1, s7, s30")               // s1 = d1 + k3*p2
                __ASM_EMIT("vmla.f32    s2, s7, s31")               // s2 = d2 + k3*p3
                __ASM_EMIT("vmla.f32    s0, s6, s30")               // s0 = d0 + k2*p2 + k3*p1
                __ASM_EMIT("vmla.f32    s1, s6, s31")               // s1 = d1 + k2*p3 + k3*p2
                __ASM_EMIT("vmla.f32    s0, s5, s31")               // s0 = d0 + k1*p3 + k2*p2 + k3*p1
                __ASM_EMIT("vstm.f32    %[d], {s0-s2}")

                // Apply tail
                __ASM_EMIT("12:")
                    __ASM_EMIT("adds        %[clen], $3")           // while (clen >= 0)
                    __ASM_EMIT("blt         14f")
                    __ASM_EMIT("vld1.32     {q0}, [%[k]]")          // q0 = k0 k1 k2 k3 k += 4
                    __ASM_EMIT("15:")
                        __ASM_EMIT("vld1.32     {d2[], d3[]}, [%[c]]!") // q1 = c0 c0 c0 c0, c++
                        __ASM_EMIT("vld1.32     {q2}, [%[d]]")          // q2 = d0 d1 d2 d3
                        __ASM_EMIT("vmla.f32    q2, q0, q1")            // q2 = d0+k0*c0 d1+k1*c0 d2+k2*c0 d3+k3*c0
                        __ASM_EMIT("vst1.32     {q2}, [%[d]]")
                        __ASM_EMIT("subs        %[clen], $1")
                        __ASM_EMIT("add         %[d], $4")              // d++
                        __ASM_EMIT("bge         15b")

                __ASM_EMIT("14:")
                __ASM_EMIT("add         %[dst], $0x10")         // dst   += 4
                __ASM_EMIT("add         %[k], $0x10")           // k += 4
                __ASM_EMIT("subs        %[count], $4")          // count -= 4
                __ASM_EMIT("bge         10b")

            __ASM_EMIT("20:")
            __ASM_EMIT("adds        %[count], $3")
            __ASM_EMIT("blt         40f")
            __ASM_EMIT("21:")
                __ASM_EMIT("mov         %[d], %[dst]")
                __ASM_EMIT("mov         %[c], %[conv]")
                __ASM_EMIT("vld1.32     {d0[], d1[]}, [%[k]]")  // q0 = k0 k0 k0 k0
                __ASM_EMIT("subs        %[clen], %[length], $8")
                __ASM_EMIT("vmov        q1, q0")                // q1 = k0 k0 k0 k0
                __ASM_EMIT("blo         22f")
                __ASM_EMIT("23:")
                    __ASM_EMIT("vld1.32     {q2-q3}, [%[c]]!")  // q2 = c0 c1 c2 c3, c+= 8
                    __ASM_EMIT("vld1.32     {q8-q9}, [%[d]]")   // q8 = d0 d1 d2 d3
                    __ASM_EMIT("vmla.f32    q8, q2, q0")        // q8 = d0+k0*c0 d1+k0*c1 d2+k0*c2 d3+k0*c3
                    __ASM_EMIT("vmla.f32    q9, q3, q1")
                    __ASM_EMIT("subs        %[clen], $8")       // clen -= 8
                    __ASM_EMIT("vst1.32     {q8-q9}, [%[d]]!")  // d += 8
                    __ASM_EMIT("bge         23b")

                // Apply convolution
                __ASM_EMIT("22:")
                __ASM_EMIT("adds        %[clen], $4")
                __ASM_EMIT("blt         24f")
                    __ASM_EMIT("vld1.32     {q2}, [%[c]]!")     // q2 = c0 c1 c2 c3, c+= 8
                    __ASM_EMIT("vld1.32     {q8}, [%[d]]")      // q8 = d0 d1 d2 d3
                    __ASM_EMIT("vmla.f32    q8, q2, q0")        // q8 = d0+k0*c0 d1+k0*c1 d2+k0*c2 d3+k0*c3
                    __ASM_EMIT("subs        %[clen], $4")       // clen -= 4
                    __ASM_EMIT("vst1.32     {q8}, [%[d]]!")     // d += 4

                // Apply tail
                __ASM_EMIT("24:")
                __ASM_EMIT("adds        %[clen], $3")
                __ASM_EMIT("blt         26f")
                __ASM_EMIT("25:")
                    __ASM_EMIT("vldm.32     %[c]!, {s2}")       // s2 = c0, c++
                    __ASM_EMIT("vldm.32     %[d], {s8}")        // s8 = d0
                    __ASM_EMIT("vmla.f32    s8, s2, s0")        // s8 = d0+k0*c0
                    __ASM_EMIT("subs        %[clen], $1")       // clen --
                    __ASM_EMIT("vstm.32     %[d]!, {s8}")       // d ++
                    __ASM_EMIT("bge         25b")

                __ASM_EMIT("26:")
                __ASM_EMIT("add         %[k], $0x04")
                __ASM_EMIT("add         %[dst], $0x04")             // dst++
                __ASM_EMIT("subs        %[count], $1")
                __ASM_EMIT("bge         21b")

            __ASM_EMIT("40:")
            : [dst] "+r" (dst),
              [k] "+r" (src), [count] "+r" (count),
              [c] "=&r" (c), [d] "=&r" (d),
              [clen] "=&r" (clen)
            : [conv] "r" (conv), [length] "r" (length)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }
}


#endif /* DSP_ARCH_ARM_NEON_D32_CONVOLVE_H_ */
