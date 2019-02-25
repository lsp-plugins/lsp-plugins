/*
 * atomic.h
 *
 *  Created on: 14 авг. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_ARM_ATOMIC_H_
#define DSP_ARCH_ARM_ATOMIC_H_

#define ATOMIC_CAS_DEF(type, qsz)                      \
    inline type arm_compare_and_swap(type *ptr, type exp, type rep) \
    { \
        type tmp; \
        \
        ARCH_ARM_ASM \
        ( \
            __ASM_EMIT("dmb") \
            __ASM_EMIT("ldr" qsz "      %[tmp], [%[ptr]]") \
            __ASM_EMIT("teq             %[tmp], %[exp]") \
            __ASM_EMIT("str" qsz "eq    %[tmp], %[rep], [%[ptr]]") \
            __ASM_EMIT("movne           %[tmp], $1") \
            __ASM_EMIT("eor             %[tmp], $1") \
            : [tmp] "=&r" (tmp) \
            : [ptr] "r" (ptr), [exp] "r" (exp), [rep] "r" (rep) \
            : "cc", "memory" \
        ); \
        return tmp; \
    }

ATOMIC_CAS_DEF(int8_t, "exb")
ATOMIC_CAS_DEF(uint8_t, "exb")
ATOMIC_CAS_DEF(int16_t, "exh")
ATOMIC_CAS_DEF(uint16_t, "exh")
ATOMIC_CAS_DEF(int32_t, "ex")
ATOMIC_CAS_DEF(uint32_t, "ex")

#undef ATOMIC_CAS_DEF

#define ATOMIC_CAS_DEF(type, qsz, extra)                        \
    inline type atomic_cas(extra type *ptr, type exp, type rep) \
    { \
        type tmp; \
        \
        ARCH_ARM_ASM \
        ( \
            __ASM_EMIT("dmb") \
            __ASM_EMIT("ldr" qsz "      %[tmp], [%[ptr]]") \
            __ASM_EMIT("teq             %[tmp], %[exp]") \
            __ASM_EMIT("str" qsz "eq    %[tmp], %[rep], [%[ptr]]") \
            __ASM_EMIT("moveq           %[tmp], $1") \
            __ASM_EMIT("movne           %[tmp], $0") \
            : [tmp] "=&r" (tmp) \
            : [ptr] "r" (ptr), [exp] "r" (exp), [rep] "r" (rep) \
            : "cc", "memory" \
        ); \
        return bool(tmp); \
    }

ATOMIC_CAS_DEF(int8_t, "exb", )
ATOMIC_CAS_DEF(int8_t, "exb", volatile)
ATOMIC_CAS_DEF(uint8_t, "exb", )
ATOMIC_CAS_DEF(uint8_t, "exb", volatile)
ATOMIC_CAS_DEF(int16_t, "exh", )
ATOMIC_CAS_DEF(int16_t, "exh", volatile)
ATOMIC_CAS_DEF(uint16_t, "exh", )
ATOMIC_CAS_DEF(uint16_t, "exh", volatile)
ATOMIC_CAS_DEF(int32_t, "ex", )
ATOMIC_CAS_DEF(int32_t, "ex", volatile)
ATOMIC_CAS_DEF(uint32_t, "ex", )
ATOMIC_CAS_DEF(uint32_t, "ex", volatile)

#undef ATOMIC_CAS_DEF

//-----------------------------------------------------------------------------
// Atomic operations
#define atomic_init(lk)         lk = 1
#define atomic_trylock(lk)      arm_compare_and_swap(&lk, 1, 0)
#define atomic_unlock(lk)       arm_compare_and_swap(&lk, 0, 1)


#endif /* DSP_ARCH_ARM_ATOMIC_H_ */
