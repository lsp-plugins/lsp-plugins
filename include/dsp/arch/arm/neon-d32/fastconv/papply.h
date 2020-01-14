/*
 * papply.h
 *
 *  Created on: 13 янв. 2020 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_ARM_NEON_D32_FASTCONV_PAPPLY_H_
#define DSP_ARCH_ARM_NEON_D32_FASTCONV_PAPPLY_H_

namespace neon_d32
{
    void fastconv_parse_apply(float *dst, float *tmp, const float *c, const float *src, size_t rank)
    {
        size_t items        = 1 << rank; // number of complex numbers per half of block (16 for rank=4)
        size_t n            = items >> 1; // number of butterflies per block (8 for rank=4)

        const float *fdw    = &XFFT_DW[(rank - 3) << 3];
        const float *fw     = &XFFT_A[(rank - 3) << 4];

        IF_ARCH_ARM(
            size_t k, p;
            float *a, *b, *ptr;
        );

        //---------------------------------------------------------------------
        // PARSE PART
        if (n > 4)
        {
            // First loop
            ARCH_ARM_ASM
            (
                __ASM_EMIT("mov         %[a], %[dst]")                  // a    = dst
                __ASM_EMIT("add         %[b], %[a], %[n], LSL $3")      // b    = &a[n*2]
                __ASM_EMIT("mov         %[k], %[n]")
                __ASM_EMIT("vldm        %[XFFT_A], {q8-q11}")           // q8   = wr0, q9 = wr1, q10 = wi0, q11 = wi1
                __ASM_EMIT("vldm        %[XFFT_DW], {q12-q13}")         // q12  = dr, q13 = di
                __ASM_EMIT("veor        q3, q3")                        // q3   = 0

                // 8x butterflies
                __ASM_EMIT("1:")
                __ASM_EMIT("vldm        %[src]!, {q0-q1}")              // q0   = ar0, q1 = ar1
                __ASM_EMIT("vmov        q2, q1")                        // q2   = ar1
                __ASM_EMIT("vmul.f32    q5, q0, q10")                   // q5   = ar0 * wi0
                __ASM_EMIT("vmul.f32    q7, q2, q11")                   // q7   = ar1 * wi1
                __ASM_EMIT("veor        q1, q1")                        // q1   = 0
                __ASM_EMIT("vmul.f32    q4, q0, q8")                    // q4   = ar0 * wr0 = br0
                __ASM_EMIT("vmul.f32    q6, q2, q9")                    // q6   = ar1 * wr1 = br1
                __ASM_EMIT("vneg.f32    q5, q5")                        // q5   = - ar0 * wi0 = bi0
                __ASM_EMIT("vneg.f32    q7, q7")                        // q7   = - ar1 * wi1 = bi1
                __ASM_EMIT("subs        %[k], $8")
                __ASM_EMIT("vstm        %[a]!, {q0-q3}")
                __ASM_EMIT("vstm        %[b]!, {q4-q7}")
                __ASM_EMIT("bls         2f")

                // Rotate angle
                __ASM_EMIT("vmul.f32    q4, q8, q13")                   // q4   = wr0 * di
                __ASM_EMIT("vmul.f32    q5, q9, q13")                   // q5   = wr1 * di
                __ASM_EMIT("vmul.f32    q6, q10, q13")                  // q6   = wi0 * di
                __ASM_EMIT("vmul.f32    q7, q11, q13")                  // q7   = wi1 * di
                __ASM_EMIT("vmul.f32    q8, q8, q12")                   // q8   = wr0 * dr
                __ASM_EMIT("vmul.f32    q9, q9, q12")                   // q9   = wr1 * dr
                __ASM_EMIT("vmul.f32    q10, q10, q12")                 // q10  = wi0 * dr
                __ASM_EMIT("vmul.f32    q11, q11, q12")                 // q11  = wi1 * dr
                __ASM_EMIT("vsub.f32    q8, q8, q6")                    // q8   = wr0*dr - wi0*di
                __ASM_EMIT("vsub.f32    q9, q9, q7")                    // q9   = wr1*dr - wi1*di
                __ASM_EMIT("vadd.f32    q10, q10, q4")                  // q10  = wi0*dr + wr0*di
                __ASM_EMIT("vadd.f32    q11, q11, q5")                  // q11  = wi1*dr + wr1*di
                __ASM_EMIT("b           1b")

                // Post-process
                __ASM_EMIT("2:")
                __ASM_EMIT("sub         %[XFFT_A], $0x40")              // fw  -= 16
                __ASM_EMIT("sub         %[XFFT_DW], $0x20")             // fdw -= 8
                __ASM_EMIT("lsr         %[n], $1")                      // n >>= 1

                : [src] "+r" (src),
                  [a] "=&r" (a), [b] "=&r" (b),
                  [XFFT_A] "+r" (fw), [XFFT_DW] "+r" (fdw),
                  [k] "=&r" (k), [n] "+r" (n)
                : [dst] "r" (tmp)
                : "cc", "memory",
                  "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
            );

            // All other loops while n > 4
            ARCH_ARM_ASM
            (
                __ASM_EMIT("cmp         %[n], $8")
                __ASM_EMIT("blo         8f")

                __ASM_EMIT("1:")
                __ASM_EMIT("mov         %[a], %[dst]")                  // a    = dst
                __ASM_EMIT("mov         %[p], %[items]")                // p    = items

                    __ASM_EMIT("3:")
                    __ASM_EMIT("add         %[b], %[a], %[n], LSL $3")      // b    = &a[n*2]
                    __ASM_EMIT("vldm        %[XFFT_A], {q8-q11}")           // q8   = wr0, q9 = wr1, q10 = wi0, q11 = wi1
                    __ASM_EMIT("mov         %[k], %[n]")
                        // 8x butterflies
                        __ASM_EMIT("5:")
                        __ASM_EMIT("vldm        %[a], {q0-q3}")                 // q0   = ar0, q1 = ai0, q2 = ar1, q3 = ai1
                        __ASM_EMIT("vldm        %[b], {q4-q7}")                 // q4   = br0, q5 = bi0, q6 = br1, q7 = bi1
                        __ASM_EMIT("vsub.f32    q12, q0, q4")                   // q12  = ar0 - br0 = cr0
                        __ASM_EMIT("vsub.f32    q13, q1, q5")                   // q13  = ai0 - bi0 = ci0
                        __ASM_EMIT("vsub.f32    q14, q2, q6")                   // q14  = ar1 - br1 = cr1
                        __ASM_EMIT("vsub.f32    q15, q3, q7")                   // q15  = ai1 - bi1 = ci1
                        __ASM_EMIT("vadd.f32    q0, q0, q4")                    // q0   = ar0 + br0 = ar0'
                        __ASM_EMIT("vadd.f32    q1, q1, q5")                    // q1   = ai0 + bi0 = ai0'
                        __ASM_EMIT("vadd.f32    q2, q2, q6")                    // q2   = ar1 + br1 = ar1'
                        __ASM_EMIT("vadd.f32    q3, q3, q7")                    // q3   = ai1 + bi1 = ai1'
                        __ASM_EMIT("vstm        %[a]!, {q0-q3}")
                        __ASM_EMIT("vmul.f32    q0, q8, q12")                   // q0   = wr0 * cr0
                        __ASM_EMIT("vmul.f32    q2, q9, q14")                   // q2   = wr1 * cr1
                        __ASM_EMIT("vmul.f32    q1, q8, q13")                   // q1   = wr0 * ci0
                        __ASM_EMIT("vmul.f32    q3, q9, q15")                   // q3   = wr1 * ci1
                        __ASM_EMIT("vmla.f32    q0, q10, q13")                  // q0   = wr0*cr0 + wi0*ci0 = br0'
                        __ASM_EMIT("vmla.f32    q2, q11, q15")                  // q2   = wr1*cr1 + wi1*ci1 = bi0'
                        __ASM_EMIT("vmls.f32    q1, q10, q12")                  // q1   = wr0*ci0 - wi0*cr0 = br1'
                        __ASM_EMIT("vmls.f32    q3, q11, q14")                  // q3   = wr1*ci1 - wi1*cr1 = bi1'
                        __ASM_EMIT("vstm        %[b]!, {q0-q3}")
                        __ASM_EMIT("subs        %[k], $8")
                        __ASM_EMIT("bls         6f")
                        // Rotate angle & repeat loop
                        __ASM_EMIT("vldm        %[XFFT_DW], {q0-q1}")           // q0   = dr, q1 = di
                        __ASM_EMIT("vmul.f32    q12, q8, q1")                   // q12  = wr0 * di
                        __ASM_EMIT("vmul.f32    q13, q9, q1")                   // q13  = wr1 * di
                        __ASM_EMIT("vmul.f32    q14, q10, q1")                  // q14  = wi0 * di
                        __ASM_EMIT("vmul.f32    q15, q11, q1")                  // q15  = wi1 * di
                        __ASM_EMIT("vmul.f32    q8, q8, q0")                    // q8   = wr0 * dr
                        __ASM_EMIT("vmul.f32    q9, q9, q0")                    // q9   = wr1 * dr
                        __ASM_EMIT("vmul.f32    q10, q10, q0")                  // q10  = wi0 * dr
                        __ASM_EMIT("vmul.f32    q11, q11, q0")                  // q11  = wi1 * dr
                        __ASM_EMIT("vsub.f32    q8, q8, q14")                   // q8   = wr0*dr - wi0*di
                        __ASM_EMIT("vsub.f32    q9, q9, q15")                   // q9   = wr1*dr - wi1*di
                        __ASM_EMIT("vadd.f32    q10, q10, q12")                 // q10  = wi0*dr + wr0*di
                        __ASM_EMIT("vadd.f32    q11, q11, q13")                 // q11  = wi1*dr + wr1*di
                        __ASM_EMIT("b           5b")
                    __ASM_EMIT("6:")
                    __ASM_EMIT("mov         %[a], %[b]")                        // a    = b
                    __ASM_EMIT("subs        %[p], %[n], LSL $1")                // p   -= n*2
                    __ASM_EMIT("bne         3b")

                __ASM_EMIT("lsr         %[n], $1")                      // n >>= 1
                __ASM_EMIT("sub         %[XFFT_A], $0x40")              // fw  -= 16
                __ASM_EMIT("sub         %[XFFT_DW], $0x20")             // fdw -= 8
                __ASM_EMIT("cmp         %[n], $8")
                __ASM_EMIT("bge         1b")

                // Loop for n=4
                __ASM_EMIT("8:")
                __ASM_EMIT("mov         %[a], %[dst]")                  // a    = dst
                __ASM_EMIT("mov         %[p], %[items]")                // p    = items
                __ASM_EMIT("vldm        %[XFFT_A], {q8-q11}")           // q8   = wr0, q9 = wr1, q10 = wi0, q11 = wi1

                // 8x butterflies
                __ASM_EMIT("9:")
                __ASM_EMIT("vldm        %[a], {q0-q7}")                 // q0   = ar0, q1 = ai0, q2 = br0, q3 = bi0, q4 = ar1, q5 = ai1, q6 = br1, q7 = bi1
                __ASM_EMIT("vsub.f32    q12, q0, q2")                   // q12  = ar0 - br0 = cr0
                __ASM_EMIT("vsub.f32    q13, q1, q3")                   // q13  = ai0 - bi0 = ci0
                __ASM_EMIT("vsub.f32    q14, q4, q6")                   // q14  = ar1 - br1 = cr1
                __ASM_EMIT("vsub.f32    q15, q5, q7")                   // q15  = ai1 - bi1 = ci1

                __ASM_EMIT("vadd.f32    q0, q0, q2")                    // q0   = ar0 + br0 = ar0'
                __ASM_EMIT("vadd.f32    q1, q1, q3")                    // q1   = ai0 + bi0 = ai0'
                __ASM_EMIT("vadd.f32    q4, q4, q6")                    // q4   = ar1 + br1 = ar1'
                __ASM_EMIT("vadd.f32    q5, q5, q7")                    // q5   = ai1 + bi1 = ai1'

                __ASM_EMIT("vmul.f32    q2, q8, q12")                   // q2   = wr0 * cr0
                __ASM_EMIT("vmul.f32    q6, q9, q14")                   // q6   = wr1 * cr1
                __ASM_EMIT("vmul.f32    q3, q8, q13")                   // q3   = wr0 * ci0
                __ASM_EMIT("vmul.f32    q7, q9, q15")                   // q7   = wr1 * ci1

                __ASM_EMIT("vmla.f32    q2, q10, q13")                  // q2   = wr0*cr0 + wi0*ci0 = br0'
                __ASM_EMIT("vmla.f32    q6, q11, q15")                  // q6   = wr1*cr1 + wi1*ci1 = br1'
                __ASM_EMIT("vmls.f32    q3, q10, q12")                  // q3   = wr0*ci0 - wi0*cr0 = bi0'
                __ASM_EMIT("vmls.f32    q7, q11, q14")                  // q7   = wr1*ci1 - wi1*cr1 = bi1'
                __ASM_EMIT("vstm        %[a]!, {q0-q7}")
                __ASM_EMIT("subs        %[p], $16")                     // p   -= 16
                __ASM_EMIT("bne         9b")

                : [a] "=&r" (a), [b] "=&r" (b),
                  [k] "=&r" (k), [p] "=&r" (p),
                  [XFFT_A] "+r" (fw), [XFFT_DW] "+r" (fdw),
                  [n] "+r" (n)
                : [dst] "r" (tmp), [items] "r" (items)
                : "cc", "memory",
                  "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
            );
        }
        else
        {
            ARCH_ARM_ASM
            (
                __ASM_EMIT("vldm        %[src], {q0}")
                __ASM_EMIT("veor        q1, q1")
                __ASM_EMIT("vstm        %[dst], {q0-q1}")
                :
                : [src] "r" (src), [dst] "r" (tmp)
                : "memory",
                  "q0", "q1"
            );
        }

        //---------------------------------------------------------------------
        // MIDDLE PART
        ARCH_ARM_ASM
        (
            // Loop for n=4
            __ASM_EMIT("subs        %[k], %[items], $8")
            __ASM_EMIT("blo         2f")

            // 8x butterflies
            __ASM_EMIT("mov         %[src], %[tmp]")
            __ASM_EMIT("mov         %[dst], %[tmp]")

            __ASM_EMIT("1:")
            __ASM_EMIT("vld4.32     {q4-q5}, [%[src]]!")    // q4   = r0 i0 r1 i1, q5 = r2 i2 r3 i3
            __ASM_EMIT("vld4.32     {q6-q7}, [%[src]]!")    // q6   = r4 i4 r5 i5, q7 = r6 i6 r7 i7
            // q4 = r0 i0 r1 i1
            // q5 = r2 i2 r3 i3
            // q6 = r4 i4 r5 i5
            // q7 = r6 i6 r7 i7
            __ASM_EMIT("vsub.f32    q0, q4, q5")            // q0   = r0-r2 i0-i2 r1-r3 i1-i3 = r1' i1' r3' i3'
            __ASM_EMIT("vsub.f32    q1, q6, q7")            // q1   = r4-r6 i4-i6 r5-r7 i5-i7 = r5' i5' r7' i7'
            __ASM_EMIT("vadd.f32    q4, q4, q5")            // q4   = r0+r2 i0+i2 r1+r3 i1+i3 = r0' i0' r2' i2'
            __ASM_EMIT("vadd.f32    q6, q6, q7")            // q6   = r4+r6 i4+i6 r5+r7 i5+i7 = r4' i4' r6' i6'

            // q4 = r0' i0' r2' i2'
            // q6 = r4' i4' r6' i6'
            // q0 = r1' i1' r3' i3'
            // q1 = r5' i5' r7' i7'
            __ASM_EMIT("vzip.32     q4, q0")                // q4   = r0' r1' i0' i1', q0 = r2' r3' i2' i3'
            __ASM_EMIT("vzip.32     q6, q1")                // q6   = r4' r5' i4' i5', q1 = r6' r7' i6' i7'
            __ASM_EMIT("vrev64.32   q0, q0")                // q0   = r3' r2' i3' i2'
            __ASM_EMIT("vrev64.32   q1, q1")                // q1   = r7' r6' i7' i6'
            __ASM_EMIT("vext.32     q0, q0, $1")            // q0   = r2' i3' i2' r3'
            __ASM_EMIT("vext.32     q1, q1, $1")            // q1   = r6' i7' i6' r7'
            // q4 = r0' r1' i0' i1'
            // q6 = r4' r5' i4' i5'
            // q0 = r2' i3' i2' r3'
            // q1 = r6' i7' i6' r7'
            __ASM_EMIT("vsub.f32    q5, q4, q0")            // q5   = r0'-r2' r1'-i3' i0'-i2' i1'-r3' = r1" r3" i1" i2"
            __ASM_EMIT("vsub.f32    q7, q6, q1")            // q7   = r4'-r6' r5'-i7' i4'-i6' i5'-r7' = r5" r7" i5" i6"
            __ASM_EMIT("vadd.f32    q4, q4, q0")            // q4   = r0'+r2' r1'+i3' i0'+i2' i1'+r3' = r0" r2" i0" i3"
            __ASM_EMIT("vadd.f32    q6, q6, q1")            // q6   = r4'+r6' r5'+i7' i4'+i6' i5'+r7' = r4" r6" i4" i7"
            // q4 = r0" r2" i0" i3"
            // q5 = r1" r3" i1" i2"
            // q6 = r4" r6" i4" i7"
            // q7 = r5" r7" i5" i6"
            __ASM_EMIT("vzip.32     q4, q5")                // q4   = r0" r1" r2" r3", q5 = i0" i1" i3" i2"
            __ASM_EMIT("vzip.32     q6, q7")                // q6   = r4" r5" r6" r7", q7 = i4" i5" i7" i6"
            __ASM_EMIT("vswp        d11, d14")              // q5   = i0" i1" i4" i5", q6 = i3" i2" i7" i6"
            __ASM_EMIT("vldm        %[conv]!, {q12-q15}")   // q12  = br0, q13 = bi0, q14 = br1, q15 = bi1
            __ASM_EMIT("vrev64.32   q7, q7")                // q7   = i2" i3" i6" i7"
            __ASM_EMIT("vswp        d11, d14")              // q5   = i0" i1" i2" i3", q7 = i4" i5" i6" i7"
            // q4 = ar0, q5 = ai0, q6 = ar1, q7 = ai1
            // q12 = br0, q13 = bi0, q14 = br1, q15 = bi1
            __ASM_EMIT("vmul.f32    q0, q4, q12")           // q0 = ar0 * br0
            __ASM_EMIT("vmul.f32    q2, q6, q14")           // q2 = ar1 * br1
            __ASM_EMIT("vmul.f32    q1, q4, q13")           // q1 = ar0 * bi0
            __ASM_EMIT("vmul.f32    q3, q6, q15")           // q3 = ar1 * bi1
            __ASM_EMIT("vmls.f32    q0, q5, q13")           // q0 = ar0*br0 - ai0*bi0
            __ASM_EMIT("vmls.f32    q2, q7, q15")           // q2 = ar1*br1 - ai1*bi1
            __ASM_EMIT("vmla.f32    q1, q5, q12")           // q1 = ar0*bi0 + ai0*br0
            __ASM_EMIT("vmla.f32    q3, q7, q14")           // q3 = ar1*bi1 + ai1*br1

            // q0 = r0 r1 r2 r3
            // q1 = i0 i1 i2 i3
            // q2 = r4 r5 r6 r7
            // q3 = i4 i5 i6 i7
            __ASM_EMIT("vtrn.32     q0, q1")                // q0 = r0 i0 r2 i2, q1 = r1 i1 r3 i3
            __ASM_EMIT("vtrn.32     q2, q3")                // q2 = r4 i4 r6 i6, q3 = r5 i5 r7 i7

            __ASM_EMIT("vadd.f32    q4, q0, q1")            // q4 = r0+r1 i0+i1 r2+r3 i2+i3 = r0' i0' r2' i2'
            __ASM_EMIT("vadd.f32    q5, q2, q3")            // q5 = r4+r5 i4+i5 r6+r7 i6+i7 = r4' i4' r6' i6'
            __ASM_EMIT("vsub.f32    q0, q0, q1")            // q0 = r0-r1 i0-i1 r2-r3 i2-i3 = r1' i1' r3' i3'
            __ASM_EMIT("vsub.f32    q1, q2, q3")            // q1 = r4-r5 i4-i5 r6-r7 i6-i7 = r5' i5' r7' i7'

            // q4 = r0' i0' r2' i2'
            // q5 = r4' i4' r6' i6'
            // q0 = r1' i1' r3' i3'
            // q1 = r5' i5' r7' i7'
            __ASM_EMIT("vswp        d9, d10")               // q4 = r0' i0' r4' i4', q5 = r2' i2' r6' i6'
            __ASM_EMIT("vswp        d1, d2")                // q0 = r1' i1' r5' i5', q1 = r3' i3' r7' i7'
            __ASM_EMIT("vrev64.32   q1, q1")                // q1 = i3' r3' i7' r7'

            __ASM_EMIT("vsub.f32    q2, q4, q5")            // q2 = r0'-r2' i0'-i2' r4'-r6' i4'-i6' = r2" i2" r6" i6"
            __ASM_EMIT("vadd.f32    q3, q0, q1")            // q3 = r1'+i3' i1'+r3' r5'+i7' i5'+r7' = r3" i1" r7" i5"
            __ASM_EMIT("vsub.f32    q1, q0, q1")            // q1 = r1'-i3' i1'-r3' r5'-i7' i5'-r7' = r1" i3" r5" i7"
            __ASM_EMIT("vadd.f32    q0, q4, q5")            // q0 = r0'+r2' i0'+i2' r4'+r6' i4'+i6' = r0" i0" r4" i4"

            // q0 = r0" i0" r4" i4"
            // q1 = r1" i3" r5" i7"
            // q2 = r2" i2" r6" i6"
            // q3 = r3" i1" r7" i5"
            __ASM_EMIT("vtrn.32     q0, q1")                // q0 = r0" r1" r4" r5", q1 = i0" i3" i4" i7"
            __ASM_EMIT("vtrn.32     q2, q3")                // q2 = r2" r3" r6" r7", q3 = i2" i1" i6" i5"
            __ASM_EMIT("vswp        d1, d4")                // q0 = r0" r1" r2" r3", q2 = r4" r5" r6" r7"
            __ASM_EMIT("vswp        d3, d6")                // q1 = i0" i3" i2" i1", q3 = i4" i7" i6" i5"

            __ASM_EMIT("vrev64.32   q1, q1")                // q1 = i3" i0" i1" i2"
            __ASM_EMIT("vrev64.32   q3, q3")                // q3 = i7" i4" i5" i6"
            __ASM_EMIT("vext.32     q1, q1, q1, $1")        // q1 = i0" i1" i2" i3"
            __ASM_EMIT("vext.32     q3, q3, q3, $1")        // q3 = i4" i5" i6" i7"

            __ASM_EMIT("subs        %[k], $8")              // k   -= 8
            __ASM_EMIT("vstm        %[dst]!, {q0-q3}")
            __ASM_EMIT("bhs         1b")

            __ASM_EMIT("2:")

            : [src] "+r" (src), [dst] "=&r" (ptr), [conv] "+r" (c),
              [k] "=&r" (k)
            : [items] "r" (items), [tmp] "r" (tmp)
            : "cc", "memory",
              "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );

        //---------------------------------------------------------------------
        // RESTORE PART
        n = 8;
        fw          = XFFT_A;

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
                  [XFFT_A] "+r" (fw)
                : [tmp] "r" (tmp), [items] "r" (items)
                : "cc", "memory",
                  "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
            );

            n <<= 1;
        }

        fdw         = &XFFT_DW[8];

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
                  [XFFT_A] "r" (fw), [XFFT_W] "r" (fdw)
                : "cc", "memory",
                  "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
            );

            n <<= 1;
            fw         += 16;
            fdw        += 8;
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
            __ASM_EMIT("vldm        %[da], {q0-q1}")                // q0   = dar1, q1 = dar2
            __ASM_EMIT("vldm        %[db], {q2-q3}")                // q2   = dbr1, q3 = dbr2
            __ASM_EMIT("vmla.f32    q0, q4, q14")                   // q0   = dar1 + (ar1 + cr1) / items
            __ASM_EMIT("vmla.f32    q1, q5, q15")                   // q1   = dar2 + (ar2 + cr2) / items
            __ASM_EMIT("vmla.f32    q2, q6, q14")                   // q2   = dbr1 + (ar1 - cr1) / items
            __ASM_EMIT("vmla.f32    q3, q7, q15")                   // q3   = dbr2 + (ar2 - cr2) / items
            __ASM_EMIT("subs        %[items], $16")
            __ASM_EMIT("vstm        %[da]!, {q0-q1}")
            __ASM_EMIT("vstm        %[db]!, {q2-q3}")
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
              [XFFT_A] "r" (fw), [XFFT_W] "r" (fdw)
            : "cc", "memory",
              "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }
}

#endif /* DSP_ARCH_ARM_NEON_D32_FASTCONV_PAPPLY_H_ */
