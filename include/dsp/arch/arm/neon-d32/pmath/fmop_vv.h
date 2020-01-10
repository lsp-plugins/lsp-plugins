/*
 * fmop_vv.h
 *
 *  Created on: 22 нояб. 2019 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_ARM_NEON_D32_PMATH_FMOP_VV_H_
#define DSP_ARCH_ARM_NEON_D32_PMATH_FMOP_VV_H_

#ifndef DSP_ARCH_ARM_NEON_32_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_ARM_NEON_32_IMPL */

namespace neon_d32
{
#define OP_DSEL(a, b)       a
#define OP_RSEL(a, b)       b

#define FMADDSUB3_CORE(DST, A, B, OP) \
    __ASM_EMIT("subs        %[count], $16") \
    __ASM_EMIT("blo         2f") \
    /* 16x blocks */ \
    __ASM_EMIT("1:") \
    __ASM_EMIT("vldm        %[" A "]!, {q4-q7}") \
    __ASM_EMIT("vldm        %[" B "]!, {q8-q11}") \
    __ASM_EMIT("vldm        %[" DST "], {q0-q3}") \
    __ASM_EMIT(OP ".f32     q0, q4, q8") \
    __ASM_EMIT(OP ".f32     q1, q5, q9") \
    __ASM_EMIT(OP ".f32     q2, q6, q10") \
    __ASM_EMIT(OP ".f32     q3, q7, q11") \
    __ASM_EMIT("subs        %[count], $16") \
    __ASM_EMIT("vstm        %[" DST "]!, {q0-q3}") \
    __ASM_EMIT("bhs         1b") \
    /* 8x block */ \
    __ASM_EMIT("2:") \
    __ASM_EMIT("adds        %[count], $8") /* 16 - 8 */ \
    __ASM_EMIT("blt         4f") \
    __ASM_EMIT("vldm        %[" A "]!, {q4-q5}") \
    __ASM_EMIT("vldm        %[" B "]!, {q8-q9}") \
    __ASM_EMIT("vldm        %[" DST "], {q0-q1}") \
    __ASM_EMIT(OP ".f32     q0, q4, q8") \
    __ASM_EMIT(OP ".f32     q1, q5, q9") \
    __ASM_EMIT("sub         %[count], $8") \
    __ASM_EMIT("vstm        %[" DST "]!, {q0-q1}") \
    /* 4x block */ \
    __ASM_EMIT("4:") \
    __ASM_EMIT("adds        %[count], $4") /* 8 - 4 */ \
    __ASM_EMIT("blt         6f") \
    __ASM_EMIT("vldm        %[" A "]!, {q4}") \
    __ASM_EMIT("vldm        %[" B "]!, {q8}") \
    __ASM_EMIT("vldm        %[" DST "], {q0}") \
    __ASM_EMIT(OP ".f32     q0, q4, q8") \
    __ASM_EMIT("sub         %[count], $4") \
    __ASM_EMIT("vstm        %[" DST "]!, {q0}") \
    /* 1x block */ \
    __ASM_EMIT("6:") \
    __ASM_EMIT("adds        %[count], $3") /* 4 - 3 */ \
    __ASM_EMIT("blt         8f") \
    __ASM_EMIT("7:") \
    __ASM_EMIT("vld1.32     {d8[], d9[]}, [%[" A "]]!") \
    __ASM_EMIT("vld1.32     {d16[], d17[]}, [%[" B "]]!") \
    __ASM_EMIT("vld1.32     {d0[], d1[]}, [%[" DST "]]") \
    __ASM_EMIT(OP ".f32     q0, q4, q8") \
    __ASM_EMIT("subs        %[count], $1") \
    __ASM_EMIT("vst1.32     {d0[0]}, [%[" DST "]]!") \
    __ASM_EMIT("bge         7b") \
    __ASM_EMIT("8:")

    void fmadd3(float *dst, const float *a, const float *b, size_t count)
    {
        ARCH_ARM_ASM
        (
            FMADDSUB3_CORE("dst", "a", "b", "vfma")
            : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11"
        );
    }

