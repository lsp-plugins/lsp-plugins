/*
 * p_switch.h
 *
 *  Created on: 15 февр. 2017 г.
 *      Author: sadko
 */

// This is the SSE implementation of the scrambling functions for self data

#define _SSE_FFT_NAME(id, mode) id ## _ ## mode
#define _SSE_CALL_NAME(id, bits, mode) id ## bits ## _ ## mode

#define SSE_FFT_NAME(id, mode) _SSE_FFT_NAME(id, mode)
#define SSE_CALL_NAME(id, bits, mode) _SSE_CALL_NAME(id, bits, mode)(dst, src, rank)

static inline void SSE_FFT_NAME(FFT_SCRAMBLE_DIRECT_NAME, FFT_MODE)(float *dst, const float *src, size_t rank)
{
    // Scramble the order of samples
    if (dst == src)
    {
        if (rank <= 8)
            SSE_CALL_NAME(FFT_SCRAMBLE_SELF_DIRECT_NAME, 8, FFT_MODE);
        else //if (rank <= 16)
            SSE_CALL_NAME(FFT_SCRAMBLE_SELF_DIRECT_NAME, 16, FFT_MODE);
    }
    else
    {
        rank -= 3;

        if (rank <= 8)
            SSE_CALL_NAME(FFT_SCRAMBLE_COPY_DIRECT_NAME, 8, FFT_MODE);
        else //if (rank <= 16)
            SSE_CALL_NAME(FFT_SCRAMBLE_COPY_DIRECT_NAME, 16, FFT_MODE);
    }
}

static inline void SSE_FFT_NAME(FFT_SCRAMBLE_REVERSE_NAME, FFT_MODE)(float *dst, const float *src, size_t rank)
{
    // Scramble the order of samples
    if (dst == src)
    {
        if (rank <= 8)
            SSE_CALL_NAME(FFT_SCRAMBLE_SELF_REVERSE_NAME, 8, FFT_MODE);
        else //if (rank <= 16)
            SSE_CALL_NAME(FFT_SCRAMBLE_SELF_REVERSE_NAME, 16, FFT_MODE);
    }
    else
    {
        rank -= 3;

        if (rank <= 8)
            SSE_CALL_NAME(FFT_SCRAMBLE_COPY_REVERSE_NAME, 8, FFT_MODE);
        else //if (rank <= 16)
            SSE_CALL_NAME(FFT_SCRAMBLE_COPY_REVERSE_NAME, 16, FFT_MODE);
    }
}

static inline void SSE_FFT_NAME(FFT_REPACK, FFT_MODE)(float *dst, size_t rank)
{
    size_t blocks     = 1 << (rank-3);

    // Perform 4-element butterflies
    ARCH_X86_ASM
    (
        __ASM_EMIT(".align 16")
        __ASM_EMIT("1:")

        /* Load data */
        __ASM_EMIT(LS_RE "      0x00(%[dst]), %%xmm0")      /* xmm0 = r0 r1 r2 r3 */
        __ASM_EMIT(LS_RE "      0x10(%[dst]), %%xmm1")      /* xmm1 = i0 i1 i2 i3 */
        __ASM_EMIT(LS_RE "      0x20(%[dst]), %%xmm4")      /* xmm4 = r4 r5 r6 r7 */
        __ASM_EMIT(LS_RE "      0x30(%[dst]), %%xmm5")      /* xmm5 = i4 i5 i6 i7 */

        /* 1st step: repack pairs */
        __ASM_EMIT("movaps      %%xmm0, %%xmm2")            /* xmm2 = r0 r1 r2 r3 */
        __ASM_EMIT("movaps      %%xmm4, %%xmm6")            /* xmm6 = r4 r5 r6 r7 */
        __ASM_EMIT("unpcklps    %%xmm1, %%xmm0")            /* xmm0 = r0 i0 r1 i1 */
        __ASM_EMIT("unpcklps    %%xmm5, %%xmm4")            /* xmm4 = r4 i4 r5 i5 */
        __ASM_EMIT("unpckhps    %%xmm1, %%xmm2")            /* xmm2 = r2 i2 r3 i3 */
        __ASM_EMIT("unpckhps    %%xmm5, %%xmm6")            /* xmm6 = r6 i6 r7 i7 */

        /* Store data */
        __ASM_EMIT(LS_RE "      %%xmm0, 0x00(%[dst])")
        __ASM_EMIT(LS_RE "      %%xmm2, 0x10(%[dst])")
        __ASM_EMIT(LS_RE "      %%xmm4, 0x20(%[dst])")
        __ASM_EMIT(LS_RE "      %%xmm6, 0x30(%[dst])")

        /* Move pointers and repeat cycle */
        __ASM_EMIT("add         $0x40, %[dst]")
        __ASM_EMIT("dec         %[blocks]")
        __ASM_EMIT("jnz         1b")

        : [dst] "+r"(dst), [blocks] "+r"(blocks)
        :
        : "cc", "memory",
          "%xmm0", "%xmm1", "%xmm2",
          "%xmm4", "%xmm5", "%xmm6"
    );
}

