/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 29 июн. 2020 г.
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

#ifndef GENERATED_ISO226_DEFS_H_
#define GENERATED_ISO226_DEFS_H_

#include <core/types.h>

namespace lsp
{
    typedef struct freq_curve_t
    {
        float   fmin;       // The minimum frequency per curve
        float   fmax;       // The maximum frequency per curve
        float   amin;       // The minimum amplitude per curve
        float   amax;       // The maximum amplitude per curve
        size_t  hdots;      // Number of dots per each curve
        size_t  curves;     // The overall number of curves

        const float * const *data; // The array of curve data
    } freq_curve_t;
}

#endif /* GENERATED_ISO226_DEFS_H_ */
