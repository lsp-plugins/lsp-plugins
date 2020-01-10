/*
 * op_kx.h
 *
 *  Created on: 19 нояб. 2019 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_AARCH64_ASIMD_PMATH_OP_KX_H_
#define DSP_ARCH_AARCH64_ASIMD_PMATH_OP_KX_H_

#ifndef DSP_ARCH_AARCH64_ASIMD_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_AARCH64_ASIMD_IMPL */

namespace asimd
{
#define OP_DORDER(a, b)     a "," b
#define OP_RORDER(a, b)     b "," a

#define OP_K2_CORE(DST, OP, ORDER) \
    __ASM_EMIT("subs        %[count], %[count], #32") \
    __ASM_EMIT("mov         v17.16b, v16.16b") \
    __ASM_EMIT("b.lo        2f") \
    /* 32x blocks */ \
    __ASM_EMIT("1:") \
    __ASM_EMIT("ldp         q0, q1, [%[" DST "], #0x00]") \
    __ASM_EMIT("ldp         q2, q3, [%[" DST "], #0x20]") \
    __ASM_EMIT("ldp         q4, q5, [%[" DST "], #0x40]") \
    __ASM_EMIT("ldp         q6, q7, [%[" DST "], #0x60]") \
    __ASM_EMIT(OP "         v0.4s, " ORDER("v0.4s", "v16.4s")) \
    __ASM_EMIT(OP "         v1.4s, " ORDER("v1.4s", "v17.4s")) \
    __ASM_EMIT(OP "         v2.4s, " ORDER("v2.4s", "v16.4s")) \
    __ASM_EMIT(OP "         v3.4s, " ORDER("v3.4s", "v17.4s")) \
    __ASM_EMIT(OP "         v4.4s, " ORDER("v4.4s", "v16.4s")) \
    __ASM_EMIT(OP "         v5.4s, " ORDER("v5.4s", "v17.4s")) \
    __ASM_EMIT(OP "         v6.4s, " ORDER("v6.4s", "v16.4s")) \
    __ASM_EMIT(OP "         v7.4s, " ORDER("v7.4s", "v17.4s")) \
    __ASM_EMIT("stp         q0, q1, [%[" DST "], #0x00]") \
    __ASM_EMIT("stp         q2, q3, [%[" DST "], #0x20]") \
    __ASM_EMIT("stp         q4, q5, [%[" DST "], #0x40]") \
    __ASM_EMIT("stp         q6, q7, [%[" DST "], #0x60]") \
    __ASM_EMIT("subs        %[count], %[count], #32") \
    __ASM_EMIT("add         %[dst], %[dst], #0x80") \
    __ASM_EMIT("b.hs        1b") \
    /* 16x block */ \
    __ASM_EMIT("2:") \
    __ASM_EMIT("adds        %[count], %[count], #16") /* 32 - 16 */ \
    __ASM_EMIT("b.lt        4f") \
    __ASM_EMIT("ldp         q0, q1, [%[" DST "], #0x00]") \
    __ASM_EMIT("ldp         q2, q3, [%[" DST "], #0x20]") \
    __ASM_EMIT(OP "         v0.4s, " ORDER("v0.4s", "v16.4s")) \
    __ASM_EMIT(OP "         v1.4s, " ORDER("v1.4s", "v17.4s")) \
    __ASM_EMIT(OP "         v2.4s, " ORDER("v2.4s", "v16.4s")) \
    __ASM_EMIT(OP "         v3.4s, " ORDER("v3.4s", "v17.4s")) \
    __ASM_EMIT("stp         q0, q1, [%[" DST "], #0x00]") \
    __ASM_EMIT("stp         q2, q3, [%[" DST "], #0x20]") \
    __ASM_EMIT("sub         %[count], %[count], #16") \
    __ASM_EMIT("add         %[dst], %[dst], #0x40") \
    /* 8x block */ \
    __ASM_EMIT("4:") \
    __ASM_EMIT("adds        %[count], %[count], #8") /* 16 - 8 */ \
    __ASM_EMIT("b.lt        6f") \
    __ASM_EMIT("ldp         q0, q1, [%[" DST "], #0x00]") \
    __ASM_EMIT(OP "         v0.4s, " ORDER("v0.4s", "v16.4s")) \
    __ASM_EMIT(OP "         v1.4s, " ORDER("v1.4s", "v17.4s")) \
    __ASM_EMIT("stp         q0, q1, [%[" DST "], #0x00]") \
    __ASM_EMIT("sub         %[count], %[count], #8") \
    __ASM_EMIT("add         %[dst], %[dst], #0x20") \
    /* 4x block */ \
    __ASM_EMIT("6:") \
    __ASM_EMIT("adds        %[count], %[count], #4") /* 8 - 4 */ \
    __ASM_EMIT("b.lt        8f") \
    __ASM_EMIT("ldr         q0, [%[" DST "], #0x00]") \
    __ASM_EMIT(OP "         v0.4s, " ORDER("v0.4s", "v16.4s")) \
    __ASM_EMIT("str         q0, [%[" DST "], #0x00]") \
    __ASM_EMIT("sub         %[count], %[count], #4") \
    __ASM_EMIT("add         %[dst], %[dst], #0x10") \
    /* 1x block */ \
    __ASM_EMIT("8:") \
    __ASM_EMIT("adds        %[count], %[count], #3") /* 4 - 3 */ \
    __ASM_EMIT("b.lt        10f") \
    __ASM_EMIT("9:") \
    __ASM_EMIT("ld1r        {v0.4s}, [%[" DST "]]") \
    __ASM_EMIT(OP "         v0.4s, " ORDER("v0.4s", "v16.4s")) \
    __ASM_EMIT("st1         {v0.s}[0], [%[" DST "]]") \
    __ASM_EMIT("subs        %[count], %[count], #1") \
    __ASM_EMIT("add         %[dst], %[dst], #0x04") \
    __ASM_EMIT("bge         9b") \
    __ASM_EMIT("10:")

#define OP_K3_CORE(DST, SRC, OP, ORDER) \
    __ASM_EMIT("subs        %[count], %[count], #32") \
    __ASM_EMIT("mov         v17.16b, v16.16b") \
    __ASM_EMIT("b.lo        2f") \
    /* 32x blocks */ \
    __ASM_EMIT("1:") \
    __ASM_EMIT("ldp         q0, q1, [%[" SRC "], #0x00]") \
    __ASM_EMIT("ldp         q2, q3, [%[" SRC "], #0x20]") \
    __ASM_EMIT("ldp         q4, q5, [%[" SRC "], #0x40]") \
    __ASM_EMIT("ldp         q6, q7, [%[" SRC "], #0x60]") \
    __ASM_EMIT(OP "         v0.4s, " ORDER("v0.4s", "v16.4s")) \
    __ASM_EMIT(OP "         v1.4s, " ORDER("v1.4s", "v17.4s")) \
    __ASM_EMIT(OP "         v2.4s, " ORDER("v2.4s", "v16.4s")) \
    __ASM_EMIT(OP "         v3.4s, " ORDER("v3.4s", "v17.4s")) \
    __ASM_EMIT(OP "         v4.4s, " ORDER("v4.4s", "v16.4s")) \
    __ASM_EMIT(OP "         v5.4s, " ORDER("v5.4s", "v17.4s")) \
    __ASM_EMIT(OP "         v6.4s, " ORDER("v6.4s", "v16.4s")) \
    __ASM_EMIT(OP "         v7.4s, " ORDER("v7.4s", "v17.4s")) \
    __ASM_EMIT("stp         q0, q1, [%[" DST "], #0x00]") \
    __ASM_EMIT("stp         q2, q3, [%[" DST "], #0x20]") \
    __ASM_EMIT("stp         q4, q5, [%[" DST "], #0x40]") \
    __ASM_EMIT("stp         q6, q7, [%[" DST "], #0x60]") \
    __ASM_EMIT("subs        %[count], %[count], #32") \
    __ASM_EMIT("add         %[src], %[src], #0x80") \
    __ASM_EMIT("add         %[dst], %[dst], #0x80") \
    __ASM_EMIT("b.hs        1b") \
    /* 16x block */ \
    __ASM_EMIT("2:") \
    __ASM_EMIT("adds        %[count], %[count], #16") /* 32 - 16 */ \
    __ASM_EMIT("b.lt        4f") \
    __ASM_EMIT("ldp         q0, q1, [%[" SRC "], #0x00]") \
    __ASM_EMIT("ldp         q2, q3, [%[" SRC "], #0x20]") \
    __ASM_EMIT(OP "         v0.4s, " ORDER("v0.4s", "v16.4s")) \
    __ASM_EMIT(OP "         v1.4s, " ORDER("v1.4s", "v17.4s")) \
    __ASM_EMIT(OP "         v2.4s, " ORDER("v2.4s", "v16.4s")) \
    __ASM_EMIT(OP "         v3.4s, " ORDER("v3.4s", "v17.4s")) \
    __ASM_EMIT("stp         q0, q1, [%[" DST "], #0x00]") \
    __ASM_EMIT("stp         q2, q3, [%[" DST "], #0x20]") \
    __ASM_EMIT("sub         %[count], %[count], #16") \
    __ASM_EMIT("add         %[src], %[src], #0x40") \
    __ASM_EMIT("add         %[dst], %[dst], #0x40") \
    /* 8x block */ \
    __ASM_EMIT("4:") \
    __ASM_EMIT("adds        %[count], %[count], #8") /* 16 - 8 */ \
    __ASM_EMIT("b.lt        6f") \
    __ASM_EMIT("ldp         q0, q1, [%[" SRC "], #0x00]") \
    __ASM_EMIT(OP "         v0.4s, " ORDER("v0.4s", "v16.4s")) \
    __ASM_EMIT(OP "         v1.4s, " ORDER("v1.4s", "v17.4s")) \
    __ASM_EMIT("stp         q0, q1, [%[" DST "], #0x00]") \
    __ASM_EMIT("sub         %[count], %[count], #8") \
    __ASM_EMIT("add         %[src], %[src], #0x20") \
    __ASM_EMIT("add         %[dst], %[dst], #0x20") \
    /* 4x block */ \
    __ASM_EMIT("6:") \
    __ASM_EMIT("adds        %[count], %[count], #4") /* 8 - 4 */ \
    __ASM_EMIT("b.lt        8f") \
    __ASM_EMIT("ldr         q0, [%[" SRC "], #0x00]") \
    __ASM_EMIT(OP "         v0.4s, " ORDER("v0.4s", "v16.4s")) \
    __ASM_EMIT("str         q0, [%[" DST "], #0x00]") \
    __ASM_EMIT("sub         %[count], %[count], #4") \
    __ASM_EMIT("add         %[src], %[src], #0x10") \
    __ASM_EMIT("add         %[dst], %[dst], #0x10") \
    /* 1x block */ \
    __ASM_EMIT("8:") \
    __ASM_EMIT("adds        %[count], %[count], #3") /* 4 - 3 */ \
    __ASM_EMIT("b.lt        10f") \
    __ASM_EMIT("9:") \
    __ASM_EMIT("ld1r        {v0.4s}, [%[" SRC "]]") \
    __ASM_EMIT(OP "         v0.4s, " ORDER("v0.4s", "v16.4s")) \
    __ASM_EMIT("st1         {v0.s}[0], [%[" DST "]]") \
    __ASM_EMIT("subs        %[count], %[count], #1") \
    __ASM_EMIT("add         %[src], %[src], #0x04") \
    __ASM_EMIT("add         %[dst], %[dst], #0x04") \
    __ASM_EMIT("bge         9b") \
    __ASM_EMIT("10:")

#define OP_DIV2_CORE(DST) \
    __ASM_EMIT("subs        %[count], %[count], #32") \
    __ASM_EMIT("mov         v9.16b, v8.16b") \
    __ASM_EMIT("b.lo        2f") \
    /* 32x blocks */ \
    __ASM_EMIT("1:") \
    __ASM_EMIT("ldp         q0, q1, [%[" DST "], #0x00]") \
    __ASM_EMIT("ldp         q2, q3, [%[" DST "], #0x20]") \
    __ASM_EMIT("ldp         q4, q5, [%[" DST "], #0x40]") \
    __ASM_EMIT("ldp         q6, q7, [%[" DST "], #0x60]") \
    __ASM_EMIT("frecpe      v16.4s, v0.4s")                     /* v16 = s2 */ \
    __ASM_EMIT("frecpe      v17.4s, v1.4s") \
    __ASM_EMIT("frecpe      v18.4s, v2.4s") \
    __ASM_EMIT("frecpe      v19.4s, v3.4s") \
    __ASM_EMIT("frecpe      v20.4s, v4.4s") \
    __ASM_EMIT("frecpe      v21.4s, v5.4s") \
    __ASM_EMIT("frecpe      v22.4s, v6.4s") \
    __ASM_EMIT("frecpe      v23.4s, v7.4s") \
    __ASM_EMIT("frecps      v24.4s, v16.4s, v0.4s")             /* v24 = (2 - R*s2) */ \
    __ASM_EMIT("frecps      v25.4s, v17.4s, v1.4s") \
    __ASM_EMIT("frecps      v26.4s, v18.4s, v2.4s") \
    __ASM_EMIT("frecps      v27.4s, v19.4s, v3.4s") \
    __ASM_EMIT("frecps      v28.4s, v20.4s, v4.4s") \
    __ASM_EMIT("frecps      v29.4s, v21.4s, v5.4s") \
    __ASM_EMIT("frecps      v30.4s, v22.4s, v6.4s") \
    __ASM_EMIT("frecps      v31.4s, v23.4s, v7.4s") \
    __ASM_EMIT("fmul        v16.4s, v24.4s, v16.4s")            /* v16 = s2' = s2 * (2 - R*s2) */ \
    __ASM_EMIT("fmul        v17.4s, v25.4s, v17.4s") \
    __ASM_EMIT("fmul        v18.4s, v26.4s, v18.4s") \
    __ASM_EMIT("fmul        v19.4s, v27.4s, v19.4s") \
    __ASM_EMIT("fmul        v20.4s, v28.4s, v20.4s") \
    __ASM_EMIT("fmul        v21.4s, v29.4s, v21.4s") \
    __ASM_EMIT("fmul        v22.4s, v30.4s, v22.4s") \
    __ASM_EMIT("fmul        v23.4s, v31.4s, v23.4s") \
    __ASM_EMIT("frecps      v24.4s, v16.4s, v0.4s")             /* v24 = (2 - R*s2') */ \
    __ASM_EMIT("frecps      v25.4s, v17.4s, v1.4s") \
    __ASM_EMIT("frecps      v26.4s, v18.4s, v2.4s") \
    __ASM_EMIT("frecps      v27.4s, v19.4s, v3.4s") \
    __ASM_EMIT("frecps      v28.4s, v20.4s, v4.4s") \
    __ASM_EMIT("frecps      v29.4s, v21.4s, v5.4s") \
    __ASM_EMIT("frecps      v30.4s, v22.4s, v6.4s") \
    __ASM_EMIT("frecps      v31.4s, v23.4s, v7.4s") \
    __ASM_EMIT("fmul        v0.4s, v24.4s, v16.4s")             /* v0  = s2" = s2' * (2 - R*s2) = 1/s2 */  \
    __ASM_EMIT("fmul        v1.4s, v25.4s, v17.4s") \
    __ASM_EMIT("fmul        v2.4s, v26.4s, v18.4s") \
    __ASM_EMIT("fmul        v3.4s, v27.4s, v19.4s") \
    __ASM_EMIT("fmul        v4.4s, v28.4s, v20.4s") \
    __ASM_EMIT("fmul        v5.4s, v29.4s, v21.4s") \
    __ASM_EMIT("fmul        v6.4s, v30.4s, v22.4s") \
    __ASM_EMIT("fmul        v7.4s, v31.4s, v23.4s") \
    __ASM_EMIT("fmul        v0.4s, v0.4s, v8.4s")               /* s1 / s2 */ \
    __ASM_EMIT("fmul        v1.4s, v1.4s, v9.4s") \
    __ASM_EMIT("fmul        v2.4s, v2.4s, v8.4s") \
    __ASM_EMIT("fmul        v3.4s, v3.4s, v9.4s") \
    __ASM_EMIT("fmul        v4.4s, v4.4s, v8.4s") \
    __ASM_EMIT("fmul        v5.4s, v5.4s, v9.4s") \
    __ASM_EMIT("fmul        v6.4s, v6.4s, v8.4s") \
    __ASM_EMIT("fmul        v7.4s, v7.4s, v9.4s") \
    __ASM_EMIT("stp         q0, q1, [%[" DST "], #0x00]") \
    __ASM_EMIT("stp         q2, q3, [%[" DST "], #0x20]") \
    __ASM_EMIT("stp         q4, q5, [%[" DST "], #0x40]") \
    __ASM_EMIT("stp         q6, q7, [%[" DST "], #0x60]") \
    __ASM_EMIT("subs        %[count], %[count], #32") \
    __ASM_EMIT("add         %[dst], %[dst], #0x80") \
    __ASM_EMIT("b.hs        1b") \
    /* 16x block */ \
    __ASM_EMIT("2:") \
    __ASM_EMIT("adds        %[count], %[count], #16") /* 32 - 16 */ \
    __ASM_EMIT("b.lt        4f") \
    __ASM_EMIT("ldp         q0, q1, [%[" DST "], #0x00]") \
    __ASM_EMIT("ldp         q2, q3, [%[" DST "], #0x20]") \
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
    __ASM_EMIT("fmul        v0.4s, v0.4s, v8.4s")               /* s1 / s2 */ \
    __ASM_EMIT("fmul        v1.4s, v1.4s, v9.4s") \
    __ASM_EMIT("fmul        v2.4s, v2.4s, v8.4s") \
    __ASM_EMIT("fmul        v3.4s, v3.4s, v9.4s") \
    __ASM_EMIT("stp         q0, q1, [%[" DST "], #0x00]") \
    __ASM_EMIT("stp         q2, q3, [%[" DST "], #0x20]") \
    __ASM_EMIT("sub         %[count], %[count], #16") \
    __ASM_EMIT("add         %[dst], %[dst], #0x40") \
    /* 8x block */ \
    __ASM_EMIT("4:") \
    __ASM_EMIT("adds        %[count], %[count], #8") /* 16 - 8 */ \
    __ASM_EMIT("b.lt        6f") \
    __ASM_EMIT("ldp         q0, q1, [%[" DST "], #0x00]") \
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
    __ASM_EMIT("fmul        v0.4s, v0.4s, v8.4s")               /* s1 / s2 */ \
    __ASM_EMIT("fmul        v1.4s, v1.4s, v9.4s") \
    __ASM_EMIT("stp         q0, q1, [%[" DST "], #0x00]") \
    __ASM_EMIT("sub         %[count], %[count], #8") \
    __ASM_EMIT("add         %[dst], %[dst], #0x20") \
    /* 4x block */ \
    __ASM_EMIT("6:") \
    __ASM_EMIT("adds        %[count], %[count], #4") /* 8 - 4 */ \
    __ASM_EMIT("b.lt        8f") \
    __ASM_EMIT("ldr         q0, [%[" DST "], #0x00]") \
    __ASM_EMIT("frecpe      v16.4s, v0.4s")                     /* v16 = s2 */ \
    __ASM_EMIT("frecps      v24.4s, v16.4s, v0.4s")             /* v24 = (2 - R*s2) */ \
    __ASM_EMIT("fmul        v16.4s, v24.4s, v16.4s")            /* v16 = s2' = s2 * (2 - R*s2) */ \
    __ASM_EMIT("frecps      v24.4s, v16.4s, v0.4s")             /* v24 = (2 - R*s2') */ \
    __ASM_EMIT("fmul        v0.4s, v24.4s, v16.4s")             /* v0  = s2" = s2' * (2 - R*s2) = 1/s2 */  \
    __ASM_EMIT("fmul        v0.4s, v0.4s, v8.4s")               /* s1 / s2 */ \
    __ASM_EMIT("str         q0, [%[" DST "], #0x00]") \
    __ASM_EMIT("sub         %[count], %[count], #4") \
    __ASM_EMIT("add         %[dst], %[dst], #0x10") \
    /* 1x block */ \
    __ASM_EMIT("8:") \
    __ASM_EMIT("adds        %[count], %[count], #3") /* 4 - 3 */ \
    __ASM_EMIT("b.lt        10f") \
    __ASM_EMIT("9:") \
    __ASM_EMIT("ld1r        {v0.4s}, [%[" DST "]]") \
    __ASM_EMIT("frecpe      v16.4s, v0.4s")                     /* v16 = s2 */ \
    __ASM_EMIT("frecps      v24.4s, v16.4s, v0.4s")             /* v24 = (2 - R*s2) */ \
    __ASM_EMIT("fmul        v16.4s, v24.4s, v16.4s")            /* v16 = s2' = s2 * (2 - R*s2) */ \
    __ASM_EMIT("frecps      v24.4s, v16.4s, v0.4s")             /* v24 = (2 - R*s2') */ \
    __ASM_EMIT("fmul        v0.4s, v24.4s, v16.4s")             /* v0  = s2" = s2' * (2 - R*s2) = 1/s2 */  \
    __ASM_EMIT("fmul        v0.4s, v0.4s, v8.4s")               /* s1 / s2 */ \
    __ASM_EMIT("st1         {v0.s}[0], [%[" DST "]]") \
    __ASM_EMIT("subs        %[count], %[count], #1") \
    __ASM_EMIT("add         %[dst], %[dst], #0x04") \
    __ASM_EMIT("bge         9b") \
    __ASM_EMIT("10:")

#define OP_DIV3_CORE(DST, SRC) \
    __ASM_EMIT("subs        %[count], %[count], #32") \
    __ASM_EMIT("mov         v9.16b, v8.16b") \
    __ASM_EMIT("b.lo        2f") \
    /* 32x blocks */ \
    __ASM_EMIT("1:") \
    __ASM_EMIT("ldp         q0, q1, [%[" SRC "], #0x00]") \
    __ASM_EMIT("ldp         q2, q3, [%[" SRC "], #0x20]") \
    __ASM_EMIT("ldp         q4, q5, [%[" SRC "], #0x40]") \
    __ASM_EMIT("ldp         q6, q7, [%[" SRC "], #0x60]") \
    __ASM_EMIT("frecpe      v16.4s, v0.4s")                     /* v16 = s2 */ \
    __ASM_EMIT("frecpe      v17.4s, v1.4s") \
    __ASM_EMIT("frecpe      v18.4s, v2.4s") \
    __ASM_EMIT("frecpe      v19.4s, v3.4s") \
    __ASM_EMIT("frecpe      v20.4s, v4.4s") \
    __ASM_EMIT("frecpe      v21.4s, v5.4s") \
    __ASM_EMIT("frecpe      v22.4s, v6.4s") \
    __ASM_EMIT("frecpe      v23.4s, v7.4s") \
    __ASM_EMIT("frecps      v24.4s, v16.4s, v0.4s")             /* v24 = (2 - R*s2) */ \
    __ASM_EMIT("frecps      v25.4s, v17.4s, v1.4s") \
    __ASM_EMIT("frecps      v26.4s, v18.4s, v2.4s") \
    __ASM_EMIT("frecps      v27.4s, v19.4s, v3.4s") \
    __ASM_EMIT("frecps      v28.4s, v20.4s, v4.4s") \
    __ASM_EMIT("frecps      v29.4s, v21.4s, v5.4s") \
    __ASM_EMIT("frecps      v30.4s, v22.4s, v6.4s") \
    __ASM_EMIT("frecps      v31.4s, v23.4s, v7.4s") \
    __ASM_EMIT("fmul        v16.4s, v24.4s, v16.4s")            /* v16 = s2' = s2 * (2 - R*s2) */ \
    __ASM_EMIT("fmul        v17.4s, v25.4s, v17.4s") \
    __ASM_EMIT("fmul        v18.4s, v26.4s, v18.4s") \
    __ASM_EMIT("fmul        v19.4s, v27.4s, v19.4s") \
    __ASM_EMIT("fmul        v20.4s, v28.4s, v20.4s") \
    __ASM_EMIT("fmul        v21.4s, v29.4s, v21.4s") \
    __ASM_EMIT("fmul        v22.4s, v30.4s, v22.4s") \
    __ASM_EMIT("fmul        v23.4s, v31.4s, v23.4s") \
    __ASM_EMIT("frecps      v24.4s, v16.4s, v0.4s")             /* v24 = (2 - R*s2') */ \
    __ASM_EMIT("frecps      v25.4s, v17.4s, v1.4s") \
    __ASM_EMIT("frecps      v26.4s, v18.4s, v2.4s") \
    __ASM_EMIT("frecps      v27.4s, v19.4s, v3.4s") \
    __ASM_EMIT("frecps      v28.4s, v20.4s, v4.4s") \
    __ASM_EMIT("frecps      v29.4s, v21.4s, v5.4s") \
    __ASM_EMIT("frecps      v30.4s, v22.4s, v6.4s") \
    __ASM_EMIT("frecps      v31.4s, v23.4s, v7.4s") \
    __ASM_EMIT("fmul        v0.4s, v24.4s, v16.4s")             /* v0  = s2" = s2' * (2 - R*s2) = 1/s2 */  \
    __ASM_EMIT("fmul        v1.4s, v25.4s, v17.4s") \
    __ASM_EMIT("fmul        v2.4s, v26.4s, v18.4s") \
    __ASM_EMIT("fmul        v3.4s, v27.4s, v19.4s") \
    __ASM_EMIT("fmul        v4.4s, v28.4s, v20.4s") \
    __ASM_EMIT("fmul        v5.4s, v29.4s, v21.4s") \
    __ASM_EMIT("fmul        v6.4s, v30.4s, v22.4s") \
    __ASM_EMIT("fmul        v7.4s, v31.4s, v23.4s") \
    __ASM_EMIT("fmul        v0.4s, v0.4s, v8.4s")               /* s1 / s2 */ \
    __ASM_EMIT("fmul        v1.4s, v1.4s, v9.4s") \
    __ASM_EMIT("fmul        v2.4s, v2.4s, v8.4s") \
    __ASM_EMIT("fmul        v3.4s, v3.4s, v9.4s") \
    __ASM_EMIT("fmul        v4.4s, v4.4s, v8.4s") \
    __ASM_EMIT("fmul        v5.4s, v5.4s, v9.4s") \
    __ASM_EMIT("fmul        v6.4s, v6.4s, v8.4s") \
    __ASM_EMIT("fmul        v7.4s, v7.4s, v9.4s") \
    __ASM_EMIT("stp         q0, q1, [%[" DST "], #0x00]") \
    __ASM_EMIT("stp         q2, q3, [%[" DST "], #0x20]") \
    __ASM_EMIT("stp         q4, q5, [%[" DST "], #0x40]") \
    __ASM_EMIT("stp         q6, q7, [%[" DST "], #0x60]") \
    __ASM_EMIT("subs        %[count], %[count], #32") \
    __ASM_EMIT("add         %[src], %[src], #0x80") \
    __ASM_EMIT("add         %[dst], %[dst], #0x80") \
    __ASM_EMIT("b.hs        1b") \
    /* 16x block */ \
    __ASM_EMIT("2:") \
    __ASM_EMIT("adds        %[count], %[count], #16") /* 32 - 16 */ \
    __ASM_EMIT("b.lt        4f") \
    __ASM_EMIT("ldp         q0, q1, [%[" SRC "], #0x00]") \
    __ASM_EMIT("ldp         q2, q3, [%[" SRC "], #0x20]") \
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
    __ASM_EMIT("fmul        v0.4s, v0.4s, v8.4s")               /* s1 / s2 */ \
    __ASM_EMIT("fmul        v1.4s, v1.4s, v9.4s") \
    __ASM_EMIT("fmul        v2.4s, v2.4s, v8.4s") \
    __ASM_EMIT("fmul        v3.4s, v3.4s, v9.4s") \
    __ASM_EMIT("stp         q0, q1, [%[" DST "], #0x00]") \
    __ASM_EMIT("stp         q2, q3, [%[" DST "], #0x20]") \
    __ASM_EMIT("sub         %[count], %[count], #16") \
    __ASM_EMIT("add         %[src], %[src], #0x40") \
    __ASM_EMIT("add         %[dst], %[dst], #0x40") \
    /* 8x block */ \
    __ASM_EMIT("4:") \
    __ASM_EMIT("adds        %[count], %[count], #8") /* 16 - 8 */ \
    __ASM_EMIT("b.lt        6f") \
    __ASM_EMIT("ldp         q0, q1, [%[" SRC "], #0x00]") \
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
    __ASM_EMIT("fmul        v0.4s, v0.4s, v8.4s")               /* s1 / s2 */ \
    __ASM_EMIT("fmul        v1.4s, v1.4s, v9.4s") \
    __ASM_EMIT("stp         q0, q1, [%[" DST "], #0x00]") \
    __ASM_EMIT("sub         %[count], %[count], #8") \
    __ASM_EMIT("add         %[src], %[src], #0x20") \
    __ASM_EMIT("add         %[dst], %[dst], #0x20") \
    /* 4x block */ \
    __ASM_EMIT("6:") \
    __ASM_EMIT("adds        %[count], %[count], #4") /* 8 - 4 */ \
    __ASM_EMIT("b.lt        8f") \
    __ASM_EMIT("ldr         q0, [%[" SRC "], #0x00]") \
    __ASM_EMIT("frecpe      v16.4s, v0.4s")                     /* v16 = s2 */ \
    __ASM_EMIT("frecps      v24.4s, v16.4s, v0.4s")             /* v24 = (2 - R*s2) */ \
    __ASM_EMIT("fmul        v16.4s, v24.4s, v16.4s")            /* v16 = s2' = s2 * (2 - R*s2) */ \
    __ASM_EMIT("frecps      v24.4s, v16.4s, v0.4s")             /* v24 = (2 - R*s2') */ \
    __ASM_EMIT("fmul        v0.4s, v24.4s, v16.4s")             /* v0  = s2" = s2' * (2 - R*s2) = 1/s2 */  \
    __ASM_EMIT("fmul        v0.4s, v0.4s, v8.4s")               /* s1 / s2 */ \
    __ASM_EMIT("str         q0, [%[" DST "], #0x00]") \
    __ASM_EMIT("sub         %[count], %[count], #4") \
    __ASM_EMIT("add         %[src], %[src], #0x10") \
    __ASM_EMIT("add         %[dst], %[dst], #0x10") \
    /* 1x block */ \
    __ASM_EMIT("8:") \
    __ASM_EMIT("adds        %[count], %[count], #3") /* 4 - 3 */ \
    __ASM_EMIT("b.lt        10f") \
    __ASM_EMIT("9:") \
    __ASM_EMIT("ld1r        {v0.4s}, [%[" SRC "]]") \
    __ASM_EMIT("frecpe      v16.4s, v0.4s")                     /* v16 = s2 */ \
    __ASM_EMIT("frecps      v24.4s, v16.4s, v0.4s")             /* v24 = (2 - R*s2) */ \
    __ASM_EMIT("fmul        v16.4s, v24.4s, v16.4s")            /* v16 = s2' = s2 * (2 - R*s2) */ \
    __ASM_EMIT("frecps      v24.4s, v16.4s, v0.4s")             /* v24 = (2 - R*s2') */ \
    __ASM_EMIT("fmul        v0.4s, v24.4s, v16.4s")             /* v0  = s2" = s2' * (2 - R*s2) = 1/s2 */  \
    __ASM_EMIT("fmul        v0.4s, v0.4s, v8.4s")               /* s1 / s2 */ \
    __ASM_EMIT("st1         {v0.s}[0], [%[" DST "]]") \
    __ASM_EMIT("subs        %[count], %[count], #1") \
    __ASM_EMIT("add         %[src], %[src], #0x04") \
    __ASM_EMIT("add         %[dst], %[dst], #0x04") \
    __ASM_EMIT("bge         9b") \
    __ASM_EMIT("10:")

