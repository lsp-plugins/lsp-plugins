/*
 * minmax.h
 *
 *  Created on: 4 дек. 2019 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_AARCH64_ASIMD_SEARCH_MINMAX_H_
#define DSP_ARCH_AARCH64_ASIMD_SEARCH_MINMAX_H_

#ifndef DSP_ARCH_AARCH64_ASIMD_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_AARCH64_ASIMD_IMPL */

namespace asimd
{
    #define MOD_NONE(src, dst)      ""
    #define MOD_ABS(src, dst)       "fabs " dst ", " src

    #define OP_SEARCH_CORE(DST, SRC, OP, MOD) \
        __ASM_EMIT("cmp         %[count], #1") \
        __ASM_EMIT("b.lo        100f") \
        /* 32x blocks */ \
        __ASM_EMIT("ld1r        {v0.4s}, [%[" SRC "]]") \
        __ASM_EMIT(MOD("v0.4s", "v0.4s")) \
        __ASM_EMIT("subs        %[count], %[count], #32") \
        __ASM_EMIT("mov         v1.16b, v0.16b") \
        __ASM_EMIT("b.lt        2f") \
        __ASM_EMIT("1:") \
        __ASM_EMIT("ldp         q16, q17, [%[" SRC "], #0x00]") \
        __ASM_EMIT("ldp         q18, q19, [%[" SRC "], #0x20]") \
        __ASM_EMIT("ldp         q20, q21, [%[" SRC "], #0x40]") \
        __ASM_EMIT("ldp         q22, q23, [%[" SRC "], #0x60]") \
        __ASM_EMIT(MOD("v16.4s", "v16.4s")) \
        __ASM_EMIT(MOD("v17.4s", "v17.4s")) \
        __ASM_EMIT(MOD("v18.4s", "v18.4s")) \
        __ASM_EMIT(MOD("v19.4s", "v19.4s")) \
        __ASM_EMIT(MOD("v20.4s", "v20.4s")) \
        __ASM_EMIT(MOD("v21.4s", "v21.4s")) \
        __ASM_EMIT(MOD("v22.4s", "v22.4s")) \
        __ASM_EMIT(MOD("v23.4s", "v23.4s")) \
        __ASM_EMIT(OP "         v0.4s, v0.4s, v16.4s") \
        __ASM_EMIT(OP "         v1.4s, v1.4s, v17.4s") \
        __ASM_EMIT(OP "         v0.4s, v0.4s, v18.4s") \
        __ASM_EMIT(OP "         v1.4s, v1.4s, v19.4s") \
        __ASM_EMIT(OP "         v0.4s, v0.4s, v20.4s") \
        __ASM_EMIT(OP "         v1.4s, v1.4s, v21.4s") \
        __ASM_EMIT(OP "         v0.4s, v0.4s, v22.4s") \
        __ASM_EMIT(OP "         v1.4s, v1.4s, v23.4s") \
        __ASM_EMIT("subs        %[count], %[count], #32") \
        __ASM_EMIT("add         %[" SRC "], %[" SRC "], #0x80") \
        __ASM_EMIT("b.hs        1b") \
        /* 16x block */ \
        __ASM_EMIT("2:") \
        __ASM_EMIT("adds        %[count], %[count], #16") \
        __ASM_EMIT("b.lt        4f") \
        __ASM_EMIT("ldp         q16, q17, [%[" SRC "], #0x00]") \
        __ASM_EMIT("ldp         q18, q19, [%[" SRC "], #0x20]") \
        __ASM_EMIT(MOD("v16.4s", "v16.4s")) \
        __ASM_EMIT(MOD("v17.4s", "v17.4s")) \
        __ASM_EMIT(MOD("v18.4s", "v18.4s")) \
        __ASM_EMIT(MOD("v19.4s", "v19.4s")) \
        __ASM_EMIT(OP "         v0.4s, v0.4s, v16.4s") \
        __ASM_EMIT(OP "         v1.4s, v1.4s, v17.4s") \
        __ASM_EMIT(OP "         v0.4s, v0.4s, v18.4s") \
        __ASM_EMIT(OP "         v1.4s, v1.4s, v19.4s") \
        __ASM_EMIT("sub         %[count], %[count], #16") \
        __ASM_EMIT("add         %[" SRC "], %[" SRC "], #0x40") \
        /* 8x block */ \
        __ASM_EMIT("4:") \
        __ASM_EMIT("adds        %[count], %[count], #8") \
        __ASM_EMIT("b.lt        6f") \
        __ASM_EMIT("ldp         q16, q17, [%[" SRC "], #0x00]") \
        __ASM_EMIT(MOD("v16.4s", "v16.4s")) \
        __ASM_EMIT(MOD("v17.4s", "v17.4s")) \
        __ASM_EMIT(OP "         v0.4s, v0.4s, v16.4s") \
        __ASM_EMIT(OP "         v1.4s, v1.4s, v17.4s") \
        __ASM_EMIT("sub         %[count], %[count], #8") \
        __ASM_EMIT("add         %[" SRC "], %[" SRC "], #0x20") \
        /* 4x block */ \
        __ASM_EMIT("6:") \
        __ASM_EMIT(OP "         v0.4s, v0.4s, v1.4s") \
        __ASM_EMIT("adds        %[count], %[count], #4") \
        __ASM_EMIT("b.lt        8f") \
        __ASM_EMIT("ldr         q16, [%[" SRC "], #0x00]") \
        __ASM_EMIT(MOD("v16.4s", "v16.4s")) \
        __ASM_EMIT(OP "         v0.4s, v0.4s, v16.4s") \
        __ASM_EMIT("sub         %[count], %[count], #4") \
        __ASM_EMIT("add         %[" SRC "], %[" SRC "], #0x10") \
        /* 1x blocks */ \
        __ASM_EMIT("8:") \
        __ASM_EMIT("adds        %[count], %[count], #3") \
        __ASM_EMIT("b.lt        10f") \
        __ASM_EMIT("9:") \
        __ASM_EMIT("ld1r        {v16.4s}, [%[" SRC "]]") \
        __ASM_EMIT(MOD("v16.4s", "v16.4s")) \
        __ASM_EMIT(OP "         v0.4s, v0.4s, v16.4s") \
        __ASM_EMIT("subs        %[count], %[count], #1") \
        __ASM_EMIT("add         %[" SRC "], %[" SRC "], #0x04") \
        __ASM_EMIT("b.ge        9b") \
        /* end */ \
        __ASM_EMIT("10:") \
        __ASM_EMIT("ext         v1.16b, v0.16b, v0.16b, #8")    /* v0 = a0 a1 a2 a3, v1 = a2 a3 a0 a1 */ \
        __ASM_EMIT(OP "         v0.4s, v0.4s, v1.4s")           /* v0 = a0?a2 a1?a3 a0?a2 a1?a3 */ \
        __ASM_EMIT("ext         v1.16b, v0.16b, v0.16b, #4")    /* v1 = a1?a3 a0?a2 a1?a3 a0?a2 */ \
        __ASM_EMIT(OP "         %S[" DST "].4s, v0.4s, v1.4s")  /* v0 = a0?a1?a2?a3 */ \
        __ASM_EMIT("100:")


