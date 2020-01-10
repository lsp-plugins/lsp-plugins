/*
 * op_vv.h
 *
 *  Created on: 19 нояб. 2019 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_ARM_NEON_D32_PMATH_OP_VV_H_
#define DSP_ARCH_ARM_NEON_D32_PMATH_OP_VV_H_

#ifndef DSP_ARCH_ARM_NEON_32_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_ARM_NEON_32_IMPL */

namespace neon_d32
{
#define OP_DSEL(a, b)      a
#define OP_RSEL(a, b)      b

#define OP_VV2_CORE(DST, SRC, OP, SEL) \
    __ASM_EMIT("subs        %[count], $32") \
    __ASM_EMIT("blo         2f") \
    /* 32x blocks */ \
    __ASM_EMIT("1:") \
    __ASM_EMIT("vldm        %[" SEL(DST, SRC) "], {q0-q7}") \
    __ASM_EMIT("vldm        %[" SEL(SRC, DST) "], {q8-q15}") \
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
    __ASM_EMIT("add         %[" SRC "], $0x80") \
    __ASM_EMIT("bhs         1b") \
    /* 16x block */ \
    __ASM_EMIT("2:") \
    __ASM_EMIT("adds        %[count], $16") /* 32 - 16 */ \
    __ASM_EMIT("blt         4f") \
    __ASM_EMIT("vldm        %[" SEL(DST, SRC) "], {q0-q3}") \
    __ASM_EMIT("vldm        %[" SEL(SRC, DST) "], {q8-q11}") \
    __ASM_EMIT(OP ".f32     q0, q0, q8") \
    __ASM_EMIT(OP ".f32     q1, q1, q9") \
    __ASM_EMIT(OP ".f32     q2, q2, q10") \
    __ASM_EMIT(OP ".f32     q3, q3, q11") \
    __ASM_EMIT("sub         %[count], $16") \
    __ASM_EMIT("vstm        %[" DST "]!, {q0-q3}") \
    __ASM_EMIT("add         %[" SRC "], $0x40") \
    /* 8x block */ \
    __ASM_EMIT("4:") \
    __ASM_EMIT("adds        %[count], $8") /* 16 - 8 */ \
    __ASM_EMIT("blt         6f") \
    __ASM_EMIT("vldm        %[" SEL(DST, SRC) "], {q0-q1}") \
    __ASM_EMIT("vldm        %[" SEL(SRC, DST) "], {q8-q9}") \
    __ASM_EMIT(OP ".f32     q0, q0, q8") \
    __ASM_EMIT(OP ".f32     q1, q1, q9") \
    __ASM_EMIT("sub         %[count], $8") \
    __ASM_EMIT("vstm        %[" DST "]!, {q0-q1}") \
    __ASM_EMIT("add         %[" SRC "], $0x20") \
    /* 4x block */ \
    __ASM_EMIT("6:") \
    __ASM_EMIT("adds        %[count], $4") /* 8 - 4 */ \
    __ASM_EMIT("blt         8f") \
    __ASM_EMIT("vldm        %[" SEL(DST, SRC) "], {q0}") \
    __ASM_EMIT("vldm        %[" SEL(SRC, DST) "], {q8}") \
    __ASM_EMIT(OP ".f32     q0, q0, q8") \
    __ASM_EMIT("sub         %[count], $4") \
    __ASM_EMIT("vstm        %[" DST "]!, {q0}") \
    __ASM_EMIT("add         %[" SRC "], $0x10") \
    /* 1x block */ \
    __ASM_EMIT("8:") \
    __ASM_EMIT("adds        %[count], $3") /* 4 - 3 */ \
    __ASM_EMIT("blt         10f") \
    __ASM_EMIT("9:") \
    __ASM_EMIT("vld1.32     {d0[], d1[]}, [%[" SEL(DST, SRC) "]]") \
    __ASM_EMIT("vld1.32     {d16[], d17[]}, [%[" SEL(SRC, DST) "]]") \
    __ASM_EMIT(OP ".f32     q0, q0, q8") \
    __ASM_EMIT("subs        %[count], $1") \
    __ASM_EMIT("vst1.32     {d0[0]}, [%[" DST "]]!") \
    __ASM_EMIT("add         %[" SRC "], $0x04") \
    __ASM_EMIT("bge         9b") \
    __ASM_EMIT("10:")

