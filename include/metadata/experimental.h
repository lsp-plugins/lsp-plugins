/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 08 апр. 2016 г.
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

#ifndef METADATA_EXPERIMENTAL_H_
#define METADATA_EXPERIMENTAL_H_

namespace lsp
{
#ifndef LSP_NO_EXPERIMENTAL
    struct test_plugin_metadata
    {
        static const float          BAND_GAIN_MIN       = GAIN_AMP_M_36_DB;
        static const float          BAND_GAIN_MAX       = GAIN_AMP_P_36_DB;
        static const float          BAND_GAIN_DFL       = GAIN_AMP_0_DB;
        static const float          BAND_GAIN_STEP      = 0.1f;

        static const float PREDELAY_MIN             = 0.0f;     // Pre-delay min (ms)
        static const float PREDELAY_MAX             = 100.0f;   // Pre-delay max (ms)
        static const float PREDELAY_DFL             = 0.0f;     // Pre-delay default (ms)
        static const float PREDELAY_STEP            = 0.1f;     // Pre-delay step (ms)

        static const float FILE_LENGTH_MIN          = 0.0f;     // Minimum convolution length (ms)
        static const float FILE_LENGTH_MAX          = 10000.0f; // Maximum convoluition length (ms)
        static const float FILE_LENGTH_DFL          = 0.0f;     // Convolution length (ms)
        static const float FILE_LENGTH_STEP         = 0.1f;     // Convolution step (ms)

        static const size_t MESH_SIZE               = 256;      // Maximum mesh size
        static const size_t TRACKS_MAX              = 2;        // Maximum tracks per mesh/sample

        static const size_t FRM_BUFFER_SIZE         = 256;
        static const size_t LIS_BUFFER_SIZE         = 4096;     // 4096 points per figure
        static const size_t LIS_BUFFER_PERIOD       = 2048;     // 2048 points - period

        static const plugin_metadata_t metadata;
    };

    struct filter_analyzer_metadata
    {
        static const size_t         MESH_POINTS         = 640;

        static const float          FREQ_MIN            = SPEC_FREQ_MIN;
        static const float          FREQ_MAX            = SPEC_FREQ_MAX;
        static const float          FREQ_DFL            = 100;
        static const float          FREQ_STEP           = 0.002;

        static const float          GAIN_MIN            = GAIN_AMP_M_36_DB;
        static const float          GAIN_MAX            = GAIN_AMP_P_36_DB;
        static const float          GAIN_DFL            = GAIN_AMP_0_DB;
        static const float          GAIN_STEP           = 0.01f;

        static const float          QUALITY_MIN         = 0.0f;
        static const float          QUALITY_MAX         = 100.0f;
        static const float          QUALITY_DFL         = 0.0f;
        static const float          QUALITY_STEP        = 0.025f;

        static const plugin_metadata_t metadata;
    };
#endif
}

#endif /* METADATA_EXPERIMENTAL_H_ */
