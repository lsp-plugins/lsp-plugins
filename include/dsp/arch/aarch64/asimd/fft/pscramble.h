/*
 * pscramble.h
 *
 *  Created on: 18 дек. 2019 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_AARCH64_ASIMD_FFT_PSCRAMBLE_H_
#define DSP_ARCH_AARCH64_ASIMD_FFT_PSCRAMBLE_H_

#ifndef DSP_ARCH_AARCH64_ASIMD_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_AARCH64_ASIMD_IMPL */

namespace asimd
{
    void packed_scramble_self_direct(float *dst, size_t rank)
    {
        IF_ARCH_AARCH64(
            size_t count = 1 << rank;
            size_t i, j, rrank = 64 - rank;
            float *dp, *src = dst;
        );

        // Self algorithm
        ARCH_AARCH64_ASM(
            // Do bit-reverse shuffle
            __ASM_EMIT("mov         %[i], #1")                          // i = 1
            __ASM_EMIT("1:")
            __ASM_EMIT("rbit        %[j], %[i]")                        // j = reverse_bits(i)
            __ASM_EMIT("add         %[src], %[src], #8")
            __ASM_EMIT("lsr         %[j], %[j], %[rrank]")              // j = reverse_bits(i) >> rank
            __ASM_EMIT("cmp         %[i], %[j]")                        // i <=> j
            __ASM_EMIT("b.hs         2f")                               // if (i >= j) continue
            __ASM_EMIT("add         %[dp], %[dst], %[j], LSL #3")       // d_re = &dst[j]
            __ASM_EMIT("ldr         d0, [%[src]]")                      // d0   = *src
            __ASM_EMIT("ldr         d1, [%[dp]]")                       // d1   = *dp
            __ASM_EMIT("str         d0, [%[dp]]")                       // *dp  = d0
            __ASM_EMIT("str         d1, [%[src]]")                      // *src = d1
            __ASM_EMIT("2:")
            __ASM_EMIT("add         %[i], %[i], #1")                    // i++
            __ASM_EMIT("cmp         %[i], %[count]")                    // i <=> count
            __ASM_EMIT("b.lo        1b")
            // Perform x8 butterflies
            __ASM_EMIT("3:")
            __ASM_EMIT("ld4         {v0.4s-v3.4s}, [%[dst]]")
            // v0 = r0 r2 r4 r6
            // v1 = i0 i2 i4 i6
            // v2 = r1 r3 r5 r7
            // v3 = i1 i3 i5 i7
            __ASM_EMIT("fadd        v6.4s, v0.4s, v2.4s")               // v6 = r0+r1 r2+r3 r4+r5 r6+r7 = r0' r2' r4' r6'
            __ASM_EMIT("fadd        v7.4s, v1.4s, v3.4s")               // v7 = i0+i1 i2+i3 i4+i5 i6+i7 = i0' i2' i4' i6'
            __ASM_EMIT("fsub        v0.4s, v0.4s, v2.4s")               // v0 = r0-r1 r2-r3 r4-r5 r6-r7 = r1' r3' r5' r7'
            __ASM_EMIT("fsub        v1.4s, v1.4s, v3.4s")               // v1 = i0-i1 i2-i3 i4-i5 i6-i7 = i1' i3' i5' i7'
            // v6 = r0' r2' r4' r6'
            // v0 = r1' r3' r5' r7'
            // v7 = i0' i2' i4' i6'
            // v1 = i1' i3' i5' i7'
            __ASM_EMIT("uzp1        v4.4s, v6.4s, v0.4s")               // v4 = r0' r4' r1' r5'
            __ASM_EMIT("uzp2        v0.4s, v6.4s, v0.4s")               // v0 = r2' r6' r3' r7'
            __ASM_EMIT("uzp1        v5.4s, v7.4s, v1.4s")               // v5 = i0' i4' i1' i5'
            __ASM_EMIT("uzp2        v1.4s, v7.4s, v1.4s")               // v1 = i2' i6' i3' i7'
            __ASM_EMIT("mov         v2.16b, v0.16b")                    // v2 = r2' r6' r3' r7'
            __ASM_EMIT("ins         v0.d[1], v1.d[1]")                  // v0 = r2' r6' i3' i7'
            __ASM_EMIT("ins         v1.d[1], v2.d[1]")                  // v1 = i2' i6' r3' r7'
            __ASM_EMIT("fadd        v2.4s, v4.4s, v0.4s")               // v2 = r0'+r2' r4'+r6' r1'+i3' r5'+i7' = r0" r4" r1" r5"
            __ASM_EMIT("fsub        v3.4s, v4.4s, v0.4s")               // v3 = r0'-r2' r4'-r6' r1'-i3' r5'-i7' = r2" r6" r3" r7"
            __ASM_EMIT("fadd        v0.4s, v5.4s, v1.4s")               // v0 = i0'+i2' i4'+i6' i1'+r3' i5'+r7' = i0" i4" i3" i7"
            __ASM_EMIT("fsub        v1.4s, v5.4s, v1.4s")               // v1 = i0'-i2' i4'-i6' i1'-r3' i5'-r7' = i2" i6" i1" i5"
            // v0 = i0" i4" i3" i7"
            // v1 = i2" i6" i1" i5"
            // v2 = r0" r4" r1" r5"
            // v3 = r2" r6" r3" r7"
            __ASM_EMIT("mov         v4.16b, v0.16b")                    // v4 = i0" i4" i3" i7"
            __ASM_EMIT("ins         v0.d[1], v1.d[1]")                  // v0 = i0" i4" i1" i5"
            __ASM_EMIT("ins         v1.d[1], v4.d[1]")                  // v1 = i2" i6" i3" i7"
            __ASM_EMIT("uzp1        v4.4s, v2.4s, v3.4s")               // v4 = r0" r1" r2" r3"
            __ASM_EMIT("uzp2        v5.4s, v2.4s, v3.4s")               // v5 = r4" r5" r6" r7"
            __ASM_EMIT("uzp1        v6.4s, v0.4s, v1.4s")               // v6 = i0" i1" i2" i3"
            __ASM_EMIT("uzp2        v7.4s, v0.4s, v1.4s")               // v7 = i4" i5" i6" i7"
            // Store
            __ASM_EMIT("stp         q4, q6, [%[dst], #0x00]")
            __ASM_EMIT("stp         q5, q7, [%[dst], #0x20]")
            __ASM_EMIT("subs        %[count], %[count], #8")            // i <=> count
            __ASM_EMIT("add         %[dst], %[dst], #0x40")
            __ASM_EMIT("b.ne        3b")

            : [src] "+r" (src), [dst] "+r" (dst), [dp] "=&r" (dp),
              [rrank] "+r" (rrank), [i] "=&r" (i), [j] "=&r" (j),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "v0", "v1", "v2", "v3",
              "v4", "v5", "v6", "v7"
        );
    }

