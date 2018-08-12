/*
 * lmath.h
 *
 *  Created on: 6 мар. 2017 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_SSE_LMATH_H_
#define DSP_ARCH_X86_SSE_LMATH_H_

#ifndef DSP_ARCH_X86_SSE_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_SSE_IMPL */

#define OP2_ALIGN(OP) \
    /* Align destination */ \
    __ASM_EMIT("1:") \
    __ASM_EMIT("test        $0x0f, %[dst]") \
    __ASM_EMIT("jz          2f") \
    __ASM_EMIT("movss       0x00(%[dst]), %%xmm0") \
    __ASM_EMIT("movss       0x00(%[src]), %%xmm4") \
    __ASM_EMIT(OP "ss       %%xmm4, %%xmm0") \
    __ASM_EMIT("movss       %%xmm0, 0x00(%[dst])") \
    __ASM_EMIT("add         $0x04, %[src]") \
    __ASM_EMIT("add         $0x04, %[dst]") \
    __ASM_EMIT("dec         %[count]") \
    __ASM_EMIT("jnz         1b") \
    __ASM_EMIT("jmp         20000f") \
    __ASM_EMIT("2:")

#define OP2_CORE(OP, MV_SRC)   \
    __ASM_EMIT("sub         $0x10, %[count]") \
    __ASM_EMIT("jb          2f") \
    /* 16x blocks */ \
    __ASM_EMIT("1:") \
    __ASM_EMIT("movaps      0x00(%[dst]), %%xmm0") \
    __ASM_EMIT("movaps      0x10(%[dst]), %%xmm1") \
    __ASM_EMIT("movaps      0x20(%[dst]), %%xmm2") \
    __ASM_EMIT("movaps      0x30(%[dst]), %%xmm3") \
    __ASM_EMIT(MV_SRC "     0x00(%[src]), %%xmm4") \
    __ASM_EMIT(MV_SRC "     0x10(%[src]), %%xmm5") \
    __ASM_EMIT(MV_SRC "     0x20(%[src]), %%xmm6") \
    __ASM_EMIT(MV_SRC "     0x30(%[src]), %%xmm7") \
    __ASM_EMIT(OP "ps       %%xmm4, %%xmm0") \
    __ASM_EMIT(OP "ps       %%xmm5, %%xmm1") \
    __ASM_EMIT(OP "ps       %%xmm6, %%xmm2") \
    __ASM_EMIT(OP "ps       %%xmm7, %%xmm3") \
    __ASM_EMIT("movaps      %%xmm0, 0x00(%[dst])") \
    __ASM_EMIT("movaps      %%xmm1, 0x10(%[dst])") \
    __ASM_EMIT("movaps      %%xmm2, 0x20(%[dst])") \
    __ASM_EMIT("movaps      %%xmm3, 0x30(%[dst])") \
    __ASM_EMIT("add         $0x40, %[src]") \
    __ASM_EMIT("add         $0x40, %[dst]") \
    __ASM_EMIT("sub         $0x10, %[count]") \
    __ASM_EMIT("jae         1b") \
    /* 8x blocks */ \
    __ASM_EMIT("2:") \
    __ASM_EMIT("add         $0x10, %[count]") \
    __ASM_EMIT("test        $8, %[count]") \
    __ASM_EMIT("jz          3f") \
    __ASM_EMIT("movaps      0x00(%[dst]), %%xmm0") \
    __ASM_EMIT("movaps      0x10(%[dst]), %%xmm1") \
    __ASM_EMIT(MV_SRC "     0x00(%[src]), %%xmm4") \
    __ASM_EMIT(MV_SRC "     0x10(%[src]), %%xmm5") \
    __ASM_EMIT(OP "ps       %%xmm4, %%xmm0") \
    __ASM_EMIT(OP "ps       %%xmm5, %%xmm1") \
    __ASM_EMIT("movaps      %%xmm0, 0x00(%[dst])") \
    __ASM_EMIT("movaps      %%xmm1, 0x10(%[dst])") \
    __ASM_EMIT("add         $0x20, %[src]") \
    __ASM_EMIT("add         $0x20, %[dst]") \
    /* 4x blocks */ \
    __ASM_EMIT("3:") \
    __ASM_EMIT("test        $4, %[count]") \
    __ASM_EMIT("jz          4f") \
    __ASM_EMIT("movaps      0x00(%[dst]), %%xmm0") \
    __ASM_EMIT(MV_SRC "     0x00(%[src]), %%xmm4") \
    __ASM_EMIT(OP "ps       %%xmm4, %%xmm0") \
    __ASM_EMIT("movaps      %%xmm0, 0x00(%[dst])") \
    __ASM_EMIT("add         $0x10, %[src]") \
    __ASM_EMIT("add         $0x10, %[dst]") \
    /* 1x blocks */ \
    __ASM_EMIT("4:") \
    __ASM_EMIT("and         $3, %[count]") \
    __ASM_EMIT("jz          20000f") \
    __ASM_EMIT("5:") \
    __ASM_EMIT("movss       0x00(%[dst]), %%xmm0") \
    __ASM_EMIT("movss       0x00(%[src]), %%xmm4") \
    __ASM_EMIT(OP "ss       %%xmm4, %%xmm0") \
    __ASM_EMIT("movss       %%xmm0, 0x00(%[dst])") \
    __ASM_EMIT("add         $0x04, %[src]") \
    __ASM_EMIT("add         $0x04, %[dst]") \
    __ASM_EMIT("dec         %[count]") \
    __ASM_EMIT("jnz         5b")


void add2(float *dst, const float *src, size_t count)
{
    __asm__ __volatile__
    (
        __ASM_EMIT("test        %[count], %[count]")
        __ASM_EMIT("jz          20000f")

        OP2_ALIGN("add")

        __ASM_EMIT("test        $0x0f, %[src]")
        __ASM_EMIT("jnz         10001f")
            OP2_CORE("add", "movaps")
            __ASM_EMIT("jmp         20000f")
        __ASM_EMIT("10001:")
            OP2_CORE("add", "movups")

        __ASM_EMIT("20000:")

        : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
        :
        : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
          "%xmm4", "%xmm5", "%xmm6", "%xmm7"
    );
}

void sub2(float *dst, const float *src, size_t count)
{
    __asm__ __volatile__
    (
        __ASM_EMIT("test        %[count], %[count]")
        __ASM_EMIT("jz          20000f")

        OP2_ALIGN("sub")

        __ASM_EMIT("test        $0x0f, %[src]")
        __ASM_EMIT("jnz         10001f")
            OP2_CORE("sub", "movaps")
            __ASM_EMIT("jmp         20000f")
        __ASM_EMIT("10001:")
            OP2_CORE("sub", "movups")

        __ASM_EMIT("20000:")

        : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
        :
        : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
          "%xmm4", "%xmm5", "%xmm6", "%xmm7"
    );
}