    void add2(float *dst, const float *src, size_t count)
    {
        ARCH_ARM_ASM
        (
            OP_VV2_CORE("dst", "src", "vadd", OP_DSEL)
            : [dst] "+r" (dst), [src] "+r" (src),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    void sub2(float *dst, const float *src, size_t count)
    {
        ARCH_ARM_ASM
        (
            OP_VV2_CORE("dst", "src", "vsub", OP_DSEL)
            : [dst] "+r" (dst), [src] "+r" (src),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    void rsub2(float *dst, const float *src, size_t count)
    {
        ARCH_ARM_ASM
        (
            OP_VV2_CORE("dst", "src", "vsub", OP_RSEL)
            : [dst] "+r" (dst), [src] "+r" (src),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    void mul2(float *dst, const float *src, size_t count)
    {
        ARCH_ARM_ASM
        (
            OP_VV2_CORE("dst", "src", "vmul", OP_DSEL)
            : [dst] "+r" (dst), [src] "+r" (src),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

#undef OP_VV2_CORE

#define OP_DIV2_CORE(DST, SRC, SEL)   \
    __ASM_EMIT("subs            %[count], $16") \
    __ASM_EMIT("blo             2f") \
    /* 16x blocks */ \
    __ASM_EMIT("1:") \
    __ASM_EMIT("vldm            %[" SEL(SRC, DST) "], {q0-q3}") \
    __ASM_EMIT("vldm            %[" SEL(DST, SRC) "], {q4-q7}") \
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
    __ASM_EMIT("vstm            %[" DST "]!, {q0-q3}") \
    __ASM_EMIT("subs            %[count], $16") \
    __ASM_EMIT("add             %[" SRC "], $0x40") \
    __ASM_EMIT("bhs             1b") \
    /* 8x block */ \
    __ASM_EMIT("2:") \
    __ASM_EMIT("adds            %[count], $8") \
    __ASM_EMIT("blt             4f") \
    __ASM_EMIT("vldm            %[" SEL(SRC, DST) "], {q0-q1}") \
    __ASM_EMIT("vldm            %[" SEL(DST, SRC) "], {q4-q5}") \
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
    __ASM_EMIT("vstm            %[" DST "]!, {q0-q1}") \
    __ASM_EMIT("add             %[" SRC "], $0x20") \
    /* 4x blocks */ \
    __ASM_EMIT("4:") \
    __ASM_EMIT("adds            %[count], $4") \
    __ASM_EMIT("blt             6f") \
    __ASM_EMIT("vldm            %[" SEL(SRC, DST) "], {q0}") \
    __ASM_EMIT("vldm            %[" SEL(DST, SRC) "], {q4}") \
    __ASM_EMIT("vrecpe.f32      q8, q0")                    /* q8 = s2 */ \
    __ASM_EMIT("vrecps.f32      q12, q8, q0")               /* q12 = (2 - R*s2) */ \
    __ASM_EMIT("vmul.f32        q8, q12, q8")               /* q8 = s2' = s2 * (2 - R*s2) */ \
    __ASM_EMIT("vrecps.f32      q12, q8, q0")               /* q12 = (2 - R*s2') */ \
    __ASM_EMIT("vmul.f32        q0, q12, q8")               /* q0 = s2" = s2' * (2 - R*s2) = 1/s2 */  \
    __ASM_EMIT("vmul.f32        q0, q0, q4")                /* s1 / s2 */ \
    __ASM_EMIT("sub             %[count], $4") \
    __ASM_EMIT("vstm            %[" DST "]!, {q0}") \
    __ASM_EMIT("add             %[" SRC "], $0x10") \
    /* 1x blocks */ \
    __ASM_EMIT("6:") \
    __ASM_EMIT("adds            %[count], $3") \
    __ASM_EMIT("blt             8f") \
    __ASM_EMIT("7:") \
    __ASM_EMIT("vld1.32         {d0[], d1[]}, [%[" SEL(SRC, DST) "]]") \
    __ASM_EMIT("vld1.32         {d8[], d9[]}, [%[" SEL(DST, SRC) "]]") \
    __ASM_EMIT("vrecpe.f32      q8, q0")                    /* q8 = s2 */ \
    __ASM_EMIT("vrecps.f32      q12, q8, q0")               /* q12 = (2 - R*s2) */ \
    __ASM_EMIT("vmul.f32        q8, q12, q8")               /* q8 = s2' = s2 * (2 - R*s2) */ \
    __ASM_EMIT("vrecps.f32      q12, q8, q0")               /* q12 = (2 - R*s2') */ \
    __ASM_EMIT("vmul.f32        q0, q12, q8")               /* q0 = s2" = s2' * (2 - R*s2) = 1/s2 */  \
    __ASM_EMIT("vmul.f32        q0, q0, q4")                /* s1 / s2 */ \
    __ASM_EMIT("subs            %[count], $1") \
    __ASM_EMIT("vst1.32         {d0[0]}, [%[" DST "]]!") \
    __ASM_EMIT("add             %[" SRC "], $0x04") \
    __ASM_EMIT("bge             7b") \
    __ASM_EMIT("8:")

    void div2(float *dst, const float *src, size_t count)
    {
        ARCH_ARM_ASM
        (
            OP_DIV2_CORE("dst", "src", OP_DSEL)
            : [dst] "+r" (dst), [src] "+r" (src),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    void rdiv2(float *dst, const float *src, size_t count)
    {
        ARCH_ARM_ASM
        (
            OP_DIV2_CORE("dst", "src", OP_RSEL)
            : [dst] "+r" (dst), [src] "+r" (src),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

#undef OP_DIV2_CORE

#define OP_VV3_CORE(DST, SRC1, SRC2, OP) \
    __ASM_EMIT("subs        %[count], $32") \
    __ASM_EMIT("blo         2f") \
    /* 32x blocks */ \
    __ASM_EMIT("1:") \
    __ASM_EMIT("vldm        %[" SRC1 "]!, {q0-q7}") \
    __ASM_EMIT("vldm        %[" SRC2 "]!, {q8-q15}") \
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
    __ASM_EMIT("vldm        %[" SRC1 "]!, {q0-q3}") \
    __ASM_EMIT("vldm        %[" SRC2 "]!, {q8-q11}") \
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
    __ASM_EMIT("vldm        %[" SRC1 "]!, {q0-q1}") \
    __ASM_EMIT("vldm        %[" SRC2 "]!, {q8-q9}") \
    __ASM_EMIT(OP ".f32     q0, q0, q8") \
    __ASM_EMIT(OP ".f32     q1, q1, q9") \
    __ASM_EMIT("sub         %[count], $8") \
    __ASM_EMIT("vstm        %[" DST "]!, {q0-q1}") \
    /* 4x block */ \
    __ASM_EMIT("6:") \
    __ASM_EMIT("adds        %[count], $4") /* 8 - 4 */ \
    __ASM_EMIT("blt         8f") \
    __ASM_EMIT("vldm        %[" SRC1 "]!, {q0}") \
    __ASM_EMIT("vldm        %[" SRC2 "]!, {q8}") \
    __ASM_EMIT(OP ".f32     q0, q0, q8") \
    __ASM_EMIT("sub         %[count], $4") \
    __ASM_EMIT("vstm        %[" DST "]!, {q0}") \
    /* 1x block */ \
    __ASM_EMIT("8:") \
    __ASM_EMIT("adds        %[count], $3") /* 4 - 3 */ \
    __ASM_EMIT("blt         10f") \
    __ASM_EMIT("9:") \
    __ASM_EMIT("vld1.32     {d0[], d1[]}, [%[" SRC1 "]]!") \
    __ASM_EMIT("vld1.32     {d16[], d17[]}, [%[" SRC2 "]]!") \
    __ASM_EMIT(OP ".f32     q0, q0, q8") \
    __ASM_EMIT("subs        %[count], $1") \
    __ASM_EMIT("vst1.32     {d0[0]}, [%[" DST "]]!") \
    __ASM_EMIT("bge         9b") \
    __ASM_EMIT("10:")

    void add3(float *dst, const float *src1, const float *src2, size_t count)
    {
        ARCH_ARM_ASM
        (
            OP_VV3_CORE("dst", "src1", "src2", "vadd")
            : [dst] "+r" (dst), [src1] "+r" (src1), [src2] "+r" (src2),
              [count] "+r" (count)
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
            OP_VV3_CORE("dst", "src1", "src2", "vsub")
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
            OP_VV3_CORE("dst", "src1", "src2", "vmul")
            : [dst] "+r" (dst), [src1] "+r" (src1), [src2] "+r" (src2),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }
#undef OP_VV3_CORE

#define OP_DIV3_CORE(DST, SRC1, SRC2)   \
    __ASM_EMIT("subs            %[count], $16") \
    __ASM_EMIT("blo             2f") \
    /* 16x blocks */ \
    __ASM_EMIT("1:") \
    __ASM_EMIT("vldm            %[" SRC2 "]!, {q0-q3}") \
    __ASM_EMIT("vldm            %[" SRC1 "]!, {q4-q7}") \
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
    __ASM_EMIT("vstm            %[" DST "]!, {q0-q3}") \
    __ASM_EMIT("subs            %[count], $16") \
    __ASM_EMIT("bhs             1b") \
    /* 8x block */ \
    __ASM_EMIT("2:") \
    __ASM_EMIT("adds            %[count], $8") \
    __ASM_EMIT("blt             4f") \
    __ASM_EMIT("vldm            %[" SRC2 "]!, {q0-q1}") \
    __ASM_EMIT("vldm            %[" SRC1 "]!, {q4-q5}") \
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
    __ASM_EMIT("vstm            %[" DST "]!, {q0-q1}") \
    /* 4x blocks */ \
    __ASM_EMIT("4:") \
    __ASM_EMIT("adds            %[count], $4") \
    __ASM_EMIT("blt             6f") \
    __ASM_EMIT("vldm            %[" SRC2 "]!, {q0}") \
    __ASM_EMIT("vldm            %[" SRC1 "]!, {q4}") \
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
    __ASM_EMIT("vld1.32         {d0[], d1[]}, [%[" SRC2 "]]!") \
    __ASM_EMIT("vld1.32         {d8[], d9[]}, [%[" SRC1 "]]!") \
    __ASM_EMIT("vrecpe.f32      q8, q0")                    /* q8 = s2 */ \
    __ASM_EMIT("vrecps.f32      q12, q8, q0")               /* q12 = (2 - R*s2) */ \
    __ASM_EMIT("vmul.f32        q8, q12, q8")               /* q8 = s2' = s2 * (2 - R*s2) */ \
    __ASM_EMIT("vrecps.f32      q12, q8, q0")               /* q12 = (2 - R*s2') */ \
    __ASM_EMIT("vmul.f32        q0, q12, q8")               /* q0 = s2" = s2' * (2 - R*s2) = 1/s2 */  \
    __ASM_EMIT("vmul.f32        q0, q0, q4")                /* s1 / s2 */ \
    __ASM_EMIT("subs            %[count], $1") \
    __ASM_EMIT("vst1.32         {d0[0]}, [%[" DST "]]!") \
    __ASM_EMIT("bge             7b") \
    __ASM_EMIT("8:")

    void div3(float *dst, const float *src1, const float *src2, size_t count)
    {
        ARCH_ARM_ASM
        (
            OP_DIV3_CORE("dst", "src1", "src2")
            : [dst] "+r" (dst), [src1] "+r" (src1), [src2] "+r" (src2),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

#undef OP_DIV3_CORE

#define FMOD_VV_CORE(DST, SRC1, INC1, SRC2, INC2, SEL)   \
    __ASM_EMIT("subs            %[count], $16") \
    __ASM_EMIT("blo             2f") \
    /* 16x blocks */ \
    __ASM_EMIT("1:") \
    __ASM_EMIT("vldm            %[" SRC1 "]" INC1 ", {q0-q3}") \
    __ASM_EMIT("vldm            %[" SRC2 "]" INC2 ", {q4-q7}") \
    __ASM_EMIT("vrecpe.f32      q8, " SEL("q4", "q0"))              /* q8 = d */ \
    __ASM_EMIT("vrecpe.f32      q9, " SEL("q5", "q1")) \
    __ASM_EMIT("vrecpe.f32      q10, " SEL("q6", "q2")) \
    __ASM_EMIT("vrecpe.f32      q11, " SEL("q7", "q3")) \
    __ASM_EMIT("vrecps.f32      q12, q8, " SEL("q4", "q0"))         /* q12 = (2 - R*s2) */ \
    __ASM_EMIT("vrecps.f32      q13, q9, " SEL("q5", "q1")) \
    __ASM_EMIT("vrecps.f32      q14, q10, " SEL("q6", "q2")) \
    __ASM_EMIT("vrecps.f32      q15, q11, " SEL("q7", "q3")) \
    __ASM_EMIT("vmul.f32        q8, q12, q8")                       /* q8 = s2' = s2 * (2 - R*s2) */ \
    __ASM_EMIT("vmul.f32        q9, q13, q9") \
    __ASM_EMIT("vmul.f32        q10, q14, q10") \
    __ASM_EMIT("vmul.f32        q11, q15, q11") \
    __ASM_EMIT("vrecps.f32      q12, q8, " SEL("q4", "q0"))         /* q12 = (2 - R*s2') */ \
    __ASM_EMIT("vrecps.f32      q13, q9, " SEL("q5", "q1")) \
    __ASM_EMIT("vrecps.f32      q14, q10, " SEL("q6", "q2")) \
    __ASM_EMIT("vrecps.f32      q15, q11, " SEL("q7", "q3")) \
    __ASM_EMIT("vmul.f32        q8, q12, q8")                       /* q8 = s2" = s2' * (2 - R*s2) = 1/d */  \
    __ASM_EMIT("vmul.f32        q9, q13, q9") \
    __ASM_EMIT("vmul.f32        q10, q14, q10") \
    __ASM_EMIT("vmul.f32        q11, q15, q11") \
    __ASM_EMIT("vmul.f32        q8, q8, " SEL("q0", "q4"))          /* q8 = s/d */  \
    __ASM_EMIT("vmul.f32        q9, q9, " SEL("q1", "q5")) \
    __ASM_EMIT("vmul.f32        q10, q10, " SEL("q2", "q6")) \
    __ASM_EMIT("vmul.f32        q11, q11, " SEL("q3", "q7")) \
    __ASM_EMIT("vcvt.s32.f32    q8, q8") \
    __ASM_EMIT("vcvt.s32.f32    q9, q9") \
    __ASM_EMIT("vcvt.s32.f32    q10, q10") \
    __ASM_EMIT("vcvt.s32.f32    q11, q11") \
    __ASM_EMIT("vcvt.f32.s32    q8, q8") \
    __ASM_EMIT("vcvt.f32.s32    q9, q9") \
    __ASM_EMIT("vcvt.f32.s32    q10, q10") \
    __ASM_EMIT("vcvt.f32.s32    q11, q11") \
    __ASM_EMIT("vmls.f32        " SEL("q0", "q4") ", q8, " SEL("q4", "q0")) \
    __ASM_EMIT("vmls.f32        " SEL("q1", "q5") ", q9, " SEL("q5", "q1")) \
    __ASM_EMIT("vmls.f32        " SEL("q2", "q6") ", q10, " SEL("q6", "q2")) \
    __ASM_EMIT("vmls.f32        " SEL("q3", "q7") ", q11, " SEL("q7", "q3")) \
    __ASM_EMIT("vstm            %[" DST "]!, " SEL("{q0-q3}", "{q4-q7}")) \
    __ASM_EMIT("subs            %[count], $16") \
    __ASM_EMIT("bhs             1b") \
    /* 8x block */ \
    __ASM_EMIT("2:") \
    __ASM_EMIT("adds            %[count], $8") \
    __ASM_EMIT("blt             4f") \
    __ASM_EMIT("vldm            %[" SRC1 "]" INC1 ", {q0-q1}") \
    __ASM_EMIT("vldm            %[" SRC2 "]" INC2 ", {q4-q5}") \
    __ASM_EMIT("vrecpe.f32      q8, " SEL("q4", "q0"))              /* q8 = d */ \
    __ASM_EMIT("vrecpe.f32      q9, " SEL("q5", "q1")) \
    __ASM_EMIT("vrecps.f32      q12, q8, " SEL("q4", "q0"))         /* q12 = (2 - R*s2) */ \
    __ASM_EMIT("vrecps.f32      q13, q9, " SEL("q5", "q1")) \
    __ASM_EMIT("vmul.f32        q8, q12, q8")                       /* q8 = s2' = s2 * (2 - R*s2) */ \
    __ASM_EMIT("vmul.f32        q9, q13, q9") \
    __ASM_EMIT("vrecps.f32      q12, q8, " SEL("q4", "q0"))         /* q12 = (2 - R*s2') */ \
    __ASM_EMIT("vrecps.f32      q13, q9, " SEL("q5", "q1")) \
    __ASM_EMIT("vmul.f32        q8, q12, q8")                       /* q8 = s2" = s2' * (2 - R*s2) = 1/d */  \
    __ASM_EMIT("vmul.f32        q9, q13, q9") \
    __ASM_EMIT("vmul.f32        q8, q8, " SEL("q0", "q4"))          /* q8 = s/d */  \
    __ASM_EMIT("vmul.f32        q9, q9, " SEL("q1", "q5")) \
    __ASM_EMIT("vcvt.s32.f32    q8, q8") \
    __ASM_EMIT("vcvt.s32.f32    q9, q9") \
    __ASM_EMIT("vcvt.f32.s32    q8, q8") \
    __ASM_EMIT("vcvt.f32.s32    q9, q9") \
    __ASM_EMIT("vmls.f32        " SEL("q0", "q4") ", q8, " SEL("q4", "q0")) \
    __ASM_EMIT("vmls.f32        " SEL("q1", "q5") ", q9, " SEL("q5", "q1")) \
    __ASM_EMIT("vstm            %[" DST "]!, " SEL("{q0-q1}", "{q4-q5}")) \
    __ASM_EMIT("sub             %[count], $8") \
    /* 4x blocks */ \
    __ASM_EMIT("4:") \
    __ASM_EMIT("adds            %[count], $4") \
    __ASM_EMIT("blt             6f") \
    __ASM_EMIT("vldm            %[" SRC1 "]" INC1 ", {q0}") \
    __ASM_EMIT("vldm            %[" SRC2 "]" INC2 ", {q4}") \
    __ASM_EMIT("vrecpe.f32      q8, " SEL("q4", "q0"))              /* q8 = d */ \
    __ASM_EMIT("vrecps.f32      q12, q8, " SEL("q4", "q0"))         /* q12 = (2 - R*s2) */ \
    __ASM_EMIT("vmul.f32        q8, q12, q8")                       /* q8 = s2' = s2 * (2 - R*s2) */ \
    __ASM_EMIT("vrecps.f32      q12, q8, " SEL("q4", "q0"))         /* q12 = (2 - R*s2') */ \
    __ASM_EMIT("vmul.f32        q8, q12, q8")                       /* q8 = s2" = s2' * (2 - R*s2) = 1/d */  \
    __ASM_EMIT("vmul.f32        q8, q8, " SEL("q0", "q4"))          /* q8 = s/d */  \
    __ASM_EMIT("vcvt.s32.f32    q8, q8") \
    __ASM_EMIT("vcvt.f32.s32    q8, q8") \
    __ASM_EMIT("vmls.f32        " SEL("q0", "q4") ", q8, " SEL("q4", "q0")) \
    __ASM_EMIT("vstm            %[" DST "]!, " SEL("{q0}", "{q4}")) \
    __ASM_EMIT("sub             %[count], $4") \
    /* 1x blocks */ \
    __ASM_EMIT("6:") \
    __ASM_EMIT("adds            %[count], $3") \
    __ASM_EMIT("blt             8f") \
    __ASM_EMIT("7:") \
    __ASM_EMIT("vld1.32         {d0[], d1[]}, [%[" SRC1 "]]" INC1) \
    __ASM_EMIT("vld1.32         {d8[], d9[]}, [%[" SRC2 "]]" INC2) \
    __ASM_EMIT("vrecpe.f32      q8, " SEL("q4", "q0"))              /* q8 = d */ \
    __ASM_EMIT("vrecps.f32      q12, q8, " SEL("q4", "q0"))         /* q12 = (2 - R*s2) */ \
    __ASM_EMIT("vmul.f32        q8, q12, q8")                       /* q8 = s2' = s2 * (2 - R*s2) */ \
    __ASM_EMIT("vrecps.f32      q12, q8, " SEL("q4", "q0"))         /* q12 = (2 - R*s2') */ \
    __ASM_EMIT("vmul.f32        q8, q12, q8")                       /* q8 = s2" = s2' * (2 - R*s2) = 1/d */  \
    __ASM_EMIT("vmul.f32        q8, q8, " SEL("q0", "q4"))          /* q8 = s/d */  \
    __ASM_EMIT("vcvt.s32.f32    q8, q8") \
    __ASM_EMIT("vcvt.f32.s32    q8, q8") \
    __ASM_EMIT("vmls.f32        " SEL("q0", "q4") ", q8, " SEL("q4", "q0")) \
    __ASM_EMIT("vst1.32         " SEL("{d0[0]}", "{d8[0]}") ", [%[" DST "]]!") \
    __ASM_EMIT("subs            %[count], $1") \
    __ASM_EMIT("bge             7b") \
    __ASM_EMIT("8:")

    void mod2(float *dst, const float *src, size_t count)
    {
        ARCH_ARM_ASM
        (
            FMOD_VV_CORE("dst", "dst", "", "src", "!", OP_DSEL)
            : [dst] "+r" (dst), [src] "+r" (src),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    void rmod2(float *dst, const float *src, size_t count)
    {
        ARCH_ARM_ASM
        (
            FMOD_VV_CORE("dst", "dst", "", "src", "!", OP_RSEL)
            : [dst] "+r" (dst), [src] "+r" (src),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    void mod3(float *dst, const float *src1, const float *src2, size_t count)
    {
        ARCH_ARM_ASM
        (
            FMOD_VV_CORE("dst", "src1", "!", "src2", "!", OP_DSEL)
            : [dst] "+r" (dst), [src1] "+r" (src1), [src2] "+r" (src2),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

#undef FMOD_VV_CORE

#undef OP_DSEL
#undef OP_RSEL
}

#endif /* DSP_ARCH_ARM_NEON_D32_PMATH_OP_VV_H_ */
