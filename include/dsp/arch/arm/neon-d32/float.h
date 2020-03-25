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
#define MULTIPLE_SATURATION_BODY(DST, SRC, SINC) \
    __ASM_EMIT("vldm            %[X_IARGS], {q8-q13}") \
    /* q8 = +inf, q9 = -inf */ \
    /* q10 = X_P_NAN, q11 = X_P_INF */ \
    /* q12 = X_N_NAN, q13 = X_N_INF */ \
    /* x8 blocks */ \
    __ASM_EMIT("subs            %[count], $8") \
    __ASM_EMIT("blo             2f") \
    __ASM_EMIT("1:") \
    __ASM_EMIT("vldm            %[" SRC "]" SINC ", {q0-q1}")       /* q0   = s */ \
    __ASM_EMIT("vcgt.s32        q2, q0, q8")                        /* q2   = [s > +inf] */ \
    __ASM_EMIT("vcgt.s32        q3, q1, q8") \
    __ASM_EMIT("vceq.s32        q4, q0, q8")                        /* q4   = [s == +inf] */ \
    __ASM_EMIT("vceq.s32        q5, q1, q8") \
    __ASM_EMIT("vbit            q0, q10, q2")                       /* q0   = s' = X_P_NAN * [s > +inf] | s * [s <= +inf] */ \
    __ASM_EMIT("vbit            q1, q10, q3") \
    __ASM_EMIT("vbit            q0, q11, q4")                       /* q0   = X_P_INF * [s == +inf] | s' * [s != +inf] */ \
    __ASM_EMIT("vbit            q1, q11, q5") \
    __ASM_EMIT("vcgt.u32        q2, q0, q9")                        /* q2   = [s > -inf] */ \
    __ASM_EMIT("vcgt.u32        q3, q1, q9") \
    __ASM_EMIT("vceq.u32        q4, q0, q9")                        /* q4   = [s == -inf] */ \
    __ASM_EMIT("vceq.u32        q5, q1, q9") \
    __ASM_EMIT("vbit            q0, q12, q2")                       /* v0   = s' = X_N_NAN * [s > -inf] | s * [s <= -inf] */ \
    __ASM_EMIT("vbit            q1, q12, q3") \
    __ASM_EMIT("vbit            q0, q13, q4")                       /* v0   = X_N_INF * [s == +inf] | s' * [s != +inf] */ \
    __ASM_EMIT("vbit            q1, q13, q5") \
    __ASM_EMIT("subs            %[count], $8") \
    __ASM_EMIT("vstm            %[" DST "]!, {q0-q1}") \
    __ASM_EMIT("bhs             1b") \
    __ASM_EMIT("2:") \
    /* x4 block */ \
    __ASM_EMIT("adds            %[count], $4") \
    __ASM_EMIT("blt             4f") \
    __ASM_EMIT("vldm            %[" SRC "]" SINC ", {q0}")          /* q0 = s */ \
    __ASM_EMIT("vcgt.s32        q2, q0, q8")                        /* q2   = [s > +inf] */ \
    __ASM_EMIT("vceq.s32        q4, q0, q8")                        /* q4   = [s == +inf] */ \
    __ASM_EMIT("vbit            q0, q10, q2")                       /* q0   = s' = X_P_NAN * [s > +inf] | s * [s <= +inf] */ \
    __ASM_EMIT("vbit            q0, q11, q4")                       /* q0   = X_P_INF * [s == +inf] | s' * [s != +inf] */ \
    __ASM_EMIT("vcgt.u32        q2, q0, q9")                        /* q2   = [s > -inf] */ \
    __ASM_EMIT("vceq.u32        q4, q0, q9")                        /* q4   = [s == -inf] */ \
    __ASM_EMIT("vbit            q0, q12, q2")                       /* v0   = s' = X_N_NAN * [s > -inf] | s * [s <= -inf] */ \
    __ASM_EMIT("vbit            q0, q13, q4")                       /* v0   = X_N_INF * [s == +inf] | s' * [s != +inf] */ \
    __ASM_EMIT("sub             %[count], $4") \
    __ASM_EMIT("vstm            %[" DST "]!, {q0}") \
    __ASM_EMIT("4:") \
    /* x1 blocks */ \
    __ASM_EMIT("adds            %[count], $3") \
    __ASM_EMIT("blt             6f") \
    __ASM_EMIT("5:") \
    __ASM_EMIT("vld1.32         {d0[], d1[]}, [%[" SRC "]]" SINC)   /* q0 = s */ \
    __ASM_EMIT("vcgt.s32        q2, q0, q8")                        /* q2   = [s > +inf] */ \
    __ASM_EMIT("vceq.s32        q4, q0, q8")                        /* q4   = [s == +inf] */ \
    __ASM_EMIT("vbit            q0, q10, q2")                       /* q0   = s' = X_P_NAN * [s > +inf] | s * [s <= +inf] */ \
    __ASM_EMIT("vbit            q0, q11, q4")                       /* q0   = X_P_INF * [s == +inf] | s' * [s != +inf] */ \
    __ASM_EMIT("vcgt.u32        q2, q0, q9")                        /* q2   = [s > -inf] */ \
    __ASM_EMIT("vceq.u32        q4, q0, q9")                        /* q4   = [s == -inf] */ \
    __ASM_EMIT("vbit            q0, q12, q2")                       /* v0   = s' = X_N_NAN * [s > -inf] | s * [s <= -inf] */ \
    __ASM_EMIT("vbit            q0, q13, q4")                       /* v0   = X_N_INF * [s == +inf] | s' * [s != +inf] */ \
    __ASM_EMIT("subs            %[count], $1") \
    __ASM_EMIT("vst1.32         {d0[0]}, [%[" DST "]]!") \
    __ASM_EMIT("bge             5b") \
    __ASM_EMIT("6:")

    void copy_saturated(float *dst, const float *src, size_t count)
    {
        ARCH_ARM_ASM (
            MULTIPLE_SATURATION_BODY("dst", "src", "!")
            : [dst] "+r" (dst), [src] "+r" (src),
              [count] "+r" (count)
            : [X_IARGS] "r" (&SAT_IARGS[4])
            : "cc", "memory",
              "q0", "q1", "q2", "q3",
              "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11",
              "q12", "q13"
        );
    }

    void saturate(float *dst, size_t count)
    {
        ARCH_ARM_ASM(
            MULTIPLE_SATURATION_BODY("dst", "dst", "")
            : [dst] "+r" (dst), [count] "+r" (count)
            : [X_IARGS] "r" (&SAT_IARGS[4])
            : "cc", "memory",
              "q0", "q1", "q2", "q3",
              "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11",
              "q12", "q13"
        );
    }

