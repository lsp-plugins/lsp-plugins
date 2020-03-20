/*
 * fmop_vv.h
 *
 *  Created on: 22 нояб. 2019 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_AVX_PMATH_FMOP_VV_H_
#define DSP_ARCH_X86_AVX_PMATH_FMOP_VV_H_

#ifndef DSP_ARCH_X86_AVX_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_AVX_IMPL */

namespace avx
{
    #define OP_DSEL(a, b)   a
    #define OP_RSEL(a, b)   b

    #define FMADDSUB_VV_CORE(DST, A, B, C, OP) \
        __ASM_EMIT("xor         %[off], %[off]") \
        __ASM_EMIT("sub         $32, %[count]") \
        __ASM_EMIT("jb          2f")    \
        /* 32x blocks */ \
        __ASM_EMIT("1:") \
        __ASM_EMIT("vmovups     0x000(%[" A "], %[off]), %%ymm0") \
        __ASM_EMIT("vmovups     0x020(%[" A "], %[off]), %%ymm1") \
        __ASM_EMIT("vmovups     0x040(%[" A "], %[off]), %%ymm2") \
        __ASM_EMIT("vmovups     0x060(%[" A "], %[off]), %%ymm3") \
        __ASM_EMIT("vmovups     0x000(%[" B "], %[off]), %%ymm4") \
        __ASM_EMIT("vmovups     0x020(%[" B "], %[off]), %%ymm5") \
        __ASM_EMIT("vmovups     0x040(%[" B "], %[off]), %%ymm6") \
        __ASM_EMIT("vmovups     0x060(%[" B "], %[off]), %%ymm7") \
        __ASM_EMIT(OP "ps       0x000(%[" C "], %[off]), %%ymm4, %%ymm0") \
        __ASM_EMIT(OP "ps       0x020(%[" C "], %[off]), %%ymm5, %%ymm1") \
        __ASM_EMIT(OP "ps       0x040(%[" C "], %[off]), %%ymm6, %%ymm2") \
        __ASM_EMIT(OP "ps       0x060(%[" C "], %[off]), %%ymm7, %%ymm3") \
        __ASM_EMIT("vmovups     %%ymm0, 0x000(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%ymm1, 0x020(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%ymm2, 0x040(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%ymm3, 0x060(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x80, %[off]") \
        __ASM_EMIT("sub         $32, %[count]") \
        __ASM_EMIT("jae         1b") \
        /* 16x block */ \
        __ASM_EMIT("2:") \
        __ASM_EMIT("add         $16, %[count]")          /* 32 - 16 */ \
        __ASM_EMIT("jl          4f") \
        __ASM_EMIT("vmovups     0x000(%[" A "], %[off]), %%ymm0") \
        __ASM_EMIT("vmovups     0x020(%[" A "], %[off]), %%ymm1") \
        __ASM_EMIT("vmovups     0x000(%[" B "], %[off]), %%ymm4") \
        __ASM_EMIT("vmovups     0x020(%[" B "], %[off]), %%ymm5") \
        __ASM_EMIT(OP "ps       0x000(%[" C "], %[off]), %%ymm4, %%ymm0") \
        __ASM_EMIT(OP "ps       0x020(%[" C "], %[off]), %%ymm5, %%ymm1") \
        __ASM_EMIT("vmovups     %%ymm0, 0x000(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%ymm1, 0x020(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x40, %[off]") \
        __ASM_EMIT("sub         $16, %[count]") \
        /* 8x block */ \
        __ASM_EMIT("4:") \
        __ASM_EMIT("add         $8, %[count]")          /* 16 - 8 */ \
        __ASM_EMIT("jl          6f") \
        __ASM_EMIT("vmovups     0x000(%[" A "], %[off]), %%ymm0") \
        __ASM_EMIT("vmovups     0x000(%[" B "], %[off]), %%ymm4") \
        __ASM_EMIT(OP "ps       0x000(%[" C "], %[off]), %%ymm4, %%ymm0") \
        __ASM_EMIT("vmovups     %%ymm0, 0x000(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x20, %[off]") \
        __ASM_EMIT("sub         $8, %[count]") \
        \
        /* 4x block */ \
        __ASM_EMIT("6:") \
        __ASM_EMIT("add         $4, %[count]")          /* 8 - 4 */ \
        __ASM_EMIT("jl          8f") \
        __ASM_EMIT("vmovups     0x000(%[" A "], %[off]), %%xmm0") \
        __ASM_EMIT("vmovups     0x000(%[" B "], %[off]), %%xmm4") \
        __ASM_EMIT(OP "ps       0x000(%[" C "], %[off]), %%xmm4, %%xmm0") \
        __ASM_EMIT("vmovups     %%xmm0, 0x000(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x10, %[off]") \
        __ASM_EMIT("sub         $4, %[count]") \
        \
        /* 1x blocks */ \
        __ASM_EMIT("8:") \
        __ASM_EMIT("add         $3, %[count]")          /* 8 - 4 */ \
        __ASM_EMIT("jl          10f") \
        __ASM_EMIT("9:") \
        __ASM_EMIT("vmovss      0x000(%[" A "], %[off]), %%xmm0") \
        __ASM_EMIT("vmovss      0x000(%[" B "], %[off]), %%xmm4") \
        __ASM_EMIT(OP "ss       0x000(%[" C "], %[off]), %%xmm4, %%xmm0") \
        __ASM_EMIT("vmovss      %%xmm0, 0x000(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x04, %[off]") \
        __ASM_EMIT("dec         %[count]") \
        __ASM_EMIT("jge         9b") \
        __ASM_EMIT("10:")

