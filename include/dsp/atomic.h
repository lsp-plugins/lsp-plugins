/*
 * atomic.h
 *
 *  Created on: 02 февр. 2016 г.
 *      Author: sadko
 */

#ifndef DSP_ATOMIC_H_
#define DSP_ATOMIC_H_

#include <dsp/types.h>

typedef unsigned int    uatomic_t;
typedef signed int      atomic_t;

#ifdef ARCH_X86
    #include <dsp/arch/x86/atomic.h>
#endif /* ARCH_X86 */

#ifdef ARCH_ARM
    #include <dsp/arch/arm/atomic.h>
#endif /* ARCH_ARM */

//-----------------------------------------------------------------------------
// Require macro definitions for target architecture
#ifndef atomic_init
    #error "atomic_init macro should be defined"
#endif /* atomic_init */

#ifndef atomic_trylock
    #error "atomic_trylock macro should be defined"
#endif /* atomic_lock */

#ifndef atomic_unlock
    #error "atomic_unlock macro should be defined"
#endif /* atomic_unlock */


#endif /* DSP_ATOMIC_H_ */
