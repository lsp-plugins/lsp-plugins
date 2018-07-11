/*
 * search.h
 *
 *  Created on: 05 окт. 2015 г.
 *      Author: sadko
 */

#ifndef CORE_X86_DSP_SEARCH_H_
#define CORE_X86_DSP_SEARCH_H_

namespace lsp
{
    namespace sse
    {
        float min(const float *src, size_t count)
        {
            if (count == 0)
                return 0.0f;

            // Init loop
            float result;

            __asm__ __volatile__
            (
                __ASM_EMIT("movss (%0), %%xmm0")

                /* Pre-check conditions */
                __ASM_EMIT("test $0xf, %0")
                __ASM_EMIT("jz 1f")

                /* Move pointer and check conditions again */
                __ASM_EMIT("add $0x4, %0")
                __ASM_EMIT("dec %1")
                __ASM_EMIT("jz 1f")

                /* Perform loop */
                __ASM_EMIT("2:")

                __ASM_EMIT("test $0xf, %0")
                __ASM_EMIT("jz 1f")

                /* Loop body */
                __ASM_EMIT("movss (%0), %%xmm4")
                __ASM_EMIT("minss %%xmm4, %%xmm0")

                /* Repeat loop */
                __ASM_EMIT("add $0x4, %0")
                __ASM_EMIT("dec %1")
                __ASM_EMIT("jnz 2b")

                __ASM_EMIT("1:")

                : "+r"(src), "+r"(count), "=Yz"(result)
                : : "cc", "%xmm4"
            );

            // Calculate amount of registers
            size_t regs     = count / SSE_MULTIPLE;
            count          %= SSE_MULTIPLE;

            if (regs > 0)
            {
                size_t blocks   = regs / 4;
                regs           %= 4;

                // Initialize register
                __asm__ __volatile__
                (
                    __ASM_EMIT("shufps $0x00, %%xmm0, %%xmm0")
                    __ASM_EMIT("prefetchnta 0x00(%0)")
                    __ASM_EMIT("prefetchnta 0x20(%0)")
                    : : "r" (src) : "%xmm0"
                );

                // Operate with blocks
                if (blocks > 0)
                {
                    __asm__ __volatile__
                    (
                        /* Prepare loop */
                        __ASM_EMIT("movaps %%xmm0, %%xmm1")
                        __ASM_EMIT("movaps %%xmm0, %%xmm2")
                        __ASM_EMIT("movaps %%xmm1, %%xmm3")

                        /* Loop */
                        __ASM_EMIT("1:")

                        /* Prefetch next data */
                        __ASM_EMIT("prefetchnta 0x40(%0)")
                        __ASM_EMIT("prefetchnta 0x60(%0)")

                        /* Process data */
                        __ASM_EMIT("movaps 0x00(%0), %%xmm4")
                        __ASM_EMIT("movaps 0x10(%0), %%xmm5")
                        __ASM_EMIT("movaps 0x20(%0), %%xmm6")
                        __ASM_EMIT("movaps 0x30(%0), %%xmm7")

                        __ASM_EMIT("minps %%xmm4, %%xmm0")
                        __ASM_EMIT("minps %%xmm5, %%xmm1")
                        __ASM_EMIT("minps %%xmm6, %%xmm2")
                        __ASM_EMIT("minps %%xmm7, %%xmm3")

                        /* Move pointer and repeat loop */
                        __ASM_EMIT("add $0x40, %0")
                        __ASM_EMIT("dec %1")
                        __ASM_EMIT("jnz 1b")

                        /* Post-process data */
                        __ASM_EMIT("minps %%xmm1, %%xmm0")
                        __ASM_EMIT("minps %%xmm3, %%xmm2")
                        __ASM_EMIT("minps %%xmm2, %%xmm0")

                        : "+r"(src), "+r"(blocks)
                        : : "cc",
                            "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                            "%xmm4", "%xmm5", "%xmm6", "%xmm7"
                    );
                }

                if (regs > 0)
                {
                    __asm__ __volatile__
                    (
                        /* Loop */
                        __ASM_EMIT("1:")

                        /* Process data */
                        __ASM_EMIT("movaps (%0), %%xmm4")
                        __ASM_EMIT("minps %%xmm4, %%xmm0")

                        /* Move pointer and repeat loop */
                        __ASM_EMIT("add $0x10, %0")
                        __ASM_EMIT("dec %1")
                        __ASM_EMIT("jnz 1b")

                        : "+r"(src), "+r"(regs)
                        : : "cc",
                            "%xmm0", "%xmm1"
                    );
                }

                // Post-process blocks
                __asm__ __volatile__
                (
                    __ASM_EMIT("movhlps %%xmm0, %%xmm4")
                    __ASM_EMIT("minps %%xmm4, %%xmm0")
                    __ASM_EMIT("movaps %%xmm0, %%xmm4")
                    __ASM_EMIT("shufps $0x55, %%xmm0, %%xmm0")
                    __ASM_EMIT("minps %%xmm4, %%xmm0")
                    : "=Yz"(result)
                    : : "%xmm4"
                );
            }

            if (count > 0)
            {
                __asm__ __volatile__
                (
                    /* Loop */
                    __ASM_EMIT("1:")

                    /* Process data */
                    __ASM_EMIT("movss (%0), %%xmm4")
                    __ASM_EMIT("minss %%xmm4, %%xmm0")

                    /* Move pointer and repeat loop */
                    __ASM_EMIT("add $0x04, %0")
                    __ASM_EMIT("dec %1")
                    __ASM_EMIT("jnz 1b")

                    : "+r"(src), "+r"(count), "=Yz"(result)
                    : : "cc", "%xmm4"
                );
            }

            return result;
        }

