/*
 * pscramble.h
 *
 *  Created on: 29 окт. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_ARM_NEON_D32_FFT_PSCRAMBLE_H_
#define DSP_ARCH_ARM_NEON_D32_FFT_PSCRAMBLE_H_

#ifndef DSP_ARCH_ARM_NEON_32_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_ARM_NEON_32_IMPL */

namespace neon_d32
{
    void packed_scramble_direct(float *dst, const float *src, size_t rank)
    {
        IF_ARCH_ARM6(uint32_t msk, tmp);

        if (dst == src)
        {
            IF_ARCH_ARM(
                size_t count = 1 << rank;
                size_t i, j, rrank = rank;
                float *dp;
            );

            // Self algorithm
            ARCH_ARM_ASM(
                // Do bit-reverse shuffle
                __ASM_EMIT("rsb         %[rrank], %[rrank], $32")           // rrank = 32 - rank
                __ASM_EMIT("mov         %[i], $1")                          // i = 1

                __ASM_EMIT("1:")
#if defined(ARCH_ARM6)
                ARMV6_MV_RBIT32("%[j]", "%[i]", "%[msk]", "%[tmp]", "%[masks]") // j = reverse_bits(i)
#else
                __ASM_EMIT("rbit        %[j], %[i]")                        // j = reverse_bits(i)
#endif
                __ASM_EMIT("add         %[src], $8")
                __ASM_EMIT("lsr         %[j], %[rrank]")                    // j = reverse_bits(i) >> rank
                __ASM_EMIT("cmp         %[i], %[j]")                        // i <=> j
                __ASM_EMIT("bhs         2f")                                // if (i >= j) continue
                __ASM_EMIT("add         %[dp], %[dst], %[j], LSL $3")       // d_re = &dst[j]
                __ASM_EMIT("vld1.32     {d0}, [%[src]]")                    // d0   = *src
                __ASM_EMIT("vld1.32     {d1}, [%[dp]]")                     // d1   = *dp
                __ASM_EMIT("vst1.32     {d1}, [%[src]]")                    // *src = d1
                __ASM_EMIT("vst1.32     {d0}, [%[dp]]")                     // *dp  = d0
                __ASM_EMIT("2:")
                __ASM_EMIT("add         %[i], $1")                          // i++
                __ASM_EMIT("cmp         %[i], %[count]")                    // i <=> count
                __ASM_EMIT("blo         1b")

                __ASM_EMIT("eor         %[i], %[i]")
                __ASM_EMIT("mov         %[src], %[dst]")                    // restore src == dst

                // Perform x8 butterflies
                __ASM_EMIT("3:")

                __ASM_EMIT("vld4.32     {q0-q1}, [%[src]]!")                // q0 = r0 r2 i0 i2, q1 = r1 r3 i1 i3
                __ASM_EMIT("vld4.32     {q2-q3}, [%[src]]!")                // q2 = r4 r6 i4 i6, q3 = r5 r7 i5 i7

                __ASM_EMIT("vadd.f32    q4, q0, q1")                        // q4 = r0+r1 r2+r3 i0+i1 i2+i3 = r0' r2' i0' i2'
                __ASM_EMIT("vadd.f32    q5, q2, q3")                        // q5 = r4+r5 r6+r7 i4+i5 i6+i7 = r4' r6' i4' i6'
                __ASM_EMIT("vsub.f32    q0, q0, q1")                        // q0 = r0-r1 r2-r3 i0-i1 i2-i3 = r1' r3' i1' i3'
                __ASM_EMIT("vsub.f32    q1, q2, q3")                        // q1 = r4-r5 r6-r7 i4-i5 i6-i7 = r5' r7' i5' i7'

                // q4 = r0' r2' i0' i2'
                // q5 = r4' r6' i4' i6'
                // q0 = r1' r3' i1' i3'
                // q1 = r5' r7' i5' i7'
                __ASM_EMIT("vtrn.32     q4, q5")                            // q4 = r0' r4' i0' i4', q5 = r2' r6' i2' i6'
                __ASM_EMIT("vtrn.32     q0, q1")                            // q0 = r1' r5' i1' i5', q1 = r3' r7' i3' i7'
                __ASM_EMIT("vext.32     q6, q1, q1, $2")                    // q6 = i3' i7' r3' r7'

                __ASM_EMIT("vadd.f32    q2, q4, q5")                        // q2 = r0'+r2' r4'+r6' i0'+i2' i4'+i6' = r0" r4" i0" i4"
                __ASM_EMIT("vsub.f32    q1, q0, q6")                        // q1 = r1'-i3' r5'-i7' i1'-r3' i5'-r7' = r3" r7" i1" i5"
                __ASM_EMIT("vsub.f32    q3, q4, q5")                        // q3 = r0'-r2' r4'-r6' i0'-i2' i4'-i6' = r2" r6" i2" i6"
                __ASM_EMIT("vadd.f32    q0, q0, q6")                        // q0 = r1'+i3' r5'+i7' i1'+r3' i5'+r7' = r1" r5" i3" i7"

                // q2 = r0" r4" i0" i4"
                // q0 = r1" r5" i3" i7"
                // q3 = r2" r6" i2" i6"
                // q1 = r3" r7" i1" i5"

                __ASM_EMIT("vtrn.32     q2, q0")                            // q2 = r0" r1" i0" i3", q0 = r4" r5" i4" i7"
                __ASM_EMIT("vtrn.32     q3, q1")                            // q3 = r2" r3" i2" i1", q1 = r6" r7" i6" i5"
                __ASM_EMIT("vswp        d5, d6")                            // q2 = r0" r1" r2" r3", q3 = i0" i3" i2" i1"
                __ASM_EMIT("vswp        d1, d2")                            // q0 = r4" r5" r6" r7", q1 = i4" i7" i6" i5"

                __ASM_EMIT("vrev64.32   q3, q3")                            // q3 = i3" i0" i1" i2"
                __ASM_EMIT("vrev64.32   q1, q1")                            // q1 = i7" i4" i5" i6"
                __ASM_EMIT("vext.32     q3, q3, q3, $1")                    // q3 = i0" i1" i2" i3"
                __ASM_EMIT("vext.32     q1, q1, q1, $1")                    // q1 = i4" i5" i6" i7"

                __ASM_EMIT("vst1.32     {q2-q3}, [%[dst]]!")
                __ASM_EMIT("subs        %[count], $8")                      // i <=> count
                __ASM_EMIT("vst1.32     {q0-q1}, [%[dst]]!")
                __ASM_EMIT("bne         3b")


                : [src] "+r" (src), [dst] "+r" (dst), [dp] "=&r" (dp),
                  [rrank] "+r" (rrank), [i] "=&r" (i), [j] "=&r" (j),
                  [count] "+r" (count)
                  IF_ARCH_ARM6(, [tmp] "=&r" (tmp), [msk] "=&r" (msk))
                : IF_ARCH_ARM6([masks] "r" (__rb_masks))
                : "cc", "memory",
                  "q0", "q1", "q2", "q3", "q4", "q5", "q6"
            );
        }
        else
        {
            IF_ARCH_ARM(
                    size_t i, j, rrank = rank - 3;
                    size_t regs = 1 << rrank;
                    float *sr1, *sr2;
            );

            ARCH_ARM_ASM(
                __ASM_EMIT("eor         %[i], %[i]")                        // i = 0
                __ASM_EMIT("rsb         %[rrank], %[rrank], $32")           // rrank = 32 - rank

                __ASM_EMIT("1:")
#if defined(ARCH_ARM6)
                ARMV6_MV_RBIT32("%[j]", "%[i]", "%[msk]", "%[tmp]", "%[masks]") // j = reverse_bits(i)
#else
                __ASM_EMIT("rbit        %[j], %[i]")                        // j = reverse_bits(i)
#endif
                __ASM_EMIT("lsr         %[j], %[rrank]")                    // j = reverse_bits(i) >> rank

                __ASM_EMIT("add         %[sr1], %[src], %[j], LSL $3")      // sr1 = &src[j]
                __ASM_EMIT("add         %[sr2], %[sr1], %[regs], LSL $3")   // sr2 = &src[j + regs]
                __ASM_EMIT("vldm        %[sr1], {d0}")                      // q0 = r0 i0 ? ?
                __ASM_EMIT("vldm        %[sr2], {d4}")                      // q2 = r4 i4 ? ?
                __ASM_EMIT("add         %[sr1], %[sr1], %[regs], LSL $4")   // sr1 = &src[j + regs*2]
                __ASM_EMIT("add         %[sr2], %[sr2], %[regs], LSL $4")   // sr2 = &src[j + regs*3]
                __ASM_EMIT("vldm        %[sr1], {d1}")                      // q0 = r0 i0 r2 i2
                __ASM_EMIT("vldm        %[sr2], {d5}")                      // q2 = r4 i4 r6 i6

                __ASM_EMIT("add         %[sr1], %[sr1], %[regs], LSL $4")   // sr1 = &src[j + regs*4]
                __ASM_EMIT("add         %[sr2], %[sr2], %[regs], LSL $4")   // sr2 = &src[j + regs*5]
                __ASM_EMIT("vldm        %[sr1], {d2}")                      // q1 = r1 i1 ? ?
                __ASM_EMIT("vldm        %[sr2], {d6}")                      // q3 = r5 i5 ? ?
                __ASM_EMIT("add         %[sr1], %[sr1], %[regs], LSL $4")   // sr1 = &src[j + regs*6]
                __ASM_EMIT("add         %[sr2], %[sr2], %[regs], LSL $4")   // sr2 = &src[j + regs*7]
                __ASM_EMIT("vldm        %[sr1], {d3}")                      // q1 = r1 i1 r3 i3
                __ASM_EMIT("vldm        %[sr2], {d7}")                      // q3 = r5 i5 r7 i7

                __ASM_EMIT("vtrn.32     d0, d1")                            // q0 = r0 r2 i0 i2
                __ASM_EMIT("vtrn.32     d2, d3")                            // q1 = r1 r3 i1 i3
                __ASM_EMIT("vtrn.32     d4, d5")                            // q2 = r4 r6 i4 i6
                __ASM_EMIT("vtrn.32     d6, d7")                            // q3 = r5 r7 i5 i7

                // q0 = r0 r2 i0 i2
                // q1 = r1 r3 i1 i3
                // q2 = r4 r6 i4 i6
                // q3 = r5 r7 i5 i7
                __ASM_EMIT("add         %[i], $1")

                __ASM_EMIT("vadd.f32    q4, q0, q1")                        // q4 = r0+r1 r2+r3 i0+i1 i2+i3 = r0' r2' i0' i2'
                __ASM_EMIT("vadd.f32    q5, q2, q3")                        // q5 = r4+r5 r6+r7 i4+i5 i6+i7 = r4' r6' i4' i6'
                __ASM_EMIT("vsub.f32    q0, q0, q1")                        // q0 = r0-r1 r2-r3 i0-i1 i2-i3 = r1' r3' i1' i3'
                __ASM_EMIT("vsub.f32    q1, q2, q3")                        // q1 = r4-r5 r6-r7 i4-i5 i6-i7 = r5' r7' i5' i7'

                // q4 = r0' r2' i0' i2'
                // q5 = r4' r6' i4' i6'
                // q0 = r1' r3' i1' i3'
                // q1 = r5' r7' i5' i7'
                __ASM_EMIT("vtrn.32     q4, q5")                            // q4 = r0' r4' i0' i4', q5 = r2' r6' i2' i6'
                __ASM_EMIT("vtrn.32     q0, q1")                            // q0 = r1' r5' i1' i5', q1 = r3' r7' i3' i7'
                __ASM_EMIT("vext.32     q6, q1, q1, $2")                    // q6 = i3' i7' r3' r7'

                __ASM_EMIT("vadd.f32    q2, q4, q5")                        // q2 = r0'+r2' r4'+r6' i0'+i2' i4'+i6' = r0" r4" i0" i4"
                __ASM_EMIT("vsub.f32    q1, q0, q6")                        // q1 = r1'-i3' r5'-i7' i1'-r3' i5'-r7' = r3" r7" i1" i5"
                __ASM_EMIT("vsub.f32    q3, q4, q5")                        // q3 = r0'-r2' r4'-r6' i0'-i2' i4'-i6' = r2" r6" i2" i6"
                __ASM_EMIT("vadd.f32    q0, q0, q6")                        // q0 = r1'+i3' r5'+i7' i1'+r3' i5'+r7' = r1" r5" i3" i7"

                // q2 = r0" r4" i0" i4"
                // q0 = r1" r5" i3" i7"
                // q3 = r2" r6" i2" i6"
                // q1 = r3" r7" i1" i5"
                __ASM_EMIT("vtrn.32     q2, q0")                            // q2 = r0" r1" i0" i3", q0 = r4" r5" i4" i7"
                __ASM_EMIT("vtrn.32     q3, q1")                            // q3 = r2" r3" i2" i1", q1 = r6" r7" i6" i5"
                __ASM_EMIT("vswp        d5, d6")                            // q2 = r0" r1" r2" r3", q3 = i0" i3" i2" i1"
                __ASM_EMIT("vswp        d1, d2")                            // q0 = r4" r5" r6" r7", q1 = i4" i7" i6" i5"

                __ASM_EMIT("vrev64.32   q3, q3")                            // q3 = i3" i0" i1" i2"
                __ASM_EMIT("vrev64.32   q1, q1")                            // q1 = i7" i4" i5" i6"
                __ASM_EMIT("vext.32     q3, q3, q3, $1")                    // q3 = i0" i1" i2" i3"
                __ASM_EMIT("vext.32     q1, q1, q1, $1")                    // q1 = i4" i5" i6" i7"

                __ASM_EMIT("vst1.32     {q2-q3}, [%[dst]]!")
                __ASM_EMIT("cmp         %[i], %[regs]")
                __ASM_EMIT("vst1.32     {q0-q1}, [%[dst]]!")
                __ASM_EMIT("blo         1b")

                : [dst] "+r" (dst), [sr1] "=&r" (sr1), [sr2] "=&r" (sr2),
                  [rrank] "+r" (rrank), [i] "=&r" (i), [j] "=&r" (j)
                  IF_ARCH_ARM6(, [tmp] "=&r" (tmp), [msk] "=&r" (msk))
                : [src] "r" (src), [regs] "r" (regs)
                  IF_ARCH_ARM6(, [masks] "r" (__rb_masks))
                : "cc", "memory",
                  "q0", "q1", "q2", "q3", "q4", "q5", "q6"
            );
        }
    }