    void add_k2(float *dst, float k, size_t count)
    {
        IF_ARCH_AARCH64(float *pk = &k);
        ARCH_AARCH64_ASM
        (
            __ASM_EMIT("ld1r            {v16.4s}, [%[k]]")
            OP_K2_CORE("dst", "fadd", OP_DORDER)
            : [dst] "+r" (dst),
              [count] "+r" (count)
            : [k] "r" (pk)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q16", "q17"
        );
    }

    void sub_k2(float *dst, float k, size_t count)
    {
        IF_ARCH_AARCH64(float *pk = &k);
        ARCH_AARCH64_ASM
        (
            __ASM_EMIT("ld1r            {v16.4s}, [%[k]]")
            OP_K2_CORE("dst", "fsub", OP_DORDER)
            : [dst] "+r" (dst),
              [count] "+r" (count)
            : [k] "r" (pk)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q16", "q17"
        );
    }

    void rsub_k2(float *dst, float k, size_t count)
    {
        IF_ARCH_AARCH64(float *pk = &k);
        ARCH_AARCH64_ASM
        (
            __ASM_EMIT("ld1r            {v16.4s}, [%[k]]")
            OP_K2_CORE("dst", "fsub", OP_RORDER)
            : [dst] "+r" (dst),
              [count] "+r" (count)
            : [k] "r" (pk)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q16", "q17"
        );
    }

