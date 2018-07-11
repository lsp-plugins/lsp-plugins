/*
 * bits.h
 *
 *  Created on: 06 февр. 2016 г.
 *      Author: sadko
 */

#ifndef CORE_BITS_H_
#define CORE_BITS_H_

#include <core/types.h>

namespace lsp
{
    extern const uint8_t    __rb[];
}

#if defined(__i386__) || defined(__x86_64__)
    #include <core/x86/bits.h>
#else
    #include <core/native/bits.h>
#endif

namespace lsp
{
    inline int8_t __lsp_forced_inline      reverse_bits(int8_t v)
    {
        return reverse_bits(uint8_t(v));
    }

    inline int16_t __lsp_forced_inline     reverse_bits(int16_t v)
    {
        return reverse_bits(uint16_t(v));
    }

    inline int32_t __lsp_forced_inline     reverse_bits(int32_t v)
    {
        return reverse_bits(uint32_t(v));
    }

    inline int64_t __lsp_forced_inline     reverse_bits(int64_t v)
    {
        return reverse_bits(uint64_t(v));
    }

    inline int8_t __lsp_forced_inline      reverse_bits(int8_t v, size_t count)
    {
        return reverse_bits(uint8_t(v), count);
    }

    inline int16_t __lsp_forced_inline     reverse_bits(int16_t v, size_t count)
    {
        return reverse_bits(uint16_t(v), count);
    }

    inline int32_t __lsp_forced_inline     reverse_bits(int32_t v, size_t count)
    {
        return reverse_bits(uint32_t(v), count);
    }

    inline int64_t __lsp_forced_inline     reverse_bits(int64_t v, size_t count)
    {
        return reverse_bits(uint64_t(v), count);
    }
}

#endif /* CORE_BITS_H_ */
