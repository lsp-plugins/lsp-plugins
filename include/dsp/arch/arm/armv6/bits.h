/*
 * bits.h
 *
 *  Created on: 22 сент. 2019 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_ARM_ARMV6_BITS_H_
#define DSP_ARCH_ARM_ARMV6_BITS_H_

#include <dsp/dsp.h>

#define ARMV6_MV_RBIT32(dst, src, msk, tmp, masks) \
    __ASM_EMIT("rev     " dst ", " src ) \
    /* round 1 */ \
    __ASM_EMIT("ldr     " msk ", [" masks ", #0]") \
    __ASM_EMIT("and     " tmp ", " msk ", " dst ", lsr #1") \
    __ASM_EMIT("and     " msk ", " msk ", " dst) \
    __ASM_EMIT("orr     " dst ", " tmp ", " msk ", lsl #1") \
    /* round 2 */ \
    __ASM_EMIT("ldr     " msk ", [" masks ", #4]") \
    __ASM_EMIT("and     " tmp ", " msk ", " dst ", lsr #2") \
    __ASM_EMIT("and     " msk ", " msk ", " dst) \
    __ASM_EMIT("orr     " dst ", " tmp ", " msk ", lsl #2") \
    /* round 3 */ \
    __ASM_EMIT("ldr     " msk ", [" masks ", #8]") \
    __ASM_EMIT("and     " tmp ", " msk ", " dst ", lsr #4") \
    __ASM_EMIT("and     " msk ", " msk ", " dst) \
    __ASM_EMIT("orr     " dst ", " tmp ", " msk ", lsl #4") \

#define ARMV6_RBIT32(dst, msk, tmp, masks)      ARMV6_MV_RBIT32(dst, dst, msk, tmp, masks)

extern const uint32_t __rb_masks[];

inline uint32_t reverse_bits(uint32_t src)
{
    uint32_t msk, tmp;

    ARCH_ARM_ASM (
        ARMV6_RBIT32("%[src]", "%[msk]", "%[tmp]", "%[masks]")
        : [src] "+r" (src),
          [msk] "=&r" (msk), [tmp] "=&r" (tmp)
        : [masks] "r" (__rb_masks)
        :
    );

    return src;
}


inline uint32_t reverse_bits(uint32_t src, size_t count)
{
    uint32_t msk, tmp;

    ARCH_ARM_ASM (
        __ASM_EMIT("rsb     %[count], %[count], $32")
        ARMV6_RBIT32("%[src]", "%[msk]", "%[tmp]", "%[masks]")
        __ASM_EMIT("lsr     %[src], %[count]")
        : [src] "+r" (src), [count] "+r" (count),
          [msk] "=&r" (msk), [tmp] "=&r" (tmp)
        : [masks] "r" (__rb_masks)
        :
    );

    return src;
}

inline int32_t reverse_bits(int32_t src)
{
    uint32_t msk, tmp;

    ARCH_ARM_ASM (
        ARMV6_RBIT32("%[src]", "%[msk]", "%[tmp]", "%[masks]")
        : [src] "+r" (src),
          [msk] "=&r" (msk), [tmp] "=&r" (tmp)
        : [masks] "r" (__rb_masks)
        :
    );

    return src;
}

inline int32_t reverse_bits(int32_t src, size_t count)
{
    uint32_t msk, tmp;

    ARCH_ARM_ASM (
        __ASM_EMIT("rsb     %[count], %[count], $32")
        ARMV6_RBIT32("%[src]", "%[msk]", "%[tmp]", "%[masks]")
        __ASM_EMIT("lsr     %[src], %[count]")
        : [src] "+r" (src), [count] "+r" (count),
          [msk] "=&r" (msk), [tmp] "=&r" (tmp)
        : [masks] "r" (__rb_masks)
        :
    );

    return src;
}

inline uint16_t reverse_bits(uint16_t src)
{
    uint32_t msk, tmp;

    ARCH_ARM_ASM (
        ARMV6_RBIT32("%[src]", "%[msk]", "%[tmp]", "%[masks]")
        __ASM_EMIT("lsr %[src], $16")
        : [src] "+r" (src),
          [msk] "=&r" (msk), [tmp] "=&r" (tmp)
        : [masks] "r" (__rb_masks)
        :
    );

    return src;
}

inline uint16_t reverse_bits(uint16_t src, size_t count)
{
    uint32_t msk, tmp;

    ARCH_ARM_ASM (
        __ASM_EMIT("rsb     %[count], %[count], $32")
        ARMV6_RBIT32("%[src]", "%[msk]", "%[tmp]", "%[masks]")
        __ASM_EMIT("lsr     %[src], %[count]")
        : [src] "+r" (src), [count] "+r" (count),
          [msk] "=&r" (msk), [tmp] "=&r" (tmp)
        : [masks] "r" (__rb_masks)
        :
    );

    return src;
}

inline int16_t reverse_bits(int16_t src)
{
    uint32_t msk, tmp;

    ARCH_ARM_ASM (
        ARMV6_RBIT32("%[src]", "%[msk]", "%[tmp]", "%[masks]")
        __ASM_EMIT("lsr %[src], $16")
        : [src] "+r" (src),
          [msk] "=&r" (msk), [tmp] "=&r" (tmp)
        : [masks] "r" (__rb_masks)
        :
    );

    return src;
}

