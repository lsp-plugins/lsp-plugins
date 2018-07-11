/*
 * bits.h
 *
 *  Created on: 06 февр. 2016 г.
 *      Author: sadko
 */

#ifndef CORE_X86_BITS_H_
#define CORE_X86_BITS_H_

namespace lsp
{
    inline uint8_t __lsp_forced_inline    reverse_bits(uint8_t v)
    {
        register size_t tmp;

        __asm__ __volatile__ (
            __ASM_EMIT("movzx %0, %1")
            __ASM_EMIT("mov (%2, %1), %0")
            : "+r"(v), "=&r"(tmp)
            : "r"(__rb)
            : "cc"
        );

        return v;
    }

    inline uint8_t __lsp_forced_inline    reverse_bits(uint8_t v, size_t count)
    {
        register size_t tmp;

        __asm__ __volatile__ (
            __ASM_EMIT("movzx %0, %1")
            __ASM_EMIT("mov (%2, %1), %0")
            __ASM_EMIT("shr %%cl, %0")
            : "+r"(v), "=&r"(tmp)
            : "r"(__rb), "c"(8-count)
            : "cc"
        );

        return v;
    }

    inline uint16_t __lsp_forced_inline   reverse_bits(uint16_t v)
    {
        #ifdef __x86_64__
        register size_t tmp;

        __asm__ __volatile__ (
            __ASM_EMIT("movzx %%al, %1")
            __ASM_EMIT("mov (%2, %1), %%al")
            __ASM_EMIT("ror $8, %%ax")
            __ASM_EMIT("movzx %%al, %1")
            __ASM_EMIT("mov (%2, %1), %%al")

            : "+a"(v), "=&r"(tmp)
            : "r"(__rb)
            : "cc"
        );
        #else
        register size_t tmp1, tmp2;

        __asm__ __volatile__ (
            __ASM_EMIT("movzx %%al, %1")
            __ASM_EMIT("movzx %%ah, %2")
            __ASM_EMIT("mov (%3, %1), %%ah")
            __ASM_EMIT("mov (%3, %2), %%al")
            : "+a"(v), "=&r"(tmp1), "=&r"(tmp2)
            : "r"(__rb)
            : "cc"
        );
        #endif /* __x86_64__ */

        return v;
    }

    inline uint16_t __lsp_forced_inline    reverse_bits(uint16_t v, size_t count)
    {
        #ifdef __x86_64__
        register size_t tmp;

        __asm__ __volatile__ (
            __ASM_EMIT("movzx %%al, %1")
            __ASM_EMIT("mov (%2, %1), %%al")
            __ASM_EMIT("ror $8, %%ax")
            __ASM_EMIT("movzx %%al, %1")
            __ASM_EMIT("mov (%2, %1), %%al")
            __ASM_EMIT("shr %%cl, %0")

            : "+a"(v), "=&r"(tmp)
            : "r"(__rb), "c"(16 - count)
            : "cc"
        );
        #else
        register size_t tmp1, tmp2;

        __asm__ __volatile__ (
            __ASM_EMIT("movzx %%al, %1")
            __ASM_EMIT("movzx %%ah, %2")
            __ASM_EMIT("mov (%3, %1), %%ah")
            __ASM_EMIT("mov (%3, %2), %%al")
            __ASM_EMIT("shr %%cl, %0")
            : "+a"(v), "=&r"(tmp1), "=&r"(tmp2)
            : "r"(__rb), "c"(16 - count)
            : "cc"
        );
        #endif /* __x86_64__ */

        return v;
    }

