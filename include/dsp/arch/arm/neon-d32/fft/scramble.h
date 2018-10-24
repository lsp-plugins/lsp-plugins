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
        ARCH_ARM_ASM(
            __ASM_EMIT("tst         %[dst_re], %[src_re]")  // dst_re == src_re ?
            __ASM_EMIT("bne         2f")                    // go to copy algorithm
            __ASM_EMIT("tst         %[dst_im], %[src_im]")  // dst_re == src_re ?
            __ASM_EMIT("bne         2f")                    // go to copy algorithm

            // Self algorithm
            __ASM_EMIT()

            // Copy algorithm
            __ASM_EMIT("2:")
        );
    }
}

#endif /* DSP_ARCH_ARM_NEON_D32_FFT_SCRAMBLE_H_ */
