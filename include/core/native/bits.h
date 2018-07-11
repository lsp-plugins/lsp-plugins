/*
 * bits.h
 *
 *  Created on: 06 февр. 2016 г.
 *      Author: sadko
 */

#ifndef CORE_NATIVE_BITS_H_
#define CORE_NATIVE_BITS_H_

namespace lsp
{
    #define ITERATE \
        result      <<=     8; \
        v           >>=     8; \
        result      |=      __rb[v & 0xff];

    inline uint8_t      reverse_bits(uint8_t src)
    {
        return __rb[src];
    }

    inline uint16_t     reverse_bits(uint16_t v)
    {
        uint32_t    result = __rb[v & 0xff];
        ITERATE;

        return result;
    }

    inline uint32_t     reverse_bits(uint32_t v)
    {
        uint32_t    result = __rb[v & 0xff];
        ITERATE;
        ITERATE;
        ITERATE;

        return result;
    }

    inline uint64_t     reverse_bits(uint64_t v)
    {
        uint64_t    result = __rb[v & 0xff];
        ITERATE;
        ITERATE;
        ITERATE;
        ITERATE;
        ITERATE;
        ITERATE;
        ITERATE;

        return result;
    }

    inline uint8_t      reverse_bits(uint8_t v, size_t count)
    {
        return reverse_bits(v) >> (sizeof(uint8_t) * 8 - count);
    }

    inline uint16_t     reverse_bits(uint16_t v, size_t count)
    {
        return reverse_bits(v) >> (sizeof(uint16_t) * 8 - count);
    }

    inline uint32_t     reverse_bits(uint32_t v, size_t count)
    {
        return reverse_bits(v) >> (sizeof(uint32_t) * 8 - count);
    }

    inline uint64_t     reverse_bits(uint64_t v, size_t count)
    {
        return reverse_bits(v) >> (sizeof(uint64_t) * 8 - count);
    }


    #undef ITERATE
}

#endif /* CORE_NATIVE_BITS_H_ */
