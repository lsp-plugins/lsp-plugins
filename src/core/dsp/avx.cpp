/*
 * avx256.cpp
 *
 *  Created on: 09 марта 2016 г.
 *      Author: sadko
 */

#include <core/types.h>
#include <core/debug.h>
#include <core/dsp.h>
#include <core/bits.h>

#include <core/x86/features.h>

#if defined(__AVX__) && defined(LSP_NO_AVX)
    #undef __AVX__
#endif

#ifdef __AVX__
    #ifndef ARCH_X86_64
        #undef __AVX__
    #endif /* ARCH_X86_64 */
#endif

#include <core/dsp.h>

#define CORE_X86_AVX_IMPL

namespace lsp
{
    namespace x86
    {
        #include <core/x86/avx/xcr.h>
    }
}

#ifdef __AVX__
namespace lsp
{
    namespace avx
    {
        #include <core/x86/avx/const.h>
        #include <core/x86/avx/vector.h>
        #include <core/x86/avx/filters/static.h>
        #include <core/x86/avx/filters/dynamic.h>
        #include <core/x86/avx/filters/transform.h>
    //    #include <core/x86/avx/graphics.h>

    }
}
#endif /* __AVX__*/

#undef CORE_X86_AVX_IMPL

namespace lsp
{
    namespace avx
    {
        using namespace x86;

        void dsp_init(const cpu_features_t *f)
        {
            #ifdef __AVX__
            if (!(f->features & CPU_OPTION_AVX))
                return;

            lsp_trace("Optimizing DSP for AVX instruction set");

//            dsp::add_multiplied             = avx::add_multiplied;
//            dsp::biquad_process_x1          = avx::biquad_process_x1;
            dsp::biquad_process_x8          = avx::biquad_process_x8;
            dsp::dyn_biquad_process_x8      = avx::dyn_biquad_process_x8;

            // This routine sucks on AMD Bulldozer processor family but is pretty great on Intel
            // Not tested on AMD Processors above Bulldozer family
            if (f->vendor == CPU_VENDOR_INTEL)
                dsp::bilinear_transform_x8      = avx::x64_bilinear_transform_x8;

            if (f->features & CPU_OPTION_FMA3)
            {
                lsp_trace("Optimizing DSP for FMA3 instruction set");
//                dsp::biquad_process_x1          = avx::biquad_process_x1_fma3;
                dsp::biquad_process_x8          = avx::biquad_process_x8_fma3;
                dsp::dyn_biquad_process_x8      = avx::dyn_biquad_process_x8_fma3;
            }
            #endif /* __AVX__ */
        }
    }
}

