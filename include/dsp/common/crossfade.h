/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 6 дек. 2020 г.
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

#ifndef DSP_COMMON_CROSSFADE_H_
#define DSP_COMMON_CROSSFADE_H_

namespace dsp
{
    /**
     * Perform linear crossfade between two signals and store result to the destination buffer:
     *
     * dst[i]     = dst[i] * interp1[i] + src[i] * interp2[i]
     * interp1[i] = y0 + ((x[i] - x0) * (y1 - y0)) / (x1 - x0)
     * interp2[i] = y1 - ((x[i] - x0) * (y1 - y0)) / (x1 - x0)
     *
     * @param dst destination buffer
     * @param source buffer
     * @param x0 the x coordinate of first initial point
     * @param y0 the y coordinate of first initial point
     * @param x1 the x coordinate of second initial point
     * @param y1 the y coordinate of second initial point
     * @param x the initial coordinate of X point
     * @param n number of x'es to process
     */
    extern void (* lin_xfade2)(
                float *dst, const float *src,
                int32_t x0, float y0, int32_t x1, float y1,
                int32_t x, uint32_t n
            );

    /**
     * Perform linear crossfade between two signals and store result to the destination buffer:
     *
     * dst[i]     = a[i] * interp1[i] + b[i] * interp2[i]
     * interp1[i] = y0 + ((x[i] - x0) * (y1 - y0)) / (x1 - x0)
     * interp2[i] = y1 - ((x[i] - x0) * (y1 - y0)) / (x1 - x0)
     *
     * @param dst destination buffer
     * @param source buffer
     * @param x0 the x coordinate of first initial point
     * @param y0 the y coordinate of first initial point
     * @param x1 the x coordinate of second initial point
     * @param y1 the y coordinate of second initial point
     * @param x the initial coordinate of X point
     * @param n number of x'es to process
     */
    extern void (* lin_xfade3)(
                float *dst, const float *a, const float *b,
                int32_t x0, float y0, int32_t x1, float y1,
                int32_t x, uint32_t n
            );

    /**
     * Perform linear crossfade between two signals and add result to the destination buffer:
     *
     * dst[i]     = dst[i] + a[i] * interp1[i] + b[i] * interp2[i]
     * interp1[i] = y0 + ((x[i] - x0) * (y1 - y0)) / (x1 - x0)
     * interp2[i] = y1 - ((x[i] - x0) * (y1 - y0)) / (x1 - x0)
     *
     * @param dst destination buffer
     * @param source buffer
     * @param x0 the x coordinate of first initial point
     * @param y0 the y coordinate of first initial point
     * @param x1 the x coordinate of second initial point
     * @param y1 the y coordinate of second initial point
     * @param x the initial coordinate of X point
     * @param n number of x'es to process
     */
    extern void (* lin_xfade_add3)(
                float *dst, const float *a, const float *b,
                int32_t x0, float y0, int32_t x1, float y1,
                int32_t x, uint32_t n
            );
}

#endif /* DSP_COMMON_CROSSFADE_H_ */
