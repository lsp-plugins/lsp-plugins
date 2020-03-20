/*
 * graphics.h
 *
 *  Created on: 07 авг. 2016 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_SSE_GRAPHICS_H_
#define DSP_ARCH_X86_SSE_GRAPHICS_H_

#ifndef DSP_ARCH_X86_SSE_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_SSE_IMPL */

namespace sse
{
#define FILL4_CORE \
    __ASM_EMIT("movss       %[c0], %%xmm0")     /* xmm0 = c0 */ \
    __ASM_EMIT("movss       %[c1], %%xmm1")     /* xmm1 = c1 */ \
    __ASM_EMIT("movss       %[c2], %%xmm2")     /* xmm2 = c2 */ \
    __ASM_EMIT("movss       %[c3], %%xmm3")     /* xmm3 = c3 */ \
    __ASM_EMIT("unpcklps    %%xmm1, %%xmm0")    /* xmm0 = c0 c1 ? ? */ \
    __ASM_EMIT("unpcklps    %%xmm3, %%xmm2")    /* xmm2 = c2 c3 ? ? */ \
    __ASM_EMIT("movlhps     %%xmm2, %%xmm0")    /* xmm0 = c0 c1 c2 c3 */ \
    __ASM_EMIT("movaps      %%xmm0, %%xmm1")    /* xmm1 = c0 c1 c2 c3 */ \
    __ASM_EMIT("movaps      %%xmm0, %%xmm2") \
    __ASM_EMIT("movaps      %%xmm1, %%xmm3") \
    \
    /* 8x blocks */ \
    __ASM_EMIT("sub         $8, %[count]") \
    __ASM_EMIT("jb          2f") \
    __ASM_EMIT("movaps      %%xmm0, %%xmm4") \
    __ASM_EMIT("movaps      %%xmm1, %%xmm5") \
    __ASM_EMIT("movaps      %%xmm0, %%xmm6") \
    __ASM_EMIT("movaps      %%xmm1, %%xmm7") \
    __ASM_EMIT("1:") \
    __ASM_EMIT("movups      %%xmm0, 0x00(%[dst])") \
    __ASM_EMIT("movups      %%xmm1, 0x10(%[dst])") \
    __ASM_EMIT("movups      %%xmm2, 0x20(%[dst])") \
    __ASM_EMIT("movups      %%xmm3, 0x30(%[dst])") \
    __ASM_EMIT("movups      %%xmm4, 0x40(%[dst])") \
    __ASM_EMIT("movups      %%xmm5, 0x50(%[dst])") \
    __ASM_EMIT("movups      %%xmm6, 0x60(%[dst])") \
    __ASM_EMIT("movups      %%xmm7, 0x70(%[dst])") \
    __ASM_EMIT("add         $0x80, %[dst]") \
    __ASM_EMIT("sub         $8, %[count]") \
    __ASM_EMIT("jae         1b") \
    /* 4x block */ \
    __ASM_EMIT("2:") \
    __ASM_EMIT("add         $4, %[count]") \
    __ASM_EMIT("jl          4f") \
    __ASM_EMIT("movups      %%xmm0, 0x00(%[dst])") \
    __ASM_EMIT("movups      %%xmm1, 0x10(%[dst])") \
    __ASM_EMIT("movups      %%xmm2, 0x20(%[dst])") \
    __ASM_EMIT("movups      %%xmm3, 0x30(%[dst])") \
    __ASM_EMIT("sub         $4, %[count]") \
    __ASM_EMIT("add         $0x40, %[dst]") \
    /* 2x block */ \
    __ASM_EMIT("4:") \
    __ASM_EMIT("add         $2, %[count]") \
    __ASM_EMIT("jl          6f") \
    __ASM_EMIT("movups      %%xmm0, 0x00(%[dst])") \
    __ASM_EMIT("movups      %%xmm1, 0x10(%[dst])") \
    __ASM_EMIT("sub         $2, %[count]") \
    __ASM_EMIT("add         $0x20, %[dst]") \
    /* 1x block */ \
    __ASM_EMIT("6:") \
    __ASM_EMIT("add         $1, %[count]") \
    __ASM_EMIT("jl          8f") \
    __ASM_EMIT("movups      %%xmm0, 0x00(%[dst])") \
    __ASM_EMIT("8:")

    void fill_rgba(float *dst, float r, float g, float b, float a, size_t count)
    {
        ARCH_X86_ASM
        (
            FILL4_CORE
            : [dst] "+r" (dst), [count] "+r" (count)
            : [c0] "m" (r), [c1] "m" (g), [c2] "m" (b), [c3] "m" (a)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void fill_hsla(float *dst, float h, float s, float l, float a, size_t count)
    {
        ARCH_X86_ASM
        (
            FILL4_CORE
            : [dst] "+r" (dst), [count] "+r" (count)
            : [c0] "m" (h), [c1] "m" (s), [c2] "m" (l), [c3] "m" (a)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

#undef FILL4_CORE

}

#endif /* DSP_ARCH_X86_SSE_GRAPHICS_H_ */
