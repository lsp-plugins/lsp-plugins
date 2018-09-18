/*
 * graphics.h
 *
 *  Created on: 06 апр. 2018 г.
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

#ifndef DSP_ARCH_X86_SSE3_GRAPHICS_H_
#define DSP_ARCH_X86_SSE3_GRAPHICS_H_

#ifndef DSP_ARCH_X86_SSE3_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_SSE3_IMPL */

namespace sse3
{
    #ifdef ARCH_X86_64

    void x64_axis_apply_log(float *x, float *y, const float *v, float zero, float norm_x, float norm_y, size_t count)
    {
        // Step 1: load vector, take absolute value and limit it by minimum value
        #define LOG_LOAD(mv_v, d)   \
            ARCH_X86_ASM \
            ( \
                __ASM_EMIT(mv_v "       (%[v]), %%xmm3")        /* xmm3 = v */ \
                __ASM_EMIT("andps       %[X_SIGN], %%xmm3")     /* xmm3 = abs(v) */ \
                __ASM_EMIT("maxps       %[X_AMP], %%xmm3")      /* xmm3 = max(X_AMP, abs(v)), ignores denormalized values */ \
                __ASM_EMIT("mulps       %%xmm0, %%xmm3")        /* xmm3 = max(X_AMP, abs(v))*zero */ \
                __ASM_EMIT("add         %[delta],%[v]") \
                : [v] "+r" (v) \
                : [delta]       "i" (d), \
                  [X_SIGN]      "m" (X_SIGN), \
                  [X_AMP]       "m" (X_AMP_THRESH) \
                : "cc", "%xmm3" \
            )

        #define LOG_LOADx2(mv_v, d)  \
            ARCH_X86_ASM \
            ( \
                __ASM_EMIT(mv_v "       0x00(%[v]), %%xmm3")    /* xmm3 = v */ \
                __ASM_EMIT(mv_v "       0x10(%[v]), %%xmm11")   /* xmm11 = v */ \
                __ASM_EMIT("andps       %[X_SIGN], %%xmm3")     /* xmm3 = abs(v) */ \
                __ASM_EMIT("andps       %[X_SIGN], %%xmm11")    /* xmm11 = abs(v) */ \
                __ASM_EMIT("maxps       %[X_AMP], %%xmm3")      /* xmm3 = max(X_AMP, abs(v)), ignores denormalized values */ \
                __ASM_EMIT("maxps       %[X_AMP], %%xmm11")     /* xmm11 = max(X_AMP, abs(v)), ignores denormalized values */ \
                __ASM_EMIT("mulps       %%xmm0, %%xmm3")        /* xmm3 = max(X_AMP, abs(v))*zero */ \
                __ASM_EMIT("mulps       %%xmm8, %%xmm11")       /* xmm11 = max(X_AMP, abs(v))*zero */ \
                __ASM_EMIT("add         %[delta],%[v]") \
                : [v] "+r" (v) \
                : [delta]       "i" (d), \
                  [X_SIGN]      "m" (X_SIGN), \
                  [X_AMP]       "m" (X_AMP_THRESH) \
                : "cc", "%xmm3", "%xmm11" \
            )

        // Step 2: parse float value
        #define LOG_STEP1       \
            ARCH_X86_ASM \
            ( \
                __ASM_EMIT("movdqa      %%xmm3, %%xmm4")        /* xmm4 = v */ \
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
                : "%xmm3", "%xmm4" \
            )

        #define LOG_STEP1x2     \
            ARCH_X86_ASM \
            ( \
                __ASM_EMIT("movdqa      %%xmm3, %%xmm4")        /* xmm4 = v */ \
                __ASM_EMIT("movdqa      %%xmm11, %%xmm12")      /* xmm12 = v */ \
                __ASM_EMIT("psrld       $23, %%xmm4")           /* xmm4 = frac(v) */ \
                __ASM_EMIT("psrld       $23, %%xmm12")          /* xmm12 = frac(v) */ \
                __ASM_EMIT("andps       %[X_MANT], %%xmm3")     /* xmm3 = mant(v) */ \
                __ASM_EMIT("andps       %[X_MANT], %%xmm11")    /* xmm11 = mant(v) */ \
                __ASM_EMIT("psubd       %[X_MMASK], %%xmm4")    /* xmm4 = frac(v) - 127 */ \
                __ASM_EMIT("psubd       %[X_MMASK], %%xmm12")   /* xmm12 = frac(v) - 127 */ \
                __ASM_EMIT("orps        %[X_HALF], %%xmm3")     /* xmm3 = mant(v)+0.5 = V */ \
                __ASM_EMIT("orps        %[X_HALF], %%xmm11")    /* xmm11 = mant(v)+0.5 = V */ \
                __ASM_EMIT("cvtdq2ps    %%xmm4, %%xmm4")        /* xmm4 = float(frac(v)-127) = E */ \
                __ASM_EMIT("cvtdq2ps    %%xmm12, %%xmm12")      /* xmm12 = float(frac(v)-127) = E */ \
                : \
                : \
                    [X_MANT]      "m" (X_MANT), \
                    [X_MMASK]     "m" (X_MMASK), \
                    [X_HALF]      "m" (X_HALF) \
                : "%xmm3", "%xmm4", "%xmm11", "%xmm12" \
            )

        // Step 3: prepare logarithm approximation calculations
        #define LOG_STEP2   \
            ARCH_X86_ASM \
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

        #define LOG_STEP2x2   \
            ARCH_X86_ASM \
            ( \
                __ASM_EMIT("movaps      %%xmm3, %%xmm5")        /* xmm5 = V */ \
                __ASM_EMIT("movaps      %%xmm11, %%xmm13")      /* xmm13 = V */ \
                __ASM_EMIT("movaps      %%xmm3, %%xmm6")        /* xmm6 = V */ \
                __ASM_EMIT("movaps      %%xmm11, %%xmm14")      /* xmm14 = V */ \
                __ASM_EMIT("cmpltps     %[SQRT1_2], %%xmm5")    /* xmm5 = / V < sqrt(1/2) / */ \
                __ASM_EMIT("cmpltps     %[SQRT1_2], %%xmm13")   /* xmm13 = / V < sqrt(1/2) / */ \
                __ASM_EMIT("movaps      %[ONE], %%xmm7")        /* xmm7 = 1.0 */ \
                __ASM_EMIT("movaps      %[ONE], %%xmm15")       /* xmm15 = 1.0 */ \
                __ASM_EMIT("andps       %%xmm5, %%xmm6")        /* xmm6 = V * / V < sqrt(1/2) / */ \
                __ASM_EMIT("andps       %%xmm13, %%xmm14")      /* xmm14 = V * / V < sqrt(1/2) / */ \
                __ASM_EMIT("addps       %%xmm6, %%xmm3")        /* xmm6 = V + V * / V < sqrt(1/2) / */ \
                __ASM_EMIT("addps       %%xmm14, %%xmm11")      /* xmm14 = V + V * / V < sqrt(1/2) / */ \
                __ASM_EMIT("andnps      %%xmm7, %%xmm5")        /* xmm5 = 1.0 * / V >= sqrt(1/2) / */ \
                __ASM_EMIT("andnps      %%xmm15, %%xmm13")      /* xmm13 = 1.0 * / V >= sqrt(1/2) / */ \
                __ASM_EMIT("addps       %%xmm5, %%xmm4")        /* xmm4 = E + 1.0 * / V >= sqrt(1/2) /      = B */ \
                __ASM_EMIT("addps       %%xmm13, %%xmm12")      /* xmm12 = E + 1.0 * / V >= sqrt(1/2) /      = B */ \
                __ASM_EMIT("subps       %%xmm7, %%xmm3")        /* xmm3 = V + V * / V < sqrt(1/2) / - 1.0   = A */ \
                __ASM_EMIT("subps       %%xmm15, %%xmm11")      /* xmm11 = V + V * / V < sqrt(1/2) / - 1.0   = A */ \
                : \
                : \
                  [SQRT1_2]     "m" (SQRT1_2), \
                  [ONE]         "m" (ONE) \
                : "%xmm3", "%xmm4", "%xmm5", "%xmm6", "%xmm7", \
                  "%xmm11", "%xmm12", "%xmm13", "%xmm14", "%xmm15" \
            )

        // Step 4: calculate four logarithmic values
        #define LOG_STEP3  /* B*LXE + A*A*A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))))) + L9*A*A + A + B*LN2 */ \
            ARCH_X86_ASM \
            ( \
                __ASM_EMIT("movaps      0x00(%[CC]), %%xmm5")   /* xmm5 = L0 */ \
                __ASM_EMIT("mulps       %%xmm3, %%xmm5")        /* xmm5 = L0*A */ \
                __ASM_EMIT("addps       0x10(%[CC]), %%xmm5")   /* xmm5 = L1+L0*A */ \
                __ASM_EMIT("mulps       %%xmm3, %%xmm5")        /* xmm5 = A*(L1+L0*A) */ \
                __ASM_EMIT("addps       0x20(%[CC]), %%xmm5")   /* xmm5 = L2+A*(L1+L0*A) */ \
                __ASM_EMIT("mulps       %%xmm3, %%xmm5")        /* xmm5 = A*(L2+A*(L1+L0*A)) */ \
                __ASM_EMIT("addps       0x30(%[CC]), %%xmm5")   /* xmm5 = L3+A*L2+A*(L1+L0*A) */ \
                __ASM_EMIT("mulps       %%xmm3, %%xmm5")        /* xmm5 = A*(L3+A*L2+A*(L1+L0*A)) */ \
                __ASM_EMIT("addps       0x40(%[CC]), %%xmm5")   /* xmm5 = L4+A*(L3+A*L2+A*(L1+L0*A)) */ \
                __ASM_EMIT("mulps       %%xmm3, %%xmm5")        /* xmm5 = A*(L4+A*(L3+A*L2+A*(L1+L0*A))) */ \
                __ASM_EMIT("addps       0x50(%[CC]), %%xmm5")   /* xmm5 = L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))) */ \
                __ASM_EMIT("mulps       %%xmm3, %%xmm5")        /* xmm5 = A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A)))) */ \
                __ASM_EMIT("addps       0x60(%[CC]), %%xmm5")   /* xmm5 = L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A)))) */ \
                __ASM_EMIT("mulps       %%xmm3, %%xmm5")        /* xmm5 = A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))) */ \
                __ASM_EMIT("addps       0x70(%[CC]), %%xmm5")   /* xmm5 = L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))) */ \
                __ASM_EMIT("mulps       %%xmm3, %%xmm5")        /* xmm5 = A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A)))))) */ \
                __ASM_EMIT("addps       0x80(%[CC]), %%xmm5")   /* xmm5 = L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A)))))) */ \
                __ASM_EMIT("mulps       %%xmm3, %%xmm5")        /* xmm5 = A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))))) */ \
                __ASM_EMIT("addps       0x90(%[CC]), %%xmm5")   /* xmm5 = A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))))) + L9 */ \
                __ASM_EMIT("mulps       %%xmm3, %%xmm5")        /* xmm5 = A*(A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))))) + L9) */ \
                __ASM_EMIT("mulps       %%xmm3, %%xmm5")        /* xmm5 = A*A*(A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))))) + L9) */ \
                __ASM_EMIT("mulps       0xa0(%[CC]), %%xmm4")   /* xmm4 = B*(LXE + LN2) */ \
                __ASM_EMIT("addps       %%xmm4, %%xmm5")        /* xmm5 = B*(LXE + LN2) + A*A*A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))))) */ \
                __ASM_EMIT("addps       %%xmm3, %%xmm5")        /* xmm5 = B*(LXE + LN2) + A*A*A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))))) + A */ \
                \
                : \
                : \
                  [CC]          "r" (LOG_CC) \
                : "%xmm4", "%xmm5", "%xmm6" \
            )

        #define LOG_STEP3x2   \
            ARCH_X86_ASM \
            ( \
                __ASM_EMIT("movaps      0x00(%[CC]), %%xmm5")   /* xmm5     = L0 */ \
                __ASM_EMIT("movaps      0x00(%[CC]), %%xmm13")  /* xmm13    = L0 */ \
                __ASM_EMIT("mulps       %%xmm3, %%xmm5")        /* xmm5     = L0*A */ \
                __ASM_EMIT("mulps       %%xmm11, %%xmm13")      /* xmm13    = L0*A */ \
                __ASM_EMIT("addps       0x10(%[CC]), %%xmm5")   /* xmm5     = L1+L0*A */ \
                __ASM_EMIT("addps       0x10(%[CC]), %%xmm13")  /* xmm13    = L1+L0*A */ \
                __ASM_EMIT("mulps       %%xmm3, %%xmm5")        /* xmm5     = A*(L1+L0*A) */ \
                __ASM_EMIT("mulps       %%xmm11, %%xmm13")      /* xmm13    = A*(L1+L0*A) */ \
                __ASM_EMIT("addps       0x20(%[CC]), %%xmm5")   /* xmm5     = L2+A*(L1+L0*A) */ \
                __ASM_EMIT("addps       0x20(%[CC]), %%xmm13")  /* xmm13    = L2+A*(L1+L0*A) */ \
                __ASM_EMIT("mulps       %%xmm3, %%xmm5")        /* xmm5     = A*(L2+A*(L1+L0*A)) */ \
                __ASM_EMIT("mulps       %%xmm11, %%xmm13")      /* xmm13    = A*(L2+A*(L1+L0*A)) */ \
                __ASM_EMIT("addps       0x30(%[CC]), %%xmm5")   /* xmm5     = L3+A*L2+A*(L1+L0*A) */ \
                __ASM_EMIT("addps       0x30(%[CC]), %%xmm13")  /* xmm13    = L3+A*L2+A*(L1+L0*A) */ \
                __ASM_EMIT("mulps       %%xmm3, %%xmm5")        /* xmm5     = A*(L3+A*L2+A*(L1+L0*A)) */ \
                __ASM_EMIT("mulps       %%xmm11, %%xmm13")      /* xmm13    = A*(L3+A*L2+A*(L1+L0*A)) */ \
                __ASM_EMIT("addps       0x40(%[CC]), %%xmm5")   /* xmm5     = L4+A*(L3+A*L2+A*(L1+L0*A)) */ \
                __ASM_EMIT("addps       0x40(%[CC]), %%xmm13")  /* xmm13    = L4+A*(L3+A*L2+A*(L1+L0*A)) */ \
                __ASM_EMIT("mulps       %%xmm3, %%xmm5")        /* xmm5     = A*(L4+A*(L3+A*L2+A*(L1+L0*A))) */ \
                __ASM_EMIT("mulps       %%xmm11, %%xmm13")      /* xmm13    = A*(L4+A*(L3+A*L2+A*(L1+L0*A))) */ \
                __ASM_EMIT("addps       0x50(%[CC]), %%xmm5")   /* xmm5     = L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))) */ \
                __ASM_EMIT("addps       0x50(%[CC]), %%xmm13")  /* xmm13    = L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))) */ \
                __ASM_EMIT("mulps       %%xmm3, %%xmm5")        /* xmm5     = A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A)))) */ \
                __ASM_EMIT("mulps       %%xmm11, %%xmm13")      /* xmm13    = A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A)))) */ \
                __ASM_EMIT("addps       0x60(%[CC]), %%xmm5")   /* xmm5     = L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A)))) */ \
                __ASM_EMIT("addps       0x60(%[CC]), %%xmm13")  /* xmm13    = L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A)))) */ \
                __ASM_EMIT("mulps       %%xmm3, %%xmm5")        /* xmm5     = A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))) */ \
                __ASM_EMIT("mulps       %%xmm11, %%xmm13")      /* xmm13    = A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))) */ \
                __ASM_EMIT("addps       0x70(%[CC]), %%xmm5")   /* xmm5     = L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))) */ \
                __ASM_EMIT("addps       0x70(%[CC]), %%xmm13")  /* xmm13    = L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))) */ \
                __ASM_EMIT("mulps       %%xmm3, %%xmm5")        /* xmm5     = A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A)))))) */ \
                __ASM_EMIT("mulps       %%xmm11, %%xmm13")      /* xmm13    = A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A)))))) */ \
                __ASM_EMIT("addps       0x80(%[CC]), %%xmm5")   /* xmm5     = L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A)))))) */ \
                __ASM_EMIT("addps       0x80(%[CC]), %%xmm13")  /* xmm13    = L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A)))))) */ \
                __ASM_EMIT("mulps       %%xmm3, %%xmm5")        /* xmm5     = A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))))) */ \
                __ASM_EMIT("mulps       %%xmm11, %%xmm13")      /* xmm13    = A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))))) */ \
                __ASM_EMIT("addps       0x90(%[CC]), %%xmm5")   /* xmm5     = A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))))) + L9 */ \
                __ASM_EMIT("addps       0x90(%[CC]), %%xmm13")  /* xmm13    = A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))))) + L9 */ \
                __ASM_EMIT("mulps       %%xmm3, %%xmm5")        /* xmm3     = A*(A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))))) + L9) */ \
                __ASM_EMIT("mulps       %%xmm11, %%xmm13")      /* xmm11    = A*(A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))))) + L9) */ \
                __ASM_EMIT("mulps       %%xmm3, %%xmm5")        /* xmm3     = A*A*(A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))))) + L9) */ \
                __ASM_EMIT("mulps       %%xmm11, %%xmm13")      /* xmm11    = A*A*(A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))))) + L9) */ \
                __ASM_EMIT("mulps       0xa0(%[CC]), %%xmm4")   /* xmm4     = B*(LXE + LN2) */ \
                __ASM_EMIT("mulps       0xa0(%[CC]), %%xmm12")  /* xmm12    = B*(LXE + LN2) */ \
                __ASM_EMIT("addps       %%xmm4, %%xmm5")        /* xmm5     = B*(LXE + LN2) + A*A*A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))))) */ \
                __ASM_EMIT("addps       %%xmm12, %%xmm13")      /* xmm13    = B*(LXE + LN2) + A*A*A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))))) */ \
                __ASM_EMIT("addps       %%xmm3, %%xmm5")        /* xmm5     = B*(LXE + LN2) + A*A*A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))))) + A */ \
                __ASM_EMIT("addps       %%xmm11, %%xmm13")      /* xmm13    = B*(LXE + LN2) + A*A*A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))))) + A */ \
                \
                : \
                : \
                  [CC]          "r" (LOG_CC) \
                : "%xmm4", "%xmm5", "%xmm6", \
                  "%xmm12", "%xmm13", "%xmm14" \
            )

        // Step 5: apply vector and store values
        #define LOG_STORE(mv_x, mv_y, d)   \
            ARCH_X86_ASM \
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

        #define LOG_STOREx2(mv_x, mv_y, d)   \
            ARCH_X86_ASM \
            ( \
                __ASM_EMIT("movaps      %%xmm5, %%xmm3")        /* xmm3 = log(abs(v*zero)), xmm5=log(abs(v*zero)) */ \
                __ASM_EMIT("movaps      %%xmm13, %%xmm11")        /* xmm11 = log(abs(v*zero)), xmm5=log(abs(v*zero)) */ \
                __ASM_EMIT(mv_x "       0x00(%[x]), %%xmm4")        /* xmm4 = x */ \
                __ASM_EMIT(mv_x "       0x10(%[x]), %%xmm12")        /* xmm12 = x */ \
                __ASM_EMIT(mv_y "       0x00(%[y]), %%xmm6")        /* xmm6 = y */ \
                __ASM_EMIT(mv_y "       0x10(%[y]), %%xmm14")        /* xmm14 = y */ \
                __ASM_EMIT("mulps       %%xmm1, %%xmm3")        /* xmm3 = log(abs(v*zero)) * norm_x */ \
                __ASM_EMIT("mulps       %%xmm9, %%xmm11")        /* xmm11 = log(abs(v*zero)) * norm_x */ \
                __ASM_EMIT("mulps       %%xmm2, %%xmm5")        /* xmm5 = log(abs(v*zero)) * norm_y */ \
                __ASM_EMIT("mulps       %%xmm10, %%xmm13")        /* xmm13 = log(abs(v*zero)) * norm_y */ \
                __ASM_EMIT("addps       %%xmm3, %%xmm4")        /* xmm4 = x + log(abs(v*zero)) * norm_x */ \
                __ASM_EMIT("addps       %%xmm11, %%xmm12")        /* xmm12 = x + log(abs(v*zero)) * norm_x */ \
                __ASM_EMIT("addps       %%xmm5, %%xmm6")        /* xmm6 = y + log(abs(v*zero)) * norm_y */ \
                __ASM_EMIT("addps       %%xmm13, %%xmm14")        /* xmm14 = y + log(abs(v*zero)) * norm_y */ \
                __ASM_EMIT(mv_x "       %%xmm4, 0x00(%[x])") \
                __ASM_EMIT(mv_x "       %%xmm12, 0x10(%[x])") \
                __ASM_EMIT(mv_y "       %%xmm6, 0x00(%[y])") \
                __ASM_EMIT(mv_y "       %%xmm14, 0x10(%[y])") \
                __ASM_EMIT("add         %[delta], %[x]") \
                __ASM_EMIT("add         %[delta], %[y]") \
                : [x] "+r" (x), [y] "+r" (y) \
                : [delta] "i" (d) \
                : "%xmm3", "%xmm4", "%xmm6", \
                  "%xmm11", "%xmm12", "%xmm14" \
            )

        #define LOG_CALC    \
            LOG_STEP1;      \
            LOG_STEP2;      \
            LOG_STEP3

        #define LOG_CALCx2    \
            LOG_STEP1x2;      \
            LOG_STEP2x2;      \
            LOG_STEP3x2

        //---------------------------------------------------------------

        if (count <= 0)
            return;

        // Prepare constants
        ARCH_X86_ASM
        (
            __ASM_EMIT("movaps      %[zero], %%xmm0")
            __ASM_EMIT("movaps      %[norm_x], %%xmm1")
            __ASM_EMIT("movaps      %[norm_y], %%xmm2")
            __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0") // xmm0 == zero
            __ASM_EMIT("shufps      $0x00, %%xmm1, %%xmm1") // xmm1 == norm_x
            __ASM_EMIT("shufps      $0x00, %%xmm2, %%xmm2") // xmm2 == norm_y
            __ASM_EMIT("movaps      %%xmm0, %%xmm8")
            __ASM_EMIT("movaps      %%xmm1, %%xmm9")
            __ASM_EMIT("movaps      %%xmm2, %%xmm10")
            :
            : [zero] "x" (zero), [norm_x] "x" (norm_x), [norm_y] "x" (norm_y)
            : "%xmm0", "%xmm1", "%xmm2",
              "%xmm8", "%xmm9", "%xmm10"
        );

        // Make main body
        while (count >= 8)
        {
            LOG_LOADx2("lddqu", 0x20);
            LOG_CALCx2;
            LOG_STOREx2("movups", "movups", 0x20);
            count -= 8;
        }
        if (count >= 4)
        {
            LOG_LOAD("lddqu", 0x10);
            LOG_CALC;
            LOG_STORE("movups", "movups", 0x10);
            count -= 4;
        }

        // Complete the tail
        while (count--)
        {
            LOG_LOAD("movss", 4);
            LOG_CALC;
            LOG_STORE("movss", "movss", 4);
        }

        #undef LOG_LOAD
        #undef LOG_LOADx2
        #undef LOG_STEP1
        #undef LOG_STEP1x2
        #undef LOG_STEP2
        #undef LOG_STEP2x2
        #undef LOG_STEP3
        #undef LOG_STEP3x2
        #undef LOG_STORE
        #undef LOG_STOREx2
        #undef LOG_CALC
        #undef LOG_CALCx2
    }

