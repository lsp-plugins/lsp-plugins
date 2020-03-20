/*
 * op_vv.h
 *
 *  Created on: 19 нояб. 2019 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_AARCH64_ASIMD_PMATH_OP_VV_H_
#define DSP_ARCH_AARCH64_ASIMD_PMATH_OP_VV_H_

#ifndef DSP_ARCH_AARCH64_ASIMD_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_AARCH64_ASIMD_IMPL */

namespace asimd
{
#define OP_SDSEL(a, b)      a
#define OP_SRSEL(a, b)      b

#define OP_VV2_CORE(DST, SRC, OP, SEL) \
    __ASM_EMIT("subs        %[count], %[count], #32") \
    __ASM_EMIT("b.lo        2f") \
    /* 32x blocks */ \
    __ASM_EMIT("1:") \
    __ASM_EMIT("ldp         q0, q1, [%[" SEL(DST, SRC) "], #0x00]") \
    __ASM_EMIT("ldp         q2, q3, [%[" SEL(DST, SRC) "], #0x20]") \
    __ASM_EMIT("ldp         q4, q5, [%[" SEL(DST, SRC) "], #0x40]") \
    __ASM_EMIT("ldp         q6, q7, [%[" SEL(DST, SRC) "], #0x60]") \
    __ASM_EMIT("ldp         q16, q17, [%[" SEL(SRC, DST) "], #0x00]") \
    __ASM_EMIT("ldp         q18, q19, [%[" SEL(SRC, DST) "], #0x20]") \
    __ASM_EMIT("ldp         q20, q21, [%[" SEL(SRC, DST) "], #0x40]") \
    __ASM_EMIT("ldp         q22, q23, [%[" SEL(SRC, DST) "], #0x60]") \
    __ASM_EMIT(OP "         v0.4s, v0.4s, v16.4s") \
    __ASM_EMIT(OP "         v1.4s, v1.4s, v17.4s") \
    __ASM_EMIT(OP "         v2.4s, v2.4s, v18.4s") \
    __ASM_EMIT(OP "         v3.4s, v3.4s, v19.4s") \
    __ASM_EMIT(OP "         v4.4s, v4.4s, v20.4s") \
    __ASM_EMIT(OP "         v5.4s, v5.4s, v21.4s") \
    __ASM_EMIT(OP "         v6.4s, v6.4s, v22.4s") \
    __ASM_EMIT(OP "         v7.4s, v7.4s, v23.4s") \
    __ASM_EMIT("stp         q0, q1, [%[" DST "], #0x00]") \
    __ASM_EMIT("stp         q2, q3, [%[" DST "], #0x20]") \
    __ASM_EMIT("stp         q4, q5, [%[" DST "], #0x40]") \
    __ASM_EMIT("stp         q6, q7, [%[" DST "], #0x60]") \
    __ASM_EMIT("subs        %[count], %[count], #32") \
    __ASM_EMIT("add         %[" DST "], %[" DST "], #0x80") \
    __ASM_EMIT("add         %[" SRC "], %[" SRC "], #0x80") \
    __ASM_EMIT("b.hs        1b") \
    /* 16x block */ \
    __ASM_EMIT("2:") \
    __ASM_EMIT("adds        %[count], %[count], #16") /* 32 - 16 */ \
    __ASM_EMIT("b.lt        4f") \
    __ASM_EMIT("ldp         q0, q1, [%[" SEL(DST, SRC) "], #0x00]") \
    __ASM_EMIT("ldp         q2, q3, [%[" SEL(DST, SRC) "], #0x20]") \
    __ASM_EMIT("ldp         q16, q17, [%[" SEL(SRC, DST) "], #0x00]") \
    __ASM_EMIT("ldp         q18, q19, [%[" SEL(SRC, DST) "], #0x20]") \
    __ASM_EMIT(OP "         v0.4s, v0.4s, v16.4s") \
    __ASM_EMIT(OP "         v1.4s, v1.4s, v17.4s") \
    __ASM_EMIT(OP "         v2.4s, v2.4s, v18.4s") \
    __ASM_EMIT(OP "         v3.4s, v3.4s, v19.4s") \
    __ASM_EMIT("stp         q0, q1, [%[" DST "], #0x00]") \
    __ASM_EMIT("stp         q2, q3, [%[" DST "], #0x20]") \
    __ASM_EMIT("sub         %[count], %[count], #16") \
    __ASM_EMIT("add         %[" DST "], %[" DST "], #0x40") \
    __ASM_EMIT("add         %[" SRC "], %[" SRC "], #0x40") \
    /* 8x block */ \
    __ASM_EMIT("4:") \
    __ASM_EMIT("adds        %[count], %[count], #8") /* 16 - 8 */ \
    __ASM_EMIT("b.lt        6f") \
    __ASM_EMIT("ldp         q0, q1, [%[" SEL(DST, SRC) "], #0x00]") \
    __ASM_EMIT("ldp         q2, q3, [%[" SEL(DST, SRC) "], #0x20]") \
    __ASM_EMIT("ldp         q16, q17, [%[" SEL(SRC, DST) "], #0x00]") \
    __ASM_EMIT("ldp         q18, q19, [%[" SEL(SRC, DST) "], #0x20]") \
    __ASM_EMIT(OP "         v0.4s, v0.4s, v16.4s") \
    __ASM_EMIT(OP "         v1.4s, v1.4s, v17.4s") \
    __ASM_EMIT("stp         q0, q1, [%[" DST "], #0x00]") \
    __ASM_EMIT("sub         %[count], %[count], #8") \
    __ASM_EMIT("add         %[" DST "], %[" DST "], #0x20") \
    __ASM_EMIT("add         %[" SRC "], %[" SRC "], #0x20") \
    /* 4x block */ \
    __ASM_EMIT("6:") \
    __ASM_EMIT("adds        %[count], %[count], #4") /* 8 - 4 */ \
    __ASM_EMIT("b.lt        8f") \
    __ASM_EMIT("ldr         q0, [%[" SEL(DST, SRC) "], #0x00]") \
    __ASM_EMIT("ldr         q16, [%[" SEL(SRC, DST) "], #0x00]") \
    __ASM_EMIT(OP "         v0.4s, v0.4s, v16.4s") \
    __ASM_EMIT("str         q0, [%[" DST "], #0x00]") \
    __ASM_EMIT("sub         %[count], %[count], #4") \
    __ASM_EMIT("add         %[" DST "], %[" DST "], #0x10") \
    __ASM_EMIT("add         %[" SRC "], %[" SRC "], #0x10") \
    /* 1x block */ \
    __ASM_EMIT("8:") \
    __ASM_EMIT("adds        %[count], %[count], #3") /* 4 - 3 */ \
    __ASM_EMIT("b.lt        10f") \
    __ASM_EMIT("9:") \
    __ASM_EMIT("ld1r        {v0.4s}, [%[" SEL(DST, SRC) "]]") \
    __ASM_EMIT("ld1r        {v16.4s}, [%[" SEL(SRC, DST) "]]") \
    __ASM_EMIT(OP "         v0.4s, v0.4s, v16.4s") \
    __ASM_EMIT("st1         {v0.s}[0], [%[" DST "]]") \
    __ASM_EMIT("subs        %[count], %[count], #1") \
    __ASM_EMIT("add         %[" DST "], %[" DST "], #0x04") \
    __ASM_EMIT("add         %[" SRC "], %[" SRC "], #0x04") \
    __ASM_EMIT("bge         9b") \
    __ASM_EMIT("10:")

