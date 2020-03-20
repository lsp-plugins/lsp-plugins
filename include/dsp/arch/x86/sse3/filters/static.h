/*
 * static.h
 *
 *  Created on: 01 сен. 2016 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_SSE3_FILTERS_STATIC_H_
#define DSP_ARCH_X86_SSE3_FILTERS_STATIC_H_

#ifndef DSP_ARCH_X86_SSE3_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_SSE3_IMPL */

namespace sse3
{
    void x64_biquad_process_x8(float *dst, const float *src, size_t count, biquad_t *f)
    {
        IF_ARCH_X86_64(
            float MASK[8] __lsp_aligned16;
            size_t mask;
        )

        ARCH_X86_64_ASM
        (
            // Check count
            __ASM_EMIT("test        %[count], %[count]")
            __ASM_EMIT("jz          8f")

            // Initialize mask
            // xmm0=tmp,  xmm1 ={s,s2[4]}, xmm2 = p1[4], xmm3 = p2[4], xmm6 = d0[4], xmm7 = d2[4]
            // xmm8=mask, xmm9 ={r,r2[4]}, xmm10= q1[4], xmm11= q2[4], xmm14= d1[4], xmm15= d3[4]
            __ASM_EMIT("mov         $1, %[mask]")
            __ASM_EMIT("movaps      %[X_MASK], %%xmm0")
            __ASM_EMIT("xorps       %%xmm1, %%xmm1")
            __ASM_EMIT("movaps      %%xmm0, 0x00 + %[MASK]")
            __ASM_EMIT("movaps      %%xmm1, 0x10 + %[MASK]")

            // Load delay buffer
            __ASM_EMIT("movaps      0x00(%[f]), %%xmm6")                        // xmm6     = d0
            __ASM_EMIT("movaps      0x10(%[f]), %%xmm14")                       // xmm14    = d1
            __ASM_EMIT("movaps      0x20(%[f]), %%xmm7")                        // xmm7     = d2
            __ASM_EMIT("movaps      0x30(%[f]), %%xmm15")                       // xmm7     = d3

            //-------------------------------------------------------------
            // Process first steps
            __ASM_EMIT(".align 16")
            __ASM_EMIT("1:")
            __ASM_EMIT("movss       (%[src]), %%xmm0")                          // xmm0     = *src
            __ASM_EMIT("movss       %%xmm0, %%xmm1")                            // xmm1     = s
            __ASM_EMIT("add         $4, %[src]")                                // src      ++
            __ASM_EMIT("movaps      %%xmm1, %%xmm2")                            // xmm2     = s
            __ASM_EMIT("movaps      %%xmm9, %%xmm10")                           // xmm10    = r
            __ASM_EMIT("movaps      %%xmm1, %%xmm3")                            // xmm3     = s
            __ASM_EMIT("movaps      %%xmm9, %%xmm11")                           // xmm11    = r
            __ASM_EMIT("mulps       0x00 + " BIQUAD_XN_SOFF "(%[f]), %%xmm1")   // xmm1     = s*a0
            __ASM_EMIT("mulps       0x10 + " BIQUAD_XN_SOFF "(%[f]), %%xmm9")   // xmm9     = r*i0
            __ASM_EMIT("mulps       0x20 + " BIQUAD_XN_SOFF "(%[f]), %%xmm2")   // xmm2     = s*a1
            __ASM_EMIT("mulps       0x30 + " BIQUAD_XN_SOFF "(%[f]), %%xmm10")  // xmm10    = r*i1
            __ASM_EMIT("addps       %%xmm6, %%xmm1")                            // xmm1     = s*a0+d0 = s2
            __ASM_EMIT("addps       %%xmm14, %%xmm9")                           // xmm9     = r*i0+d1 = r2
            __ASM_EMIT("mulps       0x40 + " BIQUAD_XN_SOFF "(%[f]), %%xmm3")   // xmm3     = s*a2
            __ASM_EMIT("mulps       0x50 + " BIQUAD_XN_SOFF "(%[f]), %%xmm11")  // xmm11    = r*i2
            __ASM_EMIT("movaps      %%xmm1, %%xmm4")                            // xmm4     = s2
            __ASM_EMIT("movaps      %%xmm9, %%xmm12")                           // xmm12    = r2
            __ASM_EMIT("movaps      %%xmm1, %%xmm5")                            // xmm5     = s2
            __ASM_EMIT("movaps      %%xmm9, %%xmm13")                           // xmm13    = r2
            __ASM_EMIT("mulps       0x60 + " BIQUAD_XN_SOFF "(%[f]), %%xmm4")   // xmm4     = s2*b1
            __ASM_EMIT("mulps       0x70 + " BIQUAD_XN_SOFF "(%[f]), %%xmm12")  // xmm12    = r2*j1
            __ASM_EMIT("mulps       0x80 + " BIQUAD_XN_SOFF "(%[f]), %%xmm5")   // xmm5     = s2*b2
            __ASM_EMIT("mulps       0x90 + " BIQUAD_XN_SOFF "(%[f]), %%xmm13")  // xmm13    = r2*j2
            __ASM_EMIT("addps       %%xmm4, %%xmm2")                            // xmm2     = s*a1 + s2*b1 = p1
            __ASM_EMIT("addps       %%xmm12, %%xmm10")                          // xmm10    = r*i1 + r2*j1 = q1
            __ASM_EMIT("addps       %%xmm5, %%xmm3")                            // xmm3     = s*a2 + s2*b2 = p2
            __ASM_EMIT("addps       %%xmm13, %%xmm11")                          // xmm11    = r*i2 + r2*j2 = q2

            // Shift buffer and update delay
            __ASM_EMIT("shufps      $0x93, %%xmm9, %%xmm9")                     // xmm9     = r2[3] r2[0] r2[1] r2[2]
            __ASM_EMIT("shufps      $0x93, %%xmm1, %%xmm1")                     // xmm1     = s2[3] s2[0] s2[1] s2[2]
            __ASM_EMIT("addps       %%xmm7, %%xmm2")                            // xmm2     = p1 + d2
            __ASM_EMIT("addps       %%xmm15, %%xmm10")                          // xmm10    = q1 + d3
            __ASM_EMIT("movss       %%xmm1, %%xmm9")                            // xmm9     = s2[3] r2[0] r2[1] r2[2]

            // Update delay only by mask for filters
            __ASM_EMIT("movaps      0x00 + %[MASK], %%xmm0")                    // xmm0     = MASK_LO
            __ASM_EMIT("movaps      0x10 + %[MASK], %%xmm8")                    // xmm8     = MASK_HI
            __ASM_EMIT("movaps      %%xmm0, %%xmm4")                            // xmm4     = MASK_LO
            __ASM_EMIT("movaps      %%xmm8, %%xmm12")                           // xmm12    = MASK_HI
            __ASM_EMIT("movaps      %%xmm0, %%xmm5")                            // xmm5     = MASK_LO
            __ASM_EMIT("movaps      %%xmm8, %%xmm13")                           // xmm13    = MASK_HI
            __ASM_EMIT("andps       %%xmm4, %%xmm2")                            // xmm2     = (p1 + d2) & MASK_LO
            __ASM_EMIT("andps       %%xmm12, %%xmm10")                          // xmm10    = (q1 + d3) & MASK_HI
            __ASM_EMIT("andps       %%xmm5, %%xmm3")                            // xmm3     = p2 & MASK_LO
            __ASM_EMIT("andps       %%xmm13, %%xmm11")                          // xmm11    = q2 & MASK_HI
            __ASM_EMIT("andnps      %%xmm6, %%xmm4")                            // xmm4     = d0 & ~MASK_LO
            __ASM_EMIT("andnps      %%xmm14, %%xmm12")                          // xmm12    = d1 & ~MASK_HI
            __ASM_EMIT("andnps      %%xmm7, %%xmm5")                            // xmm5     = d2 & ~MASK_LO
            __ASM_EMIT("andnps      %%xmm15, %%xmm13")                          // xmm13    = d3 & ~MASK_HI
            __ASM_EMIT("orps        %%xmm2, %%xmm4")                            // xmm4     = (p1 + d2) & MASK_LO | (d0 & ~MASK_LO)
            __ASM_EMIT("orps        %%xmm10, %%xmm12")                          // xmm12    = (q1 + d3) & MASK_HI | (d1 & ~MASK_HI)
            __ASM_EMIT("orps        %%xmm3, %%xmm5")                            // xmm5     = (p2 & MASK_LO) | (d2 & ~MASK_LO)
            __ASM_EMIT("orps        %%xmm11, %%xmm13")                          // xmm13    = (q2 & MASK_HI) | (d3 & ~MASK_HI)
            __ASM_EMIT("movaps      %%xmm4, %%xmm6")                            // xmm6     = d0'
            __ASM_EMIT("movaps      %%xmm12, %%xmm14")                          // xmm14    = d1'
            __ASM_EMIT("movaps      %%xmm5, %%xmm7")                            // xmm7     = d2'
            __ASM_EMIT("movaps      %%xmm13, %%xmm15")                          // xmm15    = d3'

            // Shift mask and repeat loop
            __ASM_EMIT("dec         %[count]")
            __ASM_EMIT("jz          4f")                                        // jump to completion
            __ASM_EMIT("lea         0x01(,%[mask],2), %[mask]")                 // mask     = (mask << 1) | 1
            __ASM_EMIT("movaps      %%xmm0, %%xmm2")                            // xmm2     = m[0] m[1] m[2] m[3]
            __ASM_EMIT("shufps      $0x93, %%xmm8, %%xmm8")                     // xmm8     = m[7] m[4] m[5] m[6]
            __ASM_EMIT("shufps      $0x93, %%xmm0, %%xmm0")                     // xmm0     = m[3] m[0] m[1] m[2]
            __ASM_EMIT("movss       %%xmm0, %%xmm8")                            // xmm8     = m[3] m[4] m[5] m[6]
            __ASM_EMIT("movss       %%xmm2, %%xmm0")                            // xmm0     = m[0] m[0] m[1] m[2]
            __ASM_EMIT("movaps      %%xmm0, 0x00 + %[MASK]")                    // *MASK_LO = xmm0
            __ASM_EMIT("movaps      %%xmm8, 0x10 + %[MASK]")                    // *MASK_HI = xmm8
            __ASM_EMIT("cmp         $0xff, %[mask]")
            __ASM_EMIT("jne         1b")

            //-------------------------------------------------------------
            // 8x filter processing without mask
            __ASM_EMIT(".align 16")
            __ASM_EMIT("3:")
            __ASM_EMIT("movss       (%[src]), %%xmm0")                          // xmm0     = *src
            __ASM_EMIT("movss       %%xmm0, %%xmm1")                            // xmm1     = s
            __ASM_EMIT("add         $4, %[src]")                                // src      ++
            __ASM_EMIT("movaps      %%xmm1, %%xmm2")                            // xmm2     = s
            __ASM_EMIT("movaps      %%xmm9, %%xmm10")                           // xmm10    = r
            __ASM_EMIT("movaps      %%xmm1, %%xmm3")                            // xmm3     = s
            __ASM_EMIT("movaps      %%xmm9, %%xmm11")                           // xmm11    = r
            __ASM_EMIT("mulps       0x00 + " BIQUAD_XN_SOFF "(%[f]), %%xmm1")   // xmm1     = s*a0
            __ASM_EMIT("mulps       0x10 + " BIQUAD_XN_SOFF "(%[f]), %%xmm9")   // xmm9     = r*i0
            __ASM_EMIT("mulps       0x20 + " BIQUAD_XN_SOFF "(%[f]), %%xmm2")   // xmm2     = s*a1
            __ASM_EMIT("mulps       0x30 + " BIQUAD_XN_SOFF "(%[f]), %%xmm10")  // xmm10    = r*i1
            __ASM_EMIT("addps       %%xmm6, %%xmm1")                            // xmm1     = s*a0+d0 = s2
            __ASM_EMIT("addps       %%xmm14, %%xmm9")                           // xmm9     = r*i0+d1 = r2
            __ASM_EMIT("mulps       0x40 + " BIQUAD_XN_SOFF "(%[f]), %%xmm3")   // xmm3     = s*a2
            __ASM_EMIT("mulps       0x50 + " BIQUAD_XN_SOFF "(%[f]), %%xmm11")  // xmm11    = r*i2
            __ASM_EMIT("movaps      %%xmm1, %%xmm4")                            // xmm4     = s2
            __ASM_EMIT("movaps      %%xmm9, %%xmm12")                           // xmm12    = r2
            __ASM_EMIT("movaps      %%xmm1, %%xmm5")                            // xmm5     = s2
            __ASM_EMIT("movaps      %%xmm9, %%xmm13")                           // xmm13    = r2
            __ASM_EMIT("mulps       0x60 + " BIQUAD_XN_SOFF "(%[f]), %%xmm4")   // xmm4     = s2*b1
            __ASM_EMIT("mulps       0x70 + " BIQUAD_XN_SOFF "(%[f]), %%xmm12")  // xmm12    = r2*j1
            __ASM_EMIT("mulps       0x80 + " BIQUAD_XN_SOFF "(%[f]), %%xmm5")   // xmm5     = s2*b2
            __ASM_EMIT("mulps       0x90 + " BIQUAD_XN_SOFF "(%[f]), %%xmm13")  // xmm13    = r2*j2
            __ASM_EMIT("addps       %%xmm4, %%xmm2")                            // xmm2     = s*a1 + s2*b1 = p1
            __ASM_EMIT("addps       %%xmm12, %%xmm10")                          // xmm10    = r*i1 + r2*j1 = q1
            __ASM_EMIT("addps       %%xmm5, %%xmm3")                            // xmm3     = s*a2 + s2*b2 = p2
            __ASM_EMIT("addps       %%xmm13, %%xmm11")                          // xmm11    = r*i2 + r2*j2 = q2

            // Shift buffer and update delay
            __ASM_EMIT("shufps      $0x93, %%xmm9, %%xmm9")                     // xmm9     = r2[3] r2[0] r2[1] r2[2]
            __ASM_EMIT("shufps      $0x93, %%xmm1, %%xmm1")                     // xmm1     = s2[3] s2[0] s2[1] s2[2]
            __ASM_EMIT("addps       %%xmm7, %%xmm2")                            // xmm2     = p1 + d2
            __ASM_EMIT("addps       %%xmm15, %%xmm10")                          // xmm10    = q1 + d3
            __ASM_EMIT("movss       %%xmm9, (%[dst])")                          // *dst     = r2[3]
            __ASM_EMIT("movss       %%xmm1, %%xmm9")                            // xmm9     = s2[3] r2[0] r2[1] r2[2]
            __ASM_EMIT("movaps      %%xmm2, %%xmm6")                            // xmm6     = d0'
            __ASM_EMIT("movaps      %%xmm10, %%xmm14")                          // xmm14    = d1'
            __ASM_EMIT("movaps      %%xmm3, %%xmm7")                            // xmm7     = d2'
            __ASM_EMIT("movaps      %%xmm11, %%xmm15")                          // xmm15    = d3'

            // Repeat loop
            __ASM_EMIT("add         $4, %[dst]")                                // dst      ++
            __ASM_EMIT("dec         %[count]")
            __ASM_EMIT("jnz         3b")

            //-------------------------------------------------------------
            // Prepare last loop
            __ASM_EMIT("4:")
            __ASM_EMIT("movaps      0x00 + %[MASK], %%xmm0")                    // xmm0     = m[0] m[1] m[2] m[3]
            __ASM_EMIT("movaps      0x10 + %[MASK], %%xmm8")                    // xmm8     = m[4] m[5] m[6] m[7]
            __ASM_EMIT("shufps      $0x93, %%xmm0, %%xmm0")                     // xmm0     = m[3] m[0] m[1] m[2]
            __ASM_EMIT("shufps      $0x93, %%xmm8, %%xmm8")                     // xmm8     = m[7] m[4] m[5] m[6]
            __ASM_EMIT("xorps       %%xmm2, %%xmm2")                            // xmm2     = 0 0 0 0
            __ASM_EMIT("movss       %%xmm0, %%xmm8")                            // xmm8     = m[3] m[4] m[5] m[6]
            __ASM_EMIT("shl         $1, %[mask]")                               // mask     = mask << 1
            __ASM_EMIT("movss       %%xmm2, %%xmm0")                            // xmm0     = 0 m[0] m[1] m[2]

            //-------------------------------------------------------------
            // Process last steps
            __ASM_EMIT(".align 16")
            __ASM_EMIT("5:")
            __ASM_EMIT("movaps      %%xmm1, %%xmm2")                            // xmm2     = s
            __ASM_EMIT("movaps      %%xmm9, %%xmm10")                           // xmm10    = r
            __ASM_EMIT("movaps      %%xmm1, %%xmm3")                            // xmm3     = s
            __ASM_EMIT("movaps      %%xmm9, %%xmm11")                           // xmm11    = r
            __ASM_EMIT("mulps       0x00 + " BIQUAD_XN_SOFF "(%[f]), %%xmm1")   // xmm1     = s*a0
            __ASM_EMIT("mulps       0x10 + " BIQUAD_XN_SOFF "(%[f]), %%xmm9")   // xmm9     = r*i0
            __ASM_EMIT("mulps       0x20 + " BIQUAD_XN_SOFF "(%[f]), %%xmm2")   // xmm2     = s*a1
            __ASM_EMIT("mulps       0x30 + " BIQUAD_XN_SOFF "(%[f]), %%xmm10")  // xmm10    = r*i1
            __ASM_EMIT("addps       %%xmm6, %%xmm1")                            // xmm1     = s*a0+d0 = s2
            __ASM_EMIT("addps       %%xmm14, %%xmm9")                           // xmm9     = r*i0+d1 = r2
            __ASM_EMIT("mulps       0x40 + " BIQUAD_XN_SOFF "(%[f]), %%xmm3")   // xmm3     = s*a2
            __ASM_EMIT("mulps       0x50 + " BIQUAD_XN_SOFF "(%[f]), %%xmm11")  // xmm11    = r*i2
            __ASM_EMIT("movaps      %%xmm1, %%xmm4")                            // xmm4     = s2
            __ASM_EMIT("movaps      %%xmm9, %%xmm12")                           // xmm12    = r2
            __ASM_EMIT("movaps      %%xmm1, %%xmm5")                            // xmm5     = s2
            __ASM_EMIT("movaps      %%xmm9, %%xmm13")                           // xmm13    = r2
            __ASM_EMIT("mulps       0x60 + " BIQUAD_XN_SOFF "(%[f]), %%xmm4")   // xmm4     = s2*b1
            __ASM_EMIT("mulps       0x70 + " BIQUAD_XN_SOFF "(%[f]), %%xmm12")  // xmm12    = r2*j1
            __ASM_EMIT("mulps       0x80 + " BIQUAD_XN_SOFF "(%[f]), %%xmm5")   // xmm5     = s2*b2
            __ASM_EMIT("mulps       0x90 + " BIQUAD_XN_SOFF "(%[f]), %%xmm13")  // xmm13    = r2*j2
            __ASM_EMIT("addps       %%xmm4, %%xmm2")                            // xmm2     = s*a1 + s2*b1 = p1
            __ASM_EMIT("addps       %%xmm12, %%xmm10")                          // xmm10    = r*i1 + r2*j1 = q1
            __ASM_EMIT("addps       %%xmm5, %%xmm3")                            // xmm3     = s*a2 + s2*b2 = p2
            __ASM_EMIT("addps       %%xmm13, %%xmm11")                          // xmm11    = r*i2 + r2*j2 = q2

            // Shift buffer and update delay
            __ASM_EMIT("shufps      $0x93, %%xmm9, %%xmm9")                     // xmm9     = r2[3] r2[0] r2[1] r2[2]
            __ASM_EMIT("shufps      $0x93, %%xmm1, %%xmm1")                     // xmm1     = s2[3] s2[0] s2[1] s2[2]
            __ASM_EMIT("addps       %%xmm7, %%xmm2")                            // xmm2     = p1 + d2
            __ASM_EMIT("addps       %%xmm15, %%xmm10")                          // xmm10    = q1 + d3
            __ASM_EMIT("test        $0x80, %[mask]")
            __ASM_EMIT("jz          6f")
            __ASM_EMIT("movss       %%xmm9, (%[dst])")                          // *dst     = r2[3]
            __ASM_EMIT("add         $4, %[dst]")                                // dst      ++
            __ASM_EMIT("6:")
            __ASM_EMIT("movss       %%xmm1, %%xmm9")                            // xmm9     = s2[3] r2[0] r2[1] r2[2]

            // Update delay only by mask for filters
            __ASM_EMIT("movaps      %%xmm0, %%xmm4")                            // xmm4     = MASK_LO
            __ASM_EMIT("movaps      %%xmm8, %%xmm12")                           // xmm12    = MASK_HI
            __ASM_EMIT("movaps      %%xmm0, %%xmm5")                            // xmm5     = MASK_LO
            __ASM_EMIT("movaps      %%xmm8, %%xmm13")                           // xmm13    = MASK_HI
            __ASM_EMIT("andps       %%xmm4, %%xmm2")                            // xmm2     = (p1 + d2) & MASK_LO
            __ASM_EMIT("andps       %%xmm12, %%xmm10")                          // xmm10    = (q1 + d3) & MASK_HI
            __ASM_EMIT("andps       %%xmm5, %%xmm3")                            // xmm3     = p2 & MASK_LO
            __ASM_EMIT("andps       %%xmm13, %%xmm11")                          // xmm11    = q2 & MASK_HI
            __ASM_EMIT("andnps      %%xmm6, %%xmm4")                            // xmm4     = d0 & ~MASK_LO
            __ASM_EMIT("andnps      %%xmm14, %%xmm12")                          // xmm12    = d1 & ~MASK_HI
            __ASM_EMIT("andnps      %%xmm7, %%xmm5")                            // xmm5     = d2 & ~MASK_LO
            __ASM_EMIT("andnps      %%xmm15, %%xmm13")                          // xmm13    = d3 & ~MASK_HI
            __ASM_EMIT("orps        %%xmm2, %%xmm4")                            // xmm4     = (p1 + d2) & MASK_LO | (d0 & ~MASK_LO)
            __ASM_EMIT("orps        %%xmm10, %%xmm12")                          // xmm12    = (q1 + d3) & MASK_HI | (d1 & ~MASK_HI)
            __ASM_EMIT("orps        %%xmm3, %%xmm5")                            // xmm5     = (p2 & MASK_LO) | (d2 & ~MASK_LO)
            __ASM_EMIT("orps        %%xmm11, %%xmm13")                          // xmm13    = (q2 & MASK_HI) | (d3 & ~MASK_HI)
            __ASM_EMIT("movaps      %%xmm4, %%xmm6")                            // xmm6     = d0'
            __ASM_EMIT("movaps      %%xmm12, %%xmm14")                          // xmm14    = d1'
            __ASM_EMIT("movaps      %%xmm5, %%xmm7")                            // xmm7     = d2'
            __ASM_EMIT("movaps      %%xmm13, %%xmm15")                          // xmm15    = d3'

            // Shift mask and repeat loop
            __ASM_EMIT("xorps       %%xmm2, %%xmm2")                            // xmm2     = 0 0 0 0
            __ASM_EMIT("shufps      $0x93, %%xmm0, %%xmm0")                     // xmm0     = m[3] m[0] m[1] m[2]
            __ASM_EMIT("shufps      $0x93, %%xmm8, %%xmm8")                     // xmm8     = m[7] m[4] m[5] m[6]
            __ASM_EMIT("shl         $1, %[mask]")                               // mask     = mask << 1
            __ASM_EMIT("movss       %%xmm0, %%xmm8")                            // xmm8     = m[3] m[4] m[5] m[6]
            __ASM_EMIT("and         $0xff, %[mask]")                            // mask     = (mask << 1) & 0xff
            __ASM_EMIT("movss       %%xmm2, %%xmm0")                            // xmm0     = 0 m[0] m[1] m[2]
            __ASM_EMIT("jnz         5b")                                        // check that mask is not zero

            //-------------------------------------------------------------
            // Store delay buffers
            __ASM_EMIT("movaps      %%xmm6, 0x00(%[f])")                        // xmm6     = d0
            __ASM_EMIT("movaps      %%xmm14, 0x10(%[f])")                       // xmm14    = d1
            __ASM_EMIT("movaps      %%xmm7, 0x20(%[f])")                        // xmm7     = d2
            __ASM_EMIT("movaps      %%xmm15, 0x30(%[f])")                       // xmm15    = d3

            // Exit label
            __ASM_EMIT("8:")

            : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count),
              [mask] "=&r" (mask)
            : [f] "r" (f),
              [X_MASK] "m" (X_MASK0001),
              [MASK] "o" (MASK)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7",
              "%xmm8", "%xmm9", "%xmm10", "%xmm11",
              "%xmm12", "%xmm13", "%xmm14", "%xmm15"
        );
    }
}

#endif /* DSP_ARCH_X86_SSE3_FILTERS_STATIC_H_ */