void mul2(float *dst, const float *src, size_t count)
{
    __asm__ __volatile__
    (
        __ASM_EMIT("test        %[count], %[count]")
        __ASM_EMIT("jz          20000f")

        OP2_ALIGN("mul")

        __ASM_EMIT("test        $0x0f, %[src]")
        __ASM_EMIT("jnz         10001f")
            OP2_CORE("mul", "movaps")
            __ASM_EMIT("jmp         20000f")
        __ASM_EMIT("10001:")
            OP2_CORE("mul", "movups")

        __ASM_EMIT("20000:")

        : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
        :
        : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
          "%xmm4", "%xmm5", "%xmm6", "%xmm7"
    );
}

void div2(float *dst, const float *src, size_t count)
{
    __asm__ __volatile__
    (
        __ASM_EMIT("test        %[count], %[count]")
        __ASM_EMIT("jz          20000f")

        OP2_ALIGN("div")

        __ASM_EMIT("test        $0x0f, %[src]")
        __ASM_EMIT("jnz         10001f")
            OP2_CORE("div", "movaps")
            __ASM_EMIT("jmp         20000f")
        __ASM_EMIT("10001:")
            OP2_CORE("div", "movups")

        __ASM_EMIT("20000:")

        : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
        :
        : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
          "%xmm4", "%xmm5", "%xmm6", "%xmm7"
    );
}

#undef OP2_CORE
#undef OP2_ALIGN

#define OP3_ALIGN(OP) \
    /* Align destination */ \
    __ASM_EMIT("1:") \
    __ASM_EMIT("test        $0x0f, %[dst]") \
    __ASM_EMIT("jz          2f") \
    __ASM_EMIT("movss       0x00(%[src1]), %%xmm0") \
    __ASM_EMIT("movss       0x00(%[src2]), %%xmm4") \
    __ASM_EMIT(OP "ss       %%xmm4, %%xmm0") \
    __ASM_EMIT("movss       %%xmm0, 0x00(%[dst])") \
    __ASM_EMIT("add         $0x04, %[src1]") \
    __ASM_EMIT("add         $0x04, %[src2]") \
    __ASM_EMIT("add         $0x04, %[dst]") \
    __ASM_EMIT("dec         %[count]") \
    __ASM_EMIT("jnz         1b") \
    __ASM_EMIT("jmp         20000f") \
    __ASM_EMIT("2:")

#define OP3_CORE(OP, MV_SRC1, MV_SRC2)   \
    __ASM_EMIT("sub         $0x10, %[count]") \
    __ASM_EMIT("jb          2f") \
    /* 16x blocks */ \
    __ASM_EMIT("1:") \
    __ASM_EMIT(MV_SRC1 "    0x00(%[src1]), %%xmm0") \
    __ASM_EMIT(MV_SRC1 "    0x10(%[src1]), %%xmm1") \
    __ASM_EMIT(MV_SRC1 "    0x20(%[src1]), %%xmm2") \
    __ASM_EMIT(MV_SRC1 "    0x30(%[src1]), %%xmm3") \
    __ASM_EMIT(MV_SRC2 "    0x00(%[src2]), %%xmm4") \
    __ASM_EMIT(MV_SRC2 "    0x10(%[src2]), %%xmm5") \
    __ASM_EMIT(MV_SRC2 "    0x20(%[src2]), %%xmm6") \
    __ASM_EMIT(MV_SRC2 "    0x30(%[src2]), %%xmm7") \
    __ASM_EMIT(OP "ps       %%xmm4, %%xmm0") \
    __ASM_EMIT(OP "ps       %%xmm5, %%xmm1") \
    __ASM_EMIT(OP "ps       %%xmm6, %%xmm2") \
    __ASM_EMIT(OP "ps       %%xmm7, %%xmm3") \
    __ASM_EMIT("movaps      %%xmm0, 0x00(%[dst])") \
    __ASM_EMIT("movaps      %%xmm1, 0x10(%[dst])") \
    __ASM_EMIT("movaps      %%xmm2, 0x20(%[dst])") \
    __ASM_EMIT("movaps      %%xmm3, 0x30(%[dst])") \
    __ASM_EMIT("add         $0x40, %[src1]") \
    __ASM_EMIT("add         $0x40, %[src2]") \
    __ASM_EMIT("add         $0x40, %[dst]") \
    __ASM_EMIT("sub         $0x10, %[count]") \
    __ASM_EMIT("jae         1b") \
    /* 8x blocks */ \
    __ASM_EMIT("2:") \
    __ASM_EMIT("add         $0x10, %[count]") \
    __ASM_EMIT("test        $8, %[count]") \
    __ASM_EMIT("jz          3f") \
    __ASM_EMIT(MV_SRC1 "    0x00(%[src1]), %%xmm0") \
    __ASM_EMIT(MV_SRC1 "    0x10(%[src1]), %%xmm1") \
    __ASM_EMIT(MV_SRC2 "    0x00(%[src2]), %%xmm4") \
    __ASM_EMIT(MV_SRC2 "    0x10(%[src2]), %%xmm5") \
    __ASM_EMIT(OP "ps       %%xmm4, %%xmm0") \
    __ASM_EMIT(OP "ps       %%xmm5, %%xmm1") \
    __ASM_EMIT("movaps      %%xmm0, 0x00(%[dst])") \
    __ASM_EMIT("movaps      %%xmm1, 0x10(%[dst])") \
    __ASM_EMIT("add         $0x20, %[src1]") \
    __ASM_EMIT("add         $0x20, %[src2]") \
    __ASM_EMIT("add         $0x20, %[dst]") \
    /* 4x blocks */ \
    __ASM_EMIT("3:") \
    __ASM_EMIT("test        $4, %[count]") \
    __ASM_EMIT("jz          4f") \
    __ASM_EMIT(MV_SRC1 "    0x00(%[src1]), %%xmm0") \
    __ASM_EMIT(MV_SRC2 "    0x00(%[src2]), %%xmm4") \
    __ASM_EMIT(OP "ps       %%xmm4, %%xmm0") \
    __ASM_EMIT("movaps      %%xmm0, 0x00(%[dst])") \
    __ASM_EMIT("add         $0x10, %[src1]") \
    __ASM_EMIT("add         $0x10, %[src2]") \
    __ASM_EMIT("add         $0x10, %[dst]") \
    /* 1x blocks */ \
    __ASM_EMIT("4:") \
    __ASM_EMIT("and         $3, %[count]") \
    __ASM_EMIT("jz          20000f") \
    __ASM_EMIT("5:") \
    __ASM_EMIT("movss       0x00(%[src1]), %%xmm0") \
    __ASM_EMIT("movss       0x00(%[src2]), %%xmm4") \
    __ASM_EMIT(OP "ss       %%xmm4, %%xmm0") \
    __ASM_EMIT("movss       %%xmm0, 0x00(%[dst])") \
    __ASM_EMIT("add         $0x04, %[src1]") \
    __ASM_EMIT("add         $0x04, %[src2]") \
    __ASM_EMIT("add         $0x04, %[dst]") \
    __ASM_EMIT("dec         %[count]") \
    __ASM_EMIT("jnz         5b")

