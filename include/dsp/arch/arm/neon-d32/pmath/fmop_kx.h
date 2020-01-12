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

#define FMADDSUB_K4_CORE(DST, SRC1, SRC2, OP) \
    __ASM_EMIT("subs        %[count], $16") \
    __ASM_EMIT("vmov        q9, q8") \
    __ASM_EMIT("blo         2f") \
    /* 16x blocks */ \
    __ASM_EMIT("1:") \
    __ASM_EMIT("vldm        %[" SRC2 "]!, {q4-q7}") \
    __ASM_EMIT("vldm        %[" SRC1 "]!, {q0-q3}") \
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
    __ASM_EMIT("vldm        %[" SRC2 "]!, {q4-q5}") \
    __ASM_EMIT("vldm        %[" SRC1 "]!, {q0-q1}") \
    __ASM_EMIT(OP ".f32     q0, q4, q8") \
    __ASM_EMIT(OP ".f32     q1, q5, q9") \
    __ASM_EMIT("sub         %[count], $8") \
    __ASM_EMIT("vstm        %[" DST "]!, {q0-q1}") \
    /* 4x block */ \
    __ASM_EMIT("4:") \
    __ASM_EMIT("adds        %[count], $4") /* 8 - 4 */ \
    __ASM_EMIT("blt         6f") \
    __ASM_EMIT("vldm        %[" SRC2 "]!, {q4}") \
    __ASM_EMIT("vldm        %[" SRC1 "]!, {q0}") \
    __ASM_EMIT(OP ".f32     q0, q4, q8") \
    __ASM_EMIT("sub         %[count], $4") \
    __ASM_EMIT("vstm        %[" DST "]!, {q0}") \
    /* 1x block */ \
    __ASM_EMIT("6:") \
    __ASM_EMIT("adds        %[count], $3") /* 4 - 3 */ \
    __ASM_EMIT("blt         8f") \
    __ASM_EMIT("7:") \
    __ASM_EMIT("vld1.32     {d8[], d9[]}, [%[" SRC2 "]]!") \
    __ASM_EMIT("vld1.32     {d0[], d1[]}, [%[" SRC1 "]]!") \
    __ASM_EMIT(OP ".f32     q0, q4, q8") \
    __ASM_EMIT("subs        %[count], $1") \
    __ASM_EMIT("vst1.32     {d0[0]}, [%[" DST "]]!") \
    __ASM_EMIT("bge         7b") \
    __ASM_EMIT("8:")

    void fmadd_k4(float *dst, const float *src1, const float *src2, float k, size_t count)
    {
        IF_ARCH_ARM(float *pk = &k);
        ARCH_ARM_ASM
        (
            __ASM_EMIT("vld1.f32    {d16[], d17[]}, [%[k]]")
            FMADDSUB_K4_CORE("dst", "src1", "src2", "vfma")
            : [dst] "+r" (dst), [src1] "+r" (src1), [src2] "+r" (src2),
              [count] "+r" (count)
            : [k] "r" (pk)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9"
        );
    }

    void fmsub_k4(float *dst, const float *src1, const float *src2, float k, size_t count)
    {
        IF_ARCH_ARM(float *pk = &k);
        ARCH_ARM_ASM
        (
            __ASM_EMIT("vld1.f32    {d16[], d17[]}, [%[k]]")
            FMADDSUB_K4_CORE("dst", "src1", "src2", "vfms")
            : [dst] "+r" (dst), [src1] "+r" (src1), [src2] "+r" (src2),
              [count] "+r" (count)
            : [k] "r" (pk)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9"
        );
    }

