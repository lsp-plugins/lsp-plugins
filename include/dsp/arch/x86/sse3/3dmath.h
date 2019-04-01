/*
 * 3dmath.h
 *
 *  Created on: 31 мар. 2019 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_SSE3_3DMATH_H_
#define DSP_ARCH_X86_SSE3_3DMATH_H_

#ifndef DSP_ARCH_X86_SSE3_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_SSE3_IMPL */

namespace sse3
{
    size_t colocation_x2_v1p2(const vector3d_t *pl, const point3d_t *p0, const point3d_t *p1)
    {
        float x0, x1, x2;
        float res[4] __lsp_aligned16;

        ARCH_X86_ASM
        (
            __ASM_EMIT("movups      (%[pl]), %[x2]")        /* xmm3 = pl    */
            __ASM_EMIT("movups      (%[p0]), %[x0]")        /* xmm0 = p0    */
            __ASM_EMIT("movups      (%[p1]), %[x1]")        /* xmm1 = p1    */

            __ASM_EMIT("mulps       %[x2], %[x0]")          /* xmm0 = x0*px y0*py z0*pz w0*pw */
            __ASM_EMIT("mulps       %[x2], %[x1]")          /* xmm1 = x1*px y1*py z1*pz w1*pw */
            __ASM_EMIT("haddps      %[x1], %[x0]")          /* xmm0 = x0*px+y0*py z0*pz+w0*pw x1*px+y1*py z1*pz+w1*pw */
            __ASM_EMIT("haddps      %[x0], %[x0]")          /* xmm0 = x1*px+y1*py+z1*pz+w1*pw x1*px+y1*py+z1*pz+w1*pw ? ? = k0 k1 ? ? */
            __ASM_EMIT("movaps      %[x0], %[x1]")          /* xmm1 = k0 k1 ? ? */

            __ASM_EMIT("cmpps       $2, %[PTOL], %[x0]")    /* xmm0 = k0 <= +TOL k1 <= +TOL ? ? */
            __ASM_EMIT("cmpps       $1, %[MTOL], %[x1]")    /* xmm1 = k0 < -TOL  k1 < -TOL ? ? */
            __ASM_EMIT("andps       %[IONE], %[x0]")        /* xmm0 = 1*[k0 <= +TOL] 1*[k1 <= +TOL] ? ? */
            __ASM_EMIT("andps       %[IONE], %[x1]")        /* xmm1 = 1*[k0 < -TOL] 1*[k1 < -TOL] ? ? */
            __ASM_EMIT("paddd       %[x1], %[x0]")
            __ASM_EMIT("movdqa      %[x0], %[res]")
            __ASM_EMIT32("movl      0x00 + %[res], %[p0]")
            __ASM_EMIT32("movl      0x04 + %[res], %[p1]")
            __ASM_EMIT64("movl      0x00 + %[res], %k[p0]")
            __ASM_EMIT64("movl      0x04 + %[res], %k[p1]")
            __ASM_EMIT("shl         $2, %[p1]")
            __ASM_EMIT("or          %[p1], %[p0]")
            : [p0] "+r" (p0), [p1] "+r" (p1),
              [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2)
            : [pl] "r" (pl),
              [res] "o" (res),
              [PTOL] "m" (X_3D_TOLERANCE),
              [MTOL] "m" (X_3D_MTOLERANCE),
              [IONE] "m" (IONE)
            : "cc"
        );

        return size_t(p0);
    }

