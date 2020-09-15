/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 05 авг. 2016 г.
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

#ifndef DSP_ARCH_X86_FLOAT_H_
#define DSP_ARCH_X86_FLOAT_H_

#include <core/types.h>

namespace x86
{
    static uint32_t fpu_read_cr()
    {
        uint16_t cr = 0;

        ARCH_X86_ASM
        (
            __ASM_EMIT("fstcw   %[cr]")

            : [cr] "+m" (cr)
            :
            : "memory"
        );

        return cr;
    }

    static void fpu_write_cr(uint32_t value)
    {
        uint16_t cr     = value;

        ARCH_X86_ASM
        (
            __ASM_EMIT("fldcw   %[cr]")

            :
            : [cr] "m" (cr)
            :
        );
    }
}

#endif /* DSP_ARCH_X86_FLOAT_H_ */
