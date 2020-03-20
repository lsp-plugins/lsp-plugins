/*
 * op_vv.h
 *
 *  Created on: 7 янв. 2020 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_SSE2_PMATH_OP_VV_H_
#define DSP_ARCH_X86_SSE2_PMATH_OP_VV_H_

#ifndef DSP_ARCH_X86_SSE2_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_SSE2_IMPL */

namespace sse2
{
    #define FMOD_VV_CORE(DST, SRC1, SRC2) \
        __ASM_EMIT("xor         %[off], %[off]") \
        /* 8x blocks */ \
        __ASM_EMIT("sub         $8, %[count]") \
        __ASM_EMIT("jb          2f") \
        __ASM_EMIT("1:") \
        __ASM_EMIT("movups      0x00(%[" SRC1 "], %[off]), %%xmm2") \
        __ASM_EMIT("movups      0x10(%[" SRC1 "], %[off]), %%xmm3") \
        __ASM_EMIT("movups      0x00(%[" SRC2 "], %[off]), %%xmm0") \
        __ASM_EMIT("movups      0x10(%[" SRC2 "], %[off]), %%xmm1") \
        __ASM_EMIT("movaps      %%xmm2, %%xmm4") \
        __ASM_EMIT("movaps      %%xmm3, %%xmm5") \
        __ASM_EMIT("divps       %%xmm0, %%xmm4") \
        __ASM_EMIT("divps       %%xmm1, %%xmm5") \
        __ASM_EMIT("cvttps2dq   %%xmm4, %%xmm4") \
        __ASM_EMIT("cvttps2dq   %%xmm5, %%xmm5") \
        __ASM_EMIT("cvtdq2ps    %%xmm4, %%xmm4") \
        __ASM_EMIT("cvtdq2ps    %%xmm5, %%xmm5") \
        __ASM_EMIT("mulps       %%xmm0, %%xmm4") \
        __ASM_EMIT("mulps       %%xmm1, %%xmm5") \
        __ASM_EMIT("subps       %%xmm4, %%xmm2") \
        __ASM_EMIT("subps       %%xmm5, %%xmm3") \
        __ASM_EMIT("movups      %%xmm2, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("movups      %%xmm3, 0x10(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x20, %[off]") \
        __ASM_EMIT("sub         $8, %[count]") \
        __ASM_EMIT("jae         1b") \
        __ASM_EMIT("2:") \
        /* 4x block */ \
        __ASM_EMIT("add         $4, %[count]") \
        __ASM_EMIT("jl          4f") \
        __ASM_EMIT("movups      0x00(%[" SRC1 "], %[off]), %%xmm2") \
        __ASM_EMIT("movups      0x00(%[" SRC2 "], %[off]), %%xmm0") \
        __ASM_EMIT("movaps      %%xmm2, %%xmm4") \
        __ASM_EMIT("divps       %%xmm0, %%xmm4") \
        __ASM_EMIT("cvttps2dq   %%xmm4, %%xmm4") \
        __ASM_EMIT("cvtdq2ps    %%xmm4, %%xmm4") \
        __ASM_EMIT("mulps       %%xmm0, %%xmm4") \
        __ASM_EMIT("subps       %%xmm4, %%xmm2") \
        __ASM_EMIT("movups      %%xmm2, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x10, %[off]") \
        __ASM_EMIT("sub         $4, %[count]") \
        __ASM_EMIT("4:") \
        /* 1x blocks */ \
        __ASM_EMIT("add         $3, %[count]") \
        __ASM_EMIT("jl          6f") \
        __ASM_EMIT("5:") \
        __ASM_EMIT("movss       0x00(%[" SRC1 "], %[off]), %%xmm2") \
        __ASM_EMIT("movss       0x00(%[" SRC2 "], %[off]), %%xmm0") \
        __ASM_EMIT("movaps      %%xmm2, %%xmm4") \
        __ASM_EMIT("divps       %%xmm0, %%xmm4") \
        __ASM_EMIT("cvttps2dq   %%xmm4, %%xmm4") \
        __ASM_EMIT("cvtdq2ps    %%xmm4, %%xmm4") \
        __ASM_EMIT("mulps       %%xmm0, %%xmm4") \
        __ASM_EMIT("subps       %%xmm4, %%xmm2") \
        __ASM_EMIT("movss       %%xmm2, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x04, %[off]") \
        __ASM_EMIT("dec         %[count]") \
        __ASM_EMIT("jge         5b") \
        __ASM_EMIT("6:")

    void mod2(float *dst, const float *src, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            FMOD_VV_CORE("dst", "dst", "src")
            : [count] "+r" (count), [off] "=&r" (off)
            : [dst] "r" (dst), [src] "r" (src)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5"
        );
    }

    void rmod2(float *dst, const float *src, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            FMOD_VV_CORE("dst", "src", "dst")
            : [count] "+r" (count), [off] "=&r" (off)
            : [dst] "r" (dst), [src] "r" (src)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5"
        );
    }

    void mod3(float *dst, const float *src1, const float *src2, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            FMOD_VV_CORE("dst", "src1", "src2")
            : [count] "+r" (count), [off] "=&r" (off)
            : [dst] "r" (dst), [src1] "r" (src1), [src2] "r" (src2)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5"
        );
    }

    #undef FMOD_VV_CORE
}

#endif /* DSP_ARCH_X86_SSE2_PMATH_OP_VV_H_ */
