/*
 * comp_delay.h
 *
 *  Created on: 08 апр. 2016 г.
 *      Author: sadko
 */

#ifndef METADATA_COMP_DELAY_H_
#define METADATA_COMP_DELAY_H_

namespace lsp
{
    //-------------------------------------------------------------------------
    // Compensation delay metadata
    struct comp_delay_base_metadata
    {
        static const float  METERS_MIN          = 0.0f;
        static const float  METERS_MAX          = 200.0f;
        static const float  METERS_DFL          = 0.0f;
        static const float  METERS_STEP         = 1.0f;

        static const float  CENTIMETERS_MIN     = 0.0f;
        static const float  CENTIMETERS_MAX     = 100.0f;
        static const float  CENTIMETERS_DFL     = 0.0f;
        static const float  CENTIMETERS_STEP    = 0.1f;

        static const float  SAMPLES_MIN         = 0.0f;
        static const float  SAMPLES_MAX         = 10000.0f;
        static const float  SAMPLES_DFL         = 0.0f;
        static const float  SAMPLES_STEP        = 1.0f;

        static const float  TIME_MIN            = 0.0f;
        static const float  TIME_MAX            = 1000.0f;
        static const float  TIME_DFL            = 0.0f;
        static const float  TIME_STEP           = 0.01f;

        static const float  TEMPERATURE_MIN     = -60.0f;
        static const float  TEMPERATURE_MAX     = +60.0f;
        static const float  TEMPERATURE_DFL     = 20.0f;
        static const float  TEMPERATURE_STEP    = 0.1f;

        static const float  DELAY_OUT_MAX_TIME      = 1000 /* TIME_MAX [ms] */;
        static const float  DELAY_OUT_MAX_SAMPLES   = 1000 /* TIME_MAX [ms] */ * 0.001 /* [ s/ms ] */ * MAX_SAMPLE_RATE /* [samples / s] */;
        static const float  DELAY_OUT_MAX_DISTANCE  = 1000 /* TIME_MAX [ms] */ * 0.001 /* [ s/ms ] */ * MAX_SOUND_SPEED /* [m/s] */ * 100 /* [cm / m ] */;

        enum modes
        {
            M_SAMPLES,
            M_DISTANCE,
            M_TIME
        };
    };

    struct comp_delay_mono_metadata: public comp_delay_base_metadata
    {
        static const plugin_metadata_t metadata;

        enum ports
        {
            // Input Audio
            A_IN,

            // Output Audio
            A_OUT,

            // Input controls
            BYPASS,
            MODE, RAMPING, SAMPLES, METERS, CENTIMETERS, TEMPERATURE, TIME,
            DRY, WET,
            OUT_GAIN,

            // Output controls/meters
            DEL_TIME, DEL_SAMPLES, DEL_DISTANCE
        };
    };

    struct comp_delay_stereo_metadata: public comp_delay_base_metadata
    {
        static const plugin_metadata_t metadata;

        enum ports
        {
            // Input Audio
            IN_L, IN_R,

            // Output Audio
            OUT_L, OUT_R,

            // Input controls
            BYPASS,
            MODE, RAMPING, SAMPLES, METERS, CENTIMETERS, TEMPERATURE, TIME,
            DRY, WET,
            OUT_GAIN,

            // Output controls/meters
            DEL_TIME, DEL_SAMPLES, DEL_DISTANCE
        };
    };

    struct comp_delay_x2_stereo_metadata: public comp_delay_base_metadata
    {
        static const plugin_metadata_t metadata;

        enum ports
        {
            // Input Audio
            IN_L, IN_R,

            // Output Audio
            OUT_L, OUT_R,

            // Input controls
            BYPASS,
            MODE_L, RAMPING_L, SAMPLES_L, METERS_L, CENTIMETERS_L, TEMPERATURE_L, TIME_L,
            DRY_L, WET_L,
            MODE_R, RAMPING_R, SAMPLES_R, METERS_R, CENTIMETERS_R, TEMPERATURE_R, TIME_R,
            DRY_R, WET_R,
            OUT_GAIN,

            // Output controls/meters
            DEL_TIME_L, DEL_SAMPLES_L, DEL_DISTANCE_L,
            DEL_TIME_R, DEL_SAMPLES_R, DEL_DISTANCE_R
        };
    };
}

#endif /* METADATA_COMP_DELAY_H_ */
