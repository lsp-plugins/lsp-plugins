/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 10 авг. 2018 г.
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

#ifndef DSP_COMMON_SMATH_H_
#define DSP_COMMON_SMATH_H_

#ifndef __DSP_DSP_DEFS
    #error "This header should not be included directly"
#endif /* __DSP_DSP_DEFS */

//-----------------------------------------------------------------------
// DSP scalar mathematical functions
namespace dsp
{
    /** Power of floating-point value by integer constant
     *
     * @param x value to power
     * @param deg the power degree
     * @return result of x^deg calculation
     */
    extern float (* ipowf)(float x, int deg);

    /** Calculate the integer root of value
     *
     * @param x the value to calculate
     * @param deg the root degree, should be positive
     * @return the deg'th root of x
     */
    extern float (* irootf)(float x, int deg);
}

#endif /* DSP_COMMON_SMATH_H_ */
