/*
 * p_scramble.h
 *
 *  Created on: 15 февр. 2017 г.
 *      Author: sadko
 */

namespace sse
{
    // This is the SSE implementation of the scrambling functions for self data
    static inline void FFT_SCRAMBLE_SELF_DIRECT_NAME(float *dst, const float *src, size_t rank)
    {
        // Calculate number of items
        size_t items    = (1 << rank) - 1;

        for (size_t i = 1; i < items; ++i)
        {
            size_t j = reverse_bits(FFT_TYPE(i), rank);    /* Reverse the order of the bits */
            if (i >= j)
                continue;

            /* Copy the values from the reversed position */
            ARCH_X86_ASM
            (
                __ASM_EMIT("movlps (%[dst], %[i], 8), %%xmm0")
                __ASM_EMIT("movlps (%[dst], %[j], 8), %%xmm1")
                __ASM_EMIT("movlps %%xmm0, (%[dst], %[j], 8)")
                __ASM_EMIT("movlps %%xmm1, (%[dst], %[i], 8)")
                :
                : [dst] "r"(dst), [i] "r"(i), [j] "r"(j)
                : "memory",
                  "%xmm0", "%xmm1"
            );
        }

        // Perform butterfly 8x
        items = 1 << (rank - 3);

        // Perform 4-element butterflies
        ARCH_X86_ASM
        (
            /* Loop */
            __ASM_EMIT(".align 16")
            __ASM_EMIT("1:")

            /* Load data to registers */
            __ASM_EMIT("movups      0x00(%[dst]), %%xmm0")      /* xmm0 = r0 i0 r1 i1 */
            __ASM_EMIT("movups      0x10(%[dst]), %%xmm2")      /* xmm2 = r2 i2 r3 i3 */
            __ASM_EMIT("movups      0x20(%[dst]), %%xmm4")      /* xmm4 = r4 i4 r5 i5 */
            __ASM_EMIT("movups      0x30(%[dst]), %%xmm6")      /* xmm6 = r6 i6 r7 i7 */

            /* 1st-order 4x butterfly */
            __ASM_EMIT("movaps      %%xmm0, %%xmm1")            /* xmm1 = r0 i0 r1 i1 */
            __ASM_EMIT("movaps      %%xmm4, %%xmm5")            /* xmm5 = r4 i4 r5 i5 */
            __ASM_EMIT("shufps      $0x44, %%xmm2, %%xmm0")     /* xmm0 = r0 i0 r2 i2 */
            __ASM_EMIT("shufps      $0x44, %%xmm6, %%xmm4")     /* xmm4 = r4 i4 r6 i6 */
            __ASM_EMIT("shufps      $0xee, %%xmm2, %%xmm1")     /* xmm1 = r1 i1 r3 i3 */
            __ASM_EMIT("shufps      $0xee, %%xmm6, %%xmm5")     /* xmm5 = r5 i5 r7 i7 */

            __ASM_EMIT("movaps      %%xmm0, %%xmm2")            /* xmm2 = r0 i0 r2 i2 */
            __ASM_EMIT("movaps      %%xmm4, %%xmm6")            /* xmm6 = r4 i4 r6 i6 */
            __ASM_EMIT("addps       %%xmm1, %%xmm0")            /* xmm0 = r0+r1 i0+i1 r2+r3 i2+i3 = r0' i0' r2' i2' */
            __ASM_EMIT("addps       %%xmm5, %%xmm4")            /* xmm4 = r4+r5 i4+i5 r6+r7 i6+i7 = r4' i4' r6' i6' */
            __ASM_EMIT("subps       %%xmm1, %%xmm2")            /* xmm2 = r0-r1 i0-i1 r2-r3 i2-i3 = r1' i1' r3' i3' */
            __ASM_EMIT("subps       %%xmm5, %%xmm6")            /* xmm6 = r4-r5 i4-i5 r6-r7 i6-i7 = r5' i5' r7' i7' */

            /* 2nd-order 4x butterfly */
            __ASM_EMIT("movaps      %%xmm0, %%xmm1")            /* xmm1 = r0' i0' r2' i2' */
            __ASM_EMIT("movaps      %%xmm4, %%xmm5")            /* xmm5 = r4' i4' r6' i6' */
            __ASM_EMIT("shufps      $0x44, %%xmm2, %%xmm0")     /* xmm0 = r0' i0' r1' i1' */
            __ASM_EMIT("shufps      $0x44, %%xmm6, %%xmm4")     /* xmm4 = r4' i4' r5' i5' */
            __ASM_EMIT("shufps      $0xbe, %%xmm2, %%xmm1")     /* xmm1 = r2' i2' i3' r3' */
            __ASM_EMIT("shufps      $0xbe, %%xmm6, %%xmm5")     /* xmm5 = r6' i6' i7' r7' */

            __ASM_EMIT("movaps      %%xmm0, %%xmm2")            /* xmm2 = r0' i0' r1' i1' */
            __ASM_EMIT("movaps      %%xmm4, %%xmm6")            /* xmm6 = r4' i4' r5' i5' */
            __ASM_EMIT("addps       %%xmm1, %%xmm0")            /* xmm0 = r0'+r2' i0'+i2' r1'+i3' i1'+r3' = r0" i0" r1" i3" */
            __ASM_EMIT("addps       %%xmm5, %%xmm4")            /* xmm4 = r4'+r6' i4'+i6' r5'+i7' i5'+r7' = r4" i4" r5" i7" */
            __ASM_EMIT("subps       %%xmm1, %%xmm2")            /* xmm2 = r0'-r2' i0'-i2' r1'-i3' i1'-r3' = r2" i2" r3" i1" */
            __ASM_EMIT("subps       %%xmm5, %%xmm6")            /* xmm6 = r4'-r6' i4'-i6' r5'-i7' i5'-r7' = r6" i6" r7" i5" */

            /* Reorder and store */
            __ASM_EMIT("movaps      %%xmm0, %%xmm1")            /* xmm1 = r0" i0" r1" i3" */
            __ASM_EMIT("movaps      %%xmm4, %%xmm5")            /* xmm5 = r4" i4" r5" i7" */
            __ASM_EMIT("shufps      $0x88, %%xmm2, %%xmm0")     /* xmm0 = r0" r1" r2" r3" */
            __ASM_EMIT("shufps      $0x88, %%xmm6, %%xmm4")     /* xmm4 = r4" r5" r6" r7" */
            __ASM_EMIT("shufps      $0xdd, %%xmm2, %%xmm1")     /* xmm1 = i0" i3" i2" i1" */
            __ASM_EMIT("shufps      $0xdd, %%xmm6, %%xmm5")     /* xmm5 = i4" i7" i6" i5" */
            __ASM_EMIT("shufps      $0x6c, %%xmm1, %%xmm1")     /* xmm1 = i0" i1" i2" i3" */
            __ASM_EMIT("shufps      $0x6c, %%xmm5, %%xmm5")     /* xmm5 = i4" i5" i6" i7" */

            __ASM_EMIT("movups      %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("movups      %%xmm1, 0x10(%[dst])")
            __ASM_EMIT("movups      %%xmm4, 0x20(%[dst])")
            __ASM_EMIT("movups      %%xmm5, 0x30(%[dst])")

            /* Move pointers and repeat cycle */
            __ASM_EMIT("add         $0x40, %[dst]")
            __ASM_EMIT("dec         %[items]")
            __ASM_EMIT("jnz         1b")

            : [dst] "+r"(dst), [items] "+r"(items)
            :
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm4", "%xmm5", "%xmm6"
        );
    }

    static inline void FFT_SCRAMBLE_SELF_REVERSE_NAME(float *dst, const float *src, size_t rank)
    {
        // Calculate number of items
        size_t items    = (1 << rank) - 1;

        for (size_t i = 1; i < items; ++i)
        {
            size_t j = reverse_bits(FFT_TYPE(i), rank);    /* Reverse the order of the bits */
            if (i >= j)
                continue;

            /* Copy the values from the reversed position */
            ARCH_X86_ASM
            (
                __ASM_EMIT("movlps (%[dst], %[i], 8), %%xmm0")
                __ASM_EMIT("movlps (%[dst], %[j], 8), %%xmm1")
                __ASM_EMIT("movlps %%xmm0, (%[dst], %[j], 8)")
                __ASM_EMIT("movlps %%xmm1, (%[dst], %[i], 8)")
                :
                : [dst] "r"(dst), [i] "r"(i), [j] "r"(j)
                : "memory",
                  "%xmm0", "%xmm1"
            );
        }

        // Perform butterfly 8x
        items = 1 << (rank - 3);

        // Perform 4-element butterflies
        ARCH_X86_ASM
        (
            /* Loop */
            __ASM_EMIT(".align 16")
            __ASM_EMIT("1:")

            /* Load data to registers */
            __ASM_EMIT("movups      0x00(%[dst]), %%xmm0")      /* xmm0 = r0 i0 r1 i1 */
            __ASM_EMIT("movups      0x10(%[dst]), %%xmm2")      /* xmm2 = r2 i2 r3 i3 */
            __ASM_EMIT("movups      0x20(%[dst]), %%xmm4")      /* xmm4 = r4 i4 r5 i5 */
            __ASM_EMIT("movups      0x30(%[dst]), %%xmm6")      /* xmm6 = r6 i6 r7 i7 */

            /* 1st-order 4x butterfly */
            __ASM_EMIT("movaps      %%xmm0, %%xmm1")            /* xmm1 = r0 i0 r1 i1 */
            __ASM_EMIT("movaps      %%xmm4, %%xmm5")            /* xmm5 = r4 i4 r5 i5 */
            __ASM_EMIT("shufps      $0x44, %%xmm2, %%xmm0")     /* xmm0 = r0 i0 r2 i2 */
            __ASM_EMIT("shufps      $0x44, %%xmm6, %%xmm4")     /* xmm4 = r4 i4 r6 i6 */
            __ASM_EMIT("shufps      $0xee, %%xmm2, %%xmm1")     /* xmm1 = r1 i1 r3 i3 */
            __ASM_EMIT("shufps      $0xee, %%xmm6, %%xmm5")     /* xmm5 = r5 i5 r7 i7 */

            __ASM_EMIT("movaps      %%xmm0, %%xmm2")            /* xmm2 = r0 i0 r2 i2 */
            __ASM_EMIT("movaps      %%xmm4, %%xmm6")            /* xmm6 = r4 i4 r6 i6 */
            __ASM_EMIT("addps       %%xmm1, %%xmm0")            /* xmm0 = r0+r1 i0+i1 r2+r3 i2+i3 = r0' i0' r2' i2' */
            __ASM_EMIT("addps       %%xmm5, %%xmm4")            /* xmm4 = r4+r5 i4+i5 r6+r7 i6+i7 = r4' i4' r6' i6' */
            __ASM_EMIT("subps       %%xmm1, %%xmm2")            /* xmm2 = r0-r1 i0-i1 r2-r3 i2-i3 = r1' i1' r3' i3' */
            __ASM_EMIT("subps       %%xmm5, %%xmm6")            /* xmm6 = r4-r5 i4-i5 r6-r7 i6-i7 = r5' i5' r7' i7' */

            /* 2nd-order 4x butterfly */
            __ASM_EMIT("movaps      %%xmm0, %%xmm1")            /* xmm1 = r0' i0' r2' i2' */
            __ASM_EMIT("movaps      %%xmm4, %%xmm5")            /* xmm5 = r4' i4' r6' i6' */
            __ASM_EMIT("shufps      $0x44, %%xmm2, %%xmm0")     /* xmm0 = r0' i0' r1' i1' */
            __ASM_EMIT("shufps      $0xbe, %%xmm2, %%xmm1")     /* xmm1 = r2' i2' i3' r3' */
            __ASM_EMIT("shufps      $0x44, %%xmm6, %%xmm4")     /* xmm4 = r4' i4' r5' i5' */
            __ASM_EMIT("shufps      $0xbe, %%xmm6, %%xmm5")     /* xmm5 = r6' i6' i7' r7' */

            __ASM_EMIT("movaps      %%xmm0, %%xmm2")            /* xmm2 = r0' i0' r1' i1' */
            __ASM_EMIT("movaps      %%xmm4, %%xmm6")            /* xmm6 = r4' i4' r5' i5' */
            __ASM_EMIT("addps       %%xmm1, %%xmm0")            /* xmm0 = r0'+r2' i0'+i2' r1'+i3' i1'+r3' = r0" i0" r3" i1" */
            __ASM_EMIT("addps       %%xmm5, %%xmm4")            /* xmm4 = r4'+r6' i4'+i6' r5'+i7' i5'+r7' = r4" i4" r7" i5" */
            __ASM_EMIT("subps       %%xmm1, %%xmm2")            /* xmm2 = r0'-r2' i0'-i2' r1'-i3' i1'-r3' = r2" i2" r1" i3" */
            __ASM_EMIT("subps       %%xmm5, %%xmm6")            /* xmm6 = r4'-r6' i4'-i6' r5'-i7' i5'-r7' = r6" i6" r5" i7" */

            /* Reorder and store */
            __ASM_EMIT("movaps      %%xmm0, %%xmm1")            /* xmm1 = r0" i0" r3" i1" */
            __ASM_EMIT("movaps      %%xmm4, %%xmm5")            /* xmm5 = r4" i4" r7" i5" */
            __ASM_EMIT("shufps      $0x88, %%xmm2, %%xmm0")     /* xmm0 = r0" r3" r2" r1" */
            __ASM_EMIT("shufps      $0x88, %%xmm6, %%xmm4")     /* xmm4 = r4" r7" r6" r5" */
            __ASM_EMIT("shufps      $0xdd, %%xmm2, %%xmm1")     /* xmm1 = i0" i1" i2" i3" */
            __ASM_EMIT("shufps      $0xdd, %%xmm6, %%xmm5")     /* xmm5 = i4" i5" i6" i7" */
            __ASM_EMIT("shufps      $0x6c, %%xmm0, %%xmm0")     /* xmm0 = r0" r1" r2" r3" */
            __ASM_EMIT("shufps      $0x6c, %%xmm4, %%xmm4")     /* xmm4 = r4" r5" r6" r7" */

            __ASM_EMIT("movups      %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("movups      %%xmm1, 0x10(%[dst])")
            __ASM_EMIT("movups      %%xmm4, 0x20(%[dst])")
            __ASM_EMIT("movups      %%xmm5, 0x30(%[dst])")

            /* Move pointers and repeat cycle */
            __ASM_EMIT("add         $0x40, %[dst]")
            __ASM_EMIT("dec         %[items]")
            __ASM_EMIT("jnz         1b")

            : [dst] "+r"(dst), [items] "+r"(items)
            :
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm4", "%xmm5", "%xmm6"
        );
    }

    static inline void FFT_SCRAMBLE_COPY_DIRECT_NAME(float *dst, const float *src, size_t rank)
    {
        size_t regs     = 1 << rank;

        for (size_t i=0; i<regs; ++i)
        {
            size_t index    = reverse_bits(FFT_TYPE(i), rank);

            // Perform 4-element butterflies
            ARCH_X86_ASM
            (
                /* Load data to registers */
                __ASM_EMIT("movlps      (%[src], %[index], 8), %%xmm0") /* xmm0 = r0 i0 x x     */
                __ASM_EMIT("add         %[regs], %[index]")
                __ASM_EMIT("movlps      (%[src], %[index], 8), %%xmm4") /* xmm4 = r4 i4 x x     */
                __ASM_EMIT("add         %[regs], %[index]")

                __ASM_EMIT("movlps      (%[src], %[index], 8), %%xmm2") /* xmm2 = r2 i2 x x     */
                __ASM_EMIT("add         %[regs], %[index]")
                __ASM_EMIT("movlhps     %%xmm2, %%xmm0")                /* xmm0 = r0 i0 r2 i2   */

                __ASM_EMIT("movlps      (%[src], %[index], 8), %%xmm6") /* xmm6 = r6 i6 x x     */
                __ASM_EMIT("movaps      %%xmm0, %%xmm2")            /* xmm2 = r0 i0 r2 i2 */
                __ASM_EMIT("add         %[regs], %[index]")
                __ASM_EMIT("movlhps     %%xmm6, %%xmm4")                /* xmm4 = r4 i4 r6 i6   */

                __ASM_EMIT("movlps      (%[src], %[index], 8), %%xmm1") /* xmm0 = r1 i1 x x     */
                __ASM_EMIT("add         %[regs], %[index]")
                __ASM_EMIT("movaps      %%xmm4, %%xmm6")            /* xmm6 = r4 i4 r6 i6 */
                __ASM_EMIT("movlps      (%[src], %[index], 8), %%xmm5") /* xmm4 = r5 i5 x x     */
                __ASM_EMIT("add         %[regs], %[index]")

                __ASM_EMIT("movlps      (%[src], %[index], 8), %%xmm3") /* xmm1 = r3 i3 x x     */
                __ASM_EMIT("add         %[regs], %[index]")
                __ASM_EMIT("movlhps     %%xmm3, %%xmm1")                /* xmm1 = r1 i1 r3 i3   */

                __ASM_EMIT("movlps      (%[src], %[index], 8), %%xmm7") /* xmm5 = r7 i7 x x     */
                __ASM_EMIT("addps       %%xmm1, %%xmm0")            /* xmm0 = r0+r1 i0+i1 r2+r3 i2+i3 = r0' i0' r2' i2' */
                __ASM_EMIT("add         %[regs], %[index]")
                __ASM_EMIT("movlhps     %%xmm7, %%xmm5")                /* xmm5 = r5 i5 r7 i7   */

                /* 1st-order 4x butterfly */
                /* xmm0 = r0 i0 r2 i2 */
                /* xmm4 = r4 i4 r6 i6 */
                /* xmm1 = r1 i1 r3 i3 */
                /* xmm5 = r5 i5 r7 i7 */
                __ASM_EMIT("subps       %%xmm1, %%xmm2")            /* xmm2 = r0-r1 i0-i1 r2-r3 i2-i3 = r1' i1' r3' i3' */
                __ASM_EMIT("addps       %%xmm5, %%xmm4")            /* xmm4 = r4+r5 i4+i5 r6+r7 i6+i7 = r4' i4' r6' i6' */
                __ASM_EMIT("subps       %%xmm5, %%xmm6")            /* xmm6 = r4-r5 i4-i5 r6-r7 i6-i7 = r5' i5' r7' i7' */

                /* 2nd-order 4x butterfly */
                __ASM_EMIT("movaps      %%xmm0, %%xmm1")            /* xmm1 = r0' i0' r2' i2' */
                __ASM_EMIT("movaps      %%xmm4, %%xmm5")            /* xmm5 = r4' i4' r6' i6' */
                __ASM_EMIT("shufps      $0x44, %%xmm2, %%xmm0")     /* xmm0 = r0' i0' r1' i1' */
                __ASM_EMIT("shufps      $0xbe, %%xmm2, %%xmm1")     /* xmm1 = r2' i2' i3' r3' */
                __ASM_EMIT("shufps      $0x44, %%xmm6, %%xmm4")     /* xmm4 = r4' i4' r5' i5' */
                __ASM_EMIT("shufps      $0xbe, %%xmm6, %%xmm5")     /* xmm5 = r6' i6' i7' r7' */

                __ASM_EMIT("movaps      %%xmm0, %%xmm2")            /* xmm2 = r0' i0' r1' i1' */
                __ASM_EMIT("movaps      %%xmm4, %%xmm6")            /* xmm6 = r4' i4' r5' i5' */
                __ASM_EMIT("addps       %%xmm1, %%xmm0")            /* xmm0 = r0'+r2' i0'+i2' r1'+i3' i1'+r3' = r0" i0" r1" i3" */
                __ASM_EMIT("addps       %%xmm5, %%xmm4")            /* xmm4 = r4'+r6' i4'+i6' r5'+i7' i5'+r7' = r4" i4" r5" i7" */
                __ASM_EMIT("subps       %%xmm1, %%xmm2")            /* xmm2 = r0'-r2' i0'-i2' r1'-i3' i1'-r3' = r2" i2" r3" i1" */
                __ASM_EMIT("subps       %%xmm5, %%xmm6")            /* xmm6 = r4'-r6' i4'-i6' r5'-i7' i5'-r7' = r6" i6" r7" i5" */

                /* Reorder and store */
                __ASM_EMIT("movaps      %%xmm0, %%xmm1")            /* xmm1 = r0" i0" r1" i3" */
                __ASM_EMIT("movaps      %%xmm4, %%xmm5")            /* xmm5 = r4" i4" r5" i7" */
                __ASM_EMIT("shufps      $0x88, %%xmm2, %%xmm0")     /* xmm0 = r0" r1" r2" r3" */
                __ASM_EMIT("shufps      $0x88, %%xmm6, %%xmm4")     /* xmm4 = r4" r5" r6" r7" */
                __ASM_EMIT("shufps      $0xdd, %%xmm2, %%xmm1")     /* xmm1 = i0" i3" i2" i1" */
                __ASM_EMIT("shufps      $0xdd, %%xmm6, %%xmm5")     /* xmm5 = i4" i7" i6" i5" */
                __ASM_EMIT("shufps      $0x6c, %%xmm1, %%xmm1")     /* xmm1 = i0" i1" i2" i3" */
                __ASM_EMIT("shufps      $0x6c, %%xmm5, %%xmm5")     /* xmm5 = i4" i5" i6" i7" */

                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("movups      %%xmm1, 0x10(%[dst])")
                __ASM_EMIT("movups      %%xmm4, 0x20(%[dst])")
                __ASM_EMIT("movups      %%xmm5, 0x30(%[dst])")

                /* Move pointers and repeat cycle */
                __ASM_EMIT("add         $0x40, %[dst]")

                : [dst] "+r"(dst), [src] "+r"(src), [index] "+r"(index)
                : [regs] "r" (regs)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }
    }

    static inline void FFT_SCRAMBLE_COPY_REVERSE_NAME(float *dst, const float *src, size_t rank)
    {
        size_t regs     = 1 << rank;

        for (size_t i=0; i<regs; ++i)
        {
            size_t index    = reverse_bits(FFT_TYPE(i), rank);

            // Perform 4-element butterflies
            ARCH_X86_ASM
            (
                /* Load data to registers */
                __ASM_EMIT("movlps      (%[src], %[index], 8), %%xmm0") /* xmm0 = r0 i0 x x     */
                __ASM_EMIT("add         %[regs], %[index]")

                __ASM_EMIT("movlps      (%[src], %[index], 8), %%xmm4") /* xmm4 = r4 i4 x x     */
                __ASM_EMIT("add         %[regs], %[index]")

                __ASM_EMIT("movlps      (%[src], %[index], 8), %%xmm2") /* xmm2 = r2 i2 x x     */
                __ASM_EMIT("add         %[regs], %[index]")
                __ASM_EMIT("movlhps     %%xmm2, %%xmm0")                /* xmm0 = r0 i0 r2 i2   */

                __ASM_EMIT("movlps      (%[src], %[index], 8), %%xmm6") /* xmm6 = r6 i6 x x     */
                __ASM_EMIT("movaps      %%xmm0, %%xmm2")                /* xmm2 = r0 i0 r2 i2 */
                __ASM_EMIT("add         %[regs], %[index]")
                __ASM_EMIT("movlhps     %%xmm6, %%xmm4")                /* xmm4 = r4 i4 r6 i6   */

                __ASM_EMIT("movlps      (%[src], %[index], 8), %%xmm1") /* xmm1 = r1 i1 x x     */
                __ASM_EMIT("add         %[regs], %[index]")
                __ASM_EMIT("movaps      %%xmm4, %%xmm6")                /* xmm6 = r4 i4 r6 i6 */
                __ASM_EMIT("movlps      (%[src], %[index], 8), %%xmm5") /* xmm5 = r5 i5 x x     */
                __ASM_EMIT("add         %[regs], %[index]")

                __ASM_EMIT("movlps      (%[src], %[index], 8), %%xmm3") /* xmm3 = r3 i3 x x     */
                __ASM_EMIT("add         %[regs], %[index]")
                __ASM_EMIT("movlhps     %%xmm3, %%xmm1")                /* xmm1 = r1 i1 r3 i3   */

                __ASM_EMIT("movlps      (%[src], %[index], 8), %%xmm7") /* xmm7 = r7 i7 x x     */
                __ASM_EMIT("addps       %%xmm1, %%xmm0")                /* xmm0 = r0+r1 i0+i1 r2+r3 i2+i3 = r0' i0' r2' i2' */
                __ASM_EMIT("add         %[regs], %[index]")
                __ASM_EMIT("movlhps     %%xmm7, %%xmm5")                /* xmm5 = r5 i5 r7 i7   */

                /* 1st-order 4x butterfly */
                /* xmm0 = r0 i0 r2 i2 */
                /* xmm4 = r4 i4 r6 i6 */
                /* xmm1 = r1 i1 r3 i3 */
                /* xmm5 = r5 i5 r7 i7 */
                __ASM_EMIT("subps       %%xmm1, %%xmm2")            /* xmm2 = r0-r1 i0-i1 r2-r3 i2-i3 = r1' i1' r3' i3' */
                __ASM_EMIT("addps       %%xmm5, %%xmm4")            /* xmm4 = r4+r5 i4+i5 r6+r7 i6+i7 = r4' i4' r6' i6' */
                __ASM_EMIT("subps       %%xmm5, %%xmm6")            /* xmm6 = r4-r5 i4-i5 r6-r7 i6-i7 = r5' i5' r7' i7' */

                /* 2nd-order 4x butterfly */
                __ASM_EMIT("movaps      %%xmm0, %%xmm1")            /* xmm1 = r0' i0' r2' i2' */
                __ASM_EMIT("movaps      %%xmm4, %%xmm5")            /* xmm5 = r4' i4' r6' i6' */
                __ASM_EMIT("shufps      $0x44, %%xmm2, %%xmm0")     /* xmm0 = r0' i0' r1' i1' */
                __ASM_EMIT("shufps      $0xbe, %%xmm2, %%xmm1")     /* xmm1 = r2' i2' i3' r3' */
                __ASM_EMIT("shufps      $0x44, %%xmm6, %%xmm4")     /* xmm4 = r4' i4' r5' i5' */
                __ASM_EMIT("shufps      $0xbe, %%xmm6, %%xmm5")     /* xmm5 = r6' i6' i7' r7' */

                __ASM_EMIT("movaps      %%xmm0, %%xmm2")            /* xmm2 = r0' i0' r1' i1' */
                __ASM_EMIT("movaps      %%xmm4, %%xmm6")            /* xmm6 = r4' i4' r5' i5' */
                __ASM_EMIT("addps       %%xmm1, %%xmm0")            /* xmm0 = r0'+r2' i0'+i2' r1'+i3' i1'+r3' = r0" i0" r3" i1" */
                __ASM_EMIT("addps       %%xmm5, %%xmm4")            /* xmm4 = r4'+r6' i4'+i6' r5'+i7' i5'+r7' = r4" i4" r7" i5" */
                __ASM_EMIT("subps       %%xmm1, %%xmm2")            /* xmm2 = r0'-r2' i0'-i2' r1'-i3' i1'-r3' = r2" i2" r1" i3" */
                __ASM_EMIT("subps       %%xmm5, %%xmm6")            /* xmm6 = r4'-r6' i4'-i6' r5'-i7' i5'-r7' = r6" i6" r5" i7" */

                /* Reorder and store */
                __ASM_EMIT("movaps      %%xmm0, %%xmm1")            /* xmm1 = r0" i0" r3" i1" */
                __ASM_EMIT("movaps      %%xmm4, %%xmm5")            /* xmm5 = r4" i4" r7" i5" */
                __ASM_EMIT("shufps      $0x88, %%xmm2, %%xmm0")     /* xmm0 = r0" r3" r2" r1" */
                __ASM_EMIT("shufps      $0x88, %%xmm6, %%xmm4")     /* xmm4 = r4" r7" r6" r5" */
                __ASM_EMIT("shufps      $0xdd, %%xmm2, %%xmm1")     /* xmm1 = i0" i1" i2" i3" */
                __ASM_EMIT("shufps      $0xdd, %%xmm6, %%xmm5")     /* xmm5 = i4" i5" i6" i7" */
                __ASM_EMIT("shufps      $0x6c, %%xmm0, %%xmm0")     /* xmm0 = r0" r1" r2" r3" */
                __ASM_EMIT("shufps      $0x6c, %%xmm4, %%xmm4")     /* xmm4 = r4" r5" r6" r7" */

                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("movups      %%xmm1, 0x10(%[dst])")
                __ASM_EMIT("movups      %%xmm4, 0x20(%[dst])")
                __ASM_EMIT("movups      %%xmm5, 0x30(%[dst])")

                /* Move pointers and repeat cycle */
                __ASM_EMIT("add         $0x40, %[dst]")

                : [dst] "+r"(dst), [src] "+r"(src), [index] "+r"(index)
                : [regs] "r" (regs)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }
    }
}

#undef FFT_SCRAMBLE_SELF_DIRECT_NAME
#undef FFT_SCRAMBLE_COPY_DIRECT_NAME
#undef FFT_SCRAMBLE_SELF_REVERSE_NAME
#undef FFT_SCRAMBLE_COPY_REVERSE_NAME
#undef FFT_REPACK
#undef FFT_TYPE

