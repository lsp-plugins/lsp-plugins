/*
 * atomic.h
 *
 *  Created on: 14 авг. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_ARM_ATOMIC_H_
#define DSP_ARCH_ARM_ATOMIC_H_

#define ATOMIC_CAS_DEF(type, qsz, extra)                        \
    inline type atomic_cas(extra type *ptr, type exp, type rep) \
    { \
        type tmp; \
        \
        ARCH_ARM_ASM \
        ( \
            IF_ARCH_LEAST_ARM7(__ASM_EMIT("dmb")) \
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

#define ATOMIC_ADD_DEF(type, qsz, extra) \
    inline type atomic_add(extra type *ptr, type value) \
    {                                                   \
        type tmp, sum, retval; \
        \
        ARCH_ARM_ASM                                    \
        (                                               \
            __ASM_EMIT("1:")    \
            IF_ARCH_LEAST_ARM7(__ASM_EMIT("dmb")) \
            __ASM_EMIT("ldr" qsz "      %[ret], [%[ptr]]") \
            __ASM_EMIT("add             %[sum], %[ret], %[src]") \
            __ASM_EMIT("str" qsz "      %[tmp], %[sum], [%[ptr]]") \
            __ASM_EMIT("tst             %[tmp], %[tmp]") \
            __ASM_EMIT("bne             1b") \
            : [tmp] "=&r" (tmp), \
              [sum] "=&r" (sum), \
              [ret] "=&r" (retval)  \
            : [ptr] "r" (ptr),  \
              [src] "r" (value) \
            : "memory", "cc"                            \
        );                                              \
        return retval; \
    }

ATOMIC_ADD_DEF(int8_t, "exb", )
ATOMIC_ADD_DEF(int8_t, "exb", volatile)
ATOMIC_ADD_DEF(uint8_t, "exb", )
ATOMIC_ADD_DEF(uint8_t, "exb", volatile)
ATOMIC_ADD_DEF(int16_t, "exh", )
ATOMIC_ADD_DEF(int16_t, "exh", volatile)
ATOMIC_ADD_DEF(uint16_t, "exh", )
ATOMIC_ADD_DEF(uint16_t, "exh", volatile)
ATOMIC_ADD_DEF(int32_t, "ex", )
ATOMIC_ADD_DEF(int32_t, "ex", volatile)
ATOMIC_ADD_DEF(uint32_t, "ex", )
ATOMIC_ADD_DEF(uint32_t, "ex", volatile)

#undef ATOMIC_ADD_DEF

#define ATOMIC_SWAP_DEF(type, qsz, extra) \
    inline type atomic_swap(extra type *ptr, type value) \
    {                                                   \
        type tmp, retval; \
        \
        ARCH_ARM_ASM                                    \
        (                                               \
            __ASM_EMIT("1:")    \
            IF_ARCH_LEAST_ARM7(__ASM_EMIT("dmb")) \
            __ASM_EMIT("ldr" qsz "      %[ret], [%[ptr]]") \
            __ASM_EMIT("str" qsz "      %[tmp], %[src], [%[ptr]]") \
            __ASM_EMIT("tst             %[tmp], %[tmp]") \
            __ASM_EMIT("bne             1b") \
            : [tmp] "=&r" (tmp), \
              [ret] "=&r" (retval)  \
            : [ptr] "r" (ptr), \
              [src] "r" (value) \
            : "memory", "cc"                            \
        );                                              \
        return retval; \
    }

ATOMIC_SWAP_DEF(int8_t, "exb", )
ATOMIC_SWAP_DEF(int8_t, "exb", volatile)
ATOMIC_SWAP_DEF(uint8_t, "exb", )
ATOMIC_SWAP_DEF(uint8_t, "exb", volatile)
ATOMIC_SWAP_DEF(int16_t, "exh", )
ATOMIC_SWAP_DEF(int16_t, "exh", volatile)
ATOMIC_SWAP_DEF(uint16_t, "exh", )
ATOMIC_SWAP_DEF(uint16_t, "exh", volatile)
ATOMIC_SWAP_DEF(int32_t, "ex", )
ATOMIC_SWAP_DEF(int32_t, "ex", volatile)
ATOMIC_SWAP_DEF(uint32_t, "ex", )
ATOMIC_SWAP_DEF(uint32_t, "ex", volatile)

#undef ATOMIC_SWAP_DEF

//-----------------------------------------------------------------------------
// Atomic operations
#define atomic_init(lk)         lk = 1
#define atomic_trylock(lk)      atomic_cas(&lk, 1, 0)
#define atomic_unlock(lk)       atomic_swap(&lk, 1)


#endif /* DSP_ARCH_ARM_ATOMIC_H_ */
