/*
 * endian.h
 *
 *  Created on: 04 апр. 2016 г.
 *      Author: sadko
 */

#ifndef CORE_NATIVE_BSWAP_H_
#define CORE_NATIVE_BSWAP_H_

#include <core/types.h>

namespace lsp
{
    inline uint8_t __lsp_forced_inline      reverse_bytes(uint8_t v)
    {
        return v;
    }

    inline uint16_t __lsp_forced_inline     reverse_bytes(uint16_t v)
    {
        return (v >> 8) | (v << 8);
    }

    inline uint32_t __lsp_forced_inline     reverse_bytes(uint32_t v)
    {
        v   = ((v & 0xff00ff00) >> 8) | ((v & 0x00ff00ff) << 8);
        return (v >> 16) | (v << 16);
//        return (v >> 24) |
//               ((v >> 8) & (0x0000ff00)) |
//               (v << 24) |
//               ((v << 8) & (0x00ff0000));
    }

    inline uint64_t __lsp_forced_inline     reverse_bytes(uint64_t v)
    {
        v   = ((v & 0xff00ff00ff00ff00ULL) >> 8)  | ((v & 0x00ff00ff00ff00ffULL) << 8);
        v   = ((v & 0xffff0000ffff0000ULL) >> 16) | ((v & 0x0000ffff0000ffffULL) << 16);
        return (v >> 32) | (v << 32);

//        return (v >> 56) |
//               ((v >> 40) & 0x0000ff00) |
//               ((v >> 24) & 0x00ff0000) |
//               ((v >> 8)  & 0xff000000) |
//               (v << 56) |
//               ((v & 0x0000ff00) << 40) |
//               ((v & 0x00ff0000) << 24) |
//               ((v & 0xff000000) << 8)
//        ;
    }

    inline float __lsp_forced_inline reverse_bytes(float v)
    {
        union {
            float fValue;
            uint32_t uValue;
        } tmp;
        tmp.fValue      = v;
        tmp.uValue      = reverse_bytes(tmp.uValue);
        return tmp.fValue;
    }

    inline double __lsp_forced_inline reverse_bytes(double v)
    {
        union {
            float fValue;
            uint64_t uValue;
        } tmp;
        tmp.fValue      = v;
        tmp.uValue      = reverse_bytes(tmp.uValue);
        return tmp.fValue;
    }
}

#endif /* CORE_NATIVE_BSWAP_H_ */
