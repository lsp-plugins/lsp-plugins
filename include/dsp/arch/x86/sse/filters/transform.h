/*
 * transform.h
 *
 *  Created on: 20 февр. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_SSE_FILTERS_TRANSFORM_H_
#define DSP_ARCH_X86_SSE_FILTERS_TRANSFORM_H_

// Input:
//  x0 = b0[0] b1[0] b2[0] ?
//  x1 = b0[1] b1[1] b2[1] ?
//  x2 = b0[2] b1[2] b2[2] ?
//  x3 = b0[3] b1[3] b2[3] ?
// Output:
//  x0 = b0[0] b0[1] b0[2] b0[3]
//  x2 = b1[0] b1[1] b1[2] b1[3]
//  x4 = b2[0] b2[1] b2[2] b2[3]
#define FIL_TRANSPOSE(x0, x1, x2, x3, x4, x5) \
        __ASM_EMIT("movaps      %" x0 ", %" x4 )                /* x4 = b0[0] b1[0] b2[0]   ?   */ \
        __ASM_EMIT("movaps      %" x2 ", %" x5 )                /* x5 = b0[2] b1[2] b2[2]   ?   */ \
        __ASM_EMIT("unpcklps    %" x1 ", %" x0 )                /* x0 = b0[0] b0[1] b1[0] b1[1] */ \
        __ASM_EMIT("unpcklps    %" x3 ", %" x2 )                /* x2 = b0[2] b0[3] b1[2] b1[3] */ \
        __ASM_EMIT("unpckhps    %" x1 ", %" x4 )                /* x4 = b2[0] b2[1]   ?     ?   */ \
        __ASM_EMIT("unpckhps    %" x3 ", %" x5 )                /* x5 = b2[2] b2[3]   ?     ?   */ \
        __ASM_EMIT("movaps      %" x0 ", %" x1 )                /* x1 = b0[0] b0[1] b1[0] b1[1] */ \
        __ASM_EMIT("movlhps     %" x5 ", %" x4 )                /* x4 = b2[0] b2[1] b2[2] b2[3] */ \
        __ASM_EMIT("movlhps     %" x2 ", %" x0 )                /* x0 = b0[0] b0[1] b0[2] b0[3] */ \
        __ASM_EMIT("movhlps     %" x1 ", %" x2 )                /* x2 = b1[0] b1[1] b1[2] b1[3] */

// Input:
//  x0 = kf kf kf kf
//  x1 = kf2 kf2 kf2 kf2
//  x2 = b0[0] b0[1] b0[2] b0[3]
//  x4 = b1[0] b1[1] b1[2] b1[3]
//  x6 = b2[0] b2[1] b2[2] b2[3]
//  ONE= 1 1 1 1
// Output:
//  x3 = N[0] N[1] N[2] N[3]
//  x4 = fb1[0] fb1[1] fb1[2] fb1[3]
//  x5 = fb0[0] fb0[1] fb0[2] fb0[3]
#define FIL_BILINEAR_X4_BOTTOM(x0, x1, x2, x3, x4, x5, x6, x7, ONE) \
        /* x2 = b0[0..3], x4=b1[0..3], x6=b2[0..4] */ \
        __ASM_EMIT("movaps      %" x2 ", %" x7 )                /* x7 = B0[k] = b0[k] */ \
        __ASM_EMIT("mulps       %" x0 ", %" x4 )                /* x4 = B1[k] = b1[k]*kf */ \
        __ASM_EMIT("mulps       %" x1 ", %" x6 )                /* x6 = B2[k] = b2[k]*kf2 */ \
        __ASM_EMIT("addps       %" x4 ", %" x7 )                /* x7 = B0[k] + B1[k] */ \
        __ASM_EMIT("movaps      %" ONE ", %" x3 )               /* x3 = 1 */ \
        __ASM_EMIT("addps       %" x6 ", %" x7 )                /* x7 = B0[k] + B1[k] + B2[k] */ \
        __ASM_EMIT("divps       %" x7 ", %" x3 )                /* x3 = 1 / (B0[k] + B1[k] + B2[k]) */ \
        /* x2=B0[k], x3=N[k], x4=B1[k], x6=B2[k] */ \
        __ASM_EMIT("movaps      %" x6 ", %" x5 )                /* x5 = B2[k] */ \
        __ASM_EMIT("addps       %" x2 ", %" x6 )                /* x6 = B2[k] + B0[k] */ \
        __ASM_EMIT("subps       %" x2 ", %" x5 )                /* x5 = B2[k] - B0[k] */ \
        __ASM_EMIT("subps       %" x6 ", %" x4 )                /* x4 = B1[k] - (B2[k] + B0[k]) = B1[k] - B2[k] - B0[k] */ \
        __ASM_EMIT("addps       %" x5 ", %" x5 )                /* x5 = 2 * (B2[k] - B0[k]) */ \
        __ASM_EMIT("mulps       %" x3 ", %" x4 )                /* x4 = fb1[k] = (B1[k] - B2[k] - B0[k]) * N[k] */ \
        __ASM_EMIT("mulps       %" x3 ", %" x5 )                /* x5 = fb0[k] = 2 * (B2[k] - B0[k]) * N[k] */

// Input:
//  x0 = kf kf kf kf
//  x1 = kf2 kf2 kf2 kf2
//  x2 = a0[0] a0[1] a0[2] a0[3]
//  x4 = a1[0] a1[1] a1[2] a1[3]
//  x6 = a2[0] a2[1] a2[2] a2[3]
//  x7 = N[0] N[1] N[2] N[3]
// Output:
//  x2 = fa3[0] fa2[1] fa3[2] fa3[3]
//  x3 = fa2[0] fa2[1] fa2[2] fa2[3]
//  x4 = fa0[0] fa0[1] fa0[2] fa0[3]
#define FIL_BILINEAR_X4_TOP(x0, x1, x2, x3, x4, x6, x7) \
        __ASM_EMIT("mulps       %" x0 ", %" x4 )                /* x4 = T1[k] = a1[k]*kf */ \
        __ASM_EMIT("mulps       %" x1 ", %" x6 )                /* x6 = T2[k] = a2[k]*kf2 */ \
        __ASM_EMIT("movaps      %" x2 ", %" x3 )                /* x3 = T0[k] */ \
        __ASM_EMIT("addps       %" x6 ", %" x2 )                /* x2 = T0[k] + T2[k] */ \
        __ASM_EMIT("subps       %" x6 ", %" x3 )                /* x3 = T0[k] - T2[k] */ \
        __ASM_EMIT("movaps      %" x4 ", %" x6 )                /* x6 = T1[k] */ \
        __ASM_EMIT("addps       %" x3 ", %" x3 )                /* x3 = 2*(T0[k] - T2[k]) */ \
        __ASM_EMIT("addps       %" x2 ", %" x4 )                /* x4 = T0[k] + T1[k] + T2[k] */ \
        __ASM_EMIT("mulps       %" x7 ", %" x3 )                /* x3 = fa2[k] = 2*(T0[k] - T2[k]) * N */ \
        __ASM_EMIT("subps       %" x6 ", %" x2 )                /* x2 = T0[k] - T1[k] + T2[k] */ \
        __ASM_EMIT("mulps       %" x7 ", %" x4 )                /* x4 = fa0[k] = (T0[k] + T1[k] + T2[k]) * N */ \
        __ASM_EMIT("mulps       %" x7 ", %" x2 )                /* x2 = fa3[k] = (T0[k] - T1[k] + T2[k]) * N */

//    if (count <= 0)
//        return;
//
//    float T[4], B[4], N;
//    float kf2       = kf * kf;
//
//    while (count--)
//    {
//        // Calculate bottom coefficients
//        B[0]            = bc->b[0];
//        B[1]            = bc->b[1]*kf;
//        B[2]            = bc->b[2]*kf2;
//
//        // Calculate the convolution
//        N               = 1.0 / (B[0] + B[1] + B[2]);
//
//        bf->b1          = 2.0 * (B[2] - B[0]) * N;  // Sign negated
//        bf->b2          = (B[1] - B[2] - B[0]) * N; // Sign negated
//
//        // Calculate top coefficients
//        T[0]            = bc->t[0];
//        T[1]            = bc->t[1]*kf;
//        T[2]            = bc->t[2]*kf2;
//
//        bf->a0          = (T[0] + T[1] + T[2]) * N;
//        bf->a1          = 2.0 * (T[0] - T[2]) * N;
//        bf->a2          = (T[0] - T[1] + T[2]) * N;
//        bf->p0          = 0
//        bf->p1          = 0
//        bf->p2          = 0
//
//        // Increment pointers
//        bc              ++;
//        bf              ++;
//    }

