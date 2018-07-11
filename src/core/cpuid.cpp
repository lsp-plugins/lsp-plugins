/*
 * cpuid.cpp
 *
 *  Created on: 04 дек. 2015 г.
 *      Author: sadko
 */


#include <core/cpuid.h>

namespace lsp
{

    #if defined(__i386__) || defined(__x86_64__)
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

        bool cpuid(uint32_t leaf, uint32_t subleaf, cpuid_info_t *info)
        {
            #ifdef __i386__
                if (!cpuid_supported())
                    return false;
            #endif /* __i386__ */

            __asm__ __volatile__
            (
#ifdef __i386__
                __ASM_EMIT("push %%eax")
                __ASM_EMIT("push %%ebx")
                __ASM_EMIT("push %%ecx")
                __ASM_EMIT("push %%edx")
#endif /* __i386__ */
#ifdef __x86_64__
                __ASM_EMIT("push %%rax")
                __ASM_EMIT("push %%rbx")
                __ASM_EMIT("push %%rcx")
                __ASM_EMIT("push %%rdx")
#endif /* __x86_64__ */
                __ASM_EMIT("cpuid")
                __ASM_EMIT("mov %%eax, 0x0(%2)")
                __ASM_EMIT("mov %%ebx, 0x4(%2)")
                __ASM_EMIT("mov %%ecx, 0x8(%2)")
                __ASM_EMIT("mov %%edx, 0xc(%2)")

#ifdef __i386__
                __ASM_EMIT("pop %%edx")
                __ASM_EMIT("pop %%ecx")
                __ASM_EMIT("pop %%ebx")
                __ASM_EMIT("pop %%eax")
#endif /* __i386__ */
#ifdef __x86_64__
                __ASM_EMIT("pop %%rdx")
                __ASM_EMIT("pop %%rcx")
                __ASM_EMIT("pop %%rbx")
                __ASM_EMIT("pop %%rax")
#endif /* __x86_64__ */
                :
                : "a"(leaf), "c"(subleaf), "D" (info)
                : "cc", "memory"
            );

            return true;
        }
    #endif /* defined(__i386__) || defined(__x86_64__) */

}

