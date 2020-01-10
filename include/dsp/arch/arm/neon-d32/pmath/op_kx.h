/*
 * op_kx.h
 *
 *  Created on: 19 нояб. 2019 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_ARM_NEON_D32_PMATH_OP_KX_H_
#define DSP_ARCH_ARM_NEON_D32_PMATH_OP_KX_H_

#ifndef DSP_ARCH_ARM_NEON_32_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_ARM_NEON_32_IMPL */

namespace neon_d32
{
#define OP_DORDER(a, b)     a "," b
#define OP_RORDER(a, b)     b "," a

#define OP_K2_CORE(DST, OP, ORDER) \
    __ASM_EMIT("subs        %[count], $32") \
    __ASM_EMIT("vmov        q9, q8") \
    __ASM_EMIT("blo         2f") \
    /* 32x blocks */ \
    __ASM_EMIT("1:") \
    __ASM_EMIT("vldm        %[" DST "], {q0-q7}") \
    __ASM_EMIT(OP ".f32     q0, " ORDER("q0", "q8")) \
    __ASM_EMIT(OP ".f32     q1, " ORDER("q1", "q9")) \
    __ASM_EMIT(OP ".f32     q2, " ORDER("q2", "q8")) \
    __ASM_EMIT(OP ".f32     q3, " ORDER("q3", "q9")) \
    __ASM_EMIT(OP ".f32     q4, " ORDER("q4", "q8")) \
    __ASM_EMIT(OP ".f32     q5, " ORDER("q5", "q9")) \
    __ASM_EMIT(OP ".f32     q6, " ORDER("q6", "q8")) \
    __ASM_EMIT(OP ".f32     q7, " ORDER("q7", "q9")) \
    __ASM_EMIT("subs        %[count], $32") \
    __ASM_EMIT("vstm        %[" DST "]!, {q0-q7}") \
    __ASM_EMIT("bhs         1b") \
    /* 16x block */ \
    __ASM_EMIT("2:") \
    __ASM_EMIT("adds        %[count], $16") /* 32 - 16 */ \
    __ASM_EMIT("blt         4f") \
    __ASM_EMIT("vldm        %[" DST "], {q0-q3}") \
    __ASM_EMIT(OP ".f32     q0, " ORDER("q0", "q8")) \
    __ASM_EMIT(OP ".f32     q1, " ORDER("q1", "q9")) \
    __ASM_EMIT(OP ".f32     q2, " ORDER("q2", "q8")) \
    __ASM_EMIT(OP ".f32     q3, " ORDER("q3", "q9")) \
    __ASM_EMIT("sub         %[count], $16") \
    __ASM_EMIT("vstm        %[" DST "]!, {q0-q3}") \
    /* 8x block */ \
    __ASM_EMIT("4:") \
    __ASM_EMIT("adds        %[count], $8") /* 16 - 8 */ \
    __ASM_EMIT("blt         6f") \
    __ASM_EMIT("vldm        %[" DST "], {q0-q1}") \
    __ASM_EMIT(OP ".f32     q0, " ORDER("q0", "q8")) \
    __ASM_EMIT(OP ".f32     q1, " ORDER("q1", "q9")) \
    __ASM_EMIT("sub         %[count], $8") \
    __ASM_EMIT("vstm        %[" DST "]!, {q0-q1}") \
    /* 4x block */ \
    __ASM_EMIT("6:") \
    __ASM_EMIT("adds        %[count], $4") /* 8 - 4 */ \
    __ASM_EMIT("blt         8f") \
    __ASM_EMIT("vldm        %[" DST "], {q0}") \
    __ASM_EMIT(OP ".f32     q0, " ORDER("q0", "q8")) \
    __ASM_EMIT("sub         %[count], $4") \
    __ASM_EMIT("vstm        %[" DST "]!, {q0}") \
    /* 1x block */ \
    __ASM_EMIT("8:") \
    __ASM_EMIT("adds        %[count], $3") /* 4 - 3 */ \
    __ASM_EMIT("blt         10f") \
    __ASM_EMIT("9:") \
    __ASM_EMIT("vld1.32     {d0[], d1[]}, [%[" DST "]]") \
    __ASM_EMIT(OP ".f32     q0, " ORDER("q0", "q8")) \
    __ASM_EMIT("subs        %[count], $1") \
    __ASM_EMIT("vst1.32     {d0[0]}, [%[" DST "]]!") \
    __ASM_EMIT("bge         9b") \
    __ASM_EMIT("10:")

#define OP_K3_CORE(DST, SRC, OP, ORDER) \
    __ASM_EMIT("subs        %[count], $32") \
    __ASM_EMIT("vmov        q9, q8") \
    __ASM_EMIT("blo         2f") \
    /* 32x blocks */ \
    __ASM_EMIT("1:") \
    __ASM_EMIT("vldm        %[" SRC "]!, {q0-q7}") \
    __ASM_EMIT(OP ".f32     q0, " ORDER("q0", "q8")) \
    __ASM_EMIT(OP ".f32     q1, " ORDER("q1", "q9")) \
    __ASM_EMIT(OP ".f32     q2, " ORDER("q2", "q8")) \
    __ASM_EMIT(OP ".f32     q3, " ORDER("q3", "q9")) \
    __ASM_EMIT(OP ".f32     q4, " ORDER("q4", "q8")) \
    __ASM_EMIT(OP ".f32     q5, " ORDER("q5", "q9")) \
    __ASM_EMIT(OP ".f32     q6, " ORDER("q6", "q8")) \
    __ASM_EMIT(OP ".f32     q7, " ORDER("q7", "q9")) \
    __ASM_EMIT("subs        %[count], $32") \
    __ASM_EMIT("vstm        %[" DST "]!, {q0-q7}") \
    __ASM_EMIT("bhs         1b") \
    /* 16x block */ \
    __ASM_EMIT("2:") \
    __ASM_EMIT("adds        %[count], $16") /* 32 - 16 */ \
    __ASM_EMIT("blt         4f") \
    __ASM_EMIT("vldm        %[" SRC "]!, {q0-q3}") \
    __ASM_EMIT(OP ".f32     q0, " ORDER("q0", "q8")) \
    __ASM_EMIT(OP ".f32     q1, " ORDER("q1", "q9")) \
    __ASM_EMIT(OP ".f32     q2, " ORDER("q2", "q8")) \
    __ASM_EMIT(OP ".f32     q3, " ORDER("q3", "q9")) \
    __ASM_EMIT("sub         %[count], $16") \
    __ASM_EMIT("vstm        %[" DST "]!, {q0-q3}") \
    /* 8x block */ \
    __ASM_EMIT("4:") \
    __ASM_EMIT("adds        %[count], $8") /* 16 - 8 */ \
    __ASM_EMIT("blt         6f") \
    __ASM_EMIT("vldm        %[" SRC "]!, {q0-q1}") \
    __ASM_EMIT(OP ".f32     q0, " ORDER("q0", "q8")) \
    __ASM_EMIT(OP ".f32     q1, " ORDER("q1", "q9")) \
    __ASM_EMIT("sub         %[count], $8") \
    __ASM_EMIT("vstm        %[" DST "]!, {q0-q1}") \
    /* 4x block */ \
    __ASM_EMIT("6:") \
    __ASM_EMIT("adds        %[count], $4") /* 8 - 4 */ \
    __ASM_EMIT("blt         8f") \
    __ASM_EMIT("vldm        %[" SRC "]!, {q0}") \
    __ASM_EMIT(OP ".f32     q0, " ORDER("q0", "q8")) \
    __ASM_EMIT("sub         %[count], $4") \
    __ASM_EMIT("vstm        %[" DST "]!, {q0}") \
    /* 1x block */ \
    __ASM_EMIT("8:") \
    __ASM_EMIT("adds        %[count], $3") /* 4 - 1 */ \
    __ASM_EMIT("blt         10f") \
    __ASM_EMIT("9:") \
    __ASM_EMIT("vld1.32     {d0[], d1[]}, [%[" SRC "]]!") \
    __ASM_EMIT(OP ".f32     q0, " ORDER("q0", "q8")) \
    __ASM_EMIT("subs        %[count], $1") \
    __ASM_EMIT("vst1.32     {d0[0]}, [%[" DST "]]!") \
    __ASM_EMIT("bge         9b") \
    __ASM_EMIT("10:")

#define RDIV_KX_CORE(DST, SRC, INC)   \
    __ASM_EMIT("subs            %[count], $16") \
    __ASM_EMIT("vmov            q5, q4") \
    __ASM_EMIT("blo             2f") \
    /* 16x blocks */ \
    __ASM_EMIT("1:") \
    __ASM_EMIT("vldm            %[" SRC "]" INC ", {q0-q3}") \
    __ASM_EMIT("vrecpe.f32      q8, q0")                    /* q8 = s2 */ \
    __ASM_EMIT("vrecpe.f32      q9, q1") \
    __ASM_EMIT("vrecpe.f32      q10, q2") \
    __ASM_EMIT("vrecpe.f32      q11, q3") \
    __ASM_EMIT("vrecps.f32      q12, q8, q0")               /* q12 = (2 - R*s2) */ \
    __ASM_EMIT("vrecps.f32      q13, q9, q1") \
    __ASM_EMIT("vrecps.f32      q14, q10, q2") \
    __ASM_EMIT("vrecps.f32      q15, q11, q3") \
    __ASM_EMIT("vmul.f32        q8, q12, q8")               /* q8 = s2' = s2 * (2 - R*s2) */ \
    __ASM_EMIT("vmul.f32        q9, q13, q9") \
    __ASM_EMIT("vmul.f32        q10, q14, q10") \
    __ASM_EMIT("vmul.f32        q11, q15, q11") \
    __ASM_EMIT("vrecps.f32      q12, q8, q0")               /* q12 = (2 - R*s2') */ \
    __ASM_EMIT("vrecps.f32      q13, q9, q1") \
    __ASM_EMIT("vrecps.f32      q14, q10, q2") \
    __ASM_EMIT("vrecps.f32      q15, q11, q3") \
    __ASM_EMIT("vmul.f32        q0, q12, q8")               /* q0 = s2" = s2' * (2 - R*s2) = 1/s2 */  \
    __ASM_EMIT("vmul.f32        q1, q13, q9") \
    __ASM_EMIT("vmul.f32        q2, q14, q10") \
    __ASM_EMIT("vmul.f32        q3, q15, q11") \
    __ASM_EMIT("vmul.f32        q0, q0, q4")                /* k / s2 */ \
    __ASM_EMIT("vmul.f32        q1, q1, q5") \
    __ASM_EMIT("vmul.f32        q2, q2, q4") \
    __ASM_EMIT("vmul.f32        q3, q3, q5") \
    __ASM_EMIT("vstm            %[" DST "]!, {q0-q3}") \
    __ASM_EMIT("subs            %[count], $16") \
    __ASM_EMIT("bhs             1b") \
    /* 8x block */ \
    __ASM_EMIT("2:") \
    __ASM_EMIT("adds            %[count], $8") \
    __ASM_EMIT("blt             4f") \
    __ASM_EMIT("vldm            %[" SRC "]" INC ", {q0-q1}") \
    __ASM_EMIT("vrecpe.f32      q8, q0")                    /* q8 = s2 */ \
    __ASM_EMIT("vrecpe.f32      q9, q1") \
    __ASM_EMIT("vrecps.f32      q12, q8, q0")               /* q12 = (2 - R*s2) */ \
    __ASM_EMIT("vrecps.f32      q13, q9, q1") \
    __ASM_EMIT("vmul.f32        q8, q12, q8")               /* q8 = s2' = s2 * (2 - R*s2) */ \
    __ASM_EMIT("vmul.f32        q9, q13, q9") \
    __ASM_EMIT("vrecps.f32      q12, q8, q0")               /* q12 = (2 - R*s2') */ \
    __ASM_EMIT("vrecps.f32      q13, q9, q1") \
    __ASM_EMIT("vmul.f32        q0, q12, q8")               /* q0 = s2" = s2' * (2 - R*s2) = 1/s2 */  \
    __ASM_EMIT("vmul.f32        q1, q13, q9") \
    __ASM_EMIT("vmul.f32        q0, q0, q4")                /* k / s2 */ \
    __ASM_EMIT("vmul.f32        q1, q1, q5") \
    __ASM_EMIT("sub             %[count], $8") \
    __ASM_EMIT("vstm            %[" DST "]!, {q0-q1}") \
    /* 4x blocks */ \
    __ASM_EMIT("4:") \
    __ASM_EMIT("adds            %[count], $4") \
    __ASM_EMIT("blt             6f") \
    __ASM_EMIT("vldm            %[" SRC "]" INC ", {q0}") \
    __ASM_EMIT("vrecpe.f32      q8, q0")                    /* q8 = s2 */ \
    __ASM_EMIT("vrecps.f32      q12, q8, q0")               /* q12 = (2 - R*s2) */ \
    __ASM_EMIT("vmul.f32        q8, q12, q8")               /* q8 = s2' = s2 * (2 - R*s2) */ \
    __ASM_EMIT("vrecps.f32      q12, q8, q0")               /* q12 = (2 - R*s2') */ \
    __ASM_EMIT("vmul.f32        q0, q12, q8")               /* q0 = s2" = s2' * (2 - R*s2) = 1/s2 */  \
    __ASM_EMIT("vmul.f32        q0, q0, q4")                /* s1 / s2 */ \
    __ASM_EMIT("sub             %[count], $4") \
    __ASM_EMIT("vstm            %[" DST "]!, {q0}") \
    /* 1x blocks */ \
    __ASM_EMIT("6:") \
    __ASM_EMIT("adds            %[count], $3") \
    __ASM_EMIT("blt             8f") \
    __ASM_EMIT("7:") \
    __ASM_EMIT("vld1.32         {d0[], d1[]}, [%[" SRC "]]" INC ) \
    __ASM_EMIT("vrecpe.f32      q8, q0")                    /* q8 = s2 */ \
    __ASM_EMIT("vrecps.f32      q12, q8, q0")               /* q12 = (2 - R*s2) */ \
    __ASM_EMIT("vmul.f32        q8, q12, q8")               /* q8 = s2' = s2 * (2 - R*s2) */ \
    __ASM_EMIT("vrecps.f32      q12, q8, q0")               /* q12 = (2 - R*s2') */ \
    __ASM_EMIT("vmul.f32        q0, q12, q8")               /* q0 = s2" = s2' * (2 - R*s2) = 1/s2 */  \
    __ASM_EMIT("vmul.f32        q0, q0, q4")                /* s1 / s2 */ \
    __ASM_EMIT("vst1.32         {d0[0]}, [%[" DST "]]!") \
    __ASM_EMIT("subs            %[count], $1") \
    __ASM_EMIT("bge             7b") \
    __ASM_EMIT("8:")


#define MOD_KX_CORE(DST, SRC, INC)   \
    __ASM_EMIT("subs            %[count], $16") \
    __ASM_EMIT("vrecpe.f32      q8, q4") \
    __ASM_EMIT("vmov            q5, q4")                    /* q5 = k */ \
    __ASM_EMIT("vrecps.f32      q12, q8, q4") \
    __ASM_EMIT("vmul.f32        q8, q12, q8") \
    __ASM_EMIT("vrecps.f32      q12, q8, q4") \
    __ASM_EMIT("vmul.f32        q6, q12, q8")               /* q6 = 1/k */ \
    __ASM_EMIT("vmov            q7, q6")                    /* q7 = 1/k */ \
    __ASM_EMIT("blo             2f") \
    /* 16x blocks */ \
    __ASM_EMIT("1:") \
    __ASM_EMIT("vldm            %[" SRC "]" INC ", {q0-q3}") \
    __ASM_EMIT("vmul.f32        q8, q0, q6")                /* q0 = x/k */  \
    __ASM_EMIT("vmul.f32        q9, q1, q7") \
    __ASM_EMIT("vmul.f32        q10, q2, q6") \
    __ASM_EMIT("vmul.f32        q11, q3, q7") \
    __ASM_EMIT("vcvt.s32.f32    q8, q8") \
    __ASM_EMIT("vcvt.s32.f32    q9, q9") \
    __ASM_EMIT("vcvt.s32.f32    q10, q10") \
    __ASM_EMIT("vcvt.s32.f32    q11, q11") \
    __ASM_EMIT("vcvt.f32.s32    q8, q8") \
    __ASM_EMIT("vcvt.f32.s32    q9, q9") \
    __ASM_EMIT("vcvt.f32.s32    q10, q10") \
    __ASM_EMIT("vcvt.f32.s32    q11, q11") \
    __ASM_EMIT("vmls.f32        q0, q8, q4") \
    __ASM_EMIT("vmls.f32        q1, q9, q5") \
    __ASM_EMIT("vmls.f32        q2, q10, q4") \
    __ASM_EMIT("vmls.f32        q3, q11, q5") \
    __ASM_EMIT("vstm            %[" DST "]!, {q0-q3}") \
    __ASM_EMIT("subs            %[count], $16") \
    __ASM_EMIT("bhs             1b") \
    /* 8x block */ \
    __ASM_EMIT("2:") \
    __ASM_EMIT("adds            %[count], $8") \
    __ASM_EMIT("blt             4f") \
    __ASM_EMIT("vldm            %[" SRC "]" INC ", {q0-q1}") \
    __ASM_EMIT("vmul.f32        q8, q0, q6")                /* q0 = x/k */  \
    __ASM_EMIT("vmul.f32        q9, q1, q7") \
    __ASM_EMIT("vcvt.s32.f32    q8, q8") \
    __ASM_EMIT("vcvt.s32.f32    q9, q9") \
    __ASM_EMIT("vcvt.f32.s32    q8, q8") \
    __ASM_EMIT("vcvt.f32.s32    q9, q9") \
    __ASM_EMIT("vmls.f32        q0, q8, q4") \
    __ASM_EMIT("vmls.f32        q1, q9, q5") \
    __ASM_EMIT("sub             %[count], $8") \
    __ASM_EMIT("vstm            %[" DST "]!, {q0-q1}") \
    /* 4x blocks */ \
    __ASM_EMIT("4:") \
    __ASM_EMIT("adds            %[count], $4") \
    __ASM_EMIT("blt             6f") \
    __ASM_EMIT("vldm            %[" SRC "]" INC ", {q0}") \
    __ASM_EMIT("vmul.f32        q8, q0, q6")                /* q0 = x/k */  \
    __ASM_EMIT("vcvt.s32.f32    q8, q8") \
    __ASM_EMIT("vcvt.f32.s32    q8, q8") \
    __ASM_EMIT("vmls.f32        q0, q8, q4") \
    __ASM_EMIT("sub             %[count], $4") \
    __ASM_EMIT("vstm            %[" DST "]!, {q0}") \
    /* 1x blocks */ \
    __ASM_EMIT("6:") \
    __ASM_EMIT("adds            %[count], $3") \
    __ASM_EMIT("blt             8f") \
    __ASM_EMIT("7:") \
    __ASM_EMIT("vld1.32         {d0[], d1[]}, [%[" SRC "]]" INC) \
    __ASM_EMIT("vmul.f32        q8, q0, q6")                /* q0 = x/k */  \
    __ASM_EMIT("vcvt.s32.f32    q8, q8") \
    __ASM_EMIT("vcvt.f32.s32    q8, q8") \
    __ASM_EMIT("vmls.f32        q0, q8, q4") \
    __ASM_EMIT("vst1.32         {d0[0]}, [%[" DST "]]!") \
    __ASM_EMIT("subs            %[count], $1") \
    __ASM_EMIT("bge             7b") \
    __ASM_EMIT("8:")

#define RMOD_KX_CORE(DST, SRC, INC)   \
    __ASM_EMIT("subs            %[count], $16") \
    __ASM_EMIT("vmov            q5, q4")                    /* q5 = k */ \
    __ASM_EMIT("blo             2f") \
    /* 16x blocks */ \
    __ASM_EMIT("1:") \
    __ASM_EMIT("vldm            %[" SRC "]" INC ", {q0-q3}") \
    __ASM_EMIT("vrecpe.f32      q8, q0")                    /* q8 = s2 */ \
    __ASM_EMIT("vrecpe.f32      q9, q1") \
    __ASM_EMIT("vrecpe.f32      q10, q2") \
    __ASM_EMIT("vrecpe.f32      q11, q3") \
    __ASM_EMIT("vrecps.f32      q12, q8, q0")               /* q12 = (2 - R*s2) */ \
    __ASM_EMIT("vrecps.f32      q13, q9, q1") \
    __ASM_EMIT("vrecps.f32      q14, q10, q2") \
    __ASM_EMIT("vrecps.f32      q15, q11, q3") \
    __ASM_EMIT("vmul.f32        q8, q12, q8")               /* q8 = s2' = s2 * (2 - R*s2) */ \
    __ASM_EMIT("vmul.f32        q9, q13, q9") \
    __ASM_EMIT("vmul.f32        q10, q14, q10") \
    __ASM_EMIT("vmul.f32        q11, q15, q11") \
    __ASM_EMIT("vrecps.f32      q12, q8, q0")               /* q12 = (2 - R*s2') */ \
    __ASM_EMIT("vrecps.f32      q13, q9, q1") \
    __ASM_EMIT("vrecps.f32      q14, q10, q2") \
    __ASM_EMIT("vrecps.f32      q15, q11, q3") \
    __ASM_EMIT("vmul.f32        q8, q12, q8")               /* q8 = s2" = s2' * (2 - R*s2) = 1/x */  \
    __ASM_EMIT("vmul.f32        q9, q13, q9") \
    __ASM_EMIT("vmul.f32        q10, q14, q10") \
    __ASM_EMIT("vmul.f32        q11, q15, q11") \
    __ASM_EMIT("vmul.f32        q8, q8, q4")                /* q8 = k/x */ \
    __ASM_EMIT("vmul.f32        q9, q9, q5") \
    __ASM_EMIT("vmul.f32        q10, q10, q4") \
    __ASM_EMIT("vmul.f32        q11, q11, q5") \
    __ASM_EMIT("vcvt.s32.f32    q8, q8") \
    __ASM_EMIT("vcvt.s32.f32    q9, q9") \
    __ASM_EMIT("vcvt.s32.f32    q10, q10") \
    __ASM_EMIT("vcvt.s32.f32    q11, q11") \
    __ASM_EMIT("vcvt.f32.s32    q8, q8") \
    __ASM_EMIT("vcvt.f32.s32    q9, q9") \
    __ASM_EMIT("vcvt.f32.s32    q10, q10") \
    __ASM_EMIT("vcvt.f32.s32    q11, q11") \
    __ASM_EMIT("vmul.f32        q0, q0, q8")                /* q0 = x * int(k/x) */ \
    __ASM_EMIT("vmul.f32        q1, q1, q9") \
    __ASM_EMIT("vmul.f32        q2, q2, q10") \
    __ASM_EMIT("vmul.f32        q3, q3, q11") \
    __ASM_EMIT("vsub.f32        q0, q4, q0")                /* q0 = k - x * int(k/x) */ \
    __ASM_EMIT("vsub.f32        q1, q5, q1") \
    __ASM_EMIT("vsub.f32        q2, q4, q2") \
    __ASM_EMIT("vsub.f32        q3, q5, q3") \
    __ASM_EMIT("vstm            %[" DST "]!, {q0-q3}") \
    __ASM_EMIT("subs            %[count], $16") \
    __ASM_EMIT("bhs             1b") \
    /* 8x block */ \
    __ASM_EMIT("2:") \
    __ASM_EMIT("adds            %[count], $8") \
    __ASM_EMIT("blt             4f") \
    __ASM_EMIT("vldm            %[" SRC "]" INC ", {q0-q1}") \
    __ASM_EMIT("vrecpe.f32      q8, q0")                    /* q8 = s2 */ \
    __ASM_EMIT("vrecpe.f32      q9, q1") \
    __ASM_EMIT("vrecps.f32      q12, q8, q0")               /* q12 = (2 - R*s2) */ \
    __ASM_EMIT("vrecps.f32      q13, q9, q1") \
    __ASM_EMIT("vmul.f32        q8, q12, q8")               /* q8 = s2' = s2 * (2 - R*s2) */ \
    __ASM_EMIT("vmul.f32        q9, q13, q9") \
    __ASM_EMIT("vrecps.f32      q12, q8, q0")               /* q12 = (2 - R*s2') */ \
    __ASM_EMIT("vrecps.f32      q13, q9, q1") \
    __ASM_EMIT("vmul.f32        q8, q12, q8")               /* q8 = s2" = s2' * (2 - R*s2) = 1/x */  \
    __ASM_EMIT("vmul.f32        q9, q13, q9") \
    __ASM_EMIT("vmul.f32        q8, q8, q4")                /* q8 = k/x */ \
    __ASM_EMIT("vmul.f32        q9, q9, q5") \
    __ASM_EMIT("vcvt.s32.f32    q8, q8") \
    __ASM_EMIT("vcvt.s32.f32    q9, q9") \
    __ASM_EMIT("vcvt.f32.s32    q8, q8") \
    __ASM_EMIT("vcvt.f32.s32    q9, q9") \
    __ASM_EMIT("vmul.f32        q0, q0, q8")                /* q0 = x * int(k/x) */ \
    __ASM_EMIT("vmul.f32        q1, q1, q9") \
    __ASM_EMIT("vsub.f32        q0, q4, q0")                /* q0 = k - x * int(k/x) */ \
    __ASM_EMIT("vsub.f32        q1, q5, q1") \
    __ASM_EMIT("sub             %[count], $8") \
    __ASM_EMIT("vstm            %[" DST "]!, {q0-q1}") \
    /* 4x blocks */ \
    __ASM_EMIT("4:") \
    __ASM_EMIT("adds            %[count], $4") \
    __ASM_EMIT("blt             6f") \
    __ASM_EMIT("vldm            %[" SRC "]" INC ", {q0}") \
    __ASM_EMIT("vrecpe.f32      q8, q0")                    /* q8 = s2 */ \
    __ASM_EMIT("vrecps.f32      q12, q8, q0")               /* q12 = (2 - R*s2) */ \
    __ASM_EMIT("vmul.f32        q8, q12, q8")               /* q8 = s2' = s2 * (2 - R*s2) */ \
    __ASM_EMIT("vrecps.f32      q12, q8, q0")               /* q12 = (2 - R*s2') */ \
    __ASM_EMIT("vmul.f32        q8, q12, q8")               /* q8 = s2" = s2' * (2 - R*s2) = 1/x */  \
    __ASM_EMIT("vmul.f32        q8, q8, q4")                /* q8 = k/x */ \
    __ASM_EMIT("vcvt.s32.f32    q8, q8") \
    __ASM_EMIT("vcvt.f32.s32    q8, q8") \
    __ASM_EMIT("vmul.f32        q0, q0, q8")                /* q0 = x * int(k/x) */ \
    __ASM_EMIT("vsub.f32        q0, q4, q0")                /* q0 = k - x * int(k/x) */ \
    __ASM_EMIT("sub             %[count], $4") \
    __ASM_EMIT("vstm            %[" DST "]!, {q0}") \
    /* 1x blocks */ \
    __ASM_EMIT("6:") \
    __ASM_EMIT("adds            %[count], $3") \
    __ASM_EMIT("blt             8f") \
    __ASM_EMIT("7:") \
    __ASM_EMIT("vld1.32         {d0[], d1[]}, [%[" SRC "]]" INC) \
    __ASM_EMIT("vrecpe.f32      q8, q0")                    /* q8 = s2 */ \
    __ASM_EMIT("vrecps.f32      q12, q8, q0")               /* q12 = (2 - R*s2) */ \
    __ASM_EMIT("vmul.f32        q8, q12, q8")               /* q8 = s2' = s2 * (2 - R*s2) */ \
    __ASM_EMIT("vrecps.f32      q12, q8, q0")               /* q12 = (2 - R*s2') */ \
    __ASM_EMIT("vmul.f32        q8, q12, q8")               /* q8 = s2" = s2' * (2 - R*s2) = 1/x */  \
    __ASM_EMIT("vmul.f32        q8, q8, q4")                /* q8 = k/x */ \
    __ASM_EMIT("vcvt.s32.f32    q8, q8") \
    __ASM_EMIT("vcvt.f32.s32    q8, q8") \
    __ASM_EMIT("vmul.f32        q0, q0, q8")                /* q0 = x * int(k/x) */ \
    __ASM_EMIT("vsub.f32        q0, q4, q0")                /* q0 = k - x * int(k/x) */ \
    __ASM_EMIT("vst1.32         {d0[0]}, [%[" DST "]]!") \
    __ASM_EMIT("subs            %[count], $1") \
    __ASM_EMIT("bge             7b") \
    __ASM_EMIT("8:")

