/*
 * scramble.h
 *
 *  Created on: 17 дек. 2019 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_AARCH64_ASIMD_FFT_SCRAMBLE_H_
#define DSP_ARCH_AARCH64_ASIMD_FFT_SCRAMBLE_H_

#ifndef DSP_ARCH_AARCH64_ASIMD_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_AARCH64_ASIMD_IMPL */

namespace asimd
{
    static inline void scramble_self_direct(float *dst_re, float *dst_im, size_t rank)
    {
        IF_ARCH_AARCH64(
            size_t i, j;
            float *d_re, *d_im;
            float *src_re = dst_re, *src_im = dst_im;
            size_t count = 1 << rank, rrank = 64 - rank;
        );

        // Self algorithm
        ARCH_AARCH64_ASM(
            // Do bit-reverse shuffle
            __ASM_EMIT("mov         %[i], #1")                          // i = 1
            __ASM_EMIT("1:")
            __ASM_EMIT("rbit        %[j], %[i]")                        // j = reverse_bits(i)
            __ASM_EMIT("add         %[src_re], %[src_re], #4")
            __ASM_EMIT("add         %[src_im], %[src_im], #4")
            __ASM_EMIT("lsr         %[j], %[j], %[rrank]")              // j = reverse_bits(i) >> rrank
            __ASM_EMIT("cmp         %[i], %[j]")                        // i <=> j
            __ASM_EMIT("b.hs        2f")                                // if (i >= j) continue
            __ASM_EMIT("add         %[d_re], %[dst_re], %[j], LSL #2")  // d_re = &dst_re[j]
            __ASM_EMIT("add         %[d_im], %[dst_im], %[j], LSL #2")  // d_im = &dst_im[j]
            __ASM_EMIT("ld1         {v0.s}[0], [%[src_re]]")            // v0 = *src_re
            __ASM_EMIT("ld1         {v1.s}[0], [%[src_im]]")            // v1 = *src_im
            __ASM_EMIT("ld1         {v2.s}[0], [%[d_re]]")              // v2 = *td_re
            __ASM_EMIT("ld1         {v3.s}[0], [%[d_im]]")              // v3 = *td_im
            __ASM_EMIT("st1         {v2.s}[0], [%[src_re]]")            // *(src_re++) = s2
            __ASM_EMIT("st1         {v3.s}[0], [%[src_im]]")            // *(src_im++) = s3
            __ASM_EMIT("st1         {v0.s}[0], [%[d_re]]")              // *d_re = s0
            __ASM_EMIT("st1         {v1.s}[0], [%[d_im]]")              // *d_im = s1
            __ASM_EMIT("2:")
            __ASM_EMIT("add         %[i], %[i], #1")                    // i++
            __ASM_EMIT("cmp         %[i], %[count]")                    // i <=> count
            __ASM_EMIT("b.lo        1b")

            // Perform x8 butterflies
            __ASM_EMIT("3:")
            __ASM_EMIT("ld2         {v0.4s, v1.4s}, [%[dst_re]]")       // v0 = r0 r2 r4 r6, v1 = r1 r3 r5 r7
            __ASM_EMIT("ld2         {v2.4s, v3.4s}, [%[dst_im]]")       // v2 = i0 i2 i4 i6, v3 = i1 i3 i5 i7
            __ASM_EMIT("fadd        v6.4s, v0.4s, v1.4s")               // v6 = r0+r1 r2+r3 r4+r5 r6+r7 = r0' r2' r4' r6'
            __ASM_EMIT("fadd        v7.4s, v2.4s, v3.4s")               // v7 = i0+i1 i2+i3 i4+i5 i6+i7 = i0' i2' i4' i6'
            __ASM_EMIT("fsub        v0.4s, v0.4s, v1.4s")               // v0 = r0-r1 r2-r3 r4-r5 r6-r7 = r1' r3' r5' r7'
            __ASM_EMIT("fsub        v1.4s, v2.4s, v3.4s")               // v1 = i0-i1 i2-i3 i4-i5 i6-i7 = i1' i3' i5' i7'
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
            __ASM_EMIT("stp         q4, q5, [%[dst_re]]")
            __ASM_EMIT("stp         q6, q7, [%[dst_im]]")
            __ASM_EMIT("subs        %[count], %[count], #8")            // i <=> count
            __ASM_EMIT("add         %[dst_re], %[dst_re], #0x20")
            __ASM_EMIT("add         %[dst_im], %[dst_im], #0x20")
            __ASM_EMIT("b.ne        3b")

            : [src_re] "+r" (src_re), [src_im] "+r" (src_im),
              [dst_re] "+r" (dst_re), [dst_im] "+r" (dst_im),
              [d_re] "=&r" (d_re), [d_im] "=&r" (d_im),
              [i] "=&r" (i), [j] "=&r" (j),
              [count] "+r" (count)
            : [rrank] "r" (rrank)
            : "cc", "memory",
              "v0", "v1", "v2", "v3",
              "v4", "v5", "v6", "v7"
        );
    }

