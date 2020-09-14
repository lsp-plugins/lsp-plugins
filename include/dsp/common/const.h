/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 13 авг. 2018 г.
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

#ifndef DSP_COMMON_CONST_H_
#define DSP_COMMON_CONST_H_

#ifndef __DSP_DSP_DEFS
    #error "This header should not be included directly"
#endif /* __DSP_DSP_DEFS */

#define DSP_F32VEC4(name, v)            extern const float name[]
#define DSP_U32VEC4(name, v)            extern const uint32_t name[]

#define DSP_F32VECX4(name, a, b, c, d)  extern const float name[]
#define DSP_U32VECX4(name, a, b, c, d)  extern const uint32_t name[]

#define DSP_F32REP4(v)
#define DSP_U32REP4(v)

#define DSP_F32ARRAY(name, ...)         extern const float name[]

namespace dsp
{
    #include <dsp/common/const/const16.h>
}

#undef DSP_F32ARRAY

#undef DSP_U32REP4
#undef DSP_F32REP4

#undef DSP_U32VECX4
#undef DSP_F32VECX4

#undef DSP_U32VEC4
#undef DSP_F32VEC4

#endif /* DSP_COMMON_CONST_H_ */
