/*
 * context.h
 *
 *  Created on: 12 авг. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_NATIVE_CONTEXT_H_
#define DSP_ARCH_NATIVE_CONTEXT_H_

#ifndef __DSP_NATIVE_IMPL
    #error "This header should not be included directly"
#endif /* __DSP_NATIVE_IMPL */

void start(dsp::context_t *ctx)
{
    ctx->top        = 0;
}

void finish(dsp::context_t *ctx)
{
    if (ctx->top != 0)
        lsp_warn("DSP context is not empty");
}


#endif /* DSP_ARCH_NATIVE_CONTEXT_H_ */