     void add2(float *dst, const float *src, size_t count)
     {
         ARCH_AARCH64_ASM
         (
             OP_VV2_CORE("dst", "src", "fadd", OP_SDSEL)
             : [dst] "+r" (dst), [src] "+r" (src),
               [count] "+r" (count)
             :
             : "cc", "memory",
               "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
               "q16", "q17", "q18", "q19", "q20", "q21", "q22", "q23"
         );
     }

     void sub2(float *dst, const float *src, size_t count)
     {
         ARCH_AARCH64_ASM
         (
             OP_VV2_CORE("dst", "src", "fsub", OP_SDSEL)
             : [dst] "+r" (dst), [src] "+r" (src),
               [count] "+r" (count)
             :
             : "cc", "memory",
               "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
               "q16", "q17", "q18", "q19", "q20", "q21", "q22", "q23"
         );
     }

     void rsub2(float *dst, const float *src, size_t count)
     {
         ARCH_AARCH64_ASM
         (
             OP_VV2_CORE("dst", "src", "fsub", OP_SRSEL)
             : [dst] "+r" (dst), [src] "+r" (src),
               [count] "+r" (count)
             :
             : "cc", "memory",
               "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
               "q16", "q17", "q18", "q19", "q20", "q21", "q22", "q23"
         );
     }