    void packed_scramble_reverse(float *dst, const float *src, size_t rank)
    {
        IF_ARCH_ARM6(uint32_t msk, tmp);

        if (dst == src)
        {
            IF_ARCH_ARM(
                size_t count = 1 << rank;
                size_t i, j, rrank = rank;
                float *dp;
            );

            // Self algorithm
            ARCH_ARM_ASM(
                // Do bit-reverse shuffle
                __ASM_EMIT("rsb         %[rrank], %[rrank], $32")           // rrank = 32 - rank
                __ASM_EMIT("mov         %[i], $1")                          // i = 1

                __ASM_EMIT("1:")
#if defined(ARCH_ARM6)
                ARMV6_MV_RBIT32("%[j]", "%[i]", "%[msk]", "%[tmp]", "%[masks]") // j = reverse_bits(i)
#else
                __ASM_EMIT("rbit        %[j], %[i]")                        // j = reverse_bits(i)
#endif
                __ASM_EMIT("add         %[src], $8")
                __ASM_EMIT("lsr         %[j], %[rrank]")                    // j = reverse_bits(i) >> rank
                __ASM_EMIT("cmp         %[i], %[j]")                        // i <=> j
                __ASM_EMIT("bhs         2f")                                // if (i >= j) continue
                __ASM_EMIT("add         %[dp], %[dst], %[j], LSL $3")       // d_re = &dst[j]
                __ASM_EMIT("vld1.32     {d0}, [%[src]]")                    // d0   = *src
                __ASM_EMIT("vld1.32     {d1}, [%[dp]]")                     // d1   = *dp
                __ASM_EMIT("vst1.32     {d1}, [%[src]]")                    // *src = d1
                __ASM_EMIT("vst1.32     {d0}, [%[dp]]")                     // *dp  = d0
                __ASM_EMIT("2:")
                __ASM_EMIT("add         %[i], $1")                          // i++
                __ASM_EMIT("cmp         %[i], %[count]")                    // i <=> count
                __ASM_EMIT("blo         1b")

                __ASM_EMIT("eor         %[i], %[i]")
                __ASM_EMIT("mov         %[src], %[dst]")                    // restore src == dst

                // Perform x8 butterflies
                __ASM_EMIT("3:")

                __ASM_EMIT("vld4.32     {q0-q1}, [%[src]]!")                // q0 = r0 r2 i0 i2, q1 = r1 r3 i1 i3
                __ASM_EMIT("vld4.32     {q2-q3}, [%[src]]!")                // q2 = r4 r6 i4 i6, q3 = r5 r7 i5 i7

                __ASM_EMIT("vadd.f32    q4, q0, q1")                        // q4 = r0+r1 r2+r3 i0+i1 i2+i3 = r0' r2' i0' i2'
                __ASM_EMIT("vadd.f32    q5, q2, q3")                        // q5 = r4+r5 r6+r7 i4+i5 i6+i7 = r4' r6' i4' i6'
                __ASM_EMIT("vsub.f32    q0, q0, q1")                        // q0 = r0-r1 r2-r3 i0-i1 i2-i3 = r1' r3' i1' i3'
                __ASM_EMIT("vsub.f32    q1, q2, q3")                        // q1 = r4-r5 r6-r7 i4-i5 i6-i7 = r5' r7' i5' i7'

                // q4 = r0' r2' i0' i2'
                // q5 = r4' r6' i4' i6'
                // q0 = r1' r3' i1' i3'
                // q1 = r5' r7' i5' i7'
                __ASM_EMIT("vtrn.32     q4, q5")                            // q4 = r0' r4' i0' i4', q5 = r2' r6' i2' i6'
                __ASM_EMIT("vtrn.32     q0, q1")                            // q0 = r1' r5' i1' i5', q1 = r3' r7' i3' i7'
                __ASM_EMIT("vext.32     q6, q1, q1, $2")                    // q6 = i3' i7' r3' r7'

                __ASM_EMIT("vsub.f32    q2, q0, q6")                        // q2 = r1'-i3' r5'-i7' i1'-r3' i5'-r7' = r1" r5" i3" i7"
                __ASM_EMIT("vsub.f32    q1, q4, q5")                        // q1 = r0'-r2' r4'-r6' i0'-i2' i4'-i6' = r2" r6" i2" i6"
                __ASM_EMIT("vadd.f32    q3, q0, q6")                        // q3 = r1'+i3' r5'+i7' i1'+r3' i5'+r7' = r3" r7" i1" i5"
                __ASM_EMIT("vadd.f32    q0, q4, q5")                        // q0 = r0'+r2' r4'+r6' i0'+i2' i4'+i6' = r0" r4" i0" i4"

                // q0 = r0" r4" i0" i4"
                // q1 = r2" r6" i2" i6"
                // q2 = r1" r5" i3" i7"
                // q3 = r3" r7" i1" i5"
                __ASM_EMIT("vtrn.32     q0, q2")                            // q0 = r0" r1" i0" i3", q2 = r4" r5" i4" i7"
                __ASM_EMIT("vtrn.32     q1, q3")                            // q1 = r2" r3" i2" i1", q3 = r6" r7" i6" i5"
                __ASM_EMIT("vswp        d1, d2")                            // q0 = r0" r1" r2" r3", q1 = i0" i3" i2" i1"
                __ASM_EMIT("vswp        d5, d6")                            // q2 = r4" r5" r6" r7", q3 = i4" i7" i6" i5"

                __ASM_EMIT("vrev64.32   q1, q1")                            // q1 = i3" i0" i1" i2"
                __ASM_EMIT("vrev64.32   q3, q3")                            // q3 = i7" i4" i5" i6"
                __ASM_EMIT("vext.32     q1, q1, q1, $1")                    // q1 = i0" i1" i2" i3"
                __ASM_EMIT("vext.32     q3, q3, q3, $1")                    // q3 = i4" i5" i6" i7"

                __ASM_EMIT("subs        %[count], $8")                      // i <=> count
                __ASM_EMIT("vstm        %[dst]!, {q0-q3}")
                __ASM_EMIT("bne         3b")


                : [src] "+r" (src), [dst] "+r" (dst), [dp] "=&r" (dp),
                  [rrank] "+r" (rrank), [i] "=&r" (i), [j] "=&r" (j),
                  [count] "+r" (count)
                  IF_ARCH_ARM6(, [tmp] "=&r" (tmp), [msk] "=&r" (msk))
                : IF_ARCH_ARM6([masks] "r" (__rb_masks))
                : "cc", "memory",
                  "q0", "q1", "q2", "q3", "q4", "q5", "q6"
            );
        }
        else
        {
            IF_ARCH_ARM(
                    size_t i, j, rrank = rank - 3;
                    size_t regs = 1 << rrank;
                    float *sr1, *sr2;
            );

            ARCH_ARM_ASM(
                __ASM_EMIT("eor         %[i], %[i]")                        // i = 0
                __ASM_EMIT("rsb         %[rrank], %[rrank], $32")           // rrank = 32 - rank

                __ASM_EMIT("1:")
#if defined(ARCH_ARM6)
                ARMV6_MV_RBIT32("%[j]", "%[i]", "%[msk]", "%[tmp]", "%[masks]") // j = reverse_bits(i)
#else
                __ASM_EMIT("rbit        %[j], %[i]")                        // j = reverse_bits(i)
#endif
                __ASM_EMIT("lsr         %[j], %[rrank]")                    // j = reverse_bits(i) >> rank

                __ASM_EMIT("add         %[sr1], %[src], %[j], LSL $3")      // sr1 = &src[j]
                __ASM_EMIT("add         %[sr2], %[sr1], %[regs], LSL $3")   // sr2 = &src[j + regs]
                __ASM_EMIT("vldm        %[sr1], {d0}")                      // q0 = r0 i0 ? ?
                __ASM_EMIT("vldm        %[sr2], {d4}")                      // q2 = r4 i4 ? ?
                __ASM_EMIT("add         %[sr1], %[sr1], %[regs], LSL $4")   // sr1 = &src[j + regs*2]
                __ASM_EMIT("add         %[sr2], %[sr2], %[regs], LSL $4")   // sr2 = &src[j + regs*3]
                __ASM_EMIT("vldm        %[sr1], {d1}")                      // q0 = r0 i0 r2 i2
                __ASM_EMIT("vldm        %[sr2], {d5}")                      // q2 = r4 i4 r6 i6

                __ASM_EMIT("add         %[sr1], %[sr1], %[regs], LSL $4")   // sr1 = &src[j + regs*4]
                __ASM_EMIT("add         %[sr2], %[sr2], %[regs], LSL $4")   // sr2 = &src[j + regs*5]
                __ASM_EMIT("vldm        %[sr1], {d2}")                      // q1 = r1 i1 ? ?
                __ASM_EMIT("vldm        %[sr2], {d6}")                      // q3 = r5 i5 ? ?
                __ASM_EMIT("add         %[sr1], %[sr1], %[regs], LSL $4")   // sr1 = &src[j + regs*6]
                __ASM_EMIT("add         %[sr2], %[sr2], %[regs], LSL $4")   // sr2 = &src[j + regs*7]
                __ASM_EMIT("vldm        %[sr1], {d3}")                      // q1 = r1 i1 r3 i3
                __ASM_EMIT("vldm        %[sr2], {d7}")                      // q3 = r5 i5 r7 i7

                __ASM_EMIT("vtrn.32     d0, d1")                            // q0 = r0 r2 i0 i2
                __ASM_EMIT("vtrn.32     d2, d3")                            // q1 = r1 r3 i1 i3
                __ASM_EMIT("vtrn.32     d4, d5")                            // q2 = r4 r6 i4 i6
                __ASM_EMIT("vtrn.32     d6, d7")                            // q3 = r5 r7 i5 i7

                // q0 = r0 r2 i0 i2
                // q1 = r1 r3 i1 i3
                // q2 = r4 r6 i4 i6
                // q3 = r5 r7 i5 i7
                __ASM_EMIT("add         %[i], $1")

                __ASM_EMIT("vadd.f32    q4, q0, q1")                        // q4 = r0+r1 r2+r3 i0+i1 i2+i3 = r0' r2' i0' i2'
                __ASM_EMIT("vadd.f32    q5, q2, q3")                        // q5 = r4+r5 r6+r7 i4+i5 i6+i7 = r4' r6' i4' i6'
                __ASM_EMIT("vsub.f32    q0, q0, q1")                        // q0 = r0-r1 r2-r3 i0-i1 i2-i3 = r1' r3' i1' i3'
                __ASM_EMIT("vsub.f32    q1, q2, q3")                        // q1 = r4-r5 r6-r7 i4-i5 i6-i7 = r5' r7' i5' i7'

                // q4 = r0' r2' i0' i2'
                // q5 = r4' r6' i4' i6'
                // q0 = r1' r3' i1' i3'
                // q1 = r5' r7' i5' i7'
                __ASM_EMIT("vtrn.32     q4, q5")                            // q4 = r0' r4' i0' i4', q5 = r2' r6' i2' i6'
                __ASM_EMIT("vtrn.32     q0, q1")                            // q0 = r1' r5' i1' i5', q1 = r3' r7' i3' i7'
                __ASM_EMIT("vext.32     q6, q1, q1, $2")                    // q6 = i3' i7' r3' r7'

                __ASM_EMIT("vsub.f32    q2, q0, q6")                        // q2 = r1'-i3' r5'-i7' i1'-r3' i5'-r7' = r1" r5" i3" i7"
                __ASM_EMIT("vsub.f32    q1, q4, q5")                        // q1 = r0'-r2' r4'-r6' i0'-i2' i4'-i6' = r2" r6" i2" i6"
                __ASM_EMIT("vadd.f32    q3, q0, q6")                        // q3 = r1'+i3' r5'+i7' i1'+r3' i5'+r7' = r3" r7" i1" i5"
                __ASM_EMIT("vadd.f32    q0, q4, q5")                        // q0 = r0'+r2' r4'+r6' i0'+i2' i4'+i6' = r0" r4" i0" i4"

                // q0 = r0" r4" i0" i4"
                // q1 = r2" r6" i2" i6"
                // q2 = r1" r5" i3" i7"
                // q3 = r3" r7" i1" i5"
                __ASM_EMIT("vtrn.32     q0, q2")                            // q0 = r0" r1" i0" i3", q2 = r4" r5" i4" i7"
                __ASM_EMIT("vtrn.32     q1, q3")                            // q1 = r2" r3" i2" i1", q3 = r6" r7" i6" i5"
                __ASM_EMIT("vswp        d1, d2")                            // q0 = r0" r1" r2" r3", q1 = i0" i3" i2" i1"
                __ASM_EMIT("vswp        d5, d6")                            // q2 = r4" r5" r6" r7", q3 = i4" i7" i6" i5"

                __ASM_EMIT("vrev64.32   q1, q1")                            // q1 = i3" i0" i1" i2"
                __ASM_EMIT("vrev64.32   q3, q3")                            // q3 = i7" i4" i5" i6"
                __ASM_EMIT("vext.32     q1, q1, q1, $1")                    // q1 = i0" i1" i2" i3"
                __ASM_EMIT("vext.32     q3, q3, q3, $1")                    // q3 = i4" i5" i6" i7"

                __ASM_EMIT("cmp         %[i], %[regs]")
                __ASM_EMIT("vstm        %[dst]!, {q0-q3}")
                __ASM_EMIT("blo         1b")

                : [dst] "+r" (dst), [sr1] "=&r" (sr1), [sr2] "=&r" (sr2),
                  [rrank] "+r" (rrank), [i] "=&r" (i), [j] "=&r" (j)
                  IF_ARCH_ARM6(, [tmp] "=&r" (tmp), [msk] "=&r" (msk))
                : [src] "r" (src), [regs] "r" (regs)
                  IF_ARCH_ARM6(, [masks] "r" (__rb_masks))
                : "cc", "memory",
                  "q0", "q1", "q2", "q3", "q4", "q5", "q6"
            );
        }
    }

