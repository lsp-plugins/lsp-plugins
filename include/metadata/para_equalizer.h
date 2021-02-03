/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 30 мая 2016 г.
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

#ifndef METADATA_PARA_EQUALIZER_H_
#define METADATA_PARA_EQUALIZER_H_

namespace lsp
{
    //-------------------------------------------------------------------------
    // Parametric Equalizer
    struct para_equalizer_base_metadata
    {
        static const float          FREQ_MIN            = SPEC_FREQ_MIN;
        static const float          FREQ_MAX            = SPEC_FREQ_MAX;
        static const float          FREQ_DFL            = 1000;
        static const float          FREQ_STEP           = 0.002;

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

        static const float          ZOOM_MIN            = GAIN_AMP_M_36_DB;
        static const float          ZOOM_MAX            = GAIN_AMP_0_DB;
        static const float          ZOOM_DFL            = GAIN_AMP_0_DB;
        static const float          ZOOM_STEP           = 0.025f;

        static const float          IN_GAIN_DFL         = 1.0f;
        static const float          OUT_GAIN_DFL        = 1.0f;
        static const size_t         MODE_DFL            = 0;

        static const float          PITCH_MIN           = -120.0f;
        static const float          PITCH_MAX           = 120.0f;
        static const float          PITCH_DFL           = 0.0f;
        static const float          PITCH_STEP          = 0.01f;

        static const size_t         REFRESH_RATE        = 20;

        enum eq_filter_t
        {
            EQF_OFF,
            EQF_BELL,
            EQF_HIPASS,
            EQF_HISHELF,
            EQF_LOPASS,
            EQF_LOSHELF,
            EQF_NOTCH,
            EQF_RESONANCE,
            EQF_ALLPASS,

#ifndef LSP_NO_EXPERIMENTAL
            EQF_ALLPASS2,
            EQF_LADDERPASS,
            EQF_LADDERREJ,
            EQF_ENVELOPE,
            EQF_BANDPASS
#endif
        };

        enum eq_filter_mode_t
        {
            EFM_RLC_BT,
            EFM_RLC_MT,
            EFM_BWC_BT,
            EFM_BWC_MT,
            EFM_LRX_BT,
            EFM_LRX_MT,
            EFM_APO_DR
        };

        enum para_eq_mode_t
        {
            PEM_IIR,
            PEM_FIR,
            PEM_FFT,
            PEM_SPM
        };
    };

    struct para_equalizer_x16_mono_metadata: public para_equalizer_base_metadata
    {
        static const plugin_metadata_t metadata;
    };

    struct para_equalizer_x32_mono_metadata: public para_equalizer_base_metadata
    {
        static const plugin_metadata_t metadata;
    };

    struct para_equalizer_x16_stereo_metadata: public para_equalizer_base_metadata
    {
        static const plugin_metadata_t metadata;
    };

    struct para_equalizer_x32_stereo_metadata: public para_equalizer_base_metadata
    {
        static const plugin_metadata_t metadata;
    };

    struct para_equalizer_x16_lr_metadata: public para_equalizer_base_metadata
    {
        static const plugin_metadata_t metadata;
    };

    struct para_equalizer_x32_lr_metadata: public para_equalizer_base_metadata
    {
        static const plugin_metadata_t metadata;
    };

    struct para_equalizer_x16_ms_metadata: public para_equalizer_base_metadata
    {
        static const plugin_metadata_t metadata;
    };

    struct para_equalizer_x32_ms_metadata: public para_equalizer_base_metadata
    {
        static const plugin_metadata_t metadata;
    };
}

#endif /* METADATA_PARA_EQUALIZER_H_ */
