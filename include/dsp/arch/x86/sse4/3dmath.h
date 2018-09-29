/*
 * 3dmath.h
 *
 *  Created on: 27 мая 2017 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_SSE4_3DMATH_H_
#define DSP_ARCH_X86_SSE4_3DMATH_H_

#ifndef DSP_ARCH_X86_SSE4_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_SSE4_IMPL */

namespace sse4
{
    /* Input:
     * x0 = a0 a1 a2 a3
     * x1 = b0 b1 b2 b3
     *
     * Output:
     * x0 = b0 b1 b2 b3
     * x1 = a0 a1 a2 a3
     */
    #define VECTOR_XCHG(x0, x1) \
        __ASM_EMIT("xorps      %" x0 ", %" x1)     /* xmm1 = b ^ a */ \
        __ASM_EMIT("xorps      %" x1 ", %" x0)     /* xmm0 = a ^ ( b ^ a) = b */ \
        __ASM_EMIT("xorps      %" x0 ", %" x1)     /* xmm1 = (b ^ a) ^ b = a    */

    /* 3x vector multiplication
     * Input:
     *   x0 = vector1 [dx dy dz ? ]
     *   x1 = vector2 [dx dy dz ? ]
     *   x2 = vector3 [dx dy dz ? ]
     *
     * Output:
     *   x0 = vector1 * vector2 [ vz vx vy ? ]
     *   x1 = vector2 * vector3 [ vz vx vy ? ]
     *   x2 = vector3 * vector1 [ vz vx vy ? ]
     */
    #define VECTOR_MUL3(x0, x1, x2, x3, x4, x5, x6, x7) \
        __ASM_EMIT("movaps      %" x0 ", %" x3)          /* xmm3 = dx0 dy0 dz0 dw0 */ \
        __ASM_EMIT("movaps      %" x1 ", %" x4)          /* xmm4 = dx1 dy1 dz1 dw1 */ \
        __ASM_EMIT("movaps      %" x2 ", %" x5)          /* xmm5 = dx2 dy2 dz2 dw2 */ \
        __ASM_EMIT("shufps      $0xc9, %" x3 ", %" x3)   /* xmm3 = dy0 dz0 dx0 dw0 */ \
        __ASM_EMIT("shufps      $0xc9, %" x4 ", %" x4)   /* xmm4 = dy1 dz1 dx1 dw1 */ \
        __ASM_EMIT("shufps      $0xc9, %" x5 ", %" x5)   /* xmm5 = dy2 dz2 dx2 dw2 */ \
        __ASM_EMIT("movaps      %" x0 ", %" x6)          /* xmm6 = dx0 dy0 dz0 dw0 */ \
        __ASM_EMIT("movaps      %" x3 ", %" x7)          /* xmm7 = dy0 dz0 dx0 dw0 */ \
        \
        __ASM_EMIT("mulps       %" x4 ", %" x0)          /* xmm0 = dx0*dy1 dy0*dz1 dz0*dx1 dw0*dw1 */ \
        __ASM_EMIT("mulps       %" x1 ", %" x3)          /* xmm3 = dy0*dx1 dz0*dy1 dx0*dz1 dw0*dw1 */ \
        __ASM_EMIT("mulps       %" x5 ", %" x1)          /* xmm1 = dx1*dy2 dy1*dz2 dz1*dx2 dw1*dw2 */ \
        __ASM_EMIT("mulps       %" x2 ", %" x4)          /* xmm4 = dy1*dx2 dz1*dy2 dx1*dz2 dw1*dw2 */ \
        __ASM_EMIT("mulps       %" x7 ", %" x2)          /* xmm2 = dx2*dy0 dy2*dz0 dz2*dx0 dw2*dw0 */ \
        __ASM_EMIT("mulps       %" x6 ", %" x5)          /* xmm5 = dy2*dx0 dz2*dy0 dx2*dz0 dw2*dw0 */ \
        __ASM_EMIT("subps       %" x3 ", %" x0)          /* xmm0 = nz0 nx0 ny0 nw0 */ \
        __ASM_EMIT("subps       %" x4 ", %" x1)          /* xmm1 = nz1 nx1 ny1 nw1 */ \
        __ASM_EMIT("subps       %" x5 ", %" x2)          /* xmm2 = nz2 nx2 ny2 nw2 */

    /* 3x scalar multiplication
     * Input:
     *   x0 = vector1 [dx dy dz ? ]
     *   x1 = vector2 [dx dy dz ? ]
     *   x2 = vector3 [dx dy dz ? ]
     *
     * Output:
     *   x0 = vector1 * vector2 [ S0 ? ? ? ]
     *   x1 = vector2 * vector3 [ S1 ? ? ? ]
     *   x2 = vector3 * vector1 [ S2 ? ? ? ]
     */
    #define SCALAR_MUL3(x0, x1, x2, x3) \
        __ASM_EMIT("movaps      %" x0 ", %" x3) \
        __ASM_EMIT("dpps        $0x71, %" x1 ", %" x0)      /* xmm0 = dx0*dx1+dy0*dy1+dz0*dz1 */ \
        __ASM_EMIT("dpps        $0x71, %" x2 ", %" x1)      /* xmm1 = dx0*dx1+dy0*dy1+dz0*dz1 */ \
        __ASM_EMIT("dpps        $0x71, %" x3 ", %" x2)      /* xmm2 = dx0*dx1+dy0*dy1+dz0*dz1 */

    /*
     * Input:
     *   x0 = vector 1 = dx1 dy1 dz1 dw1
     *   x1 = vector 2 = dx2 dy2 dz2 dw2
     *
     * Output:
     *   x0 = dx1*dx2 + dy1*dy2 + dz1*dz2
     */
    #define SCALAR_MUL(x0, x1, x2) \
        __ASM_EMIT("dpps        $0x71, %" x1 ", %" x0)  /* xmm0 = dx1*dx2 + dy1*dy2 + dz1*dz2 */

