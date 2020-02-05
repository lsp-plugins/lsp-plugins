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

namespace sse
{
    void biquad_process_x1(float *dst, const float *src, size_t count, biquad_t *f)
    {
        IF_ARCH_X86(size_t off);

        ARCH_X86_ASM
        (
            // Check count
            __ASM_EMIT("test        %[count], %[count]")
            __ASM_EMIT("jz          2f")

            // Load permanent data
            __ASM_EMIT("movss       0x00(%[f]), %%xmm6")                            // xmm6 = d0
            __ASM_EMIT("xor         %[off], %[off]")
            __ASM_EMIT("movss       0x04(%[f]), %%xmm7")                            // xmm7 = d1

            // Start loop
            __ASM_EMIT("1:")
            __ASM_EMIT("movss       (%[src], %[off], 4), %%xmm0")                   // xmm0 = s ? ? ?
            __ASM_EMIT("movss       " BIQUAD_XN_SOFF " + 0x00(%[f]), %%xmm1")       // xmm1 = a0
            __ASM_EMIT("movss       " BIQUAD_XN_SOFF " + 0x04(%[f]), %%xmm2")       // xmm2 = a1
            __ASM_EMIT("mulss       %%xmm0, %%xmm1")                                // xmm1 = a0*s
            __ASM_EMIT("movss       " BIQUAD_XN_SOFF " + 0x0c(%[f]), %%xmm3")       // xmm3 = b1
            __ASM_EMIT("mulss       %%xmm0, %%xmm2")                                // xmm2 = a1*s
            __ASM_EMIT("addss       %%xmm6, %%xmm1")                                // xmm1 = s' = a0*s + d0
            __ASM_EMIT("mulss       " BIQUAD_XN_SOFF " + 0x08(%[f]), %%xmm0")       // xmm0 = a2*s
            __ASM_EMIT("movss       %%xmm1, (%[dst], %[off], 4)")                   // *dst = s'
            __ASM_EMIT("movaps      %%xmm7, %%xmm6")                                // xmm6 = d1
            __ASM_EMIT("mulss       %%xmm1, %%xmm3")                                // xmm3 = b1*s'
            __ASM_EMIT("add         $1, %[off]")
            __ASM_EMIT("mulss       " BIQUAD_XN_SOFF " + 0x10(%[f]), %%xmm1")       // xmm1 = b2*s'
            __ASM_EMIT("addss       %%xmm3, %%xmm2")                                // xmm2 = a1*s + b1*s'
            __ASM_EMIT("addss       %%xmm0, %%xmm1")                                // xmm3 = d1' = a2*s + b2*s'
            __ASM_EMIT("cmp         %[count], %[off]")
            __ASM_EMIT("addss       %%xmm2, %%xmm6")                                // xmm6 = d0' = d1 + a1*s + b1*s'
            __ASM_EMIT("movaps      %%xmm1, %%xmm7")                                // xmm7 = d1'
            __ASM_EMIT("jb          1b")

            // Store the updated buffer state
            __ASM_EMIT("movss       %%xmm6, 0x00(%[f])")
            __ASM_EMIT("movss       %%xmm7, 0x04(%[f])")

            // Exit label
            __ASM_EMIT("2:")

            : [off] "=&r"(off)
            : [dst] "r" (dst), [src] "r" (src),
              [count] "r" (count),
              [f] "r" (f)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm6", "%xmm7"
        );
    }

