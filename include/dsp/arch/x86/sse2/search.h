/*
 * search.h
 *
 *  Created on: 9 окт. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_SSE2_SEARCH_H_
#define DSP_ARCH_X86_SSE2_SEARCH_H_

#ifndef DSP_ARCH_X86_SSE2_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_SSE2_IMPL */

namespace sse2
{
    static uint32_t indexes[] __lsp_aligned16 =
    {
        0, 1, 2, 3, // indexes
        4, 4, 4, 4, // step
        1, 1, 1, 1
    };

    #define CMPLTPS     "$1"
    #define CMPGTPS     "$6"

    #define SEARCH_CORE(op) \
        __ASM_EMIT("pxor            %%xmm0, %%xmm0") \
        __ASM_EMIT("test            %[count], %[count]") \
        __ASM_EMIT("jz              4f") \
        __ASM_EMIT("movss           0x00(%[src]), %%xmm2")      /* x2   = s = s0 */ \
        __ASM_EMIT("movdqa          0x00 + %[IDXS], %%xmm1")    /* x1   = I[k] = 0 1 2 3 */ \
        __ASM_EMIT("shufps          $0x00, %%xmm2, %%xmm2")     /* x2   = s[k] = s0 s0 s0 s0 */ \
        __ASM_EMIT("sub             $0x04, %[count]") \
        __ASM_EMIT("jb              2f") \
        __ASM_EMIT("movdqa          0x10 + %[IDXS], %%xmm7")    /* x7   = D = 4 4 4 4 */ \
        \
        /* x4 blocks */ \
        __ASM_EMIT("1:") \
        __ASM_EMIT("movaps          %%xmm2, %%xmm4")            /* x4   = s[k] */ \
        __ASM_EMIT("movups          0x00(%[src]), %%xmm3")      /* x3   = S[k] */ \
        __ASM_EMIT("cmpps           " op ", %%xmm3, %%xmm4")    /* x4   = s[k] <=> S[k] */ \
        __ASM_EMIT("pand            %%xmm4, %%xmm0")            /* x0   = i[k] & (s[k] <=> S[k]) */ \
        __ASM_EMIT("movdqa          %%xmm4, %%xmm5")            /* x5   = s[k] <=> S[k] */ \
        __ASM_EMIT("andps           %%xmm4, %%xmm2")            /* x2   = s[k] & (s[k] <=> S[k]) */ \
        __ASM_EMIT("pandn           %%xmm1, %%xmm5")            /* x5   = I[k] & !(s[k] <=> S[k]) */ \
        __ASM_EMIT("andnps          %%xmm3, %%xmm4")            /* x4   = S[k] & !(s[k] <=> S[k]) */ \
        __ASM_EMIT("por             %%xmm5, %%xmm0")            /* x0   = (i[k] & (s[k] <=> S[k])) | (I[k] & !(s[k] <=> S[k])) */ \
        __ASM_EMIT("orps            %%xmm4, %%xmm2")            /* x2   = (s[k] & (s[k] <=> S[k])) | (S[k] & !(s[k] <=> S[k])) */ \
        __ASM_EMIT("paddd           %%xmm7, %%xmm1")            /* x1   = I[k] = i[k]+D */ \
        __ASM_EMIT("add             $0x10, %[src]") \
        __ASM_EMIT("sub             $4, %[count]") \
        __ASM_EMIT("jae             1b") \
        /* Post-process, step 1 */ \
        __ASM_EMIT("movdqa          %%xmm1, %%xmm6") \
        __ASM_EMIT("movaps          %%xmm2, %%xmm4") \
        __ASM_EMIT("movhlps         %%xmm0, %%xmm1") \
        __ASM_EMIT("movhlps         %%xmm2, %%xmm3") \
        __ASM_EMIT("cmpps           " op ", %%xmm3, %%xmm4") \
        __ASM_EMIT("pand            %%xmm4, %%xmm0") \
        __ASM_EMIT("movdqa          %%xmm4, %%xmm5") \
        __ASM_EMIT("andps           %%xmm4, %%xmm2") \
        __ASM_EMIT("pandn           %%xmm1, %%xmm5") \
        __ASM_EMIT("andnps          %%xmm3, %%xmm4") \
        __ASM_EMIT("por             %%xmm5, %%xmm0") \
        __ASM_EMIT("orps            %%xmm4, %%xmm2") \
        /* Post-process, step 2 */ \
        __ASM_EMIT("movaps          %%xmm2, %%xmm4") \
        __ASM_EMIT("unpcklps        %%xmm2, %%xmm2") \
        __ASM_EMIT("unpcklps        %%xmm0, %%xmm0") \
        __ASM_EMIT("movhlps         %%xmm2, %%xmm3") \
        __ASM_EMIT("movhlps         %%xmm0, %%xmm1") \
        __ASM_EMIT("cmpps           " op ", %%xmm3, %%xmm4") \
        __ASM_EMIT("pand            %%xmm4, %%xmm0") \
        __ASM_EMIT("movdqa          %%xmm4, %%xmm5") \
        __ASM_EMIT("andps           %%xmm4, %%xmm2") \
        __ASM_EMIT("pandn           %%xmm1, %%xmm5") \
        __ASM_EMIT("andnps          %%xmm3, %%xmm4") \
        __ASM_EMIT("por             %%xmm5, %%xmm0") \
        __ASM_EMIT("orps            %%xmm4, %%xmm2") \
        __ASM_EMIT("movdqa          %%xmm6, %%xmm1") \
        /* x1 blocks */ \
        __ASM_EMIT("2:") \
        __ASM_EMIT("add             $3, %[count]") \
        __ASM_EMIT("jl              4f") \
        __ASM_EMIT("movdqa          0x20 + %[IDXS], %%xmm7")    /* x7   = D = 1 1 1 1 */ \
        __ASM_EMIT("3:") \
        __ASM_EMIT("movaps          %%xmm2, %%xmm4") \
        __ASM_EMIT("movss           0x00(%[src]), %%xmm3") \
        __ASM_EMIT("cmpss           " op ", %%xmm3, %%xmm4") \
        __ASM_EMIT("pand            %%xmm4, %%xmm0") \
        __ASM_EMIT("movdqa          %%xmm4, %%xmm5") \
        __ASM_EMIT("andps           %%xmm4, %%xmm2") \
        __ASM_EMIT("pandn           %%xmm1, %%xmm5") \
        __ASM_EMIT("andnps          %%xmm3, %%xmm4") \
        __ASM_EMIT("por             %%xmm5, %%xmm0") \
        __ASM_EMIT("orps            %%xmm4, %%xmm2") \
        __ASM_EMIT("paddd           %%xmm7, %%xmm1") \
        __ASM_EMIT("add             $0x04, %[src]") \
        __ASM_EMIT("dec             %[count]") \
        __ASM_EMIT("jge             3b") \
        /* End */ \
        __ASM_EMIT("4:") \
        __ASM_EMIT("movd            %%xmm0, %[index]")


