/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 15 сен. 2016 г.
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

#ifndef METADATA_COMPRESSOR_H_
#define METADATA_COMPRESSOR_H_

namespace lsp
{
    //-------------------------------------------------------------------------
    // Compressor
    struct compressor_base_metadata
    {
        static const float  ATTACK_LVL_MIN          = GAIN_AMP_M_60_DB;
        static const float  ATTACK_LVL_MAX          = GAIN_AMP_0_DB;
        static const float  ATTACK_LVL_DFL          = GAIN_AMP_M_12_DB;
        static const float  ATTACK_LVL_STEP         = 0.05f;

        static const float  RELEASE_LVL_MIN         = GAIN_AMP_M_INF_DB;
        static const float  RELEASE_LVL_MAX         = GAIN_AMP_0_DB;
        static const float  RELEASE_LVL_DFL         = GAIN_AMP_M_INF_DB;
        static const float  RELEASE_LVL_STEP        = 0.05f;

        static const float  ATTACK_TIME_MIN         = 0.0f;
        static const float  ATTACK_TIME_MAX         = 2000.0f;
        static const float  ATTACK_TIME_DFL         = 20.0f;
        static const float  ATTACK_TIME_STEP        = 0.0025f;

        static const float  RELEASE_TIME_MIN        = 0.0f;
        static const float  RELEASE_TIME_MAX        = 5000.0f;
        static const float  RELEASE_TIME_DFL        = 100.0f;
        static const float  RELEASE_TIME_STEP       = 0.0025f;

        static const float  KNEE_MIN                = GAIN_AMP_M_24_DB;
        static const float  KNEE_MAX                = GAIN_AMP_0_DB;
        static const float  KNEE_DFL                = GAIN_AMP_M_6_DB;
        static const float  KNEE_STEP               = 0.01f;

        static const float  BTH_MIN                 = GAIN_AMP_M_120_DB;
        static const float  BTH_MAX                 = GAIN_AMP_M_60_DB;
        static const float  BTH_DFL                 = GAIN_AMP_M_72_DB;
        static const float  BTH_STEP                = 0.05f;

        static const float  MAKEUP_MIN              = GAIN_AMP_M_60_DB;
        static const float  MAKEUP_MAX              = GAIN_AMP_P_60_DB;
        static const float  MAKEUP_DFL              = GAIN_AMP_0_DB;
        static const float  MAKEUP_STEP             = 0.05f;

        static const float  RATIO_MIN               = 1.0f;
        static const float  RATIO_MAX               = 100.0f;
        static const float  RATIO_DFL               = 4.0f;
        static const float  RATIO_STEP              = 0.0025f;

        static const float  LOOKAHEAD_MIN           = 0.0f;
        static const float  LOOKAHEAD_MAX           = 20.0f;
        static const float  LOOKAHEAD_DFL           = 0.0f;
        static const float  LOOKAHEAD_STEP          = 0.01f;

        static const float  REACTIVITY_MIN          = 0.000;    // Minimum reactivity [ms]
        static const float  REACTIVITY_MAX          = 250;      // Maximum reactivity [ms]
        static const float  REACTIVITY_DFL          = 10;       // Default reactivity [ms]
        static const float  REACTIVITY_STEP         = 0.025;    // Reactivity step

        static const size_t SC_MODE_DFL             = 1;
        static const size_t SC_SOURCE_DFL           = 0;
        static const size_t SC_TYPE_DFL             = 0;

        static const float  HPF_MIN                 = 10.0f;
        static const float  HPF_MAX                 = 20000.0f;
        static const float  HPF_DFL                 = 10.0f;
        static const float  HPF_STEP                = 0.0025f;

        static const float  LPF_MIN                 = 10.0f;
        static const float  LPF_MAX                 = 20000.0f;
        static const float  LPF_DFL                 = 20000.0f;
        static const float  LPF_STEP                = 0.0025f;

        static const size_t CURVE_MESH_SIZE         = 256;
        static const float  CURVE_DB_MIN            = -72;
        static const float  CURVE_DB_MAX            = +24;

        static const size_t TIME_MESH_SIZE          = 400;
        static const float  TIME_HISTORY_MAX        = 5.0f;

        enum mode_t
        {
            CM_DOWNWARD,
            CM_UPWARD,
            CM_BOOSTING
        };

        static const size_t CM_DEFAULT              = CM_DOWNWARD;

    };

    struct compressor_mono_metadata: public compressor_base_metadata
    {
        static const plugin_metadata_t metadata;
    };

    struct compressor_stereo_metadata: public compressor_base_metadata
    {
        static const plugin_metadata_t metadata;
    };

    struct compressor_lr_metadata: public compressor_base_metadata
    {
        static const plugin_metadata_t metadata;
    };

    struct compressor_ms_metadata: public compressor_base_metadata
    {
        static const plugin_metadata_t metadata;
    };

    struct sc_compressor_mono_metadata: public compressor_base_metadata
    {
        static const plugin_metadata_t metadata;
    };

    struct sc_compressor_stereo_metadata: public compressor_base_metadata
    {
        static const plugin_metadata_t metadata;
    };

    struct sc_compressor_lr_metadata: public compressor_base_metadata
    {
        static const plugin_metadata_t metadata;
    };

    struct sc_compressor_ms_metadata: public compressor_base_metadata
    {
        static const plugin_metadata_t metadata;
    };

}

#endif /* METADATA_COMPRESSOR_H_ */
