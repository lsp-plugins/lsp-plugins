/*
 * const.h
 *
 *  Created on: 05 окт. 2015 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_DSP_CONST_H_
#define DSP_ARCH_X86_DSP_CONST_H_

#ifndef DSP_ARCH_X86_SSE_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_SSE_IMPL */

// Parameters for SSE
#define SSE_MULTIPLE                4
#define SSE_ALIGN                   (SSE_MULTIPLE * sizeof(float))

static inline bool __lsp_forced_inline sse_aligned(const void *ptr)         { return !(ptrdiff_t(ptr) & (SSE_ALIGN - 1));  };
static inline ptrdiff_t __lsp_forced_inline sse_offset(const void *ptr)     { return (ptrdiff_t(ptr) & (SSE_ALIGN - 1));   };
static inline size_t __lsp_forced_inline sse_multiple(size_t count)         { return count & (SSE_MULTIPLE - 1);           };

#endif /* DSP_ARCH_X86_DSP_CONST_H_ */
