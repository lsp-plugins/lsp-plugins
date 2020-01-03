/*
 * transfer.h
 *
 *  Created on: 2 янв. 2020 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_SSE_FILTERS_TRANSFER_H_
#define DSP_ARCH_X86_SSE_FILTERS_TRANSFER_H_

#ifndef DSP_ARCH_X86_SSE_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_SSE_IMPL */

namespace sse
{
    #define F_UNPACK \
        __ASM_EMIT("movups      0x00(%[c]), %%xmm0")        /* x0   = t0 t1 t2 t3 */ \
        __ASM_EMIT("movups      0x10(%[c]), %%xmm4")        /* x4   = b0 b1 b2 b3 */ \
        __ASM_EMIT("movaps      %%xmm0, %%xmm2")            /* x3   = t0 t1 t2 t3 */ \
        __ASM_EMIT("movaps      %%xmm4, %%xmm6")            /* x6   = b0 b1 b2 b3 */ \
        __ASM_EMIT("unpcklps    %%xmm0, %%xmm0")            /* x0   = t0 t0 t1 t1 */ \
        __ASM_EMIT("unpcklps    %%xmm4, %%xmm4")            /* x4   = b0 b0 b1 b1 */ \
        __ASM_EMIT("unpckhps    %%xmm2, %%xmm2")            /* x2   = t2 t2 t3 t3 */ \
        __ASM_EMIT("unpckhps    %%xmm6, %%xmm6")            /* x6   = b2 b2 b3 b3 */ \
        __ASM_EMIT("movaps      %%xmm0, %%xmm1")            /* x1   = t0 t1 t2 t3 */ \
        __ASM_EMIT("movaps      %%xmm4, %%xmm5")            /* x5   = b0 b1 b2 b3 */ \
        __ASM_EMIT("unpcklps    %%xmm0, %%xmm0")            /* x0   = t0 t0 t0 t0 */ \
        __ASM_EMIT("unpcklps    %%xmm4, %%xmm4")            /* x4   = b0 b0 b0 b0 */ \
        __ASM_EMIT("unpckhps    %%xmm1, %%xmm1")            /* x1   = t1 t1 t1 t1 */ \
        __ASM_EMIT("unpckhps    %%xmm5, %%xmm5")            /* x5   = b1 b1 b1 b1 */ \
        __ASM_EMIT("unpcklps    %%xmm2, %%xmm2")            /* x2   = t2 t2 t2 t2 */ \
        __ASM_EMIT("unpcklps    %%xmm6, %%xmm6")            /* x6   = b2 b2 b2 b2 */ \
        __ASM_EMIT("movaps      %%xmm0, 0x00 + %[fp]")      /* x0   = t0 */ \
        __ASM_EMIT("movaps      %%xmm1, 0x10 + %[fp]")      /* x1   = t1 */ \
        __ASM_EMIT("movaps      %%xmm2, 0x20 + %[fp]")      /* x2   = t2 */ \
        __ASM_EMIT("movaps      %%xmm4, 0x30 + %[fp]")      /* x4   = b0 */ \
        __ASM_EMIT("movaps      %%xmm5, 0x40 + %[fp]")      /* x5   = b1 */ \
        __ASM_EMIT("movaps      %%xmm6, 0x50 + %[fp]")      /* x6   = b2 */

    #define F_LOAD \
        __ASM_EMIT("movaps      0x00 + %[fp], %%xmm0")      /* x0   = t0 */ \
        __ASM_EMIT("movaps      0x10 + %[fp], %%xmm1")      /* x1   = t1 */ \
        __ASM_EMIT("movaps      0x20 + %[fp], %%xmm2")      /* x2   = t2 */ \
        __ASM_EMIT("movaps      0x30 + %[fp], %%xmm4")      /* x4   = b0 */ \
        __ASM_EMIT("movaps      0x40 + %[fp], %%xmm5")      /* x5   = b1 */ \
        __ASM_EMIT("movaps      0x50 + %[fp], %%xmm6")      /* x6   = b2 */

