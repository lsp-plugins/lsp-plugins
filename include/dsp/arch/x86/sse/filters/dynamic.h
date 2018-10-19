/*
 * dynamic.h
 *
 *  Created on: 14 февр. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_SSE_FILTERS_DYNAMIC_H_
#define DSP_ARCH_X86_SSE_FILTERS_DYNAMIC_H_

#ifndef DSP_ARCH_X86_SSE_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_SSE_IMPL */

namespace sse
{
    void dyn_biquad_process_x1(float *dst, const float *src, float *d, size_t count, const biquad_x1_t *f)
    {
        ARCH_X86_ASM
        (
            // Check count
            __ASM_EMIT("test        %[count], %[count]")
            __ASM_EMIT("jz          1f")

            // Load permanent data
            __ASM_EMIT("xorps       %%xmm1, %%xmm1")                        // xmm1 = 0  0  0  0
            __ASM_EMIT("xorps       %%xmm2, %%xmm2")                        // xmm2 = 0  0  0  0
            __ASM_EMIT("movlps      (%[d]), %%xmm1")                        // xmm1 = d0 d1 0  0

            // Start loop
            __ASM_EMIT(".align 16")
            __ASM_EMIT("2:")
            __ASM_EMIT("movss       (%[src]), %%xmm0")                      // xmm0 = *src = s ? ? ?
            __ASM_EMIT("shufps      $0xd0, %%xmm1, %%xmm1")                 // xmm1 = d0 d0 d1 0
            __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0")                 // xmm0 = s s s s
            __ASM_EMIT("mulps     " DYN_BIQUAD_X1_A_SOFF "(%[f]), %%xmm0")  // xmm0 = s*a0 s*a0 s*a1 s*a2
            __ASM_EMIT("addps       %%xmm1, %%xmm0")                        // xmm0 = s*a0+d0 s*a0+d0 s*a1+d1 s*a2
            __ASM_EMIT("movaps      %%xmm0, %%xmm1")                        // xmm1 = s*a0+d0 s*a0+d0 s*a1+d1 s*a2
            __ASM_EMIT("movhlps     %%xmm0, %%xmm2")                        // xmm2 = s*a1+d1 s*a2 0 0
            __ASM_EMIT("movss       %%xmm0, (%[dst])")                      // *dst = s*a0+d0
            __ASM_EMIT("mulps     " DYN_BIQUAD_X1_B_SOFF "(%[f]), %%xmm1")  // xmm1 = (s*a0+d0)*b1 (s*a0+d0)*b2 0 0
            __ASM_EMIT("add         $4, %[src]")
            __ASM_EMIT("add         $4, %[dst]")
            __ASM_EMIT("add      $" DYN_BIQUAD_X1_SSIZE ", %[f]")           // f++
            __ASM_EMIT("addps       %%xmm2, %%xmm1")                        // xmm1 = (s*a0+d0)*b1+s*a1+d1 (s*a0+d0)*b2+s*a2 0 0
            __ASM_EMIT("dec         %[count]")
            __ASM_EMIT("jnz         2b")

            // Store the updated buffer state
            __ASM_EMIT("movlps      %%xmm1, (%[d])")                        // store memory

            // Exit label
            __ASM_EMIT("1:")

            :
            : [dst] "r" (dst), [src] "r" (src), [count] "r" (count), [f] "r" (f), [d] "r" (d)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2"
        );
    }