    inline uint32_t __lsp_forced_inline    reverse_bits(uint32_t v)
    {
        register uint32_t tmp;

        __asm__ __volatile__ (
            __ASM_EMIT("bswap %0")

            __ASM_EMIT("mov %0, %1")
            __ASM_EMIT("and $0xf0f0f0f0, %0")
            __ASM_EMIT("and $0x0f0f0f0f, %1")
            __ASM_EMIT("shr $4, %0")
            __ASM_EMIT("shl $4, %1")
            __ASM_EMIT("or  %1, %0")

            __ASM_EMIT("mov %0, %1")
            __ASM_EMIT("and $0xcccccccc, %0")
            __ASM_EMIT("and $0x33333333, %1")
            __ASM_EMIT("shr $2, %0")
            __ASM_EMIT("lea (%0, %1, 4), %0")

            __ASM_EMIT("mov %0, %1")
            __ASM_EMIT("and $0xaaaaaaaa, %0")
            __ASM_EMIT("and $0x55555555, %1")
            __ASM_EMIT("shr $1, %0")
            __ASM_EMIT("lea (%0, %1, 2), %0")

            : "+r"(v), "=&r"(tmp)
            :
            : "cc"
        );
        return v;
    }

    inline uint32_t __lsp_forced_inline    reverse_bits(uint32_t v, size_t count)
    {
        register uint32_t tmp;

        __asm__ __volatile__ (
            __ASM_EMIT("bswap %0")

            __ASM_EMIT("mov %0, %1")
            __ASM_EMIT("and $0xf0f0f0f0, %0")
            __ASM_EMIT("and $0x0f0f0f0f, %1")
            __ASM_EMIT("shr $4, %0")
            __ASM_EMIT("shl $4, %1")
            __ASM_EMIT("or  %1, %0")

            __ASM_EMIT("mov %0, %1")
            __ASM_EMIT("and $0xcccccccc, %0")
            __ASM_EMIT("and $0x33333333, %1")
            __ASM_EMIT("shr $2, %0")
            __ASM_EMIT("lea (%0, %1, 4), %0")

            __ASM_EMIT("mov %0, %1")
            __ASM_EMIT("and $0xaaaaaaaa, %0")
            __ASM_EMIT("and $0x55555555, %1")
            __ASM_EMIT("shr $1, %0")
            __ASM_EMIT("lea (%0, %1, 2), %0")

            __ASM_EMIT("shr %%cl, %0")

            : "+r"(v), "=&r"(tmp)
            : "c"(32-count)
            : "cc"
        );
        return v;
    }

    #ifdef __i386__
        inline uint64_t __lsp_forced_inline    reverse_bits(uint64_t v)
        {
            register uint32_t tmp1, tmp2;

            __asm__ __volatile__
            (
                __ASM_EMIT("bswap %%eax")
                __ASM_EMIT("bswap %%edx")
                __ASM_EMIT("xchg %%edx, %%eax")

                __ASM_EMIT("mov %%eax, %1")
                __ASM_EMIT("mov %%edx, %2")
                __ASM_EMIT("and $0xf0f0f0f0, %%eax")
                __ASM_EMIT("and $0xf0f0f0f0, %%edx")
                __ASM_EMIT("and $0x0f0f0f0f, %1")
                __ASM_EMIT("and $0x0f0f0f0f, %2")
                __ASM_EMIT("shr $4, %%eax")
                __ASM_EMIT("shr $4, %%edx")
                __ASM_EMIT("shl $4, %1")
                __ASM_EMIT("shl $4, %2")
                __ASM_EMIT("or  %1, %%eax")
                __ASM_EMIT("or  %2, %%edx")

                __ASM_EMIT("mov %%eax, %1")
                __ASM_EMIT("mov %%edx, %2")
                __ASM_EMIT("and $0xcccccccc, %%eax")
                __ASM_EMIT("and $0xcccccccc, %%edx")
                __ASM_EMIT("and $0x33333333, %1")
                __ASM_EMIT("and $0x33333333, %2")
                __ASM_EMIT("shr $2, %%eax")
                __ASM_EMIT("shr $2, %%edx")
                __ASM_EMIT("lea (%%eax, %1, 4), %%eax")
                __ASM_EMIT("lea (%%edx, %2, 4), %%edx")

                __ASM_EMIT("mov %%eax, %1")
                __ASM_EMIT("mov %%edx, %2")
                __ASM_EMIT("and $0xaaaaaaaa, %%eax")
                __ASM_EMIT("and $0xaaaaaaaa, %%edx")
                __ASM_EMIT("and $0x55555555, %1")
                __ASM_EMIT("and $0x55555555, %2")
                __ASM_EMIT("shr $1, %%eax")
                __ASM_EMIT("shr $1, %%edx")
                __ASM_EMIT("lea (%%eax, %1, 2), %%eax")
                __ASM_EMIT("lea (%%edx, %2, 2), %%edx")

                : "+A"(v), "=&r"(tmp1), "=&r"(tmp2)
                :
                : "cc"
            );

            return v;
        }

