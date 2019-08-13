/*
 * endian.h
 *
 *  Created on: 04 апр. 2016 г.
 *      Author: sadko
 */

#ifndef DSP_ENDIAN_H_
#define DSP_ENDIAN_H_

#include <common/types.h>

// Include unsigned functions definition
#if defined(ARCH_X86)
    #include <dsp/arch/x86/bswap.h>
#elif defined(ARCH_AARCH64)
    #include <dsp/arch/aarch64/bswap.h>
#elif defined(ARCH_ARM)
    #include <dsp/arch/arm/bswap.h>
#else
    #include <dsp/arch/native/bswap.h>
#endif /* defined(ARCH_X86) */

// Define macros
#ifdef ARCH_LE
    #define LE_TO_CPU(x)            (x)
    #define CPU_TO_LE(x)            (x)

    #define BE_TO_CPU(x)            byte_swap(x)
    #define CPU_TO_BE(x)            byte_swap(x)

    #define VLE_TO_CPU(v, n)
    #define CPU_TO_VLE(v, n)

    #define VBE_TO_CPU(v, n)        byte_swap(v, n)
    #define CPU_TO_VBE(v, n)        byte_swap(v, n)
    
#else
    #define LE_TO_CPU(x)            byte_swap(x)
    #define CPU_TO_LE(x)            byte_swap(x)

    #define BE_TO_CPU(x)            (x)
    #define CPU_TO_BE(x)            (x)

    #define VLE_TO_CPU(v, n)        byte_swap(v, n)
    #define CPU_TO_VLE(v, n)        byte_swap(v, n)

    #define VBE_TO_CPU(v, n)
    #define CPU_TO_VBE(v, n)

#endif /* */

inline uint8_t __lsp_forced_inline    byte_swap(uint8_t v)
{
    return v;
}

inline int8_t __lsp_forced_inline       byte_swap(int8_t v)
{
    return v;
}

inline int16_t __lsp_forced_inline      byte_swap(int16_t v)
{
    return byte_swap(uint16_t(v));
}

inline int32_t __lsp_forced_inline      byte_swap(int32_t v)
{
    return byte_swap(uint32_t(v));
}

inline int64_t __lsp_forced_inline      byte_swap(int64_t v)
{
    return byte_swap(uint64_t(v));
}

inline void __lsp_forced_inline    byte_swap(uint8_t *v, size_t n)
{
}

inline void __lsp_forced_inline    byte_swap(int8_t *v, size_t n)
{
}

#endif /* DSP_ENDIAN_H_ */
