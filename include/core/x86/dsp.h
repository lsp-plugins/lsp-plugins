/*
 * dsp.h
 *
 *  Created on: 05 окт. 2015 г.
 *      Author: sadko
 */

#ifndef CORE_X86_DSP_H_
#define CORE_X86_DSP_H_

#include <core/dsp.h>
#include <core/x86/dsp/const.h>

namespace lsp
{
    namespace sse
    {
        static void move(float *dst, const float *src, size_t count);

        static void copy_forward(float *dst, const float *src, size_t count)
        {
            __asm__ __volatile__
            (
                __ASM_EMIT("1:")

                // Check conditions
                __ASM_EMIT("test $0x0f, %1")
                __ASM_EMIT("jz 2f")

                // Copy data
                __ASM_EMIT("movss (%0), %%xmm0")
                __ASM_EMIT("movss %%xmm0, (%1)")

                // Move pointers
                __ASM_EMIT("add $0x4, %0")
                __ASM_EMIT("add $0x4, %1")

                // Repeat loop
                __ASM_EMIT("dec %2")
                __ASM_EMIT("jnz 1b")
                __ASM_EMIT("2:")

                : "+r" (src), "+r"(dst), "+r" (count) :
                : "cc", "memory",
                  "%xmm0"
            );

            // Calculate amount of registers
            size_t regs     = count / SSE_MULTIPLE;
            count          %= SSE_MULTIPLE;

            if (regs > 0)
            {
                // Calculate amount of 8-register blocks (0x80 bytes)
                size_t blocks   = regs / 8;
                regs           %= 8;

                // Prefetch data for iteration
                __asm__ __volatile__
                (
                    __ASM_EMIT("prefetchnta 0x00(%0)")
                    __ASM_EMIT("prefetchnta 0x20(%0)")
                    __ASM_EMIT("prefetchnta 0x40(%0)")
                    __ASM_EMIT("prefetchnta 0x60(%0)")
                    : : "r" (src)
                );

                if (sse_aligned(src))
                {
                    if (blocks > 0)
                    {
                        __asm__ __volatile__
                        (
                            __ASM_EMIT("1:")
                            // Prefetch next data
                            __ASM_EMIT("prefetchnta 0x80(%0)")
                            __ASM_EMIT("prefetchnta 0xa0(%0)")
                            __ASM_EMIT("prefetchnta 0xc0(%0)")
                            __ASM_EMIT("prefetchnta 0xe0(%0)")

                            // calculate
                            __ASM_EMIT("movaps 0x00(%0), %%xmm0")
                            __ASM_EMIT("movaps 0x10(%0), %%xmm1")
                            __ASM_EMIT("movaps 0x20(%0), %%xmm2")
                            __ASM_EMIT("movaps 0x30(%0), %%xmm3")
                            __ASM_EMIT("movaps 0x40(%0), %%xmm4")
                            __ASM_EMIT("movaps 0x50(%0), %%xmm5")
                            __ASM_EMIT("movaps 0x60(%0), %%xmm6")
                            __ASM_EMIT("movaps 0x70(%0), %%xmm7")

                            __ASM_EMIT(MOVNTPS " %%xmm0,  0x00(%1)")
                            __ASM_EMIT(MOVNTPS " %%xmm1,  0x10(%1)")
                            __ASM_EMIT(MOVNTPS " %%xmm2,  0x20(%1)")
                            __ASM_EMIT(MOVNTPS " %%xmm3,  0x30(%1)")
                            __ASM_EMIT(MOVNTPS " %%xmm4,  0x40(%1)")
                            __ASM_EMIT(MOVNTPS " %%xmm5,  0x50(%1)")
                            __ASM_EMIT(MOVNTPS " %%xmm6,  0x60(%1)")
                            __ASM_EMIT(MOVNTPS " %%xmm7,  0x70(%1)")

                            // Move pointers
                            __ASM_EMIT("add $0x80, %0")
                            __ASM_EMIT("add $0x80, %1")

                            // Repeat loop
                            __ASM_EMIT("dec %2")
                            __ASM_EMIT("jnz 1b")

                            : "+r" (src), "+r"(dst), "+r"(blocks) :
                            : "cc", "memory",
                              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
                        );
                    }

                    if (regs > 0)
                    {
                        __asm__ __volatile__
                        (
                            __ASM_EMIT("1:")
                            // Copy data
                            __ASM_EMIT("movaps (%0), %%xmm0")
                            __ASM_EMIT(MOVNTPS " %%xmm0, (%1)")

                            // Move pointers
                            __ASM_EMIT("add $0x10, %0")
                            __ASM_EMIT("add $0x10, %1")
                            __ASM_EMIT("dec %2")
                            __ASM_EMIT("jnz 1b")

                            : "+r" (src), "+r"(dst), "+r"(regs) :
                            : "cc", "memory",
                              "%xmm0"
                        );
                    }
                }
                else
                {
                    if (blocks > 0)
                    {
                        __asm__ __volatile__
                        (
                            __ASM_EMIT("1:")
                            // Prefetch next data
                            __ASM_EMIT("prefetchnta 0x80(%0)")
                            __ASM_EMIT("prefetchnta 0xa0(%0)")
                            __ASM_EMIT("prefetchnta 0xc0(%0)")
                            __ASM_EMIT("prefetchnta 0xe0(%0)")

                            // calculate
                            __ASM_EMIT("movups 0x00(%0), %%xmm0")
                            __ASM_EMIT("movups 0x10(%0), %%xmm1")
                            __ASM_EMIT("movups 0x20(%0), %%xmm2")
                            __ASM_EMIT("movups 0x30(%0), %%xmm3")
                            __ASM_EMIT("movups 0x40(%0), %%xmm4")
                            __ASM_EMIT("movups 0x50(%0), %%xmm5")
                            __ASM_EMIT("movups 0x60(%0), %%xmm6")
                            __ASM_EMIT("movups 0x70(%0), %%xmm7")

                            __ASM_EMIT(MOVNTPS " %%xmm0,  0x00(%1)")
                            __ASM_EMIT(MOVNTPS " %%xmm1,  0x10(%1)")
                            __ASM_EMIT(MOVNTPS " %%xmm2,  0x20(%1)")
                            __ASM_EMIT(MOVNTPS " %%xmm3,  0x30(%1)")
                            __ASM_EMIT(MOVNTPS " %%xmm4,  0x40(%1)")
                            __ASM_EMIT(MOVNTPS " %%xmm5,  0x50(%1)")
                            __ASM_EMIT(MOVNTPS " %%xmm6,  0x60(%1)")
                            __ASM_EMIT(MOVNTPS " %%xmm7,  0x70(%1)")

                            // Move pointers
                            __ASM_EMIT("add $0x80, %0")
                            __ASM_EMIT("add $0x80, %1")

                            // Repeat loop
                            __ASM_EMIT("dec %2")
                            __ASM_EMIT("jnz 1b")

                            : "+r" (src), "+r"(dst), "+r"(blocks) :
                            : "cc", "memory",
                              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
                        );
                    }

                    if (regs > 0)
                    {
                        __asm__ __volatile__
                        (
                            __ASM_EMIT("1:")
                            // Copy data
                            __ASM_EMIT("movups (%0), %%xmm0")
                            __ASM_EMIT(MOVNTPS " %%xmm0, (%1)")

                            // Move pointers
                            __ASM_EMIT("add $0x10, %0")
                            __ASM_EMIT("add $0x10, %1")
                            __ASM_EMIT("dec %2")
                            __ASM_EMIT("jnz 1b")

                            : "+r" (src), "+r"(dst), "+r"(regs) :
                            : "cc", "memory",
                              "%xmm0"
                        );
                    }
                }
            }

            // Copy tail
            __asm__ __volatile__
            (
                __ASM_EMIT("test %[count], %[count]")
                __ASM_EMIT("jz 2f")
                __ASM_EMIT("1:")

                __ASM_EMIT("movss (%[src]), %%xmm0")
                __ASM_EMIT("movss %%xmm0, (%[dst])")

                // Move pointers
                __ASM_EMIT("add $0x4, %[src]")
                __ASM_EMIT("add $0x4, %[dst]")

                // Repeat loop
                __ASM_EMIT("dec %[count]")
                __ASM_EMIT("jnz 1b")

                __ASM_EMIT("2:")

                : [src] "+r" (src), [dst] "+r"(dst), [count] "+r"(count) :
                : "cc", "memory",
                  "%xmm0"
            );

            SFENCE;
        }

        static void copy_backward(float *dst, const float *src, size_t count)
        {
            __asm__ __volatile__
            (
                // Update pointers
                __ASM_EMIT("lea 0(%0,%2,4), %0")
                __ASM_EMIT("lea 0(%1,%2,4), %1")

                __ASM_EMIT("1:")

                // Check conditions
                __ASM_EMIT("test $0x0f, %1")
                __ASM_EMIT("jz 2f")

                // Move pointers
                __ASM_EMIT("sub $0x4, %0")
                __ASM_EMIT("sub $0x4, %1")

                // Copy data
                __ASM_EMIT("movss (%0), %%xmm0")
                __ASM_EMIT("movss %%xmm0, (%1)")

                // Repeat loop
                __ASM_EMIT("dec %2")
                __ASM_EMIT("jnz 1b")
                __ASM_EMIT("2:")

                : "+r" (src), "+r"(dst), "+r" (count) :
                : "cc", "memory",
                  "%xmm0"
            );

            // Calculate amount of registers
            size_t regs     = count / SSE_MULTIPLE;
            count          %= SSE_MULTIPLE;

            if (regs > 0)
            {
                // Calculate amount of 8-register blocks (0x80 bytes)
                size_t blocks   = regs / 8;
                regs           %= 8;

                // Prefetch data for iteration
                __asm__ __volatile__
                (
                    __ASM_EMIT("prefetchnta -0x80(%0)")
                    __ASM_EMIT("prefetchnta -0x60(%0)")
                    __ASM_EMIT("prefetchnta -0x40(%0)")
                    __ASM_EMIT("prefetchnta -0x20(%0)")
                    : : "S" (src)
                );

                if (sse_aligned(src))
                {
                    if (blocks > 0)
                    {
                        __asm__ __volatile__
                        (
                            __ASM_EMIT("1:")

                            // Move pointers
                            __ASM_EMIT("sub $0x80, %0")
                            __ASM_EMIT("sub $0x80, %1")

                            // Prefetch next data
                            __ASM_EMIT("prefetchnta -0x80(%0)")
                            __ASM_EMIT("prefetchnta -0x60(%0)")
                            __ASM_EMIT("prefetchnta -0x40(%0)")
                            __ASM_EMIT("prefetchnta -0x20(%0)")

                            // calculate
                            __ASM_EMIT("movaps 0x00(%0), %%xmm0")
                            __ASM_EMIT("movaps 0x10(%0), %%xmm1")
                            __ASM_EMIT("movaps 0x20(%0), %%xmm2")
                            __ASM_EMIT("movaps 0x30(%0), %%xmm3")
                            __ASM_EMIT("movaps 0x40(%0), %%xmm4")
                            __ASM_EMIT("movaps 0x50(%0), %%xmm5")
                            __ASM_EMIT("movaps 0x60(%0), %%xmm6")
                            __ASM_EMIT("movaps 0x70(%0), %%xmm7")

                            __ASM_EMIT(MOVNTPS " %%xmm0,  0x00(%1)")
                            __ASM_EMIT(MOVNTPS " %%xmm1,  0x10(%1)")
                            __ASM_EMIT(MOVNTPS " %%xmm2,  0x20(%1)")
                            __ASM_EMIT(MOVNTPS " %%xmm3,  0x30(%1)")
                            __ASM_EMIT(MOVNTPS " %%xmm4,  0x40(%1)")
                            __ASM_EMIT(MOVNTPS " %%xmm5,  0x50(%1)")
                            __ASM_EMIT(MOVNTPS " %%xmm6,  0x60(%1)")
                            __ASM_EMIT(MOVNTPS " %%xmm7,  0x70(%1)")

                            // Check loop
                            __ASM_EMIT("dec %2")
                            __ASM_EMIT("jnz 1b")

                            : "+r" (src), "+r"(dst), "+r"(blocks) :
                            : "cc", "memory",
                              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
                        );
                    }

                    if (regs > 0)
                    {
                        __asm__ __volatile__
                        (
                            __ASM_EMIT("1:")

                            // Move pointers
                            __ASM_EMIT("sub $0x10, %0")
                            __ASM_EMIT("sub $0x10, %1")

                            // Copy data
                            __ASM_EMIT("movaps (%0), %%xmm0")
                            __ASM_EMIT(MOVNTPS " %%xmm0, (%1)")

                            __ASM_EMIT("dec %2")
                            __ASM_EMIT("jnz 1b")

                            : "+r" (src), "+r"(dst), "+r"(regs) :
                            : "cc", "memory",
                              "%xmm0"
                        );
                    }
                }
                else
                {
                    if (blocks > 0)
                    {
                        __asm__ __volatile__
                        (
                            __ASM_EMIT("1:")

                            // Move pointers
                            __ASM_EMIT("sub $0x80, %0")
                            __ASM_EMIT("sub $0x80, %1")

                            // Prefetch next data
                            __ASM_EMIT("prefetchnta -0x80(%0)")
                            __ASM_EMIT("prefetchnta -0x60(%0)")
                            __ASM_EMIT("prefetchnta -0x40(%0)")
                            __ASM_EMIT("prefetchnta -0x20(%0)")

                            // calculate
                            __ASM_EMIT("movups 0x00(%0), %%xmm0")
                            __ASM_EMIT("movups 0x10(%0), %%xmm1")
                            __ASM_EMIT("movups 0x20(%0), %%xmm2")
                            __ASM_EMIT("movups 0x30(%0), %%xmm3")
                            __ASM_EMIT("movups 0x40(%0), %%xmm4")
                            __ASM_EMIT("movups 0x50(%0), %%xmm5")
                            __ASM_EMIT("movups 0x60(%0), %%xmm6")
                            __ASM_EMIT("movups 0x70(%0), %%xmm7")

                            __ASM_EMIT(MOVNTPS " %%xmm0,  0x00(%1)")
                            __ASM_EMIT(MOVNTPS " %%xmm1,  0x10(%1)")
                            __ASM_EMIT(MOVNTPS " %%xmm2,  0x20(%1)")
                            __ASM_EMIT(MOVNTPS " %%xmm3,  0x30(%1)")
                            __ASM_EMIT(MOVNTPS " %%xmm4,  0x40(%1)")
                            __ASM_EMIT(MOVNTPS " %%xmm5,  0x50(%1)")
                            __ASM_EMIT(MOVNTPS " %%xmm6,  0x60(%1)")
                            __ASM_EMIT(MOVNTPS " %%xmm7,  0x70(%1)")

                            // Check loop
                            __ASM_EMIT("dec %2")
                            __ASM_EMIT("jnz 1b")

                            : "+r" (src), "+r"(dst), "+r"(blocks) :
                            : "cc", "memory",
                              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
                        );
                    }

                    if (regs > 0)
                    {
                        __asm__ __volatile__
                        (
                            __ASM_EMIT("1:")

                            // Move pointers
                            __ASM_EMIT("sub $0x10, %0")
                            __ASM_EMIT("sub $0x10, %1")

                            // Copy data
                            __ASM_EMIT("movups (%0), %%xmm0")
                            __ASM_EMIT(MOVNTPS " %%xmm0, (%1)")

                            __ASM_EMIT("dec %2")
                            __ASM_EMIT("jnz 1b")

                            : "+r" (src), "+r"(dst), "+r"(regs) :
                            : "cc", "memory",
                              "%xmm0"
                        );
                    }
                }
            }

            // Copy tail
            if (count > 0)
            {
                __asm__ __volatile__
                (
                    __ASM_EMIT("1:")

                    // Move pointers
                    __ASM_EMIT("sub $0x4, %0")
                    __ASM_EMIT("sub $0x4, %1")

                    __ASM_EMIT("movss (%0), %%xmm0")
                    __ASM_EMIT("movss %%xmm0, (%1)")

                    // Repeat loop
                    __ASM_EMIT("dec %2")
                    __ASM_EMIT("jnz 1b")

                    : "+r" (src), "+r"(dst), "+r"(count) :
                    : "cc", "memory",
                      "%xmm0"
                );
            }

            SFENCE;
        }

