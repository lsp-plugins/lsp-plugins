/*
 * native_dsp.hpp
 *
 *  Created on: 05 окт. 2015 г.
 *      Author: sadko
 */

#ifndef CORE_DSP_SSE_H_
#define CORE_DSP_SSE_H_

#include <core/dsp.h>

// Parameters for SSE
#define SSE_MULTIPLE                4
#define SSE_ALIGN                   (SSE_MULTIPLE * sizeof(float))

namespace lsp
{
    namespace sse
    {
        inline bool sse_aligned(const void *ptr) { return !(ptrdiff_t(ptr) & (SSE_ALIGN - 1));  };
        inline size_t sse_multiple(size_t count) { return count & (SSE_MULTIPLE - 1);           }

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

                : "+S" (src), "+D"(dst), "+r" (count) :
                : "cc", "memory",
                  "%xmm0"
            );

            // Calculate amount of registers
            size_t regs = count / SSE_MULTIPLE;
            size_t tail = count % SSE_MULTIPLE;

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
                    : : "S" (src)
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

                            __ASM_EMIT("movntps %%xmm0,  0x00(%1)")
                            __ASM_EMIT("movntps %%xmm1,  0x10(%1)")
                            __ASM_EMIT("movntps %%xmm2,  0x20(%1)")
                            __ASM_EMIT("movntps %%xmm3,  0x30(%1)")
                            __ASM_EMIT("movntps %%xmm4,  0x40(%1)")
                            __ASM_EMIT("movntps %%xmm5,  0x50(%1)")
                            __ASM_EMIT("movntps %%xmm6,  0x60(%1)")
                            __ASM_EMIT("movntps %%xmm7,  0x70(%1)")

                            // Move pointers
                            __ASM_EMIT("add $0x80, %0")
                            __ASM_EMIT("add $0x80, %1")

                            // Repeat loop
                            __ASM_EMIT("dec %2")
                            __ASM_EMIT("jnz 1b")

                            : "+S" (src), "+D"(dst), "+r"(blocks) :
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
                            __ASM_EMIT("movntps %%xmm0, (%1)")

                            // Move pointers
                            __ASM_EMIT("add $0x10, %0")
                            __ASM_EMIT("add $0x10, %1")
                            __ASM_EMIT("dec %2")
                            __ASM_EMIT("jnz 1b")

                            : "+S" (src), "+D"(dst), "+r"(regs) :
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

                            __ASM_EMIT("movntps %%xmm0,  0x00(%1)")
                            __ASM_EMIT("movntps %%xmm1,  0x10(%1)")
                            __ASM_EMIT("movntps %%xmm2,  0x20(%1)")
                            __ASM_EMIT("movntps %%xmm3,  0x30(%1)")
                            __ASM_EMIT("movntps %%xmm4,  0x40(%1)")
                            __ASM_EMIT("movntps %%xmm5,  0x50(%1)")
                            __ASM_EMIT("movntps %%xmm6,  0x60(%1)")
                            __ASM_EMIT("movntps %%xmm7,  0x70(%1)")

                            // Move pointers
                            __ASM_EMIT("add $0x80, %0")
                            __ASM_EMIT("add $0x80, %1")

                            // Repeat loop
                            __ASM_EMIT("dec %2")
                            __ASM_EMIT("jnz 1b")

                            : "+S" (src), "+D"(dst), "+r"(blocks) :
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
                            __ASM_EMIT("movntps %%xmm0, (%1)")

                            // Move pointers
                            __ASM_EMIT("add $0x10, %0")
                            __ASM_EMIT("add $0x10, %1")
                            __ASM_EMIT("dec %2")
                            __ASM_EMIT("jnz 1b")

