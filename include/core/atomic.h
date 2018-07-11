/*
 * atomic.h
 *
 *  Created on: 02 февр. 2016 г.
 *      Author: sadko
 */

#ifndef CORE_ATOMIC_H_
#define CORE_ATOMIC_H_

#include <core/types.h>

namespace lsp
{
    typedef unsigned int    uatomic_t;
    typedef signed int      atomic_t;
}

#ifdef ARCH_X86
    #include <core/x86/atomic.h>
#endif /* ARCH_X86 */

// Atomic macros
#define atomic_lock(lk)     atomic_exchange(&lk, 0)
#define atomic_init(lk)     lk = 1
#define atomic_unlock(lk)   atomic_exchange(&lk, 1)

#endif /* CORE_ATOMIC_H_ */
