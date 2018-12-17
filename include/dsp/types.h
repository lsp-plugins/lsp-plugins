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

/*
    ARM-predefined macros on Raspberry Pi

    #define __ARM_SIZEOF_WCHAR_T 4
    #define __ARM_FEATURE_SAT 1
    #define __ARM_ARCH_ISA_ARM 1
    #define __ARMEL__ 1
    #define __ARM_FEATURE_UNALIGNED 1
    #define __ARM_FEATURE_IDIV 1
    #define __ARM_FP 12
    #define __ARM_ARCH_8A__ 1
    #define __ARM_SIZEOF_MINIMAL_ENUM 4
    #define __ARM_PCS_VFP 1
    #define __ARM_FEATURE_LDREX 15
    #define __ARM_FEATURE_QBIT 1
    #define __ARM_ARCH_PROFILE 65
    #define __ARM_32BIT_STATE 1
    #define __ARM_FEATURE_CLZ 1
    #define __ARM_ARCH_ISA_THUMB 2
    #define __ARM_ARCH 8
    #define __ARM_FEATURE_SIMD32 1
    #define __ARM_FEATURE_CRC32 1
    #define __ARM_ARCH_EXT_IDIV__ 1
    #define __ARM_EABI__ 1
    #define __ARM_FEATURE_DSP 1
 */

//-----------------------------------------------------------------------------
// Detect build architecture
#if defined(__x86_64__)
    #define ARCH_X86_64
    #define ARCH_STRING "x86_64"
    #define IF_ARCH_X86_64(...)     __VA_ARGS__
#elif defined(__i386__)
    #define ARCH_I386
    #define ARCH_STRING "i586"
    #define IF_ARCH_I386(...)       __VA_ARGS__
#elif defined(__arm__)
    #define ARCH_ARM
    #define IF_ARCH_ARM(...)        __VA_ARGS__
#else
    #warning "Unsupported archtecture"
#endif

//-----------------------------------------------------------------------------
// Detect endianess and operations
#if defined(ARCH_I386) || defined(ARCH_X86_64)
    #if defined(ARCH_X86_64)
        #define ARCH_64BIT
    #else
        #define ARCH_32BIT
    #endif

    #define IF_ARCH_X86(...)        __VA_ARGS__
    #define ARCH_X86_ASM(...)       __asm__ __volatile__ ( __VA_ARGS__ )

    #ifdef ARCH_I386
        #define ARCH_I386_ASM(...)       __asm__ __volatile__ ( __VA_ARGS__ )
    #endif /* ARCH_I386 */

    #ifdef ARCH_X86_64
        #define ARCH_X86_64_ASM(...)       __asm__ __volatile__ ( __VA_ARGS__ )
    #endif /* ARCH_I386 */

    #define ARCH_X86
    #define ARCH_LE
#endif /* defined(ARCH_I386) || defined(ARCH_X86_64) */

#if defined(ARCH_ARM)
    #define ARCH_LE
    #define ARCH_ARM_ASM(...)       __asm__ __volatile__ ( __VA_ARGS__ )

    #if (__ARM_ARCH == 8)
        #define ARCH_64BIT
        #define ARCH_ARM8
        #define ARCH_STRING "armv8a"
        #define IF_ARCH_ARM8(...)        __VA_ARGS__
    #elif (__ARM_ARCH == 7)
        #define ARCH_32BIT
        #define ARCH_ARM7
        #define ARCH_STRING "armv7a"
        #define IF_ARCH_ARM7(...)        __VA_ARGS__
    #elif (__ARM_ARCH == 6)
        #define ARCH_32BIT
        #define ARCH_ARM6
        #define ARCH_STRING "armv6a"
        #define IF_ARCH_ARM6(...)        __VA_ARGS__
    #else
        #define ARCH_32BIT
        #define ARCH_STRING "arm-generic"
    #endif
#endif /* defined(ARCH_ARM) */

#ifdef ARCH_LE
    #define __IF_LEBE(le, be)   le
    #define __IF_LE(le)         le
    #define __IF_BE(be)
    #ifdef ARCH_BE
        #undef ARCH_BE
    #endif /* ARCH_BE */
#else /* ARCH_BE */
    #define __IF_LEBE(le, be)   be
    #define __IF_LE(le)
    #define __IF_BE(be)         be

    #ifndef ARCH_LE
        #define ARCH_LE
    #endif /* ARCH_LE */
#endif /* ARCH_LE */

#ifndef ARCH_STRING
    #define ARCH_STRING     "native"
#endif /* ARCH_STRING */

//-----------------------------------------------------------------------------
// Detect build platform
#if defined(__unix__) || defined(unix) || defined(__unix)
    #define PLATFORM_UNIX
#endif /* __unix__ */

#if defined(__linux__) || defined(__linux) || defined(linux)
    #define PLATFORM_LINUX
#endif /* __linux__ */

#if defined(__bsd__) || defined(__bsd) || defined(__FreeBSD__) || defined(freebsd) || defined(openbsd) || defined(bsdi) || defined(__darwin__)
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

#ifndef IF_ARCH_ARM7
    #define IF_ARCH_ARM7(...)
#endif /* IF_ARCH_ARM7 */

#ifndef IF_ARCH_ARM8
    #define IF_ARCH_ARM8(...)
#endif /* IF_ARCH_ARM8 */

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

#include <limits.h>

#ifdef PLATFORM_LINUX
    #include <linux/limits.h>
#endif /* __linux__ */

#endif /* DSP_TYPES_H_ */