                            : "+S" (src), "+D"(dst), "+r"(regs) :
                            : "cc", "memory",
                              "%xmm0"
                        );
                    }
                }
            }

            // Copy tail
            if (tail > 0)
            {
                __asm__ __volatile__
                (
                    __ASM_EMIT("1:")

                    __ASM_EMIT("movss (%0), %%xmm0")
                    __ASM_EMIT("movss %%xmm0, (%1)")

                    // Move pointers
                    __ASM_EMIT("add $0x4, %0")
                    __ASM_EMIT("add $0x4, %1")

                    // Repeat loop
                    __ASM_EMIT("dec %2")
                    __ASM_EMIT("jnz 1b")

                    : "+S" (src), "+D"(dst), "+r"(tail) :
                    : "cc", "memory",
                      "%xmm0"
                );
            }

            __asm__ __volatile__
            (
                __ASM_EMIT("sfence")
            );
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

                : "+S" (src), "+D"(dst), "+r" (count) :
                : "cc", "memory",
                  "%xmm0"
            );

            // Calculate amount of registers
            size_t regs = count / SSE_MULTIPLE;
            size_t tail = count % SSE_MULTIPLE;

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

                            __ASM_EMIT("movntps %%xmm0,  0x00(%1)")
                            __ASM_EMIT("movntps %%xmm1,  0x10(%1)")
                            __ASM_EMIT("movntps %%xmm2,  0x20(%1)")
                            __ASM_EMIT("movntps %%xmm3,  0x30(%1)")
                            __ASM_EMIT("movntps %%xmm4,  0x40(%1)")
                            __ASM_EMIT("movntps %%xmm5,  0x50(%1)")
                            __ASM_EMIT("movntps %%xmm6,  0x60(%1)")
                            __ASM_EMIT("movntps %%xmm7,  0x70(%1)")

                            // Check loop
                            __ASM_EMIT("dec %2")
                            __ASM_EMIT("jnz 1b")

                            : "+S" (src), "+D"(dst), "+r"(blocks) :
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
                            __ASM_EMIT("movntps %%xmm0, (%1)")

                            __ASM_EMIT("dec %2")
                            __ASM_EMIT("jnz 1b")

                            : "+S" (src), "+D"(dst), "+r"(regs) :
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

                            __ASM_EMIT("movntps %%xmm0,  0x00(%1)")
                            __ASM_EMIT("movntps %%xmm1,  0x10(%1)")
                            __ASM_EMIT("movntps %%xmm2,  0x20(%1)")
                            __ASM_EMIT("movntps %%xmm3,  0x30(%1)")
                            __ASM_EMIT("movntps %%xmm4,  0x40(%1)")
                            __ASM_EMIT("movntps %%xmm5,  0x50(%1)")
                            __ASM_EMIT("movntps %%xmm6,  0x60(%1)")
                            __ASM_EMIT("movntps %%xmm7,  0x70(%1)")

                            // Check loop
                            __ASM_EMIT("dec %2")
                            __ASM_EMIT("jnz 1b")

                            : "+S" (src), "+D"(dst), "+r"(blocks) :
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
                            __ASM_EMIT("movntps %%xmm0, (%1)")

                            __ASM_EMIT("dec %2")
                            __ASM_EMIT("jnz 1b")

                            : "+S" (src), "+D"(dst), "+r"(regs) :
                            : "cc", "memory",
                              "%xmm0"
                        );
                    }
                }
            }

            // Copy tail
            if (tail > 0)
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

                    : "+S" (src), "+D"(dst), "+r"(tail) :
                    : "cc", "memory",
                      "%xmm0"
                );
            }

            __asm__ __volatile__
            (
                __ASM_EMIT("sfence")
            );
        }

        void copy(float *dst, const float *src, size_t count)
        {
            if ((count == 0) || (dst == src))
                return;
            copy_forward(dst, src, count);
        }

        void move(float *dst, const float *src, size_t count)
        {
            if (count == 0)
                return;
            if (dst < src)
                copy_forward(dst, src, count);
            else if (dst > src)
                copy_backward(dst, src, count);
        }

        void multiply(float *dst, const float *src, float k, size_t count)
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

                : "+S" (src), "+D"(dst), "+r" (count), "+x"(k) :
                : "cc", "memory",
                  "%xmm4"
            );

            // Calculate amount of registers
            size_t regs = count / SSE_MULTIPLE;
            size_t tail = count % SSE_MULTIPLE;

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

                            __ASM_EMIT("movntps %%xmm0,  0x00(%1)")
                            __ASM_EMIT("movntps %%xmm1,  0x10(%1)")
                            __ASM_EMIT("movntps %%xmm2,  0x20(%1)")
                            __ASM_EMIT("movntps %%xmm3,  0x30(%1)")

                            // Move pointers
                            __ASM_EMIT("add $0x40, %0")
                            __ASM_EMIT("add $0x40, %1")
                            __ASM_EMIT("dec %2")
                            __ASM_EMIT("jnz 1b")

                            : "+S" (src), "+D"(dst), "+r"(blocks) :
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
                            __ASM_EMIT("movntps %%xmm0, (%1)")

                            // Move pointers
                            __ASM_EMIT("add $0x10, %0")
                            __ASM_EMIT("add $0x10, %1")
                            __ASM_EMIT("dec %2")
                            __ASM_EMIT("jnz 1b")

                            : "+S" (src), "+D"(dst), "+r"(regs) :
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

                            __ASM_EMIT("movntps %%xmm0,  0x00(%1)")
                            __ASM_EMIT("movntps %%xmm1,  0x10(%1)")
                            __ASM_EMIT("movntps %%xmm2,  0x20(%1)")
                            __ASM_EMIT("movntps %%xmm3,  0x30(%1)")

                            // Move pointers
                            __ASM_EMIT("add $0x40, %0")
                            __ASM_EMIT("add $0x40, %1")
                            __ASM_EMIT("dec %2")
                            __ASM_EMIT("jnz 1b")

                            : "+S" (src), "+D"(dst), "+r"(blocks) :
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
                            __ASM_EMIT("movntps %%xmm0, (%1)")

                            // Move pointers
                            __ASM_EMIT("add $0x10, %0")
                            __ASM_EMIT("add $0x10, %1")
                            __ASM_EMIT("dec %2")
                            __ASM_EMIT("jnz 1b")

                            : "+S" (src), "+D"(dst), "+r"(regs) :
                            : "cc", "memory",
                              "%xmm0"
                        );
                    }
                }
            }

            // Copy tail
            if (tail > 0)
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

                    : "+S" (src), "+D"(dst), "+r"(tail) :
                    : "cc", "memory",
                      "%xmm0"
                );
            }

            __asm__ __volatile__
            (
                __ASM_EMIT("sfence")
            );
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

                : "+D"(dst), "+r" (count), "+Yz" (value) :
                : "cc", "memory"
            );

