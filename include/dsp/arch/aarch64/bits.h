/*
 * bits.h
 *
 *  Created on: 18 мар. 2019 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_AARCH64_BITS_H_
#define DSP_ARCH_AARCH64_BITS_H_

inline uint64_t reverse_bits(uint64_t src)
{
    uint64_t res;

    ARCH_AARCH64_ASM (
        __ASM_EMIT("rbit    %[res], %[src]")
        : [res] "=r" (res)
        : [src] "r" (src)
        :
    );

    return res;
}

inline uint64_t reverse_bits(uint64_t src, size_t count)
{
    uint64_t res, tmp;

    ARCH_AARCH64_ASM (
        __ASM_EMIT("mov     %[tmp], #64")
        __ASM_EMIT("sub     %[count], %[tmp], %[count]")
        __ASM_EMIT("rbit    %[res], %[src]")
        __ASM_EMIT("lsr     %[res], %[res], %[count]")
        : [res] "=r" (res), [count] "+r" (count), [tmp] "=&r" (tmp)
        : [src] "r" (src)
        :
    );

    return res;
}

inline int64_t reverse_bits(int64_t src)
{
    int64_t res;

    ARCH_AARCH64_ASM (
        __ASM_EMIT("rbit    %[res], %[src]")
        : [res] "=r" (res)
        : [src] "r" (src)
        :
    );

    return res;
}

inline int64_t reverse_bits(int64_t src, size_t count)
{
    int64_t res, tmp;

    ARCH_AARCH64_ASM (
        __ASM_EMIT("mov     %[tmp], #64")
        __ASM_EMIT("sub     %[count], %[tmp], %[count]")
        __ASM_EMIT("rbit    %[res], %[src]")
        __ASM_EMIT("lsr     %[res], %[res], %[count]")
        : [res] "=r" (res), [count] "+r" (count), [tmp] "=&r" (tmp)
        : [src] "r" (src)
        :
    );

    return res;
}

inline uint32_t reverse_bits(uint32_t src)
{
    uint32_t res;

    ARCH_AARCH64_ASM (
        __ASM_EMIT("rbit    %w[res], %w[src]")
        : [res] "=r" (res)
        : [src] "r" (src)
        :
    );

    return res;
}

inline uint32_t reverse_bits(uint32_t src, size_t count)
{
    uint32_t res, tmp;

    ARCH_AARCH64_ASM (
        __ASM_EMIT("mov     %[tmp], #64")
        __ASM_EMIT("sub     %[count], %[tmp], %[count]")
        __ASM_EMIT("rbit    %[res], %[src]")
        __ASM_EMIT("lsr     %[res], %[res], %[count]")
        : [res] "=r" (res), [count] "+r" (count), [tmp] "=&r" (tmp)
        : [src] "r" (src)
        :
    );

    return res;
}

inline int32_t reverse_bits(int32_t src)
{
    int32_t res;

    ARCH_AARCH64_ASM (
        __ASM_EMIT("rbit    %w[res], %w[src]")
        : [res] "=r" (res)
        : [src] "r" (src)
        :
    );

    return res;
}

inline int32_t reverse_bits(int32_t src, size_t count)
{
    int32_t res, tmp;

    ARCH_AARCH64_ASM (
        __ASM_EMIT("mov     %[tmp], #64")
        __ASM_EMIT("sub     %[count], %[tmp], %[count]")
        __ASM_EMIT("rbit    %[res], %[src]")
        __ASM_EMIT("lsr     %[res], %[res], %[count]")
        : [res] "=r" (res), [count] "+r" (count), [tmp] "=&r" (tmp)
        : [src] "r" (src)
        :
    );

    return res;
}

inline uint16_t reverse_bits(uint16_t src)
{
    uint16_t res;

    ARCH_AARCH64_ASM (
        __ASM_EMIT("rbit    %[res], %[src]")
        __ASM_EMIT("lsr     %[res], %[res], #48")
        : [res] "=r" (res)
        : [src] "r" (src)
        :
    );

    return res;
}

inline uint16_t reverse_bits(uint16_t src, size_t count)
{
    uint16_t res;
    uint64_t tmp;

    ARCH_AARCH64_ASM (
        __ASM_EMIT("mov     %[tmp], #64")
        __ASM_EMIT("sub     %[count], %[tmp], %[count]")
        __ASM_EMIT("rbit    %[res], %[src]")
        __ASM_EMIT("lsr     %[res], %[res], %[count]")
        : [res] "=r" (res), [count] "+r" (count), [tmp] "=&r" (tmp)
        : [src] "r" (src)
        :
    );

    return res;
}

inline int16_t reverse_bits(int16_t src)
{
    int16_t res;

    ARCH_AARCH64_ASM (
        __ASM_EMIT("rbit    %[res], %[src]")
        __ASM_EMIT("lsr     %[res], %[res], #48")
        : [res] "=r" (res)
        : [src] "r" (src)
        :
    );

    return res;
}

inline int16_t reverse_bits(int16_t src, size_t count)
{
    int16_t res;
    uint64_t tmp;

    ARCH_AARCH64_ASM (
        __ASM_EMIT("mov     %[tmp], #64")
        __ASM_EMIT("sub     %[count], %[tmp], %[count]")
        __ASM_EMIT("rbit    %[res], %[src]")
        __ASM_EMIT("lsr     %[res], %[res], %[count]")
        : [res] "=r" (res), [count] "+r" (count), [tmp] "=&r" (tmp)
        : [src] "r" (src)
        :
    );

    return res;
}

inline uint8_t reverse_bits(uint8_t src)
{
    uint8_t res;

    ARCH_AARCH64_ASM (
        __ASM_EMIT("rbit    %[res], %[src]")
        __ASM_EMIT("lsr     %[res], %[res], #56")
        : [res] "=r" (res)
        : [src] "r" (src)
        :
    );

    return res;
}

inline uint8_t reverse_bits(uint8_t src, size_t count)
{
    uint8_t res;
    uint64_t tmp;

    ARCH_AARCH64_ASM (
        __ASM_EMIT("mov     %[tmp], #64")
        __ASM_EMIT("sub     %[count], %[tmp], %[count]")
        __ASM_EMIT("rbit    %[res], %[src]")
        __ASM_EMIT("lsr     %[res], %[res], %[count]")
        : [res] "=r" (res), [count] "+r" (count), [tmp] "=&r" (tmp)
        : [src] "r" (src)
        :
    );

    return res;
}

inline int8_t reverse_bits(int8_t src)
{
    int8_t res;

    ARCH_AARCH64_ASM (
        __ASM_EMIT("rbit    %[res], %[src]")
        __ASM_EMIT("lsr     %[res], %[res], #56")
        : [res] "=r" (res)
        : [src] "r" (src)
        :
    );

    return res;
}

inline int8_t reverse_bits(int8_t src, size_t count)
{
    int8_t res;
    uint64_t tmp;

    ARCH_AARCH64_ASM (
        __ASM_EMIT("mov     %[tmp], #64")
        __ASM_EMIT("sub     %[count], %[tmp], %[count]")
        __ASM_EMIT("rbit    %[res], %[src]")
        __ASM_EMIT("lsr     %[res], %[res], %[count]")
        : [res] "=r" (res), [count] "+r" (count), [tmp] "=&r" (tmp)
        : [src] "r" (src)
        :
    );

    return res;
}

inline int __lsp_forced_inline     int_log2(uint8_t v)
{
    int32_t res;
    ARCH_AARCH64_ASM (
        __ASM_EMIT("subs            %w[res], %w[v], #0")    // res = v
        __ASM_EMIT("b.eq            1f")                    // res == 0?
        __ASM_EMIT("clz             %w[res], %w[v]")
        __ASM_EMIT("mov             %w[v], #31")
        __ASM_EMIT("sub             %w[res], %w[v], %w[res]")
        __ASM_EMIT("1:")
        : [res] "=r" (res), [v] "+r" (v)
        : : "cc"
    );
    return res;
}

inline int __lsp_forced_inline     int_log2(int8_t v)
{
    int32_t res;
    ARCH_AARCH64_ASM (
        __ASM_EMIT("subs            %w[res], %w[v], #0")    // res = v
        __ASM_EMIT("b.eq            1f")                    // res == 0?
        __ASM_EMIT("clz             %w[res], %w[v]")
        __ASM_EMIT("mov             %w[v], #31")
        __ASM_EMIT("sub             %w[res], %w[v], %w[res]")
        __ASM_EMIT("1:")
        : [res] "=r" (res), [v] "+r" (v)
        : : "cc"
    );
    return res;
}

inline int __lsp_forced_inline     int_log2(uint16_t v)
{
    int32_t res;
    ARCH_AARCH64_ASM (
        __ASM_EMIT("subs            %w[res], %w[v], #0")    // res = v
        __ASM_EMIT("b.eq            1f")                    // res == 0?
        __ASM_EMIT("clz             %w[res], %w[v]")
        __ASM_EMIT("mov             %w[v], #31")
        __ASM_EMIT("sub             %w[res], %w[v], %w[res]")
        __ASM_EMIT("1:")
        : [res] "=r" (res), [v] "+r" (v)
        : : "cc"
    );
    return res;
}

inline int __lsp_forced_inline     int_log2(int16_t v)
{
    int32_t res;
    ARCH_AARCH64_ASM (
        __ASM_EMIT("subs            %w[res], %w[v], #0")    // res = v
        __ASM_EMIT("b.eq            1f")                    // res == 0?
        __ASM_EMIT("clz             %w[res], %w[v]")
        __ASM_EMIT("mov             %w[v], #31")
        __ASM_EMIT("sub             %w[res], %w[v], %w[res]")
        __ASM_EMIT("1:")
        : [res] "=r" (res), [v] "+r" (v)
        : : "cc"
    );
    return res;
}

inline int __lsp_forced_inline     int_log2(uint32_t v)
{
    int32_t res;
    ARCH_AARCH64_ASM (
        __ASM_EMIT("subs            %w[res], %w[v], #0")    // res = v
        __ASM_EMIT("b.eq            1f")                    // res == 0?
        __ASM_EMIT("clz             %w[res], %w[v]")
        __ASM_EMIT("mov             %w[v], #31")
        __ASM_EMIT("sub             %w[res], %w[v], %w[res]")
        __ASM_EMIT("1:")
        : [res] "=r" (res), [v] "+r" (v)
        : : "cc"
    );
    return res;
}

inline int __lsp_forced_inline     int_log2(int32_t v)
{
    int32_t res;
    ARCH_AARCH64_ASM (
        __ASM_EMIT("subs            %w[res], %w[v], #0")    // res = v
        __ASM_EMIT("b.eq            1f")                    // res == 0?
        __ASM_EMIT("clz             %w[res], %w[v]")
        __ASM_EMIT("mov             %w[v], #31")
        __ASM_EMIT("sub             %w[res], %w[v], %w[res]")
        __ASM_EMIT("1:")
        : [res] "=r" (res), [v] "+r" (v)
        : : "cc"
    );
    return res;
}

inline int __lsp_forced_inline     int_log2(uint64_t v)
{
    int32_t res;
    ARCH_AARCH64_ASM (
        __ASM_EMIT("subs            %x[res], %x[v], #0")    // res = v
        __ASM_EMIT("b.eq            1f")                    // res == 0?
        __ASM_EMIT("clz             %x[res], %x[v]")
        __ASM_EMIT("mov             %x[v], #63")
        __ASM_EMIT("sub             %x[res], %x[v], %x[res]")
        __ASM_EMIT("1:")
        : [res] "=r" (res), [v] "+r" (v)
        : : "cc"
    );
    return res;
}

inline int __lsp_forced_inline     int_log2(int64_t v)
{
    int32_t res;
    ARCH_AARCH64_ASM (
        __ASM_EMIT("subs            %x[res], %x[v], #0")    // res = v
        __ASM_EMIT("b.eq            1f")                    // res == 0?
        __ASM_EMIT("clz             %x[res], %x[v]")
        __ASM_EMIT("mov             %x[v], #63")
        __ASM_EMIT("sub             %x[res], %x[v], %x[res]")
        __ASM_EMIT("1:")
        : [res] "=r" (res), [v] "+r" (v)
        : : "cc"
    );
    return res;
}


#endif /* DSP_ARCH_AARCH64_BITS_H_ */
