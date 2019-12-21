/*
 * search.h
 *
 *  Created on: 9 окт. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_SSE2_SEARCH_IMINMAX_H_
#define DSP_ARCH_X86_SSE2_SEARCH_IMINMAX_H_

#ifndef DSP_ARCH_X86_SSE2_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_SSE2_IMPL */

namespace sse2
{
    IF_ARCH_X86(
        static uint32_t indexes[] __lsp_aligned16 =
        {
            0, 1, 2, 3, // indexes
            4, 4, 4, 4, // step
            1, 1, 1, 1
        };
    )

    #define CMPLTPS     "$1"
    #define CMPLEPS     "$2"
    #define CMPGEPS     "$5"
    #define CMPGTPS     "$6"

    /* Register allocation:
     * xmm0     = rind  - result indexes
     * xmm1     = cind  - current indexes
     * xmm2     = rval  - result values
     * xmm3     = cval  - current values
     * xmm4     = xcmp  - comparison mask
     * xmm5     = temp
     * xmm6     = temp
     * xmm7     = incr  - increment
     */
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
        __ASM_EMIT("movdqa          %%xmm4, %%xmm5")            /* x5   = s[k] <=> S[k] */ \
        __ASM_EMIT("pand            %%xmm5, %%xmm0")            /* x0   = i[k] & (s[k] <=> S[k]) */ \
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
        __ASM_EMIT("movdqa          %%xmm0, %%xmm6")            /* x5   = rind = i0 i1 i2 i3 */ \
        __ASM_EMIT("movaps          %%xmm2, %%xmm3")            /* x3   = rval = x0 x1 x2 x3 */ \
        __ASM_EMIT("pshufd          $0x39, %%xmm6, %%xmm6")     /* x6   = cind = i1 i2 i3 i0 */ \
        __ASM_EMIT("shufps          $0x39, %%xmm3, %%xmm3")     /* x3   = cval = x1 x2 x3 x0 */ \
        __ASM_EMIT("movaps          %%xmm2, %%xmm4")            /* x4   = x0 x1 x2 x3 */ \
        __ASM_EMIT("cmpps           " op ", %%xmm3, %%xmm4")    /* x4   = cmp(cval, nval) */ \
        __ASM_EMIT("movdqa          %%xmm4, %%xmm5") \
        __ASM_EMIT("pand            %%xmm5, %%xmm0") \
        __ASM_EMIT("andps           %%xmm4, %%xmm2") \
        __ASM_EMIT("pandn           %%xmm6, %%xmm5") \
        __ASM_EMIT("andnps          %%xmm3, %%xmm4") \
        __ASM_EMIT("por             %%xmm5, %%xmm0") \
        __ASM_EMIT("orps            %%xmm4, %%xmm2") \
        /* Post-process, step 2 */ \
        __ASM_EMIT("movdqa          %%xmm0, %%xmm6")            /* x5   = rind = i0 i1 i2 i3 */ \
        __ASM_EMIT("movaps          %%xmm2, %%xmm3")            /* x3   = rval = x0 x1 x2 x3 */ \
        __ASM_EMIT("punpckhdq       %%xmm6, %%xmm6")            /* x6   = cind = i2 i2 i3 i3 */ \
        __ASM_EMIT("unpckhps        %%xmm3, %%xmm3")            /* x3   = cval = x2 x3 x2 x3 */ \
        __ASM_EMIT("movaps          %%xmm2, %%xmm4")            /* x4   = x0 x1 x2 x3 */ \
        __ASM_EMIT("cmpps           " op ", %%xmm3, %%xmm4")    /* x4   = cmp(cval, nval) */ \
        __ASM_EMIT("movdqa          %%xmm4, %%xmm5") \
        __ASM_EMIT("pand            %%xmm5, %%xmm0") \
        __ASM_EMIT("andps           %%xmm4, %%xmm2") \
        __ASM_EMIT("pandn           %%xmm6, %%xmm5") \
        __ASM_EMIT("andnps          %%xmm3, %%xmm4") \
        __ASM_EMIT("por             %%xmm5, %%xmm0") \
        __ASM_EMIT("orps            %%xmm4, %%xmm2") \
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
        __ASM_EMIT("4:")


