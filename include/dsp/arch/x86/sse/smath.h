/*
 * smath.h
 *
 *  Created on: 2 апр. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_SSE_SMATH_H_
#define DSP_ARCH_X86_SSE_SMATH_H_

namespace sse
{
    float ipowf(float x, int deg)
    {
        float res = 1.0f;
        float one;

        ARCH_X86_ASM
        (
            __ASM_EMIT("cmp     $0, %[deg]")
            __ASM_EMIT("jz      100f")
            __ASM_EMIT("jl      10f")

            __ASM_EMIT("1:")
                __ASM_EMIT("test    $1, %[deg]")
                __ASM_EMIT("jz      2f")
                __ASM_EMIT("mulss   %[x], %[res]")
                __ASM_EMIT("dec     %[deg]")
                __ASM_EMIT("jnz     1b")
                __ASM_EMIT("jmp     100f")
                __ASM_EMIT("2:")
                __ASM_EMIT("mulss   %[x], %[x]")
                __ASM_EMIT("shr     $1, %[deg]")
                __ASM_EMIT("jnz     1b")
                __ASM_EMIT("jmp     100f")

            __ASM_EMIT("10:")
            __ASM_EMIT("neg     %[deg]")
            __ASM_EMIT("movaps  %[res], %[one]")
            __ASM_EMIT("1:")
                __ASM_EMIT("test    $1, %[deg]")
                __ASM_EMIT("jz      2f")
                __ASM_EMIT("mulss   %[x], %[res]")
                __ASM_EMIT("dec     %[deg]")
                __ASM_EMIT("jnz     1b")
                __ASM_EMIT("jmp     20f")
                __ASM_EMIT("2:")
                __ASM_EMIT("mulss   %[x], %[x]")
                __ASM_EMIT("shr     $1, %[deg]")
                __ASM_EMIT("jnz     1b")

            __ASM_EMIT("20:")
                __ASM_EMIT("divss   %[res], %[one]")
                __ASM_EMIT("movaps  %[one], %[res]")

            __ASM_EMIT("100:")
            : [x] "+x" (x), [res] "+x" (res), [deg] "+r" (deg), [one] "=&x" (one)
            :
            : "cc"
        );

        return res;
    }

    float irootf(float x, int deg)
    {
        float a, tmp, k, sign, xp;
        float tres, tx;
        int tdeg;

        ARCH_X86_ASM
        (
            __ASM_EMIT("cmp     $1, %[deg]")
            __ASM_EMIT("jl      100f")

            // While deg is odd, just calculate sqrt function
            __ASM_EMIT("test    $1, %[deg]")
            __ASM_EMIT("jne     2f")
            __ASM_EMIT("1:")
                __ASM_EMIT("shr     $1, %[deg]")
                __ASM_EMIT("sqrtss  %[x], %[x]")
                __ASM_EMIT("test    $1, %[deg]")
                __ASM_EMIT("jz      1b")
                __ASM_EMIT("cmp     $1, %[deg]")
                __ASM_EMIT("jbe     100f")

            // Prepare calculations
            __ASM_EMIT("2:")
            __ASM_EMIT("movaps      %[X_ONE], %[a]")
            __ASM_EMIT("cvtsi2ss    %[deg], %[tmp]")
            __ASM_EMIT("movaps      %[X_SIGN], %[sign]")
            __ASM_EMIT("divss       %[tmp], %[a]")
            __ASM_EMIT("dec         %[deg]")
            __ASM_EMIT("cvtsi2ss    %[deg], %[k]")
            __ASM_EMIT("mulss       %[a], %[k]")
            __ASM_EMIT("mulss       %[x], %[a]")

            // Newton method
            __ASM_EMIT("1:")
                __ASM_EMIT("movaps      %[x], %[xp]")       // xp = x
                __ASM_EMIT("movaps      %[X_ONE], %[tres]") // tres = 1
                __ASM_EMIT("movaps      %[x], %[tx]")       // tx = x
                __ASM_EMIT("mov         %[deg], %[tdeg]")   // tdeg = deg

                // Calculate tres = x ^ deg
                __ASM_EMIT("10:")
                    __ASM_EMIT("test        $1, %[tdeg]")       // if (tdeg & 1)
                    __ASM_EMIT("jz          2f")
                    __ASM_EMIT("mulss       %[tx], %[tres]")    // tres *= tx
                    __ASM_EMIT("dec         %[tdeg]")           // tdeg--
                    __ASM_EMIT("jnz         10b")
                    __ASM_EMIT("jmp         20f")
                    __ASM_EMIT("2:")
                    __ASM_EMIT("mulss       %[tx], %[tx]")      // tx = tx * tx
                    __ASM_EMIT("shr         $1, %[tdeg]")       // tdeg >>= 1
                    __ASM_EMIT("jnz         10b")
                __ASM_EMIT("20:")

                // Calculate x = k*x + a/tres
                __ASM_EMIT("movaps      %[a], %[tx]")           // tx = a
                __ASM_EMIT("mulss       %[k], %[x]")            // x = x*k
                __ASM_EMIT("divss       %[tres], %[tx]")        // tx = a/tres
                __ASM_EMIT("addss       %[tx], %[x]")           // x = x*k + a/tres

                // Estimate tolerance
                __ASM_EMIT("movaps      %[x], %[tx]")           // tx = x
                __ASM_EMIT("subss       %[x], %[xp]")           // xp = xp - x
                __ASM_EMIT("mulss       %[X_TOL], %[tx]")       // tx = x * TOL
                __ASM_EMIT("andps       %[sign], %[x]")         // xp = abs(xp - x)
                __ASM_EMIT("andps       %[sign], %[tx]")        // xp = abs(xp - x)
                __ASM_EMIT("ucomiss     %[tx], %[xp]")          // abs(xp - x) <> x * TOL
                __ASM_EMIT("ja          1b")                    // Repeat loop if tolerance is not enough

            __ASM_EMIT("100:")

            : [x] "+x" (x), [a] "=&x" (a), [tmp] "=&x" (tmp), [k] "=&x" (k),
              [sign] "=&x" (sign), [xp] "=&x" (xp), [tres] "=&x" (tres), [tx] "=&x" (tx),
              [deg] "+r" (deg), [tdeg] "=&r" (tdeg)
            : [X_SIGN] "m" (X_SIGN),
              [X_ONE] "m" (ONE),
              [X_TOL] "m" (X_3D_TOLERANCE)
            : "cc"
        );

        return x;

    //    // Check validity of arguments
    //    if (deg <= 1)
    //        return x;
    //
    //    // While root is odd, simple calc square root
    //    if (!(deg & 1))
    //    {
    //        do
    //        {
    //            x = sqrtf(x);
    //            deg >>= 1;
    //        } while (!(deg & 1));
    //
    //        if (deg <= 1)
    //            return x;
    //    }
    //
    //    // Newton method
    //    float xp;
    //    float a = 1.0f / float(deg);
    //    float k = float(--deg) * a;
    //    a *= x;
    //
    //    do
    //    {
    //        xp      = x;
    //        x       = k*x + a / ipospowf(x, deg);
    //    } while (fabs(x - xp) > (1e-5 * x));
    //
    //    return x;
    }
}

#endif /* INCLUDE_DSP_ARCH_X86_SSE_SMATH_H_ */
