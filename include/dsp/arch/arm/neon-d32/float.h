/*
 * float.h
 *
 *  Created on: 2 окт. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_ARM_NEON_D32_FLOAT_H_
#define DSP_ARCH_ARM_NEON_D32_FLOAT_H_

#ifndef DSP_ARCH_ARM_NEON_32_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_ARM_NEON_32_IMPL */

namespace neon_d32
{
#define MULTIPLE_SATURATION_BODY(dst, src) \
    __ASM_EMIT("vld1.32         {q8-q9}, [%[X_IARGS]]!")        /* q8 = +inf, q9 = -inf */ \
    __ASM_EMIT("vld1.32         {q10-q11}, [%[X_IARGS]]!")      /* q10 = X_P_NAN, q11 = X_P_INF */ \
    __ASM_EMIT("vld1.32         {q12-q13}, [%[X_IARGS]]")       /* q12 = X_N_NAN, q13 = X_N_INF */ \
    __ASM_EMIT("subs            %[count], $8") \
    __ASM_EMIT("blo             2f") \
    \
    /* x8 blocks */ \
    /* +NAN, +INF */ \
    __ASM_EMIT("1:") \
    __ASM_EMIT("vld1.32         {q0-q1}, [%[src]]!")            /* q0 = s */ \
    __ASM_EMIT("vcgt.s32        q2, q0, q8")                    /* q2 = [ s > +inf ] */ \
    __ASM_EMIT("vcgt.s32        q3, q1, q8") \
    __ASM_EMIT("vceq.s32        q4, q0, q8")                    /* q4 = [ s == +inf ] */ \
    __ASM_EMIT("vceq.s32        q5, q1, q8") \
    __ASM_EMIT("vclt.s32        q6, q0, q8")                    /* q6 = [ s < +inf ] */ \
    __ASM_EMIT("vclt.s32        q7, q1, q8") \
    __ASM_EMIT("vand            q2, q2, q10")                   /* q2 = X_P_NAN * [ s > +inf ] */ \
    __ASM_EMIT("vand            q3, q3, q10") \
    __ASM_EMIT("vand            q4, q4, q11")                   /* q4 = X_P_INF * [ s == +inf] */ \
    __ASM_EMIT("vand            q5, q5, q11") \
    __ASM_EMIT("vand            q6, q6, q0")                    /* q6 = s * [ s < +inf ] */ \
    __ASM_EMIT("vand            q7, q7, q1") \
    __ASM_EMIT("vorr            q2, q2, q4")                    /* q2 = X_P_NAN * [ s > +inf ] | X_P_INF * [ s == +inf] */ \
    __ASM_EMIT("vorr            q3, q3, q5") \
    __ASM_EMIT("vorr            q0, q2, q6")                    /* q0 = s * [ s < +inf] | q2 = X_P_NAN * [ s > +inf ] | X_P_INF * [ s == +inf] */ \
    __ASM_EMIT("vorr            q1, q3, q7") \
    /* -NAN, -INF */ \
    __ASM_EMIT("vcgt.u32        q2, q0, q9")                    /* q2 = [ s > -inf ] */ \
    __ASM_EMIT("vcgt.u32        q3, q1, q9") \
    __ASM_EMIT("vceq.u32        q4, q0, q9")                    /* q4 = [ s == -inf ] */ \
    __ASM_EMIT("vceq.u32        q5, q1, q9") \
    __ASM_EMIT("vclt.u32        q6, q0, q9")                    /* q6 = [ s < -inf ] */ \
    __ASM_EMIT("vclt.u32        q7, q1, q9") \
    __ASM_EMIT("vand            q2, q2, q12")                   /* q2 = X_N_NAN * [ s > -inf ] */ \
    __ASM_EMIT("vand            q3, q3, q12") \
    __ASM_EMIT("vand            q4, q4, q13")                   /* q4 = X_N_INF * [ s == -inf] */ \
    __ASM_EMIT("vand            q5, q5, q13") \
    __ASM_EMIT("vand            q6, q6, q0")                    /* q6 = s * [ s < -inf ] */ \
    __ASM_EMIT("vand            q7, q7, q1") \
    __ASM_EMIT("vorr            q2, q2, q4")                    /* q2 = X_N_NAN * [ s > -inf ] | X_N_INF * [ s == -inf] */ \
    __ASM_EMIT("vorr            q3, q3, q5") \
    __ASM_EMIT("vorr            q0, q2, q6")                    /* q0 = s * [ s < +inf] | q2 = X_N_NAN * [ s > -inf ] | X_N_INF * [ s == -inf] */ \
    __ASM_EMIT("vorr            q1, q3, q7") \
    __ASM_EMIT("subs            %[count], $8") \
    __ASM_EMIT("vst1.32         {q0-q1}, [%[dst]]!") \
    __ASM_EMIT("bhs             1b") \
    \
    __ASM_EMIT("2:") \
    __ASM_EMIT("adds            %[count], $4") \
    __ASM_EMIT("blt             4f") \
    /* x4 block */ \
    /* +NAN, +INF */ \
    __ASM_EMIT("vld1.32         {q0}, [%[src]]!")               /* q0 = s */ \
    __ASM_EMIT("vcgt.s32        q2, q0, q8")                    /* q2 = [ s > +inf ] */ \
    __ASM_EMIT("vceq.s32        q4, q0, q8")                    /* q4 = [ s == +inf ] */ \
    __ASM_EMIT("vclt.s32        q6, q0, q8")                    /* q6 = [ s < +inf ] */ \
    __ASM_EMIT("vand            q2, q2, q10")                   /* q2 = X_P_NAN * [ s > +inf ] */ \
    __ASM_EMIT("vand            q4, q4, q11")                   /* q4 = X_P_INF * [ s == +inf] */ \
    __ASM_EMIT("vand            q6, q6, q0")                    /* q6 = s * [ s < +inf ] */ \
    __ASM_EMIT("vorr            q2, q2, q4")                    /* q2 = X_P_NAN * [ s > +inf ] | X_P_INF * [ s == +inf] */ \
    __ASM_EMIT("vorr            q0, q2, q6")                    /* q0 = s * [ s < +inf] | q2 = X_P_NAN * [ s > +inf ] | X_P_INF * [ s == +inf] */ \
    /* -NAN, -INF */ \
    __ASM_EMIT("vcgt.u32        q2, q0, q9")                    /* q2 = [ s > -inf ] */ \
    __ASM_EMIT("vceq.u32        q4, q0, q9")                    /* q4 = [ s == -inf ] */ \
    __ASM_EMIT("vclt.u32        q6, q0, q9")                    /* q6 = [ s < -inf ] */ \
    __ASM_EMIT("vand            q2, q2, q12")                   /* q2 = X_N_NAN * [ s > -inf ] */ \
    __ASM_EMIT("vand            q4, q4, q13")                   /* q4 = X_N_INF * [ s == -inf] */ \
    __ASM_EMIT("vand            q6, q6, q0")                    /* q6 = s * [ s < -inf ] */ \
    __ASM_EMIT("vorr            q2, q2, q4")                    /* q2 = X_N_NAN * [ s > -inf ] | X_N_INF * [ s == -inf] */ \
    __ASM_EMIT("vorr            q0, q2, q6")                    /* q0 = s * [ s < +inf] | q2 = X_N_NAN * [ s > -inf ] | X_N_INF * [ s == -inf] */ \
    __ASM_EMIT("sub             %[count], $4") \
    __ASM_EMIT("vst1.32         {q0}, [%[dst]]!") \
    \
    __ASM_EMIT("4:") \
    __ASM_EMIT("adds            %[count], $3") \
    __ASM_EMIT("blt             6f") \
    /* x1 blocks */ \
    /* +NAN, +INF */ \
    __ASM_EMIT("5:") \
    __ASM_EMIT("vldm            %[src]!, {s0}")                 /* q0 = s */ \
    __ASM_EMIT("vcgt.s32        q2, q0, q8")                    /* q2 = [ s > +inf ] */ \
    __ASM_EMIT("vceq.s32        q4, q0, q8")                    /* q4 = [ s == +inf ] */ \
    __ASM_EMIT("vclt.s32        q6, q0, q8")                    /* q6 = [ s < +inf ] */ \
    __ASM_EMIT("vand            q2, q2, q10")                   /* q2 = X_P_NAN * [ s > +inf ] */ \
    __ASM_EMIT("vand            q4, q4, q11")                   /* q4 = X_P_INF * [ s == +inf] */ \
    __ASM_EMIT("vand            q6, q6, q0")                    /* q6 = s * [ s < +inf ] */ \
    __ASM_EMIT("vorr            q2, q2, q4")                    /* q2 = X_P_NAN * [ s > +inf ] | X_P_INF * [ s == +inf] */ \
    __ASM_EMIT("vorr            q0, q2, q6")                    /* q0 = s * [ s < +inf] | q2 = X_P_NAN * [ s > +inf ] | X_P_INF * [ s == +inf] */ \
    /* -NAN, -INF */ \
    __ASM_EMIT("vcgt.u32        q2, q0, q9")                    /* q2 = [ s > -inf ] */ \
    __ASM_EMIT("vceq.u32        q4, q0, q9")                    /* q4 = [ s == -inf ] */ \
    __ASM_EMIT("vclt.u32        q6, q0, q9")                    /* q6 = [ s < -inf ] */ \
    __ASM_EMIT("vand            q2, q2, q12")                   /* q2 = X_N_NAN * [ s > -inf ] */ \
    __ASM_EMIT("vand            q4, q4, q13")                   /* q4 = X_N_INF * [ s == -inf] */ \
    __ASM_EMIT("vand            q6, q6, q0")                    /* q6 = s * [ s < -inf ] */ \
    __ASM_EMIT("vorr            q2, q2, q4")                    /* q2 = X_N_NAN * [ s > -inf ] | X_N_INF * [ s == -inf] */ \
    __ASM_EMIT("vorr            q0, q2, q6")                    /* q0 = s * [ s < +inf] | q2 = X_N_NAN * [ s > -inf ] | X_N_INF * [ s == -inf] */ \
    __ASM_EMIT("subs            %[count], $1") \
    __ASM_EMIT("vstm            %[dst]!, {s0}") \
    __ASM_EMIT("bge             5b") \
    \
    __ASM_EMIT("6:")

    void copy_saturated(float *dst, const float *src, size_t count)
    {
        IF_ARCH_ARM(const uint32_t *X_IARGS = &SAT_IARGS[4]);

        ARCH_ARM_ASM
        (
            MULTIPLE_SATURATION_BODY("dst", "src")
            : [dst] "+r" (dst), [src] "+r" (src),
              [count] "+r" (count),
              [X_IARGS] "+r" (X_IARGS)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13"
        );
    }

    void saturate(float *dst, size_t count)
    {
        IF_ARCH_ARM(
            const uint32_t *X_IARGS = &SAT_IARGS[4];
            const float *src = dst;
        )

        ARCH_ARM_ASM
        (
            MULTIPLE_SATURATION_BODY("dst", "src")
            : [dst] "+r" (dst), [src] "+r" (src),
              [count] "+r" (count),
              [X_IARGS] "+r" (X_IARGS)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13"
        );
    }

#undef MULTIPLE_SATURATION_BODY
}

#endif /* DSP_ARCH_ARM_NEON_D32_FLOAT_H_ */
