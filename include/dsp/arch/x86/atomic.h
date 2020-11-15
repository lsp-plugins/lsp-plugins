/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 02 февр. 2016 г.
 *
 * lsp-plugins is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-plugins is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-plugins. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef DSP_ARCH_ATOMIC_X86_H_
#define DSP_ARCH_ATOMIC_X86_H_

#define ATOMIC_XCHG_DEF(type, extra)                    \
    inline type atomic_swap(extra type *ptr, type value)\
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

#ifdef ARCH_X86_64
    ATOMIC_XCHG_DEF(int64_t, )
    ATOMIC_XCHG_DEF(int64_t, volatile)
    ATOMIC_XCHG_DEF(uint64_t, )
    ATOMIC_XCHG_DEF(uint64_t, volatile)
#endif /* ARCH_X86_64 */

#undef ATOMIC_XCHG_DEF

#define ATOMIC_CAS_DEF(type, extra)                         \
    inline bool atomic_cas(extra type *ptr, type src, type rep)   \
    {                                                   \
        bool res;                                       \
        ARCH_X86_ASM                                    \
        (                                               \
            __ASM_EMIT("cmp     %[src], (%[ptr])")      \
            __ASM_EMIT("jne     2f")                    \
            __ASM_EMIT("lock")                          \
            __ASM_EMIT("cmpxchg %[rep], (%[ptr])")      \
            __ASM_EMIT("2:")                            \
            __ASM_EMIT("setz    %[res]")                \
            : [res] "=a" (res)                          \
            : [src] "a" (src),                          \
              [ptr] "r" (ptr),                          \
              [rep] "r" (rep)                           \
            : "memory", "cc"                            \
        );                                              \
        return res; \
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

#ifdef ARCH_X86_64
    ATOMIC_CAS_DEF(int64_t, )
    ATOMIC_CAS_DEF(int64_t, volatile)
    ATOMIC_CAS_DEF(uint64_t, )
    ATOMIC_CAS_DEF(uint64_t, volatile)
#endif /* ARCH_X86_64 */

#undef ATOMIC_CAS_DEF

#define ATOMIC_ADD_DEF(type, extra) \
    inline type atomic_add(extra type *ptr, type value) \
    {                                                   \
        ARCH_X86_ASM                                    \
        (                                               \
            __ASM_EMIT("lock")                          \
            __ASM_EMIT("xadd %[src], (%[ptr])")         \
            : [src] "+r" (value)                        \
            : [ptr] "r" (ptr)                           \
            : "memory", "cc"                            \
        );                                              \
        return value; \
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

#ifdef ARCH_X86_64
    ATOMIC_ADD_DEF(int64_t, )
    ATOMIC_ADD_DEF(int64_t, volatile)
    ATOMIC_ADD_DEF(uint64_t, )
    ATOMIC_ADD_DEF(uint64_t, volatile)
#endif /* ARCH_X86_64 */

#undef ATOMIC_ADD_DEF

//-----------------------------------------------------------------------------
// Atomic operations
#define atomic_init(lk)         lk = 1
#define atomic_trylock(lk)      atomic_swap(&lk, 0)
#define atomic_unlock(lk)       atomic_swap(&lk, 1)


#endif /* DSP_ARCH_ATOMIC_X86_H_ */