    size_t min_index(const float *src, size_t count)
    {
        uint32_t index;

        ARCH_X86_ASM(
            SEARCH_CORE(CMPLTPS)
            : [src] "+r" (src), [count] "+r" (count),
              [index] "=&r" (index)
            : [IDXS] "o" (indexes)
            : "cc",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );

        return index;
    }

    size_t max_index(const float *src, size_t count)
    {
        uint32_t index;

        ARCH_X86_ASM(
            SEARCH_CORE(CMPGTPS)
            : [src] "+r" (src), [count] "+r" (count),
              [index] "=&r" (index)
            : [IDXS] "o" (indexes)
            : "cc",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );

        return index;
    }

    #undef SEARCH_CORE

    #define ABS_SEARCH_CORE(op) \
        __ASM_EMIT("pxor            %%xmm0, %%xmm0") \
        __ASM_EMIT("test            %[count], %[count]") \
        __ASM_EMIT("jz              4f") \
        __ASM_EMIT("movss           0x00(%[src]), %%xmm2")      /* x2   = s = s0 */ \
        __ASM_EMIT("movdqa          0x00 + %[IDXS], %%xmm1")    /* x1   = I[k] = 0 1 2 3 */ \
        __ASM_EMIT("movaps          %[X_SIGN], %%xmm6")         /* x6   = SIGN */ \
        __ASM_EMIT("shufps          $0x00, %%xmm2, %%xmm2")     /* x2   = s[k] = s0 s0 s0 s0 */ \
        __ASM_EMIT("sub             $0x04, %[count]") \
        __ASM_EMIT("andps           %%xmm6, %%xmm2")            /* x2   = abs(s[k]) */ \
        __ASM_EMIT("jb              2f") \
        __ASM_EMIT("movdqa          0x10 + %[IDXS], %%xmm7")    /* x7   = D = 4 4 4 4 */ \
        \
        /* x4 blocks */ \
        __ASM_EMIT("1:") \
        __ASM_EMIT("movaps          %%xmm2, %%xmm4")            /* x4   = s[k] */ \
        __ASM_EMIT("movups          0x00(%[src]), %%xmm3")      /* x3   = S[k] */ \
        __ASM_EMIT("andps           %%xmm6, %%xmm3")            /* x3   = abs(S[k]) */ \
        __ASM_EMIT("cmpps           " op ", %%xmm3, %%xmm4")    /* x4   = s[k] <=> S[k] */ \
        __ASM_EMIT("pand            %%xmm4, %%xmm0")            /* x0   = i[k] & (s[k] <=> S[k]) */ \
        __ASM_EMIT("movdqa          %%xmm4, %%xmm5")            /* x5   = s[k] <=> S[k] */ \
        __ASM_EMIT("andps           %%xmm4, %%xmm2")            /* x2   = s[k] & (s[k] <=> S[k]) */ \
        __ASM_EMIT("pandn           %%xmm1, %%xmm5")            /* x5   = I[k] & !(s[k] <=> S[k]) */ \
        __ASM_EMIT("andnps          %%xmm3, %%xmm4")            /* x4   = S[k] & !(s[k] <=> S[k]) */ \
        __ASM_EMIT("por             %%xmm5, %%xmm0")            /* x0   = (i[k] & (s[k] <=> S[k])) | (I[k] & !(s[k] <=> S[k])) */ \
        __ASM_EMIT("orps            %%xmm4, %%xmm2")            /* x2   = (s[k] & (s[k] <=> S[k])) | (S[k] & !(s[k] <=> S[k])) */ \
        __ASM_EMIT("paddd           %%xmm7, %%xmm1")            /* x1   = I[k] = i[k]+D */ \
        __ASM_EMIT("add             $0x10, %[src]") \
        __ASM_EMIT("sub             $4, %[count]") \
        __ASM_EMIT("jae             1b") \
        /* Post-process, step 1 */ \
        __ASM_EMIT("movdqa          %%xmm1, %%xmm6") \
        __ASM_EMIT("movaps          %%xmm2, %%xmm4") \
        __ASM_EMIT("movhlps         %%xmm0, %%xmm1") \
        __ASM_EMIT("movhlps         %%xmm2, %%xmm3") \
        __ASM_EMIT("cmpps           " op ", %%xmm3, %%xmm4") \
        __ASM_EMIT("pand            %%xmm4, %%xmm0") \
        __ASM_EMIT("movdqa          %%xmm4, %%xmm5") \
        __ASM_EMIT("andps           %%xmm4, %%xmm2") \
        __ASM_EMIT("pandn           %%xmm1, %%xmm5") \
        __ASM_EMIT("andnps          %%xmm3, %%xmm4") \
        __ASM_EMIT("por             %%xmm5, %%xmm0") \
        __ASM_EMIT("orps            %%xmm4, %%xmm2") \
        /* Post-process, step 2 */ \
        __ASM_EMIT("movaps          %%xmm2, %%xmm4") \
        __ASM_EMIT("unpcklps        %%xmm2, %%xmm2") \
        __ASM_EMIT("unpcklps        %%xmm0, %%xmm0") \
        __ASM_EMIT("movhlps         %%xmm2, %%xmm3") \
        __ASM_EMIT("movhlps         %%xmm0, %%xmm1") \
        __ASM_EMIT("cmpps           " op ", %%xmm3, %%xmm4") \
        __ASM_EMIT("pand            %%xmm4, %%xmm0") \
        __ASM_EMIT("movdqa          %%xmm4, %%xmm5") \
        __ASM_EMIT("andps           %%xmm4, %%xmm2") \
        __ASM_EMIT("pandn           %%xmm1, %%xmm5") \
        __ASM_EMIT("andnps          %%xmm3, %%xmm4") \
        __ASM_EMIT("por             %%xmm5, %%xmm0") \
        __ASM_EMIT("orps            %%xmm4, %%xmm2") \
        __ASM_EMIT("movdqa          %%xmm6, %%xmm1") \
        /* x1 blocks */ \
        __ASM_EMIT("2:") \
        __ASM_EMIT("add             $3, %[count]") \
        __ASM_EMIT("jl              4f") \
        __ASM_EMIT("movdqa          0x20 + %[IDXS], %%xmm7")    /* x7   = D = 1 1 1 1 */ \
        __ASM_EMIT("movaps          %[X_SIGN], %%xmm6")         /* x6   = SIGN */ \
        __ASM_EMIT("3:") \
        __ASM_EMIT("movaps          %%xmm2, %%xmm4") \
        __ASM_EMIT("movss           0x00(%[src]), %%xmm3") \
        __ASM_EMIT("andps           %%xmm6, %%xmm3")            /* x3   = abs(S[k]) */ \
        __ASM_EMIT("cmpss           " op ", %%xmm3, %%xmm4") \
        __ASM_EMIT("pand            %%xmm4, %%xmm0") \
        __ASM_EMIT("movdqa          %%xmm4, %%xmm5") \
        __ASM_EMIT("andps           %%xmm4, %%xmm2") \
        __ASM_EMIT("pandn           %%xmm1, %%xmm5") \
        __ASM_EMIT("andnps          %%xmm3, %%xmm4") \
        __ASM_EMIT("por             %%xmm5, %%xmm0") \
        __ASM_EMIT("orps            %%xmm4, %%xmm2") \
        __ASM_EMIT("paddd           %%xmm7, %%xmm1") \
        __ASM_EMIT("add             $0x04, %[src]") \
        __ASM_EMIT("dec             %[count]") \
        __ASM_EMIT("jge             3b") \
        /* End */ \
        __ASM_EMIT("4:") \
        __ASM_EMIT("movd            %%xmm0, %[index]")

    size_t abs_min_index(const float *src, size_t count)
    {
        uint32_t index;

        ARCH_X86_ASM(
            ABS_SEARCH_CORE(CMPLTPS)
            : [src] "+r" (src), [count] "+r" (count),
              [index] "=&r" (index)
            : [IDXS] "o" (indexes),
              [X_SIGN] "m" (X_SIGN)
            : "cc",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );

        return index;
    }

    size_t abs_max_index(const float *src, size_t count)
    {
        uint32_t index;

        ARCH_X86_ASM(
            ABS_SEARCH_CORE(CMPGTPS)
            : [src] "+r" (src), [count] "+r" (count),
              [index] "=&r" (index)
            : [IDXS] "o" (indexes),
              [X_SIGN] "m" (X_SIGN)
            : "cc",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );

        return index;
    }

    #undef ABS_SEARCH_CORE

    #undef CMPGTPS
    #undef CMPLTPS
}

#endif /* DSP_ARCH_X86_SSE2_SEARCH_H_ */
