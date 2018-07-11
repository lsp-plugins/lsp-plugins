/*
 * x86.h
 *
 *  Created on: 02 февр. 2016 г.
 *      Author: sadko
 */

#ifndef CORE_ATOMIC_X86_H_
#define CORE_ATOMIC_X86_H_

#define ATOMIC_XCHG_DEF(type)                           \
    inline type atomic_exchange(type *ptr, type value)  \
    {                                                   \
        __asm__ __volatile__                            \
        (                                               \
            __ASM_EMIT("lock")                          \
            __ASM_EMIT("xchg %0, (%1)")                 \
            : "+r"(value)                               \
            : "r" (ptr)                                 \
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

#ifdef __x86_64__
    ATOMIC_XCHG_DEF(int64_t)
    ATOMIC_XCHG_DEF(uint64_t)
#endif /* __x86_64__ */

#undef ATOMIC_XCHG_DEF

#endif /* CORE_ATOMIC_X86_H_ */
