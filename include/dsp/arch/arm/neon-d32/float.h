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

#define U4VEC(x)        x, x, x, x
IF_ARCH_ARM(
    static uint32_t XLIM_SAT[] __lsp_aligned16 =
    {
        U4VEC(0xbf800000),      // -1
        U4VEC(0x3f800000),      // +1
        U4VEC(0x80000000),      // sign
        U4VEC(0x7f800000)       // +Inf
    };
)

#undef U4VEC


#define LIMIT_SAT_BODY_X4 \
    /* q12 = -1, q13 = +1, q14 = sign, q15 = +Inf */ \
    /* q0 = s */ \
    __ASM_EMIT("vcge.f32        q2, q0, q12")                   /* q2 = [ s >= -1 ] */ \
    __ASM_EMIT("vabs.f32        q6, q0")                        /* q6 = abs(s) */ \
    __ASM_EMIT("vcle.f32        q4, q0, q13")                   /* q4 = [ s <= +1 ] */ \
    __ASM_EMIT("vcle.s32        q6, q6, q15")                   /* q6 = abs(s) <= +Inf */ \
    __ASM_EMIT("vand.i32        q8, q0, q14")                   /* q8 = sign(s) */ \
    __ASM_EMIT("vand            q6, q6, q13")                   /* q6 = 1 & (abs(s) <= +Inf) */ \
    __ASM_EMIT("vand            q2, q2, q4")                    /* q2 = [ s >= -1 ] & [ s <= +1 ] */ \
    __ASM_EMIT("vorr            q6, q6, q8")                    /* q6 = (1 & (abs(s) <= +Inf)) | sign(s) */ \
    __ASM_EMIT("vbif            q0, q6, q2")                    /* q0 = (s & ([ s >= -1 ] & [ s <= +1 ])) | (((1 & (abs(s) <= +Inf)) | sign(s)) & ([ s < -1 ] | [ s > +1 ]))*/

#define LIMIT_SAT_BODY_X8 \
    __ASM_EMIT("vcge.f32        q2, q0, q12")                   /* q2 = [ s >= -1 ] */ \
    __ASM_EMIT("vcge.f32        q3, q1, q12") \
    __ASM_EMIT("vabs.f32        q6, q0")                        /* q6 = abs(s) */ \
    __ASM_EMIT("vabs.f32        q7, q1") \
    __ASM_EMIT("vcle.f32        q4, q0, q13")                   /* q4 = [ s <= +1 ] */ \
    __ASM_EMIT("vcle.f32        q5, q1, q13") \
    __ASM_EMIT("vcle.s32        q6, q6, q15")                   /* q6 = abs(s) <= +Inf */ \
    __ASM_EMIT("vcle.s32        q7, q7, q15") \
    __ASM_EMIT("vand.i32        q8, q0, q14")                   /* q8 = sign(s) */ \
    __ASM_EMIT("vand.i32        q9, q1, q14") \
    __ASM_EMIT("vand            q6, q6, q13")                   /* q6 = 1 & (abs(s) <= +Inf) */ \
    __ASM_EMIT("vand            q7, q7, q13") \
    __ASM_EMIT("vand            q2, q2, q4")                    /* q2 = [ s >= -1 ] & [ s <= +1 ] */ \
    __ASM_EMIT("vand            q3, q3, q5") \
    __ASM_EMIT("vorr            q6, q6, q8")                    /* q6 = (1 & (abs(s) <= +Inf)) | sign(s) */ \
    __ASM_EMIT("vorr            q7, q7, q9") \
    __ASM_EMIT("vbif            q0, q6, q2")                    /* q0 = (s & ([ s >= -1 ] & [ s <= +1 ])) | (((1 & (abs(s) <= +Inf)) | sign(s)) & ([ s < -1 ] | [ s > +1 ]))*/ \
    __ASM_EMIT("vbif            q1, q7, q3")

#define LIMIT_SAT_BODY \
    __ASM_EMIT("vldm            %[XC], {q12-q15}")              /* q12 = -1, q13 = +1, q14 = sign, q15 = +Inf */ \
    __ASM_EMIT("subs            %[count], $8") \
    __ASM_EMIT("blo             2f") \
    \
    /* 8x blocks */ \
    __ASM_EMIT("1:") \
    __ASM_EMIT("vld1.32         {q0-q1}, [%[src]]!")            /* q0 = s0, q1 = s1 */ \
    LIMIT_SAT_BODY_X8 \
    __ASM_EMIT("subs            %[count], $8") \
    __ASM_EMIT("vst1.32         {q0-q1}, [%[dst]]!") \
    __ASM_EMIT("bhs             1b") \
    \
    __ASM_EMIT("2:") \
    __ASM_EMIT("adds            %[count], $4") \
    __ASM_EMIT("blt             4f") \
    /* 4x block */ \
    __ASM_EMIT("vld1.32         {q0}, [%[src]]!")               /* q0 = s */ \
    LIMIT_SAT_BODY_X4 \
    __ASM_EMIT("sub             %[count], $4") \
    __ASM_EMIT("vst1.32         {q0}, [%[dst]]!") \
    __ASM_EMIT("4:") \
    __ASM_EMIT("adds            %[count], $4") \
    __ASM_EMIT("bls             12f") \
    \
    /* 1x-3x block */ \
    __ASM_EMIT("tst             %[count], $2") \
    __ASM_EMIT("beq             6f") \
    __ASM_EMIT("vld1.32         {d1}, [%[src]]!") \
    __ASM_EMIT("6:") \
    __ASM_EMIT("tst             %[count], $1") \
    __ASM_EMIT("beq             8f") \
    __ASM_EMIT("vldm            %[src], {s0}") \
    __ASM_EMIT("8:") \
    LIMIT_SAT_BODY_X4 \
    __ASM_EMIT("tst             %[count], $2") \
    __ASM_EMIT("beq             10f") \
    __ASM_EMIT("vst1.32         {d1}, [%[dst]]!") \
    __ASM_EMIT("10:") \
    __ASM_EMIT("tst             %[count], $1") \
    __ASM_EMIT("beq             12f") \
    __ASM_EMIT("vstm            %[dst], {s0}") \
    \
    __ASM_EMIT("12:") \

    void limit_saturate1(float *dst, size_t count)
    {
        IF_ARCH_ARM(float *src = dst);

        ARCH_ARM_ASM(
            LIMIT_SAT_BODY
            : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
            : [XC] "r" (&XLIM_SAT)
            : "cc", "memory",
              "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    void limit_saturate2(float *dst, const float *src, size_t count)
    {
        ARCH_ARM_ASM(
            LIMIT_SAT_BODY
            : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
            : [XC] "r" (&XLIM_SAT)
            : "cc", "memory",
              "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }


#undef LIMIT_SAT_BODY_X8
#undef LIMIT_SAT_BODY_X4
#undef LIMIT_SAT_BODY


}

#endif /* DSP_ARCH_ARM_NEON_D32_FLOAT_H_ */
