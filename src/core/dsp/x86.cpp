/*
 * sse.cpp
 *
 *  Created on: 04 марта 2016 г.
 *      Author: sadko
 */

#include <core/types.h>
#include <core/debug.h>
#include <core/dsp.h>

#include <core/x86/features.h>
#include <core/x86/float.h>
#include <core/x86/graphics.h>

namespace lsp
{
    namespace x86
    {
        #include <core/x86/cpuid.h>
    }
}

namespace lsp
{
    //-------------------------------------------------------------------------
    // References to extension initialization routines
    namespace sse
    {
        extern void dsp_init(const x86::cpu_features_t *f);
    }

    namespace sse3
    {
        extern void dsp_init(const x86::cpu_features_t *f);
    }

    namespace sse4
    {
        extern void dsp_init(const x86::cpu_features_t *f);
    }

    namespace avx
    {
        extern void dsp_init(const x86::cpu_features_t *f);
    }

    namespace x86
    {
        void do_intel_cpuid(cpu_features_t *f, size_t max_cpuid, size_t max_ext_cpuid)
        {
            cpuid_info_t info;
            uint64_t xcr0 = 0;

            // FUNCTION 1
            if (max_cpuid >= 1)
            {
                cpuid(&info, 1, 0);

                if (info.edx & X86_CPUID1_INTEL_EDX_FPU)
                    f->features     |= CPU_OPTION_FPU;
                if (info.edx & X86_CPUID1_INTEL_EDX_CMOV)
                    f->features     |= CPU_OPTION_CMOV;
                if (info.edx & X86_CPUID1_INTEL_EDX_MMX)
                    f->features     |= CPU_OPTION_MMX;
                if (info.edx & X86_CPUID1_INTEL_EDX_SSE)
                    f->features     |= CPU_OPTION_SSE;
                if (info.edx & X86_CPUID1_INTEL_EDX_SSE2)
                    f->features     |= CPU_OPTION_SSE2;

                if (info.ecx & X86_CPUID1_INTEL_ECX_SSE3)
                    f->features     |= CPU_OPTION_SSE3;
                if (info.ecx & X86_CPUID1_INTEL_ECX_SSSE3)
                    f->features     |= CPU_OPTION_SSSE3;
                if (info.ecx & X86_CPUID1_INTEL_ECX_SSE4_1)
                    f->features     |= CPU_OPTION_SSE4_1;
                if (info.ecx & X86_CPUID1_INTEL_ECX_SSE4_2)
                    f->features     |= CPU_OPTION_SSE4_2;
                if (info.ecx & X86_CPUID1_INTEL_ECX_XSAVE)
                    f->features     |= CPU_OPTION_FXSAVE;
                if (info.ecx & X86_CPUID1_INTEL_ECX_OSXSAVE)
                {
                    f->features     |= CPU_OPTION_OSXSAVE;

                    xcr0         = read_xcr(0);

                    // Additional check for AVX support
                    if ((xcr0 & XCR_FLAGS_AVX) == XCR_FLAGS_AVX)
                    {
                        if (info.ecx & X86_CPUID1_INTEL_ECX_FMA3)
                            f->features     |= CPU_OPTION_FMA3;
                        if (info.ecx & X86_CPUID1_INTEL_ECX_AVX)
                            f->features     |= CPU_OPTION_AVX;
                    }
                }
            }

            // FUNCTION 7
            if (max_cpuid >= 7)
            {
                cpuid(&info, 7, 0);

                if (f->features & CPU_OPTION_OSXSAVE)
                {
                    if ((xcr0 & XCR_FLAGS_AVX) == XCR_FLAGS_AVX)
                    {
                        if (info.ebx & X86_CPUID7_INTEL_EBX_AVX2)
                            f->features     |= CPU_OPTION_AVX2;
                    }

                    if ((xcr0 & XCR_FLAGS_AVX512) == XCR_FLAGS_AVX512)
                    {
                        if (info.ebx & X86_CPUID7_INTEL_EBX_AVX512F)
                            f->features     |= CPU_OPTION_AVX512F;
                        if (info.ebx & X86_CPUID7_INTEL_EBX_AVX512DQ)
                            f->features     |= CPU_OPTION_AVX512DQ;
                        if (info.ebx & X86_CPUID7_INTEL_EBX_AVX512IFMA)
                            f->features     |= CPU_OPTION_AVX512IFMA;
                        if (info.ebx & X86_CPUID7_INTEL_EBX_AVX512PF)
                            f->features     |= CPU_OPTION_AVX512PF;
                        if (info.ebx & X86_CPUID7_INTEL_EBX_AVX512ER)
                            f->features     |= CPU_OPTION_AVX512ER;
                        if (info.ebx & X86_CPUID7_INTEL_EBX_AVX512CD)
                            f->features     |= CPU_OPTION_AVX512CD;
                        if (info.ebx & X86_CPUID7_INTEL_EBX_AVX512BW)
                            f->features     |= CPU_OPTION_AVX512BW;
                        if (info.ebx & X86_CPUID7_INTEL_EBX_AVX512VL)
                            f->features     |= CPU_OPTION_AVX512VL;

                        if (info.ecx & X86_CPUID7_INTEL_ECX_AVX512VBMI)
                            f->features     |= CPU_OPTION_AVX512VBMI;
                    }
                }
            }
        }

