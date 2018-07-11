/*
 * sse.cpp
 *
 *  Created on: 04 марта 2016 г.
 *      Author: sadko
 */

#include <core/types.h>
#include <core/debug.h>
#include <core/dsp.h>
#include <core/bits.h>

#include <core/x86/dsp/const.h>
#include <core/x86/sse3/filters.h>

namespace lsp
{
    namespace sse3
    {
        void dsp_init(dsp_options_t options)
        {
            if (!(options & DSP_OPTION_SSE3))
                return;

            // Additional xmm registers are available only in 64-bit mode
            #ifdef ARCH_X86_64
                lsp_trace("Optimizing DSP for SSE3 instruction set");

                dsp::biquad_process_x2          = sse3::x64_biquad_process_x2;
//                dsp::biquad_process_x4          = sse3::x64_biquad_process_x4; // Pure SSE has a bit better throughput for this case
                dsp::biquad_process_x8          = sse3::x64_biquad_process_x8;
            #endif /* ARCH_X86_64 */
        }
    }

}
