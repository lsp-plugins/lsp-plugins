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

#ifndef DSP_COMMON_INTERPOLATE_H_
#define DSP_COMMON_INTERPOLATE_H_

namespace dsp
{
    /**
     * Perform linear interpolation of x values defined by initial points:
     *
     * dst[i]    = interp[i]
     * interp[i] = y0 + ((x[i] - x0) * (y1 - y0)) / (x1 - x0)
     *
     * @param dst destination buffer to store interpolated values
     * @param x0 the x coordinate of first initial point
     * @param y0 the y coordinate of first initial point
     * @param x1 the x coordinate of second initial point
     * @param y1 the y coordinate of second initial point
     * @param x the initial coordinate of X point
     * @param n number of x'es to process
     */
    extern void (* lin_inter_set)(
                float *dst,
                int32_t x0, float y0, int32_t x1, float y1,
                int32_t x, uint32_t n
            );

    /**
     * Perform linear interpolation of x values defined by initial points and multiply
     * them by the samples stored in destination buffer
     *
     * dst[i]    = src[i] * interp[i]
     * interp[i] = y0 + ((x[i] - x0) * (y1 - y0)) / (x1 - x0)
     *
     * @param dst destination buffer to apply multiplication by interpolated values
     * @param x0 the x coordinate of first initial point
     * @param y0 the y coordinate of first initial point
     * @param x1 the x coordinate of second initial point
     * @param y1 the y coordinate of second initial point
     * @param x the initial coordinate of X point
     * @param n number of x'es to process
     */
    extern void (* lin_inter_mul2)(
                float *dst,
                int32_t x0, float y0, int32_t x1, float y1,
                int32_t x, uint32_t n
            );

    /**
     * Perform linear interpolation of x values defined by initial points, multiply
     * them by the samples stored in source buffer and store values to destination buffer
     *
     * dst[i]    = src[i] * interp[i]
     * interp[i] = y0 + ((x[i] - x0) * (y1 - y0)) / (x1 - x0)
     *
     * @param dst destination buffer to store the result
     * @param src source buffer to apply multiplication by interpolated values
     * @param x0 the x coordinate of first initial point
     * @param y0 the y coordinate of first initial point
     * @param x1 the x coordinate of second initial point
     * @param y1 the y coordinate of second initial point
     * @param x the initial coordinate of X point
     * @param n number of x'es to process
     */
    extern void (* lin_inter_mul3)(
                float *dst, const float *src,
                int32_t x0, float y0, int32_t x1, float y1,
                int32_t x, uint32_t n
            );

    /**
     * Perform linear interpolation of x values defined by initial points, multiply
     * them by the samples stored in first source buffer and add values to values
     * stored in the destination buffer:
     *
     * dst[i]    = src[i] * interp[i] + dst[i]
     * interp[i] = y0 + ((x[i] - x0) * (y1 - y0)) / (x1 - x0)
     *
     * @param dst destination buffer to add the result
     * @param src source buffer to apply multiplication by interpolated values
     * @param x0 the x coordinate of first initial point
     * @param y0 the y coordinate of first initial point
     * @param x1 the x coordinate of second initial point
     * @param y1 the y coordinate of second initial point
     * @param x the initial coordinate of X point
     * @param n number of x'es to process
     */
    extern void (* lin_inter_fmadd2)(
                float *dst, const float *src,
                int32_t x0, float y0, int32_t x1, float y1,
                int32_t x, uint32_t n
            );

    /**
     * Perform linear interpolation of x values defined by initial points, multiply
     * them by the samples stored in destination source buffer, add values stored in
     * source buffer and store result in destination buffer
     *
     * dst[i]    = dst[i] * interp[i] + src[i]
     * interp[i] = y0 + ((x[i] - x0) * (y1 - y0)) / (x1 - x0)
     *
     * @param dst destination buffer to add the result
     * @param src source buffer to apply multiplication by interpolated values
     * @param x0 the x coordinate of first initial point
     * @param y0 the y coordinate of first initial point
     * @param x1 the x coordinate of second initial point
     * @param y1 the y coordinate of second initial point
     * @param x the initial coordinate of X point
     * @param n number of x'es to process
     */
    extern void (* lin_inter_frmadd2)(
                float *dst, const float *src,
                int32_t x0, float y0, int32_t x1, float y1,
                int32_t x, uint32_t n
            );

    /**
     * Perform linear interpolation of x values defined by initial points, multiply
     * them by the samples stored in first source buffer, add values from second source
     * buffer and store values to destination buffer
     *
     * dst[i]    = src1[i] * interp[i] + src2[i]
     * interp[i] = y0 + ((x[i] - x0) * (y1 - y0)) / (x1 - x0)
     *
     * @param dst destination buffer to store the result
     * @param src1 source buffer to apply multiplication by interpolated values
     * @param src2 source buffer to use for addition
     * @param x0 the x coordinate of first initial point
     * @param y0 the y coordinate of first initial point
     * @param x1 the x coordinate of second initial point
     * @param y1 the y coordinate of second initial point
     * @param x the initial coordinate of X point
     * @param n number of x'es to process
     */
    extern void (* lin_inter_fmadd3)(
                float *dst, const float *src1, const float *src2,
                int32_t x0, float y0, int32_t x1, float y1,
                int32_t x, uint32_t n
            );
}

#endif /* DSP_COMMON_INTERPOLATE_H_ */
