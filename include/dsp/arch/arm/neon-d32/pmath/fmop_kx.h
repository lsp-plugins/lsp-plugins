/*
 * fmop_kx.h
 *
 *  Created on: 20 нояб. 2019 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_ARM_NEON_D32_PMATH_FMOP_KX_H_
#define DSP_ARCH_ARM_NEON_D32_PMATH_FMOP_KX_H_

#ifndef DSP_ARCH_ARM_NEON_32_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_ARM_NEON_32_IMPL */

namespace neon_d32
{
#define OP_DSEL(a, b)       a
#define OP_RSEL(a, b)       b

#define FMADDSUB_K3_CORE(DST, SRC, OP) \
    __ASM_EMIT("subs        %[count], $16") \
    __ASM_EMIT("vmov        q9, q8") \
    __ASM_EMIT("blo         2f") \
    /* 16x blocks */ \
    __ASM_EMIT("1:") \
    __ASM_EMIT("vldm        %[" SRC "]!, {q4-q7}") \
    __ASM_EMIT("vldm        %[" DST "], {q0-q3}") \
    __ASM_EMIT(OP ".f32     q0, q4, q8") \
    __ASM_EMIT(OP ".f32     q1, q5, q9") \
    __ASM_EMIT(OP ".f32     q2, q6, q8") \
    __ASM_EMIT(OP ".f32     q3, q7, q9") \
    __ASM_EMIT("subs        %[count], $16") \
    __ASM_EMIT("vstm        %[" DST "]!, {q0-q3}") \
    __ASM_EMIT("bhs         1b") \
    /* 8x block */ \
    __ASM_EMIT("2:") \
    __ASM_EMIT("adds        %[count], $8") /* 16 - 8 */ \
    __ASM_EMIT("blt         4f") \
    __ASM_EMIT("vldm        %[" SRC "]!, {q4-q5}") \
    __ASM_EMIT("vldm        %[" DST "], {q0-q1}") \
    __ASM_EMIT(OP ".f32     q0, q4, q8") \
    __ASM_EMIT(OP ".f32     q1, q5, q9") \
    __ASM_EMIT("sub         %[count], $8") \
    __ASM_EMIT("vstm        %[" DST "]!, {q0-q1}") \
    /* 4x block */ \
    __ASM_EMIT("4:") \
    __ASM_EMIT("adds        %[count], $4") /* 8 - 4 */ \
    __ASM_EMIT("blt         6f") \
    __ASM_EMIT("vldm        %[" SRC "]!, {q4}") \
    __ASM_EMIT("vldm        %[" DST "], {q0}") \
    __ASM_EMIT(OP ".f32     q0, q4, q8") \
    __ASM_EMIT("sub         %[count], $4") \
    __ASM_EMIT("vstm        %[" DST "]!, {q0}") \
    /* 1x block */ \
    __ASM_EMIT("6:") \
    __ASM_EMIT("adds        %[count], $3") /* 4 - 3 */ \
    __ASM_EMIT("blt         8f") \
    __ASM_EMIT("7:") \
    __ASM_EMIT("vld1.32     {d8[], d9[]}, [%[" SRC "]]!") \
    __ASM_EMIT("vld1.32     {d0[], d1[]}, [%[" DST "]]") \
    __ASM_EMIT(OP ".f32     q0, q4, q8") \
    __ASM_EMIT("subs        %[count], $1") \
    __ASM_EMIT("vst1.32     {d0[0]}, [%[" DST "]]!") \
    __ASM_EMIT("bge         7b") \
    __ASM_EMIT("8:")

    void fmadd_k3(float *dst, const float *src, float k, size_t count)
    {
        IF_ARCH_ARM(float *pk = &k);
        ARCH_ARM_ASM
        (
            __ASM_EMIT("vld1.f32    {d16[], d17[]}, [%[k]]")
            FMADDSUB_K3_CORE("dst", "src", "vfma")
            : [dst] "+r" (dst), [src] "+r" (src),
              [count] "+r" (count)
            : [k] "r" (pk)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9"
        );
    }

