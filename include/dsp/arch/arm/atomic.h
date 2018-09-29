/*
 * atomic.h
 *
 *  Created on: 14 авг. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_ARM_ATOMIC_H_
#define DSP_ARCH_ARM_ATOMIC_H_

#define ATOMIC_CAS_DEF(type)                            \
    inline type arm_compare_and_swap(type *ptr, type exp, type rep) \
    { \
        type tmp; \
        \
        ARCH_ARM_ASM \
        ( \
            __ASM_EMIT("dmb") \
            __ASM_EMIT("ldrex       %[tmp], [%[ptr]]") \
            __ASM_EMIT("teq         %[tmp], %[exp]") \
            __ASM_EMIT("strexeq     %[tmp], %[rep], [%[ptr]]") \
            __ASM_EMIT("movne       %[tmp], #1") \
            __ASM_EMIT("eor         %[tmp], #1") \
            : [tmp] "=&r" (tmp) \
            : [ptr] "r" (ptr), [exp] "r" (exp), [rep] "r" (rep) \
            : "cc", "memory" \
        ); \
        return tmp; \
    }

ATOMIC_CAS_DEF(int8_t)
ATOMIC_CAS_DEF(uint8_t)
ATOMIC_CAS_DEF(int16_t)
ATOMIC_CAS_DEF(uint16_t)
ATOMIC_CAS_DEF(int32_t)
ATOMIC_CAS_DEF(uint32_t)

#ifdef ARCH_ARM8
    ATOMIC_CAS_DEF(int64_t)
    ATOMIC_CAS_DEF(uint64_t)
#endif /* ARCH_ARM8 */

//-----------------------------------------------------------------------------
// Atomic operations
#define atomic_init(lk)         lk = 1
#define atomic_trylock(lk)      arm_compare_and_swap(&lk, 1, 0)
#define atomic_unlock(lk)       arm_compare_and_swap(&lk, 0, 1)


#endif /* DSP_ARCH_ARM_ATOMIC_H_ */
