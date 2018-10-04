/*
 * resampling.h
 *
 *  Created on: 4 окт. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_ARM_NEON_D32_RESAMPLING_H_
#define DSP_ARCH_ARM_NEON_D32_RESAMPLING_H_

namespace neon_d32
{
    static const float lanczos_2x2[] __lsp_aligned16 =
    {
        +0.0000000000000000f,
        -0.1664152316035080f,   // k1
        +0.0000000000000000f,
        +0.6203830132406946f,   // k0

        +1.0000000000000000f,
        +0.6203830132406946f,   // k0
        +0.0000000000000000f,
        -0.1664152316035080f    // k1
    };

    static const float lanczos_2x3[] __lsp_aligned16 =
    {
        +0.0000000000000000f,
        +0.0939539981090991f,   // k2
        +0.0000000000000000f,
        -0.1910530560835854f,   // k1

        +0.0000000000000000f,
        +0.6293724479752082f,   // k0
        +1.0000000000000000f,
        +0.6293724479752082f,   // k0

        +0.0000000000000000f,
        -0.1910530560835854f,   // k1
        +0.0000000000000000f,
        +0.0939539981090991f    // k2
    };

    void lanczos_resample_2x2(float *dst, const float *src, size_t count)
    {
        IF_ARCH_ARM(
            float *dw = dst;
            const float *kernel = lanczos_2x2;
        );

        ARCH_ARM_ASM
        (
            // Prepare
            __ASM_EMIT("subs            %[count], $8")
            __ASM_EMIT("vld1.32         {q0-q1}, [%[kernel]]")      // q0 = 0 k1 0 k0, q1 = 1 k0 0 k1
            __ASM_EMIT("blo             2f")

            // 8x blocks
            __ASM_EMIT("1:")
            // Even cycle: prepare
            __ASM_EMIT("vld2.32         {q2-q3}, [%[src]]!")        // q2 = s0 s2 s4 s6, q3 = s1 s3 s5 s7
            __ASM_EMIT("vmov            q4, q2")                    // q4 = s0 s2 s4 s6
            __ASM_EMIT("vmov            q5, q2")                    // q5 = s0 s2 s4 s6
            __ASM_EMIT("vmov            q6, q2")                    // q6 = s0 s2 s4 s6
            __ASM_EMIT("vld1.32         {q7-q8}, [%[dr]]!")
            __ASM_EMIT("vtrn.32         q2, q4")                    // q2 = s0 s0 s4 s4, q4 = s2 s2 s6 s6
            __ASM_EMIT("vtrn.32         q5, q6")                    // q5 = s0 s0 s4 s4, q6 = s2 s2 s6 s6
            __ASM_EMIT("vld1.32         {q9-q10}, [%[dr]]!")
            __ASM_EMIT("vswp            d5, d10")                   // q2 = s0, q4 = s2
            __ASM_EMIT("vld1.32         {q11}, [%[dr]]")
            __ASM_EMIT("vswp            d9, d12")                   // q5 = s4, q6 = s6
            // Even cycle: convolve
            __ASM_EMIT("vmla.f32        q7, q2, q0")
            __ASM_EMIT("vmla.f32        q8, q4, q0")
            __ASM_EMIT("vmla.f32        q9, q5, q0")
            __ASM_EMIT("vmla.f32        q10, q6, q0")
            __ASM_EMIT("vmla.f32        q8, q2, q1")
            __ASM_EMIT("vmla.f32        q9, q4, q1")
            __ASM_EMIT("vst1.32         {q7-q8}, [%[dw]]!")
            __ASM_EMIT("vmla.f32        q10, q5, q1")
            __ASM_EMIT("vmla.f32        q11, q6, q1")
            __ASM_EMIT("vst1.32         {q9-q10}, [%[dw]]!")
            __ASM_EMIT("vst1.32         {q11}, [%[dw]]")
            __ASM_EMIT("sub             %[dr], $0x38")
            __ASM_EMIT("sub             %[dw], $0x38")
            // Odd cycle: prepare
            __ASM_EMIT("vmov            q4, q3")
            __ASM_EMIT("vmov            q5, q3")
            __ASM_EMIT("vmov            q6, q4")
            __ASM_EMIT("vld1.32         {q7-q8}, [%[dr]]!")
            __ASM_EMIT("vtrn.32         q3, q4")
            __ASM_EMIT("vtrn.32         q5, q6")
            __ASM_EMIT("vld1.32         {q9-q10}, [%[dr]]!")
            __ASM_EMIT("vswp            d7, d10")
            __ASM_EMIT("vld1.32         {q11}, [%[dr]]")
            __ASM_EMIT("vswp            d9, d12")
            // Odd cycle: convolve
            __ASM_EMIT("vmla.f32        q7, q3, q0")
            __ASM_EMIT("vmla.f32        q8, q4, q0")
            __ASM_EMIT("vmla.f32        q9, q5, q0")
            __ASM_EMIT("vmla.f32        q10, q6, q0")
            __ASM_EMIT("vmla.f32        q8, q3, q1")
            __ASM_EMIT("vmla.f32        q9, q4, q1")
            __ASM_EMIT("vmla.f32        q10, q5, q1")
            __ASM_EMIT("vst1.32         {q7-q8}, [%[dw]]!")
            __ASM_EMIT("vmla.f32        q11, q6, q1")
            __ASM_EMIT("vst1.32         {q9-q10}, [%[dw]]!")
            __ASM_EMIT("vst1.32         {q11}, [%[dw]]")
            __ASM_EMIT("sub             %[dr], $0x08")
            __ASM_EMIT("sub             %[dw], $0x08")
            // Repeat loop
            __ASM_EMIT("subs            %[count], $8")
            __ASM_EMIT("bhs             1b")

            // 4x block
            __ASM_EMIT("2:")
            __ASM_EMIT("adds            %[count], $4")
            __ASM_EMIT("blt             4f")

            // Prepare
            __ASM_EMIT("vld1.32         {q2}, [%[src]]!")
            __ASM_EMIT("vmov            q3, q2")
            __ASM_EMIT("vmov            q4, q2")
            __ASM_EMIT("vmov            q5, q2")
            __ASM_EMIT("vld1.32         {q7-q8}, [%[dr]]!")
            __ASM_EMIT("vtrn.32         q2, q3")
            __ASM_EMIT("vtrn.32         q4, q5")
            __ASM_EMIT("vld1.32         {q9}, [%[dr]]")
            __ASM_EMIT("vswp            d5, d8")
            __ASM_EMIT("vswp            d7, d10") // q2 = s0, q3 = s1, q4 = s2, q5 = s3
            // Even cycle: convolve
            __ASM_EMIT("vmla.f32        q7, q2, q0")
            __ASM_EMIT("vmla.f32        q8, q4, q0")
            __ASM_EMIT("vmla.f32        q8, q2, q1")
            __ASM_EMIT("vmla.f32        q9, q4, q1")
            __ASM_EMIT("vst1.32         {q7-q8}, [%[dw]]!")
            __ASM_EMIT("vst1.32         {q9}, [%[dw]]")
            __ASM_EMIT("sub             %[dr], $0x18")
            __ASM_EMIT("sub             %[dw], $0x18")
            // Odd cycle: convolve
            __ASM_EMIT("vld1.32         {q7-q8}, [%[dr]]!")
            __ASM_EMIT("vld1.32         {q9}, [%[dr]]")
            __ASM_EMIT("vmla.f32        q7, q3, q0")
            __ASM_EMIT("vmla.f32        q8, q5, q0")
            __ASM_EMIT("vmla.f32        q8, q3, q1")
            __ASM_EMIT("vmla.f32        q9, q5, q1")
            __ASM_EMIT("vst1.32         {q7-q8}, [%[dw]]!")
            __ASM_EMIT("vst1.32         {q9}, [%[dw]]")
            __ASM_EMIT("sub             %[dr], $0x08")
            __ASM_EMIT("sub             %[dw], $0x08")
            // Even cycle: load/store and shift
            __ASM_EMIT("sub             %[count], $4")

            // 1x blocks
            __ASM_EMIT("4:")
            __ASM_EMIT("adds            %[count], $3")
            __ASM_EMIT("blt             6f")
            __ASM_EMIT("vld1.32         {q3-q4}, [%[dr]]!")
            __ASM_EMIT("7:")
            __ASM_EMIT("vldm            %[src]!, {s8}")
            __ASM_EMIT("vmov            s9, s8")
            __ASM_EMIT("vmov            d5, d4")
            __ASM_EMIT("vmla.f32        q3, q2, q0")
            __ASM_EMIT("vmla.f32        q4, q2, q1")
            __ASM_EMIT("vst1.32         {d6}, [%[dw]]!")
            __ASM_EMIT("vld1.32         {d10}, [%[dr]]!")
            __ASM_EMIT("vext.8          q3, q3, q4, $8")
            __ASM_EMIT("vext.8          q4, q4, q5, $8")
            __ASM_EMIT("subs            %[count], $1")
            __ASM_EMIT("bge             7b")
            __ASM_EMIT("vst1.32         {q3-q4}, [%[dw]]!")

            __ASM_EMIT("6:")
            : [dr] "+r" (dst), [dw] "+r" (dw), [src] "+r" (src),
              [count] "+r" (count)
            : [kernel] "r" (kernel)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    void lanczos_resample_2x3(float *dst, const float *src, size_t count)
    {
        IF_ARCH_ARM(
            float *dw = dst;
            const float *kernel = lanczos_2x3;
        );

        ARCH_ARM_ASM
        (
            // Prepare
            __ASM_EMIT("vld1.32         {q0-q1}, [%[kernel]]!")     // q0 = 0 k2 0 k1, q1 = 0 k0 1 k0
            __ASM_EMIT("subs            %[count], $8")
            __ASM_EMIT("vld1.32         {q2}, [%[kernel]]")         // q2 = 0 k1 0 k2
            __ASM_EMIT("blo             2f")

            // 8x blocks
            __ASM_EMIT("1:")
            // Even cycle: prepare
            __ASM_EMIT("vld2.32         {q4-q5}, [%[src]]!")        // q4 = s0 s2 s4 s6, q5 = s1 s3 s5 s7
            __ASM_EMIT("vmov            q6, q4")                    // q6 = s0 s2 s4 s6
            __ASM_EMIT("vmov            q7, q4")                    // q7 = s0 s2 s4 s6
            __ASM_EMIT("vmov            q8, q4")                    // q8 = s0 s2 s4 s6
            __ASM_EMIT("vld1.32         {q10-q11}, [%[dr]]!")
            __ASM_EMIT("vtrn.32         q4, q6")                    // q4 = s0 s0 s4 s4, q6 = s2 s2 s6 s6
            __ASM_EMIT("vtrn.32         q7, q8")                    // q7 = s0 s0 s4 s4, q8 = s2 s2 s6 s6
            __ASM_EMIT("vld1.32         {q12-q13}, [%[dr]]!")
            __ASM_EMIT("vswp            d9, d14")                   // q4 = s0, q6 = s2
            __ASM_EMIT("vld1.32         {q14-q15}, [%[dr]]")
            __ASM_EMIT("vswp            d13, d16")                  // q7 = s4, q8 = s6
            // Even cycle: convolve
            __ASM_EMIT("vmla.f32        q10, q4, q0")
            __ASM_EMIT("vmla.f32        q11, q6, q0")
            __ASM_EMIT("vmla.f32        q12, q7, q0")
            __ASM_EMIT("vmla.f32        q13, q8, q0")
            __ASM_EMIT("vmla.f32        q11, q4, q1")
            __ASM_EMIT("vmla.f32        q12, q6, q1")
            __ASM_EMIT("vst1.32         {q10-q11}, [%[dw]]!")
            __ASM_EMIT("vmla.f32        q13, q7, q1")
            __ASM_EMIT("vmla.f32        q14, q8, q1")
            __ASM_EMIT("vmla.f32        q12, q4, q2")
            __ASM_EMIT("vmla.f32        q13, q6, q2")
            __ASM_EMIT("vmla.f32        q14, q7, q2")
            __ASM_EMIT("vst1.32         {q12-q13}, [%[dw]]!")
            __ASM_EMIT("vmla.f32        q15, q8, q2")
            __ASM_EMIT("vst1.32         {q14-q15}, [%[dw]]")
            __ASM_EMIT("sub             %[dr], $0x38")
            __ASM_EMIT("sub             %[dw], $0x38")
            // Odd cycle: prepare
            __ASM_EMIT("vmov            q6, q5")                    // q6 = s0 s2 s4 s6
            __ASM_EMIT("vmov            q7, q5")                    // q7 = s0 s2 s4 s6
            __ASM_EMIT("vmov            q8, q5")                    // q8 = s0 s2 s4 s6
            __ASM_EMIT("vld1.32         {q10-q11}, [%[dr]]!")
            __ASM_EMIT("vtrn.32         q5, q6")                    // q5 = s0 s0 s4 s4, q6 = s2 s2 s6 s6
            __ASM_EMIT("vtrn.32         q7, q8")                    // q7 = s0 s0 s4 s4, q8 = s2 s2 s6 s6
            __ASM_EMIT("vld1.32         {q12-q13}, [%[dr]]!")
            __ASM_EMIT("vswp            d11, d14")                  // q5 = s0, q6 = s2
            __ASM_EMIT("vld1.32         {q14-q15}, [%[dr]]")
            __ASM_EMIT("vswp            d13, d16")                  // q7 = s4, q8 = s6
            // Odd cycle: convolve
            __ASM_EMIT("vmla.f32        q10, q5, q0")
            __ASM_EMIT("vmla.f32        q11, q6, q0")
            __ASM_EMIT("vmla.f32        q12, q7, q0")
            __ASM_EMIT("vmla.f32        q13, q8, q0")
            __ASM_EMIT("vmla.f32        q11, q5, q1")
            __ASM_EMIT("vmla.f32        q12, q6, q1")
            __ASM_EMIT("vst1.32         {q10-q11}, [%[dw]]!")
            __ASM_EMIT("vmla.f32        q13, q7, q1")
            __ASM_EMIT("vmla.f32        q14, q8, q1")
            __ASM_EMIT("vmla.f32        q12, q5, q2")
            __ASM_EMIT("vmla.f32        q13, q6, q2")
            __ASM_EMIT("vmla.f32        q14, q7, q2")
            __ASM_EMIT("vst1.32         {q12-q13}, [%[dw]]!")
            __ASM_EMIT("vmla.f32        q15, q8, q2")
            __ASM_EMIT("vst1.32         {q14-q15}, [%[dw]]")
            __ASM_EMIT("sub             %[dr], $0x08")
            __ASM_EMIT("sub             %[dw], $0x08")
            // Repeat loop
            __ASM_EMIT("subs            %[count], $8")
            __ASM_EMIT("bhs             1b")

            // 4x block
            __ASM_EMIT("2:")
            __ASM_EMIT("adds            %[count], $4")
            __ASM_EMIT("blt             4f")

            // Prepare
            __ASM_EMIT("vld1.32         {q4}, [%[src]]!")
            __ASM_EMIT("vmov            q5, q4")
            __ASM_EMIT("vmov            q6, q4")
            __ASM_EMIT("vmov            q7, q4")
            __ASM_EMIT("vld1.32         {q10-q11}, [%[dr]]!")
            __ASM_EMIT("vtrn.32         q4, q5")
            __ASM_EMIT("vtrn.32         q6, q7")
            __ASM_EMIT("vld1.32         {q12-q13}, [%[dr]]")
            __ASM_EMIT("vswp            d9, d12")
            __ASM_EMIT("vswp            d11, d14") // q4 = s0, q5 = s1, q6 = s2, q7 = s3
            // Even cycle: convolve
            __ASM_EMIT("vmla.f32        q10, q4, q0")
            __ASM_EMIT("vmla.f32        q11, q6, q0")
            __ASM_EMIT("vmla.f32        q11, q4, q1")
            __ASM_EMIT("vmla.f32        q12, q6, q1")
            __ASM_EMIT("vmla.f32        q12, q4, q2")
            __ASM_EMIT("vmla.f32        q13, q6, q2")
            __ASM_EMIT("vst1.32         {q10-q11}, [%[dw]]!")
            __ASM_EMIT("vst1.32         {q12-q13}, [%[dw]]")
            __ASM_EMIT("sub             %[dr], $0x18")
            __ASM_EMIT("sub             %[dw], $0x18")
            // Odd cycle: convolve
            __ASM_EMIT("vld1.32         {q10-q11}, [%[dr]]!")
            __ASM_EMIT("vld1.32         {q12-q13}, [%[dr]]")
            __ASM_EMIT("vmla.f32        q10, q5, q0")
            __ASM_EMIT("vmla.f32        q11, q7, q0")
            __ASM_EMIT("vmla.f32        q11, q5, q1")
            __ASM_EMIT("vmla.f32        q12, q7, q1")
            __ASM_EMIT("vmla.f32        q12, q5, q2")
            __ASM_EMIT("vmla.f32        q13, q7, q2")
            __ASM_EMIT("vst1.32         {q10-q11}, [%[dw]]!")
            __ASM_EMIT("vst1.32         {q12-q13}, [%[dw]]")
            __ASM_EMIT("sub             %[dr], $0x08")
            __ASM_EMIT("sub             %[dw], $0x08")
            // Even cycle: load/store and shift
            __ASM_EMIT("sub             %[count], $4")

            // 1x blocks
            __ASM_EMIT("4:")
            __ASM_EMIT("adds            %[count], $3")
            __ASM_EMIT("blt             6f")
            __ASM_EMIT("vld1.32         {q4-q5}, [%[dr]]!")
            __ASM_EMIT("vld1.32         {q6}, [%[dr]]")
            __ASM_EMIT("7:")
            __ASM_EMIT("vldm            %[src]!, {s12}")
            __ASM_EMIT("vmov            s13, s12")
            __ASM_EMIT("vmov            d7, d6")
            __ASM_EMIT("vmla.f32        q4, q3, q0")
            __ASM_EMIT("vmla.f32        q5, q3, q1")
            __ASM_EMIT("vmla.f32        q6, q3, q2")
            __ASM_EMIT("vst1.32         {d8}, [%[dw]]!")
            __ASM_EMIT("vld1.32         {d14}, [%[dr]]!")
            __ASM_EMIT("vext.8          q4, q4, q5, $8")
            __ASM_EMIT("vext.8          q5, q5, q6, $8")
            __ASM_EMIT("vext.8          q6, q6, q7, $8")
            __ASM_EMIT("subs            %[count], $1")
            __ASM_EMIT("bge             7b")
            __ASM_EMIT("vst1.32         {q4-q5}, [%[dw]]!")
            __ASM_EMIT("vst1.32         {q6}, [%[dw]]")

            __ASM_EMIT("6:")
            : [dr] "+r" (dst), [dw] "+r" (dw), [src] "+r" (src),
              [count] "+r" (count),
              [kernel] "+r" (kernel)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }
}

#endif /* DSP_ARCH_ARM_NEON_D32_RESAMPLING_H_ */
