/*
 * papply.h
 *
 *  Created on: 14 янв. 2020 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_AARCH64_ASIMD_FASTCONV_PAPPLY_H_
#define DSP_ARCH_AARCH64_ASIMD_FASTCONV_PAPPLY_H_

#ifndef DSP_ARCH_AARCH64_ASIMD_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_AARCH64_ASIMD_IMPL */

namespace asimd
{
    void fastconv_parse_apply(float *dst, float *tmp, const float *conv, const float *src, size_t rank)
    {
        size_t items        = 1 << rank; // number of complex numbers per half of block (16 for rank=4)
        size_t n            = items >> 1; // number of butterflies per block (8 for rank=4)

        IF_ARCH_AARCH64(
            const float *fdw    = &XFFT_DW[(rank - 3) << 3];
            const float *fw     = &XFFT_A[(rank - 3) << 4];
            size_t k, p;
            float *a, *b, *ptr;
        );

        //---------------------------------------------------------------------
        // PARSE PART
        if (n > 4)
        {
            // First loop
            ARCH_AARCH64_ASM
            (
                __ASM_EMIT("mov         %[a], %[dst]")                      // a    = dst
                __ASM_EMIT("add         %[b], %[a], %[n], LSL #3")          // b    = &a[n*2]
                __ASM_EMIT("mov         %[k], %[n]")
                __ASM_EMIT("ldp         q16, q17, [%[XFFT_A], #0x00]")      // v8   = wr0, v9  = wr1
                __ASM_EMIT("ldp         q18, q19, [%[XFFT_A], #0x20]")      // v10  = wi0, v11 = wi1
                __ASM_EMIT("ldp         q24, q25, [%[XFFT_DW]]")            // v24  = dr,  w25 = di

                // 8x butterflies
                __ASM_EMIT("1:")
                __ASM_EMIT("ldp         q0, q2, [%[src]]")                  // v0   = ar0, v2 = ar1
                __ASM_EMIT("eor         v1.16b, v1.16b, v1.16b")            // v1   = 0
                __ASM_EMIT("eor         v3.16b, v3.16b, v3.16b")            // v3   = 0
                __ASM_EMIT("fmul        v5.4s, v0.4s, v18.4s")              // v5   = ar0 * wi0
                __ASM_EMIT("fmul        v7.4s, v2.4s, v19.4s")              // v7   = ar1 * wi1
                __ASM_EMIT("fmul        v4.4s, v0.4s, v16.4s")              // v4   = ar0 * wr0 = br0
                __ASM_EMIT("fmul        v6.4s, v2.4s, v17.4s")              // v6   = ar1 * wr1 = br1
                __ASM_EMIT("fneg        v5.4s, v5.4s")                      // v5   = - ar0 * wi0 = bi0
                __ASM_EMIT("fneg        v7.4s, v7.4s")                      // v7   = - ar1 * wi1 = bi1
                __ASM_EMIT("stp         q0, q1, [%[a], #0x00]")
                __ASM_EMIT("stp         q2, q3, [%[a], #0x20]")
                __ASM_EMIT("add         %[src], %[src], #0x20")
                __ASM_EMIT("stp         q4, q5, [%[b], #0x00]")
                __ASM_EMIT("stp         q6, q7, [%[b], #0x20]")
                __ASM_EMIT("subs        %[k], %[k], #8")
                __ASM_EMIT("add         %[a], %[a], #0x40")
                __ASM_EMIT("add         %[b], %[b], #0x40")
                __ASM_EMIT("b.ls        2f")

                // Rotate angle
                __ASM_EMIT("fmul        v20.4s, v16.4s, v25.4s")            // v20.4s  = wr0 * di
                __ASM_EMIT("fmul        v21.4s, v17.4s, v25.4s")            // v21.4s  = wr1 * di
                __ASM_EMIT("fmul        v22.4s, v18.4s, v25.4s")            // v22.4s  = wi0 * di
                __ASM_EMIT("fmul        v23.4s, v19.4s, v25.4s")            // v23.4s  = wi1 * di
                __ASM_EMIT("fmul        v16.4s, v16.4s, v24.4s")            // v16.4s  = wr0 * dr
                __ASM_EMIT("fmul        v17.4s, v17.4s, v24.4s")            // v17.4s  = wr1 * dr
                __ASM_EMIT("fmul        v18.4s, v18.4s, v24.4s")            // v18.4s  = wi0 * dr
                __ASM_EMIT("fmul        v19.4s, v19.4s, v24.4s")            // v19.4s  = wi1 * dr
                __ASM_EMIT("fsub        v16.4s, v16.4s, v22.4s")            // v16.4s  = wr0*dr - wi0*di
                __ASM_EMIT("fsub        v17.4s, v17.4s, v23.4s")            // v17.4s  = wr1*dr - wi1*di
                __ASM_EMIT("fadd        v18.4s, v18.4s, v20.4s")            // v18.4s  = wi0*dr + wr0*di
                __ASM_EMIT("fadd        v19.4s, v19.4s, v21.4s")            // v19.4s  = wi1*dr + wr1*di
                __ASM_EMIT("b           1b")

                // Post-process
                __ASM_EMIT("2:")
                __ASM_EMIT("sub         %[XFFT_A], %[XFFT_A], #0x40")       // fw  -= 16
                __ASM_EMIT("sub         %[XFFT_DW], %[XFFT_DW], #0x20")     // fdw -= 8
                __ASM_EMIT("lsr         %[n], %[n], #1")                    // n >>= 1

                : [src] "+r" (src),
                  [a] "=&r" (a), [b] "=&r" (b),
                  [XFFT_A] "+r" (fw), [XFFT_DW] "+r" (fdw),
                  [k] "=&r" (k), [n] "+r" (n)
                : [dst] "r" (tmp)
                : "cc", "memory",
                  "q0", "q1", "q2", "q3",
                  "q4", "q5", "q6", "q7",
                  "q16", "q17", "q18", "q19",
                  "q20", "q21", "q22", "q23",
                  "q24", "q25"
            );

            // All other loops while n > 4
            ARCH_AARCH64_ASM
            (
                __ASM_EMIT("cmp         %[n], #8")
                __ASM_EMIT("b.lo        8f")

                __ASM_EMIT("1:")
                __ASM_EMIT("mov         %[a], %[dst]")                      // a    = dst
                __ASM_EMIT("mov         %[p], %[items]")                    // p    = items

                    __ASM_EMIT("3:")
                    __ASM_EMIT("add         %[b], %[a], %[n], LSL #3")          // b    = &a[n*2]
                    __ASM_EMIT("ldp         q16, q17, [%[XFFT_A], #0x00]")      // v16  = wr0, v17 = wr1
                    __ASM_EMIT("ldp         q18, q19, [%[XFFT_A], #0x20]")      // v18  = wi0, v19 = wi1
                    __ASM_EMIT("ldp         q24, q25, [%[XFFT_DW]]")            // v24  = dr,  v25 = di
                    __ASM_EMIT("mov         %[k], %[n]")
                        // 8x butterflies
                        __ASM_EMIT("5:")
                        __ASM_EMIT("ldp         q0, q1, [%[a], #0x00]")             // v0   = ar0, v1 = ai0
                        __ASM_EMIT("ldp         q2, q3, [%[a], #0x20]")             // v2   = ar1, v3 = ai1
                        __ASM_EMIT("ldp         q4, q5, [%[b], #0x00]")             // v4   = br0, v5 = bi0
                        __ASM_EMIT("ldp         q6, q7, [%[b], #0x20]")             // v6   = br1, v7 = bi1
                        __ASM_EMIT("fsub        v20.4s, v0.4s, v4.4s")              // v20  = ar0 - br0 = cr0
                        __ASM_EMIT("fsub        v21.4s, v1.4s, v5.4s")              // v21  = ai0 - bi0 = ci0
                        __ASM_EMIT("fsub        v22.4s, v2.4s, v6.4s")              // v22  = ar1 - br1 = cr1
                        __ASM_EMIT("fsub        v23.4s, v3.4s, v7.4s")              // v23  = ai1 - bi1 = ci1
                        __ASM_EMIT("fadd        v0.4s, v0.4s, v4.4s")               // v0   = ar0 + br0 = ar0'
                        __ASM_EMIT("fadd        v1.4s, v1.4s, v5.4s")               // v1   = ai0 + bi0 = ai0'
                        __ASM_EMIT("fadd        v2.4s, v2.4s, v6.4s")               // v2   = ar1 + br1 = ar1'
                        __ASM_EMIT("fadd        v3.4s, v3.4s, v7.4s")               // v3   = ai1 + bi1 = ai1'
                        __ASM_EMIT("stp         q0, q1, [%[a], #0x00]")
                        __ASM_EMIT("stp         q2, q3, [%[a], #0x20]")
                        __ASM_EMIT("fmul        v0.4s, v16.4s, v20.4s")             // v0   = wr0 * cr0
                        __ASM_EMIT("fmul        v2.4s, v17.4s, v22.4s")             // v2   = wr1 * cr1
                        __ASM_EMIT("fmul        v1.4s, v16.4s, v21.4s")             // v1   = wr0 * ci0
                        __ASM_EMIT("fmul        v3.4s, v17.4s, v23.4s")             // v3   = wr1 * ci1
                        __ASM_EMIT("fmla        v0.4s, v18.4s, v21.4s")             // v0   = wr0*cr0 + wi0*ci0 = br0'
                        __ASM_EMIT("fmla        v2.4s, v19.4s, v23.4s")             // v2   = wr1*cr1 + wi1*ci1 = bi0'
                        __ASM_EMIT("fmls        v1.4s, v18.4s, v20.4s")             // v1   = wr0*ci0 - wi0*cr0 = br1'
                        __ASM_EMIT("fmls        v3.4s, v19.4s, v22.4s")             // v3   = wr1*ci1 - wi1*cr1 = bi1'
                        __ASM_EMIT("stp         q0, q1, [%[b], #0x00]")
                        __ASM_EMIT("stp         q2, q3, [%[b], #0x20]")
                        __ASM_EMIT("subs        %[k], %[k], #8")
                        __ASM_EMIT("add         %[a], %[a], #0x40")
                        __ASM_EMIT("add         %[b], %[b], #0x40")
                        __ASM_EMIT("b.ls        6f")
                        // Rotate angle & repeat loop
                        __ASM_EMIT("fmul        v20.4s, v16.4s, v25.4s")            // v20  = wr0 * di
                        __ASM_EMIT("fmul        v21.4s, v17.4s, v25.4s")            // v21  = wr1 * di
                        __ASM_EMIT("fmul        v22.4s, v18.4s, v25.4s")            // v22  = wi0 * di
                        __ASM_EMIT("fmul        v23.4s, v19.4s, v25.4s")            // v23  = wi1 * di
                        __ASM_EMIT("fmul        v16.4s, v16.4s, v24.4s")            // v16  = wr0 * dr
                        __ASM_EMIT("fmul        v17.4s, v17.4s, v24.4s")            // v17  = wr1 * dr
                        __ASM_EMIT("fmul        v18.4s, v18.4s, v24.4s")            // v18  = wi0 * dr
                        __ASM_EMIT("fmul        v19.4s, v19.4s, v24.4s")            // v19  = wi1 * dr
                        __ASM_EMIT("fsub        v16.4s, v16.4s, v22.4s")            // v16  = wr0*dr - wi0*di
                        __ASM_EMIT("fsub        v17.4s, v17.4s, v23.4s")            // v17  = wr1*dr - wi1*di
                        __ASM_EMIT("fadd        v18.4s, v18.4s, v20.4s")            // v18  = wi0*dr + wr0*di
                        __ASM_EMIT("fadd        v19.4s, v19.4s, v21.4s")            // v19  = wi1*dr + wr1*di
                        __ASM_EMIT("b           5b")
                    __ASM_EMIT("6:")
                    __ASM_EMIT("mov         %[a], %[b]")                        // a    = b
                    __ASM_EMIT("subs        %[p], %[p], %[n], LSL #1")          // p   -= n*2
                    __ASM_EMIT("bne         3b")

                __ASM_EMIT("lsr         %[n], %[n], #1")                    // n >>= 1
                __ASM_EMIT("sub         %[XFFT_A], %[XFFT_A], #0x40")       // fw  -= 16
                __ASM_EMIT("sub         %[XFFT_DW], %[XFFT_DW], #0x20")     // fdw -= 8
                __ASM_EMIT("cmp         %[n], #8")
                __ASM_EMIT("b.ge        1b")

                // Loop for n=4
                __ASM_EMIT("8:")
                __ASM_EMIT("mov         %[a], %[dst]")                      // a    = dst
                __ASM_EMIT("mov         %[p], %[items]")                    // p    = items
                __ASM_EMIT("ldp         q16, q17, [%[XFFT_A], #0x00]")      // v16  = wr0, v17 = wr1
                __ASM_EMIT("ldp         q18, q19, [%[XFFT_A], #0x20]")      // v18  = wi0, v19 = wi1

                // 8x butterflies
                __ASM_EMIT("9:")
                __ASM_EMIT("ldp         q0, q1, [%[a], #0x00]")             // v0   = ar0, v1 = ai0
                __ASM_EMIT("ldp         q2, q3, [%[a], #0x20]")             // v2   = ar1, v3 = ai1
                __ASM_EMIT("ldp         q4, q5, [%[a], #0x40]")             // v4   = ar2, v5 = ai2
                __ASM_EMIT("ldp         q6, q7, [%[a], #0x60]")             // v6   = ar3, v7 = ai3
                __ASM_EMIT("fsub        v20.4s, v0.4s, v2.4s")              // v20  = ar0 - br0 = cr0
                __ASM_EMIT("fsub        v21.4s, v1.4s, v3.4s")              // v21  = ai0 - bi0 = ci0
                __ASM_EMIT("fsub        v22.4s, v4.4s, v6.4s")              // v22  = ar1 - br1 = cr1
                __ASM_EMIT("fsub        v23.4s, v5.4s, v7.4s")              // v23  = ai1 - bi1 = ci1
                __ASM_EMIT("fadd        v0.4s, v0.4s, v2.4s")               // v0   = ar0 + br0 = ar0'
                __ASM_EMIT("fadd        v1.4s, v1.4s, v3.4s")               // v1   = ai0 + bi0 = ai0'
                __ASM_EMIT("fadd        v4.4s, v4.4s, v6.4s")               // v4   = ar1 + br1 = ar1'
                __ASM_EMIT("fadd        v5.4s, v5.4s, v7.4s")               // v5   = ai1 + bi1 = ai1'
                __ASM_EMIT("fmul        v2.4s, v16.4s, v20.4s")             // v2   = wr0 * cr0
                __ASM_EMIT("fmul        v6.4s, v17.4s, v22.4s")             // v6   = wr1 * cr1
                __ASM_EMIT("fmul        v3.4s, v16.4s, v21.4s")             // v3   = wr0 * ci0
                __ASM_EMIT("fmul        v7.4s, v17.4s, v23.4s")             // v7   = wr1 * ci1
                __ASM_EMIT("fmla        v2.4s, v18.4s, v21.4s")             // v2   = wr0*cr0 + wi0*ci0 = br0'
                __ASM_EMIT("fmla        v6.4s, v19.4s, v23.4s")             // v6   = wr1*cr1 + wi1*ci1 = br1'
                __ASM_EMIT("fmls        v3.4s, v18.4s, v20.4s")             // v3   = wr0*ci0 - wi0*cr0 = bi0'
                __ASM_EMIT("fmls        v7.4s, v19.4s, v22.4s")             // v7   = wr1*ci1 - wi1*cr1 = bi1'
                __ASM_EMIT("stp         q0, q1, [%[a], #0x00]")
                __ASM_EMIT("stp         q2, q3, [%[a], #0x20]")
                __ASM_EMIT("stp         q4, q5, [%[a], #0x40]")
                __ASM_EMIT("stp         q6, q7, [%[a], #0x60]")
                __ASM_EMIT("subs        %[p], %[p], #16")                   // p   -= 16
                __ASM_EMIT("add         %[a], %[a], #0x80")
                __ASM_EMIT("bne         9b")

                : [a] "=&r" (a), [b] "=&r" (b),
                  [k] "=&r" (k), [p] "=&r" (p),
                  [XFFT_A] "+r" (fw), [XFFT_DW] "+r" (fdw),
                  [n] "+r" (n)
                : [dst] "r" (tmp), [items] "r" (items)
                : "cc", "memory",
                  "q0", "q1", "q2", "q3",
                  "q4", "q5", "q6", "q7",
                  "q16", "q17", "q18", "q19",
                  "q20", "q21", "q22", "q23",
                  "q24", "q25"
            );
        }
        else
        {
            ARCH_AARCH64_ASM
            (
                __ASM_EMIT("ldr         q0, [%[src]]")
                __ASM_EMIT("eor         v1.16b, v1.16b, v1.16b")
                __ASM_EMIT("stp         q0, q1, [%[dst]]")
                :
                : [src] "r" (src), [dst] "r" (tmp)
                : "memory",
                  "q0", "q1"
            );
        }

        //---------------------------------------------------------------------
        // MIDDLE PART
        ARCH_AARCH64_ASM(
            __ASM_EMIT("subs        %[n], %[items], #8")
            __ASM_EMIT("ldp         q24, q25, [%[MASK]]")
            __ASM_EMIT("mov         %[ptr], %[tmp]")
            __ASM_EMIT("b.lo        2f")
            // Load data and perform complex multiplication
            __ASM_EMIT("1:")
            __ASM_EMIT("ld4         {v0.4s-v3.4s}, [%[ptr]]")       // v0   = r0 i0 r4 i4, v1 = r1 i1 r5 i5, v2 = r2 i2 r6 i6, v3 = r3 i3 r7 i7
            // v0 = r0 i0 r4 i4
            // v1 = r1 i1 r5 i5
            // v2 = r2 i2 r6 i6
            // v3 = r3 i3 r7 i7
            __ASM_EMIT("fsub        v4.4s, v0.4s, v2.4s")           // v4   = r0-r2 i0-i2 r4-r6 i4-i6 = r1' i1' r5' i5'
            __ASM_EMIT("fsub        v5.4s, v1.4s, v3.4s")           // v5   = r1-r3 i1-i3 r5-r7 i5-i7 = r3' i3' r7' i7'
            __ASM_EMIT("fadd        v6.4s, v0.4s, v2.4s")           // v6   = r0+r2 i0+i2 r4+r6 i4+i6 = r0' i0' r4' i4'
            __ASM_EMIT("fadd        v7.4s, v1.4s, v3.4s")           // v7   = r1+r3 i1+i3 r5+r7 i5+i7 = r2' i2' r6' i6'
            __ASM_EMIT("rev64       v5.4s, v5.4s")                  // v5   = i3' r3' i7' r7'
            // v4 = r1' i1' r5' i5'
            // v5 = i3' r3' i7' r7'
            // v6 = r0' i0' r4' i4'
            // v7 = r2' i2' r6' i6'
            __ASM_EMIT("fadd        v0.4s, v6.4s, v7.4s")           // v0   = r0'+r2' i0'+i2' r4'+r6' i4'+i6' = r0" i0" r4" i4"
            __ASM_EMIT("fsub        v1.4s, v6.4s, v7.4s")           // v1   = r0'-r2' i0'-i2' r4'-r6' i4'-i6' = r1" i1" r5" i5"
            __ASM_EMIT("fadd        v2.4s, v4.4s, v5.4s")           // v2   = r1'+i3' i1'+r3' r5'+i7' i5'+r7' = r2" i3" r6" i7"
            __ASM_EMIT("fsub        v3.4s, v4.4s, v5.4s")           // v3   = r1'-i3' i1'-r3' r5'-i7' i5'-r7' = r3" i2" r7" i6"
            __ASM_EMIT("mov         v6.16b, v2.16b")                // v6   = r2" i3" r6" i7"
            __ASM_EMIT("mov         v7.16b, v3.16b")                // v7   = r3" i2" r7" i6"
            __ASM_EMIT("bit         v2.16b, v7.16b, v24.16b")       // v2   = r2" i2" r6" i6"
            __ASM_EMIT("bit         v3.16b, v6.16b, v25.16b")       // v3   = r3" i3" r7" i7"
            // v0 = r0" i0" r4" i4"
            // v1 = r1" i1" r5" i5"
            // v2 = r2" i2" r6" i6"
            // v3 = r3" i3" r7" i7"
            __ASM_EMIT("trn1        v4.4s, v0.4s, v1.4s")           // v4   = r0" r1" r4" r5"
            __ASM_EMIT("trn1        v5.4s, v2.4s, v3.4s")           // v5   = r2" r3" r6" r7"
            __ASM_EMIT("trn2        v6.4s, v0.4s, v1.4s")           // v6   = i0" i1" i4" i5"
            __ASM_EMIT("trn2        v7.4s, v2.4s, v3.4s")           // v7   = i2" i3" i6" i7"
            __ASM_EMIT("trn1        v16.2d, v4.2d, v5.2d")          // v16  = r0" r1" r2" r3" = ar0
            __ASM_EMIT("trn1        v17.2d, v6.2d, v7.2d")          // v17  = i0" i1" i2" i3" = ai0
            __ASM_EMIT("trn2        v18.2d, v4.2d, v5.2d")          // v18  = r4" r5" r6" r7" = ar1
            __ASM_EMIT("trn2        v19.2d, v6.2d, v7.2d")          // v19  = i4" i5" i6" i7" = ai1
            // Apply convolution
            __ASM_EMIT("ldp         q20, q21, [%[c], #0x00]")       // v20  = br0, v21 = bi0
            __ASM_EMIT("ldp         q22, q23, [%[c], #0x20]")       // v22  = br1, v23 = bi1
            __ASM_EMIT("fmul        v0.4s, v16.4s, v20.4s")         // v0   = ar0 * br0
            __ASM_EMIT("fmul        v2.4s, v18.4s, v22.4s")         // v2   = ar1 * br1
            __ASM_EMIT("fmul        v1.4s, v16.4s, v21.4s")         // v1   = ar0 * bi0
            __ASM_EMIT("fmul        v3.4s, v18.4s, v23.4s")         // v3   = ar1 * bi1
            __ASM_EMIT("fmls        v0.4s, v17.4s, v21.4s")         // v0   = ar0*br0 - ai0*bi0
            __ASM_EMIT("fmls        v2.4s, v19.4s, v23.4s")         // v2   = ar1*br1 - ai1*bi1
            __ASM_EMIT("fmla        v1.4s, v17.4s, v20.4s")         // v1   = ar0*bi0 + ai0*br0
            __ASM_EMIT("fmla        v3.4s, v19.4s, v22.4s")         // v3   = ar1*bi1 + ai1*br1
            // v0 = r0 r1 r2 r3
            // v1 = i0 i1 i2 i3
            // v2 = r4 r5 r6 r7
            // v3 = i4 i5 i6 i7
            __ASM_EMIT("trn1        v4.4s, v0.4s, v1.4s")           // v4   = r0 i0 r2 i2
            __ASM_EMIT("trn1        v5.4s, v2.4s, v3.4s")           // v5   = r4 i4 r6 i6
            __ASM_EMIT("trn2        v6.4s, v0.4s, v1.4s")           // v6   = r1 i1 r3 i3
            __ASM_EMIT("trn2        v7.4s, v2.4s, v3.4s")           // v7   = r5 i5 r7 i7
            __ASM_EMIT("trn1        v0.2d, v4.2d, v5.2d")           // v0   = r0 i0 r4 i4
            __ASM_EMIT("trn1        v1.2d, v6.2d, v7.2d")           // v1   = r1 i1 r5 i5
            __ASM_EMIT("trn2        v2.2d, v4.2d, v5.2d")           // v2   = r2 i2 r6 i6
            __ASM_EMIT("trn2        v3.2d, v6.2d, v7.2d")           // v3   = r3 i3 r7 i7
            // v0 = r0 i0 r4 i4
            // v1 = r1 i1 r5 i5
            // v2 = r2 i2 r6 i6
            // v3 = r3 i3 r7 i7
            __ASM_EMIT("fsub        v6.4s, v0.4s, v1.4s")           // v6   = r0-r1 i0-i1 r4-r5 i4-i5 = r1' i1' r5' i5'
            __ASM_EMIT("fsub        v7.4s, v2.4s, v3.4s")           // v7   = r2-r3 i2-i3 r6-r7 i6-i7 = r3' i3' r7' i7'
            __ASM_EMIT("fadd        v4.4s, v0.4s, v1.4s")           // v4   = r0+r1 i0+i1 r4+r5 i4+i5 = r0' i0' r4' i4'
            __ASM_EMIT("fadd        v5.4s, v2.4s, v3.4s")           // v5   = r2+r3 i2+i3 r6+r7 i6+i7 = r2' i2' r6' i6'
            __ASM_EMIT("rev64       v7.4s, v7.4s")                  // v7   = i3' r3' i7' r7'
            // v4 = r0' i0' r4' i4'
            // v5 = r2' i2' r6' i6'
            // v6 = r1' i1' r5' i5'
            // v7 = i3' r3' i7' r7'
            __ASM_EMIT("fadd        v0.4s, v4.4s, v5.4s")           // v0   = r0'+r2' i0'+i2' r4'+r6' i4'+i6' = r0" i0" r4" i4"
            __ASM_EMIT("fsub        v1.4s, v6.4s, v7.4s")           // v2   = r1'-i3' i1'-r3' r5'-i7' i5'-r7' = r1" i3" r5" i7"
            __ASM_EMIT("fsub        v2.4s, v4.4s, v5.4s")           // v1   = r0'-r2' i0'-i2' r4'-r6' i4'-i6' = r2" i2" r6" i6"
            __ASM_EMIT("fadd        v3.4s, v6.4s, v7.4s")           // v3   = r1'+i3' i1'+r3' r5'+i7' i5'+r7' = r3" i1" r7" i5"
            __ASM_EMIT("mov         v6.16b, v1.16b")                // v6   = r1" i3" r5" i7"
            __ASM_EMIT("mov         v7.16b, v3.16b")                // v7   = r3" i1" r7" i5"
            __ASM_EMIT("bit         v1.16b, v7.16b, v24.16b")       // v1   = r1" i1" r5" i5"
            __ASM_EMIT("bit         v3.16b, v6.16b, v25.16b")       // v3   = r3" i3" r7" i7"
            __ASM_EMIT("st4         {v0.4s-v3.4s}, [%[ptr]]")
            __ASM_EMIT("subs        %[n], %[n], #8")                // n   -= 8
            __ASM_EMIT("add         %[c], %[c], #0x40")
            __ASM_EMIT("add         %[ptr], %[ptr], #0x40")
            __ASM_EMIT("b.hs        1b")
            __ASM_EMIT("2:")

            : [c] "+r" (conv), [ptr] "=&r" (ptr),
              [n] "=&r" (n)
            : [tmp] "r" (tmp), [items] "r" (items),
              [MASK] "r" (&fastconv_swp_mask[0])
            : "cc", "memory",
              "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
              "q16", "q17", "q18", "q19", "q20", "q21", "q22", "q23",
              "q24", "q25"
        );

        //---------------------------------------------------------------------
        // RESTORE PART
        n = 8;
        fw          = XFFT_A;

        // Perform butterflies without need of vector rotation
        if (n < items)
        {
            ARCH_AARCH64_ASM(
                __ASM_EMIT("mov         %[a], %[tmp]")
                __ASM_EMIT("subs        %[k], %[items], #16")
                __ASM_EMIT("ldp         q16, q17, [%[XFFT_A], #0x00]")      // v16  = wr1, v17 = wr2
                __ASM_EMIT("ldp         q18, q19, [%[XFFT_A], #0x20]")      // v18  = wi1, v19 = wi2
                __ASM_EMIT("b.lo        2f")
                // 8x butterflies
                // Calculate complex c = w * b
                __ASM_EMIT("1:")
                // Calc cr and ci
                __ASM_EMIT("ldp         q0, q1, [%[a], #0x00]")             // v0   = ar1, v1 = ai1
                __ASM_EMIT("ldp         q2, q3, [%[a], #0x20]")             // v2   = ar2, v3 = ai2
                __ASM_EMIT("ldp         q4, q5, [%[a], #0x40]")             // v4   = ar3, v5 = ai3
                __ASM_EMIT("ldp         q6, q7, [%[a], #0x60]")             // v6   = ar4, v7 = ai4
                __ASM_EMIT("fmul        v20.4s, v16.4s, v2.4s")             // v20  = wr1 * br1
                __ASM_EMIT("fmul        v21.4s, v17.4s, v6.4s")             // v21  = wr2 * br2
                __ASM_EMIT("fmul        v22.4s, v16.4s, v3.4s")             // v22  = wr1 * bi1
                __ASM_EMIT("fmul        v23.4s, v17.4s, v7.4s")             // v23  = wr2 * bi2
                __ASM_EMIT("fmls        v20.4s, v18.4s, v3.4s")             // v20  = wr1 * br1 - wi1 * bi1 = cr1
                __ASM_EMIT("fmls        v21.4s, v19.4s, v7.4s")             // v21  = wr2 * br2 - wi2 * bi2 = cr2
                __ASM_EMIT("fmla        v22.4s, v18.4s, v2.4s")             // v22  = wr1 * bi1 + wi1 * br1 = ci1
                __ASM_EMIT("fmla        v23.4s, v19.4s, v6.4s")             // v23  = wr1 * bi1 + wi2 * br2 = ci2
                // Apply butterfly
                __ASM_EMIT("fsub        v2.4s, v0.4s, v20.4s")              // v2   = ar1 - cr1
                __ASM_EMIT("fsub        v3.4s, v1.4s, v22.4s")              // v3   = ai1 - ci1
                __ASM_EMIT("fsub        v6.4s, v4.4s, v21.4s")              // v6   = ar2 - cr2
                __ASM_EMIT("fsub        v7.4s, v5.4s, v23.4s")              // v7   = ai2 - ci2
                __ASM_EMIT("fadd        v0.4s, v0.4s, v20.4s")              // v0   = ar1 + cr1
                __ASM_EMIT("fadd        v1.4s, v1.4s, v22.4s")              // v1   = ai1 + ci1
                __ASM_EMIT("fadd        v4.4s, v4.4s, v21.4s")              // v4   = ar2 + cr2
                __ASM_EMIT("fadd        v5.4s, v5.4s, v23.4s")              // v5   = ai2 + ci2
                __ASM_EMIT("stp         q0, q1, [%[a], #0x00]")
                __ASM_EMIT("stp         q2, q3, [%[a], #0x20]")
                __ASM_EMIT("stp         q4, q5, [%[a], #0x40]")
                __ASM_EMIT("stp         q6, q7, [%[a], #0x60]")
                __ASM_EMIT("subs        %[k], %[k], #16")
                __ASM_EMIT("add         %[a], %[a], #0x80")
                __ASM_EMIT("b.ge        1b")

                __ASM_EMIT("2:")
                __ASM_EMIT("adds        %[k], %[k], #8")
                __ASM_EMIT("b.lo        4f")
                // 4x butterflies
                __ASM_EMIT("ldp         q0, q1, [%[a], #0x00]")             // v0   = ar1, v1 = ai1
                __ASM_EMIT("ldp         q2, q3, [%[a], #0x20]")             // v2   = ar2, v3 = ai2
                __ASM_EMIT("fmul        v20.4s, v16.4s, v2.4s")             // v20  = wr1 * br1
                __ASM_EMIT("fmul        v22.4s, v16.4s, v3.4s")             // v22  = wr1 * bi1
                __ASM_EMIT("fmls        v20.4s, v18.4s, v3.4s")             // v20  = wr1 * br1 - wi1 * bi1 = cr1
                __ASM_EMIT("fmla        v22.4s, v18.4s, v2.4s")             // v22  = wr1 * bi1 + wi1 * br1 = ci1
                // Apply butterfly
                __ASM_EMIT("fsub        v2.4s, v0.4s, v20.4s")              // v2   = ar1 - cr1
                __ASM_EMIT("fsub        v3.4s, v1.4s, v22.4s")              // v3   = ai1 - ci1
                __ASM_EMIT("fadd        v0.4s, v0.4s, v20.4s")              // v0   = ar1 + cr1
                __ASM_EMIT("fadd        v1.4s, v1.4s, v22.4s")              // v1   = ai1 + ci1
                __ASM_EMIT("stp         q0, q1, [%[a], #0x00]")
                __ASM_EMIT("stp         q2, q3, [%[a], #0x20]")
                __ASM_EMIT("4:")
                __ASM_EMIT("add         %[XFFT_A], %[XFFT_A], #0x40")

                : [a] "=&r" (a), [k] "=&r" (k),
                  [XFFT_A] "+r" (fw)
                : [tmp] "r" (tmp), [items] "r" (items)
                : "cc", "memory",
                  "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
                  "q16", "q17", "q18", "q19", "q20", "q21", "q22", "q23"
            );

            n <<= 1;
        }

        fdw         = &XFFT_DW[8];

        // Perform loops except last one
        while (n < items)
        {
            ARCH_AARCH64_ASM(
                __ASM_EMIT("mov         %[k], %[items]")
                __ASM_EMIT("mov         %[a], %[tmp]")
                __ASM_EMIT("1:")
                    // Initialize sub-loop
                    __ASM_EMIT("mov         %[p], %[n]")
                    __ASM_EMIT("ldp         q16, q17, [%[XFFT_A], #0x00]")      // v16  = wr1, v17 = wr2
                    __ASM_EMIT("ldp         q18, q19, [%[XFFT_A], #0x20]")      // v18  = wi1, v19 = wi2
                    __ASM_EMIT("ldp         q24, q25, [%[XFFT_DW]]")            // v24  = dr,  v25 = di
                    __ASM_EMIT("add         %[b], %[a], %[n], LSL #2")          // b    = &a[n*4]
                    // 8x butterflies
                    __ASM_EMIT("3:")
                        // Calc cr and ci
                        __ASM_EMIT("ldp         q0, q1, [%[a], #0x00]")             // v0   = ar1, v1 = ai1
                        __ASM_EMIT("ldp         q2, q3, [%[a], #0x20]")             // v2   = ar2, v3 = ai2
                        __ASM_EMIT("ldp         q4, q5, [%[b], #0x00]")             // v4   = br1, v5 = bi1
                        __ASM_EMIT("ldp         q6, q7, [%[b], #0x20]")             // v6   = br2, v7 = bi2
                        __ASM_EMIT("fmul        v20.4s, v16.4s, v4.4s")             // v20  = wr1 * br1
                        __ASM_EMIT("fmul        v21.4s, v17.4s, v6.4s")             // v21  = wr2 * br2
                        __ASM_EMIT("fmul        v22.4s, v16.4s, v5.4s")             // v22  = wr1 * bi1
                        __ASM_EMIT("fmul        v23.4s, v17.4s, v7.4s")             // v23  = wr2 * bi2
                        __ASM_EMIT("fmls        v20.4s, v18.4s, v5.4s")             // v20  = wr1 * br1 - wi1 * bi1 = cr1
                        __ASM_EMIT("fmls        v21.4s, v19.4s, v7.4s")             // v21  = wr2 * br2 - wi2 * bi2 = cr2
                        __ASM_EMIT("fmla        v22.4s, v18.4s, v4.4s")             // v22  = wr1 * bi1 + wi1 * br1 = ci1
                        __ASM_EMIT("fmla        v23.4s, v19.4s, v6.4s")             // v23  = wr1 * bi1 + wi2 * br2 = ci2
                        // Apply butterfly
                        __ASM_EMIT("fsub        v4.4s, v0.4s, v20.4s")              // v4   = ar1 - cr1
                        __ASM_EMIT("fsub        v5.4s, v1.4s, v22.4s")              // v5   = ai1 - ci1
                        __ASM_EMIT("fsub        v6.4s, v2.4s, v21.4s")              // v6   = ar2 - cr2
                        __ASM_EMIT("fsub        v7.4s, v3.4s, v23.4s")              // v7   = ai2 - ci2
                        __ASM_EMIT("fadd        v0.4s, v0.4s, v20.4s")              // v0   = ar1 + cr1
                        __ASM_EMIT("fadd        v1.4s, v1.4s, v22.4s")              // v1   = ai1 + ci1
                        __ASM_EMIT("fadd        v2.4s, v2.4s, v21.4s")              // v2   = ar2 + cr2
                        __ASM_EMIT("fadd        v3.4s, v3.4s, v23.4s")              // v3   = ai2 + ci2
                        __ASM_EMIT("stp         q0, q1, [%[a], #0x00]")
                        __ASM_EMIT("stp         q2, q3, [%[a], #0x20]")
                        __ASM_EMIT("stp         q4, q5, [%[b], #0x00]")
                        __ASM_EMIT("stp         q6, q7, [%[b], #0x20]")
                        __ASM_EMIT("subs        %[p], %[p], #16")
                        __ASM_EMIT("add         %[a], %[a], #0x40")
                        __ASM_EMIT("add         %[b], %[b], #0x40")
                        __ASM_EMIT("b.eq        4f")
                        // Prepare next loop: rotate angle
                        __ASM_EMIT("fmul        v20.4s, v16.4s, v25.4s")            // v20  = wr1 * di
                        __ASM_EMIT("fmul        v21.4s, v17.4s, v25.4s")            // v21  = wr2 * di
                        __ASM_EMIT("fmul        v22.4s, v18.4s, v25.4s")            // v22  = wi1 * di
                        __ASM_EMIT("fmul        v23.4s, v19.4s, v25.4s")            // v23  = wi2 * di
                        __ASM_EMIT("fmul        v16.4s, v16.4s, v24.4s")            // v16  = wr1 * dr
                        __ASM_EMIT("fmul        v17.4s, v17.4s, v24.4s")            // v17  = wr2 * dr
                        __ASM_EMIT("fmul        v18.4s, v18.4s, v24.4s")            // v18  = wi1 * dr
                        __ASM_EMIT("fmul        v19.4s, v19.4s, v24.4s")            // v19  = wi2 * dr
                        __ASM_EMIT("fsub        v16.4s, v16.4s, v22.4s")            // v16  = wr1*dr - wi1*di
                        __ASM_EMIT("fsub        v17.4s, v17.4s, v23.4s")            // v17  = wr2*dr - wi2*di
                        __ASM_EMIT("fadd        v18.4s, v18.4s, v20.4s")            // v18  = wi1*dr + wr1*di
                        __ASM_EMIT("fadd        v19.4s, v19.4s, v21.4s")            // v19  = wi2*dr + wr2*di
                        __ASM_EMIT("b           3b")
                    __ASM_EMIT("4:")
                __ASM_EMIT("subs        %[k], %[k], %[n]")
                __ASM_EMIT("mov         %[a], %[b]")
                __ASM_EMIT("b.ne        1b")

                : [a] "=&r" (a), [b] "=&r" (b),
                  [p] "=&r" (p), [k] "=&r" (k)
                : [tmp] "r" (tmp), [items] "r" (items), [n] "r" (n),
                  [XFFT_A] "r" (fw), [XFFT_DW] "r" (fdw)
                : "cc", "memory",
                  "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
                  "q16", "q17", "q18", "q19", "q20", "q21", "q22", "q23",
                  "q24", "q25"
            );

            n <<= 1;
            fw         += 16;
            fdw        += 8;
        }

        // Last one large loop
        IF_ARCH_AARCH64(float fftk = 1.0f / items);

        ARCH_AARCH64_ASM(
            // Initialize sub-loop
            __ASM_EMIT("add         %[b], %[a], %[n], LSL #2")          // b    = &a[n*4]
            __ASM_EMIT("ld1r        {v23.4s}, [%[fftk]]")               // v23  = 1 / items
            __ASM_EMIT("add         %[db], %[da], %[n], LSL #1")        // db   = &da[n*2]
            __ASM_EMIT("ldp         q16, q17, [%[XFFT_A], #0x00]")      // v16  = wr1, v17 = wr2
            __ASM_EMIT("ldp         q18, q19, [%[XFFT_A], #0x20]")      // v18  = wi1, v19 = wi2
            __ASM_EMIT("ldp         q24, q25, [%[XFFT_DW]]")            // v24  = dr,  v25 = di
            __ASM_EMIT("mov         v22.16b, v23.16b")                  // v22  = 1 / items
            // 8x butterflies
            __ASM_EMIT("1:")
            __ASM_EMIT("ldp         q0, q1, [%[a], #0x00]")             // v0   = ar1, v1 = ai1
            __ASM_EMIT("ldp         q2, q3, [%[a], #0x20]")             // v2   = ar2, v3 = ai2
            __ASM_EMIT("ldp         q4, q5, [%[b], #0x00]")             // v4   = br1, v5 = bi1
            __ASM_EMIT("ldp         q6, q7, [%[b], #0x20]")             // v6   = br2, v7 = bi2
            __ASM_EMIT("fmul        v20.4s, v16.4s, v4.4s")             // v20  = wr1 * br1
            __ASM_EMIT("fmul        v21.4s, v17.4s, v6.4s")             // v21  = wr2 * br2
            __ASM_EMIT("fmls        v20.4s, v18.4s, v5.4s")             // v20  = wr1 * br1 - wi1 * bi1 = cr1
            __ASM_EMIT("fmls        v21.4s, v19.4s, v7.4s")             // v21  = wr2 * br2 - wi2 * bi2 = cr2
            // Apply butterfly, we do not need imaginary part
            __ASM_EMIT("fadd        v4.4s, v0.4s, v20.4s")              // v4   = ar1 + cr1
            __ASM_EMIT("fadd        v5.4s, v2.4s, v21.4s")              // v5   = ar2 + cr2
            __ASM_EMIT("fsub        v6.4s, v0.4s, v20.4s")              // v6   = ar1 - cr1
            __ASM_EMIT("fsub        v7.4s, v2.4s, v21.4s")              // v7   = ar2 - cr2
            __ASM_EMIT("ldp         q0, q1, [%[da]]")                   // v0   = dar1, v1 = dar2
            __ASM_EMIT("ldp         q2, q3, [%[db]]")                   // v2   = dbr1, v3 = dbr2
            __ASM_EMIT("fmla        v0.4s, v4.4s, v22.4s")              // v4   = dar1 + (ar1 + cr1) / items
            __ASM_EMIT("fmla        v1.4s, v5.4s, v23.4s")              // v5   = dar2 + (ar2 + cr2) / items
            __ASM_EMIT("fmla        v2.4s, v6.4s, v22.4s")              // v6   = dbr1 + (ar1 - cr1) / items
            __ASM_EMIT("fmla        v3.4s, v7.4s, v23.4s")              // v7   = dbr2 + (ar2 - cr2) / items
            __ASM_EMIT("stp         q0, q1, [%[da]]")
            __ASM_EMIT("stp         q2, q3, [%[db]]")
            __ASM_EMIT("subs        %[items], %[items], #16")
            __ASM_EMIT("add         %[a], %[a], #0x40")
            __ASM_EMIT("add         %[b], %[b], #0x40")
            __ASM_EMIT("add         %[da], %[da], #0x20")
            __ASM_EMIT("add         %[db], %[db], #0x20")
            __ASM_EMIT("b.ls        2f")
            // Prepare next loop: rotate angle
            __ASM_EMIT("fmul        v2.4s, v16.4s, v25.4s")             // v2   = wr1 * di
            __ASM_EMIT("fmul        v3.4s, v17.4s, v25.4s")             // v3   = wr2 * di
            __ASM_EMIT("fmul        v4.4s, v18.4s, v25.4s")             // v4   = wi1 * di
            __ASM_EMIT("fmul        v5.4s, v19.4s, v25.4s")             // v5   = wi2 * di
            __ASM_EMIT("fmul        v16.4s, v16.4s, v24.4s")            // v16  = wr1 * dr
            __ASM_EMIT("fmul        v17.4s, v17.4s, v24.4s")            // v17  = wr2 * dr
            __ASM_EMIT("fmul        v18.4s, v18.4s, v24.4s")            // v18  = wi1 * dr
            __ASM_EMIT("fmul        v19.4s, v19.4s, v24.4s")            // v19  = wi2 * dr
            __ASM_EMIT("fsub        v16.4s, v16.4s, v4.4s")             // v16  = wr1*dr - wi1*di
            __ASM_EMIT("fsub        v17.4s, v17.4s, v5.4s")             // v17  = wr2*dr - wi2*di
            __ASM_EMIT("fadd        v18.4s, v18.4s, v2.4s")             // v18  = wi1*dr + wr1*di
            __ASM_EMIT("fadd        v19.4s, v19.4s, v3.4s")             // v19  = wi2*dr + wr2*di
            __ASM_EMIT("b           1b")
            __ASM_EMIT("2:")

            : [da] "+r" (dst), [db] "=&r" (a),
              [a] "+r" (tmp), [b] "=&r" (b),
              [items] "+r" (items)
            : [n] "r" (n), [fftk] "r" (&fftk),
              [XFFT_A] "r" (fw), [XFFT_DW] "r" (fdw)
            : "cc", "memory",
              "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
              "q16", "q17", "q18", "q19", "q20", "q21", "q22", "q23",
              "q24", "q25"
        );
    }
}

#endif /* DSP_ARCH_AARCH64_ASIMD_FASTCONV_PAPPLY_H_ */
