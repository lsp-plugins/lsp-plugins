/*
 * types.h
 *
 *  Created on: 10 авг. 2018 г.
 *      Author: sadko
 */

#ifndef COMMON_TYPES_H_
#define COMMON_TYPES_H_

#include <sys/types.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>

//-----------------------------------------------------------------------------
// Detect build architecture
#if defined(__x86_64__) || defined(__x86_64) || defined(__amd64__) || defined(__amd64) || defined(_M_AMD64)
    #define ARCH_X86_64
#elif defined(__i386__) || defined(__i386)
    #define ARCH_I386
#elif defined(__aarch64__)
    #define ARCH_AARCH64
#elif defined(__arm__) || defined(__arm) || defined(_M_ARM) || defined(_ARM)
    #define ARCH_ARM
#elif defined(__PPC64__) || defined(__ppc64__) || defined(__ppc64) || defined(__powerpc64__) || defined(_ARCH_PPC64)
    #define ARCH_PPC64
#elif defined(__PPC__) || defined(__ppc__) || defined(__powerpc__) || defined(__ppc) || defined(_M_PPC) || defined(_ARCH_PPC)
    #define ARCH_PPC
#elif defined(__s390x__) || defined(__s390__) || defined(__zarch__)
    #define ARCH_S390
#elif defined(__mips__) || defined(__mips) || defined(__MIPS__)
    #define ARCH_MIPS
#elif defined(__sparc__) || defined(__sparc)
    #define ARCH_SPARC
#endif

//-----------------------------------------------------------------------------
// Detect endianess of architecture
#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
    #define ARCH_LE
#elif (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
    #define ARCH_BE
#endif

//-----------------------------------------------------------------------------
// Detect bitness of architecture
#if defined(__WORDSIZE) && (__WORDSIZE == 64)
    #define ARCH_64BIT
#elif defined(__SIZE_WIDTH__) && (__SIZE_WIDTH__ == 64)
    #define ARCH_64BIT
#elif defined(__WORDSIZE) && (__WORDSIZE == 32)
    #define ARCH_32BIT
#elif defined(__SIZE_WIDTH__) && (__SIZE_WIDTH__ == 32)
    #define ARCH_32BIT
#else
    #warning "Unsupported architecture bitness"
#endif /* __WORDSIZE, __SIZE_WIDTH__ */

//-----------------------------------------------------------------------------
// Detect endianess and operations
#if defined(ARCH_I386) || defined(ARCH_X86_64)
    #define IF_ARCH_X86(...)        __VA_ARGS__
    #define ARCH_X86_ASM(...)       __asm__ __volatile__ ( __VA_ARGS__ )

    #ifdef ARCH_I386
        #define ARCH_I386_ASM(...)       __asm__ __volatile__ ( __VA_ARGS__ )
    #endif /* ARCH_I386 */

    #ifdef ARCH_X86_64
        #define ARCH_X86_64_ASM(...)       __asm__ __volatile__ ( __VA_ARGS__ )
    #endif /* ARCH_I386 */

    #define ARCH_X86

    #if defined(ARCH_I386)
        #define ARCH_STRING             "i386"
        #define IF_ARCH_I386(...)       __VA_ARGS__
    #else
        #define ARCH_STRING             "x86_64"
        #define IF_ARCH_X86_64(...)     __VA_ARGS__
    #endif
#endif /* defined(ARCH_I386) || defined(ARCH_X86_64) */

#if defined(ARCH_ARM)
    #define IF_ARCH_ARM(...)            __VA_ARGS__
    #define ARCH_ARM_ASM(...)           __asm__ __volatile__ ( __VA_ARGS__ )

    #if !defined(ARCH_BE) && !defined(ARCH_BE)
        #define ARCH_LE
    #endif

    #if (__ARM_ARCH == 7)
        #define ARCH_ARM7
        #define ARCH_STRING             "armv7a"
        #define IF_ARCH_ARM7(...)        __VA_ARGS__
        #define IF_ARCH_LEAST_ARM7(...)  __VA_ARGS__
    #elif (__ARM_ARCH == 6)
        #define ARCH_ARM6
        #define ARCH_STRING             "armv6a"
        #define IF_ARCH_ARM6(...)        __VA_ARGS__
        #define IF_ARCH_LEAST_ARM6(...)  __VA_ARGS__
    #else
        #define ARCH_STRING             "arm-generic"
    #endif
#endif /* defined(ARCH_ARM) */