    void mul_k2(float *dst, float k, size_t count)
    {
        IF_ARCH_AARCH64(float *pk = &k);
        ARCH_AARCH64_ASM
        (
            __ASM_EMIT("ld1r            {v16.4s}, [%[k]]")
            OP_K2_CORE("dst", "fmul", OP_DORDER)
            : [dst] "+r" (dst),
              [count] "+r" (count)
            : [k] "r" (pk)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q16", "q17"
        );
    }

    void div_k2(float *dst, float k, size_t count)
    {
        IF_ARCH_AARCH64(float *pk = &k);
        ARCH_AARCH64_ASM
        (
            __ASM_EMIT("ld1r            {v0.4s}, [%[k]]")
            __ASM_EMIT("frecpe          v16.4s, v0.4s")                 /* v16 = k */
            __ASM_EMIT("frecps          v17.4s, v16.4s, v0.4s")         /* v17 = (2 - R*k) */
            __ASM_EMIT("fmul            v16.4s, v17.4s, v16.4s")        /* v16 = k' = k * (2 - R*k) */
            __ASM_EMIT("frecps          v17.4s, v16.4s, v0.4s")         /* v17 = (2 - R*k') */
            __ASM_EMIT("fmul            v16.4s, v17.4s, v16.4s")        /* v16 = k" = k' * (2 - R*k) = 1/k */
            OP_K2_CORE("dst", "fmul", OP_DORDER)
            : [dst] "+r" (dst),
              [count] "+r" (count)
            : [k] "r" (pk)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q16", "q17"
        );
    }