#undef MULTIPLE_SATURATION_BODY

#define U4VEC(x)        x, x, x, x
IF_ARCH_ARM(
    static uint32_t XLIM_SAT[] __lsp_aligned16 =
    {
        U4VEC(0x7fffffff),      // -1
        U4VEC(0x3f800000),      // +1
        U4VEC(0x80000000),      // sign
        U4VEC(0x7f800000)       // +Inf
    };
)

#undef U4VEC

#define LIMIT_SAT_BODY(DST, SRC, SINC) \
    __ASM_EMIT("vldm            %[XC], {q12-q15}")                  /* q12 = abs, q13 = +1, q14 = sign, q15 = +Inf */ \
    /* 8x blocks */ \
    __ASM_EMIT("subs            %[count], $8") \
    __ASM_EMIT("blo             2f") \
    __ASM_EMIT("1:") \
    __ASM_EMIT("vldm            %[" SRC "]" SINC ", {q0-q1}")       /* q0 = s0, q1 = s1 */ \
    __ASM_EMIT("vand            q6, q0, q12")                       /* q6 = abs(s) */ \
    __ASM_EMIT("vand            q7, q1, q12") \
    __ASM_EMIT("vcge.f32        q2, q13, q6")                       /* q2 = [ abs(s) <= +1 ] */ \
    __ASM_EMIT("vcge.f32        q3, q13, q7") \
    __ASM_EMIT("vand            q4, q0, q14")                       /* q4 = sign(s) */ \
    __ASM_EMIT("vand            q5, q1, q14") \
    __ASM_EMIT("vcge.s32        q6, q15, q6")                       /* q6 = [ abs(s) <= +Inf ] */ \
    __ASM_EMIT("vcge.s32        q7, q15, q7") \
    __ASM_EMIT("vorr            q4, q4, q13")                       /* q4 = +1 * sign(s) */ \
    __ASM_EMIT("vorr            q5, q5, q13") \
    __ASM_EMIT("vand            q4, q4, q6")                        /* v4 = r = +1 * sign(s) & [ abs(s) <= +Inf ] */ \
    __ASM_EMIT("vand            q5, q5, q7") \
    __ASM_EMIT("vbif            q0, q4, q2")                        /* q0 = ( s & [ abs(s) <= +1 ]) | ( r & [ abs(s) > +1 ]) */ \
    __ASM_EMIT("vbif            q1, q5, q3") \
    __ASM_EMIT("subs            %[count], $8") \
    __ASM_EMIT("vstm            %[" DST "]!, {q0-q1}") \
    __ASM_EMIT("bhs             1b") \
    __ASM_EMIT("2:") \
    /* 4x block */ \
    __ASM_EMIT("adds            %[count], $4") \
    __ASM_EMIT("blt             4f") \
    __ASM_EMIT("vldm            %[" SRC "]" SINC ", {q0}")          /* q0 = s0 */ \
    __ASM_EMIT("vand            q6, q0, q12")                       /* q6 = abs(s) */ \
    __ASM_EMIT("vcge.f32        q2, q13, q6")                       /* q2 = [ abs(s) <= +1 ] */ \
    __ASM_EMIT("vand            q4, q0, q14")                       /* q4 = sign(s) */ \
    __ASM_EMIT("vcge.s32        q6, q15, q6")                       /* q6 = [ abs(s) <= +Inf ] */ \
    __ASM_EMIT("vorr            q4, q4, q13")                       /* q4 = +1 * sign(s) */ \
    __ASM_EMIT("vand            q4, q4, q6")                        /* v4 = r = +1 * sign(s) & [ abs(s) <= +Inf ] */ \
    __ASM_EMIT("vbif            q0, q4, q2")                        /* q0 = ( s & [ abs(s) <= +1 ]) | ( r & [ abs(s) > +1 ]) */ \
    __ASM_EMIT("sub             %[count], $4") \
    __ASM_EMIT("vstm            %[" DST "]!, {q0}") \
    __ASM_EMIT("4:") \
    /* 1x blocks */ \
    __ASM_EMIT("adds            %[count], $3") \
    __ASM_EMIT("blt             6f") \
    __ASM_EMIT("5:") \
    __ASM_EMIT("vld1.32         {d0[], d1[]}, [%[" SRC "]]" SINC)   /* q0 = s0 */ \
    __ASM_EMIT("vand            q6, q0, q12")                       /* q6 = abs(s) */ \
    __ASM_EMIT("vcge.f32        q2, q13, q6")                       /* q2 = [ abs(s) <= +1 ] */ \
    __ASM_EMIT("vand            q4, q0, q14")                       /* q4 = sign(s) */ \
    __ASM_EMIT("vcge.s32        q6, q15, q6")                       /* q6 = [ abs(s) <= +Inf ] */ \
    __ASM_EMIT("vorr            q4, q4, q13")                       /* q4 = +1 * sign(s) */ \
    __ASM_EMIT("vand            q4, q4, q6")                        /* v4 = r = +1 * sign(s) & [ abs(s) <= +Inf ] */ \
    __ASM_EMIT("vbif            q0, q4, q2")                        /* q0 = ( s & [ abs(s) <= +1 ]) | ( r & [ abs(s) > +1 ]) */ \
    __ASM_EMIT("subs            %[count], $1") \
    __ASM_EMIT("vst1.32         {d0[0]}, [%[" DST "]]!") \
    __ASM_EMIT("bge             5b") \
    __ASM_EMIT("6:")

    void limit_saturate1(float *dst, size_t count)
    {
        ARCH_ARM_ASM(
            LIMIT_SAT_BODY("dst", "dst", "")
            : [dst] "+r" (dst), [count] "+r" (count)
            : [XC] "r" (&XLIM_SAT[0])
            : "cc", "memory",
              "q0", "q1", "q2", "q3",
              "q4", "q5", "q6", "q7",
              "q12", "q13", "q14", "q15"
        );
    }

    void limit_saturate2(float *dst, const float *src, size_t count)
    {
        ARCH_ARM_ASM(
            LIMIT_SAT_BODY("dst", "src", "!")
            : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
            : [XC] "r" (&XLIM_SAT[0])
            : "cc", "memory",
              "q0", "q1", "q2", "q3",
              "q4", "q5", "q6", "q7",
              "q12", "q13", "q14", "q15"
        );
    }


