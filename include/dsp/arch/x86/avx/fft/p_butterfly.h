/*
 * p_butterfly.h
 *
 *  Created on: 11 дек. 2019 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_AVX_FFT_P_BUTTERFLY_H_
#define DSP_ARCH_X86_AVX_FFT_P_BUTTERFLY_H_

#ifndef DSP_ARCH_X86_AVX_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_AVX_IMPL */

#define FFT_BUTTERFLY_BODY8(add_b, add_a, FMA_SEL) \
    ARCH_X86_ASM \
    ( \
        /* Prepare angle */ \
        __ASM_EMIT("vmovaps         0x00(%[fft_a]), %%ymm6")            /* ymm6 = x_re */ \
        __ASM_EMIT("vmovaps         0x20(%[fft_a]), %%ymm7")            /* ymm7 = x_im */ \
        /* Start loop */ \
        __ASM_EMIT("1:") \
            __ASM_EMIT("vmovups         0x00(%[dst], %[off1]), %%ymm0")     /* ymm0 = a_re */ \
            __ASM_EMIT("vmovups         0x20(%[dst], %[off1]), %%ymm1")     /* ymm1 = a_im */ \
            __ASM_EMIT("vmovups         0x00(%[dst], %[off2]), %%ymm2")     /* ymm2 = b_re */ \
            __ASM_EMIT("vmovups         0x20(%[dst], %[off2]), %%ymm3")     /* ymm3 = b_im */ \
            /* Calculate complex multiplication */ \
            __ASM_EMIT("vmulps          %%ymm7, %%ymm2, %%ymm4")            /* ymm4 = x_im * b_re */ \
            __ASM_EMIT("vmulps          %%ymm7, %%ymm3, %%ymm5")            /* ymm5 = x_im * b_im */ \
            __ASM_EMIT(FMA_SEL("vmulps  %%ymm6, %%ymm2, %%ymm2", ""))       /* ymm2 = x_re * b_re */ \
            __ASM_EMIT(FMA_SEL("vmulps  %%ymm6, %%ymm3, %%ymm3", ""))       /* ymm3 = x_re * b_im */ \
            __ASM_EMIT(FMA_SEL(add_b "  %%ymm5, %%ymm2, %%ymm5", add_b " %%ymm6, %%ymm2, %%ymm5")) /* ymm5 = c_re = x_re * b_re +- x_im * b_im */ \
            __ASM_EMIT(FMA_SEL(add_a "  %%ymm4, %%ymm3, %%ymm4", add_a " %%ymm6, %%ymm3, %%ymm4")) /* ymm4 = c_im = x_re * b_im -+ x_im * b_re */ \
            /* Perform butterfly */ \
            __ASM_EMIT("vsubps          %%ymm5, %%ymm0, %%ymm2")            /* ymm2 = a_re - c_re */ \
            __ASM_EMIT("vsubps          %%ymm4, %%ymm1, %%ymm3")            /* ymm3 = a_im - c_im */ \
            __ASM_EMIT("vaddps          %%ymm5, %%ymm0, %%ymm0")            /* ymm0 = a_re + c_re */ \
            __ASM_EMIT("vaddps          %%ymm4, %%ymm1, %%ymm1")            /* ymm1 = a_im + c_im */ \
            /* Store values */ \
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst], %[off1])") \
            __ASM_EMIT("vmovups         %%ymm1, 0x20(%[dst], %[off1])") \
            __ASM_EMIT("vmovups         %%ymm2, 0x00(%[dst], %[off2])") \
            __ASM_EMIT("vmovups         %%ymm3, 0x20(%[dst], %[off2])") \
            __ASM_EMIT("add             $0x40, %[off1]") \
            __ASM_EMIT("add             $0x40, %[off2]") \
            __ASM_EMIT32("subl          $8, %[np]") \
            __ASM_EMIT64("subq          $8, %[np]") \
            __ASM_EMIT("jz              2f") \
            /* Rotate angle */ \
            __ASM_EMIT("vmovaps         0x00(%[fft_w]), %%ymm4")            /* xmm4 = w_re */ \
            __ASM_EMIT("vmovaps         0x20(%[fft_w]), %%ymm5")            /* xmm5 = w_im */ \
            __ASM_EMIT("vmulps          %%ymm5, %%ymm6, %%ymm2")            /* ymm2 = w_im * x_re */ \
            __ASM_EMIT("vmulps          %%ymm5, %%ymm7, %%ymm3")            /* ymm3 = w_im * x_im */ \
            __ASM_EMIT(FMA_SEL("vmulps  %%ymm4, %%ymm6, %%ymm6", ""))       /* ymm6 = w_re * x_re */ \
            __ASM_EMIT(FMA_SEL("vmulps  %%ymm4, %%ymm7, %%ymm7", ""))       /* ymm7 = w_re * x_im */ \
            __ASM_EMIT(FMA_SEL("vsubps  %%ymm3, %%ymm6, %%ymm6", "vfmsub132ps %%ymm4, %%ymm3, %%ymm6")) /* ymm6 = x_re' = w_re * x_re - w_im * x_im */ \
            __ASM_EMIT(FMA_SEL("vaddps  %%ymm2, %%ymm7, %%ymm7", "vfmadd132ps %%ymm4, %%ymm2, %%ymm7")) /* ymm7 = x_im' = w_re * x_im + w_im * x_re */ \
            /* Repeat loop */ \
        __ASM_EMIT("jmp             1b") \
        __ASM_EMIT("2:") \
        \
        : [off1] "+r" (off1), [off2] "+r" (off2), [np] __ASM_ARG_RW(np) \
        : [dst] "r" (dst), [fft_a] "r" (fft_a), [fft_w] "r" (fft_w) \
        : "cc", "memory",  \
        "%xmm0", "%xmm1", "%xmm2", "%xmm3", \
        "%xmm4", "%xmm5", "%xmm6", "%xmm7"  \
    );

