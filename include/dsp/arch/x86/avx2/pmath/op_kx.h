/*
 * op_kx.h
 *
 *  Created on: 23 дек. 2019 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_AVX2_PMATH_OP_KX_H_
#define DSP_ARCH_X86_AVX2_PMATH_OP_KX_H_

#ifndef DSP_ARCH_X86_AVX2_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_AVX2_IMPL */

namespace avx2
{
    #define OP_DSEL(a, b)   a
    #define OP_RSEL(a, b)   b

    #define OP_K4_CORE(DST, SRC, OP, SEL) \
        __ASM_EMIT("xor         %[off], %[off]") \
        __ASM_EMIT("vbroadcastss %%xmm0, %%ymm0") \
        __ASM_EMIT("sub         $32, %[count]") \
        __ASM_EMIT("vmovaps     %%ymm0, %%ymm1") \
        __ASM_EMIT("jb          2f")    \
        /* 32x blocks */ \
        __ASM_EMIT("1:") \
        __ASM_EMIT(SEL("", "vmovups     0x000(%[" SRC "], %[off]), %%ymm4")) \
        __ASM_EMIT(SEL("", "vmovups     0x020(%[" SRC "], %[off]), %%ymm5")) \
        __ASM_EMIT(SEL("", "vmovups     0x040(%[" SRC "], %[off]), %%ymm6")) \
        __ASM_EMIT(SEL("", "vmovups     0x060(%[" SRC "], %[off]), %%ymm7")) \
        __ASM_EMIT(OP "ps       " SEL("0x000(%[" SRC "], %[off])", "%%ymm0") ", " SEL("%%ymm0", "%%ymm4") ", %%ymm4") \
        __ASM_EMIT(OP "ps       " SEL("0x020(%[" SRC "], %[off])", "%%ymm1") ", " SEL("%%ymm1", "%%ymm5") ", %%ymm5") \
        __ASM_EMIT(OP "ps       " SEL("0x040(%[" SRC "], %[off])", "%%ymm0") ", " SEL("%%ymm0", "%%ymm6") ", %%ymm6") \
        __ASM_EMIT(OP "ps       " SEL("0x060(%[" SRC "], %[off])", "%%ymm1") ", " SEL("%%ymm1", "%%ymm7") ", %%ymm7") \
        __ASM_EMIT("vmovups     %%ymm4, 0x000(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%ymm5, 0x020(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%ymm6, 0x040(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%ymm7, 0x060(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x80, %[off]") \
        __ASM_EMIT("sub         $32, %[count]") \
        __ASM_EMIT("jae         1b") \
        /* 16x block */ \
        __ASM_EMIT("2:") \
        __ASM_EMIT("add         $16, %[count]")          /* 32 - 16 */ \
        __ASM_EMIT("jl          4f") \
        __ASM_EMIT(SEL("", "vmovups     0x000(%[" SRC "], %[off]), %%ymm4")) \
        __ASM_EMIT(SEL("", "vmovups     0x020(%[" SRC "], %[off]), %%ymm5")) \
        __ASM_EMIT(OP "ps       " SEL("0x000(%[" SRC "], %[off])", "%%ymm0") ", " SEL("%%ymm0", "%%ymm4") ", %%ymm4") \
        __ASM_EMIT(OP "ps       " SEL("0x020(%[" SRC "], %[off])", "%%ymm1") ", " SEL("%%ymm1", "%%ymm5") ", %%ymm5") \
        __ASM_EMIT("vmovups     %%ymm4, 0x000(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%ymm5, 0x020(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x40, %[off]") \
        __ASM_EMIT("sub         $16, %[count]") \
        /* 8x block */ \
        __ASM_EMIT("4:") \
        __ASM_EMIT("add         $8, %[count]")          /* 16 - 8 */ \
        __ASM_EMIT("jl          6f") \
        __ASM_EMIT(SEL("", "vmovups     0x000(%[" SRC "], %[off]), %%ymm4")) \
        __ASM_EMIT(OP "ps       " SEL("0x000(%[" SRC "], %[off])", "%%ymm0") ", " SEL("%%ymm0", "%%ymm4") ", %%ymm4") \
        __ASM_EMIT("vmovups     %%ymm4, 0x000(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x20, %[off]") \
        __ASM_EMIT("sub         $8, %[count]") \
        \
        /* 4x block */ \
        __ASM_EMIT("6:") \
        __ASM_EMIT("add         $4, %[count]")          /* 8 - 4 */ \
        __ASM_EMIT("jl          8f") \
        __ASM_EMIT(SEL("", "vmovups     0x000(%[" SRC "], %[off]), %%xmm4")) \
        __ASM_EMIT(OP "ps       " SEL("0x000(%[" SRC "], %[off])", "%%xmm0") ", " SEL("%%xmm0", "%%xmm4") ", %%xmm4") \
        __ASM_EMIT("vmovups     %%xmm4, 0x000(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x10, %[off]") \
        __ASM_EMIT("sub         $4, %[count]") \
        \
        /* 1x blocks */ \
        __ASM_EMIT("8:") \
        __ASM_EMIT("add         $3, %[count]")          /* 8 - 4 */ \
        __ASM_EMIT("jl          10f") \
        __ASM_EMIT("9:") \
        __ASM_EMIT(SEL("", "vmovss      0x000(%[" SRC "], %[off]), %%xmm4")) \
        __ASM_EMIT(OP "ss       " SEL("0x000(%[" SRC "], %[off])", "%%xmm0") ", " SEL("%%xmm0", "%%xmm4") ", %%xmm4") \
        __ASM_EMIT("vmovss      %%xmm4, 0x000(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x04, %[off]") \
        __ASM_EMIT("dec         %[count]") \
        __ASM_EMIT("jge         9b") \
        __ASM_EMIT("10:")

