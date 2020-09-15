/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 24 нояб. 2016 г.
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

#ifndef METADATA_LIMITER_H_
#define METADATA_LIMITER_H_

namespace lsp
{
    //-------------------------------------------------------------------------
    // Limiter
    struct limiter_base_metadata
    {
        static const float  HISTORY_TIME            = 4.0f;     // Amount of time to display history [s]
        static const size_t HISTORY_MESH_SIZE       = 560;      // 420 dots for history
        static const size_t OVERSAMPLING_MAX        = 8;        // Maximum 8x oversampling

        static const float  LOOKAHEAD_MIN           = 0.1f;     // No lookahead [ms]
        static const float  LOOKAHEAD_MAX           = 20.0f;    // Maximum Lookahead [ms]
        static const float  LOOKAHEAD_DFL           = 5.0f;     // Default Lookahead [ms]
        static const float  LOOKAHEAD_STEP          = 0.005f;   // Lookahead step

        static const float  ATTACK_TIME_MIN         = 0.25f;
        static const float  ATTACK_TIME_MAX         = 20.0f;
        static const float  ATTACK_TIME_DFL         = 5.0f;
        static const float  ATTACK_TIME_STEP        = 0.0025f;

        static const float  RELEASE_TIME_MIN        = 0.25f;
        static const float  RELEASE_TIME_MAX        = 20.0f;
        static const float  RELEASE_TIME_DFL        = 5.0f;
        static const float  RELEASE_TIME_STEP       = 0.0025f;

        static const float  ALR_ATTACK_TIME_MIN     = 0.1f;
        static const float  ALR_ATTACK_TIME_MAX     = 200.0f;
        static const float  ALR_ATTACK_TIME_DFL     = 5.0f;
        static const float  ALR_ATTACK_TIME_STEP    = 0.0025f;

        static const float  ALR_RELEASE_TIME_MIN    = 10.0f;
        static const float  ALR_RELEASE_TIME_MAX    = 1000.0f;
        static const float  ALR_RELEASE_TIME_DFL    = 50.0f;
        static const float  ALR_RELEASE_TIME_STEP   = 0.0025f;

        static const float  THRESHOLD_MIN           = GAIN_AMP_M_48_DB;
        static const float  THRESHOLD_MAX           = GAIN_AMP_0_DB;
        static const float  THRESHOLD_DFL           = GAIN_AMP_0_DB;
        static const float  THRESHOLD_STEP          = 0.01f;

        static const float  KNEE_MIN                = GAIN_AMP_M_12_DB;
        static const float  KNEE_MAX                = GAIN_AMP_P_12_DB;
        static const float  KNEE_DFL                = GAIN_AMP_0_DB;
        static const float  KNEE_STEP               = 0.01f;

        static const float  LINKING_MIN             = 0;
        static const float  LINKING_MAX             = 100.0f;
        static const float  LINKING_DFL             = 100.0f;
        static const float  LINKING_STEP            = 0.01f;

        enum oversampling_mode_t
        {
            OVS_NONE,

            OVS_HALF_2X2,
            OVS_HALF_2X3,
            OVS_HALF_3X2,
            OVS_HALF_3X3,
            OVS_HALF_4X2,
            OVS_HALF_4X3,
            OVS_HALF_6X2,
            OVS_HALF_6X3,
            OVS_HALF_8X2,
            OVS_HALF_8X3,

            OVS_FULL_2X2,
            OVS_FULL_2X3,
            OVS_FULL_3X2,
            OVS_FULL_3X3,
            OVS_FULL_4X2,
            OVS_FULL_4X3,
            OVS_FULL_6X2,
            OVS_FULL_6X3,
            OVS_FULL_8X2,
            OVS_FULL_8X3,

            OVS_DEFAULT     = OVS_NONE
        };

        enum limiter_mode_t
        {
            LOM_HERM_THIN,
            LOM_HERM_WIDE,
            LOM_HERM_TAIL,
            LOM_HERM_DUCK,

            LOM_EXP_THIN,
            LOM_EXP_WIDE,
            LOM_EXP_TAIL,
            LOM_EXP_DUCK,

            LOM_LINE_THIN,
            LOM_LINE_WIDE,
            LOM_LINE_TAIL,
            LOM_LINE_DUCK,

            LOM_DEFAULT     = LOM_HERM_THIN
        };

        enum dithering_t
        {
            DITHER_NONE,
            DITHER_7BIT,
            DITHER_8BIT,
            DITHER_11BIT,
            DITHER_12BIT,
            DITHER_15BIT,
            DITHER_16BIT,
            DITHER_23BIT,
            DITHER_24BIT,

            DITHER_DEFAULT  = DITHER_NONE
        };
    };

    struct limiter_mono_metadata: public limiter_base_metadata
    {
        static const plugin_metadata_t metadata;
    };

    struct limiter_stereo_metadata: public limiter_base_metadata
    {
        static const plugin_metadata_t metadata;
    };

    struct sc_limiter_mono_metadata: public limiter_base_metadata
    {
        static const plugin_metadata_t metadata;
    };

    struct sc_limiter_stereo_metadata: public limiter_base_metadata
    {
        static const plugin_metadata_t metadata;
    };

}


#endif /* METADATA_LIMITER_H_ */