#if defined(ARCH_AARCH64)
    #define IF_ARCH_AARCH64(...)        __VA_ARGS__
    #define ARCH_AARCH64_ASM(...)       __asm__ __volatile__ ( __VA_ARGS__ )

    #if !defined(ARCH_BE) && !defined(ARCH_BE)
        #define ARCH_LE
    #endif

    #define ARCH_STRING                 "aarch64"

    #if (__ARM_ARCH == 8)
        #define ARCH_ARM8
        #define IF_ARCH_ARM8(...)        __VA_ARGS__
    #endif
#endif /* defined(ARCH_ARM) */

#if defined(ARCH_PPC)
    #define IF_ARCH_PPC(...)            __VA_ARGS__
    #define ARCH_PPC_ASM(...)           __asm__ __volatile__ ( __VA_ARGS__ )

    #define ARCH_STRING                 "ppc"
#endif /* defined(ARCH_PPC) */

#if defined(ARCH_PPC64)
    #define IF_ARCH_PPC64(...)          __VA_ARGS__
    #define ARCH_PPC64_ASM(...)         __asm__ __volatile__ ( __VA_ARGS__ )

    #define ARCH_STRING                 "ppc64"
#endif /* defined(ARCH_PPC) */

#if defined(ARCH_S390)
    #define IF_ARCH_S390(...)           __VA_ARGS__
    #define ARCH_S390_ASM(...)          __asm__ __volatile__ ( __VA_ARGS__ )

    #define ARCH_STRING                 "S390"
#endif /* defined(ARCH_PPC) */

#if defined(ARCH_MIPS)
    #define IF_ARCH_MIPS(...)           __VA_ARGS__
    #define ARCH_MIPS_ASM(...)          __asm__ __volatile__ ( __VA_ARGS__ )

    #define ARCH_STRING                 "MIPS"
#endif /* defined(ARCH_PPC) */

#if defined(ARCH_SPARC)
    #define IF_ARCH_SPARC(...)          __VA_ARGS__
    #define ARCH_SPARC_ASM(...)         __asm__ __volatile__ ( __VA_ARGS__ )

    #define ARCH_STRING                 "SPARC"
#endif /* defined(ARCH_PPC) */

#if defined(ARCH_LE)
    #define __IF_LEBE(le, be)   le
    #define __IF_LE(le)         le
    #define __IF_BE(be)
    #ifdef ARCH_BE
        #undef ARCH_BE
    #endif /* ARCH_BE */
#elif defined(ARCH_BE) /* ARCH_BE */
    #define __IF_LEBE(le, be)   be
    #define __IF_LE(le)
    #define __IF_BE(be)         be

    #ifdef ARCH_LE
        #undef ARCH_LE
    #endif /* ARCH_LE */
#else
    #warning "Could not detect endianess of the target architecture"
#endif /* ARCH_LE */

#ifndef ARCH_STRING
    #define ARCH_STRING                 "native"
#endif /* ARCH_STRING */

//-----------------------------------------------------------------------------
// Detect build platform
#if defined(__unix__) || defined(unix) || defined(__unix)
    #define PLATFORM_UNIX
    #define IF_PLATFORM_UNIX(...)       __VA_ARGS__
#endif /* __unix__ */

#if defined(__sun__) || defined(__sun) || defined(sun)
    #define PLATFORM_SOLARIS
    #define IF_PLATFORM_SOLARIS(...)    __VA_ARGS__
#endif /* __sun__ */

#if defined(__linux__) || defined(__linux) || defined(linux)
    #define PLATFORM_LINUX
    #define IF_PLATFORM_LINUX(...)      __VA_ARGS__
#endif /* __linux__ */

#if defined(__bsd__) || defined(__bsd) || defined(__FreeBSD__) || defined(freebsd) || defined(openbsd) || defined(bsdi) || defined(__darwin__)
    #define PLATFORM_BSD
    #define IF_PLATFORM_BSD(...)        __VA_ARGS__
#endif /* __bsd__ */

#if defined(__macosx__) || defined(__APPLE__) || defined(__MACH__)
    #define PLATFORM_MACOSX
    #define IF_PLATFORM_MACOSX(...)     __VA_ARGS__
#endif /* __macosx__ */

#if defined(PLATFORM_UNIX) || defined(PLATFORM_LINUX) || defined(PLATFORM_MACOSX) || defined(PLATFORM_BSD)
    #define PLATFORM_UNIX_COMPATIBLE
    #define PLATFORM_POSIX

    #define IF_PLATFORM_POSIX(...)      __VA_ARGS__
#endif /* unix-compatible platforms */

#if defined(__WINDOWS__) || defined(__WIN32__) || defined(__WIN64__) || defined(_WIN64) || defined(_WIN32) || defined(__WINNT) || defined(__WINNT__)
    #define PLATFORM_WINDOWS
    #define IF_PLATFORM_WINDOWS(...)    __VA_ARGS__
