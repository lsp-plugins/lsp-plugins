/*
 * atomic.h
 *
 *  Created on: 18 мар. 2019 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_AARCH64_ATOMIC_H_
#define DSP_ARCH_AARCH64_ATOMIC_H_

#if 0
#define ATOMIC_CAS_DEF(type, qsz, extra)                        \
    inline type atomic_cas(extra type *ptr, type exp, type rep) \
    { \
        type tmp; \
        \
        ARCH_AARCH64_ASM \
        ( \
            __ASM_EMIT("mov             %[tmp], %[exp]") \
            __ASM_EMIT("casa" qsz "     %[tmp], %[rep], [%[ptr]]") \
            __ASM_EMIT("cmp             %[tmp], %[exp]") \
            __ASM_EMIT("cset            %[tmp], eq") \
            : [tmp] "=&r" (tmp) \
            : [ptr] "r" (ptr), [exp] "r" (exp), [rep] "r" (rep) \
            : "cc", "memory" \
        ); \
        return tmp; \
    }
#endif

#define ATOMIC_CAS_DEF(type, qsz, extra)                        \
    inline type atomic_cas(extra type *ptr, type exp, type rep) \
    { \
        type tmp; \
        \
        ARCH_AARCH64_ASM \
        ( \
            __ASM_EMIT("mov             %[tmp], %[exp]") \
            __ASM_EMIT("ldaxr" qsz "    %[ret], [%[ptr]]") \
            __ASM_EMIT("eor             %[ret], %[exp]")    /* ret == 0 on success */ \
            __ASM_EMIT("cbnz            %[ret], 2f")        /* jump if failed */ \
            __ASM_EMIT("stxr" qsz "     %[ret], %[rep], [%[ptr]]") /* try to store rep as replacement */ \
            __ASM_EMIT("tst             %[ret], %[ret]")    /* ret == 0 on success */ \
            __ASM_EMIT("2:") \
            __ASM_EMIT("cset            %[tmp], eq") \
            : [tmp] "=&r" (tmp) \
            : [ptr] "r" (ptr), [exp] "r" (exp), [rep] "r" (rep) \
            : "cc", "memory" \
        ); \
        return tmp; \
    }

ATOMIC_CAS_DEF(int8_t, "b", )
ATOMIC_CAS_DEF(int8_t, "b", volatile)
ATOMIC_CAS_DEF(uint8_t, "b", )
ATOMIC_CAS_DEF(uint8_t, "b", volatile)
ATOMIC_CAS_DEF(int16_t, "h", )
ATOMIC_CAS_DEF(int16_t, "h", volatile)
ATOMIC_CAS_DEF(uint16_t, "h", )
ATOMIC_CAS_DEF(uint16_t, "h", volatile)
ATOMIC_CAS_DEF(int32_t, "", )
ATOMIC_CAS_DEF(int32_t, "", volatile)
ATOMIC_CAS_DEF(uint32_t, "", )
ATOMIC_CAS_DEF(uint32_t, "", volatile)
ATOMIC_CAS_DEF(int64_t, "", )
ATOMIC_CAS_DEF(int64_t, "", volatile)
ATOMIC_CAS_DEF(uint64_t, "", )
ATOMIC_CAS_DEF(uint64_t, "", volatile)

#undef ATOMIC_CAS_DEF

#define ATOMIC_ADD_DEF(type, qsz, extra) \
    inline type atomic_add(extra type *ptr, type value) \
    {                                                   \
        type tmp, sum, retval; \
        \
        ARCH_AARCH64_ASM                                \
        (                                               \
            __ASM_EMIT("1:")    \
            __ASM_EMIT("dmb") \
            __ASM_EMIT("ldaxr" qsz "    %[ret], [%[ptr]]") \
            __ASM_EMIT("add             %[sum], %[ret], %[src]") \
            __ASM_EMIT("stxr" qsz "     %[tmp], %[sum], [%[ptr]]") \
            __ASM_EMIT("cbnz            %[tmp], 1b") /* repeat if failed */ \
            : [tmp] "=&r" (tmp), \
              [sum] "=&r" (sum), \
              [ret] "=&r" (retval)  \
            : [ptr] "r" (ptr),  \
              [src] "r" (value) \
            : "memory", "cc"                            \
        );                                              \
        return retval; \
    }