    float min(const float *src, size_t count)
    {
        float res = 0.0f;
        ARCH_AARCH64_ASM(
            OP_SEARCH_CORE("res", "src", "fmin", MOD_NONE)
            : [res] "=&w" (res), [src] "+r" (src),
              [count] "+r" (count)
            :
            : "cc",
              "v1",
              "v16", "v17", "v18", "v19",
              "v20", "v21", "v22", "v23"
        );
        return res;
    }

    float max(const float *src, size_t count)
    {
        float res = 0.0f;
        ARCH_AARCH64_ASM(
            OP_SEARCH_CORE("res", "src", "fmax", MOD_NONE)
            : [res] "=&w" (res), [src] "+r" (src),
              [count] "+r" (count)
            :
            : "cc",
              "v1",
              "v16", "v17", "v18", "v19",
              "v20", "v21", "v22", "v23"
        );
        return res;
    }

    float abs_min(const float *src, size_t count)
    {
        float res = 0.0f;
        ARCH_AARCH64_ASM(
            OP_SEARCH_CORE("res", "src", "fmin", MOD_ABS)
            : [res] "=&w" (res), [src] "+r" (src),
              [count] "+r" (count)
            :
            : "cc",
              "v1",
              "v16", "v17", "v18", "v19",
              "v20", "v21", "v22", "v23"
        );
        return res;
    }