    void fmsub_k3(float *dst, const float *src, float k, size_t count)
    {
        IF_ARCH_ARM(float *pk = &k);
        ARCH_ARM_ASM
        (
            __ASM_EMIT("vld1.f32    {d16[], d17[]}, [%[k]]")
            FMADDSUB_K3_CORE("dst", "src", "vfms")
            : [dst] "+r" (dst), [src] "+r" (src),
              [count] "+r" (count)
            : [k] "r" (pk)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9"
        );
    }

#undef FMADDSUB_K3_CORE

#define FMOP_K3_CORE(DST, SRC, OP, SEL) \
    __ASM_EMIT("subs        %[count], $16") \
    __ASM_EMIT("vmov        q9, q8") \
    __ASM_EMIT("blo         2f") \
    /* 16x blocks */ \
    __ASM_EMIT("1:") \
    __ASM_EMIT("vldm        %[" SRC "]!, {q0-q3}") \
    __ASM_EMIT("vldm        %[" DST "], {q4-q7}") \
    __ASM_EMIT("vmul.f32    q0, q0, q8") \
    __ASM_EMIT("vmul.f32    q1, q1, q9") \
    __ASM_EMIT("vmul.f32    q2, q2, q8") \
    __ASM_EMIT("vmul.f32    q3, q3, q9") \
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
    __ASM_EMIT("vldm        %[" SRC "]!, {q0-q1}") \
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
    __ASM_EMIT("vldm        %[" SRC "]!, {q0}") \
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
    __ASM_EMIT("vld1.32     {d0[], d1[]}, [%[" SRC "]]!") \
    __ASM_EMIT("vld1.32     {d8[], d9[]}, [%[" DST "]]") \
    __ASM_EMIT("vmul.f32    q0, q0, q8") \
    __ASM_EMIT(OP ".f32     q0, " SEL("q4", "q0") ", " SEL("q0", "q4")) \
    __ASM_EMIT("subs        %[count], $1") \
    __ASM_EMIT("vst1.32     {d0[0]}, [%[" DST "]]!") \
    __ASM_EMIT("bge         7b") \
    __ASM_EMIT("8:")

    void fmmul_k3(float *dst, const float *src, float k, size_t count)
    {
        IF_ARCH_ARM(float *pk = &k);
        ARCH_ARM_ASM
        (
            __ASM_EMIT("vld1.f32    {d16[], d17[]}, [%[k]]")
            FMOP_K3_CORE("dst", "src", "vmul", OP_DSEL)
            : [dst] "+r" (dst), [src] "+r" (src),
              [count] "+r" (count)
            : [k] "r" (pk)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9"
        );
    }

