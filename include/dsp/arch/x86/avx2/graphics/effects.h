/*
 * effects.h
 *
 *  Created on: 19 дек. 2018 г.
 *      Author: sadko
 */

#ifndef INCLUDE_DSP_ARCH_X86_AVX2_GRAPHICS_EFFECTS_H_
#define INCLUDE_DSP_ARCH_X86_AVX2_GRAPHICS_EFFECTS_H_

#include <dsp/arch/x86/avx2/graphics/transpose.h>

namespace avx2
{
IF_ARCH_X86(
    static const float EFF_HSLA_HUE_XC[] __lsp_aligned32 =
    {
        1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f
    };
)

#define EFF_HSLA_HUE_CORE_X16 \
    /* ymm0 = v, ymm10 = h, ymm11 = s, ymm12 = l, ymm13 = 1, ymm14 = T, ymm15 = KT */ \
    __ASM_EMIT("vaddps          %%ymm0, %%ymm13, %%ymm2")           /* ymm2     = 1+v */ \
    __ASM_EMIT("vaddps          %%ymm4, %%ymm13, %%ymm6")           \
    __ASM_EMIT("vsubps          %%ymm0, %%ymm13, %%ymm3")           /* ymm3     = 1-v */ \
    __ASM_EMIT("vsubps          %%ymm4, %%ymm13, %%ymm7")           \
    __ASM_EMIT("vblendvps       %%ymm0, %%ymm2, %%ymm3, %%ymm0")    /* ymm0     = V = (v < 0) ? 1+v : 1-v */ \
    __ASM_EMIT("vblendvps       %%ymm4, %%ymm6, %%ymm7, %%ymm4")    \
    /* ymm0 = V, ymm1 = EH, ymm14 = T, ymm15 = KT */ \
    __ASM_EMIT("vsubps          %%ymm14, %%ymm0, %%ymm3")           /* ymm3     = V-T  */ \
    __ASM_EMIT("vsubps          %%ymm14, %%ymm4, %%ymm7")           \
    __ASM_EMIT("vblendvps       %%ymm3, %%ymm0, %%ymm14, %%ymm0")   /* ymm0     = (V-T) < 0 ? V : T */ \
    __ASM_EMIT("vblendvps       %%ymm7, %%ymm4, %%ymm14, %%ymm4")   \
    __ASM_EMIT("vxorps          %%ymm8, %%ymm8, %%ymm8")            /* ymm8     = 0 */ \
    __ASM_EMIT("vaddps          %%ymm10, %%ymm0, %%ymm0")           /* ymm0     = NH = h + ((V-T) < 0 ? V : T) */ \
    __ASM_EMIT("vblendvps       %%ymm3, %%ymm8, %%ymm3, %%ymm3")    /* ymm3     = (V-T) < 0 ? 0 : V-T */ \
    __ASM_EMIT("vaddps          %%ymm10, %%ymm4, %%ymm4")           \
    __ASM_EMIT("vblendvps       %%ymm7, %%ymm8, %%ymm7, %%ymm7")    \
    __ASM_EMIT("vsubps          %%ymm13, %%ymm0, %%ymm1")           /* ymm1     = NH-1 */ \
    __ASM_EMIT("vmulps          %%ymm15, %%ymm3, %%ymm3")           /* ymm3     = A = KT * ((V-T) < 0 ? 0 : V-T) */ \
    __ASM_EMIT("vsubps          %%ymm13, %%ymm4, %%ymm5")           \
    __ASM_EMIT("vmulps          %%ymm15, %%ymm7, %%ymm7")           \
    __ASM_EMIT("vmovaps         %%ymm12, %%ymm2")                   /* ymm2     = L */ \
    __ASM_EMIT("vmovaps         %%ymm12, %%ymm6")                   \
    __ASM_EMIT("vblendvps       %%ymm1, %%ymm0, %%ymm1, %%ymm0")    /* ymm0     = H = ((NH-1)<0) ? NH : NH-1 */ \
    __ASM_EMIT("vblendvps       %%ymm5, %%ymm4, %%ymm5, %%ymm4")    \
    __ASM_EMIT("vmovaps         %%ymm11, %%ymm1")                   /* ymm1     = S */ \
    __ASM_EMIT("vmovaps         %%ymm11, %%ymm5")                   \
    MAT4_TRANSPOSE("%%ymm0", "%%ymm1", "%%ymm2", "%%ymm3", "%%ymm8", "%%ymm9") \
    MAT4_TRANSPOSE("%%ymm4", "%%ymm5", "%%ymm6", "%%ymm7", "%%ymm8", "%%ymm9") \
    MAT4X2_INTERLEAVE("0", "1", "2", "3", "8", "9") \
    MAT4X2_INTERLEAVE("4", "5", "6", "7", "8", "9")

#define EFF_HSLA_HUE_CORE_X8 \
    /* ymm0 = v, ymm10 = h, ymm11 = s, ymm12 = l, ymm13 = 1, ymm14 = T, ymm15 = KT */ \
    __ASM_EMIT("vaddps          %%ymm0, %%ymm13, %%ymm2")           /* ymm2     = 1+v */ \
    __ASM_EMIT("vsubps          %%ymm0, %%ymm13, %%ymm3")           /* ymm3     = 1-v */ \
    __ASM_EMIT("vblendvps       %%ymm0, %%ymm2, %%ymm3, %%ymm0")    /* ymm0     = V = (v < 0) ? 1+v : 1-v */ \
    /* ymm0 = V, ymm1 = EH, ymm14 = T, ymm15 = KT */ \
    __ASM_EMIT("vsubps          %%ymm14, %%ymm0, %%ymm3")           /* ymm3     = V-T  */ \
    __ASM_EMIT("vblendvps       %%ymm3, %%ymm0, %%ymm14, %%ymm0")   /* ymm0     = (V-T) < 0 ? V : T */ \
    __ASM_EMIT("vxorps          %%ymm8, %%ymm8, %%ymm8")            /* ymm8     = 0 */ \
    __ASM_EMIT("vaddps          %%ymm10, %%ymm0, %%ymm0")           /* ymm0     = NH = h + ((V-T) < 0 ? V : T) */ \
    __ASM_EMIT("vblendvps       %%ymm3, %%ymm8, %%ymm3, %%ymm3")    /* ymm3     = (V-T) < 0 ? 0 : V-T */ \
    __ASM_EMIT("vsubps          %%ymm13, %%ymm0, %%ymm1")           /* ymm1     = NH-1 */ \
    __ASM_EMIT("vmulps          %%ymm15, %%ymm3, %%ymm3")           /* ymm3     = A = KT * ((V-T) < 0 ? 0 : V-T) */ \
    __ASM_EMIT("vmovaps         %%ymm12, %%ymm2")                   /* ymm2     = L */ \
    __ASM_EMIT("vblendvps       %%ymm1, %%ymm0, %%ymm1, %%ymm0")    /* ymm0     = H = ((NH-1)<0) ? NH : NH-1 */ \
    __ASM_EMIT("vmovaps         %%ymm11, %%ymm1")                   /* ymm1     = S */ \
    MAT4_TRANSPOSE("%%ymm0", "%%ymm1", "%%ymm2", "%%ymm3", "%%ymm8", "%%ymm9") \
    MAT4X2_INTERLEAVE("0", "1", "2", "3", "8", "9")

#define EFF_HSLA_HUE_CORE_X4 \
    /* xmm0 = v, xmm10 = h, xmm11 = s, xmm12 = l, xmm13 = 1, xmm14 = T, xmm15 = KT */ \
    __ASM_EMIT("vaddps          %%xmm0, %%xmm13, %%xmm2")           /* xmm2     = 1+v */ \
    __ASM_EMIT("vsubps          %%xmm0, %%xmm13, %%xmm3")           /* xmm3     = 1-v */ \
    __ASM_EMIT("vblendvps       %%xmm0, %%xmm2, %%xmm3, %%xmm0")    /* xmm0     = V = (v < 0) ? 1+v : 1-v */ \
    /* xmm0 = V, xmm1 = EH, xmm14 = T, xmm15 = KT */ \
    __ASM_EMIT("vsubps          %%xmm14, %%xmm0, %%xmm3")           /* xmm3     = V-T  */ \
    __ASM_EMIT("vblendvps       %%xmm3, %%xmm0, %%xmm14, %%xmm0")   /* xmm0     = (V-T) < 0 ? V : T */ \
    __ASM_EMIT("vxorps          %%xmm8, %%xmm8, %%xmm8")            /* xmm8     = 0 */ \
    __ASM_EMIT("vaddps          %%xmm10, %%xmm0, %%xmm0")           /* xmm0     = NH = h + ((V-T) < 0 ? V : T) */ \
    __ASM_EMIT("vblendvps       %%xmm3, %%xmm8, %%xmm3, %%xmm3")    /* xmm3     = (V-T) < 0 ? 0 : V-T */ \
    __ASM_EMIT("vsubps          %%xmm13, %%xmm0, %%xmm1")           /* xmm1     = NH-1 */ \
    __ASM_EMIT("vmulps          %%xmm15, %%xmm3, %%xmm3")           /* xmm3     = A = KT * ((V-T) < 0 ? 0 : V-T) */ \
    __ASM_EMIT("vmovaps         %%xmm12, %%xmm2")                   /* xmm2     = L */ \
    __ASM_EMIT("vblendvps       %%xmm1, %%xmm0, %%xmm1, %%xmm0")    /* xmm0     = H = ((NH-1)<0) ? NH : NH-1 */ \
    __ASM_EMIT("vmovaps         %%xmm11, %%xmm1")                   /* xmm1     = S */ \
    MAT4_TRANSPOSE("%%xmm0", "%%xmm1", "%%xmm2", "%%xmm3", "%%xmm8", "%%xmm9")