        static void scale(float *dst, const float *src, float k, size_t count)
        {
            if (count == 0)
                return;

            __asm__ __volatile__
            (
                __ASM_EMIT("movss %3, %%xmm4")
                __ASM_EMIT("1:")

                // Check conditions
                __ASM_EMIT("test $0x0f, %1")
                __ASM_EMIT("jz 2f")

                // Copy data
                __ASM_EMIT("movss (%0), %%xmm0")
                __ASM_EMIT("mulss %%xmm4, %%xmm0")
                __ASM_EMIT("movss %%xmm0, (%1)")

                // Move pointers
                __ASM_EMIT("add $0x4, %0")
                __ASM_EMIT("add $0x4, %1")

                // Repeat loop
                __ASM_EMIT("dec %2")
                __ASM_EMIT("jnz 1b")
                __ASM_EMIT("2:")

                : "+r" (src), "+r"(dst), "+r" (count), "+x"(k) :
                : "cc", "memory",
                  "%xmm4"
            );

            // Calculate amount of registers
            size_t regs     = count / SSE_MULTIPLE;
            count          %= SSE_MULTIPLE;

            if (regs > 0)
            {
                // Calculate amount of 4-register blocks (0x40 bytes)
                size_t blocks   = regs / 4;
                regs           %= 4;

                // Prefetch data for iteration
                __asm__ __volatile__
                (
                    __ASM_EMIT("shufps $0, %%xmm4, %%xmm4")
                    __ASM_EMIT("prefetchnta 0x00(%0)")
                    __ASM_EMIT("prefetchnta 0x20(%0)")
                    __ASM_EMIT("prefetchnta 0x40(%0)")
                    __ASM_EMIT("prefetchnta 0x60(%0)")
                    : : "S" (src)
                    : "%xmm4"
                );

                if (sse_aligned(src))
                {
                    if (blocks > 0)
                    {
                        __asm__ __volatile__
                        (
                            __ASM_EMIT("movaps %%xmm4, %%xmm5")
                            __ASM_EMIT("movaps %%xmm4, %%xmm6")
                            __ASM_EMIT("movaps %%xmm5, %%xmm7")

                            __ASM_EMIT("1:")
                            // Prefetch next data
                            __ASM_EMIT("prefetchnta 0x80(%0)")
                            __ASM_EMIT("prefetchnta 0xa0(%0)")

                            // calculate
                            __ASM_EMIT("movaps 0x00(%0), %%xmm0")
                            __ASM_EMIT("movaps 0x10(%0), %%xmm1")
                            __ASM_EMIT("movaps 0x20(%0), %%xmm2")
                            __ASM_EMIT("movaps 0x30(%0), %%xmm3")
                            __ASM_EMIT("mulps  %%xmm4, %%xmm0")
                            __ASM_EMIT("mulps  %%xmm5, %%xmm1")
                            __ASM_EMIT("mulps  %%xmm6, %%xmm2")
                            __ASM_EMIT("mulps  %%xmm7, %%xmm3")

                            __ASM_EMIT(MOVNTPS " %%xmm0,  0x00(%1)")
                            __ASM_EMIT(MOVNTPS " %%xmm1,  0x10(%1)")
                            __ASM_EMIT(MOVNTPS " %%xmm2,  0x20(%1)")
                            __ASM_EMIT(MOVNTPS " %%xmm3,  0x30(%1)")

                            // Move pointers
                            __ASM_EMIT("add $0x40, %0")
                            __ASM_EMIT("add $0x40, %1")
                            __ASM_EMIT("dec %2")
                            __ASM_EMIT("jnz 1b")

                            : "+r" (src), "+r"(dst), "+r"(blocks) :
                            : "cc", "memory",
                              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
                        );
                    }

                    if (regs > 0)
                    {
                        __asm__ __volatile__
                        (
                            __ASM_EMIT("1:")
                            // Copy data
                            __ASM_EMIT("movaps (%0), %%xmm0")
                            __ASM_EMIT("mulps  %%xmm4, %%xmm0")
                            __ASM_EMIT(MOVNTPS " %%xmm0, (%1)")

                            // Move pointers
                            __ASM_EMIT("add $0x10, %0")
                            __ASM_EMIT("add $0x10, %1")
                            __ASM_EMIT("dec %2")
                            __ASM_EMIT("jnz 1b")

                            : "+r" (src), "+r"(dst), "+r"(regs) :
                            : "cc", "memory",
                              "%xmm0"
                        );
                    }
                }
                else
                {
                    if (blocks > 0)
                    {
                        __asm__ __volatile__
                        (
                            __ASM_EMIT("movaps %%xmm4, %%xmm5")
                            __ASM_EMIT("movaps %%xmm4, %%xmm6")
                            __ASM_EMIT("movaps %%xmm5, %%xmm7")

                            __ASM_EMIT("1:")
                            // Prefetch next data
                            __ASM_EMIT("prefetchnta 0x80(%0)")
                            __ASM_EMIT("prefetchnta 0xa0(%0)")

                            // calculate
                            __ASM_EMIT("movups 0x00(%0), %%xmm0")
                            __ASM_EMIT("movups 0x10(%0), %%xmm1")
                            __ASM_EMIT("movups 0x20(%0), %%xmm2")
                            __ASM_EMIT("movups 0x30(%0), %%xmm3")
                            __ASM_EMIT("mulps  %%xmm4, %%xmm0")
                            __ASM_EMIT("mulps  %%xmm5, %%xmm1")
                            __ASM_EMIT("mulps  %%xmm6, %%xmm2")
                            __ASM_EMIT("mulps  %%xmm7, %%xmm3")

                            __ASM_EMIT(MOVNTPS " %%xmm0,  0x00(%1)")
                            __ASM_EMIT(MOVNTPS " %%xmm1,  0x10(%1)")
                            __ASM_EMIT(MOVNTPS " %%xmm2,  0x20(%1)")
                            __ASM_EMIT(MOVNTPS " %%xmm3,  0x30(%1)")

                            // Move pointers
                            __ASM_EMIT("add $0x40, %0")
                            __ASM_EMIT("add $0x40, %1")
                            __ASM_EMIT("dec %2")
                            __ASM_EMIT("jnz 1b")

                            : "+r" (src), "+r"(dst), "+r"(blocks) :
                            : "cc", "memory",
                              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
                        );
                    }

                    if (regs > 0)
                    {
                        __asm__ __volatile__
                        (
                            __ASM_EMIT("1:")
                            // Copy data
                            __ASM_EMIT("movups (%0), %%xmm0")
                            __ASM_EMIT("mulps  %%xmm4, %%xmm0")
                            __ASM_EMIT(MOVNTPS " %%xmm0, (%1)")

                            // Move pointers
                            __ASM_EMIT("add $0x10, %0")
                            __ASM_EMIT("add $0x10, %1")
                            __ASM_EMIT("dec %2")
                            __ASM_EMIT("jnz 1b")

                            : "+r" (src), "+r"(dst), "+r"(regs) :
                            : "cc", "memory",
                              "%xmm0"
                        );
                    }
                }
            }

            // Copy tail
            if (count > 0)
            {
                __asm__ __volatile__
                (
                    __ASM_EMIT("1:")

                    __ASM_EMIT("movss (%0), %%xmm0")
                    __ASM_EMIT("mulss  %%xmm4, %%xmm0")
                    __ASM_EMIT("movss %%xmm0, (%1)")

                    // Move pointers
                    __ASM_EMIT("add $0x4, %0")
                    __ASM_EMIT("add $0x4, %1")
                    __ASM_EMIT("dec %2")
                    __ASM_EMIT("jnz 1b")

                    : "+r" (src), "+r"(dst), "+r"(count) :
                    : "cc", "memory",
                      "%xmm0"
                );
            }

            SFENCE;
        }

        static void multiply(float *dst_mod, const float *src1, const float *src2, size_t count)
        {
            if (count == 0)
                return;

            __asm__ __volatile__
            (
                __ASM_EMIT("1:")

                // Check conditions
                __ASM_EMIT("test $0x0f, %2")
                __ASM_EMIT("jz 2f")

                // Process data
                __ASM_EMIT("movss (%0), %%xmm0")
                __ASM_EMIT("movss (%1), %%xmm1")
                __ASM_EMIT("mulss %%xmm1, %%xmm0")
                __ASM_EMIT("movss %%xmm0, (%2)")

                // Move pointers
                __ASM_EMIT("add $0x4, %0")
                __ASM_EMIT("add $0x4, %1")
                __ASM_EMIT("add $0x4, %2")

                // Repeat loop
                __ASM_EMIT("dec %3")
                __ASM_EMIT("jnz 1b")
                __ASM_EMIT("2:")

                : "+r" (src1), "+r"(src2), "+r"(dst_mod), "+r" (count) :
                : "cc", "memory",
                  "%xmm0", "%xmm1"
            );

            size_t regs     = count / SSE_MULTIPLE;
            count          %= SSE_MULTIPLE;

            if (regs > 0)
            {
                size_t blocks   = regs / 4;
                regs           %= 4;

                // Prefetch data for iteration
                __asm__ __volatile__
                (
                    __ASM_EMIT("prefetchnta  0x00(%0)")
                    __ASM_EMIT("prefetchnta  0x20(%0)")
                    __ASM_EMIT("prefetchnta  0x00(%1)")
                    __ASM_EMIT("prefetchnta  0x20(%1)")
                    : : "r" (src1), "r" (src2)
                );

                #define multiply4(l_re, l_im)   \
                { \
                    if (blocks > 0) \
                    { \
                        __asm__ __volatile__ \
                        ( \
                            __ASM_EMIT("1:") \
                            \
                            __ASM_EMIT("prefetchnta  0x40(%0)") \
                            __ASM_EMIT("prefetchnta  0x60(%0)") \
                            __ASM_EMIT("prefetchnta  0x40(%1)") \
                            __ASM_EMIT("prefetchnta  0x60(%1)") \
                            /* Process data */ \
                            __ASM_EMIT(l_re " 0x00(%0), %%xmm0") \
                            __ASM_EMIT(l_re " 0x10(%0), %%xmm1") \
                            __ASM_EMIT(l_re " 0x20(%0), %%xmm2") \
                            __ASM_EMIT(l_re " 0x30(%0), %%xmm3") \
                            __ASM_EMIT(l_im " 0x00(%1), %%xmm4") \
                            __ASM_EMIT(l_im " 0x10(%1), %%xmm5") \
                            __ASM_EMIT(l_im " 0x20(%1), %%xmm6") \
                            __ASM_EMIT(l_im " 0x30(%1), %%xmm7") \
                            \
                            __ASM_EMIT("mulps %%xmm4, %%xmm0") \
                            __ASM_EMIT("mulps %%xmm5, %%xmm1") \
                            __ASM_EMIT("mulps %%xmm6, %%xmm2") \
                            __ASM_EMIT("mulps %%xmm7, %%xmm3") \
                            \
                            __ASM_EMIT(MOVNTPS " %%xmm0, 0x00(%2)") \
                            __ASM_EMIT(MOVNTPS " %%xmm1, 0x10(%2)") \
                            __ASM_EMIT(MOVNTPS " %%xmm2, 0x20(%2)") \
                            __ASM_EMIT(MOVNTPS " %%xmm3, 0x30(%2)") \
                            \
                            /* Move pointers */ \
                            __ASM_EMIT("add $0x40, %0") \
                            __ASM_EMIT("add $0x40, %1") \
                            __ASM_EMIT("add $0x40, %2") \
                            \
                            /* Repeat loop */ \
                            __ASM_EMIT("dec %3") \
                            __ASM_EMIT("jnz 1b") \
                            __ASM_EMIT("2:") \
                            \
                            : "+r" (src1), "+r"(src2), "+r"(dst_mod), "+r" (blocks) : \
                            : "cc", "memory", \
                              "%xmm0", "%xmm1", "%xmm2", "%xmm3", \
                              "%xmm4", "%xmm5", "%xmm6", "%xmm7" \
                        ); \
                    } \
                    \
                    if (regs > 0) \
                    { \
                        __asm__ __volatile__ \
                        ( \
                            __ASM_EMIT("1:") \
                            \
                            /* Process data */ \
                            __ASM_EMIT(l_re " (%0), %%xmm0") \
                            __ASM_EMIT(l_im " (%1), %%xmm1") \
                            __ASM_EMIT("mulps %%xmm1, %%xmm0") \
                            __ASM_EMIT(MOVNTPS " %%xmm0, (%2)") \
                            \
                            /* Move pointers */ \
                            __ASM_EMIT("add $0x10, %0") \
                            __ASM_EMIT("add $0x10, %1") \
                            __ASM_EMIT("add $0x10, %2") \
                            \
                            /* Repeat loop */ \
                            __ASM_EMIT("dec %3") \
                            __ASM_EMIT("jnz 1b") \
                            __ASM_EMIT("2:") \
                            \
                            : "+r" (src1), "+r"(src2), "+r"(dst_mod), "+r" (regs) : \
                            : "cc", "memory", \
                              "%xmm0", "%xmm1" \
                        ); \
                    } \
                }

                if (sse_aligned(src1))
                {
                    if (sse_aligned(src2))
                        multiply4("movaps", "movaps")
                    else
                        multiply4("movaps", "movups")
                }
                else
                {
                    if (sse_aligned(src2))
                        multiply4("movups", "movaps")
                    else
                        multiply4("movups", "movups")
                }

                #undef multiply4
            }

            if (count > 0)
            {
                __asm__ __volatile__
                (
                    __ASM_EMIT("1:")

                    // Process data
                    __ASM_EMIT("movss (%0), %%xmm0")
                    __ASM_EMIT("movss (%1), %%xmm1")
                    __ASM_EMIT("mulss %%xmm1, %%xmm0")
                    __ASM_EMIT("movss %%xmm0, (%2)")

                    // Move pointers
                    __ASM_EMIT("add $0x4, %0")
                    __ASM_EMIT("add $0x4, %1")
                    __ASM_EMIT("add $0x4, %2")

                    // Repeat loop
                    __ASM_EMIT("dec %3")
                    __ASM_EMIT("jnz 1b")
                    __ASM_EMIT("2:")

                    : "+r" (src1), "+r"(src2), "+r"(dst_mod), "+r" (count) :
                    : "cc", "memory",
                      "%xmm0", "%xmm1"
                );
            }
        }