void add3(float *dst, const float *src1, const float *src2, size_t count)
{
    __asm__ __volatile__
    (
        __ASM_EMIT("test        %[count], %[count]")
        __ASM_EMIT("jz          20000f")

        OP3_ALIGN("add")

        __ASM_EMIT("test        $0x0f, %[src1]")
        __ASM_EMIT("jnz         10010f")
            __ASM_EMIT("test        $0x0f, %[src2]")
            __ASM_EMIT("jnz         10001f")
                OP3_CORE("add", "movaps", "movaps")
                __ASM_EMIT("jmp         20000f")
            __ASM_EMIT("10001:")
                OP3_CORE("add", "movaps", "movups")
                __ASM_EMIT("jmp         20000f")
        __ASM_EMIT("10010:")
            __ASM_EMIT("test        $0x0f, %[src2]")
            __ASM_EMIT("jnz         10011f")
                OP3_CORE("add", "movups", "movaps")
                __ASM_EMIT("jmp         20000f")
            __ASM_EMIT("10011:")
                OP3_CORE("add", "movups", "movups")

        __ASM_EMIT("20000:")

        : [dst] "+r" (dst), [src1] "+r" (src1), [src2] "+r" (src2), [count] "+r" (count)
        :
        : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
          "%xmm4", "%xmm5", "%xmm6", "%xmm7"
    );
}

void sub3(float *dst, const float *src1, const float *src2, size_t count)
{
    __asm__ __volatile__
    (
        __ASM_EMIT("test        %[count], %[count]")
        __ASM_EMIT("jz          20000f")

        OP3_ALIGN("sub")

        __ASM_EMIT("test        $0x0f, %[src1]")
        __ASM_EMIT("jnz         10010f")
            __ASM_EMIT("test        $0x0f, %[src2]")
            __ASM_EMIT("jnz         10001f")
                OP3_CORE("sub", "movaps", "movaps")
                __ASM_EMIT("jmp         20000f")
            __ASM_EMIT("10001:")
                OP3_CORE("sub", "movaps", "movups")
                __ASM_EMIT("jmp         20000f")
        __ASM_EMIT("10010:")
            __ASM_EMIT("test        $0x0f, %[src2]")
            __ASM_EMIT("jnz         10011f")
                OP3_CORE("sub", "movups", "movaps")
                __ASM_EMIT("jmp         20000f")
            __ASM_EMIT("10011:")
                OP3_CORE("sub", "movups", "movups")

        __ASM_EMIT("20000:")

        : [dst] "+r" (dst), [src1] "+r" (src1), [src2] "+r" (src2), [count] "+r" (count)
        :
        : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
          "%xmm4", "%xmm5", "%xmm6", "%xmm7"
    );
}

void mul3(float *dst, const float *src1, const float *src2, size_t count)
{
    __asm__ __volatile__
    (
        __ASM_EMIT("test        %[count], %[count]")
        __ASM_EMIT("jz          20000f")

        OP3_ALIGN("mul")

        __ASM_EMIT("test        $0x0f, %[src1]")
        __ASM_EMIT("jnz         10010f")
            __ASM_EMIT("test        $0x0f, %[src2]")
            __ASM_EMIT("jnz         10001f")
                OP3_CORE("mul", "movaps", "movaps")
                __ASM_EMIT("jmp         20000f")
            __ASM_EMIT("10001:")
                OP3_CORE("mul", "movaps", "movups")
                __ASM_EMIT("jmp         20000f")
        __ASM_EMIT("10010:")
            __ASM_EMIT("test        $0x0f, %[src2]")
            __ASM_EMIT("jnz         10011f")
                OP3_CORE("mul", "movups", "movaps")
                __ASM_EMIT("jmp         20000f")
            __ASM_EMIT("10011:")
                OP3_CORE("mul", "movups", "movups")

        __ASM_EMIT("20000:")

        : [dst] "+r" (dst), [src1] "+r" (src1), [src2] "+r" (src2), [count] "+r" (count)
        :
        : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
          "%xmm4", "%xmm5", "%xmm6", "%xmm7"
    );
}

void div3(float *dst, const float *src1, const float *src2, size_t count)
{
    __asm__ __volatile__
    (
        __ASM_EMIT("test        %[count], %[count]")
        __ASM_EMIT("jz          20000f")

        OP3_ALIGN("div")

        __ASM_EMIT("test        $0x0f, %[src1]")
        __ASM_EMIT("jnz         10010f")
            __ASM_EMIT("test        $0x0f, %[src2]")
            __ASM_EMIT("jnz         10001f")
                OP3_CORE("div", "movaps", "movaps")
                __ASM_EMIT("jmp         20000f")
            __ASM_EMIT("10001:")
                OP3_CORE("div", "movaps", "movups")
                __ASM_EMIT("jmp         20000f")
        __ASM_EMIT("10010:")
            __ASM_EMIT("test        $0x0f, %[src2]")
            __ASM_EMIT("jnz         10011f")
                OP3_CORE("div", "movups", "movaps")
                __ASM_EMIT("jmp         20000f")
            __ASM_EMIT("10011:")
                OP3_CORE("div", "movups", "movups")

        __ASM_EMIT("20000:")

        : [dst] "+r" (dst), [src1] "+r" (src1), [src2] "+r" (src2), [count] "+r" (count)
        :
        : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
          "%xmm4", "%xmm5", "%xmm6", "%xmm7"
    );
}

#undef OP3_CORE
#undef OP3_ALIGN

