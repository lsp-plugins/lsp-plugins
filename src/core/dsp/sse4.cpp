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

#define CORE_X86_SSE4_IMPL
#define CORE_X86_SSE_IMPL

#include <core/x86/sse/const.h>
#include <core/x86/sse4/3dmath.h>

#undef CORE_X86_SSE_IMPL
#undef CORE_X86_SSE4_IMPL

namespace lsp
{
    namespace sse4
    {
        void dsp_init(dsp_options_t options)
        {
            if (!(options & DSP_OPTION_SSE4_1))
                return;

            // Additional xmm registers are available only in 64-bit mode
            lsp_trace("Optimizing DSP for SSE4 instruction set");

            // 3D Math
            dsp::normalize_point            = sse4::normalize_point;
            dsp::scale_point1               = sse4::scale_point1;
            dsp::scale_point2               = sse4::scale_point2;

            dsp::normalize_vector           = sse4::normalize_vector;
            dsp::scale_vector1              = sse4::scale_vector1;
            dsp::scale_vector2              = sse4::scale_vector2;

            dsp::check_point3d_on_triangle_p3p  = sse4::check_point3d_on_triangle_p3p;
            dsp::check_point3d_on_triangle_pvp  = sse4::check_point3d_on_triangle_pvp;
            dsp::check_point3d_on_triangle_tp   = sse4::check_point3d_on_triangle_tp;

            dsp::find_intersection3d_rt     = sse4::find_intersection3d_rt;
        }
    }

}
