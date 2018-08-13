/*
 * avx256.cpp
 *
 *  Created on: 09 марта 2016 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <core/types.h>
#include <core/debug.h>

#include <dsp/arch/x86/features.h>

#if defined(__AVX__) && defined(LSP_NO_AVX)
    #undef __AVX__
#endif

#ifdef __AVX__
    #ifndef ARCH_X86_64
        #undef __AVX__
    #endif /* ARCH_X86_64 */
#endif

#define DSP_ARCH_X86_AVX_IMPL

namespace x86
{
    #include <dsp/arch/x86/avx/xcr.h>
}

#ifdef __AVX__
namespace avx
{
    #include <dsp/arch/x86/avx/const.h>
    #include <dsp/arch/x86/avx/vector.h>
    #include <dsp/arch/x86/avx/filters/static.h>
    #include <dsp/arch/x86/avx/filters/dynamic.h>
    #include <dsp/arch/x86/avx/filters/transform.h>
//    #include <dsp/arch/x86/avx/graphics.h>

}
#endif /* __AVX__*/

#undef DSP_ARCH_X86_AVX_IMPL

namespace avx
{
    using namespace x86;

    #define EXPORT2(function, export)           dsp::function = avx::export
    #define EXPORT1(function)                   EXPORT2(function, function)

    void dsp_init(const cpu_features_t *f)
    {
        #ifdef __AVX__
        if (!(f->features & CPU_OPTION_AVX))
            return;

        lsp_trace("Optimizing DSP for AVX instruction set");

//            EXPORT1(add_multiplied);
//            EXPORT1(biquad_process_x1);
        EXPORT1(biquad_process_x8);
        EXPORT1(dyn_biquad_process_x8);

        // This routine sucks on AMD Bulldozer processor family but is pretty great on Intel
        // Not tested on AMD Processors above Bulldozer family
        if (f->vendor == CPU_VENDOR_INTEL)
            EXPORT2(bilinear_transform_x8, x64_bilinear_transform_x8);

        if (f->features & CPU_OPTION_FMA3)
        {
            lsp_trace("Optimizing DSP for FMA3 instruction set");
//                dsp::biquad_process_x1          = avx::biquad_process_x1_fma3;
            EXPORT2(biquad_process_x8, biquad_process_x8_fma3);
            EXPORT2(dyn_biquad_process_x8, dyn_biquad_process_x8_fma3);
        }
        #endif /* __AVX__ */
    }

    #undef EXPORT1
    #undef EXPORT2
}

