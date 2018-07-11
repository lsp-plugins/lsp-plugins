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
    #define MOVNTPS                     "movaps"

    namespace sse
    {
        /* Sine approximation coefficients */
        const float S0[] __lsp_aligned16 = {  1.00000000000000000000,  1.00000000000000000000,  1.00000000000000000000,  1.00000000000000000000  };
        const float S1[] __lsp_aligned16 = { -0.16666666666666665741, -0.16666666666666665741, -0.16666666666666665741, -0.16666666666666665741  };
        const float S2[] __lsp_aligned16 = {  0.00833333333333333322,  0.00833333333333333322,  0.00833333333333333322,  0.00833333333333333322  };
        const float S3[] __lsp_aligned16 = { -0.00019841269841269841, -0.00019841269841269841, -0.00019841269841269841, -0.00019841269841269841  };
        const float S4[] __lsp_aligned16 = {  0.00000275573192239859,  0.00000275573192239859,  0.00000275573192239859,  0.00000275573192239859  };
        const float S5[] __lsp_aligned16 = { -0.00000002505210838544, -0.00000002505210838544, -0.00000002505210838544, -0.00000002505210838544  };

        /* Cosine approximation coefficients */
        const float C0[] __lsp_aligned16 = {  1.00000000000000000000,  1.00000000000000000000,  1.00000000000000000000,  1.00000000000000000000  };
        const float C1[] __lsp_aligned16 = { -0.50000000000000000000, -0.50000000000000000000, -0.50000000000000000000, -0.50000000000000000000  };
        const float C2[] __lsp_aligned16 = {  0.04166666666666666435,  0.04166666666666666435,  0.04166666666666666435,  0.04166666666666666435  };
        const float C3[] __lsp_aligned16 = { -0.00138888888888888894, -0.00138888888888888894, -0.00138888888888888894, -0.00138888888888888894  };
        const float C4[] __lsp_aligned16 = {  0.00002480158730158730,  0.00002480158730158730,  0.00002480158730158730,  0.00002480158730158730  };
        const float C5[] __lsp_aligned16 = { -0.00000027557319223986, -0.00000027557319223986, -0.00000027557319223986, -0.00000027557319223986  };

        const float PI[] __lsp_aligned16 = { M_PI, M_PI, M_PI, M_PI  };
        const float PI_2[] __lsp_aligned16 = { M_PI_2, M_PI_2, M_PI_2, M_PI_2  };

        const uint32_t X_SIGN[] __lsp_aligned16     = { 0x7fffffff, 0x7fffffff, 0x7fffffff, 0x7fffffff };
    }

    namespace sse
    {
        static inline bool __lsp_forced_inline sse_aligned(const void *ptr)         { return !(ptrdiff_t(ptr) & (SSE_ALIGN - 1));  };
        static inline ptrdiff_t __lsp_forced_inline sse_offset(const void *ptr)     { return (ptrdiff_t(ptr) & (SSE_ALIGN - 1));   };
        static inline size_t __lsp_forced_inline sse_multiple(size_t count)         { return count & (SSE_MULTIPLE - 1);           };
    }
}

#endif /* CORE_X86_DSP_CONST_H_ */
