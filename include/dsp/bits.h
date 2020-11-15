/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 06 февр. 2016 г.
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

#ifndef DSP_BITS_H_
#define DSP_BITS_H_

#include <common/types.h>

// Declare array only for specific architectures
#if (!defined(ARCH_ARM)) && (!defined(ARCH_AARCH64))
    extern const uint8_t    __rb[];
#endif

// Include CPU-optimized code for bit reversal
#if defined(ARCH_X86)
    #include <dsp/arch/x86/bits.h>
#elif defined(ARCH_AARCH64)
    #include <dsp/arch/aarch64/bits.h>
#elif defined(ARCH_ARM)
    #include <dsp/arch/arm/bits.h>
#else
    #include <dsp/arch/native/bits.h>
#endif

#endif /* DSP_BITS_H_ */
