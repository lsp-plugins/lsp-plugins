/*
 * static.h
 *
 *  Created on: 05 окт. 2015 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_SSE_FILTERS_STATIC_H_
#define DSP_ARCH_X86_SSE_FILTERS_STATIC_H_

#ifndef DSP_ARCH_X86_SSE_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_SSE_IMPL */

void biquad_process_x1(float *dst, const float *src, size_t count, biquad_t *f)
{
    size_t i;

    __asm__ __volatile__
    (
        // Check count
        __ASM_EMIT("test        %[count], %[count]")
        __ASM_EMIT("jz          1f")

        // Load permanent data
        __ASM_EMIT("movaps      " BIQUAD_D0_SOFF "(%[f]), %%xmm4")      // xmm4 = d0 d1 0  0
        __ASM_EMIT("xor         %[i], %[i]")
        __ASM_EMIT("xorps       %%xmm7, %%xmm7")                        // xmm7 = 0  0  0  0
        __ASM_EMIT("movaps      " BIQUAD_X1_A_SOFF "(%[f]), %%xmm6")    // xmm6 = a0 a0 a1 a2
        __ASM_EMIT("movaps      " BIQUAD_X1_B_SOFF "(%[f]), %%xmm5")    // xmm5 = b1 b2 0  0

        // Start loop
        __ASM_EMIT(".align 16")
        __ASM_EMIT("2:")

        // Load data
        __ASM_EMIT("movss       (%[src], %[i], 4), %%xmm0")             // xmm0 = s ? ? ?
        __ASM_EMIT("shufps      $0xd0, %%xmm4, %%xmm4")                 // xmm4 = d0 d0 d1 0
        __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0")                 // xmm0 = s s s s
        __ASM_EMIT("mulps       %%xmm6, %%xmm0")                        // xmm0 = s*a0 s*a0 s*a1 s*a2
        __ASM_EMIT("addps       %%xmm4, %%xmm0")                        // xmm0 = s*a0+d0 s*a0+d0 s*a1+d1 s*a2
        __ASM_EMIT("movaps      %%xmm0, %%xmm4")                        // xmm4 = s*a0+d0 s*a0+d0 s*a1+d1 s*a2
        __ASM_EMIT("movhlps     %%xmm0, %%xmm7")                        // xmm7 = s*a1+d1 s*a2 0 0
        __ASM_EMIT("movss       %%xmm0, (%[dst], %[i], 4)")             // store value
        __ASM_EMIT("mulps       %%xmm5, %%xmm4")                        // xmm4 = (s*a0+d0)*b1 (s*a0+d0)*b2 0 0
        __ASM_EMIT("inc         %[i]")
        __ASM_EMIT("cmp         %[count], %[i]")
        __ASM_EMIT("addps       %%xmm7, %%xmm4")                        // xmm4 = (s*a0+d0)*b1+s*a1+d1 (s*a0+d0)*b2+s*a2 0 0

        // Update pointers and repeat loop
        __ASM_EMIT("jb          2b")

        // Store the updated buffer state
        __ASM_EMIT("movaps      %%xmm4, " BIQUAD_D0_SOFF "(%[f])")

        // Exit label
        __ASM_EMIT("1:")

        : [i] "=&r"(i)
        : [dst] "r" (dst), [src] "r" (src), [count] "r" (count), [f] "r" (f)
        : "cc", "memory",
          "%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5", "%xmm6", "%xmm7"
    );
}

