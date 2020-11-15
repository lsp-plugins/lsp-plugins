/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 6 июл. 2020 г.
 *
 * lsp-plugins is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-plugins is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-plugins. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef DSP_ARCH_AARCH64_ASIMD_PMATH_MINMAX_H_
#define DSP_ARCH_AARCH64_ASIMD_PMATH_MINMAX_H_

namespace asimd
{
    #define INC_ON(cmd)     cmd
    #define INC_OFF(cmd)

    #define MINMAX_CORE(DST, A, B, OP, INCA) \
        __ASM_EMIT("subs        %[count], %[count], #32") \
        __ASM_EMIT("b.lo        2f") \
        /* 32x blocks */ \
        __ASM_EMIT("1:") \
        __ASM_EMIT("ldp         q8, q9, [%[" B "], #0x00]") \
        __ASM_EMIT("ldp         q10, q11, [%[" B "], #0x20]") \
        __ASM_EMIT("ldp         q12, q13, [%[" B "], #0x40]") \
        __ASM_EMIT("ldp         q14, q15, [%[" B "], #0x60]") \
        __ASM_EMIT("ldp         q0, q1, [%[" A "], #0x00]") \
        __ASM_EMIT("ldp         q2, q3, [%[" A "], #0x20]") \
        __ASM_EMIT("ldp         q4, q5, [%[" A "], #0x40]") \
        __ASM_EMIT("ldp         q6, q7, [%[" A "], #0x60]") \
        __ASM_EMIT(OP "         v0.4s, v0.4s, v8.4s") \
        __ASM_EMIT(OP "         v1.4s, v1.4s, v9.4s") \
        __ASM_EMIT(OP "         v2.4s, v2.4s, v10.4s") \
        __ASM_EMIT(OP "         v3.4s, v3.4s, v11.4s") \
        __ASM_EMIT(OP "         v4.4s, v4.4s, v12.4s") \
        __ASM_EMIT(OP "         v5.4s, v5.4s, v13.4s") \
        __ASM_EMIT(OP "         v6.4s, v6.4s, v14.4s") \
        __ASM_EMIT(OP "         v7.4s, v7.4s, v15.4s") \
        __ASM_EMIT("subs        %[count], %[count], #32") \
        __ASM_EMIT("stp         q0, q1, [%[" DST "], #0x00]") \
        __ASM_EMIT("stp         q2, q3, [%[" DST "], #0x20]") \
        __ASM_EMIT("stp         q4, q5, [%[" DST "], #0x40]") \
        __ASM_EMIT("stp         q6, q7, [%[" DST "], #0x60]") \
        __ASM_EMIT(INCA("add    %[" A "], %[" A "], #0x80")) \
        __ASM_EMIT("add         %[" B "], %[" B "], #0x80") \
        __ASM_EMIT("add         %[" DST "], %[" DST "], #0x80") \
        __ASM_EMIT("b.hs        1b") \
        /* 16x block */ \
        __ASM_EMIT("2:") \
        __ASM_EMIT("adds        %[count], %[count], #16") /* 32 - 16 */ \
        __ASM_EMIT("b.lt        4f") \
        __ASM_EMIT("ldp         q8, q9, [%[" B "], #0x00]") \
        __ASM_EMIT("ldp         q10, q11, [%[" B "], #0x20]") \
        __ASM_EMIT("ldp         q0, q1, [%[" A "], #0x00]") \
        __ASM_EMIT("ldp         q2, q3, [%[" A "], #0x20]") \
        __ASM_EMIT(OP "         v0.4s, v0.4s, v8.4s") \
        __ASM_EMIT(OP "         v1.4s, v1.4s, v9.4s") \
        __ASM_EMIT(OP "         v2.4s, v2.4s, v10.4s") \
        __ASM_EMIT(OP "         v3.4s, v3.4s, v11.4s") \
        __ASM_EMIT("sub         %[count], %[count], #16") \
        __ASM_EMIT("stp         q0, q1, [%[" DST "], #0x00]") \
        __ASM_EMIT("stp         q2, q3, [%[" DST "], #0x20]") \
        __ASM_EMIT(INCA("add    %[" A "], %[" A "], #0x40")) \
        __ASM_EMIT("add         %[" B "], %[" B "], #0x40") \
        __ASM_EMIT("add         %[" DST "], %[" DST "], #0x40") \
        /* 8x block */ \
        __ASM_EMIT("4:") \
        __ASM_EMIT("adds        %[count], %[count], #8") /* 16 - 8 */ \
        __ASM_EMIT("b.lt        6f") \
        __ASM_EMIT("ldp         q8, q9, [%[" B "], #0x00]") \
        __ASM_EMIT("ldp         q0, q1, [%[" A "], #0x00]") \
        __ASM_EMIT(OP "         v0.4s, v0.4s, v8.4s") \
        __ASM_EMIT(OP "         v1.4s, v1.4s, v9.4s") \
        __ASM_EMIT("sub         %[count], %[count], #8") \
        __ASM_EMIT("stp         q0, q1, [%[" DST "], #0x00]") \
        __ASM_EMIT(INCA("add    %[" A "], %[" A "], #0x20")) \
        __ASM_EMIT("add         %[" B "], %[" B "], #0x20") \
        __ASM_EMIT("add         %[" DST "], %[" DST "], #0x20") \
        /* 4x block */ \
        __ASM_EMIT("6:") \
        __ASM_EMIT("adds        %[count], %[count], #4") /* 8 - 4 */ \
        __ASM_EMIT("b.lt        8f") \
        __ASM_EMIT("ldr         q8, [%[" B "], #0x00]") \
        __ASM_EMIT("ldr         q0, [%[" A "], #0x00]") \
        __ASM_EMIT(OP "         v0.4s, v0.4s, v8.4s") \
        __ASM_EMIT("sub         %[count], %[count], #4") \
        __ASM_EMIT("str         q0, [%[" DST "], #0x00]") \
        __ASM_EMIT(INCA("add    %[" A "], %[" A "], #0x10")) \
        __ASM_EMIT("add         %[" B "], %[" B "], #0x10") \
        __ASM_EMIT("add         %[" DST "], %[" DST "], #0x10") \
        /* 1x block */ \
        __ASM_EMIT("8:") \
        __ASM_EMIT("adds        %[count], %[count], #3") /* 4 - 3 */ \
        __ASM_EMIT("b.lt        10f") \
        __ASM_EMIT("9:") \
        __ASM_EMIT("ld1r        {v8.4s}, [%[" B "]]") \
        __ASM_EMIT("ld1r        {v0.4s}, [%[" A "]]") \
        __ASM_EMIT(OP "         v0.4s, v0.4s, v8.4s") \
        __ASM_EMIT("subs        %[count], %[count], #1") \
        __ASM_EMIT("st1         {v0.s}[0], [%[" DST "]]") \
        __ASM_EMIT(INCA("add    %[" A "], %[" A "], #0x04")) \
        __ASM_EMIT("add         %[" B "], %[" B "], #0x04") \
        __ASM_EMIT("add         %[" DST "], %[" DST "], #0x04") \
        __ASM_EMIT("b.ge        9b") \
        __ASM_EMIT("10:")

