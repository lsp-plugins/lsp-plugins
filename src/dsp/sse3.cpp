/*
 * sse.cpp
 *
 *  Created on: 04 марта 2016 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <dsp/bits.h>
#include <core/types.h>
#include <core/debug.h>
#include <test/test.h>

#include <dsp/arch/x86/features.h>

#define DSP_ARCH_X86_SSE3_IMPL

namespace sse3
{
    inline bool __lsp_forced_inline sse_aligned(const void *ptr)         { return !(ptrdiff_t(ptr) & (0x0f));  };

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

#include <dsp/arch/x86/sse3/copy.h>
#include <dsp/arch/x86/sse3/graphics.h>
#include <dsp/arch/x86/sse3/filters/static.h>
#include <dsp/arch/x86/sse3/filters/dynamic.h>
#include <dsp/arch/x86/sse3/filters/transform.h>
#include <dsp/arch/x86/sse3/complex.h>
#include <dsp/arch/x86/sse3/3dmath.h>

#undef DSP_ARCH_X86_SSE3_IMPL

namespace sse3
{
    using namespace x86;

    #define EXPORT2(function, export)               dsp::function = sse3::export; TEST_EXPORT(sse3::export);
    #define EXPORT2_X64(function, export)           IF_ARCH_X86_64(dsp::function = sse3::export; TEST_EXPORT(sse3::export));
    #define EXPORT1(export)                         EXPORT2(export, export)
    #define SUPPORT_X64(function)                   IF_ARCH_X86_64(TEST_EXPORT(sse3::function))

    void dsp_init(const cpu_features_t *f)
    {
        if (!(f->features & CPU_OPTION_SSE3))
            return;

        lsp_trace("Optimizing DSP for SSE3 instruction set");

        // Additional xmm registers are available only in 64-bit mode
        EXPORT1(pcomplex_mul2);
        EXPORT1(pcomplex_mul3);

        if (!feature_check(f, FEAT_FAST_MOVS))
        {
            EXPORT2(copy, copy);
        }
        else
        {
            TEST_EXPORT(copy);
        }
        EXPORT2(pcomplex_mod, pcomplex_mod);
        EXPORT2_X64(pcomplex_mod, x64_pcomplex_mod);

//      EXPORT2_X64(biquad_process_x4, x64_biquad_process_x4); // Pure SSE has a bit better throughput for this case
        EXPORT2_X64(biquad_process_x8, x64_biquad_process_x8);
        EXPORT2_X64(dyn_biquad_process_x8, x64_dyn_biquad_process_x8);
        EXPORT2_X64(bilinear_transform_x8, x64_bilinear_transform_x8);
        EXPORT2_X64(axis_apply_log1, x64_axis_apply_log1);
        EXPORT2_X64(axis_apply_log2, x64_axis_apply_log2);
        EXPORT2_X64(pcomplex_mul2, x64_pcomplex_mul2);
        EXPORT2_X64(pcomplex_mul3, x64_pcomplex_mul3);

        EXPORT2_X64(rgba32_to_bgra32, x64_rgba32_to_bgra32);

        EXPORT1(calc_split_point_p2v1);
        EXPORT1(calc_split_point_pvv1);

        EXPORT1(colocation_x2_v1p2);
        EXPORT1(colocation_x2_v1pv);
        EXPORT1(colocation_x3_v1p3);
        EXPORT1(colocation_x3_v1pv);
        EXPORT1(colocation_x3_v3p1);
        EXPORT1(colocation_x3_vvp1);

        EXPORT1(calc_min_distance_p3);
        EXPORT1(calc_min_distance_pv);

        EXPORT1(split_triangle_raw);
        EXPORT1(cull_triangle_raw);
    }

    #undef EXPORT2
}
