/*
 * sse2.cpp
 *
 *  Created on: 9 окт. 2018 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <dsp/bits.h>
#include <core/types.h>
#include <core/debug.h>
#include <test/test.h>

#include <dsp/arch/x86/features.h>

#define DSP_ARCH_X86_SSE2_IMPL

namespace sse2 // TODO: make constants common for all architectures
{
    //-------------------------------------------------------------------------
    // Constants definition
    #define DSP_F32VEC4(name, v)        static const float name[] __lsp_aligned16          = { v, v, v, v }
    #define DSP_U32VEC4(name, v)        static const uint32_t name[] __lsp_aligned16       = { uint32_t(v), uint32_t(v), uint32_t(v), uint32_t(v) }

    #define DSP_F32VECX4(name, a, b, c, d)  static const float name[] __lsp_aligned16      = { a, b, c, d }
    #define DSP_U32VECX4(name, a, b, c, d)  static const uint32_t name[] __lsp_aligned16   = { uint32_t(a), uint32_t(b), uint32_t(c), uint32_t(d) }

    #define DSP_F32REP4(v)              v, v, v, v
    #define DSP_U32REP4(v)              uint32_t(v), uint32_t(v), uint32_t(v), uint32_t(v)

    #define DSP_F32ARRAY(name, ...)     static const float name[] __lsp_aligned16          = { __VA_ARGS__ }
    #define DSP_U32ARRAY(name, ...)     static const uint32_t name[] __lsp_aligned16       = { __VA_ARGS__ }

    #include <dsp/common/const/const16.h>

    #undef DSP_F32ARRAY_IMPL
    #undef DSP_F32ARRAY

    #undef DSP_U32REP4
    #undef DSP_F32REP4

    #undef DSP_U32VECX4
    #undef DSP_F32VECX4

    #undef DSP_U32VEC4
    #undef DSP_F32VEC4
}

#include <dsp/arch/x86/sse2/float.h>
#include <dsp/arch/x86/sse2/search.h>
#include <dsp/arch/x86/sse2/graphics.h>
#include <dsp/arch/x86/sse2/graphics/effects.h>

#undef DSP_ARCH_X86_SSE2_IMPL

namespace sse2
{
    using namespace x86;

#define EXPORT2(function, export)           dsp::function = sse2::export; TEST_EXPORT(sse2::export);
#define EXPORT1(function)                   EXPORT2(function, function);

    void dsp_init(const cpu_features_t *f)
    {
        if (((f->features) & (CPU_OPTION_SSE | CPU_OPTION_SSE2)) != (CPU_OPTION_SSE | CPU_OPTION_SSE2))
            return;

        lsp_trace("Optimizing DSP for SSE2 instruction set");

        EXPORT1(copy_saturated);
        EXPORT1(saturate);
        EXPORT1(limit_saturate1);
        EXPORT1(limit_saturate2);

        EXPORT1(min_index);
        EXPORT1(max_index);
        EXPORT1(minmax_index);

        EXPORT1(abs_min_index);
        EXPORT1(abs_max_index);
        EXPORT1(abs_minmax_index);

        EXPORT1(hsla_to_rgba);
        EXPORT1(rgba_to_hsla);
        EXPORT1(rgba_to_bgra32);

        EXPORT1(eff_hsla_hue);
        EXPORT1(eff_hsla_sat);
        EXPORT1(eff_hsla_light);
        EXPORT1(eff_hsla_alpha);
    }

    #undef EXPORT1
    #undef EXPORT2
}
