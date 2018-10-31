/*
 * scramble.h
 *
 *  Created on: 29 февр. 2016 г.
 *      Author: sadko
 */

// This is the SSE implementation of the scrambling functions for self data

#if 0
static inline void FFT_SCRAMBLE_SELF_DIRECT_NAME(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank)
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
            __ASM_EMIT("movss (%[dst_re], %[i], 4), %%xmm0")
            __ASM_EMIT("movss (%[dst_im], %[i], 4), %%xmm1")
            __ASM_EMIT("movss (%[dst_re], %[j], 4), %%xmm2")
            __ASM_EMIT("movss (%[dst_im], %[j], 4), %%xmm3")
            __ASM_EMIT("movss %%xmm2, (%[dst_re], %[i], 4)")
            __ASM_EMIT("movss %%xmm3, (%[dst_im], %[i], 4)")
            __ASM_EMIT("movss %%xmm0, (%[dst_re], %[j], 4)")
            __ASM_EMIT("movss %%xmm1, (%[dst_im], %[j], 4)")
            :
            : [dst_re] "r"(dst_re), [dst_im] "r"(dst_im), [i] "r"(i), [j] "r"(j)
            : "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3"
        );
    }

    // Perform butterfly 4x
    items = 1 << (rank - 2);

    // Perform 4-element butterflies
    ARCH_X86_ASM
    (
        /* Prefetch data */
        /*__ASM_EMIT("prefetchnta (%[dst_re])")
        __ASM_EMIT("prefetchnta (%[dst_im])")*/
        /* Loop */
        __ASM_EMIT(".align 16")
        __ASM_EMIT("1:")
        /* Prefetch data */
        /*__ASM_EMIT("prefetchnta 0x10(%[dst_re])")
        __ASM_EMIT("prefetchnta 0x10(%[dst_im])")*/

        /* Load data to registers */
        __ASM_EMIT(LS_RE " (%[dst_re]), %%xmm0")    /* xmm0 = r0 r1 r2 r3 */
        __ASM_EMIT(LS_IM " (%[dst_im]), %%xmm1")    /* xmm1 = i0 i1 i2 i3 */

        /* Shuffle 1 */
        __ASM_EMIT("movaps  %%xmm0, %%xmm2")        /* xmm2 = r0 r1 r2 r3 */
        __ASM_EMIT("shufps $0x88, %%xmm1, %%xmm2")  /* xmm2 = r0 r2 i0 i2 */
        __ASM_EMIT("shufps $0xdd, %%xmm1, %%xmm0")  /* xmm0 = r1 r3 i1 i3 */
        __ASM_EMIT("movaps  %%xmm2, %%xmm1")        /* xmm1 = r0 r2 i0 i2 */

        /* Transform 1 */
        __ASM_EMIT("addps   %%xmm0, %%xmm1")        /* xmm1 = sr0 sr2 si0 si2 = r0+r1 r2+r3 i0+i1 i2+i3 */
        __ASM_EMIT("subps   %%xmm0, %%xmm2")        /* xmm2 = sr1 sr3 si1 si3 = r0-r1 r2-r3 i0-i1 i2-i3 */

        /* Shuffle 2 */
        __ASM_EMIT("movaps  %%xmm1, %%xmm0")        /* xmm0 = sr0 sr2 si0 si2 */
        __ASM_EMIT("shufps $0x88, %%xmm2, %%xmm0")  /* xmm0 = sr0 si0 sr1 si1 */
        __ASM_EMIT("shufps $0x7d, %%xmm2, %%xmm1")  /* xmm1 = sr2 si2 si3 sr3 */
        __ASM_EMIT("movaps %%xmm0, %%xmm2")         /* xmm2 = sr0 si0 sr1 si1 */

        /* Transform 2 */
        __ASM_EMIT("addps   %%xmm1, %%xmm0")        /* xmm0 = dr0 di0 dr1 di3 = sr0+sr2 si0+si2 sr1+si1 si1+sr3 */
        __ASM_EMIT("subps   %%xmm1, %%xmm2")        /* xmm2 = dr2 di2 dr3 di1 = sr0-sr2 si0-si2 sr1-si3 si1-sr3 */

        /* Collect final values */
        __ASM_EMIT("movaps  %%xmm0, %%xmm1")        /* xmm1 = dr0 di0 dr1 di3 */
        __ASM_EMIT("shufps $0x88, %%xmm2, %%xmm0")  /* xmm0 = dr0 dr1 dr2 dr3 */
        __ASM_EMIT("shufps $0xdd, %%xmm2, %%xmm1")  /* xmm1 = di0 di3 di2 di1 */
        __ASM_EMIT("shufps $0x6c, %%xmm1, %%xmm1")  /* xmm1 = di0 di1 di2 di3 */

        /* Store values */
        __ASM_EMIT(LS_RE " %%xmm0, (%[dst_re])")
        __ASM_EMIT(LS_IM " %%xmm1, (%[dst_im])")

        /* Move pointers */
        __ASM_EMIT("add     $0x10, %[dst_re]")
        __ASM_EMIT("add     $0x10, %[dst_im]")

        /* Repeat cycle */
        __ASM_EMIT("dec     %[items]")
        __ASM_EMIT("jnz     1b")
        __ASM_EMIT("2:")
        : [dst_re] "+r"(dst_re), [dst_im] "+r"(dst_im), [items] "+r"(items)
        :
        : "cc", "memory",
          "%xmm0", "%xmm1", "%xmm2"
    );
}