        float max(const float *src, size_t count)
        {
            if (count == 0)
                return 0.0f;

            // Init loop
            float result;

            __asm__ __volatile__
            (
                __ASM_EMIT("movss (%0), %%xmm0")

                /* Pre-check conditions */
                __ASM_EMIT("test $0xf, %0")
                __ASM_EMIT("jz 1f")

                /* Move pointer and check conditions again */
                __ASM_EMIT("add $0x4, %0")
                __ASM_EMIT("dec %1")
                __ASM_EMIT("jz 1f")

                /* Perform loop */
                __ASM_EMIT("2:")

                __ASM_EMIT("test $0xf, %0")
                __ASM_EMIT("jz 1f")

                /* Loop body */
                __ASM_EMIT("movss (%0), %%xmm4")
                __ASM_EMIT("maxss %%xmm4, %%xmm0")

                /* Repeat loop */
                __ASM_EMIT("add $0x4, %0")
                __ASM_EMIT("dec %1")
                __ASM_EMIT("jnz 2b")

                __ASM_EMIT("1:")

                : "+r"(src), "+r"(count), "=Yz"(result)
                : : "cc", "%xmm4"
            );

            // Calculate amount of registers
            size_t regs     = count / SSE_MULTIPLE;
            count          %= SSE_MULTIPLE;

            if (regs > 0)
            {
                size_t blocks   = regs / 4;
                regs           %= 4;

                // Initialize register
                __asm__ __volatile__
                (
                    __ASM_EMIT("shufps $0x00, %%xmm0, %%xmm0")
                    __ASM_EMIT("prefetchnta 0x00(%0)")
                    __ASM_EMIT("prefetchnta 0x20(%0)")
                    : : "r" (src) : "%xmm0"
                );

                // Operate with blocks
                if (blocks > 0)
                {
                    __asm__ __volatile__
                    (
                        /* Prepare loop */
                        __ASM_EMIT("movaps %%xmm0, %%xmm1")
                        __ASM_EMIT("movaps %%xmm0, %%xmm2")
                        __ASM_EMIT("movaps %%xmm1, %%xmm3")

                        /* Loop */
                        __ASM_EMIT("1:")

                        /* Prefetch next data */
                        __ASM_EMIT("prefetchnta 0x40(%0)")
                        __ASM_EMIT("prefetchnta 0x60(%0)")

                        /* Process data */
                        __ASM_EMIT("movaps 0x00(%0), %%xmm4")
                        __ASM_EMIT("movaps 0x10(%0), %%xmm5")
                        __ASM_EMIT("movaps 0x20(%0), %%xmm6")
                        __ASM_EMIT("movaps 0x30(%0), %%xmm7")

                        __ASM_EMIT("maxps %%xmm4, %%xmm0")
                        __ASM_EMIT("maxps %%xmm5, %%xmm1")
                        __ASM_EMIT("maxps %%xmm6, %%xmm2")
                        __ASM_EMIT("maxps %%xmm7, %%xmm3")

                        /* Move pointer and repeat loop */
                        __ASM_EMIT("add $0x40, %0")
                        __ASM_EMIT("dec %1")
                        __ASM_EMIT("jnz 1b")

                        /* Post-process data */
                        __ASM_EMIT("maxps %%xmm1, %%xmm0")
                        __ASM_EMIT("maxps %%xmm3, %%xmm2")
                        __ASM_EMIT("maxps %%xmm2, %%xmm0")

                        : "+r"(src), "+r"(blocks)
                        : : "cc",
                            "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                            "%xmm4", "%xmm5", "%xmm6", "%xmm7"
                    );
                }

                if (regs > 0)
                {
                    __asm__ __volatile__
                    (
                        /* Loop */
                        __ASM_EMIT("1:")

                        /* Process data */
                        __ASM_EMIT("movaps (%0), %%xmm4")
                        __ASM_EMIT("maxps %%xmm4, %%xmm0")

                        /* Move pointer and repeat loop */
                        __ASM_EMIT("add $0x10, %0")
                        __ASM_EMIT("dec %1")
                        __ASM_EMIT("jnz 1b")

                        : "+r"(src), "+r"(regs)
                        : : "cc",
                            "%xmm0", "%xmm1"
                    );
                }

                // Post-process blocks
                __asm__ __volatile__
                (
                    __ASM_EMIT("movhlps %%xmm0, %%xmm4")
                    __ASM_EMIT("maxps %%xmm4, %%xmm0")
                    __ASM_EMIT("movaps %%xmm0, %%xmm4")
                    __ASM_EMIT("shufps $0x55, %%xmm0, %%xmm0")
                    __ASM_EMIT("maxps %%xmm4, %%xmm0")
                    : "=Yz"(result)
                    : : "%xmm4"
                );
            }

            if (count > 0)
            {
                __asm__ __volatile__
                (
                    /* Loop */
                    __ASM_EMIT("1:")

                    /* Process data */
                    __ASM_EMIT("movss (%0), %%xmm4")
                    __ASM_EMIT("maxss %%xmm4, %%xmm0")

                    /* Move pointer and repeat loop */
                    __ASM_EMIT("add $0x04, %0")
                    __ASM_EMIT("dec %1")
                    __ASM_EMIT("jnz 1b")

                    : "+r"(src), "+r"(count), "=Yz"(result)
                    : : "cc", "%xmm4"
                );
            }

            return result;
        }

