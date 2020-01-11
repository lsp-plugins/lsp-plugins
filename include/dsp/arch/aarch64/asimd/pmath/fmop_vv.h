/*
 * fmop_vv.h
 *
 *  Created on: 23 нояб. 2019 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_AARCH64_ASIMD_PMATH_FMOP_VV_H_
#define DSP_ARCH_AARCH64_ASIMD_PMATH_FMOP_VV_H_

#ifndef DSP_ARCH_AARCH64_ASIMD_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_AARCH64_ASIMD_IMPL */

namespace asimd
{
#define OP_DSEL(a, b)       a
#define OP_RSEL(a, b)       b

#define FMADDSUB_V3_CORE(DST, A, B, OP) \
    __ASM_EMIT("subs        %[count], %[count], #32") \
    __ASM_EMIT("blo         2f") \
    /* 32x blocks */ \
    __ASM_EMIT("1:") \
    __ASM_EMIT("ldp         q0, q1, [%[" DST "], #0x00]") \
    __ASM_EMIT("ldp         q2, q3, [%[" DST "], #0x20]") \
    __ASM_EMIT("ldp         q4, q5, [%[" DST "], #0x40]") \
    __ASM_EMIT("ldp         q6, q7, [%[" DST "], #0x60]") \
    __ASM_EMIT("ldp         q16, q17, [%[" A "], #0x00]") \
    __ASM_EMIT("ldp         q18, q19, [%[" A "], #0x20]") \
    __ASM_EMIT("ldp         q20, q21, [%[" A "], #0x40]") \
    __ASM_EMIT("ldp         q22, q23, [%[" A "], #0x60]") \
    __ASM_EMIT("ldp         q24, q25, [%[" B "], #0x00]") \
    __ASM_EMIT("ldp         q26, q27, [%[" B "], #0x20]") \
    __ASM_EMIT("ldp         q28, q29, [%[" B "], #0x40]") \
    __ASM_EMIT("ldp         q30, q31, [%[" B "], #0x60]") \
    __ASM_EMIT(OP "         v0.4s, v16.4s, v24.4s") \
    __ASM_EMIT(OP "         v1.4s, v17.4s, v25.4s") \
    __ASM_EMIT(OP "         v2.4s, v18.4s, v26.4s") \
    __ASM_EMIT(OP "         v3.4s, v19.4s, v27.4s") \
    __ASM_EMIT(OP "         v4.4s, v20.4s, v28.4s") \
    __ASM_EMIT(OP "         v5.4s, v21.4s, v29.4s") \
    __ASM_EMIT(OP "         v6.4s, v22.4s, v30.4s") \
    __ASM_EMIT(OP "         v7.4s, v23.4s, v31.4s") \
    __ASM_EMIT("stp         q0, q1, [%[" DST "], #0x00]") \
    __ASM_EMIT("stp         q2, q3, [%[" DST "], #0x20]") \
    __ASM_EMIT("stp         q4, q5, [%[" DST "], #0x40]") \
    __ASM_EMIT("stp         q6, q7, [%[" DST "], #0x60]") \
    __ASM_EMIT("subs        %[count], %[count], #32") \
    __ASM_EMIT("add         %[" DST "], %[" DST "], #0x80") \
    __ASM_EMIT("add         %[" A "], %[" A "], #0x80") \
    __ASM_EMIT("add         %[" B "], %[" B "], #0x80") \
    __ASM_EMIT("bhs         1b") \
    /* 16x block */ \
    __ASM_EMIT("2:") \
    __ASM_EMIT("adds        %[count], %[count], #16") /* 32-16 */ \
    __ASM_EMIT("blt         4f") \
    __ASM_EMIT("ldp         q0, q1, [%[" DST "], #0x00]") \
    __ASM_EMIT("ldp         q2, q3, [%[" DST "], #0x20]") \
    __ASM_EMIT("ldp         q16, q17, [%[" A "], #0x00]") \
    __ASM_EMIT("ldp         q18, q19, [%[" A "], #0x20]") \
    __ASM_EMIT("ldp         q24, q25, [%[" B "], #0x00]") \
    __ASM_EMIT("ldp         q26, q27, [%[" B "], #0x20]") \
    __ASM_EMIT(OP "         v0.4s, v16.4s, v24.4s") \
    __ASM_EMIT(OP "         v1.4s, v17.4s, v25.4s") \
    __ASM_EMIT(OP "         v2.4s, v18.4s, v26.4s") \
    __ASM_EMIT(OP "         v3.4s, v19.4s, v27.4s") \
    __ASM_EMIT("stp         q0, q1, [%[" DST "], #0x00]") \
    __ASM_EMIT("stp         q2, q3, [%[" DST "], #0x20]") \
    __ASM_EMIT("sub         %[count], %[count], #16") \
    __ASM_EMIT("add         %[" DST "], %[" DST "], #0x40") \
    __ASM_EMIT("add         %[" A "], %[" A "], #0x40") \
    __ASM_EMIT("add         %[" B "], %[" B "], #0x40") \
    /* 8x block */ \
    __ASM_EMIT("4:") \
    __ASM_EMIT("adds        %[count], %[count], #8") /* 16-8 */ \
    __ASM_EMIT("blt         6f") \
    __ASM_EMIT("ldp         q0, q1, [%[" DST "], #0x00]") \
    __ASM_EMIT("ldp         q16, q17, [%[" A "], #0x00]") \
    __ASM_EMIT("ldp         q24, q25, [%[" B "], #0x00]") \
    __ASM_EMIT(OP "         v0.4s, v16.4s, v24.4s") \
    __ASM_EMIT(OP "         v1.4s, v17.4s, v25.4s") \
    __ASM_EMIT("stp         q0, q1, [%[" DST "], #0x00]") \
    __ASM_EMIT("sub         %[count], %[count], #8") \
    __ASM_EMIT("add         %[" DST "], %[" DST "], #0x20") \
    __ASM_EMIT("add         %[" A "], %[" A "], #0x20") \
    __ASM_EMIT("add         %[" B "], %[" B "], #0x20") \
    /* 4x block */ \
    __ASM_EMIT("6:") \
    __ASM_EMIT("adds        %[count], %[count], #4") /* 8 - 4 */ \
    __ASM_EMIT("blt         8f") \
    __ASM_EMIT("ldr         q0, [%[" DST "], #0x00]") \
    __ASM_EMIT("ldr         q16, [%[" A "], #0x00]") \
    __ASM_EMIT("ldr         q24, [%[" B "], #0x00]") \
    __ASM_EMIT(OP "         v0.4s, v16.4s, v24.4s") \
    __ASM_EMIT("str         q0, [%[" DST "], #0x00]") \
    __ASM_EMIT("sub         %[count], %[count], #4") \
    __ASM_EMIT("add         %[" DST "], %[" DST "], #0x10") \
    __ASM_EMIT("add         %[" A "], %[" A "], #0x10") \
    __ASM_EMIT("add         %[" B "], %[" B "], #0x10") \
    /* 1x block */ \
    __ASM_EMIT("8:") \
    __ASM_EMIT("adds        %[count], %[count], #3") /* 4 - 3 */ \
    __ASM_EMIT("blt         10f") \
    __ASM_EMIT("9:") \
    __ASM_EMIT("ld1r        {v0.4s}, [%[" DST "]]") \
    __ASM_EMIT("ld1r        {v16.4s}, [%[" A "]]") \
    __ASM_EMIT("ld1r        {v24.4s}, [%[" B "]]") \
    __ASM_EMIT(OP "         v0.4s, v16.4s, v24.4s") \
    __ASM_EMIT("st1         {v0.s}[0], [%[" DST "]]") \
    __ASM_EMIT("subs        %[count], %[count], #1") \
    __ASM_EMIT("add         %[" DST "], %[" DST "], #0x04") \
    __ASM_EMIT("add         %[" A "], %[" A "], #0x04") \
    __ASM_EMIT("add         %[" B "], %[" B "], #0x04") \
    __ASM_EMIT("bge         9b") \
    __ASM_EMIT("10:")

     void fmadd3(float *dst, const float *a, const float *b, size_t count)
     {
         ARCH_AARCH64_ASM
         (
             FMADDSUB_V3_CORE("dst", "a", "b", "fmla")
             : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b),
               [count] "+r" (count)
             :
             : "cc", "memory",
               "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
               "q16", "q17", "q18", "q19", "q20", "q21", "q22", "q23",
               "q24", "q25", "q26", "q27", "q28", "q29", "q30", "q31"
         );
     }

     void fmsub3(float *dst, const float *a, const float *b, size_t count)
     {
         ARCH_AARCH64_ASM
         (
             FMADDSUB_V3_CORE("dst", "a", "b", "fmls")
             : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b),
               [count] "+r" (count)
             :
             : "cc", "memory",
               "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
               "q16", "q17", "q18", "q19", "q20", "q21", "q22", "q23",
               "q24", "q25", "q26", "q27", "q28", "q29", "q30", "q31"
         );
     }

#undef FMADDSUB_V3_CORE

