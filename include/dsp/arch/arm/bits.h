/*
 * bits.h
 *
 *  Created on: 26 окт. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_ARM_BITS_H_
#define DSP_ARCH_ARM_BITS_H_

#include <dsp/dsp.h>

inline uint32_t reverse_bits(uint32_t src)
{
    uint32_t res;

    ARCH_ARM_ASM (
        __ASM_EMIT("rbit    %[res], %[src]")
        : [res] "=r" (res)
        : [src] "r" (src)
        :
    );

    return res;
}

inline uint32_t reverse_bits(uint32_t src, size_t count)
{
    uint32_t res;

    ARCH_ARM_ASM (
        __ASM_EMIT("rsb     %[count], %[count], $32")
        __ASM_EMIT("rbit    %[res], %[src]")
        __ASM_EMIT("lsr     %[res], %[count]")
        : [res] "=r" (res), [count] "+r" (count)
        : [src] "r" (src)
        :
    );

    return res;
}

inline int32_t reverse_bits(int32_t src)
{
    int32_t res;

    ARCH_ARM_ASM (
        __ASM_EMIT("rbit    %[res], %[src]")
        : [res] "=r" (res)
        : [src] "r" (src)
        :
    );

    return res;
}

inline int32_t reverse_bits(int32_t src, size_t count)
{
    int32_t res;

    ARCH_ARM_ASM (
        __ASM_EMIT("rsb     %[count], %[count], $32")
        __ASM_EMIT("rbit    %[res], %[src]")
        __ASM_EMIT("lsr     %[res], %[count]")
        : [res] "=r" (res), [count] "+r" (count)
        : [src] "r" (src)
        :
    );

    return res;
}

inline uint16_t reverse_bits(uint16_t src)
{
    uint16_t res;

    ARCH_ARM_ASM (
        __ASM_EMIT("rbit    %[res], %[src]")
        __ASM_EMIT("lsr     %[res], $16")
        : [res] "=r" (res)
        : [src] "r" (src)
        :
    );

    return res;
}

inline uint16_t reverse_bits(uint16_t src, size_t count)
{
    uint16_t res;

    ARCH_ARM_ASM (
        __ASM_EMIT("rsb     %[count], %[count], $32")
        __ASM_EMIT("rbit    %[res], %[src]")
        __ASM_EMIT("lsr     %[res], %[count]")
        : [res] "=r" (res), [count] "+r" (count)
        : [src] "r" (src)
        :
    );

    return res;
}

inline int16_t reverse_bits(int16_t src)
{
    int16_t res;

    ARCH_ARM_ASM (
        __ASM_EMIT("rbit    %[res], %[src]")
        __ASM_EMIT("lsr     %[res], $16")
        : [res] "=r" (res)
        : [src] "r" (src)
        :
    );

    return res;
}

inline int16_t reverse_bits(int16_t src, size_t count)
{
    int16_t res;

    ARCH_ARM_ASM (
        __ASM_EMIT("rsb     %[count], %[count], $32")
        __ASM_EMIT("rbit    %[res], %[src]")
        __ASM_EMIT("lsr     %[res], %[count]")
        : [res] "=r" (res), [count] "+r" (count)
        : [src] "r" (src)
        :
    );

    return res;
}

inline uint8_t reverse_bits(uint8_t src)
{
    uint8_t res;

    ARCH_ARM_ASM (
        __ASM_EMIT("rbit    %[res], %[src]")
        __ASM_EMIT("lsr     %[res], $24")
        : [res] "=r" (res)
        : [src] "r" (src)
        :
    );

    return res;
}

inline uint8_t reverse_bits(uint8_t src, size_t count)
{
    uint8_t res;

    ARCH_ARM_ASM (
        __ASM_EMIT("rsb     %[count], %[count], $32")
        __ASM_EMIT("rbit    %[res], %[src]")
        __ASM_EMIT("lsr     %[res], %[count]")
        : [res] "=r" (res), [count] "+r" (count)
        : [src] "r" (src)
        :
    );

    return res;
}

inline int8_t reverse_bits(int8_t src)
{
    int8_t res;

    ARCH_ARM_ASM (
        __ASM_EMIT("rbit    %[res], %[src]")
        __ASM_EMIT("lsr     %[res], $24")
        : [res] "=r" (res)
        : [src] "r" (src)
        :
    );

    return res;
}

inline int8_t reverse_bits(int8_t src, size_t count)
{
    int8_t res;

    ARCH_ARM_ASM (
        __ASM_EMIT("rsb     %[count], %[count], $32")
        __ASM_EMIT("rbit    %[res], %[src]")
        __ASM_EMIT("lsr     %[res], %[count]")
        : [res] "=r" (res), [count] "+r" (count)
        : [src] "r" (src)
        :
    );

    return res;
}

inline uint64_t reverse_bits(uint64_t v)
{
    uint32_t lo = uint32_t(v);
    uint32_t hi = uint32_t(v >> 32);

    ARCH_ARM_ASM
    (
        __ASM_EMIT("rbit            %[lo], %[lo]")
        __ASM_EMIT("rbit            %[hi], %[hi]")
        : [lo] "+r" (lo), [hi] "+r" (hi)
        : :
    );

    return hi | (uint64_t(lo) << 32);
}

inline int64_t reverse_bits(int64_t v)
{
    uint32_t lo = uint32_t(v);
    uint32_t hi = uint32_t(v >> 32);

    ARCH_ARM_ASM
    (
        __ASM_EMIT("rbit            %[lo], %[lo]")
        __ASM_EMIT("rbit            %[hi], %[hi]")
        : [lo] "+r" (lo), [hi] "+r" (hi)
        : :
    );

    return hi | (int64_t(lo) << 32);
}

inline uint64_t reverse_bits(uint64_t v, size_t count)
{
    uint32_t lo = uint32_t(v);
    uint32_t hi = uint32_t(v >> 32);
    uint32_t tmp;

    ARCH_ARM_ASM
    (
        __ASM_EMIT("rsb             %[count], %[count], $64")
        __ASM_EMIT("rbit            %[lo], %[lo]")
        __ASM_EMIT("rbit            %[hi], %[hi]")                  // [ lo, hi ]
        __ASM_EMIT("cmp             %[count], $32")
        __ASM_EMIT("blo             2f")

        __ASM_EMIT("mov             %[hi], %[lo]")                  // [ lo, lo ]
        __ASM_EMIT("sub             %[count], $32")
        __ASM_EMIT("eor             %[lo], %[lo]")                  // [ 0, lo ]
        __ASM_EMIT("lsr             %[hi], %[count]")               // [ 0, lo >> count ]
        __ASM_EMIT("b               4f")

        __ASM_EMIT("2:")
        __ASM_EMIT("rsb             %[tmp], %[count], $32")         // tmp = (32 - count)
        __ASM_EMIT("lsr             %[hi], %[count]")               // [ lo, hi >> count ]
        __ASM_EMIT("lsl             %[tmp], %[lo], %[tmp]")         // tmp = lo << (32 - count)
        __ASM_EMIT("lsr             %[lo], %[count]")               // lo >> (32 - count)
        __ASM_EMIT("orr             %[hi], %[tmp]")                 // [ lo >> count, (hi >> count) | (lo << (32 - count)) ]

        __ASM_EMIT("4:")
        : [lo] "+r" (lo), [hi] "+r" (hi), [tmp] "=&r" (tmp),
          [count] "+r" (count)
        : :
    );

    return hi | (uint64_t(lo) << 32);
}

inline int64_t reverse_bits(int64_t v, size_t count)
{
    uint32_t lo = uint32_t(v);
    uint32_t hi = uint32_t(v >> 32);
    uint32_t tmp;

    ARCH_ARM_ASM
    (
        __ASM_EMIT("rsb             %[count], %[count], $64")
        __ASM_EMIT("rbit            %[lo], %[lo]")
        __ASM_EMIT("rbit            %[hi], %[hi]")                  // [ lo, hi ]
        __ASM_EMIT("cmp             %[count], $32")
        __ASM_EMIT("blo             2f")

        __ASM_EMIT("mov             %[hi], %[lo]")                  // [ lo, lo ]
        __ASM_EMIT("sub             %[count], $32")
        __ASM_EMIT("eor             %[lo], %[lo]")                  // [ 0, lo ]
        __ASM_EMIT("lsr             %[hi], %[count]")               // [ 0, lo >> count ]
        __ASM_EMIT("b               4f")

        __ASM_EMIT("2:")
        __ASM_EMIT("rsb             %[tmp], %[count], $32")         // tmp = (32 - count)
        __ASM_EMIT("lsr             %[hi], %[count]")               // [ lo, hi >> count ]
        __ASM_EMIT("lsl             %[tmp], %[lo], %[tmp]")         // tmp = lo << (32 - count)
        __ASM_EMIT("lsr             %[lo], %[count]")               // lo >> (32 - count)
        __ASM_EMIT("orr             %[hi], %[tmp]")                 // [ lo >> count, (hi >> count) | (lo << (32 - count)) ]

        __ASM_EMIT("4:")
        : [lo] "+r" (lo), [hi] "+r" (hi), [tmp] "=&r" (tmp),
          [count] "+r" (count)
        : :
    );

    return hi | (int64_t(lo) << 32);
}

#endif /* DSP_ARCH_ARM_BITS_H_ */
