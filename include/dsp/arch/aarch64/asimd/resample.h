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
}


#endif /* DSP_ARCH_AARCH64_ASIMD_RESAMPLE_H_ */
