/*
 * mix.h
 *
 *  Created on: 17 мар. 2017 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_SSE_MIX_H_
#define DSP_ARCH_X86_SSE_MIX_H_

#ifndef DSP_ARCH_X86_SSE_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_SSE_IMPL */

namespace sse
{
    #define MIX2_CORE(MV_SRC)   \
        __ASM_EMIT("sub         $0x0c, %[count]")   \
        __ASM_EMIT("jb          2f")   \
        /* x12 blocks */ \
        __ASM_EMIT("1:") \
        __ASM_EMIT("movaps      0x00(%[dst]), %%xmm2")   \
        __ASM_EMIT("movaps      0x10(%[dst]), %%xmm3")   \
        __ASM_EMIT("movaps      0x20(%[dst]), %%xmm4")   \
        __ASM_EMIT(MV_SRC "     0x00(%[src]), %%xmm5")   \
        __ASM_EMIT(MV_SRC "     0x10(%[src]), %%xmm6")   \
        __ASM_EMIT(MV_SRC "     0x20(%[src]), %%xmm7")   \
        __ASM_EMIT("mulps       %%xmm0, %%xmm2")    \
        __ASM_EMIT("mulps       %%xmm1, %%xmm5")    \
        __ASM_EMIT("mulps       %%xmm0, %%xmm3")    \
        __ASM_EMIT("mulps       %%xmm1, %%xmm6")    \
        __ASM_EMIT("mulps       %%xmm0, %%xmm4")    \
        __ASM_EMIT("mulps       %%xmm1, %%xmm7")    \
        __ASM_EMIT("addps       %%xmm5, %%xmm2")    \
        __ASM_EMIT("addps       %%xmm6, %%xmm3")    \
        __ASM_EMIT("addps       %%xmm7, %%xmm4")    \
        __ASM_EMIT("movaps      %%xmm2, 0x00(%[dst])")   \
        __ASM_EMIT("movaps      %%xmm3, 0x10(%[dst])")   \
        __ASM_EMIT("movaps      %%xmm4, 0x20(%[dst])")   \
        __ASM_EMIT("add         $0x30, %[src]") \
        __ASM_EMIT("add         $0x30, %[dst]") \
        __ASM_EMIT("sub         $0x0c, %[count]") \
        __ASM_EMIT("jae         1b")   \
        /* x8 block */ \
        __ASM_EMIT("2:") \
        __ASM_EMIT("add         $0x0c, %[count]") \
        __ASM_EMIT("test        $0x08, %[count]") \
        __ASM_EMIT("jz          3f") \
        __ASM_EMIT("movaps      0x00(%[dst]), %%xmm2")   \
        __ASM_EMIT("movaps      0x10(%[dst]), %%xmm3")   \
        __ASM_EMIT(MV_SRC "     0x00(%[src]), %%xmm5")   \
        __ASM_EMIT(MV_SRC "     0x10(%[src]), %%xmm6")   \
        __ASM_EMIT("mulps       %%xmm0, %%xmm2")    \
        __ASM_EMIT("mulps       %%xmm1, %%xmm5")    \
        __ASM_EMIT("mulps       %%xmm0, %%xmm3")    \
        __ASM_EMIT("mulps       %%xmm1, %%xmm6")    \
        __ASM_EMIT("addps       %%xmm5, %%xmm2")    \
        __ASM_EMIT("addps       %%xmm6, %%xmm3")    \
        __ASM_EMIT("movaps      %%xmm2, 0x00(%[dst])")   \
        __ASM_EMIT("movaps      %%xmm3, 0x10(%[dst])")   \
        __ASM_EMIT("add         $0x20, %[src]") \
        __ASM_EMIT("add         $0x20, %[dst]") \
        /* x4 block */ \
        __ASM_EMIT("3:") \
        __ASM_EMIT("test        $0x04, %[count]") \
        __ASM_EMIT("jz          4f") \
        __ASM_EMIT("movaps      0x00(%[dst]), %%xmm2")   \
        __ASM_EMIT(MV_SRC "     0x00(%[src]), %%xmm5")   \
        __ASM_EMIT("mulps       %%xmm0, %%xmm2")    \
        __ASM_EMIT("mulps       %%xmm1, %%xmm5")    \
        __ASM_EMIT("addps       %%xmm5, %%xmm2")    \
        __ASM_EMIT("movaps      %%xmm2, 0x00(%[dst])")   \
        __ASM_EMIT("add         $0x10, %[src]") \
        __ASM_EMIT("add         $0x10, %[dst]") \
        /* x1 blocks */ \
        __ASM_EMIT("4:") \
        __ASM_EMIT("and         $0x03, %[count]") \
        __ASM_EMIT("jz          2000f") \
        __ASM_EMIT("5:") \
        __ASM_EMIT("movss       0x00(%[dst]), %%xmm2")   \
        __ASM_EMIT("movss       0x00(%[src]), %%xmm5")   \
        __ASM_EMIT("mulss       %%xmm0, %%xmm2")    \
        __ASM_EMIT("mulss       %%xmm1, %%xmm5")    \
        __ASM_EMIT("addss       %%xmm5, %%xmm2")    \
        __ASM_EMIT("movss       %%xmm2, 0x00(%[dst])")   \
        __ASM_EMIT("add         $0x04, %[src]") \
        __ASM_EMIT("add         $0x04, %[dst]") \
        __ASM_EMIT("dec         %[count]") \
        __ASM_EMIT("jnz         5b") \


