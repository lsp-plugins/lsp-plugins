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
            __ASM_EMIT("vaddss          0x10(%[dst]), %%xmm0, %%xmm2")  // xmm2 = d4 + s0
            __ASM_EMIT("vaddss          0x18(%[dst]), %%xmm1, %%xmm3")  // xmm3 = d6 + s1
            __ASM_EMIT("vmovss          %%xmm2, 0x10(%[dst])")          // d4  += s0
            __ASM_EMIT("vmovss          %%xmm3, 0x18(%[dst])")          // d6  += s1
            __ASM_EMIT("vmulss          %%xmm0, %%xmm7, %%xmm2")        // xmm2 = k1*s0
            __ASM_EMIT("vmulss          %%xmm1, %%xmm7, %%xmm3")        // xmm3 = k1*s1
            __ASM_EMIT("vmulss          %%xmm0, %%xmm6, %%xmm4")        // xmm4 = k0*s0
            __ASM_EMIT("vmulss          %%xmm1, %%xmm6, %%xmm5")        // xmm5 = k0*s1
            __ASM_EMIT("vaddss          0x04(%[dst]), %%xmm2, %%xmm0")  // xmm0 = d1 + k1*s0
            __ASM_EMIT("vaddss          0x24(%[dst]), %%xmm3, %%xmm1")  // xmm1 = d9 + k1*s1
            __ASM_EMIT("vmovss          %%xmm0, 0x04(%[dst])")          // d1  += k1*s0
            __ASM_EMIT("vmovss          %%xmm1, 0x24(%[dst])")          // d9  += k1*s1
            __ASM_EMIT("vaddss          %%xmm4, %%xmm3, %%xmm0")        // xmm0 = k0*s0 + k1*s1
            __ASM_EMIT("vaddss          %%xmm4, %%xmm5, %%xmm1")        // xmm1 = k0*s0 + k0*s1
            __ASM_EMIT("vaddss          %%xmm5, %%xmm2, %%xmm2")        // xmm2 = k1*s0 + k0*s1
            __ASM_EMIT("vaddss          0x0c(%[dst]), %%xmm0, %%xmm0")  // xmm0 = d3 + k0*s0 + k1*s1
            __ASM_EMIT("vaddss          0x14(%[dst]), %%xmm1, %%xmm1")  // xmm1 = d5 + k0*s0 + k0*s1
            __ASM_EMIT("vaddss          0x1c(%[dst]), %%xmm2, %%xmm2")  // xmm2 = d7 + k1*s0 + k0*s1
            __ASM_EMIT("vmovss          %%xmm0, 0x0c(%[dst])")          // d3  += k0*s0 + k1*s1
            __ASM_EMIT("vmovss          %%xmm1, 0x14(%[dst])")          // d5  += k0*s0 + k0*s1
            __ASM_EMIT("vmovss          %%xmm2, 0x1c(%[dst])")          // d7  += k1*s0 + k0*s1
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
            __ASM_EMIT("sub             $2, %[count]")
            __ASM_EMIT("jb              2f")
            __ASM_EMIT(".align          16")
            __ASM_EMIT("1:")
            __ASM_EMIT("vmovss          0x00(%[src]), %%xmm0")          // xmm0 = s0
            __ASM_EMIT("vmovss          0x04(%[src]), %%xmm1")          // xmm1 = s1
            __ASM_EMIT("vmulss          0x00 + %[k], %%xmm0, %%xmm6")   // xmm6 = k2*s0
            __ASM_EMIT("vmulss          0x00 + %[k], %%xmm1, %%xmm7")   // xmm7 = k2*s1
            __ASM_EMIT("vmulss          0x04 + %[k], %%xmm0, %%xmm2")   // xmm2 = k0*s0
            __ASM_EMIT("vmulss          0x04 + %[k], %%xmm1, %%xmm3")   // xmm3 = k0*s1
            __ASM_EMIT("vaddss          0x04(%[dst]), %%xmm6, %%xmm4")  // xmm4 = d1 + k2*s0
            __ASM_EMIT("vaddss          0x34(%[dst]), %%xmm7, %%xmm5")  // xmm5 = d13 + k2*s1
            __ASM_EMIT("vmovss          %%xmm4, 0x04(%[dst])")
            __ASM_EMIT("vmovss          %%xmm5, 0x34(%[dst])")
            __ASM_EMIT("vmulss          0x08 + %[k], %%xmm0, %%xmm4")   // xmm4 = k1*s0
            __ASM_EMIT("vmulss          0x08 + %[k], %%xmm1, %%xmm5")   // xmm5 = k1*s1
            __ASM_EMIT("vaddss          0x18(%[dst]), %%xmm0, %%xmm0")  // xmm0 = d6 + s0
            __ASM_EMIT("vaddss          0x20(%[dst]), %%xmm1, %%xmm1")  // xmm1 = d8 + s1
            __ASM_EMIT("vmovss          %%xmm0, 0x18(%[dst])")
            __ASM_EMIT("vmovss          %%xmm1, 0x20(%[dst])")
            __ASM_EMIT("vaddss          %%xmm7, %%xmm4, %%xmm0")        // xmm0 = k1*s0 + k2*s1
            __ASM_EMIT("vaddss          %%xmm6, %%xmm5, %%xmm1")        // xmm1 = k2*s0 + k1*s1
            __ASM_EMIT("vaddss          0x0c(%[dst]), %%xmm0, %%xmm0")  // xmm0 = d3 + k1*s0 + k2*s1
            __ASM_EMIT("vaddss          0x2c(%[dst]), %%xmm1, %%xmm1")  // xmm1 = d11 + k2*s0 + k1*s1
            __ASM_EMIT("vmovss          %%xmm0, 0x0c(%[dst])")
            __ASM_EMIT("vmovss          %%xmm1, 0x2c(%[dst])")
            __ASM_EMIT("vaddss          %%xmm2, %%xmm5, %%xmm0")        // xmm0 = k0*s0 + k1*s1
            __ASM_EMIT("vaddss          %%xmm2, %%xmm3, %%xmm1")        // xmm1 = k0*s0 + k0*s1
            __ASM_EMIT("vaddss          %%xmm3, %%xmm4, %%xmm2")        // xmm2 = k1*s0 + k0*s1
            __ASM_EMIT("vaddss          0x14(%[dst]), %%xmm0, %%xmm0")  // xmm0 = d5 + k0*s0 + k1*s1
            __ASM_EMIT("vaddss          0x1c(%[dst]), %%xmm1, %%xmm1")  // xmm1 = d7 + k0*s0 + k0*s1
            __ASM_EMIT("vaddss          0x24(%[dst]), %%xmm2, %%xmm2")  // xmm2 = d9 + k1*s0 + k0*s1
            __ASM_EMIT("vmovss          %%xmm0, 0x14(%[dst])")
            __ASM_EMIT("vmovss          %%xmm1, 0x1c(%[dst])")
            __ASM_EMIT("vmovss          %%xmm2, 0x24(%[dst])")
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
}

#endif /* DSP_ARCH_X86_AVX_RESAMPLING_H_ */
