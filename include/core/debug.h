/*
 * debug.h
 *
 *  Created on: 14 окт. 2015 г.
 *      Author: sadko
 */

#ifndef CORE_DEBUG_H_
#define CORE_DEBUG_H_

#include <core/types.h>

// Check trace level
#ifdef LSP_TRACE
    #define lsp_trace(msg, ...)   fprintf(stderr, "[TRC][%s:%4d] %s: " msg "\n", __FILE__, __LINE__, __FUNCTION__, ## __VA_ARGS__)

    // Debug is always turned on when trace is turned on
    #ifndef LSP_DEBUG
        #define LSP_DEBUG
    #endif /* LSP_DEBUG */
#else
    #define lsp_trace(msg, ...)
#endif /* LSP_TRACE */

// Check debug level
#ifdef LSP_DEBUG
    #define lsp_debug(msg, ...)   fprintf(stderr, "[DBG][%s:%4d] %s: " msg "\n", __FILE__, __LINE__, __FUNCTION__, ## __VA_ARGS__)
#else
    #define lsp_debug(msg, ...)
#endif /* LSP_DEBUG */

// Include <stdio.h> to perform debugging output
#include <stdio.h>
#define lsp_error(msg, ...)     fprintf(stderr, "[ERR][%s:%4d] %s: " msg "\n", __FILE__, __LINE__, __FUNCTION__, ## __VA_ARGS__)
#define lsp_warn(msg, ...)      fprintf(stderr, "[WRN][%s:%4d] %s: " msg "\n", __FILE__, __LINE__, __FUNCTION__, ## __VA_ARGS__)
#define lsp_info(msg, ...)      fprintf(stderr, "[INF][%s:%4d] %s: " msg "\n", __FILE__, __LINE__, __FUNCTION__, ## __VA_ARGS__)

// Define assertions
#ifdef LSP_DEBUG
    #define lsp_paranoia(...)   { __VA_ARGS__ }

    #define lsp_assert(x)       if (!(x)) lsp_error("assertion failed: %s", #x);
#else
    #define lsp_paranoia(...)

    #define lsp_assert(x)
#endif /* ASSERTIONS */

#endif /* CORE_DEBUG_H_ */
