/*
 * debug.h
 *
 *  Created on: 14 окт. 2015 г.
 *      Author: sadko
 */

#ifndef CORE_DEBUG_H_
#define CORE_DEBUG_H_

// Include <stdio.h> to perform debugging output
#include <stdio.h>
#include <stdarg.h>

#ifdef LSP_LOG_FD
    #undef LSP_LOG_FD
#endif /* LSP_LOG_FD */

#ifdef LSP_TRACEFILE
    #define LSP_LOG_FD              ::lsp::log_fd
#else
    #define LSP_LOG_FD              stderr
#endif /* LSP_TRACEFILE */

// Check trace level
#ifdef LSP_TRACE
    #define lsp_trace(msg, ...)     { fprintf(LSP_LOG_FD, "[TRC][%s:%4d] %s: " msg "\n", __FILE__, __LINE__, __FUNCTION__, ## __VA_ARGS__); fflush(LSP_LOG_FD); }

    // Debug is always turned on when trace is turned on
    #ifndef LSP_DEBUG
        #define LSP_DEBUG
    #endif /* LSP_DEBUG */
#else
    #define lsp_trace(msg, ...)
#endif /* LSP_TRACE */

// Check debug level
#ifdef LSP_DEBUG
    #define lsp_nprintf(msg, ...)       fprintf(LSP_LOG_FD, msg, ## __VA_ARGS__)
    #define lsp_nvprintf(msg, va_args)  vfprintf(LSP_LOG_FD, msg, va_args)
    #define lsp_printf(msg, ...)        { fprintf(LSP_LOG_FD, msg "\n", ## __VA_ARGS__); fflush(LSP_LOG_FD); }
    #define lsp_debug(msg, ...)         { fprintf(LSP_LOG_FD, "[DBG][%s:%4d] %s: " msg "\n", __FILE__, __LINE__, __FUNCTION__, ## __VA_ARGS__); fflush(LSP_LOG_FD); }
    #define lsp_dumpf(s, fmt, p, n)     ::lsp::__lsp_dumpf(s, fmt, p, n)
    #define lsp_dumpb(s, p, sz)         ::lsp::__lsp_dumpb(s, p, sz)
#else
    #define lsp_nprintf(msg, ...)
    #define lsp_nvprintf(msg, va_args)
    #define lsp_printf(msg, ...)
    #define lsp_debug(msg, ...)
    #define lsp_dumpf(s, fmt, p, n)
    #define lsp_dumpb(s, p, sz)
#endif /* LSP_DEBUG */

#ifdef LSP_DEBUG
    #define lsp_error(msg, ...)     { fprintf(LSP_LOG_FD, "[ERR][%s:%4d] %s: " msg "\n", __FILE__, __LINE__, __FUNCTION__, ## __VA_ARGS__); fflush(LSP_LOG_FD); }
    #define lsp_warn(msg, ...)      { fprintf(LSP_LOG_FD, "[WRN][%s:%4d] %s: " msg "\n", __FILE__, __LINE__, __FUNCTION__, ## __VA_ARGS__); fflush(LSP_LOG_FD); }
    #define lsp_info(msg, ...)      { fprintf(LSP_LOG_FD, "[INF][%s:%4d] %s: " msg "\n", __FILE__, __LINE__, __FUNCTION__, ## __VA_ARGS__); fflush(LSP_LOG_FD); }
#else
    #define lsp_error(msg, ...)     { fprintf(LSP_LOG_FD, "[ERR] " msg "\n", ## __VA_ARGS__); fflush(LSP_LOG_FD); }
    #define lsp_warn(msg, ...)      { fprintf(LSP_LOG_FD, "[WRN] " msg "\n", ## __VA_ARGS__); fflush(LSP_LOG_FD); }
    #define lsp_info(msg, ...)      { fprintf(LSP_LOG_FD, "[INF] " msg "\n", ## __VA_ARGS__); fflush(LSP_LOG_FD); }
#endif /* LSP_DEBUG */

// Define assertions
#ifdef LSP_DEBUG
    #define lsp_paranoia(...)   { __VA_ARGS__; }

    #define lsp_guard_assert(...) __VA_ARGS__;
    #define lsp_assert(x)           if (!(x)) { lsp_error("Assertion failed: %s", #x); fflush(LSP_LOG_FD); }
    #define lsp_assert_msg(x, msg, ...)  \
            if (!(x)) { \
                fprintf(LSP_LOG_FD, "[ERR][%s:%4d] %s: Assertion failed: %s, nested message: " msg "\n", \
                    __FILE__, __LINE__, __FUNCTION__, #x, ## __VA_ARGS__); \
                fflush(LSP_LOG_FD); \
            }

#else
    #define lsp_paranoia(...)

    #define lsp_guard_assert(...)
    #define lsp_assert(x)
    #define lsp_assert_msg(x, ...)
#endif /* ASSERTIONS */

namespace lsp
{
// Define initialization function
#ifdef LSP_TRACEFILE
    #define lsp_debug_init(subsystem)        lsp::init_debug(subsystem)

    #ifdef LSP_TRACEFILE
        extern FILE *log_fd;
    #endif /* LSP_TRACEFILE */

    void init_debug(const char *subsystem);
#else
    #define lsp_debug_init(subsystem)
#endif /* LSP_DEBUG */

#ifdef LSP_DEBUG
    void __lsp_dumpf(const char *s, const char *fmt, const float *f, size_t n);
    void __lsp_dumpb(const char *s, const void *b, size_t sz);
#endif /* LSP_DEBUG */
}

#endif /* CORE_DEBUG_H_ */
