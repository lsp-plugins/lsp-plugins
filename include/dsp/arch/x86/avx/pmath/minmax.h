/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 5 июл. 2020 г.
 *
 * lsp-plugins is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-plugins is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-plugins. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef DSP_ARCH_X86_AVX_PMATH_MINMAX_H_
#define DSP_ARCH_X86_AVX_PMATH_MINMAX_H_

#ifndef DSP_ARCH_X86_AVX_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_AVX_IMPL */

namespace avx
{

    #define MINMAX_CORE(DST, A, B, OP) \
        __ASM_EMIT("xor         %[off], %[off]") \
        __ASM_EMIT("sub         $32, %[count]") \
        __ASM_EMIT("jb          2f")    \
        /* 32x blocks */ \
        __ASM_EMIT("1:") \
        __ASM_EMIT("vmovups     0x00(%[" A "], %[off]), %%ymm0") \
        __ASM_EMIT("vmovups     0x20(%[" A "], %[off]), %%ymm1") \
        __ASM_EMIT("vmovups     0x40(%[" A "], %[off]), %%ymm2") \
        __ASM_EMIT("vmovups     0x60(%[" A "], %[off]), %%ymm3") \
        __ASM_EMIT(OP "         0x00(%[" B "], %[off]), %%ymm0, %%ymm0") \
        __ASM_EMIT(OP "         0x20(%[" B "], %[off]), %%ymm1, %%ymm1") \
        __ASM_EMIT(OP "         0x40(%[" B "], %[off]), %%ymm2, %%ymm2") \
        __ASM_EMIT(OP "         0x60(%[" B "], %[off]), %%ymm3, %%ymm3") \
        __ASM_EMIT("vmovups     %%ymm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%ymm1, 0x20(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%ymm2, 0x40(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%ymm3, 0x60(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x80, %[off]") \
        __ASM_EMIT("sub         $32, %[count]") \
        __ASM_EMIT("jae         1b") \
        /* 16x block */ \
        __ASM_EMIT("2:") \
        __ASM_EMIT("add         $16, %[count]") \
        __ASM_EMIT("jl          4f") \
        __ASM_EMIT("vmovups     0x00(%[" A "], %[off]), %%ymm0") \
        __ASM_EMIT("vmovups     0x20(%[" A "], %[off]), %%ymm1") \
        __ASM_EMIT(OP "         0x00(%[" B "], %[off]), %%ymm0, %%ymm0") \
        __ASM_EMIT(OP "         0x20(%[" B "], %[off]), %%ymm1, %%ymm1") \
        __ASM_EMIT("vmovups     %%ymm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%ymm1, 0x20(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x40, %[off]") \
        __ASM_EMIT("sub         $16, %[count]") \
        /* 8x block */ \
        __ASM_EMIT("4:") \
        __ASM_EMIT("add         $8, %[count]") \
        __ASM_EMIT("jl          6f") \
        __ASM_EMIT("vmovups     0x00(%[" A "], %[off]), %%xmm0") \
        __ASM_EMIT("vmovups     0x10(%[" A "], %[off]), %%xmm1") \
        __ASM_EMIT(OP "         0x00(%[" B "], %[off]), %%xmm0, %%xmm0") \
        __ASM_EMIT(OP "         0x10(%[" B "], %[off]), %%xmm1, %%xmm1") \
        __ASM_EMIT("vmovups     %%xmm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%xmm1, 0x10(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x20, %[off]") \
        __ASM_EMIT("sub         $8, %[count]") \
        /* 4x block */ \
        __ASM_EMIT("6:") \
        __ASM_EMIT("add         $4, %[count]") \
        __ASM_EMIT("jl          8f") \
        __ASM_EMIT("vmovups     0x00(%[" A "], %[off]), %%xmm0") \
        __ASM_EMIT(OP "         0x00(%[" B "], %[off]), %%xmm0, %%xmm0") \
        __ASM_EMIT("vmovups     %%xmm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x10, %[off]") \
        __ASM_EMIT("sub         $4, %[count]") \
        /* 1x blocks */ \
        __ASM_EMIT("8:") \
        __ASM_EMIT("add         $3, %[count]") \
        __ASM_EMIT("jl          10f")    \
        __ASM_EMIT("9:") \
        __ASM_EMIT("vmovss      0x00(%[" A "], %[off]), %%xmm0") \
        __ASM_EMIT("vmovss      0x00(%[" B "], %[off]), %%xmm1") \
        __ASM_EMIT(OP "         %%xmm1, %%xmm0, %%xmm0") \
        __ASM_EMIT("vmovss      %%xmm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x04, %[off]") \
        __ASM_EMIT("dec         %[count]") \
        __ASM_EMIT("jge         9b") \
        __ASM_EMIT("10:")

