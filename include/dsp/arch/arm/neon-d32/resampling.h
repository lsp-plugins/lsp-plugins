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
        -0.1664152316035080f  // k1
        +0.0000000000000000f,
        +0.6203830132406946f, // k0
        +1.0000000000000000f,
        +0.6203830132406946f, // k0
        +0.0000000000000000f,
        -0.1664152316035080f  // k1
    };

    void lanczos_resample_2x2(float *dst, const float *src, size_t count)
    {
        float *dw       = dst;

        ARCH_ARM_ASM
        (
            // Prepare
            __ASM_EMIT("subs            %[count], $8")
            __ASM_EMIT("vld1.32         {q0-q1}, [%[kernel]]")      // q0 = 0 k1 0 k0, q1 = 1 k0 0 k1
            __ASM_EMIT("blo             2f")

            // 8x blocks
            __ASM_EMIT("vld1.32         {q7-q8}, [%[dr]]!")
            __ASM_EMIT("vld1.32         {q9-q10}, [%[dr]]!")
            __ASM_EMIT("vld1.32         {q11}, [%[dr]]!")
            __ASM_EMIT("1:")
            // Even cycle: prepare
            __ASM_EMIT("vld2.32         {q2-q3}, [%[src]]!")        // q2 = s0 s2 s4 s6, q3 = s1 s3 s5 s7
            __ASM_EMIT("vmov            q4, q2")                    // q4 = s0 s2 s4 s6
            __ASM_EMIT("vmov            q5, q2")                    // q5 = s0 s2 s4 s6
            __ASM_EMIT("vmov            q6, q2")                    // q6 = s0 s2 s4 s6
            __ASM_EMIT("vtrn.32         q2, q4")                    // q2 = s0 s0 s4 s4, q4 = s2 s2 s6 s6
            __ASM_EMIT("vtrn.32         q5, q6")                    // q5 = s0 s0 s4 s4, q6 = s2 s2 s6 s6
            __ASM_EMIT("vswp            d5, d10")                   // q2 = s0, q4 = s2
            __ASM_EMIT("vswp            d9, d12")                   // q5 = s4, q6 = s6
            // Even cycle: convolve
            __ASM_EMIT("vmla.f32        q7, q2, q0")
            __ASM_EMIT("vmla.f32        q8, q4, q0")
            __ASM_EMIT("vmla.f32        q9, q5, q0")
            __ASM_EMIT("vmla.f32        q10, q6, q0")
            __ASM_EMIT("vmla.f32        q8, q2, q1")
            __ASM_EMIT("vmla.f32        q9, q4, q1")
            __ASM_EMIT("vmla.f32        q10, q5, q1")
            __ASM_EMIT("vmla.f32        q11, q6, q1")
            // Even cycle: load/store and shift
            __ASM_EMIT("vst1.32         {d14}, [%[dw]]!")
            __ASM_EMIT("vld1.32         {d4}, [%[dr]]!")
            __ASM_EMIT("vext.8          q7, q7, q8, $8")
            __ASM_EMIT("vext.8          q8, q8, q9, $8")
            __ASM_EMIT("vext.8          q9, q9, q10, $8")
            __ASM_EMIT("vext.8          q10, q10, q11, $8")
            __ASM_EMIT("vext.8          q11, q11, q2, $8")
            // Odd cycle: prepare
            __ASM_EMIT("vmov            q4, q3")
            __ASM_EMIT("vmov            q5, q3")
            __ASM_EMIT("vmov            q6, q4")
            __ASM_EMIT("vtrn.32         q3, q4")
            __ASM_EMIT("vtrn.32         q5, q6")
            __ASM_EMIT("vswp            d7, d10")
            __ASM_EMIT("vswp            d9, d12")
            // Odd cycle: convolve
            __ASM_EMIT("vmla.f32        q7, q3, q0")
            __ASM_EMIT("vmla.f32        q8, q4, q0")
            __ASM_EMIT("vmla.f32        q9, q5, q0")
            __ASM_EMIT("vmla.f32        q10, q6, q0")
            __ASM_EMIT("vmla.f32        q8, q3, q1")
            __ASM_EMIT("vmla.f32        q9, q4, q1")
            __ASM_EMIT("vmla.f32        q10, q5, q1")
            __ASM_EMIT("vmla.f32        q11, q6, q1")
            // Odd cycle: load/store and shift
            __ASM_EMIT("vst1.32         {d14}, [%[dw]]!")
            __ASM_EMIT("vext.8          q7, q7, q8, $8")
            __ASM_EMIT("vext.8          q8, q8, q9, $8")
            __ASM_EMIT("vld1.32         {d4}, [%[dr]]!")
            __ASM_EMIT("vext.8          q9, q9, q10, $8")
            __ASM_EMIT("vext.8          q10, q10, q11, $8")
            __ASM_EMIT("vext.8          q11, q11, q2, $8")
            // Repeat loop
            __ASM_EMIT("subs            %[count], $8")
            __ASM_EMIT("bhs             1b")
            __ASM_EMIT("vst1.32         {q7-q8}, [%[dw]]!")
            __ASM_EMIT("vst1.32         {q9-q10}, [%[dw]]!")
            __ASM_EMIT("vst1.32         {q11}, [%[dw]]")
            __ASM_EMIT("sub             %[dr], $0x50")
            __ASM_EMIT("sub             %[dw], $0x40")

            // 4x block
            __ASM_EMIT("2:")
            __ASM_EMIT("adds            %[count], $4")
            __ASM_EMIt("blt             4f")

            __ASM_EMIT("vld1.32         {q7-q8}, [%[dr]]!")
            __ASM_EMIT("vld1.32         {q9}, [%[dr]]!")
            // Prepare
            __ASM_EMIT("vld1.32         {q2}, [%[src]]!")
            __ASM_EMIT("vmov            q3, q2")
            __ASM_EMIT("vmov            q4, q2")
            __ASM_EMIT("vmov            q5, q2")
            __ASM_EMIT("vtrn.32         q2, q3")
            __ASM_EMIT("vtrn.32         q4, q5")
            __ASM_EMIT("vswp            d5, d8")
            __ASM_EMIT("vswp            d7, d10") // q2 = s0, q3 = s1, q4 = s2, q5 = s3
            // Even cycle: convolve
            __ASM_EMIT("vmla.f32        q7, q2, q0")
            __ASM_EMIT("vmla.f32        q8, q4, q0")
            __ASM_EMIT("vmla.f32        q8, q2, q1")
            __ASM_EMIT("vmla.f32        q9, q4, q1")
            // Even cycle: load/store and shift
            __ASM_EMIT("vst1.32         {d14}, [%[dw]]!")
            __ASM_EMIT("vext.8          q7, q7, q8, $8")
            __ASM_EMIT("vld1.32         {d4}, [%[dr]]!")
            __ASM_EMIT("vext.8          q8, q8, q9, $8")
            __ASM_EMIT("vext.8          q9, q9, q2, $8")
            // Odd cycle: convolve
            __ASM_EMIT("vmla.f32        q7, q3, q0")
            __ASM_EMIT("vmla.f32        q8, q5, q0")
            __ASM_EMIT("vmla.f32        q8, q3, q1")
            __ASM_EMIT("vmla.f32        q9, q5, q1")
            // Even cycle: load/store and shift
            __ASM_EMIT("vst1.32         {d14}, [%[dw]]!")
            __ASM_EMIT("vext.8          q7, q7, q8, $8")
            __ASM_EMIT("vld1.32         {d4}, [%[dr]]!")
            __ASM_EMIT("vext.8          q8, q8, q9, $8")
            __ASM_EMIT("vext.8          q9, q9, q2, $8")
            __ASM_EMIT("vst1.32         {q7-q8}, [%[dw]]!")
            __ASM_EMIT("sub             %[count], $4")
            __ASM_EMIT("vst1.32         {q9}, [%[dw]]")
            __ASM_EMIT("sub             %[dr], $0x30")
            __ASM_EMIT("sub             %[dw], $0x20")

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
            __ASM_EMIT("vld1.32         {d9}, [%[dr]]!")
            __ASM_EMIT("sub             %[count], $1")
            __ASM_EMIT("bge             7b")
            __ASM_EMIT("vst1.32         {q3-q4}, [%[dw]]!")

            __ASM_EMIT("6:")
            : [dr] "+r" (dst), [dw] "+r" (dw), [src] "+r" (src),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }
}

#endif /* DSP_ARCH_ARM_NEON_D32_RESAMPLING_H_ */
