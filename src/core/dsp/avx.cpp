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

#ifdef __AVX__
    #include <core/dsp.h>
    #include <core/x86/avx/const.h>
    #include <core/x86/avx/vector.h>
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

//            lsp_trace("Optimizing DSP for AVX instruction set");
//
//            dsp::add_multiplied             = avx::add_multiplied;
            #endif /* __AVX__ */
        }
    }
}

