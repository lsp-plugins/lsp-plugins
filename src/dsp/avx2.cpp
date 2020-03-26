/*
 * avx2.cpp
 *
 *  Created on: 17 дек. 2018 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <test/test.h>

#include <core/types.h>
#include <core/debug.h>

#include <dsp/arch/x86/features.h>

#define DSP_ARCH_X86_AVX2_IMPL

#include <dsp/arch/x86/avx2/float.h>

#include <dsp/arch/x86/avx2/pmath/op_kx.h>
#include <dsp/arch/x86/avx2/pmath/fmop_kx.h>
#include <dsp/arch/x86/avx2/pmath/exp.h>
#include <dsp/arch/x86/avx2/pmath/log.h>
#include <dsp/arch/x86/avx2/pmath/pow.h>

#include <dsp/arch/x86/avx2/fft/normalize.h>

#include <dsp/arch/x86/avx2/search/iminmax.h>

#include <dsp/arch/x86/avx2/graphics/transpose.h>
#include <dsp/arch/x86/avx2/graphics/effects.h>

#undef DSP_ARCH_X86_AVX2_IMPL

namespace avx2
{
    using namespace x86;

    #define EXPORT2(function, export)               { dsp::function = avx2::export; TEST_EXPORT(avx2::export); }
    #define EXPORT1(function)                       EXPORT2(function, function)

    #define EXPORT2_X64(function, export)           IF_ARCH_X86_64(EXPORT2(function, export));
    #define SUPPORT_X64(function)                   IF_ARCH_X86_64(TEST_EXPORT(avx2::function))

    #define CEXPORT2(cond, function, export)    \
    IF_ARCH_X86( \
            TEST_EXPORT(avx2::export); \
            if (cond) \
                dsp::function = avx2::export; \
        );

    #define CEXPORT1(cond, export)    \
    IF_ARCH_X86( \
            TEST_EXPORT(avx2::export); \
            if (cond) \
                dsp::export = avx2::export; \
        );

    #define CEXPORT2_X64(cond, function, export)    \
        IF_ARCH_X86_64( \
                TEST_EXPORT(avx2::export); \
                if (cond) \
                    dsp::function = avx2::export; \
            );

    #define CEXPORT1_X64(cond, export)    \
        IF_ARCH_X86_64( \
                TEST_EXPORT(avx2::export); \
                if (cond) \
                    dsp::export = avx2::export; \
            );

    void dsp_init(const cpu_features_t *f)
    {
        if ((f->features & (CPU_OPTION_AVX | CPU_OPTION_AVX2)) != (CPU_OPTION_AVX | CPU_OPTION_AVX2))
            return;

        lsp_trace("Optimizing DSP for AVX2 instruction set");

        bool favx   = feature_check(f, FEAT_FAST_AVX);

        CEXPORT1(favx, limit_saturate1);
        CEXPORT1(favx, limit_saturate2);
        CEXPORT1(favx, copy_saturated);
        CEXPORT1(favx, saturate);
        CEXPORT1(favx, sanitize1);
        CEXPORT1(favx, sanitize2);

        CEXPORT1(favx, add_k2);
        CEXPORT1(favx, sub_k2);
        CEXPORT1(favx, rsub_k2);
        CEXPORT1(favx, mul_k2);
        CEXPORT1(favx, div_k2);
        CEXPORT1(favx, rdiv_k2);
        CEXPORT1(favx, mod_k2);
        CEXPORT1(favx, rmod_k2);

        CEXPORT1(favx, add_k3);
        CEXPORT1(favx, sub_k3);
        CEXPORT1(favx, rsub_k3);
        CEXPORT1(favx, mul_k3);
        CEXPORT1(favx, div_k3);
        CEXPORT1(favx, rdiv_k3);
        CEXPORT1(favx, mod_k3);
        CEXPORT1(favx, rmod_k3);

        CEXPORT1(favx, fmadd_k3);
        CEXPORT1(favx, fmsub_k3);
        CEXPORT1(favx, fmrsub_k3);
        CEXPORT1(favx, fmmul_k3);
        CEXPORT1(favx, fmdiv_k3);
        CEXPORT1(favx, fmrdiv_k3);
        CEXPORT1(favx, fmmod_k3);
        CEXPORT1(favx, fmrmod_k3);

        CEXPORT1(favx, fmadd_k4);
        CEXPORT1(favx, fmsub_k4);
        CEXPORT1(favx, fmrsub_k4);
        CEXPORT1(favx, fmmul_k4);
        CEXPORT1(favx, fmdiv_k4);
        CEXPORT1(favx, fmrdiv_k4);
        CEXPORT1(favx, fmmod_k4);
        CEXPORT1(favx, fmrmod_k4);

        CEXPORT2_X64(favx, exp1, x64_exp1);
        CEXPORT2_X64(favx, exp2, x64_exp2);

        CEXPORT2_X64(favx, logb1, x64_logb1);
        CEXPORT2_X64(favx, logb2, x64_logb2);
        CEXPORT2_X64(favx, loge1, x64_loge1);
        CEXPORT2_X64(favx, loge2, x64_loge2);
        CEXPORT2_X64(favx, logd1, x64_logd1);
        CEXPORT2_X64(favx, logd2, x64_logd2);

        CEXPORT2_X64(favx, powcv1, x64_powcv1);
        CEXPORT2_X64(favx, powcv2, x64_powcv2);
        CEXPORT2_X64(favx, powvc1, x64_powvc1);
        CEXPORT2_X64(favx, powvc2, x64_powvc2);
        CEXPORT2_X64(favx, powvx1, x64_powvx1);
        CEXPORT2_X64(favx, powvx2, x64_powvx2);

        CEXPORT2_X64(favx, eff_hsla_hue, x64_eff_hsla_hue);
        CEXPORT2_X64(favx, eff_hsla_sat, x64_eff_hsla_sat);
        CEXPORT2_X64(favx, eff_hsla_light, x64_eff_hsla_light);
        CEXPORT2_X64(favx, eff_hsla_alpha, x64_eff_hsla_alpha);

        CEXPORT1(favx, normalize_fft2);
        CEXPORT1(favx, normalize_fft3);

        if (f->features & CPU_OPTION_FMA3)
        {
            CEXPORT2_X64(favx, mod_k2, mod_k2_fma3);
            CEXPORT2_X64(favx, rmod_k2, rmod_k2_fma3);

            CEXPORT2_X64(favx, mod_k3, mod_k3_fma3);
            CEXPORT2_X64(favx, rmod_k3, rmod_k3_fma3);

            CEXPORT2_X64(favx, fmadd_k3, fmadd_k3_fma3);
            CEXPORT2_X64(favx, fmsub_k3, fmsub_k3_fma3);
            CEXPORT2_X64(favx, fmrsub_k3, fmrsub_k3_fma3);
            CEXPORT2_X64(favx, fmmod_k3, fmmod_k3_fma3);
            CEXPORT2_X64(favx, fmrmod_k3, fmrmod_k3_fma3);

            CEXPORT2_X64(favx, fmadd_k4, fmadd_k4_fma3);
            CEXPORT2_X64(favx, fmsub_k4, fmsub_k4_fma3);
            CEXPORT2_X64(favx, fmrsub_k4, fmrsub_k4_fma3);
            CEXPORT2_X64(favx, fmmod_k4, fmmod_k4_fma3);
            CEXPORT2_X64(favx, fmrmod_k4, fmrmod_k4_fma3);

            CEXPORT2_X64(favx, exp1, x64_exp1_fma3);
            CEXPORT2_X64(favx, exp2, x64_exp2_fma3);

            CEXPORT2_X64(favx, logb1, x64_logb1_fma3);
            CEXPORT2_X64(favx, logb2, x64_logb2_fma3);
            CEXPORT2_X64(favx, loge1, x64_loge1_fma3);
            CEXPORT2_X64(favx, loge2, x64_loge2_fma3);
            CEXPORT2_X64(favx, logd1, x64_logd1_fma3);
            CEXPORT2_X64(favx, logd2, x64_logd2_fma3);

            CEXPORT2_X64(favx, powcv1, x64_powcv1_fma3);
            CEXPORT2_X64(favx, powcv2, x64_powcv2_fma3);
            CEXPORT2_X64(favx, powvc1, x64_powvc1_fma3);
            CEXPORT2_X64(favx, powvc2, x64_powvc2_fma3);
            CEXPORT2_X64(favx, powvx1, x64_powvx1_fma3);
            CEXPORT2_X64(favx, powvx2, x64_powvx2_fma3);
        }
    }
}