    void packed_unscramble_direct(float *dst, size_t rank)
    {
        IF_ARCH_ARM (
            size_t count = 1 << rank;
            float *src = dst;
        );

        ARCH_ARM_ASM(
            __ASM_EMIT("subs        %[count], $32")
            __ASM_EMIT("blo         2f")

            // 32x blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("vldm        %[src]!, {q0-q7}")
            __ASM_EMIT("vzip.32     q0, q1")
            __ASM_EMIT("vzip.32     q2, q3")
            __ASM_EMIT("vldm        %[src]!, {q8-q15}")
            __ASM_EMIT("vzip.32     q4, q5")
            __ASM_EMIT("vzip.32     q6, q7")
            __ASM_EMIT("vzip.32     q8, q9")
            __ASM_EMIT("vzip.32     q10, q11")
            __ASM_EMIT("vstm        %[dst]!, {q0-q7}")
            __ASM_EMIT("vzip.32     q12, q13")
            __ASM_EMIT("vzip.32     q14, q15")
            __ASM_EMIT("subs        %[count], $32")
            __ASM_EMIT("vstm        %[dst]!, {q8-q15}")
            __ASM_EMIT("bhs         1b")

            // 16x block
            __ASM_EMIT("2:")
            __ASM_EMIT("adds        %[count], $16")
            __ASM_EMIT("blt         4f")
            __ASM_EMIT("vldm        %[src]!, {q0-q7}")
            __ASM_EMIT("vzip.32     q0, q1")
            __ASM_EMIT("vzip.32     q2, q3")
            __ASM_EMIT("vzip.32     q4, q5")
            __ASM_EMIT("vzip.32     q6, q7")
            __ASM_EMIT("sub         %[count], $16")
            __ASM_EMIT("vstm        %[dst]!, {q0-q7}")

            // 8x block
            __ASM_EMIT("4:")
            __ASM_EMIT("adds        %[count], $8")
            __ASM_EMIT("blt         6f")
            __ASM_EMIT("vldm        %[src]!, {q0-q4}")
            __ASM_EMIT("vzip.32     q0, q1")
            __ASM_EMIT("vzip.32     q2, q3")
            __ASM_EMIT("vstm        %[dst]!, {q0-q4}")

            __ASM_EMIT("6:")
            : [src] "+r" (src), [dst] "+r" (dst),
              [count] "+r" (count)
            :
            : "cc", "memory",
              "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
              "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
    }

    void packed_unscramble_reverse(float *dst, size_t rank)
    {
        IF_ARCH_ARM (
            size_t count = 1 << rank;
            float *src = dst;
            float k = 1.0f / count;
        );

        ARCH_ARM_ASM(
            __ASM_EMIT("vld1.32     {d16[], d17[]} , [%[k]]")
            __ASM_EMIT("subs        %[count], $16")
            __ASM_EMIT("vmov        q9, q8")
            __ASM_EMIT("blo         2f")

            // 16x blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("vldm        %[src]!, {q0-q7}")
            __ASM_EMIT("vzip.32     q0, q1")
            __ASM_EMIT("vzip.32     q2, q3")
            __ASM_EMIT("vzip.32     q4, q5")
            __ASM_EMIT("vzip.32     q6, q7")
            __ASM_EMIT("vmul.f32    q0, q8")
            __ASM_EMIT("vmul.f32    q1, q9")
            __ASM_EMIT("vmul.f32    q2, q8")
            __ASM_EMIT("vmul.f32    q3, q9")
            __ASM_EMIT("vmul.f32    q4, q8")
            __ASM_EMIT("vmul.f32    q5, q9")
            __ASM_EMIT("vmul.f32    q6, q8")
            __ASM_EMIT("vmul.f32    q7, q9")
            __ASM_EMIT("subs        %[count], $16")
            __ASM_EMIT("vstm        %[dst]!, {q0-q7}")
            __ASM_EMIT("bge         1b")

            // 8x block
            __ASM_EMIT("2:")
            __ASM_EMIT("adds        %[count], $8")
            __ASM_EMIT("blt         4f")
            __ASM_EMIT("vldm        %[src]!, {q0-q4}")
            __ASM_EMIT("vzip.32     q0, q1")
            __ASM_EMIT("vzip.32     q2, q3")
            __ASM_EMIT("vmul.f32    q0, q8")
            __ASM_EMIT("vmul.f32    q1, q9")
            __ASM_EMIT("vmul.f32    q2, q8")
            __ASM_EMIT("vmul.f32    q3, q9")
            __ASM_EMIT("vstm        %[dst]!, {q0-q4}")

            __ASM_EMIT("4:")
            : [src] "+r" (src), [dst] "+r" (dst),
              [count] "+r" (count)
            : [k] "r" (&k)
            : "cc", "memory",
              "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
              "q8", "q9"
        );
    }
}

#endif /* DSP_ARCH_ARM_NEON_D32_FFT_PSCRAMBLE_H_ */