#undef LIMIT_SAT_BODY_X8
#undef LIMIT_SAT_BODY_X4
#undef LIMIT_SAT_BODY

#define LIMIT_BODY(DST, SRC, SINC) \
    __ASM_EMIT("vld1.32         {d16[], d17[]}, [%[min]]")      \
    __ASM_EMIT("vld1.32         {d18[], d19[]}, [%[max]]")      \
    __ASM_EMIT("subs            %[count], $16") \
    __ASM_EMIT("blo             2f") \
    /* 16x blocks */ \
    __ASM_EMIT("1:") \
    __ASM_EMIT("vldm            %[" SRC "]" SINC ", {q0-q3}")       /* q0 = s0, q1 = s1 */ \
    __ASM_EMIT("vcge.f32        q4, q0, q8")                        /* q4 = c = [ (s >= min) && (!isnan(s)) ] */ \
    __ASM_EMIT("vcge.f32        q5, q1, q8") \
    __ASM_EMIT("vcge.f32        q6, q2, q8") \
    __ASM_EMIT("vcge.f32        q7, q3, q8") \
    __ASM_EMIT("vbif            q0, q8, q4")                        /* q0 = s*c | min*(!c) */ \
    __ASM_EMIT("vbif            q1, q8, q5") \
    __ASM_EMIT("vbif            q2, q8, q6") \
    __ASM_EMIT("vbif            q3, q8, q7") \
    __ASM_EMIT("vcge.f32        q4, q9, q0")                        /* q4 = c = [ (s <= max) && (!isnan(s)) ] */ \
    __ASM_EMIT("vcge.f32        q5, q9, q1") \
    __ASM_EMIT("vcge.f32        q6, q9, q2") \
    __ASM_EMIT("vcge.f32        q7, q9, q3") \
    __ASM_EMIT("vbif            q0, q9, q4")                        /* q0 = s*c | min*(!c) */ \
    __ASM_EMIT("vbif            q1, q9, q5") \
    __ASM_EMIT("vbif            q2, q9, q6") \
    __ASM_EMIT("vbif            q3, q9, q7") \
    __ASM_EMIT("subs            %[count], $16") \
    __ASM_EMIT("vstm            %[" DST "]!, {q0-q3}")              /* q0 = s0, q1 = s1 */ \
    __ASM_EMIT("bhs             1b") \
    __ASM_EMIT("2:") \
    /* 8x blocks */ \
    __ASM_EMIT("adds            %[count], $8") \
    __ASM_EMIT("blt             4f") \
    __ASM_EMIT("vldm            %[" SRC "]" SINC ", {q0-q1}")       /* q0 = s0, q1 = s1 */ \
    __ASM_EMIT("vcge.f32        q4, q0, q8")                        /* q4 = c = [ (s >= min) && (!isnan(s)) ] */ \
    __ASM_EMIT("vcge.f32        q5, q1, q8") \
    __ASM_EMIT("vbif            q0, q8, q4")                        /* q0 = s*c | min*(!c) */ \
    __ASM_EMIT("vbif            q1, q8, q5") \
    __ASM_EMIT("vcge.f32        q4, q9, q0")                        /* q4 = c = [ (s <= max) && (!isnan(s)) ] */ \
    __ASM_EMIT("vcge.f32        q5, q9, q1") \
    __ASM_EMIT("vbif            q0, q9, q4")                        /* q0 = s*c | min*(!c) */ \
    __ASM_EMIT("vbif            q1, q9, q5") \
    __ASM_EMIT("sub             %[count], $8") \
    __ASM_EMIT("vstm            %[" DST "]!, {q0-q1}")              /* q0 = s0, q1 = s1 */ \
    __ASM_EMIT("4:") \
    /* 4x block */ \
    __ASM_EMIT("adds            %[count], $4") \
    __ASM_EMIT("blt             6f") \
    __ASM_EMIT("vldm            %[" SRC "]" SINC ", {q0}")          /* q0 = s0, q1 = s1 */ \
    __ASM_EMIT("vcge.f32        q4, q0, q8")                        /* q4 = c = [ (s >= min) && (!isnan(s)) ] */ \
    __ASM_EMIT("vbif            q0, q8, q4")                        /* q0 = s*c | min*(!c) */ \
    __ASM_EMIT("vcge.f32        q4, q9, q0")                        /* q4 = c = [ (s <= max) && (!isnan(s)) ] */ \
    __ASM_EMIT("vbif            q0, q9, q4")                        /* q0 = s*c | min*(!c) */ \
    __ASM_EMIT("sub             %[count], $4") \
    __ASM_EMIT("vstm            %[" DST "]!, {q0}")                 /* q0 = s0, q1 = s1 */ \
    __ASM_EMIT("6:") \
    /* 1x blocks */ \
    __ASM_EMIT("adds            %[count], $3") \
    __ASM_EMIT("blt             8f") \
    __ASM_EMIT("7:") \
    __ASM_EMIT("vld1.32         {d0[], d1[]}, [%[" SRC "]]" SINC) \
    __ASM_EMIT("vcge.f32        q4, q0, q8")                        /* q4 = c = [ (s >= min) && (!isnan(s)) ] */ \
    __ASM_EMIT("vbif            q0, q8, q4")                        /* q0 = s*c | min*(!c) */ \
    __ASM_EMIT("vcge.f32        q4, q9, q0")                        /* q4 = c = [ (s <= max) && (!isnan(s)) ] */ \
    __ASM_EMIT("vbif            q0, q9, q4")                        /* q0 = s*c | min*(!c) */ \
    __ASM_EMIT("subs            %[count], $1") \
    __ASM_EMIT("vst1.32         {d0[0]}, [%[" DST "]]!")            /* q0 = s0, q1 = s1 */ \
    __ASM_EMIT("bge             7b") \
    __ASM_EMIT("8:")


    void limit1(float *dst, float min, float max, size_t count)
    {
        ARCH_ARM_ASM(
            LIMIT_BODY("dst", "dst", "")
            : [dst] "+r" (dst), [count] "+r" (count)
            : [min] "r" (&min), [max] "r" (&max)
            : "cc", "memory",
              "q0", "q1", "q2", "q3",
              "q4", "q5", "q6", "q7",
              "q8", "q9"
        );
    }

    void limit2(float *dst, const float *src, float min, float max, size_t count)
    {
        ARCH_ARM_ASM(
            LIMIT_BODY("dst", "src", "!")
            : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
            : [min] "r" (&min), [max] "r" (&max)
            : "cc", "memory",
              "q0", "q1", "q2", "q3",
              "q4", "q5", "q6", "q7",
              "q8", "q9"
        );
    }

    #undef LIMIT_BODY

    #undef SEL_DST
    #undef SEL_NODST

    #define SANITIZE_BODY(DST, SRC, INC) \
        __ASM_EMIT("vldm            %[CVAL], {q8-q11}") \
        __ASM_EMIT("subs            %[count], %[count], $8") \
        __ASM_EMIT("blo             2f") \
        /* 8x blocks */ \
        __ASM_EMIT("1:") \
        __ASM_EMIT("vldm            %[" SRC "]" INC ", {q0-q1}")            /* q0 = s */ \
        __ASM_EMIT("vand            q2, q0, q8")                            /* q2 = abs(s) */ \
        __ASM_EMIT("vand            q3, q1, q8") \
        __ASM_EMIT("vand            q4, q0, q9")                            /* q4 = sign(s) */ \
        __ASM_EMIT("vand            q5, q1, q9") \
        __ASM_EMIT("vcle.u32        q6, q2, q10")                           /* q6 = abs(s) <= X_MAX */ \
        __ASM_EMIT("vcle.u32        q7, q3, q10") \
        __ASM_EMIT("vcgt.u32        q2, q2, q11")                           /* q2 = abs(s) > X_MIN */ \
        __ASM_EMIT("vcgt.u32        q3, q3, q11") \
        __ASM_EMIT("vand            q2, q2, q6")                            /* q2 = (abs(s) > X_MIN) & (abs(s) <= X_MIN) */ \
        __ASM_EMIT("vand            q3, q3, q7") \
        __ASM_EMIT("vbif            q0, q4, q2")                            /* q0 = ((abs(s) > X_MIN) & (abs(s) <= X_MIN)) ? s : sign(s) */ \
        __ASM_EMIT("vbif            q1, q5, q3") \
        __ASM_EMIT("subs            %[count], %[count], $8") \
        __ASM_EMIT("vstm            %[" DST "]!, {q0-q1}")                  /* q0 = s */ \
        __ASM_EMIT("bhs             1b") \
        /* 4x block */ \
        __ASM_EMIT("2:") \
        __ASM_EMIT("adds            %[count], %[count], $4") \
        __ASM_EMIT("blt             4f") \
        __ASM_EMIT("vldm            %[" SRC "]" INC ", {q0}")               /* q0 = s */ \
        __ASM_EMIT("vand            q2, q0, q8")                            /* q2 = abs(s) */ \
        __ASM_EMIT("vand            q4, q0, q9")                            /* q4 = sign(s) */ \
        __ASM_EMIT("vcle.u32        q6, q2, q10")                           /* q6 = abs(s) <= X_MAX */ \
        __ASM_EMIT("vcgt.u32        q2, q2, q11")                           /* q2 = abs(s) > X_MIN */ \
        __ASM_EMIT("vand            q2, q2, q6")                            /* q2 = (abs(s) > X_MIN) & (abs(s) <= X_MIN) */ \
        __ASM_EMIT("vbif            q0, q4, q2")                            /* q0 = ((abs(s) > X_MIN) & (abs(s) <= X_MIN)) ? s : sign(s) */ \
        __ASM_EMIT("sub             %[count], %[count], $4") \
        __ASM_EMIT("vstm            %[" DST "]!, {q0}")                     /* q0 = s */ \
        /* 1x blocks */ \
        __ASM_EMIT("4:") \
        __ASM_EMIT("adds            %[count], %[count], $3") \
        __ASM_EMIT("blt             6f") \
        __ASM_EMIT("5:") \
        __ASM_EMIT("vld1.32         {d0[]}, [%[" SRC "]]" INC)              /* q0 = s */ \
        __ASM_EMIT("vand            q2, q0, q8")                            /* q2 = abs(s) */ \
        __ASM_EMIT("vand            q4, q0, q9")                            /* q4 = sign(s) */ \
        __ASM_EMIT("vcle.u32        q6, q2, q10")                           /* q6 = abs(s) <= X_MAX */ \
        __ASM_EMIT("vcgt.u32        q2, q2, q11")                           /* q2 = abs(s) > X_MIN */ \
        __ASM_EMIT("vand            q2, q2, q6")                            /* q2 = (abs(s) > X_MIN) & (abs(s) <= X_MIN) */ \
        __ASM_EMIT("vbif            q0, q4, q2")                            /* q0 = ((abs(s) > X_MIN) & (abs(s) <= X_MIN)) ? s : sign(s) */ \
        __ASM_EMIT("subs            %[count], %[count], $1") \
        __ASM_EMIT("vst1.32         {d0[0]}, [%[" DST "]]!")                /* q0 = s */ \
        __ASM_EMIT("bge             5b") \
        /* end */ \
        __ASM_EMIT("6:")

    #define U4VEC(x)        x, x, x, x
    IF_ARCH_ARM(
        static uint32_t SANITIZE_CVAL[] __lsp_aligned16 =
        {
            U4VEC(0x7fffffff),            // X_ABS
            U4VEC(0x80000000),            // X_SIGN
            U4VEC(0x7f7fffff),            // X_MAX
            U4VEC(0x007fffff)             // X_MIN
        };
    )
    #undef U4VEC

    void sanitize1(float *dst, size_t count)
    {
        ARCH_ARM_ASM
        (
            SANITIZE_BODY("dst", "dst", "")
            : [dst] "+r" (dst),
              [count] "+r" (count)
            : [CVAL] "r" (&SANITIZE_CVAL[0])
            : "cc", "memory",
              "q0", "q1", "q2", "q3",
              "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11"
        );
    }

    void sanitize2(float *dst, const float *src, size_t count)
    {
        ARCH_ARM_ASM
        (
            SANITIZE_BODY("dst", "src", "!")
            : [dst] "+r" (dst), [src] "+r" (src),
              [count] "+r" (count)
            : [CVAL] "r" (&SANITIZE_CVAL[0])
            : "cc", "memory",
              "q0", "q1", "q2", "q3",
              "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11"
        );
    }

#undef SANITIZE_BODY
}

#endif /* DSP_ARCH_ARM_NEON_D32_FLOAT_H_ */
