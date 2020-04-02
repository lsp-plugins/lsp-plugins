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

#include <dsp/arch/x86/sse2/search/iminmax.h>

#include <dsp/arch/x86/sse2/graphics.h>
#include <dsp/arch/x86/sse2/graphics/effects.h>
#include <dsp/arch/x86/sse2/graphics/axis.h>

#include <dsp/arch/x86/sse2/pmath/op_kx.h>
#include <dsp/arch/x86/sse2/pmath/op_vv.h>
#include <dsp/arch/x86/sse2/pmath/fmop_kx.h>
#include <dsp/arch/x86/sse2/pmath/fmop_vv.h>
#include <dsp/arch/x86/sse2/pmath/exp.h>
#include <dsp/arch/x86/sse2/pmath/log.h>
#include <dsp/arch/x86/sse2/pmath/pow.h>

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
        EXPORT1(sanitize1);
        EXPORT1(sanitize2);

        EXPORT1(mod_k2);
        EXPORT1(rmod_k2);
        EXPORT1(mod_k3);
        EXPORT1(rmod_k3);

        EXPORT1(fmmod_k3);
        EXPORT1(fmrmod_k3);
        EXPORT1(fmmod_k4);
        EXPORT1(fmrmod_k4);

        EXPORT1(mod2);
        EXPORT1(rmod2);
        EXPORT1(mod3);

        EXPORT1(exp1);
        EXPORT1(exp2);
        EXPORT1(logb1);
        EXPORT1(logb2);
        EXPORT1(loge1);
        EXPORT1(loge2);
        EXPORT1(logd1);
        EXPORT1(logd2);
        EXPORT1(powcv1);
        EXPORT1(powcv2);
        EXPORT1(powvc1);
        EXPORT1(powvc2);
        EXPORT1(powvx1);
        EXPORT1(powvx2);

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

        EXPORT1(axis_apply_log1);
        EXPORT1(axis_apply_log2);
        EXPORT1(rgba32_to_bgra32);
    }

    #undef EXPORT1
    #undef EXPORT2
}