void scale2(float *dst, float k, size_t count)
{
    #define SCALE2_CORE  \
        __ASM_EMIT("sub         $0x18, %[count]") \
        __ASM_EMIT("jb          4f")    \
        \
        /* 24x blocks */ \
        __ASM_EMIT("3:") \
        __ASM_EMIT("movaps      0x00(%[dst]), %%xmm2") \
        __ASM_EMIT("movaps      0x10(%[dst]), %%xmm3") \
        __ASM_EMIT("movaps      0x20(%[dst]), %%xmm4") \
        __ASM_EMIT("movaps      0x30(%[dst]), %%xmm5") \
        __ASM_EMIT("movaps      0x40(%[dst]), %%xmm6") \
        __ASM_EMIT("movaps      0x50(%[dst]), %%xmm7") \
        __ASM_EMIT("mulps       %%xmm0, %%xmm2") \
        __ASM_EMIT("mulps       %%xmm1, %%xmm3") \
        __ASM_EMIT("mulps       %%xmm0, %%xmm4") \
        __ASM_EMIT("mulps       %%xmm1, %%xmm5") \
        __ASM_EMIT("mulps       %%xmm0, %%xmm6") \
        __ASM_EMIT("mulps       %%xmm1, %%xmm7") \
        __ASM_EMIT("movaps      %%xmm2, 0x00(%[dst])") \
        __ASM_EMIT("movaps      %%xmm3, 0x10(%[dst])") \
        __ASM_EMIT("movaps      %%xmm4, 0x20(%[dst])") \
        __ASM_EMIT("movaps      %%xmm5, 0x30(%[dst])") \
        __ASM_EMIT("movaps      %%xmm6, 0x40(%[dst])") \
        __ASM_EMIT("movaps      %%xmm7, 0x50(%[dst])") \
        \
        __ASM_EMIT("add         $0x60, %[dst]") \
        __ASM_EMIT("sub         $0x18, %[count]") \
        __ASM_EMIT("jae         3b") \
        \
        /* 16x block */ \
        __ASM_EMIT("4:") \
        __ASM_EMIT("add         $0x18, %[count]") \
        __ASM_EMIT("test        $0x10, %[count]") \
        __ASM_EMIT("jz          5f") \
        __ASM_EMIT("movaps      0x00(%[dst]), %%xmm2") \
        __ASM_EMIT("movaps      0x10(%[dst]), %%xmm3") \
        __ASM_EMIT("movaps      0x20(%[dst]), %%xmm4") \
        __ASM_EMIT("movaps      0x30(%[dst]), %%xmm5") \
        __ASM_EMIT("mulps       %%xmm0, %%xmm2") \
        __ASM_EMIT("mulps       %%xmm1, %%xmm3") \
        __ASM_EMIT("mulps       %%xmm0, %%xmm4") \
        __ASM_EMIT("mulps       %%xmm1, %%xmm5") \
        __ASM_EMIT("movaps      %%xmm2, 0x00(%[dst])") \
        __ASM_EMIT("movaps      %%xmm3, 0x10(%[dst])") \
        __ASM_EMIT("movaps      %%xmm4, 0x20(%[dst])") \
        __ASM_EMIT("movaps      %%xmm5, 0x30(%[dst])") \
        \
        __ASM_EMIT("add         $0x40, %[dst]") \
        /* 8x block */ \
        __ASM_EMIT("5:") \
        __ASM_EMIT("test        $0x8, %[count]") \
        __ASM_EMIT("jz          6f") \
        __ASM_EMIT("movaps      0x00(%[dst]), %%xmm2") \
        __ASM_EMIT("movaps      0x10(%[dst]), %%xmm3") \
        __ASM_EMIT("mulps       %%xmm0, %%xmm2") \
        __ASM_EMIT("mulps       %%xmm1, %%xmm3") \
        __ASM_EMIT("movaps      %%xmm2, 0x00(%[dst])") \
        __ASM_EMIT("movaps      %%xmm3, 0x10(%[dst])") \
        \
        __ASM_EMIT("add         $0x20, %[dst]") \
        /* 4x block */ \
        __ASM_EMIT("6:") \
        __ASM_EMIT("test        $0x4, %[count]") \
        __ASM_EMIT("jz          7f") \
        __ASM_EMIT("movaps      0x00(%[dst]), %%xmm2") \
        __ASM_EMIT("mulps       %%xmm0, %%xmm2") \
        __ASM_EMIT("movaps      %%xmm2, 0x00(%[dst])") \
        \
        __ASM_EMIT("add         $0x10, %[dst]") \
        /* 1x blocks */ \
        __ASM_EMIT("7:") \
        __ASM_EMIT("and         $0x03, %[count]")    \
        __ASM_EMIT("jz          2000f")    \
        __ASM_EMIT("8:") \
        __ASM_EMIT("movss       (%[dst]), %%xmm2") \
        __ASM_EMIT("mulss       %%xmm0, %%xmm2") \
        __ASM_EMIT("movss       %%xmm2, (%[dst])") \
        __ASM_EMIT("add         $0x4, %[dst]") \
        __ASM_EMIT("dec         %[count]") \
        __ASM_EMIT("jnz         8b")

    __asm__ __volatile__
    (
        __ASM_EMIT("test        %[count], %[count]")
        __ASM_EMIT("jz          2000f")
        __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0")

        // Align destination
        __ASM_EMIT("1:")
        __ASM_EMIT("test        $0x0f, %[dst]")
        __ASM_EMIT("jz          2f")
        __ASM_EMIT("movss       (%[dst]), %%xmm2")
        __ASM_EMIT("mulss       %%xmm0, %%xmm2")
        __ASM_EMIT("movss       %%xmm2, (%[dst])")
        __ASM_EMIT("add         $0x4, %[dst]")
        __ASM_EMIT("dec         %[count]")
        __ASM_EMIT("jnz         1b")
        __ASM_EMIT("jmp         2000f")

        // Block operation
        __ASM_EMIT("2:")
        __ASM_EMIT("movaps      %%xmm0, %%xmm1")

        SCALE2_CORE

        __ASM_EMIT("2000:")

        : [dst] "+r"(dst), [count] "+r" (count), [k] "+Yz"(k) :
        : "cc", "memory",
          "%xmm1", "%xmm2", "%xmm3",
          "%xmm4", "%xmm5", "%xmm6", "%xmm7"
    );

    #undef SCALE2_CORE
}