     void mul2(float *dst, const float *src, size_t count)
     {
         ARCH_AARCH64_ASM
         (
             OP_VV2_CORE("dst", "src", "fmul", OP_SDSEL)
             : [dst] "+r" (dst), [src] "+r" (src),
               [count] "+r" (count)
             :
             : "cc", "memory",
               "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
               "q16", "q17", "q18", "q19", "q20", "q21", "q22", "q23"
         );
     }

#undef OP_VV2_CORE

#define OP_DIV2_CORE(DST, SRC, SEL) \
    __ASM_EMIT("subs        %[count], %[count], #16") \
    __ASM_EMIT("b.lo        2f") \
    /* 16x blocks */ \
    __ASM_EMIT("1:") \
    __ASM_EMIT("ldp         q0, q1, [%[" SEL(SRC, DST) "], #0x00]") \
    __ASM_EMIT("ldp         q2, q3, [%[" SEL(SRC, DST) "], #0x20]") \
    __ASM_EMIT("ldp         q4, q5, [%[" SEL(DST, SRC) "], #0x00]") \
    __ASM_EMIT("ldp         q6, q7, [%[" SEL(DST, SRC) "], #0x20]") \
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
    __ASM_EMIT("fmul        v0.4s, v24.4s, v16.4s")             /* v0  = s2" = s2' * (2 - R*s2) = 1/s2 */  \
    __ASM_EMIT("fmul        v1.4s, v25.4s, v17.4s") \
    __ASM_EMIT("fmul        v2.4s, v26.4s, v18.4s") \
    __ASM_EMIT("fmul        v3.4s, v27.4s, v19.4s") \
    __ASM_EMIT("fmul        v0.4s, v0.4s, v4.4s")               /* s1 / s2 */ \
    __ASM_EMIT("fmul        v1.4s, v1.4s, v5.4s") \
    __ASM_EMIT("fmul        v2.4s, v2.4s, v6.4s") \
    __ASM_EMIT("fmul        v3.4s, v3.4s, v7.4s") \
    __ASM_EMIT("stp         q0, q1, [%[" DST "], #0x00]") \
    __ASM_EMIT("stp         q2, q3, [%[" DST "], #0x20]") \
    __ASM_EMIT("subs        %[count], %[count], #16") \
    __ASM_EMIT("add         %[" DST "], %[" DST "], #0x40") \
    __ASM_EMIT("add         %[" SRC "], %[" SRC "], #0x40") \
    __ASM_EMIT("b.hs        1b") \
    /* 8x block */ \
    __ASM_EMIT("2:") \
    __ASM_EMIT("adds        %[count], %[count], #8")            /* 16 - 8 */ \
    __ASM_EMIT("b.lt        4f") \
    __ASM_EMIT("ldp         q0, q1, [%[" SEL(SRC, DST) "], #0x00]") \
    __ASM_EMIT("ldp         q4, q5, [%[" SEL(DST, SRC) "], #0x00]") \
    __ASM_EMIT("frecpe      v16.4s, v0.4s")                     /* v16 = s2 */ \
    __ASM_EMIT("frecpe      v17.4s, v1.4s") \
    __ASM_EMIT("frecps      v24.4s, v16.4s, v0.4s")             /* v24 = (2 - R*s2) */ \
    __ASM_EMIT("frecps      v25.4s, v17.4s, v1.4s") \
    __ASM_EMIT("fmul        v16.4s, v24.4s, v16.4s")            /* v16 = s2' = s2 * (2 - R*s2) */ \
    __ASM_EMIT("fmul        v17.4s, v25.4s, v17.4s") \
    __ASM_EMIT("frecps      v24.4s, v16.4s, v0.4s")             /* v24 = (2 - R*s2') */ \
    __ASM_EMIT("frecps      v25.4s, v17.4s, v1.4s") \
    __ASM_EMIT("fmul        v0.4s, v24.4s, v16.4s")             /* v0  = s2" = s2' * (2 - R*s2) = 1/s2 */  \
    __ASM_EMIT("fmul        v1.4s, v25.4s, v17.4s") \
    __ASM_EMIT("fmul        v0.4s, v0.4s, v4.4s")               /* s1 / s2 */ \
    __ASM_EMIT("fmul        v1.4s, v1.4s, v5.4s") \
    __ASM_EMIT("stp         q0, q1, [%[" DST "], #0x00]") \
    __ASM_EMIT("sub         %[count], %[count], #8") \
    __ASM_EMIT("add         %[" DST "], %[" DST "], #0x20") \
    __ASM_EMIT("add         %[" SRC "], %[" SRC "], #0x20") \
    /* 4x block */ \
    __ASM_EMIT("4:") \
    __ASM_EMIT("adds        %[count], %[count], #4")            /* 8 - 4 */ \
    __ASM_EMIT("b.lt        6f") \
    __ASM_EMIT("ldr         q0, [%[" SEL(SRC, DST) "], #0x00]") \
    __ASM_EMIT("ldr         q4, [%[" SEL(DST, SRC) "], #0x00]") \
    __ASM_EMIT("frecpe      v16.4s, v0.4s")                     /* v16 = s2 */ \
    __ASM_EMIT("frecps      v24.4s, v16.4s, v0.4s")             /* v24 = (2 - R*s2) */ \
    __ASM_EMIT("fmul        v16.4s, v24.4s, v16.4s")            /* v16 = s2' = s2 * (2 - R*s2) */ \
    __ASM_EMIT("frecps      v24.4s, v16.4s, v0.4s")             /* v24 = (2 - R*s2') */ \
    __ASM_EMIT("fmul        v0.4s, v24.4s, v16.4s")             /* v0  = s2" = s2' * (2 - R*s2) = 1/s2 */  \
    __ASM_EMIT("fmul        v0.4s, v0.4s, v4.4s")               /* s1 / s2 */ \
    __ASM_EMIT("str         q0, [%[" DST "], #0x00]") \
    __ASM_EMIT("sub         %[count], %[count], #4") \
    __ASM_EMIT("add         %[" DST "], %[" DST "], #0x10") \
    __ASM_EMIT("add         %[" SRC "], %[" SRC "], #0x10") \
    /* 1x block */ \
    __ASM_EMIT("6:") \
    __ASM_EMIT("adds        %[count], %[count], #3")            /* 4 - 3 */ \
    __ASM_EMIT("b.lt        8f") \
    __ASM_EMIT("7:") \
    __ASM_EMIT("ld1r        {v0.4s}, [%[" SEL(SRC, DST) "]]") \
    __ASM_EMIT("ld1r        {v4.4s}, [%[" SEL(DST, SRC) "]]") \
    __ASM_EMIT("frecpe      v16.4s, v0.4s")                     /* v16 = s2 */ \
    __ASM_EMIT("frecps      v24.4s, v16.4s, v0.4s")             /* v24 = (2 - R*s2) */ \
    __ASM_EMIT("fmul        v16.4s, v24.4s, v16.4s")            /* v16 = s2' = s2 * (2 - R*s2) */ \
    __ASM_EMIT("frecps      v24.4s, v16.4s, v0.4s")             /* v24 = (2 - R*s2') */ \
    __ASM_EMIT("fmul        v0.4s, v24.4s, v16.4s")             /* v0  = s2" = s2' * (2 - R*s2) = 1/s2 */  \
    __ASM_EMIT("fmul        v0.4s, v0.4s, v4.4s")               /* s1 / s2 */ \
    __ASM_EMIT("st1         {v0.s}[0], [%[" DST "]]") \
    __ASM_EMIT("subs        %[count], %[count], #1") \
    __ASM_EMIT("add         %[" DST "], %[" DST "], #0x04") \
    __ASM_EMIT("add         %[" SRC "], %[" SRC "], #0x04") \
    __ASM_EMIT("bge         7b") \
    __ASM_EMIT("8:")

     void div2(float *dst, const float *src, size_t count)
     {
         ARCH_AARCH64_ASM
         (
             OP_DIV2_CORE("dst", "src", OP_SDSEL)
             : [dst] "+r" (dst), [src] "+r" (src),
               [count] "+r" (count)
             :
             : "cc", "memory",
               "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
               "q16", "q17", "q18", "q19",
               "q24", "q25", "q26", "q27"
         );
     }

