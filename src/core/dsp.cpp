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
    // Declare static variables
    namespace dsp
    {
        void    (* copy)(float *dst, const float *src, size_t count) = NULL;
        void    (* move)(float *dst, const float *src, size_t count) = NULL;
        void    (* fill)(float *dst, float value, size_t count) = NULL;
        void    (* fill_zero)(float *dst, size_t count) = NULL;
        void    (* fill_one)(float *dst, size_t count) = NULL;
        void    (* fill_minus_one)(float *dst, size_t count) = NULL;
        void    (* abs)(float *dst, const float *src, size_t count) = NULL;
        void    (* abs_normalized)(float *dst, const float *src, size_t count) = NULL;
        void    (* normalize)(float *dst, const float *src, size_t count) = NULL;
        float   (* min)(const float *src, size_t count) = NULL;
        float   (* max)(const float *src, size_t count) = NULL;
        void    (* minmax)(const float *src, size_t count, float *min, float *max) = NULL;
        size_t  (* min_index)(const float *src, size_t count) = NULL;
        size_t  (* max_index)(const float *src, size_t count) = NULL;
        void    (* multiply)(float *dst, const float *src, float k, size_t count) = NULL;
        float   (* scalar_mul)(const float *a, const float *b, size_t count) = NULL;
        void    (* accumulate)(float *dst, const float *src, float k, float p, size_t count) = NULL;
        void    (* add_multiplied)(float *dst, const float *src, float k, size_t count) = NULL;
        void    (* sub_multiplied)(float *dst, const float *src, float k, size_t count) = NULL;
        void    (* integrate)(float *dst, const float *src, float k, size_t count) = NULL;
        void    (* mix)(float *dst, const float *src1, const float *src2, float k1, float k2, size_t count) = NULL;
    }

    namespace dsp
    {
        void init()
        {
            // Consider already initialized
            if (copy != NULL)
                return;

            // Initialize with native functions
            native::dsp_init();

            // X86-family code
            #if defined(__i386__) || defined(__x86_64__)
                // Check max CPUID
                cpuid_info_t info;
                cpuid(0, 0, &info);
                if (info.eax <= 0)
                    return;

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
                            sse::dsp_init();
                    }
                #endif /* __SSE__ */
            #endif
        }
    }

} /* namespace lsp */