void biquad_process_x2(float *dst, const float *src, size_t count, biquad_t *f)
{
    size_t i;

    __asm__ __volatile__
    (
        // Check count
        __ASM_EMIT("test        %[count], %[count]")
        __ASM_EMIT("jz          1f")

        // Load permanent data
        __ASM_EMIT("movaps      " BIQUAD_D0_SOFF "(%[f]), %%xmm4")      // xmm4 = d0 d1 0  0
        __ASM_EMIT("movaps      " BIQUAD_D1_SOFF "(%[f]), %%xmm5")      // xmm5 = e0 e1 0  0
        __ASM_EMIT("xor         %[i], %[i]")
        __ASM_EMIT("xorps       %%xmm6, %%xmm6")                        // xmm6 = 0  0  0  0
        __ASM_EMIT("xorps       %%xmm7, %%xmm7")                        // xmm7 = 0  0  0  0

        // Process first filter only
        __ASM_EMIT("movss       (%[src], %[i], 4), %%xmm1")             // xmm1 = s ? ? ?
        __ASM_EMIT("shufps      $0xd0, %%xmm4, %%xmm4")                 // xmm4 = d0 d0 d1 0
        __ASM_EMIT("shufps      $0x00, %%xmm1, %%xmm1")                 // xmm1 = s s s s
        __ASM_EMIT("mulps       " BIQUAD_X2_A_SOFF "(%[f]), %%xmm1")    // xmm1 = s*a0 s*a0 s*a1 s*a2
        __ASM_EMIT("addps       %%xmm4, %%xmm1")                        // xmm1 = s*a0+d0 s*a0+d0 s*a1+d1 s*a2
        __ASM_EMIT("movaps      %%xmm1, %%xmm4")                        // xmm4 = s*a0+d0 s*a0+d0 s*a1+d1 s*a2
        __ASM_EMIT("movhlps     %%xmm1, %%xmm6")                        // xmm6 = s*a1+d1 s*a2 0 0
        __ASM_EMIT("inc         %[i]")
        __ASM_EMIT("mulps       " BIQUAD_X2_B_SOFF "(%[f]), %%xmm4")    // xmm4 = (s*a0+d0)*b1 (s*a0+d0)*b2 0 0
        __ASM_EMIT("cmp         %[count], %[i]")
        __ASM_EMIT("addps       %%xmm6, %%xmm4")                        // xmm4 = (s*a0+d0)*b1+s*a1+d1 (s*a0+d0)*b2+s*a2 0 0
        __ASM_EMIT("jae         3f")

        // Process two filters simultaneously
        __ASM_EMIT(".align 16")
        __ASM_EMIT("2:")
        __ASM_EMIT("movss       (%[src], %[i], 4), %%xmm0")             // xmm0 = s ? ? ?
        __ASM_EMIT("shufps      $0xd0, %%xmm5, %%xmm5")                 // xmm5 = e0 e0 e1 0
        __ASM_EMIT("shufps      $0xd0, %%xmm4, %%xmm4")                 // xmm4 = d0 d0 d1 0
        __ASM_EMIT("shufps      $0x00, %%xmm1, %%xmm1")                 // xmm1 = r r r r
        __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0")                 // xmm0 = s s s s
        __ASM_EMIT("mulps       " BIQUAD_X2_I_SOFF "(%[f]), %%xmm1")    // xmm1 = r*i0 r*i0 r*i1 r*i2
        __ASM_EMIT("mulps       " BIQUAD_X2_A_SOFF "(%[f]), %%xmm0")    // xmm0 = s*a0 s*a0 s*a1 s*a2
        __ASM_EMIT("addps       %%xmm5, %%xmm1")                        // xmm1 = r*i0_e0 r*i0+e0 r*i1+e1 r*i2
        __ASM_EMIT("addps       %%xmm4, %%xmm0")                        // xmm0 = s*a0+d0 s*a0+d0 s*a1+d1 s*a2
        __ASM_EMIT("movaps      %%xmm1, %%xmm5")                        // xmm5 = r*i0+e0 r*i0+e0 r*i1+e1 r*i2
        __ASM_EMIT("movaps      %%xmm0, %%xmm4")                        // xmm4 = s*a0+d0 s*a0+d0 s*a1+d1 s*a2
        __ASM_EMIT("movhlps     %%xmm1, %%xmm7")                        // xmm7 = r*i1+e1 r*i2 0 0
        __ASM_EMIT("movhlps     %%xmm0, %%xmm6")                        // xmm6 = s*a1+d1 s*a2 0 0
        __ASM_EMIT("movss       %%xmm1, -4(%[dst], %[i], 4)")           // store value
        __ASM_EMIT("mulps       " BIQUAD_X2_J_SOFF "(%[f]), %%xmm5")    // xmm5 = (r*i0+e0)*j1 (r*i0+e0)*j2 0 0
        __ASM_EMIT("inc         %[i]")
        __ASM_EMIT("mulps       " BIQUAD_X2_B_SOFF "(%[f]), %%xmm4")    // xmm4 = (s*a0+d0)*b1 (s*a0+d0)*b2 0 0
        __ASM_EMIT("cmp         %[count], %[i]")
        __ASM_EMIT("addps       %%xmm7, %%xmm5")                        // xmm5 = (r*i0+e0)*j1+r*i1+e1 (r*i0+e0)*j2+r*i2 0 0
        __ASM_EMIT("addps       %%xmm6, %%xmm4")                        // xmm4 = (s*a0+d0)*b1+s*a1+d1 (s*a0+d0)*b2+s*a2 0 0
        __ASM_EMIT("movss       %%xmm0, %%xmm1")                        // xmm1 = r ? ? ?

        // Update pointers and repeat loop
        __ASM_EMIT("jb          2b")

        // Process second filter only
        __ASM_EMIT("3:")
        __ASM_EMIT("shufps      $0xd0, %%xmm5, %%xmm5")                 // xmm5 = e0 e0 e1 0
        __ASM_EMIT("shufps      $0x00, %%xmm1, %%xmm1")                 // xmm1 = r r r r
        __ASM_EMIT("mulps       " BIQUAD_X2_I_SOFF "(%[f]), %%xmm1")    // xmm1 = r*i0 r*i0 r*i1 r*i2
        __ASM_EMIT("addps       %%xmm5, %%xmm1")                        // xmm1 = r*i0_e0 r*i0+e0 r*i1+e1 r*i2
        __ASM_EMIT("movaps      %%xmm1, %%xmm5")                        // xmm5 = r*i0+e0 r*i0+e0 r*i1+e1 r*i2
        __ASM_EMIT("movhlps     %%xmm1, %%xmm7")                        // xmm7 = r*i1+e1 r*i2 0 0
        __ASM_EMIT("movss       %%xmm1, -4(%[dst], %[i], 4)")           // store value
        __ASM_EMIT("mulps       " BIQUAD_X2_J_SOFF "(%[f]), %%xmm5")    // xmm5 = (r*i0+e0)*j1 (r*i0+e0)*j2 0 0
        __ASM_EMIT("addps       %%xmm7, %%xmm5")                        // xmm5 = (r*i0+e0)*j1+r*i1+e1 (r*i0+e0)*j2+r*i2 0 0

        // Store the updated buffer state
        __ASM_EMIT("movaps      %%xmm4, " BIQUAD_D0_SOFF "(%[f])")
        __ASM_EMIT("movaps      %%xmm5, " BIQUAD_D1_SOFF "(%[f])")

        // Exit label
        __ASM_EMIT("1:")

        : [i] "=&r"(i)
        : [dst] "r" (dst), [src] "r" (src), [count] "r" (count), [f] "r" (f)
        : "cc", "memory",
          "%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5", "%xmm6", "%xmm7"
    );
}

