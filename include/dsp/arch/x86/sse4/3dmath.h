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
        __ASM_EMIT("jle         1000000f") \
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
        __ASM_EMIT("jle         1000000f") \
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

    size_t colocation_v1p3(const vector3d_t *pl, const point3d_t *p0, const point3d_t *p1, const point3d_t *p2)
    {
        float x0, x1, x2, x3, x4;
        float res[4] __lsp_aligned16;

        ARCH_X86_ASM
        (
            __ASM_EMIT("movups      (%[pl]), %[x3]")        /* xmm3 = pl    */
            __ASM_EMIT("movups      (%[p0]), %[x0]")        /* xmm0 = p0    */
            __ASM_EMIT("movups      (%[p1]), %[x1]")        /* xmm1 = p1    */
            __ASM_EMIT("movups      (%[p2]), %[x2]")        /* xmm2 = p2    */

            __ASM_EMIT("dpps        $0xf1, %[x3], %[x0]")   /* xmm0 = p0 * pl */
            __ASM_EMIT("dpps        $0xf2, %[x3], %[x1]")   /* xmm1 = p1 * pl */
            __ASM_EMIT("dpps        $0xf4, %[x3], %[x2]")   /* xmm2 = p2 * pl */
            __ASM_EMIT("orps        %[x1], %[x0]")          /* xmm0 = k0 k1 0 0 */
            __ASM_EMIT("orps        %[x2], %[x0]")          /* xmm0 = k0 k1 k2 0 */
            __ASM_EMIT("movaps      %[x0], %[x1]")          /* xmm1 = k0 k1 k2 0 */

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

    size_t colocation_v1pv(const vector3d_t *pl, const point3d_t *pv)
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

            __ASM_EMIT("dpps        $0xf1, %[x3], %[x0]")   /* xmm0 = p0 * pl */
            __ASM_EMIT("dpps        $0xf2, %[x3], %[x1]")   /* xmm1 = p1 * pl */
            __ASM_EMIT("dpps        $0xf4, %[x3], %[x2]")   /* xmm2 = p2 * pl */
            __ASM_EMIT("orps        %[x1], %[x0]")          /* xmm0 = k0 k1 0 0 */
            __ASM_EMIT("orps        %[x2], %[x0]")          /* xmm0 = k0 k1 k2 0 */
            __ASM_EMIT("movaps      %[x0], %[x1]")          /* xmm1 = k0 k1 k2 0 */

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
}

#endif /* DSP_ARCH_X86_SSE4_3DMATH_H_ */
