/*
 * 3dmath.h
 *
 *  Created on: 19 апр. 2017 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_SSE_3DMATH_H_
#define DSP_ARCH_X86_SSE_3DMATH_H_

#ifndef DSP_ARCH_X86_SSE_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_SSE_IMPL */

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

/* Input:
 * x0 = a0 a1 a2 ?
 * x1 = b0 b1 b2 ?
 * x2 = c0 c1 c2 ?
 *
 * Output:
 * x0 = a0 a1 a2 ?
 * x1 = b0 b1 b2 ?
 * x3 = c0 c1 c2 ?
 */
#define MAT3_TRANSPOSE(x0, x1, x2, x3)    \
    __ASM_EMIT("movaps      %" x0 ", %" x3)      /* xmm3 = a0 a1 a2 ? */   \
    __ASM_EMIT("unpcklps    %" x1 ", %" x0)      /* xmm0 = a0 b0 a1 b1 */  \
    __ASM_EMIT("unpckhps    %" x1 ", %" x3)      /* xmm3 = a2 b2 ? ? */  \
    __ASM_EMIT("movhlps     %" x0 ", %" x1)      /* xmm1 = a1 b1 b2 ? */ \
    __ASM_EMIT("movlhps     %" x2 ", %" x0)      /* xmm0 = a0 b0 c0 c1 ? */ \
    __ASM_EMIT("shufps      $0x54, %" x2 ", %" x1) /* xmm1 = a1 b1 c1 c1 */ \
    __ASM_EMIT("shufps      $0xa4, %" x2 ", %" x3) /* xmm3 = a2 b2 c2 c2 */

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
    __ASM_EMIT("mulps       %" x1 ", %" x0)      /* xmm0 = dx0*dx1 dy0*dy1 dz0*dz1 dw0*dw1 */ \
    __ASM_EMIT("mulps       %" x2 ", %" x1)      /* xmm1 = dx1*dx2 dy1*dy2 dz1*dz2 dw1*dw2 */ \
    __ASM_EMIT("mulps       %" x3 ", %" x2)      /* xmm2 = dx2*dx0 dy2*dy0 dz2*dz0 dw2*dw0 */ \
    MAT3_TRANSPOSE(x0, x1, x2, x3) \
    __ASM_EMIT("addps       %" x1 ", %" x0) \
    __ASM_EMIT("addps       %" x3 ", %" x0)      /* xmm0 = S0 S1 S2 ? */ \
    __ASM_EMIT("movhlps     %" x0 ", %" x2)      /* xmm2 = r2 ? ? ? */ \
    __ASM_EMIT("unpcklps    %" x0 ", %" x0)      /* xmm0 = r0 r0 r1 r1 */ \
    __ASM_EMIT("movhlps     %" x0 ", %" x1)      /* xmm1 = r1 r1 ? ? */

/* Get cosine of angle between two vectors
 * Input:
 *   x0 = vector1 [dx dy dz ? ]
 *   x1 = vector2 [dx dy dz ? ]
 *   m0 = -1
 *   m1 = +1
 *
 * Output:
 *   x0 = vector1 * vector2 [ S0 ? ? ? ]
 */
#define CALC_COSINE2V(x0, x1, x2, x3, x4, m0, m1) \
    __ASM_EMIT("movaps      %" x0 ", %" x2)      /* xmm2 = dx0 dy0 dz0 ? */ \
    __ASM_EMIT("mulps       %" x1 ", %" x0)      /* xmm0 = dx0*dx1 dy0*dy1 dz0*dz1 ? */ \
    __ASM_EMIT("mulps       %" x2 ", %" x2)      /* xmm2 = dx0*dx0 dy0*dy0 dz0*dz0 ? */ \
    __ASM_EMIT("mulps       %" x1 ", %" x1)      /* xmm1 = dx1*dx1 dy1*dy1 dz1*dz1 ? */ \
    __ASM_EMIT("movhlps     %" x2 ", %" x4)      /* xmm4 = dz0*dz0 */ \
    __ASM_EMIT("movhlps     %" x1 ", %" x3)      /* xmm3 = dz1*dz1 */ \
    __ASM_EMIT("addss       %" x4 ", %" x2)      /* xmm2 = dx0*dx0+dz0*dz0 dy0*dy0 dz0*dz0 ? */ \
    __ASM_EMIT("addss       %" x3 ", %" x1)      /* xmm1 = dx1*dx1+dz1*dz1 dy1*dy1 dz1*dz1 ? */ \
    __ASM_EMIT("unpcklps    %" x2 ", %" x2)      /* xmm2 = dx0*dx0+dz0*dz0 dx0*dx0+dz0*dz0 dy0*dy0 dy0*dy0 */ \
    __ASM_EMIT("unpcklps    %" x1 ", %" x1)      /* xmm1 = dx1*dx1+dz1*dz1 dx1*dx1+dz1*dz1 dy1*dy1 dy1*dy1 */ \
    __ASM_EMIT("movhlps     %" x2 ", %" x4)      /* xmm4 = dy0*dy0 */ \
    __ASM_EMIT("movhlps     %" x1 ", %" x3)      /* xmm3 = dy1*dy1 */ \
    __ASM_EMIT("addss       %" x4 ", %" x2)      /* xmm2 = dx0*dx0+dz0*dz0+dy0*dy0 */ \
    __ASM_EMIT("addss       %" x3 ", %" x1)      /* xmm1 = dx1*dx1+dz1*dz1+dy1*dy1 */ \
    __ASM_EMIT("movhlps     %" x0 ", %" x4)      /* xmm4 = dz0*dz1 */ \
    __ASM_EMIT("sqrtss      %" x2 ", %" x2)      /* xmm2 = sqrtf(dx0*dx0+dz0*dz0+dy0*dy0) */ \
    __ASM_EMIT("addss       %" x4 ", %" x0)      /* xmm0 = dz1*dz1+dx0*dx1 dy0*dy1 dz0*dz1 ? */ \
    __ASM_EMIT("sqrtss      %" x1 ", %" x1)      /* xmm1 = sqrtf(dx1*dx1+dz1*dz1+dy1*dy1) */ \
    __ASM_EMIT("unpcklps    %" x0 ", %" x0)      /* xmm0 = dz1*dz1+dx0*dx1 dz1*dz1+dx0*dx1 dy0*dy1 dy0*dy1 */ \
    __ASM_EMIT("mulss       %" x1 ", %" x2)      /* xmm2 = w */ \
    __ASM_EMIT("movhlps     %" x0 ", %" x4)      /* xmm4 = dy0*dy1 */ \
    __ASM_EMIT("xorps       %" x1 ", %" x1)      /* xmm1 = 0 */ \
    __ASM_EMIT("addss       %" x4 ", %" x0)      /* xmm0 = dz1*dz1+dx0*dx1+dy0*dy1 */ \
    __ASM_EMIT("ucomiss     %" x1 ", %" x2)      /* xmm2 <?> 0 */ \
    __ASM_EMIT("jbe         1000000f")           /* xmm2 <= 0 */ \
    __ASM_EMIT("divss       %" x2 ", %" x0)      /* xmm0 = (dz1*dz1+dx0*dx1+dy0*dy1)/w */ \
    __ASM_EMIT("1000000:") \
    __ASM_EMIT("ucomiss     %" m0 ", %" x0) \
    __ASM_EMIT("jae 1000001f") \
    __ASM_EMIT("movss       %" m0 ", %" x0) \
    __ASM_EMIT("jmp         1000002f") \
    __ASM_EMIT("1000001:") \
    __ASM_EMIT("ucomiss     %" m1 ", %" x0) \
    __ASM_EMIT("jbe 1000002f") \
    __ASM_EMIT("movss       %" m1 ", %" x0) \
    __ASM_EMIT("jmp         1000002f") \
    __ASM_EMIT("1000002:") \


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

/* 3x vector multiplication
 * Input:
 *   x0 = vector1 [dx dy dz ? ]
 *   x1 = vector2 [dx dy dz ? ]
 *   x2 = vector3 [dx dy dz ? ]
 *
 * Output:
 *   x0 = vector1 * vector2 [ vz vx vy ? ]
 */
#define VECTOR_MUL(x0, x1, x2, x3) \
    __ASM_EMIT("movaps      %" x0 ", %" x2)          /* xmm2 = dx1 dy1 dz1 dw1 */ \
    __ASM_EMIT("movaps      %" x1 ", %" x3)          /* xmm3 = dx2 dy2 dz2 dw2 */ \
    __ASM_EMIT("shufps      $0xc9, %" x2 ", %" x2)   /* xmm2 = dy1 dz1 dx1 dw1 */ \
    __ASM_EMIT("shufps      $0xc9, %" x3 ", %" x3)   /* xmm3 = dy2 dz2 dx2 dw2 */ \
    __ASM_EMIT("mulps       %" x2 ", %" x1)          /* xmm1 = dx2*dy1 dy2*dz1 dz2*dx1 dw2*dw2 */ \
    __ASM_EMIT("mulps       %" x3 ", %" x0)          /* xmm0 = dx1*dy2 dy1*dz2 dz1*dx2 dw1*dw1 */ \
    __ASM_EMIT("subps       %" x1 ", %" x0)          /* xmm0 = dx1*dy2-dx2*dy1 dy1*dz2-dy2*dz1 dz1*dx2-dz2*dx1 dw1*dw1-dw2*dw2 = NY NZ NX NW */

#define MAT4_TRANSPOSE(x0, x1, x2, x3, x4)    \
    __ASM_EMIT("movaps      %" x2 ", %" x4)      /* xmm4 = c1 c2 c3 c4 */   \
    __ASM_EMIT("punpckldq   %" x3 ", %" x2)      /* xmm2 = c1 d1 c2 d2 */   \
    __ASM_EMIT("punpckhdq   %" x3 ", %" x4)      /* xmm4 = c3 d3 c4 d4 */   \
    __ASM_EMIT("movaps      %" x0 ", %" x3)      /* xmm3 = a1 a2 a3 a4 */   \
    __ASM_EMIT("punpckldq   %" x1 ", %" x0)      /* xmm0 = a1 b1 a2 b2 */   \
    __ASM_EMIT("punpckhdq   %" x1 ", %" x3)      /* xmm3 = a3 b3 a4 b4 */   \
    __ASM_EMIT("movaps      %" x0 ", %" x1)      /* xmm1 = a1 b2 a2 b2 */   \
    __ASM_EMIT("punpcklqdq  %" x2 ", %" x0)      /* xmm0 = a1 b1 c1 d1 */   \
    __ASM_EMIT("punpckhqdq  %" x2 ", %" x1)      /* xmm1 = a2 b2 c2 d2 */   \
    __ASM_EMIT("movaps      %" x3 ", %" x2)      /* xmm2 = a3 b3 a4 b4 */   \
    __ASM_EMIT("punpcklqdq  %" x4 ", %" x2)      /* xmm2 = a3 b3 c3 d3 */   \
    __ASM_EMIT("punpckhqdq  %" x4 ", %" x3)      /* xmm3 = a4 b4 c4 d4 */

#define MATRIX_LOAD(ptr, x0, x1, x2, x3) \
    __ASM_EMIT("movups      0x00(%[" ptr "]), %" x0 ) \
    __ASM_EMIT("movups      0x10(%[" ptr "]), %" x1 ) \
    __ASM_EMIT("movups      0x20(%[" ptr "]), %" x2 ) \
    __ASM_EMIT("movups      0x30(%[" ptr "]), %" x3 )

#define MATRIX_STORE(ptr, x0, x1, x2, x3) \
    __ASM_EMIT("movups      %" x0 ", 0x00(%[" ptr "])") \
    __ASM_EMIT("movups      %" x1 ", 0x10(%[" ptr "])") \
    __ASM_EMIT("movups      %" x2 ", 0x20(%[" ptr "])") \
    __ASM_EMIT("movups      %" x3 ", 0x30(%[" ptr "])")

#define NORMALIZE(x0, x1, x2) /* x0 = vector/point to normalize */ \
    __ASM_EMIT("movaps      %" x0 ", %" x2)         /* xmm2 = x y z w */ \
    __ASM_EMIT("mulps       %" x2 ", %" x2)         /* xmm2 = x*x y*y z*z w*w */ \
    __ASM_EMIT("movhlps     %" x2 ", %" x1)         /* xmm1 = z*z w*w ? ? */ \
    __ASM_EMIT("addss       %" x1 ", %" x2)         /* xmm2 = x*x+z*z y*y z*z w*w */ \
    __ASM_EMIT("unpcklps    %" x2 ", %" x2)         /* xmm2 = x*x+z*z x*x+z*z y*y y*y */ \
    __ASM_EMIT("movhlps     %" x2 ", %" x1)         /* xmm1 = y*y y*y ? ? */ \
    __ASM_EMIT("addss       %" x1 ", %" x2)         /* xmm2 = x*x+y*y+z*z */ \
    __ASM_EMIT("xorps       %" x1 ", %" x1)         /* xmm1 = 0 */ \
    __ASM_EMIT("sqrtss      %" x2", %" x2)          /* xmm2 = sqrt(x*x + y*y + z*z) */ \
    __ASM_EMIT("ucomiss     %" x1 ", %" x2)         /* xmm2 =?= xmm1 */ \
    __ASM_EMIT("jbe         1000000f") \
    __ASM_EMIT("shufps      $0x00, %" x2 ", %" x2)  /* xmm2 = w w w w */ \
    __ASM_EMIT("divps       %" x2 ", %" x0)         /* xmm0 = x/w y/w z/w w/w */ \
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
    __ASM_EMIT("mulps       %" x3 ", %" x3)         /* xmm3 = x*x y*y z*z w*w */ \
    __ASM_EMIT("movhlps     %" x3 ", %" x2)         /* xmm2 = z*z w*w ? ? */ \
    __ASM_EMIT("addss       %" x2 ", %" x3)         /* xmm3 = x*x+z*z y*y z*z w*w */ \
    __ASM_EMIT("unpcklps    %" x3 ", %" x3)         /* xmm3 = x*x+z*z x*x+z*z y*y y*y */ \
    __ASM_EMIT("movhlps     %" x3 ", %" x2)         /* xmm2 = y*y y*y ? ? */ \
    __ASM_EMIT("addss       %" x2 ", %" x3)         /* xmm3 = x*x+y*y+z*z */ \
    __ASM_EMIT("xorps       %" x2 ", %" x2)         /* xmm2 = 0 */ \
    __ASM_EMIT("sqrtss      %" x3", %" x3)          /* xmm3 = sqrt(x*x + y*y + z*z) */ \
    __ASM_EMIT("ucomiss     %" x2 ", %" x3)         /* xmm3 =?= xmm2 */ \
    __ASM_EMIT("jbe         1000000f") \
    __ASM_EMIT("divss       %" x3 ", %" x1)         /* xmm1 = r/w */ \
    __ASM_EMIT("shufps      $0x00, %" x1 ", %" x1)  /* xmm1 = r/w r/w r/w r/w */ \
    __ASM_EMIT("mulps       %" x1 ", %" x0)         /* xmm0 = x*r/w y*r/w z*r/w w*r/w */ \
    __ASM_EMIT("1000000:")