        inline uint64_t __lsp_forced_inline    reverse_bits(uint64_t v, size_t count)
        {
            register uint32_t tmp1, tmp2;

            if (count < 32)
            {
                __asm__ __volatile__
                (
                    __ASM_EMIT("bswap %%eax")
                    __ASM_EMIT("bswap %%edx")
                    __ASM_EMIT("xchg %%edx, %%eax")

                    __ASM_EMIT("mov %%eax, %1")
                    __ASM_EMIT("mov %%edx, %2")
                    __ASM_EMIT("and $0xf0f0f0f0, %%eax")
                    __ASM_EMIT("and $0xf0f0f0f0, %%edx")
                    __ASM_EMIT("and $0x0f0f0f0f, %1")
                    __ASM_EMIT("and $0x0f0f0f0f, %2")
                    __ASM_EMIT("shr $4, %%eax")
                    __ASM_EMIT("shr $4, %%edx")
                    __ASM_EMIT("shl $4, %1")
                    __ASM_EMIT("shl $4, %2")
                    __ASM_EMIT("or  %1, %%eax")
                    __ASM_EMIT("or  %2, %%edx")

                    __ASM_EMIT("mov %%eax, %1")
                    __ASM_EMIT("mov %%edx, %2")
                    __ASM_EMIT("and $0xcccccccc, %%eax")
                    __ASM_EMIT("and $0xcccccccc, %%edx")
                    __ASM_EMIT("and $0x33333333, %1")
                    __ASM_EMIT("and $0x33333333, %2")
                    __ASM_EMIT("shr $2, %%eax")
                    __ASM_EMIT("shr $2, %%edx")
                    __ASM_EMIT("lea (%%eax, %1, 4), %%eax")
                    __ASM_EMIT("lea (%%edx, %2, 4), %%edx")

                    __ASM_EMIT("mov %%eax, %1")
                    __ASM_EMIT("mov %%edx, %2")
                    __ASM_EMIT("and $0xaaaaaaaa, %%eax")
                    __ASM_EMIT("and $0xaaaaaaaa, %%edx")
                    __ASM_EMIT("and $0x55555555, %1")
                    __ASM_EMIT("and $0x55555555, %2")
                    __ASM_EMIT("shr $1, %%eax")
                    __ASM_EMIT("shr $1, %%edx")
                    __ASM_EMIT("lea (%%eax, %1, 2), %%eax")
                    __ASM_EMIT("lea (%%edx, %2, 2), %%edx")

                    __ASM_EMIT("shrd %%cl, %%edx, %%eax")

                    : "+A"(v), "=&r"(tmp1), "=&r"(tmp2)
                    : "c" (32-count)
                    : "cc"
                );
            }
            else
            {
                __asm__ __volatile__
                (
                    __ASM_EMIT("bswap %%edx")

                    __ASM_EMIT("mov %%edx, %%eax")
                    __ASM_EMIT("and $0xf0f0f0f0, %%eax")
                    __ASM_EMIT("and $0x0f0f0f0f, %%edx")
                    __ASM_EMIT("shr $4, %%eax")
                    __ASM_EMIT("shl $4, %%edx")
                    __ASM_EMIT("or  %%eax, %%edx")

                    __ASM_EMIT("mov %%edx, %%eax")
                    __ASM_EMIT("and $0xcccccccc, %%eax")
                    __ASM_EMIT("and $0x33333333, %%edx")
                    __ASM_EMIT("shr $2, %%eax")
                    __ASM_EMIT("lea (%%eax, %%edx, 4), %%edx")

                    __ASM_EMIT("mov %%edx, %%eax")
                    __ASM_EMIT("and $0xaaaaaaaa, %%eax")
                    __ASM_EMIT("and $0x55555555, %%edx")
                    __ASM_EMIT("shr $1, %%eax")
                    __ASM_EMIT("lea (%%eax, %%edx, 4), %%eax")

                    __ASM_EMIT("xor %%edx, %%edx")
                    __ASM_EMIT("shr %%cl, %%eax")

                    : "+A"(v)
                    : "c" (64-count)
                    : "cc"
                );
            }

            return v;
        }
    #else
        inline uint64_t __lsp_forced_inline    reverse_bits(uint64_t v)
        {
            register uint64_t tmp;

            __asm__ __volatile__ (
                __ASM_EMIT("bswap %0")

                __ASM_EMIT("mov %0, %1")
                __ASM_EMIT("and %2, %1")
                __ASM_EMIT("shr $4, %0")
                __ASM_EMIT("shl $4, %1")
                __ASM_EMIT("and %2, %0")
                __ASM_EMIT("or  %1, %0")

                __ASM_EMIT("mov %0, %1")
                __ASM_EMIT("and %3, %1")
                __ASM_EMIT("shr $2, %0")
                __ASM_EMIT("and %3, %0")
                __ASM_EMIT("lea (%0,%1,4), %0")

                __ASM_EMIT("mov %0, %1")
                __ASM_EMIT("and %4, %1")
                __ASM_EMIT("shr $1, %0")
                __ASM_EMIT("and %4, %0")
                __ASM_EMIT("lea (%0,%1,2), %0")

                : "+r"(v), "=&r"(tmp)
                :
                "r"(0x0f0f0f0f0f0f0f0fULL),
                "r"(0x3333333333333333ULL),
                "r"(0x5555555555555555ULL)
                : "cc"
            );

            return v;
        }

