/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 05 окт. 2015 г.
 *
 * lsp-plugins is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-plugins is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-plugins. If not, see <https://www.gnu.org/licenses/>.
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
