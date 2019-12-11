/*
 * pfft.h
 *
 *  Created on: 11 дек. 2019 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_AVX_PFFT_H_
#define DSP_ARCH_X86_AVX_PFFT_H_

#ifndef DSP_ARCH_X86_AVX_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_AVX_IMPL */

#include <dsp/arch/x86/avx/fft/const.h>

// Scrambling functions
#define FFT_PSCRAMBLE_SELF_DIRECT_NAME      packed_scramble_self_direct8
#define FFT_PSCRAMBLE_SELF_REVERSE_NAME     packed_scramble_self_reverse8
#define FFT_PSCRAMBLE_COPY_DIRECT_NAME      packed_scramble_copy_direct8
#define FFT_PSCRAMBLE_COPY_REVERSE_NAME     packed_scramble_copy_reverse8
#define FFT_TYPE                            uint8_t
#define FFT_FMA(a, b)                       a
#include <dsp/arch/x86/avx/fft/p_scramble.h>

#define FFT_PSCRAMBLE_SELF_DIRECT_NAME      packed_scramble_self_direct16
#define FFT_PSCRAMBLE_SELF_REVERSE_NAME     packed_scramble_self_reverse16
#define FFT_PSCRAMBLE_COPY_DIRECT_NAME      packed_scramble_copy_direct16
#define FFT_PSCRAMBLE_COPY_REVERSE_NAME     packed_scramble_copy_reverse16
#define FFT_TYPE                            uint16_t
#define FFT_FMA(a, b)                       a
#include <dsp/arch/x86/avx/fft/p_scramble.h>

#define FFT_PSCRAMBLE_SELF_DIRECT_NAME      packed_scramble_self_direct8_fma3
#define FFT_PSCRAMBLE_SELF_REVERSE_NAME     packed_scramble_self_reverse8_fma3
#define FFT_PSCRAMBLE_COPY_DIRECT_NAME      packed_scramble_copy_direct8_fma3
#define FFT_PSCRAMBLE_COPY_REVERSE_NAME     packed_scramble_copy_reverse8_fma3
#define FFT_TYPE                            uint8_t
#define FFT_FMA(a, b)                       b
#include <dsp/arch/x86/avx/fft/p_scramble.h>

#define FFT_PSCRAMBLE_SELF_DIRECT_NAME      packed_scramble_self_direct16_fma3
#define FFT_PSCRAMBLE_SELF_REVERSE_NAME     packed_scramble_self_reverse16_fma3
#define FFT_PSCRAMBLE_COPY_DIRECT_NAME      packed_scramble_copy_direct16_fma3
#define FFT_PSCRAMBLE_COPY_REVERSE_NAME     packed_scramble_copy_reverse16_fma3
#define FFT_TYPE                            uint16_t
#define FFT_FMA(a, b)                   b
#include <dsp/arch/x86/avx/fft/p_scramble.h>

namespace avx
{

}

#endif /* DSP_ARCH_X86_AVX_PFFT_H_ */
