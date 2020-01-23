/*
 * graphics.h
 *
 *  Created on: 07 авг. 2016 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_AVX_GRAPHICS_H_
#define DSP_ARCH_X86_AVX_GRAPHICS_H_

#ifndef DSP_ARCH_X86_AVX_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_AVX_IMPL */

#if 0
void axis_apply_log2(float *x, float *y, const float *v, float zero, float norm_x, float norm_y, size_t count)
{
    // Step 1: load vector, take absolute value and limit it by minimum value
    #define LOG_LOAD(r, mv_v, d)   \
        ARCH_X86_ASM \
        ( \
            __ASM_EMIT(mv_v "       (%[v]), %%" r "mm3")            /* xmm3 = v */ \
            __ASM_EMIT("vandps      %[X_SIGN], %%ymm3, %%ymm3")     /* xmm3 = abs(v) */ \
            __ASM_EMIT("vmaxps      %[X_AMP], %%ymm3, %%ymm3")      /* xmm3 = max(X_AMP, abs(v)), ignores denormalized values */ \
            __ASM_EMIT("vmulps      %%ymm0, %%ymm3, %%ymm3")        /* xmm5 = max(X_AMP, abs(v))*zero */ \
            __ASM_EMIT("add         %[delta],%[v]") \
            : [v] "+r" (v) \
            : [delta]       "i" (d), \
              [X_SIGN]      "m" (X_SIGN), \
              [X_AMP]       "m" (X_AMP_THRESH) \
            : "cc", "%xmm3" \
        )

    // Step 2: parse float value
    #define LOG_STEP1       \
        ARCH_X86_ASM \
        ( \
            __ASM_EMIT("vpsrld      $23, %%ymm3, %%ymm4")           /* xmm4 = frac(v) */ \
            __ASM_EMIT("vandps      %[X_MANT], %%ymm3, %%ymm3")     /* xmm3 = mant(v) */ \
            __ASM_EMIT("vpsubd      %[X_MMASK], %%ymm4, %%ymm4")    /* xmm4 = frac(v) - 127 */ \
            __ASM_EMIT("vorps       %[X_HALF], %%ymm3, %%ymm3")     /* xmm3 = mant(v)+0.5 = V */ \
            __ASM_EMIT("vcvtdq2ps   %%ymm4, %%ymm4")                /* xmm4 = float(frac(v)-127) = E */ \
            : \
            : \
                [X_MANT]      "m" (X_MANT), \
                [X_MMASK]     "m" (X_MMASK), \
                [X_HALF]      "m" (X_HALF) \
            : "%xmm4", "%xmm3", "%xmm5" \
        )

    // Step 3: prepare logarithm approximation calculations
    #define LOG_STEP2   \
        ARCH_X86_ASM \
        ( \
            __ASM_EMIT("vcmpltps    %[SQRT1_2], %%ymm3, %%ymm5")    /* xmm5 = / V < sqrt(1/2) / */ \
            __ASM_EMIT("vmovaps     %[ONE], %%ymm7")                /* xmm7 = 1.0 */ \
            __ASM_EMIT("vandps      %%ymm5, %%ymm3, %%ymm6")        /* xmm6 = V * / V < sqrt(1/2) / */ \
            __ASM_EMIT("vaddps      %%ymm6, %%ymm3, %%ymm3")        /* xmm6 = V + V * / V < sqrt(1/2) / */ \
            __ASM_EMIT("vandnps     %%ymm7, %%ymm5, %%ymm5")        /* xmm5 = 1.0 * / V >= sqrt(1/2) / */ \
            __ASM_EMIT("vaddps      %%ymm5, %%ymm4, %%ymm4")        /* xmm4 = E + 1.0 * / V >= sqrt(1/2) /      = B */ \
            __ASM_EMIT("vsubps      %%ymm7, %%ymm3, %%ymm3")        /* xmm3 = V + V * / V < sqrt(1/2) / - 1.0   = A */ \
            : \
            : \
              [SQRT1_2]     "m" (SQRT1_2), \
              [ONE]         "m" (ONE) \
            : "%xmm3", "%xmm4", "%xmm5", "%xmm6", "%xmm7" \
        )

    // Step 4: calculate four logarithmic values
    #define LOG_STEP3   \
        ARCH_X86_ASM \
        ( \
            __ASM_EMIT("vmulps      %%ymm3, %%ymm3, %%ymm7")        /* xmm7 = A*A */ \
            \
            __ASM_EMIT("vmulps      %[L0], %%ymm3, %%ymm5")         /* xmm5 = L0*A */ \
            __ASM_EMIT("vaddps      %[L1], %%ymm5, %%ymm5")         /* xmm5 = L1+L0*A */ \
            __ASM_EMIT("vmulps      %%ymm3, %%ymm5, %%ymm5")        /* xmm5 = A*(L1+L0*A) */ \
            __ASM_EMIT("vaddps      %[L2], %%ymm5, %%ymm5")         /* xmm5 = L2+A*(L1+L0*A) */ \
            __ASM_EMIT("vmulps      %%ymm3, %%ymm5, %%ymm5")        /* xmm5 = A*(L2+A*(L1+L0*A)) */ \
            __ASM_EMIT("vaddps      %[L3], %%ymm5, %%ymm5")         /* xmm5 = L3+A*L2+A*(L1+L0*A) */ \
            __ASM_EMIT("vmulps      %%ymm3, %%ymm5, %%ymm5")        /* xmm5 = A*(L3+A*L2+A*(L1+L0*A)) */ \
            __ASM_EMIT("vaddps      %[L4], %%ymm5, %%ymm5")         /* xmm5 = L4+A*(L3+A*L2+A*(L1+L0*A)) */ \
            __ASM_EMIT("vmulps      %%ymm3, %%ymm5, %%ymm5")        /* xmm5 = A*(L4+A*(L3+A*L2+A*(L1+L0*A))) */ \
            __ASM_EMIT("vaddps      %[L5], %%ymm5, %%ymm5")         /* xmm5 = L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))) */ \
            __ASM_EMIT("vmulps      %%ymm3, %%ymm5, %%ymm5")        /* xmm5 = A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A)))) */ \
            __ASM_EMIT("vaddps      %[L6], %%ymm5, %%ymm5")         /* xmm5 = L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A)))) */ \
            __ASM_EMIT("vmulps      %%ymm3, %%ymm5, %%ymm5")        /* xmm5 = A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))) */ \
            __ASM_EMIT("vaddps      %[L7], %%ymm5, %%ymm5")         /* xmm5 = L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))) */ \
            __ASM_EMIT("vmulps      %%ymm3, %%ymm5, %%ymm5")        /* xmm5 = A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A)))))) */ \
            __ASM_EMIT("vaddps      %[L8], %%ymm5, %%ymm5")         /* xmm5 = L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A)))))) */ \
            __ASM_EMIT("vmulps      %%ymm3, %%ymm5, %%ymm5")        /* xmm5 = A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))))) */ \
            __ASM_EMIT("vmulps      %%ymm7, %%ymm5, %%ymm5")        /* xmm5 = A*A*A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))))) */ \
            \
            __ASM_EMIT("vmulps      %[LXE], %%ymm4, %%ymm6")        /* xmm6 = B*LXE */ \
            __ASM_EMIT("vaddps      %%ymm6, %%ymm5, %%ymm5")        /* xmm5 = B*LXE+A*A*A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))))) */ \
            \
            __ASM_EMIT("vmulps      %[L9], %%ymm7, %%ymm6")         /* xmm6 = L9*A*A */ \
            __ASM_EMIT("vsubps      %%ymm6, %%ymm5, %%ymm5")        /* xmm5 = B*LXE+A*A*A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))))) - L9*A*A */ \
            \
            __ASM_EMIT("vmulps      %[LN2], %%ymm4, %%ymm6")        /* xmm6 = B*LN2 */ \
            __ASM_EMIT("vaddps      %%ymm3, %%ymm5, %%ymm5")        /* xmm5 = B*LXE+A*A*A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))))) - L9*A*A + A */ \
            __ASM_EMIT("vaddps      %%ymm6, %%ymm5, %%ymm5")        /* xmm5 = B*LXE+A*A*A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*L2+A*(L1+L0*A))))))) - L9*A*A + A + B*LN2 */ \
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
    #define LOG_STORE(r, mv_x, mv_y, d)   \
        ARCH_X86_ASM \
        ( \
            __ASM_EMIT("vmovaps     %%ymm5, %%ymm3")                /* xmm3 = log(abs(v*zero)), xmm5=log(abs(v*zero)) */ \
            __ASM_EMIT(mv_x "       (%[x]), %%" r "mm4")            /* xmm4 = x */ \
            __ASM_EMIT(mv_y "       (%[y]), %%" r "mm6")            /* xmm6 = y */ \
            __ASM_EMIT("vmulps      %%ymm2, %%ymm3, %%ymm5")        /* xmm5 = log(abs(v*zero)) * norm_y */ \
            __ASM_EMIT("vmulps      %%ymm1, %%ymm3, %%ymm3")        /* xmm3 = log(abs(v*zero)) * norm_x */ \
            __ASM_EMIT("vaddps      %%ymm3, %%ymm4, %%ymm4")        /* xmm4 = x + log(abs(v*zero)) * norm_x */ \
            __ASM_EMIT("vaddps      %%ymm5, %%ymm6, %%ymm6")        /* xmm6 = y + log(abs(v*zero)) * norm_y */ \
            __ASM_EMIT(mv_x "       %%" r "mm4, (%[x])") \
            __ASM_EMIT(mv_y "       %%" r "mm6, (%[y])") \
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
    ARCH_X86_ASM
    (
        __ASM_EMIT("vshufps     $0x00, %%ymm0, %%ymm0, %%ymm0") // xmm0 == zero
        __ASM_EMIT("vshufps     $0x00, %%ymm1, %%ymm1, %%ymm1") // xmm1 == norm_x
        __ASM_EMIT("vshufps     $0x00, %%ymm2, %%ymm2, %%ymm2") // xmm2 == norm_y
        __ASM_EMIT("vinsertf128 $1, %%xmm0, %%ymm0, %%ymm0")
        __ASM_EMIT("vinsertf128 $1, %%xmm1, %%ymm1, %%ymm1")
        __ASM_EMIT("vinsertf128 $1, %%xmm2, %%ymm2, %%ymm2")
        :
        : [zero] "x" (zero), [norm_x] "x" (norm_x), [norm_y] "x" (norm_y)
        : "%xmm0", "%xmm1", "%xmm2"
    );

    // Ensure v is aligned
    while (!avx_aligned(v))
    {
        LOG_LOAD("x", "vmovss", sizeof(float));
        LOG_CALC;
        LOG_STORE("x", "vmovss", "vmovss", sizeof(float));

        if (!(--count))
            return;
    }

    // Make main body
    if (avx_aligned(x))
    {
        if (avx_aligned(y))
        {
            while (count >= AVX_MULTIPLE)
            {
                LOG_LOAD("y", "vmovaps", AVX_ALIGN);
                LOG_CALC;
                LOG_STORE("y", "vmovaps", "vmovaps", AVX_ALIGN);
                count -= AVX_MULTIPLE;
            }
        }
        else
        {
            while (count >= AVX_MULTIPLE)
            {
                LOG_LOAD("y", "vmovaps", AVX_ALIGN);
                LOG_CALC;
                LOG_STORE("y", "vmovaps", "vmovups", AVX_ALIGN);
                count -= AVX_MULTIPLE;
            }
        }
    }
    else
    {
        if (avx_aligned(y))
        {
            while (count >= AVX_MULTIPLE)
            {
                LOG_LOAD("y", "vmovaps", AVX_ALIGN);
                LOG_CALC;
                LOG_STORE("y", "vmovups", "vmovaps", AVX_ALIGN);
                count -= AVX_MULTIPLE;
            }
        }
        else
        {
            while (count >= AVX_MULTIPLE)
            {
                LOG_LOAD("y", "vmovaps", AVX_ALIGN);
                LOG_CALC;
                LOG_STORE("y", "vmovups", "vmovups", AVX_ALIGN);
                count -= AVX_MULTIPLE;
            }
        }
    }

    // Complete the tail
    while (count > 0)
    {
        LOG_LOAD("x", "vmovss", sizeof(float));
        LOG_CALC;
        LOG_STORE("x", "vmovss", "vmovss", sizeof(float));
        --count;
    }

    VZEROUPPER;

    #undef LOG_LOAD
    #undef LOG_STEP1
    #undef LOG_STEP2
    #undef LOG_STEP3
    #undef LOG_STORE
}
#endif

#endif /* DSP_ARCH_X86_AVX_GRAPHICS_H_ */
