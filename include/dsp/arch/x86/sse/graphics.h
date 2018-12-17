/*
 * graphics.h
 *
 *  Created on: 07 авг. 2016 г.
 *      Author: sadko
 */

/* Copyright (C) 2007  Julien Pommier

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.

  (this is the zlib license)
*/

#ifndef DSP_ARCH_X86_SSE_GRAPHICS_H_
#define DSP_ARCH_X86_SSE_GRAPHICS_H_

#ifndef DSP_ARCH_X86_SSE_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_SSE_IMPL */

namespace sse
{
    void axis_apply_log1(float *x, const float *v, float zero, float norm_x, size_t count)
    {
        //---------------------------------------------------------------
        // Prepare constants
        ARCH_X86_ASM
        (
            __ASM_EMIT("movss       %[zero], %%xmm0")
            __ASM_EMIT("movss       %[norm_x], %%xmm1")

            __ASM_EMIT("sub         $4, %[count]")
            __ASM_EMIT("jb          2f")
            __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0")         // xmm0 == zero
            __ASM_EMIT("shufps      $0x00, %%xmm1, %%xmm1")         // xmm1 == norm_x

            // Do x4 blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("movups      (%[v]), %%xmm3")                // xmm3 = v
            __ASM_EMIT("andps       0x00 + %[ILOG], %%xmm3")        // xmm3 = abs(v)
            __ASM_EMIT("add         $0x10, %[v]")                   // v   += 4
            __ASM_EMIT("maxps       0x00 + %[FLOG], %%xmm3")        // xmm3 = max(X_AMP, abs(v)), ignores denormalized values
            __ASM_EMIT("mulps       %%xmm0, %%xmm3")                // xmm3 = max(X_AMP, abs(v)) * zero
            // Step 2: parse float value
            __ASM_EMIT("movdqa      %%xmm3, %%xmm4")                // xmm4 = v
            __ASM_EMIT("psrld       $23, %%xmm4")                   // xmm4 = frac(v)
            __ASM_EMIT("andps       0x10 + %[ILOG], %%xmm3")        // xmm3 = mant(v)
            __ASM_EMIT("psubd       0x20 + %[ILOG], %%xmm4")        // xmm4 = frac(v) - 127
            __ASM_EMIT("orps        0x10 + %[FLOG], %%xmm3")        // xmm3 = V = mant(v)+0.5
            __ASM_EMIT("cvtdq2ps    %%xmm4, %%xmm4")                // xmm4 = E = float(frac(v)-127)
            // Prepare logarithm approximation calculations
            __ASM_EMIT("movaps      %%xmm3, %%xmm5")                // xmm5 = V
            __ASM_EMIT("movaps      %%xmm3, %%xmm6")                // xmm6 = V
            __ASM_EMIT("cmpltps     0x20 + %[FLOG], %%xmm5")        // xmm5 = [ V < sqrt(1/2) ]
            __ASM_EMIT("movaps      0x30 + %[FLOG], %%xmm7")        // xmm7 = 1.0
            __ASM_EMIT("andps       %%xmm5, %%xmm6")                // xmm6 = V * [ V < sqrt(1/2) ]
            __ASM_EMIT("addps       %%xmm6, %%xmm3")                // xmm3 = V + V * [ V < sqrt(1/2) ]
            __ASM_EMIT("andnps      %%xmm7, %%xmm5")                // xmm5 = 1.0 * [ V >= sqrt(1/2) ]
            __ASM_EMIT("addps       %%xmm5, %%xmm4")                // xmm4 = E + 1.0 * [ V >= sqrt(1/2) ]      = B
            __ASM_EMIT("subps       %%xmm7, %%xmm3")                // xmm3 = V + V * [ V < sqrt(1/2) ] - 1.0   = A
            // Calculate logarithmic values
            __ASM_EMIT("movaps      0x40 + %[FLOG], %%xmm5")        // xmm5 = L0
            __ASM_EMIT("mulps       %%xmm3, %%xmm5")                // xmm5 = L0*A
            __ASM_EMIT("addps       0x50 + %[FLOG], %%xmm5")        // xmm5 = L1+L0*A
            __ASM_EMIT("mulps       %%xmm3, %%xmm5")                // xmm5 = A*(L1+L0*A)
            __ASM_EMIT("addps       0x60 + %[FLOG], %%xmm5")        // xmm5 = L2+A*(L1+L0*A)
            __ASM_EMIT("mulps       %%xmm3, %%xmm5")                // xmm5 = A*(L2+A*(L1+L0*A))
            __ASM_EMIT("addps       0x70 + %[FLOG], %%xmm5")        // xmm5 = L3+A*L2+A*(L1+L0*A)
            __ASM_EMIT("mulps       %%xmm3, %%xmm5")                // xmm5 = A*(L3+A*L2+A*(L1+L0*A))
            __ASM_EMIT("addps       0x80 + %[FLOG], %%xmm5")        // xmm5 = L4+A*(L3+A*L2+A*(L1+L0*A))
            __ASM_EMIT("mulps       %%xmm3, %%xmm5")                // xmm5 = A*(L4+A*(L3+A*L2+A*(L1+L0*A)))
            __ASM_EMIT("addps       0x90 + %[FLOG], %%xmm5")        // xmm5 = L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A)))
            __ASM_EMIT("mulps       %%xmm3, %%xmm5")                // xmm5 = A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))
            __ASM_EMIT("addps       0xa0 + %[FLOG], %%xmm5")        // xmm5 = L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))
            __ASM_EMIT("mulps       %%xmm3, %%xmm5")                // xmm5 = A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A)))))
            __ASM_EMIT("addps       0xb0 + %[FLOG], %%xmm5")        // xmm5 = L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A)))))
            __ASM_EMIT("mulps       %%xmm3, %%xmm5")                // xmm5 = A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))))
            __ASM_EMIT("addps       0xc0 + %[FLOG], %%xmm5")        // xmm5 = L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))))
            __ASM_EMIT("mulps       %%xmm3, %%xmm5")                // xmm5 = A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A)))))))
            __ASM_EMIT("addps       0xd0 + %[FLOG], %%xmm5")        // xmm5 = A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))))) + L9
            __ASM_EMIT("mulps       %%xmm3, %%xmm5")                // xmm5 = A*(A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))))) + L9)
            __ASM_EMIT("mulps       %%xmm3, %%xmm5")                // xmm5 = A*A*(A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))))) + L9)
            __ASM_EMIT("mulps       0xe0 + %[FLOG], %%xmm4")        // xmm4 = B*(LXE + LN2)
            __ASM_EMIT("addps       %%xmm4, %%xmm5")                // xmm5 = B*(LXE + LN2) + A*A*A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A)))))))
            __ASM_EMIT("addps       %%xmm3, %%xmm5")                // xmm5 = B*(LXE + LN2) + A*A*A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))))) + A
            // Apply values to axes
            __ASM_EMIT("movaps      %%xmm5, %%xmm3")                // xmm3 = log(abs(v*zero)), xmm5=log(abs(v*zero))
            __ASM_EMIT("movups      (%[x]), %%xmm4")                // xmm4 = x
            __ASM_EMIT("mulps       %%xmm1, %%xmm3")                // xmm3 = log(abs(v*zero)) * norm_x
            __ASM_EMIT("addps       %%xmm3, %%xmm4")                // xmm4 = x + log(abs(v*zero)) * norm_x
            __ASM_EMIT("movups      %%xmm4, (%[x])")
            __ASM_EMIT("add         $0x10, %[x]")
            __ASM_EMIT("sub         $4, %[count]")
            __ASM_EMIT("jae         1b")

            __ASM_EMIT("2:")
            __ASM_EMIT("add         $3, %[count]")
            __ASM_EMIT("jl          4f")

            // Do x1 blocks
            __ASM_EMIT("3:")
            __ASM_EMIT("movss       (%[v]), %%xmm3")                // xmm3 = v
            __ASM_EMIT("andps       0x00 + %[ILOG], %%xmm3")        // xmm3 = abs(v)
            __ASM_EMIT("add         $0x04, %[v]")                   // v   += 4
            __ASM_EMIT("maxps       0x00 + %[FLOG], %%xmm3")        // xmm3 = max(X_AMP, abs(v)), ignores denormalized values
            __ASM_EMIT("mulps       %%xmm0, %%xmm3")                // xmm5 = max(X_AMP, abs(v)) * zero
            // Step 2: parse float value
            __ASM_EMIT("movdqa      %%xmm3, %%xmm4")                // xmm4 = v
            __ASM_EMIT("psrld       $23, %%xmm4")                   // xmm4 = frac(v)
            __ASM_EMIT("andps       0x10 + %[ILOG], %%xmm3")        // xmm3 = mant(v)
            __ASM_EMIT("psubd       0x20 + %[ILOG], %%xmm4")        // xmm4 = frac(v) - 127
            __ASM_EMIT("orps        0x10 + %[FLOG], %%xmm3")        // xmm3 = V = mant(v)+0.5
            __ASM_EMIT("cvtdq2ps    %%xmm4, %%xmm4")                // xmm4 = E = float(frac(v)-127)
            // Prepare logarithm approximation calculations
            __ASM_EMIT("movaps      %%xmm3, %%xmm5")                // xmm5 = V
            __ASM_EMIT("movaps      %%xmm3, %%xmm6")                // xmm6 = V
            __ASM_EMIT("cmpltps     0x20 + %[FLOG], %%xmm5")        // xmm5 = [ V < sqrt(1/2) ]
            __ASM_EMIT("movaps      0x30 + %[FLOG], %%xmm7")        // xmm7 = 1.0
            __ASM_EMIT("andps       %%xmm5, %%xmm6")                // xmm6 = V * [ V < sqrt(1/2) ]
            __ASM_EMIT("addps       %%xmm6, %%xmm3")                // xmm6 = V + V * [ V < sqrt(1/2) ]
            __ASM_EMIT("andnps      %%xmm7, %%xmm5")                // xmm5 = 1.0 * [ V >= sqrt(1/2) ]
            __ASM_EMIT("addps       %%xmm5, %%xmm4")                // xmm4 = E + 1.0 * [ V >= sqrt(1/2) ]      = B
            __ASM_EMIT("subps       %%xmm7, %%xmm3")                // xmm3 = V + V * [ V < sqrt(1/2) ] - 1.0   = A
            // Calculate logarithmic values
            __ASM_EMIT("movaps      0x40 + %[FLOG], %%xmm5")        // xmm5 = L0
            __ASM_EMIT("mulss       %%xmm3, %%xmm5")                // xmm5 = L0*A
            __ASM_EMIT("addss       0x50 + %[FLOG], %%xmm5")        // xmm5 = L1+L0*A
            __ASM_EMIT("mulss       %%xmm3, %%xmm5")                // xmm5 = A*(L1+L0*A)
            __ASM_EMIT("addss       0x60 + %[FLOG], %%xmm5")        // xmm5 = L2+A*(L1+L0*A)
            __ASM_EMIT("mulss       %%xmm3, %%xmm5")                // xmm5 = A*(L2+A*(L1+L0*A))
            __ASM_EMIT("addss       0x70 + %[FLOG], %%xmm5")        // xmm5 = L3+A*L2+A*(L1+L0*A)
            __ASM_EMIT("mulss       %%xmm3, %%xmm5")                // xmm5 = A*(L3+A*L2+A*(L1+L0*A))
            __ASM_EMIT("addss       0x80 + %[FLOG], %%xmm5")        // xmm5 = L4+A*(L3+A*L2+A*(L1+L0*A))
            __ASM_EMIT("mulss       %%xmm3, %%xmm5")                // xmm5 = A*(L4+A*(L3+A*L2+A*(L1+L0*A)))
            __ASM_EMIT("addss       0x90 + %[FLOG], %%xmm5")        // xmm5 = L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A)))
            __ASM_EMIT("mulss       %%xmm3, %%xmm5")                // xmm5 = A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))
            __ASM_EMIT("addss       0xa0 + %[FLOG], %%xmm5")        // xmm5 = L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))
            __ASM_EMIT("mulss       %%xmm3, %%xmm5")                // xmm5 = A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A)))))
            __ASM_EMIT("addss       0xb0 + %[FLOG], %%xmm5")        // xmm5 = L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A)))))
            __ASM_EMIT("mulss       %%xmm3, %%xmm5")                // xmm5 = A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))))
            __ASM_EMIT("addss       0xc0 + %[FLOG], %%xmm5")        // xmm5 = L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))))
            __ASM_EMIT("mulss       %%xmm3, %%xmm5")                // xmm5 = A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A)))))))
            __ASM_EMIT("addss       0xd0 + %[FLOG], %%xmm5")        // xmm5 = A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))))) + L9
            __ASM_EMIT("mulss       %%xmm3, %%xmm5")                // xmm5 = A*(A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))))) + L9)
            __ASM_EMIT("mulss       %%xmm3, %%xmm5")                // xmm5 = A*A*(A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))))) + L9)
            __ASM_EMIT("mulss       0xe0 + %[FLOG], %%xmm4")        // xmm4 = B*(LXE + LN2)
            __ASM_EMIT("addss       %%xmm4, %%xmm5")                // xmm5 = B*(LXE + LN2) + A*A*A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A)))))))
            __ASM_EMIT("addss       %%xmm3, %%xmm5")                // xmm5 = B*(LXE + LN2) + A*A*A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))))) + A
            // Apply values to axes
            __ASM_EMIT("movaps      %%xmm5, %%xmm3")                // xmm3 = log(abs(v*zero)), xmm5=log(abs(v*zero))
            __ASM_EMIT("movss       (%[x]), %%xmm4")                // xmm4 = x
            __ASM_EMIT("mulps       %%xmm1, %%xmm3")                // xmm3 = log(abs(v*zero)) * norm_x
            __ASM_EMIT("addps       %%xmm3, %%xmm4")                // xmm4 = x + log(abs(v*zero)) * norm_x
            __ASM_EMIT("movss       %%xmm4, (%[x])")
            __ASM_EMIT("add         $0x04, %[x]")
            __ASM_EMIT("dec         %[count]")
            __ASM_EMIT("jge         3b")

            __ASM_EMIT("4:")

            : [v] "+r" (v), [x] "+r" (x), [count] "+r" (count)
            : [zero] "m" (zero), [norm_x] "m" (norm_x),
              [ILOG] "o" (LOG_IARGS), [FLOG] "o" (LOG_FARGS)
            : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void axis_apply_log2(float *x, float *y, const float *v, float zero, float norm_x, float norm_y, size_t count)
    {
        //---------------------------------------------------------------
        // Prepare constants
        ARCH_X86_ASM
        (
            __ASM_EMIT("movss       %[zero], %%xmm0")
            __ASM_EMIT("movss       %[norm_x], %%xmm1")
            __ASM_EMIT("movss       %[norm_y], %%xmm2")

            __ASM_EMIT("sub         $4, %[count]")
            __ASM_EMIT("jb          2f")
            __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0")         // xmm0 == zero
            __ASM_EMIT("shufps      $0x00, %%xmm1, %%xmm1")         // xmm1 == norm_x
            __ASM_EMIT("shufps      $0x00, %%xmm2, %%xmm2")         // xmm2 == norm_y

            // Do x4 blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("movups      (%[v]), %%xmm3")                // xmm3 = v
            __ASM_EMIT("andps       0x00 + %[ILOG], %%xmm3")        // xmm3 = abs(v)
            __ASM_EMIT("add         $0x10, %[v]")                   // v   += 4
            __ASM_EMIT("maxps       0x00 + %[FLOG], %%xmm3")        // xmm3 = max(X_AMP, abs(v)), ignores denormalized values
            __ASM_EMIT("mulps       %%xmm0, %%xmm3")                // xmm3 = max(X_AMP, abs(v)) * zero
            // Step 2: parse float value
            __ASM_EMIT("movdqa      %%xmm3, %%xmm4")                // xmm4 = v
            __ASM_EMIT("psrld       $23, %%xmm4")                   // xmm4 = frac(v)
            __ASM_EMIT("andps       0x10 + %[ILOG], %%xmm3")        // xmm3 = mant(v)
            __ASM_EMIT("psubd       0x20 + %[ILOG], %%xmm4")        // xmm4 = frac(v) - 127
            __ASM_EMIT("orps        0x10 + %[FLOG], %%xmm3")        // xmm3 = V = mant(v)+0.5
            __ASM_EMIT("cvtdq2ps    %%xmm4, %%xmm4")                // xmm4 = E = float(frac(v)-127)
            // Prepare logarithm approximation calculations
            __ASM_EMIT("movaps      %%xmm3, %%xmm5")                // xmm5 = V
            __ASM_EMIT("movaps      %%xmm3, %%xmm6")                // xmm6 = V
            __ASM_EMIT("cmpltps     0x20 + %[FLOG], %%xmm5")        // xmm5 = [ V < sqrt(1/2) ]
            __ASM_EMIT("movaps      0x30 + %[FLOG], %%xmm7")        // xmm7 = 1.0
            __ASM_EMIT("andps       %%xmm5, %%xmm6")                // xmm6 = V * [ V < sqrt(1/2) ]
            __ASM_EMIT("addps       %%xmm6, %%xmm3")                // xmm3 = V + V * [ V < sqrt(1/2) ]
            __ASM_EMIT("andnps      %%xmm7, %%xmm5")                // xmm5 = 1.0 * [ V >= sqrt(1/2) ]
            __ASM_EMIT("addps       %%xmm5, %%xmm4")                // xmm4 = E + 1.0 * [ V >= sqrt(1/2) ]      = B
            __ASM_EMIT("subps       %%xmm7, %%xmm3")                // xmm3 = V + V * [ V < sqrt(1/2) ] - 1.0   = A
            // Calculate logarithmic values
            __ASM_EMIT("movaps      0x40 + %[FLOG], %%xmm5")        // xmm5 = L0
            __ASM_EMIT("mulps       %%xmm3, %%xmm5")                // xmm5 = L0*A
            __ASM_EMIT("addps       0x50 + %[FLOG], %%xmm5")        // xmm5 = L1+L0*A
            __ASM_EMIT("mulps       %%xmm3, %%xmm5")                // xmm5 = A*(L1+L0*A)
            __ASM_EMIT("addps       0x60 + %[FLOG], %%xmm5")        // xmm5 = L2+A*(L1+L0*A)
            __ASM_EMIT("mulps       %%xmm3, %%xmm5")                // xmm5 = A*(L2+A*(L1+L0*A))
            __ASM_EMIT("addps       0x70 + %[FLOG], %%xmm5")        // xmm5 = L3+A*L2+A*(L1+L0*A)
            __ASM_EMIT("mulps       %%xmm3, %%xmm5")                // xmm5 = A*(L3+A*L2+A*(L1+L0*A))
            __ASM_EMIT("addps       0x80 + %[FLOG], %%xmm5")        // xmm5 = L4+A*(L3+A*L2+A*(L1+L0*A))
            __ASM_EMIT("mulps       %%xmm3, %%xmm5")                // xmm5 = A*(L4+A*(L3+A*L2+A*(L1+L0*A)))
            __ASM_EMIT("addps       0x90 + %[FLOG], %%xmm5")        // xmm5 = L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A)))
            __ASM_EMIT("mulps       %%xmm3, %%xmm5")                // xmm5 = A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))
            __ASM_EMIT("addps       0xa0 + %[FLOG], %%xmm5")        // xmm5 = L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))
            __ASM_EMIT("mulps       %%xmm3, %%xmm5")                // xmm5 = A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A)))))
            __ASM_EMIT("addps       0xb0 + %[FLOG], %%xmm5")        // xmm5 = L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A)))))
            __ASM_EMIT("mulps       %%xmm3, %%xmm5")                // xmm5 = A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))))
            __ASM_EMIT("addps       0xc0 + %[FLOG], %%xmm5")        // xmm5 = L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))))
            __ASM_EMIT("mulps       %%xmm3, %%xmm5")                // xmm5 = A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A)))))))
            __ASM_EMIT("addps       0xd0 + %[FLOG], %%xmm5")        // xmm5 = A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))))) + L9
            __ASM_EMIT("mulps       %%xmm3, %%xmm5")                // xmm5 = A*(A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))))) + L9)
            __ASM_EMIT("mulps       %%xmm3, %%xmm5")                // xmm5 = A*A*(A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))))) + L9)
            __ASM_EMIT("mulps       0xe0 + %[FLOG], %%xmm4")        // xmm4 = B*(LXE + LN2)
            __ASM_EMIT("addps       %%xmm4, %%xmm5")                // xmm5 = B*(LXE + LN2) + A*A*A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A)))))))
            __ASM_EMIT("addps       %%xmm3, %%xmm5")                // xmm5 = B*(LXE + LN2) + A*A*A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))))) + A
            // Apply values to axes
            __ASM_EMIT("movaps      %%xmm5, %%xmm3")                // xmm3 = log(abs(v*zero)), xmm5=log(abs(v*zero))
            __ASM_EMIT("movups      (%[x]), %%xmm4")                // xmm4 = x
            __ASM_EMIT("movups      (%[y]), %%xmm6")                // xmm6 = y
            __ASM_EMIT("mulps       %%xmm1, %%xmm3")                // xmm3 = log(abs(v*zero)) * norm_x
            __ASM_EMIT("mulps       %%xmm2, %%xmm5")                // xmm5 = log(abs(v*zero)) * norm_y
            __ASM_EMIT("addps       %%xmm3, %%xmm4")                // xmm4 = x + log(abs(v*zero)) * norm_x
            __ASM_EMIT("addps       %%xmm5, %%xmm6")                // xmm6 = y + log(abs(v*zero)) * norm_y
            __ASM_EMIT("movups      %%xmm4, (%[x])")
            __ASM_EMIT("movups      %%xmm6, (%[y])")
            __ASM_EMIT("add         $0x10, %[x]")
            __ASM_EMIT("add         $0x10, %[y]")
            __ASM_EMIT("sub         $4, %[count]")
            __ASM_EMIT("jae         1b")

            __ASM_EMIT("2:")
            __ASM_EMIT("add         $3, %[count]")
            __ASM_EMIT("jl          4f")

            // Do x1 blocks
            __ASM_EMIT("3:")
            __ASM_EMIT("movss       (%[v]), %%xmm3")                // xmm3 = v
            __ASM_EMIT("andps       0x00 + %[ILOG], %%xmm3")        // xmm3 = abs(v)
            __ASM_EMIT("add         $0x04, %[v]")                   // v   += 4
            __ASM_EMIT("maxps       0x00 + %[FLOG], %%xmm3")        // xmm3 = max(X_AMP, abs(v)), ignores denormalized values
            __ASM_EMIT("mulps       %%xmm0, %%xmm3")                // xmm5 = max(X_AMP, abs(v)) * zero
            // Step 2: parse float value
            __ASM_EMIT("movdqa      %%xmm3, %%xmm4")                // xmm4 = v
            __ASM_EMIT("psrld       $23, %%xmm4")                   // xmm4 = frac(v)
            __ASM_EMIT("andps       0x10 + %[ILOG], %%xmm3")        // xmm3 = mant(v)
            __ASM_EMIT("psubd       0x20 + %[ILOG], %%xmm4")        // xmm4 = frac(v) - 127
            __ASM_EMIT("orps        0x10 + %[FLOG], %%xmm3")        // xmm3 = V = mant(v)+0.5
            __ASM_EMIT("cvtdq2ps    %%xmm4, %%xmm4")                // xmm4 = E = float(frac(v)-127)
            // Prepare logarithm approximation calculations
            __ASM_EMIT("movaps      %%xmm3, %%xmm5")                // xmm5 = V
            __ASM_EMIT("movaps      %%xmm3, %%xmm6")                // xmm6 = V
            __ASM_EMIT("cmpltps     0x20 + %[FLOG], %%xmm5")        // xmm5 = [ V < sqrt(1/2) ]
            __ASM_EMIT("movaps      0x30 + %[FLOG], %%xmm7")        // xmm7 = 1.0
            __ASM_EMIT("andps       %%xmm5, %%xmm6")                // xmm6 = V * [ V < sqrt(1/2) ]
            __ASM_EMIT("addps       %%xmm6, %%xmm3")                // xmm6 = V + V * [ V < sqrt(1/2) ]
            __ASM_EMIT("andnps      %%xmm7, %%xmm5")                // xmm5 = 1.0 * [ V >= sqrt(1/2) ]
            __ASM_EMIT("addps       %%xmm5, %%xmm4")                // xmm4 = E + 1.0 * [ V >= sqrt(1/2) ]      = B
            __ASM_EMIT("subps       %%xmm7, %%xmm3")                // xmm3 = V + V * [ V < sqrt(1/2) ] - 1.0   = A
            // Calculate logarithmic values
            __ASM_EMIT("movaps      0x40 + %[FLOG], %%xmm5")        // xmm5 = L0
            __ASM_EMIT("mulss       %%xmm3, %%xmm5")                // xmm5 = L0*A
            __ASM_EMIT("addss       0x50 + %[FLOG], %%xmm5")        // xmm5 = L1+L0*A
            __ASM_EMIT("mulss       %%xmm3, %%xmm5")                // xmm5 = A*(L1+L0*A)
            __ASM_EMIT("addss       0x60 + %[FLOG], %%xmm5")        // xmm5 = L2+A*(L1+L0*A)
            __ASM_EMIT("mulss       %%xmm3, %%xmm5")                // xmm5 = A*(L2+A*(L1+L0*A))
            __ASM_EMIT("addss       0x70 + %[FLOG], %%xmm5")        // xmm5 = L3+A*L2+A*(L1+L0*A)
            __ASM_EMIT("mulss       %%xmm3, %%xmm5")                // xmm5 = A*(L3+A*L2+A*(L1+L0*A))
            __ASM_EMIT("addss       0x80 + %[FLOG], %%xmm5")        // xmm5 = L4+A*(L3+A*L2+A*(L1+L0*A))
            __ASM_EMIT("mulss       %%xmm3, %%xmm5")                // xmm5 = A*(L4+A*(L3+A*L2+A*(L1+L0*A)))
            __ASM_EMIT("addss       0x90 + %[FLOG], %%xmm5")        // xmm5 = L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A)))
            __ASM_EMIT("mulss       %%xmm3, %%xmm5")                // xmm5 = A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))
            __ASM_EMIT("addss       0xa0 + %[FLOG], %%xmm5")        // xmm5 = L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))
            __ASM_EMIT("mulss       %%xmm3, %%xmm5")                // xmm5 = A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A)))))
            __ASM_EMIT("addss       0xb0 + %[FLOG], %%xmm5")        // xmm5 = L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A)))))
            __ASM_EMIT("mulss       %%xmm3, %%xmm5")                // xmm5 = A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))))
            __ASM_EMIT("addss       0xc0 + %[FLOG], %%xmm5")        // xmm5 = L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))))
            __ASM_EMIT("mulss       %%xmm3, %%xmm5")                // xmm5 = A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A)))))))
            __ASM_EMIT("addss       0xd0 + %[FLOG], %%xmm5")        // xmm5 = A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))))) + L9
            __ASM_EMIT("mulss       %%xmm3, %%xmm5")                // xmm5 = A*(A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))))) + L9)
            __ASM_EMIT("mulss       %%xmm3, %%xmm5")                // xmm5 = A*A*(A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))))) + L9)
            __ASM_EMIT("mulss       0xe0 + %[FLOG], %%xmm4")        // xmm4 = B*(LXE + LN2)
            __ASM_EMIT("addss       %%xmm4, %%xmm5")                // xmm5 = B*(LXE + LN2) + A*A*A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A)))))))
            __ASM_EMIT("addss       %%xmm3, %%xmm5")                // xmm5 = B*(LXE + LN2) + A*A*A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))))) + A
            // Apply values to axes
            __ASM_EMIT("movaps      %%xmm5, %%xmm3")                // xmm3 = log(abs(v*zero)), xmm5=log(abs(v*zero))
            __ASM_EMIT("movss       (%[x]), %%xmm4")                // xmm4 = x
            __ASM_EMIT("movss       (%[y]), %%xmm6")                // xmm6 = y
            __ASM_EMIT("mulps       %%xmm1, %%xmm3")                // xmm3 = log(abs(v*zero)) * norm_x
            __ASM_EMIT("mulps       %%xmm2, %%xmm5")                // xmm5 = log(abs(v*zero)) * norm_y
            __ASM_EMIT("addps       %%xmm3, %%xmm4")                // xmm4 = x + log(abs(v*zero)) * norm_x
            __ASM_EMIT("addps       %%xmm5, %%xmm6")                // xmm6 = y + log(abs(v*zero)) * norm_y
            __ASM_EMIT("movss       %%xmm4, (%[x])")
            __ASM_EMIT("movss       %%xmm6, (%[y])")
            __ASM_EMIT("add         $0x04, %[x]")
            __ASM_EMIT("add         $0x04, %[y]")
            __ASM_EMIT("dec         %[count]")
            __ASM_EMIT("jge         3b")

            __ASM_EMIT("4:")

            : [v] "+r" (v), [x] "+r" (x) , [y] "+r" (y),
              [count] "+r" (count)
            : [zero] "m" (zero), [norm_x] "m" (norm_x), [norm_y] "m" (norm_y),
              [ILOG] "o" (LOG_IARGS), [FLOG] "o" (LOG_FARGS)
            : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }


    void rgba32_to_bgra32(void *dst, const void *src, size_t count)
    {
        size_t off;

        ARCH_X86_ASM
        (
            __ASM_EMIT("movdqa      %[MASK], %%xmm6")                   // xmm6 = 00 ff 00 ff
            __ASM_EMIT("xor         %[off], %[off]")                    // off  = 0
            __ASM_EMIT("movdqa      %%xmm6, %%xmm7")                    // xmm7 = 00 ff 00 ff
            __ASM_EMIT("pslld       $8, %%xmm6")                        // xmm6 = ff 00 ff 00

            // 8-element blocks
            __ASM_EMIT("sub         $8, %[count]")
            __ASM_EMIT("jb          2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("movdqu      0x00(%[src], %[off]), %%xmm0")      // xmm0 = A1 R1 G1 B1
            __ASM_EMIT("movdqu      0x10(%[src], %[off]), %%xmm1")      // xmm1 = A2 R2 G2 B2
            __ASM_EMIT("movdqa      %%xmm0, %%xmm2")                    // xmm2 = A1 R1 G1 B1
            __ASM_EMIT("movdqa      %%xmm1, %%xmm3")                    // xmm3 = A2 R2 G2 B2
            __ASM_EMIT("pand        %%xmm7, %%xmm0")                    // xmm0 = 00 R1 00 B1
            __ASM_EMIT("pand        %%xmm6, %%xmm2")                    // xmm2 = A1 00 G1 00
            __ASM_EMIT("pand        %%xmm7, %%xmm1")                    // xmm1 = 00 R2 00 B2
            __ASM_EMIT("pand        %%xmm6, %%xmm3")                    // xmm3 = A2 00 G2 00
            __ASM_EMIT("movdqa      %%xmm0, %%xmm4")                    // xmm4 = A1 00 G1 00
            __ASM_EMIT("movdqa      %%xmm1, %%xmm5")                    // xmm5 = A2 00 G2 00
            __ASM_EMIT("pslld       $16, %%xmm0")                       // xmm0 = 00 B1 00 00
            __ASM_EMIT("pslld       $16, %%xmm1")                       // xmm1 = 00 B2 00 00
            __ASM_EMIT("psrld       $16, %%xmm4")                       // xmm4 = 00 00 00 R1
            __ASM_EMIT("psrld       $16, %%xmm5")                       // xmm5 = 00 00 00 R2
            __ASM_EMIT("orpd        %%xmm2, %%xmm0")                    // xmm0 = A1 B1 G1 00
            __ASM_EMIT("orpd        %%xmm3, %%xmm1")                    // xmm1 = A2 B2 G2 00
            __ASM_EMIT("orpd        %%xmm4, %%xmm0")                    // xmm0 = A1 B1 G1 R1
            __ASM_EMIT("orpd        %%xmm5, %%xmm1")                    // xmm1 = A2 B2 G2 R2
            __ASM_EMIT("movdqu      %%xmm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("movdqu      %%xmm1, 0x10(%[dst], %[off])")
            __ASM_EMIT("add         $0x20, %[off]")
            __ASM_EMIT("sub         $8, %[count]")
            __ASM_EMIT("jae         1b")

            // 4-element block
            __ASM_EMIT("2:")
            __ASM_EMIT("add         $4, %[count]")
            __ASM_EMIT("jl          4f")
            __ASM_EMIT("movdqu      0x00(%[src], %[off]), %%xmm0")      // xmm0 = A1 R1 G1 B1
            __ASM_EMIT("movdqa      %%xmm0, %%xmm2")                    // xmm2 = A1 R1 G1 B1
            __ASM_EMIT("pand        %%xmm7, %%xmm0")                    // xmm0 = 00 R1 00 B1
            __ASM_EMIT("pand        %%xmm6, %%xmm2")                    // xmm2 = A1 00 G1 00
            __ASM_EMIT("movdqa      %%xmm0, %%xmm4")                    // xmm4 = A1 00 G1 00
            __ASM_EMIT("pslld       $16, %%xmm0")                       // xmm0 = 00 B1 00 00
            __ASM_EMIT("psrld       $16, %%xmm4")                       // xmm4 = 00 00 00 R1
            __ASM_EMIT("orpd        %%xmm2, %%xmm0")                    // xmm0 = A1 B1 G1 00
            __ASM_EMIT("orpd        %%xmm4, %%xmm0")                    // xmm0 = A1 B1 G1 R1
            __ASM_EMIT("movdqu      %%xmm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("add         $0x10, %[off]")
            __ASM_EMIT("sub         $4, %[count]")

            // Tail
            __ASM_EMIT("4:")
            __ASM_EMIT("add         $3, %[count]")
            __ASM_EMIT("jl          6f")
            __ASM_EMIT("5:")
            __ASM_EMIT("movd        0x00(%[src], %[off]), %%xmm0")      // xmm0 = AA RR GG BB
            __ASM_EMIT("movdqa      %%xmm0, %%xmm1")                    // xmm1 = AA RR GG BB
            __ASM_EMIT("pand        %%xmm7, %%xmm0")                    // xmm0 = 00 RR 00 BB
            __ASM_EMIT("pand        %%xmm6, %%xmm1")                    // xmm1 = AA 00 GG 00
            __ASM_EMIT("movdqa      %%xmm0, %%xmm2")                    // xmm2 = 00 RR 00 BB
            __ASM_EMIT("pslld       $16, %%xmm0")                       // xmm0 = 00 BB 00 00
            __ASM_EMIT("psrld       $16, %%xmm2")                       // xmm2 = 00 00 00 RR
            __ASM_EMIT("orpd        %%xmm1, %%xmm0")                    // xmm0 = AA 00 GG RR
            __ASM_EMIT("orpd        %%xmm2, %%xmm0")                    // xmm0 = AA BB GG RR
            __ASM_EMIT("movd        %%xmm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("add         $4, %[off]")
            __ASM_EMIT("dec         %[count]")
            __ASM_EMIT("jge         5b")

            // End
            __ASM_EMIT("6:")

            : [dst] "+r"(dst), [src] "+r"(src), [count] "+r" (count),
              [off] "=&r" (off)
            : [MASK] "m" (X_CMASK)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

#define FILL4_CORE \
    __ASM_EMIT("movss       %[c0], %%xmm0")     /* xmm0 = c0 */ \
    __ASM_EMIT("movss       %[c1], %%xmm1")     /* xmm1 = c1 */ \
    __ASM_EMIT("movss       %[c2], %%xmm2")     /* xmm2 = c2 */ \
    __ASM_EMIT("movss       %[c3], %%xmm3")     /* xmm3 = c3 */ \
    __ASM_EMIT("unpcklps    %%xmm1, %%xmm0")    /* xmm0 = c0 c1 ? ? */ \
    __ASM_EMIT("unpcklps    %%xmm3, %%xmm2")    /* xmm2 = c2 c3 ? ? */ \
    __ASM_EMIT("movlhps     %%xmm2, %%xmm0")    /* xmm0 = c0 c1 c2 c3 */ \
    __ASM_EMIT("movaps      %%xmm0, %%xmm1")    /* xmm1 = c0 c1 c2 c3 */ \
    __ASM_EMIT("movaps      %%xmm0, %%xmm2") \
    __ASM_EMIT("movaps      %%xmm1, %%xmm3") \
    \
    /* 8x blocks */ \
    __ASM_EMIT("sub         $8, %[count]") \
    __ASM_EMIT("jb          2f") \
    __ASM_EMIT("movaps      %%xmm0, %%xmm4") \
    __ASM_EMIT("movaps      %%xmm1, %%xmm5") \
    __ASM_EMIT("movaps      %%xmm0, %%xmm6") \
    __ASM_EMIT("movaps      %%xmm1, %%xmm7") \
    __ASM_EMIT("1:") \
    __ASM_EMIT("movups      %%xmm0, 0x00(%[dst])") \
    __ASM_EMIT("movups      %%xmm1, 0x10(%[dst])") \
    __ASM_EMIT("movups      %%xmm2, 0x20(%[dst])") \
    __ASM_EMIT("movups      %%xmm3, 0x30(%[dst])") \
    __ASM_EMIT("movups      %%xmm4, 0x40(%[dst])") \
    __ASM_EMIT("movups      %%xmm5, 0x50(%[dst])") \
    __ASM_EMIT("movups      %%xmm6, 0x60(%[dst])") \
    __ASM_EMIT("movups      %%xmm7, 0x70(%[dst])") \
    __ASM_EMIT("add         $0x80, %[dst]") \
    __ASM_EMIT("sub         $8, %[count]") \
    __ASM_EMIT("jae         1b") \
    /* 4x block */ \
    __ASM_EMIT("2:") \
    __ASM_EMIT("add         $4, %[count]") \
    __ASM_EMIT("jl          4f") \
    __ASM_EMIT("movups      %%xmm0, 0x00(%[dst])") \
    __ASM_EMIT("movups      %%xmm1, 0x10(%[dst])") \
    __ASM_EMIT("movups      %%xmm2, 0x20(%[dst])") \
    __ASM_EMIT("movups      %%xmm3, 0x30(%[dst])") \
    __ASM_EMIT("sub         $4, %[count]") \
    __ASM_EMIT("add         $0x40, %[dst]") \
    /* 2x block */ \
    __ASM_EMIT("4:") \
    __ASM_EMIT("add         $2, %[count]") \
    __ASM_EMIT("jl          6f") \
    __ASM_EMIT("movups      %%xmm0, 0x00(%[dst])") \
    __ASM_EMIT("movups      %%xmm1, 0x10(%[dst])") \
    __ASM_EMIT("sub         $2, %[count]") \
    __ASM_EMIT("add         $0x20, %[dst]") \
    /* 1x block */ \
    __ASM_EMIT("6:") \
    __ASM_EMIT("add         $1, %[count]") \
    __ASM_EMIT("jl          8f") \
    __ASM_EMIT("movups      %%xmm0, 0x00(%[dst])") \
    __ASM_EMIT("8:")

    void fill_rgba(float *dst, float r, float g, float b, float a, size_t count)
    {
        ARCH_X86_ASM
        (
            FILL4_CORE
            : [dst] "+r" (dst), [count] "+r" (count)
            : [c0] "m" (r), [c1] "m" (g), [c2] "m" (b), [c3] "m" (a)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void fill_hsla(float *dst, float h, float s, float l, float a, size_t count)
    {
        ARCH_X86_ASM
        (
            FILL4_CORE
            : [dst] "+r" (dst), [count] "+r" (count)
            : [c0] "m" (h), [c1] "m" (s), [c2] "m" (l), [c3] "m" (a)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

#undef FILL4_CORE

}

#endif /* DSP_ARCH_X86_SSE_GRAPHICS_H_ */
