/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 29 нояб. 2019 г.
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

#ifndef INCLUDE_DSP_COMMON_HMATH_HSUM_H_
#define INCLUDE_DSP_COMMON_HMATH_HSUM_H_

namespace dsp
{
    /** Calculate horizontal sum: result = sum (i) from 0 to count-1 src[i]
     *
     * @param src vector to summarize
     * @param count number of elements
     * @return status of operation
     */
    extern float (* h_sum)(const float *src, size_t count);

    /** Calculate horizontal sum: result = sum (i) from 0 to count-1 sqr(src[i])
     *
     * @param src vector to summarize
     * @param count number of elements
     * @return status of operation
     */
    extern float (* h_sqr_sum)(const float *src, size_t count);

    /** Calculate horizontal sum of absolute values: result = sum (i) from 0 to count-1 abs(src[i])
     *
     * @param src vector to summarize
     * @param count number of elements
     * @return status of operation
     */
    extern float (* h_abs_sum)(const float *src, size_t count);

}

#endif /* INCLUDE_DSP_COMMON_HMATH_HSUM_H_ */
