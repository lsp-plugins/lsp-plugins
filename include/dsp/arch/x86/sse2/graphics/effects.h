/*
 * effects.h
 *
 *  Created on: 21 нояб. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_SSE_GRAPHICS_EFFECTS_H_
#define DSP_ARCH_X86_SSE_GRAPHICS_EFFECTS_H_

#ifndef DSP_ARCH_X86_SSE2_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_SSE2_IMPL */

#define X4_TRANSPOSE \
    __ASM_EMIT("movaps          %%xmm2, %%xmm4") \
    __ASM_EMIT("punpckldq       %%xmm3, %%xmm2") \
    __ASM_EMIT("punpckhdq       %%xmm3, %%xmm4") \
    __ASM_EMIT("movaps          %%xmm0, %%xmm3") \
    __ASM_EMIT("punpckldq       %%xmm1, %%xmm0") \
    __ASM_EMIT("punpckhdq       %%xmm1, %%xmm3") \
    __ASM_EMIT("movaps          %%xmm0, %%xmm1") \
    __ASM_EMIT("punpcklqdq      %%xmm2, %%xmm0") \
    __ASM_EMIT("punpckhqdq      %%xmm2, %%xmm1") \
    __ASM_EMIT("movaps          %%xmm3, %%xmm2") \
    __ASM_EMIT("punpcklqdq      %%xmm4, %%xmm2") \
    __ASM_EMIT("punpckhqdq      %%xmm4, %%xmm3") \

namespace sse2
{
    static const float EFF_HSLA_HUE_XC[] __lsp_aligned16 =
    {
        1.0f, 1.0f, 1.0f, 1.0f
    };

#define EFF_HSLA_HUE_CORE \
    /* xmm0 = v, xmm6 = T, xmm7 = KT */ \
    __ASM_EMIT("movaps          0x00 + %[XC], %%xmm5")      /* xmm5 = 1 */ \
    __ASM_EMIT("xorps           %%xmm4, %%xmm4")            /* xmm4 = 0 */ \
    __ASM_EMIT("movaps          %%xmm5, %%xmm1")            /* xmm1 = 1 */ \
    __ASM_EMIT("cmpps           $6, %%xmm0, %%xmm4")        /* xmm4 = [0 > v] */ \
    __ASM_EMIT("movups          0x00(%[eff]), %%xmm2")      /* xmm2 = h s l a */ \
    __ASM_EMIT("subps           %%xmm0, %%xmm1")            /* xmm1 = 1 - v */ \
    __ASM_EMIT("shufps          $0x00, %%xmm2, %%xmm2")     /* xmm2 = EH */ \
    __ASM_EMIT("addps           %%xmm5, %%xmm0")            /* xmm0 = 1 + v */ \
    __ASM_EMIT("andps           %%xmm4, %%xmm0")            /* xmm0 = (1+v) & [0 > v] */ \
    __ASM_EMIT("andnps          %%xmm1, %%xmm4")            /* xmm4 = (1-v) & [0 <= v] */ \
    __ASM_EMIT("orps            %%xmm4, %%xmm0")            /* xmm0 = V = ((1+v) & [0 > v]) | ((1-v) & [0 <= v]) */ \
    /* xmm0   = V  */  \
    /* xmm2   = EH */ \
    /* xmm6   = T */ \
    /* xmm7   = KT */ \
    __ASM_EMIT("movaps          %%xmm0, %%xmm1")            /* xmm1 = V */ \
    __ASM_EMIT("xorps           %%xmm4, %%xmm4")            /* xmm4 = 0 */ \
    __ASM_EMIT("subps           %%xmm6, %%xmm1")            /* xmm1 = V - T */ \
    __ASM_EMIT("addps           %%xmm2, %%xmm0")            /* xmm0 = EH + V */ \
    __ASM_EMIT("movaps          %%xmm1, %%xmm3")            /* xmm2 = V - T */ \
    __ASM_EMIT("addps           %%xmm6, %%xmm2")            /* xmm3 = EH + T */ \
    __ASM_EMIT("mulps           %%xmm7, %%xmm3")            /* xmm3 = (V-T)*KT */ \
    __ASM_EMIT("cmpps           $2, %%xmm1, %%xmm4")        /* xmm4 = [(V - T) >= 0] */ \
    __ASM_EMIT("andps           %%xmm4, %%xmm3")            /* xmm3 = A = ((V-T)*KT) & [(V - T) >= 0] */ \
    __ASM_EMIT("andps           %%xmm4, %%xmm2")            /* xmm2 = (EH + T) & [(V - T) >= 0] */ \
    __ASM_EMIT("andnps          %%xmm0, %%xmm4")            /* xmm4 = (EH + V) & [(V - T) < 0] */ \
    __ASM_EMIT("orps            %%xmm2, %%xmm4")            /* xmm4 = NH = ((EH + T) & [(V - T) >= 0]) | ((EH + V) & [(V - T) < 0]) */ \
    __ASM_EMIT("movaps          0x00 + %[XC], %%xmm0")      /* xmm0 = 1 */ \
    __ASM_EMIT("movaps          %%xmm4, %%xmm1")            /* xmm1 = NH */ \
    __ASM_EMIT("movaps          %%xmm0, %%xmm5")            /* xmm5 = 1 */ \
    __ASM_EMIT("cmpps           $6, %%xmm4, %%xmm0")        /* xmm0 = [1 > NH] */ \
    __ASM_EMIT("subps           %%xmm5, %%xmm4")            /* xmm4 = NH - 1 */ \
    __ASM_EMIT("movups          0x00(%[eff]), %%xmm2")      /* xmm2 = h s l a */ \
    __ASM_EMIT("andps           %%xmm0, %%xmm1")            /* xmm1 = NH & [1 > H] */ \
    __ASM_EMIT("andnps          %%xmm4, %%xmm0")            /* xmm0 = (NH-1) & [1 <= H] */ \
    __ASM_EMIT("orps            %%xmm1, %%xmm0")            /* xmm0 = H = (NH & [1 > H]) | ((NH-1) & [1 <= H]) */ \
    __ASM_EMIT("movaps          %%xmm2, %%xmm1")            /* xmm1 = h s l a */ \
    __ASM_EMIT("shufps          $0x55, %%xmm1, %%xmm1")     /* xmm1 = S */ \
    __ASM_EMIT("shufps          $0xaa, %%xmm2, %%xmm2")     /* xmm2 = L */ \
    \
    X4_TRANSPOSE


