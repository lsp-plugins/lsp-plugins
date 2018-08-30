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

namespace native
{
    void start(dsp::context_t *ctx)
    {
        ctx->top        = 0;
    }

    void finish(dsp::context_t *ctx)
    {
        if (ctx->top != 0)
            lsp_warn("DSP context is not empty");
    }

    dsp::info_t *info()
    {
        size_t size     =
                sizeof(dsp::info_t) +
                strlen(ARCH_STRING) + 1 +
                strlen("native cpu") + 1 +
                strlen("unknown") + 1;

        dsp::info_t *res = reinterpret_cast<dsp::info_t *>(malloc(size));
        if (res == NULL)
            return res;

        char *text  = reinterpret_cast<char *>(&res[1]);
        res->arch       = text;
        text            = stpcpy(text, ARCH_STRING) + 1;
        res->cpu        = text;
        text            = stpcpy(text, "native cpu") + 1;
        res->model      = text;
        text            = stpcpy(text, "unknown");
        res->features   = text; // Empty string

        return res;
    }
}

#endif /* DSP_ARCH_NATIVE_CONTEXT_H_ */