void scale3(float *dst, const float *src, float k, size_t count)
{
    #define SCALE3_CORE(MV_SRC)  \
        __ASM_EMIT("sub         $0x18, %[count]") \
        __ASM_EMIT("jb          4f")    \
        \
        /* 24x blocks */ \
        __ASM_EMIT("3:") \
        __ASM_EMIT(MV_SRC "     0x00(%[src]), %%xmm2") \
        __ASM_EMIT(MV_SRC "     0x10(%[src]), %%xmm3") \
        __ASM_EMIT(MV_SRC "     0x20(%[src]), %%xmm4") \
        __ASM_EMIT(MV_SRC "     0x30(%[src]), %%xmm5") \
        __ASM_EMIT(MV_SRC "     0x40(%[src]), %%xmm6") \
        __ASM_EMIT(MV_SRC "     0x50(%[src]), %%xmm7") \
        __ASM_EMIT("mulps       %%xmm0, %%xmm2") \
        __ASM_EMIT("mulps       %%xmm1, %%xmm3") \
        __ASM_EMIT("mulps       %%xmm0, %%xmm4") \
        __ASM_EMIT("mulps       %%xmm1, %%xmm5") \
        __ASM_EMIT("mulps       %%xmm0, %%xmm6") \
        __ASM_EMIT("mulps       %%xmm1, %%xmm7") \
        __ASM_EMIT("movaps      %%xmm2, 0x00(%[dst])") \
        __ASM_EMIT("movaps      %%xmm3, 0x10(%[dst])") \
        __ASM_EMIT("movaps      %%xmm4, 0x20(%[dst])") \
        __ASM_EMIT("movaps      %%xmm5, 0x30(%[dst])") \
        __ASM_EMIT("movaps      %%xmm6, 0x40(%[dst])") \
        __ASM_EMIT("movaps      %%xmm7, 0x50(%[dst])") \
        \
        __ASM_EMIT("add         $0x60, %[src]") \
        __ASM_EMIT("add         $0x60, %[dst]") \
        __ASM_EMIT("sub         $0x18, %[count]") \
        __ASM_EMIT("jae         3b") \
        \
        /* 16x block */ \
        __ASM_EMIT("4:") \
        __ASM_EMIT("add         $0x18, %[count]") \
        __ASM_EMIT("test        $0x10, %[count]") \
        __ASM_EMIT("jz          5f") \
        __ASM_EMIT(MV_SRC "     0x00(%[src]), %%xmm2") \
        __ASM_EMIT(MV_SRC "     0x10(%[src]), %%xmm3") \
        __ASM_EMIT(MV_SRC "     0x20(%[src]), %%xmm4") \
        __ASM_EMIT(MV_SRC "     0x30(%[src]), %%xmm5") \
        __ASM_EMIT("mulps       %%xmm0, %%xmm2") \
        __ASM_EMIT("mulps       %%xmm1, %%xmm3") \
        __ASM_EMIT("mulps       %%xmm0, %%xmm4") \
        __ASM_EMIT("mulps       %%xmm1, %%xmm5") \
        __ASM_EMIT("movaps      %%xmm2, 0x00(%[dst])") \
        __ASM_EMIT("movaps      %%xmm3, 0x10(%[dst])") \
        __ASM_EMIT("movaps      %%xmm4, 0x20(%[dst])") \
        __ASM_EMIT("movaps      %%xmm5, 0x30(%[dst])") \
        \
        __ASM_EMIT("sub         $0x10, %[count]") \
        __ASM_EMIT("add         $0x40, %[src]") \
        __ASM_EMIT("add         $0x40, %[dst]") \
        /* 8x block */ \
        __ASM_EMIT("5:") \
        __ASM_EMIT("test        $0x8, %[count]") \
        __ASM_EMIT("jz          6f") \
        __ASM_EMIT(MV_SRC "     0x00(%[src]), %%xmm2") \
        __ASM_EMIT(MV_SRC "     0x10(%[src]), %%xmm3") \
        __ASM_EMIT("mulps       %%xmm0, %%xmm2") \
        __ASM_EMIT("mulps       %%xmm1, %%xmm3") \
        __ASM_EMIT("movaps      %%xmm2, 0x00(%[dst])") \
        __ASM_EMIT("movaps      %%xmm3, 0x10(%[dst])") \
        \
        __ASM_EMIT("add         $0x20, %[src]") \
        __ASM_EMIT("add         $0x20, %[dst]") \
        /* 4x block */ \
        __ASM_EMIT("6:") \
        __ASM_EMIT("test        $0x4, %[count]") \
        __ASM_EMIT("jz          7f") \
        __ASM_EMIT(MV_SRC "     0x00(%[src]), %%xmm2") \
        __ASM_EMIT("mulps       %%xmm0, %%xmm2") \
        __ASM_EMIT("movaps      %%xmm2, 0x00(%[dst])") \
        \
        __ASM_EMIT("add         $0x10, %[src]") \
        __ASM_EMIT("add         $0x10, %[dst]") \
        /* 1x blocks */ \
        __ASM_EMIT("7:") \
        __ASM_EMIT("and         $0x03, %[count]")    \
        __ASM_EMIT("jz          2000f")    \
        __ASM_EMIT("8:") \
        __ASM_EMIT("movss       (%[src]), %%xmm2") \
        __ASM_EMIT("mulss       %%xmm0, %%xmm2") \
        __ASM_EMIT("movss       %%xmm2, (%[dst])") \
        __ASM_EMIT("add         $0x4, %[src]") \
        __ASM_EMIT("add         $0x4, %[dst]") \
        __ASM_EMIT("dec         %[count]") \
        __ASM_EMIT("jnz         8b")

    __asm__ __volatile__
    (
        __ASM_EMIT("test        %[count], %[count]")
        __ASM_EMIT("jz          2000f")
        __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0")

        // Align destination
        __ASM_EMIT("1:")
        __ASM_EMIT("test        $0x0f, %[dst]")
        __ASM_EMIT("jz          2f")
        __ASM_EMIT("movss       (%[src]), %%xmm2")
        __ASM_EMIT("mulss       %%xmm0, %%xmm2")
        __ASM_EMIT("movss       %%xmm2, (%[dst])")
        __ASM_EMIT("add         $0x4, %[src]")
        __ASM_EMIT("add         $0x4, %[dst]")
        __ASM_EMIT("dec         %[count]")
        __ASM_EMIT("jnz         1b")
        __ASM_EMIT("jmp         2000f")

        // Block operation
        __ASM_EMIT("2:")
        __ASM_EMIT("movaps      %%xmm0, %%xmm1")
        __ASM_EMIT("test        $0x0f, %[src]")
        __ASM_EMIT("jnz         1001f")
            SCALE3_CORE("movaps")
            __ASM_EMIT("jmp         2000f")
        __ASM_EMIT("1001:")
            SCALE3_CORE("movups")

        __ASM_EMIT("2000:")

        : [src] "+r" (src), [dst] "+r"(dst), [count] "+r" (count), [k] "+Yz"(k) :
        : "cc", "memory",
          "%xmm1", "%xmm2", "%xmm3",
          "%xmm4", "%xmm5", "%xmm6", "%xmm7"
    );

    #undef SCALE3_CORE
}