namespace avx
{
#define FMA_OFF(a, b)       a
#define FMA_ON(a, b)        b

    static inline void packed_butterfly_direct8p(float *dst, size_t rank, size_t blocks)
    {
        size_t pairs = 1 << rank;
        size_t off1 = 0, shift = 8 << rank; //1 << (rank + 3);
        const float *fft_a = &FFT_A[(rank - 2) << 4];
        const float *fft_w = &FFT_DW[(rank - 2) << 4];

        for (size_t b=0; b<blocks; ++b)
        {
            size_t off2  = off1 + shift;
            size_t np    = pairs;

            FFT_BUTTERFLY_BODY8("vaddps", "vsubps", FMA_OFF);

            off1        = off2;
        }
    }

    static inline void packed_butterfly_reverse8p(float *dst, size_t rank, size_t blocks)
    {
        size_t pairs = 1 << rank;
        size_t off1 = 0, shift = 8 << rank; // 1 << (rank + 3);
        const float *fft_a = &FFT_A[(rank - 2) << 4];
        const float *fft_w = &FFT_DW[(rank - 2) << 4];

        for (size_t b=0; b<blocks; ++b)
        {
            size_t off2  = off1 + shift;
            size_t np    = pairs;

            FFT_BUTTERFLY_BODY8("vsubps", "vaddps", FMA_OFF);

            off1        = off2;
        }
    }

    static inline void packed_butterfly_direct8p_fma3(float *dst, size_t rank, size_t blocks)
    {
        size_t pairs = 1 << rank;
        size_t off1 = 0, shift = 8 << rank; // 1 << (rank + 3);
        const float *fft_a = &FFT_A[(rank - 2) << 4];
        const float *fft_w = &FFT_DW[(rank - 2) << 4];

        for (size_t b=0; b<blocks; ++b)
        {
            size_t off2  = off1 + shift;
            size_t np    = pairs;

            FFT_BUTTERFLY_BODY8("vfmadd231ps", "vfmsub231ps", FMA_ON);

            off1        = off2;
        }
    }

    static inline void packed_butterfly_reverse8p_fma3(float *dst, size_t rank, size_t blocks)
    {
        size_t pairs = 1 << rank;
        size_t off1 = 0, shift = 8 << rank; // 1 << (rank + 3);
        const float *fft_a = &FFT_A[(rank - 2) << 4];
        const float *fft_w = &FFT_DW[(rank - 2) << 4];

        for (size_t b=0; b<blocks; ++b)
        {
            size_t off2  = off1 + shift;
            size_t np    = pairs;

            FFT_BUTTERFLY_BODY8("vfmsub231ps", "vfmadd231ps", FMA_ON);

            off1        = off2;
        }
    }

#undef FMA_OFF
#undef FMA_ON
}

#undef FFT_BUTTERFLY_BODY8


#endif /* INCLUDE_DSP_ARCH_X86_AVX_FFT_P_BUTTERFLY_H_ */
