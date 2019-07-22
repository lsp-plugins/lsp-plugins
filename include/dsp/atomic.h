/*
 * atomic.h
 *
 *  Created on: 02 февр. 2016 г.
 *      Author: sadko
 */

#ifndef DSP_ATOMIC_H_
#define DSP_ATOMIC_H_

#include <common/types.h>

typedef unsigned int    uatomic_t;
typedef signed int      atomic_t;

/**
  Common atomic interface:

    Atomically swap the value 'value' with memory chunk pointed by
    'ptr' and return the previous value stored in memory chunk as 'ret':

        ret atomic_swap(*ptr, value)

    Atomicatlly add value 'value' to memory chunk pointed by 'ptr'
    and return the previous value stored in memory chunk as 'ret':

        ret atomic_add(*ptr, value)

    Atomically compare contents of memory chunk pointed by 'ptr'
    with expected value 'exp' and overwrite it with replacement 'rep'
    if they match. Return true on success, fail on error as 'ret'.

        ret atomic_cas(*ptr, exp, rep)
*/

#if defined(ARCH_X86) /* x86 and x86_64 implementation */
    #include <dsp/arch/x86/atomic.h>
#elif defined(ARCH_AARCH64) /* ARM 64-bit implementation */
    #include <dsp/arch/aarch64/atomic.h>
#elif defined(ARCH_ARM) /* ARM 32-bit implementation */
    #include <dsp/arch/arm/atomic.h>
#else /* Native implementation */
    #include <dsp/arch/native/atomic.h>
#endif /* ARCH_X86 */

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
