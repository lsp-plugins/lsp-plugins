/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 26 окт. 2018 г.
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

#ifndef DSP_ARCH_ARM_BITS_H_
#define DSP_ARCH_ARM_BITS_H_

#include <dsp/dsp.h>

#ifdef ARCH_ARM7
    #include <dsp/arch/arm/armv7/bits.h>
#else
    #include <dsp/arch/arm/armv6/bits.h>
#endif

inline int __lsp_forced_inline     int_log2(uint8_t v)
{
    int res = v;
    ARCH_ARM_ASM
    (
        __ASM_EMIT("tst             %[res], %[res]")
        __ASM_EMIT("clzne           %[res], %[res]")
        __ASM_EMIT("rsbne           %[res], %[res], $31")
        : [res] "+r" (res)
        : : "cc"
    );
    return res;
}

inline int __lsp_forced_inline     int_log2(int8_t v)
{
    int res = uint8_t(v);
    ARCH_ARM_ASM
    (
        __ASM_EMIT("tst             %[res], %[res]")
        __ASM_EMIT("clzne           %[res], %[res]")
        __ASM_EMIT("rsbne           %[res], %[res], $31")
        : [res] "+r" (res)
        : : "cc"
    );
    return res;
}

inline int __lsp_forced_inline     int_log2(uint16_t v)
{
    int res = v;
    ARCH_ARM_ASM
    (
        __ASM_EMIT("tst             %[res], %[res]")
        __ASM_EMIT("clzne           %[res], %[res]")
        __ASM_EMIT("rsbne           %[res], %[res], $31")
        : [res] "+r" (res)
        : : "cc"
    );
    return res;
}

inline int __lsp_forced_inline     int_log2(int16_t v)
{
    int res = uint16_t(v);
    ARCH_ARM_ASM
    (
        __ASM_EMIT("tst             %[res], %[res]")
        __ASM_EMIT("clzne           %[res], %[res]")
        __ASM_EMIT("rsbne           %[res], %[res], $31")
        : [res] "+r" (res)
        : : "cc"
    );
    return res;
}

inline int __lsp_forced_inline     int_log2(uint32_t v)
{
    int res = int(v);
    ARCH_ARM_ASM
    (
        __ASM_EMIT("tst             %[res], %[res]")
        __ASM_EMIT("clzne           %[res], %[res]")
        __ASM_EMIT("rsbne           %[res], %[res], $31")
        : [res] "+r" (res)
        : : "cc"
    );
    return res;
}

inline int __lsp_forced_inline     int_log2(int32_t v)
{
    int res = int(v);
    ARCH_ARM_ASM
    (
        __ASM_EMIT("tst             %[res], %[res]")
        __ASM_EMIT("clzne           %[res], %[res]")
        __ASM_EMIT("rsbne           %[res], %[res], $31")
        : [res] "+r" (res)
        : : "cc"
    );
    return res;
}

inline int __lsp_forced_inline     int_log2(uint64_t v)
{
    int hi = int(v >> 32);
    int lo = int(v & 0xffffffff);
    ARCH_ARM_ASM
    (
        __ASM_EMIT("tst             %[hi], %[hi]")
        __ASM_EMIT("beq             2f")
        __ASM_EMIT("clzne           %[lo], %[hi]")
        __ASM_EMIT("rsbne           %[lo], %[lo], $63")
        __ASM_EMIT("b               4f")

        __ASM_EMIT("2:")
        __ASM_EMIT("tst             %[lo], %[lo]")
        __ASM_EMIT("clzne           %[lo], %[lo]")
        __ASM_EMIT("rsbne           %[lo], %[lo], $31")

        __ASM_EMIT("4:")
        : [lo] "+r" (lo)
        : [hi] "r" (hi)
        : "cc"
    );
    return lo;
}

inline int __lsp_forced_inline     int_log2(int64_t v)
{
    int hi = int(uint64_t(v) >> 32);
    int lo = int(v & 0xffffffff);
    ARCH_ARM_ASM
    (
        __ASM_EMIT("tst             %[hi], %[hi]")
        __ASM_EMIT("beq             2f")
        __ASM_EMIT("clzne           %[lo], %[hi]")
        __ASM_EMIT("rsbne           %[lo], %[lo], $63")
        __ASM_EMIT("b               4f")

        __ASM_EMIT("2:")
        __ASM_EMIT("tst             %[lo], %[lo]")
        __ASM_EMIT("clzne           %[lo], %[lo]")
        __ASM_EMIT("rsbne           %[lo], %[lo], $31")

        __ASM_EMIT("4:")
        : [lo] "+r" (lo)
        : [hi] "r" (hi)
        : "cc"
    );
    return lo;
}


#endif /* DSP_ARCH_ARM_BITS_H_ */