    void pmin2(float *dst, const float *src, size_t count)
    {
        ARCH_AARCH64_ASM
        (
            MINMAX_CORE("dst", "dst", "src", "fmin", INC_OFF)
            : [dst] "+r" (dst), [src] "+r" (src),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3",
              "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11",
              "q12", "q13", "q14", "q15"
        );
    }

    void pmin3(float *dst, const float *a, const float *b, size_t count)
    {
        ARCH_AARCH64_ASM
        (
            MINMAX_CORE("dst", "a", "b", "fmin", INC_ON)
            : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3",
              "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11",
              "q12", "q13", "q14", "q15"
        );
    }

    void pmax2(float *dst, const float *src, size_t count)
    {
        ARCH_AARCH64_ASM
        (
            MINMAX_CORE("dst", "dst", "src", "fmax", INC_OFF)
            : [dst] "+r" (dst), [src] "+r" (src),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3",
              "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11",
              "q12", "q13", "q14", "q15"
        );
    }

    void pmax3(float *dst, const float *a, const float *b, size_t count)
    {
        ARCH_AARCH64_ASM
        (
            MINMAX_CORE("dst", "a", "b", "fmax", INC_ON)
            : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3",
              "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11",
              "q12", "q13", "q14", "q15"
        );
    }

    #undef MINMAX_CORE

