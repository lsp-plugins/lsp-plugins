/*
 * abs.h
 *
 *  Created on: 21 мар. 2017 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_SSE_ABS_H_
#define DSP_ARCH_X86_SSE_ABS_H_

#ifndef DSP_ARCH_X86_SSE_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_SSE_IMPL */

namespace sse
{
#define ABS_CORE(DST, SRC)  \
    __ASM_EMIT("xor         %[off], %[off]") \
    __ASM_EMIT("movaps      %[X_SIGN], %%xmm0") \
    __ASM_EMIT("sub         $24, %[count]") \
    __ASM_EMIT("movaps      %%xmm0, %%xmm1") \
    __ASM_EMIT("jb          2f")    \
    /* 24x blocks */ \
    __ASM_EMIT("1:") \
    __ASM_EMIT("movups      0x00(%[" SRC "], %[off]), %%xmm2") \
    __ASM_EMIT("movups      0x10(%[" SRC "], %[off]), %%xmm3") \
    __ASM_EMIT("movups      0x20(%[" SRC "], %[off]), %%xmm4") \
    __ASM_EMIT("movups      0x30(%[" SRC "], %[off]), %%xmm5") \
    __ASM_EMIT("movups      0x40(%[" SRC "], %[off]), %%xmm6") \
    __ASM_EMIT("movups      0x50(%[" SRC "], %[off]), %%xmm7") \
    __ASM_EMIT("andps       %%xmm0, %%xmm2") \
    __ASM_EMIT("andps       %%xmm1, %%xmm3") \
    __ASM_EMIT("andps       %%xmm0, %%xmm4") \
    __ASM_EMIT("andps       %%xmm1, %%xmm5") \
    __ASM_EMIT("andps       %%xmm0, %%xmm6") \
    __ASM_EMIT("andps       %%xmm1, %%xmm7") \
    __ASM_EMIT("movups      %%xmm2, 0x00(%[" DST "], %[off])") \
    __ASM_EMIT("movups      %%xmm3, 0x10(%[" DST "], %[off])") \
    __ASM_EMIT("movups      %%xmm4, 0x20(%[" DST "], %[off])") \
    __ASM_EMIT("movups      %%xmm5, 0x30(%[" DST "], %[off])") \
    __ASM_EMIT("movups      %%xmm6, 0x40(%[" DST "], %[off])") \
    __ASM_EMIT("movups      %%xmm7, 0x50(%[" DST "], %[off])") \
    __ASM_EMIT("add         $0x60, %[off]") \
    __ASM_EMIT("sub         $24, %[count]") \
    __ASM_EMIT("jae         1b") \
    /* 16x block */ \
    __ASM_EMIT("2:") \
    __ASM_EMIT("add         $8, %[count]") \
    __ASM_EMIT("jl          4f") \
    __ASM_EMIT("movups      0x00(%[" SRC "], %[off]), %%xmm2") \
    __ASM_EMIT("movups      0x10(%[" SRC "], %[off]), %%xmm3") \
    __ASM_EMIT("movups      0x20(%[" SRC "], %[off]), %%xmm4") \
    __ASM_EMIT("movups      0x30(%[" SRC "], %[off]), %%xmm5") \
    __ASM_EMIT("andps       %%xmm0, %%xmm2") \
    __ASM_EMIT("andps       %%xmm1, %%xmm3") \
    __ASM_EMIT("andps       %%xmm0, %%xmm4") \
    __ASM_EMIT("andps       %%xmm1, %%xmm5") \
    __ASM_EMIT("movups      %%xmm2, 0x00(%[" DST "], %[off])") \
    __ASM_EMIT("movups      %%xmm3, 0x10(%[" DST "], %[off])") \
    __ASM_EMIT("movups      %%xmm4, 0x20(%[" DST "], %[off])") \
    __ASM_EMIT("movups      %%xmm5, 0x30(%[" DST "], %[off])") \
    __ASM_EMIT("sub         $16, %[count]") \
    __ASM_EMIT("add         $0x40, %[off]") \
    /* 8x block */ \
    __ASM_EMIT("4:") \
    __ASM_EMIT("add         $8, %[count]") \
    __ASM_EMIT("jl          6f") \
    __ASM_EMIT("movups      0x00(%[" SRC "], %[off]), %%xmm2") \
    __ASM_EMIT("movups      0x10(%[" SRC "], %[off]), %%xmm3") \
    __ASM_EMIT("andps       %%xmm0, %%xmm2") \
    __ASM_EMIT("andps       %%xmm1, %%xmm3") \
    __ASM_EMIT("movups      %%xmm2, 0x00(%[" DST "], %[off])") \
    __ASM_EMIT("movups      %%xmm3, 0x10(%[" DST "], %[off])") \
    __ASM_EMIT("sub         $8, %[count]") \
    __ASM_EMIT("add         $0x20, %[off]") \
    /* 4x block */ \
    __ASM_EMIT("6:") \
    __ASM_EMIT("add         $4, %[count]") \
    __ASM_EMIT("jl          8f") \
    __ASM_EMIT("movups      0x00(%[" SRC "], %[off]), %%xmm2") \
    __ASM_EMIT("andps       %%xmm0, %%xmm2") \
    __ASM_EMIT("movups      %%xmm2, 0x00(%[" DST "], %[off])") \
    __ASM_EMIT("sub         $4, %[count]") \
    __ASM_EMIT("add         $0x10, %[off]") \
    /* 1x block */ \
    __ASM_EMIT("8:") \
    __ASM_EMIT("add         $3, %[count]") \
    __ASM_EMIT("jl          10f") \
    __ASM_EMIT("9:") \
    __ASM_EMIT("movss       0x00(%[" SRC "], %[off]), %%xmm2") \
    __ASM_EMIT("andps       %%xmm0, %%xmm2") \
    __ASM_EMIT("movss       %%xmm2, 0x00(%[" DST "], %[off])") \
    __ASM_EMIT("add         $0x04, %[off]") \
    __ASM_EMIT("dec         %[count]") \
    __ASM_EMIT("jge         9b") \
    __ASM_EMIT("10:")

