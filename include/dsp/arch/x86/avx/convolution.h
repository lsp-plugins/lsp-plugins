/*
 * convolution.h
 *
 *  Created on: 23 янв. 2020 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_AVX_CONVOLUTION_H_
#define DSP_ARCH_X86_AVX_CONVOLUTION_H_

#ifndef DSP_ARCH_X86_AVX_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_AVX_IMPL */

namespace avx
{
    void convolve(float *dst, const float *src, const float *conv, size_t length, size_t count)
    {
        IF_ARCH_X86(
            const float *c;
            float *d;
            size_t clen;
        );

        ARCH_X86_ASM(
            // 4x blocks
            __ASM_EMIT("sub                 $4, %[count]")
            __ASM_EMIT("jb                  200f")
            __ASM_EMIT("100:")
                __ASM_EMIT("vxorps              %%ymm7, %%ymm7, %%ymm7")        // ymm7 = 0
                __ASM_EMIT("mov                 %[length], %[clen]")
                __ASM_EMIT("mov                 %[dst], %[d]")
                __ASM_EMIT("mov                 %[conv], %[c]")

                __ASM_EMIT("vbroadcastss        0x00(%[k]), %%ymm0")            // ymm0 = k0
                __ASM_EMIT("vbroadcastss        0x04(%[k]), %%ymm1")            // ymm1 = k1
                __ASM_EMIT("vbroadcastss        0x08(%[k]), %%ymm2")            // ymm2 = k2
                __ASM_EMIT("vbroadcastss        0x0c(%[k]), %%ymm3")            // ymm3 = k3
                // 8x convolution
                __ASM_EMIT("sub                 $8, %[clen]")
                __ASM_EMIT("jb                  10f")
                __ASM_EMIT(".align              16")
                __ASM_EMIT("11:")
                    __ASM_EMIT("vmovups             (%[c]), %%ymm5")                // ymm5 = c0 c1 c2 c3 c4 c5 c6 c7
                    __ASM_EMIT("vinsertf128         $1, %%xmm5, %%ymm7, %%ymm4")    // ymm4 = p0 p1 p2 p3 c0 c1 c2 c3
                    __ASM_EMIT("vmovaps             %%ymm5, %%ymm7")                // ymm7 = c0 c1 c2 c3 c4 c5 c6 c7
                    __ASM_EMIT("vshufps             $0x4e, %%ymm5, %%ymm4, %%ymm5") // ymm5 = p2 p3 c0 c1 c2 c3 c4 c5
                    __ASM_EMIT("vshufps             $0x99, %%ymm7, %%ymm5, %%ymm6") // ymm6 = p3 c0 c1 c2 c3 c4 c5 c6
                    __ASM_EMIT("vshufps             $0x99, %%ymm5, %%ymm4, %%ymm4") // ymm4 = p1 p2 p3 c0 c1 c2 c3 c4
                    __ASM_EMIT("vmulps              %%ymm3, %%ymm4, %%ymm4")        // ymm4 = k3*p1 ...
                    __ASM_EMIT("vmulps              %%ymm2, %%ymm5, %%ymm5")        // ymm5 = k2*p2 ...
                    __ASM_EMIT("vmulps              %%ymm1, %%ymm6, %%ymm6")        // ymm6 = k1*p3 ...
                    __ASM_EMIT("vaddps              %%ymm5, %%ymm4, %%ymm4")        // ymm4 = k2*p2 + k3*p1
                    __ASM_EMIT("vaddps              (%[d]), %%ymm6, %%ymm6")        // ymm6 = d0 + k1*p3
                    __ASM_EMIT("vmulps              %%ymm0, %%ymm7, %%ymm5")        // ymm5 = k0*c0 ...
                    __ASM_EMIT("vaddps              %%ymm6, %%ymm4, %%ymm4")        // ymm4 = d0 + k1*p3 + k2*p2 + k3*p1
                    __ASM_EMIT("vextractf128        $1, %%ymm7, %%xmm7")            // xmm7 = c4 c5 c6 c7
                    __ASM_EMIT("vaddps              %%ymm5, %%ymm4, %%ymm4")        // ymm4 = d0 + k0*c0 + k1*p3 + k2*p2 + k3*p1
                    __ASM_EMIT("vmovups             %%ymm4, (%[d])")
                    __ASM_EMIT("add                 $0x20, %[c]")                   // c += 8
                    __ASM_EMIT("add                 $0x20, %[d]")                   // d += 8
                    __ASM_EMIT("sub                 $8, %[clen]")                   // clen -= 8
                    __ASM_EMIT("jae                 11b")
                __ASM_EMIT("10:")
                // 4x convolution
                __ASM_EMIT("add                 $4, %[clen]")
                __ASM_EMIT("jl                  12f")
                __ASM_EMIT("vmovaps             %%xmm7, %%xmm4")                // xmm4 = p0 p1 p2 p3
                __ASM_EMIT("vmovups             (%[c]), %%xmm7")                // xmm7 = c0 c1 c2 c3
                __ASM_EMIT("vshufps             $0x4e, %%xmm7, %%xmm4, %%xmm5") // xmm5 = p2 p3 c0 c1
                __ASM_EMIT("vshufps             $0x99, %%xmm7, %%xmm5, %%xmm6") // xmm6 = p3 c0 c1 c2
                __ASM_EMIT("vshufps             $0x99, %%xmm5, %%xmm4, %%xmm4") // xmm4 = p1 p2 p3 c0
                __ASM_EMIT("vmulps              %%xmm3, %%xmm4, %%xmm4")        // xmm4 = k3*p1 ...
                __ASM_EMIT("vmulps              %%xmm2, %%xmm5, %%xmm5")        // xmm5 = k2*p2 ...
                __ASM_EMIT("vmulps              %%xmm1, %%xmm6, %%xmm6")        // xmm6 = k1*p3 ...
                __ASM_EMIT("vaddps              %%xmm5, %%xmm4, %%xmm4")        // xmm4 = k2*p2 + k3*p1
                __ASM_EMIT("vaddps              (%[d]), %%xmm6, %%xmm6")        // xmm6 = d0 + k1*p3
                __ASM_EMIT("vmulps              %%xmm0, %%xmm7, %%xmm5")        // xmm5 = k0*c0 ...
                __ASM_EMIT("vaddps              %%xmm6, %%xmm4, %%xmm4")        // xmm4 = d0 + k1*p3 + k2*p2 + k3*p1
                __ASM_EMIT("vaddps              %%xmm5, %%xmm4, %%xmm4")        // xmm4 = d0 + k0*c0 + k1*p3 + k2*p2 + k3*p1
                __ASM_EMIT("vmovups             %%xmm4, (%[d])")
                __ASM_EMIT("sub                 $4, %[clen]")                   // clen -= 4
                __ASM_EMIT("add                 $0x10, %[c]")                   // c += 4
                __ASM_EMIT("add                 $0x10, %[d]")                   // d += 4
                __ASM_EMIT("12:")
                // 4x tail
                __ASM_EMIT("vmovaps             %%xmm7, %%xmm4")                // xmm4 = p0 p1 p2 p3
                __ASM_EMIT("vxorps              %%xmm7, %%xmm7, %%xmm7")        // xmm7 = 0 0 0 0
                __ASM_EMIT("vmovhlps            %%xmm4, %%xmm7, %%xmm5")        // xmm5 = p2 p3 0 0
                __ASM_EMIT("vshufps             $0x99, %%xmm7, %%xmm5, %%xmm6") // xmm6 = p3 0 0 0
                __ASM_EMIT("vshufps             $0x99, %%xmm5, %%xmm4, %%xmm4") // xmm4 = p1 p2 p3 0
                __ASM_EMIT("vmovlps             0x00(%[d]), %%xmm0, %%xmm0")    // xmm0 = d0 d1
                __ASM_EMIT("vmovss              0x08(%[d]), %%xmm7")            // xmm7 = d2 0
                __ASM_EMIT("vmovlhps            %%xmm7, %%xmm0, %%xmm0")        // xmm0 = d0 d1 d2 0
                __ASM_EMIT("vmulps              %%xmm3, %%xmm4, %%xmm4")        // xmm4 = k3*p1 ...
                __ASM_EMIT("vmulps              %%xmm2, %%xmm5, %%xmm5")        // xmm5 = k2*p2 ...
                __ASM_EMIT("vmulps              %%xmm1, %%xmm6, %%xmm6")        // xmm6 = k1*p3 ...
                __ASM_EMIT("vaddps              %%xmm5, %%xmm4, %%xmm4")        // xmm4 = k2*p2 + k3*p1
                __ASM_EMIT("vaddps              %%xmm0, %%xmm6, %%xmm6")        // xmm6 = d0 + k1*p3
                __ASM_EMIT("vaddps              %%xmm4, %%xmm6, %%xmm6")        // xmm6 = d0 + k1*p3 + k2*p2 + k3*p1
                __ASM_EMIT("vmovhlps            %%xmm6, %%xmm7, %%xmm7")
                __ASM_EMIT("vmovlps             %%xmm6, 0x00(%[d])")
                __ASM_EMIT("vmovss              %%xmm7, 0x08(%[d])")
                // 1x convolution
                __ASM_EMIT("add                 $3, %[clen]")                   // while (clen >= 0)
                __ASM_EMIT("jl                  14f")
                __ASM_EMIT("vmovups             0x00(%[k]), %%xmm1")            // xmm1 = k0 k1 k2 k3
                __ASM_EMIT("15:")
                    __ASM_EMIT("vbroadcastss        0x00(%[c]), %%xmm0")            // xmm0 = c0 c0 c0 c0
                    __ASM_EMIT("vmulps              %%xmm1, %%xmm0, %%xmm0")        // xmm0 = k0*c0 k1*c0 k2*c0 k3*c0
                    __ASM_EMIT("vaddps              0x00(%[d]), %%xmm0, %%xmm0")    // xmm0 = d0+k0*c0 d1+k1*c0 d2+k2*c0 d3+k3*c0
                    __ASM_EMIT("vmovups             %%xmm0, 0x00(%[d])")
                    __ASM_EMIT("add                 $0x04, %[c]")               // c++
                    __ASM_EMIT("add                 $0x04, %[d]")               // d++
                    __ASM_EMIT("dec                 %[clen]")                   // clen--
                    __ASM_EMIT("jge                 15b")
                __ASM_EMIT("14:")
            __ASM_EMIT64("add               $0x10, %[dst]")         // dst += 4
            __ASM_EMIT32("addl              $0x10, %[dst]")
            __ASM_EMIT("add                 $0x10, %[k]")           // k += 4
            __ASM_EMIT("sub                 $0x04, %[count]")       // count -= 4
            __ASM_EMIT("jge                 100b")

            // 1x blocks
            __ASM_EMIT("200:")
            __ASM_EMIT("add                 $3, %[count]")
            __ASM_EMIT("jl                  400f")
            __ASM_EMIT("300:")
                __ASM_EMIT("mov                 %[length], %[clen]")
                __ASM_EMIT("mov                 %[dst], %[d]")
                __ASM_EMIT("vbroadcastss        0x00(%[k]), %%ymm0")            // ymm0 = k0
                __ASM_EMIT("mov                 %[conv], %[c]")
                __ASM_EMIT("vmovaps             %%ymm0, %%ymm1")                // ymm1 = k0
                // 32x convolution
                __ASM_EMIT("sub                 $32, %[clen]")
                __ASM_EMIT("jb                  20f")
                __ASM_EMIT(".align              16")
                __ASM_EMIT("21:")
                    __ASM_EMIT("vmulps              0x00(%[c]), %%ymm0, %%ymm2")        // ymm2 = k0*c0 k0*c1 ...
                    __ASM_EMIT("vmulps              0x20(%[c]), %%ymm1, %%ymm3")
                    __ASM_EMIT("vmulps              0x40(%[c]), %%ymm0, %%ymm4")
                    __ASM_EMIT("vmulps              0x60(%[c]), %%ymm1, %%ymm5")
                    __ASM_EMIT("vaddps              0x00(%[d]), %%ymm2, %%ymm2")        // ymm2 = d0+k0*c0 d1+k0*c1 ...
                    __ASM_EMIT("vaddps              0x20(%[d]), %%ymm3, %%ymm3")
                    __ASM_EMIT("vaddps              0x40(%[d]), %%ymm4, %%ymm4")
                    __ASM_EMIT("vaddps              0x60(%[d]), %%ymm5, %%ymm5")
                    __ASM_EMIT("vmovups             %%ymm2, 0x00(%[d])")
                    __ASM_EMIT("vmovups             %%ymm3, 0x20(%[d])")
                    __ASM_EMIT("vmovups             %%ymm4, 0x40(%[d])")
                    __ASM_EMIT("vmovups             %%ymm5, 0x60(%[d])")
                    __ASM_EMIT("add                 $0x80, %[c]")                       // c += 32
                    __ASM_EMIT("add                 $0x80, %[d]")                       // d += 32
                    __ASM_EMIT("sub                 $32, %[clen]")                      // clen -= 32
                    __ASM_EMIT("jae                 21b")
                __ASM_EMIT("20:")
                // 16x convolution
                __ASM_EMIT("add                 $16, %[clen]")
                __ASM_EMIT("jl                  22f")
                __ASM_EMIT("vmulps              0x00(%[c]), %%ymm0, %%ymm2")        // ymm2 = k0*c0 k0*c1 ...
                __ASM_EMIT("vmulps              0x20(%[c]), %%ymm1, %%ymm3")
                __ASM_EMIT("vaddps              0x00(%[d]), %%ymm2, %%ymm2")        // ymm2 = d0+k0*c0 d1+k0*c1 ...
                __ASM_EMIT("vaddps              0x20(%[d]), %%ymm3, %%ymm3")
                __ASM_EMIT("vmovups             %%ymm2, 0x00(%[d])")
                __ASM_EMIT("vmovups             %%ymm3, 0x20(%[d])")
                __ASM_EMIT("sub                 $16, %[clen]")                      // clen -= 16
                __ASM_EMIT("add                 $0x40, %[c]")                       // c += 16
                __ASM_EMIT("add                 $0x40, %[d]")                       // d += 16
                __ASM_EMIT("22:")
                // 8x convolution
                __ASM_EMIT("add                 $8, %[clen]")
                __ASM_EMIT("jl                  24f")
                __ASM_EMIT("vmulps              0x00(%[c]), %%xmm0, %%xmm2")        // xmm2 = k0*c0 k0*c1 ...
                __ASM_EMIT("vmulps              0x10(%[c]), %%xmm1, %%xmm3")
                __ASM_EMIT("vaddps              0x00(%[d]), %%xmm2, %%xmm2")        // xmm2 = d0+k0*c0 d1+k0*c1 ...
                __ASM_EMIT("vaddps              0x10(%[d]), %%xmm3, %%xmm3")
                __ASM_EMIT("vmovups             %%xmm2, 0x00(%[d])")
                __ASM_EMIT("vmovups             %%xmm3, 0x10(%[d])")
                __ASM_EMIT("sub                 $8, %[clen]")                       // clen -= 8
                __ASM_EMIT("add                 $0x20, %[c]")                       // c += 8
                __ASM_EMIT("add                 $0x20, %[d]")                       // d += 8
                __ASM_EMIT("24:")
                // 4x convolution
                __ASM_EMIT("add                 $4, %[clen]")
                __ASM_EMIT("jl                  26f")
                __ASM_EMIT("vmulps              0x00(%[c]), %%xmm0, %%xmm2")        // xmm2 = k0*c0 k0*c1 ...
                __ASM_EMIT("vaddps              0x00(%[d]), %%xmm2, %%xmm2")        // xmm2 = d0+k0*c0 d1+k0*c1 ...
                __ASM_EMIT("vmovups             %%xmm2, 0x00(%[d])")
                __ASM_EMIT("sub                 $4, %[clen]")                       // clen -= 4
                __ASM_EMIT("add                 $0x10, %[c]")                       // c += 4
                __ASM_EMIT("add                 $0x10, %[d]")                       // d += 4
                __ASM_EMIT("26:")
                // 1x convolution
                __ASM_EMIT("add                 $3, %[clen]")
                __ASM_EMIT("jl                  28f")
                __ASM_EMIT("27:")
                    __ASM_EMIT("vmulss              0x00(%[c]), %%xmm0, %%xmm2")        // xmm2 = k0*c0
                    __ASM_EMIT("vaddss              0x00(%[d]), %%xmm2, %%xmm2")        // xmm2 = d0+k0*c0
                    __ASM_EMIT("vmovss              %%xmm2, 0x00(%[d])")
                    __ASM_EMIT("add                 $0x04, %[c]")                       // c ++
                    __ASM_EMIT("add                 $0x04, %[d]")                       // d ++
                    __ASM_EMIT("dec                 %[clen]")
                    __ASM_EMIT("jge                 27b")
                __ASM_EMIT("28:")
            __ASM_EMIT("add         $0x04, %[k]")
            __ASM_EMIT64("add       $0x04, %[dst]")             // dst++
            __ASM_EMIT32("addl      $0x04, %[dst]")
            __ASM_EMIT("dec         %[count]")
            __ASM_EMIT("jge         300b")

            __ASM_EMIT("400:")
            : [dst] __ASM_ARG_RW(dst),
              [k] "+r" (src), [count] "+r" (count),
              [c] "=&r" (c), [d] "=&r" (d), [clen] "=&r" (clen)
            : [conv] "g" (conv), [length] "g" (length)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void convolve_fma3(float *dst, const float *src, const float *conv, size_t length, size_t count)
    {
        IF_ARCH_X86(
            const float *c;
            float *d;
            size_t clen;
        );

        ARCH_X86_ASM(
            // 4x blocks
            __ASM_EMIT("sub                 $4, %[count]")
            __ASM_EMIT("jb                  200f")
            __ASM_EMIT("100:")
                __ASM_EMIT("vxorps              %%ymm7, %%ymm7, %%ymm7")        // ymm7 = 0
                __ASM_EMIT("mov                 %[length], %[clen]")
                __ASM_EMIT("mov                 %[dst], %[d]")
                __ASM_EMIT("mov                 %[conv], %[c]")

                __ASM_EMIT("vbroadcastss        0x00(%[k]), %%ymm0")            // ymm0 = k0
                __ASM_EMIT("vbroadcastss        0x04(%[k]), %%ymm1")            // ymm1 = k1
                __ASM_EMIT("vbroadcastss        0x08(%[k]), %%ymm2")            // ymm2 = k2
                __ASM_EMIT("vbroadcastss        0x0c(%[k]), %%ymm3")            // ymm3 = k3
                // 8x convolution
                __ASM_EMIT("sub                 $8, %[clen]")
                __ASM_EMIT("jb                  10f")
                __ASM_EMIT(".align              16")
                __ASM_EMIT("11:")
                    __ASM_EMIT("vmovups             (%[c]), %%ymm5")                // ymm5 = c0 c1 c2 c3 c4 c5 c6 c7
                    __ASM_EMIT("vinsertf128         $1, %%xmm5, %%ymm7, %%ymm4")    // ymm4 = p0 p1 p2 p3 c0 c1 c2 c3
                    __ASM_EMIT("vmovaps             %%ymm5, %%ymm7")                // ymm7 = c0 c1 c2 c3 c4 c5 c6 c7
                    __ASM_EMIT("vshufps             $0x4e, %%ymm5, %%ymm4, %%ymm5") // ymm5 = p2 p3 c0 c1 c2 c3 c4 c5
                    __ASM_EMIT("vshufps             $0x99, %%ymm5, %%ymm4, %%ymm4") // ymm4 = p1 p2 p3 c0 c1 c2 c3 c4
                    __ASM_EMIT("vshufps             $0x99, %%ymm7, %%ymm5, %%ymm6") // ymm6 = p3 c0 c1 c2 c3 c4 c5 c6
                    __ASM_EMIT("vfmadd213ps         (%[d]), %%ymm3, %%ymm4")        // ymm4 = d0 + k3*p1 ...
                    __ASM_EMIT("vmulps              %%ymm2, %%ymm5, %%ymm5")        // ymm5 = k2*p2 ...
                    __ASM_EMIT("vfmadd231ps         %%ymm0, %%ymm7, %%ymm4")        // ymm4 = d0 + k0*c0 + k3*p1 ...
                    __ASM_EMIT("vfmadd213ps         %%ymm5, %%ymm1, %%ymm6")        // ymm6 = k1*p3 + k2*p2
                    __ASM_EMIT("vextractf128        $1, %%ymm7, %%xmm7")            // xmm7 = c4 c5 c6 c7
                    __ASM_EMIT("vaddps              %%ymm6, %%ymm4, %%ymm4")        // ymm4 = d0 + k0*c0 + k1*p3 + k2*p2 + k3*p1
                    __ASM_EMIT("vmovups             %%ymm4, (%[d])")
                    __ASM_EMIT("add                 $0x20, %[c]")                   // c += 8
                    __ASM_EMIT("add                 $0x20, %[d]")                   // d += 8
                    __ASM_EMIT("sub                 $8, %[clen]")                   // clen -= 8
                    __ASM_EMIT("jae                 11b")
                __ASM_EMIT("10:")
                // 4x convolution
                __ASM_EMIT("add                 $4, %[clen]")
                __ASM_EMIT("jl                  12f")
                __ASM_EMIT("vmovaps             %%xmm7, %%xmm4")                // xmm4 = p0 p1 p2 p3
                __ASM_EMIT("vmovups             (%[c]), %%xmm7")                // xmm7 = c0 c1 c2 c3
                __ASM_EMIT("vshufps             $0x4e, %%xmm7, %%xmm4, %%xmm5") // xmm5 = p2 p3 c0 c1
                __ASM_EMIT("vshufps             $0x99, %%xmm7, %%xmm5, %%xmm6") // xmm6 = p3 c0 c1 c2
                __ASM_EMIT("vshufps             $0x99, %%xmm5, %%xmm4, %%xmm4") // xmm4 = p1 p2 p3 c0
                __ASM_EMIT("vmulps              %%xmm2, %%xmm5, %%xmm5")        // xmm5 = k2*p2 ...
                __ASM_EMIT("vfmadd213ps         (%[d]), %%xmm3, %%xmm4")        // xmm4 = d0 + k3*p1 ...
                __ASM_EMIT("vfmadd213ps         %%xmm5, %%xmm1, %%xmm6")        // xmm6 = k1*p3 + k2*p2
                __ASM_EMIT("vfmadd231ps         %%xmm0, %%xmm7, %%xmm4")        // xmm4 = d0 + k0*c0 + k3*p1 ...
                __ASM_EMIT("vaddps              %%xmm6, %%xmm4, %%xmm4")        // xmm4 = d0 + k0*c0 + k1*p3 + k2*p2 + k3*p1
                __ASM_EMIT("vmovups             %%xmm4, (%[d])")
                __ASM_EMIT("sub                 $4, %[clen]")                   // clen -= 4
                __ASM_EMIT("add                 $0x10, %[c]")                   // c += 4
                __ASM_EMIT("add                 $0x10, %[d]")                   // d += 4
                __ASM_EMIT("12:")
                // 4x tail
                __ASM_EMIT("vmovaps             %%xmm7, %%xmm4")                // xmm4 = p0 p1 p2 p3
                __ASM_EMIT("vxorps              %%xmm7, %%xmm7, %%xmm7")        // xmm7 = 0 0 0 0
                __ASM_EMIT("vmovhlps            %%xmm4, %%xmm7, %%xmm5")        // xmm5 = p2 p3 0 0
                __ASM_EMIT("vshufps             $0x99, %%xmm7, %%xmm5, %%xmm6") // xmm6 = p3 0 0 0
                __ASM_EMIT("vmovlps             0x00(%[d]), %%xmm0, %%xmm0")    // xmm0 = d0 d1
                __ASM_EMIT("vshufps             $0x99, %%xmm5, %%xmm4, %%xmm4") // xmm4 = p1 p2 p3 0
                __ASM_EMIT("vmovss              0x08(%[d]), %%xmm7")            // xmm7 = d2 0
                __ASM_EMIT("vmovlhps            %%xmm7, %%xmm0, %%xmm0")        // xmm0 = d0 d1 d2 0
                __ASM_EMIT("vmulps              %%xmm3, %%xmm4, %%xmm4")        // xmm4 = k3*p1 ...
                __ASM_EMIT("vfmadd213ps         %%xmm0, %%xmm2, %%xmm5")        // xmm5 = d0 + k2*p2 ...
                __ASM_EMIT("vfmadd213ps         %%xmm4, %%xmm1, %%xmm6")        // xmm6 = k1*p3 + k3*p1 ...
                __ASM_EMIT("vaddps              %%xmm5, %%xmm6, %%xmm6")        // xmm6 = d0 + k1*p3 + k2*p2 + k3*p1
                __ASM_EMIT("vmovhlps            %%xmm6, %%xmm7, %%xmm7")
                __ASM_EMIT("vmovlps             %%xmm6, 0x00(%[d])")
                __ASM_EMIT("vmovss              %%xmm7, 0x08(%[d])")
                // 1x convolution
                __ASM_EMIT("add                 $3, %[clen]")                   // while (clen >= 0)
                __ASM_EMIT("jl                  14f")
                __ASM_EMIT("vmovups             0x00(%[k]), %%xmm1")            // xmm1 = k0 k1 k2 k3
                __ASM_EMIT("15:")
                    __ASM_EMIT("vbroadcastss        0x00(%[c]), %%xmm0")            // xmm0 = c0 c0 c0 c0
                    __ASM_EMIT("vfmadd213ps         0x00(%[d]), %%xmm1, %%xmm0")    // xmm0 = d0+k0*c0 d1+k1*c0 d2+k2*c0 d3+k3*c0
                    __ASM_EMIT("vmovups             %%xmm0, 0x00(%[d])")
                    __ASM_EMIT("add                 $0x04, %[c]")               // c++
                    __ASM_EMIT("add                 $0x04, %[d]")               // d++
                    __ASM_EMIT("dec                 %[clen]")                   // clen--
                    __ASM_EMIT("jge                 15b")
                __ASM_EMIT("14:")
            __ASM_EMIT64("add               $0x10, %[dst]")         // dst += 4
            __ASM_EMIT32("addl              $0x10, %[dst]")
            __ASM_EMIT("add                 $0x10, %[k]")           // k += 4
            __ASM_EMIT("sub                 $0x04, %[count]")       // count -= 4
            __ASM_EMIT("jge                 100b")

            // 1x blocks
            __ASM_EMIT("200:")
            __ASM_EMIT("add                 $3, %[count]")
            __ASM_EMIT("jl                  400f")
            __ASM_EMIT("300:")
                __ASM_EMIT("mov                 %[length], %[clen]")
                __ASM_EMIT("mov                 %[dst], %[d]")
                __ASM_EMIT("vbroadcastss        0x00(%[k]), %%ymm0")            // ymm0 = k0
                __ASM_EMIT("mov                 %[conv], %[c]")
                __ASM_EMIT("vmovaps             %%ymm0, %%ymm1")                // ymm1 = k0
                // 32x convolution
                __ASM_EMIT("sub                 $32, %[clen]")
                __ASM_EMIT("jb                  20f")
                __ASM_EMIT(".align              16")
                __ASM_EMIT("21:")
                    __ASM_EMIT("vmovups             0x00(%[c]), %%ymm2")                // ymm2 = c0 c1 ...
                    __ASM_EMIT("vmovups             0x20(%[c]), %%ymm3")
                    __ASM_EMIT("vmovups             0x40(%[c]), %%ymm4")
                    __ASM_EMIT("vmovups             0x60(%[c]), %%ymm5")
                    __ASM_EMIT("vfmadd213ps         0x00(%[d]), %%ymm0, %%ymm2")        // ymm2 = d0+k0*c0 d1+k0*c1 ...
                    __ASM_EMIT("vfmadd213ps         0x20(%[d]), %%ymm1, %%ymm3")
                    __ASM_EMIT("vfmadd213ps         0x40(%[d]), %%ymm0, %%ymm4")
                    __ASM_EMIT("vfmadd213ps         0x60(%[d]), %%ymm1, %%ymm5")
                    __ASM_EMIT("vmovups             %%ymm2, 0x00(%[d])")
                    __ASM_EMIT("vmovups             %%ymm3, 0x20(%[d])")
                    __ASM_EMIT("vmovups             %%ymm4, 0x40(%[d])")
                    __ASM_EMIT("vmovups             %%ymm5, 0x60(%[d])")
                    __ASM_EMIT("add                 $0x80, %[c]")                       // c += 32
                    __ASM_EMIT("add                 $0x80, %[d]")                       // d += 32
                    __ASM_EMIT("sub                 $32, %[clen]")                      // clen -= 32
                    __ASM_EMIT("jae                 21b")
                __ASM_EMIT("20:")
                // 16x convolution
                __ASM_EMIT("add                 $16, %[clen]")
                __ASM_EMIT("jl                  22f")
                __ASM_EMIT("vmovups             0x00(%[c]), %%ymm2")                // ymm2 = c0 c1 ...
                __ASM_EMIT("vmovups             0x20(%[c]), %%ymm3")
                __ASM_EMIT("vfmadd213ps         0x00(%[d]), %%ymm0, %%ymm2")        // ymm2 = d0+k0*c0 d1+k0*c1 ...
                __ASM_EMIT("vfmadd213ps         0x20(%[d]), %%ymm1, %%ymm3")
                __ASM_EMIT("vmovups             %%ymm2, 0x00(%[d])")
                __ASM_EMIT("vmovups             %%ymm3, 0x20(%[d])")
                __ASM_EMIT("sub                 $16, %[clen]")                      // clen -= 16
                __ASM_EMIT("add                 $0x40, %[c]")                       // c += 16
                __ASM_EMIT("add                 $0x40, %[d]")                       // d += 16
                __ASM_EMIT("22:")
                // 8x convolution
                __ASM_EMIT("add                 $8, %[clen]")
                __ASM_EMIT("jl                  24f")
                __ASM_EMIT("vmovups             0x00(%[c]), %%xmm2")                // xmm2 = c0 c1 ...
                __ASM_EMIT("vmovups             0x10(%[c]), %%xmm3")
                __ASM_EMIT("vfmadd213ps         0x00(%[d]), %%xmm0, %%xmm2")        // xmm2 = d0+k0*c0 d1+k0*c1 ...
                __ASM_EMIT("vfmadd213ps         0x10(%[d]), %%xmm1, %%xmm3")
                __ASM_EMIT("vmovups             %%xmm2, 0x00(%[d])")
                __ASM_EMIT("vmovups             %%xmm3, 0x10(%[d])")
                __ASM_EMIT("sub                 $8, %[clen]")                       // clen -= 8
                __ASM_EMIT("add                 $0x20, %[c]")                       // c += 8
                __ASM_EMIT("add                 $0x20, %[d]")                       // d += 8
                __ASM_EMIT("24:")
                // 4x convolution
                __ASM_EMIT("add                 $4, %[clen]")
                __ASM_EMIT("jl                  26f")
                __ASM_EMIT("vmulps              0x00(%[c]), %%xmm0, %%xmm2")        // xmm2 = k0*c0 k0*c1 ...
                __ASM_EMIT("vaddps              0x00(%[d]), %%xmm2, %%xmm2")        // xmm2 = d0+k0*c0 d1+k0*c1 ...
                __ASM_EMIT("vmovups             %%xmm2, 0x00(%[d])")
                __ASM_EMIT("sub                 $4, %[clen]")                       // clen -= 4
                __ASM_EMIT("add                 $0x10, %[c]")                       // c += 4
                __ASM_EMIT("add                 $0x10, %[d]")                       // d += 4
                __ASM_EMIT("26:")
                // 1x convolution
                __ASM_EMIT("add                 $3, %[clen]")
                __ASM_EMIT("jl                  28f")
                __ASM_EMIT("27:")
                    __ASM_EMIT("vmulss              0x00(%[c]), %%xmm0, %%xmm2")        // xmm2 = k0*c0
                    __ASM_EMIT("vaddss              0x00(%[d]), %%xmm2, %%xmm2")        // xmm2 = d0+k0*c0
                    __ASM_EMIT("vmovss              %%xmm2, 0x00(%[d])")
                    __ASM_EMIT("add                 $0x04, %[c]")                       // c ++
                    __ASM_EMIT("add                 $0x04, %[d]")                       // d ++
                    __ASM_EMIT("dec                 %[clen]")
                    __ASM_EMIT("jge                 27b")
                __ASM_EMIT("28:")
            __ASM_EMIT("add         $0x04, %[k]")
            __ASM_EMIT64("add       $0x04, %[dst]")             // dst++
            __ASM_EMIT32("addl      $0x04, %[dst]")
            __ASM_EMIT("dec         %[count]")
            __ASM_EMIT("jge         300b")

            __ASM_EMIT("400:")
            : [dst] __ASM_ARG_RW(dst),
              [k] "+r" (src), [count] "+r" (count),
              [c] "=&r" (c), [d] "=&r" (d), [clen] "=&r" (clen)
            : [conv] "g" (conv), [length] "g" (length)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }
}

#endif /* DSP_ARCH_X86_AVX_CONVOLUTION_H_ */
