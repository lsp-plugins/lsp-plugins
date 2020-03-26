/*
 * float.h
 *
 *  Created on: 24 янв. 2020 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_AVX_FLOAT_H_
#define DSP_ARCH_X86_AVX_FLOAT_H_

#ifndef DSP_ARCH_X86_AVX_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_AVX_IMPL */

namespace avx
{
    #define LIMIT_BODY(DST, SRC) \
        __ASM_EMIT("xor                 %[off], %[off]") \
        __ASM_EMIT("vbroadcastss        %[min], %%ymm6")                        /* ymm6 = min */ \
        __ASM_EMIT("vbroadcastss        %[max], %%ymm7")                        /* ymm7 = max */ \
        /* x16 blocks */ \
        __ASM_EMIT("sub                 $16, %[count]") \
        __ASM_EMIT("jb                  2f") \
        __ASM_EMIT("1:") \
        __ASM_EMIT("vmovups             0x00(%[" SRC "], %[off]), %%ymm0")      /* ymm0 = s0 */  \
        __ASM_EMIT("vmovups             0x20(%[" SRC "], %[off]), %%ymm1") \
        __ASM_EMIT("vcmpps              $2, %%ymm0, %%ymm6, %%ymm2")            /* ymm2 = c = [ (s >= min) & !isnan(s) ] */ \
        __ASM_EMIT("vcmpps              $2, %%ymm1, %%ymm6, %%ymm3") \
        __ASM_EMIT("vblendvps           %%ymm2, %%ymm0, %%ymm6, %%ymm0")        /* ymm0 = (s & c) | min & (!c) */ \
        __ASM_EMIT("vblendvps           %%ymm3, %%ymm1, %%ymm6, %%ymm1") \
        __ASM_EMIT("vcmpps              $5, %%ymm0, %%ymm7, %%ymm2")            /* ymm2 = c = [ (s <= max) & !isnan(s) ] */ \
        __ASM_EMIT("vcmpps              $5, %%ymm1, %%ymm7, %%ymm3") \
        __ASM_EMIT("vblendvps           %%ymm2, %%ymm0, %%ymm7, %%ymm0")        /* ymm0 = (s & c) | min & (!c) */ \
        __ASM_EMIT("vblendvps           %%ymm3, %%ymm1, %%ymm7, %%ymm1") \
        __ASM_EMIT("vmovups             %%ymm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups             %%ymm1, 0x20(%[" DST "], %[off])") \
        __ASM_EMIT("add                 $0x40, %[off]") \
        __ASM_EMIT("sub                 $16, %[count]") \
        __ASM_EMIT("jae                 1b") \
        __ASM_EMIT("2:") \
        /* x8 block */ \
        __ASM_EMIT("add                 $8, %[count]") \
        __ASM_EMIT("jl                  4f") \
        __ASM_EMIT("vmovups             0x00(%[" SRC "], %[off]), %%xmm0")      /* xmm0 = s0 */  \
        __ASM_EMIT("vmovups             0x10(%[" SRC "], %[off]), %%xmm1") \
        __ASM_EMIT("vcmpps              $2, %%xmm0, %%xmm6, %%xmm2")            /* xmm2 = c = [ (s >= min) & !isnan(s) ] */ \
        __ASM_EMIT("vcmpps              $2, %%xmm1, %%xmm6, %%xmm3") \
        __ASM_EMIT("vblendvps           %%xmm2, %%xmm0, %%xmm6, %%xmm0")        /* xmm0 = (s & c) | min & (!c) */ \
        __ASM_EMIT("vblendvps           %%xmm3, %%xmm1, %%xmm6, %%xmm1") \
        __ASM_EMIT("vcmpps              $5, %%xmm0, %%xmm7, %%xmm2")            /* xmm2 = c = [ (s <= max) & !isnan(s) ] */ \
        __ASM_EMIT("vcmpps              $5, %%xmm1, %%xmm7, %%xmm3") \
        __ASM_EMIT("vblendvps           %%xmm2, %%xmm0, %%xmm7, %%xmm0")        /* xmm0 = (s & c) | min & (!c) */ \
        __ASM_EMIT("vblendvps           %%xmm3, %%xmm1, %%xmm7, %%xmm1") \
        __ASM_EMIT("vmovups             %%xmm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups             %%xmm1, 0x10(%[" DST "], %[off])") \
        __ASM_EMIT("sub                 $8, %[count]") \
        __ASM_EMIT("add                 $0x20, %[off]") \
        __ASM_EMIT("4:") \
        /* x4 block */ \
        __ASM_EMIT("add                 $4, %[count]") \
        __ASM_EMIT("jl                  6f") \
        __ASM_EMIT("vmovups             0x00(%[" SRC "], %[off]), %%xmm0")      /* xmm0 = s0 */  \
        __ASM_EMIT("vcmpps              $2, %%xmm0, %%xmm6, %%xmm2")            /* xmm2 = c = [ (s >= min) & !isnan(s) ] */ \
        __ASM_EMIT("vblendvps           %%xmm2, %%xmm0, %%xmm6, %%xmm0")        /* xmm0 = (s & c) | min & (!c) */ \
        __ASM_EMIT("vcmpps              $5, %%xmm0, %%xmm7, %%xmm2")            /* xmm2 = c = [ (s <= max) & !isnan(s) ] */ \
        __ASM_EMIT("vblendvps           %%xmm2, %%xmm0, %%xmm7, %%xmm0")        /* xmm0 = (s & c) | min & (!c) */ \
        __ASM_EMIT("vmovups             %%xmm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("sub                 $4, %[count]") \
        __ASM_EMIT("add                 $0x10, %[off]") \
        __ASM_EMIT("6:") \
        /* x1 blocks */ \
        __ASM_EMIT("add                 $3, %[count]") \
        __ASM_EMIT("jl                  8f") \
        __ASM_EMIT("7:") \
        __ASM_EMIT("vmovss              0x00(%[" SRC "], %[off]), %%xmm0")      /* xmm0 = s0 */  \
        __ASM_EMIT("vcmpps              $2, %%xmm0, %%xmm6, %%xmm2")            /* xmm2 = c = [ (s >= min) & !isnan(s) ] */ \
        __ASM_EMIT("vblendvps           %%xmm2, %%xmm0, %%xmm6, %%xmm0")        /* xmm0 = (s & c) | min & (!c) */ \
        __ASM_EMIT("vcmpps              $5, %%xmm0, %%xmm7, %%xmm2")            /* xmm2 = c = [ (s <= max) & !isnan(s) ] */ \
        __ASM_EMIT("vblendvps           %%xmm2, %%xmm0, %%xmm7, %%xmm0")        /* xmm0 = (s & c) | min & (!c) */ \
        __ASM_EMIT("vmovss              %%xmm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("add                 $0x04, %[off]") \
        __ASM_EMIT("dec                 %[count]") \
        __ASM_EMIT("jge                 7b") \
        __ASM_EMIT("8:")

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
              "%xmm6", "%xmm7"
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
              "%xmm6", "%xmm7"
        );
    }

    #undef LIMIT_BODY