    size_t colocation_x2_v1pv(const vector3d_t *pl, const point3d_t *pv)
    {
        float x0, x1, x2;
        float res[4] __lsp_aligned16;
        size_t k0, k1;

        ARCH_X86_ASM
        (
            __ASM_EMIT("movups      (%[pl]), %[x2]")        /* xmm3 = pl    */
            __ASM_EMIT("movups      0x00(%[pv]), %[x0]")    /* xmm0 = p0    */
            __ASM_EMIT("movups      0x10(%[pv]), %[x1]")    /* xmm1 = p1    */

            __ASM_EMIT("mulps       %[x2], %[x0]")          /* xmm0 = x0*px y0*py z0*pz w0*pw */
            __ASM_EMIT("mulps       %[x2], %[x1]")          /* xmm1 = x1*px y1*py z1*pz w1*pw */
            __ASM_EMIT("haddps      %[x1], %[x0]")          /* xmm0 = x0*px+y0*py z0*pz+w0*pw x1*px+y1*py z1*pz+w1*pw */
            __ASM_EMIT("haddps      %[x0], %[x0]")          /* xmm0 = x1*px+y1*py+z1*pz+w1*pw x1*px+y1*py+z1*pz+w1*pw ? ? = k0 k1 ? ? */
            __ASM_EMIT("movaps      %[x0], %[x1]")          /* xmm1 = k0 k1 ? ? */

            __ASM_EMIT("cmpps       $2, %[PTOL], %[x0]")    /* xmm0 = k0 <= +TOL k1 <= +TOL ? ? */
            __ASM_EMIT("cmpps       $1, %[MTOL], %[x1]")    /* xmm1 = k0 < -TOL  k1 < -TOL ? ? */
            __ASM_EMIT("andps       %[IONE], %[x0]")        /* xmm0 = 1*[k0 <= +TOL] 1*[k1 <= +TOL] ? ? */
            __ASM_EMIT("andps       %[IONE], %[x1]")        /* xmm1 = 1*[k0 < -TOL] 1*[k1 < -TOL] ? ? */
            __ASM_EMIT("paddd       %[x1], %[x0]")
            __ASM_EMIT("movdqa      %[x0], %[res]")
            __ASM_EMIT32("movl      0x00 + %[res], %[k0]")
            __ASM_EMIT32("movl      0x04 + %[res], %[k1]")
            __ASM_EMIT64("movl      0x00 + %[res], %k[k0]")
            __ASM_EMIT64("movl      0x04 + %[res], %k[k1]")
            __ASM_EMIT("shl         $2, %[k1]")
            __ASM_EMIT("or          %[k1], %[k0]")
            : [k0] "=&r" (k0), [k1] "=&r" (k1),
              [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2)
            : [pl] "r" (pl), [pv] "r" (pv),
              [res] "o" (res),
              [PTOL] "m" (X_3D_TOLERANCE),
              [MTOL] "m" (X_3D_MTOLERANCE),
              [IONE] "m" (IONE)
            : "cc"
        );

        return k0;
    }

    size_t colocation_x3_v1p3(const vector3d_t *pl, const point3d_t *p0, const point3d_t *p1, const point3d_t *p2)
    {
        float x0, x1, x2, x3, x4;
        float res[4] __lsp_aligned16;

        ARCH_X86_ASM
        (
            __ASM_EMIT("movups      (%[pl]), %[x3]")        /* xmm3 = pl    */
            __ASM_EMIT("movups      (%[p0]), %[x0]")        /* xmm0 = p0    */
            __ASM_EMIT("movups      (%[p1]), %[x1]")        /* xmm1 = p1    */
            __ASM_EMIT("movups      (%[p2]), %[x2]")        /* xmm2 = p2    */

            __ASM_EMIT("mulps       %[x3], %[x0]")          /* xmm0 = p0 * pl */
            __ASM_EMIT("mulps       %[x3], %[x1]")          /* xmm1 = p1 * pl */
            __ASM_EMIT("mulps       %[x3], %[x2]")          /* xmm2 = p2 * pl */
            __ASM_EMIT("haddps      %[x1], %[x0]")
            __ASM_EMIT("haddps      %[x3], %[x2]")
            __ASM_EMIT("haddps      %[x2], %[x0]")          /* xmm0 = k0 k1 k2 ? */
            __ASM_EMIT("movaps      %[x0], %[x1]")          /* xmm1 = k0 k1 k2 ? */
            __ASM_EMIT("cmpps       $2, %[PTOL], %[x0]")    /* xmm0 = k0 <= +TOL k1 <= +TOL k2 <= +TOL ? */
            __ASM_EMIT("cmpps       $1, %[MTOL], %[x1]")    /* xmm1 = k0 < -TOL  k1 < -TOL k2 < -TOL ? */
            __ASM_EMIT("andps       %[IONE], %[x0]")        /* xmm0 = 1*[k0 <= +TOL] 1*[k1 <= +TOL] 1*[k2 <= +TOL] ? */
            __ASM_EMIT("andps       %[IONE], %[x1]")        /* xmm1 = 1*[k0 < -TOL] 1*[k1 < -TOL] 1*[k2 < -TOL] ? */
            __ASM_EMIT("paddd       %[x1], %[x0]")
            __ASM_EMIT("movdqa      %[x0], %[res]")
            __ASM_EMIT32("movl      0x00 + %[res], %[p0]")
            __ASM_EMIT32("movl      0x04 + %[res], %[p1]")
            __ASM_EMIT32("movl      0x08 + %[res], %[p2]")
            __ASM_EMIT64("movl      0x00 + %[res], %k[p0]")
            __ASM_EMIT64("movl      0x04 + %[res], %k[p1]")
            __ASM_EMIT64("movl      0x08 + %[res], %k[p2]")
            __ASM_EMIT("shl         $2, %[p1]")
            __ASM_EMIT("shl         $4, %[p2]")
            __ASM_EMIT("or          %[p1], %[p0]")
            __ASM_EMIT("or          %[p2], %[p0]")
            : [p0] "+r" (p0), [p1] "+r" (p1), [p2] "+r" (p2),
              [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3),
              [x4] "=&x" (x4)
            : [pl] "r" (pl),
              [res] "o" (res),
              [PTOL] "m" (X_3D_TOLERANCE),
              [MTOL] "m" (X_3D_MTOLERANCE),
              [IONE] "m" (IONE)
            : "cc"
        );

        return size_t(p0);
    }