    float abs_max(const float *src, size_t count)
    {
        float res = 0.0f;
        ARCH_AARCH64_ASM(
            OP_SEARCH_CORE("res", "src", "fmax", MOD_ABS)
            : [res] "=&w" (res), [src] "+r" (src),
              [count] "+r" (count)
            :
            : "cc",
              "v1",
              "v16", "v17", "v18", "v19",
              "v20", "v21", "v22", "v23"
        );
        return res;
    }

    #undef OP_SEARCH_CORE

    #define MINMAX_SEARCH_CORE(MIN, MAX, SRC, MOD) \
        __ASM_EMIT("eor         v0.16b, v0.16b, v0.16b") \
        __ASM_EMIT("eor         v1.16b, v1.16b, v1.16b") \
        __ASM_EMIT("cmp         %[count], #1") \
        __ASM_EMIT("b.lo        100f") \
        /* 32x blocks */ \
        __ASM_EMIT("ld1r        {v0.4s}, [%[" SRC "]]") \
        __ASM_EMIT(MOD("v0.4s", "v0.4s")) \
        __ASM_EMIT("subs        %[count], %[count], #32") \
        __ASM_EMIT("mov         v1.16b, v0.16b") \
        __ASM_EMIT("mov         v2.16b, v0.16b") \
        __ASM_EMIT("mov         v3.16b, v1.16b") \
        __ASM_EMIT("b.lt        2f") \
        __ASM_EMIT("1:") \
        __ASM_EMIT("ldp         q16, q17, [%[" SRC "], #0x00]") \
        __ASM_EMIT("ldp         q18, q19, [%[" SRC "], #0x20]") \
        __ASM_EMIT("ldp         q20, q21, [%[" SRC "], #0x40]") \
        __ASM_EMIT("ldp         q22, q23, [%[" SRC "], #0x60]") \
        __ASM_EMIT(MOD("v16.4s", "v16.4s")) \
        __ASM_EMIT(MOD("v17.4s", "v17.4s")) \
        __ASM_EMIT(MOD("v18.4s", "v18.4s")) \
        __ASM_EMIT(MOD("v19.4s", "v19.4s")) \
        __ASM_EMIT(MOD("v20.4s", "v20.4s")) \
        __ASM_EMIT(MOD("v21.4s", "v21.4s")) \
        __ASM_EMIT(MOD("v22.4s", "v22.4s")) \
        __ASM_EMIT(MOD("v23.4s", "v23.4s")) \
        __ASM_EMIT("fmin        v0.4s, v0.4s, v16.4s") \
        __ASM_EMIT("fmin        v2.4s, v2.4s, v17.4s") \
        __ASM_EMIT("fmax        v1.4s, v1.4s, v16.4s") \
        __ASM_EMIT("fmax        v3.4s, v3.4s, v17.4s") \
        __ASM_EMIT("fmin        v0.4s, v0.4s, v18.4s") \
        __ASM_EMIT("fmin        v2.4s, v2.4s, v19.4s") \
        __ASM_EMIT("fmax        v1.4s, v1.4s, v18.4s") \
        __ASM_EMIT("fmax        v3.4s, v3.4s, v19.4s") \
        __ASM_EMIT("fmin        v0.4s, v0.4s, v20.4s") \
        __ASM_EMIT("fmin        v2.4s, v2.4s, v21.4s") \
        __ASM_EMIT("fmax        v1.4s, v1.4s, v20.4s") \
        __ASM_EMIT("fmax        v3.4s, v3.4s, v21.4s") \
        __ASM_EMIT("fmin        v0.4s, v0.4s, v22.4s") \
        __ASM_EMIT("fmin        v2.4s, v2.4s, v23.4s") \
        __ASM_EMIT("fmax        v1.4s, v1.4s, v22.4s") \
        __ASM_EMIT("fmax        v3.4s, v3.4s, v23.4s") \
        __ASM_EMIT("subs        %[count], %[count], #32") \
        __ASM_EMIT("add         %[" SRC "], %[" SRC "], #0x80") \
        __ASM_EMIT("b.hs        1b") \
        /* 16x block */ \
        __ASM_EMIT("2:") \
        __ASM_EMIT("adds        %[count], %[count], #16") \
        __ASM_EMIT("b.lt        4f") \
        __ASM_EMIT("ldp         q16, q17, [%[" SRC "], #0x00]") \
        __ASM_EMIT("ldp         q18, q19, [%[" SRC "], #0x20]") \
        __ASM_EMIT(MOD("v16.4s", "v16.4s")) \
        __ASM_EMIT(MOD("v17.4s", "v17.4s")) \
        __ASM_EMIT(MOD("v18.4s", "v18.4s")) \
        __ASM_EMIT(MOD("v19.4s", "v19.4s")) \
        __ASM_EMIT("fmin        v0.4s, v0.4s, v16.4s") \
        __ASM_EMIT("fmin        v2.4s, v2.4s, v17.4s") \
        __ASM_EMIT("fmax        v1.4s, v1.4s, v16.4s") \
        __ASM_EMIT("fmax        v3.4s, v3.4s, v17.4s") \
        __ASM_EMIT("fmin        v0.4s, v0.4s, v18.4s") \
        __ASM_EMIT("fmin        v2.4s, v2.4s, v19.4s") \
        __ASM_EMIT("fmax        v1.4s, v1.4s, v18.4s") \
        __ASM_EMIT("fmax        v3.4s, v3.4s, v19.4s") \
        __ASM_EMIT("sub         %[count], %[count], #16") \
        __ASM_EMIT("add         %[" SRC "], %[" SRC "], #0x40") \
        /* 8x block */ \
        __ASM_EMIT("4:") \
        __ASM_EMIT("adds        %[count], %[count], #8") \
        __ASM_EMIT("b.lt        6f") \
        __ASM_EMIT("ldp         q16, q17, [%[" SRC "], #0x00]") \
        __ASM_EMIT(MOD("v16.4s", "v16.4s")) \
        __ASM_EMIT(MOD("v17.4s", "v17.4s")) \
        __ASM_EMIT("fmin        v0.4s, v0.4s, v16.4s") \
        __ASM_EMIT("fmin        v2.4s, v2.4s, v17.4s") \
        __ASM_EMIT("fmax        v1.4s, v1.4s, v16.4s") \
        __ASM_EMIT("fmax        v3.4s, v3.4s, v17.4s") \
        __ASM_EMIT("sub         %[count], %[count], #8") \
        __ASM_EMIT("add         %[" SRC "], %[" SRC "], #0x20") \
        /* 4x block */ \
        __ASM_EMIT("6:") \
        __ASM_EMIT("fmin        v0.4s, v0.4s, v2.4s") \
        __ASM_EMIT("fmax        v1.4s, v1.4s, v3.4s") \
        __ASM_EMIT("adds        %[count], %[count], #4") \
        __ASM_EMIT("b.lt        8f") \
        __ASM_EMIT("ldr         q16, [%[" SRC "], #0x00]") \
        __ASM_EMIT(MOD("v16.4s", "v16.4s")) \
        __ASM_EMIT("fmin        v0.4s, v0.4s, v16.4s") \
        __ASM_EMIT("fmax        v1.4s, v1.4s, v16.4s") \
        __ASM_EMIT("sub         %[count], %[count], #4") \
        __ASM_EMIT("add         %[" SRC "], %[" SRC "], #0x10") \
        /* 1x blocks */ \
        __ASM_EMIT("8:") \
        __ASM_EMIT("adds        %[count], %[count], #3") \
        __ASM_EMIT("b.lt        10f") \
        __ASM_EMIT("9:") \
        __ASM_EMIT("ld1r        {v16.4s}, [%[" SRC "]]") \
        __ASM_EMIT(MOD("v16.4s", "v16.4s")) \
        __ASM_EMIT("fmin        v0.4s, v0.4s, v16.4s") \
        __ASM_EMIT("fmax        v1.4s, v1.4s, v16.4s") \
        __ASM_EMIT("subs        %[count], %[count], #1") \
        __ASM_EMIT("add         %[" SRC "], %[" SRC "], #0x04") \
        __ASM_EMIT("b.ge        9b") \
        /* end */ \
        __ASM_EMIT("10:") \
        __ASM_EMIT("ext         v2.16b, v0.16b, v0.16b, #8")    /* v0 = a0 a1 a2 a3, v2 = a2 a3 a0 a1 */ \
        __ASM_EMIT("ext         v3.16b, v1.16b, v1.16b, #8")    /* v1 = b0 b1 b2 b3, v3 = b2 b3 b0 b1 */ \
        __ASM_EMIT("fmin        v0.4s, v0.4s, v2.4s")           /* v0 = a0?a2 a1?a3 a0?a2 a1?a3 */ \
        __ASM_EMIT("fmax        v1.4s, v1.4s, v3.4s")           /* v1 = b0?b2 b1?b3 b0?b2 b1?b3 */ \
        __ASM_EMIT("ext         v2.16b, v0.16b, v0.16b, #4")    /* v2 = a1?a3 a0?a2 a1?a3 a0?a2 */ \
        __ASM_EMIT("ext         v3.16b, v1.16b, v1.16b, #4")    /* v3 = b1?b3 b0?b2 b1?b3 b0?b2 */ \
        __ASM_EMIT("fmin        v0.4s, v0.4s, v2.4s")           /* v0 = a0?a1?a2?a3 */ \
        __ASM_EMIT("fmax        v1.4s, v1.4s, v3.4s")           /* v1 = b0?b1?b2?b3 */ \
        __ASM_EMIT("100:") \
        /* store */ \
        __ASM_EMIT("st1         {v0.s}[0], [%[" MIN "]]") \
        __ASM_EMIT("st1         {v1.s}[0], [%[" MAX "]]")

    void minmax(const float *src, size_t count, float *min, float *max)
    {
        ARCH_AARCH64_ASM(
            MINMAX_SEARCH_CORE("min", "max", "src", MOD_NONE)
            : [src] "+r" (src), [count] "+r" (count)
            : [min] "r" (min), [max] "r" (max)
            : "cc",
              "v0", "v1", "v2", "v3",
              "v16", "v17", "v18", "v19",
              "v20", "v21", "v22", "v23"
        );
    }

    void abs_minmax(const float *src, size_t count, float *min, float *max)
    {
        ARCH_AARCH64_ASM(
            MINMAX_SEARCH_CORE("min", "max", "src", MOD_ABS)
            : [src] "+r" (src), [count] "+r" (count)
            : [min] "r" (min), [max] "r" (max)
            : "cc",
              "v0", "v1", "v2", "v3",
              "v16", "v17", "v18", "v19",
              "v20", "v21", "v22", "v23"
        );
    }

#undef MINMAX_SEARCH_CORE
#undef MOD_NONE
#undef MOD_ABS
}

#endif /* DSP_ARCH_AARCH64_ASIMD_SEARCH_MINMAX_H_ */
