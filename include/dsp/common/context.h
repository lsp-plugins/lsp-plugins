/*
 * context.h
 *
 *  Created on: 9 апр. 2017 г.
 *      Author: sadko
 */

#ifndef DSP_COMMON_CONTEXT_H_
#define DSP_COMMON_CONTEXT_H_

#ifndef __DSP_DSP_DEFS
    #error "This header should not be included directly"
#endif /* __DSP_DSP_DEFS */

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

#endif /* DSP_COMMON_CONTEXT_H_ */
