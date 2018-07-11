/*
 * float.h
 *
 *  Created on: 06 авг. 2016 г.
 *      Author: sadko
 */

#ifndef CORE_X86_DSP_FLOAT_H_
#define CORE_X86_DSP_FLOAT_H_

namespace lsp
{
    namespace sse
    {
        static void copy_saturated(float *dst, const float *src, size_t count)
        {
            #define MULTIPLE_SATURATION_BODY(ld, st) \
                __ASM_EMIT(ld "     (%[src]), %%xmm0")      /* xmm0 = src */ \
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
                __ASM_EMIT(st    "  %%xmm1, (%[dst])")

            #define MULTIPLE_SATURATION(ld, st) \
                __asm__ __volatile__ \
                ( \
                    /* Perorm loop */ \
                    __ASM_EMIT("1:") \
                    MULTIPLE_SATURATION_BODY(ld, st) \
                    __ASM_EMIT("add     $0x10, %[src]") \
                    __ASM_EMIT("add     $0x10, %[dst]") \
                    __ASM_EMIT("dec     %[count]") \
                    __ASM_EMIT("jnz 1b") \
                    \
                    : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (regs) \
                    : \
                        [X_ZERO_M1] "m" (X_ZERO_M1), \
                        [X_P_INF] "m" (X_P_INF), \
                        [X_N_INF] "m" (X_N_INF), \
                        [SX_P_INF] "m" (SX_P_INF), \
                        [SX_N_INF] "m" (SX_N_INF), \
                        [SX_P_NAN] "m" (SX_P_NAN), \
                        [SX_N_NAN] "m" (SX_N_NAN) \
                    : "memory", "cc",   \
                      "%xmm0", "%xmm1", "%xmm2", "%xmm3", \
                      "%xmm4", "%xmm5", "%xmm6", "%xmm7" \
                );

            if (count == 0)
                return;

            __asm__ __volatile__
            (
                /* Prepare constants */
                __ASM_EMIT("movapd %[X_ZERO_M1], %%xmm7")   /* xmm7 = 0 - 1 */
                __ASM_EMIT("movapd %[X_P_INF], %%xmm6")     /* xmm6 = +inf - 1 */
                __ASM_EMIT("movapd %[X_N_INF], %%xmm5")     /* xmm5 = -inf - 1 */

                /* Test for source alignmens */
                __ASM_EMIT("1:")
                __ASM_EMIT("test    $0xf, %[dst]")
                __ASM_EMIT("jz      2f")
                MULTIPLE_SATURATION_BODY("movss", "movss")
                __ASM_EMIT("add     $4, %[src]")
                __ASM_EMIT("add     $4, %[dst]")
                __ASM_EMIT("dec     %[count]")
                __ASM_EMIT("jnz 1b")
                __ASM_EMIT("2:")

                : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
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
                if (sse_aligned(src))
                {
                    MULTIPLE_SATURATION("movapd", "movapd")
                }
                else
                {
                    MULTIPLE_SATURATION("movupd", "movapd")
                }
            }

            __asm__ __volatile__
            (
                /* Test for source alignmens */
                __ASM_EMIT("test    %[count], %[count]")
                __ASM_EMIT("jz      2f")
                __ASM_EMIT("1:")
                MULTIPLE_SATURATION_BODY("movss", "movss")
                __ASM_EMIT("add     $4, %[src]")
                __ASM_EMIT("add     $4, %[dst]")
                __ASM_EMIT("dec     %[count]")
                __ASM_EMIT("jnz 1b")
                __ASM_EMIT("2:")

                : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
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

            #undef MULTIPLE_SATURATION
            #undef MULTIPLE_SATURATION_BODY
        }

        static void saturate(float *dst, size_t count)
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

            __asm__ __volatile__
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
                __asm__ __volatile__
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

            __asm__ __volatile__
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
}

#endif /* CORE_X86_DSP_FLOAT_H_ */