    size_t colocation_x3_v1pv(const vector3d_t *pl, const point3d_t *pv)
    {
        float x0, x1, x2, x3, x4;
        float res[4] __lsp_aligned16;
        size_t k0, k1, k2;

        ARCH_X86_ASM
        (
            __ASM_EMIT("movups      (%[pl]), %[x3]")        /* xmm3 = pl    */
            __ASM_EMIT("movups      0x00(%[pv]), %[x0]")    /* xmm0 = p0    */
            __ASM_EMIT("movups      0x10(%[pv]), %[x1]")    /* xmm1 = p1    */
            __ASM_EMIT("movups      0x20(%[pv]), %[x2]")    /* xmm2 = p2    */

            __ASM_EMIT("mulps       %[x3], %[x0]")          /* xmm0 = p0 * pl */
            __ASM_EMIT("mulps       %[x3], %[x1]")          /* xmm1 = p1 * pl */
            __ASM_EMIT("mulps       %[x3], %[x2]")          /* xmm2 = p2 * pl */
            __ASM_EMIT("haddps      %[x1], %[x0]")
            __ASM_EMIT("haddps      %[x3], %[x2]")
            __ASM_EMIT("haddps      %[x2], %[x0]")          /* xmm0 = k0 k1 k2 ? */
            __ASM_EMIT("movaps      %[x0], %[x1]")          /* xmm1 = k0 k1 k2 ? */
            __ASM_EMIT("cmpps       $2, %[PTOL], %[x0]")    /* xmm0 = k0 <= +TOL k1 <= +TOL k2 <= +TOL ? */
            __ASM_EMIT("cmpps       $1, %[MTOL], %[x1]")    /* xmm1 = k0 < -TOL  k1 < -TOL k2 < -TOL ? */
            __ASM_EMIT("andps       %[IONE], %[x0]")        /* xmm0 = 1*[k0 <= +TOL] 1*[k1 <= +TOL] 1*[k2 <= +TOL] ? */
            __ASM_EMIT("andps       %[IONE], %[x1]")        /* xmm1 = 1*[k0 < -TOL] 1*[k1 < -TOL] 1*[k2 < -TOL] ? */
            __ASM_EMIT("paddd       %[x1], %[x0]")
            __ASM_EMIT("movdqa      %[x0], %[res]")
            __ASM_EMIT32("movl      0x00 + %[res], %[k0]")
            __ASM_EMIT32("movl      0x04 + %[res], %[k1]")
            __ASM_EMIT32("movl      0x08 + %[res], %[k2]")
            __ASM_EMIT64("movl      0x00 + %[res], %k[k0]")
            __ASM_EMIT64("movl      0x04 + %[res], %k[k1]")
            __ASM_EMIT64("movl      0x08 + %[res], %k[k2]")
            __ASM_EMIT("shl         $2, %[k1]")
            __ASM_EMIT("shl         $4, %[k2]")
            __ASM_EMIT("or          %[k1], %[k0]")
            __ASM_EMIT("or          %[k2], %[k0]")
            : [k0] "=&r" (k0), [k1] "=&r" (k1), [k2] "=&r" (k2),
              [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3),
              [x4] "=&x" (x4)
            : [pl] "r" (pl), [pv] "r" (pv),
              [res] "o" (res),
              [PTOL] "m" (X_3D_TOLERANCE),
              [MTOL] "m" (X_3D_MTOLERANCE),
              [IONE] "m" (IONE)
            : "cc"
        );

        return k0;
    }

