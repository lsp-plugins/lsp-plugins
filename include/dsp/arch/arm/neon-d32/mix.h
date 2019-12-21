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
    void mix2(float *a, const float *b, float k1, float k2, size_t count)
    {
        ARCH_ARM_ASM(
            __ASM_EMIT("vdup.32     q13, %y[k1]")
            __ASM_EMIT("vdup.32     q15, %y[k2]")
            __ASM_EMIT("vmov        q14, q15")
            __ASM_EMIT("vmov        q12, q13")
            __ASM_EMIT("subs        %[count], $16")
            __ASM_EMIT("bls         2f")
            /* 16x blocks */
            __ASM_EMIT("1:")
            __ASM_EMIT("vldm        %[a], {q0-q3}")
            __ASM_EMIT("vldm        %[b]!, {q4-q7}")
            __ASM_EMIT("vmul.f32    q0, q12")
            __ASM_EMIT("vmul.f32    q1, q13")
            __ASM_EMIT("vmul.f32    q2, q12")
            __ASM_EMIT("vmul.f32    q3, q13")
            __ASM_EMIT("vmla.f32    q0, q4, q14")
            __ASM_EMIT("vmla.f32    q1, q5, q15")
            __ASM_EMIT("vmla.f32    q2, q6, q14")
            __ASM_EMIT("vmla.f32    q3, q7, q15")
            __ASM_EMIT("subs        %[count], $16")
            __ASM_EMIT("vstm        %[a]!, {q0-q3}")
            __ASM_EMIT("bhs         1b")
            /* 8x block */
            __ASM_EMIT("2:")
            __ASM_EMIT("adds        %[count], $8")
            __ASM_EMIT("blt         4f")
            __ASM_EMIT("vldm        %[a], {q0-q1}")
            __ASM_EMIT("vldm        %[b]!, {q4-q5}")
            __ASM_EMIT("vmul.f32    q0, q12")
            __ASM_EMIT("vmul.f32    q1, q13")
            __ASM_EMIT("vmla.f32    q0, q4, q14")
            __ASM_EMIT("vmla.f32    q1, q5, q15")
            __ASM_EMIT("sub         %[count], $8")
            __ASM_EMIT("vstm        %[a]!, {q0-q1}")
            /* 4x block */
            __ASM_EMIT("4:")
            __ASM_EMIT("adds        %[count], $4")
            __ASM_EMIT("blt         6f")
            __ASM_EMIT("vldm        %[a], {q0}")
            __ASM_EMIT("vldm        %[b]!, {q4}")
            __ASM_EMIT("vmul.f32    q0, q12")
            __ASM_EMIT("vmla.f32    q0, q4, q14")
            __ASM_EMIT("sub         %[count], $4")
            __ASM_EMIT("vstm        %[a]!, {q0}")
            /* 1x blocks */
            __ASM_EMIT("6:")
            __ASM_EMIT("adds        %[count], $3")
            __ASM_EMIT("blt         8f")
            __ASM_EMIT("7:")
            __ASM_EMIT("vld1.32     {d0[], d1[]}, [%[a]]")
            __ASM_EMIT("vld1.32     {d8[], d9[]}, [%[b]]!")
            __ASM_EMIT("vmul.f32    q0, q12")
            __ASM_EMIT("vmla.f32    q0, q4, q14")
            __ASM_EMIT("subs        %[count], $1")
            __ASM_EMIT("vst1.32     {d0[0]}, [%[a]]!")
            __ASM_EMIT("bge         7b")
            /* End */
            __ASM_EMIT("8:")
            : [a] "+r" (a), [b] "+r" (b),
              [count] "+r" (count),
              [k1] "+t" (k1), [k2] "+t" (k2)
            :
            : "cc", "memory",
              "q2", "q3",
              "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11",
              "q12", "q13", "q14", "q15"
        );
    }

    void mix_copy2(float *dst, const float *a, const float *b, float k1, float k2, size_t count)
    {
        ARCH_ARM_ASM(
            __ASM_EMIT("vdup.32     q13, %y[k1]")
            __ASM_EMIT("vdup.32     q15, %y[k2]")
            __ASM_EMIT("vmov        q14, q15")
            __ASM_EMIT("vmov        q12, q13")
            __ASM_EMIT("subs        %[count], $16")
            __ASM_EMIT("bls         2f")
            /* 16x blocks */
            __ASM_EMIT("1:")
            __ASM_EMIT("vldm        %[a]!, {q0-q3}")
            __ASM_EMIT("vldm        %[b]!, {q4-q7}")
            __ASM_EMIT("vmul.f32    q0, q12")
            __ASM_EMIT("vmul.f32    q1, q13")
            __ASM_EMIT("vmul.f32    q2, q12")
            __ASM_EMIT("vmul.f32    q3, q13")
            __ASM_EMIT("vmla.f32    q0, q4, q14")
            __ASM_EMIT("vmla.f32    q1, q5, q15")
            __ASM_EMIT("vmla.f32    q2, q6, q14")
            __ASM_EMIT("vmla.f32    q3, q7, q15")
            __ASM_EMIT("subs        %[count], $16")
            __ASM_EMIT("vstm        %[dst]!, {q0-q3}")
            __ASM_EMIT("bhs         1b")
            /* 8x block */
            __ASM_EMIT("2:")
            __ASM_EMIT("adds        %[count], $8")
            __ASM_EMIT("blt         4f")
            __ASM_EMIT("vldm        %[a]!, {q0-q1}")
            __ASM_EMIT("vldm        %[b]!, {q4-q5}")
            __ASM_EMIT("vmul.f32    q0, q12")
            __ASM_EMIT("vmul.f32    q1, q13")
            __ASM_EMIT("vmla.f32    q0, q4, q14")
            __ASM_EMIT("vmla.f32    q1, q5, q15")
            __ASM_EMIT("sub         %[count], $8")
            __ASM_EMIT("vstm        %[dst]!, {q0-q1}")
            /* 4x block */
            __ASM_EMIT("4:")
            __ASM_EMIT("adds        %[count], $4")
            __ASM_EMIT("blt         6f")
            __ASM_EMIT("vldm        %[a]!, {q0}")
            __ASM_EMIT("vldm        %[b]!, {q4}")
            __ASM_EMIT("vmul.f32    q0, q12")
            __ASM_EMIT("vmla.f32    q0, q4, q14")
            __ASM_EMIT("sub         %[count], $4")
            __ASM_EMIT("vstm        %[dst]!, {q0}")
            /* 1x blocks */
            __ASM_EMIT("6:")
            __ASM_EMIT("adds        %[count], $3")
            __ASM_EMIT("blt         8f")
            __ASM_EMIT("7:")
            __ASM_EMIT("vld1.32     {d0[], d1[]}, [%[a]]!")
            __ASM_EMIT("vld1.32     {d8[], d9[]}, [%[b]]!")
            __ASM_EMIT("vmul.f32    q0, q12")
            __ASM_EMIT("vmla.f32    q0, q4, q14")
            __ASM_EMIT("subs        %[count], $1")
            __ASM_EMIT("vst1.32     {d0[0]}, [%[dst]]!")
            __ASM_EMIT("bge         7b")
            /* end */
            __ASM_EMIT("8:")
            : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b),
              [count] "+r" (count),
              [k1] "+t" (k1), [k2] "+t" (k2)
            :
            : "cc", "memory",
              "q2", "q3",
              "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11",
              "q12", "q13", "q14", "q15"
        );
    }

    void mix_add2(float *dst, const float *a, const float *b, float k1, float k2, size_t count)
    {
        ARCH_ARM_ASM(
            __ASM_EMIT("vdup.32     q13, %y[k1]")
            __ASM_EMIT("vdup.32     q15, %y[k2]")
            __ASM_EMIT("vmov        q14, q15")
            __ASM_EMIT("vmov        q12, q13")
            __ASM_EMIT("subs        %[count], $16")
            __ASM_EMIT("bls         2f")
            /* 16x blocks */
            __ASM_EMIT("1:")
            __ASM_EMIT("vldm        %[dst], {q8-q11}")
            __ASM_EMIT("vldm        %[a]!, {q0-q3}")
            __ASM_EMIT("vldm        %[b]!, {q4-q7}")
            __ASM_EMIT("vmla.f32    q8, q0, q12")
            __ASM_EMIT("vmla.f32    q9, q1, q13")
            __ASM_EMIT("vmla.f32    q10, q2, q12")
            __ASM_EMIT("vmla.f32    q11, q3, q13")
            __ASM_EMIT("vmla.f32    q8, q4, q14")
            __ASM_EMIT("vmla.f32    q9, q5, q15")
            __ASM_EMIT("vmla.f32    q10, q6, q14")
            __ASM_EMIT("vmla.f32    q11, q7, q15")
            __ASM_EMIT("subs        %[count], $16")
            __ASM_EMIT("vstm        %[dst]!, {q8-q11}")
            __ASM_EMIT("bhs         1b")
            /* 8x block */
            __ASM_EMIT("2:")
            __ASM_EMIT("adds        %[count], $8")
            __ASM_EMIT("blt         4f")
            __ASM_EMIT("vldm        %[dst], {q8-q9}")
            __ASM_EMIT("vldm        %[a]!, {q0-q1}")
            __ASM_EMIT("vldm        %[b]!, {q4-q5}")
            __ASM_EMIT("vmla.f32    q8, q0, q12")
            __ASM_EMIT("vmla.f32    q9, q1, q13")
            __ASM_EMIT("vmla.f32    q8, q4, q14")
            __ASM_EMIT("vmla.f32    q9, q5, q15")
            __ASM_EMIT("sub         %[count], $8")
            __ASM_EMIT("vstm        %[dst]!, {q8-q9}")
            /* 4x block */
            __ASM_EMIT("4:")
            __ASM_EMIT("adds        %[count], $4")
            __ASM_EMIT("blt         6f")
            __ASM_EMIT("vldm        %[dst], {q8}")
            __ASM_EMIT("vldm        %[a]!, {q0}")
            __ASM_EMIT("vldm        %[b]!, {q4}")
            __ASM_EMIT("vmla.f32    q8, q0, q12")
            __ASM_EMIT("vmla.f32    q8, q4, q14")
            __ASM_EMIT("sub         %[count], $4")
            __ASM_EMIT("vstm        %[dst]!, {q8}")
            /* 1x blocks */
            __ASM_EMIT("6:")
            __ASM_EMIT("adds        %[count], $3")
            __ASM_EMIT("blt         8f")
            __ASM_EMIT("7:")
            __ASM_EMIT("vld1.32     {d16[], d17[]}, [%[dst]]")
            __ASM_EMIT("vld1.32     {d0[], d1[]}, [%[a]]!")
            __ASM_EMIT("vld1.32     {d8[], d9[]}, [%[b]]!")
            __ASM_EMIT("vmla.f32    q8, q0, q12")
            __ASM_EMIT("vmla.f32    q8, q4, q14")
            __ASM_EMIT("subs        %[count], $1")
            __ASM_EMIT("vst1.32     {d16[0]}, [%[dst]]!")
            __ASM_EMIT("bge         7b")
            /* end */
            __ASM_EMIT("8:")
            : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b),
              [count] "+r" (count),
              [k1] "+t" (k1), [k2] "+t" (k2)
            :
            : "cc", "memory",
              "q2", "q3",
              "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11",
              "q12", "q13", "q14", "q15"
        );
    }

    void mix3(float *a, const float *b, const float *c, float k1, float k2, float k3, size_t count)
    {
        ARCH_ARM_ASM(
            __ASM_EMIT("vdup.32     q13, %y[k1]")
            __ASM_EMIT("vdup.32     q14, %y[k2]")
            __ASM_EMIT("vdup.32     q15, %y[k3]")
            __ASM_EMIT("subs        %[count], $16")
            __ASM_EMIT("bls         2f")
            /* 16x blocks */
            __ASM_EMIT("1:")
            __ASM_EMIT("vldm        %[a], {q0-q3}")
            __ASM_EMIT("vldm        %[b]!, {q4-q7}")
            __ASM_EMIT("vldm        %[c]!, {q8-q11}")
            __ASM_EMIT("vmul.f32    q0, q0, q13")
            __ASM_EMIT("vmul.f32    q1, q1, q13")
            __ASM_EMIT("vmul.f32    q2, q2, q13")
            __ASM_EMIT("vmul.f32    q3, q3, q13")
            __ASM_EMIT("vmla.f32    q0, q4, q14")
            __ASM_EMIT("vmla.f32    q1, q5, q14")
            __ASM_EMIT("vmla.f32    q2, q6, q14")
            __ASM_EMIT("vmla.f32    q3, q7, q14")
            __ASM_EMIT("vmla.f32    q0, q8, q15")
            __ASM_EMIT("vmla.f32    q1, q9, q15")
            __ASM_EMIT("vmla.f32    q2, q10, q15")
            __ASM_EMIT("vmla.f32    q3, q11, q15")
            __ASM_EMIT("subs        %[count], $16")
            __ASM_EMIT("vstm        %[a]!, {q0-q3}")
            __ASM_EMIT("bhs         1b")
            /* 8x block */
            __ASM_EMIT("2:")
            __ASM_EMIT("adds        %[count], $8")
            __ASM_EMIT("blt         4f")
            __ASM_EMIT("vldm        %[a], {q0-q1}")
            __ASM_EMIT("vldm        %[b]!, {q4-q5}")
            __ASM_EMIT("vldm        %[c]!, {q8-q9}")
            __ASM_EMIT("vmul.f32    q0, q0, q13")
            __ASM_EMIT("vmul.f32    q1, q1, q13")
            __ASM_EMIT("vmla.f32    q0, q4, q14")
            __ASM_EMIT("vmla.f32    q1, q5, q14")
            __ASM_EMIT("vmla.f32    q0, q8, q15")
            __ASM_EMIT("vmla.f32    q1, q9, q15")
            __ASM_EMIT("sub         %[count], $8")
            __ASM_EMIT("vstm        %[a]!, {q0-q1}")
            /* 4x block */
            __ASM_EMIT("4:")
            __ASM_EMIT("adds        %[count], $4")
            __ASM_EMIT("blt         6f")
            __ASM_EMIT("vldm        %[a], {q0}")
            __ASM_EMIT("vldm        %[b]!, {q4}")
            __ASM_EMIT("vldm        %[c]!, {q8}")
            __ASM_EMIT("vmul.f32    q0, q0, q13")
            __ASM_EMIT("vmla.f32    q0, q4, q14")
            __ASM_EMIT("vmla.f32    q0, q8, q15")
            __ASM_EMIT("sub         %[count], $4")
            __ASM_EMIT("vstm        %[a]!, {q0}")
            /* 1x blocks */
            __ASM_EMIT("6:")
            __ASM_EMIT("adds        %[count], $3")
            __ASM_EMIT("blt         8f")
            __ASM_EMIT("7:")
            __ASM_EMIT("vld1.32     {d0[], d1[]}, [%[a]]")
            __ASM_EMIT("vld1.32     {d8[], d9[]}, [%[b]]!")
            __ASM_EMIT("vld1.32     {d16[], d17[]}, [%[c]]!")
            __ASM_EMIT("vmul.f32    q0, q0, q13")
            __ASM_EMIT("vmla.f32    q0, q4, q14")
            __ASM_EMIT("vmla.f32    q0, q8, q15")
            __ASM_EMIT("subs        %[count], $1")
            __ASM_EMIT("vst1.32     {d0[0]}, [%[a]]!")
            __ASM_EMIT("bge         7b")
            /* end */
            __ASM_EMIT("8:")
            : [a] "+r" (a), [b] "+r" (b), [c] "+r" (c),
              [count] "+r" (count),
              [k1] "+t" (k1), [k2] "+t" (k2), [k3] "+t" (k3)
            :
            : "cc", "memory",
              "q3",
              "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11",
              "q12", "q13", "q14", "q15"
        );
    }

    void mix_copy3(float *dst, const float *a, const float *b, const float *c, float k1, float k2, float k3, size_t count)
    {
        ARCH_ARM_ASM(
            __ASM_EMIT("vdup.32     q13, %y[k1]")
            __ASM_EMIT("vdup.32     q14, %y[k2]")
            __ASM_EMIT("vdup.32     q15, %y[k3]")
            __ASM_EMIT("subs        %[count], $16")
            __ASM_EMIT("bls         2f")
            /* 16x blocks */
            __ASM_EMIT("1:")
            __ASM_EMIT("vldm        %[a]!, {q0-q3}")
            __ASM_EMIT("vldm        %[b]!, {q4-q7}")
            __ASM_EMIT("vldm        %[c]!, {q8-q11}")
            __ASM_EMIT("vmul.f32    q0, q0, q13")
            __ASM_EMIT("vmul.f32    q1, q1, q13")
            __ASM_EMIT("vmul.f32    q2, q2, q13")
            __ASM_EMIT("vmul.f32    q3, q3, q13")
            __ASM_EMIT("vmla.f32    q0, q4, q14")
            __ASM_EMIT("vmla.f32    q1, q5, q14")
            __ASM_EMIT("vmla.f32    q2, q6, q14")
            __ASM_EMIT("vmla.f32    q3, q7, q14")
            __ASM_EMIT("vmla.f32    q0, q8, q15")
            __ASM_EMIT("vmla.f32    q1, q9, q15")
            __ASM_EMIT("vmla.f32    q2, q10, q15")
            __ASM_EMIT("vmla.f32    q3, q11, q15")
            __ASM_EMIT("subs        %[count], $16")
            __ASM_EMIT("vstm        %[dst]!, {q0-q3}")
            __ASM_EMIT("bhs         1b")
            /* 8x block */
            __ASM_EMIT("2:")
            __ASM_EMIT("adds        %[count], $8")
            __ASM_EMIT("blt         4f")
            __ASM_EMIT("vldm        %[a]!, {q0-q1}")
            __ASM_EMIT("vldm        %[b]!, {q4-q5}")
            __ASM_EMIT("vldm        %[c]!, {q8-q9}")
            __ASM_EMIT("vmul.f32    q0, q0, q13")
            __ASM_EMIT("vmul.f32    q1, q1, q13")
            __ASM_EMIT("vmla.f32    q0, q4, q14")
            __ASM_EMIT("vmla.f32    q1, q5, q14")
            __ASM_EMIT("vmla.f32    q0, q8, q15")
            __ASM_EMIT("vmla.f32    q1, q9, q15")
            __ASM_EMIT("sub         %[count], $8")
            __ASM_EMIT("vstm        %[dst]!, {q0-q1}")
            /* 4x block */
            __ASM_EMIT("4:")
            __ASM_EMIT("adds        %[count], $4")
            __ASM_EMIT("blt         6f")
            __ASM_EMIT("vldm        %[a]!, {q0}")
            __ASM_EMIT("vldm        %[b]!, {q4}")
            __ASM_EMIT("vldm        %[c]!, {q8}")
            __ASM_EMIT("vmul.f32    q0, q0, q13")
            __ASM_EMIT("vmla.f32    q0, q4, q14")
            __ASM_EMIT("vmla.f32    q0, q8, q15")
            __ASM_EMIT("sub         %[count], $4")
            __ASM_EMIT("vstm        %[dst]!, {q0}")
            /* 1x blocks */
            __ASM_EMIT("6:")
            __ASM_EMIT("adds        %[count], $3")
            __ASM_EMIT("blt         8f")
            __ASM_EMIT("7:")
            __ASM_EMIT("vld1.32     {d0[], d1[]}, [%[a]]!")
            __ASM_EMIT("vld1.32     {d8[], d9[]}, [%[b]]!")
            __ASM_EMIT("vld1.32     {d16[], d17[]}, [%[c]]!")
            __ASM_EMIT("vmul.f32    q0, q0, q13")
            __ASM_EMIT("vmla.f32    q0, q4, q14")
            __ASM_EMIT("vmla.f32    q0, q8, q15")
            __ASM_EMIT("subs        %[count], $1")
            __ASM_EMIT("vst1.32     {d0[0]}, [%[dst]]!")
            __ASM_EMIT("bge         7b")
            /* end */
            __ASM_EMIT("8:")
            : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b), [c] "+r" (c),
              [count] "+r" (count),
              [k1] "+t" (k1), [k2] "+t" (k2), [k3] "+t" (k3)
            :
            : "cc", "memory",
              "q3",
              "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11",
              "q12", "q13", "q14", "q15"
        );
    }

    void mix_add3(float *dst, const float *a, const float *b, const float *c, float k1, float k2, float k3, size_t count)
    {
        ARCH_ARM_ASM(
            __ASM_EMIT("vdup.32     q13, %y[k1]")
            __ASM_EMIT("vdup.32     q14, %y[k2]")
            __ASM_EMIT("vdup.32     q15, %y[k3]")
            __ASM_EMIT("subs        %[count], $16")
            __ASM_EMIT("bls         2f")
            /* 16x blocks */
            __ASM_EMIT("1:")
            __ASM_EMIT("vldm        %[dst], {q0-q3}")
            __ASM_EMIT("vldm        %[a]!, {q4-q7}")
            __ASM_EMIT("vldm        %[b]!, {q8-q11}")
            __ASM_EMIT("vmla.f32    q0, q4, q13")
            __ASM_EMIT("vmla.f32    q1, q5, q13")
            __ASM_EMIT("vmla.f32    q2, q6, q13")
            __ASM_EMIT("vmla.f32    q3, q7, q13")
            __ASM_EMIT("vmla.f32    q0, q8, q14")
            __ASM_EMIT("vmla.f32    q1, q9, q14")
            __ASM_EMIT("vldm        %[c]!, {q4-q7}")
            __ASM_EMIT("vmla.f32    q2, q10, q14")
            __ASM_EMIT("vmla.f32    q3, q11, q14")
            __ASM_EMIT("vmla.f32    q0, q4, q15")
            __ASM_EMIT("vmla.f32    q1, q5, q15")
            __ASM_EMIT("vmla.f32    q2, q6, q15")
            __ASM_EMIT("vmla.f32    q3, q7, q15")
            __ASM_EMIT("subs        %[count], $16")
            __ASM_EMIT("vstm        %[dst]!, {q0-q3}")
            __ASM_EMIT("bhs         1b")
            /* 8x block */
            __ASM_EMIT("2:")
            __ASM_EMIT("adds        %[count], $8")
            __ASM_EMIT("blt         4f")
            __ASM_EMIT("vldm        %[dst], {q0-q1}")
            __ASM_EMIT("vldm        %[a]!, {q2-q3}")
            __ASM_EMIT("vldm        %[b]!, {q4-q5}")
            __ASM_EMIT("vldm        %[c]!, {q6-q7}")
            __ASM_EMIT("vmla.f32    q0, q2, q13")
            __ASM_EMIT("vmla.f32    q1, q3, q13")
            __ASM_EMIT("vmla.f32    q0, q4, q14")
            __ASM_EMIT("vmla.f32    q1, q5, q14")
            __ASM_EMIT("vmla.f32    q0, q6, q15")
            __ASM_EMIT("vmla.f32    q1, q7, q15")
            __ASM_EMIT("sub         %[count], $8")
            __ASM_EMIT("vstm        %[dst]!, {q0-q1}")
            /* 4x block */
            __ASM_EMIT("4:")
            __ASM_EMIT("adds        %[count], $4")
            __ASM_EMIT("blt         6f")
            __ASM_EMIT("vldm        %[dst], {q0}")
            __ASM_EMIT("vldm        %[a]!, {q2}")
            __ASM_EMIT("vldm        %[b]!, {q4}")
            __ASM_EMIT("vldm        %[c]!, {q6}")
            __ASM_EMIT("vmla.f32    q0, q2, q13")
            __ASM_EMIT("vmla.f32    q0, q4, q14")
            __ASM_EMIT("vmla.f32    q0, q6, q15")
            __ASM_EMIT("sub         %[count], $4")
            __ASM_EMIT("vstm        %[dst]!, {q0}")
            /* 1x blocks */
            __ASM_EMIT("6:")
            __ASM_EMIT("adds        %[count], $3")
            __ASM_EMIT("blt         8f")
            __ASM_EMIT("7:")
            __ASM_EMIT("vld1.32     {d0[], d1[]}, [%[dst]]")
            __ASM_EMIT("vld1.32     {d2[], d3[]}, [%[a]]!")
            __ASM_EMIT("vld1.32     {d4[], d5[]}, [%[b]]!")
            __ASM_EMIT("vld1.32     {d6[], d7[]}, [%[c]]!")
            __ASM_EMIT("vmla.f32    q0, q1, q13")
            __ASM_EMIT("vmla.f32    q0, q2, q14")
            __ASM_EMIT("vmla.f32    q0, q3, q15")
            __ASM_EMIT("subs        %[count], $1")
            __ASM_EMIT("vst1.32     {d0[0]}, [%[dst]]!")
            __ASM_EMIT("bge         7b")
            /* end */
            __ASM_EMIT("8:")
            : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b), [c] "+r" (c),
              [count] "+r" (count),
              [k1] "+t" (k1), [k2] "+t" (k2), [k3] "+t" (k3)
            :
            : "cc", "memory",
              "q3", "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    void mix4(float *a, const float *b, const float *c, const float *d, float k1, float k2, float k3, float k4, size_t count)
    {
        ARCH_ARM_ASM(
            __ASM_EMIT("vdup.32     q12, %y[k1]")
            __ASM_EMIT("vdup.32     q13, %y[k2]")
            __ASM_EMIT("vdup.32     q14, %y[k3]")
            __ASM_EMIT("vdup.32     q15, %y[k4]")
            __ASM_EMIT("subs        %[count], $16")
            __ASM_EMIT("bls         2f")
            /* 16x blocks */
            __ASM_EMIT("1:")
            __ASM_EMIT("vldm        %[a], {q0-q3}")
            __ASM_EMIT("vldm        %[b]!, {q4-q7}")
            __ASM_EMIT("vldm        %[c]!, {q8-q11}")
            __ASM_EMIT("vmul.f32    q0, q0, q12")
            __ASM_EMIT("vmul.f32    q1, q1, q12")
            __ASM_EMIT("vmul.f32    q2, q2, q12")
            __ASM_EMIT("vmul.f32    q3, q3, q12")
            __ASM_EMIT("vmla.f32    q0, q4, q13")
            __ASM_EMIT("vmla.f32    q1, q5, q13")
            __ASM_EMIT("vmla.f32    q2, q6, q13")
            __ASM_EMIT("vmla.f32    q3, q7, q13")
            __ASM_EMIT("vmla.f32    q0, q8, q14")
            __ASM_EMIT("vmla.f32    q1, q9, q14")
            __ASM_EMIT("vldm        %[d]!, {q4-q7}")
            __ASM_EMIT("vmla.f32    q2, q10, q14")
            __ASM_EMIT("vmla.f32    q3, q11, q14")
            __ASM_EMIT("vmla.f32    q0, q4, q15")
            __ASM_EMIT("vmla.f32    q1, q5, q15")
            __ASM_EMIT("vmla.f32    q2, q6, q15")
            __ASM_EMIT("vmla.f32    q3, q7, q15")
            __ASM_EMIT("subs        %[count], $16")
            __ASM_EMIT("vstm        %[a]!, {q0-q3}")
            __ASM_EMIT("bhs         1b")
            /* 8x block */
            __ASM_EMIT("2:")
            __ASM_EMIT("adds        %[count], $8")
            __ASM_EMIT("blt         4f")
            __ASM_EMIT("vldm        %[a], {q0-q1}")
            __ASM_EMIT("vldm        %[b]!, {q2-q3}")
            __ASM_EMIT("vldm        %[c]!, {q4-q5}")
            __ASM_EMIT("vldm        %[d]!, {q6-q7}")
            __ASM_EMIT("vmul.f32    q0, q0, q12")
            __ASM_EMIT("vmul.f32    q1, q1, q12")
            __ASM_EMIT("vmla.f32    q0, q2, q13")
            __ASM_EMIT("vmla.f32    q1, q3, q13")
            __ASM_EMIT("vmla.f32    q0, q4, q14")
            __ASM_EMIT("vmla.f32    q1, q5, q14")
            __ASM_EMIT("vmla.f32    q0, q6, q15")
            __ASM_EMIT("vmla.f32    q1, q7, q15")
            __ASM_EMIT("sub         %[count], $8")
            __ASM_EMIT("vstm        %[a]!, {q0-q1}")
            /* 4x block */
            __ASM_EMIT("4:")
            __ASM_EMIT("adds        %[count], $4")
            __ASM_EMIT("blt         6f")
            __ASM_EMIT("vldm        %[a], {q0}")
            __ASM_EMIT("vldm        %[b]!, {q2}")
            __ASM_EMIT("vldm        %[c]!, {q4}")
            __ASM_EMIT("vldm        %[d]!, {q6}")
            __ASM_EMIT("vmul.f32    q0, q0, q12")
            __ASM_EMIT("vmla.f32    q0, q2, q13")
            __ASM_EMIT("vmla.f32    q0, q4, q14")
            __ASM_EMIT("vmla.f32    q0, q6, q15")
            __ASM_EMIT("sub         %[count], $4")
            __ASM_EMIT("vstm        %[a]!, {q0}")
            /* 1x blocks */
            __ASM_EMIT("6:")
            __ASM_EMIT("adds        %[count], $3")
            __ASM_EMIT("blt         8f")
            __ASM_EMIT("7:")
            __ASM_EMIT("vld1.32     {d0[], d1[]}, [%[a]]")
            __ASM_EMIT("vld1.32     {d4[], d5[]}, [%[b]]!")
            __ASM_EMIT("vld1.32     {d8[], d9[]}, [%[c]]!")
            __ASM_EMIT("vld1.32     {d12[], d13[]}, [%[d]]!")
            __ASM_EMIT("vmul.f32    q0, q0, q12")
            __ASM_EMIT("vmla.f32    q0, q2, q13")
            __ASM_EMIT("vmla.f32    q0, q4, q14")
            __ASM_EMIT("vmla.f32    q0, q6, q15")
            __ASM_EMIT("subs        %[count], $1")
            __ASM_EMIT("vst1.32     {d0[0]}, [%[a]]!")
            __ASM_EMIT("bge         7b")
            /* end */
            __ASM_EMIT("8:")
            : [a] "+r" (a), [b] "+r" (b), [c] "+r" (c), [d] "+r" (d),
              [count] "+r" (count),
              [k1] "+t" (k1), [k2] "+t" (k2), [k3] "+t" (k3), [k4] "+t" (k4)
            :
            : "cc", "memory",
              "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11",
              "q12", "q13", "q14", "q15"
        );
    }

    void mix_copy4(float *dst, const float *a, const float *b, const float *c, const float *d,
            float k1, float k2, float k3, float k4, size_t count)
    {
        ARCH_ARM_ASM(
            __ASM_EMIT("vdup.32     q12, %y[k1]")
            __ASM_EMIT("vdup.32     q13, %y[k2]")
            __ASM_EMIT("vdup.32     q14, %y[k3]")
            __ASM_EMIT("vdup.32     q15, %y[k4]")
            __ASM_EMIT("subs        %[count], $16")
            __ASM_EMIT("bls         2f")
            /* 16x blocks */
            __ASM_EMIT("1:")
            __ASM_EMIT("vldm        %[a]!, {q0-q3}")
            __ASM_EMIT("vldm        %[b]!, {q4-q7}")
            __ASM_EMIT("vldm        %[c]!, {q8-q11}")
            __ASM_EMIT("vmul.f32    q0, q0, q12")
            __ASM_EMIT("vmul.f32    q1, q1, q12")
            __ASM_EMIT("vmul.f32    q2, q2, q12")
            __ASM_EMIT("vmul.f32    q3, q3, q12")
            __ASM_EMIT("vmla.f32    q0, q4, q13")
            __ASM_EMIT("vmla.f32    q1, q5, q13")
            __ASM_EMIT("vmla.f32    q2, q6, q13")
            __ASM_EMIT("vmla.f32    q3, q7, q13")
            __ASM_EMIT("vmla.f32    q0, q8, q14")
            __ASM_EMIT("vmla.f32    q1, q9, q14")
            __ASM_EMIT("vldm        %[d]!, {q4-q7}")
            __ASM_EMIT("vmla.f32    q2, q10, q14")
            __ASM_EMIT("vmla.f32    q3, q11, q14")
            __ASM_EMIT("vmla.f32    q0, q4, q15")
            __ASM_EMIT("vmla.f32    q1, q5, q15")
            __ASM_EMIT("vmla.f32    q2, q6, q15")
            __ASM_EMIT("vmla.f32    q3, q7, q15")
            __ASM_EMIT("subs        %[count], $16")
            __ASM_EMIT("vstm        %[dst]!, {q0-q3}")
            __ASM_EMIT("bhs         1b")
            /* 8x block */
            __ASM_EMIT("2:")
            __ASM_EMIT("adds        %[count], $8")
            __ASM_EMIT("blt         4f")
            __ASM_EMIT("vldm        %[a]!, {q0-q1}")
            __ASM_EMIT("vldm        %[b]!, {q2-q3}")
            __ASM_EMIT("vldm        %[c]!, {q4-q5}")
            __ASM_EMIT("vldm        %[d]!, {q6-q7}")
            __ASM_EMIT("vmul.f32    q0, q0, q12")
            __ASM_EMIT("vmul.f32    q1, q1, q12")
            __ASM_EMIT("vmla.f32    q0, q2, q13")
            __ASM_EMIT("vmla.f32    q1, q3, q13")
            __ASM_EMIT("vmla.f32    q0, q4, q14")
            __ASM_EMIT("vmla.f32    q1, q5, q14")
            __ASM_EMIT("vmla.f32    q0, q6, q15")
            __ASM_EMIT("vmla.f32    q1, q7, q15")
            __ASM_EMIT("sub         %[count], $8")
            __ASM_EMIT("vstm        %[dst]!, {q0-q1}")
            /* 4x block */
            __ASM_EMIT("4:")
            __ASM_EMIT("adds        %[count], $4")
            __ASM_EMIT("blt         6f")
            __ASM_EMIT("vldm        %[a]!, {q0}")
            __ASM_EMIT("vldm        %[b]!, {q2}")
            __ASM_EMIT("vldm        %[c]!, {q4}")
            __ASM_EMIT("vldm        %[d]!, {q6}")
            __ASM_EMIT("vmul.f32    q0, q0, q12")
            __ASM_EMIT("vmla.f32    q0, q2, q13")
            __ASM_EMIT("vmla.f32    q0, q4, q14")
            __ASM_EMIT("vmla.f32    q0, q6, q15")
            __ASM_EMIT("sub         %[count], $4")
            __ASM_EMIT("vstm        %[dst]!, {q0}")
            /* 1x blocks */
            __ASM_EMIT("6:")
            __ASM_EMIT("adds        %[count], $3")
            __ASM_EMIT("blt         8f")
            __ASM_EMIT("7:")
            __ASM_EMIT("vld1.32     {d0[], d1[]}, [%[a]]!")
            __ASM_EMIT("vld1.32     {d4[], d5[]}, [%[b]]!")
            __ASM_EMIT("vld1.32     {d8[], d9[]}, [%[c]]!")
            __ASM_EMIT("vld1.32     {d12[], d13[]}, [%[d]]!")
            __ASM_EMIT("vmul.f32    q0, q0, q12")
            __ASM_EMIT("vmla.f32    q0, q2, q13")
            __ASM_EMIT("vmla.f32    q0, q4, q14")
            __ASM_EMIT("vmla.f32    q0, q6, q15")
            __ASM_EMIT("subs        %[count], $1")
            __ASM_EMIT("vst1.32     {d0[0]}, [%[dst]]!")
            __ASM_EMIT("bge         7b")
            /* end */
            __ASM_EMIT("8:")
            : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b), [c] "+r" (c), [d] "+r" (d),
              [count] "+r" (count),
              [k1] "+t" (k1), [k2] "+t" (k2), [k3] "+t" (k3), [k4] "+t" (k4)
            :
            : "cc", "memory",
              "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11",
              "q12", "q13", "q14", "q15"
        );
    }

    void mix_add4(float *dst, const float *a, const float *b, const float *c, const float *d,
            float k1, float k2, float k3, float k4, size_t count)
    {
        ARCH_ARM_ASM(
            __ASM_EMIT("vdup.32     q12, %y[k1]")
            __ASM_EMIT("vdup.32     q13, %y[k2]")
            __ASM_EMIT("vdup.32     q14, %y[k3]")
            __ASM_EMIT("vdup.32     q15, %y[k4]")
            __ASM_EMIT("subs        %[count], $16")
            __ASM_EMIT("bls         2f")
            /* 16x blocks */
            __ASM_EMIT("1:")
            __ASM_EMIT("vldm        %[dst], {q0-q3}")
            __ASM_EMIT("vldm        %[a]!, {q4-q7}")
            __ASM_EMIT("vldm        %[b]!, {q8-q11}")
            __ASM_EMIT("vmla.f32    q0, q4, q12")
            __ASM_EMIT("vmla.f32    q1, q5, q12")
            __ASM_EMIT("vmla.f32    q2, q6, q12")
            __ASM_EMIT("vmla.f32    q3, q7, q12")
            __ASM_EMIT("vmla.f32    q0, q8, q13")
            __ASM_EMIT("vmla.f32    q1, q9, q13")
            __ASM_EMIT("vldm        %[c]!, {q4-q7}")
            __ASM_EMIT("vmla.f32    q2, q10, q13")
            __ASM_EMIT("vmla.f32    q3, q11, q13")
            __ASM_EMIT("vmla.f32    q0, q4, q14")
            __ASM_EMIT("vmla.f32    q1, q5, q14")
            __ASM_EMIT("vldm        %[d]!, {q8-q11}")
            __ASM_EMIT("vmla.f32    q2, q6, q14")
            __ASM_EMIT("vmla.f32    q3, q7, q14")
            __ASM_EMIT("vmla.f32    q0, q8, q15")
            __ASM_EMIT("vmla.f32    q1, q9, q15")
            __ASM_EMIT("vmla.f32    q2, q10, q15")
            __ASM_EMIT("vmla.f32    q3, q11, q15")
            __ASM_EMIT("subs        %[count], $16")
            __ASM_EMIT("vstm        %[dst]!, {q0-q3}")
            __ASM_EMIT("bhs         1b")
            /* 8x block */
            __ASM_EMIT("2:")
            __ASM_EMIT("adds        %[count], $8")
            __ASM_EMIT("blt         4f")
            __ASM_EMIT("vldm        %[dst], {q0-q1}")
            __ASM_EMIT("vldm        %[a]!, {q2-q3}")
            __ASM_EMIT("vldm        %[b]!, {q4-q5}")
            __ASM_EMIT("vldm        %[c]!, {q6-q7}")
            __ASM_EMIT("vldm        %[d]!, {q8-q9}")
            __ASM_EMIT("vmla.f32    q0, q2, q12")
            __ASM_EMIT("vmla.f32    q1, q3, q12")
            __ASM_EMIT("vmla.f32    q0, q4, q13")
            __ASM_EMIT("vmla.f32    q1, q5, q13")
            __ASM_EMIT("vmla.f32    q0, q6, q14")
            __ASM_EMIT("vmla.f32    q1, q7, q14")
            __ASM_EMIT("vmla.f32    q0, q8, q15")
            __ASM_EMIT("vmla.f32    q1, q9, q15")
            __ASM_EMIT("sub         %[count], $8")
            __ASM_EMIT("vstm        %[dst]!, {q0-q1}")
            /* 4x block */
            __ASM_EMIT("4:")
            __ASM_EMIT("adds        %[count], $4")
            __ASM_EMIT("blt         6f")
            __ASM_EMIT("vldm        %[dst], {q0}")
            __ASM_EMIT("vldm        %[a]!, {q2}")
            __ASM_EMIT("vldm        %[b]!, {q4}")
            __ASM_EMIT("vldm        %[c]!, {q6}")
            __ASM_EMIT("vldm        %[d]!, {q8}")
            __ASM_EMIT("vmla.f32    q0, q2, q12")
            __ASM_EMIT("vmla.f32    q0, q4, q13")
            __ASM_EMIT("vmla.f32    q0, q6, q14")
            __ASM_EMIT("vmla.f32    q0, q8, q15")
            __ASM_EMIT("sub         %[count], $4")
            __ASM_EMIT("vstm        %[dst]!, {q0}")
            /* 1x blocks */
            __ASM_EMIT("6:")
            __ASM_EMIT("adds        %[count], $3")
            __ASM_EMIT("blt         8f")
            __ASM_EMIT("7:")
            __ASM_EMIT("vld1.32     {d0[], d1[]}, [%[dst]]")
            __ASM_EMIT("vld1.32     {d4[], d5[]}, [%[a]]!")
            __ASM_EMIT("vld1.32     {d8[], d9[]}, [%[b]]!")
            __ASM_EMIT("vld1.32     {d12[], d13[]}, [%[c]]!")
            __ASM_EMIT("vld1.32     {d16[], d17[]}, [%[d]]!")
            __ASM_EMIT("vmla.f32    q0, q2, q12")
            __ASM_EMIT("vmla.f32    q0, q4, q13")
            __ASM_EMIT("vmla.f32    q0, q6, q14")
            __ASM_EMIT("vmla.f32    q0, q8, q15")
            __ASM_EMIT("subs        %[count], $1")
            __ASM_EMIT("vst1.32     {d0[0]}, [%[dst]]!")
            __ASM_EMIT("bge         7b")
            /* end */
            __ASM_EMIT("8:")
            : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b), [c] "+r" (c), [d] "+r" (d),
              [count] "+r" (count),
              [k1] "+t" (k1), [k2] "+t" (k2), [k3] "+t" (k3), [k4] "+t" (k4)
            :
            : "cc", "memory",
              "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11",
              "q12", "q13", "q14", "q15"
        );
    }

}

#endif /* DSP_ARCH_ARM_NEON_D32_MIX_H_ */
