/*
 * resampling.h
 *
 *  Created on: 15 янв. 2020 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_AVX_RESAMPLING_H_
#define DSP_ARCH_X86_AVX_RESAMPLING_H_

#ifndef DSP_ARCH_X86_AVX_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_AVX_IMPL */

namespace avx
{
    // Lanczos kernel 2x2: 1 AVX register
    static const float lanczos_2x2[] __lsp_aligned16 =
    {
        +0.6203830132406946f,       // k0
        -0.1664152316035080f        // k1
    };

    void lanczos_resample_2x2(float *dst, const float *src, size_t count)
    {
        ARCH_X86_ASM (
            __ASM_EMIT("vmovss          0x00 + %[k], %%xmm6")           // xmm6 = k0
            __ASM_EMIT("vmovss          0x04 + %[k], %%xmm7")           // xmm7 = k1
            // 2x blocks
            __ASM_EMIT("sub             $2, %[count]")
            __ASM_EMIT("jb              2f")
            __ASM_EMIT(".align          16")
            __ASM_EMIT("1:")
            __ASM_EMIT("vmovss          0x00(%[src]), %%xmm0")          // xmm0 = s0
            __ASM_EMIT("vmovss          0x04(%[src]), %%xmm1")          // xmm1 = s1
            __ASM_EMIT("vmulss          %%xmm7, %%xmm0, %%xmm2")        // xmm2 = k1*s0
            __ASM_EMIT("vmulss          %%xmm7, %%xmm1, %%xmm3")        // xmm3 = k1*s1
            __ASM_EMIT("vaddss          0x04(%[dst]), %%xmm2, %%xmm4")  // xmm4 = k1*s0 + d1
            __ASM_EMIT("vaddss          0x24(%[dst]), %%xmm3, %%xmm5")  // xmm5 = k1*s1 + d9
            __ASM_EMIT("vmovss          %%xmm4, 0x04(%[dst])")          // d1  += k1*s0
            __ASM_EMIT("vmovss          %%xmm5, 0x24(%[dst])")          // d9  += k1*s1
            __ASM_EMIT("vmulss          %%xmm6, %%xmm0, %%xmm4")        // xmm4 = k0*s0
            __ASM_EMIT("vmulss          %%xmm6, %%xmm1, %%xmm5")        // xmm5 = k0*s1
            __ASM_EMIT("vaddss          %%xmm4, %%xmm3, %%xmm3")        // xmm3 = k1*s1 + k0*s0
            __ASM_EMIT("vaddss          %%xmm5, %%xmm2, %%xmm2")        // xmm2 = k1*s0 + k0*s1
            __ASM_EMIT("vaddss          0x0c(%[dst]), %%xmm3, %%xmm3")  // xmm3 = k1*s1 + k0*s0 + d3
            __ASM_EMIT("vaddss          0x1c(%[dst]), %%xmm2, %%xmm2")  // xmm2 = k1*s0 + k0*s1 + d7
            __ASM_EMIT("vmovss          %%xmm3, 0x0c(%[dst])")          // d3  += k1*s1 + k0*s0
            __ASM_EMIT("vmovss          %%xmm2, 0x1c(%[dst])")          // d7  += k1*s0 + k0*s1
            __ASM_EMIT("vaddss          %%xmm5, %%xmm4, %%xmm4")        // xmm4 = k0*s0 + k0*s1
            __ASM_EMIT("vaddss          0x10(%[dst]), %%xmm0, %%xmm0")  // xmm0 = s0 + d4
            __ASM_EMIT("vaddss          0x18(%[dst]), %%xmm1, %%xmm1")  // xmm1 = s1 + d6
            __ASM_EMIT("vaddss          0x14(%[dst]), %%xmm4, %%xmm4")  // xmm4 = k0*s0 + k0*s1 + d5
            __ASM_EMIT("vmovss          %%xmm0, 0x10(%[dst])")          // d4  += s0
            __ASM_EMIT("vmovss          %%xmm4, 0x14(%[dst])")          // d5  += k0*s0 + k0*s1
            __ASM_EMIT("vmovss          %%xmm1, 0x18(%[dst])")          // d6  += s1
            __ASM_EMIT("add             $0x08, %[src]")
            __ASM_EMIT("add             $0x10, %[dst]")
            __ASM_EMIT("sub             $2, %[count]")
            __ASM_EMIT("jae             1b")
            __ASM_EMIT("2:")
            // 1x block
            __ASM_EMIT("add             $1, %[count]")
            __ASM_EMIT("jl              3f")
            __ASM_EMIT("vmovss          0x00(%[src]), %%xmm2")          // xmm2 = s0
            __ASM_EMIT("vmulss          %%xmm7, %%xmm2, %%xmm7")        // xmm7 = k1*s0
            __ASM_EMIT("vmulss          %%xmm6, %%xmm2, %%xmm6")        // xmm6 = k0*s0
            __ASM_EMIT("vaddss          0x04(%[dst]), %%xmm7, %%xmm0")  // xmm0 = d1 + k1*s0
            __ASM_EMIT("vaddss          0x0c(%[dst]), %%xmm6, %%xmm1")  // xmm1 = d3 + k0*s0
            __ASM_EMIT("vaddss          0x10(%[dst]), %%xmm2, %%xmm2")  // xmm2 = d4 + s0
            __ASM_EMIT("vaddss          0x14(%[dst]), %%xmm6, %%xmm3")  // xmm3 = d5 + k0*s0
            __ASM_EMIT("vaddss          0x1c(%[dst]), %%xmm7, %%xmm4")  // xmm4 = d7 + k1*s0
            __ASM_EMIT("vmovss          %%xmm0, 0x04(%[dst])")          // d1  += k1*s0
            __ASM_EMIT("vmovss          %%xmm1, 0x0c(%[dst])")          // d3  += k0*s0
            __ASM_EMIT("vmovss          %%xmm2, 0x10(%[dst])")          // d4  += s0
            __ASM_EMIT("vmovss          %%xmm3, 0x14(%[dst])")          // d5  += k0*s0
            __ASM_EMIT("vmovss          %%xmm4, 0x1c(%[dst])")          // d7  += k1*s0
            __ASM_EMIT("3:")

            : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
            : [k] "o" (lanczos_2x2)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    static const float lanczos_2x3[] __lsp_aligned16 =
    {
        +0.0939539981090991f,       // k2
        +0.6293724479752082f,       // k0
        -0.1910530560835854f        // k1
    };

    void lanczos_resample_2x3(float *dst, const float *src, size_t count)
    {
        ARCH_X86_ASM (
            // 2x blocks
            __ASM_EMIT("vmovss          0x04 + %[k], %%xmm6")           // xmm6 = k0
            __ASM_EMIT("vmovss          0x08 + %[k], %%xmm7")           // xmm7 = k1
            __ASM_EMIT("sub             $2, %[count]")
            __ASM_EMIT("jb              2f")
            __ASM_EMIT(".align          16")
            __ASM_EMIT("1:")
            __ASM_EMIT("vmovss          0x00(%[src]), %%xmm0")          // xmm0 = s0
            __ASM_EMIT("vmovss          0x04(%[src]), %%xmm1")          // xmm1 = s1

            __ASM_EMIT("vmulss          %[k], %%xmm0, %%xmm2")          // xmm2 = k2*s0
            __ASM_EMIT("vmulss          %[k], %%xmm1, %%xmm3")          // xmm3 = k2*s1
            __ASM_EMIT("vaddss          0x04(%[dst]), %%xmm2, %%xmm4")  // xmm4 = k2*s0 + d1
            __ASM_EMIT("vaddss          0x34(%[dst]), %%xmm3, %%xmm5")  // xmm5 = k2*s1 + d13
            __ASM_EMIT("vmovss          %%xmm4, 0x04(%[dst])")          // d1  += k2*s0
            __ASM_EMIT("vmovss          %%xmm5, 0x34(%[dst])")          // d13 += k2*s1

            __ASM_EMIT("vmulss          %%xmm7, %%xmm0, %%xmm4")        // xmm4 = k1*s0
            __ASM_EMIT("vmulss          %%xmm7, %%xmm1, %%xmm5")        // xmm5 = k1*s1
            __ASM_EMIT("vaddss          %%xmm4, %%xmm3, %%xmm3")        // xmm3 = k2*s1 + k1*s0
            __ASM_EMIT("vaddss          %%xmm5, %%xmm2, %%xmm2")        // xmm2 = k2*s0 + k1*s1
            __ASM_EMIT("vaddss          0x0c(%[dst]), %%xmm3, %%xmm3")  // xmm3 = k2*s1 + k1*s0 + d3
            __ASM_EMIT("vaddss          0x2c(%[dst]), %%xmm2, %%xmm2")  // xmm2 = k2*s0 + k1*s1 + d11
            __ASM_EMIT("vmovss          %%xmm3, 0x0c(%[dst])")          // d3  += k2*s1 + k1*s0
            __ASM_EMIT("vmovss          %%xmm2, 0x2c(%[dst])")          // d11 += k2*s0 + k1*s1

            __ASM_EMIT("vmulss          %%xmm6, %%xmm0, %%xmm2")        // xmm2 = k0*s0
            __ASM_EMIT("vmulss          %%xmm6, %%xmm1, %%xmm3")        // xmm3 = k0*s1
            __ASM_EMIT("vaddss          %%xmm2, %%xmm5, %%xmm5")        // xmm5 = k1*s1 + k0*s0
            __ASM_EMIT("vaddss          %%xmm3, %%xmm4, %%xmm4")        // xmm4 = k1*s0 + k0*s1
            __ASM_EMIT("vaddss          0x14(%[dst]), %%xmm5, %%xmm5")  // xmm5 = k1*s1 + k0*s0 + d5
            __ASM_EMIT("vaddss          0x24(%[dst]), %%xmm4, %%xmm4")  // xmm4 = k1*s0 + k0*s1 + d9
            __ASM_EMIT("vmovss          %%xmm5, 0x14(%[dst])")          // d5  += k1*s1 + k0*s0
            __ASM_EMIT("vmovss          %%xmm4, 0x24(%[dst])")          // d9  += k1*s0 + k0*s1

            __ASM_EMIT("vaddss          %%xmm3, %%xmm2, %%xmm2")        // xmm2 = k0*s0 + k0*s1
            __ASM_EMIT("vaddss          0x18(%[dst]), %%xmm0, %%xmm0")  // xmm0 = s0 + d6
            __ASM_EMIT("vaddss          0x20(%[dst]), %%xmm1, %%xmm1")  // xmm1 = s1 + d8
            __ASM_EMIT("vaddss          0x1c(%[dst]), %%xmm2, %%xmm2")  // xmm2 = k0*s0 + k0*s1 + d7
            __ASM_EMIT("vmovss          %%xmm0, 0x18(%[dst])")          // d6  += s0
            __ASM_EMIT("vmovss          %%xmm2, 0x1c(%[dst])")          // d7  += k0*s0 + k0*s1
            __ASM_EMIT("vmovss          %%xmm1, 0x20(%[dst])")          // d8  += s1
            __ASM_EMIT("add             $0x08, %[src]")
            __ASM_EMIT("add             $0x10, %[dst]")
            __ASM_EMIT("sub             $2, %[count]")
            __ASM_EMIT("jae             1b")
            __ASM_EMIT("2:")
            // 1x block
            __ASM_EMIT("add             $1, %[count]")
            __ASM_EMIT("jl              3f")
            __ASM_EMIT("vmovss          0x00(%[src]), %%xmm3")          // xmm2 = s0
            __ASM_EMIT("vmulss          0x00 + %[k], %%xmm3, %%xmm6")   // xmm6 = k2*s0
            __ASM_EMIT("vmulss          0x04 + %[k], %%xmm3, %%xmm4")   // xmm4 = k0*s0
            __ASM_EMIT("vmulss          0x08 + %[k], %%xmm3, %%xmm5")   // xmm5 = k1*s0
            __ASM_EMIT("vaddss          0x04(%[dst]), %%xmm6, %%xmm0")  // xmm0 = d1 + k2*s0
            __ASM_EMIT("vaddss          0x0c(%[dst]), %%xmm5, %%xmm1")  // xmm1 = d3 + k1*s0
            __ASM_EMIT("vaddss          0x14(%[dst]), %%xmm4, %%xmm2")  // xmm2 = d5 + k0*s0
            __ASM_EMIT("vaddss          0x18(%[dst]), %%xmm3, %%xmm3")  // xmm3 = d6 + s0
            __ASM_EMIT("vaddss          0x1c(%[dst]), %%xmm4, %%xmm4")  // xmm4 = d7 + k0*s0
            __ASM_EMIT("vaddss          0x24(%[dst]), %%xmm5, %%xmm5")  // xmm5 = d9 + k1*s0
            __ASM_EMIT("vaddss          0x2c(%[dst]), %%xmm6, %%xmm6")  // xmm6 = d11 + k2*s0
            __ASM_EMIT("vmovss          %%xmm0, 0x04(%[dst])")
            __ASM_EMIT("vmovss          %%xmm1, 0x0c(%[dst])")
            __ASM_EMIT("vmovss          %%xmm2, 0x14(%[dst])")
            __ASM_EMIT("vmovss          %%xmm3, 0x18(%[dst])")
            __ASM_EMIT("vmovss          %%xmm4, 0x1c(%[dst])")
            __ASM_EMIT("vmovss          %%xmm5, 0x24(%[dst])")
            __ASM_EMIT("vmovss          %%xmm6, 0x2c(%[dst])")
            __ASM_EMIT("3:")

            : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
            : [k] "o" (lanczos_2x3)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    // Lanczos kernel 3x2: 2 SSE registers, 2 AVX registers
    IF_ARCH_I386(
        static const float lanczos_3x2[] __lsp_aligned16 =
        {
            +0.0000000000000000f,
            -0.1451906347823569f,
            -0.1903584501504231f,
            +0.0000000000000000f,

            +0.4051504629060886f,
            +0.8228011237053413f,
            +1.0000000000000000f,
            +0.8228011237053413f,

            +0.4051504629060886f,
            +0.0000000000000000f,
            -0.1903584501504231f,
            -0.1451906347823569f,

            // Shifted by 1 left
            -0.1451906347823569f,
            -0.1903584501504231f,
            +0.0000000000000000f,
            +0.4051504629060886f,

            +0.8228011237053413f,
            +1.0000000000000000f,
            +0.8228011237053413f,
            +0.4051504629060886f,

            +0.0000000000000000f,
            -0.1903584501504231f,
            -0.1451906347823569f,
            +0.0000000000000000f
        };
    )

    IF_ARCH_X86_64(
        static const float lanczos_3x2[] __lsp_aligned16 =
        {
            +0.8228011237053413f, // k0
            +0.4051504629060886f, // k1
            -0.1903584501504231f, // k2
            -0.1451906347823569f  // k3
        };
    )

    void lanczos_resample_3x2(float *dst, const float *src, size_t count)
    {
        ARCH_I386_ASM (
            __ASM_EMIT("vmovaps         0x00 + %[k], %%xmm4")           // ymm4 = 0  k3 k2 0  0  0  0  0
            __ASM_EMIT("vmovups         0x10 + %[k], %%ymm5")           // ymm5 = k1 k0 1  k0 k1 0  k2 k3
            __ASM_EMIT("vmovups         0x30 + %[k], %%ymm6")           // ymm6 = k3 k2 0  k1 k0 1  k0 k1
            __ASM_EMIT("vmovaps         0x50 + %[k], %%xmm7")           // ymm7 = 0  k2 k3 0  0  0  0  0
            // 2x blocks
            __ASM_EMIT("sub             $2, %[count]")
            __ASM_EMIT("jb              2f")
            __ASM_EMIT(".align          16")
            __ASM_EMIT("1:")
            __ASM_EMIT("vbroadcastss    0x00(%[src]), %%ymm0")          // ymm0 = s0
            __ASM_EMIT("vbroadcastss    0x04(%[src]), %%ymm3")          // ymm3 = s1
            __ASM_EMIT("vmulps          %%ymm5, %%ymm0, %%ymm1")        // ymm1 = k1*s0 k0*s0 1*s0  k0*s0 k1*s0 0*s0  k2*s0 k3*s0
            __ASM_EMIT("vmulps          %%ymm6, %%ymm3, %%ymm2")        // ymm2 = k3*s1 k2*s1 0*s1  k1*s1 k0*s1 1*s1  k0*s1 k1*s1
            __ASM_EMIT("vmulps          %%xmm4, %%xmm0, %%xmm0")        // xmm0 = 0*s0  k3*s0 k2*s0 0*s0
            __ASM_EMIT("vmulps          %%xmm7, %%xmm3, %%xmm3")        // xmm3 = 0*s1  k2*s1 k3*s1 0*s1
            __ASM_EMIT("vaddps          0x00(%[dst]), %%xmm0, %%xmm0")  // xmm0 = d0 d1+k3*s0 d2+k2*s0 d3
            __ASM_EMIT("vaddps          0x10(%[dst]), %%ymm1, %%ymm1")  // ymm1 = d4+k1*s0 d5+k0*s0 d6+s0 d7+k0*s0 d8+k1*s0 d9 d10+k2*s0 d11+k3*s0
            __ASM_EMIT("vaddps          0x30(%[dst]), %%xmm3, %%xmm3")  // xmm3 = d12 d13+k2*s1 d14+k3*s1 d15
            __ASM_EMIT("vaddps          %%ymm2, %%ymm1, %%ymm1")        // ymm1 = d4+k1*s0+k3*s1 d5+k0*s0+k2*s1 d6+s0 d7+k0*s0+k1*s1 d8+k1*s0+k0*s1 d9+s1 d10+k2*s0+k0*s1 d11+k3*s0+k1*s1
            __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("vmovups         %%ymm1, 0x10(%[dst])")
            __ASM_EMIT("vmovups         %%xmm3, 0x30(%[dst])")
            __ASM_EMIT("add             $0x08, %[src]")
            __ASM_EMIT("add             $0x18, %[dst]")
            __ASM_EMIT("sub             $2, %[count]")
            __ASM_EMIT("jae             1b")
            __ASM_EMIT("2:")
            // 1x block
            __ASM_EMIT("add             $1, %[count]")
            __ASM_EMIT("jl              3f")
            __ASM_EMIT("vbroadcastss    0x00(%[src]), %%ymm0")          // ymm0 = s0
            __ASM_EMIT("vmulps          %%ymm5, %%ymm0, %%ymm1")        // ymm1 = k1*s0 k0*s0 1*s0  k0*s0 k1*s0 0*s0  k2*s0 k3*s0
            __ASM_EMIT("vmulps          %%xmm4, %%xmm0, %%xmm0")        // xmm0 = 0*s0  k3*s0 k2*s0 0*s0
            __ASM_EMIT("vaddps          0x00(%[dst]), %%xmm0, %%xmm0")  // xmm0 = d0 d1+k3*s0 d2+k2*s0 d3
            __ASM_EMIT("vaddps          0x10(%[dst]), %%ymm1, %%ymm1")  // ymm1 = d4+k1*s0 d5+k0*s0 d6+s0 d7+k0*s0 d8+k1*s0 d9 d10+k2*s0 d11+k3*s0
            __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("vmovups         %%ymm1, 0x10(%[dst])")
            __ASM_EMIT("3:")

            : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
            : [k] "o" (lanczos_3x2)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );

        ARCH_X86_64_ASM (
            __ASM_EMIT("vmovss          0x00 + %[k], %%xmm12")          // xmm12 = k0
            __ASM_EMIT("vmovss          0x04 + %[k], %%xmm13")          // xmm13 = k1
            __ASM_EMIT("vmovss          0x08 + %[k], %%xmm14")          // xmm14 = k2
            __ASM_EMIT("vmovss          0x0c + %[k], %%xmm15")          // xmm15 = k3
            // 2x blocks
            __ASM_EMIT("sub             $2, %[count]")
            __ASM_EMIT("jb              2f")
            __ASM_EMIT(".align          16")
            __ASM_EMIT("1:")
            __ASM_EMIT("vmovss          0x00(%[src]), %%xmm0")          // xmm0 = s0
            __ASM_EMIT("vmovss          0x04(%[src]), %%xmm1")          // xmm1 = s1
            __ASM_EMIT("vmulss          %%xmm14, %%xmm0, %%xmm4")       // xmm4 = k2*s0
            __ASM_EMIT("vmulss          %%xmm12, %%xmm1, %%xmm6")       // xmm6 = k0*s1
            __ASM_EMIT("vmulss          %%xmm15, %%xmm0, %%xmm5")       // xmm5 = k3*s0
            __ASM_EMIT("vmulss          %%xmm13, %%xmm1, %%xmm7")       // xmm7 = k1*s1
            __ASM_EMIT("vaddss          0x04(%[dst]), %%xmm5, %%xmm10") // xmm10= d1 + k3*s0
            __ASM_EMIT("vaddss          0x08(%[dst]), %%xmm4, %%xmm11") // xmm11= d2 + k2*s0
            __ASM_EMIT("vmulss          %%xmm12, %%xmm0, %%xmm2")       // xmm2 = k0*s0
            __ASM_EMIT("vmulss          %%xmm14, %%xmm1, %%xmm8")       // xmm8 = k2*s1
            __ASM_EMIT("vmovss          %%xmm10, 0x04(%[dst])")
            __ASM_EMIT("vmovss          %%xmm11, 0x08(%[dst])")
            __ASM_EMIT("vmulss          %%xmm13, %%xmm0, %%xmm3")       // xmm3 = k1*s0
            __ASM_EMIT("vmulss          %%xmm15, %%xmm1, %%xmm9")       // xmm9 = k3*s1
            __ASM_EMIT("vaddss          0x10(%[dst]), %%xmm3, %%xmm10") // xmm10= d4 + k1*s0
            __ASM_EMIT("vaddss          0x14(%[dst]), %%xmm2, %%xmm11") // xmm11= d5 + k0*s0
            __ASM_EMIT("vaddss          0x18(%[dst]), %%xmm0, %%xmm0")  // xmm0 = d6 + s0
            __ASM_EMIT("vaddss          %%xmm9, %%xmm10, %%xmm10")      // xmm10= d4 + k1*s0 + k3*s1
            __ASM_EMIT("vaddss          %%xmm8, %%xmm11, %%xmm11")      // xmm11= d5 + k0*s0 + k2*s1
            __ASM_EMIT("vmovss          %%xmm10, 0x10(%[dst])")
            __ASM_EMIT("vmovss          %%xmm11, 0x14(%[dst])")
            __ASM_EMIT("vmovss          %%xmm0, 0x18(%[dst])")
            __ASM_EMIT("vaddss          0x1c(%[dst]), %%xmm2, %%xmm2")  // xmm2 = d7 + k0*s0
            __ASM_EMIT("vaddss          0x20(%[dst]), %%xmm3, %%xmm3")  // xmm3 = d8 + k1*s0
            __ASM_EMIT("vaddss          0x24(%[dst]), %%xmm1, %%xmm1")  // xmm1 = d9 + s1
            __ASM_EMIT("vaddss          %%xmm7, %%xmm2, %%xmm2")        // xmm2 = d7 + k0*s0 + k1*s1
            __ASM_EMIT("vaddss          %%xmm6, %%xmm3, %%xmm3")        // xmm3 = d8 + k1*s0 + k0*s1
            __ASM_EMIT("vaddss          0x28(%[dst]), %%xmm4, %%xmm4")  // xmm4 = d10+ k2*s0
            __ASM_EMIT("vaddss          0x2c(%[dst]), %%xmm5, %%xmm5")  // xmm5 = d11+ k3*s0
            __ASM_EMIT("vaddss          0x34(%[dst]), %%xmm8, %%xmm8")  // xmm8 = d13+ k2*s1
            __ASM_EMIT("vaddss          0x38(%[dst]), %%xmm9, %%xmm9")  // xmm9 = d14+ k3*s1
            __ASM_EMIT("vaddss          %%xmm6, %%xmm4, %%xmm4")        // xmm4 = d10+ k2*s0 + k0*s1
            __ASM_EMIT("vaddss          %%xmm7, %%xmm5, %%xmm5")        // xmm5 = d11+ k3*s0 + k1*s1
            __ASM_EMIT("vmovss          %%xmm2, 0x1c(%[dst])")
            __ASM_EMIT("vmovss          %%xmm3, 0x20(%[dst])")
            __ASM_EMIT("vmovss          %%xmm1, 0x24(%[dst])")
            __ASM_EMIT("vmovss          %%xmm4, 0x28(%[dst])")
            __ASM_EMIT("vmovss          %%xmm5, 0x2c(%[dst])")
            __ASM_EMIT("vmovss          %%xmm8, 0x34(%[dst])")
            __ASM_EMIT("vmovss          %%xmm9, 0x38(%[dst])")
            __ASM_EMIT("add             $0x08, %[src]")
            __ASM_EMIT("add             $0x18, %[dst]")
            __ASM_EMIT("sub             $2, %[count]")
            __ASM_EMIT("jae             1b")
            __ASM_EMIT("2:")
            // 1x block
            __ASM_EMIT("add             $1, %[count]")
            __ASM_EMIT("jl              3f")
            __ASM_EMIT("vmovss          0x00(%[src]), %%xmm0")          // xmm0  = s0
            __ASM_EMIT("vmulss          %%xmm0, %%xmm15, %%xmm15")      // xmm15 = k0*s3
            __ASM_EMIT("vmulss          %%xmm0, %%xmm14, %%xmm14")      // xmm14 = k0*s2
            __ASM_EMIT("vaddss          0x04(%[dst]), %%xmm15, %%xmm1") // xmm1  = d1 + k0*s3
            __ASM_EMIT("vaddss          0x08(%[dst]), %%xmm14, %%xmm2") // xmm2  = d2 + k0*s2
            __ASM_EMIT("vmulss          %%xmm0, %%xmm13, %%xmm13")      // xmm13 = k0*s1
            __ASM_EMIT("vmulss          %%xmm0, %%xmm12, %%xmm12")      // xmm12 = k0*s0
            __ASM_EMIT("vmovss          %%xmm1, 0x04(%[dst])")
            __ASM_EMIT("vmovss          %%xmm2, 0x08(%[dst])")
            __ASM_EMIT("vaddss          0x10(%[dst]), %%xmm13, %%xmm3") // xmm3  = d4 + k0*s1
            __ASM_EMIT("vaddss          0x14(%[dst]), %%xmm12, %%xmm4") // xmm4  = d5 + k0*s0
            __ASM_EMIT("vaddss          0x18(%[dst]), %%xmm0, %%xmm0")  // xmm0  = d6 + s0
            __ASM_EMIT("vaddss          0x1c(%[dst]), %%xmm12, %%xmm5") // xmm5  = d7 + k0*s0
            __ASM_EMIT("vmovss          %%xmm3, 0x10(%[dst])")
            __ASM_EMIT("vmovss          %%xmm4, 0x14(%[dst])")
            __ASM_EMIT("vaddss          0x20(%[dst]), %%xmm13, %%xmm6") // xmm6  = d8 + k1*s0
            __ASM_EMIT("vaddss          0x28(%[dst]), %%xmm14, %%xmm7") // xmm7  = d10+ k2*s0
            __ASM_EMIT("vaddss          0x2c(%[dst]), %%xmm15, %%xmm8") // xmm8  = d11+ k3*s0
            __ASM_EMIT("vmovss          %%xmm0, 0x18(%[dst])")
            __ASM_EMIT("vmovss          %%xmm5, 0x1c(%[dst])")
            __ASM_EMIT("vmovss          %%xmm6, 0x20(%[dst])")
            __ASM_EMIT("vmovss          %%xmm7, 0x28(%[dst])")
            __ASM_EMIT("vmovss          %%xmm8, 0x2c(%[dst])")
            __ASM_EMIT("3:")

            : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
            : [k] "o" (lanczos_3x2)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7",
              "%xmm8", "%xmm9", "%xmm10", "%xmm11",
              "%xmm12", "%xmm13", "%xmm14", "%xmm15"
        );
    }

    // Lanczos kernel 3x3: 9 SSE registers
    static const float lanczos_3x3[] __lsp_aligned32 =
    {
        // Shifted by 0 left (head)
        +0.0000000000000000f,
        +0.0890793429479492f,
        +0.1055060549370832f,
        +0.0000000000000000f,

        // Shifted by 1 left (tail)
        +0.0890793429479492f,
        +0.0000000000000000f,
        +0.0000000000000000f,
        +0.0000000000000000f,

        // Shifted by 0 left
        -0.1562250559899557f,
        -0.1993645686793863f,
        +0.0000000000000000f,
        +0.4097746665281485f,

        +0.8251285571768335f,
        +1.0000000000000000f,
        +0.8251285571768335f,
        +0.4097746665281485f,

        +0.0000000000000000f,
        -0.1993645686793863f,
        -0.1562250559899557f,
        +0.0000000000000000f,

        +0.1055060549370832f,
        +0.0890793429479492f,
        +0.0000000000000000f,
        +0.0000000000000000f,

        // Shifted by 1 left
        +0.0890793429479492f,
        +0.1055060549370832f,
        +0.0000000000000000f,
        -0.1562250559899557f,

        -0.1993645686793863f,
        +0.0000000000000000f,
        +0.4097746665281485f,
        +0.8251285571768335f,

        +1.0000000000000000f,
        +0.8251285571768335f,
        +0.4097746665281485f,
        +0.0000000000000000f,

        -0.1993645686793863f,
        -0.1562250559899557f,
        +0.0000000000000000f,
        +0.1055060549370832f
    };

    void lanczos_resample_3x3(float *dst, const float *src, size_t count)
    {
        ARCH_I386_ASM (
            // 2x blocks
            __ASM_EMIT("sub             $2, %[count]")
            __ASM_EMIT("jb              2f")
            __ASM_EMIT(".align          16")
            __ASM_EMIT("1:")
            __ASM_EMIT("vbroadcastss    0x00(%[src]), %%ymm0")          // ymm0 = s0
            __ASM_EMIT("vbroadcastss    0x04(%[src]), %%ymm1")          // ymm1 = s1
            __ASM_EMIT("vmulps          0x00 + %[k], %%xmm0, %%xmm2")   // xmm2 = m0
            __ASM_EMIT("vmulss          0x10 + %[k], %%xmm1, %%xmm3")   // xmm3 = m5
            __ASM_EMIT("vmulps          0x20 + %[k], %%ymm0, %%ymm4")   // ymm4 = m1
            __ASM_EMIT("vmulps          0x40 + %[k], %%ymm0, %%ymm5")   // ymm5 = m2
            __ASM_EMIT("vmulps          0x60 + %[k], %%ymm1, %%ymm6")   // ymm6 = m3
            __ASM_EMIT("vmulps          0x80 + %[k], %%ymm1, %%ymm7")   // ymm7 = m4
            __ASM_EMIT("vaddps          %%ymm6, %%ymm4, %%ymm4")        // ymm4 = m1+m3
            __ASM_EMIT("vaddps          %%ymm7, %%ymm5, %%ymm5")        // ymm5 = m2+m4
            __ASM_EMIT("vaddps          0x00(%[dst]), %%xmm2, %%xmm2")
            __ASM_EMIT("vaddps          0x10(%[dst]), %%ymm4, %%ymm4")
            __ASM_EMIT("vaddps          0x30(%[dst]), %%ymm5, %%ymm5")
            __ASM_EMIT("vaddss          0x50(%[dst]), %%xmm3, %%xmm3")
            __ASM_EMIT("vmovups         %%xmm2, 0x00(%[dst])")
            __ASM_EMIT("vmovups         %%ymm4, 0x10(%[dst])")
            __ASM_EMIT("vmovups         %%ymm5, 0x30(%[dst])")
            __ASM_EMIT("vmovss          %%xmm3, 0x50(%[dst])")
            __ASM_EMIT("add             $0x08, %[src]")
            __ASM_EMIT("add             $0x18, %[dst]")
            __ASM_EMIT("sub             $2, %[count]")
            __ASM_EMIT("jae             1b")
            __ASM_EMIT("2:")
            // 1x block
            __ASM_EMIT("add             $1, %[count]")
            __ASM_EMIT("jl              3f")
            __ASM_EMIT("vbroadcastss    0x00(%[src]), %%ymm0")          // ymm0 = s0
            __ASM_EMIT("vmulps          0x00 + %[k], %%xmm0, %%xmm2")   // xmm2 = m0
            __ASM_EMIT("vmulps          0x20 + %[k], %%ymm0, %%ymm4")   // ymm4 = m1
            __ASM_EMIT("vmulps          0x40 + %[k], %%ymm0, %%ymm5")   // ymm5 = m2
            __ASM_EMIT("vaddps          0x00(%[dst]), %%xmm2, %%xmm2")
            __ASM_EMIT("vaddps          0x10(%[dst]), %%ymm4, %%ymm4")
            __ASM_EMIT("vaddps          0x30(%[dst]), %%ymm5, %%ymm5")
            __ASM_EMIT("vmovups         %%xmm2, 0x00(%[dst])")
            __ASM_EMIT("vmovups         %%ymm4, 0x10(%[dst])")
            __ASM_EMIT("vmovups         %%ymm5, 0x30(%[dst])")
            __ASM_EMIT("3:")

            : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
            : [k] "o" (lanczos_3x3)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );

        ARCH_X86_64_ASM (
            // 2x blocks
            __ASM_EMIT("vmovaps         0x00 + %[k], %%xmm8")
            __ASM_EMIT("vmovss          0x10 + %[k], %%xmm9")
            __ASM_EMIT("vmovaps         0x20 + %[k], %%ymm10")
            __ASM_EMIT("vmovaps         0x40 + %[k], %%ymm11")
            __ASM_EMIT("vmovaps         0x60 + %[k], %%ymm12")
            __ASM_EMIT("vmovaps         0x80 + %[k], %%ymm13")
            __ASM_EMIT("sub             $2, %[count]")
            __ASM_EMIT("jb              2f")
            __ASM_EMIT(".align          16")
            __ASM_EMIT("1:")
            __ASM_EMIT("vbroadcastss    0x00(%[src]), %%ymm0")          // ymm0 = s0
            __ASM_EMIT("vbroadcastss    0x04(%[src]), %%ymm1")          // ymm1 = s1
            __ASM_EMIT("vmulps          %%xmm8, %%xmm0, %%xmm2")        // xmm2 = m0
            __ASM_EMIT("vmulss          %%xmm9, %%xmm1, %%xmm3")        // xmm3 = m5
            __ASM_EMIT("vmulps          %%ymm10, %%ymm0, %%ymm4")       // ymm4 = m1
            __ASM_EMIT("vmulps          %%ymm11, %%ymm0, %%ymm5")       // ymm5 = m2
            __ASM_EMIT("vmulps          %%ymm12, %%ymm1, %%ymm6")       // ymm6 = m3
            __ASM_EMIT("vmulps          %%ymm13, %%ymm1, %%ymm7")       // ymm7 = m4
            __ASM_EMIT("vaddps          %%ymm6, %%ymm4, %%ymm4")        // ymm4 = m1+m3
            __ASM_EMIT("vaddps          %%ymm7, %%ymm5, %%ymm5")        // ymm5 = m2+m4
            __ASM_EMIT("vaddps          0x00(%[dst]), %%xmm2, %%xmm2")
            __ASM_EMIT("vaddps          0x10(%[dst]), %%ymm4, %%ymm4")
            __ASM_EMIT("vaddps          0x30(%[dst]), %%ymm5, %%ymm5")
            __ASM_EMIT("vaddss          0x50(%[dst]), %%xmm3, %%xmm3")
            __ASM_EMIT("vmovups         %%xmm2, 0x00(%[dst])")
            __ASM_EMIT("vmovups         %%ymm4, 0x10(%[dst])")
            __ASM_EMIT("vmovups         %%ymm5, 0x30(%[dst])")
            __ASM_EMIT("vmovss          %%xmm3, 0x50(%[dst])")
            __ASM_EMIT("add             $0x08, %[src]")
            __ASM_EMIT("add             $0x18, %[dst]")
            __ASM_EMIT("sub             $2, %[count]")
            __ASM_EMIT("jae             1b")
            __ASM_EMIT("2:")
            // 1x block
            __ASM_EMIT("add             $1, %[count]")
            __ASM_EMIT("jl              3f")
            __ASM_EMIT("vbroadcastss    0x00(%[src]), %%ymm0")          // ymm0 = s0
            __ASM_EMIT("vmulps          %%xmm8, %%xmm0, %%xmm2")        // xmm2 = m0
            __ASM_EMIT("vmulps          %%ymm10, %%ymm0, %%ymm4")       // ymm4 = m1
            __ASM_EMIT("vmulps          %%ymm11, %%ymm0, %%ymm5")       // ymm5 = m2
            __ASM_EMIT("vaddps          0x00(%[dst]), %%xmm2, %%xmm2")
            __ASM_EMIT("vaddps          0x10(%[dst]), %%ymm4, %%ymm4")
            __ASM_EMIT("vaddps          0x30(%[dst]), %%ymm5, %%ymm5")
            __ASM_EMIT("vmovups         %%xmm2, 0x00(%[dst])")
            __ASM_EMIT("vmovups         %%ymm4, 0x10(%[dst])")
            __ASM_EMIT("vmovups         %%ymm5, 0x30(%[dst])")
            __ASM_EMIT("3:")

            : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
            : [k] "o" (lanczos_3x3)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7",
              "%xmm8", "%xmm9", "%xmm10", "%xmm11",
              "%xmm12"
        );
    }

    // Lanczos kernel 4x2: 4 SSE registers
    static const float lanczos_4x2[] __lsp_aligned32 =
    {
        +0.0000000000000000f,
        -0.1187292496637064f,
        -0.2001458972657618f,
        -0.1729186314209981f,

        +0.0000000000000000f,
        +0.2957854651930789f,
        +0.6325370350028462f,
        +0.8988707620097378f,

        +1.0000000000000000f,
        +0.8988707620097378f,
        +0.6325370350028462f,
        +0.2957854651930789f,

        +0.0000000000000000f,
        -0.1729186314209981f,
        -0.2001458972657618f,
        -0.1187292496637064f
    };

    void lanczos_resample_4x2(float *dst, const float *src, size_t count)
    {
        ARCH_I386_ASM(
            // 2x blocks
            __ASM_EMIT("sub             $2, %[count]")
            __ASM_EMIT("jb              2f")
            __ASM_EMIT(".align          16")
            __ASM_EMIT("1:")
            __ASM_EMIT("vbroadcastss    0x00(%[src]), %%ymm0")          // ymm0 = s0
            __ASM_EMIT("vbroadcastss    0x04(%[src]), %%ymm1")          // ymm1 = s1
            __ASM_EMIT("vmulps          0x00 + %[k], %%xmm0, %%xmm2")   // xmm2 = m0
            __ASM_EMIT("vmulps          0x00 + %[k], %%ymm1, %%ymm5")   // ymm5 = m3
            __ASM_EMIT("vmulps          0x10 + %[k], %%ymm0, %%ymm3")   // ymm3 = m1
            __ASM_EMIT("vmulps          0x20 + %[k], %%ymm1, %%ymm6")   // ymm6 = m4
            __ASM_EMIT("vmulps          0x30 + %[k], %%xmm0, %%xmm4")   // xmm4 = m2
            __ASM_EMIT("vaddps          0x00(%[dst]), %%xmm2, %%xmm2")
            __ASM_EMIT("vaddps          0x10(%[dst]), %%ymm5, %%ymm5")
            __ASM_EMIT("vaddps          0x30(%[dst]), %%ymm6, %%ymm6")
            __ASM_EMIT("vaddps          %%ymm3, %%ymm5, %%ymm3")
            __ASM_EMIT("vaddps          %%ymm4, %%ymm6, %%ymm4")
            __ASM_EMIT("vmovups         %%xmm2, 0x00(%[dst])")
            __ASM_EMIT("vmovups         %%ymm3, 0x10(%[dst])")
            __ASM_EMIT("vmovups         %%ymm4, 0x30(%[dst])")
            __ASM_EMIT("add             $0x08, %[src]")
            __ASM_EMIT("add             $0x20, %[dst]")
            __ASM_EMIT("sub             $2, %[count]")
            __ASM_EMIT("jae             1b")
            __ASM_EMIT("2:")
            // 1x block
            __ASM_EMIT("add             $1, %[count]")
            __ASM_EMIT("jl              3f")
            __ASM_EMIT("vbroadcastss    0x00(%[src]), %%ymm0")          // ymm0 = s0
            __ASM_EMIT("vmulps          0x00 + %[k], %%ymm0, %%ymm1")   // xmm1 = m0
            __ASM_EMIT("vmulps          0x20 + %[k], %%ymm0, %%ymm2")   // ymm2 = m1
            __ASM_EMIT("vaddps          0x00(%[dst]), %%ymm1, %%ymm1")
            __ASM_EMIT("vaddps          0x20(%[dst]), %%ymm2, %%ymm2")
            __ASM_EMIT("vmovups         %%ymm1, 0x00(%[dst])")
            __ASM_EMIT("vmovups         %%ymm2, 0x20(%[dst])")
            __ASM_EMIT("3:")

            : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
            : [k] "o" (lanczos_4x2)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );

        ARCH_X86_64_ASM(
            // 2x blocks
            __ASM_EMIT("vmovaps         0x00 + %[k], %%ymm8")
            __ASM_EMIT("vmovups         0x10 + %[k], %%ymm9")
            __ASM_EMIT("vmovaps         0x20 + %[k], %%ymm10")
            __ASM_EMIT("vmovaps         0x30 + %[k], %%xmm11")
            __ASM_EMIT("sub             $2, %[count]")
            __ASM_EMIT("jb              2f")
            __ASM_EMIT(".align          16")
            __ASM_EMIT("1:")
            __ASM_EMIT("vbroadcastss    0x00(%[src]), %%ymm0")          // ymm0 = s0
            __ASM_EMIT("vbroadcastss    0x04(%[src]), %%ymm1")          // ymm1 = s1
            __ASM_EMIT("vmulps          %%xmm8, %%xmm0, %%xmm2")        // xmm2 = m0
            __ASM_EMIT("vmulps          %%ymm8, %%ymm1, %%ymm5")        // ymm5 = m3
            __ASM_EMIT("vaddps          0x00(%[dst]), %%xmm2, %%xmm2")
            __ASM_EMIT("vmulps          %%ymm9, %%ymm0, %%ymm3")        // ymm3 = m1
            __ASM_EMIT("vaddps          0x10(%[dst]), %%ymm5, %%ymm5")
            __ASM_EMIT("vmulps          %%ymm10, %%ymm1, %%ymm6")       // ymm6 = m4
            __ASM_EMIT("vmovups         %%xmm2, 0x00(%[dst])")
            __ASM_EMIT("vaddps          %%ymm3, %%ymm5, %%ymm3")
            __ASM_EMIT("vmulps          %%xmm11, %%xmm0, %%xmm4")       // xmm4 = m2
            __ASM_EMIT("vaddps          0x30(%[dst]), %%ymm6, %%ymm6")
            __ASM_EMIT("vaddps          %%ymm4, %%ymm6, %%ymm4")
            __ASM_EMIT("vmovups         %%ymm3, 0x10(%[dst])")
            __ASM_EMIT("vmovups         %%ymm4, 0x30(%[dst])")
            __ASM_EMIT("add             $0x08, %[src]")
            __ASM_EMIT("add             $0x20, %[dst]")
            __ASM_EMIT("sub             $2, %[count]")
            __ASM_EMIT("jae             1b")
            __ASM_EMIT("2:")
            // 1x block
            __ASM_EMIT("add             $1, %[count]")
            __ASM_EMIT("jl              3f")
            __ASM_EMIT("vbroadcastss    0x00(%[src]), %%ymm0")          // ymm0 = s0
            __ASM_EMIT("vmulps          %%ymm8, %%ymm0, %%ymm1")        // xmm1 = m0
            __ASM_EMIT("vmulps          %%ymm10, %%ymm0, %%ymm2")       // ymm2 = m1
            __ASM_EMIT("vaddps          0x00(%[dst]), %%ymm1, %%ymm1")
            __ASM_EMIT("vaddps          0x20(%[dst]), %%ymm2, %%ymm2")
            __ASM_EMIT("vmovups         %%ymm1, 0x00(%[dst])")
            __ASM_EMIT("vmovups         %%ymm2, 0x20(%[dst])")
            __ASM_EMIT("3:")

            : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
            : [k] "o" (lanczos_4x2)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7",
              "%xmm8", "%xmm9", "%xmm10", "%xmm11"
        );
    }

    static const float lanczos_4x3[] __lsp_aligned32 =
    {
        +0.0000000000000000f,
        +0.0749573750027847f,
        +0.1184264228024920f,
        +0.0943496807888575f,

        +0.0000000000000000f,
        -0.1241641556633335f,
        -0.2067943377468982f,
        -0.1768665375130828f,

        +0.0000000000000000f,
        +0.2981808178680967f,
        +0.6348032780190205f,
        +0.8996736756130924f,

        +1.0000000000000000f,
        +0.8996736756130924f,
        +0.6348032780190205f,
        +0.2981808178680967f,

        +0.0000000000000000f,
        -0.1768665375130828f,
        -0.2067943377468982f,
        -0.1241641556633335f,

        +0.0000000000000000f,
        +0.0943496807888575f,
        +0.1184264228024920f,
        +0.0749573750027847f,
    };

    void lanczos_resample_4x3(float *dst, const float *src, size_t count)
    {
        ARCH_I386_ASM(
            // 2x blocks
            __ASM_EMIT("sub             $2, %[count]")
            __ASM_EMIT("jb              2f")
            __ASM_EMIT(".align          16")
            __ASM_EMIT("1:")
            __ASM_EMIT("vbroadcastss    0x00(%[src]), %%ymm0")          // ymm0 = s0
            __ASM_EMIT("vbroadcastss    0x04(%[src]), %%ymm1")          // ymm1 = s1
            __ASM_EMIT("vmulps          0x00 + %[k], %%xmm0, %%xmm2")   // xmm2 = m0
            __ASM_EMIT("vmulps          0x00 + %[k], %%ymm1, %%ymm3")   // ymm3 = m4
            __ASM_EMIT("vmulps          0x10 + %[k], %%ymm0, %%ymm4")   // ymm4 = m1
            __ASM_EMIT("vmulps          0x20 + %[k], %%ymm1, %%ymm5")   // ymm5 = m5
            __ASM_EMIT("vmulps          0x30 + %[k], %%ymm0, %%ymm6")   // ymm6 = m2
            __ASM_EMIT("vmulps          0x40 + %[k], %%ymm1, %%ymm1")   // ymm1 = m6
            __ASM_EMIT("vmulps          0x50 + %[k], %%xmm0, %%xmm0")   // ymm0 = m3
            __ASM_EMIT("vaddps          0x00(%[dst]), %%xmm2, %%xmm2")
            __ASM_EMIT("vaddps          0x10(%[dst]), %%ymm3, %%ymm3")
            __ASM_EMIT("vaddps          0x30(%[dst]), %%ymm5, %%ymm5")
            __ASM_EMIT("vaddps          0x50(%[dst]), %%ymm1, %%ymm1")
            __ASM_EMIT("vaddps          %%ymm3, %%ymm4, %%ymm3")
            __ASM_EMIT("vaddps          %%ymm5, %%ymm6, %%ymm5")
            __ASM_EMIT("vaddps          %%ymm1, %%ymm0, %%ymm1")
            __ASM_EMIT("vmovups         %%xmm2, 0x00(%[dst])")
            __ASM_EMIT("vmovups         %%ymm3, 0x10(%[dst])")
            __ASM_EMIT("vmovups         %%ymm5, 0x30(%[dst])")
            __ASM_EMIT("vmovups         %%ymm1, 0x50(%[dst])")
            __ASM_EMIT("add             $0x08, %[src]")
            __ASM_EMIT("add             $0x20, %[dst]")
            __ASM_EMIT("sub             $2, %[count]")
            __ASM_EMIT("jae             1b")
            __ASM_EMIT("2:")
            // 1x block
            __ASM_EMIT("add             $1, %[count]")
            __ASM_EMIT("jl              3f")
            __ASM_EMIT("vbroadcastss    0x00(%[src]), %%ymm0")          // ymm0 = s0
            __ASM_EMIT("vmulps          0x00 + %[k], %%ymm0, %%ymm1")   // xmm1 = m0
            __ASM_EMIT("vmulps          0x20 + %[k], %%ymm0, %%ymm2")   // ymm2 = m1
            __ASM_EMIT("vmulps          0x40 + %[k], %%ymm0, %%ymm3")   // ymm3 = m2
            __ASM_EMIT("vaddps          0x00(%[dst]), %%ymm1, %%ymm1")
            __ASM_EMIT("vaddps          0x20(%[dst]), %%ymm2, %%ymm2")
            __ASM_EMIT("vaddps          0x40(%[dst]), %%ymm3, %%ymm3")
            __ASM_EMIT("vmovups         %%ymm1, 0x00(%[dst])")
            __ASM_EMIT("vmovups         %%ymm2, 0x20(%[dst])")
            __ASM_EMIT("vmovups         %%ymm3, 0x40(%[dst])")
            __ASM_EMIT("3:")

            : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
            : [k] "o" (lanczos_4x3)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );

        ARCH_X86_64_ASM(
            __ASM_EMIT("vmovaps         0x00 + %[k], %%ymm8")
            __ASM_EMIT("vmovups         0x10 + %[k], %%ymm9")
            __ASM_EMIT("vmovaps         0x20 + %[k], %%ymm10")
            __ASM_EMIT("vmovups         0x30 + %[k], %%ymm11")
            __ASM_EMIT("vmovaps         0x40 + %[k], %%ymm12")
            __ASM_EMIT("vmovaps         0x50 + %[k], %%xmm13")
            // 2x blocks
            __ASM_EMIT("sub             $2, %[count]")
            __ASM_EMIT("jb              2f")
            __ASM_EMIT(".align          16")
            __ASM_EMIT("1:")
            __ASM_EMIT("vbroadcastss    0x00(%[src]), %%ymm0")          // ymm0 = s0
            __ASM_EMIT("vbroadcastss    0x04(%[src]), %%ymm1")          // ymm1 = s1
            __ASM_EMIT("vmulps          %%xmm8, %%xmm0, %%xmm2")        // xmm2 = m0
            __ASM_EMIT("vmulps          %%ymm8, %%ymm1, %%ymm3")        // ymm3 = m4
            __ASM_EMIT("vaddps          0x00(%[dst]), %%xmm2, %%xmm2")
            __ASM_EMIT("vmulps          %%ymm9, %%ymm0, %%ymm4")        // ymm4 = m1
            __ASM_EMIT("vmulps          %%ymm10, %%ymm1, %%ymm5")       // ymm5 = m5
            __ASM_EMIT("vaddps          0x10(%[dst]), %%ymm3, %%ymm3")
            __ASM_EMIT("vmulps          %%ymm11, %%ymm0, %%ymm6")       // ymm6 = m2
            __ASM_EMIT("vmovups         %%xmm2, 0x00(%[dst])")
            __ASM_EMIT("vaddps          %%ymm3, %%ymm4, %%ymm3")
            __ASM_EMIT("vmulps          %%ymm12, %%ymm1, %%ymm1")       // ymm1 = m6
            __ASM_EMIT("vaddps          0x30(%[dst]), %%ymm5, %%ymm5")
            __ASM_EMIT("vmulps          %%xmm13, %%xmm0, %%xmm0")       // ymm0 = m3
            __ASM_EMIT("vmovups         %%ymm3, 0x10(%[dst])")
            __ASM_EMIT("vaddps          0x50(%[dst]), %%ymm1, %%ymm1")
            __ASM_EMIT("vaddps          %%ymm5, %%ymm6, %%ymm5")
            __ASM_EMIT("vaddps          %%ymm1, %%ymm0, %%ymm1")
            __ASM_EMIT("vmovups         %%ymm5, 0x30(%[dst])")
            __ASM_EMIT("vmovups         %%ymm1, 0x50(%[dst])")
            __ASM_EMIT("add             $0x08, %[src]")
            __ASM_EMIT("add             $0x20, %[dst]")
            __ASM_EMIT("sub             $2, %[count]")
            __ASM_EMIT("jae             1b")
            __ASM_EMIT("2:")
            // 1x block
            __ASM_EMIT("add             $1, %[count]")
            __ASM_EMIT("jl              3f")
            __ASM_EMIT("vbroadcastss    0x00(%[src]), %%ymm0")          // ymm0 = s0
            __ASM_EMIT("vmulps          %%ymm8, %%ymm0, %%ymm1")        // xmm1 = m0
            __ASM_EMIT("vmulps          %%ymm10, %%ymm0, %%ymm2")       // ymm2 = m1
            __ASM_EMIT("vmulps          %%ymm12, %%ymm0, %%ymm3")       // ymm3 = m2
            __ASM_EMIT("vaddps          0x00(%[dst]), %%ymm1, %%ymm1")
            __ASM_EMIT("vaddps          0x20(%[dst]), %%ymm2, %%ymm2")
            __ASM_EMIT("vaddps          0x40(%[dst]), %%ymm3, %%ymm3")
            __ASM_EMIT("vmovups         %%ymm1, 0x00(%[dst])")
            __ASM_EMIT("vmovups         %%ymm2, 0x20(%[dst])")
            __ASM_EMIT("vmovups         %%ymm3, 0x40(%[dst])")
            __ASM_EMIT("3:")

            : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
            : [k] "o" (lanczos_4x3)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7",
              "%xmm8", "%xmm9", "%xmm10", "%xmm11",
              "%xmm12", "%xmm13"
        );
    }

    // Lanczos kernel 6x2: 3 AVX registers
    static const float lanczos_6x2[] __lsp_aligned32 =
    {
        +0.0000000000000000f,
        -0.0835168749111343f,
        -0.1602001404590001f,
        -0.2067943377468982f,

        -0.2025752314530442f,
        -0.1343073344351001f,
        +0.0000000000000000f,
        +0.1894744904080251f,

        +0.4114005618526707f,
        +0.6348032780190205f,
        +0.8259440903283795f,
        +0.9546266788436470f,

        +1.0000000000000000f,
        +0.9546266788436470f,
        +0.8259440903283795f,
        +0.6348032780190205f,

        +0.4114005618526707f,
        +0.1894744904080251f,
        +0.0000000000000000f,
        -0.1343073344351001f,

        -0.2025752314530442f,
        -0.2067943377468982f,
        -0.1602001404590001f,
        -0.0835168749111343f
    };

    void lanczos_resample_6x2(float *dst, const float *src, size_t count)
    {
        ARCH_X86_ASM(
            // 1x blocks
            __ASM_EMIT("vmovaps         0x00 + %[k], %%ymm5")
            __ASM_EMIT("vmovaps         0x20 + %[k], %%ymm6")
            __ASM_EMIT("vmovaps         0x40 + %[k], %%ymm7")
            __ASM_EMIT("dec             %[count]")
            __ASM_EMIT("js              2f")
            __ASM_EMIT(".align          16")
            __ASM_EMIT("1:")
            __ASM_EMIT("vbroadcastss    0x00(%[src]), %%ymm0")          // ymm0 = s0
            __ASM_EMIT("vmulps          %%ymm5, %%ymm0, %%ymm1")        // ymm1 = m0
            __ASM_EMIT("vmulps          %%ymm6, %%ymm0, %%ymm2")        // ymm2 = m1
            __ASM_EMIT("vaddps          0x00(%[dst]), %%ymm1, %%ymm1")
            __ASM_EMIT("vmulps          %%ymm7, %%ymm0, %%ymm3")        // ymm3 = m2
            __ASM_EMIT("vaddps          0x20(%[dst]), %%ymm2, %%ymm2")
            __ASM_EMIT("vmovups         %%ymm1, 0x00(%[dst])")
            __ASM_EMIT("vaddps          0x40(%[dst]), %%ymm3, %%ymm3")
            __ASM_EMIT("vmovups         %%ymm2, 0x20(%[dst])")
            __ASM_EMIT("vmovups         %%ymm3, 0x40(%[dst])")
            __ASM_EMIT("add             $0x04, %[src]")
            __ASM_EMIT("add             $0x18, %[dst]")
            __ASM_EMIT("dec             %[count]")
            __ASM_EMIT("jns             1b")
            __ASM_EMIT("2:")

            : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
            : [k] "o" (lanczos_6x2)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    // Lanczos kernel 6x3: 4 AVX + 1 SSE registers
    static const float lanczos_6x3[] __lsp_aligned32 =
    {
        +0.0000000000000000f,
        +0.0539107649662794f,
        +0.0996822843396933f,
        +0.1233221256242928f,
        +0.1149032936217488f,
        +0.0717178866781924f,
        -0.0000000000000000f,
        -0.0853379770956540f,

        -0.1630759564500400f,
        -0.2097908159917360f,
        -0.2048873332640742f,
        -0.1354777817419537f,
        +0.0000000000000000f,
        +0.1903132912211352f,
        +0.4125642785884168f,
        +0.6358120571822271f,

        +0.8265269098929101f,
        +0.9547949937819170f,
        +1.0000000000000000f,
        +0.9547949937819170f,
        +0.8265269098929101f,
        +0.6358120571822271f,
        +0.4125642785884168f,
        +0.1903132912211352f,

        +0.0000000000000000f,
        -0.1354777817419537f,
        -0.2048873332640742f,
        -0.2097908159917360f,
        -0.1630759564500400f,
        -0.0853379770956540f,
        -0.0000000000000000f,
        +0.0717178866781924f,

        +0.1149032936217488f,
        +0.1233221256242928f,
        +0.0996822843396933f,
        +0.0539107649662794f
    };

    void lanczos_resample_6x3(float *dst, const float *src, size_t count)
    {
        ARCH_X86_ASM(
            // 1x blocks
            __ASM_EMIT("vmovaps         0x00 + %[k], %%ymm3")
            __ASM_EMIT("vmovaps         0x20 + %[k], %%ymm4")
            __ASM_EMIT("vmovaps         0x40 + %[k], %%ymm5")
            __ASM_EMIT("vmovaps         0x60 + %[k], %%ymm6")
            __ASM_EMIT("vmovaps         0x80 + %[k], %%xmm7")
            __ASM_EMIT("dec             %[count]")
            __ASM_EMIT("js              2f")
            __ASM_EMIT(".align          16")
            __ASM_EMIT("1:")
            __ASM_EMIT("vbroadcastss    0x00(%[src]), %%ymm0")
            __ASM_EMIT("vmulps          %%ymm3, %%ymm0, %%ymm1")        // ymm1 = m0
            __ASM_EMIT("vmulps          %%ymm4, %%ymm0, %%ymm2")        // ymm2 = m1
            __ASM_EMIT("vaddps          0x00(%[dst]), %%ymm1, %%ymm1")
            __ASM_EMIT("vaddps          0x20(%[dst]), %%ymm2, %%ymm2")
            __ASM_EMIT("vmovups         %%ymm1, 0x00(%[dst])")
            __ASM_EMIT("vmovups         %%ymm2, 0x20(%[dst])")
            __ASM_EMIT("vmulps          %%ymm5, %%ymm0, %%ymm1")        // ymm1 = m2
            __ASM_EMIT("vmulps          %%ymm6, %%ymm0, %%ymm2")        // ymm2 = m3
            __ASM_EMIT("vaddps          0x40(%[dst]), %%ymm1, %%ymm1")
            __ASM_EMIT("vmulps          %%xmm7, %%xmm0, %%xmm0")        // xmm0 = m4
            __ASM_EMIT("vaddps          0x60(%[dst]), %%ymm2, %%ymm2")
            __ASM_EMIT("vaddps          0x80(%[dst]), %%xmm0, %%xmm0")
            __ASM_EMIT("vmovups         %%ymm1, 0x40(%[dst])")
            __ASM_EMIT("vmovups         %%ymm2, 0x60(%[dst])")
            __ASM_EMIT("vmovups         %%xmm0, 0x80(%[dst])")
            __ASM_EMIT("add             $0x04, %[src]")
            __ASM_EMIT("add             $0x18, %[dst]")
            __ASM_EMIT("dec             %[count]")
            __ASM_EMIT("jns             1b")
            __ASM_EMIT("2:")

            : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
            : [k] "o" (lanczos_6x3)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    // Lanczos kernel 8x2: 4 AVX registers
    static const float lanczos_8x2[] __lsp_aligned32 =
    {
        -0.0000000000000000f,
        -0.0635086982303136f,
        -0.1261005767591211f,
        -0.1779172640206227f,
        -0.2091519082144436f,
        -0.2112875836092082f,
        -0.1782608868029105f,
        -0.1073988642554405f,

        +0.0000000000000000f,
        +0.1385297654836717f,
        +0.2990219260297742f,
        +0.4693478599070060f,
        +0.6355976112274050f,
        +0.7835048889119777f,
        +0.8999547969760605f,
        +0.9743975231790596f,

        +1.0000000000000000f,
        +0.9743975231790596f,
        +0.8999547969760605f,
        +0.7835048889119777f,
        +0.6355976112274050f,
        +0.4693478599070060f,
        +0.2990219260297742f,
        +0.1385297654836717f,

        +0.0000000000000000f,
        -0.1073988642554405f,
        -0.1782608868029105f,
        -0.2112875836092082f,
        -0.2091519082144436f,
        -0.1779172640206227f,
        -0.1261005767591211f,
        -0.0635086982303136f
    };


    void lanczos_resample_8x2(float *dst, const float *src, size_t count)
    {
        ARCH_I386_ASM(
            // 1x blocks
            __ASM_EMIT("vmovaps         0x00 + %[k], %%ymm4")
            __ASM_EMIT("vmovaps         0x20 + %[k], %%ymm5")
            __ASM_EMIT("vmovaps         0x40 + %[k], %%ymm6")
            __ASM_EMIT("vmovaps         0x60 + %[k], %%ymm7")
            __ASM_EMIT("dec             %[count]")
            __ASM_EMIT("js              2f")
            __ASM_EMIT(".align          16")
            __ASM_EMIT("1:")
            __ASM_EMIT("vbroadcastss    0x00(%[src]), %%ymm3")          // ymm4 = s0
            __ASM_EMIT("vmulps          %%ymm4, %%ymm3, %%ymm0")        // ymm0 = m0
            __ASM_EMIT("vmulps          %%ymm5, %%ymm3, %%ymm1")        // ymm1 = m1
            __ASM_EMIT("vaddps          0x00(%[dst]), %%ymm0, %%ymm0")
            __ASM_EMIT("vmulps          %%ymm6, %%ymm3, %%ymm2")        // ymm2 = m2
            __ASM_EMIT("vaddps          0x20(%[dst]), %%ymm1, %%ymm1")
            __ASM_EMIT("vmulps          %%ymm7, %%ymm3, %%ymm3")        // ymm3 = m3
            __ASM_EMIT("vaddps          0x40(%[dst]), %%ymm2, %%ymm2")
            __ASM_EMIT("vaddps          0x60(%[dst]), %%ymm3, %%ymm3")
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
            __ASM_EMIT("vmovups         %%ymm1, 0x20(%[dst])")
            __ASM_EMIT("vmovups         %%ymm2, 0x40(%[dst])")
            __ASM_EMIT("vmovups         %%ymm3, 0x60(%[dst])")
            __ASM_EMIT("add             $0x04, %[src]")
            __ASM_EMIT("add             $0x20, %[dst]")
            __ASM_EMIT("dec             %[count]")
            __ASM_EMIT("jns             1b")
            __ASM_EMIT("2:")

            : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
            : [k] "o" (lanczos_8x2)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );

        ARCH_X86_64_ASM(
            // 1x blocks
            __ASM_EMIT("vmovaps         0x00 + %[k], %%ymm8")
            __ASM_EMIT("vmovaps         0x20 + %[k], %%ymm9")
            __ASM_EMIT("vmovaps         0x40 + %[k], %%ymm10")
            __ASM_EMIT("vmovaps         0x60 + %[k], %%ymm11")
            __ASM_EMIT("sub             $2, %[count]")
            __ASM_EMIT("jb              2f")
            // 2x blocks
            __ASM_EMIT(".align          16")
            __ASM_EMIT("1:")
            __ASM_EMIT("vbroadcastss    0x00(%[src]), %%ymm3")          // ymm0 = s0
            __ASM_EMIT("vbroadcastss    0x04(%[src]), %%ymm7")          // ymm7 = s1
            __ASM_EMIT("vmulps          %%ymm8, %%ymm3, %%ymm0")        // ymm0 = m0
            __ASM_EMIT("vmulps          %%ymm8, %%ymm7, %%ymm4")        // ymm4 = m4
            __ASM_EMIT("vaddps          0x00(%[dst]), %%ymm0, %%ymm0")
            __ASM_EMIT("vmulps          %%ymm9, %%ymm3, %%ymm1")        // ymm1 = m1
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
            __ASM_EMIT("vmulps          %%ymm9, %%ymm7, %%ymm5")        // ymm5 = m5
            __ASM_EMIT("vaddps          0x20(%[dst]), %%ymm1, %%ymm1")
            __ASM_EMIT("vmulps          %%ymm10, %%ymm3, %%ymm2")       // ymm2 = m2
            __ASM_EMIT("vaddps          %%ymm4, %%ymm1, %%ymm1")
            __ASM_EMIT("vmulps          %%ymm10, %%ymm7, %%ymm6")       // ymm6 = m6
            __ASM_EMIT("vmovups         %%ymm1, 0x20(%[dst])")
            __ASM_EMIT("vaddps          0x40(%[dst]), %%ymm2, %%ymm2")
            __ASM_EMIT("vmulps          %%ymm11, %%ymm3, %%ymm3")       // ymm3 = m3
            __ASM_EMIT("vaddps          %%ymm5, %%ymm2, %%ymm2")
            __ASM_EMIT("vaddps          0x60(%[dst]), %%ymm3, %%ymm3")
            __ASM_EMIT("vmulps          %%ymm11, %%ymm7, %%ymm7")       // ymm7 = m7
            __ASM_EMIT("vmovups         %%ymm2, 0x40(%[dst])")
            __ASM_EMIT("vaddps          %%ymm6, %%ymm3, %%ymm3")
            __ASM_EMIT("vaddps          0x80(%[dst]), %%ymm7, %%ymm7")
            __ASM_EMIT("vmovups         %%ymm3, 0x60(%[dst])")
            __ASM_EMIT("vmovups         %%ymm7, 0x80(%[dst])")
            __ASM_EMIT("add             $0x08, %[src]")
            __ASM_EMIT("add             $0x40, %[dst]")
            __ASM_EMIT("sub             $2, %[count]")
            __ASM_EMIT("jae             1b")
            __ASM_EMIT("2:")
            // 1x block
            __ASM_EMIT("add             $1, %[count]")
            __ASM_EMIT("jl              4f")
            __ASM_EMIT("vbroadcastss    0x00(%[src]), %%ymm3")
            __ASM_EMIT("vmulps          %%ymm8, %%ymm3, %%ymm0")        // ymm0 = m0
            __ASM_EMIT("vmulps          %%ymm9, %%ymm3, %%ymm1")        // ymm1 = m1
            __ASM_EMIT("vaddps          0x00(%[dst]), %%ymm0, %%ymm0")
            __ASM_EMIT("vmulps          %%ymm10, %%ymm3, %%ymm2")       // ymm2 = m2
            __ASM_EMIT("vaddps          0x20(%[dst]), %%ymm1, %%ymm1")
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
            __ASM_EMIT("vmulps          %%ymm11, %%ymm3, %%ymm3")       // ymm3 = m3
            __ASM_EMIT("vaddps          0x40(%[dst]), %%ymm2, %%ymm2")
            __ASM_EMIT("vmovups         %%ymm1, 0x20(%[dst])")
            __ASM_EMIT("vaddps          0x60(%[dst]), %%ymm3, %%ymm3")
            __ASM_EMIT("vmovups         %%ymm2, 0x40(%[dst])")
            __ASM_EMIT("vmovups         %%ymm3, 0x60(%[dst])")
            __ASM_EMIT("4:")

            : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
            : [k] "o" (lanczos_8x2)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7",
              "%xmm8", "%xmm9", "%xmm10", "%xmm11"
        );
    }

    static const float lanczos_8x3[] __lsp_aligned16 =
    {
        +0.0000000000000000f,
        +0.0413762983728270f,
        +0.0800907151667399f,
        +0.1098388956286788f,
        +0.1250635280404858f,
        +0.1218381732799711f,
        +0.0985951550643596f,
        +0.0565868891962354f,

        -0.0000000000000000f,
        -0.0643160644975587f,
        -0.1274947024165923f,
        -0.1796106608775988f,
        -0.2108456783342821f,
        -0.2127234580145891f,
        -0.1792608653618989f,
        -0.1078863334836789f,

        +0.0000000000000000f,
        +0.1389094368005209f,
        +0.2996235873365793f,
        +0.4700032639087741f,
        +0.6361653568810439f,
        +0.7838984051945062f,
        +0.9001556302107272f,
        +0.9744518753544421f,

        +1.0000000000000000f,
        +0.9744518753544421f,
        +0.9001556302107272f,
        +0.7838984051945062f,
        +0.6361653568810439f,
        +0.4700032639087741f,
        +0.2996235873365793f,
        +0.1389094368005209f,

        +0.0000000000000000f,
        -0.1078863334836789f,
        -0.1792608653618989f,
        -0.2127234580145891f,
        -0.2108456783342821f,
        -0.1796106608775988f,
        -0.1274947024165923f,
        -0.0643160644975587f,

        -0.0000000000000000f,
        +0.0565868891962354f,
        +0.0985951550643596f,
        +0.1218381732799711f,
        +0.1250635280404858f,
        +0.1098388956286788f,
        +0.0800907151667399f,
        +0.0413762983728270f
    };

    void lanczos_resample_8x3(float *dst, const float *src, size_t count)
    {
        ARCH_I386_ASM(
            // 1x blocks
            __ASM_EMIT("vmovaps         0x40 + %[k], %%ymm4")
            __ASM_EMIT("vmovaps         0x60 + %[k], %%ymm5")
            __ASM_EMIT("vmovaps         0x80 + %[k], %%ymm6")
            __ASM_EMIT("vmovaps         0xa0 + %[k], %%ymm7")
            __ASM_EMIT("dec             %[count]")
            __ASM_EMIT("js              2f")
            __ASM_EMIT(".align          16")
            __ASM_EMIT("1:")
            __ASM_EMIT("vbroadcastss    0x00(%[src]), %%ymm3")          // ymm4 = s0
            __ASM_EMIT("vmulps          0x00 + %[k], %%ymm3, %%ymm0")   // ymm0 = m0
            __ASM_EMIT("vmulps          0x20 + %[k], %%ymm3, %%ymm1")   // ymm1 = m1
            __ASM_EMIT("vaddps          0x00(%[dst]), %%ymm0, %%ymm0")
            __ASM_EMIT("vmulps          %%ymm4, %%ymm3, %%ymm2")        // ymm2 = m2
            __ASM_EMIT("vaddps          0x20(%[dst]), %%ymm1, %%ymm1")
            __ASM_EMIT("vaddps          0x40(%[dst]), %%ymm2, %%ymm2")
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
            __ASM_EMIT("vmovups         %%ymm1, 0x20(%[dst])")
            __ASM_EMIT("vmulps          %%ymm5, %%ymm3, %%ymm0")        // ymm0 = m3
            __ASM_EMIT("vmovups         %%ymm2, 0x40(%[dst])")
            __ASM_EMIT("vmulps          %%ymm6, %%ymm3, %%ymm1")        // ymm1 = m4
            __ASM_EMIT("vaddps          0x60(%[dst]), %%ymm0, %%ymm0")
            __ASM_EMIT("vmulps          %%ymm7, %%ymm3, %%ymm2")        // ymm2 = m5
            __ASM_EMIT("vaddps          0x80(%[dst]), %%ymm1, %%ymm1")
            __ASM_EMIT("vmovups         %%ymm0, 0x60(%[dst])")
            __ASM_EMIT("vaddps          0xa0(%[dst]), %%ymm2, %%ymm2")
            __ASM_EMIT("vmovups         %%ymm1, 0x80(%[dst])")
            __ASM_EMIT("vmovups         %%ymm2, 0xa0(%[dst])")
            __ASM_EMIT("add             $0x04, %[src]")
            __ASM_EMIT("add             $0x20, %[dst]")
            __ASM_EMIT("dec             %[count]")
            __ASM_EMIT("jns             1b")
            __ASM_EMIT("2:")

            : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
            : [k] "o" (lanczos_8x3)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );

        ARCH_X86_64_ASM(
            // 1x blocks
            __ASM_EMIT("vmovaps         0x00 + %[k], %%ymm8")
            __ASM_EMIT("vmovaps         0x20 + %[k], %%ymm9")
            __ASM_EMIT("vmovaps         0x40 + %[k], %%ymm10")
            __ASM_EMIT("vmovaps         0x60 + %[k], %%ymm11")
            __ASM_EMIT("vmovaps         0x80 + %[k], %%ymm12")
            __ASM_EMIT("vmovaps         0xa0 + %[k], %%ymm13")
            __ASM_EMIT("sub             $2, %[count]")
            __ASM_EMIT("jb              2f")
            // 2x blocks
            __ASM_EMIT(".align          16")
            __ASM_EMIT("1:")
            __ASM_EMIT("vbroadcastss    0x00(%[src]), %%ymm6")          // ymm6 = s0
            __ASM_EMIT("vbroadcastss    0x04(%[src]), %%ymm7")          // ymm7 = s1
            __ASM_EMIT("vmulps          %%ymm8, %%ymm6, %%ymm0")        // ymm0 = m0
            __ASM_EMIT("vmulps          %%ymm8, %%ymm7, %%ymm3")        // ymm3 = m6
            __ASM_EMIT("vaddps          0x00(%[dst]), %%ymm0, %%ymm0")
            __ASM_EMIT("vmulps          %%ymm9, %%ymm6, %%ymm1")        // ymm1 = m1
            __ASM_EMIT("vmulps          %%ymm9, %%ymm7, %%ymm4")        // ymm4 = m7
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
            __ASM_EMIT("vaddps          0x20(%[dst]), %%ymm1, %%ymm1")
            __ASM_EMIT("vmulps          %%ymm10, %%ymm6, %%ymm2")       // ymm2 = m2
            __ASM_EMIT("vaddps          %%ymm3, %%ymm1, %%ymm1")
            __ASM_EMIT("vmulps          %%ymm10, %%ymm7, %%ymm5")       // ymm5 = m8
            __ASM_EMIT("vmovups         %%ymm1, 0x20(%[dst])")
            __ASM_EMIT("vaddps          0x40(%[dst]), %%ymm2, %%ymm2")
            __ASM_EMIT("vmulps          %%ymm11, %%ymm6, %%ymm0")       // ymm0 = m3
            __ASM_EMIT("vaddps          %%ymm4, %%ymm2, %%ymm2")
            __ASM_EMIT("vmulps          %%ymm11, %%ymm7, %%ymm3")       // ymm3 = m9
            __ASM_EMIT("vmovups         %%ymm2, 0x40(%[dst])")
            __ASM_EMIT("vmulps          %%ymm12, %%ymm6, %%ymm1")       // ymm1 = m4
            __ASM_EMIT("vaddps          0x60(%[dst]), %%ymm0, %%ymm0")
            __ASM_EMIT("vmulps          %%ymm12, %%ymm7, %%ymm4")       // ymm4 = m10
            __ASM_EMIT("vaddps          %%ymm5, %%ymm0, %%ymm0")
            __ASM_EMIT("vaddps          0x80(%[dst]), %%ymm1, %%ymm1")
            __ASM_EMIT("vmulps          %%ymm13, %%ymm6, %%ymm2")       // ymm2 = m5
            __ASM_EMIT("vmovups         %%ymm0, 0x60(%[dst])")
            __ASM_EMIT("vaddps          %%ymm3, %%ymm1, %%ymm1")
            __ASM_EMIT("vaddps          0xa0(%[dst]), %%ymm2, %%ymm2")
            __ASM_EMIT("vmovups         %%ymm1, 0x80(%[dst])")
            __ASM_EMIT("vmulps          %%ymm13, %%ymm7, %%ymm7")       // ymm7 = m11
            __ASM_EMIT("vaddps          %%ymm4, %%ymm2, %%ymm2")
            __ASM_EMIT("vaddps          0xc0(%[dst]), %%ymm7, %%ymm7")
            __ASM_EMIT("vmovups         %%ymm2, 0xa0(%[dst])")
            __ASM_EMIT("vmovups         %%ymm7, 0xc0(%[dst])")
            __ASM_EMIT("add             $0x08, %[src]")
            __ASM_EMIT("add             $0x40, %[dst]")
            __ASM_EMIT("sub             $2, %[count]")
            __ASM_EMIT("jae             1b")
            __ASM_EMIT("2:")
            // 1x block
            __ASM_EMIT("add             $1, %[count]")
            __ASM_EMIT("jl              4f")
            __ASM_EMIT("vbroadcastss    0x00(%[src]), %%ymm5")          // ymm4 = s0
            __ASM_EMIT("vmulps          %%ymm8, %%ymm5, %%ymm0")        // ymm0 = m0
            __ASM_EMIT("vmulps          %%ymm9, %%ymm5, %%ymm1")        // ymm1 = m1
            __ASM_EMIT("vmulps          %%ymm10, %%ymm5, %%ymm2")       // ymm2 = m2
            __ASM_EMIT("vmulps          %%ymm11, %%ymm5, %%ymm3")       // ymm3 = m3
            __ASM_EMIT("vmulps          %%ymm12, %%ymm5, %%ymm4")       // ymm4 = m4
            __ASM_EMIT("vmulps          %%ymm13, %%ymm5, %%ymm5")       // ymm5 = m5
            __ASM_EMIT("vaddps          0x00(%[dst]), %%ymm0, %%ymm0")
            __ASM_EMIT("vaddps          0x20(%[dst]), %%ymm1, %%ymm1")
            __ASM_EMIT("vaddps          0x40(%[dst]), %%ymm2, %%ymm2")
            __ASM_EMIT("vaddps          0x60(%[dst]), %%ymm3, %%ymm3")
            __ASM_EMIT("vaddps          0x80(%[dst]), %%ymm4, %%ymm4")
            __ASM_EMIT("vaddps          0xa0(%[dst]), %%ymm5, %%ymm5")
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
            __ASM_EMIT("vmovups         %%ymm1, 0x20(%[dst])")
            __ASM_EMIT("vmovups         %%ymm2, 0x40(%[dst])")
            __ASM_EMIT("vmovups         %%ymm3, 0x60(%[dst])")
            __ASM_EMIT("vmovups         %%ymm4, 0x80(%[dst])")
            __ASM_EMIT("vmovups         %%ymm5, 0xa0(%[dst])")
            __ASM_EMIT("4:")

            : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
            : [k] "o" (lanczos_8x3)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7",
              "%xmm8", "%xmm9", "%xmm10", "%xmm11",
              "%xmm12", "%xmm13"
        );
    }

    void downsample_2x(float *dst, const float *src, size_t count)
    {
        ARCH_X86_ASM(
            // 32x blocks
            __ASM_EMIT("sub             $32, %[count]")
            __ASM_EMIT("jb              2f")
            __ASM_EMIT(".align 16")
            __ASM_EMIT("1:")
            __ASM_EMIT("vmovups         0x000(%[src]), %%xmm0")                 // xmm0 = s0 ? s1 ?
            __ASM_EMIT("vmovups         0x010(%[src]), %%xmm1")                 // xmm1 = s2 ? s3 ?
            __ASM_EMIT("vmovups         0x020(%[src]), %%xmm2")                 // xmm2 = s4 ? s5 ?
            __ASM_EMIT("vmovups         0x030(%[src]), %%xmm3")                 // xmm3 = s6 ? s7 ?
            __ASM_EMIT("vmovups         0x040(%[src]), %%xmm4")                 // xmm4 = s8 ? s9 ?
            __ASM_EMIT("vmovups         0x050(%[src]), %%xmm5")                 // xmm5 = s10 ? s11 ?
            __ASM_EMIT("vmovups         0x060(%[src]), %%xmm6")                 // xmm6 = s12 ? s13 ?
            __ASM_EMIT("vmovups         0x070(%[src]), %%xmm7")                 // xmm7 = s14 ? s15 ?
            __ASM_EMIT("vinsertf128     $1, 0x080(%[src]), %%ymm0, %%ymm0")     // ymm0 = s0 ? s1 ? s16 ? s17
            __ASM_EMIT("vinsertf128     $1, 0x090(%[src]), %%ymm1, %%ymm1")     // ymm1 = s2 ? s3 ? s18 ? s19
            __ASM_EMIT("vinsertf128     $1, 0x0a0(%[src]), %%ymm2, %%ymm2")     // ymm2 = s4 ? s5 ? s20 ? s21
            __ASM_EMIT("vinsertf128     $1, 0x0b0(%[src]), %%ymm3, %%ymm3")     // ymm3 = s6 ? s7 ? s22 ? s23
            __ASM_EMIT("vinsertf128     $1, 0x0c0(%[src]), %%ymm4, %%ymm4")     // ymm4 = s8 ? s9 ? s24 ? s25
            __ASM_EMIT("vinsertf128     $1, 0x0d0(%[src]), %%ymm5, %%ymm5")     // ymm5 = s10 ? s11 ? s26 ? s27
            __ASM_EMIT("vinsertf128     $1, 0x0e0(%[src]), %%ymm6, %%ymm6")     // ymm6 = s12 ? s13 ? s28 ? s29
            __ASM_EMIT("vinsertf128     $1, 0x0f0(%[src]), %%ymm7, %%ymm7")     // ymm7 = s14 ? s15 ? s30 ? s31
            __ASM_EMIT("vshufps         $0x88, %%ymm1, %%ymm0, %%ymm0")         // ymm0 = s0 s1 s2 s3 s17 s17 s18 s19
            __ASM_EMIT("vshufps         $0x88, %%ymm3, %%ymm2, %%ymm2")         // ymm2 = s4 s5 s6 s7 s20 s21 s22 s23
            __ASM_EMIT("vshufps         $0x88, %%ymm5, %%ymm4, %%ymm4")         // ymm4 = s8 s9 s10 s11 s24 s25 s26 s27
            __ASM_EMIT("vshufps         $0x88, %%ymm7, %%ymm6, %%ymm6")         // ymm6 = s12 s13 s14 s15 s28 s29 s30 s31
            __ASM_EMIT("vmovups         %%xmm0, 0x000(%[dst])")
            __ASM_EMIT("vmovups         %%xmm2, 0x010(%[dst])")
            __ASM_EMIT("vmovups         %%xmm4, 0x020(%[dst])")
            __ASM_EMIT("vmovups         %%xmm6, 0x030(%[dst])")
            __ASM_EMIT("vextractf128    $1, %%ymm0, 0x040(%[dst])")
            __ASM_EMIT("vextractf128    $1, %%ymm2, 0x050(%[dst])")
            __ASM_EMIT("vextractf128    $1, %%ymm4, 0x060(%[dst])")
            __ASM_EMIT("vextractf128    $1, %%ymm6, 0x070(%[dst])")
            __ASM_EMIT("add             $0x100, %[src]")
            __ASM_EMIT("add             $0x80, %[dst]")
            __ASM_EMIT("sub             $32, %[count]")
            __ASM_EMIT("jae             1b")
            __ASM_EMIT("2:")
            // 16x block
            __ASM_EMIT("add             $16, %[count]")
            __ASM_EMIT("jl              4f")
            __ASM_EMIT("vmovups         0x000(%[src]), %%xmm0")                 // xmm0 = s0 ? s1 ?
            __ASM_EMIT("vmovups         0x010(%[src]), %%xmm1")                 // xmm1 = s2 ? s3 ?
            __ASM_EMIT("vmovups         0x020(%[src]), %%xmm2")                 // xmm2 = s4 ? s5 ?
            __ASM_EMIT("vmovups         0x030(%[src]), %%xmm3")                 // xmm3 = s6 ? s7 ?
            __ASM_EMIT("vinsertf128     $1, 0x040(%[src]), %%ymm0, %%ymm0")
            __ASM_EMIT("vinsertf128     $1, 0x050(%[src]), %%ymm1, %%ymm1")
            __ASM_EMIT("vinsertf128     $1, 0x060(%[src]), %%ymm2, %%ymm2")
            __ASM_EMIT("vinsertf128     $1, 0x070(%[src]), %%ymm3, %%ymm3")
            __ASM_EMIT("vshufps         $0x88, %%ymm1, %%ymm0, %%ymm0")
            __ASM_EMIT("vshufps         $0x88, %%ymm3, %%ymm2, %%ymm2")
            __ASM_EMIT("vmovups         %%xmm0, 0x000(%[dst])")
            __ASM_EMIT("vmovups         %%xmm2, 0x010(%[dst])")
            __ASM_EMIT("vextractf128    $1, %%ymm0, 0x020(%[dst])")
            __ASM_EMIT("vextractf128    $1, %%ymm2, 0x030(%[dst])")
            __ASM_EMIT("sub             $16, %[count]")
            __ASM_EMIT("add             $0x80, %[src]")
            __ASM_EMIT("add             $0x40, %[dst]")
            __ASM_EMIT("4:")
            // 8x block
            __ASM_EMIT("add             $8, %[count]")
            __ASM_EMIT("jl              6f")
            __ASM_EMIT("vmovups         0x000(%[src]), %%xmm0")                 // xmm0 = s0 ? s1 ?
            __ASM_EMIT("vmovups         0x010(%[src]), %%xmm1")                 // xmm1 = s2 ? s3 ?
            __ASM_EMIT("vmovups         0x020(%[src]), %%xmm2")                 // xmm2 = s4 ? s5 ?
            __ASM_EMIT("vmovups         0x030(%[src]), %%xmm3")                 // xmm3 = s6 ? s7 ?
            __ASM_EMIT("vshufps         $0x88, %%xmm1, %%xmm0, %%xmm0")
            __ASM_EMIT("vshufps         $0x88, %%xmm3, %%xmm2, %%xmm2")
            __ASM_EMIT("vmovups         %%xmm0, 0x000(%[dst])")
            __ASM_EMIT("vmovups         %%xmm2, 0x010(%[dst])")
            __ASM_EMIT("sub             $8, %[count]")
            __ASM_EMIT("add             $0x40, %[src]")
            __ASM_EMIT("add             $0x20, %[dst]")
            __ASM_EMIT("6:")
            // 4x block
            __ASM_EMIT("add             $4, %[count]")
            __ASM_EMIT("jl              8f")
            __ASM_EMIT("vmovups         0x000(%[src]), %%xmm0")                 // xmm0 = s0 ? s1 ?
            __ASM_EMIT("vmovups         0x010(%[src]), %%xmm1")                 // xmm1 = s2 ? s3 ?
            __ASM_EMIT("vshufps         $0x88, %%xmm1, %%xmm0, %%xmm0")
            __ASM_EMIT("vmovups         %%xmm0, 0x000(%[dst])")
            __ASM_EMIT("sub             $4, %[count]")
            __ASM_EMIT("add             $0x20, %[src]")
            __ASM_EMIT("add             $0x10, %[dst]")
            __ASM_EMIT("8:")
            // 2x block
            __ASM_EMIT("add             $2, %[count]")
            __ASM_EMIT("jl              10f")
            __ASM_EMIT("vmovups         0x000(%[src]), %%xmm0")                 // xmm0 = s0 ? s1 ?
            __ASM_EMIT("vshufps         $0x88, %%xmm0, %%xmm0, %%xmm0")         // xmm0 = s0 s1 s0 s1
            __ASM_EMIT("vmovlps         %%xmm0, 0x000(%[dst])")
            __ASM_EMIT("sub             $2, %[count]")
            __ASM_EMIT("add             $0x10, %[src]")
            __ASM_EMIT("add             $0x08, %[dst]")
            __ASM_EMIT("10:")
            // 1x block
            __ASM_EMIT("add             $1, %[count]")
            __ASM_EMIT("jl              12f")
            __ASM_EMIT("vmovss          0x000(%[src]), %%xmm0")                 // xmm0 = s0
            __ASM_EMIT("vmovss          %%xmm0, 0x000(%[dst])")
            __ASM_EMIT("12:")

            : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
            :
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void downsample_3x(float *dst, const float *src, size_t count)
    {
        ARCH_X86_ASM(
            // 32x blocks
            __ASM_EMIT("sub             $32, %[count]")
            __ASM_EMIT("jb              2f")
            __ASM_EMIT(".align 16")
            __ASM_EMIT("1:")
            __ASM_EMIT("vmovups         0x000(%[src]), %%xmm0")                 // xmm0 = s0 ? ? s1
            __ASM_EMIT("vmovups         0x018(%[src]), %%xmm1")                 // xmm1 = s2 ? ? s3
            __ASM_EMIT("vmovups         0x030(%[src]), %%xmm2")                 // xmm2 = s4 ? ? s5
            __ASM_EMIT("vmovups         0x048(%[src]), %%xmm3")                 // xmm3 = s6 ? ? s7
            __ASM_EMIT("vmovups         0x060(%[src]), %%xmm4")                 // xmm4 = s8 ? ? s9
            __ASM_EMIT("vmovups         0x078(%[src]), %%xmm5")                 // xmm5 = s10 ? ? s11
            __ASM_EMIT("vmovups         0x090(%[src]), %%xmm6")                 // xmm6 = s12 ? ? s13
            __ASM_EMIT("vmovups         0x0a8(%[src]), %%xmm7")                 // xmm7 = s14 ? ? s15
            __ASM_EMIT("vinsertf128     $1, 0x0c0(%[src]), %%ymm0, %%ymm0")
            __ASM_EMIT("vinsertf128     $1, 0x0d8(%[src]), %%ymm1, %%ymm1")
            __ASM_EMIT("vinsertf128     $1, 0x0f0(%[src]), %%ymm2, %%ymm2")
            __ASM_EMIT("vinsertf128     $1, 0x108(%[src]), %%ymm3, %%ymm3")
            __ASM_EMIT("vinsertf128     $1, 0x120(%[src]), %%ymm4, %%ymm4")
            __ASM_EMIT("vinsertf128     $1, 0x138(%[src]), %%ymm5, %%ymm5")
            __ASM_EMIT("vinsertf128     $1, 0x150(%[src]), %%ymm6, %%ymm6")
            __ASM_EMIT("vinsertf128     $1, 0x168(%[src]), %%ymm7, %%ymm7")
            __ASM_EMIT("vshufps         $0xcc, %%ymm1, %%ymm0, %%ymm0")         // ymm0 = s0 s1 s2 s3 s17 s17 s18 s19
            __ASM_EMIT("vshufps         $0xcc, %%ymm3, %%ymm2, %%ymm2")         // ymm2 = s4 s5 s6 s7 s20 s21 s22 s23
            __ASM_EMIT("vshufps         $0xcc, %%ymm5, %%ymm4, %%ymm4")         // ymm4 = s8 s9 s10 s11 s24 s25 s26 s27
            __ASM_EMIT("vshufps         $0xcc, %%ymm7, %%ymm6, %%ymm6")         // ymm6 = s12 s13 s14 s15 s28 s29 s30 s31
            __ASM_EMIT("vmovups         %%xmm0, 0x000(%[dst])")
            __ASM_EMIT("vmovups         %%xmm2, 0x010(%[dst])")
            __ASM_EMIT("vmovups         %%xmm4, 0x020(%[dst])")
            __ASM_EMIT("vmovups         %%xmm6, 0x030(%[dst])")
            __ASM_EMIT("vextractf128    $1, %%ymm0, 0x040(%[dst])")
            __ASM_EMIT("vextractf128    $1, %%ymm2, 0x050(%[dst])")
            __ASM_EMIT("vextractf128    $1, %%ymm4, 0x060(%[dst])")
            __ASM_EMIT("vextractf128    $1, %%ymm6, 0x070(%[dst])")
            __ASM_EMIT("add             $0x180, %[src]")
            __ASM_EMIT("add             $0x80, %[dst]")
            __ASM_EMIT("sub             $32, %[count]")
            __ASM_EMIT("jae             1b")
            __ASM_EMIT("2:")
            // 16x block
            __ASM_EMIT("add             $16, %[count]")
            __ASM_EMIT("jl              4f")
            __ASM_EMIT("vmovups         0x000(%[src]), %%xmm0")                 // xmm0 = s0 ? ? s1
            __ASM_EMIT("vmovups         0x018(%[src]), %%xmm1")                 // xmm1 = s2 ? ? s3
            __ASM_EMIT("vmovups         0x030(%[src]), %%xmm2")                 // xmm2 = s4 ? ? s5
            __ASM_EMIT("vmovups         0x048(%[src]), %%xmm3")                 // xmm3 = s6 ? ? s7
            __ASM_EMIT("vinsertf128     $1, 0x060(%[src]), %%ymm0, %%ymm0")
            __ASM_EMIT("vinsertf128     $1, 0x078(%[src]), %%ymm1, %%ymm1")
            __ASM_EMIT("vinsertf128     $1, 0x090(%[src]), %%ymm2, %%ymm2")
            __ASM_EMIT("vinsertf128     $1, 0x0a8(%[src]), %%ymm3, %%ymm3")
            __ASM_EMIT("vshufps         $0xcc, %%ymm1, %%ymm0, %%ymm0")
            __ASM_EMIT("vshufps         $0xcc, %%ymm3, %%ymm2, %%ymm2")
            __ASM_EMIT("vmovups         %%xmm0, 0x000(%[dst])")
            __ASM_EMIT("vmovups         %%xmm2, 0x010(%[dst])")
            __ASM_EMIT("vextractf128    $1, %%ymm0, 0x020(%[dst])")
            __ASM_EMIT("vextractf128    $1, %%ymm2, 0x030(%[dst])")
            __ASM_EMIT("sub             $16, %[count]")
            __ASM_EMIT("add             $0xc0, %[src]")
            __ASM_EMIT("add             $0x40, %[dst]")
            __ASM_EMIT("4:")
            // 8x block
            __ASM_EMIT("add             $8, %[count]")
            __ASM_EMIT("jl              6f")
            __ASM_EMIT("vmovups         0x000(%[src]), %%xmm0")                 // xmm0 = s0 ? ? s1
            __ASM_EMIT("vmovups         0x018(%[src]), %%xmm1")                 // xmm1 = s2 ? ? s3
            __ASM_EMIT("vmovups         0x030(%[src]), %%xmm2")                 // xmm2 = s4 ? ? s5
            __ASM_EMIT("vmovups         0x048(%[src]), %%xmm3")                 // xmm3 = s6 ? ? s7
            __ASM_EMIT("vshufps         $0xcc, %%xmm1, %%xmm0, %%xmm0")
            __ASM_EMIT("vshufps         $0xcc, %%xmm3, %%xmm2, %%xmm2")
            __ASM_EMIT("vmovups         %%xmm0, 0x000(%[dst])")
            __ASM_EMIT("vmovups         %%xmm2, 0x010(%[dst])")
            __ASM_EMIT("sub             $8, %[count]")
            __ASM_EMIT("add             $0x60, %[src]")
            __ASM_EMIT("add             $0x20, %[dst]")
            __ASM_EMIT("6:")
            // 4x block
            __ASM_EMIT("add             $4, %[count]")
            __ASM_EMIT("jl              8f")
            __ASM_EMIT("vmovups         0x000(%[src]), %%xmm0")                 // xmm0 = s0 ? ? s1
            __ASM_EMIT("vmovups         0x018(%[src]), %%xmm1")                 // xmm1 = s2 ? ? s3
            __ASM_EMIT("vshufps         $0xcc, %%xmm1, %%xmm0, %%xmm0")
            __ASM_EMIT("vmovups         %%xmm0, 0x000(%[dst])")
            __ASM_EMIT("sub             $4, %[count]")
            __ASM_EMIT("add             $0x30, %[src]")
            __ASM_EMIT("add             $0x10, %[dst]")
            __ASM_EMIT("8:")
            // 2x block
            __ASM_EMIT("add             $2, %[count]")
            __ASM_EMIT("jl              10f")
            __ASM_EMIT("vmovups         0x000(%[src]), %%xmm0")                 // xmm0 = s0 ? ? s1
            __ASM_EMIT("vshufps         $0xcc, %%xmm0, %%xmm0, %%xmm0")         // xmm0 = s0 s1 s0 s1
            __ASM_EMIT("vmovlps         %%xmm0, 0x000(%[dst])")
            __ASM_EMIT("sub             $2, %[count]")
            __ASM_EMIT("add             $0x18, %[src]")
            __ASM_EMIT("add             $0x08, %[dst]")
            __ASM_EMIT("10:")
            // 1x block
            __ASM_EMIT("add             $1, %[count]")
            __ASM_EMIT("jl              12f")
            __ASM_EMIT("vmovss          0x000(%[src]), %%xmm0")                 // xmm0 = s0
            __ASM_EMIT("vmovss          %%xmm0, 0x000(%[dst])")
            __ASM_EMIT("12:")

            : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
            :
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void downsample_4x(float *dst, const float *src, size_t count)
    {
        ARCH_X86_ASM(
            // 16x blocks
            __ASM_EMIT("sub             $16, %[count]")
            __ASM_EMIT("jb              2f")
            __ASM_EMIT(".align 16")
            __ASM_EMIT("1:")
            __ASM_EMIT("vmovss          0x000(%[src]), %%xmm4")                 // xmm4 = s0 ? ? ?
            __ASM_EMIT("vmovss          0x010(%[src]), %%xmm5")                 // xmm5 = s1 ? ? ?
            __ASM_EMIT("vmovss          0x020(%[src]), %%xmm6")                 // xmm6 = s2 ? ? ?
            __ASM_EMIT("vunpcklps       %%xmm5, %%xmm4, %%xmm0")                // xmm0 = s0 s1 ? ?
            __ASM_EMIT("vmovss          0x030(%[src]), %%xmm7")                 // xmm7 = s3 ? ? ?
            __ASM_EMIT("vunpcklps       %%xmm7, %%xmm6, %%xmm1")                // xmm1 = s2 s3 ? ?
            __ASM_EMIT("vmovss          0x040(%[src]), %%xmm4")                 // xmm4 = s4 ? ? ?
            __ASM_EMIT("vmovss          0x050(%[src]), %%xmm5")                 // xmm5 = s5 ? ? ?
            __ASM_EMIT("vmovss          0x060(%[src]), %%xmm6")                 // xmm6 = s6 ? ? ?
            __ASM_EMIT("vunpcklps       %%xmm5, %%xmm4, %%xmm2")                // xmm2 = s4 s5 ? ?
            __ASM_EMIT("vmovss          0x070(%[src]), %%xmm7")                 // xmm7 = s7 ? ? ?
            __ASM_EMIT("vunpcklps       %%xmm7, %%xmm6, %%xmm3")                // xmm3 = s6 s7 ? ?
            __ASM_EMIT("vmovss          0x080(%[src]), %%xmm4")                 // xmm4 = s8 ? ? ?
            __ASM_EMIT("vmovlhps        %%xmm1, %%xmm0, %%xmm0")                // xmm0 = s0 s1 s2 s3
            __ASM_EMIT("vmovss          0x090(%[src]), %%xmm5")                 // xmm5 = s9 ? ? ?
            __ASM_EMIT("vmovlhps        %%xmm3, %%xmm2, %%xmm2")                // xmm2 = s4 s5 s6 s7
            __ASM_EMIT("vmovss          0x0a0(%[src]), %%xmm6")                 // xmm6 = s10 ? ? ?
            __ASM_EMIT("vunpcklps       %%xmm5, %%xmm4, %%xmm1")                // xmm1 = s8 s9 ? ?
            __ASM_EMIT("vmovss          0x0b0(%[src]), %%xmm7")                 // xmm7 = s11 ? ? ?
            __ASM_EMIT("vunpcklps       %%xmm7, %%xmm6, %%xmm3")                // xmm3 = s10 s11 ? ?
            __ASM_EMIT("vmovss          0x0c0(%[src]), %%xmm4")                 // xmm4 = s12 ? ? ?
            __ASM_EMIT("vmovss          0x0d0(%[src]), %%xmm5")                 // xmm5 = s13 ? ? ?
            __ASM_EMIT("vmovss          0x0e0(%[src]), %%xmm6")                 // xmm6 = s14 ? ? ?
            __ASM_EMIT("vunpcklps       %%xmm5, %%xmm4, %%xmm4")                // xmm4 = s12 s13 ? ?
            __ASM_EMIT("vmovss          0x0f0(%[src]), %%xmm7")                 // xmm7 = s15 ? ? ?
            __ASM_EMIT("vunpcklps       %%xmm7, %%xmm6, %%xmm6")                // xmm6 = s14 s15 ? ?
            __ASM_EMIT("vmovups         %%xmm0, 0x000(%[dst])")
            __ASM_EMIT("vmovlhps        %%xmm3, %%xmm1, %%xmm1")                // xmm3 = s8 s9 s10 s11
            __ASM_EMIT("vmovups         %%xmm2, 0x010(%[dst])")
            __ASM_EMIT("vmovlhps        %%xmm6, %%xmm4, %%xmm4")                // xmm4 = s12 s13 s14 s15
            __ASM_EMIT("vmovups         %%xmm1, 0x020(%[dst])")
            __ASM_EMIT("vmovups         %%xmm4, 0x030(%[dst])")
            __ASM_EMIT("add             $0x100, %[src]")
            __ASM_EMIT("add             $0x40, %[dst]")
            __ASM_EMIT("sub             $16, %[count]")
            __ASM_EMIT("jae             1b")
            __ASM_EMIT("2:")
            // 8x block
            __ASM_EMIT("add             $8, %[count]")
            __ASM_EMIT("jl              4f")
            __ASM_EMIT("vmovss          0x000(%[src]), %%xmm4")                 // xmm4 = s0 ? ? ?
            __ASM_EMIT("vmovss          0x010(%[src]), %%xmm5")                 // xmm5 = s1 ? ? ?
            __ASM_EMIT("vmovss          0x020(%[src]), %%xmm6")                 // xmm6 = s2 ? ? ?
            __ASM_EMIT("vunpcklps       %%xmm5, %%xmm4, %%xmm0")                // xmm0 = s0 s1 ? ?
            __ASM_EMIT("vmovss          0x030(%[src]), %%xmm7")                 // xmm7 = s3 ? ? ?
            __ASM_EMIT("vunpcklps       %%xmm7, %%xmm6, %%xmm1")                // xmm1 = s2 s3 ? ?
            __ASM_EMIT("vmovss          0x040(%[src]), %%xmm4")                 // xmm4 = s4 ? ? ?
            __ASM_EMIT("vmovss          0x050(%[src]), %%xmm5")                 // xmm5 = s5 ? ? ?
            __ASM_EMIT("vmovss          0x060(%[src]), %%xmm6")                 // xmm6 = s6 ? ? ?
            __ASM_EMIT("vunpcklps       %%xmm5, %%xmm4, %%xmm2")                // xmm2 = s4 s5 ? ?
            __ASM_EMIT("vmovss          0x070(%[src]), %%xmm7")                 // xmm7 = s7 ? ? ?
            __ASM_EMIT("vunpcklps       %%xmm7, %%xmm6, %%xmm3")                // xmm3 = s6 s7 ? ?
            __ASM_EMIT("vmovlhps        %%xmm1, %%xmm0, %%xmm0")                // xmm0 = s0 s1 s2 s3
            __ASM_EMIT("vmovlhps        %%xmm3, %%xmm2, %%xmm2")                // xmm2 = s4 s5 s6 s7
            __ASM_EMIT("vmovups         %%xmm0, 0x000(%[dst])")
            __ASM_EMIT("vmovups         %%xmm2, 0x010(%[dst])")
            __ASM_EMIT("sub             $8, %[count]")
            __ASM_EMIT("add             $0x80, %[src]")
            __ASM_EMIT("add             $0x20, %[dst]")
            __ASM_EMIT("4:")
            // 4x block
            __ASM_EMIT("add             $4, %[count]")
            __ASM_EMIT("jl              6f")
            __ASM_EMIT("vmovss          0x000(%[src]), %%xmm0")                 // xmm0 = s0 ? ? ?
            __ASM_EMIT("vmovss          0x010(%[src]), %%xmm1")                 // xmm1 = s1 ? ? ?
            __ASM_EMIT("vinsertps       $0x10, 0x020(%[src]), %%xmm0, %%xmm0")  // xmm0 = s0 s2 ? ?
            __ASM_EMIT("vinsertps       $0x10, 0x030(%[src]), %%xmm1, %%xmm1")  // xmm1 = s1 s3 ? ?
            __ASM_EMIT("vunpcklps       %%xmm1, %%xmm0, %%xmm0")                // xmm0 = s0 s1 s2 s3
            __ASM_EMIT("vmovups         %%xmm0, 0x000(%[dst])")
            __ASM_EMIT("sub             $4, %[count]")
            __ASM_EMIT("add             $0x40, %[src]")
            __ASM_EMIT("add             $0x10, %[dst]")
            __ASM_EMIT("6:")
            // 2x block
            __ASM_EMIT("add             $2, %[count]")
            __ASM_EMIT("jl              8f")
            __ASM_EMIT("vmovss          0x000(%[src]), %%xmm0")                 // xmm0 = s0 ? ? ?
            __ASM_EMIT("vinsertps       $0x10, 0x010(%[src]), %%xmm0, %%xmm0")  // xmm0 = s0 s1 ? ?
            __ASM_EMIT("vmovlps         %%xmm0, 0x000(%[dst])")
            __ASM_EMIT("sub             $2, %[count]")
            __ASM_EMIT("add             $0x20, %[src]")
            __ASM_EMIT("add             $0x08, %[dst]")
            __ASM_EMIT("8:")
            // 1x block
            __ASM_EMIT("add             $1, %[count]")
            __ASM_EMIT("jl              12f")
            __ASM_EMIT("vmovss          0x000(%[src]), %%xmm0")                 // xmm0 = s0
            __ASM_EMIT("vmovss          %%xmm0, 0x000(%[dst])")
            __ASM_EMIT("12:")

            : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
            :
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void downsample_6x(float *dst, const float *src, size_t count)
    {
        ARCH_X86_ASM(
            // 16x blocks
            __ASM_EMIT("sub             $16, %[count]")
            __ASM_EMIT("jb              2f")
            __ASM_EMIT(".align 16")
            __ASM_EMIT("1:")
            __ASM_EMIT("vmovss          0x000(%[src]), %%xmm4")                 // xmm4 = s0 ? ? ?
            __ASM_EMIT("vmovss          0x018(%[src]), %%xmm5")                 // xmm5 = s1 ? ? ?
            __ASM_EMIT("vmovss          0x030(%[src]), %%xmm6")                 // xmm6 = s2 ? ? ?
            __ASM_EMIT("vunpcklps       %%xmm5, %%xmm4, %%xmm0")                // xmm0 = s0 s1 ? ?
            __ASM_EMIT("vmovss          0x048(%[src]), %%xmm7")                 // xmm7 = s3 ? ? ?
            __ASM_EMIT("vunpcklps       %%xmm7, %%xmm6, %%xmm1")                // xmm1 = s2 s3 ? ?
            __ASM_EMIT("vmovss          0x060(%[src]), %%xmm4")                 // xmm4 = s4 ? ? ?
            __ASM_EMIT("vmovss          0x078(%[src]), %%xmm5")                 // xmm5 = s5 ? ? ?
            __ASM_EMIT("vmovss          0x090(%[src]), %%xmm6")                 // xmm6 = s6 ? ? ?
            __ASM_EMIT("vunpcklps       %%xmm5, %%xmm4, %%xmm2")                // xmm2 = s4 s5 ? ?
            __ASM_EMIT("vmovss          0x0a8(%[src]), %%xmm7")                 // xmm7 = s7 ? ? ?
            __ASM_EMIT("vunpcklps       %%xmm7, %%xmm6, %%xmm3")                // xmm3 = s6 s7 ? ?
            __ASM_EMIT("vmovss          0x0c0(%[src]), %%xmm4")                 // xmm4 = s8 ? ? ?
            __ASM_EMIT("vmovlhps        %%xmm1, %%xmm0, %%xmm0")                // xmm0 = s0 s1 s2 s3
            __ASM_EMIT("vmovss          0x0d8(%[src]), %%xmm5")                 // xmm5 = s9 ? ? ?
            __ASM_EMIT("vmovlhps        %%xmm3, %%xmm2, %%xmm2")                // xmm2 = s4 s5 s6 s7
            __ASM_EMIT("vmovss          0x0f0(%[src]), %%xmm6")                 // xmm6 = s10 ? ? ?
            __ASM_EMIT("vunpcklps       %%xmm5, %%xmm4, %%xmm1")                // xmm1 = s8 s9 ? ?
            __ASM_EMIT("vmovss          0x108(%[src]), %%xmm7")                 // xmm7 = s11 ? ? ?
            __ASM_EMIT("vunpcklps       %%xmm7, %%xmm6, %%xmm3")                // xmm3 = s10 s11 ? ?
            __ASM_EMIT("vmovss          0x120(%[src]), %%xmm4")                 // xmm4 = s12 ? ? ?
            __ASM_EMIT("vmovss          0x138(%[src]), %%xmm5")                 // xmm5 = s13 ? ? ?
            __ASM_EMIT("vmovss          0x150(%[src]), %%xmm6")                 // xmm6 = s14 ? ? ?
            __ASM_EMIT("vunpcklps       %%xmm5, %%xmm4, %%xmm4")                // xmm4 = s12 s13 ? ?
            __ASM_EMIT("vmovss          0x168(%[src]), %%xmm7")                 // xmm7 = s15 ? ? ?
            __ASM_EMIT("vunpcklps       %%xmm7, %%xmm6, %%xmm6")                // xmm6 = s14 s15 ? ?
            __ASM_EMIT("vmovups         %%xmm0, 0x000(%[dst])")
            __ASM_EMIT("vmovlhps        %%xmm3, %%xmm1, %%xmm1")                // xmm3 = s8 s9 s10 s11
            __ASM_EMIT("vmovups         %%xmm2, 0x010(%[dst])")
            __ASM_EMIT("vmovlhps        %%xmm6, %%xmm4, %%xmm4")                // xmm4 = s12 s13 s14 s15
            __ASM_EMIT("vmovups         %%xmm1, 0x020(%[dst])")
            __ASM_EMIT("vmovups         %%xmm4, 0x030(%[dst])")
            __ASM_EMIT("add             $0x180, %[src]")
            __ASM_EMIT("add             $0x40, %[dst]")
            __ASM_EMIT("sub             $16, %[count]")
            __ASM_EMIT("jae             1b")
            __ASM_EMIT("2:")
            // 8x block
            __ASM_EMIT("add             $8, %[count]")
            __ASM_EMIT("jl              4f")
            __ASM_EMIT("vmovss          0x000(%[src]), %%xmm4")                 // xmm4 = s0 ? ? ?
            __ASM_EMIT("vmovss          0x018(%[src]), %%xmm5")                 // xmm5 = s1 ? ? ?
            __ASM_EMIT("vmovss          0x030(%[src]), %%xmm6")                 // xmm6 = s2 ? ? ?
            __ASM_EMIT("vunpcklps       %%xmm5, %%xmm4, %%xmm0")                // xmm0 = s0 s1 ? ?
            __ASM_EMIT("vmovss          0x048(%[src]), %%xmm7")                 // xmm7 = s3 ? ? ?
            __ASM_EMIT("vunpcklps       %%xmm7, %%xmm6, %%xmm1")                // xmm1 = s2 s3 ? ?
            __ASM_EMIT("vmovss          0x060(%[src]), %%xmm4")                 // xmm4 = s4 ? ? ?
            __ASM_EMIT("vmovss          0x078(%[src]), %%xmm5")                 // xmm5 = s5 ? ? ?
            __ASM_EMIT("vmovss          0x090(%[src]), %%xmm6")                 // xmm6 = s6 ? ? ?
            __ASM_EMIT("vunpcklps       %%xmm5, %%xmm4, %%xmm2")                // xmm2 = s4 s5 ? ?
            __ASM_EMIT("vmovss          0x0a8(%[src]), %%xmm7")                 // xmm7 = s7 ? ? ?
            __ASM_EMIT("vunpcklps       %%xmm7, %%xmm6, %%xmm3")                // xmm3 = s6 s7 ? ?
            __ASM_EMIT("vmovlhps        %%xmm1, %%xmm0, %%xmm0")                // xmm0 = s0 s1 s2 s3
            __ASM_EMIT("vmovlhps        %%xmm3, %%xmm2, %%xmm2")                // xmm2 = s4 s5 s6 s7
            __ASM_EMIT("vmovups         %%xmm0, 0x000(%[dst])")
            __ASM_EMIT("vmovups         %%xmm2, 0x010(%[dst])")
            __ASM_EMIT("sub             $8, %[count]")
            __ASM_EMIT("add             $0xc0, %[src]")
            __ASM_EMIT("add             $0x20, %[dst]")
            __ASM_EMIT("4:")
            // 4x block
            __ASM_EMIT("add             $4, %[count]")
            __ASM_EMIT("jl              6f")
            __ASM_EMIT("vmovss          0x000(%[src]), %%xmm0")                 // xmm0 = s0 ? ? ?
            __ASM_EMIT("vmovss          0x018(%[src]), %%xmm1")                 // xmm1 = s1 ? ? ?
            __ASM_EMIT("vinsertps       $0x10, 0x030(%[src]), %%xmm0, %%xmm0")  // xmm0 = s0 s2 ? ?
            __ASM_EMIT("vinsertps       $0x10, 0x048(%[src]), %%xmm1, %%xmm1")  // xmm1 = s1 s3 ? ?
            __ASM_EMIT("vunpcklps       %%xmm1, %%xmm0, %%xmm0")                // xmm0 = s0 s1 s2 s3
            __ASM_EMIT("vmovups         %%xmm0, 0x000(%[dst])")
            __ASM_EMIT("sub             $4, %[count]")
            __ASM_EMIT("add             $0x60, %[src]")
            __ASM_EMIT("add             $0x10, %[dst]")
            __ASM_EMIT("6:")
            // 2x block
            __ASM_EMIT("add             $2, %[count]")
            __ASM_EMIT("jl              8f")
            __ASM_EMIT("vmovss          0x000(%[src]), %%xmm0")                 // xmm0 = s0 ? ? ?
            __ASM_EMIT("vinsertps       $0x10, 0x018(%[src]), %%xmm0, %%xmm0")  // xmm0 = s0 s1 ? ?
            __ASM_EMIT("vmovlps         %%xmm0, 0x000(%[dst])")
            __ASM_EMIT("sub             $2, %[count]")
            __ASM_EMIT("add             $0x30, %[src]")
            __ASM_EMIT("add             $0x08, %[dst]")
            __ASM_EMIT("8:")
            // 1x block
            __ASM_EMIT("add             $1, %[count]")
            __ASM_EMIT("jl              12f")
            __ASM_EMIT("vmovss          0x000(%[src]), %%xmm0")                 // xmm0 = s0
            __ASM_EMIT("vmovss          %%xmm0, 0x000(%[dst])")
            __ASM_EMIT("12:")

            : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
            :
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void downsample_8x(float *dst, const float *src, size_t count)
    {
        ARCH_X86_ASM(
            // 16x blocks
            __ASM_EMIT("sub             $16, %[count]")
            __ASM_EMIT("jb              2f")
            __ASM_EMIT(".align 16")
            __ASM_EMIT("1:")
            __ASM_EMIT("vmovss          0x000(%[src]), %%xmm4")                 // xmm4 = s0 ? ? ?
            __ASM_EMIT("vmovss          0x020(%[src]), %%xmm5")                 // xmm5 = s1 ? ? ?
            __ASM_EMIT("vmovss          0x040(%[src]), %%xmm6")                 // xmm6 = s2 ? ? ?
            __ASM_EMIT("vunpcklps       %%xmm5, %%xmm4, %%xmm0")                // xmm0 = s0 s1 ? ?
            __ASM_EMIT("vmovss          0x060(%[src]), %%xmm7")                 // xmm7 = s3 ? ? ?
            __ASM_EMIT("vunpcklps       %%xmm7, %%xmm6, %%xmm1")                // xmm1 = s2 s3 ? ?
            __ASM_EMIT("vmovss          0x080(%[src]), %%xmm4")                 // xmm4 = s4 ? ? ?
            __ASM_EMIT("vmovss          0x0a0(%[src]), %%xmm5")                 // xmm5 = s5 ? ? ?
            __ASM_EMIT("vmovss          0x0c0(%[src]), %%xmm6")                 // xmm6 = s6 ? ? ?
            __ASM_EMIT("vunpcklps       %%xmm5, %%xmm4, %%xmm2")                // xmm2 = s4 s5 ? ?
            __ASM_EMIT("vmovss          0x0e0(%[src]), %%xmm7")                 // xmm7 = s7 ? ? ?
            __ASM_EMIT("vunpcklps       %%xmm7, %%xmm6, %%xmm3")                // xmm3 = s6 s7 ? ?
            __ASM_EMIT("vmovss          0x100(%[src]), %%xmm4")                 // xmm4 = s8 ? ? ?
            __ASM_EMIT("vmovlhps        %%xmm1, %%xmm0, %%xmm0")                // xmm0 = s0 s1 s2 s3
            __ASM_EMIT("vmovss          0x120(%[src]), %%xmm5")                 // xmm5 = s9 ? ? ?
            __ASM_EMIT("vmovlhps        %%xmm3, %%xmm2, %%xmm2")                // xmm2 = s4 s5 s6 s7
            __ASM_EMIT("vmovss          0x140(%[src]), %%xmm6")                 // xmm6 = s10 ? ? ?
            __ASM_EMIT("vunpcklps       %%xmm5, %%xmm4, %%xmm1")                // xmm1 = s8 s9 ? ?
            __ASM_EMIT("vmovss          0x160(%[src]), %%xmm7")                 // xmm7 = s11 ? ? ?
            __ASM_EMIT("vunpcklps       %%xmm7, %%xmm6, %%xmm3")                // xmm3 = s10 s11 ? ?
            __ASM_EMIT("vmovss          0x180(%[src]), %%xmm4")                 // xmm4 = s12 ? ? ?
            __ASM_EMIT("vmovss          0x1a0(%[src]), %%xmm5")                 // xmm5 = s13 ? ? ?
            __ASM_EMIT("vmovss          0x1c0(%[src]), %%xmm6")                 // xmm6 = s14 ? ? ?
            __ASM_EMIT("vunpcklps       %%xmm5, %%xmm4, %%xmm4")                // xmm4 = s12 s13 ? ?
            __ASM_EMIT("vmovss          0x1e0(%[src]), %%xmm7")                 // xmm7 = s15 ? ? ?
            __ASM_EMIT("vunpcklps       %%xmm7, %%xmm6, %%xmm6")                // xmm6 = s14 s15 ? ?
            __ASM_EMIT("vmovups         %%xmm0, 0x000(%[dst])")
            __ASM_EMIT("vmovlhps        %%xmm3, %%xmm1, %%xmm1")                // xmm3 = s8 s9 s10 s11
            __ASM_EMIT("vmovups         %%xmm2, 0x010(%[dst])")
            __ASM_EMIT("vmovlhps        %%xmm6, %%xmm4, %%xmm4")                // xmm4 = s12 s13 s14 s15
            __ASM_EMIT("vmovups         %%xmm1, 0x020(%[dst])")
            __ASM_EMIT("vmovups         %%xmm4, 0x030(%[dst])")
            __ASM_EMIT("add             $0x200, %[src]")
            __ASM_EMIT("add             $0x40, %[dst]")
            __ASM_EMIT("sub             $16, %[count]")
            __ASM_EMIT("jae             1b")
            __ASM_EMIT("2:")
            // 8x block
            __ASM_EMIT("add             $8, %[count]")
            __ASM_EMIT("jl              4f")
            __ASM_EMIT("vmovss          0x000(%[src]), %%xmm4")                 // xmm4 = s0 ? ? ?
            __ASM_EMIT("vmovss          0x020(%[src]), %%xmm5")                 // xmm5 = s1 ? ? ?
            __ASM_EMIT("vmovss          0x040(%[src]), %%xmm6")                 // xmm6 = s2 ? ? ?
            __ASM_EMIT("vunpcklps       %%xmm5, %%xmm4, %%xmm0")                // xmm0 = s0 s1 ? ?
            __ASM_EMIT("vmovss          0x060(%[src]), %%xmm7")                 // xmm7 = s3 ? ? ?
            __ASM_EMIT("vunpcklps       %%xmm7, %%xmm6, %%xmm1")                // xmm1 = s2 s3 ? ?
            __ASM_EMIT("vmovss          0x080(%[src]), %%xmm4")                 // xmm4 = s4 ? ? ?
            __ASM_EMIT("vmovss          0x0a0(%[src]), %%xmm5")                 // xmm5 = s5 ? ? ?
            __ASM_EMIT("vmovss          0x0c0(%[src]), %%xmm6")                 // xmm6 = s6 ? ? ?
            __ASM_EMIT("vunpcklps       %%xmm5, %%xmm4, %%xmm2")                // xmm2 = s4 s5 ? ?
            __ASM_EMIT("vmovss          0x0e0(%[src]), %%xmm7")                 // xmm7 = s7 ? ? ?
            __ASM_EMIT("vunpcklps       %%xmm7, %%xmm6, %%xmm3")                // xmm3 = s6 s7 ? ?
            __ASM_EMIT("vmovlhps        %%xmm1, %%xmm0, %%xmm0")                // xmm0 = s0 s1 s2 s3
            __ASM_EMIT("vmovlhps        %%xmm3, %%xmm2, %%xmm2")                // xmm2 = s4 s5 s6 s7
            __ASM_EMIT("vmovups         %%xmm0, 0x000(%[dst])")
            __ASM_EMIT("vmovups         %%xmm2, 0x010(%[dst])")
            __ASM_EMIT("sub             $8, %[count]")
            __ASM_EMIT("add             $0x100, %[src]")
            __ASM_EMIT("add             $0x20, %[dst]")
            __ASM_EMIT("4:")
            // 4x block
            __ASM_EMIT("add             $4, %[count]")
            __ASM_EMIT("jl              6f")
            __ASM_EMIT("vmovss          0x000(%[src]), %%xmm0")                 // xmm0 = s0 ? ? ?
            __ASM_EMIT("vmovss          0x020(%[src]), %%xmm1")                 // xmm1 = s1 ? ? ?
            __ASM_EMIT("vinsertps       $0x10, 0x040(%[src]), %%xmm0, %%xmm0")  // xmm0 = s0 s2 ? ?
            __ASM_EMIT("vinsertps       $0x10, 0x060(%[src]), %%xmm1, %%xmm1")  // xmm1 = s1 s3 ? ?
            __ASM_EMIT("vunpcklps       %%xmm1, %%xmm0, %%xmm0")                // xmm0 = s0 s1 s2 s3
            __ASM_EMIT("vmovups         %%xmm0, 0x000(%[dst])")
            __ASM_EMIT("sub             $4, %[count]")
            __ASM_EMIT("add             $0x80, %[src]")
            __ASM_EMIT("add             $0x10, %[dst]")
            __ASM_EMIT("6:")
            // 2x block
            __ASM_EMIT("add             $2, %[count]")
            __ASM_EMIT("jl              8f")
            __ASM_EMIT("vmovss          0x000(%[src]), %%xmm0")                 // xmm0 = s0 ? ? ?
            __ASM_EMIT("vinsertps       $0x10, 0x020(%[src]), %%xmm0, %%xmm0")  // xmm0 = s0 s1 ? ?
            __ASM_EMIT("vmovlps         %%xmm0, 0x000(%[dst])")
            __ASM_EMIT("sub             $2, %[count]")
            __ASM_EMIT("add             $0x40, %[src]")
            __ASM_EMIT("add             $0x08, %[dst]")
            __ASM_EMIT("8:")
            // 1x block
            __ASM_EMIT("add             $1, %[count]")
            __ASM_EMIT("jl              12f")
            __ASM_EMIT("vmovss          0x000(%[src]), %%xmm0")                 // xmm0 = s0
            __ASM_EMIT("vmovss          %%xmm0, 0x000(%[dst])")
            __ASM_EMIT("12:")

            : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
            :
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }
}

#endif /* DSP_ARCH_X86_AVX_RESAMPLING_H_ */