#define FMOP_V3_CORE(DST, A, B, OP, SEL) \
    __ASM_EMIT("subs        %[count], %[count], #32") \
    __ASM_EMIT("blo         2f") \
    /* 32x blocks */ \
    __ASM_EMIT("1:") \
    __ASM_EMIT("ldp         q16, q17, [%[" A "], #0x00]") \
    __ASM_EMIT("ldp         q18, q19, [%[" A "], #0x20]") \
    __ASM_EMIT("ldp         q20, q21, [%[" A "], #0x40]") \
    __ASM_EMIT("ldp         q22, q23, [%[" A "], #0x60]") \
    __ASM_EMIT("ldp         q24, q25, [%[" B "], #0x00]") \
    __ASM_EMIT("ldp         q26, q27, [%[" B "], #0x20]") \
    __ASM_EMIT("ldp         q28, q29, [%[" B "], #0x40]") \
    __ASM_EMIT("ldp         q30, q31, [%[" B "], #0x60]") \
    __ASM_EMIT("ldp         q0, q1, [%[" DST "], #0x00]") \
    __ASM_EMIT("ldp         q2, q3, [%[" DST "], #0x20]") \
    __ASM_EMIT("ldp         q4, q5, [%[" DST "], #0x40]") \
    __ASM_EMIT("ldp         q6, q7, [%[" DST "], #0x60]") \
    __ASM_EMIT("fmul        v16.4s, v16.4s, v24.4s") \
    __ASM_EMIT("fmul        v17.4s, v17.4s, v25.4s") \
    __ASM_EMIT("fmul        v18.4s, v18.4s, v26.4s") \
    __ASM_EMIT("fmul        v19.4s, v19.4s, v27.4s") \
    __ASM_EMIT("fmul        v20.4s, v20.4s, v28.4s") \
    __ASM_EMIT("fmul        v21.4s, v21.4s, v29.4s") \
    __ASM_EMIT("fmul        v22.4s, v22.4s, v30.4s") \
    __ASM_EMIT("fmul        v23.4s, v23.4s, v31.4s") \
    __ASM_EMIT(OP "         v0.4s, " SEL("v0.4s", "v16.4s") ", " SEL("v16.4s", "v0.4s")) \
    __ASM_EMIT(OP "         v1.4s, " SEL("v1.4s", "v17.4s") ", " SEL("v17.4s", "v1.4s")) \
    __ASM_EMIT(OP "         v2.4s, " SEL("v2.4s", "v18.4s") ", " SEL("v18.4s", "v2.4s")) \
    __ASM_EMIT(OP "         v3.4s, " SEL("v3.4s", "v19.4s") ", " SEL("v19.4s", "v3.4s")) \
    __ASM_EMIT(OP "         v4.4s, " SEL("v4.4s", "v20.4s") ", " SEL("v20.4s", "v4.4s")) \
    __ASM_EMIT(OP "         v5.4s, " SEL("v5.4s", "v21.4s") ", " SEL("v21.4s", "v5.4s")) \
    __ASM_EMIT(OP "         v6.4s, " SEL("v6.4s", "v22.4s") ", " SEL("v22.4s", "v6.4s")) \
    __ASM_EMIT(OP "         v7.4s, " SEL("v7.4s", "v23.4s") ", " SEL("v23.4s", "v7.4s")) \
    __ASM_EMIT("stp         q0, q1, [%[" DST "], #0x00]") \
    __ASM_EMIT("stp         q2, q3, [%[" DST "], #0x20]") \
    __ASM_EMIT("stp         q4, q5, [%[" DST "], #0x40]") \
    __ASM_EMIT("stp         q6, q7, [%[" DST "], #0x60]") \
    __ASM_EMIT("subs        %[count], %[count], #32") \
    __ASM_EMIT("add         %[" DST "], %[" DST "], #0x80") \
    __ASM_EMIT("add         %[" A "], %[" A "], #0x80") \
    __ASM_EMIT("add         %[" B "], %[" B "], #0x80") \
    __ASM_EMIT("bhs         1b") \
    /* 16x block */ \
    __ASM_EMIT("2:") \
    __ASM_EMIT("adds        %[count], %[count], #16") /* 32-16 */ \
    __ASM_EMIT("blt         4f") \
    __ASM_EMIT("ldp         q16, q17, [%[" A "], #0x00]") \
    __ASM_EMIT("ldp         q18, q19, [%[" A "], #0x20]") \
    __ASM_EMIT("ldp         q24, q25, [%[" B "], #0x00]") \
    __ASM_EMIT("ldp         q26, q27, [%[" B "], #0x20]") \
    __ASM_EMIT("ldp         q0, q1, [%[" DST "], #0x00]") \
    __ASM_EMIT("ldp         q2, q3, [%[" DST "], #0x20]") \
    __ASM_EMIT("fmul        v16.4s, v16.4s, v24.4s") \
    __ASM_EMIT("fmul        v17.4s, v17.4s, v25.4s") \
    __ASM_EMIT("fmul        v18.4s, v18.4s, v26.4s") \
    __ASM_EMIT("fmul        v19.4s, v19.4s, v27.4s") \
    __ASM_EMIT(OP "         v0.4s, " SEL("v0.4s", "v16.4s") ", " SEL("v16.4s", "v0.4s")) \
    __ASM_EMIT(OP "         v1.4s, " SEL("v1.4s", "v17.4s") ", " SEL("v17.4s", "v1.4s")) \
    __ASM_EMIT(OP "         v2.4s, " SEL("v2.4s", "v18.4s") ", " SEL("v18.4s", "v2.4s")) \
    __ASM_EMIT(OP "         v3.4s, " SEL("v3.4s", "v19.4s") ", " SEL("v19.4s", "v3.4s")) \
    __ASM_EMIT("stp         q0, q1, [%[" DST "], #0x00]") \
    __ASM_EMIT("stp         q2, q3, [%[" DST "], #0x20]") \
    __ASM_EMIT("sub         %[count], %[count], #16") \
    __ASM_EMIT("add         %[" DST "], %[" DST "], #0x40") \
    __ASM_EMIT("add         %[" A "], %[" A "], #0x40") \
    __ASM_EMIT("add         %[" B "], %[" B "], #0x40") \
    /* 8x block */ \
    __ASM_EMIT("4:") \
    __ASM_EMIT("adds        %[count], %[count], #8") /* 16-8 */ \
    __ASM_EMIT("blt         6f") \
    __ASM_EMIT("ldp         q16, q17, [%[" A "], #0x00]") \
    __ASM_EMIT("ldp         q24, q25, [%[" B "], #0x00]") \
    __ASM_EMIT("ldp         q0, q1, [%[" DST "], #0x00]") \
    __ASM_EMIT("fmul        v16.4s, v16.4s, v24.4s") \
    __ASM_EMIT("fmul        v17.4s, v17.4s, v25.4s") \
    __ASM_EMIT(OP "         v0.4s, " SEL("v0.4s", "v16.4s") ", " SEL("v16.4s", "v0.4s")) \
    __ASM_EMIT(OP "         v1.4s, " SEL("v1.4s", "v17.4s") ", " SEL("v17.4s", "v1.4s")) \
    __ASM_EMIT("stp         q0, q1, [%[" DST "], #0x00]") \
    __ASM_EMIT("sub         %[count], %[count], #8") \
    __ASM_EMIT("add         %[" DST "], %[" DST "], #0x20") \
    __ASM_EMIT("add         %[" A "], %[" A "], #0x20") \
    __ASM_EMIT("add         %[" B "], %[" B "], #0x20") \
    /* 4x block */ \
    __ASM_EMIT("6:") \
    __ASM_EMIT("adds        %[count], %[count], #4") /* 8 - 4 */ \
    __ASM_EMIT("blt         8f") \
    __ASM_EMIT("ldr         q16, [%[" A "], #0x00]") \
    __ASM_EMIT("ldr         q24, [%[" B "], #0x00]") \
    __ASM_EMIT("ldr         q0, [%[" DST "], #0x00]") \
    __ASM_EMIT("fmul        v16.4s, v16.4s, v24.4s") \
    __ASM_EMIT(OP "         v0.4s, " SEL("v0.4s", "v16.4s") ", " SEL("v16.4s", "v0.4s")) \
    __ASM_EMIT("str         q0, [%[" DST "], #0x00]") \
    __ASM_EMIT("sub         %[count], %[count], #4") \
    __ASM_EMIT("add         %[" DST "], %[" DST "], #0x10") \
    __ASM_EMIT("add         %[" A "], %[" A "], #0x10") \
    __ASM_EMIT("add         %[" B "], %[" B "], #0x10") \
    /* 1x block */ \
    __ASM_EMIT("8:") \
    __ASM_EMIT("adds        %[count], %[count], #3") /* 4 - 3 */ \
    __ASM_EMIT("blt         10f") \
    __ASM_EMIT("9:") \
    __ASM_EMIT("ld1r        {v16.4s}, [%[" A "]]") \
    __ASM_EMIT("ld1r        {v24.4s}, [%[" B "]]") \
    __ASM_EMIT("ld1r        {v0.4s}, [%[" DST "]]") \
    __ASM_EMIT("fmul        v16.4s, v16.4s, v24.4s") \
    __ASM_EMIT(OP "         v0.4s, " SEL("v0.4s", "v16.4s") ", " SEL("v16.4s", "v0.4s")) \
    __ASM_EMIT("st1         {v0.s}[0], [%[" DST "]]") \
    __ASM_EMIT("subs        %[count], %[count], #1") \
    __ASM_EMIT("add         %[" DST "], %[" DST "], #0x04") \
    __ASM_EMIT("add         %[" A "], %[" A "], #0x04") \
    __ASM_EMIT("add         %[" B "], %[" B "], #0x04") \
    __ASM_EMIT("bge         9b") \
    __ASM_EMIT("10:")

    void fmmul3(float *dst, const float *a, const float *b, size_t count)
    {
        ARCH_AARCH64_ASM
        (
            FMOP_V3_CORE("dst", "a", "b", "fmul", OP_DSEL)
            : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q16", "q17", "q18", "q19", "q20", "q21", "q22", "q23",
              "q24", "q25", "q26", "q27", "q28", "q29", "q30", "q31"
        );
    }

    void fmrsub3(float *dst, const float *a, const float *b, size_t count)
    {
        ARCH_AARCH64_ASM
        (
            FMOP_V3_CORE("dst", "a", "b", "fsub", OP_RSEL)
            : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q16", "q17", "q18", "q19", "q20", "q21", "q22", "q23",
              "q24", "q25", "q26", "q27", "q28", "q29", "q30", "q31"
        );
    }

#undef FMOP_V3_CORE

