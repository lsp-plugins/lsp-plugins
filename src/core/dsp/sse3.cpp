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

#include <core/x86/features.h>

#define CORE_X86_SSE_IMPL

namespace lsp
{
    namespace sse
    {
        #include <core/x86/sse/const.h>
    }

    namespace sse3
    {
        using namespace sse;

        #include <core/x86/sse3/graphics.h>

        #include <core/x86/sse3/filters/static.h>
        #include <core/x86/sse3/filters/dynamic.h>
        #include <core/x86/sse3/filters/transform.h>
    }
}


#include <core/x86/sse3/complex.h>

#undef CORE_X86_SSE_IMPL

namespace lsp
{
    namespace sse3
    {
        using namespace x86;

        void dsp_init(const cpu_features_t *f)
        {
            if (!(f->features & CPU_OPTION_SSE3))
                return;

            // Additional xmm registers are available only in 64-bit mode
            #ifdef ARCH_X86_64
                lsp_trace("Optimizing DSP for SSE3 instruction set");

                dsp::biquad_process_x2          = sse3::x64_biquad_process_x2;
//                dsp::biquad_process_x4          = sse3::x64_biquad_process_x4; // Pure SSE has a bit better throughput for this case
                dsp::biquad_process_x8          = sse3::x64_biquad_process_x8;

                dsp::dyn_biquad_process_x8      = sse3::x64_dyn_biquad_process_x8;
                dsp::bilinear_transform_x8      = sse3::x64_bilinear_transform_x8;

                dsp::axis_apply_log             = sse3::x64_axis_apply_log;

                dsp::packed_complex_mul         = sse3::x64_packed_complex_mul;
            #endif /* ARCH_X86_64 */
        }
    }

}