static inline void SSE_FFT_NAME(FFT_REPACK_NORMALIZE, FFT_MODE)(float *dst, size_t rank)
{
    size_t blocks       = 1 << (rank-3);
    float k             = 0.125f/blocks;

    // Perform 4-element butterflies
    ARCH_X86_ASM
    (
        __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0")     /* xmm0 = k  k  k  k  */
        __ASM_EMIT("movaps      %%xmm0, %%xmm1")            /* xmm1 = k  k  k  k  */
        __ASM_EMIT(".align 16")
        __ASM_EMIT("1:")

        /* Load data */
        __ASM_EMIT(LS_RE "      0x00(%[dst]), %%xmm2")      /* xmm2 = r0 r1 r2 r3 */
        __ASM_EMIT(LS_RE "      0x10(%[dst]), %%xmm3")      /* xmm3 = i0 i1 i2 i3 */
        __ASM_EMIT(LS_RE "      0x20(%[dst]), %%xmm5")      /* xmm5 = r4 r5 r6 r7 */
        __ASM_EMIT(LS_RE "      0x30(%[dst]), %%xmm6")      /* xmm6 = i4 i5 i6 i7 */

        /* 1st step: repack pairs */
        __ASM_EMIT("movaps      %%xmm2, %%xmm4")            /* xmm4 = r0 r1 r2 r3 */
        __ASM_EMIT("movaps      %%xmm5, %%xmm7")            /* xmm7 = r4 r5 r6 r7 */
        __ASM_EMIT("unpcklps    %%xmm3, %%xmm2")            /* xmm2 = r0 i0 r1 i1 */
        __ASM_EMIT("unpcklps    %%xmm6, %%xmm5")            /* xmm5 = r4 i4 r5 i5 */
        __ASM_EMIT("unpckhps    %%xmm3, %%xmm4")            /* xmm4 = r2 i2 r3 i3 */
        __ASM_EMIT("unpckhps    %%xmm6, %%xmm7")            /* xmm7 = r6 i6 r7 i7 */

        /* 3rd step: apply normalization */
        __ASM_EMIT("mulps       %%xmm0, %%xmm2")
        __ASM_EMIT("mulps       %%xmm1, %%xmm4")
        __ASM_EMIT("mulps       %%xmm0, %%xmm5")
        __ASM_EMIT("mulps       %%xmm1, %%xmm7")

        /* Store data */
        __ASM_EMIT(LS_RE "      %%xmm2, 0x00(%[dst])")
        __ASM_EMIT(LS_RE "      %%xmm4, 0x10(%[dst])")
        __ASM_EMIT(LS_RE "      %%xmm5, 0x20(%[dst])")
        __ASM_EMIT(LS_RE "      %%xmm7, 0x30(%[dst])")

        /* Move pointers and repeat cycle */
        __ASM_EMIT("add         $0x40, %[dst]")
        __ASM_EMIT("dec         %[blocks]")
        __ASM_EMIT("jnz         1b")

        : [dst] "+r"(dst), [blocks] "+r"(blocks), "+Yz"(k)
        :
        : "cc", "memory",
          "%xmm1", "%xmm2", "%xmm3",
          "%xmm4", "%xmm5", "%xmm6", "%xmm7"
    );
}

#undef SSE_FFT_NAME
#undef SSE_CALL_NAME
#undef _SSE_FFT_NAME
#undef _SSE_CALL_NAME

#undef FFT_SCRAMBLE_SELF_DIRECT_NAME
#undef FFT_SCRAMBLE_COPY_DIRECT_NAME
#undef FFT_SCRAMBLE_SELF_REVERSE_NAME
#undef FFT_SCRAMBLE_COPY_REVERSE_NAME
#undef LS_RE

#undef FFT_SCRAMBLE_DIRECT_NAME
#undef FFT_SCRAMBLE_REVERSE_NAME
#undef FFT_REPACK
#undef FFT_REPACK_NORMALIZED
#undef FFT_MODE
