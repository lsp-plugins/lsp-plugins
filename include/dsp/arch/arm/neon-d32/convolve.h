/*
 * convolve.h
 *
 *  Created on: 10 сент. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_ARM_NEON_D32_CONVOLVE_H_
#define DSP_ARCH_ARM_NEON_D32_CONVOLVE_H_

namespace neon_d32
{
#if 0
    void convolve(float *dst, const float *src, const float *conv, size_t length, size_t count)
    {
        const float *c;
        float *d;
        size_t clen;

        ARCH_ARM_ASM(
            __ASM_EMIT("subs        $4, %[count]")
            __ASM_EMIT("blo         20f")

            __ASM_EMIT("10:")
                __ASM_EMIT("veor        q15, q15, q15")             // q15 = 0 (history)
                __ASM_EMIT("mov         %[clen], %[length]")
                __ASM_EMIT("mov         %[d], %[dst]")
                __ASM_EMIT("mov         %[c], %[conv]")

                __ASM_EMIT("sub         %[clen], $4")
                __ASM_EMIT("blo         12f")

                // Load convolution coefficients
                __ASM_EMIT("vld2.32     {d0[], d2[]}, [%[k]]")      // q0 = k0 k0 ? ?, q1 = k1 k1 ? ?
                __ASM_EMIT("vld2.32     {d1[], d3[]}, [%[k]]!")     // q0 = k0 k0 k0 k0, q1 = k1 k1 k1 k1, k += 8
                __ASM_EMIT("vld2.32     {d4[], d6[]}, [%[k]]")      // q2 = k2 k2 ? ?, q3 = k3 k3 ? ?
                __ASM_EMIT("vld2.32     {d5[], d7[]}, [%[k]]")      // q2 = k2 k2 k2 k2, q3 = k3 k3 k3 k3
                __ASM_EMIT("sub         %[k], $8")                  // k -= 8

                // Load convolution kernel
                __ASM_EMIT("11:")
                    __ASM_EMIT("vld1.32     {q8}, %[d]")                // q8  = d0 d1 d2 d3
                    __ASM_EMIT("vld1.32     q4, [%[c]]!")               // q4  = c0 c1 c2 c3, c += 4
                    __ASM_EMIT("vext        q5, q4, q15, $4")           // q5  = p3 c0 c1 c2
                    __ASM_EMIT("vext        q6, q4, q15, $8")           // q6  = p2 p3 c0 c1
                    __ASM_EMIT("vext        q7, q4, q15, $12")          // q7  = p1 p2 p3 c0
                    __ASM_EMIT("vmla.f32    q8, q5, q1")                // q8  = d0 + k0*c0 + k1*p3 ...
                    __ASM_EMIT("vmla.f32    q8, q4, q0")                // q8  = d0+k0*c0 d1+k0*c1 d2+k0*c2 d3+k0*c3
                    __ASM_EMIT("vmla.f32    q8, q6, q2")                // q8  = d0 + k0*c0 + k1*p3 + k2*p2 ...
                    __ASM_EMIT("vmla.f32    q8, q7, q3")                // q8  = d0 + k0*c0 + k1*p3 + k2*p2 + k3*p1 ...
                    __ASM_EMIT("vmov        q15, q4")                   // q15 = c0 c1 c2 c3

                    __ASM_EMIT("vst1.32     {q8}, %[d]!")               // d += 4
                    __ASM_EMIT("subs        %[clen], $4")               // clen -= 4
                    __ASM_EMIT("bhs         11b")

                // Apply tail: q15 = p0 p1 p2 p3 => s28 = p0, s29 = p1, s30 = p2, s31 = p3
                __ASM_EMIT("vld1.32     {q1}, %[k]")                // s4 = k0, s5 = k1, s6 = k2, s7 = k3
                __ASM_EMIT("vldm.32     %[d], {s0-s2}")             // s0 = d0, s1 = d1, s2 = d2, s3 = d3
                __ASM_EMIT("vmul.f32    s8, s7, s29")               // s8 = k3*p1
                __ASM_EMIT("vmul.f32    s9, s7, s30")               // s9 = k3*p2
                __ASM_EMIT("vmla.f32    s8, s5, s31")               // s8 = k1*p3 + k3*p1
                __ASM_EMIT("vmul.f32    s10, s7, s31")              // s10 = k3*p3
                __ASM_EMIT("vmla.f32    s8, s6, s30")               // s8 = k1*p3 + k2*p2 + k3*p1
                __ASM_EMIT("vmla.f32    s9, s6, s31")               // s9 = k2*p3 + k3*p2
                __ASM_EMIT("vmadd.f32   q0, q0, q1")
                __ASM_EMIT("vstm.f32    %[d], {s0-s2}")

                // TODO
                // Apply tail
                __ASM_EMIT("12:")
                    __ASM_EMIT("add         $3, %[clen]")       // while (clen >= 0)
                    __ASM_EMIT("jl          14f")
                    __ASM_EMIT("movups      0x00(%[k]), %%xmm1")    // xmm1 = k0 k1 k2 k3

                    __ASM_EMIT("15:")
                        __ASM_EMIT("movss       0x00(%[c]), %%xmm0")    // xmm0 = c0
                        __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0") // xmm0 = c0 c0 c0 c0
                        __ASM_EMIT("movups      0x00(%[d]), %%xmm2")    // xmm2 = d0 d1 d2 d3
                        __ASM_EMIT("mulps       %%xmm1, %%xmm0")        // xmm0 = k0*c0 k1*c0 k2*c0 k3*c0
                        __ASM_EMIT("addps       %%xmm2, %%xmm0")        // xmm0 = d0+k0*c0 d1+k1*c0 d2+k2*c0 d3+k3*c0
                        __ASM_EMIT("movups      %%xmm0, 0x00(%[d])")
                        __ASM_EMIT("add         $0x04, %[c]")           // c++
                        __ASM_EMIT("add         $0x04, %[d]")           // d++
                        __ASM_EMIT("dec         %[clen]")
                        __ASM_EMIT("jge         15b")

                __ASM_EMIT("14:")
                __ASM_EMIT("add         $0x10, %[dst]")         // dst += 4
                __ASM_EMIT("add         $0x10, %[k]")           // k += 4
                __ASM_EMIT("sub         $0x04, %[count]")       // count -= 4
                __ASM_EMIT("jge         10b")

            __ASM_EMIT("20:")
            __ASM_EMIT("add         $3, %[count]")
            __ASM_EMIT("jl          40f")
            __ASM_EMIT("21:")
                __ASM_EMIT("mov         %[length], %[clen]")
                __ASM_EMIT("mov         %[dst], %[d]")
                __ASM_EMIT("mov         %[conv], %[c]")
                __ASM_EMIT("movss       0x00(%[k]), %%xmm0")    // xmm0 = k0
                __ASM_EMIT("sub         $8, %[clen]")
                __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0") // xmm0 = k0 k0 k0 k0
                __ASM_EMIT("jb          22f")
                __ASM_EMIT("movaps      %%xmm0, %%xmm1")        // xmm1 = k0 k0 k0 k0
                __ASM_EMIT("23:")
                    __ASM_EMIT("movups      0x00(%[c]), %%xmm2")    // xmm2 = c0 c1 c2 c3
                    __ASM_EMIT("movups      0x10(%[c]), %%xmm3")
                    __ASM_EMIT("movups      0x00(%[d]), %%xmm4")    // xmm4 = d0 d1 d2 d3
                    __ASM_EMIT("movups      0x10(%[d]), %%xmm5")
                    __ASM_EMIT("mulps       %%xmm0, %%xmm2")        // xmm2 = k0*c0 k0*c1 k0*c2 k0*c3
                    __ASM_EMIT("mulps       %%xmm1, %%xmm3")
                    __ASM_EMIT("addps       %%xmm4, %%xmm2")        // xmm2 = d0+k0*c0 d1+k0*c1 d2+k0*c2 d3+k0*c3
                    __ASM_EMIT("addps       %%xmm5, %%xmm3")
                    __ASM_EMIT("movups      %%xmm2, 0x00(%[d])")
                    __ASM_EMIT("movups      %%xmm3, 0x10(%[d])")
                    __ASM_EMIT("add         $0x20, %[c]")           // c += 8
                    __ASM_EMIT("add         $0x20, %[d]")           // d += 8
                    __ASM_EMIT("sub         $8, %[clen]")           // clen -= 8
                    __ASM_EMIT("jae         23b")

                // Apply convolution
                __ASM_EMIT("22:")
                __ASM_EMIT("add         $4, %[clen]")
                __ASM_EMIT("jl          24f")
                    __ASM_EMIT("movups      0x00(%[c]), %%xmm1")    // xmm1 = c0 c1 c2 c3
                    __ASM_EMIT("movups      0x00(%[d]), %%xmm2")    // xmm2 = d0 d1 d2 d3
                    __ASM_EMIT("mulps       %%xmm0, %%xmm1")        // xmm1 = k0*c0 k0*c1 k0*c2 k0*c3
                    __ASM_EMIT("addps       %%xmm1, %%xmm2")        // xmm2 = d0+k0*c0 d1+k0*c1 d2+k0*c2 d3+k0*c3
                    __ASM_EMIT("movups      %%xmm2, 0x00(%[d])")
                    __ASM_EMIT("add         $0x10, %[c]")           // c += 4
                    __ASM_EMIT("add         $0x10, %[d]")           // d += 4
                    __ASM_EMIT("sub         $4, %[clen]")           // clen -= 4

                // Apply tail
                __ASM_EMIT("24:")
                __ASM_EMIT("add         $3, %[clen]")
                __ASM_EMIT("jl          26f")
                __ASM_EMIT("25:")
                    __ASM_EMIT("movss       0x00(%[c]), %%xmm1")    // xmm1 = c0
                    __ASM_EMIT("movss       0x00(%[d]), %%xmm2")    // xmm2 = d0
                    __ASM_EMIT("mulss       %%xmm0, %%xmm1")        // xmm1 = k0*c0
                    __ASM_EMIT("addss       %%xmm1, %%xmm2")        // xmm2 = d0+k0*c0
                    __ASM_EMIT("movss       %%xmm2, 0x00(%[d])")
                    __ASM_EMIT("add         $0x04, %[c]")           // c ++
                    __ASM_EMIT("add         $0x04, %[d]")           // d ++
                    __ASM_EMIT("dec         %[clen]")
                    __ASM_EMIT("jge         25b")

                __ASM_EMIT("26:")
                __ASM_EMIT("add         $0x04, %[k]")
                __ASM_EMIT("add         $0x04, %[dst]")             // dst++
                __ASM_EMIT("dec         %[count]")
                __ASM_EMIT("jge         21b")

            __ASM_EMIT("40:")
            : [dst] "+r" (dst),
              [k] "+r" (src), [count] "+r" (count),
              [c] "=&r" (c), [d] "=&r" (d), [clen] "=&r" (clen)
            : [conv] "g" (conv), [length] "g" (length)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }
#endif
}


#endif /* DSP_ARCH_ARM_NEON_D32_CONVOLVE_H_ */
