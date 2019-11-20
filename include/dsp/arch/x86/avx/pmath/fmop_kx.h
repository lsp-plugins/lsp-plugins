/*
 * fmop_k3.h
 *
 *  Created on: 21 нояб. 2019 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_AVX_PMATH_FMOP_KX_H_
#define DSP_ARCH_X86_AVX_PMATH_FMOP_KX_H_

namespace avx
{
    #define OP_DSEL(a, b)   a
    #define OP_RSEL(a, b)   b

    #define FMADDSUB_K3_CORE(DST, SRC, OP) \
        __ASM_EMIT("xor         %[off], %[off]") \
        __ASM_EMIT("vbroadcastss %%xmm0, %%ymm0") \
        __ASM_EMIT("sub         $32, %[count]") \
        __ASM_EMIT("vmovaps     %%ymm0, %%ymm1") \
        __ASM_EMIT("jb          2f")    \
        /* 32x blocks */ \
        __ASM_EMIT("1:") \
        __ASM_EMIT("vmovups     0x000(%[" DST "], %[off]), %%ymm6") \
        __ASM_EMIT("vmovups     0x020(%[" DST "], %[off]), %%ymm7") \
        __ASM_EMIT("vmovups     0x040(%[" DST "], %[off]), %%ymm8") \
        __ASM_EMIT("vmovups     0x060(%[" DST "], %[off]), %%ymm9") \
        __ASM_EMIT(OP "ps       0x000(%[" SRC "], %[off]), %%ymm0, %%ymm6") \
        __ASM_EMIT(OP "ps       0x020(%[" SRC "], %[off]), %%ymm1, %%ymm7") \
        __ASM_EMIT(OP "ps       0x040(%[" SRC "], %[off]), %%ymm0, %%ymm8") \
        __ASM_EMIT(OP "ps       0x060(%[" SRC "], %[off]), %%ymm1, %%ymm9") \
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
        __ASM_EMIT("vmovups     0x000(%[" DST "], %[off]), %%ymm6") \
        __ASM_EMIT("vmovups     0x020(%[" DST "], %[off]), %%ymm7") \
        __ASM_EMIT(OP "ps       0x000(%[" SRC "], %[off]), %%ymm0, %%ymm6") \
        __ASM_EMIT(OP "ps       0x020(%[" SRC "], %[off]), %%ymm1, %%ymm7") \
        __ASM_EMIT("vmovups     %%ymm6, 0x000(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%ymm7, 0x020(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x40, %[off]") \
        __ASM_EMIT("sub         $16, %[count]") \
        /* 8x block */ \
        __ASM_EMIT("4:") \
        __ASM_EMIT("add         $8, %[count]")          /* 16 - 8 */ \
        __ASM_EMIT("jl          6f") \
        __ASM_EMIT("vmovups     0x000(%[" DST "], %[off]), %%ymm6") \
        __ASM_EMIT(OP "ps       0x000(%[" SRC "], %[off]), %%ymm0, %%ymm6") \
        __ASM_EMIT("vmovups     %%ymm6, 0x000(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x20, %[off]") \
        __ASM_EMIT("sub         $8, %[count]") \
        \
        /* 4x block */ \
        __ASM_EMIT("6:") \
        __ASM_EMIT("add         $4, %[count]")          /* 8 - 4 */ \
        __ASM_EMIT("jl          8f") \
        __ASM_EMIT("vmovups     0x000(%[" DST "], %[off]), %%xmm6") \
        __ASM_EMIT(OP "ps       0x000(%[" SRC "], %[off]), %%xmm0, %%xmm6") \
        __ASM_EMIT("vmovups     %%xmm6, 0x000(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x10, %[off]") \
        __ASM_EMIT("sub         $4, %[count]") \
        \
        /* 1x blocks */ \
        __ASM_EMIT("8:") \
        __ASM_EMIT("add         $3, %[count]")          /* 8 - 4 */ \
        __ASM_EMIT("jl          10f") \
        __ASM_EMIT("9:") \
        __ASM_EMIT("vmovss      0x000(%[" DST "], %[off]), %%xmm6") \
        __ASM_EMIT(OP "ss       0x000(%[" SRC "], %[off]), %%xmm0, %%xmm6") \
        __ASM_EMIT("vmovss      %%xmm6, 0x000(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x04, %[off]") \
        __ASM_EMIT("dec         %[count]") \
        __ASM_EMIT("jge         9b") \
        __ASM_EMIT("10:")

    void x64_fmadd_k3_fma3(float *dst, const float *src, float k, size_t count)
    {
        IF_ARCH_X86_64(size_t off);
        ARCH_X86_64_ASM
        (
            FMADDSUB_K3_CORE("dst", "src", "vfmadd231")
            : [off] "=&r" (off), [count] "+r" (count),
              [k] "+Yz"(k)
            : [dst] "r"(dst), [src] "r"(src)
            : "cc", "memory",
              "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7",
              "%xmm8", "%xmm9"
        );
    }

    void x64_fmsub_k3_fma3(float *dst, const float *src, float k, size_t count)
    {
        IF_ARCH_X86_64(size_t off);
        ARCH_X86_64_ASM
        (
            FMADDSUB_K3_CORE("dst", "src", "vfnmadd231")
            : [off] "=&r" (off), [count] "+r" (count),
              [k] "+Yz"(k)
            : [dst] "r"(dst), [src] "r"(src)
            : "cc", "memory",
              "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7",
              "%xmm8", "%xmm9"
        );
    }

    void x64_fmrsub_k3_fma3(float *dst, const float *src, float k, size_t count)
    {
        IF_ARCH_X86_64(size_t off);
        ARCH_X86_64_ASM
        (
            FMADDSUB_K3_CORE("dst", "src", "vfnmadd231")
            : [off] "=&r" (off), [count] "+r" (count),
              [k] "+Yz"(k)
            : [dst] "r"(dst), [src] "r"(src)
            : "cc", "memory",
              "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7",
              "%xmm8", "%xmm9"
        );
    }

    #undef FMADDSUB_K3_CORE

    #define FMOP_K3_CORE(DST, SRC, OP, SEL) \
        __ASM_EMIT("xor         %[off], %[off]") \
        __ASM_EMIT("vbroadcastss %%xmm0, %%ymm0") \
        __ASM_EMIT("sub         $32, %[count]") \
        __ASM_EMIT("vmovaps     %%ymm0, %%ymm1") \
        __ASM_EMIT("jb          2f")    \
        /* 32x blocks */ \
        __ASM_EMIT("1:") \
        __ASM_EMIT("vmovups     0x000(%[" DST "], %[off]), %%ymm6") \
        __ASM_EMIT("vmovups     0x020(%[" DST "], %[off]), %%ymm7") \
        __ASM_EMIT("vmovups     0x040(%[" DST "], %[off]), %%ymm8") \
        __ASM_EMIT("vmovups     0x060(%[" DST "], %[off]), %%ymm9") \
        __ASM_EMIT("vmulps      0x000(%[" SRC "], %[off]), %%ymm0, %%ymm2") \
        __ASM_EMIT("vmulps      0x020(%[" SRC "], %[off]), %%ymm1, %%ymm3") \
        __ASM_EMIT("vmulps      0x040(%[" SRC "], %[off]), %%ymm0, %%ymm4") \
        __ASM_EMIT("vmulps      0x060(%[" SRC "], %[off]), %%ymm1, %%ymm5") \
        __ASM_EMIT(OP "ps       " SEL("%%ymm2", "%%ymm6") ", "  SEL("%%ymm6", "%%ymm2") ", %%ymm6") \
        __ASM_EMIT(OP "ps       " SEL("%%ymm3", "%%ymm7") ", "  SEL("%%ymm7", "%%ymm3") ", %%ymm7") \
        __ASM_EMIT(OP "ps       " SEL("%%ymm4", "%%ymm8") ", "  SEL("%%ymm8", "%%ymm4") ", %%ymm8") \
        __ASM_EMIT(OP "ps       " SEL("%%ymm5", "%%ymm9") ", "  SEL("%%ymm9", "%%ymm5") ", %%ymm9") \
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
        __ASM_EMIT("vmovups     0x000(%[" DST "], %[off]), %%ymm6") \
        __ASM_EMIT("vmovups     0x020(%[" DST "], %[off]), %%ymm7") \
        __ASM_EMIT("vmulps      0x000(%[" SRC "], %[off]), %%ymm0, %%ymm2") \
        __ASM_EMIT("vmulps      0x020(%[" SRC "], %[off]), %%ymm1, %%ymm3") \
        __ASM_EMIT(OP "ps       " SEL("%%ymm2", "%%ymm6") ", "  SEL("%%ymm6", "%%ymm2") ", %%ymm6") \
        __ASM_EMIT(OP "ps       " SEL("%%ymm3", "%%ymm7") ", "  SEL("%%ymm7", "%%ymm3") ", %%ymm7") \
        __ASM_EMIT("vmovups     %%ymm6, 0x000(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%ymm7, 0x020(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x40, %[off]") \
        __ASM_EMIT("sub         $16, %[count]") \
        /* 8x block */ \
        __ASM_EMIT("4:") \
        __ASM_EMIT("add         $8, %[count]")          /* 16 - 8 */ \
        __ASM_EMIT("jl          6f") \
        __ASM_EMIT("vmovups     0x000(%[" DST "], %[off]), %%ymm6") \
        __ASM_EMIT("vmulps      0x000(%[" SRC "], %[off]), %%ymm0, %%ymm2") \
        __ASM_EMIT(OP "ps       " SEL("%%ymm2", "%%ymm6") ", "  SEL("%%ymm6", "%%ymm2") ", %%ymm6") \
        __ASM_EMIT("vmovups     %%ymm6, 0x000(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x20, %[off]") \
        __ASM_EMIT("sub         $8, %[count]") \
        \
        /* 4x block */ \
        __ASM_EMIT("6:") \
        __ASM_EMIT("add         $4, %[count]")          /* 8 - 4 */ \
        __ASM_EMIT("jl          8f") \
        __ASM_EMIT("vmovups     0x000(%[" DST "], %[off]), %%xmm6") \
        __ASM_EMIT("vmulps      0x000(%[" SRC "], %[off]), %%xmm0, %%xmm2") \
        __ASM_EMIT(OP "ps       " SEL("%%xmm2", "%%xmm6") ", "  SEL("%%xmm6", "%%xmm2") ", %%xmm6") \
        __ASM_EMIT("vmovups     %%xmm6, 0x000(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x10, %[off]") \
        __ASM_EMIT("sub         $4, %[count]") \
        \
        /* 1x blocks */ \
        __ASM_EMIT("8:") \
        __ASM_EMIT("add         $3, %[count]")          /* 8 - 4 */ \
        __ASM_EMIT("jl          10f") \
        __ASM_EMIT("9:") \
        __ASM_EMIT("vmovss      0x000(%[" DST "], %[off]), %%xmm6") \
        __ASM_EMIT("vmulss      0x000(%[" SRC "], %[off]), %%xmm0, %%xmm2") \
        __ASM_EMIT(OP "ss       " SEL("%%xmm2", "%%xmm6") ", "  SEL("%%xmm6", "%%xmm2") ", %%xmm6") \
        __ASM_EMIT("vmovss      %%xmm6, 0x000(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x04, %[off]") \
        __ASM_EMIT("dec         %[count]") \
        __ASM_EMIT("jge         9b") \
        __ASM_EMIT("10:")

    void x64_fmadd_k3(float *dst, const float *src, float k, size_t count)
    {
        IF_ARCH_X86_64(size_t off);
        ARCH_X86_64_ASM
        (
            FMOP_K3_CORE("dst", "src", "vadd", OP_DSEL)
            : [off] "=&r" (off), [count] "+r" (count),
              [k] "+Yz"(k)
            : [dst] "r"(dst), [src] "r"(src)
            : "cc", "memory",
              "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7",
              "%xmm8", "%xmm9"
        );
    }

    void x64_fmsub_k3(float *dst, const float *src, float k, size_t count)
    {
        IF_ARCH_X86_64(size_t off);
        ARCH_X86_64_ASM
        (
            FMOP_K3_CORE("dst", "src", "vsub", OP_DSEL)
            : [off] "=&r" (off), [count] "+r" (count),
              [k] "+Yz"(k)
            : [dst] "r"(dst), [src] "r"(src)
            : "cc", "memory",
              "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7",
              "%xmm8", "%xmm9"
        );
    }

    void x64_fmrsub_k3(float *dst, const float *src, float k, size_t count)
    {
        IF_ARCH_X86_64(size_t off);
        ARCH_X86_64_ASM
        (
            FMOP_K3_CORE("dst", "src", "vsub", OP_RSEL)
            : [off] "=&r" (off), [count] "+r" (count),
              [k] "+Yz"(k)
            : [dst] "r"(dst), [src] "r"(src)
            : "cc", "memory",
              "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7",
              "%xmm8", "%xmm9"
        );
    }

    void x64_fmmul_k3(float *dst, const float *src, float k, size_t count)
    {
        IF_ARCH_X86_64(size_t off);
        ARCH_X86_64_ASM
        (
            FMOP_K3_CORE("dst", "src", "vmul", OP_DSEL)
            : [off] "=&r" (off), [count] "+r" (count),
              [k] "+Yz"(k)
            : [dst] "r"(dst), [src] "r"(src)
            : "cc", "memory",
              "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7",
              "%xmm8", "%xmm9"
        );
    }

    void x64_fmdiv_k3(float *dst, const float *src, float k, size_t count)
    {
        IF_ARCH_X86_64(size_t off);
        ARCH_X86_64_ASM
        (
            FMOP_K3_CORE("dst", "src", "vdiv", OP_DSEL)
            : [off] "=&r" (off), [count] "+r" (count),
              [k] "+Yz"(k)
            : [dst] "r"(dst), [src] "r"(src)
            : "cc", "memory",
              "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7",
              "%xmm8", "%xmm9"
        );
    }

    void x64_fmrdiv_k3(float *dst, const float *src, float k, size_t count)
    {
        IF_ARCH_X86_64(size_t off);
        ARCH_X86_64_ASM
        (
            FMOP_K3_CORE("dst", "src", "vdiv", OP_RSEL)
            : [off] "=&r" (off), [count] "+r" (count),
              [k] "+Yz"(k)
            : [dst] "r"(dst), [src] "r"(src)
            : "cc", "memory",
              "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7",
              "%xmm8", "%xmm9"
        );
    }

    #undef FMOP_K3_CORE

    #undef OP_DSEL
    #undef OP_RSEL

}

#endif /* DSP_ARCH_X86_AVX_PMATH_FMOP_KX_H_ */