    void rdiv_k2(float *dst, float k, size_t count)
    {
        IF_ARCH_AARCH64(float *pk = &k);
        ARCH_AARCH64_ASM
        (
            __ASM_EMIT("ld1r            {v8.4s}, [%[k]]")
            OP_DIV2_CORE("dst")
            : [dst] "+r" (dst),
              [count] "+r" (count)
            : [k] "r" (pk)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9",
              "q16", "q17", "q18", "q19", "q20", "q21", "q22", "q23",
              "q24", "q25", "q26", "q27", "q28", "q29", "q30", "q31"
        );
    }

    void add_k3(float *dst, const float *src, float k, size_t count)
    {
        IF_ARCH_AARCH64(float *pk = &k);
        ARCH_AARCH64_ASM
        (
            __ASM_EMIT("ld1r            {v16.4s}, [%[k]]")
            OP_K3_CORE("dst", "src", "fadd", OP_DORDER)
            : [dst] "+r" (dst), [src] "+r" (src),
              [count] "+r" (count)
            : [k] "r" (pk)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q16", "q17"
        );
    }

    void sub_k3(float *dst, const float *src, float k, size_t count)
    {
        IF_ARCH_AARCH64(float *pk = &k);
        ARCH_AARCH64_ASM
        (
            __ASM_EMIT("ld1r            {v16.4s}, [%[k]]")
            OP_K3_CORE("dst", "src", "fsub", OP_DORDER)
            : [dst] "+r" (dst), [src] "+r" (src),
              [count] "+r" (count)
            : [k] "r" (pk)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q16", "q17"
        );
    }