        static float h_sum(const float *src, size_t count)
        {
            if (count == 0)
                return 0.0f;

            float result;

            __asm__ __volatile__
            (
                __ASM_EMIT("xorps %2, %2")
                __ASM_EMIT("1:")

                // Check conditions
                __ASM_EMIT("test $0x0f, %0")
                __ASM_EMIT("jz 2f")

                // Process data
                __ASM_EMIT("movss (%0), %%xmm1")
                __ASM_EMIT("addss %%xmm1, %2")

                // Move pointers
                __ASM_EMIT("add $0x4, %0")

                // Repeat loop
                __ASM_EMIT("dec %1")
                __ASM_EMIT("jnz 1b")
                __ASM_EMIT("2:")

                : "+r"(src), "+r" (count), "=Yz" (result) :
                : "cc", "memory",
                  "%xmm1"
            );

            size_t regs     = count / SSE_MULTIPLE;
            count          %= SSE_MULTIPLE;

            if (regs > 0)
            {
                // Prefetch data for iteration
                __asm__ __volatile__
                (
                    __ASM_EMIT("prefetchnta  0x00(%0)")
                    __ASM_EMIT("prefetchnta  0x20(%0)")
                    : : "r" (src)
                );

                size_t blocks   = regs / 4;
                regs           %= 4;

                if (blocks > 0)
                {
                    __asm__ __volatile__
                    (
                        __ASM_EMIT("xorps %%xmm1, %%xmm1")
                        __ASM_EMIT("xorps %%xmm2, %%xmm2")
                        __ASM_EMIT("xorps %%xmm3, %%xmm3")
                        __ASM_EMIT("1:")

                        // Prefetch data
                        __ASM_EMIT("prefetchnta  0x40(%0)")
                        __ASM_EMIT("prefetchnta  0x60(%0)")

                        // Check conditions
                        __ASM_EMIT("movaps 0x00(%0), %%xmm4")
                        __ASM_EMIT("movaps 0x10(%0), %%xmm5")
                        __ASM_EMIT("movaps 0x20(%0), %%xmm6")
                        __ASM_EMIT("movaps 0x30(%0), %%xmm7")
                        __ASM_EMIT("addps %%xmm4, %%xmm0")
                        __ASM_EMIT("addps %%xmm5, %%xmm1")
                        __ASM_EMIT("addps %%xmm6, %%xmm2")
                        __ASM_EMIT("addps %%xmm7, %%xmm3")

                        // Move pointers
                        __ASM_EMIT("add $0x40, %0")

                        // Repeat loop
                        __ASM_EMIT("dec %1")
                        __ASM_EMIT("jnz 1b")
                        __ASM_EMIT("2:")

                        // Summarize
                        __ASM_EMIT("addps %%xmm3, %%xmm2")
                        __ASM_EMIT("addps %%xmm1, %%xmm0")
                        __ASM_EMIT("addps %%xmm2, %%xmm0")

                        : "+r"(src), "+r" (blocks) :
                        : "cc", "memory",
                          "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                          "%xmm4", "%xmm5", "%xmm6", "%xmm7"
                    );
                }

                if (regs > 0)
                {
                    __asm__ __volatile__
                    (
                        __ASM_EMIT("1:")

                        // Check conditions
                        __ASM_EMIT("movaps 0x00(%0), %%xmm1")
                        __ASM_EMIT("addps %%xmm1, %%xmm0")

                        // Move pointers
                        __ASM_EMIT("add $0x10, %0")

                        // Repeat loop
                        __ASM_EMIT("dec %1")
                        __ASM_EMIT("jnz 1b")
                        __ASM_EMIT("2:")

                        : "+r"(src), "+r" (regs) :
                        : "cc", "memory",
                          "%xmm0", "%xmm1"
                    );
                }

                __asm__ __volatile__
                (
                    // Summarize horizontally
                    __ASM_EMIT("movaps %2, %%xmm1")
                    __ASM_EMIT("movaps %2, %%xmm2")
                    __ASM_EMIT("movaps %2, %%xmm3")

                    // Aggregate data
                    __ASM_EMIT("shufps $0x55, %%xmm1, %%xmm1")
                    __ASM_EMIT("shufps $0xaa, %%xmm2, %%xmm2")
                    __ASM_EMIT("shufps $0xff, %%xmm3, %%xmm3")
                    __ASM_EMIT("shufps $0x00, %2, %2")

                    __ASM_EMIT("addps %%xmm1, %2")
                    __ASM_EMIT("addps %%xmm3, %%xmm2")
                    __ASM_EMIT("addps %%xmm2, %2")

                    : "+r"(src), "+r" (regs), "=Yz"(result) :
                    : "cc", "memory",
                      "%xmm1", "%xmm2", "%xmm3"
                );
            }

            if (count > 0)
            {
                __asm__ __volatile__
                (
                    __ASM_EMIT("1:")

                    // Process data
                    __ASM_EMIT("movss (%0), %%xmm1")
                    __ASM_EMIT("addss %%xmm1, %2")

                    // Move pointers
                    __ASM_EMIT("add $0x4, %0")

                    // Repeat loop
                    __ASM_EMIT("dec %1")
                    __ASM_EMIT("jnz 1b")
                    __ASM_EMIT("2:")

                    : "+r" (src), "+r" (count), "=Yz"(result) :
                    : "cc", "memory",
                      "%xmm1"
                );
            }

            return result;
        }

        static float h_sqr_sum(const float *src, size_t count)
        {
            if (count == 0)
                return 0.0f;

            float result;

            __asm__ __volatile__
            (
                __ASM_EMIT("xorps %2, %2")
                __ASM_EMIT("1:")

                // Check conditions
                __ASM_EMIT("test $0x0f, %0")
                __ASM_EMIT("jz 2f")

                // Process data
                __ASM_EMIT("movss (%0), %%xmm1")
                __ASM_EMIT("mulss %%xmm1, %%xmm1")
                __ASM_EMIT("addss %%xmm1, %2")

                // Move pointers
                __ASM_EMIT("add $0x4, %0")

                // Repeat loop
                __ASM_EMIT("dec %1")
                __ASM_EMIT("jnz 1b")
                __ASM_EMIT("2:")

                : "+r"(src), "+r" (count), "=Yz" (result) :
                : "cc", "memory",
                  "%xmm1"
            );

            size_t regs     = count / SSE_MULTIPLE;
            count          %= SSE_MULTIPLE;

            if (regs > 0)
            {
                // Prefetch data for iteration
                __asm__ __volatile__
                (
                    __ASM_EMIT("prefetchnta  0x00(%0)")
                    __ASM_EMIT("prefetchnta  0x20(%0)")
                    : : "r" (src)
                );

                size_t blocks   = regs / 4;
                regs           %= 4;

                if (blocks > 0)
                {
                    __asm__ __volatile__
                    (
                        __ASM_EMIT("xorps %%xmm1, %%xmm1")
                        __ASM_EMIT("xorps %%xmm2, %%xmm2")
                        __ASM_EMIT("xorps %%xmm3, %%xmm3")
                        __ASM_EMIT("1:")

                        // Prefetch data
                        __ASM_EMIT("prefetchnta  0x40(%0)")
                        __ASM_EMIT("prefetchnta  0x60(%0)")

                        // Check conditions
                        __ASM_EMIT("movaps 0x00(%0), %%xmm4")
                        __ASM_EMIT("movaps 0x10(%0), %%xmm5")
                        __ASM_EMIT("movaps 0x20(%0), %%xmm6")
                        __ASM_EMIT("movaps 0x30(%0), %%xmm7")
                        __ASM_EMIT("mulps %%xmm4, %%xmm4")
                        __ASM_EMIT("mulps %%xmm5, %%xmm5")
                        __ASM_EMIT("mulps %%xmm6, %%xmm6")
                        __ASM_EMIT("mulps %%xmm7, %%xmm7")
                        __ASM_EMIT("addps %%xmm4, %%xmm0")
                        __ASM_EMIT("addps %%xmm5, %%xmm1")
                        __ASM_EMIT("addps %%xmm6, %%xmm2")
                        __ASM_EMIT("addps %%xmm7, %%xmm3")

                        // Move pointers
                        __ASM_EMIT("add $0x40, %0")

                        // Repeat loop
                        __ASM_EMIT("dec %1")
                        __ASM_EMIT("jnz 1b")
                        __ASM_EMIT("2:")

                        // Summarize
                        __ASM_EMIT("addps %%xmm3, %%xmm2")
                        __ASM_EMIT("addps %%xmm1, %%xmm0")
                        __ASM_EMIT("addps %%xmm2, %%xmm0")

                        : "+r"(src), "+r" (blocks) :
                        : "cc", "memory",
                          "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                          "%xmm4", "%xmm5", "%xmm6", "%xmm7"
                    );
                }

                if (regs > 0)
                {
                    __asm__ __volatile__
                    (
                        __ASM_EMIT("1:")

                        // Check conditions
                        __ASM_EMIT("movaps 0x00(%0), %%xmm1")
                        __ASM_EMIT("mulps %%xmm1, %%xmm1")
                        __ASM_EMIT("addps %%xmm1, %%xmm0")

                        // Move pointers
                        __ASM_EMIT("add $0x10, %0")

                        // Repeat loop
                        __ASM_EMIT("dec %1")
                        __ASM_EMIT("jnz 1b")
                        __ASM_EMIT("2:")

                        : "+r"(src), "+r" (regs) :
                        : "cc", "memory",
                          "%xmm0", "%xmm1"
                    );
                }

                __asm__ __volatile__
                (
                    // Summarize horizontally
                    __ASM_EMIT("movaps %2, %%xmm1")
                    __ASM_EMIT("movaps %2, %%xmm2")
                    __ASM_EMIT("movaps %2, %%xmm3")

                    // Aggregate data
                    __ASM_EMIT("shufps $0x55, %%xmm1, %%xmm1")
                    __ASM_EMIT("shufps $0xaa, %%xmm2, %%xmm2")
                    __ASM_EMIT("shufps $0xff, %%xmm3, %%xmm3")
                    __ASM_EMIT("shufps $0x00, %2, %2")

                    __ASM_EMIT("addps %%xmm1, %2")
                    __ASM_EMIT("addps %%xmm3, %%xmm2")
                    __ASM_EMIT("addps %%xmm2, %2")

                    : "+r"(src), "+r" (regs), "=Yz"(result) :
                    : "cc", "memory",
                      "%xmm1", "%xmm2", "%xmm3"
                );
            }

            if (count > 0)
            {
                __asm__ __volatile__
                (
                    __ASM_EMIT("1:")

                    // Process data
                    __ASM_EMIT("movss (%0), %%xmm1")
                    __ASM_EMIT("mulss %%xmm1, %%xmm1")
                    __ASM_EMIT("addss %%xmm1, %2")

                    // Move pointers
                    __ASM_EMIT("add $0x4, %0")

                    // Repeat loop
                    __ASM_EMIT("dec %1")
                    __ASM_EMIT("jnz 1b")
                    __ASM_EMIT("2:")

                    : "+r" (src), "+r" (count), "=Yz"(result) :
                    : "cc", "memory",
                      "%xmm1"
                );
            }

            return result;
        }

        static float h_abs_sum(const float *src, size_t count)
        {
            if (count == 0)
                return 0.0f;

            float result;

            __asm__ __volatile__
            (
                __ASM_EMIT("movaps %[X_SIGN], %%xmm2")
                __ASM_EMIT("xorps %[result], %[result]")
                __ASM_EMIT("1:")

                // Check conditions
                __ASM_EMIT("test $0x0f, %[src]")
                __ASM_EMIT("jz 2f")

                // Process data
                __ASM_EMIT("movss (%[src]), %%xmm4")
                __ASM_EMIT("andps %%xmm2, %%xmm4")
                __ASM_EMIT("addss %%xmm4, %[result]")

                // Move pointers
                __ASM_EMIT("add $0x4, %[src]")

                // Repeat loop
                __ASM_EMIT("dec %[count]")
                __ASM_EMIT("jnz 1b")
                __ASM_EMIT("2:")

                : [src] "+r"(src), [count] "+r" (count), [result] "=Yz" (result)
                : [X_SIGN] "m" (X_SIGN)
                : "cc", "memory",
                  "%xmm2", "%xmm4"
            );

            size_t regs     = count / SSE_MULTIPLE;
            count          %= SSE_MULTIPLE;

            if (regs > 0)
            {
                // Prefetch data for iteration
                __asm__ __volatile__
                (
                    __ASM_EMIT("prefetchnta  0x00(%[src])")
                    __ASM_EMIT("prefetchnta  0x20(%[src])")
                    : : [src] "r" (src)
                );

                size_t blocks   = regs / 4;
                regs           %= 4;

                if (blocks > 0)
                {
                    __asm__ __volatile__
                    (
                        __ASM_EMIT("xorps %%xmm1, %%xmm1")
                        __ASM_EMIT("movaps %%xmm2, %%xmm3")
                        __ASM_EMIT("1:")

                        // Prefetch data
                        __ASM_EMIT("prefetchnta  0x40(%[src])")
                        __ASM_EMIT("prefetchnta  0x60(%[src])")

                        // Check conditions
                        __ASM_EMIT("movaps 0x00(%0), %%xmm4")
                        __ASM_EMIT("movaps 0x10(%0), %%xmm5")
                        __ASM_EMIT("movaps 0x20(%0), %%xmm6")
                        __ASM_EMIT("movaps 0x30(%0), %%xmm7")
                        __ASM_EMIT("andps %%xmm2, %%xmm4")
                        __ASM_EMIT("andps %%xmm3, %%xmm5")
                        __ASM_EMIT("andps %%xmm2, %%xmm6")
                        __ASM_EMIT("andps %%xmm3, %%xmm7")
                        __ASM_EMIT("addps %%xmm4, %%xmm0")
                        __ASM_EMIT("addps %%xmm5, %%xmm1")
                        __ASM_EMIT("addps %%xmm6, %%xmm0")
                        __ASM_EMIT("addps %%xmm7, %%xmm1")

                        // Move pointers
                        __ASM_EMIT("add $0x40, %0")

                        // Repeat loop
                        __ASM_EMIT("dec %[count]")
                        __ASM_EMIT("jnz 1b")
                        __ASM_EMIT("2:")

                        // Summarize
                        __ASM_EMIT("addps %%xmm1, %%xmm0")

                        : [src] "+r"(src), [count] "+r" (blocks) :
                        : "cc", "memory",
                          "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                          "%xmm4", "%xmm5", "%xmm6", "%xmm7"
                    );
                }

                if (regs > 0)
                {
                    __asm__ __volatile__
                    (
                        __ASM_EMIT("1:")

                        // Check conditions
                        __ASM_EMIT("movaps 0x00(%[src]), %%xmm4")
                        __ASM_EMIT("andps %%xmm2, %%xmm4")
                        __ASM_EMIT("addps %%xmm4, %%xmm0")

                        // Move pointers
                        __ASM_EMIT("add $0x10, %[src]")

                        // Repeat loop
                        __ASM_EMIT("dec %[count]")
                        __ASM_EMIT("jnz 1b")
                        __ASM_EMIT("2:")

                        : [src] "+r"(src), [count] "+r" (regs) :
                        : "cc", "memory",
                          "%xmm0", "%xmm4"
                    );
                }

                __asm__ __volatile__
                (
                    // Summarize horizontally
                    __ASM_EMIT("movaps %[result], %%xmm1")
                    __ASM_EMIT("movaps %[result], %%xmm2")
                    __ASM_EMIT("movaps %[result], %%xmm3")

                    // Aggregate data
                    __ASM_EMIT("shufps $0x55, %%xmm1, %%xmm1")
                    __ASM_EMIT("shufps $0xaa, %%xmm2, %%xmm2")
                    __ASM_EMIT("shufps $0xff, %%xmm3, %%xmm3")
                    __ASM_EMIT("shufps $0x00, %[result], %[result]")

                    __ASM_EMIT("addps %%xmm1, %[result]")
                    __ASM_EMIT("addps %%xmm3, %%xmm2")
                    __ASM_EMIT("addps %%xmm2, %[result]")

                    : [result] "=Yz"(result) :
                    : "cc", "memory",
                      "%xmm1", "%xmm2", "%xmm3"
                );
            }

            if (count > 0)
            {
                __asm__ __volatile__
                (
                    __ASM_EMIT("1:")

                    // Process data
                    __ASM_EMIT("movss (%[src]), %%xmm4")
                    __ASM_EMIT("andps %%xmm2, %%xmm4")
                    __ASM_EMIT("addss %%xmm4, %[result]")

                    // Move pointers
                    __ASM_EMIT("add $0x4, %[src]")

                    // Repeat loop
                    __ASM_EMIT("dec %[count]")
                    __ASM_EMIT("jnz 1b")
                    __ASM_EMIT("2:")

                    : [src] "+r" (src), [count] "+r" (count), [result] "=Yz"(result) :
                    : "cc", "memory",
                      "%xmm4"
                );
            }

            return result;
        }

        static void fill_forward(float *dst, float value, size_t count)
        {
            __asm__ __volatile__
            (
                __ASM_EMIT("1:")

                // Check conditions
                __ASM_EMIT("test $0x0f, %0")
                __ASM_EMIT("jz 2f")

                // Write data
                __ASM_EMIT("movss %2, (%0)")

                // Move pointer
                __ASM_EMIT("add $0x4, %0")

                // Repeat loop
                __ASM_EMIT("dec %1")
                __ASM_EMIT("jnz 1b")
                __ASM_EMIT("2:")

                : "+r"(dst), "+r" (count), "+Yz" (value) :
                : "cc", "memory"
            );

            // Calculate amount of registers
            size_t regs     = count / SSE_MULTIPLE;
            count          %= SSE_MULTIPLE;

            if (regs > 0)
            {
                __asm__ __volatile__
                (
                    __ASM_EMIT("shufps $0, %%xmm0, %%xmm0")
                    : :
                    : "%xmm0"
                );

                // Calculate amount of 8-register blocks (0x80 bytes)
                size_t blocks   = regs / 8;
                regs           %= 8;

                if (blocks > 0)
                {
                    __asm__ __volatile__
                    (
                        // Prepare other registers
                        __ASM_EMIT("movaps %%xmm0, %%xmm1")
                        __ASM_EMIT("movaps %%xmm0, %%xmm2")
                        __ASM_EMIT("movaps %%xmm1, %%xmm3")
                        __ASM_EMIT("movaps %%xmm0, %%xmm4")
                        __ASM_EMIT("movaps %%xmm1, %%xmm5")
                        __ASM_EMIT("movaps %%xmm2, %%xmm6")
                        __ASM_EMIT("movaps %%xmm3, %%xmm7")

                        __ASM_EMIT("1:")

                        __ASM_EMIT(MOVNTPS " %%xmm0,  0x00(%0)")
                        __ASM_EMIT(MOVNTPS " %%xmm1,  0x10(%0)")
                        __ASM_EMIT(MOVNTPS " %%xmm2,  0x20(%0)")
                        __ASM_EMIT(MOVNTPS " %%xmm3,  0x30(%0)")
                        __ASM_EMIT(MOVNTPS " %%xmm4,  0x40(%0)")
                        __ASM_EMIT(MOVNTPS " %%xmm5,  0x50(%0)")
                        __ASM_EMIT(MOVNTPS " %%xmm6,  0x60(%0)")
                        __ASM_EMIT(MOVNTPS " %%xmm7,  0x70(%0)")

                        // Move pointers
                        __ASM_EMIT("add $0x80, %0")
                        __ASM_EMIT("dec %1")
                        __ASM_EMIT("jnz 1b")

                        : "+r"(dst), "+r"(blocks) :
                        : "cc", "memory",
                          "%xmm1", "%xmm2", "%xmm3",
                          "%xmm4", "%xmm5", "%xmm6", "%xmm7"
                    );
                }

                if (regs > 0)
                {
                    __asm__ __volatile__
                    (
                        __ASM_EMIT("1:")
                        // Store data
                        __ASM_EMIT(MOVNTPS " %%xmm0, (%0)")

                        // Move pointers
                        __ASM_EMIT("add $0x10, %0")
                        __ASM_EMIT("dec %1")
                        __ASM_EMIT("jnz 1b")

                        : "+r"(dst), "+r"(regs) :
                        : "cc", "memory"
                    );
                }
            }

            // Fill tail
            if (count > 0)
            {
                __asm__ __volatile__
                (
                    __ASM_EMIT("1:")

                    // Store value
                    __ASM_EMIT("movss %%xmm0, (%0)")

                    // Move pointer
                    __ASM_EMIT("add $0x4, %0")

                    // Repeat loop
                    __ASM_EMIT("dec %1")
                    __ASM_EMIT("jnz 1b")

                    : "+r"(dst), "+r"(count) :
                    : "cc", "memory"
                );
            }

            SFENCE;
        }

