/*
 * op_kx.h
 *
 *  Created on: 19 нояб. 2019 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_SSE_PMATH_OP_KX_H_
#define DSP_ARCH_X86_SSE_PMATH_OP_KX_H_

#ifndef DSP_ARCH_X86_SSE_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_SSE_IMPL */

namespace sse
{
#define OP_K2_CORE(DST, SRC, OP)   \
    __ASM_EMIT("xor         %[off], %[off]") \
    __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0") \
    __ASM_EMIT("sub         $24, %[count]") \
    __ASM_EMIT("movaps      %%xmm0, %%xmm1") \
    __ASM_EMIT("jb          2f") \
    /* 24x blocks */ \
    __ASM_EMIT("1:") \
    __ASM_EMIT("movups      0x00(%[" SRC "], %[off]), %%xmm2") \
    __ASM_EMIT("movups      0x10(%[" SRC "], %[off]), %%xmm3") \
    __ASM_EMIT("movups      0x20(%[" SRC "], %[off]), %%xmm4") \
    __ASM_EMIT("movups      0x30(%[" SRC "], %[off]), %%xmm5") \
    __ASM_EMIT("movups      0x40(%[" SRC "], %[off]), %%xmm6") \
    __ASM_EMIT("movups      0x50(%[" SRC "], %[off]), %%xmm7") \
    __ASM_EMIT(OP "ps       %%xmm0, %%xmm2") \
    __ASM_EMIT(OP "ps       %%xmm1, %%xmm3") \
    __ASM_EMIT(OP "ps       %%xmm0, %%xmm4") \
    __ASM_EMIT(OP "ps       %%xmm1, %%xmm5") \
    __ASM_EMIT(OP "ps       %%xmm0, %%xmm6") \
    __ASM_EMIT(OP "ps       %%xmm1, %%xmm7") \
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
    __ASM_EMIT("add         $8, %[count]") /* 24 - 16 */ \
    __ASM_EMIT("jl          4f") \
    __ASM_EMIT("movups      0x00(%[" SRC "], %[off]), %%xmm2") \
    __ASM_EMIT("movups      0x10(%[" SRC "], %[off]), %%xmm3") \
    __ASM_EMIT("movups      0x20(%[" SRC "], %[off]), %%xmm4") \
    __ASM_EMIT("movups      0x30(%[" SRC "], %[off]), %%xmm5") \
    __ASM_EMIT(OP "ps       %%xmm0, %%xmm2") \
    __ASM_EMIT(OP "ps       %%xmm1, %%xmm3") \
    __ASM_EMIT(OP "ps       %%xmm0, %%xmm4") \
    __ASM_EMIT(OP "ps       %%xmm1, %%xmm5") \
    __ASM_EMIT("movups      %%xmm2, 0x00(%[" DST "], %[off])") \
    __ASM_EMIT("movups      %%xmm3, 0x10(%[" DST "], %[off])") \
    __ASM_EMIT("movups      %%xmm4, 0x20(%[" DST "], %[off])") \
    __ASM_EMIT("movups      %%xmm5, 0x30(%[" DST "], %[off])") \
    __ASM_EMIT("sub         $16, %[count]") \
    __ASM_EMIT("add         $0x40, %[off]") \
    /* 8x blocks */ \
    __ASM_EMIT("4:") \
    __ASM_EMIT("add         $8, %[count]") /* 16 - 8 */ \
    __ASM_EMIT("jl          6f") \
    __ASM_EMIT("movups      0x00(%[" SRC "], %[off]), %%xmm2") \
    __ASM_EMIT("movups      0x10(%[" SRC "], %[off]), %%xmm3") \
    __ASM_EMIT(OP "ps       %%xmm0, %%xmm2") \
    __ASM_EMIT(OP "ps       %%xmm1, %%xmm3") \
    __ASM_EMIT("movups      %%xmm2, 0x00(%[" DST "], %[off])") \
    __ASM_EMIT("movups      %%xmm3, 0x10(%[" DST "], %[off])") \
    __ASM_EMIT("sub         $8, %[count]") \
    __ASM_EMIT("add         $0x20, %[off]") \
    /* 4x blocks */ \
    __ASM_EMIT("6:") \
    __ASM_EMIT("add         $4, %[count]") \
    __ASM_EMIT("jl          8f") \
    __ASM_EMIT("movups      0x00(%[" SRC "], %[off]), %%xmm2") \
    __ASM_EMIT(OP "ps       %%xmm0, %%xmm2") \
    __ASM_EMIT("movups      %%xmm2, 0x00(%[" DST "], %[off])") \
    __ASM_EMIT("sub         $4, %[count]") \
    __ASM_EMIT("add         $0x10, %[off]") \
    /* 1x blocks */ \
    __ASM_EMIT("8:") \
    __ASM_EMIT("add         $3, %[count]") \
    __ASM_EMIT("jl          10f") \
    __ASM_EMIT("9:") \
    __ASM_EMIT("movss       0x00(%[" SRC "], %[off]), %%xmm2") \
    __ASM_EMIT(OP "ss       %%xmm0, %%xmm2") \
    __ASM_EMIT("movss       %%xmm2, 0x00(%[" DST "], %[off])") \
    __ASM_EMIT("add         $0x04, %[off]") \
    __ASM_EMIT("dec         %[count]") \
    __ASM_EMIT("jge         9b") \
    __ASM_EMIT("10:")

#define ROP_K2_CORE(DST, SRC, OP)   \
    __ASM_EMIT("xor         %[off], %[off]") \
    __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0") \
    __ASM_EMIT("sub         $12, %[count]") \
    __ASM_EMIT("jb          2f") \
    /* 12x blocks */ \
    __ASM_EMIT("1:") \
    __ASM_EMIT("movups      0x00(%[" SRC "], %[off]), %%xmm4") \
    __ASM_EMIT("movups      0x10(%[" SRC "], %[off]), %%xmm5") \
    __ASM_EMIT("movups      0x20(%[" SRC "], %[off]), %%xmm6") \
    __ASM_EMIT("movaps      %%xmm0, %%xmm1") \
    __ASM_EMIT("movaps      %%xmm0, %%xmm2") \
    __ASM_EMIT("movaps      %%xmm1, %%xmm3") \
    __ASM_EMIT(OP "ps       %%xmm4, %%xmm1") \
    __ASM_EMIT(OP "ps       %%xmm5, %%xmm2") \
    __ASM_EMIT(OP "ps       %%xmm6, %%xmm3") \
    __ASM_EMIT("movups      %%xmm1, 0x00(%[" DST "], %[off])") \
    __ASM_EMIT("movups      %%xmm2, 0x10(%[" DST "], %[off])") \
    __ASM_EMIT("movups      %%xmm3, 0x20(%[" DST "], %[off])") \
    __ASM_EMIT("add         $0x30, %[off]") \
    __ASM_EMIT("sub         $12, %[count]") \
    __ASM_EMIT("jae         1b") \
    /* 8x block */ \
    __ASM_EMIT("2:") \
    __ASM_EMIT("add         $4, %[count]") /* 12 - 8 */ \
    __ASM_EMIT("jl          4f") \
    __ASM_EMIT("movups      0x00(%[" SRC "], %[off]), %%xmm4") \
    __ASM_EMIT("movups      0x10(%[" SRC "], %[off]), %%xmm5") \
    __ASM_EMIT("movaps      %%xmm0, %%xmm1") \
    __ASM_EMIT("movaps      %%xmm0, %%xmm2") \
    __ASM_EMIT(OP "ps       %%xmm4, %%xmm1") \
    __ASM_EMIT(OP "ps       %%xmm5, %%xmm2") \
    __ASM_EMIT("movups      %%xmm1, 0x00(%[" DST "], %[off])") \
    __ASM_EMIT("movups      %%xmm2, 0x10(%[" DST "], %[off])") \
    __ASM_EMIT("sub         $8, %[count]") \
    __ASM_EMIT("add         $0x20, %[off]") \
    /* 4x blocks */ \
    __ASM_EMIT("4:") \
    __ASM_EMIT("add         $4, %[count]") \
    __ASM_EMIT("jl          6f") \
    __ASM_EMIT("movups      0x00(%[" SRC "], %[off]), %%xmm4") \
    __ASM_EMIT("movaps      %%xmm0, %%xmm1") \
    __ASM_EMIT(OP "ps       %%xmm4, %%xmm1") \
    __ASM_EMIT("movups      %%xmm1, 0x00(%[" DST "], %[off])") \
    __ASM_EMIT("sub         $4, %[count]") \
    __ASM_EMIT("add         $0x10, %[off]") \
    /* 1x blocks */ \
    __ASM_EMIT("6:") \
    __ASM_EMIT("add         $3, %[count]") \
    __ASM_EMIT("jl          8f") \
    __ASM_EMIT("7:") \
    __ASM_EMIT("movss       0x00(%[" SRC "], %[off]), %%xmm4") \
    __ASM_EMIT("movaps      %%xmm0, %%xmm1") \
    __ASM_EMIT(OP "ss       %%xmm4, %%xmm1") \
    __ASM_EMIT("movss       %%xmm1, 0x00(%[" DST "], %[off])") \
    __ASM_EMIT("add         $0x04, %[off]") \
    __ASM_EMIT("dec         %[count]") \
    __ASM_EMIT("jge         7b") \
    __ASM_EMIT("8:")

