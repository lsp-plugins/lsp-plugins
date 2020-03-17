/*
 * sse.cpp
 *
 *  Created on: 04 марта 2016 г.
 *      Author: sadko
 */

#include <core/types.h>
#include <core/debug.h>
#include <dsp/dsp.h>
#include <test/test.h>

#include <core/stdlib/string.h>
#include <core/stdlib/stdio.h>

#include <dsp/arch/x86/features.h>
#include <dsp/arch/x86/float.h>
#include <dsp/arch/x86/copy.h>
#include <dsp/arch/x86/graphics.h>

#include <dsp/arch/x86/cpuid.h>

//-------------------------------------------------------------------------
// References to extension initialization routines
namespace sse
{
    extern void dsp_init(const x86::cpu_features_t *f);
}

namespace sse2
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

namespace avx2
{
    extern void dsp_init(const x86::cpu_features_t *f);
}

namespace x86
{
#pragma pack(push, 1)
    typedef union vendor_sig_t
    {
        char    sig[12];
        struct {
            uint32_t    ebx, edx, ecx;
        } reg;
    } vendor_sig_t;
#pragma pack(pop)

    typedef struct cpu_vendor_id_t
    {
        const char *signature;
        size_t      vendor_id;
    } vendors_t;


    static const cpu_vendor_id_t cpu_vendor_ids[] =
    {
        { "AMDisbetter!", CPU_VENDOR_AMD },
        { "AuthenticAMD", CPU_VENDOR_AMD },
        { "CentaurHauls", CPU_VENDOR_VIA },
        { "Geode by NSC", CPU_VENDOR_NSC },
        { "GenuineIntel", CPU_VENDOR_INTEL },
        { "GenuineTMx86", CPU_VENDOR_TRANSMETA },
        { "HygonGenuine", CPU_VENDOR_HYGON },
        { "TransmetaCPU", CPU_VENDOR_TRANSMETA },
        { "VIA VIA VIA ", CPU_VENDOR_VIA }
    };

    void read_brand_string(cpuid_info_t *info, uint32_t max_ext_cpuid, char *brand)
    {
        // FUNCTION 0x80000002 - 0x80000004
        if (max_ext_cpuid < 0x80000004)
        {
            strcpy(brand, "Generic " ARCH_STRING " processor");
            return;
        }

        uint32_t *dst = reinterpret_cast<uint32_t *>(brand);
        for (size_t i=0x80000002; i<=0x80000004; ++i)
        {
            cpuid(info, i, 0);
            *(dst++)    = info->eax;
            *(dst++)    = info->ebx;
            *(dst++)    = info->ecx;
            *(dst++)    = info->edx;
        }
        *dst        = 0;

        // Cut the end of the string if there are spaces
        char *end   = &brand[3 * 16 - 1];
        while ((end >= brand) && (((*end) == ' ') || ((*end) == '\0')))
            *(end--) = '\0';
        char *start = brand;
        while ((start < end) && ((*start) == ' '))
            start++;
        if (start > brand)
            memmove(brand, start, end - start + 1);
    }

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

        read_brand_string(&info, max_ext_cpuid, f->brand);
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

                xcr0 = read_xcr(0);

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

        read_brand_string(&info, max_ext_cpuid, f->brand);
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
        vendor_sig_t sig;
        sig.reg.ebx     = info.ebx;
        sig.reg.ecx     = info.ecx;
        sig.reg.edx     = info.edx;

        for (size_t i=0, n=sizeof(cpu_vendor_ids)/sizeof(cpu_vendor_id_t); i<n; ++i)
        {
            if (!memcmp(sig.sig, cpu_vendor_ids[i].signature, sizeof(vendor_sig_t)))
            {
                f->vendor   = cpu_vendor_ids[i].vendor_id;
                break;
            }
        }

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

