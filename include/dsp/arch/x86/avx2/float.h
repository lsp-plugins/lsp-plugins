/*
 * float.h
 *
 *  Created on: 26 янв. 2020 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_AVX2_FLOAT_H_
#define DSP_ARCH_X86_AVX2_FLOAT_H_

#ifndef DSP_ARCH_X86_AVX2_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_AVX2_IMPL */

namespace avx2
{
    #define LIMIT_SAT_BODY(DST, SRC) \
        __ASM_EMIT("xor                 %[off], %[off]") \
        /* x16 blocks */ \
        __ASM_EMIT("sub                 $16, %[count]") \
        __ASM_EMIT("jb                  2f") \
        __ASM_EMIT("1:") \
        __ASM_EMIT("vmovups             0x00(%[" SRC "], %[off]), %%ymm0")      /* ymm0 = s */ \
        __ASM_EMIT("vmovups             0x20(%[" SRC "], %[off]), %%ymm1") \
        __ASM_EMIT("vmovaps             0x00 + %[XC], %%ymm2")                  /* ymm2 = +1 */ \
        __ASM_EMIT("vandps              0x20 + %[XC], %%ymm0, %%ymm6")          /* ymm6 = abs(s) */ \
        __ASM_EMIT("vmovaps             %%ymm2, %%ymm3") \
        __ASM_EMIT("vandps              0x20 + %[XC], %%ymm1, %%ymm7") \
        __ASM_EMIT("vcmpps              $2, %%ymm2, %%ymm6, %%ymm2")            /* ymm2 = c = [ (abs(s) <= +1) & !isnan(s) ] */ \
        __ASM_EMIT("vcmpps              $2, %%ymm3, %%ymm7, %%ymm3") \
        __ASM_EMIT("vandps              0x40 + %[XC], %%ymm0, %%ymm4")          /* ymm4 = sign(s) */ \
        __ASM_EMIT("vandps              0x40 + %[XC], %%ymm1, %%ymm5") \
        __ASM_EMIT("vpcmpgtd            0x60 + %[XC], %%ymm6, %%ymm6")          /* ymm6 = [ abs(s) > +Inf ] */ \
        __ASM_EMIT("vpcmpgtd            0x60 + %[XC], %%ymm7, %%ymm7") \
        __ASM_EMIT("vorps               0x00 + %[XC], %%ymm4, %%ymm4")          /* ymm4 = +1 * sign(s) */ \
        __ASM_EMIT("vorps               0x00 + %[XC], %%ymm5, %%ymm5") \
        __ASM_EMIT("vandnps             %%ymm4, %%ymm6, %%ymm4")                /* ymm4 = r = +1 * sign(s) & [ abs(s) <= +Inf ] */ \
        __ASM_EMIT("vandnps             %%ymm5, %%ymm7, %%ymm5") \
        __ASM_EMIT("vblendvps           %%ymm2, %%ymm0, %%ymm4, %%ymm0")        /* ymm0 = (s & c) | (r & !c) */ \
        __ASM_EMIT("vblendvps           %%ymm3, %%ymm1, %%ymm5, %%ymm1") \
        __ASM_EMIT("vmovups             %%ymm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups             %%ymm1, 0x20(%[" DST "], %[off])") \
        __ASM_EMIT("add                 $0x40, %[off]") \
        __ASM_EMIT("sub                 $16, %[count]") \
        __ASM_EMIT("jae                 1b") \
        __ASM_EMIT("2:") \
        /* x8 blocks */ \
        __ASM_EMIT("add                 $8, %[count]") \
        __ASM_EMIT("jl                  4f") \
        __ASM_EMIT("vmovups             0x00(%[" SRC "], %[off]), %%xmm0")      /* xmm0 = s */ \
        __ASM_EMIT("vmovups             0x10(%[" SRC "], %[off]), %%xmm1") \
        __ASM_EMIT("vmovaps             0x00 + %[XC], %%xmm2")                  /* xmm2 = +1 */ \
        __ASM_EMIT("vandps              0x20 + %[XC], %%xmm0, %%xmm6")          /* xmm6 = abs(s) */ \
        __ASM_EMIT("vmovaps             %%xmm2, %%xmm3") \
        __ASM_EMIT("vandps              0x20 + %[XC], %%xmm1, %%xmm7") \
        __ASM_EMIT("vcmpps              $2, %%xmm2, %%xmm6, %%xmm2")            /* xmm2 = c = [ (abs(s) <= +1) & !isnan(s) ] */ \
        __ASM_EMIT("vcmpps              $2, %%xmm3, %%xmm7, %%xmm3") \
        __ASM_EMIT("vandps              0x40 + %[XC], %%xmm0, %%xmm4")          /* xmm4 = sign(s) */ \
        __ASM_EMIT("vandps              0x40 + %[XC], %%xmm1, %%xmm5") \
        __ASM_EMIT("vpcmpgtd            0x60 + %[XC], %%xmm6, %%xmm6")          /* xmm6 = [ abs(s) > +Inf ] */ \
        __ASM_EMIT("vpcmpgtd            0x60 + %[XC], %%xmm7, %%xmm7") \
        __ASM_EMIT("vorps               0x00 + %[XC], %%xmm4, %%xmm4")          /* xmm4 = +1 * sign(s) */ \
        __ASM_EMIT("vorps               0x00 + %[XC], %%xmm5, %%xmm5") \
        __ASM_EMIT("vandnps             %%xmm4, %%xmm6, %%xmm4")                /* xmm4 = r = +1 * sign(s) & [ abs(s) <= +Inf ] */ \
        __ASM_EMIT("vandnps             %%xmm5, %%xmm7, %%xmm5") \
        __ASM_EMIT("vblendvps           %%xmm2, %%xmm0, %%xmm4, %%xmm0")        /* xmm0 = (s & c) | (r & !c) */ \
        __ASM_EMIT("vblendvps           %%xmm3, %%xmm1, %%xmm5, %%xmm1") \
        __ASM_EMIT("vmovups             %%xmm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups             %%xmm1, 0x10(%[" DST "], %[off])") \
        __ASM_EMIT("sub                 $8, %[count]") \
        __ASM_EMIT("add                 $0x20, %[off]") \
        __ASM_EMIT("4:") \
        /* x4 block */ \
        __ASM_EMIT("add                 $4, %[count]") \
        __ASM_EMIT("jl                  6f") \
        __ASM_EMIT("vmovups             0x00(%[" SRC "], %[off]), %%xmm0")      /* xmm0 = s */ \
        __ASM_EMIT("vmovaps             0x00 + %[XC], %%xmm2")                  /* xmm2 = +1 */ \
        __ASM_EMIT("vandps              0x20 + %[XC], %%xmm0, %%xmm6")          /* xmm6 = abs(s) */ \
        __ASM_EMIT("vcmpps              $2, %%xmm2, %%xmm6, %%xmm2")            /* xmm2 = c = [ (abs(s) <= +1) & !isnan(s) ] */ \
        __ASM_EMIT("vandps              0x40 + %[XC], %%xmm0, %%xmm4")          /* xmm4 = sign(s) */ \
        __ASM_EMIT("vpcmpgtd            0x60 + %[XC], %%xmm6, %%xmm6")          /* xmm6 = [ abs(s) > +Inf ] */ \
        __ASM_EMIT("vorps               0x00 + %[XC], %%xmm4, %%xmm4")          /* xmm4 = +1 * sign(s) */ \
        __ASM_EMIT("vandnps             %%xmm4, %%xmm6, %%xmm4")                /* xmm4 = r = +1 * sign(s) & [ abs(s) <= +Inf ] */ \
        __ASM_EMIT("vblendvps           %%xmm2, %%xmm0, %%xmm4, %%xmm0")        /* xmm0 = (s & c) | (r & !c) */ \
        __ASM_EMIT("vmovups             %%xmm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("sub                 $4, %[count]") \
        __ASM_EMIT("add                 $0x10, %[off]") \
        __ASM_EMIT("6:") \
        /* x1 block */ \
        __ASM_EMIT("add                 $3, %[count]") \
        __ASM_EMIT("jl                  8f") \
        __ASM_EMIT("7:") \
        __ASM_EMIT("vmovss              0x00(%[" SRC "], %[off]), %%xmm0")      /* xmm0 = s */ \
        __ASM_EMIT("vmovaps             0x00 + %[XC], %%xmm2")                  /* xmm2 = +1 */ \
        __ASM_EMIT("vandps              0x20 + %[XC], %%xmm0, %%xmm6")          /* xmm6 = abs(s) */ \
        __ASM_EMIT("vcmpps              $2, %%xmm2, %%xmm6, %%xmm2")            /* xmm2 = c = [ (abs(s) <= +1) & !isnan(s) ] */ \
        __ASM_EMIT("vandps              0x40 + %[XC], %%xmm0, %%xmm4")          /* xmm4 = sign(s) */ \
        __ASM_EMIT("vpcmpgtd            0x60 + %[XC], %%xmm6, %%xmm6")          /* xmm6 = [ abs(s) > +Inf ] */ \
        __ASM_EMIT("vorps               0x00 + %[XC], %%xmm4, %%xmm4")          /* xmm4 = +1 * sign(s) */ \
        __ASM_EMIT("vandnps             %%xmm4, %%xmm6, %%xmm4")                /* xmm4 = r = +1 * sign(s) & [ abs(s) <= +Inf ] */ \
        __ASM_EMIT("vblendvps           %%xmm2, %%xmm0, %%xmm4, %%xmm0")        /* xmm0 = (s & c) | (r & !c) */ \
        __ASM_EMIT("vmovss              %%xmm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("add                 $0x04, %[off]") \
        __ASM_EMIT("dec                 %[count]") \
        __ASM_EMIT("jge                 7b") \
        __ASM_EMIT("8:")

    #define U8VEC(x)        x, x, x, x, x, x, x, x
    IF_ARCH_X86(
        static uint32_t XLIM_SAT[] __lsp_aligned32 =
        {
            U8VEC(0x3f800000),      // +1
            U8VEC(0x7fffffff),      // abs
            U8VEC(0x80000000),      // sign
            U8VEC(0x7f800000)       // +Inf
        };
    )
    #undef U8VEC

    void limit_saturate1(float *dst, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM(
            LIMIT_SAT_BODY("dst", "dst")
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r" (dst),
              [XC] "o" (XLIM_SAT)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void limit_saturate2(float *dst, const float *src, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM(
            LIMIT_SAT_BODY("dst", "src")
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r" (dst), [src] "r" (src),
              [XC] "o" (XLIM_SAT)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

#undef LIMIT_SAT_BODY
}



#endif /* DSP_ARCH_X86_AVX2_FLOAT_H_ */
