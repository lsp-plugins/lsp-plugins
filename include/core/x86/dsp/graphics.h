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

#ifndef CORE_X86_DSP_GRAPHICS_H_
#define CORE_X86_DSP_GRAPHICS_H_

namespace lsp
{
    namespace sse
    {
        void axis_apply_log(float *x, float *y, const float *v, float zero, float norm_x, float norm_y, size_t count)
        {
            // Step 1: load vector, take absolute value and limit it by minimum value
            #define LOG_LOAD(mv_v, d)   \
                __asm__ __volatile__ \
                ( \
                    __ASM_EMIT(mv_v "       (%[v]), %%xmm3")        /* xmm3 = v */ \
                    __ASM_EMIT("andps       %[X_SIGN], %%xmm3")     /* xmm3 = abs(v) */ \
                    __ASM_EMIT("maxps       %[X_AMP], %%xmm3")      /* xmm3 = max(X_AMP, abs(v)), ignores denormalized values */ \
                    __ASM_EMIT("mulps       %%xmm0, %%xmm3")        /* xmm5 = max(X_AMP, abs(v))*zero */ \
                    __ASM_EMIT("add         %[delta],%[v]") \
                    : [v] "+r" (v) \
                    : [delta]       "i" (d), \
                      [X_SIGN]      "m" (X_SIGN), \
                      [X_AMP]       "m" (X_AMP_THRESH) \
                    : "cc", "%xmm3" \
                )

            // Step 2: parse float value
            #define LOG_STEP1       \
                __asm__ __volatile__ \
                ( \
                    __ASM_EMIT("movapd      %%xmm3, %%xmm4")        /* xmm4 = v */ \
                    __ASM_EMIT("psrld       $23, %%xmm4")           /* xmm4 = frac(v) */ \
                    __ASM_EMIT("andps       %[X_MANT], %%xmm3")     /* xmm3 = mant(v) */ \
                    __ASM_EMIT("psubd       %[X_MMASK], %%xmm4")    /* xmm4 = frac(v) - 127 */ \
                    __ASM_EMIT("orps        %[X_HALF], %%xmm3")     /* xmm3 = mant(v)+0.5 = V */ \
                    __ASM_EMIT("cvtdq2ps    %%xmm4, %%xmm4")        /* xmm4 = float(frac(v)-127) = E */ \
                    : \
                    : \
                        [X_MANT]      "m" (X_MANT), \
                        [X_MMASK]     "m" (X_MMASK), \
                        [X_HALF]      "m" (X_HALF) \
                    : "%xmm4", "%xmm3", "%xmm5" \
                )

            // Step 3: prepare logarithm approximation calculations
            #define LOG_STEP2   \
                __asm__ __volatile__ \
                ( \
                    __ASM_EMIT("movaps      %%xmm3, %%xmm5")        /* xmm5 = V */ \
                    __ASM_EMIT("movaps      %%xmm3, %%xmm6")        /* xmm6 = V */ \
                    __ASM_EMIT("cmpltps     %[SQRT1_2], %%xmm5")    /* xmm5 = / V < sqrt(1/2) / */ \
                    __ASM_EMIT("movaps      %[ONE], %%xmm7")        /* xmm7 = 1.0 */ \
                    __ASM_EMIT("andps       %%xmm5, %%xmm6")        /* xmm6 = V * / V < sqrt(1/2) / */ \
                    __ASM_EMIT("addps       %%xmm6, %%xmm3")        /* xmm6 = V + V * / V < sqrt(1/2) / */ \
                    __ASM_EMIT("andnps      %%xmm7, %%xmm5")        /* xmm5 = 1.0 * / V >= sqrt(1/2) / */ \
                    __ASM_EMIT("addps       %%xmm5, %%xmm4")        /* xmm4 = E + 1.0 * / V >= sqrt(1/2) /      = B */ \
                    __ASM_EMIT("subps       %%xmm7, %%xmm3")        /* xmm3 = V + V * / V < sqrt(1/2) / - 1.0   = A */ \
                    : \
                    : \
                      [SQRT1_2]     "m" (SQRT1_2), \
                      [ONE]         "m" (ONE) \
                    : "%xmm3", "%xmm4", "%xmm5", "%xmm6", "%xmm7" \
                )

            // Step 4: calculate four logarithmic values
            #define LOG_STEP3   \
                __asm__ __volatile__ \
                ( \
                    __ASM_EMIT("movaps      %%xmm3, %%xmm7")        /* xmm5 = A  */ \
                    __ASM_EMIT("mulps       %%xmm7, %%xmm7")        /* xmm7 = A*A */ \
                    \
                    __ASM_EMIT("movaps      %[L0], %%xmm5")         /* xmm5 = L0 */ \
                    __ASM_EMIT("mulps       %%xmm3, %%xmm5")        /* xmm5 = L0*A */ \
                    __ASM_EMIT("movaps      %[L1], %%xmm6")         /* xmm6 = L1 */ \
                    __ASM_EMIT("addps       %%xmm6, %%xmm5")        /* xmm5 = L1+L0*A */ \
                    __ASM_EMIT("mulps       %%xmm3, %%xmm5")        /* xmm5 = A*(L1+L0*A) */ \
                    __ASM_EMIT("movaps      %[L2], %%xmm6")         /* xmm6 = L2 */ \
                    __ASM_EMIT("addps       %%xmm6, %%xmm5")        /* xmm5 = L2+A*(L1+L0*A) */ \
                    __ASM_EMIT("mulps       %%xmm3, %%xmm5")        /* xmm5 = A*(L2+A*(L1+L0*A)) */ \
                    __ASM_EMIT("movaps      %[L3], %%xmm6")         /* xmm6 = L3 */ \
                    __ASM_EMIT("addps       %%xmm6, %%xmm5")        /* xmm5 = L3+A*L2+A*(L1+L0*A) */ \
                    __ASM_EMIT("mulps       %%xmm3, %%xmm5")        /* xmm5 = A*(L3+A*L2+A*(L1+L0*A)) */ \
                    __ASM_EMIT("movaps      %[L4], %%xmm6")         /* xmm6 = L4 */ \
                    __ASM_EMIT("addps       %%xmm6, %%xmm5")        /* xmm5 = L4+A*(L3+A*L2+A*(L1+L0*A)) */ \
                    __ASM_EMIT("mulps       %%xmm3, %%xmm5")        /* xmm5 = A*(L4+A*(L3+A*L2+A*(L1+L0*A))) */ \
                    __ASM_EMIT("movaps      %[L5], %%xmm6")         /* xmm6 = L5 */ \
                    __ASM_EMIT("addps       %%xmm6, %%xmm5")        /* xmm5 = L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))) */ \
                    __ASM_EMIT("mulps       %%xmm3, %%xmm5")        /* xmm5 = A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A)))) */ \
                    __ASM_EMIT("movaps      %[L6], %%xmm6")         /* xmm6 = L6 */ \
                    __ASM_EMIT("addps       %%xmm6, %%xmm5")        /* xmm5 = L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A)))) */ \
                    __ASM_EMIT("mulps       %%xmm3, %%xmm5")        /* xmm5 = A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))) */ \
                    __ASM_EMIT("movaps      %[L7], %%xmm6")         /* xmm6 = L7 */ \
                    __ASM_EMIT("addps       %%xmm6, %%xmm5")        /* xmm5 = L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))) */ \
                    __ASM_EMIT("mulps       %%xmm3, %%xmm5")        /* xmm5 = A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A)))))) */ \
                    __ASM_EMIT("movaps      %[L8], %%xmm6")         /* xmm6 = L8 */ \
                    __ASM_EMIT("addps       %%xmm6, %%xmm5")        /* xmm5 = L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A)))))) */ \
                    __ASM_EMIT("mulps       %%xmm3, %%xmm5")        /* xmm5 = A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))))) */ \
                    __ASM_EMIT("mulps       %%xmm7, %%xmm5")        /* xmm5 = A*A*A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))))) */ \
                    \
                    __ASM_EMIT("movaps      %[LXE], %%xmm6")        /* xmm6 = LXE */ \
                    __ASM_EMIT("mulps       %%xmm4, %%xmm6")        /* xmm6 = B*LXE */ \
                    __ASM_EMIT("addps       %%xmm6, %%xmm5")        /* xmm5 = B*LXE+A*A*A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))))) */ \
                    \
                    __ASM_EMIT("movaps      %[L9], %%xmm6")         /* xmm6 = L9 */ \
                    __ASM_EMIT("mulps       %%xmm7, %%xmm6")        /* xmm6 = L9*A*A */ \
                    __ASM_EMIT("subps       %%xmm6, %%xmm5")        /* xmm5 = B*LXE+A*A*A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))))) - L9*A*A */ \
                    \
                    __ASM_EMIT("movaps      %[LN2], %%xmm6")        /* xmm6 = LN2 */ \
                    __ASM_EMIT("mulps       %%xmm4, %%xmm6")        /* xmm6 = B*LN2 */ \
                    __ASM_EMIT("addps       %%xmm3, %%xmm5")        /* xmm5 = B*LXE+A*A*A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))))) - L9*A*A + A */ \
                    __ASM_EMIT("addps       %%xmm6, %%xmm5")        /* xmm5 = B*LXE+A*A*A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))))) - L9*A*A + A + B*LN2 */ \
                    \
                    : \
                    : \
                      [L0]          "m" (L0), \
                      [L1]          "m" (L1), \
                      [L2]          "m" (L2), \
                      [L3]          "m" (L3), \
                      [L4]          "m" (L4), \
                      [L5]          "m" (L5), \
                      [L6]          "m" (L6), \
                      [L7]          "m" (L7), \
                      [L8]          "m" (L8), \
                      [L9]          "m" (L9), \
                      [LXE]         "m" (LXE), \
                      [LN2]         "m" (LN2) \
                    : "%xmm5", "%xmm6", "%xmm7" \
                )

            // Step 5: apply vector and store values
            #define LOG_STORE(mv_x, mv_y, d)   \
                __asm__ __volatile__ \
                ( \
                    __ASM_EMIT("movaps      %%xmm5, %%xmm3")        /* xmm3 = log(abs(v*zero)), xmm5=log(abs(v*zero)) */ \
                    __ASM_EMIT(mv_x "       (%[x]), %%xmm4")        /* xmm4 = x */ \
                    __ASM_EMIT(mv_y "       (%[y]), %%xmm6")        /* xmm6 = y */ \
                    __ASM_EMIT("mulps       %%xmm1, %%xmm3")        /* xmm3 = log(abs(v*zero)) * norm_x */ \
                    __ASM_EMIT("mulps       %%xmm2, %%xmm5")        /* xmm5 = log(abs(v*zero)) * norm_y */ \
                    __ASM_EMIT("addps       %%xmm3, %%xmm4")        /* xmm4 = x + log(abs(v*zero)) * norm_x */ \
                    __ASM_EMIT("addps       %%xmm5, %%xmm6")        /* xmm6 = y + log(abs(v*zero)) * norm_y */ \
                    __ASM_EMIT(mv_x "       %%xmm4, (%[x])") \
                    __ASM_EMIT(mv_y "       %%xmm6, (%[y])") \
                    __ASM_EMIT("add         %[delta], %[x]") \
                    __ASM_EMIT("add         %[delta], %[y]") \
                    : [x] "+r" (x), [y] "+r" (y) \
                    : [delta] "i" (d) \
                    : "%xmm3", "%xmm4", "%xmm6" \
                )

            #define LOG_CALC    \
                LOG_STEP1;      \
                LOG_STEP2;      \
                LOG_STEP3

            //---------------------------------------------------------------

            if (count <= 0)
                return;

            // Prepare constants
            __asm__ __volatile__
            (
                __ASM_EMIT("movaps      %[zero], %%xmm0")
                __ASM_EMIT("movaps      %[norm_x], %%xmm1")
                __ASM_EMIT("movaps      %[norm_y], %%xmm2")
                __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0") // xmm0 == zero
                __ASM_EMIT("shufps      $0x00, %%xmm1, %%xmm1") // xmm1 == norm_x
                __ASM_EMIT("shufps      $0x00, %%xmm2, %%xmm2") // xmm2 == norm_y
                :
                : [zero] "x" (zero), [norm_x] "x" (norm_x), [norm_y] "x" (norm_y)
                : "%xmm0", "%xmm1", "%xmm2"
            );

            // Ensure v is aligned
            while (!sse_aligned(v))
            {
                LOG_LOAD("movss", sizeof(float));
                LOG_CALC;
                LOG_STORE("movss", "movss", sizeof(float));

                if (!(--count))
                    return;
            }

            // Make main body
            if (sse_aligned(x))
            {
                if (sse_aligned(y))
                {
                    while (count >= SSE_MULTIPLE)
                    {
                        LOG_LOAD("movaps", SSE_ALIGN);
                        LOG_CALC;
                        LOG_STORE("movaps", "movaps", SSE_ALIGN);
                        count -= SSE_MULTIPLE;
                    }
                }
                else
                {
                    while (count >= SSE_MULTIPLE)
                    {
                        LOG_LOAD("movaps", SSE_ALIGN);
                        LOG_CALC;
                        LOG_STORE("movaps", "movups", SSE_ALIGN);
                        count -= SSE_MULTIPLE;
                    }
                }
            }
            else
            {
                if (sse_aligned(y))
                {
                    while (count >= SSE_MULTIPLE)
                    {
                        LOG_LOAD("movaps", SSE_ALIGN);
                        LOG_CALC;
                        LOG_STORE("movups", "movaps", SSE_ALIGN);
                        count -= SSE_MULTIPLE;
                    }
                }
                else
                {
                    while (count >= SSE_MULTIPLE)
                    {
                        LOG_LOAD("movaps", SSE_ALIGN);
                        LOG_CALC;
                        LOG_STORE("movups", "movups", SSE_ALIGN);
                        count -= SSE_MULTIPLE;
                    }
                }
            }

            // Complete the tail
            while (count > 0)
            {
                LOG_LOAD("movss", sizeof(float));
                LOG_CALC;
                LOG_STORE("movss", "movss", sizeof(float));
                --count;
            }

            #undef LOG_LOAD
            #undef LOG_STEP1
            #undef LOG_STEP2
            #undef LOG_STEP3
            #undef LOG_STORE
        }
    }
}

#endif /* CORE_X86_DSP_GRAPHICS_H_ */
