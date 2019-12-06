/*
 * avx.cpp
 *
 *  Created on: 09 марта 2016 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <test/test.h>

#include <core/types.h>
#include <core/debug.h>

#include <dsp/arch/x86/features.h>

#define DSP_ARCH_X86_AVX_IMPL

#include <dsp/arch/x86/avx/xcr.h>
#include <dsp/arch/x86/avx/const.h>

#include <dsp/arch/x86/avx/copy.h>
#include <dsp/arch/x86/avx/complex.h>
#include <dsp/arch/x86/avx/pcomplex.h>

#include <dsp/arch/x86/avx/pmath/op_kx.h>
#include <dsp/arch/x86/avx/pmath/op_vv.h>
#include <dsp/arch/x86/avx/pmath/fmop_kx.h>
#include <dsp/arch/x86/avx/pmath/fmop_vv.h>
#include <dsp/arch/x86/avx/pmath/abs_vv.h>

#include <dsp/arch/x86/avx/hmath/hsum.h>
#include <dsp/arch/x86/avx/hmath/hdotp.h>

#include <dsp/arch/x86/avx/mix.h>
#include <dsp/arch/x86/avx/search/minmax.h>

#include <dsp/arch/x86/avx/filters/static.h>
#include <dsp/arch/x86/avx/filters/dynamic.h>
#include <dsp/arch/x86/avx/filters/transform.h>

#undef DSP_ARCH_X86_AVX_IMPL

namespace avx
{
    using namespace x86;

    #define EXPORT2(function, export)               { dsp::function = avx::export; TEST_EXPORT(avx::export); }
    #define EXPORT1(function)                       EXPORT2(function, function)

    #define EXPORT2_X64(function, export)           IF_ARCH_X86_64(EXPORT2(function, export));
    #define SUPPORT_X64(function)                   IF_ARCH_X86_64(TEST_EXPORT(avx::function))

    #define CEXPORT2(cond, function, export)    \
    IF_ARCH_X86( \
            TEST_EXPORT(avx::export); \
            if (cond) \
                dsp::function = avx::export; \
        );

    #define CEXPORT1(cond, export)    \
    IF_ARCH_X86( \
            TEST_EXPORT(avx::export); \
            if (cond) \
                dsp::export = avx::export; \
        );

    #define CEXPORT2_X64(cond, function, export)    \
        IF_ARCH_X86_64( \
                TEST_EXPORT(avx::export); \
                if (cond) \
                    dsp::function = avx::export; \
            );

    #define CEXPORT1_X64(cond, export)    \
        IF_ARCH_X86_64( \
                TEST_EXPORT(avx::export); \
                if (cond) \
                    dsp::export = avx::export; \
            );

    void dsp_init(const cpu_features_t *f)
    {
        if (!(f->features & CPU_OPTION_AVX))
            return;

        lsp_trace("Optimizing DSP for AVX instruction set");

        EXPORT2_X64(biquad_process_x8, x64_biquad_process_x8);
        EXPORT2_X64(dyn_biquad_process_x8, x64_dyn_biquad_process_x8);

        TEST_EXPORT(avx::copy);

        // This routine sucks on AMD Bulldozer processor family but is pretty great on Intel
        // Not tested on AMD Processors above Bulldozer family
        bool favx   = feature_check(f, FEAT_FAST_AVX);

        CEXPORT2_X64(favx, reverse1, reverse1);
        CEXPORT2_X64(favx, reverse2, reverse2);

        // Conditional export, depending on fast AVX implementation
        CEXPORT1(favx, add_k2);
        CEXPORT1(favx, sub_k2);
        CEXPORT1(favx, rsub_k2);
        CEXPORT1(favx, mul_k2);
        CEXPORT1(favx, div_k2);
        CEXPORT1(favx, rdiv_k2);

        CEXPORT1(favx, add_k3);
        CEXPORT1(favx, sub_k3);
        CEXPORT1(favx, rsub_k3);
        CEXPORT1(favx, mul_k3);
        CEXPORT1(favx, div_k3);
        CEXPORT1(favx, rdiv_k3);

        CEXPORT1(favx, add2);
        CEXPORT1(favx, sub2);
        CEXPORT1(favx, rsub2);
        CEXPORT1(favx, mul2);
        CEXPORT1(favx, div2);
        CEXPORT1(favx, rdiv2);

        CEXPORT1(favx, add3);
        CEXPORT1(favx, sub3);
        CEXPORT1(favx, mul3);
        CEXPORT1(favx, div3);

        CEXPORT2_X64(favx, fmadd_k3, x64_fmadd_k3);
        CEXPORT2_X64(favx, fmsub_k3, x64_fmsub_k3);
        CEXPORT2_X64(favx, fmrsub_k3, x64_fmrsub_k3);
        CEXPORT2_X64(favx, fmmul_k3, x64_fmmul_k3);
        CEXPORT2_X64(favx, fmdiv_k3, x64_fmdiv_k3);
        CEXPORT2_X64(favx, fmrdiv_k3, x64_fmrdiv_k3);

        CEXPORT2_X64(favx, fmadd_k4, x64_fmadd_k4);
        CEXPORT2_X64(favx, fmsub_k4, x64_fmsub_k4);
        CEXPORT2_X64(favx, fmrsub_k4, x64_fmrsub_k4);
        CEXPORT2_X64(favx, fmmul_k4, x64_fmmul_k4);
        CEXPORT2_X64(favx, fmdiv_k4, x64_fmdiv_k4);
        CEXPORT2_X64(favx, fmrdiv_k4, x64_fmrdiv_k4);

        CEXPORT2_X64(favx, fmadd3, x64_fmadd3);
        CEXPORT2_X64(favx, fmsub3, x64_fmsub3);
        CEXPORT2_X64(favx, fmrsub3, x64_fmrsub3);
        CEXPORT2_X64(favx, fmmul3, x64_fmmul3);
        CEXPORT2_X64(favx, fmdiv3, x64_fmdiv3);
        CEXPORT2_X64(favx, fmrdiv3, x64_fmrdiv3);

        CEXPORT2_X64(favx, fmadd4, x64_fmadd4);
        CEXPORT2_X64(favx, fmsub4, x64_fmsub4);
        CEXPORT2_X64(favx, fmrsub4, x64_fmrsub4);
        CEXPORT2_X64(favx, fmmul4, x64_fmmul4);
        CEXPORT2_X64(favx, fmdiv4, x64_fmdiv4);
        CEXPORT2_X64(favx, fmrdiv4, x64_fmrdiv4);

        CEXPORT2_X64(favx, abs_add2, x64_abs_add2);
        CEXPORT2_X64(favx, abs_sub2, x64_abs_sub2);
        CEXPORT2_X64(favx, abs_rsub2, x64_abs_rsub2);
        CEXPORT2_X64(favx, abs_mul2, x64_abs_mul2);
        CEXPORT2_X64(favx, abs_div2, x64_abs_div2);
        CEXPORT2_X64(favx, abs_rdiv2, x64_abs_rdiv2);

        CEXPORT2_X64(favx, abs_add3, x64_abs_add3);
        CEXPORT2_X64(favx, abs_sub3, x64_abs_sub3);
        CEXPORT2_X64(favx, abs_rsub3, x64_abs_rsub3);
        CEXPORT2_X64(favx, abs_mul3, x64_abs_mul3);
        CEXPORT2_X64(favx, abs_div3, x64_abs_div3);
        CEXPORT2_X64(favx, abs_rdiv3, x64_abs_rdiv3);

        CEXPORT2_X64(favx, abs1, x64_abs1);
        CEXPORT2_X64(favx, abs2, x64_abs2);

        CEXPORT2_X64(favx, complex_mul3, x64_complex_mul3);

        CEXPORT2(favx, pcomplex_mul2, pcomplex_mul2);
        CEXPORT2(favx, pcomplex_mul3, pcomplex_mul3);
        CEXPORT2_X64(favx, pcomplex_mod, x64_pcomplex_mod);

        CEXPORT2_X64(favx, bilinear_transform_x8, x64_bilinear_transform_x8);

        CEXPORT1(favx, h_sum);
        CEXPORT1(favx, h_sqr_sum);
        CEXPORT1(favx, h_abs_sum);

        CEXPORT1(favx, h_dotp);
        CEXPORT1(favx, h_sqr_dotp);
        CEXPORT1(favx, h_abs_dotp);

        CEXPORT1(favx, mix2);
        CEXPORT1(favx, mix_copy2);
        CEXPORT1(favx, mix_add2);
        CEXPORT1(favx, mix3);
        CEXPORT1(favx, mix_copy3);
        CEXPORT1(favx, mix_add3);
        CEXPORT1(favx, mix4);
        CEXPORT1(favx, mix_copy4);
        CEXPORT1(favx, mix_add4);

        CEXPORT1(favx, min);
        CEXPORT1(favx, max);
        CEXPORT1(favx, minmax);
        CEXPORT1(favx, abs_min);
        CEXPORT1(favx, abs_max);
        CEXPORT1(favx, abs_minmax);

        // FMA3 support?
        if (f->features & CPU_OPTION_FMA3)
        {
            lsp_trace("Optimizing DSP for FMA3 instruction set");

            // Conditional export, depending on fast AVX implementation
            CEXPORT2_X64(favx, fmadd_k3, x64_fmadd_k3_fma3);
            CEXPORT2_X64(favx, fmsub_k3, x64_fmsub_k3_fma3);
            CEXPORT2_X64(favx, fmrsub_k3, x64_fmrsub_k3_fma3);

            CEXPORT2_X64(favx, fmadd_k4, x64_fmadd_k4_fma3);
            CEXPORT2_X64(favx, fmsub_k4, x64_fmsub_k4_fma3);
            CEXPORT2_X64(favx, fmrsub_k4, x64_fmrsub_k4_fma3);

            CEXPORT2_X64(favx, fmadd3, x64_fmadd3_fma3);
            CEXPORT2_X64(favx, fmsub3, x64_fmsub3_fma3);
            CEXPORT2_X64(favx, fmrsub3, x64_fmrsub3_fma3);

            CEXPORT2_X64(favx, fmadd4, x64_fmadd4_fma3);
            CEXPORT2_X64(favx, fmsub4, x64_fmsub4_fma3);
            CEXPORT2_X64(favx, fmrsub4, x64_fmrsub4_fma3);

            CEXPORT2_X64(favx, complex_mul3, x64_complex_mul3_fma3);

            CEXPORT2(favx, pcomplex_mul2, pcomplex_mul2_fma3);
            CEXPORT2(favx, pcomplex_mul3, pcomplex_mul3_fma3);

            CEXPORT2(favx, h_sqr_sum, h_sqr_sum_fma3);
//            CEXPORT2(favx, h_dotp_sum, h_dotp_sum_fma3);

            // Non-conditional export
            EXPORT2(biquad_process_x8, biquad_process_x8_fma3);
            EXPORT2_X64(dyn_biquad_process_x8, dyn_biquad_process_x8_fma3);
        }
    }

    #undef EXPORT1
    #undef EXPORT2
}

