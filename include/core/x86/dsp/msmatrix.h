/*
 * msmatrix.h
 *
 *  Created on: 9 нояб. 2016 г.
 *      Author: sadko
 */

#ifndef CORE_X86_DSP_MSMATRIX_H_
#define CORE_X86_DSP_MSMATRIX_H_

namespace lsp
{
    namespace sse
    {
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

        #define __LR_CVT_BODY(ld_l, ld_r, st_d, op) \
            __ASM_EMIT("cmp         $16, %[count]") \
            __ASM_EMIT("jb          200f") \
            \
            __ASM_EMIT("100:") \
            __ASM_EMIT("prefetchnta 0x40(%[left])") \
            __ASM_EMIT("prefetchnta 0x60(%[left])") \
            __ASM_EMIT("prefetchnta 0x40(%[right])") \
            __ASM_EMIT("prefetchnta 0x60(%[right])") \
            __ASM_EMIT(ld_l "       0x00(%[left]), %%xmm0")     /* xmm0 = l0 */ \
            __ASM_EMIT(ld_l "       0x10(%[left]), %%xmm2")     /* xmm2 = l1 */ \
            __ASM_EMIT(ld_l "       0x20(%[left]), %%xmm4")     /* xmm4 = l2 */ \
            __ASM_EMIT(ld_l "       0x30(%[left]), %%xmm6")     /* xmm6 = l3 */ \
            __ASM_EMIT(ld_r "       0x00(%[right]), %%xmm1")    /* xmm1 = r0 */ \
            __ASM_EMIT(ld_r "       0x10(%[right]), %%xmm3")    /* xmm3 = r1 */ \
            __ASM_EMIT(ld_r "       0x20(%[right]), %%xmm5")    /* xmm5 = r2 */ \
            __ASM_EMIT(ld_r "       0x30(%[right]), %%xmm7")    /* xmm7 = r3 */ \
            __ASM_EMIT(op "ps       %%xmm1, %%xmm0")            /* xmm0 = l <+-> r */ \
            __ASM_EMIT(op "ps       %%xmm3, %%xmm2")            /* xmm2 = l <+-> r */ \
            __ASM_EMIT(op "ps       %%xmm5, %%xmm4")            /* xmm4 = l <+-> r */ \
            __ASM_EMIT(op "ps       %%xmm7, %%xmm6")            /* xmm6 = l <+-> r */ \
            __ASM_EMIT("mulps       %[X_HALF], %%xmm0")         /* xmm0 = (l <+-> r) * 0.5f */ \
            __ASM_EMIT("mulps       %[X_HALF], %%xmm2")         /* xmm2 = (l <+-> r) * 0.5f */ \
            __ASM_EMIT("mulps       %[X_HALF], %%xmm4")         /* xmm4 = (l <+-> r) * 0.5f */ \
            __ASM_EMIT("mulps       %[X_HALF], %%xmm6")         /* xmm6 = (l <+-> r) * 0.5f */ \
            __ASM_EMIT(st_d "       %%xmm0, 0x00(%[dst])")      \
            __ASM_EMIT(st_d "       %%xmm2, 0x10(%[dst])")      \
            __ASM_EMIT(st_d "       %%xmm4, 0x20(%[dst])")      \
            __ASM_EMIT(st_d "       %%xmm6, 0x30(%[dst])")      \
            \
            __ASM_EMIT("sub         $16, %[count]") \
            __ASM_EMIT("add         $0x40, %[left]") \
            __ASM_EMIT("add         $0x40, %[right]") \
            __ASM_EMIT("add         $0x40, %[dst]") \
            __ASM_EMIT("cmp         $16, %[count]") \
            __ASM_EMIT("jae         100b") \
            \
            __ASM_EMIT("200:") \
            __ASM_EMIT("cmp         $4, %[count]") \
            __ASM_EMIT("jb          400f") \
            __ASM_EMIT("300:") \
            __ASM_EMIT(ld_l "       (%[left]), %%xmm0")         /* xmm0 = l0 */ \
            __ASM_EMIT(ld_r "       (%[right]), %%xmm1")        /* xmm1 = r0 */ \
            __ASM_EMIT(op "ps       %%xmm1, %%xmm0")            /* xmm0 = l <+-> r */ \
            __ASM_EMIT("mulps       %[X_HALF], %%xmm0")         /* xmm0 = (l <+-> r) * 0.5f */ \
            __ASM_EMIT(st_d "       %%xmm0, 0x00(%[dst])")      \
            __ASM_EMIT("sub         $4, %[count]") \
            __ASM_EMIT("add         $0x10, %[left]") \
            __ASM_EMIT("add         $0x10, %[right]") \
            __ASM_EMIT("add         $0x10, %[dst]") \
            __ASM_EMIT("cmp         $16, %[count]") \
            __ASM_EMIT("jae         300b") \
            \
            __ASM_EMIT("400:") \
            __ASM_EMIT("test        %[count], %[count]") \
            __ASM_EMIT("jz          1000f") \
            __ASM_EMIT("500:") \
            __ASM_EMIT("movss       (%[left]), %%xmm0")         /* xmm0 = l0 */ \
            __ASM_EMIT("movss       (%[right]), %%xmm1")        /* xmm1 = r0 */ \
            __ASM_EMIT(op "ss       %%xmm1, %%xmm0")            /* xmm0 = l <+-> r */ \
            __ASM_EMIT("mulss       %[X_HALF], %%xmm0")         /* xmm0 = (l <+-> r) * 0.5f */ \
            __ASM_EMIT("movss       %%xmm0, 0x00(%[dst])")      \
            __ASM_EMIT("add         $0x10, %[left]") \
            __ASM_EMIT("add         $0x10, %[right]") \
            __ASM_EMIT("add         $0x10, %[dst]") \
            __ASM_EMIT("dec         %[count]") \
            __ASM_EMIT("jnz         500b") \