    void rsub_k3(float *dst, const float *src, float k, size_t count)
    {
        IF_ARCH_AARCH64(float *pk = &k);
        ARCH_AARCH64_ASM
        (
            __ASM_EMIT("ld1r            {v16.4s}, [%[k]]")
            OP_K3_CORE("dst", "src", "fsub", OP_RORDER)
            : [dst] "+r" (dst), [src] "+r" (src),
              [count] "+r" (count)
            : [k] "r" (pk)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q16", "q17"
        );
    }

    void mul_k3(float *dst, const float *src, float k, size_t count)
    {
        IF_ARCH_AARCH64(float *pk = &k);
        ARCH_AARCH64_ASM
        (
            __ASM_EMIT("ld1r            {v16.4s}, [%[k]]")
            OP_K3_CORE("dst", "src", "fmul", OP_DORDER)
            : [dst] "+r" (dst), [src] "+r" (src),
              [count] "+r" (count)
            : [k] "r" (pk)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q16", "q17"
        );
    }

    void div_k3(float *dst, const float *src, float k, size_t count)
    {
        IF_ARCH_AARCH64(float *pk = &k);
        ARCH_AARCH64_ASM
        (
            __ASM_EMIT("ld1r            {v0.4s}, [%[k]]")
            __ASM_EMIT("frecpe          v16.4s, v0.4s")                 /* v16 = k */
            __ASM_EMIT("frecps          v17.4s, v16.4s, v0.4s")         /* v17 = (2 - R*k) */
            __ASM_EMIT("fmul            v16.4s, v17.4s, v16.4s")        /* v16 = k' = k * (2 - R*k) */
            __ASM_EMIT("frecps          v17.4s, v16.4s, v0.4s")         /* v17 = (2 - R*k') */
            __ASM_EMIT("fmul            v16.4s, v17.4s, v16.4s")        /* v16 = k" = k' * (2 - R*k) = 1/k */
            OP_K3_CORE("dst", "src", "fmul", OP_DORDER)
            : [dst] "+r" (dst), [src] "+r" (src),
              [count] "+r" (count)
            : [k] "r" (pk)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q16", "q17"
        );
    }

