/*
 * msmatrix.h
 *
 *  Created on: 3 окт. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_ARM_NEON_D32_MSMATRIX_H_
#define DSP_ARCH_ARM_NEON_D32_MSMATRIX_H_

#ifndef DSP_ARCH_ARM_NEON_32_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_ARM_NEON_32_IMPL */

namespace neon_d32
{
    void lr_to_ms(float *m, float *s, const float *l, const float *r, size_t count)
    {
        IF_ARCH_ARM(const float *half = X_HALF);

        ARCH_ARM_ASM
        (
            __ASM_EMIT("subs        %[count], $24")
            __ASM_EMIT("vld1.32     {q0-q1}, [%[X_HALF]]")      // q0 = 0.5, q1 = 0.5
            __ASM_EMIT("blo         2f")
            // x24 blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("vld1.32     {q2-q3}, [%[left]]!")       // q2 = l, q3 = l
            __ASM_EMIT("vld1.32     {q4-q5}, [%[right]]!")      // q4 = r, q5 = r
            __ASM_EMIT("vmov        q14, q2")                   // q14 = l
            __ASM_EMIT("vmov        q15, q3")                   // q15 = r
            __ASM_EMIT("vld1.32     {q6-q7}, [%[left]]!")
            __ASM_EMIT("vld1.32     {q8-q9}, [%[right]]!")
            __ASM_EMIT("vadd.f32    q2, q2, q4")                // q2 = l + r
            __ASM_EMIT("vadd.f32    q3, q3, q5")                // q3 = l + r
            __ASM_EMIT("vld1.32     {q10-q11}, [%[left]]!")
            __ASM_EMIT("vld1.32     {q12-q13}, [%[right]]!")
            __ASM_EMIT("vsub.f32    q4, q14, q4")               // q4 = l - r
            __ASM_EMIT("vsub.f32    q5, q15, q5")               // q5 = l - r
            __ASM_EMIT("vmul.f32    q2, q0")                    // q2 = (l + r) * 0.5f
            __ASM_EMIT("vmul.f32    q3, q1")                    // q3 = (l + r) * 0.5f
            __ASM_EMIT("vmul.f32    q4, q0")                    // q4 = (l - r) * 0.5f
            __ASM_EMIT("vmul.f32    q5, q1")                    // q5 = (l - r) * 0.5f
            __ASM_EMIT("vmov        q14, q6")
            __ASM_EMIT("vmov        q15, q7")
            __ASM_EMIT("vst1.32     {q2-q3}, [%[mid]]!")
            __ASM_EMIT("vst1.32     {q4-q5}, [%[side]]!")
            __ASM_EMIT("vadd.f32    q6, q6, q8")
            __ASM_EMIT("vadd.f32    q7, q7, q9")
            __ASM_EMIT("vsub.f32    q8, q14, q8")
            __ASM_EMIT("vsub.f32    q9, q15, q9")
            __ASM_EMIT("vmul.f32    q6, q0")
            __ASM_EMIT("vmul.f32    q7, q1")
            __ASM_EMIT("vmul.f32    q8, q0")
            __ASM_EMIT("vmul.f32    q9, q1")
            __ASM_EMIT("vmov        q14, q10")
            __ASM_EMIT("vmov        q15, q11")
            __ASM_EMIT("vst1.32     {q6-q7}, [%[mid]]!")
            __ASM_EMIT("vst1.32     {q8-q9}, [%[side]]!")
            __ASM_EMIT("vadd.f32    q10, q10, q12")
            __ASM_EMIT("vadd.f32    q11, q11, q13")
            __ASM_EMIT("vsub.f32    q12, q14, q12")
            __ASM_EMIT("vsub.f32    q13, q15, q13")
            __ASM_EMIT("vmul.f32    q10, q0")
            __ASM_EMIT("vmul.f32    q11, q1")
            __ASM_EMIT("vmul.f32    q12, q0")
            __ASM_EMIT("vmul.f32    q13, q1")
            __ASM_EMIT("vst1.32     {q10-q11}, [%[mid]]!")
            __ASM_EMIT("vst1.32     {q12-q13}, [%[side]]!")
            __ASM_EMIT("subs        %[count], $24")
            __ASM_EMIT("bhs         1b")
            // x16 block
            __ASM_EMIT("2:")
            __ASM_EMIT("adds        %[count], $8")              // 24 - 16
            __ASM_EMIT("blt         4f")
            __ASM_EMIT("vld1.32     {q2-q3}, [%[left]]!")       // q2 = l, q3 = l
            __ASM_EMIT("vld1.32     {q4-q5}, [%[right]]!")      // q4 = r, q5 = r
            __ASM_EMIT("vld1.32     {q6-q7}, [%[left]]!")
            __ASM_EMIT("vld1.32     {q8-q9}, [%[right]]!")
            __ASM_EMIT("vmov        q14, q2")                   // q14 = l
            __ASM_EMIT("vmov        q15, q3")                   // q15 = r
            __ASM_EMIT("vadd.f32    q2, q2, q4")                // q2 = l + r
            __ASM_EMIT("vadd.f32    q3, q3, q5")                // q3 = l + r
            __ASM_EMIT("vsub.f32    q4, q14, q4")               // q4 = l - r
            __ASM_EMIT("vsub.f32    q5, q15, q5")               // q5 = l - r
            __ASM_EMIT("vmul.f32    q2, q0")                    // q2 = (l + r) * 0.5f
            __ASM_EMIT("vmul.f32    q3, q1")                    // q3 = (l + r) * 0.5f
            __ASM_EMIT("vmul.f32    q4, q0")                    // q4 = (l - r) * 0.5f
            __ASM_EMIT("vmul.f32    q5, q1")                    // q5 = (l - r) * 0.5f
            __ASM_EMIT("vmov        q14, q6")
            __ASM_EMIT("vmov        q15, q7")
            __ASM_EMIT("vadd.f32    q6, q6, q8")
            __ASM_EMIT("vadd.f32    q7, q7, q9")
            __ASM_EMIT("vsub.f32    q8, q14, q8")
            __ASM_EMIT("vsub.f32    q9, q15, q9")
            __ASM_EMIT("vmul.f32    q6, q0")
            __ASM_EMIT("vmul.f32    q7, q1")
            __ASM_EMIT("vmul.f32    q8, q0")
            __ASM_EMIT("vmul.f32    q9, q1")
            __ASM_EMIT("vst1.32     {q2-q3}, [%[mid]]!")
            __ASM_EMIT("vst1.32     {q4-q5}, [%[side]]!")
            __ASM_EMIT("vst1.32     {q6-q7}, [%[mid]]!")
            __ASM_EMIT("vst1.32     {q8-q9}, [%[side]]!")
            __ASM_EMIT("sub         %[count], $16")
            // x8 block
            __ASM_EMIT("4:")
            __ASM_EMIT("adds        %[count], $8")              // 16 - 8
            __ASM_EMIT("blt         6f")
            __ASM_EMIT("vld1.32     {q2-q3}, [%[left]]!")       // q2 = l, q3 = l
            __ASM_EMIT("vld1.32     {q4-q5}, [%[right]]!")      // q4 = r, q5 = r
            __ASM_EMIT("vmov        q14, q2")                   // q14 = l
            __ASM_EMIT("vmov        q15, q3")                   // q15 = r
            __ASM_EMIT("vadd.f32    q2, q2, q4")                // q2 = l + r
            __ASM_EMIT("vadd.f32    q3, q3, q5")                // q3 = l + r
            __ASM_EMIT("vsub.f32    q4, q14, q4")               // q4 = l - r
            __ASM_EMIT("vsub.f32    q5, q15, q5")               // q5 = l - r
            __ASM_EMIT("vmul.f32    q2, q0")                    // q2 = (l + r) * 0.5f
            __ASM_EMIT("vmul.f32    q3, q1")                    // q3 = (l + r) * 0.5f
            __ASM_EMIT("vmul.f32    q4, q0")                    // q4 = (l - r) * 0.5f
            __ASM_EMIT("vmul.f32    q5, q1")                    // q5 = (l - r) * 0.5f
            __ASM_EMIT("vst1.32     {q2-q3}, [%[mid]]!")
            __ASM_EMIT("vst1.32     {q4-q5}, [%[side]]!")
            __ASM_EMIT("sub         %[count], $8")
            // x4 block
            __ASM_EMIT("6:")
            __ASM_EMIT("adds        %[count], $4")              // 8 - 4
            __ASM_EMIT("blt         8f")
            __ASM_EMIT("vld1.32     {q2}, [%[left]]!")          // q2 = l
            __ASM_EMIT("vld1.32     {q4}, [%[right]]!")         // q4 = r
            __ASM_EMIT("vmov        q14, q2")                   // q14 = l
            __ASM_EMIT("vadd.f32    q2, q2, q4")                // q2 = l + r
            __ASM_EMIT("vsub.f32    q4, q14, q4")               // q4 = l - r
            __ASM_EMIT("vmul.f32    q2, q0")                    // q2 = (l + r) * 0.5f
            __ASM_EMIT("vmul.f32    q4, q0")                    // q4 = (l - r) * 0.5f
            __ASM_EMIT("vst1.32     {q2}, [%[mid]]!")
            __ASM_EMIT("vst1.32     {q4}, [%[side]]!")
            __ASM_EMIT("sub         %[count], $4")
            // x1 blocks
            __ASM_EMIT("8:")
            __ASM_EMIT("adds        %[count], $3")              // 4 - 1
            __ASM_EMIT("blt         10f")
            __ASM_EMIT("9:")
            __ASM_EMIT("vldm        %[left]!, {s2}")            // s2 = l
            __ASM_EMIT("vldm        %[right]!, {s4}")           // s4 = r
            __ASM_EMIT("vmov        s14, s2")                   // s12 = l
            __ASM_EMIT("vadd.f32    s2, s2, s4")                // s0 = l + r
            __ASM_EMIT("vsub.f32    s4, s14, s4")               // s2 = l - r
            __ASM_EMIT("vmul.f32    s2, s0")                    // s0 = (l + r) * 0.5f
            __ASM_EMIT("vmul.f32    s4, s0")                    // s2 = (l - r) * 0.5f
            __ASM_EMIT("vstm        %[mid]!, {q2}")
            __ASM_EMIT("vstm        %[side]!, {q4}")
            __ASM_EMIT("subs        %[count], $1")
            __ASM_EMIT("bge         9b")
            __ASM_EMIT("10:")

            : [mid] "+r" (m), [side] "+r" (s),
              [left] "+r" (l), [right] "+r" (r),
              [count] "+r" (count)
            : [X_HALF] "r" (half)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    void ms_to_lr(float *l, float *r, const float *m, const float *s, size_t count)
    {
        ARCH_ARM_ASM
        (
            __ASM_EMIT("subs        %[count], $24")
            __ASM_EMIT("blo         2f")
            // x24 blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("vld1.32     {q0-q1}, [%[mid]]!")        // q0, q1 = m
            __ASM_EMIT("vld1.32     {q2-q3}, [%[side]]!")       // q2, q3 = s
            __ASM_EMIT("vmov        q12, q0")                   // q12 = m
            __ASM_EMIT("vmov        q13, q1")                   // q13 = m
            __ASM_EMIT("vld1.32     {q4-q5}, [%[mid]]!")
            __ASM_EMIT("vld1.32     {q6-q7}, [%[side]]!")
            __ASM_EMIT("vadd.f32    q0, q0, q2")                // q0 = m + s
            __ASM_EMIT("vadd.f32    q1, q1, q3")                // q1 = m + s
            __ASM_EMIT("vld1.32     {q8-q9}, [%[mid]]!")
            __ASM_EMIT("vld1.32     {q10-q11}, [%[side]]!")
            __ASM_EMIT("vsub.f32    q2, q12, q2")               // q2 = m - s
            __ASM_EMIT("vsub.f32    q3, q13, q3")               // q3 = m - s
            __ASM_EMIT("vmov        q12, q4")
            __ASM_EMIT("vmov        q13, q5")
            __ASM_EMIT("vadd.f32    q4, q4, q6")
            __ASM_EMIT("vadd.f32    q5, q5, q7")
            __ASM_EMIT("vsub.f32    q6, q12, q6")
            __ASM_EMIT("vsub.f32    q7, q13, q7")
            __ASM_EMIT("vmov        q12, q8")
            __ASM_EMIT("vmov        q13, q9")
            __ASM_EMIT("vst1.32     {q0-q1}, [%[left]]!")
            __ASM_EMIT("vst1.32     {q2-q3}, [%[right]]!")
            __ASM_EMIT("vadd.f32    q8, q8, q10")
            __ASM_EMIT("vadd.f32    q9, q9, q11")
            __ASM_EMIT("vst1.32     {q4-q5}, [%[left]]!")
            __ASM_EMIT("vst1.32     {q6-q7}, [%[right]]!")
            __ASM_EMIT("vsub.f32    q10, q12, q10")
            __ASM_EMIT("vsub.f32    q11, q13, q11")
            __ASM_EMIT("vst1.32     {q8-q9}, [%[left]]!")
            __ASM_EMIT("vst1.32     {q10-q11}, [%[right]]!")
            __ASM_EMIT("subs        %[count], $24")
            __ASM_EMIT("bhs         1b")
            // x16 block
            __ASM_EMIT("2:")
            __ASM_EMIT("adds        %[count], $8")              // 24 - 16
            __ASM_EMIT("blt         4f")
            __ASM_EMIT("vld1.32     {q0-q1}, [%[mid]]!")        // q0, q1 = m
            __ASM_EMIT("vld1.32     {q2-q3}, [%[side]]!")       // q2, q3 = s
            __ASM_EMIT("vld1.32     {q4-q5}, [%[mid]]!")
            __ASM_EMIT("vld1.32     {q6-q7}, [%[side]]!")
            __ASM_EMIT("vmov        q12, q0")                   // q12 = m
            __ASM_EMIT("vmov        q13, q1")                   // q13 = m
            __ASM_EMIT("vadd.f32    q0, q0, q2")                // q0 = m + s
            __ASM_EMIT("vadd.f32    q1, q1, q3")                // q1 = m + s
            __ASM_EMIT("vsub.f32    q2, q12, q2")               // q2 = m - s
            __ASM_EMIT("vsub.f32    q3, q13, q3")               // q3 = m - s
            __ASM_EMIT("vmov        q12, q4")
            __ASM_EMIT("vmov        q13, q5")
            __ASM_EMIT("vadd.f32    q4, q4, q6")
            __ASM_EMIT("vadd.f32    q5, q5, q7")
            __ASM_EMIT("vsub.f32    q6, q12, q6")
            __ASM_EMIT("vsub.f32    q7, q13, q7")
            __ASM_EMIT("vst1.32     {q0-q1}, [%[left]]!")
            __ASM_EMIT("vst1.32     {q2-q3}, [%[right]]!")
            __ASM_EMIT("vst1.32     {q4-q5}, [%[left]]!")
            __ASM_EMIT("vst1.32     {q6-q7}, [%[right]]!")
            __ASM_EMIT("sub         %[count], $16")
            // x8 block
            __ASM_EMIT("4:")
            __ASM_EMIT("adds        %[count], $8")              // 16 - 8
            __ASM_EMIT("blt         6f")
            __ASM_EMIT("vld1.32     {q0-q1}, [%[mid]]!")        // q0, q1 = m
            __ASM_EMIT("vld1.32     {q2-q3}, [%[side]]!")       // q2, q3 = s
            __ASM_EMIT("vmov        q12, q0")                   // q12 = m
            __ASM_EMIT("vmov        q13, q1")                   // q13 = m
            __ASM_EMIT("vadd.f32    q0, q0, q2")                // q0 = m + s
            __ASM_EMIT("vadd.f32    q1, q1, q3")                // q1 = m + s
            __ASM_EMIT("vsub.f32    q2, q12, q2")               // q2 = m - s
            __ASM_EMIT("vsub.f32    q3, q13, q3")               // q3 = m - s
            __ASM_EMIT("vst1.32     {q0-q1}, [%[left]]!")
            __ASM_EMIT("vst1.32     {q2-q3}, [%[right]]!")
            __ASM_EMIT("sub         %[count], $8")
            // x4 block
            __ASM_EMIT("6:")
            __ASM_EMIT("adds        %[count], $4")              // 8 - 4
            __ASM_EMIT("blt         8f")
            __ASM_EMIT("vld1.32     {q0}, [%[mid]]!")           // q0 = m
            __ASM_EMIT("vld1.32     {q2}, [%[side]]!")          // q2 = s
            __ASM_EMIT("vmov        q12, q0")                   // q12 = m
            __ASM_EMIT("vadd.f32    q0, q0, q2")                // q0 = m + s
            __ASM_EMIT("vsub.f32    q2, q12, q2")               // q2 = m - s
            __ASM_EMIT("vst1.32     {q0}, [%[left]]!")
            __ASM_EMIT("vst1.32     {q2}, [%[right]]!")
            __ASM_EMIT("sub         %[count], $4")
            // x1 blocks
            __ASM_EMIT("8:")
            __ASM_EMIT("adds        %[count], $3")              // 4 - 1
            __ASM_EMIT("blt         10f")
            __ASM_EMIT("9:")
            __ASM_EMIT("vldm        %[mid]!, {s0}")             // s0 = m
            __ASM_EMIT("vldm        %[side]!, {s2}")            // s2 = s
            __ASM_EMIT("vmov        s12, s0")                   // s12 = m
            __ASM_EMIT("vadd.f32    s0, s0, s2")                // s0 = m + s
            __ASM_EMIT("vsub.f32    s2, s12, s2")               // s2 = m - s
            __ASM_EMIT("vstm        %[left]!, {s2}")
            __ASM_EMIT("vstm        %[right]!, {s4}")
            __ASM_EMIT("subs        %[count], $1")
            __ASM_EMIT("bge         9b")
            __ASM_EMIT("10:")

            : [mid] "+r" (m), [side] "+r" (s),
              [left] "+r" (l), [right] "+r" (r),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    #define LR_CVT_BODY(d, l, r, op)    \
        __ASM_EMIT("subs        %[count], $24") \
        __ASM_EMIT("vld1.32     {q0-q1}, [%[X_HALF]]")      /* q0 = 0.5, q1 = 0.5 */ \
        __ASM_EMIT("blo         2f") \
        /* x24 blocks */ \
        __ASM_EMIT("1:") \
        __ASM_EMIT("vld1.32     {q2-q3}, [%[" l "]]!")      /* q2 = l, q3 = l */ \
        __ASM_EMIT("vld1.32     {q4-q5}, [%[" r "]]!")      /* q4 = r, q5 = r */ \
        __ASM_EMIT("vld1.32     {q6-q7}, [%[" l "]]!") \
        __ASM_EMIT("vld1.32     {q8-q9}, [%[" r "]]!") \
        __ASM_EMIT(op ".f32     q2, q2, q4")                /* q2 = l <+-> r */ \
        __ASM_EMIT(op ".f32     q3, q3, q5")                /* q3 = l <+-> r */ \
        __ASM_EMIT("vld1.32     {q10-q11}, [%[" l "]]!") \
        __ASM_EMIT("vld1.32     {q12-q13}, [%[" r "]]!") \
        __ASM_EMIT(op ".f32     q6, q6, q8") \
        __ASM_EMIT(op ".f32     q7, q7, q9") \
        __ASM_EMIT(op ".f32     q10, q10, q12") \
        __ASM_EMIT(op ".f32     q11, q11, q13") \
        __ASM_EMIT("vmul.f32    q2, q0")                    /* q2 = (l <+-> r) * 0.5f */ \
        __ASM_EMIT("vmul.f32    q3, q1")                    /* q3 = (l <+-> r) * 0.5f */ \
        __ASM_EMIT("vmul.f32    q6, q0") \
        __ASM_EMIT("vmul.f32    q7, q1") \
        __ASM_EMIT("vst1.32     {q2-q3}, [%[" d "]]!") \
        __ASM_EMIT("vmul.f32    q10, q0") \
        __ASM_EMIT("vmul.f32    q11, q1") \
        __ASM_EMIT("vst1.32     {q6-q7}, [%[" d "]]!") \
        __ASM_EMIT("vst1.32     {q10-q11}, [%[" d "]]!") \
        __ASM_EMIT("subs        %[count], $24") \
        __ASM_EMIT("bhs         1b") \
        /* x16 block */ \
        __ASM_EMIT("2:") \
        __ASM_EMIT("adds        %[count], $8")              /* 24 - 16 */ \
        __ASM_EMIT("blt         4f") \
        __ASM_EMIT("vld1.32     {q2-q3}, [%[" l "]]!")      /* q2 = l, q3 = l */ \
        __ASM_EMIT("vld1.32     {q4-q5}, [%[" r "]]!")      /* q4 = r, q5 = r */ \
        __ASM_EMIT("vld1.32     {q6-q7}, [%[" l "]]!") \
        __ASM_EMIT("vld1.32     {q8-q9}, [%[" r "]]!") \
        __ASM_EMIT(op ".f32     q2, q2, q4")                /* q2 = l <+-> r */ \
        __ASM_EMIT(op ".f32     q3, q3, q5")                /* q3 = l <+-> r */ \
        __ASM_EMIT(op ".f32     q6, q6, q8") \
        __ASM_EMIT(op ".f32     q7, q7, q9") \
        __ASM_EMIT("vmul.f32    q2, q0")                    /* q2 = (l <+-> r) * 0.5f */ \
        __ASM_EMIT("vmul.f32    q3, q1")                    /* q3 = (l <+-> r) * 0.5f */ \
        __ASM_EMIT("vmul.f32    q6, q0") \
        __ASM_EMIT("vmul.f32    q7, q1") \
        __ASM_EMIT("vst1.32     {q2-q3}, [%[" d "]]!") \
        __ASM_EMIT("vst1.32     {q6-q7}, [%[" d "]]!") \
        __ASM_EMIT("sub         %[count], $16") \
        /* x8 block */ \
        __ASM_EMIT("4:") \
        __ASM_EMIT("adds        %[count], $8")              /* 16 - 8 */ \
        __ASM_EMIT("blt         6f") \
        __ASM_EMIT("vld1.32     {q2-q3}, [%[" l "]]!")      /* q2 = l, q3 = l */ \
        __ASM_EMIT("vld1.32     {q4-q5}, [%[" r "]]!")      /* q4 = r, q5 = r */ \
        __ASM_EMIT(op ".f32     q2, q2, q4")                /* q2 = l <+-> r */ \
        __ASM_EMIT(op ".f32     q3, q3, q5")                /* q3 = l <+-> r */ \
        __ASM_EMIT("vmul.f32    q2, q0")                    /* q2 = (l <+-> r) * 0.5f */ \
        __ASM_EMIT("vmul.f32    q3, q1")                    /* q3 = (l <+-> r) * 0.5f */ \
        __ASM_EMIT("vst1.32     {q2-q3}, [%[" d "]]!") \
        __ASM_EMIT("sub         %[count], $8") \
        /* x4 block */ \
        __ASM_EMIT("6:") \
        __ASM_EMIT("adds        %[count], $4")              /* 8 - 4 */ \
        __ASM_EMIT("blt         8f") \
        __ASM_EMIT("vld1.32     {q2}, [%[" l "]]!")         /* q2 = l */ \
        __ASM_EMIT("vld1.32     {q4}, [%[" r "]]!")         /* q4 = r */ \
        __ASM_EMIT(op ".f32     q2, q2, q4")                /* q2 = l <+-> r */ \
        __ASM_EMIT("vmul.f32    q2, q0")                    /* q2 = (l <+-> r) * 0.5f */ \
        __ASM_EMIT("vst1.32     {q2}, [%[" d "]]!") \
        __ASM_EMIT("sub         %[count], $4") \
        /* x1 blocks */ \
        __ASM_EMIT("8:") \
        __ASM_EMIT("adds        %[count], $3")              /* 4 - 1 */ \
        __ASM_EMIT("blt         10f") \
        __ASM_EMIT("9:") \
        __ASM_EMIT("vldm        %[" l "]!, {s2}")           /* s2 = l */ \
        __ASM_EMIT("vldm        %[" r "]!, {s4}")           /* s4 = r */ \
        __ASM_EMIT(op ".f32     s2, s2, s4")                /* q2 = l <+-> r */ \
        __ASM_EMIT("vmul.f32    s2, s0")                    /* q2 = (l <+-> r) * 0.5f */ \
        __ASM_EMIT("vstm        %[" d "]!, {s2}") \
        __ASM_EMIT("subs        %[count], $1") \
        __ASM_EMIT("bge         9b") \
        __ASM_EMIT("10:")


    void lr_to_mid(float *m, const float *l, const float *r, size_t count)
    {
        IF_ARCH_ARM(const float *half = X_HALF);

        ARCH_ARM_ASM
        (
            LR_CVT_BODY("mid", "left", "right", "vadd")
            : [mid] "+r" (m),
              [left] "+r" (l), [right] "+r" (r),
              [count] "+r" (count)
            : [X_HALF] "r" (half)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    void lr_to_side(float *s, const float *l, const float *r, size_t count)
    {
        IF_ARCH_ARM(const float *half = X_HALF);

        ARCH_ARM_ASM
        (
            LR_CVT_BODY("side", "left", "right", "vsub")
            : [side] "+r" (s),
              [left] "+r" (l), [right] "+r" (r),
              [count] "+r" (count)
            : [X_HALF] "r" (half)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

#undef LR_CVT_BODY

#define MS_CVT_BODY(d, m, s, op)    \
    __ASM_EMIT("subs        %[count], $32") \
    __ASM_EMIT("blo         2f") \
    /* x24 blocks */ \
    __ASM_EMIT("1:") \
    __ASM_EMIT("vld1.32     {q0-q1}, [%[" m "]]!")      /* q0 = m, q1 = m */ \
    __ASM_EMIT("vld1.32     {q8-q9}, [%[" s "]]!")      /* q8 = s, q9 = s */ \
    __ASM_EMIT("vld1.32     {q2-q3}, [%[" m "]]!") \
    __ASM_EMIT("vld1.32     {q10-q11}, [%[" s "]]!") \
    __ASM_EMIT(op ".f32     q0, q8")                    /* q0 = m <+-> s */ \
    __ASM_EMIT(op ".f32     q1, q9")                    /* q1 = m <+-> s */ \
    __ASM_EMIT("vld1.32     {q4-q5}, [%[" m "]]!") \
    __ASM_EMIT("vld1.32     {q12-q13}, [%[" s "]]!") \
    __ASM_EMIT(op ".f32     q2, q10") \
    __ASM_EMIT(op ".f32     q3, q11") \
    __ASM_EMIT("vst1.32     {q0-q1}, [%[" d "]]!") \
    __ASM_EMIT("vld1.32     {q6-q7}, [%[" m "]]!") \
    __ASM_EMIT("vld1.32     {q14-q15}, [%[" s "]]!") \
    __ASM_EMIT(op ".f32     q4, q12") \
    __ASM_EMIT(op ".f32     q5, q13") \
    __ASM_EMIT("vst1.32     {q2-q3}, [%[" d "]]!") \
    __ASM_EMIT(op ".f32     q6, q14") \
    __ASM_EMIT(op ".f32     q7, q15") \
    __ASM_EMIT("vst1.32     {q4-q5}, [%[" d "]]!") \
    __ASM_EMIT("vst1.32     {q6-q7}, [%[" d "]]!") \
    __ASM_EMIT("subs        %[count], $32") \
    __ASM_EMIT("bhs         1b") \
    /* x16 block */ \
    __ASM_EMIT("2:") \
    __ASM_EMIT("adds        %[count], $16")              /* 32 - 16 */ \
    __ASM_EMIT("blt         4f") \
    __ASM_EMIT("vld1.32     {q0-q1}, [%[" m "]]!")      /* q0 = m, q1 = m */ \
    __ASM_EMIT("vld1.32     {q8-q9}, [%[" s "]]!")      /* q8 = s, q9 = s */ \
    __ASM_EMIT("vld1.32     {q2-q3}, [%[" m "]]!") \
    __ASM_EMIT("vld1.32     {q10-q11}, [%[" s "]]!") \
    __ASM_EMIT(op ".f32     q0, q8")                    /* q0 = m <+-> s */ \
    __ASM_EMIT(op ".f32     q1, q9")                    /* q1 = m <+-> s */ \
    __ASM_EMIT(op ".f32     q2, q10") \
    __ASM_EMIT(op ".f32     q3, q11") \
    __ASM_EMIT("vst1.32     {q0-q1}, [%[" d "]]!") \
    __ASM_EMIT("vst1.32     {q2-q3}, [%[" d "]]!") \
    __ASM_EMIT("sub         %[count], $16") \
    /* x8 block */ \
    __ASM_EMIT("4:") \
    __ASM_EMIT("adds        %[count], $8")              /* 16 - 8 */ \
    __ASM_EMIT("blt         6f") \
    __ASM_EMIT("vld1.32     {q0-q1}, [%[" m "]]!")      /* q0 = m, q1 = m */ \
    __ASM_EMIT("vld1.32     {q8-q9}, [%[" s "]]!")      /* q8 = s, q9 = s */ \
    __ASM_EMIT(op ".f32     q0, q8")                    /* q0 = m <+-> s */ \
    __ASM_EMIT(op ".f32     q1, q9")                    /* q1 = m <+-> s */ \
    __ASM_EMIT("vst1.32     {q0-q1}, [%[" d "]]!") \
    __ASM_EMIT("sub         %[count], $8") \
    /* x4 block */ \
    __ASM_EMIT("6:") \
    __ASM_EMIT("adds        %[count], $4")              /* 8 - 4 */ \
    __ASM_EMIT("blt         8f") \
    __ASM_EMIT("vld1.32     {q0}, [%[" m "]]!")         /* q0 = m, q1 = m */ \
    __ASM_EMIT("vld1.32     {q8}, [%[" s "]]!")         /* q8 = s, q9 = s */ \
    __ASM_EMIT(op ".f32     q0, q8")                    /* q0 = m <+-> s */ \
    __ASM_EMIT("vst1.32     {q0}, [%[" d "]]!") \
    __ASM_EMIT("sub         %[count], $4") \
    /* x1 blocks */ \
    __ASM_EMIT("8:") \
    __ASM_EMIT("adds        %[count], $3")              /* 4 - 1 */ \
    __ASM_EMIT("blt         10f") \
    __ASM_EMIT("9:") \
    __ASM_EMIT("vldm        %[" m "]!, {s0}")           /* s0 = m */ \
    __ASM_EMIT("vldm        %[" s "]!, {s8}")           /* s4 = s */ \
    __ASM_EMIT(op ".f32     s0, s8")                    /* q0 = m <+-> s */ \
    __ASM_EMIT("vstm        %[" d "]!, {s0}") \
    __ASM_EMIT("subs        %[count], $1") \
    __ASM_EMIT("bge         9b") \
    __ASM_EMIT("10:")

    void ms_to_left(float *l, const float *m, const float *s, size_t count)
    {
        ARCH_ARM_ASM
        (
            MS_CVT_BODY("left", "mid", "side", "vadd")
            : [left] "+r" (l),
              [mid] "+r" (m), [side] "+r" (s),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    void ms_to_right(float *r, const float *m, const float *s, size_t count)
    {
        ARCH_ARM_ASM
        (
            MS_CVT_BODY("right", "mid", "side", "vsub")
            : [right] "+r" (r),
              [mid] "+r" (m), [side] "+r" (s),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

#undef MS_CVT_BODY
}

#endif /* DSP_ARCH_ARM_NEON_D32_MSMATRIX_H_ */
