/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 2 дек. 2019 г.
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

#ifndef DSP_COMMON_PMATH_POW_H_
#define DSP_COMMON_PMATH_POW_H_

#ifndef __DSP_DSP_DEFS
    #error "This header should not be included directly"
#endif /* __DSP_DSP_DEFS */

namespace dsp
{
    /**
     * Compute v[i] = c ^ v[i], the value to be raised should be non-negative
     * @param v power array
     * @param c value to be raised
     * @param count number of elements in array
     */
    extern void (* powcv1)(float *v, float c, size_t count);

    /**
     * Compute dst[i] = c ^ v[i], the value to be raised should be non-negative
     * @param dst output array
     * @param v power array
     * @param c value to be raised
     * @param count number of elements in array
     */
    extern void (* powcv2)(float *dst, const float *v, float c, size_t count);

    /**
     * Compute v[i] = v[i] ^ c, the value to be raised should be non-negative
     * @param v values to be raised
     * @param c power value
     * @param count number of elements in array
     */
    extern void (* powvc1)(float *c, float v, size_t count);

    /**
     * Compute dst[i] = v[i] ^ c, the value to be raised should be non-negative
     * @param dst output array
     * @param v values to be raised
     * @param c power value
     * @param count number of elements in array
     */
    extern void (* powvc2)(float *dst, const float *c, float v, size_t count);

    /**
     * Compute v[i] = v[i] ^ x[i], the value to be raised should be non-negative
     * @param v values to be raised
     * @param x power values
     * @param count number of elements in array
     */
    extern void (* powvx1)(float *v, const float *x, size_t count);

    /**
     * Compute dst[i] = v[i] ^ x[i], the value to be raised should be non-negative
     * @param dst output array
     * @param v values to be raised
     * @param x power values
     * @param count number of elements in array
     */
    extern void (* powvx2)(float *dst, const float *v, const float *x, size_t count);

}

#endif /* DSP_COMMON_PMATH_POW_H_ */