        void minmax(const float *src, size_t count, float *min, float *max)
        {
            if (count == 0)
                return;

            // Init loop
            __asm__ __volatile__
            (
                __ASM_EMIT("movss (%0), %%xmm0")
                __ASM_EMIT("movss %%xmm0, %%xmm2")

                /* Pre-check conditions */
                __ASM_EMIT("test $0xf, %0")
                __ASM_EMIT("jz 1f")

                /* Move pointer and check conditions again */
                __ASM_EMIT("add $0x4, %0")
                __ASM_EMIT("dec %1")
                __ASM_EMIT("jz 1f")

                /* Perform loop */
                __ASM_EMIT("2:")
                __ASM_EMIT("test $0xf, %0")
                __ASM_EMIT("jz 1f")

                /* Loop body */
                __ASM_EMIT("movss (%0), %%xmm4")
                __ASM_EMIT("minss %%xmm4, %%xmm0")
                __ASM_EMIT("maxss %%xmm4, %%xmm2")

                /* Repeat loop */
                __ASM_EMIT("add $0x4, %0")
                __ASM_EMIT("dec %1")
                __ASM_EMIT("jnz 2b")

                __ASM_EMIT("1:")

                : "+r"(src), "+r"(count)
                : : "cc", "%xmm4"
            );

            // Calculate amount of registers
            size_t regs     = count / SSE_MULTIPLE;
            count          %= SSE_MULTIPLE;

            if (regs > 0)
            {
                size_t blocks   = regs / 4;
                regs           %= 4;

                // Initialize register
                __asm__ __volatile__
                (
                    __ASM_EMIT("shufps $0x00, %%xmm0, %%xmm0")
                    __ASM_EMIT("shufps $0x00, %%xmm2, %%xmm2")
                    __ASM_EMIT("prefetchnta 0x00(%0)")
                    __ASM_EMIT("prefetchnta 0x20(%0)")
                    : : "r" (src) : "%xmm0"
                );

                // Operate with blocks
                if (blocks > 0)
                {
                    __asm__ __volatile__
                    (
                        /* Prepare loop */
                        __ASM_EMIT("movaps %%xmm0, %%xmm1")
                        __ASM_EMIT("movaps %%xmm2, %%xmm3")

                        /* Loop */
                        __ASM_EMIT("1:")

                        /* Prefetch next data */
                        __ASM_EMIT("prefetchnta 0x40(%0)")
                        __ASM_EMIT("prefetchnta 0x60(%0)")

                        /* Process data */
                        __ASM_EMIT("movaps 0x00(%0), %%xmm4")
                        __ASM_EMIT("movaps 0x10(%0), %%xmm5")
                        __ASM_EMIT("movaps 0x20(%0), %%xmm6")
                        __ASM_EMIT("movaps 0x30(%0), %%xmm7")

                        __ASM_EMIT("minps %%xmm4, %%xmm0")
                        __ASM_EMIT("minps %%xmm5, %%xmm1")
                        __ASM_EMIT("maxps %%xmm6, %%xmm2")
                        __ASM_EMIT("maxps %%xmm7, %%xmm3")
                        __ASM_EMIT("maxps %%xmm4, %%xmm2")
                        __ASM_EMIT("maxps %%xmm5, %%xmm3")
                        __ASM_EMIT("minps %%xmm6, %%xmm0")
                        __ASM_EMIT("minps %%xmm7, %%xmm1")

                        /* Move pointer and repeat loop */
                        __ASM_EMIT("add $0x40, %0")
                        __ASM_EMIT("dec %1")
                        __ASM_EMIT("jnz 1b")

                        /* Post-process data */
                        __ASM_EMIT("minps %%xmm1, %%xmm0")
                        __ASM_EMIT("maxps %%xmm3, %%xmm2")

                        : "+r"(src), "+r"(blocks)
                        : : "cc",
                            "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                            "%xmm4", "%xmm5", "%xmm6", "%xmm7"
                    );
                }

                if (regs > 0)
                {
                    __asm__ __volatile__
                    (
                        /* Loop */
                        __ASM_EMIT("1:")

                        /* Process data */
                        __ASM_EMIT("movaps (%0), %%xmm4")
                        __ASM_EMIT("minps %%xmm4, %%xmm0")
                        __ASM_EMIT("maxps %%xmm4, %%xmm2")

                        /* Move pointer and repeat loop */
                        __ASM_EMIT("add $0x10, %0")
                        __ASM_EMIT("dec %1")
                        __ASM_EMIT("jnz 1b")

                        : "+r"(src), "+r"(regs)
                        : : "cc",
                            "%xmm0", "%xmm1"
                    );
                }

                // Post-process blocks
                __asm__ __volatile__
                (
                    __ASM_EMIT("movhlps %%xmm0, %%xmm1")
                    __ASM_EMIT("movhlps %%xmm2, %%xmm3")
                    __ASM_EMIT("minps %%xmm1, %%xmm0")
                    __ASM_EMIT("maxps %%xmm3, %%xmm2")
                    __ASM_EMIT("movaps %%xmm0, %%xmm1")
                    __ASM_EMIT("movaps %%xmm2, %%xmm3")
                    __ASM_EMIT("shufps $0x55, %%xmm0, %%xmm0")
                    __ASM_EMIT("shufps $0x55, %%xmm2, %%xmm2")
                    __ASM_EMIT("minps %%xmm1, %%xmm0")
                    __ASM_EMIT("maxps %%xmm3, %%xmm2")
                    : : : "%xmm0", "%xmm1", "%xmm2", "%xmm3"
                );
            }

            if (count > 0)
            {
                __asm__ __volatile__
                (
                    /* Loop */
                    __ASM_EMIT("1:")

                    /* Process data */
                    __ASM_EMIT("movss (%0), %%xmm4")
                    __ASM_EMIT("minss %%xmm4, %%xmm0")
                    __ASM_EMIT("maxss %%xmm4, %%xmm2")

                    /* Move pointer and repeat loop */
                    __ASM_EMIT("add $0x04, %0")
                    __ASM_EMIT("dec %1")
                    __ASM_EMIT("jnz 1b")

                    : "+r"(src), "+r"(count)
                    : : "cc", "%xmm4"
                );
            }

            // Store result
            __asm__ __volatile__
            (
                __ASM_EMIT("movss %%xmm0, (%0)")
                __ASM_EMIT("movss %%xmm2, (%1)")
                :
                : "r"(min), "r"(max)
                : "memory"
            );
        }

