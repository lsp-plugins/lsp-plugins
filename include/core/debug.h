/*
 * debug.h
 *
 *  Created on: 14 окт. 2015 г.
 *      Author: sadko
 */

#ifndef CORE_DEBUG_H_
#define CORE_DEBUG_H_

// Check trace level
#ifdef LSP_TRACE
    #define lsp_trace(msg, ...)   printf("[%s:%4d] %s: " msg "\n", __FILE__, __LINE__, __FUNCTION__, ## __VA_ARGS__)

    // Debug is always turned on when trace is turned on
    #ifndef LSP_DEBUG
        #define LSP_DEBUG
    #endif /* LSP_DEBUG */
#else
    #define lsp_trace(msg, ...)
#endif /* LSP_TRACE */

// Check debug level
#ifdef LSP_DEBUG
    #define lsp_debug(msg, ...)   printf("[%s:%4d] %s: " msg "\n", __FILE__, __LINE__, __FUNCTION__, ## __VA_ARGS__)
#else
    #define lsp_debug(msg, ...)
#endif /* LSP_DEBUG */

// Include <stdio.h> to perform debugging output
#include <stdio.h>
#define lsp_error(msg, ...)     fprintf(stderr, "[%s:%4d] %s: " msg "\n", __FILE__, __LINE__, __FUNCTION__, ## __VA_ARGS__)
#define lsp_info(msg, ...)      printf("[%s:%4d] %s: " msg "\n", __FILE__, __LINE__, __FUNCTION__, ## __VA_ARGS__)

#endif /* CORE_DEBUG_H_ */