    void rdiv_k3(float *dst, const float *src, float k, size_t count)
    {
        IF_ARCH_AARCH64(float *pk = &k);
        ARCH_AARCH64_ASM
        (
            __ASM_EMIT("ld1r            {v8.4s}, [%[k]]")
            OP_DIV3_CORE("dst", "src")
            : [dst] "+r" (dst), [src] "+r" (src),
              [count] "+r" (count)
            : [k] "r" (pk)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9",
              "q16", "q17", "q18", "q19", "q20", "q21", "q22", "q23",
              "q24", "q25", "q26", "q27", "q28", "q29", "q30", "q31"
        );
    }

#define INC_ON(x)           x
#define INC_OFF(x)

#define MOD_KX_CORE(DST, SRC, INC)   \
    __ASM_EMIT("subs            %[count], %[count], #16") \
    __ASM_EMIT("frecpe          v16.4s, v4.4s") \
    __ASM_EMIT("mov             v5.16b, v4.16b")                    /* v5   = k */ \
    __ASM_EMIT("frecps          v20.4s, v16.4s, v4.4s") \
    __ASM_EMIT("fmul            v16.4s, v20.4s, v16.4s") \
    __ASM_EMIT("frecps          v20.4s, v16.4s, v4.4s") \
    __ASM_EMIT("fmul            v6.4s, v20.4s, v16.4s")             /* v6   = 1/k */ \
    __ASM_EMIT("mov             v7.16b, v6.16b")                    /* v7   = 1/k */ \
    __ASM_EMIT("b.lo            2f") \
    /* 16x blocks */ \
    __ASM_EMIT("1:") \
    __ASM_EMIT("ldp             q0, q1, [%[" SRC "], #0x00]") \
    __ASM_EMIT("ldp             q2, q3, [%[" SRC "], #0x20]") \
    __ASM_EMIT("fmul            v16.4s, v0.4s, v6.4s")              /* v0   = x/k */  \
    __ASM_EMIT("fmul            v17.4s, v1.4s, v7.4s") \
    __ASM_EMIT("fmul            v18.4s, v2.4s, v6.4s") \
    __ASM_EMIT("fmul            v19.4s, v3.4s, v7.4s") \
    __ASM_EMIT("frintz          v16.4s, v16.4s") \
    __ASM_EMIT("frintz          v17.4s, v17.4s") \
    __ASM_EMIT("frintz          v18.4s, v18.4s") \
    __ASM_EMIT("frintz          v19.4s, v19.4s") \
    __ASM_EMIT("fmls            v0.4s, v16.4s, v4.4s") \
    __ASM_EMIT("fmls            v1.4s, v17.4s, v5.4s") \
    __ASM_EMIT("fmls            v2.4s, v18.4s, v4.4s") \
    __ASM_EMIT("fmls            v3.4s, v19.4s, v5.4s") \
    __ASM_EMIT("stp             q0, q1, [%[" DST "], #0x00]") \
    __ASM_EMIT("stp             q2, q3, [%[" DST "], #0x20]") \
    __ASM_EMIT("subs            %[count], %[count], #16") \
    __ASM_EMIT(INC("add         %[" SRC "], %[" SRC "], #0x40")) \
    __ASM_EMIT("add             %[" DST "], %[" DST "], #0x40") \
    __ASM_EMIT("b.hs            1b") \
    /* 8x block */ \
    __ASM_EMIT("2:") \
    __ASM_EMIT("adds            %[count], %[count], #8") \
    __ASM_EMIT("b.lt            4f") \
    __ASM_EMIT("ldp             q0, q1, [%[" SRC "], #0x00]") \
    __ASM_EMIT("fmul            v16.4s, v0.4s, v6.4s")              /* v0   = x/k */  \
    __ASM_EMIT("fmul            v17.4s, v1.4s, v7.4s") \
    __ASM_EMIT("frintz          v16.4s, v16.4s") \
    __ASM_EMIT("frintz          v17.4s, v17.4s") \
    __ASM_EMIT("fmls            v0.4s, v16.4s, v4.4s") \
    __ASM_EMIT("fmls            v1.4s, v17.4s, v5.4s") \
    __ASM_EMIT("stp             q0, q1, [%[" DST "], #0x00]") \
    __ASM_EMIT("sub             %[count], %[count], #8") \
    __ASM_EMIT(INC("add         %[" SRC "], %[" SRC "], #0x20")) \
    __ASM_EMIT("add             %[" DST "], %[" DST "], #0x20") \
    /* 4x blocks */ \
    __ASM_EMIT("4:") \
    __ASM_EMIT("adds            %[count], %[count], #4") \
    __ASM_EMIT("b.lt            6f") \
    __ASM_EMIT("ldr             q0, [%[" SRC "]]") \
    __ASM_EMIT("fmul            v16.4s, v0.4s, v6.4s")              /* v0   = x/k */  \
    __ASM_EMIT("frintz          v16.4s, v16.4s") \
    __ASM_EMIT("fmls            v0.4s, v16.4s, v4.4s") \
    __ASM_EMIT("str             q0, [%[" DST "]]") \
    __ASM_EMIT("sub             %[count], %[count], #4") \
    __ASM_EMIT(INC("add         %[" SRC "], %[" SRC "], #0x10")) \
    __ASM_EMIT("add             %[" DST "], %[" DST "], #0x10") \
    /* 1x blocks */ \
    __ASM_EMIT("6:") \
    __ASM_EMIT("adds            %[count], %[count], #3") \
    __ASM_EMIT("b.lt            8f") \
    __ASM_EMIT("7:") \
    __ASM_EMIT("ld1r            {v0.4s}, [%[" SRC "]]") \
    __ASM_EMIT("fmul            v16.4s, v0.4s, v6.4s")              /* v0   = x/k */  \
    __ASM_EMIT("frintz          v16.4s, v16.4s") \
    __ASM_EMIT("fmls            v0.4s, v16.4s, v4.4s") \
    __ASM_EMIT("st1             {v0.s}[0], [%[" DST "]]") \
    __ASM_EMIT("subs            %[count], %[count], #1") \
    __ASM_EMIT(INC("add         %[" SRC "], %[" SRC "], #0x04")) \
    __ASM_EMIT("add             %[" DST "], %[" DST "], #0x04") \
    __ASM_EMIT("bge             7b") \
    __ASM_EMIT("8:")

#define RMOD_KX_CORE(DST, SRC, INC)   \
    __ASM_EMIT("subs            %[count], %[count], #16") \
    __ASM_EMIT("mov             v5.16b, v4.16b")                    /* v5   = k */ \
    __ASM_EMIT("b.lo            2f") \
    /* 16x blocks */ \
    __ASM_EMIT("1:") \
    __ASM_EMIT("ldp             q0, q1, [%[" SRC "], #0x00]") \
    __ASM_EMIT("ldp             q2, q3, [%[" SRC "], #0x20]") \
    __ASM_EMIT("frecpe          v16.4s, v0.4s")                     /* v16  = s2 */ \
    __ASM_EMIT("frecpe          v17.4s, v1.4s") \
    __ASM_EMIT("frecpe          v18.4s, v2.4s") \
    __ASM_EMIT("frecpe          v19.4s, v3.4s") \
    __ASM_EMIT("frecps          v20.4s, v16.4s, v0.4s")             /* v20  = (2 - R*s2) */ \
    __ASM_EMIT("frecps          v21.4s, v17.4s, v1.4s") \
    __ASM_EMIT("frecps          v22.4s, v18.4s, v2.4s") \
    __ASM_EMIT("frecps          v23.4s, v19.4s, v3.4s") \
    __ASM_EMIT("fmul            v16.4s, v20.4s, v16.4s")            /* v16  = s2' = s2 * (2 - R*s2) */ \
    __ASM_EMIT("fmul            v17.4s, v21.4s, v17.4s") \
    __ASM_EMIT("fmul            v18.4s, v22.4s, v18.4s") \
    __ASM_EMIT("fmul            v19.4s, v23.4s, v19.4s") \
    __ASM_EMIT("frecps          v20.4s, v16.4s, v0.4s")             /* v20  = (2 - R*s2') */ \
    __ASM_EMIT("frecps          v21.4s, v17.4s, v1.4s") \
    __ASM_EMIT("frecps          v22.4s, v18.4s, v2.4s") \
    __ASM_EMIT("frecps          v23.4s, v19.4s, v3.4s") \
    __ASM_EMIT("fmul            v16.4s, v20.4s, v16.4s")            /* v16  = s2" = s2' * (2 - R*s2) = 1/x */  \
    __ASM_EMIT("fmul            v17.4s, v21.4s, v17.4s") \
    __ASM_EMIT("fmul            v18.4s, v22.4s, v18.4s") \
    __ASM_EMIT("fmul            v19.4s, v23.4s, v19.4s") \
    __ASM_EMIT("fmul            v16.4s, v16.4s, v4.4s")             /* v16  = k/x */ \
    __ASM_EMIT("fmul            v17.4s, v17.4s, v5.4s") \
    __ASM_EMIT("fmul            v18.4s, v18.4s, v4.4s") \
    __ASM_EMIT("fmul            v19.4s, v19.4s, v5.4s") \
    __ASM_EMIT("frintz          v16.4s, v16.4s") \
    __ASM_EMIT("frintz          v17.4s, v17.4s") \
    __ASM_EMIT("frintz          v18.4s, v18.4s") \
    __ASM_EMIT("frintz          v19.4s, v19.4s") \
    __ASM_EMIT("fmul            v0.4s, v0.4s, v16.4s")              /* v0   = x * int(k/x) */ \
    __ASM_EMIT("fmul            v1.4s, v1.4s, v17.4s") \
    __ASM_EMIT("fmul            v2.4s, v2.4s, v18.4s") \
    __ASM_EMIT("fmul            v3.4s, v3.4s, v19.4s") \
    __ASM_EMIT("fsub            v0.4s, v4.4s, v0.4s")               /* v0   = k - x * int(k/x) */ \
    __ASM_EMIT("fsub            v1.4s, v5.4s, v1.4s") \
    __ASM_EMIT("fsub            v2.4s, v4.4s, v2.4s") \
    __ASM_EMIT("fsub            v3.4s, v5.4s, v3.4s") \
    __ASM_EMIT("stp             q0, q1, [%[" DST "], #0x00]") \
    __ASM_EMIT("stp             q2, q3, [%[" DST "], #0x20]") \
    __ASM_EMIT("subs            %[count], %[count], #16") \
    __ASM_EMIT(INC("add         %[" SRC "], %[" SRC "], #0x40")) \
    __ASM_EMIT("add             %[" DST "], %[" DST "], #0x40") \
    __ASM_EMIT("b.hs            1b") \
    /* 8x block */ \
    __ASM_EMIT("2:") \
    __ASM_EMIT("adds            %[count], %[count], #8") \
    __ASM_EMIT("b.lt            4f") \
    __ASM_EMIT("ldp             q0, q1, [%[" SRC "], #0x00]") \
    __ASM_EMIT("frecpe          v16.4s, v0.4s")                     /* v16  = s2 */ \
    __ASM_EMIT("frecpe          v17.4s, v1.4s") \
    __ASM_EMIT("frecps          v20.4s, v16.4s, v0.4s")             /* v20  = (2 - R*s2) */ \
    __ASM_EMIT("frecps          v21.4s, v17.4s, v1.4s") \
    __ASM_EMIT("fmul            v16.4s, v20.4s, v16.4s")            /* v16  = s2' = s2 * (2 - R*s2) */ \
    __ASM_EMIT("fmul            v17.4s, v21.4s, v17.4s") \
    __ASM_EMIT("frecps          v20.4s, v16.4s, v0.4s")             /* v20  = (2 - R*s2') */ \
    __ASM_EMIT("frecps          v21.4s, v17.4s, v1.4s") \
    __ASM_EMIT("fmul            v16.4s, v20.4s, v16.4s")            /* v16  = s2" = s2' * (2 - R*s2) = 1/x */  \
    __ASM_EMIT("fmul            v17.4s, v21.4s, v17.4s") \
    __ASM_EMIT("fmul            v16.4s, v16.4s, v4.4s")             /* v16  = k/x */ \
    __ASM_EMIT("fmul            v17.4s, v17.4s, v5.4s") \
    __ASM_EMIT("frintz          v16.4s, v16.4s") \
    __ASM_EMIT("frintz          v17.4s, v17.4s") \
    __ASM_EMIT("fmul            v0.4s, v0.4s, v16.4s")              /* v0   = x * int(k/x) */ \
    __ASM_EMIT("fmul            v1.4s, v1.4s, v17.4s") \
    __ASM_EMIT("fsub            v0.4s, v4.4s, v0.4s")               /* v0   = k - x * int(k/x) */ \
    __ASM_EMIT("fsub            v1.4s, v5.4s, v1.4s") \
    __ASM_EMIT("stp             q0, q1, [%[" DST "], #0x00]") \
    __ASM_EMIT("sub             %[count], %[count], #8") \
    __ASM_EMIT(INC("add         %[" SRC "], %[" SRC "], #0x20")) \
    __ASM_EMIT("add             %[" DST "], %[" DST "], #0x20") \
    /* 4x blocks */ \
    __ASM_EMIT("4:") \
    __ASM_EMIT("adds            %[count], %[count], #4") \
    __ASM_EMIT("b.lt            6f") \
    __ASM_EMIT("ldr             q0, [%[" SRC "]]") \
    __ASM_EMIT("frecpe          v16.4s, v0.4s")                     /* v16  = s2 */ \
    __ASM_EMIT("frecps          v20.4s, v16.4s, v0.4s")             /* v20  = (2 - R*s2) */ \
    __ASM_EMIT("fmul            v16.4s, v20.4s, v16.4s")            /* v16  = s2' = s2 * (2 - R*s2) */ \
    __ASM_EMIT("frecps          v20.4s, v16.4s, v0.4s")             /* v20  = (2 - R*s2') */ \
    __ASM_EMIT("fmul            v16.4s, v20.4s, v16.4s")            /* v16  = s2" = s2' * (2 - R*s2) = 1/x */  \
    __ASM_EMIT("fmul            v16.4s, v16.4s, v4.4s")             /* v16  = k/x */ \
    __ASM_EMIT("frintz          v16.4s, v16.4s") \
    __ASM_EMIT("fmul            v0.4s, v0.4s, v16.4s")              /* v0   = x * int(k/x) */ \
    __ASM_EMIT("fsub            v0.4s, v4.4s, v0.4s")               /* v0   = k - x * int(k/x) */ \
    __ASM_EMIT("str             q0, [%[" DST "]]") \
    __ASM_EMIT("sub             %[count], %[count], #4") \
    __ASM_EMIT(INC("add         %[" SRC "], %[" SRC "], #0x10")) \
    __ASM_EMIT("add             %[" DST "], %[" DST "], #0x10") \
    /* 1x blocks */ \
    __ASM_EMIT("6:") \
    __ASM_EMIT("adds            %[count], %[count], #3") \
    __ASM_EMIT("b.lt            8f") \
    __ASM_EMIT("7:") \
    __ASM_EMIT("ld1r            {v0.4s}, [%[" SRC "]]") \
    __ASM_EMIT("frecpe          v16.4s, v0.4s")                     /* v16  = s2 */ \
    __ASM_EMIT("frecps          v20.4s, v16.4s, v0.4s")             /* v20  = (2 - R*s2) */ \
    __ASM_EMIT("fmul            v16.4s, v20.4s, v16.4s")            /* v16  = s2' = s2 * (2 - R*s2) */ \
    __ASM_EMIT("frecps          v20.4s, v16.4s, v0.4s")             /* v20  = (2 - R*s2') */ \
    __ASM_EMIT("fmul            v16.4s, v20.4s, v16.4s")            /* v16  = s2" = s2' * (2 - R*s2) = 1/x */  \
    __ASM_EMIT("fmul            v16.4s, v16.4s, v4.4s")             /* v16  = k/x */ \
    __ASM_EMIT("frintz          v16.4s, v16.4s") \
    __ASM_EMIT("fmul            v0.4s, v0.4s, v16.4s")              /* v0   = x * int(k/x) */ \
    __ASM_EMIT("fsub            v0.4s, v4.4s, v0.4s")               /* v0   = k - x * int(k/x) */ \
    __ASM_EMIT("st1             {v0.s}[0], [%[" DST "]]") \
    __ASM_EMIT("subs            %[count], %[count], #1") \
    __ASM_EMIT(INC("add         %[" SRC "], %[" SRC "], #0x04")) \
    __ASM_EMIT("add             %[" DST "], %[" DST "], #0x04") \
    __ASM_EMIT("bge             7b") \
    __ASM_EMIT("8:")