        static void add_multiplied(float *dst, const float *src, float k, size_t count)
        {
            if (count == 0)
                return;

            __asm__ __volatile__
            (
                __ASM_EMIT("movss %0, %%xmm4")
                : : "x" (k)
                : "%xmm4"
            );

            // Align destination
            __asm__ __volatile__
            (
                __ASM_EMIT("1:")

                // Check conditions
                __ASM_EMIT("test $0xf, %1")
                __ASM_EMIT("jz 2f")

                // Make iteration
                __ASM_EMIT("movss (%0), %%xmm0")
                __ASM_EMIT("mulss %%xmm4, %%xmm0")
                __ASM_EMIT("addss (%1), %%xmm0")
                __ASM_EMIT("movss %%xmm0, (%1)")

                // Move pointers
                __ASM_EMIT("add $0x4, %0")
                __ASM_EMIT("add $0x4, %1")

                // Repeat loop
                __ASM_EMIT("dec %2")
                __ASM_EMIT("jnz 1b")
                __ASM_EMIT("2:")

                : "+r" (src), "+r"(dst), "+r"(count) :
                : "cc", "memory",
                  "%xmm0"
            );

            // Calculate amount of registers
            size_t regs     = count / SSE_MULTIPLE;
            count          %= SSE_MULTIPLE;

            if (regs > 0)
            {
                // Calculate amount of 4-register blocks (0x40 bytes)
                size_t blocks   = regs / 4;
                regs           %= 4;

                // Prefetch data for iteration
                __asm__ __volatile__
                (
                    __ASM_EMIT("shufps $0, %%xmm4, %%xmm4")

                    __ASM_EMIT("prefetchnta 0x00(%0)")
                    __ASM_EMIT("prefetchnta 0x20(%0)")
                    __ASM_EMIT("prefetchnta 0x00(%1)")
                    __ASM_EMIT("prefetchnta 0x20(%1)")
                    : : "r" (src), "r" (dst)
                    : "%xmm4"
                );

                if (sse_aligned(src))
                {
                    if (blocks > 0)
                    {
                        __asm__ __volatile__
                        (
                            // Prepare registers
                            __ASM_EMIT("movaps %%xmm4, %%xmm5")
                            __ASM_EMIT("movaps %%xmm4, %%xmm6")
                            __ASM_EMIT("movaps %%xmm5, %%xmm7")

                            __ASM_EMIT("1:")
                            // Prefetch next data
                            __ASM_EMIT("prefetchnta 0x40(%0)")
                            __ASM_EMIT("prefetchnta 0x60(%0)")
                            __ASM_EMIT("prefetchnta 0x40(%1)")
                            __ASM_EMIT("prefetchnta 0x60(%1)")

                            // Calculate
                            __ASM_EMIT("movaps 0x00(%0), %%xmm0")
                            __ASM_EMIT("movaps 0x10(%0), %%xmm1")
                            __ASM_EMIT("movaps 0x20(%0), %%xmm2")
                            __ASM_EMIT("movaps 0x30(%0), %%xmm3")

                            __ASM_EMIT("mulps %%xmm4, %%xmm0")
                            __ASM_EMIT("mulps %%xmm5, %%xmm1")
                            __ASM_EMIT("mulps %%xmm6, %%xmm2")
                            __ASM_EMIT("mulps %%xmm7, %%xmm3")

                            __ASM_EMIT("addps 0x00(%1), %%xmm0")
                            __ASM_EMIT("addps 0x10(%1), %%xmm1")
                            __ASM_EMIT("addps 0x20(%1), %%xmm2")
                            __ASM_EMIT("addps 0x30(%1), %%xmm3")

                            __ASM_EMIT(MOVNTPS " %%xmm0,  0x00(%1)")
                            __ASM_EMIT(MOVNTPS " %%xmm1,  0x10(%1)")
                            __ASM_EMIT(MOVNTPS " %%xmm2,  0x20(%1)")
                            __ASM_EMIT(MOVNTPS " %%xmm3,  0x30(%1)")

                            // Move pointers
                            __ASM_EMIT("add $0x40, %0")
                            __ASM_EMIT("add $0x40, %1")
                            __ASM_EMIT("dec %2")
                            __ASM_EMIT("jnz 1b")

                            : "+r" (src), "+r"(dst), "+r"(blocks) :
                            : "cc", "memory",
                              "%xmm0", "%xmm1", "%xmm2", "%xmm3"
                        );
                    }

                    if (regs > 0)
                    {
                        __asm__ __volatile__
                        (
                            __ASM_EMIT("1:")
                            // Copy data
                            __ASM_EMIT("movaps (%0), %%xmm0")
                            __ASM_EMIT("mulps %%xmm4, %%xmm0")
                            __ASM_EMIT("addps (%1), %%xmm0")
                            __ASM_EMIT(MOVNTPS " %%xmm0, (%1)")

                            // Move pointers
                            __ASM_EMIT("add $0x10, %0")
                            __ASM_EMIT("add $0x10, %1")
                            __ASM_EMIT("dec %2")
                            __ASM_EMIT("jnz 1b")

                            : "+r" (src), "+r"(dst), "+r"(regs) :
                            : "cc", "memory",
                              "%xmm0"
                        );
                    }
                }
                else
                {
                    if (blocks > 0)
                    {
                        __asm__ __volatile__
                        (
                            // Prepare registers
                            __ASM_EMIT("movaps %%xmm4, %%xmm5")
                            __ASM_EMIT("movaps %%xmm4, %%xmm6")
                            __ASM_EMIT("movaps %%xmm5, %%xmm7")

                            __ASM_EMIT("1:")
                            // Prefetch next data
                            __ASM_EMIT("prefetchnta 0x40(%0)")
                            __ASM_EMIT("prefetchnta 0x60(%0)")
                            __ASM_EMIT("prefetchnta 0x40(%1)")
                            __ASM_EMIT("prefetchnta 0x60(%1)")

                            // Calculate
                            __ASM_EMIT("movups 0x00(%0), %%xmm0")
                            __ASM_EMIT("movups 0x10(%0), %%xmm1")
                            __ASM_EMIT("movups 0x20(%0), %%xmm2")
                            __ASM_EMIT("movups 0x30(%0), %%xmm3")

                            __ASM_EMIT("mulps %%xmm4, %%xmm0")
                            __ASM_EMIT("mulps %%xmm5, %%xmm1")
                            __ASM_EMIT("mulps %%xmm6, %%xmm2")
                            __ASM_EMIT("mulps %%xmm7, %%xmm3")

                            __ASM_EMIT("addps 0x00(%1), %%xmm0")
                            __ASM_EMIT("addps 0x10(%1), %%xmm1")
                            __ASM_EMIT("addps 0x20(%1), %%xmm2")
                            __ASM_EMIT("addps 0x30(%1), %%xmm3")

                            __ASM_EMIT(MOVNTPS " %%xmm0,  0x00(%1)")
                            __ASM_EMIT(MOVNTPS " %%xmm1,  0x10(%1)")
                            __ASM_EMIT(MOVNTPS " %%xmm2,  0x20(%1)")
                            __ASM_EMIT(MOVNTPS " %%xmm3,  0x30(%1)")

                            // Move pointers
                            __ASM_EMIT("add $0x40, %0")
                            __ASM_EMIT("add $0x40, %1")

                            // Repeat loop
                            __ASM_EMIT("dec %2")
                            __ASM_EMIT("jnz 1b")

                            : "+r" (src), "+r"(dst), "+r"(blocks) :
                            : "cc", "memory",
                              "%xmm0", "%xmm1", "%xmm2", "%xmm3"
                        );
                    }

                    if (regs > 0)
                    {
                        __asm__ __volatile__
                        (
                            __ASM_EMIT("1:")

                            // Process data
                            __ASM_EMIT("movups (%0), %%xmm0")
                            __ASM_EMIT("mulps %%xmm4, %%xmm0")
                            __ASM_EMIT("addps (%1), %%xmm0")
                            __ASM_EMIT(MOVNTPS " %%xmm0, (%1)")

                            // Move pointers
                            __ASM_EMIT("add $0x10, %0")
                            __ASM_EMIT("add $0x10, %1")

                            // Repeat loop
                            __ASM_EMIT("dec %2")
                            __ASM_EMIT("jnz 1b")

                            : "+r" (src), "+r"(dst), "+r"(regs) :
                            : "cc", "memory",
                              "%xmm0"
                        );
                    }
                }
            }

            // Process tail
            if (count > 0)
            {
                __asm__ __volatile__
                (
                    __ASM_EMIT("1:")

                    // Process data
                    __ASM_EMIT("movss (%0), %%xmm0")
                    __ASM_EMIT("mulss %%xmm4, %%xmm0")
                    __ASM_EMIT("addss (%1), %%xmm0")
                    __ASM_EMIT("movss %%xmm0, (%1)")

                    // Move pointers
                    __ASM_EMIT("add $0x4, %0")
                    __ASM_EMIT("add $0x4, %1")

                    // Repeat loop
                    __ASM_EMIT("dec %2")
                    __ASM_EMIT("jnz 1b")

                    : "+r" (src), "+r"(dst), "+r"(count) :
                    : "cc", "memory",
                      "%xmm0"
                );
            }

            SFENCE;
        }

        static void mix(float *dst, const float *src1, const float *src2, float k1, float k2, size_t count)
        {
            if (count == 0)
                return;

            __asm__ __volatile__
            (
                __ASM_EMIT("movss %[k1], %%xmm3")
                __ASM_EMIT("movss %[k2], %%xmm7")
                : : [k1] "x" (k1) , [k2] "x"(k2)
                : "%xmm3", "%xmm7"
            );

            // Align destination
            __asm__ __volatile__
            (
                __ASM_EMIT("1:")
                __ASM_EMIT("test $0xf, %[dst]")
                __ASM_EMIT("jz 2f")

                // Make processing
                __ASM_EMIT("movss (%[src1]), %%xmm0")
                __ASM_EMIT("movss (%[src2]), %%xmm4")
                __ASM_EMIT("mulss %%xmm3, %%xmm0")
                __ASM_EMIT("mulss %%xmm7, %%xmm4")
                __ASM_EMIT("addss %%xmm4, %%xmm0")
                __ASM_EMIT("movss %%xmm0, (%[dst])")

                // Move pointers
                __ASM_EMIT("add $0x4, %[src1]")
                __ASM_EMIT("add $0x4, %[src2]")
                __ASM_EMIT("add $0x4, %[dst]")

                // Repeat loop
                __ASM_EMIT("dec %[count]")
                __ASM_EMIT("jnz 1b")
                __ASM_EMIT("2:")

                : [src1] "+r" (src1), [src2] "+r"(src2), [dst] "+r"(dst), [count] "+r"(count) :
                : "cc", "memory",
                  "%xmm0", "%xmm4"
            );

            // Calculate amount of registers
            size_t regs     = count / SSE_MULTIPLE;
            count          %= SSE_MULTIPLE;

            if (regs > 0)
            {
                // Calculate amount of 2-register blocks (0x20 bytes)
                size_t blocks   = regs / 2;
                regs           %= 2;

                // Prefetch data for iteration
                __asm__ __volatile__
                (
                    __ASM_EMIT("shufps $0, %%xmm3, %%xmm3")
                    __ASM_EMIT("shufps $0, %%xmm7, %%xmm7")

                    __ASM_EMIT("prefetchnta 0x00(%[src1])")
                    __ASM_EMIT("prefetchnta 0x00(%[src2])")
                    :
                    : [src1] "r" (src1), [src2] "r" (src2)
                    : "%xmm3", "%xmm7"
                );

                if (sse_aligned(src1) && (sse_aligned(src2)))
                {
                    if (blocks > 0)
                    {
                        __asm__ __volatile__
                        (
                            // Prepare registers
                            __ASM_EMIT("movaps %%xmm3, %%xmm2")
                            __ASM_EMIT("movaps %%xmm7, %%xmm6")

                            __ASM_EMIT("1:")

                            // Prefetch next data
                            __ASM_EMIT("prefetchnta 0x20(%[src1])")
                            __ASM_EMIT("prefetchnta 0x20(%[src2])")

                            // Calculate
                            __ASM_EMIT("movaps 0x00(%[src1]), %%xmm0")
                            __ASM_EMIT("movaps 0x10(%[src1]), %%xmm1")
                            __ASM_EMIT("movaps 0x00(%[src2]), %%xmm4")
                            __ASM_EMIT("movaps 0x10(%[src2]), %%xmm5")

                            __ASM_EMIT("mulps %%xmm2, %%xmm0")
                            __ASM_EMIT("mulps %%xmm3, %%xmm1")
                            __ASM_EMIT("mulps %%xmm6, %%xmm4")
                            __ASM_EMIT("mulps %%xmm7, %%xmm5")

                            __ASM_EMIT("addps %%xmm4, %%xmm0")
                            __ASM_EMIT("addps %%xmm5, %%xmm1")

                            __ASM_EMIT(MOVNTPS " %%xmm0,  0x00(%[dst])")
                            __ASM_EMIT(MOVNTPS " %%xmm1,  0x10(%[dst])")

                            // Move pointers
                            __ASM_EMIT("add $0x20, %[src1]")
                            __ASM_EMIT("add $0x20, %[src2]")
                            __ASM_EMIT("add $0x20, %[dst]")
                            __ASM_EMIT("dec %[blocks]")
                            __ASM_EMIT("jnz 1b")

                            : [src1] "+r" (src1), [src2] "+r"(src2), [dst] "+r"(dst), [blocks] "+r"(blocks)
                            :
                            : "cc", "memory",
                              "%xmm0", "%xmm1", "%xmm2", "%xmm4", "%xmm5", "%xmm6"
                        );
                    }

                    if (regs > 0)
                    {
                        __asm__ __volatile__
                        (
                            // Copy data
                            __ASM_EMIT("movaps (%[src1]), %%xmm0")
                            __ASM_EMIT("movaps (%[src2]), %%xmm4")
                            __ASM_EMIT("mulps %%xmm3, %%xmm0")
                            __ASM_EMIT("mulps %%xmm7, %%xmm4")
                            __ASM_EMIT("addps %%xmm4, %%xmm0")
                            __ASM_EMIT(MOVNTPS " %%xmm0, (%[dst])")

                            // Move pointers
                            __ASM_EMIT("add $0x10, %[src1]")
                            __ASM_EMIT("add $0x10, %[src2]")
                            __ASM_EMIT("add $0x10, %[dst]")

                            : [src1] "+r" (src1), [src2] "+r"(src2), [dst] "+r"(dst) :
                            : "cc", "memory",
                              "%xmm0", "%xmm4"
                        );
                    }
                }
                else
                {
                    if (blocks > 0)
                    {
                        __asm__ __volatile__
                        (
                            // Prepare registers
                            __ASM_EMIT("movaps %%xmm3, %%xmm2")
                            __ASM_EMIT("movaps %%xmm7, %%xmm6")

                            __ASM_EMIT("1:")

                            // Prefetch next data
                            __ASM_EMIT("prefetchnta 0x20(%[src1])")
                            __ASM_EMIT("prefetchnta 0x20(%[src2])")

                            // Calculate
                            __ASM_EMIT("movups 0x00(%[src1]), %%xmm0")
                            __ASM_EMIT("movups 0x10(%[src1]), %%xmm1")
                            __ASM_EMIT("movups 0x00(%[src2]), %%xmm4")
                            __ASM_EMIT("movups 0x10(%[src2]), %%xmm5")

                            __ASM_EMIT("mulps %%xmm2, %%xmm0")
                            __ASM_EMIT("mulps %%xmm3, %%xmm1")
                            __ASM_EMIT("mulps %%xmm6, %%xmm4")
                            __ASM_EMIT("mulps %%xmm7, %%xmm5")

                            __ASM_EMIT("addps %%xmm4, %%xmm0")
                            __ASM_EMIT("addps %%xmm5, %%xmm1")

                            __ASM_EMIT(MOVNTPS " %%xmm0,  0x00(%[dst])")
                            __ASM_EMIT(MOVNTPS " %%xmm1,  0x10(%[dst])")

                            // Move pointers
                            __ASM_EMIT("add $0x20, %[src1]")
                            __ASM_EMIT("add $0x20, %[src2]")
                            __ASM_EMIT("add $0x20, %[dst]")
                            __ASM_EMIT("dec %[blocks]")
                            __ASM_EMIT("jnz 1b")

                            : [src1] "+r" (src1), [src2] "+r"(src2), [dst] "+r"(dst), [blocks] "+r"(blocks) :
                            : "cc", "memory",
                              "%xmm0", "%xmm1", "%xmm2", "%xmm4", "%xmm5", "%xmm6"
                        );
                    }

                    if (regs > 0)
                    {
                        __asm__ __volatile__
                        (
                            // Copy data
                            __ASM_EMIT("movups (%[src1]), %%xmm0")
                            __ASM_EMIT("movups (%[src2]), %%xmm4")
                            __ASM_EMIT("mulps %%xmm3, %%xmm0")
                            __ASM_EMIT("mulps %%xmm7, %%xmm4")
                            __ASM_EMIT("addps %%xmm4, %%xmm0")
                            __ASM_EMIT(MOVNTPS " %%xmm0, (%[dst])")

                            // Move pointers
                            __ASM_EMIT("add $0x10, %[src1]")
                            __ASM_EMIT("add $0x10, %[src2]")
                            __ASM_EMIT("add $0x10, %[dst]")

                            : [src1] "+r" (src1), [src2] "+r"(src2), [dst] "+r"(dst) :
                            : "cc", "memory",
                              "%xmm0", "%xmm4"
                        );
                    }
                }
            }

            // Process tail
            __asm__ __volatile__
            (
                __ASM_EMIT("test %[count], %[count]")
                __ASM_EMIT("jz 2f")
                __ASM_EMIT("1:")

                // Process data
                __ASM_EMIT("movss (%[src1]), %%xmm0")
                __ASM_EMIT("movss (%[src2]), %%xmm4")
                __ASM_EMIT("mulss %%xmm3, %%xmm0")
                __ASM_EMIT("mulss %%xmm7, %%xmm4")
                __ASM_EMIT("addss %%xmm4, %%xmm0")
                __ASM_EMIT("movss %%xmm0, (%[dst])")

                // Move pointers
                __ASM_EMIT("add $0x4, %[src1]")
                __ASM_EMIT("add $0x4, %[src2]")
                __ASM_EMIT("add $0x4, %[dst]")

                // Repeat loop
                __ASM_EMIT("dec %[count]")
                __ASM_EMIT("jnz 1b")

                __ASM_EMIT("2:")

                : [src1] "+r" (src1), [src2] "+r"(src2), [dst] "+r"(dst), [count] "+r"(count) :
                : "cc", "memory",
                  "%xmm0", "%xmm4"
            );

            SFENCE;
        }

