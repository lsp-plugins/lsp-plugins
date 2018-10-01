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
#define MULTIPLE_SATURATION_BODY(ldst) \
    __ASM_EMIT("xor             %[off], %[off]") \
    __ASM_EMIT("sub             $4, %[count]") \
    __ASM_EMIT("jb              2f") \
    \
    __ASM_EMIT("   (%[dst]), %%xmm0")      /* xmm0 = src */ \
    __ASM_EMIT("movapd  %%xmm0, %%xmm1")        /* xmm1 = src */ \
    __ASM_EMIT("pcmpgtd %%xmm7, %%xmm1")        /* xmm1 = /src >= 0/ */ \
    \
    /* +NAN, +INF */ \
    __ASM_EMIT("movapd  %%xmm0, %%xmm3")        /* xmm3 = src */ \
    __ASM_EMIT("movapd  %%xmm0, %%xmm4")        /* xmm4 = src */ \
    __ASM_EMIT("pcmpgtd %%xmm6, %%xmm3")        /* xmm3 = /src > +inf/ */ \
    __ASM_EMIT("pcmpeqd %%xmm6, %%xmm4")        /* xmm4 = /src == +inf/ */ \
    __ASM_EMIT("movapd  %%xmm3, %%xmm2")        /* xmm2 = /src > +inf/ */ \
    __ASM_EMIT("por     %%xmm4, %%xmm2")        /* xmm2 = /src >= +inf/ */ \
    __ASM_EMIT("pand    %[SX_P_NAN], %%xmm3")   /* xmm3 = +nan * /src > +inf/ */ \
    __ASM_EMIT("pand    %[SX_P_INF], %%xmm4")   /* xmm4 = +inf * /src == +inf/ */ \
    __ASM_EMIT("pand    %%xmm1, %%xmm2")        /* xmm2 = /src >= +inf && src >= 0/ */ \
    __ASM_EMIT("por     %%xmm3, %%xmm4")        /* xmm4 = +nan * /src > +inf/ + +inf * /src == +inf/ */ \
    \
    /* Merge */  \
    __ASM_EMIT("pandn   %%xmm0, %%xmm2")        /* xmm2 = src * !/src >= +inf && src >= 0/ */ \
    __ASM_EMIT("por     %%xmm4, %%xmm2")        /* xmm2 = src * !/src >= +inf && src >= 0/ + <+nan> * /src > +inf/ + <+inf> * /src == +inf/ */ \
    \
    /* -NAN, -INF */  \
    __ASM_EMIT("movapd  %%xmm0, %%xmm3")        /* xmm3 = src */  \
    __ASM_EMIT("movapd  %%xmm0, %%xmm4")        /* xmm4 = src */ \
    __ASM_EMIT("pcmpgtd %%xmm5, %%xmm3")        /* xmm3 = /src > -inf/ */ \
    __ASM_EMIT("pcmpeqd %%xmm5, %%xmm4")        /* xmm4 = /src == -inf/ */ \
    __ASM_EMIT("movapd  %%xmm3, %%xmm0")        /* xmm0 = /src > -inf/ */ \
    __ASM_EMIT("por     %%xmm4, %%xmm0")        /* xmm0 = /src >= -inf/ */ \
    __ASM_EMIT("pand    %[SX_N_NAN], %%xmm3")   /* xmm3 = -nan * /src > -inf/ */ \
    __ASM_EMIT("pand    %[SX_N_INF], %%xmm4")   /* xmm4 = -inf * /src == -inf/ */ \
    __ASM_EMIT("pandn   %%xmm0, %%xmm1")        /* xmm1 = /src >= -inf && src < 0/ */ \
    __ASM_EMIT("por     %%xmm3, %%xmm4")        /* xmm4 = -nan * /src > -inf/ + -inf * /src == -inf/ */ \
    \
    /* Merge */ \
    __ASM_EMIT("pand    %%xmm1, %%xmm4") \
    __ASM_EMIT("pandn   %%xmm2, %%xmm1") \
    __ASM_EMIT("por     %%xmm4, %%xmm1") \
    \
    /* Store and repeat loop */ \
    __ASM_EMIT(ldst  "  %%xmm1, (%[dst])")

    void copy_saturated(float *dst, const float *src, size_t count)
    {
        size_t off;

        ARCH_X86_ASM
        (
            __ASM_EMIT("xor             %[off], %[off]")
            __ASM_EMIT("sub             $4, %[count]")
            __ASM_EMIT("jb              2f")

            // x4 block
            __ASM_EMIT("1:")
            // +inf, +nan
            __ASM_EMIT("movdqu          0x00(%[src], %[off]), %%xmm0")
            __ASM_EMIT("movdqa          0x00 + %[X_IARGS], %%xmm7")     // xmm7 = X_P_INF
            __ASM_EMIT("movdqa          %%xmm0, %%xmm1")                // xmm1 = s
            __ASM_EMIT("movdqa          %%xmm0, %%xmm2")                // xmm2 = s
            __ASM_EMIT("pcmpeqd         %%xmm7, %%xmm1")                // xmm1 = [s == X_P_INF]
            __ASM_EMIT("pcmpgtd         %%xmm7, %%xmm2")                // xmm2 = [s > X_P_INF]
            __ASM_EMIT("movdqa          %%xmm1, %%xmm3")                // xmm3 = [s == X_P_INF]
            __ASM_EMIT("por             %%xmm2, %%xmm3")                // xmm3 = [s >= X_P_INF]
            __ASM_EMIT("pand            0x00 + %[X_FARGS], %%xmm1")     // xmm1 = +INF & [s == X_P_INF]
            __ASM_EMIT("pand            0x10 + %[X_FARGS], %%xmm2")     // xmm2 = +NAN & [s > X_P_INF]
            __ASM_EMIT("pandn           %%xmm0, %%xmm3")                // xmm3 = s & [s < X_P_INF]
            __ASM_EMIT("por             %%xmm1, %%xmm3")                // xmm3 = (s & [s < X_P_INF]) | (+INF & [s == X_P_INF])
            __ASM_EMIT("por             %%xmm2, %%xmm3")                // xmm3 = (s & [s < X_P_INF]) | (+INF & [s == X_P_INF]) | (+NAN & [s > X_P_INF])
            __ASM_EMIT("movdqa          %%xmm3, %%xmm0")
            // -inf, -nan
            __ASM_EMIT("movdqa          0x10 + %[X_IARGS], %%xmm7")     // xmm7 = X_N_INF
            __ASM_EMIT("movdqa          %%xmm0, %%xmm1")                // xmm1 = s
            __ASM_EMIT("movdqa          %%xmm0, %%xmm2")                // xmm2 = s
            __ASM_EMIT("pcmpeqd         %%xmm7, %%xmm1")                // xmm1 = [s == X_N_INF]
            __ASM_EMIT("pcmpgtd         %%xmm7, %%xmm2")                // xmm2 = [s > X_N_INF]
            __ASM_EMIT("movdqa          %%xmm1, %%xmm3")                // xmm3 = [s == X_N_INF]
            __ASM_EMIT("por             %%xmm2, %%xmm3")                // xmm3 = [s >= X_N_INF]
            __ASM_EMIT("pand            0x20 + %[X_FARGS], %%xmm1")     // xmm1 = -INF & [s == X_N_INF]
            __ASM_EMIT("pandn           0x30 + %[X_FARGS], %%xmm3")     // xmm3 = -NAN & [s < X_N_INF]
            __ASM_EMIT("pand            %%xmm2, %%xmm0")                // xmm0 = s & [s > X_N_INF]
            __ASM_EMIT("por             %%xmm1, %%xmm0")                // xmm0 = (s & [s > X_N_INF]) | (-INF & [s == X_N_INF])
            __ASM_EMIT("por             %%xmm3, %%xmm0")                // xmm0 = (s & [s > X_N_INF]) | (-INF & [s == X_N_INF]) | (-NAN & [s < X_N_INF])
            __ASM_EMIT("movdqu          %%xmm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("add             $0x10, %[off]")
            __ASM_EMIT("sub             $4, %[count]")
            __ASM_EMIT("jae             1b")

            // x1 block
            __ASM_EMIT("2:")
            __ASM_EMIT("add             $3, %[count]")
            __ASM_EMIT("jl              4f")
            __ASM_EMIT("3:")
            __ASM_EMIT("movd            0x00(%[src], %[off]), %%xmm0")
            // +inf, +nan
            __ASM_EMIT("movdqa          0x00 + %[X_IARGS], %%xmm7")     // xmm7 = X_P_INF
            __ASM_EMIT("movdqa          %%xmm0, %%xmm1")                // xmm1 = s
            __ASM_EMIT("movdqa          %%xmm0, %%xmm2")                // xmm2 = s
            __ASM_EMIT("pcmpeqd         %%xmm7, %%xmm1")                // xmm1 = [s == X_P_INF]
            __ASM_EMIT("pcmpgtd         %%xmm7, %%xmm2")                // xmm2 = [s > X_P_INF]
            __ASM_EMIT("movdqa          %%xmm1, %%xmm3")                // xmm3 = [s == X_P_INF]
            __ASM_EMIT("por             %%xmm2, %%xmm3")                // xmm3 = [s >= X_P_INF]
            __ASM_EMIT("pand            0x00 + %[X_FARGS], %%xmm1")     // xmm1 = +INF & [s == X_P_INF]
            __ASM_EMIT("pand            0x10 + %[X_FARGS], %%xmm2")     // xmm2 = +NAN & [s > X_P_INF]
            __ASM_EMIT("pandn           %%xmm0, %%xmm3")                // xmm3 = s & [s < X_P_INF]
            __ASM_EMIT("por             %%xmm1, %%xmm3")                // xmm3 = (s & [s < X_P_INF]) | (+INF & [s == X_P_INF])
            __ASM_EMIT("por             %%xmm2, %%xmm3")                // xmm3 = (s & [s < X_P_INF]) | (+INF & [s == X_P_INF]) | (+NAN & [s > X_P_INF])
            __ASM_EMIT("movdqa          %%xmm3, %%xmm0")
            // -inf, -nan
            __ASM_EMIT("movdqa          0x10 + %[X_IARGS], %%xmm7")     // xmm7 = X_N_INF
            __ASM_EMIT("movdqa          %%xmm0, %%xmm1")                // xmm1 = s
            __ASM_EMIT("movdqa          %%xmm0, %%xmm2")                // xmm2 = s
            __ASM_EMIT("pcmpeqd         %%xmm7, %%xmm1")                // xmm1 = [s == X_N_INF]
            __ASM_EMIT("pcmpgtd         %%xmm7, %%xmm2")                // xmm2 = [s > X_N_INF]
            __ASM_EMIT("movdqa          %%xmm1, %%xmm3")                // xmm3 = [s == X_N_INF]
            __ASM_EMIT("por             %%xmm2, %%xmm3")                // xmm3 = [s >= X_N_INF]
            __ASM_EMIT("pand            0x20 + %[X_FARGS], %%xmm1")     // xmm1 = -INF & [s == X_N_INF]
            __ASM_EMIT("pandn           0x30 + %[X_FARGS], %%xmm3")     // xmm3 = -NAN & [s < X_N_INF]
            __ASM_EMIT("pand            %%xmm2, %%xmm0")                // xmm0 = s & [s > X_N_INF]
            __ASM_EMIT("por             %%xmm1, %%xmm0")                // xmm0 = (s & [s > X_N_INF]) | (-INF & [s == X_N_INF])
            __ASM_EMIT("por             %%xmm3, %%xmm0")                // xmm0 = (s & [s > X_N_INF]) | (-INF & [s == X_N_INF]) | (-NAN & [s < X_N_INF])
            __ASM_EMIT("movd            %%xmm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("add             $0x04, %[off]")
            __ASM_EMIT("dec             %[count]")
            __ASM_EMIT("jge             3b")

            : [dst] "+r" (dst), [src] "+r" (src),
              [count] "+r" (count), [off] "=&r" (off)
            : [X_IARGS] "o" (SAT_IARGS),
              [X_FARGS] "o" (SAT_FARGS)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void saturate(float *dst, size_t count)
    {
        #define MULTIPLE_SATURATION_BODY(ldst) \
            __ASM_EMIT(ldst "   (%[dst]), %%xmm0")      /* xmm0 = src */ \
            __ASM_EMIT("movapd  %%xmm0, %%xmm1")        /* xmm1 = src */ \
            __ASM_EMIT("pcmpgtd %%xmm7, %%xmm1")        /* xmm1 = /src >= 0/ */ \
            \
            /* +NAN, +INF */ \
            __ASM_EMIT("movapd  %%xmm0, %%xmm3")        /* xmm3 = src */ \
            __ASM_EMIT("movapd  %%xmm0, %%xmm4")        /* xmm4 = src */ \
            __ASM_EMIT("pcmpgtd %%xmm6, %%xmm3")        /* xmm3 = /src > +inf/ */ \
            __ASM_EMIT("pcmpeqd %%xmm6, %%xmm4")        /* xmm4 = /src == +inf/ */ \
            __ASM_EMIT("movapd  %%xmm3, %%xmm2")        /* xmm2 = /src > +inf/ */ \
            __ASM_EMIT("por     %%xmm4, %%xmm2")        /* xmm2 = /src >= +inf/ */ \
            __ASM_EMIT("pand    %[SX_P_NAN], %%xmm3")   /* xmm3 = +nan * /src > +inf/ */ \
            __ASM_EMIT("pand    %[SX_P_INF], %%xmm4")   /* xmm4 = +inf * /src == +inf/ */ \
            __ASM_EMIT("pand    %%xmm1, %%xmm2")        /* xmm2 = /src >= +inf && src >= 0/ */ \
            __ASM_EMIT("por     %%xmm3, %%xmm4")        /* xmm4 = +nan * /src > +inf/ + +inf * /src == +inf/ */ \
            \
            /* Merge */  \
            __ASM_EMIT("pandn   %%xmm0, %%xmm2")        /* xmm2 = src * !/src >= +inf && src >= 0/ */ \
            __ASM_EMIT("por     %%xmm4, %%xmm2")        /* xmm2 = src * !/src >= +inf && src >= 0/ + <+nan> * /src > +inf/ + <+inf> * /src == +inf/ */ \
            \
            /* -NAN, -INF */  \
            __ASM_EMIT("movapd  %%xmm0, %%xmm3")        /* xmm3 = src */  \
            __ASM_EMIT("movapd  %%xmm0, %%xmm4")        /* xmm4 = src */ \
            __ASM_EMIT("pcmpgtd %%xmm5, %%xmm3")        /* xmm3 = /src > -inf/ */ \
            __ASM_EMIT("pcmpeqd %%xmm5, %%xmm4")        /* xmm4 = /src == -inf/ */ \
            __ASM_EMIT("movapd  %%xmm3, %%xmm0")        /* xmm0 = /src > -inf/ */ \
            __ASM_EMIT("por     %%xmm4, %%xmm0")        /* xmm0 = /src >= -inf/ */ \
            __ASM_EMIT("pand    %[SX_N_NAN], %%xmm3")   /* xmm3 = -nan * /src > -inf/ */ \
            __ASM_EMIT("pand    %[SX_N_INF], %%xmm4")   /* xmm4 = -inf * /src == -inf/ */ \
            __ASM_EMIT("pandn   %%xmm0, %%xmm1")        /* xmm1 = /src >= -inf && src < 0/ */ \
            __ASM_EMIT("por     %%xmm3, %%xmm4")        /* xmm4 = -nan * /src > -inf/ + -inf * /src == -inf/ */ \
            \
            /* Merge */ \
            __ASM_EMIT("pand    %%xmm1, %%xmm4") \
            __ASM_EMIT("pandn   %%xmm2, %%xmm1") \
            __ASM_EMIT("por     %%xmm4, %%xmm1") \
            \
            /* Store and repeat loop */ \
            __ASM_EMIT(ldst  "  %%xmm1, (%[dst])")

        if (count == 0)
            return;

        ARCH_X86_ASM
        (
            /* Prepare constants */
            __ASM_EMIT("movapd %[X_ZERO_M1], %%xmm7")   /* xmm7 = 0 - 1 */
            __ASM_EMIT("movapd %[X_P_INF], %%xmm6")     /* xmm6 = +inf - 1 */
            __ASM_EMIT("movapd %[X_N_INF], %%xmm5")     /* xmm5 = -inf - 1 */

            /* Test for source alignmens */
            __ASM_EMIT("1:")
            __ASM_EMIT("test    $0xf, %[dst]")
            __ASM_EMIT("jz      2f")
            MULTIPLE_SATURATION_BODY("movss")
            __ASM_EMIT("add     $4, %[dst]")
            __ASM_EMIT("dec     %[count]")
            __ASM_EMIT("jnz 1b")
            __ASM_EMIT("2:")

            : [dst] "+r" (dst), [count] "+r" (count)
            :
                [X_ZERO_M1] "m" (X_ZERO_M1),
                [X_P_INF] "m" (X_P_INF),
                [X_N_INF] "m" (X_N_INF),
                [SX_P_INF] "m" (SX_P_INF),
                [SX_N_INF] "m" (SX_N_INF),
                [SX_P_NAN] "m" (SX_P_NAN),
                [SX_N_NAN] "m" (SX_N_NAN)
            :   "memory", "cc",
                "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );

        size_t regs     = count / SSE_MULTIPLE;
        count          %= SSE_MULTIPLE;

        if (regs > 0)
        {
            ARCH_X86_ASM
            (
                /* Test for source alignmens */
                __ASM_EMIT("1:")
                MULTIPLE_SATURATION_BODY("movapd")
                __ASM_EMIT("add     $0x10, %[dst]")
                __ASM_EMIT("dec     %[count]")
                __ASM_EMIT("jnz 1b")

                : [dst] "+r" (dst), [count] "+r" (regs)
                :
                    [X_ZERO_M1] "m" (X_ZERO_M1),
                    [X_P_INF] "m" (X_P_INF),
                    [X_N_INF] "m" (X_N_INF),
                    [SX_P_INF] "m" (SX_P_INF),
                    [SX_N_INF] "m" (SX_N_INF),
                    [SX_P_NAN] "m" (SX_P_NAN),
                    [SX_N_NAN] "m" (SX_N_NAN)
                :   "memory", "cc",
                    "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                    "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        ARCH_X86_ASM
        (
            /* Test for source alignmens */
            __ASM_EMIT("test    %[count], %[count]")
            __ASM_EMIT("jz      2f")
            __ASM_EMIT("1:")
            MULTIPLE_SATURATION_BODY("movss")
            __ASM_EMIT("add     $4, %[dst]")
            __ASM_EMIT("dec     %[count]")
            __ASM_EMIT("jnz 1b")
            __ASM_EMIT("2:")

            : [dst] "+r" (dst), [count] "+r" (count)
            :
                [X_ZERO_M1] "m" (X_ZERO_M1),
                [X_P_INF] "m" (X_P_INF),
                [X_N_INF] "m" (X_N_INF),
                [SX_P_INF] "m" (SX_P_INF),
                [SX_N_INF] "m" (SX_N_INF),
                [SX_P_NAN] "m" (SX_P_NAN),
                [SX_N_NAN] "m" (SX_N_NAN)
            :   "memory", "cc",
                "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );

        #undef MULTIPLE_SATURATION_BODY
    }
}

#endif /* DSP_ARCH_X86_SSE_FLOAT_H_ */