    void add_k2(float *dst, float k, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            OP_K2_CORE("dst", "dst", "add")
            : [count] "+r" (count), [off] "=&r" (off),
              [k] "+Yz"(k)
            : [dst] "r" (dst)
            : "cc", "memory",
              "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void sub_k2(float *dst, float k, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            OP_K2_CORE("dst", "dst", "sub")
            : [count] "+r" (count), [off] "=&r" (off),
              [k] "+Yz"(k)
            : [dst] "r" (dst)
            : "cc", "memory",
              "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void mul_k2(float *dst, float k, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            OP_K2_CORE("dst", "dst", "mul")
            : [count] "+r" (count), [off] "=&r" (off),
              [k] "+Yz"(k)
            : [dst] "r" (dst)
            : "cc", "memory",
              "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void div_k2(float *dst, float k, size_t count)
    {
        mul_k2(dst, 1.0f / k, count);
    }

    void rsub_k2(float *dst, float k, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            ROP_K2_CORE("dst", "dst", "sub")
            : [count] "+r" (count), [off] "=&r" (off),
              [k] "+Yz"(k)
            : [dst] "r" (dst)
            : "cc", "memory",
              "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6"
        );
    }

    void rdiv_k2(float *dst, float k, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            ROP_K2_CORE("dst", "dst", "div")
            : [count] "+r" (count), [off] "=&r" (off),
              [k] "+Yz"(k)
            : [dst] "r" (dst)
            : "cc", "memory",
              "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6"
        );
    }

    void add_k3(float *dst, const float *src, float k, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            OP_K2_CORE("dst", "src", "add")
            : [count] "+r" (count), [off] "=&r" (off),
              [k] "+Yz"(k)
            : [dst] "r"(dst), [src] "r"(src)
            : "cc", "memory",
              "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void sub_k3(float *dst, const float *src, float k, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            OP_K2_CORE("dst", "src", "sub")
            : [count] "+r" (count), [off] "=&r" (off),
              [k] "+Yz"(k)
            : [dst] "r"(dst), [src] "r"(src)
            : "cc", "memory",
              "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void mul_k3(float *dst, const float *src, float k, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            OP_K2_CORE("dst", "src", "mul")
            : [count] "+r" (count), [off] "=&r" (off),
              [k] "+Yz"(k)
            : [dst] "r"(dst), [src] "r"(src)
            : "cc", "memory",
              "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void div_k3(float *dst, const float *src, float k, size_t count)
    {
        mul_k3(dst, src, 1.0f / k, count);
    }

    void rsub_k3(float *dst, const float *src, float k, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            ROP_K2_CORE("dst", "src", "sub")
            : [count] "+r" (count), [off] "=&r" (off),
              [k] "+Yz"(k)
            : [dst] "r"(dst), [src] "r"(src)
            : "cc", "memory",
              "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6"
        );
    }

    void rdiv_k3(float *dst, const float *src, float k, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            ROP_K2_CORE("dst", "src", "div")
            : [count] "+r" (count), [off] "=&r" (off),
              [k] "+Yz"(k)
            : [dst] "r"(dst), [src] "r"(src)
            : "cc", "memory",
              "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6"
        );
    }

#undef ROP_K2_CORE
#undef OP_K2_CORE

}



#endif /* DSP_ARCH_X86_SSE_PMATH_OP_KX_H_ */