        static void mix_add(float *dst, const float *src1, const float *src2, float k1, float k2, size_t count)
        {
            // xmm0 = src1[i]
            // xmm1 = src1[i+4]
            // xmm2 = src2[i]
            // xmm3 = src2[i+4]
            // xmm4 = dst[i]
            // xmm5 = dst[i+4]
            // xmm6 = k1
            // xmm7 = k2

            // xmm4 + xmm0 * xmm6 + xmm2 * xmm7
            // xmm5 + xmm1 * xmm6 + xmm3 * xmm7

            if (count == 0)
                return;

            __asm__ __volatile__
            (
                __ASM_EMIT("movss %0, %%xmm6")
                __ASM_EMIT("movss %1, %%xmm7")
                : : "x" (k1) , "x"(k2)
                : "%xmm6", "%xmm7"
            );

            // Align destination
            __asm__ __volatile__
            (
                __ASM_EMIT("1:")

                // Check conditions
                __ASM_EMIT("test $0xf, %2")
                __ASM_EMIT("jz 2f")

                // Calculate data
                __ASM_EMIT("movss (%0), %%xmm0")
                __ASM_EMIT("movss (%1), %%xmm2")
                __ASM_EMIT("movss (%2), %%xmm4")
                __ASM_EMIT("mulss %%xmm6, %%xmm0")
                __ASM_EMIT("mulss %%xmm7, %%xmm2")
                __ASM_EMIT("addss %%xmm4, %%xmm0")
                __ASM_EMIT("addss %%xmm2, %%xmm0")
                __ASM_EMIT("movss %%xmm0, (%2)")

                // Move pointers
                __ASM_EMIT("add $0x4, %0")
                __ASM_EMIT("add $0x4, %1")
                __ASM_EMIT("add $0x4, %2")

                // Repeat loop
                __ASM_EMIT("dec %3")
                __ASM_EMIT("jnz 1b")
                __ASM_EMIT("2:")

                : "+r" (src1), "+r"(src2), "+r"(dst), "+r"(count) :
                : "cc", "memory",
                  "%xmm0", "%xmm2", "%xmm4"
            );

            // Calculate amount of registers
            size_t regs     = count / SSE_MULTIPLE;
            count          %= SSE_MULTIPLE;

            if (regs > 0)
            {
                // Calculate amount of 4-register blocks (0x40 bytes)
                size_t blocks   = regs / 2;
                regs           %= 2;

                // Prefetch data for iteration
                __asm__ __volatile__
                (
                    __ASM_EMIT("shufps $0, %%xmm6, %%xmm6")
                    __ASM_EMIT("shufps $0, %%xmm7, %%xmm7")

                    __ASM_EMIT("prefetchnta 0x00(%0)")
                    __ASM_EMIT("prefetchnta 0x00(%1)")
                    __ASM_EMIT("prefetchnta 0x00(%2)")
                    :
                    : "r" (src1), "r" (src2), "r"(dst)
                    : "%xmm6", "%xmm7"
                );

                if (sse_aligned(src1) && (sse_aligned(src2)))
                {
                    if (blocks > 0)
                    {
                        __asm__ __volatile__
                        (
                            __ASM_EMIT("1:")

                            // Prefetch next data
                            __ASM_EMIT("prefetchnta 0x20(%0)")
                            __ASM_EMIT("prefetchnta 0x20(%1)")
                            __ASM_EMIT("prefetchnta 0x20(%2)")

                            // Calculate
                            __ASM_EMIT("movaps 0x00(%0), %%xmm0")
                            __ASM_EMIT("movaps 0x10(%0), %%xmm1")
                            __ASM_EMIT("movaps 0x00(%1), %%xmm2")
                            __ASM_EMIT("movaps 0x10(%1), %%xmm3")
                            __ASM_EMIT("movaps 0x00(%2), %%xmm4")
                            __ASM_EMIT("movaps 0x10(%2), %%xmm5")

                            __ASM_EMIT("mulps %%xmm6, %%xmm0")
                            __ASM_EMIT("mulps %%xmm7, %%xmm2")
                            __ASM_EMIT("mulps %%xmm6, %%xmm1")
                            __ASM_EMIT("mulps %%xmm7, %%xmm3")

                            __ASM_EMIT("addps %%xmm4, %%xmm0")
                            __ASM_EMIT("addps %%xmm5, %%xmm1")
                            __ASM_EMIT("addps %%xmm2, %%xmm0")
                            __ASM_EMIT("addps %%xmm3, %%xmm1")

                            __ASM_EMIT(MOVNTPS " %%xmm0, 0x00(%2)")
                            __ASM_EMIT(MOVNTPS " %%xmm1, 0x10(%2)")

                            // Move pointers
                            __ASM_EMIT("add $0x20, %0")
                            __ASM_EMIT("add $0x20, %1")
                            __ASM_EMIT("add $0x20, %2")
                            __ASM_EMIT("dec %3")
                            __ASM_EMIT("jnz 1b")

                            : "+r" (src1), "+r"(src2), "+r"(dst), "+r"(blocks) :
                            : "cc", "memory",
                              "%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5"
                        );
                    }

                    if (regs > 0)
                    {
                        __asm__ __volatile__
                        (
                            // Copy data
                            __ASM_EMIT("movaps (%0), %%xmm0")
                            __ASM_EMIT("movaps (%1), %%xmm2")
                            __ASM_EMIT("movaps (%2), %%xmm4")
                            __ASM_EMIT("mulps %%xmm6, %%xmm0")
                            __ASM_EMIT("mulps %%xmm7, %%xmm2")
                            __ASM_EMIT("addps %%xmm4, %%xmm0")
                            __ASM_EMIT("addps %%xmm2, %%xmm0")
                            __ASM_EMIT(MOVNTPS " %%xmm0, (%2)")

                            // Move pointers
                            __ASM_EMIT("add $0x10, %0")
                            __ASM_EMIT("add $0x10, %1")
                            __ASM_EMIT("add $0x10, %2")

                            : "+r" (src1), "+r"(src2), "+r"(dst) :
                            : "cc", "memory",
                              "%xmm0", "%xmm2", "%xmm4"
                        );
                    }
                }
                else
                {
                    if (blocks > 0)
                    {
                        __asm__ __volatile__
                        (
                            __ASM_EMIT("1:")

                            // Prefetch next data
                            __ASM_EMIT("prefetchnta 0x20(%0)")
                            __ASM_EMIT("prefetchnta 0x20(%1)")
                            __ASM_EMIT("prefetchnta 0x20(%2)")

                            // Calculate
                            __ASM_EMIT("movups 0x00(%0), %%xmm0")
                            __ASM_EMIT("movups 0x10(%0), %%xmm1")
                            __ASM_EMIT("movups 0x00(%1), %%xmm2")
                            __ASM_EMIT("movups 0x10(%1), %%xmm3")
                            __ASM_EMIT("movaps 0x00(%2), %%xmm4")
                            __ASM_EMIT("movaps 0x10(%2), %%xmm5")

                            __ASM_EMIT("mulps %%xmm6, %%xmm0")
                            __ASM_EMIT("mulps %%xmm7, %%xmm2")
                            __ASM_EMIT("mulps %%xmm6, %%xmm1")
                            __ASM_EMIT("mulps %%xmm7, %%xmm3")

                            __ASM_EMIT("addps %%xmm4, %%xmm0")
                            __ASM_EMIT("addps %%xmm5, %%xmm1")
                            __ASM_EMIT("addps %%xmm2, %%xmm0")
                            __ASM_EMIT("addps %%xmm3, %%xmm1")

                            __ASM_EMIT(MOVNTPS " %%xmm0, 0x00(%2)")
                            __ASM_EMIT(MOVNTPS " %%xmm1, 0x10(%2)")

                            // Move pointers
                            __ASM_EMIT("add $0x20, %0")
                            __ASM_EMIT("add $0x20, %1")
                            __ASM_EMIT("add $0x20, %2")
                            __ASM_EMIT("dec %3")
                            __ASM_EMIT("jnz 1b")

                            : "+r" (src1), "+r"(src2), "+r"(dst), "+r"(blocks) :
                            : "cc", "memory",
                              "%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5"
                        );
                    }

                    if (regs > 0)
                    {
                        __asm__ __volatile__
                        (
                            // Copy data
                            __ASM_EMIT("movups (%0), %%xmm0")
                            __ASM_EMIT("movups (%1), %%xmm2")
                            __ASM_EMIT("movaps (%2), %%xmm4")
                            __ASM_EMIT("mulps %%xmm6, %%xmm0")
                            __ASM_EMIT("mulps %%xmm7, %%xmm2")
                            __ASM_EMIT("addps %%xmm4, %%xmm0")
                            __ASM_EMIT("addps %%xmm2, %%xmm0")
                            __ASM_EMIT(MOVNTPS " %%xmm0, (%2)")

                            // Move pointers
                            __ASM_EMIT("add $0x10, %0")
                            __ASM_EMIT("add $0x10, %1")
                            __ASM_EMIT("add $0x10, %2")

                            : "+r" (src1), "+r"(src2), "+r"(dst) :
                            : "cc", "memory",
                              "%xmm0", "%xmm2", "%xmm4"
                        );
                    }
                }
            }

            // Process tail
            __asm__ __volatile__
            (
                __ASM_EMIT("test %3, %3")
                __ASM_EMIT("jz 2f")
                __ASM_EMIT("1:")

                // Process data
                __ASM_EMIT("movss (%0), %%xmm0")
                __ASM_EMIT("movss (%1), %%xmm2")
                __ASM_EMIT("movss (%2), %%xmm4")
                __ASM_EMIT("mulss %%xmm6, %%xmm0")
                __ASM_EMIT("mulss %%xmm7, %%xmm2")
                __ASM_EMIT("addss %%xmm4, %%xmm0")
                __ASM_EMIT("addss %%xmm2, %%xmm0")
                __ASM_EMIT("movss %%xmm0, (%2)")

                // Move pointers
                __ASM_EMIT("add $0x4, %0")
                __ASM_EMIT("add $0x4, %1")
                __ASM_EMIT("add $0x4, %2")

                // Repeat loop
                __ASM_EMIT("dec %3")
                __ASM_EMIT("jnz 1b")

                __ASM_EMIT("2:")

                : "+r" (src1), "+r"(src2), "+r"(dst), "+r"(count) :
                : "cc", "memory",
                  "%xmm0", "%xmm2", "%xmm4"
            );

            SFENCE;
        }

