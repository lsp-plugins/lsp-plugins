/*
 * types.h
 *
 *  Created on: 26 окт. 2015 г.
 *      Author: sadko
 */

#ifndef _CORE_TYPES_H_
#define _CORE_TYPES_H_

#include <sys/types.h>
#include <stddef.h>

#ifdef __linux__
    #include <linux/limits.h>
#endif /* __linux__ */

#define __ASM_EMIT(code)                    code "\n\t"

// Special symbols
#define STR_A_UMLAUT_SMALL                  "\x84"
#define STR_A_UMLAUT_LARGE                  "\x8e"
#define STR_U_UMLAUT_SMALL                  "\x81"
#define STR_U_UMLAUT_LARGE                  "\x9a"
#define STR_O_UMLAUT_SMALL                  "\x94"
#define STR_O_UMLAUT_LARGE                  "\x99"
#define STR_ESZETT                          "\xe1"

namespace lsp
{
    typedef __uint32_t      uint32_t;

    // Mesh port structure
    typedef struct mesh_t
    {
        size_t          nBuffers;
        size_t          nItems;
        float          *pvData[];
    } mesh_t;
}

#include <core/units.h>

#endif /* TYPES_H_ */
