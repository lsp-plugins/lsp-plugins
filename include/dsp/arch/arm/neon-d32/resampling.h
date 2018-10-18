/*
 * resampling.h
 *
 *  Created on: 4 окт. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_ARM_NEON_D32_RESAMPLING_H_
#define DSP_ARCH_ARM_NEON_D32_RESAMPLING_H_

#ifndef DSP_ARCH_ARM_NEON_32_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_ARM_NEON_32_IMPL */

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

    void lanczos_resample_2x2(float *dst, const float *src, size_t count)
    {
        IF_ARCH_ARM(
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
            __ASM_EMIT("vldm            %[dst], {d14-d23}")
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
            __ASM_EMIT("vstm            %[dst], {d14-d23}")
            __ASM_EMIT("add             %[dst], $0x08")
            // Odd cycle: prepare
            __ASM_EMIT("vmov            q4, q3")
            __ASM_EMIT("vmov            q5, q3")
            __ASM_EMIT("vmov            q6, q4")
            __ASM_EMIT("vldm            %[dst], {d14-d23}")
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
            __ASM_EMIT("vstm            %[dst], {d14-d23}")
            __ASM_EMIT("add             %[dst], $0x38")
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
            __ASM_EMIT("vldm            %[dst], {d14-d19}")
            __ASM_EMIT("vtrn.32         q2, q3")
            __ASM_EMIT("vtrn.32         q4, q5")
            __ASM_EMIT("vswp            d5, d8")
            __ASM_EMIT("vswp            d7, d10") // q2 = s0, q3 = s1, q4 = s2, q5 = s3
            // Even cycle: convolve
            __ASM_EMIT("vmla.f32        q7, q2, q0")
            __ASM_EMIT("vmla.f32        q8, q4, q0")
            __ASM_EMIT("vmla.f32        q8, q2, q1")
            __ASM_EMIT("vmla.f32        q9, q4, q1")
            __ASM_EMIT("vstm            %[dst], {d14-d19}")
            __ASM_EMIT("add             %[dst], $0x08")
            // Odd cycle: convolve
            __ASM_EMIT("vldm            %[dst], {d14-d19}")
            __ASM_EMIT("vmla.f32        q7, q3, q0")
            __ASM_EMIT("vmla.f32        q8, q5, q0")
            __ASM_EMIT("vmla.f32        q8, q3, q1")
            __ASM_EMIT("vmla.f32        q9, q5, q1")
            __ASM_EMIT("vstm            %[dst], {d14-d19}")
            __ASM_EMIT("sub             %[count], $4")
            __ASM_EMIT("add             %[dst], $0x18")

            // 1x blocks
            __ASM_EMIT("4:")
            __ASM_EMIT("adds            %[count], $3")
            __ASM_EMIT("blt             6f")
            __ASM_EMIT("7:")
            __ASM_EMIT("vldm            %[src]!, {s8}")
            __ASM_EMIT("vldm            %[dst], {d6-d9}")
            __ASM_EMIT("vmov            s9, s8")
            __ASM_EMIT("vmov            d5, d4")
            __ASM_EMIT("vmla.f32        q3, q2, q0")
            __ASM_EMIT("vmla.f32        q4, q2, q1")
            __ASM_EMIT("vstm            %[dst], {d6-d9}")
            __ASM_EMIT("subs            %[count], $1")
            __ASM_EMIT("add             %[dst], $0x08")
            __ASM_EMIT("bge             7b")

            __ASM_EMIT("6:")
            : [dst] "+r" (dst), [src] "+r" (src),
              [count] "+r" (count)
            : [kernel] "r" (kernel)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

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

    void lanczos_resample_2x3(float *dst, const float *src, size_t count)
    {
        IF_ARCH_ARM(
            const float *kernel = lanczos_2x3;
        );

        ARCH_ARM_ASM
        (
            // Prepare
            __ASM_EMIT("vldm            %[kernel], {q0-q2}")
            __ASM_EMIT("subs            %[count], $8")
            __ASM_EMIT("blo             2f")

            // 8x blocks
            __ASM_EMIT("1:")
            // Even cycle: prepare
            __ASM_EMIT("vld2.32         {q4-q5}, [%[src]]!")        // q4 = s0 s2 s4 s6, q5 = s1 s3 s5 s7
            __ASM_EMIT("vmov            q6, q4")                    // q6 = s0 s2 s4 s6
            __ASM_EMIT("vmov            q7, q4")                    // q7 = s0 s2 s4 s6
            __ASM_EMIT("vmov            q8, q4")                    // q8 = s0 s2 s4 s6
            __ASM_EMIT("vldm            %[dst], {q10-q15}")
            __ASM_EMIT("vtrn.32         q4, q6")                    // q4 = s0 s0 s4 s4, q6 = s2 s2 s6 s6
            __ASM_EMIT("vtrn.32         q7, q8")                    // q7 = s0 s0 s4 s4, q8 = s2 s2 s6 s6
            __ASM_EMIT("vswp            d9, d14")                   // q4 = s0, q6 = s2
            __ASM_EMIT("vswp            d13, d16")                  // q7 = s4, q8 = s6
            // Even cycle: convolve
            __ASM_EMIT("vmla.f32        q10, q4, q0")
            __ASM_EMIT("vmla.f32        q11, q6, q0")
            __ASM_EMIT("vmla.f32        q12, q7, q0")
            __ASM_EMIT("vmla.f32        q13, q8, q0")
            __ASM_EMIT("vmla.f32        q11, q4, q1")
            __ASM_EMIT("vmla.f32        q12, q6, q1")
            __ASM_EMIT("vmla.f32        q13, q7, q1")
            __ASM_EMIT("vmla.f32        q14, q8, q1")
            __ASM_EMIT("vmla.f32        q12, q4, q2")
            __ASM_EMIT("vmla.f32        q13, q6, q2")
            __ASM_EMIT("vmla.f32        q14, q7, q2")
            __ASM_EMIT("vmla.f32        q15, q8, q2")
            __ASM_EMIT("vstm            %[dst], {q10-q15}")
            __ASM_EMIT("add             %[dst], $0x08")
            // Odd cycle: prepare
            __ASM_EMIT("vmov            q6, q5")                    // q6 = s0 s2 s4 s6
            __ASM_EMIT("vmov            q7, q5")                    // q7 = s0 s2 s4 s6
            __ASM_EMIT("vmov            q8, q5")                    // q8 = s0 s2 s4 s6
            __ASM_EMIT("vldm            %[dst], {q10-q15}")
            __ASM_EMIT("vtrn.32         q5, q6")                    // q5 = s0 s0 s4 s4, q6 = s2 s2 s6 s6
            __ASM_EMIT("vtrn.32         q7, q8")                    // q7 = s0 s0 s4 s4, q8 = s2 s2 s6 s6
            __ASM_EMIT("vswp            d11, d14")                  // q5 = s0, q6 = s2
            __ASM_EMIT("vswp            d13, d16")                  // q7 = s4, q8 = s6
            // Odd cycle: convolve
            __ASM_EMIT("vmla.f32        q10, q5, q0")
            __ASM_EMIT("vmla.f32        q11, q6, q0")
            __ASM_EMIT("vmla.f32        q12, q7, q0")
            __ASM_EMIT("vmla.f32        q13, q8, q0")
            __ASM_EMIT("vmla.f32        q11, q5, q1")
            __ASM_EMIT("vmla.f32        q12, q6, q1")
            __ASM_EMIT("vmla.f32        q13, q7, q1")
            __ASM_EMIT("vmla.f32        q14, q8, q1")
            __ASM_EMIT("vmla.f32        q12, q5, q2")
            __ASM_EMIT("vmla.f32        q13, q6, q2")
            __ASM_EMIT("vmla.f32        q14, q7, q2")
            __ASM_EMIT("vmla.f32        q15, q8, q2")
            __ASM_EMIT("vstm            %[dst], {q10-q15}")
            __ASM_EMIT("subs            %[count], $8")
            __ASM_EMIT("add             %[dst], $0x38")
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
            __ASM_EMIT("vldm            %[dst], {q10-q13}")
            __ASM_EMIT("vtrn.32         q4, q5")
            __ASM_EMIT("vtrn.32         q6, q7")
            __ASM_EMIT("vswp            d9, d12")
            __ASM_EMIT("vswp            d11, d14") // q4 = s0, q5 = s1, q6 = s2, q7 = s3
            // Even cycle: convolve
            __ASM_EMIT("vmla.f32        q10, q4, q0")
            __ASM_EMIT("vmla.f32        q11, q6, q0")
            __ASM_EMIT("vmla.f32        q11, q4, q1")
            __ASM_EMIT("vmla.f32        q12, q6, q1")
            __ASM_EMIT("vmla.f32        q12, q4, q2")
            __ASM_EMIT("vmla.f32        q13, q6, q2")
            __ASM_EMIT("vstm            %[dst], {q10-q13}")
            __ASM_EMIT("add             %[dst], $0x08")
            // Odd cycle: convolve
            __ASM_EMIT("vldm            %[dst], {q10-q13}")
            __ASM_EMIT("vmla.f32        q10, q5, q0")
            __ASM_EMIT("vmla.f32        q11, q7, q0")
            __ASM_EMIT("vmla.f32        q11, q5, q1")
            __ASM_EMIT("vmla.f32        q12, q7, q1")
            __ASM_EMIT("vmla.f32        q12, q5, q2")
            __ASM_EMIT("vmla.f32        q13, q7, q2")
            __ASM_EMIT("vstm            %[dst], {q10-q13}")
            __ASM_EMIT("sub             %[count], $4")
            __ASM_EMIT("add             %[dst], $0x18")

            // 1x blocks
            __ASM_EMIT("4:")
            __ASM_EMIT("adds            %[count], $3")
            __ASM_EMIT("blt             6f")
            __ASM_EMIT("7:")
            __ASM_EMIT("vldm            %[src]!, {s12}")
            __ASM_EMIT("vldm            %[dst], {q4-q6}")
            __ASM_EMIT("vmov            s13, s12")
            __ASM_EMIT("vmov            d7, d6")
            __ASM_EMIT("vmla.f32        q4, q3, q0")
            __ASM_EMIT("vmla.f32        q5, q3, q1")
            __ASM_EMIT("vmla.f32        q6, q3, q2")
            __ASM_EMIT("vstm            %[dst], {q4-q6}")
            __ASM_EMIT("subs            %[count], $1")
            __ASM_EMIT("add             %[dst], $0x08")
            __ASM_EMIT("bge             7b")

            __ASM_EMIT("6:")
            : [dst] "+r" (dst), [src] "+r" (src),
              [count] "+r" (count)
            : [kernel] "r" (kernel)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    static const float lanczos_kernel_3x2[] __lsp_aligned16 =
    {
        +0.0000000000000000f,
        -0.1451906347823569f,
        -0.1903584501504231f,
        +0.0000000000000000f,

        +0.4051504629060886f,
        +0.8228011237053413f,
        +1.0000000000000000f,
        +0.8228011237053413f,

        +0.4051504629060886f,
        +0.0000000000000000f,
        -0.1903584501504231f,
        -0.1451906347823569f,

        // Shifted by 1 left
        -0.1451906347823569f,
        -0.1903584501504231f,
        +0.0000000000000000f,
        +0.4051504629060886f,

        +0.8228011237053413f,
        +1.0000000000000000f,
        +0.8228011237053413f,
        +0.4051504629060886f,

        +0.0000000000000000f,
        -0.1903584501504231f,
        -0.1451906347823569f,
        +0.0000000000000000f
    };

    void lanczos_resample_3x2(float *dst, const float *src, size_t count)
    {
        IF_ARCH_ARM(
            const float *kernel = lanczos_kernel_3x2;
        );

        ARCH_ARM_ASM
        (
            // Prepare
            __ASM_EMIT("vldm            %[kernel], {q2-q7}")
            __ASM_EMIT("subs            %[count], $2")
            __ASM_EMIT("blo             2f")

            // 2x blocks
            __ASM_EMIT("1:")
            // Even cycle: prepare
            __ASM_EMIT("vld1.32         {d0}, [%[src]]!")           // d0 = s1 s2
            __ASM_EMIT("vmov            d1, d0")                    // q0 = s1 s2 s1 s2
            __ASM_EMIT("vldm            %[dst], {q8-q11}")
            __ASM_EMIT("vmov            q1, q0")                    // q1 = s1 s2 s1 s2
            __ASM_EMIT("vtrn.32         q0, q1")                    // q0 = s1 s1 s1 s1, q1 = s2 s2 s2 s2
            __ASM_EMIT("vmla.f32        q8, q2, q0")
            __ASM_EMIT("vmla.f32        q9, q3, q0")
            __ASM_EMIT("vmla.f32        q10, q4, q0")
            __ASM_EMIT("vmla.f32        q9, q5, q1")
            __ASM_EMIT("vmla.f32        q10, q6, q1")
            __ASM_EMIT("vmla.f32        q11, q7, q1")
            __ASM_EMIT("vstm            %[dst], {q8-q11}")
            __ASM_EMIT("subs            %[count], $2")
            __ASM_EMIT("add             %[dst], $0x18")
            __ASM_EMIT("bhs             1b")

            // 1x block
            __ASM_EMIT("2:")
            __ASM_EMIT("adds            %[count], $1")
            __ASM_EMIT("blt             4f")

            __ASM_EMIT("vldm            %[src], {s0}")
            __ASM_EMIT("vdup.32         q0, d0[0]")
            __ASM_EMIT("vldm            %[dst], {q8-q10}")
            __ASM_EMIT("vmla.f32        q8, q2, q0")
            __ASM_EMIT("vmla.f32        q9, q3, q0")
            __ASM_EMIT("vmla.f32        q10, q4, q0")
            __ASM_EMIT("vstm            %[dst], {q8-q10}")

            __ASM_EMIT("4:")
            : [dst] "+r" (dst), [src] "+r" (src),
              [count] "+r" (count)
            : [kernel] "r" (kernel)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    static const float lanczos_kernel_3x3[] __lsp_aligned16 =
    {
        // Middle part
        +0.8251285571768335f,
        +1.0000000000000000f,
        +0.8251285571768335f,
        +0.4097746665281485f,

        +0.0000000000000000f,
        -0.1993645686793863f,
        -0.1562250559899557f,
        +0.0000000000000000f,

        +0.1055060549370832f,
        +0.0890793429479492f,
        +0.0000000000000000f,
        +0.0000000000000000f,

        +0.0890793429479492f,
        +0.1055060549370832f,
        +0.0000000000000000f,
        -0.1562250559899557f,

        -0.1993645686793863f,
        +0.0000000000000000f,
        +0.4097746665281485f,
        +0.8251285571768335f,

        +1.0000000000000000f,
        +0.8251285571768335f,
        +0.4097746665281485f,
        +0.0000000000000000f,

        // Head part
        +0.0000000000000000f,
        +0.0890793429479492f,
        +0.1055060549370832f,
        +0.0000000000000000f,

        -0.1562250559899557f,
        -0.1993645686793863f,
        +0.0000000000000000f,
        +0.4097746665281485f,

        // Tail part
        -0.1993645686793863f,
        -0.1562250559899557f,
        +0.0000000000000000f,
        +0.1055060549370832f,

        +0.0890793429479492f,
        +0.0000000000000000f,
        +0.0000000000000000f,
        +0.0000000000000000f
    };

    void lanczos_resample_3x3(float *dst, const float *src, size_t count)
    {
        IF_ARCH_ARM(
            const float *kernel = lanczos_kernel_3x3;
        );

        ARCH_ARM_ASM
        (
            // Prepare
            __ASM_EMIT("vldm            %[kernel]!, {q10-q15}")
            __ASM_EMIT("subs            %[count], $2")
            __ASM_EMIT("blo             2f")

            // 2x blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("vld1.32         {d0}, [%[src]]!")           // d0 = s1 s2
            __ASM_EMIT("vmov            d1, d0")                    // q0 = s1 s2 s1 s2
            __ASM_EMIT("vldm            %[dst], {q2-q7}")
            __ASM_EMIT("vmov            q1, q0")                    // q1 = s1 s2 s1 s2
            __ASM_EMIT("vld1.32         {q8-q9}, [%[kernel]]!")
            __ASM_EMIT("vtrn.32         q0, q1")                    // q0 = s1 s1 s1 s1, q1 = s2 s2 s2 s2

            __ASM_EMIT("vmla.f32        q2, q8, q0")
            __ASM_EMIT("vmla.f32        q3, q9, q0")
            __ASM_EMIT("vmla.f32        q4, q10, q0")
            __ASM_EMIT("vmla.f32        q3, q13, q1")
            __ASM_EMIT("vmla.f32        q4, q14, q1")
            __ASM_EMIT("vmla.f32        q5, q11, q0")
            __ASM_EMIT("vld1.32         {q8-q9}, [%[kernel]]")
            __ASM_EMIT("vmla.f32        q6, q12, q0")
            __ASM_EMIT("vmla.f32        q5, q15, q1")
            __ASM_EMIT("vmla.f32        q6, q8, q1")
            __ASM_EMIT("vmla.f32        q7, q9, q1")
            __ASM_EMIT("vstm            %[dst], {q2-q7}")
            __ASM_EMIT("sub             %[kernel], $0x20")
            __ASM_EMIT("subs            %[count], $2")
            __ASM_EMIT("add             %[dst], $0x18")
            __ASM_EMIT("bhs             1b")

            // 1x block
            __ASM_EMIT("2:")
            __ASM_EMIT("adds            %[count], $1")
            __ASM_EMIT("blt             4f")

            __ASM_EMIT("vldm            %[src], {s0}")
            __ASM_EMIT("vdup.32         q0, d0[0]")
            __ASM_EMIT("vldm            %[dst], {q2-q6}")
            __ASM_EMIT("vld1.32         {q8-q9}, [%[kernel]]")

            __ASM_EMIT("vmla.f32        q2, q8, q0")
            __ASM_EMIT("vmla.f32        q3, q9, q0")
            __ASM_EMIT("vmla.f32        q4, q10, q0")
            __ASM_EMIT("vmla.f32        q5, q11, q0")
            __ASM_EMIT("vmla.f32        q6, q12, q0")
            __ASM_EMIT("vstm            %[dst], {q2-q6}")

            __ASM_EMIT("4:")
            : [dst] "+r" (dst), [src] "+r" (src),
              [count] "+r" (count), [kernel] "+r" (kernel)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    static const float lanczos_kernel_4x2[] __lsp_aligned16 =
    {
        +0.0000000000000000f,
        -0.1187292496637064f,
        -0.2001458972657618f,
        -0.1729186314209981f,

        +0.0000000000000000f,
        +0.2957854651930789f,
        +0.6325370350028462f,
        +0.8988707620097378f,

        +1.0000000000000000f,
        +0.8988707620097378f,
        +0.6325370350028462f,
        +0.2957854651930789f,

        +0.0000000000000000f,
        -0.1729186314209981f,
        -0.2001458972657618f,
        -0.1187292496637064f
    };

    void lanczos_resample_4x2(float *dst, const float *src, size_t count)
    {
        IF_ARCH_ARM(
            const float *kernel = lanczos_kernel_4x2;
        );

        ARCH_ARM_ASM
        (
            // Prepare
            __ASM_EMIT("vldm            %[kernel], {d24-d31}")
            __ASM_EMIT("subs            %[count], $4")
            __ASM_EMIT("blo             2f")

            // 4x blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("vldm            %[src]!, {d0-d1}")
            __ASM_EMIT("vmov            q1, q0")
            __ASM_EMIT("vldm            %[dst], {d8-d21}")
            __ASM_EMIT("vmov            q2, q0")
            __ASM_EMIT("vmov            q3, q1")
            __ASM_EMIT("vtrn.32         q0, q1")
            __ASM_EMIT("vtrn.32         q2, q3")
            __ASM_EMIT("vswp            d1, d4")
            __ASM_EMIT("vswp            d3, d6")

            __ASM_EMIT("vmla.f32        q4, q12, q0")
            __ASM_EMIT("vmla.f32        q5, q13, q0")
            __ASM_EMIT("vmla.f32        q6, q14, q0")
            __ASM_EMIT("vmla.f32        q7, q15, q0")
            __ASM_EMIT("vmla.f32        q5, q12, q1")
            __ASM_EMIT("vmla.f32        q6, q13, q1")
            __ASM_EMIT("vmla.f32        q7, q14, q1")
            __ASM_EMIT("vmla.f32        q8, q15, q1")
            __ASM_EMIT("vmla.f32        q6, q12, q2")
            __ASM_EMIT("vmla.f32        q7, q13, q2")
            __ASM_EMIT("vmla.f32        q8, q14, q2")
            __ASM_EMIT("vmla.f32        q9, q15, q2")
            __ASM_EMIT("vmla.f32        q7, q12, q3")
            __ASM_EMIT("vmla.f32        q8, q13, q3")
            __ASM_EMIT("vmla.f32        q9, q14, q3")
            __ASM_EMIT("vmla.f32        q10, q15, q3")
            __ASM_EMIT("vstm            %[dst], {d8-d21}")
            __ASM_EMIT("subs            %[count], $4")
            __ASM_EMIT("add             %[dst], $0x40")
            __ASM_EMIT("bhs             1b")

            // 2x block
            __ASM_EMIT("2:")
            __ASM_EMIT("adds            %[count], $2")
            __ASM_EMIT("blt             4f")

            __ASM_EMIT("vldm            %[src]!, {d0}")
            __ASM_EMIT("vmov            d1, d0")
            __ASM_EMIT("vldm            %[dst], {d8-d17}")
            __ASM_EMIT("vmov            q1, q0")
            __ASM_EMIT("vtrn.32         q0, q1")

            __ASM_EMIT("vmla.f32        q4, q12, q0")
            __ASM_EMIT("vmla.f32        q5, q13, q0")
            __ASM_EMIT("vmla.f32        q6, q14, q0")
            __ASM_EMIT("vmla.f32        q7, q15, q0")
            __ASM_EMIT("vmla.f32        q5, q12, q1")
            __ASM_EMIT("vmla.f32        q6, q13, q1")
            __ASM_EMIT("vmla.f32        q7, q14, q1")
            __ASM_EMIT("vmla.f32        q8, q15, q1")
            __ASM_EMIT("vstm            %[dst], {d8-d17}")
            __ASM_EMIT("sub             %[count], $2")
            __ASM_EMIT("add             %[dst], $0x20")

            // 1x block
            __ASM_EMIT("4:")
            __ASM_EMIT("adds            %[count], $1")
            __ASM_EMIT("blt             6f")

            __ASM_EMIT("vldm            %[src], {s0}")
            __ASM_EMIT("vdup.32         q0, d0[0]")
            __ASM_EMIT("vldm            %[dst], {d8-d15}")
            __ASM_EMIT("vmla.f32        q4, q12, q0")
            __ASM_EMIT("vmla.f32        q5, q13, q0")
            __ASM_EMIT("vmla.f32        q6, q14, q0")
            __ASM_EMIT("vmla.f32        q7, q15, q0")
            __ASM_EMIT("vstm            %[dst], {d8-d15}")

            __ASM_EMIT("6:")
            : [dst] "+r" (dst), [src] "+r" (src),
              [count] "+r" (count)
            : [kernel] "r" (kernel)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    static const float lanczos_kernel_4x3[] __lsp_aligned16 =
    {
        +0.0000000000000000f,
        +0.0749573750027847f,
        +0.1184264228024920f,
        +0.0943496807888575f,

        +0.0000000000000000f,
        -0.1241641556633335f,
        -0.2067943377468982f,
        -0.1768665375130828f,

        +0.0000000000000000f,
        +0.2981808178680967f,
        +0.6348032780190205f,
        +0.8996736756130924f,

        +1.0000000000000000f,
        +0.8996736756130924f,
        +0.6348032780190205f,
        +0.2981808178680967f,

        +0.0000000000000000f,
        -0.1768665375130828f,
        -0.2067943377468982f,
        -0.1241641556633335f,

        +0.0000000000000000f,
        +0.0943496807888575f,
        +0.1184264228024920f,
        +0.0749573750027847f,
    };

    void lanczos_resample_4x3(float *dst, const float *src, size_t count)
    {
        IF_ARCH_ARM(
            const float *kernel = lanczos_kernel_4x3;
        );

        ARCH_ARM_ASM
        (
            // Prepare
            __ASM_EMIT("vldm            %[kernel], {q10-q15}")
            __ASM_EMIT("subs            %[count], $4")
            __ASM_EMIT("blo             2f")

            // 4x blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("vldm            %[src]!, {q0}")
            __ASM_EMIT("vmov            q1, q0")
            __ASM_EMIT("vmov            q2, q0")
            __ASM_EMIT("vmov            q3, q1")
            __ASM_EMIT("vtrn.32         q0, q1")
            __ASM_EMIT("vtrn.32         q2, q3")
            __ASM_EMIT("vswp            d1, d4")
            __ASM_EMIT("vswp            d3, d6")

            // Part 1 - process 10 multiplications
            __ASM_EMIT("vldm            %[dst], {q4-q7}")
            __ASM_EMIT("vmla.f32        q4, q10, q0")
            __ASM_EMIT("vmla.f32        q5, q11, q0")
            __ASM_EMIT("vmla.f32        q6, q12, q0")
            __ASM_EMIT("vmla.f32        q7, q13, q0")
            __ASM_EMIT("vmla.f32        q6, q11, q1")
            __ASM_EMIT("vmla.f32        q7, q11, q2")
            __ASM_EMIT("vmla.f32        q6, q10, q2")
            __ASM_EMIT("vmla.f32        q7, q10, q3")
            __ASM_EMIT("vmla.f32        q5, q10, q1")
            __ASM_EMIT("vmla.f32        q7, q12, q1")
            __ASM_EMIT("vstm            %[dst]!, {q4-q7}")
            // Part 2 - process 8 multiplications
            __ASM_EMIT("vldm            %[dst], {q4-q5}")
            __ASM_EMIT("vmla.f32        q4, q14, q0")
            __ASM_EMIT("vmla.f32        q5, q15, q0")
            __ASM_EMIT("vmla.f32        q4, q13, q1")
            __ASM_EMIT("vmla.f32        q5, q14, q1")
            __ASM_EMIT("vmla.f32        q4, q12, q2")
            __ASM_EMIT("vmla.f32        q5, q13, q2")
            __ASM_EMIT("vmla.f32        q4, q11, q3")
            __ASM_EMIT("vmla.f32        q5, q12, q3")
            __ASM_EMIT("vstm            %[dst]!, {q4-q5}")
            // Part 3 - process 6 multiplications
            __ASM_EMIT("vldm            %[dst], {q4-q6}")
            __ASM_EMIT("vmla.f32        q4, q15, q1")
            __ASM_EMIT("vmla.f32        q5, q15, q2")
            __ASM_EMIT("vmla.f32        q4, q14, q2")
            __ASM_EMIT("vmla.f32        q5, q14, q3")
            __ASM_EMIT("vmla.f32        q4, q13, q3")
            __ASM_EMIT("vmla.f32        q6, q15, q3")
            __ASM_EMIT("vstm            %[dst], {q4-q6}")
            __ASM_EMIT("subs            %[count], $4")
            __ASM_EMIT("sub             %[dst], $0x20")
            __ASM_EMIT("bhs             1b")

            // 2x block
            __ASM_EMIT("2:")
            __ASM_EMIT("adds            %[count], $2")
            __ASM_EMIT("blt             4f")

            __ASM_EMIT("vldm            %[src]!, {d0}")
            __ASM_EMIT("vmov            d1, d0")
            __ASM_EMIT("vmov            q1, q0")
            __ASM_EMIT("vtrn.32         q0, q1")

            // Part 1 - 7 multiplications
            __ASM_EMIT("vldm            %[dst], {q4-q7}")
            __ASM_EMIT("vmla.f32        q4, q10, q0")
            __ASM_EMIT("vmla.f32        q5, q10, q1")
            __ASM_EMIT("vmla.f32        q5, q11, q0")
            __ASM_EMIT("vmla.f32        q6, q11, q1")
            __ASM_EMIT("vmla.f32        q7, q12, q1")
            __ASM_EMIT("vmla.f32        q6, q12, q0")
            __ASM_EMIT("vmla.f32        q7, q13, q0")
            __ASM_EMIT("vstm            %[dst]!, {q4-q7}")

            // Part 2 - 5 multiplications
            __ASM_EMIT("vldm            %[dst], {q4-q6}")
            __ASM_EMIT("vmla.f32        q4, q14, q0")
            __ASM_EMIT("vmla.f32        q5, q14, q1")
            __ASM_EMIT("vmla.f32        q4, q13, q1")
            __ASM_EMIT("vmla.f32        q5, q15, q0")
            __ASM_EMIT("vmla.f32        q6, q15, q1")
            __ASM_EMIT("vstm            %[dst], {q4-q6}")
            __ASM_EMIT("sub             %[count], $2")
            __ASM_EMIT("sub             %[dst], $0x20")

            // 1x block
            __ASM_EMIT("4:")
            __ASM_EMIT("adds            %[count], $1")
            __ASM_EMIT("blt             6f")

            __ASM_EMIT("vldm            %[src], {s0}")
            __ASM_EMIT("vdup.32         q0, d0[0]")
            __ASM_EMIT("vldm            %[dst], {q4-q9}")
            __ASM_EMIT("vmla.f32        q4, q0, q10")
            __ASM_EMIT("vmla.f32        q5, q0, q11")
            __ASM_EMIT("vmla.f32        q6, q0, q12")
            __ASM_EMIT("vmla.f32        q7, q0, q13")
            __ASM_EMIT("vmla.f32        q8, q0, q14")
            __ASM_EMIT("vmla.f32        q9, q0, q15")
            __ASM_EMIT("vstm            %[dst], {q4-q9}")

            __ASM_EMIT("6:")
            : [dst] "+r" (dst), [src] "+r" (src),
              [count] "+r" (count)
            : [kernel] "r" (kernel)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    static const float lanczos_kernel_6x2[] __lsp_aligned16 =
    {
        +0.0000000000000000f,
        -0.0835168749111343f,
        -0.1602001404590001f,
        -0.2067943377468982f,

        -0.2025752314530442f,
        -0.1343073344351001f,
        +0.0000000000000000f,
        +0.1894744904080251f,

        +0.4114005618526707f,
        +0.6348032780190205f,
        +0.8259440903283795f,
        +0.9546266788436470f,

        +1.0000000000000000f,
        +0.9546266788436470f,
        +0.8259440903283795f,
        +0.6348032780190205f,

        +0.4114005618526707f,
        +0.1894744904080251f,
        +0.0000000000000000f,
        -0.1343073344351001f,

        -0.2025752314530442f,
        -0.2067943377468982f,
        -0.1602001404590001f,
        -0.0835168749111343f
    };

    void lanczos_resample_6x2(float *dst, const float *src, size_t count)
    {
        IF_ARCH_ARM(
            const float *kernel = lanczos_kernel_6x2;
        );

        ARCH_ARM_ASM
        (
            __ASM_EMIT("subs            %[count], $1")
            __ASM_EMIT("blo             2f")
            __ASM_EMIT("vldm            %[kernel], {q8-q13}")

            __ASM_EMIT("1:")
            __ASM_EMIT("vldm            %[src]!, {s0}")
            __ASM_EMIT("vdup.32         q0, d0[0]")
            __ASM_EMIT("vldm            %[dst], {q2-q7}")
            __ASM_EMIT("vmla.f32        q2, q8, q0")
            __ASM_EMIT("vmla.f32        q3, q9, q0")
            __ASM_EMIT("vmla.f32        q4, q10, q0")
            __ASM_EMIT("vmla.f32        q5, q11, q0")
            __ASM_EMIT("vmla.f32        q6, q12, q0")
            __ASM_EMIT("vmla.f32        q7, q13, q0")
            __ASM_EMIT("vstm            %[dst], {q2-q7}")
            __ASM_EMIT("subs            %[count], $1")
            __ASM_EMIT("add             %[dst], $0x18")
            __ASM_EMIT("bhs             1b")

            __ASM_EMIT("2:")
            : [dst] "+r" (dst), [src] "+r" (src),
              [count] "+r" (count)
            : [kernel] "r" (kernel)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    static const float lanczos_kernel_6x3[] __lsp_aligned16 =
    {
        +0.0000000000000000f,
        +0.0539107649662794f,
        +0.0996822843396933f,
        +0.1233221256242928f,

        +0.1149032936217488f,
        +0.0717178866781924f,
        -0.0000000000000000f,
        -0.0853379770956540f,

        -0.1630759564500400f,
        -0.2097908159917360f,
        -0.2048873332640742f,
        -0.1354777817419537f,

        +0.0000000000000000f,
        +0.1903132912211352f,
        +0.4125642785884168f,
        +0.6358120571822271f,

        +0.8265269098929101f,
        +0.9547949937819170f,
        +1.0000000000000000f,
        +0.9547949937819170f,

        +0.8265269098929101f,
        +0.6358120571822271f,
        +0.4125642785884168f,
        +0.1903132912211352f,

        +0.0000000000000000f,
        -0.1354777817419537f,
        -0.2048873332640742f,
        -0.2097908159917360f,

        -0.1630759564500400f,
        -0.0853379770956540f,
        -0.0000000000000000f,
        +0.0717178866781924f,

        +0.1149032936217488f,
        +0.1233221256242928f,
        +0.0996822843396933f,
        +0.0539107649662794f
    };

    void lanczos_resample_6x3(float *dst, const float *src, size_t count)
    {
        IF_ARCH_ARM(
            const float *k1, *kernel = lanczos_kernel_6x3;
            float *d1;
        );

        ARCH_ARM_ASM
        (
            __ASM_EMIT("subs            %[count], $1")
            __ASM_EMIT("blo             2f")
            __ASM_EMIT("add             %[k1], %[k0], $0x30")
            __ASM_EMIT("vldm            %[k1], {q11-q13}")
            __ASM_EMIT("add             %[k1], $0x30")
            __ASM_EMIT("1:")
            __ASM_EMIT("vldm            %[src]!, {s0}")
            __ASM_EMIT("vdup.32         q0, d0[0]")
            __ASM_EMIT("vldm            %[k0], {q8-q10}")
            __ASM_EMIT("vldm            %[d0], {q2-q7}")
            __ASM_EMIT("add             %[d1], %[d0], $0x60")
            __ASM_EMIT("vmla.f32        q2, q8, q0")
            __ASM_EMIT("vmla.f32        q3, q9, q0")
            __ASM_EMIT("vmla.f32        q4, q10, q0")
            __ASM_EMIT("vmla.f32        q5, q11, q0")
            __ASM_EMIT("vmla.f32        q6, q12, q0")
            __ASM_EMIT("vmla.f32        q7, q13, q0")
            __ASM_EMIT("vstm            %[d0], {q2-q7}")
            __ASM_EMIT("vldm            %[k1], {q8-q10}")
            __ASM_EMIT("vldm            %[d1], {q2-q4}")
            __ASM_EMIT("vmla.f32        q2, q8, q0")
            __ASM_EMIT("vmla.f32        q3, q9, q0")
            __ASM_EMIT("vmla.f32        q4, q10, q0")
            __ASM_EMIT("vstm            %[d1], {q2-q4}")
            __ASM_EMIT("subs            %[count], $1")
            __ASM_EMIT("add             %[d0], $0x18")
            __ASM_EMIT("bhs             1b")

            __ASM_EMIT("2:")
            : [d0] "+r" (dst), [d1] "=&r" (d1), [k1] "=&r" (k1),
              [src] "+r" (src),
              [count] "+r" (count)
            : [k0] "r" (kernel)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    static const float lanczos_kernel_8x2[] __lsp_aligned16 =
    {
        -0.0000000000000000f,
        -0.0635086982303136f,
        -0.1261005767591211f,
        -0.1779172640206227f,

        -0.2091519082144436f,
        -0.2112875836092082f,
        -0.1782608868029105f,
        -0.1073988642554405f,

        +0.0000000000000000f,
        +0.1385297654836717f,
        +0.2990219260297742f,
        +0.4693478599070060f,

        +0.6355976112274050f,
        +0.7835048889119777f,
        +0.8999547969760605f,
        +0.9743975231790596f,

        +1.0000000000000000f,
        +0.9743975231790596f,
        +0.8999547969760605f,
        +0.7835048889119777f,

        +0.6355976112274050f,
        +0.4693478599070060f,
        +0.2990219260297742f,
        +0.1385297654836717f,

        +0.0000000000000000f,
        -0.1073988642554405f,
        -0.1782608868029105f,
        -0.2112875836092082f,

        -0.2091519082144436f,
        -0.1779172640206227f,
        -0.1261005767591211f,
        -0.0635086982303136f
    };

    void lanczos_resample_8x2(float *dst, const float *src, size_t count)
    {
        IF_ARCH_ARM(
            const float *kernel = lanczos_kernel_8x2;
            float *d1, *d2, *d3;
        );

        ARCH_ARM_ASM
        (
            __ASM_EMIT("subs            %[count], $4")
            __ASM_EMIT("vldm            %[kernel], {q8-q15}")
            __ASM_EMIT("blo             2f")

            // x4 blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("vld1.32         {q0}, [%[src]]!")
            __ASM_EMIT("vmov            q1, q0")
            __ASM_EMIT("add             %[d1], %[d0], $0x40")
            __ASM_EMIT("vmov            q2, q0")
            __ASM_EMIT("add             %[d2], %[d0], $0x80")
            __ASM_EMIT("vmov            q3, q1")
            __ASM_EMIT("add             %[d3], %[d0], $0xc0")
            __ASM_EMIT("vtrn.32         q0, q1")
            __ASM_EMIT("vtrn.32         q2, q3")
            __ASM_EMIT("vswp            d1, d4")
            __ASM_EMIT("vswp            d3, d6") // q2 = s0, q3 = s1, q4 = s2, q5 = s3
            // Part 1
            __ASM_EMIT("vldm            %[d0], {q4-q7}")
            __ASM_EMIT("vmla.f32        q4, q8, q0")
            __ASM_EMIT("vmla.f32        q5, q9, q0")
            __ASM_EMIT("vmla.f32        q6, q8, q1")
            __ASM_EMIT("vmla.f32        q7, q9, q1")
            __ASM_EMIT("vmla.f32        q6, q10, q0")
            __ASM_EMIT("vmla.f32        q7, q11, q0")
            __ASM_EMIT("vstm            %[d0], {q4-q7}")
            // Part 2
            __ASM_EMIT("vldm            %[d1], {q4-q7}")
            __ASM_EMIT("vmla.f32        q4, q12, q0")
            __ASM_EMIT("vmla.f32        q5, q11, q1")
            __ASM_EMIT("vmla.f32        q6, q10, q2")
            __ASM_EMIT("vmla.f32        q7, q9, q3")
            __ASM_EMIT("vmla.f32        q5, q13, q0")
            __ASM_EMIT("vmla.f32        q6, q12, q1")
            __ASM_EMIT("vmla.f32        q7, q11, q2")
            __ASM_EMIT("vmla.f32        q4, q8, q2")
            __ASM_EMIT("vmla.f32        q6, q14, q0")
            __ASM_EMIT("vmla.f32        q7, q13, q1")
            __ASM_EMIT("vmla.f32        q5, q9, q2")
            __ASM_EMIT("vmla.f32        q6, q8, q3")
            __ASM_EMIT("vmla.f32        q4, q10, q1")
            __ASM_EMIT("vmla.f32        q7, q15, q0")
            __ASM_EMIT("vstm            %[d1], {q4-q7}")
            // Part 3
            __ASM_EMIT("vldm            %[d2], {q4-q7}")
            __ASM_EMIT("vmla.f32        q4, q14, q1")
            __ASM_EMIT("vmla.f32        q5, q13, q2")
            __ASM_EMIT("vmla.f32        q6, q12, q3")
            __ASM_EMIT("vmla.f32        q5, q15, q1")
            __ASM_EMIT("vmla.f32        q6, q14, q2")
            __ASM_EMIT("vmla.f32        q7, q13, q3")
            __ASM_EMIT("vmla.f32        q4, q12, q2")
            __ASM_EMIT("vmla.f32        q5, q11, q3")
            __ASM_EMIT("vmla.f32        q7, q15, q2")
            __ASM_EMIT("vmla.f32        q4, q10, q3")
            __ASM_EMIT("vstm            %[d2], {q4-q7}")
            // Part 4
            __ASM_EMIT("vldm            %[d3], {q4-q5}")
            __ASM_EMIT("vmla.f32        q4, q14, q3")
            __ASM_EMIT("vmla.f32        q5, q15, q3")
            __ASM_EMIT("vstm            %[d3], {q4-q5}")
            __ASM_EMIT("subs            %[count], $4")
            __ASM_EMIT("add             %[d0], $0x80")
            __ASM_EMIT("bhs             1b")

            // x2 block
            __ASM_EMIT("2:")
            __ASM_EMIT("adds            %[count], $2")
            __ASM_EMIT("blt             4f")

            __ASM_EMIT("vldm            %[src]!, {d0}")
            __ASM_EMIT("vmov            d1, d0")
            __ASM_EMIT("add             %[d1], %[d0], $0x50")
            __ASM_EMIT("vmov            q1, q0")
            __ASM_EMIT("vtrn.32         q0, q1")
            // Part 1
            __ASM_EMIT("vldm            %[d0], {q2-q6}")
            __ASM_EMIT("vmla.f32        q2, q8, q0")
            __ASM_EMIT("vmla.f32        q5, q9, q1")
            __ASM_EMIT("vmla.f32        q4, q10, q0")
            __ASM_EMIT("vmla.f32        q6, q12, q0")
            __ASM_EMIT("vmla.f32        q3, q9, q0")
            __ASM_EMIT("vmla.f32        q4, q8, q1")
            __ASM_EMIT("vmla.f32        q5, q11, q0")
            __ASM_EMIT("vmla.f32        q6, q10, q1")
            __ASM_EMIT("vstm            %[d0], {q2-q6}")
            // Part 2
            __ASM_EMIT("vldm            %[d1], {q2-q6}")
            __ASM_EMIT("vmla.f32        q2, q13, q0")
            __ASM_EMIT("vmla.f32        q5, q14, q1")
            __ASM_EMIT("vmla.f32        q4, q15, q0")
            __ASM_EMIT("vmla.f32        q2, q11, q1")
            __ASM_EMIT("vmla.f32        q3, q14, q0")
            __ASM_EMIT("vmla.f32        q6, q15, q1")
            __ASM_EMIT("vmla.f32        q3, q12, q1")
            __ASM_EMIT("vmla.f32        q4, q13, q1")
            __ASM_EMIT("vstm            %[d1], {q2-q6}")
            __ASM_EMIT("sub             %[count], $2")
            __ASM_EMIT("add             %[d0], $0x40")

            // x1 block
            __ASM_EMIT("4:")
            __ASM_EMIT("adds            %[count], $1")
            __ASM_EMIT("blt             6f")

            __ASM_EMIT("vldm            %[src], {s0}")
            __ASM_EMIT("vdup.32         q0, d0[0]")
            __ASM_EMIT("add             %[d1], %[d0], $0x40")
            __ASM_EMIT("vldm            %[d0], {q2-q5}")
            // Part 1
            __ASM_EMIT("vmla.f32        q2, q8, q0")
            __ASM_EMIT("vmla.f32        q3, q9, q0")
            __ASM_EMIT("vmla.f32        q4, q10, q0")
            __ASM_EMIT("vmla.f32        q5, q11, q0")
            __ASM_EMIT("vstm            %[d0], {q2-q5}")
            // Part 2
            __ASM_EMIT("vldm            %[d1], {q2-q5}")
            __ASM_EMIT("vmla.f32        q2, q12, q0")
            __ASM_EMIT("vmla.f32        q3, q13, q0")
            __ASM_EMIT("vmla.f32        q4, q14, q0")
            __ASM_EMIT("vmla.f32        q5, q15, q0")
            __ASM_EMIT("vstm            %[d1], {q2-q5}")

            __ASM_EMIT("6:")
            : [d0] "+r" (dst), [d1] "=&r" (d1), [d2] "=&r" (d2), [d3] "=&r" (d3),
              [src] "+r" (src),
              [count] "+r" (count)
            : [kernel] "r" (kernel)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    static const float lanczos_kernel_8x3[] __lsp_aligned16 =
    {
        +0.0000000000000000f,
        +0.0413762983728270f,
        +0.0800907151667399f,
        +0.1098388956286788f,

        +0.1250635280404858f,
        +0.1218381732799711f,
        +0.0985951550643596f,
        +0.0565868891962354f,

        -0.0000000000000000f,
        -0.0643160644975587f,
        -0.1274947024165923f,
        -0.1796106608775988f,

        -0.2108456783342821f,
        -0.2127234580145891f,
        -0.1792608653618989f,
        -0.1078863334836789f,

        +0.0000000000000000f,
        +0.1389094368005209f,
        +0.2996235873365793f,
        +0.4700032639087741f,

        +0.6361653568810439f,
        +0.7838984051945062f,
        +0.9001556302107272f,
        +0.9744518753544421f,

        +1.0000000000000000f,
        +0.9744518753544421f,
        +0.9001556302107272f,
        +0.7838984051945062f,

        +0.6361653568810439f,
        +0.4700032639087741f,
        +0.2996235873365793f,
        +0.1389094368005209f,

        +0.0000000000000000f,
        -0.1078863334836789f,
        -0.1792608653618989f,
        -0.2127234580145891f,

        -0.2108456783342821f,
        -0.1796106608775988f,
        -0.1274947024165923f,
        -0.0643160644975587f,

        -0.0000000000000000f,
        +0.0565868891962354f,
        +0.0985951550643596f,
        +0.1218381732799711f,

        +0.1250635280404858f,
        +0.1098388956286788f,
        +0.0800907151667399f,
        +0.0413762983728270f
    };

    void lanczos_resample_8x3(float *dst, const float *src, size_t count)
    {
        IF_ARCH_ARM(
            const float *k1, *kernel = lanczos_kernel_8x3;
            float *d1, *d2;
        );

        ARCH_ARM_ASM
        (
            __ASM_EMIT("subs            %[count], $4")
            __ASM_EMIT("blo             2f")

            // x4 blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("vld1.32         {q0}, [%[src]]!")
            __ASM_EMIT("vmov            q1, q0")
            __ASM_EMIT("vldm            %[k0], {q10-q15}")
            __ASM_EMIT("vmov            q2, q0")
            __ASM_EMIT("add             %[k1], %[k0], $0x60")
            __ASM_EMIT("vmov            q3, q1")
            __ASM_EMIT("add             %[d1], %[d0], $0x60")
            __ASM_EMIT("add             %[d2], %[d0], $0xc0")
            __ASM_EMIT("vtrn.32         q0, q1")
            __ASM_EMIT("vtrn.32         q2, q3")
            __ASM_EMIT("vswp            d1, d4")
            __ASM_EMIT("vswp            d3, d6") // q2 = s0, q3 = s1, q4 = s2, q5 = s3

            // Part 1
            __ASM_EMIT("vldm            %[d0], {q4-q9}")
            __ASM_EMIT("vmla.f32        q4, q10, q0")
            __ASM_EMIT("vmla.f32        q7, q11, q1")
            __ASM_EMIT("vmla.f32        q6, q12, q0")
            __ASM_EMIT("vmla.f32        q9, q13, q1")
            __ASM_EMIT("vmla.f32        q5, q11, q0")
            __ASM_EMIT("vmla.f32        q8, q10, q2")
            __ASM_EMIT("vmla.f32        q7, q13, q0")
            __ASM_EMIT("vmla.f32        q9, q11, q2")
            __ASM_EMIT("vmla.f32        q8, q14, q0")
            __ASM_EMIT("vmla.f32        q6, q10, q1")
            __ASM_EMIT("vmla.f32        q9, q15, q0")
            __ASM_EMIT("vmla.f32        q8, q12, q1")
            __ASM_EMIT("vstm            %[d0], {q4-q9}")
            // Part 2
            __ASM_EMIT("vldm            %[d1], {q4-q9}")
            __ASM_EMIT("vmla.f32        q4, q14, q1")
            __ASM_EMIT("vmla.f32        q7, q15, q2")
            __ASM_EMIT("vmla.f32        q8, q14, q3")
            __ASM_EMIT("vmla.f32        q6, q12, q3")
            __ASM_EMIT("vmla.f32        q5, q13, q2")
            __ASM_EMIT("vmla.f32        q9, q15, q3")
            __ASM_EMIT("vmla.f32        q4, q12, q2")
            __ASM_EMIT("vmla.f32        q7, q13, q3")
            __ASM_EMIT("vmla.f32        q6, q14, q2")
            __ASM_EMIT("vmla.f32        q4, q10, q3")
            __ASM_EMIT("vmla.f32        q5, q15, q1")
            __ASM_EMIT("vmla.f32        q5, q11, q3")
            // Part 3
            __ASM_EMIT("vldm            %[k1], {q10-q15}")
            __ASM_EMIT("vmla.f32        q4, q10, q0")
            __ASM_EMIT("vmla.f32        q7, q11, q1")
            __ASM_EMIT("vmla.f32        q6, q12, q0")
            __ASM_EMIT("vmla.f32        q9, q13, q1")
            __ASM_EMIT("vmla.f32        q5, q11, q0")
            __ASM_EMIT("vmla.f32        q8, q10, q2")
            __ASM_EMIT("vmla.f32        q7, q13, q0")
            __ASM_EMIT("vmla.f32        q9, q11, q2")
            __ASM_EMIT("vmla.f32        q8, q14, q0")
            __ASM_EMIT("vmla.f32        q6, q10, q1")
            __ASM_EMIT("vmla.f32        q9, q15, q0")
            __ASM_EMIT("vmla.f32        q8, q12, q1")
            __ASM_EMIT("vstm            %[d1], {q4-q9}")
            // Part 4
            __ASM_EMIT("vldm            %[d2], {q4-q9}")
            __ASM_EMIT("vmla.f32        q4, q14, q1")
            __ASM_EMIT("vmla.f32        q7, q15, q2")
            __ASM_EMIT("vmla.f32        q8, q14, q3")
            __ASM_EMIT("vmla.f32        q6, q12, q3")
            __ASM_EMIT("vmla.f32        q5, q13, q2")
            __ASM_EMIT("vmla.f32        q9, q15, q3")
            __ASM_EMIT("vmla.f32        q4, q12, q2")
            __ASM_EMIT("vmla.f32        q7, q13, q3")
            __ASM_EMIT("vmla.f32        q6, q14, q2")
            __ASM_EMIT("vmla.f32        q4, q10, q3")
            __ASM_EMIT("vmla.f32        q5, q15, q1")
            __ASM_EMIT("vmla.f32        q5, q11, q3")
            __ASM_EMIT("vstm            %[d2], {q4-q9}")
            __ASM_EMIT("subs            %[count], $4")
            __ASM_EMIT("add             %[d0], $0x80")
            __ASM_EMIT("bhs             1b")

            // x2 block
            __ASM_EMIT("2:")
            __ASM_EMIT("adds            %[count], $2")
            __ASM_EMIT("blt             4f")

            __ASM_EMIT("vldm            %[src]!, {d0}")
            __ASM_EMIT("vmov            d1, d0")
            __ASM_EMIT("add             %[d1], %[d0], $0x70")
            __ASM_EMIT("vmov            q1, q0")
            __ASM_EMIT("vldm            %[k0], {q9-q15}")
            __ASM_EMIT("vtrn.32         q0, q1")
            __ASM_EMIT("add             %[k1], %[k0], $0x50")
            // Part 1
            __ASM_EMIT("vldm            %[d0], {q2-q8}")
            __ASM_EMIT("vmla.f32        q2, q9, q0")
            __ASM_EMIT("vmla.f32        q7, q14, q0")
            __ASM_EMIT("vmla.f32        q4, q9, q1")
            __ASM_EMIT("vmla.f32        q3, q10, q0")
            __ASM_EMIT("vmla.f32        q6, q11, q1")
            __ASM_EMIT("vmla.f32        q8, q15, q0")
            __ASM_EMIT("vmla.f32        q5, q12, q0")
            __ASM_EMIT("vmla.f32        q8, q13, q1")
            __ASM_EMIT("vmla.f32        q4, q11, q0")
            __ASM_EMIT("vmla.f32        q5, q10, q1")
            __ASM_EMIT("vmla.f32        q6, q13, q0")
            __ASM_EMIT("vmla.f32        q7, q12, q1")
            __ASM_EMIT("vstm            %[d0], {q2-q8}")
            // Part 2
            __ASM_EMIT("vldm            %[k1], {q9-q15}")
            __ASM_EMIT("vldm            %[d1], {q2-q8}")
            __ASM_EMIT("vmla.f32        q2, q9, q1")
            __ASM_EMIT("vmla.f32        q3, q10, q1")
            __ASM_EMIT("vmla.f32        q4, q13, q0")
            __ASM_EMIT("vmla.f32        q7, q14, q1")
            __ASM_EMIT("vmla.f32        q2, q11, q0")
            __ASM_EMIT("vmla.f32        q5, q12, q1")
            __ASM_EMIT("vmla.f32        q6, q15, q0")
            __ASM_EMIT("vmla.f32        q4, q11, q1")
            __ASM_EMIT("vmla.f32        q5, q14, q0")
            __ASM_EMIT("vmla.f32        q8, q15, q1")
            __ASM_EMIT("vmla.f32        q3, q12, q0")
            __ASM_EMIT("vmla.f32        q6, q13, q1")
            __ASM_EMIT("vstm            %[d1], {q2-q8}")
            __ASM_EMIT("sub             %[count], $2")
            __ASM_EMIT("add             %[d0], $0x40")

            // x1 block
            __ASM_EMIT("4:")
            __ASM_EMIT("adds            %[count], $1")
            __ASM_EMIT("blt             6f")

            __ASM_EMIT("vldm            %[src], {s0}")
            __ASM_EMIT("vdup.32         q0, d0[0]")
            __ASM_EMIT("vldm            %[k0], {q10-q15}")
            __ASM_EMIT("add             %[d1], %[d0], $0x60")
            __ASM_EMIT("add             %[k1], %[k0], $0x60")
            // Part 1
            __ASM_EMIT("vldm            %[d0], {q4-q9}")
            __ASM_EMIT("vmla.f32        q4, q10, q0")
            __ASM_EMIT("vmla.f32        q5, q11, q0")
            __ASM_EMIT("vmla.f32        q6, q12, q0")
            __ASM_EMIT("vmla.f32        q7, q13, q0")
            __ASM_EMIT("vmla.f32        q8, q14, q0")
            __ASM_EMIT("vmla.f32        q9, q15, q0")
            __ASM_EMIT("vstm            %[d0], {q4-q9}")
            // Part 2
            __ASM_EMIT("vldm            %[k1], {q10-q15}")
            __ASM_EMIT("vldm            %[d1], {q4-q9}")
            __ASM_EMIT("vmla.f32        q4, q10, q0")
            __ASM_EMIT("vmla.f32        q5, q11, q0")
            __ASM_EMIT("vmla.f32        q6, q12, q0")
            __ASM_EMIT("vmla.f32        q7, q13, q0")
            __ASM_EMIT("vmla.f32        q8, q14, q0")
            __ASM_EMIT("vmla.f32        q9, q15, q0")
            __ASM_EMIT("vstm            %[d1], {q4-q9}")

            __ASM_EMIT("6:")
            : [d0] "+r" (dst), [d1] "=&r" (d1), [d2] "=&r" (d2), [k1] "=&r" (k1),
              [src] "+r" (src),
              [count] "+r" (count)
            : [k0] "r" (kernel)
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    void downsample_2x(float *dst, const float *src, size_t count)
    {
        ARCH_ARM_ASM
        (
            __ASM_EMIT("subs            %[count], $32")
            __ASM_EMIT("blo             2f")

            // x32 blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("vld2.32         {q0-q1}, [%[src]]!")
            __ASM_EMIT("vld2.32         {q2-q3}, [%[src]]!")
            __ASM_EMIT("vld2.32         {q4-q5}, [%[src]]!")
            __ASM_EMIT("vmov            q1, q2")
            __ASM_EMIT("vld2.32         {q6-q7}, [%[src]]!")
            __ASM_EMIT("vst1.32         {q0-q1}, [%[dst]]!")
            __ASM_EMIT("vmov            q2, q4")
            __ASM_EMIT("vld2.32         {q8-q9}, [%[src]]!")
            __ASM_EMIT("vmov            q3, q6")
            __ASM_EMIT("vld2.32         {q10-q11}, [%[src]]!")
            __ASM_EMIT("vst1.32         {q2-q3}, [%[dst]]!")
            __ASM_EMIT("vmov            q4, q8")
            __ASM_EMIT("vld2.32         {q12-q13}, [%[src]]!")
            __ASM_EMIT("vmov            q5, q10")
            __ASM_EMIT("vld2.32         {q14-q15}, [%[src]]!")
            __ASM_EMIT("vst1.32         {q4-q5}, [%[dst]]!")
            __ASM_EMIT("vmov            q6, q12")
            __ASM_EMIT("vmov            q7, q14")
            __ASM_EMIT("subs            %[count], $32")
            __ASM_EMIT("vst1.32         {q6-q7}, [%[dst]]!")
            __ASM_EMIT("bhs             1b")

            // x16 block
            __ASM_EMIT("2:")
            __ASM_EMIT("adds            %[count], $16")
            __ASM_EMIT("blt             4f")
            __ASM_EMIT("vld2.32         {q0-q1}, [%[src]]!")
            __ASM_EMIT("vld2.32         {q2-q3}, [%[src]]!")
            __ASM_EMIT("vld2.32         {q4-q5}, [%[src]]!")
            __ASM_EMIT("vmov            q1, q2")
            __ASM_EMIT("vld2.32         {q6-q7}, [%[src]]!")
            __ASM_EMIT("vst1.32         {q0-q1}, [%[dst]]!")
            __ASM_EMIT("vmov            q2, q4")
            __ASM_EMIT("vmov            q3, q6")
            __ASM_EMIT("sub             %[count], $16")
            __ASM_EMIT("vst1.32         {q2-q3}, [%[dst]]!")

            // x8 block
            __ASM_EMIT("4:")
            __ASM_EMIT("adds            %[count], $8")
            __ASM_EMIT("blt             6f")
            __ASM_EMIT("vld2.32         {q0-q1}, [%[src]]!")
            __ASM_EMIT("vld2.32         {q2-q3}, [%[src]]!")
            __ASM_EMIT("vmov            q1, q2")
            __ASM_EMIT("sub             %[count], $8")
            __ASM_EMIT("vst1.32         {q0-q1}, [%[dst]]!")

            // x4 block
            __ASM_EMIT("6:")
            __ASM_EMIT("adds            %[count], $4")
            __ASM_EMIT("blt             8f")
            __ASM_EMIT("vld2.32         {q0-q1}, [%[src]]!")
            __ASM_EMIT("sub             %[count], $4")
            __ASM_EMIT("vst1.32         {q0}, [%[dst]]!")

            // x1 blocks
            __ASM_EMIT("8:")
            __ASM_EMIT("adds            %[count], $3")
            __ASM_EMIT("blt             10f")
            __ASM_EMIT("9:")
            __ASM_EMIT("vldm.32         %[src]!, {s0, s1}")
            __ASM_EMIT("subs            %[count], $1")
            __ASM_EMIT("vstm.32         %[dst]!, {s0}")
            __ASM_EMIT("bge             9b")
            __ASM_EMIT("10:")

            : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    void downsample_3x(float *dst, const float *src, size_t count)
    {
        ARCH_ARM_ASM
        (
            __ASM_EMIT("subs            %[count], $16")
            __ASM_EMIT("blo             2f")

            // x16 blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("vldr            s0,  [%[src], $0x000]")
            __ASM_EMIT("vldr            s1,  [%[src], $0x00c]")
            __ASM_EMIT("vldr            s2,  [%[src], $0x018]")
            __ASM_EMIT("vldr            s3,  [%[src], $0x024]")
            __ASM_EMIT("vldr            s4,  [%[src], $0x030]")
            __ASM_EMIT("vldr            s5,  [%[src], $0x03c]")
            __ASM_EMIT("vldr            s6,  [%[src], $0x048]")
            __ASM_EMIT("vldr            s7,  [%[src], $0x054]")
            __ASM_EMIT("vldr            s8,  [%[src], $0x060]")
            __ASM_EMIT("vldr            s9,  [%[src], $0x06c]")
            __ASM_EMIT("vldr            s10, [%[src], $0x078]")
            __ASM_EMIT("vldr            s11, [%[src], $0x084]")
            __ASM_EMIT("vldr            s12, [%[src], $0x090]")
            __ASM_EMIT("vldr            s13, [%[src], $0x09c]")
            __ASM_EMIT("vldr            s14, [%[src], $0x0a8]")
            __ASM_EMIT("vldr            s15, [%[src], $0x0b4]")
            __ASM_EMIT("vstm            %[dst]!, {q0-q3}")
            __ASM_EMIT("add             %[src], $0xc0")
            __ASM_EMIT("subs            %[count], $16")
            __ASM_EMIT("bhs             1b")

            // x8 block
            __ASM_EMIT("2:")
            __ASM_EMIT("adds            %[count], $8")
            __ASM_EMIT("blt             4f")

            __ASM_EMIT("vldr            s0,  [%[src], $0x000]")
            __ASM_EMIT("vldr            s1,  [%[src], $0x00c]")
            __ASM_EMIT("vldr            s2,  [%[src], $0x018]")
            __ASM_EMIT("vldr            s3,  [%[src], $0x024]")
            __ASM_EMIT("vldr            s4,  [%[src], $0x030]")
            __ASM_EMIT("vldr            s5,  [%[src], $0x03c]")
            __ASM_EMIT("vldr            s6,  [%[src], $0x048]")
            __ASM_EMIT("vldr            s7,  [%[src], $0x054]")
            __ASM_EMIT("vstm            %[dst]!, {q0-q1}")
            __ASM_EMIT("add             %[src], $0x60")
            __ASM_EMIT("sub             %[count], $8")

            // x4 block
            __ASM_EMIT("4:")
            __ASM_EMIT("adds            %[count], $4")
            __ASM_EMIT("blt             6f")
            __ASM_EMIT("vldr            s0,  [%[src], $0x000]")
            __ASM_EMIT("vldr            s1,  [%[src], $0x00c]")
            __ASM_EMIT("vldr            s2,  [%[src], $0x018]")
            __ASM_EMIT("vldr            s3,  [%[src], $0x024]")
            __ASM_EMIT("vstm            %[dst]!, {q0}")
            __ASM_EMIT("add             %[src], $0x30")
            __ASM_EMIT("sub             %[count], $4")

            // x1 blocks
            __ASM_EMIT("6:")
            __ASM_EMIT("adds            %[count], $3")
            __ASM_EMIT("blt             8f")
            __ASM_EMIT("7:")
            __ASM_EMIT("vldm.32         %[src]!, {s0-s2}")
            __ASM_EMIT("vstm.32         %[dst]!, {s0}")
            __ASM_EMIT("subs            %[count], $1")
            __ASM_EMIT("bge             7b")

            __ASM_EMIT("8:")

            : [dst] "+r" (dst), [src] "+r" (src),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3"
        );
    }

    void downsample_4x(float *dst, const float *src, size_t count)
    {
        ARCH_ARM_ASM
        (
            __ASM_EMIT("subs            %[count], $16")
            __ASM_EMIT("blo             2f")

            // x16 blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("vldr            s0,  [%[src], $0x000]")
            __ASM_EMIT("vldr            s1,  [%[src], $0x010]")
            __ASM_EMIT("vldr            s2,  [%[src], $0x020]")
            __ASM_EMIT("vldr            s3,  [%[src], $0x030]")
            __ASM_EMIT("vldr            s4,  [%[src], $0x040]")
            __ASM_EMIT("vldr            s5,  [%[src], $0x050]")
            __ASM_EMIT("vldr            s6,  [%[src], $0x060]")
            __ASM_EMIT("vldr            s7,  [%[src], $0x070]")
            __ASM_EMIT("vldr            s8,  [%[src], $0x080]")
            __ASM_EMIT("vldr            s9,  [%[src], $0x090]")
            __ASM_EMIT("vldr            s10, [%[src], $0x0a0]")
            __ASM_EMIT("vldr            s11, [%[src], $0x0b0]")
            __ASM_EMIT("vldr            s12, [%[src], $0x0c0]")
            __ASM_EMIT("vldr            s13, [%[src], $0x0d0]")
            __ASM_EMIT("vldr            s14, [%[src], $0x0e0]")
            __ASM_EMIT("vldr            s15, [%[src], $0x0f0]")
            __ASM_EMIT("vstm            %[dst]!, {q0-q3}")
            __ASM_EMIT("add             %[src], $0x100")
            __ASM_EMIT("subs            %[count], $16")
            __ASM_EMIT("bhs             1b")

            // x8 block
            __ASM_EMIT("2:")
            __ASM_EMIT("adds            %[count], $8")
            __ASM_EMIT("blt             4f")

            __ASM_EMIT("vldr            s0,  [%[src], $0x000]")
            __ASM_EMIT("vldr            s1,  [%[src], $0x010]")
            __ASM_EMIT("vldr            s2,  [%[src], $0x020]")
            __ASM_EMIT("vldr            s3,  [%[src], $0x030]")
            __ASM_EMIT("vldr            s4,  [%[src], $0x040]")
            __ASM_EMIT("vldr            s5,  [%[src], $0x050]")
            __ASM_EMIT("vldr            s6,  [%[src], $0x060]")
            __ASM_EMIT("vldr            s7,  [%[src], $0x070]")
            __ASM_EMIT("vstm            %[dst]!, {q0-q1}")
            __ASM_EMIT("add             %[src], $0x80")
            __ASM_EMIT("sub             %[count], $8")

            // x4 block
            __ASM_EMIT("4:")
            __ASM_EMIT("adds            %[count], $4")
            __ASM_EMIT("blt             6f")
            __ASM_EMIT("vldr            s0,  [%[src], $0x000]")
            __ASM_EMIT("vldr            s1,  [%[src], $0x010]")
            __ASM_EMIT("vldr            s2,  [%[src], $0x020]")
            __ASM_EMIT("vldr            s3,  [%[src], $0x030]")
            __ASM_EMIT("vstm            %[dst]!, {q0}")
            __ASM_EMIT("add             %[src], $0x40")
            __ASM_EMIT("sub             %[count], $4")

            // x1 blocks
            __ASM_EMIT("6:")
            __ASM_EMIT("adds            %[count], $3")
            __ASM_EMIT("blt             8f")
            __ASM_EMIT("7:")
            __ASM_EMIT("vldm.32         %[src]!, {q0}")
            __ASM_EMIT("vstm.32         %[dst]!, {s0}")
            __ASM_EMIT("subs            %[count], $1")
            __ASM_EMIT("bge             7b")

            __ASM_EMIT("8:")

            : [dst] "+r" (dst), [src] "+r" (src),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3"
        );
    }

    void downsample_6x(float *dst, const float *src, size_t count)
    {
        ARCH_ARM_ASM
        (
            __ASM_EMIT("subs            %[count], $16")
            __ASM_EMIT("blo             2f")

            // x16 blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("vldr            s0,  [%[src], $0x000]")
            __ASM_EMIT("vldr            s1,  [%[src], $0x018]")
            __ASM_EMIT("vldr            s2,  [%[src], $0x030]")
            __ASM_EMIT("vldr            s3,  [%[src], $0x048]")
            __ASM_EMIT("vldr            s4,  [%[src], $0x060]")
            __ASM_EMIT("vldr            s5,  [%[src], $0x078]")
            __ASM_EMIT("vldr            s6,  [%[src], $0x090]")
            __ASM_EMIT("vldr            s7,  [%[src], $0x0a8]")
            __ASM_EMIT("vldr            s8,  [%[src], $0x0c0]")
            __ASM_EMIT("vldr            s9,  [%[src], $0x0d8]")
            __ASM_EMIT("vldr            s10, [%[src], $0x0f0]")
            __ASM_EMIT("vldr            s11, [%[src], $0x108]")
            __ASM_EMIT("vldr            s12, [%[src], $0x120]")
            __ASM_EMIT("vldr            s13, [%[src], $0x138]")
            __ASM_EMIT("vldr            s14, [%[src], $0x150]")
            __ASM_EMIT("vldr            s15, [%[src], $0x168]")
            __ASM_EMIT("vstm            %[dst]!, {q0-q3}")
            __ASM_EMIT("add             %[src], $0x180")
            __ASM_EMIT("subs            %[count], $16")
            __ASM_EMIT("bhs             1b")

            // x8 block
            __ASM_EMIT("2:")
            __ASM_EMIT("adds            %[count], $8")
            __ASM_EMIT("blt             4f")

            __ASM_EMIT("vldr            s0,  [%[src], $0x000]")
            __ASM_EMIT("vldr            s1,  [%[src], $0x018]")
            __ASM_EMIT("vldr            s2,  [%[src], $0x030]")
            __ASM_EMIT("vldr            s3,  [%[src], $0x048]")
            __ASM_EMIT("vldr            s4,  [%[src], $0x060]")
            __ASM_EMIT("vldr            s5,  [%[src], $0x078]")
            __ASM_EMIT("vldr            s6,  [%[src], $0x090]")
            __ASM_EMIT("vldr            s7,  [%[src], $0x0a8]")
            __ASM_EMIT("vstm            %[dst]!, {q0-q1}")
            __ASM_EMIT("add             %[src], $0xc0")
            __ASM_EMIT("sub             %[count], $8")

            // x4 block
            __ASM_EMIT("4:")
            __ASM_EMIT("adds            %[count], $4")
            __ASM_EMIT("blt             6f")
            __ASM_EMIT("vldr            s0,  [%[src], $0x000]")
            __ASM_EMIT("vldr            s1,  [%[src], $0x018]")
            __ASM_EMIT("vldr            s2,  [%[src], $0x030]")
            __ASM_EMIT("vldr            s3,  [%[src], $0x048]")
            __ASM_EMIT("vstm            %[dst]!, {q0}")
            __ASM_EMIT("add             %[src], $0x60")
            __ASM_EMIT("sub             %[count], $4")

            // x1 blocks
            __ASM_EMIT("6:")
            __ASM_EMIT("adds            %[count], $3")
            __ASM_EMIT("blt             8f")
            __ASM_EMIT("7:")
            __ASM_EMIT("vldm.32         %[src]!, {d0-d2}")
            __ASM_EMIT("vstm.32         %[dst]!, {s0}")
            __ASM_EMIT("subs            %[count], $1")
            __ASM_EMIT("bge             7b")

            __ASM_EMIT("8:")

            : [dst] "+r" (dst), [src] "+r" (src),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3"
        );
    }

    void downsample_8x(float *dst, const float *src, size_t count)
    {
        ARCH_ARM_ASM
        (
            __ASM_EMIT("subs            %[count], $16")
            __ASM_EMIT("blo             2f")

            // x16 blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("vldr            s0,  [%[src], $0x000]")
            __ASM_EMIT("vldr            s1,  [%[src], $0x020]")
            __ASM_EMIT("vldr            s2,  [%[src], $0x040]")
            __ASM_EMIT("vldr            s3,  [%[src], $0x060]")
            __ASM_EMIT("vldr            s4,  [%[src], $0x080]")
            __ASM_EMIT("vldr            s5,  [%[src], $0x0a0]")
            __ASM_EMIT("vldr            s6,  [%[src], $0x0c0]")
            __ASM_EMIT("vldr            s7,  [%[src], $0x0e0]")
            __ASM_EMIT("vldr            s8,  [%[src], $0x100]")
            __ASM_EMIT("vldr            s9,  [%[src], $0x120]")
            __ASM_EMIT("vldr            s10, [%[src], $0x140]")
            __ASM_EMIT("vldr            s11, [%[src], $0x160]")
            __ASM_EMIT("vldr            s12, [%[src], $0x180]")
            __ASM_EMIT("vldr            s13, [%[src], $0x1a0]")
            __ASM_EMIT("vldr            s14, [%[src], $0x1c0]")
            __ASM_EMIT("vldr            s15, [%[src], $0x1e0]")
            __ASM_EMIT("vstm            %[dst]!, {q0-q3}")
            __ASM_EMIT("add             %[src], $0x200")
            __ASM_EMIT("subs            %[count], $16")
            __ASM_EMIT("bhs             1b")

            // x8 block
            __ASM_EMIT("2:")
            __ASM_EMIT("adds            %[count], $8")
            __ASM_EMIT("blt             4f")

            __ASM_EMIT("vldr            s0,  [%[src], $0x000]")
            __ASM_EMIT("vldr            s1,  [%[src], $0x020]")
            __ASM_EMIT("vldr            s2,  [%[src], $0x040]")
            __ASM_EMIT("vldr            s3,  [%[src], $0x060]")
            __ASM_EMIT("vldr            s4,  [%[src], $0x080]")
            __ASM_EMIT("vldr            s5,  [%[src], $0x0a0]")
            __ASM_EMIT("vldr            s6,  [%[src], $0x0c0]")
            __ASM_EMIT("vldr            s7,  [%[src], $0x0e0]")
            __ASM_EMIT("vstm            %[dst]!, {q0-q1}")
            __ASM_EMIT("add             %[src], $0x100")
            __ASM_EMIT("sub             %[count], $8")

            // x4 block
            __ASM_EMIT("4:")
            __ASM_EMIT("adds            %[count], $4")
            __ASM_EMIT("blt             6f")
            __ASM_EMIT("vldr            s0,  [%[src], $0x000]")
            __ASM_EMIT("vldr            s1,  [%[src], $0x020]")
            __ASM_EMIT("vldr            s2,  [%[src], $0x040]")
            __ASM_EMIT("vldr            s3,  [%[src], $0x060]")
            __ASM_EMIT("vstm            %[dst]!, {q0}")
            __ASM_EMIT("add             %[src], $0x80")
            __ASM_EMIT("sub             %[count], $4")

            // x1 blocks
            __ASM_EMIT("6:")
            __ASM_EMIT("adds            %[count], $3")
            __ASM_EMIT("blt             8f")
            __ASM_EMIT("7:")
            __ASM_EMIT("vldm.32         %[src]!, {q0-q1}")
            __ASM_EMIT("vstm.32         %[dst]!, {s0}")
            __ASM_EMIT("subs            %[count], $1")
            __ASM_EMIT("bge             7b")

            __ASM_EMIT("8:")

            : [dst] "+r" (dst), [src] "+r" (src),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3"
        );
    }

}

#endif /* DSP_ARCH_ARM_NEON_D32_RESAMPLING_H_ */
