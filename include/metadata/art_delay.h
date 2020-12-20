/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 2 дек. 2020 г.
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

#ifndef METADATA_ART_DELAY_H_
#define METADATA_ART_DELAY_H_

namespace lsp
{
    //-------------------------------------------------------------------------
    // Compensation delay metadata
    struct art_delay_base_metadata
    {
        static const float  TIME_MIN            = 0.0f;
        static const float  TIME_MAX            = 256.0f;
        static const float  TIME_STEP           = 0.01f;
        static const float  TIME_DFL            = 0.0f;

        static const float  DSEL_MIN            = 0.0f;
        static const float  DSEL_MAX            = 999.999f;
        static const float  DSEL_STEP           = 0.01f;
        static const float  DSEL_DFL            = 0.0f;

        static const float  DENOMINATOR_MIN     = 1.0f;
        static const float  DENOMINATOR_MAX     = 64.0f;
        static const float  DENOMINATOR_STEP    = 1.0f;
        static const float  DENOMINATOR_DFL     = 4.0f;

        static const float  FRACTION_MIN        = 0.0f;
        static const float  FRACTION_MAX        = 2.0f;
        static const float  FRACTION_STEP       = 1.0f / 64.0f;
        static const float  FRACTION_DFL        = 0.0f;

        static const float  DFRACTION_MIN       = 0.0f;
        static const float  DFRACTION_MAX       = 2.0f;
        static const float  DFRACTION_STEP      = 1.0f / 64.0f;
        static const float  DFRACTION_DFL       = 1.0f;

        static const float  BAR_MULT_MIN        = 0.0f;
        static const float  BAR_MULT_MAX        = 1000.0f;
        static const float  BAR_MULT_STEP       = 1.0f;
        static const float  BAR_MULT_DFL        = 0.0f;

        static const float  DELAY_MULT_MIN      = 0.0f;
        static const float  DELAY_MULT_MAX      = 1000.0f;
        static const float  DELAY_MULT_STEP     = 0.1f;
        static const float  DELAY_MULT_DFL      = 0.0f;

        static const float  ATEMPO_MIN          = 0.0f;
        static const float  ATEMPO_MAX          = 9000.0f;
        static const float  ATEMPO_STEP         = 0.1f;
        static const float  ATEMPO_DFL          = 120.0f;

        static const float  TEMPO_MIN           = 20.0f;
        static const float  TEMPO_MAX           = 360.0f;
        static const float  TEMPO_STEP          = 0.05f;
        static const float  TEMPO_DFL           = 120.0f;

        static const float  BAND_GAIN_MIN       = GAIN_AMP_M_24_DB;
        static const float  BAND_GAIN_MAX       = GAIN_AMP_P_24_DB;
        static const float  BAND_GAIN_STEP      = 0.025f;
        static const float  BAND_GAIN_DFL       = GAIN_AMP_0_DB;

        static const float  LOW_CUT_MIN         = SPEC_FREQ_MIN;
        static const float  LOW_CUT_MAX         = 1000.0f;
        static const float  LOW_CUT_STEP        = 0.001f;
        static const float  LOW_CUT_DFL         = 100.0f;

        static const float  HIGH_CUT_MIN        = 1000.0f;
        static const float  HIGH_CUT_MAX        = SPEC_FREQ_MAX;
        static const float  HIGH_CUT_STEP       = 0.001f;
        static const float  HIGH_CUT_DFL        = 8000.0f;

        static const float  MEMORY_MIN          = 0.0f;
        static const float  MEMORY_MAX          = 65536.0f;
        static const float  MEMORY_DFL          = 0.0f;
        static const float  MEMORY_STEP         = 0.01f;

        static const size_t EQ_BANDS            = 5;

        static const size_t MAX_PROCESSORS      = 16;
        static const size_t MAX_TEMPOS          = 8;

        enum op_modes_t
        {
            OP_MODE_NONE,
            OP_MODE_TIME,
            OP_MODE_NOTE,
            OP_MODE_REF
        };
    };

    struct art_delay_mono_metadata
    {
        static const plugin_metadata_t metadata;
    };

    struct art_delay_stereo_metadata
    {
        static const plugin_metadata_t metadata;
    };
}



#endif /* METADATA_ART_DELAY_H_ */