    float calc_min_distance_p3(const point3d_t *sp, const point3d_t *p0, const point3d_t *p1, const point3d_t *p2)
    {
        float x0, x1, x2, x3, x4;

        ARCH_X86_ASM
        (
            __ASM_EMIT("movups      (%[sp]), %[x3]")        /* xmm3 = pl    */
            __ASM_EMIT("movups      (%[p0]), %[x0]")        /* xmm0 = p0    */
            __ASM_EMIT("movups      (%[p1]), %[x1]")        /* xmm1 = p1    */
            __ASM_EMIT("movups      (%[p2]), %[x2]")        /* xmm2 = p2    */

            __ASM_EMIT("subps       %[x3], %[x0]")          /* xmm0 = d0 */
            __ASM_EMIT("subps       %[x3], %[x1]")          /* xmm1 = d1 */
            __ASM_EMIT("mulps       %[x0], %[x0]")          /* xmm0 = d0 * d0 */
            __ASM_EMIT("subps       %[x3], %[x2]")          /* xmm2 = d2 */
            __ASM_EMIT("mulps       %[x1], %[x1]")          /* xmm1 = d1 * d1 */
            __ASM_EMIT("mulps       %[x2], %[x2]")          /* xmm2 = d2 * d2 */
            __ASM_EMIT("haddps      %[x1], %[x0]")
            __ASM_EMIT("haddps      %[x3], %[x2]")
            __ASM_EMIT("haddps      %[x2], %[x0]")          /* xmm0 = k0 k1 k2 ? */
            __ASM_EMIT("sqrtps      %[x0], %[x0]")          /* xmm0 = sqrtf(k0) sqrtf(k1) sqrtf(k2) ? */
            __ASM_EMIT("movhlps     %[x0], %[x1]")          /* xmm1 = sqrtf(k2) */
            __ASM_EMIT("unpcklps    %[x0], %[x0]")          /* xmm0 = sqrtf(k0) sqrtf(k0) sqrtf(k1) sqrtf(k1) */
            __ASM_EMIT("movhlps     %[x0], %[x2]")          /* xmm1 = sqrtf(k2) */
            __ASM_EMIT("minss       %[x1], %[x0]")
            __ASM_EMIT("minss       %[x2], %[x0]")

            : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3),
              [x4] "=&x" (x4)
            : [sp] "r" (sp), [p0] "r" (p0), [p1] "r" (p1), [p2] "r" (p2)
            :
        );

        return x0;
    }

    float calc_min_distance_pv(const point3d_t *sp, const point3d_t *pv)
    {
        float x0, x1, x2, x3, x4;

        ARCH_X86_ASM
        (
            __ASM_EMIT("movups      (%[sp]), %[x3]")        /* xmm3 = pl    */
            __ASM_EMIT("movups      0x00(%[pv]), %[x0]")    /* xmm0 = p0    */
            __ASM_EMIT("movups      0x10(%[pv]), %[x1]")    /* xmm1 = p1    */
            __ASM_EMIT("movups      0x20(%[pv]), %[x2]")    /* xmm2 = p2    */

            __ASM_EMIT("subps       %[x3], %[x0]")          /* xmm0 = d0 */
            __ASM_EMIT("subps       %[x3], %[x1]")          /* xmm1 = d1 */
            __ASM_EMIT("mulps       %[x0], %[x0]")          /* xmm0 = d0 * d0 */
            __ASM_EMIT("subps       %[x3], %[x2]")          /* xmm2 = d2 */
            __ASM_EMIT("mulps       %[x1], %[x1]")          /* xmm1 = d1 * d1 */
            __ASM_EMIT("mulps       %[x2], %[x2]")          /* xmm2 = d2 * d2 */
            __ASM_EMIT("haddps      %[x1], %[x0]")
            __ASM_EMIT("haddps      %[x3], %[x2]")
            __ASM_EMIT("haddps      %[x2], %[x0]")          /* xmm0 = k0 k1 k2 ? */
            __ASM_EMIT("sqrtps      %[x0], %[x0]")          /* xmm0 = sqrtf(k0) sqrtf(k1) sqrtf(k2) ? */
            __ASM_EMIT("movhlps     %[x0], %[x1]")          /* xmm1 = sqrtf(k2) */
            __ASM_EMIT("unpcklps    %[x0], %[x0]")          /* xmm0 = sqrtf(k0) sqrtf(k0) sqrtf(k1) sqrtf(k1) */
            __ASM_EMIT("movhlps     %[x0], %[x2]")          /* xmm1 = sqrtf(k2) */
            __ASM_EMIT("minss       %[x1], %[x0]")
            __ASM_EMIT("minss       %[x2], %[x0]")

            : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3),
              [x4] "=&x" (x4)
            : [sp] "r" (sp), [pv] "r" (pv)
            :
        );

        return x0;
    }
}



#endif /* DSP_ARCH_X86_SSE3_3DMATH_H_ */
