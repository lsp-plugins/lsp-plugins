/*
 * resampling.h
 *
 *  Created on: 19 нояб. 2016 г.
 *      Author: sadko
 */

#ifndef CORE_X86_SSE_RESAMPLING_H_
#define CORE_X86_SSE_RESAMPLING_H_

namespace lsp
{
    namespace sse
    {
        // Lanczos kernel 2x2: 2 SSE registers
//        const float lanczos_kernel_2x2[] __lsp_aligned16 =
//        {
//            +0.0000000000000000f,
//            -0.1664152316035080f,
//            +0.0000000000000000f,
//            +0.6203830132406946f,
//
//            +1.0000000000000000f,
//            +0.6203830132406946f,
//            +0.0000000000000000f,
//            -0.1664152316035080f
//        };

        const float lanczos_2x2_k0 = 0.6203830132406946f;
        const float lanczos_2x2_k1 = -0.1664152316035080f;

        // Lanczos kernel 2x3: 3 SSE registers
//        const float lanczos_kernel_2x3[] __lsp_aligned16 =
//        {
//            +0.0000000000000000f,
//            +0.0939539981090991f,
//            +0.0000000000000000f,
//            -0.1910530560835854f,
//
//            +0.0000000000000000f,
//            +0.6293724479752082f,
//            +1.0000000000000000f,
//            +0.6293724479752082f,
//
//            +0.0000000000000000f,
//            -0.1910530560835854f,
//            +0.0000000000000000f,
//            +0.0939539981090991f
//        };

        const float lanczos_2x3_k0 = 0.6293724479752082f;
        const float lanczos_2x3_k1 = -0.1910530560835854f;
        const float lanczos_2x3_k2 = 0.0939539981090991f;

        // Lanczos kernel 3x2: 3 SSE registers
//        const float lanczos_kernel_3x2[] __lsp_aligned16 =
//        {
//            +0.0000000000000000f,
//            -0.1451906347823569f,
//            -0.1903584501504231f,
//            +0.0000000000000000f,
//
//            +0.4051504629060886f,
//            +0.8228011237053413f,
//            +1.0000000000000000f,
//            +0.8228011237053413f,
//
//            +0.4051504629060886f,
//            +0.0000000000000000f,
//            -0.1903584501504231f,
//            -0.1451906347823569f,
//        };
        const float lanczos_3x2_m0[] __lsp_aligned16 =
        {
            +0.0000000000000000f,
            -0.1451906347823569f,
            -0.1903584501504231f,
            +0.0000000000000000f
        };

        const float lanczos_3x2_m1[] __lsp_aligned16 =
        {
            +0.4051504629060886f,
            +0.8228011237053413f,
            +1.0000000000000000f,
            +0.8228011237053413f
        };

        const float lanczos_3x2_m2[] __lsp_aligned16 =
        {
            +0.4051504629060886f,
            +0.0000000000000000f,
            -0.1903584501504231f,
            -0.1451906347823569f
        };

        const float lanczos_3x2_m3[] __lsp_aligned16 =
        {
            -0.1451906347823569f,
            -0.1903584501504231f,
            +0.0000000000000000f,
            +0.4051504629060886f
        };

        const float lanczos_3x2_m4[] __lsp_aligned16 =
        {
            +0.8228011237053413f,
            +1.0000000000000000f,
            +0.8228011237053413f,
            +0.4051504629060886f
        };

        const float lanczos_3x2_m5[] __lsp_aligned16 =
        {
            +0.0000000000000000f,
            -0.1903584501504231f,
            -0.1451906347823569f,
            +0.0000000000000000f
        };


        // Lanczos kernel 3x3: 5 SSE registers
        const float lanczos_3x3_m0[] __lsp_aligned16 =
        {
            +0.0000000000000000f,
            +0.0890793429479492f,
            +0.1055060549370832f,
            +0.0000000000000000f
        };

        const float lanczos_3x3_m1[] __lsp_aligned16 =
        {
            -0.1562250559899557f,
            -0.1993645686793863f,
            +0.0000000000000000f,
            +0.4097746665281485f
        };

        const float lanczos_3x3_m2[] __lsp_aligned16 =
        {
            +0.8251285571768335f,
            +1.0000000000000000f,
            +0.8251285571768335f,
            +0.4097746665281485f
        };

        const float lanczos_3x3_m3[] __lsp_aligned16 =
        {
            +0.0000000000000000f,
            -0.1993645686793863f,
            -0.1562250559899557f,
            +0.0000000000000000f
        };

        const float lanczos_3x3_m4[] __lsp_aligned16 =
        {
            +0.1055060549370832f,
            +0.0890793429479492f,
            +0.0000000000000000f,
            +0.0000000000000000f
        };

        const float lanczos_3x3_m5[] __lsp_aligned16 =
        {
            +0.0890793429479492f,
            +0.1055060549370832f,
            +0.0000000000000000f,
            -0.1562250559899557f
        };

        const float lanczos_3x3_m6[] __lsp_aligned16 =
        {
            -0.1993645686793863f,
            +0.0000000000000000f,
            +0.4097746665281485f,
            +0.8251285571768335f
        };

        const float lanczos_3x3_m7[] __lsp_aligned16 =
        {
            +1.0000000000000000f,
            +0.8251285571768335f,
            +0.4097746665281485f,
            +0.0000000000000000f
        };

        const float lanczos_3x3_m8[] __lsp_aligned16 =
        {
            -0.1993645686793863f,
            -0.1562250559899557f,
            +0.0000000000000000f,
            +0.1055060549370832f
        };

        const float lanczos_3x3_m9[] __lsp_aligned16 =
        {
            +0.0890793429479492f,
            +0.0000000000000000f,
            +0.0000000000000000f,
            +0.0000000000000000f
        };

        // Lanczos kernel 4x2: 4 SSE registers
        const float lanczos_kernel_4x2[] __lsp_aligned16 =
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

        // Lanczos kernel 4x3: 6 SSE registers
        const float lanczos_kernel_4x3[] __lsp_aligned16 =
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

