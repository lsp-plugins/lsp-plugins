/*
 * cpuid.h
 *
 *  Created on: 04 дек. 2015 г.
 *      Author: sadko
 */

#ifndef CORE_X86_CPUID_H_
#define CORE_X86_CPUID_H_

#include <core/types.h>

namespace lsp
{
    // Function 0
    #define X86_CPUID0_INTEL_EBX                    0x756e6547
    #define X86_CPUID0_INTEL_ECX                    0x6c65746e
    #define X86_CPUID0_INTEL_EDX                    0x49656e69

    #define X86_CPUID0_AMD_EBX                      0x68747541
    #define X86_CPUID0_AMD_ECX                      0x444d4163
    #define X86_CPUID0_AMD_EDX                      0x69746e65

    // Function 1
    #define X86_CPUID1_INTEL_ECX_SSE3               (uint32_t(1) << 0)
    #define X86_CPUID1_INTEL_ECX_SSSE3              (uint32_t(1) << 9)
    #define X86_CPUID1_INTEL_ECX_FMA3               (uint32_t(1) << 12)
    #define X86_CPUID1_INTEL_ECX_SSE4_1             (uint32_t(1) << 19)
    #define X86_CPUID1_INTEL_ECX_SSE4_2             (uint32_t(1) << 20)
    #define X86_CPUID1_INTEL_ECX_XSAVE              (uint32_t(1) << 26)
    #define X86_CPUID1_INTEL_ECX_AVX                (uint32_t(1) << 28)

    #define X86_CPUID1_INTEL_EDX_FPU                (uint32_t(1) << 0)
    #define X86_CPUID1_INTEL_EDX_CMOV               (uint32_t(1) << 15)
    #define X86_CPUID1_INTEL_EDX_MMX                (uint32_t(1) << 23)
    #define X86_CPUID1_INTEL_EDX_SSE                (uint32_t(1) << 25)
    #define X86_CPUID1_INTEL_EDX_SSE2               (uint32_t(1) << 26)

    #define X86_CPUID1_AMD_ECX_SSE3                 (uint32_t(1) << 0)
    #define X86_CPUID1_AMD_ECX_SSSE3                (uint32_t(1) << 9)
    #define X86_CPUID1_AMD_ECX_FMA3                 (uint32_t(1) << 12)
    #define X86_CPUID1_AMD_ECX_SSE4_1               (uint32_t(1) << 19)
    #define X86_CPUID1_AMD_ECX_SSE4_2               (uint32_t(1) << 20)
    #define X86_CPUID1_AMD_ECX_AVX                  (uint32_t(1) << 28)

    #define X86_CPUID1_AMD_EDX_FPU                  (uint32_t(1) << 0)
    #define X86_CPUID1_AMD_EDX_CMOV                 (uint32_t(1) << 15)
    #define X86_CPUID1_AMD_EDX_MMX                  (uint32_t(1) << 23)
    #define X86_CPUID1_AMD_EDX_SSE                  (uint32_t(1) << 25)
    #define X86_CPUID1_AMD_EDX_SSE2                 (uint32_t(1) << 26)

    // Function 7
    #define X86_CPUID7_INTEL_EBX_AVX2               (uint32_t(1) << 5)

    #define X86_CPUID7_AMD_EBX_AVX2                 (uint32_t(1) << 5)

    // Function 80000001
    #define X86_XCPUID1_AMD_ECX_FMA4                (uint32_t(1) << 16)
    #define X86_XCPUID1_AMD_ECX_SSE4A               (uint32_t(1) << 6)

    #define X86_XCPUID1_AMD_EDX_FPU                 (uint32_t(1) << 0)
    #define X86_XCPUID1_AMD_EDX_CMOV                (uint32_t(1) << 15)
    #define X86_XCPUID1_AMD_EDX_MMX                 (uint32_t(1) << 23)


    #define X86_FAMILY_PENTIUM                      5
    #define X86_FAMILY_PENTIUM_PRO                  6
    #define X86_FAMILY_PENTIUM_PRO_PLUS             15

    #pragma pack(push, 1)
    typedef struct cpuid_info_t
    {
        uint32_t        eax;
        uint32_t        ebx;
        uint32_t        ecx;
        uint32_t        edx;
    } cpuid_info_t;
    #pragma pack(pop)

    #ifdef __i386__
        bool cpuid_supported()
        {
            bool result;
            __asm__ __volatile__
            (
                __ASM_EMIT("pushfl")
                __ASM_EMIT("pop %%eax")
                __ASM_EMIT("mov %%eax, %%edx")
                __ASM_EMIT("xor $0x200000, %%eax")
                __ASM_EMIT("push %%eax")
                __ASM_EMIT("popfl")
                __ASM_EMIT("pushfl")
                __ASM_EMIT("pop %%eax")
                __ASM_EMIT("xor %%edx, %%eax")
                __ASM_EMIT("push %%edx")
                __ASM_EMIT("popfl")
                __ASM_EMIT("shr $21, %%eax")
                : "=a"(result)
                : : "cc", "%edx"
            );

            return result;
        }
    #endif /* __i386__ */

    inline bool cpuid(uint32_t leaf, uint32_t subleaf, cpuid_info_t *info)
    {
        #ifdef __i386__
            if (!cpuid_supported())
                return false;
        #endif /* __i386__ */

        __asm__ __volatile__
        (
            __ASM_EMIT("cpuid")
            #ifdef __i386__
                __ASM_EMIT("push %%ebx")
            #endif /* __i386__ */
            __ASM_EMIT("mov %%eax, 0x0(%[info])")
            __ASM_EMIT("mov %%ebx, 0x4(%[info])")
            __ASM_EMIT("mov %%ecx, 0x8(%[info])")
            __ASM_EMIT("mov %%edx, 0xc(%[info])")
            #ifdef __i386__
                __ASM_EMIT("pop %%ebx")
            #endif /* __i386__ */

            : "+a"(leaf), "+c"(subleaf)
            : [info] "D" (info)
            : "cc", "memory",
            #ifndef __i386__
                "%ebx",
            #endif /* __i386__ */
                "%edx"
        );

        return true;
    }
}

#endif /* CORE_X86_CPUID_H_ */