static inline void FFT_SCRAMBLE_COPY_DIRECT_NAME(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank)
{
    size_t regs     = 1 << rank;

    for (size_t i=0; i<regs; ++i)
    {
        size_t index    = reverse_bits(FFT_TYPE(i), rank);

        ARCH_X86_ASM
        (
            /* Load scalar values */
            __ASM_EMIT("movss   (%[src_re], %[index], 4), %%xmm1") /* xmm1 = r0 x x x */
            __ASM_EMIT("movss   (%[src_im], %[index], 4), %%xmm2") /* xmm2 = i0 x x x */
            __ASM_EMIT("add     %[regs], %[index]")

            __ASM_EMIT("movss   (%[src_re], %[index], 4), %%xmm5") /* xmm5 = r2 x x x */
            __ASM_EMIT("movss   (%[src_im], %[index], 4), %%xmm6") /* xmm4 = i2 x x x */
            __ASM_EMIT("add     %[regs], %[index]")

            __ASM_EMIT("movss   (%[src_re], %[index], 4), %%xmm0") /* xmm3 = r1 x x x */
            __ASM_EMIT("movss   (%[src_im], %[index], 4), %%xmm3") /* xmm2 = i1 x x x */
            __ASM_EMIT("add     %[regs], %[index]")

            __ASM_EMIT("movss   (%[src_re], %[index], 4), %%xmm4") /* xmm7 = r3 x x x */
            __ASM_EMIT("movss   (%[src_im], %[index], 4), %%xmm7") /* xmm6 = i3 x x x */

            /* Perform 4-element butterfly */
            /* Shuffle 1 */

            __ASM_EMIT("movlhps %%xmm5, %%xmm1")        /* xmm1 = r0 x r2 x */
            __ASM_EMIT("movlhps %%xmm4, %%xmm0")        /* xmm0 = r1 x r3 x */
            __ASM_EMIT("movlhps %%xmm6, %%xmm2")        /* xmm2 = i0 x i2 x */
            __ASM_EMIT("movlhps %%xmm7, %%xmm3")        /* xmm3 = i1 x i3 x */

            __ASM_EMIT("shufps $0x88, %%xmm2, %%xmm1")  /* xmm1 = r0 r2 i0 i2 */
            __ASM_EMIT("shufps $0x88, %%xmm3, %%xmm0")  /* xmm0 = r1 r3 i1 i3 */
            __ASM_EMIT("movaps %%xmm1, %%xmm2")         /* xmm2 = r0 r2 i0 i2 */

            /* Transform 1 */
            __ASM_EMIT("addps   %%xmm0, %%xmm1")        /* xmm1 = sr0 sr2 si0 si2 = r0+r1 r2+r3 i0+i1 i2+i3 */
            __ASM_EMIT("subps   %%xmm0, %%xmm2")        /* xmm2 = sr1 sr3 si1 si3 = r0-r1 r2-r3 i0-i1 i2-i3 */

            /* Shuffle 2 */
            __ASM_EMIT("movaps  %%xmm1, %%xmm0")        /* xmm0 = sr0 sr2 si0 si2 */
            __ASM_EMIT("shufps $0x88, %%xmm2, %%xmm0")  /* xmm0 = sr0 si0 sr1 si1 */
            __ASM_EMIT("shufps $0x7d, %%xmm2, %%xmm1")  /* xmm1 = sr2 si2 si3 sr3 */
            __ASM_EMIT("movaps %%xmm0, %%xmm2")         /* xmm2 = sr0 si0 sr1 si1 */

            /* Transform 2 */
            __ASM_EMIT("addps   %%xmm1, %%xmm0")        /* xmm0 = dr0 di0 dr1 di3 = sr0+sr2 si0+si2 sr1+si3 si1+sr3 */
            __ASM_EMIT("subps   %%xmm1, %%xmm2")        /* xmm2 = dr2 di2 dr3 di1 = sr0-sr2 si0-si2 sr1-si3 si1-sr3 */

            /* Collect final values */
            __ASM_EMIT("movaps  %%xmm0, %%xmm1")        /* xmm1 = dr0 di0 dr1 di3 */
            __ASM_EMIT("shufps $0x88, %%xmm2, %%xmm0")  /* xmm0 = dr0 dr1 dr2 dr3 */
            __ASM_EMIT("shufps $0xdd, %%xmm2, %%xmm1")  /* xmm1 = di0 di3 di2 di1 */
            __ASM_EMIT("shufps $0x6c, %%xmm1, %%xmm1")  /* xmm1 = di0 di1 di2 di3 */

            /* Store values */
            __ASM_EMIT(LS_RE " %%xmm0, (%[dst_re])")
            __ASM_EMIT(LS_IM " %%xmm1, (%[dst_im])")

            /* Update pointers */
            __ASM_EMIT("add     $0x10, %[dst_re]")
            __ASM_EMIT("add     $0x10, %[dst_im]")

            : [dst_re] "+r" (dst_re), [dst_im] "+r"(dst_im), [index] "+r"(index)
            : [src_re] "r"(src_re), [src_im] "r"(src_im), [regs] __ASM_ARG_RO(regs)
            : "cc", "memory",
                "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }
}

static inline void FFT_SCRAMBLE_SELF_REVERSE_NAME(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank)
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
            __ASM_EMIT("movss (%[dst_re], %[i], 4), %%xmm0")
            __ASM_EMIT("movss (%[dst_im], %[i], 4), %%xmm1")
            __ASM_EMIT("movss (%[dst_re], %[j], 4), %%xmm2")
            __ASM_EMIT("movss (%[dst_im], %[j], 4), %%xmm3")
            __ASM_EMIT("movss %%xmm2, (%[dst_re], %[i], 4)")
            __ASM_EMIT("movss %%xmm3, (%[dst_im], %[i], 4)")
            __ASM_EMIT("movss %%xmm0, (%[dst_re], %[j], 4)")
            __ASM_EMIT("movss %%xmm1, (%[dst_im], %[j], 4)")
            :
            : [dst_re] "r"(dst_re), [dst_im] "r"(dst_im), [i] "r"(i), [j] "r"(j)
            : "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3"
        );
    }

    // Perform butterfly 4x
    items = 1 << (rank - 2);

    // Perform 4-element butterflies
    ARCH_X86_ASM
    (
        /* Prefetch data */
        /*__ASM_EMIT("prefetchnta (%[dst_re])")
        __ASM_EMIT("prefetchnta (%[dst_im])")*/
        /* Loop */
        __ASM_EMIT(".align 16")
        __ASM_EMIT("1:")
        /* Prefetch data */
        /*__ASM_EMIT("prefetchnta 0x10(%[dst_re])")
        __ASM_EMIT("prefetchnta 0x10(%[dst_im])")*/

        /* Load data to registers */
        __ASM_EMIT(LS_RE " (%[dst_re]), %%xmm0")          /* xmm0 = r0 r1 r2 r3 */
        __ASM_EMIT(LS_IM " (%[dst_im]), %%xmm1")          /* xmm1 = i0 i1 i2 i3 */

        /* Shuffle 1 */
        __ASM_EMIT("movaps  %%xmm0, %%xmm2")        /* xmm2 = r0 r1 r2 r3 */
        __ASM_EMIT("shufps $0x88, %%xmm1, %%xmm2")  /* xmm2 = r0 r2 i0 i2 */
        __ASM_EMIT("shufps $0xdd, %%xmm1, %%xmm0")  /* xmm0 = r1 r3 i1 i3 */
        __ASM_EMIT("movaps  %%xmm2, %%xmm1")        /* xmm1 = r0 r2 i0 i2 */

        /* Transform 1 */
        __ASM_EMIT("addps   %%xmm0, %%xmm1")        /* xmm1 = sr0 sr2 si0 si2 = r0+r1 r2+r3 i0+i1 i2+i3 */
        __ASM_EMIT("subps   %%xmm0, %%xmm2")        /* xmm2 = sr1 sr3 si1 si3 = r0-r1 r2-r3 i0-i1 i2-i3 */

        /* Shuffle 2 */
        __ASM_EMIT("movaps  %%xmm1, %%xmm0")        /* xmm0 = sr0 sr2 si0 si2 */
        __ASM_EMIT("shufps $0x88, %%xmm2, %%xmm0")  /* xmm0 = sr0 si0 sr1 si1 */
        __ASM_EMIT("shufps $0x7d, %%xmm2, %%xmm1")  /* xmm1 = sr2 si2 si3 sr3 */
        __ASM_EMIT("movaps %%xmm0, %%xmm2")         /* xmm2 = sr0 si0 sr1 si1 */

        /* Transform 2 */
        __ASM_EMIT("addps   %%xmm1, %%xmm0")        /* xmm0 = dr0 di0 dr3 di1 = sr0+sr2 si0+si2 sr1+si3 si1+sr3 */
        __ASM_EMIT("subps   %%xmm1, %%xmm2")        /* xmm2 = dr2 di2 dr1 di3 = sr0-sr2 si0-si2 sr1-si3 si1-sr3 */

        /* Collect final values */
        __ASM_EMIT("movaps  %%xmm0, %%xmm1")        /* xmm1 = dr0 di0 dr3 di1 */
        __ASM_EMIT("shufps $0x88, %%xmm2, %%xmm0")  /* xmm0 = dr0 dr3 dr2 dr1 */
        __ASM_EMIT("shufps $0xdd, %%xmm2, %%xmm1")  /* xmm1 = di0 di1 di2 di3 */
        __ASM_EMIT("shufps $0x6c, %%xmm0, %%xmm0")  /* xmm0 = dr0 dr1 dr2 dr3 */

        /* Store values */
        __ASM_EMIT(LS_RE " %%xmm0, (%[dst_re])")
        __ASM_EMIT(LS_IM " %%xmm1, (%[dst_im])")

        /* Move pointers */
        __ASM_EMIT("add     $0x10, %[dst_re]")
        __ASM_EMIT("add     $0x10, %[dst_im]")

        /* Repeat cycle */
        __ASM_EMIT("dec     %[items]")
        __ASM_EMIT("jnz     1b")
        __ASM_EMIT("2:")
        : [dst_re] "+r"(dst_re), [dst_im] "+r"(dst_im), [items] "+r"(items)
        :
        : "cc", "memory",
          "%xmm0", "%xmm1", "%xmm2"
    );
}

