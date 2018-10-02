/*
 * float.h
 *
 *  Created on: 2 окт. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_ARM_NEON_D32_FLOAT_H_
#define DSP_ARCH_ARM_NEON_D32_FLOAT_H_

namespace neon_d32
{
#define MULTIPLE_SATURATION_BODY(dst, src) \
    __ASM_EMIT("vld1.32         {q6-q7}, [%[X_IARGS]]!")        /* q6 = +inf, q7 = -inf */ \
    __ASM_EMIT("subs            %[count], $8") \
    __ASM_EMIT("blo             2f") \
    \
    /* x8 blocks */ \
    /* +NAN, +INF */ \
    __ASM_EMIT("1:") \
    __ASM_EMIT("vld1.32         {q0-q1}, [%[src]]!")            /* q0 = s */ \
    __ASM_EMIT("vld1.32         {q4-q5}, [%[X_IARGS]]!")        /* q4 = X_P_NAN, q5 = X_P_INF */ \
    __ASM_EMIT("vcgt.s32        q2, q0, q6")                    /* q2 = [ s > +inf ] */ \
    __ASM_EMIT("vcgt.s32        q8, q1, q6") \
    __ASM_EMIT("vceq.s32        q3, q0, q6")                    /* q3 = [ s == +inf ] */ \
    __ASM_EMIT("vceq.s32        q9, q1, q6") \
    __ASM_EMIT("vclt.s32        q0, q0, q6")                    /* q0 = [ s < +inf ] */ \
    __ASM_EMIT("vclt.s32        q1, q1, q6") \
    __ASM_EMIT("vand            q2, q2, q4")                    /* q2 = X_N_NAN * [ s > +inf ] */ \
    __ASM_EMIT("vand            q3, q3, q5")                    /* q3 = X_N_INF * [ s == +inf] */ \
    __ASM_EMIT("vand            q8, q8, q4") \
    __ASM_EMIT("vand            q9, q9, q5") \
    __ASM_EMIT("vand            q0, q0, q3")                    /* q0 = s * [ s < +inf] */ \
    __ASM_EMIT("vand            q1, q1, q9") \
    __ASM_EMIT("vorr            q2, q2, q3")                    /* q2 = X_N_NAN * [ s > +inf ] | X_N_INF * [ s == +inf] */ \
    __ASM_EMIT("vorr            q8, q8, q9") \
    __ASM_EMIT("vorr            q0, q0, q2")                    /* q0 = s * [ s < +inf] | q2 = X_N_NAN * [ s > +inf ] | X_N_INF * [ s == +inf] */ \
    __ASM_EMIT("vorr            q1, q1, q8") \
    /* -NAN, -INF */ \
    __ASM_EMIT("vld1.32         {q4-q5}, [%[X_IARGS]]")         /* q4 = X_N_NAN, q5 = X_N_INF */ \
    __ASM_EMIT("vclt.s32        q2, q0, q7")                    /* q2 = [ s < -inf ] */ \
    __ASM_EMIT("vclt.s32        q8, q1, q7") \
    __ASM_EMIT("vceq.s32        q3, q0, q7")                    /* q3 = [ s == -inf ] */ \
    __ASM_EMIT("vceq.s32        q9, q1, q7") \
    __ASM_EMIT("vcgt.s32        q3, q0, q7")                    /* q3 = [ s > -inf ] */ \
    __ASM_EMIT("vcgt.s32        q9, q1, q7") \
    __ASM_EMIT("vand            q2, q2, q4")                    /* q2 = X_N_NAN * [ s < -inf ] */ \
    __ASM_EMIT("vand            q3, q3, q5")                    /* q3 = X_N_INF * [ s == -inf] */ \
    __ASM_EMIT("vand            q8, q8, q4") \
    __ASM_EMIT("vand            q9, q9, q5") \
    __ASM_EMIT("vand            q0, q0, q3")                    /* q0 = s * [ s > -inf] */ \
    __ASM_EMIT("vand            q1, q1, q9") \
    __ASM_EMIT("vorr            q2, q2, q3")                    /* q2 = X_N_NAN * [ s < -inf ] | X_N_INF * [ s == -inf] */ \
    __ASM_EMIT("vorr            q8, q8, q9") \
    __ASM_EMIT("vorr            q0, q0, q2")                    /* q0 = s * [ s > -inf] | X_N_NAN * [ s < -inf ] | X_N_INF * [ s == -inf] */ \
    __ASM_EMIT("vorr            q1, q1, q8") \
    __ASM_EMIT("subs            %[count], $8") \
    __ASM_EMIT("sub             %[X_IARGS], $0x20") \
    __ASM_EMIT("vst1.32         {q0-q1}, [%[dst]]!") \
    __ASM_EMIT("bhs             1b") \
    \
    __ASM_EMIT("2:") \
    __ASM_EMIT("adds            %[count], $4") \
    __ASM_EMIT("blt             4f") \
    /* x4 block */ \
    /* +NAN, +INF */ \
    __ASM_EMIT("vld1.32         {q0}, [%[src]]!")               /* q0 = s */ \
    __ASM_EMIT("vld1.32         {q4-q5}, [%[X_IARGS]]!")        /* q4 = X_P_NAN, q5 = X_P_INF */ \
    __ASM_EMIT("vcgt.s32        q2, q0, q6")                    /* q2 = [ s > +inf ] */ \
    __ASM_EMIT("vceq.s32        q3, q0, q6")                    /* q3 = [ s == +inf ] */ \
    __ASM_EMIT("vclt.s32        q0, q0, q6")                    /* q0 = [ s < +inf ] */ \
    __ASM_EMIT("vand            q2, q2, q4")                    /* q2 = X_N_NAN * [ s > +inf ] */ \
    __ASM_EMIT("vand            q3, q3, q5")                    /* q3 = X_N_INF * [ s == +inf] */ \
    __ASM_EMIT("vand            q0, q0, q3")                    /* q0 = s * [ s < +inf] */ \
    __ASM_EMIT("vorr            q2, q2, q3")                    /* q2 = X_N_NAN * [ s > +inf ] | X_N_INF * [ s == +inf] */ \
    __ASM_EMIT("vorr            q0, q0, q2")                    /* q0 = s * [ s < +inf] | q2 = X_N_NAN * [ s > +inf ] | X_N_INF * [ s == +inf] */ \
    /* -NAN, -INF */ \
    __ASM_EMIT("vld1.32         {q4-q5}, [%[X_IARGS]]")         /* q4 = X_N_NAN, q5 = X_N_INF */ \
    __ASM_EMIT("vclt.s32        q2, q0, q7")                    /* q2 = [ s < -inf ] */ \
    __ASM_EMIT("vceq.s32        q3, q0, q7")                    /* q3 = [ s == -inf ] */ \
    __ASM_EMIT("vcgt.s32        q3, q0, q7")                    /* q3 = [ s > -inf ] */ \
    __ASM_EMIT("vand            q2, q2, q4")                    /* q2 = X_N_NAN * [ s < -inf ] */ \
    __ASM_EMIT("vand            q3, q3, q5")                    /* q3 = X_N_INF * [ s == -inf] */ \
    __ASM_EMIT("vand            q0, q0, q3")                    /* q0 = s * [ s > -inf] */ \
    __ASM_EMIT("vorr            q2, q2, q3")                    /* q2 = X_N_NAN * [ s < -inf ] | X_N_INF * [ s == -inf] */ \
    __ASM_EMIT("vorr            q0, q0, q2")                    /* q0 = s * [ s > -inf] | X_N_NAN * [ s < -inf ] | X_N_INF * [ s == -inf] */ \
    __ASM_EMIT("sub             %[count], $4") \
    __ASM_EMIT("sub             %[X_IARGS], $0x20") \
    __ASM_EMIT("vst1.32         {q0}, [%[dst]]!") \
    \
    __ASM_EMIT("4:") \
    __ASM_EMIT("adds            %[count], $3") \
    __ASM_EMIT("blt             6f") \
    /* x1 blocks */ \
    /* +NAN, +INF */ \
    __ASM_EMIT("5:") \
    __ASM_EMIT("vldm            %[src]!, {s0}")                 /* q0 = s */ \
    __ASM_EMIT("vld1.32         {q4-q5}, [%[X_IARGS]]!")        /* q4 = X_P_NAN, q5 = X_P_INF */ \
    __ASM_EMIT("vcgt.s32        q2, q0, q6")                    /* q2 = [ s > +inf ] */ \
    __ASM_EMIT("vceq.s32        q3, q0, q6")                    /* q3 = [ s == +inf ] */ \
    __ASM_EMIT("vclt.s32        q0, q0, q6")                    /* q0 = [ s < +inf ] */ \
    __ASM_EMIT("vand            q2, q2, q4")                    /* q2 = X_N_NAN * [ s > +inf ] */ \
    __ASM_EMIT("vand            q3, q3, q5")                    /* q3 = X_N_INF * [ s == +inf] */ \
    __ASM_EMIT("vand            q0, q0, q3")                    /* q0 = s * [ s < +inf] */ \
    __ASM_EMIT("vorr            q2, q2, q3")                    /* q2 = X_N_NAN * [ s > +inf ] | X_N_INF * [ s == +inf] */ \
    __ASM_EMIT("vorr            q0, q0, q2")                    /* q0 = s * [ s < +inf] | q2 = X_N_NAN * [ s > +inf ] | X_N_INF * [ s == +inf] */ \
    /* -NAN, -INF */ \
    __ASM_EMIT("vld1.32         {q4-q5}, [%[X_IARGS]]")         /* q4 = X_N_NAN, q5 = X_N_INF */ \
    __ASM_EMIT("vclt.s32        q2, q0, q7")                    /* q2 = [ s < -inf ] */ \
    __ASM_EMIT("vceq.s32        q3, q0, q7")                    /* q3 = [ s == -inf ] */ \
    __ASM_EMIT("vcgt.s32        q3, q0, q7")                    /* q3 = [ s > -inf ] */ \
    __ASM_EMIT("vand            q2, q2, q4")                    /* q2 = X_N_NAN * [ s < -inf ] */ \
    __ASM_EMIT("vand            q3, q3, q5")                    /* q3 = X_N_INF * [ s == -inf] */ \
    __ASM_EMIT("vand            q0, q0, q3")                    /* q0 = s * [ s > -inf] */ \
    __ASM_EMIT("vorr            q2, q2, q3")                    /* q2 = X_N_NAN * [ s < -inf ] | X_N_INF * [ s == -inf] */ \
    __ASM_EMIT("vorr            q0, q0, q2")                    /* q0 = s * [ s > -inf] | X_N_NAN * [ s < -inf ] | X_N_INF * [ s == -inf] */ \
    __ASM_EMIT("subs            %[count], $1") \
    __ASM_EMIT("sub             %[X_IARGS], $0x20") \
    __ASM_EMIT("vstm            %[dst]!, {s0}") \
    __ASM_EMIT("bge             5b") \
    \
    __ASM_EMIT("6:")


    void copy_saturated(float *dst, const float *src, size_t count)
    {
        IF_ARCH_ARM(const float *X_IARGS = &SAT_IARGS[4]);

        ARCH_ARM_ASM
        (
            MULTIPLE_SATURATION_BODY("dst", "src")
            : [dst] "+r" (dst), [src] "+r" (src),
              [count] "+r" (count),
              [X_IARGS] "+r" (X_IARGS)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9"
        );
    }

    void saturate(float *dst, size_t count)
    {
        IF_ARCH_ARM(
            const float *X_IARGS = &SAT_IARGS[4];
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
              "q8", "q9"
        );
    }

#undef MULTIPLE_SATURATION_BODY
}

#endif /* DSP_ARCH_ARM_NEON_D32_FLOAT_H_ */
