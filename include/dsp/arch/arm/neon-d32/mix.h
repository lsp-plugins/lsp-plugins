/*
 * mix.h
 *
 *  Created on: 9 нояб. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_ARM_NEON_D32_MIX_H_
#define DSP_ARCH_ARM_NEON_D32_MIX_H_

#ifndef DSP_ARCH_ARM_NEON_32_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_ARM_NEON_32_IMPL */

namespace neon_d32
{
    #define MIX2_CORE  \
        __ASM_EMIT("vld1.32     {d26[], d27[]}, [%[k1]]") \
        __ASM_EMIT("vld1.32     {d30[], d31[]}, [%[k2]]") \
        __ASM_EMIT("vmov        q14, q15") \
        __ASM_EMIT("vmov        q12, q13") \
        __ASM_EMIT("subs        %[count], $16") \
        __ASM_EMIT("bls         2f") \
        /* 16x blocks */ \
        __ASM_EMIT("1:") \
        __ASM_EMIT("vldm        %[src1]!, {q0-q3}") \
        __ASM_EMIT("vldm        %[src2]!, {q4-q7}") \
        __ASM_EMIT("vmul.f32    q0, q12") \
        __ASM_EMIT("vmul.f32    q1, q13") \
        __ASM_EMIT("vmul.f32    q2, q12") \
        __ASM_EMIT("vmul.f32    q3, q13") \
        __ASM_EMIT("vmla.f32    q0, q4, q14") \
        __ASM_EMIT("vmla.f32    q1, q5, q15") \
        __ASM_EMIT("vmla.f32    q2, q6, q14") \
        __ASM_EMIT("vmla.f32    q3, q7, q15") \
        __ASM_EMIT("subs        %[count], $16") \
        __ASM_EMIT("vstm        %[dst]!, {q0-q3}") \
        __ASM_EMIT("bhs         1b") \
        \
        /* 8x block */ \
        __ASM_EMIT("2:") \
        __ASM_EMIT("adds        %[count], $8") \
        __ASM_EMIT("blt         4f") \
        __ASM_EMIT("vldm        %[src1]!, {q0-q1}") \
        __ASM_EMIT("vldm        %[src2]!, {q4-q5}") \
        __ASM_EMIT("vmul.f32    q0, q12") \
        __ASM_EMIT("vmul.f32    q1, q13") \
        __ASM_EMIT("vmla.f32    q0, q4, q14") \
        __ASM_EMIT("vmla.f32    q1, q5, q15") \
        __ASM_EMIT("sub         %[count], $8") \
        __ASM_EMIT("vstm        %[dst]!, {q0-q1}") \
        /* 4x block */ \
        __ASM_EMIT("4:") \
        __ASM_EMIT("adds        %[count], $4") \
        __ASM_EMIT("blt         6f") \
        __ASM_EMIT("vldm        %[src1]!, {q0}") \
        __ASM_EMIT("vldm        %[src2]!, {q4}") \
        __ASM_EMIT("vmul.f32    q0, q12") \
        __ASM_EMIT("vmla.f32    q0, q4, q14") \
        __ASM_EMIT("sub         %[count], $4") \
        __ASM_EMIT("vstm        %[dst]!, {q0}") \
        /* 1x blocks */ \
        __ASM_EMIT("6:") \
        __ASM_EMIT("adds        %[count], $3") \
        __ASM_EMIT("blt         8f") \
        __ASM_EMIT("vmov        q1, q12") \
        __ASM_EMIT("vmov        q2, q14") \
        __ASM_EMIT("7:") \
        __ASM_EMIT("vldm        %[src1]!, {s0}") \
        __ASM_EMIT("vldm        %[src2]!, {s1}") \
        __ASM_EMIT("vmul.f32    s0, s4") \
        __ASM_EMIT("vmla.f32    s0, s1, s8") \
        __ASM_EMIT("subs        %[count], $1") \
        __ASM_EMIT("vstm        %[dst]!, {s0}") \
        __ASM_EMIT("bge         7b") \
        \
        __ASM_EMIT("8:")


