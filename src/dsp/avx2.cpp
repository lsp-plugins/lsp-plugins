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

#include <dsp/arch/x86/avx2/pmath/exp.h>
#include <dsp/arch/x86/avx2/pmath/log.h>

#undef DSP_ARCH_X86_AVX2_IMPL

namespace avx2
{
    using namespace x86;

    #define EXPORT2(function, export)           { dsp::function = avx2::export; TEST_EXPORT(avx2::export); }
    #define EXPORT1(function)                   EXPORT2(function, function)

    #define EXPORT2_X64(function, export)       IF_ARCH_X86_64(EXPORT2(function, export));
    #define SUPPORT_X64(function)               IF_ARCH_X86_64(TEST_EXPORT(avx2::function))

    void dsp_init(const cpu_features_t *f)
    {
        if ((f->features & (CPU_OPTION_AVX | CPU_OPTION_AVX2)) != (CPU_OPTION_AVX | CPU_OPTION_AVX2))
            return;

        lsp_trace("Optimizing DSP for AVX2 instruction set");

        if (feature_check(f, FEAT_FAST_AVX))
        {
            EXPORT2_X64(exp1, x64_exp1);
            EXPORT2_X64(exp2, x64_exp2);

            EXPORT2_X64(logb1, x64_logb1);
            EXPORT2_X64(logb2, x64_logb2);
            EXPORT2_X64(loge1, x64_loge1);
            EXPORT2_X64(loge2, x64_loge2);
            EXPORT2_X64(logd1, x64_logd1);
            EXPORT2_X64(logd2, x64_logd2);

            if (f->features & CPU_OPTION_FMA3)
            {
                EXPORT2_X64(exp1, x64_exp1_fma3);
                EXPORT2_X64(exp2, x64_exp2_fma3);
            }
        }
    }
}
