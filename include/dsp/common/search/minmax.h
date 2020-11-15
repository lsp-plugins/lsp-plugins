/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 4 дек. 2019 г.
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

#ifndef DSP_COMMON_SEARCH_MINMAX_H_
#define DSP_COMMON_SEARCH_MINMAX_H_

#ifndef __DSP_DSP_DEFS
    #error "This header should not be included directly"
#endif /* __DSP_DSP_DEFS */

namespace dsp
{
    /** Calculate min { src }
     *
     * @param src source vector
     * @param count number of elements
     * @return minimum value
     */
    extern float (* min)(const float *src, size_t count);

    /** Calculate max { src }
     *
     * @param src source vector
     * @param count number of elements
     * @return maximum value
     */
    extern float (* max)(const float *src, size_t count);

    /** Get absolute maximum: result = max { abs(src[i]) }
     *
     * @param src source array
     * @param count number of elements
     * @return result
     */
    extern float (* abs_max)(const float *src, size_t count);

    /** Get absolute minimum: result = min { abs(src[i]) }
     *
     * @param src source array
     * @param count number of elements
     * @return result
     */
    extern float (* abs_min)(const float *src, size_t count);

    /** Calculate min { src }, max { src }
     *
     * @param src source vector
     * @param count number of elements
     * @return maximum value
     */
    extern void (* minmax)(const float *src, size_t count, float *min, float *max);

    /** Calculate min { abs(src) }, max { abs(src) }
     *
     * @param src source vector
     * @param count number of elements
     * @return maximum value
     */
    extern void (* abs_minmax)(const float *src, size_t count, float *min, float *max);
}

#endif /* DSP_COMMON_SEARCH_MINMAX_H_ */