        static float convolve_single(const float *src, const float *conv, size_t count)
        {
            // Initialize counter
            float result;
            __asm__ __volatile__
            (
                __ASM_EMIT("xorps %0, %0")
                : "=Yz"(result) : :
            );

            __asm__ __volatile__
            (
                __ASM_EMIT("1:")

                // Check conditions
                __ASM_EMIT("test $0x0f, %1")
                __ASM_EMIT("jz 2f")

                // Process data
                __ASM_EMIT("movss (%0), %%xmm1")
                __ASM_EMIT("movss (%1), %%xmm2")
                __ASM_EMIT("mulss %%xmm2, %%xmm1")
                __ASM_EMIT("addss %%xmm1, %%xmm0")

                // Move pointers
                __ASM_EMIT("add $0x4, %0")
                __ASM_EMIT("add $0x4, %1")

                // Repeat loop
                __ASM_EMIT("dec %2")
                __ASM_EMIT("jnz 1b")
                __ASM_EMIT("2:")

                : "+r" (src), "+r"(conv), "+r" (count), "=Yz"(result) :
                : "cc", "memory",
                  "%xmm1", "%xmm2"
            );

            // Now conv is aligned, calculate number of SSE-registers
            size_t regs     = count / SSE_MULTIPLE;
            count          %= SSE_MULTIPLE;

            if (regs > 0)
            {
                // Calculate amount of 4-register blocks (0x40 bytes)
                size_t blocks   = regs / 4;
                regs           %= 4;

                // Prefetch data for iteration
                __asm__ __volatile__
                (
                    __ASM_EMIT("prefetchnta  0x00(%0)")
                    __ASM_EMIT("prefetchnta  0x20(%0)")
                    __ASM_EMIT("prefetchnta  0x00(%1)")
                    __ASM_EMIT("prefetchnta  0x20(%1)")
                    : : "r" (src), "r" (conv)
                );

                if (sse_aligned(src))
                {
                    if (blocks > 0)
                    {
                        __asm__ __volatile__
                        (
                            __ASM_EMIT("1:")
                            // Prefetch next data
                            __ASM_EMIT("prefetchnta 0x40(%0)")
                            __ASM_EMIT("prefetchnta 0x60(%0)")
                            __ASM_EMIT("prefetchnta 0x40(%1)")
                            __ASM_EMIT("prefetchnta 0x60(%1)")

                            // Process data
                            __ASM_EMIT("movaps 0x00(%0), %%xmm1")
                            __ASM_EMIT("movaps 0x10(%0), %%xmm2")
                            __ASM_EMIT("movaps 0x00(%1), %%xmm3")
                            __ASM_EMIT("movaps 0x10(%1), %%xmm4")

                            __ASM_EMIT("mulps  %%xmm3, %%xmm1")
                            __ASM_EMIT("mulps  %%xmm4, %%xmm2")

                            __ASM_EMIT("movaps 0x20(%0), %%xmm3")
                            __ASM_EMIT("movaps 0x30(%0), %%xmm4")
                            __ASM_EMIT("movaps 0x20(%1), %%xmm5")
                            __ASM_EMIT("movaps 0x30(%1), %%xmm6")

                            __ASM_EMIT("mulps  %%xmm5, %%xmm3")
                            __ASM_EMIT("mulps  %%xmm6, %%xmm4")

                            __ASM_EMIT("addps  %%xmm3, %%xmm1")
                            __ASM_EMIT("addps  %%xmm4, %%xmm2")
                            __ASM_EMIT("addps  %%xmm1, %%xmm0")
                            __ASM_EMIT("addps  %%xmm2, %%xmm0")

                            // Move pointers
                            __ASM_EMIT("add $0x40, %0")
                            __ASM_EMIT("add $0x40, %1")

                            // Repeat loop
                            __ASM_EMIT("dec %2")
                            __ASM_EMIT("jnz 1b")

                            : "+r" (src), "+r"(conv), "+r"(blocks) :
                            : "cc", "memory",
                              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                              "%xmm4", "%xmm5", "%xmm6"
                        );
                    }

                    if (regs > 0)
                    {
                        __asm__ __volatile__
                        (
                            __ASM_EMIT("1:")
                            // Copy data
                            __ASM_EMIT("movaps (%0), %%xmm1")
                            __ASM_EMIT("movaps (%1), %%xmm2")
                            __ASM_EMIT("mulps %%xmm2, %%xmm1")
                            __ASM_EMIT("addps %%xmm1, %%xmm0")

                            // Move pointers
                            __ASM_EMIT("add $0x10, %0")
                            __ASM_EMIT("add $0x10, %1")

                            // Repeat loop
                            __ASM_EMIT("dec %2")
                            __ASM_EMIT("jnz 1b")

                            : "+r" (src), "+r"(conv), "+r"(regs) :
                            : "cc", "memory",
                              "%xmm0"
                        );
                    }
                }
                else
                {
                    if (blocks > 0)
                    {
                        __asm__ __volatile__
                        (
                            __ASM_EMIT("1:")
                            // Prefetch next data
                            __ASM_EMIT("prefetchnta 0x40(%0)")
                            __ASM_EMIT("prefetchnta 0x60(%0)")
                            __ASM_EMIT("prefetchnta 0x40(%1)")
                            __ASM_EMIT("prefetchnta 0x60(%1)")

                            // Process data
                            __ASM_EMIT("movups 0x00(%0), %%xmm1")
                            __ASM_EMIT("movups 0x10(%0), %%xmm2")
                            __ASM_EMIT("movaps 0x00(%1), %%xmm3")
                            __ASM_EMIT("movaps 0x10(%1), %%xmm4")

                            __ASM_EMIT("mulps  %%xmm3, %%xmm1")
                            __ASM_EMIT("mulps  %%xmm4, %%xmm2")

                            __ASM_EMIT("movups 0x20(%0), %%xmm3")
                            __ASM_EMIT("movups 0x30(%0), %%xmm4")
                            __ASM_EMIT("movaps 0x20(%1), %%xmm5")
                            __ASM_EMIT("movaps 0x30(%1), %%xmm6")

                            __ASM_EMIT("mulps  %%xmm5, %%xmm3")
                            __ASM_EMIT("mulps  %%xmm6, %%xmm4")

                            __ASM_EMIT("addps  %%xmm3, %%xmm1")
                            __ASM_EMIT("addps  %%xmm4, %%xmm2")
                            __ASM_EMIT("addps  %%xmm1, %%xmm0")
                            __ASM_EMIT("addps  %%xmm2, %%xmm0")

                            // Move pointers
                            __ASM_EMIT("add $0x40, %0")
                            __ASM_EMIT("add $0x40, %1")

                            // Repeat loop
                            __ASM_EMIT("dec %2")
                            __ASM_EMIT("jnz 1b")

                            : "+r" (src), "+r"(conv), "+r"(blocks) :
                            : "cc", "memory",
                              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                              "%xmm4", "%xmm5", "%xmm6"
                        );
                    }

                    if (regs > 0)
                    {
                        __asm__ __volatile__
                        (
                            __ASM_EMIT("1:")
                            // Copy data
                            __ASM_EMIT("movups (%0), %%xmm1")
                            __ASM_EMIT("movaps (%1), %%xmm2")
                            __ASM_EMIT("mulps %%xmm2, %%xmm1")
                            __ASM_EMIT("addps %%xmm1, %%xmm0")

                            // Move pointers
                            __ASM_EMIT("add $0x10, %0")
                            __ASM_EMIT("add $0x10, %1")

                            // Repeat loop
                            __ASM_EMIT("dec %2")
                            __ASM_EMIT("jnz 1b")

                            : "+r" (src), "+r"(conv), "+r"(regs) :
                            : "cc", "memory",
                              "%xmm0"
                        );
                    }
                }

                __asm__ __volatile__
                (
                    // Horizontaly sum xmm0
                    __ASM_EMIT("movhlps %%xmm0, %%xmm1")
                    __ASM_EMIT("addps %%xmm1, %%xmm0")
                    __ASM_EMIT("movaps %%xmm0, %%xmm1")
                    __ASM_EMIT("shufps $0x55, %%xmm0, %%xmm0")
                    __ASM_EMIT("addps %%xmm1, %%xmm0")
                    : "=Yz" (result)
                    : : "%xmm1"
                );
            }

            // Process tail
            if (count > 0)
            {
                __asm__ __volatile__
                (
                    __ASM_EMIT("1:")

                    // Process data
                    __ASM_EMIT("movss (%0), %%xmm1")
                    __ASM_EMIT("movss (%1), %%xmm2")
                    __ASM_EMIT("mulss %%xmm2, %%xmm1")
                    __ASM_EMIT("addss %%xmm1, %%xmm0")

                    // Move pointers
                    __ASM_EMIT("add $0x4, %0")
                    __ASM_EMIT("add $0x4, %1")

                    // Repeat loop
                    __ASM_EMIT("dec %2")
                    __ASM_EMIT("jnz 1b")
                    __ASM_EMIT("2:")

                    : "+r" (src), "+r"(conv), "+r" (count), "=Yz"(result) :
                    : "cc", "memory",
                      "%xmm1", "%xmm2"
                );
            }

            return result;
        }

        static void reverse(float *dst, size_t count)
        {
            if (count < 2)
                return;
            float *src      = &dst[count];
            count >>= 1;

            size_t regs     = count / SSE_MULTIPLE;
            count          %= SSE_MULTIPLE;

            if (regs > 0)
            {
                // Calculate number of blocks (2 blocks x 0x40 bytes)
                size_t blocks   = regs / 4;
                regs           %= 4;

                __asm__ __volatile__
                (
                    __ASM_EMIT("prefetchnta 0x00(%0)")
                    __ASM_EMIT("prefetchnta 0x20(%0)")
                    __ASM_EMIT("prefetchnta -0x40(%1)")
                    __ASM_EMIT("prefetchnta -0x20(%1)")
                    : : "r" (dst), "r" (src)
                );

                if (sse_aligned(src))
                {
                    if (sse_aligned(dst))
                    {
                        if (blocks > 0)
                        {
                            __asm__ __volatile__
                            (
                                __ASM_EMIT("1:")

                                // Prefetch next data
                                __ASM_EMIT("prefetchnta 0x40(%0)")
                                __ASM_EMIT("prefetchnta 0x60(%0)")
                                __ASM_EMIT("prefetchnta -0x80(%1)")
                                __ASM_EMIT("prefetchnta -0x60(%1)")

                                // Process data
                                __ASM_EMIT("movaps 0x00(%0), %%xmm0")
                                __ASM_EMIT("movaps 0x10(%0), %%xmm2")
                                __ASM_EMIT("movaps 0x20(%0), %%xmm4")
                                __ASM_EMIT("movaps 0x30(%0), %%xmm6")

                                __ASM_EMIT("movaps -0x40(%1), %%xmm7")
                                __ASM_EMIT("movaps -0x30(%1), %%xmm5")
                                __ASM_EMIT("movaps -0x20(%1), %%xmm3")
                                __ASM_EMIT("movaps -0x10(%1), %%xmm1")

                                __ASM_EMIT("shufps $0x1b, %%xmm0, %%xmm0")
                                __ASM_EMIT("shufps $0x1b, %%xmm1, %%xmm1")
                                __ASM_EMIT("shufps $0x1b, %%xmm2, %%xmm2")
                                __ASM_EMIT("shufps $0x1b, %%xmm3, %%xmm3")
                                __ASM_EMIT("shufps $0x1b, %%xmm4, %%xmm4")
                                __ASM_EMIT("shufps $0x1b, %%xmm5, %%xmm5")
                                __ASM_EMIT("shufps $0x1b, %%xmm6, %%xmm6")
                                __ASM_EMIT("shufps $0x1b, %%xmm7, %%xmm7")

                                __ASM_EMIT(MOVNTPS " %%xmm1, 0x00(%0)")
                                __ASM_EMIT(MOVNTPS " %%xmm3, 0x10(%0)")
                                __ASM_EMIT(MOVNTPS " %%xmm5, 0x20(%0)")
                                __ASM_EMIT(MOVNTPS " %%xmm7, 0x30(%0)")

                                __ASM_EMIT(MOVNTPS " %%xmm6, -0x40(%1)")
                                __ASM_EMIT(MOVNTPS " %%xmm4, -0x30(%1)")
                                __ASM_EMIT(MOVNTPS " %%xmm2, -0x20(%1)")
                                __ASM_EMIT(MOVNTPS " %%xmm0, -0x10(%1)")

                                // Move pointers
                                __ASM_EMIT("add $0x40, %0")
                                __ASM_EMIT("sub $0x40, %1")

                                // Repeat loop
                                __ASM_EMIT("dec %2")
                                __ASM_EMIT("jnz 1b")

                                : "+r" (dst), "+r" (src), "+r"(blocks) :
                                : "cc", "memory",
                                    "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                                    "%xmm4", "%xmm5", "%xmm6", "%xmm7"
                            );
                        }

                        if (regs > 0)
                        {
                            __asm__ __volatile__
                            (
                                __ASM_EMIT("1:")

                                // Process data
                                __ASM_EMIT("movaps 0x00(%0), %%xmm0")
                                __ASM_EMIT("movaps -0x10(%1), %%xmm1")

                                __ASM_EMIT("shufps $0x1b, %%xmm0, %%xmm0")
                                __ASM_EMIT("shufps $0x1b, %%xmm1, %%xmm1")

                                __ASM_EMIT(MOVNTPS " %%xmm1, 0x00(%0)")
                                __ASM_EMIT(MOVNTPS " %%xmm0, -0x10(%1)")

                                // Move pointers
                                __ASM_EMIT("add $0x10, %0")
                                __ASM_EMIT("sub $0x10, %1")

                                // Repeat loop
                                __ASM_EMIT("dec %2")
                                __ASM_EMIT("jnz 1b")

                                : "+r" (dst), "+r" (src), "+r"(regs) :
                                : "cc", "memory",
                                    "%xmm0", "%xmm1"
                            );
                        }
                    }
                    else // !sse_aligned(dst)
                    {
                        if (blocks > 0)
                        {
                            __asm__ __volatile__
                            (
                                __ASM_EMIT("1:")

                                // Prefetch next data
                                __ASM_EMIT("prefetchnta 0x40(%0)")
                                __ASM_EMIT("prefetchnta 0x60(%0)")
                                __ASM_EMIT("prefetchnta -0x80(%1)")
                                __ASM_EMIT("prefetchnta -0x60(%1)")

                                // Process data
                                __ASM_EMIT("movaps 0x00(%0), %%xmm0")
                                __ASM_EMIT("movaps 0x10(%0), %%xmm2")
                                __ASM_EMIT("movaps 0x20(%0), %%xmm4")
                                __ASM_EMIT("movaps 0x30(%0), %%xmm6")

                                __ASM_EMIT("movups -0x40(%1), %%xmm7")
                                __ASM_EMIT("movups -0x30(%1), %%xmm5")
                                __ASM_EMIT("movups -0x20(%1), %%xmm3")
                                __ASM_EMIT("movups -0x10(%1), %%xmm1")

                                __ASM_EMIT("shufps $0x1b, %%xmm0, %%xmm0")
                                __ASM_EMIT("shufps $0x1b, %%xmm1, %%xmm1")
                                __ASM_EMIT("shufps $0x1b, %%xmm2, %%xmm2")
                                __ASM_EMIT("shufps $0x1b, %%xmm3, %%xmm3")
                                __ASM_EMIT("shufps $0x1b, %%xmm4, %%xmm4")
                                __ASM_EMIT("shufps $0x1b, %%xmm5, %%xmm5")
                                __ASM_EMIT("shufps $0x1b, %%xmm6, %%xmm6")
                                __ASM_EMIT("shufps $0x1b, %%xmm7, %%xmm7")

                                __ASM_EMIT(MOVNTPS " %%xmm1, 0x00(%0)")
                                __ASM_EMIT(MOVNTPS " %%xmm3, 0x10(%0)")
                                __ASM_EMIT(MOVNTPS " %%xmm5, 0x20(%0)")
                                __ASM_EMIT(MOVNTPS " %%xmm7, 0x30(%0)")

                                __ASM_EMIT("movups  %%xmm6, -0x40(%1)")
                                __ASM_EMIT("movups  %%xmm4, -0x30(%1)")
                                __ASM_EMIT("movups  %%xmm2, -0x20(%1)")
                                __ASM_EMIT("movups  %%xmm0, -0x10(%1)")

                                // Move pointers
                                __ASM_EMIT("add $0x40, %0")
                                __ASM_EMIT("sub $0x40, %1")

                                // Repeat loop
                                __ASM_EMIT("dec %2")
                                __ASM_EMIT("jnz 1b")

                                : "+r" (dst), "+r" (src), "+r"(blocks) :
                                : "cc", "memory",
                                    "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                                    "%xmm4", "%xmm5", "%xmm6", "%xmm7"
                            );
                        }

                        if (regs > 0)
                        {
                            __asm__ __volatile__
                            (
                                __ASM_EMIT("1:")

                                // Process data
                                __ASM_EMIT("movaps 0x00(%0), %%xmm0")
                                __ASM_EMIT("movups -0x10(%1), %%xmm1")

                                __ASM_EMIT("shufps $0x1b, %%xmm0, %%xmm0")
                                __ASM_EMIT("shufps $0x1b, %%xmm1, %%xmm1")

                                __ASM_EMIT(MOVNTPS " %%xmm1, 0x00(%0)")
                                __ASM_EMIT("movups  %%xmm0, -0x10(%1)")

                                // Move pointers
                                __ASM_EMIT("add $0x10, %0")
                                __ASM_EMIT("sub $0x10, %1")

                                // Repeat loop
                                __ASM_EMIT("dec %2")
                                __ASM_EMIT("jnz 1b")

                                : "+r" (dst), "+r" (src), "+r"(regs) :
                                : "cc", "memory",
                                    "%xmm0", "%xmm1"
                            );
                        }
                    }
                }
                else
                {
                    if (sse_aligned(dst))
                    {
                        if (blocks > 0)
                        {
                            __asm__ __volatile__
                            (
                                __ASM_EMIT("1:")

                                // Prefetch next data
                                __ASM_EMIT("prefetchnta 0x40(%0)")
                                __ASM_EMIT("prefetchnta 0x60(%0)")
                                __ASM_EMIT("prefetchnta -0x80(%1)")
                                __ASM_EMIT("prefetchnta -0x60(%1)")

                                // Process data
                                __ASM_EMIT("movups 0x00(%0), %%xmm0")
                                __ASM_EMIT("movups 0x10(%0), %%xmm2")
                                __ASM_EMIT("movups 0x20(%0), %%xmm4")
                                __ASM_EMIT("movups 0x30(%0), %%xmm6")

                                __ASM_EMIT("movaps -0x40(%1), %%xmm7")
                                __ASM_EMIT("movaps -0x30(%1), %%xmm5")
                                __ASM_EMIT("movaps -0x20(%1), %%xmm3")
                                __ASM_EMIT("movaps -0x10(%1), %%xmm1")

                                __ASM_EMIT("shufps $0x1b, %%xmm0, %%xmm0")
                                __ASM_EMIT("shufps $0x1b, %%xmm1, %%xmm1")
                                __ASM_EMIT("shufps $0x1b, %%xmm2, %%xmm2")
                                __ASM_EMIT("shufps $0x1b, %%xmm3, %%xmm3")
                                __ASM_EMIT("shufps $0x1b, %%xmm4, %%xmm4")
                                __ASM_EMIT("shufps $0x1b, %%xmm5, %%xmm5")
                                __ASM_EMIT("shufps $0x1b, %%xmm6, %%xmm6")
                                __ASM_EMIT("shufps $0x1b, %%xmm7, %%xmm7")

                                __ASM_EMIT("movups  %%xmm1, 0x00(%0)")
                                __ASM_EMIT("movups  %%xmm3, 0x10(%0)")
                                __ASM_EMIT("movups  %%xmm5, 0x20(%0)")
                                __ASM_EMIT("movups  %%xmm7, 0x30(%0)")

                                __ASM_EMIT(MOVNTPS " %%xmm6, -0x40(%1)")
                                __ASM_EMIT(MOVNTPS " %%xmm4, -0x30(%1)")
                                __ASM_EMIT(MOVNTPS " %%xmm2, -0x20(%1)")
                                __ASM_EMIT(MOVNTPS " %%xmm0, -0x10(%1)")

                                // Move pointers
                                __ASM_EMIT("add $0x40, %0")
                                __ASM_EMIT("sub $0x40, %1")

                                // Repeat loop
                                __ASM_EMIT("dec %2")
                                __ASM_EMIT("jnz 1b")

                                : "+r" (dst), "+r" (src), "+r"(blocks) :
                                : "cc", "memory",
                                    "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                                    "%xmm4", "%xmm5", "%xmm6", "%xmm7"
                            );
                        }

                        if (regs > 0)
                        {
                            __asm__ __volatile__
                            (
                                __ASM_EMIT("1:")

                                // Process data
                                __ASM_EMIT("movups 0x00(%0), %%xmm0")
                                __ASM_EMIT("movaps -0x10(%1), %%xmm1")

                                __ASM_EMIT("shufps $0x1b, %%xmm0, %%xmm0")
                                __ASM_EMIT("shufps $0x1b, %%xmm1, %%xmm1")

                                __ASM_EMIT("movups  %%xmm1, 0x00(%0)")
                                __ASM_EMIT(MOVNTPS " %%xmm0, -0x10(%1)")

                                // Move pointers
                                __ASM_EMIT("add $0x10, %0")
                                __ASM_EMIT("sub $0x10, %1")

                                // Repeat loop
                                __ASM_EMIT("dec %2")
                                __ASM_EMIT("jnz 1b")

                                : "+r" (dst), "+r" (src), "+r"(regs) :
                                : "cc", "memory",
                                    "%xmm0", "%xmm1"
                            );
                        }
                    }
                    else
                    {
                        if (blocks > 0)
                        {
                            __asm__ __volatile__
                            (
                                __ASM_EMIT("1:")

                                // Prefetch next data
                                __ASM_EMIT("prefetchnta 0x40(%0)")
                                __ASM_EMIT("prefetchnta 0x60(%0)")
                                __ASM_EMIT("prefetchnta -0x80(%1)")
                                __ASM_EMIT("prefetchnta -0x60(%1)")

                                // Process data
                                __ASM_EMIT("movups 0x00(%0), %%xmm0")
                                __ASM_EMIT("movups 0x10(%0), %%xmm2")
                                __ASM_EMIT("movups 0x20(%0), %%xmm4")
                                __ASM_EMIT("movups 0x30(%0), %%xmm6")

                                __ASM_EMIT("movups -0x40(%1), %%xmm7")
                                __ASM_EMIT("movups -0x30(%1), %%xmm5")
                                __ASM_EMIT("movups -0x20(%1), %%xmm3")
                                __ASM_EMIT("movups -0x10(%1), %%xmm1")

                                __ASM_EMIT("shufps $0x1b, %%xmm0, %%xmm0")
                                __ASM_EMIT("shufps $0x1b, %%xmm1, %%xmm1")
                                __ASM_EMIT("shufps $0x1b, %%xmm2, %%xmm2")
                                __ASM_EMIT("shufps $0x1b, %%xmm3, %%xmm3")
                                __ASM_EMIT("shufps $0x1b, %%xmm4, %%xmm4")
                                __ASM_EMIT("shufps $0x1b, %%xmm5, %%xmm5")
                                __ASM_EMIT("shufps $0x1b, %%xmm6, %%xmm6")
                                __ASM_EMIT("shufps $0x1b, %%xmm7, %%xmm7")

                                __ASM_EMIT("movups  %%xmm1, 0x00(%0)")
                                __ASM_EMIT("movups  %%xmm3, 0x10(%0)")
                                __ASM_EMIT("movups  %%xmm5, 0x20(%0)")
                                __ASM_EMIT("movups  %%xmm7, 0x30(%0)")

                                __ASM_EMIT("movups  %%xmm6, -0x40(%1)")
                                __ASM_EMIT("movups  %%xmm4, -0x30(%1)")
                                __ASM_EMIT("movups  %%xmm2, -0x20(%1)")
                                __ASM_EMIT("movups  %%xmm0, -0x10(%1)")

                                // Move pointers
                                __ASM_EMIT("add $0x40, %0")
                                __ASM_EMIT("sub $0x40, %1")

                                // Repeat loop
                                __ASM_EMIT("dec %2")
                                __ASM_EMIT("jnz 1b")

                                : "+r" (dst), "+r" (src), "+r"(blocks) :
                                : "cc", "memory",
                                    "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                                    "%xmm4", "%xmm5", "%xmm6", "%xmm7"
                            );
                        }

                        if (regs > 0)
                        {
                            __asm__ __volatile__
                            (
                                __ASM_EMIT("1:")

                                // Process data
                                __ASM_EMIT("movups 0x00(%0), %%xmm0")
                                __ASM_EMIT("movups -0x10(%1), %%xmm1")

                                __ASM_EMIT("shufps $0x1b, %%xmm0, %%xmm0")
                                __ASM_EMIT("shufps $0x1b, %%xmm1, %%xmm1")

                                __ASM_EMIT("movups %%xmm1, 0x00(%0)")
                                __ASM_EMIT("movups %%xmm0, -0x10(%1)")

                                // Move pointers
                                __ASM_EMIT("add $0x10, %0")
                                __ASM_EMIT("sub $0x10, %1")

                                // Repeat loop
                                __ASM_EMIT("dec %2")
                                __ASM_EMIT("jnz 1b")

                                : "+r" (dst), "+r" (src), "+r"(regs) :
                                : "cc", "memory",
                                    "%xmm0", "%xmm1"
                            );
                        }
                    }
                }
            }

            // Process tail
            if (count > 0)
            {
                __asm__ __volatile__
                (
                    __ASM_EMIT("1:")

                    // Process data
                    __ASM_EMIT("movss 0x00(%0), %%xmm0")
                    __ASM_EMIT("movss -0x04(%1), %%xmm1")
                    __ASM_EMIT("movss %%xmm1, (%0)")
                    __ASM_EMIT("movss %%xmm0, -0x04(%1)")

                    // Move pointers
                    __ASM_EMIT("add $0x4, %0")
                    __ASM_EMIT("sub $0x4, %1")

                    // Repeat loop
                    __ASM_EMIT("dec %2")
                    __ASM_EMIT("jnz 1b")
                    __ASM_EMIT("2:")

                    : "+r" (dst), "+r"(src), "+r" (count) :
                    : "cc", "memory",
                      "%xmm0", "%xmm1"
                );
            }

            SFENCE;
        }

