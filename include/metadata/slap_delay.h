/*
 * slap_delay.h
 *
 *  Created on: 30 яна. 2017 г.
 *      Author: sadko
 */

#ifndef METADATA_SLAP_DELAY_H_
#define METADATA_SLAP_DELAY_H_

namespace lsp
{
    //-------------------------------------------------------------------------
    // Compensation delay metadata
    struct slap_delay_base_metadata
    {
        static const float  DISTANCE_MIN        = 0.0f;
        static const float  DISTANCE_MAX        = 400.0f;
        static const float  DISTANCE_STEP       = 0.01;
        static const float  DISTANCE_DFL        = 0.0f;

        static const float  TIME_MIN            = 0.0f;
        static const float  TIME_MAX            = 1000.0f;
        static const float  TIME_STEP           = 0.01f;
        static const float  TIME_DFL            = 0.0f;

        static const float  DENOMINATOR_MIN     = 1.0f;
        static const float  DENOMINATOR_MAX     = 64.0f;
        static const float  DENOMINATOR_STEP    = 1.0f;
        static const float  DENOMINATOR_DFL     = 4.0f;

        static const float  FRACTION_MIN        = 0.0f;
        static const float  FRACTION_MAX        = 2.0f;
        static const float  FRACTION_STEP       = 1.0f / 64.0f;
        static const float  FRACTION_DFL        = 0.0f;

        static const float  TEMPO_MIN           = 20.0f;
        static const float  TEMPO_MAX           = 360.0f;
        static const float  TEMPO_STEP          = 0.1f;
        static const float  TEMPO_DFL           = 120.0f;

        static const float  PRED_TIME_MIN       = 0.0f;
        static const float  PRED_TIME_MAX       = 200.0f;
        static const float  PRED_TIME_STEP      = 0.01f;
        static const float  PRED_TIME_DFL       = 0.0f;

        static const float  STRETCH_MIN         = 25.0f;
        static const float  STRETCH_MAX         = 400.0f;
        static const float  STRETCH_STEP        = 0.1f;
        static const float  STRETCH_DFL         = 100.0f;

        static const float  TEMPERATURE_MIN     = -60;
        static const float  TEMPERATURE_MAX     = +60;
        static const float  TEMPERATURE_DFL     = 20.0;
        static const float  TEMPERATURE_STEP    = 0.1;

        static const float  BAND_GAIN_MIN       = GAIN_AMP_M_24_DB;
        static const float  BAND_GAIN_MAX       = GAIN_AMP_P_24_DB;
        static const float  BAND_GAIN_STEP      = 0.025f;
        static const float  BAND_GAIN_DFL       = GAIN_AMP_0_DB;

        static const float  LOW_CUT_MIN         = SPEC_FREQ_MIN;
        static const float  LOW_CUT_MAX         = 1000.0f;
        static const float  LOW_CUT_STEP        = 0.01f;
        static const float  LOW_CUT_DFL         = 100.0f;

        static const float  HIGH_CUT_MIN        = 1000.0f;
        static const float  HIGH_CUT_MAX        = SPEC_FREQ_MAX;
        static const float  HIGH_CUT_STEP       = 0.01f;
        static const float  HIGH_CUT_DFL        = 8000.0f;

        static const size_t EQ_BANDS            = 5;

        static const size_t MAX_PROCESSORS      = 16;

        enum op_modes_t
        {
            OP_MODE_NONE,
            OP_MODE_TIME,
            OP_MODE_DISTANCE,
            OP_MODE_NOTE
        };
    };

    struct slap_delay_mono_metadata: public slap_delay_base_metadata
    {
        static const plugin_metadata_t metadata;
    };

    struct slap_delay_stereo_metadata: public slap_delay_base_metadata
    {
        static const plugin_metadata_t metadata;

    };

}

#endif /* METADATA_SLAP_DELAY_H_ */
