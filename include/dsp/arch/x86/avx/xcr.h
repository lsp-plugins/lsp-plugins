/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 24 февр. 2018 г.
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

#ifndef DSP_ARCH_X86_AVX_XCR_H_
#define DSP_ARCH_X86_AVX_XCR_H_

#ifndef DSP_ARCH_X86_AVX_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_AVX_IMPL */

namespace x86
{
    #ifdef ARCH_X86_AVX
        uint64_t read_xcr(umword_t xcr_id)
        {
            uint64_t xcr;

            ARCH_X86_ASM
            (
                __ASM_EMIT64("xor       %%rax, %%rax")
                __ASM_EMIT("xgetbv")
                __ASM_EMIT64("shl       $32, %%rdx")
                __ASM_EMIT64("or        %%rdx, %%rax")
                : __IF_32("=A" (xcr)) __IF_64("=a" (xcr))
                : "c" (xcr_id)
                : __IF_64("%rdx")
            );
            return xcr;
        }
    #else
        uint64_t read_xcr(umword_t xcr_id)
        {
            return 0;
        }
    #endif /* ARCH_X86_AVX */
}

#endif /* DSP_ARCH_X86_AVX_XCR_H_ */
