/*
 * p_butterfly.h
 *
 *  Created on: 15 февр. 2017 г.
 *      Author: sadko
 */

#define FFT_BUTTERFLY_BODY(add_b, add_a) \
    /* Init pointers */ \
    register float *a       = &dst[blk*pairs*2]; \
    register float *b       = &a[pairs]; \
    register size_t p       = pairs; \
    __IF_32(size_t tmp1); \
    \
    ARCH_X86_ASM \
    ( \
        /* Prepare angle */ \
        __ASM_EMIT32("mov       %[a], %[tmp1]") \
        __ASM_EMIT32("mov       %[XFFT_A], %[a]") \
        __ASM_EMIT32("movaps    0x00(%[a], %[rank]), %%xmm6")        /* xmm6 = angle_re[0..3] */ \
        __ASM_EMIT32("movaps    0x10(%[a], %[rank]), %%xmm7")        /* xmm7 = angle_im[0..3] */ \
        __ASM_EMIT32("mov       %[tmp1], %[a]") \
        __ASM_EMIT64("movaps    0x00(%[XFFT_A], %[rank]), %%xmm6")   /* xmm6 = angle_re[0..3] */ \
        __ASM_EMIT64("movaps    0x10(%[XFFT_A], %[rank]), %%xmm7")   /* xmm7 = angle_im[0..3] */ \
        /* Start loop */ \
        __ASM_EMIT(".align 16") \
        __ASM_EMIT("1:") \
        \
        /* Load complex values */ \
        /* predicate: xmm6 = w_re[0..3] */ \
        /* predicate: xmm7 = w_im[0..3] */ \
        __ASM_EMIT("movups      0x00(%[a]), %%xmm0")   /* xmm0 = a_re[0..3] */ \
        __ASM_EMIT("movups      0x10(%[a]), %%xmm1")   /* xmm1 = a_im[0..3] */ \
        __ASM_EMIT("movups      0x00(%[b]), %%xmm2")   /* xmm2 = b_re[0..3] */ \
        __ASM_EMIT("movups      0x10(%[b]), %%xmm3")   /* xmm3 = b_im[0..3] */ \
        \
        /* Calculate complex multiplication */ \
        __ASM_EMIT("movaps      %%xmm2, %%xmm4") /* xmm4 = b_re[0..3] */ \
        __ASM_EMIT("movaps      %%xmm3, %%xmm5") /* xmm5 = b_im[0..3] */ \
        __ASM_EMIT("mulps       %%xmm6, %%xmm2") /* xmm2 = w_re[0..3] * b_re[0..3] */ \
        __ASM_EMIT("mulps       %%xmm7, %%xmm4") /* xmm4 = w_im[0..3] * b_re[0..3] */ \
        __ASM_EMIT("mulps       %%xmm6, %%xmm3") /* xmm3 = w_re[0..3] * b_im[0..3] */ \
        __ASM_EMIT("mulps       %%xmm7, %%xmm5") /* xmm5 = w_im[0..3] * b_im[0..3] */ \
        __ASM_EMIT(add_a "      %%xmm4, %%xmm3") /* xmm3 = c_im[0..3] = w_re[0..3] * b_im[0..3] -+ w_im[0..3] * b_re[0..3] */ \
        __ASM_EMIT(add_b "      %%xmm5, %%xmm2") /* xmm2 = c_re[0..3] = w_re[0..3] * b_re[0..3] +- w_im[0..3] * b_im[0..3] */ \
        \
        /* Perform butterfly */ \
        __ASM_EMIT("movaps      %%xmm0, %%xmm4") /* xmm4 = a_re[0..3] */ \
        __ASM_EMIT("movaps      %%xmm1, %%xmm5") /* xmm5 = a_im[0..3] */ \
        __ASM_EMIT("subps       %%xmm2, %%xmm0") /* xmm0 = a_re[0..3] - c_re[0..3] */ \
        __ASM_EMIT("subps       %%xmm3, %%xmm1") /* xmm1 = a_im[0..3] - c_im[0..3] */ \
        __ASM_EMIT("addps       %%xmm4, %%xmm2") /* xmm2 = a_re[0..3] + c_re[0..3] */ \
        __ASM_EMIT("addps       %%xmm5, %%xmm3") /* xmm3 = a_im[0..3] + c_im[0..3] */ \
        \
        /* Store values */ \
        __ASM_EMIT("movups      %%xmm2, 0x00(%[a])") \
        __ASM_EMIT("movups      %%xmm3, 0x10(%[a])") \
        __ASM_EMIT("movups      %%xmm0, 0x00(%[b])") \
        __ASM_EMIT("movups      %%xmm1, 0x10(%[b])") \
        \
        /* Update pointers */ \
        __ASM_EMIT("add         $0x20, %[a]") \
        __ASM_EMIT("add         $0x20, %[b]") \
        \
        /* Repeat loop */ \
        __ASM_EMIT("sub         $8, %[p]") \
        __ASM_EMIT("jz          2f") \
        \
        /* Rotate angle */ \
        __ASM_EMIT("movaps      0x00(%[XFFT_W], %[rank]), %%xmm0")      /* xmm0 = w_re[0..3] */ \
        __ASM_EMIT("movaps      0x10(%[XFFT_W], %[rank]), %%xmm1")      /* xmm1 = w_im[0..3] */ \
        \
        __ASM_EMIT("movaps      %%xmm0, %%xmm2")                        /* xmm2 = w_re[0..3] */ \
        __ASM_EMIT("movaps      %%xmm1, %%xmm3")                        /* xmm3 = w_im[0..3] */ \
        \
        __ASM_EMIT("mulps       %%xmm6, %%xmm3")                        /* xmm3 = a_re[0..3] * w_im[0..3] */ \
        __ASM_EMIT("mulps       %%xmm7, %%xmm1")                        /* xmm1 = a_im[0..3] * w_im[0..3] */ \
        __ASM_EMIT("mulps       %%xmm0, %%xmm6")                        /* xmm6 = a_re[0..3] * w_re[0..3] */ \
        __ASM_EMIT("mulps       %%xmm2, %%xmm7")                        /* xmm7 = a_im[0..3] * w_re[0..3] */ \
        __ASM_EMIT("subps       %%xmm1, %%xmm6")                        /* xmm6 = a_re[0..3] * w_re[0..3] + a_im[0..3] * w_im[0..3] */ \
        __ASM_EMIT("addps       %%xmm3, %%xmm7")                        /* xmm7 = a_im[0..3] * w_re[0..3] - a_re[0..3] * w_im[0..3] */ \
        \
        /* Repeat loop */ \
        __ASM_EMIT("jmp         1b") \
        __ASM_EMIT("2:") \
        \
        : [a] "+r"(a), [b] "+r"(b), [p] "+r"(p) \
        : [rank] "r" (rank), [XFFT_W] "r"(XFFT_W), \
          __IF_64([XFFT_A] "r"(XFFT_A)) \
          __IF_32([XFFT_A] "g"(XFFT_A), [tmp1] "g"(&tmp1)) \
        : "cc", "memory",  \
        "%xmm0", "%xmm1", "%xmm2", "%xmm3", \
        "%xmm4", "%xmm5", "%xmm6", "%xmm7" \
    );

namespace sse
{
    static inline void packed_butterfly_direct(float *dst, size_t rank, size_t blocks)
    {
        size_t pairs = 1 << (rank + 1);
        rank = (rank - 2) << 5;

        for (size_t blk=0; blk<blocks; ++blk)
        {
            FFT_BUTTERFLY_BODY("addps", "subps");
        }
    }

    static inline void packed_butterfly_reverse(float *dst, size_t rank, size_t blocks)
    {
        size_t pairs = 1 << (rank + 1);
        rank = (rank - 2) << 5;

        for (size_t blk=0; blk<blocks; ++blk)
        {
            FFT_BUTTERFLY_BODY("subps", "addps");
        }
    }
}

#undef FFT_BUTTERFLY_BODY
