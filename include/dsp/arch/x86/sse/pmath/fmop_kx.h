/*
 * fmop_kx.h
 *
 *  Created on: 20 нояб. 2019 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_SSE_PMATH_FMOP_KX_H_
#define DSP_ARCH_X86_SSE_PMATH_FMOP_KX_H_

#ifndef DSP_ARCH_X86_SSE_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_SSE_IMPL */

namespace sse
{
    #define OP_DSEL(a, b)   a
    #define OP_RSEL(a, b)   b

    #define FMOP_K4_CORE(DST, SRC1, SRC2, OP, SEL) \
        __ASM_EMIT("xor         %[off], %[off]") \
        __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0") \
        __ASM_EMIT("sub         $12, %[count]") \
        __ASM_EMIT("movaps      %%xmm0, %%xmm1") \
        __ASM_EMIT("jb          2f")    \
        /* 12x blocks */ \
        __ASM_EMIT("1:") \
        __ASM_EMIT("movups      0x00(%[" SRC2 "], %[off]), %%xmm5") \
        __ASM_EMIT("movups      0x10(%[" SRC2 "], %[off]), %%xmm6") \
        __ASM_EMIT("movups      0x20(%[" SRC2 "], %[off]), %%xmm7") \
        __ASM_EMIT("movups      0x00(%[" SRC1 "], %[off]), %%xmm2") \
        __ASM_EMIT("movups      0x10(%[" SRC1 "], %[off]), %%xmm3") \
        __ASM_EMIT("movups      0x20(%[" SRC1 "], %[off]), %%xmm4") \
        __ASM_EMIT("mulps       %%xmm0, %%xmm5") \
        __ASM_EMIT("mulps       %%xmm1, %%xmm6") \
        __ASM_EMIT("mulps       %%xmm0, %%xmm7") \
        __ASM_EMIT(OP "ps       " SEL("%%xmm5", "%%xmm2") ", " SEL("%%xmm2", "%%xmm5")) \
        __ASM_EMIT(OP "ps       " SEL("%%xmm6", "%%xmm3") ", " SEL("%%xmm3", "%%xmm6")) \
        __ASM_EMIT(OP "ps       " SEL("%%xmm7", "%%xmm4") ", " SEL("%%xmm4", "%%xmm7")) \
        __ASM_EMIT("movups      " SEL("%%xmm2", "%%xmm5") ", 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("movups      " SEL("%%xmm3", "%%xmm6") ", 0x10(%[" DST "], %[off])") \
        __ASM_EMIT("movups      " SEL("%%xmm4", "%%xmm7") ", 0x20(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x30, %[off]") \
        __ASM_EMIT("sub         $12, %[count]") \
        __ASM_EMIT("jae         1b") \
        /* 8x block */ \
        __ASM_EMIT("2:") \
        __ASM_EMIT("add         $4, %[count]")          /* 12 - 8 */ \
        __ASM_EMIT("jl          4f") \
        __ASM_EMIT("movups      0x00(%[" SRC2 "], %[off]), %%xmm5") \
        __ASM_EMIT("movups      0x10(%[" SRC2 "], %[off]), %%xmm6") \
        __ASM_EMIT("movups      0x00(%[" SRC1 "], %[off]), %%xmm2") \
        __ASM_EMIT("movups      0x10(%[" SRC1 "], %[off]), %%xmm3") \
        __ASM_EMIT("mulps       %%xmm0, %%xmm5") \
        __ASM_EMIT("mulps       %%xmm1, %%xmm6") \
        __ASM_EMIT(OP "ps       " SEL("%%xmm5", "%%xmm2") ", " SEL("%%xmm2", "%%xmm5")) \
        __ASM_EMIT(OP "ps       " SEL("%%xmm6", "%%xmm3") ", " SEL("%%xmm3", "%%xmm6")) \
        __ASM_EMIT("movups      " SEL("%%xmm2", "%%xmm5") ", 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("movups      " SEL("%%xmm3", "%%xmm6") ", 0x10(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x20, %[off]") \
        __ASM_EMIT("sub         $8, %[count]") \
        \
        /* 4x block */ \
        __ASM_EMIT("4:") \
        __ASM_EMIT("add         $4, %[count]")       /* 8 - 4 */ \
        __ASM_EMIT("jl          6f") \
        __ASM_EMIT("movups      0x00(%[" SRC2 "], %[off]), %%xmm5") \
        __ASM_EMIT("movups      0x00(%[" SRC1 "], %[off]), %%xmm2") \
        __ASM_EMIT("mulps       %%xmm0, %%xmm5") \
        __ASM_EMIT(OP "ps       " SEL("%%xmm5", "%%xmm2") ", " SEL("%%xmm2", "%%xmm5")) \
        __ASM_EMIT("movups      " SEL("%%xmm2", "%%xmm5") ", 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x10, %[off]") \
        __ASM_EMIT("sub         $4, %[count]") \
        /* 1x blocks */ \
        __ASM_EMIT("6:") \
        __ASM_EMIT("add         $3, %[count]")          /* 4 - 1 */ \
        __ASM_EMIT("jl          8f")    \
        __ASM_EMIT("7:") \
        __ASM_EMIT("movss       0x00(%[" SRC2 "], %[off]), %%xmm5") \
        __ASM_EMIT("movss       0x00(%[" SRC1 "], %[off]), %%xmm2") \
        __ASM_EMIT("mulss        %%xmm0, %%xmm5") \
        __ASM_EMIT(OP "ss       " SEL("%%xmm5", "%%xmm2") ", " SEL("%%xmm2", "%%xmm5")) \
        __ASM_EMIT("movss       " SEL("%%xmm2", "%%xmm5") ", 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x04, %[off]") \
        __ASM_EMIT("dec         %[count]") \
        __ASM_EMIT("jge         7b") \
        __ASM_EMIT("8:")

    void fmadd_k3(float *dst, const float *src, float k, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            FMOP_K4_CORE("dst", "dst", "src", "add", OP_DSEL)
            : [off] "=&r" (off), [count] "+r" (count),
              [k] "+Yz"(k)
            : [dst] "r"(dst), [src] "r"(src)
            : "cc", "memory",
              "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void fmsub_k3(float *dst, const float *src, float k, size_t count)
    {
        IF_ARCH_X86(size_t off);

        ARCH_X86_ASM
        (
            FMOP_K4_CORE("dst", "dst", "src", "sub", OP_DSEL)
            : [off] "=&r" (off), [count] "+r" (count),
              [k] "+Yz"(k)
            : [dst] "r"(dst), [src] "r"(src)
            : "cc", "memory",
              "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void fmrsub_k3(float *dst, const float *src, float k, size_t count)
    {
        IF_ARCH_X86(size_t off);

        ARCH_X86_ASM
        (
            FMOP_K4_CORE("dst", "dst", "src", "sub", OP_RSEL)
            : [off] "=&r" (off), [count] "+r" (count),
              [k] "+Yz"(k)
            : [dst] "r"(dst), [src] "r"(src)
            : "cc", "memory",
              "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void fmmul_k3(float *dst, const float *src, float k, size_t count)
    {
        IF_ARCH_X86(size_t off);

        ARCH_X86_ASM
        (
            FMOP_K4_CORE("dst", "dst", "src", "mul", OP_DSEL)
            : [off] "=&r" (off), [count] "+r" (count),
              [k] "+Yz"(k)
            : [dst] "r"(dst), [src] "r"(src)
            : "cc", "memory",
              "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void fmdiv_k3(float *dst, const float *src, float k, size_t count)
    {
        IF_ARCH_X86(size_t off);

        ARCH_X86_ASM
        (
            FMOP_K4_CORE("dst", "dst", "src", "div", OP_DSEL)
            : [off] "=&r" (off), [count] "+r" (count),
              [k] "+Yz"(k)
            : [dst] "r"(dst), [src] "r"(src)
            : "cc", "memory",
              "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void fmrdiv_k3(float *dst, const float *src, float k, size_t count)
    {
        IF_ARCH_X86(size_t off);

        ARCH_X86_ASM
        (
            FMOP_K4_CORE("dst", "dst", "src", "div", OP_RSEL)
            : [off] "=&r" (off), [count] "+r" (count),
              [k] "+Yz"(k)
            : [dst] "r"(dst), [src] "r"(src)
            : "cc", "memory",
              "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void fmadd_k4(float *dst, const float *src1, const float *src2, float k, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            FMOP_K4_CORE("dst", "src1", "src2", "add", OP_DSEL)
            : [count] "+r" (count), [off] "=&r" (off),
              [k] "+Yz"(k)
            : [dst] "r"(dst), [src1] "r"(src1), [src2] "r"(src2)
            : "cc", "memory",
              "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void fmsub_k4(float *dst, const float *src1, const float *src2, float k, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            FMOP_K4_CORE("dst", "src1", "src2", "sub", OP_DSEL)
            : [count] "+r" (count), [off] "=&r" (off),
              [k] "+Yz"(k)
            : [dst] "r"(dst), [src1] "r"(src1), [src2] "r"(src2)
            : "cc", "memory",
              "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void fmrsub_k4(float *dst, const float *src1, const float *src2, float k, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            FMOP_K4_CORE("dst", "src1", "src2", "sub", OP_RSEL)
            : [count] "+r" (count), [off] "=&r" (off),
              [k] "+Yz"(k)
            : [dst] "r"(dst), [src1] "r"(src1), [src2] "r"(src2)
            : "cc", "memory",
              "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void fmmul_k4(float *dst, const float *src1, const float *src2, float k, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            FMOP_K4_CORE("dst", "src1", "src2", "mul", OP_DSEL)
            : [count] "+r" (count), [off] "=&r" (off),
              [k] "+Yz"(k)
            : [dst] "r"(dst), [src1] "r"(src1), [src2] "r"(src2)
            : "cc", "memory",
              "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void fmdiv_k4(float *dst, const float *src1, const float *src2, float k, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            FMOP_K4_CORE("dst", "src1", "src2", "div", OP_DSEL)
            : [count] "+r" (count), [off] "=&r" (off),
              [k] "+Yz"(k)
            : [dst] "r"(dst), [src1] "r"(src1), [src2] "r"(src2)
            : "cc", "memory",
              "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void fmrdiv_k4(float *dst, const float *src1, const float *src2, float k, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            FMOP_K4_CORE("dst", "src1", "src2", "div", OP_RSEL)
            : [count] "+r" (count), [off] "=&r" (off),
              [k] "+Yz"(k)
            : [dst] "r"(dst), [src1] "r"(src1), [src2] "r"(src2)
            : "cc", "memory",
              "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    #undef FMOP_K4_CORE

    #undef OP_DSEL
    #undef OP_RSEL
}

#endif /* DSP_ARCH_X86_SSE_PMATH_FMOP_KX_H_ */