    void packed_scramble_copy_direct(float *dst, const float *src, size_t rank)
    {
        IF_ARCH_AARCH64(
            size_t i, j;
            float *sr1, *sr2;
            size_t regs = 1 << (rank - 3), rrank = 64 - (rank - 3);
        );

        ARCH_AARCH64_ASM(
            __ASM_EMIT("eor         %[i], %[i], %[i]")                  // i = 0
            __ASM_EMIT("1:")
            __ASM_EMIT("rbit        %[j], %[i]")                        // j = reverse_bits(i)
            __ASM_EMIT("lsr         %[j], %[j], %[rrank]")              // j = reverse_bits(i) >> rank

            __ASM_EMIT("add         %[sr1], %[src], %[j], LSL #3")      // sr1 = &src[j]
            __ASM_EMIT("add         %[sr2], %[sr1], %[regs], LSL #3")   // sr2 = &src[j + regs]
            __ASM_EMIT("ld2         {v0.s, v1.s}[0], [%[sr1]]")         // v0 = r0  ?  ?  ?, v1 = i0  ?  ?  ?
            __ASM_EMIT("ld2         {v0.s, v1.s}[2], [%[sr2]]")         // v0 = r0  ? r4  ?, v1 = i0  ? i4  ?
            __ASM_EMIT("add         %[sr1], %[sr1], %[regs], LSL #4")   // sr1 = &src[j + regs*2]
            __ASM_EMIT("add         %[sr2], %[sr2], %[regs], LSL #4")   // sr2 = &src[j + regs*3]
            __ASM_EMIT("ld2         {v0.s, v1.s}[1], [%[sr1]]")         // v0 = r0 r2 r4  ?, v1 = i0 i2 i4  ?
            __ASM_EMIT("ld2         {v0.s, v1.s}[3], [%[sr2]]")         // v0 = r0 r2 r4 r6, v1 = i0 i2 i4 i6

            __ASM_EMIT("add         %[sr1], %[sr1], %[regs], LSL #4")   // sr1 = &src[j + regs*4]
            __ASM_EMIT("add         %[sr2], %[sr2], %[regs], LSL #4")   // sr2 = &src[j + regs*5]
            __ASM_EMIT("ld2         {v2.s, v3.s}[0], [%[sr1]]")         // v2 = r1  ?  ?  ?, v3 = i1  ?  ?  ?
            __ASM_EMIT("ld2         {v2.s, v3.s}[2], [%[sr2]]")         // v2 = r1  ? r5  ?, v3 = i1  ? i5  ?
            __ASM_EMIT("add         %[sr1], %[sr1], %[regs], LSL #4")   // sr1 = &src[j + regs*6]
            __ASM_EMIT("add         %[sr2], %[sr2], %[regs], LSL #4")   // sr2 = &src[j + regs*7]
            __ASM_EMIT("ld2         {v2.s, v3.s}[1], [%[sr1]]")         // v2 = r1 r3 r5  ?, v3 = i1 i3 i5  ?
            __ASM_EMIT("ld2         {v2.s, v3.s}[3], [%[sr2]]")         // v2 = r1 r3 r5 r7, v3 = i1 i3 i5 i7
            // v0 = r0 r2 r4 r6
            // v1 = i0 i2 i4 i6
            // v2 = r1 r3 r5 r7
            // v3 = i1 i3 i5 i7
            __ASM_EMIT("fadd        v6.4s, v0.4s, v2.4s")               // v6 = r0+r1 r2+r3 r4+r5 r6+r7 = r0' r2' r4' r6'
            __ASM_EMIT("fadd        v7.4s, v1.4s, v3.4s")               // v7 = i0+i1 i2+i3 i4+i5 i6+i7 = i0' i2' i4' i6'
            __ASM_EMIT("fsub        v0.4s, v0.4s, v2.4s")               // v0 = r0-r1 r2-r3 r4-r5 r6-r7 = r1' r3' r5' r7'
            __ASM_EMIT("fsub        v1.4s, v1.4s, v3.4s")               // v1 = i0-i1 i2-i3 i4-i5 i6-i7 = i1' i3' i5' i7'
            // v6 = r0' r2' r4' r6'
            // v0 = r1' r3' r5' r7'
            // v7 = i0' i2' i4' i6'
            // v1 = i1' i3' i5' i7'
            __ASM_EMIT("uzp1        v4.4s, v6.4s, v0.4s")               // v4 = r0' r4' r1' r5'
            __ASM_EMIT("uzp2        v0.4s, v6.4s, v0.4s")               // v0 = r2' r6' r3' r7'
            __ASM_EMIT("uzp1        v5.4s, v7.4s, v1.4s")               // v5 = i0' i4' i1' i5'
            __ASM_EMIT("uzp2        v1.4s, v7.4s, v1.4s")               // v1 = i2' i6' i3' i7'
            __ASM_EMIT("mov         v2.16b, v0.16b")                    // v2 = r2' r6' r3' r7'
            __ASM_EMIT("ins         v0.d[1], v1.d[1]")                  // v0 = r2' r6' i3' i7'
            __ASM_EMIT("ins         v1.d[1], v2.d[1]")                  // v1 = i2' i6' r3' r7'
            __ASM_EMIT("fadd        v2.4s, v4.4s, v0.4s")               // v2 = r0'+r2' r4'+r6' r1'+i3' r5'+i7' = r0" r4" r1" r5"
            __ASM_EMIT("fsub        v3.4s, v4.4s, v0.4s")               // v3 = r0'-r2' r4'-r6' r1'-i3' r5'-i7' = r2" r6" r3" r7"
            __ASM_EMIT("fadd        v0.4s, v5.4s, v1.4s")               // v0 = i0'+i2' i4'+i6' i1'+r3' i5'+r7' = i0" i4" i3" i7"
            __ASM_EMIT("fsub        v1.4s, v5.4s, v1.4s")               // v1 = i0'-i2' i4'-i6' i1'-r3' i5'-r7' = i2" i6" i1" i5"
            // v0 = i0" i4" i3" i7"
            // v1 = i2" i6" i1" i5"
            // v2 = r0" r4" r1" r5"
            // v3 = r2" r6" r3" r7"
            __ASM_EMIT("mov         v4.16b, v0.16b")                    // v4 = i0" i4" i3" i7"
            __ASM_EMIT("ins         v0.d[1], v1.d[1]")                  // v0 = i0" i4" i1" i5"
            __ASM_EMIT("ins         v1.d[1], v4.d[1]")                  // v1 = i2" i6" i3" i7"
            __ASM_EMIT("uzp1        v4.4s, v2.4s, v3.4s")               // v4 = r0" r1" r2" r3"
            __ASM_EMIT("uzp2        v5.4s, v2.4s, v3.4s")               // v5 = r4" r5" r6" r7"
            __ASM_EMIT("uzp1        v6.4s, v0.4s, v1.4s")               // v6 = i0" i1" i2" i3"
            __ASM_EMIT("uzp2        v7.4s, v0.4s, v1.4s")               // v7 = i4" i5" i6" i7"
            // Store
            __ASM_EMIT("add         %[i], %[i], #1")
            __ASM_EMIT("stp         q4, q6, [%[dst], #0x00]")
            __ASM_EMIT("stp         q5, q7, [%[dst], #0x20]")
            __ASM_EMIT("cmp         %[i], %[regs]")
            __ASM_EMIT("add         %[dst], %[dst], #0x40")
            __ASM_EMIT("b.lo        1b")

            : [dst] "+r" (dst), [sr1] "=&r" (sr1), [sr2] "=&r" (sr2),
              [rrank] "+r" (rrank), [i] "=&r" (i), [j] "=&r" (j)
            : [src] "r" (src), [regs] "r" (regs)
            : "cc", "memory",
              "v0", "v1", "v2", "v3",
              "v4", "v5", "v6", "v7"
        );
    }