        void lanczos_resample_2x2(float *dst, const float *src, size_t count)
        {
            __asm__ __volatile__
            (
                __ASM_EMIT("cmp         $2, %[count]")
                __ASM_EMIT("movss       %[k0], %%xmm6")         // xmm6 = k0
                __ASM_EMIT("movss       %[k1], %%xmm7")         // xmm7 = k1
                __ASM_EMIT("jb          2f")

                // Load samples
                __ASM_EMIT(".align 16")
                __ASM_EMIT("1:")
                __ASM_EMIT("movss       0x00(%[src]), %%xmm0")  // xmm0 = s0
                __ASM_EMIT("movss       0x04(%[src]), %%xmm1")  // xmm1 = s1
                __ASM_EMIT("sub         $2, %[count]")

                // Do convolution
                __ASM_EMIT("movaps      %%xmm0, %%xmm2")        // xmm2 = s0
                __ASM_EMIT("movaps      %%xmm1, %%xmm3")        // xmm3 = s1
                __ASM_EMIT("mulss       %%xmm7, %%xmm2")        // xmm2 = k1*s0
                __ASM_EMIT("mulss       %%xmm7, %%xmm3")        // xmm3 = k1*s1
                __ASM_EMIT("movaps      %%xmm2, %%xmm4")        // xmm4 = k1*s0
                __ASM_EMIT("movaps      %%xmm2, %%xmm5")        // xmm5 = k1*s1
                __ASM_EMIT("addss       0x04(%[dst]), %%xmm4")  // xmm4 = k1*s0 + d1
                __ASM_EMIT("addss       0x24(%[dst]), %%xmm5")  // xmm5 = k1*s1 + d9
                __ASM_EMIT("movss       %%xmm4, 0x04(%[dst])")  // d1 += k1*s0
                __ASM_EMIT("movss       %%xmm5, 0x24(%[dst])")  // d9 += k1*s1

                __ASM_EMIT("movaps      %%xmm0, %%xmm4")        // xmm4 = s0
                __ASM_EMIT("movaps      %%xmm1, %%xmm5")        // xmm5 = s1
                __ASM_EMIT("mulss       %%xmm6, %%xmm4")        // xmm4 = k0*s0
                __ASM_EMIT("mulss       %%xmm6, %%xmm5")        // xmm5 = k0*s1
                __ASM_EMIT("addss       %%xmm4, %%xmm3")        // xmm3 = k1*s1 + k0*s0
                __ASM_EMIT("addss       %%xmm5, %%xmm2")        // xmm2 = k1*s0 + k0*s1
                __ASM_EMIT("addss       0x0c(%[dst]), %%xmm3")  // xmm3 = k1*s1 + k0*s0 + d3
                __ASM_EMIT("addss       0x1c(%[dst]), %%xmm2")  // xmm2 = k1*s0 + k0*s1 + d7
                __ASM_EMIT("movss       %%xmm3, 0x0c(%[dst])")  // d3 += k1*s1 + k0*s0
                __ASM_EMIT("movss       %%xmm2, 0x1c(%[dst])")  // d7 += k1*s0 + k0*s1

                __ASM_EMIT("addss       %%xmm5, %%xmm4")        // xmm4 = k0*s0 + k0*s1
                __ASM_EMIT("addss       0x10(%[dst]), %%xmm0")  // xmm0 = s0 + d4
                __ASM_EMIT("addss       0x18(%[dst]), %%xmm1")  // xmm1 = s1 + d6
                __ASM_EMIT("addss       0x14(%[dst]), %%xmm4")  // xmm4 = k0*s0 + k0*s1 + d5
                __ASM_EMIT("movss       %%xmm0, 0x10(%[dst])")  // d4 += s0
                __ASM_EMIT("movss       %%xmm4, 0x14(%[dst])")  // d5 += k0*s0 + k0*s1
                __ASM_EMIT("movss       %%xmm1, 0x18(%[dst])")  // d6 += s1

                // Update pointers
                __ASM_EMIT("add         $0x08, %[src]")
                __ASM_EMIT("add         $0x10, %[dst]")
                __ASM_EMIT("cmp         $2, %[count]")
                __ASM_EMIT("jae         1b")

                // Complete loop
                __ASM_EMIT("2:")
                __ASM_EMIT("test        %[count], %[count]")
                __ASM_EMIT("jz          3f")

                // Load sample
                __ASM_EMIT("movss       (%[src]), %%xmm0")      // xmm0 = s0

                // Do convolution
                __ASM_EMIT("movaps      %%xmm0, %%xmm1")        // xmm1 = s0
                __ASM_EMIT("movaps      %%xmm0, %%xmm2")        // xmm2 = s0
                __ASM_EMIT("mulss       %%xmm6, %%xmm1")        // xmm1 = k0*s0
                __ASM_EMIT("mulss       %%xmm7, %%xmm2")        // xmm2 = k1*s0
                __ASM_EMIT("movaps      %%xmm1, %%xmm3")        // xmm3 = k0*s0
                __ASM_EMIT("movaps      %%xmm2, %%xmm4")        // xmm4 = k1*s0

                __ASM_EMIT("addss       0x04(%[dst]), %%xmm2")  // xmm2 = k1*s0 + d1
                __ASM_EMIT("addss       0x0c(%[dst]), %%xmm1")  // xmm1 = k0*s0 + d3
                __ASM_EMIT("addss       0x10(%[dst]), %%xmm0")  // xmm0 = s0 + d4
                __ASM_EMIT("addss       0x14(%[dst]), %%xmm3")  // xmm3 = k0*s0 + d5
                __ASM_EMIT("addss       0x1c(%[dst]), %%xmm4")  // xmm4 = k1*s0 + d7

                __ASM_EMIT("movss       %%xmm2, 0x04(%[dst])")  // d1 += k1*s0
                __ASM_EMIT("movss       %%xmm1, 0x0c(%[dst])")  // d3 += k0*s0
                __ASM_EMIT("movss       %%xmm0, 0x10(%[dst])")  // d4 += s0
                __ASM_EMIT("movss       %%xmm3, 0x14(%[dst])")  // d5 += k0*s0
                __ASM_EMIT("movss       %%xmm4, 0x1c(%[dst])")  // d7 += k1*s0

                __ASM_EMIT("3:")

                : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
                : [k0] "m" (lanczos_2x2_k0),
                  [k1] "m" (lanczos_2x2_k1)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }
/*
        void lanczos_resample_2x2(float *dst, const float *src, size_t count)
        {
            __asm__ __volatile__
            (
                __ASM_EMIT("cmp         $4, %[count]")
                __ASM_EMIT("jb          200f")

                __ASM_EMIT("test        $0x7, %[dst]")
                __ASM_EMIT("jnz         3f")                        // dst unaligned
                __ASM_EMIT("test        $0x8, %[dst]")
                __ASM_EMIT("jnz         2f")                        // dst semi-aligned

                // dst is aligned
                __ASM_EMIT("1:")

                // Load samples and destination buffer
                __ASM_EMIT("movups      0x00(%[src]), %%xmm0")      // xmm0 = s0  s1  s2  s3
                __ASM_EMIT("movaps      %%xmm0, %%xmm1")            // xmm1 = s0  s1  s2  s3
                __ASM_EMIT("movaps      %%xmm0, %%xmm2")            // xmm2 = s0  s1  s2  s3
                __ASM_EMIT("movaps      %%xmm1, %%xmm3")            // xmm3 = s0  s1  s2  s3

                __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0")     // xmm0 = s0  s0  s0  s0
                __ASM_EMIT("shufps      $0x55, %%xmm1, %%xmm1")     // xmm1 = s1  s1  s1  s1
                __ASM_EMIT("shufps      $0xaa, %%xmm2, %%xmm2")     // xmm2 = s2  s2  s2  s2
                __ASM_EMIT("shufps      $0xff, %%xmm3, %%xmm3")     // xmm3 = s3  s3  s3  s3

                __ASM_EMIT("movaps      %%xmm0, %%xmm4")            // xmm4 = s0  s0  s0  s0
                __ASM_EMIT("movaps      %%xmm1, %%xmm5")            // xmm5 = s1  s1  s1  s1
                __ASM_EMIT("movaps      %%xmm2, %%xmm6")            // xmm6 = s2  s2  s2  s2
                __ASM_EMIT("movaps      %%xmm3, %%xmm7")            // xmm7 = s3  s3  s3  s3

                // Perform convolution
                __ASM_EMIT("mulps       0x00(%[k]), %%xmm0")        // xmm0 = s0*k[0..3]
                __ASM_EMIT("mulps       0x00(%[k]), %%xmm1")        // xmm1 = s1*k[0..3]
                __ASM_EMIT("mulps       0x00(%[k]), %%xmm2")        // xmm2 = s2*k[0..3]
                __ASM_EMIT("mulps       0x00(%[k]), %%xmm3")        // xmm3 = s3*k[0..3]
                __ASM_EMIT("mulps       0x10(%[k]), %%xmm4")        // xmm4 = s0*k[4..7]
                __ASM_EMIT("mulps       0x10(%[k]), %%xmm5")        // xmm5 = s1*k[4..7]
                __ASM_EMIT("mulps       0x10(%[k]), %%xmm6")        // xmm6 = s2*k[4..7]
                __ASM_EMIT("mulps       0x10(%[k]), %%xmm7")        // xmm7 = s3*k[4..7]
                __ASM_EMIT("addps       %%xmm4, %%xmm2")            // xmm2 = s0*k[4..7] + s2*k[0..3]
                __ASM_EMIT("addps       %%xmm5, %%xmm3")            // xmm3 = s1*k[4..7] + s3*k[0..3]

                // Store convolution
                __ASM_EMIT("addps       0x00(%[dst]), %%xmm0")      // xmm0 = dst[0..3] + s0*k[0..3]
                __ASM_EMIT("addps       0x10(%[dst]), %%xmm2")      // xmm2 = dst[4..7] + s0*k[4..7] + s2*k[0..3]
                __ASM_EMIT("addps       0x20(%[dst]), %%xmm6")      // xmm6 = dst[8..11] + s2*k[4..7]
                __ASM_EMIT("movaps      %%xmm0, 0x00(%[dst])")      // dst[0..3] = xmm0
                __ASM_EMIT("movaps      %%xmm2, 0x10(%[dst])")      // dst[4..7] = xmm2
                __ASM_EMIT("movaps      %%xmm6, 0x20(%[dst])")      // dst[8..11] = xmm6
                __ASM_EMIT("movups      0x08(%[dst]), %%xmm0")      // xmm0 = dst[2..5]
                __ASM_EMIT("movups      0x18(%[dst]), %%xmm2")      // xmm2 = dst[6..9]
                __ASM_EMIT("movups      0x28(%[dst]), %%xmm6")      // xmm6 = dst[10..13]
                __ASM_EMIT("addps       %%xmm1, %%xmm0")            // xmm0 = dst[2..5] + s1*k[0..3]
                __ASM_EMIT("addps       %%xmm3, %%xmm2")            // xmm2 = dst[6..9] + s1*k[4..7] + s3*k[0..3]
                __ASM_EMIT("addps       %%xmm7, %%xmm6")            // xmm6 = dst[10..13] + s3*k[4..7]
                __ASM_EMIT("movups      %%xmm0, 0x08(%[dst])")      // dst[2..5] = xmm0
                __ASM_EMIT("movups      %%xmm2, 0x18(%[dst])")      // dst[6..9] = xmm2
                __ASM_EMIT("movups      %%xmm6, 0x28(%[dst])")      // dst[10..13] = xmm6

                // Add pointer and repeat loop
                __ASM_EMIT("sub         $4, %[count]")
                __ASM_EMIT("add         $0x20, %[dst]")
                __ASM_EMIT("add         $0x10, %[src]")
                __ASM_EMIT("cmp         $4, %[count]")
                __ASM_EMIT("jae         1b")
                __ASM_EMIT("jmp         200f")

                // dst is semi-aligned
                __ASM_EMIT("2:")

                // Load samples and destination buffer
                __ASM_EMIT("movups      0x00(%[src]), %%xmm0")      // xmm0 = s0  s1  s2  s3
                __ASM_EMIT("movaps      %%xmm0, %%xmm1")            // xmm1 = s0  s1  s2  s3
                __ASM_EMIT("movaps      %%xmm0, %%xmm2")            // xmm2 = s0  s1  s2  s3
                __ASM_EMIT("movaps      %%xmm1, %%xmm3")            // xmm3 = s0  s1  s2  s3

                __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0")     // xmm0 = s0  s0  s0  s0
                __ASM_EMIT("shufps      $0x55, %%xmm1, %%xmm1")     // xmm1 = s1  s1  s1  s1
                __ASM_EMIT("shufps      $0xaa, %%xmm2, %%xmm2")     // xmm2 = s2  s2  s2  s2
                __ASM_EMIT("shufps      $0xff, %%xmm3, %%xmm3")     // xmm3 = s3  s3  s3  s3

                __ASM_EMIT("movaps      %%xmm0, %%xmm4")            // xmm4 = s0  s0  s0  s0
                __ASM_EMIT("movaps      %%xmm1, %%xmm5")            // xmm5 = s1  s1  s1  s1
                __ASM_EMIT("movaps      %%xmm2, %%xmm6")            // xmm6 = s2  s2  s2  s2
                __ASM_EMIT("movaps      %%xmm3, %%xmm7")            // xmm7 = s3  s3  s3  s3

                // Perform convolution
                __ASM_EMIT("mulps       0x00(%[k]), %%xmm0")        // xmm0 = s0*k[0..3]
                __ASM_EMIT("mulps       0x00(%[k]), %%xmm1")        // xmm1 = s1*k[0..3]
                __ASM_EMIT("mulps       0x00(%[k]), %%xmm2")        // xmm2 = s2*k[0..3]
                __ASM_EMIT("mulps       0x00(%[k]), %%xmm3")        // xmm3 = s3*k[0..3]
                __ASM_EMIT("mulps       0x10(%[k]), %%xmm4")        // xmm4 = s0*k[4..7]
                __ASM_EMIT("mulps       0x10(%[k]), %%xmm5")        // xmm5 = s1*k[4..7]
                __ASM_EMIT("mulps       0x10(%[k]), %%xmm6")        // xmm6 = s2*k[4..7]
                __ASM_EMIT("mulps       0x10(%[k]), %%xmm7")        // xmm7 = s3*k[4..7]
                __ASM_EMIT("addps       %%xmm4, %%xmm2")            // xmm2 = s0*k[4..7] + s2*k[0..3]
                __ASM_EMIT("addps       %%xmm5, %%xmm3")            // xmm3 = s1*k[4..7] + s3*k[0..3]

                // Store convolution
                __ASM_EMIT("addps       0x08(%[dst]), %%xmm1")      // xmm1 = dst[2..5] + s1*k[0..3]
                __ASM_EMIT("addps       0x18(%[dst]), %%xmm3")      // xmm3 = dst[6..9] + s1*k[4..7] + s3*k[0..3]
                __ASM_EMIT("addps       0x28(%[dst]), %%xmm7")      // xmm7 = dst[10..13] + s3*k[4..7]
                __ASM_EMIT("movaps      %%xmm1, 0x08(%[dst])")      // dst[2..5] = xmm1
                __ASM_EMIT("movaps      %%xmm3, 0x18(%[dst])")      // dst[6..9] = xmm3
                __ASM_EMIT("movaps      %%xmm7, 0x28(%[dst])")      // dst[10..13] = xmm7
                __ASM_EMIT("movups      0x00(%[dst]), %%xmm1")      // xmm1 = dst[0..3]
                __ASM_EMIT("movups      0x10(%[dst]), %%xmm3")      // xmm3 = dst[4..7]
                __ASM_EMIT("movups      0x20(%[dst]), %%xmm7")      // xmm7 = dst[8..11]
                __ASM_EMIT("addps       %%xmm0, %%xmm1")            // xmm1 = dst[0..3] + s0*k[0..3]
                __ASM_EMIT("addps       %%xmm2, %%xmm3")            // xmm3 = dst[4..7] + s0*k[4..7] + s2*k[0..3]
                __ASM_EMIT("addps       %%xmm6, %%xmm7")            // xmm7 = dst[8..11] + s3*k[4..7]
                __ASM_EMIT("movups      %%xmm1, 0x00(%[dst])")      // dst[0..3] = xmm1
                __ASM_EMIT("movups      %%xmm3, 0x10(%[dst])")      // dst[4..7] = xmm3
                __ASM_EMIT("movups      %%xmm7, 0x20(%[dst])")      // dst[8..11] = xmm7

                // Add pointer and repeat loop
                __ASM_EMIT("sub         $4, %[count]")
                __ASM_EMIT("add         $0x20, %[dst]")
                __ASM_EMIT("add         $0x10, %[src]")
                __ASM_EMIT("cmp         $4, %[count]")
                __ASM_EMIT("jae         2b")
                __ASM_EMIT("jmp         200f")

                // dst is unaligned
                __ASM_EMIT("3:")
                __ASM_EMIT("movups      0x00(%[src]), %%xmm0")      // xmm0 = s0  s1  s2  s3
                __ASM_EMIT("movaps      %%xmm0, %%xmm1")            // xmm1 = s0  s1  s2  s3
                __ASM_EMIT("movaps      %%xmm0, %%xmm2")            // xmm2 = s0  s1  s2  s3
                __ASM_EMIT("movaps      %%xmm1, %%xmm3")            // xmm3 = s0  s1  s2  s3
                __ASM_EMIT("movaps      %%xmm2, %%xmm4")            // xmm4 = s0  s1  s2  s3

                __ASM_EMIT("shufps      $0x00, %%xmm1, %%xmm1")     // xmm1 = s0  s0  s0  s0
                __ASM_EMIT("shufps      $0x00, %%xmm2, %%xmm2")     // xmm2 = s0  s0  s0  s0
                __ASM_EMIT("shufps      $0xaa, %%xmm3, %%xmm3")     // xmm3 = s2  s2  s2  s2
                __ASM_EMIT("shufps      $0xaa, %%xmm4, %%xmm4")     // xmm4 = s2  s2  s2  s2

                // Convolution, turn 1
                __ASM_EMIT("mulps       0x00(%[k]), %%xmm1")        // xmm1 = s0*k[0..3]
                __ASM_EMIT("mulps       0x00(%[k]), %%xmm3")        // xmm3 = s2*k[0..3]
                __ASM_EMIT("mulps       0x10(%[k]), %%xmm2")        // xmm2 = s0*k[4..7]
                __ASM_EMIT("mulps       0x10(%[k]), %%xmm4")        // xmm4 = s2*k[4..7]
                __ASM_EMIT("addps       %%xmm3, %%xmm2")            // xmm2 = s2*k[4..7] + s2*k[0..3]

                __ASM_EMIT("movups      0x00(%[dst]), %%xmm5")      // xmm5 = dst[0..3]
                __ASM_EMIT("movups      0x10(%[dst]), %%xmm6")      // xmm6 = dst[4..7]
                __ASM_EMIT("movups      0x20(%[dst]), %%xmm7")      // xmm7 = dst[8..11]
                __ASM_EMIT("addps       %%xmm1, %%xmm5")            // xmm5 = dst[0..3] + s0*k[0..3]
                __ASM_EMIT("addps       %%xmm2, %%xmm6")            // xmm6 = dst[4..7] + s2*k[4..7] + s2*k[0..3]
                __ASM_EMIT("addps       %%xmm4, %%xmm7")            // xmm7 = dst[8..11] + s2*k[4..7]
                __ASM_EMIT("movups      %%xmm5, 0x00(%[dst])")      // dst[0..3] = xmm5
                __ASM_EMIT("movups      %%xmm6, 0x10(%[dst])")      // dst[4..7] = xmm6
                __ASM_EMIT("movups      %%xmm7, 0x20(%[dst])")      // dst[8..11] = xmm7

                // Convolution, turn 2
                __ASM_EMIT("movaps      %%xmm0, %%xmm1")            // xmm1 = s0  s1  s2  s3
                __ASM_EMIT("movaps      %%xmm0, %%xmm2")            // xmm2 = s0  s1  s2  s3
                __ASM_EMIT("movaps      %%xmm1, %%xmm3")            // xmm3 = s0  s1  s2  s3

                __ASM_EMIT("shufps      $0x55, %%xmm0, %%xmm0")     // xmm0 = s0  s0  s0  s0
                __ASM_EMIT("shufps      $0x55, %%xmm1, %%xmm1")     // xmm1 = s0  s0  s0  s0
                __ASM_EMIT("shufps      $0xff, %%xmm2, %%xmm2")     // xmm2 = s2  s2  s2  s2
                __ASM_EMIT("shufps      $0xff, %%xmm3, %%xmm3")     // xmm3 = s2  s2  s2  s2

                __ASM_EMIT("mulps       0x00(%[k]), %%xmm0")        // xmm0 = s1*k[0..3]
                __ASM_EMIT("mulps       0x00(%[k]), %%xmm2")        // xmm2 = s3*k[0..3]
                __ASM_EMIT("mulps       0x10(%[k]), %%xmm1")        // xmm1 = s1*k[4..7]
                __ASM_EMIT("mulps       0x10(%[k]), %%xmm3")        // xmm3 = s3*k[4..7]
                __ASM_EMIT("addps       %%xmm2, %%xmm1")            // xmm1 = s1*k[4..7] + s3*k[0..3]

                __ASM_EMIT("movups      0x08(%[dst]), %%xmm4")      // xmm4 = dst[2..5]
                __ASM_EMIT("movups      0x18(%[dst]), %%xmm5")      // xmm5 = dst[6..9]
                __ASM_EMIT("movups      0x28(%[dst]), %%xmm6")      // xmm6 = dst[10..13]
                __ASM_EMIT("addps       %%xmm0, %%xmm4")            // xmm4 = dst[2..5] + s1*k[0..3]
                __ASM_EMIT("addps       %%xmm1, %%xmm5")            // xmm5 = dst[6..9] + s1*k[4..7] + s3*k[0..3]
                __ASM_EMIT("addps       %%xmm3, %%xmm6")            // xmm6 = dst[10..13] + s3*k[4..7]
                __ASM_EMIT("movups      %%xmm4, 0x08(%[dst])")      // dst[2..5] = xmm4
                __ASM_EMIT("movups      %%xmm5, 0x18(%[dst])")      // dst[6..9] = xmm5
                __ASM_EMIT("movups      %%xmm6, 0x28(%[dst])")      // dst[10..13] = xmm6

                // Add pointer and repeat loop
                __ASM_EMIT("sub         $4, %[count]")
                __ASM_EMIT("add         $0x20, %[dst]")
                __ASM_EMIT("add         $0x10, %[src]")
                __ASM_EMIT("cmp         $4, %[count]")
                __ASM_EMIT("jae         3b")

                // Calculate tail
                __ASM_EMIT("200:")
                __ASM_EMIT("test        %[count], %[count]")
                __ASM_EMIT("jz          300f")

                // Repeat loop
                __ASM_EMIT("4:")

                // Load samples and destination buffer
                __ASM_EMIT("movss       0x00(%[src]), %%xmm0")      // xmm0 = 0 0 0 s0
                __ASM_EMIT("movups      0x00(%[dst]), %%xmm2")      // xmm2 = dst[0..3]
                __ASM_EMIT("movups      0x10(%[dst]), %%xmm3")      // xmm3 = dst[4..7]
                __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0")     // xmm0 = s0 s0 s0 s0
                __ASM_EMIT("movaps      %%xmm0, %%xmm1")            // xmm1 = s0 s0 s0 s0

                // Perform convolution
                __ASM_EMIT("mulps       0x00(%[k]), %%xmm0")        // xmm0 = s0*k[0..3]
                __ASM_EMIT("mulps       0x10(%[k]), %%xmm1")        // xmm1 = s0*k[4..7]
                __ASM_EMIT("addps       %%xmm0, %%xmm2")            // xmm2 = s0*k[0..3] + dst[0..3]
                __ASM_EMIT("addps       %%xmm1, %%xmm3")            // xmm3 = s0*k[4..7] + dst[4..7]
                __ASM_EMIT("movups      %%xmm2, 0x00(%[dst])")      // dst[0..3] = xmm2
                __ASM_EMIT("movups      %%xmm3, 0x10(%[dst])")      // dst[4..7] = xmm3

                __ASM_EMIT("add         $0x08, %[dst]")
                __ASM_EMIT("add         $0x04, %[src]")
                __ASM_EMIT("dec         %[count]")
                __ASM_EMIT("jnz         4b")

                // End of processing
                __ASM_EMIT("300:")

                : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
                : [k] "r" (lanczos_kernel_2x2)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }
*/
        void lanczos_resample_2x3(float *dst, const float *src, size_t count)
        {
            __asm__ __volatile__
            (
                __ASM_EMIT("cmp         $2, %[count]")
                __ASM_EMIT("movss       %[k0], %%xmm6")         // xmm6 = k0
                __ASM_EMIT("movss       %[k1], %%xmm7")         // xmm7 = k1
                __ASM_EMIT("jb          2f")

                // Load samples
                __ASM_EMIT(".align 16")
                __ASM_EMIT("1:")
                __ASM_EMIT("movss       0x00(%[src]), %%xmm0")  // xmm0 = s0
                __ASM_EMIT("movss       0x04(%[src]), %%xmm1")  // xmm1 = s1
                __ASM_EMIT("sub         $2, %[count]")

                // Do convolution
                __ASM_EMIT("movaps      %%xmm0, %%xmm2")        // xmm2 = s0
                __ASM_EMIT("movaps      %%xmm1, %%xmm3")        // xmm3 = s1
                __ASM_EMIT("mulss       %[k2], %%xmm2")         // xmm2 = k2*s0
                __ASM_EMIT("mulss       %[k2], %%xmm3")         // xmm3 = k2*s1
                __ASM_EMIT("movaps      %%xmm2, %%xmm4")        // xmm4 = k2*s0
                __ASM_EMIT("movaps      %%xmm2, %%xmm5")        // xmm5 = k2*s1
                __ASM_EMIT("addss       0x04(%[dst]), %%xmm4")  // xmm4 = k2*s0 + d1
                __ASM_EMIT("addss       0x34(%[dst]), %%xmm5")  // xmm5 = k2*s1 + d13
                __ASM_EMIT("movss       %%xmm4, 0x04(%[dst])")  // d1  += k2*s0
                __ASM_EMIT("movss       %%xmm5, 0x34(%[dst])")  // d13 += k2*s1

                __ASM_EMIT("movaps      %%xmm0, %%xmm4")        // xmm4 = s0
                __ASM_EMIT("movaps      %%xmm1, %%xmm5")        // xmm5 = s1
                __ASM_EMIT("mulss       %%xmm7, %%xmm4")        // xmm4 = k1*s0
                __ASM_EMIT("mulss       %%xmm7, %%xmm5")        // xmm5 = k1*s1
                __ASM_EMIT("addss       %%xmm4, %%xmm3")        // xmm3 = k2*s1 + k1*s0
                __ASM_EMIT("addss       %%xmm5, %%xmm2")        // xmm2 = k2*s0 + k1*s1
                __ASM_EMIT("addss       0x0c(%[dst]), %%xmm3")  // xmm3 = k2*s1 + k1*s0 + d3
                __ASM_EMIT("addss       0x2c(%[dst]), %%xmm2")  // xmm2 = k2*s0 + k1*s1 + d11
                __ASM_EMIT("movss       %%xmm3, 0x0c(%[dst])")  // d3  += k2*s1 + k1*s0
                __ASM_EMIT("movss       %%xmm2, 0x2c(%[dst])")  // d11 += k2*s0 + k1*s1

                __ASM_EMIT("movaps      %%xmm0, %%xmm2")        // xmm2 = s0
                __ASM_EMIT("movaps      %%xmm1, %%xmm3")        // xmm3 = s1
                __ASM_EMIT("mulss       %%xmm6, %%xmm2")        // xmm2 = k0*s0
                __ASM_EMIT("mulss       %%xmm6, %%xmm3")        // xmm3 = k0*s1
                __ASM_EMIT("addss       %%xmm2, %%xmm5")        // xmm5 = k1*s1 + k0*s0
                __ASM_EMIT("addss       %%xmm3, %%xmm4")        // xmm4 = k1*s0 + k0*s1
                __ASM_EMIT("addss       0x14(%[dst]), %%xmm5")  // xmm5 = k1*s1 + k0*s0 + d5
                __ASM_EMIT("addss       0x24(%[dst]), %%xmm4")  // xmm4 = k1*s0 + k0*s1 + d9
                __ASM_EMIT("movss       %%xmm5, 0x14(%[dst])")  // d5  += k1*s1 + k0*s0
                __ASM_EMIT("movss       %%xmm4, 0x24(%[dst])")  // d9  += k1*s0 + k0*s1

                __ASM_EMIT("addss       %%xmm3, %%xmm2")        // xmm2 = k0*s0 + k0*s1
                __ASM_EMIT("addss       0x18(%[dst]), %%xmm0")  // xmm0 = s0 + d6
                __ASM_EMIT("addss       0x20(%[dst]), %%xmm1")  // xmm1 = s1 + d8
                __ASM_EMIT("addss       0x1c(%[dst]), %%xmm2")  // xmm2 = k0*s0 + k0*s1 + d7
                __ASM_EMIT("movss       %%xmm0, 0x18(%[dst])")  // d6  += s0
                __ASM_EMIT("movss       %%xmm2, 0x1c(%[dst])")  // d7  += k0*s0 + k0*s1
                __ASM_EMIT("movss       %%xmm1, 0x20(%[dst])")  // d8  += s1

                // Update pointers
                __ASM_EMIT("add         $0x08, %[src]")
                __ASM_EMIT("add         $0x10, %[dst]")
                __ASM_EMIT("cmp         $2, %[count]")
                __ASM_EMIT("jae         1b")

                // Complete loop
                __ASM_EMIT("2:")
                __ASM_EMIT("test        %[count], %[count]")
                __ASM_EMIT("jz          3f")

                // Load sample
                __ASM_EMIT("movss       (%[src]), %%xmm0")      // xmm0 = s0

                // Do convolution
                __ASM_EMIT("movaps      %%xmm0, %%xmm1")        // xmm1 = s0
                __ASM_EMIT("movaps      %%xmm0, %%xmm2")        // xmm2 = s0
                __ASM_EMIT("movaps      %%xmm0, %%xmm3")        // xmm3 = s0
                __ASM_EMIT("mulss       %%xmm6, %%xmm1")        // xmm1 = k0*s0
                __ASM_EMIT("mulss       %%xmm7, %%xmm2")        // xmm2 = k1*s0
                __ASM_EMIT("mulss       %[k2], %%xmm3")         // xmm3 = k2*s0
                __ASM_EMIT("movaps      %%xmm1, %%xmm4")        // xmm4 = k0*s0
                __ASM_EMIT("movaps      %%xmm2, %%xmm5")        // xmm5 = k1*s0
                __ASM_EMIT("movaps      %%xmm3, %%xmm6")        // xmm6 = k2*s0

                __ASM_EMIT("addss       0x04(%[dst]), %%xmm3")  // xmm3 = k2*s0 + d1
                __ASM_EMIT("addss       0x0c(%[dst]), %%xmm2")  // xmm2 = k1*s0 + d3
                __ASM_EMIT("addss       0x14(%[dst]), %%xmm1")  // xmm1 = k0*s0 + d5
                __ASM_EMIT("addss       0x18(%[dst]), %%xmm0")  // xmm0 = s0 + d6
                __ASM_EMIT("addss       0x1c(%[dst]), %%xmm4")  // xmm4 = k0*s0 + d7
                __ASM_EMIT("addss       0x24(%[dst]), %%xmm5")  // xmm5 = k1*s0 + d9
                __ASM_EMIT("addss       0x2c(%[dst]), %%xmm6")  // xmm6 = k2*s0 + d11

                __ASM_EMIT("movss       %%xmm3, 0x04(%[dst])")  // d1 += k2*s0
                __ASM_EMIT("movss       %%xmm2, 0x0c(%[dst])")  // d3 += k1*s0
                __ASM_EMIT("movss       %%xmm1, 0x14(%[dst])")  // d5 += k0*s0
                __ASM_EMIT("movss       %%xmm0, 0x18(%[dst])")  // d6 += s0
                __ASM_EMIT("movss       %%xmm4, 0x1c(%[dst])")  // d1 += k0*s0
                __ASM_EMIT("movss       %%xmm5, 0x24(%[dst])")  // d3 += k1*s0
                __ASM_EMIT("movss       %%xmm6, 0x2c(%[dst])")  // d5 += k2*s0

                __ASM_EMIT("3:")

                : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
                : [k0] "m" (lanczos_2x3_k0),
                  [k1] "m" (lanczos_2x3_k1),
                  [k2] "m" (lanczos_2x3_k2)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        /*void lanczos_resample_2x3(float *dst, const float *src, size_t count)
        {
            __asm__ __volatile__
            (
                __ASM_EMIT("cmp         $4, %[count]")
                __ASM_EMIT("jb          200f")

                __ASM_EMIT("test        $0x7, %[dst]")
                __ASM_EMIT("jnz         3f")                        // dst unaligned
                __ASM_EMIT("test        $0x8, %[dst]")
                __ASM_EMIT("jnz         2f")                        // dst semi-aligned

                // dst is aligned
                __ASM_EMIT(".align 16")
                __ASM_EMIT("1:")

                // Prepare for even convolution
                __ASM_EMIT("movups      0x00(%[src]), %%xmm0")      // xmm0 = s0  s1  s2  s3
                __ASM_EMIT("movaps      %%xmm0, %%xmm3")            // xmm3 = s0  s1  s2  s3
                __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0")     // xmm0 = s0  s0  s0  s0
                __ASM_EMIT("shufps      $0xaa, %%xmm3, %%xmm3")     // xmm3 = s2  s2  s2  s2
                __ASM_EMIT("movaps      %%xmm0, %%xmm1")            // xmm1 = s0  s0  s0  s0
                __ASM_EMIT("movaps      %%xmm3, %%xmm4")            // xmm4 = s2  s2  s2  s2
                __ASM_EMIT("movaps      %%xmm0, %%xmm2")            // xmm2 = s0  s0  s0  s0
                __ASM_EMIT("movaps      %%xmm3, %%xmm5")            // xmm5 = s2  s2  s2  s2

                // Perform even convolution
                __ASM_EMIT("mulps       0x00(%[k]), %%xmm0")        // xmm0 = s0*k[0..3]
                __ASM_EMIT("mulps       0x00(%[k]), %%xmm3")        // xmm3 = s2*k[0..3]
                __ASM_EMIT("mulps       0x10(%[k]), %%xmm1")        // xmm1 = s0*k[4..7]
                __ASM_EMIT("mulps       0x10(%[k]), %%xmm4")        // xmm4 = s2*k[4..7]
                __ASM_EMIT("mulps       0x20(%[k]), %%xmm2")        // xmm2 = s0*k[8..11]
                __ASM_EMIT("mulps       0x20(%[k]), %%xmm5")        // xmm5 = s2*k[8..11]
                __ASM_EMIT("addps       %%xmm3, %%xmm1")            // xmm1 = s0*k[4..7] + s2*k[0..3]
                __ASM_EMIT("addps       %%xmm4, %%xmm2")            // xmm2 = s0*k[8..11] + s2*k[4..7]

                // Store even convolution
                __ASM_EMIT("addps       0x00(%[dst]), %%xmm0")      // xmm0 = dst[0..3] + s0*k[0..3]
                __ASM_EMIT("addps       0x10(%[dst]), %%xmm1")      // xmm1 = dst[4..7] + s2*k[0..3] + s0*k[4..7]
                __ASM_EMIT("addps       0x20(%[dst]), %%xmm2")      // xmm2 = dst[8..11] + s0*k[8..11] + s2*k[4..7]
                __ASM_EMIT("addps       0x30(%[dst]), %%xmm5")      // xmm5 = dst[12..15] + s2*k[8..11]
                __ASM_EMIT("movaps      %%xmm0, 0x00(%[dst])")      // dst[0..3] = xmm2
                __ASM_EMIT("movaps      %%xmm1, 0x10(%[dst])")      // dst[4..7] = xmm3
                __ASM_EMIT("movaps      %%xmm2, 0x20(%[dst])")      // dst[8..11] = xmm4
                __ASM_EMIT("movaps      %%xmm5, 0x30(%[dst])")      // dst[8..11] = xmm7

                // Prepare for odd convolution
                __ASM_EMIT("movups      0x00(%[src]), %%xmm0")      // xmm0 = s0  s1  s2  s3
                __ASM_EMIT("movaps      %%xmm0, %%xmm3")            // xmm3 = s0  s1  s2  s3
                __ASM_EMIT("shufps      $0x55, %%xmm0, %%xmm0")     // xmm0 = s1  s1  s1  s1
                __ASM_EMIT("shufps      $0xff, %%xmm3, %%xmm3")     // xmm3 = s3  s3  s3  s3
                __ASM_EMIT("movaps      %%xmm0, %%xmm1")            // xmm1 = s1  s1  s1  s1
                __ASM_EMIT("movaps      %%xmm3, %%xmm4")            // xmm4 = s3  s3  s3  s3
                __ASM_EMIT("movaps      %%xmm0, %%xmm2")            // xmm2 = s1  s1  s1  s1
                __ASM_EMIT("movaps      %%xmm3, %%xmm5")            // xmm5 = s3  s3  s3  s3

                // Perform odd convolution
                __ASM_EMIT("mulps       0x00(%[k]), %%xmm0")        // xmm0 = s1*k[0..3]
                __ASM_EMIT("mulps       0x00(%[k]), %%xmm3")        // xmm3 = s3*k[0..3]
                __ASM_EMIT("mulps       0x10(%[k]), %%xmm1")        // xmm1 = s1*k[4..7]
                __ASM_EMIT("mulps       0x10(%[k]), %%xmm4")        // xmm4 = s3*k[4..7]
                __ASM_EMIT("mulps       0x20(%[k]), %%xmm2")        // xmm2 = s1*k[8..11]
                __ASM_EMIT("mulps       0x20(%[k]), %%xmm5")        // xmm5 = s3*k[8..11]
                __ASM_EMIT("addps       %%xmm3, %%xmm1")            // xmm1 = s1*k[4..7] + s3*k[0..3]
                __ASM_EMIT("addps       %%xmm4, %%xmm2")            // xmm2 = s1*k[8..11] + s3*k[4..7]

                // Store odd convolution
                __ASM_EMIT("movups      0x08(%[dst]), %%xmm3")      // xmm3 = dst[2..5]
                __ASM_EMIT("movups      0x18(%[dst]), %%xmm4")      // xmm4 = dst[6..9]
                __ASM_EMIT("movups      0x28(%[dst]), %%xmm6")      // xmm6 = dst[10..13]
                __ASM_EMIT("movups      0x38(%[dst]), %%xmm7")      // xmm7 = dst[14..17]
                __ASM_EMIT("addps       %%xmm0, %%xmm3")            // xmm3 = dst[2..5] + s1*k[0..3]
                __ASM_EMIT("addps       %%xmm1, %%xmm4")            // xmm4 = dst[6..9] + s1*k[4..7] + s3*k[0..3]
                __ASM_EMIT("addps       %%xmm2, %%xmm6")            // xmm6 = dst[10..13] + s1*k[8..11] + s3*k[4..7]
                __ASM_EMIT("addps       %%xmm5, %%xmm7")            // xmm7 = dst[14..17] + s3*k[8..11]
                __ASM_EMIT("movups      %%xmm3, 0x08(%[dst])")      // dst[2..5] = xmm3
                __ASM_EMIT("movups      %%xmm4, 0x18(%[dst])")      // dst[6..9] = xmm4
                __ASM_EMIT("movups      %%xmm6, 0x28(%[dst])")      // dst[10..13] = xmm6
                __ASM_EMIT("movups      %%xmm7, 0x38(%[dst])")      // dst[14..17] = xmm7

                // Add pointer and repeat loop
                __ASM_EMIT("sub         $4, %[count]")
                __ASM_EMIT("add         $0x20, %[dst]")
                __ASM_EMIT("add         $0x10, %[src]")
                __ASM_EMIT("cmp         $4, %[count]")
                __ASM_EMIT("jae         1b")
                __ASM_EMIT("jmp         200f")

                // dst is semi-aligned
                __ASM_EMIT(".align 16")
                __ASM_EMIT("2:")

                // Prepare for odd convolution
                __ASM_EMIT("movups      0x00(%[src]), %%xmm0")      // xmm0 = s0  s1  s2  s3
                __ASM_EMIT("movaps      %%xmm0, %%xmm3")            // xmm3 = s0  s1  s2  s3
                __ASM_EMIT("shufps      $0x55, %%xmm0, %%xmm0")     // xmm0 = s1  s1  s1  s1
                __ASM_EMIT("shufps      $0xff, %%xmm3, %%xmm3")     // xmm3 = s3  s3  s3  s3
                __ASM_EMIT("movaps      %%xmm0, %%xmm1")            // xmm1 = s1  s1  s1  s1
                __ASM_EMIT("movaps      %%xmm3, %%xmm4")            // xmm4 = s3  s3  s3  s3
                __ASM_EMIT("movaps      %%xmm0, %%xmm2")            // xmm2 = s1  s1  s1  s1
                __ASM_EMIT("movaps      %%xmm3, %%xmm5")            // xmm5 = s3  s3  s3  s3

                // Perform odd convolution
                __ASM_EMIT("mulps       0x00(%[k]), %%xmm0")        // xmm0 = s1*k[0..3]
                __ASM_EMIT("mulps       0x00(%[k]), %%xmm3")        // xmm3 = s3*k[0..3]
                __ASM_EMIT("mulps       0x10(%[k]), %%xmm1")        // xmm1 = s1*k[4..7]
                __ASM_EMIT("mulps       0x10(%[k]), %%xmm4")        // xmm4 = s3*k[4..7]
                __ASM_EMIT("mulps       0x20(%[k]), %%xmm2")        // xmm2 = s1*k[8..11]
                __ASM_EMIT("mulps       0x20(%[k]), %%xmm5")        // xmm5 = s3*k[8..11]
                __ASM_EMIT("addps       %%xmm3, %%xmm1")            // xmm1 = s1*k[4..7] + s3*k[0..3]
                __ASM_EMIT("addps       %%xmm4, %%xmm2")            // xmm2 = s1*k[8..11] + s3*k[4..7]

                // Store odd convolution
                __ASM_EMIT("addps       0x08(%[dst]), %%xmm0")      // xmm0 = dst[0..3] + s1*k[0..3]
                __ASM_EMIT("addps       0x18(%[dst]), %%xmm1")      // xmm1 = dst[4..7] + s3*k[0..3] + s1*k[4..7]
                __ASM_EMIT("addps       0x28(%[dst]), %%xmm2")      // xmm2 = dst[8..11] + s1*k[8..11] + s3*k[4..7]
                __ASM_EMIT("addps       0x38(%[dst]), %%xmm5")      // xmm5 = dst[12..15] + s3*k[8..11]
                __ASM_EMIT("movaps      %%xmm0, 0x08(%[dst])")      // dst[0..3] = xmm2
                __ASM_EMIT("movaps      %%xmm1, 0x18(%[dst])")      // dst[4..7] = xmm3
                __ASM_EMIT("movaps      %%xmm2, 0x28(%[dst])")      // dst[8..11] = xmm4
                __ASM_EMIT("movaps      %%xmm5, 0x38(%[dst])")      // dst[8..11] = xmm7

                // Prepare for even convolution
                __ASM_EMIT("movups      0x00(%[src]), %%xmm0")      // xmm0 = s0  s1  s2  s3
                __ASM_EMIT("movaps      %%xmm0, %%xmm3")            // xmm3 = s0  s1  s2  s3
                __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0")     // xmm0 = s0  s0  s0  s0
                __ASM_EMIT("shufps      $0xaa, %%xmm3, %%xmm3")     // xmm3 = s2  s2  s2  s2
                __ASM_EMIT("movaps      %%xmm0, %%xmm1")            // xmm1 = s0  s0  s0  s0
                __ASM_EMIT("movaps      %%xmm3, %%xmm4")            // xmm4 = s2  s2  s2  s2
                __ASM_EMIT("movaps      %%xmm0, %%xmm2")            // xmm2 = s0  s0  s0  s0
                __ASM_EMIT("movaps      %%xmm3, %%xmm5")            // xmm5 = s2  s2  s2  s2

                // Perform even convolution
                __ASM_EMIT("mulps       0x00(%[k]), %%xmm0")        // xmm0 = s0*k[0..3]
                __ASM_EMIT("mulps       0x00(%[k]), %%xmm3")        // xmm3 = s2*k[0..3]
                __ASM_EMIT("mulps       0x10(%[k]), %%xmm1")        // xmm1 = s0*k[4..7]
                __ASM_EMIT("mulps       0x10(%[k]), %%xmm4")        // xmm4 = s2*k[4..7]
                __ASM_EMIT("mulps       0x20(%[k]), %%xmm2")        // xmm2 = s0*k[8..11]
                __ASM_EMIT("mulps       0x20(%[k]), %%xmm5")        // xmm5 = s2*k[8..11]
                __ASM_EMIT("addps       %%xmm3, %%xmm1")            // xmm1 = s0*k[4..7] + s2*k[0..3]
                __ASM_EMIT("addps       %%xmm4, %%xmm2")            // xmm2 = s0*k[8..11] + s2*k[4..7]

                // Store even convolution
                __ASM_EMIT("movups      0x00(%[dst]), %%xmm3")      // xmm3 = dst[2..5]
                __ASM_EMIT("movups      0x10(%[dst]), %%xmm4")      // xmm4 = dst[6..9]
                __ASM_EMIT("movups      0x20(%[dst]), %%xmm6")      // xmm6 = dst[10..13]
                __ASM_EMIT("movups      0x30(%[dst]), %%xmm7")      // xmm7 = dst[14..17]
                __ASM_EMIT("addps       %%xmm0, %%xmm3")            // xmm3 = dst[2..5] + s0*k[0..3]
                __ASM_EMIT("addps       %%xmm1, %%xmm4")            // xmm4 = dst[6..9] + s0*k[4..7] + s2*k[0..3]
                __ASM_EMIT("addps       %%xmm2, %%xmm6")            // xmm6 = dst[10..13] + s0*k[8..11] + s2*k[4..7]
                __ASM_EMIT("addps       %%xmm5, %%xmm7")            // xmm7 = dst[14..17] + s2*k[8..11]
                __ASM_EMIT("movups      %%xmm3, 0x00(%[dst])")      // dst[2..5] = xmm3
                __ASM_EMIT("movups      %%xmm4, 0x10(%[dst])")      // dst[6..9] = xmm4
                __ASM_EMIT("movups      %%xmm6, 0x20(%[dst])")      // dst[10..13] = xmm6
                __ASM_EMIT("movups      %%xmm7, 0x30(%[dst])")      // dst[14..17] = xmm7

                // Add pointer and repeat loop
                __ASM_EMIT("sub         $4, %[count]")
                __ASM_EMIT("add         $0x20, %[dst]")
                __ASM_EMIT("add         $0x10, %[src]")
                __ASM_EMIT("cmp         $4, %[count]")
                __ASM_EMIT("jae         2b")
                __ASM_EMIT("jmp         200f")

                // dst is unaligned
                __ASM_EMIT(".align 16")
                __ASM_EMIT("3:")

                // Prepare for turn 1
                __ASM_EMIT("movups      0x00(%[src]), %%xmm0")      // xmm0 = s0  s1  s2  s3
                __ASM_EMIT("movaps      %%xmm0, %%xmm3")            // xmm3 = s0  s1  s2  s3
                __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0")     // xmm0 = s0  s0  s0  s0
                __ASM_EMIT("shufps      $0xaa, %%xmm3, %%xmm3")     // xmm3 = s2  s2  s2  s2
                __ASM_EMIT("movaps      %%xmm0, %%xmm1")            // xmm1 = s0  s0  s0  s0
                __ASM_EMIT("movaps      %%xmm3, %%xmm4")            // xmm4 = s2  s2  s2  s2
                __ASM_EMIT("movaps      %%xmm0, %%xmm2")            // xmm2 = s0  s0  s0  s0
                __ASM_EMIT("movaps      %%xmm3, %%xmm5")            // xmm5 = s2  s2  s2  s2

                // Perform turn 1 convolution
                __ASM_EMIT("mulps       0x00(%[k]), %%xmm0")        // xmm0 = s0*k[0..3]
                __ASM_EMIT("mulps       0x00(%[k]), %%xmm3")        // xmm3 = s2*k[0..3]
                __ASM_EMIT("mulps       0x10(%[k]), %%xmm1")        // xmm1 = s0*k[4..7]
                __ASM_EMIT("mulps       0x10(%[k]), %%xmm4")        // xmm4 = s2*k[4..7]
                __ASM_EMIT("mulps       0x20(%[k]), %%xmm2")        // xmm2 = s0*k[8..11]
                __ASM_EMIT("mulps       0x20(%[k]), %%xmm5")        // xmm5 = s2*k[8..11]
                __ASM_EMIT("addps       %%xmm3, %%xmm1")            // xmm1 = s0*k[4..7] + s2*k[0..3]
                __ASM_EMIT("addps       %%xmm4, %%xmm2")            // xmm2 = s0*k[8..11] + s2*k[4..7]

                // Store turn 1 convolution
                __ASM_EMIT("movups      0x00(%[dst]), %%xmm3")      // xmm3 = dst[0..3]
                __ASM_EMIT("movups      0x10(%[dst]), %%xmm4")      // xmm4 = dst[4..7]
                __ASM_EMIT("movups      0x20(%[dst]), %%xmm6")      // xmm6 = dst[8..11]
                __ASM_EMIT("movups      0x30(%[dst]), %%xmm7")      // xmm7 = dst[12..15]
                __ASM_EMIT("addps       %%xmm0, %%xmm3")            // xmm3 = dst[2..5] + s0*k[0..3]
                __ASM_EMIT("addps       %%xmm1, %%xmm4")            // xmm4 = dst[6..9] + s0*k[4..7] + s2*k[0..3]
                __ASM_EMIT("addps       %%xmm2, %%xmm6")            // xmm6 = dst[10..13] + s0*k[8..11] + s2*k[4..7]
                __ASM_EMIT("addps       %%xmm5, %%xmm7")            // xmm7 = dst[14..17] + s2*k[8..11]
                __ASM_EMIT("movups      %%xmm3, 0x00(%[dst])")      // dst[0..3] = xmm2
                __ASM_EMIT("movups      %%xmm4, 0x10(%[dst])")      // dst[4..7] = xmm3
                __ASM_EMIT("movups      %%xmm6, 0x20(%[dst])")      // dst[8..11] = xmm4
                __ASM_EMIT("movups      %%xmm7, 0x30(%[dst])")      // dst[8..11] = xmm7

                // Prepare for turn 2
                __ASM_EMIT("movups      0x00(%[src]), %%xmm0")      // xmm0 = s0  s1  s2  s3
                __ASM_EMIT("movaps      %%xmm0, %%xmm3")            // xmm3 = s0  s1  s2  s3
                __ASM_EMIT("shufps      $0x55, %%xmm0, %%xmm0")     // xmm0 = s1  s1  s1  s1
                __ASM_EMIT("shufps      $0xff, %%xmm3, %%xmm3")     // xmm3 = s3  s3  s3  s3
                __ASM_EMIT("movaps      %%xmm0, %%xmm1")            // xmm1 = s1  s1  s1  s1
                __ASM_EMIT("movaps      %%xmm3, %%xmm4")            // xmm4 = s3  s3  s3  s3
                __ASM_EMIT("movaps      %%xmm0, %%xmm2")            // xmm2 = s1  s1  s1  s1
                __ASM_EMIT("movaps      %%xmm3, %%xmm5")            // xmm5 = s3  s3  s3  s3

                // Perform turn 2 convolution
                __ASM_EMIT("mulps       0x00(%[k]), %%xmm0")        // xmm0 = s1*k[0..3]
                __ASM_EMIT("mulps       0x00(%[k]), %%xmm3")        // xmm3 = s3*k[0..3]
                __ASM_EMIT("mulps       0x10(%[k]), %%xmm1")        // xmm1 = s1*k[4..7]
                __ASM_EMIT("mulps       0x10(%[k]), %%xmm4")        // xmm4 = s3*k[4..7]
                __ASM_EMIT("mulps       0x20(%[k]), %%xmm2")        // xmm2 = s1*k[8..11]
                __ASM_EMIT("mulps       0x20(%[k]), %%xmm5")        // xmm5 = s3*k[8..11]
                __ASM_EMIT("addps       %%xmm3, %%xmm1")            // xmm1 = s1*k[4..7] + s3*k[0..3]
                __ASM_EMIT("addps       %%xmm4, %%xmm2")            // xmm2 = s1*k[8..11] + s3*k[4..7]

                // Store turn 2 convolution
                __ASM_EMIT("movups      0x08(%[dst]), %%xmm3")      // xmm3 = dst[0..3]
                __ASM_EMIT("movups      0x18(%[dst]), %%xmm4")      // xmm4 = dst[4..7]
                __ASM_EMIT("movups      0x28(%[dst]), %%xmm6")      // xmm6 = dst[8..11]
                __ASM_EMIT("movups      0x38(%[dst]), %%xmm7")      // xmm7 = dst[12..15]
                __ASM_EMIT("addps       %%xmm0, %%xmm3")            // xmm3 = dst[2..5] + s1*k[0..3]
                __ASM_EMIT("addps       %%xmm1, %%xmm4")            // xmm4 = dst[6..9] + s1*k[4..7] + s3*k[0..3]
                __ASM_EMIT("addps       %%xmm2, %%xmm6")            // xmm6 = dst[10..13] + s1*k[8..11] + s3*k[4..7]
                __ASM_EMIT("addps       %%xmm5, %%xmm7")            // xmm7 = dst[14..17] + s3*k[8..11]
                __ASM_EMIT("movups      %%xmm3, 0x08(%[dst])")      // dst[0..3] = xmm2
                __ASM_EMIT("movups      %%xmm4, 0x18(%[dst])")      // dst[4..7] = xmm3
                __ASM_EMIT("movups      %%xmm6, 0x28(%[dst])")      // dst[8..11] = xmm4
                __ASM_EMIT("movups      %%xmm7, 0x38(%[dst])")      // dst[8..11] = xmm7

                // Add pointer and repeat loop
                __ASM_EMIT("sub         $4, %[count]")
                __ASM_EMIT("add         $0x20, %[dst]")
                __ASM_EMIT("add         $0x10, %[src]")
                __ASM_EMIT("cmp         $4, %[count]")
                __ASM_EMIT("jae         3b")

                // Calculate tail
                __ASM_EMIT("200:")
                __ASM_EMIT("test        %[count], %[count]")
                __ASM_EMIT("jz          300f")

                // Repeat loop
                __ASM_EMIT(".align 16")
                __ASM_EMIT("4:")

                // Load samples and destination buffer
                __ASM_EMIT("movss       0x00(%[src]), %%xmm0")      // xmm0 = 0 0 0 s0
                __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0")     // xmm0 = s0 s0 s0 s0
                __ASM_EMIT("movaps      %%xmm0, %%xmm1")            // xmm1 = s0 s0 s0 s0
                __ASM_EMIT("movaps      %%xmm0, %%xmm2")            // xmm2 = s0 s0 s0 s0

                __ASM_EMIT("movups      0x00(%[dst]), %%xmm3")      // xmm3 = dst[0..3]
                __ASM_EMIT("movups      0x10(%[dst]), %%xmm4")      // xmm4 = dst[4..7]
                __ASM_EMIT("movups      0x20(%[dst]), %%xmm5")      // xmm5 = dst[8..11]

                // Perform convolution
                __ASM_EMIT("mulps       0x00(%[k]), %%xmm0")        // xmm0 = s0*k[0..3]
                __ASM_EMIT("mulps       0x10(%[k]), %%xmm1")        // xmm1 = s0*k[4..7]
                __ASM_EMIT("mulps       0x20(%[k]), %%xmm2")        // xmm2 = s0*k[8..11]

                __ASM_EMIT("addps       %%xmm3, %%xmm0")            // xmm0 = s0*k[0..3] + dst[0..3]
                __ASM_EMIT("addps       %%xmm4, %%xmm1")            // xmm1 = s0*k[4..7] + dst[4..7]
                __ASM_EMIT("addps       %%xmm5, %%xmm2")            // xmm2 = s0*k[8..11] + dst[8..11]

                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst])")      // dst[0..3] = xmm0
                __ASM_EMIT("movups      %%xmm1, 0x10(%[dst])")      // dst[4..7] = xmm1
                __ASM_EMIT("movups      %%xmm2, 0x20(%[dst])")      // dst[8..11] = xmm2

                __ASM_EMIT("add         $0x08, %[dst]")
                __ASM_EMIT("add         $0x04, %[src]")
                __ASM_EMIT("dec         %[count]")
                __ASM_EMIT("jnz         4b")

                // End of processing
                __ASM_EMIT("300:")

                : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
                : [k] "r" (lanczos_kernel_2x3)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }*/

        void lanczos_resample_3x2(float *dst, const float *src, size_t count)
        {
            __asm__ __volatile__
            (
                __ASM_EMIT("cmp         $2, %[count]")
                __ASM_EMIT("jb          2f")

                // Load samples
                __ASM_EMIT(".align 16")
                __ASM_EMIT("1:")
                __ASM_EMIT("movss       0x00(%[src]), %%xmm0")  // xmm0 = s0
                __ASM_EMIT("movss       0x04(%[src]), %%xmm4")  // xmm4 = s1
                __ASM_EMIT("sub         $2, %[count]")
                __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0") // xmm0 = s0
                __ASM_EMIT("shufps      $0x00, %%xmm4, %%xmm4") // xmm4 = s1
                __ASM_EMIT("movaps      %%xmm0, %%xmm1")        // xmm1 = s0
                __ASM_EMIT("movaps      %%xmm4, %%xmm5")        // xmm5 = s1
                __ASM_EMIT("movaps      %%xmm0, %%xmm2")        // xmm2 = s0
                __ASM_EMIT("movaps      %%xmm4, %%xmm6")        // xmm6 = s1

                // Do convolution
                __ASM_EMIT("mulps       %[m0], %%xmm0")         // xmm0 = s0*m0
                __ASM_EMIT("mulps       %[m1], %%xmm1")         // xmm1 = s0*m1
                __ASM_EMIT("mulps       %[m2], %%xmm2")         // xmm2 = s0*m2
                __ASM_EMIT("mulps       %[m3], %%xmm4")         // xmm4 = s1*m3
                __ASM_EMIT("mulps       %[m4], %%xmm5")         // xmm5 = s1*m4
                __ASM_EMIT("mulps       %[m5], %%xmm6")         // xmm6 = s1*m5
                __ASM_EMIT("addps       %%xmm4, %%xmm1")        // xmm1 = s0*m1 + s1*m3
                __ASM_EMIT("addps       %%xmm5, %%xmm2")        // xmm2 = s0*m2 + s1*m4
                __ASM_EMIT("movups      0x00(%[dst]), %%xmm3")  // xmm3 = d0
                __ASM_EMIT("movups      0x10(%[dst]), %%xmm4")  // xmm4 = d1
                __ASM_EMIT("movups      0x20(%[dst]), %%xmm5")  // xmm5 = d2
                __ASM_EMIT("movups      0x30(%[dst]), %%xmm7")  // xmm7 = d3
                __ASM_EMIT("addps       %%xmm0, %%xmm3")        // xmm3 = d0 + s0*m0
                __ASM_EMIT("addps       %%xmm1, %%xmm4")        // xmm4 = d1 + s0*m1 + s1*m3
                __ASM_EMIT("addps       %%xmm2, %%xmm5")        // xmm5 = d2 + s0*m2 + s1*m4
                __ASM_EMIT("addps       %%xmm6, %%xmm7")        // xmm6 = d3 + s1*m5
                __ASM_EMIT("movups      %%xmm3, 0x00(%[dst])")  // d0  += s0*m0
                __ASM_EMIT("movups      %%xmm4, 0x10(%[dst])")  // d1  += s0*m1 + s1*m3
                __ASM_EMIT("movups      %%xmm5, 0x20(%[dst])")  // d2  += s0*m2 + s1*m4
                __ASM_EMIT("movups      %%xmm7, 0x30(%[dst])")  // d3  += s1*m5

                // Update pointers
                __ASM_EMIT("add         $0x08, %[src]")
                __ASM_EMIT("add         $0x18, %[dst]")
                __ASM_EMIT("cmp         $2, %[count]")
                __ASM_EMIT("jae         1b")

                // Complete loop
                __ASM_EMIT("2:")
                __ASM_EMIT("test        %[count], %[count]")
                __ASM_EMIT("jz          3f")

                // Load sample
                __ASM_EMIT("movss       0x00(%[src]), %%xmm0")  // xmm0 = s0
                __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0") // xmm0 = s0
                __ASM_EMIT("movaps      %%xmm0, %%xmm1")        // xmm1 = s0
                __ASM_EMIT("movaps      %%xmm0, %%xmm2")        // xmm2 = s0

                // Do convolution
                __ASM_EMIT("mulps       %[m0], %%xmm0")         // xmm0 = s0*m0
                __ASM_EMIT("mulps       %[m1], %%xmm1")         // xmm1 = s0*m1
                __ASM_EMIT("mulps       %[m2], %%xmm2")         // xmm2 = s0*m2

                __ASM_EMIT("movups      0x00(%[dst]), %%xmm3")  // xmm3 = d0
                __ASM_EMIT("movups      0x10(%[dst]), %%xmm4")  // xmm4 = d1
                __ASM_EMIT("movups      0x20(%[dst]), %%xmm5")  // xmm5 = d2
                __ASM_EMIT("addps       %%xmm0, %%xmm3")        // xmm3 = d0 + s0*m0
                __ASM_EMIT("addps       %%xmm1, %%xmm4")        // xmm4 = d1 + s0*m1
                __ASM_EMIT("addps       %%xmm2, %%xmm5")        // xmm5 = d2 + s0*m2
                __ASM_EMIT("movups      %%xmm3, 0x00(%[dst])")  // d0  += s0*m0
                __ASM_EMIT("movups      %%xmm4, 0x10(%[dst])")  // d1  += s0*m1
                __ASM_EMIT("movups      %%xmm5, 0x20(%[dst])")  // d2  += s0*m2

                __ASM_EMIT("3:")

                : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
                : [m0] "m" (lanczos_3x2_m0),
                  [m1] "m" (lanczos_3x2_m1),
                  [m2] "m" (lanczos_3x2_m2),
                  [m3] "m" (lanczos_3x2_m3),
                  [m4] "m" (lanczos_3x2_m4),
                  [m5] "m" (lanczos_3x2_m5)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void lanczos_resample_3x3(float *dst, const float *src, size_t count)
        {
            __asm__ __volatile__
            (
                __ASM_EMIT("cmp         $2, %[count]")
                __ASM_EMIT("jb          2f")

                // Load samples
                __ASM_EMIT(".align 16")
                __ASM_EMIT("1:")
                __ASM_EMIT("movss       0x00(%[src]), %%xmm0")  // xmm0 = s0
                __ASM_EMIT("movss       0x04(%[src]), %%xmm4")  // xmm4 = s1
                __ASM_EMIT("sub         $2, %[count]")
                __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0") // xmm0 = s0
                __ASM_EMIT("shufps      $0x00, %%xmm4, %%xmm4") // xmm4 = s1
                __ASM_EMIT("movaps      %%xmm0, %%xmm1")        // xmm1 = s0
                __ASM_EMIT("movaps      %%xmm4, %%xmm5")        // xmm5 = s1
                __ASM_EMIT("movaps      %%xmm0, %%xmm2")        // xmm2 = s0
                __ASM_EMIT("movaps      %%xmm4, %%xmm6")        // xmm6 = s1
                __ASM_EMIT("movaps      %%xmm0, %%xmm3")        // xmm3 = s0

                // Do convolution
                __ASM_EMIT("mulps       %[m0], %%xmm1")         // xmm1 = s0*m0
                __ASM_EMIT("mulps       %[m1], %%xmm2")         // xmm2 = s0*m1
                __ASM_EMIT("mulps       %[m2], %%xmm3")         // xmm3 = s0*m2
                __ASM_EMIT("mulps       %[m5], %%xmm5")         // xmm5 = s1*m5
                __ASM_EMIT("mulps       %[m6], %%xmm6")         // xmm6 = s1*m6
                __ASM_EMIT("addps       %%xmm5, %%xmm2")        // xmm2 = s0*m1 + s1*m5
                __ASM_EMIT("addps       %%xmm6, %%xmm3")        // xmm3 = s0*m2 + s1*m6
                __ASM_EMIT("movups      0x00(%[dst]), %%xmm5")  // xmm5 = d0
                __ASM_EMIT("movups      0x10(%[dst]), %%xmm6")  // xmm6 = d1
                __ASM_EMIT("movups      0x20(%[dst]), %%xmm7")  // xmm7 = d2
                __ASM_EMIT("addps       %%xmm1, %%xmm5")        // xmm5 = d0 + s0*m0
                __ASM_EMIT("addps       %%xmm2, %%xmm6")        // xmm6 = d1 + s0*m1 + s1*m5
                __ASM_EMIT("addps       %%xmm3, %%xmm7")        // xmm7 = d2 + s0*m2 + s1*m6
                __ASM_EMIT("movups      %%xmm5, 0x00(%[dst])")  // d0  += s0*m0
                __ASM_EMIT("movups      %%xmm6, 0x10(%[dst])")  // d1  += s0*m1 + s1*m5
                __ASM_EMIT("movups      %%xmm7, 0x20(%[dst])")  // d2  += s0*m2 + s1*m6

                __ASM_EMIT("movaps      %%xmm4, %%xmm5")        // xmm4 = s1
                __ASM_EMIT("movaps      %%xmm0, %%xmm1")        // xmm1 = s0
                __ASM_EMIT("movaps      %%xmm4, %%xmm6")        // xmm6 = s1
                __ASM_EMIT("mulps       %[m3], %%xmm0")         // xmm0 = s0*m3
                __ASM_EMIT("mulps       %[m4], %%xmm1")         // xmm1 = s0*m4
                __ASM_EMIT("mulps       %[m7], %%xmm4")         // xmm4 = s1*m7
                __ASM_EMIT("mulps       %[m8], %%xmm5")         // xmm5 = s1*m8
                __ASM_EMIT("mulss       %[m9], %%xmm6")         // xmm6 = s1*m9
                __ASM_EMIT("addps       %%xmm4, %%xmm0")        // xmm0 = s0*m3 + s1*m7
                __ASM_EMIT("addps       %%xmm5, %%xmm1")        // xmm1 = s0*m4 + s1*m8
                __ASM_EMIT("movups      0x30(%[dst]), %%xmm2")  // xmm2 = d3
                __ASM_EMIT("movups      0x40(%[dst]), %%xmm3")  // xmm3 = d4
                __ASM_EMIT("addss       0x50(%[dst]), %%xmm6")  // xmm6 = d5 + s1*m9
                __ASM_EMIT("addps       %%xmm2, %%xmm0")        // xmm0 = d3 + s0*m3 + s1*m7
                __ASM_EMIT("addps       %%xmm3, %%xmm1")        // xmm1 = d1 + s0*m4 + s1*m8
                __ASM_EMIT("movups      %%xmm0, 0x30(%[dst])")  // d3  += s0*m3 + s1*m7
                __ASM_EMIT("movups      %%xmm1, 0x40(%[dst])")  // d4  += s0*m4 + s1*m8
                __ASM_EMIT("movss       %%xmm6, 0x50(%[dst])")  // d5  += s1*m9

                // Update pointers
                __ASM_EMIT("add         $0x08, %[src]")
                __ASM_EMIT("add         $0x18, %[dst]")
                __ASM_EMIT("cmp         $2, %[count]")
                __ASM_EMIT("jae         1b")

                // Complete loop
                __ASM_EMIT("2:")
                __ASM_EMIT("test        %[count], %[count]")
                __ASM_EMIT("jz          3f")

                // Load samples
                __ASM_EMIT("movss       0x00(%[src]), %%xmm0")  // xmm0 = s0
                __ASM_EMIT("movups      0x00(%[dst]), %%xmm5")  // xmm5 = d0
                __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0") // xmm0 = s0
                __ASM_EMIT("movups      0x10(%[dst]), %%xmm6")  // xmm6 = d1
                __ASM_EMIT("movaps      %%xmm0, %%xmm1")        // xmm1 = s0
                __ASM_EMIT("movups      0x20(%[dst]), %%xmm7")  // xmm7 = d2
                __ASM_EMIT("movaps      %%xmm0, %%xmm2")        // xmm2 = s0
                __ASM_EMIT("movaps      %%xmm1, %%xmm3")        // xmm3 = s0
                __ASM_EMIT("movaps      %%xmm0, %%xmm4")        // xmm4 = s0

                // Do convolution
                __ASM_EMIT("mulps       %[m0], %%xmm0")         // xmm0 = s0*m0
                __ASM_EMIT("mulps       %[m1], %%xmm1")         // xmm1 = s0*m1
                __ASM_EMIT("mulps       %[m2], %%xmm2")         // xmm2 = s0*m2
                __ASM_EMIT("mulps       %[m3], %%xmm3")         // xmm3 = s0*m3
                __ASM_EMIT("mulps       %[m4], %%xmm4")         // xmm4 = s0*m4

                __ASM_EMIT("addps       %%xmm5, %%xmm0")        // xmm0 = d0 + s0*m0
                __ASM_EMIT("addps       %%xmm6, %%xmm1")        // xmm1 = d1 + s0*m1
                __ASM_EMIT("addps       %%xmm7, %%xmm2")        // xmm2 = d2 + s0*m2
                __ASM_EMIT("movups      0x30(%[dst]), %%xmm5")  // xmm5 = d3
                __ASM_EMIT("movups      0x40(%[dst]), %%xmm6")  // xmm6 = d4
                __ASM_EMIT("addps       %%xmm5, %%xmm3")        // xmm1 = d1 + s0*m1
                __ASM_EMIT("addps       %%xmm6, %%xmm4")        // xmm2 = d2 + s0*m2
                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst])")  // d0  += s0*m0
                __ASM_EMIT("movups      %%xmm1, 0x10(%[dst])")  // d1  += s0*m1
                __ASM_EMIT("movups      %%xmm2, 0x20(%[dst])")  // d2  += s0*m2
                __ASM_EMIT("movups      %%xmm3, 0x30(%[dst])")  // d3  += s0*m3
                __ASM_EMIT("movups      %%xmm4, 0x40(%[dst])")  // d3  += s0*m4

                // End of loop
                __ASM_EMIT("3:")

                : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
                : [m0] "m" (lanczos_3x3_m0),
                  [m1] "m" (lanczos_3x3_m1),
                  [m2] "m" (lanczos_3x3_m2),
                  [m3] "m" (lanczos_3x3_m3),
                  [m4] "m" (lanczos_3x3_m4),
                  [m5] "m" (lanczos_3x3_m5),
                  [m6] "m" (lanczos_3x3_m6),
                  [m7] "m" (lanczos_3x3_m7),
                  [m8] "m" (lanczos_3x3_m8),
                  [m9] "m" (lanczos_3x3_m9)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void lanczos_resample_4x2(float *dst, const float *src, size_t count)
        {
            __asm__ __volatile__
            (
                __ASM_EMIT("cmp         $2, %[count]")
                __ASM_EMIT("jb          2f")

                // Load samples
                __ASM_EMIT(".align 16")
                __ASM_EMIT("1:")
                __ASM_EMIT("movss       0x00(%[src]), %%xmm0")  // xmm0 = s0
                __ASM_EMIT("movss       0x04(%[src]), %%xmm4")  // xmm4 = s1
                __ASM_EMIT("sub         $2, %[count]")
                __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0") // xmm0 = s0
                __ASM_EMIT("shufps      $0x00, %%xmm4, %%xmm4") // xmm4 = s1
                __ASM_EMIT("movaps      %%xmm0, %%xmm1")        // xmm1 = s0
                __ASM_EMIT("movaps      %%xmm4, %%xmm5")        // xmm5 = s1
                __ASM_EMIT("movaps      %%xmm0, %%xmm2")        // xmm2 = s0
                __ASM_EMIT("movaps      %%xmm4, %%xmm6")        // xmm6 = s1
                __ASM_EMIT("movaps      %%xmm0, %%xmm3")        // xmm3 = s0
                __ASM_EMIT("movaps      %%xmm4, %%xmm7")        // xmm7 = s1

                // Do convolution
                __ASM_EMIT("mulps       0x00(%[k]), %%xmm0")    // xmm0 = s0*k0
                __ASM_EMIT("mulps       0x00(%[k]), %%xmm4")    // xmm4 = s1*k0
                __ASM_EMIT("mulps       0x10(%[k]), %%xmm1")    // xmm1 = s0*k1
                __ASM_EMIT("mulps       0x10(%[k]), %%xmm5")    // xmm5 = s1*k1
                __ASM_EMIT("mulps       0x20(%[k]), %%xmm2")    // xmm2 = s0*k2
                __ASM_EMIT("mulps       0x20(%[k]), %%xmm6")    // xmm6 = s1*k2
                __ASM_EMIT("mulps       0x30(%[k]), %%xmm3")    // xmm3 = s0*k3
                __ASM_EMIT("mulps       0x30(%[k]), %%xmm7")    // xmm7 = s1*k3
                __ASM_EMIT("addps       %%xmm4, %%xmm1")        // xmm1 = s0*k1 + s1*k0
                __ASM_EMIT("addps       %%xmm5, %%xmm2")        // xmm2 = s0*k2 + s1*k1
                __ASM_EMIT("addps       %%xmm6, %%xmm3")        // xmm3 = s0*k3 + s1*k2
                __ASM_EMIT("movups      0x00(%[dst]), %%xmm4")  // xmm4 = d0
                __ASM_EMIT("movups      0x10(%[dst]), %%xmm5")  // xmm5 = d1
                __ASM_EMIT("movups      0x20(%[dst]), %%xmm6")  // xmm6 = d2
                __ASM_EMIT("addps       %%xmm0, %%xmm4")        // xmm4 = d0 + s0*k0
                __ASM_EMIT("addps       %%xmm1, %%xmm5")        // xmm5 = d1 + s0*k1 + s1*k0
                __ASM_EMIT("addps       %%xmm2, %%xmm6")        // xmm6 = d2 + s0*k2 + s1*k1
                __ASM_EMIT("movups      0x30(%[dst]), %%xmm0")  // xmm0 = d3
                __ASM_EMIT("movups      0x40(%[dst]), %%xmm1")  // xmm1 = d4
                __ASM_EMIT("addps       %%xmm3, %%xmm0")        // xmm3 = d3 + s0*k3 + s1*k2
                __ASM_EMIT("addps       %%xmm7, %%xmm1")        // xmm7 = d4 + s1*k3
                __ASM_EMIT("movups      %%xmm4, 0x00(%[dst])")  // d0  += s0*k0
                __ASM_EMIT("movups      %%xmm5, 0x10(%[dst])")  // d1  += s0*k1 + s1*k0
                __ASM_EMIT("movups      %%xmm6, 0x20(%[dst])")  // d2  += s0*k2 + s1*k1
                __ASM_EMIT("movups      %%xmm0, 0x30(%[dst])")  // d3  += s0*k3 + s1*k2
                __ASM_EMIT("movups      %%xmm1, 0x40(%[dst])")  // d4  += s1*k3

                // Update pointers
                __ASM_EMIT("add         $0x08, %[src]")
                __ASM_EMIT("add         $0x20, %[dst]")
                __ASM_EMIT("cmp         $2, %[count]")
                __ASM_EMIT("jae         1b")

                // Complete loop
                __ASM_EMIT("2:")
                __ASM_EMIT("test        %[count], %[count]")
                __ASM_EMIT("jz          3f")

                __ASM_EMIT("movss       0x00(%[src]), %%xmm0")  // xmm0 = s0
                __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0") // xmm0 = s0
                __ASM_EMIT("movaps      %%xmm0, %%xmm1")        // xmm1 = s0
                __ASM_EMIT("movaps      %%xmm0, %%xmm2")        // xmm2 = s0
                __ASM_EMIT("movaps      %%xmm1, %%xmm3")        // xmm3 = s0

                // Do convolution
                __ASM_EMIT("mulps       0x00(%[k]), %%xmm0")    // xmm0 = s0*k0
                __ASM_EMIT("mulps       0x10(%[k]), %%xmm1")    // xmm1 = s0*k1
                __ASM_EMIT("mulps       0x20(%[k]), %%xmm2")    // xmm2 = s0*k2
                __ASM_EMIT("mulps       0x30(%[k]), %%xmm3")    // xmm3 = s0*k3
                __ASM_EMIT("movups      0x00(%[dst]), %%xmm4")  // xmm4 = d0
                __ASM_EMIT("movups      0x10(%[dst]), %%xmm5")  // xmm5 = d1
                __ASM_EMIT("movups      0x20(%[dst]), %%xmm6")  // xmm6 = d2
                __ASM_EMIT("movups      0x30(%[dst]), %%xmm7")  // xmm7 = d3
                __ASM_EMIT("addps       %%xmm0, %%xmm4")        // xmm4 = d0 + s0*k0
                __ASM_EMIT("addps       %%xmm1, %%xmm5")        // xmm5 = d1 + s0*k1
                __ASM_EMIT("addps       %%xmm2, %%xmm6")        // xmm6 = d2 + s0*k2
                __ASM_EMIT("addps       %%xmm3, %%xmm7")        // xmm7 = d3 + s0*k3
                __ASM_EMIT("movups      %%xmm4, 0x00(%[dst])")  // d0  += s0*k0
                __ASM_EMIT("movups      %%xmm5, 0x10(%[dst])")  // d1  += s0*k1
                __ASM_EMIT("movups      %%xmm6, 0x20(%[dst])")  // d2  += s0*k2
                __ASM_EMIT("movups      %%xmm7, 0x30(%[dst])")  // d2  += s0*k2

                // End of loop
                __ASM_EMIT("3:")

                : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
                : [k] "r" (lanczos_kernel_4x2)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void lanczos_resample_4x3(float *dst, const float *src, size_t count)
        {
            __asm__ __volatile__
            (
                __ASM_EMIT("cmp         $2, %[count]")
                __ASM_EMIT("jb          2f")

                // Load samples
                __ASM_EMIT(".align 16")
                __ASM_EMIT("1:")
                __ASM_EMIT("movss       0x00(%[src]), %%xmm0")  // xmm0 = s0
                __ASM_EMIT("movss       0x04(%[src]), %%xmm4")  // xmm4 = s1
                __ASM_EMIT("sub         $2, %[count]")
                __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0") // xmm0 = s0
                __ASM_EMIT("shufps      $0x00, %%xmm4, %%xmm4") // xmm4 = s1
                __ASM_EMIT("movaps      %%xmm0, %%xmm1")        // xmm1 = s0
                __ASM_EMIT("movaps      %%xmm4, %%xmm5")        // xmm5 = s1
                __ASM_EMIT("movaps      %%xmm0, %%xmm2")        // xmm2 = s0
                __ASM_EMIT("movaps      %%xmm4, %%xmm6")        // xmm6 = s1
                __ASM_EMIT("movaps      %%xmm0, %%xmm3")        // xmm3 = s0
                __ASM_EMIT("movaps      %%xmm4, %%xmm7")        // xmm7 = s1

                // Do convolution
                __ASM_EMIT("mulps       0x00(%[k]), %%xmm1")    // xmm1 = s0*k0
                __ASM_EMIT("mulps       0x00(%[k]), %%xmm5")    // xmm5 = s1*k0
                __ASM_EMIT("mulps       0x10(%[k]), %%xmm2")    // xmm2 = s0*k1
                __ASM_EMIT("mulps       0x10(%[k]), %%xmm6")    // xmm6 = s1*k1
                __ASM_EMIT("mulps       0x20(%[k]), %%xmm3")    // xmm3 = s0*k2
                __ASM_EMIT("mulps       0x20(%[k]), %%xmm7")    // xmm7 = s1*k2
                __ASM_EMIT("addps       %%xmm5, %%xmm2")        // xmm2 = s0*k1 + s1*k0
                __ASM_EMIT("addps       %%xmm6, %%xmm3")        // xmm3 = s0*k2 + s1*k1
                __ASM_EMIT("movups      0x00(%[dst]), %%xmm5")  // xmm5 = d0
                __ASM_EMIT("movups      0x10(%[dst]), %%xmm6")  // xmm6 = d1
                __ASM_EMIT("addps       %%xmm1, %%xmm5")        // xmm5 = d0 + s0*k0
                __ASM_EMIT("addps       %%xmm2, %%xmm6")        // xmm6 = d1 + s0*k1 + s1*k0
                __ASM_EMIT("movups      %%xmm5, 0x00(%[dst])")  // d0  += s0*k0
                __ASM_EMIT("movups      %%xmm6, 0x10(%[dst])")  // d1  += s0*k1 + s1*k0
                __ASM_EMIT("movups      0x20(%[dst]), %%xmm5")  // xmm5 = d2
                __ASM_EMIT("movaps      %%xmm0, %%xmm1")        // xmm1 = s0
                __ASM_EMIT("addps       %%xmm3, %%xmm5")        // xmm5 = d2 + s0*k2 + s1*k1
                __ASM_EMIT("movaps      %%xmm4, %%xmm3")        // xmm3 = s1
                __ASM_EMIT("movups      %%xmm5, 0x20(%[dst])")  // d2  += s0*k2 + s1*k1
                __ASM_EMIT("movaps      %%xmm0, %%xmm2")        // xmm2 = s0
                __ASM_EMIT("movaps      %%xmm4, %%xmm5")        // xmm5 = s1
                __ASM_EMIT("mulps       0x30(%[k]), %%xmm0")    // xmm0 = s0*k3
                __ASM_EMIT("mulps       0x30(%[k]), %%xmm3")    // xmm3 = s1*k3
                __ASM_EMIT("mulps       0x40(%[k]), %%xmm1")    // xmm1 = s0*k4
                __ASM_EMIT("mulps       0x40(%[k]), %%xmm4")    // xmm4 = s1*k4
                __ASM_EMIT("mulps       0x50(%[k]), %%xmm2")    // xmm2 = s0*k5
                __ASM_EMIT("mulps       0x50(%[k]), %%xmm5")    // xmm5 = s1*k5
                __ASM_EMIT("addps       %%xmm7, %%xmm0")        // xmm0 = s0*k3 + s1*k2
                __ASM_EMIT("addps       %%xmm3, %%xmm1")        // xmm1 = s0*k4 + s1*k3
                __ASM_EMIT("addps       %%xmm4, %%xmm2")        // xmm2 = s0*k5 + s1*k4

                __ASM_EMIT("movups      0x30(%[dst]), %%xmm3")  // xmm3 = d3
                __ASM_EMIT("movups      0x40(%[dst]), %%xmm4")  // xmm4 = d4
                __ASM_EMIT("movups      0x50(%[dst]), %%xmm6")  // xmm6 = d5
                __ASM_EMIT("movups      0x60(%[dst]), %%xmm7")  // xmm7 = d6
                __ASM_EMIT("addps       %%xmm0, %%xmm3")        // xmm3 = d3 + s0*k3 + s1*k2
                __ASM_EMIT("addps       %%xmm1, %%xmm4")        // xmm4 = d4 + s0*k4 + s1*k3
                __ASM_EMIT("addps       %%xmm2, %%xmm6")        // xmm6 = d5 + s0*k5 + s1*k4
                __ASM_EMIT("addps       %%xmm5, %%xmm7")        // xmm7 = d6 + s1*k5
                __ASM_EMIT("movups      %%xmm3, 0x30(%[dst])")  // d3  += s0*k3 + s1*k2
                __ASM_EMIT("movups      %%xmm4, 0x40(%[dst])")  // d4  += s0*k4 + s1*k3
                __ASM_EMIT("movups      %%xmm6, 0x50(%[dst])")  // d5  += s0*k5 + s1*k4
                __ASM_EMIT("movups      %%xmm7, 0x60(%[dst])")  // d6  += s1*k5

                // Update pointers
                __ASM_EMIT("add         $0x08, %[src]")
                __ASM_EMIT("add         $0x20, %[dst]")
                __ASM_EMIT("cmp         $2, %[count]")
                __ASM_EMIT("jae         1b")

                // Complete loop
                __ASM_EMIT("2:")
                __ASM_EMIT("test        %[count], %[count]")
                __ASM_EMIT("jz          3f")

                __ASM_EMIT("movss       0x00(%[src]), %%xmm0")  // xmm0 = s0
                __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0") // xmm0 = s0
                __ASM_EMIT("movaps      %%xmm0, %%xmm1")        // xmm1 = s0
                __ASM_EMIT("movaps      %%xmm0, %%xmm2")        // xmm2 = s0
                __ASM_EMIT("movaps      %%xmm1, %%xmm3")        // xmm3 = s0
                __ASM_EMIT("movaps      %%xmm0, %%xmm4")        // xmm4 = s0
                __ASM_EMIT("movaps      %%xmm2, %%xmm5")        // xmm5 = s0

                // Do convolution
                __ASM_EMIT("mulps       0x00(%[k]), %%xmm0")    // xmm0 = s0*k0
                __ASM_EMIT("mulps       0x10(%[k]), %%xmm1")    // xmm1 = s0*k1
                __ASM_EMIT("mulps       0x20(%[k]), %%xmm2")    // xmm2 = s0*k2
                __ASM_EMIT("mulps       0x30(%[k]), %%xmm3")    // xmm3 = s0*k3
                __ASM_EMIT("mulps       0x40(%[k]), %%xmm4")    // xmm2 = s0*k4
                __ASM_EMIT("mulps       0x50(%[k]), %%xmm5")    // xmm3 = s0*k5
                __ASM_EMIT("movups      0x00(%[dst]), %%xmm6")  // xmm6 = d0
                __ASM_EMIT("movups      0x10(%[dst]), %%xmm7")  // xmm7 = d1
                __ASM_EMIT("addps       %%xmm0, %%xmm6")        // xmm4 = d0 + s0*k0
                __ASM_EMIT("addps       %%xmm1, %%xmm7")        // xmm5 = d1 + s0*k1
                __ASM_EMIT("movups      %%xmm6, 0x00(%[dst])")  // d0  += s0*k0
                __ASM_EMIT("movups      %%xmm7, 0x10(%[dst])")  // d1  += s0*k1
                __ASM_EMIT("movups      0x20(%[dst]), %%xmm0")  // xmm0 = d2
                __ASM_EMIT("movups      0x30(%[dst]), %%xmm1")  // xmm1 = d3
                __ASM_EMIT("movups      0x40(%[dst]), %%xmm6")  // xmm6 = d4
                __ASM_EMIT("movups      0x50(%[dst]), %%xmm7")  // xmm7 = d5
                __ASM_EMIT("addps       %%xmm2, %%xmm0")        // xmm0 = d2 + s0*k2
                __ASM_EMIT("addps       %%xmm3, %%xmm1")        // xmm1 = d3 + s0*k3
                __ASM_EMIT("addps       %%xmm4, %%xmm6")        // xmm6 = d4 + s0*k4
                __ASM_EMIT("addps       %%xmm5, %%xmm7")        // xmm7 = d5 + s0*k5
                __ASM_EMIT("movups      %%xmm0, 0x20(%[dst])")  // d2  += s0*k2
                __ASM_EMIT("movups      %%xmm1, 0x30(%[dst])")  // d3  += s0*k3
                __ASM_EMIT("movups      %%xmm6, 0x40(%[dst])")  // d4  += s0*k4
                __ASM_EMIT("movups      %%xmm7, 0x50(%[dst])")  // d5  += s0*k5

                // End of loop
                __ASM_EMIT("3:")

                : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
                : [k] "r" (lanczos_kernel_4x3)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void downsample_2x(float *dst, const float *src, size_t count)
        {
            __asm__ __volatile__
            (
                __ASM_EMIT("cmp         $16, %[count]")
                __ASM_EMIT("jb          2f")

                // 16 x sample loop
                __ASM_EMIT(".align 16")
                __ASM_EMIT("1:")
                __ASM_EMIT("movups      0x00(%[src]), %%xmm0")  // xmm0 = s0 ? s1 ?
                __ASM_EMIT("movups      0x10(%[src]), %%xmm4")  // xmm4 = s2 ? s3 ?
                __ASM_EMIT("movups      0x20(%[src]), %%xmm1")  // xmm1 = s4 x s5 ?
                __ASM_EMIT("movups      0x30(%[src]), %%xmm5")  // xmm5 = s6 ? s7 ?
                __ASM_EMIT("movups      0x40(%[src]), %%xmm2")  // xmm2 = s8 ? s9 ?
                __ASM_EMIT("movups      0x50(%[src]), %%xmm6")  // xmm6 = s10 ? s11 ?
                __ASM_EMIT("movups      0x60(%[src]), %%xmm3")  // xmm3 = s12 ? s13 ?
                __ASM_EMIT("movups      0x70(%[src]), %%xmm7")  // xmm7 = s14 ? s15 ?

                __ASM_EMIT("shufps      $0x88, %%xmm4, %%xmm0") // xmm0 = s0 s1 s2 s3
                __ASM_EMIT("shufps      $0x88, %%xmm5, %%xmm1") // xmm1 = s4 s5 s6 s7
                __ASM_EMIT("shufps      $0x88, %%xmm6, %%xmm2") // xmm2 = s8 s9 s10 s11
                __ASM_EMIT("shufps      $0x88, %%xmm7, %%xmm3") // xmm3 = s12 s13 s14 s15

                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("movups      %%xmm1, 0x10(%[dst])")
                __ASM_EMIT("movups      %%xmm2, 0x20(%[dst])")
                __ASM_EMIT("movups      %%xmm3, 0x30(%[dst])")

                // Repeat loop
                __ASM_EMIT("sub         $16, %[count]")
                __ASM_EMIT("add         $0x80, %[src]")
                __ASM_EMIT("add         $0x40, %[dst]")
                __ASM_EMIT("cmp         $16, %[count]")
                __ASM_EMIT("jae         1b")

                // 4x iterations
                __ASM_EMIT("2:")
                __ASM_EMIT("cmp         $4, %[count]")
                __ASM_EMIT("jb          4f")

                __ASM_EMIT("3:")
                __ASM_EMIT("movups      0x00(%[src]), %%xmm0")  // xmm0 = s0 ? s1 ?
                __ASM_EMIT("movups      0x10(%[src]), %%xmm4")  // xmm4 = s2 ? s3 ?
                __ASM_EMIT("shufps      $0x88, %%xmm4, %%xmm0") // xmm0 = s0 s1 s2 s3
                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst])")

                __ASM_EMIT("sub         $4, %[count]")
                __ASM_EMIT("add         $0x20, %[src]")
                __ASM_EMIT("add         $0x10, %[dst]")
                __ASM_EMIT("cmp         $4, %[count]")
                __ASM_EMIT("jae         3b")

                // 1x iterations
                __ASM_EMIT("4:")
                __ASM_EMIT("test        %[count], %[count]")
                __ASM_EMIT("jz          6f")

                __ASM_EMIT("5:")
                __ASM_EMIT("movss       0x00(%[src]), %%xmm0")  // xmm0 = s0
                __ASM_EMIT("movss       %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add         $0x8, %[src]")
                __ASM_EMIT("add         $0x4, %[dst]")
                __ASM_EMIT("dec         %[count]")
                __ASM_EMIT("jnz         5b")

                // End of routine
                __ASM_EMIT("6:")

                : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
                :
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void downsample_3x(float *dst, const float *src, size_t count)
        {
            __asm__ __volatile__
            (
                __ASM_EMIT("cmp         $16, %[count]")
                __ASM_EMIT("jb          2f")

                // 16 x sample loop
                __ASM_EMIT(".align 16")
                __ASM_EMIT("1:")
                __ASM_EMIT("movups      0x00(%[src]), %%xmm0")  // xmm0 = s0 ? ? s1
                __ASM_EMIT("movss       0x18(%[src]), %%xmm1")  // xmm1 = s2 ? ? ?
                __ASM_EMIT("movss       0x24(%[src]), %%xmm6")  // xmm6 = s3 ? ? ?
                __ASM_EMIT("movups      0x30(%[src]), %%xmm2")  // xmm2 = s4 ? ? s5
                __ASM_EMIT("movss       0x48(%[src]), %%xmm3")  // xmm3 = s6 ? ? ?
                __ASM_EMIT("movss       0x54(%[src]), %%xmm7")  // xmm7 = s7 ? ? ?
                __ASM_EMIT("movlhps     %%xmm6, %%xmm1")        // xmm1 = s2 ? s3 ?
                __ASM_EMIT("movlhps     %%xmm7, %%xmm3")        // xmm3 = s6 ? s7 ?
                __ASM_EMIT("shufps      $0x8c, %%xmm1, %%xmm0") // xmm0 = s0 s1 s2 s3
                __ASM_EMIT("shufps      $0x8c, %%xmm3, %%xmm2") // xmm2 = s4 s5 s6 s7

                __ASM_EMIT("movups      0x60(%[src]), %%xmm4")  // xmm4 = s8 ? ? s9
                __ASM_EMIT("movss       0x78(%[src]), %%xmm5")  // xmm5 = s10 ? ? ?
                __ASM_EMIT("movss       0x84(%[src]), %%xmm1")  // xmm1 = s11 ? ? ?
                __ASM_EMIT("movups      0x90(%[src]), %%xmm6")  // xmm6 = s12 ? ? s13
                __ASM_EMIT("movss       0xa8(%[src]), %%xmm7")  // xmm7 = s14 ? ? ?
                __ASM_EMIT("movss       0xb4(%[src]), %%xmm3")  // xmm3 = s15 ? ? ?
                __ASM_EMIT("movlhps     %%xmm1, %%xmm5")        // xmm5 = s10 ? s11 ?
                __ASM_EMIT("movlhps     %%xmm3, %%xmm7")        // xmm7 = s14 ? s15 ?
                __ASM_EMIT("shufps      $0x8c, %%xmm5, %%xmm4") // xmm4 = s8 s9 s10 s11
                __ASM_EMIT("shufps      $0x8c, %%xmm7, %%xmm6") // xmm6 = s12 s13 s14 s15

                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("movups      %%xmm2, 0x10(%[dst])")
                __ASM_EMIT("movups      %%xmm4, 0x20(%[dst])")
                __ASM_EMIT("movups      %%xmm6, 0x30(%[dst])")

                // Repeat loop
                __ASM_EMIT("sub         $16, %[count]")
                __ASM_EMIT("add         $0xc0, %[src]")
                __ASM_EMIT("add         $0x40, %[dst]")
                __ASM_EMIT("cmp         $16, %[count]")
                __ASM_EMIT("jae         1b")

                // 4x iterations
                __ASM_EMIT("2:")
                __ASM_EMIT("cmp         $4, %[count]")
                __ASM_EMIT("jb          4f")

                __ASM_EMIT("3:")
                __ASM_EMIT("movups      0x00(%[src]), %%xmm0")  // xmm0 = s0 ? s1 ?
                __ASM_EMIT("movups      0x18(%[src]), %%xmm1")  // xmm1 = s2 ? ? ?
                __ASM_EMIT("movups      0x24(%[src]), %%xmm2")  // xmm2 = s3 ? ? ?
                __ASM_EMIT("movlhps     %%xmm2, %%xmm1")        // xmm1 = s2 ? s3 ?
                __ASM_EMIT("shufps      $0x8c, %%xmm1, %%xmm0") // xmm0 = s0 s1 s2 s3
                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst])")

                __ASM_EMIT("sub         $4, %[count]")
                __ASM_EMIT("add         $0x30, %[src]")
                __ASM_EMIT("add         $0x10, %[dst]")
                __ASM_EMIT("cmp         $4, %[count]")
                __ASM_EMIT("jae         3b")

                // 1x iterations
                __ASM_EMIT("4:")
                __ASM_EMIT("test        %[count], %[count]")
                __ASM_EMIT("jz          6f")

                __ASM_EMIT("5:")
                __ASM_EMIT("movss       0x00(%[src]), %%xmm0")  // xmm0 = s0
                __ASM_EMIT("movss       %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add         $0xc, %[src]")
                __ASM_EMIT("add         $0x4, %[dst]")
                __ASM_EMIT("dec         %[count]")
                __ASM_EMIT("jnz         5b")

                // End of routine
                __ASM_EMIT("6:")

                : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
                :
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void downsample_4x(float *dst, const float *src, size_t count)
        {
            __asm__ __volatile__
            (
                __ASM_EMIT("cmp         $16, %[count]")
                __ASM_EMIT("jb          2f")

                // 16 x sample loop
                __ASM_EMIT(".align 16")
                __ASM_EMIT("1:")
                __ASM_EMIT("movss       0x00(%[src]), %%xmm0")  // xmm0 = s0 ? ? ?
                __ASM_EMIT("movss       0x10(%[src]), %%xmm2")  // xmm2 = s1 ? ? ?
                __ASM_EMIT("movss       0x20(%[src]), %%xmm4")  // xmm4 = s2 ? ? ?
                __ASM_EMIT("movss       0x30(%[src]), %%xmm3")  // xmm3 = s3 ? ? ?
                __ASM_EMIT("movlhps     %%xmm2, %%xmm0")        // xmm0 = s0 ? s1 ?
                __ASM_EMIT("movlhps     %%xmm3, %%xmm4")        // xmm4 = s2 ? s3 ?

                __ASM_EMIT("movss       0x40(%[src]), %%xmm1")  // xmm1 = s4 ? ? ?
                __ASM_EMIT("movss       0x50(%[src]), %%xmm2")  // xmm2 = s5 ? ? ?
                __ASM_EMIT("movss       0x60(%[src]), %%xmm5")  // xmm3 = s6 ? ? ?
                __ASM_EMIT("movss       0x70(%[src]), %%xmm3")  // xmm5 = s7 ? ? ?
                __ASM_EMIT("movlhps     %%xmm2, %%xmm1")        // xmm1 = s4 ? s5 ?
                __ASM_EMIT("movlhps     %%xmm3, %%xmm5")        // xmm4 = s6 ? s7 ?
                __ASM_EMIT("shufps      $0x88, %%xmm4, %%xmm0") // xmm0 = s0 s1 s2 s3

                __ASM_EMIT("movss       0x80(%[src]), %%xmm2")  // xmm2 = s8 ? ? ?
                __ASM_EMIT("movss       0x90(%[src]), %%xmm3")  // xmm3 = s9 ? ? ?
                __ASM_EMIT("movss       0xa0(%[src]), %%xmm6")  // xmm6 = s10 ? ? ?
                __ASM_EMIT("movss       0xb0(%[src]), %%xmm7")  // xmm7 = s11 ? ? ?
                __ASM_EMIT("movlhps     %%xmm3, %%xmm2")        // xmm2 = s8 ? s9 ?
                __ASM_EMIT("movlhps     %%xmm7, %%xmm6")        // xmm6 = s10 ? s11 ?
                __ASM_EMIT("shufps      $0x88, %%xmm5, %%xmm1") // xmm1 = s4 s5 s6 s7

                __ASM_EMIT("movss       0xc0(%[src]), %%xmm3")  // xmm3 = s12 ? ? ?
                __ASM_EMIT("movss       0xd0(%[src]), %%xmm4")  // xmm4 = s13 ? ? ?
                __ASM_EMIT("movss       0xe0(%[src]), %%xmm7")  // xmm7 = s14 ? ? ?
                __ASM_EMIT("movss       0xf0(%[src]), %%xmm5")  // xmm5 = s15 ? ? ?
                __ASM_EMIT("movlhps     %%xmm4, %%xmm3")        // xmm3 = s12 ? s13 ?
                __ASM_EMIT("movlhps     %%xmm5, %%xmm7")        // xmm7 = s14 ? s15 ?
                __ASM_EMIT("shufps      $0x88, %%xmm6, %%xmm2") // xmm2 = s8 s9 s10 s11
                __ASM_EMIT("shufps      $0x88, %%xmm7, %%xmm3") // xmm3 = s12 s13 s14 s15

                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("movups      %%xmm1, 0x10(%[dst])")
                __ASM_EMIT("movups      %%xmm2, 0x20(%[dst])")
                __ASM_EMIT("movups      %%xmm3, 0x30(%[dst])")

                // Repeat loop
                __ASM_EMIT("sub         $16, %[count]")
                __ASM_EMIT("add         $0x100, %[src]")
                __ASM_EMIT("add         $0x40, %[dst]")
                __ASM_EMIT("cmp         $16, %[count]")
                __ASM_EMIT("jae         1b")

                // 4x iterations
                __ASM_EMIT("2:")
                __ASM_EMIT("cmp         $4, %[count]")
                __ASM_EMIT("jb          4f")

                __ASM_EMIT("3:")
                __ASM_EMIT("movss       0x00(%[src]), %%xmm0")  // xmm0 = s0 ? ? ?
                __ASM_EMIT("movss       0x10(%[src]), %%xmm2")  // xmm2 = s1 ? ? ?
                __ASM_EMIT("movss       0x20(%[src]), %%xmm4")  // xmm4 = s2 ? ? ?
                __ASM_EMIT("movss       0x30(%[src]), %%xmm3")  // xmm3 = s3 ? ? ?
                __ASM_EMIT("movlhps     %%xmm2, %%xmm0")        // xmm0 = s0 ? s1 ?
                __ASM_EMIT("movlhps     %%xmm3, %%xmm4")        // xmm4 = s2 ? s3 ?
                __ASM_EMIT("shufps      $0x88, %%xmm4, %%xmm0") // xmm0 = s0 s1 s2 s3
                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst])")

                __ASM_EMIT("sub         $4, %[count]")
                __ASM_EMIT("add         $0x40, %[src]")
                __ASM_EMIT("add         $0x10, %[dst]")
                __ASM_EMIT("cmp         $4, %[count]")
                __ASM_EMIT("jae         3b")

                // 1x iterations
                __ASM_EMIT("4:")
                __ASM_EMIT("test        %[count], %[count]")
                __ASM_EMIT("jz          6f")

                __ASM_EMIT("5:")
                __ASM_EMIT("movss       0x00(%[src]), %%xmm0")  // xmm0 = s0
                __ASM_EMIT("movss       %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add         $0x10, %[src]")
                __ASM_EMIT("add         $0x4, %[dst]")
                __ASM_EMIT("dec         %[count]")
                __ASM_EMIT("jnz         5b")

                // End of routine
                __ASM_EMIT("6:")

                : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
                :
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }
    }
}

#endif /* CORE_X86_SSE_RESAMPLING_H_ */