static inline void FFT_SCRAMBLE_COPY_REVERSE_NAME(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank)
{
    size_t regs     = 1 << rank;

    for (size_t i=0; i<regs; ++i)
    {
        size_t index    = reverse_bits(FFT_TYPE(i), rank);

        ARCH_X86_ASM
        (
            /* Load scalar values */
            __ASM_EMIT("movss   (%[src_re], %[index], 4), %%xmm1") /* xmm1 = r0 x x x */
            __ASM_EMIT("movss   (%[src_im], %[index], 4), %%xmm2") /* xmm2 = i0 x x x */
            __ASM_EMIT("add     %[regs], %[index]")

            __ASM_EMIT("movss   (%[src_re], %[index], 4), %%xmm5") /* xmm5 = r2 x x x */
            __ASM_EMIT("movss   (%[src_im], %[index], 4), %%xmm6") /* xmm4 = i2 x x x */
            __ASM_EMIT("add     %[regs], %[index]")

            __ASM_EMIT("movss   (%[src_re], %[index], 4), %%xmm0") /* xmm3 = r1 x x x */
            __ASM_EMIT("movss   (%[src_im], %[index], 4), %%xmm3") /* xmm2 = i1 x x x */
            __ASM_EMIT("add     %[regs], %[index]")

            __ASM_EMIT("movss   (%[src_re], %[index], 4), %%xmm4") /* xmm7 = r3 x x x */
            __ASM_EMIT("movss   (%[src_im], %[index], 4), %%xmm7") /* xmm6 = i3 x x x */

            /* Perform 4-element butterfly */
            /* Shuffle 1 */

            __ASM_EMIT("movlhps %%xmm5, %%xmm1")        /* xmm1 = r0 x r2 x */
            __ASM_EMIT("movlhps %%xmm4, %%xmm0")        /* xmm0 = r1 x r3 x */
            __ASM_EMIT("movlhps %%xmm6, %%xmm2")        /* xmm2 = i0 x i2 x */
            __ASM_EMIT("movlhps %%xmm7, %%xmm3")        /* xmm3 = i1 x i3 x */

            __ASM_EMIT("shufps $0x88, %%xmm2, %%xmm1")  /* xmm1 = r0 r2 i0 i2 */
            __ASM_EMIT("shufps $0x88, %%xmm3, %%xmm0")  /* xmm0 = r1 r3 i1 i3 */
            __ASM_EMIT("movaps %%xmm1, %%xmm2")         /* xmm2 = r0 r2 i0 i2 */

            /* Transform 1 */
            __ASM_EMIT("addps   %%xmm0, %%xmm1")        /* xmm1 = sr0 sr2 si0 si2 = r0+r1 r2+r3 i0+i1 i2+i3 */
            __ASM_EMIT("subps   %%xmm0, %%xmm2")        /* xmm2 = sr1 sr3 si1 si3 = r0-r1 r2-r3 i0-i1 i2-i3 */

            /* Shuffle 2 */
            __ASM_EMIT("movaps  %%xmm1, %%xmm0")        /* xmm0 = sr0 sr2 si0 si2 */
            __ASM_EMIT("shufps $0x88, %%xmm2, %%xmm0")  /* xmm0 = sr0 si0 sr1 si1 */
            __ASM_EMIT("shufps $0x7d, %%xmm2, %%xmm1")  /* xmm1 = sr2 si2 si3 sr3 */
            __ASM_EMIT("movaps %%xmm0, %%xmm2")         /* xmm2 = sr0 si0 sr1 si1 */

            /* Transform 2 */
            __ASM_EMIT("addps   %%xmm1, %%xmm0")        /* xmm0 = dr0 di0 dr3 di1 = sr0+sr2 si0+si2 sr1+si3 si1+sr3 */
            __ASM_EMIT("subps   %%xmm1, %%xmm2")        /* xmm2 = dr2 di2 dr1 di3 = sr0-sr2 si0-si2 sr1-si3 si1-sr3 */

            /* Collect final values */
            __ASM_EMIT("movaps  %%xmm0, %%xmm1")        /* xmm1 = dr0 di0 dr3 di1 */
            __ASM_EMIT("shufps $0x88, %%xmm2, %%xmm0")  /* xmm0 = dr0 dr3 dr2 dr1 */
            __ASM_EMIT("shufps $0xdd, %%xmm2, %%xmm1")  /* xmm1 = di0 di1 di2 di3 */
            __ASM_EMIT("shufps $0x6c, %%xmm0, %%xmm0")  /* xmm0 = dr0 dr1 dr2 dr3 */

            /* Store values */
            __ASM_EMIT(LS_RE " %%xmm0, (%[dst_re])")
            __ASM_EMIT(LS_IM " %%xmm1, (%[dst_im])")

            /* Update pointers */
            __ASM_EMIT("add     $0x10, %[dst_re]")
            __ASM_EMIT("add     $0x10, %[dst_im]")

            : [dst_re] "+r" (dst_re), [dst_im] "+r"(dst_im), [index] "+r"(index)
            : [src_re] "r"(src_re), [src_im] "r"(src_im), [regs] __ASM_ARG_RO(regs)
            : "cc", "memory",
                "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }
}
#else
static inline void FFT_SCRAMBLE_SELF_DIRECT_NAME(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank)
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
            __ASM_EMIT("movss (%[dst_re], %[i], 4), %%xmm0")
            __ASM_EMIT("movss (%[dst_im], %[i], 4), %%xmm1")
            __ASM_EMIT("movss (%[dst_re], %[j], 4), %%xmm2")
            __ASM_EMIT("movss (%[dst_im], %[j], 4), %%xmm3")
            __ASM_EMIT("movss %%xmm2, (%[dst_re], %[i], 4)")
            __ASM_EMIT("movss %%xmm3, (%[dst_im], %[i], 4)")
            __ASM_EMIT("movss %%xmm0, (%[dst_re], %[j], 4)")
            __ASM_EMIT("movss %%xmm1, (%[dst_im], %[j], 4)")
            :
            : [dst_re] "r"(dst_re), [dst_im] "r"(dst_im), [i] "r"(i), [j] "r"(j)
            : "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3"
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
        __ASM_EMIT(LS_RE "      0x00(%[dst_re]), %%xmm0")   /* xmm0 = r0 r1 r2 r3 */
        __ASM_EMIT(LS_RE "      0x10(%[dst_re]), %%xmm4")   /* xmm4 = r4 r5 r6 r7 */
        __ASM_EMIT(LS_IM "      0x00(%[dst_im]), %%xmm2")   /* xmm2 = i0 i1 i2 i3 */
        __ASM_EMIT(LS_IM "      0x10(%[dst_im]), %%xmm6")   /* xmm6 = i4 i5 i6 i7 */

        /* 1st-order 4x butterfly */
        __ASM_EMIT("movaps      %%xmm0, %%xmm1")            /* xmm1 = r0 r1 r2 r3 */
        __ASM_EMIT("movaps      %%xmm2, %%xmm3")            /* xmm3 = i0 i1 i2 i3 */
        __ASM_EMIT("shufps      $0x88, %%xmm4, %%xmm0")     /* xmm0 = r0 r2 r4 r6 */
        __ASM_EMIT("shufps      $0x88, %%xmm6, %%xmm2")     /* xmm2 = i0 i2 i4 i6 */
        __ASM_EMIT("shufps      $0xdd, %%xmm4, %%xmm1")     /* xmm1 = r1 r3 r5 r7 */
        __ASM_EMIT("shufps      $0xdd, %%xmm6, %%xmm3")     /* xmm3 = i1 i3 i5 i7 */

        __ASM_EMIT("movaps      %%xmm0, %%xmm4")            /* xmm4 = r0 r2 r4 r6 */
        __ASM_EMIT("movaps      %%xmm2, %%xmm6")            /* xmm5 = i0 i2 i4 i6 */
        __ASM_EMIT("addps       %%xmm1, %%xmm0")            /* xmm0 = r0+r1 r2+r3 r4+r5 r6+r7 = r0' r2' r4' r6' */
        __ASM_EMIT("addps       %%xmm3, %%xmm2")            /* xmm2 = i0+i1 i2+i3 i4+i5 i6+i7 = i0' i2' i4' i6' */
        __ASM_EMIT("subps       %%xmm1, %%xmm4")            /* xmm4 = r0-r1 r2-r3 r4-r5 r6-r7 = r1' r3' r5' r7' */
        __ASM_EMIT("subps       %%xmm3, %%xmm6")            /* xmm6 = i0-i1 i2-i3 i4-i5 i6-i7 = i1' i3' i5' i7' */

        /* 2nd-order 4x butterfly */
        __ASM_EMIT("movaps      %%xmm0, %%xmm1")            /* xmm1 = r0' r2' r4' r6' */
        __ASM_EMIT("movaps      %%xmm2, %%xmm3")            /* xmm3 = i0' i2' i4' i6' */
        __ASM_EMIT("shufps      $0x88, %%xmm4, %%xmm0")     /* xmm0 = r0' r4' r1' r5' */
        __ASM_EMIT("shufps      $0xdd, %%xmm6, %%xmm1")     /* xmm1 = r2' r6' i3' i7' */
        __ASM_EMIT("shufps      $0xdd, %%xmm4, %%xmm3")     /* xmm3 = i2' i6' r3' r7' */
        __ASM_EMIT("shufps      $0x88, %%xmm6, %%xmm2")     /* xmm2 = i0' i4' i1' i5' */

        __ASM_EMIT("movaps      %%xmm0, %%xmm4")            /* xmm4 = r0' r4' r1' r5' */
        __ASM_EMIT("movaps      %%xmm2, %%xmm6")            /* xmm5 = i0' i4' i1' i5' */
        __ASM_EMIT("addps       %%xmm1, %%xmm0")            /* xmm0 = r0'+r2' r4'+r6' r1'+i3' r5'+i7' = r0" r4" r1" r5" */
        __ASM_EMIT("addps       %%xmm3, %%xmm2")            /* xmm2 = i0'+i2' i4'+i6' i1'+r3' i5'+r7' = i0" i4" i3" i7" */
        __ASM_EMIT("subps       %%xmm1, %%xmm4")            /* xmm4 = r0'-r2' r4'-r6' r1'-i3' r5'-i7' = r2" r6" r3" r7" */
        __ASM_EMIT("subps       %%xmm3, %%xmm6")            /* xmm6 = i0'-i2' i4'-i6' i1'-r3' i5'-r7' = i2" i6" i1" i5" */

        /* Reorder and store */
        __ASM_EMIT("movaps      %%xmm0, %%xmm1")            /* xmm1 = r0" r4" r1" r5" */
        __ASM_EMIT("movaps      %%xmm2, %%xmm3")            /* xmm3 = i0" i4" i3" i7" */
        __ASM_EMIT("shufps      $0x88, %%xmm4, %%xmm0")     /* xmm0 = r0" r1" r2" r3" */
        __ASM_EMIT("shufps      $0x88, %%xmm6, %%xmm2")     /* xmm2 = i0" i3" i2" i1" */
        __ASM_EMIT("shufps      $0xdd, %%xmm4, %%xmm1")     /* xmm1 = r4" r5" r6" r7" */
        __ASM_EMIT("shufps      $0xdd, %%xmm6, %%xmm3")     /* xmm3 = i4" i7" i6" i5" */
        __ASM_EMIT("shufps      $0x6c, %%xmm2, %%xmm2")     /* xmm2 = i0" i1" i2" i3" */
        __ASM_EMIT("shufps      $0x6c, %%xmm3, %%xmm3")     /* xmm3 = i4" i5" i6" i7" */

        __ASM_EMIT(LS_RE "      %%xmm0, 0x00(%[dst_re])")
        __ASM_EMIT(LS_RE "      %%xmm1, 0x10(%[dst_re])")
        __ASM_EMIT(LS_IM "      %%xmm2, 0x00(%[dst_im])")
        __ASM_EMIT(LS_IM "      %%xmm3, 0x10(%[dst_im])")

        /* Move pointers */
        __ASM_EMIT("add     $0x20, %[dst_re]")
        __ASM_EMIT("add     $0x20, %[dst_im]")

        /* Repeat cycle */
        __ASM_EMIT("dec     %[items]")
        __ASM_EMIT("jnz     1b")
        __ASM_EMIT("2:")
        : [dst_re] "+r"(dst_re), [dst_im] "+r"(dst_im), [items] "+r"(items)
        :
        : "cc", "memory",
          "%xmm0", "%xmm1", "%xmm2"
    );
}

