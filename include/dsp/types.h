/*
 * types.h
 *
 *  Created on: 10 авг. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_TYPES_H_
#define DSP_TYPES_H_

#include <sys/types.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdint.h>

#ifdef __linux__
    #include <linux/limits.h>
#endif /* __linux__ */

//-----------------------------------------------------------------------------
// Detect build architecture
#if defined(__x86_64__)
    #define ARCH_X86_64
#elif defined(__i386__)
    #define ARCH_I386
#else
    // TODO
#endif

//-----------------------------------------------------------------------------
// Detect endianess and operations
#if defined(ARCH_I386) || defined(ARCH_X86_64)
    #define ARCH_X86
    #define ARCH_LE
#endif /* defined(ARCH_I386) || defined(ARCH_X86_64) */

#ifdef ARCH_LE
    #define __IF_LEBE(le, be)   (le)
    #define __IF_LE(le)         (le)
    #define __IF_BE(be)
    #ifdef ARCH_BE
        #undef ARCH_BE
    #endif /* ARCH_BE */
#else /* ARCH_BE */
    #define __IF_LEBE(le, be)   (be)
    #define __IF_LE(le)
    #define __IF_BE(be)         (be)

    #ifndef ARCH_LE
        #define ARCH_LE
    #endif /* ARCH_LE */
#endif /* ARCH_LE */

//-----------------------------------------------------------------------------
// Detect build platform
#if defined(__unix__) || defined(unix) || defined(__unix)
    #define PLATFORM_UNIX
#endif /* __unix__ */

#if defined(__linux__) || defined(__linux) || defined(linux)
    #define PLATFORM_LINUX
#endif /* __linux__ */

#if defined(__bsd__) || defined(__bsd) || defined(__FreeBSD__)
    #define PLATFORM_BSD
#endif /* __bsd__ */

#if defined(__macosx__) || defined(__APPLE__) || defined(__MACH__)
    #define PLATFORM_MACOSX
#endif /* __macosx__ */

#if defined(PLATFORM_UNIX) || defined(PLATFORM_LINUX) || defined(PLATFORM_MACOSX) || defined(PLATFORM_BSD)
    #define PLATFORM_UNIX_COMPATIBLE
    #define PLATFORM_POSIX
#endif /* unix-compatible platforms */

#if defined(__WINDOWS__) || defined(__WIN32__) || defined(__WIN64__) || defined(_WIN64) || defined(_WIN32) || defined(__WINNT) || defined(__WINNT__)
    #define PLATFORM_WINDOWS
#endif /* __macosx__ */

// File separators for platform tuning
#if defined(PLATFORM_UNIX) || defined(PLATFORM_LINUX) || defined(PLATFORM_MACOSX)
    #define FILE_SEPARATOR_C      '/'
    #define FILE_SEPARATOR_S      "/"
#elif defined(PLATFORM_WINDOWS)
    #define FILE_SEPARATOR_C      '\\'
    #define FILE_SEPARATOR_S      "\\"
#endif /* */

//-----------------------------------------------------------------------------
// Conditional assemblying
#define __ASM_EMIT(code)                        code "\n\t"

#ifdef ARCH_I386
    #define __ASM_EMIT32(code)                  code "\n\t"
    #define __IF_32(...)                        __VA_ARGS__

    #ifdef LSP_PROFILING
        #define __IF_32P(...)                       __VA_ARGS__
    #else
        #define __IF_32NP(...)                      __VA_ARGS__
    #endif /* LSP_PROFILING */
#endif /* __i386__ */

#ifdef ARCH_X86_64
    #define __ASM_EMIT64(code)                  code "\n\t"
    #define __IF_64(...)                        __VA_ARGS__
#endif

#ifdef LSP_PROFILING
    #define __ASM_EMITP(code)                      code "\n\t"
    #define __ASM_EMITNP(code)
#else
    #define __ASM_EMITP(code)
    #define __ASM_EMITNP(code)                     code "\n\t"
#endif /* LSP_PROFILING */

#define __ASM_ARG_TMP(var)                      __IF_32P("=&g"(var)) __IF_32NP("=&r"(var)) __IF_64("=&r"(var))
#define __ASM_ARG_RW(var)                       __IF_32P("+g"(var))  __IF_32NP("+r"(var))  __IF_64("+r"(var))
#define __ASM_ARG_RO(var)                       __IF_32P("g"(var))   __IF_32NP("r"(var))   __IF_64("r"(var))

#define __lsp_forced_inline                 __attribute__ ((always_inline))
#define __lsp_aligned16                     __attribute__ ((aligned (16)))
#define __lsp_aligned64                     __attribute__ ((aligned (64)))
#define __lsp_aligned(bytes)                __attribute__ ((aligned (bytes)))

//-----------------------------------------------------------------------------
// Define macros that may not be previously defined
#ifndef __ASM_EMIT32
    #define __ASM_EMIT32(code)
#endif /* __ASM_EMIT64 */

#ifndef __ASM_EMIT64
    #define __ASM_EMIT64(code)
#endif /* __ASM_EMIT64 */

#ifndef __IF_32
    #define __IF_32(...)
#endif /* __IF_32 */

#ifndef __IF_32P
    #define __IF_32P(...)
#endif /* __IF_32 */

#ifndef __IF_32NP
    #define __IF_32NP(...)
#endif /* __IF_32 */

#ifndef __IF_64
    #define __IF_64(...)
#endif /* __IF_64 */

//-----------------------------------------------------------------------------
// Optimizations
#ifdef ARCH_X86
    #define DEFAULT_ALIGN                   0x10
#endif /* ARCH_X86 */

#ifndef DEFAULT_ALIGN
    #define DEFAULT_ALIGN                   0x10
#endif /* DEFAULT_ALIGN */

//-----------------------------------------------------------------------------
// Type definitions
__IF_32( typedef        uint32_t            umword_t );
__IF_32( typedef        int32_t             smword_t );
__IF_64( typedef        uint64_t            umword_t );
__IF_64( typedef        int64_t             smword_t );

#endif /* DSP_TYPES_H_ */