    void abs1(float *dst, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            ABS_CORE("dst", "dst")
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r" (dst),
              [X_SIGN] "m" (X_SIGN)
            : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void abs2(float *dst, const float *src, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            ABS_CORE("dst", "src")
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r" (dst), [src] "r" (src),
              [X_SIGN] "m" (X_SIGN)
            : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

#undef ABS_CORE

#define ABS_OP2_CORE(OP, DST, SRC1, SRC2)  \
    __ASM_EMIT("xor         %[off], %[off]") \
    __ASM_EMIT("movaps      %[X_SIGN], %%xmm0") \
    __ASM_EMIT("sub         $12, %[count]") \
    __ASM_EMIT("movaps      %%xmm0, %%xmm1") \
    __ASM_EMIT("jb          2f")    \
    /* 12x blocks */ \
    __ASM_EMIT("1:") \
    __ASM_EMIT("movups      0x00(%[" SRC2 "], %[off]), %%xmm2") \
    __ASM_EMIT("movups      0x10(%[" SRC2 "], %[off]), %%xmm3") \
    __ASM_EMIT("movups      0x20(%[" SRC2 "], %[off]), %%xmm4") \
    __ASM_EMIT("movups      0x00(%[" SRC1 "], %[off]), %%xmm5") \
    __ASM_EMIT("movups      0x10(%[" SRC1 "], %[off]), %%xmm6") \
    __ASM_EMIT("movups      0x20(%[" SRC1 "], %[off]), %%xmm7") \
    __ASM_EMIT("andps       %%xmm0, %%xmm2") \
    __ASM_EMIT("andps       %%xmm1, %%xmm3") \
    __ASM_EMIT("andps       %%xmm0, %%xmm4") \
    __ASM_EMIT(OP "ps       %%xmm2, %%xmm5") \
    __ASM_EMIT(OP "ps       %%xmm3, %%xmm6") \
    __ASM_EMIT(OP "ps       %%xmm4, %%xmm7") \
    __ASM_EMIT("movups      %%xmm5, 0x00(%[" DST "], %[off])") \
    __ASM_EMIT("movups      %%xmm6, 0x10(%[" DST "], %[off])") \
    __ASM_EMIT("movups      %%xmm7, 0x20(%[" DST "], %[off])") \
    __ASM_EMIT("add         $0x30, %[off]") \
    __ASM_EMIT("sub         $12, %[count]") \
    __ASM_EMIT("jae         1b") \
    /* 8x block */ \
    __ASM_EMIT("2:") \
    __ASM_EMIT("add         $4, %[count]") \
    __ASM_EMIT("jl          4f") \
    __ASM_EMIT("movups      0x00(%[" SRC2 "], %[off]), %%xmm2") \
    __ASM_EMIT("movups      0x10(%[" SRC2 "], %[off]), %%xmm3") \
    __ASM_EMIT("movups      0x00(%[" SRC1 "], %[off]), %%xmm4") \
    __ASM_EMIT("movups      0x10(%[" SRC1 "], %[off]), %%xmm5") \
    __ASM_EMIT("andps       %%xmm0, %%xmm2") \
    __ASM_EMIT("andps       %%xmm1, %%xmm3") \
    __ASM_EMIT(OP "ps       %%xmm2, %%xmm4") \
    __ASM_EMIT(OP "ps       %%xmm3, %%xmm5") \
    __ASM_EMIT("movups      %%xmm4, 0x00(%[" DST "], %[off])") \
    __ASM_EMIT("movups      %%xmm5, 0x10(%[" DST "], %[off])") \
    __ASM_EMIT("sub         $8, %[count]") \
    __ASM_EMIT("add         $0x20, %[off]") \
    /* 4x block */ \
    __ASM_EMIT("4:") \
    __ASM_EMIT("add         $4, %[count]") \
    __ASM_EMIT("jl          6f") \
    __ASM_EMIT("movups      0x00(%[" SRC2 "], %[off]), %%xmm2") \
    __ASM_EMIT("movups      0x00(%[" SRC1 "], %[off]), %%xmm4") \
    __ASM_EMIT("andps       %%xmm0, %%xmm2") \
    __ASM_EMIT(OP "ps       %%xmm2, %%xmm4") \
    __ASM_EMIT("movups      %%xmm4, 0x00(%[" DST "], %[off])") \
    __ASM_EMIT("sub         $4, %[count]") \
    __ASM_EMIT("add         $0x10, %[off]") \
    /* 1x blocks */ \
    __ASM_EMIT("6:") \
    __ASM_EMIT("add         $3, %[count]") \
    __ASM_EMIT("jl          8f") \
    __ASM_EMIT("7:") \
    __ASM_EMIT("movss       0x00(%[" SRC2 "], %[off]), %%xmm2") \
    __ASM_EMIT("movss       0x00(%[" SRC1 "], %[off]), %%xmm4") \
    __ASM_EMIT("andps       %%xmm0, %%xmm2") \
    __ASM_EMIT(OP "ss       %%xmm2, %%xmm4") \
    __ASM_EMIT("movss       %%xmm4, 0x00(%[" DST "], %[off])") \
    __ASM_EMIT("add         $0x04, %[off]") \
    __ASM_EMIT("dec         %[count]") \
    __ASM_EMIT("jge         7b") \
    __ASM_EMIT("8:") \

    void abs_add2(float *dst, const float *src, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            ABS_OP2_CORE("add", "dst", "dst", "src")
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r" (dst), [src] "r" (src),
              [X_SIGN] "m" (X_SIGN)
            : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void abs_sub2(float *dst, const float *src, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            ABS_OP2_CORE("sub", "dst", "dst", "src")
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r" (dst), [src] "r" (src),
              [X_SIGN] "m" (X_SIGN)
            : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void abs_mul2(float *dst, const float *src, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            ABS_OP2_CORE("mul", "dst", "dst", "src")
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r" (dst), [src] "r" (src),
              [X_SIGN] "m" (X_SIGN)
            : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void abs_div2(float *dst, const float *src, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            ABS_OP2_CORE("div", "dst", "dst", "src")
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r" (dst), [src] "r" (src),
              [X_SIGN] "m" (X_SIGN)
            : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void abs_add3(float *dst, const float *src1, const float *src2, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            ABS_OP2_CORE("add", "dst", "src1", "src2")
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r" (dst), [src1] "r" (src1), [src2] "r" (src2),
              [X_SIGN] "m" (X_SIGN)
            : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void abs_sub3(float *dst, const float *src1, const float *src2, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            ABS_OP2_CORE("sub", "dst", "src1", "src2")
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r" (dst), [src1] "r" (src1), [src2] "r" (src2),
              [X_SIGN] "m" (X_SIGN)
            : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void abs_mul3(float *dst, const float *src1, const float *src2, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            ABS_OP2_CORE("mul", "dst", "src1", "src2")
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r" (dst), [src1] "r" (src1), [src2] "r" (src2),
              [X_SIGN] "m" (X_SIGN)
            : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void abs_div3(float *dst, const float *src1, const float *src2, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            ABS_OP2_CORE("div", "dst", "src1", "src2")
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r" (dst), [src1] "r" (src1), [src2] "r" (src2),
              [X_SIGN] "m" (X_SIGN)
            : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

#undef ABS_OP2_CORE
}

#endif /* DSP_ARCH_X86_SSE_ABS_H_ */
