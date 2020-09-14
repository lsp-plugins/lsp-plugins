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

#ifndef DSP_ARCH_ARM_NEON_D32_PMATH_MINMAX_H_
#define DSP_ARCH_ARM_NEON_D32_PMATH_MINMAX_H_

namespace neon_d32
{
    #define MINMAX_CORE(DST, A, B, OP, INCA) \
        __ASM_EMIT("subs        %[count], $32") \
        __ASM_EMIT("blo         2f") \
        /* 32x blocks */ \
        __ASM_EMIT("1:") \
        __ASM_EMIT("vldm        %[" B "]!, {q8-q15}") \
        __ASM_EMIT("vldm        %[" A "]" INCA ", {q0-q7}") \
        __ASM_EMIT(OP ".f32     q0, q0, q8") \
        __ASM_EMIT(OP ".f32     q1, q1, q9") \
        __ASM_EMIT(OP ".f32     q2, q2, q10") \
        __ASM_EMIT(OP ".f32     q3, q3, q11") \
        __ASM_EMIT(OP ".f32     q4, q4, q12") \
        __ASM_EMIT(OP ".f32     q5, q5, q13") \
        __ASM_EMIT(OP ".f32     q6, q6, q14") \
        __ASM_EMIT(OP ".f32     q7, q7, q15") \
        __ASM_EMIT("subs        %[count], $32") \
        __ASM_EMIT("vstm        %[" DST "]!, {q0-q7}") \
        __ASM_EMIT("bhs         1b") \
        /* 16x block */ \
        __ASM_EMIT("2:") \
        __ASM_EMIT("adds        %[count], $16") /* 32 - 16 */ \
        __ASM_EMIT("blt         4f") \
        __ASM_EMIT("vldm        %[" B "]!, {q8-q11}") \
        __ASM_EMIT("vldm        %[" A "]" INCA ", {q0-q3}") \
        __ASM_EMIT(OP ".f32     q0, q0, q8") \
        __ASM_EMIT(OP ".f32     q1, q1, q9") \
        __ASM_EMIT(OP ".f32     q2, q2, q10") \
        __ASM_EMIT(OP ".f32     q3, q3, q11") \
        __ASM_EMIT("sub         %[count], $16") \
        __ASM_EMIT("vstm        %[" DST "]!, {q0-q3}") \
        /* 8x block */ \
        __ASM_EMIT("4:") \
        __ASM_EMIT("adds        %[count], $8") /* 16 - 8 */ \
        __ASM_EMIT("blt         6f") \
        __ASM_EMIT("vldm        %[" B "]!, {q8-q9}") \
        __ASM_EMIT("vldm        %[" A "]" INCA ", {q0-q1}") \
        __ASM_EMIT(OP ".f32     q0, q0, q8") \
        __ASM_EMIT(OP ".f32     q1, q1, q9") \
        __ASM_EMIT("sub         %[count], $8") \
        __ASM_EMIT("vstm        %[" DST "]!, {q0-q1}") \
        /* 4x block */ \
        __ASM_EMIT("6:") \
        __ASM_EMIT("adds        %[count], $4") /* 8 - 4 */ \
        __ASM_EMIT("blt         8f") \
        __ASM_EMIT("vldm        %[" B "]!, {q8}") \
        __ASM_EMIT("vldm        %[" A "]" INCA ", {q0}") \
        __ASM_EMIT(OP ".f32     q0, q0, q8") \
        __ASM_EMIT("sub         %[count], $4") \
        __ASM_EMIT("vstm        %[" DST "]!, {q0}") \
        /* 1x block */ \
        __ASM_EMIT("8:") \
        __ASM_EMIT("adds        %[count], $3") /* 4 - 3 */ \
        __ASM_EMIT("blt         10f") \
        __ASM_EMIT("9:") \
        __ASM_EMIT("vld1.32     {d16[], d17[]}, [%[" B "]]!") \
        __ASM_EMIT("vld1.32     {d0[], d1[]}, [%[" A "]]" INCA) \
        __ASM_EMIT(OP ".f32     q0, q0, q8") \
        __ASM_EMIT("subs        %[count], $1") \
        __ASM_EMIT("vst1.32     {d0[0]}, [%[" DST "]]!") \
        __ASM_EMIT("bge         9b") \
        __ASM_EMIT("10:")

