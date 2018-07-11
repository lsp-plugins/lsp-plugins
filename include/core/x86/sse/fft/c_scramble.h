/*
 * c_scramble.h
 *
 *  Created on: 24 февр. 2017 г.
 *      Author: sadko
 */

// This is the SSE implementation of the scrambling functions for self data

namespace lsp
{
    namespace sse
    {
        static inline void FFT_SCRAMBLE_COPY_DIRECT_NAME(float *dst, const float *src, size_t rank)
        {
            size_t regs     = 1 << rank;

            for (size_t i=0; i<regs; ++i)
            {
                size_t index    = reverse_bits(FFT_TYPE(i), rank);

                // Perform 4-element butterflies
                __asm__ __volatile__
                (
                    /* Load data to registers */
#if 1
                    __ASM_EMIT("movss       (%[src], %[index], 4), %%xmm0") /* xmm0 = r0 0 0 0      */
                    __ASM_EMIT("add         %[regs], %[index]")
                    __ASM_EMIT("shufps      $0xf0, %%xmm0, %%xmm0")         /* xmm0 = r0 r0 0 0 */

                    __ASM_EMIT("movss       (%[src], %[index], 4), %%xmm4") /* xmm4 = r4 0 0 0      */
                    __ASM_EMIT("movaps      %%xmm0, %%xmm1")                /* xmm1 = r0 r0 0 0 */
                    __ASM_EMIT("add         %[regs], %[index]")
                    __ASM_EMIT("shufps      $0xf0, %%xmm4, %%xmm4")         /* xmm4 = r4 r4 0 0 */

                    __ASM_EMIT("movss       (%[src], %[index], 4), %%xmm2") /* xmm2 = r2 0 0 0      */
                    __ASM_EMIT("movaps      %%xmm4, %%xmm5")                /* xmm5 = r4 r4 0 0 */
                    __ASM_EMIT("add         %[regs], %[index]")
                    __ASM_EMIT("movlhps     %%xmm2, %%xmm2")                /* xmm2 = r2 0 r2 0 */

                    __ASM_EMIT("movss       (%[src], %[index], 4), %%xmm6") /* xmm6 = r6 0 0 0      */
                    __ASM_EMIT("addps       %%xmm2, %%xmm0")                /* xmm0 = r0+r2 r0 r2 0 = r0" r1" i3" i2" */
                    __ASM_EMIT("add         %[regs], %[index]")
                    __ASM_EMIT("movlhps     %%xmm6, %%xmm6")                /* xmm6 = r6 0 r6 0 */

                    __ASM_EMIT("subps       %%xmm2, %%xmm1")                /* xmm1 = r0-r2 r0 -r2 0 = r2" r3" i1" i0" */
                    __ASM_EMIT("addps       %%xmm6, %%xmm4")                /* xmm4 = r4+r6 r4 r6 0 = r4" r5" i7" i6" */
                    __ASM_EMIT("movaps      %%xmm0, %%xmm2")                /* xmm2 = r0" r1" i3" i2" */
                    __ASM_EMIT("subps       %%xmm6, %%xmm5")                /* xmm5 = r4-r6 r4 -r6 0 = r6" r7" i5" i4" */
                    __ASM_EMIT("movlhps     %%xmm1, %%xmm0")                /* xmm0 = r0" r1" r2" r3" */
                    __ASM_EMIT("movaps      %%xmm4, %%xmm6")                /* xmm6 = r4" r5" i7" i6" */
                    __ASM_EMIT("movlhps     %%xmm5, %%xmm4")                /* xmm4 = r4" r5" r6" r7" */
                    __ASM_EMIT("shufps      $0xbb, %%xmm2, %%xmm1")         /* xmm1 = i0" i1" i2" i3" */
                    __ASM_EMIT("shufps      $0xbb, %%xmm6, %%xmm5")         /* xmm5 = i4" i5" i6" i7" */
#else
                    /* Load data to registers */
                    __ASM_EMIT("movss       (%[src], %[index], 4), %%xmm0") /* xmm0 = r0 0 0 0      */
                    __ASM_EMIT("add         %[regs], %[index]")
                    __ASM_EMIT("movss       (%[src], %[index], 4), %%xmm4") /* xmm4 = r4 0 0 0      */
                    __ASM_EMIT("add         %[regs], %[index]")
                    __ASM_EMIT("movss       (%[src], %[index], 4), %%xmm2") /* xmm2 = r2 0 0 0      */
                    __ASM_EMIT("add         %[regs], %[index]")
                    __ASM_EMIT("movss       (%[src], %[index], 4), %%xmm6") /* xmm6 = r6 0 0 0      */
                    __ASM_EMIT("add         %[regs], %[index]")

                    /* Do butterfly */
                    __ASM_EMIT("shufps      $0xf0, %%xmm0, %%xmm0")     /* xmm0 = r0 r0 0 0 */
                    __ASM_EMIT("shufps      $0xf0, %%xmm4, %%xmm4")     /* xmm4 = r4 r4 0 0 */
                    __ASM_EMIT("movlhps     %%xmm2, %%xmm2")            /* xmm2 = r2 0 r2 0 */
                    __ASM_EMIT("movlhps     %%xmm6, %%xmm6")            /* xmm6 = r6 0 r6 0 */

                    __ASM_EMIT("movaps      %%xmm0, %%xmm1")            /* xmm1 = r0 r0 0 0 */
                    __ASM_EMIT("movaps      %%xmm4, %%xmm5")            /* xmm5 = r4 r4 0 0 */
                    __ASM_EMIT("addps       %%xmm2, %%xmm0")            /* xmm0 = r0+r2 r0 r2 0 = r0" r1" i3" i2" */
                    __ASM_EMIT("addps       %%xmm6, %%xmm4")            /* xmm4 = r4+r6 r4 r6 0 = r4" r5" i7" i6" */
                    __ASM_EMIT("subps       %%xmm2, %%xmm1")            /* xmm1 = r0-r2 r0 -r2 0 = r2" r3" i1" i0" */
                    __ASM_EMIT("subps       %%xmm6, %%xmm5")            /* xmm5 = r4-r6 r4 -r6 0 = r6" r7" i5" i4" */

                    /* Re-shuffle */
                    __ASM_EMIT("movaps      %%xmm0, %%xmm2")            /* xmm2 = r0" r1" i3" i2" */
                    __ASM_EMIT("movaps      %%xmm4, %%xmm6")            /* xmm6 = r4" r5" i7" i6" */
                    __ASM_EMIT("movlhps     %%xmm1, %%xmm0")            /* xmm0 = r0" r1" r2" r3" */
                    __ASM_EMIT("shufps      $0xbb, %%xmm2, %%xmm1")     /* xmm1 = i0" i1" i2" i3" */
                    __ASM_EMIT("movlhps     %%xmm5, %%xmm4")            /* xmm4 = r4" r5" r6" r7" */
                    __ASM_EMIT("shufps      $0xbb, %%xmm6, %%xmm5")     /* xmm5 = i4" i5" i6" i7" */
#endif

                    /* Store data */
                    __ASM_EMIT(LS_RE "      %%xmm0, 0x00(%[dst])")
                    __ASM_EMIT(LS_RE "      %%xmm1, 0x10(%[dst])")
                    __ASM_EMIT(LS_RE "      %%xmm4, 0x20(%[dst])")
                    __ASM_EMIT(LS_RE "      %%xmm5, 0x30(%[dst])")

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

    } // namespace sse

} // namespace lsp

#undef FFT_SCRAMBLE_COPY_DIRECT_NAME
#undef FFT_REPACK
#undef FFT_TYPE
#undef LS_RE