    #define HF_CORE \
        /* Compute H[f] */ \
        __ASM_EMIT("movaps      %%xmm3, %%xmm7")            /* x7   = f */ \
        __ASM_EMIT("mulps       %%xmm3, %%xmm1")            /* x1   = t_im = t1 * f */ \
        __ASM_EMIT("mulps       %%xmm7, %%xmm7")            /* x7   = f2 = f * f */ \
        __ASM_EMIT("mulps       %%xmm3, %%xmm5")            /* x5   = b_im = b1 * f */ \
        __ASM_EMIT("mulps       %%xmm7, %%xmm2")            /* x2   = t2 * f2 */ \
        __ASM_EMIT("mulps       %%xmm7, %%xmm6")            /* x6   = b2 * f2 */ \
        __ASM_EMIT("subps       %%xmm2, %%xmm0")            /* x0   = t_re = t0 - t2*f2 */ \
        __ASM_EMIT("subps       %%xmm6, %%xmm4")            /* x4   = b_re = b0 - b2*f2 */ \
        __ASM_EMIT("movaps      %%xmm5, %%xmm3")            /* x3   = b_im */ \
        __ASM_EMIT("movaps      %%xmm4, %%xmm2")            /* x2   = b_re */ \
        __ASM_EMIT("mulps       %%xmm3, %%xmm3")            /* x3   = b_im * b_im */ \
        __ASM_EMIT("mulps       %%xmm2, %%xmm2")            /* x2   = b_re * b_re */ \
        __ASM_EMIT("addps       %%xmm2, %%xmm3")            /* x3   = W = b_re * b_re + b_im * b_im */ \
        __ASM_EMIT("movaps      %%xmm0, %%xmm6")            /* x6   = t_re */ \
        __ASM_EMIT("movaps      %%xmm1, %%xmm7")            /* x7   = t_im */ \
        __ASM_EMIT("mulps       %%xmm4, %%xmm0")            /* x0   = t_re * b_re */ \
        __ASM_EMIT("mulps       %%xmm5, %%xmm7")            /* x7   = t_im * b_im */ \
        __ASM_EMIT("mulps       %%xmm4, %%xmm1")            /* x1   = t_im * b_re */ \
        __ASM_EMIT("mulps       %%xmm5, %%xmm6")            /* x6   = t_re * b_im */ \
        __ASM_EMIT("addps       %%xmm7, %%xmm0")            /* x0   = t_re * b_re + t_im * b_im */ \
        __ASM_EMIT("subps       %%xmm6, %%xmm1")            /* x1   = t_im * b_re - t_re * b_im */ \
        __ASM_EMIT("divps       %%xmm3, %%xmm0")            /* x0   = a_re = (t_re * b_re + t_im * b_im) / W */ \
        __ASM_EMIT("divps       %%xmm3, %%xmm1")            /* x1   = a_im = (t_im * b_re - t_re * b_im) / W */

    #define HF_APPLY \
        /* Compute dst = H[f] * dst */ \
        __ASM_EMIT("movaps      %%xmm0, %%xmm4")            /* x4   = a_re */ \
        __ASM_EMIT("movaps      %%xmm1, %%xmm5")            /* x5   = a_im */ \
        __ASM_EMIT("mulps       %%xmm2, %%xmm0")            /* x0   = a_re * b_re */ \
        __ASM_EMIT("mulps       %%xmm3, %%xmm4")            /* x4   = a_re * b_im */ \
        __ASM_EMIT("mulps       %%xmm2, %%xmm1")            /* x1   = a_im * b_re */ \
        __ASM_EMIT("mulps       %%xmm3, %%xmm5")            /* x5   = a_im * b_im */ \
        __ASM_EMIT("addps       %%xmm4, %%xmm1")            /* x1   = a_re * b_im + a_im * b_re */ \
        __ASM_EMIT("subps       %%xmm5, %%xmm0")            /* x0   = a_re * b_re - a_im * b_im */

