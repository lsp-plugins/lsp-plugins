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

// For IDEs: define this symbol in IDE to properly compile and debug
#ifdef LSP_IDE_DEBUG
    #define LSP_USE_EXPAT
    #define LSP_HOST_SIMPULATION
#endif /* LSP_IDE_DEBUG */

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

    enum mesh_state_t
    {
        M_WAIT,         // Mesh is waiting for data request
        M_EMPTY,        // Mesh is empty
        M_DATA          // Mesh contains data
    };

    // Mesh port structure
    typedef struct mesh_t
    {
        mesh_state_t    nState;
        size_t          nBuffers;
        size_t          nItems;
        float          *pvData[];

        inline bool isEmpty() const         { return nState == M_EMPTY; };
        inline bool containsData() const    { return nState == M_DATA; };
        inline bool isWaiting() const       { return nState == M_WAIT;  };

        inline void data(size_t bufs, size_t items)
        {
            nBuffers    = bufs;
            nItems      = items;
            nState      = M_DATA; // This should be the last operation
        }

        inline void cleanup()
        {
            nBuffers    = 0;
            nItems      = 0;
            nState      = M_EMPTY; // This should be the last operation
        }

        inline void setWaiting()
        {
            nState      = M_WAIT; // This should be the last operation
        }
    } mesh_t;

    typedef struct resource_t
    {
        const char *id;
        const char *text;
    } resource_t;
}

#include <core/units.h>

#endif /* TYPES_H_ */