     void rdiv2(float *dst, const float *src, size_t count)
     {
         ARCH_AARCH64_ASM
         (
             OP_DIV2_CORE("dst", "src", OP_SRSEL)
             : [dst] "+r" (dst), [src] "+r" (src),
               [count] "+r" (count)
             :
             : "cc", "memory",
               "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
               "q16", "q17", "q18", "q19",
               "q24", "q25", "q26", "q27"
         );
     }

#undef OP_DIV2_CORE

#define OP_VV3_CORE(DST, SRC1, SRC2, OP) \
    __ASM_EMIT("subs        %[count], %[count], #32") \
    __ASM_EMIT("b.lo        2f") \
    /* 32x blocks */ \
    __ASM_EMIT("1:") \
    __ASM_EMIT("ldp         q0, q1, [%[" SRC1 "], #0x00]") \
    __ASM_EMIT("ldp         q2, q3, [%[" SRC1 "], #0x20]") \
    __ASM_EMIT("ldp         q4, q5, [%[" SRC1 "], #0x40]") \
    __ASM_EMIT("ldp         q6, q7, [%[" SRC1 "], #0x60]") \
    __ASM_EMIT("ldp         q16, q17, [%[" SRC2 "], #0x00]") \
    __ASM_EMIT("ldp         q18, q19, [%[" SRC2 "], #0x20]") \
    __ASM_EMIT("ldp         q20, q21, [%[" SRC2 "], #0x40]") \
    __ASM_EMIT("ldp         q22, q23, [%[" SRC2 "], #0x60]") \
    __ASM_EMIT(OP "         v0.4s, v0.4s, v16.4s") \
    __ASM_EMIT(OP "         v1.4s, v1.4s, v17.4s") \
    __ASM_EMIT(OP "         v2.4s, v2.4s, v18.4s") \
    __ASM_EMIT(OP "         v3.4s, v3.4s, v19.4s") \
    __ASM_EMIT(OP "         v4.4s, v4.4s, v20.4s") \
    __ASM_EMIT(OP "         v5.4s, v5.4s, v21.4s") \
    __ASM_EMIT(OP "         v6.4s, v6.4s, v22.4s") \
    __ASM_EMIT(OP "         v7.4s, v7.4s, v23.4s") \
    __ASM_EMIT("stp         q0, q1, [%[" DST "], #0x00]") \
    __ASM_EMIT("stp         q2, q3, [%[" DST "], #0x20]") \
    __ASM_EMIT("stp         q4, q5, [%[" DST "], #0x40]") \
    __ASM_EMIT("stp         q6, q7, [%[" DST "], #0x60]") \
    __ASM_EMIT("subs        %[count], %[count], #32") \
    __ASM_EMIT("add         %[" DST "], %[" DST "], #0x80") \
    __ASM_EMIT("add         %[" SRC1 "], %[" SRC1 "], #0x80") \
    __ASM_EMIT("add         %[" SRC2 "], %[" SRC2 "], #0x80") \
    __ASM_EMIT("b.hs        1b") \
    /* 16x block */ \
    __ASM_EMIT("2:") \
    __ASM_EMIT("adds        %[count], %[count], #16") /* 32 - 16 */ \
    __ASM_EMIT("b.lt        4f") \
    __ASM_EMIT("ldp         q0, q1, [%[" SRC1 "], #0x00]") \
    __ASM_EMIT("ldp         q2, q3, [%[" SRC1 "], #0x20]") \
    __ASM_EMIT("ldp         q16, q17, [%[" SRC2 "], #0x00]") \
    __ASM_EMIT("ldp         q18, q19, [%[" SRC2 "], #0x20]") \
    __ASM_EMIT(OP "         v0.4s, v0.4s, v16.4s") \
    __ASM_EMIT(OP "         v1.4s, v1.4s, v17.4s") \
    __ASM_EMIT(OP "         v2.4s, v2.4s, v18.4s") \
    __ASM_EMIT(OP "         v3.4s, v3.4s, v19.4s") \
    __ASM_EMIT("stp         q0, q1, [%[" DST "], #0x00]") \
    __ASM_EMIT("stp         q2, q3, [%[" DST "], #0x20]") \
    __ASM_EMIT("sub         %[count], %[count], #16") \
    __ASM_EMIT("add         %[" DST "], %[" DST "], #0x40") \
    __ASM_EMIT("add         %[" SRC1 "], %[" SRC1 "], #0x40") \
    __ASM_EMIT("add         %[" SRC2 "], %[" SRC2 "], #0x40") \
    /* 8x block */ \
    __ASM_EMIT("4:") \
    __ASM_EMIT("adds        %[count], %[count], #8") /* 16 - 8 */ \
    __ASM_EMIT("b.lt        6f") \
    __ASM_EMIT("ldp         q0, q1, [%[" SRC1 "], #0x00]") \
    __ASM_EMIT("ldp         q16, q17, [%[" SRC2 "], #0x00]") \
    __ASM_EMIT(OP "         v0.4s, v0.4s, v16.4s") \
    __ASM_EMIT(OP "         v1.4s, v1.4s, v17.4s") \
    __ASM_EMIT("stp         q0, q1, [%[" DST "], #0x00]") \
    __ASM_EMIT("sub         %[count], %[count], #8") \
    __ASM_EMIT("add         %[" DST "], %[" DST "], #0x20") \
    __ASM_EMIT("add         %[" SRC1 "], %[" SRC1 "], #0x20") \
    __ASM_EMIT("add         %[" SRC2 "], %[" SRC2 "], #0x20") \
    /* 4x block */ \
    __ASM_EMIT("6:") \
    __ASM_EMIT("adds        %[count], %[count], #4") /* 8 - 4 */ \
    __ASM_EMIT("b.lt        8f") \
    __ASM_EMIT("ldr         q0, [%[" SRC1 "], #0x00]") \
    __ASM_EMIT("ldr         q16, [%[" SRC2 "], #0x00]") \
    __ASM_EMIT(OP "         v0.4s, v0.4s, v16.4s") \
    __ASM_EMIT("str         q0, [%[" DST "], #0x00]") \
    __ASM_EMIT("sub         %[count], %[count], #4") \
    __ASM_EMIT("add         %[" DST "], %[" DST "], #0x10") \
    __ASM_EMIT("add         %[" SRC1 "], %[" SRC1 "], #0x10") \
    __ASM_EMIT("add         %[" SRC2 "], %[" SRC2 "], #0x10") \
    /* 1x block */ \
    __ASM_EMIT("8:") \
    __ASM_EMIT("adds        %[count], %[count], #3") /* 4 - 3 */ \
    __ASM_EMIT("b.lt        10f") \
    __ASM_EMIT("9:") \
    __ASM_EMIT("ld1r        {v0.4s}, [%[" SRC1 "]]") \
    __ASM_EMIT("ld1r        {v16.4s}, [%[" SRC2 "]]") \
    __ASM_EMIT(OP "         v0.4s, v0.4s, v16.4s") \
    __ASM_EMIT("st1         {v0.s}[0], [%[" DST "]]") \
    __ASM_EMIT("subs        %[count], %[count], #1") \
    __ASM_EMIT("add         %[" DST "], %[" DST "], #0x04") \
    __ASM_EMIT("add         %[" SRC1 "], %[" SRC1 "], #0x04") \
    __ASM_EMIT("add         %[" SRC2 "], %[" SRC2 "], #0x04") \
    __ASM_EMIT("bge         9b") \
    __ASM_EMIT("10:")