static inline void FFT_SCRAMBLE_COPY_DIRECT_NAME(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank)
{
    size_t regs     = 1 << rank;

    for (size_t i=0; i<regs; ++i)
    {
        size_t index    = reverse_bits(FFT_TYPE(i), rank);

        ARCH_X86_ASM
        (
            /* Load scalar values */
            __ASM_EMIT("movss       (%[src_re], %[index], 4), %%xmm0")  /* xmm0 = r0 x x x      */
            __ASM_EMIT("movss       (%[src_im], %[index], 4), %%xmm2")  /* xmm2 = i0 x x x      */
            __ASM_EMIT("add         %[regs], %[index]")

            __ASM_EMIT("movss       (%[src_re], %[index], 4), %%xmm1")  /* xmm1 = r4 x x x      */
            __ASM_EMIT("movss       (%[src_im], %[index], 4), %%xmm3")  /* xmm3 = i4 x x x      */
            __ASM_EMIT("add         %[regs], %[index]")

            __ASM_EMIT("movss       (%[src_re], %[index], 4), %%xmm4")  /* xmm4 = r2 x x x      */
            __ASM_EMIT("movss       (%[src_im], %[index], 4), %%xmm6")  /* xmm6 = i2 x x x      */
            __ASM_EMIT("add         %[regs], %[index]")
            __ASM_EMIT("movlhps     %%xmm4, %%xmm0")                    /* xmm0 = r0 x r2 x     */
            __ASM_EMIT("movlhps     %%xmm6, %%xmm2")                    /* xmm2 = i0 x i2 x     */

            __ASM_EMIT("movss       (%[src_re], %[index], 4), %%xmm4")  /* xmm4 = r6 x x x      */
            __ASM_EMIT("movss       (%[src_im], %[index], 4), %%xmm6")  /* xmm6 = i6 x x x      */
            __ASM_EMIT("add         %[regs], %[index]")
            __ASM_EMIT("movlhps     %%xmm4, %%xmm1")                    /* xmm1 = r4 x r6 x     */
            __ASM_EMIT("movlhps     %%xmm6, %%xmm3")                    /* xmm3 = i4 x i6 x     */
            __ASM_EMIT("shufps      $0x88, %%xmm1, %%xmm0")             /* xmm0 = r0 r2 r4 r6   */
            __ASM_EMIT("shufps      $0x88, %%xmm3, %%xmm2")             /* xmm2 = i0 i2 i4 i6   */

            __ASM_EMIT("movss       (%[src_re], %[index], 4), %%xmm1")  /* xmm1 = r1 x x x      */
            __ASM_EMIT("movss       (%[src_im], %[index], 4), %%xmm3")  /* xmm3 = i1 x x x      */
            __ASM_EMIT("add         %[regs], %[index]")

            __ASM_EMIT("movss       (%[src_re], %[index], 4), %%xmm4")  /* xmm4 = r5 x x x      */
            __ASM_EMIT("movss       (%[src_im], %[index], 4), %%xmm6")  /* xmm6 = i5 x x x      */
            __ASM_EMIT("add         %[regs], %[index]")

            __ASM_EMIT("movss       (%[src_re], %[index], 4), %%xmm5")  /* xmm5 = r3 x x x      */
            __ASM_EMIT("movss       (%[src_im], %[index], 4), %%xmm7")  /* xmm7 = i3 x x x      */
            __ASM_EMIT("add         %[regs], %[index]")
            __ASM_EMIT("movlhps     %%xmm5, %%xmm1")                    /* xmm1 = r1 x r3 x     */
            __ASM_EMIT("movlhps     %%xmm7, %%xmm3")                    /* xmm3 = i1 x i3 x     */

            __ASM_EMIT("movss       (%[src_re], %[index], 4), %%xmm5")  /* xmm5 = r7 x x x      */
            __ASM_EMIT("movss       (%[src_im], %[index], 4), %%xmm7")  /* xmm7 = i7 x x x      */
            __ASM_EMIT("movlhps     %%xmm5, %%xmm4")                    /* xmm4 = r5 x r7 x     */
            __ASM_EMIT("movlhps     %%xmm7, %%xmm6")                    /* xmm6 = i5 x i7 x     */
            __ASM_EMIT("shufps      $0x88, %%xmm4, %%xmm1")             /* xmm1 = r1 r3 r5 r7   */
            __ASM_EMIT("shufps      $0x88, %%xmm6, %%xmm3")             /* xmm3 = i1 i3 i5 i7   */

            /* 1st-order 4x butterfly */
            /* xmm0 = r0 r2 r4 r6 */
            /* xmm2 = i0 i2 i4 i6 */
            /* xmm1 = r1 r3 r5 r7 */
            /* xmm3 = i1 i3 i5 i7 */

            __ASM_EMIT("movaps      %%xmm0, %%xmm4")            /* xmm4 = r0 r2 r4 r6 */
            __ASM_EMIT("movaps      %%xmm2, %%xmm6")            /* xmm5 = i0 i2 i4 i6 */
            __ASM_EMIT("addps       %%xmm1, %%xmm0")            /* xmm0 = r0+r1 r2+r3 r4+r5 r6+r7 = r0' r2' r4' r6' */
            __ASM_EMIT("addps       %%xmm3, %%xmm2")            /* xmm2 = i0+i1 i2+i3 i4+i5 i6+i7 = i0' i2' i4' i6' */
            __ASM_EMIT("subps       %%xmm1, %%xmm4")            /* xmm4 = r0-r1 r2-r3 r4-r5 r6-r7 = r1' r3' r5' r7' */
            __ASM_EMIT("subps       %%xmm3, %%xmm6")            /* xmm6 = i0-i1 i2-i3 i4-i5 i6-i7 = i1' i3' i5' i7' */

            /* 2nd-order 4x butterfly */
            __ASM_EMIT("movaps      %%xmm0, %%xmm1")            /* xmm1 = r0' r2' r4' r6' */
            __ASM_EMIT("movaps      %%xmm2, %%xmm3")            /* xmm3 = i0' i2' i4' i6' */
            __ASM_EMIT("shufps      $0x88, %%xmm4, %%xmm0")     /* xmm0 = r0' r4' r1' r5' */
            __ASM_EMIT("shufps      $0xdd, %%xmm6, %%xmm1")     /* xmm1 = r2' r6' i3' i7' */
            __ASM_EMIT("shufps      $0xdd, %%xmm4, %%xmm3")     /* xmm3 = i2' i6' r3' r7' */
            __ASM_EMIT("shufps      $0x88, %%xmm6, %%xmm2")     /* xmm2 = i0' i4' i1' i5' */

            __ASM_EMIT("movaps      %%xmm0, %%xmm4")            /* xmm4 = r0' r4' r1' r5' */
            __ASM_EMIT("movaps      %%xmm2, %%xmm6")            /* xmm5 = i0' i4' i1' i5' */
            __ASM_EMIT("addps       %%xmm1, %%xmm0")            /* xmm0 = r0'+r2' r4'+r6' r1'+i3' r5'+i7' = r0" r4" r1" r5" */
            __ASM_EMIT("addps       %%xmm3, %%xmm2")            /* xmm2 = i0'+i2' i4'+i6' i1'+i3' i5'+i7' = i0" i4" i3" i7" */
            __ASM_EMIT("subps       %%xmm1, %%xmm4")            /* xmm4 = r0'-r2' r4'-r6' r1'-i3' r5'-i7' = r2" r6" r3" r7" */
            __ASM_EMIT("subps       %%xmm3, %%xmm6")            /* xmm6 = i0'-i2' i4'-i6' i1'-i3' i5'-i7' = i2" i6" i1" i5" */

            /* Reorder and store */
            __ASM_EMIT("movaps      %%xmm0, %%xmm1")            /* xmm1 = r0" r4" r1" r5" */
            __ASM_EMIT("movaps      %%xmm2, %%xmm3")            /* xmm3 = i0" i4" i3" i7" */
            __ASM_EMIT("shufps      $0x88, %%xmm4, %%xmm0")     /* xmm0 = r0" r1" r2" r3" */
            __ASM_EMIT("shufps      $0x88, %%xmm6, %%xmm2")     /* xmm2 = i0" i3" i2" i1" */
            __ASM_EMIT("shufps      $0xdd, %%xmm4, %%xmm1")     /* xmm1 = r4" r5" r6" r7" */
            __ASM_EMIT("shufps      $0xdd, %%xmm6, %%xmm3")     /* xmm3 = i4" i7" i6" i5" */
            __ASM_EMIT("shufps      $0x6c, %%xmm2, %%xmm2")     /* xmm2 = i0" i1" i2" i3" */
            __ASM_EMIT("shufps      $0x6c, %%xmm3, %%xmm3")     /* xmm3 = i4" i5" i6" i7" */

            __ASM_EMIT(LS_RE "      %%xmm0, 0x00(%[dst_re])")
            __ASM_EMIT(LS_RE "      %%xmm1, 0x10(%[dst_re])")
            __ASM_EMIT(LS_IM "      %%xmm2, 0x00(%[dst_im])")
            __ASM_EMIT(LS_IM "      %%xmm3, 0x10(%[dst_im])")

            /* Update pointers */
            __ASM_EMIT("add     $0x20, %[dst_re]")
            __ASM_EMIT("add     $0x20, %[dst_im]")

            : [dst_re] "+r" (dst_re), [dst_im] "+r"(dst_im), [index] "+r"(index)
            : [src_re] "r"(src_re), [src_im] "r"(src_im), [regs] __ASM_ARG_RO(regs)
            : "cc", "memory",
                "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }
}

static inline void FFT_SCRAMBLE_SELF_REVERSE_NAME(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank)
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
            __ASM_EMIT("movss (%[dst_re], %[i], 4), %%xmm0")
            __ASM_EMIT("movss (%[dst_im], %[i], 4), %%xmm1")
            __ASM_EMIT("movss (%[dst_re], %[j], 4), %%xmm2")
            __ASM_EMIT("movss (%[dst_im], %[j], 4), %%xmm3")
            __ASM_EMIT("movss %%xmm2, (%[dst_re], %[i], 4)")
            __ASM_EMIT("movss %%xmm3, (%[dst_im], %[i], 4)")
            __ASM_EMIT("movss %%xmm0, (%[dst_re], %[j], 4)")
            __ASM_EMIT("movss %%xmm1, (%[dst_im], %[j], 4)")
            :
            : [dst_re] "r"(dst_re), [dst_im] "r"(dst_im), [i] "r"(i), [j] "r"(j)
            : "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3"
        );
    }

    // Perform butterfly 4x
    items = 1 << (rank - 3);

    // Perform 4-element butterflies
    ARCH_X86_ASM
    (
        /* Loop */
        __ASM_EMIT(".align 16")
        __ASM_EMIT("1:")

        /* Load data to registers */
        __ASM_EMIT(LS_RE "      0x00(%[dst_re]), %%xmm0")   /* xmm0 = r0 r1 r2 r3 */
        __ASM_EMIT(LS_RE "      0x10(%[dst_re]), %%xmm4")   /* xmm4 = r4 r5 r6 r7 */
        __ASM_EMIT(LS_IM "      0x00(%[dst_im]), %%xmm2")   /* xmm2 = i0 i1 i2 i3 */
        __ASM_EMIT(LS_IM "      0x10(%[dst_im]), %%xmm6")   /* xmm6 = i4 i5 i6 i7 */

        /* 1st-order 4x butterfly */
        __ASM_EMIT("movaps      %%xmm0, %%xmm1")            /* xmm1 = r0 r1 r2 r3 */
        __ASM_EMIT("movaps      %%xmm2, %%xmm3")            /* xmm3 = i0 i1 i2 i3 */
        __ASM_EMIT("shufps      $0x88, %%xmm4, %%xmm0")     /* xmm0 = r0 r2 r4 r6 */
        __ASM_EMIT("shufps      $0x88, %%xmm6, %%xmm2")     /* xmm2 = i0 i2 i4 i6 */
        __ASM_EMIT("shufps      $0xdd, %%xmm4, %%xmm1")     /* xmm1 = r1 r3 r5 r7 */
        __ASM_EMIT("shufps      $0xdd, %%xmm6, %%xmm3")     /* xmm3 = i1 i3 i5 i7 */

        __ASM_EMIT("movaps      %%xmm0, %%xmm4")            /* xmm4 = r0 r2 r4 r6 */
        __ASM_EMIT("movaps      %%xmm2, %%xmm6")            /* xmm5 = i0 i2 i4 i6 */
        __ASM_EMIT("addps       %%xmm1, %%xmm0")            /* xmm0 = r0+r1 r2+r3 r4+r5 r6+r7 = r0' r2' r4' r6' */
        __ASM_EMIT("addps       %%xmm3, %%xmm2")            /* xmm2 = i0+i1 i2+i3 i4+i5 i6+i7 = i0' i2' i4' i6' */
        __ASM_EMIT("subps       %%xmm1, %%xmm4")            /* xmm4 = r0-r1 r2-r3 r4-r5 r6-r7 = r1' r3' r5' r7' */
        __ASM_EMIT("subps       %%xmm3, %%xmm6")            /* xmm6 = i0-i1 i2-i3 i4-i5 i6-i7 = i1' i3' i5' i7' */

        /* 2nd-order 4x butterfly */
        __ASM_EMIT("movaps      %%xmm0, %%xmm1")            /* xmm1 = r0' r2' r4' r6' */
        __ASM_EMIT("movaps      %%xmm2, %%xmm3")            /* xmm3 = i0' i2' i4' i6' */
        __ASM_EMIT("shufps      $0x88, %%xmm4, %%xmm0")     /* xmm0 = r0' r4' r1' r5' */
        __ASM_EMIT("shufps      $0xdd, %%xmm6, %%xmm1")     /* xmm1 = r2' r6' i3' i7' */
        __ASM_EMIT("shufps      $0xdd, %%xmm4, %%xmm3")     /* xmm3 = i2' i6' r3' r7' */
        __ASM_EMIT("shufps      $0x88, %%xmm6, %%xmm2")     /* xmm2 = i0' i4' i1' i5' */

        __ASM_EMIT("movaps      %%xmm0, %%xmm4")            /* xmm4 = r0' r4' r1' r5' */
        __ASM_EMIT("movaps      %%xmm2, %%xmm6")            /* xmm5 = i0' i4' i1' i5' */
        __ASM_EMIT("addps       %%xmm1, %%xmm0")            /* xmm0 = r0'+r2' r4'+r6' r1'+i3' r5'+i7' = r0" r4" r3" r7" */
        __ASM_EMIT("addps       %%xmm3, %%xmm2")            /* xmm2 = i0'+i2' i4'+i6' i1'+i3' i5'+i7' = i0" i4" i1" i5" */
        __ASM_EMIT("subps       %%xmm1, %%xmm4")            /* xmm4 = r0'-r2' r4'-r6' r1'-i3' r5'-i7' = r2" r6" r1" r5" */
        __ASM_EMIT("subps       %%xmm3, %%xmm6")            /* xmm6 = i0'-i2' i4'-i6' i1'-i3' i5'-i7' = i2" i6" i3" i7" */

        /* Reorder and store */
        __ASM_EMIT("movaps      %%xmm0, %%xmm1")            /* xmm1 = r0" r4" r3" r7" */
        __ASM_EMIT("movaps      %%xmm2, %%xmm3")            /* xmm3 = i0" i4" i1" i5" */
        __ASM_EMIT("shufps      $0x88, %%xmm4, %%xmm0")     /* xmm0 = r0" r3" r2" r1" */
        __ASM_EMIT("shufps      $0x88, %%xmm6, %%xmm2")     /* xmm2 = i0" i1" i2" i3" */
        __ASM_EMIT("shufps      $0xdd, %%xmm4, %%xmm1")     /* xmm1 = r4" r7" r6" r5" */
        __ASM_EMIT("shufps      $0xdd, %%xmm6, %%xmm3")     /* xmm3 = i4" i5" i6" i7" */
        __ASM_EMIT("shufps      $0x6c, %%xmm0, %%xmm0")     /* xmm0 = r0" r1" r2" r3" */
        __ASM_EMIT("shufps      $0x6c, %%xmm1, %%xmm1")     /* xmm1 = r4" r5" r6" r7" */

        __ASM_EMIT(LS_RE "      %%xmm0, 0x00(%[dst_re])")
        __ASM_EMIT(LS_RE "      %%xmm1, 0x10(%[dst_re])")
        __ASM_EMIT(LS_IM "      %%xmm2, 0x00(%[dst_im])")
        __ASM_EMIT(LS_IM "      %%xmm3, 0x10(%[dst_im])")

        /* Move pointers */
        __ASM_EMIT("add     $0x20, %[dst_re]")
        __ASM_EMIT("add     $0x20, %[dst_im]")

        /* Repeat cycle */
        __ASM_EMIT("dec     %[items]")
        __ASM_EMIT("jnz     1b")
        __ASM_EMIT("2:")
        : [dst_re] "+r"(dst_re), [dst_im] "+r"(dst_im), [items] "+r"(items)
        :
        : "cc", "memory",
          "%xmm0", "%xmm1", "%xmm2"
    );
}

