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
    )

    return res;
}

inline uint32_t reverse_bits(uint32_t src, size_t count)
{
    uint32_t res;

    ARCH_ARM_ASM (
        __ASM_EMIT("rsub    %[count], %[count], $32")
        __ASM_EMIT("rbit    %[res], %[src]")
        __ASM_EMIT("lsr     %[res], %[count]")
        : [res] "=r" (res), [count] "+r" (count)
        : [src] "r" (src)
        :
    )

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
    )

    return res;
}

inline int32_t reverse_bits(int32_t src, size_t count)
{
    int32_t res;

    ARCH_ARM_ASM (
        __ASM_EMIT("rsub    %[count], %[count], $32")
        __ASM_EMIT("rbit    %[res], %[src]")
        __ASM_EMIT("lsr     %[res], %[count]")
        : [res] "=r" (res), [count] "+r" (count)
        : [src] "r" (src)
        :
    )

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
    )

    return res;
}

inline uint16_t reverse_bits(uint16_t src, size_t count)
{
    uint32_t res = src;

    ARCH_ARM_ASM (
        __ASM_EMIT("rsub    %[count], %[count], $32")
        __ASM_EMIT("rbit    %[res], %[res]")
        __ASM_EMIT("lsr     %[res], %[count]")
        : [res] "=r" (res), [count] "+r" (count)
        : :
    )

    return uint16_t(res);
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
    )

    return res;
}

inline int16_t reverse_bits(int16_t src, size_t count)
{
    uint32_t res = uint16_t(src);

    ARCH_ARM_ASM (
        __ASM_EMIT("rsub    %[count], %[count], $32")
        __ASM_EMIT("rbit    %[res], %[res]")
        __ASM_EMIT("lsr     %[res], %[count]")
        : [res] "=r" (res), [count] "+r" (count)
        : :
    )

    return int16_t(res);
}

inline uint8_t reverse_bits(uint8_t src)
{
    uint16_t res;

    ARCH_ARM_ASM (
        __ASM_EMIT("rbit    %[res], %[src]")
        __ASM_EMIT("lsr     %[res], $24")
        : [res] "=r" (res)
        : [src] "r" (src)
        :
    )

    return res;
}

inline uint16_t reverse_bits(uint8_t src, size_t count)
{
    uint32_t res = src;

    ARCH_ARM_ASM (
        __ASM_EMIT("rsub    %[count], %[count], $32")
        __ASM_EMIT("rbit    %[res], %[res]")
        __ASM_EMIT("lsr     %[res], %[count]")
        : [res] "=r" (res), [count] "+r" (count)
        : :
    )

    return uint8_t(res);
}

inline int8_t reverse_bits(int8_t src)
{
    int16_t res;

    ARCH_ARM_ASM (
        __ASM_EMIT("rbit    %[res], %[src]")
        __ASM_EMIT("lsr     %[res], $24")
        : [res] "=r" (res)
        : [src] "r" (src)
        :
    )

    return res;
}

inline int8_t reverse_bits(int8_t src, size_t count)
{
    uint32_t res = src;

    ARCH_ARM_ASM (
        __ASM_EMIT("rsub    %[count], %[count], $32")
        __ASM_EMIT("rbit    %[res], %[res]")
        __ASM_EMIT("lsr     %[res], %[count]")
        : [res] "=r" (res), [count] "+r" (count)
        : :
    )

    return int8_t(res);
}

#endif /* DSP_ARCH_ARM_BITS_H_ */
