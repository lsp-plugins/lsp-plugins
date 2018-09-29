/*
 * convolve.h
 *
 *  Created on: 7 сент. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_SSE_CONVOLUTION_H_
#define DSP_ARCH_X86_SSE_CONVOLUTION_H_

namespace sse
{
    void convolve(float *dst, const float *src, const float *conv, size_t length, size_t count)
    {
        const float *c;
        float *d;
        size_t clen;

        ARCH_X86_ASM(
            __ASM_EMIT("sub         $4, %[count]")
            __ASM_EMIT("jb          20f")

            __ASM_EMIT("10:")
                __ASM_EMIT("xorps       %%xmm7, %%xmm7")            // xmm7 = 0
                __ASM_EMIT("mov         %[length], %[clen]")
                __ASM_EMIT("mov         %[dst], %[d]")
                __ASM_EMIT("mov         %[conv], %[c]")

                __ASM_EMIT("sub         $4, %[clen]")
                __ASM_EMIT("jb          12f")
                // Load convolution kernel
                __ASM_EMIT("movups      (%[k]), %%xmm0")            // xmm0 = k0 k1 k2 k3
                __ASM_EMIT("movaps      %%xmm0, %%xmm1")            // xmm1 = k0 k1 k2 k3
                __ASM_EMIT("movaps      %%xmm0, %%xmm2")            // xmm2 = k0 k1 k2 k3
                __ASM_EMIT("movaps      %%xmm1, %%xmm3")            // xmm3 = k0 k1 k2 k3
                __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0")     // xmm0 = k0 k0 k0 k0
                __ASM_EMIT("shufps      $0x55, %%xmm1, %%xmm1")     // xmm1 = k1 k1 k1 k1
                __ASM_EMIT("shufps      $0xaa, %%xmm2, %%xmm2")     // xmm2 = k2 k2 k2 k2
                __ASM_EMIT("shufps      $0xff, %%xmm3, %%xmm3")     // xmm3 = k2 k2 k2 k2

                __ASM_EMIT("11:")
                    __ASM_EMIT("movaps      %%xmm7, %%xmm4")            // xmm4 = p0 p1 p2 p3
                    __ASM_EMIT("movups      (%[c]), %%xmm7")            // xmm7 = c0 c1 c2 c3
                    __ASM_EMIT("movaps      %%xmm4, %%xmm5")            // xmm5 = p0 p1 p2 p3
                    __ASM_EMIT("shufps      $0x4e, %%xmm7, %%xmm5")     // xmm5 = p2 p3 c0 c1 (+)
                    __ASM_EMIT("movaps      %%xmm5, %%xmm6")            // xmm6 = p2 p3 c0 c1
                    __ASM_EMIT("shufps      $0x99, %%xmm7, %%xmm6")     // xmm6 = p3 c0 c1 c2 (+)
                    __ASM_EMIT("shufps      $0x99, %%xmm5, %%xmm4")     // xmm4 = p1 p2 p3 c0

                    // Untouchable: xmm0, xmm1, xmm2, xmm3, xmm7
                    __ASM_EMIT("mulps       %%xmm2, %%xmm5")            // xmm5 = V2 = k2*p2 k2*p3 k2*c0 k2*c1
                    __ASM_EMIT("mulps       %%xmm1, %%xmm6")            // xmm6 = V3 = k1*p3 k1*c0 k1*c1 k1*c2
                    __ASM_EMIT("mulps       %%xmm3, %%xmm4")            // xmm4 = V1 = k3*p1 k3*p2 k3*p3 k3*c0
                    __ASM_EMIT("addps       %%xmm6, %%xmm5")            // xmm5 = V2 + V3
                    __ASM_EMIT("movaps      %%xmm7, %%xmm6")            // xmm6 = c0 c1 c2 c3
                    __ASM_EMIT("addps       %%xmm5, %%xmm4")            // xmm4 = V1 + V2 + V3
                    __ASM_EMIT("mulps       %%xmm0, %%xmm6")            // xmm6 = V0 = k0*c0 k0*c1 k0*c2 k0*c3
                    __ASM_EMIT("movups      (%[d]), %%xmm5")            // xmm5 = D + d0 d1 d2 d3
                    __ASM_EMIT("addps       %%xmm6, %%xmm4")            // xmm4 = V0 + V1 + V2 + V3
                    __ASM_EMIT("addps       %%xmm5, %%xmm4")            // xmm4 = D + V0 + V1 + V2 + V3
                    __ASM_EMIT("movups      %%xmm4, (%[d])")
                    __ASM_EMIT("add         $0x10, %[c]")               // c += 4
                    __ASM_EMIT("add         $0x10, %[d]")               // d += 4
                    __ASM_EMIT("sub         $4, %[clen]")               // clen -= 4
                    __ASM_EMIT("jae         11b")

                // Apply tail: xmm7 =  p0 p1 p2 p3
                __ASM_EMIT("movaps      %%xmm7, %%xmm5")            // xmm5 = p0 p1 p2 p3
                __ASM_EMIT("movhlps     %%xmm7, %%xmm6")            // xmm6 = p2
                __ASM_EMIT("shufps      $0xff, %%xmm5, %%xmm5")     // xmm5 = p3
                __ASM_EMIT("shufps      $0x55, %%xmm7, %%xmm7")     // xmm7 = p1
                __ASM_EMIT("movss       0x04(%[k]), %%xmm0")        // xmm0 = k1
                __ASM_EMIT("movss       0x08(%[k]), %%xmm1")        // xmm1 = k2
                __ASM_EMIT("movss       0x0c(%[k]), %%xmm2")        // xmm2 = k3

                __ASM_EMIT("movaps      %%xmm0, %%xmm3")            // xmm3 = k1
                __ASM_EMIT("movaps      %%xmm1, %%xmm4")            // xmm4 = k2
                __ASM_EMIT("mulss       %%xmm2, %%xmm7")            // xmm7 = k3*p1
                __ASM_EMIT("mulss       %%xmm5, %%xmm3")            // xmm3 = k1*p3
                __ASM_EMIT("mulss       %%xmm6, %%xmm4")            // xmm4 = k2*p2
                __ASM_EMIT("addss       %%xmm7, %%xmm3")            // xmm3 = k1*p3 + k3*p1
                __ASM_EMIT("addss       %%xmm4, %%xmm3")            // xmm3 = k1*p3 + k2*p2 + k3*p1
                __ASM_EMIT("movss       0x00(%[d]), %%xmm7")        // xmm7 = d0
                __ASM_EMIT("addss       %%xmm3, %%xmm7")            // xmm7 = d0 + k1*p3 + k2*p2 + k3*p1
                __ASM_EMIT("movss       %%xmm7, 0x00(%[d])")        // xmm7 = d0

                __ASM_EMIT("movss       0x04(%[d]), %%xmm7")        // xmm4 = d1
                __ASM_EMIT("mulss       %%xmm2, %%xmm6")            // xmm6 = k3*p2
                __ASM_EMIT("mulss       %%xmm5, %%xmm1")            // xmm1 = k2*p3
                __ASM_EMIT("addss       %%xmm6, %%xmm7")            // xmm7 = d1 + k3*p2
                __ASM_EMIT("addss       %%xmm1, %%xmm7")            // xmm7 = d1 + k3*p2 + k2*p3
                __ASM_EMIT("movss       %%xmm7, 0x04(%[d])")

                __ASM_EMIT("movss       0x08(%[d]), %%xmm7")        // xmm7 = d2
                __ASM_EMIT("mulss       %%xmm5, %%xmm2")            // xmm2 = k3*p3
                __ASM_EMIT("addss       %%xmm2, %%xmm7")            // xmm7 = d2 + k3*p3
                __ASM_EMIT("movss       %%xmm7, 0x08(%[d])")

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
                __ASM_EMIT64("add       $0x10, %[dst]")         // dst += 4
                __ASM_EMIT32("addl      $0x10, %[dst]")
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
                __ASM_EMIT64("add       $0x04, %[dst]")             // dst++
                __ASM_EMIT32("addl      $0x04, %[dst]")
                __ASM_EMIT("dec         %[count]")
                __ASM_EMIT("jge         21b")

            __ASM_EMIT("40:")
            : __IF_32([dst] "+m" (dst)) __IF_64([dst] "+r" (dst)),
              [k] "+r" (src), [count] "+r" (count),
              [c] "=&r" (c), [d] "=&r" (d), [clen] "=&r" (clen)
            : [conv] "g" (conv), [length] "g" (length)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }
}

#endif /* DSP_ARCH_X86_SSE_CONVOLUTION_H_ */
