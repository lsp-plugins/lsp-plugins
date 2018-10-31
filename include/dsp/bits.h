/*
 * bits.h
 *
 *  Created on: 06 февр. 2016 г.
 *      Author: sadko
 */

#ifndef DSP_BITS_H_
#define DSP_BITS_H_

#include <dsp/types.h>

extern const uint8_t    __rb[];

#if defined(ARCH_X86)
    #include <dsp/arch/x86/bits.h>
#else
    #include <dsp/arch/native/bits.h>
#endif

#endif /* DSP_BITS_H_ */
