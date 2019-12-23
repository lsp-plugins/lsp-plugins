/*
 * p_repack.h
 *
 *  Created on: 11 дек. 2019 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_AVX_FFT_P_REPACK_H_
#define DSP_ARCH_X86_AVX_FFT_P_REPACK_H_

#ifndef DSP_ARCH_X86_AVX_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_AVX_IMPL */

namespace avx
{
    static inline void packed_fft_repack(float *dst, size_t rank)
    {
        size_t blocks = 1 << rank;

        // Perform 4-element butterflies
        ARCH_X86_ASM
        (
            // 16x blocks
            __ASM_EMIT("sub             $16, %[blocks]")
            __ASM_EMIT("jb              2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vmovups         0x00(%[dst]), %%ymm0")      /* ymm0 = r0  r1  r2  r3  r4  r5  r6  r7  */
            __ASM_EMIT("vmovups         0x20(%[dst]), %%ymm1")      /* ymm1 = i0  i1  i2  i3  i4  i5  i6  i7  */
            __ASM_EMIT("vmovups         0x40(%[dst]), %%ymm2")      /* ymm2 = r8  r9  r10 r11 r12 r13 r14 r15 */
            __ASM_EMIT("vmovups         0x60(%[dst]), %%ymm3")      /* ymm3 = i8  i9  i10 i11 i12 i13 i14 i15 */
            __ASM_EMIT("vunpcklps       %%ymm1, %%ymm0, %%ymm4")    /* ymm4 = r0  i0  r1  i1  r4  i4  r5  i5  */
            __ASM_EMIT("vunpckhps       %%ymm1, %%ymm0, %%ymm5")    /* ymm5 = r2  i2  r3  i3  r6  i6  r7  i7  */
            __ASM_EMIT("vunpcklps       %%ymm3, %%ymm2, %%ymm0")    /* ymm0 = r8  i8  r9  i9  r12 i12 r13 i13 */
            __ASM_EMIT("vunpckhps       %%ymm3, %%ymm2, %%ymm1")    /* ymm1 = r10 i10 r12 i12 r14 i14 r15 i15 */
            __ASM_EMIT("vmovups         %%xmm4, 0x00(%[dst])")
            __ASM_EMIT("vmovups         %%xmm5, 0x10(%[dst])")
            __ASM_EMIT("vextractf128    $1, %%ymm4, 0x20(%[dst])")
            __ASM_EMIT("vextractf128    $1, %%ymm5, 0x30(%[dst])")
            __ASM_EMIT("vmovups         %%xmm0, 0x40(%[dst])")
            __ASM_EMIT("vmovups         %%xmm1, 0x50(%[dst])")
            __ASM_EMIT("vextractf128    $1, %%ymm0, 0x60(%[dst])")
            __ASM_EMIT("vextractf128    $1, %%ymm1, 0x70(%[dst])")
            __ASM_EMIT("add             $0x80, %[dst]")
            __ASM_EMIT("sub             $16, %[blocks]")
            __ASM_EMIT("jae             1b")
            __ASM_EMIT("2:")
            // 8x block
            __ASM_EMIT("add             $8, %[blocks]")
            __ASM_EMIT("jl              4f")
            __ASM_EMIT("vmovups         0x00(%[dst]), %%ymm0")      /* ymm0 = r0  r1  r2  r3  r4  r5  r6  r7  */
            __ASM_EMIT("vmovups         0x20(%[dst]), %%ymm1")      /* ymm1 = i0  i1  i2  i3  i4  i5  i6  i7  */
            __ASM_EMIT("vunpcklps       %%ymm1, %%ymm0, %%ymm4")    /* ymm4 = r0  i0  r1  i1  r4  i4  r5  i5  */
            __ASM_EMIT("vunpckhps       %%ymm1, %%ymm0, %%ymm5")    /* ymm5 = r2  i2  r3  i3  r6  i6  r7  i7  */
            __ASM_EMIT("vmovups         %%xmm4, 0x00(%[dst])")
            __ASM_EMIT("vmovups         %%xmm5, 0x10(%[dst])")
            __ASM_EMIT("vextractf128    $1, %%ymm4, 0x20(%[dst])")
            __ASM_EMIT("vextractf128    $1, %%ymm5, 0x30(%[dst])")
            __ASM_EMIT("4:")

            : [dst] "+r"(dst), [blocks] "+r" (blocks)
            :
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5"
        );
    }

