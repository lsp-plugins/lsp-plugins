/*
 * x86.h
 *
 *  Created on: 02 февр. 2016 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_ATOMIC_X86_H_
#define DSP_ARCH_ATOMIC_X86_H_

#define ATOMIC_XCHG_DEF(type)                           \
    inline type x86_atomic_exchange(type *ptr, type value)  \
    {                                                   \
        ARCH_X86_ASM                                    \
        (                                               \
            __ASM_EMIT("lock")                          \
            __ASM_EMIT("xchg    %[value], (%[ptr])")    \
            : [value] "+r"(value)                       \
            : [ptr] "r" (ptr)                           \
            : "memory", "cc"                            \
        );                                              \
        return value;                                   \
    }

ATOMIC_XCHG_DEF(int8_t)
ATOMIC_XCHG_DEF(uint8_t)
ATOMIC_XCHG_DEF(int16_t)
ATOMIC_XCHG_DEF(uint16_t)
ATOMIC_XCHG_DEF(int32_t)
ATOMIC_XCHG_DEF(uint32_t)

#ifdef ARCH_X86_64
    ATOMIC_XCHG_DEF(int64_t)
    ATOMIC_XCHG_DEF(uint64_t)
#endif /* ARCH_X86_64 */

#undef ATOMIC_XCHG_DEF

//-----------------------------------------------------------------------------
// Atomic operations
#define atomic_init(lk)         lk = 1
#define atomic_trylock(lk)      x86_atomic_exchange(&lk, 0)
#define atomic_unlock(lk)       x86_atomic_exchange(&lk, 1)

#endif /* DSP_ARCH_ATOMIC_X86_H_ */