    size_t min_index(const float *src, size_t count)
    {
        uint32_t index;

        ARCH_X86_ASM(
            SEARCH_CORE(CMPLEPS)
            : [src] "+r" (src), [count] "+r" (count),
              [index] "=Yz" (index)
            : [IDXS] "o" (indexes)
            : "cc",
              "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );

        return index;
    }

    size_t max_index(const float *src, size_t count)
    {
        uint32_t index;

        ARCH_X86_ASM(
            SEARCH_CORE(CMPGEPS)
            : [src] "+r" (src), [count] "+r" (count),
              [index] "=Yz" (index)
            : [IDXS] "o" (indexes)
            : "cc",
              "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );

        return index;
    }

    #undef SEARCH_CORE

    /* Register allocation:
     * xmm0     = rind  - result indexes
     * xmm1     = cind  - current indexes
     * xmm2     = rval  - result values
     * xmm3     = cval  - current values
     * xmm4     = xcmp  - comparison mask
     * xmm5     = temp
     * xmm6     = sign  - sign mask
     * xmm7     = incr  - increment
     */
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
        __ASM_EMIT("movdqa          %%xmm4, %%xmm5")            /* x5   = s[k] <=> S[k] */ \
        __ASM_EMIT("pand            %%xmm5, %%xmm0")            /* x0   = i[k] & (s[k] <=> S[k]) */ \
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
        __ASM_EMIT("movdqa          %%xmm0, %%xmm6")            /* x5   = rind = i0 i1 i2 i3 */ \
        __ASM_EMIT("movaps          %%xmm2, %%xmm3")            /* x3   = rval = x0 x1 x2 x3 */ \
        __ASM_EMIT("pshufd          $0x39, %%xmm6, %%xmm6")     /* x6   = cind = i1 i2 i3 i0 */ \
        __ASM_EMIT("shufps          $0x39, %%xmm3, %%xmm3")     /* x3   = cval = x1 x2 x3 x0 */ \
        __ASM_EMIT("movaps          %%xmm2, %%xmm4")            /* x4   = x0 x1 x2 x3 */ \
        __ASM_EMIT("cmpps           " op ", %%xmm3, %%xmm4")    /* x4   = cmp(cval, nval) */ \
        __ASM_EMIT("movdqa          %%xmm4, %%xmm5") \
        __ASM_EMIT("pand            %%xmm5, %%xmm0") \
        __ASM_EMIT("andps           %%xmm4, %%xmm2") \
        __ASM_EMIT("pandn           %%xmm6, %%xmm5") \
        __ASM_EMIT("andnps          %%xmm3, %%xmm4") \
        __ASM_EMIT("por             %%xmm5, %%xmm0") \
        __ASM_EMIT("orps            %%xmm4, %%xmm2") \
        /* Post-process, step 2 */ \
        __ASM_EMIT("movdqa          %%xmm0, %%xmm6")            /* x5   = rind = i0 i1 i2 i3 */ \
        __ASM_EMIT("movaps          %%xmm2, %%xmm3")            /* x3   = rval = x0 x1 x2 x3 */ \
        __ASM_EMIT("punpckhdq       %%xmm6, %%xmm6")            /* x6   = cind = i2 i2 i3 i3 */ \
        __ASM_EMIT("unpckhps        %%xmm3, %%xmm3")            /* x3   = cval = x2 x3 x2 x3 */ \
        __ASM_EMIT("movaps          %%xmm2, %%xmm4")            /* x4   = x0 x1 x2 x3 */ \
        __ASM_EMIT("cmpps           " op ", %%xmm3, %%xmm4")    /* x4   = cmp(cval, nval) */ \
        __ASM_EMIT("movdqa          %%xmm4, %%xmm5") \
        __ASM_EMIT("pand            %%xmm5, %%xmm0") \
        __ASM_EMIT("andps           %%xmm4, %%xmm2") \
        __ASM_EMIT("pandn           %%xmm6, %%xmm5") \
        __ASM_EMIT("andnps          %%xmm3, %%xmm4") \
        __ASM_EMIT("por             %%xmm5, %%xmm0") \
        __ASM_EMIT("orps            %%xmm4, %%xmm2") \
        __ASM_EMIT("movaps          %[X_SIGN], %%xmm6")         /* x6   = SIGN (restore) */ \
        /* x1 blocks */ \
        __ASM_EMIT("2:") \
        __ASM_EMIT("add             $3, %[count]") \
        __ASM_EMIT("jl              4f") \
        __ASM_EMIT("movdqa          0x20 + %[IDXS], %%xmm7")    /* x7   = D = 1 1 1 1 */ \
        __ASM_EMIT("3:") \
        __ASM_EMIT("movaps          %%xmm2, %%xmm4") \
        __ASM_EMIT("movss           0x00(%[src]), %%xmm3") \
        __ASM_EMIT("andps           %%xmm6, %%xmm3")            /* x3   = abs(S[k]) */ \
        __ASM_EMIT("cmpps           " op ", %%xmm3, %%xmm4") \
        __ASM_EMIT("movdqa          %%xmm4, %%xmm5")            /* x5   = s[k] <=> S[k] */ \
        __ASM_EMIT("pand            %%xmm5, %%xmm0")            /* x0   = i[k] & (s[k] <=> S[k]) */ \
        __ASM_EMIT("andps           %%xmm4, %%xmm2")            /* x2   = s[k] & (s[k] <=> S[k]) */ \
        __ASM_EMIT("pandn           %%xmm1, %%xmm5")            /* x5   = I[k] & !(s[k] <=> S[k]) */ \
        __ASM_EMIT("andnps          %%xmm3, %%xmm4")            /* x4   = S[k] & !(s[k] <=> S[k]) */ \
        __ASM_EMIT("por             %%xmm5, %%xmm0")            /* x0   = (i[k] & (s[k] <=> S[k])) | (I[k] & !(s[k] <=> S[k])) */ \
        __ASM_EMIT("orps            %%xmm4, %%xmm2")            /* x2   = (s[k] & (s[k] <=> S[k])) | (S[k] & !(s[k] <=> S[k])) */ \
        __ASM_EMIT("paddd           %%xmm7, %%xmm1")            /* x1   = I[k] = i[k]+D */ \
        __ASM_EMIT("add             $0x04, %[src]") \
        __ASM_EMIT("dec             %[count]") \
        __ASM_EMIT("jge             3b") \
        /* End */ \
        __ASM_EMIT("4:")

    size_t abs_min_index(const float *src, size_t count)
    {
        uint32_t index;

        ARCH_X86_ASM(
            ABS_SEARCH_CORE(CMPLEPS)
            : [src] "+r" (src), [count] "+r" (count),
              [index] "=Yz" (index)
            : [IDXS] "o" (indexes),
              [X_SIGN] "m" (X_SIGN)
            : "cc",
              "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );

        return index;
    }

    size_t abs_max_index(const float *src, size_t count)
    {
        uint32_t index;

        ARCH_X86_ASM(
            ABS_SEARCH_CORE(CMPGEPS)
            : [src] "+r" (src), [count] "+r" (count),
              [index] "=Yz" (index)
            : [IDXS] "o" (indexes),
              [X_SIGN] "m" (X_SIGN)
            : "cc",
              "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );

        return index;
    }

    #undef ABS_SEARCH_CORE

    void minmax_index(const float *src, size_t count, size_t *min, size_t *max)
    {
        uint32_t counters[4] __lsp_aligned16;

        ARCH_X86_ASM(
            __ASM_EMIT("pxor            %%xmm0, %%xmm0")            // x0 = idx_min
            __ASM_EMIT("pxor            %%xmm1, %%xmm1")            // x1 = idx_max
            __ASM_EMIT("test            %[count], %[count]")
            __ASM_EMIT("jz              4f")

            __ASM_EMIT("movss           0x00(%[src]), %%xmm2")      // x2   = min
            __ASM_EMIT("shufps          $0x00, %%xmm2, %%xmm2")     // x2   = min
            __ASM_EMIT("movdqa          0x00 + %[IDXS], %%xmm4")    // x4   = idx_new
            __ASM_EMIT("movaps          %%xmm2, %%xmm3")            // x3   = max
            __ASM_EMIT("sub             $0x04, %[count]")
            __ASM_EMIT("jb              2f")

            // x4 blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("movups          0x00(%[src]), %%xmm5")      // x5   = sample
            // Find minimum
            __ASM_EMIT("movaps          %%xmm2, %%xmm6")            // x6   = min
            __ASM_EMIT("cmpps " CMPLEPS ", %%xmm5, %%xmm6")         // x6   = min <= sample
            __ASM_EMIT("movdqa          %%xmm6, %%xmm7")            // x7   = min <= sample
            __ASM_EMIT("pand            %%xmm6, %%xmm0")            // x0   = idx_min & (min <= sample)
            __ASM_EMIT("andps           %%xmm7, %%xmm2")            // x2   = min & (min <= sample)
            __ASM_EMIT("pandn           %%xmm4, %%xmm6")            // x6   = idx_new & !(min <= sample)
            __ASM_EMIT("andnps          %%xmm5, %%xmm7")            // x7   = sample & !(min <= sample)
            __ASM_EMIT("por             %%xmm6, %%xmm0")            // x0   = idx_min & (min <= sample) | idx_new & !(min <= sample)
            __ASM_EMIT("orps            %%xmm7, %%xmm2")            // x2   = min & (min <= sample) | sample & !(min <= sample)
            // Find maximum
            __ASM_EMIT("movaps          %%xmm3, %%xmm6")            // x6   = max
            __ASM_EMIT("cmpps " CMPGEPS ", %%xmm5, %%xmm6")         // x6   = max >= sample
            __ASM_EMIT("movdqa          %%xmm6, %%xmm7")            // x7   = max >= sample
            __ASM_EMIT("pand            %%xmm6, %%xmm1")            // x1   = idx_max & (max >= sample)
            __ASM_EMIT("andps           %%xmm7, %%xmm3")            // x3   = max & (max >= sample)
            __ASM_EMIT("pandn           %%xmm4, %%xmm6")            // x6   = idx_new & !(max >= sample)
            __ASM_EMIT("andnps          %%xmm5, %%xmm7")            // x7   = sample & !(max >= sample)
            __ASM_EMIT("por             %%xmm6, %%xmm1")            // x1   = idx_max & (max >= sample) | idx_new & !(max >= sample)
            __ASM_EMIT("orps            %%xmm7, %%xmm3")            // x3   = max & (max >= sample) | sample & !(max >= sample)
            // Next loop
            __ASM_EMIT("paddd           0x10 + %[IDXS], %%xmm4")    // x4   = idx_new + 4
            __ASM_EMIT("add             $0x10, %[src]")             // src += 4
            __ASM_EMIT("sub             $4, %[count]")              // count -= 4
            __ASM_EMIT("jae             1b")

            // Post-process
            __ASM_EMIT("movdqa          %%xmm4, %[COUNTERS]")       // Save current index
            // Step 1
            __ASM_EMIT("movhlps         %%xmm0, %%xmm4")
            __ASM_EMIT("movhlps         %%xmm2, %%xmm5")
            __ASM_EMIT("movaps          %%xmm2, %%xmm6")
            __ASM_EMIT("cmpps " CMPLEPS ", %%xmm5, %%xmm6")
            __ASM_EMIT("movdqa          %%xmm6, %%xmm7")
            __ASM_EMIT("pand            %%xmm6, %%xmm0")
            __ASM_EMIT("andps           %%xmm7, %%xmm2")
            __ASM_EMIT("pandn           %%xmm4, %%xmm6")
            __ASM_EMIT("andnps          %%xmm5, %%xmm7")
            __ASM_EMIT("por             %%xmm6, %%xmm0")
            __ASM_EMIT("orps            %%xmm7, %%xmm2")

            __ASM_EMIT("movhlps         %%xmm1, %%xmm4")
            __ASM_EMIT("movhlps         %%xmm3, %%xmm5")
            __ASM_EMIT("movaps          %%xmm3, %%xmm6")
            __ASM_EMIT("cmpps " CMPGEPS ", %%xmm5, %%xmm6")
            __ASM_EMIT("movdqa          %%xmm6, %%xmm7")
            __ASM_EMIT("pand            %%xmm6, %%xmm1")
            __ASM_EMIT("andps           %%xmm7, %%xmm3")
            __ASM_EMIT("pandn           %%xmm4, %%xmm6")
            __ASM_EMIT("andnps          %%xmm5, %%xmm7")
            __ASM_EMIT("por             %%xmm6, %%xmm1")
            __ASM_EMIT("orps            %%xmm7, %%xmm3")

            // Step 2
            __ASM_EMIT("unpcklps        %%xmm0, %%xmm0")
            __ASM_EMIT("unpcklps        %%xmm2, %%xmm2")
            __ASM_EMIT("movhlps         %%xmm0, %%xmm4")
            __ASM_EMIT("movhlps         %%xmm2, %%xmm5")
            __ASM_EMIT("movaps          %%xmm2, %%xmm6")
            __ASM_EMIT("cmpps " CMPLEPS ", %%xmm5, %%xmm6")
            __ASM_EMIT("movdqa          %%xmm6, %%xmm7")
            __ASM_EMIT("pand            %%xmm6, %%xmm0")
            __ASM_EMIT("andps           %%xmm7, %%xmm2")
            __ASM_EMIT("pandn           %%xmm4, %%xmm6")
            __ASM_EMIT("andnps          %%xmm5, %%xmm7")
            __ASM_EMIT("por             %%xmm6, %%xmm0")
            __ASM_EMIT("orps            %%xmm7, %%xmm2")

            __ASM_EMIT("unpcklps        %%xmm1, %%xmm1")
            __ASM_EMIT("unpcklps        %%xmm3, %%xmm3")
            __ASM_EMIT("movhlps         %%xmm1, %%xmm4")
            __ASM_EMIT("movhlps         %%xmm3, %%xmm5")
            __ASM_EMIT("movaps          %%xmm3, %%xmm6")
            __ASM_EMIT("cmpps " CMPGEPS ", %%xmm5, %%xmm6")
            __ASM_EMIT("movdqa          %%xmm6, %%xmm7")
            __ASM_EMIT("pand            %%xmm6, %%xmm1")
            __ASM_EMIT("andps           %%xmm7, %%xmm3")
            __ASM_EMIT("pandn           %%xmm4, %%xmm6")
            __ASM_EMIT("andnps          %%xmm5, %%xmm7")
            __ASM_EMIT("por             %%xmm6, %%xmm1")
            __ASM_EMIT("orps            %%xmm7, %%xmm3")

            __ASM_EMIT("movdqa          %[COUNTERS], %%xmm4")       // Restore current index

            __ASM_EMIT("2:")
            __ASM_EMIT("add             $3, %[count]")
            __ASM_EMIT("jl              4f")

            // x1 blocks
            __ASM_EMIT("3:")
            __ASM_EMIT("movss           0x00(%[src]), %%xmm5")      // x5   = sample
            // Find minimum
            __ASM_EMIT("movaps          %%xmm2, %%xmm6")            // x6   = min
            __ASM_EMIT("cmpss " CMPLEPS ", %%xmm5, %%xmm6")         // x6   = min <= sample
            __ASM_EMIT("movdqa          %%xmm6, %%xmm7")            // x7   = min <= sample
            __ASM_EMIT("pand            %%xmm6, %%xmm0")            // x0   = idx_min & (min <= sample)
            __ASM_EMIT("andps           %%xmm7, %%xmm2")            // x2   = min & (min <= sample)
            __ASM_EMIT("pandn           %%xmm4, %%xmm6")            // x6   = idx_new & !(min <= sample)
            __ASM_EMIT("andnps          %%xmm5, %%xmm7")            // x7   = sample & !(min <= sample)
            __ASM_EMIT("por             %%xmm6, %%xmm0")            // x0   = idx_min & (min <= sample) | idx_new & !(min <= sample)
            __ASM_EMIT("orps            %%xmm7, %%xmm2")            // x2   = min & (min <= sample) | sample & !(min <= sample)
            // Find maximum
            __ASM_EMIT("movaps          %%xmm3, %%xmm6")            // x6   = max
            __ASM_EMIT("cmpss " CMPGEPS ", %%xmm5, %%xmm6")         // x6   = max >= sample
            __ASM_EMIT("movdqa          %%xmm6, %%xmm7")            // x7   = max >= sample
            __ASM_EMIT("pand            %%xmm6, %%xmm1")            // x1   = idx_max & (max >= sample)
            __ASM_EMIT("andps           %%xmm7, %%xmm3")            // x3   = max & (max >= sample)
            __ASM_EMIT("pandn           %%xmm4, %%xmm6")            // x6   = idx_new & !(max >= sample)
            __ASM_EMIT("andnps          %%xmm5, %%xmm7")            // x7   = sample & !(max >= sample)
            __ASM_EMIT("por             %%xmm6, %%xmm1")            // x1   = idx_max & (max >= sample) | idx_new & !(max >= sample)
            __ASM_EMIT("orps            %%xmm7, %%xmm3")            // x3   = max & (max >= sample) | sample & !(max >= sample)
            // Next loop
            __ASM_EMIT("paddd           0x20 + %[IDXS], %%xmm4")    // x4   = idx_new + 1
            __ASM_EMIT("add             $0x04, %[src]")             // src += 4
            __ASM_EMIT("dec             %[count]")                  // count --
            __ASM_EMIT("jge             3b")

            __ASM_EMIT("4:")
            __ASM_EMIT("movd            %%xmm0, (%[min])")
            __ASM_EMIT("movd            %%xmm1, (%[max])")
            __ASM_EMIT64("movl          $0, 0x04(%[min])")
            __ASM_EMIT64("movl          $0, 0x04(%[max])")
            : [src] "+r" (src), [count] "+r" (count)
            : [min] "r" (min), [max] "r" (max),
              [IDXS] "o" (indexes),
              [COUNTERS] "m" (counters)
        );
    }

    void abs_minmax_index(const float *src, size_t count, size_t *min, size_t *max)
    {
        uint32_t counters[4] __lsp_aligned16;

        ARCH_X86_ASM(
            __ASM_EMIT("pxor            %%xmm0, %%xmm0")            // x0 = idx_min
            __ASM_EMIT("pxor            %%xmm1, %%xmm1")            // x1 = idx_max
            __ASM_EMIT("test            %[count], %[count]")
            __ASM_EMIT("jz              4f")

            __ASM_EMIT("movss           0x00(%[src]), %%xmm2")      // x2   = min
            __ASM_EMIT("shufps          $0x00, %%xmm2, %%xmm2")     // x2   = min
            __ASM_EMIT("movdqa          0x00 + %[IDXS], %%xmm4")    // x4   = idx_new
            __ASM_EMIT("andps           %[X_SIGN], %%xmm2")         // x5   = abs(sample)
            __ASM_EMIT("movaps          %%xmm2, %%xmm3")            // x3   = max = abs(sample)
            __ASM_EMIT("sub             $0x04, %[count]")
            __ASM_EMIT("jb              2f")

            // x4 blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("movups          0x00(%[src]), %%xmm5")      // x5   = sample
            // Find minimum
            __ASM_EMIT("movaps          %%xmm2, %%xmm6")            // x6   = min
            __ASM_EMIT("andps           %[X_SIGN], %%xmm5")         // x5   = abs(sample)
            __ASM_EMIT("cmpps " CMPLEPS ", %%xmm5, %%xmm6")         // x6   = min <= sample
            __ASM_EMIT("movdqa          %%xmm6, %%xmm7")            // x7   = min <= sample
            __ASM_EMIT("pand            %%xmm6, %%xmm0")            // x0   = idx_min & (min <= sample)
            __ASM_EMIT("andps           %%xmm7, %%xmm2")            // x2   = min & (min <= sample)
            __ASM_EMIT("pandn           %%xmm4, %%xmm6")            // x6   = idx_new & !(min <= sample)
            __ASM_EMIT("andnps          %%xmm5, %%xmm7")            // x7   = sample & !(min <= sample)
            __ASM_EMIT("por             %%xmm6, %%xmm0")            // x0   = idx_min & (min <= sample) | idx_new & !(min <= sample)
            __ASM_EMIT("orps            %%xmm7, %%xmm2")            // x2   = min & (min <= sample) | sample & !(min <= sample)
            // Find maximum
            __ASM_EMIT("movaps          %%xmm3, %%xmm6")            // x6   = max
            __ASM_EMIT("cmpps " CMPGEPS ", %%xmm5, %%xmm6")         // x6   = max >= sample
            __ASM_EMIT("movdqa          %%xmm6, %%xmm7")            // x7   = max >= sample
            __ASM_EMIT("pand            %%xmm6, %%xmm1")            // x1   = idx_max & (max >= sample)
            __ASM_EMIT("andps           %%xmm7, %%xmm3")            // x3   = max & (max >= sample)
            __ASM_EMIT("pandn           %%xmm4, %%xmm6")            // x6   = idx_new & !(max >= sample)
            __ASM_EMIT("andnps          %%xmm5, %%xmm7")            // x7   = sample & !(max >= sample)
            __ASM_EMIT("por             %%xmm6, %%xmm1")            // x1   = idx_max & (max >= sample) | idx_new & !(max >= sample)
            __ASM_EMIT("orps            %%xmm7, %%xmm3")            // x3   = max & (max >= sample) | sample & !(max >= sample)
            // Next loop
            __ASM_EMIT("paddd           0x10 + %[IDXS], %%xmm4")    // x4   = idx_new + 4
            __ASM_EMIT("add             $0x10, %[src]")             // src += 4
            __ASM_EMIT("sub             $4, %[count]")              // count -= 4
            __ASM_EMIT("jae             1b")

            // Post-process
            __ASM_EMIT("movdqa          %%xmm4, %[COUNTERS]")       // Save current index
            // Step 1
            __ASM_EMIT("movhlps         %%xmm0, %%xmm4")
            __ASM_EMIT("movhlps         %%xmm2, %%xmm5")
            __ASM_EMIT("movaps          %%xmm2, %%xmm6")
            __ASM_EMIT("cmpps " CMPLEPS ", %%xmm5, %%xmm6")
            __ASM_EMIT("movdqa          %%xmm6, %%xmm7")
            __ASM_EMIT("pand            %%xmm6, %%xmm0")
            __ASM_EMIT("andps           %%xmm7, %%xmm2")
            __ASM_EMIT("pandn           %%xmm4, %%xmm6")
            __ASM_EMIT("andnps          %%xmm5, %%xmm7")
            __ASM_EMIT("por             %%xmm6, %%xmm0")
            __ASM_EMIT("orps            %%xmm7, %%xmm2")

            __ASM_EMIT("movhlps         %%xmm1, %%xmm4")
            __ASM_EMIT("movhlps         %%xmm3, %%xmm5")
            __ASM_EMIT("movaps          %%xmm3, %%xmm6")
            __ASM_EMIT("cmpps " CMPGEPS ", %%xmm5, %%xmm6")
            __ASM_EMIT("movdqa          %%xmm6, %%xmm7")
            __ASM_EMIT("pand            %%xmm6, %%xmm1")
            __ASM_EMIT("andps           %%xmm7, %%xmm3")
            __ASM_EMIT("pandn           %%xmm4, %%xmm6")
            __ASM_EMIT("andnps          %%xmm5, %%xmm7")
            __ASM_EMIT("por             %%xmm6, %%xmm1")
            __ASM_EMIT("orps            %%xmm7, %%xmm3")

            // Step 2
            __ASM_EMIT("unpcklps        %%xmm0, %%xmm0")
            __ASM_EMIT("unpcklps        %%xmm2, %%xmm2")
            __ASM_EMIT("movhlps         %%xmm0, %%xmm4")
            __ASM_EMIT("movhlps         %%xmm2, %%xmm5")
            __ASM_EMIT("movaps          %%xmm2, %%xmm6")
            __ASM_EMIT("cmpps " CMPLEPS ", %%xmm5, %%xmm6")
            __ASM_EMIT("movdqa          %%xmm6, %%xmm7")
            __ASM_EMIT("pand            %%xmm6, %%xmm0")
            __ASM_EMIT("andps           %%xmm7, %%xmm2")
            __ASM_EMIT("pandn           %%xmm4, %%xmm6")
            __ASM_EMIT("andnps          %%xmm5, %%xmm7")
            __ASM_EMIT("por             %%xmm6, %%xmm0")
            __ASM_EMIT("orps            %%xmm7, %%xmm2")

            __ASM_EMIT("unpcklps        %%xmm1, %%xmm1")
            __ASM_EMIT("unpcklps        %%xmm3, %%xmm3")
            __ASM_EMIT("movhlps         %%xmm1, %%xmm4")
            __ASM_EMIT("movhlps         %%xmm3, %%xmm5")
            __ASM_EMIT("movaps          %%xmm3, %%xmm6")
            __ASM_EMIT("cmpps " CMPGEPS ", %%xmm5, %%xmm6")
            __ASM_EMIT("movdqa          %%xmm6, %%xmm7")
            __ASM_EMIT("pand            %%xmm6, %%xmm1")
            __ASM_EMIT("andps           %%xmm7, %%xmm3")
            __ASM_EMIT("pandn           %%xmm4, %%xmm6")
            __ASM_EMIT("andnps          %%xmm5, %%xmm7")
            __ASM_EMIT("por             %%xmm6, %%xmm1")
            __ASM_EMIT("orps            %%xmm7, %%xmm3")

            __ASM_EMIT("movdqa          %[COUNTERS], %%xmm4")       // Restore current index

            __ASM_EMIT("2:")
            __ASM_EMIT("add             $3, %[count]")
            __ASM_EMIT("jl              4f")

            // x1 blocks
            __ASM_EMIT("3:")
            __ASM_EMIT("movss           0x00(%[src]), %%xmm5")      // x5   = sample
            // Find minimum
            __ASM_EMIT("movaps          %%xmm2, %%xmm6")            // x6   = min
            __ASM_EMIT("andps           %[X_SIGN], %%xmm5")         // x5   = abs(sample)
            __ASM_EMIT("cmpss " CMPLEPS ", %%xmm5, %%xmm6")         // x6   = min <= sample
            __ASM_EMIT("movdqa          %%xmm6, %%xmm7")            // x7   = min <= sample
            __ASM_EMIT("pand            %%xmm6, %%xmm0")            // x0   = idx_min & (min <= sample)
            __ASM_EMIT("andps           %%xmm7, %%xmm2")            // x2   = min & (min <= sample)
            __ASM_EMIT("pandn           %%xmm4, %%xmm6")            // x6   = idx_new & !(min <= sample)
            __ASM_EMIT("andnps          %%xmm5, %%xmm7")            // x7   = sample & !(min <= sample)
            __ASM_EMIT("por             %%xmm6, %%xmm0")            // x0   = idx_min & (min <= sample) | idx_new & !(min <= sample)
            __ASM_EMIT("orps            %%xmm7, %%xmm2")            // x2   = min & (min <= sample) | sample & !(min <= sample)
            // Find maximum
            __ASM_EMIT("movaps          %%xmm3, %%xmm6")            // x6   = max
            __ASM_EMIT("cmpss " CMPGEPS ", %%xmm5, %%xmm6")         // x6   = max >= sample
            __ASM_EMIT("movdqa          %%xmm6, %%xmm7")            // x7   = max >= sample
            __ASM_EMIT("pand            %%xmm6, %%xmm1")            // x1   = idx_max & (max >= sample)
            __ASM_EMIT("andps           %%xmm7, %%xmm3")            // x3   = max & (max >= sample)
            __ASM_EMIT("pandn           %%xmm4, %%xmm6")            // x6   = idx_new & !(max >= sample)
            __ASM_EMIT("andnps          %%xmm5, %%xmm7")            // x7   = sample & !(max >= sample)
            __ASM_EMIT("por             %%xmm6, %%xmm1")            // x1   = idx_max & (max >= sample) | idx_new & !(max >= sample)
            __ASM_EMIT("orps            %%xmm7, %%xmm3")            // x3   = max & (max >= sample) | sample & !(max >= sample)
            // Next loop
            __ASM_EMIT("paddd           0x20 + %[IDXS], %%xmm4")    // x4   = idx_new + 1
            __ASM_EMIT("add             $0x04, %[src]")             // src += 4
            __ASM_EMIT("dec             %[count]")                  // count --
            __ASM_EMIT("jge             3b")

            __ASM_EMIT("4:")
            __ASM_EMIT("movd            %%xmm0, (%[min])")
            __ASM_EMIT("movd            %%xmm1, (%[max])")
            __ASM_EMIT64("movl          $0, 0x04(%[min])")
            __ASM_EMIT64("movl          $0, 0x04(%[max])")
            : [src] "+r" (src), [count] "+r" (count)
            : [min] "r" (min), [max] "r" (max),
              [IDXS] "o" (indexes),
              [X_SIGN] "o" (X_SIGN),
              [COUNTERS] "m" (counters)
        );
    }

    #undef CMPGTPS
    #undef CMPGEPS
    #undef CMPLTPS
    #undef CMPLEPS
}

#endif /* DSP_ARCH_X86_SSE2_SEARCH_IMINMAX_H_ */
