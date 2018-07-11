/*
 * endian.h
 *
 *  Created on: 04 апр. 2016 г.
 *      Author: sadko
 */

#ifndef CORE_ENDIAN_H_
#define CORE_ENDIAN_H_

#include <core/types.h>

// Include unsigned functions definition
#if defined(__x86_64__) || defined(__i386__)
    #include <core/x86/bswap.h>
#else
    #include <core/native/bswap.h>
#endif /* defined(__x86_64__) || defined(__i386__) */

// Define macros
#if defined(__x86_64__) || defined(__i386__)
    #define LE_DATA(x)              (x)
    #define BE_DATA(x)              byte_swap(x)
#else
    #error "Has to be specified LE/BE model for the architecture"
#endif /* */

namespace lsp
{
    inline int8_t __lsp_forced_inline       byte_swap(int8_t v)
    {
        return v;
    }

    inline int16_t __lsp_forced_inline       byte_swap(int16_t v)
    {
        return byte_swap(uint16_t(v));
    }

    inline int32_t __lsp_forced_inline       byte_swap(int32_t v)
    {
        return byte_swap(uint32_t(v));
    }

    inline int64_t __lsp_forced_inline       byte_swap(int64_t v)
    {
        return byte_swap(uint64_t(v));
    }
}


#endif /* CORE_ENDIAN_H_ */
