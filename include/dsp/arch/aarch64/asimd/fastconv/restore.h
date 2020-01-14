/*
 * restore.h
 *
 *  Created on: 13 янв. 2020 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_AARCH64_ASIMD_FASTCONV_RESTORE_H_
#define DSP_ARCH_AARCH64_ASIMD_FASTCONV_RESTORE_H_

#ifndef DSP_ARCH_AARCH64_ASIMD_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_AARCH64_ASIMD_IMPL */

namespace asimd
{
    void fastconv_restore(float *dst, float *tmp, size_t rank)
    {
        size_t items        = 1 << rank; // number of complex numbers per half of block (16 for rank=4)
        size_t n;

        IF_ARCH_AARCH64(
            float *a, *b;
            size_t k, p;
        );

        ARCH_AARCH64_ASM(
            __ASM_EMIT("mov         %[a], %[tmp]")
            __ASM_EMIT("subs        %[n], %[items], #8")
            __ASM_EMIT("ldp         q16, q17, [%[MASK]]")
            __ASM_EMIT("b.lo        2f")

            __ASM_EMIT("1:")
            __ASM_EMIT("ld4         {v0.4s-v3.4s}, [%[a]]")
            // v0 = r0 i0 r4 i4
            // v1 = r1 i1 r5 i5
            // v2 = r2 i2 r6 i6
            // v3 = r3 i3 r7 i7
            __ASM_EMIT("fsub        v6.4s, v0.4s, v1.4s")       // v6 = r0-r1 i0-i1 r4-r5 i4-i5 = r1' i1' r5' i5'
            __ASM_EMIT("fsub        v7.4s, v2.4s, v3.4s")       // v7 = r2-r3 i2-i3 r6-r7 i6-i7 = r3' i3' r7' i7'
            __ASM_EMIT("fadd        v4.4s, v0.4s, v1.4s")       // v4 = r0+r1 i0+i1 r4+r5 i4+i5 = r0' i0' r4' i4'
            __ASM_EMIT("fadd        v5.4s, v2.4s, v3.4s")       // v5 = r2+r3 i2+i3 r6+r7 i6+i7 = r2' i2' r6' i6'
            __ASM_EMIT("rev64       v7.4s, v7.4s")              // v7 = i3' r3' i7' r7'
            // v4 = r0' i0' r4' i4'
            // v5 = r2' i2' r6' i6'
            // v6 = r1' i1' r5' i5'
            // v7 = i3' r3' i7' r7'
            __ASM_EMIT("fadd        v0.4s, v4.4s, v5.4s")       // v0 = r0'+r2' i0'+i2' r4'+r6' i4'+i6' = r0" i0" r4" i4"
            __ASM_EMIT("fsub        v1.4s, v6.4s, v7.4s")       // v2 = r1'-i3' i1'-r3' r5'-i7' i5'-r7' = r1" i3" r5" i7"
            __ASM_EMIT("fsub        v2.4s, v4.4s, v5.4s")       // v1 = r0'-r2' i0'-i2' r4'-r6' i4'-i6' = r2" i2" r6" i6"
            __ASM_EMIT("fadd        v3.4s, v6.4s, v7.4s")       // v3 = r1'+i3' i1'+r3' r5'+i7' i5'+r7' = r3" i1" r7" i5"
            __ASM_EMIT("mov         v6.16b, v1.16b")            // v6   = r1" i3" r5" i7"
            __ASM_EMIT("mov         v7.16b, v3.16b")            // v7   = r3" i1" r7" i5"
            __ASM_EMIT("bit         v1.16b, v7.16b, v16.16b")   // v1   = r1" i1" r5" i5"
            __ASM_EMIT("bit         v3.16b, v6.16b, v17.16b")   // v3   = r3" i3" r7" i7"
            __ASM_EMIT("st4         {v0.4s-v3.4s}, [%[a]]")
            __ASM_EMIT("subs        %[n], %[n], #8")            // n   -= 8
            __ASM_EMIT("add         %[a], %[a], #0x40")
            __ASM_EMIT("b.hs        1b")
            __ASM_EMIT("2:")

            : [a] "=&r" (a), [n] "=&r" (n)
            : [tmp] "r" (tmp), [items] "r" (items),
              [MASK] "r" (&fastconv_swp_mask[0])
            : "cc", "memory",
              "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
              "q16", "q17"
        );

        n = 8;
        const float *xfft_a     = XFFT_A;

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
                  [XFFT_A] "+r" (xfft_a)
                : [tmp] "r" (tmp), [items] "r" (items)
                : "cc", "memory",
                  "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
                  "q16", "q17", "q18", "q19", "q20", "q21", "q22", "q23"
            );

            n <<= 1;
        }

        const float *xfft_dw    = &XFFT_DW[8];

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
                  [XFFT_A] "r" (xfft_a), [XFFT_DW] "r" (xfft_dw)
                : "cc", "memory",
                  "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
                  "q16", "q17", "q18", "q19", "q20", "q21", "q22", "q23",
                  "q24", "q25"
            );

            n <<= 1;
            xfft_a     += 16;
            xfft_dw    += 8;
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
            __ASM_EMIT("fmul        v4.4s, v4.4s, v22.4s")              // v4   = (ar1 + cr1) / items
            __ASM_EMIT("fmul        v5.4s, v5.4s, v23.4s")              // v5   = (ar2 + cr2) / items
            __ASM_EMIT("fmul        v6.4s, v6.4s, v22.4s")              // v6   = (ar1 - cr1) / items
            __ASM_EMIT("fmul        v7.4s, v7.4s, v23.4s")              // v7   = (ar2 - cr2) / items
            __ASM_EMIT("stp         q4, q5, [%[da]]")
            __ASM_EMIT("stp         q6, q7, [%[db]]")
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
              [XFFT_A] "r" (xfft_a), [XFFT_DW] "r" (xfft_dw)
            : "cc", "memory",
              "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
              "q16", "q17", "q18", "q19", "q20", "q21", "q22", "q23",
              "q24", "q25"
        );
    }
}


#endif /* DSP_ARCH_AARCH64_ASIMD_FASTCONV_RESTORE_H_ */
