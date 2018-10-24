/*
 * scramble.h
 *
 *  Created on: 24 окт. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_ARM_NEON_D32_FFT_SCRAMBLE_H_
#define DSP_ARCH_ARM_NEON_D32_FFT_SCRAMBLE_H_

#ifndef DSP_ARCH_ARM_NEON_32_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_ARM_NEON_32_IMPL */

namespace neon_d32
{
    void fft_scramble(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank)
    {
        IF_ARCH_ARM( size_t count = 1 << rank );

        if ((dst_re == src_re) || (dst_im == src_im))
        {
            IF_ARCH_ARM(
                    size_t i, j, rrank;
                    float *td_re, *td_im, *ts_re, *ts_im;
            );

            // Self algorithm
            ARCH_ARM_ASM(
                __ASM_EMIT("rsb         %[rrank], %[rank], $32")            // rrank = 32 - rank
                __ASM_EMIT("mov         %[ts_re], %[src_re]")               // ts_re = src_re
                __ASM_EMIT("mov         %[i], %1")                          // i = 1
                __ASM_EMIT("mov         %[ts_im], %[src_im]")               // ts_im = src_im
                __ASM_EMIT("1:")
                __ASM_EMIT("rbit        %[j], %[i]")                        // j = reverse_bits(i)
                __ASM_EMIT("lsr         %[j], %[rrank]")                    // j = reverse_bits(i) >> rank
                __ASM_EMIT("cmp         %[i], %[j]")                        // i <=> j
                __ASM_EMIT("bhs         2f")                                // if (i >= j) continue
                __ASM_EMIT("vadd        %[td_re], %[dst_re], %[j], LSL $2") // td_re = &dst_re[j]
                __ASM_EMIT("vadd        %[td_im], %[dst_im], %[j], LSL $2") // td_im = &dst_im[j]
                __ASM_EMIT("vldm        %[ts_re], {s0}")                    // s0 = *ts_re
                __ASM_EMIT("vldm        %[ts_im], {s1}")                    // s1 = *ts_im
                __ASM_EMIT("vldm        %[td_re], {s2}")                    // s2 = *td_re
                __ASM_EMIT("vldm        %[td_im], {s3}")                    // s3 = *td_im
                __ASM_EMIT("vstm        %[ts_re]!, {s2}")                   // *(ts_re++) = s2
                __ASM_EMIT("vstm        %[ts_im]!, {s3}")                   // *(ts_im++) = s3
                __ASM_EMIT("vstm        %[td_re], {s0}")                    // *td_re = s0
                __ASM_EMIT("vstm        %[td_im], {s1}")                    // td_im = s1
                __ASM_EMIT("2:")
                __ASM_EMIT("adds        %[i], $1")                          // i++
                __ASM_EMIT("cmp         %[i], %[count]")                    // i <=> count
                __ASM_EMIT("blo         1b")

                : [ts_re] "=&r" (ts_re), [ts_im] "=&r" (ts_im),
                  [td_re] "=&r" (td_re), [td_im] "=&r" (td_im),
                  [i] "=&r" (i), [j] "=&r" (j), [rrank] "=&r" (rrank)
                : [count] "r" (count), [rank] "r" (rank)
                : "cc", "memory",
                  "q0"
            );
        }
        else
        {

        }
    }
}

#endif /* DSP_ARCH_ARM_NEON_D32_FFT_SCRAMBLE_H_ */
