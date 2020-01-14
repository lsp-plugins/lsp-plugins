/*
 * msmatrix.h
 *
 *  Created on: 14 янв. 2020 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_AVX_MSMATRIX_H_
#define DSP_ARCH_X86_AVX_MSMATRIX_H_

#ifndef DSP_ARCH_X86_AVX_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_AVX_IMPL */

namespace avx
{
    void lr_to_ms(float *m, float *s, const float *l, const float *r, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM(
            __ASM_EMIT("xor             %[off], %[off]")
            __ASM_EMIT("vmovaps         %[X_HALF], %%ymm7")
            // 16x blocks
            __ASM_EMIT32("subl          $16, %[count]")
            __ASM_EMIT64("sub           $16, %[count]")
            __ASM_EMIT("vmovaps         %%ymm7, %%ymm6")
            __ASM_EMIT("jb              2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vmulps          0x00(%[left], %[off]), %%ymm6, %%ymm0")     // ymm0 = l*0.5
            __ASM_EMIT("vmulps          0x20(%[left], %[off]), %%ymm7, %%ymm1")
            __ASM_EMIT("vmulps          0x00(%[right], %[off]), %%ymm6, %%ymm4")    // ymm4 = r*0.5
            __ASM_EMIT("vmulps          0x20(%[right], %[off]), %%ymm7, %%ymm5")
            __ASM_EMIT("vsubps          %%ymm4, %%ymm0, %%ymm2")                    // ymm2 = s = (l-r)*0.5
            __ASM_EMIT("vsubps          %%ymm5, %%ymm1, %%ymm3")
            __ASM_EMIT("vaddps          %%ymm4, %%ymm0, %%ymm0")                    // ymm0 = m = (l+r)*0.5
            __ASM_EMIT("vaddps          %%ymm5, %%ymm1, %%ymm1")
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[mid], %[off])")
            __ASM_EMIT("vmovups         %%ymm1, 0x20(%[mid], %[off])")
            __ASM_EMIT("vmovups         %%ymm2, 0x00(%[side], %[off])")
            __ASM_EMIT("vmovups         %%ymm3, 0x20(%[side], %[off])")
            __ASM_EMIT("add             $0x40, %[off]")
            __ASM_EMIT32("subl          $16, %[count]")
            __ASM_EMIT64("sub           $16, %[count]")
            __ASM_EMIT("jae             1b")
            __ASM_EMIT("2:")
            // 8x block
            __ASM_EMIT32("addl          $8, %[count]")
            __ASM_EMIT64("add           $8, %[count]")
            __ASM_EMIT("jl              4f")
            __ASM_EMIT("vmulps          0x00(%[left], %[off]), %%ymm6, %%ymm0")     // ymm0 = l*0.5
            __ASM_EMIT("vmulps          0x00(%[right], %[off]), %%ymm7, %%ymm4")    // ymm4 = r*0.5
            __ASM_EMIT("vsubps          %%ymm4, %%ymm0, %%ymm2")                    // ymm2 = s = (l-r)*0.5
            __ASM_EMIT("vaddps          %%ymm4, %%ymm0, %%ymm0")                    // ymm0 = m = (l+r)*0.5
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[mid], %[off])")
            __ASM_EMIT("vmovups         %%ymm2, 0x00(%[side], %[off])")
            __ASM_EMIT("add             $0x20, %[off]")
            __ASM_EMIT32("subl          $8, %[count]")
            __ASM_EMIT64("sub           $8, %[count]")
            __ASM_EMIT("4:")
            // 4x block
            __ASM_EMIT32("addl          $4, %[count]")
            __ASM_EMIT64("add           $4, %[count]")
            __ASM_EMIT("jl              6f")
            __ASM_EMIT("vmulps          0x00(%[left], %[off]), %%xmm6, %%xmm0")     // xmm0 = l*0.5
            __ASM_EMIT("vmulps          0x00(%[right], %[off]), %%xmm7, %%xmm4")    // xmm4 = r*0.5
            __ASM_EMIT("vsubps          %%xmm4, %%xmm0, %%xmm2")                    // xmm2 = s = (l-r)*0.5
            __ASM_EMIT("vaddps          %%xmm4, %%xmm0, %%xmm0")                    // xmm0 = m = (l+r)*0.5
            __ASM_EMIT("vmovups         %%xmm0, 0x00(%[mid], %[off])")
            __ASM_EMIT("vmovups         %%xmm2, 0x00(%[side], %[off])")
            __ASM_EMIT("add             $0x10, %[off]")
            __ASM_EMIT32("subl          $8, %[count]")
            __ASM_EMIT64("sub           $8, %[count]")
            __ASM_EMIT("6:")
            // 1x blocks
            __ASM_EMIT32("addl          $3, %[count]")
            __ASM_EMIT64("add           $3, %[count]")
            __ASM_EMIT("jl              8f")
            __ASM_EMIT("7:")
            __ASM_EMIT("vmulss          0x00(%[left], %[off]), %%xmm6, %%xmm0")     // xmm0 = l*0.5
            __ASM_EMIT("vmulss          0x00(%[right], %[off]), %%xmm7, %%xmm4")    // xmm4 = r*0.5
            __ASM_EMIT("vsubss          %%xmm4, %%xmm0, %%xmm2")                    // xmm2 = s = (l-r)*0.5
            __ASM_EMIT("vaddss          %%xmm4, %%xmm0, %%xmm0")                    // xmm0 = m = (l+r)*0.5
            __ASM_EMIT("vmovss          %%xmm0, 0x00(%[mid], %[off])")
            __ASM_EMIT("vmovss          %%xmm2, 0x00(%[side], %[off])")
            __ASM_EMIT("add             $0x04, %[off]")
            __ASM_EMIT32("decl          %[count]")
            __ASM_EMIT64("dec           %[count]")
            __ASM_EMIT("jge             7b")
            __ASM_EMIT("8:")

