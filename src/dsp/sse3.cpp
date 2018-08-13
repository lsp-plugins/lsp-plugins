/*
 * sse.cpp
 *
 *  Created on: 04 марта 2016 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <dsp/bits.h>
#include <core/types.h>
#include <core/debug.h>

#include <dsp/arch/x86/features.h>

#define DSP_ARCH_X86_SSE3_IMPL

namespace sse3
{
    inline bool __lsp_forced_inline sse_aligned(const void *ptr)         { return !(ptrdiff_t(ptr) & (0x0f));  };
}

#include <dsp/arch/x86/sse3/graphics.h>
#include <dsp/arch/x86/sse3/filters/static.h>
#include <dsp/arch/x86/sse3/filters/dynamic.h>
#include <dsp/arch/x86/sse3/filters/transform.h>
#include <dsp/arch/x86/sse3/complex.h>

#undef DSP_ARCH_X86_SSE3_IMPL

namespace sse3
{
    using namespace x86;

    #define EXPORT2(function, export)               dsp::function = sse3::export

    void dsp_init(const cpu_features_t *f)
    {
        if (!(f->features & CPU_OPTION_SSE3))
            return;

        // Additional xmm registers are available only in 64-bit mode
        #ifdef ARCH_X86_64
            lsp_trace("Optimizing DSP for SSE3 instruction set");

            EXPORT2(biquad_process_x2, x64_biquad_process_x2);
//                EXPORT2(biquad_process_x4, x64_biquad_process_x4); // Pure SSE has a bit better throughput for this case
            EXPORT2(biquad_process_x8, x64_biquad_process_x8);

            EXPORT2(dyn_biquad_process_x8, x64_dyn_biquad_process_x8);
            EXPORT2(bilinear_transform_x8, x64_bilinear_transform_x8);

            EXPORT2(axis_apply_log, x64_axis_apply_log);

            EXPORT2(packed_complex_mul, x64_packed_complex_mul);
        #endif /* ARCH_X86_64 */
    }

    #undef EXPORT2
}
