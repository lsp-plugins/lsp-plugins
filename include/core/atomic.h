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

#if defined(__i386__) || defined(__x86_64__)
    #include <core/x86/atomic.h>
#endif /* defined(__i386__) || defined(__x86_64__) */

#endif /* CORE_ATOMIC_H_ */