        #define LR_CVT_BODY(dst_ptr, op) \
            __asm__ __volatile__ \
            ( \
                __ASM_EMIT("test        %[count], %[count]") \
                __ASM_EMIT("jz          1000f") \
                __ASM_EMIT("1:") \
                \
                __ASM_EMIT("test        $0x0f, %[left]") \
                __ASM_EMIT("jz          2f") \
                __ASM_EMIT("movss       (%[left]),  %%xmm0")  /* xmm0 = l */ \
                __ASM_EMIT("movss       (%[right]), %%xmm1")  /* xmm1 = r */ \
                __ASM_EMIT(op "ss       %%xmm1, %%xmm0")      /* xmm0 = l <+-> r */ \
                __ASM_EMIT("mulss       %[X_HALF], %%xmm0")   /* xmm0 = (l <+-> r) * 0.5f */ \
                __ASM_EMIT("movss       %%xmm0, (%[dst])")    \
                \
                __ASM_EMIT("add         $0x4, %[left]") \
                __ASM_EMIT("add         $0x4, %[right]") \
                __ASM_EMIT("add         $0x4, %[dst]") \
                __ASM_EMIT("dec         %[count]") \
                __ASM_EMIT("jnz         1b") \
                __ASM_EMIT("2:") \
                \
                __ASM_EMIT("prefetchnta  0x00(%[left])") \
                __ASM_EMIT("prefetchnta  0x20(%[left])") \
                __ASM_EMIT("prefetchnta  0x00(%[right])") \
                __ASM_EMIT("prefetchnta  0x20(%[right])") \
                \
                __ASM_EMIT("test $0x0f, %[right]") \
                __ASM_EMIT("jnz 2f") \
                    __ASM_EMIT("test $0x0f, %[dst]") \
                    __ASM_EMIT("jnz 1f") \
                        __LR_CVT_BODY("movaps", "movaps", "movaps", op) \
                        __ASM_EMIT("jmp 1000f") \
                    __ASM_EMIT("1:") \
                        __LR_CVT_BODY("movaps", "movaps", "movups", op) \
                        __ASM_EMIT("jmp 1000f") \
                __ASM_EMIT("2:") \
                    __ASM_EMIT("test $0x0f, %[dst]") \
                    __ASM_EMIT("jnz 3f") \
                        __LR_CVT_BODY("movaps", "movups", "movaps", op) \
                        __ASM_EMIT("jmp 1000f") \
                    __ASM_EMIT("3:") \
                        __LR_CVT_BODY("movaps", "movups", "movups", op) \
                __ASM_EMIT("1000:") \
                \
                : [left] "+r"(l), [right] "+r" (r), [dst] "+r" (dst_ptr), [count] "+r" (count) \
                : [X_HALF] "m" (X_HALF) \
                : "cc", "memory", \
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3", \
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7" \
            );

