/*
 * pmath.h
 *
 *  Created on: 21 сент. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_ARM_NEON_D32_PMATH_H_
#define DSP_ARCH_ARM_NEON_D32_PMATH_H_

#ifndef DSP_ARCH_ARM_NEON_32_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_ARM_NEON_32_IMPL */

namespace neon_d32
{
#define OP_CORE(OP, DST, SRC1, SRC2)   \
    __ASM_EMIT("subs        %[count], $32") \
    __ASM_EMIT("blo         2f") \
    /* 32x blocks */ \
    __ASM_EMIT("1:") \
    __ASM_EMIT("vld1.32     {q0-q1}, [%[" SRC1 "]]!") \
    __ASM_EMIT("vld1.32     {q8-q9}, [%[" SRC2 "]]!") \
    __ASM_EMIT("vld1.32     {q2-q3}, [%[" SRC1 "]]!") \
    __ASM_EMIT("vld1.32     {q10-q11}, [%[" SRC2 "]]!") \
    __ASM_EMIT("vld1.32     {q4-q5}, [%[" SRC1 "]]!") \
    __ASM_EMIT("vld1.32     {q12-q13}, [%[" SRC2 "]]!") \
    __ASM_EMIT("vld1.32     {q6-q7}, [%[" SRC1 "]]!") \
    __ASM_EMIT("vld1.32     {q14-q15}, [%[" SRC2 "]]!") \
    __ASM_EMIT(OP "         q0, q0, q8") \
    __ASM_EMIT(OP "         q1, q1, q9") \
    __ASM_EMIT(OP "         q2, q2, q10") \
    __ASM_EMIT(OP "         q3, q3, q11") \
    __ASM_EMIT("vst1.32     {q0-q1}, [%[" DST "]]!") \
    __ASM_EMIT(OP "         q4, q4, q12") \
    __ASM_EMIT(OP "         q5, q5, q13") \
    __ASM_EMIT("vst1.32     {q2-q3}, [%[" DST "]]!") \
    __ASM_EMIT(OP "         q6, q6, q14") \
    __ASM_EMIT(OP "         q7, q7, q15") \
    __ASM_EMIT("vst1.32     {q4-q5}, [%[" DST "]]!") \
    __ASM_EMIT("subs        %[count], $32") \
    __ASM_EMIT("vst1.32     {q6-q7}, [%[" DST "]]!") \
    __ASM_EMIT("bhs         1b") \
    /* 16x block */ \
    __ASM_EMIT("2:") \
    __ASM_EMIT("adds        %[count], $16") \
    __ASM_EMIT("blt         4f") \
    __ASM_EMIT("vld1.32     {q0-q1}, [%[" SRC1 "]]!") \
    __ASM_EMIT("vld1.32     {q8-q9}, [%[" SRC2 "]]!") \
    __ASM_EMIT("vld1.32     {q2-q3}, [%[" SRC1 "]]!") \
    __ASM_EMIT("vld1.32     {q10-q11}, [%[" SRC2 "]]!") \
    __ASM_EMIT(OP "         q0, q0, q8") \
    __ASM_EMIT(OP "         q1, q1, q9") \
    __ASM_EMIT(OP "         q2, q2, q10") \
    __ASM_EMIT(OP "         q3, q3, q11") \
    __ASM_EMIT("vst1.32     {q0-q1}, [%[" DST "]]!") \
    __ASM_EMIT("sub         %[count], $16") \
    __ASM_EMIT("vst1.32     {q2-q3}, [%[" DST "]]!") \
    /* 8x block */ \
    __ASM_EMIT("4:") \
    __ASM_EMIT("adds        %[count], $8") \
    __ASM_EMIT("blt         6f") \
    __ASM_EMIT("vld1.32     {q0-q1}, [%[" SRC1 "]]!") \
    __ASM_EMIT("vld1.32     {q8-q9}, [%[" SRC2 "]]!") \
    __ASM_EMIT(OP "         q0, q0, q8") \
    __ASM_EMIT(OP "         q1, q1, q9") \
    __ASM_EMIT("sub         %[count], $8") \
    __ASM_EMIT("vst1.32     {q0-q1}, [%[" DST "]]!") \
    /* 4x blocks */ \
    __ASM_EMIT("6:") \
    __ASM_EMIT("adds        %[count], $4") \
    __ASM_EMIT("blt         8f") \
    __ASM_EMIT("vld1.32     {q0}, [%[" SRC1 "]]!") \
    __ASM_EMIT("vld1.32     {q8}, [%[" SRC2 "]]!") \
    __ASM_EMIT(OP "         q0, q0, q8") \
    __ASM_EMIT("sub         %[count], $4") \
    __ASM_EMIT("vst1.32     {q0}, [%[" DST "]]!") \
    /* 1x blocks */ \
    __ASM_EMIT("8:") \
    __ASM_EMIT("adds        %[count], $3") \
    __ASM_EMIT("blt         10f") \
    __ASM_EMIT("9:") \
    __ASM_EMIT("vldm.32     %[" SRC1 "]!, {s0}") \
    __ASM_EMIT("vldm.32     %[" SRC2 "]!, {s1}") \
    __ASM_EMIT(OP "         s0, s0, s1") \
    __ASM_EMIT("vstm.32     %[" DST "]!, {s0}") \
    __ASM_EMIT("subs        %[count], $1") \
    __ASM_EMIT("bge         9b") \
    __ASM_EMIT("10:")

