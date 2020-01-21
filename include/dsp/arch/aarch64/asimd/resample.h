/*
 * resample.h
 *
 *  Created on: 20 янв. 2020 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_AARCH64_ASIMD_RESAMPLE_H_
#define DSP_ARCH_AARCH64_ASIMD_RESAMPLE_H_

#ifndef DSP_ARCH_AARCH64_ASIMD_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_AARCH64_ASIMD_IMPL */

namespace asimd
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
        ARCH_AARCH64_ASM(
            // Prepare
            __ASM_EMIT("ldp             q22, q23, [%[kernel]]")     // v22 = 0 k1 0 k0, v23 = 1 k0 0 k1
            // 8x blocks
            __ASM_EMIT("subs            %[count], %[count], #8")
            __ASM_EMIT("b.lo            2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("ldp             q16, q17, [%[src], #0x00]") // v16 = s0 s1 s2 s3, v17 = s4 s5 s6 s7

            __ASM_EMIT("dup             v18.4s, v16.s[0]")
            __ASM_EMIT("dup             v20.4s, v17.s[0]")
            __ASM_EMIT("dup             v19.4s, v16.s[2]")
            __ASM_EMIT("dup             v21.4s, v17.s[2]")
            __ASM_EMIT("ldp             q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("ldp             q2, q3, [%[dst], #0x20]")
            __ASM_EMIT("ldr             q4, [%[dst], #0x40]")
            __ASM_EMIT("fmla            v0.4s, v18.4s, v22.4s")
            __ASM_EMIT("fmla            v1.4s, v19.4s, v22.4s")
            __ASM_EMIT("fmla            v2.4s, v20.4s, v22.4s")
            __ASM_EMIT("fmla            v3.4s, v21.4s, v22.4s")
            __ASM_EMIT("fmla            v1.4s, v18.4s, v23.4s")
            __ASM_EMIT("fmla            v2.4s, v19.4s, v23.4s")
            __ASM_EMIT("fmla            v3.4s, v20.4s, v23.4s")
            __ASM_EMIT("fmla            v4.4s, v21.4s, v23.4s")
            __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("stp             q2, q3, [%[dst], #0x20]")
            __ASM_EMIT("str             q4, [%[dst], #0x40]")

            __ASM_EMIT("dup             v18.4s, v16.s[1]")
            __ASM_EMIT("dup             v20.4s, v17.s[1]")
            __ASM_EMIT("add             %[dst], %[dst], #0x08")
            __ASM_EMIT("dup             v19.4s, v16.s[3]")
            __ASM_EMIT("dup             v21.4s, v17.s[3]")
            __ASM_EMIT("ldp             q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("ldp             q2, q3, [%[dst], #0x20]")
            __ASM_EMIT("ldr             q4, [%[dst], #0x40]")
            __ASM_EMIT("fmla            v0.4s, v18.4s, v22.4s")
            __ASM_EMIT("fmla            v1.4s, v19.4s, v22.4s")
            __ASM_EMIT("fmla            v2.4s, v20.4s, v22.4s")
            __ASM_EMIT("fmla            v3.4s, v21.4s, v22.4s")
            __ASM_EMIT("fmla            v1.4s, v18.4s, v23.4s")
            __ASM_EMIT("fmla            v2.4s, v19.4s, v23.4s")
            __ASM_EMIT("fmla            v3.4s, v20.4s, v23.4s")
            __ASM_EMIT("fmla            v4.4s, v21.4s, v23.4s")
            __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("stp             q2, q3, [%[dst], #0x20]")
            __ASM_EMIT("str             q4, [%[dst], #0x40]")
            __ASM_EMIT("subs            %[count], %[count], #8")
            __ASM_EMIT("add             %[src], %[src], #0x20")
            __ASM_EMIT("add             %[dst], %[dst], #0x38")
            __ASM_EMIT("b.hs            1b")
            __ASM_EMIT("2:")
            // 4x block
            __ASM_EMIT("adds            %[count], %[count], #4")
            __ASM_EMIT("b.lt            4f")

            __ASM_EMIT("ldr             q16, [%[src]]")             // v16 = s0 s1 s2 s3
            __ASM_EMIT("dup             v18.4s, v16.s[0]")
            __ASM_EMIT("dup             v19.4s, v16.s[2]")
            __ASM_EMIT("ldp             q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("ldr             q2, [%[dst], #0x20]")
            __ASM_EMIT("fmla            v0.4s, v18.4s, v22.4s")
            __ASM_EMIT("fmla            v1.4s, v19.4s, v22.4s")
            __ASM_EMIT("fmla            v2.4s, v19.4s, v23.4s")
            __ASM_EMIT("fmla            v1.4s, v18.4s, v23.4s")
            __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("str             q2, [%[dst], #0x20]")

            __ASM_EMIT("dup             v18.4s, v16.s[1]")
            __ASM_EMIT("add             %[dst], %[dst], #0x08")
            __ASM_EMIT("dup             v19.4s, v16.s[3]")
            __ASM_EMIT("ldp             q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("ldr             q2, [%[dst], #0x20]")
            __ASM_EMIT("fmla            v0.4s, v18.4s, v22.4s")
            __ASM_EMIT("fmla            v1.4s, v19.4s, v22.4s")
            __ASM_EMIT("fmla            v2.4s, v19.4s, v23.4s")
            __ASM_EMIT("fmla            v1.4s, v18.4s, v23.4s")
            __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("str             q2, [%[dst], #0x20]")

            __ASM_EMIT("sub             %[count], %[count], #4")
            __ASM_EMIT("add             %[src], %[src], #0x10")
            __ASM_EMIT("add             %[dst], %[dst], #0x18")
            __ASM_EMIT("4:")
            // 1x blocks
            __ASM_EMIT("4:")
            __ASM_EMIT("adds            %[count], %[count], #3")
            __ASM_EMIT("b.lt            6f")
            __ASM_EMIT("7:")
            __ASM_EMIT("ld1r            {v18.4s}, [%[src]]")
            __ASM_EMIT("ldp             q0, q1, [%[dst]]")
            __ASM_EMIT("fmla            v0.4s, v18.4s, v22.4s")
            __ASM_EMIT("fmla            v1.4s, v18.4s, v23.4s")
            __ASM_EMIT("stp             q0, q1, [%[dst]]")
            __ASM_EMIT("subs            %[count], %[count], #1")
            __ASM_EMIT("add             %[src], %[src], #0x04")
            __ASM_EMIT("add             %[dst], %[dst], #0x08")
            __ASM_EMIT("b.ge            7b")
            __ASM_EMIT("6:")

            : [dst] "+r" (dst), [src] "+r" (src),
              [count] "+r" (count)
            : [kernel] "r" (&lanczos_2x2[0])
            : "cc", "memory",
              "q0", "q1", "q2", "q3", "q4",
              "q16", "q17", "q18", "q19", "q20", "q21", "q22", "q23"
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
        ARCH_AARCH64_ASM(
            // Prepare
            __ASM_EMIT("ldp             q22, q23, [%[kernel]]")
            __ASM_EMIT("ldr             q24, [%[kernel], #0x20]")
            // 8x blocks
            __ASM_EMIT("subs            %[count], %[count], #8")
            __ASM_EMIT("b.lo            2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("ldp             q16, q17, [%[src], #0x00]") // v16 = s0 s1 s2 s3, v17 = s4 s5 s6 s7

            __ASM_EMIT("dup             v18.4s, v16.s[0]")
            __ASM_EMIT("dup             v20.4s, v17.s[0]")
            __ASM_EMIT("dup             v19.4s, v16.s[2]")
            __ASM_EMIT("dup             v21.4s, v17.s[2]")
            __ASM_EMIT("ldp             q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("ldp             q2, q3, [%[dst], #0x20]")
            __ASM_EMIT("ldp             q4, q5, [%[dst], #0x40]")
            __ASM_EMIT("fmla            v0.4s, v18.4s, v22.4s")
            __ASM_EMIT("fmla            v1.4s, v19.4s, v22.4s")
            __ASM_EMIT("fmla            v2.4s, v20.4s, v22.4s")
            __ASM_EMIT("fmla            v3.4s, v21.4s, v22.4s")
            __ASM_EMIT("fmla            v1.4s, v18.4s, v23.4s")
            __ASM_EMIT("fmla            v2.4s, v19.4s, v23.4s")
            __ASM_EMIT("fmla            v3.4s, v20.4s, v23.4s")
            __ASM_EMIT("fmla            v4.4s, v21.4s, v23.4s")
            __ASM_EMIT("fmla            v2.4s, v18.4s, v24.4s")
            __ASM_EMIT("fmla            v3.4s, v19.4s, v24.4s")
            __ASM_EMIT("fmla            v4.4s, v20.4s, v24.4s")
            __ASM_EMIT("fmla            v5.4s, v21.4s, v24.4s")
            __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("stp             q2, q3, [%[dst], #0x20]")
            __ASM_EMIT("stp             q4, q5, [%[dst], #0x40]")

            __ASM_EMIT("dup             v18.4s, v16.s[1]")
            __ASM_EMIT("dup             v20.4s, v17.s[1]")
            __ASM_EMIT("add             %[dst], %[dst], #0x08")
            __ASM_EMIT("dup             v19.4s, v16.s[3]")
            __ASM_EMIT("dup             v21.4s, v17.s[3]")
            __ASM_EMIT("ldp             q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("ldp             q2, q3, [%[dst], #0x20]")
            __ASM_EMIT("ldp             q4, q5, [%[dst], #0x40]")
            __ASM_EMIT("fmla            v0.4s, v18.4s, v22.4s")
            __ASM_EMIT("fmla            v1.4s, v19.4s, v22.4s")
            __ASM_EMIT("fmla            v2.4s, v20.4s, v22.4s")
            __ASM_EMIT("fmla            v3.4s, v21.4s, v22.4s")
            __ASM_EMIT("fmla            v1.4s, v18.4s, v23.4s")
            __ASM_EMIT("fmla            v2.4s, v19.4s, v23.4s")
            __ASM_EMIT("fmla            v3.4s, v20.4s, v23.4s")
            __ASM_EMIT("fmla            v4.4s, v21.4s, v23.4s")
            __ASM_EMIT("fmla            v2.4s, v18.4s, v24.4s")
            __ASM_EMIT("fmla            v3.4s, v19.4s, v24.4s")
            __ASM_EMIT("fmla            v4.4s, v20.4s, v24.4s")
            __ASM_EMIT("fmla            v5.4s, v21.4s, v24.4s")
            __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("stp             q2, q3, [%[dst], #0x20]")
            __ASM_EMIT("stp             q4, q5, [%[dst], #0x40]")
            __ASM_EMIT("subs            %[count], %[count], #8")
            __ASM_EMIT("add             %[src], %[src], #0x20")
            __ASM_EMIT("add             %[dst], %[dst], #0x38")
            __ASM_EMIT("b.hs            1b")
            __ASM_EMIT("2:")
            // 4x block
            __ASM_EMIT("adds            %[count], %[count], #4")
            __ASM_EMIT("b.lt            4f")

            __ASM_EMIT("ldr             q16, [%[src]]")             // v16 = s0 s1 s2 s3
            __ASM_EMIT("dup             v18.4s, v16.s[0]")
            __ASM_EMIT("dup             v19.4s, v16.s[2]")
            __ASM_EMIT("ldp             q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("ldp             q2, q3, [%[dst], #0x20]")
            __ASM_EMIT("fmla            v0.4s, v18.4s, v22.4s")
            __ASM_EMIT("fmla            v1.4s, v18.4s, v23.4s")
            __ASM_EMIT("fmla            v2.4s, v18.4s, v24.4s")
            __ASM_EMIT("fmla            v1.4s, v19.4s, v22.4s")
            __ASM_EMIT("fmla            v2.4s, v19.4s, v23.4s")
            __ASM_EMIT("fmla            v3.4s, v19.4s, v24.4s")
            __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("stp             q2, q3, [%[dst], #0x20]")

            __ASM_EMIT("dup             v18.4s, v16.s[1]")
            __ASM_EMIT("add             %[dst], %[dst], #0x08")
            __ASM_EMIT("dup             v19.4s, v16.s[3]")
            __ASM_EMIT("ldp             q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("ldp             q2, q3, [%[dst], #0x20]")
            __ASM_EMIT("fmla            v0.4s, v18.4s, v22.4s")
            __ASM_EMIT("fmla            v1.4s, v18.4s, v23.4s")
            __ASM_EMIT("fmla            v2.4s, v18.4s, v24.4s")
            __ASM_EMIT("fmla            v1.4s, v19.4s, v22.4s")
            __ASM_EMIT("fmla            v2.4s, v19.4s, v23.4s")
            __ASM_EMIT("fmla            v3.4s, v19.4s, v24.4s")
            __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("stp             q2, q3, [%[dst], #0x20]")

            __ASM_EMIT("sub             %[count], %[count], #4")
            __ASM_EMIT("add             %[src], %[src], #0x10")
            __ASM_EMIT("add             %[dst], %[dst], #0x18")
            __ASM_EMIT("4:")
            // 1x blocks
            __ASM_EMIT("4:")
            __ASM_EMIT("adds            %[count], %[count], #3")
            __ASM_EMIT("b.lt            6f")
            __ASM_EMIT("7:")
            __ASM_EMIT("ld1r            {v18.4s}, [%[src]]")
            __ASM_EMIT("ldp             q0, q1, [%[dst]]")
            __ASM_EMIT("ldr             q2, [%[dst], #0x20]")
            __ASM_EMIT("fmla            v0.4s, v18.4s, v22.4s")
            __ASM_EMIT("fmla            v1.4s, v18.4s, v23.4s")
            __ASM_EMIT("fmla            v2.4s, v18.4s, v24.4s")
            __ASM_EMIT("stp             q0, q1, [%[dst]]")
            __ASM_EMIT("str             q2, [%[dst], #0x20]")
            __ASM_EMIT("subs            %[count], %[count], #1")
            __ASM_EMIT("add             %[src], %[src], #0x04")
            __ASM_EMIT("add             %[dst], %[dst], #0x08")
            __ASM_EMIT("b.ge            7b")
            __ASM_EMIT("6:")

            : [dst] "+r" (dst), [src] "+r" (src),
              [count] "+r" (count)
            : [kernel] "r" (&lanczos_2x3[0])
            : "cc", "memory",
              "q0", "q1", "q2", "q3", "q4",
              "q16", "q17", "q18", "q19", "q20", "q21", "q22", "q23",
              "q24"
        );
    }

    static const float lanczos_3x2[] __lsp_aligned16 =
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
        ARCH_AARCH64_ASM(
            // Prepare
            __ASM_EMIT("ldp             q24, q25, [%[kernel], #0x00]")
            __ASM_EMIT("ldp             q26, q27, [%[kernel], #0x20]")
            __ASM_EMIT("ldp             q28, q29, [%[kernel], #0x40]")
            // 2x blocks
            __ASM_EMIT("subs            %[count], %[count], #2")
            __ASM_EMIT("b.lo            2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("ld2r            {v4.4s, v5.4s}, [%[src]]") // v4 = s0, v5 = s1
            __ASM_EMIT("ldp             q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("ldp             q2, q3, [%[dst], #0x20]")
            __ASM_EMIT("fmla            v0.4s, v4.4s, v24.4s")
            __ASM_EMIT("fmla            v1.4s, v4.4s, v25.4s")
            __ASM_EMIT("fmla            v2.4s, v4.4s, v26.4s")
            __ASM_EMIT("fmla            v1.4s, v5.4s, v27.4s")
            __ASM_EMIT("fmla            v2.4s, v5.4s, v28.4s")
            __ASM_EMIT("fmla            v3.4s, v5.4s, v29.4s")
            __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("stp             q2, q3, [%[dst], #0x20]")
            __ASM_EMIT("subs            %[count], %[count], #2")
            __ASM_EMIT("add             %[src], %[src], #0x08")
            __ASM_EMIT("add             %[dst], %[dst], #0x18")
            __ASM_EMIT("b.hs            1b")
            __ASM_EMIT("2:")
            // 1x block
            __ASM_EMIT("adds            %[count], %[count], #1")
            __ASM_EMIT("b.lt            4f")
            __ASM_EMIT("ld1r            {v4.4s}, [%[src]]")
            __ASM_EMIT("ldp             q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("ldr             q2, [%[dst], #0x20]")
            __ASM_EMIT("fmla            v0.4s, v4.4s, v24.4s")
            __ASM_EMIT("fmla            v1.4s, v4.4s, v25.4s")
            __ASM_EMIT("fmla            v2.4s, v4.4s, v26.4s")
            __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("str             q2, [%[dst], #0x20]")
            __ASM_EMIT("4:")

            : [dst] "+r" (dst), [src] "+r" (src),
              [count] "+r" (count)
            : [kernel] "r" (&lanczos_3x2[0])
            : "cc", "memory",
              "q0", "q1", "q2", "q3", "q4", "q5",
              "q24", "q25", "q26", "q27", "q28", "q29"
        );
    }

    static const float lanczos_3x3[] __lsp_aligned16 =
    {
        // Part 1
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
        -0.1993645686793863f,
        -0.1562250559899557f,
        +0.0000000000000000f,

        +0.1055060549370832f,
        +0.0890793429479492f,
        +0.0000000000000000f,
        +0.0000000000000000f,

        // Part 2
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
        ARCH_AARCH64_ASM(
            // Prepare
            __ASM_EMIT("ldp             q22, q23, [%[kernel], #0x00]")
            __ASM_EMIT("ldp             q24, q25, [%[kernel], #0x20]")
            __ASM_EMIT("ldp             q26, q27, [%[kernel], #0x40]")
            __ASM_EMIT("ldp             q28, q29, [%[kernel], #0x60]")
            __ASM_EMIT("ldp             q30, q31, [%[kernel], #0x80]")
            // 2x blocks
            __ASM_EMIT("subs            %[count], %[count], #2")
            __ASM_EMIT("b.lo            2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("ld2r            {v6.4s, v7.4s}, [%[src]]") // v4 = s0, v5 = s1
            __ASM_EMIT("ldp             q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("ldp             q2, q3, [%[dst], #0x20]")
            __ASM_EMIT("ldp             q4, q5, [%[dst], #0x40]")
            __ASM_EMIT("fmla            v0.4s, v6.4s, v22.4s")
            __ASM_EMIT("fmla            v1.4s, v6.4s, v23.4s")
            __ASM_EMIT("fmla            v2.4s, v6.4s, v24.4s")
            __ASM_EMIT("fmla            v3.4s, v6.4s, v25.4s")
            __ASM_EMIT("fmla            v4.4s, v6.4s, v26.4s")
            __ASM_EMIT("fmla            v1.4s, v7.4s, v27.4s")
            __ASM_EMIT("fmla            v2.4s, v7.4s, v28.4s")
            __ASM_EMIT("fmla            v3.4s, v7.4s, v29.4s")
            __ASM_EMIT("fmla            v4.4s, v7.4s, v30.4s")
            __ASM_EMIT("fmla            v5.4s, v7.4s, v31.4s")
            __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("stp             q2, q3, [%[dst], #0x20]")
            __ASM_EMIT("stp             q4, q5, [%[dst], #0x40]")
            __ASM_EMIT("subs            %[count], %[count], #2")
            __ASM_EMIT("add             %[src], %[src], #0x08")
            __ASM_EMIT("add             %[dst], %[dst], #0x18")
            __ASM_EMIT("b.hs            1b")
            __ASM_EMIT("2:")
            // 1x block
            __ASM_EMIT("adds            %[count], %[count], #1")
            __ASM_EMIT("b.lt            4f")
            __ASM_EMIT("ld1r            {v6.4s}, [%[src]]")
            __ASM_EMIT("ldp             q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("ldp             q2, q3, [%[dst], #0x20]")
            __ASM_EMIT("ldr             q4, [%[dst], #0x40]")
            __ASM_EMIT("fmla            v0.4s, v6.4s, v22.4s")
            __ASM_EMIT("fmla            v1.4s, v6.4s, v23.4s")
            __ASM_EMIT("fmla            v2.4s, v6.4s, v24.4s")
            __ASM_EMIT("fmla            v3.4s, v6.4s, v25.4s")
            __ASM_EMIT("fmla            v4.4s, v6.4s, v26.4s")
            __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("stp             q2, q3, [%[dst], #0x20]")
            __ASM_EMIT("str             q4, [%[dst], #0x40]")
            __ASM_EMIT("4:")

            : [dst] "+r" (dst), [src] "+r" (src),
              [count] "+r" (count)
            : [kernel] "r" (&lanczos_3x3[0])
            : "cc", "memory",
              "q0", "q1", "q2", "q3",
              "q4", "q5",
              "q22", "q23",
              "q24", "q25", "q26", "q27",
              "q28", "q29", "q30", "q31"
        );
    }

    static const float lanczos_4x2[] __lsp_aligned16 =
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
        ARCH_AARCH64_ASM(
            // Prepare
            __ASM_EMIT("ldp             q24, q25, [%[kernel], #0x00]")
            __ASM_EMIT("ldp             q26, q27, [%[kernel], #0x20]")
            // 4x blocks
            __ASM_EMIT("subs            %[count], %[count], #4")
            __ASM_EMIT("b.lo            2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("ld4r            {v16.4s, v17.4s, v18.4s, v19.4s}, [%[src]]") // v16 = s0, v17 = s1, v18 = s2, v19 = s3
            __ASM_EMIT("ldp             q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("ldp             q2, q3, [%[dst], #0x20]")
            __ASM_EMIT("ldp             q4, q5, [%[dst], #0x40]")
            __ASM_EMIT("ldr             q6, [%[dst], #0x60]")
            __ASM_EMIT("fmla            v0.4s, v16.4s, v24.4s")
            __ASM_EMIT("fmla            v2.4s, v18.4s, v24.4s")
            __ASM_EMIT("fmla            v1.4s, v16.4s, v25.4s")
            __ASM_EMIT("fmla            v3.4s, v18.4s, v25.4s")
            __ASM_EMIT("fmla            v2.4s, v16.4s, v26.4s")
            __ASM_EMIT("fmla            v4.4s, v18.4s, v26.4s")
            __ASM_EMIT("fmla            v3.4s, v16.4s, v27.4s")
            __ASM_EMIT("fmla            v5.4s, v18.4s, v27.4s")
            __ASM_EMIT("fmla            v1.4s, v17.4s, v24.4s")
            __ASM_EMIT("fmla            v3.4s, v19.4s, v24.4s")
            __ASM_EMIT("fmla            v2.4s, v17.4s, v25.4s")
            __ASM_EMIT("fmla            v4.4s, v19.4s, v25.4s")
            __ASM_EMIT("fmla            v3.4s, v17.4s, v26.4s")
            __ASM_EMIT("fmla            v5.4s, v19.4s, v26.4s")
            __ASM_EMIT("fmla            v4.4s, v17.4s, v27.4s")
            __ASM_EMIT("fmla            v6.4s, v19.4s, v27.4s")
            __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("stp             q2, q3, [%[dst], #0x20]")
            __ASM_EMIT("stp             q4, q5, [%[dst], #0x40]")
            __ASM_EMIT("str             q6, [%[dst], #0x60]")
            __ASM_EMIT("subs            %[count], %[count], #4")
            __ASM_EMIT("add             %[src], %[src], #0x10")
            __ASM_EMIT("add             %[dst], %[dst], #0x40")
            __ASM_EMIT("b.hs            1b")
            __ASM_EMIT("2:")
            // 2x block
            __ASM_EMIT("adds            %[count], %[count], #2")
            __ASM_EMIT("b.lt            4f")
            __ASM_EMIT("1:")
            __ASM_EMIT("ld2r            {v16.4s, v17.4s}, [%[src]]") // v16 = s0, v17 = s1
            __ASM_EMIT("ldp             q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("ldp             q2, q3, [%[dst], #0x20]")
            __ASM_EMIT("ldr             q4, [%[dst], #0x40]")
            __ASM_EMIT("fmla            v0.4s, v16.4s, v24.4s")
            __ASM_EMIT("fmla            v2.4s, v17.4s, v25.4s")
            __ASM_EMIT("fmla            v1.4s, v16.4s, v25.4s")
            __ASM_EMIT("fmla            v3.4s, v17.4s, v26.4s")
            __ASM_EMIT("fmla            v2.4s, v16.4s, v26.4s")
            __ASM_EMIT("fmla            v3.4s, v16.4s, v27.4s")
            __ASM_EMIT("fmla            v4.4s, v17.4s, v27.4s")
            __ASM_EMIT("fmla            v1.4s, v17.4s, v24.4s")
            __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("stp             q2, q3, [%[dst], #0x20]")
            __ASM_EMIT("str             q4, [%[dst], #0x40]")
            __ASM_EMIT("sub             %[count], %[count], #2")
            __ASM_EMIT("add             %[src], %[src], #0x08")
            __ASM_EMIT("add             %[dst], %[dst], #0x20")
            // 1x block
            __ASM_EMIT("4:")
            __ASM_EMIT("adds            %[count], %[count], #1")
            __ASM_EMIT("b.lt            6f")
            __ASM_EMIT("ld1r            {v16.4s}, [%[src]]")
            __ASM_EMIT("ldp             q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("ldp             q2, q3, [%[dst], #0x20]")
            __ASM_EMIT("fmla            v0.4s, v16.4s, v24.4s")
            __ASM_EMIT("fmla            v1.4s, v16.4s, v25.4s")
            __ASM_EMIT("fmla            v2.4s, v16.4s, v26.4s")
            __ASM_EMIT("fmla            v3.4s, v16.4s, v27.4s")
            __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("stp             q2, q3, [%[dst], #0x20]")
            __ASM_EMIT("6:")

            : [dst] "+r" (dst), [src] "+r" (src),
              [count] "+r" (count)
            : [kernel] "r" (&lanczos_4x2[0])
            : "cc", "memory",
              "q0", "q1", "q2", "q3",
              "q4", "q5",
              "q16", "q17", "q18", "q19",
              "q24", "q25", "q26", "q27"
        );
    }

    static const float lanczos_4x3[] __lsp_aligned16 =
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
        ARCH_AARCH64_ASM(
            // Prepare
            __ASM_EMIT("ldp             q24, q25, [%[kernel], #0x00]")
            __ASM_EMIT("ldp             q26, q27, [%[kernel], #0x20]")
            __ASM_EMIT("ldp             q28, q29, [%[kernel], #0x40]")
            // 4x blocks
            __ASM_EMIT("subs            %[count], %[count], #4")
            __ASM_EMIT("b.lo            2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("ld4r            {v16.4s, v17.4s, v18.4s, v19.4s}, [%[src]]") // v16 = s0, v17 = s1, v18 = s2, v19 = s3
            __ASM_EMIT("ldp             q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("ldp             q2, q3, [%[dst], #0x20]")
            __ASM_EMIT("ldp             q4, q5, [%[dst], #0x40]")
            __ASM_EMIT("ldp             q6, q7, [%[dst], #0x60]")
            __ASM_EMIT("ldr             q8, [%[dst], #0x80]")
            __ASM_EMIT("fmla            v0.4s, v16.4s, v24.4s")
            __ASM_EMIT("fmla            v2.4s, v18.4s, v24.4s")
            __ASM_EMIT("fmla            v1.4s, v16.4s, v25.4s")
            __ASM_EMIT("fmla            v3.4s, v18.4s, v25.4s")
            __ASM_EMIT("fmla            v2.4s, v16.4s, v26.4s")
            __ASM_EMIT("fmla            v4.4s, v18.4s, v26.4s")
            __ASM_EMIT("fmla            v3.4s, v16.4s, v27.4s")
            __ASM_EMIT("fmla            v5.4s, v18.4s, v27.4s")
            __ASM_EMIT("fmla            v4.4s, v16.4s, v28.4s")
            __ASM_EMIT("fmla            v6.4s, v18.4s, v28.4s")
            __ASM_EMIT("fmla            v5.4s, v16.4s, v29.4s")
            __ASM_EMIT("fmla            v7.4s, v18.4s, v29.4s")
            __ASM_EMIT("fmla            v1.4s, v17.4s, v24.4s")
            __ASM_EMIT("fmla            v3.4s, v19.4s, v24.4s")
            __ASM_EMIT("fmla            v2.4s, v17.4s, v25.4s")
            __ASM_EMIT("fmla            v4.4s, v19.4s, v25.4s")
            __ASM_EMIT("fmla            v3.4s, v17.4s, v26.4s")
            __ASM_EMIT("fmla            v5.4s, v19.4s, v26.4s")
            __ASM_EMIT("fmla            v4.4s, v17.4s, v27.4s")
            __ASM_EMIT("fmla            v6.4s, v19.4s, v27.4s")
            __ASM_EMIT("fmla            v5.4s, v17.4s, v28.4s")
            __ASM_EMIT("fmla            v7.4s, v19.4s, v28.4s")
            __ASM_EMIT("fmla            v6.4s, v17.4s, v29.4s")
            __ASM_EMIT("fmla            v8.4s, v19.4s, v29.4s")
            __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("stp             q2, q3, [%[dst], #0x20]")
            __ASM_EMIT("stp             q4, q5, [%[dst], #0x40]")
            __ASM_EMIT("stp             q6, q7, [%[dst], #0x60]")
            __ASM_EMIT("str             q8, [%[dst], #0x80]")
            __ASM_EMIT("subs            %[count], %[count], #4")
            __ASM_EMIT("add             %[src], %[src], #0x10")
            __ASM_EMIT("add             %[dst], %[dst], #0x40")
            __ASM_EMIT("b.hs            1b")
            __ASM_EMIT("2:")
            // 2x block
            __ASM_EMIT("adds            %[count], %[count], #2")
            __ASM_EMIT("b.lt            4f")
            __ASM_EMIT("1:")
            __ASM_EMIT("ld2r            {v16.4s, v17.4s}, [%[src]]") // v16 = s0, v17 = s1
            __ASM_EMIT("ldp             q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("ldp             q2, q3, [%[dst], #0x20]")
            __ASM_EMIT("ldp             q4, q5, [%[dst], #0x40]")
            __ASM_EMIT("ldr             q6, [%[dst], #0x60]")
            __ASM_EMIT("fmla            v1.4s, v17.4s, v24.4s")
            __ASM_EMIT("fmla            v0.4s, v16.4s, v24.4s")
            __ASM_EMIT("fmla            v2.4s, v17.4s, v25.4s")
            __ASM_EMIT("fmla            v1.4s, v16.4s, v25.4s")
            __ASM_EMIT("fmla            v3.4s, v17.4s, v26.4s")
            __ASM_EMIT("fmla            v2.4s, v16.4s, v26.4s")
            __ASM_EMIT("fmla            v4.4s, v17.4s, v27.4s")
            __ASM_EMIT("fmla            v3.4s, v16.4s, v27.4s")
            __ASM_EMIT("fmla            v5.4s, v17.4s, v28.4s")
            __ASM_EMIT("fmla            v4.4s, v16.4s, v28.4s")
            __ASM_EMIT("fmla            v5.4s, v16.4s, v29.4s")
            __ASM_EMIT("fmla            v6.4s, v17.4s, v29.4s")
            __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("stp             q2, q3, [%[dst], #0x20]")
            __ASM_EMIT("stp             q4, q5, [%[dst], #0x40]")
            __ASM_EMIT("str             q6, [%[dst], #0x60]")
            __ASM_EMIT("sub             %[count], %[count], #2")
            __ASM_EMIT("add             %[src], %[src], #0x08")
            __ASM_EMIT("add             %[dst], %[dst], #0x20")
            // 1x block
            __ASM_EMIT("4:")
            __ASM_EMIT("adds            %[count], %[count], #1")
            __ASM_EMIT("b.lt            6f")
            __ASM_EMIT("ld1r            {v16.4s}, [%[src]]")
            __ASM_EMIT("ldp             q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("ldp             q2, q3, [%[dst], #0x20]")
            __ASM_EMIT("ldp             q4, q5, [%[dst], #0x40]")
            __ASM_EMIT("fmla            v0.4s, v16.4s, v24.4s")
            __ASM_EMIT("fmla            v1.4s, v16.4s, v25.4s")
            __ASM_EMIT("fmla            v2.4s, v16.4s, v26.4s")
            __ASM_EMIT("fmla            v3.4s, v16.4s, v27.4s")
            __ASM_EMIT("fmla            v4.4s, v16.4s, v28.4s")
            __ASM_EMIT("fmla            v5.4s, v16.4s, v29.4s")
            __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("stp             q2, q3, [%[dst], #0x20]")
            __ASM_EMIT("stp             q4, q5, [%[dst], #0x40]")
            __ASM_EMIT("6:")

            : [dst] "+r" (dst), [src] "+r" (src),
              [count] "+r" (count)
            : [kernel] "r" (&lanczos_4x3[0])
            : "cc", "memory",
              "q0", "q1", "q2", "q3",
              "q4", "q5", "q6", "q7",
              "q8",
              "q16", "q17", "q18", "q19",
              "q24", "q25", "q26", "q27",
              "q28", "q29"
        );
    }

    static const float lanczos_6x2[] __lsp_aligned16 =
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
        ARCH_AARCH64_ASM(
            // Prepare
            __ASM_EMIT("ldp             q24, q25, [%[kernel], #0x00]")
            __ASM_EMIT("ldp             q26, q27, [%[kernel], #0x20]")
            __ASM_EMIT("ldp             q28, q29, [%[kernel], #0x40]")
            // 4x blocks
            __ASM_EMIT("subs            %[count], %[count], #4")
            __ASM_EMIT("b.lo            2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("ld4r            {v16.4s, v17.4s, v18.4s, v19.4s}, [%[src]]") // v16 = s0, v17 = s1, v18 = s2, v19 = s3

            __ASM_EMIT("ldp             q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("ldp             q2, q3, [%[dst], #0x20]")
            __ASM_EMIT("ldp             q4, q5, [%[dst], #0x40]")
            __ASM_EMIT("ldp             q6, q7, [%[dst], #0x60]")
            __ASM_EMIT("ldr             q8, [%[dst], #0x80]")
            __ASM_EMIT("fmla            v0.4s, v16.4s, v24.4s")
            __ASM_EMIT("fmla            v3.4s, v18.4s, v24.4s")
            __ASM_EMIT("fmla            v1.4s, v16.4s, v25.4s")
            __ASM_EMIT("fmla            v4.4s, v18.4s, v25.4s")
            __ASM_EMIT("fmla            v2.4s, v16.4s, v26.4s")
            __ASM_EMIT("fmla            v5.4s, v18.4s, v26.4s")
            __ASM_EMIT("fmla            v3.4s, v16.4s, v27.4s")
            __ASM_EMIT("fmla            v6.4s, v18.4s, v27.4s")
            __ASM_EMIT("fmla            v4.4s, v16.4s, v28.4s")
            __ASM_EMIT("fmla            v7.4s, v18.4s, v28.4s")
            __ASM_EMIT("fmla            v5.4s, v16.4s, v29.4s")
            __ASM_EMIT("fmla            v8.4s, v18.4s, v29.4s")
            __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("stp             q2, q3, [%[dst], #0x20]")
            __ASM_EMIT("stp             q4, q5, [%[dst], #0x40]")
            __ASM_EMIT("stp             q6, q7, [%[dst], #0x60]")
            __ASM_EMIT("str             q8, [%[dst], #0x80]")
            __ASM_EMIT("add             %[dst], %[dst], #0x18")

            __ASM_EMIT("ldp             q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("ldp             q2, q3, [%[dst], #0x20]")
            __ASM_EMIT("ldp             q4, q5, [%[dst], #0x40]")
            __ASM_EMIT("ldp             q6, q7, [%[dst], #0x60]")
            __ASM_EMIT("ldr             q8, [%[dst], #0x80]")
            __ASM_EMIT("fmla            v0.4s, v17.4s, v24.4s")
            __ASM_EMIT("fmla            v3.4s, v19.4s, v24.4s")
            __ASM_EMIT("fmla            v1.4s, v17.4s, v25.4s")
            __ASM_EMIT("fmla            v4.4s, v19.4s, v25.4s")
            __ASM_EMIT("fmla            v2.4s, v17.4s, v26.4s")
            __ASM_EMIT("fmla            v5.4s, v19.4s, v26.4s")
            __ASM_EMIT("fmla            v3.4s, v17.4s, v27.4s")
            __ASM_EMIT("fmla            v6.4s, v19.4s, v27.4s")
            __ASM_EMIT("fmla            v4.4s, v17.4s, v28.4s")
            __ASM_EMIT("fmla            v7.4s, v19.4s, v28.4s")
            __ASM_EMIT("fmla            v5.4s, v17.4s, v29.4s")
            __ASM_EMIT("fmla            v8.4s, v19.4s, v29.4s")
            __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("stp             q2, q3, [%[dst], #0x20]")
            __ASM_EMIT("stp             q4, q5, [%[dst], #0x40]")
            __ASM_EMIT("stp             q6, q7, [%[dst], #0x60]")
            __ASM_EMIT("str             q8, [%[dst], #0x80]")

            __ASM_EMIT("subs            %[count], %[count], #4")
            __ASM_EMIT("add             %[src], %[src], #0x10")
            __ASM_EMIT("add             %[dst], %[dst], #0x48")
            __ASM_EMIT("b.hs            1b")
            __ASM_EMIT("2:")
            // 1x blocks
            __ASM_EMIT("4:")
            __ASM_EMIT("adds            %[count], %[count], #3")
            __ASM_EMIT("b.lt            4f")
            __ASM_EMIT("3:")
            __ASM_EMIT("ld1r            {v16.4s}, [%[src]]")
            __ASM_EMIT("ldp             q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("ldp             q2, q3, [%[dst], #0x20]")
            __ASM_EMIT("ldp             q4, q5, [%[dst], #0x40]")
            __ASM_EMIT("fmla            v0.4s, v16.4s, v24.4s")
            __ASM_EMIT("fmla            v1.4s, v16.4s, v25.4s")
            __ASM_EMIT("fmla            v2.4s, v16.4s, v26.4s")
            __ASM_EMIT("fmla            v3.4s, v16.4s, v27.4s")
            __ASM_EMIT("fmla            v4.4s, v16.4s, v28.4s")
            __ASM_EMIT("fmla            v5.4s, v16.4s, v29.4s")
            __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("stp             q2, q3, [%[dst], #0x20]")
            __ASM_EMIT("stp             q4, q5, [%[dst], #0x40]")
            __ASM_EMIT("subs            %[count], %[count], #1")
            __ASM_EMIT("add             %[src], %[src], #0x04")
            __ASM_EMIT("add             %[dst], %[dst], #0x18")
            __ASM_EMIT("b.ge            3b")
            __ASM_EMIT("4:")

            : [dst] "+r" (dst), [src] "+r" (src),
              [count] "+r" (count)
            : [kernel] "r" (&lanczos_6x2[0])
            : "cc", "memory",
              "q0", "q1", "q2", "q3",
              "q4", "q5", "q6", "q7",
              "q8",
              "q16", "q17", "q18", "q19",
              "q24", "q25", "q26", "q27",
              "q28", "q29"
        );
    }

    static const float lanczos_6x3[] __lsp_aligned16 =
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
        ARCH_AARCH64_ASM(
            // Prepare
            __ASM_EMIT("ldp             q22, q23, [%[kernel], #0x00]")
            __ASM_EMIT("ldp             q24, q25, [%[kernel], #0x20]")
            __ASM_EMIT("ldp             q26, q27, [%[kernel], #0x40]")
            __ASM_EMIT("ldp             q28, q29, [%[kernel], #0x60]")
            __ASM_EMIT("ldr             q30, [%[kernel], #0x80]")
            // 4x blocks
            __ASM_EMIT("subs            %[count], %[count], #4")
            __ASM_EMIT("b.lo            2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("ld4r            {v16.4s, v17.4s, v18.4s, v19.4s}, [%[src]]") // v16 = s0, v17 = s1, v18 = s2, v19 = s3

            __ASM_EMIT("ldp             q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("ldp             q2, q3, [%[dst], #0x20]")
            __ASM_EMIT("ldp             q4, q5, [%[dst], #0x40]")
            __ASM_EMIT("ldp             q6, q7, [%[dst], #0x60]")
            __ASM_EMIT("ldp             q8, q9, [%[dst], #0x80]")
            __ASM_EMIT("ldp             q10, q11, [%[dst], #0xa0]")
            __ASM_EMIT("fmla            v0.4s, v16.4s, v22.4s")
            __ASM_EMIT("fmla            v3.4s, v18.4s, v22.4s")
            __ASM_EMIT("fmla            v1.4s, v16.4s, v23.4s")
            __ASM_EMIT("fmla            v4.4s, v18.4s, v23.4s")
            __ASM_EMIT("fmla            v2.4s, v16.4s, v24.4s")
            __ASM_EMIT("fmla            v5.4s, v18.4s, v24.4s")
            __ASM_EMIT("fmla            v3.4s, v16.4s, v25.4s")
            __ASM_EMIT("fmla            v6.4s, v18.4s, v25.4s")
            __ASM_EMIT("fmla            v4.4s, v16.4s, v26.4s")
            __ASM_EMIT("fmla            v7.4s, v18.4s, v26.4s")
            __ASM_EMIT("fmla            v5.4s, v16.4s, v27.4s")
            __ASM_EMIT("fmla            v8.4s, v18.4s, v27.4s")
            __ASM_EMIT("fmla            v6.4s, v16.4s, v28.4s")
            __ASM_EMIT("fmla            v9.4s, v18.4s, v28.4s")
            __ASM_EMIT("fmla            v7.4s, v16.4s, v29.4s")
            __ASM_EMIT("fmla            v10.4s, v18.4s, v29.4s")
            __ASM_EMIT("fmla            v8.4s, v16.4s, v30.4s")
            __ASM_EMIT("fmla            v11.4s, v18.4s, v30.4s")
            __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("stp             q2, q3, [%[dst], #0x20]")
            __ASM_EMIT("stp             q4, q5, [%[dst], #0x40]")
            __ASM_EMIT("stp             q6, q7, [%[dst], #0x60]")
            __ASM_EMIT("stp             q8, q9, [%[dst], #0x80]")
            __ASM_EMIT("stp             q10, q11, [%[dst], #0xa0]")
            __ASM_EMIT("add             %[dst], %[dst], #0x18")

            __ASM_EMIT("ldp             q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("ldp             q2, q3, [%[dst], #0x20]")
            __ASM_EMIT("ldp             q4, q5, [%[dst], #0x40]")
            __ASM_EMIT("ldp             q6, q7, [%[dst], #0x60]")
            __ASM_EMIT("ldp             q8, q9, [%[dst], #0x80]")
            __ASM_EMIT("ldp             q10, q11, [%[dst], #0xa0]")
            __ASM_EMIT("fmla            v0.4s, v17.4s, v22.4s")
            __ASM_EMIT("fmla            v3.4s, v19.4s, v22.4s")
            __ASM_EMIT("fmla            v1.4s, v17.4s, v23.4s")
            __ASM_EMIT("fmla            v4.4s, v19.4s, v23.4s")
            __ASM_EMIT("fmla            v2.4s, v17.4s, v24.4s")
            __ASM_EMIT("fmla            v5.4s, v19.4s, v24.4s")
            __ASM_EMIT("fmla            v3.4s, v17.4s, v25.4s")
            __ASM_EMIT("fmla            v6.4s, v19.4s, v25.4s")
            __ASM_EMIT("fmla            v4.4s, v17.4s, v26.4s")
            __ASM_EMIT("fmla            v7.4s, v19.4s, v26.4s")
            __ASM_EMIT("fmla            v5.4s, v17.4s, v27.4s")
            __ASM_EMIT("fmla            v8.4s, v19.4s, v27.4s")
            __ASM_EMIT("fmla            v6.4s, v17.4s, v28.4s")
            __ASM_EMIT("fmla            v9.4s, v19.4s, v28.4s")
            __ASM_EMIT("fmla            v7.4s, v17.4s, v29.4s")
            __ASM_EMIT("fmla            v10.4s, v19.4s, v29.4s")
            __ASM_EMIT("fmla            v8.4s, v17.4s, v30.4s")
            __ASM_EMIT("fmla            v11.4s, v19.4s, v30.4s")
            __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("stp             q2, q3, [%[dst], #0x20]")
            __ASM_EMIT("stp             q4, q5, [%[dst], #0x40]")
            __ASM_EMIT("stp             q6, q7, [%[dst], #0x60]")
            __ASM_EMIT("stp             q8, q9, [%[dst], #0x80]")
            __ASM_EMIT("stp             q10, q11, [%[dst], #0xa0]")

            __ASM_EMIT("subs            %[count], %[count], #4")
            __ASM_EMIT("add             %[src], %[src], #0x10")
            __ASM_EMIT("add             %[dst], %[dst], #0x48")
            __ASM_EMIT("b.hs            1b")
            __ASM_EMIT("2:")
            // 1x blocks
            __ASM_EMIT("4:")
            __ASM_EMIT("adds            %[count], %[count], #3")
            __ASM_EMIT("b.lt            4f")
            __ASM_EMIT("3:")
            __ASM_EMIT("ld1r            {v16.4s}, [%[src]]")
            __ASM_EMIT("ldp             q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("ldp             q2, q3, [%[dst], #0x20]")
            __ASM_EMIT("ldp             q4, q5, [%[dst], #0x40]")
            __ASM_EMIT("ldp             q6, q7, [%[dst], #0x60]")
            __ASM_EMIT("ldr             q8, [%[dst], #0x80]")
            __ASM_EMIT("fmla            v0.4s, v16.4s, v22.4s")
            __ASM_EMIT("fmla            v1.4s, v16.4s, v23.4s")
            __ASM_EMIT("fmla            v2.4s, v16.4s, v24.4s")
            __ASM_EMIT("fmla            v3.4s, v16.4s, v25.4s")
            __ASM_EMIT("fmla            v4.4s, v16.4s, v26.4s")
            __ASM_EMIT("fmla            v5.4s, v16.4s, v27.4s")
            __ASM_EMIT("fmla            v6.4s, v16.4s, v28.4s")
            __ASM_EMIT("fmla            v7.4s, v16.4s, v29.4s")
            __ASM_EMIT("fmla            v8.4s, v16.4s, v30.4s")
            __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("stp             q2, q3, [%[dst], #0x20]")
            __ASM_EMIT("stp             q4, q5, [%[dst], #0x40]")
            __ASM_EMIT("stp             q6, q7, [%[dst], #0x60]")
            __ASM_EMIT("str             q8, [%[dst], #0x80]")
            __ASM_EMIT("subs            %[count], %[count], #1")
            __ASM_EMIT("add             %[src], %[src], #0x04")
            __ASM_EMIT("add             %[dst], %[dst], #0x18")
            __ASM_EMIT("b.ge            3b")
            __ASM_EMIT("4:")

            : [dst] "+r" (dst), [src] "+r" (src),
              [count] "+r" (count)
            : [kernel] "r" (&lanczos_6x3[0])
            : "cc", "memory",
              "q0", "q1", "q2", "q3",
              "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11",
              "q16", "q17", "q18", "q19",
              "q22", "q23",
              "q24", "q25", "q26", "q27",
              "q28", "q29", "q30"
        );
    }

    static const float lanczos_8x2[] __lsp_aligned16 =
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
        ARCH_AARCH64_ASM(
            // Prepare
            __ASM_EMIT("ldp             q24, q25, [%[kernel], #0x00]")
            __ASM_EMIT("ldp             q26, q27, [%[kernel], #0x20]")
            __ASM_EMIT("ldp             q28, q29, [%[kernel], #0x40]")
            __ASM_EMIT("ldp             q30, q31, [%[kernel], #0x60]")
            // 4x blocks
            __ASM_EMIT("subs            %[count], %[count], #4")
            __ASM_EMIT("b.lo            2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("ld4r            {v16.4s, v17.4s, v18.4s, v19.4s}, [%[src]]") // v16 = s0, v17 = s1, v18 = s2, v19 = s3
            __ASM_EMIT("ldp             q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("ldp             q2, q3, [%[dst], #0x20]")
            __ASM_EMIT("ldp             q4, q5, [%[dst], #0x40]")
            __ASM_EMIT("ldp             q6, q7, [%[dst], #0x60]")
            __ASM_EMIT("ldp             q8, q9, [%[dst], #0x80]")
            __ASM_EMIT("ldp             q10, q11, [%[dst], #0xa0]")
            __ASM_EMIT("ldp             q12, q13, [%[dst], #0xc0]")

            __ASM_EMIT("fmla            v0.4s, v16.4s, v24.4s")
            __ASM_EMIT("fmla            v1.4s, v16.4s, v25.4s")
            __ASM_EMIT("fmla            v4.4s, v18.4s, v24.4s")
            __ASM_EMIT("fmla            v5.4s, v18.4s, v25.4s")
            __ASM_EMIT("fmla            v2.4s, v16.4s, v26.4s")
            __ASM_EMIT("fmla            v3.4s, v16.4s, v27.4s")
            __ASM_EMIT("fmla            v6.4s, v18.4s, v26.4s")
            __ASM_EMIT("fmla            v7.4s, v18.4s, v27.4s")
            __ASM_EMIT("fmla            v4.4s, v16.4s, v28.4s")
            __ASM_EMIT("fmla            v5.4s, v16.4s, v29.4s")
            __ASM_EMIT("fmla            v8.4s, v18.4s, v28.4s")
            __ASM_EMIT("fmla            v9.4s, v18.4s, v29.4s")
            __ASM_EMIT("fmla            v6.4s, v16.4s, v30.4s")
            __ASM_EMIT("fmla            v7.4s, v16.4s, v31.4s")
            __ASM_EMIT("fmla            v10.4s, v18.4s, v30.4s")
            __ASM_EMIT("fmla            v11.4s, v18.4s, v31.4s")

            __ASM_EMIT("fmla            v2.4s, v17.4s, v24.4s")
            __ASM_EMIT("fmla            v3.4s, v17.4s, v25.4s")
            __ASM_EMIT("fmla            v6.4s, v19.4s, v24.4s")
            __ASM_EMIT("fmla            v7.4s, v19.4s, v25.4s")
            __ASM_EMIT("fmla            v4.4s, v17.4s, v26.4s")
            __ASM_EMIT("fmla            v5.4s, v17.4s, v27.4s")
            __ASM_EMIT("fmla            v8.4s, v19.4s, v26.4s")
            __ASM_EMIT("fmla            v9.4s, v19.4s, v27.4s")
            __ASM_EMIT("fmla            v6.4s, v17.4s, v28.4s")
            __ASM_EMIT("fmla            v7.4s, v17.4s, v29.4s")
            __ASM_EMIT("fmla            v10.4s, v19.4s, v28.4s")
            __ASM_EMIT("fmla            v11.4s, v19.4s, v29.4s")
            __ASM_EMIT("fmla            v8.4s, v17.4s, v30.4s")
            __ASM_EMIT("fmla            v9.4s, v17.4s, v31.4s")
            __ASM_EMIT("fmla            v12.4s, v19.4s, v30.4s")
            __ASM_EMIT("fmla            v13.4s, v19.4s, v31.4s")

            __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("stp             q2, q3, [%[dst], #0x20]")
            __ASM_EMIT("stp             q4, q5, [%[dst], #0x40]")
            __ASM_EMIT("stp             q6, q7, [%[dst], #0x60]")
            __ASM_EMIT("stp             q8, q9, [%[dst], #0x80]")
            __ASM_EMIT("stp             q10, q11, [%[dst], #0xa0]")
            __ASM_EMIT("stp             q12, q13, [%[dst], #0xc0]")
            __ASM_EMIT("subs            %[count], %[count], #4")
            __ASM_EMIT("add             %[src], %[src], #0x10")
            __ASM_EMIT("add             %[dst], %[dst], #0x80")
            __ASM_EMIT("b.hs            1b")
            __ASM_EMIT("2:")
            // 2x block
            __ASM_EMIT("adds            %[count], %[count], #2")
            __ASM_EMIT("b.lt            4f")
            __ASM_EMIT("ld2r            {v16.4s, v17.4s}, [%[src]]") // v16 = s0, v17 = s1
            __ASM_EMIT("ldp             q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("ldp             q2, q3, [%[dst], #0x20]")
            __ASM_EMIT("ldp             q4, q5, [%[dst], #0x40]")
            __ASM_EMIT("ldp             q6, q7, [%[dst], #0x60]")
            __ASM_EMIT("ldp             q8, q9, [%[dst], #0x80]")

            __ASM_EMIT("fmla            v0.4s, v16.4s, v24.4s")
            __ASM_EMIT("fmla            v1.4s, v16.4s, v25.4s")
            __ASM_EMIT("fmla            v2.4s, v17.4s, v24.4s")
            __ASM_EMIT("fmla            v3.4s, v17.4s, v25.4s")
            __ASM_EMIT("fmla            v2.4s, v16.4s, v26.4s")
            __ASM_EMIT("fmla            v3.4s, v16.4s, v27.4s")
            __ASM_EMIT("fmla            v4.4s, v17.4s, v26.4s")
            __ASM_EMIT("fmla            v5.4s, v17.4s, v27.4s")
            __ASM_EMIT("fmla            v4.4s, v16.4s, v28.4s")
            __ASM_EMIT("fmla            v5.4s, v16.4s, v29.4s")
            __ASM_EMIT("fmla            v6.4s, v17.4s, v28.4s")
            __ASM_EMIT("fmla            v7.4s, v17.4s, v29.4s")
            __ASM_EMIT("fmla            v6.4s, v16.4s, v30.4s")
            __ASM_EMIT("fmla            v7.4s, v16.4s, v31.4s")
            __ASM_EMIT("fmla            v8.4s, v17.4s, v30.4s")
            __ASM_EMIT("fmla            v9.4s, v17.4s, v31.4s")

            __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("stp             q2, q3, [%[dst], #0x20]")
            __ASM_EMIT("stp             q4, q5, [%[dst], #0x40]")
            __ASM_EMIT("stp             q6, q7, [%[dst], #0x60]")
            __ASM_EMIT("stp             q8, q9, [%[dst], #0x80]")
            __ASM_EMIT("sub             %[count], %[count], #2")
            __ASM_EMIT("add             %[src], %[src], #0x08")
            __ASM_EMIT("add             %[dst], %[dst], #0x40")
            __ASM_EMIT("4:")
            // 1x block
            __ASM_EMIT("adds            %[count], %[count], #1")
            __ASM_EMIT("b.lt            6f")
            __ASM_EMIT("ld1r            {v16.4s}, [%[src]]")

            __ASM_EMIT("ldp             q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("ldp             q2, q3, [%[dst], #0x20]")
            __ASM_EMIT("ldp             q4, q5, [%[dst], #0x40]")
            __ASM_EMIT("ldp             q6, q7, [%[dst], #0x60]")

            __ASM_EMIT("fmla            v0.4s, v16.4s, v24.4s")
            __ASM_EMIT("fmla            v1.4s, v16.4s, v25.4s")
            __ASM_EMIT("fmla            v2.4s, v16.4s, v26.4s")
            __ASM_EMIT("fmla            v3.4s, v16.4s, v27.4s")
            __ASM_EMIT("fmla            v4.4s, v16.4s, v28.4s")
            __ASM_EMIT("fmla            v5.4s, v16.4s, v29.4s")
            __ASM_EMIT("fmla            v6.4s, v16.4s, v30.4s")
            __ASM_EMIT("fmla            v7.4s, v16.4s, v31.4s")

            __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("stp             q2, q3, [%[dst], #0x20]")
            __ASM_EMIT("stp             q4, q5, [%[dst], #0x40]")
            __ASM_EMIT("stp             q6, q7, [%[dst], #0x60]")
            __ASM_EMIT("6:")

            : [dst] "+r" (dst), [src] "+r" (src),
              [count] "+r" (count)
            : [kernel] "r" (&lanczos_8x2[0])
            : "cc", "memory",
              "q0", "q1", "q2", "q3",
              "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11",
              "q12", "q13",
              "q16", "q17", "q18", "q19",
              "q24", "q25", "q26", "q27",
              "q28", "q29", "q30", "q31"
        );
    }

    static const float lanczos_8x3[] __lsp_aligned16 =
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
        ARCH_AARCH64_ASM(
            // Prepare
            __ASM_EMIT("ldp             q20, q21, [%[kernel], #0x00]")
            __ASM_EMIT("ldp             q22, q23, [%[kernel], #0x20]")
            __ASM_EMIT("ldp             q24, q25, [%[kernel], #0x40]")
            __ASM_EMIT("ldp             q26, q27, [%[kernel], #0x60]")
            __ASM_EMIT("ldp             q28, q29, [%[kernel], #0x80]")
            __ASM_EMIT("ldp             q30, q31, [%[kernel], #0xa0]")
            // 2x blocks
            __ASM_EMIT("subs            %[count], %[count], #2")
            __ASM_EMIT("b.lo            2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("ld2r            {v16.4s, v17.4s}, [%[src]]") // v16 = s0, v17 = s1
            __ASM_EMIT("ldp             q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("ldp             q2, q3, [%[dst], #0x20]")
            __ASM_EMIT("ldp             q4, q5, [%[dst], #0x40]")
            __ASM_EMIT("ldp             q6, q7, [%[dst], #0x60]")
            __ASM_EMIT("ldp             q8, q9, [%[dst], #0x80]")
            __ASM_EMIT("ldp             q10, q11, [%[dst], #0xa0]")
            __ASM_EMIT("ldp             q12, q13, [%[dst], #0xc0]")

            __ASM_EMIT("fmla            v0.4s, v16.4s, v20.4s")
            __ASM_EMIT("fmla            v1.4s, v16.4s, v21.4s")
            __ASM_EMIT("fmla            v2.4s, v17.4s, v20.4s")
            __ASM_EMIT("fmla            v3.4s, v17.4s, v21.4s")
            __ASM_EMIT("fmla            v2.4s, v16.4s, v22.4s")
            __ASM_EMIT("fmla            v3.4s, v16.4s, v23.4s")
            __ASM_EMIT("fmla            v4.4s, v17.4s, v22.4s")
            __ASM_EMIT("fmla            v5.4s, v17.4s, v23.4s")
            __ASM_EMIT("fmla            v4.4s, v16.4s, v24.4s")
            __ASM_EMIT("fmla            v5.4s, v16.4s, v25.4s")
            __ASM_EMIT("fmla            v6.4s, v17.4s, v24.4s")
            __ASM_EMIT("fmla            v7.4s, v17.4s, v25.4s")
            __ASM_EMIT("fmla            v6.4s, v16.4s, v26.4s")
            __ASM_EMIT("fmla            v7.4s, v16.4s, v27.4s")
            __ASM_EMIT("fmla            v8.4s, v17.4s, v26.4s")
            __ASM_EMIT("fmla            v9.4s, v17.4s, v27.4s")
            __ASM_EMIT("fmla            v8.4s, v16.4s, v28.4s")
            __ASM_EMIT("fmla            v9.4s, v16.4s, v29.4s")
            __ASM_EMIT("fmla            v10.4s, v17.4s, v28.4s")
            __ASM_EMIT("fmla            v11.4s, v17.4s, v29.4s")
            __ASM_EMIT("fmla            v10.4s, v16.4s, v30.4s")
            __ASM_EMIT("fmla            v11.4s, v16.4s, v31.4s")
            __ASM_EMIT("fmla            v12.4s, v17.4s, v30.4s")
            __ASM_EMIT("fmla            v13.4s, v17.4s, v31.4s")

            __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("stp             q2, q3, [%[dst], #0x20]")
            __ASM_EMIT("stp             q4, q5, [%[dst], #0x40]")
            __ASM_EMIT("stp             q6, q7, [%[dst], #0x60]")
            __ASM_EMIT("stp             q8, q9, [%[dst], #0x80]")
            __ASM_EMIT("stp             q10, q11, [%[dst], #0xa0]")
            __ASM_EMIT("stp             q12, q13, [%[dst], #0xc0]")
            __ASM_EMIT("subs            %[count], %[count], #2")
            __ASM_EMIT("add             %[src], %[src], #0x08")
            __ASM_EMIT("add             %[dst], %[dst], #0x40")
            __ASM_EMIT("b.hs            1b")
            __ASM_EMIT("2:")

            // 1x block
            __ASM_EMIT("adds            %[count], %[count], #1")
            __ASM_EMIT("b.lt            4f")
            __ASM_EMIT("ld1r            {v16.4s}, [%[src]]")

            __ASM_EMIT("ldp             q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("ldp             q2, q3, [%[dst], #0x20]")
            __ASM_EMIT("ldp             q4, q5, [%[dst], #0x40]")
            __ASM_EMIT("ldp             q6, q7, [%[dst], #0x60]")
            __ASM_EMIT("ldp             q8, q9, [%[dst], #0x80]")
            __ASM_EMIT("ldp             q10, q11, [%[dst], #0xa0]")

            __ASM_EMIT("fmla            v0.4s, v16.4s, v20.4s")
            __ASM_EMIT("fmla            v1.4s, v16.4s, v21.4s")
            __ASM_EMIT("fmla            v2.4s, v16.4s, v22.4s")
            __ASM_EMIT("fmla            v3.4s, v16.4s, v23.4s")
            __ASM_EMIT("fmla            v4.4s, v16.4s, v24.4s")
            __ASM_EMIT("fmla            v5.4s, v16.4s, v25.4s")
            __ASM_EMIT("fmla            v6.4s, v16.4s, v26.4s")
            __ASM_EMIT("fmla            v7.4s, v16.4s, v27.4s")
            __ASM_EMIT("fmla            v8.4s, v16.4s, v28.4s")
            __ASM_EMIT("fmla            v9.4s, v16.4s, v29.4s")
            __ASM_EMIT("fmla            v10.4s, v16.4s, v30.4s")
            __ASM_EMIT("fmla            v11.4s, v16.4s, v31.4s")

            __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("stp             q2, q3, [%[dst], #0x20]")
            __ASM_EMIT("stp             q4, q5, [%[dst], #0x40]")
            __ASM_EMIT("stp             q6, q7, [%[dst], #0x60]")
            __ASM_EMIT("stp             q8, q9, [%[dst], #0x80]")
            __ASM_EMIT("stp             q10, q11, [%[dst], #0xa0]")
            __ASM_EMIT("4:")

            : [dst] "+r" (dst), [src] "+r" (src),
              [count] "+r" (count)
            : [kernel] "r" (&lanczos_8x3[0])
            : "cc", "memory",
              "q0", "q1", "q2", "q3",
              "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11",
              "q12", "q13",
              "q16", "q17",
              "q24", "q25", "q26", "q27",
              "q28", "q29", "q30", "q31"
        );
    }
}


#endif /* DSP_ARCH_AARCH64_ASIMD_RESAMPLE_H_ */