/*
 * Input:
 *   x0 = vector 1 = dx1 dy1 dz1 dw1
 *   x1 = vector 2 = dx2 dy2 dz2 dw2
 *
 *  Output:
 *   x0 = dx1*dx2 + dy1*dy2 + dz1*dz2
 */
#define SCALAR_MUL(x0, x1, x2) \
    __ASM_EMIT("mulps       %" x1 ", %" x0)         /* xmm0 = x*x y*y z*z w*w */ \
    __ASM_EMIT("movhlps     %" x0 ", %" x2)         /* xmm2 = z*z w*w ? ? */ \
    __ASM_EMIT("addss       %" x2 ", %" x0)         /* xmm0 = x*x+z*z y*y z*z w*w */ \
    __ASM_EMIT("unpcklps    %" x0 ", %" x0)         /* xmm0 = x*x+z*z x*x+z*z y*y y*y */ \
    __ASM_EMIT("movhlps     %" x0 ", %" x2)         /* xmm2 = y*y y*y ? ? */ \
    __ASM_EMIT("addss       %" x2 ", %" x0)         /* xmm0 = x*x+y*y+z*z */

#define TRIPLET_CHECK(x0, x1, x2, x3, x4)           /* x0 = normal; x1, x2 = vectors */ \
    __ASM_EMIT("movaps      %" x1 ", %" x3)         /* xmm3 = x1 y1 z1 w1 */ \
    __ASM_EMIT("movaps      %" x2 ", %" x4)         /* xmm4 = x2 y2 z2 w2 */ \
    __ASM_EMIT("shufps      $0xc9, %" x3 ", %" x3)  /* xmm3 = y1 z1 x1 w1 */ \
    __ASM_EMIT("shufps      $0xc9, %" x4 ", %" x4)  /* xmm4 = y2 z2 x2 w2 */ \
    __ASM_EMIT("shufps      $0xd2, %" x0 ", %" x0)  /* xmm0 = nz nx ny nw */ \
    __ASM_EMIT("mulps       %" x4 ", %" x1)         /* xmm1 = x1*y2 y1*z2 z1*x2 w1*w2 */ \
    __ASM_EMIT("mulps       %" x3 ", %" x2)         /* xmm2 = y1*x2 z1*y2 x1*z2 w1*w2 */ \
    __ASM_EMIT("subps       %" x2 ", %" x1)         /* xmm1 = x1*y2-y1*x2 y1*z2-z1*y2 z1*x2-x1*z2 w1*w2-w1*w2 = vz vx vy vw */ \
    __ASM_EMIT("mulps       %" x1 ", %" x0)         /* xmm0 = nz*vz nx*vx ny*vy nw*vw */ \
    __ASM_EMIT("movhlps     %" x0 ", %" x2)         /* xmm2 = ny*vy nw*vw ? ? */ \
    __ASM_EMIT("unpcklps    %" x0 ", %" x0)         /* xmm0 = nz*vz nz*vz nx*vx nx*vx */ \
    __ASM_EMIT("movhlps     %" x0 ", %" x1)         /* xmm1 = nx*vx nx*vx ? ? */ \
    __ASM_EMIT("addss       %" x2 ", %" x0)         /* xmm0 = nz*vz + ny*vy */ \
    __ASM_EMIT("addss       %" x1 ", %" x0)         /* xmm0 = nz*vz + ny*vy + nx*vx */

namespace sse
{
    static const float IDENTITY0[4] __lsp_aligned16 = { 1.0f, 0.0f, 0.0f, 0.0f };
    static const float IDENTITY1[4] __lsp_aligned16 = { 0.0f, 1.0f, 0.0f, 0.0f };
    static const float IDENTITY2[4] __lsp_aligned16 = { 0.0f, 0.0f, 1.0f, 0.0f };
    static const float IDENTITY3[4] __lsp_aligned16 = { 0.0f, 0.0f, 0.0f, 1.0f };

    void init_point_xyz(point3d_t *p, float x, float y, float z)
    {
        ARCH_X86_ASM
        (
            __ASM_EMIT64("andps     %[mask], %[x0]")        // xmm0 = x 0 0 0
            __ASM_EMIT64("andps     %[mask], %[x2]")        // xmm2 = z 0 0 0
            __ASM_EMIT("movlhps     %[x1], %[x0]")          // xmm0 = x 0 y ?
            __ASM_EMIT("shufps      $0x88, %[x2], %[x0]")   // xmm0 = x y z 0
            __ASM_EMIT("orps        %[omask], %[x0]")       // xmm0 = x y z 1
            __ASM_EMIT("movups      %[x0], (%[p])")

            : [x0] "+x" (x), [x1] "+x" (y), [x2] "+x"(z)
            : [p] "r" (p),
              [mask] "m" (X_MASK0001),
              [omask] "m" (X_3DPOINT)
            : "memory"
        );
    }

    void init_point(point3d_t *p, const point3d_t *s)
    {
        float x0;

        ARCH_X86_ASM
        (
            __ASM_EMIT("movups      (%[s]), %[x0]")
            __ASM_EMIT("movups      %[x0], (%[p])")
            : [x0] "=&x" (x0)
            : [s] "r" (s), [p] "r" (p)
            : "memory"
        );
    }

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

    void init_vector_dxyz(vector3d_t *p, float dx, float dy, float dz)
    {
        ARCH_X86_ASM
        (
            __ASM_EMIT64("andps     %[mask], %[x0]")        // xmm0 = dx 0 0 0
            __ASM_EMIT64("andps     %[mask], %[x2]")        // xmm2 = dz 0 0 0
            __ASM_EMIT("movlhps   %[x1], %[x0]")            // xmm0 = dx 0 dy ?
            __ASM_EMIT("shufps    $0x88, %[x2], %[x0]")     // xmm0 = dx dy dz 0
            __ASM_EMIT("movups    %[x0], (%[p])")

            : [x0] "+x" (dx), [x1] "+x" (dy), [x2] "+x"(dz)
            :
                [p] "r" (p),
                [mask] "m" (X_MASK0001)
            : "memory"
        );
    }

    void init_vector(vector3d_t *p, const vector3d_t *s)
    {
        float x0;

        ARCH_X86_ASM
        (
            __ASM_EMIT("movups      (%[s]), %[x0]")
            __ASM_EMIT("movups      %[x0], (%[p])")
            : [x0] "=&x" (x0)
            : [s] "r" (s), [p] "r" (p)
            : "memory"
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

    void init_ray_xyz(ray3d_t *l,
        float x0, float y0, float z0,
        float x1, float y1, float z1
    )
    {
        float x7;

        ARCH_X86_ASM
        (
            // xmm0 = x0 ? ? ?
            // xmm1 = y0 ? ? ?
            // xmm2 = z0 ? ? ?
            // xmm3 = x1 ? ? ?
            // xmm4 = y1 ? ? ?
            // xmm5 = z1 ? ? ?
            __ASM_EMIT("movaps      %[idm0], %[x7]")        // xmm7 = 1 0 0 0
            __ASM_EMIT("movlhps     %[x1], %[x0]")          // xmm0 = x0 ? y0 ?
            __ASM_EMIT("movlhps     %[x4], %[x3]")          // xmm3 = x1 ? y1 ?
            __ASM_EMIT("movlhps     %[x7], %[x2]")          // xmm2 = z0 ? 1 0
            __ASM_EMIT("movlhps     %[x7], %[x5]")          // xmm5 = z1 ? 1 0
            __ASM_EMIT("shufps      $0x88, %[x2], %[x0]")   // xmm2 = x0 y0 z0 1
            __ASM_EMIT("shufps      $0x88, %[x5], %[x3]")   // xmm3 = x1 y1 z1 1
            __ASM_EMIT("movups      %[x3], 0x00(%[l])")
            __ASM_EMIT("subps       %[x2], %[x3]")          // xmm3 = x1-x0 y1-y0 z1-z0 0
            __ASM_EMIT("movups      %[x3], 0x10(%[l])")

            : [x0] "+x" (x0), [x1] "+x" (y0), [x2] "+x" (z0),
              [x3] "+x" (x1), [x4] "+x" (y1), [x5] "+x" (z1),
              [x7] "=&x" (x7)
            : [l] "r" (l),
              [idm0] "m" (IDENTITY0)
            : "memory"
        );
    }

    void init_ray_dxyz(ray3d_t *l,
        float x0, float y0, float z0,
        float dx, float dy, float dz
    )
    {
        float x7;

        ARCH_X86_ASM
        (
            // xmm0 = x0 ? ? ?
            // xmm1 = y0 ? ? ?
            // xmm2 = z0 ? ? ?
            // xmm3 = x1 ? ? ?
            // xmm4 = y1 ? ? ?
            // xmm5 = z1 ? ? ?
            __ASM_EMIT("movaps      %[idm0], %[x7]")        // xmm7 = 1 0 0 0
            __ASM_EMIT("movlhps     %[x1], %[x0]")          // xmm0 = x0 ? y0 ?
            __ASM_EMIT("movlhps     %[x4], %[x3]")          // xmm3 = dx ? dy ?
            __ASM_EMIT("movlhps     %[x7], %[x2]")          // xmm2 = z0 ? 1 0
            __ASM_EMIT("movlhps     %[x7], %[x5]")          // xmm5 = dz ? 1 0
            __ASM_EMIT("shufps      $0x88, %[x2], %[x0]")   // xmm2 = x0 y0 z0 1
            __ASM_EMIT("shufps      $0xc8, %[x5], %[x3]")   // xmm5 = x1 y1 z1 0
            __ASM_EMIT("movups      %[x0], 0x00(%[l])")
            __ASM_EMIT("movups      %[x3], 0x10(%[l])")

            : [x0] "+x" (x0), [x1] "+x" (y0), [x2] "+x" (z0),
              [x3] "+x" (dx), [x4] "+x" (dy), [x5] "+x" (dz),
              [x7] "=&x" (x7)
            : [l] "r" (l),
              [idm0] "m" (IDENTITY0)
            : "memory"
        );
    }

    void init_ray_pdv(ray3d_t *l, const point3d_t *p, const vector3d_t *v)
    {
        float x0, x1;

        ARCH_X86_ASM
        (
            __ASM_EMIT("movups      (%[p]), %[x0]")
            __ASM_EMIT("movups      (%[v]), %[x1]")
            __ASM_EMIT("movups      %[x0], 0x00(%[l])")
            __ASM_EMIT("movups      %[x1], 0x10(%[l])")
            : [x0] "=&x" (x0), [x1] "=&x" (x1)
            : [l] "r" (l),
              [p] "r" (p),
              [v] "r" (v)
            : "memory"
        );
    }

    void init_ray_p2(ray3d_t *l, const point3d_t *p1, const point3d_t *p2)
    {
        float x0, x1;

        ARCH_X86_ASM
        (
            __ASM_EMIT("movups      0x00(%[p1]), %[x0]")    // xmm0 = x0 y0 z0 w0
            __ASM_EMIT("movups      0x00(%[p2]), %[x1]")    // xmm1 = x1 y1 z1 w1
            __ASM_EMIT("movaps      %[x0], %[x1]")          // xmm1 = x1-x0 y1-y0 z1-z0 w1-w0
            __ASM_EMIT("andps       %[mask], %[x1]")        // xmm1 = x1-x0 y1-y0 z1-z0 0
            __ASM_EMIT("movups      %[x0], 0x00(%[l])")
            __ASM_EMIT("movups      %[x1], 0x10(%[l])")

            : [x0] "=&x" (x0), [x1] "=&x" (x1)
            : [l] "r" (l),
              [p1] "r" (p1),
              [p2] "r" (p2),
              [mask] "m" (X_MASK0111)
            : "memory"
        );
    }

    void init_ray_pv(ray3d_t *l, const point3d_t *p)
    {
        float x0, x1;

        ARCH_X86_ASM
        (
            __ASM_EMIT("movups      0x00(%[p]), %[x0]")     // xmm0 = x0 y0 z0 w0
            __ASM_EMIT("movups      0x10(%[p]), %[x1]")     // xmm1 = x1 y1 z1 w1
            __ASM_EMIT("movaps      %[x0], %[x1]")          // xmm1 = x1-x0 y1-y0 z1-z0 w1-w0
            __ASM_EMIT("andps       %[mask], %[x1]")        // xmm1 = x1-x0 y1-y0 z1-z0 0
            __ASM_EMIT("movups      %[x0], 0x00(%[l])")
            __ASM_EMIT("movups      %[x1], 0x10(%[l])")

            : [x0] "=&x" (x0), [x1] "=&x" (x1)
            : [l] "r" (l),
              [p] "r" (p),
              [mask] "m" (X_MASK0111)
            : "memory"
        );
    }

    void init_ray(ray3d_t *l, const ray3d_t *r)
    {
        float x0, x1;

        ARCH_X86_ASM
        (
            __ASM_EMIT("movups      0x00(%[r]), %[x0]")
            __ASM_EMIT("movups      0x10(%[r]), %[x1]")
            __ASM_EMIT("movups      %[x0], 0x00(%[l])")
            __ASM_EMIT("movups      %[x1], 0x10(%[l])")
            : [x0] "=&x" (x0), [x1] "=&x" (x1)
            : [l] "r" (l),
              [r] "r" (r)
            : "memory"
        );
    }

    void calc_ray_xyz(ray3d_t *l,
        float x0, float y0, float z0,
        float x1, float y1, float z1
    )
    {
        float x7;

        ARCH_X86_ASM
        (
            // xmm0 = x0 ? ? ?
            // xmm1 = y0 ? ? ?
            // xmm2 = z0 ? ? ?
            // xmm3 = x1 ? ? ?
            // xmm4 = y1 ? ? ?
            // xmm5 = z1 ? ? ?
            __ASM_EMIT("movaps      %[idm0], %[x7]")        // xmm7 = 1 0 0 0
            __ASM_EMIT("movlhps     %[x1], %[x0]")          // xmm0 = x0 ? y0 ?
            __ASM_EMIT("movlhps     %[x4], %[x3]")          // xmm3 = x1 ? y1 ?
            __ASM_EMIT("movlhps     %[x7], %[x2]")          // xmm2 = z0 ? 1 0
            __ASM_EMIT("movlhps     %[x7], %[x5]")          // xmm5 = z1 ? 1 0
            __ASM_EMIT("shufps      $0x88, %[x2], %[x0]")   // xmm2 = x0 y0 z0 1
            __ASM_EMIT("shufps      $0x88, %[x5], %[x3]")   // xmm5 = x1 y1 z1 1
            __ASM_EMIT("subps       %[x2], %[x5]")          // xmm5 = x1-x0 y1-y0 z1-z0 0
            NORMALIZE("[x5]", "[x0]", "[x1]")
            __ASM_EMIT("movups      %[x2], 0x00(%[l])")
            __ASM_EMIT("movups      %[x5], 0x10(%[l])")

            : [x0] "+x" (x0), [x1] "+x" (y0), [x2] "+x" (z0),
              [x3] "+x" (x1), [x4] "+x" (y1), [x5] "+x" (z1),
              [x7] "=&x" (x7)
            : [l] "r" (l),
              [idm0] "m" (IDENTITY0)
            : "cc", "memory"
        );
    }

    void calc_ray_dxyz(ray3d_t *l,
        float x0, float y0, float z0,
        float dx, float dy, float dz
    )
    {
        float x7;

        ARCH_X86_ASM
        (
            // xmm0 = x0 ? ? ?
            // xmm1 = y0 ? ? ?
            // xmm2 = z0 ? ? ?
            // xmm3 = x1 ? ? ?
            // xmm4 = y1 ? ? ?
            // xmm5 = z1 ? ? ?
            __ASM_EMIT("movaps      %[idm0], %[x7]")        // xmm7 = 1 0 0 0
            __ASM_EMIT("movlhps     %[x1], %[x0]")          // xmm0 = x0 ? y0 ?
            __ASM_EMIT("movlhps     %[x4], %[x3]")          // xmm3 = dx ? dy ?
            __ASM_EMIT("movlhps     %[x7], %[x2]")          // xmm2 = z0 ? 1 0
            __ASM_EMIT("movlhps     %[x7], %[x5]")          // xmm5 = dz ? 1 0
            __ASM_EMIT("shufps      $0x88, %[x2], %[x0]")   // xmm2 = x0 y0 z0 1
            __ASM_EMIT("shufps      $0xc8, %[x5], %[x3]")   // xmm5 = x1 y1 z1 0
            NORMALIZE("[x5]", "[x0]", "[x1]")
            __ASM_EMIT("movups      %[x2], 0x00(%[l])")
            __ASM_EMIT("movups      %[x5], 0x10(%[l])")

            : [x0] "+x" (x0), [x1] "+x" (y0), [x2] "+x" (z0),
              [x3] "+x" (dx), [x4] "+x" (dy), [x5] "+x" (dz),
              [x7] "=&x" (x7)
            : [l] "r" (l),
              [idm0] "m" (IDENTITY0)
            : "cc", "memory"
        );
    }

    void calc_ray_pdv(ray3d_t *l, const point3d_t *p, const vector3d_t *v)
    {
        float x0, x1, x2, x3;

        ARCH_X86_ASM
        (
            __ASM_EMIT("movups      (%[p]), %[x0]")
            __ASM_EMIT("movups      (%[v]), %[x1]")
            NORMALIZE("[x1]", "[x2]", "[x3]")
            __ASM_EMIT("movups      %[x0], 0x00(%[l])")
            __ASM_EMIT("movups      %[x1], 0x10(%[l])")
            : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3)
            : [l] "r" (l),
              [p] "r" (p),
              [v] "r" (v)
            : "cc", "memory"
        );
    }

    void calc_ray_p2(ray3d_t *l, const point3d_t *p1, const point3d_t *p2)
    {
        float x0, x1, x2, x3;

        ARCH_X86_ASM
        (
            __ASM_EMIT("movups      0x00(%[p1]), %[x0]")    // xmm0 = x0 y0 z0 w0
            __ASM_EMIT("movups      0x00(%[p2]), %[x1]")    // xmm1 = x1 y1 z1 w1
            __ASM_EMIT("movaps      %[x0], %[x1]")          // xmm1 = x1-x0 y1-y0 z1-z0 w1-w0
            __ASM_EMIT("andps       %[mask], %[x1]")        // xmm1 = x1-x0 y1-y0 z1-z0 0
            NORMALIZE("[x1]", "[x2]", "[x3]")
            __ASM_EMIT("movups      %[x0], 0x00(%[l])")
            __ASM_EMIT("movups      %[x1], 0x10(%[l])")

            : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3)
            : [l] "r" (l),
              [p1] "r" (p1),
              [p2] "r" (p2),
              [mask] "m" (X_MASK0111)
            : "cc", "memory"
        );
    }