            : [off] "=&r" (off), [count] __ASM_ARG_RW(count)
            : [left] "r"(l), [right] "r" (r),
              [mid] "r" (m), [side] "r" (s),
              [X_HALF] "m" (X_HALF)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    #define LR_TO_PART(P, L, R, OP) \
            __ASM_EMIT("xor             %[off], %[off]") \
            __ASM_EMIT("vmovaps         %[X_HALF], %%ymm7") \
            /*  32x blocks */ \
            __ASM_EMIT("sub             $32, %[count]") \
            __ASM_EMIT("jb              2f") \
            __ASM_EMIT("vmovaps         %%ymm7, %%ymm6") \
            __ASM_EMIT("1:") \
            __ASM_EMIT("vmovups         0x00(%[" L "], %[off]), %%ymm0")                /*  ymm0 = l */ \
            __ASM_EMIT("vmovups         0x20(%[" L "], %[off]), %%ymm1") \
            __ASM_EMIT("vmovups         0x40(%[" L "], %[off]), %%ymm2") \
            __ASM_EMIT("vmovups         0x60(%[" L "], %[off]), %%ymm3") \
            __ASM_EMIT(OP "ps           0x00(%[" R "], %[off]), %%ymm0, %%ymm0")        /*  ymm0 = l op r */ \
            __ASM_EMIT(OP "ps           0x20(%[" R "], %[off]), %%ymm1, %%ymm1") \
            __ASM_EMIT(OP "ps           0x40(%[" R "], %[off]), %%ymm2, %%ymm2") \
            __ASM_EMIT(OP "ps           0x60(%[" R "], %[off]), %%ymm3, %%ymm3") \
            __ASM_EMIT("vmulps          %%ymm6, %%ymm0, %%ymm0")                        /*  ymm0 = (l op r) * 0.5 */ \
            __ASM_EMIT("vmulps          %%ymm7, %%ymm1, %%ymm1") \
            __ASM_EMIT("vmulps          %%ymm6, %%ymm2, %%ymm2") \
            __ASM_EMIT("vmulps          %%ymm7, %%ymm3, %%ymm3") \
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[" P "], %[off])") \
            __ASM_EMIT("vmovups         %%ymm1, 0x20(%[" P "], %[off])") \
            __ASM_EMIT("vmovups         %%ymm2, 0x40(%[" P "], %[off])") \
            __ASM_EMIT("vmovups         %%ymm3, 0x60(%[" P "], %[off])") \
            __ASM_EMIT("add             $0x80, %[off]") \
            __ASM_EMIT("sub             $32, %[count]") \
            __ASM_EMIT("jae             1b") \
            __ASM_EMIT("2:") \
            /*  16x block */ \
            __ASM_EMIT("add             $16, %[count]") \
            __ASM_EMIT("jl              4f") \
            __ASM_EMIT("vmovups         0x00(%[" L "], %[off]), %%ymm0")                /*  ymm0 = l */ \
            __ASM_EMIT("vmovups         0x20(%[" L "], %[off]), %%ymm1") \
            __ASM_EMIT(OP "ps           0x00(%[" R "], %[off]), %%ymm0, %%ymm0")        /*  ymm0 = l op r */ \
            __ASM_EMIT(OP "ps           0x20(%[" R "], %[off]), %%ymm1, %%ymm1") \
            __ASM_EMIT("vmulps          %%ymm6, %%ymm0, %%ymm0")                        /*  ymm0 = (l op r) * 0.5 */ \
            __ASM_EMIT("vmulps          %%ymm7, %%ymm1, %%ymm1") \
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[" P "], %[off])") \
            __ASM_EMIT("vmovups         %%ymm1, 0x20(%[" P "], %[off])") \
            __ASM_EMIT("sub             $16, %[count]") \
            __ASM_EMIT("add             $0x40, %[off]") \
            __ASM_EMIT("4:") \
            /*  8x block */ \
            __ASM_EMIT("add             $8, %[count]") \
            __ASM_EMIT("jl              6f") \
            __ASM_EMIT("vmovups         0x00(%[" L "], %[off]), %%ymm0")                /*  ymm0 = l */ \
            __ASM_EMIT(OP "ps           0x00(%[" R "], %[off]), %%ymm0, %%ymm0")        /*  ymm0 = l op r */ \
            __ASM_EMIT("vmulps          %%ymm6, %%ymm0, %%ymm0")                        /*  ymm0 = (l op r) * 0.5 */ \
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[" P "], %[off])") \
            __ASM_EMIT("sub             $8, %[count]") \
            __ASM_EMIT("add             $0x20, %[off]") \
            __ASM_EMIT("6:") \
            /*  4x block */ \
            __ASM_EMIT("add             $4, %[count]") \
            __ASM_EMIT("jl              8f") \
            __ASM_EMIT("vmovups         0x00(%[" L "], %[off]), %%xmm0")                /*  xmm0 = l */ \
            __ASM_EMIT(OP "ps           0x00(%[" R "], %[off]), %%xmm0, %%xmm0")        /*  xmm0 = l op r */ \
            __ASM_EMIT("vmulps          %%xmm6, %%xmm0, %%xmm0")                        /*  xmm0 = (l op r) * 0.5 */ \
            __ASM_EMIT("vmovups         %%xmm0, 0x00(%[" P "], %[off])") \
            __ASM_EMIT("sub             $4, %[count]") \
            __ASM_EMIT("add             $0x10, %[off]") \
            __ASM_EMIT("8:") \
            /*  1x blocks */ \
            __ASM_EMIT("add             $3, %[count]") \
            __ASM_EMIT("jl              10f") \
            __ASM_EMIT("9:") \
            __ASM_EMIT("vmovss          0x00(%[" L "], %[off]), %%xmm0")                /*  xmm0 = l */ \
            __ASM_EMIT(OP "ss           0x00(%[" R "], %[off]), %%xmm0, %%xmm0")        /*  xmm0 = l op r */ \
            __ASM_EMIT("vmulss          %%xmm6, %%xmm0, %%xmm0")                        /*  xmm0 = (l op r) * 0.5 */ \
            __ASM_EMIT("vmovss          %%xmm0, 0x00(%[" P "], %[off])") \
            __ASM_EMIT("add             $0x04, %[off]") \
            __ASM_EMIT("dec             %[count]") \
            __ASM_EMIT("jge             9b") \
            __ASM_EMIT("10:")

    void lr_to_mid(float *m, const float *l, const float *r, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM (
            LR_TO_PART("mid", "left", "right", "vadd")
            : [off] "=&r" (off), [count] "+r" (count)
            : [left] "r" (l), [right] "r" (r),
              [mid] "r" (m),
              [X_HALF] "m" (X_HALF)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm6", "%xmm7"
        );
    }

    void lr_to_side(float *s, const float *l, const float *r, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM (
            LR_TO_PART("side", "left", "right", "vsub")
            : [off] "=&r" (off), [count] "+r" (count)
            : [left] "r" (l), [right] "r" (r),
              [side] "r" (s),
              [X_HALF] "m" (X_HALF)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm6", "%xmm7"
        );
    }

    #undef LR_TO_PART

    void ms_to_lr(float *l, float *r, const float *m, const float *s, size_t count)
    {
        IF_ARCH_X86(size_t off);
        ARCH_X86_ASM(
            __ASM_EMIT("xor             %[off], %[off]")
            // 32x blocks
            __ASM_EMIT32("subl          $32, %[count]")
            __ASM_EMIT64("sub           $32, %[count]")
            __ASM_EMIT("jb              2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vmovups         0x00(%[mid], %[off]), %%ymm0")              // ymm0 = m
            __ASM_EMIT("vmovups         0x20(%[mid], %[off]), %%ymm1")
            __ASM_EMIT("vmovups         0x40(%[mid], %[off]), %%ymm2")
            __ASM_EMIT("vmovups         0x60(%[mid], %[off]), %%ymm3")
            __ASM_EMIT("vsubps          0x00(%[side], %[off]), %%ymm0, %%ymm4")     // ymm4 = m - s
            __ASM_EMIT("vsubps          0x20(%[side], %[off]), %%ymm1, %%ymm5")
            __ASM_EMIT("vsubps          0x40(%[side], %[off]), %%ymm2, %%ymm6")
            __ASM_EMIT("vsubps          0x60(%[side], %[off]), %%ymm3, %%ymm7")
            __ASM_EMIT("vaddps          0x00(%[side], %[off]), %%ymm0, %%ymm0")     // ymm0 = m + s
            __ASM_EMIT("vaddps          0x20(%[side], %[off]), %%ymm1, %%ymm1")
            __ASM_EMIT("vaddps          0x40(%[side], %[off]), %%ymm2, %%ymm2")
            __ASM_EMIT("vaddps          0x60(%[side], %[off]), %%ymm3, %%ymm3")
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[left], %[off])")
            __ASM_EMIT("vmovups         %%ymm1, 0x20(%[left], %[off])")
            __ASM_EMIT("vmovups         %%ymm2, 0x40(%[left], %[off])")
            __ASM_EMIT("vmovups         %%ymm3, 0x60(%[left], %[off])")
            __ASM_EMIT("vmovups         %%ymm4, 0x00(%[right], %[off])")
            __ASM_EMIT("vmovups         %%ymm5, 0x20(%[right], %[off])")
            __ASM_EMIT("vmovups         %%ymm6, 0x40(%[right], %[off])")
            __ASM_EMIT("vmovups         %%ymm7, 0x60(%[right], %[off])")
            __ASM_EMIT("add             $0x80, %[off]")
            __ASM_EMIT32("subl          $32, %[count]")
            __ASM_EMIT64("sub           $32, %[count]")
            __ASM_EMIT("jae             1b")
            __ASM_EMIT("2:")
            // 16x block
            __ASM_EMIT32("addl          $16, %[count]")
            __ASM_EMIT64("add           $16, %[count]")
            __ASM_EMIT("jl              4f")
            __ASM_EMIT("vmovups         0x00(%[mid], %[off]), %%ymm0")              // ymm0 = m
            __ASM_EMIT("vmovups         0x20(%[mid], %[off]), %%ymm1")
            __ASM_EMIT("vsubps          0x00(%[side], %[off]), %%ymm0, %%ymm4")     // ymm4 = m - s
            __ASM_EMIT("vsubps          0x20(%[side], %[off]), %%ymm1, %%ymm5")
            __ASM_EMIT("vaddps          0x00(%[side], %[off]), %%ymm0, %%ymm0")     // ymm0 = m + s
            __ASM_EMIT("vaddps          0x20(%[side], %[off]), %%ymm1, %%ymm1")
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[left], %[off])")
            __ASM_EMIT("vmovups         %%ymm1, 0x20(%[left], %[off])")
            __ASM_EMIT("vmovups         %%ymm4, 0x00(%[right], %[off])")
            __ASM_EMIT("vmovups         %%ymm5, 0x20(%[right], %[off])")
            __ASM_EMIT32("subl          $16, %[count]")
            __ASM_EMIT64("sub           $16, %[count]")
            __ASM_EMIT("add             $0x40, %[off]")
            __ASM_EMIT("4:")
            // 8x block
            __ASM_EMIT32("addl          $8, %[count]")
            __ASM_EMIT64("add           $8, %[count]")
            __ASM_EMIT("jl              6f")
            __ASM_EMIT("vmovups         0x00(%[mid], %[off]), %%ymm0")              // ymm0 = m
            __ASM_EMIT("vsubps          0x00(%[side], %[off]), %%ymm0, %%ymm4")     // ymm4 = m - s
            __ASM_EMIT("vaddps          0x00(%[side], %[off]), %%ymm0, %%ymm0")     // ymm0 = m + s
            __ASM_EMIT("vmovups         %%ymm0, 0x00(%[left], %[off])")
            __ASM_EMIT("vmovups         %%ymm4, 0x00(%[right], %[off])")
            __ASM_EMIT32("subl          $8, %[count]")
            __ASM_EMIT64("sub           $8, %[count]")
            __ASM_EMIT("add             $0x20, %[off]")
            __ASM_EMIT("6:")
            // 4x block
            __ASM_EMIT32("addl          $4, %[count]")
            __ASM_EMIT64("add           $4, %[count]")
            __ASM_EMIT("jl              8f")
            __ASM_EMIT("vmovups         0x00(%[mid], %[off]), %%xmm0")              // xmm0 = m
            __ASM_EMIT("vsubps          0x00(%[side], %[off]), %%xmm0, %%xmm4")     // xmm4 = m - s
            __ASM_EMIT("vaddps          0x00(%[side], %[off]), %%xmm0, %%xmm0")     // xmm0 = m + s
            __ASM_EMIT("vmovups         %%xmm0, 0x00(%[left], %[off])")
            __ASM_EMIT("vmovups         %%xmm4, 0x00(%[right], %[off])")
            __ASM_EMIT32("subl          $4, %[count]")
            __ASM_EMIT64("sub           $4, %[count]")
            __ASM_EMIT("add             $0x10, %[off]")
            __ASM_EMIT("8:")
            // 1x blocks
            __ASM_EMIT32("addl          $3, %[count]")
            __ASM_EMIT64("add           $3, %[count]")
            __ASM_EMIT("jl              10f")
            __ASM_EMIT("9:")
            __ASM_EMIT("vmovss          0x00(%[mid], %[off]), %%xmm0")              // xmm0 = m
            __ASM_EMIT("vsubss          0x00(%[side], %[off]), %%xmm0, %%xmm4")     // xmm4 = m - s
            __ASM_EMIT("vaddss          0x00(%[side], %[off]), %%xmm0, %%xmm0")     // xmm0 = m + s
            __ASM_EMIT("vmovss          %%xmm0, 0x00(%[left], %[off])")
            __ASM_EMIT("vmovss          %%xmm4, 0x00(%[right], %[off])")
            __ASM_EMIT("add             $0x04, %[off]")
            __ASM_EMIT32("decl          %[count]")
            __ASM_EMIT64("dec           %[count]")
            __ASM_EMIT("jge             9b")
            __ASM_EMIT("10:")

            : [off] "=&r" (off), [count] __ASM_ARG_RW(count)
            : [left] "r"(l), [right] "r" (r),
              [mid] "r" (m), [side] "r" (s)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    #define MS_TO_PART(P, M, S, OP) \
        __ASM_EMIT("xor             %[off], %[off]") \
        /*  32x blocks */ \
        __ASM_EMIT("sub             $32, %[count]") \
        __ASM_EMIT("jb              2f") \
        __ASM_EMIT("1:") \
        __ASM_EMIT("vmovups         0x00(%[" M "], %[off]), %%ymm0")            /*  ymm0 = m */ \
        __ASM_EMIT("vmovups         0x20(%[" M "], %[off]), %%ymm1") \
        __ASM_EMIT("vmovups         0x40(%[" M "], %[off]), %%ymm2") \
        __ASM_EMIT("vmovups         0x60(%[" M "], %[off]), %%ymm3") \
        __ASM_EMIT(OP "ps           0x00(%[" S "], %[off]), %%ymm0, %%ymm0")    /*  ymm4 = m - s */ \
        __ASM_EMIT(OP "ps           0x20(%[" S "], %[off]), %%ymm1, %%ymm1") \
        __ASM_EMIT(OP "ps           0x40(%[" S "], %[off]), %%ymm2, %%ymm2") \
        __ASM_EMIT(OP "ps           0x60(%[" S "], %[off]), %%ymm3, %%ymm3") \
        __ASM_EMIT("vmovups         %%ymm0, 0x00(%[" P "], %[off])") \
        __ASM_EMIT("vmovups         %%ymm1, 0x20(%[" P "], %[off])") \
        __ASM_EMIT("vmovups         %%ymm2, 0x40(%[" P "], %[off])") \
        __ASM_EMIT("vmovups         %%ymm3, 0x60(%[" P "], %[off])") \
        __ASM_EMIT("add             $0x80, %[off]") \
        __ASM_EMIT("sub             $32, %[count]") \
        __ASM_EMIT("jae             1b") \
        __ASM_EMIT("2:") \
        /*  16x block */ \
        __ASM_EMIT("add             $16, %[count]") \
        __ASM_EMIT("jl              4f") \
        __ASM_EMIT("vmovups         0x00(%[" M "], %[off]), %%ymm0")            /*  ymm0 = m */ \
        __ASM_EMIT("vmovups         0x20(%[" M "], %[off]), %%ymm1") \
        __ASM_EMIT(OP "ps           0x00(%[" S "], %[off]), %%ymm0, %%ymm0")    /*  ymm4 = m - s */ \
        __ASM_EMIT(OP "ps           0x20(%[" S "], %[off]), %%ymm1, %%ymm1") \
        __ASM_EMIT("vmovups         %%ymm0, 0x00(%[" P "], %[off])") \
        __ASM_EMIT("vmovups         %%ymm1, 0x20(%[" P "], %[off])") \
        __ASM_EMIT("sub             $16, %[count]") \
        __ASM_EMIT("add             $0x40, %[off]") \
        __ASM_EMIT("4:") \
        /*  8x block */ \
        __ASM_EMIT("add             $8, %[count]") \
        __ASM_EMIT("jl              6f") \
        __ASM_EMIT("vmovups         0x00(%[" M "], %[off]), %%ymm0")            /*  ymm0 = m */ \
        __ASM_EMIT(OP "ps           0x00(%[" S "], %[off]), %%ymm0, %%ymm0")    /*  ymm4 = m - s */ \
        __ASM_EMIT("vmovups         %%ymm0, 0x00(%[" P "], %[off])") \
        __ASM_EMIT("sub             $8, %[count]") \
        __ASM_EMIT("add             $0x20, %[off]") \
        __ASM_EMIT("6:") \
        /*  4x block */ \
        __ASM_EMIT("add             $4, %[count]") \
        __ASM_EMIT("jl              8f") \
        __ASM_EMIT("vmovups         0x00(%[" M "], %[off]), %%xmm0")            /*  xmm0 = m */ \
        __ASM_EMIT(OP "ps           0x00(%[" S "], %[off]), %%xmm0, %%xmm0")    /*  xmm4 = m - s */ \
        __ASM_EMIT("vmovups         %%xmm0, 0x00(%[" P "], %[off])") \
        __ASM_EMIT("sub             $4, %[count]") \
        __ASM_EMIT("add             $0x10, %[off]") \
        __ASM_EMIT("8:") \
        /*  1x blocks */ \
        __ASM_EMIT("add             $3, %[count]") \
        __ASM_EMIT("jl              10f") \
        __ASM_EMIT("9:") \
        __ASM_EMIT("vmovss          0x00(%[" M "], %[off]), %%xmm0")            /*  xmm0 = m */ \
        __ASM_EMIT(OP "ss           0x00(%[" S "], %[off]), %%xmm0, %%xmm0")    /*  xmm4 = m - s */ \
        __ASM_EMIT("vmovss          %%xmm0, 0x00(%[" P "], %[off])") \
        __ASM_EMIT("add             $0x04, %[off]") \
        __ASM_EMIT("dec             %[count]") \
        __ASM_EMIT("jge             9b") \
        __ASM_EMIT("10:")

    void ms_to_left(float *l, const float *m, const float *s, size_t count)
    {
        size_t off;

        ARCH_X86_ASM (
            MS_TO_PART("left", "mid", "side", "vadd")
            : [off] "=&r" (off), [count] "+r" (count)
            : [left] "r" (l),
              [mid] "r" (m), [side] "r" (s)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3"
        );
    }

    void ms_to_right(float *r, const float *m, const float *s, size_t count)
    {
        size_t off;

        ARCH_X86_ASM (
            MS_TO_PART("right", "mid", "side", "vsub")
            : [off] "=&r" (off), [count] "+r" (count)
            : [right] "r" (r),
              [mid] "r" (m), [side] "r" (s)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3"
        );
    }

    #undef MS_TO_PART
}

#endif /* DSP_ARCH_X86_AVX_MSMATRIX_H_ */