    void add_k2(float *dst, float k, size_t count)
    {
        IF_ARCH_ARM(float *pk = &k);
        ARCH_ARM_ASM
        (
            __ASM_EMIT("vld1.f32    {d16[], d17[]}, [%[k]]")
            OP_K2_CORE("dst", "vadd", OP_DORDER)
            : [dst] "+r" (dst),
              [count] "+r" (count)
            : [k] "r" (pk)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9"
        );
    }

    void sub_k2(float *dst, float k, size_t count)
    {
        IF_ARCH_ARM(float *pk = &k);
        ARCH_ARM_ASM
        (
            __ASM_EMIT("vld1.f32    {d16[], d17[]}, [%[k]]")
            OP_K2_CORE("dst", "vsub", OP_DORDER)
            : [dst] "+r" (dst),
              [count] "+r" (count)
            : [k] "r" (pk)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9"
        );
    }

    void rsub_k2(float *dst, float k, size_t count)
    {
        IF_ARCH_ARM(float *pk = &k);
        ARCH_ARM_ASM
        (
            __ASM_EMIT("vld1.f32    {d16[], d17[]}, [%[k]]")
            OP_K2_CORE("dst", "vsub", OP_RORDER)
            : [dst] "+r" (dst),
              [count] "+r" (count)
            : [k] "r" (pk)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9"
        );
    }