    void add2(float *dst, const float *src, size_t count)
    {
        IF_ARCH_ARM(float *src2);

        ARCH_ARM_ASM
        (
            __ASM_EMIT("mov     %[src2], %[dst]")
            OP_CORE("vadd.f32", "dst", "src1", "src2")
            : [dst] "+r" (dst), [src1] "+r" (src), [src2] "=&r" (src2),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    void sub2(float *dst, const float *src, size_t count)
    {
        IF_ARCH_ARM(float *src2);

        ARCH_ARM_ASM
        (
            __ASM_EMIT("mov     %[src1], %[dst]")
            OP_CORE("vsub.f32", "dst", "src1", "src2")
            : [dst] "+r" (dst), [src1] "=&r" (src2), [src2] "+r" (src),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    void mul2(float *dst, const float *src, size_t count)
    {
        IF_ARCH_ARM(float *src2);

        ARCH_ARM_ASM
        (
            __ASM_EMIT("mov     %[src2], %[dst]")
            OP_CORE("vmul.f32", "dst", "src1", "src2")
            : [dst] "+r" (dst), [src1] "+r" (src), [src2] "=&r" (src2),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    void add3(float *dst, const float *src1, const float *src2, size_t count)
    {
        IF_ARCH_ARM(size_t off);

        ARCH_ARM_ASM
        (
            OP_CORE("vadd.f32", "dst", "src1", "src2")
            : [dst] "+r" (dst), [src1] "+r" (src1), [src2] "+r" (src2),
              [off] "=&r" (off), [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    void sub3(float *dst, const float *src1, const float *src2, size_t count)
    {
        ARCH_ARM_ASM
        (
            OP_CORE("vsub.f32", "dst", "src1", "src2")
            : [dst] "+r" (dst), [src1] "+r" (src1), [src2] "+r" (src2),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    void mul3(float *dst, const float *src1, const float *src2, size_t count)
    {
        ARCH_ARM_ASM
        (
            OP_CORE("vmul.f32", "dst", "src1", "src2")
            : [dst] "+r" (dst), [src1] "+r" (src1), [src2] "+r" (src2),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

#undef OP_CORE

#define OP_ABS_CORE(OP, DST, SRC1, SRC2)   \
    __ASM_EMIT("subs        %[count], $32") \
    __ASM_EMIT("blo         2f") \
    /* 32x blocks */ \
    __ASM_EMIT("1:") \
    __ASM_EMIT("vld1.32     {q0-q1}, [%[" SRC1 "]]!") \
    __ASM_EMIT("vld1.32     {q8-q9}, [%[" SRC2 "]]!") \
    __ASM_EMIT("vld1.32     {q2-q3}, [%[" SRC1 "]]!") \
    __ASM_EMIT("vabs.f32    q8, q8") \
    __ASM_EMIT("vld1.32     {q10-q11}, [%[" SRC2 "]]!") \
    __ASM_EMIT("vabs.f32    q9, q9") \
    __ASM_EMIT("vld1.32     {q4-q5}, [%[" SRC1 "]]!") \
    __ASM_EMIT("vabs.f32    q10, q10") \
    __ASM_EMIT("vld1.32     {q12-q13}, [%[" SRC2 "]]!") \
    __ASM_EMIT("vabs.f32    q11, q11") \
    __ASM_EMIT("vld1.32     {q6-q7}, [%[" SRC1 "]]!") \
    __ASM_EMIT("vabs.f32    q12, q12") \
    __ASM_EMIT("vld1.32     {q14-q15}, [%[" SRC2 "]]!") \
    __ASM_EMIT("vabs.f32    q13, q13") \
    __ASM_EMIT("vabs.f32    q14, q14") \
    __ASM_EMIT("vabs.f32    q15, q15") \
    __ASM_EMIT(OP "         q0, q0, q8") \
    __ASM_EMIT(OP "         q1, q1, q9") \
    __ASM_EMIT(OP "         q2, q2, q10") \
    __ASM_EMIT(OP "         q3, q3, q11") \
    __ASM_EMIT("vst1.32     {q0-q1}, [%[" DST "]]!") \
    __ASM_EMIT(OP "         q4, q4, q12") \
    __ASM_EMIT(OP "         q5, q5, q13") \
    __ASM_EMIT("vst1.32     {q2-q3}, [%[" DST "]]!") \
    __ASM_EMIT(OP "         q6, q6, q14") \
    __ASM_EMIT(OP "         q7, q7, q15") \
    __ASM_EMIT("vst1.32     {q4-q5}, [%[" DST "]]!") \
    __ASM_EMIT("subs        %[count], $32") \
    __ASM_EMIT("vst1.32     {q6-q7}, [%[" DST "]]!") \
    __ASM_EMIT("bhs         1b") \
    /* 16x block */ \
    __ASM_EMIT("2:") \
    __ASM_EMIT("adds        %[count], $16") \
    __ASM_EMIT("blt         4f") \
    __ASM_EMIT("vld1.32     {q0-q1}, [%[" SRC1 "]]!") \
    __ASM_EMIT("vld1.32     {q8-q9}, [%[" SRC2 "]]!") \
    __ASM_EMIT("vld1.32     {q2-q3}, [%[" SRC1 "]]!") \
    __ASM_EMIT("vabs.f32    q8, q8") \
    __ASM_EMIT("vld1.32     {q10-q11}, [%[" SRC2 "]]!") \
    __ASM_EMIT("vabs.f32    q9, q9") \
    __ASM_EMIT("vabs.f32    q10, q10") \
    __ASM_EMIT("vabs.f32    q11, q11") \
    __ASM_EMIT(OP "         q0, q0, q8") \
    __ASM_EMIT(OP "         q1, q1, q9") \
    __ASM_EMIT(OP "         q2, q2, q10") \
    __ASM_EMIT(OP "         q3, q3, q11") \
    __ASM_EMIT("vst1.32     {q0-q1}, [%[" DST "]]!") \
    __ASM_EMIT("sub         %[count], $16") \
    __ASM_EMIT("vst1.32     {q2-q3}, [%[" DST "]]!") \
    /* 8x block */ \
    __ASM_EMIT("4:") \
    __ASM_EMIT("adds        %[count], $8") \
    __ASM_EMIT("blt         6f") \
    __ASM_EMIT("vld1.32     {q0-q1}, [%[" SRC1 "]]!") \
    __ASM_EMIT("vld1.32     {q8-q9}, [%[" SRC2 "]]!") \
    __ASM_EMIT("vabs.f32    q8, q8") \
    __ASM_EMIT("vabs.f32    q9, q9") \
    __ASM_EMIT(OP "         q0, q0, q8") \
    __ASM_EMIT(OP "         q1, q1, q9") \
    __ASM_EMIT("sub         %[count], $8") \
    __ASM_EMIT("vst1.32     {q0-q1}, [%[" DST "]]!") \
    /* 4x blocks */ \
    __ASM_EMIT("6:") \
    __ASM_EMIT("adds        %[count], $4") \
    __ASM_EMIT("blt         8f") \
    __ASM_EMIT("vld1.32     {q0}, [%[" SRC1 "]]!") \
    __ASM_EMIT("vld1.32     {q8}, [%[" SRC2 "]]!") \
    __ASM_EMIT("vabs.f32    q8, q8") \
    __ASM_EMIT(OP "         q0, q0, q8") \
    __ASM_EMIT("sub         %[count], $4") \
    __ASM_EMIT("vst1.32     {q0}, [%[" DST "]]!") \
    /* 1x blocks */ \
    __ASM_EMIT("8:") \
    __ASM_EMIT("adds        %[count], $3") \
    __ASM_EMIT("blt         10f") \
    __ASM_EMIT("9:") \
    __ASM_EMIT("vldm.32     %[" SRC1 "]!, {s0}") \
    __ASM_EMIT("vldm.32     %[" SRC2 "]!, {s2}") \
    __ASM_EMIT("vabs.f32    d1, d1") \
    __ASM_EMIT(OP "         s0, s0, s2") \
    __ASM_EMIT("vstm.32     %[" DST "]!, {s0}") \
    __ASM_EMIT("subs        %[count], $1") \
    __ASM_EMIT("bge         9b") \
    __ASM_EMIT("10:")

    void abs_add2(float *dst, const float *src, size_t count)
    {
        IF_ARCH_ARM(float *src2);

        ARCH_ARM_ASM
        (
            __ASM_EMIT("mov     %[src2], %[dst]")
            OP_ABS_CORE("vadd.f32", "dst", "src1", "src2")
            : [dst] "+r" (dst), [src1] "+r" (src), [src2] "=&r" (src2),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    void abs_sub2(float *dst, const float *src, size_t count)
    {
        IF_ARCH_ARM(float *src2);

        ARCH_ARM_ASM
        (
            __ASM_EMIT("mov     %[src1], %[dst]")
            OP_ABS_CORE("vsub.f32", "dst", "src1", "src2")
            : [dst] "+r" (dst), [src1] "=&r" (src2), [src2] "+r" (src),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    void abs_mul2(float *dst, const float *src, size_t count)
    {
        IF_ARCH_ARM(float *src2);

        ARCH_ARM_ASM
        (
            __ASM_EMIT("mov     %[src2], %[dst]")
            OP_ABS_CORE("vmul.f32", "dst", "src1", "src2")
            : [dst] "+r" (dst), [src1] "+r" (src), [src2] "=&r" (src2),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    void abs_add3(float *dst, const float *src1, const float *src2, size_t count)
    {
        IF_ARCH_ARM(size_t off);

        ARCH_ARM_ASM
        (
            OP_ABS_CORE("vadd.f32", "dst", "src1", "src2")
            : [dst] "+r" (dst), [src1] "+r" (src1), [src2] "+r" (src2),
              [off] "=&r" (off), [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    void abs_sub3(float *dst, const float *src1, const float *src2, size_t count)
    {
        ARCH_ARM_ASM
        (
            OP_ABS_CORE("vsub.f32", "dst", "src1", "src2")
            : [dst] "+r" (dst), [src1] "+r" (src1), [src2] "+r" (src2),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    void abs_mul3(float *dst, const float *src1, const float *src2, size_t count)
    {
        ARCH_ARM_ASM
        (
            OP_ABS_CORE("vmul.f32", "dst", "src1", "src2")
            : [dst] "+r" (dst), [src1] "+r" (src1), [src2] "+r" (src2),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

#undef OP_ABS_CORE


#define OP_CORE_DIV(DST, SRC1, SRC2)   \
    __ASM_EMIT("subs            %[count], $16") \
    __ASM_EMIT("blo             2f") \
    /* 16x blocks */ \
    __ASM_EMIT("1:") \
    __ASM_EMIT("vld1.32         {q0-q1}, [%[" SRC2 "]]!") \
    __ASM_EMIT("vld1.32         {q4-q5}, [%[" SRC1 "]]!") \
    __ASM_EMIT("vld1.32         {q2-q3}, [%[" SRC2 "]]!") \
    __ASM_EMIT("vld1.32         {q6-q7}, [%[" SRC1 "]]!") \
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
    __ASM_EMIT("vmul.f32        q0, q0, q4")                /* s1 / s2 */ \
    __ASM_EMIT("vmul.f32        q1, q1, q5") \
    __ASM_EMIT("vmul.f32        q2, q2, q6") \
    __ASM_EMIT("vmul.f32        q3, q3, q7") \
    __ASM_EMIT("vst1.32         {q0-q1}, [%[" DST "]]!") \
    __ASM_EMIT("subs            %[count], $16") \
    __ASM_EMIT("vst1.32         {q2-q3}, [%[" DST "]]!") \
    __ASM_EMIT("bhs             1b") \
    /* 8x block */ \
    __ASM_EMIT("2:") \
    __ASM_EMIT("adds            %[count], $8") \
    __ASM_EMIT("blt             4f") \
    __ASM_EMIT("vld1.32         {q0-q1}, [%[" SRC2 "]]!") \
    __ASM_EMIT("vld1.32         {q4-q5}, [%[" SRC1 "]]!") \
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
    __ASM_EMIT("vmul.f32        q0, q0, q4")                /* s1 / s2 */ \
    __ASM_EMIT("vmul.f32        q1, q1, q5") \
    __ASM_EMIT("sub             %[count], $8") \
    __ASM_EMIT("vst1.32         {q0-q1}, [%[" DST "]]!") \
    /* 4x blocks */ \
    __ASM_EMIT("4:") \
    __ASM_EMIT("adds            %[count], $4") \
    __ASM_EMIT("blt             6f") \
    __ASM_EMIT("vld1.32         {q0}, [%[" SRC2 "]]!") \
    __ASM_EMIT("vld1.32         {q4}, [%[" SRC1 "]]!") \
    __ASM_EMIT("vrecpe.f32      q8, q0")                    /* q8 = s2 */ \
    __ASM_EMIT("vrecps.f32      q12, q8, q0")               /* q12 = (2 - R*s2) */ \
    __ASM_EMIT("vmul.f32        q8, q12, q8")               /* q8 = s2' = s2 * (2 - R*s2) */ \
    __ASM_EMIT("vrecps.f32      q12, q8, q0")               /* q12 = (2 - R*s2') */ \
    __ASM_EMIT("vmul.f32        q0, q12, q8")               /* q0 = s2" = s2' * (2 - R*s2) = 1/s2 */  \
    __ASM_EMIT("vmul.f32        q0, q0, q4")                /* s1 / s2 */ \
    __ASM_EMIT("sub             %[count], $4") \
    __ASM_EMIT("vst1.32         {q0}, [%[" DST "]]!") \
    /* 1x blocks */ \
    __ASM_EMIT("6:") \
    __ASM_EMIT("adds            %[count], $3") \
    __ASM_EMIT("blt             8f") \
    __ASM_EMIT("7:") \
    __ASM_EMIT("vldm.32         %[" SRC1 "]!, {s0}") \
    __ASM_EMIT("vldm.32         %[" SRC2 "]!, {s1}") \
    __ASM_EMIT("vdiv.f32        s0, s0, s1") \
    __ASM_EMIT("vstm.32         %[" DST "]!, {s0}") \
    __ASM_EMIT("subs            %[count], $1") \
    __ASM_EMIT("bge             7b") \
    __ASM_EMIT("8:")

    void div2(float *dst, const float *src, size_t count)
    {
        IF_ARCH_ARM(float *src2);

        ARCH_ARM_ASM
        (
            __ASM_EMIT("mov         %[src2], %[dst]")
            OP_CORE_DIV("dst", "src2", "src1")
            : [dst] "+r" (dst), [src1] "+r" (src), [src2] "=&r" (src2),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    void div3(float *dst, const float *src1, const float *src2, size_t count)
    {
        ARCH_ARM_ASM
        (
            OP_CORE_DIV("dst", "src1", "src2")
            : [dst] "+r" (dst), [src1] "+r" (src1), [src2] "+r" (src2),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

#undef OP_CORE_DIV

#define OP_ABS_CORE_DIV(DST, SRC1, SRC2)   \
    __ASM_EMIT("subs            %[count], $16") \
    __ASM_EMIT("blo             2f") \
    /* 16x blocks */ \
    __ASM_EMIT("1:") \
    __ASM_EMIT("vld1.32         {q0-q1}, [%[" SRC2 "]]!") \
    __ASM_EMIT("vld1.32         {q4-q5}, [%[" SRC1 "]]!") \
    __ASM_EMIT("vabs.f32        q0, q0") \
    __ASM_EMIT("vld1.32         {q2-q3}, [%[" SRC2 "]]!") \
    __ASM_EMIT("vabs.f32        q1, q1") \
    __ASM_EMIT("vld1.32         {q6-q7}, [%[" SRC1 "]]!") \
    __ASM_EMIT("vabs.f32        q2, q2") \
    __ASM_EMIT("vabs.f32        q3, q3") \
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
    __ASM_EMIT("vmul.f32        q0, q0, q4")                /* s1 / s2 */ \
    __ASM_EMIT("vmul.f32        q1, q1, q5") \
    __ASM_EMIT("vmul.f32        q2, q2, q6") \
    __ASM_EMIT("vmul.f32        q3, q3, q7") \
    __ASM_EMIT("vst1.32         {q0-q1}, [%[" DST "]]!") \
    __ASM_EMIT("subs            %[count], $16") \
    __ASM_EMIT("vst1.32         {q2-q3}, [%[" DST "]]!") \
    __ASM_EMIT("bhs             1b") \
    /* 8x block */ \
    __ASM_EMIT("2:") \
    __ASM_EMIT("adds            %[count], $8") \
    __ASM_EMIT("blt             4f") \
    __ASM_EMIT("vld1.32         {q0-q1}, [%[" SRC2 "]]!") \
    __ASM_EMIT("vld1.32         {q4-q5}, [%[" SRC1 "]]!") \
    __ASM_EMIT("vabs.f32        q0, q0") \
    __ASM_EMIT("vabs.f32        q1, q1") \
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
    __ASM_EMIT("vmul.f32        q0, q0, q4")                /* s1 / s2 */ \
    __ASM_EMIT("vmul.f32        q1, q1, q5") \
    __ASM_EMIT("sub             %[count], $8") \
    __ASM_EMIT("vst1.32         {q0-q1}, [%[" DST "]]!") \
    /* 4x blocks */ \
    __ASM_EMIT("4:") \
    __ASM_EMIT("adds            %[count], $4") \
    __ASM_EMIT("blt             6f") \
    __ASM_EMIT("vld1.32         {q0}, [%[" SRC2 "]]!") \
    __ASM_EMIT("vld1.32         {q4}, [%[" SRC1 "]]!") \
    __ASM_EMIT("vabs.f32        q0, q0") \
    __ASM_EMIT("vrecpe.f32      q8, q0")                    /* q8 = s2 */ \
    __ASM_EMIT("vrecps.f32      q12, q8, q0")               /* q12 = (2 - R*s2) */ \
    __ASM_EMIT("vmul.f32        q8, q12, q8")               /* q8 = s2' = s2 * (2 - R*s2) */ \
    __ASM_EMIT("vrecps.f32      q12, q8, q0")               /* q12 = (2 - R*s2') */ \
    __ASM_EMIT("vmul.f32        q0, q12, q8")               /* q0 = s2" = s2' * (2 - R*s2) = 1/s2 */  \
    __ASM_EMIT("vmul.f32        q0, q0, q4")                /* s1 / s2 */ \
    __ASM_EMIT("sub             %[count], $4") \
    __ASM_EMIT("vst1.32         {q0}, [%[" DST "]]!") \
    /* 1x blocks */ \
    __ASM_EMIT("6:") \
    __ASM_EMIT("adds            %[count], $3") \
    __ASM_EMIT("blt             8f") \
    __ASM_EMIT("7:") \
    __ASM_EMIT("vldm.32         %[" SRC1 "]!, {s0}") \
    __ASM_EMIT("vldm.32         %[" SRC2 "]!, {s2}") \
    __ASM_EMIT("vabs.f32        d1, d1") \
    __ASM_EMIT("vdiv.f32        s0, s0, s2") \
    __ASM_EMIT("vstm.32         %[" DST "]!, {s0}") \
    __ASM_EMIT("subs            %[count], $1") \
    __ASM_EMIT("bge             7b") \
    __ASM_EMIT("8:")

    void abs_div2(float *dst, const float *src, size_t count)
    {
        IF_ARCH_ARM(float *src2);

        ARCH_ARM_ASM
        (
            __ASM_EMIT("mov         %[src2], %[dst]")
            OP_ABS_CORE_DIV("dst", "src2", "src1")
            : [dst] "+r" (dst), [src1] "+r" (src), [src2] "=&r" (src2),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    void abs_div3(float *dst, const float *src1, const float *src2, size_t count)
    {
        ARCH_ARM_ASM
        (
            OP_ABS_CORE_DIV("dst", "src1", "src2")
            : [dst] "+r" (dst), [src1] "+r" (src1), [src2] "+r" (src2),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

#undef OP_ABS_CORE_DIV

#define ABS_CORE(DST, SRC)   \
    __ASM_EMIT("subs        %[count], $64") \
    __ASM_EMIT("blo         2f") \
    /* 64x blocks */ \
    __ASM_EMIT("1:") \
    __ASM_EMIT("vld1.32     {q0-q1}, [%[" SRC "]]!") \
    __ASM_EMIT("vld1.32     {q2-q3}, [%[" SRC "]]!") \
    __ASM_EMIT("vabs.f32    q0, q0") \
    __ASM_EMIT("vld1.32     {q4-q5}, [%[" SRC "]]!") \
    __ASM_EMIT("vabs.f32    q1, q1") \
    __ASM_EMIT("vld1.32     {q6-q7}, [%[" SRC "]]!") \
    __ASM_EMIT("vabs.f32    q2, q2") \
    __ASM_EMIT("vld1.32     {q8-q9}, [%[" SRC "]]!") \
    __ASM_EMIT("vabs.f32    q3, q3") \
    __ASM_EMIT("vld1.32     {q10-q11}, [%[" SRC "]]!") \
    __ASM_EMIT("vabs.f32    q4, q4") \
    __ASM_EMIT("vld1.32     {q12-q13}, [%[" SRC "]]!") \
    __ASM_EMIT("vabs.f32    q5, q5") \
    __ASM_EMIT("vld1.32     {q14-q15}, [%[" SRC "]]!") \
    __ASM_EMIT("vabs.f32    q6, q6") \
    __ASM_EMIT("vst1.32     {q0-q1}, [%[" DST "]]!") \
    __ASM_EMIT("vabs.f32    q7, q7") \
    __ASM_EMIT("vst1.32     {q2-q3}, [%[" DST "]]!") \
    __ASM_EMIT("vabs.f32    q8, q8") \
    __ASM_EMIT("vst1.32     {q4-q5}, [%[" DST "]]!") \
    __ASM_EMIT("vabs.f32    q9, q9") \
    __ASM_EMIT("vst1.32     {q6-q7}, [%[" DST "]]!") \
    __ASM_EMIT("vabs.f32    q10, q10") \
    __ASM_EMIT("vst1.32     {q8-q9}, [%[" DST "]]!") \
    __ASM_EMIT("vabs.f32    q11, q11") \
    __ASM_EMIT("vabs.f32    q12, q12") \
    __ASM_EMIT("vst1.32     {q10-q11}, [%[" DST "]]!") \
    __ASM_EMIT("vabs.f32    q13, q13") \
    __ASM_EMIT("vabs.f32    q14, q14") \
    __ASM_EMIT("vst1.32     {q12-q13}, [%[" DST "]]!") \
    __ASM_EMIT("vabs.f32    q15, q15") \
    __ASM_EMIT("subs        %[count], $64") \
    __ASM_EMIT("vst1.32     {q14-q15}, [%[" DST "]]!") \
    __ASM_EMIT("bhs         1b") \
    /* 32x block */ \
    __ASM_EMIT("2:") \
    __ASM_EMIT("adds        %[count], $32") \
    __ASM_EMIT("blt         4f") \
    __ASM_EMIT("vld1.32     {q0-q1}, [%[" SRC "]]!") \
    __ASM_EMIT("vld1.32     {q2-q3}, [%[" SRC "]]!") \
    __ASM_EMIT("vabs.f32    q0, q0") \
    __ASM_EMIT("vld1.32     {q4-q5}, [%[" SRC "]]!") \
    __ASM_EMIT("vabs.f32    q1, q1") \
    __ASM_EMIT("vld1.32     {q6-q7}, [%[" SRC "]]!") \
    __ASM_EMIT("vabs.f32    q2, q2") \
    __ASM_EMIT("vst1.32     {q0-q1}, [%[" DST "]]!") \
    __ASM_EMIT("vabs.f32    q3, q3") \
    __ASM_EMIT("vabs.f32    q4, q4") \
    __ASM_EMIT("vst1.32     {q2-q3}, [%[" DST "]]!") \
    __ASM_EMIT("vabs.f32    q5, q5") \
    __ASM_EMIT("vabs.f32    q6, q6") \
    __ASM_EMIT("vst1.32     {q4-q5}, [%[" DST "]]!") \
    __ASM_EMIT("vabs.f32    q7, q7") \
    __ASM_EMIT("sub         %[count], $32") \
    __ASM_EMIT("vst1.32     {q6-q7}, [%[" DST "]]!") \
    /* 16x block */ \
    __ASM_EMIT("4:") \
    __ASM_EMIT("adds        %[count], $16") \
    __ASM_EMIT("blt         6f") \
    __ASM_EMIT("vld1.32     {q0-q1}, [%[" SRC "]]!") \
    __ASM_EMIT("vld1.32     {q2-q3}, [%[" SRC "]]!") \
    __ASM_EMIT("vabs.f32    q0, q0") \
    __ASM_EMIT("vabs.f32    q1, q1") \
    __ASM_EMIT("vabs.f32    q2, q2") \
    __ASM_EMIT("vst1.32     {q0-q1}, [%[" DST "]]!") \
    __ASM_EMIT("vabs.f32    q3, q3") \
    __ASM_EMIT("sub         %[count], $16") \
    __ASM_EMIT("vst1.32     {q2-q3}, [%[" DST "]]!") \
    /* 8x block */ \
    __ASM_EMIT("6:") \
    __ASM_EMIT("adds        %[count], $8") \
    __ASM_EMIT("blt         8f") \
    __ASM_EMIT("vld1.32     {q0-q1}, [%[" SRC "]]!") \
    __ASM_EMIT("vabs.f32    q0, q0") \
    __ASM_EMIT("vabs.f32    q1, q1") \
    __ASM_EMIT("sub         %[count], $8") \
    __ASM_EMIT("vst1.32     {q0-q1}, [%[" DST "]]!") \
    /* 4x block */ \
    __ASM_EMIT("8:") \
    __ASM_EMIT("adds        %[count], $4") \
    __ASM_EMIT("blt         10f") \
    __ASM_EMIT("vld1.32     {q0}, [%[" SRC "]]!") \
    __ASM_EMIT("vabs.f32    q0, q0") \
    __ASM_EMIT("sub         %[count], $4") \
    __ASM_EMIT("vst1.32     {q0}, [%[" DST "]]!") \
    /* 1x block */ \
    __ASM_EMIT("10:") \
    __ASM_EMIT("adds        %[count], $3") \
    __ASM_EMIT("blt         12f") \
    __ASM_EMIT("11:") \
    __ASM_EMIT("vldm.32     %[" SRC "]!, {s0}") \
    __ASM_EMIT("vabs.f32    d0, d0") \
    __ASM_EMIT("subs        %[count], $1") \
    __ASM_EMIT("vstm.32     %[" DST "]!, {s0}") \
    __ASM_EMIT("bge         11b") \
    __ASM_EMIT("12:")


    void abs1(float *dst, size_t count)
    {
        IF_ARCH_ARM(float *src);

        ARCH_ARM_ASM
        (
            __ASM_EMIT("mov         %[src], %[dst]")
            ABS_CORE("dst", "src")
            : [dst] "+r" (dst), [src] "=&r" (src),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    void abs2(float *dst, const float *src, size_t count)
    {
        ARCH_ARM_ASM
        (
            ABS_CORE("dst", "src")
            : [dst] "+r" (dst), [src] "+r" (src),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

#undef OP_CORE_ABS


#define SCALE_CORE(DST, SRC)   \
    __ASM_EMIT("vldm.32     %[k], {s0}") \
    __ASM_EMIT("vdup.32     q0, d0[0]") \
    __ASM_EMIT("subs        %[count], $56") \
    __ASM_EMIT("vmov        q1, q0") \
    __ASM_EMIT("blo         2f") \
    /* 56x blocks */ \
    __ASM_EMIT("1:") \
    __ASM_EMIT("vld1.32     {q2-q3}, [%[" SRC "]]!") \
    __ASM_EMIT("vld1.32     {q4-q5}, [%[" SRC "]]!") \
    __ASM_EMIT("vmul.f32    q2, q2, q0") \
    __ASM_EMIT("vld1.32     {q6-q7}, [%[" SRC "]]!") \
    __ASM_EMIT("vmul.f32    q3, q3, q1") \
    __ASM_EMIT("vld1.32     {q8-q9}, [%[" SRC "]]!") \
    __ASM_EMIT("vmul.f32    q4, q4, q0") \
    __ASM_EMIT("vld1.32     {q10-q11}, [%[" SRC "]]!") \
    __ASM_EMIT("vmul.f32    q5, q5, q1") \
    __ASM_EMIT("vld1.32     {q12-q13}, [%[" SRC "]]!") \
    __ASM_EMIT("vmul.f32    q6, q6, q0") \
    __ASM_EMIT("vld1.32     {q14-q15}, [%[" SRC "]]!") \
    __ASM_EMIT("vmul.f32    q7, q7, q1") \
    __ASM_EMIT("vmul.f32    q8, q8, q0") \
    __ASM_EMIT("vmul.f32    q9, q9, q1") \
    __ASM_EMIT("vst1.32     {q2-q3}, [%[" DST "]]!") \
    __ASM_EMIT("vmul.f32    q10, q10, q0") \
    __ASM_EMIT("vst1.32     {q4-q5}, [%[" DST "]]!") \
    __ASM_EMIT("vmul.f32    q11, q11, q1") \
    __ASM_EMIT("vst1.32     {q6-q7}, [%[" DST "]]!") \
    __ASM_EMIT("vmul.f32    q12, q12, q0") \
    __ASM_EMIT("vst1.32     {q8-q9}, [%[" DST "]]!") \
    __ASM_EMIT("vmul.f32    q13, q13, q1") \
    __ASM_EMIT("vst1.32     {q10-q11}, [%[" DST "]]!") \
    __ASM_EMIT("vmul.f32    q14, q14, q0") \
    __ASM_EMIT("vst1.32     {q12-q13}, [%[" DST "]]!") \
    __ASM_EMIT("vmul.f32    q15, q15, q1") \
    __ASM_EMIT("subs        %[count], $56") \
    __ASM_EMIT("vst1.32     {q14-q15}, [%[" DST "]]!") \
    __ASM_EMIT("bhs         1b") \
    /* 32x block */ \
    __ASM_EMIT("2:") \
    __ASM_EMIT("adds        %[count], $24") \
    __ASM_EMIT("blt         4f") \
    __ASM_EMIT("vld1.32     {q2-q3}, [%[" SRC "]]!") \
    __ASM_EMIT("vld1.32     {q4-q5}, [%[" SRC "]]!") \
    __ASM_EMIT("vmul.f32    q2, q2, q0") \
    __ASM_EMIT("vld1.32     {q6-q7}, [%[" SRC "]]!") \
    __ASM_EMIT("vmul.f32    q3, q3, q1") \
    __ASM_EMIT("vld1.32     {q8-q9}, [%[" SRC "]]!") \
    __ASM_EMIT("vmul.f32    q4, q4, q0") \
    __ASM_EMIT("vmul.f32    q5, q5, q1") \
    __ASM_EMIT("vmul.f32    q6, q6, q0") \
    __ASM_EMIT("vst1.32     {q2-q3}, [%[" DST "]]!") \
    __ASM_EMIT("vmul.f32    q7, q7, q1") \
    __ASM_EMIT("vst1.32     {q4-q5}, [%[" DST "]]!") \
    __ASM_EMIT("vmul.f32    q8, q8, q0") \
    __ASM_EMIT("vst1.32     {q6-q7}, [%[" DST "]]!") \
    __ASM_EMIT("vmul.f32    q9, q9, q1") \
    __ASM_EMIT("sub         %[count], $32") \
    __ASM_EMIT("vst1.32     {q8-q9}, [%[" DST "]]!") \
    /* 16x block */ \
    __ASM_EMIT("4:") \
    __ASM_EMIT("adds        %[count], $16") \
    __ASM_EMIT("blt         6f") \
    __ASM_EMIT("vld1.32     {q2-q3}, [%[" SRC "]]!") \
    __ASM_EMIT("vld1.32     {q4-q5}, [%[" SRC "]]!") \
    __ASM_EMIT("vmul.f32    q2, q2, q0") \
    __ASM_EMIT("vmul.f32    q3, q3, q1") \
    __ASM_EMIT("vmul.f32    q4, q4, q0") \
    __ASM_EMIT("vst1.32     {q2-q3}, [%[" DST "]]!") \
    __ASM_EMIT("vmul.f32    q5, q5, q1") \
    __ASM_EMIT("sub         %[count], $16") \
    __ASM_EMIT("vst1.32     {q4-q5}, [%[" DST "]]!") \
    /* 8x block */ \
    __ASM_EMIT("6:") \
    __ASM_EMIT("adds        %[count], $8") \
    __ASM_EMIT("blt         8f") \
    __ASM_EMIT("vld1.32     {q2-q3}, [%[" SRC "]]!") \
    __ASM_EMIT("vmul.f32    q2, q2, q0") \
    __ASM_EMIT("vmul.f32    q3, q3, q1") \
    __ASM_EMIT("sub         %[count], $8") \
    __ASM_EMIT("vst1.32     {q2-q3}, [%[" DST "]]!") \
    /* 4x block */ \
    __ASM_EMIT("8:") \
    __ASM_EMIT("adds        %[count], $4") \
    __ASM_EMIT("blt         10f") \
    __ASM_EMIT("vld1.32     {q2}, [%[" SRC "]]!") \
    __ASM_EMIT("vmul.f32    q2, q2, q0") \
    __ASM_EMIT("sub         %[count], $4") \
    __ASM_EMIT("vst1.32     {q2}, [%[" DST "]]!") \
    /* 1x block */ \
    __ASM_EMIT("10:") \
    __ASM_EMIT("adds        %[count], $3") \
    __ASM_EMIT("blt         12f") \
    __ASM_EMIT("11:") \
    __ASM_EMIT("vldm.32     %[" SRC "]!, {s1}") \
    __ASM_EMIT("vmul.f32    s1, s1, s0") \
    __ASM_EMIT("subs        %[count], $1") \
    __ASM_EMIT("vstm.32     %[" DST "]!, {s1}") \
    __ASM_EMIT("bge         11b") \
    __ASM_EMIT("12:")

    void scale2(float *dst, float k, size_t count)
    {
        IF_ARCH_ARM(float *src, *pk = &k);
        ARCH_ARM_ASM
        (
            __ASM_EMIT("mov     %[src], %[dst]")
            SCALE_CORE("dst", "src")
            : [dst] "+r" (dst), [src] "=&r" (src),
              [count] "+r" (count)
            : [k] "r" (pk)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    void scale3(float *dst, const float *src, float k, size_t count)
    {
        IF_ARCH_ARM(float *pk = &k);
        ARCH_ARM_ASM
        (
            SCALE_CORE("dst", "src")
            : [dst] "+r" (dst), [src] "+r" (src),
              [count] "+r" (count)
            : [k] "r" (pk)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

#define SCALE_ADDSUB_CORE(OP, DST, SRC1, SRC2)   \
    __ASM_EMIT("vldm.32     %[k], {s0}") \
    __ASM_EMIT("vdup.32     q0, d0[0]") \
    __ASM_EMIT("subs        %[count], $24") \
    __ASM_EMIT("vmov        q1, q0") \
    __ASM_EMIT("blo         2f") \
    /* 24x blocks */ \
    __ASM_EMIT("1:") \
    __ASM_EMIT("vld1.32     {q2-q3}, [%[" SRC1 "]]!") \
    __ASM_EMIT("vld1.32     {q8-q9}, [%[" SRC2 "]]!") \
    __ASM_EMIT("vld1.32     {q4-q5}, [%[" SRC1 "]]!") \
    __ASM_EMIT(OP "         q2, q8, q0") \
    __ASM_EMIT("vld1.32     {q10-q11}, [%[" SRC2 "]]!") \
    __ASM_EMIT(OP "         q3, q9, q1") \
    __ASM_EMIT("vld1.32     {q6-q7}, [%[" SRC1 "]]!") \
    __ASM_EMIT(OP "         q4, q10, q0") \
    __ASM_EMIT("vld1.32     {q12-q13}, [%[" SRC2 "]]!") \
    __ASM_EMIT(OP "         q5, q11, q1") \
    __ASM_EMIT("vst1.32     {q2-q3}, [%[" DST "]]!") \
    __ASM_EMIT(OP "         q6, q12, q0") \
    __ASM_EMIT("vst1.32     {q4-q5}, [%[" DST "]]!") \
    __ASM_EMIT(OP "         q7, q13, q1") \
    __ASM_EMIT("subs        %[count], $24") \
    __ASM_EMIT("vst1.32     {q6-q7}, [%[" DST "]]!") \
    __ASM_EMIT("bhs         1b") \
    /* 16x block */ \
    __ASM_EMIT("2:") \
    __ASM_EMIT("adds        %[count], $8") \
    __ASM_EMIT("blt         4f") \
    __ASM_EMIT("vld1.32     {q2-q3}, [%[" SRC1 "]]!") \
    __ASM_EMIT("vld1.32     {q8-q9}, [%[" SRC2 "]]!") \
    __ASM_EMIT("vld1.32     {q4-q5}, [%[" SRC1 "]]!") \
    __ASM_EMIT(OP "         q2, q8, q0") \
    __ASM_EMIT("vld1.32     {q10-q11}, [%[" SRC2 "]]!") \
    __ASM_EMIT(OP "         q3, q9, q1") \
    __ASM_EMIT(OP "         q4, q10, q0") \
    __ASM_EMIT("vst1.32     {q2-q3}, [%[" DST "]]!") \
    __ASM_EMIT(OP "         q5, q11, q1") \
    __ASM_EMIT("sub         %[count], $16") \
    __ASM_EMIT("vst1.32     {q4-q5}, [%[" DST "]]!") \
    /* 8x block */ \
    __ASM_EMIT("4:") \
    __ASM_EMIT("adds        %[count], $8") \
    __ASM_EMIT("blt         6f") \
    __ASM_EMIT("vld1.32     {q2-q3}, [%[" SRC1 "]]!") \
    __ASM_EMIT("vld1.32     {q8-q9}, [%[" SRC2 "]]!") \
    __ASM_EMIT(OP "         q2, q8, q0") \
    __ASM_EMIT(OP "         q3, q9, q1") \
    __ASM_EMIT("sub         %[count], $8") \
    __ASM_EMIT("vst1.32     {q2-q3}, [%[" DST "]]!") \
    /* 4x block */ \
    __ASM_EMIT("6:") \
    __ASM_EMIT("adds        %[count], $4") \
    __ASM_EMIT("blt         8f") \
    __ASM_EMIT("vld1.32     {q2}, [%[" SRC1 "]]!") \
    __ASM_EMIT("vld1.32     {q8}, [%[" SRC2 "]]!") \
    __ASM_EMIT(OP "         q2, q8, q0") \
    __ASM_EMIT("sub         %[count], $4") \
    __ASM_EMIT("vst1.32     {q2}, [%[" DST "]]!") \
    /* 1x block */ \
    __ASM_EMIT("8:") \
    __ASM_EMIT("adds        %[count], $3") \
    __ASM_EMIT("blt         10f") \
    __ASM_EMIT("9:") \
    __ASM_EMIT("vldm.32     %[" SRC1 "]!, {s1}") \
    __ASM_EMIT("vldm.32     %[" SRC2 "]!, {s2}") \
    __ASM_EMIT(OP "         s1, s2, s0") \
    __ASM_EMIT("subs        %[count], $1") \
    __ASM_EMIT("vstm.32     %[" DST "]!, {s1}") \
    __ASM_EMIT("bge         9b") \
    __ASM_EMIT("10:")

    void scale_add3(float *dst, const float *src, float k, size_t count)
    {
        IF_ARCH_ARM(float *src1, *pk = &k);
        ARCH_ARM_ASM
        (
            __ASM_EMIT("mov     %[src1], %[dst]")
            SCALE_ADDSUB_CORE("vmla.f32", "dst", "src1", "src2")
            : [dst] "+r" (dst), [src1] "=&r" (src1), [src2] "+r" (src),
              [count] "+r" (count)
            : [k] "r" (pk)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    void scale_sub3(float *dst, const float *src, float k, size_t count)
    {
        IF_ARCH_ARM(float *src1, *pk = &k);
        ARCH_ARM_ASM
        (
            __ASM_EMIT("mov     %[src1], %[dst]")
            SCALE_ADDSUB_CORE("vmls.f32", "dst", "src1", "src2")
            : [dst] "+r" (dst), [src1] "=&r" (src1), [src2] "+r" (src),
              [count] "+r" (count)
            : [k] "r" (pk)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    void scale_add4(float *dst, const float *src1, const float *src2, float k, size_t count)
    {
        IF_ARCH_ARM(float *pk = &k);
        ARCH_ARM_ASM
        (
            SCALE_ADDSUB_CORE("vmla.f32", "dst", "src1", "src2")
            : [dst] "+r" (dst), [src1] "+r" (src1), [src2] "+r" (src2),
              [count] "+r" (count)
            : [k] "r" (pk)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    void scale_sub4(float *dst, const float *src1, const float *src2, float k, size_t count)
    {
        IF_ARCH_ARM(float *pk = &k);
        ARCH_ARM_ASM
        (
            SCALE_ADDSUB_CORE("vmls.f32", "dst", "src1", "src2")
            : [dst] "+r" (dst), [src1] "+r" (src1), [src2] "+r" (src2),
              [count] "+r" (count)
            : [k] "r" (pk)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

#undef SCALE_CORE

#define SCALE_MUL_CORE(DST, SRC1, SRC2)   \
    __ASM_EMIT("vldm.32     %[k], {s0}") \
    __ASM_EMIT("vdup.32     q0, d0[0]") \
    __ASM_EMIT("subs        %[count], $24") \
    __ASM_EMIT("vmov        q1, q0") \
    __ASM_EMIT("blo         2f") \
    /* 24x blocks */ \
    __ASM_EMIT("1:") \
    __ASM_EMIT("vld1.32     {q2-q3}, [%[" SRC1 "]]!") \
    __ASM_EMIT("vld1.32     {q8-q9}, [%[" SRC2 "]]!") \
    __ASM_EMIT("vld1.32     {q4-q5}, [%[" SRC1 "]]!") \
    __ASM_EMIT("vmul.f32    q8, q8, q0") \
    __ASM_EMIT("vld1.32     {q10-q11}, [%[" SRC2 "]]!") \
    __ASM_EMIT("vmul.f32    q9, q9, q1") \
    __ASM_EMIT("vld1.32     {q6-q7}, [%[" SRC1 "]]!") \
    __ASM_EMIT("vmul.f32    q10, q10, q0") \
    __ASM_EMIT("vld1.32     {q12-q13}, [%[" SRC2 "]]!") \
    __ASM_EMIT("vmul.f32    q11, q11, q1") \
    __ASM_EMIT("vmul.f32    q12, q12, q0") \
    __ASM_EMIT("vmul.f32    q13, q13, q1") \
    __ASM_EMIT("vmul.f32    q2, q2, q8") \
    __ASM_EMIT("vmul.f32    q3, q3, q9") \
    __ASM_EMIT("vmul.f32    q4, q4, q10") \
    __ASM_EMIT("vmul.f32    q5, q5, q11") \
    __ASM_EMIT("vst1.32     {q2-q3}, [%[" DST "]]!") \
    __ASM_EMIT("vmul.f32    q6, q6, q12") \
    __ASM_EMIT("vst1.32     {q4-q5}, [%[" DST "]]!") \
    __ASM_EMIT("vmul.f32    q7, q7, q13") \
    __ASM_EMIT("subs        %[count], $24") \
    __ASM_EMIT("vst1.32     {q6-q7}, [%[" DST "]]!") \
    __ASM_EMIT("bhs         1b") \
    /* 16x block */ \
    __ASM_EMIT("2:") \
    __ASM_EMIT("adds        %[count], $8") \
    __ASM_EMIT("blt         4f") \
    __ASM_EMIT("vld1.32     {q2-q3}, [%[" SRC1 "]]!") \
    __ASM_EMIT("vld1.32     {q8-q9}, [%[" SRC2 "]]!") \
    __ASM_EMIT("vld1.32     {q4-q5}, [%[" SRC1 "]]!") \
    __ASM_EMIT("vmul.f32    q8, q8, q0") \
    __ASM_EMIT("vld1.32     {q10-q11}, [%[" SRC2 "]]!") \
    __ASM_EMIT("vmul.f32    q9, q9, q1") \
    __ASM_EMIT("vmul.f32    q10, q10, q0") \
    __ASM_EMIT("vmul.f32    q11, q11, q1") \
    __ASM_EMIT("vmul.f32    q2, q2, q8") \
    __ASM_EMIT("vmul.f32    q3, q3, q9") \
    __ASM_EMIT("vmul.f32    q4, q4, q10") \
    __ASM_EMIT("vst1.32     {q2-q3}, [%[" DST "]]!") \
    __ASM_EMIT("vmul.f32    q5, q5, q11") \
    __ASM_EMIT("sub         %[count], $16") \
    __ASM_EMIT("vst1.32     {q4-q5}, [%[" DST "]]!") \
    /* 8x block */ \
    __ASM_EMIT("4:") \
    __ASM_EMIT("adds        %[count], $8") \
    __ASM_EMIT("blt         6f") \
    __ASM_EMIT("vld1.32     {q2-q3}, [%[" SRC1 "]]!") \
    __ASM_EMIT("vld1.32     {q8-q9}, [%[" SRC2 "]]!") \
    __ASM_EMIT("vmul.f32    q8, q8, q0") \
    __ASM_EMIT("vmul.f32    q9, q9, q1") \
    __ASM_EMIT("vmul.f32    q2, q2, q8") \
    __ASM_EMIT("vmul.f32    q3, q3, q9") \
    __ASM_EMIT("sub         %[count], $8") \
    __ASM_EMIT("vst1.32     {q2-q3}, [%[" DST "]]!") \
    /* 4x block */ \
    __ASM_EMIT("6:") \
    __ASM_EMIT("adds        %[count], $4") \
    __ASM_EMIT("blt         8f") \
    __ASM_EMIT("vld1.32     {q2}, [%[" SRC1 "]]!") \
    __ASM_EMIT("vld1.32     {q8}, [%[" SRC2 "]]!") \
    __ASM_EMIT("vmul.f32    q8, q8, q0") \
    __ASM_EMIT("vmul.f32    q2, q2, q8") \
    __ASM_EMIT("sub        %[count], $4") \
    __ASM_EMIT("vst1.32     {q2}, [%[" DST "]]!") \
    /* 1x block */ \
    __ASM_EMIT("8:") \
    __ASM_EMIT("adds        %[count], $3") \
    __ASM_EMIT("blt         10f") \
    __ASM_EMIT("9:") \
    __ASM_EMIT("vldm.32     %[" SRC1 "]!, {s1}") \
    __ASM_EMIT("vldm.32     %[" SRC2 "]!, {s2}") \
    __ASM_EMIT("vmul.f32    s2, s2, s0") \
    __ASM_EMIT("vmul.f32    s1, s1, s2") \
    __ASM_EMIT("subs        %[count], $1") \
    __ASM_EMIT("vstm.32     %[" DST "]!, {s1}") \
    __ASM_EMIT("bge         9b") \
    __ASM_EMIT("10:")

    void scale_mul3(float *dst, const float *src, float k, size_t count)
    {
        IF_ARCH_ARM(float *src1, *pk = &k);
        ARCH_ARM_ASM
        (
            __ASM_EMIT("mov     %[src1], %[dst]")
            SCALE_MUL_CORE("dst", "src1", "src2")
            : [dst] "+r" (dst), [src1] "=&r" (src1), [src2] "+r" (src),
              [count] "+r" (count)
            : [k] "r" (pk)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    void scale_mul4(float *dst, const float *src1, const float *src2, float k, size_t count)
    {
        IF_ARCH_ARM(float *pk = &k);
        ARCH_ARM_ASM
        (
            SCALE_MUL_CORE("dst", "src1", "src2")
            : [dst] "+r" (dst), [src1] "+r" (src1), [src2] "+r" (src2),
              [count] "+r" (count)
            : [k] "r" (pk)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

#undef SCALE_MUL_CORE

#define SCALE_DIV_CORE(DST, SRC1, SRC2) \
    __ASM_EMIT("vldm.32         %[k], {s0}") \
    __ASM_EMIT("vdup.32         q0, d0[0]") \
    __ASM_EMIT("subs            %[count], $8") \
    __ASM_EMIT("vmov            q1, q0") \
    __ASM_EMIT("blo             2f") \
    /* 8x blocks */ \
    __ASM_EMIT("1:") \
    __ASM_EMIT("vld1.32         {q2-q3}, [%[" SRC2 "]]!") \
    __ASM_EMIT("vld1.32         {q4-q5}, [%[" SRC1 "]]!") \
    __ASM_EMIT("vmul.f32        q2, q0")                    /* q2 = s2 = x * k */ \
    __ASM_EMIT("vmul.f32        q3, q1") \
    __ASM_EMIT("vrecpe.f32      q6, q2")                    /* q6 = s2 */ \
    __ASM_EMIT("vrecpe.f32      q7, q3") \
    __ASM_EMIT("vrecps.f32      q8, q6, q2")                /* q8 = (2 - R*s2) */ \
    __ASM_EMIT("vrecps.f32      q9, q7, q3") \
    __ASM_EMIT("vmul.f32        q6, q8, q6")                /* q6 = s2' = s2 * (2 - R*s2) */ \
    __ASM_EMIT("vmul.f32        q7, q9, q7") \
    __ASM_EMIT("vrecps.f32      q8, q6, q2")                /* q8 = (2 - R*s2') */ \
    __ASM_EMIT("vrecps.f32      q9, q7, q3") \
    __ASM_EMIT("vmul.f32        q2, q8, q6")                /* q2 = s2" = s2' * (2 - R*s2) = 1/s2 */  \
    __ASM_EMIT("vmul.f32        q3, q9, q7") \
    __ASM_EMIT("vmul.f32        q2, q2, q4")                /* s1 / s2 */ \
    __ASM_EMIT("vmul.f32        q3, q3, q5") \
    __ASM_EMIT("subs            %[count], $8") \
    __ASM_EMIT("vst1.32         {q2-q3}, [%[" DST "]]!") \
    __ASM_EMIT("bhs             1b") \
    /* 4x blocks */ \
    __ASM_EMIT("2:") \
    __ASM_EMIT("adds            %[count], $4") \
    __ASM_EMIT("blt             4f") \
    __ASM_EMIT("vld1.32         {q2}, [%[" SRC2 "]]!") \
    __ASM_EMIT("vld1.32         {q4}, [%[" SRC1 "]]!") \
    __ASM_EMIT("vmul.f32        q2, q0")                    /* q2 = s2 = x * k */ \
    __ASM_EMIT("vrecpe.f32      q6, q2")                    /* q6 = s2 */ \
    __ASM_EMIT("vrecps.f32      q8, q6, q2")                /* q8 = (2 - R*s2) */ \
    __ASM_EMIT("vmul.f32        q6, q8, q6")                /* q6 = s2' = s2 * (2 - R*s2) */ \
    __ASM_EMIT("vrecps.f32      q8, q6, q2")                /* q8 = (2 - R*s2') */ \
    __ASM_EMIT("vmul.f32        q2, q8, q6")                /* q2 = s2" = s2' * (2 - R*s2) = 1/s2 */  \
    __ASM_EMIT("vmul.f32        q2, q2, q4")                /* s1 / s2 */ \
    __ASM_EMIT("sub             %[count], $4") \
    __ASM_EMIT("vst1.32         {q2}, [%[" DST "]]!") \
    /* 1x blocks */ \
    __ASM_EMIT("4:") \
    __ASM_EMIT("adds            %[count], $3") \
    __ASM_EMIT("blt             6f") \
    __ASM_EMIT("5:") \
    __ASM_EMIT("vldm.32         %[" SRC1 "]!, {s1}") \
    __ASM_EMIT("vldm.32         %[" SRC2 "]!, {s2}") \
    __ASM_EMIT("vmul.f32        s2, s2, s0") \
    __ASM_EMIT("vdiv.f32        s1, s1, s2") \
    __ASM_EMIT("vstm.32         %[" DST "]!, {s1}") \
    __ASM_EMIT("subs            %[count], $1") \
    __ASM_EMIT("bge             5b") \
    __ASM_EMIT("6:")

    void scale_div3(float *dst, const float *src, float k, size_t count)
    {
        IF_ARCH_ARM(float *src1, *pk = &k);
        ARCH_ARM_ASM
        (
            __ASM_EMIT("mov     %[src1], %[dst]")
            SCALE_DIV_CORE("dst", "src1", "src2")
            : [dst] "+r" (dst), [src1] "=&r" (src1), [src2] "+r" (src),
              [count] "+r" (count)
            : [k] "r" (pk)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    void scale_div4(float *dst, const float *src1, const float *src2, float k, size_t count)
    {
        IF_ARCH_ARM(float *pk = &k);
        ARCH_ARM_ASM
        (
            SCALE_DIV_CORE("dst", "src1", "src2")
            : [dst] "+r" (dst), [src1] "+r" (src1), [src2] "+r" (src2),
              [count] "+r" (count)
            : [k] "r" (pk)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

#undef SCALE_DIV_CORE
}

#endif /* DSP_ARCH_ARM_NEON_D32_PMATH_H_ */