    #define SIGN_MINMAX_CORE(DST, A, B, OP, INCA) \
        __ASM_EMIT("subs        %[count], %[count], #16") \
        __ASM_EMIT("b.lo        2f") \
        /* 16x blocks */ \
        __ASM_EMIT("1:") \
        __ASM_EMIT("ldp         q4, q5, [%[" B "], #0x00]") \
        __ASM_EMIT("ldp         q6, q7, [%[" B "], #0x20]") \
        __ASM_EMIT("ldp         q0, q1, [%[" A "], #0x00]") \
        __ASM_EMIT("ldp         q2, q3, [%[" A "], #0x20]") \
        __ASM_EMIT("facgt       v8.4s,  v4.4s, v0.4s") \
        __ASM_EMIT("facgt       v9.4s,  v5.4s, v1.4s") \
        __ASM_EMIT("facgt       v10.4s, v6.4s, v2.4s") \
        __ASM_EMIT("facgt       v11.4s, v7.4s, v3.4s") \
        __ASM_EMIT(OP "         v0.16b, v4.16b, v8.16b") \
        __ASM_EMIT(OP "         v1.16b, v5.16b, v9.16b") \
        __ASM_EMIT(OP "         v2.16b, v6.16b, v10.16b") \
        __ASM_EMIT(OP "         v3.16b, v7.16b, v11.16b") \
        __ASM_EMIT("subs        %[count], %[count], #16") \
        __ASM_EMIT("stp         q0, q1, [%[" DST "], #0x00]") \
        __ASM_EMIT("stp         q2, q3, [%[" DST "], #0x20]") \
        __ASM_EMIT(INCA("add    %[" A "], %[" A "], #0x40")) \
        __ASM_EMIT("add         %[" B "], %[" B "], #0x40") \
        __ASM_EMIT("add         %[" DST "], %[" DST "], #0x40") \
        __ASM_EMIT("b.hs        1b") \
        /* 8x block */ \
        __ASM_EMIT("2:") \
        __ASM_EMIT("adds        %[count], %[count], #8") /* 16 - 8 */ \
        __ASM_EMIT("b.lt        4f") \
        __ASM_EMIT("ldp         q4, q5, [%[" B "], #0x00]") \
        __ASM_EMIT("ldp         q0, q1, [%[" A "], #0x00]") \
        __ASM_EMIT("facgt       v8.4s,  v4.4s, v0.4s") \
        __ASM_EMIT("facgt       v9.4s,  v5.4s, v1.4s") \
        __ASM_EMIT(OP "         v0.16b, v4.16b, v8.16b") \
        __ASM_EMIT(OP "         v1.16b, v5.16b, v9.16b") \
        __ASM_EMIT("sub         %[count], %[count], #8") \
        __ASM_EMIT("stp         q0, q1, [%[" DST "], #0x00]") \
        __ASM_EMIT(INCA("add    %[" A "], %[" A "], #0x20")) \
        __ASM_EMIT("add         %[" B "], %[" B "], #0x20") \
        __ASM_EMIT("add         %[" DST "], %[" DST "], #0x20") \
        /* 4x block */ \
        __ASM_EMIT("4:") \
        __ASM_EMIT("adds        %[count], %[count], #4") /* 8 - 4 */ \
        __ASM_EMIT("b.lt        6f") \
        __ASM_EMIT("ldr         q4, [%[" B "], #0x00]") \
        __ASM_EMIT("ldr         q0, [%[" A "], #0x00]") \
        __ASM_EMIT("facgt       v8.4s,  v4.4s, v0.4s") \
        __ASM_EMIT(OP "         v0.16b, v4.16b, v8.16b") \
        __ASM_EMIT("sub         %[count], %[count], #4") \
        __ASM_EMIT("str         q0, [%[" DST "], #0x00]") \
        __ASM_EMIT(INCA("add    %[" A "], %[" A "], #0x10")) \
        __ASM_EMIT("add         %[" B "], %[" B "], #0x10") \
        __ASM_EMIT("add         %[" DST "], %[" DST "], #0x10") \
        /* 1x block */ \
        __ASM_EMIT("6:") \
        __ASM_EMIT("adds        %[count], %[count], #3") /* 4 - 3 */ \
        __ASM_EMIT("b.lt        8f") \
        __ASM_EMIT("7:") \
        __ASM_EMIT("ld1r        {v4.4s}, [%[" B "]]") \
        __ASM_EMIT("ld1r        {v0.4s}, [%[" A "]]") \
        __ASM_EMIT("facgt       v8.4s,  v4.4s, v0.4s") \
        __ASM_EMIT(OP "         v0.16b, v4.16b, v8.16b") \
        __ASM_EMIT("subs        %[count], %[count], #1") \
        __ASM_EMIT("st1         {v0.s}[0], [%[" DST "]]") \
        __ASM_EMIT(INCA("add    %[" A "], %[" A "], #0x04")) \
        __ASM_EMIT("add         %[" B "], %[" B "], #0x04") \
        __ASM_EMIT("add         %[" DST "], %[" DST "], #0x04") \
        __ASM_EMIT("b.ge        7b") \
        __ASM_EMIT("8:")