    void mul_k2(float *dst, float k, size_t count)
    {
        IF_ARCH_ARM(float *pk = &k);
        ARCH_ARM_ASM
        (
            __ASM_EMIT("vld1.f32    {d16[], d17[]}, [%[k]]")
            OP_K2_CORE("dst", "vmul", OP_DORDER)
            : [dst] "+r" (dst),
              [count] "+r" (count)
            : [k] "r" (pk)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9"
        );
    }

    void div_k2(float *dst, float k, size_t count)
    {
        IF_ARCH_ARM(float *pk = &k);
        ARCH_ARM_ASM
        (
            __ASM_EMIT("vld1.f32        {d0[], d1[]}, [%[k]]")
            __ASM_EMIT("vrecpe.f32      q8, q0")                    /* q8 = k */
            __ASM_EMIT("vrecps.f32      q9, q8, q0")                /* q9 = (2 - R*k) */
            __ASM_EMIT("vmul.f32        q8, q9, q8")                /* q8 = k' = k * (2 - R*k) */
            __ASM_EMIT("vrecps.f32      q9, q8, q0")                /* q9 = (2 - R*k') */
            __ASM_EMIT("vmul.f32        q8, q9, q8")                /* q8 = k" = k' * (2 - R*k) = 1/k */
            OP_K2_CORE("dst", "vmul", OP_DORDER)
            : [dst] "+r" (dst),
              [count] "+r" (count)
            : [k] "r" (pk)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    void rdiv_k2(float *dst, float k, size_t count)
    {
        IF_ARCH_ARM(float *pk = &k);
        ARCH_ARM_ASM
        (
            __ASM_EMIT("vld1.f32    {d8[], d9[]}, [%[k]]")
            RDIV_KX_CORE("dst", "dst", "")
            : [dst] "+r" (dst),
              [count] "+r" (count)
            : [k] "r" (pk)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    void mod_k2(float *dst, float k, size_t count)
    {
        IF_ARCH_ARM(float *pk = &k);
        ARCH_ARM_ASM
        (
            __ASM_EMIT("vld1.f32    {d8[], d9[]}, [%[k]]")
            MOD_KX_CORE("dst", "dst", "")
            : [dst] "+r" (dst),
              [count] "+r" (count)
            : [k] "r" (pk)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    void rmod_k2(float *dst, float k, size_t count)
    {
        IF_ARCH_ARM(float *pk = &k);
        ARCH_ARM_ASM
        (
            __ASM_EMIT("vld1.f32    {d8[], d9[]}, [%[k]]")
            RMOD_KX_CORE("dst", "dst", "")
            : [dst] "+r" (dst),
              [count] "+r" (count)
            : [k] "r" (pk)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    void add_k3(float *dst, const float *src, float k, size_t count)
    {
        IF_ARCH_ARM(float *pk = &k);
        ARCH_ARM_ASM
        (
            __ASM_EMIT("vld1.f32    {d16[], d17[]}, [%[k]]")
            OP_K3_CORE("dst", "src", "vadd", OP_DORDER)
            : [dst] "+r" (dst), [src] "+r" (src),
              [count] "+r" (count)
            : [k] "r" (pk)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9"
        );
    }

    void sub_k3(float *dst, const float *src, float k, size_t count)
    {
        IF_ARCH_ARM(float *pk = &k);
        ARCH_ARM_ASM
        (
            __ASM_EMIT("vld1.f32    {d16[], d17[]}, [%[k]]")
            OP_K3_CORE("dst", "src", "vsub", OP_DORDER)
            : [dst] "+r" (dst), [src] "+r" (src),
              [count] "+r" (count)
            : [k] "r" (pk)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9"
        );
    }

    void rsub_k3(float *dst, const float *src, float k, size_t count)
    {
        IF_ARCH_ARM(float *pk = &k);
        ARCH_ARM_ASM
        (
            __ASM_EMIT("vld1.f32    {d16[], d17[]}, [%[k]]")
            OP_K3_CORE("dst", "src", "vsub", OP_RORDER)
            : [dst] "+r" (dst), [src] "+r" (src),
              [count] "+r" (count)
            : [k] "r" (pk)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9"
        );
    }

    void mul_k3(float *dst, const float *src, float k, size_t count)
    {
        IF_ARCH_ARM(float *pk = &k);
        ARCH_ARM_ASM
        (
            __ASM_EMIT("vld1.f32    {d16[], d17[]}, [%[k]]")
            OP_K3_CORE("dst", "src", "vmul", OP_DORDER)
            : [dst] "+r" (dst), [src] "+r" (src),
              [count] "+r" (count)
            : [k] "r" (pk)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9"
        );
    }

    void div_k3(float *dst, const float *src, float k, size_t count)
    {
        IF_ARCH_ARM(float *pk = &k);
        ARCH_ARM_ASM
        (
            __ASM_EMIT("vld1.f32        {d0[], d1[]}, [%[k]]")
            __ASM_EMIT("vrecpe.f32      q8, q0")                    /* q8 = k */
            __ASM_EMIT("vrecps.f32      q9, q8, q0")                /* q9 = (2 - R*k) */
            __ASM_EMIT("vmul.f32        q8, q9, q8")                /* q8 = k' = k * (2 - R*k) */
            __ASM_EMIT("vrecps.f32      q9, q8, q0")                /* q9 = (2 - R*k') */
            __ASM_EMIT("vmul.f32        q8, q9, q8")                /* q8 = k" = k' * (2 - R*k) = 1/k */
            OP_K3_CORE("dst", "src", "vmul", OP_DORDER)
            : [dst] "+r" (dst), [src] "+r" (src),
              [count] "+r" (count)
            : [k] "r" (pk)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9"
        );
    }

    void rdiv_k3(float *dst, const float *src, float k, size_t count)
    {
        IF_ARCH_ARM(float *pk = &k);
        ARCH_ARM_ASM
        (
            __ASM_EMIT("vld1.f32    {d8[], d9[]}, [%[k]]")
            RDIV_KX_CORE("dst", "src", "!")
            : [dst] "+r" (dst), [src] "+r" (src),
              [count] "+r" (count)
            : [k] "r" (pk)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    void mod_k3(float *dst, const float *src, float k, size_t count)
    {
        IF_ARCH_ARM(float *pk = &k);
        ARCH_ARM_ASM
        (
            __ASM_EMIT("vld1.f32    {d8[], d9[]}, [%[k]]")
            MOD_KX_CORE("dst", "src", "!")
            : [dst] "+r" (dst), [src] "+r" (src),
              [count] "+r" (count)
            : [k] "r" (pk)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    void rmod_k3(float *dst, const float *src, float k, size_t count)
    {
        IF_ARCH_ARM(float *pk = &k);
        ARCH_ARM_ASM
        (
            __ASM_EMIT("vld1.f32    {d8[], d9[]}, [%[k]]")
            RMOD_KX_CORE("dst", "src", "!")
            : [dst] "+r" (dst), [src] "+r" (src),
              [count] "+r" (count)
            : [k] "r" (pk)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    #undef OP_DORDER
    #undef OP_RORDER
    #undef OP_K2_CORE
    #undef OP_K3_CORE
    #undef RDIV_KX_CORE
    #undef MOD_KX_CORE
    #undef RMOD_KX_CORE
}

#endif /* DSP_ARCH_ARM_NEON_D32_PMATH_OP_KX_H_ */