    void mod_k2(float *dst, float k, size_t count)
    {
        IF_ARCH_AARCH64(float *pk = &k);
        ARCH_AARCH64_ASM
        (
            __ASM_EMIT("ld1r       {v4.4s}, [%[k]]")
            MOD_KX_CORE("dst", "dst", INC_OFF)
            : [dst] "+r" (dst),
              [count] "+r" (count)
            : [k] "r" (pk)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q16", "q17", "q18", "q19", "q20", "q21", "q22", "q23"
        );
    }

    void rmod_k2(float *dst, float k, size_t count)
    {
        IF_ARCH_AARCH64(float *pk = &k);
        ARCH_AARCH64_ASM
        (
            __ASM_EMIT("ld1r        {v4.4s}, [%[k]]")
            RMOD_KX_CORE("dst", "dst", INC_OFF)
            : [dst] "+r" (dst),
              [count] "+r" (count)
            : [k] "r" (pk)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q16", "q17", "q18", "q19", "q20", "q21", "q22", "q23"
        );
    }

    void mod_k3(float *dst, const float *src, float k, size_t count)
    {
        IF_ARCH_AARCH64(float *pk = &k);
        ARCH_AARCH64_ASM
        (
            __ASM_EMIT("ld1r        {v4.4s}, [%[k]]")
            MOD_KX_CORE("dst", "src", INC_ON)
            : [dst] "+r" (dst), [src] "+r" (src),
              [count] "+r" (count)
            : [k] "r" (pk)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q16", "q17", "q18", "q19", "q20", "q21", "q22", "q23"
        );
    }

    void rmod_k3(float *dst, const float *src, float k, size_t count)
    {
        IF_ARCH_AARCH64(float *pk = &k);
        ARCH_AARCH64_ASM
        (
            __ASM_EMIT("ld1r        {v4.4s}, [%[k]]")
            RMOD_KX_CORE("dst", "src", INC_ON)
            : [dst] "+r" (dst), [src] "+r" (src),
              [count] "+r" (count)
            : [k] "r" (pk)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q16", "q17", "q18", "q19", "q20", "q21", "q22", "q23"
        );
    }

#undef INC_ON
#undef INC_OFF

#undef OP_DORDER
#undef OP_RORDER
#undef OP_K2_CORE
#undef OP_K3_CORE
#undef OP_DIV2_CORE
#undef OP_DIV3_CORE
#undef MOD_KX_CORE
#undef RMOD_KX_CORE
}

#endif /* DSP_ARCH_AARCH64_ASIMD_PMATH_OP_KX_H_ */