#define FMOP_K4_CORE(DST, SRC1, SRC2, OP, SEL) \
    __ASM_EMIT("subs        %[count], $16") \
    __ASM_EMIT("vmov        q9, q8") \
    __ASM_EMIT("blo         2f") \
    /* 16x blocks */ \
    __ASM_EMIT("1:") \
    __ASM_EMIT("vldm        %[" SRC2 "]!, {q0-q3}") \
    __ASM_EMIT("vldm        %[" SRC1 "]!, {q4-q7}") \
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
    __ASM_EMIT("vldm        %[" SRC2 "]!, {q0-q1}") \
    __ASM_EMIT("vldm        %[" SRC1 "]!, {q4-q5}") \
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
    __ASM_EMIT("vldm        %[" SRC2 "]!, {q0}") \
    __ASM_EMIT("vldm        %[" SRC1 "]!, {q4}") \
    __ASM_EMIT("vmul.f32    q0, q0, q8") \
    __ASM_EMIT(OP ".f32     q0, " SEL("q4", "q0") ", " SEL("q0", "q4")) \
    __ASM_EMIT("sub         %[count], $4") \
    __ASM_EMIT("vstm        %[" DST "]!, {q0}") \
    /* 1x block */ \
    __ASM_EMIT("6:") \
    __ASM_EMIT("adds        %[count], $3") /* 4 - 3 */ \
    __ASM_EMIT("blt         8f") \
    __ASM_EMIT("7:") \
    __ASM_EMIT("vld1.32     {d0[], d1[]}, [%[" SRC2 "]]!") \
    __ASM_EMIT("vld1.32     {d8[], d9[]}, [%[" SRC1 "]]!") \
    __ASM_EMIT("vmul.f32    q0, q0, q8") \
    __ASM_EMIT(OP ".f32     q0, " SEL("q4", "q0") ", " SEL("q0", "q4")) \
    __ASM_EMIT("subs        %[count], $1") \
    __ASM_EMIT("vst1.32     {d0[0]}, [%[" DST "]]!") \
    __ASM_EMIT("bge         7b") \
    __ASM_EMIT("8:")

    void fmmul_k4(float *dst, const float *src1, const float *src2, float k, size_t count)
    {
        IF_ARCH_ARM(float *pk = &k);
        ARCH_ARM_ASM
        (
            __ASM_EMIT("vld1.f32    {d16[], d17[]}, [%[k]]")
            FMOP_K4_CORE("dst", "src1", "src2", "vmul", OP_DSEL)
            : [dst] "+r" (dst), [src1] "+r" (src1), [src2] "+r" (src2),
              [count] "+r" (count)
            : [k] "r" (pk)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9"
        );
    }

    void fmrsub_k4(float *dst, const float *src1, const float *src2, float k, size_t count)
    {
        IF_ARCH_ARM(float *pk = &k);
        ARCH_ARM_ASM
        (
            __ASM_EMIT("vld1.f32    {d16[], d17[]}, [%[k]]")
            FMOP_K4_CORE("dst", "src1", "src2", "vsub", OP_RSEL)
            : [dst] "+r" (dst), [src1] "+r" (src1), [src2] "+r" (src2),
              [count] "+r" (count)
            : [k] "r" (pk)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9"
        );
    }

#undef FMOP_K4_CORE

