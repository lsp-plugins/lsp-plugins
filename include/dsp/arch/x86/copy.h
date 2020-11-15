/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 4 сент. 2018 г.
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

#ifndef DSP_ARCH_X86_COPY_H_
#define DSP_ARCH_X86_COPY_H_

namespace x86
{
    void copy(float *dst, const float *src, size_t count)
    {
        IF_ARCH_X86_64(size_t tmp);

        ARCH_X86_64_ASM
        (
            __ASM_EMIT("mov     %[count], %[tmp]")
            __ASM_EMIT("shr     $1, %[count]")
            __ASM_EMIT("jz      1f")
            __ASM_EMIT("rep     movsq")
            __ASM_EMIT("1:")
            __ASM_EMIT("test    $1, %[tmp]")
            __ASM_EMIT("jz      2f")
            __ASM_EMIT("movsd   ")
            __ASM_EMIT("2:")
            : [dst] "+D" (dst), [src] "+S" (src),
              [tmp] "=&r" (tmp), [count] "+c" (count)
            :
            : "cc", "memory"
        );

        ARCH_I386_ASM
        (
            __ASM_EMIT("rep     movsl")
            : [dst] "+D" (dst), [src] "+S" (src),
              [count] "+c" (count)
            :
            : "cc", "memory"
        );
    }
}

#endif /* DSP_ARCH_X86_COPY_H_ */
