/*
 * bswap.h
 *
 *  Created on: 14 авг. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_ARM_BSWAP_H_
#define DSP_ARCH_ARM_BSWAP_H_

inline uint16_t __lsp_forced_inline    byte_swap(uint16_t v)
{
    ARCH_ARM_ASM (
        __ASM_EMIT("rev16   %[v], %[v]")
        : [v] "+r"(v)
        : :
    );
    return v;
}

inline uint32_t __lsp_forced_inline    byte_swap(uint32_t v)
{
    ARCH_ARM_ASM (
        __ASM_EMIT("rev     %[v], %[v]")
        : [v] "+r"(v)
        : :
    );
    return v;
}

inline float __lsp_forced_inline    byte_swap(float v)
{
    ARCH_ARM_ASM (
        __ASM_EMIT("rev         %[v], %[v]")
        : [v] "+r"(v)
        : :
    );
    return v;
}

inline uint64_t __lsp_forced_inline    byte_swap(uint64_t v)
{
    uint32_t lo = uint32_t(v);
    uint32_t hi = uint32_t(v >> 32);
    ARCH_ARM_ASM (
        __ASM_EMIT("rev     %[lo], %[lo]")
        __ASM_EMIT("rev     %[hi], %[hi]")
        : [lo] "+r"(lo), [hi] "+r"(hi)
        : :
    );
    return (uint64_t(lo) << 32) | hi;
}

inline double __lsp_forced_inline    byte_swap(double v)
{
    #pragma pack(push, 1)
    union
    {
        struct { uint32_t lo, hi; } w;
        double d;
    } x;
    #pragma pack(pop)

    x.d = v;
    ARCH_ARM_ASM (
        __ASM_EMIT("rev     %[lo], %[lo]")
        __ASM_EMIT("rev     %[hi], %[hi]")
        : [lo] "+r"(x.w.lo), [hi] "+r"(x.w.hi)
        : :
    );
    return x.d;
}

inline void __lsp_forced_inline    byte_swap(uint16_t *v, size_t n)
{
    uint16_t tmp;
    ARCH_ARM_ASM (
        __ASM_EMIT("subs        %[n], #2")
        __ASM_EMIT("blo         2f")

        __ASM_EMIT("1:")
        __ASM_EMIT("ldr         %[tmp], [%[v]]")
        __ASM_EMIT("rev16       %[tmp], %[tmp]")
        __ASM_EMIT("str         %[tmp], [%[v]], #4")
        __ASM_EMIT("subs        %[n], #2")
        __ASM_EMIT("bge         1b")

        __ASM_EMIT("2:")
        __ASM_EMIT("adds        %[n], #1")
        __ASM_EMIT("blt         3f")
        __ASM_EMIT("ldrh        %[tmp], [%[v]]")
        __ASM_EMIT("rev16       %[tmp], %[tmp]")
        __ASM_EMIT("strh        %[tmp], [%[v]]")

        __ASM_EMIT("3:")
        : [v] "+r"(v), [n] "+r" (n), [tmp] "=&r"(tmp)
        : : "cc", "memory"
    );
}

inline void __lsp_forced_inline    byte_swap(int16_t *v, size_t n)
{
    uint16_t tmp;
    ARCH_ARM_ASM (
        __ASM_EMIT("subs        %[n], #2")
        __ASM_EMIT("blo         2f")

        __ASM_EMIT("1:")
        __ASM_EMIT("ldr         %[tmp], [%[v]]")
        __ASM_EMIT("rev16       %[tmp], %[tmp]")
        __ASM_EMIT("str         %[tmp], [%[v]], #4")
        __ASM_EMIT("subs        %[n], #2")
        __ASM_EMIT("bge         1b")

        __ASM_EMIT("2:")
        __ASM_EMIT("adds        %[n], #1")
        __ASM_EMIT("blt         3f")
        __ASM_EMIT("ldrh        %[tmp], [%[v]]")
        __ASM_EMIT("rev16       %[tmp], %[tmp]")
        __ASM_EMIT("strh        %[tmp], [%[v]]")

        __ASM_EMIT("3:")
        : [v] "+r"(v), [n] "+r" (n), [tmp] "=&r"(tmp)
        : : "cc", "memory"
    );
}

inline void __lsp_forced_inline    byte_swap(uint32_t *v, size_t n)
{
    uint32_t tmp;
    ARCH_ARM_ASM (
        __ASM_EMIT("cmp         %[n], #0")
        __ASM_EMIT("bls         2f")

        __ASM_EMIT("1:")
        __ASM_EMIT("ldr         %[tmp], [%[v]]")
        __ASM_EMIT("rev         %[tmp], %[tmp]")
        __ASM_EMIT("str         %[tmp], [%[v]], #4")
        __ASM_EMIT("subs        %[n], #1")
        __ASM_EMIT("bhi         1b")

        __ASM_EMIT("2:")
        : [v] "+r"(v), [n] "+r" (n), [tmp] "=&r"(tmp)
        : : "cc", "memory"
    );
}

inline void __lsp_forced_inline    byte_swap(int32_t *v, size_t n)
{
    uint32_t tmp;
    ARCH_ARM_ASM (
        __ASM_EMIT("cmp         %[n], #0")
        __ASM_EMIT("bls         2f")

        __ASM_EMIT("1:")
        __ASM_EMIT("ldr         %[tmp], [%[v]]")
        __ASM_EMIT("rev         %[tmp], %[tmp]")
        __ASM_EMIT("str         %[tmp], [%[v]], #4")
        __ASM_EMIT("subs        %[n], #1")
        __ASM_EMIT("bhi         1b")

        __ASM_EMIT("2:")
        : [v] "+r"(v), [n] "+r" (n), [tmp] "=&r"(tmp)
        : : "cc", "memory"
    );
}

inline void __lsp_forced_inline    byte_swap(float *v, size_t n)
{
    uint32_t tmp;
    ARCH_ARM_ASM (
        __ASM_EMIT("cmp         %[n], #0")
        __ASM_EMIT("bls         2f")

        __ASM_EMIT("1:")
        __ASM_EMIT("ldr         %[tmp], [%[v]]")
        __ASM_EMIT("rev         %[tmp], %[tmp]")
        __ASM_EMIT("str         %[tmp], [%[v]], #4")
        __ASM_EMIT("subs        %[n], #1")
        __ASM_EMIT("bhi         1b")

        __ASM_EMIT("2:")
        : [v] "+r"(v), [n] "+r" (n), [tmp] "=&r"(tmp)
        : : "cc", "memory"
    );
}

inline void __lsp_forced_inline    byte_swap(uint64_t *v, size_t n)
{
    uint32_t tmp1, tmp2;
    ARCH_ARM_ASM (
        __ASM_EMIT("cmp         %[n], #0")
        __ASM_EMIT("bls         2f")

        __ASM_EMIT("1:")
        __ASM_EMIT("ldr         %[tmp1], [%[v]]")
        __ASM_EMIT("ldr         %[tmp2], [%[v], #4]")
        __ASM_EMIT("rev         %[tmp1], %[tmp1]")
        __ASM_EMIT("rev         %[tmp2], %[tmp2]")
        __ASM_EMIT("str         %[tmp2], [%[v]], #4")
        __ASM_EMIT("str         %[tmp1], [%[v]], #4")
        __ASM_EMIT("subs        %[n], #1")
        __ASM_EMIT("bhi         1b")

        __ASM_EMIT("2:")
        : [v] "+r"(v), [n] "+r" (n), [tmp1] "=&r"(tmp1), [tmp2] "=&r"(tmp2)
        : : "cc", "memory"
    );
}

inline void __lsp_forced_inline    byte_swap(int64_t *v, size_t n)
{
    uint32_t tmp1, tmp2;
    ARCH_ARM_ASM (
        __ASM_EMIT("cmp         %[n], #0")
        __ASM_EMIT("bls         2f")

        __ASM_EMIT("1:")
        __ASM_EMIT("ldr         %[tmp1], [%[v]]")
        __ASM_EMIT("ldr         %[tmp2], [%[v], #4]")
        __ASM_EMIT("rev         %[tmp1], %[tmp1]")
        __ASM_EMIT("rev         %[tmp2], %[tmp2]")
        __ASM_EMIT("str         %[tmp2], [%[v]], #4")
        __ASM_EMIT("str         %[tmp1], [%[v]], #4")
        __ASM_EMIT("subs        %[n], #1")
        __ASM_EMIT("bhi         1b")

        __ASM_EMIT("2:")
        : [v] "+r"(v), [n] "+r" (n), [tmp1] "=&r"(tmp1), [tmp2] "=&r"(tmp2)
        : : "cc", "memory"
    );
}

inline void __lsp_forced_inline    byte_swap(double *v, size_t n)
{
    uint32_t tmp1, tmp2;
    ARCH_ARM_ASM (
        __ASM_EMIT("cmp         %[n], #0")
        __ASM_EMIT("bls         2f")

        __ASM_EMIT("1:")
        __ASM_EMIT("ldr         %[tmp1], [%[v]]")
        __ASM_EMIT("ldr         %[tmp2], [%[v], #4]")
        __ASM_EMIT("rev         %[tmp1], %[tmp1]")
        __ASM_EMIT("rev         %[tmp2], %[tmp2]")
        __ASM_EMIT("str         %[tmp2], [%[v]], #4")
        __ASM_EMIT("str         %[tmp1], [%[v]], #4")
        __ASM_EMIT("subs        %[n], #1")
        __ASM_EMIT("bhi         1b")

        __ASM_EMIT("2:")
        : [v] "+r"(v), [n] "+r" (n), [tmp1] "=&r"(tmp1), [tmp2] "=&r"(tmp2)
        : : "cc", "memory"
    );
}

#endif /* DSP_ARCH_ARM_BSWAP_H_ */