    void packed_scramble_self_reverse(float *dst, size_t rank)
    {
        IF_ARCH_AARCH64(
            size_t count = 1 << rank;
            size_t i, j, rrank = 64 - rank;
            float *dp, *src = dst;
        );

        // Self algorithm
        ARCH_AARCH64_ASM(
            // Do bit-reverse shuffle
            __ASM_EMIT("mov         %[i], #1")                          // i = 1
            __ASM_EMIT("1:")
            __ASM_EMIT("rbit        %[j], %[i]")                        // j = reverse_bits(i)
            __ASM_EMIT("add         %[src], %[src], #8")
            __ASM_EMIT("lsr         %[j], %[j], %[rrank]")              // j = reverse_bits(i) >> rank
            __ASM_EMIT("cmp         %[i], %[j]")                        // i <=> j
            __ASM_EMIT("b.hs         2f")                               // if (i >= j) continue
            __ASM_EMIT("add         %[dp], %[dst], %[j], LSL #3")       // d_re = &dst[j]
            __ASM_EMIT("ldr         d0, [%[src]]")                      // d0   = *src
            __ASM_EMIT("ldr         d1, [%[dp]]")                       // d1   = *dp
            __ASM_EMIT("str         d0, [%[dp]]")                       // *dp  = d0
            __ASM_EMIT("str         d1, [%[src]]")                      // *src = d1
            __ASM_EMIT("2:")
            __ASM_EMIT("add         %[i], %[i], #1")                    // i++
            __ASM_EMIT("cmp         %[i], %[count]")                    // i <=> count
            __ASM_EMIT("b.lo        1b")
            // Perform x8 butterflies
            __ASM_EMIT("3:")
            __ASM_EMIT("ld4         {v0.4s-v3.4s}, [%[dst]]")
            // v0 = r0 r2 r4 r6
            // v1 = i0 i2 i4 i6
            // v2 = r1 r3 r5 r7
            // v3 = i1 i3 i5 i7
            __ASM_EMIT("fadd        v6.4s, v0.4s, v2.4s")               // v6 = r0+r1 r2+r3 r4+r5 r6+r7 = r0' r2' r4' r6'
            __ASM_EMIT("fadd        v7.4s, v1.4s, v3.4s")               // v7 = i0+i1 i2+i3 i4+i5 i6+i7 = i0' i2' i4' i6'
            __ASM_EMIT("fsub        v0.4s, v0.4s, v2.4s")               // v0 = r0-r1 r2-r3 r4-r5 r6-r7 = r1' r3' r5' r7'
            __ASM_EMIT("fsub        v1.4s, v1.4s, v3.4s")               // v1 = i0-i1 i2-i3 i4-i5 i6-i7 = i1' i3' i5' i7'
            // v6 = r0' r2' r4' r6'
            // v0 = r1' r3' r5' r7'
            // v7 = i0' i2' i4' i6'
            // v1 = i1' i3' i5' i7'
            __ASM_EMIT("uzp1        v4.4s, v6.4s, v0.4s")               // v4 = r0' r4' r1' r5'
            __ASM_EMIT("uzp2        v0.4s, v6.4s, v0.4s")               // v0 = r2' r6' r3' r7'
            __ASM_EMIT("uzp1        v5.4s, v7.4s, v1.4s")               // v5 = i0' i4' i1' i5'
            __ASM_EMIT("uzp2        v1.4s, v7.4s, v1.4s")               // v1 = i2' i6' i3' i7'
            __ASM_EMIT("mov         v2.16b, v0.16b")                    // v2 = r2' r6' r3' r7'
            __ASM_EMIT("ins         v0.d[1], v1.d[1]")                  // v0 = r2' r6' i3' i7'
            __ASM_EMIT("ins         v1.d[1], v2.d[1]")                  // v1 = i2' i6' r3' r7'
            __ASM_EMIT("fadd        v2.4s, v4.4s, v0.4s")               // v2 = r0'+r2' r4'+r6' r1'+i3' r5'+i7' = r0" r4" r3" r7"
            __ASM_EMIT("fsub        v3.4s, v4.4s, v0.4s")               // v3 = r0'-r2' r4'-r6' r1'-i3' r5'-i7' = r2" r6" r1" r5"
            __ASM_EMIT("fadd        v0.4s, v5.4s, v1.4s")               // v0 = i0'+i2' i4'+i6' i1'+r3' i5'+r7' = i0" i4" i1" i5"
            __ASM_EMIT("fsub        v1.4s, v5.4s, v1.4s")               // v1 = i0'-i2' i4'-i6' i1'-r3' i5'-r7' = i2" i6" i3" i7"
            // v0 = i0" i4" i1" i5"
            // v1 = i2" i6" i3" i7"
            // v2 = r0" r4" r3" r7"
            // v3 = r2" r6" r1" r5"
            __ASM_EMIT("mov         v4.16b, v2.16b")                    // v4 = r0" r4" r3" r7"
            __ASM_EMIT("ins         v2.d[1], v3.d[1]")                  // v2 = r0" r4" r1" r5"
            __ASM_EMIT("ins         v3.d[1], v4.d[1]")                  // v3 = r2" r6" r3" r7"
            __ASM_EMIT("uzp1        v4.4s, v2.4s, v3.4s")               // v4 = r0" r1" r2" r3"
            __ASM_EMIT("uzp2        v5.4s, v2.4s, v3.4s")               // v5 = r4" r5" r6" r7"
            __ASM_EMIT("uzp1        v6.4s, v0.4s, v1.4s")               // v6 = i0" i1" i2" i3"
            __ASM_EMIT("uzp2        v7.4s, v0.4s, v1.4s")               // v7 = i4" i5" i6" i7"
            // Store
            __ASM_EMIT("stp         q4, q6, [%[dst], #0x00]")
            __ASM_EMIT("stp         q5, q7, [%[dst], #0x20]")
            __ASM_EMIT("subs        %[count], %[count], #8")            // i <=> count
            __ASM_EMIT("add         %[dst], %[dst], #0x40")
            __ASM_EMIT("b.ne        3b")

            : [src] "+r" (src), [dst] "+r" (dst), [dp] "=&r" (dp),
              [rrank] "+r" (rrank), [i] "=&r" (i), [j] "=&r" (j),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "v0", "v1", "v2", "v3",
              "v4", "v5", "v6", "v7"
        );
    }