     void add3(float *dst, const float *src1, const float *src2, size_t count)
     {
         ARCH_AARCH64_ASM
         (
             OP_VV3_CORE("dst", "src1", "src2", "fadd")
             : [dst] "+r" (dst), [src1] "+r" (src1), [src2] "+r" (src2),
               [count] "+r" (count)
             :
             : "cc", "memory",
               "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
               "q16", "q17", "q18", "q19", "q20", "q21", "q22", "q23"
         );
     }

     void sub3(float *dst, const float *src1, const float *src2, size_t count)
     {
         ARCH_AARCH64_ASM
         (
             OP_VV3_CORE("dst", "src1", "src2", "fsub")
             : [dst] "+r" (dst), [src1] "+r" (src1), [src2] "+r" (src2),
               [count] "+r" (count)
             :
             : "cc", "memory",
               "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
               "q16", "q17", "q18", "q19", "q20", "q21", "q22", "q23"
         );
     }

     void mul3(float *dst, const float *src1, const float *src2, size_t count)
     {
         ARCH_AARCH64_ASM
         (
             OP_VV3_CORE("dst", "src1", "src2", "fmul")
             : [dst] "+r" (dst), [src1] "+r" (src1), [src2] "+r" (src2),
               [count] "+r" (count)
             :
             : "cc", "memory",
               "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
               "q16", "q17", "q18", "q19", "q20", "q21", "q22", "q23"
         );
     }

#undef OP_VV3_CORE

#define OP_DIV3_CORE(DST, SRC1, SRC2) \
    __ASM_EMIT("subs        %[count], %[count], #16") \
    __ASM_EMIT("b.lo        2f") \
    /* 16x blocks */ \
    __ASM_EMIT("1:") \
    __ASM_EMIT("ldp         q0, q1, [%[" SRC2 "], #0x00]") \
    __ASM_EMIT("ldp         q2, q3, [%[" SRC2 "], #0x20]") \
    __ASM_EMIT("ldp         q4, q5, [%[" SRC1 "], #0x00]") \
    __ASM_EMIT("ldp         q6, q7, [%[" SRC1 "], #0x20]") \
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
    __ASM_EMIT("fmul        v0.4s, v24.4s, v16.4s")             /* v0  = s2" = s2' * (2 - R*s2) = 1/s2 */  \
    __ASM_EMIT("fmul        v1.4s, v25.4s, v17.4s") \
    __ASM_EMIT("fmul        v2.4s, v26.4s, v18.4s") \
    __ASM_EMIT("fmul        v3.4s, v27.4s, v19.4s") \
    __ASM_EMIT("fmul        v0.4s, v0.4s, v4.4s")               /* s1 / s2 */ \
    __ASM_EMIT("fmul        v1.4s, v1.4s, v5.4s") \
    __ASM_EMIT("fmul        v2.4s, v2.4s, v6.4s") \
    __ASM_EMIT("fmul        v3.4s, v3.4s, v7.4s") \
    __ASM_EMIT("stp         q0, q1, [%[" DST "], #0x00]") \
    __ASM_EMIT("stp         q2, q3, [%[" DST "], #0x20]") \
    __ASM_EMIT("subs        %[count], %[count], #16") \
    __ASM_EMIT("add         %[" DST "], %[" DST "], #0x40") \
    __ASM_EMIT("add         %[" SRC1 "], %[" SRC1 "], #0x40") \
    __ASM_EMIT("add         %[" SRC2 "], %[" SRC2 "], #0x40") \
    __ASM_EMIT("b.hs        1b") \
    /* 8x block */ \
    __ASM_EMIT("2:") \
    __ASM_EMIT("adds        %[count], %[count], #8")            /* 16 - 8 */ \
    __ASM_EMIT("b.lt        4f") \
    __ASM_EMIT("ldp         q0, q1, [%[" SRC2 "], #0x00]") \
    __ASM_EMIT("ldp         q4, q5, [%[" SRC1 "], #0x00]") \
    __ASM_EMIT("frecpe      v16.4s, v0.4s")                     /* v16 = s2 */ \
    __ASM_EMIT("frecpe      v17.4s, v1.4s") \
    __ASM_EMIT("frecps      v24.4s, v16.4s, v0.4s")             /* v24 = (2 - R*s2) */ \
    __ASM_EMIT("frecps      v25.4s, v17.4s, v1.4s") \
    __ASM_EMIT("fmul        v16.4s, v24.4s, v16.4s")            /* v16 = s2' = s2 * (2 - R*s2) */ \
    __ASM_EMIT("fmul        v17.4s, v25.4s, v17.4s") \
    __ASM_EMIT("frecps      v24.4s, v16.4s, v0.4s")             /* v24 = (2 - R*s2') */ \
    __ASM_EMIT("frecps      v25.4s, v17.4s, v1.4s") \
    __ASM_EMIT("fmul        v0.4s, v24.4s, v16.4s")             /* v0  = s2" = s2' * (2 - R*s2) = 1/s2 */  \
    __ASM_EMIT("fmul        v1.4s, v25.4s, v17.4s") \
    __ASM_EMIT("fmul        v0.4s, v0.4s, v4.4s")               /* s1 / s2 */ \
    __ASM_EMIT("fmul        v1.4s, v1.4s, v5.4s") \
    __ASM_EMIT("stp         q0, q1, [%[" DST "], #0x00]") \
    __ASM_EMIT("sub         %[count], %[count], #8") \
    __ASM_EMIT("add         %[" DST "], %[" DST "], #0x20") \
    __ASM_EMIT("add         %[" SRC1 "], %[" SRC1 "], #0x20") \
    __ASM_EMIT("add         %[" SRC2 "], %[" SRC2 "], #0x20") \
    /* 4x block */ \
    __ASM_EMIT("4:") \
    __ASM_EMIT("adds        %[count], %[count], #4")            /* 8 - 4 */ \
    __ASM_EMIT("b.lt        6f") \
    __ASM_EMIT("ldr         q0, [%[" SRC2 "], #0x00]") \
    __ASM_EMIT("ldr         q4, [%[" SRC1 "], #0x00]") \
    __ASM_EMIT("frecpe      v16.4s, v0.4s")                     /* v16 = s2 */ \
    __ASM_EMIT("frecps      v24.4s, v16.4s, v0.4s")             /* v24 = (2 - R*s2) */ \
    __ASM_EMIT("fmul        v16.4s, v24.4s, v16.4s")            /* v16 = s2' = s2 * (2 - R*s2) */ \
    __ASM_EMIT("frecps      v24.4s, v16.4s, v0.4s")             /* v24 = (2 - R*s2') */ \
    __ASM_EMIT("fmul        v0.4s, v24.4s, v16.4s")             /* v0  = s2" = s2' * (2 - R*s2) = 1/s2 */  \
    __ASM_EMIT("fmul        v0.4s, v0.4s, v4.4s")               /* s1 / s2 */ \
    __ASM_EMIT("str         q0, [%[" DST "], #0x00]") \
    __ASM_EMIT("sub         %[count], %[count], #4") \
    __ASM_EMIT("add         %[" DST "], %[" DST "], #0x10") \
    __ASM_EMIT("add         %[" SRC1 "], %[" SRC1 "], #0x10") \
    __ASM_EMIT("add         %[" SRC2 "], %[" SRC2 "], #0x10") \
    /* 1x block */ \
    __ASM_EMIT("6:") \
    __ASM_EMIT("adds        %[count], %[count], #3")            /* 4 - 3 */ \
    __ASM_EMIT("b.lt        8f") \
    __ASM_EMIT("7:") \
    __ASM_EMIT("ld1r        {v0.4s}, [%[" SRC2 "]]") \
    __ASM_EMIT("ld1r        {v4.4s}, [%[" SRC1 "]]") \
    __ASM_EMIT("frecpe      v16.4s, v0.4s")                     /* v16 = s2 */ \
    __ASM_EMIT("frecps      v24.4s, v16.4s, v0.4s")             /* v24 = (2 - R*s2) */ \
    __ASM_EMIT("fmul        v16.4s, v24.4s, v16.4s")            /* v16 = s2' = s2 * (2 - R*s2) */ \
    __ASM_EMIT("frecps      v24.4s, v16.4s, v0.4s")             /* v24 = (2 - R*s2') */ \
    __ASM_EMIT("fmul        v0.4s, v24.4s, v16.4s")             /* v0  = s2" = s2' * (2 - R*s2) = 1/s2 */  \
    __ASM_EMIT("fmul        v0.4s, v0.4s, v4.4s")               /* s1 / s2 */ \
    __ASM_EMIT("st1         {v0.s}[0], [%[" DST "]]") \
    __ASM_EMIT("subs        %[count], %[count], #1") \
    __ASM_EMIT("add         %[" DST "], %[" DST "], #0x04") \
    __ASM_EMIT("add         %[" SRC1 "], %[" SRC1 "], #0x04") \
    __ASM_EMIT("add         %[" SRC2 "], %[" SRC2 "], #0x04") \
    __ASM_EMIT("bge         7b") \
    __ASM_EMIT("8:")