    static inline void scramble_copy_direct(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank)
    {
        IF_ARCH_AARCH64(
            size_t i, j;
            float *s_re, *s_im;
            size_t regs = 1 << (rank - 3), rrank = 64 - (rank - 3);
        );

        ARCH_AARCH64_ASM(
            __ASM_EMIT("eor         %[i], %[i], %[i]")                  // i = 0
            __ASM_EMIT("1:")
            __ASM_EMIT("rbit        %[j], %[i]")                        // j = reverse_bits(i)
            __ASM_EMIT("lsr         %[j], %[j], %[rrank]")              // j = reverse_bits(i) >> rrank

            __ASM_EMIT("add         %[s_re], %[src_re], %[j], LSL #2")  // s_re = &src_re[i]
            __ASM_EMIT("add         %[s_im], %[src_im], %[j], LSL #2")  // s_re = &src_im[i]
            __ASM_EMIT("ld1         {v0.s}[0], [%[s_re]]")              // v0 = r0 ? ? ?
            __ASM_EMIT("ld1         {v2.s}[0], [%[s_im]]")              // v2 = i0 ? ? ?
            __ASM_EMIT("add         %[s_re], %[s_re], %[regs], LSL #2") // s_re = &src_re[i + regs]
            __ASM_EMIT("add         %[s_im], %[s_im], %[regs], LSL #2") // s_re = &src_im[i + regs]
            __ASM_EMIT("ld1         {v0.s}[2], [%[s_re]]")              // v0 = r0 ? r4 ?
            __ASM_EMIT("ld1         {v2.s}[2], [%[s_im]]")              // v2 = i0 ? i4 ?
            __ASM_EMIT("add         %[s_re], %[s_re], %[regs], LSL #2") // s_re = &src_re[i + regs*2]
            __ASM_EMIT("add         %[s_im], %[s_im], %[regs], LSL #2") // s_re = &src_im[i + regs*2]
            __ASM_EMIT("ld1         {v0.s}[1], [%[s_re]]")              // v0 = r0 r2 r4 ?
            __ASM_EMIT("ld1         {v2.s}[1], [%[s_im]]")              // v2 = i0 i2 i4 ?
            __ASM_EMIT("add         %[s_re], %[s_re], %[regs], LSL #2") // s_re = &src_re[i + regs*3]
            __ASM_EMIT("add         %[s_im], %[s_im], %[regs], LSL #2") // s_re = &src_im[i + regs*3]
            __ASM_EMIT("ld1         {v0.s}[3], [%[s_re]]")              // v0 = r0 r2 r4 r6
            __ASM_EMIT("ld1         {v2.s}[3], [%[s_im]]")              // v2 = i0 i2 i4 i6

            __ASM_EMIT("add         %[s_re], %[s_re], %[regs], LSL #2") // s_re = &src_re[i + regs*4]
            __ASM_EMIT("add         %[s_im], %[s_im], %[regs], LSL #2") // s_re = &src_im[i + regs*4]
            __ASM_EMIT("ld1         {v1.s}[0], [%[s_re]]")              // v1 = r1 ? ? ?
            __ASM_EMIT("ld1         {v3.s}[0], [%[s_im]]")              // v3 = i1 ? ? ?
            __ASM_EMIT("add         %[s_re], %[s_re], %[regs], LSL #2") // s_re = &src_re[i + regs*5]
            __ASM_EMIT("add         %[s_im], %[s_im], %[regs], LSL #2") // s_re = &src_im[i + regs*5]
            __ASM_EMIT("ld1         {v1.s}[2], [%[s_re]]")              // v1 = r1 ? r5 ?
            __ASM_EMIT("ld1         {v3.s}[2], [%[s_im]]")              // v3 = i1 ? i5 ?
            __ASM_EMIT("add         %[s_re], %[s_re], %[regs], LSL #2") // s_re = &src_re[i + regs*6]
            __ASM_EMIT("add         %[s_im], %[s_im], %[regs], LSL #2") // s_re = &src_im[i + regs*6]
            __ASM_EMIT("ld1         {v1.s}[1], [%[s_re]]")              // v1 = r1 r3 r5 ?
            __ASM_EMIT("ld1         {v3.s}[1], [%[s_im]]")              // v3 = i1 i3 i5 ?
            __ASM_EMIT("add         %[s_re], %[s_re], %[regs], LSL #2") // s_re = &src_re[i + regs*7]
            __ASM_EMIT("add         %[s_im], %[s_im], %[regs], LSL #2") // s_re = &src_im[i + regs*7]
            __ASM_EMIT("ld1         {v1.s}[3], [%[s_re]]")              // v1 = r1 r3 r5 r7
            __ASM_EMIT("ld1         {v3.s}[3], [%[s_im]]")              // v3 = i1 i3 i5 i7
            __ASM_EMIT("add         %[i], %[i], #1")
            // v0 = r0 r2 r4 r6
            // v1 = r1 r3 r5 r7
            // v2 = i0 i2 i4 i6
            // v3 = i1 i3 i5 i7
            __ASM_EMIT("fadd        v6.4s, v0.4s, v1.4s")               // v6 = r0+r1 r2+r3 r4+r5 r6+r7 = r0' r2' r4' r6'
            __ASM_EMIT("fadd        v7.4s, v2.4s, v3.4s")               // v7 = i0+i1 i2+i3 i4+i5 i6+i7 = i0' i2' i4' i6'
            __ASM_EMIT("fsub        v0.4s, v0.4s, v1.4s")               // v0 = r0-r1 r2-r3 r4-r5 r6-r7 = r1' r3' r5' r7'
            __ASM_EMIT("fsub        v1.4s, v2.4s, v3.4s")               // v1 = i0-i1 i2-i3 i4-i5 i6-i7 = i1' i3' i5' i7'
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
            __ASM_EMIT("stp         q4, q5, [%[dst_re]]")
            __ASM_EMIT("stp         q6, q7, [%[dst_im]]")
            __ASM_EMIT("cmp         %[i], %[regs]")
            __ASM_EMIT("add         %[dst_re], %[dst_re], #0x20")
            __ASM_EMIT("add         %[dst_im], %[dst_im], #0x20")
            __ASM_EMIT("b.lo        1b")

            : [dst_re] "+r" (dst_re), [dst_im] "+r" (dst_im),
              [s_re] "=&r" (s_re), [s_im] "=&r" (s_im),
              [i] "=&r" (i), [j] "=&r" (j)
            : [src_re] "r" (src_re), [src_im] "r" (src_im),
              [rrank] "r" (rrank), [regs] "r" (regs)
            : "cc", "memory",
              "v0", "v1", "v2", "v3",
              "v4", "v5", "v6", "v7"
        );
    }

