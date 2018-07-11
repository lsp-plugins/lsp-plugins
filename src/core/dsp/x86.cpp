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

namespace lsp
{
    namespace x86
    {
        void dsp_init(dsp_options_t options)
        {
            lsp_trace("Optimizing system with some assembly code");
            dsp::copy_saturated         = x86::copy_saturated;
            dsp::saturate               = x86::saturate;

            if (options & DSP_OPTION_CMOV)
            {
                dsp::copy_saturated         = x86::copy_saturated_cmov;
                dsp::saturate               = x86::saturate_cmov;
            }
        }
    }

}