        void do_amd_cpuid(cpu_features_t *f, size_t max_cpuid, size_t max_ext_cpuid)
        {
            cpuid_info_t info;
            uint64_t xcr0 = 0;

            // FUNCTION 1
            if (max_cpuid >= 1)
            {
                cpuid(&info, 1, 0);

                if (info.edx & X86_CPUID1_AMD_EDX_FPU)
                    f->features     |= CPU_OPTION_FPU;
                if (info.edx & X86_CPUID1_AMD_EDX_CMOV)
                    f->features     |= CPU_OPTION_CMOV;
                if (info.edx & X86_CPUID1_AMD_EDX_MMX)
                    f->features     |= CPU_OPTION_MMX;
                if (info.edx & X86_CPUID1_AMD_EDX_SSE)
                    f->features     |= CPU_OPTION_SSE;
                if (info.edx & X86_CPUID1_AMD_EDX_SSE2)
                    f->features     |= CPU_OPTION_SSE2;

                if (info.ecx & X86_CPUID1_AMD_ECX_SSE3)
                    f->features     |= CPU_OPTION_SSE3;
                if (info.ecx & X86_CPUID1_AMD_ECX_SSSE3)
                    f->features     |= CPU_OPTION_SSSE3;
                if (info.ecx & X86_CPUID1_AMD_ECX_SSE4_1)
                    f->features     |= CPU_OPTION_SSE4_1;
                if (info.ecx & X86_CPUID1_AMD_ECX_SSE4_2)
                    f->features     |= CPU_OPTION_SSE4_2;
                if (info.ecx & X86_CPUID1_AMD_ECX_XSAVE)
                    f->features     |= CPU_OPTION_FXSAVE;
                if (info.ecx & X86_CPUID1_AMD_ECX_OSXSAVE)
                {
                    f->features     |= CPU_OPTION_OSXSAVE;

                    uint64_t xcr0 = read_xcr(0);

                    // Additional check for AVX support
                    if ((xcr0 & XCR_FLAGS_AVX) == XCR_FLAGS_AVX)
                    {
                        if (info.ecx & X86_CPUID1_AMD_ECX_FMA3)
                            f->features     |= CPU_OPTION_FMA3;
                        if (info.ecx & X86_CPUID1_AMD_ECX_AVX)
                            f->features     |= CPU_OPTION_AVX;
                    }
                }
            }

            // FUNCTION 7
            if (max_cpuid >= 7)
            {
                cpuid(&info, 7, 0);

                if (info.ebx & X86_CPUID7_AMD_EBX_AVX2)
                    f->features     |= CPU_OPTION_AVX2;
            }

            // FUNCTION 0x80000001
            if (max_ext_cpuid >= 0x80000001)
            {
                cpuid(&info, 0x80000001, 0);

                if (info.ecx & X86_XCPUID1_AMD_ECX_SSE4A)
                    f->features     |= CPU_OPTION_SSE4A;

                if (info.edx & X86_XCPUID1_AMD_EDX_FPU)
                    f->features     |= CPU_OPTION_FPU;
                if (info.edx & X86_XCPUID1_AMD_EDX_CMOV)
                    f->features     |= CPU_OPTION_CMOV;
                if (info.edx & X86_XCPUID1_AMD_EDX_MMX)
                    f->features     |= CPU_OPTION_MMX;

                if (f->features & CPU_OPTION_OSXSAVE)
                {
                    if ((xcr0 & XCR_FLAGS_AVX) == XCR_FLAGS_AVX)
                    {
                        if (info.ecx & X86_XCPUID1_AMD_ECX_FMA4)
                            f->features     |= CPU_OPTION_FMA4;
                    }
                }
            }
        }