    void psmin2(float *dst, const float *src, size_t count)
    {
        ARCH_AARCH64_ASM
        (
            SIGN_MINMAX_CORE("dst", "dst", "src", "bif", INC_OFF)
            : [dst] "+r" (dst), [src] "+r" (src),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3",
              "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11",
              "q12", "q13", "q14", "q15"
        );
    }

    void psmin3(float *dst, const float *a, const float *b, size_t count)
    {
        ARCH_AARCH64_ASM
        (
            SIGN_MINMAX_CORE("dst", "a", "b", "bif", INC_ON)
            : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3",
              "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11",
              "q12", "q13", "q14", "q15"
        );
    }

    void psmax2(float *dst, const float *src, size_t count)
    {
        ARCH_AARCH64_ASM
        (
            SIGN_MINMAX_CORE("dst", "dst", "src", "bit", INC_OFF)
            : [dst] "+r" (dst), [src] "+r" (src),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3",
              "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11",
              "q12", "q13", "q14", "q15"
        );
    }

    void psmax3(float *dst, const float *a, const float *b, size_t count)
    {
        ARCH_AARCH64_ASM
        (
            SIGN_MINMAX_CORE("dst", "a", "b", "bit", INC_ON)
            : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3",
              "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11",
              "q12", "q13", "q14", "q15"
        );
    }

    #undef SIGN_MINMAX_CORE

