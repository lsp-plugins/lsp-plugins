/*
 * minmax.h
 *
 *  Created on: 5 июл. 2020 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_SSE_PMATH_MINMAX_H_
#define DSP_ARCH_X86_SSE_PMATH_MINMAX_H_

#ifndef DSP_ARCH_X86_SSE_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_SSE_IMPL */

namespace sse
{
    #define MINMAX_CORE(DST, A, B, OP) \
        __ASM_EMIT("xor         %[off], %[off]") \
        __ASM_EMIT("sub         $16, %[count]") \
        __ASM_EMIT("jb          2f")    \
        /* 16x blocks */ \
        __ASM_EMIT("1:") \
        __ASM_EMIT("movups      0x00(%[" A "], %[off]), %%xmm0") \
        __ASM_EMIT("movups      0x10(%[" A "], %[off]), %%xmm1") \
        __ASM_EMIT("movups      0x20(%[" A "], %[off]), %%xmm2") \
        __ASM_EMIT("movups      0x30(%[" A "], %[off]), %%xmm3") \
        __ASM_EMIT("movups      0x00(%[" B "], %[off]), %%xmm4") \
        __ASM_EMIT("movups      0x10(%[" B "], %[off]), %%xmm5") \
        __ASM_EMIT("movups      0x20(%[" B "], %[off]), %%xmm6") \
        __ASM_EMIT("movups      0x30(%[" B "], %[off]), %%xmm7") \
        __ASM_EMIT(OP "         %%xmm4, %%xmm0") \
        __ASM_EMIT(OP "         %%xmm5, %%xmm1") \
        __ASM_EMIT(OP "         %%xmm6, %%xmm2") \
        __ASM_EMIT(OP "         %%xmm7, %%xmm3") \
        __ASM_EMIT("movups      %%xmm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("movups      %%xmm1, 0x10(%[" DST "], %[off])") \
        __ASM_EMIT("movups      %%xmm2, 0x20(%[" DST "], %[off])") \
        __ASM_EMIT("movups      %%xmm3, 0x30(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x40, %[off]") \
        __ASM_EMIT("sub         $16, %[count]") \
        __ASM_EMIT("jae         1b") \
        \
        /* 8x block */ \
        __ASM_EMIT("2:") \
        __ASM_EMIT("add         $8, %[count]") \
        __ASM_EMIT("jl          4f") \
        __ASM_EMIT("movups      0x00(%[" A "], %[off]), %%xmm0") \
        __ASM_EMIT("movups      0x10(%[" A "], %[off]), %%xmm1") \
        __ASM_EMIT("movups      0x00(%[" B "], %[off]), %%xmm4") \
        __ASM_EMIT("movups      0x10(%[" B "], %[off]), %%xmm5") \
        __ASM_EMIT(OP "         %%xmm4, %%xmm0") \
        __ASM_EMIT(OP "         %%xmm5, %%xmm1") \
        __ASM_EMIT("movups      %%xmm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("movups      %%xmm1, 0x10(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x20, %[off]") \
        __ASM_EMIT("sub         $8, %[count]") \
        /* 4x block */ \
        __ASM_EMIT("4:") \
        __ASM_EMIT("add         $4, %[count]") \
        __ASM_EMIT("jl          6f") \
        __ASM_EMIT("movups      0x00(%[" A "], %[off]), %%xmm0") \
        __ASM_EMIT("movups      0x00(%[" B "], %[off]), %%xmm4") \
        __ASM_EMIT(OP "         %%xmm4, %%xmm0") \
        __ASM_EMIT("movups      %%xmm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x10, %[off]") \
        __ASM_EMIT("sub         $4, %[count]") \
        /* 1x blocks */ \
        __ASM_EMIT("6:") \
        __ASM_EMIT("add         $3, %[count]") \
        __ASM_EMIT("jl          8f")    \
        __ASM_EMIT("7:") \
        __ASM_EMIT("movss       0x00(%[" A "], %[off]), %%xmm0") \
        __ASM_EMIT("movss       0x00(%[" B "], %[off]), %%xmm4") \
        __ASM_EMIT(OP "         %%xmm4, %%xmm0") \
        __ASM_EMIT("movss       %%xmm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x04, %[off]") \
        __ASM_EMIT("dec         %[count]") \
        __ASM_EMIT("jge         7b") \
        __ASM_EMIT("8:")

    void pmin2(float *dst, const float *src, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            MINMAX_CORE("dst", "dst", "src", "minps")
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r" (dst), [src] "r" (src)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void pmin3(float *dst, const float *a, const float *b, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            MINMAX_CORE("dst", "a", "b", "minps")
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r" (dst), [a] "r" (a), [b] "r" (b)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void pmax2(float *dst, const float *src, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            MINMAX_CORE("dst", "dst", "src", "maxps")
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r" (dst), [src] "r" (src)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void pmax3(float *dst, const float *a, const float *b, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            MINMAX_CORE("dst", "a", "b", "maxps")
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r" (dst), [a] "r" (a), [b] "r" (b)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    #undef MINMAX_CORE

    #define SIGN_MINMAX_CORE(DST, A, B, CMP) \
        __ASM_EMIT("movaps      %[X_SIGN], %%xmm6") \
        __ASM_EMIT("xor         %[off], %[off]") \
        __ASM_EMIT("movaps      %%xmm6, %%xmm7") \
        __ASM_EMIT("sub         $4, %[count]") \
        __ASM_EMIT("jb          2f")    \
        /* 4x blocks */ \
        __ASM_EMIT("1:") \
        __ASM_EMIT("movups      0x00(%[" A "], %[off]), %%xmm0") \
        __ASM_EMIT("movups      0x00(%[" B "], %[off]), %%xmm1") \
        __ASM_EMIT("movaps      %%xmm0, %%xmm2") \
        __ASM_EMIT("movaps      %%xmm1, %%xmm3") \
        __ASM_EMIT("andps       %%xmm6, %%xmm2")            /* xmm2 = abs(a) */ \
        __ASM_EMIT("andps       %%xmm7, %%xmm3")            /* xmm3 = abs(b) */\
        __ASM_EMIT("cmpps       " CMP ", %%xmm3, %%xmm2")   /* xmm2 = abs(a) <=> abs(b) */ \
        __ASM_EMIT("andps       %%xmm2, %%xmm0")            /* xmm0 = a & (abs(a) <=> abs(b)) */ \
        __ASM_EMIT("andnps      %%xmm1, %%xmm2")            /* xmm2 = b & ~(abs(a) <=> abs(b)) */ \
        __ASM_EMIT("orps        %%xmm2, %%xmm0")            /* xmm0 = (a & (abs(a) <=> abs(b))) | (b & ~(abs(a) <=> abs(b))) */ \
        __ASM_EMIT("movups      %%xmm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x10, %[off]") \
        __ASM_EMIT("sub         $4, %[count]") \
        __ASM_EMIT("jae         1b") \
        \
        /* 1x blocks */ \
        __ASM_EMIT("2:") \
        __ASM_EMIT("add         $3, %[count]") \
        __ASM_EMIT("jl          4f")    \
        __ASM_EMIT("3:") \
        __ASM_EMIT("movss       0x00(%[" A "], %[off]), %%xmm0") \
        __ASM_EMIT("movss       0x00(%[" B "], %[off]), %%xmm1") \
        __ASM_EMIT("movaps      %%xmm0, %%xmm2") \
        __ASM_EMIT("movaps      %%xmm1, %%xmm3") \
        __ASM_EMIT("andps       %%xmm6, %%xmm2")            /* xmm2 = abs(a) */ \
        __ASM_EMIT("andps       %%xmm7, %%xmm3")            /* xmm3 = abs(b) */\
        __ASM_EMIT("cmpps       " CMP ", %%xmm3, %%xmm2")   /* xmm2 = abs(a) <=> abs(b) */ \
        __ASM_EMIT("andps       %%xmm2, %%xmm0")            /* xmm0 = a & (abs(a) <=> abs(b)) */ \
        __ASM_EMIT("andnps      %%xmm1, %%xmm2")            /* xmm2 = b & ~(abs(a) <=> abs(b)) */ \
        __ASM_EMIT("orps        %%xmm2, %%xmm0")            /* xmm0 = (a & (abs(a) <=> abs(b))) | (b & ~(abs(a) <=> abs(b))) */ \
        __ASM_EMIT("movss       %%xmm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x04, %[off]") \
        __ASM_EMIT("dec         %[count]") \
        __ASM_EMIT("jge         3b") \
        __ASM_EMIT("4:")

    void psmin2(float *dst, const float *src, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            SIGN_MINMAX_CORE("dst", "dst", "src", "$1")
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r" (dst), [src] "r" (src),
              [X_SIGN] "m" (X_SIGN)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm6", "%xmm7"
        );
    }

    void psmin3(float *dst, const float *a, const float *b, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            SIGN_MINMAX_CORE("dst", "a", "b", "$1")
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r" (dst), [a] "r" (a), [b] "r" (b),
              [X_SIGN] "m" (X_SIGN)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm6", "%xmm7"
        );
    }

    void psmax2(float *dst, const float *src, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            SIGN_MINMAX_CORE("dst", "dst", "src", "$6")
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r" (dst), [src] "r" (src),
              [X_SIGN] "m" (X_SIGN)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm6", "%xmm7"
        );
    }