#define SANITIZE_BODY(DST, SRC) \
    __ASM_EMIT("xor             %[off], %[off]") \
    __ASM_EMIT("sub             $8, %[count]") \
    __ASM_EMIT("jb              2f") \
    /* 8x blocks */ \
    __ASM_EMIT("1:") \
    __ASM_EMIT("vmovdqu         0x00(%[" SRC "], %[off]), %%xmm0")      /* xmm0 = s */ \
    __ASM_EMIT("vmovdqu         0x10(%[" SRC "], %[off]), %%xmm4") \
    __ASM_EMIT("vpand           0x00 + %[CVAL], %%xmm0, %%xmm1")        /* xmm1 = abs(s) */ \
    __ASM_EMIT("vpand           0x00 + %[CVAL], %%xmm4, %%xmm5") \
    __ASM_EMIT("vpand           0x10 + %[CVAL], %%xmm0, %%xmm2")        /* xmm2 = sign(s) */ \
    __ASM_EMIT("vpand           0x10 + %[CVAL], %%xmm4, %%xmm6") \
    __ASM_EMIT("vpcmpgtd        0x20 + %[CVAL], %%xmm1, %%xmm3")        /* xmm3 = abs(s) > X_MAX  */ \
    __ASM_EMIT("vpcmpgtd        0x20 + %[CVAL], %%xmm5, %%xmm7") \
    __ASM_EMIT("vpcmpgtd        0x30 + %[CVAL], %%xmm1, %%xmm1")        /* xmm1 = abs(s) > X_MIN  */ \
    __ASM_EMIT("vpcmpgtd        0x30 + %[CVAL], %%xmm5, %%xmm5") \
    __ASM_EMIT("vpandn          %%xmm1, %%xmm3, %%xmm1")                /* xmm1 = (abs(s) > X_MIN) & (abs(s) <= X_MAX) */ \
    __ASM_EMIT("vpandn          %%xmm5, %%xmm7, %%xmm5") \
    __ASM_EMIT("vblendvps       %%xmm1, %%xmm0, %%xmm2, %%xmm0")        /* xmm0 = ((abs(s) > X_MIN) & (abs(s) <= X_MAX))) ? s : sign(s) */ \
    __ASM_EMIT("vblendvps       %%xmm5, %%xmm4, %%xmm6, %%xmm4") \
    __ASM_EMIT("vmovdqu         %%xmm0, 0x00(%[" DST "], %[off])")      /* xmm0 = s */ \
    __ASM_EMIT("vmovdqu         %%xmm4, 0x10(%[" DST "], %[off])") \
    __ASM_EMIT("add             $0x20, %[off]") \
    __ASM_EMIT("sub             $8, %[count]") \
    __ASM_EMIT("jae             1b") \
    /* 4x block */ \
    __ASM_EMIT("2:") \
    __ASM_EMIT("add             $4, %[count]") \
    __ASM_EMIT("jl              4f") \
    __ASM_EMIT("vmovdqu         0x00(%[" SRC "], %[off]), %%xmm0")      /* xmm0 = s */ \
    __ASM_EMIT("vpand           0x00 + %[CVAL], %%xmm0, %%xmm1")        /* xmm1 = abs(s) */ \
    __ASM_EMIT("vpand           0x10 + %[CVAL], %%xmm0, %%xmm2")        /* xmm2 = sign(s) */ \
    __ASM_EMIT("vpcmpgtd        0x20 + %[CVAL], %%xmm1, %%xmm3")        /* xmm3 = abs(s) > X_MAX  */ \
    __ASM_EMIT("vpcmpgtd        0x30 + %[CVAL], %%xmm1, %%xmm1")        /* xmm1 = abs(s) > X_MIN  */ \
    __ASM_EMIT("vpandn          %%xmm1, %%xmm3, %%xmm1")                /* xmm1 = (abs(s) > X_MIN) & (abs(s) <= X_MAX) */ \
    __ASM_EMIT("vblendvps       %%xmm1, %%xmm0, %%xmm2, %%xmm0")        /* xmm0 = ((abs(s) > X_MIN) & (abs(s) <= X_MAX))) ? s : sign(s) */ \
    __ASM_EMIT("vmovdqu         %%xmm0, 0x00(%[" DST "], %[off])")      /* xmm0 = s */ \
    __ASM_EMIT("add             $0x10, %[off]") \
    __ASM_EMIT("sub             $4, %[count]") \
    /* 1x blocks */ \
    __ASM_EMIT("4:") \
    __ASM_EMIT("add             $3, %[count]") \
    __ASM_EMIT("jl              6f") \
    __ASM_EMIT("5:") \
    __ASM_EMIT("vmovd           0x00(%[" SRC "], %[off]), %%xmm0")      /* xmm0 = s */ \
    __ASM_EMIT("vpand           0x00 + %[CVAL], %%xmm0, %%xmm1")        /* xmm1 = abs(s) */ \
    __ASM_EMIT("vpand           0x10 + %[CVAL], %%xmm0, %%xmm2")        /* xmm2 = sign(s) */ \
    __ASM_EMIT("vpcmpgtd        0x20 + %[CVAL], %%xmm1, %%xmm3")        /* xmm3 = abs(s) > X_MAX  */ \
    __ASM_EMIT("vpcmpgtd        0x30 + %[CVAL], %%xmm1, %%xmm1")        /* xmm1 = abs(s) > X_MIN  */ \
    __ASM_EMIT("vpandn          %%xmm1, %%xmm3, %%xmm1")                /* xmm1 = (abs(s) > X_MIN) & (abs(s) <= X_MAX) */ \
    __ASM_EMIT("vblendvps       %%xmm1, %%xmm0, %%xmm2, %%xmm0")        /* xmm0 = ((abs(s) > X_MIN) & (abs(s) <= X_MAX))) ? s : sign(s) */ \
    __ASM_EMIT("vmovd           %%xmm0, 0x00(%[" DST "], %[off])")      /* xmm0 = s */ \
    __ASM_EMIT("add             $0x04, %[off]") \
    __ASM_EMIT("dec             %[count]") \
    __ASM_EMIT("jge             5b") \
    /* end */ \
    __ASM_EMIT("6:")

    #define U4VEC(x)        x, x, x, x
    IF_ARCH_X86(
        static uint32_t SANITIZE_CVAL[] __lsp_aligned16 =
        {
            U4VEC(0x7fffffff),            // X_ABS
            U4VEC(0x80000000),            // X_SIGN
            U4VEC(0x7f7fffff),            // X_MAX
            U4VEC(0x007fffff)             // X_MIN
        };
    )
    #undef U4VEC

    void sanitize1(float *dst, size_t count)
    {
        IF_ARCH_X86(size_t off);

        ARCH_X86_ASM
        (
            SANITIZE_BODY("dst", "dst")
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r" (dst),
              [CVAL] "o" (SANITIZE_CVAL)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void sanitize2(float *dst, const float *src, size_t count)
    {
        IF_ARCH_X86(size_t off);

        ARCH_X86_ASM
        (
            SANITIZE_BODY("dst", "src")
            : [off] "=&r" (off), [count] "+r" (count)
            : [dst] "r" (dst), [src] "r" (src),
              [CVAL] "o" (SANITIZE_CVAL)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

#undef SANITIZE_BODY
}

#endif /* DSP_ARCH_X86_AVX_FLOAT_H_ */
