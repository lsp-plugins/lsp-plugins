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

/* 1x scalar multiplication of 3 coordinates
 * Input:
 *   x0 = vector1 [dx dy dz ? ]
 *   x1 = vector2 [dx dy dz ? ]
 *   x2 = temporary
 *
 * Output:
 *   x0 = result (scalar)
 */
#define VECTOR_DPPS3(x0, x1, x2) \
    __ASM_EMIT("mulps       %" x1 ", %" x0)         /* xmm0 = dx0*dx1 dy0*dy1 dz0*dz1 ? */ \
    __ASM_EMIT("movhlps     %" x0 ", %" x2)         /* xmm2 = dz0*dz1 ? ? ? */ \
    __ASM_EMIT("addss       %" x0 ", %" x2)         /* xmm2 = dz0*dz1 + dx0*dx1 */ \
    __ASM_EMIT("shufps      $0x55, %" x0 ", %" x0)  /* xmm0 = dy0*dy1 dy0*dy1 dy0*dy1 dy0*dy1 */ \
    __ASM_EMIT("addss       %" x2 ", %" x0)         /* xmm0 = dz0*dz1 + dx0*dx1 + dy0*dy1 */ \

/* 1x scalar multiplication of 4 coordinates
 * Input:
 *   x0 = vector1 [dx dy dz dw ]
 *   x1 = vector2 [dx dy dz dw ]
 *   x2 = temporary
 *
 * Output:
 *   x0 = result (scalar)
 */
#define VECTOR_DPPS4(x0, x1, x2) \
    __ASM_EMIT("mulps       %" x1 ", %" x0)         /* xmm0 = dx0*dx1 dy0*dy1 dz0*dz1 dw0*dw1 = A B C D */ \
    __ASM_EMIT("movhlps     %" x0 ", %" x2)         /* xmm2 = C D  */ \
    __ASM_EMIT("addps       %" x2 ", %" x0)         /* xmm0 = A+C B+D */ \
    __ASM_EMIT("unpcklps    %" x0 ", %" x0)         /* xmm0 = A+C A+C B+D B+D */ \
    __ASM_EMIT("movhlps     %" x0 ", %" x2)         /* xmm2 = B+D B+D  */ \
    __ASM_EMIT("addps       %" x2 ", %" x0)         /* xmm0 = A+C+B+D  */

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