    void calc_ray_pv(ray3d_t *l, const point3d_t *p)
    {
        float x0, x1, x2, x3;

        ARCH_X86_ASM
        (
            __ASM_EMIT("movups      0x00(%[p]), %[x0]")     // xmm0 = x0 y0 z0 w0
            __ASM_EMIT("movups      0x10(%[p]), %[x1]")     // xmm1 = x1 y1 z1 w1
            __ASM_EMIT("movaps      %[x0], %[x1]")          // xmm1 = x1-x0 y1-y0 z1-z0 w1-w0
            __ASM_EMIT("andps       %[mask], %[x1]")        // xmm1 = x1-x0 y1-y0 z1-z0 0
            NORMALIZE("[x1]", "[x2]", "[x3]")
            __ASM_EMIT("movups      %[x0], 0x00(%[l])")
            __ASM_EMIT("movups      %[x1], 0x10(%[l])")

            : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3)
            : [l] "r" (l),
              [p] "r" (p),
              [mask] "m" (X_MASK0111)
            : "cc", "memory"
        );
    }

    void calc_ray(ray3d_t *l, const ray3d_t *r)
    {
        float x0, x1, x2, x3;

        ARCH_X86_ASM
        (
            __ASM_EMIT("movups      0x00(%[r]), %[x0]")
            __ASM_EMIT("movups      0x10(%[r]), %[x1]")
            NORMALIZE("[x1]", "[x2]", "[x3]")
            __ASM_EMIT("movups      %[x0], 0x00(%[l])")
            __ASM_EMIT("movups      %[x1], 0x10(%[l])")
            : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3)
            : [l] "r" (l),
              [r] "r" (r)
            : "cc", "memory"
        );
    }

    void init_segment_xyz(segment3d_t *s,
        float x0, float y0, float z0,
        float x1, float y1, float z1
    )
    {
        float x7;

        ARCH_X86_ASM
        (
            // xmm0 = x0 ? ? ?
            // xmm1 = y0 ? ? ?
            // xmm2 = z0 ? ? ?
            // xmm3 = x1 ? ? ?
            // xmm4 = y1 ? ? ?
            // xmm5 = z1 ? ? ?
            __ASM_EMIT("movaps      %[idm0], %[x7]")        // xmm7 = 1 0 0 0
            __ASM_EMIT("movlhps     %[x1], %[x0]")          // xmm0 = x0 ? y0 ?
            __ASM_EMIT("movlhps     %[x4], %[x3]")          // xmm3 = x1 ? y1 ?
            __ASM_EMIT("movlhps     %[x7], %[x2]")          // xmm2 = z0 ? 1 0
            __ASM_EMIT("movlhps     %[x7], %[x5]")          // xmm5 = z1 ? 1 0
            __ASM_EMIT("shufps      $0xc8, %[x2], %[x0]")   // xmm2 = x0 y0 z0 0
            __ASM_EMIT("shufps      $0xc8, %[x5], %[x3]")   // xmm5 = x1 y1 z1 0
            __ASM_EMIT("movups      %[x2], 0x00(%[s])")
            __ASM_EMIT("movups      %[x5], 0x10(%[s])")

            : [x0] "+x" (x0), [x1] "+x" (y0), [x2] "+x" (z0),
              [x3] "+x" (x1), [x4] "+x" (y1), [x5] "+x" (z1),
              [x7] "=&x" (x7)
            : [s] "r" (s),
              [idm0] "m" (IDENTITY0)
            : "memory"
        );
    }

    void init_segment_p2(segment3d_t *s, const point3d_t *p1, const point3d_t *p2)
    {
        float x0, x1;

        ARCH_X86_ASM
        (
            __ASM_EMIT("movups      0x00(%[p1]), %[x0]")    // xmm0 = x0 y0 z0 w0
            __ASM_EMIT("movups      0x00(%[p2]), %[x1]")    // xmm1 = x1 y1 z1 w1
            __ASM_EMIT("movups      %[x0], 0x00(%[s])")
            __ASM_EMIT("movups      %[x1], 0x10(%[s])")

            : [x0] "=&x" (x0), [x1] "=&x" (x1)
            : [s] "r" (s),
              [p1] "r" (p1),
              [p2] "r" (p2)
            : "memory"
        );
    }

    void init_segment_pv(segment3d_t *s, const point3d_t *p)
    {
        float x0, x1;

        ARCH_X86_ASM
        (
            __ASM_EMIT("movups      0x00(%[p]), %[x0]")     // xmm0 = x0 y0 z0 w0
            __ASM_EMIT("movups      0x10(%[p]), %[x1]")     // xmm1 = x1 y1 z1 w1
            __ASM_EMIT("movups      %[x0], 0x00(%[s])")
            __ASM_EMIT("movups      %[x1], 0x10(%[s])")

            : [x0] "=&x" (x0), [x1] "=&x" (x1)
            : [s] "r" (s),
              [p] "r" (p)
            : "memory"
        );
    }


    void init_matrix3d(matrix3d_t *dst, const matrix3d_t *src)
    {
        ARCH_X86_ASM
        (
            MATRIX_LOAD("s", "%xmm0", "%xmm1", "%xmm2", "%xmm3")
            MATRIX_STORE("d", "%xmm0", "%xmm1", "%xmm2", "%xmm3")
            :
            : [s] "r" (src), [d] "r" (dst)
            : "memory",
                "%xmm0", "%xmm1", "%xmm2", "%xmm3"
        );
    }

    void init_matrix3d_zero(matrix3d_t *m)
    {
        ARCH_X86_ASM
        (
            __ASM_EMIT("xorps       %%xmm0, %%xmm0")
            __ASM_EMIT("xorps       %%xmm1, %%xmm1")
            MATRIX_STORE("m", "%xmm0", "%xmm1", "%xmm0", "%xmm1")
            :
            : [m] "r" (m)
            : "memory",
                "%xmm0", "%xmm1"
        );
    }

    void init_matrix3d_one(matrix3d_t *m)
    {
        ARCH_X86_ASM
        (
            __ASM_EMIT("movaps      %[one], %%xmm0")
            __ASM_EMIT("movaps      %%xmm0, %%xmm1")
            MATRIX_STORE("m", "%xmm0", "%xmm1", "%xmm0", "%xmm1")
            :
            : [m] "r" (m), [one] "m" (ONE)
            : "memory",
                "%xmm0", "%xmm1"
        );
    }

    void init_matrix3d_identity(matrix3d_t *m)
    {
        ARCH_X86_ASM
        (
            __ASM_EMIT("movaps      %[idm0], %%xmm0")
            __ASM_EMIT("movaps      %[idm1], %%xmm1")
            __ASM_EMIT("movaps      %[idm2], %%xmm2")
            __ASM_EMIT("movaps      %[idm3], %%xmm3")
            MATRIX_STORE("m", "%xmm0", "%xmm1", "%xmm2", "%xmm3")
            :
            : [m] "r" (m),
                [idm0] "m" (IDENTITY0),
                [idm1] "m" (IDENTITY1),
                [idm2] "m" (IDENTITY2),
                [idm3] "m" (IDENTITY3)
            : "memory",
                "%xmm0", "%xmm1", "%xmm2", "%xmm3"
        );
    }

    void init_matrix3d_translate(matrix3d_t *m, float dx, float dy, float dz)
    {
        float x3, x4, x5;

        ARCH_X86_ASM
        (
            __ASM_EMIT("movaps      %[idm0], %[x3]")        // xmm3 = 1 0 0 0
            __ASM_EMIT("movaps      %[idm1], %[x4]")        // xmm4 = 0 1 0 0
            __ASM_EMIT("movaps      %[idm2], %[x5]")        // xmm5 = 0 0 1 0
            __ASM_EMIT("movlhps     %[x3], %[x2]")          // xmm2 = dz ? 1 0
            __ASM_EMIT("movlhps     %[x1], %[x0]")          // xmm0 = dx ? dy ?
            __ASM_EMIT("shufps      $0x88, %[x2], %[x0]")   // xmm0 = dx dy dz 1

            MATRIX_STORE("m", "[x3]", "[x4]", "[x5]", "[x0]")

            : [x0] "+x" (dx), [x1] "+x" (dy), [x2] "+x"(dz),
              [x3] "=&x" (x3), [x4] "=&x" (x4), [x5] "=&x" (x5)
            : [m] "r" (m),
              [idm0] "m" (IDENTITY0),
              [idm1] "m" (IDENTITY1),
              [idm2] "m" (IDENTITY2),
              [idm3] "m" (IDENTITY3)
            : "memory"
        );
    }

    void init_matrix3d_scale(matrix3d_t *m, float sx, float sy, float sz)
    {
        float x3;

        ARCH_X86_ASM
        (
            __ASM_EMIT("andps     %[mask], %[x0]")          // xmm0 = sx 0 0 0
            __ASM_EMIT("andps     %[mask], %[x1]")          // xmm1 = sy 0 0 0
            __ASM_EMIT("andps     %[mask], %[x2]")          // xmm2 = sz 0 0 0

            __ASM_EMIT("shufps      $0xf3, %[x1], %[x1]")   // xmm1 = 0 sy 0 0
            __ASM_EMIT("shufps      $0xcf, %[x2], %[x2]")   // xmm2 = 0 0 sz 0
            __ASM_EMIT("movaps      %[idm3], %[x3]")        // xmm3 = 0 0 0 1

            MATRIX_STORE("m", "[x0]", "[x1]", "[x2]", "[x3]")

            : [x0] "+x" (sx), [x1] "+x" (sy), [x2] "+x"(sz),
              [x3] "=&x" (x3)
            : [m] "r" (m),
              [idm3] "m" (IDENTITY3),
              [mask] "m" (X_MASK0001)
            : "memory"
        );
    }

    inline void init_matrix3d_rotate_x(matrix3d_t *m, float angle)
    {
        float s     = sinf(angle);
        float c     = cosf(angle);
        float x2, x3;

        ARCH_X86_ASM
        (
            __ASM_EMIT("movss       %[s], %[x0]")           // xmm0 = s 0 0 0
            __ASM_EMIT("movss       %[c], %[x1]")           // xmm1 = c 0 0 0

            __ASM_EMIT("shufps      $0xc3, %[x1], %[x0]")   // xmm0 = 0 s c 0
            __ASM_EMIT("movaps      %[idm0], %[x2]")        // xmm2 = 1 0 0 0
            __ASM_EMIT("movaps      %[x0], %[x1]")          // xmm1 = 0 s c 0
            __ASM_EMIT("movaps      %[idm3], %[x3]")        // xmm3 = 0 0 0 1
            __ASM_EMIT("xorps       %[mask], %[x0]")        // xmm0 = 0 -s c 0
            __ASM_EMIT("shufps      $0xd8, %[x1], %[x1]")   // xmm1 = 0 c s 0

            MATRIX_STORE("m", "[x2]", "[x1]", "[x0]", "[x3]")
            : [x0] "+x" (s), [x1] "+x" (c), [x2] "=&x" (x2), [x3] "=&x" (x3)
            : [s] "m" (s), [c] "m" (c),
                [m] "r" (m),
                [idm0] "m" (IDENTITY0),
                [idm3] "m" (IDENTITY3),
                [mask] "m" (X_SMASK0010)
            : "memory"
        );
    }

    inline void init_matrix3d_rotate_y(matrix3d_t *m, float angle)
    {
        float s     = sinf(angle);
        float c     = cosf(angle);
        float x2, x3;

        ARCH_X86_ASM
        (
            __ASM_EMIT("movss       %[s], %[x0]")          // xmm0 = s 0 0 0
            __ASM_EMIT("movss       %[c], %[x1]")          // xmm1 = c 0 0 0

            __ASM_EMIT("movlhps     %[x1], %[x0]")        // xmm0 = s 0 c 0
            __ASM_EMIT("movaps      %[x0], %[x1]")        // xmm1 = s 0 c 0
            __ASM_EMIT("movaps      %[idm1], %[x2]")       // xmm2 = 0 1 0 0
            __ASM_EMIT("xorps       %[mask], %[x1]")       // xmm1 = -s 0 c 0
            __ASM_EMIT("movaps      %[idm3], %[x3]")       // xmm3 = 0 0 0 1
            __ASM_EMIT("shufps      $0xc6, %[x1], %[x1]") // xmm1 = c 0 -s 0

            MATRIX_STORE("m", "[x1]", "[x2]", "[x0]", "[x3]")
            : [x0] "+x" (s), [x1] "+x" (c), [x2] "=&x" (x2), [x3] "=&x" (x3)
            : [s] "m" (s), [c] "m" (c),
                [m] "r" (m),
                [idm1] "m" (IDENTITY1),
                [idm3] "m" (IDENTITY3),
                [mask] "m" (X_SMASK0001)
            : "memory"
        );
    }

    inline void init_matrix3d_rotate_z(matrix3d_t *m, float angle)
    {
        float s     = sinf(angle);
        float c     = cosf(angle);
        float x2, x3, x4;

        ARCH_X86_ASM
        (
            __ASM_EMIT("movss       %[s], %[x0]")          // xmm0 = s 0 0 0
            __ASM_EMIT("movss       %[c], %[x1]")          // xmm1 = c 0 0 0

            __ASM_EMIT("movaps      %[x0], %[x2]")        // xmm2 = s 0 0 0
            __ASM_EMIT("movaps      %[idm2], %[x3]")       // xmm3 = 0 0 1 0
            __ASM_EMIT("unpcklps    %[x1], %[x0]")        // xmm0 = s c 0 0
            __ASM_EMIT("movaps      %[idm3], %[x4]")       // xmm3 = 0 0 0 1
            __ASM_EMIT("unpcklps    %[x2], %[x1]")        // xmm1 = c s 0 0
            __ASM_EMIT("xorps       %[mask], %[x0]")       // xmm0 = -s c 0 0

            MATRIX_STORE("m", "[x1]", "[x0]", "[x3]", "[x4]")
            : [x0] "+x" (s), [x1] "+x" (c), [x2] "=&x" (x2), [x3] "=&x" (x3), [x4] "=&x" (x4)
            : [s] "m" (s), [c] "m" (c),
                [m] "r" (m),
                [idm2] "m" (IDENTITY2),
                [idm3] "m" (IDENTITY3),
                [mask] "m" (X_SMASK0001)
            : "memory"
        );
    }

    void init_matrix3d_rotate_xyz(matrix3d_t *m, float x, float y, float z, float angle)
    {
        if (x == 0.0f)
        {
            if (y == 0.0f)
            {
                if (z > 0.0f)
                    init_matrix3d_rotate_z(m, angle);
                else if (z < 0.0f)
                    init_matrix3d_rotate_z(m, -angle);
                else // (z == 0.0)
                    init_matrix3d_identity(m);
            }
            else if (z == 0.0f)
            {
                if (y > 0.0f)
                    init_matrix3d_rotate_y(m, angle);
                else if (y < 0.0f)
                    init_matrix3d_rotate_y(m, -angle);
            }
            return;
        }
        else if ((y == 0.0) && (z == 0.0))
        {
            if (x > 0.0f)
                init_matrix3d_rotate_x(m, angle);
            else
                init_matrix3d_rotate_x(m, -angle);
            return;
        }

        float s = sinf(angle);
        float c = cosf(angle);

        float xx, yy, zz, xy, yz, zx, xs, ys, zs, one_c;
        float mag = sqrtf(x*x + y*y + z*z);

    //            if (mag <= 1.0e-4)
    //                return *this;

        x          /= mag;
        y          /= mag;
        z          /= mag;

        xx          = x * x;
        yy          = y * y;
        zz          = z * z;
        xy          = x * y;
        yz          = y * z;
        zx          = z * x;
        xs          = x * s;
        ys          = y * s;
        zs          = z * s;
        one_c       = 1.0f - c;

        float *M    = m->m;
        M[0]        = (one_c * xx) + c;
        M[1]        = (one_c * xy) + zs;
        M[2]        = (one_c * zx) - ys;
        M[3]        = 0.0f;
        M[4]        = (one_c * xy) - zs;
        M[5]        = (one_c * yy) + c;
        M[6]        = (one_c * yz) + xs;
        M[7]        = 0.0f;
        M[8]        = (one_c * zx) + ys;
        M[9]        = (one_c * yz) - xs;
        M[10]       = (one_c * zz) + c;
        M[11]       = 0.0f;
        M[12]       = 0.0f;
        M[13]       = 0.0f;
        M[14]       = 0.0f;
        M[15]       = 0.0f;
    }

    void apply_matrix3d_mv2(vector3d_t *r, const vector3d_t *v, const matrix3d_t *m)
    {
        ARCH_X86_ASM
        (
            __ASM_EMIT("movups      (%[v]), %%xmm0")        // xmm0 = vx vy vz vw
            __ASM_EMIT("movups      0x00(%[m]), %%xmm4")    // xmm4 = m0  m1  m2  m3
            __ASM_EMIT("movaps      %%xmm0, %%xmm1")        // xmm1 = vx vy vz vw
            __ASM_EMIT("movups      0x10(%[m]), %%xmm5")    // xmm5 = m4  m5  m6  m7
            __ASM_EMIT("movaps      %%xmm0, %%xmm2")        // xmm2 = vx vy vz vw
            __ASM_EMIT("movups      0x20(%[m]), %%xmm6")    // xmm6 = m8  m9  m10 m11
            __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0") // xmm0 = vx vx vx vx
            __ASM_EMIT("shufps      $0x55, %%xmm1, %%xmm1") // xmm1 = vy vy vy vy
            __ASM_EMIT("shufps      $0xaa, %%xmm2, %%xmm2") // xmm2 = vz vz vz vz

            __ASM_EMIT("mulps       %%xmm4, %%xmm0")
            __ASM_EMIT("mulps       %%xmm5, %%xmm1")
            __ASM_EMIT("mulps       %%xmm6, %%xmm2")
            __ASM_EMIT("addps       %%xmm1, %%xmm0")
            __ASM_EMIT("addps       %%xmm2, %%xmm0")        // xmm0 = rx ry rz rw

            __ASM_EMIT("xorps       %%xmm2, %%xmm2")        // xmm2 = 0 0 0 0
            __ASM_EMIT("movaps      %%xmm0, %%xmm1")        // xmm1 = rdx rdy rdz rdw
            __ASM_EMIT("shufps      $0xff, %%xmm1, %%xmm1") // xmm1 = rdw rdw rdw rdw
            __ASM_EMIT("ucomiss     %%xmm1, %%xmm2")        // xmm2 =?= xmm1
            __ASM_EMIT("je          1f")
            __ASM_EMIT("divps       %%xmm1, %%xmm0")
            __ASM_EMIT("1:")
            __ASM_EMIT("movups      %%xmm0, (%[r])")
            :
            : [r] "r" (r), [v] "r" (v), [m] "r" (m)
            : "cc", "memory",
                "%xmm0", "%xmm1", "%xmm2",
                "%xmm4", "%xmm5", "%xmm6"
        );
    }

    void apply_matrix3d_mv1(vector3d_t *r, const matrix3d_t *m)
    {
        ARCH_X86_ASM
        (
            __ASM_EMIT("movups      (%[r]), %%xmm0")        // xmm0 = vx vy vz vw
            __ASM_EMIT("movups      0x00(%[m]), %%xmm4")    // xmm4 = m0  m1  m2  m3
            __ASM_EMIT("movaps      %%xmm0, %%xmm1")        // xmm1 = vx vy vz vw
            __ASM_EMIT("movups      0x10(%[m]), %%xmm5")    // xmm5 = m4  m5  m6  m7
            __ASM_EMIT("movaps      %%xmm0, %%xmm2")        // xmm2 = vx vy vz vw
            __ASM_EMIT("movups      0x20(%[m]), %%xmm6")    // xmm6 = m8  m9  m10 m11
            __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0") // xmm0 = vx vx vx vx
            __ASM_EMIT("shufps      $0x55, %%xmm1, %%xmm1") // xmm1 = vy vy vy vy
            __ASM_EMIT("shufps      $0xaa, %%xmm2, %%xmm2") // xmm2 = vz vz vz vz

            __ASM_EMIT("mulps       %%xmm4, %%xmm0")
            __ASM_EMIT("mulps       %%xmm5, %%xmm1")
            __ASM_EMIT("mulps       %%xmm6, %%xmm2")
            __ASM_EMIT("addps       %%xmm1, %%xmm0")
            __ASM_EMIT("addps       %%xmm2, %%xmm0")        // xmm0 = rx ry rz rw

            __ASM_EMIT("xorps       %%xmm2, %%xmm2")        // xmm2 = 0 0 0 0
            __ASM_EMIT("movaps      %%xmm0, %%xmm1")        // xmm1 = rdx rdy rdz rdw
            __ASM_EMIT("shufps      $0xff, %%xmm1, %%xmm1") // xmm1 = rdw rdw rdw rdw
            __ASM_EMIT("ucomiss     %%xmm1, %%xmm2")        // xmm2 =?= xmm1
            __ASM_EMIT("je          1f")
            __ASM_EMIT("divps       %%xmm1, %%xmm0")
            __ASM_EMIT("1:")
            __ASM_EMIT("movups      %%xmm0, (%[r])")
            :
            : [r] "r" (r), [m] "r" (m)
            : "cc", "memory",
                "%xmm0", "%xmm1", "%xmm2",
                "%xmm4", "%xmm5", "%xmm6"
        );
    }

    void apply_matrix3d_mp2(point3d_t *r, const point3d_t *p, const matrix3d_t *m)
    {
        ARCH_X86_ASM
        (
            __ASM_EMIT("movups      (%[p]), %%xmm0")        // xmm0 = px py pz pw
            __ASM_EMIT("movups      0x00(%[m]), %%xmm4")    // xmm4 = m0  m1  m2  m3
            __ASM_EMIT("movaps      %%xmm0, %%xmm1")        // xmm1 = px py pz pw
            __ASM_EMIT("movups      0x10(%[m]), %%xmm5")    // xmm5 = m4  m5  m6  m7
            __ASM_EMIT("movaps      %%xmm0, %%xmm2")        // xmm2 = px py pz pw
            __ASM_EMIT("movups      0x20(%[m]), %%xmm6")    // xmm6 = m8  m9  m10 m11
            __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0") // xmm0 = px px px px
            __ASM_EMIT("movups      0x30(%[m]), %%xmm7")    // xmm7 = m12 m13 m14 m15
            __ASM_EMIT("shufps      $0x55, %%xmm1, %%xmm1") // xmm1 = py py py py
            __ASM_EMIT("shufps      $0xaa, %%xmm2, %%xmm2") // xmm1 = pz pz pz pz

            __ASM_EMIT("mulps       %%xmm4, %%xmm0")
            __ASM_EMIT("mulps       %%xmm5, %%xmm1")
            __ASM_EMIT("mulps       %%xmm6, %%xmm2")
            __ASM_EMIT("addps       %%xmm1, %%xmm0")
            __ASM_EMIT("addps       %%xmm7, %%xmm2")
            __ASM_EMIT("addps       %%xmm2, %%xmm0")        // xmm0 = rx ry rz rw

            __ASM_EMIT("xorps       %%xmm2, %%xmm2")        // xmm2 = 0 0 0 0
            __ASM_EMIT("movaps      %%xmm0, %%xmm1")        // xmm1 = rx ry rz rw
            __ASM_EMIT("shufps      $0xff, %%xmm1, %%xmm1") // xmm1 = rw rw rw rw
            __ASM_EMIT("ucomiss     %%xmm1, %%xmm2")        // xmm2 =?= xmm1
            __ASM_EMIT("je          1f")
            __ASM_EMIT("divps       %%xmm1, %%xmm0")
            __ASM_EMIT("1:")
            __ASM_EMIT("movups      %%xmm0, (%[r])")
            :
            : [r] "r" (r), [p] "r" (p), [m] "r" (m)
            : "cc", "memory",
                "%xmm0", "%xmm1", "%xmm2",
                "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void apply_matrix3d_mp1(point3d_t *r, const matrix3d_t *m)
    {
        ARCH_X86_ASM
        (
            __ASM_EMIT("movups      (%[r]), %%xmm0")        // xmm0 = px py pz pw
            __ASM_EMIT("movups      0x00(%[m]), %%xmm4")    // xmm4 = m0  m1  m2  m3
            __ASM_EMIT("movaps      %%xmm0, %%xmm1")        // xmm1 = px py pz pw
            __ASM_EMIT("movups      0x10(%[m]), %%xmm5")    // xmm5 = m4  m5  m6  m7
            __ASM_EMIT("movaps      %%xmm0, %%xmm2")        // xmm2 = px py pz pw
            __ASM_EMIT("movups      0x20(%[m]), %%xmm6")    // xmm6 = m8  m9  m10 m11
            __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0") // xmm0 = px px px px
            __ASM_EMIT("movups      0x30(%[m]), %%xmm7")    // xmm7 = m12 m13 m14 m15
            __ASM_EMIT("shufps      $0x55, %%xmm1, %%xmm1") // xmm1 = py py py py
            __ASM_EMIT("shufps      $0xaa, %%xmm2, %%xmm2") // xmm1 = pz pz pz pz

            __ASM_EMIT("mulps       %%xmm4, %%xmm0")
            __ASM_EMIT("mulps       %%xmm5, %%xmm1")
            __ASM_EMIT("mulps       %%xmm6, %%xmm2")
            __ASM_EMIT("addps       %%xmm1, %%xmm0")
            __ASM_EMIT("addps       %%xmm7, %%xmm2")
            __ASM_EMIT("addps       %%xmm2, %%xmm0")        // xmm0 = rx ry rz rw

            __ASM_EMIT("xorps       %%xmm2, %%xmm2")        // xmm2 = 0 0 0 0
            __ASM_EMIT("movaps      %%xmm0, %%xmm1")        // xmm1 = rx ry rz rw
            __ASM_EMIT("shufps      $0xff, %%xmm1, %%xmm1") // xmm1 = rw rw rw rw
            __ASM_EMIT("ucomiss     %%xmm1, %%xmm2")        // xmm2 =?= xmm1
            __ASM_EMIT("je          1f")
            __ASM_EMIT("divps       %%xmm1, %%xmm0")
            __ASM_EMIT("1:")
            __ASM_EMIT("movups      %%xmm0, (%[r])")
            :
            : [r] "r" (r), [m] "r" (m)
            : "cc", "memory",
                "%xmm0", "%xmm1", "%xmm2",
                "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    #define MATMUL_CORE(x0, x1, x2, x3, x4, x5, x6, x7) \
        __ASM_EMIT("movaps      %" x4 ", %" x5)         /* xmm5 = b0 b1 b2 b3 */ \
        __ASM_EMIT("movaps      %" x4 ", %" x6)         /* xmm6 = b0 b1 b2 b3 */ \
        __ASM_EMIT("movaps      %" x5 ", %" x7)         /* xmm7 = b0 b1 b2 b3 */ \
        __ASM_EMIT("shufps      $0x00, %" x4 ", %" x4)  /* xmm4 = b0 b0 b0 b0 */ \
        __ASM_EMIT("shufps      $0x55, %" x5 ", %" x5)  /* xmm5 = b1 b1 b1 b1 */ \
        __ASM_EMIT("shufps      $0xaa, %" x6 ", %" x6)  /* xmm6 = b2 b2 b2 b2 */ \
        __ASM_EMIT("shufps      $0xff, %" x7 ", %" x7)  /* xmm7 = b3 b3 b3 b3 */ \
        __ASM_EMIT("mulps       %" x0 ", %" x4) \
        __ASM_EMIT("mulps       %" x1 ", %" x5) \
        __ASM_EMIT("mulps       %" x2 ", %" x6) \
        __ASM_EMIT("mulps       %" x3 ", %" x7) \
        __ASM_EMIT("addps       %" x5 ", %" x4) \
        __ASM_EMIT("addps       %" x7 ", %" x6) \
        __ASM_EMIT("addps       %" x6 ", %" x4)

    void apply_matrix3d_mm2(matrix3d_t *r, const matrix3d_t *s, const matrix3d_t *m)
    {
        ARCH_X86_ASM
        (
            MATRIX_LOAD("A", "%xmm0", "%xmm1", "%xmm2", "%xmm3")

            // Multiply matrix and store
            __ASM_EMIT("movups      0x00(%[B]), %%xmm4")    // xmm4 = b0 b1 b2 b3
            MATMUL_CORE("%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5", "%xmm6", "%xmm7")
            __ASM_EMIT("movups      %%xmm4, 0x00(%[R])")

            __ASM_EMIT("movups      0x10(%[B]), %%xmm5")    // xmm5 = b4 b5 b6 b7
            MATMUL_CORE("%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm5", "%xmm6", "%xmm7", "%xmm4")
            __ASM_EMIT("movups      %%xmm5, 0x10(%[R])")

            __ASM_EMIT("movups      0x20(%[B]), %%xmm6")    // xmm6 = b4 b5 b6 b7
            MATMUL_CORE("%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm6", "%xmm7", "%xmm4", "%xmm5")
            __ASM_EMIT("movups      %%xmm6, 0x20(%[R])")

            __ASM_EMIT("movups      0x30(%[B]), %%xmm7")    // xmm7 = b4 b5 b6 b7
            MATMUL_CORE("%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm7", "%xmm4", "%xmm5", "%xmm6")
            __ASM_EMIT("movups      %%xmm7, 0x30(%[R])")

            :
            : [A] "r" (s), [B] "r" (m), [R] "r" (r)
            : "memory",
                "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void apply_matrix3d_mm1(matrix3d_t *r, const matrix3d_t *m)
    {
        matrix3d_t __lsp_aligned16 t;

        ARCH_X86_ASM
        (
            MATRIX_LOAD("A", "%xmm0", "%xmm1", "%xmm2", "%xmm3")

            // Multiply matrix and store
            __ASM_EMIT("movups      0x00(%[B]), %%xmm4")    // xmm4 = b0 b1 b2 b3
            MATMUL_CORE("%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5", "%xmm6", "%xmm7")
            __ASM_EMIT("movaps      %%xmm4, 0x00(%[T])")

            __ASM_EMIT("movups      0x10(%[B]), %%xmm5")    // xmm5 = b4 b5 b6 b7
            MATMUL_CORE("%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm5", "%xmm6", "%xmm7", "%xmm4")
            __ASM_EMIT("movaps      %%xmm5, 0x10(%[T])")

            __ASM_EMIT("movups      0x20(%[B]), %%xmm6")    // xmm6 = b4 b5 b6 b7
            MATMUL_CORE("%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm6", "%xmm7", "%xmm4", "%xmm5")
            __ASM_EMIT("movaps      %%xmm6, 0x20(%[T])")

            __ASM_EMIT("movups      0x30(%[B]), %%xmm7")    // xmm7 = b4 b5 b6 b7
            MATMUL_CORE("%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm7", "%xmm4", "%xmm5", "%xmm6")

            // Load matrix
            __ASM_EMIT("movaps      0x00(%[T]), %%xmm4")
            __ASM_EMIT("movaps      0x10(%[T]), %%xmm5")
            __ASM_EMIT("movaps      0x20(%[T]), %%xmm6")

            // Store matrix
            MATRIX_STORE("A", "%xmm4", "%xmm5", "%xmm6", "%xmm7")

            :
            : [A] "r" (r), [B] "r" (m), [T] "r" (&t)
            : "memory",
                "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                "%xmm4", "%xmm5", "%xmm6", "%xmm7"
        );
    }

    void transpose_matrix3d1(matrix3d_t *r)
    {
        ARCH_X86_ASM
        (
            MATRIX_LOAD("m", "%xmm0", "%xmm1", "%xmm2", "%xmm3")
            MAT4_TRANSPOSE("%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4")
            MATRIX_STORE("m", "%xmm0", "%xmm1", "%xmm2", "%xmm3")
            :
            : [m] "r" (r)
            : "memory",
                "%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4"
        );
    }

    void transpose_matrix3d2(matrix3d_t *r, const matrix3d_t *m)
    {
        ARCH_X86_ASM
        (
            MATRIX_LOAD("m", "%xmm0", "%xmm1", "%xmm2", "%xmm3")
            MAT4_TRANSPOSE("%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4")
            MATRIX_STORE("r", "%xmm0", "%xmm1", "%xmm2", "%xmm3")
            :
            : [r] "r" (r), [m] "r" (m)
            : "memory",
                "%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4"
        );
    }

    float check_triplet3d_p3n(const point3d_t *p1, const point3d_t *p2, const point3d_t *p3, const vector3d_t *n)
    {
        float x0, x1, x2, x3, x4;

        ARCH_X86_ASM
        (
            __ASM_EMIT("movups      (%[p2]), %[x1]")        // xmm1 = px2 py2 pz2 pw2
            __ASM_EMIT("movups      (%[p3]), %[x2]")        // xmm2 = px3 py3 pz3 pw3
            __ASM_EMIT("movups      (%[p1]), %[x4]")        // xmm4 = px1 py1 pz1 pw1
            __ASM_EMIT("subps       %[x1], %[x2]")          // xmm2 = x2 y2 z2 w2
            __ASM_EMIT("movups      (%[n]), %[x0]")         // xmm0 = nx ny nz nw
            __ASM_EMIT("subps       %[x4], %[x1]")          // xmm1 = x1 y1 z1 w1

            TRIPLET_CHECK("x0", "x1", "x2", "x3", "x4")

            : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3), [x4] "=&x" (x4)
            : [p1] "r" (p1), [p2] "r" (p2), [p3] "r" (p3), [n] "r" (n)
        );

        return x0;
    }

    float check_triplet3d_pvn(const point3d_t *pv, const vector3d_t *n)
    {
        float x0, x1, x2, x3, x4;

        ARCH_X86_ASM
        (
            __ASM_EMIT("movups      0x10(%[pv]), %[x1]")    // xmm1 = px2 py2 pz2 pw2
            __ASM_EMIT("movups      0x20(%[pv]), %[x2]")    // xmm2 = px3 py3 pz3 pw3
            __ASM_EMIT("movups      0x00(%[pv]), %[x4]")    // xmm4 = px1 py1 pz1 pw1
            __ASM_EMIT("subps       %[x1], %[x2]")          // xmm2 = x2 y2 z2 w2
            __ASM_EMIT("movups      (%[n]), %[x0]")         // xmm0 = nx ny nz nw
            __ASM_EMIT("subps       %[x4], %[x1]")          // xmm1 = x1 y1 z1 w1

            TRIPLET_CHECK("x0", "x1", "x2", "x3", "x4")

            : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3), [x4] "=&x" (x4)
            : [pv] "r" (pv), [n] "r" (n)
        );

        return x0;
    }

    float check_triplet3d_v2n(const vector3d_t *v1, const vector3d_t *v2, const vector3d_t *n)
    {
        float x0, x1, x2, x3, x4;

        ARCH_X86_ASM
        (
            __ASM_EMIT("movups      (%[v1]), %[x1]")       // xmm1 = x1 y1 z1 w1
            __ASM_EMIT("movups      (%[v2]), %[x2]")       // xmm2 = x2 y2 z2 w2
            __ASM_EMIT("movups      (%[n]), %[x0]")        // xmm0 = nx ny nz nw

            TRIPLET_CHECK("x0", "x1", "x2", "x3", "x4")

            : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3), [x4] "=&x" (x4)
            : [v1] "r" (v1), [v2] "r" (v2), [n] "r" (n)
        );

        return x0;
    }

    float check_triplet3d_vvn(const vector3d_t *v, const vector3d_t *n)
    {
        float x0, x1, x2, x3, x4;

        ARCH_X86_ASM
        (
            __ASM_EMIT("movups      0x00(%[v]), %[x1]")    // xmm1 = x1 y1 z1 w1
            __ASM_EMIT("movups      0x10(%[v]), %[x2]")    // xmm2 = x2 y2 z2 w2
            __ASM_EMIT("movups      (%[n]), %[x0]")        // xmm0 = nx ny nz nw

            TRIPLET_CHECK("x0", "x1", "x2", "x3", "x4")

            : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3), [x4] "=&x" (x4)
            : [v] "r" (v), [n] "r" (n)
        );

        return x0;
    }

    float check_triplet3d_vv(const vector3d_t *v)
    {
        float x0, x1, x2, x3, x4;

        ARCH_X86_ASM
        (
            __ASM_EMIT("movups      0x00(%[v]), %[x1]")    // xmm1 = x1 y1 z1 w1
            __ASM_EMIT("movups      0x10(%[v]), %[x2]")    // xmm2 = x2 y2 z2 w2
            __ASM_EMIT("movups      0x20(%[v]), %[x0]")    // xmm0 = nx ny nz nw

            TRIPLET_CHECK("x0", "x1", "x2", "x3", "x4")

            : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3), [x4] "=&x" (x4)
            : [v] "r" (v)
        );

        return x0;
    }

    float check_triplet3d_t(const triangle3d_t *t)
    {
        float x0, x1, x2, x3, x4;

        ARCH_X86_ASM
        (
            __ASM_EMIT("movups      0x10(%[t]), %[x1]")     // xmm1 = px2 py2 pz2 pw2
            __ASM_EMIT("movups      0x20(%[t]), %[x2]")     // xmm2 = px3 py3 pz3 pw3
            __ASM_EMIT("movups      0x00(%[t]), %[x4]")     // xmm3 = px1 py1 pz1 pw1
            __ASM_EMIT("subps       %[x1], %[x2]")          // xmm2 = x2 y2 z2 w2
            __ASM_EMIT("movups      0x30(%[t]), %[x0]")     // xmm0 = nx ny nz nw
            __ASM_EMIT("subps       %[x4], %[x1]")          // xmm1 = x1 y1 z1 w1

            TRIPLET_CHECK("x0", "x1", "x2", "x3", "x4")

            : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3), [x4] "=&x" (x4)
            : [t] "r" (t)
        );

        return x0;
    }

    float check_triplet3d_tn(const triangle3d_t *t, const vector3d_t *n)
    {
        float x0, x1, x2, x3, x4;

        ARCH_X86_ASM
        (
            __ASM_EMIT("movups      0x10(%[t]), %[x1]")     // xmm1 = px2 py2 pz2 pw2
            __ASM_EMIT("movups      0x20(%[t]), %[x2]")     // xmm2 = px3 py3 pz3 pw3
            __ASM_EMIT("movups      0x00(%[t]), %[x4]")     // xmm4 = px1 py1 pz1 pw1
            __ASM_EMIT("subps       %[x1], %[x2]")          // xmm2 = x2 y2 z2 w2
            __ASM_EMIT("movups      (%[n]), %[x0]")         // xmm0 = nx ny nz nw
            __ASM_EMIT("subps       %[x4], %[x1]")          // xmm1 = x1 y1 z1 w1

            TRIPLET_CHECK("x0", "x1", "x2", "x3", "x4")

            : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3), [x4] "=&x" (x4)
            : [t] "r" (t), [n] "r" (n)
        );

        return x0;
    }

    float check_point3d_on_edge_p2p(const point3d_t *p1, const point3d_t *p2, const point3d_t *p)
    {
        float x0, x1, x2, x3;

        ARCH_X86_ASM
        (
            // Calc vectors
            __ASM_EMIT("movups      (%[p1]), %[x0]")        // xmm0 = x1 y1 z1 w1
            __ASM_EMIT("movups      (%[p2]), %[x1]")        // xmm1 = x0 y0 z0 w0
            __ASM_EMIT("movaps      %[x0], %[x2]")          // xmm2 = x1 y1 z1 w1
            __ASM_EMIT("movups      (%[p]), %[x3]")         // xmm3 = xp yp zp wp

            __ASM_EMIT("subps       %[x1], %[x0]")          // xmm0 = x1-x0 y1-y0 z1-z0 w1-w0
            __ASM_EMIT("subps       %[x3], %[x1]")          // xmm1 = x0-xp y1-yp z0-zp w0-wp
            __ASM_EMIT("mulps       %[x0], %[x0]")
            __ASM_EMIT("subps       %[x3], %[x2]")          // xmm2 = x1-xp y1-yp z1-zp w1-wp
            __ASM_EMIT("mulps       %[x1], %[x1]")
            __ASM_EMIT("mulps       %[x2], %[x2]")

            // Calc scalar multiplication
            MAT3_TRANSPOSE("[x0]", "[x1]", "[x2]", "[x3]")
            __ASM_EMIT("addps       %[x1], %[x0]")
            __ASM_EMIT("addps       %[x3], %[x0]")          // xmm0 = S0 S1 S2 ?
            __ASM_EMIT("sqrtps      %[x0], %[x0]")

            __ASM_EMIT("movhlps     %[x0], %[x2]")          // xmm2 = S2 ? ? ? ?
            __ASM_EMIT("unpcklps    %[x0], %[x0]")          // xmm0 = S0 S0 S1 S1
            __ASM_EMIT("movhlps     %[x0], %[x1]")          // xmm1 = S1 ? ? ? ?
            __ASM_EMIT("subps       %[x2], %[x0]")
            __ASM_EMIT("subps       %[x1], %[x0]")

            __ASM_EMIT("2:")
            : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3)
            : [p1] "r" (p1), [p2] "r" (p2), [p] "r" (p)
        );

        return x0;
    }

    float check_point3d_on_edge_pvp(const point3d_t *pv, const point3d_t *p)
    {
        float x0, x1, x2, x3;

        ARCH_X86_ASM
        (
            // Calc vectors
            __ASM_EMIT("movups      0x00(%[pv]), %[x0]")    // xmm0 = x1 y1 z1 w1
            __ASM_EMIT("movups      0x10(%[pv]), %[x1]")    // xmm1 = x0 y0 z0 w0
            __ASM_EMIT("movaps      %[x0], %[x2]")          // xmm2 = x1 y1 z1 w1
            __ASM_EMIT("movups      (%[p]), %[x3]")         // xmm3 = xp yp zp wp

            __ASM_EMIT("subps       %[x1], %[x0]")          // xmm0 = x1-x0 y1-y0 z1-z0 w1-w0
            __ASM_EMIT("subps       %[x3], %[x1]")          // xmm1 = x0-xp y1-yp z0-zp w0-wp
            __ASM_EMIT("mulps       %[x0], %[x0]")
            __ASM_EMIT("subps       %[x3], %[x2]")          // xmm2 = x1-xp y1-yp z1-zp w1-wp
            __ASM_EMIT("mulps       %[x1], %[x1]")
            __ASM_EMIT("mulps       %[x2], %[x2]")

            // Calc scalar multiplication
            MAT3_TRANSPOSE("[x0]", "[x1]", "[x2]", "[x3]")
            __ASM_EMIT("addps       %[x1], %[x0]")
            __ASM_EMIT("addps       %[x3], %[x0]")          // xmm0 = S0 S1 S2 ?
            __ASM_EMIT("sqrtps      %[x0], %[x0]")

            __ASM_EMIT("movhlps     %[x0], %[x2]")          // xmm2 = S2 ? ? ? ?
            __ASM_EMIT("unpcklps    %[x0], %[x0]")          // xmm0 = S0 S0 S1 S1
            __ASM_EMIT("movhlps     %[x0], %[x1]")          // xmm1 = S1 ? ? ? ?
            __ASM_EMIT("subps       %[x2], %[x0]")
            __ASM_EMIT("subps       %[x1], %[x0]")

            __ASM_EMIT("2:")
            : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3)
            : [pv] "r" (pv), [p] "r" (p)
        );

        return x0;
    }

    size_t longest_edge3d_p3(const point3d_t *p1, const point3d_t *p2, const point3d_t *p3)
    {
        size_t res;
        float x0, x1, x2, x3;

        ARCH_X86_ASM
        (
            // Calc vectors
            __ASM_EMIT("movups      (%[p1]), %[x2]")        // xmm2 = x0 y0 z0 w0
            __ASM_EMIT("movups      (%[p2]), %[x0]")        // xmm0 = x1 y1 z1 w1
            __ASM_EMIT("movups      (%[p3]), %[x1]")        // xmm1 = x2 y2 z2 w2
            __ASM_EMIT("movaps      %[x2], %[x3]")          // xmm3 = x0 y0 z0 w0
            __ASM_EMIT("subps       %[x1], %[x2]")          // xmm2 = x0-x2 y0-y2 z0-z2 w0-w2 = dx2 dy2 dz2 dw2
            __ASM_EMIT("subps       %[x0], %[x1]")          // xmm1 = x2-x1 y2-y1 z2-z1 w2-w1 = dx1 dy1 dz1 dw1
            __ASM_EMIT("subps       %[x3], %[x0]")          // xmm0 = x1-x0 y1-y0 z1-z0 w1-w0 = dx0 dy0 dz0 dw0

            // Calc scalar multiplication
            __ASM_EMIT("mulps       %[x2], %[x2]")          // xmm2 = sx2 sy2 sz2 sw2
            __ASM_EMIT("mulps       %[x1], %[x1]")          // xmm1 = sx2 sy2 sz2 sw2
            __ASM_EMIT("mulps       %[x0], %[x0]")          // xmm0 = sx2 sy2 sz2 sw2
            MAT3_TRANSPOSE("[x0]", "[x1]", "[x2]", "[x3]")
            __ASM_EMIT("addps       %[x1], %[x0]")
            __ASM_EMIT("addps       %[x3], %[x0]")          // xmm0 = S0 S1 S2 ?

            __ASM_EMIT("xor         %[res], %[res]")
            __ASM_EMIT("movhlps     %[x0], %[x2]")          // xmm2 = S2 ? ? ? ?
            __ASM_EMIT("unpcklps    %[x0], %[x0]")          // xmm0 = S0 S0 S1 S1
            __ASM_EMIT("movhlps     %[x0], %[x1]")          // xmm1 = S1 ? ? ? ?

            __ASM_EMIT("ucomiss     %[x1], %[x0]")          // S0 <?> S1
            __ASM_EMIT("ja          1f")                   // S0 > S1
                __ASM_EMIT("ucomiss     %[x2], %[x1]")          // S1 <?> S2
                __ASM_EMIT("setbe       %%al")
                __ASM_EMIT("inc         %%al")
                __ASM_EMIT("jmp         2f")
            __ASM_EMIT("1:")                                // S0 <= S1
                __ASM_EMIT("ucomiss     %[x2], %[x0]")          // S0 <?> S2
                __ASM_EMIT("ja          2f")                    // S0 > S2
                __ASM_EMIT("add         $2, %%al")              // res = 2
            __ASM_EMIT("2:")
            : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3),
              [res] "=a" (res)
            : [p1] "r" (p1), [p2] "r" (p2), [p3] "r" (p3)
        );

        return res;
    }

    size_t longest_edge3d_pv(const point3d_t *p)
    {
        size_t res;
        float x0, x1, x2, x3;

        ARCH_X86_ASM
        (
            // Calc vectors
            __ASM_EMIT("movups      0x00(%[p]), %[x2]")     // xmm2 = x0 y0 z0 w0
            __ASM_EMIT("movups      0x10(%[p]), %[x0]")     // xmm0 = x1 y1 z1 w1
            __ASM_EMIT("movups      0x20(%[p]), %[x1]")     // xmm1 = x2 y2 z2 w2
            __ASM_EMIT("movaps      %[x2], %[x3]")          // xmm3 = x0 y0 z0 w0
            __ASM_EMIT("subps       %[x1], %[x2]")          // xmm2 = x0-x2 y0-y2 z0-z2 w0-w2 = dx2 dy2 dz2 dw2
            __ASM_EMIT("subps       %[x0], %[x1]")          // xmm1 = x2-x1 y2-y1 z2-z1 w2-w1 = dx1 dy1 dz1 dw1
            __ASM_EMIT("subps       %[x3], %[x0]")          // xmm0 = x1-x0 y1-y0 z1-z0 w1-w0 = dx0 dy0 dz0 dw0

            // Calc scalar multiplication
            __ASM_EMIT("mulps       %[x2], %[x2]")          // xmm2 = sx2 sy2 sz2 sw2
            __ASM_EMIT("mulps       %[x1], %[x1]")          // xmm1 = sx2 sy2 sz2 sw2
            __ASM_EMIT("mulps       %[x0], %[x0]")          // xmm0 = sx2 sy2 sz2 sw2
            MAT3_TRANSPOSE("[x0]", "[x1]", "[x2]", "[x3]")
            __ASM_EMIT("addps       %[x1], %[x0]")
            __ASM_EMIT("addps       %[x3], %[x0]")          // xmm0 = S0 S1 S2 ?

            __ASM_EMIT("xor         %[res], %[res]")
            __ASM_EMIT("movhlps     %[x0], %[x2]")          // xmm2 = S2 ? ? ? ?
            __ASM_EMIT("unpcklps    %[x0], %[x0]")          // xmm0 = S0 S0 S1 S1
            __ASM_EMIT("movhlps     %[x0], %[x1]")          // xmm1 = S1 ? ? ? ?

            __ASM_EMIT("ucomiss     %[x1], %[x0]")          // S0 <?> S1
            __ASM_EMIT("ja          1f")                   // S0 > S1
                __ASM_EMIT("ucomiss     %[x2], %[x1]")          // S1 <?> S2
                __ASM_EMIT("setbe       %%al")
                __ASM_EMIT("inc         %%al")
                __ASM_EMIT("jmp         2f")
            __ASM_EMIT("1:")                                // S0 <= S1
                __ASM_EMIT("ucomiss     %[x2], %[x0]")          // S0 <?> S2
                __ASM_EMIT("ja          2f")                    // S0 > S2
                __ASM_EMIT("add         $2, %%al")              // res = 2
            __ASM_EMIT("2:")
            : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3),
              [res] "=a" (res)
            : [p] "r" (p)
        );

        return res;
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
    //            m[2].dw      = l->v.dx * l->z.y - l->v.dy * l->z.x;
    //            m[3].dw      = l->v.dy * l->z.z - l->v.dz * l->z.y;
    //            m[1].dw      = l->v.dz * l->z.x - l->v.dx * l->z.z;

    //            point3d_t   ix; // intersection point
    //
    //            // Form equations
    //            /*
    //
    //             Nx*x0 + Ny*y0 + Nz*z0    + W               = 0
    //           - dz*x0 +         dx*z0    + (dz*xs - dx*zs) = 0
    //             dy*x0 - dx*y0            + (dx*ys - dy*xs) = 0
    //                     dz*y0 - dy*z0    + (dy*zs - dz*ys) = 0
    //
    //             */
    //            vector3d_t  m[4];
    //
    //            m[0].dx      = t->n.dx;
    //            m[0].dy      = t->n.dy;
    //            m[0].dz      = t->n.dz;
    //            m[0].dw      = t->n.dw;
    //
    //            m[1].dx      = -l->v.dz;
    //            m[1].dy      = 0.0f;
    //            m[1].dz      = l->v.dx;
    //            m[1].dw      = l->v.dz * l->z.x - l->v.dx * l->z.z;
    //
    //            m[2].dx      = l->v.dy;
    //            m[2].dy      = -l->v.dx;
    //            m[2].dz      = 0.0f;
    //            m[2].dw      = l->v.dx * l->z.y - l->v.dy * l->z.x;
    //
    //            m[3].dx      = 0.0f;
    //            m[3].dy      = l->v.dz;
    //            m[3].dz      = -l->v.dy;
    //            m[3].dw      = l->v.dy * l->z.z - l->v.dz * l->z.y;
    //
    //            if (is_zero(m[0].dx))
    //            {
    //                if (!is_zero(m[1].dx))
    //                    swap_vectors(&m[0], &m[1]);
    //                else if (!is_zero(m[2].dx))
    //                    swap_vectors(&m[0], &m[2]);
    //                else
    //                    return -1.0f;
    //            }
    //
    //            if (!is_zero(m[1].dx))
    //            {
    //                float k      = m[1].dx / m[0].dx;
    //                m[1].dx      = m[1].dx - m[0].dx * k;
    //                m[1].dy      = m[1].dy - m[0].dy * k;
    //                m[1].dz      = m[1].dz - m[0].dz * k;
    //                m[1].dw      = m[1].dw - m[0].dw * k;
    //            }
    //            if (!is_zero(m[2].dx))
    //            {
    //                float k      = m[2].dx / m[0].dx;
    //                m[2].dx      = m[2].dx - m[0].dx * k;
    //                m[2].dy      = m[2].dy - m[0].dy * k;
    //                m[2].dz      = m[2].dz - m[0].dz * k;
    //                m[2].dw      = m[2].dw - m[0].dw * k;
    //            }
    //
    //            // Solve equations, step 1
    //            if (is_zero(m[1].dy))
    //            {
    //                if (!is_zero(m[2].dy))
    //                    swap_vectors(&m[1], &m[2]);
    //                else if (!is_zero(m[3].dy))
    //                    swap_vectors(&m[1], &m[3]);
    //                else
    //                    return -1.0f;
    //            }
    //
    //            if (!is_zero(m[2].dy))
    //            {
    //                float k      = m[2].dy / m[1].dy;
    //                m[2].dx      = m[2].dx - m[1].dx * k;
    //                m[2].dy      = m[2].dy - m[1].dy * k;
    //                m[2].dz      = m[2].dz - m[1].dz * k;
    //                m[2].dw      = m[2].dw - m[1].dw * k;
    //            }
    //            if (!is_zero(m[3].dy))
    //            {
    //                float k      = m[3].dy / m[1].dy;
    //                m[3].dx      = m[3].dx - m[1].dx * k;
    //                m[3].dy      = m[3].dy - m[1].dy * k;
    //                m[3].dz      = m[3].dz - m[1].dz * k;
    //                m[3].dw      = m[3].dw - m[1].dw * k;
    //            }
    //
    //            // Check that matrix form is right
    //            if (is_zero(m[2].dz))
    //            {
    //                if (!is_zero(m[3].dz))
    //                    swap_vectors(&m[2], &m[3]);
    //                else
    //                    return -1.0f;
    //            }
    //
    //            // Now solve matrix into intersection point
    //            ix.z        = - m[2].dw / m[2].dz;
    //            ix.y        = - (m[1].dw + m[1].dz * ix.z) / m[1].dy;
    //            ix.x        = - (m[0].dw + m[0].dy * ix.y + m[0].dz * ix.z) / m[0].dx;
    //
    //            //---------------------------------------------------
    //            // PART 2: check that point lies on the ray
    //            vector3d_t  pv; // Projection vector
    //            pv.dx       = ix.x - l->z.x;
    //            pv.dy       = ix.y - l->z.y;
    //            pv.dz       = ix.z - l->z.z;
    //
    //            float proj  = pv.dx*l->v.dx + pv.dy*l->v.dy + pv.dz*l->v.dz; // Projection on the ray, can be also interpreted as a distance
    //            if (proj < 0.0f)
    //                return proj;
    //
    //            //---------------------------------------------------
    //            // PART 3: check that point lies within a triangle
    //            if (check_point3d_on_triangle_tp(t, &ix) < 0.0f)
    //                return -1.0f;
    //
    //            // Return point
    //            *ip         = ix;
    //            return proj;

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

    // This works slower:
    //                __ASM_EMIT("cmpltps     %[x1], %[x2]")
    //                __ASM_EMIT("xorps       %[x0], %[x2]")
    //                __ASM_EMIT("movmskps    %[x2], %[tmp1]")
    //                __ASM_EMIT("cmpltps     %[x1], %[x3]")
    //                __ASM_EMIT("cmp         $0x07, %b[tmp1]")
    //                __ASM_EMIT("jnz         0f")
    //
    //                __ASM_EMIT("xorps       %[x0], %[x3]")
    //                __ASM_EMIT("movmskps    %[x3], %[tmp1]")
    //                __ASM_EMIT("cmpltps     %[x1], %[x4]")
    //                __ASM_EMIT("cmp         $0x07, %b[tmp1]")
    //                __ASM_EMIT("jnz         0f")
    //
    //                __ASM_EMIT("xorps       %[x0], %[x4]")
    //                __ASM_EMIT("movmskps    %[x4], %[tmp1]")
    //                __ASM_EMIT("cmp         $0x07, %b[tmp1]")
    //                __ASM_EMIT("jz          2000f")                 /* signs differ */


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
    //                __ASM_EMIT("unpckhps        %[x3], %[x3]")          /* xmm3 = c2 c2 c3 c3 */
    //                __ASM_EMIT("xorps           %[x0], %[x0]")          /* xmm0 = 0 0 0 0 */
    //                __ASM_EMIT("movhlps         %[x3], %[x4]")          /* xmm4 = c3 c3 ? ? */
    //                __ASM_EMIT("divss           %[x3], %[x4]")          /* xmm4 = c3/c2 */
    //                __ASM_EMIT("subss           %[x4], %[x0]")          /* xmm0 = -c3/c2 0 0 0 = iz 0 0 0 */
    //
    //                __ASM_EMIT("movhlps         %[x2], %[x3]")          /* xmm3 = b2 b3 ? ? */
    //                __ASM_EMIT("mulss           %[x0], %[x3]")          /* xmm3 = b2*iz b3 ? ? */
    //                __ASM_EMIT("shufps          $0x93, %[x0], %[x0]")   /* xmm0 = 0 iz 0 0 */
    //                __ASM_EMIT("shufps          $0x55, %[x2], %[x2]")   /* xmm2 = b1 b1 b1 b1 */
    //                __ASM_EMIT("unpcklps        %[x3], %[x3]")          /* xmm3 = b2*iz b2*iz b3 b3 */
    //                __ASM_EMIT("movhlps         %[x3], %[x4]")          /* xmm4 = b3 b3 */
    //                __ASM_EMIT("addss           %[x3], %[x4]")          /* xmm4 = b3 + b2*iz */
    //                __ASM_EMIT("divss           %[x2], %[x4]")          /* xmm4 = (b3 + b2*iz)/b1 */
    //                __ASM_EMIT("subss           %[x4], %[x0]")          /* xmm0 = -(b3 + b2*iz)/b1 iz 0 0 = iy iz 0 0 */
    //
    //                __ASM_EMIT("movaps          %[x1], %[x2]")          /* xmm2 = a0 a1 a2 a3 */
    //                __ASM_EMIT("shufps          $0x93, %[x0], %[x0]")   /* xmm0 = 0 iy iz 0 */
    //                __ASM_EMIT("mulps           %[x0], %[x1]")          /* xmm1 = 0 iy*a1 iz*a2 0 */
    //                __ASM_EMIT("movhlps         %[x2], %[x3]")          /* xmm3 = a2 a3 ? ? */
    //                __ASM_EMIT("movhlps         %[x1], %[x4]")          /* xmm4 =  0 ? ? */
    //                __ASM_EMIT("addps           %[x3], %[x1]")          /* xmm1 = a2 a3+iy*a1 ? ? */
    //                __ASM_EMIT("shufps          $0x55, %[x1], %[x1]")   /* xmm1 = a3+iy*a1 a3+iy*a1 a3+iy*a1 a3+iy*a1 */
    //                __ASM_EMIT("addss           %[x1], %[x4]")          /* xmm4 = a3+iy*a1+iz*a2 */
    //                __ASM_EMIT("divss           %[x2], %[x4]")          /* xmm4 = (a3 + iy*a1 + iz*a2)/a0 */
    //                __ASM_EMIT("subss           %[x4], %[x0]")          /* xmm0 = -((a3 + iy*a1 + iz*a2) iy iz 0 = ix iy iz 0 */

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
            __ASM_EMIT("mulps           %[x3], %[x1]")          /* xmm1 = rdx*pdx rdy*pdy rdz*pdz rdw*pdw */
            __ASM_EMIT("movhlps         %[x1], %[x2]")          /* xmm2 = rdz*pdz rdw*pdw ? ? */
            __ASM_EMIT("unpcklps        %[x1], %[x1]")          /* xmm1 = rdx*pdx rdx*pdx rdy*pdy rdy*pdy */
            __ASM_EMIT("movhlps         %[x1], %[x3]")          /* xmm3 = rdy*pdy rdy*pdy ? ? */
            __ASM_EMIT("addss           %[x2], %[x1]")          /* xmm1 = rdx*pdx + rdz*pdz */
            __ASM_EMIT("xorps           %[x4], %[x4]")          /* xmm4 = 0 */
            __ASM_EMIT("addss           %[x3], %[x1]")          /* xmm1 = rdx*pdx + rdy*pdy + rdz*pdz = proj */

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

    void reflect_ray(raytrace3d_t *rt, raytrace3d_t *rf, const intersection3d_t *ix)
    {
        // TODO: update algorithm
        void *p1, *p2;
        size_t tmp;

        material3d_t m __lsp_aligned16;
        const material3d_t **mp     = const_cast<const material3d_t **>(ix->m);
        const triangle3d_t **tp     = const_cast<const triangle3d_t **>(ix->t);
        size_t n                    = ix->n;

        ARCH_X86_ASM
        (
            __ASM_EMIT("mov         (%[m]), %[p1]")
            __ASM_EMIT("mov         (%[t]), %[p2]")
            __ASM_EMIT("movups      0x00(%[p1]), %%xmm1")       // xmm1 = material[0..3]
            __ASM_EMIT("movups      0x10(%[p1]), %%xmm2")       // xmm2 = material[4..7]
            __ASM_EMIT("movups      0x30(%[p2]), %%xmm0")       // xmm0 = nx ny nz nw
            __ASM_EMIT("dec         %[n]")                      // n--
            __ASM_EMIT("jz          3f")
            __ASM_EMIT("mov         %[n], %[tmp]")              // store N-1
            __ASM_EMIT("1:")

            // Calculate average normal and material
            __ASM_EMIT("add         %[step], %[m]")             // p1++
            __ASM_EMIT("add         %[step], %[t]")             // p2++
            __ASM_EMIT("mov         (%[m]), %[p1]")
            __ASM_EMIT("mov         (%[t]), %[p2]")
            __ASM_EMIT("movups      0x00(%[p1]), %%xmm4")       // xmm4 = material[0..3]
            __ASM_EMIT("movups      0x10(%[p1]), %%xmm5")       // xmm5 = material[4..7]
            __ASM_EMIT("movups      0x00(%[p2]), %%xmm3")       // xmm3 = nx ny nz nw
            __ASM_EMIT("addps       %%xmm4, %%xmm1")
            __ASM_EMIT("addps       %%xmm5, %%xmm2")
            __ASM_EMIT("addps       %%xmm3, %%xmm0")
            __ASM_EMIT("dec         %[n]")                      // n--
            __ASM_EMIT("jnz         1b")

            // Normalize normal
            __ASM_EMIT("mov         %[tmp], %[n]")              // restore N-1
            NORMALIZE("%xmm0", "%xmm3", "%xmm4")
            __ASM_EMIT("inc         %[n]")
            __ASM_EMIT("cvtsi2ss    %[n], %%xmm3")
            __ASM_EMIT("shufps      $0x00, %%xmm3, %%xmm3")
            __ASM_EMIT("divps       %%xmm3, %%xmm1")
            __ASM_EMIT("divps       %%xmm3, %%xmm2")

            __ASM_EMIT("3:")

            // Store material
            __ASM_EMIT("lea         %[mat], %[p1]")
            __ASM_EMIT("movaps      %%xmm1, 0x00(%[p1])")
            __ASM_EMIT("movaps      %%xmm2, 0x10(%[p1])")

            : [m] "+r" (mp), [t] "+r" (tp), [n] "+r" (n), __IF_64([tmp] "=&r" (tmp), )
              [p1] "=&r" (p1), [p2] "=&r" (p2)
            : __IF_32([tmp] "m" (tmp), )
              [step] "i" (sizeof(void *)),
              [mat] "m" (m)
            : "cc", "memory",
                "%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5"
        );

        float x0, x1, x2, x3;

        ARCH_X86_ASM
        (
            // Calculate the tangent and normal vectors
            __ASM_EMIT("movups      0x10(%[rt]), %%xmm4")       // xmm4 = dx1 dy1 dz1 dw1
            __ASM_EMIT("movaps      %%xmm4, %%xmm3")            // xmm3 = dx1 dy1 dz1 dw1
            SCALAR_MUL("%xmm4", "%xmm0", "%xmm5")               // xmm4 = dx1*nx + dy1*ny + dz1*nz = proj
            __ASM_EMIT("shufps      $0x00, %%xmm4, %%xmm4")     // xmm4 = proj proj proj proj
            __ASM_EMIT("mulps       %%xmm4, %%xmm0")            // xmm0 = nx*proj ny*proj nz*proj nw*proj = vnx vny vnz vnw
            __ASM_EMIT("subps       %%xmm0, %%xmm3")            // xmm3 = dx1-vnx dy1-vny dz1-vnz dw1-vnw = vtx vty vtz vtw

            // Form the reflected and refracted ray parameters
            __ASM_EMIT("movaps      %%xmm2, %%xmm5")            // xmm5 = reflection refraction diffuse ?
            __ASM_EMIT("movaps      %%xmm0, %%xmm6")            // xmm6 = vnx vny vnz vnw
            __ASM_EMIT("movaps      %%xmm3, %%xmm7")            // xmm7 = vtx vty vtz vtw
            __ASM_EMIT("shufps      $0x00, %%xmm2, %%xmm2")     // xmm2 = reflection reflection reflection reflection
            __ASM_EMIT("shufps      $0x55, %%xmm5, %%xmm5")     // xmm5 = refraction refraction refraction refraction
            __ASM_EMIT("mulps       %%xmm2, %%xmm0")            // xmm0 = vnx*refl vny*refl vnz*refl vnw*refl
            __ASM_EMIT("mulps       %%xmm5, %%xmm6")            // xmm6 = vnx*refr vny*refr vnz*refr vnw*refr
            __ASM_EMIT("divps       %%xmm2, %%xmm3")            // xmm3 = vtx/refl vty/refl vtz/refl vtw/refl
            __ASM_EMIT("divps       %%xmm5, %%xmm7")            // xmm7 = vtx/refr vty/refr vtz/refr vtw/refr
            __ASM_EMIT("subps       %%xmm0, %%xmm3")            // xmm3 = vtx/refl-vnx*refl vty/refl-vny*refl vtz/refl-vnz*refl vtw/refl-vnw*refl
            __ASM_EMIT("addps       %%xmm6, %%xmm7")            // xmm7 = vtx/refr+vnx*refr vty/refr+vny*refr vtz/refr+vnz*refr vtw/refr+vnw*refr
            __ASM_EMIT("movups      %%xmm3, 0x10(%[rt])")
            __ASM_EMIT("movups      %%xmm7, 0x10(%[rf])")

            // Update energy parameters
            __ASM_EMIT("movups      0x00(%[rt]), %[x0]")        // xmm0 = zx1 zy1 zz1 zw1
            __ASM_EMIT("movups      0x00(%[ix]), %[x1]")        // xmm1 = ix iy iz iw
            __ASM_EMIT("subps       %[x1], %[x0]")              // xmm0 = zx1-ix zy1-iy zz1-iz zw1-iw = dvx dvy dvz dvw
            __ASM_EMIT("movups      %[x1], 0x00(%[rt])")
            __ASM_EMIT("movups      %[x1], 0x00(%[rf])")
            SCALAR_MUL("[x0]", "[x0]", "[x1]")                  // xmm0 = dvx*dvx + dvy*dvy + dvz*dvz = dist^2
            __ASM_EMIT("sqrtss      %[x0], %[x0]")              // xmm0 = sqrtf(dvx*dvx + dvy*dvy + dvz*dvz) = dist

            : [x0] "=&x" (x0), [x1] "=&x" (x1)
            : [rt] "r" (rt), [rf] "r" (rf), [ix] "r" (ix)
            : "memory"
        );

        x1      = expf(m.damping * x0);

        ARCH_X86_ASM
        (
            __ASM_EMIT("movss       0x20(%[rt]), %[x2]")        // xmm2 = amplitude
            __ASM_EMIT("mulss       0x08(%[m]), %[x1]")         // xmm1 = absorption * exp
            __ASM_EMIT("mulss       %[x2], %[x1]")              // xmm1 = amplitude * absorption * exp = ampl
            __ASM_EMIT("movaps      %[x1], %[x2]")              // xmm2 = ampl
            __ASM_EMIT("mulss       0x0c(%[m]), %[x1]")         // xmm1 = ampl * transparency
            __ASM_EMIT("divss       0x00(%[m]), %[x0]")         // xmm0 = dist / speed
            __ASM_EMIT("movaps      %[x1], %[x3]")              // xmm3 = ampl * transparency
            __ASM_EMIT("subss       %[x2], %[x3]")              // xmm2 = ampl * transparency - ampl

            // Store
            __ASM_EMIT("movss       %[x1], 0x20(%[rf])")
            __ASM_EMIT("movss       %[x0], 0x24(%[rf])")
            __ASM_EMIT("movss       %[x3], 0x20(%[rt])")
            __ASM_EMIT("movss       %[x0], 0x24(%[rt])")

            : [x0] "+&x" (x0), [x1] "+&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3)
            : [m] "r" (&m), [rt] "r" (rt), [rf] "r" (rf)
            : "memory"
        );

        n = sizeof(intersection3d_t)/sizeof(size_t);
        ARCH_X86_ASM
        (
            __ASM_EMIT("mov         %[ix], %[src]")
            __ASM_EMIT("lea         0x30(%[dst]), %[dst]")
            __ASM_EMIT32("rep;      movsd")
            __ASM_EMIT64("rep;      movsq")
            : [dst] "+D" (rt), [src] "+S" (p1), [n] "+c" (n)
            : [ix] "r" (ix)
            : "cc", "memory"
        );

        n = sizeof(intersection3d_t)/sizeof(size_t);
        ARCH_X86_ASM
        (
            __ASM_EMIT("lea         0x30(%[dst]), %[dst]")
            __ASM_EMIT32("rep;      movsd")
            __ASM_EMIT64("rep;      movsq")
            : [dst] "+D" (rf), [src] "+S" (ix), [n] "+c" (n)
            :
            : "cc", "memory"
        );
    }

    float calc_angle3d_v2(const vector3d_t *v1, const vector3d_t *v2)
    {
        float x0, x1, x2, x3, x4;

        ARCH_X86_ASM
        (
            /* Load vectors */
            __ASM_EMIT("movups      (%[v1]), %[x0]")        /* xmm0 = dx0 dy0 dz0 dw0 */
            __ASM_EMIT("movups      (%[v2]), %[x1]")        /* xmm1 = dx1 dy1 dz1 dw1 */
            CALC_COSINE2V("[x0]", "[x1]", "[x2]", "[x3]", "[x4]", "[M_ONE]", "[ONE]")
            : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3),
              [x4] "=&x" (x4)
            : [v1] "r" (v1), [v2] "r" (v2),
              [ONE] "m" (ONE),
              [M_ONE] "m" (X_MINUS_ONE)
            : "cc", "memory"
        );

        return x0;
    }

    float calc_angle3d_vv(const vector3d_t *v)
    {
        float x0, x1, x2, x3, x4;

        ARCH_X86_ASM
        (
            /* Load vectors */
            __ASM_EMIT("movups      0x00(%[v]), %[x0]")      /* xmm0 = dx0 dy0 dz0 dw0 */
            __ASM_EMIT("movups      0x10(%[v]), %[x1]")      /* xmm1 = dx1 dy1 dz1 dw1 */
            CALC_COSINE2V("[x0]", "[x1]", "[x2]", "[x3]", "[x4]", "[M_ONE]", "[ONE]")
            : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3),
              [x4] "=&x" (x4)
            : [v] "r" (v),
              [ONE] "m" (ONE),
              [M_ONE] "m" (X_MINUS_ONE)
            : "cc", "memory"
        );

        return x0;
    }

    inline void calc_normal3d_v2(vector3d_t *n, const vector3d_t *v1, const vector3d_t *v2)
    {
        float x0, x1, x2, x3;

        ARCH_X86_ASM
        (
            __ASM_EMIT("movups      (%[v1]), %[x0]")        /* xmm0 = dx1 dy1 dz1 dw1 */
            __ASM_EMIT("movups      (%[v2]), %[x1]")        /* xmm1 = dx2 dy2 dz2 dw2 */
            VECTOR_MUL("[x0]", "[x1]", "[x2]", "[x3]")      /* xmm0 = NZ NX NY NW */
            __ASM_EMIT("shufps      $0xc9, %[x0], %[x0]")   /* xmm0 = NX NY NZ NW */
            NORMALIZE("[x0]", "[x1]", "[x2]")               /* xmm0 = nx ny nz nw */
            __ASM_EMIT("movups      %[x0], (%[n])")

            : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3)
            : [n] "r" (n), [v1] "r" (v1), [v2] "r" (v2)
            : "cc" , "memory"
        );
    }

    inline void calc_normal3d_vv(vector3d_t *n, const vector3d_t *vv)
    {
        float x0, x1, x2, x3;

        ARCH_X86_ASM
        (
            __ASM_EMIT("movups      0x00(%[vv]), %[x0]")    /* xmm0 = dx1 dy1 dz1 dw1 */
            __ASM_EMIT("movups      0x10(%[vv]), %[x1]")    /* xmm1 = dx2 dy2 dz2 dw2 */
            VECTOR_MUL("[x0]", "[x1]", "[x2]", "[x3]")      /* xmm0 = NZ NX NY NW */
            __ASM_EMIT("shufps      $0xc9, %[x0], %[x0]")   /* xmm0 = NX NY NZ NW */
            NORMALIZE("[x0]", "[x1]", "[x2]")               /* xmm0 = nx ny nz nw */
            __ASM_EMIT("movups      %[x0], (%[n])")

            : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3)
            : [n] "r" (n), [vv] "r" (vv)
            : "cc" , "memory"
        );
    }

    void calc_normal3d_p3(vector3d_t *n, const point3d_t *p1, const point3d_t *p2, const point3d_t *p3)
    {
        float x0, x1, x2, x3;

        ARCH_X86_ASM
        (
            __ASM_EMIT("movups      (%[p1]), %[x2]")        /* xmm0 = x1 y1 z1 w1 */
            __ASM_EMIT("movups      (%[p2]), %[x0]")        /* xmm1 = x2 y2 z2 w2 */
            __ASM_EMIT("movups      (%[p3]), %[x1]")        /* xmm2 = x3 y3 z3 w3 */
            __ASM_EMIT("subps       %[x0], %[x1]")          /* xmm0 = dx1 dy1 dz1 dw1 */
            __ASM_EMIT("subps       %[x2], %[x0]")          /* xmm1 = dx2 dy2 dz2 dw2 */
            VECTOR_MUL("[x0]", "[x1]", "[x2]", "[x3]")      /* xmm0 = NZ NX NY NW */
            __ASM_EMIT("shufps      $0xc9, %[x0], %[x0]")   /* xmm0 = NX NY NZ NW */
            NORMALIZE("[x0]", "[x1]", "[x2]")               /* xmm0 = nx ny nz nw */
            __ASM_EMIT("movups      %[x0], (%[n])")

            : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3)
            : [n] "r" (n), [p1] "r" (p1), [p2] "r" (p2), [p3] "r" (p3)
            : "cc" , "memory"
        );
    }

    void calc_normal3d_pv(vector3d_t *n, const point3d_t *pv)
    {
        float x0, x1, x2, x3;

        ARCH_X86_ASM
        (
            __ASM_EMIT("movups      0x00(%[pv]), %[x2]")    /* xmm0 = x1 y1 z1 w1 */
            __ASM_EMIT("movups      0x10(%[pv]), %[x0]")    /* xmm1 = x2 y2 z2 w2 */
            __ASM_EMIT("movups      0x20(%[pv]), %[x1]")    /* xmm2 = x3 y3 z3 w3 */
            __ASM_EMIT("subps       %[x0], %[x1]")          /* xmm0 = dx1 dy1 dz1 dw1 */
            __ASM_EMIT("subps       %[x2], %[x0]")          /* xmm1 = dx2 dy2 dz2 dw2 */
            VECTOR_MUL("[x0]", "[x1]", "[x2]", "[x3]")      /* xmm0 = NZ NX NY NW */
            __ASM_EMIT("shufps      $0xc9, %[x0], %[x0]")   /* xmm0 = NX NY NZ NW */
            NORMALIZE("[x0]", "[x1]", "[x2]")               /* xmm0 = nx ny nz nw */
            __ASM_EMIT("movups      %[x0], (%[n])")

            : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3)
            : [n] "r" (n), [pv] "r" (pv)
            : "cc" , "memory"
        );
    }

    void move_point3d_p2(point3d_t *p, const point3d_t *p1, const point3d_t *p2, float k)
    {
        float x0, x1;

        ARCH_X86_ASM
        (
            __ASM_EMIT("movups      (%[p1]), %[x0]")        // xmm0 = x0 y0 z0 w0
            __ASM_EMIT("movups      (%[p2]), %[x1]")        // xmm1 = x1 y1 z1 w1
            __ASM_EMIT("shufps      $0x00, %[x2], %[x2]")   // xmm2 = k k k k
            __ASM_EMIT("subps       %[x0], %[x1]")          // xmm1 = dx dy dz dw
            __ASM_EMIT("mulps       %[x2], %[x1]")          // xmm1 = dx*k dy*k dz*k dw*k
            __ASM_EMIT("addps       %[x1], %[x0]")          // xmm0 = x0+dx*k y0+dx*k z0+dx*k w0+dw*k
            __ASM_EMIT("andps       %[zmask], %[x0]")       // xmm0 = x0+dx*k y0+dx*k z0+dx*k 0
            __ASM_EMIT("orps        %[omask], %[x0]")       // xmm0 = x0+dx*k y0+dx*k z0+dx*k 1
            __ASM_EMIT("movups      %[x0], (%[p])")
            : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "+x" (k)
            : [p] "r" (p), [p1] "r" (p1), [p2] "r" (p2),
              [zmask] "m" (X_MASK0111),
              [omask] "m" (X_3DPOINT)
            : "memory"
        );
    }

    void move_point3d_pv(point3d_t *p, const point3d_t *pv, float k)
    {
        float x0, x1;

        ARCH_X86_ASM
        (
            __ASM_EMIT("movups      0x00(%[pv]), %[x0]")    // xmm0 = x0 y0 z0 w0
            __ASM_EMIT("movups      0x10(%[pv]), %[x1]")    // xmm1 = x1 y1 z1 w1
            __ASM_EMIT("shufps      $0x00, %[x2], %[x2]")   // xmm2 = k k k k
            __ASM_EMIT("subps       %[x0], %[x1]")          // xmm1 = dx dy dz dw
            __ASM_EMIT("mulps       %[x2], %[x1]")          // xmm1 = dx*k dy*k dz*k dw*k
            __ASM_EMIT("addps       %[x1], %[x0]")          // xmm0 = x0+dx*k y0+dx*k z0+dx*k w0+dw*k
            __ASM_EMIT("andps       %[zmask], %[x0]")       // xmm0 = x0+dx*k y0+dx*k z0+dx*k 0
            __ASM_EMIT("orps        %[omask], %[x0]")       // xmm0 = x0+dx*k y0+dx*k z0+dx*k 1
            __ASM_EMIT("movups      %[x0], (%[p])")
            : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "+x" (k)
            : [p] "r" (p), [pv] "r" (pv),
              [zmask] "m" (X_MASK0111),
              [omask] "m" (X_3DPOINT)
            : "memory"
        );
    }

    bool check_octant3d_rv(const octant3d_t *o, const ray3d_t *r)
    {
        size_t index;
        bool result;
        float x0, x1, x2;

        ARCH_X86_ASM
        (
            __ASM_EMIT("movups      0x00(%[r]), %[x0]")     // xmm0 = zx zy zz zw
            __ASM_EMIT("movups      0x10(%[r]), %[x1]")     // xmm1 = rdx rdy rdz rdw
            __ASM_EMIT("movmskps    %[x1], %[index]")
            __ASM_EMIT("lea         0x10(,%[index], 8), %[index]")  // index = (index*8 + 16)
            __ASM_EMIT("movups      (%[o],%[index],2), %[x2]")  // xmm2 = bounds[index*2]
            __ASM_EMIT("subps       %[x0], %[x2]")          // xmm2 = dx dy dz dw
            __ASM_EMIT("mulps       %[x1], %[x2]")          // xmm2 = dx*rdz dy*rdy dz*rdz dw*rdw
            __ASM_EMIT("movmskps    %[x2], %[index]")
            __ASM_EMIT32("and       $0x07, %[index]")
            __ASM_EMIT64("and       $0x07, %b[index]")
            __ASM_EMIT("setz        %[result]")
            : [index] "=&r" (index), [result] "=r" (result),
              [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2)
            : [o] "r" (o), [r] "r" (r)
            : "cc"
        );

        return result;
    }
}

#undef SCALAR_MUL
#undef MAT3_TRANSPOSE
#undef MAT4_TRANSPOSE
#undef MATRIX_LOAD
#undef MATRIX_STORE
#undef NORMALIZE
#undef TRIPLET_CHECK
//        #undef CHECK_LOCATION
#undef SCALAR_MUL3
#undef VECTOR_MUL3
#undef VECTOR_MUL
#undef VECTOR_XCHG
#undef CALC_COSINE2V

#endif /* DSP_ARCH_X86_SSE_3DMATH_H_ */
