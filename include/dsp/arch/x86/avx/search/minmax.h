/*
 * minmax.h
 *
 *  Created on: 4 дек. 2019 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_AVX_SEARCH_MINMAX_H_
#define DSP_ARCH_X86_AVX_SEARCH_MINMAX_H_

#ifndef DSP_ARCH_X86_AVX_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_AVX_IMPL */

namespace avx
{
    #define MINMAX_CORE(SRC, OP) \
        __ASM_EMIT("vxorps          %%ymm0, %%ymm0, %%ymm0") \
        __ASM_EMIT("test            %[count], %[count]") \
        __ASM_EMIT("jz              10f") \
        /* 32x block */ \
        __ASM_EMIT("vbroadcastss    (%[" SRC "]), %%ymm0") \
        __ASM_EMIT("sub             $32, %[count]") \
        __ASM_EMIT("vmovaps         %%ymm0, %%ymm1") \
        __ASM_EMIT("jb              2f") \
        __ASM_EMIT("vmovaps         %%ymm0, %%ymm2") \
        __ASM_EMIT("vmovaps         %%ymm1, %%ymm3") \
        __ASM_EMIT("1:") \
        __ASM_EMIT(OP "ps           0x000(%[" SRC "]), %%ymm0, %%ymm0") \
        __ASM_EMIT(OP "ps           0x020(%[" SRC "]), %%ymm1, %%ymm1") \
        __ASM_EMIT(OP "ps           0x040(%[" SRC "]), %%ymm2, %%ymm2") \
        __ASM_EMIT(OP "ps           0x060(%[" SRC "]), %%ymm3, %%ymm3") \
        __ASM_EMIT("add             $0x80, %[" SRC "]") \
        __ASM_EMIT("sub             $32, %[count]") \
        __ASM_EMIT("jae             1b") \
        __ASM_EMIT(OP "ps           %%ymm2, %%ymm0, %%ymm0") \
        __ASM_EMIT(OP "ps           %%ymm3, %%ymm1, %%ymm1") \
        __ASM_EMIT("2:") \
        /* 16x block */ \
        __ASM_EMIT("add             $16, %[count]") \
        __ASM_EMIT("jl              4f") \
        __ASM_EMIT(OP "ps           0x000(%[" SRC "]), %%ymm0, %%ymm0") \
        __ASM_EMIT(OP "ps           0x020(%[" SRC "]), %%ymm1, %%ymm1") \
        __ASM_EMIT("add             $0x40, %[" SRC "]") \
        __ASM_EMIT("sub             $16, %[count]") \
        __ASM_EMIT("4:") \
        __ASM_EMIT(OP "ps           %%ymm1, %%ymm0, %%ymm0") \
        /* 8x block */ \
        __ASM_EMIT("add             $8, %[count]") \
        __ASM_EMIT("jl              6f") \
        __ASM_EMIT(OP "ps           0x000(%[" SRC "]), %%ymm0, %%ymm0") \
        __ASM_EMIT("add             $0x20, %[" SRC "]") \
        __ASM_EMIT("sub             $8, %[count]") \
        __ASM_EMIT("6:") \
        __ASM_EMIT("vextractf128    $1, %%ymm0, %%xmm1") \
        __ASM_EMIT(OP "ps           %%xmm1, %%xmm0, %%xmm0") \
        /* 4x block */ \
        __ASM_EMIT("add             $4, %[count]") \
        __ASM_EMIT("jl              8f") \
        __ASM_EMIT(OP "ps           0x000(%[" SRC "]), %%xmm0, %%xmm0") \
        __ASM_EMIT("add             $0x10, %[" SRC "]") \
        __ASM_EMIT("sub             $4, %[count]") \
        __ASM_EMIT("8:") \
        __ASM_EMIT("vunpckhps       %%xmm0, %%xmm0, %%xmm1") /* xmm0 = a b c d, xmm1 = c c d d */ \
        __ASM_EMIT("vunpcklps       %%xmm0, %%xmm0, %%xmm0") /* xmm0 = a a b b */ \
        __ASM_EMIT(OP "ps           %%xmm1, %%xmm0, %%xmm0") \
        __ASM_EMIT("vunpckhps       %%xmm0, %%xmm0, %%xmm1") \
        __ASM_EMIT("vunpcklps       %%xmm0, %%xmm0, %%xmm0") \
        __ASM_EMIT(OP "ps           %%xmm1, %%xmm0, %%xmm0") \
        /* 1x blocks */ \
        __ASM_EMIT("add             $3, %[count]") \
        __ASM_EMIT("jl              10f") \
        __ASM_EMIT("9:") \
        __ASM_EMIT("vmovss          0x000(%[" SRC "]), %%xmm1") \
        __ASM_EMIT(OP "ps           %%xmm1, %%xmm0, %%xmm0") \
        __ASM_EMIT("add             $0x04, %[" SRC "]") \
        __ASM_EMIT("dec             %[count]") \
        __ASM_EMIT("jge             9b") \
        /* end */ \
        __ASM_EMIT("10:")