        static void lr_to_ms(float *m, float *s, const float *l, const float *r, size_t count)
        {
            if (count == 0)
                return;

            // Head
            __asm__ __volatile__
            (
                __ASM_EMIT("movaps %[X_HALF], %%xmm3")
                __ASM_EMIT("1:")

                // Check conditions
                __ASM_EMIT("test $0x0f, %[left]")
                __ASM_EMIT("jz 2f")

                // Process data
                __ASM_EMIT("movss (%[left]),  %%xmm0")  // xmm0 = l
                __ASM_EMIT("movss (%[right]), %%xmm2")  // xmm2 = r
                __ASM_EMIT("movss %%xmm0, %%xmm1")      // xmm1 = l
                __ASM_EMIT("addss %%xmm2, %%xmm0")      // xmm0 = l + r
                __ASM_EMIT("subss %%xmm2, %%xmm1")      // xmm1 = l - r
                __ASM_EMIT("mulss %%xmm3, %%xmm0")      // xmm0 = (l + r) * 0.5f
                __ASM_EMIT("mulss %%xmm3, %%xmm1")      // xmm1 = (l - r) * 0.5f
                __ASM_EMIT("movss %%xmm0, (%[mid])")
                __ASM_EMIT("movss %%xmm1, (%[side])")

                // Move pointers
                __ASM_EMIT("add $0x4, %[left]")
                __ASM_EMIT("add $0x4, %[right]")
                __ASM_EMIT("add $0x4, %[mid]")
                __ASM_EMIT("add $0x4, %[side]")

                // Repeat loop
                __ASM_EMIT("dec %[count]")
                __ASM_EMIT("jnz 1b")
                __ASM_EMIT("2:")

                : [left] "+r"(l), [right] "+r" (r), [mid] "+r" (m), [side] "+r" (s), [count] "+r" (count)
                : [X_HALF] "m" (X_HALF)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3"
            );

            size_t regs     = count / SSE_MULTIPLE;
            count          %= SSE_MULTIPLE;

            // Body
            if (regs > 0)
            {
                // Prefetch data for iteration
                __asm__ __volatile__
                (
                    __ASM_EMIT("prefetchnta  0x00(%[left])")
                    __ASM_EMIT("prefetchnta  0x00(%[right])")
                    : : [left] "r"(l), [right] "r" (r)
                );

                #define _LR_TO_MS(l_left, l_right, s_mid, s_side)   \
                    __ASM_EMIT("test $0x1, %[count]")   \
                    __ASM_EMIT("jz 100f")               \
                    /* Blocks of 4 items */             \
                    __ASM_EMIT("dec %[count]")          \
                    __ASM_EMIT("100:")                  \
                    /* Process block */                 \
                    __ASM_EMIT(l_left " (%[left]), %%xmm0")     /* xmm0 = l */ \
                    __ASM_EMIT(l_right " (%[right]), %%xmm2")   /* xmm2 = r */ \
                    __ASM_EMIT("movaps %%xmm0, %%xmm1")         /* xmm1 = l */ \
                    __ASM_EMIT("addps %%xmm2, %%xmm0")          /* xmm0 = l + r */ \
                    __ASM_EMIT("subps %%xmm2, %%xmm1")          /* xmm1 = l - r */ \
                    __ASM_EMIT("mulps %%xmm3, %%xmm0")          /* xmm0 = (l + r) * 0.5f */ \
                    __ASM_EMIT("mulps %%xmm3, %%xmm1")          /* xmm1 = (l - r) * 0.5f */ \
                    __ASM_EMIT(s_mid " %%xmm0, (%[mid])")       \
                    __ASM_EMIT(s_side " %%xmm1, (%[side])")     \
                    /* Increment pointers */            \
                    __ASM_EMIT("add $0x10, %[left]")    \
                    __ASM_EMIT("add $0x10, %[right]")   \
                    __ASM_EMIT("add $0x10, %[mid]")     \
                    __ASM_EMIT("add $0x10, %[side]")    \
                    /* Blocks of 8 items */             \
                    __ASM_EMIT("shr $0x1, %[count]")    \
                    __ASM_EMIT("jz 300f")               \
                    __ASM_EMIT("movaps %%xmm3, %%xmm7") \
                    __ASM_EMIT("200:")                  \
                    /* Prefetch data */                 \
                    __ASM_EMIT("prefetchnta  0x20(%[left])")    \
                    __ASM_EMIT("prefetchnta  0x20(%[right])")   \
                    /* Process block */                 \
                    __ASM_EMIT(l_left " (%[left]), %%xmm0")         /* xmm0 = l0 */ \
                    __ASM_EMIT(l_left " 0x10(%[left]), %%xmm4")     /* xmm4 = l1 */ \
                    __ASM_EMIT(l_right " (%[right]), %%xmm2")       /* xmm2 = r0 */ \
                    __ASM_EMIT(l_right " 0x10(%[right]), %%xmm6")   /* xmm6 = r1 */ \
                    __ASM_EMIT("movaps %%xmm0, %%xmm1")             /* xmm1 = l0 */ \
                    __ASM_EMIT("movaps %%xmm4, %%xmm5")             /* xmm5 = l1 */ \
                    __ASM_EMIT("addps %%xmm2, %%xmm0")              /* xmm0 = l0 + r0 */ \
                    __ASM_EMIT("subps %%xmm2, %%xmm1")              /* xmm1 = l0 - r0 */ \
                    __ASM_EMIT("addps %%xmm6, %%xmm4")              /* xmm4 = l1 + r1 */ \
                    __ASM_EMIT("subps %%xmm6, %%xmm5")              /* xmm5 = l1 - r1 */ \
                    __ASM_EMIT("mulps %%xmm3, %%xmm0")              /* xmm0 = (l0 + r0) * 0.5f */ \
                    __ASM_EMIT("mulps %%xmm7, %%xmm4")              /* xmm4 = (l1 + r1) * 0.5f */ \
                    __ASM_EMIT("mulps %%xmm3, %%xmm1")              /* xmm1 = (l0 - r0) * 0.5f */ \
                    __ASM_EMIT("mulps %%xmm7, %%xmm5")              /* xmm5 = (l1 - r1) * 0.5f */ \
                    __ASM_EMIT(s_mid " %%xmm0, (%[mid])")           \
                    __ASM_EMIT(s_mid " %%xmm4, 0x10(%[mid])")       \
                    __ASM_EMIT(s_side " %%xmm1, (%[side])")         \
                    __ASM_EMIT(s_side " %%xmm5, 0x10(%[side])")     \
                    /* Increment pointers */            \
                    __ASM_EMIT("add $0x20, %[left]")    \
                    __ASM_EMIT("add $0x20, %[right]")   \
                    __ASM_EMIT("add $0x20, %[mid]")     \
                    __ASM_EMIT("add $0x20, %[side]")    \
                    /* Decrement counter */             \
                    __ASM_EMIT("dec %[count]")          \
                    __ASM_EMIT("jnz 200b")              \
                    /* End of block processing */       \
                    __ASM_EMIT("300:")

                // Process blocks
                __asm__ __volatile__
                (
                    __ASM_EMIT("test $0x0f, %[right]")
                    __ASM_EMIT("jnz 4f")
                        __ASM_EMIT("test $0x0f, %[mid]")
                        __ASM_EMIT("jnz 2f")
                            __ASM_EMIT("test $0x0f, %[side]")
                            __ASM_EMIT("jnz 1f")
                                _LR_TO_MS("movaps", "movaps", MOVNTPS, MOVNTPS)
                                __ASM_EMIT("jmp 8f")
                            __ASM_EMIT("1:")
                                _LR_TO_MS("movaps", "movaps", MOVNTPS, "movups")
                                __ASM_EMIT("jmp 8f")
                            __ASM_EMIT("2:")
                            __ASM_EMIT("test $0x0f, %[side]")
                            __ASM_EMIT("jnz 3f")
                                _LR_TO_MS("movaps", "movaps", "movups", MOVNTPS)
                                __ASM_EMIT("jmp 8f")
                            __ASM_EMIT("3:")
                                _LR_TO_MS("movaps", "movaps", "movups", "movups")
                                __ASM_EMIT("jmp 8f")
                    __ASM_EMIT("4:")
                        __ASM_EMIT("test $0x0f, %[mid]")
                        __ASM_EMIT("jnz 6f")
                            __ASM_EMIT("test $0x0f, %[side]")
                            __ASM_EMIT("jnz 5f")
                                _LR_TO_MS("movaps", "movups", MOVNTPS, MOVNTPS)
                                __ASM_EMIT("jmp 8f")
                            __ASM_EMIT("5:")
                                _LR_TO_MS("movaps", "movups", MOVNTPS, "movups")
                                __ASM_EMIT("jmp 8f")
                            __ASM_EMIT("6:")
                            __ASM_EMIT("test $0x0f, %[side]")
                            __ASM_EMIT("jnz 7f")
                                _LR_TO_MS("movaps", "movups", "movups", MOVNTPS)
                                __ASM_EMIT("jmp 8f")
                            __ASM_EMIT("7:")
                                _LR_TO_MS("movaps", "movups", "movups", "movups")
                    __ASM_EMIT("8:")

                    : [left] "+r"(l), [right] "+r" (r), [mid] "+r" (m), [side] "+r" (s), [count] "+r" (regs)
                    :
                    : "cc", "memory",
                      "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                      "%xmm4", "%xmm5", "%xmm6", "%xmm7"
                );

                #undef _LR_TO_MS
            }

            // Tail
            __asm__ __volatile__
            (
                __ASM_EMIT("test %[count], %[count]")
                __ASM_EMIT("jz 2f")
                __ASM_EMIT("1:")

                // Process data
                __ASM_EMIT("movss (%[left]),  %%xmm0")  // xmm0 = l
                __ASM_EMIT("movss (%[right]), %%xmm2")  // xmm2 = r
                __ASM_EMIT("movss %%xmm0, %%xmm1")      // xmm1 = l
                __ASM_EMIT("addss %%xmm2, %%xmm0")      // xmm0 = l + r
                __ASM_EMIT("subss %%xmm2, %%xmm1")      // xmm1 = l - r
                __ASM_EMIT("mulss %%xmm3, %%xmm0")      // xmm0 = (l + r) * 0.5f
                __ASM_EMIT("mulss %%xmm3, %%xmm1")      // xmm1 = (l - r) * 0.5f
                __ASM_EMIT("movss %%xmm0, (%[mid])")
                __ASM_EMIT("movss %%xmm1, (%[side])")

                // Move pointers
                __ASM_EMIT("add $0x4, %[left]")
                __ASM_EMIT("add $0x4, %[right]")
                __ASM_EMIT("add $0x4, %[mid]")
                __ASM_EMIT("add $0x4, %[side]")

                // Repeat loop
                __ASM_EMIT("dec %[count]")
                __ASM_EMIT("jnz 1b")
                __ASM_EMIT("2:")

                : [left] "+r"(l), [right] "+r" (r), [mid] "+r" (m), [side] "+r" (s), [count] "+r" (count)
                :
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3"
            );

            SFENCE;
        }