#define FMDIV_V3_CORE(DST, A, B, SEL) \
    __ASM_EMIT("subs        %[count], %[count], #16") \
    __ASM_EMIT("mov         v21.16b, v20.16b") \
    __ASM_EMIT("blo         2f") \
    /* 16x blocks */ \
    __ASM_EMIT("1:") \
    __ASM_EMIT("ldp         q0, q1, [%[" SEL(A, DST) "], #0x00]") \
    __ASM_EMIT("ldp         q2, q3, [%[" SEL(A, DST) "], #0x20]") \
    __ASM_EMIT("ldp         q4, q5, [%[" SEL(DST, A) "], #0x00]") \
    __ASM_EMIT("ldp         q6, q7, [%[" SEL(DST, A) "], #0x20]") \
    __ASM_EMIT("ldp         q16, q17, [%[" B "], #0x00]") \
    __ASM_EMIT("ldp         q18, q19, [%[" B "], #0x20]") \
    __ASM_EMIT("fmul      " SEL("v0.4s, v0.4s", "v4.4s, v4.4s") ", v16.4s") /* {v0, v4} = {A*B, DST} | {DST, A*B} */ \
    __ASM_EMIT("fmul      " SEL("v1.4s, v1.4s", "v5.4s, v5.4s") ", v17.4s") \
    __ASM_EMIT("fmul      " SEL("v2.4s, v2.4s", "v6.4s, v6.4s") ", v18.4s") \
    __ASM_EMIT("fmul      " SEL("v3.4s, v3.4s", "v7.4s, v7.4s") ", v19.4s") \
    __ASM_EMIT("frecpe      v16.4s, v0.4s")                     /* v16 = s2 */ \
    __ASM_EMIT("frecpe      v17.4s, v1.4s") \
    __ASM_EMIT("frecpe      v18.4s, v2.4s") \
    __ASM_EMIT("frecpe      v19.4s, v3.4s") \
    __ASM_EMIT("frecps      v24.4s, v16.4s, v0.4s")             /* v24 = (2 - R*s2) */ \
    __ASM_EMIT("frecps      v25.4s, v17.4s, v1.4s") \
    __ASM_EMIT("frecps      v26.4s, v18.4s, v2.4s") \
    __ASM_EMIT("frecps      v27.4s, v19.4s, v3.4s") \
    __ASM_EMIT("fmul        v16.4s, v24.4s, v16.4s")            /* v16 = s2' = s2 * (2 - R*s2) */ \
    __ASM_EMIT("fmul        v17.4s, v25.4s, v17.4s") \
    __ASM_EMIT("fmul        v18.4s, v26.4s, v18.4s") \
    __ASM_EMIT("fmul        v19.4s, v27.4s, v19.4s") \
    __ASM_EMIT("frecps      v24.4s, v16.4s, v0.4s")             /* v24 = (2 - R*s2') */ \
    __ASM_EMIT("frecps      v25.4s, v17.4s, v1.4s") \
    __ASM_EMIT("frecps      v26.4s, v18.4s, v2.4s") \
    __ASM_EMIT("frecps      v27.4s, v19.4s, v3.4s") \
    __ASM_EMIT("fmul        v0.4s, v24.4s, v16.4s")             /* v0  = s2" = s2' * (2 - R*s2) = 1/s2 = 1/(A*B) | 1/DST */  \
    __ASM_EMIT("fmul        v1.4s, v25.4s, v17.4s") \
    __ASM_EMIT("fmul        v2.4s, v26.4s, v18.4s") \
    __ASM_EMIT("fmul        v3.4s, v27.4s, v19.4s") \
    __ASM_EMIT("fmul        v0.4s, v0.4s, v4.4s")               /* DST/(A*B) | (A*B)/DST */ \
    __ASM_EMIT("fmul        v1.4s, v1.4s, v5.4s") \
    __ASM_EMIT("fmul        v2.4s, v2.4s, v6.4s") \
    __ASM_EMIT("fmul        v3.4s, v3.4s, v7.4s") \
    __ASM_EMIT("stp         q0, q1, [%[" DST "], #0x00]") \
    __ASM_EMIT("stp         q2, q3, [%[" DST "], #0x20]") \
    __ASM_EMIT("subs        %[count], %[count], #16") \
    __ASM_EMIT("add         %[" DST "], %[" DST "], #0x40") \
    __ASM_EMIT("add         %[" A "], %[" A "], #0x40") \
    __ASM_EMIT("add         %[" B "], %[" B "], #0x40") \
    __ASM_EMIT("bhs         1b") \
    /* 8x block */ \
    __ASM_EMIT("2:") \
    __ASM_EMIT("adds        %[count], %[count], #8")            /* 16 - 8 */ \
    __ASM_EMIT("blt         4f") \
    __ASM_EMIT("ldp         q0, q1, [%[" SEL(A, DST) "], #0x00]") \
    __ASM_EMIT("ldp         q4, q5, [%[" SEL(DST, A) "], #0x00]") \
    __ASM_EMIT("ldp         q16, q17, [%[" B "], #0x00]") \
    __ASM_EMIT("fmul      " SEL("v0.4s, v0.4s", "v4.4s, v4.4s") ", v16.4s") /* {v0, v4} = {A*B, DST} | {DST, A*B} */ \
    __ASM_EMIT("fmul      " SEL("v1.4s, v1.4s", "v5.4s, v5.4s") ", v17.4s") \
    __ASM_EMIT("frecpe      v16.4s, v0.4s")                     /* v16 = s2 */ \
    __ASM_EMIT("frecpe      v17.4s, v1.4s") \
    __ASM_EMIT("frecps      v24.4s, v16.4s, v0.4s")             /* v24 = (2 - R*s2) */ \
    __ASM_EMIT("frecps      v25.4s, v17.4s, v1.4s") \
    __ASM_EMIT("fmul        v16.4s, v24.4s, v16.4s")            /* v16 = s2' = s2 * (2 - R*s2) */ \
    __ASM_EMIT("fmul        v17.4s, v25.4s, v17.4s") \
    __ASM_EMIT("frecps      v24.4s, v16.4s, v0.4s")             /* v24 = (2 - R*s2') */ \
    __ASM_EMIT("frecps      v25.4s, v17.4s, v1.4s") \
    __ASM_EMIT("fmul        v0.4s, v24.4s, v16.4s")             /* v0  = s2" = s2' * (2 - R*s2) = 1/s2 = 1/(A*B) | 1/DST */  \
    __ASM_EMIT("fmul        v1.4s, v25.4s, v17.4s") \
    __ASM_EMIT("fmul        v0.4s, v0.4s, v4.4s")               /* DST/(A*B) | (A*B)/DST */ \
    __ASM_EMIT("fmul        v1.4s, v1.4s, v5.4s") \
    __ASM_EMIT("stp         q0, q1, [%[" DST "], #0x00]") \
    __ASM_EMIT("sub         %[count], %[count], #8") \
    __ASM_EMIT("add         %[" DST "], %[" DST "], #0x20") \
    __ASM_EMIT("add         %[" A "], %[" A "], #0x20") \
    __ASM_EMIT("add         %[" B "], %[" B "], #0x20") \
    /* 4x block */ \
    __ASM_EMIT("4:") \
    __ASM_EMIT("adds        %[count], %[count], #4")            /* 8 - 4 */ \
    __ASM_EMIT("blt         6f") \
    __ASM_EMIT("ldr         q0, [%[" SEL(A, DST) "], #0x00]") \
    __ASM_EMIT("ldr         q4, [%[" SEL(DST, A) "], #0x00]") \
    __ASM_EMIT("ldr         q16, [%[" B "], #0x00]") \
    __ASM_EMIT("fmul      " SEL("v0.4s, v0.4s", "v4.4s, v4.4s") ", v16.4s") /* {v0, v4} = {A*B, DST} | {DST, A*B} */ \
    __ASM_EMIT("frecpe      v16.4s, v0.4s")                     /* v16 = s2 */ \
    __ASM_EMIT("frecps      v24.4s, v16.4s, v0.4s")             /* v24 = (2 - R*s2) */ \
    __ASM_EMIT("fmul        v16.4s, v24.4s, v16.4s")            /* v16 = s2' = s2 * (2 - R*s2) */ \
    __ASM_EMIT("frecps      v24.4s, v16.4s, v0.4s")             /* v24 = (2 - R*s2') */ \
    __ASM_EMIT("fmul        v0.4s, v24.4s, v16.4s")             /* v0  = s2" = s2' * (2 - R*s2) = 1/s2 = 1/(A*B) | 1/DST */  \
    __ASM_EMIT("fmul        v0.4s, v0.4s, v4.4s")               /* DST/(A*B) | (A*B)/DST */ \
    __ASM_EMIT("str         q0, [%[" DST "], #0x00]") \
    __ASM_EMIT("sub         %[count], %[count], #4") \
    __ASM_EMIT("add         %[" DST "], %[" DST "], #0x10") \
    __ASM_EMIT("add         %[" A "], %[" A "], #0x10") \
    __ASM_EMIT("add         %[" B "], %[" B "], #0x10") \
    /* 1x block */ \
    __ASM_EMIT("6:") \
    __ASM_EMIT("adds        %[count], %[count], #3")            /* 4 - 3 */ \
    __ASM_EMIT("blt         8f") \
    __ASM_EMIT("7:") \
    __ASM_EMIT("ld1r        {v0.4s}, [%[" SEL(A, DST) "]]") \
    __ASM_EMIT("ld1r        {v4.4s}, [%[" SEL(DST, A) "]]") \
    __ASM_EMIT("ld1r        {v16.4s}, [%[" B "]]") \
    __ASM_EMIT("fmul      " SEL("v0.4s, v0.4s", "v4.4s, v4.4s") ", v16.4s") /* {v0, v4} = {A*B, DST} | {DST, A*B} */ \
    __ASM_EMIT("frecpe      v16.4s, v0.4s")                     /* v16 = s2 */ \
    __ASM_EMIT("frecps      v24.4s, v16.4s, v0.4s")             /* v24 = (2 - R*s2) */ \
    __ASM_EMIT("fmul        v16.4s, v24.4s, v16.4s")            /* v16 = s2' = s2 * (2 - R*s2) */ \
    __ASM_EMIT("frecps      v24.4s, v16.4s, v0.4s")             /* v24 = (2 - R*s2') */ \
    __ASM_EMIT("fmul        v0.4s, v24.4s, v16.4s")             /* v0  = s2" = s2' * (2 - R*s2) = 1/s2 = 1/(A*B) | 1/DST */  \
    __ASM_EMIT("fmul        v0.4s, v0.4s, v4.4s")               /* DST/(A*B) | (A*B)/DST */ \
    __ASM_EMIT("st1         {v0.s}[0], [%[" DST "]]") \
    __ASM_EMIT("subs        %[count], %[count], #1") \
    __ASM_EMIT("add         %[" DST "], %[" DST "], #0x04") \
    __ASM_EMIT("add         %[" A "], %[" A "], #0x04") \
    __ASM_EMIT("add         %[" B "], %[" B "], #0x04") \
    __ASM_EMIT("bge         7b") \
    __ASM_EMIT("8:")

    void fmdiv3(float *dst, const float *a, const float *b, size_t count)
    {
        ARCH_AARCH64_ASM
        (
            FMDIV_V3_CORE("dst", "a", "b", OP_DSEL)
            : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
              "q16", "q17", "q18", "q19",
              "q24", "q25", "q26", "q27"
        );
    }

    void fmrdiv3(float *dst, const float *a, const float *b, size_t count)
    {
        ARCH_AARCH64_ASM
        (
            FMDIV_V3_CORE("dst", "a", "b", OP_RSEL)
            : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
              "q16", "q17", "q18", "q19",
              "q24", "q25", "q26", "q27"
        );
    }

