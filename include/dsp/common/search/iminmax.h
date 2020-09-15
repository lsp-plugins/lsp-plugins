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

#ifndef DSP_COMMON_SEARCH_IMINMAX_H_
#define DSP_COMMON_SEARCH_IMINMAX_H_

#ifndef __DSP_DSP_DEFS
    #error "This header should not be included directly"
#endif /* __DSP_DSP_DEFS */

namespace dsp
{
    /** Calculate @ min { src }
     *
     * @param src source vector
     * @param count number of elements
     * @return minimum value index
     */
    extern size_t (* min_index)(const float *src, size_t count);

    /** Calculate @ max { src }
     *
     * @param src source vector
     * @param count number of elements
     * @return maximum value
     */
    extern size_t (* max_index)(const float *src, size_t count);

    /** Calculate @ minmax { src }
     *
     * @param src source vector
     * @param count number of elements
     * @param min pointer to store minimum value index
     * @param max pointer to store maximum value index
     */
    extern void (* minmax_index)(const float *src, size_t count, size_t *min, size_t *max);

    /** Calculate @ max { abs(src) }
     *
     * @param src source
     * @param count number of samples
     * @return index of maximum element
     */
    extern size_t  (* abs_max_index)(const float *src, size_t count);

    /** Calculate @ min { abs(src) }
     *
     * @param src source
     * @param count number of samples
     * @return index of maximum element
     */
    extern size_t  (* abs_min_index)(const float *src, size_t count);

    /** Calculate @ minmax { abs(src) }
     *
     * @param src source vector
     * @param count number of elements
     * @param min pointer to store absolute minimum value index
     * @param max pointer to store absolute maximum value index
     */
    extern void (* abs_minmax_index)(const float *src, size_t count, size_t *min, size_t *max);

}



#endif /* DSP_COMMON_SEARCH_IMINMAX_H_ */