    void psmax3(float *dst, const float *a, const float *b, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            SIGN_MINMAX_CORE("dst", "a", "b", "$6")
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r" (dst), [a] "r" (a), [b] "r" (b),
              [X_SIGN] "m" (X_SIGN)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm6", "%xmm7"
        );
    }

    #undef SIGN_MINMAX_CORE

    #define ABS_MINMAX_CORE(DST, A, B, OP) \
        __ASM_EMIT("movaps      %[X_SIGN], %%xmm6") \
        __ASM_EMIT("xor         %[off], %[off]") \
        __ASM_EMIT("movaps      %%xmm6, %%xmm7") \
        __ASM_EMIT("sub         $12, %[count]") \
        __ASM_EMIT("jb          2f")    \
        /* 12x blocks */ \
        __ASM_EMIT("1:") \
        __ASM_EMIT("movups      0x00(%[" A "], %[off]), %%xmm0") \
        __ASM_EMIT("movups      0x10(%[" A "], %[off]), %%xmm1") \
        __ASM_EMIT("movups      0x20(%[" A "], %[off]), %%xmm2") \
        __ASM_EMIT("movups      0x00(%[" B "], %[off]), %%xmm3") \
        __ASM_EMIT("movups      0x10(%[" B "], %[off]), %%xmm4") \
        __ASM_EMIT("movups      0x20(%[" B "], %[off]), %%xmm5") \
        __ASM_EMIT("andps       %%xmm6, %%xmm0") \
        __ASM_EMIT("andps       %%xmm7, %%xmm1") \
        __ASM_EMIT("andps       %%xmm6, %%xmm2") \
        __ASM_EMIT("andps       %%xmm7, %%xmm3") \
        __ASM_EMIT("andps       %%xmm6, %%xmm4") \
        __ASM_EMIT("andps       %%xmm7, %%xmm5") \
        __ASM_EMIT(OP "         %%xmm3, %%xmm0") \
        __ASM_EMIT(OP "         %%xmm4, %%xmm1") \
        __ASM_EMIT(OP "         %%xmm5, %%xmm2") \
        __ASM_EMIT("movups      %%xmm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("movups      %%xmm1, 0x10(%[" DST "], %[off])") \
        __ASM_EMIT("movups      %%xmm2, 0x20(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x30, %[off]") \
        __ASM_EMIT("sub         $12, %[count]") \
        __ASM_EMIT("jae         1b") \
        \
        /* 8x block */ \
        __ASM_EMIT("2:") \
        __ASM_EMIT("add         $4, %[count]") \
        __ASM_EMIT("jl          4f") \
        __ASM_EMIT("movups      0x00(%[" A "], %[off]), %%xmm0") \
        __ASM_EMIT("movups      0x10(%[" A "], %[off]), %%xmm1") \
        __ASM_EMIT("movups      0x00(%[" B "], %[off]), %%xmm3") \
        __ASM_EMIT("movups      0x10(%[" B "], %[off]), %%xmm4") \
        __ASM_EMIT("andps       %%xmm6, %%xmm0") \
        __ASM_EMIT("andps       %%xmm7, %%xmm1") \
        __ASM_EMIT("andps       %%xmm7, %%xmm3") \
        __ASM_EMIT("andps       %%xmm6, %%xmm4") \
        __ASM_EMIT(OP "         %%xmm3, %%xmm0") \
        __ASM_EMIT(OP "         %%xmm4, %%xmm1") \
        __ASM_EMIT("movups      %%xmm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("movups      %%xmm1, 0x10(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x20, %[off]") \
        __ASM_EMIT("sub         $8, %[count]") \
        /* 4x block */ \
        __ASM_EMIT("4:") \
        __ASM_EMIT("add         $4, %[count]") \
        __ASM_EMIT("jl          6f") \
        __ASM_EMIT("movups      0x00(%[" A "], %[off]), %%xmm0") \
        __ASM_EMIT("movups      0x00(%[" B "], %[off]), %%xmm3") \
        __ASM_EMIT("andps       %%xmm6, %%xmm0") \
        __ASM_EMIT("andps       %%xmm7, %%xmm3") \
        __ASM_EMIT(OP "         %%xmm3, %%xmm0") \
        __ASM_EMIT("movups      %%xmm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x10, %[off]") \
        __ASM_EMIT("sub         $4, %[count]") \
        /* 1x blocks */ \
        __ASM_EMIT("6:") \
        __ASM_EMIT("add         $3, %[count]") \
        __ASM_EMIT("jl          8f")    \
        __ASM_EMIT("7:") \
        __ASM_EMIT("movss       0x00(%[" A "], %[off]), %%xmm0") \
        __ASM_EMIT("movss       0x00(%[" B "], %[off]), %%xmm3") \
        __ASM_EMIT("andps       %%xmm6, %%xmm0") \
        __ASM_EMIT("andps       %%xmm7, %%xmm3") \
        __ASM_EMIT(OP "         %%xmm3, %%xmm0") \
        __ASM_EMIT("movss       %%xmm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x04, %[off]") \
        __ASM_EMIT("dec         %[count]") \
        __ASM_EMIT("jge         7b") \
        __ASM_EMIT("8:")

    void pamin2(float *dst, const float *src, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            ABS_MINMAX_CORE("dst", "dst", "src", "minps")
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r" (dst), [src] "r" (src),
              [X_SIGN] "m" (X_SIGN)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void pamin3(float *dst, const float *a, const float *b, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            ABS_MINMAX_CORE("dst", "a", "b", "minps")
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r" (dst), [a] "r" (a), [b] "r" (b),
              [X_SIGN] "m" (X_SIGN)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void pamax2(float *dst, const float *src, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            ABS_MINMAX_CORE("dst", "dst", "src", "maxps")
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r" (dst), [src] "r" (src),
              [X_SIGN] "m" (X_SIGN)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void pamax3(float *dst, const float *a, const float *b, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            ABS_MINMAX_CORE("dst", "a", "b", "maxps")
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r" (dst), [a] "r" (a), [b] "r" (b),
              [X_SIGN] "m" (X_SIGN)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    #undef ABS_MINMAX_CORE
}

#endif /* DSP_ARCH_X86_SSE_PMATH_MINMAX_H_ */
