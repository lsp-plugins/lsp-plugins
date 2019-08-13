/*
 * bits.h
 *
 *  Created on: 06 февр. 2016 г.
 *      Author: sadko
 */

#ifndef DSP_BITS_H_
#define DSP_BITS_H_

#include <common/types.h>

// Declare array only for specific architectures
#if (!defined(ARCH_ARM)) && (!defined(ARCH_AARCH64))
    extern const uint8_t    __rb[];
#endif

// Include CPU-optimized code for bit reversal
#if defined(ARCH_X86)
    #include <dsp/arch/x86/bits.h>
#elif defined(ARCH_AARCH64)
    #include <dsp/arch/aarch64/bits.h>
#elif defined(ARCH_ARM)
    #include <dsp/arch/arm/bits.h>
#else
    #include <dsp/arch/native/bits.h>
#endif

#endif /* DSP_BITS_H_ */
