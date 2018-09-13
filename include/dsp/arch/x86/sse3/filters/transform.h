/*
 * transform.h
 *
 *  Created on: 23 февр. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_SSE3_FILTERS_TRANSFORM_H_
#define DSP_ARCH_X86_SSE3_FILTERS_TRANSFORM_H_

#ifndef DSP_ARCH_X86_SSE3_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_SSE3_IMPL */

namespace sse3
{
    #ifdef ARCH_X86_64

    void x64_bilinear_transform_x8(biquad_x8_t *bf, const f_cascade_t *bc, float kf, size_t count)
    {
        float x1, x2, x3, x4, x5, x6, x7;
        float x8, x9, x10, x11, x12, x13, x14, x15;

        // Same ad bilinear_tranform_x1 but number of cycles is doubled
        ARCH_X86_ASM
        (
            __ASM_EMIT("test        %[count], %[count]")
            __ASM_EMIT("jz          100f")

            // Initialize
            __ASM_EMIT("shufps      $0x00, %[x0], %[x0]")       // x0 = kf kf kf kf
            __ASM_EMIT("movaps      %[x0], %[x1]")              // x1 = kf kf kf kf
            __ASM_EMIT("mulps       %[x1], %[x1]")              // x1 = kf*kf kf*kf kf*kf kf*kf = kf2 kf2 kf2 kf2

            //---------------------------------------------------------------------
            // Load bottom part of cascade and transpose
            __ASM_EMIT("1:")
            // Load bottom part
            __ASM_EMIT("movaps      0x10(%[bc]), %[x2]")        // x2 = b0[0] b1[0] b2[0] ?
            __ASM_EMIT("movaps      0x30(%[bc]), %[x3]")        // x3 = b0[1] b1[1] b2[1] ?
            __ASM_EMIT("movaps      0x50(%[bc]), %[x4]")        // x4 = b0[2] b1[2] b2[2] ?
            __ASM_EMIT("movaps      0x70(%[bc]), %[x5]")        // x5 = b0[3] b1[3] b2[3] ?
            __ASM_EMIT("movaps      0x90(%[bc]), %[x10]")
            __ASM_EMIT("movaps      0xb0(%[bc]), %[x11]")
            __ASM_EMIT("movaps      0xd0(%[bc]), %[x12]")
            __ASM_EMIT("movaps      0xf0(%[bc]), %[x13]")

            // Transpose
            __ASM_EMIT("movaps      %[x2], %[x6]")              // x6 = b0[0] b1[0] b2[0]   ?
            __ASM_EMIT("movaps      %[x10], %[x14]")
            __ASM_EMIT("movaps      %[x4], %[x7]")              // x7 = b0[2] b1[2] b2[2]   ?
            __ASM_EMIT("movaps      %[x12], %[x15]")
            __ASM_EMIT("unpcklps    %[x3], %[x2]")              // x2 = b0[0] b0[1] b1[0] b1[1]
            __ASM_EMIT("unpcklps    %[x11], %[x10]")
            __ASM_EMIT("unpcklps    %[x5], %[x4]")              // x4 = b0[2] b0[3] b1[2] b1[3]
            __ASM_EMIT("unpcklps    %[x13], %[x12]")
            __ASM_EMIT("unpckhps    %[x3], %[x6]")              // x6 = b2[0] b2[1]   ?     ?
            __ASM_EMIT("unpckhps    %[x11], %[x14]")
            __ASM_EMIT("unpckhps    %[x5], %[x7]")              // x7 = b2[2] b2[3]   ?     ?
            __ASM_EMIT("unpckhps    %[x13], %[x15]")
            __ASM_EMIT("movaps      %[x2], %[x3]")              // x3 = b0[0] b0[1] b1[0] b1[1]
            __ASM_EMIT("movaps      %[x10], %[x11]")
            __ASM_EMIT("movlhps     %[x7], %[x6]")              // x6 = b2[0] b2[1] b2[2] b2[3]
            __ASM_EMIT("movlhps     %[x15], %[x14]")
            __ASM_EMIT("movlhps     %[x4], %[x2]")              // x2 = b0[0] b0[1] b0[2] b0[3]
            __ASM_EMIT("movlhps     %[x12], %[x10]")
            __ASM_EMIT("movhlps     %[x3], %[x4]")              // x4 = b1[0] b1[1] b1[2] b1[3]
            __ASM_EMIT("movhlps     %[x11], %[x12]")

            // Calc bottom
            __ASM_EMIT("movaps      %[x2], %[x7]")              // x7 = B0[k] = b0[k]
            __ASM_EMIT("movaps      %[x10], %[x15]")
            __ASM_EMIT("mulps       %[x0], %[x4]")              // x4 = B1[k] = b1[k]*kf
            __ASM_EMIT("mulps       %[x1], %[x6]")              // x6 = B2[k] = b2[k]*kf2
            __ASM_EMIT("mulps       %[x0], %[x12]")
            __ASM_EMIT("mulps       %[x1], %[x14]")
            __ASM_EMIT("addps       %[x4], %[x7]")              // x7 = B0[k] + B1[k]
            __ASM_EMIT("addps       %[x12], %[x15]")
            __ASM_EMIT("movaps      %[ONE], %[x3]")             // x3 = 1
            __ASM_EMIT("addps       %[x6], %[x7]")              // x7 = B0[k] + B1[k] + B2[k]
            __ASM_EMIT("movaps      %[x3], %[x11]")
            __ASM_EMIT("addps       %[x14], %[x15]")
            __ASM_EMIT("divps       %[x7], %[x3]")              // x3 = 1 / (B0[k] + B1[k] + B2[k])
            __ASM_EMIT("divps       %[x15], %[x11]")
            __ASM_EMIT("movaps      %[x6], %[x5]")              // x5 = B2[k]
            __ASM_EMIT("movaps      %[x14], %[x13]")
            __ASM_EMIT("addps       %[x2], %[x6]")              // x6 = B2[k] + B0[k]
            __ASM_EMIT("addps       %[x10], %[x14]")
            __ASM_EMIT("subps       %[x2], %[x5]")              // x5 = B2[k] - B0[k]
            __ASM_EMIT("subps       %[x10], %[x13]")
            __ASM_EMIT("subps       %[x6], %[x4]")              // x4 = B1[k] - (B2[k] + B0[k]) = B1[k] - B2[k] - B0[k]
            __ASM_EMIT("subps       %[x14], %[x12]")
            __ASM_EMIT("addps       %[x5], %[x5]")              // x5 = 2 * (B2[k] - B0[k])
            __ASM_EMIT("addps       %[x13], %[x13]")
            __ASM_EMIT("mulps       %[x3], %[x4]")              // x4 = fb1[k] = (B1[k] - B2[k] - B0[k]) * N[k]
            __ASM_EMIT("mulps       %[x11], %[x12]")
            __ASM_EMIT("mulps       %[x3], %[x5]")              // x5 = fb0[k] = 2 * (B2[k] - B0[k]) * N[k]
            __ASM_EMIT("mulps       %[x11], %[x13]")

            // Save bottom part
            __ASM_EMIT("movaps      %[x3],  %[x8]")             // x8 = N1
            __ASM_EMIT("movaps      %[x11], %[x9]")             // x9 = N2
            __ASM_EMIT("movaps      %[x5],  0x60(%[bf])")
            __ASM_EMIT("movaps      %[x13], 0x70(%[bf])")
            __ASM_EMIT("movaps      %[x4],  0x80(%[bf])")
            __ASM_EMIT("movaps      %[x12], 0x90(%[bf])")

            // Load Top part of cascade and transpose
            __ASM_EMIT("movaps      0x00(%[bc]), %[x2]")        // x2 = a0[0] a1[0] a2[0] ?
            __ASM_EMIT("movaps      0x20(%[bc]), %[x3]")        // x3 = a0[1] a1[1] a2[1] ?
            __ASM_EMIT("movaps      0x40(%[bc]), %[x4]")        // x4 = a0[2] a1[2] a2[2] ?
            __ASM_EMIT("movaps      0x60(%[bc]), %[x5]")        // x5 = a0[3] a1[3] a2[3] ?
            __ASM_EMIT("movaps      0x80(%[bc]), %[x10]")
            __ASM_EMIT("movaps      0xa0(%[bc]), %[x11]")
            __ASM_EMIT("movaps      0xc0(%[bc]), %[x12]")
            __ASM_EMIT("movaps      0xe0(%[bc]), %[x13]")

            // Transpose
            __ASM_EMIT("movaps      %[x2], %[x6]")              // x6 = b0[0] b1[0] b2[0]   ?
            __ASM_EMIT("movaps      %[x10], %[x14]")
            __ASM_EMIT("movaps      %[x4], %[x7]")              // x7 = b0[2] b1[2] b2[2]   ?
            __ASM_EMIT("movaps      %[x12], %[x15]")
            __ASM_EMIT("unpcklps    %[x3], %[x2]")              // x2 = b0[0] b0[1] b1[0] b1[1]
            __ASM_EMIT("unpcklps    %[x11], %[x10]")
            __ASM_EMIT("unpcklps    %[x5], %[x4]")              // x4 = b0[2] b0[3] b1[2] b1[3]
            __ASM_EMIT("unpcklps    %[x13], %[x12]")
            __ASM_EMIT("unpckhps    %[x3], %[x6]")              // x6 = b2[0] b2[1]   ?     ?
            __ASM_EMIT("unpckhps    %[x11], %[x14]")
            __ASM_EMIT("unpckhps    %[x5], %[x7]")              // x7 = b2[2] b2[3]   ?     ?
            __ASM_EMIT("unpckhps    %[x13], %[x15]")
            __ASM_EMIT("movaps      %[x2], %[x3]")              // x3 = b0[0] b0[1] b1[0] b1[1]
            __ASM_EMIT("movaps      %[x10], %[x11]")
            __ASM_EMIT("movlhps     %[x7], %[x6]")              // x6 = b2[0] b2[1] b2[2] b2[3]
            __ASM_EMIT("movlhps     %[x15], %[x14]")
            __ASM_EMIT("movlhps     %[x4], %[x2]")              // x2 = b0[0] b0[1] b0[2] b0[3]
            __ASM_EMIT("movlhps     %[x12], %[x10]")
            __ASM_EMIT("movhlps     %[x3], %[x4]")              // x4 = b1[0] b1[1] b1[2] b1[3]
            __ASM_EMIT("movhlps     %[x11], %[x12]")

            // Process top part
            __ASM_EMIT("mulps       %[x0], %[x4]")              // x4 = T1[k] = a1[k]*kf
            __ASM_EMIT("mulps       %[x1], %[x6]")              // x6 = T2[k] = a2[k]*kf2
            __ASM_EMIT("mulps       %[x0], %[x12]")
            __ASM_EMIT("mulps       %[x1], %[x14]")
            __ASM_EMIT("movaps      %[x2], %[x3]")              // x3 = T0[k]
            __ASM_EMIT("movaps      %[x10], %[x11]")
            __ASM_EMIT("addps       %[x6], %[x2]")              // x2 = T0[k] + T2[k]
            __ASM_EMIT("addps       %[x14], %[x10]")
            __ASM_EMIT("subps       %[x6], %[x3]")              // x3 = T0[k] - T2[k]
            __ASM_EMIT("subps       %[x14], %[x11]")
            __ASM_EMIT("movaps      %[x4], %[x6]")              // x6 = T1[k]
            __ASM_EMIT("movaps      %[x12], %[x14]")
            __ASM_EMIT("addps       %[x3], %[x3]")              // x3 = 2*(T0[k] - T2[k])
            __ASM_EMIT("addps       %[x11], %[x11]")
            __ASM_EMIT("addps       %[x2], %[x4]")              // x4 = T0[k] + T1[k] + T2[k]
            __ASM_EMIT("addps       %[x10], %[x12]")
            __ASM_EMIT("mulps       %[x8], %[x3]")              // x3 = fa2[k] = 2*(T0[k] - T2[k]) * N
            __ASM_EMIT("mulps       %[x9], %[x11]")
            __ASM_EMIT("subps       %[x6], %[x2]")              // x2 = T0[k] - T1[k] + T2[k]
            __ASM_EMIT("subps       %[x14], %[x10]")
            __ASM_EMIT("mulps       %[x8], %[x4]")              // x4 = fa0[k] = (T0[k] + T1[k] + T2[k]) * N
            __ASM_EMIT("mulps       %[x9], %[x12]")
            __ASM_EMIT("mulps       %[x8], %[x2]")              // x2 = fa3[k] = (T0[k] - T1[k] + T2[k]) * N
            __ASM_EMIT("mulps       %[x9], %[x10]")

            // Save top part
            __ASM_EMIT("movaps      %[x4],  0x00(%[bf])")
            __ASM_EMIT("movaps      %[x12], 0x10(%[bf])")
            __ASM_EMIT("movaps      %[x3],  0x20(%[bf])")
            __ASM_EMIT("movaps      %[x11], 0x30(%[bf])")
            __ASM_EMIT("movaps      %[x2],  0x40(%[bf])")
            __ASM_EMIT("movaps      %[x10], 0x50(%[bf])")

            // Update pointers and repeat loop
            __ASM_EMIT("add         $0x100, %[bc]")
            __ASM_EMIT("add         $0xa0,  %[bf]")
            __ASM_EMIT("dec         %[count]")
            __ASM_EMIT("jnz         1b")

            __ASM_EMIT("100:")
            : [x0] "+x" (kf), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3),
              [x4] "=&x" (x4), [x5] "=&x" (x5), [x6] "=&x" (x6), [x7] "=&x" (x7),
              [x8] "=&x" (x8), [x9] "=&x" (x9), [x10] "=&x" (x10), [x11] "=&x" (x11),
              [x12] "=&x" (x12), [x13] "=&x" (x13), [x14] "=&x" (x14), [x15] "=&x" (x15),
              [count] "+r" (count),
              [bc] "+r" (bc),
              [bf] "+r" (bf)
            : [ONE] "m" (ONE)
            : "cc", "memory"
        );
    }

    #endif /* ARCH_X86_64 */
}

#endif /* DSP_ARCH_X86_SSE3_FILTERS_TRANSFORM_H_ */
