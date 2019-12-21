/*
 * abs_vv.h
 *
 *  Created on: 27 нояб. 2019 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_AVX_PMATH_ABS_VV_H_
#define DSP_ARCH_X86_AVX_PMATH_ABS_VV_H_

#ifndef DSP_ARCH_X86_AVX_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_AVX_IMPL */

#ifdef ARCH_X86_64

namespace avx
{
#define OP_DSEL(a, b)   a
#define OP_RSEL(a, b)   b

#define ABS_V4_CORE(DST, SRC1, SRC2, OP, SEL) \
    __ASM_EMIT("xor         %[off], %[off]") \
    __ASM_EMIT("vmovaps     %[SIGN], %%ymm0") \
    __ASM_EMIT("sub         $32, %[count]") \
    __ASM_EMIT("vmovaps     %%ymm0, %%ymm1") \
    __ASM_EMIT("jb          2f")    \
    /* 32x blocks */ \
    __ASM_EMIT("1:") \
    __ASM_EMIT(SEL("vmovups     0x000(%[" SRC1 "], %[off]), %%ymm6", "")) \
    __ASM_EMIT(SEL("vmovups     0x020(%[" SRC1 "], %[off]), %%ymm7", "")) \
    __ASM_EMIT(SEL("vmovups     0x040(%[" SRC1 "], %[off]), %%ymm8", "")) \
    __ASM_EMIT(SEL("vmovups     0x060(%[" SRC1 "], %[off]), %%ymm9", "")) \
    __ASM_EMIT("vandps      0x000(%[" SRC2 "], %[off]), %%ymm0, %%ymm2") \
    __ASM_EMIT("vandps      0x020(%[" SRC2 "], %[off]), %%ymm1, %%ymm3") \
    __ASM_EMIT("vandps      0x040(%[" SRC2 "], %[off]), %%ymm0, %%ymm4") \
    __ASM_EMIT("vandps      0x060(%[" SRC2 "], %[off]), %%ymm1, %%ymm5") \
    __ASM_EMIT(OP "ps       " SEL("%%ymm2", "0x000(%[" SRC1 "], %[off])") ", "  SEL("%%ymm6", "%%ymm2") ", %%ymm6") \
    __ASM_EMIT(OP "ps       " SEL("%%ymm3", "0x020(%[" SRC1 "], %[off])") ", "  SEL("%%ymm7", "%%ymm3") ", %%ymm7") \
    __ASM_EMIT(OP "ps       " SEL("%%ymm4", "0x040(%[" SRC1 "], %[off])") ", "  SEL("%%ymm8", "%%ymm4") ", %%ymm8") \
    __ASM_EMIT(OP "ps       " SEL("%%ymm5", "0x060(%[" SRC1 "], %[off])") ", "  SEL("%%ymm9", "%%ymm5") ", %%ymm9") \
    __ASM_EMIT("vmovups     %%ymm6, 0x000(%[" DST "], %[off])") \
    __ASM_EMIT("vmovups     %%ymm7, 0x020(%[" DST "], %[off])") \
    __ASM_EMIT("vmovups     %%ymm8, 0x040(%[" DST "], %[off])") \
    __ASM_EMIT("vmovups     %%ymm9, 0x060(%[" DST "], %[off])") \
    __ASM_EMIT("add         $0x80, %[off]") \
    __ASM_EMIT("sub         $32, %[count]") \
    __ASM_EMIT("jae         1b") \
    /* 16x block */ \
    __ASM_EMIT("2:") \
    __ASM_EMIT("add         $16, %[count]")          /* 32 - 16 */ \
    __ASM_EMIT("jl          4f") \
    __ASM_EMIT(SEL("vmovups     0x000(%[" SRC1 "], %[off]), %%ymm6", "")) \
    __ASM_EMIT(SEL("vmovups     0x020(%[" SRC1 "], %[off]), %%ymm7", "")) \
    __ASM_EMIT("vandps      0x000(%[" SRC2 "], %[off]), %%ymm0, %%ymm2") \
    __ASM_EMIT("vandps      0x020(%[" SRC2 "], %[off]), %%ymm1, %%ymm3") \
    __ASM_EMIT(OP "ps       " SEL("%%ymm2", "0x000(%[" SRC1 "], %[off])") ", "  SEL("%%ymm6", "%%ymm2") ", %%ymm6") \
    __ASM_EMIT(OP "ps       " SEL("%%ymm3", "0x020(%[" SRC1 "], %[off])") ", "  SEL("%%ymm7", "%%ymm3") ", %%ymm7") \
    __ASM_EMIT("vmovups     %%ymm6, 0x000(%[" DST "], %[off])") \
    __ASM_EMIT("vmovups     %%ymm7, 0x020(%[" DST "], %[off])") \
    __ASM_EMIT("add         $0x40, %[off]") \
    __ASM_EMIT("sub         $16, %[count]") \
    /* 8x block */ \
    __ASM_EMIT("4:") \
    __ASM_EMIT("add         $8, %[count]")          /* 16 - 8 */ \
    __ASM_EMIT("jl          6f") \
    __ASM_EMIT(SEL("vmovups     0x000(%[" SRC1 "], %[off]), %%ymm6", "")) \
    __ASM_EMIT("vandps      0x000(%[" SRC2 "], %[off]), %%ymm0, %%ymm2") \
    __ASM_EMIT(OP "ps       " SEL("%%ymm2", "0x000(%[" SRC1 "], %[off])") ", "  SEL("%%ymm6", "%%ymm2") ", %%ymm6") \
    __ASM_EMIT("vmovups     %%ymm6, 0x000(%[" DST "], %[off])") \
    __ASM_EMIT("add         $0x20, %[off]") \
    __ASM_EMIT("sub         $8, %[count]") \
    \
    /* 4x block */ \
    __ASM_EMIT("6:") \
    __ASM_EMIT("add         $4, %[count]")          /* 8 - 4 */ \
    __ASM_EMIT("jl          8f") \
    __ASM_EMIT(SEL("vmovups     0x000(%[" SRC1 "], %[off]), %%xmm6", "")) \
    __ASM_EMIT("vandps      0x000(%[" SRC2 "], %[off]), %%xmm0, %%xmm2") \
    __ASM_EMIT(OP "ps       " SEL("%%xmm2", "0x000(%[" SRC1 "], %[off])") ", "  SEL("%%xmm6", "%%xmm2") ", %%xmm6") \
    __ASM_EMIT("vmovups     %%xmm6, 0x000(%[" DST "], %[off])") \
    __ASM_EMIT("add         $0x10, %[off]") \
    __ASM_EMIT("sub         $4, %[count]") \
    \
    /* 1x blocks */ \
    __ASM_EMIT("8:") \
    __ASM_EMIT("add         $3, %[count]")          /* 8 - 4 */ \
    __ASM_EMIT("jl          10f") \
    __ASM_EMIT("9:") \
    __ASM_EMIT(SEL("vmovss      0x000(%[" SRC1 "], %[off]), %%xmm6", "")) \
    __ASM_EMIT("vandps      0x000(%[" SRC2 "], %[off]), %%xmm0, %%xmm2") \
    __ASM_EMIT(OP "ss       " SEL("%%xmm2", "0x000(%[" SRC1 "], %[off])") ", "  SEL("%%xmm6", "%%xmm2") ", %%xmm6") \
    __ASM_EMIT("vmovss      %%xmm6, 0x000(%[" DST "], %[off])") \
    __ASM_EMIT("add         $0x04, %[off]") \
    __ASM_EMIT("dec         %[count]") \
    __ASM_EMIT("jge         9b") \
    __ASM_EMIT("10:")