#define SCALE_OP3_CORE(OP, MV_SRC)  \
    __ASM_EMIT("sub         $0x08, %[count]") \
    __ASM_EMIT("jb          4f")    \
    \
    /* 8x blocks */ \
    __ASM_EMIT("3:") \
    __ASM_EMIT(MV_SRC "     0x00(%[src]), %%xmm2") \
    __ASM_EMIT(MV_SRC "     0x10(%[src]), %%xmm3") \
    __ASM_EMIT("movaps      0x00(%[dst]), %%xmm4") \
    __ASM_EMIT("movaps      0x10(%[dst]), %%xmm5") \
    __ASM_EMIT("mulps       %%xmm0, %%xmm2") \
    __ASM_EMIT("mulps       %%xmm1, %%xmm3") \
    __ASM_EMIT(OP "ps       %%xmm2, %%xmm4") \
    __ASM_EMIT(OP "ps       %%xmm3, %%xmm5") \
    __ASM_EMIT("movaps      %%xmm4, 0x00(%[dst])") \
    __ASM_EMIT("movaps      %%xmm5, 0x10(%[dst])") \
    __ASM_EMIT("add         $0x20, %[src]") \
    __ASM_EMIT("add         $0x20, %[dst]") \
    __ASM_EMIT("sub         $0x08, %[count]") \
    __ASM_EMIT("jae         3b") \
    \
    /* 4x block */ \
    __ASM_EMIT("4:") \
    __ASM_EMIT("add         $0x08, %[count]") \
    __ASM_EMIT("test        $0x04, %[count]") \
    __ASM_EMIT("jz          5f") \
    __ASM_EMIT(MV_SRC "     0x00(%[src]), %%xmm2") \
    __ASM_EMIT("movaps      0x00(%[dst]), %%xmm4") \
    __ASM_EMIT("mulps       %%xmm0, %%xmm2") \
    __ASM_EMIT(OP "ps       %%xmm2, %%xmm4") \
    __ASM_EMIT("movaps      %%xmm4, 0x00(%[dst])") \
    __ASM_EMIT("add         $0x10, %[src]") \
    __ASM_EMIT("add         $0x10, %[dst]") \
    /* 1x blocks */ \
    __ASM_EMIT("5:") \
    __ASM_EMIT("and         $0x03, %[count]")    \
    __ASM_EMIT("jz          2000f")    \
    __ASM_EMIT("6:") \
    __ASM_EMIT("movss       0x00(%[src]), %%xmm2") \
    __ASM_EMIT("movss       0x00(%[dst]), %%xmm4") \
    __ASM_EMIT("mulss       %%xmm0, %%xmm2") \
    __ASM_EMIT(OP "ss       %%xmm2, %%xmm4") \
    __ASM_EMIT("movss       %%xmm4, 0x00(%[dst])") \
    __ASM_EMIT("add         $0x4, %[src]") \
    __ASM_EMIT("add         $0x4, %[dst]") \
    __ASM_EMIT("dec         %[count]") \
    __ASM_EMIT("jnz         6b")

#define SCALE_OP3_ALIGN(OP)  \
    __ASM_EMIT("1:") \
    __ASM_EMIT("test        $0x0f, %[dst]") \
    __ASM_EMIT("jz          2f") \
    __ASM_EMIT("movss       0x00(%[src]), %%xmm2") \
    __ASM_EMIT("movss       0x00(%[dst]), %%xmm4") \
    __ASM_EMIT("mulss       %%xmm0, %%xmm2") \
    __ASM_EMIT(OP "ss       %%xmm2, %%xmm4") \
    __ASM_EMIT("movss       %%xmm4, 0x00(%[dst])") \
    __ASM_EMIT("add         $0x4, %[src]") \
    __ASM_EMIT("add         $0x4, %[dst]") \
    __ASM_EMIT("dec         %[count]") \
    __ASM_EMIT("jnz         1b") \
    __ASM_EMIT("jmp         2000f")

void scale_add3(float *dst, const float *src, float k, size_t count)
{
    __asm__ __volatile__
    (
        __ASM_EMIT("test        %[count], %[count]")
        __ASM_EMIT("jz          2000f")
        __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0")

        // Align destination
        SCALE_OP3_ALIGN("add")

        // Block operation
        __ASM_EMIT("2:")
        __ASM_EMIT("movaps      %%xmm0, %%xmm1")
        __ASM_EMIT("test        $0x0f, %[src]")
        __ASM_EMIT("jnz         1001f")
            SCALE_OP3_CORE("add", "movaps")
            __ASM_EMIT("jmp         2000f")
        __ASM_EMIT("1001:")
            SCALE_OP3_CORE("add", "movups")

        __ASM_EMIT("2000:")

        : [dst] "+r"(dst), [src] "+r"(src), [count] "+r" (count), [k] "+Yz"(k) :
        : "cc", "memory",
          "%xmm1", "%xmm2", "%xmm3",
          "%xmm4", "%xmm5", "%xmm6", "%xmm7"
    );
}

void scale_sub3(float *dst, const float *src, float k, size_t count)
{
    __asm__ __volatile__
    (
        __ASM_EMIT("test        %[count], %[count]")
        __ASM_EMIT("jz          2000f")
        __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0")

        // Align destination
        SCALE_OP3_ALIGN("sub")

        // Block operation
        __ASM_EMIT("2:")
        __ASM_EMIT("movaps      %%xmm0, %%xmm1")
        __ASM_EMIT("test        $0x0f, %[src]")
        __ASM_EMIT("jnz         1001f")
            SCALE_OP3_CORE("sub", "movaps")
            __ASM_EMIT("jmp         2000f")
        __ASM_EMIT("1001:")
            SCALE_OP3_CORE("sub", "movups")

        __ASM_EMIT("2000:")

        : [dst] "+r"(dst), [src] "+r"(src), [count] "+r" (count), [k] "+Yz"(k) :
        : "cc", "memory",
          "%xmm1", "%xmm2", "%xmm3",
          "%xmm4", "%xmm5", "%xmm6", "%xmm7"
    );
}

