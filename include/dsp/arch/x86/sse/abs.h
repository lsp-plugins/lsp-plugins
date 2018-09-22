/*
 * abs.h
 *
 *  Created on: 21 мар. 2017 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_SSE_ABS_H_
#define DSP_ARCH_X86_SSE_ABS_H_

#ifndef DSP_ARCH_X86_SSE_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_SSE_IMPL */

namespace sse
{
#define ABS_CORE(DST, SRC)  \
    __ASM_EMIT("xor         %[off], %[off]") \
    __ASM_EMIT("movaps      %[X_SIGN], %%xmm0") \
    __ASM_EMIT("sub         $24, %[count]") \
    __ASM_EMIT("movaps      %%xmm0, %%xmm1") \
    __ASM_EMIT("jb          2f")    \
    /* 24x blocks */ \
    __ASM_EMIT("1:") \
    __ASM_EMIT("movups      0x00(%[" SRC "], %[off]), %%xmm2") \
    __ASM_EMIT("movups      0x10(%[" SRC "], %[off]), %%xmm3") \
    __ASM_EMIT("movups      0x20(%[" SRC "], %[off]), %%xmm4") \
    __ASM_EMIT("movups      0x30(%[" SRC "], %[off]), %%xmm5") \
    __ASM_EMIT("movups      0x40(%[" SRC "], %[off]), %%xmm6") \
    __ASM_EMIT("movups      0x50(%[" SRC "], %[off]), %%xmm7") \
    __ASM_EMIT("andps       %%xmm0, %%xmm2") \
    __ASM_EMIT("andps       %%xmm1, %%xmm3") \
    __ASM_EMIT("andps       %%xmm0, %%xmm4") \
    __ASM_EMIT("andps       %%xmm1, %%xmm5") \
    __ASM_EMIT("andps       %%xmm0, %%xmm6") \
    __ASM_EMIT("andps       %%xmm1, %%xmm7") \
    __ASM_EMIT("movups      %%xmm2, 0x00(%[" DST "], %[off])") \
    __ASM_EMIT("movups      %%xmm3, 0x10(%[" DST "], %[off])") \
    __ASM_EMIT("movups      %%xmm4, 0x20(%[" DST "], %[off])") \
    __ASM_EMIT("movups      %%xmm5, 0x30(%[" DST "], %[off])") \
    __ASM_EMIT("movups      %%xmm6, 0x40(%[" DST "], %[off])") \
    __ASM_EMIT("movups      %%xmm7, 0x50(%[" DST "], %[off])") \
    __ASM_EMIT("add         $0x60, %[off]") \
    __ASM_EMIT("sub         $24, %[count]") \
    __ASM_EMIT("jae         1b") \
    /* 16x block */ \
    __ASM_EMIT("2:") \
    __ASM_EMIT("add         $8, %[count]") \
    __ASM_EMIT("jl          4f") \
    __ASM_EMIT("movups      0x00(%[" SRC "], %[off]), %%xmm2") \
    __ASM_EMIT("movups      0x10(%[" SRC "], %[off]), %%xmm3") \
    __ASM_EMIT("movups      0x20(%[" SRC "], %[off]), %%xmm4") \
    __ASM_EMIT("movups      0x30(%[" SRC "], %[off]), %%xmm5") \
    __ASM_EMIT("andps       %%xmm0, %%xmm2") \
    __ASM_EMIT("andps       %%xmm1, %%xmm3") \
    __ASM_EMIT("andps       %%xmm0, %%xmm4") \
    __ASM_EMIT("andps       %%xmm1, %%xmm5") \
    __ASM_EMIT("movups      %%xmm2, 0x00(%[" DST "], %[off])") \
    __ASM_EMIT("movups      %%xmm3, 0x10(%[" DST "], %[off])") \
    __ASM_EMIT("movups      %%xmm4, 0x20(%[" DST "], %[off])") \
    __ASM_EMIT("movups      %%xmm5, 0x30(%[" DST "], %[off])") \
    __ASM_EMIT("sub         $16, %[count]") \
    __ASM_EMIT("add         $0x40, %[off]") \
    /* 8x block */ \
    __ASM_EMIT("4:") \
    __ASM_EMIT("add         $8, %[count]") \
    __ASM_EMIT("jl          6f") \
    __ASM_EMIT("movups      0x00(%[" SRC "], %[off]), %%xmm2") \
    __ASM_EMIT("movups      0x10(%[" SRC "], %[off]), %%xmm3") \
    __ASM_EMIT("andps       %%xmm0, %%xmm2") \
    __ASM_EMIT("andps       %%xmm1, %%xmm3") \
    __ASM_EMIT("movups      %%xmm2, 0x00(%[" DST "], %[off])") \
    __ASM_EMIT("movups      %%xmm3, 0x10(%[" DST "], %[off])") \
    __ASM_EMIT("sub         $8, %[count]") \
    __ASM_EMIT("add         $0x20, %[off]") \
    /* 4x block */ \
    __ASM_EMIT("6:") \
    __ASM_EMIT("add         $4, %[count]") \
    __ASM_EMIT("jl          8f") \
    __ASM_EMIT("movups      0x00(%[" SRC "], %[off]), %%xmm2") \
    __ASM_EMIT("andps       %%xmm0, %%xmm2") \
    __ASM_EMIT("movups      %%xmm2, 0x00(%[" DST "], %[off])") \
    __ASM_EMIT("sub         $4, %[count]") \
    __ASM_EMIT("add         $0x10, %[off]") \
    /* 1x block */ \
    __ASM_EMIT("8:") \
    __ASM_EMIT("add         $3, %[count]") \
    __ASM_EMIT("jl          10f") \
    __ASM_EMIT("9:") \
    __ASM_EMIT("movss       0x00(%[" SRC "], %[off]), %%xmm2") \
    __ASM_EMIT("andps       %%xmm0, %%xmm2") \
    __ASM_EMIT("movss       %%xmm2, 0x00(%[" DST "], %[off])") \
    __ASM_EMIT("add         $0x04, %[off]") \
    __ASM_EMIT("dec         %[count]") \
    __ASM_EMIT("jge         9b") \
    __ASM_EMIT("10:")

