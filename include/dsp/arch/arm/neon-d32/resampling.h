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
    IF_ARCH_ARM(
        static const float lanczos_2x2[] __lsp_aligned16 =
        {
            +0.0000000000000000f,
            -0.0636843520278618f,   // k1
            +0.0000000000000000f,
            +0.5731591682507563f,   // k0

            +1.0000000000000000f,
            +0.5731591682507563f,   // k0
            +0.0000000000000000f,
            -0.0636843520278618f    // k1
        };
    )

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
            __ASM_EMIT("vldm            %[dst], {d14-d23}")
            __ASM_EMIT("vdup.32         q6, d5[1]")
            __ASM_EMIT("vdup.32         q5, d5[0]")
            __ASM_EMIT("vdup.32         q4, d4[1]")
            __ASM_EMIT("vdup.32         q2, d4[0]")
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
            __ASM_EMIT("vldm            %[dst], {d14-d23}")
            __ASM_EMIT("vdup.32         q6, d7[1]")
            __ASM_EMIT("vdup.32         q5, d7[0]")
            __ASM_EMIT("vdup.32         q4, d6[1]")
            __ASM_EMIT("vdup.32         q3, d6[0]")
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
            __ASM_EMIT("vldm            %[dst], {d14-d19}")
            __ASM_EMIT("vdup.32         q5, d5[1]") // q5 = s3
            __ASM_EMIT("vdup.32         q4, d5[0]") // q4 = s2
            __ASM_EMIT("vdup.32         q3, d4[1]") // q3 = s1
            __ASM_EMIT("vdup.32         q2, d4[0]") // q2 = s0
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
            __ASM_EMIT("vld1.32         {d4[], d5[]}, [%[src]]!")
            __ASM_EMIT("vldm            %[dst], {q3-q4}")
            __ASM_EMIT("vmla.f32        q3, q2, q0")
            __ASM_EMIT("vmla.f32        q4, q2, q1")
            __ASM_EMIT("vstm            %[dst], {q3-q4}")
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

    IF_ARCH_ARM
    (
        static const float lanczos_2x3[] __lsp_aligned16 =
        {
            +0.0000000000000000f,
            +0.0243170840741611f,   // k2
            +0.0000000000000000f,
            -0.1350949115231170f,   // k1

            +0.0000000000000000f,
            +0.6079271018540265f,   // k0
            +1.0000000000000000f,
            +0.6079271018540265f,   // k0

            +0.0000000000000000f,
            -0.1350949115231170f,   // k1
            +0.0000000000000000f,
            +0.0243170840741611f    // k2
        };
    )

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
            __ASM_EMIT("vldm            %[dst], {q10-q15}")
            __ASM_EMIT("vdup.32         q8, d9[1]")                 // q8 = s6
            __ASM_EMIT("vdup.32         q7, d9[0]")                 // q7 = s4
            __ASM_EMIT("vdup.32         q6, d8[1]")                 // q6 = s2
            __ASM_EMIT("vdup.32         q4, d8[0]")                 // q4 = s0
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

            __ASM_EMIT("vldm            %[dst], {q10-q15}")
            __ASM_EMIT("vdup.32         q8, d11[1]")                // q8 = s7
            __ASM_EMIT("vdup.32         q7, d11[0]")                // q7 = s5
            __ASM_EMIT("vdup.32         q6, d10[1]")                // q6 = s3
            __ASM_EMIT("vdup.32         q5, d10[0]")                // q5 = s1
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
            __ASM_EMIT("vldm            %[dst], {q10-q13}")
            __ASM_EMIT("vdup.32         q7, d9[1]")                 // q7 = s3
            __ASM_EMIT("vdup.32         q6, d9[0]")                 // q6 = s2
            __ASM_EMIT("vdup.32         q5, d8[1]")                 // q5 = s1
            __ASM_EMIT("vdup.32         q4, d8[0]")                 // q4 = s0
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
            __ASM_EMIT("vld1.32         {d6[], d7[]}, [%[src]]!")
            __ASM_EMIT("vldm            %[dst], {q4-q6}")
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

    IF_ARCH_ARM(
        static const float lanczos_kernel_3x2[] __lsp_aligned16 =
        {
            +0.0000000000000000f,
            -0.0315888188312782f,
            -0.0854897486982225f,
            +0.0000000000000000f,

            +0.3419589947928900f,
            +0.7897204707819555f,
            +1.0000000000000000f,
            +0.7897204707819555f,

            +0.3419589947928900f,
            +0.0000000000000000f,
            -0.0854897486982225f,
            -0.0315888188312782f,

            // Shifted by 1 left
            -0.0315888188312782f,
            -0.0854897486982225f,
            +0.0000000000000000f,
            +0.3419589947928900f,

            +0.7897204707819555f,
            +1.0000000000000000f,
            +0.7897204707819555f,
            +0.3419589947928900f,

            +0.0000000000000000f,
            -0.0854897486982225f,
            -0.0315888188312782f,
            +0.0000000000000000f
        };
    )

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
            __ASM_EMIT("vldm            %[dst], {q8-q11}")
            __ASM_EMIT("vdup.32         q1, d0[1]")                 // q1 = s2 s2 s2 s2
            __ASM_EMIT("vdup.32         q0, d0[0]")                 // q0 = s1 s1 s1 s1
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

            __ASM_EMIT("vld1.32         {d0[], d1[]}, [%[src]]")
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

    IF_ARCH_ARM(
        static const float lanczos_kernel_3x3[] __lsp_aligned16 =
        {
            // Middle part
            +0.8103009258121772f,
            +1.0000000000000000f,
            +0.8103009258121772f,
            +0.3807169003008463f,

            +0.0000000000000000f,
            -0.1458230329384726f,
            -0.0933267410806225f,
            +0.0000000000000000f,

            +0.0310789306368038f,
            +0.0126609519658153f,
            +0.0000000000000000f,
            +0.0000000000000000f,

            +0.0126609519658153f,
            +0.0310789306368038f,
            +0.0000000000000000f,
            -0.0933267410806225f,

            -0.1458230329384726f,
            +0.0000000000000000f,
            +0.3807169003008463f,
            +0.8103009258121772f,

            +1.0000000000000000f,
            +0.8103009258121772f,
            +0.3807169003008463f,
            +0.0000000000000000f,

            // Head part
            +0.0000000000000000f,
            +0.0126609519658153f,
            +0.0310789306368038f,
            +0.0000000000000000f,

            -0.0933267410806225f,
            -0.1458230329384726f,
            +0.0000000000000000f,
            +0.3807169003008463f,

            // Tail part
            -0.1458230329384726f,
            -0.0933267410806225f,
            -0.0000000000000000f,
            +0.0310789306368038f,

            +0.0126609519658153f,
            +0.0000000000000000f,
            +0.0000000000000000f,
            +0.0000000000000000f
        };
    )

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
            __ASM_EMIT("vldm            %[dst], {q2-q7}")
            __ASM_EMIT("vdup.32         q1, d0[1]")                 // q1 = s2 s2 s2 s2
            __ASM_EMIT("vdup.32         q0, d0[0]")                 // q0 = s1 s1 s1 s1
            __ASM_EMIT("vld1.32         {q8-q9}, [%[kernel]]!")

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

            __ASM_EMIT("vld1.32         {d0[], d1[]}, [%[src]]")
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

    IF_ARCH_ARM(
        static const float lanczos_kernel_4x2[] __lsp_aligned16 =
        {
            +0.0000000000000000f,
            -0.0179051851263444f,
            -0.0636843520278618f,
            -0.0847248039068907f,

            +0.0000000000000000f,
            +0.2353466775191407f,
            +0.5731591682507563f,
            +0.8773540711908775f,

            +1.0000000000000000f,
            +0.8773540711908775f,
            +0.5731591682507563f,
            +0.2353466775191407f,

            +0.0000000000000000f,
            -0.0847248039068907f,
            -0.0636843520278618f,
            -0.0179051851263444f
        };
    )

    void lanczos_resample_4x2(float *dst, const float *src, size_t count)
    {
        IF_ARCH_ARM(
            const float *kernel = lanczos_kernel_4x2;
        );

        ARCH_ARM_ASM
        (
            // Prepare
            __ASM_EMIT("vldm            %[kernel], {q12-q15}")
            __ASM_EMIT("subs            %[count], $4")
            __ASM_EMIT("blo             2f")

            // 4x blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("vldm            %[src]!, {q0}")
            __ASM_EMIT("vldm            %[dst], {q4-q10}")
            __ASM_EMIT("vdup.32         q3, d1[1]")
            __ASM_EMIT("vdup.32         q2, d1[0]")
            __ASM_EMIT("vdup.32         q1, d0[1]")
            __ASM_EMIT("vdup.32         q0, d0[0]")

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
            __ASM_EMIT("vstm            %[dst], {q4-q10}")
            __ASM_EMIT("subs            %[count], $4")
            __ASM_EMIT("add             %[dst], $0x40")
            __ASM_EMIT("bhs             1b")

            // 2x block
            __ASM_EMIT("2:")
            __ASM_EMIT("adds            %[count], $2")
            __ASM_EMIT("blt             4f")

            __ASM_EMIT("vldm            %[src]!, {d0}")
            __ASM_EMIT("vldm            %[dst], {q4-q8}")
            __ASM_EMIT("vdup.32         q1, d0[1]")
            __ASM_EMIT("vdup.32         q0, d0[0]")

            __ASM_EMIT("vmla.f32        q4, q12, q0")
            __ASM_EMIT("vmla.f32        q5, q13, q0")
            __ASM_EMIT("vmla.f32        q6, q14, q0")
            __ASM_EMIT("vmla.f32        q7, q15, q0")
            __ASM_EMIT("vmla.f32        q5, q12, q1")
            __ASM_EMIT("vmla.f32        q6, q13, q1")
            __ASM_EMIT("vmla.f32        q7, q14, q1")
            __ASM_EMIT("vmla.f32        q8, q15, q1")
            __ASM_EMIT("vstm            %[dst], {q4-q8}")
            __ASM_EMIT("sub             %[count], $2")
            __ASM_EMIT("add             %[dst], $0x20")

            // 1x block
            __ASM_EMIT("4:")
            __ASM_EMIT("adds            %[count], $1")
            __ASM_EMIT("blt             6f")

            __ASM_EMIT("vld1.32         {d0[], d1[]}, [%[src]]")
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

    IF_ARCH_ARM(
        static const float lanczos_kernel_4x3[] __lsp_aligned16 =
        {
            +0.0000000000000000f,
            +0.0073559260471942f,
            +0.0243170840741611f,
            +0.0300210914495816f,

            +0.0000000000000000f,
            -0.0677913359005429f,
            -0.1350949115231170f,
            -0.1328710183650640f,

            +0.0000000000000000f,
            +0.2701898230462341f,
            +0.6079271018540265f,
            +0.8900670517104946f,

            +1.0000000000000000f,
            +0.8900670517104946f,
            +0.6079271018540265f,
            +0.2701898230462341f,

            +0.0000000000000000f,
            -0.1328710183650640f,
            -0.1350949115231170f,
            -0.0677913359005429f,

            +0.0000000000000000f,
            +0.0300210914495816f,
            +0.0243170840741611f,
            +0.0073559260471942f
        };
    )

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
            __ASM_EMIT("vdup.32         q3, d1[1]")
            __ASM_EMIT("vdup.32         q2, d1[0]")
            __ASM_EMIT("vdup.32         q1, d0[1]")
            __ASM_EMIT("vdup.32         q0, d0[0]")

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
            __ASM_EMIT("vdup.32         q1, d0[1]")
            __ASM_EMIT("vdup.32         q0, d0[0]")

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

            __ASM_EMIT("vld1.32         {d0[], d1[]}, [%[src]]")
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

    IF_ARCH_ARM(
        static const float lanczos_kernel_6x2[] __lsp_aligned16 =
        {
            +0.0000000000000000f,
            -0.0078021377848166f,
            -0.0315888188312782f,
            -0.0636843520278618f,

            -0.0854897486982225f,
            -0.0719035699814534f,
            +0.0000000000000000f,
            +0.1409309971636486f,

            +0.3419589947928900f,
            +0.5731591682507563f,
            +0.7897204707819555f,
            +0.9440586719628122f,

            +1.0000000000000000f,
            +0.9440586719628122f,
            +0.7897204707819555f,
            +0.5731591682507563f,

            +0.3419589947928900f,
            +0.1409309971636486f,
            +0.0000000000000000f,
            -0.0719035699814534f,

            -0.0854897486982225f,
            -0.0636843520278618f,
            -0.0315888188312782f,
            -0.0078021377848166f
        };
    )

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
            __ASM_EMIT("vld1.32         {d0[], d1[]}, [%[src]]!")
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

    IF_ARCH_ARM(
        static const float lanczos_kernel_6x3[] __lsp_aligned16 =
        {
            +0.0000000000000000f,
            +0.0032875048460955f,
            +0.0126609519658153f,
            +0.0243170840741611f,

            +0.0310789306368038f,
            +0.0248005479513036f,
            -0.0000000000000000f,
            -0.0424907562338176f,

            -0.0933267410806225f,
            -0.1350949115231170f,
            -0.1458230329384726f,
            -0.1049261531488149f,

            +0.0000000000000000f,
            +0.1676517041508127f,
            +0.3807169003008463f,
            +0.6079271018540265f,

            +0.8103009258121772f,
            +0.9500889005216107f,
            +1.0000000000000000f,
            +0.9500889005216107f,

            +0.8103009258121772f,
            +0.6079271018540265f,
            +0.3807169003008463f,
            +0.1676517041508127f,

            +0.0000000000000000f,
            -0.1049261531488149f,
            -0.1458230329384726f,
            -0.1350949115231170f,

            -0.0933267410806225f,
            -0.0424907562338176f,
            -0.0000000000000000f,
            +0.0248005479513036f,

            +0.0310789306368038f,
            +0.0243170840741611f,
            +0.0126609519658153f,
            +0.0032875048460955f
        };
    )

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
            // 1x blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("vld1.32         {d0[], d1[]}, [%[src]]!")
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

    IF_ARCH_ARM(
        static const float lanczos_kernel_8x2[] __lsp_aligned16 =
        {
            +0.0000000000000000f,
            -0.0043033145538298f,
            -0.0179051851263444f,
            -0.0393892611124141f,

            -0.0636843520278618f,
            -0.0823353965569232f,
            -0.0847248039068907f,
            -0.0600950644541902f,

            +0.0000000000000000f,
            +0.0993408208324369f,
            +0.2353466775191407f,
            +0.3985033193355084f,

            +0.5731591682507563f,
            +0.7396427919997760f,
            +0.8773540711908775f,
            +0.9682457746117045f,

            +1.0000000000000000f,
            +0.9682457746117045f,
            +0.8773540711908775f,
            +0.7396427919997760f,

            +0.5731591682507563f,
            +0.3985033193355084f,
            +0.2353466775191407f,
            +0.0993408208324369f,

            +0.0000000000000000f,
            -0.0600950644541902f,
            -0.0847248039068907f,
            -0.0823353965569232f,

            -0.0636843520278618f,
            -0.0393892611124141f,
            -0.0179051851263444f,
            -0.0043033145538298f
        };
    )

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
            __ASM_EMIT("add             %[d1], %[d0], $0x40")
            __ASM_EMIT("add             %[d2], %[d0], $0x80")
            __ASM_EMIT("add             %[d3], %[d0], $0xc0")
            __ASM_EMIT("vdup.32         q3, d1[1]")
            __ASM_EMIT("vdup.32         q2, d1[0]")
            __ASM_EMIT("vdup.32         q1, d0[1]")
            __ASM_EMIT("vdup.32         q0, d0[0]")
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
            __ASM_EMIT("add             %[d1], %[d0], $0x50")
            __ASM_EMIT("vdup.32         q1, d0[1]")
            __ASM_EMIT("vdup.32         q0, d0[0]")
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

            __ASM_EMIT("vld1.32         {d0[], d1[]}, [%[src]]")
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

    IF_ARCH_ARM(
        static const float lanczos_kernel_8x3[] __lsp_aligned16 =
        {
            +0.0000000000000000f,
            +0.0018368899607481f,
            +0.0073559260471942f,
            +0.0155961678435580f,

            +0.0243170840741611f,
            +0.0303079634725070f,
            +0.0300210914495816f,
            +0.0204366616947175f,

            +0.0000000000000000f,
            -0.0305684889733737f,
            -0.0677913359005429f,
            -0.1054383717904384f,

            -0.1350949115231170f,
            -0.1472651639056537f,
            -0.1328710183650640f,
            -0.0849124693704824f,

            +0.0000000000000000f,
            +0.1205345965259870f,
            +0.2701898230462341f,
            +0.4376469925430009f,

            +0.6079271018540265f,
            +0.7642122243343417f,
            +0.8900670517104946f,
            +0.9717147892357163f,

            +1.0000000000000000f,
            +0.9717147892357163f,
            +0.8900670517104946f,
            +0.7642122243343417f,

            +0.6079271018540265f,
            +0.4376469925430009f,
            +0.2701898230462341f,
            +0.1205345965259870f,

            +0.0000000000000000f,
            -0.0849124693704824f,
            -0.1328710183650640f,
            -0.1472651639056537f,

            -0.1350949115231170f,
            -0.1054383717904384f,
            -0.0677913359005429f,
            -0.0305684889733737f,

            +0.0000000000000000f,
            +0.0204366616947175f,
            +0.0300210914495816f,
            +0.0303079634725070f,

            +0.0243170840741611f,
            +0.0155961678435580f,
            +0.0073559260471942f,
            +0.0018368899607481f
        };
    )

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
            __ASM_EMIT("vldm            %[k0], {q10-q15}")
            __ASM_EMIT("add             %[k1], %[k0], $0x60")
            __ASM_EMIT("add             %[d1], %[d0], $0x60")
            __ASM_EMIT("add             %[d2], %[d0], $0xc0")
            __ASM_EMIT("vdup.32         q3, d1[1]")
            __ASM_EMIT("vdup.32         q2, d1[0]")
            __ASM_EMIT("vdup.32         q1, d0[1]")
            __ASM_EMIT("vdup.32         q0, d0[0]")

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
            __ASM_EMIT("add             %[d1], %[d0], $0x70")
            __ASM_EMIT("vldm            %[k0], {q9-q15}")
            __ASM_EMIT("add             %[k1], %[k0], $0x50")
            __ASM_EMIT("vdup.32         q1, d0[1]")
            __ASM_EMIT("vdup.32         q0, d0[0]")
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

            __ASM_EMIT("vld1.32         {d0[], d1[]}, [%[src]]")
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
