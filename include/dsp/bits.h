/*
 * bits.h
 *
 *  Created on: 06 февр. 2016 г.
 *      Author: sadko
 */

#ifndef DSP_BITS_H_
#define DSP_BITS_H_

#include <dsp/types.h>

namespace lsp
{
    extern const uint8_t    __rb[];
}

#if defined(ARCH_X86)
    #include <dsp/arch/x86/bits.h>
#else
    #include <dsp/arch/native/bits.h>
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

#endif /* DSP_BITS_H_ */