    void biquad_process_x2(float *dst, const float *src, size_t count, biquad_t *f)
    {
        ARCH_X86_ASM
        (
            // Check count
            __ASM_EMIT("test        %[count], %[count]")
            __ASM_EMIT("jz          4f")

            // Start loop
            __ASM_EMIT("movss       (%[src]), %%xmm0")                          // xmm0 = s ? ? ?
            __ASM_EMIT("movlps      " BIQUAD_XN_SOFF " + 0x00(%[f]), %%xmm1")   // xmm1 = a0
            __ASM_EMIT("movlps      " BIQUAD_XN_SOFF " + 0x08(%[f]), %%xmm2")   // xmm2 = a1
            __ASM_EMIT("mulss       %%xmm0, %%xmm1")                            // xmm1 = a0*s
            __ASM_EMIT("movlps      " BIQUAD_XN_SOFF " + 0x10(%[f]), %%xmm3")   // xmm3 = a2
            __ASM_EMIT("mulss       %%xmm0, %%xmm2")                            // xmm2 = a1*s
            __ASM_EMIT("movlps      " BIQUAD_XN_SOFF " + 0x18(%[f]), %%xmm4")   // xmm4 = b1
            __ASM_EMIT("addss       0x00(%[f]), %%xmm1")                        // xmm1 = s' = a0*s + d0
            __ASM_EMIT("mulss       %%xmm0, %%xmm3")                            // xmm3 = a2*s
            __ASM_EMIT("mulss       %%xmm1, %%xmm4")                            // xmm4 = b1*s'
            __ASM_EMIT("movaps      %%xmm1, %%xmm0")                            // xmm0 = s'
            __ASM_EMIT("mulss       " BIQUAD_XN_SOFF " + 0x20(%[f]), %%xmm1")   // xmm1 = b2*s'
            __ASM_EMIT("shufps      $0xb1, %%xmm0, %%xmm0")                     // shift
            __ASM_EMIT("addss       %%xmm4, %%xmm2")                            // xmm2 = a1*s + b1*s'
            __ASM_EMIT("addss       %%xmm3, %%xmm1")                            // xmm1 = d1' = a2*s + b2*s'
            __ASM_EMIT("addss       0x08(%[f]), %%xmm2")                        // xmm2 = d0' = d1 + a1*s + b1*s'
            __ASM_EMIT("add         $0x04, %[src]")                             // src++
            __ASM_EMIT("dec         %[count]")
            __ASM_EMIT("movss       %%xmm2, 0x00(%[f])")
            __ASM_EMIT("movss       %%xmm1, 0x08(%[f])")
            __ASM_EMIT("jz          2f")
            // x2 loop
            __ASM_EMIT("movaps      0x00(%[f]), %%xmm6")                        // xmm6 = d0 e0 d1 e1
            __ASM_EMIT("xorps       %%xmm7, %%xmm7")                            // xmm7 = 0 0 0 0
            __ASM_EMIT(".align      16")
            __ASM_EMIT("1:")
            __ASM_EMIT("movss       (%[src]), %%xmm4")                          // xmm4 = s0
            __ASM_EMIT("movlps      " BIQUAD_XN_SOFF " + 0x00(%[f]), %%xmm1")   // xmm1 = a0 i0
            __ASM_EMIT("movss       %%xmm4, %%xmm0")                            // xmm0 = s0 s1
            __ASM_EMIT("movups      " BIQUAD_XN_SOFF " + 0x08(%[f]), %%xmm2")   // xmm2 = a1 i1 a2 i2
            __ASM_EMIT("movlhps     %%xmm0, %%xmm0")                            // xmm0 = s0 s1 s0 s1
            __ASM_EMIT("movups      " BIQUAD_XN_SOFF " + 0x18(%[f]), %%xmm3")   // xmm3 = b1 j1 b2 j2
            __ASM_EMIT("mulps       %%xmm0, %%xmm2")                            // xmm2 = a1*s0 i1*s1 a2*s0 i2*s1
            __ASM_EMIT("mulps       %%xmm1, %%xmm0")                            // xmm0 = a0*s0 i0*s1
            __ASM_EMIT("addps       %%xmm6, %%xmm0")                            // xmm0 = s0' s1' = d0+a0*s0 e0+i0*s1
            __ASM_EMIT("shufps      $0x0e, %%xmm7, %%xmm6")                     // xmm6 = d1 e1 0 0
            __ASM_EMIT("movlhps     %%xmm0, %%xmm0")                            // xmm0 = s0' s1' s0' s1'
            __ASM_EMIT("addps       %%xmm2, %%xmm6")                            // xmm6 = d1+a1*s0 e1+i1*s1 a2*s0 i2*s1
            __ASM_EMIT("mulps       %%xmm0, %%xmm3")                            // xmm3 = b1*s0' j1*s1' b2*s0' j2*s1'
            __ASM_EMIT("shufps      $0xb1, %%xmm0, %%xmm0")                     // shift
            __ASM_EMIT("addps       %%xmm3, %%xmm6")                            // xmm6 = d0' e0' d1' e1' = d1+a1*s0+b1*s0' e1+i1*s1+j1*s1' a2*s0+b2*s0' i2*s1+j2*s1'
            __ASM_EMIT("movss       %%xmm0, (%[dst])")
            __ASM_EMIT("add         $0x04, %[src]")
            __ASM_EMIT("add         $0x04, %[dst]")
            __ASM_EMIT("dec         %[count]")
            __ASM_EMIT("jnz         1b")
            __ASM_EMIT("movaps      %%xmm6, 0x00(%[f])")
            // Last step
            __ASM_EMIT("2:")
            __ASM_EMIT("shufps      $0xb1, %%xmm0, %%xmm0")                     // shift
            __ASM_EMIT("movss       " BIQUAD_XN_SOFF " + 0x04(%[f]), %%xmm1")   // xmm1 = a0
            __ASM_EMIT("movss       " BIQUAD_XN_SOFF " + 0x0c(%[f]), %%xmm2")   // xmm2 = a1
            __ASM_EMIT("mulss       %%xmm0, %%xmm1")                            // xmm1 = a0*s
            __ASM_EMIT("movss       " BIQUAD_XN_SOFF " + 0x14(%[f]), %%xmm3")   // xmm3 = a2
            __ASM_EMIT("mulss       %%xmm0, %%xmm2")                            // xmm2 = a1*s
            __ASM_EMIT("movss       " BIQUAD_XN_SOFF " + 0x1c(%[f]), %%xmm4")   // xmm4 = b1
            __ASM_EMIT("addss       0x04(%[f]), %%xmm1")                        // xmm1 = s' = a0*s + d0
            __ASM_EMIT("mulss       %%xmm0, %%xmm3")                            // xmm3 = a2*s
            __ASM_EMIT("mulss       %%xmm1, %%xmm4")                            // xmm4 = b1*s'
            __ASM_EMIT("movaps      %%xmm1, %%xmm0")                            // xmm0 = s'
            __ASM_EMIT("mulss      " BIQUAD_XN_SOFF " + 0x24(%[f]), %%xmm1")    // xmm1 = b2*s'
            __ASM_EMIT("movss       %%xmm0, (%[dst])")
            __ASM_EMIT("addps       %%xmm4, %%xmm2")                            // xmm2 = a1*s + b1*s'
            __ASM_EMIT("addps       %%xmm3, %%xmm1")                            // xmm3 = d1' = a2*s + b2*s'
            __ASM_EMIT("addss       0x0c(%[f]), %%xmm2")                        // xmm6 = d0' = d1 + a1*s + b1*s'
            __ASM_EMIT("movss       %%xmm2, 0x04(%[f])")
            __ASM_EMIT("movss       %%xmm1, 0x0c(%[f])")

            // Exit label
            __ASM_EMIT("4:")

            : [dst] "+r" (dst), [src] "+r" (src)
            : [count] "r" (count), [f] "r" (f)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void biquad_process_x4(float *dst, const float *src, size_t count, biquad_t *f)
    {
        IF_ARCH_X86(
            float   MASK[4] __lsp_aligned16;
            size_t  mask;
        )

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
            __ASM_EMIT("movaps      0x00(%[f]), %%xmm6")                        // xmm6     = d0
            __ASM_EMIT("movaps      0x10(%[f]), %%xmm7")                        // xmm7     = d1

            // Process first 3 steps
            __ASM_EMIT(".align 16")
            __ASM_EMIT("1:")
            __ASM_EMIT("movss       (%[src]), %%xmm0")                          // xmm0     = *src
            __ASM_EMIT("add         $4, %[src]")                                // src      ++
            __ASM_EMIT("movss       %%xmm0, %%xmm1")                            // xmm1     = s
            __ASM_EMIT("movaps      %%xmm1, %%xmm2")                            // xmm2     = s
            __ASM_EMIT("movaps      %%xmm1, %%xmm3")                            // xmm3     = s
            __ASM_EMIT("mulps       0x00 + " BIQUAD_XN_SOFF "(%[f]), %%xmm1")   // xmm1     = s*a0
            __ASM_EMIT("mulps       0x10 + " BIQUAD_XN_SOFF "(%[f]), %%xmm2")   // xmm2     = s*a1
            __ASM_EMIT("addps       %%xmm6, %%xmm1")                            // xmm1     = s*a0+d0 = s2
            __ASM_EMIT("mulps       0x20 + " BIQUAD_XN_SOFF "(%[f]), %%xmm3")   // xmm3     = s*a2
            __ASM_EMIT("movaps      %%xmm1, %%xmm4")                            // xmm4     = s2
            __ASM_EMIT("movaps      %%xmm1, %%xmm5")                            // xmm5     = s2
            __ASM_EMIT("mulps       0x30 + " BIQUAD_XN_SOFF "(%[f]), %%xmm4")   // xmm4     = s2*b1
            __ASM_EMIT("mulps       0x40 + " BIQUAD_XN_SOFF "(%[f]), %%xmm5")   // xmm5     = s2*b2
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
            __ASM_EMIT("mulps       0x00 + " BIQUAD_XN_SOFF "(%[f]), %%xmm1")   // xmm1     = s*a0
            __ASM_EMIT("mulps       0x10 + " BIQUAD_XN_SOFF "(%[f]), %%xmm2")   // xmm2     = s*a1
            __ASM_EMIT("addps       %%xmm6, %%xmm1")                            // xmm1     = s*a0+d0 = s2
            __ASM_EMIT("mulps       0x20 + " BIQUAD_XN_SOFF "(%[f]), %%xmm3")   // xmm3     = s*a2
            __ASM_EMIT("movaps      %%xmm1, %%xmm4")                            // xmm4     = s2
            __ASM_EMIT("movaps      %%xmm1, %%xmm5")                            // xmm5     = s2
            __ASM_EMIT("mulps       0x30 + " BIQUAD_XN_SOFF "(%[f]), %%xmm4")   // xmm4     = s2*b1
            __ASM_EMIT("mulps       0x40 + " BIQUAD_XN_SOFF "(%[f]), %%xmm5")   // xmm5     = s2*b2
            __ASM_EMIT("addps       %%xmm4, %%xmm2")                            // xmm2     = s*a1 + s2*b1 = p1
            __ASM_EMIT("addps       %%xmm5, %%xmm3")                            // xmm3     = s*a2 + s2*b2 = p2
            __ASM_EMIT("addps       %%xmm7, %%xmm2")                		    // xmm2     = p1 + d1
            __ASM_EMIT("movaps      %%xmm3, %%xmm7")                		    // xmm7     = p2
            __ASM_EMIT("movaps      %%xmm2, %%xmm6")                		    // xmm6     = p1 + d1

            // Shift buffer and repeat loop
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
            __ASM_EMIT("mulps       0x00 + " BIQUAD_XN_SOFF "(%[f]), %%xmm1")   // xmm1     = s*a0
            __ASM_EMIT("mulps       0x10 + " BIQUAD_XN_SOFF "(%[f]), %%xmm2")   // xmm2     = s*a1
            __ASM_EMIT("addps       %%xmm6, %%xmm1")                            // xmm1     = s*a0+d0 = s2
            __ASM_EMIT("mulps       0x20 + " BIQUAD_XN_SOFF "(%[f]), %%xmm3")   // xmm3     = s*a2
            __ASM_EMIT("movaps      %%xmm1, %%xmm4")                            // xmm4     = s2
            __ASM_EMIT("movaps      %%xmm1, %%xmm5")                            // xmm5     = s2
            __ASM_EMIT("mulps       0x30 + " BIQUAD_XN_SOFF "(%[f]), %%xmm4")   // xmm4     = s2*b1
            __ASM_EMIT("mulps       0x40 + " BIQUAD_XN_SOFF "(%[f]), %%xmm5")   // xmm5     = s2*b2
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
            __ASM_EMIT("shl         $1, %[mask]")                               // mask     = mask << 1
            __ASM_EMIT("shufps      $0x90, %%xmm0, %%xmm0")                     // xmm0     = m[0] m[0] m[1] m[2]
            __ASM_EMIT("and         $0x0f, %[mask]")                            // mask     = (mask << 1) & 0x0f
            __ASM_EMIT("jnz         5b")                                        // check that mask is not zero

            // Store delay buffer
            __ASM_EMIT("movaps      %%xmm6, 0x00(%[f])")                        // xmm6     = d0
            __ASM_EMIT("movaps      %%xmm7, 0x10(%[f])")                        // xmm7     = d1

            // Exit label
            __ASM_EMIT("8:")

            : [dst] "+r" (dst), [src] "+r" (src), [mask] "=&r"(mask), [count] "+r" (count)
            :
              [f] "r" (f),
              [X_MASK] "m" (X_MASK0001),
              [MASK] "m" (MASK)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void biquad_process_x8(float *dst, const float *src, size_t count, biquad_t *f)
    {
        IF_ARCH_X86(
            float   MASK[4] __lsp_aligned16;
            size_t mask;
        )

#pragma pack(push)
        IF_ARCH_X86(
            struct {
                float  *dst;
                size_t  count;
            } context;
        )
#pragma pack(pop)

        ARCH_X86_ASM
        (
            // Check count
            __ASM_EMIT("test        %[count], %[count]")
            __ASM_EMIT("jz          10f")

            //---------------------------------------------------------------------
            // Cycle 1
            __ASM_EMIT32("mov       %[dst], 0x00 + %[context]")
            __ASM_EMIT32("mov       %[count], 0x04 + %[context]")
            __ASM_EMIT64("mov       %[dst], 0x00 + %[context]")
            __ASM_EMIT64("mov       %[count], 0x08 + %[context]")

            // Initialize mask
            // xmm0=tmp, xmm1={s,s2[4]}, xmm2=p1[4], xmm3=p2[4], xmm6=d0[4], xmm7=d1[4]
            __ASM_EMIT("mov         $1, %[mask]")
            __ASM_EMIT("movaps      %[X_MASK], %%xmm0")
            __ASM_EMIT("xorps       %%xmm1, %%xmm1")
            __ASM_EMIT("movaps      %%xmm0, %[MASK]")

            // Load delay buffer
            __ASM_EMIT("movaps      0x00(%[f]), %%xmm6")                        // xmm6     = d0
            __ASM_EMIT("movaps      0x20(%[f]), %%xmm7")                        // xmm7     = d1

            // Process first 3 steps
            __ASM_EMIT(".align 16")
            __ASM_EMIT("1:")
            __ASM_EMIT("movss       (%[src]), %%xmm0")                          // xmm0     = *src
            __ASM_EMIT("add         $4, %[src]")                                // src      ++
            __ASM_EMIT("movss       %%xmm0, %%xmm1")                            // xmm1     = s
            __ASM_EMIT("movaps      %%xmm1, %%xmm2")                            // xmm2     = s
            __ASM_EMIT("movaps      %%xmm1, %%xmm3")                            // xmm3     = s
            __ASM_EMIT("mulps       0x00 + " BIQUAD_XN_SOFF "(%[f]), %%xmm1")   // xmm1     = s*a0
            __ASM_EMIT("mulps       0x20 + " BIQUAD_XN_SOFF "(%[f]), %%xmm2")   // xmm2     = s*a1
            __ASM_EMIT("addps       %%xmm6, %%xmm1")                            // xmm1     = s*a0+d0 = s2
            __ASM_EMIT("mulps       0x40 + " BIQUAD_XN_SOFF "(%[f]), %%xmm3")   // xmm3     = s*a2
            __ASM_EMIT("movaps      %%xmm1, %%xmm4")                            // xmm4     = s2
            __ASM_EMIT("movaps      %%xmm1, %%xmm5")                            // xmm5     = s2
            __ASM_EMIT("mulps       0x60 + " BIQUAD_XN_SOFF "(%[f]), %%xmm4")   // xmm4     = s2*b1
            __ASM_EMIT("mulps       0x80 + " BIQUAD_XN_SOFF "(%[f]), %%xmm5")   // xmm5     = s2*b2
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
            __ASM_EMIT("mulps       0x00 + " BIQUAD_XN_SOFF "(%[f]), %%xmm1")   // xmm1     = s*a0
            __ASM_EMIT("mulps       0x20 + " BIQUAD_XN_SOFF "(%[f]), %%xmm2")   // xmm2     = s*a1
            __ASM_EMIT("addps       %%xmm6, %%xmm1")                            // xmm1     = s*a0+d0 = s2
            __ASM_EMIT("mulps       0x40 + " BIQUAD_XN_SOFF "(%[f]), %%xmm3")   // xmm3     = s*a2
            __ASM_EMIT("movaps      %%xmm1, %%xmm4")                            // xmm4     = s2
            __ASM_EMIT("movaps      %%xmm1, %%xmm5")                            // xmm5     = s2
            __ASM_EMIT("mulps       0x60 + " BIQUAD_XN_SOFF "(%[f]), %%xmm4")   // xmm4     = s2*b1
            __ASM_EMIT("mulps       0x80 + " BIQUAD_XN_SOFF "(%[f]), %%xmm5")   // xmm5     = s2*b2
            __ASM_EMIT("addps       %%xmm4, %%xmm2")                            // xmm2     = s*a1 + s2*b1 = p1
            __ASM_EMIT("addps       %%xmm5, %%xmm3")                            // xmm3     = s*a2 + s2*b2 = p2
            __ASM_EMIT("addps       %%xmm7, %%xmm2")                            // xmm2     = p1 + d1
            __ASM_EMIT("movaps      %%xmm3, %%xmm7")                            // xmm7     = p2
            __ASM_EMIT("movaps      %%xmm2, %%xmm6")                            // xmm6     = p1 + d1

            // Shift buffer and repeat loop
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
            __ASM_EMIT("mulps       0x00 + " BIQUAD_XN_SOFF "(%[f]), %%xmm1")   // xmm1     = s*a0
            __ASM_EMIT("mulps       0x20 + " BIQUAD_XN_SOFF "(%[f]), %%xmm2")   // xmm2     = s*a1
            __ASM_EMIT("addps       %%xmm6, %%xmm1")                            // xmm1     = s*a0+d0 = s2
            __ASM_EMIT("mulps       0x40 + " BIQUAD_XN_SOFF "(%[f]), %%xmm3")   // xmm3     = s*a2
            __ASM_EMIT("movaps      %%xmm1, %%xmm4")                            // xmm4     = s2
            __ASM_EMIT("movaps      %%xmm1, %%xmm5")                            // xmm5     = s2
            __ASM_EMIT("mulps       0x60 + " BIQUAD_XN_SOFF "(%[f]), %%xmm4")   // xmm4     = s2*b1
            __ASM_EMIT("mulps       0x80 + " BIQUAD_XN_SOFF "(%[f]), %%xmm5")   // xmm5     = s2*b2
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
            __ASM_EMIT("xorps       %%xmm2, %%xmm2")                            // xmm2     = 0 0 0 0
            __ASM_EMIT("shl         $1, %[mask]")                               // mask     = mask << 1
            __ASM_EMIT("shufps      $0x90, %%xmm0, %%xmm0")                     // xmm0     = m[0] m[0] m[1] m[2]
            __ASM_EMIT("and         $0x0f, %[mask]")                            // mask     = (mask << 1) & 0x0f
            __ASM_EMIT("movss       %%xmm2, %%xmm0")                            // xmm0     = 0 m[0] m[1] m[2]
            __ASM_EMIT("jnz         5b")                                        // check that mask is not zero

            // Store delay buffer
            __ASM_EMIT("movaps      %%xmm6, 0x00(%[f])")                        // xmm6     = d0
            __ASM_EMIT("movaps      %%xmm7, 0x20(%[f])")                        // xmm7     = d1

            //---------------------------------------------------------------------
            // Cycle 2
            __ASM_EMIT32("mov       0x00 + %[context], %[dst]")
            __ASM_EMIT32("mov       0x04 + %[context], %[count]")
            __ASM_EMIT64("mov       0x00 + %[context], %[dst]")
            __ASM_EMIT64("mov       0x08 + %[context], %[count]")
            __ASM_EMIT("mov         %[dst], %[src]")                            // Chaining filter groups

            // Initialize mask
            // xmm0=tmp, xmm1={s,s2[4]}, xmm2=p1[4], xmm3=p2[4], xmm6=d0[4], xmm7=d1[4]
            __ASM_EMIT("mov         $1, %[mask]")
            __ASM_EMIT("movaps      %[X_MASK], %%xmm0")
            __ASM_EMIT("xorps       %%xmm1, %%xmm1")
            __ASM_EMIT("movaps      %%xmm0, %[MASK]")

            // Load delay buffer
            __ASM_EMIT("movaps      0x10(%[f]), %%xmm6")                        // xmm6     = d0
            __ASM_EMIT("movaps      0x30(%[f]), %%xmm7")                        // xmm7     = d1

            // Process first 3 steps
            __ASM_EMIT(".align 16")
            __ASM_EMIT("1:")
            __ASM_EMIT("movss       (%[src]), %%xmm0")                          // xmm0     = *src
            __ASM_EMIT("add         $4, %[src]")                                // src      ++
            __ASM_EMIT("movss       %%xmm0, %%xmm1")                            // xmm1     = s
            __ASM_EMIT("movaps      %%xmm1, %%xmm2")                            // xmm2     = s
            __ASM_EMIT("movaps      %%xmm1, %%xmm3")                            // xmm3     = s
            __ASM_EMIT("mulps       0x10 + " BIQUAD_XN_SOFF "(%[f]), %%xmm1")   // xmm1     = s*a0
            __ASM_EMIT("mulps       0x30 + " BIQUAD_XN_SOFF "(%[f]), %%xmm2")   // xmm2     = s*a1
            __ASM_EMIT("addps       %%xmm6, %%xmm1")                            // xmm1     = s*a0+d0 = s2
            __ASM_EMIT("mulps       0x50 + " BIQUAD_XN_SOFF "(%[f]), %%xmm3")   // xmm3     = s*a2
            __ASM_EMIT("movaps      %%xmm1, %%xmm4")                            // xmm4     = s2
            __ASM_EMIT("movaps      %%xmm1, %%xmm5")                            // xmm5     = s2
            __ASM_EMIT("mulps       0x70 + " BIQUAD_XN_SOFF "(%[f]), %%xmm4")   // xmm4     = s2*b1
            __ASM_EMIT("mulps       0x90 + " BIQUAD_XN_SOFF "(%[f]), %%xmm5")   // xmm5     = s2*b2
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
            __ASM_EMIT("mulps       0x10 + " BIQUAD_XN_SOFF "(%[f]), %%xmm1")   // xmm1     = s*a0
            __ASM_EMIT("mulps       0x30 + " BIQUAD_XN_SOFF "(%[f]), %%xmm2")   // xmm2     = s*a1
            __ASM_EMIT("addps       %%xmm6, %%xmm1")                            // xmm1     = s*a0+d0 = s2
            __ASM_EMIT("mulps       0x50 + " BIQUAD_XN_SOFF "(%[f]), %%xmm3")   // xmm3     = s*a2
            __ASM_EMIT("movaps      %%xmm1, %%xmm4")                            // xmm4     = s2
            __ASM_EMIT("movaps      %%xmm1, %%xmm5")                            // xmm5     = s2
            __ASM_EMIT("mulps       0x70 + " BIQUAD_XN_SOFF "(%[f]), %%xmm4")   // xmm4     = s2*b1
            __ASM_EMIT("mulps       0x90 + " BIQUAD_XN_SOFF "(%[f]), %%xmm5")   // xmm5     = s2*b2
            __ASM_EMIT("addps       %%xmm4, %%xmm2")                            // xmm2     = s*a1 + s2*b1 = p1
            __ASM_EMIT("addps       %%xmm5, %%xmm3")                            // xmm3     = s*a2 + s2*b2 = p2
            __ASM_EMIT("addps       %%xmm7, %%xmm2")                            // xmm2     = p1 + d1
            __ASM_EMIT("movaps      %%xmm3, %%xmm7")                            // xmm7     = p2
            __ASM_EMIT("movaps      %%xmm2, %%xmm6")                            // xmm6     = p1 + d1

            // Shift buffer and repeat loop
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
            __ASM_EMIT("mulps       0x10 + " BIQUAD_XN_SOFF "(%[f]), %%xmm1")   // xmm1     = s*a0
            __ASM_EMIT("mulps       0x30 + " BIQUAD_XN_SOFF "(%[f]), %%xmm2")   // xmm2     = s*a1
            __ASM_EMIT("addps       %%xmm6, %%xmm1")                            // xmm1     = s*a0+d0 = s2
            __ASM_EMIT("mulps       0x50 + " BIQUAD_XN_SOFF "(%[f]), %%xmm3")   // xmm3     = s*a2
            __ASM_EMIT("movaps      %%xmm1, %%xmm4")                            // xmm4     = s2
            __ASM_EMIT("movaps      %%xmm1, %%xmm5")                            // xmm5     = s2
            __ASM_EMIT("mulps       0x70 + " BIQUAD_XN_SOFF "(%[f]), %%xmm4")   // xmm4     = s2*b1
            __ASM_EMIT("mulps       0x90 + " BIQUAD_XN_SOFF "(%[f]), %%xmm5")   // xmm5     = s2*b2
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
            __ASM_EMIT("xorps       %%xmm2, %%xmm2")                            // xmm2     = 0 0 0 0
            __ASM_EMIT("shl         $1, %[mask]")                               // mask     = mask << 1
            __ASM_EMIT("shufps      $0x90, %%xmm0, %%xmm0")                     // xmm0     = m[0] m[0] m[1] m[2]
            __ASM_EMIT("and         $0x0f, %[mask]")                            // mask     = (mask << 1) & 0x0f
            __ASM_EMIT("movss       %%xmm2, %%xmm0")                            // xmm0     = 0 m[0] m[1] m[2]
            __ASM_EMIT("jnz         5b")                                        // check that mask is not zero

            // Store delay buffer
            __ASM_EMIT("movaps      %%xmm6, 0x10(%[f])")                        // xmm6     = d0
            __ASM_EMIT("movaps      %%xmm7, 0x30(%[f])")                        // xmm7     = d1

            // Exit label
            __ASM_EMIT("10:")

            : [dst] "+r" (dst), [src] "+r" (src), [mask] "=&r" (mask), [count] "+r" (count)
            :
              [f] "r" (f),
              [context] "o" (context),
              [X_MASK] "m" (X_MASK0001),
              [MASK] "m" (MASK)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }
}

#endif /* DSP_ARCH_X86_SSE_FILTERS_STATIC_H_ */
