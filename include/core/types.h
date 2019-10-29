/*
 * types.h
 *
 *  Created on: 26 окт. 2015 г.
 *      Author: sadko
 */

#ifndef CORE_TYPES_H_
#define CORE_TYPES_H_

#include <common/types.h>

#if defined(PLATFORM_WINDOWS)
    #include <windows.h>
#endif /* PLATFORM_WINDOWS */

// For IDEs: define this symbol in IDE to properly compile and debug
#ifdef LSP_IDE_DEBUG
    #ifdef PLATFORM_WINDOWS
        #define LSP_USE_MSXML
    #else
        #define LSP_USE_EXPAT
    #endif /* PLATFORM */
    //#define LSP_HOST_SIMULATION
#else
    #define LSP_BUILTIN_RESOURCES
#endif /* LSP_IDE_DEBUG */

#ifdef __cplusplus
    #define LSP_LIBRARY_EXTERN  extern "C"
#else
    #define LSP_LIBRARY_EXTERN
#endif /* __cplusplus */

#ifdef PLATFORM_WINDOWS
    #define LSP_LIBRARY_IMPORT  __declspec(dllexport)
    #define LSP_LIBRARY_EXPORT  LSP_LIBRARY_EXTERN __declspec(dllexport)
#else
    #define LSP_LIBRARY_IMPORT
    #define LSP_LIBRARY_EXPORT  LSP_LIBRARY_EXTERN __attribute__ ((visibility ("default")))
#endif

typedef uint64_t        wsize_t;
typedef int64_t         wssize_t;

/** Unicode character definition
 *
 */
typedef uint32_t        lsp_wchar_t;
typedef int32_t         lsp_swchar_t;

#if defined(WCHART_16BIT)
    typedef WCHAR               lsp_utf16_t;
    typedef uint32_t            lsp_utf32_t;
#else
    typedef uint16_t            lsp_utf16_t;
    typedef wchar_t             lsp_utf32_t;
#endif

#if defined(PLATFORM_WINDOWS)
    typedef HANDLE              lsp_fhandle_t;
#else
    typedef int                 lsp_fhandle_t;
#endif /* PLATFORM_WINDOWS */

// Include units
#include <core/sugar.h>
#include <core/units.h>
#include <core/characters.h>
#include <core/assert.h>
#include <core/status.h>

namespace lsp
{
    enum lsp_wrap_flags_t
    {
        WRAP_NONE       = 0,

        WRAP_CLOSE      = 1 << 0,
        WRAP_DELETE     = 1 << 1
    };

    enum lsp_memdrop_t
    {
        MEMDROP_NONE,
        MEMDROP_FREE,
        MEMDROP_DELETE,
        MEMDROP_ARR_DELETE
    };

    typedef struct float_buffer_t
    {
        size_t      lines;
        size_t      items;
        float      *v[];

        static float_buffer_t  *create(size_t lines, size_t items);
        static float_buffer_t  *reuse(float_buffer_t *buf, size_t lines, size_t items);
        void                    detroy();
        float_buffer_t         *resize(size_t lines, size_t items);
    } float_buffer_t;
}

#endif /* CORE_TYPES_H_ */