#endif /* __macosx__ */

// File separators for platform tuning
#if defined(PLATFORM_UNIX) || defined(PLATFORM_LINUX) || defined(PLATFORM_MACOSX)
    #define FILE_SEPARATOR_C        '/'
    #define FILE_SEPARATOR_S        "/"
#elif defined(PLATFORM_WINDOWS)
    #define FILE_SEPARATOR_C      '\\'
    #define FILE_SEPARATOR_S      "\\"
#endif /* */

//-----------------------------------------------------------------------------
// Conditional assemblying
#define __ASM_EMIT(code)                        code "\n\t"

#ifdef ARCH_32BIT
    #define __ASM_EMIT32(code)                  code "\n\t"
    #define __IF_32(...)                        __VA_ARGS__
    #define __IF_32_64(a, b)                    a

    #ifdef LSP_PROFILING
        #define __IF_32P(...)                       __VA_ARGS__
    #else
        #define __IF_32NP(...)                      __VA_ARGS__
    #endif /* LSP_PROFILING */
#endif /* ARCH_32BIT */

#ifdef ARCH_64BIT
    #define __ASM_EMIT64(code)                  code "\n\t"
    #define __IF_64(...)                        __VA_ARGS__
    #define __IF_32_64(a, b)                    b
#endif /* ARCH_32BIT */

#ifdef LSP_PROFILING
    #define __ASM_EMITP(code)                      code "\n\t"
    #define __ASM_EMITNP(code)
#else
    #define __ASM_EMITP(code)
    #define __ASM_EMITNP(code)                     code "\n\t"
#endif /* LSP_PROFILING */

#ifndef ARCH_X86_ASM
    #define ARCH_X86_ASM(...)
#endif /* ARCH_X86_ASM */

#ifndef ARCH_I386_ASM
    #define ARCH_I386_ASM(...)
#endif /* ARCH_I386_ASM */

#ifndef ARCH_X86_64_ASM
    #define ARCH_X86_64_ASM(...)
#endif /* ARCH_I386_ASM */

#ifndef ARCH_ARM_ASM
    #define ARCH_ARM_ASM(...)
#endif /* ARCH_ARM_ASM */

#ifndef ARCH_AARCH64_ASM
    #define ARCH_AARCH64_ASM(...)
#endif /* ARCH_AARCH64_ASM */

#ifndef ARCH_PPC64_ASM
    #define ARCH_PPC64_ASM(...)
#endif /* ARCH_PPC64_ASM */

#ifndef ARCH_PPC_ASM
    #define ARCH_PPC_ASM(...)
#endif /* ARCH_PPC_ASM */

#ifndef ARCH_S390_ASM
    #define ARCH_S390_ASM(...)
#endif /* ARCH_S390_ASM */

#ifndef ARCH_MIPS_ASM
    #define ARCH_MIPS_ASM(...)
#endif /* ARCH_MIPS_ASM */

#ifndef ARCH_SPARC_ASM
    #define ARCH_SPARC_ASM(...)
#endif /* ARCH_SPARC_ASM */

#define __ASM_ARG_TMP(var)                      __IF_32P("=&g"(var)) __IF_32NP("=&r"(var)) __IF_64("=&r"(var))
#define __ASM_ARG_RW(var)                       __IF_32P("+g"(var))  __IF_32NP("+r"(var))  __IF_64("+r"(var))
#define __ASM_ARG_RO(var)                       __IF_32P("g"(var))   __IF_32NP("r"(var))   __IF_64("r"(var))

#define __lsp_forced_inline                 __attribute__ ((always_inline))
#define __lsp_aligned16                     __attribute__ ((aligned (16)))
#define __lsp_aligned32                     __attribute__ ((aligned (32)))
#define __lsp_aligned64                     __attribute__ ((aligned (64)))
#define __lsp_aligned(bytes)                __attribute__ ((aligned (bytes)))

//-----------------------------------------------------------------------------
// CPU extensions

