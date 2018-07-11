/*
 * const.h
 *
 *  Created on: 05 окт. 2015 г.
 *      Author: sadko
 */

#ifndef CORE_X86_DSP_CONST_H_
#define CORE_X86_DSP_CONST_H_

namespace lsp
{
    // Parameters for SSE
    #define SSE_MULTIPLE                4
    #define SSE_ALIGN                   (SSE_MULTIPLE * sizeof(float))
    #define SFENCE                      __asm__ __volatile__ ( __ASM_EMIT("sfence") )
    #define EMMS                        __asm__ __volatile__ ( __ASM_EMIT("emms") )
    #define MOVNTPS                     "movaps"
    #define SSE_SVEC4(name, value)      const float name[] __lsp_aligned16      = { value, value, value, value }
    #define SSE_UVEC4(name, value)      const uint32_t name[] __lsp_aligned16   = { value, value, value, value }

    namespace sse
    {
        /* Sine approximation coefficients */
        SSE_SVEC4(S0,  1.00000000000000000000);
        SSE_SVEC4(S1, -0.16666666666666665741);
        SSE_SVEC4(S2,  0.00833333333333333322);
        SSE_SVEC4(S3, -0.00019841269841269841);
        SSE_SVEC4(S4,  0.00000275573192239859);
        SSE_SVEC4(S5, -0.00000002505210838544);

        /* Cosine approximation coefficients */
        SSE_SVEC4(C0,  1.00000000000000000000);
        SSE_SVEC4(C1, -0.50000000000000000000);
        SSE_SVEC4(C2,  0.04166666666666666435);
        SSE_SVEC4(C3, -0.00138888888888888894);
        SSE_SVEC4(C4,  0.00002480158730158730);
        SSE_SVEC4(C5, -0.00000027557319223986);

        /* Logarithm approximation coefficients */
        SSE_SVEC4(L0, 7.0376836292E-2);
        SSE_SVEC4(L1, -1.1514610310E-1);
        SSE_SVEC4(L2, 1.1676998740E-1);
        SSE_SVEC4(L3, -1.2420140846E-1);
        SSE_SVEC4(L4, +1.4249322787E-1);
        SSE_SVEC4(L5, -1.6668057665E-1);
        SSE_SVEC4(L6, +2.0000714765E-1);
        SSE_SVEC4(L7, -2.4999993993E-1);
        SSE_SVEC4(L8, +3.3333331174E-1);
        SSE_SVEC4(L9, 0.5);
        SSE_SVEC4(LXE, -2.12194440e-4);

        /* Math constants */
        SSE_SVEC4(ONE, 1.0f);
        SSE_SVEC4(PI,  M_PI);
        SSE_SVEC4(PI_2, M_PI_2);
        SSE_SVEC4(SQRT1_2, M_SQRT1_2);
        SSE_SVEC4(LN2, M_LN2);

        /* Sign Mask */
        SSE_UVEC4(X_SIGN,  0x7fffffff);
        SSE_SVEC4(X_HALF,  0.5f);
        SSE_UVEC4(X_MANT,  0x007fffff);
        SSE_UVEC4(X_MMASK, 0x0000007f);
        SSE_SVEC4(X_AMP_THRESH, AMPLIFICATION_THRESH);
        SSE_UVEC4(X_P_DENORM, 0x00800000);
        SSE_UVEC4(X_N_DENORM, 0x80800000);

        /* Positive and negative infinities */
        SSE_UVEC4(X_P_INF,  0x7f800000);
        SSE_UVEC4(X_N_INF,  0xff800000);
        SSE_UVEC4(X_P_INFM1,0x7f7fffff);
        SSE_UVEC4(X_N_INFM1,0xff7fffff);
        SSE_UVEC4(X_ZERO_M1,0xffffffff);

        /* Saturation replacement */
        SSE_SVEC4(SX_P_INF, FLOAT_SAT_P_INF);
        SSE_SVEC4(SX_N_INF, FLOAT_SAT_N_INF);
        SSE_SVEC4(SX_P_NAN, FLOAT_SAT_P_NAN);
        SSE_SVEC4(SX_N_NAN, FLOAT_SAT_N_NAN);
    }

    namespace sse
    {
        static inline bool __lsp_forced_inline sse_aligned(const void *ptr)         { return !(ptrdiff_t(ptr) & (SSE_ALIGN - 1));  };
        static inline ptrdiff_t __lsp_forced_inline sse_offset(const void *ptr)     { return (ptrdiff_t(ptr) & (SSE_ALIGN - 1));   };
        static inline size_t __lsp_forced_inline sse_multiple(size_t count)         { return count & (SSE_MULTIPLE - 1);           };
    }
}

#endif /* CORE_X86_DSP_CONST_H_ */
