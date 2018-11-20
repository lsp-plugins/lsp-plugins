/*
 * float.h
 *
 *  Created on: 06 авг. 2016 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_SSE2_FLOAT_H_
#define DSP_ARCH_X86_SSE2_FLOAT_H_

#ifndef DSP_ARCH_X86_SSE2_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_SSE2_IMPL */

namespace sse2
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
    /* +NAN, +INF */ \
    __ASM_EMIT("movdqa          %%xmm0, %%xmm1")                    /* xmm1 = s */ \
    __ASM_EMIT("pcmpgtd         %%xmm7, %%xmm1")                    /* xmm1 = [s >= 0] */ \
    __ASM_EMIT("movdqa          %%xmm0, %%xmm3")                    /* xmm3 = s */ \
    __ASM_EMIT("movdqa          %%xmm0, %%xmm4")                    /* xmm4 = s */ \
    __ASM_EMIT("pcmpgtd         %%xmm6, %%xmm3")                    /* xmm3 = [s > +inf] */ \
    __ASM_EMIT("pcmpeqd         %%xmm6, %%xmm4")                    /* xmm4 = [s == +inf] */ \
    __ASM_EMIT("movdqa          %%xmm3, %%xmm2")                    /* xmm2 = [s > +inf] */ \
    __ASM_EMIT("por             %%xmm4, %%xmm2")                    /* xmm2 = [s >= +inf] */ \
    __ASM_EMIT("pand            0x30 + %[X_IARGS], %%xmm3")         /* xmm3 = X_P_NAN * [s > +inf] */ \
    __ASM_EMIT("pand            0x40 + %[X_IARGS], %%xmm4")         /* xmm4 = X_P_INF * [s == +inf] */ \
    __ASM_EMIT("pand            %%xmm1, %%xmm2")                    /* xmm2 = [s >= +inf && s >= 0] */ \
    __ASM_EMIT("por             %%xmm3, %%xmm4")                    /* xmm4 = X_P_NAN * [s > +inf] + X_P_INF * [s == +inf] */ \
    __ASM_EMIT("pandn           %%xmm0, %%xmm2")                    /* xmm2 = s * ![s >= +inf && s >= 0] */ \
    __ASM_EMIT("por             %%xmm4, %%xmm2")                    /* xmm2 = s * ![s >= +inf && s >= 0] + X_P_NAN * [s > +inf] + X_P_INF * [s == +inf] */ \
    /* -NAN, -INF */  \
    __ASM_EMIT("movdqa          %%xmm0, %%xmm3")                    /* xmm3 = s */  \
    __ASM_EMIT("movdqa          %%xmm0, %%xmm4")                    /* xmm4 = s */ \
    __ASM_EMIT("pcmpgtd         %%xmm5, %%xmm3")                    /* xmm3 = [s > -inf] */ \
    __ASM_EMIT("pcmpeqd         %%xmm5, %%xmm4")                    /* xmm4 = [s == -inf] */ \
    __ASM_EMIT("movdqa          %%xmm3, %%xmm0")                    /* xmm0 = [s > -inf] */ \
    __ASM_EMIT("por             %%xmm4, %%xmm0")                    /* xmm0 = [s >= -inf] */ \
    __ASM_EMIT("pand            0x50 + %[X_IARGS], %%xmm3")         /* xmm3 = X_M_NAN * [s > -inf] */ \
    __ASM_EMIT("pand            0x60 + %[X_IARGS], %%xmm4")         /* xmm4 = X_M_INF * [s == -inf] */ \
    __ASM_EMIT("pandn           %%xmm0, %%xmm1")                    /* xmm1 = [s >= -inf && s < 0] */ \
    __ASM_EMIT("por             %%xmm3, %%xmm4")                    /* xmm4 = X_M_NAN * [s > -inf] + X_M_INF * [s == -inf] */ \
    __ASM_EMIT("pand            %%xmm1, %%xmm4") \
    __ASM_EMIT("pandn           %%xmm2, %%xmm1") \
    __ASM_EMIT("por             %%xmm4, %%xmm1") \
    \
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
    /* +NAN, +INF */ \
    __ASM_EMIT("movdqa          %%xmm0, %%xmm1")                    /* xmm1 = s */ \
    __ASM_EMIT("pcmpgtd         %%xmm7, %%xmm1")                    /* xmm1 = [s >= 0] */ \
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

#undef MULTIPLE_SATURATION_BODY

#define LIMIT_SAT_BODY \
    /* xmm0 = s */ \
    __ASM_EMIT("movaps          %%xmm0, %%xmm1")                /* xmm1 = s */ \
    __ASM_EMIT("movaps          %%xmm0, %%xmm2")                /* xmm2 = s */ \
    __ASM_EMIT("movaps          %%xmm1, %%xmm3")                /* xmm3 = s */ \
    __ASM_EMIT("movaps          %%xmm2, %%xmm4")                /* xmm4 = s */ \
    __ASM_EMIT("cmpps           $5, 0x00 + %[XC], %%xmm1")      /* xmm1 = [ s >= -1 ] */ \
    __ASM_EMIT("cmpps           $2, 0x10 + %[XC], %%xmm2")      /* xmm2 = [ s <= 1 ] */ \
    __ASM_EMIT("andps           0x20 + %[XC], %%xmm3")          /* xmm3 = sign(s) */ \
    __ASM_EMIT("andps           %%xmm2, %%xmm1")                /* xmm1 = [ s >= -1 ] & [ s <= 1 ] */ \
    __ASM_EMIT("andps           0x30 + %[XC], %%xmm4")          /* xmm4 = abs(s) */ \
    __ASM_EMIT("andps           %%xmm1, %%xmm0")                /* xmm0 = s & [ s >= -1 ] & [ s <= 1 ] */ \
    __ASM_EMIT("pcmpgtd         0x40 + %[XC], %%xmm4")          /* xmm4 = abs(s) > +Inf */ \
    __ASM_EMIT("andnps          0x10 + %[XC], %%xmm4")          /* xmm4 = 1 & (abs(s) <= +Inf) */ \
    __ASM_EMIT("orps            %%xmm3, %%xmm4")                /* xmm4 = (1 & (abs(s) == +Inf)) | sign(s) */ \
    __ASM_EMIT("andnps          %%xmm4, %%xmm1")                /* xmm1 = ((1 & (abs(s) == +Inf)) | sign(s)) & ([ s < -1 ] | [ s > 1 ]) */ \
    __ASM_EMIT("orps            %%xmm1, %%xmm0")                /* xmm0 = (s & [ s >= -1 ] & [ s <= 1 ]) | (((1 & (abs(s) <= +Inf)) | sign(s)) & ([ s < -1 ] | [ s > 1 ])) */

