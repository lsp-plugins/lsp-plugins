/*
 * float.h
 *
 *  Created on: 27 янв. 2020 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_SSE_FLOAT_H_
#define DSP_ARCH_X86_SSE_FLOAT_H_

#ifndef DSP_ARCH_X86_SSE_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_SSE_IMPL */

namespace sse
{
    #define LIMIT_BODY(DST, SRC) \
        __ASM_EMIT("xor                 %[off], %[off]") \
        __ASM_EMIT("movss               %[min], %%xmm6") \
        __ASM_EMIT("movss               %[max], %%xmm7") \
        __ASM_EMIT("shufps              $0x00, %%xmm6, %%xmm6") /* ymm6 = min */ \
        __ASM_EMIT("shufps              $0x00, %%xmm7, %%xmm7") /* ymm7 = max */ \
        /* x8 blocks */ \
        __ASM_EMIT("sub                 $8, %[count]") \
        __ASM_EMIT("jb                  2f") \
        __ASM_EMIT("1:") \
        __ASM_EMIT("movups              0x00(%[" SRC "], %[off]), %%xmm0")      /* xmm0 = s0 */  \
        __ASM_EMIT("movups              0x10(%[" SRC "], %[off]), %%xmm1") \
        __ASM_EMIT("movaps              %%xmm6, %%xmm2")                        /* xmm6 = min */ \
        __ASM_EMIT("movaps              %%xmm6, %%xmm3") \
        __ASM_EMIT("cmpps               $2, %%xmm0, %%xmm2")                    /* xmm2 = c = [ (s >= min) & !isnan(s) ] */ \
        __ASM_EMIT("cmpps               $2, %%xmm1, %%xmm3") \
        __ASM_EMIT("movaps              %%xmm2, %%xmm4")                        /* xmm4 = c */ \
        __ASM_EMIT("movaps              %%xmm3, %%xmm5") \
        __ASM_EMIT("andps               %%xmm2, %%xmm0")                        /* xmm0 = s & c */ \
        __ASM_EMIT("andps               %%xmm3, %%xmm1") \
        __ASM_EMIT("andnps              %%xmm6, %%xmm2")                        /* xmm2 = min & (!c) */ \
        __ASM_EMIT("andnps              %%xmm6, %%xmm3") \
        __ASM_EMIT("orps                %%xmm2, %%xmm0")                        /* xmm0 = (s & c) | min & (!c) */ \
        __ASM_EMIT("orps                %%xmm3, %%xmm1") \
        __ASM_EMIT("movaps              %%xmm7, %%xmm2")                        /* xmm6 = min */ \
        __ASM_EMIT("movaps              %%xmm7, %%xmm3") \
        __ASM_EMIT("cmpps               $5, %%xmm0, %%xmm2")                    /* xmm2 = c = [ (s <= max) & !isnan(s) ] */ \
        __ASM_EMIT("cmpps               $5, %%xmm1, %%xmm3") \
        __ASM_EMIT("movaps              %%xmm2, %%xmm4")                        /* xmm4 = c */ \
        __ASM_EMIT("movaps              %%xmm3, %%xmm5") \
        __ASM_EMIT("andps               %%xmm2, %%xmm0")                        /* xmm0 = s & c */ \
        __ASM_EMIT("andps               %%xmm3, %%xmm1") \
        __ASM_EMIT("andnps              %%xmm7, %%xmm2")                        /* xmm2 = min & (!c) */ \
        __ASM_EMIT("andnps              %%xmm7, %%xmm3") \
        __ASM_EMIT("orps                %%xmm2, %%xmm0")                        /* xmm0 = (s & c) | min & (!c) */ \
        __ASM_EMIT("orps                %%xmm3, %%xmm1") \
        __ASM_EMIT("movups              %%xmm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("movups              %%xmm1, 0x10(%[" DST "], %[off])") \
        __ASM_EMIT("add                 $0x20, %[off]") \
        __ASM_EMIT("sub                 $8, %[count]") \
        __ASM_EMIT("jae                 1b") \
        __ASM_EMIT("2:") \
        /* x4 block */ \
        __ASM_EMIT("add                 $4, %[count]") \
        __ASM_EMIT("jl                  4f") \
        __ASM_EMIT("movups              0x00(%[" SRC "], %[off]), %%xmm0")      /* xmm0 = s0 */  \
        __ASM_EMIT("movaps              %%xmm6, %%xmm2")                        /* xmm6 = min */ \
        __ASM_EMIT("cmpps               $2, %%xmm0, %%xmm2")                    /* xmm2 = c = [ (s >= min) & !isnan(s) ] */ \
        __ASM_EMIT("movaps              %%xmm2, %%xmm4")                        /* xmm4 = c */ \
        __ASM_EMIT("andps               %%xmm2, %%xmm0")                        /* xmm0 = s & c */ \
        __ASM_EMIT("andnps              %%xmm6, %%xmm2")                        /* xmm2 = min & (!c) */ \
        __ASM_EMIT("orps                %%xmm2, %%xmm0")                        /* xmm0 = (s & c) | min & (!c) */ \
        __ASM_EMIT("movaps              %%xmm7, %%xmm2")                        /* xmm6 = min */ \
        __ASM_EMIT("cmpps               $5, %%xmm0, %%xmm2")                    /* xmm2 = c = [ (s <= max) & !isnan(s) ] */ \
        __ASM_EMIT("movaps              %%xmm2, %%xmm4")                        /* xmm4 = c */ \
        __ASM_EMIT("andps               %%xmm2, %%xmm0")                        /* xmm0 = s & c */ \
        __ASM_EMIT("andnps              %%xmm7, %%xmm2")                        /* xmm2 = min & (!c) */ \
        __ASM_EMIT("orps                %%xmm2, %%xmm0")                        /* xmm0 = (s & c) | min & (!c) */ \
        __ASM_EMIT("movups              %%xmm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("sub                 $4, %[count]") \
        __ASM_EMIT("add                 $0x10, %[off]") \
        __ASM_EMIT("4:") \
        /* x1 blocks */ \
        __ASM_EMIT("add                 $3, %[count]") \
        __ASM_EMIT("jl                  6f") \
        __ASM_EMIT("5:") \
        __ASM_EMIT("movss               0x00(%[" SRC "], %[off]), %%xmm0")      /* xmm0 = s0 */  \
        __ASM_EMIT("movaps              %%xmm6, %%xmm2")                        /* xmm6 = min */ \
        __ASM_EMIT("cmpps               $2, %%xmm0, %%xmm2")                    /* xmm2 = c = [ (s >= min) & !isnan(s) ] */ \
        __ASM_EMIT("movaps              %%xmm2, %%xmm4")                        /* xmm4 = c */ \
        __ASM_EMIT("andps               %%xmm2, %%xmm0")                        /* xmm0 = s & c */ \
        __ASM_EMIT("andnps              %%xmm6, %%xmm2")                        /* xmm2 = min & (!c) */ \
        __ASM_EMIT("orps                %%xmm2, %%xmm0")                        /* xmm0 = (s & c) | min & (!c) */ \
        __ASM_EMIT("movaps              %%xmm7, %%xmm2")                        /* xmm6 = min */ \
        __ASM_EMIT("cmpps               $5, %%xmm0, %%xmm2")                    /* xmm2 = c = [ (s <= max) & !isnan(s) ] */ \
        __ASM_EMIT("movaps              %%xmm2, %%xmm4")                        /* xmm4 = c */ \
        __ASM_EMIT("andps               %%xmm2, %%xmm0")                        /* xmm0 = s & c */ \
        __ASM_EMIT("andnps              %%xmm7, %%xmm2")                        /* xmm2 = min & (!c) */ \
        __ASM_EMIT("orps                %%xmm2, %%xmm0")                        /* xmm0 = (s & c) | min & (!c) */ \
        __ASM_EMIT("movss               %%xmm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("add                 $0x04, %[off]") \
        __ASM_EMIT("dec                 %[count]") \
        __ASM_EMIT("jge                 5b") \
        __ASM_EMIT("6:")

    void limit1(float *dst, float min, float max, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM(
            LIMIT_BODY("dst", "dst")
            : [count] "+r" (count), [off] "=&r" (off)
            : [dst] "r" (dst),
              [min] "o" (min), [max] "o" (max)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void limit2(float *dst, const float *src, float min, float max, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM(
            LIMIT_BODY("dst", "src")
            : [count] "+r" (count), [off] "=&r" (off)
            : [dst] "r" (dst), [src] "r" (src),
              [min] "o" (min), [max] "o" (max)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    #undef LIMIT_BODY
}

#endif /* INCLUDE_DSP_ARCH_X86_SSE_FLOAT_H_ */
