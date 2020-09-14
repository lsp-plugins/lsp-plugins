/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 8 сент. 2020 г.
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

#ifndef METADATA_CROSSOVER_H_
#define METADATA_CROSSOVER_H_

namespace lsp
{
    //-------------------------------------------------------------------------
    // Crossover
    struct crossover_base_metadata
    {
        // Maximum supported number of bands
        static const size_t         BANDS_MAX           = 8;
        static const size_t         SLOPE_DFL           = 2;

        // In/out gain
        static const float          IN_GAIN_DFL         = GAIN_AMP_0_DB;
        static const float          OUT_GAIN_DFL        = GAIN_AMP_0_DB;

        // Makeup gain for each band
        static const float          MAKEUP_MIN          = GAIN_AMP_M_60_DB;
        static const float          MAKEUP_MAX          = GAIN_AMP_P_60_DB;
        static const float          MAKEUP_DFL          = GAIN_AMP_0_DB;
        static const float          MAKEUP_STEP         = 0.05f;

        // Split frequency
        static const float          SPLIT_FREQ_MIN      = 10.0f;
        static const float          SPLIT_FREQ_MAX      = 20000.0f;
        static const float          SPLIT_FREQ_DFL      = 1000.0f;
        static const float          SPLIT_FREQ_STEP     = 0.002f;

        // Frequency analysis
        static const float          REACT_TIME_MIN      = 0.000;
        static const float          REACT_TIME_MAX      = 1.000;
        static const float          REACT_TIME_DFL      = 0.200;
        static const float          REACT_TIME_STEP     = 0.001;
        static const size_t         FFT_RANK            = 13;
        static const size_t         FFT_ITEMS           = 1 << FFT_RANK;
        static const size_t         MESH_POINTS         = 640;
        static const size_t         FILTER_MESH_POINTS  = MESH_POINTS + 2;
        static const size_t         FFT_WINDOW          = windows::HANN;
        static const size_t         REFRESH_RATE        = 20;

        // Zoom
        static const float          ZOOM_MIN            = GAIN_AMP_M_18_DB;
        static const float          ZOOM_MAX            = GAIN_AMP_0_DB;
        static const float          ZOOM_DFL            = GAIN_AMP_0_DB;
        static const float          ZOOM_STEP           = 0.0125f;
    };

    struct crossover_mono_metadata: public crossover_base_metadata
    {
        static const plugin_metadata_t metadata;
    };

    struct crossover_stereo_metadata: public crossover_base_metadata
    {
        static const plugin_metadata_t metadata;
    };

    struct crossover_lr_metadata: public crossover_base_metadata
    {
        static const plugin_metadata_t metadata;
    };

    struct crossover_ms_metadata: public crossover_base_metadata
    {
        static const plugin_metadata_t metadata;
    };
}



#endif /* METADATA_CROSSOVER_H_ */
