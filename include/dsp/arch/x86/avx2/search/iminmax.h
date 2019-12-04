/*
 * iminmax.h
 *
 *  Created on: 4 дек. 2019 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_AVX2_SEARCH_IMINMAX_H_
#define DSP_ARCH_X86_AVX2_SEARCH_IMINMAX_H_

namespace avx2
{
    IF_ARCH_X86(
        static const uint32_t minmax_incr[] __lsp_aligned64 =
        {
            0, 1, 2, 3, // Index
            4, 5, 6, 7,
            8, 8, 8, 8, // Increment
            8, 8, 8, 8,
            4, 4, 4, 4,
            1, 1, 1, 1,
            0x7fffffff, 0x7fffffff, 0x7fffffff, 0x7fffffff, // Abs mask
            0x7fffffff, 0x7fffffff, 0x7fffffff, 0x7fffffff
        }
    );

    #define SEL_NONE(x, y)          x
    #define SEL_ABS(x, y)           y

    /*
     * Register allocation:
     *  ymm0    = imin
     *  ymm1    = vmin
     *  ymm2    = cind
     *  ymm3    = cval
     *  ymm4    = cmask
     *  ymm5    = temp
     *  ymm6    = amask
     *  ymm7    = incr
     */
    #define IMINMAX_CORE(COND, SEL) \
        __ASM_EMIT("vpxor           %%ymm0, %%ymm0, %%ymm0") \
        __ASM_EMIT("test            %[count], %[count]") \
        __ASM_EMIT("jz              6f") \
        /* 8x block */ \
        __ASM_EMIT("vbroadcastss    0x00(%[src]), %%ymm1") \
        __ASM_EMIT(SEL("", "vmovaps         0x60 + %[INCR], %%ymm6")) \
        __ASM_EMIT("vmovdqu         0x00 + %[INCR], %%ymm2") \
        __ASM_EMIT(SEL("", "vandps          %%ymm6, %%ymm1, %%ymm1")) \
        __ASM_EMIT("vmovdqu         0x20 + %[INCR], %%ymm7") \
        __ASM_EMIT("sub             $8, %[count]") \
        __ASM_EMIT("jb              2f") \
        __ASM_EMIT("1:") \
        __ASM_EMIT(SEL("vmovups     0x00(%[src]), %%ymm3", "vandps 0x00(%[src]), %%ymm6, %%ymm3")) \
        __ASM_EMIT("vcmpps          " COND ", %%ymm1, %%ymm3, %%ymm4") \
        __ASM_EMIT("vpblendvb       %%ymm4, %%ymm2, %%ymm0, %%ymm0") \
        __ASM_EMIT("vblendvps       %%ymm4, %%ymm3, %%ymm1, %%ymm1") \
        __ASM_EMIT("vpaddd          %%ymm7, %%ymm2, %%ymm2") \
        __ASM_EMIT("add             $0x20, %[src]") \
        __ASM_EMIT("sub             $8, %[count]") \
        __ASM_EMIT("jae             1b") \
        __ASM_EMIT("vextracti128    $1, %%ymm0, %%xmm5") \
        __ASM_EMIT("vextractf128    $1, %%ymm1, %%xmm3") \
        __ASM_EMIT("vcmpps          " COND ", %%xmm1, %%xmm3, %%xmm4") \
        __ASM_EMIT("vpblendvb       %%xmm4, %%xmm5, %%xmm0, %%xmm0") \
        __ASM_EMIT("vblendvps       %%xmm4, %%xmm3, %%xmm1, %%xmm1") \
        __ASM_EMIT("2:") \
        /* 4x block */ \
        __ASM_EMIT("add             $4, %[count]") \
        __ASM_EMIT("jl              4f") \
        __ASM_EMIT(SEL("vmovups     0x00(%[src]), %%xmm3", "vandps 0x00(%[src]), %%xmm6, %%xmm3")) \
        __ASM_EMIT("vcmpps          " COND ", %%xmm1, %%xmm3, %%xmm4") \
        __ASM_EMIT("vpblendvb       %%xmm4, %%xmm2, %%xmm0, %%xmm0") \
        __ASM_EMIT("vblendvps       %%xmm4, %%xmm3, %%xmm1, %%xmm1") \
        __ASM_EMIT("vpaddd          0x40 + %[INCR], %%xmm2, %%xmm2") \
        __ASM_EMIT("sub             $4, %[count]") \
        __ASM_EMIT("add             $0x10, %[src]") \
        __ASM_EMIT("4:") \
        __ASM_EMIT("vpshufd         $0x39, %%xmm0, %%xmm5") \
        __ASM_EMIT("vshufps         $0x39, %%xmm1, %%xmm1, %%xmm3") \
        __ASM_EMIT("vcmpps          " COND ", %%xmm1, %%xmm3, %%xmm4") \
        __ASM_EMIT("vpblendvb       %%xmm4, %%xmm5, %%xmm0, %%xmm0") \
        __ASM_EMIT("vblendvps       %%xmm4, %%xmm3, %%xmm1, %%xmm1") \
        __ASM_EMIT("vpunpckhdq      %%xmm0, %%xmm0, %%xmm5") \
        __ASM_EMIT("vunpckhps       %%xmm1, %%xmm1, %%xmm3") \
        __ASM_EMIT("vcmpps          " COND ", %%xmm1, %%xmm3, %%xmm4") \
        __ASM_EMIT("vpblendvb       %%xmm4, %%xmm5, %%xmm0, %%xmm0") \
        __ASM_EMIT("vblendvps       %%xmm4, %%xmm3, %%xmm1, %%xmm1") \
        /* 1x block */ \
        __ASM_EMIT("add             $3, %[count]") \
        __ASM_EMIT("jl              6f") \
        __ASM_EMIT("5:") \
        __ASM_EMIT("movss           0x00(%[src]), %%xmm3") \
        __ASM_EMIT(SEL("", "vandps 0x00(%[src]), %%ymm6, %%ymm3")) \
        __ASM_EMIT("vcmpps          " COND ", %%xmm1, %%xmm3, %%xmm4") \
        __ASM_EMIT("vpblendvb       %%xmm4, %%xmm2, %%xmm0, %%xmm0") \
        __ASM_EMIT("vblendvps       %%xmm4, %%xmm3, %%xmm1, %%xmm1") \
        __ASM_EMIT("vpaddd          0x50 + %[INCR], %%xmm2, %%xmm2") \
        __ASM_EMIT("add             $0x04, %[src]") \
        __ASM_EMIT("dec             %[count]") \
        __ASM_EMIT("jge             5b") \
        /* end */ \
        __ASM_EMIT("6:")


    size_t min_index(const float *src, size_t count)
    {
        uint32_t index;

        ARCH_X86_ASM(
            IMINMAX_CORE("$1", SEL_NONE)
            : [src] "+r" (src), [count] "+r" (count),
              [index] "=Yz" (index)
            : [INCR] "o" (minmax_incr)
            : "cc",
              "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );

        return index;
    }

    size_t max_index(const float *src, size_t count)
    {
        uint32_t index;

        ARCH_X86_ASM(
            IMINMAX_CORE("$6", SEL_NONE)
            : [src] "+r" (src), [count] "+r" (count),
              [index] "=Yz" (index)
            : [INCR] "o" (minmax_incr)
            : "cc",
              "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );

        return index;
    }

    size_t abs_min_index(const float *src, size_t count)
    {
        uint32_t index;

        ARCH_X86_ASM(
            IMINMAX_CORE("$1", SEL_ABS)
            : [src] "+r" (src), [count] "+r" (count),
              [index] "=Yz" (index)
            : [INCR] "o" (minmax_incr)
            : "cc",
              "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );

        return index;
    }

    size_t abs_max_index(const float *src, size_t count)
    {
        uint32_t index;

        ARCH_X86_ASM(
            IMINMAX_CORE("$6", SEL_ABS)
            : [src] "+r" (src), [count] "+r" (count),
              [index] "=Yz" (index)
            : [INCR] "o" (minmax_incr)
            : "cc",
              "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );

        return index;
    }

    #undef SEL_NONE
    #undef SEL_ABS

}

#endif /* DSP_ARCH_X86_AVX2_SEARCH_IMINMAX_H_ */
