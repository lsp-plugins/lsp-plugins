/*
 * pcomplex.h
 *
 *  Created on: 5 сент. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_ARM_NEON_D32_PCOMPLEX_H_
#define DSP_ARCH_ARM_NEON_D32_PCOMPLEX_H_

#ifndef DSP_ARCH_ARM_NEON_32_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_ARM_NEON_32_IMPL */

namespace neon_d32
{
    void pcomplex_mul3(float *dst, const float *src1, const float *src2, size_t count)
    {
        ARCH_ARM_ASM
        (
            __ASM_EMIT("subs        %[count], $8")
            __ASM_EMIT("blo         2f")

            // x8 blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("vld2.32     {q0-q1}, [%[src1]]!")        // q0 = sr, q1 = si
            __ASM_EMIT("vld2.32     {q2-q3}, [%[src2]]!")        // q2 = dr, q3 = di
            __ASM_EMIT("vld2.32     {q4-q5}, [%[src1]]!")
            __ASM_EMIT("vmul.f32    q8, q0, q2")                 // q8 = sr*dr
            __ASM_EMIT("vld2.32     {q6-q7}, [%[src2]]!")
            __ASM_EMIT("vmul.f32    q9, q1, q2")                 // q9 = si*dr
            __ASM_EMIT("vmul.f32    q10, q4, q6")
            __ASM_EMIT("vmls.f32    q8, q1, q3")                 // q8 = sr*dr - si*di
            __ASM_EMIT("vmul.f32    q11, q5, q6")
            __ASM_EMIT("vmla.f32    q9, q0, q3")                 // q9 = si*dr + sr*di
            __ASM_EMIT("vmls.f32    q10, q5, q7")
            __ASM_EMIT("vst2.32     {q8-q9}, [%[dst]]!")
            __ASM_EMIT("vmla.f32    q11, q4, q7")
            __ASM_EMIT("vst2.32     {q10-q11}, [%[dst]]!")
            __ASM_EMIT("subs        %[count], $8")
            __ASM_EMIT("bhs         1b")

            // x4 blocks
            __ASM_EMIT("2:")
            __ASM_EMIT("adds        %[count], $4")
            __ASM_EMIT("blt         4f")
            __ASM_EMIT("vld2.32     {q0-q1}, [%[src1]]!")        // q0 = sr, q1 = si
            __ASM_EMIT("vld2.32     {q2-q3}, [%[src2]]!")        // q2 = dr, q3 = di
            __ASM_EMIT("vmul.f32    q8, q0, q2")                 // q8 = sr*dr
            __ASM_EMIT("vmul.f32    q9, q1, q2")                 // q9 = si*dr
            __ASM_EMIT("vmls.f32    q8, q1, q3")                 // q8 = sr*dr - si*di
            __ASM_EMIT("vmla.f32    q9, q0, q3")                 // q9 = si*dr + sr*di
            __ASM_EMIT("vst2.32     {q8-q9}, [%[dst]]!")
            __ASM_EMIT("sub         %[count], $4")

            // x1 blocks
            __ASM_EMIT("4:")
            __ASM_EMIT("adds        %[count], $3")
            __ASM_EMIT("blt         6f")
            __ASM_EMIT("5:")
            __ASM_EMIT("vldm.32     %[src1]!, {s0-s1}")          // s0 = sr, q1 = si
            __ASM_EMIT("vldm.32     %[src2]!, {s2-s3}")          // s2 = dr, q3 = di
            __ASM_EMIT("vmul.f32    s8, s0, s2")                 // s8 = sr*dr
            __ASM_EMIT("vmul.f32    s9, s1, s2")                 // s9 = si*dr
            __ASM_EMIT("vmls.f32    s8, s1, s3")                 // s8 = sr*dr - si*di
            __ASM_EMIT("vmla.f32    s9, s0, s3")                 // s9 = si*dr + sr*di
            __ASM_EMIT("vstm.32     %[dst]!, {s8-s9}")
            __ASM_EMIT("subs        %[count], $1")
            __ASM_EMIT("bge         5b")

            __ASM_EMIT("6:")

            : [dst] "+r" (dst), [src1] "+r" (src1), [src2] "+r" (src2),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11"
        );
    }