    void div3(float *dst, const float *src1, const float *src2, size_t count)
    {
        ARCH_AARCH64_ASM
        (
            OP_DIV3_CORE("dst", "src1", "src2")
            : [dst] "+r" (dst), [src1] "+r" (src1), [src2] "+r" (src2),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q16", "q17", "q18", "q19",
              "q24", "q25", "q26", "q27"
        );
    }

#undef OP_DIV3_CORE

#define INC_ON(x)           x
#define INC_OFF(x)

#define FMOD_VV_CORE(DST, SRC1, INC1, SRC2, INC2, SEL)   \
    __ASM_EMIT("subs            %[count], %[count], #16") \
    __ASM_EMIT("b.lo            2f") \
    /* 16x blocks */ \
    __ASM_EMIT("1:") \
    __ASM_EMIT("ldp             q0, q1, [%[" SRC1 "], #0x00]") \
    __ASM_EMIT("ldp             q2, q3, [%[" SRC1 "], #0x20]") \
    __ASM_EMIT("ldp             q4, q5, [%[" SRC2 "], #0x00]") \
    __ASM_EMIT("ldp             q6, q7, [%[" SRC2 "], #0x20]") \
    __ASM_EMIT("frecpe          v16.4s, " SEL("v4.4s", "v0.4s"))                    /* v16  = d */ \
    __ASM_EMIT("frecpe          v17.4s, " SEL("v5.4s", "v1.4s")) \
    __ASM_EMIT("frecpe          v18.4s, " SEL("v6.4s", "v2.4s")) \
    __ASM_EMIT("frecpe          v19.4s, " SEL("v7.4s", "v3.4s")) \
    __ASM_EMIT("frecps          v20.4s, v16.4s, " SEL("v4.4s", "v0.4s"))            /* v20  = (2 - R*s2) */ \
    __ASM_EMIT("frecps          v21.4s, v17.4s, " SEL("v5.4s", "v1.4s")) \
    __ASM_EMIT("frecps          v22.4s, v18.4s, " SEL("v6.4s", "v2.4s")) \
    __ASM_EMIT("frecps          v23.4s, v19.4s, " SEL("v7.4s", "v3.4s")) \
    __ASM_EMIT("fmul            v16.4s, v20.4s, v16.4s")                            /* v16  = s2' = s2 * (2 - R*s2) */ \
    __ASM_EMIT("fmul            v17.4s, v21.4s, v17.4s") \
    __ASM_EMIT("fmul            v18.4s, v22.4s, v18.4s") \
    __ASM_EMIT("fmul            v19.4s, v23.4s, v19.4s") \
    __ASM_EMIT("frecps          v20.4s, v16.4s, " SEL("v4.4s", "v0.4s"))            /* v20  = (2 - R*s2') */ \
    __ASM_EMIT("frecps          v21.4s, v17.4s, " SEL("v5.4s", "v1.4s")) \
    __ASM_EMIT("frecps          v22.4s, v18.4s, " SEL("v6.4s", "v2.4s")) \
    __ASM_EMIT("frecps          v23.4s, v19.4s, " SEL("v7.4s", "v3.4s")) \
    __ASM_EMIT("fmul            v16.4s, v20.4s, v16.4s")                            /* v16  = s2" = s2' * (2 - R*s2) = 1/d */  \
    __ASM_EMIT("fmul            v17.4s, v21.4s, v17.4s") \
    __ASM_EMIT("fmul            v18.4s, v22.4s, v18.4s") \
    __ASM_EMIT("fmul            v19.4s, v23.4s, v19.4s") \
    __ASM_EMIT("fmul            v16.4s, v16.4s, " SEL("v0.4s", "v4.4s"))            /* v16  = s/d */  \
    __ASM_EMIT("fmul            v17.4s, v17.4s, " SEL("v1.4s", "v5.4s")) \
    __ASM_EMIT("fmul            v18.4s, v18.4s, " SEL("v2.4s", "v6.4s")) \
    __ASM_EMIT("fmul            v19.4s, v19.4s, " SEL("v3.4s", "v7.4s")) \
    __ASM_EMIT("frintz          v16.4s, v16.4s") \
    __ASM_EMIT("frintz          v17.4s, v17.4s") \
    __ASM_EMIT("frintz          v18.4s, v18.4s") \
    __ASM_EMIT("frintz          v19.4s, v19.4s") \
    __ASM_EMIT("fmls            " SEL("v0.4s", "v4.4s") ", v16.4s, " SEL("v4.4s", "v0.4s")) \
    __ASM_EMIT("fmls            " SEL("v1.4s", "v5.4s") ", v17.4s, " SEL("v5.4s", "v1.4s")) \
    __ASM_EMIT("fmls            " SEL("v2.4s", "v6.4s") ", v18.4s, " SEL("v6.4s", "v2.4s")) \
    __ASM_EMIT("fmls            " SEL("v3.4s", "v7.4s") ", v19.4s, " SEL("v7.4s", "v3.4s")) \
    __ASM_EMIT("stp             " SEL("q0, q1", "q4, q5") ", [%[" DST "], #0x00]") \
    __ASM_EMIT("stp             " SEL("q2, q3", "q6, q7") ", [%[" DST "], #0x20]") \
    __ASM_EMIT("subs            %[count], %[count], #16") \
    __ASM_EMIT(INC1("add        %[" SRC1 "], %[" SRC1 "], #0x40")) \
    __ASM_EMIT(INC2("add        %[" SRC2 "], %[" SRC2 "], #0x40")) \
    __ASM_EMIT("add             %[" DST "], %[" DST "], #0x40") \
    __ASM_EMIT("b.hs            1b") \
    /* 8x block */ \
    __ASM_EMIT("2:") \
    __ASM_EMIT("adds            %[count], %[count], #8") \
    __ASM_EMIT("b.lt            4f") \
    __ASM_EMIT("ldp             q0, q1, [%[" SRC1 "], #0x00]") \
    __ASM_EMIT("ldp             q4, q5, [%[" SRC2 "], #0x00]") \
    __ASM_EMIT("frecpe          v16.4s, " SEL("v4.4s", "v0.4s"))                    /* v16  = d */ \
    __ASM_EMIT("frecpe          v17.4s, " SEL("v5.4s", "v1.4s")) \
    __ASM_EMIT("frecps          v20.4s, v16.4s, " SEL("v4.4s", "v0.4s"))            /* v20  = (2 - R*s2) */ \
    __ASM_EMIT("frecps          v21.4s, v17.4s, " SEL("v5.4s", "v1.4s")) \
    __ASM_EMIT("fmul            v16.4s, v20.4s, v16.4s")                            /* v16  = s2' = s2 * (2 - R*s2) */ \
    __ASM_EMIT("fmul            v17.4s, v21.4s, v17.4s") \
    __ASM_EMIT("frecps          v20.4s, v16.4s, " SEL("v4.4s", "v0.4s"))            /* v20  = (2 - R*s2') */ \
    __ASM_EMIT("frecps          v21.4s, v17.4s, " SEL("v5.4s", "v1.4s")) \
    __ASM_EMIT("fmul            v16.4s, v20.4s, v16.4s")                            /* v16  = s2" = s2' * (2 - R*s2) = 1/d */  \
    __ASM_EMIT("fmul            v17.4s, v21.4s, v17.4s") \
    __ASM_EMIT("fmul            v16.4s, v16.4s, " SEL("v0.4s", "v4.4s"))            /* v16  = s/d */  \
    __ASM_EMIT("fmul            v17.4s, v17.4s, " SEL("v1.4s", "v5.4s")) \
    __ASM_EMIT("frintz          v16.4s, v16.4s") \
    __ASM_EMIT("frintz          v17.4s, v17.4s") \
    __ASM_EMIT("fmls            " SEL("v0.4s", "v4.4s") ", v16.4s, " SEL("v4.4s", "v0.4s")) \
    __ASM_EMIT("fmls            " SEL("v1.4s", "v5.4s") ", v17.4s, " SEL("v5.4s", "v1.4s")) \
    __ASM_EMIT("stp             " SEL("q0, q1", "q4, q5") ", [%[" DST "], #0x00]") \
    __ASM_EMIT("sub             %[count], %[count], #8") \
    __ASM_EMIT(INC1("add        %[" SRC1 "], %[" SRC1 "], #0x20")) \
    __ASM_EMIT(INC2("add        %[" SRC2 "], %[" SRC2 "], #0x20")) \
    __ASM_EMIT("add             %[" DST "], %[" DST "], #0x20") \
    /* 4x blocks */ \
    __ASM_EMIT("4:") \
    __ASM_EMIT("adds            %[count], %[count], #4") \
    __ASM_EMIT("b.lt            6f") \
    __ASM_EMIT("ldr             q0, [%[" SRC1 "]]") \
    __ASM_EMIT("ldr             q4, [%[" SRC2 "]]") \
    __ASM_EMIT("frecpe          v16.4s, " SEL("v4.4s", "v0.4s"))                    /* v16  = d */ \
    __ASM_EMIT("frecps          v20.4s, v16.4s, " SEL("v4.4s", "v0.4s"))            /* v20  = (2 - R*s2) */ \
    __ASM_EMIT("fmul            v16.4s, v20.4s, v16.4s")                            /* v16  = s2' = s2 * (2 - R*s2) */ \
    __ASM_EMIT("frecps          v20.4s, v16.4s, " SEL("v4.4s", "v0.4s"))            /* v20  = (2 - R*s2') */ \
    __ASM_EMIT("fmul            v16.4s, v20.4s, v16.4s")                            /* v16  = s2" = s2' * (2 - R*s2) = 1/d */  \
    __ASM_EMIT("fmul            v16.4s, v16.4s, " SEL("v0.4s", "v4.4s"))            /* v16  = s/d */  \
    __ASM_EMIT("frintz          v16.4s, v16.4s") \
    __ASM_EMIT("fmls            " SEL("v0.4s", "v4.4s") ", v16.4s, " SEL("v4.4s", "v0.4s")) \
    __ASM_EMIT("str             " SEL("q0", "q4") ", [%[" DST "]]") \
    __ASM_EMIT("sub             %[count], %[count], #4") \
    __ASM_EMIT(INC1("add        %[" SRC1 "], %[" SRC1 "], #0x10")) \
    __ASM_EMIT(INC2("add        %[" SRC2 "], %[" SRC2 "], #0x10")) \
    __ASM_EMIT("add             %[" DST "], %[" DST "], #0x10") \
    /* 1x blocks */ \
    __ASM_EMIT("6:") \
    __ASM_EMIT("adds            %[count], %[count], #3") \
    __ASM_EMIT("b.lt            8f") \
    __ASM_EMIT("7:") \
    __ASM_EMIT("ld1r            {v0.4s}, [%[" SRC1 "]]") \
    __ASM_EMIT("ld1r            {v4.4s}, [%[" SRC2 "]]") \
    __ASM_EMIT("frecpe          v16.4s, " SEL("v4.4s", "v0.4s"))                    /* v16  = d */ \
    __ASM_EMIT("frecps          v20.4s, v16.4s, " SEL("v4.4s", "v0.4s"))            /* v20  = (2 - R*s2) */ \
    __ASM_EMIT("fmul            v16.4s, v20.4s, v16.4s")                            /* v16  = s2' = s2 * (2 - R*s2) */ \
    __ASM_EMIT("frecps          v20.4s, v16.4s, " SEL("v4.4s", "v0.4s"))            /* v20  = (2 - R*s2') */ \
    __ASM_EMIT("fmul            v16.4s, v20.4s, v16.4s")                            /* v16  = s2" = s2' * (2 - R*s2) = 1/d */  \
    __ASM_EMIT("fmul            v16.4s, v16.4s, " SEL("v0.4s", "v4.4s"))            /* v16  = s/d */  \
    __ASM_EMIT("frintz          v16.4s, v16.4s") \
    __ASM_EMIT("fmls            " SEL("v0.4s", "v4.4s") ", v16.4s, " SEL("v4.4s", "v0.4s")) \
    __ASM_EMIT("st1             " SEL("{v0.s}[0]", "{v4.s}[0]") ", [%[" DST "]]") \
    __ASM_EMIT("subs            %[count], %[count], #1") \
    __ASM_EMIT(INC1("add        %[" SRC1 "], %[" SRC1 "], #0x04")) \
    __ASM_EMIT(INC2("add        %[" SRC2 "], %[" SRC2 "], #0x04")) \
    __ASM_EMIT("add             %[" DST "], %[" DST "], #0x04") \
    __ASM_EMIT("bge             7b") \
    __ASM_EMIT("8:")

