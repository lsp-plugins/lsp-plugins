/*
 * transfer.h
 *
 *  Created on: 3 янв. 2020 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_AVX_FILTERS_TRANSFER_H_
#define DSP_ARCH_X86_AVX_FILTERS_TRANSFER_H_

#ifndef DSP_ARCH_X86_AVX_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_AVX_IMPL */

namespace avx
{
    void filter_transfer_calc_ri(float *re, float *im, const f_cascade_t *c, const float *freq, size_t count)
    {
        IF_ARCH_X86( float fp[6*8] __lsp_aligned32; );

        ARCH_X86_ASM(
            // Unpack filter params
            __ASM_EMIT("vbroadcastss        0x00(%[c]), %%ymm0")                // y0   = t0
            __ASM_EMIT("vbroadcastss        0x04(%[c]), %%ymm1")                // y1   = t1
            __ASM_EMIT("vbroadcastss        0x08(%[c]), %%ymm2")                // y2   = t2
            __ASM_EMIT("vbroadcastss        0x10(%[c]), %%ymm3")                // y3   = b0
            __ASM_EMIT("vbroadcastss        0x14(%[c]), %%ymm4")                // y4   = b1
            __ASM_EMIT("vbroadcastss        0x18(%[c]), %%ymm5")                // y5   = b2
            __ASM_EMIT("vmovaps             %%ymm0, 0x00 + %[fp]")              // x0   = t0
            __ASM_EMIT("vmovaps             %%ymm1, 0x20 + %[fp]")              // x1   = t1
            __ASM_EMIT("vmovaps             %%ymm2, 0x40 + %[fp]")              // x2   = t2
            __ASM_EMIT("vmovaps             %%ymm3, 0x60 + %[fp]")              // x4   = b0
            __ASM_EMIT("vmovaps             %%ymm4, 0x80 + %[fp]")              // x5   = b1
            __ASM_EMIT("vmovaps             %%ymm5, 0xa0 + %[fp]")              // x6   = b2
            // x8 blocks
            __ASM_EMIT("sub                 $8, %[count]")
            __ASM_EMIT("jb                  2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vmovups             0x00(%[f]), %%ymm6")                // y6   = f
            __ASM_EMIT("vmulps              %%ymm6, %%ymm6, %%ymm7")            // y7   = f2 = f*f
            __ASM_EMIT("vmulps              %%ymm6, %%ymm1, %%ymm1")            // y1   = t_im = t1*f
            __ASM_EMIT("vmulps              %%ymm7, %%ymm2, %%ymm2")            // y2   = t2*f2
            __ASM_EMIT("vmulps              %%ymm6, %%ymm4, %%ymm4")            // y4   = b_im = b1*f
            __ASM_EMIT("vmulps              %%ymm7, %%ymm5, %%ymm5")            // y5   = b2*f2
            __ASM_EMIT("vsubps              %%ymm2, %%ymm0, %%ymm0")            // y0   = t_re = t0 - t2*f2
            __ASM_EMIT("vsubps              %%ymm5, %%ymm3, %%ymm3")            // y3   = b_re = b0 - b2*f2
            __ASM_EMIT("vmulps              %%ymm4, %%ymm4, %%ymm5")            // y5   = b_im*b_im
            __ASM_EMIT("vmulps              %%ymm3, %%ymm3, %%ymm2")            // y2   = b_re*b_re
            __ASM_EMIT("vaddps              %%ymm5, %%ymm2, %%ymm2")            // y2   = W = b_re*b_re + b_im*b_im
            __ASM_EMIT("vmulps              %%ymm4, %%ymm0, %%ymm6")            // y6   = t_re*b_im
            __ASM_EMIT("vmulps              %%ymm4, %%ymm1, %%ymm7")            // y7   = t_im*b_im
            __ASM_EMIT("vmulps              %%ymm3, %%ymm0, %%ymm0")            // y0   = t_re*b_re
            __ASM_EMIT("vmulps              %%ymm3, %%ymm1, %%ymm1")            // y1   = t_im*b_re
            __ASM_EMIT("vaddps              %%ymm7, %%ymm0, %%ymm0")            // y0   = t_re*b_re + t_im*b_im
            __ASM_EMIT("vsubps              %%ymm6, %%ymm1, %%ymm1")            // y1   = t_im*b_re - t_re*b_im
            __ASM_EMIT("vdivps              %%ymm2, %%ymm0, %%ymm0")            // y0   = a_re = (t_re*b_re + t_im*b_im)/W
            __ASM_EMIT("vdivps              %%ymm2, %%ymm1, %%ymm1")            // y1   = b_re = (t_im*b_re - t_re*b_im)/W
            // Apply
            __ASM_EMIT("vmovups             %%ymm0, 0x00(%[re])")
            __ASM_EMIT("vmovups             %%ymm1, 0x00(%[im])")
            // Reload
            __ASM_EMIT("vmovaps             0x00 + %[fp], %%ymm0")
            __ASM_EMIT("vmovaps             0x20 + %[fp], %%ymm1")
            __ASM_EMIT("vmovaps             0x40 + %[fp], %%ymm2")
            __ASM_EMIT("vmovaps             0x60 + %[fp], %%ymm3")
            __ASM_EMIT("vmovaps             0x80 + %[fp], %%ymm4")
            __ASM_EMIT("vmovaps             0xa0 + %[fp], %%ymm5")
            __ASM_EMIT("add                 $0x20, %[f]")
            __ASM_EMIT("add                 $0x20, %[re]")
            __ASM_EMIT("add                 $0x20, %[im]")
            __ASM_EMIT("sub                 $8, %[count]")
            __ASM_EMIT("jae                 1b")
            __ASM_EMIT("2:")
            // x4 block
            __ASM_EMIT("add                 $4, %[count]")
            __ASM_EMIT("jl                  4f")
            __ASM_EMIT("vmovups             0x00(%[f]), %%xmm6")                // x6   = f
            __ASM_EMIT("vmulps              %%xmm6, %%xmm6, %%xmm7")            // x7   = f2 = f*f
            __ASM_EMIT("vmulps              %%xmm6, %%xmm1, %%xmm1")            // x1   = t_im = t1*f
            __ASM_EMIT("vmulps              %%xmm7, %%xmm2, %%xmm2")            // x2   = t2*f2
            __ASM_EMIT("vmulps              %%xmm6, %%xmm4, %%xmm4")            // x4   = b_im = b1*f
            __ASM_EMIT("vmulps              %%xmm7, %%xmm5, %%xmm5")            // x5   = b2*f2
            __ASM_EMIT("vsubps              %%xmm2, %%xmm0, %%xmm0")            // x0   = t_re = t0 - t2*f2
            __ASM_EMIT("vsubps              %%xmm5, %%xmm3, %%xmm3")            // x3   = b_re = b0 - b2*f2
            __ASM_EMIT("vmulps              %%xmm4, %%xmm4, %%xmm5")            // x5   = b_im*b_im
            __ASM_EMIT("vmulps              %%xmm3, %%xmm3, %%xmm2")            // x2   = b_re*b_re
            __ASM_EMIT("vaddps              %%xmm5, %%xmm2, %%xmm2")            // x2   = W = b_re*b_re + b_im*b_im
            __ASM_EMIT("vmulps              %%xmm4, %%xmm0, %%xmm6")            // x6   = t_re*b_im
            __ASM_EMIT("vmulps              %%xmm4, %%xmm1, %%xmm7")            // x7   = t_im*b_im
            __ASM_EMIT("vmulps              %%xmm3, %%xmm0, %%xmm0")            // x0   = t_re*b_re
            __ASM_EMIT("vmulps              %%xmm3, %%xmm1, %%xmm1")            // x1   = t_im*b_re
            __ASM_EMIT("vaddps              %%xmm7, %%xmm0, %%xmm0")            // x0   = t_re*b_re + t_im*b_im
            __ASM_EMIT("vsubps              %%xmm6, %%xmm1, %%xmm1")            // x1   = t_im*b_re - t_re*b_im
            __ASM_EMIT("vdivps              %%xmm2, %%xmm0, %%xmm0")            // x0   = a_re = (t_re*b_re + t_im*b_im)/W
            __ASM_EMIT("vdivps              %%xmm2, %%xmm1, %%xmm1")            // x1   = b_re = (t_im*b_re - t_re*b_im)/W
            // Apply
            __ASM_EMIT("vmovups             %%xmm0, 0x00(%[re])")
            __ASM_EMIT("vmovups             %%xmm1, 0x00(%[im])")
            // Reload
            __ASM_EMIT("vmovaps             0x00 + %[fp], %%xmm0")
            __ASM_EMIT("vmovaps             0x20 + %[fp], %%xmm1")
            __ASM_EMIT("vmovaps             0x40 + %[fp], %%xmm2")
            __ASM_EMIT("vmovaps             0x60 + %[fp], %%xmm3")
            __ASM_EMIT("vmovaps             0x80 + %[fp], %%xmm4")
            __ASM_EMIT("vmovaps             0xa0 + %[fp], %%xmm5")
            __ASM_EMIT("sub                 $4, %[count]")
            __ASM_EMIT("add                 $0x10, %[f]")
            __ASM_EMIT("add                 $0x10, %[re]")
            __ASM_EMIT("add                 $0x10, %[im]")
            __ASM_EMIT("4:")
            // x2 block
            __ASM_EMIT("add                 $2, %[count]")
            __ASM_EMIT("jl                  6f")
            __ASM_EMIT("vmovlps             0x00(%[f]), %%xmm6, %%xmm6")        // x6   = f
            __ASM_EMIT("vmulps              %%xmm6, %%xmm6, %%xmm7")            // x7   = f2 = f*f
            __ASM_EMIT("vmulps              %%xmm6, %%xmm1, %%xmm1")            // x1   = t_im = t1*f
            __ASM_EMIT("vmulps              %%xmm7, %%xmm2, %%xmm2")            // x2   = t2*f2
            __ASM_EMIT("vmulps              %%xmm6, %%xmm4, %%xmm4")            // x4   = b_im = b1*f
            __ASM_EMIT("vmulps              %%xmm7, %%xmm5, %%xmm5")            // x5   = b2*f2
            __ASM_EMIT("vsubps              %%xmm2, %%xmm0, %%xmm0")            // x0   = t_re = t0 - t2*f2
            __ASM_EMIT("vsubps              %%xmm5, %%xmm3, %%xmm3")            // x3   = b_re = b0 - b2*f2
            __ASM_EMIT("vmulps              %%xmm4, %%xmm4, %%xmm5")            // x5   = b_im*b_im
            __ASM_EMIT("vmulps              %%xmm3, %%xmm3, %%xmm2")            // x2   = b_re*b_re
            __ASM_EMIT("vaddps              %%xmm5, %%xmm2, %%xmm2")            // x2   = W = b_re*b_re + b_im*b_im
            __ASM_EMIT("vmulps              %%xmm4, %%xmm0, %%xmm6")            // x6   = t_re*b_im
            __ASM_EMIT("vmulps              %%xmm4, %%xmm1, %%xmm7")            // x7   = t_im*b_im
            __ASM_EMIT("vmulps              %%xmm3, %%xmm0, %%xmm0")            // x0   = t_re*b_re
            __ASM_EMIT("vmulps              %%xmm3, %%xmm1, %%xmm1")            // x1   = t_im*b_re
            __ASM_EMIT("vaddps              %%xmm7, %%xmm0, %%xmm0")            // x0   = t_re*b_re + t_im*b_im
            __ASM_EMIT("vsubps              %%xmm6, %%xmm1, %%xmm1")            // x1   = t_im*b_re - t_re*b_im
            __ASM_EMIT("vdivps              %%xmm2, %%xmm0, %%xmm0")            // x0   = a_re = (t_re*b_re + t_im*b_im)/W
            __ASM_EMIT("vdivps              %%xmm2, %%xmm1, %%xmm1")            // x1   = b_re = (t_im*b_re - t_re*b_im)/W
            // Apply
            __ASM_EMIT("vmovlps             %%xmm0, 0x00(%[re])")
            __ASM_EMIT("vmovlps             %%xmm1, 0x00(%[im])")
            // Reload
            __ASM_EMIT("vmovaps             0x00 + %[fp], %%xmm0")
            __ASM_EMIT("vmovaps             0x20 + %[fp], %%xmm1")
            __ASM_EMIT("vmovaps             0x40 + %[fp], %%xmm2")
            __ASM_EMIT("vmovaps             0x60 + %[fp], %%xmm3")
            __ASM_EMIT("vmovaps             0x80 + %[fp], %%xmm4")
            __ASM_EMIT("vmovaps             0xa0 + %[fp], %%xmm5")
            __ASM_EMIT("sub                 $2, %[count]")
            __ASM_EMIT("add                 $0x08, %[f]")
            __ASM_EMIT("add                 $0x08, %[re]")
            __ASM_EMIT("add                 $0x08, %[im]")
            __ASM_EMIT("6:")
            // x1 block
            __ASM_EMIT("add                 $1, %[count]")
            __ASM_EMIT("jl                  8f")
            __ASM_EMIT("vmovss              0x00(%[f]), %%xmm6")                // x6   = f
            __ASM_EMIT("vmulps              %%xmm6, %%xmm6, %%xmm7")            // x7   = f2 = f*f
            __ASM_EMIT("vmulps              %%xmm6, %%xmm1, %%xmm1")            // x1   = t_im = t1*f
            __ASM_EMIT("vmulps              %%xmm7, %%xmm2, %%xmm2")            // x2   = t2*f2
            __ASM_EMIT("vmulps              %%xmm6, %%xmm4, %%xmm4")            // x4   = b_im = b1*f
            __ASM_EMIT("vmulps              %%xmm7, %%xmm5, %%xmm5")            // x5   = b2*f2
            __ASM_EMIT("vsubps              %%xmm2, %%xmm0, %%xmm0")            // x0   = t_re = t0 - t2*f2
            __ASM_EMIT("vsubps              %%xmm5, %%xmm3, %%xmm3")            // x3   = b_re = b0 - b2*f2
            __ASM_EMIT("vmulps              %%xmm4, %%xmm4, %%xmm5")            // x5   = b_im*b_im
            __ASM_EMIT("vmulps              %%xmm3, %%xmm3, %%xmm2")            // x2   = b_re*b_re
            __ASM_EMIT("vaddps              %%xmm5, %%xmm2, %%xmm2")            // x2   = W = b_re*b_re + b_im*b_im
            __ASM_EMIT("vmulps              %%xmm4, %%xmm0, %%xmm6")            // x6   = t_re*b_im
            __ASM_EMIT("vmulps              %%xmm4, %%xmm1, %%xmm7")            // x7   = t_im*b_im
            __ASM_EMIT("vmulps              %%xmm3, %%xmm0, %%xmm0")            // x0   = t_re*b_re
            __ASM_EMIT("vmulps              %%xmm3, %%xmm1, %%xmm1")            // x1   = t_im*b_re
            __ASM_EMIT("vaddps              %%xmm7, %%xmm0, %%xmm0")            // x0   = t_re*b_re + t_im*b_im
            __ASM_EMIT("vsubps              %%xmm6, %%xmm1, %%xmm1")            // x1   = t_im*b_re - t_re*b_im
            __ASM_EMIT("vdivps              %%xmm2, %%xmm0, %%xmm0")            // x0   = a_re = (t_re*b_re + t_im*b_im)/W
            __ASM_EMIT("vdivps              %%xmm2, %%xmm1, %%xmm1")            // x1   = b_re = (t_im*b_re - t_re*b_im)/W
            // Apply
            __ASM_EMIT("vmovss              %%xmm0, 0x00(%[re])")
            __ASM_EMIT("vmovss              %%xmm1, 0x00(%[im])")
            __ASM_EMIT("8:")

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
        IF_ARCH_X86( float fp[6*8] __lsp_aligned32; );

        ARCH_X86_ASM(
            // Unpack filter params
            __ASM_EMIT("vbroadcastss        0x00(%[c]), %%ymm0")                // y0   = t0
            __ASM_EMIT("vbroadcastss        0x04(%[c]), %%ymm1")                // y1   = t1
            __ASM_EMIT("vbroadcastss        0x08(%[c]), %%ymm2")                // y2   = t2
            __ASM_EMIT("vbroadcastss        0x10(%[c]), %%ymm3")                // y3   = b0
            __ASM_EMIT("vbroadcastss        0x14(%[c]), %%ymm4")                // y4   = b1
            __ASM_EMIT("vbroadcastss        0x18(%[c]), %%ymm5")                // y5   = b2
            __ASM_EMIT("vmovaps             %%ymm0, 0x00 + %[fp]")              // x0   = t0
            __ASM_EMIT("vmovaps             %%ymm1, 0x20 + %[fp]")              // x1   = t1
            __ASM_EMIT("vmovaps             %%ymm2, 0x40 + %[fp]")              // x2   = t2
            __ASM_EMIT("vmovaps             %%ymm3, 0x60 + %[fp]")              // x4   = b0
            __ASM_EMIT("vmovaps             %%ymm4, 0x80 + %[fp]")              // x5   = b1
            __ASM_EMIT("vmovaps             %%ymm5, 0xa0 + %[fp]")              // x6   = b2
            // x8 blocks
            __ASM_EMIT("sub                 $8, %[count]")
            __ASM_EMIT("jb                  2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vmovups             0x00(%[f]), %%ymm6")                // y6   = f
            __ASM_EMIT("vmulps              %%ymm6, %%ymm6, %%ymm7")            // y7   = f2 = f*f
            __ASM_EMIT("vmulps              %%ymm6, %%ymm1, %%ymm1")            // y1   = t_im = t1*f
            __ASM_EMIT("vmulps              %%ymm7, %%ymm2, %%ymm2")            // y2   = t2*f2
            __ASM_EMIT("vmulps              %%ymm6, %%ymm4, %%ymm4")            // y4   = b_im = b1*f
            __ASM_EMIT("vmulps              %%ymm7, %%ymm5, %%ymm5")            // y5   = b2*f2
            __ASM_EMIT("vsubps              %%ymm2, %%ymm0, %%ymm0")            // y0   = t_re = t0 - t2*f2
            __ASM_EMIT("vsubps              %%ymm5, %%ymm3, %%ymm3")            // y3   = b_re = b0 - b2*f2
            __ASM_EMIT("vmulps              %%ymm4, %%ymm4, %%ymm5")            // y5   = b_im*b_im
            __ASM_EMIT("vmulps              %%ymm3, %%ymm3, %%ymm2")            // y2   = b_re*b_re
            __ASM_EMIT("vaddps              %%ymm5, %%ymm2, %%ymm2")            // y2   = W = b_re*b_re + b_im*b_im
            __ASM_EMIT("vmulps              %%ymm4, %%ymm0, %%ymm6")            // y6   = t_re*b_im
            __ASM_EMIT("vmulps              %%ymm4, %%ymm1, %%ymm7")            // y7   = t_im*b_im
            __ASM_EMIT("vmulps              %%ymm3, %%ymm0, %%ymm0")            // y0   = t_re*b_re
            __ASM_EMIT("vmulps              %%ymm3, %%ymm1, %%ymm1")            // y1   = t_im*b_re
            __ASM_EMIT("vaddps              %%ymm7, %%ymm0, %%ymm0")            // y0   = t_re*b_re + t_im*b_im
            __ASM_EMIT("vsubps              %%ymm6, %%ymm1, %%ymm1")            // y1   = t_im*b_re - t_re*b_im
            __ASM_EMIT("vdivps              %%ymm2, %%ymm0, %%ymm0")            // y0   = a_re = (t_re*b_re + t_im*b_im)/W
            __ASM_EMIT("vdivps              %%ymm2, %%ymm1, %%ymm1")            // y1   = b_re = (t_im*b_re - t_re*b_im)/W
            // Apply
            __ASM_EMIT("vmulps              0x00(%[im]), %%ymm0, %%ymm2")       // y2   = a_re*b_im
            __ASM_EMIT("vmulps              0x00(%[im]), %%ymm1, %%ymm3")       // y3   = a_im*b_im
            __ASM_EMIT("vmulps              0x00(%[re]), %%ymm0, %%ymm0")       // y0   = a_re*b_re
            __ASM_EMIT("vmulps              0x00(%[re]), %%ymm1, %%ymm1")       // y1   = a_im*b_re
            __ASM_EMIT("vsubps              %%ymm3, %%ymm0, %%ymm0")            // y0   = a_re*b_re - a_im*b_im
            __ASM_EMIT("vaddps              %%ymm2, %%ymm1, %%ymm1")            // y1   = a_im*b_re + a_re*b_im
            __ASM_EMIT("vmovups             %%ymm0, 0x00(%[re])")
            __ASM_EMIT("vmovups             %%ymm1, 0x00(%[im])")
            // Reload
            __ASM_EMIT("vmovaps             0x00 + %[fp], %%ymm0")
            __ASM_EMIT("vmovaps             0x20 + %[fp], %%ymm1")
            __ASM_EMIT("vmovaps             0x40 + %[fp], %%ymm2")
            __ASM_EMIT("vmovaps             0x60 + %[fp], %%ymm3")
            __ASM_EMIT("vmovaps             0x80 + %[fp], %%ymm4")
            __ASM_EMIT("vmovaps             0xa0 + %[fp], %%ymm5")
            __ASM_EMIT("add                 $0x20, %[f]")
            __ASM_EMIT("add                 $0x20, %[re]")
            __ASM_EMIT("add                 $0x20, %[im]")
            __ASM_EMIT("sub                 $8, %[count]")
            __ASM_EMIT("jae                 1b")
            __ASM_EMIT("2:")
            // x4 block
            __ASM_EMIT("add                 $4, %[count]")
            __ASM_EMIT("jl                  4f")
            __ASM_EMIT("vmovups             0x00(%[f]), %%xmm6")                // x6   = f
            __ASM_EMIT("vmulps              %%xmm6, %%xmm6, %%xmm7")            // x7   = f2 = f*f
            __ASM_EMIT("vmulps              %%xmm6, %%xmm1, %%xmm1")            // x1   = t_im = t1*f
            __ASM_EMIT("vmulps              %%xmm7, %%xmm2, %%xmm2")            // x2   = t2*f2
            __ASM_EMIT("vmulps              %%xmm6, %%xmm4, %%xmm4")            // x4   = b_im = b1*f
            __ASM_EMIT("vmulps              %%xmm7, %%xmm5, %%xmm5")            // x5   = b2*f2
            __ASM_EMIT("vsubps              %%xmm2, %%xmm0, %%xmm0")            // x0   = t_re = t0 - t2*f2
            __ASM_EMIT("vsubps              %%xmm5, %%xmm3, %%xmm3")            // x3   = b_re = b0 - b2*f2
            __ASM_EMIT("vmulps              %%xmm4, %%xmm4, %%xmm5")            // x5   = b_im*b_im
            __ASM_EMIT("vmulps              %%xmm3, %%xmm3, %%xmm2")            // x2   = b_re*b_re
            __ASM_EMIT("vaddps              %%xmm5, %%xmm2, %%xmm2")            // x2   = W = b_re*b_re + b_im*b_im
            __ASM_EMIT("vmulps              %%xmm4, %%xmm0, %%xmm6")            // x6   = t_re*b_im
            __ASM_EMIT("vmulps              %%xmm4, %%xmm1, %%xmm7")            // x7   = t_im*b_im
            __ASM_EMIT("vmulps              %%xmm3, %%xmm0, %%xmm0")            // x0   = t_re*b_re
            __ASM_EMIT("vmulps              %%xmm3, %%xmm1, %%xmm1")            // x1   = t_im*b_re
            __ASM_EMIT("vaddps              %%xmm7, %%xmm0, %%xmm0")            // x0   = t_re*b_re + t_im*b_im
            __ASM_EMIT("vsubps              %%xmm6, %%xmm1, %%xmm1")            // x1   = t_im*b_re - t_re*b_im
            __ASM_EMIT("vdivps              %%xmm2, %%xmm0, %%xmm0")            // x0   = a_re = (t_re*b_re + t_im*b_im)/W
            __ASM_EMIT("vdivps              %%xmm2, %%xmm1, %%xmm1")            // x1   = b_re = (t_im*b_re - t_re*b_im)/W
            // Apply
            __ASM_EMIT("vmulps              0x00(%[im]), %%xmm0, %%xmm2")       // x2   = a_re*b_im
            __ASM_EMIT("vmulps              0x00(%[im]), %%xmm1, %%xmm3")       // x3   = a_im*b_im
            __ASM_EMIT("vmulps              0x00(%[re]), %%xmm0, %%xmm0")       // x0   = a_re*b_re
            __ASM_EMIT("vmulps              0x00(%[re]), %%xmm1, %%xmm1")       // x1   = a_im*b_re
            __ASM_EMIT("vsubps              %%xmm3, %%xmm0, %%xmm0")            // x0   = a_re*b_re - a_im*b_im
            __ASM_EMIT("vaddps              %%xmm2, %%xmm1, %%xmm1")            // x1   = a_im*b_re + a_re*b_im
            __ASM_EMIT("vmovups             %%xmm0, 0x00(%[re])")
            __ASM_EMIT("vmovups             %%xmm1, 0x00(%[im])")
            // Reload
            __ASM_EMIT("vmovaps             0x00 + %[fp], %%xmm0")
            __ASM_EMIT("vmovaps             0x20 + %[fp], %%xmm1")
            __ASM_EMIT("vmovaps             0x40 + %[fp], %%xmm2")
            __ASM_EMIT("vmovaps             0x60 + %[fp], %%xmm3")
            __ASM_EMIT("vmovaps             0x80 + %[fp], %%xmm4")
            __ASM_EMIT("vmovaps             0xa0 + %[fp], %%xmm5")
            __ASM_EMIT("sub                 $4, %[count]")
            __ASM_EMIT("add                 $0x10, %[f]")
            __ASM_EMIT("add                 $0x10, %[re]")
            __ASM_EMIT("add                 $0x10, %[im]")
            __ASM_EMIT("4:")
            // x2 block
            __ASM_EMIT("add                 $2, %[count]")
            __ASM_EMIT("jl                  6f")
            __ASM_EMIT("vmovlps             0x00(%[f]), %%xmm6, %%xmm6")        // x6   = f
            __ASM_EMIT("vmulps              %%xmm6, %%xmm6, %%xmm7")            // x7   = f2 = f*f
            __ASM_EMIT("vmulps              %%xmm6, %%xmm1, %%xmm1")            // x1   = t_im = t1*f
            __ASM_EMIT("vmulps              %%xmm7, %%xmm2, %%xmm2")            // x2   = t2*f2
            __ASM_EMIT("vmulps              %%xmm6, %%xmm4, %%xmm4")            // x4   = b_im = b1*f
            __ASM_EMIT("vmulps              %%xmm7, %%xmm5, %%xmm5")            // x5   = b2*f2
            __ASM_EMIT("vsubps              %%xmm2, %%xmm0, %%xmm0")            // x0   = t_re = t0 - t2*f2
            __ASM_EMIT("vsubps              %%xmm5, %%xmm3, %%xmm3")            // x3   = b_re = b0 - b2*f2
            __ASM_EMIT("vmulps              %%xmm4, %%xmm4, %%xmm5")            // x5   = b_im*b_im
            __ASM_EMIT("vmulps              %%xmm3, %%xmm3, %%xmm2")            // x2   = b_re*b_re
            __ASM_EMIT("vaddps              %%xmm5, %%xmm2, %%xmm2")            // x2   = W = b_re*b_re + b_im*b_im
            __ASM_EMIT("vmulps              %%xmm4, %%xmm0, %%xmm6")            // x6   = t_re*b_im
            __ASM_EMIT("vmulps              %%xmm4, %%xmm1, %%xmm7")            // x7   = t_im*b_im
            __ASM_EMIT("vmulps              %%xmm3, %%xmm0, %%xmm0")            // x0   = t_re*b_re
            __ASM_EMIT("vmulps              %%xmm3, %%xmm1, %%xmm1")            // x1   = t_im*b_re
            __ASM_EMIT("vaddps              %%xmm7, %%xmm0, %%xmm0")            // x0   = t_re*b_re + t_im*b_im
            __ASM_EMIT("vsubps              %%xmm6, %%xmm1, %%xmm1")            // x1   = t_im*b_re - t_re*b_im
            __ASM_EMIT("vdivps              %%xmm2, %%xmm0, %%xmm0")            // x0   = a_re = (t_re*b_re + t_im*b_im)/W
            __ASM_EMIT("vdivps              %%xmm2, %%xmm1, %%xmm1")            // x1   = b_re = (t_im*b_re - t_re*b_im)/W
            // Apply
            __ASM_EMIT("vmovlps             0x00(%[im]), %%xmm7, %%xmm7")
            __ASM_EMIT("vmovlps             0x00(%[re]), %%xmm6, %%xmm6")
            __ASM_EMIT("vmulps              %%xmm7, %%xmm0, %%xmm2")            // x2   = a_re*b_im
            __ASM_EMIT("vmulps              %%xmm7, %%xmm1, %%xmm3")            // x3   = a_im*b_im
            __ASM_EMIT("vmulps              %%xmm6, %%xmm0, %%xmm0")            // x0   = a_re*b_re
            __ASM_EMIT("vmulps              %%xmm6, %%xmm1, %%xmm1")            // x1   = a_im*b_re
            __ASM_EMIT("vsubps              %%xmm3, %%xmm0, %%xmm0")            // x0   = a_re*b_re - a_im*b_im
            __ASM_EMIT("vaddps              %%xmm2, %%xmm1, %%xmm1")            // x1   = a_im*b_re + a_re*b_im
            __ASM_EMIT("vmovlps             %%xmm0, 0x00(%[re])")
            __ASM_EMIT("vmovlps             %%xmm1, 0x00(%[im])")
            // Reload
            __ASM_EMIT("vmovaps             0x00 + %[fp], %%xmm0")
            __ASM_EMIT("vmovaps             0x20 + %[fp], %%xmm1")
            __ASM_EMIT("vmovaps             0x40 + %[fp], %%xmm2")
            __ASM_EMIT("vmovaps             0x60 + %[fp], %%xmm3")
            __ASM_EMIT("vmovaps             0x80 + %[fp], %%xmm4")
            __ASM_EMIT("vmovaps             0xa0 + %[fp], %%xmm5")
            __ASM_EMIT("sub                 $2, %[count]")
            __ASM_EMIT("add                 $0x08, %[f]")
            __ASM_EMIT("add                 $0x08, %[re]")
            __ASM_EMIT("add                 $0x08, %[im]")
            __ASM_EMIT("6:")
            // x1 block
            __ASM_EMIT("add                 $1, %[count]")
            __ASM_EMIT("jl                  8f")
            __ASM_EMIT("vmovss              0x00(%[f]), %%xmm6")                // x6   = f
            __ASM_EMIT("vmulps              %%xmm6, %%xmm6, %%xmm7")            // x7   = f2 = f*f
            __ASM_EMIT("vmulps              %%xmm6, %%xmm1, %%xmm1")            // x1   = t_im = t1*f
            __ASM_EMIT("vmulps              %%xmm7, %%xmm2, %%xmm2")            // x2   = t2*f2
            __ASM_EMIT("vmulps              %%xmm6, %%xmm4, %%xmm4")            // x4   = b_im = b1*f
            __ASM_EMIT("vmulps              %%xmm7, %%xmm5, %%xmm5")            // x5   = b2*f2
            __ASM_EMIT("vsubps              %%xmm2, %%xmm0, %%xmm0")            // x0   = t_re = t0 - t2*f2
            __ASM_EMIT("vsubps              %%xmm5, %%xmm3, %%xmm3")            // x3   = b_re = b0 - b2*f2
            __ASM_EMIT("vmulps              %%xmm4, %%xmm4, %%xmm5")            // x5   = b_im*b_im
            __ASM_EMIT("vmulps              %%xmm3, %%xmm3, %%xmm2")            // x2   = b_re*b_re
            __ASM_EMIT("vaddps              %%xmm5, %%xmm2, %%xmm2")            // x2   = W = b_re*b_re + b_im*b_im
            __ASM_EMIT("vmulps              %%xmm4, %%xmm0, %%xmm6")            // x6   = t_re*b_im
            __ASM_EMIT("vmulps              %%xmm4, %%xmm1, %%xmm7")            // x7   = t_im*b_im
            __ASM_EMIT("vmulps              %%xmm3, %%xmm0, %%xmm0")            // x0   = t_re*b_re
            __ASM_EMIT("vmulps              %%xmm3, %%xmm1, %%xmm1")            // x1   = t_im*b_re
            __ASM_EMIT("vaddps              %%xmm7, %%xmm0, %%xmm0")            // x0   = t_re*b_re + t_im*b_im
            __ASM_EMIT("vsubps              %%xmm6, %%xmm1, %%xmm1")            // x1   = t_im*b_re - t_re*b_im
            __ASM_EMIT("vdivps              %%xmm2, %%xmm0, %%xmm0")            // x0   = a_re = (t_re*b_re + t_im*b_im)/W
            __ASM_EMIT("vdivps              %%xmm2, %%xmm1, %%xmm1")            // x1   = b_re = (t_im*b_re - t_re*b_im)/W
            // Apply
            __ASM_EMIT("vmovss              0x00(%[im]), %%xmm7")
            __ASM_EMIT("vmovss              0x00(%[re]), %%xmm6")
            __ASM_EMIT("vmulps              %%xmm7, %%xmm0, %%xmm2")            // x2   = a_re*b_im
            __ASM_EMIT("vmulps              %%xmm7, %%xmm1, %%xmm3")            // x3   = a_im*b_im
            __ASM_EMIT("vmulps              %%xmm6, %%xmm0, %%xmm0")            // x0   = a_re*b_re
            __ASM_EMIT("vmulps              %%xmm6, %%xmm1, %%xmm1")            // x1   = a_im*b_re
            __ASM_EMIT("vsubps              %%xmm3, %%xmm0, %%xmm0")            // x0   = a_re*b_re - a_im*b_im
            __ASM_EMIT("vaddps              %%xmm2, %%xmm1, %%xmm1")            // x1   = a_im*b_re + a_re*b_im
            __ASM_EMIT("vmovss              %%xmm0, 0x00(%[re])")
            __ASM_EMIT("vmovss              %%xmm1, 0x00(%[im])")
            __ASM_EMIT("8:")

            : [re] "+r" (re), [im] "+r" (im), [f] "+r" (freq), [count] "+r" (count)
            : [c] "r" (c),
              [fp] "o" (fp)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void filter_transfer_calc_pc(float *dst, const f_cascade_t *c, const float *freq, size_t count)
    {
        IF_ARCH_X86( float fp[6*8] __lsp_aligned32; );

        ARCH_X86_ASM(
            // Unpack filter params
            __ASM_EMIT("vbroadcastss        0x00(%[c]), %%ymm0")                // y0   = t0
            __ASM_EMIT("vbroadcastss        0x04(%[c]), %%ymm1")                // y1   = t1
            __ASM_EMIT("vbroadcastss        0x08(%[c]), %%ymm2")                // y2   = t2
            __ASM_EMIT("vbroadcastss        0x10(%[c]), %%ymm3")                // y3   = b0
            __ASM_EMIT("vbroadcastss        0x14(%[c]), %%ymm4")                // y4   = b1
            __ASM_EMIT("vbroadcastss        0x18(%[c]), %%ymm5")                // y5   = b2
            __ASM_EMIT("vmovaps             %%ymm0, 0x00 + %[fp]")              // x0   = t0
            __ASM_EMIT("vmovaps             %%ymm1, 0x20 + %[fp]")              // x1   = t1
            __ASM_EMIT("vmovaps             %%ymm2, 0x40 + %[fp]")              // x2   = t2
            __ASM_EMIT("vmovaps             %%ymm3, 0x60 + %[fp]")              // x4   = b0
            __ASM_EMIT("vmovaps             %%ymm4, 0x80 + %[fp]")              // x5   = b1
            __ASM_EMIT("vmovaps             %%ymm5, 0xa0 + %[fp]")              // x6   = b2
            // x8 blocks
            __ASM_EMIT("sub                 $8, %[count]")
            __ASM_EMIT("jb                  2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vmovups             0x00(%[f]), %%ymm6")                // y6   = f
            __ASM_EMIT("vmulps              %%ymm6, %%ymm6, %%ymm7")            // y7   = f2 = f*f
            __ASM_EMIT("vmulps              %%ymm6, %%ymm1, %%ymm1")            // y1   = t_im = t1*f
            __ASM_EMIT("vmulps              %%ymm7, %%ymm2, %%ymm2")            // y2   = t2*f2
            __ASM_EMIT("vmulps              %%ymm6, %%ymm4, %%ymm4")            // y4   = b_im = b1*f
            __ASM_EMIT("vmulps              %%ymm7, %%ymm5, %%ymm5")            // y5   = b2*f2
            __ASM_EMIT("vsubps              %%ymm2, %%ymm0, %%ymm0")            // y0   = t_re = t0 - t2*f2
            __ASM_EMIT("vsubps              %%ymm5, %%ymm3, %%ymm3")            // y3   = b_re = b0 - b2*f2
            __ASM_EMIT("vmulps              %%ymm4, %%ymm4, %%ymm5")            // y5   = b_im*b_im
            __ASM_EMIT("vmulps              %%ymm3, %%ymm3, %%ymm2")            // y2   = b_re*b_re
            __ASM_EMIT("vaddps              %%ymm5, %%ymm2, %%ymm2")            // y2   = W = b_re*b_re + b_im*b_im
            __ASM_EMIT("vmulps              %%ymm4, %%ymm0, %%ymm6")            // y6   = t_re*b_im
            __ASM_EMIT("vmulps              %%ymm4, %%ymm1, %%ymm7")            // y7   = t_im*b_im
            __ASM_EMIT("vmulps              %%ymm3, %%ymm0, %%ymm0")            // y0   = t_re*b_re
            __ASM_EMIT("vmulps              %%ymm3, %%ymm1, %%ymm1")            // y1   = t_im*b_re
            __ASM_EMIT("vaddps              %%ymm7, %%ymm0, %%ymm0")            // y0   = t_re*b_re + t_im*b_im
            __ASM_EMIT("vsubps              %%ymm6, %%ymm1, %%ymm1")            // y1   = t_im*b_re - t_re*b_im
            __ASM_EMIT("vdivps              %%ymm2, %%ymm0, %%ymm0")            // y0   = a_re = (t_re*b_re + t_im*b_im)/W
            __ASM_EMIT("vdivps              %%ymm2, %%ymm1, %%ymm1")            // y1   = b_re = (t_im*b_re - t_re*b_im)/W
            // Apply
            __ASM_EMIT("vunpcklps           %%ymm1, %%ymm0, %%ymm2")            // y2   = r0 i0 r1 i1 r4 i4 r5 i5
            __ASM_EMIT("vunpckhps           %%ymm1, %%ymm0, %%ymm3")            // y3   = r2 i2 r3 i3 r6 i6 r7 i7
            __ASM_EMIT("vmovups             %%xmm2, 0x00(%[dst])")
            __ASM_EMIT("vmovups             %%xmm3, 0x10(%[dst])")
            __ASM_EMIT("vextractf128        $1, %%ymm2, 0x20(%[dst])")
            __ASM_EMIT("vextractf128        $1, %%ymm3, 0x30(%[dst])")
            // Reload
            __ASM_EMIT("vmovaps             0x00 + %[fp], %%ymm0")
            __ASM_EMIT("vmovaps             0x20 + %[fp], %%ymm1")
            __ASM_EMIT("vmovaps             0x40 + %[fp], %%ymm2")
            __ASM_EMIT("vmovaps             0x60 + %[fp], %%ymm3")
            __ASM_EMIT("vmovaps             0x80 + %[fp], %%ymm4")
            __ASM_EMIT("vmovaps             0xa0 + %[fp], %%ymm5")
            __ASM_EMIT("add                 $0x20, %[f]")
            __ASM_EMIT("add                 $0x40, %[dst]")
            __ASM_EMIT("sub                 $8, %[count]")
            __ASM_EMIT("jae                 1b")
            __ASM_EMIT("2:")
            // x4 block
            __ASM_EMIT("add                 $4, %[count]")
            __ASM_EMIT("jl                  4f")
            __ASM_EMIT("vmovups             0x00(%[f]), %%xmm6")                // x6   = f
            __ASM_EMIT("vmulps              %%xmm6, %%xmm6, %%xmm7")            // x7   = f2 = f*f
            __ASM_EMIT("vmulps              %%xmm6, %%xmm1, %%xmm1")            // x1   = t_im = t1*f
            __ASM_EMIT("vmulps              %%xmm7, %%xmm2, %%xmm2")            // x2   = t2*f2
            __ASM_EMIT("vmulps              %%xmm6, %%xmm4, %%xmm4")            // x4   = b_im = b1*f
            __ASM_EMIT("vmulps              %%xmm7, %%xmm5, %%xmm5")            // x5   = b2*f2
            __ASM_EMIT("vsubps              %%xmm2, %%xmm0, %%xmm0")            // x0   = t_re = t0 - t2*f2
            __ASM_EMIT("vsubps              %%xmm5, %%xmm3, %%xmm3")            // x3   = b_re = b0 - b2*f2
            __ASM_EMIT("vmulps              %%xmm4, %%xmm4, %%xmm5")            // x5   = b_im*b_im
            __ASM_EMIT("vmulps              %%xmm3, %%xmm3, %%xmm2")            // x2   = b_re*b_re
            __ASM_EMIT("vaddps              %%xmm5, %%xmm2, %%xmm2")            // x2   = W = b_re*b_re + b_im*b_im
            __ASM_EMIT("vmulps              %%xmm4, %%xmm0, %%xmm6")            // x6   = t_re*b_im
            __ASM_EMIT("vmulps              %%xmm4, %%xmm1, %%xmm7")            // x7   = t_im*b_im
            __ASM_EMIT("vmulps              %%xmm3, %%xmm0, %%xmm0")            // x0   = t_re*b_re
            __ASM_EMIT("vmulps              %%xmm3, %%xmm1, %%xmm1")            // x1   = t_im*b_re
            __ASM_EMIT("vaddps              %%xmm7, %%xmm0, %%xmm0")            // x0   = t_re*b_re + t_im*b_im
            __ASM_EMIT("vsubps              %%xmm6, %%xmm1, %%xmm1")            // x1   = t_im*b_re - t_re*b_im
            __ASM_EMIT("vdivps              %%xmm2, %%xmm0, %%xmm0")            // x0   = a_re = (t_re*b_re + t_im*b_im)/W
            __ASM_EMIT("vdivps              %%xmm2, %%xmm1, %%xmm1")            // x1   = b_re = (t_im*b_re - t_re*b_im)/W
            // Apply
            __ASM_EMIT("vunpcklps           %%xmm1, %%xmm0, %%xmm2")            // x2   = r0 i0 r1 i1 r4 i4 r5 i5
            __ASM_EMIT("vunpckhps           %%xmm1, %%xmm0, %%xmm3")            // x3   = r2 i2 r3 i3 r6 i6 r7 i7
            __ASM_EMIT("vmovups             %%xmm2, 0x00(%[dst])")
            __ASM_EMIT("vmovups             %%xmm3, 0x10(%[dst])")
            // Reload
            __ASM_EMIT("vmovaps             0x00 + %[fp], %%xmm0")
            __ASM_EMIT("vmovaps             0x20 + %[fp], %%xmm1")
            __ASM_EMIT("vmovaps             0x40 + %[fp], %%xmm2")
            __ASM_EMIT("vmovaps             0x60 + %[fp], %%xmm3")
            __ASM_EMIT("vmovaps             0x80 + %[fp], %%xmm4")
            __ASM_EMIT("vmovaps             0xa0 + %[fp], %%xmm5")
            __ASM_EMIT("sub                 $4, %[count]")
            __ASM_EMIT("add                 $0x10, %[f]")
            __ASM_EMIT("add                 $0x20, %[dst]")
            __ASM_EMIT("4:")
            // x2 block
            __ASM_EMIT("add                 $2, %[count]")
            __ASM_EMIT("jl                  6f")
            __ASM_EMIT("vmovlps             0x00(%[f]), %%xmm6, %%xmm6")        // x6   = f
            __ASM_EMIT("vmulps              %%xmm6, %%xmm6, %%xmm7")            // x7   = f2 = f*f
            __ASM_EMIT("vmulps              %%xmm6, %%xmm1, %%xmm1")            // x1   = t_im = t1*f
            __ASM_EMIT("vmulps              %%xmm7, %%xmm2, %%xmm2")            // x2   = t2*f2
            __ASM_EMIT("vmulps              %%xmm6, %%xmm4, %%xmm4")            // x4   = b_im = b1*f
            __ASM_EMIT("vmulps              %%xmm7, %%xmm5, %%xmm5")            // x5   = b2*f2
            __ASM_EMIT("vsubps              %%xmm2, %%xmm0, %%xmm0")            // x0   = t_re = t0 - t2*f2
            __ASM_EMIT("vsubps              %%xmm5, %%xmm3, %%xmm3")            // x3   = b_re = b0 - b2*f2
            __ASM_EMIT("vmulps              %%xmm4, %%xmm4, %%xmm5")            // x5   = b_im*b_im
            __ASM_EMIT("vmulps              %%xmm3, %%xmm3, %%xmm2")            // x2   = b_re*b_re
            __ASM_EMIT("vaddps              %%xmm5, %%xmm2, %%xmm2")            // x2   = W = b_re*b_re + b_im*b_im
            __ASM_EMIT("vmulps              %%xmm4, %%xmm0, %%xmm6")            // x6   = t_re*b_im
            __ASM_EMIT("vmulps              %%xmm4, %%xmm1, %%xmm7")            // x7   = t_im*b_im
            __ASM_EMIT("vmulps              %%xmm3, %%xmm0, %%xmm0")            // x0   = t_re*b_re
            __ASM_EMIT("vmulps              %%xmm3, %%xmm1, %%xmm1")            // x1   = t_im*b_re
            __ASM_EMIT("vaddps              %%xmm7, %%xmm0, %%xmm0")            // x0   = t_re*b_re + t_im*b_im
            __ASM_EMIT("vsubps              %%xmm6, %%xmm1, %%xmm1")            // x1   = t_im*b_re - t_re*b_im
            __ASM_EMIT("vdivps              %%xmm2, %%xmm0, %%xmm0")            // x0   = a_re = (t_re*b_re + t_im*b_im)/W
            __ASM_EMIT("vdivps              %%xmm2, %%xmm1, %%xmm1")            // x1   = b_re = (t_im*b_re - t_re*b_im)/W
            // Apply
            __ASM_EMIT("vunpcklps           %%xmm1, %%xmm0, %%xmm0")            // x0   = r0 i0 r1 i1
            __ASM_EMIT("vmovups             %%xmm0, 0x00(%[dst])")
            // Reload
            __ASM_EMIT("vmovaps             0x00 + %[fp], %%xmm0")
            __ASM_EMIT("vmovaps             0x20 + %[fp], %%xmm1")
            __ASM_EMIT("vmovaps             0x40 + %[fp], %%xmm2")
            __ASM_EMIT("vmovaps             0x60 + %[fp], %%xmm3")
            __ASM_EMIT("vmovaps             0x80 + %[fp], %%xmm4")
            __ASM_EMIT("vmovaps             0xa0 + %[fp], %%xmm5")
            __ASM_EMIT("sub                 $2, %[count]")
            __ASM_EMIT("add                 $0x08, %[f]")
            __ASM_EMIT("add                 $0x10, %[dst]")
            __ASM_EMIT("6:")
            // x1 block
            __ASM_EMIT("add                 $1, %[count]")
            __ASM_EMIT("jl                  8f")
            __ASM_EMIT("vmovss              0x00(%[f]), %%xmm6")                // x6   = f
            __ASM_EMIT("vmulps              %%xmm6, %%xmm6, %%xmm7")            // x7   = f2 = f*f
            __ASM_EMIT("vmulps              %%xmm6, %%xmm1, %%xmm1")            // x1   = t_im = t1*f
            __ASM_EMIT("vmulps              %%xmm7, %%xmm2, %%xmm2")            // x2   = t2*f2
            __ASM_EMIT("vmulps              %%xmm6, %%xmm4, %%xmm4")            // x4   = b_im = b1*f
            __ASM_EMIT("vmulps              %%xmm7, %%xmm5, %%xmm5")            // x5   = b2*f2
            __ASM_EMIT("vsubps              %%xmm2, %%xmm0, %%xmm0")            // x0   = t_re = t0 - t2*f2
            __ASM_EMIT("vsubps              %%xmm5, %%xmm3, %%xmm3")            // x3   = b_re = b0 - b2*f2
            __ASM_EMIT("vmulps              %%xmm4, %%xmm4, %%xmm5")            // x5   = b_im*b_im
            __ASM_EMIT("vmulps              %%xmm3, %%xmm3, %%xmm2")            // x2   = b_re*b_re
            __ASM_EMIT("vaddps              %%xmm5, %%xmm2, %%xmm2")            // x2   = W = b_re*b_re + b_im*b_im
            __ASM_EMIT("vmulps              %%xmm4, %%xmm0, %%xmm6")            // x6   = t_re*b_im
            __ASM_EMIT("vmulps              %%xmm4, %%xmm1, %%xmm7")            // x7   = t_im*b_im
            __ASM_EMIT("vmulps              %%xmm3, %%xmm0, %%xmm0")            // x0   = t_re*b_re
            __ASM_EMIT("vmulps              %%xmm3, %%xmm1, %%xmm1")            // x1   = t_im*b_re
            __ASM_EMIT("vaddps              %%xmm7, %%xmm0, %%xmm0")            // x0   = t_re*b_re + t_im*b_im
            __ASM_EMIT("vsubps              %%xmm6, %%xmm1, %%xmm1")            // x1   = t_im*b_re - t_re*b_im
            __ASM_EMIT("vdivps              %%xmm2, %%xmm0, %%xmm0")            // x0   = a_re = (t_re*b_re + t_im*b_im)/W
            __ASM_EMIT("vdivps              %%xmm2, %%xmm1, %%xmm1")            // x1   = b_re = (t_im*b_re - t_re*b_im)/W
            // Apply
            __ASM_EMIT("vunpcklps           %%xmm1, %%xmm0, %%xmm0")            // x0   = r0 i0
            __ASM_EMIT("vmovlps             %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("8:")

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
        IF_ARCH_X86( float fp[6*8] __lsp_aligned32; );

        ARCH_X86_ASM(
            // Unpack filter params
            __ASM_EMIT("vbroadcastss        0x00(%[c]), %%ymm0")                // y0   = t0
            __ASM_EMIT("vbroadcastss        0x04(%[c]), %%ymm1")                // y1   = t1
            __ASM_EMIT("vbroadcastss        0x08(%[c]), %%ymm2")                // y2   = t2
            __ASM_EMIT("vbroadcastss        0x10(%[c]), %%ymm3")                // y3   = b0
            __ASM_EMIT("vbroadcastss        0x14(%[c]), %%ymm4")                // y4   = b1
            __ASM_EMIT("vbroadcastss        0x18(%[c]), %%ymm5")                // y5   = b2
            __ASM_EMIT("vmovaps             %%ymm0, 0x00 + %[fp]")              // x0   = t0
            __ASM_EMIT("vmovaps             %%ymm1, 0x20 + %[fp]")              // x1   = t1
            __ASM_EMIT("vmovaps             %%ymm2, 0x40 + %[fp]")              // x2   = t2
            __ASM_EMIT("vmovaps             %%ymm3, 0x60 + %[fp]")              // x4   = b0
            __ASM_EMIT("vmovaps             %%ymm4, 0x80 + %[fp]")              // x5   = b1
            __ASM_EMIT("vmovaps             %%ymm5, 0xa0 + %[fp]")              // x6   = b2
            // x8 blocks
            __ASM_EMIT("sub                 $8, %[count]")
            __ASM_EMIT("jb                  2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vmovups             0x00(%[f]), %%ymm6")                // y6   = f
            __ASM_EMIT("vmulps              %%ymm6, %%ymm6, %%ymm7")            // y7   = f2 = f*f
            __ASM_EMIT("vmulps              %%ymm6, %%ymm1, %%ymm1")            // y1   = t_im = t1*f
            __ASM_EMIT("vmulps              %%ymm7, %%ymm2, %%ymm2")            // y2   = t2*f2
            __ASM_EMIT("vmulps              %%ymm6, %%ymm4, %%ymm4")            // y4   = b_im = b1*f
            __ASM_EMIT("vmulps              %%ymm7, %%ymm5, %%ymm5")            // y5   = b2*f2
            __ASM_EMIT("vsubps              %%ymm2, %%ymm0, %%ymm0")            // y0   = t_re = t0 - t2*f2
            __ASM_EMIT("vsubps              %%ymm5, %%ymm3, %%ymm3")            // y3   = b_re = b0 - b2*f2
            __ASM_EMIT("vmulps              %%ymm4, %%ymm4, %%ymm5")            // y5   = b_im*b_im
            __ASM_EMIT("vmulps              %%ymm3, %%ymm3, %%ymm2")            // y2   = b_re*b_re
            __ASM_EMIT("vaddps              %%ymm5, %%ymm2, %%ymm2")            // y2   = W = b_re*b_re + b_im*b_im
            __ASM_EMIT("vmulps              %%ymm4, %%ymm0, %%ymm6")            // y6   = t_re*b_im
            __ASM_EMIT("vmulps              %%ymm4, %%ymm1, %%ymm7")            // y7   = t_im*b_im
            __ASM_EMIT("vmulps              %%ymm3, %%ymm0, %%ymm0")            // y0   = t_re*b_re
            __ASM_EMIT("vmulps              %%ymm3, %%ymm1, %%ymm1")            // y1   = t_im*b_re
            __ASM_EMIT("vaddps              %%ymm7, %%ymm0, %%ymm0")            // y0   = t_re*b_re + t_im*b_im
            __ASM_EMIT("vsubps              %%ymm6, %%ymm1, %%ymm1")            // y1   = t_im*b_re - t_re*b_im
            __ASM_EMIT("vdivps              %%ymm2, %%ymm0, %%ymm0")            // y0   = a_re = (t_re*b_re + t_im*b_im)/W
            __ASM_EMIT("vdivps              %%ymm2, %%ymm1, %%ymm1")            // y1   = b_re = (t_im*b_re - t_re*b_im)/W
            // Apply
            __ASM_EMIT("vmovups             0x00(%[dst]), %%xmm2")              // x2   = r0 i0 r1 i1
            __ASM_EMIT("vmovups             0x10(%[dst]), %%xmm3")              // x3   = r2 i2 r3 i3
            __ASM_EMIT("vinsertf128         $1, 0x20(%[dst]), %%ymm2, %%ymm2")  // y2   = r0 i0 r1 i1 r4 i4 r5 i5
            __ASM_EMIT("vinsertf128         $1, 0x30(%[dst]), %%ymm3, %%ymm3")  // y3   = r2 i2 r3 i3 r6 i6 r7 i7
            __ASM_EMIT("vshufps             $0x88, %%ymm3, %%ymm2, %%ymm6")     // y6   = b_re = r0 r1 r2 r3 r4 r5 r6 r7
            __ASM_EMIT("vshufps             $0xdd, %%ymm3, %%ymm2, %%ymm7")     // y7   = b_im = i0 i1 i2 i3 i4 i5 i6 i7
            __ASM_EMIT("vmulps              %%ymm7, %%ymm0, %%ymm2")            // y2   = a_re*b_im
            __ASM_EMIT("vmulps              %%ymm7, %%ymm1, %%ymm3")            // y3   = a_im*b_im
            __ASM_EMIT("vmulps              %%ymm6, %%ymm0, %%ymm0")            // y0   = a_re*b_re
            __ASM_EMIT("vmulps              %%ymm6, %%ymm1, %%ymm1")            // y1   = a_im*b_re
            __ASM_EMIT("vsubps              %%ymm3, %%ymm0, %%ymm0")            // y0   = a_re*b_re - a_im*b_im
            __ASM_EMIT("vaddps              %%ymm2, %%ymm1, %%ymm1")            // y1   = a_im*b_re + a_re*b_im
            __ASM_EMIT("vunpcklps           %%ymm1, %%ymm0, %%ymm2")            // y2   = r0 i0 r1 i1 r4 i4 r5 i5
            __ASM_EMIT("vunpckhps           %%ymm1, %%ymm0, %%ymm3")            // y3   = r2 i2 r3 i3 r6 i6 r7 i7
            __ASM_EMIT("vmovups             %%xmm2, 0x00(%[dst])")
            __ASM_EMIT("vmovups             %%xmm3, 0x10(%[dst])")
            __ASM_EMIT("vextractf128        $1, %%ymm2, 0x20(%[dst])")
            __ASM_EMIT("vextractf128        $1, %%ymm3, 0x30(%[dst])")
            // Reload
            __ASM_EMIT("vmovaps             0x00 + %[fp], %%ymm0")
            __ASM_EMIT("vmovaps             0x20 + %[fp], %%ymm1")
            __ASM_EMIT("vmovaps             0x40 + %[fp], %%ymm2")
            __ASM_EMIT("vmovaps             0x60 + %[fp], %%ymm3")
            __ASM_EMIT("vmovaps             0x80 + %[fp], %%ymm4")
            __ASM_EMIT("vmovaps             0xa0 + %[fp], %%ymm5")
            __ASM_EMIT("add                 $0x20, %[f]")
            __ASM_EMIT("add                 $0x40, %[dst]")
            __ASM_EMIT("sub                 $8, %[count]")
            __ASM_EMIT("jae                 1b")
            __ASM_EMIT("2:")
            // x4 block
            __ASM_EMIT("add                 $4, %[count]")
            __ASM_EMIT("jl                  4f")
            __ASM_EMIT("vmovups             0x00(%[f]), %%xmm6")                // x6   = f
            __ASM_EMIT("vmulps              %%xmm6, %%xmm6, %%xmm7")            // x7   = f2 = f*f
            __ASM_EMIT("vmulps              %%xmm6, %%xmm1, %%xmm1")            // x1   = t_im = t1*f
            __ASM_EMIT("vmulps              %%xmm7, %%xmm2, %%xmm2")            // x2   = t2*f2
            __ASM_EMIT("vmulps              %%xmm6, %%xmm4, %%xmm4")            // x4   = b_im = b1*f
            __ASM_EMIT("vmulps              %%xmm7, %%xmm5, %%xmm5")            // x5   = b2*f2
            __ASM_EMIT("vsubps              %%xmm2, %%xmm0, %%xmm0")            // x0   = t_re = t0 - t2*f2
            __ASM_EMIT("vsubps              %%xmm5, %%xmm3, %%xmm3")            // x3   = b_re = b0 - b2*f2
            __ASM_EMIT("vmulps              %%xmm4, %%xmm4, %%xmm5")            // x5   = b_im*b_im
            __ASM_EMIT("vmulps              %%xmm3, %%xmm3, %%xmm2")            // x2   = b_re*b_re
            __ASM_EMIT("vaddps              %%xmm5, %%xmm2, %%xmm2")            // x2   = W = b_re*b_re + b_im*b_im
            __ASM_EMIT("vmulps              %%xmm4, %%xmm0, %%xmm6")            // x6   = t_re*b_im
            __ASM_EMIT("vmulps              %%xmm4, %%xmm1, %%xmm7")            // x7   = t_im*b_im
            __ASM_EMIT("vmulps              %%xmm3, %%xmm0, %%xmm0")            // x0   = t_re*b_re
            __ASM_EMIT("vmulps              %%xmm3, %%xmm1, %%xmm1")            // x1   = t_im*b_re
            __ASM_EMIT("vaddps              %%xmm7, %%xmm0, %%xmm0")            // x0   = t_re*b_re + t_im*b_im
            __ASM_EMIT("vsubps              %%xmm6, %%xmm1, %%xmm1")            // x1   = t_im*b_re - t_re*b_im
            __ASM_EMIT("vdivps              %%xmm2, %%xmm0, %%xmm0")            // x0   = a_re = (t_re*b_re + t_im*b_im)/W
            __ASM_EMIT("vdivps              %%xmm2, %%xmm1, %%xmm1")            // x1   = b_re = (t_im*b_re - t_re*b_im)/W
            // Apply
            __ASM_EMIT("vmovups             0x00(%[dst]), %%xmm2")              // x2   = r0 i0 r1 i1
            __ASM_EMIT("vmovups             0x10(%[dst]), %%xmm3")              // x3   = r2 i2 r3 i3
            __ASM_EMIT("vshufps             $0x88, %%xmm3, %%xmm2, %%xmm6")     // x6   = b_re = r0 r1 r2 r3 r4 r5 r6 r7
            __ASM_EMIT("vshufps             $0xdd, %%xmm3, %%xmm2, %%xmm7")     // x7   = b_im = i0 i1 i2 i3 i4 i5 i6 i7
            __ASM_EMIT("vmulps              %%xmm7, %%xmm0, %%xmm2")            // x2   = a_re*b_im
            __ASM_EMIT("vmulps              %%xmm7, %%xmm1, %%xmm3")            // x3   = a_im*b_im
            __ASM_EMIT("vmulps              %%xmm6, %%xmm0, %%xmm0")            // x0   = a_re*b_re
            __ASM_EMIT("vmulps              %%xmm6, %%xmm1, %%xmm1")            // x1   = a_im*b_re
            __ASM_EMIT("vsubps              %%xmm3, %%xmm0, %%xmm0")            // x0   = a_re*b_re - a_im*b_im
            __ASM_EMIT("vaddps              %%xmm2, %%xmm1, %%xmm1")            // x1   = a_im*b_re + a_re*b_im
            __ASM_EMIT("vunpcklps           %%xmm1, %%xmm0, %%xmm2")            // x2   = r0 i0 r1 i1 r4 i4 r5 i5
            __ASM_EMIT("vunpckhps           %%xmm1, %%xmm0, %%xmm3")            // x3   = r2 i2 r3 i3 r6 i6 r7 i7
            __ASM_EMIT("vmovups             %%xmm2, 0x00(%[dst])")
            __ASM_EMIT("vmovups             %%xmm3, 0x10(%[dst])")
            // Reload
            __ASM_EMIT("vmovaps             0x00 + %[fp], %%xmm0")
            __ASM_EMIT("vmovaps             0x20 + %[fp], %%xmm1")
            __ASM_EMIT("vmovaps             0x40 + %[fp], %%xmm2")
            __ASM_EMIT("vmovaps             0x60 + %[fp], %%xmm3")
            __ASM_EMIT("vmovaps             0x80 + %[fp], %%xmm4")
            __ASM_EMIT("vmovaps             0xa0 + %[fp], %%xmm5")
            __ASM_EMIT("sub                 $4, %[count]")
            __ASM_EMIT("add                 $0x10, %[f]")
            __ASM_EMIT("add                 $0x20, %[dst]")
            __ASM_EMIT("4:")
            // x2 block
            __ASM_EMIT("add                 $2, %[count]")
            __ASM_EMIT("jl                  6f")
            __ASM_EMIT("vmovlps             0x00(%[f]), %%xmm6, %%xmm6")        // x6   = f
            __ASM_EMIT("vmulps              %%xmm6, %%xmm6, %%xmm7")            // x7   = f2 = f*f
            __ASM_EMIT("vmulps              %%xmm6, %%xmm1, %%xmm1")            // x1   = t_im = t1*f
            __ASM_EMIT("vmulps              %%xmm7, %%xmm2, %%xmm2")            // x2   = t2*f2
            __ASM_EMIT("vmulps              %%xmm6, %%xmm4, %%xmm4")            // x4   = b_im = b1*f
            __ASM_EMIT("vmulps              %%xmm7, %%xmm5, %%xmm5")            // x5   = b2*f2
            __ASM_EMIT("vsubps              %%xmm2, %%xmm0, %%xmm0")            // x0   = t_re = t0 - t2*f2
            __ASM_EMIT("vsubps              %%xmm5, %%xmm3, %%xmm3")            // x3   = b_re = b0 - b2*f2
            __ASM_EMIT("vmulps              %%xmm4, %%xmm4, %%xmm5")            // x5   = b_im*b_im
            __ASM_EMIT("vmulps              %%xmm3, %%xmm3, %%xmm2")            // x2   = b_re*b_re
            __ASM_EMIT("vaddps              %%xmm5, %%xmm2, %%xmm2")            // x2   = W = b_re*b_re + b_im*b_im
            __ASM_EMIT("vmulps              %%xmm4, %%xmm0, %%xmm6")            // x6   = t_re*b_im
            __ASM_EMIT("vmulps              %%xmm4, %%xmm1, %%xmm7")            // x7   = t_im*b_im
            __ASM_EMIT("vmulps              %%xmm3, %%xmm0, %%xmm0")            // x0   = t_re*b_re
            __ASM_EMIT("vmulps              %%xmm3, %%xmm1, %%xmm1")            // x1   = t_im*b_re
            __ASM_EMIT("vaddps              %%xmm7, %%xmm0, %%xmm0")            // x0   = t_re*b_re + t_im*b_im
            __ASM_EMIT("vsubps              %%xmm6, %%xmm1, %%xmm1")            // x1   = t_im*b_re - t_re*b_im
            __ASM_EMIT("vdivps              %%xmm2, %%xmm0, %%xmm0")            // x0   = a_re = (t_re*b_re + t_im*b_im)/W
            __ASM_EMIT("vdivps              %%xmm2, %%xmm1, %%xmm1")            // x1   = b_re = (t_im*b_re - t_re*b_im)/W
            // Apply
            __ASM_EMIT("vmovups             0x00(%[dst]), %%xmm2")              // x2   = r0 i0 r1 i1
            __ASM_EMIT("vshufps             $0x88, %%xmm2, %%xmm2, %%xmm6")     // x6   = b_re = r0 r1 r0 r1
            __ASM_EMIT("vshufps             $0xdd, %%xmm2, %%xmm2, %%xmm7")     // x7   = b_im = i0 i1 i0 i1
            __ASM_EMIT("vmulps              %%xmm7, %%xmm0, %%xmm2")            // x2   = a_re*b_im
            __ASM_EMIT("vmulps              %%xmm7, %%xmm1, %%xmm3")            // x3   = a_im*b_im
            __ASM_EMIT("vmulps              %%xmm6, %%xmm0, %%xmm0")            // x0   = a_re*b_re
            __ASM_EMIT("vmulps              %%xmm6, %%xmm1, %%xmm1")            // x1   = a_im*b_re
            __ASM_EMIT("vsubps              %%xmm3, %%xmm0, %%xmm0")            // x0   = a_re*b_re - a_im*b_im
            __ASM_EMIT("vaddps              %%xmm2, %%xmm1, %%xmm1")            // x1   = a_im*b_re + a_re*b_im
            __ASM_EMIT("vunpcklps           %%xmm1, %%xmm0, %%xmm0")            // x0   = r0 i0 r1 i1
            __ASM_EMIT("vmovups             %%xmm0, 0x00(%[dst])")
            // Reload
            __ASM_EMIT("vmovaps             0x00 + %[fp], %%xmm0")
            __ASM_EMIT("vmovaps             0x20 + %[fp], %%xmm1")
            __ASM_EMIT("vmovaps             0x40 + %[fp], %%xmm2")
            __ASM_EMIT("vmovaps             0x60 + %[fp], %%xmm3")
            __ASM_EMIT("vmovaps             0x80 + %[fp], %%xmm4")
            __ASM_EMIT("vmovaps             0xa0 + %[fp], %%xmm5")
            __ASM_EMIT("sub                 $2, %[count]")
            __ASM_EMIT("add                 $0x08, %[f]")
            __ASM_EMIT("add                 $0x10, %[dst]")
            __ASM_EMIT("6:")
            // x1 block
            __ASM_EMIT("add                 $1, %[count]")
            __ASM_EMIT("jl                  8f")
            __ASM_EMIT("vmovss              0x00(%[f]), %%xmm6")                // x6   = f
            __ASM_EMIT("vmulps              %%xmm6, %%xmm6, %%xmm7")            // x7   = f2 = f*f
            __ASM_EMIT("vmulps              %%xmm6, %%xmm1, %%xmm1")            // x1   = t_im = t1*f
            __ASM_EMIT("vmulps              %%xmm7, %%xmm2, %%xmm2")            // x2   = t2*f2
            __ASM_EMIT("vmulps              %%xmm6, %%xmm4, %%xmm4")            // x4   = b_im = b1*f
            __ASM_EMIT("vmulps              %%xmm7, %%xmm5, %%xmm5")            // x5   = b2*f2
            __ASM_EMIT("vsubps              %%xmm2, %%xmm0, %%xmm0")            // x0   = t_re = t0 - t2*f2
            __ASM_EMIT("vsubps              %%xmm5, %%xmm3, %%xmm3")            // x3   = b_re = b0 - b2*f2
            __ASM_EMIT("vmulps              %%xmm4, %%xmm4, %%xmm5")            // x5   = b_im*b_im
            __ASM_EMIT("vmulps              %%xmm3, %%xmm3, %%xmm2")            // x2   = b_re*b_re
            __ASM_EMIT("vaddps              %%xmm5, %%xmm2, %%xmm2")            // x2   = W = b_re*b_re + b_im*b_im
            __ASM_EMIT("vmulps              %%xmm4, %%xmm0, %%xmm6")            // x6   = t_re*b_im
            __ASM_EMIT("vmulps              %%xmm4, %%xmm1, %%xmm7")            // x7   = t_im*b_im
            __ASM_EMIT("vmulps              %%xmm3, %%xmm0, %%xmm0")            // x0   = t_re*b_re
            __ASM_EMIT("vmulps              %%xmm3, %%xmm1, %%xmm1")            // x1   = t_im*b_re
            __ASM_EMIT("vaddps              %%xmm7, %%xmm0, %%xmm0")            // x0   = t_re*b_re + t_im*b_im
            __ASM_EMIT("vsubps              %%xmm6, %%xmm1, %%xmm1")            // x1   = t_im*b_re - t_re*b_im
            __ASM_EMIT("vdivps              %%xmm2, %%xmm0, %%xmm0")            // x0   = a_re = (t_re*b_re + t_im*b_im)/W
            __ASM_EMIT("vdivps              %%xmm2, %%xmm1, %%xmm1")            // x1   = b_re = (t_im*b_re - t_re*b_im)/W
            // Apply
            __ASM_EMIT("vmovss              0x00(%[dst]), %%xmm6")              // x6   = b_re
            __ASM_EMIT("vmovss              0x04(%[dst]), %%xmm7")              // x7   = b_im
            __ASM_EMIT("vmulps              %%xmm7, %%xmm0, %%xmm2")            // x2   = a_re*b_im
            __ASM_EMIT("vmulps              %%xmm7, %%xmm1, %%xmm3")            // x3   = a_im*b_im
            __ASM_EMIT("vmulps              %%xmm6, %%xmm0, %%xmm0")            // x0   = a_re*b_re
            __ASM_EMIT("vmulps              %%xmm6, %%xmm1, %%xmm1")            // x1   = a_im*b_re
            __ASM_EMIT("vsubps              %%xmm3, %%xmm0, %%xmm0")            // x0   = a_re*b_re - a_im*b_im
            __ASM_EMIT("vaddps              %%xmm2, %%xmm1, %%xmm1")            // x1   = a_im*b_re + a_re*b_im
            __ASM_EMIT("vunpcklps           %%xmm1, %%xmm0, %%xmm0")            // x0   = r0 i0
            __ASM_EMIT("vmovlps             %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("8:")

            : [dst] "+r" (dst), [f] "+r" (freq), [count] "+r" (count)
            : [c] "r" (c),
              [fp] "o" (fp)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void filter_transfer_calc_ri_fma3(float *re, float *im, const f_cascade_t *c, const float *freq, size_t count)
    {
        IF_ARCH_X86( float fp[6*8] __lsp_aligned32; );

        ARCH_X86_ASM(
            // Unpack filter params
            __ASM_EMIT("vbroadcastss        0x00(%[c]), %%ymm0")                // y0   = t0
            __ASM_EMIT("vbroadcastss        0x04(%[c]), %%ymm1")                // y1   = t1
            __ASM_EMIT("vbroadcastss        0x08(%[c]), %%ymm2")                // y2   = t2
            __ASM_EMIT("vbroadcastss        0x10(%[c]), %%ymm3")                // y3   = b0
            __ASM_EMIT("vbroadcastss        0x14(%[c]), %%ymm4")                // y4   = b1
            __ASM_EMIT("vbroadcastss        0x18(%[c]), %%ymm5")                // y5   = b2
            __ASM_EMIT("vmovaps             %%ymm0, 0x00 + %[fp]")              // x0   = t0
            __ASM_EMIT("vmovaps             %%ymm1, 0x20 + %[fp]")              // x1   = t1
            __ASM_EMIT("vmovaps             %%ymm2, 0x40 + %[fp]")              // x2   = t2
            __ASM_EMIT("vmovaps             %%ymm3, 0x60 + %[fp]")              // x4   = b0
            __ASM_EMIT("vmovaps             %%ymm4, 0x80 + %[fp]")              // x5   = b1
            __ASM_EMIT("vmovaps             %%ymm5, 0xa0 + %[fp]")              // x6   = b2
            // x8 blocks
            __ASM_EMIT("sub                 $8, %[count]")
            __ASM_EMIT("jb                  2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vmovups             0x00(%[f]), %%ymm6")                // y6   = f
            __ASM_EMIT("vmulps              %%ymm6, %%ymm6, %%ymm7")            // y7   = f2 = f*f
            __ASM_EMIT("vmulps              %%ymm6, %%ymm1, %%ymm1")            // y1   = t_im = t1*f
            __ASM_EMIT("vmulps              %%ymm6, %%ymm4, %%ymm4")            // y4   = b_im = b1*f
            __ASM_EMIT("vfnmadd231ps        %%ymm7, %%ymm2, %%ymm0")            // y0   = t_re = t0 - t2*f2
            __ASM_EMIT("vfnmadd231ps        %%ymm7, %%ymm5, %%ymm3")            // y3   = b_re = b0 - b2*f2
            __ASM_EMIT("vmulps              %%ymm4, %%ymm4, %%ymm2")            // y2   = b_im*b_im
            __ASM_EMIT("vfmadd231ps         %%ymm3, %%ymm3, %%ymm2")            // y2   = W = b_re*b_re + b_im*b_im
            __ASM_EMIT("vmulps              %%ymm4, %%ymm1, %%ymm7")            // y7   = t_im*b_im
            __ASM_EMIT("vmulps              %%ymm4, %%ymm0, %%ymm6")            // y6   = t_re*b_im
            __ASM_EMIT("vfmadd132ps         %%ymm3, %%ymm7, %%ymm0")            // y0   = t_re*b_re + t_im*b_im
            __ASM_EMIT("vfmsub132ps         %%ymm3, %%ymm6, %%ymm1")            // y1   = t_im*b_re - t_re*b_im
            __ASM_EMIT("vdivps              %%ymm2, %%ymm0, %%ymm0")            // y0   = a_re = (t_re*b_re + t_im*b_im)/W
            __ASM_EMIT("vdivps              %%ymm2, %%ymm1, %%ymm1")            // y1   = b_re = (t_im*b_re - t_re*b_im)/W
            // Apply
            __ASM_EMIT("vmovups             %%ymm0, 0x00(%[re])")
            __ASM_EMIT("vmovups             %%ymm1, 0x00(%[im])")
            // Reload
            __ASM_EMIT("vmovaps             0x00 + %[fp], %%ymm0")
            __ASM_EMIT("vmovaps             0x20 + %[fp], %%ymm1")
            __ASM_EMIT("vmovaps             0x40 + %[fp], %%ymm2")
            __ASM_EMIT("vmovaps             0x60 + %[fp], %%ymm3")
            __ASM_EMIT("vmovaps             0x80 + %[fp], %%ymm4")
            __ASM_EMIT("vmovaps             0xa0 + %[fp], %%ymm5")
            __ASM_EMIT("add                 $0x20, %[f]")
            __ASM_EMIT("add                 $0x20, %[re]")
            __ASM_EMIT("add                 $0x20, %[im]")
            __ASM_EMIT("sub                 $8, %[count]")
            __ASM_EMIT("jae                 1b")
            __ASM_EMIT("2:")
            // x4 block
            __ASM_EMIT("add                 $4, %[count]")
            __ASM_EMIT("jl                  4f")
            __ASM_EMIT("vmovups             0x00(%[f]), %%xmm6")                // x6   = f
            __ASM_EMIT("vmulps              %%xmm6, %%xmm6, %%xmm7")            // x7   = f2 = f*f
            __ASM_EMIT("vmulps              %%xmm6, %%xmm1, %%xmm1")            // x1   = t_im = t1*f
            __ASM_EMIT("vmulps              %%xmm6, %%xmm4, %%xmm4")            // x4   = b_im = b1*f
            __ASM_EMIT("vfnmadd231ps        %%xmm7, %%xmm2, %%xmm0")            // x0   = t_re = t0 - t2*f2
            __ASM_EMIT("vfnmadd231ps        %%xmm7, %%xmm5, %%xmm3")            // x3   = b_re = b0 - b2*f2
            __ASM_EMIT("vmulps              %%xmm4, %%xmm4, %%xmm2")            // x2   = b_im*b_im
            __ASM_EMIT("vfmadd231ps         %%xmm3, %%xmm3, %%xmm2")            // x2   = W = b_re*b_re + b_im*b_im
            __ASM_EMIT("vmulps              %%xmm4, %%xmm1, %%xmm7")            // x7   = t_im*b_im
            __ASM_EMIT("vmulps              %%xmm4, %%xmm0, %%xmm6")            // x6   = t_re*b_im
            __ASM_EMIT("vfmadd132ps         %%xmm3, %%xmm7, %%xmm0")            // x0   = t_re*b_re + t_im*b_im
            __ASM_EMIT("vfmsub132ps         %%xmm3, %%xmm6, %%xmm1")            // x1   = t_im*b_re - t_re*b_im
            __ASM_EMIT("vdivps              %%xmm2, %%xmm0, %%xmm0")            // x0   = a_re = (t_re*b_re + t_im*b_im)/W
            __ASM_EMIT("vdivps              %%xmm2, %%xmm1, %%xmm1")            // x1   = b_re = (t_im*b_re - t_re*b_im)/W
            // Apply
            __ASM_EMIT("vmovups             %%xmm0, 0x00(%[re])")
            __ASM_EMIT("vmovups             %%xmm1, 0x00(%[im])")
            // Reload
            __ASM_EMIT("vmovaps             0x00 + %[fp], %%xmm0")
            __ASM_EMIT("vmovaps             0x20 + %[fp], %%xmm1")
            __ASM_EMIT("vmovaps             0x40 + %[fp], %%xmm2")
            __ASM_EMIT("vmovaps             0x60 + %[fp], %%xmm3")
            __ASM_EMIT("vmovaps             0x80 + %[fp], %%xmm4")
            __ASM_EMIT("vmovaps             0xa0 + %[fp], %%xmm5")
            __ASM_EMIT("sub                 $4, %[count]")
            __ASM_EMIT("add                 $0x10, %[f]")
            __ASM_EMIT("add                 $0x10, %[re]")
            __ASM_EMIT("add                 $0x10, %[im]")
            __ASM_EMIT("4:")
            // x2 block
            __ASM_EMIT("add                 $2, %[count]")
            __ASM_EMIT("jl                  6f")
            __ASM_EMIT("vmovlps             0x00(%[f]), %%xmm6, %%xmm6")        // x6   = f
            __ASM_EMIT("vmulps              %%xmm6, %%xmm6, %%xmm7")            // x7   = f2 = f*f
            __ASM_EMIT("vmulps              %%xmm6, %%xmm1, %%xmm1")            // x1   = t_im = t1*f
            __ASM_EMIT("vmulps              %%xmm6, %%xmm4, %%xmm4")            // x4   = b_im = b1*f
            __ASM_EMIT("vfnmadd231ps        %%xmm7, %%xmm2, %%xmm0")            // x0   = t_re = t0 - t2*f2
            __ASM_EMIT("vfnmadd231ps        %%xmm7, %%xmm5, %%xmm3")            // x3   = b_re = b0 - b2*f2
            __ASM_EMIT("vmulps              %%xmm4, %%xmm4, %%xmm2")            // x2   = b_im*b_im
            __ASM_EMIT("vfmadd231ps         %%xmm3, %%xmm3, %%xmm2")            // x2   = W = b_re*b_re + b_im*b_im
            __ASM_EMIT("vmulps              %%xmm4, %%xmm1, %%xmm7")            // x7   = t_im*b_im
            __ASM_EMIT("vmulps              %%xmm4, %%xmm0, %%xmm6")            // x6   = t_re*b_im
            __ASM_EMIT("vfmadd132ps         %%xmm3, %%xmm7, %%xmm0")            // x0   = t_re*b_re + t_im*b_im
            __ASM_EMIT("vfmsub132ps         %%xmm3, %%xmm6, %%xmm1")            // x1   = t_im*b_re - t_re*b_im
            __ASM_EMIT("vdivps              %%xmm2, %%xmm0, %%xmm0")            // x0   = a_re = (t_re*b_re + t_im*b_im)/W
            __ASM_EMIT("vdivps              %%xmm2, %%xmm1, %%xmm1")            // x1   = b_re = (t_im*b_re - t_re*b_im)/W
            // Apply
            __ASM_EMIT("vmovlps             %%xmm0, 0x00(%[re])")
            __ASM_EMIT("vmovlps             %%xmm1, 0x00(%[im])")
            // Reload
            __ASM_EMIT("vmovaps             0x00 + %[fp], %%xmm0")
            __ASM_EMIT("vmovaps             0x20 + %[fp], %%xmm1")
            __ASM_EMIT("vmovaps             0x40 + %[fp], %%xmm2")
            __ASM_EMIT("vmovaps             0x60 + %[fp], %%xmm3")
            __ASM_EMIT("vmovaps             0x80 + %[fp], %%xmm4")
            __ASM_EMIT("vmovaps             0xa0 + %[fp], %%xmm5")
            __ASM_EMIT("sub                 $2, %[count]")
            __ASM_EMIT("add                 $0x08, %[f]")
            __ASM_EMIT("add                 $0x08, %[re]")
            __ASM_EMIT("add                 $0x08, %[im]")
            __ASM_EMIT("6:")
            // x1 block
            __ASM_EMIT("add                 $1, %[count]")
            __ASM_EMIT("jl                  8f")
            __ASM_EMIT("vmovss              0x00(%[f]), %%xmm6")                // x6   = f
            __ASM_EMIT("vmulps              %%xmm6, %%xmm6, %%xmm7")            // x7   = f2 = f*f
            __ASM_EMIT("vmulps              %%xmm6, %%xmm1, %%xmm1")            // x1   = t_im = t1*f
            __ASM_EMIT("vmulps              %%xmm6, %%xmm4, %%xmm4")            // x4   = b_im = b1*f
            __ASM_EMIT("vfnmadd231ps        %%xmm7, %%xmm2, %%xmm0")            // x0   = t_re = t0 - t2*f2
            __ASM_EMIT("vfnmadd231ps        %%xmm7, %%xmm5, %%xmm3")            // x3   = b_re = b0 - b2*f2
            __ASM_EMIT("vmulps              %%xmm4, %%xmm4, %%xmm2")            // x2   = b_im*b_im
            __ASM_EMIT("vfmadd231ps         %%xmm3, %%xmm3, %%xmm2")            // x2   = W = b_re*b_re + b_im*b_im
            __ASM_EMIT("vmulps              %%xmm4, %%xmm1, %%xmm7")            // x7   = t_im*b_im
            __ASM_EMIT("vmulps              %%xmm4, %%xmm0, %%xmm6")            // x6   = t_re*b_im
            __ASM_EMIT("vfmadd132ps         %%xmm3, %%xmm7, %%xmm0")            // x0   = t_re*b_re + t_im*b_im
            __ASM_EMIT("vfmsub132ps         %%xmm3, %%xmm6, %%xmm1")            // x1   = t_im*b_re - t_re*b_im
            __ASM_EMIT("vdivps              %%xmm2, %%xmm0, %%xmm0")            // x0   = a_re = (t_re*b_re + t_im*b_im)/W
            __ASM_EMIT("vdivps              %%xmm2, %%xmm1, %%xmm1")            // x1   = b_re = (t_im*b_re - t_re*b_im)/W
            // Apply
            __ASM_EMIT("vmovss              %%xmm0, 0x00(%[re])")
            __ASM_EMIT("vmovss              %%xmm1, 0x00(%[im])")
            __ASM_EMIT("8:")

            : [re] "+r" (re), [im] "+r" (im), [f] "+r" (freq), [count] "+r" (count)
            : [c] "r" (c),
              [fp] "o" (fp)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void filter_transfer_apply_ri_fma3(float *re, float *im, const f_cascade_t *c, const float *freq, size_t count)
    {
        IF_ARCH_X86( float fp[6*8] __lsp_aligned32; );

        ARCH_X86_ASM(
            // Unpack filter params
            __ASM_EMIT("vbroadcastss        0x00(%[c]), %%ymm0")                // y0   = t0
            __ASM_EMIT("vbroadcastss        0x04(%[c]), %%ymm1")                // y1   = t1
            __ASM_EMIT("vbroadcastss        0x08(%[c]), %%ymm2")                // y2   = t2
            __ASM_EMIT("vbroadcastss        0x10(%[c]), %%ymm3")                // y3   = b0
            __ASM_EMIT("vbroadcastss        0x14(%[c]), %%ymm4")                // y4   = b1
            __ASM_EMIT("vbroadcastss        0x18(%[c]), %%ymm5")                // y5   = b2
            __ASM_EMIT("vmovaps             %%ymm0, 0x00 + %[fp]")              // x0   = t0
            __ASM_EMIT("vmovaps             %%ymm1, 0x20 + %[fp]")              // x1   = t1
            __ASM_EMIT("vmovaps             %%ymm2, 0x40 + %[fp]")              // x2   = t2
            __ASM_EMIT("vmovaps             %%ymm3, 0x60 + %[fp]")              // x4   = b0
            __ASM_EMIT("vmovaps             %%ymm4, 0x80 + %[fp]")              // x5   = b1
            __ASM_EMIT("vmovaps             %%ymm5, 0xa0 + %[fp]")              // x6   = b2
            // x8 blocks
            __ASM_EMIT("sub                 $8, %[count]")
            __ASM_EMIT("jb                  2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vmovups             0x00(%[f]), %%ymm6")                // y6   = f
            __ASM_EMIT("vmulps              %%ymm6, %%ymm6, %%ymm7")            // y7   = f2 = f*f
            __ASM_EMIT("vmulps              %%ymm6, %%ymm1, %%ymm1")            // y1   = t_im = t1*f
            __ASM_EMIT("vmulps              %%ymm6, %%ymm4, %%ymm4")            // y4   = b_im = b1*f
            __ASM_EMIT("vfnmadd231ps        %%ymm7, %%ymm2, %%ymm0")            // y0   = t_re = t0 - t2*f2
            __ASM_EMIT("vfnmadd231ps        %%ymm7, %%ymm5, %%ymm3")            // y3   = b_re = b0 - b2*f2
            __ASM_EMIT("vmulps              %%ymm4, %%ymm4, %%ymm2")            // y2   = b_im*b_im
            __ASM_EMIT("vfmadd231ps         %%ymm3, %%ymm3, %%ymm2")            // y2   = W = b_re*b_re + b_im*b_im
            __ASM_EMIT("vmulps              %%ymm4, %%ymm1, %%ymm7")            // y7   = t_im*b_im
            __ASM_EMIT("vmulps              %%ymm4, %%ymm0, %%ymm6")            // y6   = t_re*b_im
            __ASM_EMIT("vfmadd132ps         %%ymm3, %%ymm7, %%ymm0")            // y0   = t_re*b_re + t_im*b_im
            __ASM_EMIT("vfmsub132ps         %%ymm3, %%ymm6, %%ymm1")            // y1   = t_im*b_re - t_re*b_im
            __ASM_EMIT("vdivps              %%ymm2, %%ymm0, %%ymm0")            // y0   = a_re = (t_re*b_re + t_im*b_im)/W
            __ASM_EMIT("vdivps              %%ymm2, %%ymm1, %%ymm1")            // y1   = b_re = (t_im*b_re - t_re*b_im)/W
            // Apply
            __ASM_EMIT("vmulps              0x00(%[im]), %%ymm1, %%ymm3")       // y3   = a_im*b_im
            __ASM_EMIT("vmulps              0x00(%[im]), %%ymm0, %%ymm2")       // y2   = a_re*b_im
            __ASM_EMIT("vfmsub132ps         0x00(%[re]), %%ymm3, %%ymm0")       // y0   = a_re*b_re - a_im*b_im
            __ASM_EMIT("vfmadd132ps         0x00(%[re]), %%ymm2, %%ymm1")       // y1   = a_im*b_re + a_re*b_im
            __ASM_EMIT("vmovups             %%ymm0, 0x00(%[re])")
            __ASM_EMIT("vmovups             %%ymm1, 0x00(%[im])")
            // Reload
            __ASM_EMIT("vmovaps             0x00 + %[fp], %%ymm0")
            __ASM_EMIT("vmovaps             0x20 + %[fp], %%ymm1")
            __ASM_EMIT("vmovaps             0x40 + %[fp], %%ymm2")
            __ASM_EMIT("vmovaps             0x60 + %[fp], %%ymm3")
            __ASM_EMIT("vmovaps             0x80 + %[fp], %%ymm4")
            __ASM_EMIT("vmovaps             0xa0 + %[fp], %%ymm5")
            __ASM_EMIT("add                 $0x20, %[f]")
            __ASM_EMIT("add                 $0x20, %[re]")
            __ASM_EMIT("add                 $0x20, %[im]")
            __ASM_EMIT("sub                 $8, %[count]")
            __ASM_EMIT("jae                 1b")
            __ASM_EMIT("2:")
            // x4 block
            __ASM_EMIT("add                 $4, %[count]")
            __ASM_EMIT("jl                  4f")
            __ASM_EMIT("vmovups             0x00(%[f]), %%xmm6")                // x6   = f
            __ASM_EMIT("vmulps              %%xmm6, %%xmm6, %%xmm7")            // x7   = f2 = f*f
            __ASM_EMIT("vmulps              %%xmm6, %%xmm1, %%xmm1")            // x1   = t_im = t1*f
            __ASM_EMIT("vmulps              %%xmm6, %%xmm4, %%xmm4")            // x4   = b_im = b1*f
            __ASM_EMIT("vfnmadd231ps        %%xmm7, %%xmm2, %%xmm0")            // x0   = t_re = t0 - t2*f2
            __ASM_EMIT("vfnmadd231ps        %%xmm7, %%xmm5, %%xmm3")            // x3   = b_re = b0 - b2*f2
            __ASM_EMIT("vmulps              %%xmm4, %%xmm4, %%xmm2")            // x2   = b_im*b_im
            __ASM_EMIT("vfmadd231ps         %%xmm3, %%xmm3, %%xmm2")            // x2   = W = b_re*b_re + b_im*b_im
            __ASM_EMIT("vmulps              %%xmm4, %%xmm1, %%xmm7")            // x7   = t_im*b_im
            __ASM_EMIT("vmulps              %%xmm4, %%xmm0, %%xmm6")            // x6   = t_re*b_im
            __ASM_EMIT("vfmadd132ps         %%xmm3, %%xmm7, %%xmm0")            // x0   = t_re*b_re + t_im*b_im
            __ASM_EMIT("vfmsub132ps         %%xmm3, %%xmm6, %%xmm1")            // x1   = t_im*b_re - t_re*b_im
            __ASM_EMIT("vdivps              %%xmm2, %%xmm0, %%xmm0")            // x0   = a_re = (t_re*b_re + t_im*b_im)/W
            __ASM_EMIT("vdivps              %%xmm2, %%xmm1, %%xmm1")            // x1   = b_re = (t_im*b_re - t_re*b_im)/W
            // Apply
            __ASM_EMIT("vmulps              0x00(%[im]), %%xmm1, %%xmm3")       // x3   = a_im*b_im
            __ASM_EMIT("vmulps              0x00(%[im]), %%xmm0, %%xmm2")       // x2   = a_re*b_im
            __ASM_EMIT("vfmsub132ps         0x00(%[re]), %%xmm3, %%xmm0")       // x0   = a_re*b_re - a_im*b_im
            __ASM_EMIT("vfmadd132ps         0x00(%[re]), %%xmm2, %%xmm1")       // x1   = a_im*b_re + a_re*b_im
            __ASM_EMIT("vmovups             %%xmm0, 0x00(%[re])")
            __ASM_EMIT("vmovups             %%xmm1, 0x00(%[im])")
            // Reload
            __ASM_EMIT("vmovaps             0x00 + %[fp], %%xmm0")
            __ASM_EMIT("vmovaps             0x20 + %[fp], %%xmm1")
            __ASM_EMIT("vmovaps             0x40 + %[fp], %%xmm2")
            __ASM_EMIT("vmovaps             0x60 + %[fp], %%xmm3")
            __ASM_EMIT("vmovaps             0x80 + %[fp], %%xmm4")
            __ASM_EMIT("vmovaps             0xa0 + %[fp], %%xmm5")
            __ASM_EMIT("sub                 $4, %[count]")
            __ASM_EMIT("add                 $0x10, %[f]")
            __ASM_EMIT("add                 $0x10, %[re]")
            __ASM_EMIT("add                 $0x10, %[im]")
            __ASM_EMIT("4:")
            // x2 block
            __ASM_EMIT("add                 $2, %[count]")
            __ASM_EMIT("jl                  6f")
            __ASM_EMIT("vmovlps             0x00(%[f]), %%xmm6, %%xmm6")        // x6   = f
            __ASM_EMIT("vmulps              %%xmm6, %%xmm6, %%xmm7")            // x7   = f2 = f*f
            __ASM_EMIT("vmulps              %%xmm6, %%xmm1, %%xmm1")            // x1   = t_im = t1*f
            __ASM_EMIT("vmulps              %%xmm6, %%xmm4, %%xmm4")            // x4   = b_im = b1*f
            __ASM_EMIT("vfnmadd231ps        %%xmm7, %%xmm2, %%xmm0")            // x0   = t_re = t0 - t2*f2
            __ASM_EMIT("vfnmadd231ps        %%xmm7, %%xmm5, %%xmm3")            // x3   = b_re = b0 - b2*f2
            __ASM_EMIT("vmulps              %%xmm4, %%xmm4, %%xmm2")            // x2   = b_im*b_im
            __ASM_EMIT("vfmadd231ps         %%xmm3, %%xmm3, %%xmm2")            // x2   = W = b_re*b_re + b_im*b_im
            __ASM_EMIT("vmulps              %%xmm4, %%xmm1, %%xmm7")            // x7   = t_im*b_im
            __ASM_EMIT("vmulps              %%xmm4, %%xmm0, %%xmm6")            // x6   = t_re*b_im
            __ASM_EMIT("vfmadd132ps         %%xmm3, %%xmm7, %%xmm0")            // x0   = t_re*b_re + t_im*b_im
            __ASM_EMIT("vfmsub132ps         %%xmm3, %%xmm6, %%xmm1")            // x1   = t_im*b_re - t_re*b_im
            __ASM_EMIT("vdivps              %%xmm2, %%xmm0, %%xmm0")            // x0   = a_re = (t_re*b_re + t_im*b_im)/W
            __ASM_EMIT("vdivps              %%xmm2, %%xmm1, %%xmm1")            // x1   = b_re = (t_im*b_re - t_re*b_im)/W
            // Apply
            __ASM_EMIT("vmovlps             0x00(%[im]), %%xmm7, %%xmm7")
            __ASM_EMIT("vmovlps             0x00(%[re]), %%xmm6, %%xmm6")
            __ASM_EMIT("vmulps              %%xmm7, %%xmm1, %%xmm3")            // x3   = a_im*b_im
            __ASM_EMIT("vmulps              %%xmm7, %%xmm0, %%xmm2")            // x2   = a_re*b_im
            __ASM_EMIT("vfmsub132ps         %%xmm6, %%xmm3, %%xmm0")            // x0   = a_re*b_re - a_im*b_im
            __ASM_EMIT("vfmadd132ps         %%xmm6, %%xmm2, %%xmm1")            // x1   = a_im*b_re + a_re*b_im
            __ASM_EMIT("vmovlps             %%xmm0, 0x00(%[re])")
            __ASM_EMIT("vmovlps             %%xmm1, 0x00(%[im])")
            // Reload
            __ASM_EMIT("vmovaps             0x00 + %[fp], %%xmm0")
            __ASM_EMIT("vmovaps             0x20 + %[fp], %%xmm1")
            __ASM_EMIT("vmovaps             0x40 + %[fp], %%xmm2")
            __ASM_EMIT("vmovaps             0x60 + %[fp], %%xmm3")
            __ASM_EMIT("vmovaps             0x80 + %[fp], %%xmm4")
            __ASM_EMIT("vmovaps             0xa0 + %[fp], %%xmm5")
            __ASM_EMIT("sub                 $2, %[count]")
            __ASM_EMIT("add                 $0x08, %[f]")
            __ASM_EMIT("add                 $0x08, %[re]")
            __ASM_EMIT("add                 $0x08, %[im]")
            __ASM_EMIT("6:")
            // x1 block
            __ASM_EMIT("add                 $1, %[count]")
            __ASM_EMIT("jl                  8f")
            __ASM_EMIT("vmovss              0x00(%[f]), %%xmm6")                // x6   = f
            __ASM_EMIT("vmulps              %%xmm6, %%xmm6, %%xmm7")            // x7   = f2 = f*f
            __ASM_EMIT("vmulps              %%xmm6, %%xmm1, %%xmm1")            // x1   = t_im = t1*f
            __ASM_EMIT("vmulps              %%xmm6, %%xmm4, %%xmm4")            // x4   = b_im = b1*f
            __ASM_EMIT("vfnmadd231ps        %%xmm7, %%xmm2, %%xmm0")            // x0   = t_re = t0 - t2*f2
            __ASM_EMIT("vfnmadd231ps        %%xmm7, %%xmm5, %%xmm3")            // x3   = b_re = b0 - b2*f2
            __ASM_EMIT("vmulps              %%xmm4, %%xmm4, %%xmm2")            // x2   = b_im*b_im
            __ASM_EMIT("vfmadd231ps         %%xmm3, %%xmm3, %%xmm2")            // x2   = W = b_re*b_re + b_im*b_im
            __ASM_EMIT("vmulps              %%xmm4, %%xmm1, %%xmm7")            // x7   = t_im*b_im
            __ASM_EMIT("vmulps              %%xmm4, %%xmm0, %%xmm6")            // x6   = t_re*b_im
            __ASM_EMIT("vfmadd132ps         %%xmm3, %%xmm7, %%xmm0")            // x0   = t_re*b_re + t_im*b_im
            __ASM_EMIT("vfmsub132ps         %%xmm3, %%xmm6, %%xmm1")            // x1   = t_im*b_re - t_re*b_im
            __ASM_EMIT("vdivps              %%xmm2, %%xmm0, %%xmm0")            // x0   = a_re = (t_re*b_re + t_im*b_im)/W
            __ASM_EMIT("vdivps              %%xmm2, %%xmm1, %%xmm1")            // x1   = b_re = (t_im*b_re - t_re*b_im)/W
            // Apply
            __ASM_EMIT("vmovss              0x00(%[im]), %%xmm7")
            __ASM_EMIT("vmovss              0x00(%[re]), %%xmm6")
            __ASM_EMIT("vmulps              %%xmm7, %%xmm1, %%xmm3")            // x3   = a_im*b_im
            __ASM_EMIT("vmulps              %%xmm7, %%xmm0, %%xmm2")            // x2   = a_re*b_im
            __ASM_EMIT("vfmsub132ps         %%xmm6, %%xmm3, %%xmm0")            // x0   = a_re*b_re - a_im*b_im
            __ASM_EMIT("vfmadd132ps         %%xmm6, %%xmm2, %%xmm1")            // x1   = a_im*b_re + a_re*b_im
            __ASM_EMIT("vmovss              %%xmm0, 0x00(%[re])")
            __ASM_EMIT("vmovss              %%xmm1, 0x00(%[im])")
            __ASM_EMIT("8:")

            : [re] "+r" (re), [im] "+r" (im), [f] "+r" (freq), [count] "+r" (count)
            : [c] "r" (c),
              [fp] "o" (fp)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void filter_transfer_calc_pc_fma3(float *dst, const f_cascade_t *c, const float *freq, size_t count)
    {
        IF_ARCH_X86( float fp[6*8] __lsp_aligned32; );

        ARCH_X86_ASM(
            // Unpack filter params
            __ASM_EMIT("vbroadcastss        0x00(%[c]), %%ymm0")                // y0   = t0
            __ASM_EMIT("vbroadcastss        0x04(%[c]), %%ymm1")                // y1   = t1
            __ASM_EMIT("vbroadcastss        0x08(%[c]), %%ymm2")                // y2   = t2
            __ASM_EMIT("vbroadcastss        0x10(%[c]), %%ymm3")                // y3   = b0
            __ASM_EMIT("vbroadcastss        0x14(%[c]), %%ymm4")                // y4   = b1
            __ASM_EMIT("vbroadcastss        0x18(%[c]), %%ymm5")                // y5   = b2
            __ASM_EMIT("vmovaps             %%ymm0, 0x00 + %[fp]")              // x0   = t0
            __ASM_EMIT("vmovaps             %%ymm1, 0x20 + %[fp]")              // x1   = t1
            __ASM_EMIT("vmovaps             %%ymm2, 0x40 + %[fp]")              // x2   = t2
            __ASM_EMIT("vmovaps             %%ymm3, 0x60 + %[fp]")              // x4   = b0
            __ASM_EMIT("vmovaps             %%ymm4, 0x80 + %[fp]")              // x5   = b1
            __ASM_EMIT("vmovaps             %%ymm5, 0xa0 + %[fp]")              // x6   = b2
            // x8 blocks
            __ASM_EMIT("sub                 $8, %[count]")
            __ASM_EMIT("jb                  2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vmovups             0x00(%[f]), %%ymm6")                // y6   = f
            __ASM_EMIT("vmulps              %%ymm6, %%ymm6, %%ymm7")            // y7   = f2 = f*f
            __ASM_EMIT("vmulps              %%ymm6, %%ymm1, %%ymm1")            // y1   = t_im = t1*f
            __ASM_EMIT("vmulps              %%ymm6, %%ymm4, %%ymm4")            // y4   = b_im = b1*f
            __ASM_EMIT("vfnmadd231ps        %%ymm7, %%ymm2, %%ymm0")            // y0   = t_re = t0 - t2*f2
            __ASM_EMIT("vfnmadd231ps        %%ymm7, %%ymm5, %%ymm3")            // y3   = b_re = b0 - b2*f2
            __ASM_EMIT("vmulps              %%ymm4, %%ymm4, %%ymm2")            // y2   = b_im*b_im
            __ASM_EMIT("vfmadd231ps         %%ymm3, %%ymm3, %%ymm2")            // y2   = W = b_re*b_re + b_im*b_im
            __ASM_EMIT("vmulps              %%ymm4, %%ymm1, %%ymm7")            // y7   = t_im*b_im
            __ASM_EMIT("vmulps              %%ymm4, %%ymm0, %%ymm6")            // y6   = t_re*b_im
            __ASM_EMIT("vfmadd132ps         %%ymm3, %%ymm7, %%ymm0")            // y0   = t_re*b_re + t_im*b_im
            __ASM_EMIT("vfmsub132ps         %%ymm3, %%ymm6, %%ymm1")            // y1   = t_im*b_re - t_re*b_im
            __ASM_EMIT("vdivps              %%ymm2, %%ymm0, %%ymm0")            // y0   = a_re = (t_re*b_re + t_im*b_im)/W
            __ASM_EMIT("vdivps              %%ymm2, %%ymm1, %%ymm1")            // y1   = b_re = (t_im*b_re - t_re*b_im)/W
            // Apply
            __ASM_EMIT("vunpcklps           %%ymm1, %%ymm0, %%ymm2")            // y2   = r0 i0 r1 i1 r4 i4 r5 i5
            __ASM_EMIT("vunpckhps           %%ymm1, %%ymm0, %%ymm3")            // y3   = r2 i2 r3 i3 r6 i6 r7 i7
            __ASM_EMIT("vmovups             %%xmm2, 0x00(%[dst])")
            __ASM_EMIT("vmovups             %%xmm3, 0x10(%[dst])")
            __ASM_EMIT("vextractf128        $1, %%ymm2, 0x20(%[dst])")
            __ASM_EMIT("vextractf128        $1, %%ymm3, 0x30(%[dst])")
            // Reload
            __ASM_EMIT("vmovaps             0x00 + %[fp], %%ymm0")
            __ASM_EMIT("vmovaps             0x20 + %[fp], %%ymm1")
            __ASM_EMIT("vmovaps             0x40 + %[fp], %%ymm2")
            __ASM_EMIT("vmovaps             0x60 + %[fp], %%ymm3")
            __ASM_EMIT("vmovaps             0x80 + %[fp], %%ymm4")
            __ASM_EMIT("vmovaps             0xa0 + %[fp], %%ymm5")
            __ASM_EMIT("add                 $0x20, %[f]")
            __ASM_EMIT("add                 $0x40, %[dst]")
            __ASM_EMIT("sub                 $8, %[count]")
            __ASM_EMIT("jae                 1b")
            __ASM_EMIT("2:")
            // x4 block
            __ASM_EMIT("add                 $4, %[count]")
            __ASM_EMIT("jl                  4f")
            __ASM_EMIT("vmovups             0x00(%[f]), %%xmm6")                // x6   = f
            __ASM_EMIT("vmulps              %%xmm6, %%xmm6, %%xmm7")            // x7   = f2 = f*f
            __ASM_EMIT("vmulps              %%xmm6, %%xmm1, %%xmm1")            // x1   = t_im = t1*f
            __ASM_EMIT("vmulps              %%xmm6, %%xmm4, %%xmm4")            // x4   = b_im = b1*f
            __ASM_EMIT("vfnmadd231ps        %%xmm7, %%xmm2, %%xmm0")            // x0   = t_re = t0 - t2*f2
            __ASM_EMIT("vfnmadd231ps        %%xmm7, %%xmm5, %%xmm3")            // x3   = b_re = b0 - b2*f2
            __ASM_EMIT("vmulps              %%xmm4, %%xmm4, %%xmm2")            // x2   = b_im*b_im
            __ASM_EMIT("vfmadd231ps         %%xmm3, %%xmm3, %%xmm2")            // x2   = W = b_re*b_re + b_im*b_im
            __ASM_EMIT("vmulps              %%xmm4, %%xmm1, %%xmm7")            // x7   = t_im*b_im
            __ASM_EMIT("vmulps              %%xmm4, %%xmm0, %%xmm6")            // x6   = t_re*b_im
            __ASM_EMIT("vfmadd132ps         %%xmm3, %%xmm7, %%xmm0")            // x0   = t_re*b_re + t_im*b_im
            __ASM_EMIT("vfmsub132ps         %%xmm3, %%xmm6, %%xmm1")            // x1   = t_im*b_re - t_re*b_im
            __ASM_EMIT("vdivps              %%xmm2, %%xmm0, %%xmm0")            // x0   = a_re = (t_re*b_re + t_im*b_im)/W
            __ASM_EMIT("vdivps              %%xmm2, %%xmm1, %%xmm1")            // x1   = b_re = (t_im*b_re - t_re*b_im)/W
            // Apply
            __ASM_EMIT("vunpcklps           %%xmm1, %%xmm0, %%xmm2")            // x2   = r0 i0 r1 i1 r4 i4 r5 i5
            __ASM_EMIT("vunpckhps           %%xmm1, %%xmm0, %%xmm3")            // x3   = r2 i2 r3 i3 r6 i6 r7 i7
            __ASM_EMIT("vmovups             %%xmm2, 0x00(%[dst])")
            __ASM_EMIT("vmovups             %%xmm3, 0x10(%[dst])")
            // Reload
            __ASM_EMIT("vmovaps             0x00 + %[fp], %%xmm0")
            __ASM_EMIT("vmovaps             0x20 + %[fp], %%xmm1")
            __ASM_EMIT("vmovaps             0x40 + %[fp], %%xmm2")
            __ASM_EMIT("vmovaps             0x60 + %[fp], %%xmm3")
            __ASM_EMIT("vmovaps             0x80 + %[fp], %%xmm4")
            __ASM_EMIT("vmovaps             0xa0 + %[fp], %%xmm5")
            __ASM_EMIT("sub                 $4, %[count]")
            __ASM_EMIT("add                 $0x10, %[f]")
            __ASM_EMIT("add                 $0x20, %[dst]")
            __ASM_EMIT("4:")
            // x2 block
            __ASM_EMIT("add                 $2, %[count]")
            __ASM_EMIT("jl                  6f")
            __ASM_EMIT("vmovlps             0x00(%[f]), %%xmm6, %%xmm6")        // x6   = f
            __ASM_EMIT("vmulps              %%xmm6, %%xmm6, %%xmm7")            // x7   = f2 = f*f
            __ASM_EMIT("vmulps              %%xmm6, %%xmm1, %%xmm1")            // x1   = t_im = t1*f
            __ASM_EMIT("vmulps              %%xmm6, %%xmm4, %%xmm4")            // x4   = b_im = b1*f
            __ASM_EMIT("vfnmadd231ps        %%xmm7, %%xmm2, %%xmm0")            // x0   = t_re = t0 - t2*f2
            __ASM_EMIT("vfnmadd231ps        %%xmm7, %%xmm5, %%xmm3")            // x3   = b_re = b0 - b2*f2
            __ASM_EMIT("vmulps              %%xmm4, %%xmm4, %%xmm2")            // x2   = b_im*b_im
            __ASM_EMIT("vfmadd231ps         %%xmm3, %%xmm3, %%xmm2")            // x2   = W = b_re*b_re + b_im*b_im
            __ASM_EMIT("vmulps              %%xmm4, %%xmm1, %%xmm7")            // x7   = t_im*b_im
            __ASM_EMIT("vmulps              %%xmm4, %%xmm0, %%xmm6")            // x6   = t_re*b_im
            __ASM_EMIT("vfmadd132ps         %%xmm3, %%xmm7, %%xmm0")            // x0   = t_re*b_re + t_im*b_im
            __ASM_EMIT("vfmsub132ps         %%xmm3, %%xmm6, %%xmm1")            // x1   = t_im*b_re - t_re*b_im
            __ASM_EMIT("vdivps              %%xmm2, %%xmm0, %%xmm0")            // x0   = a_re = (t_re*b_re + t_im*b_im)/W
            __ASM_EMIT("vdivps              %%xmm2, %%xmm1, %%xmm1")            // x1   = b_re = (t_im*b_re - t_re*b_im)/W
            // Apply
            __ASM_EMIT("vunpcklps           %%xmm1, %%xmm0, %%xmm0")            // x0   = r0 i0 r1 i1
            __ASM_EMIT("vmovups             %%xmm0, 0x00(%[dst])")
            // Reload
            __ASM_EMIT("vmovaps             0x00 + %[fp], %%xmm0")
            __ASM_EMIT("vmovaps             0x20 + %[fp], %%xmm1")
            __ASM_EMIT("vmovaps             0x40 + %[fp], %%xmm2")
            __ASM_EMIT("vmovaps             0x60 + %[fp], %%xmm3")
            __ASM_EMIT("vmovaps             0x80 + %[fp], %%xmm4")
            __ASM_EMIT("vmovaps             0xa0 + %[fp], %%xmm5")
            __ASM_EMIT("sub                 $2, %[count]")
            __ASM_EMIT("add                 $0x08, %[f]")
            __ASM_EMIT("add                 $0x10, %[dst]")
            __ASM_EMIT("6:")
            // x1 block
            __ASM_EMIT("add                 $1, %[count]")
            __ASM_EMIT("jl                  8f")
            __ASM_EMIT("vmovss              0x00(%[f]), %%xmm6")                // x6   = f
            __ASM_EMIT("vmulps              %%xmm6, %%xmm6, %%xmm7")            // x7   = f2 = f*f
            __ASM_EMIT("vmulps              %%xmm6, %%xmm1, %%xmm1")            // x1   = t_im = t1*f
            __ASM_EMIT("vmulps              %%xmm6, %%xmm4, %%xmm4")            // x4   = b_im = b1*f
            __ASM_EMIT("vfnmadd231ps        %%xmm7, %%xmm2, %%xmm0")            // x0   = t_re = t0 - t2*f2
            __ASM_EMIT("vfnmadd231ps        %%xmm7, %%xmm5, %%xmm3")            // x3   = b_re = b0 - b2*f2
            __ASM_EMIT("vmulps              %%xmm4, %%xmm4, %%xmm2")            // x2   = b_im*b_im
            __ASM_EMIT("vfmadd231ps         %%xmm3, %%xmm3, %%xmm2")            // x2   = W = b_re*b_re + b_im*b_im
            __ASM_EMIT("vmulps              %%xmm4, %%xmm1, %%xmm7")            // x7   = t_im*b_im
            __ASM_EMIT("vmulps              %%xmm4, %%xmm0, %%xmm6")            // x6   = t_re*b_im
            __ASM_EMIT("vfmadd132ps         %%xmm3, %%xmm7, %%xmm0")            // x0   = t_re*b_re + t_im*b_im
            __ASM_EMIT("vfmsub132ps         %%xmm3, %%xmm6, %%xmm1")            // x1   = t_im*b_re - t_re*b_im
            __ASM_EMIT("vdivps              %%xmm2, %%xmm0, %%xmm0")            // x0   = a_re = (t_re*b_re + t_im*b_im)/W
            __ASM_EMIT("vdivps              %%xmm2, %%xmm1, %%xmm1")            // x1   = b_re = (t_im*b_re - t_re*b_im)/W
            // Apply
            __ASM_EMIT("vunpcklps           %%xmm1, %%xmm0, %%xmm0")            // x0   = r0 i0
            __ASM_EMIT("vmovlps             %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("8:")

            : [dst] "+r" (dst), [f] "+r" (freq), [count] "+r" (count)
            : [c] "r" (c),
              [fp] "o" (fp)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void filter_transfer_apply_pc_fma3(float *dst, const f_cascade_t *c, const float *freq, size_t count)
    {
        IF_ARCH_X86( float fp[6*8] __lsp_aligned32; );

        ARCH_X86_ASM(
            // Unpack filter params
            __ASM_EMIT("vbroadcastss        0x00(%[c]), %%ymm0")                // y0   = t0
            __ASM_EMIT("vbroadcastss        0x04(%[c]), %%ymm1")                // y1   = t1
            __ASM_EMIT("vbroadcastss        0x08(%[c]), %%ymm2")                // y2   = t2
            __ASM_EMIT("vbroadcastss        0x10(%[c]), %%ymm3")                // y3   = b0
            __ASM_EMIT("vbroadcastss        0x14(%[c]), %%ymm4")                // y4   = b1
            __ASM_EMIT("vbroadcastss        0x18(%[c]), %%ymm5")                // y5   = b2
            __ASM_EMIT("vmovaps             %%ymm0, 0x00 + %[fp]")              // x0   = t0
            __ASM_EMIT("vmovaps             %%ymm1, 0x20 + %[fp]")              // x1   = t1
            __ASM_EMIT("vmovaps             %%ymm2, 0x40 + %[fp]")              // x2   = t2
            __ASM_EMIT("vmovaps             %%ymm3, 0x60 + %[fp]")              // x4   = b0
            __ASM_EMIT("vmovaps             %%ymm4, 0x80 + %[fp]")              // x5   = b1
            __ASM_EMIT("vmovaps             %%ymm5, 0xa0 + %[fp]")              // x6   = b2
            // x8 blocks
            __ASM_EMIT("sub                 $8, %[count]")
            __ASM_EMIT("jb                  2f")
            __ASM_EMIT("1:")
            __ASM_EMIT("vmovups             0x00(%[f]), %%ymm6")                // y6   = f
            __ASM_EMIT("vmulps              %%ymm6, %%ymm6, %%ymm7")            // y7   = f2 = f*f
            __ASM_EMIT("vmulps              %%ymm6, %%ymm1, %%ymm1")            // y1   = t_im = t1*f
            __ASM_EMIT("vmulps              %%ymm6, %%ymm4, %%ymm4")            // y4   = b_im = b1*f
            __ASM_EMIT("vfnmadd231ps        %%ymm7, %%ymm2, %%ymm0")            // y0   = t_re = t0 - t2*f2
            __ASM_EMIT("vfnmadd231ps        %%ymm7, %%ymm5, %%ymm3")            // y3   = b_re = b0 - b2*f2
            __ASM_EMIT("vmulps              %%ymm4, %%ymm4, %%ymm2")            // y2   = b_im*b_im
            __ASM_EMIT("vfmadd231ps         %%ymm3, %%ymm3, %%ymm2")            // y2   = W = b_re*b_re + b_im*b_im
            __ASM_EMIT("vmulps              %%ymm4, %%ymm1, %%ymm7")            // y7   = t_im*b_im
            __ASM_EMIT("vmulps              %%ymm4, %%ymm0, %%ymm6")            // y6   = t_re*b_im
            __ASM_EMIT("vfmadd132ps         %%ymm3, %%ymm7, %%ymm0")            // y0   = t_re*b_re + t_im*b_im
            __ASM_EMIT("vfmsub132ps         %%ymm3, %%ymm6, %%ymm1")            // y1   = t_im*b_re - t_re*b_im
            __ASM_EMIT("vdivps              %%ymm2, %%ymm0, %%ymm0")            // y0   = a_re = (t_re*b_re + t_im*b_im)/W
            __ASM_EMIT("vdivps              %%ymm2, %%ymm1, %%ymm1")            // y1   = b_re = (t_im*b_re - t_re*b_im)/W
            // Apply
            __ASM_EMIT("vmovups             0x00(%[dst]), %%xmm2")              // x2   = r0 i0 r1 i1
            __ASM_EMIT("vmovups             0x10(%[dst]), %%xmm3")              // x3   = r2 i2 r3 i3
            __ASM_EMIT("vinsertf128         $1, 0x20(%[dst]), %%ymm2, %%ymm2")  // y2   = r0 i0 r1 i1 r4 i4 r5 i5
            __ASM_EMIT("vinsertf128         $1, 0x30(%[dst]), %%ymm3, %%ymm3")  // y3   = r2 i2 r3 i3 r6 i6 r7 i7
            __ASM_EMIT("vshufps             $0x88, %%ymm3, %%ymm2, %%ymm6")     // y6   = b_re = r0 r1 r2 r3 r4 r5 r6 r7
            __ASM_EMIT("vshufps             $0xdd, %%ymm3, %%ymm2, %%ymm7")     // y7   = b_im = i0 i1 i2 i3 i4 i5 i6 i7
            __ASM_EMIT("vmulps              %%ymm7, %%ymm1, %%ymm3")            // y3   = a_im*b_im
            __ASM_EMIT("vmulps              %%ymm7, %%ymm0, %%ymm2")            // y2   = a_re*b_im
            __ASM_EMIT("vfmsub132ps         %%ymm6, %%ymm3, %%ymm0")            // y0   = a_re*b_re - a_im*b_im
            __ASM_EMIT("vfmadd132ps         %%ymm6, %%ymm2, %%ymm1")            // y1   = a_im*b_re + a_re*b_im
            __ASM_EMIT("vunpcklps           %%ymm1, %%ymm0, %%ymm2")            // y2   = r0 i0 r1 i1 r4 i4 r5 i5
            __ASM_EMIT("vunpckhps           %%ymm1, %%ymm0, %%ymm3")            // y3   = r2 i2 r3 i3 r6 i6 r7 i7
            __ASM_EMIT("vmovups             %%xmm2, 0x00(%[dst])")
            __ASM_EMIT("vmovups             %%xmm3, 0x10(%[dst])")
            __ASM_EMIT("vextractf128        $1, %%ymm2, 0x20(%[dst])")
            __ASM_EMIT("vextractf128        $1, %%ymm3, 0x30(%[dst])")
            // Reload
            __ASM_EMIT("vmovaps             0x00 + %[fp], %%ymm0")
            __ASM_EMIT("vmovaps             0x20 + %[fp], %%ymm1")
            __ASM_EMIT("vmovaps             0x40 + %[fp], %%ymm2")
            __ASM_EMIT("vmovaps             0x60 + %[fp], %%ymm3")
            __ASM_EMIT("vmovaps             0x80 + %[fp], %%ymm4")
            __ASM_EMIT("vmovaps             0xa0 + %[fp], %%ymm5")
            __ASM_EMIT("add                 $0x20, %[f]")
            __ASM_EMIT("add                 $0x40, %[dst]")
            __ASM_EMIT("sub                 $8, %[count]")
            __ASM_EMIT("jae                 1b")
            __ASM_EMIT("2:")
            // x4 block
            __ASM_EMIT("add                 $4, %[count]")
            __ASM_EMIT("jl                  4f")
            __ASM_EMIT("vmovups             0x00(%[f]), %%xmm6")                // x6   = f
            __ASM_EMIT("vmulps              %%xmm6, %%xmm6, %%xmm7")            // x7   = f2 = f*f
            __ASM_EMIT("vmulps              %%xmm6, %%xmm1, %%xmm1")            // x1   = t_im = t1*f
            __ASM_EMIT("vmulps              %%xmm6, %%xmm4, %%xmm4")            // x4   = b_im = b1*f
            __ASM_EMIT("vfnmadd231ps        %%xmm7, %%xmm2, %%xmm0")            // x0   = t_re = t0 - t2*f2
            __ASM_EMIT("vfnmadd231ps        %%xmm7, %%xmm5, %%xmm3")            // x3   = b_re = b0 - b2*f2
            __ASM_EMIT("vmulps              %%xmm4, %%xmm4, %%xmm2")            // x2   = b_im*b_im
            __ASM_EMIT("vfmadd231ps         %%xmm3, %%xmm3, %%xmm2")            // x2   = W = b_re*b_re + b_im*b_im
            __ASM_EMIT("vmulps              %%xmm4, %%xmm1, %%xmm7")            // x7   = t_im*b_im
            __ASM_EMIT("vmulps              %%xmm4, %%xmm0, %%xmm6")            // x6   = t_re*b_im
            __ASM_EMIT("vfmadd132ps         %%xmm3, %%xmm7, %%xmm0")            // x0   = t_re*b_re + t_im*b_im
            __ASM_EMIT("vfmsub132ps         %%xmm3, %%xmm6, %%xmm1")            // x1   = t_im*b_re - t_re*b_im
            __ASM_EMIT("vdivps              %%xmm2, %%xmm0, %%xmm0")            // x0   = a_re = (t_re*b_re + t_im*b_im)/W
            __ASM_EMIT("vdivps              %%xmm2, %%xmm1, %%xmm1")            // x1   = b_re = (t_im*b_re - t_re*b_im)/W
            // Apply
            __ASM_EMIT("vmovups             0x00(%[dst]), %%xmm2")              // x2   = r0 i0 r1 i1
            __ASM_EMIT("vmovups             0x10(%[dst]), %%xmm3")              // x3   = r2 i2 r3 i3
            __ASM_EMIT("vshufps             $0x88, %%xmm3, %%xmm2, %%xmm6")     // x6   = b_re = r0 r1 r2 r3 r4 r5 r6 r7
            __ASM_EMIT("vshufps             $0xdd, %%xmm3, %%xmm2, %%xmm7")     // x7   = b_im = i0 i1 i2 i3 i4 i5 i6 i7
            __ASM_EMIT("vmulps              %%xmm7, %%xmm1, %%xmm3")            // y3   = a_im*b_im
            __ASM_EMIT("vmulps              %%xmm7, %%xmm0, %%xmm2")            // y2   = a_re*b_im
            __ASM_EMIT("vfmsub132ps         %%xmm6, %%xmm3, %%xmm0")            // y0   = a_re*b_re - a_im*b_im
            __ASM_EMIT("vfmadd132ps         %%xmm6, %%xmm2, %%xmm1")            // y1   = a_im*b_re + a_re*b_im
            __ASM_EMIT("vunpcklps           %%xmm1, %%xmm0, %%xmm2")            // x2   = r0 i0 r1 i1 r4 i4 r5 i5
            __ASM_EMIT("vunpckhps           %%xmm1, %%xmm0, %%xmm3")            // x3   = r2 i2 r3 i3 r6 i6 r7 i7
            __ASM_EMIT("vmovups             %%xmm2, 0x00(%[dst])")
            __ASM_EMIT("vmovups             %%xmm3, 0x10(%[dst])")
            // Reload
            __ASM_EMIT("vmovaps             0x00 + %[fp], %%xmm0")
            __ASM_EMIT("vmovaps             0x20 + %[fp], %%xmm1")
            __ASM_EMIT("vmovaps             0x40 + %[fp], %%xmm2")
            __ASM_EMIT("vmovaps             0x60 + %[fp], %%xmm3")
            __ASM_EMIT("vmovaps             0x80 + %[fp], %%xmm4")
            __ASM_EMIT("vmovaps             0xa0 + %[fp], %%xmm5")
            __ASM_EMIT("sub                 $4, %[count]")
            __ASM_EMIT("add                 $0x10, %[f]")
            __ASM_EMIT("add                 $0x20, %[dst]")
            __ASM_EMIT("4:")
            // x2 block
            __ASM_EMIT("add                 $2, %[count]")
            __ASM_EMIT("jl                  6f")
            __ASM_EMIT("vmovlps             0x00(%[f]), %%xmm6, %%xmm6")        // x6   = f
            __ASM_EMIT("vmulps              %%xmm6, %%xmm6, %%xmm7")            // x7   = f2 = f*f
            __ASM_EMIT("vmulps              %%xmm6, %%xmm1, %%xmm1")            // x1   = t_im = t1*f
            __ASM_EMIT("vmulps              %%xmm6, %%xmm4, %%xmm4")            // x4   = b_im = b1*f
            __ASM_EMIT("vfnmadd231ps        %%xmm7, %%xmm2, %%xmm0")            // x0   = t_re = t0 - t2*f2
            __ASM_EMIT("vfnmadd231ps        %%xmm7, %%xmm5, %%xmm3")            // x3   = b_re = b0 - b2*f2
            __ASM_EMIT("vmulps              %%xmm4, %%xmm4, %%xmm2")            // x2   = b_im*b_im
            __ASM_EMIT("vfmadd231ps         %%xmm3, %%xmm3, %%xmm2")            // x2   = W = b_re*b_re + b_im*b_im
            __ASM_EMIT("vmulps              %%xmm4, %%xmm1, %%xmm7")            // x7   = t_im*b_im
            __ASM_EMIT("vmulps              %%xmm4, %%xmm0, %%xmm6")            // x6   = t_re*b_im
            __ASM_EMIT("vfmadd132ps         %%xmm3, %%xmm7, %%xmm0")            // x0   = t_re*b_re + t_im*b_im
            __ASM_EMIT("vfmsub132ps         %%xmm3, %%xmm6, %%xmm1")            // x1   = t_im*b_re - t_re*b_im
            __ASM_EMIT("vdivps              %%xmm2, %%xmm0, %%xmm0")            // x0   = a_re = (t_re*b_re + t_im*b_im)/W
            __ASM_EMIT("vdivps              %%xmm2, %%xmm1, %%xmm1")            // x1   = b_re = (t_im*b_re - t_re*b_im)/W
            // Apply
            __ASM_EMIT("vmovups             0x00(%[dst]), %%xmm2")              // x2   = r0 i0 r1 i1
            __ASM_EMIT("vshufps             $0x88, %%xmm2, %%xmm2, %%xmm6")     // x6   = b_re = r0 r1 r0 r1
            __ASM_EMIT("vshufps             $0xdd, %%xmm2, %%xmm2, %%xmm7")     // x7   = b_im = i0 i1 i0 i1
            __ASM_EMIT("vmulps              %%xmm7, %%xmm1, %%xmm3")            // y3   = a_im*b_im
            __ASM_EMIT("vmulps              %%xmm7, %%xmm0, %%xmm2")            // y2   = a_re*b_im
            __ASM_EMIT("vfmsub132ps         %%xmm6, %%xmm3, %%xmm0")            // y0   = a_re*b_re - a_im*b_im
            __ASM_EMIT("vfmadd132ps         %%xmm6, %%xmm2, %%xmm1")            // y1   = a_im*b_re + a_re*b_im
            __ASM_EMIT("vunpcklps           %%xmm1, %%xmm0, %%xmm0")            // x0   = r0 i0 r1 i1
            __ASM_EMIT("vmovups             %%xmm0, 0x00(%[dst])")
            // Reload
            __ASM_EMIT("vmovaps             0x00 + %[fp], %%xmm0")
            __ASM_EMIT("vmovaps             0x20 + %[fp], %%xmm1")
            __ASM_EMIT("vmovaps             0x40 + %[fp], %%xmm2")
            __ASM_EMIT("vmovaps             0x60 + %[fp], %%xmm3")
            __ASM_EMIT("vmovaps             0x80 + %[fp], %%xmm4")
            __ASM_EMIT("vmovaps             0xa0 + %[fp], %%xmm5")
            __ASM_EMIT("sub                 $2, %[count]")
            __ASM_EMIT("add                 $0x08, %[f]")
            __ASM_EMIT("add                 $0x10, %[dst]")
            __ASM_EMIT("6:")
            // x1 block
            __ASM_EMIT("add                 $1, %[count]")
            __ASM_EMIT("jl                  8f")
            __ASM_EMIT("vmovss              0x00(%[f]), %%xmm6")                // x6   = f
            __ASM_EMIT("vmulps              %%xmm6, %%xmm6, %%xmm7")            // x7   = f2 = f*f
            __ASM_EMIT("vmulps              %%xmm6, %%xmm1, %%xmm1")            // x1   = t_im = t1*f
            __ASM_EMIT("vmulps              %%xmm6, %%xmm4, %%xmm4")            // x4   = b_im = b1*f
            __ASM_EMIT("vfnmadd231ps        %%xmm7, %%xmm2, %%xmm0")            // x0   = t_re = t0 - t2*f2
            __ASM_EMIT("vfnmadd231ps        %%xmm7, %%xmm5, %%xmm3")            // x3   = b_re = b0 - b2*f2
            __ASM_EMIT("vmulps              %%xmm4, %%xmm4, %%xmm2")            // x2   = b_im*b_im
            __ASM_EMIT("vfmadd231ps         %%xmm3, %%xmm3, %%xmm2")            // x2   = W = b_re*b_re + b_im*b_im
            __ASM_EMIT("vmulps              %%xmm4, %%xmm1, %%xmm7")            // x7   = t_im*b_im
            __ASM_EMIT("vmulps              %%xmm4, %%xmm0, %%xmm6")            // x6   = t_re*b_im
            __ASM_EMIT("vfmadd132ps         %%xmm3, %%xmm7, %%xmm0")            // x0   = t_re*b_re + t_im*b_im
            __ASM_EMIT("vfmsub132ps         %%xmm3, %%xmm6, %%xmm1")            // x1   = t_im*b_re - t_re*b_im
            __ASM_EMIT("vdivps              %%xmm2, %%xmm0, %%xmm0")            // x0   = a_re = (t_re*b_re + t_im*b_im)/W
            __ASM_EMIT("vdivps              %%xmm2, %%xmm1, %%xmm1")            // x1   = b_re = (t_im*b_re - t_re*b_im)/W
            // Apply
            __ASM_EMIT("vmovss              0x00(%[dst]), %%xmm6")              // x6   = b_re
            __ASM_EMIT("vmovss              0x04(%[dst]), %%xmm7")              // x7   = b_im
            __ASM_EMIT("vmulps              %%xmm7, %%xmm1, %%xmm3")            // y3   = a_im*b_im
            __ASM_EMIT("vmulps              %%xmm7, %%xmm0, %%xmm2")            // y2   = a_re*b_im
            __ASM_EMIT("vfmsub132ps         %%xmm6, %%xmm3, %%xmm0")            // y0   = a_re*b_re - a_im*b_im
            __ASM_EMIT("vfmadd132ps         %%xmm6, %%xmm2, %%xmm1")            // y1   = a_im*b_re + a_re*b_im
            __ASM_EMIT("vunpcklps           %%xmm1, %%xmm0, %%xmm0")            // x0   = r0 i0
            __ASM_EMIT("vmovlps             %%xmm0, 0x00(%[dst])")
            __ASM_EMIT("8:")

            : [dst] "+r" (dst), [f] "+r" (freq), [count] "+r" (count)
            : [c] "r" (c),
              [fp] "o" (fp)
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }
}

#endif /* DSP_ARCH_X86_AVX_FILTERS_TRANSFER_H_ */
