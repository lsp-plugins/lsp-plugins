/*
 * papply.h
 *
 *  Created on: 6 мар. 2017 г.
 *      Author: sadko
 */
#ifndef DSP_ARCH_X86_SSE_FASTCONV_H_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_SSE_FASTCONV_H_IMPL */

namespace sse
{
    void fastconv_parse_apply_internal(float *tmp, const float *conv, size_t rank)
    {
        size_t items    = size_t(1) << (rank + 1);

        ARCH_X86_ASM
        (
            __ASM_EMIT("1:")

            // Load data
            __ASM_EMIT("movups      0x00(%[tmp]), %%xmm0")      /* xmm0 = r0 r1 r2 r3 */
            __ASM_EMIT("movups      0x10(%[tmp]), %%xmm1")      /* xmm1 = i0 i1 i2 i3 */
            __ASM_EMIT("movups      0x20(%[tmp]), %%xmm4")
            __ASM_EMIT("movups      0x30(%[tmp]), %%xmm5")

            // Do x4 direct butterflies
            __ASM_EMIT("movaps      %%xmm0, %%xmm2")            /* xmm2 = r0 r1 r2 r3 */
            __ASM_EMIT("movaps      %%xmm4, %%xmm6")
            __ASM_EMIT("unpcklps    %%xmm1, %%xmm0")            /* xmm0 = r0 i0 r1 i1 */
            __ASM_EMIT("unpcklps    %%xmm5, %%xmm4")
            __ASM_EMIT("unpckhps    %%xmm1, %%xmm2")            /* xmm2 = r2 i2 r3 i3 */
            __ASM_EMIT("unpckhps    %%xmm5, %%xmm6")
            __ASM_EMIT("movaps      %%xmm0, %%xmm1")            /* xmm1 = r0 i0 r1 i1 */
            __ASM_EMIT("movaps      %%xmm4, %%xmm5")
            __ASM_EMIT("addps       %%xmm2, %%xmm0")            /* xmm0 = r0+r2 i0+i2 r1+r3 i1+i3 = r0k i0k r2k i2k */
            __ASM_EMIT("addps       %%xmm6, %%xmm4")
            __ASM_EMIT("subps       %%xmm2, %%xmm1")            /* xmm1 = r0-r2 i0-i2 r1-r3 i1-i3 = r1k i1k r3k i3k */
            __ASM_EMIT("subps       %%xmm6, %%xmm5")

            __ASM_EMIT("movaps      %%xmm0, %%xmm2")            /* xmm2 = r0k i0k r2k i2k */
            __ASM_EMIT("movaps      %%xmm4, %%xmm6")
            __ASM_EMIT("unpcklps    %%xmm1, %%xmm0")            /* xmm0 = r0k r1k i0k i1k */
            __ASM_EMIT("unpcklps    %%xmm5, %%xmm4")
            __ASM_EMIT("unpckhps    %%xmm1, %%xmm2")            /* xmm2 = r2k r3k i2k i3k */
            __ASM_EMIT("unpckhps    %%xmm5, %%xmm6")
            __ASM_EMIT("movaps      %%xmm0, %%xmm1")            /* xmm1 = r0k r1k i0k i1k */
            __ASM_EMIT("movaps      %%xmm4, %%xmm5")
            __ASM_EMIT("shufps      $0x6c, %%xmm2, %%xmm2")     /* xmm2 = r2k i3k i2k r3k */
            __ASM_EMIT("shufps      $0x6c, %%xmm6, %%xmm6")
            __ASM_EMIT("addps       %%xmm2, %%xmm0")            /* xmm0 = r0k+r2k r1k+i3k i0k+i2k i1k+r3k */
            __ASM_EMIT("addps       %%xmm6, %%xmm4")
            __ASM_EMIT("subps       %%xmm2, %%xmm1")            /* xmm1 = r0k-r2k r1k-i3k i0k-i2k i1k-r3k */
            __ASM_EMIT("subps       %%xmm6, %%xmm5")

            __ASM_EMIT("movaps      %%xmm0, %%xmm2")            /* xmm2 = r0k+r2k r1k+i3k i0k+i2k i1k+r3k */
            __ASM_EMIT("movaps      %%xmm4, %%xmm6")
            __ASM_EMIT("unpcklps    %%xmm1, %%xmm0")            /* xmm0 = r0k+r2k r0k-r2k r1k+i3k r1k-i3k */
            __ASM_EMIT("unpcklps    %%xmm5, %%xmm4")
            __ASM_EMIT("unpckhps    %%xmm1, %%xmm2")            /* xmm2 = i0k+i2k i0k-i2k i1k+r3k i1k-r3k */
            __ASM_EMIT("unpckhps    %%xmm5, %%xmm6")
            __ASM_EMIT("shufps      $0xb4, %%xmm2, %%xmm2")     /* xmm2 = i0k+i2k i0k-i2k i1k-r3k i1k+r3k */
            __ASM_EMIT("shufps      $0xb4, %%xmm6, %%xmm6")

            // Do complex multiplication
            // xmm0 = r0 r1 r2 r3
            // xmm2 = i0 i1 i2 i3
            // xmm4 = r4 r5 r6 r7
            // xmm6 = i4 i5 i6 i7
            __ASM_EMIT("movups      0x00(%[conv]), %%xmm1")     /* xmm1 = rc0 rc1 rc2 rc3 */
            __ASM_EMIT("movups      0x10(%[conv]), %%xmm3")     /* xmm3 = ic0 ic1 ic2 ic3 */
            __ASM_EMIT("movaps      %%xmm1, %%xmm5")            /* xmm5 = rc0 rc1 rc2 rc3 */
            __ASM_EMIT("movaps      %%xmm3, %%xmm7")            /* xmm7 = ic0 ic1 ic2 ic3 */
            __ASM_EMIT("mulps       %%xmm0, %%xmm1")            /* xmm1 = rc0*r0 rc1*r1 rc2*r2 rc3*r3 */
            __ASM_EMIT("mulps       %%xmm2, %%xmm7")            /* xmm7 = ic0*i0 ic1*i1 ic2*i2 ic3*i3 */
            __ASM_EMIT("mulps       %%xmm0, %%xmm3")            /* xmm3 = ic0*r0 ic1*r1 ic2*r2 ic3*r3 */
            __ASM_EMIT("mulps       %%xmm2, %%xmm5")            /* xmm5 = rc0*i0 rc1*i1 rc2*i2 rc3*i3 */
            __ASM_EMIT("subps       %%xmm7, %%xmm1")            /* xmm1 = rc0*r0-ic0*i0 rc1*r1-ic1*i1 rc2*r2-ic2*i2 rc3*r3-ic3*i3 */
            __ASM_EMIT("addps       %%xmm5, %%xmm3")            /* xmm3 = ic0*r0+rc0*i0 ic1*r1+rc1*i1 ic2*r2+rc2*i2 ic3*r3+rc3*i3 */

            __ASM_EMIT("movups      0x20(%[conv]), %%xmm0")     /* xmm0 = rc4 rc5 rc6 rc7 */
            __ASM_EMIT("movups      0x30(%[conv]), %%xmm2")     /* xmm2 = ic4 ic5 ic6 ic7 */
            __ASM_EMIT("movaps      %%xmm0, %%xmm5")            /* xmm5 = rc4 rc5 rc6 rc7 */
            __ASM_EMIT("movaps      %%xmm2, %%xmm7")            /* xmm7 = ic4 ic5 ic6 ic7 */
            __ASM_EMIT("mulps       %%xmm4, %%xmm5")            /* xmm5 = rc4*r4 rc5*r5 rc6*r6 rc7*r7 */
            __ASM_EMIT("mulps       %%xmm6, %%xmm2")            /* xmm2 = ic4*i4 ic5*i5 ic6*i6 ic7*i7 */
            __ASM_EMIT("mulps       %%xmm4, %%xmm7")            /* xmm7 = ic4*r4 ic5*r5 ic6*r6 ic7*r7 */
            __ASM_EMIT("mulps       %%xmm6, %%xmm0")            /* xmm0 = rc4*i4 rc5*i5 rc6*i6 rc7*i7 */
            __ASM_EMIT("subps       %%xmm2, %%xmm5")            /* xmm5 = rc4*r4-ic4*i4 rc5*r5-ic5*i5 rc6*r6-ic6*i6 rc7*r7-ic7*i7 */
            __ASM_EMIT("addps       %%xmm0, %%xmm7")            /* xmm7 = ic4*r4+rc4*i4 ic5*r5+rc5*i5 ic6*r6+rc6*i6 ic7*r7+rc7*i7 */

            // Do x4 reverse butterflies
            // xmm1 = r0 r1 r2 r3
            // xmm3 = i0 i1 i2 i3
            // xmm5 = r4 r5 r6 r7
            // xmm7 = i4 i5 i6 i7
            __ASM_EMIT("movaps      %%xmm1, %%xmm0")            /* xmm0 = r0 r1 r2 r3 */
            __ASM_EMIT("movaps      %%xmm5, %%xmm4")
            __ASM_EMIT("shufps      $0x88, %%xmm3, %%xmm0")     /* xmm0 = r0 r2 i0 i2 */
            __ASM_EMIT("shufps      $0x88, %%xmm7, %%xmm4")
            __ASM_EMIT("shufps      $0xdd, %%xmm3, %%xmm1")     /* xmm1 = r1 r3 i1 i3 */
            __ASM_EMIT("shufps      $0xdd, %%xmm7, %%xmm5")
            __ASM_EMIT("movaps      %%xmm0, %%xmm2")            /* xmm2 = r0 r2 i0 i2 */
            __ASM_EMIT("movaps      %%xmm4, %%xmm6")
            __ASM_EMIT("addps       %%xmm1, %%xmm0")            /* xmm0 = r0+r1 r2+r3 i0+i1 i2+i3 = r0k r2k i0k i2k */
            __ASM_EMIT("addps       %%xmm5, %%xmm4")
            __ASM_EMIT("subps       %%xmm1, %%xmm2")            /* xmm2 = r0-r1 r2-r3 i0-i1 i2-i3 = r1k r3k i1k i3k */
            __ASM_EMIT("subps       %%xmm5, %%xmm6")

            __ASM_EMIT("movaps      %%xmm0, %%xmm1")            /* xmm1 = r0k r2k i0k i2k */
            __ASM_EMIT("movaps      %%xmm4, %%xmm5")
            __ASM_EMIT("shufps      $0x88, %%xmm2, %%xmm0")     /* xmm0 = r0k i0k r1k i1k */
            __ASM_EMIT("shufps      $0x88, %%xmm6, %%xmm4")
            __ASM_EMIT("shufps      $0x7d, %%xmm2, %%xmm1")     /* xmm1 = r2k i2k i3k r3k */
            __ASM_EMIT("shufps      $0x7d, %%xmm6, %%xmm5")
            __ASM_EMIT("movaps      %%xmm0, %%xmm2")            /* xmm2 = r0k i0k r1k i1k */
            __ASM_EMIT("movaps      %%xmm4, %%xmm6")
            __ASM_EMIT("addps       %%xmm1, %%xmm0")            /* xmm0 = r0k+r2k i0k+i2k r1k+i3k i1k+r3k = d0 d4 d3 d5 */
            __ASM_EMIT("addps       %%xmm5, %%xmm4")
            __ASM_EMIT("subps       %%xmm1, %%xmm2")            /* xmm2 = r0k-r2k i0k-i2k r1k-i3k i1k-r3k = d2 d6 d1 d7  */
            __ASM_EMIT("subps       %%xmm5, %%xmm6")

            __ASM_EMIT("movaps      %%xmm0, %%xmm1")            /* xmm1 = d0 d4 d3 d5 */
            __ASM_EMIT("movaps      %%xmm4, %%xmm5")
            __ASM_EMIT("shufps      $0x88, %%xmm2, %%xmm0")     /* xmm0 = d0 d3 d2 d1 */
            __ASM_EMIT("shufps      $0x88, %%xmm6, %%xmm4")
            __ASM_EMIT("shufps      $0xdd, %%xmm2, %%xmm1")     /* xmm1 = d4 d5 d6 d7 */
            __ASM_EMIT("shufps      $0xdd, %%xmm6, %%xmm5")
            __ASM_EMIT("shufps      $0x6c, %%xmm0, %%xmm0")     /* xmm0 = d0 d1 d2 d3 */
            __ASM_EMIT("shufps      $0x6c, %%xmm4, %%xmm4")

            // Finally store result
            __ASM_EMIT("movups      %%xmm0, 0x00(%[tmp])")
            __ASM_EMIT("movups      %%xmm1, 0x10(%[tmp])")
            __ASM_EMIT("movups      %%xmm4, 0x20(%[tmp])")
            __ASM_EMIT("movups      %%xmm5, 0x30(%[tmp])")

            __ASM_EMIT("add         $0x40, %[tmp]")
            __ASM_EMIT("add         $0x40, %[conv]")
            __ASM_EMIT("sub         $16, %[k]")
            __ASM_EMIT("jnz         1b")

            : [tmp] "+r" (tmp), [k] "+r" (items), [conv] "+r" (conv)
            :
            : "cc", "memory",
              "%xmm0", "%xmm1", "%xmm2", "%xmm3",
              "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );

    }
}
