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

#ifdef ARCH_X86_64_AVX
    #include <dsp/arch/x86/avx/const.h>
    #include <dsp/arch/x86/avx/vector.h>

#endif /* ARCH_X86_64_AVX */

#include <dsp/arch/x86/avx/copy.h>
#include <dsp/arch/x86/avx/complex.h>
#include <dsp/arch/x86/avx/filters/static.h>
#include <dsp/arch/x86/avx/filters/dynamic.h>
#include <dsp/arch/x86/avx/filters/transform.h>

#undef DSP_ARCH_X86_AVX_IMPL

namespace avx
{
    using namespace x86;

    #define EXPORT2(function, export)           { dsp::function = avx::export; TEST_EXPORT(avx::export); }
    #define EXPORT1(function)                   EXPORT2(function, function)

    #define EXPORT2_X64(function, export)       IF_ARCH_X86_64(EXPORT2(function, export));
    #define SUPPORT_X64(function)               IF_ARCH_X86_64(TEST_EXPORT(avx::function))

    void dsp_init(const cpu_features_t *f)
    {
        if (!(f->features & CPU_OPTION_AVX))
            return;

        lsp_trace("Optimizing DSP for AVX instruction set");

//            EXPORT1(add_multiplied);
//            EXPORT1(biquad_process_x1);
        EXPORT2_X64(biquad_process_x8, x64_biquad_process_x8);
        EXPORT2_X64(dyn_biquad_process_x8, x64_dyn_biquad_process_x8);

        TEST_EXPORT(avx::copy);

        // This routine sucks on AMD Bulldozer processor family but is pretty great on Intel
        // Not tested on AMD Processors above Bulldozer family
        if (feature_check(f, FEAT_FAST_AVX))
        {
            EXPORT2_X64(complex_mul3, x64_complex_mul3);
            EXPORT2_X64(pcomplex_mul3, x64_pcomplex_mul3);
            EXPORT2_X64(pcomplex_mod, x64_pcomplex_mod);
            EXPORT2_X64(bilinear_transform_x8, x64_bilinear_transform_x8);
        }
        else
        {
            SUPPORT_X64(x64_pcomplex_mod);
            SUPPORT_X64(x64_complex_mul3);
            SUPPORT_X64(x64_pcomplex_mul3);
            SUPPORT_X64(x64_bilinear_transform_x8);
        }

        if (f->features & CPU_OPTION_FMA3)
        {
            lsp_trace("Optimizing DSP for FMA3 instruction set");

            if (f->vendor == CPU_VENDOR_INTEL)
            {
                EXPORT2_X64(complex_mul3, x64_complex_mul3_fma3);
                EXPORT2_X64(pcomplex_mul3, x64_pcomplex_mul3_fma3)
            }
            else
            {
                SUPPORT_X64(x64_complex_mul3_fma3);
                SUPPORT_X64(x64_pcomplex_mul3_fma3);
            }

//                dsp::biquad_process_x1          = avx::biquad_process_x1_fma3;
            EXPORT2_X64(biquad_process_x8, x64_biquad_process_x8_fma3);
            EXPORT2_X64(dyn_biquad_process_x8, x64_dyn_biquad_process_x8_fma3);
        }
    }

    #undef EXPORT1
    #undef EXPORT2
}