        void detect_options(cpu_features_t *f)
        {
            // Initialize structure
            f->vendor       = CPU_VENDOR_UNKNOWN;
            f->family       = 0;
            f->model        = 0;
            f->features     = 0;

            // X86-family code
            if (!cpuid_supported())
                return;

            // Check max CPUID
            cpuid_info_t info;
            cpuid(&info, 0, 0);

            // Detect vendor
            if ((info.ebx == X86_CPUID0_INTEL_EBX) && (info.ecx == X86_CPUID0_INTEL_ECX) && (info.edx == X86_CPUID0_INTEL_EDX))
                f->vendor   = CPU_VENDOR_INTEL;
            else if ((info.ebx == X86_CPUID0_AMD_EBX) && (info.ecx == X86_CPUID0_AMD_ECX) && (info.edx == X86_CPUID0_AMD_EDX))
                f->vendor   = CPU_VENDOR_AMD;

            size_t max_cpuid    = info.eax;
            if (max_cpuid <= 0)
                return;

            // Get model and family
            cpuid(&info, 1, 0);
            f->family           = (info.eax >> 8) & 0x0f;
            f->model            = (info.eax >> 4) & 0x0f;

            if (f->family == 0x0f)
                f->family           += (info.eax >> 20) & 0xff;
            if ((f->family == 0x0f) || (f->family == 0x06))
                f->model            += (info.eax >> 12) & 0xf0;

            // Get maximum available extended CPUID
            cpuid(&info, 0x80000000, 0);
            size_t max_ext_cpuid = info.eax;

            if (f->vendor == CPU_VENDOR_INTEL)
                do_intel_cpuid(f, max_cpuid, max_ext_cpuid);
            else if (f->vendor == CPU_VENDOR_AMD)
                do_amd_cpuid(f, max_cpuid, max_ext_cpuid);
        }

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

#ifdef LSP_TRACE

        static const char *cpu_vendors[] =
        {
            "unknown", "Intel", "AMD"
        };

        static const char *cpu_features[] =
        {
            "FPU", "CMOV", "MMX", "FXSAVE",
            "SSE", "SSE2", "SSE3", "SSSE3",
            "SSE4.1", "SSE4.2", "SSE4A", "XSAVE",
            "FMA3", "FMA4", "AVX", "AVX2",
            "AVX512F", "AVX512DQ", "AVX512IFMA", "AVX512PF",
            "AVX512ER", "AVX512CD", "AVX512BW", "AVX512VL",
            "AVX512VBMI"
        };

        char *build_feature_list(const cpu_features_t *f)
        {
            // Estimate the string length
            size_t estimate = 1; // End of string character
            for (size_t x = f->features, i=0; x > 0; i++)
            {
                if (x & 1)
                {
                    estimate += strlen(cpu_features[i]);
                    x >>= 1;
                    if (x)
                        estimate ++; // Space character
                }
                else
                    x >>= 1;
            }

            // Allocate string
            char *res = reinterpret_cast<char *>(malloc(estimate));
            if (res == NULL)
                return res;

            // Build string
            char *s = res;
            *s = '\0';

            for (size_t x = f->features, i=0; x > 0; i++)
            {
                if (x & 1)
                {
                    strcpy(s, cpu_features[i]);
                    while ((*s) != '\0')
                        s++;
                    x >>= 1;
                    if (x)
                    {
                        *(s++) = ' ';
                        *s = '\0';
                    }
                }
                else
                    x >>= 1;
            }

            return res;
        }

#endif /* LSP_TRACE */

        #define EXPORT2(function, export)           dsp::function = x86::export
        #define EXPORT1(function)                   EXPORT2(function, function)

        void dsp_init()
        {
            // Dectect CPU options
            cpu_features_t f;
            detect_options(&f);
#ifdef LSP_TRACE
            char *flist = build_feature_list(&f);

            lsp_printf("Detected CPU: vendor=%s, family=%d, model=%d",
                        cpu_vendors[f.vendor], int(f.family), int(f.model));
            if (flist != NULL)
            {
                lsp_printf("CPU features: %s", flist);
                free(flist);
            }
#endif /* LSP_TRACE */

            lsp_trace("Optimizing system with some assembly code");

            // Save previous entry points
            dsp_start                   = dsp::start;
            dsp_finish                  = dsp::finish;
            
            // Export functions
            EXPORT1(start);
            EXPORT1(finish);

            EXPORT1(copy_saturated);
            EXPORT1(saturate);
            EXPORT1(rgba32_to_bgra32);

            if (f.features & CPU_OPTION_CMOV)
            {
                EXPORT2(copy_saturated, copy_saturated_cmov);
                EXPORT2(saturate, saturate_cmov);
            }

            // Initialize extensions
            sse::dsp_init(&f);
            sse3::dsp_init(&f);
            sse4::dsp_init(&f);
            avx::dsp_init(&f);
        }
        
        #undef EXPORT1
        #undef EXPORT2
    }

}
