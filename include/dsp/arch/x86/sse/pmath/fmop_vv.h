/*
 * fmop_vv.h
 *
 *  Created on: 22 нояб. 2019 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_SSE_PMATH_FMOP_VV_H_
#define DSP_ARCH_X86_SSE_PMATH_FMOP_VV_H_

#ifndef DSP_ARCH_X86_SSE_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_SSE_IMPL */

namespace sse
{
#define OP_DSEL(a, b)   a
#define OP_RSEL(a, b)   b

#define FMOP_VV_CORE(DST, A, B, C, OP, SEL) \
    __ASM_EMIT("xor         %[off], %[off]") \
    __ASM_EMIT("sub         $8, %[count]") \
    __ASM_EMIT("jb          2f")    \
    /* 8x blocks */ \
    __ASM_EMIT("1:") \
    __ASM_EMIT("movups      0x00(%[" A "], %[off]), %%xmm0") \
    __ASM_EMIT("movups      0x10(%[" A "], %[off]), %%xmm1") \
    __ASM_EMIT("movups      0x00(%[" B "], %[off]), %%xmm2") \
    __ASM_EMIT("movups      0x10(%[" B "], %[off]), %%xmm3") \
    __ASM_EMIT("movups      0x00(%[" C "], %[off]), %%xmm4") \
    __ASM_EMIT("movups      0x10(%[" C "], %[off]), %%xmm5") \
    __ASM_EMIT("mulps       %%xmm2, %%xmm4") \
    __ASM_EMIT("mulps       %%xmm3, %%xmm5") \
    __ASM_EMIT(OP "ps       " SEL("%%xmm4", "%%xmm0") ", " SEL("%%xmm0", "%%xmm4")) \
    __ASM_EMIT(OP "ps       " SEL("%%xmm5", "%%xmm1") ", " SEL("%%xmm1", "%%xmm5")) \
    __ASM_EMIT("movups      " SEL("%%xmm0", "%%xmm4") ", 0x00(%[" DST "], %[off])") \
    __ASM_EMIT("movups      " SEL("%%xmm1", "%%xmm5") ", 0x10(%[" DST "], %[off])") \
    __ASM_EMIT("add         $0x20, %[off]") \
    __ASM_EMIT("sub         $8, %[count]") \
    __ASM_EMIT("jae         1b") \
    \
    /* 4x block */ \
    __ASM_EMIT("2:") \
    __ASM_EMIT("add         $4, %[count]")       /* 8 - 4 */ \
    __ASM_EMIT("jl          4f") \
    __ASM_EMIT("movups      0x00(%[" A "], %[off]), %%xmm0") \
    __ASM_EMIT("movups      0x00(%[" B "], %[off]), %%xmm2") \
    __ASM_EMIT("movups      0x00(%[" C "], %[off]), %%xmm4") \
    __ASM_EMIT("mulps       %%xmm2, %%xmm4") \
    __ASM_EMIT(OP "ps       " SEL("%%xmm4", "%%xmm0") ", " SEL("%%xmm0", "%%xmm4")) \
    __ASM_EMIT("movups      " SEL("%%xmm0", "%%xmm4") ", 0x00(%[" DST "], %[off])") \
    __ASM_EMIT("add         $0x10, %[off]") \
    __ASM_EMIT("sub         $4, %[count]") \
    /* 1x blocks */ \
    __ASM_EMIT("4:") \
    __ASM_EMIT("add         $3, %[count]")          /* 4 - 1 */ \
    __ASM_EMIT("jl          6f")    \
    __ASM_EMIT("5:") \
    __ASM_EMIT("movss       0x00(%[" A "], %[off]), %%xmm0") \
    __ASM_EMIT("movss       0x00(%[" B "], %[off]), %%xmm2") \
    __ASM_EMIT("movss       0x00(%[" C "], %[off]), %%xmm4") \
    __ASM_EMIT("mulss       %%xmm2, %%xmm4") \
    __ASM_EMIT(OP "ss       " SEL("%%xmm4", "%%xmm0") ", " SEL("%%xmm0", "%%xmm4")) \
    __ASM_EMIT("movss       " SEL("%%xmm0", "%%xmm4") ", 0x00(%[" DST "], %[off])") \
    __ASM_EMIT("add         $0x04, %[off]") \
    __ASM_EMIT("dec         %[count]") \
    __ASM_EMIT("jge         5b") \
    __ASM_EMIT("6:")

void fmadd3(float *dst, const float *a, const float *b, size_t count)
{
    IF_ARCH_X86(size_t off);
    ARCH_X86_ASM
    (
        FMOP_VV_CORE("dst", "dst", "a", "b", "add", OP_DSEL)
        : [off] "=&r" (off), [count] "+r" (count)
        : [dst] "r" (dst), [a] "r" (a), [b] "r" (b)
        : "cc", "memory",
          "%xmm0", "%xmm1", "%xmm2", "%xmm3",
          "%xmm4", "%xmm5"
    );
}

void fmsub3(float *dst, const float *a, const float *b, size_t count)
{
    IF_ARCH_X86(size_t off);
    ARCH_X86_ASM
    (
        FMOP_VV_CORE("dst", "dst", "a", "b", "sub", OP_DSEL)
        : [off] "=&r" (off), [count] "+r" (count)
        : [dst] "r" (dst), [a] "r" (a), [b] "r" (b)
        : "cc", "memory",
          "%xmm0", "%xmm1", "%xmm2", "%xmm3",
          "%xmm4", "%xmm5"
    );
}

void fmrsub3(float *dst, const float *a, const float *b, size_t count)
{
    IF_ARCH_X86(size_t off);
    ARCH_X86_ASM
    (
        FMOP_VV_CORE("dst", "dst", "a", "b", "sub", OP_RSEL)
        : [off] "=&r" (off), [count] "+r" (count)
        : [dst] "r" (dst), [a] "r" (a), [b] "r" (b)
        : "cc", "memory",
          "%xmm0", "%xmm1", "%xmm2", "%xmm3",
          "%xmm4", "%xmm5"
    );
}

void fmmul3(float *dst, const float *a, const float *b, size_t count)
{
    IF_ARCH_X86(size_t off);
    ARCH_X86_ASM
    (
        FMOP_VV_CORE("dst", "dst", "a", "b", "mul", OP_DSEL)
        : [off] "=&r" (off), [count] "+r" (count)
        : [dst] "r" (dst), [a] "r" (a), [b] "r" (b)
        : "cc", "memory",
          "%xmm0", "%xmm1", "%xmm2", "%xmm3",
          "%xmm4", "%xmm5"
    );
}

void fmdiv3(float *dst, const float *a, const float *b, size_t count)
{
    IF_ARCH_X86(size_t off);
    ARCH_X86_ASM
    (
        FMOP_VV_CORE("dst", "dst", "a", "b", "div", OP_DSEL)
        : [off] "=&r" (off), [count] "+r" (count)
        : [dst] "r" (dst), [a] "r" (a), [b] "r" (b)
        : "cc", "memory",
          "%xmm0", "%xmm1", "%xmm2", "%xmm3",
          "%xmm4", "%xmm5"
    );
}

void fmrdiv3(float *dst, const float *a, const float *b, size_t count)
{
    IF_ARCH_X86(size_t off);
    ARCH_X86_ASM
    (
        FMOP_VV_CORE("dst", "dst", "a", "b", "div", OP_RSEL)
        : [off] "=&r" (off), [count] "+r" (count)
        : [dst] "r" (dst), [a] "r" (a), [b] "r" (b)
        : "cc", "memory",
          "%xmm0", "%xmm1", "%xmm2", "%xmm3",
          "%xmm4", "%xmm5"
    );
}

void fmadd4(float *dst, const float *a, const float *b, const float *c, size_t count)
{
    IF_ARCH_X86(size_t off);
    ARCH_X86_ASM
    (
        FMOP_VV_CORE("dst", "a", "b", "c", "add", OP_DSEL)
        : [off] "=&r" (off), [count] "+r" (count)
        : [dst] "r" (dst), [a] "r" (a), [b] "r" (b), [c] "r" (c)
        : "cc", "memory",
          "%xmm0", "%xmm1", "%xmm2", "%xmm3",
          "%xmm4", "%xmm5"
    );
}

void fmsub4(float *dst, const float *a, const float *b, const float *c, size_t count)
{
    IF_ARCH_X86(size_t off);
    ARCH_X86_ASM
    (
        FMOP_VV_CORE("dst", "a", "b", "c", "sub", OP_DSEL)
        : [off] "=&r" (off), [count] "+r" (count)
        : [dst] "r" (dst), [a] "r" (a), [b] "r" (b), [c] "r" (c)
        : "cc", "memory",
          "%xmm0", "%xmm1", "%xmm2", "%xmm3",
          "%xmm4", "%xmm5"
    );
}

void fmrsub4(float *dst, const float *a, const float *b, const float *c, size_t count)
{
    IF_ARCH_X86(size_t off);
    ARCH_X86_ASM
    (
        FMOP_VV_CORE("dst", "a", "b", "c", "sub", OP_RSEL)
        : [off] "=&r" (off), [count] "+r" (count)
        : [dst] "r" (dst), [a] "r" (a), [b] "r" (b), [c] "r" (c)
        : "cc", "memory",
          "%xmm0", "%xmm1", "%xmm2", "%xmm3",
          "%xmm4", "%xmm5"
    );
}

void fmmul4(float *dst, const float *a, const float *b, const float *c, size_t count)
{
    IF_ARCH_X86(size_t off);
    ARCH_X86_ASM
    (
        FMOP_VV_CORE("dst", "a", "b", "c", "mul", OP_DSEL)
        : [off] "=&r" (off), [count] "+r" (count)
        : [dst] "r" (dst), [a] "r" (a), [b] "r" (b), [c] "r" (c)
        : "cc", "memory",
          "%xmm0", "%xmm1", "%xmm2", "%xmm3",
          "%xmm4", "%xmm5"
    );
}

void fmdiv4(float *dst, const float *a, const float *b, const float *c, size_t count)
{
    IF_ARCH_X86(size_t off);
    ARCH_X86_ASM
    (
        FMOP_VV_CORE("dst", "a", "b", "c", "div", OP_DSEL)
        : [off] "=&r" (off), [count] "+r" (count)
        : [dst] "r" (dst), [a] "r" (a), [b] "r" (b), [c] "r" (c)
        : "cc", "memory",
          "%xmm0", "%xmm1", "%xmm2", "%xmm3",
          "%xmm4", "%xmm5"
    );
}

void fmrdiv4(float *dst, const float *a, const float *b, const float *c, size_t count)
{
    IF_ARCH_X86(size_t off);
    ARCH_X86_ASM
    (
        FMOP_VV_CORE("dst", "a", "b", "c", "div", OP_RSEL)
        : [off] "=&r" (off), [count] "+r" (count)
        : [dst] "r" (dst), [a] "r" (a), [b] "r" (b), [c] "r" (c)
        : "cc", "memory",
          "%xmm0", "%xmm1", "%xmm2", "%xmm3",
          "%xmm4", "%xmm5"
    );
}

#undef FMOP_K4_CORE

#undef OP_DSEL
#undef OP_RSEL
}

#endif /* DSP_ARCH_X86_SSE_PMATH_FMOP_VV_H_ */