static inline void FFT_SCRAMBLE_COPY_REVERSE_NAME(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank)
{
    size_t regs     = 1 << rank;

    for (size_t i=0; i<regs; ++i)
    {
        size_t index    = reverse_bits(FFT_TYPE(i), rank);

        ARCH_X86_ASM
        (
            /* Load scalar values */
            __ASM_EMIT("movss       (%[src_re], %[index], 4), %%xmm0")  /* xmm0 = r0 x x x      */
            __ASM_EMIT("movss       (%[src_im], %[index], 4), %%xmm2")  /* xmm2 = i0 x x x      */
            __ASM_EMIT("add         %[regs], %[index]")

            __ASM_EMIT("movss       (%[src_re], %[index], 4), %%xmm1")  /* xmm1 = r4 x x x      */
            __ASM_EMIT("movss       (%[src_im], %[index], 4), %%xmm3")  /* xmm3 = i4 x x x      */
            __ASM_EMIT("add         %[regs], %[index]")

            __ASM_EMIT("movss       (%[src_re], %[index], 4), %%xmm4")  /* xmm4 = r2 x x x      */
            __ASM_EMIT("movss       (%[src_im], %[index], 4), %%xmm6")  /* xmm6 = i2 x x x      */
            __ASM_EMIT("add         %[regs], %[index]")
            __ASM_EMIT("movlhps     %%xmm4, %%xmm0")                    /* xmm0 = r0 x r2 x     */
            __ASM_EMIT("movlhps     %%xmm6, %%xmm2")                    /* xmm2 = i0 x i2 x     */

            __ASM_EMIT("movss       (%[src_re], %[index], 4), %%xmm4")  /* xmm4 = r6 x x x      */
            __ASM_EMIT("movss       (%[src_im], %[index], 4), %%xmm6")  /* xmm6 = i6 x x x      */
            __ASM_EMIT("add         %[regs], %[index]")
            __ASM_EMIT("movlhps     %%xmm4, %%xmm1")                    /* xmm1 = r4 x r6 x     */
            __ASM_EMIT("movlhps     %%xmm6, %%xmm3")                    /* xmm3 = i4 x i6 x     */
            __ASM_EMIT("shufps      $0x88, %%xmm1, %%xmm0")             /* xmm0 = r0 r2 r4 r6   */
            __ASM_EMIT("shufps      $0x88, %%xmm3, %%xmm2")             /* xmm2 = i0 i2 i4 i6   */

            __ASM_EMIT("movss       (%[src_re], %[index], 4), %%xmm1")  /* xmm1 = r1 x x x      */
            __ASM_EMIT("movss       (%[src_im], %[index], 4), %%xmm3")  /* xmm3 = i1 x x x      */
            __ASM_EMIT("add         %[regs], %[index]")

            __ASM_EMIT("movss       (%[src_re], %[index], 4), %%xmm4")  /* xmm4 = r5 x x x      */
            __ASM_EMIT("movss       (%[src_im], %[index], 4), %%xmm6")  /* xmm6 = i5 x x x      */
            __ASM_EMIT("add         %[regs], %[index]")

            __ASM_EMIT("movss       (%[src_re], %[index], 4), %%xmm5")  /* xmm5 = r3 x x x      */
            __ASM_EMIT("movss       (%[src_im], %[index], 4), %%xmm7")  /* xmm7 = i3 x x x      */
            __ASM_EMIT("add         %[regs], %[index]")
            __ASM_EMIT("movlhps     %%xmm5, %%xmm1")                    /* xmm1 = r1 x r3 x     */
            __ASM_EMIT("movlhps     %%xmm7, %%xmm3")                    /* xmm3 = i1 x i3 x     */

            __ASM_EMIT("movss       (%[src_re], %[index], 4), %%xmm5")  /* xmm5 = r7 x x x      */
            __ASM_EMIT("movss       (%[src_im], %[index], 4), %%xmm7")  /* xmm7 = i7 x x x      */
            __ASM_EMIT("movlhps     %%xmm5, %%xmm4")                    /* xmm4 = r5 x r7 x     */
            __ASM_EMIT("movlhps     %%xmm7, %%xmm6")                    /* xmm6 = i5 x i7 x     */
            __ASM_EMIT("shufps      $0x88, %%xmm4, %%xmm1")             /* xmm1 = r1 r3 r5 r7   */
            __ASM_EMIT("shufps      $0x88, %%xmm6, %%xmm3")             /* xmm3 = i1 i3 i5 i7   */

            /* 1st-order 4x butterfly */
            /* xmm0 = r0 r2 r4 r6 */
            /* xmm2 = i0 i2 i4 i6 */
            /* xmm1 = r1 r3 r5 r7 */
            /* xmm3 = i1 i3 i5 i7 */

            __ASM_EMIT("movaps      %%xmm0, %%xmm4")            /* xmm4 = r0 r2 r4 r6 */
            __ASM_EMIT("movaps      %%xmm2, %%xmm6")            /* xmm5 = i0 i2 i4 i6 */
            __ASM_EMIT("addps       %%xmm1, %%xmm0")            /* xmm0 = r0+r1 r2+r3 r4+r5 r6+r7 = r0' r2' r4' r6' */
            __ASM_EMIT("addps       %%xmm3, %%xmm2")            /* xmm2 = i0+i1 i2+i3 i4+i5 i6+i7 = i0' i2' i4' i6' */
            __ASM_EMIT("subps       %%xmm1, %%xmm4")            /* xmm4 = r0-r1 r2-r3 r4-r5 r6-r7 = r1' r3' r5' r7' */
            __ASM_EMIT("subps       %%xmm3, %%xmm6")            /* xmm6 = i0-i1 i2-i3 i4-i5 i6-i7 = i1' i3' i5' i7' */

            /* 2nd-order 4x butterfly */
            __ASM_EMIT("movaps      %%xmm0, %%xmm1")            /* xmm1 = r0' r2' r4' r6' */
            __ASM_EMIT("movaps      %%xmm2, %%xmm3")            /* xmm3 = i0' i2' i4' i6' */
            __ASM_EMIT("shufps      $0x88, %%xmm4, %%xmm0")     /* xmm0 = r0' r4' r1' r5' */
            __ASM_EMIT("shufps      $0xdd, %%xmm6, %%xmm1")     /* xmm1 = r2' r6' i3' i7' */
            __ASM_EMIT("shufps      $0xdd, %%xmm4, %%xmm3")     /* xmm3 = i2' i6' r3' r7' */
            __ASM_EMIT("shufps      $0x88, %%xmm6, %%xmm2")     /* xmm2 = i0' i4' i1' i5' */

            __ASM_EMIT("movaps      %%xmm0, %%xmm4")            /* xmm4 = r0' r4' r1' r5' */
            __ASM_EMIT("movaps      %%xmm2, %%xmm6")            /* xmm5 = i0' i4' i1' i5' */
            __ASM_EMIT("addps       %%xmm1, %%xmm0")            /* xmm0 = r0'+r2' r4'+r6' r1'+i3' r5'+i7' = r0" r4" r3" r7" */
            __ASM_EMIT("addps       %%xmm3, %%xmm2")            /* xmm2 = i0'+i2' i4'+i6' i1'+r3' i5'+i7' = i0" i4" i1" i5" */
            __ASM_EMIT("subps       %%xmm1, %%xmm4")            /* xmm4 = r0'-r2' r4'-r6' r1'-i3' r5'-i7' = r2" r6" r1" r5" */
            __ASM_EMIT("subps       %%xmm3, %%xmm6")            /* xmm6 = i0'-i2' i4'-i6' i1'-r3' i5'-i7' = i2" i6" i3" i7" */

            /* Reorder and store */
            __ASM_EMIT("movaps      %%xmm0, %%xmm1")            /* xmm1 = r0" r4" r3" r7" */
            __ASM_EMIT("movaps      %%xmm2, %%xmm3")            /* xmm3 = i0" i4" i1" i5" */
            __ASM_EMIT("shufps      $0x88, %%xmm4, %%xmm0")     /* xmm0 = r0" r3" r2" r1" */
            __ASM_EMIT("shufps      $0x88, %%xmm6, %%xmm2")     /* xmm2 = i0" i1" i2" i3" */
            __ASM_EMIT("shufps      $0xdd, %%xmm4, %%xmm1")     /* xmm1 = r4" r7" r6" r5" */
            __ASM_EMIT("shufps      $0xdd, %%xmm6, %%xmm3")     /* xmm3 = i4" i5" i6" i7" */
            __ASM_EMIT("shufps      $0x6c, %%xmm0, %%xmm0")     /* xmm0 = r0" r1" r2" r3" */
            __ASM_EMIT("shufps      $0x6c, %%xmm1, %%xmm1")     /* xmm1 = r4" r5" r6" r7" */

            __ASM_EMIT(LS_RE "      %%xmm0, 0x00(%[dst_re])")
            __ASM_EMIT(LS_RE "      %%xmm1, 0x10(%[dst_re])")
            __ASM_EMIT(LS_IM "      %%xmm2, 0x00(%[dst_im])")
            __ASM_EMIT(LS_IM "      %%xmm3, 0x10(%[dst_im])")

            /* Update pointers */
            __ASM_EMIT("add     $0x20, %[dst_re]")
            __ASM_EMIT("add     $0x20, %[dst_im]")

            : [dst_re] "+r" (dst_re), [dst_im] "+r"(dst_im), [index] "+r"(index)
            : [src_re] "r"(src_re), [src_im] "r"(src_im), [regs] __ASM_ARG_RO(regs)
            : "cc", "memory",
                "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }
}
#endif

#undef FFT_SCRAMBLE_SELF_DIRECT_NAME
#undef FFT_SCRAMBLE_SELF_REVERSE_NAME
#undef FFT_SCRAMBLE_COPY_DIRECT_NAME
#undef FFT_SCRAMBLE_COPY_REVERSE_NAME
#undef FFT_TYPE
#undef LS_RE
#undef LS_IM