    /*
     * Input:
     *   x0 = vector/point to normalize
     * Output:
     *   x0 = normalized vector
     */
    #define NORMALIZE(x0, x1, x2) \
        __ASM_EMIT("movaps      %" x0 ", %" x1)         /* xmm1 = dx dy dz dw */ \
        __ASM_EMIT("xorps       %" x2 ", %" x2)         /* xmm2 = 0 */ \
        __ASM_EMIT("dpps        $0x7f, %" x1 ", %" x1)  /* xmm1 = dx*dx+dy*dy+dz*dz */ \
        __ASM_EMIT("sqrtps      %" x1", %" x1)          /* xmm1 = sqrt(x*x + y*y + z*z) = w */ \
        __ASM_EMIT("ucomiss     %" x2 ", %" x1)         /* xmm2 =?= xmm1 */ \
        __ASM_EMIT("jbe         1000000f") \
        __ASM_EMIT("divps       %" x1 ", %" x0)         /* xmm0 = dx/w dy/w dz/w dw/w */ \
        __ASM_EMIT("1000000:")

    /*
     * Input:
     *   x0 = vector/point to scale
     *   x1 = radius-vector to match length
     *
     * Output:
     *   x0 = scaled vector
    */
    #define SCALE(x0, x1, x2, x3) \
        __ASM_EMIT("movaps      %" x0 ", %" x3)         /* xmm3 = x y z w */ \
        __ASM_EMIT("xorps       %" x2 ", %" x2)         /* xmm2 = 0 */ \
        __ASM_EMIT("dpps        $0x7f, %" x3 ", %" x3)  /* xmm3 = dx*dx+dy*dy+dz*dz */ \
        __ASM_EMIT("shufps      $0x00, %" x1 ", %" x1)  /* xmm1 = r r r r */ \
        __ASM_EMIT("sqrtps      %" x3", %" x3)          /* xmm3 = sqrt(x*x + y*y + z*z) = W W W W */ \
        __ASM_EMIT("ucomiss     %" x2 ", %" x3)         /* xmm3 =?= xmm2 */ \
        __ASM_EMIT("jbe         1000000f") \
        __ASM_EMIT("divps       %" x3 ", %" x0)         /* xmm0 = r/W r/W r/W r/W */ \
        __ASM_EMIT("mulps       %" x1 ", %" x0)         /* xmm0 = x*r/W y*r/W z*r/W w*r/W */ \
        __ASM_EMIT("1000000:")

    void normalize_point(point3d_t *p)
    {
        float x0, x1, x2;

        ARCH_X86_ASM
        (
            __ASM_EMIT("movups      (%[p]), %[x0]")         // xmm0 = x y z w
            NORMALIZE("[x0]", "[x1]", "[x2]")
            __ASM_EMIT("andps       %[zmask], %[x0]")       // xmm0 = x y z 0
            __ASM_EMIT("orps        %[omask], %[x0]")       // xmm0 = x y z 1
            __ASM_EMIT("movups      %[x0], (%[p])")

            : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2)
            : [p] "r" (p),
              [zmask] "m" (X_MASK0111),
              [omask] "m" (X_3DPOINT)
            : "cc", "memory"
        );
    }

    void scale_point1(point3d_t *p, float r)
    {
        float x0, x1, x2;

        ARCH_X86_ASM
        (
            __ASM_EMIT("movups      (%[p]), %[x0]")         // xmm0 = x y z w
            SCALE("[x0]", "[x3]", "[x1]", "[x2]")
            __ASM_EMIT("andps       %[zmask], %[x0]")       // xmm0 = x y z 0
            __ASM_EMIT("orps        %[omask], %[x0]")       // xmm0 = x y z 1
            __ASM_EMIT("movups      %[x0], (%[p])")

            : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "+x" (r)
            : [p] "r" (p),
              [zmask] "m" (X_MASK0111),
              [omask] "m" (X_3DPOINT)
            : "cc", "memory"
        );
    }

    void scale_point2(point3d_t *p, const point3d_t *s, float r)
    {
        float x0, x1, x2;

        ARCH_X86_ASM
        (
            __ASM_EMIT("movups      (%[s]), %[x0]")         // xmm0 = x y z w
            SCALE("[x0]", "[x3]", "[x1]", "[x2]")
            __ASM_EMIT("andps       %[zmask], %[x0]")       // xmm0 = x y z 0
            __ASM_EMIT("orps        %[omask], %[x0]")       // xmm0 = x y z 1
            __ASM_EMIT("movups      %[x0], (%[p])")

            : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "+x" (r)
            : [p] "r" (p), [s] "r" (s),
              [zmask] "m" (X_MASK0111),
              [omask] "m" (X_3DPOINT)
            : "cc", "memory"
        );
    }

    void normalize_vector(vector3d_t *v)
    {
        float x0, x1, x2;

        ARCH_X86_ASM
        (
            __ASM_EMIT("movups      (%[v]), %[x0]")     // xmm0 = dx dy dz dw
            NORMALIZE("[x0]", "[x1]", "[x2]")
            __ASM_EMIT("movups      %[x0], (%[v])")

            : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2)
            : [v] "r" (v)
            : "cc", "memory"
        );
    }

    void scale_vector1(vector3d_t *v, float r)
    {
        float x0, x1, x2;

        ARCH_X86_ASM
        (
            __ASM_EMIT("movups      (%[v]), %[x0]")     // xmm0 = dx dy dz dw
            SCALE("[x0]", "[x3]", "[x1]", "[x2]")
            __ASM_EMIT("andps       %[zmask], %[x0]")
            __ASM_EMIT("movups      %[x0], (%[v])")

            : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "+x" (r)
            : [v] "r" (v),
              [zmask] "m" (X_MASK0111)
            : "cc", "memory"
        );
    }

    void scale_vector2(vector3d_t *v, const vector3d_t *s, float r)
    {
        float x0, x1, x2;

        ARCH_X86_ASM
        (
            __ASM_EMIT("movups      (%[s]), %[x0]")     // xmm0 = dx dy dz dw
            SCALE("[x0]", "[x3]", "[x1]", "[x2]")
            __ASM_EMIT("andps       %[zmask], %[x0]")
            __ASM_EMIT("movups      %[x0], (%[v])")

            : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "+x" (r)
            : [v] "r" (v), [s] "r" (s),
              [zmask] "m" (X_MASK0111)
            : "cc", "memory"
        );
    }

    float check_point3d_on_triangle_tp(const triangle3d_t *t, const point3d_t *p)
    {
        float x0, x1, x2, x3, x4, x5, x6, x7;

        ARCH_X86_ASM
        (
            /* Load vectors */
            __ASM_EMIT("movups      (%[p]), %[x3]")         /* xmm3 = px py pz pw */
            __ASM_EMIT("movups      0x00(%[t]), %[x0]")     /* xmm0 = x0 y0 z0 w0 */
            __ASM_EMIT("movups      0x10(%[t]), %[x1]")     /* xmm1 = x1 y1 z1 w1 */
            __ASM_EMIT("movups      0x20(%[t]), %[x2]")     /* xmm2 = x2 y2 z2 w2 */
            __ASM_EMIT("subps       %[x3], %[x0]")          /* xmm0 = dx0 dy0 dz0 dw0 */
            __ASM_EMIT("subps       %[x3], %[x1]")          /* xmm1 = dx1 dy1 dz1 dw1 */
            __ASM_EMIT("subps       %[x3], %[x2]")          /* xmm2 = dx2 dy2 dz2 dw2 */
            /* 3x vector multiplications */
            VECTOR_MUL3("[x0]", "[x1]", "[x2]", "[x3]", "[x4]", "[x5]", "[x6]", "[x7]")
            /* 3x scalar multiplications */
            SCALAR_MUL3("[x0]", "[x1]", "[x2]", "[x3]")
            /* Compare with zeros */
            __ASM_EMIT("xorps       %[x4], %[x4]")
            __ASM_EMIT("ucomiss     %[x4], %[x0]")
            __ASM_EMIT("jb          110f")
            __ASM_EMIT("ucomiss     %[x4], %[x1]")
            __ASM_EMIT("jb          109f")
            __ASM_EMIT("ucomiss     %[x4], %[x2]")
            __ASM_EMIT("jb          108f")
            __ASM_EMIT("mulss       %[x1], %[x0]")
            __ASM_EMIT("mulss       %[x2], %[x0]")
            __ASM_EMIT("ucomiss     %[x4], %[x0]")
            __ASM_EMIT("jne         110f")
            /* There is somewhere zero, need additional check */
            /* Load vectors */
            __ASM_EMIT("movups      (%[p]), %[x3]")         /* xmm3 = px py pz pw */
            __ASM_EMIT("movups      0x00(%[t]), %[x0]")     /* xmm0 = x0 y0 z0 w0 */
            __ASM_EMIT("movups      0x10(%[t]), %[x1]")     /* xmm1 = x1 y1 z1 w1 */
            __ASM_EMIT("movups      0x20(%[t]), %[x2]")     /* xmm2 = x2 y2 z2 w2 */
            __ASM_EMIT("subps       %[x3], %[x0]")          /* xmm0 = dx0 dy0 dz0 dw0 */
            __ASM_EMIT("subps       %[x3], %[x1]")          /* xmm1 = dx1 dy1 dz1 dw1 */
            __ASM_EMIT("subps       %[x3], %[x2]")          /* xmm2 = dx2 dy2 dz2 dw2 */
            /* Do 3x scalar multiplications */
            SCALAR_MUL3("[x0]", "[x1]", "[x2]", "[x3]")
            __ASM_EMIT("mulss       %[x1], %[x0]")
            __ASM_EMIT("mulss       %[x2], %[x0]")
            __ASM_EMIT("jmp         110f")

            __ASM_EMIT("108:")
            __ASM_EMIT("movss       %[x2], %[x0]")
            __ASM_EMIT("jmp         110f")
            __ASM_EMIT("109:")
            __ASM_EMIT("movss       %[x1], %[x0]")
            __ASM_EMIT("110:")
            : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3),
              [x4] "=&x" (x4), [x5] "=&x" (x5), [x6] "=&x" (x6), [x7] "=&x" (x7)
            : [p] "r" (p), [t] "r" (t)
        );

        return x0;
