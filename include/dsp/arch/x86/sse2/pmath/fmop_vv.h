/*
 * fmop_vv.h
 *
 *  Created on: 9 янв. 2020 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_SSE2_PMATH_FMOP_VV_H_
#define DSP_ARCH_X86_SSE2_PMATH_FMOP_VV_H_

namespace sse2
{
    #define SEL_DIR(a, b)       a
    #define SEL_REV(a, b)       b

    #define FMOP_MOD_VV_CORE(DST, SRC1, SRC2, SRC3, SEL) \
        __ASM_EMIT("xor         %[off], %[off]") \
        /* 8x blocks */ \
        __ASM_EMIT32("subl      $8, %[count]") \
        __ASM_EMIT64("sub       $8, %[count]") \
        __ASM_EMIT("jb          2f") \
        __ASM_EMIT("1:") \
        __ASM_EMIT("movups      0x00(%[" SRC3 "], %[off]), %%xmm0") \
        __ASM_EMIT("movups      0x10(%[" SRC3 "], %[off]), %%xmm1") \
        __ASM_EMIT("movups      0x00(%[" SEL(SRC2, SRC1) "], %[off]), %%xmm4") \
        __ASM_EMIT("movups      0x10(%[" SEL(SRC2, SRC1) "], %[off]), %%xmm5") \
        __ASM_EMIT("movups      0x00(%[" SEL(SRC1, SRC2) "], %[off]), %%xmm2") \
        __ASM_EMIT("movups      0x10(%[" SEL(SRC1, SRC2) "], %[off]), %%xmm3") \
        __ASM_EMIT("mulps       %%xmm0, " SEL("%%xmm4", "%%xmm2")) \
        __ASM_EMIT("mulps       %%xmm1, " SEL("%%xmm5", "%%xmm3")) \
        __ASM_EMIT("movaps      %%xmm2, %%xmm6") \
        __ASM_EMIT("movaps      %%xmm3, %%xmm7") \
        __ASM_EMIT("divps       %%xmm4, %%xmm6") \
        __ASM_EMIT("divps       %%xmm5, %%xmm7") \
        __ASM_EMIT("cvttps2dq   %%xmm6, %%xmm6") \
        __ASM_EMIT("cvttps2dq   %%xmm7, %%xmm7") \
        __ASM_EMIT("cvtdq2ps    %%xmm6, %%xmm6") \
        __ASM_EMIT("cvtdq2ps    %%xmm7, %%xmm7") \
        __ASM_EMIT("mulps       %%xmm4, %%xmm6") \
        __ASM_EMIT("mulps       %%xmm5, %%xmm7") \
        __ASM_EMIT("subps       %%xmm6, %%xmm2") \
        __ASM_EMIT("subps       %%xmm7, %%xmm3") \
        __ASM_EMIT("movups      %%xmm2, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("movups      %%xmm3, 0x10(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x20, %[off]") \
        __ASM_EMIT32("subl      $8, %[count]") \
        __ASM_EMIT64("sub       $8, %[count]") \
        __ASM_EMIT("jae         1b") \
        __ASM_EMIT("2:") \
        /* 4x block */ \
        __ASM_EMIT32("addl      $4, %[count]") \
        __ASM_EMIT64("add       $4, %[count]") \
        __ASM_EMIT("jl          4f") \
        __ASM_EMIT("movups      0x00(%[" SRC3 "], %[off]), %%xmm0") \
        __ASM_EMIT("movups      0x00(%[" SEL(SRC2, SRC1) "], %[off]), %%xmm4") \
        __ASM_EMIT("movups      0x00(%[" SEL(SRC1, SRC2) "], %[off]), %%xmm2") \
        __ASM_EMIT("mulps       %%xmm0, " SEL("%%xmm4", "%%xmm2")) \
        __ASM_EMIT("movaps      %%xmm2, %%xmm6") \
        __ASM_EMIT("divps       %%xmm4, %%xmm6") \
        __ASM_EMIT("cvttps2dq   %%xmm6, %%xmm6") \
        __ASM_EMIT("cvtdq2ps    %%xmm6, %%xmm6") \
        __ASM_EMIT("mulps       %%xmm4, %%xmm6") \
        __ASM_EMIT("subps       %%xmm6, %%xmm2") \
        __ASM_EMIT("movups      %%xmm2, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x10, %[off]") \
        __ASM_EMIT32("subl      $4, %[count]") \
        __ASM_EMIT64("sub       $4, %[count]") \
        __ASM_EMIT("4:") \
        /* 1x blocks */ \
        __ASM_EMIT32("addl      $3, %[count]") \
        __ASM_EMIT64("add       $3, %[count]") \
        __ASM_EMIT("jl          6f") \
        __ASM_EMIT("5:") \
        __ASM_EMIT("movss       0x00(%[" SRC3 "], %[off]), %%xmm0") \
        __ASM_EMIT("movss       0x00(%[" SEL(SRC2, SRC1) "], %[off]), %%xmm4") \
        __ASM_EMIT("movss       0x00(%[" SEL(SRC1, SRC2) "], %[off]), %%xmm2") \
        __ASM_EMIT("mulps       %%xmm0, " SEL("%%xmm4", "%%xmm2")) \
        __ASM_EMIT("movaps      %%xmm2, %%xmm6") \
        __ASM_EMIT("divps       %%xmm4, %%xmm6") \
        __ASM_EMIT("cvttps2dq   %%xmm6, %%xmm6") \
        __ASM_EMIT("cvtdq2ps    %%xmm6, %%xmm6") \
        __ASM_EMIT("mulps       %%xmm4, %%xmm6") \
        __ASM_EMIT("subps       %%xmm6, %%xmm2") \
        __ASM_EMIT("movss       %%xmm2, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x04, %[off]") \
        __ASM_EMIT32("decl      %[count]") \
        __ASM_EMIT64("dec       %[count]") \
        __ASM_EMIT("jge         5b") \
        __ASM_EMIT("6:")

    void fmmod3(float *dst, const float *a, const float *b, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            FMOP_MOD_VV_CORE("dst", "dst", "a", "b", SEL_DIR)
            : [count] "+r" (count), [off] "=&r" (off)
            : [dst] "r" (dst), [a] "r" (a), [b] "r" (b)
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
            FMOP_MOD_VV_CORE("dst", "dst", "a", "b", SEL_REV)
            : [count] "+r" (count), [off] "=&r" (off)
            : [dst] "r" (dst), [a] "r" (a), [b] "r" (b)
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
            FMOP_MOD_VV_CORE("dst", "a", "b", "c", SEL_DIR)
            : [off] "=&r" (off), [count] __ASM_ARG_RW(count)
            : [dst] "r" (dst), [a] "r" (a), [b] "r" (b), [c] "r" (c)
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
            FMOP_MOD_VV_CORE("dst", "a", "b", "c", SEL_REV)
            : [off] "=&r" (off), [count] __ASM_ARG_RW(count)
            : [dst] "r" (dst), [a] "r" (a), [b] "r" (b), [c] "r" (c)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    #undef FMOP_MOD_VV_CORE
    #undef SEL_DIR
    #undef SEL_REV
}



#endif /* DSP_ARCH_X86_SSE2_PMATH_FMOP_VV_H_ */