/* 1x vector multiplication
 * Input:
 *   x0 = vector1 [dx dy dz ? ]
 *   x1 = vector2 [dx dy dz ? ]
 *   x2 = temporary
 *   x3 = temporary
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
    __ASM_EMIT("jle         1000000f") \
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
    __ASM_EMIT("jle         1000000f") \
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

    void add_vector_pv1(point3d_t *p, const vector3d_t *dv)
    {
        float x0, x1;

        ARCH_X86_ASM
        (
            __ASM_EMIT("movups      (%[p]), %[x0]")
            __ASM_EMIT("movups      (%[dv]), %[x1]")
            __ASM_EMIT("addps       %[x1], %[x0]")
            __ASM_EMIT("movups      %[x0], (%[p])")
            : [x0] "=&x" (x0), [x1] "=&x" (x1)
            : [p] "r" (p), [dv] "r" (dv)
            : "memory"
        );
    }

    void add_vector_pv2(point3d_t *p, const point3d_t *sp, const vector3d_t *dv)
    {
        float x0, x1;

        ARCH_X86_ASM
        (
            __ASM_EMIT("movups      (%[sp]), %[x0]")
            __ASM_EMIT("movups      (%[dv]), %[x1]")
            __ASM_EMIT("addps       %[x1], %[x0]")
            __ASM_EMIT("movups      %[x0], (%[p])")
            : [x0] "=&x" (x0), [x1] "=&x" (x1)
            : [p] "r" (p), [sp] "r" (sp), [dv] "r" (dv)
            : "memory"
        );
    }

    void add_vector_pvk1(point3d_t *p, const vector3d_t *dv, float k)
    {
        float x0, x1;

        ARCH_X86_ASM
        (
            __ASM_EMIT("movups      (%[dv]), %[x1]")
            __ASM_EMIT("shufps      $0x00, %[x2], %[x2]")
            __ASM_EMIT("movups      (%[p]), %[x0]")
            __ASM_EMIT("mulps       %[x2], %[x1]")
            __ASM_EMIT("addps       %[x1], %[x0]")
            __ASM_EMIT("movups      %[x0], (%[p])")
            : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "+x" (k)
            : [p] "r" (p), [dv] "r" (dv)
            : "memory"
        );
    }

    void add_vector_pvk2(point3d_t *p, const point3d_t *sp, const vector3d_t *dv, float k)
    {
        float x0, x1;

        ARCH_X86_ASM
        (
            __ASM_EMIT("movups      (%[dv]), %[x1]")
            __ASM_EMIT("shufps      $0x00, %[x2], %[x2]")
            __ASM_EMIT("movups      (%[sp]), %[x0]")
            __ASM_EMIT("mulps       %[x2], %[x1]")
            __ASM_EMIT("addps       %[x1], %[x0]")
            __ASM_EMIT("movups      %[x0], (%[p])")
            : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "+x" (k)
            : [p] "r" (p), [sp] "r" (sp), [dv] "r" (dv)
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

    void normalize_vector2(vector3d_t *v, const vector3d_t *sv)
    {
        float x0, x1, x2;

        ARCH_X86_ASM
        (
            __ASM_EMIT("movups      (%[sv]), %[x0]")    // xmm0 = dx dy dz dw
            NORMALIZE("[x0]", "[x1]", "[x2]")
            __ASM_EMIT("movups      %[x0], (%[v])")

            : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2)
            : [v] "r" (v), [sv] "r" (sv)
            : "cc", "memory"
        );
    }

    void flip_vector_v1(vector3d_t *v)
    {
        ARCH_X86_ASM
        (
            __ASM_EMIT("movups      (%[v]), %%xmm0")    // xmm0 = dx dy dz dw
            __ASM_EMIT("xorps       %[mask], %%xmm0")
            __ASM_EMIT("movups      %%xmm0, (%[v])")
            :
            : [v] "r" (v),
              [mask] "m" (X_SMASK0111)
            : "memory",
              "xmm0"
        );
    }

    void flip_vector_v2(vector3d_t *v, const vector3d_t *sv)
    {
        ARCH_X86_ASM
        (
            __ASM_EMIT("movups      (%[sv]), %%xmm0")   // xmm0 = dx dy dz dw
            __ASM_EMIT("xorps       %[mask], %%xmm0")
            __ASM_EMIT("movups      %%xmm0, (%[v])")
            :
            : [v] "r" (v), [sv] "r" (sv),
              [mask] "m" (X_SMASK0111)
            : "memory",
              "xmm0"
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
                return;
            }
            else if (z == 0.0f)
            {
                if (y > 0.0f)
                    init_matrix3d_rotate_y(m, angle);
                else if (y < 0.0f)
                    init_matrix3d_rotate_y(m, -angle);
                return;
            }
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
//            __ASM_EMIT("shufps      $0xc9, %[x0], %[x0]")   /* xmm0 = NX NY NZ NW */
//            NORMALIZE("[x0]", "[x1]", "[x2]")               /* xmm0 = nx ny nz nw */
//            __ASM_EMIT("movups      %[x0], (%[n])")
            __ASM_EMIT("movaps      %[x0], %[x1]")          /* xmm1 = NZ NX NY ? */
            VECTOR_DPPS3("[x0]", "[x0]", "[x2]")            /* xmm0 = NX*NX + NY*NY + NZ*NZ = W2 */
            __ASM_EMIT("shufps      $0x09, %[x1], %[x1]")   /* xmm1 = NX NY NZ NZ */
            __ASM_EMIT("sqrtss      %[x0], %[x0]")          /* xmm0 = sqrtf(W2) = W */
            __ASM_EMIT("xorps       %[x3], %[x3]")          /* xmm3 = 0 */
            __ASM_EMIT("shufps      $0x00, %[x0], %[x0]")   /* xmm0 = W2 W2 W2 W2 */
            __ASM_EMIT("cmpps       $4, %[x0], %[x3]")      /* xmm3 = W != 0 */
            __ASM_EMIT("divps       %[x0], %[x1]")          /* xmm1 = NX/W NY/W NZ/W NZ/W */
            __ASM_EMIT("andps       %[x3], %[x1]")          /* xmm1 = (NX/W) & [W!=0] (NY/W) & [W!=0] (NZ/W) & [W!=0] (NZ/W) & [W!=0] = nx ny nz nz */
            __ASM_EMIT("andps       %[X_3DMASK], %[x1]")    /* xmm1 = nx ny nz 0 */
            __ASM_EMIT("movups      %[x1], (%[n])")

            : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3)
            : [n] "r" (n), [v1] "r" (v1), [v2] "r" (v2),
              [X_3DMASK] "m" (X_3DMASK)
            : "memory"
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
//            __ASM_EMIT("shufps      $0xc9, %[x0], %[x0]")   /* xmm0 = NX NY NZ NW */
//            NORMALIZE("[x0]", "[x1]", "[x2]")               /* xmm0 = nx ny nz nw */
//            __ASM_EMIT("movups      %[x0], (%[n])")
            __ASM_EMIT("movaps      %[x0], %[x1]")          /* xmm1 = NZ NX NY ? */
            VECTOR_DPPS3("[x0]", "[x0]", "[x2]")            /* xmm0 = NX*NX + NY*NY + NZ*NZ = W2 */
            __ASM_EMIT("shufps      $0x09, %[x1], %[x1]")   /* xmm1 = NX NY NZ NZ */
            __ASM_EMIT("sqrtss      %[x0], %[x0]")          /* xmm0 = sqrtf(W2) = W */
            __ASM_EMIT("xorps       %[x3], %[x3]")          /* xmm3 = 0 */
            __ASM_EMIT("shufps      $0x00, %[x0], %[x0]")   /* xmm0 = W2 W2 W2 W2 */
            __ASM_EMIT("cmpps       $4, %[x0], %[x3]")      /* xmm3 = W != 0 */
            __ASM_EMIT("divps       %[x0], %[x1]")          /* xmm1 = NX/W NY/W NZ/W NZ/W */
            __ASM_EMIT("andps       %[x3], %[x1]")          /* xmm1 = (NX/W) & [W!=0] (NY/W) & [W!=0] (NZ/W) & [W!=0] (NZ/W) & [W!=0] = nx ny nz nz */
            __ASM_EMIT("andps       %[X_3DMASK], %[x1]")    /* xmm1 = nx ny nz 0 */
            __ASM_EMIT("movups      %[x1], (%[n])")

            : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3)
            : [n] "r" (n), [vv] "r" (vv),
              [X_3DMASK] "m" (X_3DMASK)
            : "memory"
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
//            __ASM_EMIT("shufps      $0xc9, %[x0], %[x0]")   /* xmm0 = NX NY NZ NW */
//            NORMALIZE("[x0]", "[x1]", "[x2]")               /* xmm0 = nx ny nz nw */
//            __ASM_EMIT("movups      %[x0], (%[n])")
            __ASM_EMIT("movaps      %[x0], %[x1]")          /* xmm1 = NZ NX NY ? */
            VECTOR_DPPS3("[x0]", "[x0]", "[x2]")            /* xmm0 = NX*NX + NY*NY + NZ*NZ = W2 */
            __ASM_EMIT("shufps      $0x09, %[x1], %[x1]")   /* xmm1 = NX NY NZ NZ */
            __ASM_EMIT("sqrtss      %[x0], %[x0]")          /* xmm0 = sqrtf(W2) = W */
            __ASM_EMIT("xorps       %[x3], %[x3]")          /* xmm3 = 0 */
            __ASM_EMIT("shufps      $0x00, %[x0], %[x0]")   /* xmm0 = W2 W2 W2 W2 */
            __ASM_EMIT("cmpps       $4, %[x0], %[x3]")      /* xmm3 = W != 0 */
            __ASM_EMIT("divps       %[x0], %[x1]")          /* xmm1 = NX/W NY/W NZ/W NZ/W */
            __ASM_EMIT("andps       %[x3], %[x1]")          /* xmm1 = (NX/W) & [W!=0] (NY/W) & [W!=0] (NZ/W) & [W!=0] (NZ/W) & [W!=0] = nx ny nz nz */
            __ASM_EMIT("andps       %[X_3DMASK], %[x1]")    /* xmm1 = nx ny nz 0 */
            __ASM_EMIT("movups      %[x1], (%[n])")

            : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3)
            : [n] "r" (n), [p1] "r" (p1), [p2] "r" (p2), [p3] "r" (p3),
              [X_3DMASK] "m" (X_3DMASK)
            : "memory"
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
//            __ASM_EMIT("shufps      $0xc9, %[x0], %[x0]")   /* xmm0 = NX NY NZ NW */
//            NORMALIZE("[x0]", "[x1]", "[x2]")               /* xmm0 = nx ny nz nw */
//            __ASM_EMIT("movups      %[x0], (%[n])")
            __ASM_EMIT("movaps      %[x0], %[x1]")          /* xmm1 = NZ NX NY ? */
            VECTOR_DPPS3("[x0]", "[x0]", "[x2]")            /* xmm0 = NX*NX + NY*NY + NZ*NZ = W2 */
            __ASM_EMIT("shufps      $0x09, %[x1], %[x1]")   /* xmm1 = NX NY NZ NZ */
            __ASM_EMIT("sqrtss      %[x0], %[x0]")          /* xmm0 = sqrtf(W2) = W */
            __ASM_EMIT("xorps       %[x3], %[x3]")          /* xmm3 = 0 */
            __ASM_EMIT("shufps      $0x00, %[x0], %[x0]")   /* xmm0 = W2 W2 W2 W2 */
            __ASM_EMIT("cmpps       $4, %[x0], %[x3]")      /* xmm3 = W != 0 */
            __ASM_EMIT("divps       %[x0], %[x1]")          /* xmm1 = NX/W NY/W NZ/W NZ/W */
            __ASM_EMIT("andps       %[x3], %[x1]")          /* xmm1 = (NX/W) & [W!=0] (NY/W) & [W!=0] (NZ/W) & [W!=0] (NZ/W) & [W!=0] = nx ny nz nz */
            __ASM_EMIT("andps       %[X_3DMASK], %[x1]")    /* xmm1 = nx ny nz 0 */
            __ASM_EMIT("movups      %[x1], (%[n])")

            : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3)
            : [n] "r" (n), [pv] "r" (pv),
              [X_3DMASK] "m" (X_3DMASK)
            : "memory"
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

    float calc_area_p3(const point3d_t *p0, const point3d_t *p1, const point3d_t *p2)
    {
        float x0, x1, x2, x3;

        ARCH_X86_ASM
        (
            __ASM_EMIT("movups      (%[p0]), %[x2]")        /* xmm0 = x0 y0 z0 w0 */
            __ASM_EMIT("movups      (%[p1]), %[x0]")        /* xmm1 = x1 y1 z1 w1 */
            __ASM_EMIT("movups      (%[p2]), %[x1]")        /* xmm2 = x2 y2 z2 w2 */
            __ASM_EMIT("subps       %[x2], %[x0]")          /* xmm0 = p1 - p0 = dx1 dy1 dz1 dw1 */
            __ASM_EMIT("subps       %[x2], %[x1]")          /* xmm1 = p2 - p0 = dx2 dy2 dz2 dw2 */
            VECTOR_MUL("[x0]", "[x1]", "[x2]", "[x3]")      /* xmm0 = NZ NX NY NW */
            VECTOR_DPPS3("[x0]", "[x0]", "[x2]")            /* xmm0 = NX*NX + NY*NY + NZ*NZ */
            __ASM_EMIT("sqrtss      %[x0], %[x0]")          /* xmm0 = sqrtf(NX*NX + NY*NY + NZ*NZ) */

            : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3)
            : [p0] "r" (p0), [p1] "r" (p1), [p2] "r" (p2)
            : "memory"
        );

        return x0;
    }

    float calc_area_pv(const point3d_t *pv)
    {
        float x0, x1, x2, x3;

        ARCH_X86_ASM
        (
            __ASM_EMIT("movups      0x00(%[pv]), %[x2]")    /* xmm0 = x0 y0 z0 w0 */
            __ASM_EMIT("movups      0x10(%[pv]), %[x0]")    /* xmm1 = x1 y1 z1 w1 */
            __ASM_EMIT("movups      0x20(%[pv]), %[x1]")    /* xmm2 = x2 y2 z2 w2 */
            __ASM_EMIT("subps       %[x2], %[x0]")          /* xmm0 = p1 - p0 = dx1 dy1 dz1 dw1 */
            __ASM_EMIT("subps       %[x2], %[x1]")          /* xmm1 = p2 - p0 = dx2 dy2 dz2 dw2 */
            VECTOR_MUL("[x0]", "[x1]", "[x2]", "[x3]")      /* xmm0 = NZ NX NY NW */
            VECTOR_DPPS3("[x0]", "[x0]", "[x2]")            /* xmm0 = NX*NX + NY*NY + NZ*NZ */
            __ASM_EMIT("sqrtss      %[x0], %[x0]")          /* xmm0 = sqrtf(NX*NX + NY*NY + NZ*NZ) */

            : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3)
            : [pv] "r" (pv)
            : "memory"
        );

        return x0;
    }

    float calc_plane_p3(vector3d_t *v, const point3d_t *p0, const point3d_t *p1, const point3d_t *p2)
    {
        float x0, x1, x2, x3;

        ARCH_X86_ASM
        (
            __ASM_EMIT("movups      (%[p0]), %[x2]")        /* xmm2 = x0 y0 z0 w0 */
            __ASM_EMIT("movups      (%[p1]), %[x0]")        /* xmm0 = x1 y1 z1 w1 */
            __ASM_EMIT("movups      (%[p2]), %[x1]")        /* xmm1 = x2 y2 z2 w2 */
            __ASM_EMIT("subps       %[x2], %[x0]")          /* xmm0 = p1 - p0 = dx1 dy1 dz1 dw1 */
            __ASM_EMIT("subps       %[x2], %[x1]")          /* xmm1 = p2 - p0 = dx2 dy2 dz2 dw2 */
            VECTOR_MUL("[x0]", "[x1]", "[x2]", "[x3]")      /* xmm0 = NZ NX NY ? */
            __ASM_EMIT("movaps      %[x0], %[x1]")          /* xmm1 = NZ NX NY ? */
            VECTOR_DPPS3("[x0]", "[x0]", "[x2]")            /* xmm0 = NX*NX + NY*NY + NZ*NZ = W2 */
            __ASM_EMIT("shufps      $0x09, %[x1], %[x1]")   /* xmm1 = NX NY NZ NZ */
            __ASM_EMIT("shufps      $0x00, %[x0], %[x0]")   /* xmm0 = W2 W2 W2 W2 */
            __ASM_EMIT("xorps       %[x3], %[x3]")          /* xmm3 = 0 */
            __ASM_EMIT("sqrtps      %[x0], %[x0]")          /* xmm0 = sqrtf(W2) sqrtf(W2) sqrtf(W2) sqrtf(W2) = W W W W */
            __ASM_EMIT("cmpps       $4, %[x0], %[x3]")      /* xmm3 = W != 0 */
            __ASM_EMIT("divps       %[x0], %[x1]")          /* xmm1 = NX/W NY/W NZ/W NZ/W */
            __ASM_EMIT("andps       %[x3], %[x1]")          /* xmm1 = (NX/W) & [W!=0] (NY/W) & [W!=0] (NZ/W) & [W!=0] (NZ/W) & [W!=0] = nx ny nz nz */
            __ASM_EMIT("movaps      %[x1], %[x2]")          /* xmm2 = nx ny nz nz */
            __ASM_EMIT("movups      (%[p0]), %[x3]")        /* xmm3 = x0 y0 z0 w0 */
            __ASM_EMIT("xorps       %[X_ISIGN], %[x1]")     /* xmm1 = -nx -ny -nz -nz */
            VECTOR_DPPS3("[x1]", "[x3]", "[x3]")            /* xmm1 = -(nx*x0 + ny*y0 + nz*z0) = dw ? */
            __ASM_EMIT("shufps      $0xf0, %[x2], %[x1]")   /* xmm1 = dw dw nz nz */
            __ASM_EMIT("shufps      $0x24, %[x1], %[x2]")   /* xmm2 = nx ny nz dw */
            __ASM_EMIT("movups      %[x2], (%[v])")

            : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3)
            : [v] "r" (v), [p0] "r" (p0), [p1] "r" (p1), [p2] "r" (p2),
              [X_ISIGN] "m" (X_ISIGN)
            : "memory"
        );

        return x0;
    }

    float calc_plane_pv(vector3d_t *v, const point3d_t *pv)
    {
        float x0, x1, x2, x3;

        ARCH_X86_ASM
        (
            __ASM_EMIT("movups      0x00(%[pv]), %[x2]")    /* xmm2 = x0 y0 z0 w0 */
            __ASM_EMIT("movups      0x10(%[pv]), %[x0]")    /* xmm0 = x1 y1 z1 w1 */
            __ASM_EMIT("movups      0x20(%[pv]), %[x1]")    /* xmm1 = x2 y2 z2 w2 */
            __ASM_EMIT("subps       %[x2], %[x0]")          /* xmm0 = p1 - p0 = dx1 dy1 dz1 dw1 */
            __ASM_EMIT("subps       %[x2], %[x1]")          /* xmm1 = p2 - p0 = dx2 dy2 dz2 dw2 */
            VECTOR_MUL("[x0]", "[x1]", "[x2]", "[x3]")      /* xmm0 = NZ NX NY ? */
            __ASM_EMIT("movaps      %[x0], %[x1]")          /* xmm1 = NZ NX NY ? */
            VECTOR_DPPS3("[x0]", "[x0]", "[x2]")            /* xmm0 = NX*NX + NY*NY + NZ*NZ = W2 */
            __ASM_EMIT("shufps      $0x09, %[x1], %[x1]")   /* xmm1 = NX NY NZ NZ */
            __ASM_EMIT("shufps      $0x00, %[x0], %[x0]")   /* xmm0 = W2 W2 W2 W2 */
            __ASM_EMIT("xorps       %[x3], %[x3]")          /* xmm3 = 0 */
            __ASM_EMIT("sqrtps      %[x0], %[x0]")          /* xmm0 = sqrtf(W2) sqrtf(W2) sqrtf(W2) sqrtf(W2) = W W W W */
            __ASM_EMIT("cmpps       $4, %[x0], %[x3]")      /* xmm3 = W != 0 */
            __ASM_EMIT("divps       %[x0], %[x1]")          /* xmm1 = NX/W NY/W NZ/W NZ/W */
            __ASM_EMIT("andps       %[x3], %[x1]")          /* xmm1 = (NX/W) & [W!=0] (NY/W) & [W!=0] (NZ/W) & [W!=0] (NZ/W) & [W!=0] = nx ny nz nz */
            __ASM_EMIT("movaps      %[x1], %[x2]")          /* xmm2 = nx ny nz nz */
            __ASM_EMIT("movups      0x00(%[pv]), %[x3]")    /* xmm3 = x0 y0 z0 w0 */
            __ASM_EMIT("xorps       %[X_ISIGN], %[x1]")     /* xmm1 = -nx -ny -nz -nz */
            VECTOR_DPPS3("[x1]", "[x3]", "[x3]")            /* xmm1 = -(nx*x0 + ny*y0 + nz*z0) = dw ? */
            __ASM_EMIT("shufps      $0xf0, %[x2], %[x1]")   /* xmm1 = dw dw nz nz */
            __ASM_EMIT("shufps      $0x24, %[x1], %[x2]")   /* xmm2 = nx ny nz dw */
            __ASM_EMIT("movups      %[x2], (%[v])")

            : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3)
            : [v] "r" (v), [pv] "r" (pv),
              [X_ISIGN] "m" (X_ISIGN)
            : "memory"
        );

        return x0;
    }

    float calc_plane_v1p2(vector3d_t *v, const vector3d_t *v0, const point3d_t *p0, const point3d_t *p1)
    {
        float x0, x1, x2, x3;

        ARCH_X86_ASM
        (
            __ASM_EMIT("movups      (%[p0]), %[x2]")        /* xmm2 = x0 y0 z0 w0 */
            __ASM_EMIT("movups      (%[p1]), %[x0]")        /* xmm0 = x1 y1 z1 w1 */
            __ASM_EMIT("movups      (%[v0]), %[x1]")        /* xmm1 = v = dx2 dy2 dz2 dw2 */
            __ASM_EMIT("subps       %[x2], %[x0]")          /* xmm0 = p1 - p0 = dx1 dy1 dz1 dw1 */
            VECTOR_MUL("[x0]", "[x1]", "[x2]", "[x3]")      /* xmm0 = NZ NX NY ? */
            __ASM_EMIT("movaps      %[x0], %[x1]")          /* xmm1 = NZ NX NY ? */
            VECTOR_DPPS3("[x0]", "[x0]", "[x2]")            /* xmm0 = NX*NX + NY*NY + NZ*NZ = W2 */
            __ASM_EMIT("shufps      $0x09, %[x1], %[x1]")   /* xmm1 = NX NY NZ NZ */
            __ASM_EMIT("shufps      $0x00, %[x0], %[x0]")   /* xmm0 = W2 W2 W2 W2 */
            __ASM_EMIT("xorps       %[x3], %[x3]")          /* xmm3 = 0 */
            __ASM_EMIT("sqrtps      %[x0], %[x0]")          /* xmm0 = sqrtf(W2) sqrtf(W2) sqrtf(W2) sqrtf(W2) = W W W W */
            __ASM_EMIT("cmpps       $4, %[x0], %[x3]")      /* xmm3 = W != 0 */
            __ASM_EMIT("divps       %[x0], %[x1]")          /* xmm1 = NX/W NY/W NZ/W NZ/W */
            __ASM_EMIT("andps       %[x3], %[x1]")          /* xmm1 = (NX/W) & [W!=0] (NY/W) & [W!=0] (NZ/W) & [W!=0] (NZ/W) & [W!=0] = nx ny nz nz */
            __ASM_EMIT("movaps      %[x1], %[x2]")          /* xmm2 = nx ny nz nz */
            __ASM_EMIT("movups      (%[p0]), %[x3]")        /* xmm3 = x0 y0 z0 w0 */
            __ASM_EMIT("xorps       %[X_ISIGN], %[x1]")     /* xmm1 = -nx -ny -nz -nz */
            VECTOR_DPPS3("[x1]", "[x3]", "[x3]")            /* xmm1 = -(nx*x0 + ny*y0 + nz*z0) = dw ? */
            __ASM_EMIT("shufps      $0xf0, %[x2], %[x1]")   /* xmm1 = dw dw nz nz */
            __ASM_EMIT("shufps      $0x24, %[x1], %[x2]")   /* xmm2 = nx ny nz dw */
            __ASM_EMIT("movups      %[x2], (%[v])")

            : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3)
            : [v] "r" (v), [v0] "r" (v0), [p0] "r" (p0), [p1] "r" (p1),
              [X_ISIGN] "m" (X_ISIGN)
            : "memory"
        );

        return x0;
    }

    void calc_split_point_p2v1(point3d_t *sp, const point3d_t *l0, const point3d_t *l1, const vector3d_t *pl)
    {
        float x0, x1, x2, x3, x4, x5, x6;

        ARCH_X86_ASM
        (
            __ASM_EMIT("movups      (%[l0]), %[x1]")        /* xmm1 = l0 = lx0 ly0 lz0 ? */
            __ASM_EMIT("movups      (%[l1]), %[x0]")        /* xmm0 = l1 = lx1 ly1 lz1 ? */
            __ASM_EMIT("movups      (%[pl]), %[x2]")        /* xmm2 = pl = nx ny nz nw */
            __ASM_EMIT("subps       %[x1], %[x0]")          /* xmm0 = d = l1 - l0 = dx dy dz ? */
            __ASM_EMIT("movaps      %[x2], %[x3]")          /* xmm3 = pl */
            __ASM_EMIT("movaps      %[x2], %[x4]")          /* xmm4 = pl */
            __ASM_EMIT("mulps       %[x1], %[x2]")          /* xmm2 = l0 * pl = lx0*nx ly0*ny lz0*nz ? */
            __ASM_EMIT("mulps       %[x0], %[x3]")          /* xmm3 = d * pl = dx*nx dy*ny dz*nz ? */
            __ASM_EMIT("movhlps     %[x2], %[x5]")          /* xmm5 = lz0*nz ? */
            __ASM_EMIT("shufps      $0xff, %[x4], %[x4]")   /* xmm4 = nw nw nw nw */
            __ASM_EMIT("addss       %[x2], %[x5]")          /* xmm5 = lz0*nz + lx0*nx */
            __ASM_EMIT("movhlps     %[x3], %[x6]")          /* xmm6 = dz*nz ? */
            __ASM_EMIT("shufps      $0x55, %[x2], %[x2]")   /* xmm2 = ly0*ny ly0*ny ly0*ny ly0*ny */
            __ASM_EMIT("addss       %[x3], %[x6]")          /* xmm6 = dz*nz + dx*nx */
            __ASM_EMIT("addss       %[x5], %[x2]")          /* xmm2 = lx0*nx + ly0*ny + lz0*nz */
            __ASM_EMIT("shufps      $0x55, %[x3], %[x3]")   /* xmm3 = dy*ny dy*ny dy*ny dy*ny */
            __ASM_EMIT("addss       %[x4], %[x2]")          /* xmm2 = lx0*nx + ly0*ny + lz0*nz + nw */
            __ASM_EMIT("addss       %[x6], %[x3]")          /* xmm3 = dx*nx + dy*ny + dz*nz */
            __ASM_EMIT("divss       %[x3], %[x2]")          /* xmm2 = t = (lx0*nx + ly0*ny + lz0*nz + nw) / (dx*nx + dy*ny + dz*nz) */
            __ASM_EMIT("shufps      $0x00, %[x2], %[x2]")   /* xmm2 = t t t t */
            __ASM_EMIT("mulps       %[x2], %[x0]")          /* xmm0 = dx*t dy*t dz*t ? */
            __ASM_EMIT("subps       %[x0], %[x1]")          /* xmm1 = l0 - d*t */
            __ASM_EMIT("andps       %[X_3DMASK], %[x1]")
            __ASM_EMIT("orps        %[X_3DPOINT], %[x1]")
            __ASM_EMIT("movups      %[x1], (%[sp])")
            : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3),
              [x4] "=&x" (x4), [x5] "=&x" (x5), [x6] "=&x" (x6)
            : [sp] "r" (sp), [l0] "r" (l0), [l1] "r" (l1), [pl] "r" (pl),
              [X_3DMASK] "m" (X_3DMASK),
              [X_3DPOINT] "m" (X_3DPOINT)
            : "memory"
        );
    }

    void calc_split_point_pvv1(point3d_t *sp, const point3d_t *lv, const vector3d_t *pl)
    {
        float x0, x1, x2, x3, x4, x5, x6;

        ARCH_X86_ASM
        (
            __ASM_EMIT("movups      0x00(%[lv]), %[x1]")    /* xmm1 = l0 = lx0 ly0 lz0 ? */
            __ASM_EMIT("movups      0x10(%[lv]), %[x0]")    /* xmm0 = l1 = lx1 ly1 lz1 ? */
            __ASM_EMIT("movups      (%[pl]), %[x2]")        /* xmm2 = pl = nx ny nz nw */
            __ASM_EMIT("subps       %[x1], %[x0]")          /* xmm0 = d = l1 - l0 = dx dy dz ? */
            __ASM_EMIT("movaps      %[x2], %[x3]")          /* xmm3 = pl */
            __ASM_EMIT("movaps      %[x2], %[x4]")          /* xmm4 = pl */
            __ASM_EMIT("mulps       %[x1], %[x2]")          /* xmm2 = l0 * pl = lx0*nx ly0*ny lz0*nz ? */
            __ASM_EMIT("mulps       %[x0], %[x3]")          /* xmm3 = d * pl = dx*nx dy*ny dz*nz ? */
            __ASM_EMIT("movhlps     %[x2], %[x5]")          /* xmm5 = lz0*nz ? */
            __ASM_EMIT("shufps      $0xff, %[x4], %[x4]")   /* xmm4 = nw nw nw nw */
            __ASM_EMIT("addss       %[x2], %[x5]")          /* xmm5 = lz0*nz + lx0*nx */
            __ASM_EMIT("movhlps     %[x3], %[x6]")          /* xmm6 = dz*nz ? */
            __ASM_EMIT("shufps      $0x55, %[x2], %[x2]")   /* xmm2 = ly0*ny ly0*ny ly0*ny ly0*ny */
            __ASM_EMIT("addss       %[x3], %[x6]")          /* xmm6 = dz*nz + dx*nx */
            __ASM_EMIT("addss       %[x5], %[x2]")          /* xmm2 = lx0*nx + ly0*ny + lz0*nz */
            __ASM_EMIT("shufps      $0x55, %[x3], %[x3]")   /* xmm3 = dy*ny dy*ny dy*ny dy*ny */
            __ASM_EMIT("addss       %[x4], %[x2]")          /* xmm2 = lx0*nx + ly0*ny + lz0*nz + nw */
            __ASM_EMIT("addss       %[x6], %[x3]")          /* xmm3 = dx*nx + dy*ny + dz*nz */
            __ASM_EMIT("divss       %[x3], %[x2]")          /* xmm2 = t = (lx0*nx + ly0*ny + lz0*nz + nw) / (dx*nx + dy*ny + dz*nz) */
            __ASM_EMIT("shufps      $0x00, %[x2], %[x2]")   /* xmm2 = t t t t */
            __ASM_EMIT("mulps       %[x2], %[x0]")          /* xmm0 = dx*t dy*t dz*t ? */
            __ASM_EMIT("subps       %[x0], %[x1]")          /* xmm1 = l0 - d*t */
            __ASM_EMIT("andps       %[X_3DMASK], %[x1]")
            __ASM_EMIT("orps        %[X_3DPOINT], %[x1]")
            __ASM_EMIT("movups      %[x1], (%[sp])")
            : [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3),
              [x4] "=&x" (x4), [x5] "=&x" (x5), [x6] "=&x" (x6)
            : [sp] "r" (sp), [lv] "r" (lv), [pl] "r" (pl),
              [X_3DMASK] "m" (X_3DMASK),
              [X_3DPOINT] "m" (X_3DPOINT)
            : "memory"
        );
    }

    size_t colocation_x2_v1p2(const vector3d_t *pl, const point3d_t *p0, const point3d_t *p1)
    {
        float x0, x1, x2, x3;
        float res[4] __lsp_aligned16;

        ARCH_X86_ASM
        (
            __ASM_EMIT("movups      (%[pl]), %[x2]")        /* xmm3 = pl    */
            __ASM_EMIT("movups      (%[p0]), %[x0]")        /* xmm0 = p0    */
            __ASM_EMIT("movups      (%[p1]), %[x1]")        /* xmm1 = p1    */

            __ASM_EMIT("mulps       %[x2], %[x0]")          /* xmm0 = x0*px y0*py z0*pz w0*pw */
            __ASM_EMIT("mulps       %[x2], %[x1]")          /* xmm1 = x1*px y1*py z1*pz w1*pw */
            __ASM_EMIT("movaps      %[x0], %[x3]")          /* xmm3 = x0*px y0*py z0*pz w0*pw */
            __ASM_EMIT("movlhps     %[x1], %[x0]")          /* xmm0 = x0*px y0*py x1*px y1*py */
            __ASM_EMIT("movhlps     %[x3], %[x1]")          /* xmm1 = z0*pz w0*pw z1*pz w1*pw */
            __ASM_EMIT("addps       %[x1], %[x0]")          /* xmm0 = x0*px+z0*pz y0*py+w0*pw x1*px+z1*pz y1*py+w1*pw */
            __ASM_EMIT("shufps      $0xd8, %[x0], %[x0]")   /* xmm0 = x0*px+z0*pz x1*px+z1*pz y0*py+w0*pw y1*py+w1*pw */
            __ASM_EMIT("movhlps     %[x0], %[x1]")          /* xmm1 = y0*py+w0*pw y1*py+w1*pw ? ? */
            __ASM_EMIT("addps       %[x1], %[x0]")          /* xmm0 = x0*px+y0*py+z0*pz+w0*pw x1*px+y1*py+z1*pz+w1*pw ? ? = k0 k1 ? ? */
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
              [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3)
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
        float x0, x1, x2, x3;
        float res[4] __lsp_aligned16;
        size_t k0, k1;

        ARCH_X86_ASM
        (
            __ASM_EMIT("movups      (%[pl]), %[x2]")        /* xmm3 = pl    */
            __ASM_EMIT("movups      0x00(%[pv]), %[x0]")    /* xmm0 = p0    */
            __ASM_EMIT("movups      0x10(%[pv]), %[x1]")    /* xmm1 = p1    */

            __ASM_EMIT("mulps       %[x2], %[x0]")          /* xmm0 = x0*px y0*py z0*pz w0*pw */
            __ASM_EMIT("mulps       %[x2], %[x1]")          /* xmm1 = x1*px y1*py z1*pz w1*pw */
            __ASM_EMIT("movaps      %[x0], %[x3]")          /* xmm3 = x0*px y0*py z0*pz w0*pw */
            __ASM_EMIT("movlhps     %[x1], %[x0]")          /* xmm0 = x0*px y0*py x1*px y1*py */
            __ASM_EMIT("movhlps     %[x3], %[x1]")          /* xmm1 = z0*pz w0*pw z1*pz w1*pw */
            __ASM_EMIT("addps       %[x1], %[x0]")          /* xmm0 = x0*px+z0*pz y0*py+w0*pw x1*px+z1*pz y1*py+w1*pw */
            __ASM_EMIT("shufps      $0xd8, %[x0], %[x0]")   /* xmm0 = x0*px+z0*pz x1*px+z1*pz y0*py+w0*pw y1*py+w1*pw */
            __ASM_EMIT("movhlps     %[x0], %[x1]")          /* xmm1 = y0*py+w0*pw y1*py+w1*pw ? ? */
            __ASM_EMIT("addps       %[x1], %[x0]")          /* xmm0 = x0*px+y0*py+z0*pz+w0*pw x1*px+y1*py+z1*pz+w1*pw ? ? = k0 k1 ? ? */
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
              [x0] "=&x" (x0), [x1] "=&x" (x1), [x2] "=&x" (x2), [x3] "=&x" (x3)
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
            MAT4_TRANSPOSE("[x0]", "[x1]", "[x2]", "[x3]", "[x4]")
            /* xmm0 = x0 x1 x2 ? */
            /* xmm1 = y0 y1 y2 ? */
            /* xmm2 = z0 z1 z2 ? */
            /* xmm3 = w0 w1 w2 ? */
            __ASM_EMIT("addps       %[x1], %[x0]")
            __ASM_EMIT("addps       %[x3], %[x2]")
            __ASM_EMIT("addps       %[x2], %[x0]")          /* xmm0 = k0 k1 k2 ? */
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
            MAT4_TRANSPOSE("[x0]", "[x1]", "[x2]", "[x3]", "[x4]")
            /* xmm0 = x0 x1 x2 ? */
            /* xmm1 = y0 y1 y2 ? */
            /* xmm2 = z0 z1 z2 ? */
            /* xmm3 = w0 w1 w2 ? */
            __ASM_EMIT("addps       %[x1], %[x0]")
            __ASM_EMIT("addps       %[x3], %[x2]")
            __ASM_EMIT("addps       %[x2], %[x0]")          /* xmm0 = k0 k1 k2 ? */
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

            __ASM_EMIT("mulps       %[x3], %[x0]")          /* xmm0 = v0 * p */
            __ASM_EMIT("mulps       %[x3], %[x1]")          /* xmm1 = v1 * p */
            __ASM_EMIT("mulps       %[x3], %[x2]")          /* xmm2 = v2 * p */
            MAT4_TRANSPOSE("[x0]", "[x1]", "[x2]", "[x3]", "[x4]")
            /* xmm0 = x0 x1 x2 ? */
            /* xmm1 = y0 y1 y2 ? */
            /* xmm2 = z0 z1 z2 ? */
            /* xmm3 = w0 w1 w2 ? */
            __ASM_EMIT("addps       %[x1], %[x0]")
            __ASM_EMIT("addps       %[x3], %[x2]")
            __ASM_EMIT("addps       %[x2], %[x0]")          /* xmm0 = k0 k1 k2 ? */
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

            __ASM_EMIT("mulps       %[x3], %[x0]")          /* xmm0 = v0 * p */
            __ASM_EMIT("mulps       %[x3], %[x1]")          /* xmm1 = v1 * p */
            __ASM_EMIT("mulps       %[x3], %[x2]")          /* xmm2 = v2 * p */
            MAT4_TRANSPOSE("[x0]", "[x1]", "[x2]", "[x3]", "[x4]")
            /* xmm0 = x0 x1 x2 ? */
            /* xmm1 = y0 y1 y2 ? */
            /* xmm2 = z0 z1 z2 ? */
            /* xmm3 = w0 w1 w2 ? */
            __ASM_EMIT("addps       %[x1], %[x0]")
            __ASM_EMIT("addps       %[x3], %[x2]")
            __ASM_EMIT("addps       %[x2], %[x0]")          /* xmm0 = k0 k1 k2 ? */
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
            MAT4_TRANSPOSE("[x0]", "[x1]", "[x2]", "[x3]", "[x4]")
            /* xmm0 = x0 x1 x2 ? */
            /* xmm1 = y0 y1 y2 ? */
            /* xmm2 = z0 z1 z2 ? */
            /* xmm3 = w0 w1 w2 ? */
            __ASM_EMIT("addps       %[x1], %[x0]")
            __ASM_EMIT("addps       %[x3], %[x2]")
            __ASM_EMIT("addps       %[x2], %[x0]")          /* xmm0 = k0 k1 k2 ? */
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
            MAT4_TRANSPOSE("[x0]", "[x1]", "[x2]", "[x3]", "[x4]")
            /* xmm0 = x0 x1 x2 ? */
            /* xmm1 = y0 y1 y2 ? */
            /* xmm2 = z0 z1 z2 ? */
            /* xmm3 = w0 w1 w2 ? */
            __ASM_EMIT("addps       %[x1], %[x0]")
            __ASM_EMIT("addps       %[x3], %[x2]")
            __ASM_EMIT("addps       %[x2], %[x0]")          /* xmm0 = k0 k1 k2 ? */
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

            __ASM_EMIT("mulps       %[x3], %[x0]")          /* xmm0 = p0 * pl */
            __ASM_EMIT("mulps       %[x3], %[x1]")          /* xmm1 = p1 * pl */
            __ASM_EMIT("mulps       %[x3], %[x2]")          /* xmm2 = p2 * pl */
            MAT4_TRANSPOSE("[x0]", "[x1]", "[x2]", "[x3]", "[x4]")

            __ASM_EMIT("addps       %[x1], %[x0]")
            __ASM_EMIT("addps       %[x3], %[x2]")
            __ASM_EMIT("addps       %[x2], %[x0]")          /* xmm0 = k0 k1 k2 ? */
            __ASM_EMIT("movaps      %[x0], %[x1]")          /* xmm1 = k0 k1 k2 ? */
            __ASM_EMIT("movaps      %[x0], %[k]")           /* *k   = xmm0 */
            __ASM_EMIT("cmpps       $2, %[PTOL], %[x0]")    /* xmm0 = k0 <= +TOL k1 <= +TOL k2 <= +TOL ? */
            __ASM_EMIT("cmpps       $1, %[MTOL], %[x1]")    /* xmm1 = k0 < -TOL  k1 < -TOL k2 < -TOL ? */
            __ASM_EMIT("pand        %[ICULL3], %[x0]")      /* xmm0 = 1*[k0 <= +TOL] 1*[k1 <= +TOL] 1*[k2 <= +TOL] ? */
            __ASM_EMIT("pand        %[ICULL3], %[x1]")      /* xmm1 = 1*[k0 < -TOL] 1*[k1 < -TOL] 1*[k2 < -TOL] ? */
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
                __ASM_EMIT("movhlps     %[x2], %[x4]")                  /* xmm4 = dz*nz 0 ? ? */ \
                __ASM_EMIT("addps       %[x4], %[x2]")                  /* xmm2 = dx*nx+dz*nz dy*ny ? ? */ \
                __ASM_EMIT("unpcklps    %[x2], %[x2]")                  /* xmm2 = dx*nx+dz*nz dx*nx+dz*nz dy*ny dy*ny */ \
                __ASM_EMIT("mulps       %[x3], %[x1]")                  /* xmm1 = k*dx k*dy k*dz 0 */ \
                __ASM_EMIT("movhlps     %[x2], %[x4]")                  /* xmm4 = dy*ny dy*ny ? ? */ \
                __ASM_EMIT("addps       %[x4], %[x2]")                  /* xmm2 = dx*nx+dy*ny+dw*nw dx*nx+dy*ny+dw*nw ? ? = b b ? ? */ \
                __ASM_EMIT("unpcklps    %[x2], %[x2]")                  /* xmm2 = b b b b */ \
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
                __ASM_EMIT("movhlps     %[x4], %[x6]")                  /* xmm6 = dz1*nz 0 ? ? */ \
                __ASM_EMIT("movhlps     %[x5], %[x7]")                  /* xmm7 = dz2*nz 0 ? ? */ \
                __ASM_EMIT("addps       %[x6], %[x4]")                  /* xmm4 = dx1*nx+dz1*nz dy1*ny ? ? */ \
                __ASM_EMIT("addps       %[x7], %[x5]")                  /* xmm5 = dx1*nx+dz1*nz dy1*ny ? ? */ \
                __ASM_EMIT("unpcklps    %[x4], %[x4]")                  /* xmm4 = dx1*nx+dz1*nz dx1*nx+dz1*nz dy1*ny dy1*ny */ \
                __ASM_EMIT("unpcklps    %[x5], %[x5]")                  /* xmm5 = dx2*nx+dz2*nz dx2*nx+dz2*nz dy2*ny dy2*ny */ \
                __ASM_EMIT("movhlps     %[x4], %[x6]")                  /* xmm6 = dy1*ny dy1*ny ? ? */ \
                __ASM_EMIT("movhlps     %[x5], %[x7]")                  /* xmm7 = dy2*ny dy2*ny ? ? */ \
                __ASM_EMIT("addps       %[x6], %[x4]")                  /* xmm4 = dx1*nx+dy1*ny+dz1*nz dx1*nx+dy1*ny+dz1*nz ? ? = b1 b1 ? ? */ \
                __ASM_EMIT("addps       %[x7], %[x5]")                  /* xmm5 = dx2*nx+dy2*ny+dz2*nz dx2*nx+dy2*ny+dz2*nz ? ? = b2 b2 ? ? */ \
                __ASM_EMIT("unpcklps    %[x4], %[x4]")                  /* xmm4 = b1 b1 b1 b1 */ \
                __ASM_EMIT("unpcklps    %[x5], %[x5]")                  /* xmm5 = b2 b2 b2 b2 */ \
                __ASM_EMIT("movss       0x" koff " + %[k], %[x6]")      /* xmm6 = k */ \
                __ASM_EMIT("divps       %[x4], %[x2]")                  /* xmm2 = dx1/b1 dy1/b1 dz1/b1 0 */ \
                __ASM_EMIT("shufps      $0x00, %[x6], %[x6]")           /* xmm6 = k k k k */ \
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
                    __ASM_EMIT("movups     %[x1], 0x00(%[out])")
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

            __ASM_EMIT("mulps       %[x3], %[x0]")          /* xmm0 = p0 * pl */
            __ASM_EMIT("mulps       %[x3], %[x1]")          /* xmm1 = p1 * pl */
            __ASM_EMIT("mulps       %[x3], %[x2]")          /* xmm2 = p2 * pl */
            MAT4_TRANSPOSE("[x0]", "[x1]", "[x2]", "[x3]", "[x4]")

            __ASM_EMIT("addps       %[x1], %[x0]")
            __ASM_EMIT("addps       %[x3], %[x2]")
            __ASM_EMIT("addps       %[x2], %[x0]")          /* xmm0 = k0 k1 k2 ? */
            __ASM_EMIT("movaps      %[x0], %[x1]")          /* xmm1 = k0 k1 k2 ? */
            __ASM_EMIT("movaps      %[x0], %[k]")           /* *k   = xmm0 */
            __ASM_EMIT("cmpps       $2, %[PTOL], %[x0]")    /* xmm0 = k0 <= +TOL k1 <= +TOL k2 <= +TOL ? */
            __ASM_EMIT("cmpps       $1, %[MTOL], %[x1]")    /* xmm1 = k0 < -TOL  k1 < -TOL k2 < -TOL ? */
            __ASM_EMIT("pand        %[ICULL3], %[x0]")      /* xmm0 = 1*[k0 <= +TOL] 1*[k1 <= +TOL] 1*[k2 <= +TOL] ? */
            __ASM_EMIT("pand        %[ICULL3], %[x1]")      /* xmm1 = 1*[k0 < -TOL] 1*[k1 < -TOL] 1*[k2 < -TOL] ? */
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
                __ASM_EMIT("movhlps     %[x2], %[x4]")                  /* xmm4 = dz*nz 0 ? ? */ \
                __ASM_EMIT("addps       %[x4], %[x2]")                  /* xmm2 = dx*nx+dz*nz dy*ny ? ? */ \
                __ASM_EMIT("unpcklps    %[x2], %[x2]")                  /* xmm2 = dx*nx+dz*nz dx*nx+dz*nz dy*ny dy*ny */ \
                __ASM_EMIT("mulps       %[x3], %[x1]")                  /* xmm1 = k*dx k*dy k*dz 0 */ \
                __ASM_EMIT("movhlps     %[x2], %[x4]")                  /* xmm4 = dy*ny dy*ny ? ? */ \
                __ASM_EMIT("addps       %[x4], %[x2]")                  /* xmm2 = dx*nx+dy*ny+dw*nw dx*nx+dy*ny+dw*nw ? ? = b b ? ? */ \
                __ASM_EMIT("unpcklps    %[x2], %[x2]")                  /* xmm2 = b b b b */ \
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
                __ASM_EMIT("movhlps     %[x4], %[x6]")                  /* xmm6 = dz1*nz 0 ? ? */ \
                __ASM_EMIT("movhlps     %[x5], %[x7]")                  /* xmm7 = dz2*nz 0 ? ? */ \
                __ASM_EMIT("addps       %[x6], %[x4]")                  /* xmm4 = dx1*nx+dz1*nz dy1*ny ? ? */ \
                __ASM_EMIT("addps       %[x7], %[x5]")                  /* xmm5 = dx1*nx+dz1*nz dy1*ny ? ? */ \
                __ASM_EMIT("unpcklps    %[x4], %[x4]")                  /* xmm4 = dx1*nx+dz1*nz dx1*nx+dz1*nz dy1*ny dy1*ny */ \
                __ASM_EMIT("unpcklps    %[x5], %[x5]")                  /* xmm5 = dx2*nx+dz2*nz dx2*nx+dz2*nz dy2*ny dy2*ny */ \
                __ASM_EMIT("movhlps     %[x4], %[x6]")                  /* xmm6 = dy1*ny dy1*ny ? ? */ \
                __ASM_EMIT("movhlps     %[x5], %[x7]")                  /* xmm7 = dy2*ny dy2*ny ? ? */ \
                __ASM_EMIT("addps       %[x6], %[x4]")                  /* xmm4 = dx1*nx+dy1*ny+dz1*nz dx1*nx+dy1*ny+dz1*nz ? ? = b1 b1 ? ? */ \
                __ASM_EMIT("addps       %[x7], %[x5]")                  /* xmm5 = dx2*nx+dy2*ny+dz2*nz dx2*nx+dy2*ny+dz2*nz ? ? = b2 b2 ? ? */ \
                __ASM_EMIT("unpcklps    %[x4], %[x4]")                  /* xmm4 = b1 b1 b1 b1 */ \
                __ASM_EMIT("unpcklps    %[x5], %[x5]")                  /* xmm5 = b2 b2 b2 b2 */ \
                __ASM_EMIT("movss       0x" koff " + %[k], %[x6]")      /* xmm6 = k */ \
                __ASM_EMIT("divps       %[x4], %[x2]")                  /* xmm2 = dx1/b1 dy1/b1 dz1/b1 0 */ \
                __ASM_EMIT("shufps      $0x00, %[x6], %[x6]")           /* xmm6 = k k k k */ \
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