    void filter_transfer_calc_ri(float *re, float *im, const f_cascade_t *c, const float *freq, size_t count)
    {
        IF_ARCH_X86( float fp[6*4] __lsp_aligned16; );

        ARCH_X86_ASM(
            // Unpack filter params
            F_UNPACK
            // x4 blocks
            __ASM_EMIT("sub         $4, %[count]")
            __ASM_EMIT("jb          2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("movups      0x00(%[f]), %%xmm3")        // x3   = f
            HF_CORE
            __ASM_EMIT("movups      %%xmm0, 0x00(%[re])")
            __ASM_EMIT("movups      %%xmm1, 0x00(%[im])")
            F_LOAD
            __ASM_EMIT("add         $0x10, %[f]")
            __ASM_EMIT("add         $0x10, %[re]")
            __ASM_EMIT("add         $0x10, %[im]")
            __ASM_EMIT("sub         $4, %[count]")
            __ASM_EMIT("jae         1b")
            __ASM_EMIT("2:")
            // x2 block
            __ASM_EMIT("add         $2, %[count]")
            __ASM_EMIT("jl          4f")
            __ASM_EMIT("movlps      0x00(%[f]), %%xmm3")        // x3   = f
            HF_CORE
            __ASM_EMIT("movlps      %%xmm0, 0x00(%[re])")
            __ASM_EMIT("movlps      %%xmm1, 0x00(%[im])")
            F_LOAD
            __ASM_EMIT("sub         $2, %[count]")
            __ASM_EMIT("add         $0x08, %[f]")
            __ASM_EMIT("add         $0x08, %[re]")
            __ASM_EMIT("add         $0x08, %[im]")
            __ASM_EMIT("4:")
            // x1 block
            __ASM_EMIT("add         $1, %[count]")
            __ASM_EMIT("jl          6f")
            __ASM_EMIT("movss       0x00(%[f]), %%xmm3")        // x3   = f
            HF_CORE
            __ASM_EMIT("movss       %%xmm0, 0x00(%[re])")
            __ASM_EMIT("movss       %%xmm1, 0x00(%[im])")
            __ASM_EMIT("6:")

            : [re] "+r" (re), [im] "+r" (im), [f] "+r" (freq), [count] "+r" (count)
            : [c] "r" (c),
              [fp] "o" (fp)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void filter_transfer_apply_ri(float *re, float *im, const f_cascade_t *c, const float *freq, size_t count)
    {
        IF_ARCH_X86( float fp[6*4] __lsp_aligned16; );

        ARCH_X86_ASM(
            // Unpack filter params
            F_UNPACK
            // x4 blocks
            __ASM_EMIT("sub         $4, %[count]")
            __ASM_EMIT("jb          2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("movups      0x00(%[f]), %%xmm3")        // x3   = f
            HF_CORE
            __ASM_EMIT("movups      0x00(%[re]), %%xmm2")       // x2   = b_re
            __ASM_EMIT("movups      0x00(%[im]), %%xmm3")       // x3   = b_im
            HF_APPLY
            __ASM_EMIT("movups      %%xmm0, 0x00(%[re])")
            __ASM_EMIT("movups      %%xmm1, 0x00(%[im])")
            F_LOAD
            __ASM_EMIT("add         $0x10, %[f]")
            __ASM_EMIT("add         $0x10, %[re]")
            __ASM_EMIT("add         $0x10, %[im]")
            __ASM_EMIT("sub         $4, %[count]")
            __ASM_EMIT("jae         1b")
            __ASM_EMIT("2:")
            // x2 block
            __ASM_EMIT("add         $2, %[count]")
            __ASM_EMIT("jl          4f")
            __ASM_EMIT("movlps      0x00(%[f]), %%xmm3")        // x3   = f
            HF_CORE
            __ASM_EMIT("movlps      0x00(%[re]), %%xmm2")       // x2   = b_re
            __ASM_EMIT("movlps      0x00(%[im]), %%xmm3")       // x3   = b_im
            HF_APPLY
            __ASM_EMIT("movlps      %%xmm0, 0x00(%[re])")
            __ASM_EMIT("movlps      %%xmm1, 0x00(%[im])")
            F_LOAD
            __ASM_EMIT("sub         $2, %[count]")
            __ASM_EMIT("add         $0x08, %[f]")
            __ASM_EMIT("add         $0x08, %[re]")
            __ASM_EMIT("add         $0x08, %[im]")
            __ASM_EMIT("4:")
            // x1 block
            __ASM_EMIT("add         $1, %[count]")
            __ASM_EMIT("jl          6f")
            __ASM_EMIT("movss       0x00(%[f]), %%xmm3")        // x3   = f
            HF_CORE
            __ASM_EMIT("movss       0x00(%[re]), %%xmm2")       // x2   = b_re
            __ASM_EMIT("movss       0x00(%[im]), %%xmm3")       // x3   = b_im
            HF_APPLY
            __ASM_EMIT("movss       %%xmm0, 0x00(%[re])")
            __ASM_EMIT("movss       %%xmm1, 0x00(%[im])")
            __ASM_EMIT("6:")

            : [re] "+r" (re), [im] "+r" (im), [f] "+r" (freq), [count] "+r" (count)
            : [c] "r" (c),
              [fp] "o" (fp)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    #undef HF_CORE
    #undef HF_APPLY

    #define PHF_CORE \
        /* Compute H[f] */ \
        __ASM_EMIT("movaps      %%xmm3, %%xmm7")            /* x7   = f */ \
        __ASM_EMIT("mulps       %%xmm3, %%xmm1")            /* x1   = t_im = t1 * f */ \
        __ASM_EMIT("mulps       %%xmm7, %%xmm7")            /* x7   = f2 = f * f */ \
        __ASM_EMIT("mulps       %%xmm3, %%xmm5")            /* x5   = b_im = b1 * f */ \
        __ASM_EMIT("mulps       %%xmm7, %%xmm2")            /* x2   = t2 * f2 */ \
        __ASM_EMIT("mulps       %%xmm7, %%xmm6")            /* x6   = b2 * f2 */ \
        __ASM_EMIT("subps       %%xmm2, %%xmm0")            /* x0   = t_re = t0 - t2*f2 */ \
        __ASM_EMIT("subps       %%xmm6, %%xmm4")            /* x4   = b_re = b0 - b2*f2 */ \
        __ASM_EMIT("movaps      %%xmm5, %%xmm3")            /* x3   = b_im */ \
        __ASM_EMIT("movaps      %%xmm4, %%xmm2")            /* x2   = b_re */ \
        __ASM_EMIT("mulps       %%xmm3, %%xmm3")            /* x3   = b_im * b_im */ \
        __ASM_EMIT("mulps       %%xmm2, %%xmm2")            /* x2   = b_re * b_re */ \
        __ASM_EMIT("addps       %%xmm2, %%xmm3")            /* x3   = W = b_re * b_re + b_im * b_im */ \
        __ASM_EMIT("movaps      %%xmm0, %%xmm6")            /* x6   = t_re */ \
        __ASM_EMIT("movaps      %%xmm1, %%xmm7")            /* x7   = t_im */ \
        __ASM_EMIT("mulps       %%xmm4, %%xmm0")            /* x0   = t_re * b_re */ \
        __ASM_EMIT("mulps       %%xmm5, %%xmm7")            /* x7   = t_im * b_im */ \
        __ASM_EMIT("mulps       %%xmm4, %%xmm1")            /* x1   = t_im * b_re */ \
        __ASM_EMIT("mulps       %%xmm5, %%xmm6")            /* x6   = t_re * b_im */ \
        __ASM_EMIT("addps       %%xmm7, %%xmm0")            /* x0   = t_re * b_re + t_im * b_im */ \
        __ASM_EMIT("subps       %%xmm6, %%xmm1")            /* x1   = t_im * b_re - t_re * b_im */ \
        __ASM_EMIT("divps       %%xmm3, %%xmm0")            /* x0   = a_re = (t_re * b_re + t_im * b_im) / W */ \
        __ASM_EMIT("divps       %%xmm3, %%xmm1")            /* x1   = a_im = (t_im * b_re - t_re * b_im) / W */

    #define PHF_APPLY \
        /* Compute dst = H[f] * dst */ \
        __ASM_EMIT("movaps      %%xmm0, %%xmm4")            /* x4   = a_re */ \
        __ASM_EMIT("movaps      %%xmm1, %%xmm5")            /* x5   = a_im */ \
        __ASM_EMIT("mulps       %%xmm2, %%xmm0")            /* x0   = a_re * b_re */ \
        __ASM_EMIT("mulps       %%xmm3, %%xmm4")            /* x4   = a_re * b_im */ \
        __ASM_EMIT("mulps       %%xmm2, %%xmm1")            /* x1   = a_im * b_re */ \
        __ASM_EMIT("mulps       %%xmm3, %%xmm5")            /* x5   = a_im * b_im */ \
        __ASM_EMIT("addps       %%xmm4, %%xmm1")            /* x1   = a_re * b_im + a_im * b_re */ \
        __ASM_EMIT("subps       %%xmm5, %%xmm0")            /* x0   = a_re * b_re - a_im * b_im */ \
        __ASM_EMIT("movaps      %%xmm0, %%xmm2")            /* x2   = re */ \
        __ASM_EMIT("unpcklps    %%xmm1, %%xmm0")            /* x0   = r0 i0 r1 i1 */ \
        __ASM_EMIT("unpckhps    %%xmm1, %%xmm2")            /* x2   = r2 i2 r3 i3 */

    void filter_transfer_calc_pc(float *dst, const f_cascade_t *c, const float *freq, size_t count)
    {
        IF_ARCH_X86( float fp[6*4] __lsp_aligned16; );

        ARCH_X86_ASM(
            // Unpack filter params
            F_UNPACK
            // x4 block
            __ASM_EMIT("sub         $4, %[count]")
            __ASM_EMIT("jb          2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("movups      0x00(%[f]), %%xmm3")        // x3   = f
            PHF_CORE
            __ASM_EMIT("movaps      %%xmm0, %%xmm2")            // x2   = re
            __ASM_EMIT("unpcklps    %%xmm1, %%xmm0")            // x0   = r0 i0 r1 i1
            __ASM_EMIT("unpckhps    %%xmm1, %%xmm2")            // x2   = r2 i2 r3 i3
            __ASM_EMIT("movups      %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("movups      %%xmm2, 0x10(%[dst])")
            // Load filter params and repeat loop
            F_LOAD
            __ASM_EMIT("add         $0x10, %[f]")
            __ASM_EMIT("add         $0x20, %[dst]")
            __ASM_EMIT("sub         $4, %[count]")
            __ASM_EMIT("jae         1b")
            __ASM_EMIT("2:")
            // x2 block
            __ASM_EMIT("add         $2, %[count]")
            __ASM_EMIT("jl          4f")
            __ASM_EMIT("movlps      0x00(%[f]), %%xmm3")        // x3   = f
            PHF_CORE
            __ASM_EMIT("unpcklps    %%xmm1, %%xmm0")            // x0   = r0 i0 r1 i1
            __ASM_EMIT("movups      %%xmm0, 0x00(%[dst])")
            F_LOAD
            __ASM_EMIT("sub         $2, %[count]")
            __ASM_EMIT("add         $0x08, %[f]")
            __ASM_EMIT("add         $0x10, %[dst]")
            __ASM_EMIT("4:")
            // x1 block
            __ASM_EMIT("add         $1, %[count]")
            __ASM_EMIT("jl          6f")
            __ASM_EMIT("movss       0x00(%[f]), %%xmm3")        // x3   = f
            PHF_CORE
            __ASM_EMIT("unpcklps    %%xmm1, %%xmm0")            // x0   = r0 i0 r1 i1
            __ASM_EMIT("movlps      %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("6:")

            : [dst] "+r" (dst), [f] "+r" (freq), [count] "+r" (count)
            : [c] "r" (c),
              [fp] "o" (fp)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void filter_transfer_apply_pc(float *dst, const f_cascade_t *c, const float *freq, size_t count)
    {
        IF_ARCH_X86( float fp[6*4] __lsp_aligned16; );

        ARCH_X86_ASM(
            // Unpack filter params
            F_UNPACK
            // x4 block
            __ASM_EMIT("sub         $4, %[count]")
            __ASM_EMIT("jb          2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("movups      0x00(%[f]), %%xmm3")        // x3   = f
            PHF_CORE
            __ASM_EMIT("movups      0x00(%[dst]), %%xmm2")      // x2   = br0 bi0 br1 bi1
            __ASM_EMIT("movups      0x10(%[dst]), %%xmm4")      // x4   = br2 bi2 br3 bi3
            __ASM_EMIT("movaps      %%xmm2, %%xmm3")            // x3   = br0 bi0 br1 bi1
            __ASM_EMIT("shufps      $0x88, %%xmm4, %%xmm2")     // x2   = br0 br1 br2 br3
            __ASM_EMIT("shufps      $0xdd, %%xmm4, %%xmm3")     // x3   = bi0 bi1 bi2 bi3
            PHF_APPLY
            __ASM_EMIT("movups      %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("movups      %%xmm2, 0x10(%[dst])")
            // Load filter params and repeat loop
            F_LOAD
            __ASM_EMIT("add         $0x10, %[f]")
            __ASM_EMIT("add         $0x20, %[dst]")
            __ASM_EMIT("sub         $4, %[count]")
            __ASM_EMIT("jae         1b")
            __ASM_EMIT("2:")
            // x2 block
            __ASM_EMIT("add         $2, %[count]")
            __ASM_EMIT("jl          4f")
            __ASM_EMIT("movlps      0x00(%[f]), %%xmm3")        // x3   = f
            PHF_CORE
            __ASM_EMIT("movups      0x00(%[dst]), %%xmm2")      // x2   = br0 bi0 br1 bi1
            __ASM_EMIT("movaps      %%xmm2, %%xmm3")            // x3   = br0 bi0 br1 bi1
            __ASM_EMIT("shufps      $0x88, %%xmm2, %%xmm2")     // x2   = br0 br1 br0 bi1
            __ASM_EMIT("shufps      $0xdd, %%xmm3, %%xmm3")     // x3   = bi0 bi1 bi0 bi1
            PHF_APPLY
            __ASM_EMIT("movups      %%xmm0, 0x00(%[dst])")
            F_LOAD
            __ASM_EMIT("sub         $2, %[count]")
            __ASM_EMIT("add         $0x08, %[f]")
            __ASM_EMIT("add         $0x10, %[dst]")
            __ASM_EMIT("4:")
            // x1 block
            __ASM_EMIT("add         $1, %[count]")
            __ASM_EMIT("jl          6f")
            __ASM_EMIT("movss       0x00(%[f]), %%xmm3")        // x3   = f
            PHF_CORE
            __ASM_EMIT("movss       0x00(%[dst]), %%xmm2")      // x2   = br0
            __ASM_EMIT("movss       0x04(%[dst]), %%xmm3")      // x3   = bi0
            PHF_APPLY
            __ASM_EMIT("movlps      %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("6:")

            : [dst] "+r" (dst), [f] "+r" (freq), [count] "+r" (count)
            : [c] "r" (c),
              [fp] "o" (fp)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    #undef PHF_CORE
    #undef PHF_APPLY

    #undef F_UNPACK
    #undef F_LOAD
}

#endif /* DSP_ARCH_X86_SSE_FILTERS_TRANSFER_H_ */