#undef FMDIV_V3_CORE

#define FMADDSUB_V4_CORE(DST, A, B, C, OP) \
    __ASM_EMIT("subs        %[count], %[count], #32") \
    __ASM_EMIT("blo         2f") \
    /* 32x blocks */ \
    __ASM_EMIT("1:") \
    __ASM_EMIT("ldp         q0, q1, [%[" A "], #0x00]") \
    __ASM_EMIT("ldp         q2, q3, [%[" A "], #0x20]") \
    __ASM_EMIT("ldp         q4, q5, [%[" A "], #0x40]") \
    __ASM_EMIT("ldp         q6, q7, [%[" A "], #0x60]") \
    __ASM_EMIT("ldp         q16, q17, [%[" B "], #0x00]") \
    __ASM_EMIT("ldp         q18, q19, [%[" B "], #0x20]") \
    __ASM_EMIT("ldp         q20, q21, [%[" B "], #0x40]") \
    __ASM_EMIT("ldp         q22, q23, [%[" B "], #0x60]") \
    __ASM_EMIT("ldp         q24, q25, [%[" C "], #0x00]") \
    __ASM_EMIT("ldp         q26, q27, [%[" C "], #0x20]") \
    __ASM_EMIT("ldp         q28, q29, [%[" C "], #0x40]") \
    __ASM_EMIT("ldp         q30, q31, [%[" C "], #0x60]") \
    __ASM_EMIT(OP "         v0.4s, v16.4s, v24.4s") \
    __ASM_EMIT(OP "         v1.4s, v17.4s, v25.4s") \
    __ASM_EMIT(OP "         v2.4s, v18.4s, v26.4s") \
    __ASM_EMIT(OP "         v3.4s, v19.4s, v27.4s") \
    __ASM_EMIT(OP "         v4.4s, v20.4s, v28.4s") \
    __ASM_EMIT(OP "         v5.4s, v21.4s, v29.4s") \
    __ASM_EMIT(OP "         v6.4s, v22.4s, v30.4s") \
    __ASM_EMIT(OP "         v7.4s, v23.4s, v31.4s") \
    __ASM_EMIT("stp         q0, q1, [%[" DST "], #0x00]") \
    __ASM_EMIT("stp         q2, q3, [%[" DST "], #0x20]") \
    __ASM_EMIT("stp         q4, q5, [%[" DST "], #0x40]") \
    __ASM_EMIT("stp         q6, q7, [%[" DST "], #0x60]") \
    __ASM_EMIT("subs        %[count], %[count], #32") \
    __ASM_EMIT("add         %[" DST "], %[" DST "], #0x80") \
    __ASM_EMIT("add         %[" A "], %[" A "], #0x80") \
    __ASM_EMIT("add         %[" B "], %[" B "], #0x80") \
    __ASM_EMIT("add         %[" C "], %[" C "], #0x80") \
    __ASM_EMIT("bhs         1b") \
    /* 16x block */ \
    __ASM_EMIT("2:") \
    __ASM_EMIT("adds        %[count], %[count], #16") /* 32-16 */ \
    __ASM_EMIT("blt         4f") \
    __ASM_EMIT("ldp         q0, q1, [%[" A "], #0x00]") \
    __ASM_EMIT("ldp         q2, q3, [%[" A "], #0x20]") \
    __ASM_EMIT("ldp         q16, q17, [%[" B "], #0x00]") \
    __ASM_EMIT("ldp         q18, q19, [%[" B "], #0x20]") \
    __ASM_EMIT("ldp         q24, q25, [%[" C "], #0x00]") \
    __ASM_EMIT("ldp         q26, q27, [%[" C "], #0x20]") \
    __ASM_EMIT(OP "         v0.4s, v16.4s, v24.4s") \
    __ASM_EMIT(OP "         v1.4s, v17.4s, v25.4s") \
    __ASM_EMIT(OP "         v2.4s, v18.4s, v26.4s") \
    __ASM_EMIT(OP "         v3.4s, v19.4s, v27.4s") \
    __ASM_EMIT("stp         q0, q1, [%[" DST "], #0x00]") \
    __ASM_EMIT("stp         q2, q3, [%[" DST "], #0x20]") \
    __ASM_EMIT("sub         %[count], %[count], #16") \
    __ASM_EMIT("add         %[" DST "], %[" DST "], #0x40") \
    __ASM_EMIT("add         %[" A "], %[" A "], #0x40") \
    __ASM_EMIT("add         %[" B "], %[" B "], #0x40") \
    __ASM_EMIT("add         %[" C "], %[" C "], #0x40") \
    /* 8x block */ \
    __ASM_EMIT("4:") \
    __ASM_EMIT("adds        %[count], %[count], #8") /* 16-8 */ \
    __ASM_EMIT("blt         6f") \
    __ASM_EMIT("ldp         q0, q1, [%[" A "], #0x00]") \
    __ASM_EMIT("ldp         q16, q17, [%[" B "], #0x00]") \
    __ASM_EMIT("ldp         q24, q25, [%[" C "], #0x00]") \
    __ASM_EMIT(OP "         v0.4s, v16.4s, v24.4s") \
    __ASM_EMIT(OP "         v1.4s, v17.4s, v25.4s") \
    __ASM_EMIT("stp         q0, q1, [%[" DST "], #0x00]") \
    __ASM_EMIT("sub         %[count], %[count], #8") \
    __ASM_EMIT("add         %[" DST "], %[" DST "], #0x20") \
    __ASM_EMIT("add         %[" A "], %[" A "], #0x20") \
    __ASM_EMIT("add         %[" B "], %[" B "], #0x20") \
    __ASM_EMIT("add         %[" C "], %[" C "], #0x20") \
    /* 4x block */ \
    __ASM_EMIT("6:") \
    __ASM_EMIT("adds        %[count], %[count], #4") /* 8 - 4 */ \
    __ASM_EMIT("blt         8f") \
    __ASM_EMIT("ldr         q0, [%[" A "], #0x00]") \
    __ASM_EMIT("ldr         q16, [%[" B "], #0x00]") \
    __ASM_EMIT("ldr         q24, [%[" C "], #0x00]") \
    __ASM_EMIT(OP "         v0.4s, v16.4s, v24.4s") \
    __ASM_EMIT("str         q0, [%[" DST "], #0x00]") \
    __ASM_EMIT("sub         %[count], %[count], #4") \
    __ASM_EMIT("add         %[" DST "], %[" DST "], #0x10") \
    __ASM_EMIT("add         %[" A "], %[" A "], #0x10") \
    __ASM_EMIT("add         %[" B "], %[" B "], #0x10") \
    __ASM_EMIT("add         %[" C "], %[" C "], #0x10") \
    /* 1x block */ \
    __ASM_EMIT("8:") \
    __ASM_EMIT("adds        %[count], %[count], #3") /* 4 - 3 */ \
    __ASM_EMIT("blt         10f") \
    __ASM_EMIT("9:") \
    __ASM_EMIT("ld1r        {v0.4s}, [%[" A "]]") \
    __ASM_EMIT("ld1r        {v16.4s}, [%[" B "]]") \
    __ASM_EMIT("ld1r        {v24.4s}, [%[" C "]]") \
    __ASM_EMIT(OP "         v0.4s, v16.4s, v24.4s") \
    __ASM_EMIT("st1         {v0.s}[0], [%[" DST "]]") \
    __ASM_EMIT("subs        %[count], %[count], #1") \
    __ASM_EMIT("add         %[" DST "], %[" DST "], #0x04") \
    __ASM_EMIT("add         %[" A "], %[" A "], #0x04") \
    __ASM_EMIT("add         %[" B "], %[" B "], #0x04") \
    __ASM_EMIT("add         %[" C "], %[" C "], #0x04") \
    __ASM_EMIT("bge         9b") \
    __ASM_EMIT("10:")

     void fmadd4(float *dst, const float *a, const float *b, const float *c, size_t count)
     {
         ARCH_AARCH64_ASM
         (
             FMADDSUB_V4_CORE("dst", "a", "b", "c", "fmla")
             : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b), [c] "+r" (c),
               [count] "+r" (count)
             :
             : "cc", "memory",
               "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
               "q16", "q17", "q18", "q19", "q20", "q21", "q22", "q23",
               "q24", "q25", "q26", "q27", "q28", "q29", "q30", "q31"
         );
     }

     void fmsub4(float *dst, const float *a, const float *b, const float *c, size_t count)
     {
         ARCH_AARCH64_ASM
         (
             FMADDSUB_V4_CORE("dst", "a", "b", "c", "fmls")
             : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b), [c] "+r" (c),
               [count] "+r" (count)
             :
             : "cc", "memory",
               "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
               "q16", "q17", "q18", "q19", "q20", "q21", "q22", "q23",
               "q24", "q25", "q26", "q27", "q28", "q29", "q30", "q31"
         );
     }

