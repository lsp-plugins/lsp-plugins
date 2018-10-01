/*
 * float.h
 *
 *  Created on: 06 авг. 2016 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_SSE_FLOAT_H_
#define DSP_ARCH_X86_SSE_FLOAT_H_

#ifndef DSP_ARCH_X86_SSE_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_SSE_IMPL */

namespace sse
{
#define MULTIPLE_SATURATION_BODY(dst, src) \
    /* Prepare values */ \
    __ASM_EMIT("movdqa          0x00 + %[X_IARGS], %%xmm7")         /* xmm7 = -1 */ \
    __ASM_EMIT("movdqa          0x10 + %[X_IARGS], %%xmm6")         /* xmm6 = +inf */ \
    __ASM_EMIT("movdqa          0x20 + %[X_IARGS], %%xmm5")         /* xmm5 = -inf */ \
    __ASM_EMIT("xor             %[off], %[off]") \
    __ASM_EMIT("sub             $4, %[count]") \
    __ASM_EMIT("jb              2f") \
    \
    /* x4 blocks */ \
    __ASM_EMIT("1:") \
    __ASM_EMIT("movdqu          0x00(%[" src "], %[off]), %%xmm0")  /* xmm0 = s */ \
    __ASM_EMIT("movdqa          %%xmm0, %%xmm1")                    /* xmm1 = s */ \
    __ASM_EMIT("pcmpgtd         %%xmm7, %%xmm1")                    /* xmm1 = [s >= 0] */ \
    /* +NAN, +INF */ \
    __ASM_EMIT("movdqa          %%xmm0, %%xmm3")                    /* xmm3 = src */ \
    __ASM_EMIT("movdqa          %%xmm0, %%xmm4")                    /* xmm4 = src */ \
    __ASM_EMIT("pcmpgtd         %%xmm6, %%xmm3")                    /* xmm3 = /src > +inf/ */ \
    __ASM_EMIT("pcmpeqd         %%xmm6, %%xmm4")                    /* xmm4 = /src == +inf/ */ \
    __ASM_EMIT("movdqa          %%xmm3, %%xmm2")                    /* xmm2 = /src > +inf/ */ \
    __ASM_EMIT("por             %%xmm4, %%xmm2")                    /* xmm2 = /src >= +inf/ */ \
    __ASM_EMIT("pand            0x30 + %[X_IARGS], %%xmm3")         /* xmm3 = +nan * /src > +inf/ */ \
    __ASM_EMIT("pand            0x40 + %[X_IARGS], %%xmm4")         /* xmm4 = +inf * /src == +inf/ */ \
    __ASM_EMIT("pand            %%xmm1, %%xmm2")                    /* xmm2 = /src >= +inf && src >= 0/ */ \
    __ASM_EMIT("por             %%xmm3, %%xmm4")                    /* xmm4 = +nan * /src > +inf/ + +inf * /src == +inf/ */ \
    __ASM_EMIT("pandn           %%xmm0, %%xmm2")                    /* xmm2 = src * !/src >= +inf && src >= 0/ */ \
    __ASM_EMIT("por             %%xmm4, %%xmm2")                    /* xmm2 = src * !/src >= +inf && src >= 0/ + <+nan> * /src > +inf/ + <+inf> * /src == +inf/ */ \
    /* -NAN, -INF */  \
    __ASM_EMIT("movdqa          %%xmm0, %%xmm3")                    /* xmm3 = src */  \
    __ASM_EMIT("movdqa          %%xmm0, %%xmm4")                    /* xmm4 = src */ \
    __ASM_EMIT("pcmpgtd         %%xmm5, %%xmm3")                    /* xmm3 = /src > -inf/ */ \
    __ASM_EMIT("pcmpeqd         %%xmm5, %%xmm4")                    /* xmm4 = /src == -inf/ */ \
    __ASM_EMIT("movdqa          %%xmm3, %%xmm0")                    /* xmm0 = /src > -inf/ */ \
    __ASM_EMIT("por             %%xmm4, %%xmm0")                    /* xmm0 = /src >= -inf/ */ \
    __ASM_EMIT("pand            0x50 + %[X_IARGS], %%xmm3")         /* xmm3 = -nan * /src > -inf/ */ \
    __ASM_EMIT("pand            0x60 + %[X_IARGS], %%xmm4")         /* xmm4 = -inf * /src == -inf/ */ \
    __ASM_EMIT("pandn           %%xmm0, %%xmm1")                    /* xmm1 = /src >= -inf && src < 0/ */ \
    __ASM_EMIT("por             %%xmm3, %%xmm4")                    /* xmm4 = -nan * /src > -inf/ + -inf * /src == -inf/ */ \
    __ASM_EMIT("pand            %%xmm1, %%xmm4") \
    __ASM_EMIT("pandn           %%xmm2, %%xmm1") \
    __ASM_EMIT("por             %%xmm4, %%xmm1") \
    __ASM_EMIT("movdqu          %%xmm1, 0x00(%[" dst "], %[off])") \
    __ASM_EMIT("add             $0x10, %[off]") \
    __ASM_EMIT("sub             $4, %[count]") \
    __ASM_EMIT("jae             1b") \
    \
    __ASM_EMIT("2:") \
    __ASM_EMIT("add             $3, %[count]") \
    __ASM_EMIT("jl              4f") \
    /* x1 blocks */ \
    __ASM_EMIT("3:") \
    __ASM_EMIT("movd            0x00(%[" src "], %[off]), %%xmm0")  /* xmm0 = s */ \
    __ASM_EMIT("movdqa          %%xmm0, %%xmm1")                    /* xmm1 = s */ \
    __ASM_EMIT("pcmpgtd         %%xmm7, %%xmm1")                    /* xmm1 = [s >= 0] */ \
    /* +NAN, +INF */ \
    __ASM_EMIT("movdqa          %%xmm0, %%xmm3")                    /* xmm3 = src */ \
    __ASM_EMIT("movdqa          %%xmm0, %%xmm4")                    /* xmm4 = src */ \
    __ASM_EMIT("pcmpgtd         %%xmm6, %%xmm3")                    /* xmm3 = /src > +inf/ */ \
    __ASM_EMIT("pcmpeqd         %%xmm6, %%xmm4")                    /* xmm4 = /src == +inf/ */ \
    __ASM_EMIT("movdqa          %%xmm3, %%xmm2")                    /* xmm2 = /src > +inf/ */ \
    __ASM_EMIT("por             %%xmm4, %%xmm2")                    /* xmm2 = /src >= +inf/ */ \
    __ASM_EMIT("pand            0x30 + %[X_IARGS], %%xmm3")         /* xmm3 = +nan * /src > +inf/ */ \
    __ASM_EMIT("pand            0x40 + %[X_IARGS], %%xmm4")         /* xmm4 = +inf * /src == +inf/ */ \
    __ASM_EMIT("pand            %%xmm1, %%xmm2")                    /* xmm2 = /src >= +inf && src >= 0/ */ \
    __ASM_EMIT("por             %%xmm3, %%xmm4")                    /* xmm4 = +nan * /src > +inf/ + +inf * /src == +inf/ */ \
    __ASM_EMIT("pandn           %%xmm0, %%xmm2")                    /* xmm2 = src * !/src >= +inf && src >= 0/ */ \
    __ASM_EMIT("por             %%xmm4, %%xmm2")                    /* xmm2 = src * !/src >= +inf && src >= 0/ + <+nan> * /src > +inf/ + <+inf> * /src == +inf/ */ \
    /* -NAN, -INF */  \
    __ASM_EMIT("movdqa          %%xmm0, %%xmm3")                    /* xmm3 = src */  \
    __ASM_EMIT("movdqa          %%xmm0, %%xmm4")                    /* xmm4 = src */ \
    __ASM_EMIT("pcmpgtd         %%xmm5, %%xmm3")                    /* xmm3 = /src > -inf/ */ \
    __ASM_EMIT("pcmpeqd         %%xmm5, %%xmm4")                    /* xmm4 = /src == -inf/ */ \
    __ASM_EMIT("movdqa          %%xmm3, %%xmm0")                    /* xmm0 = /src > -inf/ */ \
    __ASM_EMIT("por             %%xmm4, %%xmm0")                    /* xmm0 = /src >= -inf/ */ \
    __ASM_EMIT("pand            0x50 + %[X_IARGS], %%xmm3")         /* xmm3 = -nan * /src > -inf/ */ \
    __ASM_EMIT("pand            0x60 + %[X_IARGS], %%xmm4")         /* xmm4 = -inf * /src == -inf/ */ \
    __ASM_EMIT("pandn           %%xmm0, %%xmm1")                    /* xmm1 = /src >= -inf && src < 0/ */ \
    __ASM_EMIT("por             %%xmm3, %%xmm4")                    /* xmm4 = -nan * /src > -inf/ + -inf * /src == -inf/ */ \
    __ASM_EMIT("pand            %%xmm1, %%xmm4") \
    __ASM_EMIT("pandn           %%xmm2, %%xmm1") \
    __ASM_EMIT("por             %%xmm4, %%xmm1") \
    __ASM_EMIT("movd            %%xmm1, 0x00(%[" dst "], %[off])") \
    __ASM_EMIT("add             $0x04, %[off]") \
    __ASM_EMIT("dec             %[count]") \
    __ASM_EMIT("jge             3b") \
    \
    __ASM_EMIT("4:") \

    void copy_saturated(float *dst, const float *src, size_t count)
    {
        size_t off;

        ARCH_X86_ASM
        (
            MULTIPLE_SATURATION_BODY("dst", "src")
            : [count] "+r" (count), [off] "=&r" (off)
            : [dst] "r" (dst), [src] "r" (src),
              [X_IARGS] "o" (SAT_IARGS)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void saturate(float *dst, size_t count)
    {
        size_t off;

        ARCH_X86_ASM
        (
            MULTIPLE_SATURATION_BODY("dst", "dst")
            : [count] "+r" (count), [off] "=&r" (off)
            : [dst] "r" (dst),
              [X_IARGS] "o" (SAT_IARGS)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }
}

#endif /* DSP_ARCH_X86_SSE_FLOAT_H_ */