        inline uint64_t __lsp_forced_inline    reverse_bits(uint64_t v, size_t count)
        {
            register uint64_t tmp;

            __asm__ __volatile__ (
                __ASM_EMIT("bswap %0")

                __ASM_EMIT("mov %0, %1")
                __ASM_EMIT("and %2, %1")
                __ASM_EMIT("shr $4, %0")
                __ASM_EMIT("shl $4, %1")
                __ASM_EMIT("and %2, %0")
                __ASM_EMIT("or  %1, %0")

                __ASM_EMIT("mov %0, %1")
                __ASM_EMIT("and %3, %1")
                __ASM_EMIT("shr $2, %0")
                __ASM_EMIT("and %3, %0")
                __ASM_EMIT("lea (%0,%1,4), %0")

                __ASM_EMIT("mov %0, %1")
                __ASM_EMIT("and %4, %1")
                __ASM_EMIT("shr $1, %0")
                __ASM_EMIT("and %4, %0")
                __ASM_EMIT("lea (%0,%1,2), %0")

                __ASM_EMIT("shr %%cl, %0")

                : "+r"(v), "=&r"(tmp)
                :
                "r"(0x0f0f0f0f0f0f0f0fULL),
                "r"(0x3333333333333333ULL),
                "r"(0x5555555555555555ULL),
                "c"(64-count)
                : "cc"
            );

            return v;
        }

    #endif /* __i386__ */
}

#endif /* CORE_X86_BITS_H_ */
