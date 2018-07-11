/*
 * bswap.h
 *
 *  Created on: 04 мар. 2016 г.
 *      Author: sadko
 */

#ifndef CORE_X86_BSWAP_H_
#define CORE_X86_BSWAP_H_

namespace lsp
{
    inline uint8_t __lsp_forced_inline    byte_swap(uint8_t v)
    {
        return v;
    }

    inline uint16_t __lsp_forced_inline    byte_swap(uint16_t v)
    {
        __asm__ __volatile__ (
            __ASM_EMIT("ror $8, %[v]")
            : [v] "+r"(v)
            : : "cc"
        );
        return v;
    }

    inline uint32_t __lsp_forced_inline    byte_swap(uint32_t v)
    {
        __asm__ __volatile__ (
            __ASM_EMIT("bswap %[v]")
            : [v] "+r"(v)
            : :
        );
        return v;
    }

    inline float __lsp_forced_inline    byte_swap(float v)
    {
        __asm__ __volatile__ (
            __ASM_EMIT("bswap %[v]")
            : [v] "+r"(v)
            : :
        );
        return v;
    }

    #ifdef __x86_64__
        inline uint64_t __lsp_forced_inline    byte_swap(uint64_t v)
        {
            __asm__ __volatile__ (
                __ASM_EMIT("bswap %[v]")
                : [v] "+r"(v)
                : :
            );
            return v;
        }

        inline double __lsp_forced_inline    byte_swap(double v)
        {
            __asm__ __volatile__ (
                __ASM_EMIT("bswap %[v]")
                : [v] "+r"(v)
                : :
            );
            return v;
        }
    #else /* __i386__ */
        inline uint64_t __lsp_forced_inline    byte_swap(uint64_t v)
        {
            __asm__ __volatile__ (
                __ASM_EMIT("bswap %%eax")
                __ASM_EMIT("bswap %%edx")
                __ASM_EMIT("xchg %%edx, %%eax")
                : "+A"(v)
                : :
            );
            return v;
        }

        inline double __lsp_forced_inline    byte_swap(double v)
        {
            __asm__ __volatile__ (
                __ASM_EMIT("bswap %%eax")
                __ASM_EMIT("bswap %%edx")
                __ASM_EMIT("xchg %%edx, %%eax")
                : "+A"(v)
                : :
            );
            return v;
        }
    #endif /* __x86_64__ */
}

#endif /* CORE_X86_BSWAP_H_ */
