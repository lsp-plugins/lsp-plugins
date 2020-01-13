/*
 * parse.h
 *
 *  Created on: 13 янв. 2020 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_AARCH64_ASIMD_FASTCONV_PARSE_H_
#define DSP_ARCH_AARCH64_ASIMD_FASTCONV_PARSE_H_

#ifndef DSP_ARCH_AARCH64_ASIMD_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_AARCH64_ASIMD_IMPL */

namespace asimd
{
    void fastconv_parse(float *dst, const float *src, size_t rank)
    {
        size_t items        = 1 << rank; // number of complex numbers per half of block (16 for rank=4)
        size_t n            = items >> 1; // number of butterflies per block (8 for rank=4)

        IF_ARCH_AARCH64(
            const float *fdw    = &XFFT_DW[(rank - 3) << 3];
            const float *fw     = &XFFT_A[(rank - 3) << 4];
            size_t k, p;
            float *a, *b;
        );

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
                : [dst] "r" (dst)
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
                : [dst] "r" (dst), [items] "r" (items)
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
                : [src] "r" (src), [dst] "r" (dst)
                : "memory",
                  "q0", "q1"
            );
        }

        // Perform small 4x4 post-processing
        ARCH_AARCH64_ASM(
            // Loop for n=4
            __ASM_EMIT("subs        %[items], %[items], #8")
            __ASM_EMIT("ldp         q16, q17, [%[MASK]]")
            __ASM_EMIT("b.lo        2f")
            // 8x butterflies
            __ASM_EMIT("1:")
            __ASM_EMIT("ld4         {v0.4s-v3.4s}, [%[dst]]")   // v0   = r0 i0 r4 i4, v1 = r1 i1 r5 i5, v2 = r2 i2 r6 i6, v3 = r3 i3 r7 i7
            // v0 = r0 i0 r4 i4
            // v1 = r1 i1 r5 i5
            // v2 = r2 i2 r6 i6
            // v3 = r3 i3 r7 i7
            __ASM_EMIT("fsub        v4.4s, v0.4s, v2.4s")       // v4   = r0-r2 i0-i2 r4-r6 i4-i6 = r1' i1' r5' i5'
            __ASM_EMIT("fsub        v5.4s, v1.4s, v3.4s")       // v5   = r1-r3 i1-i3 r5-r7 i5-i7 = r3' i3' r7' i7'
            __ASM_EMIT("fadd        v6.4s, v0.4s, v2.4s")       // v6   = r0+r2 i0+i2 r4+r6 i4+i6 = r0' i0' r4' i4'
            __ASM_EMIT("fadd        v7.4s, v1.4s, v3.4s")       // v7   = r1+r3 i1+i3 r5+r7 i5+i7 = r2' i2' r6' i6'
            __ASM_EMIT("rev64       v5.4s, v5.4s")              // v5   = i3' r3' i7' r7'
            // v4 = r1' i1' r5' i5'
            // v5 = i3' r3' i7' r7'
            // v6 = r0' i0' r4' i4'
            // v7 = r2' i2' r6' i6'
            __ASM_EMIT("fadd        v0.4s, v6.4s, v7.4s")       // v0   = r0'+r2' i0'+i2' r4'+r6' i4'+i6' = r0" i0" r4" i4"
            __ASM_EMIT("fsub        v1.4s, v6.4s, v7.4s")       // v1   = r0'-r2' i0'-i2' r4'-r6' i4'-i6' = r1" i1" r5" i5"
            __ASM_EMIT("fadd        v2.4s, v4.4s, v5.4s")       // v2   = r1'+i3' i1'+r3' r5'+i7' i5'+r7' = r2" i3" r6" i7"
            __ASM_EMIT("fsub        v3.4s, v4.4s, v5.4s")       // v3   = r1'-i3' i1'-r3' r5'-i7' i5'-r7' = r3" i2" r7" i6"
            __ASM_EMIT("mov         v6.16b, v2.16b")            // v6   = r2" i3" r6" i7"
            __ASM_EMIT("mov         v7.16b, v3.16b")            // v7   = r3" i2" r7" i6"
            __ASM_EMIT("bit         v2.16b, v7.16b, v16.16b")   // v2   = r2" i2" r6" i6"
            __ASM_EMIT("bit         v3.16b, v6.16b, v17.16b")   // v3   = r3" i3" r7" i7"
            __ASM_EMIT("st4         {v0.4s-v3.4s}, [%[dst]]")
            __ASM_EMIT("subs        %[items], %[items], #8")    // items -= 8
            __ASM_EMIT("add         %[dst], %[dst], #0x40")
            __ASM_EMIT("b.hs        1b")
            __ASM_EMIT("2:")
            // 4x butterflies
            __ASM_EMIT("adds        %[items], %[items], #4")
            __ASM_EMIT("b.lt        4f")
            __ASM_EMIT("ldp         q2, q3, [%[dst]]")          // v2   = r0 r1 r2 r3, v3 = i0 i1 i2 i3
            __ASM_EMIT("zip1        v0.4s, v2.4s, v3.4s")       // v0   = r0 i0 r1 i1
            __ASM_EMIT("zip2        v1.4s, v2.4s, v3.4s")       // v1   = r2 i2 r3 i3
            __ASM_EMIT("fsub        v4.4s, v0.4s, v1.4s")       // v4   = r0-r2 i0-i2 r1-r3 i1-i3 = r1' i1' r3' i3'
            __ASM_EMIT("fadd        v5.4s, v0.4s, v1.4s")       // v5   = r0+r2 i0+i2 r1+r3 i1+i3 = r0' i0' r2' i2'
            __ASM_EMIT("rev64       v6.4s, v4.4s")              // v6   = i1' r1' i3' r3'
            __ASM_EMIT("ins         v6.d[0], v5.d[1]")          // v6   = r2' i2' i3' r3'
            __ASM_EMIT("ins         v5.d[1], v4.d[0]")          // v5   = r0' i0' r1' i1'
            __ASM_EMIT("fadd        v0.4s, v5.4s, v6.4s")       // v0   = r0'+r2' i0'+r2' r1'+i3' i1'+r3' = r0" i0" r2" i3"
            __ASM_EMIT("fsub        v1.4s, v5.4s, v6.4s")       // v1   = r0'-r2' i0'-r2' r1'-i3' i1'-r3' = r1" i1" r3" i2"
            __ASM_EMIT("zip1        v4.4s, v0.4s, v1.4s")       // v4   = r0" r1" i0" i1"
            __ASM_EMIT("zip2        v5.4s, v0.4s, v1.4s")       // v5   = r2" r3" i3" i2"
            __ASM_EMIT("rev64       v6.4s, v5.4s")              // v6   = r3" r2" i2" i3"
            __ASM_EMIT("ins         v6.d[0], v4.d[1]")          // v6   = i0" i1" i2" i3"
            __ASM_EMIT("ins         v4.d[1], v5.d[0]")          // v4   = r0" r1" r2" r3"
            __ASM_EMIT("stp         q4, q6, [%[dst]]")
            __ASM_EMIT("4:")

            : [dst] "+r" (dst), [items] "+r" (items)
            : [MASK] "r" (&fastconv_swp_mask[0])
            : "cc", "memory",
              "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
              "q16", "q17"
        );
    }
}

#endif /* DSP_ARCH_AARCH64_ASIMD_FASTCONV_PARSE_H_ */