namespace sse
{
    void bilinear_transform_x1(biquad_x1_t *bf, const f_cascade_t *bc, float kf, size_t count)
    {
        float x1, x2, x3, x4, x5, x6, x7;
        float DATA[12] __lsp_aligned16;

        ARCH_X86_ASM
        (
            __ASM_EMIT("test        %[count], %[count]")
            __ASM_EMIT("jz          100f")

            // Initialize
            __ASM_EMIT("shufps      $0x00, %[x0], %[x0]")       // x0 = kf kf kf kf
            __ASM_EMIT("movaps      %[x0], %[x1]")              // x1 = kf kf kf kf
            __ASM_EMIT("mulps       %[x1], %[x1]")              // x1 = kf*kf kf*kf kf*kf kf*kf = kf2 kf2 kf2 kf2

            // Perform x4 blocks
            __ASM_EMIT("sub         $4, %[count]")
            __ASM_EMIT("jb          2f")

            // Load bottom part of cascade and transpose
            __ASM_EMIT("1:")
            __ASM_EMIT("movaps      0x10(%[bc]), %[x2]")        // x2 = b0[0] b1[0] b2[0] ?
            __ASM_EMIT("movaps      0x30(%[bc]), %[x3]")        // x3 = b0[1] b1[1] b2[1] ?
            __ASM_EMIT("movaps      0x50(%[bc]), %[x4]")        // x4 = b0[2] b1[2] b2[2] ?
            __ASM_EMIT("movaps      0x70(%[bc]), %[x5]")        // x5 = b0[3] b1[3] b2[3] ?

            FIL_TRANSPOSE("[x2]", "[x3]", "[x4]", "[x5]", "[x6]", "[x7]")

            // x2 = b0[0..3], x4=b1[0..3], x6=b2[0..4]
            FIL_BILINEAR_X4_BOTTOM("[x0]", "[x1]", "[x2]", "[x3]", "[x4]", "[x5]", "[x6]", "[x7]", "[ONE]")

            // Now we need to transpose it back
            // x3 = N[k]
            // x4 = fb1[0] fb1[1] fb1[2] fb1[3]
            // x5 = fb0[0] fb0[1] fb0[2] fb0[3]
            __ASM_EMIT("movaps      %[x3], 0x00 + %[DATA]")     // store N
            __ASM_EMIT("movaps      %[x5], 0x10 + %[DATA]")     // store B0
            __ASM_EMIT("movaps      %[x4], 0x20 + %[DATA]")     // store B1

            // Load Top part of cascade and transpose
            __ASM_EMIT("movaps      0x00(%[bc]), %[x2]")        // x2 = a0[0] a1[0] a2[0] ?
            __ASM_EMIT("movaps      0x20(%[bc]), %[x3]")        // x3 = a0[1] a1[1] a2[1] ?
            __ASM_EMIT("movaps      0x40(%[bc]), %[x4]")        // x4 = a0[2] a1[2] a2[2] ?
            __ASM_EMIT("movaps      0x60(%[bc]), %[x5]")        // x5 = a0[3] a1[3] a2[3] ?

            FIL_TRANSPOSE("[x2]", "[x3]", "[x4]", "[x5]", "[x6]", "[x7]")

            // x2 = T0[k] = a0[0..3], x4=a1[0..3], x6=a2[0..4]
            __ASM_EMIT("movaps      0x00 + %[DATA], %[x7]")     // load N
            FIL_BILINEAR_X4_TOP("[x0]", "[x1]", "[x2]", "[x3]", "[x4]", "[x6]", "[x7]")

            // Now we need to transpose data and store filter
            // x2 = fa2[k], x3 = fa1[k], x4 = fa0[k]
            // x4 = a0 a1 a2 a3
            // x3 = b0 b1 b2 b3
            // x2 = c0 c1 c2 c3
            __ASM_EMIT("movaps      0x10 + %[DATA], %[x7]")     // x7 = d0 d1 d2 d3
            __ASM_EMIT("movaps      %[x4], %[x5]")              // x5 = a0 a1 a2 a3
            __ASM_EMIT("movaps      %[x3], %[x6]")              // x6 = b0 b1 b2 b3
            __ASM_EMIT("unpckhps    %[x2], %[x5]")              // x5 = a2 c2 a3 c3
            __ASM_EMIT("unpckhps    %[x7], %[x6]")              // x6 = b2 d2 b3 d3
            __ASM_EMIT("unpcklps    %[x2], %[x4]")              // x4 = a0 c0 a1 c1
            __ASM_EMIT("unpcklps    %[x7], %[x3]")              // x3 = b0 d0 b1 d1
            __ASM_EMIT("movaps      %[x4], %[x2]")              // x2 = a0 c0 a1 c1
            __ASM_EMIT("movaps      %[x5], %[x7]")              // x7 = a2 c2 a3 c3
            __ASM_EMIT("unpcklps    %[x3], %[x4]")              // x4 = a0 b0 c0 d0
            __ASM_EMIT("unpcklps    %[x6], %[x5]")              // x5 = a2 b2 c2 d2
            __ASM_EMIT("unpckhps    %[x3], %[x2]")              // x2 = a1 b1 c1 d1
            __ASM_EMIT("unpckhps    %[x6], %[x7]")              // x7 = a3 b3 c3 d3
            __ASM_EMIT("movaps      %[x4], 0x00(%[bf])")
            __ASM_EMIT("movaps      %[x2], 0x20(%[bf])")
            __ASM_EMIT("movaps      %[x5], 0x40(%[bf])")
            __ASM_EMIT("movaps      %[x7], 0x60(%[bf])")
            __ASM_EMIT("movss       0x20 + %[DATA], %%xmm2")    // x2 = b1[0] 0 0 0
            __ASM_EMIT("movss       0x24 + %[DATA], %%xmm3")    // x3 = b1[1] 0 0 0
            __ASM_EMIT("movss       0x28 + %[DATA], %%xmm4")    // x4 = b1[2] 0 0 0
            __ASM_EMIT("movss       0x2c + %[DATA], %%xmm5")    // x5 = b1[3] 0 0 0
            __ASM_EMIT("movaps      %%xmm2, 0x10(%[bf])")
            __ASM_EMIT("movaps      %%xmm3, 0x30(%[bf])")
            __ASM_EMIT("movaps      %%xmm4, 0x50(%[bf])")
            __ASM_EMIT("movaps      %%xmm5, 0x70(%[bf])")

            // Update pointers and repeat loop
            __ASM_EMIT("add         $0x80, %[bc]")
            __ASM_EMIT("add         $0x80, %[bf]")
            __ASM_EMIT("sub         $4, %[count]")
            __ASM_EMIT("jae         1b")

            // Single-item loop
            __ASM_EMIT("2:")
            __ASM_EMIT("add         $4, %[count]")
            __ASM_EMIT("jz          100f")

            // Calculate bottom part
            __ASM_EMIT("3:")
            __ASM_EMIT("movss       0x10(%[bc]), %[x2]")        // x2 = B0 = b0
            __ASM_EMIT("movss       0x14(%[bc]), %[x3]")        // x3 = b1
            __ASM_EMIT("movss       0x18(%[bc]), %[x4]")        // x4 = b2
            __ASM_EMIT("mulss       %[x0], %[x3]")              // x3 = B1 = b1 * kf
            __ASM_EMIT("mulss       %[x1], %[x4]")              // x4 = B2 = b2 * kf2
            __ASM_EMIT("movss       %[x3], %[x5]")              // x5 = B1
            __ASM_EMIT("addss       %[x4], %[x3]")              // x3 = B1 + B2
            __ASM_EMIT("subss       %[x4], %[x5]")              // x5 = B1 - B2
            __ASM_EMIT("addss       %[x2], %[x3]")              // x3 = B0 + B1 + B2
            __ASM_EMIT("movaps      %[ONE], %[x7]")             // x7 = 1
            __ASM_EMIT("subss       %[x2], %[x5]")              // x5 = B1 - B2 - B0
            __ASM_EMIT("subss       %[x2], %[x4]")              // x4 = B2 - B0
            __ASM_EMIT("divss       %[x3], %[x7]")              // x7 = N = 1 / (B0 + B1 + B2)
            __ASM_EMIT("addss       %[x4], %[x4]")              // x4 = 2 * (B2 - B0)
            __ASM_EMIT("xorps       %[x6], %[x6]")              // x6 = 0
            __ASM_EMIT("mulss       %[x7], %[x5]")              // x5 = bf1 = (B1 - B2 - B0) * N
            __ASM_EMIT("mulss       %[x7], %[x4]")              // x4 = bf0 = 2 * (B2 - B0) * N
            __ASM_EMIT("movss       %[x5], %[x6]")              // x6 = bf1 0 0 0
            __ASM_EMIT("movss       %[x4], 0x0c(%[bf])")
            __ASM_EMIT("movaps      %[x6], 0x10(%[bf])")

            // Calculate top part
            // x7 = N
            __ASM_EMIT("movss       0x00(%[bc]), %[x2]")        // x2 = T0 = t0
            __ASM_EMIT("movss       0x04(%[bc]), %[x3]")        // x3 = t1
            __ASM_EMIT("movss       0x08(%[bc]), %[x4]")        // x4 = t2
            __ASM_EMIT("mulss       %[x0], %[x3]")              // x3 = T1 = t1 * kf
            __ASM_EMIT("mulss       %[x1], %[x4]")              // x4 = T2 = t2 * kf2
            __ASM_EMIT("movss       %[x2], %[x5]")              // x5 = T0
            __ASM_EMIT("addss       %[x4], %[x2]")              // x2 = T0 + T2
            __ASM_EMIT("subss       %[x4], %[x5]")              // x5 = T0 - T2
            __ASM_EMIT("movss       %[x2], %[x6]")              // x6 = T0 + T2
            __ASM_EMIT("addss       %[x3], %[x2]")              // x2 = T0 + T1 + T2
            __ASM_EMIT("addss       %[x5], %[x5]")              // x5 = 2 * (T0 - T2)
            __ASM_EMIT("subss       %[x3], %[x6]")              // x6 = T0 - T1 + T2
            __ASM_EMIT("mulss       %[x7], %[x2]")              // x2 = ba[0] = (T0 + T1 + T2) * N
            __ASM_EMIT("mulss       %[x7], %[x5]")              // x5 = ba[1] = 2 * (T0 - T2) * N
            __ASM_EMIT("mulss       %[x7], %[x6]")              // x6 = ba[2] = (T0 - T1 + T2) * N
            __ASM_EMIT("movss       %[x2], 0x00(%[bf])")
            __ASM_EMIT("movss       %[x5], 0x04(%[bf])")
            __ASM_EMIT("movss       %[x6], 0x08(%[bf])")

            // Repeat loop
            __ASM_EMIT("add         $0x20, %[bc]")
            __ASM_EMIT("add         $0x20, %[bf]")
            __ASM_EMIT("dec         %[count]")
            __ASM_EMIT("jnz         3b")

            __ASM_EMIT("100:")
            : [x0] "+x" (kf), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3),
              [x4] "=&x" (x4), [x5] "=&x" (x5), [x6] "=&x" (x6), [x7] "=&x" (x7),
              [count] "+r" (count),
              [bc] "+r" (bc),
              [bf] "+r" (bf)
            : [ONE] "m" (ONE),
              [DATA] "o" (DATA)
            : "cc", "memory"
        );
    }

    void bilinear_transform_x2(biquad_x2_t *bf, const f_cascade_t *bc, float kf, size_t count)
    {
        float x1, x2, x3, x4, x5, x6, x7;
        float N[4] __lsp_aligned16;

        // Same ad bilinear_tranform_x1 but number of cycles is doubled
        ARCH_X86_ASM
        (
            __ASM_EMIT("test        %[count], %[count]")
            __ASM_EMIT("jz          100f")

            // Initialize
            __ASM_EMIT("shufps      $0x00, %[x0], %[x0]")       // x0 = kf kf kf kf
            __ASM_EMIT("movaps      %[x0], %[x1]")              // x1 = kf kf kf kf
            __ASM_EMIT("mulps       %[x1], %[x1]")              // x1 = kf*kf kf*kf kf*kf kf*kf = kf2 kf2 kf2 kf2
            __ASM_EMIT("sub         $2, %[count]")
            __ASM_EMIT("jb          2f")

            //---------------------------------------------------------------------
            // Load bottom part of cascade and transpose
            __ASM_EMIT("1:")
            __ASM_EMIT("movaps      0x10(%[bc]), %[x2]")        // x2 = b0[0] b1[0] b2[0] ?
            __ASM_EMIT("movaps      0x30(%[bc]), %[x3]")        // x3 = b0[1] b1[1] b2[1] ?
            __ASM_EMIT("movaps      0x50(%[bc]), %[x4]")        // x4 = b0[2] b1[2] b2[2] ?
            __ASM_EMIT("movaps      0x70(%[bc]), %[x5]")        // x5 = b0[3] b1[3] b2[3] ?

            FIL_TRANSPOSE("[x2]", "[x3]", "[x4]", "[x5]", "[x6]", "[x7]")

            // x2 = b0[0..3], x4=b1[0..3], x6=b2[0..4]
            FIL_BILINEAR_X4_BOTTOM("[x0]", "[x1]", "[x2]", "[x3]", "[x4]", "[x5]", "[x6]", "[x7]", "[ONE]")

            // Now we need to transpose it back
            // x3 = N[k], x4 = fb1[k], x5 = fb0[k]
            // x5 = a0 a1 a2 a3
            // x4 = b0 b1 b2 b3
            __ASM_EMIT("movaps      %[x3], %[N]")               // store N
            __ASM_EMIT("xorps       %[x2], %[x2]")              // x2 = 0 0 0 0
            __ASM_EMIT("movlps      %[x5], 0x18(%[bf])")
            __ASM_EMIT("xorps       %[x7], %[x7]")              // x7 = 0 0 0 0
            __ASM_EMIT("movhlps     %[x4], %[x2]")              // x2 = b2 b3 0 0
            __ASM_EMIT("movhps      %[x5], 0x48(%[bf])")
            __ASM_EMIT("movlhps     %[x7], %[x4]")              // x4 = b0 b1 0 0
            __ASM_EMIT("movaps      %[x4], 0x20(%[bf])")
            __ASM_EMIT("movaps      %[x2], 0x50(%[bf])")

            // Load Top part of cascade and transpose
            __ASM_EMIT("movaps      0x00(%[bc]), %[x2]")        // x2 = a0[0] a1[0] a2[0] ?
            __ASM_EMIT("movaps      0x20(%[bc]), %[x3]")        // x3 = a0[1] a1[1] a2[1] ?
            __ASM_EMIT("movaps      0x40(%[bc]), %[x4]")        // x4 = a0[2] a1[2] a2[2] ?
            __ASM_EMIT("movaps      0x60(%[bc]), %[x5]")        // x5 = a0[3] a1[3] a2[3] ?

            FIL_TRANSPOSE("[x2]", "[x3]", "[x4]", "[x5]", "[x6]", "[x7]")

            // x2 = T0[k] = a0[0..3], x4=a1[0..3], x6=a2[0..4]
            __ASM_EMIT("movaps      %[N], %[x7]")               // load N
            FIL_BILINEAR_X4_TOP("[x0]", "[x1]", "[x2]", "[x3]", "[x4]", "[x6]", "[x7]")

            // Now we need to store top part
            // x2 = fa2[k], x3 = fa1[k], x4 = fa0[k]
            // x4 = a0 a1 a2 a3
            // x3 = b0 b1 b2 b3
            // x2 = c0 c1 c2 c3
            __ASM_EMIT("movaps      %[x4], %[x5]")              // x5 = a0 a1 a2 a3
            __ASM_EMIT("movlhps     %[x3], %[x4]")              // x4 = a0 a1 b0 b1
            __ASM_EMIT("movhlps     %[x5], %[x3]")              // x3 = a2 a3 b2 b3

            __ASM_EMIT("movaps      %[x4], 0x00(%[bf])")
            __ASM_EMIT("movlps      %[x2], 0x10(%[bf])")
            __ASM_EMIT("movaps      %[x3], 0x30(%[bf])")
            __ASM_EMIT("movhps      %[x2], 0x40(%[bf])")

            // Update pointers and repeat loop
            __ASM_EMIT("add         $0x80, %[bc]")
            __ASM_EMIT("add         $0x60, %[bf]")
            __ASM_EMIT("sub         $2, %[count]")
            __ASM_EMIT("jae         1b")

            // Do we have a tail?
            __ASM_EMIT("2:")
            __ASM_EMIT("test        $1, %[count]")
            __ASM_EMIT("jz          100f")

            //---------------------------------------------------------------------
            // Perform x2 block
            // Load data
            __ASM_EMIT("movaps      0x10(%[bc]), %[x2]")        // x2 = b0[0] b1[0] b2[0] ?
            __ASM_EMIT("movaps      0x30(%[bc]), %[x3]")        // x3 = b0[1] b1[1] b2[1] ?
            __ASM_EMIT("movaps      %[x2], %[x6]")              // x6 = b0[0] b1[0] b2[0] ?
            __ASM_EMIT("unpcklps    %[x3], %[x2]")              // x2 = b0[0] b0[1] b1[0] b1[1]
            __ASM_EMIT("unpckhps    %[x3], %[x6]")              // x6 = b2[0] b2[1] ? ?
            __ASM_EMIT("movhlps     %[x2], %[x4]")              // x4 = b1[0] b1[1] ? ?

            // x2 = b0[0..1], x4=b1[0..1], x6=b2[0..1]
            FIL_BILINEAR_X4_BOTTOM("[x0]", "[x1]", "[x2]", "[x3]", "[x4]", "[x5]", "[x6]", "[x7]", "[ONE]")

            // Store computed data
            // x3 = N[0] N[1]
            // x4 = fb1[0] fb1[1]
            // x5 = fb0[0] fb0[1]
            __ASM_EMIT("xorps       %[x6], %[x6]")              // x6 = 0
            __ASM_EMIT("movlhps     %[x4], %[x5]")              // x5 = fb0[0] fb0[1] fb1[0] fb1[1]
            __ASM_EMIT("movaps      %[x3], %[x7]")              // x7 = N
            __ASM_EMIT("movups      %[x5], 0x18(%[bf])")
            __ASM_EMIT("movlps      %[x6], 0x28(%[bf])")

            // Calculate top part
            __ASM_EMIT("movaps      0x00(%[bc]), %[x2]")        // x2 = a0[0] a1[0] a2[0] ?
            __ASM_EMIT("movaps      0x20(%[bc]), %[x3]")        // x3 = a0[1] a1[1] a2[1] ?
            __ASM_EMIT("movaps      %[x2], %[x6]")              // x6 = a0[1] a1[1] a2[1] ?
            __ASM_EMIT("unpcklps    %[x3], %[x2]")              // x2 = a0[0] a0[1] a1[0] a1[1]
            __ASM_EMIT("unpckhps    %[x3], %[x6]")              // x6 = a2[0] a2[1] ? ?
            __ASM_EMIT("movhlps     %[x2], %[x4]")              // x4 = a1[0] a1[1] ? ?

            // x2 = a0[0..3], x4=a1[0..3], x6=a2[0..4], x7=N
            FIL_BILINEAR_X4_TOP("[x0]", "[x1]", "[x2]", "[x3]", "[x4]", "[x6]", "[x7]")

            // Now we need to store data
            // x2 = fa2[k], x3 = fa1[k], x4 = fa0[k]
            __ASM_EMIT("movlhps     %[x3], %[x4]")
            __ASM_EMIT("movaps      %[x4], 0x00(%[bf])")
            __ASM_EMIT("movlps      %[x2], 0x10(%[bf])")

            __ASM_EMIT("100:")
            : [x0] "+x" (kf), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3),
              [x4] "=&x" (x4), [x5] "=&x" (x5), [x6] "=&x" (x6), [x7] "=&x" (x7),
              [count] "+r" (count),
              [bc] "+r" (bc),
              [bf] "+r" (bf)
            : [ONE] "m" (ONE),
              [N] "m" (N)
            : "cc", "memory"
        );
    }

    void bilinear_transform_x4(biquad_x4_t *bf, const f_cascade_t *bc, float kf, size_t count)
    {
        float x1, x2, x3, x4, x5, x6, x7;
        float N[4] __lsp_aligned16;

        // Same ad bilinear_tranform_x1 but number of cycles is doubled
        ARCH_X86_ASM
        (
            __ASM_EMIT("test        %[count], %[count]")
            __ASM_EMIT("jz          100f")

            // Initialize
            __ASM_EMIT("shufps      $0x00, %[x0], %[x0]")       // x0 = kf kf kf kf
            __ASM_EMIT("movaps      %[x0], %[x1]")              // x1 = kf kf kf kf
            __ASM_EMIT("mulps       %[x1], %[x1]")              // x1 = kf*kf kf*kf kf*kf kf*kf = kf2 kf2 kf2 kf2

            //---------------------------------------------------------------------
            // Load bottom part of cascade and transpose
            __ASM_EMIT("1:")
            __ASM_EMIT("movaps      0x10(%[bc]), %[x2]")        // x2 = b0[0] b1[0] b2[0] ?
            __ASM_EMIT("movaps      0x30(%[bc]), %[x3]")        // x3 = b0[1] b1[1] b2[1] ?
            __ASM_EMIT("movaps      0x50(%[bc]), %[x4]")        // x4 = b0[2] b1[2] b2[2] ?
            __ASM_EMIT("movaps      0x70(%[bc]), %[x5]")        // x5 = b0[3] b1[3] b2[3] ?

            FIL_TRANSPOSE("[x2]", "[x3]", "[x4]", "[x5]", "[x6]", "[x7]")

            // x2 = b0[0..3], x4=b1[0..3], x6=b2[0..4]
            FIL_BILINEAR_X4_BOTTOM("[x0]", "[x1]", "[x2]", "[x3]", "[x4]", "[x5]", "[x6]", "[x7]", "[ONE]")

            // Now we need to transpose it back
            // x3 = N[k]
            // x4 = fb1[0] fb1[1] fb1[2] fb1[3]
            // x5 = fb0[0] fb0[1] fb0[2] fb0[3]
            // Store bottom part of filters
            __ASM_EMIT("movaps      %[x3], %[N]")
            __ASM_EMIT("movaps      %[x5], 0x30(%[bf])")
            __ASM_EMIT("movaps      %[x4], 0x40(%[bf])")

            // Load Top part of cascade and transpose
            __ASM_EMIT("movaps      0x00(%[bc]), %[x2]")        // x2 = a0[0] a1[0] a2[0] ?
            __ASM_EMIT("movaps      0x20(%[bc]), %[x3]")        // x3 = a0[1] a1[1] a2[1] ?
            __ASM_EMIT("movaps      0x40(%[bc]), %[x4]")        // x4 = a0[2] a1[2] a2[2] ?
            __ASM_EMIT("movaps      0x60(%[bc]), %[x5]")        // x5 = a0[3] a1[3] a2[3] ?

            FIL_TRANSPOSE("[x2]", "[x3]", "[x4]", "[x5]", "[x6]", "[x7]")

            // x2 = T0[k] = a0[0..3], x4=a1[0..3], x6=a2[0..4]
            __ASM_EMIT("movaps      %[N], %[x7]")               // load N
            FIL_BILINEAR_X4_TOP("[x0]", "[x1]", "[x2]", "[x3]", "[x4]", "[x6]", "[x7]")

            // Now we need to transpose data
            // x2 = fa3[k], x3 = fa2[k], x4 = fa0[k]
            // Store top part of filters
            __ASM_EMIT("movaps      %[x4], 0x00(%[bf])")
            __ASM_EMIT("movaps      %[x3], 0x10(%[bf])")
            __ASM_EMIT("movaps      %[x2], 0x20(%[bf])")

            // Update pointers and repeat loop
            __ASM_EMIT("add         $0x80, %[bc]")
            __ASM_EMIT("add         $0x50, %[bf]")
            __ASM_EMIT("dec         %[count]")
            __ASM_EMIT("jnz         1b")

            __ASM_EMIT("100:")
            : [x0] "+x" (kf), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3),
              [x4] "=&x" (x4), [x5] "=&x" (x5), [x6] "=&x" (x6), [x7] "=&x" (x7),
              [count] "+r" (count),
              [bc] "+r" (bc),
              [bf] "+r" (bf)
            : [ONE] "m" (ONE),
              [N] "m" (N)
            : "cc", "memory"
        );
    }

    void bilinear_transform_x8(biquad_x8_t *bf, const f_cascade_t *bc, float kf, size_t count)
    {
        float x1, x2, x3, x4, x5, x6, x7;
        float N[4] __lsp_aligned16;

        // Same ad bilinear_tranform_x1 but number of cycles is doubled
        ARCH_X86_ASM
        (
            __ASM_EMIT("test        %[count], %[count]")
            __ASM_EMIT("jz          100f")

            // Initialize
            __ASM_EMIT("shufps      $0x00, %[x0], %[x0]")       // x0 = kf kf kf kf
            __ASM_EMIT("movaps      %[x0], %[x1]")              // x1 = kf kf kf kf
            __ASM_EMIT("mulps       %[x1], %[x1]")              // x1 = kf*kf kf*kf kf*kf kf*kf = kf2 kf2 kf2 kf2

            //---------------------------------------------------------------------
            // x8 block
            __ASM_EMIT("1:")
            // Group 1
            // Load bottom part of cascade and transpose
            __ASM_EMIT("movaps      0x10(%[bc]), %[x2]")        // x2 = b0[0] b1[0] b2[0] ?
            __ASM_EMIT("movaps      0x30(%[bc]), %[x3]")        // x3 = b0[1] b1[1] b2[1] ?
            __ASM_EMIT("movaps      0x50(%[bc]), %[x4]")        // x4 = b0[2] b1[2] b2[2] ?
            __ASM_EMIT("movaps      0x70(%[bc]), %[x5]")        // x5 = b0[3] b1[3] b2[3] ?

            FIL_TRANSPOSE("[x2]", "[x3]", "[x4]", "[x5]", "[x6]", "[x7]")
            FIL_BILINEAR_X4_BOTTOM("[x0]", "[x1]", "[x2]", "[x3]", "[x4]", "[x5]", "[x6]", "[x7]", "[ONE]")

            __ASM_EMIT("movaps      %[x3], %[N]")
            __ASM_EMIT("movaps      %[x5], 0x60(%[bf])")
            __ASM_EMIT("movaps      %[x4], 0x80(%[bf])")

            // Load Top part of cascade and transpose
            __ASM_EMIT("movaps      0x00(%[bc]), %[x2]")        // x2 = a0[0] a1[0] a2[0] ?
            __ASM_EMIT("movaps      0x20(%[bc]), %[x3]")        // x3 = a0[1] a1[1] a2[1] ?
            __ASM_EMIT("movaps      0x40(%[bc]), %[x4]")        // x4 = a0[2] a1[2] a2[2] ?
            __ASM_EMIT("movaps      0x60(%[bc]), %[x5]")        // x5 = a0[3] a1[3] a2[3] ?

            FIL_TRANSPOSE("[x2]", "[x3]", "[x4]", "[x5]", "[x6]", "[x7]")
            __ASM_EMIT("movaps      %[N], %[x7]")               // load N
            FIL_BILINEAR_X4_TOP("[x0]", "[x1]", "[x2]", "[x3]", "[x4]", "[x6]", "[x7]")

            __ASM_EMIT("movaps      %[x4], 0x00(%[bf])")
            __ASM_EMIT("movaps      %[x3], 0x20(%[bf])")
            __ASM_EMIT("movaps      %[x2], 0x40(%[bf])")

            // Group 2
            // Load bottom part of cascade and transpose
            __ASM_EMIT("movaps      0x90(%[bc]), %[x2]")        // x2 = b0[0] b1[0] b2[0] ?
            __ASM_EMIT("movaps      0xb0(%[bc]), %[x3]")        // x3 = b0[1] b1[1] b2[1] ?
            __ASM_EMIT("movaps      0xd0(%[bc]), %[x4]")        // x4 = b0[2] b1[2] b2[2] ?
            __ASM_EMIT("movaps      0xf0(%[bc]), %[x5]")        // x5 = b0[3] b1[3] b2[3] ?

            FIL_TRANSPOSE("[x2]", "[x3]", "[x4]", "[x5]", "[x6]", "[x7]")
            FIL_BILINEAR_X4_BOTTOM("[x0]", "[x1]", "[x2]", "[x3]", "[x4]", "[x5]", "[x6]", "[x7]", "[ONE]")

            __ASM_EMIT("movaps      %[x3], %[N]")
            __ASM_EMIT("movaps      %[x5], 0x70(%[bf])")
            __ASM_EMIT("movaps      %[x4], 0x90(%[bf])")

            __ASM_EMIT("movaps      0x80(%[bc]), %[x2]")        // x2 = a0[0] a1[0] a2[0] ?
            __ASM_EMIT("movaps      0xa0(%[bc]), %[x3]")        // x3 = a0[1] a1[1] a2[1] ?
            __ASM_EMIT("movaps      0xc0(%[bc]), %[x4]")        // x4 = a0[2] a1[2] a2[2] ?
            __ASM_EMIT("movaps      0xe0(%[bc]), %[x5]")        // x5 = a0[3] a1[3] a2[3] ?

            FIL_TRANSPOSE("[x2]", "[x3]", "[x4]", "[x5]", "[x6]", "[x7]")
            __ASM_EMIT("movaps      %[N], %[x7]")               // load N
            FIL_BILINEAR_X4_TOP("[x0]", "[x1]", "[x2]", "[x3]", "[x4]", "[x6]", "[x7]")

            __ASM_EMIT("movaps      %[x4], 0x10(%[bf])")
            __ASM_EMIT("movaps      %[x3], 0x30(%[bf])")
            __ASM_EMIT("movaps      %[x2], 0x50(%[bf])")

            // Update pointers and repeat loop
            __ASM_EMIT("add         $0x100, %[bc]")
            __ASM_EMIT("add         $0xa0, %[bf]")
            __ASM_EMIT("dec         %[count]")
            __ASM_EMIT("jnz         1b")

            __ASM_EMIT("100:")
            : [x0] "+x" (kf), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3),
              [x4] "=&x" (x4), [x5] "=&x" (x5), [x6] "=&x" (x6), [x7] "=&x" (x7),
              [count] "+r" (count),
              [bc] "+r" (bc),
              [bf] "+r" (bf)
            : [ONE] "m" (ONE),
              [N] "o" (N)
            : "cc", "memory"
        );
    }

    #undef FIL_BILINEAR_X4_TOP
    #undef FIL_BILINEAR_X4_BOTTOM
    #undef FIL_TRANSPOSE


    static void matched_solve(float *p, float kf, float td, size_t count, size_t stride)
    {
        if (p[2] == 0.0) // Test polynom for second-order
        {
            if (p[1] == 0.0) // Test polynom for first order
            {
                while (count--)
                {
                    p[3]        = 1; // transfer function
                    p          += stride;
                }
            }
            else
            {
                // First-order polynom:
                //   p(s) = p[0] + p[1]*(s/f)
                //
                // Transformed polynom:
                //   P[z] = p[1]/f - p[1]/f * exp(-f*p[0]*T/p[1]) * z^-1
                while (count--)
                {
                    float k     = p[1]/kf;
                    float R     = -p[0]/k;
                    p[3]        = sqrtf(p[0]*p[0] + p[1]*p[1]*0.01f); // transfer function
                    p[0]        = k;
                    p[1]        = -k * expf(R*td);

                    p          += stride;
                }
            }
        }
        else
        {
            // Second-order polynom:
            //   p(s) = p[0] + p[1]*(s/f) + p[2]*(s/f)^2 = p[2]/f^2 * (p[0]*f^2/p[2] + p[1]*f/p[2]*s + s^2)
            //
            // Calculate the roots of the second-order polynom equation a*x^2 + b*x + c = 0
            float k, b, c, D;
            float a2   = 2.0f/(kf*kf);

            while (count--)
            {
                // Transfer function
                b           = p[0] - p[2]*0.01f;
                c           = p[1]*0.1f;
                p[3]        = sqrt(b*b + c*c);

                // Calculate parameters
                k           = p[2];
                b           = p[1]/(kf*p[2]);
                c           = p[0]/p[2];
                D           = b*b - 2.0f*a2*c;

                if (D >= 0.0f)
                {
                    // Has real roots R0 and R1
                    // Transformed form is:
                    //   P[z] = k*(1 - (exp(R0*T) + exp(R1*T))*z^-1 + exp((R0+R1)*T)*z^-2)
                    D           = sqrtf(D);
                    float R0    = td*(-b - D)/a2;
                    float R1    = td*(-b + D)/a2;
                    p[0]        = k;
                    p[1]        = -k * (expf(R0) + expf(R1));
                    p[2]        = k * expf(R0+R1);
                }
                else
                {
                    // Has complex roots R+j*K and R-j*K
                    // Transformed form is:
                    //   P[z] = k*(1 - 2*exp(R*T)*cos(K*T)*z^-1 + exp(2*R*T)*z^-2)
                    D           = sqrtf(-D);
                    float R     = -(td*b) /a2;
                    float K     = D /a2;
                    p[0]        = k;
                    p[1]        = -2.0 * k * expf(R) * cosf(K*td);
                    p[2]        = k * expf(R+R);
                }

                // Update pointer
                p          += stride;
            }
        }
    }

    #if 0

    void test_matched_transform_x1(biquad_x1_t *bf, f_cascade_t *bc, float kf, float td, size_t count)
    {
        // Find roots for top and bottom polynoms
        matched_solve(bc->t, kf, td, count, sizeof(f_cascade_t)/sizeof(float));
        matched_solve(bc->b, kf, td, count, sizeof(f_cascade_t)/sizeof(float));

        double w        = kf * td * 0.1;
        float cos_w     = cos(w);
        float sin_w     = sin(w);

        float COSW[4]  __lsp_aligned16;
        float SINW[4]  __lsp_aligned16;
        float COS2W[4] __lsp_aligned16;
        float SIN2W[4] __lsp_aligned16;
        float T[16] __lsp_aligned16; // T0-T3
        float B[16] __lsp_aligned16; // B0-B3
        float CT[12] __lsp_aligned16; // RE, IM, AT
        float CB[12] __lsp_aligned16; // RE, IM, AB
        float N[12] __lsp_aligned16; // AN, N2, N1

        float x2, x3, x4, x5, x6, x7;

        ARCH_X86_ASM
        (
            // Prepare
            // Calculate COSW, SINW, COS2W, SIN2W
            __ASM_EMIT("shufps      $0x00, %[x0], %[x0]")   // x0 = cos_w
            __ASM_EMIT("shufps      $0x00, %[x1], %[x1]")   // x1 = sin_w
            __ASM_EMIT("movaps      %[x0], %[COSW]")
            __ASM_EMIT("movaps      %[x1], %[SINW]")
            __ASM_EMIT("movaps      %[x0], %[x2]")          // x2 = cos_w
            __ASM_EMIT("mulps       %[x1], %[x2]")          // x2 = cos_w*sin_w
            __ASM_EMIT("mulps       %[x0], %[x0]")          // x0 = cos_w*cos_w
            __ASM_EMIT("mulps       %[x1], %[x1]")          // x1 = sin_w*sin_w
            __ASM_EMIT("addps       %[x2], %[x2]")          // x2 = sin_2w = 2*cos_w*sin_w
            __ASM_EMIT("subps       %[x1], %[x0]")          // x0 = cos_2s = cos_w*cos_w - sin_w*sin_w
            __ASM_EMIT("movaps      %[x0], %[COS2W]")
            __ASM_EMIT("movaps      %[x2], %[SIN2W]")
            : [x0] "+x" (cos_w), [x1] "+x" (sin_w), [x2] "=&x" (x2)
            : [COSW] "m" (COSW),
              [SINW] "m" (SINW),
              [COS2W] "m" (COS2W),
              [SIN2W] "m" (SIN2W)
            : "memory"
        );

        lsp_dumpf("COSW", "%f", COSW, 4);
        lsp_dumpf("SINW", "%f", SINW, 4);
        lsp_dumpf("COS2W", "%f", COS2W, 4);
        lsp_dumpf("SIN2W", "%f", SIN2W, 4);

        while (count >= 4)
        {
            ARCH_X86_ASM
            (
                // Load top part
                __ASM_EMIT("movaps      0x00(%[bc]), %[x0]")    // x0 = t0[0] t0[1] t0[2] t0[3]
                __ASM_EMIT("movaps      0x20(%[bc]), %[x1]")    // x1 = t1[0] t1[1] t1[2] t1[3]
                __ASM_EMIT("movaps      0x40(%[bc]), %[x2]")    // x2 = t2[0] t2[1] t2[2] t2[3]
                __ASM_EMIT("movaps      0x60(%[bc]), %[x3]")    // x3 = t3[0] t3[1] t3[2] t3[3]

                // Transpose top part
                __ASM_EMIT("movaps      %[x0], %[x4]")          // x4 = t0[0] t0[1] t0[2] t0[3]
                __ASM_EMIT("movaps      %[x2], %[x5]")          // x5 = t2[0] t2[1] t2[2] t2[3]
                __ASM_EMIT("unpcklps    %[x1], %[x0]")          // x0 = t0[0] t1[0] t0[1] t1[1]
                __ASM_EMIT("unpcklps    %[x3], %[x2]")          // x2 = t2[0] t3[0] t2[1] t3[1]
                __ASM_EMIT("unpckhps    %[x1], %[x4]")          // x4 = t0[2] t1[2] t0[3] t1[3]
                __ASM_EMIT("unpckhps    %[x3], %[x5]")          // x5 = t2[2] t3[2] t2[3] t3[3]
                __ASM_EMIT("movaps      %[x0], %[x1]")          // x1 = t0[0] t1[0] t0[1] t1[1]
                __ASM_EMIT("movaps      %[x4], %[x3]")          // x3 = t0[2] t1[2] t0[3] t1[3]
                __ASM_EMIT("movlhps     %[x2], %[x0]")          // x0 = t0[0] t1[0] t2[0] t3[0]
                __ASM_EMIT("movhlps     %[x1], %[x2]")          // x2 = t0[1] t1[1] t2[1] t3[1]
                __ASM_EMIT("movlhps     %[x5], %[x4]")          // x4 = t0[2] t1[2] t2[2] t3[2]
                __ASM_EMIT("movhlps     %[x3], %[x5]")          // x5 = t0[3] t1[3] t2[3] t3[3]

                // x0 = t[0], x2 = t[1], x4 = t[2], x5 = t[3]
                __ASM_EMIT("movaps      %[x0], 0x00(%[bf])")
                __ASM_EMIT("movaps      %[x2], 0x20(%[bf])")
                __ASM_EMIT("movaps      %[x4], 0x40(%[bf])")
                __ASM_EMIT("movaps      %[x0], 0x00(%[T])")     // DEBUG
                __ASM_EMIT("movaps      %[x2], 0x10(%[T])")     // DEBUG
                __ASM_EMIT("movaps      %[x4], 0x20(%[T])")     // DEBUG
                __ASM_EMIT("movaps      %[x5], 0x30(%[T])")     // DEBUG

                __ASM_EMIT("movaps      %[x0], %[x1]")          // x1 = t[0]
                __ASM_EMIT("movaps      %[x2], %[x3]")          // x3 = t[1]
                __ASM_EMIT("mulps       %[COS2W], %[x1]")       // x1 = t[0]*cos_2w
                __ASM_EMIT("mulps       %[SIN2W], %[x0]")       // x0 = t[0]*sin_2w
                __ASM_EMIT("mulps       %[COSW], %[x3]")        // x3 = t[1]*cos_w
                __ASM_EMIT("mulps       %[SINW], %[x2]")        // x2 = t[1]*sin_w
                __ASM_EMIT("addps       %[x1], %[x4]")          // x4 = t[0]*cos_2w + t[2]
                __ASM_EMIT("addps       %[x0], %[x2]")          // x2 = im = t[0]*sin_2w + t[1]*sin_w
                __ASM_EMIT("addps       %[x3], %[x4]")          // x4 = re = t[0]*cos_2w + t[1]*cos_w + t[2]
                __ASM_EMIT("movaps      %[x4], 0x00(%[CT])")    // DEBUG
                __ASM_EMIT("movaps      %[x2], 0x10(%[CT])")    // DEBUG
                __ASM_EMIT("mulps       %[x2], %[x2]")          // x2 = im*im
                __ASM_EMIT("mulps       %[x4], %[x4]")          // x4 = re*re
                __ASM_EMIT("addps       %[x2], %[x4]")          // x4 = re*re + im*im
                __ASM_EMIT("movaps      %[x5], %[x7]")          // x7 = t[3]
                __ASM_EMIT("sqrtps      %[x4], %[x6]")          // x6 = at = sqrt(re*re + im*im)
                __ASM_EMIT("movaps      %[x6], 0x20(%[CT])")    // DEBUG

                // Load bottom part
                __ASM_EMIT("movaps      0x10(%[bc]), %[x0]")    // x0 = t0[0] t0[1] t0[2] t0[3]
                __ASM_EMIT("movaps      0x30(%[bc]), %[x1]")    // x1 = t1[0] t1[1] t1[2] t1[3]
                __ASM_EMIT("movaps      0x50(%[bc]), %[x2]")    // x2 = t2[0] t2[1] t2[2] t2[3]
                __ASM_EMIT("movaps      0x70(%[bc]), %[x3]")    // x3 = t3[0] t3[1] t3[2] t3[3]

                // Transpose bottom part
                __ASM_EMIT("movaps      %[x0], %[x4]")          // x4 = t0[0] t0[1] t0[2] t0[3]
                __ASM_EMIT("movaps      %[x2], %[x5]")          // x5 = t2[0] t2[1] t2[2] t2[3]
                __ASM_EMIT("unpcklps    %[x1], %[x0]")          // x0 = t0[0] t1[0] t0[1] t1[1]
                __ASM_EMIT("unpcklps    %[x3], %[x2]")          // x2 = t2[0] t3[0] t2[1] t3[1]
                __ASM_EMIT("unpckhps    %[x1], %[x4]")          // x4 = t0[2] t1[2] t0[3] t1[3]
                __ASM_EMIT("unpckhps    %[x3], %[x5]")          // x5 = t2[2] t3[2] t2[3] t3[3]
                __ASM_EMIT("movaps      %[x0], %[x1]")          // x1 = t0[0] t1[0] t0[1] t1[1]
                __ASM_EMIT("movaps      %[x4], %[x3]")          // x3 = t0[2] t1[2] t0[3] t1[3]
                __ASM_EMIT("movlhps     %[x2], %[x0]")          // x0 = t0[0] t1[0] t2[0] t3[0]
                __ASM_EMIT("movhlps     %[x1], %[x2]")          // x2 = t0[1] t1[1] t2[1] t3[1]
                __ASM_EMIT("movlhps     %[x5], %[x4]")          // x4 = t0[2] t1[2] t2[2] t3[2]
                __ASM_EMIT("movhlps     %[x3], %[x5]")          // x5 = t0[3] t1[3] t2[3] t3[3]

                __ASM_EMIT("movaps      %[x0], 0x00(%[B])")     // DEBUG
                __ASM_EMIT("movaps      %[x2], 0x10(%[B])")     // DEBUG
                __ASM_EMIT("movaps      %[x4], 0x20(%[B])")     // DEBUG
                __ASM_EMIT("movaps      %[x5], 0x30(%[B])")     // DEBUG

                // x0 = b[0], x2 = b[1], x4 = b[2], x5 = b[3]
                __ASM_EMIT("movaps      %[x2], 0x10(%[bf])")
                __ASM_EMIT("movaps      %[x4], 0x30(%[bf])")

                __ASM_EMIT("mulps       %[x5], %[x6]")          // x6 = at * b[3]
                __ASM_EMIT("movaps      %[x0], %[x1]")          // x1 = b[0]
                __ASM_EMIT("movaps      %[x2], %[x3]")          // x3 = b[1]
                __ASM_EMIT("movaps      %[x0], %[x5]")          // x5 = b[0]
                __ASM_EMIT("mulps       %[COS2W], %[x1]")       // x1 = b[0]*cos_2w
                __ASM_EMIT("mulps       %[SIN2W], %[x0]")       // x0 = b[0]*sin_2w
                __ASM_EMIT("mulps       %[COSW], %[x3]")        // x3 = b[1]*cos_w
                __ASM_EMIT("mulps       %[SINW], %[x2]")        // x2 = b[1]*sin_w
                __ASM_EMIT("addps       %[x1], %[x4]")          // x4 = b[0]*cos_2w + b[2]
                __ASM_EMIT("addps       %[x0], %[x2]")          // x2 = im = b[0]*sin_2w + b[1]*sin_w
                __ASM_EMIT("addps       %[x3], %[x4]")          // x4 = re = b[0]*cos_2w + b[1]*cos_w + b[2]
                __ASM_EMIT("movaps      %[x4], 0x00(%[CB])")    // DEBUG
                __ASM_EMIT("movaps      %[x2], 0x10(%[CB])")    // DEBUG
                __ASM_EMIT("mulps       %[x2], %[x2]")          // x2 = im*im
                __ASM_EMIT("mulps       %[x4], %[x4]")          // x4 = re*re
                __ASM_EMIT("movaps      %[ONE], %[x0]")         // x0 = 1
                __ASM_EMIT("addps       %[x2], %[x4]")          // x4 = re*re + im*im
                __ASM_EMIT("divps       %[x5], %[x0]")          // x0 = N2 = 1/b[0]
                __ASM_EMIT("movaps      %[x0], 0x10(%[N])")     // DEBUG
                __ASM_EMIT("sqrtps      %[x4], %[x1]")          // x1 = ab = sqrt(re*re + im*im)
                __ASM_EMIT("movaps      %[x1], 0x20(%[CB])")    // DEBUG
                __ASM_EMIT("mulps       %[x7], %[x1]")          // x1 = ab * t[3]
                __ASM_EMIT("divps       %[x6], %[x1]")          // x1 = AN = (ab * t[3]) / (at * b[3])
                __ASM_EMIT("movaps      %[x1], 0x00(%[N])")     // DEBUG
                __ASM_EMIT("mulps       %[x0], %[x1]")          // x1 = N1 = AN * N2
                __ASM_EMIT("movaps      %[x1], 0x20(%[N])")     // DEBUG

                // Update filters
                // Top part
                __ASM_EMIT("movaps      0x00(%[bf]), %[x2]")    // x2 = t[0]
                __ASM_EMIT("movaps      0x20(%[bf]), %[x3]")    // x3 = t[1]
                __ASM_EMIT("movaps      0x40(%[bf]), %[x4]")    // x4 = t[2]

                __ASM_EMIT("mulps       %[x1], %[x2]")          // x2 = t[0] * N1 = a0[0] a0[1] a0[2] a0[3]
                __ASM_EMIT("mulps       %[x1], %[x3]")          // x3 = t[1] * N1 = a1[0] a1[1] a1[2] a1[3]
                __ASM_EMIT("movaps      %[x2], %[x5]")          // x5 = a0[0] a0[1] a0[2] a0[3]
                __ASM_EMIT("mulps       %[x1], %[x4]")          // x4 = t[2] * N1 = a2[0] a2[1] a2[2] a2[3]
                __ASM_EMIT("movaps      %[x2], %[x6]")          // x6 = a0[0] a0[1] a0[2] a0[3]
                __ASM_EMIT("movaps      %[x3], %[x1]")          // x1 = a1[0] a1[1] a1[2] a1[3]
                __ASM_EMIT("unpcklps    %[x4], %[x3]")          // x3 = a1[0] a2[0] a1[1] a2[1]
                __ASM_EMIT("movaps      %[x2], %[x7]")          // x7 = a0[0] a0[1] a0[2] a0[3]
                __ASM_EMIT("unpckhps    %[x4], %[x1]")          // x1 = a1[2] a2[2] a1[3] a2[3]
                __ASM_EMIT("shufps      $0x40, %[x3], %[x2]")   // x2 = a0[0] a0[0] a1[0] a2[0]
                __ASM_EMIT("shufps      $0x4a, %[x1], %[x5]")   // x5 = a0[2] a0[2] a1[2] a2[2]
                __ASM_EMIT("shufps      $0xe5, %[x3], %[x6]")   // x6 = a0[1] a0[1] a1[1] a2[1]
                __ASM_EMIT("shufps      $0xef, %[x1], %[x7]")   // x7 = a0[3] a0[3] a1[3] a2[3]

                __ASM_EMIT("movaps      %[x2], 0x00(%[bf])")
                __ASM_EMIT("movaps      %[x6], 0x20(%[bf])")
                __ASM_EMIT("movaps      %[x5], 0x40(%[bf])")
                __ASM_EMIT("movaps      %[x7], 0x60(%[bf])")

                // Bottom part
                __ASM_EMIT("xorps       %[ISIGN], %[x0]")       // x0 = -N2
                __ASM_EMIT("movaps      0x10(%[bf]), %[x2]")    // x2 = b[1]
                __ASM_EMIT("movaps      0x30(%[bf]), %[x3]")    // x3 = b[2]

                __ASM_EMIT("mulps       %[x0], %[x2]")          // x2 = b[1] * N2 = b1[0] b1[1] b1[2] b1[3]
                __ASM_EMIT("xorps       %[x4], %[x4]")          // x4 = 0 0 0 0
                __ASM_EMIT("mulps       %[x0], %[x3]")          // x3 = b[2] * N2 = b2[0] b2[1] b2[2] b2[3]
                __ASM_EMIT("movaps      %[x2], %[x1]")          // x1 = b1[0] b1[1] b1[2] b1[3]
                __ASM_EMIT("xorps       %[x5], %[x5]")          // x5 = 0 0 0 0
                __ASM_EMIT("unpcklps    %[x3], %[x2]")          // x2 = b1[0] b2[0] b1[1] b2[1]
                __ASM_EMIT("unpckhps    %[x3], %[x1]")          // x1 = b1[2] b2[2] b1[3] b2[3]
                __ASM_EMIT("movhlps     %[x2], %[x4]")          // x4 = b1[1] b2[1] 0 0
                __ASM_EMIT("movhlps     %[x1], %[x5]")          // x5 = b1[3] b2[3] 0 0
                __ASM_EMIT("shufps      $0xf4, %[x4], %[x2]")   // x2 = b1[0] b2[0] 0 0
                __ASM_EMIT("shufps      $0xf4, %[x5], %[x1]")   // x1 = b1[2] b2[2] 0 0

                __ASM_EMIT("movaps      %[x2], 0x10(%[bf])")
                __ASM_EMIT("movaps      %[x4], 0x30(%[bf])")
                __ASM_EMIT("movaps      %[x1], 0x50(%[bf])")
                __ASM_EMIT("movaps      %[x5], 0x70(%[bf])")

                : [x0] "+x" (cos_w), [x1] "+x" (sin_w), [x2] "=&x" (x2), [x3] "=&x" (x3),
                  [x4] "=&x" (x4), [x5] "=&x" (x5), [x6] "=&x" (x6), [x7] "=&x" (x7),
                  [count] "+r" (count),
                  [bc] "+r" (bc),
                  [bf] "+r" (bf)
                : [ONE] "m" (ONE),
                  [ISIGN] "m" (X_ISIGN),
                  [COSW] "m" (COSW),
                  [SINW] "m" (SINW),
                  [COS2W] "m" (COS2W),
                  [SIN2W] "m" (SIN2W)
                  , [T] "r" (T), [B] "r" (B), [N] "r" (N), [CT] "r" (CT), [CB] "r" (CB) // DEBUG
                : "cc", "memory"
            );

            lsp_dumpf("T0", "%f", &T[0], 4);
            lsp_dumpf("T1", "%f", &T[4], 4);
            lsp_dumpf("T2", "%f", &T[8], 4);
            lsp_dumpf("T3", "%f", &T[12], 4);

            lsp_dumpf("RE", "%f", &CT[0], 4);
            lsp_dumpf("IM", "%f", &CT[4], 4);
            lsp_dumpf("AT", "%f", &CT[8], 4);

            lsp_dumpf("B0", "%f", &B[0], 4);
            lsp_dumpf("B1", "%f", &B[4], 4);
            lsp_dumpf("B2", "%f", &B[8], 4);
            lsp_dumpf("B3", "%f", &B[12], 4);

            lsp_dumpf("RE", "%f", &CB[0], 4);
            lsp_dumpf("IM", "%f", &CB[4], 4);
            lsp_dumpf("AB", "%f", &CB[8], 4);

            lsp_dumpf("AN", "%f", &N[0], 4);
            lsp_dumpf("N2", "%f", &N[4], 4);
            lsp_dumpf("N1", "%f", &N[8], 4);

            lsp_dumpf("bf0", "%f", bf[0].a, 8);
            lsp_dumpf("bf1", "%f", bf[1].a, 8);
            lsp_dumpf("bf2", "%f", bf[2].a, 8);
            lsp_dumpf("bf3", "%f", bf[3].a, 8);

            bf += 4;
            bc += 4;
            count -= 4;
        }
    }

    #endif

    void matched_transform_x1(biquad_x1_t *bf, f_cascade_t *bc, float kf, float td, size_t count)
    {
        // Find roots for top and bottom polynoms
        matched_solve(bc->t, kf, td, count, sizeof(f_cascade_t)/sizeof(float));
        matched_solve(bc->b, kf, td, count, sizeof(f_cascade_t)/sizeof(float));

        double w        = kf * td * 0.1;
        float cos_w     = cos(w);
        float sin_w     = sin(w);
    //    float cos_2w    = cos_w*cos_w - sin_w*sin_w; // cos(2x) = cos(x)^2 - sin(x)^2
    //    float sin_2w    = 2*sin_w*cos_w; // sin(2x) = 2 * cos(x) * sin(x)

        float COSW[4]  __lsp_aligned16;
        float SINW[4]  __lsp_aligned16;
        float COS2W[4] __lsp_aligned16;
        float SIN2W[4] __lsp_aligned16;

        float x2, x3, x4, x5, x6, x7;

        ARCH_X86_ASM
        (
            // Prepare
            // Calculate COSW, SINW, COS2W, SIN2W
            __ASM_EMIT("shufps      $0x00, %[x0], %[x0]")   // x0 = cos_w
            __ASM_EMIT("shufps      $0x00, %[x1], %[x1]")   // x1 = sin_w
            __ASM_EMIT("movaps      %[x0], %[COSW]")
            __ASM_EMIT("movaps      %[x1], %[SINW]")
            __ASM_EMIT("movaps      %[x0], %[x2]")          // x2 = cos_w
            __ASM_EMIT("mulps       %[x1], %[x2]")          // x2 = cos_w*sin_w
            __ASM_EMIT("mulps       %[x0], %[x0]")          // x0 = cos_w*cos_w
            __ASM_EMIT("mulps       %[x1], %[x1]")          // x1 = sin_w*sin_w
            __ASM_EMIT("addps       %[x2], %[x2]")          // x2 = sin_2w = 2*cos_w*sin_w
            __ASM_EMIT("subps       %[x1], %[x0]")          // x0 = cos_2s = cos_w*cos_w - sin_w*sin_w
            __ASM_EMIT("movaps      %[x0], %[COS2W]")
            __ASM_EMIT("movaps      %[x2], %[SIN2W]")

            // Check block size
            __ASM_EMIT("sub         $4, %[count]")
            __ASM_EMIT("jb          2f")

            // Load top part
            __ASM_EMIT("1:")
            __ASM_EMIT("movaps      0x00(%[bc]), %[x0]")    // x0 = t0[0] t0[1] t0[2] t0[3]
            __ASM_EMIT("movaps      0x20(%[bc]), %[x1]")    // x1 = t1[0] t1[1] t1[2] t1[3]
            __ASM_EMIT("movaps      0x40(%[bc]), %[x2]")    // x2 = t2[0] t2[1] t2[2] t2[3]
            __ASM_EMIT("movaps      0x60(%[bc]), %[x3]")    // x3 = t3[0] t3[1] t3[2] t3[3]

            // Transpose top part
            __ASM_EMIT("movaps      %[x0], %[x4]")          // x4 = t0[0] t0[1] t0[2] t0[3]
            __ASM_EMIT("movaps      %[x2], %[x5]")          // x5 = t2[0] t2[1] t2[2] t2[3]
            __ASM_EMIT("unpcklps    %[x1], %[x0]")          // x0 = t0[0] t1[0] t0[1] t1[1]
            __ASM_EMIT("unpcklps    %[x3], %[x2]")          // x2 = t2[0] t3[0] t2[1] t3[1]
            __ASM_EMIT("unpckhps    %[x1], %[x4]")          // x4 = t0[2] t1[2] t0[3] t1[3]
            __ASM_EMIT("unpckhps    %[x3], %[x5]")          // x5 = t2[2] t3[2] t2[3] t3[3]
            __ASM_EMIT("movaps      %[x0], %[x1]")          // x1 = t0[0] t1[0] t0[1] t1[1]
            __ASM_EMIT("movaps      %[x4], %[x3]")          // x3 = t0[2] t1[2] t0[3] t1[3]
            __ASM_EMIT("movlhps     %[x2], %[x0]")          // x0 = t0[0] t1[0] t2[0] t3[0]
            __ASM_EMIT("movhlps     %[x1], %[x2]")          // x2 = t0[1] t1[1] t2[1] t3[1]
            __ASM_EMIT("movlhps     %[x5], %[x4]")          // x4 = t0[2] t1[2] t2[2] t3[2]
            __ASM_EMIT("movhlps     %[x3], %[x5]")          // x5 = t0[3] t1[3] t2[3] t3[3]

            // x0 = t[0], x2 = t[1], x4 = t[2], x5 = t[3]
            __ASM_EMIT("movaps      %[x0], 0x00(%[bf])")
            __ASM_EMIT("movaps      %[x2], 0x20(%[bf])")
            __ASM_EMIT("movaps      %[x4], 0x40(%[bf])")

            __ASM_EMIT("movaps      %[x0], %[x1]")          // x1 = t[0]
            __ASM_EMIT("movaps      %[x2], %[x3]")          // x3 = t[1]
            __ASM_EMIT("mulps       %[COS2W], %[x1]")       // x1 = t[0]*cos_2w
            __ASM_EMIT("mulps       %[SIN2W], %[x0]")       // x0 = t[0]*sin_2w
            __ASM_EMIT("mulps       %[COSW], %[x3]")        // x3 = t[1]*cos_w
            __ASM_EMIT("mulps       %[SINW], %[x2]")        // x2 = t[1]*sin_w
            __ASM_EMIT("addps       %[x1], %[x4]")          // x4 = t[0]*cos_2w + t[2]
            __ASM_EMIT("addps       %[x0], %[x2]")          // x2 = im = t[0]*sin_2w + t[1]*sin_w
            __ASM_EMIT("addps       %[x3], %[x4]")          // x4 = re = t[0]*cos_2w + t[1]*cos_w + t[2]
            __ASM_EMIT("mulps       %[x2], %[x2]")          // x2 = im*im
            __ASM_EMIT("mulps       %[x4], %[x4]")          // x4 = re*re
            __ASM_EMIT("addps       %[x2], %[x4]")          // x4 = re*re + im*im
            __ASM_EMIT("movaps      %[x5], %[x7]")          // x7 = t[3]
            __ASM_EMIT("sqrtps      %[x4], %[x6]")          // x6 = at = sqrt(re*re + im*im)

            // Load bottom part
            __ASM_EMIT("movaps      0x10(%[bc]), %[x0]")    // x0 = t0[0] t0[1] t0[2] t0[3]
            __ASM_EMIT("movaps      0x30(%[bc]), %[x1]")    // x1 = t1[0] t1[1] t1[2] t1[3]
            __ASM_EMIT("movaps      0x50(%[bc]), %[x2]")    // x2 = t2[0] t2[1] t2[2] t2[3]
            __ASM_EMIT("movaps      0x70(%[bc]), %[x3]")    // x3 = t3[0] t3[1] t3[2] t3[3]

            // Transpose bottom part
            __ASM_EMIT("movaps      %[x0], %[x4]")          // x4 = t0[0] t0[1] t0[2] t0[3]
            __ASM_EMIT("movaps      %[x2], %[x5]")          // x5 = t2[0] t2[1] t2[2] t2[3]
            __ASM_EMIT("unpcklps    %[x1], %[x0]")          // x0 = t0[0] t1[0] t0[1] t1[1]
            __ASM_EMIT("unpcklps    %[x3], %[x2]")          // x2 = t2[0] t3[0] t2[1] t3[1]
            __ASM_EMIT("unpckhps    %[x1], %[x4]")          // x4 = t0[2] t1[2] t0[3] t1[3]
            __ASM_EMIT("unpckhps    %[x3], %[x5]")          // x5 = t2[2] t3[2] t2[3] t3[3]
            __ASM_EMIT("movaps      %[x0], %[x1]")          // x1 = t0[0] t1[0] t0[1] t1[1]
            __ASM_EMIT("movaps      %[x4], %[x3]")          // x3 = t0[2] t1[2] t0[3] t1[3]
            __ASM_EMIT("movlhps     %[x2], %[x0]")          // x0 = t0[0] t1[0] t2[0] t3[0]
            __ASM_EMIT("movhlps     %[x1], %[x2]")          // x2 = t0[1] t1[1] t2[1] t3[1]
            __ASM_EMIT("movlhps     %[x5], %[x4]")          // x4 = t0[2] t1[2] t2[2] t3[2]
            __ASM_EMIT("movhlps     %[x3], %[x5]")          // x5 = t0[3] t1[3] t2[3] t3[3]

            // x0 = b[0], x2 = b[1], x4 = b[2], x5 = b[3]
            __ASM_EMIT("movaps      %[x2], 0x10(%[bf])")
            __ASM_EMIT("movaps      %[x4], 0x30(%[bf])")

            __ASM_EMIT("mulps       %[x5], %[x6]")          // x6 = at * b[3]
            __ASM_EMIT("movaps      %[x0], %[x1]")          // x1 = b[0]
            __ASM_EMIT("movaps      %[x2], %[x3]")          // x3 = b[1]
            __ASM_EMIT("movaps      %[x0], %[x5]")          // x5 = b[0]
            __ASM_EMIT("mulps       %[COS2W], %[x1]")       // x1 = b[0]*cos_2w
            __ASM_EMIT("mulps       %[SIN2W], %[x0]")       // x0 = b[0]*sin_2w
            __ASM_EMIT("mulps       %[COSW], %[x3]")        // x3 = b[1]*cos_w
            __ASM_EMIT("mulps       %[SINW], %[x2]")        // x2 = b[1]*sin_w
            __ASM_EMIT("addps       %[x1], %[x4]")          // x4 = b[0]*cos_2w + b[2]
            __ASM_EMIT("addps       %[x0], %[x2]")          // x2 = im = b[0]*sin_2w + b[1]*sin_w
            __ASM_EMIT("addps       %[x3], %[x4]")          // x4 = re = b[0]*cos_2w + b[1]*cos_w + b[2]
            __ASM_EMIT("mulps       %[x2], %[x2]")          // x2 = im*im
            __ASM_EMIT("mulps       %[x4], %[x4]")          // x4 = re*re
            __ASM_EMIT("movaps      %[ONE], %[x0]")         // x0 = 1
            __ASM_EMIT("addps       %[x2], %[x4]")          // x4 = re*re + im*im
            __ASM_EMIT("divps       %[x5], %[x0]")          // x0 = N2 = 1/b[0]
            __ASM_EMIT("sqrtps      %[x4], %[x1]")          // x1 = ab = sqrt(re*re + im*im)
            __ASM_EMIT("mulps       %[x7], %[x1]")          // x1 = ab * t[3]
            __ASM_EMIT("divps       %[x6], %[x1]")          // x1 = AN = (ab * t[3]) / (at * b[3])
            __ASM_EMIT("mulps       %[x0], %[x1]")          // x1 = N1 = AN * N2

            // Update filters
            // Top part
            __ASM_EMIT("movaps      0x00(%[bf]), %[x2]")    // x2 = t[0]
            __ASM_EMIT("movaps      0x20(%[bf]), %[x3]")    // x3 = t[1]
            __ASM_EMIT("movaps      0x40(%[bf]), %[x4]")    // x4 = t[2]

            __ASM_EMIT("mulps       %[x1], %[x2]")          // x2 = t[0] * N1 = a0[0] a0[1] a0[2] a0[3]
            __ASM_EMIT("mulps       %[x1], %[x3]")          // x3 = t[1] * N1 = a1[0] a1[1] a1[2] a1[3]
            __ASM_EMIT("movaps      %[x2], %[x5]")          // x5 = a0[0] a0[1] a0[2] a0[3]
            __ASM_EMIT("mulps       %[x1], %[x4]")          // x4 = t[2] * N1 = a2[0] a2[1] a2[2] a2[3]
            __ASM_EMIT("movaps      %[x2], %[x6]")          // x6 = a0[0] a0[1] a0[2] a0[3]
            __ASM_EMIT("movaps      %[x3], %[x1]")          // x1 = a1[0] a1[1] a1[2] a1[3]
            __ASM_EMIT("unpcklps    %[x4], %[x3]")          // x3 = a1[0] a2[0] a1[1] a2[1]
            __ASM_EMIT("movaps      %[x2], %[x7]")          // x7 = a0[0] a0[1] a0[2] a0[3]
            __ASM_EMIT("unpckhps    %[x4], %[x1]")          // x1 = a1[2] a2[2] a1[3] a2[3]
            __ASM_EMIT("shufps      $0x40, %[x3], %[x2]")   // x2 = a0[0] a0[0] a1[0] a2[0]
            __ASM_EMIT("shufps      $0x4a, %[x1], %[x5]")   // x5 = a0[2] a0[2] a1[2] a2[2]
            __ASM_EMIT("shufps      $0xe5, %[x3], %[x6]")   // x6 = a0[1] a0[1] a1[1] a2[1]
            __ASM_EMIT("shufps      $0xef, %[x1], %[x7]")   // x7 = a0[3] a0[3] a1[3] a2[3]

            __ASM_EMIT("movaps      %[x2], 0x00(%[bf])")
            __ASM_EMIT("movaps      %[x6], 0x20(%[bf])")
            __ASM_EMIT("movaps      %[x5], 0x40(%[bf])")
            __ASM_EMIT("movaps      %[x7], 0x60(%[bf])")

            // Bottom part
            __ASM_EMIT("xorps       %[ISIGN], %[x0]")       // x0 = -N2
            __ASM_EMIT("movaps      0x10(%[bf]), %[x2]")    // x2 = b[1]
            __ASM_EMIT("movaps      0x30(%[bf]), %[x3]")    // x3 = b[2]

            __ASM_EMIT("mulps       %[x0], %[x2]")          // x2 = b[1] * N2 = b1[0] b1[1] b1[2] b1[3]
            __ASM_EMIT("xorps       %[x4], %[x4]")          // x4 = 0 0 0 0
            __ASM_EMIT("mulps       %[x0], %[x3]")          // x3 = b[2] * N2 = b2[0] b2[1] b2[2] b2[3]
            __ASM_EMIT("movaps      %[x2], %[x1]")          // x1 = b1[0] b1[1] b1[2] b1[3]
            __ASM_EMIT("xorps       %[x5], %[x5]")          // x5 = 0 0 0 0
            __ASM_EMIT("unpcklps    %[x3], %[x2]")          // x2 = b1[0] b2[0] b1[1] b2[1]
            __ASM_EMIT("unpckhps    %[x3], %[x1]")          // x1 = b1[2] b2[2] b1[3] b2[3]
            __ASM_EMIT("movhlps     %[x2], %[x4]")          // x4 = b1[1] b2[1] 0 0
            __ASM_EMIT("movhlps     %[x1], %[x5]")          // x5 = b1[3] b2[3] 0 0
            __ASM_EMIT("shufps      $0xf4, %[x4], %[x2]")   // x2 = b1[0] b2[0] 0 0
            __ASM_EMIT("shufps      $0xf4, %[x5], %[x1]")   // x1 = b1[2] b2[2] 0 0

            __ASM_EMIT("movaps      %[x2], 0x10(%[bf])")
            __ASM_EMIT("movaps      %[x4], 0x30(%[bf])")
            __ASM_EMIT("movaps      %[x1], 0x50(%[bf])")
            __ASM_EMIT("movaps      %[x5], 0x70(%[bf])")

            // Repeat loop
            __ASM_EMIT("add         $0x80, %[bc]")
            __ASM_EMIT("add         $0x80, %[bf]")
            __ASM_EMIT("sub         $4, %[count]")
            __ASM_EMIT("jae         1b")

            __ASM_EMIT("2:")
            __ASM_EMIT("add         $4, %[count]")
            __ASM_EMIT("jz          100f")

            // Single-item loop
            __ASM_EMIT("3:")
            __ASM_EMIT("movss       0x00(%[bc]), %[x0]")    // x0 = t[0]
            __ASM_EMIT("movss       0x04(%[bc]), %[x2]")    // x2 = t[1]
            __ASM_EMIT("movss       0x08(%[bc]), %[x4]")    // x4 = t[2]
            __ASM_EMIT("movss       0x0c(%[bc]), %[x5]")    // x5 = t[3]

            // x0 = t[0], x2 = t[1], x4 = t[2], x5 = t[3]
            __ASM_EMIT("movaps      %[x0], %[x1]")          // x1 = t[0]
            __ASM_EMIT("movaps      %[x2], %[x3]")          // x3 = t[1]
            __ASM_EMIT("mulss       %[COS2W], %[x1]")       // x1 = t[0]*cos_2w
            __ASM_EMIT("mulss       %[SIN2W], %[x0]")       // x0 = t[0]*sin_2w
            __ASM_EMIT("mulss       %[COSW], %[x3]")        // x3 = t[1]*cos_w
            __ASM_EMIT("mulss       %[SINW], %[x2]")        // x2 = t[1]*sin_w
            __ASM_EMIT("addss       %[x1], %[x4]")          // x4 = t[0]*cos_2w + t[2]
            __ASM_EMIT("addss       %[x0], %[x2]")          // x2 = im = t[0]*sin_2w + t[1]*sin_w
            __ASM_EMIT("addss       %[x3], %[x4]")          // x4 = re = t[0]*cos_2w + t[1]*cos_w + t[2]
            __ASM_EMIT("mulss       %[x2], %[x2]")          // x2 = im*im
            __ASM_EMIT("mulss       %[x4], %[x4]")          // x4 = re*re
            __ASM_EMIT("addss       %[x2], %[x4]")          // x4 = re*re + im*im
            __ASM_EMIT("movaps      %[x5], %[x7]")          // x7 = t[3]
            __ASM_EMIT("sqrtss      %[x4], %[x6]")          // x6 = at = sqrt(re*re + im*im)

            // Load bottom part
            __ASM_EMIT("movss       0x10(%[bc]), %[x0]")    // x0 = b[0]
            __ASM_EMIT("movss       0x14(%[bc]), %[x2]")    // x2 = b[1]
            __ASM_EMIT("movss       0x18(%[bc]), %[x4]")    // x4 = b[2]
            __ASM_EMIT("movss       0x1c(%[bc]), %[x5]")    // x5 = b[3]

            // x0 = b[0], x2 = b[1], x4 = b[2], x5 = b[3]
            __ASM_EMIT("mulss       %[x5], %[x6]")          // x6 = at * b[3]
            __ASM_EMIT("movaps      %[x0], %[x1]")          // x1 = b[0]
            __ASM_EMIT("movaps      %[x2], %[x3]")          // x3 = b[1]
            __ASM_EMIT("movaps      %[x0], %[x5]")          // x5 = b[0]
            __ASM_EMIT("mulss       %[COS2W], %[x1]")       // x1 = b[0]*cos_2w
            __ASM_EMIT("mulss       %[SIN2W], %[x0]")       // x0 = b[0]*sin_2w
            __ASM_EMIT("mulss       %[COSW], %[x3]")        // x3 = b[1]*cos_w
            __ASM_EMIT("mulss       %[SINW], %[x2]")        // x2 = b[1]*sin_w
            __ASM_EMIT("addss       %[x1], %[x4]")          // x4 = b[0]*cos_2w + b[2]
            __ASM_EMIT("addss       %[x0], %[x2]")          // x2 = im = b[0]*sin_2w + b[1]*sin_w
            __ASM_EMIT("addss       %[x3], %[x4]")          // x4 = re = b[0]*cos_2w + b[1]*cos_w + b[2]
            __ASM_EMIT("mulss       %[x2], %[x2]")          // x2 = im*im
            __ASM_EMIT("mulss       %[x4], %[x4]")          // x4 = re*re
            __ASM_EMIT("movaps      %[ONE], %[x0]")         // x0 = 1
            __ASM_EMIT("addss       %[x2], %[x4]")          // x4 = re*re + im*im
            __ASM_EMIT("divss       %[x5], %[x0]")          // x0 = N2 = 1/b[0]
            __ASM_EMIT("sqrtss      %[x4], %[x1]")          // x1 = ab = sqrt(re*re + im*im)
            __ASM_EMIT("mulss       %[x7], %[x1]")          // x1 = ab * t[3]
            __ASM_EMIT("divss       %[x6], %[x1]")          // x1 = AN = (ab * t[3]) / (at * b[3])
            __ASM_EMIT("mulss       %[x0], %[x1]")          // x1 = N1 = AN * N2

            // Update filters
            // Top part
            __ASM_EMIT("movss       0x00(%[bc]), %[x2]")    // x2 = t[0]
            __ASM_EMIT("movss       0x04(%[bc]), %[x3]")    // x3 = t[1]
            __ASM_EMIT("movss       0x08(%[bc]), %[x4]")    // x4 = t[2]

            __ASM_EMIT("mulss       %[x1], %[x2]")          // x2 = t[0] * N1 = a0
            __ASM_EMIT("mulss       %[x1], %[x3]")          // x3 = t[1] * N1 = a1
            __ASM_EMIT("mulss       %[x1], %[x4]")          // x4 = t[2] * N1 = a2
            __ASM_EMIT("unpcklps    %[x4], %[x3]")          // x3 = a1 a2 ? ?
            __ASM_EMIT("shufps      $0x40, %[x3], %[x2]")   // x2 = a0 a0 a1 a2
            __ASM_EMIT("movaps      %[x2], 0x00(%[bf])")

            // Bottom part
            __ASM_EMIT("xorps       %[ISIGN], %[x0]")       // x0 = -N2
            __ASM_EMIT("movss       0x14(%[bc]), %[x2]")    // x2 = b[1]
            __ASM_EMIT("movss       0x18(%[bc]), %[x3]")    // x3 = b[2]

            __ASM_EMIT("mulss       %[x0], %[x2]")          // x2 = b[1] * N2 = b1
            __ASM_EMIT("mulss       %[x0], %[x3]")          // x3 = b[2] * N2 = b2
            __ASM_EMIT("unpcklps    %[x3], %[x2]")          // x2 = b1 b2 0 0
            __ASM_EMIT("movaps      %[x2], 0x10(%[bf])")

            // Repeat loop
            __ASM_EMIT("add         $0x20, %[bc]")
            __ASM_EMIT("add         $0x20, %[bf]")
            __ASM_EMIT("dec         %[count]")
            __ASM_EMIT("jnz         3b")

            __ASM_EMIT("100:")
            : [x0] "+x" (cos_w), [x1] "+x" (sin_w), [x2] "=&x" (x2), [x3] "=&x" (x3),
              [x4] "=&x" (x4), [x5] "=&x" (x5), [x6] "=&x" (x6), [x7] "=&x" (x7),
              [count] "+r" (count),
              [bc] "+r" (bc),
              [bf] "+r" (bf)
            : [ONE] "m" (ONE),
              [ISIGN] "m" (X_ISIGN),
              [COSW] "m" (COSW),
              [SINW] "m" (SINW),
              [COS2W] "m" (COS2W),
              [SIN2W] "m" (SIN2W)
            : "cc", "memory"
        );
    }
}

#endif /* DSP_ARCH_X86_SSE_FILTERS_TRANSFORM_H_ */
