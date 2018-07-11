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
        static const float  METERS_MAX          = 200;
        static const float  CENTIMETERS_MAX     = 100;
        static const float  CENTIMETERS_STEP    = 0.1;
        static const float  SAMPLES_MAX         = 10000;
        static const float  TIME_MAX            = 1000;
        static const float  TIME_STEP           = 0.01;

        static const float  TEMPERATURE_MIN     = -60;
        static const float  TEMPERATURE_MAX     = +60;
        static const float  TEMPERATURE_DFL     = 20.0;
        static const float  TEMPERATURE_STEP    = 0.1;

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
            IN,

            // Output Audio
            OUT,

            // Input controls
            BYPASS,
            MODE, SAMPLES, METERS, CENTIMETERS, TEMPERATURE, TIME,
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
            MODE, SAMPLES, METERS, CENTIMETERS, TEMPERATURE, TIME,
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
            MODE_L, SAMPLES_L, METERS_L, CENTIMETERS_L, TEMPERATURE_L, TIME_L,
            DRY_L, WET_L,
            MODE_R, SAMPLES_R, METERS_R, CENTIMETERS_R, TEMPERATURE_R, TIME_R,
            DRY_R, WET_R,
            OUT_GAIN,

            // Output controls/meters
            DEL_TIME_L, DEL_SAMPLES_L, DEL_DISTANCE_L,
            DEL_TIME_R, DEL_SAMPLES_R, DEL_DISTANCE_R
        };
    };
}

#endif /* METADATA_COMP_DELAY_H_ */
