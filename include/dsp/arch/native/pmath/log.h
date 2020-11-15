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

#ifndef DSP_ARCH_NATIVE_PMATH_LOG_H_
#define DSP_ARCH_NATIVE_PMATH_LOG_H_

#ifndef __DSP_NATIVE_IMPL
    #error "This header should not be included directly"
#endif /* __DSP_NATIVE_IMPL */

namespace native
{
    void logb1(float *dst, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i] = ::logf(dst[i]) * M_LOG2E;
    }

    void logb2(float *dst, const float *src, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i] = ::logf(src[i]) * M_LOG2E;
    }

    void loge1(float *dst, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i] = ::logf(dst[i]);
    }

    void loge2(float *dst, const float *src, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i] = ::logf(src[i]);
    }

    void logd1(float *dst, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i] = ::log10f(dst[i]);
    }

    void logd2(float *dst, const float *src, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i] = ::log10f(src[i]);
    }
}

#endif /* DSP_ARCH_NATIVE_PMATH_LOG_H_ */