    void packed_scramble_copy_reverse(float *dst, const float *src, size_t rank)
    {
        IF_ARCH_AARCH64(
            size_t i, j;
            float *sr1, *sr2;
            size_t regs = 1 << (rank - 3), rrank = 64 - (rank - 3);
        );

        ARCH_AARCH64_ASM(
            __ASM_EMIT("eor         %[i], %[i], %[i]")                  // i = 0
            __ASM_EMIT("1:")
            __ASM_EMIT("rbit        %[j], %[i]")                        // j = reverse_bits(i)
            __ASM_EMIT("lsr         %[j], %[j], %[rrank]")              // j = reverse_bits(i) >> rank

            __ASM_EMIT("add         %[sr1], %[src], %[j], LSL #3")      // sr1 = &src[j]
            __ASM_EMIT("add         %[sr2], %[sr1], %[regs], LSL #3")   // sr2 = &src[j + regs]
            __ASM_EMIT("ld2         {v0.s, v1.s}[0], [%[sr1]]")         // v0 = r0  ?  ?  ?, v1 = i0  ?  ?  ?
            __ASM_EMIT("ld2         {v0.s, v1.s}[2], [%[sr2]]")         // v0 = r0  ? r4  ?, v1 = i0  ? i4  ?
            __ASM_EMIT("add         %[sr1], %[sr1], %[regs], LSL #4")   // sr1 = &src[j + regs*2]
            __ASM_EMIT("add         %[sr2], %[sr2], %[regs], LSL #4")   // sr2 = &src[j + regs*3]
            __ASM_EMIT("ld2         {v0.s, v1.s}[1], [%[sr1]]")         // v0 = r0 r2 r4  ?, v1 = i0 i2 i4  ?
            __ASM_EMIT("ld2         {v0.s, v1.s}[3], [%[sr2]]")         // v0 = r0 r2 r4 r6, v1 = i0 i2 i4 i6

            __ASM_EMIT("add         %[sr1], %[sr1], %[regs], LSL #4")   // sr1 = &src[j + regs*4]
            __ASM_EMIT("add         %[sr2], %[sr2], %[regs], LSL #4")   // sr2 = &src[j + regs*5]
            __ASM_EMIT("ld2         {v2.s, v3.s}[0], [%[sr1]]")         // v2 = r1  ?  ?  ?, v3 = i1  ?  ?  ?
            __ASM_EMIT("ld2         {v2.s, v3.s}[2], [%[sr2]]")         // v2 = r1  ? r5  ?, v3 = i1  ? i5  ?
            __ASM_EMIT("add         %[sr1], %[sr1], %[regs], LSL #4")   // sr1 = &src[j + regs*6]
            __ASM_EMIT("add         %[sr2], %[sr2], %[regs], LSL #4")   // sr2 = &src[j + regs*7]
            __ASM_EMIT("ld2         {v2.s, v3.s}[1], [%[sr1]]")         // v2 = r1 r3 r5  ?, v3 = i1 i3 i5  ?
            __ASM_EMIT("ld2         {v2.s, v3.s}[3], [%[sr2]]")         // v2 = r1 r3 r5 r7, v3 = i1 i3 i5 i7
            // v0 = r0 r2 r4 r6
            // v1 = i0 i2 i4 i6
            // v2 = r1 r3 r5 r7
            // v3 = i1 i3 i5 i7
            __ASM_EMIT("fadd        v6.4s, v0.4s, v2.4s")               // v6 = r0+r1 r2+r3 r4+r5 r6+r7 = r0' r2' r4' r6'
            __ASM_EMIT("fadd        v7.4s, v1.4s, v3.4s")               // v7 = i0+i1 i2+i3 i4+i5 i6+i7 = i0' i2' i4' i6'
            __ASM_EMIT("fsub        v0.4s, v0.4s, v2.4s")               // v0 = r0-r1 r2-r3 r4-r5 r6-r7 = r1' r3' r5' r7'
            __ASM_EMIT("fsub        v1.4s, v1.4s, v3.4s")               // v1 = i0-i1 i2-i3 i4-i5 i6-i7 = i1' i3' i5' i7'
            // v6 = r0' r2' r4' r6'
            // v0 = r1' r3' r5' r7'
            // v7 = i0' i2' i4' i6'
            // v1 = i1' i3' i5' i7'
            __ASM_EMIT("uzp1        v4.4s, v6.4s, v0.4s")               // v4 = r0' r4' r1' r5'
            __ASM_EMIT("uzp2        v0.4s, v6.4s, v0.4s")               // v0 = r2' r6' r3' r7'
            __ASM_EMIT("uzp1        v5.4s, v7.4s, v1.4s")               // v5 = i0' i4' i1' i5'
            __ASM_EMIT("uzp2        v1.4s, v7.4s, v1.4s")               // v1 = i2' i6' i3' i7'
            __ASM_EMIT("mov         v2.16b, v0.16b")                    // v2 = r2' r6' r3' r7'
            __ASM_EMIT("ins         v0.d[1], v1.d[1]")                  // v0 = r2' r6' i3' i7'
            __ASM_EMIT("ins         v1.d[1], v2.d[1]")                  // v1 = i2' i6' r3' r7'
            __ASM_EMIT("fadd        v2.4s, v4.4s, v0.4s")               // v2 = r0'+r2' r4'+r6' r1'+i3' r5'+i7' = r0" r4" r3" r7"
            __ASM_EMIT("fsub        v3.4s, v4.4s, v0.4s")               // v3 = r0'-r2' r4'-r6' r1'-i3' r5'-i7' = r2" r6" r1" r5"
            __ASM_EMIT("fadd        v0.4s, v5.4s, v1.4s")               // v0 = i0'+i2' i4'+i6' i1'+r3' i5'+r7' = i0" i4" i1" i5"
            __ASM_EMIT("fsub        v1.4s, v5.4s, v1.4s")               // v1 = i0'-i2' i4'-i6' i1'-r3' i5'-r7' = i2" i6" i3" i7"
            // v0 = i0" i4" i1" i5"
            // v1 = i2" i6" i3" i7"
            // v2 = r0" r4" r3" r7"
            // v3 = r2" r6" r1" r5"
            __ASM_EMIT("mov         v4.16b, v2.16b")                    // v4 = r0" r4" r3" r7"
            __ASM_EMIT("ins         v2.d[1], v3.d[1]")                  // v2 = r0" r4" r1" r5"
            __ASM_EMIT("ins         v3.d[1], v4.d[1]")                  // v3 = r2" r6" r3" r7"
            __ASM_EMIT("uzp1        v4.4s, v2.4s, v3.4s")               // v4 = r0" r1" r2" r3"
            __ASM_EMIT("uzp2        v5.4s, v2.4s, v3.4s")               // v5 = r4" r5" r6" r7"
            __ASM_EMIT("uzp1        v6.4s, v0.4s, v1.4s")               // v6 = i0" i1" i2" i3"
            __ASM_EMIT("uzp2        v7.4s, v0.4s, v1.4s")               // v7 = i4" i5" i6" i7"
            // Store
            __ASM_EMIT("add         %[i], %[i], #1")
            __ASM_EMIT("stp         q4, q6, [%[dst], #0x00]")
            __ASM_EMIT("stp         q5, q7, [%[dst], #0x20]")
            __ASM_EMIT("cmp         %[i], %[regs]")
            __ASM_EMIT("add         %[dst], %[dst], #0x40")
            __ASM_EMIT("b.lo        1b")

            : [dst] "+r" (dst), [sr1] "=&r" (sr1), [sr2] "=&r" (sr2),
              [rrank] "+r" (rrank), [i] "=&r" (i), [j] "=&r" (j)
            : [src] "r" (src), [regs] "r" (regs)
            : "cc", "memory",
              "v0", "v1", "v2", "v3",
              "v4", "v5", "v6", "v7"
        );
    }