    #endif /* ARCH_X86_64 */

    void x64_rgba32_to_bgra32(void *dst, const void *src, size_t count)
    {
        size_t off;

        ARCH_X86_64_ASM
        (
            __ASM_EMIT("movdqa      %[MASK], %%xmm6")                   // xmm6 = 00 ff 00 ff
            __ASM_EMIT("xor         %[off], %[off]")                    // off  = 0
            __ASM_EMIT("movdqa      %%xmm6, %%xmm7")                    // xmm7 = 00 ff 00 ff
            __ASM_EMIT("pslld       $8, %%xmm6")                        // xmm6 = ff 00 ff 00

            // 16-element blocks
            __ASM_EMIT("sub         $16, %[count]")
            __ASM_EMIT("jb          2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("prefetcht0  0x40(%[src], %[off])")
            __ASM_EMIT("prefetcht0  0x60(%[src], %[off])")
            __ASM_EMIT("movdqu      0x00(%[src], %[off]), %%xmm0")      // xmm0 = A1 R1 G1 B1
            __ASM_EMIT("movdqu      0x10(%[src], %[off]), %%xmm1")
            __ASM_EMIT("movdqu      0x20(%[src], %[off]), %%xmm2")
            __ASM_EMIT("movdqu      0x30(%[src], %[off]), %%xmm3")
            __ASM_EMIT("movdqa      %%xmm0, %%xmm8")                    // xmm8 = A1 R1 G1 B1
            __ASM_EMIT("movdqa      %%xmm1, %%xmm10")
            __ASM_EMIT("movdqa      %%xmm2, %%xmm12")
            __ASM_EMIT("movdqa      %%xmm3, %%xmm14")
            __ASM_EMIT("pand        %%xmm7, %%xmm0")                    // xmm0 = 00 R1 00 B1
            __ASM_EMIT("pand        %%xmm7, %%xmm1")
            __ASM_EMIT("pand        %%xmm7, %%xmm2")
            __ASM_EMIT("pand        %%xmm7, %%xmm3")
            __ASM_EMIT("pand        %%xmm6, %%xmm8")                    // xmm8 = A1 00 G1 00
            __ASM_EMIT("pand        %%xmm6, %%xmm10")
            __ASM_EMIT("pand        %%xmm6, %%xmm12")
            __ASM_EMIT("pand        %%xmm6, %%xmm14")
            __ASM_EMIT("movdqa      %%xmm0, %%xmm9")                    // xmm9 = A1 00 G1 00
            __ASM_EMIT("movdqa      %%xmm1, %%xmm11")
            __ASM_EMIT("movdqa      %%xmm2, %%xmm13")
            __ASM_EMIT("movdqa      %%xmm3, %%xmm15")
            __ASM_EMIT("pslld       $16, %%xmm0")                       // xmm0 = 00 B1 00 00
            __ASM_EMIT("pslld       $16, %%xmm1")
            __ASM_EMIT("pslld       $16, %%xmm2")
            __ASM_EMIT("pslld       $16, %%xmm3")
            __ASM_EMIT("psrld       $16, %%xmm9")                       // xmm9 = 00 00 00 R1
            __ASM_EMIT("psrld       $16, %%xmm11")
            __ASM_EMIT("psrld       $16, %%xmm13")
            __ASM_EMIT("psrld       $16, %%xmm15")
            __ASM_EMIT("orpd        %%xmm8, %%xmm0")                    // xmm0 = A1 B1 G1 00
            __ASM_EMIT("orpd        %%xmm10, %%xmm1")
            __ASM_EMIT("orpd        %%xmm12, %%xmm2")
            __ASM_EMIT("orpd        %%xmm14, %%xmm3")
            __ASM_EMIT("orpd        %%xmm9, %%xmm0")                    // xmm0 = A1 B1 G1 R1
            __ASM_EMIT("orpd        %%xmm11, %%xmm1")
            __ASM_EMIT("orpd        %%xmm13, %%xmm2")
            __ASM_EMIT("orpd        %%xmm15, %%xmm3")
            __ASM_EMIT("movdqu      %%xmm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("movdqu      %%xmm1, 0x10(%[dst], %[off])")
            __ASM_EMIT("movdqu      %%xmm2, 0x20(%[dst], %[off])")
            __ASM_EMIT("movdqu      %%xmm3, 0x30(%[dst], %[off])")
            __ASM_EMIT("add         $0x40, %[off]")
            __ASM_EMIT("sub         $16, %[count]")
            __ASM_EMIT("jae         1b")

            // 8-element blocks
            __ASM_EMIT("2:")
            __ASM_EMIT("add         $8, %[count]")
            __ASM_EMIT("jl          4f")
            __ASM_EMIT("movdqu      0x00(%[src], %[off]), %%xmm0")      // xmm0 = A1 R1 G1 B1
            __ASM_EMIT("movdqu      0x10(%[src], %[off]), %%xmm1")
            __ASM_EMIT("movdqa      %%xmm0, %%xmm2")                    // xmm2 = A1 R1 G1 B1
            __ASM_EMIT("movdqa      %%xmm1, %%xmm3")
            __ASM_EMIT("pand        %%xmm7, %%xmm0")                    // xmm0 = 00 R1 00 B1
            __ASM_EMIT("pand        %%xmm6, %%xmm2")                    // xmm2 = A1 00 G1 00
            __ASM_EMIT("pand        %%xmm7, %%xmm1")
            __ASM_EMIT("pand        %%xmm6, %%xmm3")
            __ASM_EMIT("movdqa      %%xmm0, %%xmm4")                    // xmm4 = A1 00 G1 00
            __ASM_EMIT("movdqa      %%xmm1, %%xmm5")
            __ASM_EMIT("pslld       $16, %%xmm0")                       // xmm0 = 00 B1 00 00
            __ASM_EMIT("pslld       $16, %%xmm1")
            __ASM_EMIT("psrld       $16, %%xmm4")                       // xmm4 = 00 00 00 R1
            __ASM_EMIT("psrld       $16, %%xmm5")
            __ASM_EMIT("orpd        %%xmm2, %%xmm0")                    // xmm0 = A1 B1 G1 00
            __ASM_EMIT("orpd        %%xmm3, %%xmm1")
            __ASM_EMIT("orpd        %%xmm4, %%xmm0")                    // xmm0 = A1 B1 G1 R1
            __ASM_EMIT("orpd        %%xmm5, %%xmm1")
            __ASM_EMIT("movdqu      %%xmm0, 0x00(%[dst], %[off])")
            __ASM_EMIT("movdqu      %%xmm1, 0x10(%[dst], %[off])")
            __ASM_EMIT("add         $0x20, %[off]")
            __ASM_EMIT("sub         $8, %[count]")

            // 4-element block
            __ASM_EMIT("4:")
            __ASM_EMIT("add         $4, %[count]")
            __ASM_EMIT("jl          6f")
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
            __ASM_EMIT("6:")
            __ASM_EMIT("add         $3, %[count]")
            __ASM_EMIT("jl          8f")
            __ASM_EMIT("7:")
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
            __ASM_EMIT("jge         7b")

            // End
            __ASM_EMIT("8:")

            : [dst] "+r"(dst), [src] "+r"(src), [count] "+r" (count),
              [off] "=&r" (off)
            : [MASK] "m" (X_CMASK)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7",
              "%xmm8", "%xmm9", "%xmm10", "%xmm11",
              "%xmm12", "%xmm13", "%xmm14", "%xmm15"
        );
    }
}

#endif /* DSP_ARCH_X86_SSE3_GRAPHICS_H_ */
