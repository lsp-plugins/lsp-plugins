/*
 * dsp.h
 *
 *  Created on: 05 окт. 2015 г.
 *      Author: sadko
 */

#ifndef CORE_X86_DSP_H_
#define CORE_X86_DSP_H_

#include <core/dsp.h>
#include <core/x86/sse/const.h>

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
                            __ASM_EMIT("movaps (%[src]), %%xmm0")
                            __ASM_EMIT(MOVNTPS " %%xmm0, (%[dst])")

                            // Move pointers
                            __ASM_EMIT("add $0x10, %[src]")
                            __ASM_EMIT("add $0x10, %[dst]")
                            __ASM_EMIT("dec %[count]")
                            __ASM_EMIT("jnz 1b")

                            : [src] "+r" (src), [dst] "+r"(dst), [count] "+r"(regs) :
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
                            __ASM_EMIT("movups (%[src]), %%xmm0")
                            __ASM_EMIT(MOVNTPS " %%xmm0, (%[dst])")

                            // Move pointers
                            __ASM_EMIT("add $0x10, %[src]")
                            __ASM_EMIT("add $0x10, %[dst]")
                            __ASM_EMIT("dec %[count]")
                            __ASM_EMIT("jnz 1b")

                            : [src] "+r" (src), [dst] "+r"(dst), [count] "+r"(regs) :
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
                __ASM_EMIT("movss %[k], %%xmm4")
                : : [k] "x" (k)
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
                    // Horizontally sum xmm0
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
                    __ASM_EMIT("prefetchnta 0x00(%[dst])")
                    __ASM_EMIT("prefetchnta 0x20(%[dst])")
                    __ASM_EMIT("prefetchnta -0x40(%[src])")
                    __ASM_EMIT("prefetchnta -0x20(%[src])")
                    : : [dst] "r" (dst), [src] "r" (src)
                );

                #define REVERSE_BLOCKS(mv_dst, mv_src)   \
                    if (blocks > 0) \
                    { \
                        __asm__ __volatile__ \
                        ( \
                            __ASM_EMIT("1:") \
                            \
                            /* Prefetch next data */ \
                            __ASM_EMIT("prefetchnta 0x40(%[dst])") \
                            __ASM_EMIT("prefetchnta 0x60(%[dst])") \
                            __ASM_EMIT("prefetchnta -0x80(%[src])") \
                            __ASM_EMIT("prefetchnta -0x60(%[src])") \
                            \
                            /* Process data */ \
                            __ASM_EMIT(mv_dst " 0x00(%[dst]), %%xmm0") \
                            __ASM_EMIT(mv_dst " 0x10(%[dst]), %%xmm2") \
                            __ASM_EMIT(mv_dst " 0x20(%[dst]), %%xmm4") \
                            __ASM_EMIT(mv_dst " 0x30(%[dst]), %%xmm6") \
                            \
                            __ASM_EMIT(mv_src " -0x40(%[src]), %%xmm7") \
                            __ASM_EMIT(mv_src " -0x30(%[src]), %%xmm5") \
                            __ASM_EMIT(mv_src " -0x20(%[src]), %%xmm3") \
                            __ASM_EMIT(mv_src " -0x10(%[src]), %%xmm1") \
                            \
                            __ASM_EMIT("shufps $0x1b, %%xmm0, %%xmm0") \
                            __ASM_EMIT("shufps $0x1b, %%xmm1, %%xmm1") \
                            __ASM_EMIT("shufps $0x1b, %%xmm2, %%xmm2") \
                            __ASM_EMIT("shufps $0x1b, %%xmm3, %%xmm3") \
                            __ASM_EMIT("shufps $0x1b, %%xmm4, %%xmm4") \
                            __ASM_EMIT("shufps $0x1b, %%xmm5, %%xmm5") \
                            __ASM_EMIT("shufps $0x1b, %%xmm6, %%xmm6") \
                            __ASM_EMIT("shufps $0x1b, %%xmm7, %%xmm7") \
                            \
                            __ASM_EMIT(mv_dst " %%xmm1, 0x00(%[dst])") \
                            __ASM_EMIT(mv_dst " %%xmm3, 0x10(%[dst])") \
                            __ASM_EMIT(mv_dst " %%xmm5, 0x20(%[dst])") \
                            __ASM_EMIT(mv_dst " %%xmm7, 0x30(%[dst])") \
                            \
                            __ASM_EMIT(mv_src " %%xmm6, -0x40(%[src])") \
                            __ASM_EMIT(mv_src " %%xmm4, -0x30(%[src])") \
                            __ASM_EMIT(mv_src " %%xmm2, -0x20(%[src])") \
                            __ASM_EMIT(mv_src " %%xmm0, -0x10(%[src])") \
                            \
                            /* Move pointers */ \
                            __ASM_EMIT("add $0x40, %[dst]") \
                            __ASM_EMIT("sub $0x40, %[src]") \
                            \
                            /* Repeat loop */ \
                            __ASM_EMIT("dec %[blocks]") \
                            __ASM_EMIT("jnz 1b") \
                            \
                            : [dst] "+r" (dst), [src] "+r" (src), [blocks] "+r"(blocks) : \
                            : "cc", "memory", \
                                "%xmm0", "%xmm1", "%xmm2", "%xmm3", \
                                "%xmm4", "%xmm5", "%xmm6", "%xmm7" \
                        ); \
                    }

                #define REVERSE_REGS(mv_dst, mv_src)   \
                    if (regs > 0) \
                    { \
                        __asm__ __volatile__ \
                        ( \
                            __ASM_EMIT("1:") \
                            \
                            /* Process data */ \
                            __ASM_EMIT(mv_dst " 0x00(%[dst]), %%xmm0") \
                            __ASM_EMIT(mv_src " -0x10(%[src]), %%xmm1") \
                            \
                            __ASM_EMIT("shufps $0x1b, %%xmm0, %%xmm0") \
                            __ASM_EMIT("shufps $0x1b, %%xmm1, %%xmm1") \
                            \
                            __ASM_EMIT(mv_dst " %%xmm1, 0x00(%[dst])") \
                            __ASM_EMIT(mv_src " %%xmm0, -0x10(%[src])") \
                            \
                            /* Move pointers */ \
                            __ASM_EMIT("add $0x10, %[dst]") \
                            __ASM_EMIT("sub $0x10, %[src]") \
                            \
                            /* Repeat loop */ \
                            __ASM_EMIT("dec %[count]") \
                            __ASM_EMIT("jnz 1b") \
                            \
                            : [dst] "+r" (dst), [src] "+r" (src), [count] "+r"(regs) : \
                            : "cc", "memory", \
                                "%xmm0", "%xmm1" \
                        ); \
                    }

                if (sse_aligned(src))
                {
                    if (sse_aligned(dst))
                    {
                        REVERSE_BLOCKS("movaps", "movaps");
                        REVERSE_REGS("movaps", "movaps");
                    }
                    else // !sse_aligned(dst)
                    {
                        REVERSE_BLOCKS("movups", "movaps");
                        REVERSE_REGS("movups", "movaps");
                    }
                }
                else // !sse_aligned(src)
                {
                    if (sse_aligned(dst))
                    {
                        REVERSE_BLOCKS("movaps", "movups");
                        REVERSE_REGS("movaps", "movups");
                    }
                    else
                    {
                        REVERSE_BLOCKS("movups", "movups");
                        REVERSE_REGS("movups", "movups");
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
                    __ASM_EMIT("movss 0x00(%[dst]), %%xmm0")
                    __ASM_EMIT("movss -0x04(%[src]), %%xmm1")
                    __ASM_EMIT("movss %%xmm1, (%[dst])")
                    __ASM_EMIT("movss %%xmm0, -0x04(%[src])")

                    // Move pointers
                    __ASM_EMIT("add $0x4, %[dst]")
                    __ASM_EMIT("sub $0x4, %[src]")

                    // Repeat loop
                    __ASM_EMIT("dec %[count]")
                    __ASM_EMIT("jnz 1b")
                    __ASM_EMIT("2:")

                    : [dst] "+r" (dst), [src] "+r"(src), [count] "+r" (count) :
                    : "cc", "memory",
                      "%xmm0", "%xmm1"
                );
            }

            SFENCE;

            #undef REVERSE_BLOCKS
            #undef REVERSE_REGS
        }

    } // namespace sse
} // namespace lsp


#endif /* CORE_X86_DSP_H_ */