    float min(const float *src, size_t count)
    {
        float res = 0.0f;
        ARCH_X86_ASM(
            MINMAX_CORE("src", "vmin")
            : [src] "+r" (src), [count] "+r" (count),
              [res] "+Yz" (res)
            :
            : "cc",
              "%xmm1", "%xmm2", "%xmm3"
        );

        return res;
    }

    float max(const float *src, size_t count)
    {
        float res = 0.0f;
        ARCH_X86_ASM(
            MINMAX_CORE("src", "vmax")
            : [src] "+r" (src), [count] "+r" (count),
              [res] "+Yz" (res)
            :
            : "cc",
              "%xmm1", "%xmm2", "%xmm3"
        );

        return res;
    }

#undef MINMAX_CORE

#define AMINMAX_CORE(SRC, OP) \
    __ASM_EMIT("vxorps          %%ymm0, %%ymm0, %%ymm0") \
    __ASM_EMIT("test            %[count], %[count]") \
    __ASM_EMIT("jz              10f") \
    /* 32x block */ \
    __ASM_EMIT("vbroadcastss    (%[src]), %%ymm0") \
    __ASM_EMIT("vmovaps         %[SIGN], %%ymm2") \
    __ASM_EMIT("vandps          %%ymm0, %%ymm2, %%ymm0") \
    __ASM_EMIT("sub             $32, %[count]") \
    __ASM_EMIT("vmovaps         %%ymm0, %%ymm1") \
    __ASM_EMIT("jb              2f") \
    __ASM_EMIT("1:") \
    __ASM_EMIT("vandps          0x000(%[" SRC "]), %%ymm2, %%ymm4") \
    __ASM_EMIT("vandps          0x020(%[" SRC "]), %%ymm2, %%ymm5") \
    __ASM_EMIT("vandps          0x040(%[" SRC "]), %%ymm2, %%ymm6") \
    __ASM_EMIT("vandps          0x060(%[" SRC "]), %%ymm2, %%ymm7") \
    __ASM_EMIT(OP "ps           %%ymm4, %%ymm0, %%ymm0") \
    __ASM_EMIT(OP "ps           %%ymm5, %%ymm1, %%ymm1") \
    __ASM_EMIT(OP "ps           %%ymm6, %%ymm0, %%ymm0") \
    __ASM_EMIT(OP "ps           %%ymm7, %%ymm1, %%ymm1") \
    __ASM_EMIT("add             $0x80, %[" SRC "]") \
    __ASM_EMIT("sub             $32, %[count]") \
    __ASM_EMIT("jae             1b") \
    __ASM_EMIT("2:") \
    /* 16x block */ \
    __ASM_EMIT("add             $16, %[count]") \
    __ASM_EMIT("jl              4f") \
    __ASM_EMIT("vandps          0x000(%[" SRC "]), %%ymm2, %%ymm4") \
    __ASM_EMIT("vandps          0x020(%[" SRC "]), %%ymm2, %%ymm5") \
    __ASM_EMIT(OP "ps           %%ymm4, %%ymm0, %%ymm0") \
    __ASM_EMIT(OP "ps           %%ymm5, %%ymm1, %%ymm1") \
    __ASM_EMIT("add             $0x40, %[" SRC "]") \
    __ASM_EMIT("sub             $16, %[count]") \
    __ASM_EMIT("4:") \
    __ASM_EMIT(OP "ps           %%ymm1, %%ymm0, %%ymm0") \
    /* 8x block */ \
    __ASM_EMIT("add             $8, %[count]") \
    __ASM_EMIT("jl              6f") \
    __ASM_EMIT("vandps          0x000(%[" SRC "]), %%ymm2, %%ymm4") \
    __ASM_EMIT(OP "ps           %%ymm4, %%ymm0, %%ymm0") \
    __ASM_EMIT("add             $0x20, %[" SRC "]") \
    __ASM_EMIT("sub             $8, %[count]") \
    __ASM_EMIT("6:") \
    __ASM_EMIT("vextractf128    $1, %%ymm0, %%xmm4") \
    __ASM_EMIT(OP "ps           %%xmm4, %%xmm0, %%xmm0") \
    /* 4x block */ \
    __ASM_EMIT("add             $4, %[count]") \
    __ASM_EMIT("jl              8f") \
    __ASM_EMIT("vandps          0x000(%[" SRC "]), %%xmm2, %%xmm4") \
    __ASM_EMIT(OP "ps           %%xmm4, %%xmm0, %%xmm0") \
    __ASM_EMIT("add             $0x10, %[" SRC "]") \
    __ASM_EMIT("sub             $4, %[count]") \
    __ASM_EMIT("8:") \
    __ASM_EMIT("vunpckhps       %%xmm0, %%xmm0, %%xmm4") /* xmm0 = a b c d, xmm1 = c c d d */ \
    __ASM_EMIT("vunpcklps       %%xmm0, %%xmm0, %%xmm0") /* xmm0 = a a b b */ \
    __ASM_EMIT(OP "ps           %%xmm4, %%xmm0, %%xmm0") \
    __ASM_EMIT("vunpckhps       %%xmm0, %%xmm0, %%xmm4") \
    __ASM_EMIT("vunpcklps       %%xmm0, %%xmm0, %%xmm0") \
    __ASM_EMIT(OP "ps           %%xmm4, %%xmm0, %%xmm0") \
    /* 1x blocks */ \
    __ASM_EMIT("add             $3, %[count]") \
    __ASM_EMIT("jl              10f") \
    __ASM_EMIT("9:") \
    __ASM_EMIT("vmovss          0x000(%[" SRC "]), %%xmm4") \
    __ASM_EMIT("vandps          %%xmm4, %%xmm2, %%xmm4") \
    __ASM_EMIT(OP "ps           %%xmm4, %%xmm0, %%xmm0") \
    __ASM_EMIT("add             $0x04, %[" SRC "]") \
    __ASM_EMIT("dec             %[count]") \
    __ASM_EMIT("jge             9b") \
    /* end */ \
    __ASM_EMIT("10:")