    void x64_abs_add2(float *dst, const float *src, size_t count)
    {
        IF_ARCH_X86_64(size_t off);
        ARCH_X86_64_ASM
        (
            ABS_V4_CORE("dst", "dst", "src", "vadd", OP_DSEL)
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r"(dst), [src] "r"(src),
              [SIGN] "o" (X_SIGN)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7",
              "%xmm8", "%xmm9"
        );
    }

    void x64_abs_sub2(float *dst, const float *src, size_t count)
    {
        IF_ARCH_X86_64(size_t off);
        ARCH_X86_64_ASM
        (
            ABS_V4_CORE("dst", "dst", "src", "vsub", OP_DSEL)
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r"(dst), [src] "r"(src),
              [SIGN] "o" (X_SIGN)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7",
              "%xmm8", "%xmm9"
        );
    }

    void x64_abs_rsub2(float *dst, const float *src, size_t count)
    {
        IF_ARCH_X86_64(size_t off);
        ARCH_X86_64_ASM
        (
            ABS_V4_CORE("dst", "dst", "src", "vsub", OP_RSEL)
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r"(dst), [src] "r"(src),
              [SIGN] "o" (X_SIGN)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7",
              "%xmm8", "%xmm9"
        );
    }

    void x64_abs_mul2(float *dst, const float *src, size_t count)
    {
        IF_ARCH_X86_64(size_t off);
        ARCH_X86_64_ASM
        (
            ABS_V4_CORE("dst", "dst", "src", "vmul", OP_DSEL)
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r"(dst), [src] "r"(src),
              [SIGN] "o" (X_SIGN)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7",
              "%xmm8", "%xmm9"
        );
    }