#define FMDIV_K4_CORE(DST, SRC1, SRC2, SEL)   \
    __ASM_EMIT("subs            %[count], $16") \
    __ASM_EMIT("vmov            q5, q4") \
    __ASM_EMIT("blo             2f") \
    /* 16x blocks */ \
    __ASM_EMIT("1:") \
    __ASM_EMIT("vldm            %[" SEL(SRC2, SRC1) "]!, {q0-q3}") \
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
    __ASM_EMIT("vldm            %[" SEL(SRC1, SRC2) "]!, {q12-q15}") \
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
    __ASM_EMIT("bhs             1b") \
    /* 8x block */ \
    __ASM_EMIT("2:") \
    __ASM_EMIT("adds            %[count], $8") \
    __ASM_EMIT("blt             4f") \
    __ASM_EMIT("vldm            %[" SEL(SRC2, SRC1) "]!, {q0-q1}") \
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
    __ASM_EMIT("vldm            %[" SEL(SRC1, SRC2) "]!, {q12-q13}") \
    __ASM_EMIT("vmul.f32        q0, q0, q4")                /* k * 1/s2 */ \
    __ASM_EMIT("vmul.f32        q1, q1, q5") \
    __ASM_EMIT("vmul.f32        q0, q0, q12")               /* s1 * k * 1/s2 */ \
    __ASM_EMIT("vmul.f32        q1, q1, q13") \
    __ASM_EMIT("sub             %[count], $8") \
    __ASM_EMIT("vstm            %[" DST "]!, {q0-q1}") \
    /* 4x blocks */ \
    __ASM_EMIT("4:") \
    __ASM_EMIT("adds            %[count], $4") \
    __ASM_EMIT("blt             6f") \
    __ASM_EMIT("vldm            %[" SEL(SRC2, SRC1) "]!, {q0}") \
    __ASM_EMIT("vrecpe.f32      q8, q0")                    /* q8 = s2 */ \
    __ASM_EMIT("vrecps.f32      q12, q8, q0")               /* q12 = (2 - R*s2) */ \
    __ASM_EMIT("vmul.f32        q8, q12, q8")               /* q8 = s2' = s2 * (2 - R*s2) */ \
    __ASM_EMIT("vrecps.f32      q12, q8, q0")               /* q12 = (2 - R*s2') */ \
    __ASM_EMIT("vmul.f32        q0, q12, q8")               /* q0 = s2" = s2' * (2 - R*s2) = 1/s2 */  \
    __ASM_EMIT("vldm            %[" SEL(SRC1, SRC2) "]!, {q12}") \
    __ASM_EMIT("vmul.f32        q0, q0, q4")                /* k * 1/s2 */ \
    __ASM_EMIT("vmul.f32        q0, q0, q12")               /* s1 * k * 1/s2 */ \
    __ASM_EMIT("vstm            %[" DST "]!, {q0}") \
    __ASM_EMIT("sub             %[count], $4") \
    /* 1x blocks */ \
    __ASM_EMIT("6:") \
    __ASM_EMIT("adds            %[count], $3") \
    __ASM_EMIT("blt             8f") \
    __ASM_EMIT("7:") \
    __ASM_EMIT("vld1.32         {d0[], d1[]}, [%[" SEL(SRC2, SRC1) "]]!") \
    __ASM_EMIT("vrecpe.f32      q8, q0")                    /* q8 = s2 */ \
    __ASM_EMIT("vrecps.f32      q12, q8, q0")               /* q12 = (2 - R*s2) */ \
    __ASM_EMIT("vmul.f32        q8, q12, q8")               /* q8 = s2' = s2 * (2 - R*s2) */ \
    __ASM_EMIT("vrecps.f32      q12, q8, q0")               /* q12 = (2 - R*s2') */ \
    __ASM_EMIT("vmul.f32        q0, q12, q8")               /* q0 = s2" = s2' * (2 - R*s2) = 1/s2 */  \
    __ASM_EMIT("vld1.32         {d24[], d25[]}, [%[" SEL(SRC1, SRC2) "]]!") \
    __ASM_EMIT("vmul.f32        q0, q0, q4")                /* k * 1/s2 */ \
    __ASM_EMIT("vmul.f32        q0, q0, q12")               /* s1 * k * 1/s2 */ \
    __ASM_EMIT("subs            %[count], $1") \
    __ASM_EMIT("vst1.32         {d0[0]}, [%[" DST "]]!") \
    __ASM_EMIT("bge             7b") \
    __ASM_EMIT("8:")

    void fmdiv_k4(float *dst, const float *src1, const float *src2, float k, size_t count)
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
            FMDIV_K4_CORE("dst", "src1", "src2", OP_DSEL)
            : [dst] "+r" (dst), [src1] "+r" (src1), [src2] "+r" (src2),
              [count] "+r" (count)
            : [k] "r" (pk)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    void fmrdiv_k4(float *dst, const float *src1, const float *src2, float k, size_t count)
    {
        IF_ARCH_ARM(float *pk = &k);
        ARCH_ARM_ASM
        (
            __ASM_EMIT("vld1.f32    {d8[], d9[]}, [%[k]]")
            FMDIV_K4_CORE("dst", "src1", "src2", OP_RSEL)
            : [dst] "+r" (dst), [src1] "+r" (src1), [src2] "+r" (src2),
              [count] "+r" (count)
            : [k] "r" (pk)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

#undef FMDIV_K4_CORE

#define FMMOD_VV_CORE(DST, SRC1, INC1, SRC2, INC2, SEL)   \
    __ASM_EMIT("subs            %[count], $16") \
    __ASM_EMIT("vmov            q13, q12") \
    __ASM_EMIT("blo             2f") \
    /* 16x blocks */ \
    __ASM_EMIT("1:") \
    __ASM_EMIT("vldm            %[" SEL(SRC2, SRC1) "]" SEL(INC2, INC1) ", {q4-q7}") \
    __ASM_EMIT(SEL("vmul.f32    q4, q4, q12", ""))                  /* q4 = d */ \
    __ASM_EMIT(SEL("vmul.f32    q5, q5, q13", "")) \
    __ASM_EMIT(SEL("vmul.f32    q6, q6, q12", "")) \
    __ASM_EMIT(SEL("vmul.f32    q7, q7, q13", "")) \
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
    __ASM_EMIT("vldm            %[" SEL(SRC1, SRC2) "]" SEL(INC1, INC2) ", {q0-q3}") \
    __ASM_EMIT(SEL("", "vmul.f32    q0, q0, q12"))                  /* q0 = s */ \
    __ASM_EMIT(SEL("", "vmul.f32    q1, q1, q13"))\
    __ASM_EMIT(SEL("", "vmul.f32    q2, q2, q12")) \
    __ASM_EMIT(SEL("", "vmul.f32    q3, q3, q13")) \
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
    __ASM_EMIT("vldm            %[" SEL(SRC2, SRC1) "]" SEL(INC2, INC1) ", {q4-q5}") \
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
    __ASM_EMIT("vldm            %[" SEL(SRC1, SRC2) "]" SEL(INC1, INC2) ", {q0-q1}") \
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
    __ASM_EMIT("vldm            %[" SEL(SRC2, SRC1) "]" SEL(INC2, INC1) ", {q4}") \
    __ASM_EMIT(SEL("vmul.f32    q4, q4, q12", ""))                  /* q4 = d */ \
    __ASM_EMIT("vrecpe.f32      q8, q4")                            /* q8 = s2 */ \
    __ASM_EMIT("vrecps.f32      q0, q8, q4")                        /* q0 = (2 - R*s2) */ \
    __ASM_EMIT("vmul.f32        q8, q0, q8")                        /* q8 = s2' = s2 * (2 - R*s2) */ \
    __ASM_EMIT("vrecps.f32      q0, q8, q4")                        /* q0 = (2 - R*s2') */ \
    __ASM_EMIT("vmul.f32        q8, q0, q8")                        /* q8 = s2" = s2' * (2 - R*s2) = 1/d */  \
    __ASM_EMIT("vldm            %[" SEL(SRC1, SRC2) "]" SEL(INC1, INC2) ", {q0}") \
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
    __ASM_EMIT("vld1.32         {d8[], d9[]}, [%[" SEL(SRC2, SRC1) "]]" SEL(INC2, INC1)) \
    __ASM_EMIT(SEL("vmul.f32    q4, q4, q12", ""))                  /* q4 = d */ \
    __ASM_EMIT("vrecpe.f32      q8, q4")                            /* q8 = s2 */ \
    __ASM_EMIT("vrecps.f32      q0, q8, q4")                        /* q0 = (2 - R*s2) */ \
    __ASM_EMIT("vmul.f32        q8, q0, q8")                        /* q8 = s2' = s2 * (2 - R*s2) */ \
    __ASM_EMIT("vrecps.f32      q0, q8, q4")                        /* q0 = (2 - R*s2') */ \
    __ASM_EMIT("vmul.f32        q8, q0, q8")                        /* q8 = s2" = s2' * (2 - R*s2) = 1/d */  \
    __ASM_EMIT("vld1.32         {d0[], d1[]}, [%[" SEL(SRC1, SRC2) "]]" SEL(INC1, INC2)) \
    __ASM_EMIT(SEL("", "vmul.f32    q0, q0, q12"))                  /* q0 = s */ \
    __ASM_EMIT("vmul.f32        q8, q8, q0")                        /* q8 = s/d */ \
    __ASM_EMIT("vcvt.s32.f32    q8, q8") \
    __ASM_EMIT("vcvt.f32.s32    q8, q8") \
    __ASM_EMIT("vmls.f32        q0, q4, q8")                        /* q0 = s - d*int(s/d) */ \
    __ASM_EMIT("vst1.32         {d0[0]}, [%[" DST "]]!") \
    __ASM_EMIT("subs            %[count], $1") \
    __ASM_EMIT("bge             7b") \
    __ASM_EMIT("8:")

    void fmmod_k3(float *dst, const float *src, float k, size_t count)
    {
        IF_ARCH_ARM(float *pk = &k);
        ARCH_ARM_ASM
        (
            __ASM_EMIT("vld1.f32    {d24[], d25[]}, [%[k]]")
            FMMOD_VV_CORE("dst", "dst", "", "src", "!", OP_DSEL)
            : [dst] "+r" (dst), [src] "+r" (src),
              [count] "+r" (count)
            : [k] "r" (pk)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    void fmrmod_k3(float *dst, const float *src, float k, size_t count)
    {
        IF_ARCH_ARM(float *pk = &k);
        ARCH_ARM_ASM
        (
            __ASM_EMIT("vld1.f32    {d24[], d25[]}, [%[k]]")
            FMMOD_VV_CORE("dst", "dst", "", "src", "!", OP_RSEL)
            : [dst] "+r" (dst), [src] "+r" (src),
              [count] "+r" (count)
            : [k] "r" (pk)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    void fmmod_k4(float *dst, const float *src1, const float *src2, float k, size_t count)
    {
        IF_ARCH_ARM(float *pk = &k);
        ARCH_ARM_ASM
        (
            __ASM_EMIT("vld1.f32    {d24[], d25[]}, [%[k]]")
            FMMOD_VV_CORE("dst", "src1", "!", "src2", "!", OP_DSEL)
            : [dst] "+r" (dst), [src1] "+r" (src1), [src2] "+r" (src2),
              [count] "+r" (count)
            : [k] "r" (pk)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    void fmrmod_k4(float *dst, const float *src1, const float *src2, float k, size_t count)
    {
        IF_ARCH_ARM(float *pk = &k);
        ARCH_ARM_ASM
        (
            __ASM_EMIT("vld1.f32    {d24[], d25[]}, [%[k]]")
            FMMOD_VV_CORE("dst", "src1", "!", "src2", "!", OP_RSEL)
            : [dst] "+r" (dst), [src1] "+r" (src1), [src2] "+r" (src2),
              [count] "+r" (count)
            : [k] "r" (pk)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

#undef FMMOD_VV_CORE

#undef OP_DSEL
#undef OP_RSEL

}

#endif /* DSP_ARCH_ARM_NEON_D32_PMATH_FMOP_KX_H_ */
