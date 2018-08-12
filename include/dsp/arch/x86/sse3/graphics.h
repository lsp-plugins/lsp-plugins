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

#ifndef DSP_ARCH_X86_SSE_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_SSE_IMPL */

#ifdef ARCH_X86_64

void x64_axis_apply_log(float *x, float *y, const float *v, float zero, float norm_x, float norm_y, size_t count)
{
    // Step 1: load vector, take absolute value and limit it by minimum value
    #define LOG_LOAD(mv_v, d)   \
        __asm__ __volatile__ \
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
        __asm__ __volatile__ \
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
        __asm__ __volatile__ \
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
        __asm__ __volatile__ \
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

    #define LOG_STEP2x2   \
        __asm__ __volatile__ \
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

    #define LOG_STEP3x2   \
        __asm__ __volatile__ \
        ( \
            __ASM_EMIT("movaps      %%xmm3, %%xmm7")        /* xmm5 = A  */ \
        __ASM_EMIT("movaps      %%xmm11, %%xmm15")      /* xmm13 = A  */ \
            __ASM_EMIT("mulps       %%xmm7, %%xmm7")        /* xmm7 = A*A */ \
        __ASM_EMIT("mulps       %%xmm15, %%xmm15")      /* xmm15 = A*A */ \
            \
            __ASM_EMIT("movaps      %[L0], %%xmm5")         /* xmm5 = L0 */ \
        __ASM_EMIT("movaps      %[L0], %%xmm13")         /* xmm13 = L0 */ \
            __ASM_EMIT("mulps       %%xmm3, %%xmm5")        /* xmm5 = L0*A */ \
        __ASM_EMIT("mulps       %%xmm11, %%xmm13")        /* xmm13 = L0*A */ \
            __ASM_EMIT("movaps      %[L1], %%xmm6")         /* xmm6 = L1 */ \
        __ASM_EMIT("movaps      %[L1], %%xmm14")         /* xmm14 = L1 */ \
            __ASM_EMIT("addps       %%xmm6, %%xmm5")        /* xmm5 = L1+L0*A */ \
        __ASM_EMIT("addps       %%xmm14, %%xmm13")        /* xmm13 = L1+L0*A */ \
            __ASM_EMIT("mulps       %%xmm3, %%xmm5")        /* xmm5 = A*(L1+L0*A) */ \
        __ASM_EMIT("mulps       %%xmm11, %%xmm13")        /* xmm13 = A*(L1+L0*A) */ \
            __ASM_EMIT("movaps      %[L2], %%xmm6")         /* xmm6 = L2 */ \
        __ASM_EMIT("movaps      %[L2], %%xmm14")         /* xmm14 = L2 */ \
            __ASM_EMIT("addps       %%xmm6, %%xmm5")        /* xmm5 = L2+A*(L1+L0*A) */ \
        __ASM_EMIT("addps       %%xmm14, %%xmm13")        /* xmm13 = L2+A*(L1+L0*A) */ \
            __ASM_EMIT("mulps       %%xmm3, %%xmm5")        /* xmm5 = A*(L2+A*(L1+L0*A)) */ \
        __ASM_EMIT("mulps       %%xmm11, %%xmm13")        /* xmm13 = A*(L2+A*(L1+L0*A)) */ \
            __ASM_EMIT("movaps      %[L3], %%xmm6")         /* xmm6 = L3 */ \
        __ASM_EMIT("movaps      %[L3], %%xmm14")         /* xmm14 = L3 */ \
            __ASM_EMIT("addps       %%xmm6, %%xmm5")        /* xmm5 = L3+A*L2+A*(L1+L0*A) */ \
        __ASM_EMIT("addps       %%xmm14, %%xmm13")        /* xmm13 = L3+A*L2+A*(L1+L0*A) */ \
            __ASM_EMIT("mulps       %%xmm3, %%xmm5")        /* xmm5 = A*(L3+A*L2+A*(L1+L0*A)) */ \
        __ASM_EMIT("mulps       %%xmm11, %%xmm13")        /* xmm13 = A*(L3+A*L2+A*(L1+L0*A)) */ \
            __ASM_EMIT("movaps      %[L4], %%xmm6")         /* xmm6 = L4 */ \
        __ASM_EMIT("movaps      %[L4], %%xmm14")         /* xmm14 = L4 */ \
            __ASM_EMIT("addps       %%xmm6, %%xmm5")        /* xmm5 = L4+A*(L3+A*L2+A*(L1+L0*A)) */ \
        __ASM_EMIT("addps       %%xmm14, %%xmm13")        /* xmm13 = L4+A*(L3+A*L2+A*(L1+L0*A)) */ \
            __ASM_EMIT("mulps       %%xmm3, %%xmm5")        /* xmm5 = A*(L4+A*(L3+A*L2+A*(L1+L0*A))) */ \
        __ASM_EMIT("mulps       %%xmm11, %%xmm13")        /* xmm13 = A*(L4+A*(L3+A*L2+A*(L1+L0*A))) */ \
            __ASM_EMIT("movaps      %[L5], %%xmm6")         /* xmm6 = L5 */ \
        __ASM_EMIT("movaps      %[L5], %%xmm14")         /* xmm14 = L5 */ \
            __ASM_EMIT("addps       %%xmm6, %%xmm5")        /* xmm5 = L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))) */ \
        __ASM_EMIT("addps       %%xmm14, %%xmm13")        /* xmm13 = L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))) */ \
            __ASM_EMIT("mulps       %%xmm3, %%xmm5")        /* xmm5 = A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A)))) */ \
        __ASM_EMIT("mulps       %%xmm11, %%xmm13")        /* xmm13 = A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A)))) */ \
            __ASM_EMIT("movaps      %[L6], %%xmm6")         /* xmm6 = L6 */ \
        __ASM_EMIT("movaps      %[L6], %%xmm14")         /* xmm14 = L6 */ \
            __ASM_EMIT("addps       %%xmm6, %%xmm5")        /* xmm5 = L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A)))) */ \
        __ASM_EMIT("addps       %%xmm14, %%xmm13")        /* xmm13 = L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A)))) */ \
            __ASM_EMIT("mulps       %%xmm3, %%xmm5")        /* xmm5 = A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))) */ \
        __ASM_EMIT("mulps       %%xmm11, %%xmm13")        /* xmm13 = A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))) */ \
            __ASM_EMIT("movaps      %[L7], %%xmm6")         /* xmm6 = L7 */ \
        __ASM_EMIT("movaps      %[L7], %%xmm14")         /* xmm14 = L7 */ \
            __ASM_EMIT("addps       %%xmm6, %%xmm5")        /* xmm5 = L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))) */ \
        __ASM_EMIT("addps       %%xmm14, %%xmm13")        /* xmm13 = L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))) */ \
            __ASM_EMIT("mulps       %%xmm3, %%xmm5")        /* xmm5 = A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A)))))) */ \
        __ASM_EMIT("mulps       %%xmm11, %%xmm13")        /* xmm13 = A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A)))))) */ \
            __ASM_EMIT("movaps      %[L8], %%xmm6")         /* xmm6 = L8 */ \
        __ASM_EMIT("movaps      %[L8], %%xmm14")         /* xmm14 = L8 */ \
            __ASM_EMIT("addps       %%xmm6, %%xmm5")        /* xmm5 = L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A)))))) */ \
        __ASM_EMIT("addps       %%xmm14, %%xmm13")        /* xmm13 = L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A)))))) */ \
            __ASM_EMIT("mulps       %%xmm3, %%xmm5")        /* xmm5 = A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))))) */ \
        __ASM_EMIT("mulps       %%xmm11, %%xmm13")        /* xmm13 = A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))))) */ \
            __ASM_EMIT("mulps       %%xmm7, %%xmm5")        /* xmm5 = A*A*A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))))) */ \
        __ASM_EMIT("mulps       %%xmm15, %%xmm13")        /* xmm13 = A*A*A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))))) */ \
            \
            __ASM_EMIT("movaps      %[LXE], %%xmm6")        /* xmm6 = LXE */ \
        __ASM_EMIT("movaps      %[LXE], %%xmm14")        /* xmm14 = LXE */ \
            __ASM_EMIT("mulps       %%xmm4, %%xmm6")        /* xmm6 = B*LXE */ \
        __ASM_EMIT("mulps       %%xmm12, %%xmm14")        /* xmm14 = B*LXE */ \
            __ASM_EMIT("addps       %%xmm6, %%xmm5")        /* xmm5 = B*LXE+A*A*A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))))) */ \
        __ASM_EMIT("addps       %%xmm14, %%xmm13")        /* xmm13 = B*LXE+A*A*A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))))) */ \
            \
            __ASM_EMIT("movaps      %[L9], %%xmm6")         /* xmm6 = L9 */ \
        __ASM_EMIT("movaps      %[L9], %%xmm14")         /* xmm14 = L9 */ \
            __ASM_EMIT("mulps       %%xmm7, %%xmm6")        /* xmm6 = L9*A*A */ \
        __ASM_EMIT("mulps       %%xmm15, %%xmm14")        /* xmm14 = L9*A*A */ \
            __ASM_EMIT("subps       %%xmm6, %%xmm5")        /* xmm5 = B*LXE+A*A*A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))))) - L9*A*A */ \
        __ASM_EMIT("subps       %%xmm14, %%xmm13")        /* xmm13 = B*LXE+A*A*A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))))) - L9*A*A */ \
            \
            __ASM_EMIT("movaps      %[LN2], %%xmm6")        /* xmm6 = LN2 */ \
        __ASM_EMIT("movaps      %[LN2], %%xmm14")        /* xmm14 = LN2 */ \
            __ASM_EMIT("mulps       %%xmm4, %%xmm6")        /* xmm6 = B*LN2 */ \
        __ASM_EMIT("mulps       %%xmm12, %%xmm14")        /* xmm14 = B*LN2 */ \
            __ASM_EMIT("addps       %%xmm3, %%xmm5")        /* xmm5 = B*LXE+A*A*A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))))) - L9*A*A + A */ \
        __ASM_EMIT("addps       %%xmm11, %%xmm13")        /* xmm13 = B*LXE+A*A*A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))))) - L9*A*A + A */ \
            __ASM_EMIT("addps       %%xmm6, %%xmm5")        /* xmm5 = B*LXE+A*A*A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))))) - L9*A*A + A + B*LN2 */ \
        __ASM_EMIT("addps       %%xmm14, %%xmm13")        /* xmm13 = B*LXE+A*A*A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))))) - L9*A*A + A + B*LN2 */ \
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
            : "%xmm5", "%xmm6", "%xmm7", \
              "%xmm13", "%xmm14", "%xmm15" \
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

    #define LOG_STOREx2(mv_x, mv_y, d)   \
        __asm__ __volatile__ \
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
    __asm__ __volatile__
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
            while (count >= 8)
            {
                LOG_LOADx2("movaps", 0x20);
                LOG_CALCx2;
                LOG_STOREx2("movaps", "movaps", 0x20);
                count -= 8;
            }
            if (count >= 4)
            {
                LOG_LOAD("movaps", 0x10);
                LOG_CALC;
                LOG_STORE("movaps", "movaps", 0x10);
                count -= 4;
            }
        }
        else
        {
            while (count >= 8)
            {
                LOG_LOADx2("movaps", 0x20);
                LOG_CALCx2;
                LOG_STOREx2("movaps", "movups", 0x20);
                count -= 8;
            }
            if (count >= 4)
            {
                LOG_LOAD("movaps", 0x10);
                LOG_CALC;
                LOG_STORE("movaps", "movups", 0x10);
                count -= 4;
            }
        }
    }
    else
    {
        if (sse_aligned(y))
        {
            while (count >= 8)
            {
                LOG_LOADx2("movups", 0x20);
                LOG_CALCx2;
                LOG_STOREx2("movups", "movaps", 0x20);
                count -= 8;
            }
            if (count >= 4)
            {
                LOG_LOAD("movups", 0x10);
                LOG_CALC;
                LOG_STORE("movups", "movaps", 0x10);
                count -= 4;
            }
        }
        else
        {
            while (count >= 8)
            {
                LOG_LOADx2("movups", 0x20);
                LOG_CALCx2;
                LOG_STOREx2("movups", "movups", 0x20);
                count -= 8;
            }
            if (count >= 4)
            {
                LOG_LOAD("movups", 0x10);
                LOG_CALC;
                LOG_STORE("movups", "movups", 0x10);
                count -= 4;
            }
        }
    }

    // Complete the tail
    while (count--)
    {
        LOG_LOAD("movss", sizeof(float));
        LOG_CALC;
        LOG_STORE("movss", "movss", sizeof(float));
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

#endif /* DSP_ARCH_X86_SSE3_GRAPHICS_H_ */