    void x64_abs_div2(float *dst, const float *src, size_t count)
    {
        IF_ARCH_X86_64(size_t off);
        ARCH_X86_64_ASM
        (
            ABS_V4_CORE("dst", "dst", "src", "vdiv", OP_DSEL)
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r"(dst), [src] "r"(src),
              [SIGN] "o" (X_SIGN)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7",
              "%xmm8", "%xmm9"
        );
    }

    void x64_abs_rdiv2(float *dst, const float *src, size_t count)
    {
        IF_ARCH_X86_64(size_t off);
        ARCH_X86_64_ASM
        (
            ABS_V4_CORE("dst", "dst", "src", "vdiv", OP_RSEL)
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r"(dst), [src] "r"(src),
              [SIGN] "o" (X_SIGN)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7",
              "%xmm8", "%xmm9"
        );
    }

    void x64_abs_add3(float *dst, const float *src1, const float *src2, size_t count)
    {
        IF_ARCH_X86_64(size_t off);
        ARCH_X86_64_ASM
        (
            ABS_V4_CORE("dst", "src1", "src2", "vadd", OP_DSEL)
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r"(dst), [src1] "r" (src1), [src2] "r" (src2),
              [SIGN] "o" (X_SIGN)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7",
              "%xmm8", "%xmm9"
        );
    }

    void x64_abs_sub3(float *dst, const float *src1, const float *src2, size_t count)
    {
        IF_ARCH_X86_64(size_t off);
        ARCH_X86_64_ASM
        (
            ABS_V4_CORE("dst", "src1", "src2", "vsub", OP_DSEL)
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r"(dst), [src1] "r" (src1), [src2] "r" (src2),
              [SIGN] "o" (X_SIGN)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7",
              "%xmm8", "%xmm9"
        );
    }

    void x64_abs_rsub3(float *dst, const float *src1, const float *src2, size_t count)
    {
        IF_ARCH_X86_64(size_t off);
        ARCH_X86_64_ASM
        (
            ABS_V4_CORE("dst", "src1", "src2", "vsub", OP_RSEL)
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r"(dst), [src1] "r" (src1), [src2] "r" (src2),
              [SIGN] "o" (X_SIGN)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7",
              "%xmm8", "%xmm9"
        );
    }

    void x64_abs_mul3(float *dst, const float *src1, const float *src2, size_t count)
    {
        IF_ARCH_X86_64(size_t off);
        ARCH_X86_64_ASM
        (
            ABS_V4_CORE("dst", "src1", "src2", "vmul", OP_DSEL)
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r"(dst), [src1] "r" (src1), [src2] "r" (src2),
              [SIGN] "o" (X_SIGN)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7",
              "%xmm8", "%xmm9"
        );
    }

