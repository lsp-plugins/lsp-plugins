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
    #ifdef ARCH_X86_64
    void x64_biquad_process_x2(float *dst, const float *src, size_t count, biquad_t *f)
    {
        size_t i;

        ARCH_X86_ASM
        (
            // Check count
            __ASM_EMIT("test        %[count], %[count]")
            __ASM_EMIT("jz          1f")

            // Load permanent data
            // xmm8 = A, xmm9 = I, xmm10 = B, xmm11 = J
            __ASM_EMIT("movaps      " BIQUAD_D0_SOFF "(%[f]), %%xmm4")      // xmm4 = d0 d1 0  0
            __ASM_EMIT("movaps      " BIQUAD_D1_SOFF "(%[f]), %%xmm5")      // xmm5 = e0 e1 0  0
            __ASM_EMIT("xor         %[i], %[i]")
            __ASM_EMIT("xorps       %%xmm6, %%xmm6")                        // xmm6 = 0  0  0  0
            __ASM_EMIT("xorps       %%xmm7, %%xmm7")                        // xmm7 = 0  0  0  0
            __ASM_EMIT("movaps      " BIQUAD_X2_A_SOFF "(%[f]), %%xmm8")    // xmm8 = A
            __ASM_EMIT("movaps      " BIQUAD_X2_I_SOFF "(%[f]), %%xmm9")    // xmm9 = I
            __ASM_EMIT("movaps      " BIQUAD_X2_B_SOFF "(%[f]), %%xmm10")   // xmm10 = A
            __ASM_EMIT("movaps      " BIQUAD_X2_J_SOFF "(%[f]), %%xmm11")   // xmm11 = A

            // Process first filter only
            __ASM_EMIT("movss       (%[src], %[i], 4), %%xmm1")             // xmm1 = s ? ? ?
            __ASM_EMIT("shufps      $0xd0, %%xmm4, %%xmm4")                 // xmm4 = d0 d0 d1 0
            __ASM_EMIT("shufps      $0x00, %%xmm1, %%xmm1")                 // xmm1 = s s s s
            __ASM_EMIT("mulps       %%xmm8, %%xmm1")                        // xmm1 = s*a0 s*a0 s*a1 s*a2
            __ASM_EMIT("addps       %%xmm4, %%xmm1")                        // xmm1 = s*a0+d0 s*a0+d0 s*a1+d1 s*a2
            __ASM_EMIT("movaps      %%xmm1, %%xmm4")                        // xmm4 = s*a0+d0 s*a0+d0 s*a1+d1 s*a2
            __ASM_EMIT("movhlps     %%xmm1, %%xmm6")                        // xmm6 = s*a1+d1 s*a2 0 0
            __ASM_EMIT("mulps       %%xmm10, %%xmm4")                       // xmm4 = (s*a0+d0)*b1 (s*a0+d0)*b2 0 0
            __ASM_EMIT("add         $1, %[i]")
            __ASM_EMIT("cmp         %[count], %[i]")
            __ASM_EMIT("addps       %%xmm6, %%xmm4")                        // xmm4 = (s*a0+d0)*b1+s*a1+d1 (s*a0+d0)*b2+s*a2 0 0
            __ASM_EMIT("jae         3f")

            // Process two filters simultaneously
            __ASM_EMIT("2:")
            __ASM_EMIT("movss       (%[src], %[i], 4), %%xmm0")             // xmm0 = s ? ? ?
            __ASM_EMIT("shufps      $0xd0, %%xmm5, %%xmm5")                 // xmm5 = e0 e0 e1 0
            __ASM_EMIT("shufps      $0xd0, %%xmm4, %%xmm4")                 // xmm4 = d0 d0 d1 0
            __ASM_EMIT("shufps      $0x00, %%xmm1, %%xmm1")                 // xmm1 = r r r r
            __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0")                 // xmm0 = s s s s
            __ASM_EMIT("mulps       %%xmm9, %%xmm1")                        // xmm1 = r*i0 r*i0 r*i1 r*i2
            __ASM_EMIT("mulps       %%xmm8, %%xmm0")                        // xmm0 = s*a0 s*a0 s*a1 s*a2
            __ASM_EMIT("addps       %%xmm5, %%xmm1")                        // xmm1 = r*i0_e0 r*i0+e0 r*i1+e1 r*i2
            __ASM_EMIT("addps       %%xmm4, %%xmm0")                        // xmm0 = s*a0+d0 s*a0+d0 s*a1+d1 s*a2
            __ASM_EMIT("movaps      %%xmm1, %%xmm5")                        // xmm5 = r*i0+e0 r*i0+e0 r*i1+e1 r*i2
            __ASM_EMIT("movaps      %%xmm0, %%xmm4")                        // xmm4 = s*a0+d0 s*a0+d0 s*a1+d1 s*a2
            __ASM_EMIT("movhlps     %%xmm1, %%xmm7")                        // xmm7 = r*i1+e1 r*i2 0 0
            __ASM_EMIT("movhlps     %%xmm0, %%xmm6")                        // xmm6 = s*a1+d1 s*a2 0 0
            __ASM_EMIT("movss       %%xmm1, -4(%[dst], %[i], 4)")           // store value
            __ASM_EMIT("mulps       %%xmm11, %%xmm5")                       // xmm5 = (r*i0+e0)*j1 (r*i0+e0)*j2 0 0
            __ASM_EMIT("mulps       %%xmm10, %%xmm4")                       // xmm4 = (s*a0+d0)*b1 (s*a0+d0)*b2 0 0
            __ASM_EMIT("inc         %[i]")
            __ASM_EMIT("movaps      %%xmm0, %%xmm1")                        // xmm1 = r ? ? ?
            __ASM_EMIT("cmp         %[count], %[i]")
            __ASM_EMIT("addps       %%xmm7, %%xmm5")                        // xmm5 = (r*i0+e0)*j1+r*i1+e1 (r*i0+e0)*j2+r*i2 0 0
            __ASM_EMIT("addps       %%xmm6, %%xmm4")                        // xmm4 = (s*a0+d0)*b1+s*a1+d1 (s*a0+d0)*b2+s*a2 0 0

            // Update pointers and repeat loop
            __ASM_EMIT("jb          2b")

            // Process second filter only
            __ASM_EMIT("3:")
            __ASM_EMIT("shufps      $0xd0, %%xmm5, %%xmm5")                 // xmm5 = e0 e0 e1 0
            __ASM_EMIT("shufps      $0x00, %%xmm1, %%xmm1")                 // xmm1 = r r r r
            __ASM_EMIT("mulps       %%xmm9, %%xmm1")                        // xmm1 = r*i0 r*i0 r*i1 r*i2
            __ASM_EMIT("addps       %%xmm5, %%xmm1")                        // xmm1 = r*i0_e0 r*i0+e0 r*i1+e1 r*i2
            __ASM_EMIT("movaps      %%xmm1, %%xmm5")                        // xmm5 = r*i0+e0 r*i0+e0 r*i1+e1 r*i2
            __ASM_EMIT("movhlps     %%xmm1, %%xmm7")                        // xmm7 = r*i1+e1 r*i2 0 0
            __ASM_EMIT("movss       %%xmm1, -4(%[dst], %[i], 4)")           // store value
            __ASM_EMIT("mulps       %%xmm11, %%xmm5")                       // xmm5 = (r*i0+e0)*j1 (r*i0+e0)*j2 0 0
            __ASM_EMIT("addps       %%xmm7, %%xmm5")                        // xmm5 = (r*i0+e0)*j1+r*i1+e1 (r*i0+e0)*j2+r*i2 0 0

            // Store the updated buffer state
            __ASM_EMIT("movaps      %%xmm4, " BIQUAD_D0_SOFF "(%[f])")
            __ASM_EMIT("movaps      %%xmm5, " BIQUAD_D1_SOFF "(%[f])")

            // Exit label
            __ASM_EMIT("1:")

            : [i] "=&r"(i)
            : [dst] "r" (dst), [src] "r" (src), [count] "r" (count), [f] "r" (f)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5", "%xmm6", "%xmm7",
              "%xmm8", "%xmm9", "%xmm10", "%xmm11"
        );
    }

    #if 0
    static void x64_biquad_process_x4(float *dst, const float *src, size_t count, biquad_t *f)
    {
        size_t i, mask;

        ARCH_X86_ASM
        (
            // Check count
            __ASM_EMIT("test        %[count], %[count]")
            __ASM_EMIT("jz          4f")

            // Initialize mask
            // xmm0=tmp, xmm1={s,s2[4]}, xmm2=p1[4], xmm3=p2[4], xmm6=d0[4], xmm7=d1[4]
            // xmm8=a0, xmm9=a1, xmm10=a2, xmm11=b1, xmm12=b2
            // xmm15=mask
            __ASM_EMIT("mov         $1, %[mask]")
            __ASM_EMIT("xor         %[i], %[i]")
            __ASM_EMIT("movaps      %[X_MASK], %%xmm15")                    // xmm15    = mask
            __ASM_EMIT("xorps       %%xmm1, %%xmm1")

            // Load delay buffer
            __ASM_EMIT("movaps      " BIQUAD_D0_SOFF "(%[f]), %%xmm6")      // xmm6     = d0
            __ASM_EMIT("movaps      " BIQUAD_D1_SOFF "(%[f]), %%xmm7")      // xmm7     = d1
            __ASM_EMIT("movaps      " BIQUAD_X4_A0_SOFF "(%[f]), %%xmm8")   // xmm8     = A0
            __ASM_EMIT("movaps      " BIQUAD_X4_A1_SOFF "(%[f]), %%xmm9")   // xmm9     = A1
            __ASM_EMIT("movaps      " BIQUAD_X4_A2_SOFF "(%[f]), %%xmm10")  // xmm10    = A2
            __ASM_EMIT("movaps      " BIQUAD_X4_B1_SOFF "(%[f]), %%xmm11")  // xmm11    = B1
            __ASM_EMIT("movaps      " BIQUAD_X4_B2_SOFF "(%[f]), %%xmm12")  // xmm12    = B2

            // Process first 3 steps
            __ASM_EMIT("1:")
            __ASM_EMIT("movss       (%[src]), %%xmm0")                      // xmm0     = *src
            __ASM_EMIT("add         $4, %[src]")                            // src      ++
            __ASM_EMIT("movss       %%xmm0, %%xmm1")                        // xmm1     = s
            __ASM_EMIT("movaps      %%xmm1, %%xmm2")                        // xmm2     = s
            __ASM_EMIT("movaps      %%xmm1, %%xmm3")                        // xmm3     = s
            __ASM_EMIT("mulps       %%xmm8, %%xmm1")                        // xmm1     = s*a0
            __ASM_EMIT("mulps       %%xmm9, %%xmm2")                        // xmm2     = s*a1
            __ASM_EMIT("addps       %%xmm6, %%xmm1")                        // xmm1     = s*a0+d0 = s2
            __ASM_EMIT("mulps       %%xmm10, %%xmm3")                       // xmm3     = s*a2
            __ASM_EMIT("movaps      %%xmm1, %%xmm4")                        // xmm4     = s2
            __ASM_EMIT("movaps      %%xmm1, %%xmm5")                        // xmm5     = s2
            __ASM_EMIT("mulps       %%xmm11, %%xmm4")                       // xmm4     = s2*b1
            __ASM_EMIT("mulps       %%xmm12, %%xmm5")                       // xmm5     = s2*b2
            __ASM_EMIT("addps       %%xmm4, %%xmm2")                        // xmm2     = s*a1 + s2*b1 = p1
            __ASM_EMIT("addps       %%xmm5, %%xmm3")                        // xmm3     = s*a2 + s2*b2 = p2

            // Shift buffer
            __ASM_EMIT("shufps      $0x90, %%xmm1, %%xmm1")                 // xmm1     = s2[0] s2[0] s2[1] s2[2]

            // Update delay only by mask
            __ASM_EMIT("addps       %%xmm7, %%xmm2")                        // xmm2     = p1 + d1
            __ASM_EMIT("movaps      %%xmm15, %%xmm4")                       // xmm4     = MASK
            __ASM_EMIT("movaps      %%xmm15, %%xmm5")                       // xmm5     = MASK
            __ASM_EMIT("andps       %%xmm0, %%xmm2")                        // xmm2     = (p1 + d1) & MASK
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
            __ASM_EMITNP("shl       $1, %[mask]")                           // mask     = mask << 1
            __ASM_EMITNP("or        $1, %[mask]")                           // mask     = (mask << 1) | 1

            __ASM_EMIT("shufps      $0x90, %%xmm15, %%xmm15")               // xmm15     = m[0] m[0] m[1] m[2]
            __ASM_EMIT("add         $1, %[i]")                              // i++
            __ASM_EMIT("cmp         %[count], %[i]")
            __ASM_EMIT("jae         3f")                                    // jump to completion
            __ASM_EMIT("cmp         $3, %[i]")
            __ASM_EMIT("jb          1b")

            // 4x filter processing without mask
            __ASM_EMIT("2:")
            __ASM_EMIT("movss       (%[src]), %%xmm0")                      // xmm0     = *src
            __ASM_EMIT("add         $4, %[src]")                            // src      ++
            __ASM_EMIT("movss       %%xmm0, %%xmm1")                        // xmm1     = s
            __ASM_EMIT("movaps      %%xmm1, %%xmm2")                        // xmm2     = s
            __ASM_EMIT("movaps      %%xmm1, %%xmm3")                        // xmm3     = s
            __ASM_EMIT("mulps       %%xmm8, %%xmm1")                        // xmm1     = s*a0
            __ASM_EMIT("mulps       %%xmm9, %%xmm2")                        // xmm2     = s*a1
            __ASM_EMIT("addps       %%xmm6, %%xmm1")                        // xmm1     = s*a0+d0 = s2
            __ASM_EMIT("mulps       %%xmm10, %%xmm3")                       // xmm3     = s*a2
            __ASM_EMIT("movaps      %%xmm1, %%xmm4")                        // xmm4     = s2
            __ASM_EMIT("movaps      %%xmm1, %%xmm5")                        // xmm5     = s2
            __ASM_EMIT("mulps       %%xmm11, %%xmm4")                       // xmm4     = s2*b1
            __ASM_EMIT("mulps       %%xmm12, %%xmm5")                       // xmm5     = s2*b2
            __ASM_EMIT("addps       %%xmm4, %%xmm2")                        // xmm2     = s*a1 + s2*b1 = p1
            __ASM_EMIT("addps       %%xmm5, %%xmm3")                        // xmm3     = s*a2 + s2*b2 = p2
            __ASM_EMIT("addps       %%xmm7, %%xmm2")                		// xmm2     = p1 + d1
            __ASM_EMIT("movaps      %%xmm3, %%xmm7")                		// xmm7     = p2
            __ASM_EMIT("movaps      %%xmm2, %%xmm6")                		// xmm6     = p1 + d1

            // Shift buffer
            __ASM_EMIT("shufps      $0x93, %%xmm1, %%xmm1")                 // xmm1     = s2[3] s2[0] s2[1] s2[2]
            __ASM_EMIT("movss       %%xmm1, (%[dst])")                      // *dst     = s2[3]
            __ASM_EMIT("add         $4, %[dst]")                            // dst      ++

            // Repeat loop
            __ASM_EMIT("add         $1, %[i]")                              // i++
            __ASM_EMIT("cmp         %[count], %[i]")
            __ASM_EMIT("jb          2b")

            // Prepare last loop
            __ASM_EMIT("mov         %[mask], %[i]")                         // i        = mask
            __ASM_EMIT("xorps       %%xmm2, %%xmm2")                        // xmm2     = 0 0 0 0
            __ASM_EMIT("shl         $1, %[i]")                              // i        = mask << 1
            __ASM_EMIT("shufps      $0x90, %%xmm15, %%xmm15")               // xmm15    = m[0] m[0] m[1] m[2]
            __ASM_EMIT("and         $0x0f, %[i]")                           // i        = (mask << 1) & 0x0f
            __ASM_EMIT("movss       %%xmm2, %%xmm15")                       // xmm15    = 0 m[0] m[1] m[2]

            // Process steps
            __ASM_EMIT("3:")
            __ASM_EMIT("movaps      %%xmm1, %%xmm2")                        // xmm2     = s
            __ASM_EMIT("movaps      %%xmm1, %%xmm3")                        // xmm3     = s
            __ASM_EMIT("mulps       %%xmm8, %%xmm1")                        // xmm1     = s*a0
            __ASM_EMIT("mulps       %%xmm9, %%xmm2")                        // xmm2     = s*a1
            __ASM_EMIT("addps       %%xmm6, %%xmm1")                        // xmm1     = s*a0+d0 = s2
            __ASM_EMIT("mulps       %%xmm10, %%xmm3")                       // xmm3     = s*a2
            __ASM_EMIT("movaps      %%xmm1, %%xmm4")                        // xmm4     = s2
            __ASM_EMIT("movaps      %%xmm1, %%xmm5")                        // xmm5     = s2
            __ASM_EMIT("mulps       %%xmm11, %%xmm4")                       // xmm4     = s2*b1
            __ASM_EMIT("mulps       %%xmm12, %%xmm5")                       // xmm5     = s2*b2
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
            __ASM_EMIT("shufps      $0x90, %%xmm15, %%xmm15")               // xmm15    = m[0] m[0] m[1] m[2]
            __ASM_EMIT("and         $0x0f, %[i]")                           // i        = (mask << 1) & 0x0f
            __ASM_EMIT("movss       %%xmm2, %%xmm15")                       // xmm15    = 0 m[0] m[1] m[2]
            __ASM_EMIT("jnz         3b")                                    // check that mask is not zero

            // Store delay buffer
            __ASM_EMIT("movaps      %%xmm6, " BIQUAD_D0_SOFF "(%[f])")      // xmm6     = d0
            __ASM_EMIT("movaps      %%xmm7, " BIQUAD_D1_SOFF "(%[f])")      // xmm7     = d1

            // Exit label
            __ASM_EMIT("4:")

            : [dst] "+r" (dst), [src] "+r" (src), [mask] __ASM_ARG_TMP(mask), [i] "=&r"(i)
            :
              [count] "r" (count), [f] "r" (f),
              [X_MASK] "m" (X_MASK0001)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5", "%xmm6", "%xmm7",
              "%xmm8", "%xmm9", "%xmm10", "%xmm11", "%xmm12", "%xmm15"
        );
    }
    #endif /* 0 */

    void x64_biquad_process_x8(float *dst, const float *src, size_t count, biquad_t *f)
    {
        float MASK[8] __lsp_aligned16;
        size_t mask;

        ARCH_X86_ASM
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
            __ASM_EMIT("movaps      " BIQUAD_D0_SOFF "(%[f]), %%xmm6")      // xmm6     = d0
            __ASM_EMIT("movaps      " BIQUAD_D1_SOFF "(%[f]), %%xmm14")     // xmm14    = d1
            __ASM_EMIT("movaps      " BIQUAD_D2_SOFF "(%[f]), %%xmm7")      // xmm7     = d2
            __ASM_EMIT("movaps      " BIQUAD_D3_SOFF "(%[f]), %%xmm15")     // xmm7     = d3

            //-------------------------------------------------------------
            // Process first steps
            __ASM_EMIT(".align 16")
            __ASM_EMIT("1:")
            __ASM_EMIT("movss       (%[src]), %%xmm0")                      // xmm0     = *src
            __ASM_EMIT("movss       %%xmm0, %%xmm1")                        // xmm1     = s
            __ASM_EMIT("add         $4, %[src]")                            // src      ++
            __ASM_EMIT("movaps      %%xmm1, %%xmm2")                        // xmm2     = s
            __ASM_EMIT("movaps      %%xmm9, %%xmm10")                       // xmm10    = r
            __ASM_EMIT("movaps      %%xmm1, %%xmm3")                        // xmm3     = s
            __ASM_EMIT("movaps      %%xmm9, %%xmm11")                       // xmm11    = r
            __ASM_EMIT("mulps       " BIQUAD_X8_A0_SOFF "(%[f]), %%xmm1")   // xmm1     = s*a0
            __ASM_EMIT("mulps       " BIQUAD_X8_I0_SOFF "(%[f]), %%xmm9")   // xmm9     = r*i0
            __ASM_EMIT("mulps       " BIQUAD_X8_A1_SOFF "(%[f]), %%xmm2")   // xmm2     = s*a1
            __ASM_EMIT("mulps       " BIQUAD_X8_I1_SOFF "(%[f]), %%xmm10")  // xmm10    = r*i1
            __ASM_EMIT("addps       %%xmm6, %%xmm1")                        // xmm1     = s*a0+d0 = s2
            __ASM_EMIT("addps       %%xmm14, %%xmm9")                       // xmm9     = r*i0+d1 = r2
            __ASM_EMIT("mulps       " BIQUAD_X8_A2_SOFF "(%[f]), %%xmm3")   // xmm3     = s*a2
            __ASM_EMIT("mulps       " BIQUAD_X8_I2_SOFF "(%[f]), %%xmm11")  // xmm11    = r*i2
            __ASM_EMIT("movaps      %%xmm1, %%xmm4")                        // xmm4     = s2
            __ASM_EMIT("movaps      %%xmm9, %%xmm12")                       // xmm12    = r2
            __ASM_EMIT("movaps      %%xmm1, %%xmm5")                        // xmm5     = s2
            __ASM_EMIT("movaps      %%xmm9, %%xmm13")                       // xmm13    = r2
            __ASM_EMIT("mulps       " BIQUAD_X8_B1_SOFF "(%[f]), %%xmm4")   // xmm4     = s2*b1
            __ASM_EMIT("mulps       " BIQUAD_X8_J1_SOFF "(%[f]), %%xmm12")  // xmm12    = r2*j1
            __ASM_EMIT("mulps       " BIQUAD_X8_B2_SOFF "(%[f]), %%xmm5")   // xmm5     = s2*b2
            __ASM_EMIT("mulps       " BIQUAD_X8_J2_SOFF "(%[f]), %%xmm13")  // xmm13    = r2*j2
            __ASM_EMIT("addps       %%xmm4, %%xmm2")                        // xmm2     = s*a1 + s2*b1 = p1
            __ASM_EMIT("addps       %%xmm12, %%xmm10")                      // xmm10    = r*i1 + r2*j1 = q1
            __ASM_EMIT("addps       %%xmm5, %%xmm3")                        // xmm3     = s*a2 + s2*b2 = p2
            __ASM_EMIT("addps       %%xmm13, %%xmm11")                      // xmm11    = r*i2 + r2*j2 = q2

            // Shift buffer and update delay
            __ASM_EMIT("shufps      $0x93, %%xmm9, %%xmm9")                 // xmm9     = r2[3] r2[0] r2[1] r2[2]
            __ASM_EMIT("shufps      $0x93, %%xmm1, %%xmm1")                 // xmm1     = s2[3] s2[0] s2[1] s2[2]
            __ASM_EMIT("addps       %%xmm7, %%xmm2")                        // xmm2     = p1 + d2
            __ASM_EMIT("addps       %%xmm15, %%xmm10")                      // xmm10    = q1 + d3
            __ASM_EMIT("movss       %%xmm1, %%xmm9")                        // xmm9     = s2[3] r2[0] r2[1] r2[2]

            // Update delay only by mask for filters
            __ASM_EMIT("movaps      0x00 + %[MASK], %%xmm0")                // xmm0     = MASK_LO
            __ASM_EMIT("movaps      0x10 + %[MASK], %%xmm8")                // xmm8     = MASK_HI
            __ASM_EMIT("movaps      %%xmm0, %%xmm4")                        // xmm4     = MASK_LO
            __ASM_EMIT("movaps      %%xmm8, %%xmm12")                       // xmm12    = MASK_HI
            __ASM_EMIT("movaps      %%xmm0, %%xmm5")                        // xmm5     = MASK_LO
            __ASM_EMIT("movaps      %%xmm8, %%xmm13")                       // xmm13    = MASK_HI
            __ASM_EMIT("andps       %%xmm4, %%xmm2")                        // xmm2     = (p1 + d2) & MASK_LO
            __ASM_EMIT("andps       %%xmm12, %%xmm10")                      // xmm10    = (q1 + d3) & MASK_HI
            __ASM_EMIT("andps       %%xmm5, %%xmm3")                        // xmm3     = p2 & MASK_LO
            __ASM_EMIT("andps       %%xmm13, %%xmm11")                      // xmm11    = q2 & MASK_HI
            __ASM_EMIT("andnps      %%xmm6, %%xmm4")                        // xmm4     = d0 & ~MASK_LO
            __ASM_EMIT("andnps      %%xmm14, %%xmm12")                      // xmm12    = d1 & ~MASK_HI
            __ASM_EMIT("andnps      %%xmm7, %%xmm5")                        // xmm5     = d2 & ~MASK_LO
            __ASM_EMIT("andnps      %%xmm15, %%xmm13")                      // xmm13    = d3 & ~MASK_HI
            __ASM_EMIT("orps        %%xmm2, %%xmm4")                        // xmm4     = (p1 + d2) & MASK_LO | (d0 & ~MASK_LO)
            __ASM_EMIT("orps        %%xmm10, %%xmm12")                      // xmm12    = (q1 + d3) & MASK_HI | (d1 & ~MASK_HI)
            __ASM_EMIT("orps        %%xmm3, %%xmm5")                        // xmm5     = (p2 & MASK_LO) | (d2 & ~MASK_LO)
            __ASM_EMIT("orps        %%xmm11, %%xmm13")                      // xmm13    = (q2 & MASK_HI) | (d3 & ~MASK_HI)
            __ASM_EMIT("movaps      %%xmm4, %%xmm6")                        // xmm6     = d0'
            __ASM_EMIT("movaps      %%xmm12, %%xmm14")                      // xmm14    = d1'
            __ASM_EMIT("movaps      %%xmm5, %%xmm7")                        // xmm7     = d2'
            __ASM_EMIT("movaps      %%xmm13, %%xmm15")                      // xmm15    = d3'

            // Shift mask and repeat loop
            __ASM_EMIT("dec         %[count]")
            __ASM_EMIT("jz          4f")                                    // jump to completion
            __ASM_EMIT("lea         0x01(,%[mask],2), %[mask]")             // mask     = (mask << 1) | 1
            __ASM_EMIT("movaps      %%xmm0, %%xmm2")                        // xmm2     = m[0] m[1] m[2] m[3]
            __ASM_EMIT("shufps      $0x93, %%xmm8, %%xmm8")                 // xmm8     = m[7] m[4] m[5] m[6]
            __ASM_EMIT("shufps      $0x93, %%xmm0, %%xmm0")                 // xmm0     = m[3] m[0] m[1] m[2]
            __ASM_EMIT("movss       %%xmm0, %%xmm8")                        // xmm8     = m[3] m[4] m[5] m[6]
            __ASM_EMIT("movss       %%xmm2, %%xmm0")                        // xmm0     = m[0] m[0] m[1] m[2]
            __ASM_EMIT("movaps      %%xmm0, 0x00 + %[MASK]")                // *MASK_LO = xmm0
            __ASM_EMIT("movaps      %%xmm8, 0x10 + %[MASK]")                // *MASK_HI = xmm8
            __ASM_EMIT("cmp         $0xff, %[mask]")
            __ASM_EMIT("jne         1b")

            //-------------------------------------------------------------
            // 8x filter processing without mask
            __ASM_EMIT(".align 16")
            __ASM_EMIT("3:")
            __ASM_EMIT("movss       (%[src]), %%xmm0")                      // xmm0     = *src
            __ASM_EMIT("movss       %%xmm0, %%xmm1")                        // xmm1     = s
            __ASM_EMIT("add         $4, %[src]")                            // src      ++
            __ASM_EMIT("movaps      %%xmm1, %%xmm2")                        // xmm2     = s
            __ASM_EMIT("movaps      %%xmm9, %%xmm10")                       // xmm10    = r
            __ASM_EMIT("movaps      %%xmm1, %%xmm3")                        // xmm3     = s
            __ASM_EMIT("movaps      %%xmm9, %%xmm11")                       // xmm11    = r
            __ASM_EMIT("mulps       " BIQUAD_X8_A0_SOFF "(%[f]), %%xmm1")   // xmm1     = s*a0
            __ASM_EMIT("mulps       " BIQUAD_X8_I0_SOFF "(%[f]), %%xmm9")   // xmm9     = r*i0
            __ASM_EMIT("mulps       " BIQUAD_X8_A1_SOFF "(%[f]), %%xmm2")   // xmm2     = s*a1
            __ASM_EMIT("mulps       " BIQUAD_X8_I1_SOFF "(%[f]), %%xmm10")  // xmm10    = r*i1
            __ASM_EMIT("addps       %%xmm6, %%xmm1")                        // xmm1     = s*a0+d0 = s2
            __ASM_EMIT("addps       %%xmm14, %%xmm9")                       // xmm9     = r*i0+d1 = r2
            __ASM_EMIT("mulps       " BIQUAD_X8_A2_SOFF "(%[f]), %%xmm3")   // xmm3     = s*a2
            __ASM_EMIT("mulps       " BIQUAD_X8_I2_SOFF "(%[f]), %%xmm11")  // xmm11    = r*i2
            __ASM_EMIT("movaps      %%xmm1, %%xmm4")                        // xmm4     = s2
            __ASM_EMIT("movaps      %%xmm9, %%xmm12")                       // xmm12    = r2
            __ASM_EMIT("movaps      %%xmm1, %%xmm5")                        // xmm5     = s2
            __ASM_EMIT("movaps      %%xmm9, %%xmm13")                       // xmm13    = r2
            __ASM_EMIT("mulps       " BIQUAD_X8_B1_SOFF "(%[f]), %%xmm4")   // xmm4     = s2*b1
            __ASM_EMIT("mulps       " BIQUAD_X8_J1_SOFF "(%[f]), %%xmm12")  // xmm12    = r2*j1
            __ASM_EMIT("mulps       " BIQUAD_X8_B2_SOFF "(%[f]), %%xmm5")   // xmm5     = s2*b2
            __ASM_EMIT("mulps       " BIQUAD_X8_J2_SOFF "(%[f]), %%xmm13")  // xmm13    = r2*j2
            __ASM_EMIT("addps       %%xmm4, %%xmm2")                        // xmm2     = s*a1 + s2*b1 = p1
            __ASM_EMIT("addps       %%xmm12, %%xmm10")                      // xmm10    = r*i1 + r2*j1 = q1
            __ASM_EMIT("addps       %%xmm5, %%xmm3")                        // xmm3     = s*a2 + s2*b2 = p2
            __ASM_EMIT("addps       %%xmm13, %%xmm11")                      // xmm11    = r*i2 + r2*j2 = q2

            // Shift buffer and update delay
            __ASM_EMIT("shufps      $0x93, %%xmm9, %%xmm9")                 // xmm9     = r2[3] r2[0] r2[1] r2[2]
            __ASM_EMIT("shufps      $0x93, %%xmm1, %%xmm1")                 // xmm1     = s2[3] s2[0] s2[1] s2[2]
            __ASM_EMIT("addps       %%xmm7, %%xmm2")                        // xmm2     = p1 + d2
            __ASM_EMIT("addps       %%xmm15, %%xmm10")                      // xmm10    = q1 + d3
            __ASM_EMIT("movss       %%xmm9, (%[dst])")                      // *dst     = r2[3]
            __ASM_EMIT("movss       %%xmm1, %%xmm9")                        // xmm9     = s2[3] r2[0] r2[1] r2[2]
            __ASM_EMIT("movaps      %%xmm2, %%xmm6")                        // xmm6     = d0'
            __ASM_EMIT("movaps      %%xmm10, %%xmm14")                      // xmm14    = d1'
            __ASM_EMIT("movaps      %%xmm3, %%xmm7")                        // xmm7     = d2'
            __ASM_EMIT("movaps      %%xmm11, %%xmm15")                      // xmm15    = d3'

            // Repeat loop
            __ASM_EMIT("add         $4, %[dst]")                            // dst      ++
            __ASM_EMIT("dec         %[count]")
            __ASM_EMIT("jnz         3b")

            //-------------------------------------------------------------
            // Prepare last loop
            __ASM_EMIT("4:")
            __ASM_EMIT("movaps      0x00 + %[MASK], %%xmm0")                // xmm0     = m[0] m[1] m[2] m[3]
            __ASM_EMIT("movaps      0x10 + %[MASK], %%xmm8")                // xmm8     = m[4] m[5] m[6] m[7]
            __ASM_EMIT("shufps      $0x93, %%xmm0, %%xmm0")                 // xmm0     = m[3] m[0] m[1] m[2]
            __ASM_EMIT("shufps      $0x93, %%xmm8, %%xmm8")                 // xmm8     = m[7] m[4] m[5] m[6]
            __ASM_EMIT("xorps       %%xmm2, %%xmm2")                        // xmm2     = 0 0 0 0
            __ASM_EMIT("movss       %%xmm0, %%xmm8")                        // xmm8     = m[3] m[4] m[5] m[6]
            __ASM_EMIT("shl         $1, %[mask]")                           // mask     = mask << 1
            __ASM_EMIT("movss       %%xmm2, %%xmm0")                        // xmm0     = 0 m[0] m[1] m[2]

            //-------------------------------------------------------------
            // Process last steps
            __ASM_EMIT(".align 16")
            __ASM_EMIT("5:")
            __ASM_EMIT("movaps      %%xmm1, %%xmm2")                        // xmm2     = s
            __ASM_EMIT("movaps      %%xmm9, %%xmm10")                       // xmm10    = r
            __ASM_EMIT("movaps      %%xmm1, %%xmm3")                        // xmm3     = s
            __ASM_EMIT("movaps      %%xmm9, %%xmm11")                       // xmm11    = r
            __ASM_EMIT("mulps       " BIQUAD_X8_A0_SOFF "(%[f]), %%xmm1")   // xmm1     = s*a0
            __ASM_EMIT("mulps       " BIQUAD_X8_I0_SOFF "(%[f]), %%xmm9")   // xmm9     = r*i0
            __ASM_EMIT("mulps       " BIQUAD_X8_A1_SOFF "(%[f]), %%xmm2")   // xmm2     = s*a1
            __ASM_EMIT("mulps       " BIQUAD_X8_I1_SOFF "(%[f]), %%xmm10")  // xmm10    = r*i1
            __ASM_EMIT("addps       %%xmm6, %%xmm1")                        // xmm1     = s*a0+d0 = s2
            __ASM_EMIT("addps       %%xmm14, %%xmm9")                       // xmm9     = r*i0+d1 = r2
            __ASM_EMIT("mulps       " BIQUAD_X8_A2_SOFF "(%[f]), %%xmm3")   // xmm3     = s*a2
            __ASM_EMIT("mulps       " BIQUAD_X8_I2_SOFF "(%[f]), %%xmm11")  // xmm11    = r*i2
            __ASM_EMIT("movaps      %%xmm1, %%xmm4")                        // xmm4     = s2
            __ASM_EMIT("movaps      %%xmm9, %%xmm12")                       // xmm12    = r2
            __ASM_EMIT("movaps      %%xmm1, %%xmm5")                        // xmm5     = s2
            __ASM_EMIT("movaps      %%xmm9, %%xmm13")                       // xmm13    = r2
            __ASM_EMIT("mulps       " BIQUAD_X8_B1_SOFF "(%[f]), %%xmm4")   // xmm4     = s2*b1
            __ASM_EMIT("mulps       " BIQUAD_X8_J1_SOFF "(%[f]), %%xmm12")  // xmm12    = r2*j1
            __ASM_EMIT("mulps       " BIQUAD_X8_B2_SOFF "(%[f]), %%xmm5")   // xmm5     = s2*b2
            __ASM_EMIT("mulps       " BIQUAD_X8_J2_SOFF "(%[f]), %%xmm13")  // xmm13    = r2*j2
            __ASM_EMIT("addps       %%xmm4, %%xmm2")                        // xmm2     = s*a1 + s2*b1 = p1
            __ASM_EMIT("addps       %%xmm12, %%xmm10")                      // xmm10    = r*i1 + r2*j1 = q1
            __ASM_EMIT("addps       %%xmm5, %%xmm3")                        // xmm3     = s*a2 + s2*b2 = p2
            __ASM_EMIT("addps       %%xmm13, %%xmm11")                      // xmm11    = r*i2 + r2*j2 = q2

            // Shift buffer and update delay
            __ASM_EMIT("shufps      $0x93, %%xmm9, %%xmm9")                 // xmm9     = r2[3] r2[0] r2[1] r2[2]
            __ASM_EMIT("shufps      $0x93, %%xmm1, %%xmm1")                 // xmm1     = s2[3] s2[0] s2[1] s2[2]
            __ASM_EMIT("addps       %%xmm7, %%xmm2")                        // xmm2     = p1 + d2
            __ASM_EMIT("addps       %%xmm15, %%xmm10")                      // xmm10    = q1 + d3
            __ASM_EMIT("test        $0x80, %[mask]")
            __ASM_EMIT("jz          6f")
            __ASM_EMIT("movss       %%xmm9, (%[dst])")                      // *dst     = r2[3]
            __ASM_EMIT("add         $4, %[dst]")                            // dst      ++
            __ASM_EMIT("6:")
            __ASM_EMIT("movss       %%xmm1, %%xmm9")                        // xmm9     = s2[3] r2[0] r2[1] r2[2]

            // Update delay only by mask for filters
            __ASM_EMIT("movaps      %%xmm0, %%xmm4")                        // xmm4     = MASK_LO
            __ASM_EMIT("movaps      %%xmm8, %%xmm12")                       // xmm12    = MASK_HI
            __ASM_EMIT("movaps      %%xmm0, %%xmm5")                        // xmm5     = MASK_LO
            __ASM_EMIT("movaps      %%xmm8, %%xmm13")                       // xmm13    = MASK_HI
            __ASM_EMIT("andps       %%xmm4, %%xmm2")                        // xmm2     = (p1 + d2) & MASK_LO
            __ASM_EMIT("andps       %%xmm12, %%xmm10")                      // xmm10    = (q1 + d3) & MASK_HI
            __ASM_EMIT("andps       %%xmm5, %%xmm3")                        // xmm3     = p2 & MASK_LO
            __ASM_EMIT("andps       %%xmm13, %%xmm11")                      // xmm11    = q2 & MASK_HI
            __ASM_EMIT("andnps      %%xmm6, %%xmm4")                        // xmm4     = d0 & ~MASK_LO
            __ASM_EMIT("andnps      %%xmm14, %%xmm12")                      // xmm12    = d1 & ~MASK_HI
            __ASM_EMIT("andnps      %%xmm7, %%xmm5")                        // xmm5     = d2 & ~MASK_LO
            __ASM_EMIT("andnps      %%xmm15, %%xmm13")                      // xmm13    = d3 & ~MASK_HI
            __ASM_EMIT("orps        %%xmm2, %%xmm4")                        // xmm4     = (p1 + d2) & MASK_LO | (d0 & ~MASK_LO)
            __ASM_EMIT("orps        %%xmm10, %%xmm12")                      // xmm12    = (q1 + d3) & MASK_HI | (d1 & ~MASK_HI)
            __ASM_EMIT("orps        %%xmm3, %%xmm5")                        // xmm5     = (p2 & MASK_LO) | (d2 & ~MASK_LO)
            __ASM_EMIT("orps        %%xmm11, %%xmm13")                      // xmm13    = (q2 & MASK_HI) | (d3 & ~MASK_HI)
            __ASM_EMIT("movaps      %%xmm4, %%xmm6")                        // xmm6     = d0'
            __ASM_EMIT("movaps      %%xmm12, %%xmm14")                      // xmm14    = d1'
            __ASM_EMIT("movaps      %%xmm5, %%xmm7")                        // xmm7     = d2'
            __ASM_EMIT("movaps      %%xmm13, %%xmm15")                      // xmm15    = d3'

            // Shift mask and repeat loop
            __ASM_EMIT("xorps       %%xmm2, %%xmm2")                        // xmm2     = 0 0 0 0
            __ASM_EMIT("shufps      $0x93, %%xmm0, %%xmm0")                 // xmm0     = m[3] m[0] m[1] m[2]
            __ASM_EMIT("shufps      $0x93, %%xmm8, %%xmm8")                 // xmm8     = m[7] m[4] m[5] m[6]
            __ASM_EMIT("shl         $1, %[mask]")                           // mask     = mask << 1
            __ASM_EMIT("movss       %%xmm0, %%xmm8")                        // xmm8     = m[3] m[4] m[5] m[6]
            __ASM_EMIT("and         $0xff, %[mask]")                        // mask     = (mask << 1) & 0xff
            __ASM_EMIT("movss       %%xmm2, %%xmm0")                        // xmm0     = 0 m[0] m[1] m[2]
            __ASM_EMIT("jnz         5b")                                    // check that mask is not zero

            //-------------------------------------------------------------
            // Store delay buffers
            __ASM_EMIT("movaps      %%xmm6, "  BIQUAD_D0_SOFF "(%[f])")     // xmm6     = d0
            __ASM_EMIT("movaps      %%xmm14, " BIQUAD_D1_SOFF "(%[f])")     // xmm14    = d1
            __ASM_EMIT("movaps      %%xmm7, "  BIQUAD_D2_SOFF "(%[f])")     // xmm7     = d2
            __ASM_EMIT("movaps      %%xmm15, " BIQUAD_D3_SOFF "(%[f])")     // xmm15    = d3

            // Exit label
            __ASM_EMIT("8:")

            : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count),
              [mask] "=&r" (mask)
            :
              [f] "r" (f),
              [X_MASK] "m" (X_MASK0001),
              [MASK] "o" (MASK)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5", "%xmm6", "%xmm7",
              "%xmm8", "%xmm9", "%xmm10", "%xmm11", "%xmm12", "%xmm13", "%xmm14", "%xmm15"
        );
    }
    #endif /* ARCH_X86_64 */
}

#endif /* DSP_ARCH_X86_SSE3_FILTERS_STATIC_H_ */
