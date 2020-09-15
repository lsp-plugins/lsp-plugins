/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 7 нояб. 2016 г.
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

#ifndef METADATA_GATE_H_
#define METADATA_GATE_H_

namespace lsp
{
    //-------------------------------------------------------------------------
    // Gate
    struct gate_base_metadata
    {
        static const float  THRESHOLD_MIN           = GAIN_AMP_M_60_DB;
        static const float  THRESHOLD_MAX           = GAIN_AMP_0_DB;
        static const float  THRESHOLD_DFL           = GAIN_AMP_M_24_DB;
        static const float  THRESHOLD_STEP          = 0.05f;

        static const float  H_THRESHOLD_MIN         = GAIN_AMP_M_60_DB;
        static const float  H_THRESHOLD_MAX         = GAIN_AMP_0_DB;
        static const float  H_THRESHOLD_DFL         = GAIN_AMP_M_12_DB;
        static const float  H_THRESHOLD_STEP        = 0.05f;

        static const float  REDUCTION_MIN           = GAIN_AMP_M_72_DB;
        static const float  REDUCTION_MAX           = GAIN_AMP_0_DB;
        static const float  REDUCTION_DFL           = GAIN_AMP_M_24_DB;
        static const float  REDUCTION_STEP          = 0.05f;

        static const float  ATTACK_TIME_MIN         = 0.0f;
        static const float  ATTACK_TIME_MAX         = 2000.0f;
        static const float  ATTACK_TIME_DFL         = 20.0f;
        static const float  ATTACK_TIME_STEP        = 0.0025f;

        static const float  RELEASE_TIME_MIN        = 0.0f;
        static const float  RELEASE_TIME_MAX        = 5000.0f;
        static const float  RELEASE_TIME_DFL        = 100.0f;
        static const float  RELEASE_TIME_STEP       = 0.0025f;

        static const float  MAKEUP_MIN              = GAIN_AMP_M_60_DB;
        static const float  MAKEUP_MAX              = GAIN_AMP_P_60_DB;
        static const float  MAKEUP_DFL              = GAIN_AMP_0_DB;
        static const float  MAKEUP_STEP             = 0.05f;

        static const float  ZONE_MIN                = GAIN_AMP_M_60_DB;
        static const float  ZONE_MAX                = GAIN_AMP_0_DB;
        static const float  ZONE_DFL                = GAIN_AMP_M_6_DB;
        static const float  ZONE_STEP               = 0.05f;

        static const float  LOOKAHEAD_MIN           = 0.0f;
        static const float  LOOKAHEAD_MAX           = 20.0f;
        static const float  LOOKAHEAD_DFL           = 0.0f;
        static const float  LOOKAHEAD_STEP          = 0.01f;

        static const float  REACTIVITY_MIN          = 0.000;    // Minimum reactivity [ms]
        static const float  REACTIVITY_MAX          = 250;      // Maximum reactivity [ms]
        static const float  REACTIVITY_DFL          = 10;       // Default reactivity [ms]
        static const float  REACTIVITY_STEP         = 0.01;     // Reactivity step

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
            EM_DOWNWARD,
            EM_UPWARD
        };

        static const size_t EM_DEFAULT              = EM_UPWARD;
    };

    struct gate_mono_metadata: public gate_base_metadata
    {
        static const plugin_metadata_t metadata;
    };

    struct gate_stereo_metadata: public gate_base_metadata
    {
        static const plugin_metadata_t metadata;
    };

    struct gate_lr_metadata: public gate_base_metadata
    {
        static const plugin_metadata_t metadata;
    };

    struct gate_ms_metadata: public gate_base_metadata
    {
        static const plugin_metadata_t metadata;
    };

    struct sc_gate_mono_metadata: public gate_base_metadata
    {
        static const plugin_metadata_t metadata;
    };

    struct sc_gate_stereo_metadata: public gate_base_metadata
    {
        static const plugin_metadata_t metadata;
    };

    struct sc_gate_lr_metadata: public gate_base_metadata
    {
        static const plugin_metadata_t metadata;
    };

    struct sc_gate_ms_metadata: public gate_base_metadata
    {
        static const plugin_metadata_t metadata;
    };
}


#endif /* METADATA_GATE_H_ */