    void pcomplex_mod(float *dst, const float *src, size_t count)
    {
        ARCH_ARM_ASM
        (
            __ASM_EMIT("subs            %[count], $16")
            __ASM_EMIT("blo             2f")

            //-----------------------------------------------------------------
            // x16 blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("vld2.32         {q0-q1}, [%[src]]!")        // q0 = r, q1 = i
            __ASM_EMIT("vld2.32         {q2-q3}, [%[src]]!")
            __ASM_EMIT("vmul.f32        q0, q0, q0")                // q0 = r*r
            __ASM_EMIT("vld2.32         {q4-q5}, [%[src]]!")
            __ASM_EMIT("vmul.f32        q2, q2, q2")
            __ASM_EMIT("vld2.32         {q6-q7}, [%[src]]!")
            __ASM_EMIT("vmul.f32        q4, q4, q4")
            __ASM_EMIT("vmul.f32        q6, q6, q6")
            __ASM_EMIT("vmla.f32        q0, q1, q1")                // q0 = R = r*r + i*i
            __ASM_EMIT("vmla.f32        q2, q3, q3")
            __ASM_EMIT("vmla.f32        q4, q5, q5")
            __ASM_EMIT("vmla.f32        q6, q7, q7")

            // 1/sqrt(R) calculation
            __ASM_EMIT("vrsqrte.f32     q1, q0")                    // q1 = x0
            __ASM_EMIT("vrsqrte.f32     q3, q2")
            __ASM_EMIT("vrsqrte.f32     q5, q4")
            __ASM_EMIT("vrsqrte.f32     q7, q6")
            __ASM_EMIT("vmul.f32        q8, q1, q0")                // q8 = R * x0
            __ASM_EMIT("vmul.f32        q10, q3, q2")
            __ASM_EMIT("vmul.f32        q12, q5, q4")
            __ASM_EMIT("vmul.f32        q14, q7, q6")
            __ASM_EMIT("vrsqrts.f32     q9, q8, q1")                // q9 = (3 - R * x0 * x0) / 2
            __ASM_EMIT("vrsqrts.f32     q11, q10, q3")
            __ASM_EMIT("vrsqrts.f32     q13, q12, q5")
            __ASM_EMIT("vrsqrts.f32     q15, q14, q7")
            __ASM_EMIT("vmul.f32        q1, q1, q9")                // q1 = x1 = x0 * (3 - R * x0 * x0) / 2
            __ASM_EMIT("vmul.f32        q3, q3, q11")
            __ASM_EMIT("vmul.f32        q5, q5, q13")
            __ASM_EMIT("vmul.f32        q7, q7, q15")
            __ASM_EMIT("vmul.f32        q8, q1, q0")                // q8 = R * x1
            __ASM_EMIT("vmul.f32        q10, q3, q2")
            __ASM_EMIT("vmul.f32        q12, q5, q4")
            __ASM_EMIT("vmul.f32        q14, q7, q6")
            __ASM_EMIT("vrsqrts.f32     q9, q8, q1")                // q9 = (3 - R * x1 * x1) / 2
            __ASM_EMIT("vrsqrts.f32     q11, q10, q3")
            __ASM_EMIT("vrsqrts.f32     q13, q12, q5")
            __ASM_EMIT("vrsqrts.f32     q15, q14, q7")
            __ASM_EMIT("vmul.f32        q0, q1, q9")                // q0 = x2 = x1 * (3 - R * x1 * x1) / 2
            __ASM_EMIT("vmul.f32        q2, q3, q11")
            __ASM_EMIT("vmul.f32        q4, q5, q13")
            __ASM_EMIT("vmul.f32        q6, q7, q15")
            // 1 / (1/sqrt(R)) = sqrt(R) calculation
            __ASM_EMIT("vrecpe.f32      q1, q0")                    // q1 = x0
            __ASM_EMIT("vrecpe.f32      q3, q2")
            __ASM_EMIT("vrecpe.f32      q5, q4")
            __ASM_EMIT("vrecpe.f32      q7, q6")
            __ASM_EMIT("vrecps.f32      q8, q1, q0")                // q8 = (2 - R*x0)
            __ASM_EMIT("vrecps.f32      q10, q3, q2")
            __ASM_EMIT("vrecps.f32      q12, q5, q4")
            __ASM_EMIT("vrecps.f32      q14, q7, q6")
            __ASM_EMIT("vmul.f32        q1, q8, q1")                // q1 = x1 = x0 * (2 - R*x0)
            __ASM_EMIT("vmul.f32        q3, q10, q3")
            __ASM_EMIT("vmul.f32        q5, q12, q5")
            __ASM_EMIT("vmul.f32        q7, q14, q7")
            __ASM_EMIT("vrecps.f32      q8, q1, q0")                // q8 = (2 - R*x1)
            __ASM_EMIT("vrecps.f32      q10, q3, q2")
            __ASM_EMIT("vrecps.f32      q12, q5, q4")
            __ASM_EMIT("vrecps.f32      q14, q7, q6")
            __ASM_EMIT("vmul.f32        q0, q8, q1")                // q0 = x2 = x1 * (2 - R*x0)
            __ASM_EMIT("vmul.f32        q1, q10, q3")
            __ASM_EMIT("vmul.f32        q2, q12, q5")
            __ASM_EMIT("vmul.f32        q3, q14, q7")
            __ASM_EMIT("vst1.32         {q0-q1}, [%[dst]]!")
            __ASM_EMIT("subs            %[count], $16")
            __ASM_EMIT("vst1.32         {q2-q3}, [%[dst]]!")
            __ASM_EMIT("bhs             1b")

            //-----------------------------------------------------------------
            // x8 blocks
            __ASM_EMIT("2:")
            __ASM_EMIT("adds            %[count], $8")
            __ASM_EMIT("blt             4f")
            __ASM_EMIT("vld2.32         {q0-q1}, [%[src]]!")        // q0 = r, q1 = i
            __ASM_EMIT("vld2.32         {q2-q3}, [%[src]]!")
            __ASM_EMIT("vmul.f32        q0, q0, q0")                // q0 = r*r
            __ASM_EMIT("vmul.f32        q2, q2, q2")
            __ASM_EMIT("vmla.f32        q0, q1, q1")                // q0 = R = r*r + i*i
            __ASM_EMIT("vmla.f32        q2, q3, q3")
            // 1/sqrt(R) calculation
            __ASM_EMIT("vrsqrte.f32     q1, q0")                    // q1 = x0
            __ASM_EMIT("vrsqrte.f32     q3, q2")
            __ASM_EMIT("vmul.f32        q8, q1, q0")                // q8 = R * x0
            __ASM_EMIT("vmul.f32        q10, q3, q2")
            __ASM_EMIT("vrsqrts.f32     q9, q8, q1")                // q9 = (3 - R * x0 * x0) / 2
            __ASM_EMIT("vrsqrts.f32     q11, q10, q3")
            __ASM_EMIT("vmul.f32        q1, q1, q9")                // q1 = x1 = x0 * (3 - R * x0 * x0) / 2
            __ASM_EMIT("vmul.f32        q3, q3, q11")
            __ASM_EMIT("vmul.f32        q8, q1, q0")                // q8 = R * x1
            __ASM_EMIT("vmul.f32        q10, q3, q2")
            __ASM_EMIT("vrsqrts.f32     q9, q8, q1")                // q9 = (3 - R * x1 * x1) / 2
            __ASM_EMIT("vrsqrts.f32     q11, q10, q3")
            __ASM_EMIT("vmul.f32        q0, q1, q9")                // q0 = x2 = x1 * (3 - R * x1 * x1) / 2
            __ASM_EMIT("vmul.f32        q2, q3, q11")
            // 1 / (1/sqrt(R)) = sqrt(R) calculation
            __ASM_EMIT("vrecpe.f32      q1, q0")                    // q1 = x0
            __ASM_EMIT("vrecpe.f32      q3, q2")
            __ASM_EMIT("vrecps.f32      q8, q1, q0")                // q8 = (2 - R*x0)
            __ASM_EMIT("vrecps.f32      q10, q3, q2")
            __ASM_EMIT("vmul.f32        q1, q8, q1")                // q1 = x1 = x0 * (2 - R*x0)
            __ASM_EMIT("vmul.f32        q3, q10, q3")
            __ASM_EMIT("vrecps.f32      q8, q1, q0")                // q8 = (2 - R*x1)
            __ASM_EMIT("vrecps.f32      q10, q3, q2")
            __ASM_EMIT("vmul.f32        q0, q8, q1")                // q0 = x2 = x1 * (2 - R*x0)
            __ASM_EMIT("vmul.f32        q1, q10, q3")
            __ASM_EMIT("sub             %[count], $8")
            __ASM_EMIT("vst1.32         {q0-q1}, [%[dst]]!")

            //-----------------------------------------------------------------
            // x4 blocks
            __ASM_EMIT("4:")
            __ASM_EMIT("adds            %[count], $4")
            __ASM_EMIT("blt             6f")
            __ASM_EMIT("vld2.32         {q0-q1}, [%[src]]!")        // q0 = r, q1 = i
            __ASM_EMIT("vmul.f32        q0, q0, q0")                // q0 = r*r
            __ASM_EMIT("vmla.f32        q0, q1, q1")                // q0 = R = r*r + i*i
            // 1/sqrt(R) calculation
            __ASM_EMIT("vrsqrte.f32     q1, q0")                    // q1 = x0
            __ASM_EMIT("vmul.f32        q2, q1, q0")                // q2 = R * x0
            __ASM_EMIT("vrsqrts.f32     q3, q2, q1")                // q3 = (3 - R * x0 * x0) / 2
            __ASM_EMIT("vmul.f32        q1, q1, q3")                // q1 = x1 = x0 * (3 - R * x0 * x0) / 2
            __ASM_EMIT("vmul.f32        q2, q1, q0")                // q2 = R * x1
            __ASM_EMIT("vrsqrts.f32     q3, q2, q1")                // q3 = (3 - R * x1 * x1) / 2
            __ASM_EMIT("vmul.f32        q0, q1, q3")                // q0 = x2 = x1 * (3 - R * x1 * x1) / 2
            // 1 / (1/sqrt(R)) = sqrt(R) calculation
            __ASM_EMIT("vrecpe.f32      q1, q0")                    // q1 = x0
            __ASM_EMIT("vrecps.f32      q2, q1, q0")                // q2 = (2 - R*x0)
            __ASM_EMIT("vmul.f32        q1, q2, q1")                // q1 = x1 = x0 * (2 - R*x0)
            __ASM_EMIT("vrecps.f32      q2, q1, q0")                // q2 = (2 - R*x1)
            __ASM_EMIT("vmul.f32        q0, q2, q1")                // q0 = x2 = x1 * (2 - R*x0)
            __ASM_EMIT("sub             %[count], $4")
            __ASM_EMIT("vst1.32         {q0}, [%[dst]]!")

            // x1 blocks
            __ASM_EMIT("6:")
            __ASM_EMIT("adds        %[count], $3")
            __ASM_EMIT("blt         8f")
            __ASM_EMIT("7:")
            __ASM_EMIT("vldm.32     %[src]!, {s0-s1}")
            __ASM_EMIT("vmul.f32    s0, s0, s0")                    // s0 = r*r
            __ASM_EMIT("vmla.f32    s0, s1, s1")                    // s0 = r*r + i*i
            __ASM_EMIT("vsqrt.f32   s0, s0")                        // s0 = sqrt(r*r + i*i)
            __ASM_EMIT("subs        %[count], $1")
            __ASM_EMIT("vstm.32     %[dst]!, {s0}")
            __ASM_EMIT("bge         7b")

            __ASM_EMIT("8:")

            : [dst] "+r" (dst), [src] "+r" (src),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    void pcomplex_rcp1(float *dst, size_t count)
    {
        IF_ARCH_ARM(float *src);

        ARCH_ARM_ASM
        (
            __ASM_EMIT("subs            %[count], $8")
            __ASM_EMIT("blo             2f")

            //-----------------------------------------------------------------
            // x8 blocks
            __ASM_EMIT("mov             %[src], %[dst]")
            __ASM_EMIT("1:")
            __ASM_EMIT("vld2.32         {q0-q1}, [%[src]]!")        // q0 = r, q1 = i
            __ASM_EMIT("vld2.32         {q2-q3}, [%[src]]!")
            __ASM_EMIT("vmul.f32        q4, q0, q0")                // q4 = r*r
            __ASM_EMIT("vmul.f32        q5, q2, q2")
            __ASM_EMIT("vmla.f32        q4, q1, q1")                // q4 = R = r*r + i*i
            __ASM_EMIT("vmla.f32        q5, q3, q3")
            // 1 / R calculation
            __ASM_EMIT("vrecpe.f32      q6, q4")                    // q6 = x0
            __ASM_EMIT("vrecpe.f32      q8, q5")
            __ASM_EMIT("vrecps.f32      q7, q6, q4")                // q7 = (2 - R*x0)
            __ASM_EMIT("vrecps.f32      q9, q8, q5")
            __ASM_EMIT("vmul.f32        q6, q7, q6")                // q6 = x1 = x0 * (2 - R*x0)
            __ASM_EMIT("vmul.f32        q8, q9, q8")
            __ASM_EMIT("vrecps.f32      q7, q6, q4")                // q7 = (2 - R*x1)
            __ASM_EMIT("vrecps.f32      q9, q8, q5")
            __ASM_EMIT("vmul.f32        q4, q7, q6")                // q4 = x2 = x1 * (2 - R*x0)
            __ASM_EMIT("vmul.f32        q5, q9, q8")
            // r/R, -i/R
            __ASM_EMIT("vneg.f32        q1, q1")                    // q1 = -i
            __ASM_EMIT("vneg.f32        q3, q3")
            __ASM_EMIT("vmul.f32        q0, q0, q4")                // q0 = r / R
            __ASM_EMIT("vmul.f32        q2, q2, q5")
            __ASM_EMIT("vmul.f32        q1, q1, q4")                // q1 = -i / R
            __ASM_EMIT("vmul.f32        q3, q3, q5")
            __ASM_EMIT("vst2.32         {q0-q1}, [%[dst]]!")
            __ASM_EMIT("subs            %[count], $8")
            __ASM_EMIT("vst2.32         {q2-q3}, [%[dst]]!")
            __ASM_EMIT("bhs             1b")

            //-----------------------------------------------------------------
            // x4 blocks
            __ASM_EMIT("2:")
            __ASM_EMIT("adds            %[count], $4")
            __ASM_EMIT("blt             4f")
            __ASM_EMIT("vld2.32         {q0-q1}, [%[dst]]")         // q0 = r, q1 = i
            __ASM_EMIT("vmul.f32        q4, q0, q0")                // q4 = r*r
            __ASM_EMIT("vmla.f32        q4, q1, q1")                // q4 = R = r*r + i*i
            // 1 / R calculation
            __ASM_EMIT("vrecpe.f32      q6, q4")                    // q6 = x0
            __ASM_EMIT("vrecps.f32      q7, q6, q4")                // q7 = (2 - R*x0)
            __ASM_EMIT("vmul.f32        q6, q7, q6")                // q6 = x1 = x0 * (2 - R*x0)
            __ASM_EMIT("vrecps.f32      q7, q6, q4")                // q7 = (2 - R*x1)
            __ASM_EMIT("vmul.f32        q4, q7, q6")                // q4 = x2 = x1 * (2 - R*x0)
            // r/R, -i/R
            __ASM_EMIT("vneg.f32        q1, q1")                    // q1 = -i
            __ASM_EMIT("vmul.f32        q0, q0, q4")                // q0 = r / R
            __ASM_EMIT("vmul.f32        q1, q1, q4")                // q1 = -i / R
            __ASM_EMIT("sub             %[count], $4")
            __ASM_EMIT("vst2.32         {q0-q1}, [%[dst]]!")

            // x1 blocks
            __ASM_EMIT("4:")
            __ASM_EMIT("adds            %[count], $3")
            __ASM_EMIT("blt             6f")
            __ASM_EMIT("5:")
            __ASM_EMIT("vldm.32         %[dst], {s0-s1}")           // s0 = r, s1 = i
            __ASM_EMIT("vmul.f32        s4, s0, s0")                // s4 = r*r
            __ASM_EMIT("vmla.f32        s4, s1, s1")                // s4 = R = r*r + i*i
            __ASM_EMIT("vneg.f32        s1, s1")                    // s1 = -i
            __ASM_EMIT("vdiv.f32        s0, s0, s4")                // s0 = r / R
            __ASM_EMIT("vdiv.f32        s1, s1, s4")                // s1 = -i / R
            __ASM_EMIT("subs            %[count], $1")
            __ASM_EMIT("vstm.32         %[dst]!, {s0-s1}")
            __ASM_EMIT("bge             5b")

            __ASM_EMIT("6:")

            : [src] "=&r" (src), [dst] "+r" (dst), [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11"
        );
    }

    void pcomplex_rcp2(float *dst, const float *src, size_t count)
    {
        ARCH_ARM_ASM
        (
            __ASM_EMIT("subs            %[count], $8")
            __ASM_EMIT("blo             2f")

            //-----------------------------------------------------------------
            // x8 blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("vld2.32         {q0-q1}, [%[src]]!")        // q0 = r, q1 = i
            __ASM_EMIT("vld2.32         {q2-q3}, [%[src]]!")
            __ASM_EMIT("vmul.f32        q4, q0, q0")                // q4 = r*r
            __ASM_EMIT("vmul.f32        q5, q2, q2")
            __ASM_EMIT("vmla.f32        q4, q1, q1")                // q4 = R = r*r + i*i
            __ASM_EMIT("vmla.f32        q5, q3, q3")
            // 1 / R calculation
            __ASM_EMIT("vrecpe.f32      q6, q4")                    // q6 = x0
            __ASM_EMIT("vrecpe.f32      q8, q5")
            __ASM_EMIT("vrecps.f32      q7, q6, q4")                // q7 = (2 - R*x0)
            __ASM_EMIT("vrecps.f32      q9, q8, q5")
            __ASM_EMIT("vmul.f32        q6, q7, q6")                // q6 = x1 = x0 * (2 - R*x0)
            __ASM_EMIT("vmul.f32        q8, q9, q8")
            __ASM_EMIT("vrecps.f32      q7, q6, q4")                // q7 = (2 - R*x1)
            __ASM_EMIT("vrecps.f32      q9, q8, q5")
            __ASM_EMIT("vmul.f32        q4, q7, q6")                // q4 = x2 = x1 * (2 - R*x0)
            __ASM_EMIT("vmul.f32        q5, q9, q8")
            // r/R, -i/R
            __ASM_EMIT("vneg.f32        q1, q1")                    // q1 = -i
            __ASM_EMIT("vneg.f32        q3, q3")
            __ASM_EMIT("vmul.f32        q0, q0, q4")                // q0 = r / R
            __ASM_EMIT("vmul.f32        q2, q2, q5")
            __ASM_EMIT("vmul.f32        q1, q1, q4")                // q1 = -i / R
            __ASM_EMIT("vmul.f32        q3, q3, q5")
            __ASM_EMIT("vst2.32         {q0-q1}, [%[dst]]!")
            __ASM_EMIT("subs            %[count], $8")
            __ASM_EMIT("vst2.32         {q2-q3}, [%[dst]]!")
            __ASM_EMIT("bhs             1b")

            //-----------------------------------------------------------------
            // x4 blocks
            __ASM_EMIT("2:")
            __ASM_EMIT("adds            %[count], $4")
            __ASM_EMIT("blt             4f")
            __ASM_EMIT("vld2.32         {q0-q1}, [%[src]]!")        // q0 = r, q1 = i
            __ASM_EMIT("vmul.f32        q4, q0, q0")                // q4 = r*r
            __ASM_EMIT("vmla.f32        q4, q1, q1")                // q4 = R = r*r + i*i
            // 1 / R calculation
            __ASM_EMIT("vrecpe.f32      q6, q4")                    // q6 = x0
            __ASM_EMIT("vrecps.f32      q7, q6, q4")                // q7 = (2 - R*x0)
            __ASM_EMIT("vmul.f32        q6, q7, q6")                // q6 = x1 = x0 * (2 - R*x0)
            __ASM_EMIT("vrecps.f32      q7, q6, q4")                // q7 = (2 - R*x1)
            __ASM_EMIT("vmul.f32        q4, q7, q6")                // q4 = x2 = x1 * (2 - R*x0)
            // r/R, -i/R
            __ASM_EMIT("vneg.f32        q1, q1")                    // q1 = -i
            __ASM_EMIT("vmul.f32        q0, q0, q4")                // q0 = r / R
            __ASM_EMIT("vmul.f32        q1, q1, q4")                // q1 = -i / R
            __ASM_EMIT("sub             %[count], $4")
            __ASM_EMIT("vst2.32         {q0-q1}, [%[dst]]!")

            // x1 blocks
            __ASM_EMIT("4:")
            __ASM_EMIT("adds            %[count], $3")
            __ASM_EMIT("blt             6f")
            __ASM_EMIT("5:")
            __ASM_EMIT("vldm.32         %[src]!, {s0-s1}")          // s0 = r, s1 = i
            __ASM_EMIT("vmul.f32        s4, s0, s0")                // s4 = r*r
            __ASM_EMIT("vmla.f32        s4, s1, s1")                // s4 = R = r*r + i*i
            __ASM_EMIT("vneg.f32        s1, s1")                    // s1 = -i
            __ASM_EMIT("vdiv.f32        s0, s0, s4")                // s0 = r / R
            __ASM_EMIT("vdiv.f32        s1, s1, s4")                // s1 = -i / R
            __ASM_EMIT("subs            %[count], $1")
            __ASM_EMIT("vstm.32         %[dst]!, {s0-s1}")
            __ASM_EMIT("bge             5b")

            __ASM_EMIT("6:")

            : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11"
        );
    }

}

#endif /* DSP_ARCH_ARM_NEON_D32_PCOMPLEX_H_ */