    void fmsub3(float *dst, const float *a, const float *b, size_t count)
    {
        ARCH_ARM_ASM
        (
            FMADDSUB3_CORE("dst", "a", "b", "vfms")
            : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11"
        );
    }

#undef FMADDSUB3_CORE

#define FMOP3_CORE(DST, A, B, OP, SEL) \
    __ASM_EMIT("subs        %[count], $16") \
    __ASM_EMIT("blo         2f") \
    /* 16x blocks */ \
    __ASM_EMIT("1:") \
    __ASM_EMIT("vldm        %[" A "]!, {q0-q3}") \
    __ASM_EMIT("vldm        %[" B "]!, {q8-q11}") \
    __ASM_EMIT("vldm        %[" DST "], {q4-q7}") \
    __ASM_EMIT("vmul.f32    q0, q0, q8") \
    __ASM_EMIT("vmul.f32    q1, q1, q9") \
    __ASM_EMIT("vmul.f32    q2, q2, q10") \
    __ASM_EMIT("vmul.f32    q3, q3, q11") \
    __ASM_EMIT(OP ".f32     q0, " SEL("q4", "q0") ", " SEL("q0", "q4")) \
    __ASM_EMIT(OP ".f32     q1, " SEL("q5", "q1") ", " SEL("q1", "q5")) \
    __ASM_EMIT(OP ".f32     q2, " SEL("q6", "q2") ", " SEL("q2", "q6")) \
    __ASM_EMIT(OP ".f32     q3, " SEL("q7", "q3") ", " SEL("q3", "q7")) \
    __ASM_EMIT("subs        %[count], $16") \
    __ASM_EMIT("vstm        %[" DST "]!, {q0-q3}") \
    __ASM_EMIT("bhs         1b") \
    /* 8x block */ \
    __ASM_EMIT("2:") \
    __ASM_EMIT("adds        %[count], $8") /* 16 - 8 */ \
    __ASM_EMIT("blt         4f") \
    __ASM_EMIT("vldm        %[" A "]!, {q0-q1}") \
    __ASM_EMIT("vldm        %[" B "]!, {q8-q9}") \
    __ASM_EMIT("vldm        %[" DST "], {q4-q5}") \
    __ASM_EMIT("vmul.f32    q0, q0, q8") \
    __ASM_EMIT("vmul.f32    q1, q1, q9") \
    __ASM_EMIT(OP ".f32     q0, " SEL("q4", "q0") ", " SEL("q0", "q4")) \
    __ASM_EMIT(OP ".f32     q1, " SEL("q5", "q1") ", " SEL("q1", "q5")) \
    __ASM_EMIT("sub         %[count], $8") \
    __ASM_EMIT("vstm        %[" DST "]!, {q0-q1}") \
    /* 4x block */ \
    __ASM_EMIT("4:") \
    __ASM_EMIT("adds        %[count], $4") /* 8 - 4 */ \
    __ASM_EMIT("blt         6f") \
    __ASM_EMIT("vldm        %[" A "]!, {q0}") \
    __ASM_EMIT("vldm        %[" B "]!, {q8}") \
    __ASM_EMIT("vldm        %[" DST "], {q4}") \
    __ASM_EMIT("vmul.f32    q0, q0, q8") \
    __ASM_EMIT(OP ".f32     q0, " SEL("q4", "q0") ", " SEL("q0", "q4")) \
    __ASM_EMIT("sub         %[count], $4") \
    __ASM_EMIT("vstm        %[" DST "]!, {q0}") \
    /* 1x block */ \
    __ASM_EMIT("6:") \
    __ASM_EMIT("adds        %[count], $3") /* 4 - 3 */ \
    __ASM_EMIT("blt         8f") \
    __ASM_EMIT("7:") \
    __ASM_EMIT("vld1.32     {d0[], d1[]}, [%[" A "]]!") \
    __ASM_EMIT("vld1.32     {d16[], d17[]}, [%[" B "]]!") \
    __ASM_EMIT("vld1.32     {d8[], d9[]}, [%[" DST "]]") \
    __ASM_EMIT("vmul.f32    q0, q0, q8") \
    __ASM_EMIT(OP ".f32     q0, " SEL("q4", "q0") ", " SEL("q0", "q4")) \
    __ASM_EMIT("subs        %[count], $1") \
    __ASM_EMIT("vst1.32     {d0[0]}, [%[" DST "]]!") \
    __ASM_EMIT("bge         7b") \
    __ASM_EMIT("8:")

    void fmmul3(float *dst, const float *a, const float *b, size_t count)
    {
        ARCH_ARM_ASM
        (
            FMOP3_CORE("dst", "a", "b", "vmul", OP_DSEL)
            : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11"
        );
    }

