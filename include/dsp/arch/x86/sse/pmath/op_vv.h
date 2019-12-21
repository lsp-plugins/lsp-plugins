/*
 * op_vv.h
 *
 *  Created on: 19 нояб. 2019 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_SSE_PMATH_OP_VV_H_
#define DSP_ARCH_X86_SSE_PMATH_OP_VV_H_

#ifndef DSP_ARCH_X86_SSE_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_SSE_IMPL */

namespace sse
{
#define OP_CORE(OP, SRC1, SRC2, DST)   \
    __ASM_EMIT("xor         %[off], %[off]") \
    __ASM_EMIT("sub         $16, %[count]") \
    __ASM_EMIT("jb          2f") \
    /* 16x blocks */ \
    __ASM_EMIT("1:") \
    __ASM_EMIT("movups      0x00(%[" SRC1 "], %[off]), %%xmm0") \
    __ASM_EMIT("movups      0x10(%[" SRC1 "], %[off]), %%xmm1") \
    __ASM_EMIT("movups      0x20(%[" SRC1 "], %[off]), %%xmm2") \
    __ASM_EMIT("movups      0x30(%[" SRC1 "], %[off]), %%xmm3") \
    __ASM_EMIT("movups      0x00(%[" SRC2 "], %[off]), %%xmm4") \
    __ASM_EMIT("movups      0x10(%[" SRC2 "], %[off]), %%xmm5") \
    __ASM_EMIT("movups      0x20(%[" SRC2 "], %[off]), %%xmm6") \
    __ASM_EMIT("movups      0x30(%[" SRC2 "], %[off]), %%xmm7") \
    __ASM_EMIT(OP "ps       %%xmm4, %%xmm0") \
    __ASM_EMIT(OP "ps       %%xmm5, %%xmm1") \
    __ASM_EMIT(OP "ps       %%xmm6, %%xmm2") \
    __ASM_EMIT(OP "ps       %%xmm7, %%xmm3") \
    __ASM_EMIT("movups      %%xmm0, 0x00(%[" DST "], %[off])") \
    __ASM_EMIT("movups      %%xmm1, 0x10(%[" DST "], %[off])") \
    __ASM_EMIT("movups      %%xmm2, 0x20(%[" DST "], %[off])") \
    __ASM_EMIT("movups      %%xmm3, 0x30(%[" DST "], %[off])") \
    __ASM_EMIT("add         $0x40, %[off]") \
    __ASM_EMIT("sub         $0x10, %[count]") \
    __ASM_EMIT("jae         1b") \
    /* 8x blocks */ \
    __ASM_EMIT("2:") \
    __ASM_EMIT("add         $8, %[count]") \
    __ASM_EMIT("jl          4f") \
    __ASM_EMIT("movups      0x00(%[" SRC1 "], %[off]), %%xmm0") \
    __ASM_EMIT("movups      0x10(%[" SRC1 "], %[off]), %%xmm1") \
    __ASM_EMIT("movups      0x00(%[" SRC2 "], %[off]), %%xmm4") \
    __ASM_EMIT("movups      0x10(%[" SRC2 "], %[off]), %%xmm5") \
    __ASM_EMIT(OP "ps       %%xmm4, %%xmm0") \
    __ASM_EMIT(OP "ps       %%xmm5, %%xmm1") \
    __ASM_EMIT("movups      %%xmm0, 0x00(%[" DST "], %[off])") \
    __ASM_EMIT("movups      %%xmm1, 0x10(%[" DST "], %[off])") \
    __ASM_EMIT("sub         $8, %[count]") \
    __ASM_EMIT("add         $0x20, %[off]") \
    /* 4x blocks */ \
    __ASM_EMIT("4:") \
    __ASM_EMIT("add         $4, %[count]") \
    __ASM_EMIT("jl          6f") \
    __ASM_EMIT("movups      0x00(%[" SRC1 "], %[off]), %%xmm0") \
    __ASM_EMIT("movups      0x00(%[" SRC2 "], %[off]), %%xmm4") \
    __ASM_EMIT(OP "ps       %%xmm4, %%xmm0") \
    __ASM_EMIT("movups      %%xmm0, 0x00(%[" DST "], %[off])") \
    __ASM_EMIT("sub         $4, %[count]") \
    __ASM_EMIT("add         $0x10, %[off]") \
    /* 1x blocks */ \
    __ASM_EMIT("6:") \
    __ASM_EMIT("add         $3, %[count]") \
    __ASM_EMIT("jl          8f") \
    __ASM_EMIT("7:") \
    __ASM_EMIT("movss       0x00(%[" SRC1 "], %[off]), %%xmm0") \
    __ASM_EMIT("movss       0x00(%[" SRC2 "], %[off]), %%xmm4") \
    __ASM_EMIT(OP "ss       %%xmm4, %%xmm0") \
    __ASM_EMIT("movss       %%xmm0, 0x00(%[" DST "], %[off])") \
    __ASM_EMIT("add         $0x04, %[off]") \
    __ASM_EMIT("dec         %[count]") \
    __ASM_EMIT("jge         7b") \
    __ASM_EMIT("8:")

    void add2(float *dst, const float *src, size_t count)
    {
        size_t off;
        ARCH_X86_ASM
        (
            OP_CORE("add", "dst", "src", "dst")
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r" (dst), [src] "r" (src)
            : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void sub2(float *dst, const float *src, size_t count)
    {
        size_t off;
        ARCH_X86_ASM
        (
            OP_CORE("sub", "dst", "src", "dst")
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r" (dst), [src] "r" (src)
            : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void rsub2(float *dst, const float *src, size_t count)
    {
        size_t off;
        ARCH_X86_ASM
        (
            OP_CORE("sub", "src", "dst", "dst")
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r" (dst), [src] "r" (src)
            : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void mul2(float *dst, const float *src, size_t count)
    {
        size_t off;
        ARCH_X86_ASM
        (
            OP_CORE("mul", "dst", "src", "dst")
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r" (dst), [src] "r" (src)
            : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void div2(float *dst, const float *src, size_t count)
    {
        size_t off;
        ARCH_X86_ASM
        (
            OP_CORE("div", "dst", "src", "dst")
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r" (dst), [src] "r" (src)
            : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void rdiv2(float *dst, const float *src, size_t count)
    {
        size_t off;
        ARCH_X86_ASM
        (
            OP_CORE("div", "src", "dst", "dst")
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r" (dst), [src] "r" (src)
            : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void add3(float *dst, const float *src1, const float *src2, size_t count)
    {
        size_t off;
        ARCH_X86_ASM
        (
            OP_CORE("add", "src1", "src2", "dst")
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r" (dst), [src1] "r" (src1), [src2] "r" (src2)
            : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void sub3(float *dst, const float *src1, const float *src2, size_t count)
    {
        size_t off;
        ARCH_X86_ASM
        (
            OP_CORE("sub", "src1", "src2", "dst")
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r" (dst), [src1] "r" (src1), [src2] "r" (src2)
            : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void mul3(float *dst, const float *src1, const float *src2, size_t count)
    {
        size_t off;
        ARCH_X86_ASM
        (
            OP_CORE("mul", "src1", "src2", "dst")
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r" (dst), [src1] "r" (src1), [src2] "r" (src2)
            : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void div3(float *dst, const float *src1, const float *src2, size_t count)
    {
        size_t off;
        ARCH_X86_ASM
        (
            OP_CORE("div", "src1", "src2", "dst")
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r" (dst), [src1] "r" (src1), [src2] "r" (src2)
            : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

#undef OP_CORE
}

#endif /* DSP_ARCH_X86_SSE_PMATH_OP_VV_H_ */