    void eff_hsla_hue(float *dst, const float *v, const dsp::hsla_hue_eff_t *eff, size_t count)
    {
        ARCH_X86_ASM(
            __ASM_EMIT("movaps          0x00 + %[XC], %%xmm6")      /* xmm6 = 1 */
            __ASM_EMIT("movss           0x10(%[eff]), %%xmm4")      /* xmm4 = t 0 0 0 */
            __ASM_EMIT("movaps          %%xmm6, %%xmm7")            /* xmm7 = 1 */
            __ASM_EMIT("shufps          $0x00, %%xmm4, %%xmm4")     /* xmm4 = t */
            __ASM_EMIT("subps           %%xmm4, %%xmm6")            /* xmm6 = T  = 1 - t */
            __ASM_EMIT("divps           %%xmm4, %%xmm7")            /* xmm7 = KT = 1 / t */

            __ASM_EMIT("sub             $4, %[count]")
            __ASM_EMIT("jb              2f")

            //-----------------------------------------------------------------
            // 4x blocks
            __ASM_EMIT("1:")

            __ASM_EMIT("movups          0x00(%[src]), %%xmm0")      /* xmm0 = v */
            EFF_HSLA_HUE_CORE

            __ASM_EMIT("movups          %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("movups          %%xmm1, 0x10(%[dst])")
            __ASM_EMIT("movups          %%xmm2, 0x20(%[dst])")
            __ASM_EMIT("movups          %%xmm3, 0x30(%[dst])")

            __ASM_EMIT("add             $0x10, %[src]")
            __ASM_EMIT("add             $0x40, %[dst]")
            __ASM_EMIT("sub             $4, %[count]")
            __ASM_EMIT("jae             1b")

            __ASM_EMIT("2:")
            __ASM_EMIT("add             $4, %[count]")
            __ASM_EMIT("jle             10f")

            //-----------------------------------------------------------------
            // 1x - 3x block
            // Load last variable-sized chunk
            __ASM_EMIT("test            $1, %[count]")
            __ASM_EMIT("jz              4f")
            __ASM_EMIT("movss           0x00(%[src]), %%xmm0")
            __ASM_EMIT("add             $0x04, %[src]")
            __ASM_EMIT("movlhps         %%xmm0, %%xmm0")
            __ASM_EMIT("4:")
            __ASM_EMIT("test            $2, %[count]")
            __ASM_EMIT("jz              6f")
            __ASM_EMIT("movlps          0x00(%[src]), %%xmm0")
            __ASM_EMIT("6:")

            EFF_HSLA_HUE_CORE

            // Store last chunk
            __ASM_EMIT("test            $1, %[count]")
            __ASM_EMIT("jz              8f")
            __ASM_EMIT("movups          %%xmm2, 0x00(%[dst])")
            __ASM_EMIT("add             $0x10, %[dst]")
            __ASM_EMIT("8:")
            __ASM_EMIT("test            $2, %[count]")
            __ASM_EMIT("jz              10f")
            __ASM_EMIT("movups          %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("movups          %%xmm1, 0x10(%[dst])")

            __ASM_EMIT("10:")

            : [dst] "+r" (dst), [src] "+r" (v), [count] "+r" (count)
            : [eff] "r" (eff),
              [XC] "o" (EFF_HSLA_HUE_XC)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
/*
        float value, hue, alpha;
        float t     = 1.0f - eff->thresh;
        float kt    = 1.0f / eff->thresh;

        for (size_t i=0; i<count; ++i, dst += 4)
        {
            value   = v[i];
            value   = (0 > value) ? 1.0f + value : 1.0f - value;

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

#undef EFF_HSLA_HUE_CORE

    static const float EFF_HSLA_ALPHA_XC[] __lsp_aligned16 =
    {
        1.0f, 1.0f, 1.0f, 1.0f
    };

#define EFF_HSLA_ALPHA_CORE \
    /* xmm3 = v */ \
    __ASM_EMIT("xorps           %%xmm4, %%xmm4")            /* xmm4 = 0 */ \
    __ASM_EMIT("movaps          0x00 + %[XC], %%xmm5")      /* xmm5 = 1 */ \
    __ASM_EMIT("cmpps           $6, %%xmm3, %%xmm4")        /* xmm4 = [0 > v] */ \
    __ASM_EMIT("movups          0x00(%[eff]), %%xmm0")      /* xmm0 = hsla */ \
    __ASM_EMIT("movaps          %%xmm3, %%xmm6")            /* xmm6 = v */ \
    __ASM_EMIT("movaps          %%xmm0, %%xmm1")            /* xmm1 = hsla */ \
    __ASM_EMIT("addps           %%xmm5, %%xmm3")            /* xmm3 = 1 + v */ \
    __ASM_EMIT("movaps          %%xmm1, %%xmm2")            /* xmm2 = hsla */ \
    __ASM_EMIT("subps           %%xmm6, %%xmm5")            /* xmm5 = 1 - v */ \
    __ASM_EMIT("shufps          $0x00, %%xmm0, %%xmm0")     /* xmm0 = H */ \
    __ASM_EMIT("andps           %%xmm4, %%xmm3")            /* xmm4 = (1+v) & [0 > v] */ \
    __ASM_EMIT("shufps          $0x55, %%xmm1, %%xmm1")     /* xmm1 = S */ \
    __ASM_EMIT("andnps          %%xmm5, %%xmm4")            /* xmm4 = (1-v) & [0 <= v] */ \
    __ASM_EMIT("shufps          $0xaa, %%xmm2, %%xmm2")     /* xmm1 = L */ \
    __ASM_EMIT("orps            %%xmm4, %%xmm3")            /* xmm3 = A = ((1+v) & [0 > v]) | ((1-v) & [0 <= v]) */ \
    \
    X4_TRANSPOSE

    /*
        value   = v[i];
        value   = (0.0f > value) ? 1.0f + value : 1.0f - value;

        dst[0]  = eff->h;
        dst[1]  = eff->s;
        dst[2]  = eff->l;
        dst[3]  = value; // Fill alpha channel
     */

    void eff_hsla_alpha(float *dst, const float *v, const dsp::hsla_alpha_eff_t *eff, size_t count)
    {
        ARCH_X86_ASM(
            __ASM_EMIT("sub             $4, %[count]")
            __ASM_EMIT("jb              2f")

            //-----------------------------------------------------------------
            // 4x blocks
            __ASM_EMIT("1:")

            __ASM_EMIT("movups          0x00(%[src]), %%xmm3")      /* xmm3 = v */
            EFF_HSLA_ALPHA_CORE

            __ASM_EMIT("movups          %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("movups          %%xmm1, 0x10(%[dst])")
            __ASM_EMIT("movups          %%xmm2, 0x20(%[dst])")
            __ASM_EMIT("movups          %%xmm3, 0x30(%[dst])")

            __ASM_EMIT("add             $0x10, %[src]")
            __ASM_EMIT("add             $0x40, %[dst]")
            __ASM_EMIT("sub             $4, %[count]")
            __ASM_EMIT("jae             1b")

            __ASM_EMIT("2:")
            __ASM_EMIT("add             $4, %[count]")
            __ASM_EMIT("jle             10f")

            //-----------------------------------------------------------------
            // 1x - 3x block
            // Load last variable-sized chunk
            __ASM_EMIT("test            $1, %[count]")
            __ASM_EMIT("jz              4f")
            __ASM_EMIT("movss           0x00(%[src]), %%xmm3")
            __ASM_EMIT("add             $0x04, %[src]")
            __ASM_EMIT("movlhps         %%xmm3, %%xmm3")
            __ASM_EMIT("4:")
            __ASM_EMIT("test            $2, %[count]")
            __ASM_EMIT("jz              6f")
            __ASM_EMIT("movlps          0x00(%[src]), %%xmm3")
            __ASM_EMIT("6:")

            EFF_HSLA_ALPHA_CORE

            // Store last chunk
            __ASM_EMIT("test            $1, %[count]")
            __ASM_EMIT("jz              8f")
            __ASM_EMIT("movups          %%xmm2, 0x00(%[dst])")
            __ASM_EMIT("add             $0x10, %[dst]")
            __ASM_EMIT("8:")
            __ASM_EMIT("test            $2, %[count]")
            __ASM_EMIT("jz              10f")
            __ASM_EMIT("movups          %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("movups          %%xmm1, 0x10(%[dst])")

            __ASM_EMIT("10:")

            : [dst] "+r" (dst), [src] "+r" (v), [count] "+r" (count)
            : [eff] "r" (eff),
              [XC] "o" (EFF_HSLA_ALPHA_XC)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );

    }
#undef EFF_HSLA_ALPHA_CORE

    static const uint32_t EFF_HSLA_SAT_XC[] __lsp_aligned16 =
    {
        0x3f800000, 0x3f800000, 0x3f800000, 0x3f800000,
        0x7fffffff, 0x7fffffff, 0x7fffffff, 0x7fffffff
    };

#define EFF_HSLA_SAT_CORE \
    /* xmm1 = v, xmm6 = T, xmm7 = KT */ \
    __ASM_EMIT("movaps          %%xmm6, %%xmm0")            /* xmm0 = T */ \
    __ASM_EMIT("andps           0x10 + %[XC], %%xmm1")      /* xmm1 = V */ \
    __ASM_EMIT("xorps           %%xmm5, %%xmm5")            /* xmm5 = 0 */ \
    __ASM_EMIT("subps           %%xmm1, %%xmm0")            /* xmm0 = T-V */ \
    __ASM_EMIT("cmpps           $6, %%xmm0, %%xmm5")        /* xmm5 = [ 0 >= T - V] */ \
    __ASM_EMIT("mulps           %%xmm7, %%xmm0")            /* xmm0 = (T-V)*KT */ \
    __ASM_EMIT("movaps          %%xmm5, %%xmm3")            /* xmm3 = [ 0 >= T - V] */ \
    __ASM_EMIT("movups          0x00(%[eff]), %%xmm2")      /* xmm2 = hsla */ \
    __ASM_EMIT("andps           %%xmm5, %%xmm1")            /* xmm1 = V & [ 0 >= T - V] */ \
    __ASM_EMIT("andnps          %%xmm0, %%xmm3")            /* xmm3 = A = (T-V)*KT & [ 0 < T - V] */ \
    __ASM_EMIT("andnps          %%xmm6, %%xmm5")            /* xmm5 = T & [0 < T-V] */ \
    __ASM_EMIT("movaps          %%xmm2, %%xmm0")            /* xmm0 = hsla */ \
    __ASM_EMIT("orps            %%xmm5, %%xmm1")            /* xmm1 = KS = (V & [ 0 >= T - V]) | (T & [0 < T-V]) */ \
    __ASM_EMIT("movaps          %%xmm2, %%xmm4")            /* xmm4 = hsla */ \
    __ASM_EMIT("shufps          $0x00, %%xmm0, %%xmm0")     /* xmm0 = H */ \
    __ASM_EMIT("shufps          $0x55, %%xmm4, %%xmm4")     /* xmm4 = s */ \
    __ASM_EMIT("shufps          $0xaa, %%xmm2, %%xmm2")     /* xmm2 = L */ \
    __ASM_EMIT("mulps           %%xmm4, %%xmm1")            /* xmm1 = S = s * KS */ \
    \
    X4_TRANSPOSE

    /*
        kt      = 1.0f / eff->thresh;
        value   = (value >= 0.0f) ? value : -value;

        if (0 >= (eff->thresh - value))
        {
            dst[1]      = eff->s * value;
            dst[3]      = 0.0f;
        }
        else
        {
            dst[1]      = eff->s * eff->thresh;
            dst[3]      = (eff->thresh - value) * kt;
        }

        dst[0]      = eff->h;
        dst[2]      = eff->l;
     */

    void eff_hsla_sat(float *dst, const float *v, const dsp::hsla_sat_eff_t *eff, size_t count)
    {
        ARCH_X86_ASM(
            __ASM_EMIT("movss           0x10(%[eff]), %%xmm6")      /* xmm6 = t 0 0 0 */
            __ASM_EMIT("movaps          0x00 + %[XC], %%xmm7")      /* xmm7 = 1 */
            __ASM_EMIT("shufps          $0x00, %%xmm6, %%xmm6")     /* xmm6 = T */
            __ASM_EMIT("divps           %%xmm6, %%xmm7")            /* xmm7 = KT = 1 / t */

            __ASM_EMIT("sub             $4, %[count]")
            __ASM_EMIT("jb              2f")

            //-----------------------------------------------------------------
            // 4x blocks
            __ASM_EMIT("1:")

            __ASM_EMIT("movups          0x00(%[src]), %%xmm1")      /* xmm1 = v */
            EFF_HSLA_SAT_CORE

            __ASM_EMIT("movups          %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("movups          %%xmm1, 0x10(%[dst])")
            __ASM_EMIT("movups          %%xmm2, 0x20(%[dst])")
            __ASM_EMIT("movups          %%xmm3, 0x30(%[dst])")

            __ASM_EMIT("add             $0x10, %[src]")
            __ASM_EMIT("add             $0x40, %[dst]")
            __ASM_EMIT("sub             $4, %[count]")
            __ASM_EMIT("jae             1b")

            __ASM_EMIT("2:")
            __ASM_EMIT("add             $4, %[count]")
            __ASM_EMIT("jle             10f")

            //-----------------------------------------------------------------
            // 1x - 3x block
            // Load last variable-sized chunk
            __ASM_EMIT("test            $1, %[count]")
            __ASM_EMIT("jz              4f")
            __ASM_EMIT("movss           0x00(%[src]), %%xmm1")
            __ASM_EMIT("add             $0x04, %[src]")
            __ASM_EMIT("movlhps         %%xmm1, %%xmm1")
            __ASM_EMIT("4:")
            __ASM_EMIT("test            $2, %[count]")
            __ASM_EMIT("jz              6f")
            __ASM_EMIT("movlps          0x00(%[src]), %%xmm1")
            __ASM_EMIT("6:")

            EFF_HSLA_SAT_CORE

            // Store last chunk
            __ASM_EMIT("test            $1, %[count]")
            __ASM_EMIT("jz              8f")
            __ASM_EMIT("movups          %%xmm2, 0x00(%[dst])")
            __ASM_EMIT("add             $0x10, %[dst]")
            __ASM_EMIT("8:")
            __ASM_EMIT("test            $2, %[count]")
            __ASM_EMIT("jz              10f")
            __ASM_EMIT("movups          %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("movups          %%xmm1, 0x10(%[dst])")

            __ASM_EMIT("10:")

            : [dst] "+r" (dst), [src] "+r" (v), [count] "+r" (count)
            : [eff] "r" (eff),
              [XC] "o" (EFF_HSLA_SAT_XC)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

#undef EFF_HSLA_SAT_CORE

    static const uint32_t EFF_HSLA_LIGHT_XC[] __lsp_aligned16 =
    {
        0x3f800000, 0x3f800000, 0x3f800000, 0x3f800000,
        0x7fffffff, 0x7fffffff, 0x7fffffff, 0x7fffffff
    };

#define EFF_HSLA_LIGHT_CORE \
    /* xmm2 = v, xmm6 = T, xmm7 = KT */ \
    __ASM_EMIT("movaps          %%xmm6, %%xmm0")            /* xmm0 = T */ \
    __ASM_EMIT("andps           0x10 + %[XC], %%xmm2")      /* xmm2 = V */ \
    __ASM_EMIT("xorps           %%xmm5, %%xmm5")            /* xmm5 = 0 */ \
    __ASM_EMIT("subps           %%xmm2, %%xmm0")            /* xmm0 = T-V */ \
    __ASM_EMIT("cmpps           $6, %%xmm0, %%xmm5")        /* xmm5 = [ 0 >= T - V] */ \
    __ASM_EMIT("mulps           %%xmm7, %%xmm0")            /* xmm0 = (T-V)*KT */ \
    __ASM_EMIT("movaps          %%xmm5, %%xmm3")            /* xmm3 = [ 0 >= T - V] */ \
    __ASM_EMIT("movups          0x00(%[eff]), %%xmm1")      /* xmm1 = hsla */ \
    __ASM_EMIT("andps           %%xmm5, %%xmm2")            /* xmm2 = V & [ 0 >= T - V] */ \
    __ASM_EMIT("andnps          %%xmm0, %%xmm3")            /* xmm3 = A = (T-V)*KT & [ 0 < T - V] */ \
    __ASM_EMIT("andnps          %%xmm6, %%xmm5")            /* xmm5 = T & [0 < T-V] */ \
    __ASM_EMIT("movaps          %%xmm1, %%xmm0")            /* xmm0 = hsla */ \
    __ASM_EMIT("orps            %%xmm5, %%xmm2")            /* xmm2 = KL = (V & [ 0 >= T - V]) | (T & [0 < T-V]) */ \
    __ASM_EMIT("movaps          %%xmm1, %%xmm4")            /* xmm4 = hsla */ \
    __ASM_EMIT("shufps          $0x00, %%xmm0, %%xmm0")     /* xmm0 = H */ \
    __ASM_EMIT("shufps          $0xaa, %%xmm4, %%xmm4")     /* xmm4 = l */ \
    __ASM_EMIT("shufps          $0x55, %%xmm1, %%xmm1")     /* xmm1 = S */ \
    __ASM_EMIT("mulps           %%xmm4, %%xmm2")            /* xmm2 = L = l * KL */ \
    \
    X4_TRANSPOSE

    /*
        value   = (value >= 0.0f) ? value : -value;

        if (0 >= (eff->thresh - value))
        {
            dst[2]      = eff->l * value;
            dst[3]      = 0.0f;
        }
        else
        {
            dst[2]      = eff->l * eff->thresh;
            dst[3]      = (eff->thresh - value) * kt;
        }

        dst[0]      = eff->h;
        dst[1]      = eff->s;
     */

    void eff_hsla_light(float *dst, const float *v, const dsp::hsla_light_eff_t *eff, size_t count)
    {
        ARCH_X86_ASM(
            __ASM_EMIT("movss           0x10(%[eff]), %%xmm6")      /* xmm6 = t 0 0 0 */
            __ASM_EMIT("movaps          0x00 + %[XC], %%xmm7")      /* xmm7 = 1 */
            __ASM_EMIT("shufps          $0x00, %%xmm6, %%xmm6")     /* xmm6 = T */
            __ASM_EMIT("divps           %%xmm6, %%xmm7")            /* xmm7 = KT = 1 / t */

            __ASM_EMIT("sub             $4, %[count]")
            __ASM_EMIT("jb              2f")

            //-----------------------------------------------------------------
            // 4x blocks
            __ASM_EMIT("1:")

            __ASM_EMIT("movups          0x00(%[src]), %%xmm2")      /* xmm1 = v */
            EFF_HSLA_LIGHT_CORE

            __ASM_EMIT("movups          %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("movups          %%xmm1, 0x10(%[dst])")
            __ASM_EMIT("movups          %%xmm2, 0x20(%[dst])")
            __ASM_EMIT("movups          %%xmm3, 0x30(%[dst])")

            __ASM_EMIT("add             $0x10, %[src]")
            __ASM_EMIT("add             $0x40, %[dst]")
            __ASM_EMIT("sub             $4, %[count]")
            __ASM_EMIT("jae             1b")

            __ASM_EMIT("2:")
            __ASM_EMIT("add             $4, %[count]")
            __ASM_EMIT("jle             10f")

            //-----------------------------------------------------------------
            // 1x - 3x block
            // Load last variable-sized chunk
            __ASM_EMIT("test            $1, %[count]")
            __ASM_EMIT("jz              4f")
            __ASM_EMIT("movss           0x00(%[src]), %%xmm2")
            __ASM_EMIT("add             $0x04, %[src]")
            __ASM_EMIT("movlhps         %%xmm2, %%xmm2")
            __ASM_EMIT("4:")
            __ASM_EMIT("test            $2, %[count]")
            __ASM_EMIT("jz              6f")
            __ASM_EMIT("movlps          0x00(%[src]), %%xmm2")
            __ASM_EMIT("6:")

            EFF_HSLA_LIGHT_CORE

            // Store last chunk
            __ASM_EMIT("test            $1, %[count]")
            __ASM_EMIT("jz              8f")
            __ASM_EMIT("movups          %%xmm2, 0x00(%[dst])")
            __ASM_EMIT("add             $0x10, %[dst]")
            __ASM_EMIT("8:")
            __ASM_EMIT("test            $2, %[count]")
            __ASM_EMIT("jz              10f")
            __ASM_EMIT("movups          %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("movups          %%xmm1, 0x10(%[dst])")

            __ASM_EMIT("10:")

            : [dst] "+r" (dst), [src] "+r" (v), [count] "+r" (count)
            : [eff] "r" (eff),
              [XC] "o" (EFF_HSLA_LIGHT_XC)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

#undef EFF_HSLA_LIGHT_CORE

}

#undef X4_TRANSPOSE

#endif /* DSP_ARCH_X86_SSE_GRAPHICS_EFFECTS_H_ */