    static inline void scramble_self_reverse(float *dst_re, float *dst_im, size_t rank)
    {
        IF_ARCH_AARCH64(
            size_t i, j;
            float *d_re, *d_im;
            size_t count = 1 << rank, rrank = 64 - rank;
            float *src_re = dst_re, *src_im = dst_im;
        );

        // Self algorithm
        ARCH_AARCH64_ASM(
            // Do bit-reverse shuffle
            __ASM_EMIT("mov         %[i], #1")                          // i = 1
            __ASM_EMIT("1:")
            __ASM_EMIT("rbit        %[j], %[i]")                        // j = reverse_bits(i)
            __ASM_EMIT("add         %[src_re], %[src_re], #4")
            __ASM_EMIT("add         %[src_im], %[src_im], #4")
            __ASM_EMIT("lsr         %[j], %[j], %[rrank]")              // j = reverse_bits(i) >> rrank
            __ASM_EMIT("cmp         %[i], %[j]")                        // i <=> j
            __ASM_EMIT("b.hs        2f")                                // if (i >= j) continue
            __ASM_EMIT("add         %[d_re], %[dst_re], %[j], LSL #2")  // d_re = &dst_re[j]
            __ASM_EMIT("add         %[d_im], %[dst_im], %[j], LSL #2")  // d_im = &dst_im[j]
            __ASM_EMIT("ld1         {v0.s}[0], [%[src_re]]")            // v0 = *src_re
            __ASM_EMIT("ld1         {v1.s}[0], [%[src_im]]")            // v1 = *src_im
            __ASM_EMIT("ld1         {v2.s}[0], [%[d_re]]")              // v2 = *td_re
            __ASM_EMIT("ld1         {v3.s}[0], [%[d_im]]")              // v3 = *td_im
            __ASM_EMIT("st1         {v2.s}[0], [%[src_re]]")            // *(src_re++) = s2
            __ASM_EMIT("st1         {v3.s}[0], [%[src_im]]")            // *(src_im++) = s3
            __ASM_EMIT("st1         {v0.s}[0], [%[d_re]]")              // *d_re = s0
            __ASM_EMIT("st1         {v1.s}[0], [%[d_im]]")              // *d_im = s1
            __ASM_EMIT("2:")
            __ASM_EMIT("add         %[i], %[i], #1")                    // i++
            __ASM_EMIT("cmp         %[i], %[count]")                    // i <=> count
            __ASM_EMIT("b.lo        1b")
            // Perform x8 butterflies
            __ASM_EMIT("3:")
            __ASM_EMIT("ld2         {v0.4s, v1.4s}, [%[dst_re]]")       // v0 = r0 r2 r4 r6, v1 = r1 r3 r5 r7
            __ASM_EMIT("ld2         {v2.4s, v3.4s}, [%[dst_im]]")       // v2 = i0 i2 i4 i6, v3 = i1 i3 i5 i7
            __ASM_EMIT("fadd        v6.4s, v0.4s, v1.4s")               // v6 = r0+r1 r2+r3 r4+r5 r6+r7 = r0' r2' r4' r6'
            __ASM_EMIT("fadd        v7.4s, v2.4s, v3.4s")               // v7 = i0+i1 i2+i3 i4+i5 i6+i7 = i0' i2' i4' i6'
            __ASM_EMIT("fsub        v0.4s, v0.4s, v1.4s")               // v0 = r0-r1 r2-r3 r4-r5 r6-r7 = r1' r3' r5' r7'
            __ASM_EMIT("fsub        v1.4s, v2.4s, v3.4s")               // v1 = i0-i1 i2-i3 i4-i5 i6-i7 = i1' i3' i5' i7'
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
            __ASM_EMIT("stp         q4, q5, [%[dst_re]]")
            __ASM_EMIT("stp         q6, q7, [%[dst_im]]")
            __ASM_EMIT("subs        %[count], %[count], #8")            // i <=> count
            __ASM_EMIT("add         %[dst_re], %[dst_re], #0x20")
            __ASM_EMIT("add         %[dst_im], %[dst_im], #0x20")
            __ASM_EMIT("b.ne        3b")

            : [src_re] "+r" (src_re), [src_im] "+r" (src_im),
              [dst_re] "+r" (dst_re), [dst_im] "+r" (dst_im),
              [d_re] "=&r" (d_re), [d_im] "=&r" (d_im),
              [i] "=&r" (i), [j] "=&r" (j),
              [count] "+r" (count)
            : [rrank] "r" (rrank)
            : "cc", "memory",
              "v0", "v1", "v2", "v3",
              "v4", "v5", "v6", "v7"
        );
    }