/*            // Vector parameters
        vector3d_t  v[3];
        v[0].dx             = t->p[0].x - p->x;
        v[0].dy             = t->p[0].y - p->y;
        v[0].dz             = t->p[0].z - p->z;

        v[1].dx             = t->p[1].x - p->x;
        v[1].dy             = t->p[1].y - p->y;
        v[1].dz             = t->p[1].z - p->z;

        v[2].dx             = t->p[2].x - p->x;
        v[2].dy             = t->p[2].y - p->y;
        v[2].dz             = t->p[2].z - p->z;

        // Calculate vector multiplications
        vector3d_t  m[3];
        m[0].dz             = v[0].dx * v[1].dy - v[0].dy * v[1].dx;
        m[0].dx             = v[0].dy * v[1].dz - v[0].dz * v[1].dy;
        m[0].dy             = v[0].dz * v[1].dx - v[0].dx * v[1].dz;

        m[1].dz             = v[1].dx * v[2].dy - v[1].dy * v[2].dx;
        m[1].dx             = v[1].dy * v[2].dz - v[1].dz * v[2].dy;
        m[1].dy             = v[1].dz * v[2].dx - v[1].dx * v[2].dz;

        m[2].dz             = v[2].dx * v[0].dy - v[2].dy * v[0].dx;
        m[2].dx             = v[2].dy * v[0].dz - v[2].dz * v[0].dy;
        m[2].dy             = v[2].dz * v[0].dx - v[2].dx * v[0].dz;

        // Calculate scalar projections
        float r[3];
        r[0]                = m[0].dx * m[1].dx + m[0].dy * m[1].dy + m[0].dz * m[1].dz;
        r[1]                = m[1].dx * m[2].dx + m[1].dy * m[2].dy + m[1].dz * m[2].dz;
        r[2]                = m[2].dx * m[0].dx + m[2].dy * m[0].dy + m[2].dz * m[0].dz;

        if (r[0] < 0.0f)
            return r[0];
        else if (r[1] < 0.0f)
            return r[1];
        else if (r[2] < 0.0f)
            return r[2];
        r[2]                = r[0]*r[1]*r[2];
        if (r[2] != 0.0f)
            return r[2];

        r[0]                = v[0].dx * v[1].dx + v[0].dy * v[1].dy + v[0].dz * v[1].dz;
        r[1]                = v[1].dx * v[2].dx + v[1].dy * v[2].dy + v[1].dz * v[2].dz;
        r[2]                = v[2].dx * v[0].dx + v[2].dy * v[0].dy + v[2].dz * v[0].dz;

        return r[0]*r[1]*r[2];*/
    }

    float check_point3d_on_triangle_pvp(const point3d_t *pv, const point3d_t *p)
    {
        float x0, x1, x2, x3, x4, x5, x6, x7;

        ARCH_X86_ASM
        (
            /* Load vectors */
            __ASM_EMIT("movups      (%[p]), %[x3]")         /* xmm3 = px py pz pw */
            __ASM_EMIT("movups      0x00(%[pv]), %[x0]")    /* xmm0 = x0 y0 z0 w0 */
            __ASM_EMIT("movups      0x10(%[pv]), %[x1]")    /* xmm1 = x1 y1 z1 w1 */
            __ASM_EMIT("movups      0x20(%[pv]), %[x2]")    /* xmm2 = x2 y2 z2 w2 */
            __ASM_EMIT("subps       %[x3], %[x0]")          /* xmm0 = dx0 dy0 dz0 dw0 */
            __ASM_EMIT("subps       %[x3], %[x1]")          /* xmm1 = dx1 dy1 dz1 dw1 */
            __ASM_EMIT("subps       %[x3], %[x2]")          /* xmm2 = dx2 dy2 dz2 dw2 */
            /* 3x vector multiplications */
            VECTOR_MUL3("[x0]", "[x1]", "[x2]", "[x3]", "[x4]", "[x5]", "[x6]", "[x7]")
            /* 3x scalar multiplications */
            SCALAR_MUL3("[x0]", "[x1]", "[x2]", "[x3]")
            /* Compare with zeros */
            __ASM_EMIT("xorps       %[x4], %[x4]")
            __ASM_EMIT("ucomiss     %[x4], %[x0]")
            __ASM_EMIT("jb          110f")
            __ASM_EMIT("ucomiss     %[x4], %[x1]")
            __ASM_EMIT("jb          109f")
            __ASM_EMIT("ucomiss     %[x4], %[x2]")
            __ASM_EMIT("jb          108f")
            __ASM_EMIT("mulss       %[x1], %[x0]")
            __ASM_EMIT("mulss       %[x2], %[x0]")
            __ASM_EMIT("ucomiss     %[x4], %[x0]")
            __ASM_EMIT("jne         110f")
            /* There is somewhere zero, need additional check */
            /* Load vectors */
            __ASM_EMIT("movups      (%[p]), %[x3]")         /* xmm3 = px py pz pw */
            __ASM_EMIT("movups      0x00(%[pv]), %[x0]")    /* xmm0 = x0 y0 z0 w0 */
            __ASM_EMIT("movups      0x10(%[pv]), %[x1]")    /* xmm1 = x1 y1 z1 w1 */
            __ASM_EMIT("movups      0x20(%[pv]), %[x2]")    /* xmm2 = x2 y2 z2 w2 */
            __ASM_EMIT("subps       %[x3], %[x0]")          /* xmm0 = dx0 dy0 dz0 dw0 */
            __ASM_EMIT("subps       %[x3], %[x1]")          /* xmm1 = dx1 dy1 dz1 dw1 */
            __ASM_EMIT("subps       %[x3], %[x2]")          /* xmm2 = dx2 dy2 dz2 dw2 */
            /* Do 3x scalar multiplications */
            SCALAR_MUL3("[x0]", "[x1]", "[x2]", "[x3]")
            __ASM_EMIT("mulss       %[x1], %[x0]")
            __ASM_EMIT("mulss       %[x2], %[x0]")
            __ASM_EMIT("jmp         110f")

            __ASM_EMIT("108:")
            __ASM_EMIT("movss       %[x2], %[x0]")
            __ASM_EMIT("jmp         110f")
            __ASM_EMIT("109:")
            __ASM_EMIT("movss       %[x1], %[x0]")
            __ASM_EMIT("110:")
            : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3),
              [x4] "=&x" (x4), [x5] "=&x" (x5), [x6] "=&x" (x6), [x7] "=&x" (x7)
            : [pv] "r" (pv), [p] "r" (p)
        );

        return x0;
    }

    float check_point3d_on_triangle_p3p(const point3d_t *p1, const point3d_t *p2, const point3d_t *p3, const point3d_t *p)
    {
        float x0, x1, x2, x3, x4, x5, x6, x7;

        ARCH_X86_ASM
        (
            /* Load vectors */
            __ASM_EMIT("movups      (%[p]), %[x3]")         /* xmm3 = px py pz pw */
            __ASM_EMIT("movups      (%[p1]), %[x0]")        /* xmm0 = x0 y0 z0 w0 */
            __ASM_EMIT("movups      (%[p2]), %[x1]")        /* xmm1 = x1 y1 z1 w1 */
            __ASM_EMIT("movups      (%[p3]), %[x2]")        /* xmm2 = x2 y2 z2 w2 */
            __ASM_EMIT("subps       %[x3], %[x0]")          /* xmm0 = dx0 dy0 dz0 dw0 */
            __ASM_EMIT("subps       %[x3], %[x1]")          /* xmm1 = dx1 dy1 dz1 dw1 */
            __ASM_EMIT("subps       %[x3], %[x2]")          /* xmm2 = dx2 dy2 dz2 dw2 */
            /* 3x vector multiplications */
            VECTOR_MUL3("[x0]", "[x1]", "[x2]", "[x3]", "[x4]", "[x5]", "[x6]", "[x7]")
            /* 3x scalar multiplications */
            SCALAR_MUL3("[x0]", "[x1]", "[x2]", "[x3]")
            /* Compare with zeros */
            __ASM_EMIT("xorps       %[x4], %[x4]")
            __ASM_EMIT("ucomiss     %[x4], %[x0]")
            __ASM_EMIT("jb          110f")
            __ASM_EMIT("ucomiss     %[x4], %[x1]")
            __ASM_EMIT("jb          109f")
            __ASM_EMIT("ucomiss     %[x4], %[x2]")
            __ASM_EMIT("jb          108f")
            __ASM_EMIT("mulss       %[x1], %[x0]")
            __ASM_EMIT("mulss       %[x2], %[x0]")
            __ASM_EMIT("ucomiss     %[x4], %[x0]")
            __ASM_EMIT("jne         110f")
            /* There is somewhere zero, need additional check */
            /* Load vectors */
            __ASM_EMIT("movups      (%[p]), %[x3]")         /* xmm3 = px py pz pw */
            __ASM_EMIT("movups      (%[p1]), %[x0]")        /* xmm0 = x0 y0 z0 w0 */
            __ASM_EMIT("movups      (%[p2]), %[x1]")        /* xmm1 = x1 y1 z1 w1 */
            __ASM_EMIT("movups      (%[p3]), %[x2]")        /* xmm2 = x2 y2 z2 w2 */
            __ASM_EMIT("subps       %[x3], %[x0]")          /* xmm0 = dx0 dy0 dz0 dw0 */
            __ASM_EMIT("subps       %[x3], %[x1]")          /* xmm1 = dx1 dy1 dz1 dw1 */
            __ASM_EMIT("subps       %[x3], %[x2]")          /* xmm2 = dx2 dy2 dz2 dw2 */
            /* Do 3x scalar multiplications */
            SCALAR_MUL3("[x0]", "[x1]", "[x2]", "[x3]")
            __ASM_EMIT("mulss       %[x1], %[x0]")
            __ASM_EMIT("mulss       %[x2], %[x0]")
            __ASM_EMIT("jmp         110f")

            __ASM_EMIT("108:")
            __ASM_EMIT("movss       %[x2], %[x0]")
            __ASM_EMIT("jmp         110f")
            __ASM_EMIT("109:")
            __ASM_EMIT("movss       %[x1], %[x0]")
            __ASM_EMIT("110:")
            : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3),
              [x4] "=&x" (x4), [x5] "=&x" (x5), [x6] "=&x" (x6), [x7] "=&x" (x7)
            : [p1] "r" (p1), [p2] "r" (p2), [p3] "r" (p3), [p] "r" (p)
        );

        return x0;
    }

    float find_intersection3d_rt(point3d_t *ip, const ray3d_t *l, const triangle3d_t *t)
    {
        point3d_t ix __lsp_aligned16;
        float proj;
        float x0, x1, x2, x3, x4, x5, x6, x7;
        size_t tmp1;//, tmp2, tmp3;

        ARCH_X86_ASM
        (
            //---------------------------------------------------
            // PART 0: perform simple culling
            __ASM_EMIT("movups      0x00(%[l]), %[x1]")     /* xmm1 = rzx rzy rzz rzw */
            __ASM_EMIT("movups      0x10(%[l]), %[x0]")     /* xmm0 = rdx rdy rdz rdw */
            __ASM_EMIT("movups      0x00(%[t]), %[x2]")     /* xmm2 = x1 y1 z1 w1     */
            __ASM_EMIT("movups      0x10(%[t]), %[x3]")     /* xmm3 = x2 y2 z2 w2     */
            __ASM_EMIT("movups      0x20(%[t]), %[x4]")     /* xmm4 = x3 y3 z3 w3     */

            __ASM_EMIT("cmpltps     %[x1], %[x2]")
            __ASM_EMIT("cmpltps     %[x1], %[x3]")
            __ASM_EMIT("cmpltps     %[x1], %[x4]")
            __ASM_EMIT("xorps       %[x0], %[x2]")
            __ASM_EMIT("xorps       %[x0], %[x3]")
            __ASM_EMIT("xorps       %[x0], %[x4]")
            __ASM_EMIT("andps       %[x3], %[x2]")
            __ASM_EMIT("andps       %[x4], %[x2]")
            __ASM_EMIT("movmskps    %[x2], %[tmp1]")
            __ASM_EMIT32("test      $0x07, %[tmp1]")
            __ASM_EMIT64("test      $0x07, %b[tmp1]")
            __ASM_EMIT("jnz         2000f")                 /* signs differ */

            //---------------------------------------------------
            // PART 1: check intersection with plane
            // Form equations

            // Calculate pre-requisites for equations
            /* xmm1 = rzx rzy rzz rzw */
            /* xmm0 = rdx rdy rdz rdw */
            __ASM_EMIT("0:")
            __ASM_EMIT("movaps      %[x1], %[x2]")          /* xmm2 = rzx rzy rzz rzw */
            __ASM_EMIT("movaps      %[x0], %[x3]")          /* xmm3 = rdx rdy rdz rdw */
            __ASM_EMIT("shufps      $0xc9, %[x2], %[x2]")   /* xmm2 = rzy rzz rzx rzw */
            __ASM_EMIT("movaps      %[x0], %[x4]")          /* xmm4 = rdx rdy rdz rdw */
            __ASM_EMIT("shufps      $0xc9, %[x3], %[x3]")   /* xmm3 = rdy rdz rdx rdw */
            __ASM_EMIT("mulps       %[x2], %[x0]")          /* xmm0 = rdx*rzy rdy*rzz rdz*rzx rdw*rzw */
            __ASM_EMIT("mulps       %[x3], %[x1]")          /* xmm1 = rdy*rzx rdz*rzy rdx*rzz rdw*rzw */
            __ASM_EMIT("subps       %[x1], %[x0]")          /* xmm0 = rdx*rzy-rdy*rzx rdy*rzz-rdz*rzy rdz*rzx-rdx*rzz rdw*rzw-rdw*rzw = m2dw m3dw m1dw 0 */

            // Form equation
            __ASM_EMIT("movaps      %[x4], %[x2]")          /* xmm2 = rdx rdy rdz rdw */
            __ASM_EMIT("movups      0x30(%[t]), %[x1]")     /* xmm1 = nx ny nz nw */
            __ASM_EMIT("movaps      %[x4], %[x3]")          /* xmm3 = rdx rdy rdz rdw */
            __ASM_EMIT("shufps      $0xe2, %[x0], %[x2]")   /* xmm2 = rdz rdx m1dw 0 */
            __ASM_EMIT("shufps      $0x31, %[x0], %[x3]")   /* xmm3 = rdy rdx 0 m2dw */
            __ASM_EMIT("shufps      $0x79, %[x0], %[x4]")   /* xmm4 = rdy rdz 0 m3dw */
            __ASM_EMIT("xorps       %[smask0001], %[x2]")   /* xmm2 = -rdz rdx m1dw 0 */
            __ASM_EMIT("xorps       %[smask0010], %[x3]")   /* xmm3 = rdy -rdx 0 m2dw */
            __ASM_EMIT("xorps       %[smask0001], %[x4]")   /* xmm4 = -rdy rdz 0 m3dw */
            __ASM_EMIT("shufps      $0x9c, %[x2], %[x2]")   /* xmm2 = -rdz 0 rdx m1dw */
            __ASM_EMIT("shufps      $0xc6, %[x4], %[x4]")   /* xmm4 = 0 rdz -rdy m3dw */

            __ASM_EMIT("100:")
            // Current state:
            // xmm0 = 0 0 0 0
            // xmm1 =  nx   ny   nz   nw    = a0 a1 a2 a3
            // xmm2 = -rdz  0    rdx  m1dw  = b0 b1 b2 b3
            // xmm3 =  rdy -rdx  0    m2dw  = c0 c1 c2 c3
            // xmm4 =   0   rdz -rdy  m3dw  = 0  d1 d2 d3
            // Make matrix triangular
            // Step 1
            __ASM_EMIT("movaps      %[x1], %[x5]")          /* xmm5 = a0 a1 a2 a3 */
            __ASM_EMIT("movaps      %[zero], %[x0]")        /* xmm0 = ~0 ~0 ~0 ~0 */
            __ASM_EMIT("andps       %[abs], %[x5]")         /* xmm5 = abs(a0) */
            __ASM_EMIT("ucomiss     %[x0], %[x5]")          /* xmm5 <?> ~0 */
            __ASM_EMIT("jae         200f")                  /* xmm5 >= ~0 */
                // a0 == 0, need to swap
                __ASM_EMIT("movaps      %[x2], %[x5]")          /* xmm5 = b0 b1 b2 b3 */
                __ASM_EMIT("movaps      %[x3], %[x6]")          /* xmm6 = c0 c1 c2 c3 */
                __ASM_EMIT("andps       %[abs], %[x5]")         /* xmm5 = abs(b0) */
                __ASM_EMIT("andps       %[abs], %[x6]")         /* xmm6 = abs(c0) */

                // Try to exchange
                __ASM_EMIT("ucomiss     %[zero], %[x5]")        /* b0 <?> 0 */
                __ASM_EMIT("jb          101f")                  /* b0 < 0 */
                    VECTOR_XCHG("[x1]", "[x2]")                     /* xmm2 <=> xmm1 */
                    __ASM_EMIT("jmp         200f")

                __ASM_EMIT("101:")
                __ASM_EMIT("ucomiss     %[x0], %[x6]")          /* c0 <?> 0 */
                __ASM_EMIT("jb          2000f")                 /* c0 < 0 */
                    VECTOR_XCHG("[x1]", "[x3]")                     /* xmm3 <=> xmm1 */

            __ASM_EMIT("200:")
                __ASM_EMIT("movaps      %[x1], %[x7]")          /* xmm7 = a0 a1 a2 a3 */
                __ASM_EMIT("movaps      %[x2], %[x5]")          /* xmm5 = b0 b1 b2 b3 */
                __ASM_EMIT("movaps      %[x3], %[x6]")          /* xmm6 = c0 c1 c2 c3 */
                __ASM_EMIT("divps       %[x7], %[x5]")          /* xmm5 = b0/a0 */
                __ASM_EMIT("divps       %[x7], %[x6]")          /* xmm6 = c0/a0 */
                __ASM_EMIT("shufps      $0x00, %[x5], %[x5]")   /* xmm5 = b0/a0 b0/a0 b0/a0 b0/a0 */
                __ASM_EMIT("shufps      $0x00, %[x6], %[x6]")   /* xmm6 = c0/a0 c0/a0 c0/a0 c0/a0 */
                __ASM_EMIT("mulps       %[x7], %[x5]")          /* xmm5 = a0*b0/a0 a1*b0/a0 a2*b0/a0 a3*b0/a0 */
                __ASM_EMIT("mulps       %[x7], %[x6]")          /* xmm6 = a0*c0/a0 a1*c0/a0 a2*c0/a0 a3*c0/a0 */
                __ASM_EMIT("subps       %[x5], %[x2]")          /* xmm2 = b0-a0*b0/a0 b1-a1*b0/a0 b2-a2*b0/a0 b3-a3*b0/a0 = 0 b1 b2 b3 */
                __ASM_EMIT("subps       %[x6], %[x3]")          /* xmm3 = c0-a0*c0/a0 c1-a1*c0/a0 c2-a2*c0/a0 c3-a3*c0/a0 = 0 c1 c2 c3 */

            // Step 2
            __ASM_EMIT("movaps      %[x2], %[x5]")          /* xmm5 = 0 b1 b2 b3 */
            __ASM_EMIT("shufps      $0x55, %[x5], %[x5]")   /* xmm5 = b1 b1 b1 b1 */
            __ASM_EMIT("andps       %[abs], %[x5]")         /* xmm5 = abs(b1) */
            __ASM_EMIT("ucomiss     %[x0], %[x5]")          /* abs(b1) <?> ~0 */
            __ASM_EMIT("jae         300f")                  /* abs(b1) >= ~0 */
                // b1 == 0, need to swap
                __ASM_EMIT("movaps      %[x3], %[x5]")          /* xmm5 = 0 c1 c2 c3 */
                __ASM_EMIT("movaps      %[x4], %[x6]")          /* xmm6 = 0 d1 d2 d3 */
                __ASM_EMIT("andps       %[abs], %[x5]")         /* xmm5 = abs(c1) */
                __ASM_EMIT("andps       %[abs], %[x6]")         /* xmm6 = abs(d1) */
                __ASM_EMIT("shufps      $0x55, %[x5], %[x5]")       /* xmm5 = c1 c1 c1 c1 */
                __ASM_EMIT("shufps      $0x55, %[x6], %[x6]")       /* xmm6 = d1 d1 d1 d1 */

                // Try to exchange
                __ASM_EMIT("ucomiss     %[x0], %[x5]")          /* c1 <?> 0 */
                __ASM_EMIT("jb          201f")                  /* c1 < 0 */
                    VECTOR_XCHG("[x2]", "[x3]")                     /* xmm3 <=> xmm2 */
                    __ASM_EMIT("jmp         300f")

                __ASM_EMIT("201:")
                __ASM_EMIT("ucomiss     %[x0], %[x6]")          /* d1 <?> 0 */
                __ASM_EMIT("jb          2000f")                 /* d1 < 0 */
                    VECTOR_XCHG("[x2]", "[x3]")                     /* xmm3 <=> xmm1 */

            __ASM_EMIT("300:")
                __ASM_EMIT("movaps      %[x2], %[x7]")          /* xmm7 = 0 b1 b2 b3 */
                __ASM_EMIT("movaps      %[x3], %[x5]")          /* xmm5 = 0 c1 c2 c3 */
                __ASM_EMIT("movaps      %[x4], %[x6]")          /* xmm6 = 0 d1 d2 d3 */
                __ASM_EMIT("divps       %[x7], %[x5]")          /* xmm5 = c1/b1 */
                __ASM_EMIT("divps       %[x7], %[x6]")          /* xmm6 = d1/b1 */
                __ASM_EMIT("shufps      $0x55, %[x5], %[x5]")   /* xmm5 = c1/b1 c1/b1 c1/b1 c1/b1 */
                __ASM_EMIT("shufps      $0x55, %[x6], %[x6]")   /* xmm6 = d1/b1 d1/b1 d1/b1 d1/b1 */
                __ASM_EMIT("mulps       %[x7], %[x5]")          /* xmm5 = 0 b1*c1/b1 b2*c1/b1 b3*c1/b1 */
                __ASM_EMIT("mulps       %[x7], %[x6]")          /* xmm6 = 0 b1*d1/b1 b2*d1/b1 b3*d1/b1 */
                __ASM_EMIT("subps       %[x5], %[x3]")          /* xmm3 = 0-0 c1-b1*c1/b1 c2-b2*c1/b1 c3-b3*c1/b1 = 0 0 c2 c3 */
                __ASM_EMIT("subps       %[x6], %[x4]")          /* xmm4 = 0-0 d1-b1*d1/b1 d2-b2*d1/b1 d3-b3*d1/b1 = 0 0 d2 d3 */

            // Step 3
            __ASM_EMIT("movhlps     %[x3], %[x5]")          /* xmm5 = c2 c3 ? ? */
            __ASM_EMIT("andps       %[abs], %[x5]")         /* xmm5 = abs(c2) */
            __ASM_EMIT("ucomiss     %[x0], %[x5]")          /* abs(c2) <?> ~0 */
            __ASM_EMIT("jae         400f")                  /* abs(c2) >= ~0 */
                // c2 == 0, need to swap
                __ASM_EMIT("movhlps     %[x4], %[x5]")          /* xmm5 = d2 d3 */
                __ASM_EMIT("andps       %[abs], %[x5]")         /* xmm5 = abs(d2) */
                __ASM_EMIT("ucomiss     %[x0], %[x5]")          /* abs(d2) <?> ~0 */
                __ASM_EMIT("jb          2000f")                 /* d2 < 0 */
                    VECTOR_XCHG("[x3]", "[x4]")                     /* xmm3 <=> xmm1 */

            __ASM_EMIT("400:")
            // Solve matrix:
            // xmm1 = a0 a1 a2 a3
            // xmm2 =  0 b1 b2 b3
            // xmm3 =  0  0 c2 c3
            __ASM_EMIT("unpckhps        %[x3], %[x3]")          /* xmm3 = c2 c2 c3 c3 */
            __ASM_EMIT("xorps           %[x0], %[x0]")          /* xmm0 = 0 0 0 0 */
            __ASM_EMIT("movhlps         %[x3], %[x4]")          /* xmm4 = c3 c3 ? ? */
            __ASM_EMIT("movhlps         %[x2], %[x5]")          /* xmm5 = b2 b3 ? ? */
            __ASM_EMIT("divss           %[x3], %[x4]")          /* xmm4 = c3/c2 */
            __ASM_EMIT("subss           %[x4], %[x0]")          /* xmm0 = -c3/c2 0 0 0 = iz 0 0 0 */

            __ASM_EMIT("mulss           %[x0], %[x5]")          /* xmm5 = b2*iz b3 ? ? */
            __ASM_EMIT("shufps          $0x93, %[x0], %[x0]")   /* xmm0 = 0 iz 0 0 */
            __ASM_EMIT("shufps          $0x55, %[x2], %[x2]")   /* xmm2 = b1 b1 b1 b1 */
            __ASM_EMIT("unpcklps        %[x5], %[x5]")          /* xmm5 = b2*iz b2*iz b3 b3 */
            __ASM_EMIT("movhlps         %[x5], %[x4]")          /* xmm4 = b3 b3 */
            __ASM_EMIT("addss           %[x5], %[x4]")          /* xmm4 = b3 + b2*iz */
            __ASM_EMIT("divss           %[x2], %[x4]")          /* xmm4 = (b3 + b2*iz)/b1 */
            __ASM_EMIT("movaps          %[x1], %[x6]")          /* xmm6 = a0 a1 a2 a3 */
            __ASM_EMIT("subss           %[x4], %[x0]")          /* xmm0 = -(b3 + b2*iz)/b1 iz 0 0 = iy iz 0 0 */

            __ASM_EMIT("shufps          $0x93, %[x0], %[x0]")   /* xmm0 = 0 iy iz 0 */
            __ASM_EMIT("mulps           %[x0], %[x1]")          /* xmm1 = 0 iy*a1 iz*a2 0 */
            __ASM_EMIT("movhlps         %[x6], %[x3]")          /* xmm3 = a2 a3 ? ? */
            __ASM_EMIT("movhlps         %[x1], %[x4]")          /* xmm4 =  0 ? ? */
            __ASM_EMIT("addps           %[x3], %[x1]")          /* xmm1 = a2 a3+iy*a1 ? ? */
            __ASM_EMIT("shufps          $0x55, %[x1], %[x1]")   /* xmm1 = a3+iy*a1 a3+iy*a1 a3+iy*a1 a3+iy*a1 */
            __ASM_EMIT("addss           %[x1], %[x4]")          /* xmm4 = a3+iy*a1+iz*a2 */
            __ASM_EMIT("divss           %[x6], %[x4]")          /* xmm4 = (a3 + iy*a1 + iz*a2)/a0 */
            __ASM_EMIT("subss           %[x4], %[x0]")          /* xmm0 = -((a3 + iy*a1 + iz*a2) iy iz 0 = ix iy iz 0 */

            //---------------------------------------------------
            // PART 2: check that point lies on the ray
            __ASM_EMIT("movups          0x00(%[l]), %[x2]")     /* xmm2 = rzx rzy rzz rzw */
            __ASM_EMIT("movaps          %[x0], %[x1]")          /* xmm1 = ix iy iz 0 */
            __ASM_EMIT("movups          0x10(%[l]), %[x3]")     /* xmm3 = rdx rdy rdz rdw */
            __ASM_EMIT("subps           %[x2], %[x1]")          /* xmm1 = ix-rzx iy-rzy iz-rzz -rzw = pdx pdy pdz pdw */
            __ASM_EMIT("xorps           %[x4], %[x4]")          /* xmm4 = 0 */
            __ASM_EMIT("dpps            $0x71, %[x3], %[x1]")   /* xmm1 = rdx*pdx + rdy*pdy + rdz*pdz = proj */

            __ASM_EMIT("ucomiss         %[x4], %[x1]")          /* xmm1 <=> 0 */
            __ASM_EMIT("jb              2000f")
            __ASM_EMIT("movaps          %[x0], %[ix]")          /* store point */
            __ASM_EMIT("movss           %[x1], %[proj]")        /* store projection */

            //---------------------------------------------------
            // PART 3: check that point lies within a triangle
            /* Load vectors */
            /* xmm0 = ix iy iz 0 */
            __ASM_EMIT("movups      0x00(%[t]), %[x1]")     /* xmm1 = x0 y0 z0 w0 */
            __ASM_EMIT("movups      0x10(%[t]), %[x2]")     /* xmm2 = x1 y1 z1 w1 */
            __ASM_EMIT("movups      0x20(%[t]), %[x3]")     /* xmm3 = x2 y2 z2 w2 */
            __ASM_EMIT("subps       %[x0], %[x1]")          /* xmm1 = dx0 dy0 dz0 dw0 */
            __ASM_EMIT("subps       %[x0], %[x2]")          /* xmm2 = dx1 dy1 dz1 dw1 */
            __ASM_EMIT("subps       %[x0], %[x3]")          /* xmm3 = dx2 dy2 dz2 dw2 */
            /* 3x vector multiplications */
            VECTOR_MUL3("[x1]", "[x2]", "[x3]", "[x4]", "[x5]", "[x6]", "[x7]", "[x0]")
            /* 3x scalar multiplications */
            SCALAR_MUL3("[x1]", "[x2]", "[x3]", "[x0]")
            /* Compare with zeros */
            __ASM_EMIT("xorps       %[x4], %[x4]")
            __ASM_EMIT("ucomiss     %[x4], %[x1]")
            __ASM_EMIT("jb          2000f")
            __ASM_EMIT("ucomiss     %[x4], %[x2]")
            __ASM_EMIT("jb          2000f")
            __ASM_EMIT("ucomiss     %[x4], %[x3]")
            __ASM_EMIT("jb          2000f")
            __ASM_EMIT("mulss       %[x2], %[x1]")
            __ASM_EMIT("mulss       %[x3], %[x1]")
            __ASM_EMIT("ucomiss     %[x4], %[x1]")
            __ASM_EMIT("jne         3000f")
            /* There is somewhere zero, need additional check */
            /* Load vectors */
            __ASM_EMIT("movaps      %[ix], %[x3]")          /* xmm3 = ix iy iz 0 */
            __ASM_EMIT("movups      0x00(%[t]), %[x0]")     /* xmm0 = x0 y0 z0 w0 */
            __ASM_EMIT("movups      0x10(%[t]), %[x1]")     /* xmm1 = x1 y1 z1 w1 */
            __ASM_EMIT("movups      0x20(%[t]), %[x2]")     /* xmm2 = x2 y2 z2 w2 */
            __ASM_EMIT("subps       %[x3], %[x0]")          /* xmm0 = dx0 dy0 dz0 dw0 */
            __ASM_EMIT("subps       %[x3], %[x1]")          /* xmm1 = dx1 dy1 dz1 dw1 */
            __ASM_EMIT("subps       %[x3], %[x2]")          /* xmm2 = dx2 dy2 dz2 dw2 */
            /* Do 3x scalar multiplications */
            SCALAR_MUL3("[x0]", "[x1]", "[x2]", "[x3]")
            __ASM_EMIT("mulss       %[x1], %[x0]")
            __ASM_EMIT("mulss       %[x2], %[x0]")
            __ASM_EMIT("ucomiss     %[x4], %[x0]")          /* xmm0 <?> 1 */
            __ASM_EMIT("jae         3000f")

            __ASM_EMIT("2000:")
            __ASM_EMIT("movaps      %[negative], %[x0]")    /* xmm0 = -1 */
            __ASM_EMIT("jmp         1000f")

            /* Copy point and return good result */
            __ASM_EMIT("3000:")
            __ASM_EMIT("movaps      %[ix], %[x1]")          /* xmm1 = ix iy iz 0 */
            __ASM_EMIT("movss       %[proj], %[x0]")        /* xmm0 = proj */
            __ASM_EMIT("movups      %[x1], (%[ip])")

            __ASM_EMIT("1000:")

            : [tmp1] "=&r" (tmp1),
              [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3),
              [x4] "=&x" (x4), [x5] "=&x" (x5), [x6] "=&x" (x6), [x7] "=&x" (x7)
            : [l] "r" (l), [t] "r" (t), [ip] "r" (ip),
              [smask0001] "m" (X_SMASK0001),
              [smask0010] "m" (X_SMASK0010),
              [abs] "m" (X_SIGN),
              [zero] "m" (X_3D_TOLERANCE),
              [negative] "m" (X_MINUS_ONE),
              [ix] "m" (ix),
              [proj] "m" (proj)
          : "cc", "memory"
        );

        return x0;
    }
}

#endif /* DSP_ARCH_X86_SSE4_3DMATH_H_ */
