/*
 * fmop_kx.h
 *
 *  Created on: 5 янв. 2020 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_SSE2_PMATH_FMOP_KX_H_
#define DSP_ARCH_X86_SSE2_PMATH_FMOP_KX_H_

#ifndef DSP_ARCH_X86_SSE2_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_SSE2_IMPL */

namespace sse2
{
    #define FMOD_KX_CORE(DST, SRC) \
        __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0") \
        __ASM_EMIT("xor         %[off], %[off]") \
        __ASM_EMIT("movaps      %%xmm0, %%xmm1") \
        /* 16x blocks */ \
        __ASM_EMIT("sub         $16, %[count]") \
        __ASM_EMIT("jb          2f") \
        __ASM_EMIT("1:") \
        __ASM_EMIT("movups      0x00(%[src], %[off]), %%xmm2")) \
        __ASM_EMIT("movups      0x10(%[src], %[off]), %%xmm3")) \
        __ASM_EMIT("movaps      %%xmm2, %%xmm6") \
        __ASM_EMIT("movaps      %%xmm3, %%xmm7") \
        __ASM_EMIT("divps       %%xmm0, %%xmm6") \
        __ASM_EMIT("divps       %%xmm1, %%xmm7") \
        __ASM_EMIT("cvttps2dq   %%xmm6, %%xmm6") \
        __ASM_EMIT("cvttps2dq   %%xmm7, %%xmm7") \
        __ASM_EMIT("cvtdq2ps    %%xmm6, %%xmm6") \
        __ASM_EMIT("cvtdq2ps    %%xmm7, %%xmm7") \
        __ASM_EMIT("mulps       %%xmm2, %%xmm6") \
        __ASM_EMIT("mulps       %%xmm3, %%xmm7") \
        __ASM_EMIT("subps       %%xmm6, %%xmm2") \
        __ASM_EMIT("subps       %%xmm7, %%xmm3") \
        __ASM_EMIT("movups      %%xmm2, 0x00(%[dst], %[off])")) \
        __ASM_EMIT("movups      %%xmm3, 0x10(%[dst], %[off])")) \
        __ASM_EMIT("divps       %%xmm%%xmm6") \
        __ASM_EMIT("add         $0x20, %[off]") \
        __ASM_EMIT("sub         $16, %[count]") \
        __ASM_EMIT("jae         1b") \
        __ASM_EMIT("2:") \
}

#endif /* DSP_ARCH_X86_SSE2_PMATH_FMOP_KX_H_ */
