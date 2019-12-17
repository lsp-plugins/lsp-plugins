/*
 * hdotp.h
 *
 *  Created on: 29 нояб. 2019 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_SSE_HMATH_HDOTP_H_
#define DSP_ARCH_X86_SSE_HMATH_HDOTP_H_

#ifndef DSP_ARCH_X86_SSE_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_SSE_IMPL */

namespace sse
{
    float h_dotp(const float *a, const float *b, size_t count)
    {
        IF_ARCH_X86(
            float result;
            size_t off;
        );
        ARCH_X86_ASM
        (
            __ASM_EMIT("xorps       %%xmm0, %%xmm0")
            __ASM_EMIT("xor         %[off], %[off]")
            __ASM_EMIT("xorps       %%xmm1, %%xmm1")
            __ASM_EMIT("sub         $12, %[count]")
            __ASM_EMIT("jl          2f")

            /* x12 Blocks */
            __ASM_EMIT("1:")
            __ASM_EMIT("movups      0x00(%[a], %[off]), %%xmm2")
            __ASM_EMIT("movups      0x10(%[a], %[off]), %%xmm3")
            __ASM_EMIT("movups      0x20(%[a], %[off]), %%xmm4")
            __ASM_EMIT("movups      0x00(%[b], %[off]), %%xmm5")
            __ASM_EMIT("movups      0x10(%[b], %[off]), %%xmm6")
            __ASM_EMIT("movups      0x20(%[b], %[off]), %%xmm7")
            __ASM_EMIT("mulps       %%xmm5, %%xmm2")
            __ASM_EMIT("mulps       %%xmm6, %%xmm3")
            __ASM_EMIT("mulps       %%xmm7, %%xmm4")
            __ASM_EMIT("addps       %%xmm2, %%xmm0")
            __ASM_EMIT("addps       %%xmm3, %%xmm1")
            __ASM_EMIT("addps       %%xmm4, %%xmm0")
            __ASM_EMIT("add         $0x30, %[off]")
            __ASM_EMIT("sub         $12, %[count]")
            __ASM_EMIT("jae         1b")
            /* x8 Block */
            __ASM_EMIT("2:")
            __ASM_EMIT("add         $4, %[count]")
            __ASM_EMIT("jl          4f")
            __ASM_EMIT("movups      0x00(%[a], %[off]), %%xmm2")
            __ASM_EMIT("movups      0x10(%[a], %[off]), %%xmm3")
            __ASM_EMIT("movups      0x00(%[b], %[off]), %%xmm5")
            __ASM_EMIT("movups      0x10(%[b], %[off]), %%xmm6")
            __ASM_EMIT("mulps       %%xmm5, %%xmm2")
            __ASM_EMIT("mulps       %%xmm6, %%xmm3")
            __ASM_EMIT("addps       %%xmm2, %%xmm0")
            __ASM_EMIT("addps       %%xmm3, %%xmm1")
            __ASM_EMIT("sub         $8, %[count]")
            __ASM_EMIT("add         $0x20, %[off]")
            /* x4 Block */
            __ASM_EMIT("4:")
            __ASM_EMIT("add         $4, %[count]")
            __ASM_EMIT("jl          6f")
            __ASM_EMIT("movups      0x00(%[a], %[off]), %%xmm2")
            __ASM_EMIT("movups      0x00(%[b], %[off]), %%xmm5")
            __ASM_EMIT("mulps       %%xmm5, %%xmm2")
            __ASM_EMIT("addps       %%xmm2, %%xmm0")
            __ASM_EMIT("sub         $4, %[count]")
            __ASM_EMIT("add         $0x10, %[off]")
            /* x1 Blocks */
            __ASM_EMIT("6:")
            __ASM_EMIT("add         $3, %[count]")
            __ASM_EMIT("jl          8f")
            __ASM_EMIT("7:")
            __ASM_EMIT("movss       0x00(%[a], %[off]), %%xmm2")
            __ASM_EMIT("movss       0x00(%[b], %[off]), %%xmm5")
            __ASM_EMIT("mulss       %%xmm5, %%xmm2")
            __ASM_EMIT("addss       %%xmm2, %%xmm0")
            __ASM_EMIT("add         $0x04, %[off]")
            __ASM_EMIT("dec         %[count]")
            __ASM_EMIT("jge         7b")
            /* x1 End */
            __ASM_EMIT("8:")
            __ASM_EMIT("addps       %%xmm1, %%xmm0")
            __ASM_EMIT("movhlps     %%xmm0, %%xmm1")
            __ASM_EMIT("addps       %%xmm1, %%xmm0")
            __ASM_EMIT("unpcklps    %%xmm1, %%xmm0")
            __ASM_EMIT("movhlps     %%xmm0, %%xmm1")
            __ASM_EMIT("addps       %%xmm1, %%xmm0")

            : [count] "+r" (count), [off] "=&r" (off),
              "=Yz" (result)
            : [a] "r" (a), [b] "r" (b)
            : "cc",
              "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );

        return result;
    }

    float h_sqr_dotp(const float *a, const float *b, size_t count)
    {
        IF_ARCH_X86(
            float result;
            size_t off;
        );
        ARCH_X86_ASM
        (
            __ASM_EMIT("xorps       %%xmm0, %%xmm0")
            __ASM_EMIT("xor         %[off], %[off]")
            __ASM_EMIT("xorps       %%xmm1, %%xmm1")
            __ASM_EMIT("sub         $12, %[count]")
            __ASM_EMIT("jl          2f")

            /* x12 Blocks */
            __ASM_EMIT("1:")
            __ASM_EMIT("movups      0x00(%[a], %[off]), %%xmm2")
            __ASM_EMIT("movups      0x10(%[a], %[off]), %%xmm3")
            __ASM_EMIT("movups      0x20(%[a], %[off]), %%xmm4")
            __ASM_EMIT("movups      0x00(%[b], %[off]), %%xmm5")
            __ASM_EMIT("movups      0x10(%[b], %[off]), %%xmm6")
            __ASM_EMIT("movups      0x20(%[b], %[off]), %%xmm7")
            __ASM_EMIT("mulps       %%xmm2, %%xmm2")
            __ASM_EMIT("mulps       %%xmm3, %%xmm3")
            __ASM_EMIT("mulps       %%xmm4, %%xmm4")
            __ASM_EMIT("mulps       %%xmm5, %%xmm5")
            __ASM_EMIT("mulps       %%xmm6, %%xmm6")
            __ASM_EMIT("mulps       %%xmm7, %%xmm7")
            __ASM_EMIT("mulps       %%xmm5, %%xmm2")
            __ASM_EMIT("mulps       %%xmm6, %%xmm3")
            __ASM_EMIT("mulps       %%xmm7, %%xmm4")
            __ASM_EMIT("addps       %%xmm2, %%xmm0")
            __ASM_EMIT("addps       %%xmm3, %%xmm1")
            __ASM_EMIT("addps       %%xmm4, %%xmm0")
            __ASM_EMIT("add         $0x30, %[off]")
            __ASM_EMIT("sub         $12, %[count]")
            __ASM_EMIT("jae         1b")
            /* x8 Block */
            __ASM_EMIT("2:")
            __ASM_EMIT("add         $4, %[count]")
            __ASM_EMIT("jl          4f")
            __ASM_EMIT("movups      0x00(%[a], %[off]), %%xmm2")
            __ASM_EMIT("movups      0x10(%[a], %[off]), %%xmm3")
            __ASM_EMIT("movups      0x00(%[b], %[off]), %%xmm5")
            __ASM_EMIT("movups      0x10(%[b], %[off]), %%xmm6")
            __ASM_EMIT("mulps       %%xmm2, %%xmm2")
            __ASM_EMIT("mulps       %%xmm3, %%xmm3")
            __ASM_EMIT("mulps       %%xmm5, %%xmm5")
            __ASM_EMIT("mulps       %%xmm6, %%xmm6")
            __ASM_EMIT("mulps       %%xmm5, %%xmm2")
            __ASM_EMIT("mulps       %%xmm6, %%xmm3")
            __ASM_EMIT("addps       %%xmm2, %%xmm0")
            __ASM_EMIT("addps       %%xmm3, %%xmm1")
            __ASM_EMIT("sub         $8, %[count]")
            __ASM_EMIT("add         $0x20, %[off]")
            /* x4 Block */
            __ASM_EMIT("4:")
            __ASM_EMIT("add         $4, %[count]")
            __ASM_EMIT("jl          6f")
            __ASM_EMIT("movups      0x00(%[a], %[off]), %%xmm2")
            __ASM_EMIT("movups      0x00(%[b], %[off]), %%xmm5")
            __ASM_EMIT("mulps       %%xmm2, %%xmm2")
            __ASM_EMIT("mulps       %%xmm5, %%xmm5")
            __ASM_EMIT("mulps       %%xmm5, %%xmm2")
            __ASM_EMIT("addps       %%xmm2, %%xmm0")
            __ASM_EMIT("sub         $4, %[count]")
            __ASM_EMIT("add         $0x10, %[off]")
            /* x1 Blocks */
            __ASM_EMIT("6:")
            __ASM_EMIT("add         $3, %[count]")
            __ASM_EMIT("jl          8f")
            __ASM_EMIT("7:")
            __ASM_EMIT("movss       0x00(%[a], %[off]), %%xmm2")
            __ASM_EMIT("movss       0x00(%[b], %[off]), %%xmm5")
            __ASM_EMIT("mulss       %%xmm2, %%xmm2")
            __ASM_EMIT("mulss       %%xmm5, %%xmm5")
            __ASM_EMIT("mulss       %%xmm5, %%xmm2")
            __ASM_EMIT("addss       %%xmm2, %%xmm0")
            __ASM_EMIT("add         $0x04, %[off]")
            __ASM_EMIT("dec         %[count]")
            __ASM_EMIT("jge         7b")
            /* x1 End */
            __ASM_EMIT("8:")
            __ASM_EMIT("addps       %%xmm1, %%xmm0")
            __ASM_EMIT("movhlps     %%xmm0, %%xmm1")
            __ASM_EMIT("addps       %%xmm1, %%xmm0")
            __ASM_EMIT("unpcklps    %%xmm1, %%xmm0")
            __ASM_EMIT("movhlps     %%xmm0, %%xmm1")
            __ASM_EMIT("addps       %%xmm1, %%xmm0")

            : [count] "+r" (count), [off] "=&r" (off),
              "=Yz" (result)
            : [a] "r" (a), [b] "r" (b)
            : "cc",
              "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );

        return result;
    }

    float h_abs_dotp(const float *a, const float *b, size_t count)
    {
        IF_ARCH_X86(
            float result;
            size_t off;
        );
        ARCH_X86_ASM
        (
            __ASM_EMIT("xorps       %%xmm0, %%xmm0")
            __ASM_EMIT("xor         %[off], %[off]")
            __ASM_EMIT("xorps       %%xmm1, %%xmm1")
            __ASM_EMIT("movaps      %[SIGN], %%xmm2")
            __ASM_EMIT("sub         $8, %[count]")
            __ASM_EMIT("movaps      %%xmm2, %%xmm3")
            __ASM_EMIT("jl          2f")

            /* x8 Block */
            __ASM_EMIT("1:")
            __ASM_EMIT("movups      0x00(%[a], %[off]), %%xmm4")
            __ASM_EMIT("movups      0x10(%[a], %[off]), %%xmm5")
            __ASM_EMIT("movups      0x00(%[b], %[off]), %%xmm6")
            __ASM_EMIT("movups      0x10(%[b], %[off]), %%xmm7")
            __ASM_EMIT("andps       %%xmm2, %%xmm4")
            __ASM_EMIT("andps       %%xmm3, %%xmm5")
            __ASM_EMIT("andps       %%xmm2, %%xmm6")
            __ASM_EMIT("andps       %%xmm3, %%xmm7")
            __ASM_EMIT("mulps       %%xmm6, %%xmm4")
            __ASM_EMIT("mulps       %%xmm7, %%xmm5")
            __ASM_EMIT("addps       %%xmm4, %%xmm0")
            __ASM_EMIT("addps       %%xmm5, %%xmm1")
            __ASM_EMIT("add         $0x20, %[off]")
            __ASM_EMIT("sub         $8, %[count]")
            __ASM_EMIT("jae         1b")
            /* x4 Block */
            __ASM_EMIT("2:")
            __ASM_EMIT("add         $4, %[count]")
            __ASM_EMIT("jl          4f")
            __ASM_EMIT("movups      0x00(%[a], %[off]), %%xmm4")
            __ASM_EMIT("movups      0x00(%[b], %[off]), %%xmm6")
            __ASM_EMIT("andps       %%xmm2, %%xmm4")
            __ASM_EMIT("andps       %%xmm3, %%xmm6")
            __ASM_EMIT("mulps       %%xmm6, %%xmm4")
            __ASM_EMIT("addps       %%xmm4, %%xmm0")
            __ASM_EMIT("sub         $4, %[count]")
            __ASM_EMIT("add         $0x10, %[off]")
            /* x1 Blocks */
            __ASM_EMIT("4:")
            __ASM_EMIT("add         $3, %[count]")
            __ASM_EMIT("jl          6f")
            __ASM_EMIT("5:")
            __ASM_EMIT("movss       0x00(%[a], %[off]), %%xmm4")
            __ASM_EMIT("movss       0x00(%[b], %[off]), %%xmm6")
            __ASM_EMIT("andps       %%xmm2, %%xmm4")
            __ASM_EMIT("andps       %%xmm3, %%xmm6")
            __ASM_EMIT("mulss       %%xmm6, %%xmm4")
            __ASM_EMIT("addss       %%xmm4, %%xmm0")
            __ASM_EMIT("add         $0x04, %[off]")
            __ASM_EMIT("dec         %[count]")
            __ASM_EMIT("jge         5b")
            /* x1 End */
            __ASM_EMIT("6:")
            __ASM_EMIT("addps       %%xmm1, %%xmm0")
            __ASM_EMIT("movhlps     %%xmm0, %%xmm1")
            __ASM_EMIT("addps       %%xmm1, %%xmm0")
            __ASM_EMIT("unpcklps    %%xmm1, %%xmm0")
            __ASM_EMIT("movhlps     %%xmm0, %%xmm1")
            __ASM_EMIT("addps       %%xmm1, %%xmm0")

            : [count] "+r" (count), [off] "=&r" (off),
              "=Yz" (result)
            : [a] "r" (a), [b] "r" (b),
              [SIGN] "o" (X_SIGN)
            : "cc", "memory",
              "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );

        return result;
    }

}

#endif /* DSP_ARCH_X86_SSE_HMATH_HDOTP_H_ */
