/*
 * dsp.cpp
 *
 *  Created on: 02 окт. 2015 г.
 *      Author: sadko
 */

#include <math.h>

#include <core/dsp.h>
#include <core/debug.h>
#include <core/dsp/native.h>

#if defined(__i386__) || defined(__x86_64__)
    #include <core/cpuid.h>
    #ifdef __SSE__
        #include <core/dsp/sse.h>
    #endif /* __SSE__ */
#endif /* defined(__i386__) || defined(__x86_64__) */

namespace lsp
{
    dsp::dsp()
    {
    }
    
    dsp::~dsp()
    {
    }
    
    dsp* dsp::createInstance()
    {
        dsp_factory_t result = native_dsp::instance;

        // X86-family code
        #if defined(__i386__) || defined(__x86_64__)
            // Check max CPUID
            cpuid_info_t info;
            cpuid(0, 0, &info);
            if (info.eax <= 0)
                return result();

            // Get model and family
            cpuid(1, 0, &info);
            size_t family_id    = (info.eax >> 8) & 0x0f;
            size_t model_id     = (info.eax >> 4) & 0x0f;

            if (family_id == 0x0f)
                family_id   += (info.eax >> 20) & 0xff;
            if ((family_id == 0x0f) || (family_id == 0x06))
                model_id    += (info.eax >> 12) & 0xf0;

            // Get model, family and extensions
            cpuid(1, 0, &info);

            #ifdef __SSE__
                if (info.edx & X86_CPUID_FEAT_EDX_SSE)
                {
                    // Set SSE ONLY if OS supports it
                    if ((family_id == 6) || (family_id > 15))
                    {
                        lsp_debug("Optmizing DSP for SSE instructions");
                        result  = sse_dsp::instance;
                    }
                }
            #endif /* __SSE__ */
        #endif

        return result();
    }

} /* namespace lsp */