    #define ABS_MINMAX_CORE(DST, A, B, OP, INCA) \
        __ASM_EMIT("subs        %[count], %[count], #32") \
        __ASM_EMIT("b.lo        2f") \
        /* 32x blocks */ \
        __ASM_EMIT("1:") \
        __ASM_EMIT("ldp         q8, q9, [%[" B "], #0x00]") \
        __ASM_EMIT("ldp         q10, q11, [%[" B "], #0x20]") \
        __ASM_EMIT("ldp         q12, q13, [%[" B "], #0x40]") \
        __ASM_EMIT("ldp         q14, q15, [%[" B "], #0x60]") \
        __ASM_EMIT("ldp         q0, q1, [%[" A "], #0x00]") \
        __ASM_EMIT("ldp         q2, q3, [%[" A "], #0x20]") \
        __ASM_EMIT("ldp         q4, q5, [%[" A "], #0x40]") \
        __ASM_EMIT("ldp         q6, q7, [%[" A "], #0x60]") \
        __ASM_EMIT("fabs        v0.4s, v0.4s") \
        __ASM_EMIT("fabs        v1.4s, v1.4s") \
        __ASM_EMIT("fabs        v2.4s, v2.4s") \
        __ASM_EMIT("fabs        v3.4s, v3.4s") \
        __ASM_EMIT("fabs        v4.4s, v4.4s") \
        __ASM_EMIT("fabs        v5.4s, v5.4s") \
        __ASM_EMIT("fabs        v6.4s, v6.4s") \
        __ASM_EMIT("fabs        v7.4s, v7.4s") \
        __ASM_EMIT("fabs        v8.4s, v8.4s") \
        __ASM_EMIT("fabs        v9.4s, v9.4s") \
        __ASM_EMIT("fabs        v10.4s, v10.4s") \
        __ASM_EMIT("fabs        v11.4s, v11.4s") \
        __ASM_EMIT("fabs        v12.4s, v12.4s") \
        __ASM_EMIT("fabs        v13.4s, v13.4s") \
        __ASM_EMIT("fabs        v14.4s, v14.4s") \
        __ASM_EMIT("fabs        v15.4s, v15.4s") \
        __ASM_EMIT(OP "         v0.4s, v0.4s, v8.4s") \
        __ASM_EMIT(OP "         v1.4s, v1.4s, v9.4s") \
        __ASM_EMIT(OP "         v2.4s, v2.4s, v10.4s") \
        __ASM_EMIT(OP "         v3.4s, v3.4s, v11.4s") \
        __ASM_EMIT(OP "         v4.4s, v4.4s, v12.4s") \
        __ASM_EMIT(OP "         v5.4s, v5.4s, v13.4s") \
        __ASM_EMIT(OP "         v6.4s, v6.4s, v14.4s") \
        __ASM_EMIT(OP "         v7.4s, v7.4s, v15.4s") \
        __ASM_EMIT("subs        %[count], %[count], #32") \
        __ASM_EMIT("stp         q0, q1, [%[" DST "], #0x00]") \
        __ASM_EMIT("stp         q2, q3, [%[" DST "], #0x20]") \
        __ASM_EMIT("stp         q4, q5, [%[" DST "], #0x40]") \
        __ASM_EMIT("stp         q6, q7, [%[" DST "], #0x60]") \
        __ASM_EMIT(INCA("add    %[" A "], %[" A "], #0x80")) \
        __ASM_EMIT("add         %[" B "], %[" B "], #0x80") \
        __ASM_EMIT("add         %[" DST "], %[" DST "], #0x80") \
        __ASM_EMIT("b.hs        1b") \
        /* 16x block */ \
        __ASM_EMIT("2:") \
        __ASM_EMIT("adds        %[count], %[count], #16") /* 32 - 16 */ \
        __ASM_EMIT("b.lt        4f") \
        __ASM_EMIT("ldp         q8, q9, [%[" B "], #0x00]") \
        __ASM_EMIT("ldp         q10, q11, [%[" B "], #0x20]") \
        __ASM_EMIT("ldp         q0, q1, [%[" A "], #0x00]") \
        __ASM_EMIT("ldp         q2, q3, [%[" A "], #0x20]") \
        __ASM_EMIT("fabs        v0.4s, v0.4s") \
        __ASM_EMIT("fabs        v1.4s, v1.4s") \
        __ASM_EMIT("fabs        v2.4s, v2.4s") \
        __ASM_EMIT("fabs        v3.4s, v3.4s") \
        __ASM_EMIT("fabs        v8.4s, v8.4s") \
        __ASM_EMIT("fabs        v9.4s, v9.4s") \
        __ASM_EMIT("fabs        v10.4s, v10.4s") \
        __ASM_EMIT("fabs        v11.4s, v11.4s") \
        __ASM_EMIT(OP "         v0.4s, v0.4s, v8.4s") \
        __ASM_EMIT(OP "         v1.4s, v1.4s, v9.4s") \
        __ASM_EMIT(OP "         v2.4s, v2.4s, v10.4s") \
        __ASM_EMIT(OP "         v3.4s, v3.4s, v11.4s") \
        __ASM_EMIT("sub         %[count], %[count], #16") \
        __ASM_EMIT("stp         q0, q1, [%[" DST "], #0x00]") \
        __ASM_EMIT("stp         q2, q3, [%[" DST "], #0x20]") \
        __ASM_EMIT(INCA("add    %[" A "], %[" A "], #0x40")) \
        __ASM_EMIT("add         %[" B "], %[" B "], #0x40") \
        __ASM_EMIT("add         %[" DST "], %[" DST "], #0x40") \
        /* 8x block */ \
        __ASM_EMIT("4:") \
        __ASM_EMIT("adds        %[count], %[count], #8") /* 16 - 8 */ \
        __ASM_EMIT("b.lt        6f") \
        __ASM_EMIT("ldp         q8, q9, [%[" B "], #0x00]") \
        __ASM_EMIT("ldp         q0, q1, [%[" A "], #0x00]") \
        __ASM_EMIT("fabs        v0.4s, v0.4s") \
        __ASM_EMIT("fabs        v1.4s, v1.4s") \
        __ASM_EMIT("fabs        v8.4s, v8.4s") \
        __ASM_EMIT("fabs        v9.4s, v9.4s") \
        __ASM_EMIT(OP "         v0.4s, v0.4s, v8.4s") \
        __ASM_EMIT(OP "         v1.4s, v1.4s, v9.4s") \
        __ASM_EMIT("sub         %[count], %[count], #8") \
        __ASM_EMIT("stp         q0, q1, [%[" DST "], #0x00]") \
        __ASM_EMIT(INCA("add    %[" A "], %[" A "], #0x20")) \
        __ASM_EMIT("add         %[" B "], %[" B "], #0x20") \
        __ASM_EMIT("add         %[" DST "], %[" DST "], #0x20") \
        /* 4x block */ \
        __ASM_EMIT("6:") \
        __ASM_EMIT("adds        %[count], %[count], #4") /* 8 - 4 */ \
        __ASM_EMIT("b.lt        8f") \
        __ASM_EMIT("ldr         q8, [%[" B "], #0x00]") \
        __ASM_EMIT("ldr         q0, [%[" A "], #0x00]") \
        __ASM_EMIT("fabs        v0.4s, v0.4s") \
        __ASM_EMIT("fabs        v8.4s, v8.4s") \
        __ASM_EMIT(OP "         v0.4s, v0.4s, v8.4s") \
        __ASM_EMIT("sub         %[count], %[count], #4") \
        __ASM_EMIT("str         q0, [%[" DST "], #0x00]") \
        __ASM_EMIT(INCA("add    %[" A "], %[" A "], #0x10")) \
        __ASM_EMIT("add         %[" B "], %[" B "], #0x10") \
        __ASM_EMIT("add         %[" DST "], %[" DST "], #0x10") \
        /* 1x block */ \
        __ASM_EMIT("8:") \
        __ASM_EMIT("adds        %[count], %[count], #3") /* 4 - 3 */ \
        __ASM_EMIT("b.lt        10f") \
        __ASM_EMIT("9:") \
        __ASM_EMIT("ld1r        {v8.4s}, [%[" B "]]") \
        __ASM_EMIT("ld1r        {v0.4s}, [%[" A "]]") \
        __ASM_EMIT("fabs        v0.4s, v0.4s") \
        __ASM_EMIT("fabs        v8.4s, v8.4s") \
        __ASM_EMIT(OP "         v0.4s, v0.4s, v8.4s") \
        __ASM_EMIT("subs        %[count], %[count], #1") \
        __ASM_EMIT("st1         {v0.s}[0], [%[" DST "]]") \
        __ASM_EMIT(INCA("add    %[" A "], %[" A "], #0x04")) \
        __ASM_EMIT("add         %[" B "], %[" B "], #0x04") \
        __ASM_EMIT("add         %[" DST "], %[" DST "], #0x04") \
        __ASM_EMIT("b.ge        9b") \
        __ASM_EMIT("10:")

