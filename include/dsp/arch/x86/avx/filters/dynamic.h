/*
 * dynamic.h
 *
 *  Created on: 15 февр. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_AVX_DYN_FILTERS_H_
#define DSP_ARCH_X86_AVX_DYN_FILTERS_H_

#ifndef DSP_ARCH_X86_AVX_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_AVX_IMPL */

namespace avx
{
    void dyn_biquad_process_x1(float *dst, const float *src, float *d, size_t count, const biquad_x1_t *f)
    {
        IF_ARCH_X86(size_t off);

        ARCH_X86_ASM
        (
            // Check count
            __ASM_EMIT64("test              %[count], %[count]")
            __ASM_EMIT32NP("test            %[count], %[count]")
            __ASM_EMIT32P("cmpl             $0, %[count]")
            __ASM_EMIT("jz                  2f")

            // Load permanent data
            __ASM_EMIT("vmovss              0x00(%[d]), %%xmm6")                                // xmm6 = d0
            __ASM_EMIT("xor                 %[off], %[off]")
            __ASM_EMIT("vmovss              0x04(%[d]), %%xmm7")                                // xmm7 = d1

            // Start loop
            __ASM_EMIT("1:")
            __ASM_EMIT("vmovss              (%[src], %[off], 4), %%xmm0")                       // xmm0 = s ? ? ?
            __ASM_EMIT("vmulss              0x00(%[f]), %%xmm0, %%xmm1")                        // xmm1 = a0*s
            __ASM_EMIT("vmulss              0x04(%[f]), %%xmm0, %%xmm2")                        // xmm2 = a1*s
            __ASM_EMIT("vmulss              0x08(%[f]), %%xmm0, %%xmm3")                        // xmm3 = a2*s
            __ASM_EMIT("vaddss              %%xmm6, %%xmm1, %%xmm0")                            // xmm0 = s' = d0 + a0*s
            __ASM_EMIT("vaddss              %%xmm7, %%xmm2, %%xmm2")                            // xmm2 = d1 + a1*s
            __ASM_EMIT("vmulss              0x0c(%[f]), %%xmm0, %%xmm4")                        // xmm4 = b1*s'
            __ASM_EMIT("vmulss              0x10(%[f]), %%xmm0, %%xmm5")                        // xmm5 = b2*s'
            __ASM_EMIT("vmovss              %%xmm0, (%[dst], %[off], 4)")                       // *dst = s'
            __ASM_EMIT("add                 $0x20, %[f]")
            __ASM_EMIT("vaddss              %%xmm4, %%xmm2, %%xmm6")                            // xmm6 = d0' = d1 + a1*s + b1*s'
            __ASM_EMIT("add                 $1, %[off]")
            __ASM_EMIT("vaddss              %%xmm5, %%xmm3, %%xmm7")                            // xmm7 = d1' = a2*s + b2*s'
            __ASM_EMIT("cmp                 %[count], %[off]")
            __ASM_EMIT("jb                  1b")

            // Store the updated buffer state
            __ASM_EMIT("vmovss              %%xmm6, 0x00(%[d])")
            __ASM_EMIT("vmovss              %%xmm7, 0x04(%[d])")

            // Exit label
            __ASM_EMIT("2:")

            : [off] "=&r"(off), [f] "+r" (f)
            : [dst] "r" (dst), [src] "r" (src),
              [count] "r" (count),
              [d] "r" (d)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void dyn_biquad_process_x1_fma3(float *dst, const float *src, float *d, size_t count, const biquad_x1_t *f)
    {
        IF_ARCH_X86(size_t off);

        ARCH_X86_ASM
        (
            // Check count
            __ASM_EMIT64("test              %[count], %[count]")
            __ASM_EMIT32NP("test            %[count], %[count]")
            __ASM_EMIT32P("cmpl             $0, %[count]")
            __ASM_EMIT("jz                  2f")

            // Load permanent data
            __ASM_EMIT("vmovss              0x00(%[d]), %%xmm6")                                // xmm6 = d0
            __ASM_EMIT("xor                 %[off], %[off]")
            __ASM_EMIT("vmovss              0x04(%[d]), %%xmm7")                                // xmm7 = d1

            // Start loop
            __ASM_EMIT("1:")
            __ASM_EMIT("vmovss              (%[src], %[off], 4), %%xmm0")                       // xmm0 = s ? ? ?
            __ASM_EMIT("vmovaps             %%xmm7, %%xmm5")                                    // xmm5 = d1
            __ASM_EMIT("vmulss              0x04(%[f]), %%xmm0, %%xmm2")                        // xmm2 = a1*s
            __ASM_EMIT("vmulss              0x08(%[f]), %%xmm0, %%xmm7")                        // xmm7 = a2*s
            __ASM_EMIT("vfmadd132ss         0x00(%[f]), %%xmm6, %%xmm0")                        // xmm0 = s' = d0 + a0*s
            __ASM_EMIT("vfmadd231ss         0x0c(%[f]), %%xmm0, %%xmm2")                        // xmm2 = a1*s + b1*s'
            __ASM_EMIT("vmovss              %%xmm0, (%[dst], %[off], 4)")                       // *dst = s'
            __ASM_EMIT("vfmadd231ss         0x10(%[f]), %%xmm0, %%xmm7")                        // xmm7 = d1' = a2*s + b2*s'
            __ASM_EMIT("add                 $1, %[off]")
            __ASM_EMIT("add                 $0x20, %[f]")
            __ASM_EMIT("vaddss              %%xmm5, %%xmm2, %%xmm6")                            // xmm6 = d0' = d1 + a1*s + b1*s'
            __ASM_EMIT("cmp                 %[count], %[off]")
            __ASM_EMIT("jb                  1b")

            // Store the updated buffer state
            __ASM_EMIT("vmovss              %%xmm6, 0x00(%[d])")
            __ASM_EMIT("vmovss              %%xmm7, 0x04(%[d])")

            // Exit label
            __ASM_EMIT("2:")

            : [off] "=&r"(off), [f] "+r" (f)
            : [dst] "r" (dst), [src] "r" (src),
              [count] "r" (count),
              [d] "r" (d)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void dyn_biquad_process_x2(float *dst, const float *src, float *d, size_t count, const biquad_x2_t *f)
    {
        ARCH_X86_ASM
        (
            // Check count
            __ASM_EMIT("test                %[count], %[count]")
            __ASM_EMIT("jz                  4f")

            // Start loop
            __ASM_EMIT("vmovss              (%[src]), %%xmm0")                                  // xmm0 = s ? ? ?
            __ASM_EMIT("vmulss              0x00(%[f]), %%xmm0, %%xmm1")                        // xmm1 = a0*s
            __ASM_EMIT("vmulss              0x08(%[f]), %%xmm0, %%xmm2")                        // xmm2 = a1*s
            __ASM_EMIT("vmulss              0x10(%[f]), %%xmm0, %%xmm3")                        // xmm3 = a2*s
            __ASM_EMIT("vaddss              0x00(%[d]), %%xmm1, %%xmm0")                        // xmm0 = s' = a0*s + d0
            __ASM_EMIT("vaddss              0x08(%[d]), %%xmm2, %%xmm2")                        // xmm2 = d1 + a1*s
            __ASM_EMIT("vmulss              0x18(%[f]), %%xmm0, %%xmm4")                        // xmm4 = b1*s'
            __ASM_EMIT("vmulss              0x20(%[f]), %%xmm0, %%xmm5")                        // xmm5 = b2*s'
            __ASM_EMIT("vaddss              %%xmm4, %%xmm2, %%xmm6")                            // xmm6 = d0' = d1 + a1*s + b1*s'
            __ASM_EMIT("vaddss              %%xmm5, %%xmm3, %%xmm7")                            // xmm7 = d1' = a2*s + b2*s'
            __ASM_EMIT("vshufps             $0xb1, %%xmm0, %%xmm0, %%xmm0")                     // shift
            __ASM_EMIT("add                 $0x04, %[src]")                                     // src++
            __ASM_EMIT("add                 $0x30, %[f]")
            __ASM_EMIT("dec                 %[count]")
            __ASM_EMIT("vmovss              %%xmm6, 0x00(%[d])")
            __ASM_EMIT("vmovss              %%xmm7, 0x08(%[d])")
            __ASM_EMIT("jz                  2f")
            // x2 loop
            __ASM_EMIT("vmovups             0x00(%[d]), %%xmm6")                                // xmm6 = d0 e0 d1 e1
            __ASM_EMIT("vxorps              %%xmm7, %%xmm7, %%xmm7")                            // xmm7 = 0 0 0 0
            __ASM_EMIT(".align              16")
            __ASM_EMIT("1:")
            __ASM_EMIT("vinsertps           $0xc0, (%[src]), %%xmm0, %%xmm0")                   // xmm0 = s0 s1
            __ASM_EMIT("vmovlhps            %%xmm0, %%xmm0, %%xmm0")                            // xmm0 = s0 s1 s0 s1
            __ASM_EMIT("vmulps              0x08(%[f]), %%xmm0, %%xmm2")                        // xmm2 = a1*s0 i1*s1 a2*s0 i2*s1
            __ASM_EMIT("vmulps              0x00(%[f]), %%xmm0, %%xmm0")                        // xmm0 = a0*s0 i0*s1
            __ASM_EMIT("vaddps              %%xmm6, %%xmm0, %%xmm0")                            // xmm0 = s0' s1' = d0+a0*s0 e0+i0*s1
            __ASM_EMIT("vshufps             $0x0e, %%xmm7, %%xmm6, %%xmm6")                     // xmm6 = d1 e1 0 0
            __ASM_EMIT("vmovlhps            %%xmm0, %%xmm0, %%xmm0")                            // xmm0 = s0' s1' s0' s1'
            __ASM_EMIT("vaddps              %%xmm2, %%xmm6, %%xmm6")                            // xmm6 = d1+a1*s0 e1+i1*s1 a2*s0 i2*s1
            __ASM_EMIT("vmulps              0x18(%[f]), %%xmm0, %%xmm3")                        // xmm3 = b1*s0' j1*s1' b2*s0' j2*s1'
            __ASM_EMIT("vshufps             $0xb1, %%xmm0, %%xmm0, %%xmm0")                     // shift
            __ASM_EMIT("vaddps              %%xmm3, %%xmm6, %%xmm6")                            // xmm6 = d0' e0' d1' e1' = d1+a1*s0+b1*s0' e1+i1*s1+j1*s1' a2*s0+b2*s0' i2*s1+j2*s1'
            __ASM_EMIT("vmovss              %%xmm0, (%[dst])")
            __ASM_EMIT("add                 $0x04, %[src]")
            __ASM_EMIT("add                 $0x04, %[dst]")
            __ASM_EMIT("add                 $0x30, %[f]")
            __ASM_EMIT("dec                 %[count]")
            __ASM_EMIT("jnz                 1b")
            __ASM_EMIT("vmovups             %%xmm6, 0x00(%[d])")
            // Last step
            __ASM_EMIT("2:")
            __ASM_EMIT("vshufps             $0xb1, %%xmm0, %%xmm0, %%xmm0")                     // shift
            __ASM_EMIT("vmulss              0x04(%[f]), %%xmm0, %%xmm1")                        // xmm1 = a0*s
            __ASM_EMIT("vmulss              0x0c(%[f]), %%xmm0, %%xmm2")                        // xmm2 = a1*s
            __ASM_EMIT("vmulss              0x14(%[f]), %%xmm0, %%xmm3")                        // xmm3 = a2*s
            __ASM_EMIT("vaddss              0x04(%[d]), %%xmm1, %%xmm0")                        // xmm0 = s' = a0*s + d0
            __ASM_EMIT("vaddss              0x0c(%[d]), %%xmm2, %%xmm2")                        // xmm2 = d1 + a1*s
            __ASM_EMIT("vmulss              0x1c(%[f]), %%xmm0, %%xmm4")                        // xmm4 = b1*s'
            __ASM_EMIT("vmulss              0x24(%[f]), %%xmm0, %%xmm5")                        // xmm5 = b2*s'
            __ASM_EMIT("vaddss              %%xmm4, %%xmm2, %%xmm6")                            // xmm6 = d0' = d1 + a1*s + b1*s'
            __ASM_EMIT("vaddss              %%xmm5, %%xmm3, %%xmm7")                            // xmm7 = d1' = a2*s + b2*s'
            __ASM_EMIT("vmovss              %%xmm0, (%[dst])")
            __ASM_EMIT("vmovss              %%xmm6, 0x04(%[d])")
            __ASM_EMIT("vmovss              %%xmm7, 0x0c(%[d])")
            // Exit label
            __ASM_EMIT("4:")

            : [dst] "+r" (dst), [src] "+r" (src), [f] "+r" (f)
            : [count] "r" (count), [d] "r" (d)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void dyn_biquad_process_x2_fma3(float *dst, const float *src, float *d, size_t count, const biquad_x2_t *f)
    {
        ARCH_X86_ASM
        (
            // Check count
            __ASM_EMIT("test                %[count], %[count]")
            __ASM_EMIT("jz                  4f")

            // Start loop
            __ASM_EMIT("vmovss              (%[src]), %%xmm0")                                  // xmm0 = s ? ? ?
            __ASM_EMIT("vmulss              0x00(%[f]), %%xmm0, %%xmm1")                        // xmm1 = a0*s
            __ASM_EMIT("vmulss              0x08(%[f]), %%xmm0, %%xmm2")                        // xmm2 = a1*s
            __ASM_EMIT("vmulss              0x10(%[f]), %%xmm0, %%xmm3")                        // xmm3 = a2*s
            __ASM_EMIT("vaddss              0x00(%[d]), %%xmm1, %%xmm0")                        // xmm0 = s' = a0*s + d0
            __ASM_EMIT("vaddss              0x08(%[d]), %%xmm2, %%xmm2")                        // xmm2 = d1 + a1*s
            __ASM_EMIT("vfmadd231ss         0x20(%[f]), %%xmm0, %%xmm3")                        // xmm3 = d1' = a2*s + b2*s'
            __ASM_EMIT("vfmadd231ss         0x18(%[f]), %%xmm0, %%xmm2")                        // xmm2 = d0' = d1 + a1*s + b1*s'
            __ASM_EMIT("vshufps             $0xb1, %%xmm0, %%xmm0, %%xmm0")                     // shift
            __ASM_EMIT("add                 $0x04, %[src]")                                     // src++
            __ASM_EMIT("add                 $0x30, %[f]")
            __ASM_EMIT("dec                 %[count]")
            __ASM_EMIT("vmovss              %%xmm2, 0x00(%[d])")
            __ASM_EMIT("vmovss              %%xmm3, 0x08(%[d])")
            __ASM_EMIT("jz                  2f")
            // x2 loop
            __ASM_EMIT("vmovups             0x00(%[d]), %%xmm6")                                // xmm6 = d0 e0 d1 e1
            __ASM_EMIT("vxorps              %%xmm7, %%xmm7, %%xmm7")                            // xmm7 = 0 0 0 0
            __ASM_EMIT(".align              16")
            __ASM_EMIT("1:")
            __ASM_EMIT("vinsertps           $0xc0, (%[src]), %%xmm0, %%xmm0")                   // xmm0 = s0 s1
            __ASM_EMIT("vmovlhps            %%xmm0, %%xmm0, %%xmm0")                            // xmm0 = s0 s1 s0 s1
            __ASM_EMIT("vmulps              0x08(%[f]), %%xmm0, %%xmm2")                        // xmm2 = a1*s0 i1*s1 a2*s0 i2*s1
            __ASM_EMIT("vfmadd132ps         0x00(%[f]), %%xmm6, %%xmm0")                        // xmm0 = s0' s1' = d0+a0*s0 e0+i0*s1
            __ASM_EMIT("vshufps             $0x0e, %%xmm7, %%xmm6, %%xmm6")                     // xmm6 = d1 e1 0 0
            __ASM_EMIT("vmovlhps            %%xmm0, %%xmm0, %%xmm0")                            // xmm0 = s0' s1' s0' s1'
            __ASM_EMIT("vaddps              %%xmm2, %%xmm6, %%xmm6")                            // xmm6 = d1+a1*s0 e1+i1*s1 a2*s0 i2*s1
            __ASM_EMIT("vmulps              0x18(%[f]), %%xmm0, %%xmm3")                        // xmm3 = b1*s0' j1*s1' b2*s0' j2*s1'
            __ASM_EMIT("vshufps             $0xb1, %%xmm0, %%xmm0, %%xmm0")                     // shift
            __ASM_EMIT("vaddps              %%xmm3, %%xmm6, %%xmm6")                            // xmm6 = d0' e0' d1' e1' = d1+a1*s0+b1*s0' e1+i1*s1+j1*s1' a2*s0+b2*s0' i2*s1+j2*s1'
            __ASM_EMIT("vmovss              %%xmm0, (%[dst])")
            __ASM_EMIT("add                 $0x04, %[src]")
            __ASM_EMIT("add                 $0x04, %[dst]")
            __ASM_EMIT("add                 $0x30, %[f]")
            __ASM_EMIT("dec                 %[count]")
            __ASM_EMIT("jnz                 1b")
            __ASM_EMIT("vmovups             %%xmm6, 0x00(%[d])")
            // Last step
            __ASM_EMIT("2:")
            __ASM_EMIT("vshufps             $0xb1, %%xmm0, %%xmm0, %%xmm0")                     // shift
            __ASM_EMIT("vmulss              0x04(%[f]), %%xmm0, %%xmm1")                        // xmm1 = a0*s
            __ASM_EMIT("vmulss              0x0c(%[f]), %%xmm0, %%xmm2")                        // xmm2 = a1*s
            __ASM_EMIT("vmulss              0x14(%[f]), %%xmm0, %%xmm3")                        // xmm3 = a2*s
            __ASM_EMIT("vaddss              0x04(%[d]), %%xmm1, %%xmm0")                        // xmm0 = s' = a0*s + d0
            __ASM_EMIT("vaddss              0x0c(%[d]), %%xmm2, %%xmm2")                        // xmm2 = d1 + a1*s
            __ASM_EMIT("vfmadd231ss         0x24(%[f]), %%xmm0, %%xmm3")                        // xmm3 = d1' = a2*s + b2*s'
            __ASM_EMIT("vfmadd231ss         0x1c(%[f]), %%xmm0, %%xmm2")                        // xmm2 = d0' = d1 + a1*s + b1*s'
            __ASM_EMIT("vmovss              %%xmm0, (%[dst])")
            __ASM_EMIT("vmovss              %%xmm2, 0x04(%[d])")
            __ASM_EMIT("vmovss              %%xmm3, 0x0c(%[d])")
            // Exit label
            __ASM_EMIT("4:")

            : [dst] "+r" (dst), [src] "+r" (src), [f] "+r" (f)
            : [count] "r" (count), [d] "r" (d)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void dyn_biquad_process_x4(float *dst, const float *src, float *d, size_t count, const biquad_x4_t *f)
    {
        IF_ARCH_X86(
            float   MASK[4] __lsp_aligned16;
            size_t  mask;
        )

        ARCH_X86_ASM
        (
            // Check count
            __ASM_EMIT64("test              %[count], %[count]")
            __ASM_EMIT32NP("test            %[count], %[count]")
            __ASM_EMIT32P("cmpl             $0, %[count]")
            __ASM_EMIT("jz                  8f")

            // Initialize mask
            // xmm0=tmp, xmm1={s,s2[4]}, xmm2=p1[4], xmm3=p2[4], xmm6=d0[4], xmm7=d1[4]
            __ASM_EMIT("mov                 $1, %[mask]")
            __ASM_EMIT("vmovaps             %[X_MASK], %%xmm5")
            __ASM_EMIT("xorps               %%xmm1, %%xmm1")
            __ASM_EMIT("vmovaps             %%xmm5, %[MASK]")

            // Load delay buffer
            __ASM_EMIT("vmovups             0x00(%[d]), %%xmm6")                                // xmm6     = d0
            __ASM_EMIT("vmovups             0x10(%[d]), %%xmm7")                                // xmm7     = d1

            // Process first 3 steps
            __ASM_EMIT(".align 16")
            __ASM_EMIT("1:")
            __ASM_EMIT("vinsertps           $0x00, (%[src]), %%xmm0, %%xmm0")                   // xmm0     = s = *src
            __ASM_EMIT("add                 $4, %[src]")                                        // src      ++
            __ASM_EMIT("vmulps              0x00(%[f]), %%xmm0, %%xmm1")                        // xmm1     = a0*s
            __ASM_EMIT("vmulps              0x10(%[f]), %%xmm0, %%xmm2")                        // xmm2     = a1*s
            __ASM_EMIT("vmulps              0x20(%[f]), %%xmm0, %%xmm3")                        // xmm3     = a2*s
            __ASM_EMIT("vaddps              %%xmm6, %%xmm1, %%xmm0")                            // xmm0     = s' = a0*s + d0
            __ASM_EMIT("vaddps              %%xmm7, %%xmm2, %%xmm2")                            // xmm2     = d1 + a1*s
            __ASM_EMIT("vmulps              0x30(%[f]), %%xmm0, %%xmm4")                        // xmm4     = b1*s'
            __ASM_EMIT("vmulps              0x40(%[f]), %%xmm0, %%xmm5")                        // xmm5     = b2*s'
            __ASM_EMIT("vaddps              %%xmm4, %%xmm2, %%xmm2")                            // xmm2     = d0' = d1 + a1*s + b1*s'
            __ASM_EMIT("vaddps              %%xmm5, %%xmm3, %%xmm3")                            // xmm3     = d1' = a2*s + b2*s'
            __ASM_EMIT("vmovaps             %[MASK], %%xmm5")                                   // xmm5     = mask
            __ASM_EMIT("vshufps             $0x90, %%xmm0, %%xmm0, %%xmm0")                     // xmm0     = s2[0] s2[0] s2[1] s2[2]
            __ASM_EMIT("vblendvps           %%xmm5, %%xmm2, %%xmm6, %%xmm6")                    // xmm6     = (d0') & MASK | (d0 & ~MASK)
            __ASM_EMIT("vblendvps           %%xmm5, %%xmm3, %%xmm7, %%xmm7")                    // xmm7     = (d1') & MASK | (d0 & ~MASK)
            __ASM_EMIT("add                 $0x50, %[f]")
            __ASM_EMIT32("decl              %[count]")
            __ASM_EMIT64("dec               %[count]")
            __ASM_EMIT("jz                  4f")                                                // jump to completion
            __ASM_EMIT("vshufps             $0x90, %%xmm5, %%xmm5, %%xmm5")                     // xmm5     = m[0] m[0] m[1] m[2]
            __ASM_EMIT("lea                 0x01(,%[mask], 2), %[mask]")                        // mask     = (mask << 1) | 1
            __ASM_EMIT("vmovaps             %%xmm5, %[MASK]")                                   // store mask
            __ASM_EMIT("cmp                 $0x0f, %[mask]")
            __ASM_EMIT("jne                 1b")

            // 4x filter processing without mask
            __ASM_EMIT(".align 16")
            __ASM_EMIT("3:")
            __ASM_EMIT("vinsertps           $0x00, (%[src]), %%xmm0, %%xmm0")                   // xmm0     = *src
            __ASM_EMIT("add                 $4, %[src]")                                        // src      ++
            __ASM_EMIT("vmulps              0x00(%[f]), %%xmm0, %%xmm1")                        // xmm1     = a0*s
            __ASM_EMIT("vmulps              0x10(%[f]), %%xmm0, %%xmm2")                        // xmm2     = a1*s
            __ASM_EMIT("vmulps              0x20(%[f]), %%xmm0, %%xmm3")                        // xmm3     = a2*s
            __ASM_EMIT("vaddps              %%xmm6, %%xmm1, %%xmm0")                            // xmm0     = s' = a0*s + d0
            __ASM_EMIT("vaddps              %%xmm7, %%xmm2, %%xmm2")                            // xmm2     = d1 + a1*s
            __ASM_EMIT("vmulps              0x30(%[f]), %%xmm0, %%xmm4")                        // xmm4     = b1*s'
            __ASM_EMIT("vmulps              0x40(%[f]), %%xmm0, %%xmm5")                        // xmm5     = b2*s'
            __ASM_EMIT("vaddps              %%xmm4, %%xmm2, %%xmm6")                            // xmm6     = d0' = d1 + a1*s + b1*s'
            __ASM_EMIT("vshufps             $0x93, %%xmm0, %%xmm0, %%xmm0")                     // xmm0     = s2[0] s2[0] s2[1] s2[2]
            __ASM_EMIT("vaddps              %%xmm5, %%xmm3, %%xmm7")                            // xmm7     = d1' = a2*s + b2*s'
            __ASM_EMIT("vmovss              %%xmm0, (%[dst])")                                  // *dst     = s2[3]
            __ASM_EMIT("add                 $0x50, %[f]")
            __ASM_EMIT("add                 $4, %[dst]")                                        // dst      ++
            __ASM_EMIT32("decl              %[count]")
            __ASM_EMIT64("dec               %[count]")
            __ASM_EMIT("jnz                 3b")
            __ASM_EMIT("4:")
            // Prepare last loop
            __ASM_EMIT("vmovaps             %[MASK], %%xmm5")                                   // xmm5     = m[0] m[1] m[2] m[3]
            __ASM_EMIT("vxorps              %%xmm2, %%xmm2, %%xmm2")                            // xmm2     = 0 0 0 0
            __ASM_EMIT("vshufps             $0x90, %%xmm5, %%xmm5, %%xmm5")                     // xmm5     = m[0] m[0] m[1] m[2]
            __ASM_EMIT("shl                 $1, %[mask]")                                       // mask     = mask << 1
            __ASM_EMIT("vmovss              %%xmm2, %%xmm5, %%xmm5")                            // xmm0     = 0 m[0] m[1] m[2]
            __ASM_EMIT("and                 $0x0f, %[mask]")                                    // mask     = (mask << 1) & 0x0f
            __ASM_EMIT("vmovaps             %%xmm5, %[MASK]")

            // Process steps
            __ASM_EMIT(".align 16")
            __ASM_EMIT("5:")
            __ASM_EMIT("vmulps              0x00(%[f]), %%xmm0, %%xmm1")                        // xmm1     = a0*s
            __ASM_EMIT("vmulps              0x10(%[f]), %%xmm0, %%xmm2")                        // xmm2     = a1*s
            __ASM_EMIT("vmulps              0x20(%[f]), %%xmm0, %%xmm3")                        // xmm3     = a2*s
            __ASM_EMIT("vaddps              %%xmm6, %%xmm1, %%xmm0")                            // xmm0     = s' = a0*s + d0
            __ASM_EMIT("vaddps              %%xmm7, %%xmm2, %%xmm2")                            // xmm2     = d1 + a1*s
            __ASM_EMIT("vmulps              0x30(%[f]), %%xmm0, %%xmm4")                        // xmm4     = b1*s'
            __ASM_EMIT("vmulps              0x40(%[f]), %%xmm0, %%xmm5")                        // xmm5     = b2*s'
            __ASM_EMIT("vaddps              %%xmm4, %%xmm2, %%xmm2")                            // xmm2     = d0' = d1 + a1*s + b1*s'
            __ASM_EMIT("vaddps              %%xmm5, %%xmm3, %%xmm3")                            // xmm3     = d1' = a2*s + b2*s'
            __ASM_EMIT("vmovaps             %[MASK], %%xmm5")                                   // xmm5     = mask
            __ASM_EMIT("vshufps             $0x93, %%xmm0, %%xmm0, %%xmm0")                     // xmm0     = s2[3] s2[0] s2[1] s2[2]
            __ASM_EMIT("add                 $0x50, %[f]")
            __ASM_EMIT("test                $0x8, %[mask]")
            __ASM_EMIT("jz                  7f")
            __ASM_EMIT("vmovss              %%xmm0, (%[dst])")                                  // *dst     = s2[3]
            __ASM_EMIT("add                 $4, %[dst]")                                        // dst      ++
            __ASM_EMIT("7:")
            __ASM_EMIT("vblendvps           %%xmm5, %%xmm2, %%xmm6, %%xmm6")                    // xmm6     = (d0') & MASK | (d0 & ~MASK)
            __ASM_EMIT("vblendvps           %%xmm5, %%xmm3, %%xmm7, %%xmm7")                    // xmm7     = (d1') & MASK | (d0 & ~MASK)
            // Repeat loop
            __ASM_EMIT("shl                 $1, %[mask]")                                       // mask     = mask << 1
            __ASM_EMIT("vshufps             $0x90, %%xmm5, %%xmm5, %%xmm5")                     // xmm0     = m[0] m[0] m[1] m[2]
            __ASM_EMIT("and                 $0x0f, %[mask]")                                    // mask     = (mask << 1) & 0x0f
            __ASM_EMIT("vmovaps             %%xmm5, %[MASK]")
            __ASM_EMIT("jnz                 5b")                                                // check that mask is not zero

            // Store delay buffer
            __ASM_EMIT("vmovups             %%xmm6, 0x00(%[d])")                                // xmm6     = d0
            __ASM_EMIT("vmovups             %%xmm7, 0x10(%[d])")                                // xmm7     = d1
            __ASM_EMIT("8:")


            : [dst] "+r" (dst), [src] "+r" (src),
              [mask] "=&r"(mask), [f] "+r" (f),
              [count] __ASM_ARG_RW(count)
            : [d] "r" (d),
              [X_MASK] "m" (BQ_X4MASK),
              [MASK] "m" (MASK)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void dyn_biquad_process_x4_fma3(float *dst, const float *src, float *d, size_t count, const biquad_x4_t *f)
    {
        IF_ARCH_X86(
            float   MASK[4] __lsp_aligned16;
            size_t  mask;
        )

        ARCH_X86_ASM
        (
            // Check count
            __ASM_EMIT64("test              %[count], %[count]")
            __ASM_EMIT32NP("test            %[count], %[count]")
            __ASM_EMIT32P("cmpl             $0, %[count]")
            __ASM_EMIT("jz                  8f")

            // Initialize mask
            // xmm0=tmp, xmm1={s,s2[4]}, xmm2=p1[4], xmm3=p2[4], xmm6=d0[4], xmm7=d1[4]
            __ASM_EMIT("mov                 $1, %[mask]")
            __ASM_EMIT("vmovaps             %[X_MASK], %%xmm5")
            __ASM_EMIT("xorps               %%xmm1, %%xmm1")
            __ASM_EMIT("vmovaps             %%xmm5, %[MASK]")

            // Load delay buffer
            __ASM_EMIT("vmovups             0x00(%[d]), %%xmm6")                                // xmm6     = d0
            __ASM_EMIT("vmovups             0x10(%[d]), %%xmm7")                                // xmm7     = d1

            // Process first 3 steps
            __ASM_EMIT(".align 16")
            __ASM_EMIT("1:")
            __ASM_EMIT("vinsertps           $0x00, (%[src]), %%xmm0, %%xmm0")                   // xmm0     = s = *src
            __ASM_EMIT("add                 $4, %[src]")                                        // src      ++
            __ASM_EMIT("vmulps              0x00(%[f]), %%xmm0, %%xmm1")                        // xmm1     = a0*s
            __ASM_EMIT("vmulps              0x10(%[f]), %%xmm0, %%xmm2")                        // xmm2     = a1*s
            __ASM_EMIT("vmulps              0x20(%[f]), %%xmm0, %%xmm3")                        // xmm3     = a2*s
            __ASM_EMIT("vaddps              %%xmm6, %%xmm1, %%xmm0")                            // xmm0     = s' = a0*s + d0
            __ASM_EMIT("vaddps              %%xmm7, %%xmm2, %%xmm2")                            // xmm2     = d1 + a1*s
            __ASM_EMIT("vfmadd231ps         0x30(%[f]), %%xmm0, %%xmm2")                        // xmm2     = d0' = d1 + a1*s + b1*s'
            __ASM_EMIT("vfmadd231ps         0x40(%[f]), %%xmm0, %%xmm3")                        // xmm3     = d1' = a2*s + b2*s'
            __ASM_EMIT("vmovaps             %[MASK], %%xmm5")                                   // xmm5     = mask
            __ASM_EMIT("vshufps             $0x90, %%xmm0, %%xmm0, %%xmm0")                     // xmm0     = s2[0] s2[0] s2[1] s2[2]
            __ASM_EMIT("vblendvps           %%xmm5, %%xmm2, %%xmm6, %%xmm6")                    // xmm6     = (d0') & MASK | (d0 & ~MASK)
            __ASM_EMIT("vblendvps           %%xmm5, %%xmm3, %%xmm7, %%xmm7")                    // xmm7     = (d1') & MASK | (d0 & ~MASK)
            __ASM_EMIT("add                 $0x50, %[f]")
            __ASM_EMIT64("dec               %[count]")
            __ASM_EMIT32("decl              %[count]")
            __ASM_EMIT("jz                  4f")                                                // jump to completion
            __ASM_EMIT("vshufps             $0x90, %%xmm5, %%xmm5, %%xmm5")                     // xmm5     = m[0] m[0] m[1] m[2]
            __ASM_EMIT("lea                 0x01(,%[mask], 2), %[mask]")                        // mask     = (mask << 1) | 1
            __ASM_EMIT("vmovaps             %%xmm5, %[MASK]")                                   // store mask
            __ASM_EMIT("cmp                 $0x0f, %[mask]")
            __ASM_EMIT("jne                 1b")

            // 4x filter processing without mask
            __ASM_EMIT(".align 16")
            __ASM_EMIT("3:")
            __ASM_EMIT("vinsertps           $0x00, (%[src]), %%xmm0, %%xmm0")                   // xmm0     = *src
            __ASM_EMIT("add                 $4, %[src]")                                        // src      ++
            __ASM_EMIT("vmulps              0x10(%[f]), %%xmm0, %%xmm2")                        // xmm2     = a1*s
            __ASM_EMIT("vmulps              0x20(%[f]), %%xmm0, %%xmm3")                        // xmm3     = a2*s
            __ASM_EMIT("vfmadd132ps         0x00(%[f]), %%xmm6, %%xmm0")                        // xmm0     = s' = a0*s + d0
            __ASM_EMIT("vaddps              %%xmm7, %%xmm2, %%xmm6")                            // xmm6     = d1 + a1*s
            __ASM_EMIT("vfmadd231ps         0x40(%[f]), %%xmm0, %%xmm3")                        // xmm3     = d1' = a2*s + b2*s'
            __ASM_EMIT("vfmadd231ps         0x30(%[f]), %%xmm0, %%xmm6")                        // xmm6     = d0' = d1 + a1*s + b1*s'
            __ASM_EMIT("vmovaps             %%xmm3, %%xmm7")                                    // xmm7     = d1' = a2*s + b2*s'
            __ASM_EMIT("vshufps             $0x93, %%xmm0, %%xmm0, %%xmm0")                     // xmm0     = s2[0] s2[0] s2[1] s2[2]
            __ASM_EMIT("vmovss              %%xmm0, (%[dst])")                                  // *dst     = s2[3]
            __ASM_EMIT("add                 $0x50, %[f]")
            __ASM_EMIT("add                 $4, %[dst]")                                        // dst      ++
            __ASM_EMIT64("dec               %[count]")
            __ASM_EMIT32("decl              %[count]")
            __ASM_EMIT("jnz                 3b")
            __ASM_EMIT("4:")
            // Prepare last loop
            __ASM_EMIT("vmovaps             %[MASK], %%xmm5")                                   // xmm5     = m[0] m[1] m[2] m[3]
            __ASM_EMIT("vxorps              %%xmm2, %%xmm2, %%xmm2")                            // xmm2     = 0 0 0 0
            __ASM_EMIT("vshufps             $0x90, %%xmm5, %%xmm5, %%xmm5")                     // xmm5     = m[0] m[0] m[1] m[2]
            __ASM_EMIT("shl                 $1, %[mask]")                                       // mask     = mask << 1
            __ASM_EMIT("vmovss              %%xmm2, %%xmm5, %%xmm5")                            // xmm0     = 0 m[0] m[1] m[2]
            __ASM_EMIT("and                 $0x0f, %[mask]")                                    // mask     = (mask << 1) & 0x0f
            __ASM_EMIT("vmovaps             %%xmm5, %[MASK]")

            // Process steps
            __ASM_EMIT(".align 16")
            __ASM_EMIT("5:")
            __ASM_EMIT("vmulps              0x00(%[f]), %%xmm0, %%xmm1")                        // xmm1     = a0*s
            __ASM_EMIT("vmulps              0x10(%[f]), %%xmm0, %%xmm2")                        // xmm2     = a1*s
            __ASM_EMIT("vmulps              0x20(%[f]), %%xmm0, %%xmm3")                        // xmm3     = a2*s
            __ASM_EMIT("vaddps              %%xmm6, %%xmm1, %%xmm0")                            // xmm0     = s' = a0*s + d0
            __ASM_EMIT("vaddps              %%xmm7, %%xmm2, %%xmm2")                            // xmm2     = d1 + a1*s
            __ASM_EMIT("vfmadd231ps         0x30(%[f]), %%xmm0, %%xmm2")                        // xmm2     = d0' = d1 + a1*s + b1*s'
            __ASM_EMIT("vfmadd231ps         0x40(%[f]), %%xmm0, %%xmm3")                        // xmm3     = d1' = a2*s + b2*s'
            __ASM_EMIT("vmovaps             %[MASK], %%xmm5")                                   // xmm5     = mask
            __ASM_EMIT("vshufps             $0x93, %%xmm0, %%xmm0, %%xmm0")                     // xmm0     = s2[3] s2[0] s2[1] s2[2]
            __ASM_EMIT("add                 $0x50, %[f]")
            __ASM_EMIT("test                $0x8, %[mask]")
            __ASM_EMIT("jz                  7f")
            __ASM_EMIT("vmovss              %%xmm0, (%[dst])")                                  // *dst     = s2[3]
            __ASM_EMIT("add                 $4, %[dst]")                                        // dst      ++
            __ASM_EMIT("7:")
            __ASM_EMIT("vblendvps           %%xmm5, %%xmm2, %%xmm6, %%xmm6")                    // xmm6     = (d0') & MASK | (d0 & ~MASK)
            __ASM_EMIT("vblendvps           %%xmm5, %%xmm3, %%xmm7, %%xmm7")                    // xmm7     = (d1') & MASK | (d0 & ~MASK)
            // Repeat loop
            __ASM_EMIT("shl                 $1, %[mask]")                                       // mask     = mask << 1
            __ASM_EMIT("vshufps             $0x90, %%xmm5, %%xmm5, %%xmm5")                     // xmm0     = m[0] m[0] m[1] m[2]
            __ASM_EMIT("and                 $0x0f, %[mask]")                                    // mask     = (mask << 1) & 0x0f
            __ASM_EMIT("vmovaps             %%xmm5, %[MASK]")
            __ASM_EMIT("jnz                 5b")                                                // check that mask is not zero

            // Store delay buffer
            __ASM_EMIT("vmovups             %%xmm6, 0x00(%[d])")                                // xmm6     = d0
            __ASM_EMIT("vmovups             %%xmm7, 0x10(%[d])")                                // xmm7     = d1
            __ASM_EMIT("8:")

            : [dst] "+r" (dst), [src] "+r" (src),
              [mask] "=&r"(mask), [f] "+r" (f),
              [count] __ASM_ARG_RW(count)
            : [d] "r" (d),
              [X_MASK] "m" (BQ_X4MASK),
              [MASK] "m" (MASK)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    // This function is tested, works and delivers high performance
    void x64_dyn_biquad_process_x8(float *dst, const float *src, float *d, size_t count, const biquad_x8_t *f)
    {
        IF_ARCH_X86_64(size_t mask);
        ARCH_X86_64_ASM
        (
            // Check count
            __ASM_EMIT64("test          %[count], %[count]")
            __ASM_EMIT32NP("test        %[count], %[count]")
            __ASM_EMIT32P("cmpl         $0, %[count]")
            __ASM_EMIT("jz              8f")

            // Initialize mask
            // ymm0=tmp, ymm1={s,s2[8]}, ymm2=p1[8], ymm3=p2[8], ymm6=d0[8], ymm7=d1[8], ymm8=mask[8]
            __ASM_EMIT("mov             $1, %[mask]")
            __ASM_EMIT("vmovaps         %[X_MASK], %%ymm8")                             // ymm8     = m
            __ASM_EMIT("vxorps          %%ymm1, %%ymm1, %%ymm1")                        // ymm1     = 0

            // Load delay buffer
            __ASM_EMIT("vmovups         0x00(%[d]), %%ymm6")                            // ymm6     = d0
            __ASM_EMIT("vmovups         0x20(%[d]), %%ymm7")                            // ymm7     = d1

            // Process first 3 steps
            __ASM_EMIT("1:")
            __ASM_EMIT("vmovss          (%[src]), %%xmm0")                              // xmm0     = *src
            __ASM_EMIT("add             $4, %[src]")                                    // src      ++
            __ASM_EMIT("vblendps        $0x01, %%ymm0, %%ymm1, %%ymm1")                 // ymm1     = s
            __ASM_EMIT("vmulps          0x20(%[f]), %%ymm1, %%ymm2")                    // ymm2     = s*a1
            __ASM_EMIT("vmulps          0x40(%[f]), %%ymm1, %%ymm3")                    // ymm3     = s*a2
            __ASM_EMIT("vmulps          0x00(%[f]), %%ymm1, %%ymm1")                    // ymm1     = s*a0
            __ASM_EMIT("vaddps          %%ymm6, %%ymm1, %%ymm1")                        // ymm1     = s*a0+d0 = s2
            __ASM_EMIT("vmulps          0x60(%[f]), %%ymm1, %%ymm4")                    // ymm4     = s2*b1
            __ASM_EMIT("vmulps          0x80(%[f]), %%ymm1, %%ymm5")                    // ymm5     = s2*b2
            __ASM_EMIT("vaddps          %%ymm4, %%ymm2, %%ymm2")                        // ymm2     = s*a1 + s2*b1 = p1
            __ASM_EMIT("vaddps          %%ymm5, %%ymm3, %%ymm3")                        // ymm3     = s*a2 + s2*b2 = p2
            __ASM_EMIT("vaddps          %%ymm7, %%ymm2, %%ymm2")                        // ymm2     = p1 + d1

            // Rotate buffer
            __ASM_EMIT("vpermilps       $0x93, %%ymm1, %%ymm1")                         // ymm1     = s2[3] s2[0] s2[1] s2[2] s2[7] s2[4] s2[5] s2[6]
            __ASM_EMIT("vblendvps       %%ymm8, %%ymm2, %%ymm6, %%ymm6")                // ymm6     = (p1 + d1) & MASK | (d0 & ~MASK)
            __ASM_EMIT("vperm2f128      $0x01, %%ymm1, %%ymm1, %%ymm0")                 // ymm0     = s2[7] s2[4] s2[5] s2[6] s2[3] s2[0] s2[1] s2[2]
            __ASM_EMIT("vblendvps       %%ymm8, %%ymm3, %%ymm7, %%ymm7")                // ymm7     = (p2 & MASK) | (d1 & ~MASK)
            __ASM_EMIT("vblendps        $0x11, %%ymm0, %%ymm1, %%ymm1")                 // ymm1     = s2[7] s2[0] s2[1] s2[2] s2[3] s2[4] s2[5] s2[6]

            // Repeat loop
            __ASM_EMIT("add             $0xa0, %[f]")                                   // f++
            __ASM_EMIT64("dec           %[count]")
            __ASM_EMIT32("decl          %[count]")
            __ASM_EMIT("jz              4f")                                            // jump to completion
            __ASM_EMIT("lea             0x01(,%[mask], 2), %[mask]")                    // mask     = (mask << 1) | 1
            __ASM_EMIT("vpermilps       $0x93, %%ymm8, %%ymm8")                         // ymm8     =  m[3]  m[0]  m[1]  m[2]  m[7]  m[4]  m[5]  m[6]
            __ASM_EMIT("vperm2f128      $0x01, %%ymm8, %%ymm8, %%ymm3")                 // ymm3     =  m[7]  m[4]  m[5]  m[6]  m[3]  m[0]  m[1]  m[2]
            __ASM_EMIT("vblendps        $0x11, %%ymm3, %%ymm8, %%ymm8")                 // ymm8     =  m[7]  m[0]  m[1]  m[2]  m[3]  m[4]  m[5]  m[6]
            __ASM_EMIT("vorps           %[X_MASK], %%ymm8, %%ymm8")                     // ymm8     =  m[0]  m[0]  m[1]  m[2]  m[3]  m[4]  m[5]  m[6]
            __ASM_EMIT("cmp             $0xff, %[mask]")
            __ASM_EMIT("jne             1b")

            // 8x filter processing without mask
            __ASM_EMIT(".align 16")
            __ASM_EMIT("3:")
            __ASM_EMIT("vmovss          (%[src]), %%xmm0")                              // xmm0     = *src
            __ASM_EMIT("add             $4, %[src]")                                    // src      ++
            __ASM_EMIT("vblendps        $0x01, %%ymm0, %%ymm1, %%ymm1")                 // ymm1     = s
            __ASM_EMIT("vmulps          0x20(%[f]), %%ymm1, %%ymm2")                    // ymm2     = s*a1
            __ASM_EMIT("vmulps          0x40(%[f]), %%ymm1, %%ymm3")                    // ymm3     = s*a2
            __ASM_EMIT("vmulps          0x00(%[f]), %%ymm1, %%ymm1")                    // ymm1     = s*a0
            __ASM_EMIT("vaddps          %%ymm6, %%ymm1, %%ymm1")                        // ymm1     = s*a0+d0 = s2
            __ASM_EMIT("vmulps          0x60(%[f]), %%ymm1, %%ymm4")                    // ymm4     = s2*b1
            __ASM_EMIT("vmulps          0x80(%[f]), %%ymm1, %%ymm5")                    // ymm5     = s2*b2
            __ASM_EMIT("vaddps          %%ymm4, %%ymm2, %%ymm2")                        // ymm2     = s*a1 + s2*b1 = p1
            __ASM_EMIT("vpermilps       $0x93, %%ymm1, %%ymm1")                         // ymm1     = s2[3] s2[0] s2[1] s2[2] s2[7] s2[4] s2[5] s2[6]
            __ASM_EMIT("vaddps          %%ymm7, %%ymm2, %%ymm6")                        // ymm6     = p1 + d1
            __ASM_EMIT("vperm2f128      $0x01, %%ymm1, %%ymm1, %%ymm0")                 // ymm0     = s2[7] s2[4] s2[5] s2[6] s2[3] s2[0] s2[1] s2[2]
            __ASM_EMIT("vaddps          %%ymm5, %%ymm3, %%ymm7")                        // ymm7     = s*a2 + s2*b2 = p2
            __ASM_EMIT("vblendps        $0x11, %%ymm0, %%ymm1, %%ymm1")                 // ymm1     = s2[7] s2[0] s2[1] s2[2] s2[3] s2[4] s2[5] s2[6]
            __ASM_EMIT("vmovss          %%xmm1, (%[dst])")                              // *dst     = s2[7]

            // Repeat loop
            __ASM_EMIT("add             $0xa0, %[f]")                                   // f++
            __ASM_EMIT("add             $4, %[dst]")                                    // dst      ++
            __ASM_EMIT64("dec           %[count]")
            __ASM_EMIT32("decl          %[count]")
            __ASM_EMIT("jnz             3b")

            // Prepare last loop, shift mask
            __ASM_EMIT("4:")
            __ASM_EMIT("vxorps          %%ymm2, %%ymm2, %%ymm2")                        // ymm2     =  0
            __ASM_EMIT("vpermilps       $0x93, %%ymm8, %%ymm8")                         // ymm8     =  m[3]  m[0]  m[1]  m[2]  m[7]  m[4]  m[5]  m[6]
            __ASM_EMIT("vinsertf128     $0x01, %%xmm8, %%ymm2, %%ymm2")                 // ymm2     =  0     0     0     0     m[3]  m[0]  m[1]  m[2]
            __ASM_EMIT("vblendps        $0x11, %%ymm2, %%ymm8, %%ymm8")                 // ymm8     =  0     m[0]  m[1]  m[2]  m[3]  m[4]  m[5]  m[6]
            __ASM_EMIT("shl             $1, %[mask]")                                   // mask     = mask << 1

            // Process steps
            __ASM_EMIT(".align 16")
            __ASM_EMIT("5:")
            __ASM_EMIT("vmulps          0x20(%[f]), %%ymm1, %%ymm2")                    // ymm2     = s*a1
            __ASM_EMIT("vmulps          0x40(%[f]), %%ymm1, %%ymm3")                    // ymm3     = s*a2
            __ASM_EMIT("vmulps          0x00(%[f]), %%ymm1, %%ymm1")                    // ymm1     = s*a0
            __ASM_EMIT("vaddps          %%ymm6, %%ymm1, %%ymm1")                        // ymm1     = s*a0+d0 = s2
            __ASM_EMIT("vmulps          0x60(%[f]), %%ymm1, %%ymm4")                    // ymm4     = s2*b1
            __ASM_EMIT("vmulps          0x80(%[f]), %%ymm1, %%ymm5")                    // ymm5     = s2*b2
            __ASM_EMIT("vaddps          %%ymm4, %%ymm2, %%ymm2")                        // ymm2     = s*a1 + s2*b1 = p1
            __ASM_EMIT("vaddps          %%ymm5, %%ymm3, %%ymm3")                        // ymm3     = s*a2 + s2*b2 = p2
            __ASM_EMIT("vaddps          %%ymm7, %%ymm2, %%ymm2")                        // ymm2     = p1 + d1

            // Update delay only by mask
            __ASM_EMIT("vblendvps       %%ymm8, %%ymm2, %%ymm6, %%ymm6")                // ymm6     = (p1 + d1) & MASK | (d0 & ~MASK)
            __ASM_EMIT("vblendvps       %%ymm8, %%ymm3, %%ymm7, %%ymm7")                // ymm7     = (p2 & MASK) | (d1 & ~MASK)

            // Rotate buffer and mask, AVX2 has better option for it
            __ASM_EMIT("add             $0xa0, %[f]")                                   // f++
            __ASM_EMIT("vpermilps       $0x93, %%ymm1, %%ymm1")                         // ymm1     = s2[3] s2[0] s2[1] s2[2] s2[7] s2[4] s2[5] s2[6]
            __ASM_EMIT("vpermilps       $0x93, %%ymm8, %%ymm8")                         // ymm8     =  m[3]  m[0]  m[1]  m[2]  m[7]  m[4]  m[5]  m[6]
            __ASM_EMIT("vperm2f128      $0x01, %%ymm1, %%ymm1, %%ymm0")                 // ymm0     = s2[7] s2[4] s2[5] s2[6] s2[3] s2[0] s2[1] s2[2]
            __ASM_EMIT("vxorps          %%ymm2, %%ymm2, %%ymm2")                        // ymm2     =  0
            __ASM_EMIT("vblendps        $0x11, %%ymm0, %%ymm1, %%ymm1")                 // ymm1     = s2[7] s2[0] s2[1] s2[2] s2[3] s2[4] s2[5] s2[6]
            __ASM_EMIT("vinsertf128     $0x01, %%xmm8, %%ymm2, %%ymm2")                 // ymm2     =  0     0     0     0     m[3]  m[0]  m[1]  m[2]
            __ASM_EMIT("vblendps        $0x11, %%ymm2, %%ymm8, %%ymm8")                 // ymm1     =  0     m[0]  m[1]  m[2]  m[3]  m[4]  m[5]  m[6]
            __ASM_EMIT("test            $0x80, %[mask]")
            __ASM_EMIT("jz              6f")
            __ASM_EMIT("vmovss          %%xmm1, (%[dst])")                              // *dst     = s2[7]
            __ASM_EMIT("add             $4, %[dst]")                                    // dst      ++
            __ASM_EMIT("6:")

            // Repeat loop
            __ASM_EMIT("shl             $1, %[mask]")                                   // mask     = mask << 1
            __ASM_EMIT("and             $0xff, %[mask]")                                // mask     = (mask << 1) & 0xff
            __ASM_EMIT("jnz             5b")                                            // check that mask is not zero

            // Store delay buffer
            __ASM_EMIT("vmovups         %%ymm6, 0x00(%[d])")                            // *d0      = %%ymm6
            __ASM_EMIT("vmovups         %%ymm7, 0x20(%[d])")                            // *d1      = %%ymm7

            // Exit label
            __ASM_EMIT("8:")

            : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count), [f] "+r" (f),
              [mask] "=&r"(mask)
            : [d] "r" (d),
              [X_MASK] "m" (X_MASK0001)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7",
              "%xmm8"
        );
    }

    // This function is FMA3 implementation of biquad_process_x8
    void dyn_biquad_process_x8_fma3(float *dst, const float *src, float *d, size_t count, const biquad_x8_t *f)
    {
        IF_ARCH_X86(size_t mask);
        ARCH_X86_ASM
        (
            // Check count
            __ASM_EMIT64("test          %[count], %[count]")
            __ASM_EMIT32NP("test        %[count], %[count]")
            __ASM_EMIT32P("cmpl         $0, %[count]")
            __ASM_EMIT("jz              8f")

            // Initialize mask
            // ymm0=tmp, ymm1={s,s2[8]}, ymm2=p1[8], ymm3=p2[8], ymm6=d0[8], ymm7=d1[8], ymm5=mask[8]
            __ASM_EMIT("mov             $1, %[mask]")
            __ASM_EMIT("vmovaps         %[X_MASK], %%ymm5")                             // ymm5     = m
            __ASM_EMIT("vxorps          %%ymm1, %%ymm1, %%ymm1")                        // ymm1     = 0

            // Load delay buffer
            __ASM_EMIT("vmovups         0x00(%[d]), %%ymm6")                            // ymm6     = d0
            __ASM_EMIT("vmovups         0x20(%[d]), %%ymm7")                            // ymm7     = d1

            // Process first 3 steps
            __ASM_EMIT("1:")
            __ASM_EMIT("vmovss          (%[src]), %%xmm0")                              // xmm0     = *src
            __ASM_EMIT("add             $4, %[src]")                                    // src      ++
            __ASM_EMIT("vblendps        $0x01, %%ymm0, %%ymm1, %%ymm1")                 // ymm1     = s
            __ASM_EMIT("vmulps          0x20(%[f]), %%ymm1, %%ymm2")                    // ymm2     = s*a1
            __ASM_EMIT("vmulps          0x40(%[f]), %%ymm1, %%ymm3")                    // ymm3     = s*a2
            __ASM_EMIT("vfmadd132ps     0x00(%[f]), %%ymm6, %%ymm1")                    // ymm1     = s*a0+d0 = s2
            __ASM_EMIT("vfmadd231ps     0x60(%[f]), %%ymm1, %%ymm2")                    // ymm2     = s*a1 + s2*b1 = p1
            __ASM_EMIT("vfmadd231ps     0x80(%[f]), %%ymm1, %%ymm3")                    // ymm3     = s*a2 + s2*b2 = p2
            __ASM_EMIT("vaddps          %%ymm7, %%ymm2, %%ymm2")                        // ymm2     = p1 + d1

            // Update delay only by mask
            __ASM_EMIT("vpermilps       $0x93, %%ymm1, %%ymm1")                         // ymm1     = s2[3] s2[0] s2[1] s2[2] s2[7] s2[4] s2[5] s2[6]
            __ASM_EMIT("vblendvps       %%ymm5, %%ymm2, %%ymm6, %%ymm6")                // ymm6     = (p1 + d1) & MASK | (d0 & ~MASK)
            __ASM_EMIT("vperm2f128      $0x01, %%ymm1, %%ymm1, %%ymm0")                 // ymm0     = s2[7] s2[4] s2[5] s2[6] s2[3] s2[0] s2[1] s2[2]
            __ASM_EMIT("vblendvps       %%ymm5, %%ymm3, %%ymm7, %%ymm7")                // ymm7     = (p2 & MASK) | (d1 & ~MASK)
            __ASM_EMIT("vblendps        $0x11, %%ymm0, %%ymm1, %%ymm1")                 // ymm1     = s2[7] s2[0] s2[1] s2[2] s2[3] s2[4] s2[5] s2[6]

            // Repeat loop
            __ASM_EMIT("add             $0xa0, %[f]")                                   // f++
            __ASM_EMIT64("dec           %[count]")
            __ASM_EMIT32("decl          %[count]")
            __ASM_EMIT("jz              4f")                                            // jump to completion
            __ASM_EMIT("lea             0x01(,%[mask], 2), %[mask]")                    // mask     = (mask << 1) | 1
            __ASM_EMIT("vpermilps       $0x93, %%ymm5, %%ymm5")                         // ymm5     =  m[3]  m[0]  m[1]  m[2]  m[7]  m[4]  m[5]  m[6]
            __ASM_EMIT("vextractf128    $0x01, %%ymm5, %%xmm3")                         // ymm3     =  m[7]  m[4]  m[5]  m[6] ? ? ? ?
            __ASM_EMIT("vinsertf128     $0x01, %%xmm5, %%ymm3, %%ymm3")                 // ymm3     =  m[7]  m[4]  m[5]  m[6]  m[3]  m[0]  m[1]  m[2]
            __ASM_EMIT("vblendps        $0x11, %%ymm3, %%ymm5, %%ymm5")                 // ymm5     =  m[7]  m[0]  m[1]  m[2]  m[3]  m[4]  m[5]  m[6]
            __ASM_EMIT("vorps           %[X_MASK], %%ymm5, %%ymm5")                     // ymm5     =  m[0]  m[0]  m[1]  m[2]  m[3]  m[4]  m[5]  m[6]
            __ASM_EMIT("cmp             $0xff, %[mask]")
            __ASM_EMIT("jne             1b")

            // 8x filter processing without mask
            __ASM_EMIT(".align 16")
            __ASM_EMIT("3:")
            __ASM_EMIT("vmovss          (%[src]), %%xmm0")                              // xmm0     = *src
            __ASM_EMIT("add             $4, %[src]")                                    // src      ++
            __ASM_EMIT("vblendps        $0x01, %%ymm0, %%ymm1, %%ymm1")                 // ymm1     = s
            __ASM_EMIT("vmulps          0x20(%[f]), %%ymm1, %%ymm2")                    // ymm2     = s*a1
            __ASM_EMIT("vmulps          0x40(%[f]), %%ymm1, %%ymm3")                    // ymm3     = s*a2
            __ASM_EMIT("vfmadd132ps     0x00(%[f]), %%ymm6, %%ymm1")                    // ymm1     = s*a0+d0 = s2
            __ASM_EMIT("vfmadd231ps     0x60(%[f]), %%ymm1, %%ymm2")                    // ymm2     = s*a1 + s2*b1 = p1
            __ASM_EMIT("vfmadd231ps     0x80(%[f]), %%ymm1, %%ymm3")                    // ymm3     = s*a2 + s2*b2 = p2
            __ASM_EMIT("vaddps          %%ymm7, %%ymm2, %%ymm6")                        // ymm6     = p1 + d1
            __ASM_EMIT("vpermilps       $0x93, %%ymm1, %%ymm1")                         // ymm1     = s2[3] s2[0] s2[1] s2[2] s2[7] s2[4] s2[5] s2[6]
            __ASM_EMIT("vmovaps         %%ymm3, %%ymm7")                                // ymm7     = s*a2 + s2*b2 = p2
            __ASM_EMIT("vextractf128    $0x01, %%ymm1, %%xmm0")                         // ymm0     = s2[7] s2[4] s2[5] s2[6] ? ? ? ?
            __ASM_EMIT("vinsertf128     $0x01, %%xmm1, %%ymm0, %%ymm0")                 // ymm0     = s2[7] s2[4] s2[5] s2[6] s2[3] s2[0] s2[1] s2[2]
            __ASM_EMIT("vblendps        $0x11, %%ymm0, %%ymm1, %%ymm1")                 // ymm1     = s2[7] s2[0] s2[1] s2[2] s2[3] s2[4] s2[5] s2[6]
            __ASM_EMIT("vmovss          %%xmm1, (%[dst])")                              // *dst     = s2[7]

            // Repeat loop
            __ASM_EMIT("add             $4, %[dst]")                                    // dst      ++
            __ASM_EMIT64("dec           %[count]")
            __ASM_EMIT32("decl          %[count]")
            __ASM_EMIT("jnz             3b")

            // Prepare last loop, shift mask
            __ASM_EMIT("4:")
            __ASM_EMIT("vxorps          %%ymm2, %%ymm2, %%ymm2")                        // ymm2     =  0
            __ASM_EMIT("vpermilps       $0x93, %%ymm5, %%ymm5")                         // ymm5     =  m[3]  m[0]  m[1]  m[2]  m[7]  m[4]  m[5]  m[6]
            __ASM_EMIT("vinsertf128     $0x01, %%xmm5, %%ymm2, %%ymm2")                 // ymm2     =  0     0     0     0     m[3]  m[0]  m[1]  m[2]
            __ASM_EMIT("vblendps        $0x11, %%ymm2, %%ymm5, %%ymm5")                 // ymm5     =  0     m[0]  m[1]  m[2]  m[3]  m[4]  m[5]  m[6]
            __ASM_EMIT("shl             $1, %[mask]")                                   // mask     = mask << 1

            // Process steps
            __ASM_EMIT("5:")
            __ASM_EMIT("vmulps          0x20(%[f]), %%ymm1, %%ymm2")                    // ymm2     = s*a1
            __ASM_EMIT("vmulps          0x40(%[f]), %%ymm1, %%ymm3")                    // ymm3     = s*a2
            __ASM_EMIT("vfmadd132ps     0x00(%[f]), %%ymm6, %%ymm1")                    // ymm1     = s*a0+d0 = s2
            __ASM_EMIT("vfmadd231ps     0x60(%[f]), %%ymm1, %%ymm2")                    // ymm2     = s*a1 + s2*b1 = p1
            __ASM_EMIT("vfmadd231ps     0x80(%[f]), %%ymm1, %%ymm3")                    // ymm3     = s*a2 + s2*b2 = p2
            __ASM_EMIT("vaddps          %%ymm7, %%ymm2, %%ymm2")                        // ymm2     = p1 + d1

            // Update delay only by mask
            __ASM_EMIT("vblendvps       %%ymm5, %%ymm2, %%ymm6, %%ymm6")                // ymm6     = (p1 + d1) & MASK | (d0 & ~MASK)
            __ASM_EMIT("vblendvps       %%ymm5, %%ymm3, %%ymm7, %%ymm7")                // ymm7     = (p2 & MASK) | (d1 & ~MASK)

            // Rotate buffer and mask, AVX2 has better option for it
            __ASM_EMIT("add             $0xa0, %[f]")                                   // f++
            __ASM_EMIT("vpermilps       $0x93, %%ymm1, %%ymm1")                         // ymm1     = s2[3] s2[0] s2[1] s2[2] s2[7] s2[4] s2[5] s2[6]
            __ASM_EMIT("vpermilps       $0x93, %%ymm5, %%ymm5")                         // ymm5     =  m[3]  m[0]  m[1]  m[2]  m[7]  m[4]  m[5]  m[6]
            __ASM_EMIT("vperm2f128      $0x01, %%ymm1, %%ymm1, %%ymm0")                 // ymm0     = s2[7] s2[4] s2[5] s2[6] s2[3] s2[0] s2[1] s2[2]
            __ASM_EMIT("vxorps          %%ymm2, %%ymm2, %%ymm2")                        // ymm2     =  0
            __ASM_EMIT("vblendps        $0x11, %%ymm0, %%ymm1, %%ymm1")                 // ymm1     = s2[7] s2[0] s2[1] s2[2] s2[3] s2[4] s2[5] s2[6]
            __ASM_EMIT("vinsertf128     $0x01, %%xmm5, %%ymm2, %%ymm2")                 // ymm2     =  0     0     0     0     m[3]  m[0]  m[1]  m[2]
            __ASM_EMIT("vblendps        $0x11, %%ymm2, %%ymm5, %%ymm5")                 // ymm1     =  0     m[0]  m[1]  m[2]  m[3]  m[4]  m[5]  m[6]
            __ASM_EMIT("test            $0x80, %[mask]")
            __ASM_EMIT("jz              6f")
            __ASM_EMIT("vmovss          %%xmm1, (%[dst])")                              // *dst     = s2[7]
            __ASM_EMIT("add             $4, %[dst]")                                    // dst      ++
            __ASM_EMIT("6:")

            // Repeat loop
            __ASM_EMIT("shl             $1, %[mask]")                                   // mask     = mask << 1
            __ASM_EMIT("and             $0xff, %[mask]")                                // mask     = (mask << 1) & 0xff
            __ASM_EMIT("jnz             5b")                                            // check that mask is not zero

            // Store delay buffer
            __ASM_EMIT("vmovups         %%ymm6, 0x00(%[d])")                            // *d0      = %%ymm6
            __ASM_EMIT("vmovups         %%ymm7, 0x20(%[d])")                            // *d1      = %%ymm7

            // Exit label
            __ASM_EMIT("8:")

            : [dst] "+r" (dst), [src] "+r" (src), [f] "+r" (f),
              [mask] "=&r"(mask),
              [count] __ASM_ARG_RW(count)
            : [d] "r" (d),
              [X_MASK] "m" (X_MASK0001)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

}

#endif /* DSP_ARCH_X86_AVX_DYN_FILTERS_H_ */
