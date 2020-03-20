/*
 * restore.h
 *
 *  Created on: 13 янв. 2020 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_ARM_NEON_D32_FASTCONV_RESTORE_H_
#define DSP_ARCH_ARM_NEON_D32_FASTCONV_RESTORE_H_

#ifndef DSP_ARCH_ARM_NEON_32_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_ARM_NEON_32_IMPL */

namespace neon_d32
{
    void fastconv_restore(float *dst, float *tmp, size_t rank)
    {
        size_t items        = 1 << rank; // number of complex numbers per half of block (16 for rank=4)
        size_t n;

        IF_ARCH_ARM(
            float *a, *b;
            size_t k, p;
        );

        ARCH_ARM_ASM
        (
            __ASM_EMIT("mov         %[a], %[tmp]")
            __ASM_EMIT("mov         %[b], %[tmp]")
            __ASM_EMIT("subs        %[n], %[items], $8")
            __ASM_EMIT("blo         2f")

            __ASM_EMIT("1:")
            __ASM_EMIT("vld2.32     {q0-q1}, [%[a]]!")      // q0 = r0 r2 i0 i2, q1 = r1 r3 i1 i3
            __ASM_EMIT("vld2.32     {q2-q3}, [%[a]]!")      // q2 = r4 r6 i4 i6, q3 = r5 r7 i5 i7

            __ASM_EMIT("vadd.f32    q4, q0, q1")            // q4 = r0+r1 r2+r3 i0+i1 i2+i3 = r0' r2' i0' i2'
            __ASM_EMIT("vadd.f32    q5, q2, q3")            // q5 = r4+r5 r6+r7 i4+i5 i6+i7 = r4' r6' i4' i6'
            __ASM_EMIT("vsub.f32    q0, q0, q1")            // q0 = r0-r1 r2-r3 i0-i1 i2-i3 = r1' r3' i1' i3'
            __ASM_EMIT("vsub.f32    q1, q2, q3")            // q1 = r4-r5 r6-r7 i4-i5 i6-i7 = r5' r7' i5' i7'

            // q4 = r0' r2' i0' i2'
            // q5 = r4' r6' i4' i6'
            // q0 = r1' r3' i1' i3'
            // q1 = r5' r7' i5' i7'
            __ASM_EMIT("vtrn.32     q4, q5")                // q4 = r0' r4' i0' i4', q5 = r2' r6' i2' i6'
            __ASM_EMIT("vtrn.32     q0, q1")                // q0 = r1' r5' i1' i5', q1 = r3' r7' i3' i7'
            __ASM_EMIT("vext.32     q6, q1, q1, $2")        // q6 = i3' i7' r3' r7'

            __ASM_EMIT("vsub.f32    q2, q0, q6")            // q2 = r1'-i3' r5'-i7' i1'-r3' i5'-r7' = r1" r5" i3" i7"
            __ASM_EMIT("vsub.f32    q1, q4, q5")            // q1 = r0'-r2' r4'-r6' i0'-i2' i4'-i6' = r2" r6" i2" i6"
            __ASM_EMIT("vadd.f32    q3, q0, q6")            // q3 = r1'+i3' r5'+i7' i1'+r3' i5'+r7' = r3" r7" i1" i5"
            __ASM_EMIT("vadd.f32    q0, q4, q5")            // q0 = r0'+r2' r4'+r6' i0'+i2' i4'+i6' = r0" r4" i0" i4"

            // q0 = r0" r4" i0" i4"
            // q1 = r2" r6" i2" i6"
            // q2 = r1" r5" i3" i7"
            // q3 = r3" r7" i1" i5"
            __ASM_EMIT("vtrn.32     q0, q2")                // q0 = r0" r1" i0" i3", q2 = r4" r5" i4" i7"
            __ASM_EMIT("vtrn.32     q1, q3")                // q1 = r2" r3" i2" i1", q3 = r6" r7" i6" i5"
            __ASM_EMIT("vswp        d1, d2")                // q0 = r0" r1" r2" r3", q1 = i0" i3" i2" i1"
            __ASM_EMIT("vswp        d5, d6")                // q2 = r4" r5" r6" r7", q3 = i4" i7" i6" i5"

            __ASM_EMIT("vrev64.32   q1, q1")                // q1 = i3" i0" i1" i2"
            __ASM_EMIT("vrev64.32   q3, q3")                // q3 = i7" i4" i5" i6"
            __ASM_EMIT("vext.32     q1, q1, q1, $1")        // q1 = i0" i1" i2" i3"
            __ASM_EMIT("vext.32     q3, q3, q3, $1")        // q3 = i4" i5" i6" i7"

            __ASM_EMIT("subs        %[n], $8")              // n   -= 8
            __ASM_EMIT("vstm        %[b]!, {q0-q3}")
            __ASM_EMIT("bhs         1b")
            __ASM_EMIT("2:")

            : [a] "=&r" (a), [b] "=&r" (b),
              [n] "=&r" (n)
            : [tmp] "r" (tmp), [items] "r" (items)
            : "cc", "memory",
              "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7"
        );

        n = 8;
        const float *xfft_a     = XFFT_A;

        // Perform butterflies without need of vector rotation
        if (n < items)
        {
            ARCH_ARM_ASM
            (
                __ASM_EMIT("mov         %[a], %[tmp]")
                __ASM_EMIT("subs        %[k], %[items], $16")
                __ASM_EMIT("vldm        %[XFFT_A]!, {q8-q11}")   // q8   = wr1, q9 = wr2, q10 = wi1, q11 = wi2
                __ASM_EMIT("blo         2f")
                // 8x butterflies
                // Calculate complex c = w * b
                __ASM_EMIT("1:")
                // Calc cr and ci
                __ASM_EMIT("vldm        %[a], {q0-q7}")     // q0   = ar1, q1 = ai1, q2 = br1, q3 = bi1, q4 = ar2, q5 = ai2, q6 = br2, q7 = bi2
                __ASM_EMIT("vmul.f32    q12, q8, q2")       // q12  = wr1 * br1
                __ASM_EMIT("vmul.f32    q13, q9, q6")       // q13  = wr2 * br2
                __ASM_EMIT("vmul.f32    q14, q8, q3")       // q14  = wr1 * bi1
                __ASM_EMIT("vmul.f32    q15, q9, q7")       // q15  = wr2 * bi2
                __ASM_EMIT("vmls.f32    q12, q10, q3")      // q12  = wr1 * br1 - wi1 * bi1 = cr1
                __ASM_EMIT("vmls.f32    q13, q11, q7")      // q13  = wr2 * br2 - wi2 * bi2 = cr2
                __ASM_EMIT("vmla.f32    q14, q10, q2")      // q14  = wr1 * bi1 + wi1 * br1 = ci1
                __ASM_EMIT("vmla.f32    q15, q11, q6")      // q15  = wr1 * bi1 + wi2 * br2 = ci2
                // Apply butterfly
                __ASM_EMIT("vsub.f32    q2, q0, q12")       // q2   = ar1 - cr1
                __ASM_EMIT("vsub.f32    q3, q1, q14")       // q3   = ai1 - ci1
                __ASM_EMIT("vsub.f32    q6, q4, q13")       // q6   = ar2 - cr2
                __ASM_EMIT("vsub.f32    q7, q5, q15")       // q7   = ai2 - ci2
                __ASM_EMIT("vadd.f32    q0, q0, q12")       // q0   = ar1 + cr1
                __ASM_EMIT("vadd.f32    q1, q1, q14")       // q1   = ai1 + ci1
                __ASM_EMIT("vadd.f32    q4, q4, q13")       // q4   = ar2 + cr2
                __ASM_EMIT("vadd.f32    q5, q5, q15")       // q5   = ai2 + ci2
                __ASM_EMIT("vstm        %[a]!, {q0-q7}")
                __ASM_EMIT("subs        %[k], $16")
                __ASM_EMIT("bge         1b")

                __ASM_EMIT("2:")
                __ASM_EMIT("adds        %[k], $8")
                __ASM_EMIT("blo         4f")
                // 4x butterflies
                __ASM_EMIT("vldm        %[a], {q0-q3}")     // q0   = ar1, q1 = ai1, q2 = br1, q3 = bi1
                __ASM_EMIT("vmul.f32    q12, q8, q2")       // q12  = wr1 * br1
                __ASM_EMIT("vmul.f32    q14, q8, q3")       // q14  = wr1 * bi1
                __ASM_EMIT("vmls.f32    q12, q10, q3")      // q12  = wr1 * br1 - wi1 * bi1 = cr1
                __ASM_EMIT("vmla.f32    q14, q10, q2")      // q14  = wr1 * bi1 + wi1 * br1 = ci1
                // Apply butterfly
                __ASM_EMIT("vsub.f32    q2, q0, q12")       // q2   = ar1 - cr1
                __ASM_EMIT("vsub.f32    q3, q1, q14")       // q3   = ai1 - ci1
                __ASM_EMIT("vadd.f32    q0, q0, q12")       // q0   = ar1 + cr1
                __ASM_EMIT("vadd.f32    q1, q1, q14")       // q1   = ai1 + ci1
                __ASM_EMIT("vstm        %[a]!, {q0-q3}")
                __ASM_EMIT("4:")

                : [a] "=&r" (a), [k] "=&r" (k),
                  [XFFT_A] "+r" (xfft_a)
                : [tmp] "r" (tmp), [items] "r" (items)
                : "cc", "memory",
                  "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
            );

            n <<= 1;
        }

        const float *xfft_dw    = &XFFT_DW[8];

        // Perform loops except last one
        while (n < items)
        {
            ARCH_ARM_ASM(
                __ASM_EMIT("mov         %[k], %[items]")
                __ASM_EMIT("mov         %[a], %[dst]")
                __ASM_EMIT("1:")
                    // Initialize sub-loop
                    __ASM_EMIT("mov         %[p], %[n]")
                    __ASM_EMIT("vldm        %[XFFT_A], {q8-q11}")           // q8   = wr1, q9 = wr2, q10 = wi1, q11 = wi2
                    __ASM_EMIT("add         %[b], %[a], %[n], LSL $2")      // b    = &a[n*4]
                    // 8x butterflies
                    __ASM_EMIT("3:")
                    // Calc cr and ci
                    __ASM_EMIT("vldm        %[b], {q4-q7}")                 // q4   = br1, q5 = bi1, q6 = br2, q7 = bi2
                    __ASM_EMIT("vldm        %[a], {q0-q3}")                 // q0   = ar1, q1 = ai1, q2 = ar2, q3 = ai2
                    __ASM_EMIT("vmul.f32    q12, q8, q4")                   // q12  = wr1 * br1
                    __ASM_EMIT("vmul.f32    q13, q9, q6")                   // q13  = wr2 * br2
                    __ASM_EMIT("vmul.f32    q14, q8, q5")                   // q14  = wr1 * bi1
                    __ASM_EMIT("vmul.f32    q15, q9, q7")                   // q15  = wr2 * bi2
                    __ASM_EMIT("vmls.f32    q12, q10, q5")                  // q12  = wr1 * br1 - wi1 * bi1 = cr1
                    __ASM_EMIT("vmls.f32    q13, q11, q7")                  // q13  = wr2 * br2 - wi2 * bi2 = cr2
                    __ASM_EMIT("vmla.f32    q14, q10, q4")                  // q14  = wr1 * bi1 + wi1 * br1 = ci1
                    __ASM_EMIT("vmla.f32    q15, q11, q6")                  // q15  = wr1 * bi1 + wi2 * br2 = ci2
                    // Apply butterfly
                    __ASM_EMIT("vsub.f32    q4, q0, q12")                   // q4   = ar1 - cr1
                    __ASM_EMIT("vsub.f32    q5, q1, q14")                   // q5   = ai1 - ci1
                    __ASM_EMIT("vsub.f32    q6, q2, q13")                   // q6   = ar2 - cr2
                    __ASM_EMIT("vsub.f32    q7, q3, q15")                   // q7   = ai2 - ci2
                    __ASM_EMIT("vadd.f32    q0, q0, q12")                   // q0   = ar1 + cr1
                    __ASM_EMIT("vadd.f32    q1, q1, q14")                   // q1   = ai1 + ci1
                    __ASM_EMIT("vadd.f32    q2, q2, q13")                   // q2   = ar2 + cr2
                    __ASM_EMIT("vadd.f32    q3, q3, q15")                   // q3   = ai2 + ci2
                    __ASM_EMIT("vstm        %[b]!, {q4-q7}")
                    __ASM_EMIT("subs        %[p], $16")
                    __ASM_EMIT("vstm        %[a]!, {q0-q3}")
                    __ASM_EMIT("beq         4f")
                    // Prepare next loop: rotate angle
                    __ASM_EMIT("vldm        %[XFFT_W], {q0-q1}")            // q0   = dr, q1 = di
                    __ASM_EMIT("vmul.f32    q12, q8, q1")                   // q12  = wr1 * di
                    __ASM_EMIT("vmul.f32    q13, q9, q1")                   // q13  = wr2 * di
                    __ASM_EMIT("vmul.f32    q14, q10, q1")                  // q14  = wi1 * di
                    __ASM_EMIT("vmul.f32    q15, q11, q1")                  // q15  = wi2 * di
                    __ASM_EMIT("vmul.f32    q8, q8, q0")                    // q8   = wr1 * dr
                    __ASM_EMIT("vmul.f32    q9, q9, q0")                    // q9   = wr2 * dr
                    __ASM_EMIT("vmul.f32    q10, q10, q0")                  // q10  = wi1 * dr
                    __ASM_EMIT("vmul.f32    q11, q11, q0")                  // q11  = wi2 * dr
                    __ASM_EMIT("vsub.f32    q8, q8, q14")                   // q8   = wr1*dr - wi1*di
                    __ASM_EMIT("vsub.f32    q9, q9, q15")                   // q9   = wr2*dr - wi2*di
                    __ASM_EMIT("vadd.f32    q10, q10, q12")                 // q10  = wi1*dr + wr1*di
                    __ASM_EMIT("vadd.f32    q11, q11, q13")                 // q11  = wi2*dr + wr2*di
                    __ASM_EMIT("b           3b")
                __ASM_EMIT("4:")
                __ASM_EMIT("subs        %[k], %[n]")
                __ASM_EMIT("mov         %[a], %[b]")
                __ASM_EMIT("bne         1b")

                : [a] "=&r" (a), [b] "=&r" (b),
                  [p] "=&r" (p), [k] "=&r" (k)
                : [dst] "r" (tmp), [items] "r" (items), [n] "r" (n),
                  [XFFT_A] "r" (xfft_a), [XFFT_W] "r" (xfft_dw)
                : "cc", "memory",
                  "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
            );

            n <<= 1;
            xfft_a     += 16;
            xfft_dw    += 8;
        }

        // Last one large loop
        IF_ARCH_ARM(float fftk = 1.0f / items);

        ARCH_ARM_ASM(
            // Initialize sub-loop
            __ASM_EMIT("add         %[b], %[a], %[n], LSL $2")      // b    = &a[n*4]
            __ASM_EMIT("vld1.32     {d30[], d31[]}, [%[fftk]]")     // q15  = 1 / items
            __ASM_EMIT("add         %[db], %[da], %[n], LSL $1")    // db   = &da[n*2]
            __ASM_EMIT("vldm        %[XFFT_A], {q8-q11}")           // q8   = wr1, q9 = wr2, q10 = wi1, q11 = wi2
            __ASM_EMIT("vmov        q14, q15")                      // q14  = 1 / items
            // 8x butterflies
            __ASM_EMIT("1:")
            // Calc cr, do not need ci
            __ASM_EMIT("vldm        %[b]!, {q4-q7}")                // q4   = br1, q5 = bi1, q6 = br2, q7 = bi2
            __ASM_EMIT("vldm        %[a]!, {q0-q3}")                // q0   = ar1, q1 = ai1, q2 = ar2, q3 = ai2
            __ASM_EMIT("vmul.f32    q12, q8, q4")                   // q12  = wr1 * br1
            __ASM_EMIT("vmul.f32    q13, q9, q6")                   // q13  = wr2 * br2
            __ASM_EMIT("vmls.f32    q12, q10, q5")                  // q12  = wr1 * br1 - wi1 * bi1 = cr1
            __ASM_EMIT("vmls.f32    q13, q11, q7")                  // q13  = wr2 * br2 - wi2 * bi2 = cr2
            // Apply butterfly, we do not need imaginary part
            __ASM_EMIT("vadd.f32    q4, q0, q12")                   // q4   = ar1 + cr1
            __ASM_EMIT("vadd.f32    q5, q2, q13")                   // q5   = ar2 + cr2
            __ASM_EMIT("vsub.f32    q6, q0, q12")                   // q6   = ar1 - cr1
            __ASM_EMIT("vsub.f32    q7, q2, q13")                   // q7   = ar2 - cr2
            __ASM_EMIT("vmul.f32    q4, q14")                       // q4   = (ar1 + cr1) / items
            __ASM_EMIT("vmul.f32    q5, q15")                       // q5   = (ar2 + cr2) / items
            __ASM_EMIT("vmul.f32    q6, q14")                       // q6   = (ar1 - cr1) / items
            __ASM_EMIT("vmul.f32    q7, q15")                       // q7   = (ar2 - cr2) / items
            __ASM_EMIT("subs        %[items], $16")
            __ASM_EMIT("vstm        %[db]!, {q6-q7}")
            __ASM_EMIT("vstm        %[da]!, {q4-q5}")
            __ASM_EMIT("beq         2f")
            // Prepare next loop: rotate angle
            __ASM_EMIT("vldm        %[XFFT_W], {q0-q1}")            // q0   = dr, q1 = di
            __ASM_EMIT("vmul.f32    q2, q8, q1")                    // q2   = wr1 * di
            __ASM_EMIT("vmul.f32    q3, q9, q1")                    // q3   = wr2 * di
            __ASM_EMIT("vmul.f32    q4, q10, q1")                   // q4   = wi1 * di
            __ASM_EMIT("vmul.f32    q5, q11, q1")                   // q5   = wi2 * di
            __ASM_EMIT("vmul.f32    q8, q8, q0")                    // q8   = wr1 * dr
            __ASM_EMIT("vmul.f32    q9, q9, q0")                    // q9   = wr2 * dr
            __ASM_EMIT("vmul.f32    q10, q10, q0")                  // q10  = wi1 * dr
            __ASM_EMIT("vmul.f32    q11, q11, q0")                  // q11  = wi2 * dr
            __ASM_EMIT("vsub.f32    q8, q8, q4")                    // q8   = wr1*dr - wi1*di
            __ASM_EMIT("vsub.f32    q9, q9, q5")                    // q9   = wr2*dr - wi2*di
            __ASM_EMIT("vadd.f32    q10, q10, q2")                  // q10  = wi1*dr + wr1*di
            __ASM_EMIT("vadd.f32    q11, q11, q3")                  // q11  = wi2*dr + wr2*di
            __ASM_EMIT("b           1b")
            __ASM_EMIT("2:")

            : [da] "+r" (dst), [db] "=&r" (a),
              [a] "+r" (tmp), [b] "=&r" (b),
              [items] "+r" (items)
            : [n] "r" (n), [fftk] "r" (&fftk),
              [XFFT_A] "r" (xfft_a), [XFFT_W] "r" (xfft_dw)
            : "cc", "memory",
              "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }
}



#endif /* DSP_ARCH_ARM_NEON_D32_FASTCONV_RESTORE_H_ */