    void mod2(float *dst, const float *src, size_t count)
    {
        ARCH_AARCH64_ASM
        (
            FMOD_VV_CORE("dst", "dst", INC_OFF, "src", INC_ON, OP_SDSEL)
            : [dst] "+r" (dst), [src] "+r" (src),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q16", "q17", "q18", "q19", "q20", "q21", "q22", "q23"
        );
    }

    void rmod2(float *dst, const float *src, size_t count)
    {
        ARCH_AARCH64_ASM
        (
            FMOD_VV_CORE("dst", "dst", INC_OFF, "src", INC_ON, OP_SRSEL)
            : [dst] "+r" (dst), [src] "+r" (src),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q16", "q17", "q18", "q19", "q20", "q21", "q22", "q23"
        );
    }

    void mod3(float *dst, const float *src1, const float *src2, size_t count)
    {
        ARCH_AARCH64_ASM
        (
            FMOD_VV_CORE("dst", "src1", INC_ON, "src2", INC_ON, OP_SDSEL)
            : [dst] "+r" (dst), [src1] "+r" (src1), [src2] "+r" (src2),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q16", "q17", "q18", "q19", "q20", "q21", "q22", "q23"
        );
    }

#undef FMOD_VV_CORE
#undef INC_ON
#undef INC_OFF

#undef OP_SDSEL
#undef OP_SRSEL

}

#endif /* DSP_ARCH_AARCH64_ASIMD_PMATH_OP_VV_H_ */
