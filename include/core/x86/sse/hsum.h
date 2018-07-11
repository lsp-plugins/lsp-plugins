/*
 * hsum.h
 *
 *  Created on: 18 мар. 2017 г.
 *      Author: sadko
 */

#ifndef CORE_X86_SSE_HSUM_H_
#define CORE_X86_SSE_HSUM_H_

#ifndef CORE_X86_SSE_IMPL
    #error "This header should not be included directly"
#endif /* CORE_X86_SSE_IMPL */

namespace lsp
{
    namespace sse
    {

        float h_sum(const float *src, size_t count)
        {
            float result;

            __asm__ __volatile__
            (
                __ASM_EMIT("xorps       %%xmm0, %%xmm0")
                __ASM_EMIT("test        %[count], %[count]")
                __ASM_EMIT("jz          2000f")

                /* Align source */
                __ASM_EMIT("1:")
                __ASM_EMIT("test        $0x0f, %[src]")
                __ASM_EMIT("jz          2f")
                __ASM_EMIT("movss       0x00(%[src]), %%xmm1")
                __ASM_EMIT("addss       %%xmm1, %%xmm0")
                __ASM_EMIT("add         $0x04, %[src]")
                __ASM_EMIT("dec         %[count]")
                __ASM_EMIT("jnz         1b")
                __ASM_EMIT("jmp         2000f")
                /* x24 Blocks */
                __ASM_EMIT("2:")
                __ASM_EMIT("xorps       %%xmm1, %%xmm1")
                __ASM_EMIT("sub         $0x18, %[count]")
                __ASM_EMIT("jb          4f")
                __ASM_EMIT("3:")
                __ASM_EMIT("movaps      0x00(%[src]), %%xmm2")
                __ASM_EMIT("movaps      0x10(%[src]), %%xmm3")
                __ASM_EMIT("movaps      0x20(%[src]), %%xmm4")
                __ASM_EMIT("movaps      0x30(%[src]), %%xmm5")
                __ASM_EMIT("movaps      0x40(%[src]), %%xmm6")
                __ASM_EMIT("movaps      0x50(%[src]), %%xmm7")
                __ASM_EMIT("addps       %%xmm2, %%xmm0")
                __ASM_EMIT("addps       %%xmm3, %%xmm1")
                __ASM_EMIT("addps       %%xmm4, %%xmm0")
                __ASM_EMIT("addps       %%xmm5, %%xmm1")
                __ASM_EMIT("addps       %%xmm6, %%xmm0")
                __ASM_EMIT("addps       %%xmm7, %%xmm1")
                __ASM_EMIT("add         $0x60, %[src]")
                __ASM_EMIT("sub         $0x18, %[count]")
                __ASM_EMIT("jae         3b")
                /* x16 Block */
                __ASM_EMIT("4:")
                __ASM_EMIT("add         $0x18, %[count]")
                __ASM_EMIT("test        $0x10, %[count]")
                __ASM_EMIT("jz          5f")
                __ASM_EMIT("movaps      0x00(%[src]), %%xmm2")
                __ASM_EMIT("movaps      0x10(%[src]), %%xmm3")
                __ASM_EMIT("movaps      0x20(%[src]), %%xmm4")
                __ASM_EMIT("movaps      0x30(%[src]), %%xmm5")
                __ASM_EMIT("addps       %%xmm2, %%xmm0")
                __ASM_EMIT("addps       %%xmm3, %%xmm1")
                __ASM_EMIT("addps       %%xmm4, %%xmm0")
                __ASM_EMIT("addps       %%xmm5, %%xmm1")
                __ASM_EMIT("add         $0x40, %[src]")
                /* x8 Block */
                __ASM_EMIT("5:")
                __ASM_EMIT("test        $0x08, %[count]")
                __ASM_EMIT("jz          6f")
                __ASM_EMIT("movaps      0x00(%[src]), %%xmm2")
                __ASM_EMIT("movaps      0x10(%[src]), %%xmm3")
                __ASM_EMIT("addps       %%xmm2, %%xmm0")
                __ASM_EMIT("addps       %%xmm3, %%xmm1")
                __ASM_EMIT("add         $0x20, %[src]")
                /* x4 Block */
                __ASM_EMIT("6:")
                __ASM_EMIT("test        $0x04, %[count]")
                __ASM_EMIT("jz          7f")
                __ASM_EMIT("movaps      0x00(%[src]), %%xmm2")
                __ASM_EMIT("addps       %%xmm2, %%xmm0")
                __ASM_EMIT("add         $0x10, %[src]")
                /* x1 Blocks */
                __ASM_EMIT("7:")
                __ASM_EMIT("addps       %%xmm1, %%xmm0")
                __ASM_EMIT("movhlps     %%xmm0, %%xmm1")
                __ASM_EMIT("addps       %%xmm1, %%xmm0")
                __ASM_EMIT("unpcklps    %%xmm1, %%xmm0")
                __ASM_EMIT("movhlps     %%xmm0, %%xmm1")
                __ASM_EMIT("and         $0x03, %[count]")
                __ASM_EMIT("addss       %%xmm1, %%xmm0")
                __ASM_EMIT("jz          2000f")
                __ASM_EMIT("8:")
                __ASM_EMIT("movss       0x00(%[src]), %%xmm1")
                __ASM_EMIT("addss       %%xmm1, %%xmm0")
                __ASM_EMIT("add         $0x04, %[src]")
                __ASM_EMIT("dec         %[count]")
                __ASM_EMIT("jnz         8b")

                __ASM_EMIT("2000:")

                : [src] "+r" (src), [count] "+r" (count), "=Yz" (result)
                :
                : "cc",
                  "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );

            return result;
        }

        float h_sqr_sum(const float *src, size_t count)
        {
            float result;

            __asm__ __volatile__
            (
                __ASM_EMIT("xorps       %%xmm0, %%xmm0")
                __ASM_EMIT("test        %[count], %[count]")
                __ASM_EMIT("jz          2000f")

                /* Align source */
                __ASM_EMIT("1:")
                __ASM_EMIT("test        $0x0f, %[src]")
                __ASM_EMIT("jz          2f")
                __ASM_EMIT("movss       0x00(%[src]), %%xmm1")
                __ASM_EMIT("mulss       %%xmm1, %%xmm1")
                __ASM_EMIT("add         $0x04, %[src]")
                __ASM_EMIT("addss       %%xmm1, %%xmm0")
                __ASM_EMIT("dec         %[count]")
                __ASM_EMIT("jnz         1b")
                __ASM_EMIT("jmp         2000f")
                /* x24 Blocks */
                __ASM_EMIT("2:")
                __ASM_EMIT("xorps       %%xmm1, %%xmm1")
                __ASM_EMIT("sub         $0x18, %[count]")
                __ASM_EMIT("jb          4f")
                __ASM_EMIT("3:")
                __ASM_EMIT("movaps      0x00(%[src]), %%xmm2")
                __ASM_EMIT("movaps      0x10(%[src]), %%xmm3")
                __ASM_EMIT("movaps      0x20(%[src]), %%xmm4")
                __ASM_EMIT("movaps      0x30(%[src]), %%xmm5")
                __ASM_EMIT("movaps      0x40(%[src]), %%xmm6")
                __ASM_EMIT("movaps      0x50(%[src]), %%xmm7")
                __ASM_EMIT("mulps       %%xmm2, %%xmm2")
                __ASM_EMIT("mulps       %%xmm3, %%xmm3")
                __ASM_EMIT("mulps       %%xmm4, %%xmm4")
                __ASM_EMIT("mulps       %%xmm5, %%xmm5")
                __ASM_EMIT("mulps       %%xmm6, %%xmm6")
                __ASM_EMIT("mulps       %%xmm7, %%xmm7")
                __ASM_EMIT("addps       %%xmm2, %%xmm0")
                __ASM_EMIT("addps       %%xmm3, %%xmm1")
                __ASM_EMIT("addps       %%xmm4, %%xmm0")
                __ASM_EMIT("addps       %%xmm5, %%xmm1")
                __ASM_EMIT("addps       %%xmm6, %%xmm0")
                __ASM_EMIT("addps       %%xmm7, %%xmm1")
                __ASM_EMIT("add         $0x60, %[src]")
                __ASM_EMIT("sub         $0x18, %[count]")
                __ASM_EMIT("jae         3b")
                /* x16 Block */
                __ASM_EMIT("4:")
                __ASM_EMIT("add         $0x18, %[count]")
                __ASM_EMIT("test        $0x10, %[count]")
                __ASM_EMIT("jz          5f")
                __ASM_EMIT("movaps      0x00(%[src]), %%xmm2")
                __ASM_EMIT("movaps      0x10(%[src]), %%xmm3")
                __ASM_EMIT("movaps      0x20(%[src]), %%xmm4")
                __ASM_EMIT("movaps      0x30(%[src]), %%xmm5")
                __ASM_EMIT("mulps       %%xmm2, %%xmm2")
                __ASM_EMIT("mulps       %%xmm3, %%xmm3")
                __ASM_EMIT("mulps       %%xmm4, %%xmm4")
                __ASM_EMIT("mulps       %%xmm5, %%xmm5")
                __ASM_EMIT("addps       %%xmm2, %%xmm0")
                __ASM_EMIT("addps       %%xmm3, %%xmm1")
                __ASM_EMIT("addps       %%xmm4, %%xmm0")
                __ASM_EMIT("addps       %%xmm5, %%xmm1")
                __ASM_EMIT("add         $0x40, %[src]")
                /* x8 Block */
                __ASM_EMIT("5:")
                __ASM_EMIT("test        $0x08, %[count]")
                __ASM_EMIT("jz          6f")
                __ASM_EMIT("movaps      0x00(%[src]), %%xmm2")
                __ASM_EMIT("movaps      0x10(%[src]), %%xmm3")
                __ASM_EMIT("mulps       %%xmm2, %%xmm2")
                __ASM_EMIT("mulps       %%xmm3, %%xmm3")
                __ASM_EMIT("addps       %%xmm2, %%xmm0")
                __ASM_EMIT("addps       %%xmm3, %%xmm1")
                __ASM_EMIT("add         $0x20, %[src]")
                /* x4 Block */
                __ASM_EMIT("6:")
                __ASM_EMIT("test        $0x04, %[count]")
                __ASM_EMIT("jz          7f")
                __ASM_EMIT("movaps      0x00(%[src]), %%xmm2")
                __ASM_EMIT("mulps       %%xmm2, %%xmm2")
                __ASM_EMIT("addps       %%xmm2, %%xmm0")
                __ASM_EMIT("add         $0x10, %[src]")
                /* x1 Blocks */
                __ASM_EMIT("7:")
                __ASM_EMIT("addps       %%xmm1, %%xmm0")
                __ASM_EMIT("movhlps     %%xmm0, %%xmm1")
                __ASM_EMIT("addps       %%xmm1, %%xmm0")
                __ASM_EMIT("unpcklps    %%xmm1, %%xmm0")
                __ASM_EMIT("movhlps     %%xmm0, %%xmm1")
                __ASM_EMIT("and         $0x03, %[count]")
                __ASM_EMIT("addss       %%xmm1, %%xmm0")
                __ASM_EMIT("jz          2000f")
                __ASM_EMIT("8:")
                __ASM_EMIT("movss       0x00(%[src]), %%xmm1")
                __ASM_EMIT("mulss       %%xmm1, %%xmm1")
                __ASM_EMIT("add         $0x04, %[src]")
                __ASM_EMIT("addss       %%xmm1, %%xmm0")
                __ASM_EMIT("dec         %[count]")
                __ASM_EMIT("jnz         8b")

                __ASM_EMIT("2000:")

                : [src] "+r" (src), [count] "+r" (count), "=Yz" (result)
                :
                : "cc",
                  "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );

            return result;
        }

        float h_abs_sum(const float *src, size_t count)
        {
            float result;

            __asm__ __volatile__
            (
                __ASM_EMIT("xorps       %%xmm0, %%xmm0")
                __ASM_EMIT("test        %[count], %[count]")
                __ASM_EMIT("jz          2000f")
                __ASM_EMIT("movaps      %[X_SIGN], %%xmm6")

                /* Align source */
                __ASM_EMIT("1:")
                __ASM_EMIT("test        $0x0f, %[src]")
                __ASM_EMIT("jz          2f")
                __ASM_EMIT("movss       0x00(%[src]), %%xmm1")
                __ASM_EMIT("andps       %%xmm6, %%xmm1")
                __ASM_EMIT("add         $0x04, %[src]")
                __ASM_EMIT("addss       %%xmm1, %%xmm0")
                __ASM_EMIT("dec         %[count]")
                __ASM_EMIT("jnz         1b")
                __ASM_EMIT("jmp         2000f")
                /* x16 Blocks */
                __ASM_EMIT("2:")
                __ASM_EMIT("xorps       %%xmm1, %%xmm1")
                __ASM_EMIT("movaps      %%xmm6, %%xmm7")
                __ASM_EMIT("sub         $0x10, %[count]")
                __ASM_EMIT("jb          4f")
                __ASM_EMIT("3:")
                __ASM_EMIT("movaps      0x00(%[src]), %%xmm2")
                __ASM_EMIT("movaps      0x10(%[src]), %%xmm3")
                __ASM_EMIT("movaps      0x20(%[src]), %%xmm4")
                __ASM_EMIT("movaps      0x30(%[src]), %%xmm5")
                __ASM_EMIT("andps       %%xmm6, %%xmm2")
                __ASM_EMIT("andps       %%xmm7, %%xmm3")
                __ASM_EMIT("andps       %%xmm6, %%xmm4")
                __ASM_EMIT("andps       %%xmm7, %%xmm5")
                __ASM_EMIT("addps       %%xmm2, %%xmm0")
                __ASM_EMIT("addps       %%xmm3, %%xmm1")
                __ASM_EMIT("addps       %%xmm4, %%xmm0")
                __ASM_EMIT("addps       %%xmm5, %%xmm1")
                __ASM_EMIT("add         $0x40, %[src]")
                __ASM_EMIT("sub         $0x10, %[count]")
                __ASM_EMIT("jae         3b")
                /* x8 Block */
                __ASM_EMIT("4:")
                __ASM_EMIT("add         $0x10, %[count]")
                __ASM_EMIT("test        $0x08, %[count]")
                __ASM_EMIT("jz          5f")
                __ASM_EMIT("movaps      0x00(%[src]), %%xmm2")
                __ASM_EMIT("movaps      0x10(%[src]), %%xmm3")
                __ASM_EMIT("andps       %%xmm6, %%xmm2")
                __ASM_EMIT("andps       %%xmm7, %%xmm3")
                __ASM_EMIT("addps       %%xmm2, %%xmm0")
                __ASM_EMIT("addps       %%xmm3, %%xmm1")
                __ASM_EMIT("add         $0x20, %[src]")
                /* x4 Block */
                __ASM_EMIT("5:")
                __ASM_EMIT("test        $0x04, %[count]")
                __ASM_EMIT("jz          6f")
                __ASM_EMIT("movaps      0x00(%[src]), %%xmm2")
                __ASM_EMIT("andps       %%xmm6, %%xmm2")
                __ASM_EMIT("addps       %%xmm2, %%xmm0")
                __ASM_EMIT("add         $0x10, %[src]")
                /* x1 Blocks */
                __ASM_EMIT("6:")
                __ASM_EMIT("addps       %%xmm1, %%xmm0")
                __ASM_EMIT("movhlps     %%xmm0, %%xmm1")
                __ASM_EMIT("addps       %%xmm1, %%xmm0")
                __ASM_EMIT("unpcklps    %%xmm1, %%xmm0")
                __ASM_EMIT("movhlps     %%xmm0, %%xmm1")
                __ASM_EMIT("and         $0x03, %[count]")
                __ASM_EMIT("addss       %%xmm1, %%xmm0")
                __ASM_EMIT("jz          2000f")
                __ASM_EMIT("7:")
                __ASM_EMIT("movss       0x00(%[src]), %%xmm1")
                __ASM_EMIT("andps       %%xmm6, %%xmm1")
                __ASM_EMIT("add         $0x04, %[src]")
                __ASM_EMIT("addss       %%xmm1, %%xmm0")
                __ASM_EMIT("dec         %[count]")
                __ASM_EMIT("jnz         7b")

                __ASM_EMIT("2000:")

                : [src] "+r" (src), [count] "+r" (count), "=Yz" (result)
                : [X_SIGN] "m" (X_SIGN)
                : "cc",
                  "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );

            return result;
        }

    }
}



#endif /* CORE_X86_SSE_HSUM_H_ */