    void packed_unscramble_direct(float *dst, size_t rank)
    {
        IF_ARCH_AARCH64(
            size_t count = 1 << rank;
        );

        ARCH_AARCH64_ASM(
            // 16x blocks
            __ASM_EMIT("subs        %[count], %[count], #16")
            __ASM_EMIT("b.lo        2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("ldp         q0, q16, [%[dst], #0x00]")
            __ASM_EMIT("ldp         q2, q17, [%[dst], #0x20]")
            __ASM_EMIT("ldp         q4, q18, [%[dst], #0x40]")
            __ASM_EMIT("ldp         q6, q19, [%[dst], #0x60]")
            __ASM_EMIT("zip2        v1.4s, v0.4s, v16.4s")
            __ASM_EMIT("zip1        v0.4s, v0.4s, v16.4s")
            __ASM_EMIT("zip2        v3.4s, v2.4s, v17.4s")
            __ASM_EMIT("zip1        v2.4s, v2.4s, v17.4s")
            __ASM_EMIT("zip2        v5.4s, v4.4s, v18.4s")
            __ASM_EMIT("zip1        v4.4s, v4.4s, v18.4s")
            __ASM_EMIT("zip2        v7.4s, v6.4s, v19.4s")
            __ASM_EMIT("zip1        v6.4s, v6.4s, v19.4s")
            __ASM_EMIT("stp         q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("stp         q2, q3, [%[dst], #0x20]")
            __ASM_EMIT("stp         q4, q5, [%[dst], #0x40]")
            __ASM_EMIT("stp         q6, q7, [%[dst], #0x60]")
            __ASM_EMIT("subs        %[count], %[count], #16")
            __ASM_EMIT("add         %[dst], %[dst], #0x80")
            __ASM_EMIT("b.hs        1b")
            __ASM_EMIT("2:")
            // 8x block
            __ASM_EMIT("adds        %[count], %[count], #8")
            __ASM_EMIT("b.lt        4f")
            __ASM_EMIT("ldp         q0, q16, [%[dst], #0x00]")
            __ASM_EMIT("ldp         q2, q17, [%[dst], #0x20]")
            __ASM_EMIT("zip2        v1.4s, v0.4s, v16.4s")
            __ASM_EMIT("zip1        v0.4s, v0.4s, v16.4s")
            __ASM_EMIT("zip2        v3.4s, v2.4s, v17.4s")
            __ASM_EMIT("zip1        v2.4s, v2.4s, v17.4s")
            __ASM_EMIT("stp         q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("stp         q2, q3, [%[dst], #0x20]")
            __ASM_EMIT("4:")
            : [dst] "+r" (dst),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "v0", "v1", "v2", "v3",
              "v4", "v5", "v6", "v7",
              "v16", "v17", "v18", "v19"
        );
    }

    void packed_unscramble_reverse(float *dst, size_t rank)
    {
        IF_ARCH_AARCH64(
            size_t count = 1 << rank;
            float k = 1.0f/(1 << rank);
        );

        ARCH_AARCH64_ASM(
            __ASM_EMIT("dup         v31.4s, %S[k].s[0]")             // v31   = k
            // 16x blocks
            __ASM_EMIT("subs        %[count], %[count], #16")
            __ASM_EMIT("mov         v30.16b, v31.16b")
            __ASM_EMIT("b.lo        2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("ldp         q0, q16, [%[dst], #0x00]")
            __ASM_EMIT("ldp         q2, q17, [%[dst], #0x20]")
            __ASM_EMIT("ldp         q4, q18, [%[dst], #0x40]")
            __ASM_EMIT("ldp         q6, q19, [%[dst], #0x60]")
            __ASM_EMIT("zip2        v1.4s, v0.4s, v16.4s")
            __ASM_EMIT("zip1        v0.4s, v0.4s, v16.4s")
            __ASM_EMIT("fmul        v1.4s, v1.4s, v30.4s")
            __ASM_EMIT("fmul        v0.4s, v0.4s, v31.4s")
            __ASM_EMIT("zip2        v3.4s, v2.4s, v17.4s")
            __ASM_EMIT("zip1        v2.4s, v2.4s, v17.4s")
            __ASM_EMIT("fmul        v3.4s, v3.4s, v30.4s")
            __ASM_EMIT("fmul        v2.4s, v2.4s, v31.4s")
            __ASM_EMIT("zip2        v5.4s, v4.4s, v18.4s")
            __ASM_EMIT("zip1        v4.4s, v4.4s, v18.4s")
            __ASM_EMIT("fmul        v5.4s, v5.4s, v30.4s")
            __ASM_EMIT("fmul        v4.4s, v4.4s, v31.4s")
            __ASM_EMIT("zip2        v7.4s, v6.4s, v19.4s")
            __ASM_EMIT("zip1        v6.4s, v6.4s, v19.4s")
            __ASM_EMIT("fmul        v7.4s, v7.4s, v30.4s")
            __ASM_EMIT("fmul        v6.4s, v6.4s, v31.4s")
            __ASM_EMIT("stp         q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("stp         q2, q3, [%[dst], #0x20]")
            __ASM_EMIT("stp         q4, q5, [%[dst], #0x40]")
            __ASM_EMIT("stp         q6, q7, [%[dst], #0x60]")
            __ASM_EMIT("subs        %[count], %[count], #16")
            __ASM_EMIT("add         %[dst], %[dst], #0x80")
            __ASM_EMIT("b.hs        1b")
            __ASM_EMIT("2:")
            // 8x block
            __ASM_EMIT("adds        %[count], %[count], #8")
            __ASM_EMIT("b.lt        4f")
            __ASM_EMIT("ldp         q0, q16, [%[dst], #0x00]")
            __ASM_EMIT("ldp         q2, q17, [%[dst], #0x20]")
            __ASM_EMIT("zip2        v1.4s, v0.4s, v16.4s")
            __ASM_EMIT("zip1        v0.4s, v0.4s, v16.4s")
            __ASM_EMIT("fmul        v1.4s, v1.4s, v30.4s")
            __ASM_EMIT("fmul        v0.4s, v0.4s, v31.4s")
            __ASM_EMIT("zip2        v3.4s, v2.4s, v17.4s")
            __ASM_EMIT("zip1        v2.4s, v2.4s, v17.4s")
            __ASM_EMIT("fmul        v3.4s, v3.4s, v30.4s")
            __ASM_EMIT("fmul        v2.4s, v2.4s, v31.4s")
            __ASM_EMIT("stp         q0, q1, [%[dst], #0x00]")
            __ASM_EMIT("stp         q2, q3, [%[dst], #0x20]")
            __ASM_EMIT("4:")
            : [dst] "+r" (dst),
              [count] "+r" (count),
              [k] "+w" (k)
            :
            : "cc", "memory",
              "v0", "v1", "v2", "v3",
              "v4", "v5", "v6", "v7",
              "v16", "v17", "v18", "v19",
              "v30", "v31"
        );
    }
}

#endif /* DSP_ARCH_AARCH64_ASIMD_FFT_PSCRAMBLE_H_ */