// Extensions for x86 architecture enabled by compiler
#ifdef ARCH_X86
    #ifdef __SSE__
        #define ARCH_X86_SSE
    #endif /* __SSE__ */

    #ifdef __SSE2__
        #define ARCH_X86_SSE2
    #endif /* __SSE2__ */

    #ifdef __SSE3__
        #define ARCH_X86_SSE3
    #endif /* __SSE2__ */

    #ifdef __SSSE3__
        #define ARCH_X86_SSSE3
    #endif /* __SSE2__ */

    #ifdef __SSE4_1__
        #define ARCH_X86_SSE4_1
    #endif /* __SSE4_1__ */

    #ifdef __SSE4_2__
        #define ARCH_X86_SSE4_2
    #endif /* __SSE4_1__ */

    #ifndef LSP_NO_AVX /* Special directive that forces to disable AVX support */
        #ifdef __AVX__
            #define ARCH_X86_AVX
            #ifdef ARCH_X86_64
                #define ARCH_X86_64_AVX
            #else
                #define ARCH_I386_AVX
            #endif
        #endif /* __AVX__ */

        #ifdef __AVX2__
            #define ARCH_X86_AVX2
            #ifdef ARCH_X86_64
                #define ARCH_X86_64_AVX2
            #else
                #define ARCH_I386_AVX2
            #endif
        #endif /* __AVX2__ */
    #endif /* LSP_NO_AVX */
#endif /* ARCH_X86 */


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
// Default architectures
#ifndef IF_ARCH_X86
    #define IF_ARCH_X86(...)
#endif /* IF_ARCH_X86 */

#ifndef IF_ARCH_I386
    #define IF_ARCH_I386(...)
#endif /* IF_ARCH_I386 */

#ifndef IF_ARCH_X86_64
    #define IF_ARCH_X86_64(...)
#endif /* IF_ARCH_X86_64 */

#ifndef IF_ARCH_ARM
    #define IF_ARCH_ARM(...)
#endif /* IF_ARCH_ARM */

#ifndef IF_ARCH_ARM6
    #define IF_ARCH_ARM6(...)
#endif /* IF_ARCH_ARM6 */

#ifndef IF_ARCH_LEAST_ARM6
    #define IF_ARCH_LEAST_ARM6(...)
#endif /* IF_ARCH_LEAST_ARM6 */

#ifndef IF_ARCH_ARM7
    #define IF_ARCH_ARM7(...)
#endif /* IF_ARCH_ARM7 */

#ifndef IF_ARCH_LEAST_ARM7
    #define IF_ARCH_LEAST_ARM7(...)
#endif /* IF_ARCH_LEAST_ARM7 */

#ifndef IF_ARCH_ARM8
    #define IF_ARCH_ARM8(...)
#endif /* IF_ARCH_ARM8 */

#ifndef IF_ARCH_AARCH64
    #define IF_ARCH_AARCH64(...)
#endif /* IF_ARCH_AARCH64 */

#ifndef IF_ARCH_PPC
    #define IF_ARCH_PPC(...)
#endif /* IF_ARCH_PPC */

#ifndef IF_ARCH_PPC64
    #define IF_ARCH_PPC64(...)
#endif /* IF_ARCH_PPC64 */

#ifndef IF_ARCH_S390
    #define IF_ARCH_S390(...)
#endif /* IF_ARCH_S390 */

#ifndef IF_ARCH_MIPS
    #define IF_ARCH_MIPS(...)
#endif /* IF_ARCH_MIPS */

#ifndef IF_ARCH_SPARC
    #define IF_ARCH_SPARC(...)
#endif /* IF_ARCH_MIPS */

//-----------------------------------------------------------------------------
// Default platform
#ifndef IF_PLATFORM_UNIX
    #define IF_PLATFORM_UNIX(...)
#endif /* IF_PLATFORM_UNIX */

#ifndef IF_PLATFORM_SOLARIS
    #define IF_PLATFORM_SOLARIS(...)
#endif /* IF_PLATFORM_SOLARIS */

#ifndef IF_PLATFORM_LINUX
    #define IF_PLATFORM_LINUX(...)
#endif /* IF_PLATFORM_LINUX */

#ifndef IF_PLATFORM_BSD
    #define IF_PLATFORM_BSD(...)
#endif /* IF_PLATFORM_BSD */

#ifndef IF_PLATFORM_MACOSX
    #define IF_PLATFORM_MACOSX(...)
#endif /* IF_PLATFORM_MACOSX */

#ifndef IF_PLATFORM_POSIX
    #define IF_PLATFORM_POSIX(...)
#endif /* IF_PLATFORM_POSIX */

#ifndef IF_PLATFORM_WINDOWS
    #define IF_PLATFORM_WINDOWS(...)
#endif /* IF_PLATFORM_WINDOWS */

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

#ifdef PLATFORM_LINUX
    #include <linux/limits.h>
#endif /* __linux__ */

//-----------------------------------------------------------------------------
// Character type sizes
#if (WCHAR_MAX >= 0x10000ul)
    #define WCHART_32BIT
#else
    #define WCHART_16BIT
#endif /* WCHAR_MAX */

#endif /* COMMON_TYPES_H_ */