void scale_mul3(float *dst, const float *src, float k, size_t count)
{
    __asm__ __volatile__
    (
        __ASM_EMIT("test        %[count], %[count]")
        __ASM_EMIT("jz          2000f")
        __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0")

        // Align destination
        SCALE_OP3_ALIGN("mul")

        // Block operation
        __ASM_EMIT("2:")
        __ASM_EMIT("movaps      %%xmm0, %%xmm1")
        __ASM_EMIT("test        $0x0f, %[src]")
        __ASM_EMIT("jnz         1001f")
            SCALE_OP3_CORE("mul", "movaps")
            __ASM_EMIT("jmp         2000f")
        __ASM_EMIT("1001:")
            SCALE_OP3_CORE("mul", "movups")

        __ASM_EMIT("2000:")

        : [dst] "+r"(dst), [src] "+r"(src), [count] "+r" (count), [k] "+Yz"(k) :
        : "cc", "memory",
          "%xmm1", "%xmm2", "%xmm3",
          "%xmm4", "%xmm5", "%xmm6", "%xmm7"
    );
}

void scale_div3(float *dst, const float *src, float k, size_t count)
{
    __asm__ __volatile__
    (
        __ASM_EMIT("test        %[count], %[count]")
        __ASM_EMIT("jz          2000f")
        __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0")

        // Align destination
        SCALE_OP3_ALIGN("div")

        // Block operation
        __ASM_EMIT("2:")
        __ASM_EMIT("movaps      %%xmm0, %%xmm1")
        __ASM_EMIT("test        $0x0f, %[src]")
        __ASM_EMIT("jnz         1001f")
            SCALE_OP3_CORE("div", "movaps")
            __ASM_EMIT("jmp         2000f")
        __ASM_EMIT("1001:")
            SCALE_OP3_CORE("div", "movups")

        __ASM_EMIT("2000:")

        : [dst] "+r"(dst), [src] "+r"(src), [count] "+r" (count), [k] "+Yz"(k) :
        : "cc", "memory",
          "%xmm1", "%xmm2", "%xmm3",
          "%xmm4", "%xmm5", "%xmm6", "%xmm7"
    );
}

#undef SCALE_OP3_ALIGN
#undef SCALE_OP3_CORE

#define SCALE_OP4_CORE(OP, MV_SRC1, MV_SRC2)  \
    __ASM_EMIT("sub         $0x08, %[count]") \
    __ASM_EMIT("jb          4f")    \
    \
    /* 8x blocks */ \
    __ASM_EMIT("3:") \
    __ASM_EMIT(MV_SRC2 "    0x00(%[src2]), %%xmm2") \
    __ASM_EMIT(MV_SRC2 "    0x10(%[src2]), %%xmm3") \
    __ASM_EMIT(MV_SRC1 "    0x00(%[src1]), %%xmm4") \
    __ASM_EMIT(MV_SRC1 "    0x10(%[src1]), %%xmm5") \
    __ASM_EMIT("mulps       %%xmm0, %%xmm2") \
    __ASM_EMIT("mulps       %%xmm1, %%xmm3") \
    __ASM_EMIT(OP "ps       %%xmm2, %%xmm4") \
    __ASM_EMIT(OP "ps       %%xmm3, %%xmm5") \
    __ASM_EMIT("movaps      %%xmm4, 0x00(%[dst])") \
    __ASM_EMIT("movaps      %%xmm5, 0x10(%[dst])") \
    __ASM_EMIT("add         $0x20, %[src2]") \
    __ASM_EMIT("add         $0x20, %[src1]") \
    __ASM_EMIT("add         $0x20, %[dst]") \
    __ASM_EMIT("sub         $0x08, %[count]") \
    __ASM_EMIT("jae         3b") \
    \
    /* 4x block */ \
    __ASM_EMIT("4:") \
    __ASM_EMIT("add         $0x08, %[count]") \
    __ASM_EMIT("test        $0x04, %[count]") \
    __ASM_EMIT("jz          5f") \
    __ASM_EMIT(MV_SRC2 "    0x00(%[src2]), %%xmm2") \
    __ASM_EMIT(MV_SRC1 "    0x00(%[src1]), %%xmm4") \
    __ASM_EMIT("mulps       %%xmm0, %%xmm2") \
    __ASM_EMIT(OP "ps       %%xmm2, %%xmm4") \
    __ASM_EMIT("movaps      %%xmm4, 0x00(%[dst])") \
    __ASM_EMIT("add         $0x10, %[src2]") \
    __ASM_EMIT("add         $0x10, %[src1]") \
    __ASM_EMIT("add         $0x10, %[dst]") \
    /* 1x blocks */ \
    __ASM_EMIT("5:") \
    __ASM_EMIT("and         $0x03, %[count]")    \
    __ASM_EMIT("jz          2000f")    \
    __ASM_EMIT("6:") \
    __ASM_EMIT("movss       0x00(%[src2]), %%xmm2") \
    __ASM_EMIT("movss       0x00(%[src1]), %%xmm4") \
    __ASM_EMIT("mulss       %%xmm0, %%xmm2") \
    __ASM_EMIT(OP "ss       %%xmm2, %%xmm4") \
    __ASM_EMIT("movss       %%xmm4, 0x00(%[dst])") \
    __ASM_EMIT("add         $0x4, %[src2]") \
    __ASM_EMIT("add         $0x4, %[src1]") \
    __ASM_EMIT("add         $0x4, %[dst]") \
    __ASM_EMIT("dec         %[count]") \
    __ASM_EMIT("jnz         6b")

#define SCALE_OP4_ALIGN(OP)  \
    __ASM_EMIT("1:") \
    __ASM_EMIT("test        $0x0f, %[dst]") \
    __ASM_EMIT("jz          2f") \
    __ASM_EMIT("movss       0x00(%[src2]), %%xmm2") \
    __ASM_EMIT("movss       0x00(%[src1]), %%xmm4") \
    __ASM_EMIT("mulss       %%xmm0, %%xmm2") \
    __ASM_EMIT(OP "ss       %%xmm2, %%xmm4") \
    __ASM_EMIT("movss       %%xmm4, 0x00(%[dst])") \
    __ASM_EMIT("add         $0x4, %[src2]") \
    __ASM_EMIT("add         $0x4, %[src1]") \
    __ASM_EMIT("add         $0x4, %[dst]") \
    __ASM_EMIT("dec         %[count]") \
    __ASM_EMIT("jnz         1b") \
    __ASM_EMIT("jmp         2000f")