    static inline void scramble_copy_reverse(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank)
    {
        IF_ARCH_AARCH64(
            size_t i, j;
            float *s_re, *s_im;
            size_t regs = 1 << (rank - 3), rrank = 64 - (rank - 3);
        );

        ARCH_AARCH64_ASM(
            __ASM_EMIT("eor         %[i], %[i], %[i]")                  // i = 0
            __ASM_EMIT("1:")
            __ASM_EMIT("rbit        %[j], %[i]")                        // j = reverse_bits(i)
            __ASM_EMIT("lsr         %[j], %[j], %[rrank]")              // j = reverse_bits(i) >> rrank

            __ASM_EMIT("add         %[s_re], %[src_re], %[j], LSL #2")  // s_re = &src_re[i]
            __ASM_EMIT("add         %[s_im], %[src_im], %[j], LSL #2")  // s_re = &src_im[i]
            __ASM_EMIT("ld1         {v0.s}[0], [%[s_re]]")              // v0 = r0 ? ? ?
            __ASM_EMIT("ld1         {v2.s}[0], [%[s_im]]")              // v2 = i0 ? ? ?
            __ASM_EMIT("add         %[s_re], %[s_re], %[regs], LSL #2") // s_re = &src_re[i + regs]
            __ASM_EMIT("add         %[s_im], %[s_im], %[regs], LSL #2") // s_re = &src_im[i + regs]
            __ASM_EMIT("ld1         {v0.s}[2], [%[s_re]]")              // v0 = r0 ? r4 ?
            __ASM_EMIT("ld1         {v2.s}[2], [%[s_im]]")              // v2 = i0 ? i4 ?
            __ASM_EMIT("add         %[s_re], %[s_re], %[regs], LSL #2") // s_re = &src_re[i + regs*2]
            __ASM_EMIT("add         %[s_im], %[s_im], %[regs], LSL #2") // s_re = &src_im[i + regs*2]
            __ASM_EMIT("ld1         {v0.s}[1], [%[s_re]]")              // v0 = r0 r2 r4 ?
            __ASM_EMIT("ld1         {v2.s}[1], [%[s_im]]")              // v2 = i0 i2 i4 ?
            __ASM_EMIT("add         %[s_re], %[s_re], %[regs], LSL #2") // s_re = &src_re[i + regs*3]
            __ASM_EMIT("add         %[s_im], %[s_im], %[regs], LSL #2") // s_re = &src_im[i + regs*3]
            __ASM_EMIT("ld1         {v0.s}[3], [%[s_re]]")              // v0 = r0 r2 r4 r6
            __ASM_EMIT("ld1         {v2.s}[3], [%[s_im]]")              // v2 = i0 i2 i4 i6

            __ASM_EMIT("add         %[s_re], %[s_re], %[regs], LSL #2") // s_re = &src_re[i + regs*4]
            __ASM_EMIT("add         %[s_im], %[s_im], %[regs], LSL #2") // s_re = &src_im[i + regs*4]
            __ASM_EMIT("ld1         {v1.s}[0], [%[s_re]]")              // v1 = r1 ? ? ?
            __ASM_EMIT("ld1         {v3.s}[0], [%[s_im]]")              // v3 = i1 ? ? ?
            __ASM_EMIT("add         %[s_re], %[s_re], %[regs], LSL #2") // s_re = &src_re[i + regs*5]
            __ASM_EMIT("add         %[s_im], %[s_im], %[regs], LSL #2") // s_re = &src_im[i + regs*5]
            __ASM_EMIT("ld1         {v1.s}[2], [%[s_re]]")              // v1 = r1 ? r5 ?
            __ASM_EMIT("ld1         {v3.s}[2], [%[s_im]]")              // v3 = i1 ? i5 ?
            __ASM_EMIT("add         %[s_re], %[s_re], %[regs], LSL #2") // s_re = &src_re[i + regs*6]
            __ASM_EMIT("add         %[s_im], %[s_im], %[regs], LSL #2") // s_re = &src_im[i + regs*6]
            __ASM_EMIT("ld1         {v1.s}[1], [%[s_re]]")              // v1 = r1 r3 r5 ?
            __ASM_EMIT("ld1         {v3.s}[1], [%[s_im]]")              // v3 = i1 i3 i5 ?
            __ASM_EMIT("add         %[s_re], %[s_re], %[regs], LSL #2") // s_re = &src_re[i + regs*7]
            __ASM_EMIT("add         %[s_im], %[s_im], %[regs], LSL #2") // s_re = &src_im[i + regs*7]
            __ASM_EMIT("ld1         {v1.s}[3], [%[s_re]]")              // v1 = r1 r3 r5 r7
            __ASM_EMIT("ld1         {v3.s}[3], [%[s_im]]")              // v3 = i1 i3 i5 i7
            __ASM_EMIT("add         %[i], %[i], #1")
            // v0 = r0 r2 r4 r6
            // v1 = r1 r3 r5 r7
            // v2 = i0 i2 i4 i6
            // v3 = i1 i3 i5 i7
            __ASM_EMIT("fadd        v6.4s, v0.4s, v1.4s")               // v6 = r0+r1 r2+r3 r4+r5 r6+r7 = r0' r2' r4' r6'
            __ASM_EMIT("fadd        v7.4s, v2.4s, v3.4s")               // v7 = i0+i1 i2+i3 i4+i5 i6+i7 = i0' i2' i4' i6'
            __ASM_EMIT("fsub        v0.4s, v0.4s, v1.4s")               // v0 = r0-r1 r2-r3 r4-r5 r6-r7 = r1' r3' r5' r7'
            __ASM_EMIT("fsub        v1.4s, v2.4s, v3.4s")               // v1 = i0-i1 i2-i3 i4-i5 i6-i7 = i1' i3' i5' i7'
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
            __ASM_EMIT("stp         q4, q5, [%[dst_re]]")
            __ASM_EMIT("stp         q6, q7, [%[dst_im]]")
            __ASM_EMIT("cmp         %[i], %[regs]")
            __ASM_EMIT("add         %[dst_re], %[dst_re], #0x20")
            __ASM_EMIT("add         %[dst_im], %[dst_im], #0x20")
            __ASM_EMIT("blo         1b")

            : [dst_re] "+r" (dst_re), [dst_im] "+r" (dst_im),
              [s_re] "=&r" (s_re), [s_im] "=&r" (s_im),
              [i] "=&r" (i), [j] "=&r" (j)
            : [src_re] "r" (src_re), [src_im] "r" (src_im),
              [rrank] "r" (rrank), [regs] "r" (regs)
            : "cc", "memory",
              "v0", "v1", "v2", "v3",
              "v4", "v5", "v6", "v7"
        );
    }
}


#endif /* DSP_ARCH_AARCH64_ASIMD_FFT_SCRAMBLE_H_ */
