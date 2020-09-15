/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 4 июл. 2020 г.
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

#ifndef METADATA_SURGE_FILTER_H_
#define METADATA_SURGE_FILTER_H_

namespace lsp
{
    struct surge_filter_base_metadata
    {
        static const float THRESH_MIN       = GAIN_AMP_M_120_DB;
        static const float THRESH_MAX       = GAIN_AMP_M_24_DB;
        static const float THRESH_DFL       = GAIN_AMP_M_72_DB;
        static const float THRESH_STEP      = 0.1f;

        static const float RMS_MIN          = 4.0f;
        static const float RMS_MAX          = 100.0f;
        static const float RMS_DFL          = 10.0f;
        static const float RMS_STEP         = 0.01f;

        static const float FADEIN_MIN       = 0.0f;
        static const float FADEIN_MAX       = 1000.0f;
        static const float FADEIN_DFL       = 100.0f;
        static const float FADEIN_STEP      = 0.5f;

        static const float FADEOUT_MIN      = 0.0f;
        static const float FADEOUT_MAX      = 500.0f;
        static const float FADEOUT_DFL      = 0.0f;
        static const float FADEOUT_STEP     = 0.5f;

        static const float PAUSE_MIN        = 0.0f;
        static const float PAUSE_MAX        = 100.0f;
        static const float PAUSE_DFL        = 10.0f;
        static const float PAUSE_STEP       = 0.5f;

        static const size_t MESH_POINTS     = 640;
        static const float MESH_TIME        = 5.0f;
    };

    struct surge_filter_mono_metadata
    {
        static const plugin_metadata_t metadata;
    };

    struct surge_filter_stereo_metadata
    {
        static const plugin_metadata_t metadata;
    };
}


#endif /* METADATA_SURGE_FILTER_H_ */
