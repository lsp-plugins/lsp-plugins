/*
 * atomic.h
 *
 *  Created on: 19 мар. 2019 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_NATIVE_ATOMIC_H_
#define DSP_ARCH_NATIVE_ATOMIC_H_

#define ATOMIC_XCHG_DEF(type, extra) \
    inline type atomic_swap(extra type *ptr, type value) { \
        volatile type *xptr = ptr; \
        while (true) { \
            type x = *xptr; \
            if (__sync_bool_compare_and_swap (ptr, x, value)) \
                return x; \
        } \
    }

ATOMIC_XCHG_DEF(int8_t, )
ATOMIC_XCHG_DEF(int8_t, volatile)
ATOMIC_XCHG_DEF(uint8_t, )
ATOMIC_XCHG_DEF(uint8_t, volatile)
ATOMIC_XCHG_DEF(int16_t, )
ATOMIC_XCHG_DEF(int16_t, volatile)
ATOMIC_XCHG_DEF(uint16_t, )
ATOMIC_XCHG_DEF(uint16_t, volatile)
ATOMIC_XCHG_DEF(int32_t, )
ATOMIC_XCHG_DEF(int32_t, volatile)
ATOMIC_XCHG_DEF(uint32_t, )
ATOMIC_XCHG_DEF(uint32_t, volatile)

#ifdef ARCH_64BIT
    ATOMIC_XCHG_DEF(int64_t, )
    ATOMIC_XCHG_DEF(int64_t, volatile)
    ATOMIC_XCHG_DEF(uint64_t, )
    ATOMIC_XCHG_DEF(uint64_t, volatile)
#endif /* ARCH_64BIT */

#undef ATOMIC_XCHG_DEF

#define ATOMIC_CAS_DEF(type, extra)                         \
    inline bool atomic_cas(extra type *ptr, type src, type rep) {  \
        return __sync_bool_compare_and_swap (ptr, src, rep); \
    }

ATOMIC_CAS_DEF(int8_t, )
ATOMIC_CAS_DEF(int8_t, volatile)
ATOMIC_CAS_DEF(uint8_t, )
ATOMIC_CAS_DEF(uint8_t, volatile)
ATOMIC_CAS_DEF(int16_t, )
ATOMIC_CAS_DEF(int16_t, volatile)
ATOMIC_CAS_DEF(uint16_t, )
ATOMIC_CAS_DEF(uint16_t, volatile)
ATOMIC_CAS_DEF(int32_t, )
ATOMIC_CAS_DEF(int32_t, volatile)
ATOMIC_CAS_DEF(uint32_t, )
ATOMIC_CAS_DEF(uint32_t, volatile)

#ifdef ARCH_64BIT
    ATOMIC_CAS_DEF(int64_t, )
    ATOMIC_CAS_DEF(int64_t, volatile)
    ATOMIC_CAS_DEF(uint64_t, )
    ATOMIC_CAS_DEF(uint64_t, volatile)
#endif /* ARCH_64BIT */

#undef ATOMIC_CAS_DEF

#define ATOMIC_ADD_DEF(type, extra) \
    inline type atomic_add(extra type *ptr, type value) { \
        return __sync_fetch_and_add(ptr, value); \
    }

ATOMIC_ADD_DEF(int8_t, )
ATOMIC_ADD_DEF(int8_t, volatile)
ATOMIC_ADD_DEF(uint8_t, )
ATOMIC_ADD_DEF(uint8_t, volatile)
ATOMIC_ADD_DEF(int16_t, )
ATOMIC_ADD_DEF(int16_t, volatile)
ATOMIC_ADD_DEF(uint16_t, )
ATOMIC_ADD_DEF(uint16_t, volatile)
ATOMIC_ADD_DEF(int32_t, )
ATOMIC_ADD_DEF(int32_t, volatile)
ATOMIC_ADD_DEF(uint32_t, )
ATOMIC_ADD_DEF(uint32_t, volatile)

#ifdef ARCH_64BIT
    ATOMIC_ADD_DEF(int64_t, )
    ATOMIC_ADD_DEF(int64_t, volatile)
    ATOMIC_ADD_DEF(uint64_t, )
    ATOMIC_ADD_DEF(uint64_t, volatile)
#endif /* ARCH_64BIT */

#undef ATOMIC_ADD_DEF

//-----------------------------------------------------------------------------
// Atomic operations
#define atomic_init(lk)         lk = 1
#define atomic_trylock(lk)      atomic_cas(&lk, 1, 0)
#define atomic_unlock(lk)       atomic_swap(&lk, 1)

#endif /* DSP_ARCH_NATIVE_ATOMIC_H_ */