#undef FMADDSUB_V4_CORE

#define FMOP_V4_CORE(DST, A, B, C, OP, SEL) \
    __ASM_EMIT("subs        %[count], %[count], #32") \
    __ASM_EMIT("blo         2f") \
    /* 32x blocks */ \
    __ASM_EMIT("1:") \
    __ASM_EMIT("ldp         q16, q17, [%[" B "], #0x00]") \
    __ASM_EMIT("ldp         q18, q19, [%[" B "], #0x20]") \
    __ASM_EMIT("ldp         q20, q21, [%[" B "], #0x40]") \
    __ASM_EMIT("ldp         q22, q23, [%[" B "], #0x60]") \
    __ASM_EMIT("ldp         q24, q25, [%[" C "], #0x00]") \
    __ASM_EMIT("ldp         q26, q27, [%[" C "], #0x20]") \
    __ASM_EMIT("ldp         q28, q29, [%[" C "], #0x40]") \
    __ASM_EMIT("ldp         q30, q31, [%[" C "], #0x60]") \
    __ASM_EMIT("ldp         q0, q1, [%[" A "], #0x00]") \
    __ASM_EMIT("ldp         q2, q3, [%[" A "], #0x20]") \
    __ASM_EMIT("ldp         q4, q5, [%[" A "], #0x40]") \
    __ASM_EMIT("ldp         q6, q7, [%[" A "], #0x60]") \
    __ASM_EMIT("fmul        v16.4s, v16.4s, v24.4s") \
    __ASM_EMIT("fmul        v17.4s, v17.4s, v25.4s") \
    __ASM_EMIT("fmul        v18.4s, v18.4s, v26.4s") \
    __ASM_EMIT("fmul        v19.4s, v19.4s, v27.4s") \
    __ASM_EMIT("fmul        v20.4s, v20.4s, v28.4s") \
    __ASM_EMIT("fmul        v21.4s, v21.4s, v29.4s") \
    __ASM_EMIT("fmul        v22.4s, v22.4s, v30.4s") \
    __ASM_EMIT("fmul        v23.4s, v23.4s, v31.4s") \
    __ASM_EMIT(OP "         v0.4s, " SEL("v0.4s", "v16.4s") ", " SEL("v16.4s", "v0.4s")) \
    __ASM_EMIT(OP "         v1.4s, " SEL("v1.4s", "v17.4s") ", " SEL("v17.4s", "v1.4s")) \
    __ASM_EMIT(OP "         v2.4s, " SEL("v2.4s", "v18.4s") ", " SEL("v18.4s", "v2.4s")) \
    __ASM_EMIT(OP "         v3.4s, " SEL("v3.4s", "v19.4s") ", " SEL("v19.4s", "v3.4s")) \
    __ASM_EMIT(OP "         v4.4s, " SEL("v4.4s", "v20.4s") ", " SEL("v20.4s", "v4.4s")) \
    __ASM_EMIT(OP "         v5.4s, " SEL("v5.4s", "v21.4s") ", " SEL("v21.4s", "v5.4s")) \
    __ASM_EMIT(OP "         v6.4s, " SEL("v6.4s", "v22.4s") ", " SEL("v22.4s", "v6.4s")) \
    __ASM_EMIT(OP "         v7.4s, " SEL("v7.4s", "v23.4s") ", " SEL("v23.4s", "v7.4s")) \
    __ASM_EMIT("stp         q0, q1, [%[" DST "], #0x00]") \
    __ASM_EMIT("stp         q2, q3, [%[" DST "], #0x20]") \
    __ASM_EMIT("stp         q4, q5, [%[" DST "], #0x40]") \
    __ASM_EMIT("stp         q6, q7, [%[" DST "], #0x60]") \
    __ASM_EMIT("subs        %[count], %[count], #32") \
    __ASM_EMIT("add         %[" DST "], %[" DST "], #0x80") \
    __ASM_EMIT("add         %[" A "], %[" A "], #0x80") \
    __ASM_EMIT("add         %[" B "], %[" B "], #0x80") \
    __ASM_EMIT("add         %[" C "], %[" C "], #0x80") \
    __ASM_EMIT("bhs         1b") \
    /* 16x block */ \
    __ASM_EMIT("2:") \
    __ASM_EMIT("adds        %[count], %[count], #16") /* 32-16 */ \
    __ASM_EMIT("blt         4f") \
    __ASM_EMIT("ldp         q16, q17, [%[" B "], #0x00]") \
    __ASM_EMIT("ldp         q18, q19, [%[" B "], #0x20]") \
    __ASM_EMIT("ldp         q24, q25, [%[" C "], #0x00]") \
    __ASM_EMIT("ldp         q26, q27, [%[" C "], #0x20]") \
    __ASM_EMIT("ldp         q0, q1, [%[" A "], #0x00]") \
    __ASM_EMIT("ldp         q2, q3, [%[" A "], #0x20]") \
    __ASM_EMIT("fmul        v16.4s, v16.4s, v24.4s") \
    __ASM_EMIT("fmul        v17.4s, v17.4s, v25.4s") \
    __ASM_EMIT("fmul        v18.4s, v18.4s, v26.4s") \
    __ASM_EMIT("fmul        v19.4s, v19.4s, v27.4s") \
    __ASM_EMIT(OP "         v0.4s, " SEL("v0.4s", "v16.4s") ", " SEL("v16.4s", "v0.4s")) \
    __ASM_EMIT(OP "         v1.4s, " SEL("v1.4s", "v17.4s") ", " SEL("v17.4s", "v1.4s")) \
    __ASM_EMIT(OP "         v2.4s, " SEL("v2.4s", "v18.4s") ", " SEL("v18.4s", "v2.4s")) \
    __ASM_EMIT(OP "         v3.4s, " SEL("v3.4s", "v19.4s") ", " SEL("v19.4s", "v3.4s")) \
    __ASM_EMIT("stp         q0, q1, [%[" DST "], #0x00]") \
    __ASM_EMIT("stp         q2, q3, [%[" DST "], #0x20]") \
    __ASM_EMIT("sub         %[count], %[count], #16") \
    __ASM_EMIT("add         %[" DST "], %[" DST "], #0x40") \
    __ASM_EMIT("add         %[" A "], %[" A "], #0x40") \
    __ASM_EMIT("add         %[" B "], %[" B "], #0x40") \
    __ASM_EMIT("add         %[" C "], %[" C "], #0x40") \
    /* 8x block */ \
    __ASM_EMIT("4:") \
    __ASM_EMIT("adds        %[count], %[count], #8") /* 16-8 */ \
    __ASM_EMIT("blt         6f") \
    __ASM_EMIT("ldp         q16, q17, [%[" B "], #0x00]") \
    __ASM_EMIT("ldp         q24, q25, [%[" C "], #0x00]") \
    __ASM_EMIT("ldp         q0, q1, [%[" A "], #0x00]") \
    __ASM_EMIT("fmul        v16.4s, v16.4s, v24.4s") \
    __ASM_EMIT("fmul        v17.4s, v17.4s, v25.4s") \
    __ASM_EMIT(OP "         v0.4s, " SEL("v0.4s", "v16.4s") ", " SEL("v16.4s", "v0.4s")) \
    __ASM_EMIT(OP "         v1.4s, " SEL("v1.4s", "v17.4s") ", " SEL("v17.4s", "v1.4s")) \
    __ASM_EMIT("stp         q0, q1, [%[" DST "], #0x00]") \
    __ASM_EMIT("sub         %[count], %[count], #8") \
    __ASM_EMIT("add         %[" DST "], %[" DST "], #0x20") \
    __ASM_EMIT("add         %[" A "], %[" A "], #0x20") \
    __ASM_EMIT("add         %[" B "], %[" B "], #0x20") \
    __ASM_EMIT("add         %[" C "], %[" C "], #0x20") \
    /* 4x block */ \
    __ASM_EMIT("6:") \
    __ASM_EMIT("adds        %[count], %[count], #4") /* 8 - 4 */ \
    __ASM_EMIT("blt         8f") \
    __ASM_EMIT("ldr         q16, [%[" B "], #0x00]") \
    __ASM_EMIT("ldr         q24, [%[" C "], #0x00]") \
    __ASM_EMIT("ldr         q0, [%[" A "], #0x00]") \
    __ASM_EMIT("fmul        v16.4s, v16.4s, v24.4s") \
    __ASM_EMIT(OP "         v0.4s, " SEL("v0.4s", "v16.4s") ", " SEL("v16.4s", "v0.4s")) \
    __ASM_EMIT("str         q0, [%[" DST "], #0x00]") \
    __ASM_EMIT("sub         %[count], %[count], #4") \
    __ASM_EMIT("add         %[" DST "], %[" DST "], #0x10") \
    __ASM_EMIT("add         %[" A "], %[" A "], #0x10") \
    __ASM_EMIT("add         %[" B "], %[" B "], #0x10") \
    __ASM_EMIT("add         %[" C "], %[" C "], #0x10") \
    /* 1x block */ \
    __ASM_EMIT("8:") \
    __ASM_EMIT("adds        %[count], %[count], #3") /* 4 - 3 */ \
    __ASM_EMIT("blt         10f") \
    __ASM_EMIT("9:") \
    __ASM_EMIT("ld1r        {v16.4s}, [%[" B "]]") \
    __ASM_EMIT("ld1r        {v24.4s}, [%[" C "]]") \
    __ASM_EMIT("ld1r        {v0.4s}, [%[" A "]]") \
    __ASM_EMIT("fmul        v16.4s, v16.4s, v24.4s") \
    __ASM_EMIT(OP "         v0.4s, " SEL("v0.4s", "v16.4s") ", " SEL("v16.4s", "v0.4s")) \
    __ASM_EMIT("st1         {v0.s}[0], [%[" DST "]]") \
    __ASM_EMIT("subs        %[count], %[count], #1") \
    __ASM_EMIT("add         %[" DST "], %[" DST "], #0x04") \
    __ASM_EMIT("add         %[" A "], %[" A "], #0x04") \
    __ASM_EMIT("add         %[" B "], %[" B "], #0x04") \
    __ASM_EMIT("add         %[" C "], %[" C "], #0x04") \
    __ASM_EMIT("bge         9b") \
    __ASM_EMIT("10:")

    void fmmul4(float *dst, const float *a, const float *b, const float *c, size_t count)
    {
        ARCH_AARCH64_ASM
        (
            FMOP_V4_CORE("dst", "a", "b", "c", "fmul", OP_DSEL)
            : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b), [c] "+r" (c),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q16", "q17", "q18", "q19", "q20", "q21", "q22", "q23",
              "q24", "q25", "q26", "q27", "q28", "q29", "q30", "q31"
        );
    }

    void fmrsub4(float *dst, const float *a, const float *b, const float *c, size_t count)
    {
        ARCH_AARCH64_ASM
        (
            FMOP_V4_CORE("dst", "a", "b", "c", "fsub", OP_RSEL)
            : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b), [c] "+r" (c),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q16", "q17", "q18", "q19", "q20", "q21", "q22", "q23",
              "q24", "q25", "q26", "q27", "q28", "q29", "q30", "q31"
        );
    }