    void abs1(float *dst, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            ABS_CORE("dst", "dst")
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r" (dst),
              [X_SIGN] "m" (X_SIGN)
            : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void abs2(float *dst, const float *src, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM
        (
            ABS_CORE("dst", "src")
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r" (dst), [src] "r" (src),
              [X_SIGN] "m" (X_SIGN)
            : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

#undef ABS_CORE

    #define ABS_OP2_CORE(OP, MV_SRC)  \
        __ASM_EMIT("sub         $0x08, %[count]") \
        __ASM_EMIT("jb          4f")    \
        \
        /* 8x blocks */ \
        __ASM_EMIT("3:") \
        __ASM_EMIT(MV_SRC "     0x00(%[src]), %%xmm2") \
        __ASM_EMIT(MV_SRC "     0x10(%[src]), %%xmm3") \
        __ASM_EMIT("movaps      0x00(%[dst]), %%xmm4") \
        __ASM_EMIT("movaps      0x10(%[dst]), %%xmm5") \
        __ASM_EMIT("andps       %%xmm0, %%xmm2") \
        __ASM_EMIT("andps       %%xmm1, %%xmm3") \
        __ASM_EMIT(OP "ps       %%xmm2, %%xmm4") \
        __ASM_EMIT(OP "ps       %%xmm3, %%xmm5") \
        __ASM_EMIT("movaps      %%xmm4, 0x00(%[dst])") \
        __ASM_EMIT("movaps      %%xmm5, 0x10(%[dst])") \
        __ASM_EMIT("add         $0x20, %[src]") \
        __ASM_EMIT("add         $0x20, %[dst]") \
        __ASM_EMIT("sub         $0x08, %[count]") \
        __ASM_EMIT("jae         3b") \
        /* 4x block */ \
        __ASM_EMIT("4:") \
        __ASM_EMIT("add         $0x08, %[count]") \
        __ASM_EMIT("test        $0x04, %[count]") \
        __ASM_EMIT("jz          3000f") \
        __ASM_EMIT(MV_SRC "     0x00(%[src]), %%xmm2") \
        __ASM_EMIT("movaps      0x00(%[dst]), %%xmm4") \
        __ASM_EMIT("andps       %%xmm0, %%xmm2") \
        __ASM_EMIT(OP "ps       %%xmm2, %%xmm4") \
        __ASM_EMIT("movaps      %%xmm4, 0x00(%[dst])") \
        __ASM_EMIT("add         $0x10, %[src]") \
        __ASM_EMIT("add         $0x10, %[dst]") \

    #define ABS_OP2_ALIGN(OP)  \
        __ASM_EMIT("1:") \
        __ASM_EMIT("test        $0x0f, %[dst]") \
        __ASM_EMIT("jz          2f") \
        __ASM_EMIT("movss       0x00(%[src]), %%xmm2") \
        __ASM_EMIT("movss       0x00(%[dst]), %%xmm4") \
        __ASM_EMIT("andps       %%xmm0, %%xmm2") \
        __ASM_EMIT(OP "ss       %%xmm2, %%xmm4") \
        __ASM_EMIT("movss       %%xmm4, 0x00(%[dst])") \
        __ASM_EMIT("add         $0x4, %[src]") \
        __ASM_EMIT("add         $0x4, %[dst]") \
        __ASM_EMIT("dec         %[count]") \
        __ASM_EMIT("jnz         1b") \
        __ASM_EMIT("jmp         2000f")

    #define ABS_OP2_TAIL(OP) \
        /* 1x blocks */ \
        __ASM_EMIT("3000:") \
        __ASM_EMIT("and         $0x03, %[count]") \
        __ASM_EMIT("jz          2000f") \
        __ASM_EMIT("6:") \
        __ASM_EMIT("movss       0x00(%[src]), %%xmm2") \
        __ASM_EMIT("movss       0x00(%[dst]), %%xmm4") \
        __ASM_EMIT("andps       %%xmm0, %%xmm2") \
        __ASM_EMIT(OP "ss       %%xmm2, %%xmm4") \
        __ASM_EMIT("movss       %%xmm4, 0x00(%[dst])") \
        __ASM_EMIT("add         $0x4, %[src]") \
        __ASM_EMIT("add         $0x4, %[dst]") \
        __ASM_EMIT("dec         %[count]") \
        __ASM_EMIT("jnz         6b")


    #define ABS_OP2_BODY(OP) \
        ARCH_X86_ASM \
        ( \
            __ASM_EMIT("test        %[count], %[count]") \
            __ASM_EMIT("jz          2000f") \
            __ASM_EMIT("movaps      %[mask], %%xmm0") \
            \
            /* Align destination */ \
            ABS_OP2_ALIGN(OP)  \
            \
            /* Block operation */  \
            __ASM_EMIT("2:") \
            __ASM_EMIT("movaps      %%xmm0, %%xmm1") \
            __ASM_EMIT("test        $0x0f, %[src]") \
            __ASM_EMIT("jnz         1001f") \
                ABS_OP2_CORE(OP, "movaps") \
                __ASM_EMIT("jmp         3000f") \
            __ASM_EMIT("1001:") \
                ABS_OP2_CORE(OP, "movups") \
            \
            ABS_OP2_TAIL(OP) \
            \
            __ASM_EMIT("2000:") \
            \
            : [dst] "+r"(dst), [src] "+r"(src), [count] "+r" (count) \
            : [mask] "m" (X_SIGN) \
            : "cc", "memory", \
              "%xmm0", "%xmm1", "%xmm2", "%xmm3", \
              "%xmm4", "%xmm5", "%xmm6", "%xmm7" \
        );

    void abs_add2(float *dst, const float *src, size_t count)
    {
        ABS_OP2_BODY("add");
    }

    void abs_sub2(float *dst, const float *src, size_t count)
    {
        ABS_OP2_BODY("sub");
    }

    void abs_mul2(float *dst, const float *src, size_t count)
    {
        ABS_OP2_BODY("mul");
    }

    void abs_div2(float *dst, const float *src, size_t count)
    {
        ABS_OP2_BODY("div");
    }

    #undef ABS_OP2_BODY
    #undef ABS_OP2_ALIGN
    #undef ABS_OP2_CORE
    #undef ABS_OP2_TAIL
}

#endif /* DSP_ARCH_X86_SSE_ABS_H_ */