    void dyn_biquad_process_x2(float *dst, const float *src, float *d, size_t count, const biquad_x2_t *f)
    {
        ARCH_X86_ASM
        (
            // Check count
            __ASM_EMIT("test        %[count], %[count]")
            __ASM_EMIT("jz          1f")

            // Load permanent data
            __ASM_EMIT("xorps       %%xmm2, %%xmm2")                        // xmm2 = 0  0  0  0
            __ASM_EMIT("xorps       %%xmm3, %%xmm3")                        // xmm3 = 0  0  0  0
            __ASM_EMIT("xorps       %%xmm4, %%xmm4")                        // xmm4 = 0  0  0  0
            __ASM_EMIT("xorps       %%xmm5, %%xmm5")                        // xmm5 = 0  0  0  0
            __ASM_EMIT("movlps      0x00(%[d]), %%xmm2")                    // xmm2 = d0 d1 0  0
            __ASM_EMIT("movlps      0x08(%[d]), %%xmm3")                    // xmm3 = e0 e1 0  0

            // Load data
            __ASM_EMIT("movss       (%[src]), %%xmm1")                      // xmm1 = *src = s 0 0 0
            __ASM_EMIT("shufps      $0xd0, %%xmm2, %%xmm2")                 // xmm2 = d0 d0 d1 0
            __ASM_EMIT("shufps      $0x00, %%xmm1, %%xmm1")                 // xmm1 = s s s s
            __ASM_EMIT("mulps     " DYN_BIQUAD_X2_A_SOFF "(%[f]), %%xmm1")  // xmm1 = s*a0 s*a0 s*a1 s*a2
            __ASM_EMIT("addps       %%xmm2, %%xmm1")                        // xmm1 = s*a0+d0 s*a0+d0 s*a1+d1 s*a2
            __ASM_EMIT("movaps      %%xmm1, %%xmm2")                        // xmm2 = s*a0+d0 s*a0+d0 s*a1+d1 s*a2
            __ASM_EMIT("movhlps     %%xmm1, %%xmm4")                        // xmm4 = s*a1+d1 s*a2 0 0
            __ASM_EMIT("mulps     " DYN_BIQUAD_X2_B_SOFF "(%[f]), %%xmm2")  // xmm2 = (s*a0+d0)*b1 (s*a0+d0)*b2 0 0
            __ASM_EMIT("add         $4, %[src]")
            __ASM_EMIT("add      $" DYN_BIQUAD_X2_SSIZE ", %[f]")           // f++
            __ASM_EMIT("addps       %%xmm4, %%xmm2")                        // xmm2 = (s*a0+d0)*b1+s*a1+d1 (s*a0+d0)*b2+s*a2 0 0
            __ASM_EMIT("dec         %[count]")
            __ASM_EMIT("jz          3f")

            // Start loop
            // xmm1 = r
            __ASM_EMIT(".align 16")
            __ASM_EMIT("2:")

            // Load data
            __ASM_EMIT("movss       (%[src]), %%xmm0")                      // xmm0 = *src = s 0 0 0
            __ASM_EMIT("shufps      $0xd0, %%xmm2, %%xmm2")                 // xmm2 = d0 d0 d1 0
            __ASM_EMIT("shufps      $0xd0, %%xmm3, %%xmm3")                 // xmm3 = e0 e0 e1 0
            __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0")                 // xmm0 = s s s s
            __ASM_EMIT("shufps      $0x00, %%xmm1, %%xmm1")                 // xmm1 = r r r r

            __ASM_EMIT("mulps     " DYN_BIQUAD_X2_A_SOFF "(%[f]), %%xmm0")  // xmm0 = s*a0 s*a0 s*a1 s*a2
            __ASM_EMIT("mulps     " DYN_BIQUAD_X2_I_SOFF "(%[f]), %%xmm1")  // xmm1 = r*i0 r*i0 r*i1 r*i2
            __ASM_EMIT("addps       %%xmm2, %%xmm0")                        // xmm0 = s*a0+d0 s*a0+d0 s*a1+d1 s*a2
            __ASM_EMIT("addps       %%xmm3, %%xmm1")                        // xmm1 = r*i0+e0 r*i0+e0 r*i1+e1 i*e2
            __ASM_EMIT("movaps      %%xmm0, %%xmm2")                        // xmm2 = s*a0+d0 s*a0+d0 s*a1+d1 s*a2
            __ASM_EMIT("movaps      %%xmm1, %%xmm3")                        // xmm3 = r*i0+e0 r*i0+e0 r*i1+e1 i*e2
            __ASM_EMIT("movhlps     %%xmm0, %%xmm4")                        // xmm4 = s*a1+d1 s*a2 0 0
            __ASM_EMIT("movhlps     %%xmm1, %%xmm5")                        // xmm5 = r*i1+e1 r*i2 0 0
            __ASM_EMIT("movss       %%xmm1, (%[dst])")                      // *dst = r*i0+e0
            __ASM_EMIT("mulps     " DYN_BIQUAD_X2_B_SOFF "(%[f]), %%xmm2")  // xmm2 = (s*a0+d0)*b1 (s*a0+d0)*b2 0 0
            __ASM_EMIT("mulps     " DYN_BIQUAD_X2_J_SOFF "(%[f]), %%xmm3")  // xmm3 = (r*i0+e0)*j1 (r*i0+e0)*j2 0 0
            __ASM_EMIT("addps       %%xmm4, %%xmm2")                        // xmm2 = (s*a0+d0)*b1+s*a1+d1 (s*a0+d0)*b2+s*a2 0 0
            __ASM_EMIT("addps       %%xmm5, %%xmm3")                        // xmm3 = (r*i0+e0)*b1+r*i1+e1 (r*i0+e0)*b2+r*i2 0 0
            __ASM_EMIT("movaps      %%xmm0, %%xmm1")                        // xmm1 = xmm0 = r ? ? ?
            __ASM_EMIT("add      $" DYN_BIQUAD_X2_SSIZE ", %[f]")           // f++
            __ASM_EMIT("add         $4, %[src]")
            __ASM_EMIT("add         $4, %[dst]")
            __ASM_EMIT("dec         %[count]")
            __ASM_EMIT("jnz         2b")

            __ASM_EMIT("3:")
            __ASM_EMIT("shufps      $0xd0, %%xmm3, %%xmm3")                 // xmm3 = e0 e0 e1 0
            __ASM_EMIT("shufps      $0x00, %%xmm1, %%xmm1")                 // xmm1 = r r r r
            __ASM_EMIT("mulps     " DYN_BIQUAD_X2_I_SOFF "(%[f]), %%xmm1")  // xmm1 = r*i0 r*i0 r*i1 r*i2
            __ASM_EMIT("addps       %%xmm3, %%xmm1")                        // xmm1 = r*i0+e0 r*i0+e0 r*i1+e1 i*e2
            __ASM_EMIT("movaps      %%xmm1, %%xmm3")                        // xmm3 = r*i0+e0 r*i0+e0 r*i1+e1 i*e2
            __ASM_EMIT("movhlps     %%xmm1, %%xmm5")                        // xmm5 = r*i1+e1 r*i2 0 0
            __ASM_EMIT("movss       %%xmm1, (%[dst])")                      // *dst = r*i0+e0
            __ASM_EMIT("mulps     " DYN_BIQUAD_X2_J_SOFF "(%[f]), %%xmm3")  // xmm3 = (r*i0+e0)*j1 (r*i0+e0)*j2 0 0
            __ASM_EMIT("addps       %%xmm5, %%xmm3")                        // xmm3 = (r*i0+e0)*b1+r*i1+e1 (r*i0+e0)*b2+r*i2 0 0

            // Store the updated buffer state
            __ASM_EMIT("movlps      %%xmm2, 0x00(%[d])")                    // store memory
            __ASM_EMIT("movlps      %%xmm3, 0x08(%[d])")                    // store memory

            // Exit label
            __ASM_EMIT("1:")

            :
            : [dst] "r" (dst), [src] "r" (src), [count] "r" (count), [f] "r" (f), [d] "r" (d)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5"
        );
    }