#undef FMOP_V4_CORE

#define FMDIV_V4_CORE(DST, A, B, C, SEL) \
    __ASM_EMIT("subs        %[count], %[count], #16") \
    __ASM_EMIT("mov         v21.16b, v20.16b") \
    __ASM_EMIT("blo         2f") \
    /* 16x blocks */ \
    __ASM_EMIT("1:") \
    __ASM_EMIT("ldp         q0, q1, [%[" SEL(B, A) "], #0x00]") \
    __ASM_EMIT("ldp         q2, q3, [%[" SEL(B, A) "], #0x20]") \
    __ASM_EMIT("ldp         q4, q5, [%[" SEL(A, B) "], #0x00]") \
    __ASM_EMIT("ldp         q6, q7, [%[" SEL(A, B) "], #0x20]") \
    __ASM_EMIT("ldp         q16, q17, [%[" C "], #0x00]") \
    __ASM_EMIT("ldp         q18, q19, [%[" C "], #0x20]") \
    __ASM_EMIT("fmul      " SEL("v0.4s, v0.4s", "v4.4s, v4.4s") ", v16.4s") /* {v0, v4} = {B*C, DST} | {DST, B*C} */ \
    __ASM_EMIT("fmul      " SEL("v1.4s, v1.4s", "v5.4s, v5.4s") ", v17.4s") \
    __ASM_EMIT("fmul      " SEL("v2.4s, v2.4s", "v6.4s, v6.4s") ", v18.4s") \
    __ASM_EMIT("fmul      " SEL("v3.4s, v3.4s", "v7.4s, v7.4s") ", v19.4s") \
    __ASM_EMIT("frecpe      v16.4s, v0.4s")                     /* v16 = s2 */ \
    __ASM_EMIT("frecpe      v17.4s, v1.4s") \
    __ASM_EMIT("frecpe      v18.4s, v2.4s") \
    __ASM_EMIT("frecpe      v19.4s, v3.4s") \
    __ASM_EMIT("frecps      v24.4s, v16.4s, v0.4s")             /* v24 = (2 - R*s2) */ \
    __ASM_EMIT("frecps      v25.4s, v17.4s, v1.4s") \
    __ASM_EMIT("frecps      v26.4s, v18.4s, v2.4s") \
    __ASM_EMIT("frecps      v27.4s, v19.4s, v3.4s") \
    __ASM_EMIT("fmul        v16.4s, v24.4s, v16.4s")            /* v16 = s2' = s2 * (2 - R*s2) */ \
    __ASM_EMIT("fmul        v17.4s, v25.4s, v17.4s") \
    __ASM_EMIT("fmul        v18.4s, v26.4s, v18.4s") \
    __ASM_EMIT("fmul        v19.4s, v27.4s, v19.4s") \
    __ASM_EMIT("frecps      v24.4s, v16.4s, v0.4s")             /* v24 = (2 - R*s2') */ \
    __ASM_EMIT("frecps      v25.4s, v17.4s, v1.4s") \
    __ASM_EMIT("frecps      v26.4s, v18.4s, v2.4s") \
    __ASM_EMIT("frecps      v27.4s, v19.4s, v3.4s") \
    __ASM_EMIT("fmul        v0.4s, v24.4s, v16.4s")             /* v0  = s2" = s2' * (2 - R*s2) = 1/s2 = 1/(B*C) | 1/DST */  \
    __ASM_EMIT("fmul        v1.4s, v25.4s, v17.4s") \
    __ASM_EMIT("fmul        v2.4s, v26.4s, v18.4s") \
    __ASM_EMIT("fmul        v3.4s, v27.4s, v19.4s") \
    __ASM_EMIT("fmul        v0.4s, v0.4s, v4.4s")               /* DST/(B*C) | (B*C)/DST */ \
    __ASM_EMIT("fmul        v1.4s, v1.4s, v5.4s") \
    __ASM_EMIT("fmul        v2.4s, v2.4s, v6.4s") \
    __ASM_EMIT("fmul        v3.4s, v3.4s, v7.4s") \
    __ASM_EMIT("stp         q0, q1, [%[" DST "], #0x00]") \
    __ASM_EMIT("stp         q2, q3, [%[" DST "], #0x20]") \
    __ASM_EMIT("subs        %[count], %[count], #16") \
    __ASM_EMIT("add         %[" DST "], %[" DST "], #0x40") \
    __ASM_EMIT("add         %[" A "], %[" A "], #0x40") \
    __ASM_EMIT("add         %[" B "], %[" B "], #0x40") \
    __ASM_EMIT("add         %[" C "], %[" C "], #0x40") \
    __ASM_EMIT("bhs         1b") \
    /* 8x block */ \
    __ASM_EMIT("2:") \
    __ASM_EMIT("adds        %[count], %[count], #8")            /* 16 - 8 */ \
    __ASM_EMIT("blt         4f") \
    __ASM_EMIT("ldp         q0, q1, [%[" SEL(B, A) "], #0x00]") \
    __ASM_EMIT("ldp         q4, q5, [%[" SEL(A, B) "], #0x00]") \
    __ASM_EMIT("ldp         q16, q17, [%[" C "], #0x00]") \
    __ASM_EMIT("fmul      " SEL("v0.4s, v0.4s", "v4.4s, v4.4s") ", v16.4s") /* {v0, v4} = {B*C, DST} | {DST, B*C} */ \
    __ASM_EMIT("fmul      " SEL("v1.4s, v1.4s", "v5.4s, v5.4s") ", v17.4s") \
    __ASM_EMIT("frecpe      v16.4s, v0.4s")                     /* v16 = s2 */ \
    __ASM_EMIT("frecpe      v17.4s, v1.4s") \
    __ASM_EMIT("frecps      v24.4s, v16.4s, v0.4s")             /* v24 = (2 - R*s2) */ \
    __ASM_EMIT("frecps      v25.4s, v17.4s, v1.4s") \
    __ASM_EMIT("fmul        v16.4s, v24.4s, v16.4s")            /* v16 = s2' = s2 * (2 - R*s2) */ \
    __ASM_EMIT("fmul        v17.4s, v25.4s, v17.4s") \
    __ASM_EMIT("frecps      v24.4s, v16.4s, v0.4s")             /* v24 = (2 - R*s2') */ \
    __ASM_EMIT("frecps      v25.4s, v17.4s, v1.4s") \
    __ASM_EMIT("fmul        v0.4s, v24.4s, v16.4s")             /* v0  = s2" = s2' * (2 - R*s2) = 1/s2 = 1/(B*C) | 1/DST */  \
    __ASM_EMIT("fmul        v1.4s, v25.4s, v17.4s") \
    __ASM_EMIT("fmul        v0.4s, v0.4s, v4.4s")               /* DST/(B*C) | (B*C)/DST */ \
    __ASM_EMIT("fmul        v1.4s, v1.4s, v5.4s") \
    __ASM_EMIT("stp         q0, q1, [%[" DST "], #0x00]") \
    __ASM_EMIT("sub         %[count], %[count], #8") \
    __ASM_EMIT("add         %[" DST "], %[" DST "], #0x20") \
    __ASM_EMIT("add         %[" A "], %[" A "], #0x20") \
    __ASM_EMIT("add         %[" B "], %[" B "], #0x20") \
    __ASM_EMIT("add         %[" C "], %[" C "], #0x20") \
    /* 4x block */ \
    __ASM_EMIT("4:") \
    __ASM_EMIT("adds        %[count], %[count], #4")            /* 8 - 4 */ \
    __ASM_EMIT("blt         6f") \
    __ASM_EMIT("ldr         q0, [%[" SEL(B, A) "], #0x00]") \
    __ASM_EMIT("ldr         q4, [%[" SEL(A, B) "], #0x00]") \
    __ASM_EMIT("ldr         q16, [%[" C "], #0x00]") \
    __ASM_EMIT("fmul      " SEL("v0.4s, v0.4s", "v4.4s, v4.4s") ", v16.4s") /* {v0, v4} = {B*C, DST} | {DST, B*C} */ \
    __ASM_EMIT("frecpe      v16.4s, v0.4s")                     /* v16 = s2 */ \
    __ASM_EMIT("frecps      v24.4s, v16.4s, v0.4s")             /* v24 = (2 - R*s2) */ \
    __ASM_EMIT("fmul        v16.4s, v24.4s, v16.4s")            /* v16 = s2' = s2 * (2 - R*s2) */ \
    __ASM_EMIT("frecps      v24.4s, v16.4s, v0.4s")             /* v24 = (2 - R*s2') */ \
    __ASM_EMIT("fmul        v0.4s, v24.4s, v16.4s")             /* v0  = s2" = s2' * (2 - R*s2) = 1/s2 = 1/(B*C) | 1/DST */  \
    __ASM_EMIT("fmul        v0.4s, v0.4s, v4.4s")               /* DST/(B*C) | (B*C)/DST */ \
    __ASM_EMIT("str         q0, [%[" DST "], #0x00]") \
    __ASM_EMIT("sub         %[count], %[count], #4") \
    __ASM_EMIT("add         %[" DST "], %[" DST "], #0x10") \
    __ASM_EMIT("add         %[" A "], %[" A "], #0x10") \
    __ASM_EMIT("add         %[" B "], %[" B "], #0x10") \
    __ASM_EMIT("add         %[" C "], %[" C "], #0x10") \
    /* 1x block */ \
    __ASM_EMIT("6:") \
    __ASM_EMIT("adds        %[count], %[count], #3")            /* 4 - 3 */ \
    __ASM_EMIT("blt         8f") \
    __ASM_EMIT("7:") \
    __ASM_EMIT("ld1r        {v0.4s}, [%[" SEL(B, A) "]]") \
    __ASM_EMIT("ld1r        {v4.4s}, [%[" SEL(A, B) "]]") \
    __ASM_EMIT("ld1r        {v16.4s}, [%[" C "]]") \
    __ASM_EMIT("fmul      " SEL("v0.4s, v0.4s", "v4.4s, v4.4s") ", v16.4s") /* {v0, v4} = {B*C, DST} | {DST, B*C} */ \
    __ASM_EMIT("frecpe      v16.4s, v0.4s")                     /* v16 = s2 */ \
    __ASM_EMIT("frecps      v24.4s, v16.4s, v0.4s")             /* v24 = (2 - R*s2) */ \
    __ASM_EMIT("fmul        v16.4s, v24.4s, v16.4s")            /* v16 = s2' = s2 * (2 - R*s2) */ \
    __ASM_EMIT("frecps      v24.4s, v16.4s, v0.4s")             /* v24 = (2 - R*s2') */ \
    __ASM_EMIT("fmul        v0.4s, v24.4s, v16.4s")             /* v0  = s2" = s2' * (2 - R*s2) = 1/s2 = 1/(B*C) | 1/DST */  \
    __ASM_EMIT("fmul        v0.4s, v0.4s, v4.4s")               /* DST/(B*C) | (B*C)/DST */ \
    __ASM_EMIT("st1         {v0.s}[0], [%[" DST "]]") \
    __ASM_EMIT("subs        %[count], %[count], #1") \
    __ASM_EMIT("add         %[" DST "], %[" DST "], #0x04") \
    __ASM_EMIT("add         %[" A "], %[" A "], #0x04") \
    __ASM_EMIT("add         %[" B "], %[" B "], #0x04") \
    __ASM_EMIT("add         %[" C "], %[" C "], #0x04") \
    __ASM_EMIT("bge         7b") \
    __ASM_EMIT("8:")

    void fmdiv4(float *dst, const float *a, const float *b, const float *c, size_t count)
    {
        ARCH_AARCH64_ASM
        (
            FMDIV_V4_CORE("dst", "a", "b", "c", OP_DSEL)
            : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b), [c] "+r" (c),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
              "q16", "q17", "q18", "q19",
              "q24", "q25", "q26", "q27"
        );
    }

    void fmrdiv4(float *dst, const float *a, const float *b, const float *c, size_t count)
    {
        ARCH_AARCH64_ASM
        (
            FMDIV_V4_CORE("dst", "a", "b", "c", OP_RSEL)
            : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b), [c] "+r" (c),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
              "q16", "q17", "q18", "q19",
              "q24", "q25", "q26", "q27"
        );
    }

