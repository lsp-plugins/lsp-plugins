/*
 * op_kx.h
 *
 *  Created on: 5 янв. 2020 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_SSE2_PMATH_OP_KX_H_
#define DSP_ARCH_X86_SSE2_PMATH_OP_KX_H_

#ifndef DSP_ARCH_X86_SSE2_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_SSE2_IMPL */

namespace sse2
{
    #define FMOD_KX_CORE(DST, SRC) \
        __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0") \
        __ASM_EMIT("xor         %[off], %[off]") \
        __ASM_EMIT("movaps      %%xmm0, %%xmm1") \
        /* 8x blocks */ \
        __ASM_EMIT("sub         $8, %[count]") \
        __ASM_EMIT("jb          2f") \
        __ASM_EMIT("1:") \
        __ASM_EMIT("movups      0x00(%[" SRC "], %[off]), %%xmm2") \
        __ASM_EMIT("movups      0x10(%[" SRC "], %[off]), %%xmm3") \
        __ASM_EMIT("movaps      %%xmm2, %%xmm6") \
        __ASM_EMIT("movaps      %%xmm3, %%xmm7") \
        __ASM_EMIT("divps       %%xmm0, %%xmm6") \
        __ASM_EMIT("divps       %%xmm1, %%xmm7") \
        __ASM_EMIT("cvttps2dq   %%xmm6, %%xmm6") \
        __ASM_EMIT("cvttps2dq   %%xmm7, %%xmm7") \
        __ASM_EMIT("cvtdq2ps    %%xmm6, %%xmm6") \
        __ASM_EMIT("cvtdq2ps    %%xmm7, %%xmm7") \
        __ASM_EMIT("mulps       %%xmm0, %%xmm6") \
        __ASM_EMIT("mulps       %%xmm1, %%xmm7") \
        __ASM_EMIT("subps       %%xmm6, %%xmm2") \
        __ASM_EMIT("subps       %%xmm7, %%xmm3") \
        __ASM_EMIT("movups      %%xmm2, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("movups      %%xmm3, 0x10(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x20, %[off]") \
        __ASM_EMIT("sub         $8, %[count]") \
        __ASM_EMIT("jae         1b") \
        __ASM_EMIT("2:") \
        /* 4x block */ \
        __ASM_EMIT("add         $4, %[count]") \
        __ASM_EMIT("jl          4f") \
        __ASM_EMIT("movups      0x00(%[" SRC "], %[off]), %%xmm2") \
        __ASM_EMIT("movaps      %%xmm2, %%xmm6") \
        __ASM_EMIT("divps       %%xmm0, %%xmm6") \
        __ASM_EMIT("cvttps2dq   %%xmm6, %%xmm6") \
        __ASM_EMIT("cvtdq2ps    %%xmm6, %%xmm6") \
        __ASM_EMIT("mulps       %%xmm0, %%xmm6") \
        __ASM_EMIT("subps       %%xmm6, %%xmm2") \
        __ASM_EMIT("movups      %%xmm2, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x10, %[off]") \
        __ASM_EMIT("sub         $4, %[count]") \
        __ASM_EMIT("4:") \
        /* 1x blocks */ \
        __ASM_EMIT("add         $3, %[count]") \
        __ASM_EMIT("jl          6f") \
        __ASM_EMIT("5:") \
        __ASM_EMIT("movss       0x00(%[" SRC "], %[off]), %%xmm2") \
        __ASM_EMIT("movaps      %%xmm2, %%xmm6") \
        __ASM_EMIT("divps       %%xmm0, %%xmm6") \
        __ASM_EMIT("cvttps2dq   %%xmm6, %%xmm6") \
        __ASM_EMIT("cvtdq2ps    %%xmm6, %%xmm6") \
        __ASM_EMIT("mulps       %%xmm0, %%xmm6") \
        __ASM_EMIT("subps       %%xmm6, %%xmm2") \
        __ASM_EMIT("movss       %%xmm2, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x04, %[off]") \
        __ASM_EMIT("dec         %[count]") \
        __ASM_EMIT("jge         5b") \
        __ASM_EMIT("6:")

