/*
 * cpuid.h
 *
 *  Created on: 04 дек. 2015 г.
 *      Author: sadko
 */

#ifndef CORE_CPUID_H_
#define CORE_CPUID_H_

#include <core/types.h>

namespace lsp
{
    #if defined(__i386__) || defined(__x86_64__)

        #define X86_CPUID_FEAT_EDX_MMXEXT               (uint32_t(1) << 22)
        #define X86_CPUID_FEAT_EDX_MMX                  (uint32_t(1) << 23)
        #define X86_CPUID_FEAT_EDX_SSE                  (uint32_t(1) << 25)

        #define X86_CPUID_FEAT_ECX_SSE3                 (uint32_t(1) << 0)
        #define X86_CPUID_FEAT_ECX_SSSE3                (uint32_t(1) << 9)
        #define X86_CPUID_FEAT_ECX_XSAVE                (uint32_t(1) << 26)
        #define X86_CPUID_FEAT_ECX_AVX                  (uint32_t(1) << 28)

        #pragma pack(push, 1)
        typedef struct cpuid_info_t
        {
            uint32_t        eax;
            uint32_t        ebx;
            uint32_t        ecx;
            uint32_t        edx;
        } cpuid_info_t;
        #pragma pack(pop)

        bool cpuid(uint32_t leaf, uint32_t subleaf, cpuid_info_t *info);

    #endif /* defined(__i386__) || defined(__x86_64__) */
}



#endif /* CORE_CPUID_H_ */
