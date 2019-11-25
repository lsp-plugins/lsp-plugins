/*
 * abs_vv.h
 *
 *  Created on: 26 нояб. 2019 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_AARCH64_ASIMD_PMATH_ABS_VV_H_
#define DSP_ARCH_AARCH64_ASIMD_PMATH_ABS_VV_H_

#ifndef DSP_ARCH_AARCH64_ASIMD_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_AARCH64_ASIMD_IMPL */

namespace asimd
{
    void abs1(float *dst, size_t count)
    {
        ARCH_AARCH64_ASM
        (
            /* 32x block */
            __ASM_EMIT("subs        %[count], %[count], #32")
            __ASM_EMIT("b.lt        2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("ldp         q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("ldp         q2, q3, [%[dst], #0x20]")
            __ASM_EMIT("ldp         q4, q5, [%[dst], #0x40]")
            __ASM_EMIT("ldp         q6, q7, [%[dst], #0x60]")
            __ASM_EMIT("fabs        v0.4s, v0.4s")
            __ASM_EMIT("fabs        v1.4s, v1.4s")
            __ASM_EMIT("fabs        v2.4s, v2.4s")
            __ASM_EMIT("fabs        v3.4s, v3.4s")
            __ASM_EMIT("fabs        v4.4s, v4.4s")
            __ASM_EMIT("fabs        v5.4s, v5.4s")
            __ASM_EMIT("fabs        v6.4s, v6.4s")
            __ASM_EMIT("fabs        v7.4s, v7.4s")
            __ASM_EMIT("fabs        v8.4s, v8.4s")
            __ASM_EMIT("stp         q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("stp         q2, q3, [%[dst], #0x20]")
            __ASM_EMIT("stp         q4, q5, [%[dst], #0x40]")
            __ASM_EMIT("stp         q6, q7, [%[dst], #0x60]")

            __ASM_EMIT("subs        %[count], %[count], #32")
            __ASM_EMIT("add         %[dst], %[dst], #0x80")
            __ASM_EMIT("b.hs        1b")

            /* 16x block */
            __ASM_EMIT("2:")
            __ASM_EMIT("adds        %[count], %[count], #16") // 32-16
            __ASM_EMIT("b.lt        4f")

            __ASM_EMIT("ldp         q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("ldp         q2, q3, [%[dst], #0x20]")
            __ASM_EMIT("fabs        v0.4s, v0.4s")
            __ASM_EMIT("fabs        v1.4s, v1.4s")
            __ASM_EMIT("fabs        v2.4s, v2.4s")
            __ASM_EMIT("fabs        v3.4s, v3.4s")
            __ASM_EMIT("stp         q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("stp         q2, q3, [%[dst], #0x20]")

            __ASM_EMIT("sub         %[count], %[count], #16")
            __ASM_EMIT("add         %[dst], %[dst], #0x40")

            /* 8x block */
            __ASM_EMIT("4:")
            __ASM_EMIT("adds        %[count], %[count], #8") // 16-8
            __ASM_EMIT("b.lt        6f")
            __ASM_EMIT("ldp         q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("fabs        v0.4s, v0.4s")
            __ASM_EMIT("fabs        v1.4s, v1.4s")
            __ASM_EMIT("stp         q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("sub         %[count], %[count], #8")
            __ASM_EMIT("add         %[dst], %[dst], #0x20")

            /* 4x block */
            __ASM_EMIT("6:")
            __ASM_EMIT("adds        %[count], %[count], #4") // 8-4
            __ASM_EMIT("b.lt        8f")
            __ASM_EMIT("ldr         q0, [%[dst], #0x000]")
            __ASM_EMIT("fabs        v0.4s, v0.4s")
            __ASM_EMIT("str         q0, [%[dst], #0x000]")
            __ASM_EMIT("sub         %[count], %[count], #4")
            __ASM_EMIT("add         %[dst], %[dst], #0x10")

            /* 1x blocks */
            __ASM_EMIT("8:")
            __ASM_EMIT("adds        %[count], %[count], #3") // 4-1
            __ASM_EMIT("b.lt        10f")
            __ASM_EMIT("13:")
            __ASM_EMIT("ld1r        {v0.4s}, [%[dst]]")
            __ASM_EMIT("fabs        v0.4s, v0.4s")
            __ASM_EMIT("subs        %[count], %[count], #1")
            __ASM_EMIT("st1         {v0.s}[0], [%[dst]]")
            __ASM_EMIT("add         %[dst], %[dst], #0x04")
            __ASM_EMIT("b.ge        13b")

            /* End of copy */
            __ASM_EMIT("10:")

            : [dst] "+r" (dst),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7"
        );
    }

    void abs2(float *dst, const float *src, size_t count)
    {
        ARCH_AARCH64_ASM
        (
            /* 32x block */
            __ASM_EMIT("subs        %[count], %[count], #32")
            __ASM_EMIT("b.lt        2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("ldp         q0, q1, [%[src], #0x00]")
            __ASM_EMIT("ldp         q2, q3, [%[src], #0x20]")
            __ASM_EMIT("ldp         q4, q5, [%[src], #0x40]")
            __ASM_EMIT("ldp         q6, q7, [%[src], #0x60]")
            __ASM_EMIT("fabs        v0.4s, v0.4s")
            __ASM_EMIT("fabs        v1.4s, v1.4s")
            __ASM_EMIT("fabs        v2.4s, v2.4s")
            __ASM_EMIT("fabs        v3.4s, v3.4s")
            __ASM_EMIT("fabs        v4.4s, v4.4s")
            __ASM_EMIT("fabs        v5.4s, v5.4s")
            __ASM_EMIT("fabs        v6.4s, v6.4s")
            __ASM_EMIT("fabs        v7.4s, v7.4s")
            __ASM_EMIT("fabs        v8.4s, v8.4s")
            __ASM_EMIT("stp         q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("stp         q2, q3, [%[dst], #0x20]")
            __ASM_EMIT("stp         q4, q5, [%[dst], #0x40]")
            __ASM_EMIT("stp         q6, q7, [%[dst], #0x60]")

            __ASM_EMIT("subs        %[count], %[count], #32")
            __ASM_EMIT("add         %[src], %[src], #0x80")
            __ASM_EMIT("add         %[dst], %[dst], #0x80")
            __ASM_EMIT("b.hs        1b")

            /* 16x block */
            __ASM_EMIT("2:")
            __ASM_EMIT("adds        %[count], %[count], #16") // 32-16
            __ASM_EMIT("b.lt        4f")

            __ASM_EMIT("ldp         q0, q1, [%[src], #0x00]")
            __ASM_EMIT("ldp         q2, q3, [%[src], #0x20]")
            __ASM_EMIT("fabs        v0.4s, v0.4s")
            __ASM_EMIT("fabs        v1.4s, v1.4s")
            __ASM_EMIT("fabs        v2.4s, v2.4s")
            __ASM_EMIT("fabs        v3.4s, v3.4s")
            __ASM_EMIT("stp         q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("stp         q2, q3, [%[dst], #0x20]")

            __ASM_EMIT("sub         %[count], %[count], #16")
            __ASM_EMIT("add         %[src], %[src], #0x40")
            __ASM_EMIT("add         %[dst], %[dst], #0x40")

            /* 8x block */
            __ASM_EMIT("4:")
            __ASM_EMIT("adds        %[count], %[count], #8") // 16-8
            __ASM_EMIT("b.lt        6f")
            __ASM_EMIT("ldp         q0, q1, [%[src], #0x00]")
            __ASM_EMIT("fabs        v0.4s, v0.4s")
            __ASM_EMIT("fabs        v1.4s, v1.4s")
            __ASM_EMIT("stp         q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("sub         %[count], %[count], #8")
            __ASM_EMIT("add         %[src], %[src], #0x20")
            __ASM_EMIT("add         %[dst], %[dst], #0x20")

            /* 4x block */
            __ASM_EMIT("6:")
            __ASM_EMIT("adds        %[count], %[count], #4") // 8-4
            __ASM_EMIT("b.lt        8f")
            __ASM_EMIT("ldr         q0, [%[src], #0x000]")
            __ASM_EMIT("fabs        v0.4s, v0.4s")
            __ASM_EMIT("str         q0, [%[dst], #0x000]")
            __ASM_EMIT("sub         %[count], %[count], #4")
            __ASM_EMIT("add         %[src], %[src], #0x10")
            __ASM_EMIT("add         %[dst], %[dst], #0x10")

            /* 1x blocks */
            __ASM_EMIT("8:")
            __ASM_EMIT("adds        %[count], %[count], #3") // 4-1
            __ASM_EMIT("b.lt        10f")
            __ASM_EMIT("13:")
            __ASM_EMIT("ld1r        {v0.4s}, [%[src]]")
            __ASM_EMIT("fabs        v0.4s, v0.4s")
            __ASM_EMIT("subs        %[count], %[count], #1")
            __ASM_EMIT("st1         {v0.s}[0], [%[dst]]")
            __ASM_EMIT("add         %[src], %[src], #0x04")
            __ASM_EMIT("add         %[dst], %[dst], #0x04")
            __ASM_EMIT("b.ge        13b")

            /* End of copy */
            __ASM_EMIT("10:")

            : [src] "+r" (src), [dst] "+r"(dst),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7"
        );
    }

#define OP_DSEL(a, b)       a
#define OP_RSEL(a, b)       b

#define OP_ABS_VV2_CORE(DST, SRC, OP, SEL) \
    __ASM_EMIT("subs        %[count], %[count], #32") \
    __ASM_EMIT("blo         2f") \
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
    __ASM_EMIT("fabs        " SEL("v16.4s, v16.4s", "v0.4s, v0.4s")) \
    __ASM_EMIT("fabs        " SEL("v17.4s, v17.4s", "v1.4s, v1.4s")) \
    __ASM_EMIT("fabs        " SEL("v18.4s, v18.4s", "v2.4s, v2.4s")) \
    __ASM_EMIT("fabs        " SEL("v19.4s, v19.4s", "v3.4s, v3.4s")) \
    __ASM_EMIT("fabs        " SEL("v20.4s, v20.4s", "v4.4s, v4.4s")) \
    __ASM_EMIT("fabs        " SEL("v21.4s, v21.4s", "v5.4s, v5.4s")) \
    __ASM_EMIT("fabs        " SEL("v22.4s, v22.4s", "v6.4s, v6.4s")) \
    __ASM_EMIT("fabs        " SEL("v23.4s, v23.4s", "v7.4s, v7.4s")) \
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
    __ASM_EMIT("bhs         1b") \
    /* 16x block */ \
    __ASM_EMIT("2:") \
    __ASM_EMIT("adds        %[count], %[count], #16") /* 32 - 16 */ \
    __ASM_EMIT("blt         4f") \
    __ASM_EMIT("ldp         q0, q1, [%[" SEL(DST, SRC) "], #0x00]") \
    __ASM_EMIT("ldp         q2, q3, [%[" SEL(DST, SRC) "], #0x20]") \
    __ASM_EMIT("ldp         q16, q17, [%[" SEL(SRC, DST) "], #0x00]") \
    __ASM_EMIT("ldp         q18, q19, [%[" SEL(SRC, DST) "], #0x20]") \
    __ASM_EMIT("fabs        " SEL("v16.4s, v16.4s", "v0.4s, v0.4s")) \
    __ASM_EMIT("fabs        " SEL("v17.4s, v17.4s", "v1.4s, v1.4s")) \
    __ASM_EMIT("fabs        " SEL("v18.4s, v18.4s", "v2.4s, v2.4s")) \
    __ASM_EMIT("fabs        " SEL("v19.4s, v19.4s", "v3.4s, v3.4s")) \
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
    __ASM_EMIT("blt         6f") \
    __ASM_EMIT("ldp         q0, q1, [%[" SEL(DST, SRC) "], #0x00]") \
    __ASM_EMIT("ldp         q2, q3, [%[" SEL(DST, SRC) "], #0x20]") \
    __ASM_EMIT("ldp         q16, q17, [%[" SEL(SRC, DST) "], #0x00]") \
    __ASM_EMIT("ldp         q18, q19, [%[" SEL(SRC, DST) "], #0x20]") \
    __ASM_EMIT("fabs        " SEL("v16.4s, v16.4s", "v0.4s, v0.4s")) \
    __ASM_EMIT("fabs        " SEL("v17.4s, v17.4s", "v1.4s, v1.4s")) \
    __ASM_EMIT(OP "         v0.4s, v0.4s, v16.4s") \
    __ASM_EMIT(OP "         v1.4s, v1.4s, v17.4s") \
    __ASM_EMIT("stp         q0, q1, [%[" DST "], #0x00]") \
    __ASM_EMIT("sub         %[count], %[count], #8") \
    __ASM_EMIT("add         %[" DST "], %[" DST "], #0x20") \
    __ASM_EMIT("add         %[" SRC "], %[" SRC "], #0x20") \
    /* 4x block */ \
    __ASM_EMIT("6:") \
    __ASM_EMIT("adds        %[count], %[count], #4") /* 8 - 4 */ \
    __ASM_EMIT("blt         8f") \
    __ASM_EMIT("ldr         q0, [%[" SEL(DST, SRC) "], #0x00]") \
    __ASM_EMIT("ldr         q16, [%[" SEL(SRC, DST) "], #0x00]") \
    __ASM_EMIT("fabs        " SEL("v16.4s, v16.4s", "v0.4s, v0.4s")) \
    __ASM_EMIT(OP "         v0.4s, v0.4s, v16.4s") \
    __ASM_EMIT("str         q0, [%[" DST "], #0x00]") \
    __ASM_EMIT("sub         %[count], %[count], #4") \
    __ASM_EMIT("add         %[" DST "], %[" DST "], #0x10") \
    __ASM_EMIT("add         %[" SRC "], %[" SRC "], #0x10") \
    /* 1x block */ \
    __ASM_EMIT("8:") \
    __ASM_EMIT("adds        %[count], %[count], #3") /* 4 - 3 */ \
    __ASM_EMIT("blt         10f") \
    __ASM_EMIT("9:") \
    __ASM_EMIT("ld1r        {v0.4s}, [%[" SEL(DST, SRC) "]]") \
    __ASM_EMIT("ld1r        {v16.4s}, [%[" SEL(SRC, DST) "]]") \
    __ASM_EMIT("fabs        " SEL("v16.4s, v16.4s", "v0.4s, v0.4s")) \
    __ASM_EMIT(OP "         v0.4s, v0.4s, v16.4s") \
    __ASM_EMIT("st1         {v0.s}[0], [%[" DST "]]") \
    __ASM_EMIT("subs        %[count], %[count], #1") \
    __ASM_EMIT("add         %[" DST "], %[" DST "], #0x04") \
    __ASM_EMIT("add         %[" SRC "], %[" SRC "], #0x04") \
    __ASM_EMIT("bge         9b") \
    __ASM_EMIT("10:")

    void abs_add2(float *dst, const float *src, size_t count)
    {
       ARCH_AARCH64_ASM
       (
           OP_ABS_VV2_CORE("dst", "src", "fadd", OP_DSEL)
           : [dst] "+r" (dst), [src] "+r" (src),
             [count] "+r" (count)
           :
           : "cc", "memory",
             "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
             "q16", "q17", "q18", "q19", "q20", "q21", "q22", "q23"
       );
   }

   void abs_sub2(float *dst, const float *src, size_t count)
   {
       ARCH_AARCH64_ASM
       (
           OP_ABS_VV2_CORE("dst", "src", "fsub", OP_DSEL)
           : [dst] "+r" (dst), [src] "+r" (src),
             [count] "+r" (count)
           :
           : "cc", "memory",
             "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
             "q16", "q17", "q18", "q19", "q20", "q21", "q22", "q23"
       );
   }

   void abs_rsub2(float *dst, const float *src, size_t count)
   {
       ARCH_AARCH64_ASM
       (
           OP_ABS_VV2_CORE("dst", "src", "fsub", OP_RSEL)
           : [dst] "+r" (dst), [src] "+r" (src),
             [count] "+r" (count)
           :
           : "cc", "memory",
             "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
             "q16", "q17", "q18", "q19", "q20", "q21", "q22", "q23"
       );
   }

   void abs_mul2(float *dst, const float *src, size_t count)
   {
       ARCH_AARCH64_ASM
       (
           OP_ABS_VV2_CORE("dst", "src", "fmul", OP_DSEL)
           : [dst] "+r" (dst), [src] "+r" (src),
             [count] "+r" (count)
           :
           : "cc", "memory",
             "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
             "q16", "q17", "q18", "q19", "q20", "q21", "q22", "q23"
       );
   }

#undef OP_ABS_VV2_CORE

#define OP_ABS_DIV2_CORE(DST, SRC, SEL) \
    __ASM_EMIT("subs        %[count], %[count], #16") \
    __ASM_EMIT("blo         2f") \
    /* 16x blocks */ \
    __ASM_EMIT("1:") \
    __ASM_EMIT("ldp         q0, q1, [%[" SEL(SRC, DST) "], #0x00]") \
    __ASM_EMIT("ldp         q2, q3, [%[" SEL(SRC, DST) "], #0x20]") \
    __ASM_EMIT("ldp         q4, q5, [%[" SEL(DST, SRC) "], #0x00]") \
    __ASM_EMIT("ldp         q6, q7, [%[" SEL(DST, SRC) "], #0x20]") \
    __ASM_EMIT("fabs        " SEL("v0.4s, v0.4s", "v4.4s, v4.4s")) \
    __ASM_EMIT("fabs        " SEL("v1.4s, v1.4s", "v5.4s, v5.4s")) \
    __ASM_EMIT("fabs        " SEL("v2.4s, v2.4s", "v6.4s, v6.4s")) \
    __ASM_EMIT("fabs        " SEL("v3.4s, v3.4s", "v7.4s, v7.4s")) \
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
    __ASM_EMIT("bhs         1b") \
    /* 8x block */ \
    __ASM_EMIT("2:") \
    __ASM_EMIT("adds        %[count], %[count], #8")            /* 16 - 8 */ \
    __ASM_EMIT("blt         4f") \
    __ASM_EMIT("ldp         q0, q1, [%[" SEL(SRC, DST) "], #0x00]") \
    __ASM_EMIT("ldp         q4, q5, [%[" SEL(DST, SRC) "], #0x00]") \
    __ASM_EMIT("fabs        " SEL("v0.4s, v0.4s", "v4.4s, v4.4s")) \
    __ASM_EMIT("fabs        " SEL("v1.4s, v1.4s", "v5.4s, v5.4s")) \
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
    __ASM_EMIT("blt         6f") \
    __ASM_EMIT("ldr         q0, [%[" SEL(SRC, DST) "], #0x00]") \
    __ASM_EMIT("ldr         q4, [%[" SEL(DST, SRC) "], #0x00]") \
    __ASM_EMIT("fabs        " SEL("v0.4s, v0.4s", "v4.4s, v4.4s")) \
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
    __ASM_EMIT("blt         8f") \
    __ASM_EMIT("7:") \
    __ASM_EMIT("ld1r        {v0.4s}, [%[" SEL(SRC, DST) "]]") \
    __ASM_EMIT("ld1r        {v4.4s}, [%[" SEL(DST, SRC) "]]") \
    __ASM_EMIT("fabs        " SEL("v0.4s, v0.4s", "v4.4s, v4.4s")) \
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

   void abs_div2(float *dst, const float *src, size_t count)
   {
       ARCH_AARCH64_ASM
       (
           OP_ABS_DIV2_CORE("dst", "src", OP_DSEL)
           : [dst] "+r" (dst), [src] "+r" (src),
             [count] "+r" (count)
           :
           : "cc", "memory",
             "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
             "q16", "q17", "q18", "q19",
             "q24", "q25", "q26", "q27"
       );
   }

   void abs_rdiv2(float *dst, const float *src, size_t count)
   {
       ARCH_AARCH64_ASM
       (
           OP_ABS_DIV2_CORE("dst", "src", OP_RSEL)
           : [dst] "+r" (dst), [src] "+r" (src),
             [count] "+r" (count)
           :
           : "cc", "memory",
             "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
             "q16", "q17", "q18", "q19",
             "q24", "q25", "q26", "q27"
       );
   }

#undef OP_ABS_DIV2_CORE

#define OP_ABS_VV3_CORE(DST, SRC1, SRC2, OP, SEL) \
    __ASM_EMIT("subs        %[count], %[count], #32") \
    __ASM_EMIT("blo         2f") \
    /* 32x blocks */ \
    __ASM_EMIT("1:") \
    __ASM_EMIT("ldp         q16, q17, [%[" SRC2 "], #0x00]") \
    __ASM_EMIT("ldp         q18, q19, [%[" SRC2 "], #0x20]") \
    __ASM_EMIT("ldp         q20, q21, [%[" SRC2 "], #0x40]") \
    __ASM_EMIT("ldp         q22, q23, [%[" SRC2 "], #0x60]") \
    __ASM_EMIT("ldp         q0, q1, [%[" SRC1 "], #0x00]") \
    __ASM_EMIT("ldp         q2, q3, [%[" SRC1 "], #0x20]") \
    __ASM_EMIT("ldp         q4, q5, [%[" SRC1 "], #0x40]") \
    __ASM_EMIT("ldp         q6, q7, [%[" SRC1 "], #0x60]") \
    __ASM_EMIT("fabs        v16.4s, v16.4s") \
    __ASM_EMIT("fabs        v17.4s, v17.4s") \
    __ASM_EMIT("fabs        v18.4s, v18.4s") \
    __ASM_EMIT("fabs        v19.4s, v19.4s") \
    __ASM_EMIT("fabs        v20.4s, v20.4s") \
    __ASM_EMIT("fabs        v21.4s, v21.4s") \
    __ASM_EMIT("fabs        v22.4s, v22.4s") \
    __ASM_EMIT("fabs        v23.4s, v23.4s") \
    __ASM_EMIT(OP "         v0.4s, " SEL("v0.4s", "v16.4s") ", " SEL("v16.4s", "v0.4s") ) \
    __ASM_EMIT(OP "         v1.4s, " SEL("v1.4s", "v17.4s") ", " SEL("v17.4s", "v1.4s") ) \
    __ASM_EMIT(OP "         v2.4s, " SEL("v2.4s", "v18.4s") ", " SEL("v18.4s", "v2.4s") ) \
    __ASM_EMIT(OP "         v3.4s, " SEL("v3.4s", "v19.4s") ", " SEL("v19.4s", "v3.4s") ) \
    __ASM_EMIT(OP "         v4.4s, " SEL("v4.4s", "v20.4s") ", " SEL("v20.4s", "v4.4s") ) \
    __ASM_EMIT(OP "         v5.4s, " SEL("v5.4s", "v21.4s") ", " SEL("v21.4s", "v5.4s") ) \
    __ASM_EMIT(OP "         v6.4s, " SEL("v6.4s", "v22.4s") ", " SEL("v22.4s", "v6.4s") ) \
    __ASM_EMIT(OP "         v7.4s, " SEL("v7.4s", "v23.4s") ", " SEL("v23.4s", "v7.4s") ) \
    __ASM_EMIT("stp         q0, q1, [%[" DST "], #0x00]") \
    __ASM_EMIT("stp         q2, q3, [%[" DST "], #0x20]") \
    __ASM_EMIT("stp         q4, q5, [%[" DST "], #0x40]") \
    __ASM_EMIT("stp         q6, q7, [%[" DST "], #0x60]") \
    __ASM_EMIT("subs        %[count], %[count], #32") \
    __ASM_EMIT("add         %[" DST "], %[" DST "], #0x80") \
    __ASM_EMIT("add         %[" SRC1 "], %[" SRC1 "], #0x80") \
    __ASM_EMIT("add         %[" SRC2 "], %[" SRC2 "], #0x80") \
    __ASM_EMIT("bhs         1b") \
    /* 16x block */ \
    __ASM_EMIT("2:") \
    __ASM_EMIT("adds        %[count], %[count], #16") /* 32 - 16 */ \
    __ASM_EMIT("blt         4f") \
    __ASM_EMIT("ldp         q16, q17, [%[" SRC2 "], #0x00]") \
    __ASM_EMIT("ldp         q18, q19, [%[" SRC2 "], #0x20]") \
    __ASM_EMIT("ldp         q0, q1, [%[" SRC1 "], #0x00]") \
    __ASM_EMIT("ldp         q2, q3, [%[" SRC1 "], #0x20]") \
    __ASM_EMIT("fabs        v16.4s, v16.4s") \
    __ASM_EMIT("fabs        v17.4s, v17.4s") \
    __ASM_EMIT("fabs        v18.4s, v18.4s") \
    __ASM_EMIT("fabs        v19.4s, v19.4s") \
    __ASM_EMIT(OP "         v0.4s, " SEL("v0.4s", "v16.4s") ", " SEL("v16.4s", "v0.4s") ) \
    __ASM_EMIT(OP "         v1.4s, " SEL("v1.4s", "v17.4s") ", " SEL("v17.4s", "v1.4s") ) \
    __ASM_EMIT(OP "         v2.4s, " SEL("v2.4s", "v18.4s") ", " SEL("v18.4s", "v2.4s") ) \
    __ASM_EMIT(OP "         v3.4s, " SEL("v3.4s", "v19.4s") ", " SEL("v19.4s", "v3.4s") ) \
    __ASM_EMIT("stp         q0, q1, [%[" DST "], #0x00]") \
    __ASM_EMIT("stp         q2, q3, [%[" DST "], #0x20]") \
    __ASM_EMIT("sub         %[count], %[count], #16") \
    __ASM_EMIT("add         %[" DST "], %[" DST "], #0x40") \
    __ASM_EMIT("add         %[" SRC1 "], %[" SRC1 "], #0x40") \
    __ASM_EMIT("add         %[" SRC2 "], %[" SRC2 "], #0x40") \
    /* 8x block */ \
    __ASM_EMIT("4:") \
    __ASM_EMIT("adds        %[count], %[count], #8") /* 16 - 8 */ \
    __ASM_EMIT("blt         6f") \
    __ASM_EMIT("ldp         q16, q17, [%[" SRC2 "], #0x00]") \
    __ASM_EMIT("ldp         q0, q1, [%[" SRC1 "], #0x00]") \
    __ASM_EMIT("fabs        v16.4s, v16.4s") \
    __ASM_EMIT("fabs        v17.4s, v17.4s") \
    __ASM_EMIT(OP "         v0.4s, " SEL("v0.4s", "v16.4s") ", " SEL("v16.4s", "v0.4s") ) \
    __ASM_EMIT(OP "         v1.4s, " SEL("v1.4s", "v17.4s") ", " SEL("v17.4s", "v1.4s") ) \
    __ASM_EMIT("stp         q0, q1, [%[" DST "], #0x00]") \
    __ASM_EMIT("sub         %[count], %[count], #8") \
    __ASM_EMIT("add         %[" DST "], %[" DST "], #0x20") \
    __ASM_EMIT("add         %[" SRC1 "], %[" SRC1 "], #0x20") \
    __ASM_EMIT("add         %[" SRC2 "], %[" SRC2 "], #0x20") \
    /* 4x block */ \
    __ASM_EMIT("6:") \
    __ASM_EMIT("adds        %[count], %[count], #4") /* 8 - 4 */ \
    __ASM_EMIT("blt         8f") \
    __ASM_EMIT("ldr         q16, [%[" SRC2 "], #0x00]") \
    __ASM_EMIT("ldr         q0, [%[" SRC1 "], #0x00]") \
    __ASM_EMIT("fabs        v16.4s, v16.4s") \
    __ASM_EMIT(OP "         v0.4s, " SEL("v0.4s", "v16.4s") ", " SEL("v16.4s", "v0.4s") ) \
    __ASM_EMIT("str         q0, [%[" DST "], #0x00]") \
    __ASM_EMIT("sub         %[count], %[count], #4") \
    __ASM_EMIT("add         %[" DST "], %[" DST "], #0x10") \
    __ASM_EMIT("add         %[" SRC1 "], %[" SRC1 "], #0x10") \
    __ASM_EMIT("add         %[" SRC2 "], %[" SRC2 "], #0x10") \
    /* 1x block */ \
    __ASM_EMIT("8:") \
    __ASM_EMIT("adds        %[count], %[count], #3") /* 4 - 3 */ \
    __ASM_EMIT("blt         10f") \
    __ASM_EMIT("9:") \
    __ASM_EMIT("ld1r        {v16.4s}, [%[" SRC2 "]]") \
    __ASM_EMIT("ld1r        {v0.4s}, [%[" SRC1 "]]") \
    __ASM_EMIT("fabs        v16.4s, v16.4s") \
    __ASM_EMIT(OP "         v0.4s, " SEL("v0.4s", "v16.4s") ", " SEL("v16.4s", "v0.4s") ) \
    __ASM_EMIT("st1         {v0.s}[0], [%[" DST "]]") \
    __ASM_EMIT("subs        %[count], %[count], #1") \
    __ASM_EMIT("add         %[" DST "], %[" DST "], #0x04") \
    __ASM_EMIT("add         %[" SRC1 "], %[" SRC1 "], #0x04") \
    __ASM_EMIT("add         %[" SRC2 "], %[" SRC2 "], #0x04") \
    __ASM_EMIT("bge         9b") \
    __ASM_EMIT("10:")

     void abs_add3(float *dst, const float *src1, const float *src2, size_t count)
     {
         ARCH_AARCH64_ASM
         (
             OP_ABS_VV3_CORE("dst", "src1", "src2", "fadd", OP_DSEL)
             : [dst] "+r" (dst), [src1] "+r" (src1), [src2] "+r" (src2),
               [count] "+r" (count)
             :
             : "cc", "memory",
               "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
               "q16", "q17", "q18", "q19", "q20", "q21", "q22", "q23"
         );
     }

     void abs_sub3(float *dst, const float *src1, const float *src2, size_t count)
     {
         ARCH_AARCH64_ASM
         (
             OP_ABS_VV3_CORE("dst", "src1", "src2", "fsub", OP_DSEL)
             : [dst] "+r" (dst), [src1] "+r" (src1), [src2] "+r" (src2),
               [count] "+r" (count)
             :
             : "cc", "memory",
               "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
               "q16", "q17", "q18", "q19", "q20", "q21", "q22", "q23"
         );
     }

     void abs_rsub3(float *dst, const float *src1, const float *src2, size_t count)
     {
         ARCH_AARCH64_ASM
         (
             OP_ABS_VV3_CORE("dst", "src1", "src2", "fsub", OP_RSEL)
             : [dst] "+r" (dst), [src1] "+r" (src1), [src2] "+r" (src2),
               [count] "+r" (count)
             :
             : "cc", "memory",
               "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
               "q16", "q17", "q18", "q19", "q20", "q21", "q22", "q23"
         );
     }

     void abs_mul3(float *dst, const float *src1, const float *src2, size_t count)
     {
         ARCH_AARCH64_ASM
         (
             OP_ABS_VV3_CORE("dst", "src1", "src2", "fmul", OP_DSEL)
             : [dst] "+r" (dst), [src1] "+r" (src1), [src2] "+r" (src2),
               [count] "+r" (count)
             :
             : "cc", "memory",
               "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
               "q16", "q17", "q18", "q19", "q20", "q21", "q22", "q23"
         );
     }

#undef OP_ABS_VV3_CORE

#define OP_ABS_DIV3_CORE(DST, SRC1, SRC2, SEL) \
    __ASM_EMIT("subs        %[count], %[count], #16") \
    __ASM_EMIT("blo         2f") \
    /* 16x blocks */ \
    __ASM_EMIT("1:") \
    __ASM_EMIT("ldp         q0, q1, [%[" SEL(SRC2, SRC1) "], #0x00]") \
    __ASM_EMIT("ldp         q2, q3, [%[" SEL(SRC2, SRC1) "], #0x20]") \
    __ASM_EMIT("ldp         q4, q5, [%[" SEL(SRC1, SRC2) "], #0x00]") \
    __ASM_EMIT("ldp         q6, q7, [%[" SEL(SRC1, SRC2) "], #0x20]") \
    __ASM_EMIT("fabs        " SEL("v0.4s, v0.4s", "v4.4s, v4.4s")) \
    __ASM_EMIT("fabs        " SEL("v1.4s, v1.4s", "v5.4s, v5.4s")) \
    __ASM_EMIT("fabs        " SEL("v2.4s, v2.4s", "v6.4s, v6.4s")) \
    __ASM_EMIT("fabs        " SEL("v3.4s, v3.4s", "v7.4s, v7.4s")) \
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
    __ASM_EMIT("bhs         1b") \
    /* 8x block */ \
    __ASM_EMIT("2:") \
    __ASM_EMIT("adds        %[count], %[count], #8")            /* 16 - 8 */ \
    __ASM_EMIT("blt         4f") \
    __ASM_EMIT("ldp         q0, q1, [%[" SEL(SRC2, SRC1) "], #0x00]") \
    __ASM_EMIT("ldp         q4, q5, [%[" SEL(SRC1, SRC2) "], #0x00]") \
    __ASM_EMIT("fabs        " SEL("v0.4s, v0.4s", "v4.4s, v4.4s")) \
    __ASM_EMIT("fabs        " SEL("v1.4s, v1.4s", "v5.4s, v5.4s")) \
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
    __ASM_EMIT("blt         6f") \
    __ASM_EMIT("ldr         q0, [%[" SEL(SRC2, SRC1) "], #0x00]") \
    __ASM_EMIT("ldr         q4, [%[" SEL(SRC1, SRC2) "], #0x00]") \
    __ASM_EMIT("fabs        " SEL("v0.4s, v0.4s", "v4.4s, v4.4s")) \
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
    __ASM_EMIT("blt         8f") \
    __ASM_EMIT("7:") \
    __ASM_EMIT("ld1r        {v0.4s}, [%[" SEL(SRC2, SRC1) "]]") \
    __ASM_EMIT("ld1r        {v4.4s}, [%[" SEL(SRC1, SRC2) "]]") \
    __ASM_EMIT("fabs        " SEL("v0.4s, v0.4s", "v4.4s, v4.4s")) \
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

    void abs_div3(float *dst, const float *src1, const float *src2, size_t count)
    {
        ARCH_AARCH64_ASM
        (
            OP_ABS_DIV3_CORE("dst", "src1", "src2", OP_DSEL)
            : [dst] "+r" (dst), [src1] "+r" (src1), [src2] "+r" (src2),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q16", "q17", "q18", "q19",
              "q24", "q25", "q26", "q27"
        );
    }

    void abs_rdiv3(float *dst, const float *src1, const float *src2, size_t count)
    {
        ARCH_AARCH64_ASM
        (
            OP_ABS_DIV3_CORE("dst", "src1", "src2", OP_RSEL)
            : [dst] "+r" (dst), [src1] "+r" (src1), [src2] "+r" (src2),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q16", "q17", "q18", "q19",
              "q24", "q25", "q26", "q27"
        );
    }

#undef OP_ABS_DIV3_CORE

#undef OP_DSEL
#undef OP_RSEL

}



#endif /* DSP_ARCH_AARCH64_ASIMD_PMATH_ABS_VV_H_ */