    void mod_k2(float *dst, float k, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            FMOD_KX_CORE("dst", "dst")
            : [count] "+r" (count), [off] "=&r" (off),
              [k] "+Yz"(k)
            : [dst] "r" (dst)
            : "cc", "memory",
              "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void mod_k3(float *dst, const float *src, float k, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            FMOD_KX_CORE("dst", "src")
            : [count] "+r" (count), [off] "=&r" (off),
              [k] "+Yz"(k)
            : [dst] "r" (dst), [src] "r" (src)
            : "cc", "memory",
              "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    #undef FMOD_KX_CORE

    #define FRMOD_KX_CORE(DST, SRC) \
        __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0") \
        __ASM_EMIT("xor         %[off], %[off]") \
        __ASM_EMIT("movaps      %%xmm0, %%xmm1") \
        /* 8x blocks */ \
        __ASM_EMIT("sub         $8, %[count]") \
        __ASM_EMIT("jb          2f") \
        __ASM_EMIT("1:") \
        __ASM_EMIT("movups      0x00(%[" SRC "], %[off]), %%xmm2") \
        __ASM_EMIT("movups      0x10(%[" SRC "], %[off]), %%xmm3") \
        __ASM_EMIT("movaps      %%xmm0, %%xmm4") \
        __ASM_EMIT("movaps      %%xmm1, %%xmm5") \
        __ASM_EMIT("movaps      %%xmm0, %%xmm6") \
        __ASM_EMIT("movaps      %%xmm1, %%xmm7") \
        __ASM_EMIT("divps       %%xmm2, %%xmm6") \
        __ASM_EMIT("divps       %%xmm3, %%xmm7") \
        __ASM_EMIT("cvttps2dq   %%xmm6, %%xmm6") \
        __ASM_EMIT("cvttps2dq   %%xmm7, %%xmm7") \
        __ASM_EMIT("cvtdq2ps    %%xmm6, %%xmm6") \
        __ASM_EMIT("cvtdq2ps    %%xmm7, %%xmm7") \
        __ASM_EMIT("mulps       %%xmm2, %%xmm6") \
        __ASM_EMIT("mulps       %%xmm3, %%xmm7") \
        __ASM_EMIT("subps       %%xmm6, %%xmm4") \
        __ASM_EMIT("subps       %%xmm7, %%xmm5") \
        __ASM_EMIT("movups      %%xmm4, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("movups      %%xmm5, 0x10(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x20, %[off]") \
        __ASM_EMIT("sub         $8, %[count]") \
        __ASM_EMIT("jae         1b") \
        __ASM_EMIT("2:") \
        /* 4x block */ \
        __ASM_EMIT("add         $4, %[count]") \
        __ASM_EMIT("jl          4f") \
        __ASM_EMIT("movups      0x00(%[" SRC "], %[off]), %%xmm2") \
        __ASM_EMIT("movaps      %%xmm0, %%xmm4") \
        __ASM_EMIT("movaps      %%xmm0, %%xmm6") \
        __ASM_EMIT("divps       %%xmm2, %%xmm6") \
        __ASM_EMIT("cvttps2dq   %%xmm6, %%xmm6") \
        __ASM_EMIT("cvtdq2ps    %%xmm6, %%xmm6") \
        __ASM_EMIT("mulps       %%xmm2, %%xmm6") \
        __ASM_EMIT("subps       %%xmm6, %%xmm4") \
        __ASM_EMIT("movups      %%xmm4, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x10, %[off]") \
        __ASM_EMIT("sub         $4, %[count]") \
        __ASM_EMIT("4:") \
        /* 1x blocks */ \
        __ASM_EMIT("add         $3, %[count]") \
        __ASM_EMIT("jl          6f") \
        __ASM_EMIT("5:") \
        __ASM_EMIT("movss       0x00(%[" SRC "], %[off]), %%xmm2") \
        __ASM_EMIT("movaps      %%xmm0, %%xmm4") \
        __ASM_EMIT("movaps      %%xmm0, %%xmm6") \
        __ASM_EMIT("divps       %%xmm2, %%xmm6") \
        __ASM_EMIT("cvttps2dq   %%xmm6, %%xmm6") \
        __ASM_EMIT("cvtdq2ps    %%xmm6, %%xmm6") \
        __ASM_EMIT("mulps       %%xmm2, %%xmm6") \
        __ASM_EMIT("subps       %%xmm6, %%xmm4") \
        __ASM_EMIT("movss       %%xmm4, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x04, %[off]") \
        __ASM_EMIT("dec         %[count]") \
        __ASM_EMIT("jge         5b") \
        __ASM_EMIT("6:")

    void rmod_k2(float *dst, float k, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            FRMOD_KX_CORE("dst", "dst")
            : [count] "+r" (count), [off] "=&r" (off),
              [k] "+Yz"(k)
            : [dst] "r" (dst)
            : "cc", "memory",
              "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void rmod_k3(float *dst, const float *src, float k, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            FRMOD_KX_CORE("dst", "src")
            : [count] "+r" (count), [off] "=&r" (off),
              [k] "+Yz"(k)
            : [dst] "r" (dst), [src] "r" (src)
            : "cc", "memory",
              "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    #undef FRMOD_KX_CORE
}

#endif /* DSP_ARCH_X86_SSE2_PMATH_OP_KX_H_ */