    void x64_abs_div3(float *dst, const float *src1, const float *src2, size_t count)
    {
        IF_ARCH_X86_64(size_t off);
        ARCH_X86_64_ASM
        (
            ABS_V4_CORE("dst", "src1", "src2", "vdiv", OP_DSEL)
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r"(dst), [src1] "r" (src1), [src2] "r" (src2),
              [SIGN] "o" (X_SIGN)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7",
              "%xmm8", "%xmm9"
        );
    }

    void x64_abs_rdiv3(float *dst, const float *src1, const float *src2, size_t count)
    {
        IF_ARCH_X86_64(size_t off);
        ARCH_X86_64_ASM
        (
            ABS_V4_CORE("dst", "src1", "src2", "vdiv", OP_RSEL)
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r"(dst), [src1] "r" (src1), [src2] "r" (src2),
              [SIGN] "o" (X_SIGN)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7",
              "%xmm8", "%xmm9"
        );
    }

    #undef ABS_V4_CORE

#define ABS_CORE(DST, SRC)  \
    __ASM_EMIT("xor         %[off], %[off]") \
    __ASM_EMIT("vmovaps     %[SIGN], %%ymm8") \
    __ASM_EMIT("sub         $64, %[count]") \
    __ASM_EMIT("vmovaps     %%ymm8, %%ymm9") \
    __ASM_EMIT("jb          2f")    \
    /* 64x blocks */ \
    __ASM_EMIT("1:") \
    __ASM_EMIT("vandps      0x000(%[" SRC "], %[off]), %%ymm8, %%ymm0") \
    __ASM_EMIT("vandps      0x020(%[" SRC "], %[off]), %%ymm9, %%ymm1") \
    __ASM_EMIT("vandps      0x040(%[" SRC "], %[off]), %%ymm8, %%ymm2") \
    __ASM_EMIT("vandps      0x060(%[" SRC "], %[off]), %%ymm9, %%ymm3") \
    __ASM_EMIT("vandps      0x080(%[" SRC "], %[off]), %%ymm8, %%ymm4") \
    __ASM_EMIT("vandps      0x0a0(%[" SRC "], %[off]), %%ymm9, %%ymm5") \
    __ASM_EMIT("vandps      0x0c0(%[" SRC "], %[off]), %%ymm8, %%ymm6") \
    __ASM_EMIT("vandps      0x0e0(%[" SRC "], %[off]), %%ymm9, %%ymm7") \
    __ASM_EMIT("vmovups     %%ymm0, 0x000(%[" DST "], %[off])") \
    __ASM_EMIT("vmovups     %%ymm1, 0x020(%[" DST "], %[off])") \
    __ASM_EMIT("vmovups     %%ymm2, 0x040(%[" DST "], %[off])") \
    __ASM_EMIT("vmovups     %%ymm3, 0x060(%[" DST "], %[off])") \
    __ASM_EMIT("vmovups     %%ymm4, 0x080(%[" DST "], %[off])") \
    __ASM_EMIT("vmovups     %%ymm5, 0x0a0(%[" DST "], %[off])") \
    __ASM_EMIT("vmovups     %%ymm6, 0x0c0(%[" DST "], %[off])") \
    __ASM_EMIT("vmovups     %%ymm7, 0x0e0(%[" DST "], %[off])") \
    __ASM_EMIT("add         $0x100, %[off]") \
    __ASM_EMIT("sub         $64, %[count]") \
    __ASM_EMIT("jae         1b") \
    /* 32x block */ \
    __ASM_EMIT("2:") \
    __ASM_EMIT("add         $32, %[count]") \
    __ASM_EMIT("jl          4f") \
    __ASM_EMIT("vandps      0x000(%[" SRC "], %[off]), %%ymm8, %%ymm0") \
    __ASM_EMIT("vandps      0x020(%[" SRC "], %[off]), %%ymm9, %%ymm1") \
    __ASM_EMIT("vandps      0x040(%[" SRC "], %[off]), %%ymm8, %%ymm2") \
    __ASM_EMIT("vandps      0x060(%[" SRC "], %[off]), %%ymm9, %%ymm3") \
    __ASM_EMIT("vmovups     %%ymm0, 0x000(%[" DST "], %[off])") \
    __ASM_EMIT("vmovups     %%ymm1, 0x020(%[" DST "], %[off])") \
    __ASM_EMIT("vmovups     %%ymm2, 0x040(%[" DST "], %[off])") \
    __ASM_EMIT("vmovups     %%ymm3, 0x060(%[" DST "], %[off])") \
    __ASM_EMIT("sub         $32, %[count]") \
    __ASM_EMIT("add         $0x80, %[off]") \
    /* 16x block */ \
    __ASM_EMIT("4:") \
    __ASM_EMIT("add         $16, %[count]") \
    __ASM_EMIT("jl          6f") \
    __ASM_EMIT("vandps      0x000(%[" SRC "], %[off]), %%ymm8, %%ymm0") \
    __ASM_EMIT("vandps      0x020(%[" SRC "], %[off]), %%ymm9, %%ymm1") \
    __ASM_EMIT("vmovups     %%ymm0, 0x000(%[" DST "], %[off])") \
    __ASM_EMIT("vmovups     %%ymm1, 0x020(%[" DST "], %[off])") \
    __ASM_EMIT("sub         $16, %[count]") \
    __ASM_EMIT("add         $0x40, %[off]") \
    /* 8x block */ \
    __ASM_EMIT("6:") \
    __ASM_EMIT("add         $8, %[count]") \
    __ASM_EMIT("jl          8f") \
    __ASM_EMIT("vandps      0x000(%[" SRC "], %[off]), %%ymm8, %%ymm0") \
    __ASM_EMIT("vmovups     %%ymm0, 0x000(%[" DST "], %[off])") \
    __ASM_EMIT("sub         $8, %[count]") \
    __ASM_EMIT("add         $0x20, %[off]") \
    /* 4x block */ \
    __ASM_EMIT("8:") \
    __ASM_EMIT("add         $4, %[count]") \
    __ASM_EMIT("jl          10f") \
    __ASM_EMIT("vandps      0x000(%[" SRC "], %[off]), %%xmm8, %%xmm0") \
    __ASM_EMIT("vmovups     %%xmm0, 0x000(%[" DST "], %[off])") \
    __ASM_EMIT("sub         $4, %[count]") \
    __ASM_EMIT("add         $0x10, %[off]") \
    /* 1x block */ \
    __ASM_EMIT("10:") \
    __ASM_EMIT("add         $3, %[count]") \
    __ASM_EMIT("jl          12f") \
    __ASM_EMIT("11:") \
    __ASM_EMIT("vmovss      0x00(%[" SRC "], %[off]), %%xmm0") \
    __ASM_EMIT("vandps      %%xmm0, %%xmm8, %%xmm0") \
    __ASM_EMIT("vmovss      %%xmm0, 0x00(%[" DST "], %[off])") \
    __ASM_EMIT("add         $0x04, %[off]") \
    __ASM_EMIT("dec         %[count]") \
    __ASM_EMIT("jge         11b") \
    __ASM_EMIT("12:")

    void x64_abs1(float *dst, size_t count)
    {
        IF_ARCH_X86_64(size_t off);
        ARCH_X86_64_ASM
        (
            ABS_CORE("dst", "dst")
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r" (dst),
              [SIGN] "o" (X_SIGN)
            : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7",
              "%xmm8", "%xmm9"
        );
    }

    void x64_abs2(float *dst, const float *src, size_t count)
    {
        IF_ARCH_X86_64(size_t off);
        ARCH_X86_64_ASM
        (
            ABS_CORE("dst", "src")
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r" (dst), [src] "r" (src),
              [SIGN] "o" (X_SIGN)
            : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7",
              "%xmm8", "%xmm9"
        );
    }

#undef ABS_CORE

#undef OP_DSEL
#undef OP_RSEL
}

#endif

#endif /* DSP_ARCH_X86_AVX_PMATH_ABS_VV_H_ */