    void fmrsub3(float *dst, const float *a, const float *b, size_t count)
    {
        ARCH_ARM_ASM
        (
            FMOP3_CORE("dst", "a", "b", "vsub", OP_RSEL)
            : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11"
        );
    }

#undef FMOP3_CORE

#define FMDIV3_CORE(DST, A, B, SEL)   \
    __ASM_EMIT("subs            %[count], $16") \
    __ASM_EMIT("blo             2f") \
    /* 16x blocks */ \
    __ASM_EMIT("1:") \
    __ASM_EMIT("vldm            %[" SEL(A, DST) "], {q0-q3}") \
    __ASM_EMIT("vldm            %[" B "]!, {q4-q7}") \
    __ASM_EMIT(SEL("vmul.f32        q0, q0, q4", ""))       /* q0 = A*B | DST */ \
    __ASM_EMIT(SEL("vmul.f32        q1, q1, q5", "")) \
    __ASM_EMIT(SEL("vmul.f32        q2, q2, q6", "")) \
    __ASM_EMIT(SEL("vmul.f32        q3, q3, q7", "")) \
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
    __ASM_EMIT("vmul.f32        q0, q12, q8")               /* q0 = s2" = s2' * (2 - R*s2) = 1/A*B | 1/DST */  \
    __ASM_EMIT("vmul.f32        q1, q13, q9") \
    __ASM_EMIT("vmul.f32        q2, q14, q10") \
    __ASM_EMIT("vmul.f32        q3, q15, q11") \
    __ASM_EMIT(SEL("", "vmul.f32        q0, q0, q4"))       /* q0 = 1/(A*B) | B/DST */ \
    __ASM_EMIT(SEL("", "vmul.f32        q1, q1, q5"))\
    __ASM_EMIT(SEL("", "vmul.f32        q2, q2, q6")) \
    __ASM_EMIT(SEL("", "vmul.f32        q3, q3, q7")) \
    __ASM_EMIT("vldm            %[" SEL(DST, A) "], {q12-q15}") /* q12 = DST | A */ \
    __ASM_EMIT("vmul.f32        q0, q0, q12")               /* q0 = DST/(A*B) | (A*B)/DST */ \
    __ASM_EMIT("vmul.f32        q1, q1, q13") \
    __ASM_EMIT("vmul.f32        q2, q2, q14") \
    __ASM_EMIT("vmul.f32        q3, q3, q15") \
    __ASM_EMIT("vstm            %[" DST "]!, {q0-q3}") \
    __ASM_EMIT("subs            %[count], $16") \
    __ASM_EMIT("add             %[" A "], $0x40") \
    __ASM_EMIT("bhs             1b") \
    /* 8x block */ \
    __ASM_EMIT("2:") \
    __ASM_EMIT("adds            %[count], $8") \
    __ASM_EMIT("blt             4f") \
    __ASM_EMIT("vldm            %[" SEL(A, DST) "], {q0-q1}") \
    __ASM_EMIT("vldm            %[" B "]!, {q4-q5}") \
    __ASM_EMIT(SEL("vmul.f32        q0, q0, q4", ""))       /* q0 = A*B | DST */ \
    __ASM_EMIT(SEL("vmul.f32        q1, q1, q5", "")) \
    __ASM_EMIT("vrecpe.f32      q8, q0")                    /* q8 = s2 */ \
    __ASM_EMIT("vrecpe.f32      q9, q1") \
    __ASM_EMIT("vrecps.f32      q12, q8, q0")               /* q12 = (2 - R*s2) */ \
    __ASM_EMIT("vrecps.f32      q13, q9, q1") \
    __ASM_EMIT("vmul.f32        q8, q12, q8")               /* q8 = s2' = s2 * (2 - R*s2) */ \
    __ASM_EMIT("vmul.f32        q9, q13, q9") \
    __ASM_EMIT("vrecps.f32      q12, q8, q0")               /* q12 = (2 - R*s2') */ \
    __ASM_EMIT("vrecps.f32      q13, q9, q1") \
    __ASM_EMIT("vmul.f32        q0, q12, q8")               /* q0 = s2" = s2' * (2 - R*s2) = 1/A*B | 1/DST */  \
    __ASM_EMIT("vmul.f32        q1, q13, q9") \
    __ASM_EMIT(SEL("", "vmul.f32        q0, q0, q4"))       /* q0 = 1/(A*B) | B/DST */ \
    __ASM_EMIT(SEL("", "vmul.f32        q1, q1, q5"))\
    __ASM_EMIT("vldm            %[" SEL(DST, A) "], {q12-q13}") \
    __ASM_EMIT("vmul.f32        q0, q0, q12")               /* q0 = DST/(A*B) | (A*B)/DST */ \
    __ASM_EMIT("vmul.f32        q1, q1, q13") \
    __ASM_EMIT("sub             %[count], $8") \
    __ASM_EMIT("vstm            %[" DST "]!, {q0-q1}") \
    __ASM_EMIT("add             %[" A "], $0x20") \
    /* 4x blocks */ \
    __ASM_EMIT("4:") \
    __ASM_EMIT("adds            %[count], $4") \
    __ASM_EMIT("blt             6f") \
    __ASM_EMIT("vldm            %[" SEL(A, DST) "], {q0}") \
    __ASM_EMIT("vldm            %[" B "]!, {q4}") \
    __ASM_EMIT(SEL("vmul.f32        q0, q0, q4", ""))       /* q0 = A*B | DST */ \
    __ASM_EMIT("vrecpe.f32      q8, q0")                    /* q8 = s2 */ \
    __ASM_EMIT("vrecps.f32      q12, q8, q0")               /* q12 = (2 - R*s2) */ \
    __ASM_EMIT("vmul.f32        q8, q12, q8")               /* q8 = s2' = s2 * (2 - R*s2) */ \
    __ASM_EMIT("vrecps.f32      q12, q8, q0")               /* q12 = (2 - R*s2') */ \
    __ASM_EMIT("vmul.f32        q0, q12, q8")               /* q0 = s2" = s2' * (2 - R*s2) = 1/A*B | 1/DST */ \
    __ASM_EMIT(SEL("", "vmul.f32        q0, q0, q4"))       /* q0 = 1/(A*B) | B/DST */ \
    __ASM_EMIT("vldm            %[" SEL(DST, A) "], {q12}") \
    __ASM_EMIT("vmul.f32        q0, q0, q12")               /* q0 = DST/(A*B) | (A*B)/DST */ \
    __ASM_EMIT("sub             %[count], $4") \
    __ASM_EMIT("vstm            %[" DST "]!, {q0}") \
    __ASM_EMIT("add             %[" A "], $0x10") \
    /* 1x blocks */ \
    __ASM_EMIT("6:") \
    __ASM_EMIT("adds            %[count], $3") \
    __ASM_EMIT("blt             8f") \
    __ASM_EMIT("7:") \
    __ASM_EMIT("vld1.32         {d0[], d1[]}, [%[" SEL(A, DST) "]]") \
    __ASM_EMIT("vld1.32         {d8[], d9[]}, [%["B "]]!") \
    __ASM_EMIT(SEL("vmul.f32        q0, q0, q4", ""))       /* q0 = A*B | DST */ \
    __ASM_EMIT("vrecpe.f32      q8, q0")                    /* q8 = s2 */ \
    __ASM_EMIT("vrecps.f32      q12, q8, q0")               /* q12 = (2 - R*s2) */ \
    __ASM_EMIT("vmul.f32        q8, q12, q8")               /* q8 = s2' = s2 * (2 - R*s2) */ \
    __ASM_EMIT("vrecps.f32      q12, q8, q0")               /* q12 = (2 - R*s2') */ \
    __ASM_EMIT("vmul.f32        q0, q12, q8")               /* q0 = s2" = s2' * (2 - R*s2) = 1/A*B | 1/DST */ \
    __ASM_EMIT(SEL("", "vmul.f32        q0, q0, q4"))       /* q0 = 1/(A*B) | B/DST */ \
    __ASM_EMIT("vld1.32         {d24[], d25[]}, [%[" SEL(DST, A) "]]") \
    __ASM_EMIT("vmul.f32        q0, q0, q12")               /* q0 = DST/(A*B) | (A*B)/DST */ \
    __ASM_EMIT("subs            %[count], $1") \
    __ASM_EMIT("vst1.32         {d0[0]}, [%[" DST "]]!") \
    __ASM_EMIT("add             %[" A "], $0x04") \
    __ASM_EMIT("bge             7b") \
    __ASM_EMIT("8:")