    static inline void packed_fft_repack_normalize(float *dst, size_t rank)
    {
        size_t blocks = 1 << rank;
        float norm = 1.0f / float(blocks);

        // Perform 4-element butterflies
        ARCH_X86_ASM
        (
            __ASM_EMIT("vbroadcastss    %[norm], %%ymm6")
            __ASM_EMIT("vmovaps         %%ymm6, %%ymm7")
            // 16x blocks
            __ASM_EMIT("sub             $16, %[blocks]")
            __ASM_EMIT("jb              2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vmulps          0x00(%[dst]), %%ymm6, %%ymm0")  /* ymm0 = r0  r1  r2  r3  r4  r5  r6  r7  */
            __ASM_EMIT("vmulps          0x20(%[dst]), %%ymm7, %%ymm1")  /* ymm1 = i0  i1  i2  i3  i4  i5  i6  i7  */
            __ASM_EMIT("vmulps          0x40(%[dst]), %%ymm6, %%ymm2")  /* ymm2 = r8  r9  r10 r11 r12 r13 r14 r15 */
            __ASM_EMIT("vmulps          0x60(%[dst]), %%ymm7, %%ymm3")  /* ymm3 = i8  i9  i10 i11 i12 i13 i14 i15 */
            __ASM_EMIT("vunpcklps       %%ymm1, %%ymm0, %%ymm4")        /* ymm4 = r0  i0  r1  i1  r4  i4  r5  i5  */
            __ASM_EMIT("vunpckhps       %%ymm1, %%ymm0, %%ymm5")        /* ymm5 = r2  i2  r3  i3  r6  i6  r7  i7  */
            __ASM_EMIT("vunpcklps       %%ymm3, %%ymm2, %%ymm0")        /* ymm0 = r8  i8  r9  i9  r12 i12 r13 i13 */
            __ASM_EMIT("vunpckhps       %%ymm3, %%ymm2, %%ymm1")        /* ymm1 = r10 i10 r12 i12 r14 i14 r15 i15 */
            __ASM_EMIT("vmovups         %%xmm4, 0x00(%[dst])")
            __ASM_EMIT("vmovups         %%xmm5, 0x10(%[dst])")
            __ASM_EMIT("vextractf128    $1, %%ymm4, 0x20(%[dst])")
            __ASM_EMIT("vextractf128    $1, %%ymm5, 0x30(%[dst])")
            __ASM_EMIT("vmovups         %%xmm0, 0x40(%[dst])")
            __ASM_EMIT("vmovups         %%xmm1, 0x50(%[dst])")
            __ASM_EMIT("vextractf128    $1, %%ymm0, 0x60(%[dst])")
            __ASM_EMIT("vextractf128    $1, %%ymm1, 0x70(%[dst])")
            __ASM_EMIT("add             $0x80, %[dst]")
            __ASM_EMIT("sub             $16, %[blocks]")
            __ASM_EMIT("jae             1b")
            __ASM_EMIT("2:")
            // 8x block
            __ASM_EMIT("add             $8, %[blocks]")
            __ASM_EMIT("jl              4f")
            __ASM_EMIT("vmulps          0x00(%[dst]), %%ymm6, %%ymm0")  /* ymm0 = r0  r1  r2  r3  r4  r5  r6  r7  */
            __ASM_EMIT("vmulps          0x20(%[dst]), %%ymm7, %%ymm1")  /* ymm1 = i0  i1  i2  i3  i4  i5  i6  i7  */
            __ASM_EMIT("vunpcklps       %%ymm1, %%ymm0, %%ymm4")        /* ymm4 = r0  i0  r1  i1  r4  i4  r5  i5  */
            __ASM_EMIT("vunpckhps       %%ymm1, %%ymm0, %%ymm5")        /* ymm5 = r2  i2  r3  i3  r6  i6  r7  i7  */
            __ASM_EMIT("vmovups         %%xmm4, 0x00(%[dst])")
            __ASM_EMIT("vmovups         %%xmm5, 0x10(%[dst])")
            __ASM_EMIT("vextractf128    $1, %%ymm4, 0x20(%[dst])")
            __ASM_EMIT("vextractf128    $1, %%ymm5, 0x30(%[dst])")
            __ASM_EMIT("4:")

            : [dst] "+r"(dst), [blocks] "+r" (blocks)
            : [norm] "o" (norm)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }
}

#endif /* DSP_ARCH_X86_AVX_FFT_P_REPACK_H_ */
