/*
 * vector.h
 *
 *  Created on: 05 окт. 2015 г.
 *      Author: sadko
 */

#ifndef CORE_X86_AVX256_VECTOR_H_
#define CORE_X86_AVX256_VECTOR_H_

namespace lsp
{
    namespace avx256
    {
        static void multiply(float *dst, const float *src1, const float *src2, size_t count)
        {
            if (count == 0)
                return;

            __asm__ __volatile__
            (
                __ASM_EMIT("1:")

                // Check conditions
                __ASM_EMIT("test $0x01f, %2")
                __ASM_EMIT("jz 2f")

                // Process data
                __ASM_EMIT("vmovss (%[src1]), %%xmm0")
                __ASM_EMIT("vmulss (%[src2]), %%xmm0, %%xmm0")
                __ASM_EMIT("vmovss %%xmm0, (%[dst])")

                // Move pointers
                __ASM_EMIT("add $0x4, %[src1]")
                __ASM_EMIT("add $0x4, %[src2]")
                __ASM_EMIT("add $0x4, %[dst]")

                // Repeat loop
                __ASM_EMIT("dec %[count]")
                __ASM_EMIT("jnz 1b")
                __ASM_EMIT("2:")

                : [src1] "+r" (src1), [src2] "+r"(src2), [dst]"+r"(dst), [count]"+r" (count) :
                : "cc", "memory",
                  "%xmm0", "%xmm1"
            );

            size_t regs     = count / AVX256_MULTIPLE;
            count          %= AVX256_MULTIPLE;

            if (regs > 0)
            {
                #ifdef __i386__
                size_t blocks   = regs / 4;
                regs           %= 4;
                #else
                size_t blocks   = regs / 8;
                regs           %= 8;
                #endif /* __i386__ */

                // Prefetch data for iteration
                __asm__ __volatile__
                (
                    __ASM_EMIT      ("prefetchnta  0x000(%[src1])")
                    __ASM_EMIT      ("prefetchnta  0x040(%[src1])")
                    __ASM_EMIT64    ("prefetchnta  0x080(%[src1])")
                    __ASM_EMIT64    ("prefetchnta  0x0c0(%[src1])")
                    __ASM_EMIT      ("prefetchnta  0x000(%[src2])")
                    __ASM_EMIT      ("prefetchnta  0x040(%[src2])")
                    __ASM_EMIT64    ("prefetchnta  0x080(%[src2])")
                    __ASM_EMIT64    ("prefetchnta  0x0c0(%[src2])")
                    : : [src1] "r" (src1), [src2] "r" (src2)
                );

                #define avx256_multiply(l_s1, l_s2)   \
                { \
                    if (blocks > 0) \
                    { \
                        __asm__ __volatile__ \
                        ( \
                            __ASM_EMIT("1:") \
                            \
                            __ASM_EMIT      ("prefetchnta  0x100(%[src1])") \
                            __ASM_EMIT      ("prefetchnta  0x140(%[src1])") \
                            __ASM_EMIT64    ("prefetchnta  0x180(%[src1])") \
                            __ASM_EMIT64    ("prefetchnta  0x1c0(%[src1])") \
                            __ASM_EMIT      ("prefetchnta  0x100(%[src2])") \
                            __ASM_EMIT      ("prefetchnta  0x140(%[src2])") \
                            __ASM_EMIT64    ("prefetchnta  0x180(%[src2])") \
                            __ASM_EMIT64    ("prefetchnta  0x1c0(%[src2])") \
                            /* Process data */ \
                            __ASM_EMIT      (l_s1 " 0x000(%[src1]), %%ymm0") \
                            __ASM_EMIT      (l_s1 " 0x020(%[src1]), %%ymm2") \
                            __ASM_EMIT      (l_s1 " 0x040(%[src1]), %%ymm4") \
                            __ASM_EMIT      (l_s1 " 0x060(%[src1]), %%ymm6") \
                            __ASM_EMIT64    (l_s1 " 0x080(%[src1]), %%ymm8") \
                            __ASM_EMIT64    (l_s1 " 0x0a0(%[src1]), %%ymm10") \
                            __ASM_EMIT64    (l_s1 " 0x0c0(%[src1]), %%ymm12") \
                            __ASM_EMIT64    (l_s1 " 0x0e0(%[src1]), %%ymm14") \
                            \
                            __ASM_EMIT      ("vmulps 0x000(%[src2]), %%ymm0, %%ymm0") \
                            __ASM_EMIT      ("vmulps 0x020(%[src2]), %%ymm2, %%ymm2") \
                            __ASM_EMIT      ("vmulps 0x040(%[src2]), %%ymm4, %%ymm4") \
                            __ASM_EMIT      ("vmulps 0x060(%[src2]), %%ymm6, %%ymm6") \
                            __ASM_EMIT64    ("vmulps 0x080(%[src2]), %%ymm8, %%ymm8") \
                            __ASM_EMIT64    ("vmulps 0x0a0(%[src2]), %%ymm10, %%ymm10") \
                            __ASM_EMIT64    ("vmulps 0x0c0(%[src2]), %%ymm12, %%ymm12") \
                            __ASM_EMIT64    ("vmulps 0x0e0(%[src2]), %%ymm14, %%ymm14") \
                            \
                            __ASM_EMIT      (VMOVNTPS " %%ymm0,  0x000(%[dst])") \
                            __ASM_EMIT      (VMOVNTPS " %%ymm2,  0x020(%[dst])") \
                            __ASM_EMIT      (VMOVNTPS " %%ymm4,  0x040(%[dst])") \
                            __ASM_EMIT      (VMOVNTPS " %%ymm6,  0x060(%[dst])") \
                            __ASM_EMIT64    (VMOVNTPS " %%ymm8,  0x080(%[dst])") \
                            __ASM_EMIT64    (VMOVNTPS " %%ymm10, 0x0a0(%[dst])") \
                            __ASM_EMIT64    (VMOVNTPS " %%ymm12, 0x0c0(%[dst])") \
                            __ASM_EMIT64    (VMOVNTPS " %%ymm14, 0x0e0(%[dst])") \
                            \
                            /* Move pointers */ \
                            __ASM_EMIT32    ("add $0x080, %[src1]") \
                            __ASM_EMIT32    ("add $0x080, %[src2]") \
                            __ASM_EMIT32    ("add $0x080, %[dst]") \
                            __ASM_EMIT64    ("add $0x100, %[src1]") \
                            __ASM_EMIT64    ("add $0x100, %[src2]") \
                            __ASM_EMIT64    ("add $0x100, %[dst]") \
                            \
                            /* Repeat loop */ \
                            __ASM_EMIT("dec %[count]") \
                            __ASM_EMIT("jnz 1b") \
                            __ASM_EMIT("2:") \
                            \
                            : [src1] "+r" (src1), [src2] "+r"(src2), [dst] "+r"(dst), [count] "+r" (blocks) : \
                            : "cc", "memory", \
                              __IF_64( \
                                "%xmm8", "%xmm9", "%xmm10", "%xmm11", \
                                "%xmm12", "%xmm13", "%xmm14", "%xmm15", \
                              ) \
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
                            __ASM_EMIT      (l_s1 " (%[src1]), %%ymm0") \
                            __ASM_EMIT      ("vmulps (%[src2]), %%ymm0, %%ymm0") \
                            __ASM_EMIT      (VMOVNTPS " %%ymm0, (%[dst])") \
                            \
                            /* Move pointers */ \
                            __ASM_EMIT      ("add $0x20, %[src1]") \
                            __ASM_EMIT      ("add $0x20, %[src2]") \
                            __ASM_EMIT      ("add $0x20, %[dst]") \
                            \
                            /* Repeat loop */ \
                            __ASM_EMIT      ("dec %[count]") \
                            __ASM_EMIT      ("jnz 1b") \
                            __ASM_EMIT      ("2:") \
                            \
                            : [src1] "+r" (src1), [src2] "+r"(src2), [dst] "+r"(dst), [count] "+r" (regs) : \
                            : "cc", "memory", \
                              "%xmm0", "%xmm1" \
                        ); \
                    } \
                }

                if (avx256_aligned(src1))
                {
                    if (avx256_aligned(src2))
                        avx256_multiply("vmovaps", "vmovaps")
                    else
                        avx256_multiply("vmovaps", "vmovups")
                }
                else
                {
                    if (avx256_aligned(src2))
                        avx256_multiply("vmovups", "vmovaps")
                    else
                        avx256_multiply("vmovups", "vmovups")
                }

                #undef avx256_multiply
            }

            if (count > 0)
            {
                __asm__ __volatile__
                (
                    __ASM_EMIT("1:")

                    // Process data
                    __ASM_EMIT("vmovss (%[src1]), %%xmm0")
                    __ASM_EMIT("vmulss (%[src2]), %%xmm0, %%xmm0")
                    __ASM_EMIT("vmovss %%xmm0, (%[dst])")

                    // Move pointers
                    __ASM_EMIT("add $0x4, %[src1]")
                    __ASM_EMIT("add $0x4, %[src2]")
                    __ASM_EMIT("add $0x4, %[dst]")

                    // Repeat loop
                    __ASM_EMIT("dec %[count]")
                    __ASM_EMIT("jnz 1b")
                    __ASM_EMIT("2:")

                    : [src1] "+r" (src1), [src2] "+r"(src2), [dst]"+r"(dst), [count]"+r" (count) :
                    : "cc", "memory",
                      "%xmm0", "%xmm1"
                );
            }

            VZEROUPPER;
        }
    }

}

#endif /* CORE_X86_AVX256_VECTOR_H_ */