ATOMIC_ADD_DEF(int8_t, "b", )
ATOMIC_ADD_DEF(int8_t, "b", volatile)
ATOMIC_ADD_DEF(uint8_t, "b", )
ATOMIC_ADD_DEF(uint8_t, "b", volatile)
ATOMIC_ADD_DEF(int16_t, "h", )
ATOMIC_ADD_DEF(int16_t, "h", volatile)
ATOMIC_ADD_DEF(uint16_t, "h", )
ATOMIC_ADD_DEF(uint16_t, "h", volatile)
ATOMIC_ADD_DEF(int32_t, "", )
ATOMIC_ADD_DEF(int32_t, "", volatile)
ATOMIC_ADD_DEF(uint32_t, "", )
ATOMIC_ADD_DEF(uint32_t, "", volatile)
ATOMIC_ADD_DEF(int64_t, "", )
ATOMIC_ADD_DEF(int64_t, "", volatile)
ATOMIC_ADD_DEF(uint64_t, "", )
ATOMIC_ADD_DEF(uint64_t, "", volatile)

#undef ATOMIC_ADD_DEF

#define ATOMIC_SWAP_DEF(type, qsz, extra) \
    inline type atomic_swap(extra type *ptr, type value) \
    {                                                   \
        type tmp, retval; \
        \
        ARCH_AARCH64_ASM                                \
        (                                               \
            __ASM_EMIT("1:")    \
            __ASM_EMIT("dmb") \
            __ASM_EMIT("ldaxr" qsz "    %[ret], [%[ptr]]") \
            __ASM_EMIT("stxr" qsz "     %[tmp], %[value], [%[ptr]]") \
            __ASM_EMIT("cbnz            %[tmp], 1b") /* repeat if failed */ \
            : [tmp] "=&r" (tmp), \
              [ret] "=&r" (retval)  \
            : [ptr] "r" (ptr),  \
              [value] "r" (value) \
            : "memory", "cc"                            \
        );                                              \
        return retval; \
    }

ATOMIC_SWAP_DEF(int8_t, "b", )
ATOMIC_SWAP_DEF(int8_t, "b", volatile)
ATOMIC_SWAP_DEF(uint8_t, "b", )
ATOMIC_SWAP_DEF(uint8_t, "b", volatile)
ATOMIC_SWAP_DEF(int16_t, "h", )
ATOMIC_SWAP_DEF(int16_t, "h", volatile)
ATOMIC_SWAP_DEF(uint16_t, "h", )
ATOMIC_SWAP_DEF(uint16_t, "h", volatile)
ATOMIC_SWAP_DEF(int32_t, "", )
ATOMIC_SWAP_DEF(int32_t, "", volatile)
ATOMIC_SWAP_DEF(uint32_t, "", )
ATOMIC_SWAP_DEF(uint32_t, "", volatile)
ATOMIC_SWAP_DEF(int64_t, "", )
ATOMIC_SWAP_DEF(int64_t, "", volatile)
ATOMIC_SWAP_DEF(uint64_t, "", )
ATOMIC_SWAP_DEF(uint64_t, "", volatile)

#undef ATOMIC_SWAP_DEF

//-----------------------------------------------------------------------------
// Atomic operations
#define atomic_init(lk)         lk = 1
#define atomic_trylock(lk)      atomic_cas(&lk, 1, 0)
#define atomic_unlock(lk)       atomic_cas(&lk, 0, 1)

#endif /* DSP_ARCH_AARCH64_ATOMIC_H_ */