        static void ms_to_lr(float *l, float *r, const float *m, const float *s, size_t count)
        {
            if (count == 0)
                return;

            // Head
            __asm__ __volatile__
            (
                __ASM_EMIT("1:")

                // Check conditions
                __ASM_EMIT("test $0x0f, %[mid]")
                __ASM_EMIT("jz 2f")

                // Process data
                __ASM_EMIT("movss (%[mid]),  %%xmm0")   // xmm0 = m
                __ASM_EMIT("movss (%[side]), %%xmm2")   // xmm2 = s
                __ASM_EMIT("movss %%xmm0, %%xmm1")      // xmm1 = m
                __ASM_EMIT("addss %%xmm2, %%xmm0")      // xmm0 = m + s
                __ASM_EMIT("subss %%xmm2, %%xmm1")      // xmm1 = m - s
                __ASM_EMIT("movss %%xmm0, (%[left])")
                __ASM_EMIT("movss %%xmm1, (%[right])")

                // Move pointers
                __ASM_EMIT("add $0x4, %[left]")
                __ASM_EMIT("add $0x4, %[right]")
                __ASM_EMIT("add $0x4, %[mid]")
                __ASM_EMIT("add $0x4, %[side]")

                // Repeat loop
                __ASM_EMIT("dec %[count]")
                __ASM_EMIT("jnz 1b")
                __ASM_EMIT("2:")

                : [left] "+r"(l), [right] "+r" (r), [mid] "+r" (m), [side] "+r" (s), [count] "+r" (count)
                : [X_HALF] "m" (X_HALF)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2"
            );

            size_t regs     = count / SSE_MULTIPLE;
            count          %= SSE_MULTIPLE;

            // Body
            if (regs > 0)
            {
                // Prefetch data for iteration
                __asm__ __volatile__
                (
                    __ASM_EMIT("prefetchnta  0x00(%[mid])")
                    __ASM_EMIT("prefetchnta  0x00(%[side])")
                    : : [mid] "r"(m), [side] "r" (s)
                );

                #define _MS_TO_LR(l_mid, l_side, s_left, s_right)   \
                    __ASM_EMIT("test $0x1, %[count]")   \
                    __ASM_EMIT("jz 100f")               \
                    /* Blocks of 4 items */             \
                    __ASM_EMIT("dec %[count]")          \
                    __ASM_EMIT("100:")                  \
                    /* Process block */                 \
                    __ASM_EMIT(l_mid " (%[mid]), %%xmm0")       /* xmm0 = m */ \
                    __ASM_EMIT(l_side " (%[side]), %%xmm2")     /* xmm2 = s */ \
                    __ASM_EMIT("movaps %%xmm0, %%xmm1")         /* xmm1 = m */ \
                    __ASM_EMIT("addps %%xmm2, %%xmm0")          /* xmm0 = m + s */ \
                    __ASM_EMIT("subps %%xmm2, %%xmm1")          /* xmm1 = m - s */ \
                    __ASM_EMIT(s_left " %%xmm0, (%[left])")      \
                    __ASM_EMIT(s_right " %%xmm1, (%[right])")    \
                    /* Increment pointers */            \
                    __ASM_EMIT("add $0x10, %[left]")    \
                    __ASM_EMIT("add $0x10, %[right]")   \
                    __ASM_EMIT("add $0x10, %[mid]")     \
                    __ASM_EMIT("add $0x10, %[side]")    \
                    /* Blocks of 8 items */             \
                    __ASM_EMIT("shr $0x1, %[count]")    \
                    __ASM_EMIT("jz 300f")               \
                    __ASM_EMIT("200:")                  \
                    /* Prefetch data */                 \
                    __ASM_EMIT("prefetchnta  0x20(%[mid])")    \
                    __ASM_EMIT("prefetchnta  0x20(%[side])")   \
                    /* Process block */                 \
                    __ASM_EMIT(l_mid " (%[mid]), %%xmm0")           /* xmm0 = m0 */ \
                    __ASM_EMIT(l_mid " 0x10(%[mid]), %%xmm4")       /* xmm4 = m1 */ \
                    __ASM_EMIT(l_side " (%[side]), %%xmm2")         /* xmm2 = s0 */ \
                    __ASM_EMIT(l_side " 0x10(%[side]), %%xmm6")     /* xmm6 = s1 */ \
                    __ASM_EMIT("movaps %%xmm0, %%xmm1")             /* xmm1 = m0 */ \
                    __ASM_EMIT("movaps %%xmm4, %%xmm5")             /* xmm5 = m1 */ \
                    __ASM_EMIT("addps %%xmm2, %%xmm0")              /* xmm0 = m0 + s0 */ \
                    __ASM_EMIT("subps %%xmm2, %%xmm1")              /* xmm1 = m0 - s0 */ \
                    __ASM_EMIT("addps %%xmm6, %%xmm4")              /* xmm4 = m1 + s1 */ \
                    __ASM_EMIT("subps %%xmm6, %%xmm5")              /* xmm5 = m1 - s1 */ \
                    __ASM_EMIT(s_left " %%xmm0, (%[left])")           \
                    __ASM_EMIT(s_left " %%xmm4, 0x10(%[left])")       \
                    __ASM_EMIT(s_right " %%xmm1, (%[right])")         \
                    __ASM_EMIT(s_right " %%xmm5, 0x10(%[right])")     \
                    /* Increment pointers */            \
                    __ASM_EMIT("add $0x20, %[left]")    \
                    __ASM_EMIT("add $0x20, %[right]")   \
                    __ASM_EMIT("add $0x20, %[mid]")     \
                    __ASM_EMIT("add $0x20, %[side]")    \
                    /* Decrement counter */             \
                    __ASM_EMIT("dec %[count]")          \
                    __ASM_EMIT("jnz 200b")              \
                    /* End of block processing */       \
                    __ASM_EMIT("300:")

                // Process blocks
                __asm__ __volatile__
                (
                    __ASM_EMIT("test $0x0f, %[side]")
                    __ASM_EMIT("jnz 4f")
                        __ASM_EMIT("test $0x0f, %[left]")
                        __ASM_EMIT("jnz 2f")
                            __ASM_EMIT("test $0x0f, %[right]")
                            __ASM_EMIT("jnz 1f")
                                _MS_TO_LR("movaps", "movaps", MOVNTPS, MOVNTPS)
                                __ASM_EMIT("jmp 8f")
                            __ASM_EMIT("1:")
                                _MS_TO_LR("movaps", "movaps", MOVNTPS, "movups")
                                __ASM_EMIT("jmp 8f")
                            __ASM_EMIT("2:")
                            __ASM_EMIT("test $0x0f, %[right]")
                            __ASM_EMIT("jnz 3f")
                                _MS_TO_LR("movaps", "movaps", "movups", MOVNTPS)
                                __ASM_EMIT("jmp 8f")
                            __ASM_EMIT("3:")
                                _MS_TO_LR("movaps", "movaps", "movups", "movups")
                                __ASM_EMIT("jmp 8f")
                    __ASM_EMIT("4:")
                        __ASM_EMIT("test $0x0f, %[left]")
                        __ASM_EMIT("jnz 6f")
                            __ASM_EMIT("test $0x0f, %[right]")
                            __ASM_EMIT("jnz 5f")
                                _MS_TO_LR("movaps", "movups", MOVNTPS, MOVNTPS)
                                __ASM_EMIT("jmp 8f")
                            __ASM_EMIT("5:")
                                _MS_TO_LR("movaps", "movups", MOVNTPS, "movups")
                                __ASM_EMIT("jmp 8f")
                            __ASM_EMIT("6:")
                            __ASM_EMIT("test $0x0f, %[right]")
                            __ASM_EMIT("jnz 7f")
                                _MS_TO_LR("movaps", "movups", "movups", MOVNTPS)
                                __ASM_EMIT("jmp 8f")
                            __ASM_EMIT("7:")
                                _MS_TO_LR("movaps", "movups", "movups", "movups")
                    __ASM_EMIT("8:")

                    : [left] "+r"(l), [right] "+r" (r), [mid] "+r" (m), [side] "+r" (s), [count] "+r" (regs)
                    :
                    : "cc", "memory",
                      "%xmm0", "%xmm1", "%xmm2",
                      "%xmm4", "%xmm5", "%xmm6"
                );

                #undef _MS_TO_LR
            }

            // Tail
            __asm__ __volatile__
            (
                __ASM_EMIT("test %[count], %[count]")
                __ASM_EMIT("jz 2f")
                __ASM_EMIT("1:")

                // Process data
                __ASM_EMIT("movss (%[mid]),  %%xmm0")   // xmm0 = m
                __ASM_EMIT("movss (%[side]), %%xmm2")   // xmm2 = s
                __ASM_EMIT("movss %%xmm0, %%xmm1")      // xmm1 = m
                __ASM_EMIT("addss %%xmm2, %%xmm0")      // xmm0 = m + s
                __ASM_EMIT("subss %%xmm2, %%xmm1")      // xmm1 = m - s
                __ASM_EMIT("movss %%xmm0, (%[left])")
                __ASM_EMIT("movss %%xmm1, (%[right])")

                // Move pointers
                __ASM_EMIT("add $0x4, %[left]")
                __ASM_EMIT("add $0x4, %[right]")
                __ASM_EMIT("add $0x4, %[mid]")
                __ASM_EMIT("add $0x4, %[side]")

                // Repeat loop
                __ASM_EMIT("dec %[count]")
                __ASM_EMIT("jnz 1b")
                __ASM_EMIT("2:")

                : [left] "+r"(l), [right] "+r" (r), [mid] "+r" (m), [side] "+r" (s), [count] "+r" (count)
                :
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2"
            );

            SFENCE;
        }

        static float biquad_process(float *buf, const float *ir, float sample)
        {
            __asm__ __volatile__
            (
                // Load data
                __ASM_EMIT("shufps $0x00, %%xmm0, %%xmm0")  // xmm0 = s s s s
                __ASM_EMIT("movaps (%[buf]), %%xmm1")       // xmm1 = b0 b1 b2 b3
                __ASM_EMIT("movaps (%[ir]), %%xmm2")        // xmm2 = i0 i1 i2 i3
                __ASM_EMIT("movss  0x10(%[ir]), %%xmm3")    // xmm3 = i4

                // Calculate infinite impulse response
                __ASM_EMIT("mulps  %%xmm1, %%xmm2")         // xmm2 = b0*i0 b1*i1 b2*i2 b3*i3
                __ASM_EMIT("mulss  %%xmm3, %%xmm0")         // xmm0 = i4*s s s s

                // Calculate horizontal sum of xmm2
                __ASM_EMIT("movhlps %%xmm2, %%xmm3")
                __ASM_EMIT("addps %%xmm3, %%xmm2")
                __ASM_EMIT("movaps %%xmm2, %%xmm3")
                __ASM_EMIT("shufps $0x55, %%xmm2, %%xmm3")
                __ASM_EMIT("addss %%xmm3, %%xmm2")          // xmm2 = b0*i0 + b1*i1 + b2*i2 + b3*i3
                __ASM_EMIT("addss %%xmm2, %%xmm0")          // xmm0 = R s s s

                // Shift buffer into xmm0
                __ASM_EMIT("movlhps %%xmm1, %%xmm0")        // xmm0 = R s b0 b1

                // Store new buffer
                __ASM_EMIT("movaps %%xmm0, (%[buf])")

                : "+Yz" (sample)
                : [buf] "r" (buf), [ir] "r" (ir)
                : "memory",
                  "%xmm1", "%xmm2", "%xmm3"
            );

            return sample;
        }

        static void biquad_process_multi(float *dst, const float *src, size_t count, float *buf, const float *ir)
        {
            __asm__ __volatile__
            (
                // Check count
                __ASM_EMIT("test %[count], %[count]")
                __ASM_EMIT("jz 1f")

                // Load permanent data
                __ASM_EMIT("movups (%[ir]), %%xmm6")        // xmm6 = i0 i1 i2 i3
                __ASM_EMIT("movss  0x10(%[ir]), %%xmm7")    // xmm7 = i4
                __ASM_EMIT("movups (%[buf]), %%xmm1")       // xmm1 = b0 b1 b2 b3

                // Start loop
                __ASM_EMIT("2:")

                // Load data
                __ASM_EMIT("movss (%[src]), %%xmm0")        // xmm0 = s ? ? ?
                __ASM_EMIT("movaps %%xmm1, %%xmm2")         // xmm2 = b0 b1 b2 b3
                __ASM_EMIT("shufps $0x00, %%xmm0, %%xmm0")  // xmm0 = s s s s

                // Calculate infinite impulse response
                __ASM_EMIT("mulps  %%xmm6, %%xmm2")         // xmm2 = b0*i0 b1*i1 b2*i2 b3*i3
                __ASM_EMIT("mulss  %%xmm7, %%xmm0")         // xmm0 = i4*s s s s

                // Calculate horizontal sum of xmm2, temporary register is xmm3
                __ASM_EMIT("movhlps %%xmm2, %%xmm3")
                __ASM_EMIT("addps %%xmm3, %%xmm2")
                __ASM_EMIT("movaps %%xmm2, %%xmm3")
                __ASM_EMIT("shufps $0x55, %%xmm2, %%xmm3")
                __ASM_EMIT("addss %%xmm3, %%xmm2")          // xmm2 = b0*i0 + b1*i1 + b2*i2 + b3*i3
                __ASM_EMIT("addss %%xmm2, %%xmm0")          // xmm0 = R s s s

                // Shift buffer into xmm0 and update buffer
                __ASM_EMIT("movlhps %%xmm1, %%xmm0")        // xmm0 = R s b0 b1
                __ASM_EMIT("movaps %%xmm0, %%xmm1")         // xmm1 = R s b0 b1 = b0' b1' b2' b3'

                // Store processed sample and re-load buffer
                __ASM_EMIT("movss %%xmm0, (%[dst])")

                // Update pointers and repeat loop
                __ASM_EMIT("add $4, %[src]")
                __ASM_EMIT("add $4, %[dst]")
                __ASM_EMIT("dec %[count]")
                __ASM_EMIT("jnz 2b")

                // Store the updated buffer state
                __ASM_EMIT("movups %%xmm0, (%[buf])")

                // Exit label
                __ASM_EMIT("1:")

                : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
                : [buf] "r" (buf), [ir] "r" (ir)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm6", "%xmm7"
            );
        }

    } // namespace sse
} // namespace lsp


#endif /* CORE_X86_DSP_H_ */
