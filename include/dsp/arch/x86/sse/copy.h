/*
 * copy.h
 *
 *  Created on: 17 мар. 2017 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_SSE_COPY_H_
#define DSP_ARCH_X86_SSE_COPY_H_

#ifndef DSP_ARCH_X86_SSE_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_SSE_IMPL */

namespace sse
{
    void copy(float *dst, const float *src, size_t count)
    {
        size_t off;

        ARCH_X86_ASM
        (
            __ASM_EMIT("cmp         %[dst], %[src]")
            __ASM_EMIT("je          10f")

            /* Destination is aligned */
            __ASM_EMIT("xor         %[off], %[off]")
            __ASM_EMIT("sub         $0x20, %[count]")
            __ASM_EMIT("jb          2f")

            /* Block of 32 items */
            __ASM_EMIT("1:")
            __ASM_EMIT("movups      0x00(%[src], %[off]), %%xmm0")
            __ASM_EMIT("movups      0x10(%[src], %[off]), %%xmm1")
            __ASM_EMIT("movups      0x20(%[src], %[off]), %%xmm2")
            __ASM_EMIT("movups      0x30(%[src], %[off]), %%xmm3")
            __ASM_EMIT("movups      0x40(%[src], %[off]), %%xmm4")
            __ASM_EMIT("movups      0x50(%[src], %[off]), %%xmm5")
            __ASM_EMIT("movups      0x60(%[src], %[off]), %%xmm6")
            __ASM_EMIT("movups      0x70(%[src], %[off]), %%xmm7")
            __ASM_EMIT("movups      %%xmm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("movups      %%xmm1, 0x10(%[dst], %[off])")
            __ASM_EMIT("movups      %%xmm2, 0x20(%[dst], %[off])")
            __ASM_EMIT("movups      %%xmm3, 0x30(%[dst], %[off])")
            __ASM_EMIT("movups      %%xmm4, 0x40(%[dst], %[off])")
            __ASM_EMIT("movups      %%xmm5, 0x50(%[dst], %[off])")
            __ASM_EMIT("movups      %%xmm6, 0x60(%[dst], %[off])")
            __ASM_EMIT("movups      %%xmm7, 0x70(%[dst], %[off])")
            __ASM_EMIT("add         $0x80, %[off]")
            __ASM_EMIT("sub         $0x20, %[count]")
            __ASM_EMIT("jae         1b")
            /* Block of 16 items */
            __ASM_EMIT("2:")
            __ASM_EMIT("add         $0x10, %[count]")
            __ASM_EMIT("jl          4f")
            __ASM_EMIT("movups      0x00(%[src], %[off]), %%xmm0")
            __ASM_EMIT("movups      0x10(%[src], %[off]), %%xmm1")
            __ASM_EMIT("movups      0x20(%[src], %[off]), %%xmm2")
            __ASM_EMIT("movups      0x30(%[src], %[off]), %%xmm3")
            __ASM_EMIT("movups      %%xmm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("movups      %%xmm1, 0x10(%[dst], %[off])")
            __ASM_EMIT("movups      %%xmm2, 0x20(%[dst], %[off])")
            __ASM_EMIT("movups      %%xmm3, 0x30(%[dst], %[off])")
            __ASM_EMIT("add         $0x40, %[off]")
            __ASM_EMIT("sub         $0x10, %[count]")
            /* Block of 8 items */
            __ASM_EMIT("4:")
            __ASM_EMIT("add         $0x08, %[count]")
            __ASM_EMIT("jl          6f")
            __ASM_EMIT("movups      0x00(%[src], %[off]), %%xmm0")
            __ASM_EMIT("movups      0x10(%[src], %[off]), %%xmm1")
            __ASM_EMIT("movups      %%xmm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("movups      %%xmm1, 0x10(%[dst], %[off])")
            __ASM_EMIT("add         $0x20, %[off]")
            __ASM_EMIT("sub         $0x08, %[count]")
            /* Block of 4 items */
            __ASM_EMIT("6:")
            __ASM_EMIT("add         $0x04, %[count]")
            __ASM_EMIT("jl          8f")
            __ASM_EMIT("movups      0x00(%[src], %[off]), %%xmm0")
            __ASM_EMIT("movups      %%xmm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("add         $0x10, %[off]")
            __ASM_EMIT("sub         $0x04, %[count]")
            /* 1x items */
            __ASM_EMIT("8:")
            __ASM_EMIT("add         $0x03, %[count]")
            __ASM_EMIT("jl          10f")
            __ASM_EMIT("9:")
            __ASM_EMIT("movss       0x00(%[src], %[off]), %%xmm0")
            __ASM_EMIT("movss       %%xmm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("add         $0x04, %[off]")
            __ASM_EMIT("dec         %[count]")
            __ASM_EMIT("jge         9b")

            __ASM_EMIT("10:")

            : [src] "+r" (src), [dst] "+r"(dst),
              [count] "+r" (count), [off] "=&r" (off)
            :
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void copy_movntps(float *dst, const float *src, size_t count)
    {
        size_t off;

        ARCH_X86_ASM
        (
            __ASM_EMIT("cmp         %[dst], %[src]")
            __ASM_EMIT("je          100f")
            __ASM_EMIT("test        %[count], %[count]")
            __ASM_EMIT("jz          100f")

            /* Align destination */
            __ASM_EMIT("test        $0x0f, %[dst]")
            __ASM_EMIT("jz          2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("movss       (%[src]), %%xmm0")
            __ASM_EMIT("movss       %%xmm0, (%[dst])")
            __ASM_EMIT("add         $4, %[src]")
            __ASM_EMIT("add         $4, %[dst]")
            __ASM_EMIT("dec         %[count]")
            __ASM_EMIT("jz          100f")
            __ASM_EMIT("test        $0x0f, %[dst]")
            __ASM_EMIT("jnz         1b")

            /* Block of 32 items */
            __ASM_EMIT("2:")
            __ASM_EMIT("xor         %[off], %[off]")
            __ASM_EMIT("sub         $0x20, %[count]")
            __ASM_EMIT("jb          4f")

            __ASM_EMIT("3:")
            __ASM_EMIT("movups      0x00(%[src], %[off]), %%xmm0")
            __ASM_EMIT("movups      0x10(%[src], %[off]), %%xmm1")
            __ASM_EMIT("movups      0x20(%[src], %[off]), %%xmm2")
            __ASM_EMIT("movups      0x30(%[src], %[off]), %%xmm3")
            __ASM_EMIT("movups      0x40(%[src], %[off]), %%xmm4")
            __ASM_EMIT("movups      0x50(%[src], %[off]), %%xmm5")
            __ASM_EMIT("movups      0x60(%[src], %[off]), %%xmm6")
            __ASM_EMIT("movups      0x70(%[src], %[off]), %%xmm7")
            __ASM_EMIT("movntps     %%xmm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("movntps     %%xmm1, 0x10(%[dst], %[off])")
            __ASM_EMIT("movntps     %%xmm2, 0x20(%[dst], %[off])")
            __ASM_EMIT("movntps     %%xmm3, 0x30(%[dst], %[off])")
            __ASM_EMIT("movntps     %%xmm4, 0x40(%[dst], %[off])")
            __ASM_EMIT("movntps     %%xmm5, 0x50(%[dst], %[off])")
            __ASM_EMIT("movntps     %%xmm6, 0x60(%[dst], %[off])")
            __ASM_EMIT("movntps     %%xmm7, 0x70(%[dst], %[off])")
            __ASM_EMIT("add         $0x80, %[off]")
            __ASM_EMIT("sub         $0x20, %[count]")
            __ASM_EMIT("jae         3b")

            /* Block of 16 items */
            __ASM_EMIT("4:")
            __ASM_EMIT("add         $0x10, %[count]")
            __ASM_EMIT("jl          6f")
            __ASM_EMIT("movups      0x00(%[src], %[off]), %%xmm0")
            __ASM_EMIT("movups      0x10(%[src], %[off]), %%xmm1")
            __ASM_EMIT("movups      0x20(%[src], %[off]), %%xmm2")
            __ASM_EMIT("movups      0x30(%[src], %[off]), %%xmm3")
            __ASM_EMIT("movntps     %%xmm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("movntps     %%xmm1, 0x10(%[dst], %[off])")
            __ASM_EMIT("movntps     %%xmm2, 0x20(%[dst], %[off])")
            __ASM_EMIT("movntps     %%xmm3, 0x30(%[dst], %[off])")
            __ASM_EMIT("add         $0x40, %[off]")
            __ASM_EMIT("sub         $0x10, %[count]")
            /* Block of 8 items */
            __ASM_EMIT("6:")
            __ASM_EMIT("add         $0x08, %[count]")
            __ASM_EMIT("jl          8f")
            __ASM_EMIT("movups      0x00(%[src], %[off]), %%xmm0")
            __ASM_EMIT("movups      0x10(%[src], %[off]), %%xmm1")
            __ASM_EMIT("movntps     %%xmm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("movntps     %%xmm1, 0x10(%[dst], %[off])")
            __ASM_EMIT("add         $0x20, %[off]")
            __ASM_EMIT("sub         $0x08, %[count]")
            /* Block of 4 items */
            __ASM_EMIT("8:")
            __ASM_EMIT("add         $0x04, %[count]")
            __ASM_EMIT("jl          10f")
            __ASM_EMIT("movups      0x00(%[src], %[off]), %%xmm0")
            __ASM_EMIT("movntps     %%xmm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("add         $0x10, %[off]")
            __ASM_EMIT("sub         $0x04, %[count]")
            /* 1x items */
            __ASM_EMIT("10:")
            __ASM_EMIT("add         $0x03, %[count]")
            __ASM_EMIT("jl          12f")
            __ASM_EMIT("9:")
            __ASM_EMIT("movss       0x00(%[src], %[off]), %%xmm0")
            __ASM_EMIT("movss       %%xmm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("add         $0x04, %[off]")
            __ASM_EMIT("dec         %[count]")
            __ASM_EMIT("jge         9b")

            __ASM_EMIT("12:")
            __ASM_EMIT("sfence")

            __ASM_EMIT("100:")

            : [src] "+r" (src), [dst] "+r"(dst),
              [count] "+r" (count), [off] "=&r" (off)
            :
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void copy_backward(float *dst, const float *src, size_t count)
    {
        #define RCOPY_CORE(MV_SRC)   \
            __ASM_EMIT("sub         $0x20, %[count]")   \
            __ASM_EMIT("jb          4f")   \
            \
            /* Block of 32 items */ \
            __ASM_EMIT("3:") \
            __ASM_EMIT("sub         $0x80, %[src]") \
            __ASM_EMIT("sub         $0x80, %[dst]") \
            __ASM_EMIT("prefetchnta -0x80(%[src])") \
            __ASM_EMIT("prefetchnta -0x60(%[src])") \
            __ASM_EMIT("prefetchnta -0x40(%[src])") \
            __ASM_EMIT("prefetchnta -0x20(%[src])") \
            __ASM_EMIT(MV_SRC "     0x00(%[src]), %%xmm0") \
            __ASM_EMIT(MV_SRC "     0x10(%[src]), %%xmm1") \
            __ASM_EMIT(MV_SRC "     0x20(%[src]), %%xmm2") \
            __ASM_EMIT(MV_SRC "     0x30(%[src]), %%xmm3") \
            __ASM_EMIT(MV_SRC "     0x40(%[src]), %%xmm4") \
            __ASM_EMIT(MV_SRC "     0x50(%[src]), %%xmm5") \
            __ASM_EMIT(MV_SRC "     0x60(%[src]), %%xmm6") \
            __ASM_EMIT(MV_SRC "     0x70(%[src]), %%xmm7") \
            __ASM_EMIT("movaps      %%xmm0, 0x00(%[dst])") \
            __ASM_EMIT("movaps      %%xmm1, 0x10(%[dst])") \
            __ASM_EMIT("movaps      %%xmm2, 0x20(%[dst])") \
            __ASM_EMIT("movaps      %%xmm3, 0x30(%[dst])") \
            __ASM_EMIT("movaps      %%xmm4, 0x40(%[dst])") \
            __ASM_EMIT("movaps      %%xmm5, 0x50(%[dst])") \
            __ASM_EMIT("movaps      %%xmm6, 0x60(%[dst])") \
            __ASM_EMIT("movaps      %%xmm7, 0x70(%[dst])") \
            __ASM_EMIT("sub         $0x20, %[count]") \
            __ASM_EMIT("jae         3b") \
            /* Block of 16 items */ \
            __ASM_EMIT("4:") \
            __ASM_EMIT("add         $0x20, %[count]")   \
            __ASM_EMIT("test        $0x10, %[count]")   \
            __ASM_EMIT("jz          5f")   \
            __ASM_EMIT("sub         $0x40, %[src]") \
            __ASM_EMIT("sub         $0x40, %[dst]") \
            __ASM_EMIT(MV_SRC "     0x00(%[src]), %%xmm0") \
            __ASM_EMIT(MV_SRC "     0x10(%[src]), %%xmm1") \
            __ASM_EMIT(MV_SRC "     0x20(%[src]), %%xmm2") \
            __ASM_EMIT(MV_SRC "     0x30(%[src]), %%xmm3") \
            __ASM_EMIT("movaps      %%xmm0, 0x00(%[dst])") \
            __ASM_EMIT("movaps      %%xmm1, 0x10(%[dst])") \
            __ASM_EMIT("movaps      %%xmm2, 0x20(%[dst])") \
            __ASM_EMIT("movaps      %%xmm3, 0x30(%[dst])") \
            /* Block of 8 items */ \
            __ASM_EMIT("5:") \
            __ASM_EMIT("test        $0x08, %[count]")   \
            __ASM_EMIT("jz          6f")   \
            __ASM_EMIT("sub         $0x20, %[src]") \
            __ASM_EMIT("sub         $0x20, %[dst]") \
            __ASM_EMIT(MV_SRC "     0x00(%[src]), %%xmm0") \
            __ASM_EMIT(MV_SRC "     0x10(%[src]), %%xmm1") \
            __ASM_EMIT("movaps      %%xmm0, 0x00(%[dst])") \
            __ASM_EMIT("movaps      %%xmm1, 0x10(%[dst])") \
            /* Block of 4 items */ \
            __ASM_EMIT("6:") \
            __ASM_EMIT("test        $0x04, %[count]")   \
            __ASM_EMIT("jz          3000f")   \
            __ASM_EMIT("sub         $0x10, %[src]") \
            __ASM_EMIT("sub         $0x10, %[dst]") \
            __ASM_EMIT(MV_SRC "     0x00(%[src]), %%xmm0") \
            __ASM_EMIT("movaps      %%xmm0, 0x00(%[dst])")

        ARCH_X86_ASM
        (
            __ASM_EMIT("cmp         %[dst], %[src]")
            __ASM_EMIT("je          2000f")
            __ASM_EMIT("test        %[count], %[count]")
            __ASM_EMIT("jz          2000f")

            /* Initlaize address pointers */
            __ASM_EMIT("lea         (%[src], %[count], 4), %[src]")
            __ASM_EMIT("lea         (%[dst], %[count], 4), %[dst]")

            /* Align destination */
            __ASM_EMIT("1:")
            __ASM_EMIT("test        $0x0f, %[dst]")
            __ASM_EMIT("jz          2f")
            __ASM_EMIT("sub         $0x4, %[src]")
            __ASM_EMIT("sub         $0x4, %[dst]")
            __ASM_EMIT("movss       0x00(%[src]), %%xmm0")
            __ASM_EMIT("dec         %[count]")
            __ASM_EMIT("movss       %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("jnz         1b")
            __ASM_EMIT("jmp         2000f")

            __ASM_EMIT("2:")

            /* Destination is aligned */
            __ASM_EMIT("test        $0x0f, %[src]")
            __ASM_EMIT("jnz         1001f")
                RCOPY_CORE("movaps")
                __ASM_EMIT("jmp         3000f")
            __ASM_EMIT("1001:")
                RCOPY_CORE("movups")

            /* Tail */
            __ASM_EMIT("3000:")
            __ASM_EMIT("and         $0x03, %[count]")
            __ASM_EMIT("jz          2000f")
            __ASM_EMIT("8:")
            __ASM_EMIT("sub         $0x04, %[src]")
            __ASM_EMIT("sub         $0x04, %[dst]")
            __ASM_EMIT("movss       0x00(%[src]), %%xmm0")
            __ASM_EMIT("dec         %[count]")
            __ASM_EMIT("movss       %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("jnz         8b")

            __ASM_EMIT("2000:")

            : [src] "+r" (src), [dst] "+r"(dst), [count] "+r" (count) :
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );

        #undef RCOPY_CORE
    }

    void move(float *dst, const float *src, size_t count)
    {
        if (dst < src)
            copy(dst, src, count);
        else if (dst > src)
            copy_backward(dst, src, count);
    }

    void fill(float *dst, float value, size_t count)
    {
        #define FILL_CORE   \
            /* Blocks of 32 items */ \
            __ASM_EMIT("sub         $0x20, %[count]")   \
            __ASM_EMIT("jb          4f")   \
            __ASM_EMIT("movaps      %%xmm0, %%xmm4") \
            __ASM_EMIT("movaps      %%xmm1, %%xmm5") \
            __ASM_EMIT("movaps      %%xmm2, %%xmm6") \
            __ASM_EMIT("movaps      %%xmm3, %%xmm7") \
            __ASM_EMIT("3:") \
            __ASM_EMIT("movaps      %%xmm0, 0x00(%[dst])") \
            __ASM_EMIT("movaps      %%xmm1, 0x10(%[dst])") \
            __ASM_EMIT("movaps      %%xmm2, 0x20(%[dst])") \
            __ASM_EMIT("movaps      %%xmm3, 0x30(%[dst])") \
            __ASM_EMIT("movaps      %%xmm4, 0x40(%[dst])") \
            __ASM_EMIT("movaps      %%xmm5, 0x50(%[dst])") \
            __ASM_EMIT("movaps      %%xmm6, 0x60(%[dst])") \
            __ASM_EMIT("movaps      %%xmm7, 0x70(%[dst])") \
            __ASM_EMIT("add         $0x80, %[dst]") \
            __ASM_EMIT("sub         $0x20, %[count]") \
            __ASM_EMIT("jae         3b")   \
            /* Blocks of 16 items */ \
            __ASM_EMIT("4:") \
            __ASM_EMIT("add         $0x20, %[count]") \
            __ASM_EMIT("test        $0x10, %[count]") \
            __ASM_EMIT("jz          5f") \
            __ASM_EMIT("movaps      %%xmm0, 0x00(%[dst])") \
            __ASM_EMIT("movaps      %%xmm1, 0x10(%[dst])") \
            __ASM_EMIT("movaps      %%xmm2, 0x20(%[dst])") \
            __ASM_EMIT("movaps      %%xmm3, 0x30(%[dst])") \
            __ASM_EMIT("add         $0x40, %[dst]") \
            /* Blocks of 8 items */ \
            __ASM_EMIT("5:") \
            __ASM_EMIT("test        $0x08, %[count]") \
            __ASM_EMIT("jz          6f") \
            __ASM_EMIT("movaps      %%xmm0, 0x00(%[dst])") \
            __ASM_EMIT("movaps      %%xmm1, 0x10(%[dst])") \
            __ASM_EMIT("add         $0x20, %[dst]") \
            /* Blocks of 4 items */ \
            __ASM_EMIT("6:") \
            __ASM_EMIT("test        $0x04, %[count]") \
            __ASM_EMIT("jz          7f") \
            __ASM_EMIT("movaps      %%xmm0, 0x00(%[dst])") \
            __ASM_EMIT("add         $0x10, %[dst]") \
            /* Tail */ \
            __ASM_EMIT("7:") \
            __ASM_EMIT("and         $0x03, %[count]") \
            __ASM_EMIT("jz          2000f") \
            __ASM_EMIT("8:") \
            __ASM_EMIT("movss       %%xmm0, 0x00(%[dst])") \
            __ASM_EMIT("add         $0x04, %[dst]") \
            __ASM_EMIT("dec         %[count]") \
            __ASM_EMIT("jnz         8b")

        ARCH_X86_ASM
        (
            __ASM_EMIT("test        %[count], %[count]")
            __ASM_EMIT("jz          2000f")
            __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0")

            /* Align destination */
            __ASM_EMIT("1:")
            __ASM_EMIT("test        $0x0f, %[dst]")
            __ASM_EMIT("jz          2f")
            __ASM_EMIT("movss       %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("add         $0x4, %[dst]")
            __ASM_EMIT("dec         %[count]")
            __ASM_EMIT("jnz         1b")
            __ASM_EMIT("jmp         2000f")

            __ASM_EMIT("2:")
            __ASM_EMIT("movaps      %%xmm0, %%xmm1")
            __ASM_EMIT("movaps      %%xmm0, %%xmm2")
            __ASM_EMIT("movaps      %%xmm1, %%xmm3")

            /* Destination is aligned */
            FILL_CORE

            __ASM_EMIT("2000:")

            : [dst] "+r"(dst), [count] "+r" (count), "+Yz"(value) :
            : "cc", "memory",
              "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );

        #undef FILL_CORE
    }

    void fill_zero(float *dst, size_t count)
    {
        fill(dst, 0.0f, count);
    }

    void fill_one(float *dst, size_t count)
    {
        fill(dst, 1.0f, count);
    }

    void fill_minus_one(float *dst, size_t count)
    {
        fill(dst, -1.0f, count);
    }

    #define REVERSE1_CORE(MV_TAIL) \
        __ASM_EMIT("sub         $0x10, %[count]") \
        __ASM_EMIT("jb          4f") \
        /* x16 blocks */ \
        __ASM_EMIT("3:") \
        __ASM_EMIT("sub         $0x40, %[tail]") \
        __ASM_EMIT("movaps      0x00(%[head]), %%xmm3") \
        __ASM_EMIT("movaps      0x10(%[head]), %%xmm2") \
        __ASM_EMIT("movaps      0x20(%[head]), %%xmm1") \
        __ASM_EMIT("movaps      0x30(%[head]), %%xmm0") \
        __ASM_EMIT(MV_TAIL "    0x00(%[tail]), %%xmm7") \
        __ASM_EMIT(MV_TAIL "    0x10(%[tail]), %%xmm6") \
        __ASM_EMIT(MV_TAIL "    0x20(%[tail]), %%xmm5") \
        __ASM_EMIT(MV_TAIL "    0x30(%[tail]), %%xmm4") \
        __ASM_EMIT("shufps      $0x1b, %%xmm0, %%xmm0") \
        __ASM_EMIT("shufps      $0x1b, %%xmm1, %%xmm1") \
        __ASM_EMIT("shufps      $0x1b, %%xmm2, %%xmm2") \
        __ASM_EMIT("shufps      $0x1b, %%xmm3, %%xmm3") \
        __ASM_EMIT("shufps      $0x1b, %%xmm4, %%xmm4") \
        __ASM_EMIT("shufps      $0x1b, %%xmm5, %%xmm5") \
        __ASM_EMIT("shufps      $0x1b, %%xmm6, %%xmm6") \
        __ASM_EMIT("shufps      $0x1b, %%xmm7, %%xmm7") \
        __ASM_EMIT("movaps      %%xmm4, 0x00(%[head])") \
        __ASM_EMIT("movaps      %%xmm5, 0x10(%[head])") \
        __ASM_EMIT("movaps      %%xmm6, 0x20(%[head])") \
        __ASM_EMIT("movaps      %%xmm7, 0x30(%[head])") \
        __ASM_EMIT(MV_TAIL "    %%xmm0, 0x00(%[tail])") \
        __ASM_EMIT(MV_TAIL "    %%xmm1, 0x10(%[tail])") \
        __ASM_EMIT(MV_TAIL "    %%xmm2, 0x20(%[tail])") \
        __ASM_EMIT(MV_TAIL "    %%xmm3, 0x30(%[tail])") \
        __ASM_EMIT("add         $0x40, %[head]") \
        __ASM_EMIT("sub         $0x10, %[count]") \
        __ASM_EMIT("jae         3b") \
        /* x8 block */ \
        __ASM_EMIT("4:") \
        __ASM_EMIT("add         $0x10, %[count]") \
        __ASM_EMIT("test        $0x08, %[count]") \
        __ASM_EMIT("jz          5f") \
        __ASM_EMIT("sub         $0x20, %[tail]") \
        __ASM_EMIT("movaps      0x00(%[head]), %%xmm1") \
        __ASM_EMIT("movaps      0x10(%[head]), %%xmm0") \
        __ASM_EMIT(MV_TAIL "    0x00(%[tail]), %%xmm3") \
        __ASM_EMIT(MV_TAIL "    0x10(%[tail]), %%xmm2") \
        __ASM_EMIT("shufps      $0x1b, %%xmm0, %%xmm0") \
        __ASM_EMIT("shufps      $0x1b, %%xmm1, %%xmm1") \
        __ASM_EMIT("shufps      $0x1b, %%xmm2, %%xmm2") \
        __ASM_EMIT("shufps      $0x1b, %%xmm3, %%xmm3") \
        __ASM_EMIT("movaps      %%xmm2, 0x00(%[head])") \
        __ASM_EMIT("movaps      %%xmm3, 0x10(%[head])") \
        __ASM_EMIT(MV_TAIL "    %%xmm0, 0x00(%[tail])") \
        __ASM_EMIT(MV_TAIL "    %%xmm1, 0x10(%[tail])") \
        __ASM_EMIT("add         $0x20, %[head]") \
        /* x4 block */ \
        __ASM_EMIT("5:") \
        __ASM_EMIT("test        $0x04, %[count]") \
        __ASM_EMIT("jz          3000f") \
        __ASM_EMIT("sub         $0x10, %[tail]") \
        __ASM_EMIT("movaps      0x00(%[head]), %%xmm0") \
        __ASM_EMIT(MV_TAIL "    0x00(%[tail]), %%xmm1") \
        __ASM_EMIT("shufps      $0x1b, %%xmm0, %%xmm0") \
        __ASM_EMIT("shufps      $0x1b, %%xmm1, %%xmm1") \
        __ASM_EMIT("movaps      %%xmm1, 0x00(%[head])") \
        __ASM_EMIT(MV_TAIL "    %%xmm0, 0x00(%[tail])") \
        __ASM_EMIT("add         $0x10, %[head]")

    void reverse1(float *dst, size_t count)
    {
        const float *src;

        ARCH_X86_ASM
        (
            __ASM_EMIT("lea         (%[head], %[count], 4), %[tail]")
            __ASM_EMIT("shr         $1, %[count]")
            __ASM_EMIT("jz          2000f")

            // Align head
            __ASM_EMIT("1:")
            __ASM_EMIT("test        $0x0f, %[head]")
            __ASM_EMIT("jz          2f")
            __ASM_EMIT("sub         $0x04, %[tail]")
            __ASM_EMIT("movss       0x00(%[head]), %%xmm0")
            __ASM_EMIT("movss       0x00(%[tail]), %%xmm1")
            __ASM_EMIT("movss       %%xmm1, 0x00(%[head])")
            __ASM_EMIT("movss       %%xmm0, 0x00(%[tail])")
            __ASM_EMIT("add         $0x04, %[head]")
            __ASM_EMIT("dec         %[count]")
            __ASM_EMIT("jnz         1b")
            __ASM_EMIT("jmp         2000f")

            // Block operation
            __ASM_EMIT("2:")
            __ASM_EMIT("test        $0x0f, %[tail]")
            __ASM_EMIT("jnz         1001f")
                REVERSE1_CORE("movaps")
                __ASM_EMIT("jmp         3000f")
            __ASM_EMIT("1001:")
                REVERSE1_CORE("movups")

            /* x1 blocks */
            __ASM_EMIT("3000:")
            __ASM_EMIT("and         $0x03, %[count]")
            __ASM_EMIT("jz          2000f")
            __ASM_EMIT("7:")
            __ASM_EMIT("sub         $0x04, %[tail]")
            __ASM_EMIT("movss       0x00(%[head]), %%xmm0")
            __ASM_EMIT("movss       0x00(%[tail]), %%xmm1")
            __ASM_EMIT("movss       %%xmm1, 0x00(%[head])")
            __ASM_EMIT("movss       %%xmm0, 0x00(%[tail])")
            __ASM_EMIT("add         $0x04, %[head]")
            __ASM_EMIT("dec         %[count]")
            __ASM_EMIT("jnz         7b")

            __ASM_EMIT("2000:")

            : [head] "+r" (dst), [tail] "=&r" (src), [count] "+r" (count)
            :
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    #undef REVERSE1_CORE

    #define REVERSE2_CORE(MV_TAIL) \
        __ASM_EMIT("sub         $0x20, %[count]") \
        __ASM_EMIT("jb          4f") \
        /* x32 blocks */ \
        __ASM_EMIT("3:") \
        __ASM_EMIT(MV_TAIL "   -0x80(%[tail]), %%xmm7") \
        __ASM_EMIT(MV_TAIL "   -0x70(%[tail]), %%xmm6") \
        __ASM_EMIT(MV_TAIL "   -0x60(%[tail]), %%xmm5") \
        __ASM_EMIT(MV_TAIL "   -0x50(%[tail]), %%xmm4") \
        __ASM_EMIT(MV_TAIL "   -0x40(%[tail]), %%xmm3") \
        __ASM_EMIT(MV_TAIL "   -0x30(%[tail]), %%xmm2") \
        __ASM_EMIT(MV_TAIL "   -0x20(%[tail]), %%xmm1") \
        __ASM_EMIT(MV_TAIL "   -0x10(%[tail]), %%xmm0") \
        __ASM_EMIT("shufps      $0x1b, %%xmm4, %%xmm4") \
        __ASM_EMIT("shufps      $0x1b, %%xmm5, %%xmm5") \
        __ASM_EMIT("shufps      $0x1b, %%xmm6, %%xmm6") \
        __ASM_EMIT("shufps      $0x1b, %%xmm7, %%xmm7") \
        __ASM_EMIT("shufps      $0x1b, %%xmm0, %%xmm0") \
        __ASM_EMIT("shufps      $0x1b, %%xmm1, %%xmm1") \
        __ASM_EMIT("shufps      $0x1b, %%xmm2, %%xmm2") \
        __ASM_EMIT("shufps      $0x1b, %%xmm3, %%xmm3") \
        __ASM_EMIT("movaps      %%xmm0, 0x00(%[head])") \
        __ASM_EMIT("movaps      %%xmm1, 0x10(%[head])") \
        __ASM_EMIT("movaps      %%xmm2, 0x20(%[head])") \
        __ASM_EMIT("movaps      %%xmm3, 0x30(%[head])") \
        __ASM_EMIT("movaps      %%xmm4, 0x40(%[head])") \
        __ASM_EMIT("movaps      %%xmm5, 0x50(%[head])") \
        __ASM_EMIT("movaps      %%xmm6, 0x60(%[head])") \
        __ASM_EMIT("movaps      %%xmm7, 0x70(%[head])") \
        __ASM_EMIT("sub         $0x80, %[tail]") \
        __ASM_EMIT("add         $0x80, %[head]") \
        __ASM_EMIT("sub         $0x20, %[count]") \
        __ASM_EMIT("jae         3b") \
        /* x16 block */ \
        __ASM_EMIT("4:") \
        __ASM_EMIT("add         $0x20, %[count]") \
        __ASM_EMIT("test        $0x10, %[count]") \
        __ASM_EMIT("jz          5f") \
        __ASM_EMIT(MV_TAIL "   -0x40(%[tail]), %%xmm3") \
        __ASM_EMIT(MV_TAIL "   -0x30(%[tail]), %%xmm2") \
        __ASM_EMIT(MV_TAIL "   -0x20(%[tail]), %%xmm1") \
        __ASM_EMIT(MV_TAIL "   -0x10(%[tail]), %%xmm0") \
        __ASM_EMIT("shufps      $0x1b, %%xmm2, %%xmm2") \
        __ASM_EMIT("shufps      $0x1b, %%xmm3, %%xmm3") \
        __ASM_EMIT("shufps      $0x1b, %%xmm0, %%xmm0") \
        __ASM_EMIT("shufps      $0x1b, %%xmm1, %%xmm1") \
        __ASM_EMIT("movaps      %%xmm0, 0x00(%[head])") \
        __ASM_EMIT("movaps      %%xmm1, 0x10(%[head])") \
        __ASM_EMIT("movaps      %%xmm2, 0x20(%[head])") \
        __ASM_EMIT("movaps      %%xmm3, 0x30(%[head])") \
        __ASM_EMIT("sub         $0x40, %[tail]") \
        __ASM_EMIT("add         $0x40, %[head]") \
        /* x8 block */ \
        __ASM_EMIT("5:") \
        __ASM_EMIT("test        $0x08, %[count]") \
        __ASM_EMIT("jz          6f") \
        __ASM_EMIT(MV_TAIL "   -0x20(%[tail]), %%xmm1") \
        __ASM_EMIT(MV_TAIL "   -0x10(%[tail]), %%xmm0") \
        __ASM_EMIT("shufps      $0x1b, %%xmm1, %%xmm1") \
        __ASM_EMIT("shufps      $0x1b, %%xmm0, %%xmm0") \
        __ASM_EMIT("movaps      %%xmm0, 0x00(%[head])") \
        __ASM_EMIT("movaps      %%xmm1, 0x10(%[head])") \
        __ASM_EMIT("sub         $0x20, %[tail]") \
        __ASM_EMIT("add         $0x20, %[head]") \
        /* x4 block */ \
        __ASM_EMIT("6:") \
        __ASM_EMIT("test        $0x04, %[count]") \
        __ASM_EMIT("jz          3000f") \
        __ASM_EMIT(MV_TAIL "   -0x10(%[tail]), %%xmm0") \
        __ASM_EMIT("shufps      $0x1b, %%xmm0, %%xmm0") \
        __ASM_EMIT("movaps      %%xmm0, 0x00(%[head])") \
        __ASM_EMIT("sub         $0x10, %[tail]") \
        __ASM_EMIT("add         $0x10, %[head]")

    void reverse2(float *dst, const float *src, size_t count)
    {
        if (dst == src)
        {
            reverse1(dst, count);
            return;
        }

        ARCH_X86_ASM
        (
            __ASM_EMIT("test        %[count], %[count]")
            __ASM_EMIT("jz          2000f")

            __ASM_EMIT("lea         (%[tail], %[count], 4), %[tail]")

            // Align head
            __ASM_EMIT("1:")
            __ASM_EMIT("test        $0x0f, %[head]")
            __ASM_EMIT("jz          2f")
            __ASM_EMIT("sub         $0x04, %[tail]")
            __ASM_EMIT("movss       0x00(%[tail]), %%xmm0")
            __ASM_EMIT("movss       %%xmm0, 0x00(%[head])")
            __ASM_EMIT("add         $0x04, %[head]")
            __ASM_EMIT("dec         %[count]")
            __ASM_EMIT("jnz         1b")
            __ASM_EMIT("jmp         2000f")

            // Block operation
            __ASM_EMIT("2:")
            __ASM_EMIT("test        $0x0f, %[tail]")
            __ASM_EMIT("jnz         1001f")
                REVERSE2_CORE("movaps")
                __ASM_EMIT("jmp         3000f")
            __ASM_EMIT("1001:")
                REVERSE2_CORE("movups")

            /* x1 blocks */
            __ASM_EMIT("3000:")
            __ASM_EMIT("and         $0x03, %[count]")
            __ASM_EMIT("jz          2000f")
            __ASM_EMIT("8:")
            __ASM_EMIT("sub         $0x04, %[tail]")
            __ASM_EMIT("movss       0x00(%[tail]), %%xmm0")
            __ASM_EMIT("movss       %%xmm0, 0x00(%[head])")
            __ASM_EMIT("add         $0x04, %[head]")
            __ASM_EMIT("dec         %[count]")
            __ASM_EMIT("jnz         8b")

            __ASM_EMIT("2000:")

            : [head] "+r" (dst), [tail] "+r" (src), [count] "+r" (count)
            :
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    #undef REVERSE2_CORE
}

#endif /* DSP_ARCH_X86_SSE_COPY_H_ */