    void x64_eff_hsla_hue(float *dst, const float *v, const dsp::hsla_hue_eff_t *eff, size_t count)
    {
        ARCH_X86_ASM(
            //-----------------------------------------------------------------
            // Prepare
            __ASM_EMIT("vbroadcastf128  0x00(%[eff]), %%ymm10")             /* ymm10    = h s l a h s l a */
            __ASM_EMIT("vbroadcastss    0x10(%[eff]), %%ymm14")             /* ymm14    = t */
            __ASM_EMIT("vmovaps         0x00 + %[XC], %%ymm13")             /* ymm13    = 1 */
            __ASM_EMIT("vshufps         $0xaa, %%ymm10, %%ymm10, %%ymm12")  /* ymm12    = l */
            __ASM_EMIT("vdivps          %%ymm14, %%ymm13, %%ymm15")         /* ymm15    = KT = 1 / t */
            __ASM_EMIT("vshufps         $0x55, %%ymm10, %%ymm10, %%ymm11")  /* ymm12    = s */
            __ASM_EMIT("vsubps          %%ymm14, %%ymm13, %%ymm14")         /* ymm14    = T = 1 - t */
            __ASM_EMIT("vshufps         $0x00, %%ymm10, %%ymm10, %%ymm10")  /* ymm12    = h */

            //-----------------------------------------------------------------
            // x16 blocks
            __ASM_EMIT("sub             $16, %[count]")
            __ASM_EMIT("jb              2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
            __ASM_EMIT("vmovups         0x20(%[src]), %%ymm4")
            EFF_HSLA_HUE_CORE_X16
            __ASM_EMIT("vmovups         %%ymm0, 0x000(%[dst])")
            __ASM_EMIT("vmovups         %%ymm1, 0x020(%[dst])")
            __ASM_EMIT("vmovups         %%ymm2, 0x040(%[dst])")
            __ASM_EMIT("vmovups         %%ymm3, 0x060(%[dst])")
            __ASM_EMIT("vmovups         %%ymm4, 0x080(%[dst])")
            __ASM_EMIT("vmovups         %%ymm5, 0x0a0(%[dst])")
            __ASM_EMIT("vmovups         %%ymm6, 0x0c0(%[dst])")
            __ASM_EMIT("vmovups         %%ymm7, 0x0e0(%[dst])")
            __ASM_EMIT("add             $0x40, %[src]")
            __ASM_EMIT("add             $0x100, %[dst]")
            __ASM_EMIT("sub             $16, %[count]")
            __ASM_EMIT("jae             1b")

            //-----------------------------------------------------------------
            // x8 block
            __ASM_EMIT("2:")
            __ASM_EMIT("add             $8, %[count]")
            __ASM_EMIT("jl              4f")
            __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
            EFF_HSLA_HUE_CORE_X8
            __ASM_EMIT("vmovups         %%ymm0, 0x000(%[dst])")
            __ASM_EMIT("vmovups         %%ymm1, 0x020(%[dst])")
            __ASM_EMIT("vmovups         %%ymm2, 0x040(%[dst])")
            __ASM_EMIT("vmovups         %%ymm3, 0x060(%[dst])")
            __ASM_EMIT("sub             $8, %[count]")
            __ASM_EMIT("add             $0x20, %[src]")
            __ASM_EMIT("add             $0x80, %[dst]")

            //-----------------------------------------------------------------
            // x4 block
            __ASM_EMIT("4:")
            __ASM_EMIT("add             $4, %[count]")
            __ASM_EMIT("jl              6f")
            __ASM_EMIT("vmovups         0x00(%[src]), %%xmm0")
            EFF_HSLA_HUE_CORE_X4
            __ASM_EMIT("vmovups         %%xmm0, 0x000(%[dst])")
            __ASM_EMIT("vmovups         %%xmm1, 0x010(%[dst])")
            __ASM_EMIT("vmovups         %%xmm2, 0x020(%[dst])")
            __ASM_EMIT("vmovups         %%xmm3, 0x030(%[dst])")
            __ASM_EMIT("sub             $4, %[count]")
            __ASM_EMIT("add             $0x10, %[src]")
            __ASM_EMIT("add             $0x40, %[dst]")

            //-----------------------------------------------------------------
            // x1-x3 block
            __ASM_EMIT("6:")
            __ASM_EMIT("add             $4, %[count]")
            __ASM_EMIT("jle             14f")
            __ASM_EMIT("test            $1, %[count]")
            __ASM_EMIT("jz              8f")
            __ASM_EMIT("vmovss          0x00(%[src]), %%xmm0")
            __ASM_EMIT("add             $0x04, %[src]")
            __ASM_EMIT("8:")
            __ASM_EMIT("test            $2, %[count]")
            __ASM_EMIT("jz              10f")
            __ASM_EMIT("vmovhps         0x00(%[src]), %%xmm0, %%xmm0")
            __ASM_EMIT("10:")

            EFF_HSLA_HUE_CORE_X4

            // Store last chunk
            __ASM_EMIT("test            $1, %[count]")
            __ASM_EMIT("jz              12f")
            __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("add             $0x10, %[dst]")
            __ASM_EMIT("12:")
            __ASM_EMIT("test            $2, %[count]")
            __ASM_EMIT("jz              14f")
            __ASM_EMIT("vmovups         %%xmm2, 0x00(%[dst])")
            __ASM_EMIT("vmovups         %%xmm3, 0x10(%[dst])")

            __ASM_EMIT("14:")
            __ASM_EMIT("vzeroupper")

            : [dst] "+r" (dst), [src] "+r" (v), [count] "+r" (count)
            : [eff] "r" (eff),
              [XC] "o" (EFF_HSLA_HUE_XC)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7",
              "%xmm8", "%xmm9", "%xmm10", "%xmm11",
              "%xmm12", "%xmm13", "%xmm14", "%xmm15"
        );
/*
        float value, hue, alpha;
        float t     = 1.0f - eff->thresh;
        float kt    = 1.0f / eff->thresh;

        for (size_t i=0; i<count; ++i, dst += 4)
        {
            value   = v[i];
            value   = (value < 0) ? 1.0f + value : 1.0f - value;

            if ((value - t) >= 0)
            {
                hue         = eff->h + t;
                alpha       = ((value - t) * kt);
            }
            else
            {
                hue         = eff->h + value;
                alpha       = 0.0f;
            }

            dst[0]      = (hue < 1.0f) ? hue : hue - 1.0f;
            dst[1]      = eff->s;
            dst[2]      = eff->l;
            dst[3]      = alpha;
        }*/
    }
}


#endif /* INCLUDE_DSP_ARCH_X86_AVX2_GRAPHICS_EFFECTS_H_ */
