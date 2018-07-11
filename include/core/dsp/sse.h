/*
 * native_dsp.hpp
 *
 *  Created on: 05 окт. 2015 г.
 *      Author: sadko
 */

#ifndef CORE_DSP_SSE_H_
#define CORE_DSP_SSE_H_

#include <core/dsp.h>

namespace lsp
{
    class sse_dsp: public native_dsp
    {
        private:
            static const size_t SSE_MULTIPLE        = 4;
            static const size_t SSE_ALIGN           = SSE_MULTIPLE * sizeof(float);

        public:
            sse_dsp() {};

            static dsp *instance()
            {
                return new sse_dsp();
            }

            inline bool sse_aligned(const void *ptr) { return !(ptrdiff_t(ptr) & (SSE_ALIGN - 1)); };
            inline size_t sse_multiple(size_t count) { return count & (SSE_MULTIPLE - 1); }

        public:
            virtual void copy(float *dst, const float *src, size_t count)
            {
                if (dst == src)
                    return;

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

            virtual void copy_multiplied(float *dst, const float *src, float k, size_t count)
            {
                __asm__ __volatile__
                (
                    __ASM_EMIT("movss %0, %%xmm4")
                    : : "m" (k)
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

            virtual void fill(float *dst, float value, size_t count)
            {
                __asm__ __volatile__
                (
                    __ASM_EMIT("movss %0, %%xmm0")
                    : : "m" (value)
                    : "%xmm0"
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
                        __ASM_EMIT("movss %%xmm0, (%0)")

                        // Move pointers
                        __ASM_EMIT("add $0x4, %0")

                        : "+D"(dst) :
                        : "cc", "memory"
                    );
                }

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

                        __ASM_EMIT("movss %%xmm0, (%1)")

                        // Move pointer
                        __ASM_EMIT("add $0x4, %0")
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

            virtual void fill_zero(float *dst, size_t count)
            {
                return sse_dsp::fill(dst, 0.0f, count);
            }

            virtual void fill_one(float *dst, size_t count)
            {
                return sse_dsp::fill(dst, 1.0f, count);
            }

            virtual void fill_minus_one(float *dst, size_t count)
            {
                return sse_dsp::fill(dst, -1.0f, count);
            }

            virtual void add_multiplied(float *dst, const float *src, float k, size_t count)
            {
                __asm__ __volatile__
                (
                    __ASM_EMIT("movss %0, %%xmm4")
                    : : "m" (k)
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

            virtual void sub_multiplied(float *dst, const float *src, float k, size_t count)
            {
                return sse_dsp::add_multiplied(dst, src, -k, count);
            }

            virtual void integrate(float *dst, const float *src, float k, size_t count)
            {
                __asm__ __volatile__
                (
                    __ASM_EMIT("movss %0, %%xmm4")
                    : : "m" (k)
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

            virtual void mix(float *dst, const float *src1, const float *src2, float k1, float k2, size_t count)
            {
                __asm__ __volatile__
                (
                    __ASM_EMIT("movss %0, %%xmm3")
                    __ASM_EMIT("movss %1, %%xmm7")
                    : : "m" (k1) , "m"(k2)
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

//
//            virtual float min(const float *src, size_t count)
//            {
//                size_t blocks   = count / (4 * SSE_MULTIPLE);
//                size_t tail     = count % (4 * SSE_MULTIPLE);
//
//                if (sse_aligned(src))
//                {
//                    if (blocks > 0)
//                    {
//                        __asm__ __volatile
//                        (
//                            // Prefetch values
//                            __ASM_EMIT("prefetchnta 0x00(%0)")
//                            __ASM_EMIT("prefetchnta 0x20(%0)")
//                            __ASM_EMIT("prefetchnta 0x40(%0)")
//                            __ASM_EMIT("prefetchnta 0x60(%0)")
//
//                            // Fetch registers
//                            __ASM_EMIT("movaps 0x00(%0), %%xmm0")
//                            __ASM_EMIT("movaps 0x10(%0), %%xmm1")
//                            __ASM_EMIT("movaps 0x20(%0), %%xmm2")
//                            __ASM_EMIT("movaps 0x30(%0), %%xmm3")
//                            __ASM_EMIT("add $0x40, %0")
//                            __ASM_EMIT("dec %1")
//                            __ASM_EMIT("jz 2f")
//
//                            __ASM_EMIT("1:")
//
//                            __ASM_EMIT("prefetchnta 0x40(%0)")
//                            __ASM_EMIT("prefetchnta 0x60(%0)")
//
//                            __ASM_EMIT("movaps 0x00(%0), %%xmm4")
//                            __ASM_EMIT("movaps 0x10(%0), %%xmm5")
//                            __ASM_EMIT("movaps 0x20(%0), %%xmm6")
//                            __ASM_EMIT("movaps 0x30(%0), %%xmm7")
//
//                            __ASM_EMIT("minps  %%xmm4, %%xmm0")
//                            __ASM_EMIT("minps  %%xmm5, %%xmm1")
//                            __ASM_EMIT("minps  %%xmm6, %%xmm2")
//                            __ASM_EMIT("minps  %%xmm7, %%xmm3")
//
//                            __ASM_EMIT("add $0x40, %0")
//                            __ASM_EMIT("dec %1")
//                            __ASM_EMIT("jnz 1b")
//
//                            __ASM_EMIT("2:")
//                            __ASM_EMIT("minps  %%xmm1, %%xmm0")
//                            __ASM_EMIT("minps  %%xmm3, %%xmm2")
//                            __ASM_EMIT("minps  %%xmm2, %%xmm0")
//
//                            : "+S" (src), "+r"(blocks)
//                            : "cc", "memory",
//                              "%xmm0", "%xmm1", "%xmm2", "%xmm3"
//                        );
//                    }
//                    else
//                    {
//                    }
//                }
//                else
//                {
//                }
//            }
    };

}


#endif /* CORE_DSP_SSE_H_ */