        #define SSE_MINMAX_ABS_CORE(op)    \
            float result; \
            \
            __asm__ __volatile__ \
            ( \
                /* Prepare */ \
                __ASM_EMIT("xorps       %%xmm0, %%xmm0") \
                __ASM_EMIT("test        %[count], %[count]") \
                __ASM_EMIT("jz          8f") \
                \
                __ASM_EMIT("movss       (%[src]), %%xmm0") \
                __ASM_EMIT("movaps      %[X_SIGN], %%xmm1") \
                __ASM_EMIT("andps       %%xmm1, %%xmm0") \
                \
                /* Loop 1: align source to 4x boundary */ \
                __ASM_EMIT("1:") \
                __ASM_EMIT("test        $0x0f, %[src]") \
                __ASM_EMIT("jz          2f") \
                __ASM_EMIT("movss       (%[src]), %%xmm2") \
                __ASM_EMIT("andps       %%xmm1, %%xmm2") \
                __ASM_EMIT(op "ss       %%xmm2, %%xmm0") \
                __ASM_EMIT("add         $0x4, %[src]") \
                __ASM_EMIT("dec         %[count]") \
                __ASM_EMIT("jnz         1b") \
                __ASM_EMIT("jmp         8f") \
                \
                /* Prepare vectorization */ \
                __ASM_EMIT("2:") \
                __ASM_EMIT("cmp         $4, %[count]") \
                __ASM_EMIT("jb          7f") \
                \
                __ASM_EMIT("prefetchnta 0x00(%[src])") \
                __ASM_EMIT("prefetchnta 0x20(%[src])") \
                __ASM_EMIT("prefetchnta 0x40(%[src])") \
                \
                __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0") \
                __ASM_EMIT("cmp         $24, %[count]") \
                __ASM_EMIT("jb          4f") \
                \
                /* Loop 2: 6 * 4x elements */ \
                __ASM_EMIT("3:") \
                __ASM_EMIT("prefetchnta 0x60(%[src])") \
                __ASM_EMIT("prefetchnta 0x80(%[src])") \
                __ASM_EMIT("prefetchnta 0xa0(%[src])") \
                __ASM_EMIT("movaps      0x00(%[src]), %%xmm2") \
                __ASM_EMIT("movaps      0x10(%[src]), %%xmm3") \
                __ASM_EMIT("movaps      0x20(%[src]), %%xmm4") \
                __ASM_EMIT("movaps      0x30(%[src]), %%xmm5") \
                __ASM_EMIT("movaps      0x40(%[src]), %%xmm6") \
                __ASM_EMIT("movaps      0x50(%[src]), %%xmm7") \
                __ASM_EMIT("andps       %%xmm1, %%xmm2") \
                __ASM_EMIT("andps       %%xmm1, %%xmm3") \
                __ASM_EMIT("andps       %%xmm1, %%xmm4") \
                __ASM_EMIT("andps       %%xmm1, %%xmm5") \
                __ASM_EMIT("andps       %%xmm1, %%xmm6") \
                __ASM_EMIT("andps       %%xmm1, %%xmm7") \
                __ASM_EMIT(op "ps       %%xmm3, %%xmm2") \
                __ASM_EMIT(op "ps       %%xmm5, %%xmm4") \
                __ASM_EMIT(op "ps       %%xmm7, %%xmm6") \
                __ASM_EMIT(op "ps       %%xmm2, %%xmm0") \
                __ASM_EMIT(op "ps       %%xmm6, %%xmm4") \
                __ASM_EMIT(op "ps       %%xmm4, %%xmm0") \
                \
                __ASM_EMIT("sub         $24, %[count]") \
                __ASM_EMIT("add         $0x60, %[src]") \
                __ASM_EMIT("cmp         $24, %[count]") \
                __ASM_EMIT("jae         3b") \
                \
                /* Loop 3: 4x elements */ \
                __ASM_EMIT("4:") \
                __ASM_EMIT("cmp         $4, %[count]") \
                __ASM_EMIT("jb          6f") \
                \
                __ASM_EMIT("5:") \
                __ASM_EMIT("movaps      0x00(%[src]), %%xmm2") \
                __ASM_EMIT("andps       %%xmm1, %%xmm2") \
                __ASM_EMIT(op "ps       %%xmm2, %%xmm0") \
                __ASM_EMIT("sub         $4, %[count]") \
                __ASM_EMIT("add         $0x10, %[src]") \
                __ASM_EMIT("cmp         $4, %[count]") \
                __ASM_EMIT("jae         5b") \
                \
                /* De-vectorize */ \
                __ASM_EMIT("6:") \
                __ASM_EMIT("movhlps     %%xmm0, %%xmm2") \
                __ASM_EMIT(op "ps       %%xmm2, %%xmm0") \
                __ASM_EMIT("movaps      %%xmm0, %%xmm2") \
                __ASM_EMIT("shufps      $0x55, %%xmm0, %%xmm0") \
                __ASM_EMIT(op "ss       %%xmm2, %%xmm0") \
                \
                /* Loop 4: 1x tail */ \
                __ASM_EMIT("test        %[count], %[count]") \
                __ASM_EMIT("jz          8f") \
                \
                __ASM_EMIT("7:") \
                __ASM_EMIT("movss       (%[src]), %%xmm2") \
                __ASM_EMIT("andps       %%xmm1, %%xmm2") \
                __ASM_EMIT(op "ss       %%xmm2, %%xmm0") \
                __ASM_EMIT("add         $0x4, %[src]") \
                __ASM_EMIT("dec         %[count]") \
                __ASM_EMIT("jnz         7b") \
                \
                /* End */ \
                __ASM_EMIT("8:") \
                \
                : [src] "+r" (src), [count] "+r" (count), "=Yz" (result) \
                : [X_SIGN] "m" (X_SIGN) \
                : "cc", \
                    "%xmm1", "%xmm2", "%xmm3", \
                    "%xmm4", "%xmm5", "%xmm6", "%xmm7" \
            ); \
            \
            return result;


        float abs_min(const float *src, size_t count)
        {
            SSE_MINMAX_ABS_CORE("min");
        }

        float abs_max(const float *src, size_t count)
        {
            SSE_MINMAX_ABS_CORE("max");
        }

        #undef SSE_MINMAX_ABS_CORE
    }
}

#endif /* CORE_X86_DSP_SEARCH_H_ */
