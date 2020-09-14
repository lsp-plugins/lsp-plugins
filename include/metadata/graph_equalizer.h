/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 03 авг. 2016 г.
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

#ifndef METADATA_GRAPH_EQUALIZER_H_
#define METADATA_GRAPH_EQUALIZER_H_

namespace lsp
{
    //-------------------------------------------------------------------------
    // Graphic Equalizer

    struct graph_equalizer_base_metadata
    {
        static const size_t         SLOPE_MIN           = 2;
        static const size_t         SLOPE_DFL           = 0;
        static const float          FREQ_MIN            = SPEC_FREQ_MIN;
        static const float          FREQ_MAX            = SPEC_FREQ_MAX;

        static const size_t         FFT_RANK            = 13;
        static const size_t         FFT_ITEMS           = 1 << FFT_RANK;
        static const size_t         MESH_POINTS         = 640;
        static const size_t         FILTER_MESH_POINTS  = MESH_POINTS + 2;
        static const size_t         FFT_WINDOW          = windows::HANN;
        static const size_t         FFT_ENVELOPE        = envelope::PINK_NOISE;

        static const float          REACT_TIME_MIN      = 0.000;
        static const float          REACT_TIME_MAX      = 1.000;
        static const float          REACT_TIME_DFL      = 0.200;
        static const float          REACT_TIME_STEP     = 0.001;

        static const float          BAND_GAIN_MIN       = GAIN_AMP_M_36_DB;
        static const float          BAND_GAIN_MAX       = GAIN_AMP_P_36_DB;
        static const float          BAND_GAIN_DFL       = GAIN_AMP_0_DB;
        static const float          BAND_GAIN_STEP      = 0.025f;

        static const float          ZOOM_MIN            = GAIN_AMP_M_36_DB;
        static const float          ZOOM_MAX            = GAIN_AMP_0_DB;
        static const float          ZOOM_DFL            = GAIN_AMP_0_DB;
        static const float          ZOOM_STEP           = 0.025f;

        static const float          IN_GAIN_DFL         = 1.0f;
        static const float          OUT_GAIN_DFL        = 1.0f;
        static const size_t         MODE_DFL            = 0;

        static const size_t         REFRESH_RATE        = 20;

        enum para_eq_mode_t
        {
            PEM_IIR,
            PEM_FIR,
            PEM_FFT
        };
    };

    struct graph_equalizer_x16_mono_metadata: public graph_equalizer_base_metadata
    {
        static const plugin_metadata_t metadata;
    };

    struct graph_equalizer_x32_mono_metadata: public graph_equalizer_base_metadata
    {
        static const plugin_metadata_t metadata;
    };

    struct graph_equalizer_x16_stereo_metadata: public graph_equalizer_base_metadata
    {
        static const plugin_metadata_t metadata;
    };

    struct graph_equalizer_x32_stereo_metadata: public graph_equalizer_base_metadata
    {
        static const plugin_metadata_t metadata;
    };

    struct graph_equalizer_x16_lr_metadata: public graph_equalizer_base_metadata
    {
        static const plugin_metadata_t metadata;
    };

    struct graph_equalizer_x32_lr_metadata: public graph_equalizer_base_metadata
    {
        static const plugin_metadata_t metadata;
    };

    struct graph_equalizer_x16_ms_metadata: public graph_equalizer_base_metadata
    {
        static const plugin_metadata_t metadata;
    };

    struct graph_equalizer_x32_ms_metadata: public graph_equalizer_base_metadata
    {
        static const plugin_metadata_t metadata;
    };
}

#endif /* METADATA_GRAPH_EQUALIZER_H_ */
