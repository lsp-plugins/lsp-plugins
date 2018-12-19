/*
 * transpose.h
 *
 *  Created on: 19 дек. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_AVX2_GRAPHICS_TRANSPOSE_H_
#define DSP_ARCH_X86_AVX2_GRAPHICS_TRANSPOSE_H_

#ifndef DSP_ARCH_X86_AVX2_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_AVX2_IMPL */

namespace avx2
{
    #define MAT4_TRANSPOSE(x0, x1, x2, x3, x4, x5) \
        /* x0 = a0 a1 a2 a3 */ \
        /* x1 = b0 b1 b2 b3 */ \
        /* x2 = c0 c1 c2 c3 */ \
        /* x3 = d0 d1 d2 d3 */ \
        __ASM_EMIT("vpunpckhdq      " x1 ", " x0 ", " x4)   /* x4 = a2 b2 a3 b3 */ \
        __ASM_EMIT("vpunpckhdq      " x3 ", " x2 ", " x5)   /* x5 = c2 d2 c3 d3 */ \
        __ASM_EMIT("vpunpckldq      " x1 ", " x0 ", " x0)   /* x0 = a0 b0 a1 b1 */ \
        __ASM_EMIT("vpunpckldq      " x3 ", " x2 ", " x2)   /* x2 = c0 d0 c1 d1 */ \
        __ASM_EMIT("vpunpckhqdq     " x2 ", " x0 ", " x1)   /* x1 = a1 b1 c1 d1 */ \
        __ASM_EMIT("vpunpckhqdq     " x5 ", " x4 ", " x3)   /* x3 = a3 b3 c3 d3 */ \
        __ASM_EMIT("vpunpcklqdq     " x2 ", " x0 ", " x0)   /* x1 = a0 b0 c0 d0 */ \
        __ASM_EMIT("vpunpcklqdq     " x5 ", " x4 ", " x2)   /* x2 = a2 b2 c2 d2 */

    #define MAT4X2_INTERLEAVE(y0, y1, y2, y3, y4, y5) \
        /* y0 = c0 c4 */ \
        /* y1 = c1 c5 */ \
        /* y2 = c2 c6 */ \
        /* y3 = c3 c7 */ \
        __ASM_EMIT("vextractf128    $1, %%ymm" y0 ", %%xmm" y4 )                /* y4 = c4 0 */ \
        __ASM_EMIT("vextractf128    $1, %%ymm" y2 ", %%xmm" y5 )                /* y5 = c6 0 */ \
        __ASM_EMIT("vinsertf128     $0, %%xmm" y4 ", %%ymm" y1 ", %%ymm" y4)    /* y4 = c4 c5 */ \
        __ASM_EMIT("vinsertf128     $0, %%xmm" y5 ", %%ymm" y3 ", %%ymm" y5)    /* y5 = c6 c7 */ \
        __ASM_EMIT("vinsertf128     $1, %%xmm" y1 ", %%ymm" y0 ", %%ymm" y0)    /* y0 = c0 c1 */ \
        __ASM_EMIT("vinsertf128     $1, %%xmm" y3 ", %%ymm" y2 ", %%ymm" y1)    /* y1 = c2 c3 */ \
        __ASM_EMIT("vmovaps         %%ymm" y4 ", %%ymm" y2 )                    /* y2 = c4 c5 */ \
        __ASM_EMIT("vmovaps         %%ymm" y5 ", %%ymm" y3 )                    /* y3 = c6 c7 */

}


#endif /* INCLUDE_DSP_ARCH_X86_AVX2_GRAPHICS_TRANSPOSE_H_ */