void biquad_process_x4(float *dst, const float *src, size_t count, biquad_t *f)
{
    float   MASK[4] __lsp_aligned16;
    size_t  mask;

    __asm__ __volatile__
    (
        // Check count
        __ASM_EMIT("test        %[count], %[count]")
        __ASM_EMIT("jz          4f")

        // Initialize mask
        // xmm0=tmp, xmm1={s,s2[4]}, xmm2=p1[4], xmm3=p2[4], xmm6=d0[4], xmm7=d1[4]
        __ASM_EMIT("mov         $1, %[mask]")
        __ASM_EMIT("movaps      %[X_MASK], %%xmm0")
        __ASM_EMIT("xorps       %%xmm1, %%xmm1")
        __ASM_EMIT("movaps      %%xmm0, %[MASK]")

        // Load delay buffer
        __ASM_EMIT("movaps      " BIQUAD_D0_SOFF "(%[f]), %%xmm6")      // xmm6     = d0
        __ASM_EMIT("movaps      " BIQUAD_D1_SOFF "(%[f]), %%xmm7")      // xmm7     = d1

        // Process first 3 steps
        __ASM_EMIT(".align 16")
        __ASM_EMIT("1:")
        __ASM_EMIT("movss       (%[src]), %%xmm0")                      // xmm0     = *src
        __ASM_EMIT("add         $4, %[src]")                            // src      ++
        __ASM_EMIT("movss       %%xmm0, %%xmm1")                        // xmm1     = s
        __ASM_EMIT("movaps      %%xmm1, %%xmm2")                        // xmm2     = s
        __ASM_EMIT("movaps      %%xmm1, %%xmm3")                        // xmm3     = s
        __ASM_EMIT("mulps       " BIQUAD_X4_A0_SOFF "(%[f]), %%xmm1")   // xmm1     = s*a0
        __ASM_EMIT("mulps       " BIQUAD_X4_A1_SOFF "(%[f]), %%xmm2")   // xmm2     = s*a1
        __ASM_EMIT("addps       %%xmm6, %%xmm1")                        // xmm1     = s*a0+d0 = s2
        __ASM_EMIT("mulps       " BIQUAD_X4_A2_SOFF "(%[f]), %%xmm3")   // xmm3     = s*a2
        __ASM_EMIT("movaps      %%xmm1, %%xmm4")                        // xmm4     = s2
        __ASM_EMIT("movaps      %%xmm1, %%xmm5")                        // xmm5     = s2
        __ASM_EMIT("mulps       " BIQUAD_X4_B1_SOFF "(%[f]), %%xmm4")   // xmm4     = s2*b1
        __ASM_EMIT("mulps       " BIQUAD_X4_B2_SOFF "(%[f]), %%xmm5")   // xmm5     = s2*b2
        __ASM_EMIT("addps       %%xmm4, %%xmm2")                        // xmm2     = s*a1 + s2*b1 = p1
        __ASM_EMIT("addps       %%xmm5, %%xmm3")                        // xmm3     = s*a2 + s2*b2 = p2

        // Shift buffer
        __ASM_EMIT("shufps      $0x90, %%xmm1, %%xmm1")                 // xmm1     = s2[0] s2[0] s2[1] s2[2]

        // Update delay only by mask
        __ASM_EMIT("addps       %%xmm7, %%xmm2")                        // xmm2     = p1 + d1
        __ASM_EMIT("movaps      %[MASK], %%xmm0")                       // xmm0     = MASK
        __ASM_EMIT("movaps      %%xmm0, %%xmm4")                        // xmm4     = MASK
        __ASM_EMIT("movaps      %%xmm0, %%xmm5")                        // xmm5     = MASK
        __ASM_EMIT("andps       %%xmm4, %%xmm2")                        // xmm2     = (p1 + d1) & MASK
        __ASM_EMIT("andps       %%xmm5, %%xmm3")                        // xmm3     = p2 & MASK
        __ASM_EMIT("andnps      %%xmm6, %%xmm4")                        // xmm4     = d0 & ~MASK
        __ASM_EMIT("andnps      %%xmm7, %%xmm5")                        // xmm5     = d1 & ~MASK
        __ASM_EMIT("orps        %%xmm2, %%xmm4")                        // xmm4     = (p1 + d1) & MASK | (d0 & ~MASK)
        __ASM_EMIT("orps        %%xmm3, %%xmm5")                        // xmm5     = (p2 & MASK) | (d1 & ~MASK)
        __ASM_EMIT("movaps      %%xmm4, %%xmm6")                        // xmm6     = d0 & ~MASK
        __ASM_EMIT("movaps      %%xmm5, %%xmm7")                        // xmm7     = d1 & ~MASK

        // Repeat loop
        __ASM_EMIT("lea         0x01(,%[mask], 2), %[mask]")            // mask     = (mask << 1) | 1
        __ASM_EMIT("shufps      $0x90, %%xmm0, %%xmm0")                 // xmm0     = m[0] m[0] m[1] m[2]
        __ASM_EMIT("movaps      %%xmm0, %[MASK]")                       // store mask
        __ASM_EMIT("dec         %[count]")
        __ASM_EMIT("jz          3f")                                    // jump to completion
        __ASM_EMIT("cmp         $0x0f, %[mask]")
        __ASM_EMIT("jb          1b")

        // 4x filter processing without mask
        __ASM_EMIT(".align 16")
        __ASM_EMIT("2:")
        __ASM_EMIT("movss       (%[src]), %%xmm0")                      // xmm0     = *src
        __ASM_EMIT("add         $4, %[src]")                            // src      ++
        __ASM_EMIT("movss       %%xmm0, %%xmm1")                        // xmm1     = s
        __ASM_EMIT("movaps      %%xmm1, %%xmm2")                        // xmm2     = s
        __ASM_EMIT("movaps      %%xmm1, %%xmm3")                        // xmm3     = s
        __ASM_EMIT("mulps       " BIQUAD_X4_A0_SOFF "(%[f]), %%xmm1")   // xmm1     = s*a0
        __ASM_EMIT("mulps       " BIQUAD_X4_A1_SOFF "(%[f]), %%xmm2")   // xmm2     = s*a1
        __ASM_EMIT("addps       %%xmm6, %%xmm1")                        // xmm1     = s*a0+d0 = s2
        __ASM_EMIT("mulps       " BIQUAD_X4_A2_SOFF "(%[f]), %%xmm3")   // xmm3     = s*a2
        __ASM_EMIT("movaps      %%xmm1, %%xmm4")                        // xmm4     = s2
        __ASM_EMIT("movaps      %%xmm1, %%xmm5")                        // xmm5     = s2
        __ASM_EMIT("mulps       " BIQUAD_X4_B1_SOFF "(%[f]), %%xmm4")   // xmm4     = s2*b1
        __ASM_EMIT("mulps       " BIQUAD_X4_B2_SOFF "(%[f]), %%xmm5")   // xmm5     = s2*b2
        __ASM_EMIT("addps       %%xmm4, %%xmm2")                        // xmm2     = s*a1 + s2*b1 = p1
        __ASM_EMIT("addps       %%xmm5, %%xmm3")                        // xmm3     = s*a2 + s2*b2 = p2
        __ASM_EMIT("addps       %%xmm7, %%xmm2")                		// xmm2     = p1 + d1
        __ASM_EMIT("movaps      %%xmm3, %%xmm7")                		// xmm7     = p2
        __ASM_EMIT("movaps      %%xmm2, %%xmm6")                		// xmm6     = p1 + d1

        // Shift buffer and repeat loop
        __ASM_EMIT("shufps      $0x93, %%xmm1, %%xmm1")                 // xmm1     = s2[3] s2[0] s2[1] s2[2]
        __ASM_EMIT("movss       %%xmm1, (%[dst])")                      // *dst     = s2[3]
        __ASM_EMIT("add         $4, %[dst]")                            // dst      ++
        __ASM_EMIT("dec         %[count]")
        __ASM_EMIT("jnz          2b")

        // Prepare last loop
        __ASM_EMIT("movaps      %[MASK], %%xmm0")                       // xmm0     = m[0] m[1] m[2] m[3]
        __ASM_EMIT("xorps       %%xmm2, %%xmm2")                        // xmm2     = 0 0 0 0
        __ASM_EMIT("shl         $1, %[mask]")                           // mask     = mask << 1
        __ASM_EMIT("shufps      $0x90, %%xmm0, %%xmm0")                 // xmm0     = m[0] m[0] m[1] m[2]
        __ASM_EMIT("and         $0x0f, %[mask]")                        // mask     = (mask << 1) & 0x0f
        __ASM_EMIT("movss       %%xmm2, %%xmm0")                        // xmm0     = 0 m[0] m[1] m[2]

        // Process steps
        __ASM_EMIT(".align 16")
        __ASM_EMIT("3:")
        __ASM_EMIT("movaps      %%xmm1, %%xmm2")                        // xmm2     = s
        __ASM_EMIT("movaps      %%xmm1, %%xmm3")                        // xmm3     = s
        __ASM_EMIT("mulps       " BIQUAD_X4_A0_SOFF "(%[f]), %%xmm1")   // xmm1     = s*a0
        __ASM_EMIT("mulps       " BIQUAD_X4_A1_SOFF "(%[f]), %%xmm2")   // xmm2     = s*a1
        __ASM_EMIT("addps       %%xmm6, %%xmm1")                        // xmm1     = s*a0+d0 = s2
        __ASM_EMIT("mulps       " BIQUAD_X4_A2_SOFF "(%[f]), %%xmm3")   // xmm3     = s*a2
        __ASM_EMIT("movaps      %%xmm1, %%xmm4")                        // xmm4     = s2
        __ASM_EMIT("movaps      %%xmm1, %%xmm5")                        // xmm5     = s2
        __ASM_EMIT("mulps       " BIQUAD_X4_B1_SOFF "(%[f]), %%xmm4")   // xmm4     = s2*b1
        __ASM_EMIT("mulps       " BIQUAD_X4_B2_SOFF "(%[f]), %%xmm5")   // xmm5     = s2*b2
        __ASM_EMIT("addps       %%xmm4, %%xmm2")                        // xmm2     = s*a1 + s2*b1 = p1
        __ASM_EMIT("addps       %%xmm5, %%xmm3")                        // xmm3     = s*a2 + s2*b2 = p2

        // Shift buffer
        __ASM_EMIT("shufps      $0x93, %%xmm1, %%xmm1")                 // xmm1     = s2[3] s2[0] s2[1] s2[2]
        __ASM_EMIT("movss       %%xmm1, (%[dst])")                      // *dst     = s2[3]
        __ASM_EMIT("add         $4, %[dst]")                            // dst      ++

        // Update delay only by mask
        __ASM_EMIT("addps       %%xmm7, %%xmm2")                        // xmm2     = p1 + d1
        __ASM_EMIT("movaps      %%xmm0, %%xmm4")                        // xmm4     = MASK
        __ASM_EMIT("movaps      %%xmm0, %%xmm5")                        // xmm5     = MASK
        __ASM_EMIT("andps       %%xmm4, %%xmm2")                        // xmm2     = (p1 + d1) & MASK
        __ASM_EMIT("andps       %%xmm5, %%xmm3")                        // xmm3     = p2 & MASK
        __ASM_EMIT("andnps      %%xmm6, %%xmm4")                        // xmm4     = d0 & ~MASK
        __ASM_EMIT("andnps      %%xmm7, %%xmm5")                        // xmm5     = d1 & ~MASK
        __ASM_EMIT("orps        %%xmm2, %%xmm4")                        // xmm4     = (p1 + d1) & MASK | (d0 & ~MASK)
        __ASM_EMIT("orps        %%xmm3, %%xmm5")                        // xmm5     = (p2 & MASK) | (d1 & ~MASK)
        __ASM_EMIT("movaps      %%xmm4, %%xmm6")                        // xmm6     = d0 & ~MASK
        __ASM_EMIT("movaps      %%xmm5, %%xmm7")                        // xmm7     = d1 & ~MASK

        // Repeat loop
        __ASM_EMIT("xorps       %%xmm2, %%xmm2")                        // xmm2     = 0 0 0 0
        __ASM_EMIT("shl         $1, %[mask]")                           // mask     = mask << 1
        __ASM_EMIT("shufps      $0x90, %%xmm0, %%xmm0")                 // xmm0     = m[0] m[0] m[1] m[2]
        __ASM_EMIT("and         $0x0f, %[mask]")                        // mask     = (mask << 1) & 0x0f
        __ASM_EMIT("movss       %%xmm2, %%xmm0")                        // xmm0     = 0 m[0] m[1] m[2]
        __ASM_EMIT("jnz         3b")                                    // check that mask is not zero

        // Store delay buffer
        __ASM_EMIT("movaps      %%xmm6, " BIQUAD_D0_SOFF "(%[f])")      // xmm6     = d0
        __ASM_EMIT("movaps      %%xmm7, " BIQUAD_D1_SOFF "(%[f])")      // xmm7     = d1

        // Exit label
        __ASM_EMIT("4:")

        : [dst] "+r" (dst), [src] "+r" (src), [mask] "=&r"(mask), [count] "+r" (count)
        :
          [f] "r" (f),
          [X_MASK] "m" (X_MASK0001),
          [MASK] "m" (MASK)
        : "cc", "memory",
          "%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5", "%xmm6", "%xmm7"
    );
}