inline int16_t reverse_bits(int16_t src, size_t count)
{
    uint32_t msk, tmp;

    ARCH_ARM_ASM (
        __ASM_EMIT("rsb     %[count], %[count], $32")
        ARMV6_RBIT32("%[src]", "%[msk]", "%[tmp]", "%[masks]")
        __ASM_EMIT("lsr     %[src], %[count]")
        : [src] "+r" (src), [count] "+r" (count),
          [msk] "=&r" (msk), [tmp] "=&r" (tmp)
        : [masks] "r" (__rb_masks)
        :
    );

    return src;
}

inline uint8_t reverse_bits(uint8_t src)
{
    uint32_t msk, tmp;

    ARCH_ARM_ASM (
        ARMV6_RBIT32("%[src]", "%[msk]", "%[tmp]", "%[masks]")
        __ASM_EMIT("lsr %[src], $24")
        : [src] "+r" (src),
          [msk] "=&r" (msk), [tmp] "=&r" (tmp)
        : [masks] "r" (__rb_masks)
        :
    );

    return src;
}

inline uint8_t reverse_bits(uint8_t src, size_t count)
{
    uint32_t msk, tmp;

    ARCH_ARM_ASM (
        __ASM_EMIT("rsb     %[count], %[count], $32")
        ARMV6_RBIT32("%[src]", "%[msk]", "%[tmp]", "%[masks]")
        __ASM_EMIT("lsr     %[src], %[count]")
        : [src] "+r" (src), [count] "+r" (count),
          [msk] "=&r" (msk), [tmp] "=&r" (tmp)
        : [masks] "r" (__rb_masks)
        :
    );

    return src;
}

inline int8_t reverse_bits(int8_t src)
{
    uint32_t msk, tmp;

    ARCH_ARM_ASM (
        ARMV6_RBIT32("%[src]", "%[msk]", "%[tmp]", "%[masks]")
        __ASM_EMIT("lsr %[src], $24")
        : [src] "+r" (src),
          [msk] "=&r" (msk), [tmp] "=&r" (tmp)
        : [masks] "r" (__rb_masks)
    );

    return src;
}

inline int8_t reverse_bits(int8_t src, size_t count)
{
    uint32_t msk, tmp;

    ARCH_ARM_ASM (
        __ASM_EMIT("rsb     %[count], %[count], $32")
        ARMV6_RBIT32("%[src]", "%[msk]", "%[tmp]", "%[masks]")
        __ASM_EMIT("lsr     %[src], %[count]")
        : [src] "+r" (src), [count] "+r" (count),
          [msk] "=&r" (msk), [tmp] "=&r" (tmp)
        : [masks] "r" (__rb_masks)
        :
    );

    return src;
}

inline uint64_t reverse_bits(uint64_t v)
{
    uint32_t lo = uint32_t(v);
    uint32_t hi = uint32_t(v >> 32);
    uint32_t msk, tmp;

    ARCH_ARM_ASM
    (
        ARMV6_RBIT32("%[lo]", "%[msk]", "%[tmp]", "%[masks]")
        ARMV6_RBIT32("%[hi]", "%[msk]", "%[tmp]", "%[masks]")
        : [lo] "+r" (lo), [hi] "+r" (hi),
          [msk] "=&r" (msk), [tmp] "=&r" (tmp)
        : [masks] "r" (__rb_masks)
        :
    );

    return hi | (uint64_t(lo) << 32);
}

inline int64_t reverse_bits(int64_t v)
{
    uint32_t lo = uint32_t(v);
    uint32_t hi = uint32_t(v >> 32);
    uint32_t msk, tmp;

    ARCH_ARM_ASM
    (
        ARMV6_RBIT32("%[lo]", "%[msk]", "%[tmp]", "%[masks]")
        ARMV6_RBIT32("%[hi]", "%[msk]", "%[tmp]", "%[masks]")
        : [lo] "+r" (lo), [hi] "+r" (hi),
          [msk] "=&r" (msk), [tmp] "=&r" (tmp)
        : [masks] "r" (__rb_masks)
        :
    );

    return hi | (int64_t(lo) << 32);
}

inline uint64_t reverse_bits(uint64_t v, size_t count)
{
    uint32_t lo = uint32_t(v);
    uint32_t hi = uint32_t(v >> 32);
    uint32_t msk, tmp;

    ARCH_ARM_ASM
    (
        __ASM_EMIT("rsb             %[count], %[count], $64")
        ARMV6_RBIT32("%[lo]", "%[msk]", "%[tmp]", "%[masks]")
        ARMV6_RBIT32("%[hi]", "%[msk]", "%[tmp]", "%[masks]")
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
        : [lo] "+r" (lo), [hi] "+r" (hi), [count] "+r" (count),
          [msk] "=&r" (msk), [tmp] "=&r" (tmp)
        : [masks] "r" (__rb_masks)
        :
    );

    return hi | (uint64_t(lo) << 32);
}

inline int64_t reverse_bits(int64_t v, size_t count)
{
    uint32_t lo = uint32_t(v);
    uint32_t hi = uint32_t(v >> 32);
    uint32_t msk, tmp;

    ARCH_ARM_ASM
    (
        __ASM_EMIT("rsb             %[count], %[count], $64")
        ARMV6_RBIT32("%[lo]", "%[msk]", "%[tmp]", "%[masks]")
        ARMV6_RBIT32("%[hi]", "%[msk]", "%[tmp]", "%[masks]")
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
        : [lo] "+r" (lo), [hi] "+r" (hi), [count] "+r" (count),
          [msk] "=&r" (msk), [tmp] "=&r" (tmp)
        : [masks] "r" (__rb_masks)
        :
    );

    return hi | (int64_t(lo) << 32);
}

#endif /* DSP_ARCH_ARM_ARMV6_BITS_H_ */
