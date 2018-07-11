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

#if defined(__AVX__) && defined(LSP_NO_AVX)
    #undef __AVX__
#endif

#ifdef __AVX__
    #ifndef ARCH_X86_64
        #undef __AVX__
    #endif /* ARCH_X86_64 */
#endif

#ifdef __AVX__
    #include <core/dsp.h>
    #include <core/x86/avx/const.h>
    #include <core/x86/avx/vector.h>
    #include <core/x86/avx/filters.h>
//    #include <core/x86/avx/graphics.h>
#endif /* __AVX__*/

namespace lsp
{
    namespace avx
    {
        void dsp_init(dsp_options_t options)
        {
            #ifdef __AVX__
            if (!(options & DSP_OPTION_AVX))
                return;

            lsp_trace("Optimizing DSP for AVX instruction set");

//            dsp::add_multiplied             = avx::add_multiplied;
//            dsp::biquad_process_x1          = avx::biquad_process_x1;
            dsp::biquad_process_x8          = avx::biquad_process_x8;

            if (options & DSP_OPTION_FMA3)
            {
                lsp_trace("Optimizing DSP for FMA3 instruction set");
//                dsp::biquad_process_x1          = avx::biquad_process_x1_fma3;
                dsp::biquad_process_x8          = avx::biquad_process_x8_fma3;
            }
            #endif /* __AVX__ */
        }
    }
}

