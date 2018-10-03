/*
 * msmatrix.h
 *
 *  Created on: 9 нояб. 2016 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_SSE_MSMATRIX_H_
#define DSP_ARCH_X86_SSE_MSMATRIX_H_

#ifndef DSP_ARCH_X86_SSE_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_SSE_IMPL */

namespace sse
{
    void lr_to_ms(float *m, float *s, const float *l, const float *r, size_t count)
    {
        size_t off;

        ARCH_X86_ASM
        (
            __ASM_EMIT("xor         %[off], %[off]")
            __ASM_EMIT("movaps      %[X_HALF], %%xmm7")
            __ASM_EMIT64("sub       $8, %[count]")
            __ASM_EMIT32("subl      $8, %[count]")
            __ASM_EMIT("jb          2f")
            // 8x blocks
            __ASM_EMIT("movaps      %%xmm7, %%xmm6")
            __ASM_EMIT("1:")
            __ASM_EMIT("movups      0x00(%[left], %[off]), %%xmm0")     // xmm0 = l
            __ASM_EMIT("movups      0x10(%[left], %[off]), %%xmm3")
            __ASM_EMIT("movups      0x00(%[right], %[off]), %%xmm2")    // xmm2 = r
            __ASM_EMIT("movups      0x10(%[right], %[off]), %%xmm5")
            __ASM_EMIT("movaps      %%xmm0, %%xmm1")                    // xmm1 = l
            __ASM_EMIT("movaps      %%xmm3, %%xmm4")
            __ASM_EMIT("addps       %%xmm2, %%xmm0")                    // xmm0 = l + r
            __ASM_EMIT("addps       %%xmm5, %%xmm3")
            __ASM_EMIT("subps       %%xmm2, %%xmm1")                    // xmm1 = l - r
            __ASM_EMIT("subps       %%xmm5, %%xmm4")
            __ASM_EMIT("mulps       %%xmm6, %%xmm0")                    // xmm0 = (l + r) * 0.5f
            __ASM_EMIT("mulps       %%xmm7, %%xmm3")
            __ASM_EMIT("mulps       %%xmm6, %%xmm1")                    // xmm1 = (l - r) * 0.5f
            __ASM_EMIT("mulps       %%xmm7, %%xmm4")
            __ASM_EMIT("movups      %%xmm0, 0x00(%[mid], %[off])")
            __ASM_EMIT("movups      %%xmm3, 0x10(%[mid], %[off])")
            __ASM_EMIT("movups      %%xmm1, 0x00(%[side], %[off])")
            __ASM_EMIT("movups      %%xmm4, 0x10(%[side], %[off])")
            __ASM_EMIT("add         $0x20, %[off]")
            __ASM_EMIT64("sub       $8, %[count]")
            __ASM_EMIT32("subl      $8, %[count]")
            __ASM_EMIT("jae         1b")
            // 4x block
            __ASM_EMIT("2:")
            __ASM_EMIT64("add       $4, %[count]")
            __ASM_EMIT32("addl      $4, %[count]")
            __ASM_EMIT("jl          4f")
            __ASM_EMIT("movups      0x00(%[left], %[off]), %%xmm0")     // xmm0 = l
            __ASM_EMIT("movups      0x00(%[right], %[off]), %%xmm2")    // xmm2 = r
            __ASM_EMIT("movaps      %%xmm0, %%xmm1")                    // xmm1 = l
            __ASM_EMIT("addps       %%xmm2, %%xmm0")                    // xmm0 = l + r
            __ASM_EMIT("subps       %%xmm2, %%xmm1")                    // xmm1 = l - r
            __ASM_EMIT("mulps       %%xmm7, %%xmm0")                    // xmm0 = (l + r) * 0.5f
            __ASM_EMIT("mulps       %%xmm7, %%xmm1")                    // xmm1 = (l - r) * 0.5f
            __ASM_EMIT("movups      %%xmm0, 0x00(%[mid], %[off])")
            __ASM_EMIT("movups      %%xmm1, 0x00(%[side], %[off])")
            __ASM_EMIT64("sub       $4, %[count]")
            __ASM_EMIT32("subl      $4, %[count]")
            __ASM_EMIT("add         $0x10, %[off]")
            // 1x blocks
            __ASM_EMIT("4:")
            __ASM_EMIT64("add       $3, %[count]")
            __ASM_EMIT32("addl      $3, %[count]")
            __ASM_EMIT("jl          6f")
            __ASM_EMIT("5:")
            __ASM_EMIT("movss       0x00(%[left], %[off]), %%xmm0")     // xmm0 = l
            __ASM_EMIT("movss       0x00(%[right], %[off]), %%xmm2")    // xmm2 = r
            __ASM_EMIT("movaps      %%xmm0, %%xmm1")                    // xmm1 = l
            __ASM_EMIT("addss       %%xmm2, %%xmm0")                    // xmm0 = l + r
            __ASM_EMIT("subss       %%xmm2, %%xmm1")                    // xmm1 = l - r
            __ASM_EMIT("mulss       %%xmm7, %%xmm0")                    // xmm0 = (l + r) * 0.5f
            __ASM_EMIT("mulss       %%xmm7, %%xmm1")                    // xmm1 = (l - r) * 0.5f
            __ASM_EMIT("movss       %%xmm0, 0x00(%[mid], %[off])")
            __ASM_EMIT("movss       %%xmm1, 0x00(%[side], %[off])")
            __ASM_EMIT("add         $0x04, %[off]")
            __ASM_EMIT64("dec       %[count]")
            __ASM_EMIT32("decl      %[count]")
            __ASM_EMIT("jge         5b")
            __ASM_EMIT("6:")

            : [off] "=&r" (off),
              __IF_64([count] "+r" (count))
              __IF_32([count] "+g" (count))
            : [left] "r"(l), [right] "r" (r),
              [mid] "r" (m), [side] "r" (s),
              [X_HALF] "m" (X_HALF)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void ms_to_lr(float *l, float *r, const float *m, const float *s, size_t count)
    {
        size_t off;

        ARCH_X86_ASM
        (
            __ASM_EMIT("xor         %[off], %[off]")
            __ASM_EMIT64("sub       $8, %[count]")
            __ASM_EMIT32("subl      $8, %[count]")
            __ASM_EMIT("jb          2f")
            // 8x blocks
            __ASM_EMIT("1:")
            __ASM_EMIT("movups      0x00(%[mid], %[off]), %%xmm0")      // xmm0 = m
            __ASM_EMIT("movups      0x10(%[mid], %[off]), %%xmm4")
            __ASM_EMIT("movups      0x00(%[side], %[off]), %%xmm2")     // xmm2 = s
            __ASM_EMIT("movups      0x10(%[side], %[off]), %%xmm6")
            __ASM_EMIT("movaps      %%xmm0, %%xmm1")                    // xmm1 = m
            __ASM_EMIT("movaps      %%xmm4, %%xmm5")
            __ASM_EMIT("addps       %%xmm2, %%xmm0")                    // xmm0 = m + s
            __ASM_EMIT("addps       %%xmm6, %%xmm4")
            __ASM_EMIT("subps       %%xmm2, %%xmm1")                    // xmm1 = m - s
            __ASM_EMIT("subps       %%xmm6, %%xmm5")
            __ASM_EMIT("movups      %%xmm0, 0x00(%[left], %[off])")
            __ASM_EMIT("movups      %%xmm4, 0x10(%[left], %[off])")
            __ASM_EMIT("movups      %%xmm1, 0x00(%[right], %[off])")
            __ASM_EMIT("movups      %%xmm5, 0x10(%[right], %[off])")
            __ASM_EMIT("add         $0x20, %[off]")
            __ASM_EMIT64("sub       $8, %[count]")
            __ASM_EMIT32("subl      $8, %[count]")
            __ASM_EMIT("jae         1b")
            // 4x block
            __ASM_EMIT("2:")
            __ASM_EMIT64("add       $4, %[count]")
            __ASM_EMIT32("addl      $4, %[count]")
            __ASM_EMIT("jl          4f")
            __ASM_EMIT("movups      0x00(%[mid], %[off]), %%xmm0")      // xmm0 = m
            __ASM_EMIT("movups      0x00(%[side], %[off]), %%xmm2")     // xmm2 = s
            __ASM_EMIT("movaps      %%xmm0, %%xmm1")                    // xmm1 = m
            __ASM_EMIT("addps       %%xmm2, %%xmm0")                    // xmm0 = m + s
            __ASM_EMIT("subps       %%xmm2, %%xmm1")                    // xmm1 = m - s
            __ASM_EMIT("movups      %%xmm0, 0x00(%[left], %[off])")
            __ASM_EMIT("movups      %%xmm1, 0x00(%[right], %[off])")
            __ASM_EMIT64("sub       $4, %[count]")
            __ASM_EMIT32("subl      $4, %[count]")
            __ASM_EMIT("add         $0x10, %[off]")
            // 1x blocks
            __ASM_EMIT("4:")
            __ASM_EMIT64("add       $3, %[count]")
            __ASM_EMIT32("addl      $3, %[count]")
            __ASM_EMIT("jl          6f")
            __ASM_EMIT("5:")
            __ASM_EMIT("movss       0x00(%[mid], %[off]), %%xmm0")      // xmm0 = m
            __ASM_EMIT("movss       0x00(%[side], %[off]), %%xmm2")     // xmm2 = s
            __ASM_EMIT("movaps      %%xmm0, %%xmm1")                    // xmm1 = m
            __ASM_EMIT("addss       %%xmm2, %%xmm0")                    // xmm0 = m + s
            __ASM_EMIT("subss       %%xmm2, %%xmm1")                    // xmm1 = m - s
            __ASM_EMIT("movss       %%xmm0, 0x00(%[left], %[off])")
            __ASM_EMIT("movss       %%xmm1, 0x00(%[right], %[off])")
            __ASM_EMIT("add         $0x04, %[off]")
            __ASM_EMIT64("dec       %[count]")
            __ASM_EMIT32("decl      %[count]")
            __ASM_EMIT("jge         5b")
            __ASM_EMIT("6:")

            : [off] "=&r" (off),
              __IF_64([count] "+r" (count))
              __IF_32([count] "+g" (count))
            : [left] "r"(l), [right] "r" (r),
              [mid] "r" (m), [side] "r" (s)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    #define LR_CVT_BODY(d, l, r, op) \
        __ASM_EMIT("xor         %[off], %[off]") \
        __ASM_EMIT("sub         $16, %[count]") \
        __ASM_EMIT("movaps      %[X_HALF], %%xmm7") \
        __ASM_EMIT("jb          2f") \
        \
        /* x12 blocks */ \
        __ASM_EMIT("movaps      %%xmm7, %%xmm6") \
        __ASM_EMIT("1:") \
        __ASM_EMIT("movups      0x00(%[" l "], %[off]), %%xmm0") \
        __ASM_EMIT("movups      0x10(%[" l "], %[off]), %%xmm1") \
        __ASM_EMIT("movups      0x20(%[" l "], %[off]), %%xmm2") \
        __ASM_EMIT("movups      0x00(%[" r "], %[off]), %%xmm3") \
        __ASM_EMIT("movups      0x10(%[" r "], %[off]), %%xmm4") \
        __ASM_EMIT("movups      0x20(%[" r "], %[off]), %%xmm5") \
        __ASM_EMIT(op "ps       %%xmm3, %%xmm0")            /* xmm0 = l <+-> r */ \
        __ASM_EMIT(op "ps       %%xmm4, %%xmm1") \
        __ASM_EMIT(op "ps       %%xmm5, %%xmm2") \
        __ASM_EMIT("mulps       %%xmm7, %%xmm0")            /* xmm0 = (l <+-> r) * 0.5f */ \
        __ASM_EMIT("mulps       %%xmm6, %%xmm1") \
        __ASM_EMIT("mulps       %%xmm7, %%xmm2") \
        __ASM_EMIT("movups      %%xmm0, 0x00(%[" d "], %[off])")      \
        __ASM_EMIT("movups      %%xmm1, 0x10(%[" d "], %[off])")      \
        __ASM_EMIT("movups      %%xmm2, 0x20(%[" d "], %[off])")      \
        \
        __ASM_EMIT("add         $0x30, %[off]") \
        __ASM_EMIT("sub         $12, %[count]") \
        __ASM_EMIT("jae         1b") \
        /* x8 block */ \
        __ASM_EMIT("2:") \
        __ASM_EMIT("add         $4, %[count]") \
        __ASM_EMIT("jl          4f") \
        __ASM_EMIT("movups      0x00(%[" l "], %[off]), %%xmm0") \
        __ASM_EMIT("movups      0x10(%[" l "], %[off]), %%xmm1") \
        __ASM_EMIT("movups      0x00(%[" r "], %[off]), %%xmm3") \
        __ASM_EMIT("movups      0x10(%[" r "], %[off]), %%xmm4") \
        __ASM_EMIT(op "ps       %%xmm3, %%xmm0")            /* xmm0 = l <+-> r */ \
        __ASM_EMIT(op "ps       %%xmm4, %%xmm1") \
        __ASM_EMIT("mulps       %%xmm7, %%xmm0")            /* xmm0 = (l <+-> r) * 0.5f */ \
        __ASM_EMIT("mulps       %%xmm7, %%xmm1") \
        __ASM_EMIT("movups      %%xmm0, 0x00(%[" d "], %[off])")      \
        __ASM_EMIT("movups      %%xmm1, 0x10(%[" d "], %[off])")      \
        __ASM_EMIT("sub         $8, %[count]") \
        __ASM_EMIT("add         $0x20, %[off]") \
        /* x4 block */ \
        __ASM_EMIT("4:") \
        __ASM_EMIT("add         $4, %[count]") \
        __ASM_EMIT("jl          6f") \
        __ASM_EMIT("movups      0x00(%[" l "], %[off]), %%xmm0") \
        __ASM_EMIT("movups      0x00(%[" r "], %[off]), %%xmm3") \
        __ASM_EMIT(op "ps       %%xmm3, %%xmm0")            /* xmm0 = l <+-> r */ \
        __ASM_EMIT("mulps       %%xmm7, %%xmm0")            /* xmm0 = (l <+-> r) * 0.5f */ \
        __ASM_EMIT("movups      %%xmm0, 0x00(%[" d "], %[off])")      \
        __ASM_EMIT("sub         $4, %[count]") \
        __ASM_EMIT("add         $0x10, %[off]") \
        /* x1 blocks */ \
        __ASM_EMIT("6:") \
        __ASM_EMIT("add         $3, %[count]") \
        __ASM_EMIT("jl          8f") \
        __ASM_EMIT("7:") \
        __ASM_EMIT("movss       0x00(%[" l "], %[off]), %%xmm0") \
        __ASM_EMIT("movss       0x00(%[" r "], %[off]), %%xmm3") \
        __ASM_EMIT(op "ss       %%xmm3, %%xmm0")            /* xmm0 = l <+-> r */ \
        __ASM_EMIT("mulss       %%xmm7, %%xmm0")            /* xmm0 = (l <+-> r) * 0.5f */ \
        __ASM_EMIT("movss       %%xmm0, 0x00(%[" d "], %[off])")      \
        __ASM_EMIT("add         $0x04, %[off]") \
        __ASM_EMIT("dec         %[count]") \
        __ASM_EMIT("jge         7b") \
        \
        __ASM_EMIT("8:")

    void lr_to_mid(float *m, const float *l, const float *r, size_t count)
    {
        size_t off;

        ARCH_X86_ASM
        (
            LR_CVT_BODY("mid", "left", "right", "add")
            : [off] "=&r" (off), [count] "+r" (count)
            : [left] "r"(l), [right] "r" (r),
              [mid] "r" (m),
              [X_HALF] "m" (X_HALF)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void lr_to_side(float *s, const float *l, const float *r, size_t count)
    {
        size_t off;

        ARCH_X86_ASM
        (
            LR_CVT_BODY("side", "left", "right", "sub")
            : [off] "=&r" (off), [count] "+r" (count)
            : [left] "r"(l), [right] "r" (r),
              [side] "r" (s),
              [X_HALF] "m" (X_HALF)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    #undef LR_CVT_BODY

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
        ARCH_X86_ASM \
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

    void ms_to_left(float *l, const float *m, const float *s, size_t count)
    {
        MS_CVT_BODY(l, "add");
    }

    void ms_to_right(float *r, const float *m, const float *s, size_t count)
    {
        MS_CVT_BODY(r, "sub");
    }

    #undef MS_CVT_BODY
    #undef __MS_CVT_BODY
}

#endif /* DSP_ARCH_X86_SSE_MSMATRIX_H_ */