    void dyn_biquad_process_x4(float *dst, const float *src, float *d, size_t count, const biquad_x4_t *f)
    {
        float   MASK[4] __lsp_aligned16;
        size_t  mask;
        IF_ARCH_I386(float *f_s);

        ARCH_X86_ASM
        (
            // Check count
            __ASM_EMIT("test        %[count], %[count]")
            __ASM_EMIT("jz          8f")

            // Initialize mask
            // xmm0=tmp, xmm1={s,s2[4]}, xmm2=p1[4], xmm3=p2[4], xmm6=d0[4], xmm7=d1[4]
            __ASM_EMIT("mov         $1, %[mask]")
            __ASM_EMIT("movaps      %[X_MASK], %%xmm0")
            __ASM_EMIT("xorps       %%xmm1, %%xmm1")
            __ASM_EMIT("movaps      %%xmm0, %[MASK]")

            // Load delay buffer
            __ASM_EMIT32("mov       %[f], %[f_s]")
            __ASM_EMIT32("mov       %[d], %[f]")
            __ASM_EMIT32("movups    0x00(%[f]), %%xmm6")                        // xmm6     = d0
            __ASM_EMIT32("movups    0x10(%[f]), %%xmm7")                        // xmm7     = d1
            __ASM_EMIT32("mov       %[f_s], %[f]")
            __ASM_EMIT64("movups    0x00(%[d]), %%xmm6")                        // xmm6     = d0
            __ASM_EMIT64("movups    0x10(%[d]), %%xmm7")                        // xmm7     = d1

            // Process first 3 steps
            __ASM_EMIT(".align 16")
            __ASM_EMIT("1:")
            __ASM_EMIT("movss       (%[src]), %%xmm0")                          // xmm0     = *src
            __ASM_EMIT("add         $4, %[src]")                                // src      ++
            __ASM_EMIT("movss       %%xmm0, %%xmm1")                            // xmm1     = s
            __ASM_EMIT("movaps      %%xmm1, %%xmm2")                            // xmm2     = s
            __ASM_EMIT("movaps      %%xmm1, %%xmm3")                            // xmm3     = s
            __ASM_EMIT("mulps       " DYN_BIQUAD_X4_A0_SOFF "(%[f]), %%xmm1")   // xmm1     = s*a0
            __ASM_EMIT("mulps       " DYN_BIQUAD_X4_A1_SOFF "(%[f]), %%xmm2")   // xmm2     = s*a1
            __ASM_EMIT("addps       %%xmm6, %%xmm1")                            // xmm1     = s*a0+d0 = s2
            __ASM_EMIT("mulps       " DYN_BIQUAD_X4_A2_SOFF "(%[f]), %%xmm3")   // xmm3     = s*a2
            __ASM_EMIT("movaps      %%xmm1, %%xmm4")                            // xmm4     = s2
            __ASM_EMIT("movaps      %%xmm1, %%xmm5")                            // xmm5     = s2
            __ASM_EMIT("mulps       " DYN_BIQUAD_X4_B1_SOFF "(%[f]), %%xmm4")   // xmm4     = s2*b1
            __ASM_EMIT("mulps       " DYN_BIQUAD_X4_B2_SOFF "(%[f]), %%xmm5")   // xmm5     = s2*b2
            __ASM_EMIT("addps       %%xmm4, %%xmm2")                            // xmm2     = s*a1 + s2*b1 = p1
            __ASM_EMIT("addps       %%xmm5, %%xmm3")                            // xmm3     = s*a2 + s2*b2 = p2

            // Shift buffer
            __ASM_EMIT("shufps      $0x90, %%xmm1, %%xmm1")                     // xmm1     = s2[0] s2[0] s2[1] s2[2]

            // Update delay only by mask
            __ASM_EMIT("addps       %%xmm7, %%xmm2")                            // xmm2     = p1 + d1
            __ASM_EMIT("movaps      %[MASK], %%xmm0")                           // xmm0     = MASK
            __ASM_EMIT("movaps      %%xmm0, %%xmm4")                            // xmm4     = MASK
            __ASM_EMIT("movaps      %%xmm0, %%xmm5")                            // xmm5     = MASK
            __ASM_EMIT("andps       %%xmm4, %%xmm2")                            // xmm2     = (p1 + d1) & MASK
            __ASM_EMIT("andps       %%xmm5, %%xmm3")                            // xmm3     = p2 & MASK
            __ASM_EMIT("andnps      %%xmm6, %%xmm4")                            // xmm4     = d0 & ~MASK
            __ASM_EMIT("andnps      %%xmm7, %%xmm5")                            // xmm5     = d1 & ~MASK
            __ASM_EMIT("orps        %%xmm2, %%xmm4")                            // xmm4     = (p1 + d1) & MASK | (d0 & ~MASK)
            __ASM_EMIT("orps        %%xmm3, %%xmm5")                            // xmm5     = (p2 & MASK) | (d1 & ~MASK)
            __ASM_EMIT("movaps      %%xmm4, %%xmm6")                            // xmm6     = d0 & ~MASK
            __ASM_EMIT("movaps      %%xmm5, %%xmm7")                            // xmm7     = d1 & ~MASK

            // Repeat loop
            __ASM_EMIT("add      $" DYN_BIQUAD_X4_SSIZE ", %[f]")               // f++
            __ASM_EMIT("dec         %[count]")
            __ASM_EMIT("jz          4f")                                        // jump to completion
            __ASM_EMIT("lea         0x01(,%[mask], 2), %[mask]")                // mask     = (mask << 1) | 1
            __ASM_EMIT("shufps      $0x90, %%xmm0, %%xmm0")                     // xmm0     = m[0] m[0] m[1] m[2]
            __ASM_EMIT("movaps      %%xmm0, %[MASK]")                           // store mask
            __ASM_EMIT("cmp         $0x0f, %[mask]")
            __ASM_EMIT("jne         1b")

            // 4x filter processing without mask
            __ASM_EMIT(".align 16")
            __ASM_EMIT("3:")
            __ASM_EMIT("movss       (%[src]), %%xmm0")                          // xmm0     = *src
            __ASM_EMIT("add         $4, %[src]")                                // src      ++
            __ASM_EMIT("movss       %%xmm0, %%xmm1")                            // xmm1     = s
            __ASM_EMIT("movaps      %%xmm1, %%xmm2")                            // xmm2     = s
            __ASM_EMIT("movaps      %%xmm1, %%xmm3")                            // xmm3     = s
            __ASM_EMIT("mulps       " DYN_BIQUAD_X4_A0_SOFF "(%[f]), %%xmm1")   // xmm1     = s*a0
            __ASM_EMIT("mulps       " DYN_BIQUAD_X4_A1_SOFF "(%[f]), %%xmm2")   // xmm2     = s*a1
            __ASM_EMIT("addps       %%xmm6, %%xmm1")                            // xmm1     = s*a0+d0 = s2
            __ASM_EMIT("mulps       " DYN_BIQUAD_X4_A2_SOFF "(%[f]), %%xmm3")   // xmm3     = s*a2
            __ASM_EMIT("movaps      %%xmm1, %%xmm4")                            // xmm4     = s2
            __ASM_EMIT("movaps      %%xmm1, %%xmm5")                            // xmm5     = s2
            __ASM_EMIT("mulps       " DYN_BIQUAD_X4_B1_SOFF "(%[f]), %%xmm4")   // xmm4     = s2*b1
            __ASM_EMIT("mulps       " DYN_BIQUAD_X4_B2_SOFF "(%[f]), %%xmm5")   // xmm5     = s2*b2
            __ASM_EMIT("addps       %%xmm4, %%xmm2")                            // xmm2     = s*a1 + s2*b1 = p1
            __ASM_EMIT("addps       %%xmm5, %%xmm3")                            // xmm3     = s*a2 + s2*b2 = p2
            __ASM_EMIT("addps       %%xmm7, %%xmm2")                            // xmm2     = p1 + d1
            __ASM_EMIT("movaps      %%xmm3, %%xmm7")                            // xmm7     = p2
            __ASM_EMIT("movaps      %%xmm2, %%xmm6")                            // xmm6     = p1 + d1

            // Shift buffer and repeat loop
            __ASM_EMIT("add      $" DYN_BIQUAD_X4_SSIZE ", %[f]")               // f++
            __ASM_EMIT("shufps      $0x93, %%xmm1, %%xmm1")                     // xmm1     = s2[3] s2[0] s2[1] s2[2]
            __ASM_EMIT("movss       %%xmm1, (%[dst])")                          // *dst     = s2[3]
            __ASM_EMIT("add         $4, %[dst]")                                // dst      ++
            __ASM_EMIT("dec         %[count]")
            __ASM_EMIT("jnz         3b")

            // Prepare last loop
            __ASM_EMIT("4:")
            __ASM_EMIT("movaps      %[MASK], %%xmm0")                           // xmm0     = m[0] m[1] m[2] m[3]
            __ASM_EMIT("xorps       %%xmm2, %%xmm2")                            // xmm2     = 0 0 0 0
            __ASM_EMIT("shl         $1, %[mask]")                               // mask     = mask << 1
            __ASM_EMIT("shufps      $0x90, %%xmm0, %%xmm0")                     // xmm0     = m[0] m[0] m[1] m[2]
            __ASM_EMIT("and         $0x0f, %[mask]")                            // mask     = (mask << 1) & 0x0f
            __ASM_EMIT("movss       %%xmm2, %%xmm0")                            // xmm0     = 0 m[0] m[1] m[2]

            // Process steps
            __ASM_EMIT(".align 16")
            __ASM_EMIT("5:")
            __ASM_EMIT("movaps      %%xmm1, %%xmm2")                            // xmm2     = s
            __ASM_EMIT("movaps      %%xmm1, %%xmm3")                            // xmm3     = s
            __ASM_EMIT("mulps       " DYN_BIQUAD_X4_A0_SOFF "(%[f]), %%xmm1")   // xmm1     = s*a0
            __ASM_EMIT("mulps       " DYN_BIQUAD_X4_A1_SOFF "(%[f]), %%xmm2")   // xmm2     = s*a1
            __ASM_EMIT("addps       %%xmm6, %%xmm1")                            // xmm1     = s*a0+d0 = s2
            __ASM_EMIT("mulps       " DYN_BIQUAD_X4_A2_SOFF "(%[f]), %%xmm3")   // xmm3     = s*a2
            __ASM_EMIT("movaps      %%xmm1, %%xmm4")                            // xmm4     = s2
            __ASM_EMIT("movaps      %%xmm1, %%xmm5")                            // xmm5     = s2
            __ASM_EMIT("mulps       " DYN_BIQUAD_X4_B1_SOFF "(%[f]), %%xmm4")   // xmm4     = s2*b1
            __ASM_EMIT("mulps       " DYN_BIQUAD_X4_B2_SOFF "(%[f]), %%xmm5")   // xmm5     = s2*b2
            __ASM_EMIT("addps       %%xmm4, %%xmm2")                            // xmm2     = s*a1 + s2*b1 = p1
            __ASM_EMIT("addps       %%xmm5, %%xmm3")                            // xmm3     = s*a2 + s2*b2 = p2

            // Shift buffer and store
            __ASM_EMIT("test        $0x8, %[mask]")
            __ASM_EMIT("shufps      $0x93, %%xmm1, %%xmm1")                     // xmm1     = s2[3] s2[0] s2[1] s2[2]
            __ASM_EMIT("jz          7f")
            __ASM_EMIT("movss       %%xmm1, (%[dst])")                          // *dst     = s2[3]
            __ASM_EMIT("add         $4, %[dst]")                                // dst      ++
            __ASM_EMIT("7:")

            // Update delay only by mask
            __ASM_EMIT("addps       %%xmm7, %%xmm2")                            // xmm2     = p1 + d1
            __ASM_EMIT("movaps      %%xmm0, %%xmm4")                            // xmm4     = MASK
            __ASM_EMIT("movaps      %%xmm0, %%xmm5")                            // xmm5     = MASK
            __ASM_EMIT("andps       %%xmm4, %%xmm2")                            // xmm2     = (p1 + d1) & MASK
            __ASM_EMIT("andps       %%xmm5, %%xmm3")                            // xmm3     = p2 & MASK
            __ASM_EMIT("andnps      %%xmm6, %%xmm4")                            // xmm4     = d0 & ~MASK
            __ASM_EMIT("andnps      %%xmm7, %%xmm5")                            // xmm5     = d1 & ~MASK
            __ASM_EMIT("orps        %%xmm2, %%xmm4")                            // xmm4     = (p1 + d1) & MASK | (d0 & ~MASK)
            __ASM_EMIT("orps        %%xmm3, %%xmm5")                            // xmm5     = (p2 & MASK) | (d1 & ~MASK)
            __ASM_EMIT("movaps      %%xmm4, %%xmm6")                            // xmm6     = d0 & ~MASK
            __ASM_EMIT("movaps      %%xmm5, %%xmm7")                            // xmm7     = d1 & ~MASK

            // Repeat loop
            __ASM_EMIT("add      $" DYN_BIQUAD_X4_SSIZE ", %[f]")               // f++
            __ASM_EMIT("xorps       %%xmm2, %%xmm2")                            // xmm2     = 0 0 0 0
            __ASM_EMIT("shl         $1, %[mask]")                               // mask     = mask << 1
            __ASM_EMIT("shufps      $0x90, %%xmm0, %%xmm0")                     // xmm0     = m[0] m[0] m[1] m[2]
            __ASM_EMIT("and         $0x0f, %[mask]")                            // mask     = (mask << 1) & 0x0f
            __ASM_EMIT("movss       %%xmm2, %%xmm0")                            // xmm0     = 0 m[0] m[1] m[2]
            __ASM_EMIT("jnz         5b")                                        // check that mask is not zero

            // Store delay buffer
            __ASM_EMIT32("mov       %[d], %[f]")
            __ASM_EMIT32("movups    %%xmm6, 0x00(%[f])")                        // xmm6     = d0
            __ASM_EMIT32("movups    %%xmm7, 0x10(%[f])")                        // xmm7     = d1
            __ASM_EMIT64("movups    %%xmm6, 0x00(%[d])")                        // xmm6     = d0
            __ASM_EMIT64("movups    %%xmm7, 0x10(%[d])")                        // xmm7     = d1

            // Exit label
            __ASM_EMIT("8:")

            : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count), [f] "+r" (f),
              [mask] "=&r"(mask)
            : __IF_64([d] "r" (d),)
              __IF_32([d] "g" (d), [f_s] "m" (f_s), )
              [X_MASK] "m" (X_MASK0001),
              [MASK] "m" (MASK)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void dyn_biquad_process_x8(float *dst, const float *src, float *d, size_t count, const biquad_x8_t *f)
    {
        float   MASK[4] __lsp_aligned16;
        size_t mask;

#pragma pack(push)
        struct {
            float  *f;
            float  *dst;
            size_t  count;
        } context;
#pragma pack(pop)

        ARCH_X86_ASM
        (
            // Check count
            __ASM_EMIT("test        %[count], %[count]")
            __ASM_EMIT("jz          10f")

            //---------------------------------------------------------------------
            // Cycle 1
            __ASM_EMIT("mov         %[f], 0x00 + %[context]")
            __ASM_EMIT32("mov       %[dst], 0x04 + %[context]")
            __ASM_EMIT32("mov       %[count], 0x08 + %[context]")
            __ASM_EMIT64("mov       %[dst], 0x08 + %[context]")
            __ASM_EMIT64("mov       %[count], 0x10 + %[context]")

            // Initialize mask
            // xmm0=tmp, xmm1={s,s2[4]}, xmm2=p1[4], xmm3=p2[4], xmm6=d0[4], xmm7=d1[4]
            __ASM_EMIT("mov         $1, %[mask]")
            __ASM_EMIT("movaps      %[X_MASK], %%xmm0")
            __ASM_EMIT("xorps       %%xmm1, %%xmm1")
            __ASM_EMIT("movaps      %%xmm0, %[MASK]")

            // Load delay buffer
            __ASM_EMIT32("mov       %[d], %[f]")
            __ASM_EMIT32("movups    0x00(%[f]), %%xmm6")                        // xmm6     = d0
            __ASM_EMIT32("movups    0x20(%[f]), %%xmm7")                        // xmm7     = d1
            __ASM_EMIT32("mov       0x00 + %[context], %[f]")
            __ASM_EMIT64("movups    0x00(%[d]), %%xmm6")                        // xmm6     = d0
            __ASM_EMIT64("movups    0x20(%[d]), %%xmm7")                        // xmm7     = d1

            // Process first 3 steps
            __ASM_EMIT(".align 16")
            __ASM_EMIT("1:")
            __ASM_EMIT("movss       (%[src]), %%xmm0")                          // xmm0     = *src
            __ASM_EMIT("add         $4, %[src]")                                // src      ++
            __ASM_EMIT("movss       %%xmm0, %%xmm1")                            // xmm1     = s
            __ASM_EMIT("movaps      %%xmm1, %%xmm2")                            // xmm2     = s
            __ASM_EMIT("movaps      %%xmm1, %%xmm3")                            // xmm3     = s
            __ASM_EMIT("mulps       " DYN_BIQUAD_X8_A0_SOFF "(%[f]), %%xmm1")   // xmm1     = s*a0
            __ASM_EMIT("mulps       " DYN_BIQUAD_X8_A1_SOFF "(%[f]), %%xmm2")   // xmm2     = s*a1
            __ASM_EMIT("addps       %%xmm6, %%xmm1")                            // xmm1     = s*a0+d0 = s2
            __ASM_EMIT("mulps       " DYN_BIQUAD_X8_A2_SOFF "(%[f]), %%xmm3")   // xmm3     = s*a2
            __ASM_EMIT("movaps      %%xmm1, %%xmm4")                            // xmm4     = s2
            __ASM_EMIT("movaps      %%xmm1, %%xmm5")                            // xmm5     = s2
            __ASM_EMIT("mulps       " DYN_BIQUAD_X8_B1_SOFF "(%[f]), %%xmm4")   // xmm4     = s2*b1
            __ASM_EMIT("mulps       " DYN_BIQUAD_X8_B2_SOFF "(%[f]), %%xmm5")   // xmm5     = s2*b2
            __ASM_EMIT("addps       %%xmm4, %%xmm2")                            // xmm2     = s*a1 + s2*b1 = p1
            __ASM_EMIT("addps       %%xmm5, %%xmm3")                            // xmm3     = s*a2 + s2*b2 = p2

            // Shift buffer
            __ASM_EMIT("shufps      $0x90, %%xmm1, %%xmm1")                     // xmm1     = s2[0] s2[0] s2[1] s2[2]

            // Update delay only by mask
            __ASM_EMIT("addps       %%xmm7, %%xmm2")                            // xmm2     = p1 + d1
            __ASM_EMIT("movaps      %[MASK], %%xmm0")                           // xmm0     = MASK
            __ASM_EMIT("movaps      %%xmm0, %%xmm4")                            // xmm4     = MASK
            __ASM_EMIT("movaps      %%xmm0, %%xmm5")                            // xmm5     = MASK
            __ASM_EMIT("andps       %%xmm4, %%xmm2")                            // xmm2     = (p1 + d1) & MASK
            __ASM_EMIT("andps       %%xmm5, %%xmm3")                            // xmm3     = p2 & MASK
            __ASM_EMIT("andnps      %%xmm6, %%xmm4")                            // xmm4     = d0 & ~MASK
            __ASM_EMIT("andnps      %%xmm7, %%xmm5")                            // xmm5     = d1 & ~MASK
            __ASM_EMIT("orps        %%xmm2, %%xmm4")                            // xmm4     = (p1 + d1) & MASK | (d0 & ~MASK)
            __ASM_EMIT("orps        %%xmm3, %%xmm5")                            // xmm5     = (p2 & MASK) | (d1 & ~MASK)
            __ASM_EMIT("movaps      %%xmm4, %%xmm6")                            // xmm6     = d0 & ~MASK
            __ASM_EMIT("movaps      %%xmm5, %%xmm7")                            // xmm7     = d1 & ~MASK

            // Repeat loop
            __ASM_EMIT("add      $" DYN_BIQUAD_X8_SSIZE ", %[f]")               // f++
            __ASM_EMIT("dec         %[count]")
            __ASM_EMIT("jz          4f")                                        // jump to completion
            __ASM_EMIT("lea         0x01(,%[mask], 2), %[mask]")                // mask     = (mask << 1) | 1
            __ASM_EMIT("shufps      $0x90, %%xmm0, %%xmm0")                     // xmm0     = m[0] m[0] m[1] m[2]
            __ASM_EMIT("movaps      %%xmm0, %[MASK]")                           // store mask
            __ASM_EMIT("cmp         $0x0f, %[mask]")
            __ASM_EMIT("jne         1b")

            // 4x filter processing without mask
            __ASM_EMIT(".align 16")
            __ASM_EMIT("3:")
            __ASM_EMIT("movss       (%[src]), %%xmm0")                          // xmm0     = *src
            __ASM_EMIT("add         $4, %[src]")                                // src      ++
            __ASM_EMIT("movss       %%xmm0, %%xmm1")                            // xmm1     = s
            __ASM_EMIT("movaps      %%xmm1, %%xmm2")                            // xmm2     = s
            __ASM_EMIT("movaps      %%xmm1, %%xmm3")                            // xmm3     = s
            __ASM_EMIT("mulps       " DYN_BIQUAD_X8_A0_SOFF "(%[f]), %%xmm1")   // xmm1     = s*a0
            __ASM_EMIT("mulps       " DYN_BIQUAD_X8_A1_SOFF "(%[f]), %%xmm2")   // xmm2     = s*a1
            __ASM_EMIT("addps       %%xmm6, %%xmm1")                            // xmm1     = s*a0+d0 = s2
            __ASM_EMIT("mulps       " DYN_BIQUAD_X8_A2_SOFF "(%[f]), %%xmm3")   // xmm3     = s*a2
            __ASM_EMIT("movaps      %%xmm1, %%xmm4")                            // xmm4     = s2
            __ASM_EMIT("movaps      %%xmm1, %%xmm5")                            // xmm5     = s2
            __ASM_EMIT("mulps       " DYN_BIQUAD_X8_B1_SOFF "(%[f]), %%xmm4")   // xmm4     = s2*b1
            __ASM_EMIT("mulps       " DYN_BIQUAD_X8_B2_SOFF "(%[f]), %%xmm5")   // xmm5     = s2*b2
            __ASM_EMIT("addps       %%xmm4, %%xmm2")                            // xmm2     = s*a1 + s2*b1 = p1
            __ASM_EMIT("addps       %%xmm5, %%xmm3")                            // xmm3     = s*a2 + s2*b2 = p2
            __ASM_EMIT("addps       %%xmm7, %%xmm2")                            // xmm2     = p1 + d1
            __ASM_EMIT("movaps      %%xmm3, %%xmm7")                            // xmm7     = p2
            __ASM_EMIT("movaps      %%xmm2, %%xmm6")                            // xmm6     = p1 + d1

            // Shift buffer and repeat loop
            __ASM_EMIT("add      $" DYN_BIQUAD_X8_SSIZE ", %[f]")               // f++
            __ASM_EMIT("shufps      $0x93, %%xmm1, %%xmm1")                     // xmm1     = s2[3] s2[0] s2[1] s2[2]
            __ASM_EMIT("movss       %%xmm1, (%[dst])")                          // *dst     = s2[3]
            __ASM_EMIT("add         $4, %[dst]")                                // dst      ++
            __ASM_EMIT("dec         %[count]")
            __ASM_EMIT("jnz         3b")

            // Prepare last loop
            __ASM_EMIT("4:")
            __ASM_EMIT("movaps      %[MASK], %%xmm0")                           // xmm0     = m[0] m[1] m[2] m[3]
            __ASM_EMIT("xorps       %%xmm2, %%xmm2")                            // xmm2     = 0 0 0 0
            __ASM_EMIT("shl         $1, %[mask]")                               // mask     = mask << 1
            __ASM_EMIT("shufps      $0x90, %%xmm0, %%xmm0")                     // xmm0     = m[0] m[0] m[1] m[2]
            __ASM_EMIT("and         $0x0f, %[mask]")                            // mask     = (mask << 1) & 0x0f
            __ASM_EMIT("movss       %%xmm2, %%xmm0")                            // xmm0     = 0 m[0] m[1] m[2]

            // Process steps
            __ASM_EMIT(".align 16")
            __ASM_EMIT("5:")
            __ASM_EMIT("movaps      %%xmm1, %%xmm2")                            // xmm2     = s
            __ASM_EMIT("movaps      %%xmm1, %%xmm3")                            // xmm3     = s
            __ASM_EMIT("mulps       " DYN_BIQUAD_X8_A0_SOFF "(%[f]), %%xmm1")   // xmm1     = s*a0
            __ASM_EMIT("mulps       " DYN_BIQUAD_X8_A1_SOFF "(%[f]), %%xmm2")   // xmm2     = s*a1
            __ASM_EMIT("addps       %%xmm6, %%xmm1")                            // xmm1     = s*a0+d0 = s2
            __ASM_EMIT("mulps       " DYN_BIQUAD_X8_A2_SOFF "(%[f]), %%xmm3")   // xmm3     = s*a2
            __ASM_EMIT("movaps      %%xmm1, %%xmm4")                            // xmm4     = s2
            __ASM_EMIT("movaps      %%xmm1, %%xmm5")                            // xmm5     = s2
            __ASM_EMIT("mulps       " DYN_BIQUAD_X8_B1_SOFF "(%[f]), %%xmm4")   // xmm4     = s2*b1
            __ASM_EMIT("mulps       " DYN_BIQUAD_X8_B2_SOFF "(%[f]), %%xmm5")   // xmm5     = s2*b2
            __ASM_EMIT("addps       %%xmm4, %%xmm2")                            // xmm2     = s*a1 + s2*b1 = p1
            __ASM_EMIT("addps       %%xmm5, %%xmm3")                            // xmm3     = s*a2 + s2*b2 = p2

            // Shift buffer and store
            __ASM_EMIT("test        $0x8, %[mask]")
            __ASM_EMIT("shufps      $0x93, %%xmm1, %%xmm1")                     // xmm1     = s2[3] s2[0] s2[1] s2[2]
            __ASM_EMIT("jz          7f")
            __ASM_EMIT("movss       %%xmm1, (%[dst])")                          // *dst     = s2[3]
            __ASM_EMIT("add         $4, %[dst]")                                // dst      ++
            __ASM_EMIT("7:")

            // Update delay only by mask
            __ASM_EMIT("addps       %%xmm7, %%xmm2")                            // xmm2     = p1 + d1
            __ASM_EMIT("movaps      %%xmm0, %%xmm4")                            // xmm4     = MASK
            __ASM_EMIT("movaps      %%xmm0, %%xmm5")                            // xmm5     = MASK
            __ASM_EMIT("andps       %%xmm4, %%xmm2")                            // xmm2     = (p1 + d1) & MASK
            __ASM_EMIT("andps       %%xmm5, %%xmm3")                            // xmm3     = p2 & MASK
            __ASM_EMIT("andnps      %%xmm6, %%xmm4")                            // xmm4     = d0 & ~MASK
            __ASM_EMIT("andnps      %%xmm7, %%xmm5")                            // xmm5     = d1 & ~MASK
            __ASM_EMIT("orps        %%xmm2, %%xmm4")                            // xmm4     = (p1 + d1) & MASK | (d0 & ~MASK)
            __ASM_EMIT("orps        %%xmm3, %%xmm5")                            // xmm5     = (p2 & MASK) | (d1 & ~MASK)
            __ASM_EMIT("movaps      %%xmm4, %%xmm6")                            // xmm6     = d0 & ~MASK
            __ASM_EMIT("movaps      %%xmm5, %%xmm7")                            // xmm7     = d1 & ~MASK

            // Repeat loop
            __ASM_EMIT("add      $" DYN_BIQUAD_X8_SSIZE ", %[f]")               // f++
            __ASM_EMIT("xorps       %%xmm2, %%xmm2")                            // xmm2     = 0 0 0 0
            __ASM_EMIT("shl         $1, %[mask]")                               // mask     = mask << 1
            __ASM_EMIT("shufps      $0x90, %%xmm0, %%xmm0")                     // xmm0     = m[0] m[0] m[1] m[2]
            __ASM_EMIT("and         $0x0f, %[mask]")                            // mask     = (mask << 1) & 0x0f
            __ASM_EMIT("movss       %%xmm2, %%xmm0")                            // xmm0     = 0 m[0] m[1] m[2]
            __ASM_EMIT("jnz         5b")                                        // check that mask is not zero

            // Store delay buffer
            __ASM_EMIT32("mov       %[d], %[f]")
            __ASM_EMIT32("movups    %%xmm6, 0x00(%[f])")                        // xmm6     = d0
            __ASM_EMIT32("movups    %%xmm7, 0x20(%[f])")                        // xmm7     = d1
            __ASM_EMIT64("movups    %%xmm6, 0x00(%[d])")                        // xmm6     = d0
            __ASM_EMIT64("movups    %%xmm7, 0x20(%[d])")                        // xmm7     = d1

            //---------------------------------------------------------------------
            // Cycle 2
            __ASM_EMIT32("mov       0x04 + %[context], %[dst]")
            __ASM_EMIT32("mov       0x08 + %[context], %[count]")
            __ASM_EMIT64("mov       0x08 + %[context], %[dst]")
            __ASM_EMIT64("mov       0x10 + %[context], %[count]")
            __ASM_EMIT("mov         %[dst], %[src]")                            // Chaining filter groups

            // Initialize mask
            // xmm0=tmp, xmm1={s,s2[4]}, xmm2=p1[4], xmm3=p2[4], xmm6=d0[4], xmm7=d1[4]
            __ASM_EMIT("mov         $1, %[mask]")
            __ASM_EMIT("movaps      %[X_MASK], %%xmm0")
            __ASM_EMIT("xorps       %%xmm1, %%xmm1")
            __ASM_EMIT("movaps      %%xmm0, %[MASK]")

            // Load delay buffer
            __ASM_EMIT32("movups    0x10(%[f]), %%xmm6")                        // xmm6     = d0
            __ASM_EMIT32("movups    0x30(%[f]), %%xmm7")                        // xmm7     = d1
            __ASM_EMIT64("movups    0x10(%[d]), %%xmm6")                        // xmm6     = d0
            __ASM_EMIT64("movups    0x30(%[d]), %%xmm7")                        // xmm7     = d1
            __ASM_EMIT("mov         0x00 + %[context], %[f]")

            // Process first 3 steps
            __ASM_EMIT(".align 16")
            __ASM_EMIT("1:")
            __ASM_EMIT("movss       (%[src]), %%xmm0")                          // xmm0     = *src
            __ASM_EMIT("add         $4, %[src]")                                // src      ++
            __ASM_EMIT("movss       %%xmm0, %%xmm1")                            // xmm1     = s
            __ASM_EMIT("movaps      %%xmm1, %%xmm2")                            // xmm2     = s
            __ASM_EMIT("movaps      %%xmm1, %%xmm3")                            // xmm3     = s
            __ASM_EMIT("mulps       " DYN_BIQUAD_X8_I0_SOFF "(%[f]), %%xmm1")   // xmm1     = s*a0
            __ASM_EMIT("mulps       " DYN_BIQUAD_X8_I1_SOFF "(%[f]), %%xmm2")   // xmm2     = s*a1
            __ASM_EMIT("addps       %%xmm6, %%xmm1")                            // xmm1     = s*a0+d0 = s2
            __ASM_EMIT("mulps       " DYN_BIQUAD_X8_I2_SOFF "(%[f]), %%xmm3")   // xmm3     = s*a2
            __ASM_EMIT("movaps      %%xmm1, %%xmm4")                            // xmm4     = s2
            __ASM_EMIT("movaps      %%xmm1, %%xmm5")                            // xmm5     = s2
            __ASM_EMIT("mulps       " DYN_BIQUAD_X8_J1_SOFF "(%[f]), %%xmm4")   // xmm4     = s2*b1
            __ASM_EMIT("mulps       " DYN_BIQUAD_X8_J2_SOFF "(%[f]), %%xmm5")   // xmm5     = s2*b2
            __ASM_EMIT("addps       %%xmm4, %%xmm2")                            // xmm2     = s*a1 + s2*b1 = p1
            __ASM_EMIT("addps       %%xmm5, %%xmm3")                            // xmm3     = s*a2 + s2*b2 = p2

            // Shift buffer
            __ASM_EMIT("shufps      $0x90, %%xmm1, %%xmm1")                     // xmm1     = s2[0] s2[0] s2[1] s2[2]

            // Update delay only by mask
            __ASM_EMIT("addps       %%xmm7, %%xmm2")                            // xmm2     = p1 + d1
            __ASM_EMIT("movaps      %[MASK], %%xmm0")                           // xmm0     = MASK
            __ASM_EMIT("movaps      %%xmm0, %%xmm4")                            // xmm4     = MASK
            __ASM_EMIT("movaps      %%xmm0, %%xmm5")                            // xmm5     = MASK
            __ASM_EMIT("andps       %%xmm4, %%xmm2")                            // xmm2     = (p1 + d1) & MASK
            __ASM_EMIT("andps       %%xmm5, %%xmm3")                            // xmm3     = p2 & MASK
            __ASM_EMIT("andnps      %%xmm6, %%xmm4")                            // xmm4     = d0 & ~MASK
            __ASM_EMIT("andnps      %%xmm7, %%xmm5")                            // xmm5     = d1 & ~MASK
            __ASM_EMIT("orps        %%xmm2, %%xmm4")                            // xmm4     = (p1 + d1) & MASK | (d0 & ~MASK)
            __ASM_EMIT("orps        %%xmm3, %%xmm5")                            // xmm5     = (p2 & MASK) | (d1 & ~MASK)
            __ASM_EMIT("movaps      %%xmm4, %%xmm6")                            // xmm6     = d0 & ~MASK
            __ASM_EMIT("movaps      %%xmm5, %%xmm7")                            // xmm7     = d1 & ~MASK

            // Repeat loop
            __ASM_EMIT("add      $" DYN_BIQUAD_X8_SSIZE ", %[f]")               // f++
            __ASM_EMIT("dec         %[count]")
            __ASM_EMIT("jz          4f")                                        // jump to completion
            __ASM_EMIT("lea         0x01(,%[mask], 2), %[mask]")                // mask     = (mask << 1) | 1
            __ASM_EMIT("shufps      $0x90, %%xmm0, %%xmm0")                     // xmm0     = m[0] m[0] m[1] m[2]
            __ASM_EMIT("movaps      %%xmm0, %[MASK]")                           // store mask
            __ASM_EMIT("cmp         $0x0f, %[mask]")
            __ASM_EMIT("jne         1b")

            // 4x filter processing without mask
            __ASM_EMIT(".align 16")
            __ASM_EMIT("3:")
            __ASM_EMIT("movss       (%[src]), %%xmm0")                          // xmm0     = *src
            __ASM_EMIT("add         $4, %[src]")                                // src      ++
            __ASM_EMIT("movss       %%xmm0, %%xmm1")                            // xmm1     = s
            __ASM_EMIT("movaps      %%xmm1, %%xmm2")                            // xmm2     = s
            __ASM_EMIT("movaps      %%xmm1, %%xmm3")                            // xmm3     = s
            __ASM_EMIT("mulps       " DYN_BIQUAD_X8_I0_SOFF "(%[f]), %%xmm1")   // xmm1     = s*a0
            __ASM_EMIT("mulps       " DYN_BIQUAD_X8_I1_SOFF "(%[f]), %%xmm2")   // xmm2     = s*a1
            __ASM_EMIT("addps       %%xmm6, %%xmm1")                            // xmm1     = s*a0+d0 = s2
            __ASM_EMIT("mulps       " DYN_BIQUAD_X8_I2_SOFF "(%[f]), %%xmm3")   // xmm3     = s*a2
            __ASM_EMIT("movaps      %%xmm1, %%xmm4")                            // xmm4     = s2
            __ASM_EMIT("movaps      %%xmm1, %%xmm5")                            // xmm5     = s2
            __ASM_EMIT("mulps       " DYN_BIQUAD_X8_J1_SOFF "(%[f]), %%xmm4")   // xmm4     = s2*b1
            __ASM_EMIT("mulps       " DYN_BIQUAD_X8_J2_SOFF "(%[f]), %%xmm5")   // xmm5     = s2*b2
            __ASM_EMIT("addps       %%xmm4, %%xmm2")                            // xmm2     = s*a1 + s2*b1 = p1
            __ASM_EMIT("addps       %%xmm5, %%xmm3")                            // xmm3     = s*a2 + s2*b2 = p2
            __ASM_EMIT("addps       %%xmm7, %%xmm2")                            // xmm2     = p1 + d1
            __ASM_EMIT("movaps      %%xmm3, %%xmm7")                            // xmm7     = p2
            __ASM_EMIT("movaps      %%xmm2, %%xmm6")                            // xmm6     = p1 + d1

            // Shift buffer and repeat loop
            __ASM_EMIT("add      $" DYN_BIQUAD_X8_SSIZE ", %[f]")               // f++
            __ASM_EMIT("shufps      $0x93, %%xmm1, %%xmm1")                     // xmm1     = s2[3] s2[0] s2[1] s2[2]
            __ASM_EMIT("movss       %%xmm1, (%[dst])")                          // *dst     = s2[3]
            __ASM_EMIT("add         $4, %[dst]")                                // dst      ++
            __ASM_EMIT("dec         %[count]")
            __ASM_EMIT("jnz         3b")

            // Prepare last loop
            __ASM_EMIT("4:")
            __ASM_EMIT("movaps      %[MASK], %%xmm0")                           // xmm0     = m[0] m[1] m[2] m[3]
            __ASM_EMIT("xorps       %%xmm2, %%xmm2")                            // xmm2     = 0 0 0 0
            __ASM_EMIT("shl         $1, %[mask]")                               // mask     = mask << 1
            __ASM_EMIT("shufps      $0x90, %%xmm0, %%xmm0")                     // xmm0     = m[0] m[0] m[1] m[2]
            __ASM_EMIT("and         $0x0f, %[mask]")                            // mask     = (mask << 1) & 0x0f
            __ASM_EMIT("movss       %%xmm2, %%xmm0")                            // xmm0     = 0 m[0] m[1] m[2]

            // Process steps
            __ASM_EMIT(".align 16")
            __ASM_EMIT("5:")
            __ASM_EMIT("movaps      %%xmm1, %%xmm2")                            // xmm2     = s
            __ASM_EMIT("movaps      %%xmm1, %%xmm3")                            // xmm3     = s
            __ASM_EMIT("mulps       " DYN_BIQUAD_X8_I0_SOFF "(%[f]), %%xmm1")   // xmm1     = s*a0
            __ASM_EMIT("mulps       " DYN_BIQUAD_X8_I1_SOFF "(%[f]), %%xmm2")   // xmm2     = s*a1
            __ASM_EMIT("addps       %%xmm6, %%xmm1")                            // xmm1     = s*a0+d0 = s2
            __ASM_EMIT("mulps       " DYN_BIQUAD_X8_I2_SOFF "(%[f]), %%xmm3")   // xmm3     = s*a2
            __ASM_EMIT("movaps      %%xmm1, %%xmm4")                            // xmm4     = s2
            __ASM_EMIT("movaps      %%xmm1, %%xmm5")                            // xmm5     = s2
            __ASM_EMIT("mulps       " DYN_BIQUAD_X8_J1_SOFF "(%[f]), %%xmm4")   // xmm4     = s2*b1
            __ASM_EMIT("mulps       " DYN_BIQUAD_X8_J2_SOFF "(%[f]), %%xmm5")   // xmm5     = s2*b2
            __ASM_EMIT("addps       %%xmm4, %%xmm2")                            // xmm2     = s*a1 + s2*b1 = p1
            __ASM_EMIT("addps       %%xmm5, %%xmm3")                            // xmm3     = s*a2 + s2*b2 = p2

            // Shift buffer and store
            __ASM_EMIT("test        $0x8, %[mask]")
            __ASM_EMIT("shufps      $0x93, %%xmm1, %%xmm1")                     // xmm1     = s2[3] s2[0] s2[1] s2[2]
            __ASM_EMIT("jz          7f")
            __ASM_EMIT("movss       %%xmm1, (%[dst])")                          // *dst     = s2[3]
            __ASM_EMIT("add         $4, %[dst]")                                // dst      ++
            __ASM_EMIT("7:")

            // Update delay only by mask
            __ASM_EMIT("addps       %%xmm7, %%xmm2")                            // xmm2     = p1 + d1
            __ASM_EMIT("movaps      %%xmm0, %%xmm4")                            // xmm4     = MASK
            __ASM_EMIT("movaps      %%xmm0, %%xmm5")                            // xmm5     = MASK
            __ASM_EMIT("andps       %%xmm4, %%xmm2")                            // xmm2     = (p1 + d1) & MASK
            __ASM_EMIT("andps       %%xmm5, %%xmm3")                            // xmm3     = p2 & MASK
            __ASM_EMIT("andnps      %%xmm6, %%xmm4")                            // xmm4     = d0 & ~MASK
            __ASM_EMIT("andnps      %%xmm7, %%xmm5")                            // xmm5     = d1 & ~MASK
            __ASM_EMIT("orps        %%xmm2, %%xmm4")                            // xmm4     = (p1 + d1) & MASK | (d0 & ~MASK)
            __ASM_EMIT("orps        %%xmm3, %%xmm5")                            // xmm5     = (p2 & MASK) | (d1 & ~MASK)
            __ASM_EMIT("movaps      %%xmm4, %%xmm6")                            // xmm6     = (p1 + d1) & MASK | (d0 & ~MASK)
            __ASM_EMIT("movaps      %%xmm5, %%xmm7")                            // xmm7     = (p2 & MASK) | (d1 & ~MASK)

            // Repeat loop
            __ASM_EMIT("add      $" DYN_BIQUAD_X8_SSIZE ", %[f]")               // f++
            __ASM_EMIT("xorps       %%xmm2, %%xmm2")                            // xmm2     = 0 0 0 0
            __ASM_EMIT("shl         $1, %[mask]")                               // mask     = mask << 1
            __ASM_EMIT("shufps      $0x90, %%xmm0, %%xmm0")                     // xmm0     = m[0] m[0] m[1] m[2]
            __ASM_EMIT("and         $0x0f, %[mask]")                            // mask     = (mask << 1) & 0x0f
            __ASM_EMIT("movss       %%xmm2, %%xmm0")                            // xmm0     = 0 m[0] m[1] m[2]
            __ASM_EMIT("jnz         5b")                                        // check that mask is not zero

            // Store delay buffer
            __ASM_EMIT32("mov       %[d], %[f]")
            __ASM_EMIT32("movups    %%xmm6, 0x10(%[f])")                        // xmm6     = d0
            __ASM_EMIT32("movups    %%xmm7, 0x30(%[f])")                        // xmm7     = d1
            __ASM_EMIT64("movups    %%xmm6, 0x10(%[d])")                        // xmm6     = d0
            __ASM_EMIT64("movups    %%xmm7, 0x30(%[d])")                        // xmm7     = d1

            // Exit label
            __ASM_EMIT("10:")

            : [dst] "+r" (dst), [src] "+r" (src), [mask] "=&r" (mask), [count] "+r" (count), [f] "+r" (f)
            : [context] "o" (context),
              __IF_64([d] "r" (d),)
              __IF_32([d] "g" (d),)
              [X_MASK] "m" (X_MASK0001),
              [MASK] "m" (MASK)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

}

#endif /* DSP_ARCH_X86_SSE_FILTERS_DYNAMIC_H_ */
