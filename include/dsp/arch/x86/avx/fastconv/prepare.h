/*
 * prepare.h
 *
 *  Created on: 13 дек. 2019 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_AVX_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_AVX_IMPL */

#define FASTCONV_DIRECT_PREPARE_BODY(FMA_SEL) \
        size_t off; \
        \
        ARCH_X86_ASM( \
            __ASM_EMIT("lea                 (,%[np], 8), %[off]") \
            __ASM_EMIT("vmovups             0x00(%[ak]), %%ymm6")               /* ymm6 = x_re */ \
            __ASM_EMIT("vmovups             0x20(%[ak]), %%ymm7")               /* ymm7 = x_im */ \
            __ASM_EMIT("vmovups             0x00(%[wk]), %%ymm4")               /* ymm4 = w_re */ \
            __ASM_EMIT("vmovups             0x20(%[wk]), %%ymm5")               /* ymm5 = w_im */ \
            __ASM_EMIT("vxorps              %%ymm1, %%ymm1, %%ymm1")            /* ymm1 = a_im = 0 */ \
            /* x8 blocks */ \
            __ASM_EMIT32("subl              $8, %[np]") \
            __ASM_EMIT64("sub               $8, %[np]") \
            __ASM_EMIT64("jb                2f") \
            __ASM_EMIT("1:") \
            __ASM_EMIT("vmovups             0x00(%[src]), %%ymm0")              /* ymm0 = a_re = re */ \
            __ASM_EMIT("vmulps              %%ymm0, %%ymm7, %%ymm3")            /* ymm3 = x_im * re */ \
            __ASM_EMIT("vmulps              %%ymm0, %%ymm6, %%ymm2")            /* ymm2 = b_re = x_re * re */ \
            __ASM_EMIT("vsubps              %%ymm3, %%ymm1, %%ymm3")            /* ymm3 = b_im = -x_im * re */ \
            __ASM_EMIT("vmovups             %%ymm0, 0x00(%[dst])") \
            __ASM_EMIT("vmovups             %%ymm1, 0x20(%[dst])") \
            __ASM_EMIT("vmovups             %%ymm2, 0x00(%[dst], %[off])") \
            __ASM_EMIT("vmovups             %%ymm3, 0x20(%[dst], %[off])") \
            __ASM_EMIT("add                 $0x20, %[src]") \
            __ASM_EMIT("add                 $0x40, %[dst]") \
            __ASM_EMIT32("subl              $8, %[np]") \
            __ASM_EMIT64("sub               $8, %[np]") \
            __ASM_EMIT("jb                  2f") \
            /* Rotate angle */ \
            __ASM_EMIT("vmulps              %%ymm5, %%ymm6, %%ymm2")            /* ymm2 = w_im * x_re */ \
            __ASM_EMIT("vmulps              %%ymm5, %%ymm7, %%ymm3")            /* ymm3 = w_im * x_im */ \
            __ASM_EMIT(FMA_SEL("vmulps      %%ymm4, %%ymm6, %%ymm6", ""))       /* ymm6 = w_re * x_re */ \
            __ASM_EMIT(FMA_SEL("vmulps      %%ymm4, %%ymm7, %%ymm7", ""))       /* ymm7 = w_re * x_im */ \
            __ASM_EMIT(FMA_SEL("vsubps      %%ymm3, %%ymm6, %%ymm6", "vfmsub132ps %%ymm4, %%ymm3, %%ymm6")) /* ymm6 = x_re' = w_re * x_re - w_im * x_im */ \
            __ASM_EMIT(FMA_SEL("vaddps      %%ymm2, %%ymm7, %%ymm7", "vfmadd132ps %%ymm4, %%ymm2, %%ymm7")) /* ymm7 = x_im' = w_re * x_im + w_im * x_re */ \
            __ASM_EMIT("jmp                 1b") \
            __ASM_EMIT("2:") \
            : [dst] "+r" (dst), [src] "+r" (src), \
              [off] "=&r" (off), [np] __ASM_ARG_RW(np) \
            : [ak] "r" (ak), [wk] "r" (wk) \
            : "%xmm0", "%xmm1", "%xmm2", "%xmm3", \
              "%xmm4", "%xmm5", "%xmm6", "%xmm7" \
        );

namespace avx
{
#define FMA_OFF(a, b)       a
#define FMA_ON(a, b)        b

    static inline void fastconv_prepare(float *dst, const float *src, const float *ak, const float *wk, size_t np)
    {
        FASTCONV_DIRECT_PREPARE_BODY(FMA_OFF)
    }

    static inline void fastconv_prepare_fma3(float *dst, const float *src, const float *ak, const float *wk, size_t np)
    {
        FASTCONV_DIRECT_PREPARE_BODY(FMA_OFF)
    }

    static inline void fastconv_unpack(float *dst, const float *src, size_t np)
    {
        ARCH_X86_ASM(
            __ASM_EMIT("vmovups         (%[src]), %%xmm0")
            __ASM_EMIT("vxorps          %%ymm1, %%ymm1, %%ymm1")
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
            __ASM_EMIT("vmovups         %%ymm1, 0x20(%[dst])")
            :
            : [dst] "r" (dst), [src] "r" (src), [np] "r" (np)
            : "%xmm0", "%xmm1"
        );
    }

#undef FMA_OFF
#undef FMA_ON
}