        static void lr_to_mid(float *m, const float *l, const float *r, size_t count)
        {
            LR_CVT_BODY(m, "add");
        }

        static void lr_to_side(float *s, const float *l, const float *r, size_t count)
        {
            LR_CVT_BODY(s, "sub");
        }

        #undef LR_CVT_BODY
        #undef __LR_CVT_BODY

        #define __MS_CVT_BODY(ld_l, ld_r, st_d, op) \
            __ASM_EMIT("cmp         $16, %[count]") \
            __ASM_EMIT("jb          200f") \
            \
            __ASM_EMIT("100:") \
            __ASM_EMIT("prefetchnta 0x40(%[mid])") \
            __ASM_EMIT("prefetchnta 0x60(%[mid])") \
            __ASM_EMIT("prefetchnta 0x40(%[side])") \
            __ASM_EMIT("prefetchnta 0x60(%[side])") \
            __ASM_EMIT(ld_l "       0x00(%[mid]), %%xmm0")      /* xmm0 = m0 */ \
            __ASM_EMIT(ld_l "       0x10(%[mid]), %%xmm2")      /* xmm2 = m1 */ \
            __ASM_EMIT(ld_l "       0x20(%[mid]), %%xmm4")      /* xmm4 = m2 */ \
            __ASM_EMIT(ld_l "       0x30(%[mid]), %%xmm6")      /* xmm6 = m3 */ \
            __ASM_EMIT(ld_r "       0x00(%[side]), %%xmm1")     /* xmm1 = s0 */ \
            __ASM_EMIT(ld_r "       0x10(%[side]), %%xmm3")     /* xmm3 = s1 */ \
            __ASM_EMIT(ld_r "       0x20(%[side]), %%xmm5")     /* xmm5 = s2 */ \
            __ASM_EMIT(ld_r "       0x30(%[side]), %%xmm7")     /* xmm7 = s3 */ \
            __ASM_EMIT(op "ps       %%xmm1, %%xmm0")            /* xmm0 = m <+-> s */ \
            __ASM_EMIT(op "ps       %%xmm3, %%xmm2")            /* xmm2 = m <+-> s */ \
            __ASM_EMIT(op "ps       %%xmm5, %%xmm4")            /* xmm4 = m <+-> s */ \
            __ASM_EMIT(op "ps       %%xmm7, %%xmm6")            /* xmm6 = m <+-> s */ \
            __ASM_EMIT(st_d "       %%xmm0, 0x00(%[dst])")      \
            __ASM_EMIT(st_d "       %%xmm2, 0x10(%[dst])")      \
            __ASM_EMIT(st_d "       %%xmm4, 0x20(%[dst])")      \
            __ASM_EMIT(st_d "       %%xmm6, 0x30(%[dst])")      \
            \
            __ASM_EMIT("sub         $16, %[count]") \
            __ASM_EMIT("add         $0x40, %[mid]") \
            __ASM_EMIT("add         $0x40, %[side]") \
            __ASM_EMIT("add         $0x40, %[dst]") \
            __ASM_EMIT("cmp         $16, %[count]") \
            __ASM_EMIT("jae         100b") \
            \
            __ASM_EMIT("200:") \
            __ASM_EMIT("cmp         $4, %[count]") \
            __ASM_EMIT("jb          400f") \
            __ASM_EMIT("300:") \
            __ASM_EMIT(ld_l "       (%[mid]), %%xmm0")          /* xmm0 = m0 */ \
            __ASM_EMIT(ld_r "       (%[side]), %%xmm1")         /* xmm1 = s0 */ \
            __ASM_EMIT(op "ps       %%xmm1, %%xmm0")            /* xmm0 = m <+-> s */ \
            __ASM_EMIT(st_d "       %%xmm0, 0x00(%[dst])")      \
            __ASM_EMIT("sub         $4, %[count]") \
            __ASM_EMIT("add         $0x10, %[mid]") \
            __ASM_EMIT("add         $0x10, %[side]") \
            __ASM_EMIT("add         $0x10, %[dst]") \
            __ASM_EMIT("cmp         $16, %[count]") \
            __ASM_EMIT("jae         300b") \
            \
            __ASM_EMIT("400:") \
            __ASM_EMIT("test        %[count], %[count]") \
            __ASM_EMIT("jz          1000f") \
            __ASM_EMIT("500:") \
            __ASM_EMIT("movss       (%[mid]), %%xmm0")          /* xmm0 = m0 */ \
            __ASM_EMIT("movss       (%[side]), %%xmm1")         /* xmm1 = s0 */ \
            __ASM_EMIT(op "ss       %%xmm1, %%xmm0")            /* xmm0 = m <+-> s */ \
            __ASM_EMIT("movss       %%xmm0, 0x00(%[dst])")      \
            __ASM_EMIT("add         $0x10, %[mid]") \
            __ASM_EMIT("add         $0x10, %[side]") \
            __ASM_EMIT("add         $0x10, %[dst]") \
            __ASM_EMIT("dec         %[count]") \
            __ASM_EMIT("jnz         500b") \