    void mix2(float *dst, const float *src, float k1, float k2, size_t count)
    {
        IF_ARCH_ARM(float *tgt = dst);
        ARCH_ARM_ASM(
            MIX2_CORE
            : [dst] "+r" (tgt), [src1] "+r" (dst), [src2] "+r" (src),
              [count] "+r" (count)
            : [k1] "r" (&k1), [k2] "r" (&k2)
            : "cc", "memory",
              "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    void mix_copy2(float *dst, const float *src1, const float *src2, float k1, float k2, size_t count)
    {
        ARCH_ARM_ASM(
            MIX2_CORE
            : [dst] "+r" (dst), [src1] "+r" (src1), [src2] "+r" (src2),
              [count] "+r" (count)
            : [k1] "r" (&k1), [k2] "r" (&k2)
            : "cc", "memory",
              "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    #undef MIX2_CORE

    #define MIX2_ADD_CORE  \
        __ASM_EMIT("vld1.32     {d26[], d27[]}, [%[k1]]") \
        __ASM_EMIT("vld1.32     {d30[], d31[]}, [%[k2]]") \
        __ASM_EMIT("vmov        q14, q15") \
        __ASM_EMIT("vmov        q12, q13") \
        __ASM_EMIT("subs        %[count], $16") \
        __ASM_EMIT("bls         2f") \
        /* 16x blocks */ \
        __ASM_EMIT("1:") \
        __ASM_EMIT("vldm        %[dst], {q8-q11}") \
        __ASM_EMIT("vldm        %[src1]!, {q0-q3}") \
        __ASM_EMIT("vldm        %[src2]!, {q4-q7}") \
        __ASM_EMIT("vmla.f32    q8, q0, q12") \
        __ASM_EMIT("vmla.f32    q9, q1, q13") \
        __ASM_EMIT("vmla.f32    q10, q2, q12") \
        __ASM_EMIT("vmla.f32    q11, q3, q13") \
        __ASM_EMIT("vmla.f32    q8, q4, q14") \
        __ASM_EMIT("vmla.f32    q9, q5, q15") \
        __ASM_EMIT("vmla.f32    q10, q6, q14") \
        __ASM_EMIT("vmla.f32    q11, q7, q15") \
        __ASM_EMIT("subs        %[count], $16") \
        __ASM_EMIT("vstm        %[dst]!, {q8-q11}") \
        __ASM_EMIT("bhs         1b") \
        \
        /* 8x block */ \
        __ASM_EMIT("2:") \
        __ASM_EMIT("adds        %[count], $8") \
        __ASM_EMIT("blt         4f") \
        __ASM_EMIT("vldm        %[dst], {q8-q9}") \
        __ASM_EMIT("vldm        %[src1]!, {q0-q1}") \
        __ASM_EMIT("vldm        %[src2]!, {q4-q5}") \
        __ASM_EMIT("vmla.f32    q8, q0, q12") \
        __ASM_EMIT("vmla.f32    q9, q1, q13") \
        __ASM_EMIT("vmla.f32    q8, q4, q14") \
        __ASM_EMIT("vmla.f32    q9, q5, q15") \
        __ASM_EMIT("sub         %[count], $8") \
        __ASM_EMIT("vstm        %[dst]!, {q8-q9}") \
        /* 4x block */ \
        __ASM_EMIT("4:") \
        __ASM_EMIT("adds        %[count], $4") \
        __ASM_EMIT("blt         6f") \
        __ASM_EMIT("vldm        %[dst], {q8}") \
        __ASM_EMIT("vldm        %[src1]!, {q0}") \
        __ASM_EMIT("vldm        %[src2]!, {q4}") \
        __ASM_EMIT("vmla.f32    q8, q0, q12") \
        __ASM_EMIT("vmla.f32    q8, q4, q14") \
        __ASM_EMIT("sub         %[count], $4") \
        __ASM_EMIT("vstm        %[dst]!, {q8}") \
        /* 1x blocks */ \
        __ASM_EMIT("6:") \
        __ASM_EMIT("adds        %[count], $3") \
        __ASM_EMIT("blt         8f") \
        __ASM_EMIT("vmov        q1, q12") \
        __ASM_EMIT("vmov        q2, q14") \
        __ASM_EMIT("7:") \
        __ASM_EMIT("vldm        %[dst], {s2}") \
        __ASM_EMIT("vldm        %[src1]!, {s0}") \
        __ASM_EMIT("vldm        %[src2]!, {s1}") \
        __ASM_EMIT("vmla.f32    s2, s0, s4") \
        __ASM_EMIT("vmla.f32    s2, s1, s8") \
        __ASM_EMIT("subs        %[count], $1") \
        __ASM_EMIT("vstm        %[dst]!, {s2}") \
        __ASM_EMIT("bge         7b") \
        \
        __ASM_EMIT("8:")

    void mix_add2(float *dst, const float *src1, const float *src2, float k1, float k2, size_t count)
    {
        ARCH_ARM_ASM(
            MIX2_ADD_CORE
            : [dst] "+r" (dst), [src1] "+r" (src1), [src2] "+r" (src2),
              [count] "+r" (count)
            : [k1] "r" (&k1), [k2] "r" (&k2)
            : "cc", "memory",
              "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    #undef MIX2_ADD_CORE

    #define MIX3_CORE  \
        __ASM_EMIT("vld1.32     {d26[], d27[]}, [%[k1]]") \
        __ASM_EMIT("vld1.32     {d28[], d29[]}, [%[k2]]") \
        __ASM_EMIT("vld1.32     {d30[], d31[]}, [%[k3]]") \
        __ASM_EMIT("subs        %[count], $16") \
        __ASM_EMIT("bls         2f") \
        /* 16x blocks */ \
        __ASM_EMIT("1:") \
        __ASM_EMIT("vldm        %[src1]!, {q0-q3}") \
        __ASM_EMIT("vldm        %[src2]!, {q4-q7}") \
        __ASM_EMIT("vldm        %[src3]!, {q8-q11}") \
        __ASM_EMIT("vmul.f32    q0, q0, q13") \
        __ASM_EMIT("vmul.f32    q1, q1, q13") \
        __ASM_EMIT("vmul.f32    q2, q2, q13") \
        __ASM_EMIT("vmul.f32    q3, q3, q13") \
        __ASM_EMIT("vmla.f32    q0, q4, q14") \
        __ASM_EMIT("vmla.f32    q1, q5, q14") \
        __ASM_EMIT("vmla.f32    q2, q6, q14") \
        __ASM_EMIT("vmla.f32    q3, q7, q14") \
        __ASM_EMIT("vmla.f32    q0, q8, q15") \
        __ASM_EMIT("vmla.f32    q1, q9, q15") \
        __ASM_EMIT("vmla.f32    q2, q10, q15") \
        __ASM_EMIT("vmla.f32    q3, q11, q15") \
        __ASM_EMIT("subs        %[count], $16") \
        __ASM_EMIT("vstm        %[dst]!, {q0-q3}") \
        __ASM_EMIT("bhs         1b") \
        \
        /* 8x block */ \
        __ASM_EMIT("2:") \
        __ASM_EMIT("adds        %[count], $8") \
        __ASM_EMIT("blt         4f") \
        __ASM_EMIT("vldm        %[src1]!, {q0-q1}") \
        __ASM_EMIT("vldm        %[src2]!, {q4-q5}") \
        __ASM_EMIT("vldm        %[src3]!, {q8-q9}") \
        __ASM_EMIT("vmul.f32    q0, q0, q13") \
        __ASM_EMIT("vmul.f32    q1, q1, q13") \
        __ASM_EMIT("vmla.f32    q0, q4, q14") \
        __ASM_EMIT("vmla.f32    q1, q5, q14") \
        __ASM_EMIT("vmla.f32    q0, q8, q15") \
        __ASM_EMIT("vmla.f32    q1, q9, q15") \
        __ASM_EMIT("sub         %[count], $8") \
        __ASM_EMIT("vstm        %[dst]!, {q0-q1}") \
        /* 4x block */ \
        __ASM_EMIT("4:") \
        __ASM_EMIT("adds        %[count], $4") \
        __ASM_EMIT("blt         6f") \
        __ASM_EMIT("vldm        %[src1]!, {q0}") \
        __ASM_EMIT("vldm        %[src2]!, {q4}") \
        __ASM_EMIT("vldm        %[src3]!, {q8}") \
        __ASM_EMIT("vmul.f32    q0, q0, q13") \
        __ASM_EMIT("vmla.f32    q0, q4, q14") \
        __ASM_EMIT("vmla.f32    q0, q8, q15") \
        __ASM_EMIT("sub         %[count], $4") \
        __ASM_EMIT("vstm        %[dst]!, {q0}") \
        /* 1x blocks */ \
        __ASM_EMIT("6:") \
        __ASM_EMIT("adds        %[count], $3") \
        __ASM_EMIT("blt         8f") \
        __ASM_EMIT("vmov        q1, q13") \
        __ASM_EMIT("vmov        q2, q14") \
        __ASM_EMIT("vmov        q3, q15") \
        __ASM_EMIT("7:") \
        __ASM_EMIT("vldm        %[src1]!, {s0}") \
        __ASM_EMIT("vldm        %[src2]!, {s1}") \
        __ASM_EMIT("vldm        %[src3]!, {s2}") \
        __ASM_EMIT("vmul.f32    s0, s0, s4") \
        __ASM_EMIT("vmla.f32    s0, s1, s8") \
        __ASM_EMIT("vmla.f32    s0, s2, s12") \
        __ASM_EMIT("subs        %[count], $1") \
        __ASM_EMIT("vstm        %[dst]!, {s0}") \
        __ASM_EMIT("bge         7b") \
        \
        __ASM_EMIT("8:")

        void mix3(float *dst, const float *src1, const float *src2, float k1, float k2, float k3, size_t count)
        {
            IF_ARCH_ARM(float *tgt = dst);
            ARCH_ARM_ASM(
                MIX3_CORE
                : [dst] "+r" (tgt), [src1] "+r" (dst), [src2] "+r" (src1), [src3] "+r" (src2),
                  [count] "+r" (count)
                : [k1] "r" (&k1), [k2] "r" (&k2), [k3] "r" (&k3)
                : "cc", "memory",
                  "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
            );
        }

        void mix_copy3(float *dst, const float *src1, const float *src2, const float *src3, float k1, float k2, float k3, size_t count)
        {
            ARCH_ARM_ASM(
                MIX3_CORE
                : [dst] "+r" (dst), [src1] "+r" (src1), [src2] "+r" (src2), [src3] "+r" (src3),
                  [count] "+r" (count)
                : [k1] "r" (&k1), [k2] "r" (&k2), [k3] "r" (&k3)
                : "cc", "memory",
                  "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
            );
        }

    #undef MIX3_CORE

    #define MIX_ADD3_CORE  \
        __ASM_EMIT("vld1.32     {d26[], d27[]}, [%[k1]]") \
        __ASM_EMIT("vld1.32     {d28[], d29[]}, [%[k2]]") \
        __ASM_EMIT("vld1.32     {d30[], d31[]}, [%[k3]]") \
        __ASM_EMIT("subs        %[count], $16") \
        __ASM_EMIT("bls         2f") \
        /* 16x blocks */ \
        __ASM_EMIT("1:") \
        __ASM_EMIT("vldm        %[dst], {q0-q3}") \
        __ASM_EMIT("vldm        %[src1]!, {q4-q7}") \
        __ASM_EMIT("vldm        %[src2]!, {q8-q11}") \
        __ASM_EMIT("vmla.f32    q0, q4, q13") \
        __ASM_EMIT("vmla.f32    q1, q5, q13") \
        __ASM_EMIT("vmla.f32    q2, q6, q13") \
        __ASM_EMIT("vmla.f32    q3, q7, q13") \
        __ASM_EMIT("vmla.f32    q0, q8, q14") \
        __ASM_EMIT("vmla.f32    q1, q9, q14") \
        __ASM_EMIT("vldm        %[src3]!, {q4-q7}") \
        __ASM_EMIT("vmla.f32    q2, q10, q14") \
        __ASM_EMIT("vmla.f32    q3, q11, q14") \
        __ASM_EMIT("vmla.f32    q0, q4, q15") \
        __ASM_EMIT("vmla.f32    q1, q5, q15") \
        __ASM_EMIT("vmla.f32    q2, q6, q15") \
        __ASM_EMIT("vmla.f32    q3, q7, q15") \
        __ASM_EMIT("subs        %[count], $16") \
        __ASM_EMIT("vstm        %[dst]!, {q0-q3}") \
        __ASM_EMIT("bhs         1b") \
        \
        /* 8x block */ \
        __ASM_EMIT("2:") \
        __ASM_EMIT("adds        %[count], $8") \
        __ASM_EMIT("blt         4f") \
        __ASM_EMIT("vldm        %[dst], {q0-q1}") \
        __ASM_EMIT("vldm        %[src1]!, {q2-q3}") \
        __ASM_EMIT("vldm        %[src2]!, {q4-q5}") \
        __ASM_EMIT("vldm        %[src3]!, {q6-q7}") \
        __ASM_EMIT("vmla.f32    q0, q2, q13") \
        __ASM_EMIT("vmla.f32    q1, q3, q13") \
        __ASM_EMIT("vmla.f32    q0, q4, q14") \
        __ASM_EMIT("vmla.f32    q1, q5, q14") \
        __ASM_EMIT("vmla.f32    q0, q6, q15") \
        __ASM_EMIT("vmla.f32    q1, q7, q15") \
        __ASM_EMIT("sub         %[count], $8") \
        __ASM_EMIT("vstm        %[dst]!, {q0-q1}") \
        /* 4x block */ \
        __ASM_EMIT("4:") \
        __ASM_EMIT("adds        %[count], $4") \
        __ASM_EMIT("blt         6f") \
        __ASM_EMIT("vldm        %[dst], {q0}") \
        __ASM_EMIT("vldm        %[src1]!, {q2}") \
        __ASM_EMIT("vldm        %[src2]!, {q4}") \
        __ASM_EMIT("vldm        %[src3]!, {q6}") \
        __ASM_EMIT("vmla.f32    q0, q2, q13") \
        __ASM_EMIT("vmla.f32    q0, q4, q14") \
        __ASM_EMIT("vmla.f32    q0, q6, q15") \
        __ASM_EMIT("sub         %[count], $4") \
        __ASM_EMIT("vstm        %[dst]!, {q0}") \
        /* 1x blocks */ \
        __ASM_EMIT("6:") \
        __ASM_EMIT("adds        %[count], $3") \
        __ASM_EMIT("blt         8f") \
        __ASM_EMIT("vmov        q1, q13") \
        __ASM_EMIT("vmov        q2, q14") \
        __ASM_EMIT("vmov        q3, q15") \
        __ASM_EMIT("7:") \
        __ASM_EMIT("vldm        %[dst], {s0}") \
        __ASM_EMIT("vldm        %[src1]!, {s1}") \
        __ASM_EMIT("vldm        %[src2]!, {s2}") \
        __ASM_EMIT("vldm        %[src3]!, {s3}") \
        __ASM_EMIT("vmla.f32    s0, s1, s4") \
        __ASM_EMIT("vmla.f32    s0, s2, s8") \
        __ASM_EMIT("vmla.f32    s0, s3, s12") \
        __ASM_EMIT("subs        %[count], $1") \
        __ASM_EMIT("vstm        %[dst]!, {s0}") \
        __ASM_EMIT("bge         7b") \
        \
        __ASM_EMIT("8:")

        void mix_add3(float *dst, const float *src1, const float *src2, const float *src3, float k1, float k2, float k3, size_t count)
        {
            ARCH_ARM_ASM(
                MIX_ADD3_CORE
                : [dst] "+r" (dst), [src1] "+r" (src1), [src2] "+r" (src2), [src3] "+r" (src3),
                  [count] "+r" (count)
                : [k1] "r" (&k1), [k2] "r" (&k2), [k3] "r" (&k3)
                : "cc", "memory",
                  "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
            );
        }

    #undef MIX_ADD3_CORE

    #define MIX4_CORE  \
        __ASM_EMIT("vld1.32     {d24[], d25[]}, [%[k1]]") \
        __ASM_EMIT("vld1.32     {d26[], d27[]}, [%[k2]]") \
        __ASM_EMIT("vld1.32     {d28[], d29[]}, [%[k3]]") \
        __ASM_EMIT("vld1.32     {d30[], d31[]}, [%[k4]]") \
        __ASM_EMIT("subs        %[count], $16") \
        __ASM_EMIT("bls         2f") \
        /* 16x blocks */ \
        __ASM_EMIT("1:") \
        __ASM_EMIT("vldm        %[src1]!, {q0-q3}") \
        __ASM_EMIT("vldm        %[src2]!, {q4-q7}") \
        __ASM_EMIT("vldm        %[src3]!, {q8-q11}") \
        __ASM_EMIT("vmul.f32    q0, q0, q12") \
        __ASM_EMIT("vmul.f32    q1, q1, q12") \
        __ASM_EMIT("vmul.f32    q2, q2, q12") \
        __ASM_EMIT("vmul.f32    q3, q3, q12") \
        __ASM_EMIT("vmla.f32    q0, q4, q13") \
        __ASM_EMIT("vmla.f32    q1, q5, q13") \
        __ASM_EMIT("vmla.f32    q2, q6, q13") \
        __ASM_EMIT("vmla.f32    q3, q7, q13") \
        __ASM_EMIT("vmla.f32    q0, q8, q14") \
        __ASM_EMIT("vmla.f32    q1, q9, q14") \
        __ASM_EMIT("vldm        %[src4]!, {q4-q7}") \
        __ASM_EMIT("vmla.f32    q2, q10, q14") \
        __ASM_EMIT("vmla.f32    q3, q11, q14") \
        __ASM_EMIT("vmla.f32    q0, q4, q15") \
        __ASM_EMIT("vmla.f32    q1, q5, q15") \
        __ASM_EMIT("vmla.f32    q2, q6, q15") \
        __ASM_EMIT("vmla.f32    q3, q7, q15") \
        __ASM_EMIT("subs        %[count], $16") \
        __ASM_EMIT("vstm        %[dst]!, {q0-q3}") \
        __ASM_EMIT("bhs         1b") \
        \
        /* 8x block */ \
        __ASM_EMIT("2:") \
        __ASM_EMIT("adds        %[count], $8") \
        __ASM_EMIT("blt         4f") \
        __ASM_EMIT("vldm        %[src1]!, {q0-q1}") \
        __ASM_EMIT("vldm        %[src2]!, {q2-q3}") \
        __ASM_EMIT("vldm        %[src3]!, {q4-q5}") \
        __ASM_EMIT("vldm        %[src4]!, {q6-q7}") \
        __ASM_EMIT("vmul.f32    q0, q0, q12") \
        __ASM_EMIT("vmul.f32    q1, q1, q12") \
        __ASM_EMIT("vmla.f32    q0, q2, q13") \
        __ASM_EMIT("vmla.f32    q1, q3, q13") \
        __ASM_EMIT("vmla.f32    q0, q4, q14") \
        __ASM_EMIT("vmla.f32    q1, q5, q14") \
        __ASM_EMIT("vmla.f32    q0, q6, q15") \
        __ASM_EMIT("vmla.f32    q1, q7, q15") \
        __ASM_EMIT("sub         %[count], $8") \
        __ASM_EMIT("vstm        %[dst]!, {q0-q1}") \
        /* 4x block */ \
        __ASM_EMIT("4:") \
        __ASM_EMIT("adds        %[count], $4") \
        __ASM_EMIT("blt         6f") \
        __ASM_EMIT("vldm        %[src1]!, {q0}") \
        __ASM_EMIT("vldm        %[src2]!, {q2}") \
        __ASM_EMIT("vldm        %[src3]!, {q4}") \
        __ASM_EMIT("vldm        %[src4]!, {q6}") \
        __ASM_EMIT("vmul.f32    q0, q0, q12") \
        __ASM_EMIT("vmla.f32    q0, q2, q13") \
        __ASM_EMIT("vmla.f32    q0, q4, q14") \
        __ASM_EMIT("vmla.f32    q0, q6, q15") \
        __ASM_EMIT("sub         %[count], $4") \
        __ASM_EMIT("vstm        %[dst]!, {q0}") \
        /* 1x blocks */ \
        __ASM_EMIT("6:") \
        __ASM_EMIT("adds        %[count], $3") \
        __ASM_EMIT("blt         8f") \
        __ASM_EMIT("vmov        q1, q12") \
        __ASM_EMIT("vmov        q2, q13") \
        __ASM_EMIT("vmov        q3, q14") \
        __ASM_EMIT("vmov        q4, q15") \
        __ASM_EMIT("7:") \
        __ASM_EMIT("vldm        %[src1]!, {s0}") \
        __ASM_EMIT("vldm        %[src2]!, {s1}") \
        __ASM_EMIT("vldm        %[src3]!, {s2}") \
        __ASM_EMIT("vldm        %[src4]!, {s3}") \
        __ASM_EMIT("vmul.f32    s0, s0, s4") \
        __ASM_EMIT("vmla.f32    s0, s1, s8") \
        __ASM_EMIT("vmla.f32    s0, s2, s12") \
        __ASM_EMIT("vmla.f32    s0, s3, s16") \
        __ASM_EMIT("subs        %[count], $1") \
        __ASM_EMIT("vstm        %[dst]!, {s0}") \
        __ASM_EMIT("bge         7b") \
        \
        __ASM_EMIT("8:")

    void mix4(float *dst, const float *src1, const float *src2, const float *src3, float k1, float k2, float k3, float k4, size_t count)
    {
        IF_ARCH_ARM(float *tgt = dst);
        ARCH_ARM_ASM(
            MIX4_CORE
            : [dst] "+r" (tgt), [src1] "+r" (dst), [src2] "+r" (src1), [src3] "+r" (src2), [src4] "+r" (src3),
              [count] "+r" (count)
            : [k1] "r" (&k1), [k2] "r" (&k2), [k3] "r" (&k3), [k4] "r" (&k4)
            : "cc", "memory",
              "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    void mix_copy4(float *dst, const float *src1, const float *src2, const float *src3, const float *src4, float k1, float k2, float k3, float k4, size_t count)
    {
        ARCH_ARM_ASM(
            MIX4_CORE
            : [dst] "+r" (dst), [src1] "+r" (src1), [src2] "+r" (src2), [src3] "+r" (src3), [src4] "+r" (src4),
              [count] "+r" (count)
            : [k1] "r" (&k1), [k2] "r" (&k2), [k3] "r" (&k3), [k4] "r" (&k4)
            : "cc", "memory",
              "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    #undef MIX4_CORE

    #define MIX_ADD4_CORE  \
        __ASM_EMIT("vld1.32     {d24[], d25[]}, [%[k1]]") \
        __ASM_EMIT("vld1.32     {d26[], d27[]}, [%[k2]]") \
        __ASM_EMIT("vld1.32     {d28[], d29[]}, [%[k3]]") \
        __ASM_EMIT("vld1.32     {d30[], d31[]}, [%[k4]]") \
        __ASM_EMIT("subs        %[count], $16") \
        __ASM_EMIT("bls         2f") \
        /* 16x blocks */ \
        __ASM_EMIT("1:") \
        __ASM_EMIT("vldm        %[dst], {q0-q3}") \
        __ASM_EMIT("vldm        %[src1]!, {q4-q7}") \
        __ASM_EMIT("vldm        %[src2]!, {q8-q11}") \
        __ASM_EMIT("vmla.f32    q0, q4, q12") \
        __ASM_EMIT("vmla.f32    q1, q5, q12") \
        __ASM_EMIT("vmla.f32    q2, q6, q12") \
        __ASM_EMIT("vmla.f32    q3, q7, q12") \
        __ASM_EMIT("vmla.f32    q0, q8, q13") \
        __ASM_EMIT("vmla.f32    q1, q9, q13") \
        __ASM_EMIT("vldm        %[src3]!, {q4-q7}") \
        __ASM_EMIT("vmla.f32    q2, q10, q13") \
        __ASM_EMIT("vmla.f32    q3, q11, q13") \
        __ASM_EMIT("vmla.f32    q0, q4, q14") \
        __ASM_EMIT("vmla.f32    q1, q5, q14") \
        __ASM_EMIT("vldm        %[src4]!, {q8-q11}") \
        __ASM_EMIT("vmla.f32    q2, q6, q14") \
        __ASM_EMIT("vmla.f32    q3, q7, q14") \
        __ASM_EMIT("vmla.f32    q0, q8, q15") \
        __ASM_EMIT("vmla.f32    q1, q9, q15") \
        __ASM_EMIT("vmla.f32    q2, q10, q15") \
        __ASM_EMIT("vmla.f32    q3, q11, q15") \
        __ASM_EMIT("subs        %[count], $16") \
        __ASM_EMIT("vstm        %[dst]!, {q0-q3}") \
        __ASM_EMIT("bhs         1b") \
        \
        /* 8x block */ \
        __ASM_EMIT("2:") \
        __ASM_EMIT("adds        %[count], $8") \
        __ASM_EMIT("blt         4f") \
        __ASM_EMIT("vldm        %[dst], {q0-q1}") \
        __ASM_EMIT("vldm        %[src1]!, {q2-q3}") \
        __ASM_EMIT("vldm        %[src2]!, {q4-q5}") \
        __ASM_EMIT("vldm        %[src3]!, {q6-q7}") \
        __ASM_EMIT("vldm        %[src4]!, {q8-q9}") \
        __ASM_EMIT("vmla.f32    q0, q2, q12") \
        __ASM_EMIT("vmla.f32    q1, q3, q12") \
        __ASM_EMIT("vmla.f32    q0, q4, q13") \
        __ASM_EMIT("vmla.f32    q1, q5, q13") \
        __ASM_EMIT("vmla.f32    q0, q6, q14") \
        __ASM_EMIT("vmla.f32    q1, q7, q14") \
        __ASM_EMIT("vmla.f32    q0, q8, q15") \
        __ASM_EMIT("vmla.f32    q1, q9, q15") \
        __ASM_EMIT("sub         %[count], $8") \
        __ASM_EMIT("vstm        %[dst]!, {q0-q1}") \
        /* 4x block */ \
        __ASM_EMIT("4:") \
        __ASM_EMIT("adds        %[count], $4") \
        __ASM_EMIT("blt         6f") \
        __ASM_EMIT("vldm        %[dst], {q0}") \
        __ASM_EMIT("vldm        %[src1]!, {q2}") \
        __ASM_EMIT("vldm        %[src2]!, {q4}") \
        __ASM_EMIT("vldm        %[src3]!, {q6}") \
        __ASM_EMIT("vldm        %[src4]!, {q8}") \
        __ASM_EMIT("vmla.f32    q0, q2, q12") \
        __ASM_EMIT("vmla.f32    q0, q4, q13") \
        __ASM_EMIT("vmla.f32    q0, q6, q14") \
        __ASM_EMIT("vmla.f32    q0, q8, q15") \
        __ASM_EMIT("sub         %[count], $4") \
        __ASM_EMIT("vstm        %[dst]!, {q0}") \
        /* 1x blocks */ \
        __ASM_EMIT("6:") \
        __ASM_EMIT("adds        %[count], $3") \
        __ASM_EMIT("blt         8f") \
        __ASM_EMIT("vmov        q2, q12") \
        __ASM_EMIT("vmov        q3, q13") \
        __ASM_EMIT("vmov        q4, q14") \
        __ASM_EMIT("vmov        q5, q15") \
        __ASM_EMIT("7:") \
        __ASM_EMIT("vldm        %[dst], {s0}") \
        __ASM_EMIT("vldm        %[src1]!, {s1}") \
        __ASM_EMIT("vldm        %[src2]!, {s2}") \
        __ASM_EMIT("vldm        %[src3]!, {s3}") \
        __ASM_EMIT("vldm        %[src4]!, {s4}") \
        __ASM_EMIT("vmla.f32    s0, s1, s8") \
        __ASM_EMIT("vmla.f32    s0, s2, s12") \
        __ASM_EMIT("vmla.f32    s0, s3, s16") \
        __ASM_EMIT("vmla.f32    s0, s4, s20") \
        __ASM_EMIT("subs        %[count], $1") \
        __ASM_EMIT("vstm        %[dst]!, {s0}") \
        __ASM_EMIT("bge         7b") \
        \
        __ASM_EMIT("8:")

    void mix_add4(float *dst, const float *src1, const float *src2, const float *src3, const float *src4, float k1, float k2, float k3, float k4, size_t count)
    {
        ARCH_ARM_ASM(
            MIX_ADD4_CORE
            : [dst] "+r" (dst), [src1] "+r" (src1), [src2] "+r" (src2), [src3] "+r" (src3), [src4] "+r" (src4),
              [count] "+r" (count)
            : [k1] "r" (&k1), [k2] "r" (&k2), [k3] "r" (&k3), [k4] "r" (&k4)
            : "cc", "memory",
              "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    #undef MIX_ADD4_CORE

}

#endif /* DSP_ARCH_ARM_NEON_D32_MIX_H_ */