    float abs_min(const float *src, size_t count)
    {
        float res = 0.0f;
        ARCH_X86_ASM(
            AMINMAX_CORE("src", "vmin")
            : [src] "+r" (src), [count] "+r" (count),
              [res] "+Yz" (res)
            : [SIGN] "o" (X_SIGN)
            : "cc",
              "%xmm1", "%xmm2",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );

        return res;
    }

    float abs_max(const float *src, size_t count)
    {
        float res = 0.0f;
        ARCH_X86_ASM(
            AMINMAX_CORE("src", "vmax")
            : [src] "+r" (src), [count] "+r" (count),
              [res] "+Yz" (res)
            : [SIGN] "o" (X_SIGN)
            : "cc",
              "%xmm1", "%xmm2",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );

        return res;
    }

#undef AMINMAX_CORE

    void minmax(const float *src, size_t count, float *min, float *max)
    {
        ARCH_X86_ASM(
            __ASM_EMIT("vxorps          %%ymm0, %%ymm0, %%ymm0")
            __ASM_EMIT("vxorps          %%ymm1, %%ymm1, %%ymm1")
            __ASM_EMIT("test            %[count], %[count]")
            __ASM_EMIT("jz              10f")
            __ASM_EMIT("vbroadcastss    (%[src]), %%ymm0")
            __ASM_EMIT("vmovaps         %%ymm0, %%ymm1")
            /* 32x block */
            __ASM_EMIT("sub             $32, %[count]")
            __ASM_EMIT("jb              2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vmovups         0x000(%[src]), %%ymm4")
            __ASM_EMIT("vmovups         0x020(%[src]), %%ymm5")
            __ASM_EMIT("vmovups         0x040(%[src]), %%ymm6")
            __ASM_EMIT("vmovups         0x060(%[src]), %%ymm7")
            __ASM_EMIT("vminps          %%ymm4, %%ymm0, %%ymm0")
            __ASM_EMIT("vmaxps          %%ymm6, %%ymm1, %%ymm1")
            __ASM_EMIT("vminps          %%ymm5, %%ymm0, %%ymm0")
            __ASM_EMIT("vmaxps          %%ymm7, %%ymm1, %%ymm1")
            __ASM_EMIT("vminps          %%ymm6, %%ymm0, %%ymm0")
            __ASM_EMIT("vmaxps          %%ymm4, %%ymm1, %%ymm1")
            __ASM_EMIT("vminps          %%ymm7, %%ymm0, %%ymm0")
            __ASM_EMIT("vmaxps          %%ymm5, %%ymm1, %%ymm1")
            __ASM_EMIT("add             $0x80, %[src]")
            __ASM_EMIT("sub             $32, %[count]")
            __ASM_EMIT("jae             1b")
            __ASM_EMIT("2:")
            /* 16x block */
            __ASM_EMIT("add             $16, %[count]")
            __ASM_EMIT("jl              4f")
            __ASM_EMIT("vmovups         0x000(%[src]), %%ymm4")
            __ASM_EMIT("vmovups         0x020(%[src]), %%ymm5")
            __ASM_EMIT("vminps          %%ymm4, %%ymm0, %%ymm0")
            __ASM_EMIT("vmaxps          %%ymm4, %%ymm1, %%ymm1")
            __ASM_EMIT("vminps          %%ymm5, %%ymm0, %%ymm0")
            __ASM_EMIT("vmaxps          %%ymm5, %%ymm1, %%ymm1")
            __ASM_EMIT("add             $0x40, %[src]")
            __ASM_EMIT("sub             $16, %[count]")
            __ASM_EMIT("4:")
            /* 8x block */
            __ASM_EMIT("add             $8, %[count]")
            __ASM_EMIT("jl              6f")
            __ASM_EMIT("vmovups         0x000(%[src]), %%ymm4")
            __ASM_EMIT("vminps          %%ymm4, %%ymm0, %%ymm0")
            __ASM_EMIT("vmaxps          %%ymm4, %%ymm1, %%ymm1")
            __ASM_EMIT("add             $0x20, %[src]")
            __ASM_EMIT("sub             $8, %[count]")
            __ASM_EMIT("6:")
            __ASM_EMIT("vextractf128    $1, %%ymm0, %%xmm4")
            __ASM_EMIT("vextractf128    $1, %%ymm1, %%xmm5")
            __ASM_EMIT("vminps          %%xmm4, %%xmm0, %%xmm0")
            __ASM_EMIT("vmaxps          %%xmm5, %%xmm1, %%xmm1")
            /* 4x block */
            __ASM_EMIT("add             $4, %[count]")
            __ASM_EMIT("jl              8f")
            __ASM_EMIT("vmovups         0x000(%[src]), %%xmm4")
            __ASM_EMIT("vminps          %%xmm4, %%xmm0, %%xmm0")
            __ASM_EMIT("vmaxps          %%xmm4, %%xmm1, %%xmm1")
            __ASM_EMIT("add             $0x10, %[src]")
            __ASM_EMIT("sub             $4, %[count]")
            __ASM_EMIT("8:")
            __ASM_EMIT("vunpckhps       %%xmm0, %%xmm0, %%xmm4")
            __ASM_EMIT("vunpckhps       %%xmm1, %%xmm1, %%xmm5")
            __ASM_EMIT("vunpcklps       %%xmm0, %%xmm0, %%xmm0")
            __ASM_EMIT("vunpcklps       %%xmm1, %%xmm1, %%xmm1")
            __ASM_EMIT("vminps          %%xmm4, %%xmm0, %%xmm0")
            __ASM_EMIT("vmaxps          %%xmm5, %%xmm1, %%xmm1")
            __ASM_EMIT("vunpckhps       %%xmm0, %%xmm0, %%xmm4")
            __ASM_EMIT("vunpckhps       %%xmm1, %%xmm1, %%xmm5")
            __ASM_EMIT("vunpcklps       %%xmm0, %%xmm0, %%xmm0")
            __ASM_EMIT("vunpcklps       %%xmm1, %%xmm1, %%xmm1")
            __ASM_EMIT("vminps          %%xmm4, %%xmm0, %%xmm0")
            __ASM_EMIT("vmaxps          %%xmm5, %%xmm1, %%xmm1")
            /* 1x blocks */
            __ASM_EMIT("add             $3, %[count]")
            __ASM_EMIT("jl              10f")
            __ASM_EMIT("9:")
            __ASM_EMIT("vmovss          0x000(%[src]), %%xmm4")
            __ASM_EMIT("vminps          %%xmm4, %%xmm0, %%xmm0")
            __ASM_EMIT("vmaxps          %%xmm4, %%xmm1, %%xmm1")
            __ASM_EMIT("add             $0x04, %[src]")
            __ASM_EMIT("dec             %[count]")
            __ASM_EMIT("jge             9b")
            /* end */
            __ASM_EMIT("10:")
            __ASM_EMIT("vmovss          %%xmm0, 0x00(%[min])")
            __ASM_EMIT("vmovss          %%xmm1, 0x00(%[max])")
            : [src] "+r" (src), [count] "+r" (count)
            : [min] "r" (min), [max] "r" (max)
            : "cc", "memory",
              "%xmm0", "%xmm1",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void abs_minmax(const float *src, size_t count, float *min, float *max)
    {
        ARCH_X86_ASM(
            __ASM_EMIT("vxorps          %%ymm0, %%ymm0, %%ymm0")
            __ASM_EMIT("vxorps          %%ymm1, %%ymm1, %%ymm1")
            __ASM_EMIT("test            %[count], %[count]")
            __ASM_EMIT("jz              10f")
            /* 32x block */
            __ASM_EMIT("vmovaps         %[SIGN], %%ymm2")
            __ASM_EMIT("vbroadcastss    (%[src]), %%ymm0")
            __ASM_EMIT("vandps          %%ymm0, %%ymm2, %%ymm0")
            __ASM_EMIT("sub             $32, %[count]")
            __ASM_EMIT("vmovaps         %%ymm0, %%ymm1")
            __ASM_EMIT("jb              2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vandps          0x000(%[src]), %%ymm2, %%ymm4")
            __ASM_EMIT("vandps          0x020(%[src]), %%ymm2, %%ymm5")
            __ASM_EMIT("vandps          0x040(%[src]), %%ymm2, %%ymm6")
            __ASM_EMIT("vandps          0x060(%[src]), %%ymm2, %%ymm7")
            __ASM_EMIT("vminps          %%ymm4, %%ymm0, %%ymm0")
            __ASM_EMIT("vmaxps          %%ymm6, %%ymm1, %%ymm1")
            __ASM_EMIT("vminps          %%ymm5, %%ymm0, %%ymm0")
            __ASM_EMIT("vmaxps          %%ymm7, %%ymm1, %%ymm1")
            __ASM_EMIT("vminps          %%ymm6, %%ymm0, %%ymm0")
            __ASM_EMIT("vmaxps          %%ymm4, %%ymm1, %%ymm1")
            __ASM_EMIT("vminps          %%ymm7, %%ymm0, %%ymm0")
            __ASM_EMIT("vmaxps          %%ymm5, %%ymm1, %%ymm1")
            __ASM_EMIT("add             $0x80, %[src]")
            __ASM_EMIT("sub             $32, %[count]")
            __ASM_EMIT("jae             1b")
            __ASM_EMIT("2:")
            /* 16x block */
            __ASM_EMIT("add             $16, %[count]")
            __ASM_EMIT("jl              4f")
            __ASM_EMIT("vandps          0x000(%[src]), %%ymm2, %%ymm4")
            __ASM_EMIT("vandps          0x020(%[src]), %%ymm2, %%ymm5")
            __ASM_EMIT("vminps          %%ymm4, %%ymm0, %%ymm0")
            __ASM_EMIT("vmaxps          %%ymm4, %%ymm1, %%ymm1")
            __ASM_EMIT("vminps          %%ymm5, %%ymm0, %%ymm0")
            __ASM_EMIT("vmaxps          %%ymm5, %%ymm1, %%ymm1")
            __ASM_EMIT("add             $0x40, %[src]")
            __ASM_EMIT("sub             $16, %[count]")
            __ASM_EMIT("4:")
            /* 8x block */
            __ASM_EMIT("add             $8, %[count]")
            __ASM_EMIT("jl              6f")
            __ASM_EMIT("vandps          0x000(%[src]), %%ymm2, %%ymm4")
            __ASM_EMIT("vminps          %%ymm4, %%ymm0, %%ymm0")
            __ASM_EMIT("vmaxps          %%ymm4, %%ymm1, %%ymm1")
            __ASM_EMIT("add             $0x20, %[src]")
            __ASM_EMIT("sub             $8, %[count]")
            __ASM_EMIT("6:")
            __ASM_EMIT("vextractf128    $1, %%ymm0, %%xmm4")
            __ASM_EMIT("vextractf128    $1, %%ymm1, %%xmm5")
            __ASM_EMIT("vminps          %%xmm4, %%xmm0, %%xmm0")
            __ASM_EMIT("vmaxps          %%xmm5, %%xmm1, %%xmm1")
            /* 4x block */
            __ASM_EMIT("add             $4, %[count]")
            __ASM_EMIT("jl              8f")
            __ASM_EMIT("vandps          0x000(%[src]), %%xmm2, %%xmm4")
            __ASM_EMIT("vminps          %%xmm4, %%xmm0, %%xmm0")
            __ASM_EMIT("vmaxps          %%xmm4, %%xmm1, %%xmm1")
            __ASM_EMIT("add             $0x10, %[src]")
            __ASM_EMIT("sub             $4, %[count]")
            __ASM_EMIT("8:")
            __ASM_EMIT("vunpckhps       %%xmm0, %%xmm0, %%xmm4")
            __ASM_EMIT("vunpckhps       %%xmm1, %%xmm1, %%xmm5")
            __ASM_EMIT("vunpcklps       %%xmm0, %%xmm0, %%xmm0")
            __ASM_EMIT("vunpcklps       %%xmm1, %%xmm1, %%xmm1")
            __ASM_EMIT("vminps          %%xmm4, %%xmm0, %%xmm0")
            __ASM_EMIT("vmaxps          %%xmm5, %%xmm1, %%xmm1")
            __ASM_EMIT("vunpckhps       %%xmm0, %%xmm0, %%xmm4")
            __ASM_EMIT("vunpckhps       %%xmm1, %%xmm1, %%xmm5")
            __ASM_EMIT("vunpcklps       %%xmm0, %%xmm0, %%xmm0")
            __ASM_EMIT("vunpcklps       %%xmm1, %%xmm1, %%xmm1")
            __ASM_EMIT("vminps          %%xmm4, %%xmm0, %%xmm0")
            __ASM_EMIT("vmaxps          %%xmm5, %%xmm1, %%xmm1")
            /* 1x blocks */
            __ASM_EMIT("add             $3, %[count]")
            __ASM_EMIT("jl              10f")
            __ASM_EMIT("9:")
            __ASM_EMIT("vmovss          0x000(%[src]), %%xmm4")
            __ASM_EMIT("vandps          %%xmm4, %%xmm2, %%xmm4")
            __ASM_EMIT("vminps          %%xmm4, %%xmm0, %%xmm0")
            __ASM_EMIT("vmaxps          %%xmm4, %%xmm1, %%xmm1")
            __ASM_EMIT("add             $0x04, %[src]")
            __ASM_EMIT("dec             %[count]")
            __ASM_EMIT("jge             9b")
            /* end */
            __ASM_EMIT("10:")
            __ASM_EMIT("vmovss          %%xmm0, 0x00(%[min])")
            __ASM_EMIT("vmovss          %%xmm1, 0x00(%[max])")
            : [src] "+r" (src), [count] "+r" (count)
            : [min] "r" (min), [max] "r" (max),
              [SIGN] "o" (X_SIGN)
            : "cc", "memory",
              "%xmm0", "%xmm1",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

}




#endif /* DSP_ARCH_X86_AVX_SEARCH_MINMAX_H_ */
