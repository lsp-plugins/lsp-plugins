/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 30 июн. 2020 г.
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

#ifndef METADATA_LC_VOLUME_H_
#define METADATA_LC_VOLUME_H_

namespace lsp
{
    struct loud_comp_base_metadata
    {
        static const float PHONS_MIN            = -83;
        static const float PHONS_MAX            = 7;
        static const float PHONS_DFL            = 0;
        static const float PHONS_STEP           = 0.1f;

        static const float HCRANGE_MIN          = 0;
        static const float HCRANGE_MAX          = 24;
        static const float HCRANGE_DFL          = 6;
        static const float HCRANGE_STEP         = 0.05f;

        static const float FREQ_MIN             = SPEC_FREQ_MIN;
        static const float FREQ_MAX             = SPEC_FREQ_MAX;

        static const size_t FFT_RANK_MIN        = 8;
        static const size_t FFT_RANK_MAX        = 14;
        static const size_t FFT_RANK_IDX_DFL    = 4;

        static const size_t STD_DFL             = 1;

        static const size_t CURVE_MESH_SIZE     = 512;
    };

    struct loud_comp_mono_metadata
    {
        static const plugin_metadata_t metadata;
    };

    struct loud_comp_stereo_metadata
    {
        static const plugin_metadata_t metadata;
    };
}

#endif /* METADATA_LC_VOLUME_H_ */
