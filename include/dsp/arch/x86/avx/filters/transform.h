/*
 * transform.h
 *
 *  Created on: 24 февр. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_AVX_FILTERS_TRANSFORM_H_
#define DSP_ARCH_X86_AVX_FILTERS_TRANSFORM_H_

#ifndef DSP_ARCH_X86_AVX_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_AVX_IMPL */

namespace avx
{
    void bilinear_transform_x1(biquad_x1_t *bf, const f_cascade_t *bc, float kf, size_t count)
    {
        float DATA[12] __lsp_aligned16;

        ARCH_X86_ASM
        (
            __ASM_EMIT("test                %[count], %[count]")
            __ASM_EMIT("jz                  100f")

            // Initialize
            __ASM_EMIT("vbroadcastss        %[kf], %%xmm0")             // xmm0 = kf
            __ASM_EMIT("vmulps              %%xmm0, %%xmm0, %%xmm1")    // xmm1 = kf*kf = kf2

            // Perform x4 blocks
            __ASM_EMIT("sub                 $4, %[count]")
            __ASM_EMIT("jb                  2f")

            // Load bottom part of cascade and transpose
            __ASM_EMIT(".align              16")
            __ASM_EMIT("1:")
            __ASM_EMIT("vmovaps             0x10(%[bc]), %%xmm2")       // xmm2 = b00 b10 b20 ?
            __ASM_EMIT("vmovaps             0x30(%[bc]), %%xmm3")       // xmm3 = b01 b11 b21 ?
            __ASM_EMIT("vmovaps             0x50(%[bc]), %%xmm4")       // xmm4 = b02 b12 b22 ?
            __ASM_EMIT("vmovaps             0x70(%[bc]), %%xmm5")       // xmm5 = b03 b13 b23 ?

            __ASM_EMIT("vunpckhps           %%xmm5, %%xmm3, %%xmm7")    // xmm7 = b21 b23 ?   ?
            __ASM_EMIT("vunpcklps           %%xmm5, %%xmm3, %%xmm6")    // xmm6 = b01 b03 b11 b13
            __ASM_EMIT("vunpckhps           %%xmm4, %%xmm2, %%xmm5")    // xmm5 = b20 b22 ?   ?
            __ASM_EMIT("vunpcklps           %%xmm4, %%xmm2, %%xmm3")    // xmm3 = b00 b02 b10 b12
            __ASM_EMIT("vunpcklps           %%xmm6, %%xmm3, %%xmm2")    // xmm2 = b00 b01 b02 b03
            __ASM_EMIT("vunpckhps           %%xmm6, %%xmm3, %%xmm3")    // xmm3 = b10 b11 b12 b13
            __ASM_EMIT("vunpcklps           %%xmm7, %%xmm5, %%xmm4")    // xmm4 = b20 b21 b22 b23

            // Compute bottom part
            // xmm2 = b0, xmm3 = b1, xmm4 = b2
            __ASM_EMIT("vmulps              %%xmm0, %%xmm3, %%xmm3")    // xmm3 = B1 = b1*kf
            __ASM_EMIT("vmulps              %%xmm1, %%xmm4, %%xmm4")    // xmm4 = B2 = b2*kf2
            __ASM_EMIT("vaddps              %%xmm2, %%xmm3, %%xmm5")    // xmm5 = B0 + B1
            __ASM_EMIT("vmovaps             %[ONE], %%xmm6")            // xmm6 = 1
            __ASM_EMIT("vaddps              %%xmm4, %%xmm5, %%xmm5")    // xmm5 = B0 + B1 + B2
            __ASM_EMIT("vaddps              %%xmm4, %%xmm2, %%xmm7")    // xmm7 = B2 + B0
            __ASM_EMIT("vdivps              %%xmm5, %%xmm6, %%xmm5")    // xmm5 = N = 1 / (B0 + B1 + B2)
            __ASM_EMIT("vsubps              %%xmm2, %%xmm4, %%xmm6")    // xmm6 = B2 - B0
            __ASM_EMIT("vsubps              %%xmm7, %%xmm3, %%xmm7")    // xmm7 = B1 - (B2 + B0) = B1 - B2 + B0
            __ASM_EMIT("vaddps              %%xmm6, %%xmm6, %%xmm6")    // xmm6 = 2*(B2 - B0)
            __ASM_EMIT("vmulps              %%xmm5, %%xmm7, %%xmm7")    // xmm7 = b2' = (B1 - B2 + B0) * N
            __ASM_EMIT("vmulps              %%xmm5, %%xmm6, %%xmm6")    // xmm6 = b1' = 2 * (B2 - B0) * N
            __ASM_EMIT("vmovaps             %%xmm5, 0x00 + %[DATA]")    // store N
            __ASM_EMIT("vmovaps             %%xmm6, 0x10 + %[DATA]")    // store b1
            __ASM_EMIT("vmovaps             %%xmm7, 0x20 + %[DATA]")    // store b2

            // Load Top part of cascade and transpose
            __ASM_EMIT("vmovaps             0x00(%[bc]), %%xmm2")       // xmm2 = t0[0] t1[0] t2[0] ?
            __ASM_EMIT("vmovaps             0x20(%[bc]), %%xmm3")       // xmm3 = t0[1] t1[1] t2[1] ?
            __ASM_EMIT("vmovaps             0x40(%[bc]), %%xmm4")       // xmm4 = t0[2] t1[2] t2[2] ?
            __ASM_EMIT("vmovaps             0x60(%[bc]), %%xmm5")       // xmm5 = t0[3] t1[3] t2[3] ?

            __ASM_EMIT("vunpckhps           %%xmm5, %%xmm3, %%xmm7")    // xmm7 = t21 t23 ?   ?
            __ASM_EMIT("vunpcklps           %%xmm5, %%xmm3, %%xmm6")    // xmm6 = t01 t03 t11 t13
            __ASM_EMIT("vunpckhps           %%xmm4, %%xmm2, %%xmm5")    // xmm5 = t20 t22 ?   ?
            __ASM_EMIT("vunpcklps           %%xmm4, %%xmm2, %%xmm3")    // xmm3 = t00 t02 t10 t12
            __ASM_EMIT("vunpcklps           %%xmm6, %%xmm3, %%xmm2")    // xmm2 = t00 t01 t02 t03
            __ASM_EMIT("vunpckhps           %%xmm6, %%xmm3, %%xmm3")    // xmm3 = t10 t11 t12 t13
            __ASM_EMIT("vunpcklps           %%xmm7, %%xmm5, %%xmm4")    // xmm4 = t20 t21 t22 t23

            // x2 = T0 = t0, x3=t1, x4=t2
            __ASM_EMIT("vmovaps             0x00 + %[DATA], %%xmm7")    // load N
            __ASM_EMIT("vmulps              %%xmm1, %%xmm4, %%xmm6")    // xmm6 = T2 = t2*kf2
            __ASM_EMIT("vmulps              %%xmm0, %%xmm3, %%xmm5")    // xmm5 = T1 = t1*kf
            __ASM_EMIT("vaddps              %%xmm6, %%xmm2, %%xmm4")    // xmm4 = T0 + T2
            __ASM_EMIT("vsubps              %%xmm6, %%xmm2, %%xmm3")    // xmm3 = T0 - T2
            __ASM_EMIT("vaddps              %%xmm5, %%xmm4, %%xmm2")    // xmm2 = T0 + T1 + T2
            __ASM_EMIT("vaddps              %%xmm3, %%xmm3, %%xmm3")    // xmm3 = 2*(T0 - T2)
            __ASM_EMIT("vmulps              %%xmm7, %%xmm2, %%xmm2")    // xmm2 = a0 = (T0 + T1 + T2) * N
            __ASM_EMIT("vsubps              %%xmm5, %%xmm4, %%xmm4")    // xmm4 = T0 - T1 + T2
            __ASM_EMIT("vmulps              %%xmm7, %%xmm3, %%xmm3")    // xmm3 = a1 = 2*(T0 - T2) * N
            __ASM_EMIT("vmovaps             0x10 + %[DATA], %%xmm5")    // xmm5 = b1
            __ASM_EMIT("vmulps              %%xmm7, %%xmm4, %%xmm4")    // xmm4 = a2 = (T0 - T1 + T2) * N

            __ASM_EMIT("vunpckhps           %%xmm5, %%xmm3, %%xmm7")
            __ASM_EMIT("vunpcklps           %%xmm5, %%xmm3, %%xmm6")
            __ASM_EMIT("vunpckhps           %%xmm4, %%xmm2, %%xmm5")
            __ASM_EMIT("vunpcklps           %%xmm4, %%xmm2, %%xmm3")
            __ASM_EMIT("vunpcklps           %%xmm6, %%xmm3, %%xmm2")
            __ASM_EMIT("vunpckhps           %%xmm6, %%xmm3, %%xmm3")
            __ASM_EMIT("vunpcklps           %%xmm7, %%xmm5, %%xmm4")
            __ASM_EMIT("vunpckhps           %%xmm7, %%xmm5, %%xmm5")

            __ASM_EMIT("vmovss              0x20 + %[DATA], %%xmm6")    // x6 = b1[0] 0 0 0
            __ASM_EMIT("vmovss              0x24 + %[DATA], %%xmm7")    // x7 = b1[1] 0 0 0
            __ASM_EMIT("vmovaps             %%xmm2, 0x00(%[bf])")
            __ASM_EMIT("vmovaps             %%xmm6, 0x10(%[bf])")
            __ASM_EMIT("vmovaps             %%xmm3, 0x20(%[bf])")
            __ASM_EMIT("vmovaps             %%xmm7, 0x30(%[bf])")
            __ASM_EMIT("vmovss              0x28 + %[DATA], %%xmm6")    // x4 = b1[2] 0 0 0
            __ASM_EMIT("vmovss              0x2c + %[DATA], %%xmm7")    // x5 = b1[3] 0 0 0
            __ASM_EMIT("vmovaps             %%xmm4, 0x40(%[bf])")
            __ASM_EMIT("vmovaps             %%xmm6, 0x50(%[bf])")
            __ASM_EMIT("vmovaps             %%xmm5, 0x60(%[bf])")
            __ASM_EMIT("vmovaps             %%xmm7, 0x70(%[bf])")

            // Update pointers and repeat loop
            __ASM_EMIT("add                 $0x80, %[bc]")
            __ASM_EMIT("add                 $0x80, %[bf]")
            __ASM_EMIT("sub                 $4, %[count]")
            __ASM_EMIT("jae                 1b")

            // Single-item loop
            __ASM_EMIT("2:")
            __ASM_EMIT("add                 $4, %[count]")
            __ASM_EMIT("jz                  100f")

            // Calculate bottom part
            __ASM_EMIT("3:")
            __ASM_EMIT("vmovss              0x10(%[bc]), %%xmm2")       // x2 = B0 = b0
            __ASM_EMIT("vmovss              0x14(%[bc]), %%xmm3")       // x3 = b1
            __ASM_EMIT("vmovss              0x18(%[bc]), %%xmm4")       // x4 = b2
            __ASM_EMIT("vmulss              %%xmm0, %%xmm3, %%xmm3")    // xmm3 = B1 = b1*kf
            __ASM_EMIT("vmulss              %%xmm1, %%xmm4, %%xmm4")    // xmm4 = B2 = b2*kf2
            __ASM_EMIT("vaddss              %%xmm2, %%xmm3, %%xmm7")    // xmm7 = B0 + B1
            __ASM_EMIT("vmovaps             %[ONE], %%xmm6")            // xmm6 = 1
            __ASM_EMIT("vaddss              %%xmm4, %%xmm7, %%xmm7")    // xmm7 = B0 + B1 + B2
            __ASM_EMIT("vaddss              %%xmm4, %%xmm2, %%xmm5")    // xmm5 = B2 + B0
            __ASM_EMIT("vdivss              %%xmm7, %%xmm6, %%xmm7")    // xmm7 = N = 1 / (B0 + B1 + B2)
            __ASM_EMIT("vsubss              %%xmm2, %%xmm4, %%xmm6")    // xmm6 = B2 - B0
            __ASM_EMIT("vsubss              %%xmm5, %%xmm3, %%xmm5")    // xmm5 = B1 - (B2 + B0) = B1 - B2 + B0
            __ASM_EMIT("vaddss              %%xmm6, %%xmm6, %%xmm6")    // xmm6 = 2*(B2 - B0)
            __ASM_EMIT("vmulss              %%xmm7, %%xmm5, %%xmm5")    // xmm5 = b2' = (B1 - B2 + B0) * N
            __ASM_EMIT("vmulss              %%xmm7, %%xmm6, %%xmm6")    // xmm6 = b1' = 2 * (B2 - B0) * N
            __ASM_EMIT("vmovss              %%xmm6, 0x0c(%[bf])")
            __ASM_EMIT("vmovaps             %%xmm5, 0x10(%[bf])")

            // Calculate top part
            // xmm7 = N
            __ASM_EMIT("vmovss              0x00(%[bc]), %%xmm2")       // x2 = T0 = t0
            __ASM_EMIT("vmovss              0x04(%[bc]), %%xmm3")       // x3 = t1
            __ASM_EMIT("vmovss              0x08(%[bc]), %%xmm4")       // x4 = t2
            __ASM_EMIT("vmulss              %%xmm1, %%xmm4, %%xmm6")    // xmm6 = T2 = t2*kf2
            __ASM_EMIT("vmulss              %%xmm0, %%xmm3, %%xmm5")    // xmm5 = T1 = t1*kf
            __ASM_EMIT("vaddss              %%xmm6, %%xmm2, %%xmm4")    // xmm4 = T0 + T2
            __ASM_EMIT("vsubss              %%xmm6, %%xmm2, %%xmm3")    // xmm3 = T0 - T2
            __ASM_EMIT("vaddss              %%xmm5, %%xmm4, %%xmm2")    // xmm2 = T0 + T1 + T2
            __ASM_EMIT("vaddss              %%xmm3, %%xmm3, %%xmm3")    // xmm3 = 2*(T0 - T2)
            __ASM_EMIT("vmulss              %%xmm7, %%xmm2, %%xmm2")    // xmm2 = a0 = (T0 + T1 + T2) * N
            __ASM_EMIT("vsubss              %%xmm5, %%xmm4, %%xmm4")    // xmm4 = T0 - T1 + T2
            __ASM_EMIT("vmulss              %%xmm7, %%xmm3, %%xmm3")    // xmm3 = a1 = 2*(T0 - T2) * N
            __ASM_EMIT("vmulss              %%xmm7, %%xmm4, %%xmm4")    // xmm4 = a2 = (T0 - T1 + T2) * N
            __ASM_EMIT("vmovss              %%xmm2, 0x00(%[bf])")
            __ASM_EMIT("vmovss              %%xmm3, 0x04(%[bf])")
            __ASM_EMIT("vmovss              %%xmm4, 0x08(%[bf])")

            // Repeat loop
            __ASM_EMIT("add                 $0x20, %[bc]")
            __ASM_EMIT("add                 $0x20, %[bf]")
            __ASM_EMIT("dec                 %[count]")
            __ASM_EMIT("jnz                 3b")

            __ASM_EMIT("100:")
            : [bc] "+r" (bc), [bf] "+r" (bf), [count] "+r" (count)
            : [ONE] "m" (ONE),
              [kf] "o" (kf),
              [DATA] "o" (DATA)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void bilinear_transform_x2(biquad_x2_t *bf, const f_cascade_t *bc, float kf, size_t count)
    {
        float DATA[4] __lsp_aligned16;

        ARCH_X86_ASM
        (
            // Initialize
            __ASM_EMIT("vbroadcastss        %[kf], %%xmm0")             // xmm0 = kf
            __ASM_EMIT("vmulps              %%xmm0, %%xmm0, %%xmm1")    // xmm1 = kf*kf = kf2

            // Perform x4 blocks
            __ASM_EMIT("sub                 $2, %[count]")
            __ASM_EMIT("jb                  2f")

            // Load bottom part of cascade and transpose
            __ASM_EMIT("1:")
            __ASM_EMIT("vmovaps             0x10(%[bc]), %%xmm2")       // xmm2 = b00 b10 b20 ?
            __ASM_EMIT("vmovaps             0x30(%[bc]), %%xmm3")       // xmm3 = b01 b11 b21 ?
            __ASM_EMIT("vmovaps             0x50(%[bc]), %%xmm4")       // xmm4 = b02 b12 b22 ?
            __ASM_EMIT("vmovaps             0x70(%[bc]), %%xmm5")       // xmm5 = b03 b13 b23 ?

            __ASM_EMIT("vunpckhps           %%xmm5, %%xmm3, %%xmm7")    // xmm7 = b21 b23 ?   ?
            __ASM_EMIT("vunpcklps           %%xmm5, %%xmm3, %%xmm6")    // xmm6 = b01 b03 b11 b13
            __ASM_EMIT("vunpckhps           %%xmm4, %%xmm2, %%xmm5")    // xmm5 = b20 b22 ?   ?
            __ASM_EMIT("vunpcklps           %%xmm4, %%xmm2, %%xmm3")    // xmm3 = b00 b02 b10 b12
            __ASM_EMIT("vunpcklps           %%xmm6, %%xmm3, %%xmm2")    // xmm2 = b00 b01 b02 b03
            __ASM_EMIT("vunpckhps           %%xmm6, %%xmm3, %%xmm3")    // xmm3 = b10 b11 b12 b13
            __ASM_EMIT("vunpcklps           %%xmm7, %%xmm5, %%xmm4")    // xmm4 = b20 b21 b22 b23

            // Compute bottom part
            // xmm2 = b0, xmm3 = b1, xmm4 = b2
            __ASM_EMIT("vmulps              %%xmm0, %%xmm3, %%xmm3")    // xmm3 = B1 = b1*kf
            __ASM_EMIT("vmulps              %%xmm1, %%xmm4, %%xmm4")    // xmm4 = B2 = b2*kf2
            __ASM_EMIT("vaddps              %%xmm2, %%xmm3, %%xmm5")    // xmm5 = B0 + B1
            __ASM_EMIT("vmovaps             %[ONE], %%xmm6")            // xmm6 = 1
            __ASM_EMIT("vaddps              %%xmm4, %%xmm5, %%xmm5")    // xmm5 = B0 + B1 + B2
            __ASM_EMIT("vaddps              %%xmm4, %%xmm2, %%xmm7")    // xmm7 = B2 + B0
            __ASM_EMIT("vdivps              %%xmm5, %%xmm6, %%xmm5")    // xmm5 = N = 1 / (B0 + B1 + B2)
            __ASM_EMIT("vsubps              %%xmm2, %%xmm4, %%xmm6")    // xmm6 = B2 - B0
            __ASM_EMIT("vsubps              %%xmm7, %%xmm3, %%xmm7")    // xmm7 = B1 - (B2 + B0) = B1 - B2 + B0
            __ASM_EMIT("vaddps              %%xmm6, %%xmm6, %%xmm6")    // xmm6 = 2*(B2 - B0)
            __ASM_EMIT("vmulps              %%xmm5, %%xmm7, %%xmm7")    // xmm7 = b2' = (B1 - B2 + B0) * N
            __ASM_EMIT("vmulps              %%xmm5, %%xmm6, %%xmm6")    // xmm6 = b1' = 2 * (B2 - B0) * N
            __ASM_EMIT("vxorps              %%xmm4, %%xmm4, %%xmm4")    // xmm4 = 0
            __ASM_EMIT("vmovaps             %%xmm5, 0x00 + %[DATA]")    // store N
            __ASM_EMIT("vmovlps             %%xmm6, 0x18(%[bf])")
            __ASM_EMIT("vmovlps             %%xmm7, 0x20(%[bf])")
            __ASM_EMIT("vmovlps             %%xmm4, 0x28(%[bf])")
            __ASM_EMIT("vmovhps             %%xmm6, 0x48(%[bf])")
            __ASM_EMIT("vmovhps             %%xmm7, 0x50(%[bf])")
            __ASM_EMIT("vmovhps             %%xmm4, 0x58(%[bf])")

            // Load Top part of cascade and transpose
            __ASM_EMIT("vmovaps             0x00(%[bc]), %%xmm2")       // xmm2 = t0[0] t1[0] t2[0] ?
            __ASM_EMIT("vmovaps             0x20(%[bc]), %%xmm3")       // xmm3 = t0[1] t1[1] t2[1] ?
            __ASM_EMIT("vmovaps             0x40(%[bc]), %%xmm4")       // xmm4 = t0[2] t1[2] t2[2] ?
            __ASM_EMIT("vmovaps             0x60(%[bc]), %%xmm5")       // xmm5 = t0[3] t1[3] t2[3] ?

            __ASM_EMIT("vunpckhps           %%xmm5, %%xmm3, %%xmm7")    // xmm7 = t21 t23 ?   ?
            __ASM_EMIT("vunpcklps           %%xmm5, %%xmm3, %%xmm6")    // xmm6 = t01 t03 t11 t13
            __ASM_EMIT("vunpckhps           %%xmm4, %%xmm2, %%xmm5")    // xmm5 = t20 t22 ?   ?
            __ASM_EMIT("vunpcklps           %%xmm4, %%xmm2, %%xmm3")    // xmm3 = t00 t02 t10 t12
            __ASM_EMIT("vunpcklps           %%xmm6, %%xmm3, %%xmm2")    // xmm2 = t00 t01 t02 t03
            __ASM_EMIT("vunpckhps           %%xmm6, %%xmm3, %%xmm3")    // xmm3 = t10 t11 t12 t13
            __ASM_EMIT("vunpcklps           %%xmm7, %%xmm5, %%xmm4")    // xmm4 = t20 t21 t22 t23

            // x2 = T0 = t0, x3=t1, x4=t2
            __ASM_EMIT("vmovaps             0x00 + %[DATA], %%xmm7")    // load N
            __ASM_EMIT("vmulps              %%xmm1, %%xmm4, %%xmm6")    // xmm6 = T2 = t2*kf2
            __ASM_EMIT("vmulps              %%xmm0, %%xmm3, %%xmm5")    // xmm5 = T1 = t1*kf
            __ASM_EMIT("vaddps              %%xmm6, %%xmm2, %%xmm4")    // xmm4 = T0 + T2
            __ASM_EMIT("vsubps              %%xmm6, %%xmm2, %%xmm3")    // xmm3 = T0 - T2
            __ASM_EMIT("vaddps              %%xmm5, %%xmm4, %%xmm2")    // xmm2 = T0 + T1 + T2
            __ASM_EMIT("vaddps              %%xmm3, %%xmm3, %%xmm3")    // xmm3 = 2*(T0 - T2)
            __ASM_EMIT("vmulps              %%xmm7, %%xmm2, %%xmm2")    // xmm2 = a0 = (T0 + T1 + T2) * N
            __ASM_EMIT("vsubps              %%xmm5, %%xmm4, %%xmm4")    // xmm4 = T0 - T1 + T2
            __ASM_EMIT("vmulps              %%xmm7, %%xmm3, %%xmm3")    // xmm3 = a1 = 2*(T0 - T2) * N
            __ASM_EMIT("vmulps              %%xmm7, %%xmm4, %%xmm4")    // xmm4 = a2 = (T0 - T1 + T2) * N

            // xmm2 = a0
            // xmm3 = a1
            // xmm4 = a2
            __ASM_EMIT("vmovlps             %%xmm2, 0x00(%[bf])")
            __ASM_EMIT("vmovlps             %%xmm3, 0x08(%[bf])")
            __ASM_EMIT("vmovlps             %%xmm4, 0x10(%[bf])")
            __ASM_EMIT("vmovhps             %%xmm2, 0x30(%[bf])")
            __ASM_EMIT("vmovhps             %%xmm3, 0x38(%[bf])")
            __ASM_EMIT("vmovhps             %%xmm4, 0x40(%[bf])")

            // Update pointers and repeat loop
            __ASM_EMIT("add                 $0x80, %[bc]")
            __ASM_EMIT("add                 $0x60, %[bf]")
            __ASM_EMIT("sub                 $2, %[count]")
            __ASM_EMIT("jae                 1b")
            __ASM_EMIT("2:")

            // Single-item loop
            __ASM_EMIT("add                 $1, %[count]")
            __ASM_EMIT("jl                  4f")

            // Calculate bottom part
            __ASM_EMIT("vmovss              0x10(%[bc]), %%xmm2")       // x2 = B0 = b0
            __ASM_EMIT("vmovss              0x14(%[bc]), %%xmm3")       // x3 = b1
            __ASM_EMIT("vmovss              0x18(%[bc]), %%xmm4")       // x4 = b2
            __ASM_EMIT("vinsertps           $0x10, 0x30(%[bc]), %%xmm2, %%xmm2")
            __ASM_EMIT("vinsertps           $0x10, 0x34(%[bc]), %%xmm3, %%xmm3")
            __ASM_EMIT("vinsertps           $0x10, 0x38(%[bc]), %%xmm4, %%xmm4")
            __ASM_EMIT("vmulps              %%xmm0, %%xmm3, %%xmm3")    // xmm3 = B1 = b1*kf
            __ASM_EMIT("vmulps              %%xmm1, %%xmm4, %%xmm4")    // xmm4 = B2 = b2*kf2
            __ASM_EMIT("vaddps              %%xmm2, %%xmm3, %%xmm7")    // xmm7 = B0 + B1
            __ASM_EMIT("vmovaps             %[ONE], %%xmm6")            // xmm6 = 1
            __ASM_EMIT("vaddps              %%xmm4, %%xmm7, %%xmm7")    // xmm7 = B0 + B1 + B2
            __ASM_EMIT("vaddps              %%xmm4, %%xmm2, %%xmm5")    // xmm5 = B2 + B0
            __ASM_EMIT("vdivps              %%xmm7, %%xmm6, %%xmm7")    // xmm7 = N = 1 / (B0 + B1 + B2)
            __ASM_EMIT("vsubps              %%xmm2, %%xmm4, %%xmm6")    // xmm6 = B2 - B0
            __ASM_EMIT("vsubps              %%xmm5, %%xmm3, %%xmm5")    // xmm5 = B1 - (B2 + B0) = B1 - B2 + B0
            __ASM_EMIT("vaddps              %%xmm6, %%xmm6, %%xmm6")    // xmm6 = 2*(B2 - B0)
            __ASM_EMIT("vmulps              %%xmm7, %%xmm5, %%xmm5")    // xmm5 = b2' = (B1 - B2 + B0) * N
            __ASM_EMIT("vmulps              %%xmm7, %%xmm6, %%xmm6")    // xmm6 = b1' = 2 * (B2 - B0) * N
            __ASM_EMIT("vxorps              %%xmm4, %%xmm4, %%xmm4")    // xmm4 = 0
            __ASM_EMIT("vmovlps             %%xmm6, 0x18(%[bf])")
            __ASM_EMIT("vmovlps             %%xmm5, 0x20(%[bf])")
            __ASM_EMIT("vmovlps             %%xmm4, 0x28(%[bf])")

            // Calculate top part
            // xmm7 = N
            __ASM_EMIT("vmovss              0x00(%[bc]), %%xmm2")       // x2 = T0 = t0
            __ASM_EMIT("vmovss              0x04(%[bc]), %%xmm3")       // x3 = t1
            __ASM_EMIT("vmovss              0x08(%[bc]), %%xmm4")       // x4 = t2
            __ASM_EMIT("vinsertps           $0x10, 0x20(%[bc]), %%xmm2, %%xmm2")
            __ASM_EMIT("vinsertps           $0x10, 0x24(%[bc]), %%xmm3, %%xmm3")
            __ASM_EMIT("vinsertps           $0x10, 0x28(%[bc]), %%xmm4, %%xmm4")
            __ASM_EMIT("vmulps              %%xmm1, %%xmm4, %%xmm6")    // xmm6 = T2 = t2*kf2
            __ASM_EMIT("vmulps              %%xmm0, %%xmm3, %%xmm5")    // xmm5 = T1 = t1*kf
            __ASM_EMIT("vaddps              %%xmm6, %%xmm2, %%xmm4")    // xmm4 = T0 + T2
            __ASM_EMIT("vsubps              %%xmm6, %%xmm2, %%xmm3")    // xmm3 = T0 - T2
            __ASM_EMIT("vaddps              %%xmm5, %%xmm4, %%xmm2")    // xmm2 = T0 + T1 + T2
            __ASM_EMIT("vaddps              %%xmm3, %%xmm3, %%xmm3")    // xmm3 = 2*(T0 - T2)
            __ASM_EMIT("vmulps              %%xmm7, %%xmm2, %%xmm2")    // xmm2 = a0 = (T0 + T1 + T2) * N
            __ASM_EMIT("vsubps              %%xmm5, %%xmm4, %%xmm4")    // xmm4 = T0 - T1 + T2
            __ASM_EMIT("vmulps              %%xmm7, %%xmm3, %%xmm3")    // xmm3 = a1 = 2*(T0 - T2) * N
            __ASM_EMIT("vmulps              %%xmm7, %%xmm4, %%xmm4")    // xmm4 = a2 = (T0 - T1 + T2) * N
            __ASM_EMIT("vmovlps             %%xmm2, 0x00(%[bf])")
            __ASM_EMIT("vmovlps             %%xmm3, 0x08(%[bf])")
            __ASM_EMIT("vmovlps             %%xmm4, 0x10(%[bf])")
            __ASM_EMIT("4:")

            : [bc] "+r" (bc), [bf] "+r" (bf), [count] "+r" (count)
            : [ONE] "m" (ONE),
              [kf] "o" (kf),
              [DATA] "o" (DATA)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

#if 0
    static const float bt_one_x4[] __lsp_aligned32 =
    {
        1.0f, 1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f, -1.0f
    };
#endif

    void bilinear_transform_x4(biquad_x4_t *bf, const f_cascade_t *bc, float kf, size_t count)
    {
        float DATA[4] __lsp_aligned16;

        ARCH_X86_ASM
        (
#if 1
            // Initialize
            __ASM_EMIT("vbroadcastss        %[kf], %%xmm0")             // xmm0 = kf
            __ASM_EMIT("vmulps              %%xmm0, %%xmm0, %%xmm1")    // xmm1 = kf*kf = kf2

            // Perform x4 blocks
            __ASM_EMIT("sub                 $1, %[count]")
            __ASM_EMIT("jl                  2f")
            __ASM_EMIT("1:")

            // Load bottom part of cascade and transpose
            __ASM_EMIT("vmovaps             0x10(%[bc]), %%xmm2")       // xmm2 = b00 b10 b20 ?
            __ASM_EMIT("vmovaps             0x30(%[bc]), %%xmm3")       // xmm3 = b01 b11 b21 ?
            __ASM_EMIT("vmovaps             0x50(%[bc]), %%xmm4")       // xmm4 = b02 b12 b22 ?
            __ASM_EMIT("vmovaps             0x70(%[bc]), %%xmm5")       // xmm5 = b03 b13 b23 ?

            __ASM_EMIT("vunpckhps           %%xmm5, %%xmm3, %%xmm7")    // xmm7 = b21 b23 ?   ?
            __ASM_EMIT("vunpcklps           %%xmm5, %%xmm3, %%xmm6")    // xmm6 = b01 b03 b11 b13
            __ASM_EMIT("vunpckhps           %%xmm4, %%xmm2, %%xmm5")    // xmm5 = b20 b22 ?   ?
            __ASM_EMIT("vunpcklps           %%xmm4, %%xmm2, %%xmm3")    // xmm3 = b00 b02 b10 b12
            __ASM_EMIT("vunpcklps           %%xmm6, %%xmm3, %%xmm2")    // xmm2 = b00 b01 b02 b03
            __ASM_EMIT("vunpckhps           %%xmm6, %%xmm3, %%xmm3")    // xmm3 = b10 b11 b12 b13
            __ASM_EMIT("vunpcklps           %%xmm7, %%xmm5, %%xmm4")    // xmm4 = b20 b21 b22 b23

            // Compute bottom part
            // xmm2 = b0, xmm3 = b1, xmm4 = b2
            __ASM_EMIT("vmulps              %%xmm0, %%xmm3, %%xmm3")    // xmm3 = B1 = b1*kf
            __ASM_EMIT("vmulps              %%xmm1, %%xmm4, %%xmm4")    // xmm4 = B2 = b2*kf2
            __ASM_EMIT("vaddps              %%xmm2, %%xmm3, %%xmm5")    // xmm5 = B0 + B1
            __ASM_EMIT("vmovaps             %[ONE], %%xmm6")            // xmm6 = 1
            __ASM_EMIT("vaddps              %%xmm4, %%xmm5, %%xmm5")    // xmm5 = B0 + B1 + B2
            __ASM_EMIT("vaddps              %%xmm4, %%xmm2, %%xmm7")    // xmm7 = B2 + B0
            __ASM_EMIT("vdivps              %%xmm5, %%xmm6, %%xmm5")    // xmm5 = N = 1 / (B0 + B1 + B2)
            __ASM_EMIT("vsubps              %%xmm2, %%xmm4, %%xmm6")    // xmm6 = B2 - B0
            __ASM_EMIT("vsubps              %%xmm7, %%xmm3, %%xmm7")    // xmm7 = B1 - (B2 + B0) = B1 - B2 + B0
            __ASM_EMIT("vaddps              %%xmm6, %%xmm6, %%xmm6")    // xmm6 = 2*(B2 - B0)
            __ASM_EMIT("vmulps              %%xmm5, %%xmm7, %%xmm7")    // xmm7 = b2' = (B1 - B2 + B0) * N
            __ASM_EMIT("vmulps              %%xmm5, %%xmm6, %%xmm6")    // xmm6 = b1' = 2 * (B2 - B0) * N
            __ASM_EMIT("vmovaps             %%xmm5, 0x00 + %[DATA]")    // store N
            __ASM_EMIT("vmovaps             %%xmm6, 0x30(%[bf])")
            __ASM_EMIT("vmovaps             %%xmm7, 0x40(%[bf])")

            // Load Top part of cascade and transpose
            __ASM_EMIT("vmovaps             0x00(%[bc]), %%xmm2")       // xmm2 = t0[0] t1[0] t2[0] ?
            __ASM_EMIT("vmovaps             0x20(%[bc]), %%xmm3")       // xmm3 = t0[1] t1[1] t2[1] ?
            __ASM_EMIT("vmovaps             0x40(%[bc]), %%xmm4")       // xmm4 = t0[2] t1[2] t2[2] ?
            __ASM_EMIT("vmovaps             0x60(%[bc]), %%xmm5")       // xmm5 = t0[3] t1[3] t2[3] ?

            __ASM_EMIT("vunpckhps           %%xmm5, %%xmm3, %%xmm7")    // xmm7 = t21 t23 ?   ?
            __ASM_EMIT("vunpcklps           %%xmm5, %%xmm3, %%xmm6")    // xmm6 = t01 t03 t11 t13
            __ASM_EMIT("vunpckhps           %%xmm4, %%xmm2, %%xmm5")    // xmm5 = t20 t22 ?   ?
            __ASM_EMIT("vunpcklps           %%xmm4, %%xmm2, %%xmm3")    // xmm3 = t00 t02 t10 t12
            __ASM_EMIT("vunpcklps           %%xmm6, %%xmm3, %%xmm2")    // xmm2 = t00 t01 t02 t03
            __ASM_EMIT("vunpckhps           %%xmm6, %%xmm3, %%xmm3")    // xmm3 = t10 t11 t12 t13
            __ASM_EMIT("vunpcklps           %%xmm7, %%xmm5, %%xmm4")    // xmm4 = t20 t21 t22 t23

            // x2 = T0 = t0, x3=t1, x4=t2
            __ASM_EMIT("vmovaps             0x00 + %[DATA], %%xmm7")    // load N
            __ASM_EMIT("vmulps              %%xmm1, %%xmm4, %%xmm6")    // xmm6 = T2 = t2*kf2
            __ASM_EMIT("vmulps              %%xmm0, %%xmm3, %%xmm5")    // xmm5 = T1 = t1*kf
            __ASM_EMIT("vaddps              %%xmm6, %%xmm2, %%xmm4")    // xmm4 = T0 + T2
            __ASM_EMIT("vsubps              %%xmm6, %%xmm2, %%xmm3")    // xmm3 = T0 - T2
            __ASM_EMIT("vaddps              %%xmm5, %%xmm4, %%xmm2")    // xmm2 = T0 + T1 + T2
            __ASM_EMIT("vaddps              %%xmm3, %%xmm3, %%xmm3")    // xmm3 = 2*(T0 - T2)
            __ASM_EMIT("vmulps              %%xmm7, %%xmm2, %%xmm2")    // xmm2 = a0 = (T0 + T1 + T2) * N
            __ASM_EMIT("vsubps              %%xmm5, %%xmm4, %%xmm4")    // xmm4 = T0 - T1 + T2
            __ASM_EMIT("vmulps              %%xmm7, %%xmm3, %%xmm3")    // xmm3 = a1 = 2*(T0 - T2) * N
            __ASM_EMIT("vmulps              %%xmm7, %%xmm4, %%xmm4")    // xmm4 = a2 = (T0 - T1 + T2) * N

            // xmm2 = a0
            // xmm3 = a1
            // xmm4 = a2
            __ASM_EMIT("vmovaps             %%xmm2, 0x00(%[bf])")
            __ASM_EMIT("vmovaps             %%xmm3, 0x10(%[bf])")
            __ASM_EMIT("vmovaps             %%xmm4, 0x20(%[bf])")

            __ASM_EMIT("add                 $0x80, %[bc]")
            __ASM_EMIT("add                 $0x50, %[bf]")
            __ASM_EMIT("sub                 $1, %[count]")
            __ASM_EMIT("jae                 1b")

            // Update pointers and repeat loop
            __ASM_EMIT("2:")

            : [bc] "+r" (bc), [bf] "+r" (bf), [count] "+r" (count)
            : [ONE] "m" (ONE),
              [kf] "o" (kf),
              [DATA] "o" (DATA)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
#else
              // Initialize
              __ASM_EMIT("vbroadcastss        %[kf], %%ymm0")             // ymm0 = kf
              __ASM_EMIT("vmulps              %%ymm0, %%ymm0, %%ymm1")    // ymm1 = kf*kf = kf2

              // Perform x4 blocks
              __ASM_EMIT("sub                 $1, %[count]")
              __ASM_EMIT("jb                  2f")
              __ASM_EMIT("1:")

              // Load bottom part of cascade and transpose
              __ASM_EMIT("vmovups             0x00(%[bc]), %%ymm2")       // ymm2 = t00 t10 t20 ? b00 b10 b20 ?
              __ASM_EMIT("vmovups             0x20(%[bc]), %%ymm3")       // ymm3 = t01 t11 t21 ? b01 b11 b21 ?
              __ASM_EMIT("vmovups             0x40(%[bc]), %%ymm4")       // ymm4 = t02 t12 t22 ? b02 b12 b22 ?
              __ASM_EMIT("vmovups             0x60(%[bc]), %%ymm5")       // ymm5 = t03 t13 t23 ? b03 b13 b23 ?

              __ASM_EMIT("vunpckhps           %%ymm5, %%ymm3, %%ymm7")
              __ASM_EMIT("vunpcklps           %%ymm5, %%ymm3, %%ymm6")
              __ASM_EMIT("vunpckhps           %%ymm4, %%ymm2, %%ymm5")
              __ASM_EMIT("vunpcklps           %%ymm4, %%ymm2, %%ymm3")
              __ASM_EMIT("vunpcklps           %%ymm6, %%ymm3, %%ymm2")
              __ASM_EMIT("vunpckhps           %%ymm6, %%ymm3, %%ymm3")
              __ASM_EMIT("vunpcklps           %%ymm7, %%ymm5, %%ymm4")

              // Compute bottom part
              // ymm2 = t0 b0 = T0 B0
              // ymm3 = t1 b1
              // ymm4 = t2 b2
              __ASM_EMIT("vmovaps             %[FONE], %%ymm7")               // ymm7 = 1 -1
              __ASM_EMIT("vmulps              %%ymm1, %%ymm4, %%ymm6")        // ymm6 = T2 B2 = t2*kf2 b2*kf2
              __ASM_EMIT("vmulps              %%ymm0, %%ymm3, %%ymm5")        // ymm5 = T1 B1 = t1*kf b1*kf
              __ASM_EMIT("vaddps              %%ymm6, %%ymm2, %%ymm4")        // ymm4 = T0+T2 B0+B2
              __ASM_EMIT("vsubps              %%ymm6, %%ymm2, %%ymm3")        // ymm3 = T0-T2 B0-B2
              __ASM_EMIT("vaddps              %%ymm5, %%ymm4, %%ymm2")        // ymm2 = T0+T1+T2 B0+B1+B2
              __ASM_EMIT("vperm2f128          $0x01, %%ymm2, %%ymm2, %%ymm6") // ymm6 = B0+B1+B2 T0+T1+T2
              __ASM_EMIT("vinsertf128         $1, %%xmm6, %%ymm6, %%ymm6")    // ymm6 = B0+B1+B2 B0+B1+B2
              __ASM_EMIT("vdivps              %%ymm6, %%ymm7, %%ymm7")        // ymm7 = 1/(B0+B1+B2) -1/(B0+B1+B2) = N -N
              __ASM_EMIT("vaddps              %%ymm3, %%ymm3, %%ymm3")        // ymm3 = 2*(T0-T2) 2*(B0-B2)
              __ASM_EMIT("vsubps              %%ymm5, %%ymm4, %%ymm4")        // ymm4 = T0-T1+T2 B0-B1+B2
              __ASM_EMIT("vmulps              %%ymm7, %%ymm2, %%ymm2")        // ymm2 = a0' b0' = (T0+T1+T2)*N -(B0+B1+B2)*N
              __ASM_EMIT("vmulps              %%ymm7, %%ymm3, %%ymm3")        // ymm3 = a1' b1' = 2*(T0-T2)*N -2*(B0-B2)*N
              __ASM_EMIT("vmulps              %%ymm7, %%ymm4, %%ymm4")        // ymm4 = a2' b2' = (T0-T1+T2)*N -(B0-B1+B2)*N

              __ASM_EMIT("vmovups             %%xmm2, 0x00(%[bf])")
              __ASM_EMIT("vmovups             %%xmm3, 0x10(%[bf])")
              __ASM_EMIT("vmovups             %%xmm4, 0x20(%[bf])")
              __ASM_EMIT("vextractf128        $1, %%ymm3, 0x30(%[bf])")
              __ASM_EMIT("vextractf128        $1, %%ymm4, 0x40(%[bf])")

              __ASM_EMIT("add                 $0x80, %[bc]")
              __ASM_EMIT("add                 $0x50, %[bf]")
              __ASM_EMIT("sub                 $1, %[count]")
              __ASM_EMIT("jae                 1b")

              // Update pointers and repeat loop
              __ASM_EMIT("2:")

              : [bc] "+r" (bc), [bf] "+r" (bf), [count] "+r" (count)
              : [FONE] "m" (bt_one_x4),
                [kf] "o" (kf)
              : "cc", "memory",
                "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                "%xmm4", "%xmm5", "%xmm6", "%xmm7"
#endif
        );
    }

    void x64_bilinear_transform_x8(biquad_x8_t *bf, const f_cascade_t *bc, float kf, size_t count)
    {
        ARCH_X86_64_ASM(
            __ASM_EMIT("test            %[count], %[count]")
            __ASM_EMIT("jz              100f")

            // Prepare values
            __ASM_EMIT("vbroadcastss    %[kf], %%ymm0")                     // ymm0  = kf
            __ASM_EMIT("vmovaps         %[ONE], %%ymm15")                   // ymm15 = 1
            __ASM_EMIT("vmulps          %%ymm0, %%ymm0, %%ymm1")            // ymm1  = kf*kf = kf2

            // Do a loop
            __ASM_EMIT("1:")
            __ASM_EMIT("vmovaps         0x00(%[bc]), %%ymm2")               // ymm2  = t0[0] t1[0] t2[0]   ?   b0[0] b1[0] b2[0]   ?
            __ASM_EMIT("vmovaps         0x20(%[bc]), %%ymm3")               // ymm3  = t0[1] t1[1] t2[1]   ?   b0[1] b1[1] b2[1]   ?
            __ASM_EMIT("vmovaps         0x40(%[bc]), %%ymm4")               // ymm4  = t0[2] t1[2] t2[2]   ?   b0[2] b1[2] b2[2]   ?
            __ASM_EMIT("vmovaps         0x60(%[bc]), %%ymm5")               // ymm5  = t0[3] t1[3] t2[3]   ?   b0[3] b1[3] b2[3]   ?
            __ASM_EMIT("vmovaps         0x80(%[bc]), %%ymm6")               // ymm6  = t0[4] t1[4] t2[4]   ?   b0[4] b1[4] b2[4]   ?
            __ASM_EMIT("vmovaps         0xa0(%[bc]), %%ymm7")               // ymm7  = t0[5] t1[5] t2[5]   ?   b0[5] b1[5] b2[5]   ?
            __ASM_EMIT("vmovaps         0xc0(%[bc]), %%ymm8")               // ymm8  = t0[6] t1[6] t2[6]   ?   b0[6] b1[6] b2[6]   ?
            __ASM_EMIT("vmovaps         0xe0(%[bc]), %%ymm9")               // ymm9  = t0[7] t1[7] t2[7]   ?   b0[7] b1[7] b2[7]   ?

            // Transpose, step 1
            __ASM_EMIT("vunpcklps       %%ymm3, %%ymm2, %%ymm10")           // ymm10 = t0[0] t0[1] t1[0] t1[1] b0[0] b0[1] b1[0] b1[1]
            __ASM_EMIT("vunpcklps       %%ymm5, %%ymm4, %%ymm11")           // ymm11 = t0[2] t0[3] t1[2] t1[3] b0[2] b0[3] b1[2] b1[3]
            __ASM_EMIT("vunpcklps       %%ymm7, %%ymm6, %%ymm12")           // ymm12 = t0[4] t0[5] t1[4] t1[5] b0[4] b0[5] b1[4] b1[5]
            __ASM_EMIT("vunpcklps       %%ymm9, %%ymm8, %%ymm13")           // ymm13 = t0[6] t0[7] t1[6] t1[7] b0[6] b0[7] b1[6] b1[7]
            __ASM_EMIT("vunpckhps       %%ymm3, %%ymm2, %%ymm2")            // ymm2  = t2[0] t2[1]   ?     ?   b2[0] b2[1]   ?     ?
            __ASM_EMIT("vunpckhps       %%ymm5, %%ymm4, %%ymm4")            // ymm4  = t2[2] t2[3]   ?     ?   b2[2] b2[3]   ?     ?
            __ASM_EMIT("vunpckhps       %%ymm7, %%ymm6, %%ymm6")            // ymm6  = t2[4] t2[5]   ?     ?   b2[4] b2[5]   ?     ?
            __ASM_EMIT("vunpckhps       %%ymm9, %%ymm8, %%ymm8")            // ymm8  = t2[6] t2[7]   ?     ?   b2[6] b2[7]   ?     ?

            // Transpose, step 2
            __ASM_EMIT("vshufps         $0x44, %%ymm11, %%ymm10, %%ymm3")   // ymm3  = t0[0] t0[1] t0[2] t0[3] b0[0] b0[1] b0[2] b0[3]
            __ASM_EMIT("vshufps         $0x44, %%ymm13, %%ymm12, %%ymm5")   // ymm5  = t0[4] t0[5] t0[6] t0[7] b0[4] b0[5] b0[6] b0[7]
            __ASM_EMIT("vshufps         $0x44, %%ymm4,  %%ymm2,  %%ymm2")   // ymm2  = t2[0] t2[1] t2[2] t2[3] b2[0] b2[1] b2[2] b2[3]
            __ASM_EMIT("vshufps         $0x44, %%ymm8,  %%ymm6,  %%ymm6")   // ymm6  = t2[4] t2[5] t2[6] t2[7] b2[4] b2[5] b2[6] b2[7]
            __ASM_EMIT("vshufps         $0xee, %%ymm11, %%ymm10, %%ymm4")   // ymm4  = t1[0] t1[1] t1[2] t1[3] b1[0] b1[1] b1[2] b1[3]
            __ASM_EMIT("vshufps         $0xee, %%ymm13, %%ymm12, %%ymm7")   // ymm7  = t1[4] t1[5] t1[6] t1[7] b1[4] b1[5] b1[6] b1[7]

            // Transpose, step 3
            __ASM_EMIT("vextractf128    $1, %%ymm3,  %%xmm8")               // xmm8  = b0[0] b0[1] b0[2] b0[3]   0     0     0     0
            __ASM_EMIT("vextractf128    $1, %%ymm2,  %%xmm9")               // xmm9  = b2[0] b2[1] b2[2] b2[3]   0     0     0     0
            __ASM_EMIT("vextractf128    $1, %%ymm4,  %%xmm10")              // xmm10 = b1[0] b1[1] b1[2] b1[3]   0     0     0     0
            __ASM_EMIT("vinsertf128     $1, %%xmm5,  %%ymm3, %%ymm3")       // ymm3  = t0[0] t0[1] t0[2] t0[3] t0[4] t0[5] t0[6] t0[7]
            __ASM_EMIT("vinsertf128     $1, %%xmm6,  %%ymm2, %%ymm2")       // ymm2  = t2[0] t2[1] t2[2] t2[3] t2[4] t2[5] t2[6] t2[7]
            __ASM_EMIT("vinsertf128     $1, %%xmm7,  %%ymm4, %%ymm4")       // ymm4  = t1[0] t1[1] t1[2] t1[3] t1[4] t1[5] t1[6] t1[7]
            __ASM_EMIT("vinsertf128     $0, %%xmm8,  %%ymm5, %%ymm5")       // ymm5  = b0[0] b0[1] b0[2] b0[3] b0[4] b0[5] b0[6] b0[7]
            __ASM_EMIT("vinsertf128     $0, %%xmm9,  %%ymm6, %%ymm6")       // ymm6  = b2[0] b2[1] b2[2] b2[3] b2[4] b2[5] b2[6] b2[7]
            __ASM_EMIT("vinsertf128     $0, %%xmm10, %%ymm7, %%ymm7")       // ymm7  = b1[0] b1[1] b1[2] b1[3] b1[4] b1[5] b1[6] b1[7]

            // Now we are ready to perform calculations of top and bottom part
            // ymm3 = T0
            // ymm5 = B0
            __ASM_EMIT("vmulps          %%ymm0, %%ymm4, %%ymm4")            // ymm4  = T1 = t1 * kf
            __ASM_EMIT("vmulps          %%ymm1, %%ymm2, %%ymm2")            // ymm2  = T2 = t2 * kf2
            __ASM_EMIT("vmulps          %%ymm0, %%ymm7, %%ymm7")            // ymm7  = B1 = b1 * kf
            __ASM_EMIT("vmulps          %%ymm1, %%ymm6, %%ymm6")            // ymm6  = B2 = b2 * kf2

            __ASM_EMIT("vaddps          %%ymm2, %%ymm3, %%ymm10")           // ymm10 = T0 + T2
            __ASM_EMIT("vsubps          %%ymm5, %%ymm6, %%ymm8")            // ymm8  = B2 - B0
            __ASM_EMIT("vsubps          %%ymm2, %%ymm3, %%ymm3")            // ymm3  = T0 - T2
            __ASM_EMIT("vaddps          %%ymm5, %%ymm6, %%ymm5")            // ymm5  = B2 + B0
            __ASM_EMIT("vaddps          %%ymm4, %%ymm10, %%ymm11")          // ymm11 = T0 + T1 + T2
            __ASM_EMIT("vaddps          %%ymm8, %%ymm8, %%ymm8")            // ymm8  = 2 * (B2 - B0)
            __ASM_EMIT("vaddps          %%ymm3, %%ymm3, %%ymm3")            // ymm3  = 2 * (T0 - T2)
            __ASM_EMIT("vaddps          %%ymm5, %%ymm7, %%ymm9")            // ymm9  = B0 + B1 + B2
            __ASM_EMIT("vsubps          %%ymm4, %%ymm10, %%ymm4")           // ymm4  = T0 - T1 + T2
            __ASM_EMIT("vdivps          %%ymm9, %%ymm15, %%ymm9")           // ymm9  = N = 1 / (B0 + B1 + B2)
            __ASM_EMIT("vsubps          %%ymm5, %%ymm7, %%ymm5")            // ymm5  = B1 - B2 - B0

            __ASM_EMIT("vmulps          %%ymm9, %%ymm11, %%ymm11")          // ymm11 = A0 = (T0 + T1 + T2) * N
            __ASM_EMIT("vmulps          %%ymm9, %%ymm3, %%ymm3")            // ymm3  = A1 = 2 * (T0 - T2) * N
            __ASM_EMIT("vmulps          %%ymm9, %%ymm4, %%ymm4")            // ymm4  = A2 = (T0 - T1 + T2) * N
            __ASM_EMIT("vmulps          %%ymm9, %%ymm8, %%ymm8")            // ymm8  = B0 = 2 * (B2 - B0) * N
            __ASM_EMIT("vmulps          %%ymm9, %%ymm5, %%ymm5")            // ymm5  = B1 = (B1 - B2 - B0) * N

            // Store values
            __ASM_EMIT("vmovaps         %%ymm11, 0x00(%[bf])")
            __ASM_EMIT("vmovaps         %%ymm3,  0x20(%[bf])")
            __ASM_EMIT("vmovaps         %%ymm4,  0x40(%[bf])")
            __ASM_EMIT("vmovaps         %%ymm8,  0x60(%[bf])")
            __ASM_EMIT("vmovaps         %%ymm5,  0x80(%[bf])")

            // Repeat cycle
            __ASM_EMIT("add             $0x100, %[bc]")
            __ASM_EMIT("add             $0xa0,  %[bf]")
            __ASM_EMIT("dec             %[count]")
            __ASM_EMIT("jnz             1b")

            __ASM_EMIT("100:")
            : [count] "+r" (count), [bc] "+r" (bc), [bf] "+r" (bf)
            : [ONE] "m" (ONE), [kf] "m" (kf)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5", "%xmm6", "%xmm7",
              "%xmm8", "%xmm9", "%xmm10", "%xmm11", "%xmm12", "%xmm13", "%xmm14", "%xmm15"
        );
    //    while (count--)
    //    {
    //        // Calculate bottom coefficients
    //        B[0]            = bc->b[0];
    //        B[1]            = bc->b[1]*kf;
    //        B[2]            = bc->b[2]*kf2;
    //
    //        // Calculate the convolution
    //        N               = 1.0 / (B[0] + B[1] + B[2]);
    //
    //        bf->b[0]        = 2.0 * (B[2] - B[0]) * N;  // Sign negated
    //        bf->b[1]        = (B[1] - B[2] - B[0]) * N; // Sign negated
    //        bf->b[2]        = 0.0f;
    //        bf->b[3]        = 0.0f;
    //
    //        // Calculate top coefficients
    //        T[0]            = bc->t[0];
    //        T[1]            = bc->t[1]*kf;
    //        T[2]            = bc->t[2]*kf2;
    //
    //        bf->a[0]        = (T[0] + T[1] + T[2]) * N;
    //        bf->a[1]        = 2.0 * (T[0] - T[2]) * N;
    //        bf->a[2]        = (T[0] - T[1] + T[2]) * N;
    //
    //        // Increment pointers
    //        bc              ++;
    //        bf              ++;
    }
}

#endif /* DSP_ARCH_X86_AVX_FILTERS_TRANSFORM_H_ */
