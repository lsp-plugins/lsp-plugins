/*
 * sse.cpp
 *
 *  Created on: 04 марта 2016 г.
 *      Author: sadko
 */

#include <core/types.h>
#include <core/debug.h>
#include <core/dsp.h>
#include <core/x86/float.h>
#include <core/x86/graphics.h>

namespace lsp
{
    namespace x86
    {
        static dsp::start_t     dsp_start       = NULL;
        static dsp::finish_t    dsp_finish      = NULL;

        static void start(dsp_context_t *ctx)
        {
            dsp_start(ctx);
            uint32_t cr                 = fpu_read_cr();
            ctx->data[ctx->top++]       = cr;

            fpu_write_cr(cr);
        }

        static void finish(dsp_context_t *ctx)
        {
            fpu_write_cr(ctx->data[--ctx->top]);
            dsp_finish(ctx);
        }

        void dsp_init(dsp_options_t options)
        {
            lsp_trace("Optimizing system with some assembly code");
            // Save previous entry points
            dsp_start                   = dsp::start;
            dsp_finish                  = dsp::finish;
            dsp::start                  = x86::start;
            dsp::finish                 = x86::finish;

            dsp::copy_saturated         = x86::copy_saturated;
            dsp::saturate               = x86::saturate;
            dsp::rgba32_to_bgra32       = x86::rgba32_to_bgra32;

            if (options & DSP_OPTION_CMOV)
            {
                dsp::copy_saturated         = x86::copy_saturated_cmov;
                dsp::saturate               = x86::saturate_cmov;
            }
        }
    }

}