    void fmrsub_k3(float *dst, const float *src, float k, size_t count)
    {
        IF_ARCH_ARM(float *pk = &k);
        ARCH_ARM_ASM
        (
            __ASM_EMIT("vld1.f32    {d16[], d17[]}, [%[k]]")
            FMOP_K3_CORE("dst", "src", "vsub", OP_RSEL)
            : [dst] "+r" (dst), [src] "+r" (src),
              [count] "+r" (count)
            : [k] "r" (pk)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9"
        );
    }

#undef FMOP_K3_CORE

#define FMDIV_K3_CORE(DST, SRC, SEL)   \
    __ASM_EMIT("subs            %[count], $16") \
    __ASM_EMIT("vmov            q5, q4") \
    __ASM_EMIT("blo             2f") \
    /* 16x blocks */ \
    __ASM_EMIT("1:") \
    __ASM_EMIT("vldm            %[" SEL(SRC, DST) "], {q0-q3}") \
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
    __ASM_EMIT("vldm            %[" SEL(DST, SRC) "], {q12-q15}") \
    __ASM_EMIT("vmul.f32        q0, q0, q4")                /* k * 1/s2 */ \
    __ASM_EMIT("vmul.f32        q1, q1, q5") \
    __ASM_EMIT("vmul.f32        q2, q2, q4") \
    __ASM_EMIT("vmul.f32        q3, q3, q5") \
    __ASM_EMIT("vmul.f32        q0, q0, q12")               /* s1 * k * 1/s2 */ \
    __ASM_EMIT("vmul.f32        q1, q1, q13") \
    __ASM_EMIT("vmul.f32        q2, q2, q14") \
    __ASM_EMIT("vmul.f32        q3, q3, q15") \
    __ASM_EMIT("vstm            %[" DST "]!, {q0-q3}") \
    __ASM_EMIT("subs            %[count], $16") \
    __ASM_EMIT("add             %[" SRC "], $0x40") \
    __ASM_EMIT("bhs             1b") \
    /* 8x block */ \
    __ASM_EMIT("2:") \
    __ASM_EMIT("adds            %[count], $8") \
    __ASM_EMIT("blt             4f") \
    __ASM_EMIT("vldm            %[" SEL(SRC, DST) "], {q0-q1}") \
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
    __ASM_EMIT("vldm            %[" SEL(DST, SRC) "], {q12-q13}") \
    __ASM_EMIT("vmul.f32        q0, q0, q4")                /* k * 1/s2 */ \
    __ASM_EMIT("vmul.f32        q1, q1, q5") \
    __ASM_EMIT("vmul.f32        q0, q0, q12")               /* s1 * k * 1/s2 */ \
    __ASM_EMIT("vmul.f32        q1, q1, q13") \
    __ASM_EMIT("sub             %[count], $8") \
    __ASM_EMIT("vstm            %[" DST "]!, {q0-q1}") \
    __ASM_EMIT("add             %[" SRC "], $0x20") \
    /* 4x blocks */ \
    __ASM_EMIT("4:") \
    __ASM_EMIT("adds            %[count], $4") \
    __ASM_EMIT("blt             6f") \
    __ASM_EMIT("vldm            %[" SEL(SRC, DST) "], {q0}") \
    __ASM_EMIT("vrecpe.f32      q8, q0")                    /* q8 = s2 */ \
    __ASM_EMIT("vrecps.f32      q12, q8, q0")               /* q12 = (2 - R*s2) */ \
    __ASM_EMIT("vmul.f32        q8, q12, q8")               /* q8 = s2' = s2 * (2 - R*s2) */ \
    __ASM_EMIT("vrecps.f32      q12, q8, q0")               /* q12 = (2 - R*s2') */ \
    __ASM_EMIT("vmul.f32        q0, q12, q8")               /* q0 = s2" = s2' * (2 - R*s2) = 1/s2 */  \
    __ASM_EMIT("vldm            %[" SEL(DST, SRC) "], {q12}") \
    __ASM_EMIT("vmul.f32        q0, q0, q4")                /* k * 1/s2 */ \
    __ASM_EMIT("vmul.f32        q0, q0, q12")               /* s1 * k * 1/s2 */ \
    __ASM_EMIT("vstm            %[" DST "]!, {q0}") \
    __ASM_EMIT("add             %[" SRC "], $0x10") \
    __ASM_EMIT("sub             %[count], $4") \
    /* 1x blocks */ \
    __ASM_EMIT("6:") \
    __ASM_EMIT("adds            %[count], $3") \
    __ASM_EMIT("blt             8f") \
    __ASM_EMIT("7:") \
    __ASM_EMIT("vld1.32         {d0[], d1[]}, [%[" SEL(SRC, DST) "]]") \
    __ASM_EMIT("vrecpe.f32      q8, q0")                    /* q8 = s2 */ \
    __ASM_EMIT("vrecps.f32      q12, q8, q0")               /* q12 = (2 - R*s2) */ \
    __ASM_EMIT("vmul.f32        q8, q12, q8")               /* q8 = s2' = s2 * (2 - R*s2) */ \
    __ASM_EMIT("vrecps.f32      q12, q8, q0")               /* q12 = (2 - R*s2') */ \
    __ASM_EMIT("vmul.f32        q0, q12, q8")               /* q0 = s2" = s2' * (2 - R*s2) = 1/s2 */  \
    __ASM_EMIT("vld1.32         {d24[], d25[]}, [%[" SEL(DST, SRC) "]]") \
    __ASM_EMIT("vmul.f32        q0, q0, q4")                /* k * 1/s2 */ \
    __ASM_EMIT("vmul.f32        q0, q0, q12")               /* s1 * k * 1/s2 */ \
    __ASM_EMIT("subs            %[count], $1") \
    __ASM_EMIT("vst1.32         {d0[0]}, [%[" DST "]]!") \
    __ASM_EMIT("add             %[" SRC "], $0x04") \
    __ASM_EMIT("bge             7b") \
    __ASM_EMIT("8:")

    void fmdiv_k3(float *dst, const float *src, float k, size_t count)
    {
        IF_ARCH_ARM(float *pk = &k);
        ARCH_ARM_ASM
        (
            __ASM_EMIT("vld1.f32        {d0[], d1[]}, [%[k]]")
            __ASM_EMIT("vrecpe.f32      q8, q0")                    /* q8 = k */
            __ASM_EMIT("vrecps.f32      q9, q8, q0")                /* q9 = (2 - R*k) */
            __ASM_EMIT("vmul.f32        q8, q9, q8")                /* q8 = k' = k * (2 - R*k) */
            __ASM_EMIT("vrecps.f32      q9, q8, q0")                /* q9 = (2 - R*k') */
            __ASM_EMIT("vmul.f32        q4, q9, q8")                /* q4 = k" = k' * (2 - R*k) = 1/k */
            FMDIV_K3_CORE("dst", "src", OP_DSEL)
            : [dst] "+r" (dst), [src] "+r" (src),
              [count] "+r" (count)
            : [k] "r" (pk)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    void fmrdiv_k3(float *dst, const float *src, float k, size_t count)
    {
        IF_ARCH_ARM(float *pk = &k);
        ARCH_ARM_ASM
        (
            __ASM_EMIT("vld1.f32    {d8[], d9[]}, [%[k]]")
            FMDIV_K3_CORE("dst", "src", OP_RSEL)
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

#undef SCALE_ADDSUB_CORE

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

#undef OP_DSEL
#undef OP_RSEL

}

#endif /* DSP_ARCH_ARM_NEON_D32_PMATH_FMOP_KX_H_ */