        #define MS_CVT_BODY(dst_ptr, op) \
            __asm__ __volatile__ \
            ( \
                __ASM_EMIT("test        %[count], %[count]") \
                __ASM_EMIT("jz          1000f") \
                __ASM_EMIT("1:") \
                \
                __ASM_EMIT("test        $0x0f, %[mid]") \
                __ASM_EMIT("jz          2f") \
                __ASM_EMIT("movss       (%[mid]),  %%xmm0")     /* xmm0 = m */ \
                __ASM_EMIT("movss       (%[side]), %%xmm1")     /* xmm1 = m */ \
                __ASM_EMIT(op "ss       %%xmm1, %%xmm0")        /* xmm0 = m <+-> s */ \
                __ASM_EMIT("movss       %%xmm0, (%[dst])")    \
                \
                __ASM_EMIT("add         $0x4, %[mid]") \
                __ASM_EMIT("add         $0x4, %[side]") \
                __ASM_EMIT("add         $0x4, %[dst]") \
                __ASM_EMIT("dec         %[count]") \
                __ASM_EMIT("jnz         1b") \
                __ASM_EMIT("2:") \
                \
                __ASM_EMIT("prefetchnta  0x00(%[mid])") \
                __ASM_EMIT("prefetchnta  0x20(%[mid])") \
                __ASM_EMIT("prefetchnta  0x00(%[side])") \
                __ASM_EMIT("prefetchnta  0x20(%[side])") \
                \
                __ASM_EMIT("test $0x0f, %[side]") \
                __ASM_EMIT("jnz 2f") \
                    __ASM_EMIT("test $0x0f, %[dst]") \
                    __ASM_EMIT("jnz 1f") \
                        __MS_CVT_BODY("movaps", "movaps", "movaps", op) \
                        __ASM_EMIT("jmp 1000f") \
                    __ASM_EMIT("1:") \
                        __MS_CVT_BODY("movaps", "movaps", "movups", op) \
                        __ASM_EMIT("jmp 1000f") \
                __ASM_EMIT("2:") \
                    __ASM_EMIT("test $0x0f, %[dst]") \
                    __ASM_EMIT("jnz 3f") \
                        __MS_CVT_BODY("movaps", "movups", "movaps", op) \
                        __ASM_EMIT("jmp 1000f") \
                    __ASM_EMIT("3:") \
                        __MS_CVT_BODY("movaps", "movups", "movups", op) \
                __ASM_EMIT("1000:") \
                \
                : [mid] "+r"(m), [side] "+r" (s), [dst] "+r" (dst_ptr), [count] "+r" (count) \
                : \
                : "cc", "memory", \
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3", \
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7" \
            );

        static void ms_to_left(float *l, const float *m, const float *s, size_t count)
        {
            MS_CVT_BODY(l, "add");
        }

        static void ms_to_right(float *r, const float *m, const float *s, size_t count)
        {
            MS_CVT_BODY(r, "sub");
        }

        #undef MS_CVT_BODY
        #undef __MS_CVT_BODY

    } // namespace sse
} // namespace lsp

#endif /* CORE_X86_DSP_MSMATRIX_H_ */
