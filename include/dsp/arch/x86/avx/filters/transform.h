/*
 * transform.h
 *
 *  Created on: 24 февр. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_AVX_FILTERS_TRANSFORM_H_
#define DSP_ARCH_X86_AVX_FILTERS_TRANSFORM_H_

#ifndef DSP_ARCH_X86_AVX_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_AVX_IMPL */

namespace avx
{
    void x64_bilinear_transform_x8(biquad_x8_t *bf, const f_cascade_t *bc, float kf, size_t count)
    {
    #ifdef ARCH_X86_64_AVX
        ARCH_X86_ASM
        (
            __ASM_EMIT("test            %[count], %[count]")
            __ASM_EMIT("jz              100f")

            // Prepare values
            __ASM_EMIT("vbroadcastss    %[kf], %%ymm0")                     // ymm0  = kf
            __ASM_EMIT("vmovaps         %[ONE], %%ymm15")                   // ymm15 = 1
            __ASM_EMIT("vmulps          %%ymm0, %%ymm0, %%ymm1")            // ymm1  = kf*kf = kf2

            // Do a loop
            __ASM_EMIT("1:")
            __ASM_EMIT("vmovaps         0x00(%[bc]), %%ymm2")               // ymm2  = t0[0] t1[0] t2[0]   ?   b0[0] b1[0] b2[0]   ?
            __ASM_EMIT("vmovaps         0x20(%[bc]), %%ymm3")               // ymm3  = t0[1] t1[1] t2[1]   ?   b0[1] b1[1] b2[1]   ?
            __ASM_EMIT("vmovaps         0x40(%[bc]), %%ymm4")               // ymm4  = t0[2] t1[2] t2[2]   ?   b0[2] b1[2] b2[2]   ?
            __ASM_EMIT("vmovaps         0x60(%[bc]), %%ymm5")               // ymm5  = t0[3] t1[3] t2[3]   ?   b0[3] b1[3] b2[3]   ?
            __ASM_EMIT("vmovaps         0x80(%[bc]), %%ymm6")               // ymm6  = t0[4] t1[4] t2[4]   ?   b0[4] b1[4] b2[4]   ?
            __ASM_EMIT("vmovaps         0xa0(%[bc]), %%ymm7")               // ymm7  = t0[5] t1[5] t2[5]   ?   b0[5] b1[5] b2[5]   ?
            __ASM_EMIT("vmovaps         0xc0(%[bc]), %%ymm8")               // ymm8  = t0[6] t1[6] t2[6]   ?   b0[6] b1[6] b2[6]   ?
            __ASM_EMIT("vmovaps         0xe0(%[bc]), %%ymm9")               // ymm9  = t0[7] t1[7] t2[7]   ?   b0[7] b1[7] b2[7]   ?

            // Transpose, step 1
            __ASM_EMIT("vunpcklps       %%ymm3, %%ymm2, %%ymm10")           // ymm10 = t0[0] t0[1] t1[0] t1[1] b0[0] b0[1] b1[0] b1[1]
            __ASM_EMIT("vunpcklps       %%ymm5, %%ymm4, %%ymm11")           // ymm11 = t0[2] t0[3] t1[2] t1[3] b0[2] b0[3] b1[2] b1[3]
            __ASM_EMIT("vunpcklps       %%ymm7, %%ymm6, %%ymm12")           // ymm12 = t0[4] t0[5] t1[4] t1[5] b0[4] b0[5] b1[4] b1[5]
            __ASM_EMIT("vunpcklps       %%ymm9, %%ymm8, %%ymm13")           // ymm13 = t0[6] t0[7] t1[6] t1[7] b0[6] b0[7] b1[6] b1[7]
            __ASM_EMIT("vunpckhps       %%ymm3, %%ymm2, %%ymm2")            // ymm2  = t2[0] t2[1]   ?     ?   b2[0] b2[1]   ?     ?
            __ASM_EMIT("vunpckhps       %%ymm5, %%ymm4, %%ymm4")            // ymm4  = t2[2] t2[3]   ?     ?   b2[2] b2[3]   ?     ?
            __ASM_EMIT("vunpckhps       %%ymm7, %%ymm6, %%ymm6")            // ymm6  = t2[4] t2[5]   ?     ?   b2[4] b2[5]   ?     ?
            __ASM_EMIT("vunpckhps       %%ymm9, %%ymm8, %%ymm8")            // ymm8  = t2[6] t2[7]   ?     ?   b2[6] b2[7]   ?     ?

            // Transpose, step 2
            __ASM_EMIT("vshufps         $0x44, %%ymm11, %%ymm10, %%ymm3")   // ymm3  = t0[0] t0[1] t0[2] t0[3] b0[0] b0[1] b0[2] b0[3]
            __ASM_EMIT("vshufps         $0x44, %%ymm13, %%ymm12, %%ymm5")   // ymm5  = t0[4] t0[5] t0[6] t0[7] b0[4] b0[5] b0[6] b0[7]
            __ASM_EMIT("vshufps         $0x44, %%ymm4,  %%ymm2,  %%ymm2")   // ymm2  = t2[0] t2[1] t2[2] t2[3] b2[0] b2[1] b2[2] b2[3]
            __ASM_EMIT("vshufps         $0x44, %%ymm8,  %%ymm6,  %%ymm6")   // ymm6  = t2[4] t2[5] t2[6] t2[7] b2[4] b2[5] b2[6] b2[7]
            __ASM_EMIT("vshufps         $0xee, %%ymm11, %%ymm10, %%ymm4")   // ymm4  = t1[0] t1[1] t1[2] t1[3] b1[0] b1[1] b1[2] b1[3]
            __ASM_EMIT("vshufps         $0xee, %%ymm13, %%ymm12, %%ymm7")   // ymm7  = t1[4] t1[5] t1[6] t1[7] b1[4] b1[5] b1[6] b1[7]

            // Transpose, step 3
            __ASM_EMIT("vextractf128    $1, %%ymm3,  %%xmm8")               // xmm8  = b0[0] b0[1] b0[2] b0[3]   0     0     0     0
            __ASM_EMIT("vextractf128    $1, %%ymm2,  %%xmm9")               // xmm9  = b2[0] b2[1] b2[2] b2[3]   0     0     0     0
            __ASM_EMIT("vextractf128    $1, %%ymm4,  %%xmm10")              // xmm10 = b1[0] b1[1] b1[2] b1[3]   0     0     0     0
            __ASM_EMIT("vinsertf128     $1, %%xmm5,  %%ymm3, %%ymm3")       // ymm3  = t0[0] t0[1] t0[2] t0[3] t0[4] t0[5] t0[6] t0[7]
            __ASM_EMIT("vinsertf128     $1, %%xmm6,  %%ymm2, %%ymm2")       // ymm2  = t2[0] t2[1] t2[2] t2[3] t2[4] t2[5] t2[6] t2[7]
            __ASM_EMIT("vinsertf128     $1, %%xmm7,  %%ymm4, %%ymm4")       // ymm4  = t1[0] t1[1] t1[2] t1[3] t1[4] t1[5] t1[6] t1[7]
            __ASM_EMIT("vinsertf128     $0, %%xmm8,  %%ymm5, %%ymm5")       // ymm5  = b0[0] b0[1] b0[2] b0[3] b0[4] b0[5] b0[6] b0[7]
            __ASM_EMIT("vinsertf128     $0, %%xmm9,  %%ymm6, %%ymm6")       // ymm6  = b2[0] b2[1] b2[2] b2[3] b2[4] b2[5] b2[6] b2[7]
            __ASM_EMIT("vinsertf128     $0, %%xmm10, %%ymm7, %%ymm7")       // ymm7  = b1[0] b1[1] b1[2] b1[3] b1[4] b1[5] b1[6] b1[7]

            // Now we are ready to perform calculations of top and bottom part
            // ymm3 = T0
            // ymm5 = B0
            __ASM_EMIT("vmulps          %%ymm0, %%ymm4, %%ymm4")            // ymm4  = T1 = t1 * kf
            __ASM_EMIT("vmulps          %%ymm1, %%ymm2, %%ymm2")            // ymm2  = T2 = t2 * kf2
            __ASM_EMIT("vmulps          %%ymm0, %%ymm7, %%ymm7")            // ymm7  = B1 = b1 * kf
            __ASM_EMIT("vmulps          %%ymm1, %%ymm6, %%ymm6")            // ymm6  = B2 = b2 * kf2

            __ASM_EMIT("vaddps          %%ymm2, %%ymm3, %%ymm10")           // ymm10 = T0 + T2
            __ASM_EMIT("vsubps          %%ymm5, %%ymm6, %%ymm8")            // ymm8  = B2 - B0
            __ASM_EMIT("vsubps          %%ymm2, %%ymm3, %%ymm3")            // ymm3  = T0 - T2
            __ASM_EMIT("vaddps          %%ymm5, %%ymm6, %%ymm5")            // ymm5  = B2 + B0
            __ASM_EMIT("vaddps          %%ymm4, %%ymm10, %%ymm11")          // ymm11 = T0 + T1 + T2
            __ASM_EMIT("vaddps          %%ymm8, %%ymm8, %%ymm8")            // ymm8  = 2 * (B2 - B0)
            __ASM_EMIT("vaddps          %%ymm3, %%ymm3, %%ymm3")            // ymm3  = 2 * (T0 - T2)
            __ASM_EMIT("vaddps          %%ymm5, %%ymm7, %%ymm9")            // ymm9  = B0 + B1 + B2
            __ASM_EMIT("vsubps          %%ymm4, %%ymm10, %%ymm4")           // ymm4  = T0 - T1 + T2
            __ASM_EMIT("vdivps          %%ymm9, %%ymm15, %%ymm9")           // ymm9  = N = 1 / (B0 + B1 + B2)
            __ASM_EMIT("vsubps          %%ymm5, %%ymm7, %%ymm5")            // ymm5  = B1 - B2 - B0

            __ASM_EMIT("vmulps          %%ymm9, %%ymm11, %%ymm11")          // ymm11 = A0 = (T0 + T1 + T2) * N
            __ASM_EMIT("vmulps          %%ymm9, %%ymm3, %%ymm3")            // ymm3  = A1 = 2 * (T0 - T2) * N
            __ASM_EMIT("vmulps          %%ymm9, %%ymm4, %%ymm4")            // ymm4  = A2 = (T0 - T1 + T2) * N
            __ASM_EMIT("vmulps          %%ymm9, %%ymm8, %%ymm8")            // ymm8  = B0 = 2 * (B2 - B0) * N
            __ASM_EMIT("vmulps          %%ymm9, %%ymm5, %%ymm5")            // ymm5  = B1 = (B1 - B2 - B0) * N

            // Store values
            __ASM_EMIT("vmovaps         %%ymm11, 0x00(%[bf])")
            __ASM_EMIT("vmovaps         %%ymm3,  0x20(%[bf])")
            __ASM_EMIT("vmovaps         %%ymm4,  0x40(%[bf])")
            __ASM_EMIT("vmovaps         %%ymm8,  0x60(%[bf])")
            __ASM_EMIT("vmovaps         %%ymm5,  0x80(%[bf])")

            // Repeat cycle
            __ASM_EMIT("add             $0x100, %[bc]")
            __ASM_EMIT("add             $0xa0,  %[bf]")
            __ASM_EMIT("dec             %[count]")
            __ASM_EMIT("jnz             1b")

            __ASM_EMIT("vzeroupper")

            __ASM_EMIT("100:")
            : [count] "+r" (count), [bc] "+r" (bc), [bf] "+r" (bf)
            : [ONE] "m" (ONE), [kf] "m" (kf)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5", "%xmm6", "%xmm7",
              "%xmm8", "%xmm9", "%xmm10", "%xmm11", "%xmm12", "%xmm13", "%xmm14", "%xmm15"
        );
    #endif /* ARCH_X86_64_AVX */

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
    //        bf->b[0]        = 2.0 * (B[2] - B[0]) * N;  // Sign negated
    //        bf->b[1]        = (B[1] - B[2] - B[0]) * N; // Sign negated
    //        bf->b[2]        = 0.0f;
    //        bf->b[3]        = 0.0f;
    //
    //        // Calculate top coefficients
    //        T[0]            = bc->t[0];
    //        T[1]            = bc->t[1]*kf;
    //        T[2]            = bc->t[2]*kf2;
    //
    //        bf->a[0]        = (T[0] + T[1] + T[2]) * N;
    //        bf->a[1]        = bf->a[0];
    //        bf->a[2]        = 2.0 * (T[0] - T[2]) * N;
    //        bf->a[3]        = (T[0] - T[1] + T[2]) * N;
    //
    //        // Increment pointers
    //        bc              ++;
    //        bf              ++;
    }
}

#endif /* DSP_ARCH_X86_AVX_FILTERS_TRANSFORM_H_ */