//            // Align destination
//            while (true)
//            {
//                if (sse_aligned(dst))
//                    break;
//                if (count-- <= 0)
//                    return;
//                __asm__ __volatile__
//                (
//                    // Store value
//                    __ASM_EMIT("movss %%xmm0, (%0)")
//
//                    // Move pointers
//                    __ASM_EMIT("add $0x4, %0")
//
//                    : "+D"(dst) :
//                    : "cc", "memory"
//                );
//            }

            // Calculate amount of registers
            size_t regs = count / SSE_MULTIPLE;
            size_t tail = count % SSE_MULTIPLE;

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

                        __ASM_EMIT("movntps %%xmm0,  0x00(%0)")
                        __ASM_EMIT("movntps %%xmm1,  0x10(%0)")
                        __ASM_EMIT("movntps %%xmm2,  0x20(%0)")
                        __ASM_EMIT("movntps %%xmm3,  0x30(%0)")
                        __ASM_EMIT("movntps %%xmm4,  0x40(%0)")
                        __ASM_EMIT("movntps %%xmm5,  0x50(%0)")
                        __ASM_EMIT("movntps %%xmm6,  0x60(%0)")
                        __ASM_EMIT("movntps %%xmm7,  0x70(%0)")

                        // Move pointers
                        __ASM_EMIT("add $0x80, %0")
                        __ASM_EMIT("dec %1")
                        __ASM_EMIT("jnz 1b")

                        : "+D"(dst), "+r"(blocks) :
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
                        __ASM_EMIT("movntps %%xmm0, (%0)")

                        // Move pointers
                        __ASM_EMIT("add $0x10, %0")
                        __ASM_EMIT("dec %1")
                        __ASM_EMIT("jnz 1b")

                        : "+D"(dst), "+r"(regs) :
                        : "cc", "memory"
                    );
                }
            }

            // Fill tail
            if (tail > 0)
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

                    : "+D"(dst), "+r"(tail) :
                    : "cc", "memory"
                );
            }

            __asm__ __volatile__
            (
                __ASM_EMIT("sfence")
            );
        }

        void fill(float *dst, float value, size_t count)
        {
            if (count > 0)
                fill_forward(dst, value, count);
        }

        void fill_zero(float *dst, size_t count)
        {
            if (count > 0)
                fill_forward(dst, 0.0f, count);
        }

        void fill_one(float *dst, size_t count)
        {
            if (count > 0)
                fill_forward(dst, 1.0f, count);
        }

        void fill_minus_one(float *dst, size_t count)
        {
            if (count > 0)
                fill_forward(dst, -1.0f, count);
        }

        void add_multiplied(float *dst, const float *src, float k, size_t count)
        {
            __asm__ __volatile__
            (
                __ASM_EMIT("movss %0, %%xmm4")
                : : "x" (k)
                : "%xmm4"
            );

            // Align destination
            while (true)
            {
                if (sse_aligned(dst))
                    break;
                if (count-- <= 0)
                    return;
                __asm__ __volatile__
                (
                    __ASM_EMIT("movss (%0), %%xmm0")
                    __ASM_EMIT("mulss %%xmm4, %%xmm0")
                    __ASM_EMIT("addss (%1), %%xmm0")
                    __ASM_EMIT("movss %%xmm0, (%1)")

                    // Move pointers
                    __ASM_EMIT("add $0x4, %0")
                    __ASM_EMIT("add $0x4, %1")

                    : "+S" (src), "+D"(dst) :
                    : "cc", "memory",
                      "%xmm0"
                );
            }

            // Calculate amount of registers
            size_t regs = count / SSE_MULTIPLE;
            size_t tail = count % SSE_MULTIPLE;

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
                    : : "S" (src), "D" (dst)
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

                            __ASM_EMIT("movntps %%xmm0,  0x00(%1)")
                            __ASM_EMIT("movntps %%xmm1,  0x10(%1)")
                            __ASM_EMIT("movntps %%xmm2,  0x20(%1)")
                            __ASM_EMIT("movntps %%xmm3,  0x30(%1)")

                            // Move pointers
                            __ASM_EMIT("add $0x40, %0")
                            __ASM_EMIT("add $0x40, %1")
                            __ASM_EMIT("dec %2")
                            __ASM_EMIT("jnz 1b")

                            : "+S" (src), "+D"(dst), "+r"(blocks) :
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
                            __ASM_EMIT("movntps %%xmm0, (%1)")

                            // Move pointers
                            __ASM_EMIT("add $0x10, %0")
                            __ASM_EMIT("add $0x10, %1")
                            __ASM_EMIT("dec %2")
                            __ASM_EMIT("jnz 1b")

                            : "+S" (src), "+D"(dst), "+r"(regs) :
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

                            __ASM_EMIT("movntps %%xmm0,  0x00(%1)")
                            __ASM_EMIT("movntps %%xmm1,  0x10(%1)")
                            __ASM_EMIT("movntps %%xmm2,  0x20(%1)")
                            __ASM_EMIT("movntps %%xmm3,  0x30(%1)")

                            // Move pointers
                            __ASM_EMIT("add $0x40, %0")
                            __ASM_EMIT("add $0x40, %1")

                            // Repeat loop
                            __ASM_EMIT("dec %2")
                            __ASM_EMIT("jnz 1b")

                            : "+S" (src), "+D"(dst), "+r"(blocks) :
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
                            __ASM_EMIT("movntps %%xmm0, (%1)")

                            // Move pointers
                            __ASM_EMIT("add $0x10, %0")
                            __ASM_EMIT("add $0x10, %1")

                            // Repeat loop
                            __ASM_EMIT("dec %2")
                            __ASM_EMIT("jnz 1b")

                            : "+S" (src), "+D"(dst), "+r"(regs) :
                            : "cc", "memory",
                              "%xmm0"
                        );
                    }
                }
            }

            // Process tail
            if (tail > 0)
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

                    : "+S" (src), "+D"(dst), "+r"(tail) :
                    : "cc", "memory",
                      "%xmm0"
                );
            }

            __asm__ __volatile__
            (
                __ASM_EMIT("sfence")
            );
        }

        void sub_multiplied(float *dst, const float *src, float k, size_t count)
        {
            add_multiplied(dst, src, -k, count);
        }

        void integrate(float *dst, const float *src, float k, size_t count)
        {
            __asm__ __volatile__
            (
                __ASM_EMIT("movss %0, %%xmm4")
                : : "x" (k)
                : "%xmm4"
            );

            // Align destination
            while (true)
            {
                if (sse_aligned(dst))
                    break;
                if (count-- <= 0)
                    return;
                __asm__ __volatile__
                (
                    __ASM_EMIT("movss (%0), %%xmm0")
                    __ASM_EMIT("subss (%1), %%xmm0")
                    __ASM_EMIT("mulss %%xmm4, %%xmm0")
                    __ASM_EMIT("addss (%1), %%xmm0")
                    __ASM_EMIT("movss %%xmm0, (%1)")

                    // Move pointers
                    __ASM_EMIT("add $0x4, %0")
                    __ASM_EMIT("add $0x4, %1")

                    : "+S" (src), "+D"(dst) :
                    : "cc", "memory",
                      "%xmm0"
                );
            }

            // Calculate amount of registers
            size_t regs = count / SSE_MULTIPLE;
            size_t tail = count % SSE_MULTIPLE;

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
                    : : "S" (src), "D" (dst)
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

                            __ASM_EMIT("subps 0x00(%1), %%xmm0")
                            __ASM_EMIT("subps 0x10(%1), %%xmm1")
                            __ASM_EMIT("subps 0x20(%1), %%xmm2")
                            __ASM_EMIT("subps 0x30(%1), %%xmm3")

                            __ASM_EMIT("mulps %%xmm4, %%xmm0")
                            __ASM_EMIT("mulps %%xmm5, %%xmm1")
                            __ASM_EMIT("mulps %%xmm6, %%xmm2")
                            __ASM_EMIT("mulps %%xmm7, %%xmm3")

                            __ASM_EMIT("addps 0x00(%1), %%xmm0")
                            __ASM_EMIT("addps 0x10(%1), %%xmm1")
                            __ASM_EMIT("addps 0x20(%1), %%xmm2")
                            __ASM_EMIT("addps 0x30(%1), %%xmm3")

                            __ASM_EMIT("movntps %%xmm0,  0x00(%1)")
                            __ASM_EMIT("movntps %%xmm1,  0x10(%1)")
                            __ASM_EMIT("movntps %%xmm2,  0x20(%1)")
                            __ASM_EMIT("movntps %%xmm3,  0x30(%1)")

                            // Move pointers
                            __ASM_EMIT("add $0x40, %0")
                            __ASM_EMIT("add $0x40, %1")
                            __ASM_EMIT("dec %2")
                            __ASM_EMIT("jnz 1b")

                            : "+S" (src), "+D"(dst), "+r"(blocks) :
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
                            __ASM_EMIT("subps (%1), %%xmm0")
                            __ASM_EMIT("mulps %%xmm4, %%xmm0")
                            __ASM_EMIT("addps (%1), %%xmm0")
                            __ASM_EMIT("movntps %%xmm0, (%1)")

                            // Move pointers
                            __ASM_EMIT("add $0x10, %0")
                            __ASM_EMIT("add $0x10, %1")
                            __ASM_EMIT("dec %2")
                            __ASM_EMIT("jnz 1b")

                            : "+S" (src), "+D"(dst), "+r"(regs) :
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

                            __ASM_EMIT("subps 0x00(%1), %%xmm0")
                            __ASM_EMIT("subps 0x10(%1), %%xmm1")
                            __ASM_EMIT("subps 0x20(%1), %%xmm2")
                            __ASM_EMIT("subps 0x30(%1), %%xmm3")

                            __ASM_EMIT("mulps %%xmm4, %%xmm0")
                            __ASM_EMIT("mulps %%xmm5, %%xmm1")
                            __ASM_EMIT("mulps %%xmm6, %%xmm2")
                            __ASM_EMIT("mulps %%xmm7, %%xmm3")

                            __ASM_EMIT("addps 0x00(%1), %%xmm0")
                            __ASM_EMIT("addps 0x10(%1), %%xmm1")
                            __ASM_EMIT("addps 0x20(%1), %%xmm2")
                            __ASM_EMIT("addps 0x30(%1), %%xmm3")

                            __ASM_EMIT("movntps %%xmm0,  0x00(%1)")
                            __ASM_EMIT("movntps %%xmm1,  0x10(%1)")
                            __ASM_EMIT("movntps %%xmm2,  0x20(%1)")
                            __ASM_EMIT("movntps %%xmm3,  0x30(%1)")

                            // Move pointers
                            __ASM_EMIT("add $0x40, %0")
                            __ASM_EMIT("add $0x40, %1")

                            // Repeat loop
                            __ASM_EMIT("dec %2")
                            __ASM_EMIT("jnz 1b")

                            : "+S" (src), "+D"(dst), "+r"(blocks) :
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
                            __ASM_EMIT("subps (%1), %%xmm0")
                            __ASM_EMIT("mulps %%xmm4, %%xmm0")
                            __ASM_EMIT("addps (%1), %%xmm0")
                            __ASM_EMIT("movntps %%xmm0, (%1)")

                            // Move pointers
                            __ASM_EMIT("add $0x10, %0")
                            __ASM_EMIT("add $0x10, %1")

                            // Repeat loop
                            __ASM_EMIT("dec %2")
                            __ASM_EMIT("jnz 1b")

                            : "+S" (src), "+D"(dst), "+r"(regs) :
                            : "cc", "memory",
                              "%xmm0"
                        );
                    }
                }
            }

            // Process tail
            if (tail > 0)
            {
                __asm__ __volatile__
                (
                    __ASM_EMIT("1:")

                    // Process data
                    __ASM_EMIT("movss (%0), %%xmm0")
                    __ASM_EMIT("subss (%1), %%xmm0")
                    __ASM_EMIT("mulss %%xmm4, %%xmm0")
                    __ASM_EMIT("addss (%1), %%xmm0")
                    __ASM_EMIT("movss %%xmm0, (%1)")

                    // Move pointers
                    __ASM_EMIT("add $0x4, %0")
                    __ASM_EMIT("add $0x4, %1")

                    // Repeat loop
                    __ASM_EMIT("dec %2")
                    __ASM_EMIT("jnz 1b")

                    : "+S" (src), "+D"(dst), "+r"(tail) :
                    : "cc", "memory",
                      "%xmm0"
                );
            }

            __asm__ __volatile__
            (
                __ASM_EMIT("sfence")
            );
        }

        void mix(float *dst, const float *src1, const float *src2, float k1, float k2, size_t count)
        {
            __asm__ __volatile__
            (
                __ASM_EMIT("movss %0, %%xmm3")
                __ASM_EMIT("movss %1, %%xmm7")
                : : "x" (k1) , "x"(k2)
                : "%xmm3", "%xmm7"
            );

            // Align destination
            while (true)
            {
                if (sse_aligned(dst))
                    break;
                if (count-- <= 0)
                    return;
                __asm__ __volatile__
                (
                    __ASM_EMIT("movss (%0), %%xmm0")
                    __ASM_EMIT("movss (%1), %%xmm4")
                    __ASM_EMIT("mulss %%xmm3, %%xmm0")
                    __ASM_EMIT("mulss %%xmm7, %%xmm4")
                    __ASM_EMIT("addss %%xmm4, %%xmm0")
                    __ASM_EMIT("movss %%xmm0, (%2)")

                    // Move pointers
                    __ASM_EMIT("add $0x4, %0")
                    __ASM_EMIT("add $0x4, %1")
                    __ASM_EMIT("add $0x4, %2")

                    : "+S" (src1), "+D"(src2), "+r"(dst) :
                    : "cc", "memory",
                      "%xmm0", "%xmm4"
                );
            }

            // Calculate amount of registers
            size_t regs = count / SSE_MULTIPLE;
            size_t tail = count % SSE_MULTIPLE;

            if (regs > 0)
            {
                // Calculate amount of 4-register blocks (0x40 bytes)
                size_t blocks   = regs / 2;
                regs           %= 2;

                // Prefetch data for iteration
                __asm__ __volatile__
                (
                    __ASM_EMIT("shufps $0, %%xmm3, %%xmm3")
                    __ASM_EMIT("shufps $0, %%xmm7, %%xmm7")

                    __ASM_EMIT("prefetchnta 0x00(%0)")
                    __ASM_EMIT("prefetchnta 0x00(%1)")
                    : : "S" (src1), "D" (src2)
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
                            __ASM_EMIT("prefetchnta 0x20(%0)")
                            __ASM_EMIT("prefetchnta 0x20(%1)")

                            // Calculate
                            __ASM_EMIT("movaps 0x00(%0), %%xmm0")
                            __ASM_EMIT("movaps 0x10(%0), %%xmm1")
                            __ASM_EMIT("movaps 0x00(%1), %%xmm4")
                            __ASM_EMIT("movaps 0x10(%1), %%xmm5")

                            __ASM_EMIT("mulps %%xmm2, %%xmm0")
                            __ASM_EMIT("mulps %%xmm3, %%xmm1")
                            __ASM_EMIT("mulps %%xmm6, %%xmm4")
                            __ASM_EMIT("mulps %%xmm7, %%xmm5")

                            __ASM_EMIT("addps %%xmm4, %%xmm0")
                            __ASM_EMIT("addps %%xmm5, %%xmm1")

                            __ASM_EMIT("movntps %%xmm0,  0x00(%2)")
                            __ASM_EMIT("movntps %%xmm1,  0x10(%2)")

                            // Move pointers
                            __ASM_EMIT("add $0x20, %0")
                            __ASM_EMIT("add $0x20, %1")
                            __ASM_EMIT("add $0x20, %2")
                            __ASM_EMIT("dec %3")
                            __ASM_EMIT("jnz 1b")

                            : "+S" (src1), "+D"(src2), "+r"(dst), "+r"(blocks) :
                            : "cc", "memory",
                              "%xmm0", "%xmm1", "%xmm2", "%xmm4", "%xmm5", "%xmm6"
                        );
                    }

                    if (regs > 0)
                    {
                        __asm__ __volatile__
                        (
                            // Copy data
                            __ASM_EMIT("movaps (%0), %%xmm0")
                            __ASM_EMIT("movaps (%1), %%xmm4")
                            __ASM_EMIT("mulps %%xmm3, %%xmm0")
                            __ASM_EMIT("mulps %%xmm7, %%xmm4")
                            __ASM_EMIT("addps %%xmm4, %%xmm0")
                            __ASM_EMIT("movntps %%xmm0, (%2)")

                            // Move pointers
                            __ASM_EMIT("add $0x10, %0")
                            __ASM_EMIT("add $0x10, %1")
                            __ASM_EMIT("add $0x10, %2")

                            : "+S" (src1), "+D"(src2), "+r"(dst) :
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
                            __ASM_EMIT("prefetchnta 0x20(%0)")
                            __ASM_EMIT("prefetchnta 0x20(%1)")

                            // Calculate
                            __ASM_EMIT("movups 0x00(%0), %%xmm0")
                            __ASM_EMIT("movups 0x10(%0), %%xmm1")
                            __ASM_EMIT("movups 0x00(%1), %%xmm4")
                            __ASM_EMIT("movups 0x10(%1), %%xmm5")

                            __ASM_EMIT("mulps %%xmm2, %%xmm0")
                            __ASM_EMIT("mulps %%xmm3, %%xmm1")
                            __ASM_EMIT("mulps %%xmm6, %%xmm4")
                            __ASM_EMIT("mulps %%xmm7, %%xmm5")

                            __ASM_EMIT("addps %%xmm4, %%xmm0")
                            __ASM_EMIT("addps %%xmm5, %%xmm1")

                            __ASM_EMIT("movntps %%xmm0,  0x00(%2)")
                            __ASM_EMIT("movntps %%xmm1,  0x10(%2)")

                            // Move pointers
                            __ASM_EMIT("add $0x20, %0")
                            __ASM_EMIT("add $0x20, %1")
                            __ASM_EMIT("add $0x20, %2")
                            __ASM_EMIT("dec %3")
                            __ASM_EMIT("jnz 1b")

                            : "+S" (src1), "+D"(src2), "+r"(dst), "+r"(blocks) :
                            : "cc", "memory",
                              "%xmm0", "%xmm1", "%xmm2", "%xmm4", "%xmm5", "%xmm6"
                        );
                    }

                    if (regs > 0)
                    {
                        __asm__ __volatile__
                        (
                            // Copy data
                            __ASM_EMIT("movups (%0), %%xmm0")
                            __ASM_EMIT("movups (%1), %%xmm4")
                            __ASM_EMIT("mulps %%xmm3, %%xmm0")
                            __ASM_EMIT("mulps %%xmm7, %%xmm4")
                            __ASM_EMIT("addps %%xmm4, %%xmm0")
                            __ASM_EMIT("movntps %%xmm0, (%2)")

                            // Move pointers
                            __ASM_EMIT("add $0x10, %0")
                            __ASM_EMIT("add $0x10, %1")
                            __ASM_EMIT("add $0x10, %2")

                            : "+S" (src1), "+D"(src2), "+r"(dst) :
                            : "cc", "memory",
                              "%xmm0", "%xmm4"
                        );
                    }
                }
            }

            // Process tail
            if (tail > 0)
            {
                __asm__ __volatile__
                (
                    __ASM_EMIT("1:")

                    // Process data
                    __ASM_EMIT("movss (%0), %%xmm0")
                    __ASM_EMIT("movss (%1), %%xmm4")
                    __ASM_EMIT("mulss %%xmm3, %%xmm0")
                    __ASM_EMIT("mulss %%xmm7, %%xmm4")
                    __ASM_EMIT("addss %%xmm4, %%xmm0")
                    __ASM_EMIT("movss %%xmm0, (%2)")

                    // Move pointers
                    __ASM_EMIT("add $0x4, %0")
                    __ASM_EMIT("add $0x4, %1")
                    __ASM_EMIT("add $0x4, %2")

                    // Repeat loop
                    __ASM_EMIT("dec %3")
                    __ASM_EMIT("jnz 1b")

                    : "+S" (src1), "+D"(src1), "+r"(dst), "+r"(tail) :
                    : "cc", "memory",
                      "%xmm0", "%xmm4"
                );
            }

            __asm__ __volatile__
            (
                __ASM_EMIT("sfence")
            );
        }

        void dsp_init()
        {
            lsp_trace("Optimizing DSP for SSE instruction set");

            dsp::copy                       = sse::copy;
            dsp::move                       = sse::move;
            dsp::fill                       = sse::fill;
            dsp::fill_one                   = sse::fill_one;
            dsp::fill_zero                  = sse::fill_zero;
            dsp::fill_minus_one             = sse::fill_minus_one;
    //        dsp::abs                        = sse::abs;
    //        dsp::abs_normalized             = sse::abs_normalized;
    //        dsp::normalize                  = sse::normalize;
    //        dsp::min                        = sse::min;
    //        dsp::max                        = sse::max;
    //        dsp::minmax                     = sse::minmax;
    //        dsp::min_index                  = sse::min_index;
    //        dsp::max_index                  = sse::max_index;
            dsp::multiply                   = sse::multiply;
    //        dsp::scalar_mul                 = sse::scalar_mul;
    //        dsp::accumulate                 = sse::accumulate;
            dsp::add_multiplied             = sse::add_multiplied;
            dsp::sub_multiplied             = sse::sub_multiplied;
            dsp::integrate                  = sse::integrate;
            dsp::mix                        = sse::mix;
        }
    }

}


#endif /* CORE_DSP_SSE_H_ */