    void mix2(float *dst, const float *src, float k1, float k2, size_t count)
    {
        ARCH_X86_ASM
        (
            __ASM_EMIT("test        %[count], %[count]")
            __ASM_EMIT("jz          2000f")

            __ASM_EMIT("movss       %[k1], %%xmm0")
            __ASM_EMIT("movss       %[k2], %%xmm1")
            __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0")
            __ASM_EMIT("shufps      $0x00, %%xmm1, %%xmm1")

            // Align dst
            __ASM_EMIT("1:")
            __ASM_EMIT("test        $0x0f, %[dst]")
            __ASM_EMIT("jz          2f") \
            __ASM_EMIT("movss       0x00(%[dst]), %%xmm2")
            __ASM_EMIT("movss       0x00(%[src]), %%xmm5")
            __ASM_EMIT("mulss       %%xmm0, %%xmm2")
            __ASM_EMIT("mulss       %%xmm1, %%xmm5")
            __ASM_EMIT("addss       %%xmm5, %%xmm2")
            __ASM_EMIT("movss       %%xmm2, 0x00(%[dst])")
            __ASM_EMIT("add         $0x04, %[src]")
            __ASM_EMIT("add         $0x04, %[dst]")
            __ASM_EMIT("dec         %[count]")
            __ASM_EMIT("jnz         1b")
            __ASM_EMIT("jmp         2000f")

            // Do main function
            __ASM_EMIT("2:")
            __ASM_EMIT("test        $0x0f, %[src]")
            __ASM_EMIT("jnz         1001f")
                MIX2_CORE("movaps")
                __ASM_EMIT("jmp         2000f")
            __ASM_EMIT("1001:")
                MIX2_CORE("movups")

            __ASM_EMIT("2000:")

            : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
            : [k1] "m"(k1), [k2] "m"(k2)
            : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    #undef MIX2_CORE

    #define MIXMOVE2_CORE(MV_SRC1, MV_SRC2)   \
        __ASM_EMIT("sub         $0x0c, %[count]")   \
        __ASM_EMIT("jb          2f")   \
        /* x12 blocks */ \
        __ASM_EMIT("1:") \
        __ASM_EMIT(MV_SRC1 "    0x00(%[src1]), %%xmm2")   \
        __ASM_EMIT(MV_SRC1 "    0x10(%[src1]), %%xmm3")   \
        __ASM_EMIT(MV_SRC1 "    0x20(%[src1]), %%xmm4")   \
        __ASM_EMIT(MV_SRC2 "    0x00(%[src2]), %%xmm5")   \
        __ASM_EMIT(MV_SRC2 "    0x10(%[src2]), %%xmm6")   \
        __ASM_EMIT(MV_SRC2 "    0x20(%[src2]), %%xmm7")   \
        __ASM_EMIT("mulps       %%xmm0, %%xmm2")    \
        __ASM_EMIT("mulps       %%xmm1, %%xmm5")    \
        __ASM_EMIT("mulps       %%xmm0, %%xmm3")    \
        __ASM_EMIT("mulps       %%xmm1, %%xmm6")    \
        __ASM_EMIT("mulps       %%xmm0, %%xmm4")    \
        __ASM_EMIT("mulps       %%xmm1, %%xmm7")    \
        __ASM_EMIT("addps       %%xmm5, %%xmm2")    \
        __ASM_EMIT("addps       %%xmm6, %%xmm3")    \
        __ASM_EMIT("addps       %%xmm7, %%xmm4")    \
        __ASM_EMIT("movaps      %%xmm2, 0x00(%[dst])")   \
        __ASM_EMIT("movaps      %%xmm3, 0x10(%[dst])")   \
        __ASM_EMIT("movaps      %%xmm4, 0x20(%[dst])")   \
        __ASM_EMIT("add         $0x30, %[src1]") \
        __ASM_EMIT("add         $0x30, %[src2]") \
        __ASM_EMIT("add         $0x30, %[dst]") \
        __ASM_EMIT("sub         $0x0c, %[count]") \
        __ASM_EMIT("jae         1b")   \
        /* x8 block */ \
        __ASM_EMIT("2:") \
        __ASM_EMIT("add         $0x0c, %[count]") \
        __ASM_EMIT("test        $0x08, %[count]") \
        __ASM_EMIT("jz          3f") \
        __ASM_EMIT(MV_SRC1 "    0x00(%[src1]), %%xmm2")   \
        __ASM_EMIT(MV_SRC1 "    0x10(%[src1]), %%xmm3")   \
        __ASM_EMIT(MV_SRC2 "    0x00(%[src2]), %%xmm5")   \
        __ASM_EMIT(MV_SRC2 "    0x10(%[src2]), %%xmm6")   \
        __ASM_EMIT("mulps       %%xmm0, %%xmm2")    \
        __ASM_EMIT("mulps       %%xmm1, %%xmm5")    \
        __ASM_EMIT("mulps       %%xmm0, %%xmm3")    \
        __ASM_EMIT("mulps       %%xmm1, %%xmm6")    \
        __ASM_EMIT("addps       %%xmm5, %%xmm2")    \
        __ASM_EMIT("addps       %%xmm6, %%xmm3")    \
        __ASM_EMIT("movaps      %%xmm2, 0x00(%[dst])")   \
        __ASM_EMIT("movaps      %%xmm3, 0x10(%[dst])")   \
        __ASM_EMIT("add         $0x20, %[src1]") \
        __ASM_EMIT("add         $0x20, %[src2]") \
        __ASM_EMIT("add         $0x20, %[dst]") \
        /* x4 block */ \
        __ASM_EMIT("3:") \
        __ASM_EMIT("test        $0x04, %[count]") \
        __ASM_EMIT("jz          4f") \
        __ASM_EMIT(MV_SRC1 "    0x00(%[src1]), %%xmm2")   \
        __ASM_EMIT(MV_SRC2 "    0x00(%[src2]), %%xmm5")   \
        __ASM_EMIT("mulps       %%xmm0, %%xmm2")    \
        __ASM_EMIT("mulps       %%xmm1, %%xmm5")    \
        __ASM_EMIT("addps       %%xmm5, %%xmm2")    \
        __ASM_EMIT("movaps      %%xmm2, 0x00(%[dst])")   \
        __ASM_EMIT("add         $0x10, %[src1]") \
        __ASM_EMIT("add         $0x10, %[src2]") \
        __ASM_EMIT("add         $0x10, %[dst]") \
        /* x1 blocks */ \
        __ASM_EMIT("4:") \
        __ASM_EMIT("and         $0x03, %[count]") \
        __ASM_EMIT("jz          2000f") \
        __ASM_EMIT("5:") \
        __ASM_EMIT("movss       0x00(%[src1]), %%xmm2")   \
        __ASM_EMIT("movss       0x00(%[src2]), %%xmm5")   \
        __ASM_EMIT("mulss       %%xmm0, %%xmm2")    \
        __ASM_EMIT("mulss       %%xmm1, %%xmm5")    \
        __ASM_EMIT("addss       %%xmm5, %%xmm2")    \
        __ASM_EMIT("movss       %%xmm2, 0x00(%[dst])")   \
        __ASM_EMIT("add         $0x04, %[src1]") \
        __ASM_EMIT("add         $0x04, %[src2]") \
        __ASM_EMIT("add         $0x04, %[dst]") \
        __ASM_EMIT("dec         %[count]") \
        __ASM_EMIT("jnz         5b") \

    void mix_copy2(float *dst, const float *src1, const float *src2, float k1, float k2, size_t count)
    {
        ARCH_X86_ASM
        (
            __ASM_EMIT("test        %[count], %[count]")
            __ASM_EMIT("jz          2000f")

            __ASM_EMIT("movss       %[k1], %%xmm0")
            __ASM_EMIT("movss       %[k2], %%xmm1")
            __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0")
            __ASM_EMIT("shufps      $0x00, %%xmm1, %%xmm1")

            // Align dst
            __ASM_EMIT("1:")
            __ASM_EMIT("test        $0x0f, %[dst]")
            __ASM_EMIT("jz          2f")
            __ASM_EMIT("movss       0x00(%[src1]), %%xmm2")
            __ASM_EMIT("movss       0x00(%[src2]), %%xmm5")
            __ASM_EMIT("mulss       %%xmm0, %%xmm2")
            __ASM_EMIT("mulss       %%xmm1, %%xmm5")
            __ASM_EMIT("addss       %%xmm5, %%xmm2")
            __ASM_EMIT("movss       %%xmm2, 0x00(%[dst])")
            __ASM_EMIT("add         $0x04, %[src1]")
            __ASM_EMIT("add         $0x04, %[src2]")
            __ASM_EMIT("add         $0x04, %[dst]")
            __ASM_EMIT("dec         %[count]")
            __ASM_EMIT("jnz         1b")
            __ASM_EMIT("jmp         2000f")

            // Do main function
            __ASM_EMIT("2:")
            __ASM_EMIT("test        $0x0f, %[src1]")
            __ASM_EMIT("jnz         1010f")
                __ASM_EMIT("test        $0x0f, %[src2]")
                __ASM_EMIT("jnz         1001f")
                    MIXMOVE2_CORE("movaps", "movaps")
                    __ASM_EMIT("jmp         2000f")
                __ASM_EMIT("1001:")
                    MIXMOVE2_CORE("movaps", "movups")
                    __ASM_EMIT("jmp         2000f")
            __ASM_EMIT("1010:")
                __ASM_EMIT("test        $0x0f, %[src2]")
                __ASM_EMIT("jnz         1011f")
                    MIXMOVE2_CORE("movups", "movaps")
                    __ASM_EMIT("jmp         2000f")
                __ASM_EMIT("1011:")
                    MIXMOVE2_CORE("movups", "movups")

            __ASM_EMIT("2000:")

            : [dst] "+r" (dst), [src1] "+r" (src1), [src2] "+r" (src2), [count] "+r" (count)
            : [k1] "m"(k1), [k2] "m"(k2)
            : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    #undef MIXMOVE2_CORE

    #define MIXADD2_CORE(MV_SRC1, MV_SRC2)   \
        __ASM_EMIT("sub         $0x0c, %[count]")   \
        __ASM_EMIT("jb          2f")   \
        /* x12 blocks */ \
        __ASM_EMIT("1:") \
        __ASM_EMIT(MV_SRC1 "    0x00(%[src1]), %%xmm2")   \
        __ASM_EMIT(MV_SRC1 "    0x10(%[src1]), %%xmm3")   \
        __ASM_EMIT(MV_SRC1 "    0x20(%[src1]), %%xmm4")   \
        __ASM_EMIT(MV_SRC2 "    0x00(%[src2]), %%xmm5")   \
        __ASM_EMIT(MV_SRC2 "    0x10(%[src2]), %%xmm6")   \
        __ASM_EMIT(MV_SRC2 "    0x20(%[src2]), %%xmm7")   \
        __ASM_EMIT("mulps       %%xmm0, %%xmm2")    \
        __ASM_EMIT("mulps       %%xmm1, %%xmm5")    \
        __ASM_EMIT("mulps       %%xmm0, %%xmm3")    \
        __ASM_EMIT("mulps       %%xmm1, %%xmm6")    \
        __ASM_EMIT("mulps       %%xmm0, %%xmm4")    \
        __ASM_EMIT("mulps       %%xmm1, %%xmm7")    \
        __ASM_EMIT("addps       %%xmm5, %%xmm2")    \
        __ASM_EMIT("addps       %%xmm6, %%xmm3")    \
        __ASM_EMIT("addps       %%xmm7, %%xmm4")    \
        __ASM_EMIT("movaps      0x00(%[dst]), %%xmm5")   \
        __ASM_EMIT("movaps      0x10(%[dst]), %%xmm6")   \
        __ASM_EMIT("movaps      0x20(%[dst]), %%xmm7")   \
        __ASM_EMIT("addps       %%xmm2, %%xmm5")    \
        __ASM_EMIT("addps       %%xmm3, %%xmm6")    \
        __ASM_EMIT("addps       %%xmm4, %%xmm7")    \
        __ASM_EMIT("movaps      %%xmm5, 0x00(%[dst])")   \
        __ASM_EMIT("movaps      %%xmm6, 0x10(%[dst])")   \
        __ASM_EMIT("movaps      %%xmm7, 0x20(%[dst])")   \
        __ASM_EMIT("add         $0x30, %[src1]") \
        __ASM_EMIT("add         $0x30, %[src2]") \
        __ASM_EMIT("add         $0x30, %[dst]") \
        __ASM_EMIT("sub         $0x0c, %[count]") \
        __ASM_EMIT("jae         1b")   \
        /* x8 block */ \
        __ASM_EMIT("2:") \
        __ASM_EMIT("add         $0x0c, %[count]") \
        __ASM_EMIT("test        $0x08, %[count]") \
        __ASM_EMIT("jz          3f") \
        __ASM_EMIT(MV_SRC1 "    0x00(%[src1]), %%xmm2")   \
        __ASM_EMIT(MV_SRC1 "    0x10(%[src1]), %%xmm3")   \
        __ASM_EMIT(MV_SRC2 "    0x00(%[src2]), %%xmm5")   \
        __ASM_EMIT(MV_SRC2 "    0x10(%[src2]), %%xmm6")   \
        __ASM_EMIT("mulps       %%xmm0, %%xmm2")    \
        __ASM_EMIT("mulps       %%xmm1, %%xmm5")    \
        __ASM_EMIT("mulps       %%xmm0, %%xmm3")    \
        __ASM_EMIT("mulps       %%xmm1, %%xmm6")    \
        __ASM_EMIT("addps       %%xmm5, %%xmm2")    \
        __ASM_EMIT("addps       %%xmm6, %%xmm3")    \
        __ASM_EMIT("movaps      0x00(%[dst]), %%xmm5")   \
        __ASM_EMIT("movaps      0x10(%[dst]), %%xmm6")   \
        __ASM_EMIT("addps       %%xmm2, %%xmm5")    \
        __ASM_EMIT("addps       %%xmm3, %%xmm6")    \
        __ASM_EMIT("movaps      %%xmm5, 0x00(%[dst])")   \
        __ASM_EMIT("movaps      %%xmm6, 0x10(%[dst])")   \
        __ASM_EMIT("add         $0x20, %[src1]") \
        __ASM_EMIT("add         $0x20, %[src2]") \
        __ASM_EMIT("add         $0x20, %[dst]") \
        /* x4 block */ \
        __ASM_EMIT("3:") \
        __ASM_EMIT("test        $0x04, %[count]") \
        __ASM_EMIT("jz          4f") \
        __ASM_EMIT(MV_SRC1 "    0x00(%[src1]), %%xmm2")   \
        __ASM_EMIT(MV_SRC2 "    0x00(%[src2]), %%xmm5")   \
        __ASM_EMIT("mulps       %%xmm0, %%xmm2")    \
        __ASM_EMIT("mulps       %%xmm1, %%xmm5")    \
        __ASM_EMIT("addps       %%xmm5, %%xmm2")    \
        __ASM_EMIT("movaps      0x00(%[dst]), %%xmm5")   \
        __ASM_EMIT("addps       %%xmm2, %%xmm5")    \
        __ASM_EMIT("movaps      %%xmm5, 0x00(%[dst])")   \
        __ASM_EMIT("add         $0x10, %[src1]") \
        __ASM_EMIT("add         $0x10, %[src2]") \
        __ASM_EMIT("add         $0x10, %[dst]") \
        /* x1 blocks */ \
        __ASM_EMIT("4:") \
        __ASM_EMIT("and         $0x03, %[count]") \
        __ASM_EMIT("jz          2000f") \
        __ASM_EMIT("5:") \
        __ASM_EMIT("movss       0x00(%[src1]), %%xmm2")   \
        __ASM_EMIT("movss       0x00(%[src2]), %%xmm5")   \
        __ASM_EMIT("mulss       %%xmm0, %%xmm2")    \
        __ASM_EMIT("mulss       %%xmm1, %%xmm5")    \
        __ASM_EMIT("addss       %%xmm5, %%xmm2")    \
        __ASM_EMIT("movss       0x00(%[dst]), %%xmm5")   \
        __ASM_EMIT("addss       %%xmm2, %%xmm5")    \
        __ASM_EMIT("movss       %%xmm5, 0x00(%[dst])")   \
        __ASM_EMIT("add         $0x04, %[src1]") \
        __ASM_EMIT("add         $0x04, %[src2]") \
        __ASM_EMIT("add         $0x04, %[dst]") \
        __ASM_EMIT("dec         %[count]") \
        __ASM_EMIT("jnz         5b") \

    void mix_add2(float *dst, const float *src1, const float *src2, float k1, float k2, size_t count)
    {
        ARCH_X86_ASM
        (
            __ASM_EMIT("test        %[count], %[count]")
            __ASM_EMIT("jz          2000f")

            __ASM_EMIT("movss       %[k1], %%xmm0")
            __ASM_EMIT("movss       %[k2], %%xmm1")
            __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0")
            __ASM_EMIT("shufps      $0x00, %%xmm1, %%xmm1")

            // Align dst
            __ASM_EMIT("1:")
            __ASM_EMIT("test        $0x0f, %[dst]")
            __ASM_EMIT("jz          2f")
            __ASM_EMIT("movss       0x00(%[src1]), %%xmm2")
            __ASM_EMIT("movss       0x00(%[src2]), %%xmm5")
            __ASM_EMIT("mulss       %%xmm0, %%xmm2")
            __ASM_EMIT("mulss       %%xmm1, %%xmm5")
            __ASM_EMIT("addss       %%xmm5, %%xmm2")
            __ASM_EMIT("movss       0x00(%[dst]), %%xmm5")
            __ASM_EMIT("addss       %%xmm2, %%xmm5")
            __ASM_EMIT("movss       %%xmm5, 0x00(%[dst])")
            __ASM_EMIT("add         $0x04, %[src1]")
            __ASM_EMIT("add         $0x04, %[src2]")
            __ASM_EMIT("add         $0x04, %[dst]")
            __ASM_EMIT("dec         %[count]")
            __ASM_EMIT("jnz         1b")
            __ASM_EMIT("jmp         2000f")

            // Do main function
            __ASM_EMIT("2:")
            __ASM_EMIT("test        $0x0f, %[src1]")
            __ASM_EMIT("jnz         1010f")
                __ASM_EMIT("test        $0x0f, %[src2]")
                __ASM_EMIT("jnz         1001f")
                    MIXADD2_CORE("movaps", "movaps")
                    __ASM_EMIT("jmp         2000f")
                __ASM_EMIT("1001:")
                    MIXADD2_CORE("movaps", "movups")
                    __ASM_EMIT("jmp         2000f")
            __ASM_EMIT("1010:")
                __ASM_EMIT("test        $0x0f, %[src2]")
                __ASM_EMIT("jnz         1011f")
                    MIXADD2_CORE("movups", "movaps")
                    __ASM_EMIT("jmp         2000f")
                __ASM_EMIT("1011:")
                    MIXADD2_CORE("movups", "movups")

            __ASM_EMIT("2000:")

            : [dst] "+r" (dst), [src1] "+r" (src1), [src2] "+r" (src2), [count] "+r" (count)
            : [k1] "m"(k1), [k2] "m"(k2)
            : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    #undef MIXADD2_CORE

    #define MIX3_CORE(MV_SRC1, MV_SRC2)   \
        __ASM_EMIT("sub         $0x08, %[count]")   \
        __ASM_EMIT("jb          2f")   \
        /* x8 blocks */ \
        __ASM_EMIT("1:") \
        __ASM_EMIT("movaps      0x00(%[dst]), %%xmm3")   \
        __ASM_EMIT("movaps      0x10(%[dst]), %%xmm4")   \
        __ASM_EMIT(MV_SRC1 "    0x00(%[src1]), %%xmm5")   \
        __ASM_EMIT(MV_SRC1 "    0x10(%[src1]), %%xmm6")   \
        __ASM_EMIT("mulps       %%xmm0, %%xmm3") \
        __ASM_EMIT("mulps       %%xmm1, %%xmm5") \
        __ASM_EMIT("mulps       %%xmm0, %%xmm4") \
        __ASM_EMIT("mulps       %%xmm1, %%xmm6") \
        __ASM_EMIT("addps       %%xmm5, %%xmm3") \
        __ASM_EMIT("addps       %%xmm6, %%xmm4") \
        __ASM_EMIT(MV_SRC2 "    0x00(%[src2]), %%xmm5")   \
        __ASM_EMIT(MV_SRC2 "    0x10(%[src2]), %%xmm6")   \
        __ASM_EMIT("mulps       %%xmm2, %%xmm5") \
        __ASM_EMIT("mulps       %%xmm2, %%xmm6") \
        __ASM_EMIT("addps       %%xmm5, %%xmm3") \
        __ASM_EMIT("addps       %%xmm6, %%xmm4") \
        __ASM_EMIT("movaps      %%xmm3, 0x00(%[dst])") \
        __ASM_EMIT("movaps      %%xmm4, 0x10(%[dst])") \
        __ASM_EMIT("add         $0x20, %[src1]") \
        __ASM_EMIT("add         $0x20, %[src2]") \
        __ASM_EMIT("add         $0x20, %[dst]") \
        __ASM_EMIT("sub         $0x08, %[count]") \
        __ASM_EMIT("jae         1b") \
        /* x4 block */ \
        __ASM_EMIT("2:") \
        __ASM_EMIT("add         $0x08, %[count]") \
        __ASM_EMIT("test        $0x04, %[count]")   \
        __ASM_EMIT("jz          3f")   \
        __ASM_EMIT("movaps      0x00(%[dst]), %%xmm3")   \
        __ASM_EMIT(MV_SRC1 "    0x00(%[src1]), %%xmm4")   \
        __ASM_EMIT(MV_SRC2 "    0x00(%[src2]), %%xmm5")   \
        __ASM_EMIT("mulps       %%xmm0, %%xmm3") \
        __ASM_EMIT("mulps       %%xmm1, %%xmm4") \
        __ASM_EMIT("mulps       %%xmm2, %%xmm5") \
        __ASM_EMIT("addps       %%xmm3, %%xmm4") \
        __ASM_EMIT("addps       %%xmm4, %%xmm5") \
        __ASM_EMIT("movaps      %%xmm5, 0x00(%[dst])") \
        __ASM_EMIT("add         $0x10, %[src1]") \
        __ASM_EMIT("add         $0x10, %[src2]") \
        __ASM_EMIT("add         $0x10, %[dst]") \
        /* x1 blocks */ \
        __ASM_EMIT("3:") \
        __ASM_EMIT("and         $0x03, %[count]") \
        __ASM_EMIT("jz          2000f") \
        __ASM_EMIT("4:") \
        __ASM_EMIT("movss       0x00(%[dst]), %%xmm3")   \
        __ASM_EMIT("movss       0x00(%[src1]), %%xmm4")   \
        __ASM_EMIT("movss       0x00(%[src2]), %%xmm5")   \
        __ASM_EMIT("mulss       %%xmm0, %%xmm3") \
        __ASM_EMIT("mulss       %%xmm1, %%xmm4") \
        __ASM_EMIT("mulss       %%xmm2, %%xmm5") \
        __ASM_EMIT("addss       %%xmm3, %%xmm4") \
        __ASM_EMIT("addss       %%xmm4, %%xmm5") \
        __ASM_EMIT("movss       %%xmm5, 0x00(%[dst])") \
        __ASM_EMIT("add         $0x04, %[src1]") \
        __ASM_EMIT("add         $0x04, %[src2]") \
        __ASM_EMIT("add         $0x04, %[dst]") \
        __ASM_EMIT("dec         %[count]") \
        __ASM_EMIT("jnz         4b")

    void mix3(float *dst, const float *src1, const float *src2, float k1, float k2, float k3, size_t count)
    {
        ARCH_X86_ASM
        (
            __ASM_EMIT("test        %[count], %[count]")
            __ASM_EMIT("jz          2000f")

            __ASM_EMIT("movss       %[k1], %%xmm0")
            __ASM_EMIT("movss       %[k2], %%xmm1")
            __ASM_EMIT("movss       %[k3], %%xmm2")
            __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0")
            __ASM_EMIT("shufps      $0x00, %%xmm1, %%xmm1")
            __ASM_EMIT("shufps      $0x00, %%xmm2, %%xmm2")

            // Align dst
            __ASM_EMIT("1:")
            __ASM_EMIT("test        $0x0f, %[dst]")
            __ASM_EMIT("jz          2f")
            __ASM_EMIT("movss       0x00(%[dst]), %%xmm3")
            __ASM_EMIT("movss       0x00(%[src1]), %%xmm4")
            __ASM_EMIT("movss       0x00(%[src2]), %%xmm5")
            __ASM_EMIT("mulss       %%xmm0, %%xmm3")
            __ASM_EMIT("mulss       %%xmm1, %%xmm4")
            __ASM_EMIT("mulss       %%xmm2, %%xmm5")
            __ASM_EMIT("addss       %%xmm3, %%xmm4")
            __ASM_EMIT("addss       %%xmm4, %%xmm5")
            __ASM_EMIT("movss       %%xmm5, 0x00(%[dst])")
            __ASM_EMIT("add         $0x04, %[src1]")
            __ASM_EMIT("add         $0x04, %[src2]")
            __ASM_EMIT("add         $0x04, %[dst]")
            __ASM_EMIT("dec         %[count]")
            __ASM_EMIT("jnz         1b")
            __ASM_EMIT("jmp         2000f")

            // Do main function
            __ASM_EMIT("2:")
            __ASM_EMIT("test        $0x0f, %[src1]")
            __ASM_EMIT("jnz         1010f")
                __ASM_EMIT("test        $0x0f, %[src2]")
                __ASM_EMIT("jnz         1001f")
                    MIX3_CORE("movaps", "movaps")
                    __ASM_EMIT("jmp         2000f")
                __ASM_EMIT("1001:")
                    MIX3_CORE("movaps", "movups")
                    __ASM_EMIT("jmp         2000f")
            __ASM_EMIT("1010:")
                __ASM_EMIT("test        $0x0f, %[src2]")
                __ASM_EMIT("jnz         1011f")
                    MIX3_CORE("movups", "movaps")
                    __ASM_EMIT("jmp         2000f")
                __ASM_EMIT("1011:")
                    MIX3_CORE("movups", "movups")

            __ASM_EMIT("2000:")

            : [dst] "+r" (dst), [src1] "+r" (src1), [src2] "+r" (src2), [count] "+r" (count)
            : [k1] "m"(k1), [k2] "m"(k2), [k3] "m"(k3)
            : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    #undef MIX3_CORE

    #define MIXMOVE3_CORE(MV_SRC1, MV_SRC2, MV_SRC3)   \
        __ASM_EMIT("sub         $0x08, %[count]")   \
        __ASM_EMIT("jb          2f")   \
        /* x8 blocks */ \
        __ASM_EMIT("1:") \
        __ASM_EMIT(MV_SRC1 "    0x00(%[src1]), %%xmm3")   \
        __ASM_EMIT(MV_SRC1 "    0x10(%[src1]), %%xmm4")   \
        __ASM_EMIT(MV_SRC2 "    0x00(%[src2]), %%xmm5")   \
        __ASM_EMIT(MV_SRC2 "    0x10(%[src2]), %%xmm6")   \
        __ASM_EMIT("mulps       %%xmm0, %%xmm3") \
        __ASM_EMIT("mulps       %%xmm1, %%xmm5") \
        __ASM_EMIT("mulps       %%xmm0, %%xmm4") \
        __ASM_EMIT("mulps       %%xmm1, %%xmm6") \
        __ASM_EMIT("addps       %%xmm5, %%xmm3") \
        __ASM_EMIT("addps       %%xmm6, %%xmm4") \
        __ASM_EMIT(MV_SRC3 "    0x00(%[src3]), %%xmm5")   \
        __ASM_EMIT(MV_SRC3 "    0x10(%[src3]), %%xmm6")   \
        __ASM_EMIT("mulps       %%xmm2, %%xmm5") \
        __ASM_EMIT("mulps       %%xmm2, %%xmm6") \
        __ASM_EMIT("addps       %%xmm5, %%xmm3") \
        __ASM_EMIT("addps       %%xmm6, %%xmm4") \
        __ASM_EMIT("movaps      %%xmm3, 0x00(%[dst])") \
        __ASM_EMIT("movaps      %%xmm4, 0x10(%[dst])") \
        __ASM_EMIT("add         $0x20, %[src1]") \
        __ASM_EMIT("add         $0x20, %[src2]") \
        __ASM_EMIT("add         $0x20, %[src3]") \
        __ASM_EMIT("add         $0x20, %[dst]") \
        __ASM_EMIT("sub         $0x08, %[count]") \
        __ASM_EMIT("jae         1b") \
        /* x4 block */ \
        __ASM_EMIT("2:") \
        __ASM_EMIT("add         $0x08, %[count]") \
        __ASM_EMIT("test        $0x04, %[count]")   \
        __ASM_EMIT("jz          3f")   \
        __ASM_EMIT(MV_SRC1 "    0x00(%[src1]), %%xmm3")   \
        __ASM_EMIT(MV_SRC2 "    0x00(%[src2]), %%xmm4")   \
        __ASM_EMIT(MV_SRC3 "    0x00(%[src3]), %%xmm5")   \
        __ASM_EMIT("mulps       %%xmm0, %%xmm3") \
        __ASM_EMIT("mulps       %%xmm1, %%xmm4") \
        __ASM_EMIT("mulps       %%xmm2, %%xmm5") \
        __ASM_EMIT("addps       %%xmm3, %%xmm4") \
        __ASM_EMIT("addps       %%xmm4, %%xmm5") \
        __ASM_EMIT("movaps      %%xmm5, 0x00(%[dst])") \
        __ASM_EMIT("add         $0x10, %[src1]") \
        __ASM_EMIT("add         $0x10, %[src2]") \
        __ASM_EMIT("add         $0x10, %[src3]") \
        __ASM_EMIT("add         $0x10, %[dst]") \
        /* x1 blocks */ \
        __ASM_EMIT("3:") \
        __ASM_EMIT("and         $0x03, %[count]") \
        __ASM_EMIT("jz          2000f") \
        __ASM_EMIT("4:") \
        __ASM_EMIT("movss       0x00(%[src1]), %%xmm3")   \
        __ASM_EMIT("movss       0x00(%[src2]), %%xmm4")   \
        __ASM_EMIT("movss       0x00(%[src3]), %%xmm5")   \
        __ASM_EMIT("mulss       %%xmm0, %%xmm3") \
        __ASM_EMIT("mulss       %%xmm1, %%xmm4") \
        __ASM_EMIT("mulss       %%xmm2, %%xmm5") \
        __ASM_EMIT("addss       %%xmm3, %%xmm4") \
        __ASM_EMIT("addss       %%xmm4, %%xmm5") \
        __ASM_EMIT("movss       %%xmm5, 0x00(%[dst])") \
        __ASM_EMIT("add         $0x04, %[src1]") \
        __ASM_EMIT("add         $0x04, %[src2]") \
        __ASM_EMIT("add         $0x04, %[src3]") \
        __ASM_EMIT("add         $0x04, %[dst]") \
        __ASM_EMIT("dec         %[count]") \
        __ASM_EMIT("jnz         4b")

    void mix_copy3(float *dst, const float *src1, const float *src2, const float *src3, float k1, float k2, float k3, size_t count)
    {
        ARCH_X86_ASM
        (
            __ASM_EMIT("test        %[count], %[count]")
            __ASM_EMIT("jz          2000f")

            __ASM_EMIT("movss       %[k1], %%xmm0")
            __ASM_EMIT("movss       %[k2], %%xmm1")
            __ASM_EMIT("movss       %[k3], %%xmm2")
            __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0")
            __ASM_EMIT("shufps      $0x00, %%xmm1, %%xmm1")
            __ASM_EMIT("shufps      $0x00, %%xmm2, %%xmm2")

            // Align dst
            __ASM_EMIT("1:")
            __ASM_EMIT("test        $0x0f, %[dst]")
            __ASM_EMIT("jz          2f")
            __ASM_EMIT("movss       0x00(%[src1]), %%xmm3")
            __ASM_EMIT("movss       0x00(%[src2]), %%xmm4")
            __ASM_EMIT("movss       0x00(%[src3]), %%xmm5")
            __ASM_EMIT("mulss       %%xmm0, %%xmm3")
            __ASM_EMIT("mulss       %%xmm1, %%xmm4")
            __ASM_EMIT("mulss       %%xmm2, %%xmm5")
            __ASM_EMIT("addss       %%xmm3, %%xmm4")
            __ASM_EMIT("addss       %%xmm4, %%xmm5")
            __ASM_EMIT("movss       %%xmm5, 0x00(%[dst])")
            __ASM_EMIT("add         $0x04, %[src1]")
            __ASM_EMIT("add         $0x04, %[src2]")
            __ASM_EMIT("add         $0x04, %[src3]")
            __ASM_EMIT("add         $0x04, %[dst]")
            __ASM_EMIT("dec         %[count]")
            __ASM_EMIT("jnz         1b")
            __ASM_EMIT("jmp         2000f")

            // Do main function
            __ASM_EMIT("2:")
            __ASM_EMIT("test        $0x0f, %[src1]")
            __ASM_EMIT("jnz         10100f")
                __ASM_EMIT("test        $0x0f, %[src2]")
                __ASM_EMIT("jnz         10010f")
                    __ASM_EMIT("test        $0x0f, %[src3]")
                    __ASM_EMIT("jnz         10001f")
                        MIXMOVE3_CORE("movaps", "movaps", "movaps")
                        __ASM_EMIT("jmp         2000f")
                    __ASM_EMIT("10001:")
                        MIXMOVE3_CORE("movaps", "movaps", "movups")
                        __ASM_EMIT("jmp         2000f")
                __ASM_EMIT("10010:")
                    __ASM_EMIT("test        $0x0f, %[src3]")
                    __ASM_EMIT("jnz         10011f")
                        MIXMOVE3_CORE("movaps", "movups", "movaps")
                        __ASM_EMIT("jmp         2000f")
                    __ASM_EMIT("10011:")
                        MIXMOVE3_CORE("movaps", "movups", "movups")
                        __ASM_EMIT("jmp         2000f")

            __ASM_EMIT("10100:")
                __ASM_EMIT("test        $0x0f, %[src2]")
                __ASM_EMIT("jnz         10110f")
                    __ASM_EMIT("test        $0x0f, %[src3]")
                    __ASM_EMIT("jnz         10101f")
                        MIXMOVE3_CORE("movups", "movaps", "movaps")
                        __ASM_EMIT("jmp         2000f")
                    __ASM_EMIT("10101:")
                        MIXMOVE3_CORE("movups", "movaps", "movups")
                        __ASM_EMIT("jmp         2000f")
                __ASM_EMIT("10110:")
                    __ASM_EMIT("test        $0x0f, %[src3]")
                    __ASM_EMIT("jnz         10111f")
                        MIXMOVE3_CORE("movups", "movups", "movaps")
                        __ASM_EMIT("jmp         2000f")
                    __ASM_EMIT("10111:")
                        MIXMOVE3_CORE("movups", "movups", "movups")
                        __ASM_EMIT("jmp         2000f")

            __ASM_EMIT("2000:")

            : [dst] "+r" (dst), [src1] "+r" (src1), [src2] "+r" (src2), [src3] "+r" (src3), [count] "+r" (count)
            : [k1] "m"(k1), [k2] "m"(k2), [k3] "m"(k3)
            : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    #undef MIXMOVE3_CORE

    #define MIXADD3_CORE(MV_SRC1, MV_SRC2, MV_SRC3)   \
        __ASM_EMIT("sub         $0x08, %[count]")   \
        __ASM_EMIT("jb          2f")   \
        /* x8 blocks */ \
        __ASM_EMIT("1:") \
        __ASM_EMIT(MV_SRC1 "    0x00(%[src1]), %%xmm3")   \
        __ASM_EMIT(MV_SRC1 "    0x10(%[src1]), %%xmm4")   \
        __ASM_EMIT(MV_SRC2 "    0x00(%[src2]), %%xmm5")   \
        __ASM_EMIT(MV_SRC2 "    0x10(%[src2]), %%xmm6")   \
        __ASM_EMIT("mulps       %%xmm0, %%xmm3") \
        __ASM_EMIT("mulps       %%xmm1, %%xmm5") \
        __ASM_EMIT("mulps       %%xmm0, %%xmm4") \
        __ASM_EMIT("mulps       %%xmm1, %%xmm6") \
        __ASM_EMIT("addps       %%xmm5, %%xmm3") \
        __ASM_EMIT("addps       %%xmm6, %%xmm4") \
        __ASM_EMIT(MV_SRC3 "    0x00(%[src3]), %%xmm5")   \
        __ASM_EMIT(MV_SRC3 "    0x10(%[src3]), %%xmm6")   \
        __ASM_EMIT("mulps       %%xmm2, %%xmm5") \
        __ASM_EMIT("mulps       %%xmm2, %%xmm6") \
        __ASM_EMIT("addps       %%xmm5, %%xmm3") \
        __ASM_EMIT("addps       %%xmm6, %%xmm4") \
        __ASM_EMIT("movaps      0x00(%[dst]), %%xmm5") \
        __ASM_EMIT("movaps      0x10(%[dst]), %%xmm6") \
        __ASM_EMIT("addps       %%xmm3, %%xmm5") \
        __ASM_EMIT("addps       %%xmm4, %%xmm6") \
        __ASM_EMIT("movaps      %%xmm5, 0x00(%[dst])") \
        __ASM_EMIT("movaps      %%xmm6, 0x10(%[dst])") \
        __ASM_EMIT("add         $0x20, %[src1]") \
        __ASM_EMIT("add         $0x20, %[src2]") \
        __ASM_EMIT("add         $0x20, %[src3]") \
        __ASM_EMIT("add         $0x20, %[dst]") \
        __ASM_EMIT("sub         $0x08, %[count]") \
        __ASM_EMIT("jae         1b") \
        /* x4 block */ \
        __ASM_EMIT("2:") \
        __ASM_EMIT("add         $0x08, %[count]") \
        __ASM_EMIT("test        $0x04, %[count]")   \
        __ASM_EMIT("jz          3f")   \
        __ASM_EMIT(MV_SRC1 "    0x00(%[src1]), %%xmm3")   \
        __ASM_EMIT(MV_SRC2 "    0x00(%[src2]), %%xmm4")   \
        __ASM_EMIT(MV_SRC3 "    0x00(%[src3]), %%xmm5")   \
        __ASM_EMIT("mulps       %%xmm0, %%xmm3") \
        __ASM_EMIT("mulps       %%xmm1, %%xmm4") \
        __ASM_EMIT("mulps       %%xmm2, %%xmm5") \
        __ASM_EMIT("addps       %%xmm3, %%xmm4") \
        __ASM_EMIT("addps       %%xmm4, %%xmm5") \
        __ASM_EMIT("movaps      0x00(%[dst]), %%xmm4") \
        __ASM_EMIT("addps       %%xmm5, %%xmm4") \
        __ASM_EMIT("movaps      %%xmm4, 0x00(%[dst])") \
        __ASM_EMIT("add         $0x10, %[src1]") \
        __ASM_EMIT("add         $0x10, %[src2]") \
        __ASM_EMIT("add         $0x10, %[src3]") \
        __ASM_EMIT("add         $0x10, %[dst]") \
        /* x1 blocks */ \
        __ASM_EMIT("3:") \
        __ASM_EMIT("and         $0x03, %[count]") \
        __ASM_EMIT("jz          2000f") \
        __ASM_EMIT("4:") \
        __ASM_EMIT("movss       0x00(%[src1]), %%xmm3")   \
        __ASM_EMIT("movss       0x00(%[src2]), %%xmm4")   \
        __ASM_EMIT("movss       0x00(%[src3]), %%xmm5")   \
        __ASM_EMIT("mulss       %%xmm0, %%xmm3") \
        __ASM_EMIT("mulss       %%xmm1, %%xmm4") \
        __ASM_EMIT("mulss       %%xmm2, %%xmm5") \
        __ASM_EMIT("addss       %%xmm3, %%xmm4") \
        __ASM_EMIT("addss       %%xmm4, %%xmm5") \
        __ASM_EMIT("movss       0x00(%[dst]), %%xmm4") \
        __ASM_EMIT("addss       %%xmm5, %%xmm4") \
        __ASM_EMIT("movss       %%xmm4, 0x00(%[dst])") \
        __ASM_EMIT("add         $0x04, %[src1]") \
        __ASM_EMIT("add         $0x04, %[src2]") \
        __ASM_EMIT("add         $0x04, %[src3]") \
        __ASM_EMIT("add         $0x04, %[dst]") \
        __ASM_EMIT("dec         %[count]") \
        __ASM_EMIT("jnz         4b")

    void mix_add3(float *dst, const float *src1, const float *src2, const float *src3, float k1, float k2, float k3, size_t count)
    {
        ARCH_X86_ASM
        (
            __ASM_EMIT("test        %[count], %[count]")
            __ASM_EMIT("jz          2000f")

            __ASM_EMIT("movss       %[k1], %%xmm0")
            __ASM_EMIT("movss       %[k2], %%xmm1")
            __ASM_EMIT("movss       %[k3], %%xmm2")
            __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0")
            __ASM_EMIT("shufps      $0x00, %%xmm1, %%xmm1")
            __ASM_EMIT("shufps      $0x00, %%xmm2, %%xmm2")

            // Align dst
            __ASM_EMIT("1:")
            __ASM_EMIT("test        $0x0f, %[dst]")
            __ASM_EMIT("jz          2f")
            __ASM_EMIT("movss       0x00(%[src1]), %%xmm3")
            __ASM_EMIT("movss       0x00(%[src2]), %%xmm4")
            __ASM_EMIT("movss       0x00(%[src3]), %%xmm5")
            __ASM_EMIT("mulss       %%xmm0, %%xmm3")
            __ASM_EMIT("mulss       %%xmm1, %%xmm4")
            __ASM_EMIT("mulss       %%xmm2, %%xmm5")
            __ASM_EMIT("addss       %%xmm3, %%xmm4")
            __ASM_EMIT("addss       %%xmm4, %%xmm5")
            __ASM_EMIT("movss       0x00(%[dst]), %%xmm4")
            __ASM_EMIT("addss       %%xmm5, %%xmm4")
            __ASM_EMIT("movss       %%xmm4, 0x00(%[dst])")
            __ASM_EMIT("add         $0x04, %[src1]")
            __ASM_EMIT("add         $0x04, %[src2]")
            __ASM_EMIT("add         $0x04, %[src3]")
            __ASM_EMIT("add         $0x04, %[dst]")
            __ASM_EMIT("dec         %[count]")
            __ASM_EMIT("jnz         1b")
            __ASM_EMIT("jmp         2000f")

            // Do main function
            __ASM_EMIT("2:")
            __ASM_EMIT("test        $0x0f, %[src1]")
            __ASM_EMIT("jnz         10100f")
                __ASM_EMIT("test        $0x0f, %[src2]")
                __ASM_EMIT("jnz         10010f")
                    __ASM_EMIT("test        $0x0f, %[src3]")
                    __ASM_EMIT("jnz         10001f")
                        MIXADD3_CORE("movaps", "movaps", "movaps")
                        __ASM_EMIT("jmp         2000f")
                    __ASM_EMIT("10001:")
                        MIXADD3_CORE("movaps", "movaps", "movups")
                        __ASM_EMIT("jmp         2000f")
                __ASM_EMIT("10010:")
                    __ASM_EMIT("test        $0x0f, %[src3]")
                    __ASM_EMIT("jnz         10011f")
                        MIXADD3_CORE("movaps", "movups", "movaps")
                        __ASM_EMIT("jmp         2000f")
                    __ASM_EMIT("10011:")
                        MIXADD3_CORE("movaps", "movups", "movups")
                        __ASM_EMIT("jmp         2000f")

            __ASM_EMIT("10100:")
                __ASM_EMIT("test        $0x0f, %[src2]")
                __ASM_EMIT("jnz         10110f")
                    __ASM_EMIT("test        $0x0f, %[src3]")
                    __ASM_EMIT("jnz         10101f")
                        MIXADD3_CORE("movups", "movaps", "movaps")
                        __ASM_EMIT("jmp         2000f")
                    __ASM_EMIT("10101:")
                        MIXADD3_CORE("movups", "movaps", "movups")
                        __ASM_EMIT("jmp         2000f")
                __ASM_EMIT("10110:")
                    __ASM_EMIT("test        $0x0f, %[src3]")
                    __ASM_EMIT("jnz         10111f")
                        MIXADD3_CORE("movups", "movups", "movaps")
                        __ASM_EMIT("jmp         2000f")
                    __ASM_EMIT("10111:")
                        MIXADD3_CORE("movups", "movups", "movups")
                        __ASM_EMIT("jmp         2000f")

            __ASM_EMIT("2000:")

            : [dst] "+r" (dst), [src1] "+r" (src1), [src2] "+r" (src2), [src3] "+r" (src3), [count] "+r" (count)
            : [k1] "m"(k1), [k2] "m"(k2), [k3] "m"(k3)
            : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    #undef MIXADD3_CORE

    #define MIX4_CORE(MV_SRC1, MV_SRC2, MV_SRC3)    \
        __ASM_EMIT("sub         $0x04, %[count]")   \
        __ASM_EMIT("jb          2f")   \
        /* x4 blocks */ \
        __ASM_EMIT("1:") \
        __ASM_EMIT("movaps      0x00(%[dst]), %%xmm4")   \
        __ASM_EMIT(MV_SRC1 "    0x00(%[src1]), %%xmm5")   \
        __ASM_EMIT(MV_SRC2 "    0x00(%[src2]), %%xmm6")   \
        __ASM_EMIT(MV_SRC3 "    0x00(%[src3]), %%xmm7")   \
        __ASM_EMIT("mulps       %%xmm0, %%xmm4") \
        __ASM_EMIT("mulps       %%xmm1, %%xmm5") \
        __ASM_EMIT("mulps       %%xmm2, %%xmm6") \
        __ASM_EMIT("mulps       %%xmm3, %%xmm7") \
        __ASM_EMIT("addps       %%xmm4, %%xmm6") \
        __ASM_EMIT("addps       %%xmm5, %%xmm7") \
        __ASM_EMIT("addps       %%xmm6, %%xmm7") \
        __ASM_EMIT("movaps      %%xmm7, 0x00(%[dst])") \
        __ASM_EMIT("add         $0x10, %[src1]") \
        __ASM_EMIT("add         $0x10, %[src2]") \
        __ASM_EMIT("add         $0x10, %[src3]") \
        __ASM_EMIT("add         $0x10, %[dst]") \
        __ASM_EMIT("sub         $0x04, %[count]") \
        __ASM_EMIT("jae         1b") \
        /* x1 blocks */ \
        __ASM_EMIT("2:") \
        __ASM_EMIT("add         $0x04, %[count]") \
        __ASM_EMIT("and         $0x03, %[count]")   \
        __ASM_EMIT("jz          2000f")   \
        __ASM_EMIT("3:") \
        __ASM_EMIT("movss       0x00(%[dst]), %%xmm4")   \
        __ASM_EMIT("movss       0x00(%[src1]), %%xmm5")   \
        __ASM_EMIT("movss       0x00(%[src2]), %%xmm6")   \
        __ASM_EMIT("movss       0x00(%[src3]), %%xmm7")   \
        __ASM_EMIT("mulss       %%xmm0, %%xmm4") \
        __ASM_EMIT("mulss       %%xmm1, %%xmm5") \
        __ASM_EMIT("mulss       %%xmm2, %%xmm6") \
        __ASM_EMIT("mulss       %%xmm3, %%xmm7") \
        __ASM_EMIT("addss       %%xmm4, %%xmm6") \
        __ASM_EMIT("addss       %%xmm5, %%xmm7") \
        __ASM_EMIT("addss       %%xmm6, %%xmm7") \
        __ASM_EMIT("movss       %%xmm7, 0x00(%[dst])") \
        __ASM_EMIT("add         $0x04, %[src1]") \
        __ASM_EMIT("add         $0x04, %[src2]") \
        __ASM_EMIT("add         $0x04, %[src3]") \
        __ASM_EMIT("add         $0x04, %[dst]") \
        __ASM_EMIT("dec         %[count]") \
        __ASM_EMIT("jnz         3b")

    void mix4(float *dst, const float *src1, const float *src2, const float *src3, float k1, float k2, float k3, float k4, size_t count)
    {
        ARCH_X86_ASM
        (
            __ASM_EMIT("test        %[count], %[count]")
            __ASM_EMIT("jz          2000f")

            __ASM_EMIT("movss       %[k1], %%xmm0")
            __ASM_EMIT("movss       %[k2], %%xmm1")
            __ASM_EMIT("movss       %[k3], %%xmm2")
            __ASM_EMIT("movss       %[k4], %%xmm3")
            __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0")
            __ASM_EMIT("shufps      $0x00, %%xmm1, %%xmm1")
            __ASM_EMIT("shufps      $0x00, %%xmm2, %%xmm2")
            __ASM_EMIT("shufps      $0x00, %%xmm3, %%xmm3")

            // Align dst
            __ASM_EMIT("1:")
            __ASM_EMIT("test        $0x0f, %[dst]")
            __ASM_EMIT("jz          2f")
            __ASM_EMIT("movss       0x00(%[dst]), %%xmm4")
            __ASM_EMIT("movss       0x00(%[src1]), %%xmm5")
            __ASM_EMIT("movss       0x00(%[src2]), %%xmm6")
            __ASM_EMIT("movss       0x00(%[src3]), %%xmm7")
            __ASM_EMIT("mulss       %%xmm0, %%xmm4")
            __ASM_EMIT("mulss       %%xmm1, %%xmm5")
            __ASM_EMIT("mulss       %%xmm2, %%xmm6")
            __ASM_EMIT("mulss       %%xmm3, %%xmm7")
            __ASM_EMIT("addss       %%xmm4, %%xmm6")
            __ASM_EMIT("addss       %%xmm5, %%xmm7")
            __ASM_EMIT("addss       %%xmm6, %%xmm7")
            __ASM_EMIT("movss       %%xmm7, 0x00(%[dst])")
            __ASM_EMIT("add         $0x04, %[src1]")
            __ASM_EMIT("add         $0x04, %[src2]")
            __ASM_EMIT("add         $0x04, %[src3]")
            __ASM_EMIT("add         $0x04, %[dst]")
            __ASM_EMIT("dec         %[count]")
            __ASM_EMIT("jnz         1b")
            __ASM_EMIT("jmp         2000f")

            // Do main function
            __ASM_EMIT("2:")
            __ASM_EMIT("test        $0x0f, %[src1]")
            __ASM_EMIT("jnz         1100f")
                __ASM_EMIT("test        $0x0f, %[src2]")
                __ASM_EMIT("jnz         1010f")
                    __ASM_EMIT("test        $0x0f, %[src3]")
                    __ASM_EMIT("jnz         1001f")
                        MIX4_CORE("movaps", "movaps", "movaps")
                        __ASM_EMIT("jmp         2000f")
                    __ASM_EMIT("1001:")
                        MIX4_CORE("movaps", "movaps", "movups")
                        __ASM_EMIT("jmp         2000f")
                __ASM_EMIT("1010:")
                    __ASM_EMIT("test        $0x0f, %[src3]")
                    __ASM_EMIT("jnz         1011f")
                        MIX4_CORE("movaps", "movups", "movaps")
                        __ASM_EMIT("jmp         2000f")
                    __ASM_EMIT("1011:")
                        MIX4_CORE("movaps", "movups", "movups")
                        __ASM_EMIT("jmp         2000f")
            __ASM_EMIT("1100:")
                __ASM_EMIT("test        $0x0f, %[src2]")
                __ASM_EMIT("jnz         1110f")
                    __ASM_EMIT("test        $0x0f, %[src3]")
                    __ASM_EMIT("jnz         1101f")
                        MIX4_CORE("movups", "movaps", "movaps")
                        __ASM_EMIT("jmp         2000f")
                    __ASM_EMIT("1101:")
                        MIX4_CORE("movups", "movaps", "movups")
                        __ASM_EMIT("jmp         2000f")
                __ASM_EMIT("1110:")
                    __ASM_EMIT("test        $0x0f, %[src3]")
                    __ASM_EMIT("jnz         1111f")
                        MIX4_CORE("movups", "movups", "movaps")
                        __ASM_EMIT("jmp         2000f")
                    __ASM_EMIT("1111:")
                        MIX4_CORE("movups", "movups", "movups")

            __ASM_EMIT("2000:")

            : [dst] "+r" (dst), [src1] "+r" (src1), [src2] "+r" (src2), [src3] "+r" (src3), [count] "+r" (count)
            : [k1] "m"(k1), [k2] "m"(k2), [k3] "m"(k3), [k4] "m"(k4)
            : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    #undef MIX4_CORE

    #define MIXMOVE4_CORE(MV_SRC1, MV_SRC2, MV_SRC3, MV_SRC4)    \
        __ASM_EMIT64("sub       $0x04, %[count]")   \
        __ASM_EMIT32("subl      $0x04, %[count]")   \
        __ASM_EMIT("jb          2f")   \
        /* x4 blocks */ \
        __ASM_EMIT("1:") \
        __ASM_EMIT(MV_SRC1 "    0x00(%[src1]), %%xmm4")   \
        __ASM_EMIT(MV_SRC2 "    0x00(%[src2]), %%xmm5")   \
        __ASM_EMIT(MV_SRC3 "    0x00(%[src3]), %%xmm6")   \
        __ASM_EMIT(MV_SRC4 "    0x00(%[src4]), %%xmm7")   \
        __ASM_EMIT("mulps       %%xmm0, %%xmm4") \
        __ASM_EMIT("mulps       %%xmm1, %%xmm5") \
        __ASM_EMIT("mulps       %%xmm2, %%xmm6") \
        __ASM_EMIT("mulps       %%xmm3, %%xmm7") \
        __ASM_EMIT("addps       %%xmm4, %%xmm6") \
        __ASM_EMIT("addps       %%xmm5, %%xmm7") \
        __ASM_EMIT("addps       %%xmm6, %%xmm7") \
        __ASM_EMIT("movaps      %%xmm7, 0x00(%[dst])") \
        __ASM_EMIT("add         $0x10, %[src1]") \
        __ASM_EMIT("add         $0x10, %[src2]") \
        __ASM_EMIT("add         $0x10, %[src3]") \
        __ASM_EMIT("add         $0x10, %[src4]") \
        __ASM_EMIT("add         $0x10, %[dst]") \
        __ASM_EMIT64("sub       $0x04, %[count]") \
        __ASM_EMIT32("subl      $0x04, %[count]") \
        __ASM_EMIT("jae         1b") \
        /* x1 blocks */ \
        __ASM_EMIT("2:") \
        __ASM_EMIT64("add       $0x04, %[count]") \
        __ASM_EMIT32("addl      $0x04, %[count]") \
        __ASM_EMIT64("and       $0x03, %[count]")   \
        __ASM_EMIT32("andl      $0x03, %[count]")   \
        __ASM_EMIT("jz          2000f")   \
        __ASM_EMIT("3:") \
        __ASM_EMIT("movss       0x00(%[src1]), %%xmm4")   \
        __ASM_EMIT("movss       0x00(%[src2]), %%xmm5")   \
        __ASM_EMIT("movss       0x00(%[src3]), %%xmm6")   \
        __ASM_EMIT("movss       0x00(%[src4]), %%xmm7")   \
        __ASM_EMIT("mulss       %%xmm0, %%xmm4") \
        __ASM_EMIT("mulss       %%xmm1, %%xmm5") \
        __ASM_EMIT("mulss       %%xmm2, %%xmm6") \
        __ASM_EMIT("mulss       %%xmm3, %%xmm7") \
        __ASM_EMIT("addss       %%xmm4, %%xmm6") \
        __ASM_EMIT("addss       %%xmm5, %%xmm7") \
        __ASM_EMIT("addss       %%xmm6, %%xmm7") \
        __ASM_EMIT("movss       %%xmm7, 0x00(%[dst])") \
        __ASM_EMIT("add         $0x04, %[src1]") \
        __ASM_EMIT("add         $0x04, %[src2]") \
        __ASM_EMIT("add         $0x04, %[src3]") \
        __ASM_EMIT("add         $0x04, %[src4]") \
        __ASM_EMIT("add         $0x04, %[dst]") \
        __ASM_EMIT64("dec       %[count]") \
        __ASM_EMIT32("decl      %[count]") \
        __ASM_EMIT("jnz         3b")

    void mix_copy4(float *dst, const float *src1, const float *src2, const float *src3, const float *src4, float k1, float k2, float k3, float k4, size_t count)
    {
        ARCH_X86_ASM
        (
            __ASM_EMIT64("test      %[count], %[count]")
            __ASM_EMIT32("cmpl      $0, %[count]")
            __ASM_EMIT("jz          2000f")

            __ASM_EMIT("movss       %[k1], %%xmm0")
            __ASM_EMIT("movss       %[k2], %%xmm1")
            __ASM_EMIT("movss       %[k3], %%xmm2")
            __ASM_EMIT("movss       %[k4], %%xmm3")
            __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0")
            __ASM_EMIT("shufps      $0x00, %%xmm1, %%xmm1")
            __ASM_EMIT("shufps      $0x00, %%xmm2, %%xmm2")
            __ASM_EMIT("shufps      $0x00, %%xmm3, %%xmm3")

            // Align dst
            __ASM_EMIT("1:")
            __ASM_EMIT("test        $0x0f, %[dst]")
            __ASM_EMIT("jz          2f")
            __ASM_EMIT("movss       0x00(%[src1]), %%xmm4")
            __ASM_EMIT("movss       0x00(%[src2]), %%xmm5")
            __ASM_EMIT("movss       0x00(%[src3]), %%xmm6")
            __ASM_EMIT("movss       0x00(%[src4]), %%xmm7")
            __ASM_EMIT("mulss       %%xmm0, %%xmm4")
            __ASM_EMIT("mulss       %%xmm1, %%xmm5")
            __ASM_EMIT("mulss       %%xmm2, %%xmm6")
            __ASM_EMIT("mulss       %%xmm3, %%xmm7")
            __ASM_EMIT("addss       %%xmm4, %%xmm6")
            __ASM_EMIT("addss       %%xmm5, %%xmm7")
            __ASM_EMIT("addss       %%xmm6, %%xmm7")
            __ASM_EMIT("movss       %%xmm7, 0x00(%[dst])")
            __ASM_EMIT("add         $0x04, %[src1]")
            __ASM_EMIT("add         $0x04, %[src2]")
            __ASM_EMIT("add         $0x04, %[src3]")
            __ASM_EMIT("add         $0x04, %[src4]")
            __ASM_EMIT("add         $0x04, %[dst]")
            __ASM_EMIT64("dec       %[count]")
            __ASM_EMIT32("decl      %[count]")
            __ASM_EMIT("jnz         1b")
            __ASM_EMIT("jmp         2000f")

            // Do main function
            __ASM_EMIT("2:")
            __ASM_EMIT("test        $0x0f, %[src1]")
            __ASM_EMIT("jnz         11000f")
                __ASM_EMIT("test        $0x0f, %[src2]")
                __ASM_EMIT("jnz         10100f")
                    __ASM_EMIT("test        $0x0f, %[src3]")
                    __ASM_EMIT("jnz         10010f")
                        __ASM_EMIT("test        $0x0f, %[src4]")
                        __ASM_EMIT("jnz         10001f")
                            MIXMOVE4_CORE("movaps", "movaps", "movaps", "movaps")
                            __ASM_EMIT("jmp         2000f")
                        __ASM_EMIT("10001:")
                            MIXMOVE4_CORE("movaps", "movaps", "movaps", "movups")
                            __ASM_EMIT("jmp         2000f")
                    __ASM_EMIT("10010:")
                        __ASM_EMIT("test        $0x0f, %[src4]")
                        __ASM_EMIT("jnz         10011f")
                            MIXMOVE4_CORE("movaps", "movaps", "movups", "movaps")
                            __ASM_EMIT("jmp         2000f")
                        __ASM_EMIT("10011:")
                            MIXMOVE4_CORE("movaps", "movaps", "movups", "movups")
                            __ASM_EMIT("jmp         2000f")

                __ASM_EMIT("10100:")
                    __ASM_EMIT("test        $0x0f, %[src3]")
                    __ASM_EMIT("jnz         10110f")
                        __ASM_EMIT("test        $0x0f, %[src4]")
                        __ASM_EMIT("jnz         10101f")
                            MIXMOVE4_CORE("movaps", "movups", "movaps", "movaps")
                            __ASM_EMIT("jmp         2000f")
                        __ASM_EMIT("10101:")
                            MIXMOVE4_CORE("movaps", "movups", "movaps", "movups")
                            __ASM_EMIT("jmp         2000f")
                    __ASM_EMIT("10110:")
                        __ASM_EMIT("test        $0x0f, %[src4]")
                        __ASM_EMIT("jnz         10111f")
                            MIXMOVE4_CORE("movaps", "movups", "movups", "movaps")
                            __ASM_EMIT("jmp         2000f")
                        __ASM_EMIT("10111:")
                            MIXMOVE4_CORE("movaps", "movups", "movups", "movups")
                            __ASM_EMIT("jmp         2000f")

             __ASM_EMIT("11000:")
                 __ASM_EMIT("test        $0x0f, %[src2]")
                 __ASM_EMIT("jnz         11100f")
                     __ASM_EMIT("test        $0x0f, %[src3]")
                     __ASM_EMIT("jnz         11010f")
                         __ASM_EMIT("test        $0x0f, %[src4]")
                         __ASM_EMIT("jnz         11001f")
                             MIXMOVE4_CORE("movups", "movaps", "movaps", "movaps")
                             __ASM_EMIT("jmp         2000f")
                         __ASM_EMIT("11001:")
                             MIXMOVE4_CORE("movups", "movaps", "movaps", "movups")
                             __ASM_EMIT("jmp         2000f")
                     __ASM_EMIT("11010:")
                         __ASM_EMIT("test        $0x0f, %[src4]")
                         __ASM_EMIT("jnz         11011f")
                             MIXMOVE4_CORE("movups", "movaps", "movups", "movaps")
                             __ASM_EMIT("jmp         2000f")
                         __ASM_EMIT("11011:")
                             MIXMOVE4_CORE("movups", "movaps", "movups", "movups")
                             __ASM_EMIT("jmp         2000f")

                 __ASM_EMIT("11100:")
                     __ASM_EMIT("test        $0x0f, %[src3]")
                     __ASM_EMIT("jnz         11110f")
                         __ASM_EMIT("test        $0x0f, %[src4]")
                         __ASM_EMIT("jnz         11101f")
                             MIXMOVE4_CORE("movups", "movups", "movaps", "movaps")
                             __ASM_EMIT("jmp         2000f")
                         __ASM_EMIT("11101:")
                             MIXMOVE4_CORE("movups", "movups", "movaps", "movups")
                             __ASM_EMIT("jmp         2000f")
                     __ASM_EMIT("11110:")
                         __ASM_EMIT("test        $0x0f, %[src4]")
                         __ASM_EMIT("jnz         11111f")
                             MIXMOVE4_CORE("movups", "movups", "movups", "movaps")
                             __ASM_EMIT("jmp         2000f")
                         __ASM_EMIT("11111:")
                             MIXMOVE4_CORE("movups", "movups", "movups", "movups")

            __ASM_EMIT("2000:")

            : [dst] "+r" (dst),
              [src1] "+r" (src1), [src2] "+r" (src2), [src3] "+r" (src3), [src4] "+r" (src4),
              [count] __ASM_ARG_RW(count)
            : [k1] "m"(k1), [k2] "m"(k2), [k3] "m"(k3), [k4] "m"(k4)
            : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    #undef MIXMOVE4_CORE

    #define MIXADD4_CORE(MV_SRC1, MV_SRC2, MV_SRC3, MV_SRC4)    \
        __ASM_EMIT64("sub       $0x04, %[count]")   \
        __ASM_EMIT32("subl      $0x04, %[count]")   \
        __ASM_EMIT("jb          2f")   \
        /* x4 blocks */ \
        __ASM_EMIT("1:") \
        __ASM_EMIT(MV_SRC1 "    0x00(%[src1]), %%xmm4")   \
        __ASM_EMIT(MV_SRC2 "    0x00(%[src2]), %%xmm5")   \
        __ASM_EMIT(MV_SRC3 "    0x00(%[src3]), %%xmm6")   \
        __ASM_EMIT(MV_SRC4 "    0x00(%[src4]), %%xmm7")   \
        __ASM_EMIT("mulps       %%xmm0, %%xmm4") \
        __ASM_EMIT("mulps       %%xmm1, %%xmm5") \
        __ASM_EMIT("mulps       %%xmm2, %%xmm6") \
        __ASM_EMIT("mulps       %%xmm3, %%xmm7") \
        __ASM_EMIT("addps       %%xmm4, %%xmm6") \
        __ASM_EMIT("addps       %%xmm5, %%xmm7") \
        __ASM_EMIT("addps       %%xmm6, %%xmm7") \
        __ASM_EMIT("movaps      0x00(%[dst]), %%xmm6") \
        __ASM_EMIT("addps       %%xmm7, %%xmm6") \
        __ASM_EMIT("movaps      %%xmm6, 0x00(%[dst])") \
        __ASM_EMIT("add         $0x10, %[src1]") \
        __ASM_EMIT("add         $0x10, %[src2]") \
        __ASM_EMIT("add         $0x10, %[src3]") \
        __ASM_EMIT("add         $0x10, %[src4]") \
        __ASM_EMIT("add         $0x10, %[dst]") \
        __ASM_EMIT64("sub       $0x04, %[count]") \
        __ASM_EMIT32("subl      $0x04, %[count]") \
        __ASM_EMIT("jae         1b") \
        /* x1 blocks */ \
        __ASM_EMIT("2:") \
        __ASM_EMIT64("add       $0x04, %[count]") \
        __ASM_EMIT32("addl      $0x04, %[count]") \
        __ASM_EMIT64("and       $0x03, %[count]")   \
        __ASM_EMIT32("andl      $0x03, %[count]")   \
        __ASM_EMIT("jz          2000f")   \
        __ASM_EMIT("3:") \
        __ASM_EMIT("movss       0x00(%[src1]), %%xmm4")   \
        __ASM_EMIT("movss       0x00(%[src2]), %%xmm5")   \
        __ASM_EMIT("movss       0x00(%[src3]), %%xmm6")   \
        __ASM_EMIT("movss       0x00(%[src4]), %%xmm7")   \
        __ASM_EMIT("mulss       %%xmm0, %%xmm4") \
        __ASM_EMIT("mulss       %%xmm1, %%xmm5") \
        __ASM_EMIT("mulss       %%xmm2, %%xmm6") \
        __ASM_EMIT("mulss       %%xmm3, %%xmm7") \
        __ASM_EMIT("addss       %%xmm4, %%xmm6") \
        __ASM_EMIT("addss       %%xmm5, %%xmm7") \
        __ASM_EMIT("addss       %%xmm6, %%xmm7") \
        __ASM_EMIT("movss       0x00(%[dst]), %%xmm6") \
        __ASM_EMIT("addss       %%xmm7, %%xmm6") \
        __ASM_EMIT("movss       %%xmm6, 0x00(%[dst])") \
        __ASM_EMIT("add         $0x04, %[src1]") \
        __ASM_EMIT("add         $0x04, %[src2]") \
        __ASM_EMIT("add         $0x04, %[src3]") \
        __ASM_EMIT("add         $0x04, %[src4]") \
        __ASM_EMIT("add         $0x04, %[dst]") \
        __ASM_EMIT64("dec       %[count]") \
        __ASM_EMIT32("decl      %[count]") \
        __ASM_EMIT("jnz         3b")

    void mix_add4(float *dst, const float *src1, const float *src2, const float *src3, const float *src4, float k1, float k2, float k3, float k4, size_t count)
    {
        ARCH_X86_ASM
        (
            __ASM_EMIT64("test      %[count], %[count]")
            __ASM_EMIT32("cmpl      $0, %[count]")
            __ASM_EMIT("jz          2000f")

            __ASM_EMIT("movss       %[k1], %%xmm0")
            __ASM_EMIT("movss       %[k2], %%xmm1")
            __ASM_EMIT("movss       %[k3], %%xmm2")
            __ASM_EMIT("movss       %[k4], %%xmm3")
            __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0")
            __ASM_EMIT("shufps      $0x00, %%xmm1, %%xmm1")
            __ASM_EMIT("shufps      $0x00, %%xmm2, %%xmm2")
            __ASM_EMIT("shufps      $0x00, %%xmm3, %%xmm3")

            // Align dst
            __ASM_EMIT("1:")
            __ASM_EMIT("test        $0x0f, %[dst]")
            __ASM_EMIT("jz          2f")
            __ASM_EMIT("movss       0x00(%[src1]), %%xmm4")
            __ASM_EMIT("movss       0x00(%[src2]), %%xmm5")
            __ASM_EMIT("movss       0x00(%[src3]), %%xmm6")
            __ASM_EMIT("movss       0x00(%[src4]), %%xmm7")
            __ASM_EMIT("mulss       %%xmm0, %%xmm4")
            __ASM_EMIT("mulss       %%xmm1, %%xmm5")
            __ASM_EMIT("mulss       %%xmm2, %%xmm6")
            __ASM_EMIT("mulss       %%xmm3, %%xmm7")
            __ASM_EMIT("addss       %%xmm4, %%xmm6")
            __ASM_EMIT("addss       %%xmm5, %%xmm7")
            __ASM_EMIT("addss       %%xmm6, %%xmm7")
            __ASM_EMIT("movss       0x00(%[dst]), %%xmm6")
            __ASM_EMIT("addss       %%xmm7, %%xmm6")
            __ASM_EMIT("movss       %%xmm6, 0x00(%[dst])")
            __ASM_EMIT("add         $0x04, %[src1]")
            __ASM_EMIT("add         $0x04, %[src2]")
            __ASM_EMIT("add         $0x04, %[src3]")
            __ASM_EMIT("add         $0x04, %[src4]")
            __ASM_EMIT("add         $0x04, %[dst]")
            __ASM_EMIT64("dec       %[count]")
            __ASM_EMIT32("decl      %[count]")
            __ASM_EMIT("jnz         1b")
            __ASM_EMIT("jmp         2000f")

            // Do main function
            __ASM_EMIT("2:")
            __ASM_EMIT("test        $0x0f, %[src1]")
            __ASM_EMIT("jnz         11000f")
                __ASM_EMIT("test        $0x0f, %[src2]")
                __ASM_EMIT("jnz         10100f")
                    __ASM_EMIT("test        $0x0f, %[src3]")
                    __ASM_EMIT("jnz         10010f")
                        __ASM_EMIT("test        $0x0f, %[src4]")
                        __ASM_EMIT("jnz         10001f")
                            MIXADD4_CORE("movaps", "movaps", "movaps", "movaps")
                            __ASM_EMIT("jmp         2000f")
                        __ASM_EMIT("10001:")
                            MIXADD4_CORE("movaps", "movaps", "movaps", "movups")
                            __ASM_EMIT("jmp         2000f")
                    __ASM_EMIT("10010:")
                        __ASM_EMIT("test        $0x0f, %[src4]")
                        __ASM_EMIT("jnz         10011f")
                            MIXADD4_CORE("movaps", "movaps", "movups", "movaps")
                            __ASM_EMIT("jmp         2000f")
                        __ASM_EMIT("10011:")
                            MIXADD4_CORE("movaps", "movaps", "movups", "movups")
                            __ASM_EMIT("jmp         2000f")

                __ASM_EMIT("10100:")
                    __ASM_EMIT("test        $0x0f, %[src3]")
                    __ASM_EMIT("jnz         10110f")
                        __ASM_EMIT("test        $0x0f, %[src4]")
                        __ASM_EMIT("jnz         10101f")
                            MIXADD4_CORE("movaps", "movups", "movaps", "movaps")
                            __ASM_EMIT("jmp         2000f")
                        __ASM_EMIT("10101:")
                            MIXADD4_CORE("movaps", "movups", "movaps", "movups")
                            __ASM_EMIT("jmp         2000f")
                    __ASM_EMIT("10110:")
                        __ASM_EMIT("test        $0x0f, %[src4]")
                        __ASM_EMIT("jnz         10111f")
                            MIXADD4_CORE("movaps", "movups", "movups", "movaps")
                            __ASM_EMIT("jmp         2000f")
                        __ASM_EMIT("10111:")
                            MIXADD4_CORE("movaps", "movups", "movups", "movups")
                            __ASM_EMIT("jmp         2000f")

             __ASM_EMIT("11000:")
                 __ASM_EMIT("test        $0x0f, %[src2]")
                 __ASM_EMIT("jnz         11100f")
                     __ASM_EMIT("test        $0x0f, %[src3]")
                     __ASM_EMIT("jnz         11010f")
                         __ASM_EMIT("test        $0x0f, %[src4]")
                         __ASM_EMIT("jnz         11001f")
                             MIXADD4_CORE("movups", "movaps", "movaps", "movaps")
                             __ASM_EMIT("jmp         2000f")
                         __ASM_EMIT("11001:")
                             MIXADD4_CORE("movups", "movaps", "movaps", "movups")
                             __ASM_EMIT("jmp         2000f")
                     __ASM_EMIT("11010:")
                         __ASM_EMIT("test        $0x0f, %[src4]")
                         __ASM_EMIT("jnz         11011f")
                             MIXADD4_CORE("movups", "movaps", "movups", "movaps")
                             __ASM_EMIT("jmp         2000f")
                         __ASM_EMIT("11011:")
                             MIXADD4_CORE("movups", "movaps", "movups", "movups")
                             __ASM_EMIT("jmp         2000f")

                 __ASM_EMIT("11100:")
                     __ASM_EMIT("test        $0x0f, %[src3]")
                     __ASM_EMIT("jnz         11110f")
                         __ASM_EMIT("test        $0x0f, %[src4]")
                         __ASM_EMIT("jnz         11101f")
                             MIXADD4_CORE("movups", "movups", "movaps", "movaps")
                             __ASM_EMIT("jmp         2000f")
                         __ASM_EMIT("11101:")
                             MIXADD4_CORE("movups", "movups", "movaps", "movups")
                             __ASM_EMIT("jmp         2000f")
                     __ASM_EMIT("11110:")
                         __ASM_EMIT("test        $0x0f, %[src4]")
                         __ASM_EMIT("jnz         11111f")
                             MIXADD4_CORE("movups", "movups", "movups", "movaps")
                             __ASM_EMIT("jmp         2000f")
                         __ASM_EMIT("11111:")
                             MIXADD4_CORE("movups", "movups", "movups", "movups")

            __ASM_EMIT("2000:")

            : [dst] "+r" (dst),
              [src1] "+r" (src1), [src2] "+r" (src2), [src3] "+r" (src3), [src4] "+r" (src4),
              [count] __ASM_ARG_RW(count)
            : [k1] "m"(k1), [k2] "m"(k2), [k3] "m"(k3), [k4] "m"(k4)
            : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    #undef MIXADD4_CORE
}

#endif /* DSP_ARCH_X86_SSE_MIX_H_ */
