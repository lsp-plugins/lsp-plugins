/*
 * bswap.h
 *
 *  Created on: 04 мар. 2016 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_BSWAP_H_
#define DSP_ARCH_X86_BSWAP_H_

inline uint16_t __lsp_forced_inline    byte_swap(uint16_t v)
{
    ARCH_X86_ASM (
        __ASM_EMIT("ror $8, %[v]")
        : [v] "+r"(v)
        : : "cc"
    );
    return v;
}

inline uint32_t __lsp_forced_inline    byte_swap(uint32_t v)
{
    ARCH_X86_ASM (
        __ASM_EMIT("bswap %[v]")
        : [v] "+r"(v)
        : :
    );
    return v;
}

inline float __lsp_forced_inline    byte_swap(float v)
{
    ARCH_X86_ASM (
        __ASM_EMIT("bswap %[v]")
        : [v] "+r"(v)
        : :
    );
    return v;
}

#ifdef ARCH_X86_64
    inline uint64_t __lsp_forced_inline    byte_swap(uint64_t v)
    {
        ARCH_X86_ASM (
            __ASM_EMIT("bswap %[v]")
            : [v] "+r"(v)
            : :
        );
        return v;
    }

    inline double __lsp_forced_inline    byte_swap(double v)
    {
        ARCH_X86_ASM (
            __ASM_EMIT("bswap %[v]")
            : [v] "+r"(v)
            : :
        );
        return v;
    }

#else /* ARCH_I386 */
    inline uint64_t __lsp_forced_inline    byte_swap(uint64_t v)
    {
        ARCH_X86_ASM (
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
        ARCH_X86_ASM (
            __ASM_EMIT("bswap %%eax")
            __ASM_EMIT("bswap %%edx")
            __ASM_EMIT("xchg %%edx, %%eax")
            : "+A"(v)
            : :
        );
        return v;
    }

#endif  /* ARCH_X86_64 */

inline void __lsp_forced_inline    byte_swap(uint16_t *v, size_t n)
{
    uint16_t tmp;
    ARCH_X86_ASM (
        __ASM_EMIT("test    %[n], %[n]")
        __ASM_EMIT("jz      2f")
        __ASM_EMIT("1:")
        __ASM_EMIT("lodsw")
        __ASM_EMIT("ror     $8, %[tmp]")
        __ASM_EMIT("dec     %[n]")
        __ASM_EMIT("mov     %[tmp], -2(%[v])")
        __ASM_EMIT("jnz     1b")
        __ASM_EMIT("2:")
        : [v] "+S"(v), [n] "+r" (n), [tmp] "=&a"(tmp)
        : : "cc", "memory"
    );
}

inline void __lsp_forced_inline    byte_swap(int16_t *v, size_t n)
{
    uint16_t tmp;
    ARCH_X86_ASM (
        __ASM_EMIT("test    %[n], %[n]")
        __ASM_EMIT("jz      2f")
        __ASM_EMIT("1:")
        __ASM_EMIT("lodsw")
        __ASM_EMIT("ror     $8, %[tmp]")
        __ASM_EMIT("dec     %[n]")
        __ASM_EMIT("mov     %[tmp], -2(%[v])")
        __ASM_EMIT("jnz     1b")
        __ASM_EMIT("2:")
        : [v] "+S"(v), [n] "+r" (n), [tmp] "=&a"(tmp)
        : : "cc", "memory"
    );
}

inline void __lsp_forced_inline    byte_swap(uint32_t *v, size_t n)
{
    uint32_t tmp;
    ARCH_X86_ASM (
        __ASM_EMIT("test    %[n], %[n]")
        __ASM_EMIT("jz      2f")
        __ASM_EMIT("1:")
        __ASM_EMIT("lodsl")
        __ASM_EMIT("bswap   %[tmp]")
        __ASM_EMIT("dec     %[n]")
        __ASM_EMIT("mov     %[tmp], -4(%[v])")
        __ASM_EMIT("jnz     1b")
        __ASM_EMIT("2:")
        : [v] "+S"(v), [n] "+r" (n), [tmp] "=&a"(tmp)
        : : "cc", "memory"
    );
}

inline void __lsp_forced_inline    byte_swap(int32_t *v, size_t n)
{
    uint32_t tmp;
    ARCH_X86_ASM (
        __ASM_EMIT("test    %[n], %[n]")
        __ASM_EMIT("jz      2f")
        __ASM_EMIT("1:")
        __ASM_EMIT("lodsl")
        __ASM_EMIT("bswap   %[tmp]")
        __ASM_EMIT("dec     %[n]")
        __ASM_EMIT("mov     %[tmp], -4(%[v])")
        __ASM_EMIT("jnz     1b")
        __ASM_EMIT("2:")
        : [v] "+S"(v), [n] "+r" (n), [tmp] "=&a"(tmp)
        : : "cc", "memory"
    );
}

inline void __lsp_forced_inline    byte_swap(float *v, size_t n)
{
    uint32_t tmp;
    ARCH_X86_ASM (
        __ASM_EMIT("test    %[n], %[n]")
        __ASM_EMIT("jz      2f")
        __ASM_EMIT("1:")
        __ASM_EMIT("lodsl")
        __ASM_EMIT("bswap   %[tmp]")
        __ASM_EMIT("dec     %[n]")
        __ASM_EMIT("mov     %[tmp], -4(%[v])")
        __ASM_EMIT("jnz     1b")
        __ASM_EMIT("2:")
        : [v] "+S"(v), [n] "+r" (n), [tmp] "=&a"(tmp)
        : : "cc", "memory"
    );
}

#ifdef ARCH_X86_64
    inline void __lsp_forced_inline    byte_swap(uint64_t *v, size_t n)
    {
        uint64_t tmp;
        ARCH_X86_ASM (
            __ASM_EMIT("test    %[n], %[n]")
            __ASM_EMIT("jz      2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("lodsq")
            __ASM_EMIT("bswap   %[tmp]")
            __ASM_EMIT("dec     %[n]")
            __ASM_EMIT("mov     %[tmp], -8(%[v])")
            __ASM_EMIT("jnz     1b")
            __ASM_EMIT("2:")
            : [v] "+S"(v), [n] "+r" (n), [tmp] "=&a"(tmp)
            : : "cc", "memory"
        );
    }

    inline void __lsp_forced_inline    byte_swap(int64_t *v, size_t n)
    {
        uint64_t tmp;
        ARCH_X86_ASM (
            __ASM_EMIT("test    %[n], %[n]")
            __ASM_EMIT("jz      2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("lodsq")
            __ASM_EMIT("bswap   %[tmp]")
            __ASM_EMIT("dec     %[n]")
            __ASM_EMIT("mov     %[tmp], -8(%[v])")
            __ASM_EMIT("jnz     1b")
            __ASM_EMIT("2:")
            : [v] "+S"(v), [n] "+r" (n), [tmp] "=&a"(tmp)
            : : "cc", "memory"
        );
    }

    inline void __lsp_forced_inline    byte_swap(double *v, size_t n)
    {
        uint64_t tmp;
        ARCH_X86_ASM (
            __ASM_EMIT("test    %[n], %[n]")
            __ASM_EMIT("jz      2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("lodsq")
            __ASM_EMIT("bswap   %[tmp]")
            __ASM_EMIT("dec     %[n]")
            __ASM_EMIT("mov     %[tmp], -8(%[v])")
            __ASM_EMIT("jnz     1b")
            __ASM_EMIT("2:")
            : [v] "+S"(v), [n] "+r" (n), [tmp] "=&a"(tmp)
            : : "cc", "memory"
        );
    }

#else /* ARCH_I386 */

    inline void __lsp_forced_inline    byte_swap(uint64_t *v, size_t n)
    {
        uint32_t a, b;
        ARCH_X86_ASM (
            __ASM_EMIT("test    %[n], %[n]")
            __ASM_EMIT("jz      2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("mov     (%[v]), %[a]")
            __ASM_EMIT("mov     4(%[v]), %[b]")
            __ASM_EMIT("bswap   %[a]")
            __ASM_EMIT("bswap   %[b]")
            __ASM_EMIT("mov     %[b], (%[v])")
            __ASM_EMIT("mov     %[a], 4(%[v])")
            __ASM_EMIT("add     $8, %[v]")
            __ASM_EMIT("dec     %[n]")
            __ASM_EMIT("jnz     1b")
            __ASM_EMIT("2:")
            : [v] "+r"(v), [n] "+r" (n), [a] "=&r" (a), [b] "=&r" (b)
            : : "cc", "memory"
        );
    }

    inline void __lsp_forced_inline    byte_swap(int64_t *v, size_t n)
    {
        uint32_t a, b;
        ARCH_X86_ASM (
            __ASM_EMIT("test    %[n], %[n]")
            __ASM_EMIT("jz      2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("mov     (%[v]), %[a]")
            __ASM_EMIT("mov     4(%[v]), %[b]")
            __ASM_EMIT("bswap   %[a]")
            __ASM_EMIT("bswap   %[b]")
            __ASM_EMIT("mov     %[b], (%[v])")
            __ASM_EMIT("mov     %[a], 4(%[v])")
            __ASM_EMIT("add     $8, %[v]")
            __ASM_EMIT("dec     %[n]")
            __ASM_EMIT("jnz     1b")
            __ASM_EMIT("2:")
            : [v] "+r"(v), [n] "+r" (n), [a] "=&r" (a), [b] "=&r" (b)
            : : "cc", "memory"
        );
    }

    inline void __lsp_forced_inline    byte_swap(double *v, size_t n)
    {
        uint32_t a, b;
        ARCH_X86_ASM (
            __ASM_EMIT("test    %[n], %[n]")
            __ASM_EMIT("jz      2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("mov     (%[v]), %[a]")
            __ASM_EMIT("mov     4(%[v]), %[b]")
            __ASM_EMIT("bswap   %[a]")
            __ASM_EMIT("bswap   %[b]")
            __ASM_EMIT("mov     %[b], (%[v])")
            __ASM_EMIT("mov     %[a], 4(%[v])")
            __ASM_EMIT("add     $8, %[v]")
            __ASM_EMIT("dec     %[n]")
            __ASM_EMIT("jnz     1b")
            __ASM_EMIT("2:")
            : [v] "+r"(v), [n] "+r" (n), [a] "=&r"(a), [b] "=&r"(b)
            : : "cc", "memory"
        );
    }
#endif /* ARCH_X86_64 */

#endif /* DSP_ARCH_X86_BSWAP_H_ */
