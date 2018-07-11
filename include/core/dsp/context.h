/*
 * context.h
 *
 *  Created on: 9 апр. 2017 г.
 *      Author: sadko
 */

#ifndef CORE_DSP_CONTEXT_H_
#define CORE_DSP_CONTEXT_H_

#ifndef __CORE_DSP_DEFS
    #error "This header should not be included directly"
#endif /* __CORE_DSP_DEFS */

#define DSP_OPTION_CPU_UNKNOWN          0UL
#define DSP_OPTION_CPU_INTEL            1UL
#define DSP_OPTION_CPU_AMD              2UL
#define DSP_OPTION_CPU_MASK             0x3UL
#define DSP_OPTION_FPU                  (1UL << 2)
#define DSP_OPTION_CMOV                 (1UL << 3)
#define DSP_OPTION_MMX                  (1UL << 4)
#define DSP_OPTION_SSE                  (1UL << 5)
#define DSP_OPTION_SSE2                 (1UL << 6)
#define DSP_OPTION_SSE3                 (1UL << 7)
#define DSP_OPTION_SSSE3                (1UL << 8)
#define DSP_OPTION_SSE4_1               (1UL << 9)
#define DSP_OPTION_SSE4_2               (1UL << 10)
#define DSP_OPTION_SSE4A                (1UL << 11)
#define DSP_OPTION_FMA3                 (1UL << 12)
#define DSP_OPTION_FMA4                 (1UL << 13)
#define DSP_OPTION_AVX                  (1UL << 14)
#define DSP_OPTION_AVX2                 (1UL << 15)

namespace lsp
{
    //-----------------------------------------------------------------------
    // DSP context parameters
#pragma pack(push, 1)
    typedef struct dsp_context_t
    {
        uint32_t        top;
        uint32_t        data[15];
    } dsp_context_t;
#pragma pack(pop)

    // Namespace containing function
    namespace dsp
    {
        // Start and finish types
        typedef void (* start_t)(dsp_context_t *ctx);
        typedef void (* finish_t)(dsp_context_t *ctx);

        /** Initialize DSP
         *
         */
        void init();

        /** Start DSP processing, save machine context
         *
         * @param ctx structure to save context
         */
        extern void (* start)(dsp_context_t *ctx);

        /** Finish DSP processing, restore machine context
         *
         * @param ctx structure to restore context
         */
        extern void (* finish)(dsp_context_t *ctx);
    }
}

#endif /* CORE_DSP_CONTEXT_H_ */
