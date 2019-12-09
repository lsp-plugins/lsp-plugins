/*
 * scramble.h
 *
 *  Created on: 9 дек. 2019 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_AVX_FFT_SCRAMBLE_H_
#define DSP_ARCH_X86_AVX_FFT_SCRAMBLE_H_

namespace avx
{
    static inline void scramble_self_direct(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank)
    {
        // Calculate number of items
        size_t items    = (1 << rank) - 1;

        for (size_t i = 1; i < items; ++i)
        {
            size_t j = reverse_bits(i, rank);    /* Reverse the order of the bits */
            if (i >= j)
                continue;

            /* Copy the values from the reversed position */
            ARCH_X86_ASM
            (
                __ASM_EMIT("vmovss (%[dst_re], %[i], 4), %%xmm0")
                __ASM_EMIT("vmovss (%[dst_im], %[i], 4), %%xmm1")
                __ASM_EMIT("vmovss (%[dst_re], %[j], 4), %%xmm2")
                __ASM_EMIT("vmovss (%[dst_im], %[j], 4), %%xmm3")
                __ASM_EMIT("vmovss %%xmm2, (%[dst_re], %[i], 4)")
                __ASM_EMIT("vmovss %%xmm3, (%[dst_im], %[i], 4)")
                __ASM_EMIT("vmovss %%xmm0, (%[dst_re], %[j], 4)")
                __ASM_EMIT("vmovss %%xmm1, (%[dst_im], %[j], 4)")
                :
                : [dst_re] "r"(dst_re), [dst_im] "r"(dst_im), [i] "r"(i), [j] "r"(j)
                : "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3"
            );
        }

        // Perform butterfly 8x
        size_t off = 0;
        items = 1 << (rank - 3);

        // Perform 4-element butterflies
        ARCH_X86_ASM
        (
            /* Loop 2x 4-element butterflies */
            __ASM_EMIT("sub             $2, %[items]")
            __ASM_EMIT("jb              2f")
            __ASM_EMIT("1:")
            /* Load data to registers */
            __ASM_EMIT("vmovups         0x00(%[dst_re], %[off]), %%ymm0")   /* ymm0 = r0 r1 r2 r3 */
            __ASM_EMIT("vmovups         0x20(%[dst_re], %[off]), %%ymm4")   /* ymm4 = r4 r5 r6 r7 */
            __ASM_EMIT("vmovups         0x00(%[dst_im], %[off]), %%ymm2")   /* ymm2 = i0 i1 i2 i3 */
            __ASM_EMIT("vmovups         0x20(%[dst_im], %[off]), %%ymm6")   /* ymm6 = i4 i5 i6 i7 */
            /* 1st-order 4x butterfly */
            __ASM_EMIT("vhsubps         %%ymm4, %%ymm0, %%ymm1")            /* ymm1 = r0-r1 r2-r3 r4-r5 r6-r7 = r1' r3' r5' r7' */
            __ASM_EMIT("vhsubps         %%ymm6, %%ymm2, %%ymm3")            /* ymm3 = i0-i1 i2-i3 i4-i5 i6-i7 = i1' i3' i5' i7' */
            __ASM_EMIT("vhaddps         %%ymm4, %%ymm0, %%ymm0")            /* ymm0 = r0+r1 r2+r3 r4+r5 r6+r7 = r0' r2' r4' r6' */
            __ASM_EMIT("vhaddps         %%ymm6, %%ymm2, %%ymm2")            /* ymm2 = i0+i1 i2+i3 i4+i5 i6+i7 = i0' i2' i4' i6' */
            /* 2nd-order 4x butterfly */
            __ASM_EMIT("vblendps        $0x0a, %%ymm3, %%ymm1, %%ymm4")     /* ymm4 = r1' i3' r5' i7' */
            __ASM_EMIT("vblendps        $0x0a, %%ymm1, %%ymm3, %%ymm5")     /* ymm5 = i1' r3' i5' r7' */
            __ASM_EMIT("vhsubps         %%ymm4, %%ymm0, %%ymm1")            /* ymm1 = r0'-r2' r4'-r6' r1'-i3' r5'-i7' = r2" r6" r3" r7" */
            __ASM_EMIT("vhsubps         %%ymm5, %%ymm2, %%ymm3")            /* ymm3 = i0'-i2' i4'-i6' i1'-r3' i5'-r7' = i2" i6" i1" i5" */
            __ASM_EMIT("vhaddps         %%ymm4, %%ymm0, %%ymm0")            /* ymm0 = r0'+r2' r4'+r6' r1'+i3' r5'+i7' = r0" r4" r1" r5" */
            __ASM_EMIT("vhsubps         %%ymm5, %%ymm2, %%ymm2")            /* ymm2 = i0'+i2' i4'+i6' i1'+r3' i5'+r7' = i0" i4" i3" i7" */
            /* Reorder and store */
            __ASM_EMIT("vblendps        $0x0c, %%ymm3, %%ymm2, %%ymm4")     /* ymm4 = i0" i4" i1" i5" */
            __ASM_EMIT("vblendps        $0x0c, %%ymm2, %%ymm3, %%ymm5")     /* ymm5 = i2" i6" i3" i7" */
            __ASM_EMIT("vshufps         $0x88, %%ymm1, %%ymm0, %%ymm2")     /* ymm2 = r0" r1" r2" r3" */
            __ASM_EMIT("vshufps         $0xdd, %%ymm1, %%ymm0, %%ymm3")     /* ymm3 = r4" r5" r6" r7" */
            __ASM_EMIT("vshufps         $0x88, %%ymm5, %%ymm4, %%ymm6")     /* ymm6 = i0" i1" i2" i3" */
            __ASM_EMIT("vshufps         $0xdd, %%ymm5, %%ymm4, %%ymm7")     /* ymm7 = i4" i5" i6" i7" */
            __ASM_EMIT("vmovups         %%ymm2, 0x00(%[dst_re], %[off])")
            __ASM_EMIT("vmovups         %%ymm3, 0x20(%[dst_re], %[off])")
            __ASM_EMIT("vmovups         %%ymm6, 0x00(%[dst_im], %[off])")
            __ASM_EMIT("vmovups         %%ymm7, 0x20(%[dst_im], %[off])")
            /* Move pointers and repeat*/
            __ASM_EMIT("add             $0x40, %[off]")
            __ASM_EMIT("sub             $2, %[items]")
            __ASM_EMIT("jae             1b")

            /* Repeat cycle */
            __ASM_EMIT("sub     $2, %[items]")
            __ASM_EMIT("jae     1b")
            __ASM_EMIT("2:")
            : [dst_re] "+r"(dst_re), [dst_im] "+r"(dst_im),
              [off] "+r" (off), [items] "+r"(items)
            :
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2"
        );
    }

    static inline void scramble_copy_direct(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank)
    {
    }
}

#endif /* DSP_ARCH_X86_AVX_FFT_SCRAMBLE_H_ */
