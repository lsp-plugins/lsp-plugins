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
    __ASM_EMIT("blt         6f") \
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
        IF_ARCH_ARM(
            size_t off;
            float *src2 = dst;
        )

        ARCH_ARM_ASM
        (
            OP_CORE("vadd.f32", "dst", "src1", "src2")
            : [dst] "+r" (dst), [src1] "+r" (src), [src2] "+r" (src2),
              [off] "=&r" (off), [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    void sub2(float *dst, const float *src, size_t count)
    {
        IF_ARCH_ARM(
            size_t off;
            float *src2 = dst;
        )

        ARCH_ARM_ASM
        (
            OP_CORE("vsub.f32", "dst", "src1", "src2")
            : [dst] "+r" (dst), [src1] "+r" (src), [src2] "+r" (src2),
              [off] "=&r" (off), [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    void mul2(float *dst, const float *src, size_t count)
    {
        IF_ARCH_ARM(
            size_t off;
            float *src2 = dst;
        )

        ARCH_ARM_ASM
        (
            OP_CORE("vmul.f32", "dst", "src1", "src2")
            : [dst] "+r" (dst), [src1] "+r" (src), [src2] "+r" (src2),
              [off] "=&r" (off), [count] "+r" (count)
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
            : [dst] "+r" (dst), [src1] "+r" (src), [src2] "+r" (src2),
              [off] "=&r" (off), [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    void sub3(float *dst, const float *src1, const float *src2, size_t count)
    {
        IF_ARCH_ARM(size_t off);

        ARCH_ARM_ASM
        (
            OP_CORE("vsub.f32", "dst", "src1", "src2")
            : [dst] "+r" (dst), [src1] "+r" (src), [src2] "+r" (src2),
              [off] "=&r" (off), [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    void mul3(float *dst, const float *src1, const float *src2, size_t count)
    {
        IF_ARCH_ARM(size_t off);

        ARCH_ARM_ASM
        (
            OP_CORE("vmul.f32", "dst", "src1", "src2")
            : [dst] "+r" (dst), [src1] "+r" (src), [src2] "+r" (src2),
              [off] "=&r" (off), [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }
}


#endif /* DSP_ARCH_ARM_NEON_D32_PMATH_H_ */