    void pamin2(float *dst, const float *src, size_t count)
    {
        ARCH_AARCH64_ASM
        (
            ABS_MINMAX_CORE("dst", "dst", "src", "fmin", INC_OFF)
            : [dst] "+r" (dst), [src] "+r" (src),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3",
              "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11",
              "q12", "q13", "q14", "q15"
        );
    }

    void pamin3(float *dst, const float *a, const float *b, size_t count)
    {
        ARCH_AARCH64_ASM
        (
            ABS_MINMAX_CORE("dst", "a", "b", "fmin", INC_ON)
            : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3",
              "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11",
              "q12", "q13", "q14", "q15"
        );
    }

    void pamax2(float *dst, const float *src, size_t count)
    {
        ARCH_AARCH64_ASM
        (
            ABS_MINMAX_CORE("dst", "dst", "src", "fmax", INC_OFF)
            : [dst] "+r" (dst), [src] "+r" (src),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3",
              "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11",
              "q12", "q13", "q14", "q15"
        );
    }

    void pamax3(float *dst, const float *a, const float *b, size_t count)
    {
        ARCH_AARCH64_ASM
        (
            ABS_MINMAX_CORE("dst", "a", "b", "fmax", INC_ON)
            : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3",
              "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11",
              "q12", "q13", "q14", "q15"
        );
    }

    #undef ABS_MINMAX_CORE

    #undef INC_ON
    #undef INC_OFF
}



#endif /* DSP_ARCH_AARCH64_ASIMD_PMATH_MINMAX_H_ */