    void pmin2(float *dst, const float *src, size_t count)
    {
        ARCH_ARM_ASM
        (
            MINMAX_CORE("dst", "dst", "src", "vmin", "")
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
        ARCH_ARM_ASM
        (
            MINMAX_CORE("dst", "a", "b", "vmin", "!")
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
        ARCH_ARM_ASM
        (
            MINMAX_CORE("dst", "dst", "src", "vmax", "")
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
        ARCH_ARM_ASM
        (
            MINMAX_CORE("dst", "a", "b", "vmax", "!")
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
        __ASM_EMIT("subs        %[count], $16") \
        __ASM_EMIT("blo         2f") \
        /* 16x blocks */ \
        __ASM_EMIT("1:") \
        __ASM_EMIT("vldm        %[" B "]!, {q4-q7}") \
        __ASM_EMIT("vldm        %[" A "]" INCA ", {q0-q3}") \
        __ASM_EMIT("vacgt.f32   q8,  q4, q0") \
        __ASM_EMIT("vacgt.f32   q9,  q5, q1") \
        __ASM_EMIT("vacgt.f32   q10, q6, q2") \
        __ASM_EMIT("vacgt.f32   q11, q7, q3") \
        __ASM_EMIT(OP "         q0,  q4, q8") \
        __ASM_EMIT(OP "         q1,  q5, q9") \
        __ASM_EMIT(OP "         q2,  q6, q10") \
        __ASM_EMIT(OP "         q3,  q7, q11") \
        __ASM_EMIT("subs        %[count], $16") \
        __ASM_EMIT("vstm        %[" DST "]!, {q0-q3}") \
        __ASM_EMIT("bhs         1b") \
        /* 8x block */ \
        __ASM_EMIT("2:") \
        __ASM_EMIT("adds        %[count], $8") /* 16 - 8 */ \
        __ASM_EMIT("blt         4f") \
        __ASM_EMIT("vldm        %[" B "]!, {q4-q5}") \
        __ASM_EMIT("vldm        %[" A "]" INCA ", {q0-q1}") \
        __ASM_EMIT("vacgt.f32   q8,  q4, q0") \
        __ASM_EMIT("vacgt.f32   q9,  q5, q1") \
        __ASM_EMIT(OP "         q0,  q4, q8") \
        __ASM_EMIT(OP "         q1,  q5, q9") \
        __ASM_EMIT("sub         %[count], $8") \
        __ASM_EMIT("vstm        %[" DST "]!, {q0-q1}") \
        /* 4x block */ \
        __ASM_EMIT("4:") \
        __ASM_EMIT("adds        %[count], $4") /* 8 - 4 */ \
        __ASM_EMIT("blt         6f") \
        __ASM_EMIT("vldm        %[" B "]!, {q4}") \
        __ASM_EMIT("vldm        %[" A "]" INCA ", {q0}") \
        __ASM_EMIT("vacgt.f32   q8,  q4, q0") \
        __ASM_EMIT(OP "         q0,  q4, q8") \
        __ASM_EMIT("sub         %[count], $4") \
        __ASM_EMIT("vstm        %[" DST "]!, {q0}") \
        /* 1x block */ \
        __ASM_EMIT("6:") \
        __ASM_EMIT("adds        %[count], $3") /* 4 - 3 */ \
        __ASM_EMIT("blt         8f") \
        __ASM_EMIT("7:") \
        __ASM_EMIT("vld1.32     {d8[], d9[]}, [%[" B "]]!") \
        __ASM_EMIT("vld1.32     {d0[], d1[]}, [%[" A "]]" INCA) \
        __ASM_EMIT("vacgt.f32   q8,  q4, q0") \
        __ASM_EMIT(OP "         q0,  q4, q8") \
        __ASM_EMIT("subs        %[count], $1") \
        __ASM_EMIT("vst1.32     {d0[0]}, [%[" DST "]]!") \
        __ASM_EMIT("bge         7b") \
        __ASM_EMIT("8:")

    void psmin2(float *dst, const float *src, size_t count)
    {
        ARCH_ARM_ASM
        (
            SIGN_MINMAX_CORE("dst", "dst", "src", "vbif", "")
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
        ARCH_ARM_ASM
        (
            SIGN_MINMAX_CORE("dst", "a", "b", "vbif", "!")
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
        ARCH_ARM_ASM
        (
            SIGN_MINMAX_CORE("dst", "dst", "src", "vbit", "")
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
        ARCH_ARM_ASM
        (
            SIGN_MINMAX_CORE("dst", "a", "b", "vbit", "!")
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
        __ASM_EMIT("subs        %[count], $32") \
        __ASM_EMIT("blo         2f") \
        /* 32x blocks */ \
        __ASM_EMIT("1:") \
        __ASM_EMIT("vldm        %[" B "]!, {q8-q15}") \
        __ASM_EMIT("vldm        %[" A "]" INCA ", {q0-q7}") \
        __ASM_EMIT("vabs.f32    q0, q0") \
        __ASM_EMIT("vabs.f32    q1, q1") \
        __ASM_EMIT("vabs.f32    q2, q2") \
        __ASM_EMIT("vabs.f32    q3, q3") \
        __ASM_EMIT("vabs.f32    q4, q4") \
        __ASM_EMIT("vabs.f32    q5, q5") \
        __ASM_EMIT("vabs.f32    q6, q6") \
        __ASM_EMIT("vabs.f32    q7, q7") \
        __ASM_EMIT("vabs.f32    q8, q8") \
        __ASM_EMIT("vabs.f32    q9, q9") \
        __ASM_EMIT("vabs.f32    q10, q10") \
        __ASM_EMIT("vabs.f32    q11, q11") \
        __ASM_EMIT("vabs.f32    q12, q12") \
        __ASM_EMIT("vabs.f32    q13, q13") \
        __ASM_EMIT("vabs.f32    q14, q14") \
        __ASM_EMIT("vabs.f32    q15, q15") \
        __ASM_EMIT(OP ".f32     q0, q0, q8") \
        __ASM_EMIT(OP ".f32     q1, q1, q9") \
        __ASM_EMIT(OP ".f32     q2, q2, q10") \
        __ASM_EMIT(OP ".f32     q3, q3, q11") \
        __ASM_EMIT(OP ".f32     q4, q4, q12") \
        __ASM_EMIT(OP ".f32     q5, q5, q13") \
        __ASM_EMIT(OP ".f32     q6, q6, q14") \
        __ASM_EMIT(OP ".f32     q7, q7, q15") \
        __ASM_EMIT("subs        %[count], $32") \
        __ASM_EMIT("vstm        %[" DST "]!, {q0-q7}") \
        __ASM_EMIT("bhs         1b") \
        /* 16x block */ \
        __ASM_EMIT("2:") \
        __ASM_EMIT("adds        %[count], $16") /* 32 - 16 */ \
        __ASM_EMIT("blt         4f") \
        __ASM_EMIT("vldm        %[" B "]!, {q8-q11}") \
        __ASM_EMIT("vldm        %[" A "]" INCA ", {q0-q3}") \
        __ASM_EMIT("vabs.f32    q0, q0") \
        __ASM_EMIT("vabs.f32    q1, q1") \
        __ASM_EMIT("vabs.f32    q2, q2") \
        __ASM_EMIT("vabs.f32    q3, q3") \
        __ASM_EMIT("vabs.f32    q8, q8") \
        __ASM_EMIT("vabs.f32    q9, q9") \
        __ASM_EMIT("vabs.f32    q10, q10") \
        __ASM_EMIT("vabs.f32    q11, q11") \
        __ASM_EMIT(OP ".f32     q0, q0, q8") \
        __ASM_EMIT(OP ".f32     q1, q1, q9") \
        __ASM_EMIT(OP ".f32     q2, q2, q10") \
        __ASM_EMIT(OP ".f32     q3, q3, q11") \
        __ASM_EMIT("sub         %[count], $16") \
        __ASM_EMIT("vstm        %[" DST "]!, {q0-q3}") \
        /* 8x block */ \
        __ASM_EMIT("4:") \
        __ASM_EMIT("adds        %[count], $8") /* 16 - 8 */ \
        __ASM_EMIT("blt         6f") \
        __ASM_EMIT("vldm        %[" B "]!, {q8-q9}") \
        __ASM_EMIT("vldm        %[" A "]" INCA ", {q0-q1}") \
        __ASM_EMIT("vabs.f32    q0, q0") \
        __ASM_EMIT("vabs.f32    q1, q1") \
        __ASM_EMIT("vabs.f32    q8, q8") \
        __ASM_EMIT("vabs.f32    q9, q9") \
        __ASM_EMIT(OP ".f32     q0, q0, q8") \
        __ASM_EMIT(OP ".f32     q1, q1, q9") \
        __ASM_EMIT("sub         %[count], $8") \
        __ASM_EMIT("vstm        %[" DST "]!, {q0-q1}") \
        /* 4x block */ \
        __ASM_EMIT("6:") \
        __ASM_EMIT("adds        %[count], $4") /* 8 - 4 */ \
        __ASM_EMIT("blt         8f") \
        __ASM_EMIT("vldm        %[" B "]!, {q8}") \
        __ASM_EMIT("vldm        %[" A "]" INCA ", {q0}") \
        __ASM_EMIT("vabs.f32    q0, q0") \
        __ASM_EMIT("vabs.f32    q8, q8") \
        __ASM_EMIT(OP ".f32     q0, q0, q8") \
        __ASM_EMIT("sub         %[count], $4") \
        __ASM_EMIT("vstm        %[" DST "]!, {q0}") \
        /* 1x block */ \
        __ASM_EMIT("8:") \
        __ASM_EMIT("adds        %[count], $3") /* 4 - 3 */ \
        __ASM_EMIT("blt         10f") \
        __ASM_EMIT("9:") \
        __ASM_EMIT("vld1.32     {d16[], d17[]}, [%[" B "]]!") \
        __ASM_EMIT("vld1.32     {d0[], d1[]}, [%[" A "]]" INCA) \
        __ASM_EMIT("vabs.f32    q0, q0") \
        __ASM_EMIT("vabs.f32    q8, q8") \
        __ASM_EMIT(OP ".f32     q0, q0, q8") \
        __ASM_EMIT("subs        %[count], $1") \
        __ASM_EMIT("vst1.32     {d0[0]}, [%[" DST "]]!") \
        __ASM_EMIT("bge         9b") \
        __ASM_EMIT("10:")

    void pamin2(float *dst, const float *src, size_t count)
    {
        ARCH_ARM_ASM
        (
            ABS_MINMAX_CORE("dst", "dst", "src", "vmin", "")
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
        ARCH_ARM_ASM
        (
            ABS_MINMAX_CORE("dst", "a", "b", "vmin", "!")
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
        ARCH_ARM_ASM
        (
            ABS_MINMAX_CORE("dst", "dst", "src", "vmax", "")
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
        ARCH_ARM_ASM
        (
            ABS_MINMAX_CORE("dst", "a", "b", "vmax", "!")
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
}



#endif /* DSP_ARCH_ARM_NEON_D32_PMATH_MINMAX_H_ */