    void add_k2(float *dst, float k, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            OP_K4_CORE("dst", "dst", "vadd", OP_DSEL)
            : [count] "+r" (count), [off] "=&r" (off),
              [k] "+Yz" (k)
            : [dst] "r" (dst)
            : "cc", "memory",
              "%xmm1",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void sub_k2(float *dst, float k, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            OP_K4_CORE("dst", "dst", "vsub", OP_DSEL)
            : [count] "+r" (count), [off] "=&r" (off),
              [k] "+Yz" (k)
            : [dst] "r" (dst)
            : "cc", "memory",
              "%xmm1",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void rsub_k2(float *dst, float k, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            OP_K4_CORE("dst", "dst", "vsub", OP_RSEL)
            : [count] "+r" (count), [off] "=&r" (off),
              [k] "+Yz" (k)
            : [dst] "r" (dst)
            : "cc", "memory",
              "%xmm1",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void mul_k2(float *dst, float k, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            OP_K4_CORE("dst", "dst", "vmul", OP_DSEL)
            : [count] "+r" (count), [off] "=&r" (off),
              [k] "+Yz" (k)
            : [dst] "r" (dst)
            : "cc", "memory",
              "%xmm1",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void div_k2(float *dst, float k, size_t count)
    {
        mul_k2(dst, 1.0f/k, count);
    }

    void rdiv_k2(float *dst, float k, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            OP_K4_CORE("dst", "dst", "vdiv", OP_DSEL)
            : [count] "+r" (count), [off] "=&r" (off),
              [k] "+Yz" (k)
            : [dst] "r" (dst)
            : "cc", "memory",
              "%xmm1",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void add_k3(float *dst, const float *src, float k, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            OP_K4_CORE("dst", "src", "vadd", OP_DSEL)
            : [count] "+r" (count), [off] "=&r" (off),
              [k] "+Yz" (k)
            : [dst] "r" (dst), [src] "r" (src)
            : "cc", "memory",
              "%xmm1",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void sub_k3(float *dst, const float *src, float k, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            OP_K4_CORE("dst", "src", "vsub", OP_DSEL)
            : [count] "+r" (count), [off] "=&r" (off),
              [k] "+Yz" (k)
            : [dst] "r" (dst), [src] "r" (src)
            : "cc", "memory",
              "%xmm1",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void rsub_k3(float *dst, const float *src, float k, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            OP_K4_CORE("dst", "src", "vsub", OP_RSEL)
            : [count] "+r" (count), [off] "=&r" (off),
              [k] "+Yz" (k)
            : [dst] "r" (dst), [src] "r" (src)
            : "cc", "memory",
              "%xmm1",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void mul_k3(float *dst, const float *src, float k, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            OP_K4_CORE("dst", "src", "vmul", OP_DSEL)
            : [count] "+r" (count), [off] "=&r" (off),
              [k] "+Yz" (k)
            : [dst] "r" (dst), [src] "r" (src)
            : "cc", "memory",
              "%xmm1",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void div_k3(float *dst, const float *src, float k, size_t count)
    {
        mul_k3(dst, src, 1.0f/k, count);
    }

    void rdiv_k3(float *dst, const float *src, float k, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            OP_K4_CORE("dst", "src", "vdiv", OP_DSEL)
            : [count] "+r" (count), [off] "=&r" (off),
              [k] "+Yz" (k)
            : [dst] "r" (dst), [src] "r" (src)
            : "cc", "memory",
              "%xmm1",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

#undef OP_K4_CORE
#undef OP_DSEL
#undef OP_RSEL
}

#endif /* DSP_ARCH_X86_AVX2_PMATH_OP_KX_H_ */