#define U4VEC(x)        x, x, x, x
    static uint32_t XLIM_SAT[] __lsp_aligned16 =
    {
        U4VEC(0xbf800000),      // -1
        U4VEC(0x3f800000),      // +1
        U4VEC(0x80000000),      // sign
        U4VEC(0x7fffffff),      // abs
        U4VEC(0x7f800000)       // +Inf
    };

#undef U4VEC

    void limit_saturate1(float *dst, size_t count)
    {
        ARCH_X86_ASM
        (
            __ASM_EMIT("sub         $4, %[count]")
            __ASM_EMIT("jb          2f")

            // 4x blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("movups      (%[dst]), %%xmm0")
            LIMIT_SAT_BODY
            __ASM_EMIT("movups      %%xmm0, (%[dst])")
            __ASM_EMIT("add         $0x10, %[dst]")
            __ASM_EMIT("sub         $4, %[count]")
            __ASM_EMIT("jae         1b")

            __ASM_EMIT("2:")
            __ASM_EMIT("add         $4, %[count]")
            __ASM_EMIT("jle         10f")

            // 1x - 3x block
            __ASM_EMIT("test        $1, %[count]")
            __ASM_EMIT("jz          4f")
            __ASM_EMIT("movss       (%[dst]), %%xmm0")
            __ASM_EMIT("add         $4, %[dst]")
            __ASM_EMIT("4:")
            __ASM_EMIT("test        $2, %[count]")
            __ASM_EMIT("jz          6f")
            __ASM_EMIT("movlhps     %%xmm0, %%xmm0")
            __ASM_EMIT("movlps      (%[dst]), %%xmm0")
            __ASM_EMIT("6:")

            LIMIT_SAT_BODY

            __ASM_EMIT("test        $2, %[count]")
            __ASM_EMIT("jz          8f")
            __ASM_EMIT("movlps      %%xmm0, (%[dst])")
            __ASM_EMIT("movhlps     %%xmm0, %%xmm0")
            __ASM_EMIT("8:")
            __ASM_EMIT("test        $1, %[count]")
            __ASM_EMIT("jz          10f")
            __ASM_EMIT("sub         $4, %[dst]")
            __ASM_EMIT("movss       %%xmm0, (%[dst])")

            __ASM_EMIT("10:")
            : [dst] "+r" (dst), [count] "+r" (count)
            : [XC] "o" (XLIM_SAT)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void limit_saturate2(float *dst, const float *src, size_t count)
    {
        ARCH_X86_ASM
        (
            __ASM_EMIT("sub         $4, %[count]")
            __ASM_EMIT("jb          2f")

            // 4x blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("movups      (%[src]), %%xmm0")
            LIMIT_SAT_BODY
            __ASM_EMIT("movups      %%xmm0, (%[dst])")
            __ASM_EMIT("add         $0x10, %[src]")
            __ASM_EMIT("add         $0x10, %[dst]")
            __ASM_EMIT("sub         $4, %[count]")
            __ASM_EMIT("jae         1b")

            __ASM_EMIT("2:")
            __ASM_EMIT("add         $4, %[count]")
            __ASM_EMIT("jle         10f")

            // 1x - 3x block
            __ASM_EMIT("test        $1, %[count]")
            __ASM_EMIT("jz          4f")
            __ASM_EMIT("movss       (%[src]), %%xmm0")
            __ASM_EMIT("add         $4, %[src]")
            __ASM_EMIT("add         $4, %[dst]")
            __ASM_EMIT("4:")
            __ASM_EMIT("test        $2, %[count]")
            __ASM_EMIT("jz          6f")
            __ASM_EMIT("movlhps     %%xmm0, %%xmm0")
            __ASM_EMIT("movlps      (%[src]), %%xmm0")
            __ASM_EMIT("6:")

            LIMIT_SAT_BODY

            __ASM_EMIT("test        $2, %[count]")
            __ASM_EMIT("jz          8f")
            __ASM_EMIT("movlps      %%xmm0, (%[dst])")
            __ASM_EMIT("movhlps     %%xmm0, %%xmm0")
            __ASM_EMIT("8:")
            __ASM_EMIT("test        $1, %[count]")
            __ASM_EMIT("jz          10f")
            __ASM_EMIT("sub         $4, %[dst]")
            __ASM_EMIT("movss       %%xmm0, (%[dst])")

            __ASM_EMIT("10:")
            : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
            : [XC] "o" (XLIM_SAT)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

}

#endif /* DSP_ARCH_X86_SSE2_FLOAT_H_ */