    void fmadd3_fma3(float *dst, const float *a, const float *b, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            FMADDSUB_VV_CORE("dst", "dst", "a", "b", "vfmadd231")
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r" (dst), [a] "r" (a), [b] "r" (b)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void fmsub3_fma3(float *dst, const float *a, const float *b, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            FMADDSUB_VV_CORE("dst", "dst", "a", "b", "vfnmadd231")
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r" (dst), [a] "r" (a), [b] "r" (b)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void fmrsub3_fma3(float *dst, const float *a, const float *b, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            FMADDSUB_VV_CORE("dst", "dst", "a", "b", "vfmsub231")
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r" (dst), [a] "r" (a), [b] "r" (b)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void fmadd4_fma3(float *dst, const float *a, const float *b, const float *c, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            FMADDSUB_VV_CORE("dst", "a", "b", "c", "vfmadd231")
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r" (dst), [a] "r" (a), [b] "r" (b), [c] "r" (c)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void fmsub4_fma3(float *dst, const float *a, const float *b, const float *c, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            FMADDSUB_VV_CORE("dst", "a", "b", "c", "vfnmadd231")
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r" (dst), [a] "r" (a), [b] "r" (b), [c] "r" (c)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void fmrsub4_fma3(float *dst, const float *a, const float *b, const float *c, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            FMADDSUB_VV_CORE("dst", "a", "b", "c", "vfmsub231")
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r" (dst), [a] "r" (a), [b] "r" (b), [c] "r" (c)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    #undef FMADDSUB_VV_CORE

    #define FMOP_VV_CORE(DST, A, B, C, OP, SEL) \
        __ASM_EMIT("xor         %[off], %[off]") \
        __ASM_EMIT("sub         $32, %[count]") \
        __ASM_EMIT("jb          2f")    \
        /* 32x blocks */ \
        __ASM_EMIT("1:") \
        __ASM_EMIT("vmovups     0x000(%[" B "], %[off]), %%ymm4") \
        __ASM_EMIT("vmovups     0x020(%[" B "], %[off]), %%ymm5") \
        __ASM_EMIT("vmovups     0x040(%[" B "], %[off]), %%ymm6") \
        __ASM_EMIT("vmovups     0x060(%[" B "], %[off]), %%ymm7") \
        __ASM_EMIT("vmovups     0x000(%[" A "], %[off]), %%ymm0") \
        __ASM_EMIT("vmovups     0x020(%[" A "], %[off]), %%ymm1") \
        __ASM_EMIT("vmovups     0x040(%[" A "], %[off]), %%ymm2") \
        __ASM_EMIT("vmovups     0x060(%[" A "], %[off]), %%ymm3") \
        __ASM_EMIT("vmulps      0x000(%[" C "], %[off]), %%ymm4, %%ymm4") \
        __ASM_EMIT("vmulps      0x020(%[" C "], %[off]), %%ymm5, %%ymm5") \
        __ASM_EMIT("vmulps      0x040(%[" C "], %[off]), %%ymm6, %%ymm6") \
        __ASM_EMIT("vmulps      0x060(%[" C "], %[off]), %%ymm7, %%ymm7") \
        __ASM_EMIT(OP "ps       " SEL("%%ymm4", "%%ymm0") ", "  SEL("%%ymm0", "%%ymm4") ", %%ymm0") \
        __ASM_EMIT(OP "ps       " SEL("%%ymm5", "%%ymm1") ", "  SEL("%%ymm1", "%%ymm5") ", %%ymm1") \
        __ASM_EMIT(OP "ps       " SEL("%%ymm6", "%%ymm2") ", "  SEL("%%ymm2", "%%ymm6") ", %%ymm2") \
        __ASM_EMIT(OP "ps       " SEL("%%ymm7", "%%ymm3") ", "  SEL("%%ymm3", "%%ymm7") ", %%ymm3") \
        __ASM_EMIT("vmovups     %%ymm0, 0x000(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%ymm1, 0x020(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%ymm2, 0x040(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%ymm3, 0x060(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x80, %[off]") \
        __ASM_EMIT("sub         $32, %[count]") \
        __ASM_EMIT("jae         1b") \
        /* 16x block */ \
        __ASM_EMIT("2:") \
        __ASM_EMIT("add         $16, %[count]")          /* 32 - 16 */ \
        __ASM_EMIT("jl          4f") \
        __ASM_EMIT("vmovups     0x000(%[" B "], %[off]), %%ymm4") \
        __ASM_EMIT("vmovups     0x020(%[" B "], %[off]), %%ymm5") \
        __ASM_EMIT("vmovups     0x000(%[" A "], %[off]), %%ymm0") \
        __ASM_EMIT("vmovups     0x020(%[" A "], %[off]), %%ymm1") \
        __ASM_EMIT("vmulps      0x000(%[" C "], %[off]), %%ymm4, %%ymm4") \
        __ASM_EMIT("vmulps      0x020(%[" C "], %[off]), %%ymm5, %%ymm5") \
        __ASM_EMIT(OP "ps       " SEL("%%ymm4", "%%ymm0") ", "  SEL("%%ymm0", "%%ymm4") ", %%ymm0") \
        __ASM_EMIT(OP "ps       " SEL("%%ymm5", "%%ymm1") ", "  SEL("%%ymm1", "%%ymm5") ", %%ymm1") \
        __ASM_EMIT("vmovups     %%ymm0, 0x000(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%ymm1, 0x020(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x40, %[off]") \
        __ASM_EMIT("sub         $16, %[count]") \
        /* 8x block */ \
        __ASM_EMIT("4:") \
        __ASM_EMIT("add         $8, %[count]")          /* 16 - 8 */ \
        __ASM_EMIT("jl          6f") \
        __ASM_EMIT("vmovups     0x000(%[" B "], %[off]), %%ymm4") \
        __ASM_EMIT("vmovups     0x000(%[" A "], %[off]), %%ymm0") \
        __ASM_EMIT("vmulps      0x000(%[" C "], %[off]), %%ymm4, %%ymm4") \
        __ASM_EMIT(OP "ps       " SEL("%%ymm4", "%%ymm0") ", "  SEL("%%ymm0", "%%ymm4") ", %%ymm0") \
        __ASM_EMIT("vmovups     %%ymm0, 0x000(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x20, %[off]") \
        __ASM_EMIT("sub         $8, %[count]") \
        \
        /* 4x block */ \
        __ASM_EMIT("6:") \
        __ASM_EMIT("add         $4, %[count]")          /* 8 - 4 */ \
        __ASM_EMIT("jl          8f") \
        __ASM_EMIT("vmovups     0x000(%[" B "], %[off]), %%xmm4") \
        __ASM_EMIT("vmovups     0x000(%[" A "], %[off]), %%xmm0") \
        __ASM_EMIT("vmulps      0x000(%[" C "], %[off]), %%xmm4, %%xmm4") \
        __ASM_EMIT(OP "ps       " SEL("%%xmm4", "%%xmm0") ", "  SEL("%%xmm0", "%%xmm4") ", %%xmm0") \
        __ASM_EMIT("vmovups     %%xmm0, 0x000(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x10, %[off]") \
        __ASM_EMIT("sub         $4, %[count]") \
        \
        /* 1x blocks */ \
        __ASM_EMIT("8:") \
        __ASM_EMIT("add         $3, %[count]")          /* 8 - 4 */ \
        __ASM_EMIT("jl          10f") \
        __ASM_EMIT("9:") \
        __ASM_EMIT("vmovss      0x000(%[" B "], %[off]), %%xmm4") \
        __ASM_EMIT("vmovss      0x000(%[" A "], %[off]), %%xmm0") \
        __ASM_EMIT("vmulss      0x000(%[" C "], %[off]), %%xmm4, %%xmm4") \
        __ASM_EMIT(OP "ss       " SEL("%%xmm4", "%%xmm0") ", "  SEL("%%xmm0", "%%xmm4") ", %%xmm0") \
        __ASM_EMIT("vmovss      %%xmm0, 0x000(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x04, %[off]") \
        __ASM_EMIT("dec         %[count]") \
        __ASM_EMIT("jge         9b") \
        __ASM_EMIT("10:")

    void fmadd3(float *dst, const float *a, const float *b, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            FMOP_VV_CORE("dst", "dst", "a", "b", "vadd", OP_DSEL)
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r"(dst), [a] "r" (a), [b] "r" (b)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void fmsub3(float *dst, const float *a, const float *b, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            FMOP_VV_CORE("dst", "dst", "a", "b", "vsub", OP_DSEL)
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r"(dst), [a] "r" (a), [b] "r" (b)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void fmrsub3(float *dst, const float *a, const float *b, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            FMOP_VV_CORE("dst", "dst", "a", "b", "vsub", OP_RSEL)
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r"(dst), [a] "r" (a), [b] "r" (b)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void fmmul3(float *dst, const float *a, const float *b, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            FMOP_VV_CORE("dst", "dst", "a", "b", "vmul", OP_DSEL)
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r"(dst), [a] "r" (a), [b] "r" (b)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void fmdiv3(float *dst, const float *a, const float *b, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            FMOP_VV_CORE("dst", "dst", "a", "b", "vdiv", OP_DSEL)
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r"(dst), [a] "r" (a), [b] "r" (b)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void fmrdiv3(float *dst, const float *a, const float *b, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            FMOP_VV_CORE("dst", "dst", "a", "b", "vdiv", OP_RSEL)
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r"(dst), [a] "r" (a), [b] "r" (b)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void fmadd4(float *dst, const float *a, const float *b, const float *c, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            FMOP_VV_CORE("dst", "a", "b", "c", "vadd", OP_DSEL)
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r"(dst), [a] "r" (a), [b] "r" (b), [c] "r" (c)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void fmsub4(float *dst, const float *a, const float *b, const float *c, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            FMOP_VV_CORE("dst", "a", "b", "c", "vsub", OP_DSEL)
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r"(dst), [a] "r" (a), [b] "r" (b), [c] "r" (c)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void fmrsub4(float *dst, const float *a, const float *b, const float *c, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            FMOP_VV_CORE("dst", "a", "b", "c", "vsub", OP_RSEL)
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r"(dst), [a] "r" (a), [b] "r" (b), [c] "r" (c)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void fmmul4(float *dst, const float *a, const float *b, const float *c, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            FMOP_VV_CORE("dst", "a", "b", "c", "vmul", OP_DSEL)
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r"(dst), [a] "r" (a), [b] "r" (b), [c] "r" (c)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void fmdiv4(float *dst, const float *a, const float *b, const float *c, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            FMOP_VV_CORE("dst", "a", "b", "c", "vdiv", OP_DSEL)
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r"(dst), [a] "r" (a), [b] "r" (b), [c] "r" (c)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void fmrdiv4(float *dst, const float *a, const float *b, const float *c, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            FMOP_VV_CORE("dst", "a", "b", "c", "vdiv", OP_RSEL)
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r"(dst), [a] "r" (a), [b] "r" (b), [c] "r" (c)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    #undef FMOP_VV_CORE

    #define FMA_OFF(a, b)   a
    #define FMA_ON(a, b)    b

    #define FMOD_VV_CORE(DST, SRC1, SRC2, SRC3, OP_SEL, FMA_SEL) \
        __ASM_EMIT("xor                 %[off], %[off]") \
        /* x16 blocks */ \
        __ASM_EMIT32("subl              $16, %[count]") \
        __ASM_EMIT64("sub               $16, %[count]") \
        __ASM_EMIT("jb                  2f") \
        __ASM_EMIT("1:") \
        __ASM_EMIT("vmovups             0x00(%[" SRC3 "], %[off]), %%ymm0") \
        __ASM_EMIT("vmovups             0x20(%[" SRC3 "], %[off]), %%ymm1") \
        __ASM_EMIT("vmovups             0x00(%[" SRC1 "], %[off]), " OP_SEL("%%ymm2", "%%ymm6")) \
        __ASM_EMIT("vmovups             0x20(%[" SRC1 "], %[off]), " OP_SEL("%%ymm3", "%%ymm7")) \
        __ASM_EMIT("vmulps              0x00(%[" SRC2 "], %[off]), %%ymm0, " OP_SEL("%%ymm6", "%%ymm2")) \
        __ASM_EMIT("vmulps              0x20(%[" SRC2 "], %[off]), %%ymm1, " OP_SEL("%%ymm7", "%%ymm3")) \
        __ASM_EMIT("vdivps              %%ymm6, %%ymm2, %%ymm4") \
        __ASM_EMIT("vdivps              %%ymm7, %%ymm3, %%ymm5") \
        __ASM_EMIT("vcvttps2dq          %%ymm4, %%ymm4") \
        __ASM_EMIT("vcvttps2dq          %%ymm5, %%ymm5") \
        __ASM_EMIT("vcvtdq2ps           %%ymm4, %%ymm4") \
        __ASM_EMIT("vcvtdq2ps           %%ymm5, %%ymm5") \
        __ASM_EMIT(FMA_SEL("vmulps      %%ymm6, %%ymm4, %%ymm4", "")) \
        __ASM_EMIT(FMA_SEL("vmulps      %%ymm7, %%ymm5, %%ymm5", "")) \
        __ASM_EMIT(FMA_SEL("vsubps      %%ymm4, %%ymm2, %%ymm2", "vfnmadd231ps  %%ymm6, %%ymm4, %%ymm2")) \
        __ASM_EMIT(FMA_SEL("vsubps      %%ymm5, %%ymm3, %%ymm3", "vfnmadd231ps  %%ymm7, %%ymm5, %%ymm3")) \
        __ASM_EMIT("vmovups             %%ymm2, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups             %%ymm3, 0x20(%[" DST "], %[off])") \
        __ASM_EMIT("add                 $0x40, %[off]") \
        __ASM_EMIT32("subl              $16, %[count]") \
        __ASM_EMIT64("sub               $16, %[count]") \
        __ASM_EMIT("jae                 1b") \
        __ASM_EMIT("2:") \
        /* x8 block */ \
        __ASM_EMIT32("addl              $8, %[count]") \
        __ASM_EMIT64("add               $8, %[count]") \
        __ASM_EMIT("jl                  4f") \
        __ASM_EMIT("vmovups             0x00(%[" SRC3 "], %[off]), %%xmm0") \
        __ASM_EMIT("vmovups             0x10(%[" SRC3 "], %[off]), %%xmm1") \
        __ASM_EMIT("vmovups             0x00(%[" SRC1 "], %[off]), " OP_SEL("%%xmm2", "%%xmm6")) \
        __ASM_EMIT("vmovups             0x10(%[" SRC1 "], %[off]), " OP_SEL("%%xmm3", "%%xmm7")) \
        __ASM_EMIT("vmulps              0x00(%[" SRC2 "], %[off]), %%xmm0, " OP_SEL("%%xmm6", "%%xmm2")) \
        __ASM_EMIT("vmulps              0x10(%[" SRC2 "], %[off]), %%xmm1, " OP_SEL("%%xmm7", "%%xmm3")) \
        __ASM_EMIT("vdivps              %%xmm6, %%xmm2, %%xmm4") \
        __ASM_EMIT("vdivps              %%xmm7, %%xmm3, %%xmm5") \
        __ASM_EMIT("vcvttps2dq          %%xmm4, %%xmm4") \
        __ASM_EMIT("vcvttps2dq          %%xmm5, %%xmm5") \
        __ASM_EMIT("vcvtdq2ps           %%xmm4, %%xmm4") \
        __ASM_EMIT("vcvtdq2ps           %%xmm5, %%xmm5") \
        __ASM_EMIT(FMA_SEL("vmulps      %%xmm6, %%xmm4, %%xmm4", "")) \
        __ASM_EMIT(FMA_SEL("vmulps      %%xmm7, %%xmm5, %%xmm5", "")) \
        __ASM_EMIT(FMA_SEL("vsubps      %%xmm4, %%xmm2, %%xmm2", "vfnmadd231ps  %%xmm6, %%xmm4, %%xmm2")) \
        __ASM_EMIT(FMA_SEL("vsubps      %%xmm5, %%xmm3, %%xmm3", "vfnmadd231ps  %%xmm7, %%xmm5, %%xmm3")) \
        __ASM_EMIT("vmovups             %%xmm2, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups             %%xmm3, 0x10(%[" DST "], %[off])") \
        __ASM_EMIT32("subl              $8, %[count]") \
        __ASM_EMIT64("sub               $8, %[count]") \
        __ASM_EMIT("add                 $0x20, %[off]") \
        __ASM_EMIT("4:") \
        /* x4 block */ \
        __ASM_EMIT32("addl              $4, %[count]") \
        __ASM_EMIT64("add               $4, %[count]") \
        __ASM_EMIT("jl                  6f") \
        __ASM_EMIT("vmovups             0x00(%[" SRC3 "], %[off]), %%xmm0") \
        __ASM_EMIT("vmovups             0x00(%[" SRC1 "], %[off]), " OP_SEL("%%xmm2", "%%xmm6")) \
        __ASM_EMIT("vmulps              0x00(%[" SRC2 "], %[off]), %%xmm0, " OP_SEL("%%xmm6", "%%xmm2")) \
        __ASM_EMIT("vdivps              %%xmm6, %%xmm2, %%xmm4") \
        __ASM_EMIT("vcvttps2dq          %%xmm4, %%xmm4") \
        __ASM_EMIT("vcvtdq2ps           %%xmm4, %%xmm4") \
        __ASM_EMIT(FMA_SEL("vmulps      %%xmm6, %%xmm4, %%xmm4", "")) \
        __ASM_EMIT(FMA_SEL("vsubps      %%xmm4, %%xmm2, %%xmm2", "vfnmadd231ps  %%xmm6, %%xmm4, %%xmm2")) \
        __ASM_EMIT("vmovups             %%xmm2, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT32("subl              $4, %[count]") \
        __ASM_EMIT64("sub               $4, %[count]") \
        __ASM_EMIT("add                 $0x10, %[off]") \
        __ASM_EMIT("6:") \
        /* x1 blocks */ \
        __ASM_EMIT32("addl              $3, %[count]") \
        __ASM_EMIT64("add               $3, %[count]") \
        __ASM_EMIT("jl                  8f") \
        __ASM_EMIT("7:") \
        __ASM_EMIT("vmovss              0x00(%[" SRC3 "], %[off]), %%xmm0") \
        __ASM_EMIT("vmovss              0x00(%[" SRC1 "], %[off]), " OP_SEL("%%xmm2", "%%xmm6")) \
        __ASM_EMIT("vmulss              0x00(%[" SRC2 "], %[off]), %%xmm0, " OP_SEL("%%xmm6", "%%xmm2")) \
        __ASM_EMIT("vdivss              %%xmm6, %%xmm2, %%xmm4") \
        __ASM_EMIT("vcvttps2dq          %%xmm4, %%xmm4") \
        __ASM_EMIT("vcvtdq2ps           %%xmm4, %%xmm4") \
        __ASM_EMIT(FMA_SEL("vmulss      %%xmm6, %%xmm4, %%xmm4", "")) \
        __ASM_EMIT(FMA_SEL("vsubss      %%xmm4, %%xmm2, %%xmm2", "vfnmadd231ss  %%xmm6, %%xmm4, %%xmm2")) \
        __ASM_EMIT("vmovss              %%xmm2, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("add                 $0x04, %[off]") \
        __ASM_EMIT32("decl              %[count]") \
        __ASM_EMIT64("dec               %[count]") \
        __ASM_EMIT("jge                 7b") \
        __ASM_EMIT("8:")

    void fmmod3(float *dst, const float *a, const float *b, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            FMOD_VV_CORE("dst", "dst", "a", "b", OP_DSEL, FMA_OFF)
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r"(dst), [a] "r" (a), [b] "r" (b)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void fmmod3_fma3(float *dst, const float *a, const float *b, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            FMOD_VV_CORE("dst", "dst", "a", "b", OP_DSEL, FMA_OFF)
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r"(dst), [a] "r" (a), [b] "r" (b)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void fmrmod3(float *dst, const float *a, const float *b, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            FMOD_VV_CORE("dst", "dst", "a", "b", OP_RSEL, FMA_OFF)
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r"(dst), [a] "r" (a), [b] "r" (b)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void fmrmod3_fma3(float *dst, const float *a, const float *b, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            FMOD_VV_CORE("dst", "dst", "a", "b", OP_RSEL, FMA_OFF)
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r"(dst), [a] "r" (a), [b] "r" (b)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void fmmod4(float *dst, const float *a, const float *b, const float *c, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            FMOD_VV_CORE("dst", "a", "b", "c", OP_DSEL, FMA_OFF)
            : [off] "=&r" (off), [count] __ASM_ARG_RW(count)
            : [dst] "r"(dst), [a] "r" (a), [b] "r" (b), [c] "r" (c)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void fmmod4_fma3(float *dst, const float *a, const float *b, const float *c, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            FMOD_VV_CORE("dst", "a", "b", "c", OP_DSEL, FMA_ON)
            : [off] "=&r" (off), [count] __ASM_ARG_RW(count)
            : [dst] "r"(dst), [a] "r" (a), [b] "r" (b), [c] "r" (c)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void fmrmod4(float *dst, const float *a, const float *b, const float *c, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            FMOD_VV_CORE("dst", "a", "b", "c", OP_RSEL, FMA_OFF)
            : [off] "=&r" (off), [count] __ASM_ARG_RW(count)
            : [dst] "r"(dst), [a] "r" (a), [b] "r" (b), [c] "r" (c)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void fmrmod4_fma3(float *dst, const float *a, const float *b, const float *c, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            FMOD_VV_CORE("dst", "a", "b", "c", OP_RSEL, FMA_ON)
            : [off] "=&r" (off), [count] __ASM_ARG_RW(count)
            : [dst] "r"(dst), [a] "r" (a), [b] "r" (b), [c] "r" (c)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    #undef FMOD_VV_CORE

    #undef FMA_OFF
    #undef FMA_ON

    #undef OP_DSEL
    #undef OP_RSEL
}

#endif /* DSP_ARCH_X86_AVX_PMATH_FMOP_VV_H_ */