        switch (f->vendor)
        {
            case CPU_VENDOR_INTEL:
                do_intel_cpuid(f, max_cpuid, max_ext_cpuid);
                break;

            case CPU_VENDOR_AMD:
            case CPU_VENDOR_HYGON:
                do_amd_cpuid(f, max_cpuid, max_ext_cpuid);
                break;

            default:
                break;
        }
    }

    static dsp::start_t     dsp_start       = NULL;
    static dsp::finish_t    dsp_finish      = NULL;

    static void start(dsp::context_t *ctx)
    {
        dsp_start(ctx);
        uint32_t cr                 = fpu_read_cr();
        ctx->data[ctx->top++]       = cr;

        fpu_write_cr(cr);
    }

    static void finish(dsp::context_t *ctx)
    {
        fpu_write_cr(ctx->data[--ctx->top]);
        dsp_finish(ctx);
    }

    static const char *cpu_vendors[] =
    {
        "Unknown",
        "AMD",
        "Hygon",
        "Intel",
        "NSC",
        "Transmeta",
        "VIA"
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

    static size_t estimate_features_size(const cpu_features_t *f)
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
        return estimate;
    }

    static char *build_features_list(char *dst, const cpu_features_t *f)
    {
        // Build string
        char *s = dst;

        for (size_t x = f->features, i=0; x > 0; i++)
        {
            if (x & 1)
            {
                s = stpcpy(s, cpu_features[i]);
                x >>= 1;
                if (x)
                    *(s++) = ' ';
            }
            else
                x >>= 1;
        }
        *s = '\0';

        return s;
    }

    dsp::info_t *info()
    {
        cpu_features_t f;
        detect_options(&f);

        char *model     = NULL;
        int n = asprintf(&model, "vendor=%s, family=0x%x, model=0x%x", cpu_vendors[f.vendor], int(f.family), int(f.model));
        if ((n < 0) || (model == NULL))
            return NULL;

        size_t size     = sizeof(dsp::info_t);
        size           += strlen(ARCH_STRING) + 1;
        size           += strlen(f.brand) + 1;
        size           += strlen(model) + 1;
        size           += estimate_features_size(&f);

        dsp::info_t *res = reinterpret_cast<dsp::info_t *>(malloc(size));
        if (res == NULL)
        {
            free(model);
            return res;
        }

        char *text      = reinterpret_cast<char *>(&res[1]);
        res->arch       = text;
        text            = stpcpy(text, ARCH_STRING) + 1;
        res->cpu        = text;
        text            = stpcpy(text, f.brand) + 1;
        res->model      = text;
        text            = stpcpy(text, model) + 1;
        res->features   = text;
        build_features_list(text, &f);

        free(model);
        return res;
    }

    bool feature_check(const cpu_features_t *f, feature_t ops)
    {
        switch (ops)
        {
            case FEAT_FAST_MOVS:
                if (f->vendor == CPU_VENDOR_INTEL)
                {
                    if ((f->family == 0x6) && (f->model >= 0x5e)) // Should be some Core i3 microarchitecture...
                        return true;
                }
                break;
            case FEAT_FAST_AVX:
                if (f->vendor == CPU_VENDOR_INTEL) // Any Intel CPU is good enough with AVX
                    return true;
                if ((f->vendor == CPU_VENDOR_AMD) || (f->vendor == CPU_VENDOR_HYGON))
                    return (f->family >= AMD_FAMILY_ZEN); // Only starting with ZEN architecture AMD's implementation of AVX is fast enough
                break;
            case FEAT_FAST_FMA3:
                if (f->vendor == CPU_VENDOR_INTEL) // Any Intel CPU is good enough with AVX
                    return true;
                // AMD: maybe once FMA3 will be faster
                break;
            default:
                break;
        }

        return false;
    }

    #define EXPORT2(function, export)           dsp::function = x86::export; TEST_EXPORT(x86::export);
    #define EXPORT1(function)                   EXPORT2(function, function)

    void dsp_init()
    {
        // Dectect CPU options
        cpu_features_t f;
        detect_options(&f);

        lsp_trace("Optimizing system with some assembly code");

        // Save previous entry points
        dsp_start                   = dsp::start;
        dsp_finish                  = dsp::finish;

        // Export functions
        EXPORT1(start);
        EXPORT1(finish);
        EXPORT1(info);

        EXPORT1(copy);
        EXPORT1(rgba32_to_bgra32);
        EXPORT1(abgr32_to_bgra32);

        // Initialize extensions
        sse::dsp_init(&f);
        sse2::dsp_init(&f);
        sse3::dsp_init(&f);
        sse4::dsp_init(&f);
        avx::dsp_init(&f);
        avx2::dsp_init(&f);
    }

    #undef EXPORT1
    #undef EXPORT2
}