void scale_add4(float *dst, const float *src1, const float *src2, float k, size_t count)
{
    __asm__ __volatile__
    (
        __ASM_EMIT("test        %[count], %[count]")
        __ASM_EMIT("jz          2000f")
        __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0")

        // Align destination
        SCALE_OP4_ALIGN("add")

        // Block operation
        __ASM_EMIT("2:")
        __ASM_EMIT("movaps      %%xmm0, %%xmm1")
        __ASM_EMIT("test        $0x0f, %[src1]")
        __ASM_EMIT("jnz         1010f")
            __ASM_EMIT("test        $0x0f, %[src2]")
            __ASM_EMIT("jnz         1001f")
                SCALE_OP4_CORE("add", "movaps", "movaps")
                __ASM_EMIT("jmp         2000f")
            __ASM_EMIT("1001:")
                SCALE_OP4_CORE("add", "movaps", "movups")
                __ASM_EMIT("jmp         2000f")

        __ASM_EMIT("1010:")
            __ASM_EMIT("test        $0x0f, %[src2]")
            __ASM_EMIT("jnz         1011f")
                SCALE_OP4_CORE("add", "movups", "movaps")
                __ASM_EMIT("jmp         2000f")
            __ASM_EMIT("1011:")
                SCALE_OP4_CORE("add", "movups", "movups")

        __ASM_EMIT("2000:")

        : [dst] "+r"(dst), [src1] "+r"(src1), [src2] "+r"(src2), [count] "+r" (count), [k] "+Yz"(k) :
        : "cc", "memory",
          "%xmm1", "%xmm2", "%xmm3",
          "%xmm4", "%xmm5", "%xmm6", "%xmm7"
    );
}

void scale_sub4(float *dst, const float *src1, const float *src2, float k, size_t count)
{
    __asm__ __volatile__
    (
        __ASM_EMIT("test        %[count], %[count]")
        __ASM_EMIT("jz          2000f")
        __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0")

        // Align destination
        SCALE_OP4_ALIGN("sub")

        // Block operation
        __ASM_EMIT("2:")
        __ASM_EMIT("movaps      %%xmm0, %%xmm1")
        __ASM_EMIT("test        $0x0f, %[src1]")
        __ASM_EMIT("jnz         1010f")
            __ASM_EMIT("test        $0x0f, %[src2]")
            __ASM_EMIT("jnz         1001f")
                SCALE_OP4_CORE("sub", "movaps", "movaps")
                __ASM_EMIT("jmp         2000f")
            __ASM_EMIT("1001:")
                SCALE_OP4_CORE("sub", "movaps", "movups")
                __ASM_EMIT("jmp         2000f")

        __ASM_EMIT("1010:")
            __ASM_EMIT("test        $0x0f, %[src2]")
            __ASM_EMIT("jnz         1011f")
                SCALE_OP4_CORE("sub", "movups", "movaps")
                __ASM_EMIT("jmp         2000f")
            __ASM_EMIT("1011:")
                SCALE_OP4_CORE("sub", "movups", "movups")

        __ASM_EMIT("2000:")

        : [dst] "+r"(dst), [src1] "+r"(src1), [src2] "+r"(src2), [count] "+r" (count), [k] "+Yz"(k) :
        : "cc", "memory",
          "%xmm1", "%xmm2", "%xmm3",
          "%xmm4", "%xmm5", "%xmm6", "%xmm7"
    );
}

void scale_mul4(float *dst, const float *src1, const float *src2, float k, size_t count)
{
    __asm__ __volatile__
    (
        __ASM_EMIT("test        %[count], %[count]")
        __ASM_EMIT("jz          2000f")
        __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0")

        // Align destination
        SCALE_OP4_ALIGN("mul")

        // Block operation
        __ASM_EMIT("2:")
        __ASM_EMIT("movaps      %%xmm0, %%xmm1")
        __ASM_EMIT("test        $0x0f, %[src1]")
        __ASM_EMIT("jnz         1010f")
            __ASM_EMIT("test        $0x0f, %[src2]")
            __ASM_EMIT("jnz         1001f")
                SCALE_OP4_CORE("mul", "movaps", "movaps")
                __ASM_EMIT("jmp         2000f")
            __ASM_EMIT("1001:")
                SCALE_OP4_CORE("mul", "movaps", "movups")
                __ASM_EMIT("jmp         2000f")

        __ASM_EMIT("1010:")
            __ASM_EMIT("test        $0x0f, %[src2]")
            __ASM_EMIT("jnz         1011f")
                SCALE_OP4_CORE("mul", "movups", "movaps")
                __ASM_EMIT("jmp         2000f")
            __ASM_EMIT("1011:")
                SCALE_OP4_CORE("mul", "movups", "movups")

        __ASM_EMIT("2000:")

        : [dst] "+r"(dst), [src1] "+r"(src1), [src2] "+r"(src2), [count] "+r" (count), [k] "+Yz"(k) :
        : "cc", "memory",
          "%xmm1", "%xmm2", "%xmm3",
          "%xmm4", "%xmm5", "%xmm6", "%xmm7"
    );
}

void scale_div4(float *dst, const float *src1, const float *src2, float k, size_t count)
{
    __asm__ __volatile__
    (
        __ASM_EMIT("test        %[count], %[count]")
        __ASM_EMIT("jz          2000f")
        __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0")

        // Align destination
        SCALE_OP4_ALIGN("div")

        // Block operation
        __ASM_EMIT("2:")
        __ASM_EMIT("movaps      %%xmm0, %%xmm1")
        __ASM_EMIT("test        $0x0f, %[src1]")
        __ASM_EMIT("jnz         1010f")
            __ASM_EMIT("test        $0x0f, %[src2]")
            __ASM_EMIT("jnz         1001f")
                SCALE_OP4_CORE("div", "movaps", "movaps")
                __ASM_EMIT("jmp         2000f")
            __ASM_EMIT("1001:")
                SCALE_OP4_CORE("div", "movaps", "movups")
                __ASM_EMIT("jmp         2000f")

        __ASM_EMIT("1010:")
            __ASM_EMIT("test        $0x0f, %[src2]")
            __ASM_EMIT("jnz         1011f")
                SCALE_OP4_CORE("div", "movups", "movaps")
                __ASM_EMIT("jmp         2000f")
            __ASM_EMIT("1011:")
                SCALE_OP4_CORE("div", "movups", "movups")

        __ASM_EMIT("2000:")

        : [dst] "+r"(dst), [src1] "+r"(src1), [src2] "+r"(src2), [count] "+r" (count), [k] "+Yz"(k) :
        : "cc", "memory",
          "%xmm1", "%xmm2", "%xmm3",
          "%xmm4", "%xmm5", "%xmm6", "%xmm7"
    );
}

#undef SCALE_OP4_ALIGN
#undef SCALE_OP4_CORE

#endif /* DSP_ARCH_X86_SSE_LMATH_H_ */