    void pmin2(float *dst, const float *src, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            MINMAX_CORE("dst", "dst", "src", "vminps")
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
            MINMAX_CORE("dst", "a", "b", "vminps")
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
            MINMAX_CORE("dst", "dst", "src", "vmaxps")
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
            MINMAX_CORE("dst", "a", "b", "vmaxps")
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r" (dst), [a] "r" (a), [b] "r" (b)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    #undef MINMAX_CORE

    #define SIGN_MINMAX_CORE(DST, A, B, CMP) \
        __ASM_EMIT("vmovaps     %[X_SIGN], %%ymm6") \
        __ASM_EMIT("xor         %[off], %[off]") \
        __ASM_EMIT("vmovaps     %%ymm6, %%ymm7") \
        __ASM_EMIT("sub         $8, %[count]") \
        __ASM_EMIT("jb          2f")    \
        /* 8x blocks */ \
        __ASM_EMIT("1:") \
        __ASM_EMIT("vmovups     0x00(%[" A "], %[off]), %%ymm0") \
        __ASM_EMIT("vmovups     0x00(%[" B "], %[off]), %%ymm1") \
        __ASM_EMIT("vandps      %%ymm6, %%ymm0, %%ymm2") \
        __ASM_EMIT("vandps      %%ymm7, %%ymm1, %%ymm3") \
        __ASM_EMIT("vcmpps      " CMP ", %%ymm3, %%ymm2, %%ymm2") \
        __ASM_EMIT("vblendvps   %%ymm2, %%ymm0, %%ymm1, %%ymm0") \
        __ASM_EMIT("vmovups     %%ymm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x20, %[off]") \
        __ASM_EMIT("sub         $8, %[count]") \
        __ASM_EMIT("jae         1b") \
        /* 4x block */ \
        __ASM_EMIT("2:") \
        __ASM_EMIT("add         $4, %[count]") \
        __ASM_EMIT("jl          4f")    \
        __ASM_EMIT("vmovups     0x00(%[" A "], %[off]), %%xmm0") \
        __ASM_EMIT("vmovups     0x00(%[" B "], %[off]), %%xmm1") \
        __ASM_EMIT("vandps      %%xmm6, %%xmm0, %%xmm2") \
        __ASM_EMIT("vandps      %%xmm7, %%xmm1, %%xmm3") \
        __ASM_EMIT("vcmpps      " CMP ", %%xmm3, %%xmm2, %%xmm2") \
        __ASM_EMIT("vblendvps   %%xmm2, %%xmm0, %%xmm1, %%xmm0") \
        __ASM_EMIT("vmovups     %%xmm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x10, %[off]") \
        __ASM_EMIT("sub         $4, %[count]") \
        /* 1x blocks */ \
        __ASM_EMIT("4:") \
        __ASM_EMIT("add         $3, %[count]") \
        __ASM_EMIT("jl          6f")    \
        __ASM_EMIT("5:") \
        __ASM_EMIT("vmovss      0x00(%[" A "], %[off]), %%xmm0") \
        __ASM_EMIT("vmovss      0x00(%[" B "], %[off]), %%xmm1") \
        __ASM_EMIT("vandps      %%xmm6, %%xmm0, %%xmm2") \
        __ASM_EMIT("vandps      %%xmm7, %%xmm1, %%xmm3") \
        __ASM_EMIT("vcmpps      " CMP ", %%xmm3, %%xmm2, %%xmm2") \
        __ASM_EMIT("vblendvps   %%xmm2, %%xmm0, %%xmm1, %%xmm0") \
        __ASM_EMIT("vmovss      %%xmm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x04, %[off]") \
        __ASM_EMIT("dec         %[count]") \
        __ASM_EMIT("jge         5b") \
        __ASM_EMIT("6:")

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
        __ASM_EMIT("vmovaps     %[X_SIGN], %%ymm6") \
        __ASM_EMIT("xor         %[off], %[off]") \
        __ASM_EMIT("vmovaps     %%ymm6, %%ymm7") \
        __ASM_EMIT("sub         $24, %[count]") \
        __ASM_EMIT("jb          2f")    \
        /* 24x blocks */ \
        __ASM_EMIT("1:") \
        __ASM_EMIT("vandps      0x00(%[" A "], %[off]), %%ymm6, %%ymm0") \
        __ASM_EMIT("vandps      0x20(%[" A "], %[off]), %%ymm7, %%ymm1") \
        __ASM_EMIT("vandps      0x40(%[" A "], %[off]), %%ymm6, %%ymm2") \
        __ASM_EMIT("vandps      0x00(%[" B "], %[off]), %%ymm7, %%ymm3") \
        __ASM_EMIT("vandps      0x20(%[" B "], %[off]), %%ymm6, %%ymm4") \
        __ASM_EMIT("vandps      0x40(%[" B "], %[off]), %%ymm7, %%ymm5") \
        __ASM_EMIT(OP "         %%ymm3, %%ymm0, %%ymm0") \
        __ASM_EMIT(OP "         %%ymm4, %%ymm1, %%ymm1") \
        __ASM_EMIT(OP "         %%ymm5, %%ymm2, %%ymm2") \
        __ASM_EMIT("vmovups     %%ymm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%ymm1, 0x20(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%ymm2, 0x40(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x60, %[off]") \
        __ASM_EMIT("sub         $24, %[count]") \
        __ASM_EMIT("jae         1b") \
        /* 16x block */ \
        __ASM_EMIT("2:") \
        __ASM_EMIT("add         $8, %[count]") \
        __ASM_EMIT("jl          4f")    \
        __ASM_EMIT("vandps      0x00(%[" A "], %[off]), %%ymm6, %%ymm0") \
        __ASM_EMIT("vandps      0x20(%[" A "], %[off]), %%ymm7, %%ymm1") \
        __ASM_EMIT("vandps      0x00(%[" B "], %[off]), %%ymm6, %%ymm3") \
        __ASM_EMIT("vandps      0x20(%[" B "], %[off]), %%ymm7, %%ymm4") \
        __ASM_EMIT(OP "         %%ymm3, %%ymm0, %%ymm0") \
        __ASM_EMIT(OP "         %%ymm4, %%ymm1, %%ymm1") \
        __ASM_EMIT("vmovups     %%ymm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%ymm1, 0x20(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x40, %[off]") \
        __ASM_EMIT("sub         $16, %[count]") \
        /* 8x block */ \
        __ASM_EMIT("4:") \
        __ASM_EMIT("add         $8, %[count]") \
        __ASM_EMIT("jl          6f")    \
        __ASM_EMIT("vandps      0x00(%[" A "], %[off]), %%xmm6, %%xmm0") \
        __ASM_EMIT("vandps      0x10(%[" A "], %[off]), %%xmm7, %%xmm1") \
        __ASM_EMIT("vandps      0x00(%[" B "], %[off]), %%xmm6, %%xmm3") \
        __ASM_EMIT("vandps      0x10(%[" B "], %[off]), %%xmm7, %%xmm4") \
        __ASM_EMIT(OP "         %%xmm3, %%xmm0, %%xmm0") \
        __ASM_EMIT(OP "         %%xmm4, %%xmm1, %%xmm1") \
        __ASM_EMIT("vmovups     %%xmm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%xmm1, 0x10(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x20, %[off]") \
        __ASM_EMIT("sub         $8, %[count]") \
        /* 4x block */ \
        __ASM_EMIT("6:") \
        __ASM_EMIT("add         $4, %[count]") \
        __ASM_EMIT("jl          8f")    \
        __ASM_EMIT("vandps      0x00(%[" A "], %[off]), %%xmm6, %%xmm0") \
        __ASM_EMIT("vandps      0x00(%[" B "], %[off]), %%xmm7, %%xmm3") \
        __ASM_EMIT(OP "         %%xmm3, %%xmm0, %%xmm0") \
        __ASM_EMIT("vmovups     %%xmm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x10, %[off]") \
        __ASM_EMIT("sub         $4, %[count]") \
        /* 1x blocks */ \
        __ASM_EMIT("8:") \
        __ASM_EMIT("add         $3, %[count]") \
        __ASM_EMIT("jl          10f")    \
        __ASM_EMIT("9:") \
        __ASM_EMIT("vmovss      0x00(%[" A "], %[off]), %%xmm0") \
        __ASM_EMIT("vmovss      0x00(%[" B "], %[off]), %%xmm3") \
        __ASM_EMIT("vandps      %%xmm0, %%xmm6, %%xmm0") \
        __ASM_EMIT("vandps      %%xmm3, %%xmm7, %%xmm3") \
        __ASM_EMIT(OP "         %%xmm3, %%xmm0, %%xmm0") \
        __ASM_EMIT("vmovss      %%xmm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x04, %[off]") \
        __ASM_EMIT("dec         %[count]") \
        __ASM_EMIT("jge         9b") \
        __ASM_EMIT("10:")

    void pamin2(float *dst, const float *src, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            ABS_MINMAX_CORE("dst", "dst", "src", "vminps")
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
            ABS_MINMAX_CORE("dst", "a", "b", "vminps")
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
            ABS_MINMAX_CORE("dst", "dst", "src", "vmaxps")
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
            ABS_MINMAX_CORE("dst", "a", "b", "vmaxps")
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

#endif /* DSP_ARCH_X86_AVX_PMATH_MINMAX_H_ */