#undef FMDIV_V3_CORE

#define INC_ON(x)           x
#define INC_OFF(x)

#define FMMOD_VV_CORE(DST, INCD, A, INCA, B, INCB, C, INCC, SEL)   \
    __ASM_EMIT("subs            %[count], %[count], #16") \
    __ASM_EMIT("b.lo            2f") \
    /* 16x blocks */ \
    __ASM_EMIT("1:") \
    __ASM_EMIT("ldp             q20, q21, [%[" C "], #0x00]") \
    __ASM_EMIT("ldp             q22, q23, [%[" C "], #0x20]") \
    __ASM_EMIT("ldp             q4, q5, [%[" SEL(B, A) "], #0x00]") \
    __ASM_EMIT("ldp             q6, q7, [%[" SEL(B, A) "], #0x20]") \
    __ASM_EMIT(SEL("fmul        v4.4s, v4.4s, v20.4s", ""))                     /* v4   = d */ \
    __ASM_EMIT(SEL("fmul        v5.4s, v5.4s, v21.4s", "")) \
    __ASM_EMIT(SEL("fmul        v6.4s, v6.4s, v22.4s", "")) \
    __ASM_EMIT(SEL("fmul        v7.4s, v7.4s, v23.4s", "")) \
    __ASM_EMIT("frecpe          v16.4s, v4.4s")                                 /* v16  = s2 */ \
    __ASM_EMIT("frecpe          v17.4s, v5.4s") \
    __ASM_EMIT("frecpe          v18.4s, v6.4s") \
    __ASM_EMIT("frecpe          v19.4s, v7.4s") \
    __ASM_EMIT("frecps          v0.4s, v16.4s, v4.4s")                          /* v0   = (2 - R*s2) */ \
    __ASM_EMIT("frecps          v1.4s, v17.4s, v5.4s") \
    __ASM_EMIT("frecps          v2.4s, v18.4s, v6.4s") \
    __ASM_EMIT("frecps          v3.4s, v19.4s, v7.4s") \
    __ASM_EMIT("fmul            v16.4s, v0.4s, v16.4s")                         /* v16  = s2' = s2 * (2 - R*s2) */ \
    __ASM_EMIT("fmul            v17.4s, v1.4s, v17.4s") \
    __ASM_EMIT("fmul            v18.4s, v2.4s, v18.4s") \
    __ASM_EMIT("fmul            v19.4s, v3.4s, v19.4s") \
    __ASM_EMIT("frecps          v0.4s, v16.4s, v4.4s")                          /* v0   = (2 - R*s2') */ \
    __ASM_EMIT("frecps          v1.4s, v17.4s, v5.4s") \
    __ASM_EMIT("frecps          v2.4s, v18.4s, v6.4s") \
    __ASM_EMIT("frecps          v3.4s, v19.4s, v7.4s") \
    __ASM_EMIT("fmul            v16.4s, v0.4s, v16.4s")                         /* v16  = s2" = s2' * (2 - R*s2) = 1/d */  \
    __ASM_EMIT("fmul            v17.4s, v1.4s, v17.4s") \
    __ASM_EMIT("fmul            v18.4s, v2.4s, v18.4s") \
    __ASM_EMIT("fmul            v19.4s, v3.4s, v19.4s") \
    __ASM_EMIT("ldp             q0, q1, [%[" SEL(A, B) "], #0x00]") \
    __ASM_EMIT("ldp             q2, q3, [%[" SEL(A, B) "], #0x20]") \
    __ASM_EMIT(SEL("", "fmul    v0.4s, v0.4s, v20.4s"))                         /* v0   = s */ \
    __ASM_EMIT(SEL("", "fmul    v1.4s, v1.4s, v21.4s"))\
    __ASM_EMIT(SEL("", "fmul    v2.4s, v2.4s, v22.4s")) \
    __ASM_EMIT(SEL("", "fmul    v3.4s, v3.4s, v23.4s")) \
    __ASM_EMIT("fmul            v16.4s, v16.4s, v0.4s")                         /* v16  = s/d */ \
    __ASM_EMIT("fmul            v17.4s, v17.4s, v1.4s") \
    __ASM_EMIT("fmul            v18.4s, v18.4s, v2.4s") \
    __ASM_EMIT("fmul            v19.4s, v19.4s, v3.4s") \
    __ASM_EMIT("frintz          v16.4s, v16.4s") \
    __ASM_EMIT("frintz          v17.4s, v17.4s") \
    __ASM_EMIT("frintz          v18.4s, v18.4s") \
    __ASM_EMIT("frintz          v19.4s, v19.4s") \
    __ASM_EMIT("fmls            v0.4s, v4.4s, v16.4s")                          /* v0   = s - d*int(s/d) */ \
    __ASM_EMIT("fmls            v1.4s, v5.4s, v17.4s") \
    __ASM_EMIT("fmls            v2.4s, v6.4s, v18.4s") \
    __ASM_EMIT("fmls            v3.4s, v7.4s, v19.4s") \
    __ASM_EMIT("stp             q0, q1, [%[" DST "], #0x00]") \
    __ASM_EMIT("stp             q2, q3, [%[" DST "], #0x20]") \
    __ASM_EMIT("subs            %[count], %[count], #16") \
    __ASM_EMIT(INCA("add        %[" A "], %[" A "], #0x40")) \
    __ASM_EMIT(INCB("add        %[" B "], %[" B "], #0x40")) \
    __ASM_EMIT(INCC("add        %[" C "], %[" C "], #0x40")) \
    __ASM_EMIT(INCD("add        %[" DST "], %[" DST "], #0x40")) \
    __ASM_EMIT("b.hs            1b") \
    /* 8x block */ \
    __ASM_EMIT("2:") \
    __ASM_EMIT("adds            %[count], %[count], #8") \
    __ASM_EMIT("b.lt            4f") \
    __ASM_EMIT("ldp             q20, q21, [%[" C "], #0x00]") \
    __ASM_EMIT("ldp             q4, q5, [%[" SEL(B, A) "], #0x00]") \
    __ASM_EMIT(SEL("fmul        v4.4s, v4.4s, v20.4s", ""))                     /* v4   = d */ \
    __ASM_EMIT(SEL("fmul        v5.4s, v5.4s, v21.4s", "")) \
    __ASM_EMIT("frecpe          v16.4s, v4.4s")                                 /* v16  = s2 */ \
    __ASM_EMIT("frecpe          v17.4s, v5.4s") \
    __ASM_EMIT("frecps          v0.4s, v16.4s, v4.4s")                          /* v0   = (2 - R*s2) */ \
    __ASM_EMIT("frecps          v1.4s, v17.4s, v5.4s") \
    __ASM_EMIT("fmul            v16.4s, v0.4s, v16.4s")                         /* v16  = s2' = s2 * (2 - R*s2) */ \
    __ASM_EMIT("fmul            v17.4s, v1.4s, v17.4s") \
    __ASM_EMIT("frecps          v0.4s, v16.4s, v4.4s")                          /* v0   = (2 - R*s2') */ \
    __ASM_EMIT("frecps          v1.4s, v17.4s, v5.4s") \
    __ASM_EMIT("fmul            v16.4s, v0.4s, v16.4s")                         /* v16  = s2" = s2' * (2 - R*s2) = 1/d */  \
    __ASM_EMIT("fmul            v17.4s, v1.4s, v17.4s") \
    __ASM_EMIT("ldp             q0, q1, [%[" SEL(A, B) "], #0x00]") \
    __ASM_EMIT(SEL("", "fmul    v0.4s, v0.4s, v20.4s"))                         /* v0   = s */ \
    __ASM_EMIT(SEL("", "fmul    v1.4s, v1.4s, v21.4s"))\
    __ASM_EMIT("fmul            v16.4s, v16.4s, v0.4s")                         /* v16  = s/d */ \
    __ASM_EMIT("fmul            v17.4s, v17.4s, v1.4s") \
    __ASM_EMIT("frintz          v16.4s, v16.4s") \
    __ASM_EMIT("frintz          v17.4s, v17.4s") \
    __ASM_EMIT("fmls            v0.4s, v4.4s, v16.4s")                          /* v0   = s - d*int(s/d) */ \
    __ASM_EMIT("fmls            v1.4s, v5.4s, v17.4s") \
    __ASM_EMIT("stp             q0, q1, [%[" DST "], #0x00]") \
    __ASM_EMIT("sub             %[count], %[count], #8") \
    __ASM_EMIT(INCA("add        %[" A "], %[" A "], #0x20")) \
    __ASM_EMIT(INCB("add        %[" B "], %[" B "], #0x20")) \
    __ASM_EMIT(INCC("add        %[" C "], %[" C "], #0x20")) \
    __ASM_EMIT(INCD("add        %[" DST "], %[" DST "], #0x20")) \
    /* 4x blocks */ \
    __ASM_EMIT("4:") \
    __ASM_EMIT("adds            %[count], %[count], #4") \
    __ASM_EMIT("b.lt            6f") \
    __ASM_EMIT("ldr             q20, [%[" C "]]") \
    __ASM_EMIT("ldr             q4, [%[" SEL(B, A) "]]") \
    __ASM_EMIT(SEL("fmul        v4.4s, v4.4s, v20.4s", ""))                     /* v4   = d */ \
    __ASM_EMIT("frecpe          v16.4s, v4.4s")                                 /* v16  = s2 */ \
    __ASM_EMIT("frecps          v0.4s, v16.4s, v4.4s")                          /* v0   = (2 - R*s2) */ \
    __ASM_EMIT("fmul            v16.4s, v0.4s, v16.4s")                         /* v16  = s2' = s2 * (2 - R*s2) */ \
    __ASM_EMIT("frecps          v0.4s, v16.4s, v4.4s")                          /* v0   = (2 - R*s2') */ \
    __ASM_EMIT("fmul            v16.4s, v0.4s, v16.4s")                         /* v16  = s2" = s2' * (2 - R*s2) = 1/d */  \
    __ASM_EMIT("ldr             q0,[%[" SEL(A, B) "]]") \
    __ASM_EMIT(SEL("", "fmul    v0.4s, v0.4s, v20.4s"))                         /* v0   = s */ \
    __ASM_EMIT("fmul            v16.4s, v16.4s, v0.4s")                         /* v16  = s/d */ \
    __ASM_EMIT("frintz          v16.4s, v16.4s") \
    __ASM_EMIT("fmls            v0.4s, v4.4s, v16.4s")                          /* v0   = s - d*int(s/d) */ \
    __ASM_EMIT("str             q0, [%[" DST "]]") \
    __ASM_EMIT("sub             %[count], %[count], #4") \
    __ASM_EMIT(INCA("add        %[" A "], %[" A "], #0x10")) \
    __ASM_EMIT(INCB("add        %[" B "], %[" B "], #0x10")) \
    __ASM_EMIT(INCC("add        %[" C "], %[" C "], #0x10")) \
    __ASM_EMIT(INCD("add        %[" DST "], %[" DST "], #0x10")) \
    /* 1x blocks */ \
    __ASM_EMIT("6:") \
    __ASM_EMIT("adds            %[count], %[count], #3") \
    __ASM_EMIT("b.lt            8f") \
    __ASM_EMIT("7:") \
    __ASM_EMIT("ld1r            {v20.4s}, [%[" C "]]") \
    __ASM_EMIT("ld1r            {v4.4s}, [%[" SEL(B, A) "]]") \
    __ASM_EMIT(SEL("fmul        v4.4s, v4.4s, v20.4s", ""))                     /* v4   = d */ \
    __ASM_EMIT("frecpe          v16.4s, v4.4s")                                 /* v16  = s2 */ \
    __ASM_EMIT("frecps          v0.4s, v16.4s, v4.4s")                          /* v0   = (2 - R*s2) */ \
    __ASM_EMIT("fmul            v16.4s, v0.4s, v16.4s")                         /* v16  = s2' = s2 * (2 - R*s2) */ \
    __ASM_EMIT("frecps          v0.4s, v16.4s, v4.4s")                          /* v0   = (2 - R*s2') */ \
    __ASM_EMIT("fmul            v16.4s, v0.4s, v16.4s")                         /* v16  = s2" = s2' * (2 - R*s2) = 1/d */  \
    __ASM_EMIT("ld1r            {v0.4s}, [%[" SEL(A, B) "]]") \
    __ASM_EMIT(SEL("", "fmul    v0.4s, v0.4s, v20.4s"))                         /* v0   = s */ \
    __ASM_EMIT("fmul            v16.4s, v16.4s, v0.4s")                         /* v16  = s/d */ \
    __ASM_EMIT("frintz          v16.4s, v16.4s") \
    __ASM_EMIT("fmls            v0.4s, v4.4s, v16.4s")                          /* v0   = s - d*int(s/d) */ \
    __ASM_EMIT("st1             {v0.s}[0], [%[" DST "]]") \
    __ASM_EMIT("subs            %[count], %[count], #1") \
    __ASM_EMIT(INCA("add        %[" A "], %[" A "], #0x04")) \
    __ASM_EMIT(INCB("add        %[" B "], %[" B "], #0x04")) \
    __ASM_EMIT(INCC("add        %[" C "], %[" C "], #0x04")) \
    __ASM_EMIT(INCD("add        %[" DST "], %[" DST "], #0x04")) \
    __ASM_EMIT("b.ge            7b") \
    __ASM_EMIT("8:")

    void fmmod3(float *dst, const float *a, const float *b, size_t count)
    {
        ARCH_AARCH64_ASM
        (
            FMMOD_VV_CORE("dst", INC_ON, "dst", INC_OFF, "a", INC_ON, "b", INC_ON, OP_DSEL)
            : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
              "q16", "q17", "q18", "q19", "q20", "q21", "q22", "q23"
        );
    }

    void fmrmod3(float *dst, const float *a, const float *b, size_t count)
    {
        ARCH_AARCH64_ASM
        (
            FMMOD_VV_CORE("dst", INC_ON, "dst", INC_OFF, "a", INC_ON, "b", INC_ON, OP_RSEL)
            : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
              "q16", "q17", "q18", "q19", "q20", "q21", "q22", "q23"
        );
    }

    void fmmod4(float *dst, const float *a, const float *b, const float *c, size_t count)
    {
        ARCH_AARCH64_ASM
        (
            FMMOD_VV_CORE("dst", INC_ON, "a", INC_ON, "b", INC_ON, "c", INC_ON, OP_DSEL)
            : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b), [c] "+r" (c),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
              "q16", "q17", "q18", "q19", "q20", "q21", "q22", "q23"
        );
    }

    void fmrmod4(float *dst, const float *a, const float *b, const float *c, size_t count)
    {
        ARCH_AARCH64_ASM
        (
            FMMOD_VV_CORE("dst", INC_ON, "a", INC_ON, "b", INC_ON, "c", INC_ON, OP_RSEL)
            : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b), [c] "+r" (c),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
              "q16", "q17", "q18", "q19", "q20", "q21", "q22", "q23"
        );
    }

#undef FMMOD_VV_CORE

#undef INC_ON
#undef INC_OFF

#undef OP_DSEL
#undef OP_RSEL
}

#endif /* DSP_ARCH_AARCH64_ASIMD_PMATH_FMOP_VV_H_ */