    void fmdiv3(float *dst, const float *a, const float *b, size_t count)
    {
        ARCH_ARM_ASM
        (
            FMDIV3_CORE("dst", "a", "b", OP_DSEL)
            : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    void fmrdiv3(float *dst, const float *a, const float *b, size_t count)
    {
        ARCH_ARM_ASM
        (
            FMDIV3_CORE("dst", "a", "b", OP_RSEL)
            : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

#undef FMDIV3_CORE

#define FMADDSUB4_CORE(DST, A, B, C, OP) \
    __ASM_EMIT("subs        %[count], $16") \
    __ASM_EMIT("blo         2f") \
    /* 16x blocks */ \
    __ASM_EMIT("1:") \
    __ASM_EMIT("vldm        %[" A "]!, {q0-q3}") \
    __ASM_EMIT("vldm        %[" B "]!, {q4-q7}") \
    __ASM_EMIT("vldm        %[" C "]!, {q8-q11}") \
    __ASM_EMIT(OP ".f32     q0, q4, q8") \
    __ASM_EMIT(OP ".f32     q1, q5, q9") \
    __ASM_EMIT(OP ".f32     q2, q6, q10") \
    __ASM_EMIT(OP ".f32     q3, q7, q11") \
    __ASM_EMIT("subs        %[count], $16") \
    __ASM_EMIT("vstm        %[" DST "]!, {q0-q3}") \
    __ASM_EMIT("bhs         1b") \
    /* 8x block */ \
    __ASM_EMIT("2:") \
    __ASM_EMIT("adds        %[count], $8") /* 16 - 8 */ \
    __ASM_EMIT("blt         4f") \
    __ASM_EMIT("vldm        %[" A "]!, {q0-q1}") \
    __ASM_EMIT("vldm        %[" B "]!, {q4-q5}") \
    __ASM_EMIT("vldm        %[" C "]!, {q8-q9}") \
    __ASM_EMIT(OP ".f32     q0, q4, q8") \
    __ASM_EMIT(OP ".f32     q1, q5, q9") \
    __ASM_EMIT("sub         %[count], $8") \
    __ASM_EMIT("vstm        %[" DST "]!, {q0-q1}") \
    /* 4x block */ \
    __ASM_EMIT("4:") \
    __ASM_EMIT("adds        %[count], $4") /* 8 - 4 */ \
    __ASM_EMIT("blt         6f") \
    __ASM_EMIT("vldm        %[" A "]!, {q0}") \
    __ASM_EMIT("vldm        %[" B "]!, {q4}") \
    __ASM_EMIT("vldm        %[" C "]!, {q8}") \
    __ASM_EMIT(OP ".f32     q0, q4, q8") \
    __ASM_EMIT("sub         %[count], $4") \
    __ASM_EMIT("vstm        %[" DST "]!, {q0}") \
    /* 1x block */ \
    __ASM_EMIT("6:") \
    __ASM_EMIT("adds        %[count], $3") /* 4 - 3 */ \
    __ASM_EMIT("blt         8f") \
    __ASM_EMIT("7:") \
    __ASM_EMIT("vld1.32     {d0[], d1[]}, [%[" A "]]!") \
    __ASM_EMIT("vld1.32     {d8[], d9[]}, [%[" B "]]!") \
    __ASM_EMIT("vld1.32     {d16[], d17[]}, [%[" C "]]!") \
    __ASM_EMIT(OP ".f32     q0, q4, q8") \
    __ASM_EMIT("subs        %[count], $1") \
    __ASM_EMIT("vst1.32     {d0[0]}, [%[" DST "]]!") \
    __ASM_EMIT("bge         7b") \
    __ASM_EMIT("8:")

    void fmadd4(float *dst, const float *a, const float *b, const float *c, size_t count)
    {
        ARCH_ARM_ASM
        (
            FMADDSUB4_CORE("dst", "a", "b", "c", "vfma")
            : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b), [c] "+r" (c),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11"
        );
    }

    void fmsub4(float *dst, const float *a, const float *b, const float *c, size_t count)
    {
        ARCH_ARM_ASM
        (
            FMADDSUB4_CORE("dst", "a", "b", "c", "vfms")
            : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b), [c] "+r" (c),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11"
        );
    }

#undef FMADDSUB4_CORE

#define FMOP4_CORE(DST, A, B, C, OP, SEL) \
    __ASM_EMIT("subs        %[count], $16") \
    __ASM_EMIT("blo         2f") \
    /* 16x blocks */ \
    __ASM_EMIT("1:") \
    __ASM_EMIT("vldm        %[" A "]!, {q4-q7}") \
    __ASM_EMIT("vldm        %[" B "]!, {q0-q3}") \
    __ASM_EMIT("vldm        %[" C "]!, {q8-q11}") \
    __ASM_EMIT("vmul.f32    q0, q0, q8") \
    __ASM_EMIT("vmul.f32    q1, q1, q9") \
    __ASM_EMIT("vmul.f32    q2, q2, q10") \
    __ASM_EMIT("vmul.f32    q3, q3, q11") \
    __ASM_EMIT(OP ".f32     q0, " SEL("q4", "q0") ", " SEL("q0", "q4")) \
    __ASM_EMIT(OP ".f32     q1, " SEL("q5", "q1") ", " SEL("q1", "q5")) \
    __ASM_EMIT(OP ".f32     q2, " SEL("q6", "q2") ", " SEL("q2", "q6")) \
    __ASM_EMIT(OP ".f32     q3, " SEL("q7", "q3") ", " SEL("q3", "q7")) \
    __ASM_EMIT("subs        %[count], $16") \
    __ASM_EMIT("vstm        %[" DST "]!, {q0-q3}") \
    __ASM_EMIT("bhs         1b") \
    /* 8x block */ \
    __ASM_EMIT("2:") \
    __ASM_EMIT("adds        %[count], $8") /* 16 - 8 */ \
    __ASM_EMIT("blt         4f") \
    __ASM_EMIT("vldm        %[" A "]!, {q4-q5}") \
    __ASM_EMIT("vldm        %[" B "]!, {q0-q1}") \
    __ASM_EMIT("vldm        %[" C "]!, {q8-q9}") \
    __ASM_EMIT("vmul.f32    q0, q0, q8") \
    __ASM_EMIT("vmul.f32    q1, q1, q9") \
    __ASM_EMIT(OP ".f32     q0, " SEL("q4", "q0") ", " SEL("q0", "q4")) \
    __ASM_EMIT(OP ".f32     q1, " SEL("q5", "q1") ", " SEL("q1", "q5")) \
    __ASM_EMIT("sub         %[count], $8") \
    __ASM_EMIT("vstm        %[" DST "]!, {q0-q1}") \
    /* 4x block */ \
    __ASM_EMIT("4:") \
    __ASM_EMIT("adds        %[count], $4") /* 8 - 4 */ \
    __ASM_EMIT("blt         6f") \
    __ASM_EMIT("vldm        %[" A "]!, {q4}") \
    __ASM_EMIT("vldm        %[" B "]!, {q0}") \
    __ASM_EMIT("vldm        %[" C "]!, {q8}") \
    __ASM_EMIT("vmul.f32    q0, q0, q8") \
    __ASM_EMIT(OP ".f32     q0, " SEL("q4", "q0") ", " SEL("q0", "q4")) \
    __ASM_EMIT("sub         %[count], $4") \
    __ASM_EMIT("vstm        %[" DST "]!, {q0}") \
    /* 1x block */ \
    __ASM_EMIT("6:") \
    __ASM_EMIT("adds        %[count], $3") /* 4 - 3 */ \
    __ASM_EMIT("blt         8f") \
    __ASM_EMIT("7:") \
    __ASM_EMIT("vld1.32     {d8[], d9[]}, [%[" A "]]!") \
    __ASM_EMIT("vld1.32     {d0[], d1[]}, [%[" B "]]!") \
    __ASM_EMIT("vld1.32     {d16[], d17[]}, [%[" C "]]!") \
    __ASM_EMIT("vmul.f32    q0, q0, q8") \
    __ASM_EMIT(OP ".f32     q0, " SEL("q4", "q0") ", " SEL("q0", "q4")) \
    __ASM_EMIT("subs        %[count], $1") \
    __ASM_EMIT("vst1.32     {d0[0]}, [%[" DST "]]!") \
    __ASM_EMIT("bge         7b") \
    __ASM_EMIT("8:")

    void fmmul4(float *dst, const float *a, const float *b, const float *c, size_t count)
    {
        ARCH_ARM_ASM
        (
            FMOP4_CORE("dst", "a", "b", "c", "vmul", OP_DSEL)
            : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b), [c] "+r" (c),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11"
        );
    }

    void fmrsub4(float *dst, const float *a, const float *b, const float *c, size_t count)
    {
        ARCH_ARM_ASM
        (
            FMOP4_CORE("dst", "a", "b", "c", "vsub", OP_RSEL)
            : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b), [c] "+r" (c),
               [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11"
        );
    }

#undef FMOP4_CORE

#define FMDIV4_CORE(DST, A, B, C, SEL)   \
    __ASM_EMIT("subs            %[count], $16") \
    __ASM_EMIT("blo             2f") \
    /* 16x blocks */ \
    __ASM_EMIT("1:") \
    __ASM_EMIT("vldm            %[" SEL(B, A) "]!, {q0-q3}") \
    __ASM_EMIT("vldm            %[" C "]!, {q4-q7}") \
    __ASM_EMIT(SEL("vmul.f32        q0, q0, q4", ""))       /* q0 = B*C | A */ \
    __ASM_EMIT(SEL("vmul.f32        q1, q1, q5", "")) \
    __ASM_EMIT(SEL("vmul.f32        q2, q2, q6", "")) \
    __ASM_EMIT(SEL("vmul.f32        q3, q3, q7", "")) \
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
    __ASM_EMIT("vmul.f32        q0, q12, q8")               /* q0 = s2" = s2' * (2 - R*s2) = 1/(B*C) | 1/A */  \
    __ASM_EMIT("vmul.f32        q1, q13, q9") \
    __ASM_EMIT("vmul.f32        q2, q14, q10") \
    __ASM_EMIT("vmul.f32        q3, q15, q11") \
    __ASM_EMIT(SEL("", "vmul.f32        q0, q0, q4"))       /* q0 = 1/(B*C) | C/A */ \
    __ASM_EMIT(SEL("", "vmul.f32        q1, q1, q5")) \
    __ASM_EMIT(SEL("", "vmul.f32        q2, q2, q6")) \
    __ASM_EMIT(SEL("", "vmul.f32        q3, q3, q7")) \
    __ASM_EMIT("vldm            %[" SEL(A, B) "]!, {q12-q15}") /* q12 = A | B */ \
    __ASM_EMIT("vmul.f32        q0, q0, q12")               /* q0 = A/(B*C) | (B*C)/A */ \
    __ASM_EMIT("vmul.f32        q1, q1, q13") \
    __ASM_EMIT("vmul.f32        q2, q2, q14") \
    __ASM_EMIT("vmul.f32        q3, q3, q15") \
    __ASM_EMIT("vstm            %[" DST "]!, {q0-q3}") \
    __ASM_EMIT("subs            %[count], $16") \
    __ASM_EMIT("bhs             1b") \
    /* 8x block */ \
    __ASM_EMIT("2:") \
    __ASM_EMIT("adds            %[count], $8") \
    __ASM_EMIT("blt             4f") \
    __ASM_EMIT("vldm            %[" SEL(B, A) "]!, {q0-q1}") \
    __ASM_EMIT("vldm            %[" C "]!, {q4-q5}") \
    __ASM_EMIT(SEL("vmul.f32        q0, q0, q4", ""))       /* q0 = B*C | A */ \
    __ASM_EMIT(SEL("vmul.f32        q1, q1, q5", "")) \
    __ASM_EMIT("vrecpe.f32      q8, q0")                    /* q8 = s2 */ \
    __ASM_EMIT("vrecpe.f32      q9, q1") \
    __ASM_EMIT("vrecps.f32      q12, q8, q0")               /* q12 = (2 - R*s2) */ \
    __ASM_EMIT("vrecps.f32      q13, q9, q1") \
    __ASM_EMIT("vmul.f32        q8, q12, q8")               /* q8 = s2' = s2 * (2 - R*s2) */ \
    __ASM_EMIT("vmul.f32        q9, q13, q9") \
    __ASM_EMIT("vrecps.f32      q12, q8, q0")               /* q12 = (2 - R*s2') */ \
    __ASM_EMIT("vrecps.f32      q13, q9, q1") \
    __ASM_EMIT("vmul.f32        q0, q12, q8")               /* q0 = s2" = s2' * (2 - R*s2) = 1/(B*C) | 1/A */  \
    __ASM_EMIT("vmul.f32        q1, q13, q9") \
    __ASM_EMIT(SEL("", "vmul.f32        q0, q0, q4"))       /* q0 = 1/(B*C) | C/A */ \
    __ASM_EMIT(SEL("", "vmul.f32        q1, q1, q5")) \
    __ASM_EMIT("vldm            %[" SEL(A, B) "]!, {q12-q13}") /* q12 = A | B */ \
    __ASM_EMIT("vmul.f32        q0, q0, q12")               /* q0 = A/(B*C) | (B*C)/A */ \
    __ASM_EMIT("vmul.f32        q1, q1, q13") \
    __ASM_EMIT("sub             %[count], $8") \
    __ASM_EMIT("vstm            %[" DST "]!, {q0-q1}") \
    /* 4x blocks */ \
    __ASM_EMIT("4:") \
    __ASM_EMIT("adds            %[count], $4") \
    __ASM_EMIT("blt             6f") \
    __ASM_EMIT("vldm            %[" SEL(B, A) "]!, {q0}") \
    __ASM_EMIT("vldm            %[" C "]!, {q4}") \
    __ASM_EMIT(SEL("vmul.f32        q0, q0, q4", ""))       /* q0 = B*C | A */ \
    __ASM_EMIT("vrecpe.f32      q8, q0")                    /* q8 = s2 */ \
    __ASM_EMIT("vrecps.f32      q12, q8, q0")               /* q12 = (2 - R*s2) */ \
    __ASM_EMIT("vmul.f32        q8, q12, q8")               /* q8 = s2' = s2 * (2 - R*s2) */ \
    __ASM_EMIT("vrecps.f32      q12, q8, q0")               /* q12 = (2 - R*s2') */ \
    __ASM_EMIT("vmul.f32        q0, q12, q8")               /* q0 = s2" = s2' * (2 - R*s2) = 1/(B*C) | 1/A */  \
    __ASM_EMIT(SEL("", "vmul.f32        q0, q0, q4"))       /* q0 = 1/(B*C) | C/A */ \
    __ASM_EMIT("vldm            %[" SEL(A, B) "]!, {q12}")  /* q12 = A | B */ \
    __ASM_EMIT("vmul.f32        q0, q0, q12")               /* q0 = A/(B*C) | (B*C)/A */ \
    __ASM_EMIT("sub             %[count], $4") \
    __ASM_EMIT("vstm            %[" DST "]!, {q0}") \
    /* 1x blocks */ \
    __ASM_EMIT("6:") \
    __ASM_EMIT("adds            %[count], $3") \
    __ASM_EMIT("blt             8f") \
    __ASM_EMIT("7:") \
    __ASM_EMIT("vld1.32         {d0[], d1[]}, [%[" SEL(B, A) "]]!") \
    __ASM_EMIT("vld1.32         {d8[], d9[]}, [%[" C "]]!") \
    __ASM_EMIT(SEL("vmul.f32        q0, q0, q4", ""))       /* q0 = B*C | A */ \
    __ASM_EMIT("vrecpe.f32      q8, q0")                    /* q8 = s2 */ \
    __ASM_EMIT("vrecps.f32      q12, q8, q0")               /* q12 = (2 - R*s2) */ \
    __ASM_EMIT("vmul.f32        q8, q12, q8")               /* q8 = s2' = s2 * (2 - R*s2) */ \
    __ASM_EMIT("vrecps.f32      q12, q8, q0")               /* q12 = (2 - R*s2') */ \
    __ASM_EMIT("vmul.f32        q0, q12, q8")               /* q0 = s2" = s2' * (2 - R*s2) = 1/(B*C) | 1/A */  \
    __ASM_EMIT(SEL("", "vmul.f32        q0, q0, q4"))       /* q0 = 1/(B*C) | C/A */ \
    __ASM_EMIT("vld1.32         {d24[], d25[]}, [%[" SEL(A, B) "]]!") \
    __ASM_EMIT("vmul.f32        q0, q0, q12")               /* q0 = A/(B*C) | (B*C)/A */ \
    __ASM_EMIT("subs            %[count], $1") \
    __ASM_EMIT("vst1.32         {d0[0]}, [%[" DST "]]!") \
    __ASM_EMIT("bge             7b") \
    __ASM_EMIT("8:")

    void fmdiv4(float *dst, const float *a, const float *b, const float *c, size_t count)
    {
        ARCH_ARM_ASM
        (
            FMDIV4_CORE("dst", "a", "b", "c", OP_DSEL)
            : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b), [c] "+r" (c),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    void fmrdiv4(float *dst, const float *a, const float *b, const float *c, size_t count)
    {
        ARCH_ARM_ASM
        (
            FMDIV4_CORE("dst", "a", "b", "c", OP_RSEL)
            : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b), [c] "+r" (c),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

#undef FMDIV4_CORE

#define FMMOD_VV_CORE(DST, A, INCA, B, INCB, C, INCC, SEL)   \
    __ASM_EMIT("subs            %[count], $16") \
    __ASM_EMIT("blo             2f") \
    /* 16x blocks */ \
    __ASM_EMIT("1:") \
    __ASM_EMIT("vldm            %[" C "]" INCC ", {q12-q15}") \
    __ASM_EMIT("vldm            %[" SEL(B, A) "]" SEL(INCB, INCA) ", {q4-q7}") \
    __ASM_EMIT(SEL("vmul.f32    q4, q4, q12", ""))                  /* q4 = d */ \
    __ASM_EMIT(SEL("vmul.f32    q5, q5, q13", "")) \
    __ASM_EMIT(SEL("vmul.f32    q6, q6, q14", "")) \
    __ASM_EMIT(SEL("vmul.f32    q7, q7, q15", "")) \
    __ASM_EMIT("vrecpe.f32      q8, q4")                            /* q8 = s2 */ \
    __ASM_EMIT("vrecpe.f32      q9, q5") \
    __ASM_EMIT("vrecpe.f32      q10, q6") \
    __ASM_EMIT("vrecpe.f32      q11, q7") \
    __ASM_EMIT("vrecps.f32      q0, q8, q4")                        /* q0 = (2 - R*s2) */ \
    __ASM_EMIT("vrecps.f32      q1, q9, q5") \
    __ASM_EMIT("vrecps.f32      q2, q10, q6") \
    __ASM_EMIT("vrecps.f32      q3, q11, q7") \
    __ASM_EMIT("vmul.f32        q8, q0, q8")                        /* q8 = s2' = s2 * (2 - R*s2) */ \
    __ASM_EMIT("vmul.f32        q9, q1, q9") \
    __ASM_EMIT("vmul.f32        q10, q2, q10") \
    __ASM_EMIT("vmul.f32        q11, q3, q11") \
    __ASM_EMIT("vrecps.f32      q0, q8, q4")                        /* q0 = (2 - R*s2') */ \
    __ASM_EMIT("vrecps.f32      q1, q9, q5") \
    __ASM_EMIT("vrecps.f32      q2, q10, q6") \
    __ASM_EMIT("vrecps.f32      q3, q11, q7") \
    __ASM_EMIT("vmul.f32        q8, q0, q8")                        /* q8 = s2" = s2' * (2 - R*s2) = 1/d */  \
    __ASM_EMIT("vmul.f32        q9, q1, q9") \
    __ASM_EMIT("vmul.f32        q10, q2, q10") \
    __ASM_EMIT("vmul.f32        q11, q3, q11") \
    __ASM_EMIT("vldm            %[" SEL(A, B) "]" SEL(INCA, INCB) ", {q0-q3}") \
    __ASM_EMIT(SEL("", "vmul.f32    q0, q0, q12"))                  /* q0 = s */ \
    __ASM_EMIT(SEL("", "vmul.f32    q1, q1, q13"))\
    __ASM_EMIT(SEL("", "vmul.f32    q2, q2, q14")) \
    __ASM_EMIT(SEL("", "vmul.f32    q3, q3, q15")) \
    __ASM_EMIT("vmul.f32        q8, q8, q0")                        /* q8 = s/d */ \
    __ASM_EMIT("vmul.f32        q9, q9, q1") \
    __ASM_EMIT("vmul.f32        q10, q10, q2") \
    __ASM_EMIT("vmul.f32        q11, q11, q3") \
    __ASM_EMIT("vcvt.s32.f32    q8, q8") \
    __ASM_EMIT("vcvt.s32.f32    q9, q9") \
    __ASM_EMIT("vcvt.s32.f32    q10, q10") \
    __ASM_EMIT("vcvt.s32.f32    q11, q11") \
    __ASM_EMIT("vcvt.f32.s32    q8, q8") \
    __ASM_EMIT("vcvt.f32.s32    q9, q9") \
    __ASM_EMIT("vcvt.f32.s32    q10, q10") \
    __ASM_EMIT("vcvt.f32.s32    q11, q11") \
    __ASM_EMIT("vmls.f32        q0, q4, q8")                        /* q0 = s - d*int(s/d) */ \
    __ASM_EMIT("vmls.f32        q1, q5, q9") \
    __ASM_EMIT("vmls.f32        q2, q6, q10") \
    __ASM_EMIT("vmls.f32        q3, q7, q11") \
    __ASM_EMIT("vstm            %[" DST "]!, {q0-q3}") \
    __ASM_EMIT("subs            %[count], $16") \
    __ASM_EMIT("bhs             1b") \
    /* 8x block */ \
    __ASM_EMIT("2:") \
    __ASM_EMIT("adds            %[count], $8") \
    __ASM_EMIT("blt             4f") \
    __ASM_EMIT("vldm            %[" C "]" INCC ", {q12-q13}") \
    __ASM_EMIT("vldm            %[" SEL(B, A) "]" SEL(INCB, INCA) ", {q4-q5}") \
    __ASM_EMIT(SEL("vmul.f32    q4, q4, q12", ""))                  /* q4 = d */ \
    __ASM_EMIT(SEL("vmul.f32    q5, q5, q13", "")) \
    __ASM_EMIT("vrecpe.f32      q8, q4")                            /* q8 = s2 */ \
    __ASM_EMIT("vrecpe.f32      q9, q5") \
    __ASM_EMIT("vrecps.f32      q0, q8, q4")                        /* q0 = (2 - R*s2) */ \
    __ASM_EMIT("vrecps.f32      q1, q9, q5") \
    __ASM_EMIT("vmul.f32        q8, q0, q8")                        /* q8 = s2' = s2 * (2 - R*s2) */ \
    __ASM_EMIT("vmul.f32        q9, q1, q9") \
    __ASM_EMIT("vrecps.f32      q0, q8, q4")                        /* q0 = (2 - R*s2') */ \
    __ASM_EMIT("vrecps.f32      q1, q9, q5") \
    __ASM_EMIT("vmul.f32        q8, q0, q8")                        /* q8 = s2" = s2' * (2 - R*s2) = 1/d */  \
    __ASM_EMIT("vmul.f32        q9, q1, q9") \
    __ASM_EMIT("vldm            %[" SEL(A, B) "]" SEL(INCA, INCB) ", {q0-q1}") \
    __ASM_EMIT(SEL("", "vmul.f32    q0, q0, q12"))                  /* q0 = s */ \
    __ASM_EMIT(SEL("", "vmul.f32    q1, q1, q13"))\
    __ASM_EMIT("vmul.f32        q8, q8, q0")                        /* q8 = s/d */ \
    __ASM_EMIT("vmul.f32        q9, q9, q1") \
    __ASM_EMIT("vcvt.s32.f32    q8, q8") \
    __ASM_EMIT("vcvt.s32.f32    q9, q9") \
    __ASM_EMIT("vcvt.f32.s32    q8, q8") \
    __ASM_EMIT("vcvt.f32.s32    q9, q9") \
    __ASM_EMIT("vmls.f32        q0, q4, q8")                        /* q0 = s - d*int(s/d) */ \
    __ASM_EMIT("vmls.f32        q1, q5, q9") \
    __ASM_EMIT("vstm            %[" DST "]!, {q0-q1}") \
    __ASM_EMIT("sub             %[count], $8") \
    /* 4x blocks */ \
    __ASM_EMIT("4:") \
    __ASM_EMIT("adds            %[count], $4") \
    __ASM_EMIT("blt             6f") \
    __ASM_EMIT("vldm            %[" C "]" INCC ", {q12}") \
    __ASM_EMIT("vldm            %[" SEL(B, A) "]" SEL(INCB, INCA) ", {q4}") \
    __ASM_EMIT(SEL("vmul.f32    q4, q4, q12", ""))                  /* q4 = d */ \
    __ASM_EMIT("vrecpe.f32      q8, q4")                            /* q8 = s2 */ \
    __ASM_EMIT("vrecps.f32      q0, q8, q4")                        /* q0 = (2 - R*s2) */ \
    __ASM_EMIT("vmul.f32        q8, q0, q8")                        /* q8 = s2' = s2 * (2 - R*s2) */ \
    __ASM_EMIT("vrecps.f32      q0, q8, q4")                        /* q0 = (2 - R*s2') */ \
    __ASM_EMIT("vmul.f32        q8, q0, q8")                        /* q8 = s2" = s2' * (2 - R*s2) = 1/d */  \
    __ASM_EMIT("vldm            %[" SEL(A, B) "]" SEL(INCA, INCB) ", {q0}") \
    __ASM_EMIT(SEL("", "vmul.f32    q0, q0, q12"))                  /* q0 = s */ \
    __ASM_EMIT("vmul.f32        q8, q8, q0")                        /* q8 = s/d */ \
    __ASM_EMIT("vcvt.s32.f32    q8, q8") \
    __ASM_EMIT("vcvt.f32.s32    q8, q8") \
    __ASM_EMIT("vmls.f32        q0, q4, q8")                        /* q0 = s - d*int(s/d) */ \
    __ASM_EMIT("vstm            %[" DST "]!, {q0}") \
    __ASM_EMIT("sub             %[count], $4") \
    /* 1x blocks */ \
    __ASM_EMIT("6:") \
    __ASM_EMIT("adds            %[count], $3") \
    __ASM_EMIT("blt             8f") \
    __ASM_EMIT("7:") \
    __ASM_EMIT("vld1.32         {d24[], d25[]}, [%[" C "]]" INCC) \
    __ASM_EMIT("vld1.32         {d8[], d9[]}, [%[" SEL(B, A) "]]" SEL(INCB, INCA)) \
    __ASM_EMIT(SEL("vmul.f32    q4, q4, q12", ""))                  /* q4 = d */ \
    __ASM_EMIT("vrecpe.f32      q8, q4")                            /* q8 = s2 */ \
    __ASM_EMIT("vrecps.f32      q0, q8, q4")                        /* q0 = (2 - R*s2) */ \
    __ASM_EMIT("vmul.f32        q8, q0, q8")                        /* q8 = s2' = s2 * (2 - R*s2) */ \
    __ASM_EMIT("vrecps.f32      q0, q8, q4")                        /* q0 = (2 - R*s2') */ \
    __ASM_EMIT("vmul.f32        q8, q0, q8")                        /* q8 = s2" = s2' * (2 - R*s2) = 1/d */  \
    __ASM_EMIT("vld1.32         {d0[], d1[]}, [%[" SEL(A, B) "]]" SEL(INCA, INCB)) \
    __ASM_EMIT(SEL("", "vmul.f32    q0, q0, q12"))                  /* q0 = s */ \
    __ASM_EMIT("vmul.f32        q8, q8, q0")                        /* q8 = s/d */ \
    __ASM_EMIT("vcvt.s32.f32    q8, q8") \
    __ASM_EMIT("vcvt.f32.s32    q8, q8") \
    __ASM_EMIT("vmls.f32        q0, q4, q8")                        /* q0 = s - d*int(s/d) */ \
    __ASM_EMIT("vst1.32         {d0[0]}, [%[" DST "]]!") \
    __ASM_EMIT("subs            %[count], $1") \
    __ASM_EMIT("bge             7b") \
    __ASM_EMIT("8:")

    void fmmod3(float *dst, const float *a, const float *b, size_t count)
    {
        ARCH_ARM_ASM
        (
            FMMOD_VV_CORE("dst", "dst", "", "a", "!", "b", "!", OP_DSEL)
            : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    void fmrmod3(float *dst, const float *a, const float *b, size_t count)
    {
        ARCH_ARM_ASM
        (
            FMMOD_VV_CORE("dst", "dst", "", "a", "!", "b", "!", OP_RSEL)
            : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    void fmmod4(float *dst, const float *a, const float *b, const float *c, size_t count)
    {
        ARCH_ARM_ASM
        (
            FMMOD_VV_CORE("dst", "a", "!", "b", "!", "c", "!", OP_DSEL)
            : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b), [c] "+r" (c),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    void fmrmod4(float *dst, const float *a, const float *b, const float *c, size_t count)
    {
        ARCH_ARM_ASM
        (
            FMMOD_VV_CORE("dst", "a", "!", "b", "!", "c", "!", OP_RSEL)
            : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b), [c] "+r" (c),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

#undef FMMOD_VV_CORE

#undef OP_DSEL
#undef OP_RSEL
}

#endif /* DSP_ARCH_ARM_NEON_D32_PMATH_FMOP_VV_H_ */