void biquad_process_x8(float *dst, const float *src, size_t count, biquad_t *f)
{
    float   MASK[4] __lsp_aligned16;
    size_t i, mask;
    float          *dst_bk;

    __asm__ __volatile__
    (
        // Check count
        __ASM_EMIT("test        %[count], %[count]")
        __ASM_EMIT("jz          8f")

        //---------------------------------------------------------------------
        // Cycle 1
        __ASM_EMIT("mov         %[dst], %[dst_bk]")

        // Initialize mask
        // xmm0=tmp, xmm1={s,s2[4]}, xmm2=p1[4], xmm3=p2[4], xmm6=d0[4], xmm7=d1[4]
        __ASM_EMIT32("movl      $1, %[mask]")
        __ASM_EMIT64("movq      $1, %[mask]")
        __ASM_EMIT("xor         %[i], %[i]")
        __ASM_EMIT("movaps      %[X_MASK], %%xmm0")
        __ASM_EMIT("movaps      %%xmm0, %[MASK]")
        __ASM_EMIT("xorps       %%xmm1, %%xmm1")

        // Load delay buffer
        __ASM_EMIT("movaps      " BIQUAD_D0_SOFF "(%[f]), %%xmm6")      // xmm6     = d0
        __ASM_EMIT("movaps      " BIQUAD_D2_SOFF "(%[f]), %%xmm7")      // xmm7     = d1

        // Process first 3 steps
        __ASM_EMIT(".align 16")
        __ASM_EMIT("1:")
        __ASM_EMIT("movss       (%[src]), %%xmm0")                      // xmm0     = *src
        __ASM_EMIT("add         $4, %[src]")                            // src      ++
        __ASM_EMIT("movss       %%xmm0, %%xmm1")                        // xmm1     = s
        __ASM_EMIT("movaps      %%xmm1, %%xmm2")                        // xmm2     = s
        __ASM_EMIT("movaps      %%xmm1, %%xmm3")                        // xmm3     = s
        __ASM_EMIT("mulps       " BIQUAD_X8_A0_SOFF "(%[f]), %%xmm1")   // xmm1     = s*a0
        __ASM_EMIT("mulps       " BIQUAD_X8_A1_SOFF "(%[f]), %%xmm2")   // xmm2     = s*a1
        __ASM_EMIT("addps       %%xmm6, %%xmm1")                        // xmm1     = s*a0+d0 = s2
        __ASM_EMIT("mulps       " BIQUAD_X8_A2_SOFF "(%[f]), %%xmm3")   // xmm3     = s*a2
        __ASM_EMIT("movaps      %%xmm1, %%xmm4")                        // xmm4     = s2
        __ASM_EMIT("movaps      %%xmm1, %%xmm5")                        // xmm5     = s2
        __ASM_EMIT("mulps       " BIQUAD_X8_B1_SOFF "(%[f]), %%xmm4")   // xmm4     = s2*b1
        __ASM_EMIT("mulps       " BIQUAD_X8_B2_SOFF "(%[f]), %%xmm5")   // xmm5     = s2*b2
        __ASM_EMIT("addps       %%xmm4, %%xmm2")                        // xmm2     = s*a1 + s2*b1 = p1
        __ASM_EMIT("addps       %%xmm5, %%xmm3")                        // xmm3     = s*a2 + s2*b2 = p2

        // Shift buffer
        __ASM_EMIT("shufps      $0x90, %%xmm1, %%xmm1")                 // xmm1     = s2[0] s2[0] s2[1] s2[2]

        // Update delay only by mask
        __ASM_EMIT("addps       %%xmm7, %%xmm2")                        // xmm2     = p1 + d1
        __ASM_EMIT("movaps      %[MASK], %%xmm0")                       // xmm0     = MASK
        __ASM_EMIT("movaps      %%xmm0, %%xmm4")                        // xmm4     = MASK
        __ASM_EMIT("movaps      %%xmm0, %%xmm5")                        // xmm5     = MASK
        __ASM_EMIT("andps       %%xmm4, %%xmm2")                        // xmm2     = (p1 + d1) & MASK
        __ASM_EMIT("andps       %%xmm5, %%xmm3")                        // xmm3     = p2 & MASK
        __ASM_EMIT("andnps      %%xmm6, %%xmm4")                        // xmm4     = d0 & ~MASK
        __ASM_EMIT("andnps      %%xmm7, %%xmm5")                        // xmm5     = d1 & ~MASK
        __ASM_EMIT("orps        %%xmm2, %%xmm4")                        // xmm4     = (p1 + d1) & MASK | (d0 & ~MASK)
        __ASM_EMIT("orps        %%xmm3, %%xmm5")                        // xmm5     = (p2 & MASK) | (d1 & ~MASK)
        __ASM_EMIT("movaps      %%xmm4, %%xmm6")                        // xmm6     = d0 & ~MASK
        __ASM_EMIT("movaps      %%xmm5, %%xmm7")                        // xmm7     = d1 & ~MASK

        // Repeat loop
        __ASM_EMITP("xchg       %[mask], %[i]")
        __ASM_EMITP("shl        $1, %[i]")                              // mask     = mask << 1
        __ASM_EMITP("or         $1, %[i]")                              // mask     = (mask << 1) | 1
        __ASM_EMITP("xchg       %[mask], %[i]")                         //
//                __ASM_EMITNP("shl       $1, %[mask]")                           // mask     = mask << 1
//                __ASM_EMITNP("or        $1, %[mask]")                           // mask     = (mask << 1) | 1
        __ASM_EMITNP("lea       0x01(,%[mask], 2), %[mask]")            // mask     = (mask << 1) | 1

        __ASM_EMIT("shufps      $0x90, %%xmm0, %%xmm0")                 // xmm0     = m[0] m[0] m[1] m[2]
        __ASM_EMIT("inc         %[i]")                                  // i++
        __ASM_EMIT("movaps      %%xmm0, %[MASK]")                       // store mask
        __ASM_EMIT("cmp         %[count], %[i]")
        __ASM_EMIT("jae         3f")                                    // jump to completion
        __ASM_EMIT("cmp         $3, %[i]")
        __ASM_EMIT("jb          1b")

        // 4x filter processing without mask
        __ASM_EMIT(".align 16")
        __ASM_EMIT("2:")
        __ASM_EMIT("movss       (%[src]), %%xmm0")                      // xmm0     = *src
        __ASM_EMIT("add         $4, %[src]")                            // src      ++
        __ASM_EMIT("movss       %%xmm0, %%xmm1")                        // xmm1     = s
        __ASM_EMIT("movaps      %%xmm1, %%xmm2")                        // xmm2     = s
        __ASM_EMIT("movaps      %%xmm1, %%xmm3")                        // xmm3     = s
        __ASM_EMIT("mulps       " BIQUAD_X8_A0_SOFF "(%[f]), %%xmm1")   // xmm1     = s*a0
        __ASM_EMIT("mulps       " BIQUAD_X8_A1_SOFF "(%[f]), %%xmm2")   // xmm2     = s*a1
        __ASM_EMIT("addps       %%xmm6, %%xmm1")                        // xmm1     = s*a0+d0 = s2
        __ASM_EMIT("mulps       " BIQUAD_X8_A2_SOFF "(%[f]), %%xmm3")   // xmm3     = s*a2
        __ASM_EMIT("movaps      %%xmm1, %%xmm4")                        // xmm4     = s2
        __ASM_EMIT("movaps      %%xmm1, %%xmm5")                        // xmm5     = s2
        __ASM_EMIT("mulps       " BIQUAD_X8_B1_SOFF "(%[f]), %%xmm4")   // xmm4     = s2*b1
        __ASM_EMIT("mulps       " BIQUAD_X8_B2_SOFF "(%[f]), %%xmm5")   // xmm5     = s2*b2
        __ASM_EMIT("addps       %%xmm4, %%xmm2")                        // xmm2     = s*a1 + s2*b1 = p1
        __ASM_EMIT("addps       %%xmm5, %%xmm3")                        // xmm3     = s*a2 + s2*b2 = p2
        __ASM_EMIT("addps       %%xmm7, %%xmm2")                		// xmm2     = p1 + d1
        __ASM_EMIT("movaps      %%xmm3, %%xmm7")                		// xmm7     = p2
        __ASM_EMIT("movaps      %%xmm2, %%xmm6")                		// xmm6     = p1 + d1

        // Shift buffer and repeat loop
        __ASM_EMIT("shufps      $0x93, %%xmm1, %%xmm1")                 // xmm1     = s2[3] s2[0] s2[1] s2[2]
        __ASM_EMIT("movss       %%xmm1, (%[dst])")                      // *dst     = s2[3]
        __ASM_EMIT("inc         %[i]")                                  // i++
        __ASM_EMIT("add         $4, %[dst]")                            // dst      ++
        __ASM_EMIT("cmp         %[count], %[i]")
        __ASM_EMIT("jb          2b")

        // Prepare last loop
        __ASM_EMIT("mov         %[mask], %[i]")                         // i        = mask
        __ASM_EMIT("movaps      %[MASK], %%xmm0")                       // xmm0     = m[0] m[1] m[2] m[3]
        __ASM_EMIT("xorps       %%xmm2, %%xmm2")                        // xmm2     = 0 0 0 0
        __ASM_EMIT("shl         $1, %[i]")                              // i        = mask << 1
        __ASM_EMIT("shufps      $0x90, %%xmm0, %%xmm0")                 // xmm0     = m[0] m[0] m[1] m[2]
        __ASM_EMIT("and         $0x0f, %[i]")                           // i        = (mask << 1) & 0x0f
        __ASM_EMIT("movss       %%xmm2, %%xmm0")                        // xmm0     = 0 m[0] m[1] m[2]

        // Process steps
        __ASM_EMIT(".align 16")
        __ASM_EMIT("3:")
        __ASM_EMIT("movaps      %%xmm1, %%xmm2")                        // xmm2     = s
        __ASM_EMIT("movaps      %%xmm1, %%xmm3")                        // xmm3     = s
        __ASM_EMIT("mulps       " BIQUAD_X8_A0_SOFF "(%[f]), %%xmm1")   // xmm1     = s*a0
        __ASM_EMIT("mulps       " BIQUAD_X8_A1_SOFF "(%[f]), %%xmm2")   // xmm2     = s*a1
        __ASM_EMIT("addps       %%xmm6, %%xmm1")                        // xmm1     = s*a0+d0 = s2
        __ASM_EMIT("mulps       " BIQUAD_X8_A2_SOFF "(%[f]), %%xmm3")   // xmm3     = s*a2
        __ASM_EMIT("movaps      %%xmm1, %%xmm4")                        // xmm4     = s2
        __ASM_EMIT("movaps      %%xmm1, %%xmm5")                        // xmm5     = s2
        __ASM_EMIT("mulps       " BIQUAD_X8_B1_SOFF "(%[f]), %%xmm4")   // xmm4     = s2*b1
        __ASM_EMIT("mulps       " BIQUAD_X8_B2_SOFF "(%[f]), %%xmm5")   // xmm5     = s2*b2
        __ASM_EMIT("addps       %%xmm4, %%xmm2")                        // xmm2     = s*a1 + s2*b1 = p1
        __ASM_EMIT("addps       %%xmm5, %%xmm3")                        // xmm3     = s*a2 + s2*b2 = p2

        // Shift buffer
        __ASM_EMIT("shufps      $0x93, %%xmm1, %%xmm1")                 // xmm1     = s2[3] s2[0] s2[1] s2[2]
        __ASM_EMIT("movss       %%xmm1, (%[dst])")                      // *dst     = s2[3]
        __ASM_EMIT("add         $4, %[dst]")                            // dst      ++

        // Update delay only by mask
        __ASM_EMIT("addps       %%xmm7, %%xmm2")                        // xmm2     = p1 + d1
        __ASM_EMIT("movaps      %%xmm0, %%xmm4")                        // xmm4     = MASK
        __ASM_EMIT("movaps      %%xmm0, %%xmm5")                        // xmm5     = MASK
        __ASM_EMIT("andps       %%xmm4, %%xmm2")                        // xmm2     = (p1 + d1) & MASK
        __ASM_EMIT("andps       %%xmm5, %%xmm3")                        // xmm3     = p2 & MASK
        __ASM_EMIT("andnps      %%xmm6, %%xmm4")                        // xmm4     = d0 & ~MASK
        __ASM_EMIT("andnps      %%xmm7, %%xmm5")                        // xmm5     = d1 & ~MASK
        __ASM_EMIT("orps        %%xmm2, %%xmm4")                        // xmm4     = (p1 + d1) & MASK | (d0 & ~MASK)
        __ASM_EMIT("orps        %%xmm3, %%xmm5")                        // xmm5     = (p2 & MASK) | (d1 & ~MASK)
        __ASM_EMIT("movaps      %%xmm4, %%xmm6")                        // xmm6     = d0 & ~MASK
        __ASM_EMIT("movaps      %%xmm5, %%xmm7")                        // xmm7     = d1 & ~MASK

        // Repeat loop
        __ASM_EMIT("xorps       %%xmm2, %%xmm2")                        // xmm2     = 0 0 0 0
        __ASM_EMIT("shl         $1, %[i]")                              // i        = mask << 1
        __ASM_EMIT("shufps      $0x90, %%xmm0, %%xmm0")                 // xmm0     = m[0] m[0] m[1] m[2]
        __ASM_EMIT("and         $0x0f, %[i]")                           // i        = (mask << 1) & 0x0f
        __ASM_EMIT("movss       %%xmm2, %%xmm0")                        // xmm0     = 0 m[0] m[1] m[2]
        __ASM_EMIT("jnz         3b")                                    // check that mask is not zero

        // Store delay buffer
        __ASM_EMIT("movaps      %%xmm6, " BIQUAD_D0_SOFF "(%[f])")      // xmm6     = d0
        __ASM_EMIT("movaps      %%xmm7, " BIQUAD_D2_SOFF "(%[f])")      // xmm7     = d1

        //---------------------------------------------------------------------
        // Cycle 2
        __ASM_EMIT("mov         %[dst_bk], %[src]")
        __ASM_EMIT("mov         %[dst_bk], %[dst]")

        // Initialize mask
        // xmm0=tmp, xmm1={s,s2[4]}, xmm2=p1[4], xmm3=p2[4], xmm6=d0[4], xmm7=d1[4]
        __ASM_EMIT32("movl      $1, %[mask]")
        __ASM_EMIT64("movq      $1, %[mask]")
        __ASM_EMIT("xor         %[i], %[i]")
        __ASM_EMIT("movaps      %[X_MASK], %%xmm0")
        __ASM_EMIT("movaps      %%xmm0, %[MASK]")
        __ASM_EMIT("xorps       %%xmm1, %%xmm1")

        // Load delay buffer
        __ASM_EMIT("movaps      " BIQUAD_D1_SOFF "(%[f]), %%xmm6")      // xmm6     = d0
        __ASM_EMIT("movaps      " BIQUAD_D3_SOFF "(%[f]), %%xmm7")      // xmm7     = d1

        // Process first 3 steps
        __ASM_EMIT(".align 16")
        __ASM_EMIT("1:")
        __ASM_EMIT("movss       (%[src]), %%xmm0")                      // xmm0     = *src
        __ASM_EMIT("add         $4, %[src]")                            // src      ++
        __ASM_EMIT("movss       %%xmm0, %%xmm1")                        // xmm1     = s
        __ASM_EMIT("movaps      %%xmm1, %%xmm2")                        // xmm2     = s
        __ASM_EMIT("movaps      %%xmm1, %%xmm3")                        // xmm3     = s
        __ASM_EMIT("mulps       " BIQUAD_X8_I0_SOFF "(%[f]), %%xmm1")   // xmm1     = s*a0
        __ASM_EMIT("mulps       " BIQUAD_X8_I1_SOFF "(%[f]), %%xmm2")   // xmm2     = s*a1
        __ASM_EMIT("addps       %%xmm6, %%xmm1")                        // xmm1     = s*a0+d0 = s2
        __ASM_EMIT("mulps       " BIQUAD_X8_I2_SOFF "(%[f]), %%xmm3")   // xmm3     = s*a2
        __ASM_EMIT("movaps      %%xmm1, %%xmm4")                        // xmm4     = s2
        __ASM_EMIT("movaps      %%xmm1, %%xmm5")                        // xmm5     = s2
        __ASM_EMIT("mulps       " BIQUAD_X8_J1_SOFF "(%[f]), %%xmm4")   // xmm4     = s2*b1
        __ASM_EMIT("mulps       " BIQUAD_X8_J2_SOFF "(%[f]), %%xmm5")   // xmm5     = s2*b2
        __ASM_EMIT("addps       %%xmm4, %%xmm2")                        // xmm2     = s*a1 + s2*b1 = p1
        __ASM_EMIT("addps       %%xmm5, %%xmm3")                        // xmm3     = s*a2 + s2*b2 = p2

        // Shift buffer
        __ASM_EMIT("shufps      $0x90, %%xmm1, %%xmm1")                 // xmm1     = s2[0] s2[0] s2[1] s2[2]

        // Update delay only by mask
        __ASM_EMIT("addps       %%xmm7, %%xmm2")                        // xmm2     = p1 + d1
        __ASM_EMIT("movaps      %[MASK], %%xmm0")                       // xmm0     = MASK
        __ASM_EMIT("movaps      %%xmm0, %%xmm4")                        // xmm4     = MASK
        __ASM_EMIT("movaps      %%xmm0, %%xmm5")                        // xmm5     = MASK
        __ASM_EMIT("andps       %%xmm4, %%xmm2")                        // xmm2     = (p1 + d1) & MASK
        __ASM_EMIT("andps       %%xmm5, %%xmm3")                        // xmm3     = p2 & MASK
        __ASM_EMIT("andnps      %%xmm6, %%xmm4")                        // xmm4     = d0 & ~MASK
        __ASM_EMIT("andnps      %%xmm7, %%xmm5")                        // xmm5     = d1 & ~MASK
        __ASM_EMIT("orps        %%xmm2, %%xmm4")                        // xmm4     = (p1 + d1) & MASK | (d0 & ~MASK)
        __ASM_EMIT("orps        %%xmm3, %%xmm5")                        // xmm5     = (p2 & MASK) | (d1 & ~MASK)
        __ASM_EMIT("movaps      %%xmm4, %%xmm6")                        // xmm6     = d0 & ~MASK
        __ASM_EMIT("movaps      %%xmm5, %%xmm7")                        // xmm7     = d1 & ~MASK

        // Repeat loop
        __ASM_EMITP("xchg       %[mask], %[i]")
        __ASM_EMITP("shl        $1, %[i]")                              // mask     = mask << 1
        __ASM_EMITP("or         $1, %[i]")                              // mask     = (mask << 1) | 1
        __ASM_EMITP("xchg       %[mask], %[i]")                         //
//                __ASM_EMITNP("shl       $1, %[mask]")                           // mask     = mask << 1
//                __ASM_EMITNP("or        $1, %[mask]")                           // mask     = (mask << 1) | 1
        __ASM_EMITNP("lea       0x01(,%[mask], 2), %[mask]")            // mask     = (mask << 1) | 1

        __ASM_EMIT("shufps      $0x90, %%xmm0, %%xmm0")                 // xmm0     = m[0] m[0] m[1] m[2]
        __ASM_EMIT("inc         %[i]")                                  // i++
        __ASM_EMIT("movaps      %%xmm0, %[MASK]")                       // store mask
        __ASM_EMIT("cmp         %[count], %[i]")
        __ASM_EMIT("jae         3f")                                    // jump to completion
        __ASM_EMIT("cmp         $3, %[i]")
        __ASM_EMIT("jb          1b")

        // 4x filter processing without mask
        __ASM_EMIT(".align 16")
        __ASM_EMIT("2:")
        __ASM_EMIT("movss       (%[src]), %%xmm0")                      // xmm0     = *src
        __ASM_EMIT("add         $4, %[src]")                            // src      ++
        __ASM_EMIT("movss       %%xmm0, %%xmm1")                        // xmm1     = s
        __ASM_EMIT("movaps      %%xmm1, %%xmm2")                        // xmm2     = s
        __ASM_EMIT("movaps      %%xmm1, %%xmm3")                        // xmm3     = s
        __ASM_EMIT("mulps       " BIQUAD_X8_I0_SOFF "(%[f]), %%xmm1")   // xmm1     = s*a0
        __ASM_EMIT("mulps       " BIQUAD_X8_I1_SOFF "(%[f]), %%xmm2")   // xmm2     = s*a1
        __ASM_EMIT("addps       %%xmm6, %%xmm1")                        // xmm1     = s*a0+d0 = s2
        __ASM_EMIT("mulps       " BIQUAD_X8_I2_SOFF "(%[f]), %%xmm3")   // xmm3     = s*a2
        __ASM_EMIT("movaps      %%xmm1, %%xmm4")                        // xmm4     = s2
        __ASM_EMIT("movaps      %%xmm1, %%xmm5")                        // xmm5     = s2
        __ASM_EMIT("mulps       " BIQUAD_X8_J1_SOFF "(%[f]), %%xmm4")   // xmm4     = s2*b1
        __ASM_EMIT("mulps       " BIQUAD_X8_J2_SOFF "(%[f]), %%xmm5")   // xmm5     = s2*b2
        __ASM_EMIT("addps       %%xmm4, %%xmm2")                        // xmm2     = s*a1 + s2*b1 = p1
        __ASM_EMIT("addps       %%xmm5, %%xmm3")                        // xmm3     = s*a2 + s2*b2 = p2
        __ASM_EMIT("addps       %%xmm7, %%xmm2")                        // xmm2     = p1 + d1
        __ASM_EMIT("movaps      %%xmm3, %%xmm7")                        // xmm7     = p2
        __ASM_EMIT("movaps      %%xmm2, %%xmm6")                        // xmm6     = p1 + d1

        // Shift buffer and repeat loop
        __ASM_EMIT("shufps      $0x93, %%xmm1, %%xmm1")                 // xmm1     = s2[3] s2[0] s2[1] s2[2]
        __ASM_EMIT("movss       %%xmm1, (%[dst])")                      // *dst     = s2[3]
        __ASM_EMIT("inc         %[i]")                                  // i++
        __ASM_EMIT("add         $4, %[dst]")                            // dst      ++
        __ASM_EMIT("cmp         %[count], %[i]")
        __ASM_EMIT("jb          2b")

        // Prepare last loop
        __ASM_EMIT("mov         %[mask], %[i]")                         // i        = mask
        __ASM_EMIT("movaps      %[MASK], %%xmm0")                       // xmm0     = m[0] m[1] m[2] m[3]
        __ASM_EMIT("xorps       %%xmm2, %%xmm2")                        // xmm2     = 0 0 0 0
        __ASM_EMIT("shl         $1, %[i]")                              // i        = mask << 1
        __ASM_EMIT("shufps      $0x90, %%xmm0, %%xmm0")                 // xmm0     = m[0] m[0] m[1] m[2]
        __ASM_EMIT("and         $0x0f, %[i]")                           // i        = (mask << 1) & 0x0f
        __ASM_EMIT("movss       %%xmm2, %%xmm0")                        // xmm0     = 0 m[0] m[1] m[2]

        // Process steps
        __ASM_EMIT(".align 16")
        __ASM_EMIT("3:")
        __ASM_EMIT("movaps      %%xmm1, %%xmm2")                        // xmm2     = s
        __ASM_EMIT("movaps      %%xmm1, %%xmm3")                        // xmm3     = s
        __ASM_EMIT("mulps       " BIQUAD_X8_I0_SOFF "(%[f]), %%xmm1")   // xmm1     = s*a0
        __ASM_EMIT("mulps       " BIQUAD_X8_I1_SOFF "(%[f]), %%xmm2")   // xmm2     = s*a1
        __ASM_EMIT("addps       %%xmm6, %%xmm1")                        // xmm1     = s*a0+d0 = s2
        __ASM_EMIT("mulps       " BIQUAD_X8_I2_SOFF "(%[f]), %%xmm3")   // xmm3     = s*a2
        __ASM_EMIT("movaps      %%xmm1, %%xmm4")                        // xmm4     = s2
        __ASM_EMIT("movaps      %%xmm1, %%xmm5")                        // xmm5     = s2
        __ASM_EMIT("mulps       " BIQUAD_X8_J1_SOFF "(%[f]), %%xmm4")   // xmm4     = s2*b1
        __ASM_EMIT("mulps       " BIQUAD_X8_J2_SOFF "(%[f]), %%xmm5")   // xmm5     = s2*b2
        __ASM_EMIT("addps       %%xmm4, %%xmm2")                        // xmm2     = s*a1 + s2*b1 = p1
        __ASM_EMIT("addps       %%xmm5, %%xmm3")                        // xmm3     = s*a2 + s2*b2 = p2

        // Shift buffer
        __ASM_EMIT("shufps      $0x93, %%xmm1, %%xmm1")                 // xmm1     = s2[3] s2[0] s2[1] s2[2]
        __ASM_EMIT("movss       %%xmm1, (%[dst])")                      // *dst     = s2[3]
        __ASM_EMIT("add         $4, %[dst]")                            // dst      ++

        // Update delay only by mask
        __ASM_EMIT("addps       %%xmm7, %%xmm2")                        // xmm2     = p1 + d1
        __ASM_EMIT("movaps      %%xmm0, %%xmm4")                        // xmm4     = MASK
        __ASM_EMIT("movaps      %%xmm0, %%xmm5")                        // xmm5     = MASK
        __ASM_EMIT("andps       %%xmm4, %%xmm2")                        // xmm2     = (p1 + d1) & MASK
        __ASM_EMIT("andps       %%xmm5, %%xmm3")                        // xmm3     = p2 & MASK
        __ASM_EMIT("andnps      %%xmm6, %%xmm4")                        // xmm4     = d0 & ~MASK
        __ASM_EMIT("andnps      %%xmm7, %%xmm5")                        // xmm5     = d1 & ~MASK
        __ASM_EMIT("orps        %%xmm2, %%xmm4")                        // xmm4     = (p1 + d1) & MASK | (d0 & ~MASK)
        __ASM_EMIT("orps        %%xmm3, %%xmm5")                        // xmm5     = (p2 & MASK) | (d1 & ~MASK)
        __ASM_EMIT("movaps      %%xmm4, %%xmm6")                        // xmm6     = (p1 + d1) & MASK | (d0 & ~MASK)
        __ASM_EMIT("movaps      %%xmm5, %%xmm7")                        // xmm7     = (p2 & MASK) | (d1 & ~MASK)

        // Repeat loop
        __ASM_EMIT("xorps       %%xmm2, %%xmm2")                        // xmm2     = 0 0 0 0
        __ASM_EMIT("shl         $1, %[i]")                              // i        = mask << 1
        __ASM_EMIT("shufps      $0x90, %%xmm0, %%xmm0")                 // xmm0     = m[0] m[0] m[1] m[2]
        __ASM_EMIT("and         $0x0f, %[i]")                           // i        = (mask << 1) & 0x0f
        __ASM_EMIT("movss       %%xmm2, %%xmm0")                        // xmm0     = 0 m[0] m[1] m[2]
        __ASM_EMIT("jnz         3b")                                    // check that mask is not zero

        // Store delay buffer
        __ASM_EMIT("movaps      %%xmm6, " BIQUAD_D1_SOFF "(%[f])")      // xmm6     = d0
        __ASM_EMIT("movaps      %%xmm7, " BIQUAD_D3_SOFF "(%[f])")      // xmm7     = d1

        // Exit label
        __ASM_EMIT("8:")

        : [dst] "+r" (dst), [src] "+r" (src), [mask] __ASM_ARG_TMP(mask), [i] "=&r"(i)
        :
          [count] "r" (count), [f] "r" (f),
          [dst_bk] "m" (dst_bk),
          [X_MASK] "m" (X_MASK0001),
          [MASK] "m" (MASK)
        : "cc", "memory",
          "%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5", "%xmm6", "%xmm7"
    );
}

#endif /* DSP_ARCH_X86_SSE_FILTERS_STATIC_H_ */
