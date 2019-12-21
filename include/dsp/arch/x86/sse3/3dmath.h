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
    void calc_split_point_p2v1(point3d_t *sp, const point3d_t *l0, const point3d_t *l1, const vector3d_t *pl)
    {
        float x0, x1, x2, x3;

        ARCH_X86_ASM
        (
            __ASM_EMIT("movups      (%[l0]), %[x1]")        /* xmm1 = l0 = lx0 ly0 lz0 1 */
            __ASM_EMIT("movups      (%[l1]), %[x0]")        /* xmm0 = l1 = lx1 ly1 lz1 1 */
            __ASM_EMIT("movaps      %[x1], %[x3]")          /* xmm4 = lx0 ly0 lz0 1 */
            __ASM_EMIT("movups      (%[pl]), %[x2]")        /* xmm2 = pl = nx ny nz nw */
            __ASM_EMIT("subps       %[x1], %[x0]")          /* xmm0 = d = l1 - l0 = dx dy dz 0 */

            __ASM_EMIT("mulps       %[x2], %[x1]")          /* xmm1 = nx*lx0 ny*ly0 nz*lz0 nw */
            __ASM_EMIT("mulps       %[x0], %[x2]")          /* xmm2 = nx*dx ny*dy nz*dz 0 */
            __ASM_EMIT("haddps      %[x1], %[x1]")          /* xmm1 = nx*lx0+ny*ly0 nz*lz0+nw nx*lx0+ny*ly0 nz*lz0+nw */
            __ASM_EMIT("haddps      %[x2], %[x2]")          /* xmm2 = nx*dx+ny*dy nz*dz nx*dx+ny*dy nz*dz */
            __ASM_EMIT("haddps      %[x1], %[x1]")          /* xmm1 = nx*lx0+ny*ly0+nz*lz0+nw = T T T T */
            __ASM_EMIT("haddps      %[x2], %[x2]")          /* xmm2 = nx*dx+ny*dy nz*dz B B B B */
            __ASM_EMIT("divps       %[x2], %[x1]")          /* xmm1 = T/B T/B T/B T/B = W */
            __ASM_EMIT("mulps       %[x1], %[x0]")          /* xmm0 = dx*W dy*W dz*W 0 */
            __ASM_EMIT("subps       %[x0], %[x3]")          /* xmm0 = lx0-dx*W ly0-dy*W lz0-dz*W 1 */

            __ASM_EMIT("movups      %[x3], (%[sp])")
            : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3)
            : [sp] "r" (sp), [l0] "r" (l0), [l1] "r" (l1), [pl] "r" (pl)
            : "memory"
        );
    }

    void calc_split_point_pvv1(point3d_t *sp, const point3d_t *lv, const vector3d_t *pl)
    {
        float x0, x1, x2, x3;

        ARCH_X86_ASM
        (
            __ASM_EMIT("movups      0x00(%[lv]), %[x1]")    /* xmm1 = l0 = lx0 ly0 lz0 ? */
            __ASM_EMIT("movups      0x10(%[lv]), %[x0]")    /* xmm0 = l1 = lx1 ly1 lz1 ? */
            __ASM_EMIT("movaps      %[x1], %[x3]")          /* xmm4 = lx0 ly0 lz0 1 */
            __ASM_EMIT("movups      (%[pl]), %[x2]")        /* xmm2 = pl = nx ny nz nw */
            __ASM_EMIT("subps       %[x1], %[x0]")          /* xmm0 = d = l1 - l0 = dx dy dz 0 */

            __ASM_EMIT("mulps       %[x2], %[x1]")          /* xmm1 = nx*lx0 ny*ly0 nz*lz0 nw */
            __ASM_EMIT("mulps       %[x0], %[x2]")          /* xmm2 = nx*dx ny*dy nz*dz 0 */
            __ASM_EMIT("haddps      %[x1], %[x1]")          /* xmm1 = nx*lx0+ny*ly0 nz*lz0+nw nx*lx0+ny*ly0 nz*lz0+nw */
            __ASM_EMIT("haddps      %[x2], %[x2]")          /* xmm2 = nx*dx+ny*dy nz*dz nx*dx+ny*dy nz*dz */
            __ASM_EMIT("haddps      %[x1], %[x1]")          /* xmm1 = nx*lx0+ny*ly0+nz*lz0+nw = T T T T */
            __ASM_EMIT("haddps      %[x2], %[x2]")          /* xmm2 = nx*dx+ny*dy nz*dz B B B B */
            __ASM_EMIT("divps       %[x2], %[x1]")          /* xmm1 = T/B T/B T/B T/B = W */
            __ASM_EMIT("mulps       %[x1], %[x0]")          /* xmm0 = dx*W dy*W dz*W 0 */
            __ASM_EMIT("subps       %[x0], %[x3]")          /* xmm0 = lx0-dx*W ly0-dy*W lz0-dz*W 1 */

            __ASM_EMIT("movups      %[x3], (%[sp])")

            : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3)
            : [sp] "r" (sp), [lv] "r" (lv), [pl] "r" (pl)
            : "memory"
        );
    }

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
            __ASM_EMIT("lea         (%[p0], %[p1], 4), %[p0]")
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
            __ASM_EMIT("lea         (%[k0], %[k1], 4), %[k0]")
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
            __ASM_EMIT("lea         (%[p1], %[p2], 4), %[p1]")
            __ASM_EMIT("lea         (%[p0], %[p1], 4), %[p0]")
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
            __ASM_EMIT("lea         (%[k1], %[k2], 4), %[k1]")
            __ASM_EMIT("lea         (%[k0], %[k1], 4), %[k0]")
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

    size_t colocation_x3_v3p1(const vector3d_t *v0, const vector3d_t *v1, const vector3d_t *v2, const point3d_t *p)
    {
        float x0, x1, x2, x3, x4;
        float res[4] __lsp_aligned16;

        ARCH_X86_ASM
        (
            __ASM_EMIT("movups      (%[p]), %[x3]")         /* xmm3 = p     */
            __ASM_EMIT("movups      (%[v0]), %[x0]")        /* xmm0 = v0    */
            __ASM_EMIT("movups      (%[v1]), %[x1]")        /* xmm1 = v1    */
            __ASM_EMIT("movups      (%[v2]), %[x2]")        /* xmm2 = v2    */

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
            __ASM_EMIT32("movl      0x00 + %[res], %[v0]")
            __ASM_EMIT32("movl      0x04 + %[res], %[v1]")
            __ASM_EMIT32("movl      0x08 + %[res], %[v2]")
            __ASM_EMIT64("movl      0x00 + %[res], %k[v0]")
            __ASM_EMIT64("movl      0x04 + %[res], %k[v1]")
            __ASM_EMIT64("movl      0x08 + %[res], %k[v2]")
            __ASM_EMIT("lea         (%[v1], %[v2], 4), %[v1]")
            __ASM_EMIT("lea         (%[v0], %[v1], 4), %[v0]")

            : [v0] "+r" (v0), [v1] "+r" (v1), [v2] "+r" (v2),
              [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3),
              [x4] "=&x" (x4)
            : [p] "r" (p),
              [res] "o" (res),
              [PTOL] "m" (X_3D_TOLERANCE),
              [MTOL] "m" (X_3D_MTOLERANCE),
              [IONE] "m" (IONE)
            : "cc"
        );

        return size_t(v0);
    }

    size_t colocation_x3_vvp1(const vector3d_t *vv, const point3d_t *p)
    {
        float x0, x1, x2, x3, x4;
        float res[4] __lsp_aligned16;
        size_t k0, k1, k2;

        ARCH_X86_ASM
        (
            __ASM_EMIT("movups      (%[p]), %[x3]")         /* xmm3 = p     */
            __ASM_EMIT("movups      0x00(%[vv]), %[x0]")    /* xmm0 = v0    */
            __ASM_EMIT("movups      0x10(%[vv]), %[x1]")    /* xmm1 = v1    */
            __ASM_EMIT("movups      0x20(%[vv]), %[x2]")    /* xmm2 = v2    */

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
            __ASM_EMIT("lea         (%[k1], %[k2], 4), %[k1]")
            __ASM_EMIT("lea         (%[k0], %[k1], 4), %[k0]")

            : [k0] "=&r" (k0), [k1] "=&r" (k1), [k2] "=&r" (k2),
              [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3),
              [x4] "=&x" (x4)
            : [p] "r" (p), [vv] "r" (vv),
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

    void split_triangle_raw(
            raw_triangle_t *out,
            size_t *n_out,
            raw_triangle_t *in,
            size_t *n_in,
            const vector3d_t *pl,
            const raw_triangle_t *pv
        )
    {
        float    x0, x1, x2, x3, x4, x5, x6, x7;
        float    k[4] __lsp_aligned16;
        size_t   t0;

        in     += *n_in;
        out    += *n_out;

        // Estimate co-location of triangle and point
        ARCH_X86_ASM
        (
            __ASM_EMIT("movups      (%[pl]), %[x3]")        /* xmm3 = pl    */
            __ASM_EMIT("movups      0x00(%[pv]), %[x0]")    /* xmm0 = p0    */
            __ASM_EMIT("movups      0x10(%[pv]), %[x1]")    /* xmm1 = p1    */
            __ASM_EMIT("movups      0x20(%[pv]), %[x2]")    /* xmm2 = p2    */

            __ASM_EMIT("mulps       %[x3], %[x0]")          /* xmm0 = p0 * pl = sx0 sy0 sz0 sw0 */
            __ASM_EMIT("mulps       %[x3], %[x1]")          /* xmm1 = p1 * pl = sx1 sy1 sz1 sw1 */
            __ASM_EMIT("mulps       %[x3], %[x2]")          /* xmm2 = p2 * pl = sx2 sy2 sz2 sw2 */
            __ASM_EMIT("haddps      %[x1], %[x0]")          /* xmm0 = sx0+sy0 sz0+sw0 sx1+sy1 sz1+sw1 */
            __ASM_EMIT("haddps      %[x2], %[x2]")          /* xmm2 = sx2+sy2 sz2+sw2 sx2+sy2 sz2+sw2 */
            __ASM_EMIT("haddps      %[x2], %[x0]")          /* xmm0 = sx0+sy0+sz0+sw0 sx1+sy1+sz1+sw1 sx2+sy2+sz2+sw2 sx2+sy2+sz2+sw2 = k0 k1 k2 k2 */
            __ASM_EMIT("movaps      %[x0], %[x1]")          /* xmm1 = k0 k1 k2 k2 */
            __ASM_EMIT("movaps      %[x0], %[k]")           /* *k   = xmm0 */
            __ASM_EMIT("cmpps       $2, %[PTOL], %[x0]")    /* xmm0 = k0 <= +TOL k1 <= +TOL k2 <= +TOL k2 <= +TOL */
            __ASM_EMIT("cmpps       $1, %[MTOL], %[x1]")    /* xmm1 = k0 < -TOL  k1 < -TOL k2 < -TOL k2 < -TOL */
            __ASM_EMIT("pand        %[ICULL3], %[x0]")      /* xmm0 = 1*[k0 <= +TOL] 1*[k1 <= +TOL] 1*[k2 <= +TOL] 0 */
            __ASM_EMIT("pand        %[ICULL3], %[x1]")      /* xmm1 = 1*[k0 < -TOL] 1*[k1 < -TOL] 1*[k2 < -TOL] 0 */
            __ASM_EMIT("paddd       %[x1], %[x0]")          /* xmm0 = x0 x1 x2 0 */
            __ASM_EMIT("movdqa      %[x0], %[x2]")          /* xmm2 = x0 x1 x2 0 */
            __ASM_EMIT("movdqa      %[x0], %[x1]")          /* xmm1 = x0 x1 x2 0 */
            __ASM_EMIT("psrldq      $8, %[x2]")             /* xmm2 = x2 0 0 0 */
            __ASM_EMIT("psrldq      $4, %[x1]")             /* xmm1 = x1 x2 0 0 */
            __ASM_EMIT("por         %[x2], %[x0]")
            __ASM_EMIT("por         %[x1], %[x0]")
            __ASM_EMIT32("movd      %[x0], %[t0]")
            __ASM_EMIT64("movd      %[x0], %k[t0]")
            : [t0] "=&r" (t0),
              [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3),
              [x4] "=&x" (x4)
            : [pl] "r" (pl), [pv] "r" (pv),
              [k] "o" (k),
              [PTOL] "m" (X_3D_TOLERANCE),
              [MTOL] "m" (X_3D_MTOLERANCE),
              [ICULL3] "m" (ICULL3)
            : "cc"
        );

        #define STR_COPY_TO(tgt) \
            ARCH_X86_ASM( \
                __ASM_EMIT("movups      0x00(%[src]), %[x0]") \
                __ASM_EMIT("movups      0x10(%[src]), %[x1]") \
                __ASM_EMIT("movups      0x20(%[src]), %[x2]") \
                __ASM_EMIT("movups      %[x0], 0x00(%[dst])") \
                __ASM_EMIT("movups      %[x1], 0x10(%[dst])") \
                __ASM_EMIT("movups      %[x2], 0x20(%[dst])") \
                : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2) \
                : [dst] "r" (tgt), \
                  [src] "r" (pv) \
                : "cc", "memory" \
            );

        #define STR_SPLIT_1P(off0, off1, koff, store) \
            ARCH_X86_ASM( \
                __ASM_EMIT("movups      0x" off0 "(%[st]), %[x0]")      /* xmm0 = p0 = lx0 ly0 lz0 1 */ \
                __ASM_EMIT("movups      0x" off1 "(%[st]), %[x1]")      /* xmm1 = p1 = lx1 ly1 lz1 1 */ \
                __ASM_EMIT("movups      (%[pl]), %[x2]")                /* xmm2 = pl = nx ny nz nw */ \
                \
                __ASM_EMIT("subps       %[x0], %[x1]")                  /* xmm1 = d = p1 - p0 = dx dy dz 0 */ \
                __ASM_EMIT("movss       0x" koff " + %[k], %[x3]")      /* xmm3 = k 0 0 0 */ \
                __ASM_EMIT("mulps       %[x1], %[x2]")                  /* xmm2 = dx*nx dy*ny dz*nz 0 */ \
                __ASM_EMIT("shufps      $0x00, %[x3], %[x3]")           /* xmm3 = k k k k */ \
                __ASM_EMIT("haddps      %[x2], %[x2]")                  /* xmm2 = dx*nx+dy*ny dz*nz dx*nx+dy*ny dz*nz */ \
                __ASM_EMIT("mulps       %[x3], %[x1]")                  /* xmm1 = k*dx k*dy k*dz 0 */ \
                __ASM_EMIT("haddps      %[x2], %[x2]")                  /* xmm2 = dx*nx+dy*ny+dz*nz = b b b b */ \
                __ASM_EMIT("divps       %[x2], %[x1]")                  /* xmm1 = k*dx/b k*dy/b k*dz/b 0 */ \
                __ASM_EMIT("subps       %[x1], %[x0]")                  /* xmm0 = lx0-k*dx/b ly0-k*dy/b lz0-k*dz/b 1 */ \
                \
                store \
                \
                : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3), [x4] "=&x" (x4) \
                : [st] "r" (pv), [pl] "r" (pl), [out] "r" (out), [in] "r" (in), \
                  [k] "o" (k) \
                : "cc", "memory" \
            );

        #define STR_SPLIT_2P(off0, off1, off2, koff, store) \
            ARCH_X86_ASM( \
                __ASM_EMIT("movups      0x" off0 "(%[st]), %[x0]")      /* xmm0 = p0 = lx0 ly0 lz0 1 */ \
                __ASM_EMIT("movups      0x" off1 "(%[st]), %[x2]")      /* xmm2 = p1 = lx1 ly1 lz1 1 */ \
                __ASM_EMIT("movups      0x" off2 "(%[st]), %[x3]")      /* xmm3 = p2 = lx2 ly2 lz2 1 */ \
                __ASM_EMIT("movups      (%[pl]), %[x4]")                /* xmm4 = pl = nx ny nz nw */ \
                \
                __ASM_EMIT("movaps      %[x0], %[x1]")                  /* xmm1 = pl = nx ny nz nw */ \
                __ASM_EMIT("movaps      %[x4], %[x5]")                  /* xmm5 = pl = nx ny nz nw */ \
                __ASM_EMIT("subps       %[x0], %[x2]")                  /* xmm2 = d1 = p1 - p0 = dx1 dy1 dz1 0 */ \
                __ASM_EMIT("subps       %[x1], %[x3]")                  /* xmm3 = d2 = p2 - p0 = dx2 dy2 dz2 0 */ \
                __ASM_EMIT("mulps       %[x2], %[x4]")                  /* xmm4 = dx1*nx dy1*ny dz1*nz 0 */ \
                __ASM_EMIT("mulps       %[x3], %[x5]")                  /* xmm5 = dx2*nx dy2*ny dz2*nz 0 */ \
                __ASM_EMIT("movss       0x" koff " + %[k], %[x6]")      /* xmm6 = k */ \
                __ASM_EMIT("haddps      %[x4], %[x4]")                  /* xmm4 = dx1*nx+dy1*ny dz1*nz dx1*nx+dy1*ny dz1*nz */ \
                __ASM_EMIT("haddps      %[x5], %[x5]")                  /* xmm5 = dx2*nx+dy2*ny dz2*nz dx2*nx+dy2*ny dz2*nz */ \
                __ASM_EMIT("shufps      $0x00, %[x6], %[x6]")           /* xmm6 = k k k k */ \
                __ASM_EMIT("haddps      %[x4], %[x4]")                  /* xmm4 = dx1*nx+dy1*ny+dz1*nz = b1 b1 b1 b1 */ \
                __ASM_EMIT("haddps      %[x5], %[x5]")                  /* xmm5 = dx2*nx+dy2*ny+dz2*nz = b2 b2 b2 b2 */ \
                __ASM_EMIT("divps       %[x4], %[x2]")                  /* xmm2 = dx1/b1 dy1/b1 dz1/b1 0 */ \
                __ASM_EMIT("divps       %[x5], %[x3]")                  /* xmm3 = dx2/b2 dy2/b2 dz2/b2 0 */ \
                __ASM_EMIT("mulps       %[x6], %[x2]")                  /* xmm2 = k*dx1/b1 k*dy1/b1 k*dz1/b1 0 */ \
                __ASM_EMIT("mulps       %[x6], %[x3]")                  /* xmm3 = k*dx2/b2 k*dy2/b2 k*dz2/b2 0 */ \
                __ASM_EMIT("subps       %[x2], %[x0]")                  /* xmm0 = lx0-k*dx1/b1 ly0-k*dy1/b1 lz0-k*dz1/b1 1 */ \
                __ASM_EMIT("subps       %[x3], %[x1]")                  /* xmm1 = lx1-k*dx2/b2 ly1-k*dy2/b2 lz1-k*dz2/b2 1 */ \
                \
                store \
                \
                : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3), \
                  [x4] "=&x" (x4), [x5] "=&x" (x5), [x6] "=&x" (x6), [x7] "=&x" (x7) \
                : [st] "r" (pv), [pl] "r" (pl), [out] "r" (out), [in] "r" (in), \
                  [k] "o" (k) \
                : "cc", "memory" \
            );

        switch (t0)
        {
            // 0 intersections, triangle is above
            case 0x00:  // 0 0 0
            case 0x01:  // 0 0 1
            case 0x04:  // 0 1 0
            case 0x05:  // 0 1 1
            case 0x10:  // 1 0 0
            case 0x11:  // 1 0 1
            case 0x14:  // 1 1 0
            case 0x15:  // 1 1 1
                STR_COPY_TO(out);
                ++(*n_out);
                break;

            // 0 intersections, triangle is below
            case 0x16:  // 1 1 2
            case 0x19:  // 1 2 1
            case 0x1a:  // 1 2 2
            case 0x25:  // 2 1 1
            case 0x26:  // 2 1 2
            case 0x29:  // 2 2 1
            case 0x2a:  // 2 2 2
                STR_COPY_TO(in);
                ++(*n_in);
                break;

            // 1 intersection, 1 triangle above, 1 triangle below, counter-clockwise
            case 0x06:  // 0 1 2
                STR_SPLIT_1P("00", "20", "00",
                    __ASM_EMIT("movups      0x00(%[st]), %[x1]")
                    __ASM_EMIT("movups      0x10(%[st]), %[x2]")
                    __ASM_EMIT("movups      0x20(%[st]), %[x3]")
                    __ASM_EMIT("movups      %[x1], 0x00(%[in])")
                    __ASM_EMIT("movups      %[x2], 0x10(%[in])")
                    __ASM_EMIT("movups      %[x0], 0x20(%[in])")
                    __ASM_EMIT("movups      %[x2], 0x00(%[out])")
                    __ASM_EMIT("movups      %[x3], 0x10(%[out])")
                    __ASM_EMIT("movups      %[x0], 0x20(%[out])")
                );
                ++(*n_out); ++(*n_in);
                break;
            case 0x21:  // 2 0 1
                STR_SPLIT_1P("00", "10", "00",
                    __ASM_EMIT("movups      0x00(%[st]), %[x1]")
                    __ASM_EMIT("movups      0x10(%[st]), %[x2]")
                    __ASM_EMIT("movups      0x20(%[st]), %[x3]")
                    __ASM_EMIT("movups      %[x3], 0x00(%[in])")
                    __ASM_EMIT("movups      %[x1], 0x10(%[in])")
                    __ASM_EMIT("movups      %[x0], 0x20(%[in])")
                    __ASM_EMIT("movups      %[x2], 0x00(%[out])")
                    __ASM_EMIT("movups      %[x0], 0x10(%[out])")
                    __ASM_EMIT("movups      %[x1], 0x20(%[out])")
                );
                ++(*n_out); ++(*n_in);
                break;
            case 0x18:  // 1 2 0
                STR_SPLIT_1P("10", "20", "04",
                    __ASM_EMIT("movups      0x00(%[st]), %[x1]")
                    __ASM_EMIT("movups      0x10(%[st]), %[x2]")
                    __ASM_EMIT("movups      0x20(%[st]), %[x3]")
                    __ASM_EMIT("movups      %[x2], 0x00(%[in])")
                    __ASM_EMIT("movups      %[x3], 0x10(%[in])")
                    __ASM_EMIT("movups      %[x0], 0x20(%[in])")
                    __ASM_EMIT("movups      %[x1], 0x00(%[out])")
                    __ASM_EMIT("movups      %[x0], 0x10(%[out])")
                    __ASM_EMIT("movups      %[x3], 0x20(%[out])")
                );
                ++(*n_out); ++(*n_in);
                break;

            // 1 intersection, 1 triangle above, 1 triangle below, clockwise
            case 0x24:  // 2 1 0
                STR_SPLIT_1P("00", "20", "00",
                    __ASM_EMIT("movups      0x00(%[st]), %[x1]")
                    __ASM_EMIT("movups      0x10(%[st]), %[x2]")
                    __ASM_EMIT("movups      0x20(%[st]), %[x3]")
                    __ASM_EMIT("movups      %[x3], 0x00(%[in])")
                    __ASM_EMIT("movups      %[x0], 0x10(%[in])")
                    __ASM_EMIT("movups      %[x1], 0x20(%[in])")
                    __ASM_EMIT("movups      %[x1], 0x00(%[out])")
                    __ASM_EMIT("movups      %[x2], 0x10(%[out])")
                    __ASM_EMIT("movups      %[x0], 0x20(%[out])")
                );
                ++(*n_out); ++(*n_in);
                break;
            case 0x12:  // 1 0 2
                STR_SPLIT_1P("00", "10", "00",
                    __ASM_EMIT("movups      0x00(%[st]), %[x1]")
                    __ASM_EMIT("movups      0x10(%[st]), %[x2]")
                    __ASM_EMIT("movups      0x20(%[st]), %[x3]")
                    __ASM_EMIT("movups      %[x1], 0x00(%[in])")
                    __ASM_EMIT("movups      %[x0], 0x10(%[in])")
                    __ASM_EMIT("movups      %[x3], 0x20(%[in])")
                    __ASM_EMIT("movups      %[x2], 0x00(%[out])")
                    __ASM_EMIT("movups      %[x3], 0x10(%[out])")
                    __ASM_EMIT("movups      %[x0], 0x20(%[out])")
                );
                ++(*n_out); ++(*n_in);
                break;
            case 0x09:  // 0 2 1
                STR_SPLIT_1P("10", "20", "04",
                    __ASM_EMIT("movups      0x00(%[st]), %[x1]")
                    __ASM_EMIT("movups      0x10(%[st]), %[x2]")
                    __ASM_EMIT("movups      0x20(%[st]), %[x3]")
                    __ASM_EMIT("movups      %[x2], 0x00(%[in])")
                    __ASM_EMIT("movups      %[x0], 0x10(%[in])")
                    __ASM_EMIT("movups      %[x1], 0x20(%[in])")
                    __ASM_EMIT("movups      %[x3], 0x00(%[out])")
                    __ASM_EMIT("movups      %[x1], 0x10(%[out])")
                    __ASM_EMIT("movups      %[x0], 0x20(%[out])")
                );
                ++(*n_out); ++(*n_in);
                break;

            // 2 intersections, 1 triangle below, 2 triangles above
            case 0x02:  // 0 0 2
                STR_SPLIT_2P("00", "10", "20", "00",
                    __ASM_EMIT("movups      0x00(%[st]), %[x2]")
                    __ASM_EMIT("movups      0x10(%[st]), %[x3]")
                    __ASM_EMIT("movups      0x20(%[st]), %[x4]")
                    __ASM_EMIT("movups      %[x2], 0x00(%[in])")
                    __ASM_EMIT("movups      %[x0], 0x10(%[in])")
                    __ASM_EMIT("movups      %[x1], 0x20(%[in])")
                    __ASM_EMIT("movups      %[x3], 0x00(%[out])")
                    __ASM_EMIT("movups      %[x1], 0x10(%[out])")
                    __ASM_EMIT("movups      %[x0], 0x20(%[out])")
                    __ASM_EMIT("movups      %[x4], 0x30(%[out])")
                    __ASM_EMIT("movups      %[x1], 0x40(%[out])")
                    __ASM_EMIT("movups      %[x3], 0x50(%[out])")
                );
                (*n_out) += 2; ++(*n_in);
                break;
            case 0x08:  // 0 2 0
                STR_SPLIT_2P("10", "00", "20", "04",
                    __ASM_EMIT("movups      0x00(%[st]), %[x2]")
                    __ASM_EMIT("movups      0x10(%[st]), %[x3]")
                    __ASM_EMIT("movups      0x20(%[st]), %[x4]")
                    __ASM_EMIT("movups      %[x3], 0x00(%[in])")
                    __ASM_EMIT("movups      %[x1], 0x10(%[in])")
                    __ASM_EMIT("movups      %[x0], 0x20(%[in])")
                    __ASM_EMIT("movups      %[x4], 0x00(%[out])")
                    __ASM_EMIT("movups      %[x0], 0x10(%[out])")
                    __ASM_EMIT("movups      %[x1], 0x20(%[out])")
                    __ASM_EMIT("movups      %[x2], 0x30(%[out])")
                    __ASM_EMIT("movups      %[x0], 0x40(%[out])")
                    __ASM_EMIT("movups      %[x4], 0x50(%[out])")
                );
                (*n_out) += 2; ++(*n_in);
                break;
            case 0x20:  // 2 0 0
                STR_SPLIT_2P("20", "00", "10", "08",
                    __ASM_EMIT("movups      0x00(%[st]), %[x2]")
                    __ASM_EMIT("movups      0x10(%[st]), %[x3]")
                    __ASM_EMIT("movups      0x20(%[st]), %[x4]")
                    __ASM_EMIT("movups      %[x4], 0x00(%[in])")
                    __ASM_EMIT("movups      %[x0], 0x10(%[in])")
                    __ASM_EMIT("movups      %[x1], 0x20(%[in])")
                    __ASM_EMIT("movups      %[x2], 0x00(%[out])")
                    __ASM_EMIT("movups      %[x1], 0x10(%[out])")
                    __ASM_EMIT("movups      %[x0], 0x20(%[out])")
                    __ASM_EMIT("movups      %[x3], 0x30(%[out])")
                    __ASM_EMIT("movups      %[x1], 0x40(%[out])")
                    __ASM_EMIT("movups      %[x2], 0x50(%[out])")
                );
                (*n_out) += 2; ++(*n_in);
                break;

            // 2 intersections, 1 triangle above, 2 triangles below
            case 0x28:  // 2 2 0
                STR_SPLIT_2P("00", "10", "20", "00",
                    __ASM_EMIT("movups      0x00(%[st]), %[x2]")
                    __ASM_EMIT("movups      0x10(%[st]), %[x3]")
                    __ASM_EMIT("movups      0x20(%[st]), %[x4]")
                    __ASM_EMIT("movups      %[x3], 0x00(%[in])")
                    __ASM_EMIT("movups      %[x1], 0x10(%[in])")
                    __ASM_EMIT("movups      %[x0], 0x20(%[in])")
                    __ASM_EMIT("movups      %[x4], 0x30(%[in])")
                    __ASM_EMIT("movups      %[x1], 0x40(%[in])")
                    __ASM_EMIT("movups      %[x3], 0x50(%[in])")
                    __ASM_EMIT("movups      %[x2], 0x00(%[out])")
                    __ASM_EMIT("movups      %[x0], 0x10(%[out])")
                    __ASM_EMIT("movups      %[x1], 0x20(%[out])")
                );
                ++(*n_out); (*n_in) += 2;
                break;

            case 0x22:  // 2 0 2
                STR_SPLIT_2P("10", "00", "20", "04",
                    __ASM_EMIT("movups      0x00(%[st]), %[x2]")
                    __ASM_EMIT("movups      0x10(%[st]), %[x3]")
                    __ASM_EMIT("movups      0x20(%[st]), %[x4]")
                    __ASM_EMIT("movups      %[x4], 0x00(%[in])")
                    __ASM_EMIT("movups      %[x0], 0x10(%[in])")
                    __ASM_EMIT("movups      %[x1], 0x20(%[in])")
                    __ASM_EMIT("movups      %[x2], 0x30(%[in])")
                    __ASM_EMIT("movups      %[x0], 0x40(%[in])")
                    __ASM_EMIT("movups      %[x4], 0x50(%[in])")
                    __ASM_EMIT("movups      %[x3], 0x00(%[out])")
                    __ASM_EMIT("movups      %[x1], 0x10(%[out])")
                    __ASM_EMIT("movups      %[x0], 0x20(%[out])")
                );
                ++(*n_out); (*n_in) += 2;
                break;

            case 0x0a:  // 0 2 2
                STR_SPLIT_2P("20", "00", "10", "08",
                    __ASM_EMIT("movups      0x00(%[st]), %[x2]")
                    __ASM_EMIT("movups      0x10(%[st]), %[x3]")
                    __ASM_EMIT("movups      0x20(%[st]), %[x4]")
                    __ASM_EMIT("movups      %[x2], 0x00(%[in])")
                    __ASM_EMIT("movups      %[x1], 0x10(%[in])")
                    __ASM_EMIT("movups      %[x0], 0x20(%[in])")
                    __ASM_EMIT("movups      %[x3], 0x30(%[in])")
                    __ASM_EMIT("movups      %[x1], 0x40(%[in])")
                    __ASM_EMIT("movups      %[x2], 0x50(%[in])")
                    __ASM_EMIT("movups      %[x4], 0x00(%[out])")
                    __ASM_EMIT("movups      %[x0], 0x10(%[out])")
                    __ASM_EMIT("movups      %[x1], 0x20(%[out])")
                );
                ++(*n_out); (*n_in) += 2;
                break;

            default:
                break;
        }

        #undef STR_COPY_TO
        #undef STR_SPLIT_1P
        #undef STR_SPLIT_2P
    }

    void cull_triangle_raw(
            raw_triangle_t *in,
            size_t *n_in,
            const vector3d_t *pl,
            const raw_triangle_t *pv
        )
    {
        float    x0, x1, x2, x3, x4, x5, x6, x7;
        float    k[4] __lsp_aligned16;
        size_t   t0;

        in     += *n_in;

        // Estimate co-location of triangle and point
        ARCH_X86_ASM
        (
            __ASM_EMIT("movups      (%[pl]), %[x3]")        /* xmm3 = pl    */
            __ASM_EMIT("movups      0x00(%[pv]), %[x0]")    /* xmm0 = p0    */
            __ASM_EMIT("movups      0x10(%[pv]), %[x1]")    /* xmm1 = p1    */
            __ASM_EMIT("movups      0x20(%[pv]), %[x2]")    /* xmm2 = p2    */

            __ASM_EMIT("mulps       %[x3], %[x0]")          /* xmm0 = p0 * pl = sx0 sy0 sz0 sw0 */
            __ASM_EMIT("mulps       %[x3], %[x1]")          /* xmm1 = p1 * pl = sx1 sy1 sz1 sw1 */
            __ASM_EMIT("mulps       %[x3], %[x2]")          /* xmm2 = p2 * pl = sx2 sy2 sz2 sw2 */
            __ASM_EMIT("haddps      %[x1], %[x0]")          /* xmm0 = sx0+sy0 sz0+sw0 sx1+sy1 sz1+sw1 */
            __ASM_EMIT("haddps      %[x2], %[x2]")          /* xmm2 = sx2+sy2 sz2+sw2 sx2+sy2 sz2+sw2 */
            __ASM_EMIT("haddps      %[x2], %[x0]")          /* xmm0 = sx0+sy0+sz0+sw0 sx1+sy1+sz1+sw1 sx2+sy2+sz2+sw2 sx2+sy2+sz2+sw2 = k0 k1 k2 k2 */
            __ASM_EMIT("movaps      %[x0], %[x1]")          /* xmm1 = k0 k1 k2 k2 */
            __ASM_EMIT("movaps      %[x0], %[k]")           /* *k   = xmm0 */
            __ASM_EMIT("cmpps       $2, %[PTOL], %[x0]")    /* xmm0 = k0 <= +TOL k1 <= +TOL k2 <= +TOL k2 <= +TOL */
            __ASM_EMIT("cmpps       $1, %[MTOL], %[x1]")    /* xmm1 = k0 < -TOL  k1 < -TOL k2 < -TOL k2 < -TOL */
            __ASM_EMIT("pand        %[ICULL3], %[x0]")      /* xmm0 = 1*[k0 <= +TOL] 1*[k1 <= +TOL] 1*[k2 <= +TOL] 0 */
            __ASM_EMIT("pand        %[ICULL3], %[x1]")      /* xmm1 = 1*[k0 < -TOL] 1*[k1 < -TOL] 1*[k2 < -TOL] 0 */
            __ASM_EMIT("paddd       %[x1], %[x0]")          /* xmm0 = x0 x1 x2 0 */
            __ASM_EMIT("movdqa      %[x0], %[x2]")          /* xmm2 = x0 x1 x2 0 */
            __ASM_EMIT("movdqa      %[x0], %[x1]")          /* xmm1 = x0 x1 x2 0 */
            __ASM_EMIT("psrldq      $8, %[x2]")             /* xmm2 = x2 0 0 0 */
            __ASM_EMIT("psrldq      $4, %[x1]")             /* xmm1 = x1 x2 0 0 */
            __ASM_EMIT("por         %[x2], %[x0]")
            __ASM_EMIT("por         %[x1], %[x0]")
            __ASM_EMIT32("movd      %[x0], %[t0]")
            __ASM_EMIT64("movd      %[x0], %k[t0]")

            : [t0] "=&r" (t0),
              [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3),
              [x4] "=&x" (x4)
            : [pl] "r" (pl), [pv] "r" (pv),
              [k] "o" (k),
              [PTOL] "m" (X_3D_TOLERANCE),
              [MTOL] "m" (X_3D_MTOLERANCE),
              [ICULL3] "m" (ICULL3)
            : "cc"
        );

        #define STR_COPY_TO(tgt) \
            ARCH_X86_ASM( \
                __ASM_EMIT("movups      0x00(%[src]), %[x0]") \
                __ASM_EMIT("movups      0x10(%[src]), %[x1]") \
                __ASM_EMIT("movups      0x20(%[src]), %[x2]") \
                __ASM_EMIT("movups      %[x0], 0x00(%[dst])") \
                __ASM_EMIT("movups      %[x1], 0x10(%[dst])") \
                __ASM_EMIT("movups      %[x2], 0x20(%[dst])") \
                : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2) \
                : [dst] "r" (tgt), \
                  [src] "r" (pv) \
                : "cc", "memory" \
            );

        #define STR_SPLIT_1P(off0, off1, koff, store) \
            ARCH_X86_ASM( \
                __ASM_EMIT("movups      0x" off0 "(%[st]), %[x0]")      /* xmm0 = p0 = lx0 ly0 lz0 1 */ \
                __ASM_EMIT("movups      0x" off1 "(%[st]), %[x1]")      /* xmm1 = p1 = lx1 ly1 lz1 1 */ \
                __ASM_EMIT("movups      (%[pl]), %[x2]")                /* xmm2 = pl = nx ny nz nw */ \
                \
                __ASM_EMIT("subps       %[x0], %[x1]")                  /* xmm1 = d = p1 - p0 = dx dy dz 0 */ \
                __ASM_EMIT("movss       0x" koff " + %[k], %[x3]")      /* xmm3 = k 0 0 0 */ \
                __ASM_EMIT("mulps       %[x1], %[x2]")                  /* xmm2 = dx*nx dy*ny dz*nz 0 */ \
                __ASM_EMIT("shufps      $0x00, %[x3], %[x3]")           /* xmm3 = k k k k */ \
                __ASM_EMIT("haddps      %[x2], %[x2]")                  /* xmm2 = dx*nx+dy*ny dz*nz dx*nx+dy*ny dz*nz */ \
                __ASM_EMIT("mulps       %[x3], %[x1]")                  /* xmm1 = k*dx k*dy k*dz 0 */ \
                __ASM_EMIT("haddps      %[x2], %[x2]")                  /* xmm2 = dx*nx+dy*ny+dz*nz = b b b b */ \
                __ASM_EMIT("divps       %[x2], %[x1]")                  /* xmm1 = k*dx/b k*dy/b k*dz/b 0 */ \
                __ASM_EMIT("subps       %[x1], %[x0]")                  /* xmm0 = lx0-k*dx/b ly0-k*dy/b lz0-k*dz/b 1 */ \
                \
                store \
                \
                : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3), [x4] "=&x" (x4) \
                : [st] "r" (pv), [pl] "r" (pl), [in] "r" (in), \
                  [k] "o" (k) \
                : "cc", "memory" \
            );

        #define STR_SPLIT_2P(off0, off1, off2, koff, store) \
            ARCH_X86_ASM( \
                    __ASM_EMIT("movups      0x" off0 "(%[st]), %[x0]")      /* xmm0 = p0 = lx0 ly0 lz0 1 */ \
                    __ASM_EMIT("movups      0x" off1 "(%[st]), %[x2]")      /* xmm2 = p1 = lx1 ly1 lz1 1 */ \
                    __ASM_EMIT("movups      0x" off2 "(%[st]), %[x3]")      /* xmm3 = p2 = lx2 ly2 lz2 1 */ \
                    __ASM_EMIT("movups      (%[pl]), %[x4]")                /* xmm4 = pl = nx ny nz nw */ \
                    \
                    __ASM_EMIT("movaps      %[x0], %[x1]")                  /* xmm1 = pl = nx ny nz nw */ \
                    __ASM_EMIT("movaps      %[x4], %[x5]")                  /* xmm5 = pl = nx ny nz nw */ \
                    __ASM_EMIT("subps       %[x0], %[x2]")                  /* xmm2 = d1 = p1 - p0 = dx1 dy1 dz1 0 */ \
                    __ASM_EMIT("subps       %[x1], %[x3]")                  /* xmm3 = d2 = p2 - p0 = dx2 dy2 dz2 0 */ \
                    __ASM_EMIT("mulps       %[x2], %[x4]")                  /* xmm4 = dx1*nx dy1*ny dz1*nz 0 */ \
                    __ASM_EMIT("mulps       %[x3], %[x5]")                  /* xmm5 = dx2*nx dy2*ny dz2*nz 0 */ \
                    __ASM_EMIT("movss       0x" koff " + %[k], %[x6]")      /* xmm6 = k */ \
                    __ASM_EMIT("haddps      %[x4], %[x4]")                  /* xmm4 = dx1*nx+dy1*ny dz1*nz dx1*nx+dy1*ny dz1*nz */ \
                    __ASM_EMIT("haddps      %[x5], %[x5]")                  /* xmm5 = dx2*nx+dy2*ny dz2*nz dx2*nx+dy2*ny dz2*nz */ \
                    __ASM_EMIT("shufps      $0x00, %[x6], %[x6]")           /* xmm6 = k k k k */ \
                    __ASM_EMIT("haddps      %[x4], %[x4]")                  /* xmm4 = dx1*nx+dy1*ny+dz1*nz = b1 b1 b1 b1 */ \
                    __ASM_EMIT("haddps      %[x5], %[x5]")                  /* xmm5 = dx2*nx+dy2*ny+dz2*nz = b2 b2 b2 b2 */ \
                    __ASM_EMIT("divps       %[x4], %[x2]")                  /* xmm2 = dx1/b1 dy1/b1 dz1/b1 0 */ \
                    __ASM_EMIT("divps       %[x5], %[x3]")                  /* xmm3 = dx2/b2 dy2/b2 dz2/b2 0 */ \
                    __ASM_EMIT("mulps       %[x6], %[x2]")                  /* xmm2 = k*dx1/b1 k*dy1/b1 k*dz1/b1 0 */ \
                    __ASM_EMIT("mulps       %[x6], %[x3]")                  /* xmm3 = k*dx2/b2 k*dy2/b2 k*dz2/b2 0 */ \
                    __ASM_EMIT("subps       %[x2], %[x0]")                  /* xmm0 = lx0-k*dx1/b1 ly0-k*dy1/b1 lz0-k*dz1/b1 1 */ \
                    __ASM_EMIT("subps       %[x3], %[x1]")                  /* xmm1 = lx1-k*dx2/b2 ly1-k*dy2/b2 lz1-k*dz2/b2 1 */ \
                \
                store \
                \
                : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3), \
                  [x4] "=&x" (x4), [x5] "=&x" (x5), [x6] "=&x" (x6), [x7] "=&x" (x7) \
                : [st] "r" (pv), [pl] "r" (pl), [in] "r" (in), \
                  [k] "o" (k) \
                : "cc", "memory" \
            );

        switch (t0)
        {
            // 0 intersections, triangle is above
            case 0x00:  // 0 0 0
            case 0x01:  // 0 0 1
            case 0x04:  // 0 1 0
            case 0x05:  // 0 1 1
            case 0x10:  // 1 0 0
            case 0x11:  // 1 0 1
            case 0x14:  // 1 1 0
            case 0x15:  // 1 1 1
                break;

            // 0 intersections, triangle is below
            case 0x16:  // 1 1 2
            case 0x19:  // 1 2 1
            case 0x1a:  // 1 2 2
            case 0x25:  // 2 1 1
            case 0x26:  // 2 1 2
            case 0x29:  // 2 2 1
            case 0x2a:  // 2 2 2
                STR_COPY_TO(in);
                ++(*n_in);
                break;

            // 1 intersection, 1 triangle above, 1 triangle below, counter-clockwise
            case 0x06:  // 0 1 2
                STR_SPLIT_1P("00", "20", "00",
                    __ASM_EMIT("movups      0x00(%[st]), %[x1]")
                    __ASM_EMIT("movups      0x10(%[st]), %[x2]")
                    __ASM_EMIT("movups      %[x1], 0x00(%[in])")
                    __ASM_EMIT("movups      %[x2], 0x10(%[in])")
                    __ASM_EMIT("movups      %[x0], 0x20(%[in])")
                );
                ++(*n_in);
                break;
            case 0x21:  // 2 0 1
                STR_SPLIT_1P("00", "10", "00",
                    __ASM_EMIT("movups      0x00(%[st]), %[x1]")
                    __ASM_EMIT("movups      0x20(%[st]), %[x3]")
                    __ASM_EMIT("movups      %[x3], 0x00(%[in])")
                    __ASM_EMIT("movups      %[x1], 0x10(%[in])")
                    __ASM_EMIT("movups      %[x0], 0x20(%[in])")
                );
                ++(*n_in);
                break;
            case 0x18:  // 1 2 0
                STR_SPLIT_1P("10", "20", "04",
                    __ASM_EMIT("movups      0x10(%[st]), %[x2]")
                    __ASM_EMIT("movups      0x20(%[st]), %[x3]")
                    __ASM_EMIT("movups      %[x2], 0x00(%[in])")
                    __ASM_EMIT("movups      %[x3], 0x10(%[in])")
                    __ASM_EMIT("movups      %[x0], 0x20(%[in])")
                );
                ++(*n_in);
                break;

            // 1 intersection, 1 triangle above, 1 triangle below, clockwise
            case 0x24:  // 2 1 0
                STR_SPLIT_1P("00", "20", "00",
                    __ASM_EMIT("movups      0x00(%[st]), %[x1]")
                    __ASM_EMIT("movups      0x20(%[st]), %[x3]")
                    __ASM_EMIT("movups      %[x3], 0x00(%[in])")
                    __ASM_EMIT("movups      %[x0], 0x10(%[in])")
                    __ASM_EMIT("movups      %[x1], 0x20(%[in])")
                );
                ++(*n_in);
                break;
            case 0x12:  // 1 0 2
                STR_SPLIT_1P("00", "10", "00",
                    __ASM_EMIT("movups      0x00(%[st]), %[x1]")
                    __ASM_EMIT("movups      0x20(%[st]), %[x3]")
                    __ASM_EMIT("movups      %[x1], 0x00(%[in])")
                    __ASM_EMIT("movups      %[x0], 0x10(%[in])")
                    __ASM_EMIT("movups      %[x3], 0x20(%[in])")
                );
                ++(*n_in);
                break;
            case 0x09:  // 0 2 1
                STR_SPLIT_1P("10", "20", "04",
                    __ASM_EMIT("movups      0x00(%[st]), %[x1]")
                    __ASM_EMIT("movups      0x10(%[st]), %[x2]")
                    __ASM_EMIT("movups      %[x2], 0x00(%[in])")
                    __ASM_EMIT("movups      %[x0], 0x10(%[in])")
                    __ASM_EMIT("movups      %[x1], 0x20(%[in])")
                );
                ++(*n_in);
                break;

            // 2 intersections, 1 triangle below, 2 triangles above
            case 0x02:  // 0 0 2
                STR_SPLIT_2P("00", "10", "20", "00",
                    __ASM_EMIT("movups      0x00(%[st]), %[x2]")
                    __ASM_EMIT("movups      %[x2], 0x00(%[in])")
                    __ASM_EMIT("movups      %[x0], 0x10(%[in])")
                    __ASM_EMIT("movups      %[x1], 0x20(%[in])")
                );
                ++(*n_in);
                break;
            case 0x08:  // 0 2 0
                STR_SPLIT_2P("10", "00", "20", "04",
                    __ASM_EMIT("movups      0x10(%[st]), %[x3]")
                    __ASM_EMIT("movups      %[x3], 0x00(%[in])")
                    __ASM_EMIT("movups      %[x1], 0x10(%[in])")
                    __ASM_EMIT("movups      %[x0], 0x20(%[in])")
                );
                ++(*n_in);
                break;
            case 0x20:  // 2 0 0
                STR_SPLIT_2P("20", "00", "10", "08",
                    __ASM_EMIT("movups      0x20(%[st]), %[x4]")
                    __ASM_EMIT("movups      %[x4], 0x00(%[in])")
                    __ASM_EMIT("movups      %[x0], 0x10(%[in])")
                    __ASM_EMIT("movups      %[x1], 0x20(%[in])")
                );
                ++(*n_in);
                break;

            // 2 intersections, 1 triangle above, 2 triangles below
            case 0x28:  // 2 2 0
                STR_SPLIT_2P("00", "10", "20", "00",
                    __ASM_EMIT("movups      0x10(%[st]), %[x3]")
                    __ASM_EMIT("movups      0x20(%[st]), %[x4]")
                    __ASM_EMIT("movups      %[x3], 0x00(%[in])")
                    __ASM_EMIT("movups      %[x1], 0x10(%[in])")
                    __ASM_EMIT("movups      %[x0], 0x20(%[in])")
                    __ASM_EMIT("movups      %[x4], 0x30(%[in])")
                    __ASM_EMIT("movups      %[x1], 0x40(%[in])")
                    __ASM_EMIT("movups      %[x3], 0x50(%[in])")
                );
                (*n_in) += 2;
                break;

            case 0x22:  // 2 0 2
                STR_SPLIT_2P("10", "00", "20", "04",
                    __ASM_EMIT("movups      0x00(%[st]), %[x2]")
                    __ASM_EMIT("movups      0x20(%[st]), %[x4]")
                    __ASM_EMIT("movups      %[x4], 0x00(%[in])")
                    __ASM_EMIT("movups      %[x0], 0x10(%[in])")
                    __ASM_EMIT("movups      %[x1], 0x20(%[in])")
                    __ASM_EMIT("movups      %[x2], 0x30(%[in])")
                    __ASM_EMIT("movups      %[x0], 0x40(%[in])")
                    __ASM_EMIT("movups      %[x4], 0x50(%[in])")
                );
                (*n_in) += 2;
                break;

            case 0x0a:  // 0 2 2
                STR_SPLIT_2P("20", "00", "10", "08",
                    __ASM_EMIT("movups      0x00(%[st]), %[x2]")
                    __ASM_EMIT("movups      0x10(%[st]), %[x3]")
                    __ASM_EMIT("movups      %[x2], 0x00(%[in])")
                    __ASM_EMIT("movups      %[x1], 0x10(%[in])")
                    __ASM_EMIT("movups      %[x0], 0x20(%[in])")
                    __ASM_EMIT("movups      %[x3], 0x30(%[in])")
                    __ASM_EMIT("movups      %[x1], 0x40(%[in])")
                    __ASM_EMIT("movups      %[x2], 0x50(%[in])")
                );
                (*n_in) += 2;
                break;

            default:
                break;
        }

        #undef STR_COPY_TO
        #undef STR_SPLIT_1P
        #undef STR_SPLIT_2P
    }
}



#endif /* DSP_ARCH_X86_SSE3_3DMATH_H_ */
