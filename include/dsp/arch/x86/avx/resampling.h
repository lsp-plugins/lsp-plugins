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
    static const float lanczos_2x2[] __lsp_aligned64 =
    {
        0.6203830132406946f, // k0
        -0.1664152316035080f // k1
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
}

#endif /* DSP_ARCH_X86_AVX_RESAMPLING_H_ */
