/*
 * metadata.h
 *
 *  Created on: 16 окт. 2015 г.
 *      Author: sadko
 */

#ifndef CORE_PLUGIN_METADATA_H_
#define CORE_PLUGIN_METADATA_H_

#include <core/metadata.h>
#include <core/types.h>
#include <core/windows.h>
#include <core/envelope.h>

namespace lsp
{
    //-------------------------------------------------------------------------
    // Phase detector metadata
    struct phase_detector_metadata
    {
        static const plugin_metadata_t metadata;

        static const float DETECT_TIME_MIN          =   1.0f;
        static const float DETECT_TIME_MAX          =   50.0f;
        static const float DETECT_TIME_DFL          =   10.0f;
        static const float DETECT_TIME_STEP         =   0.025f;
        static const float DETECT_TIME_RANGE_MAX    =   100.0f;
        static const float DETECT_TIME_RANGE_MIN    =   - 100.0f;

        static const size_t MESH_POINTS             =   256;

        static const float REACT_TIME_MIN           =   0.000;
        static const float REACT_TIME_MAX           =  10.000;
        static const float REACT_TIME_DFL           =   1.000;
        static const float REACT_TIME_STEP          =   0.025;

        static const float SELECTOR_MIN             =   -100.0f;
        static const float SELECTOR_MAX             =   100.0f;
        static const float SELECTOR_DFL             =   0.0f;
        static const float SELECTOR_STEP            =   0.1f;

        static const float SAMPLES_MIN              =   - 50.0f /* DETECT_TIME_MAX [ms] */ * 0.001 /* [s/ms] */ * MAX_SAMPLE_RATE /* [ samples / s ] */;
        static const float SAMPLES_MAX              =   + 50.0f /* DETECT_TIME_MAX [ms] */ * 0.001 /* [s/ms] */ * MAX_SAMPLE_RATE /* [ samples / s ] */;
        static const float DISTANCE_MIN             =   - 50.0f /* DETECT_TIME_MAX [ms] */ * 0.001 /* [s/ms] */ * MAX_SOUND_SPEED /* [ m / s] */ * 100 /* c / m */;
        static const float DISTANCE_MAX             =   + 50.0f /* DETECT_TIME_MAX [ms] */ * 0.001 /* [s/ms] */ * MAX_SOUND_SPEED /* [ m / s] */ * 100 /* c / m */;
        static const float TIME_MIN                 =   - 50.0f /* DETECT_TIME_MAX [ms] */;
        static const float TIME_MAX                 =   + 50.0f /* DETECT_TIME_MAX [ms] */;

        enum ports
        {
            // Input Audio
            IN_A, IN_B,

            // Output Audio
            OUT_A, OUT_B,

            // Input controls
            BYPASS, RESET,
            TIME, REACTIVITY, SELECTOR,

            // Output controls/meters
            BEST_TIME, BEST_SAMPLES, BEST_DISTANCE, BEST_VALUE,
            SEL_TIME, SEL_SAMPLES, SEL_DISTANCE, SEL_VALUE,
            WORST_TIME, WORST_SAMPLES, WORST_DISTANCE, WORST_VALUE,
            FUNCTION
        };
    };

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

    //-------------------------------------------------------------------------
    // Spectral analyzer
    struct spectrum_analyzer_base_metadata
    {
        static const float          FREQ_MIN            = SPEC_FREQ_MIN;
        static const float          FREQ_DFL            = 1000.0f;
        static const float          FREQ_MAX            = SPEC_FREQ_MAX;

        static const size_t         PORTS_PER_INPUT     = 6;
        static const size_t         RANK_MIN            = 10;
        static const size_t         RANK_DFL            = 12;
        static const size_t         RANK_MAX            = 14;
        static const size_t         MESH_POINTS         = 512;
        static const size_t         WND_DFL             = windows::HANN;
        static const size_t         ENV_DFL             = envelope::PINK_NOISE;

        static const float          REACT_TIME_MIN      = 0.000;
        static const float          REACT_TIME_MAX      = 1.000;
        static const float          REACT_TIME_DFL      = 0.200;
        static const float          REACT_TIME_STEP     = 0.001;

        static const float          SELECTOR_MIN        = 0;
        static const float          SELECTOR_DFL        = 0;
        static const float          SELECTOR_MAX        = 100;
        static const float          SELECTOR_STEP       = 0.005;

        static const float          PREAMP_DFL          = 1.0;

        static const size_t         REFRESH_RATE        = 20;
    };

    #define SA_INPUT(x) IN_ ## x, OUT_ ## x, ON_ ## x, SOLO_ ## x, FREEZE_ ## x, HUE_ ## x, SHIFT_ ## x, SPECTRUM_ ## x
    #define SA_COMMON   BYPASS, TOLERANCE, WINDOW, ENVELOPE, PREAMP, REACT, CHANNEL, SELECTOR, FREQUENCY, LEVEL

    struct spectrum_analyzer_x1_metadata: public spectrum_analyzer_base_metadata
    {
        static const plugin_metadata_t metadata;

        enum ports
        {
            SA_INPUT(0),
            SA_COMMON
        };
    };

    struct spectrum_analyzer_x2_metadata: public spectrum_analyzer_base_metadata
    {
        static const plugin_metadata_t metadata;

        enum ports
        {
            SA_INPUT(0),
            SA_INPUT(1),
            SA_COMMON
        };
    };

    struct spectrum_analyzer_x4_metadata: public spectrum_analyzer_base_metadata
    {
        static const plugin_metadata_t metadata;

        enum ports
        {
            SA_INPUT(0),
            SA_INPUT(1),
            SA_INPUT(2),
            SA_INPUT(3),
            SA_COMMON
        };
    };

    struct spectrum_analyzer_x8_metadata: public spectrum_analyzer_base_metadata
    {
        static const plugin_metadata_t metadata;

        enum ports
        {
            SA_INPUT(0),
            SA_INPUT(1),
            SA_INPUT(2),
            SA_INPUT(3),
            SA_INPUT(4),
            SA_INPUT(5),
            SA_INPUT(6),
            SA_INPUT(7),
            SA_COMMON
        };
    };

    struct spectrum_analyzer_x12_metadata: public spectrum_analyzer_base_metadata
    {
        static const plugin_metadata_t metadata;

        enum ports
        {
            SA_INPUT(0),
            SA_INPUT(1),
            SA_INPUT(2),
            SA_INPUT(3),
            SA_INPUT(4),
            SA_INPUT(5),
            SA_INPUT(6),
            SA_INPUT(7),
            SA_INPUT(8),
            SA_INPUT(9),
            SA_INPUT(10),
            SA_INPUT(11),
            SA_INPUT(12),
            SA_COMMON
        };
    };

    struct spectrum_analyzer_x16_metadata: public spectrum_analyzer_base_metadata
    {
        static const plugin_metadata_t metadata;

        enum ports
        {
            SA_INPUT(0),
            SA_INPUT(1),
            SA_INPUT(2),
            SA_INPUT(3),
            SA_INPUT(4),
            SA_INPUT(5),
            SA_INPUT(6),
            SA_INPUT(7),
            SA_INPUT(8),
            SA_INPUT(9),
            SA_INPUT(10),
            SA_INPUT(11),
            SA_INPUT(12),
            SA_INPUT(13),
            SA_INPUT(14),
            SA_INPUT(15),
            SA_COMMON
        };
    };

    struct spectrum_analyzer_x24_metadata: public spectrum_analyzer_base_metadata
    {
        static const plugin_metadata_t metadata;

        enum ports
        {
            SA_INPUT(0),
            SA_INPUT(1),
            SA_INPUT(2),
            SA_INPUT(3),
            SA_INPUT(4),
            SA_INPUT(5),
            SA_INPUT(6),
            SA_INPUT(7),
            SA_INPUT(8),
            SA_INPUT(9),
            SA_INPUT(10),
            SA_INPUT(11),
            SA_INPUT(12),
            SA_INPUT(13),
            SA_INPUT(14),
            SA_INPUT(15),
            SA_INPUT(16),
            SA_INPUT(17),
            SA_INPUT(18),
            SA_INPUT(19),
            SA_INPUT(20),
            SA_INPUT(21),
            SA_INPUT(22),
            SA_INPUT(23),
            SA_COMMON
        };
    };

    struct spectrum_analyzer_x32_metadata: public spectrum_analyzer_base_metadata
    {
        static const plugin_metadata_t metadata;

        enum ports
        {
            SA_INPUT(0),
            SA_INPUT(1),
            SA_INPUT(2),
            SA_INPUT(3),
            SA_INPUT(4),
            SA_INPUT(5),
            SA_INPUT(6),
            SA_INPUT(7),
            SA_INPUT(8),
            SA_INPUT(9),
            SA_INPUT(10),
            SA_INPUT(11),
            SA_INPUT(12),
            SA_INPUT(13),
            SA_INPUT(14),
            SA_INPUT(15),
            SA_INPUT(16),
            SA_INPUT(17),
            SA_INPUT(18),
            SA_INPUT(19),
            SA_INPUT(20),
            SA_INPUT(21),
            SA_INPUT(22),
            SA_INPUT(23),
            SA_INPUT(24),
            SA_INPUT(25),
            SA_INPUT(26),
            SA_INPUT(27),
            SA_INPUT(28),
            SA_INPUT(29),
            SA_INPUT(30),
            SA_INPUT(31),
            SA_COMMON
        };
    };

    #undef SA_INPUT
    #undef SA_COMMON

#ifndef LSP_NO_EXPERMIENTAL
    //-------------------------------------------------------------------------
    // Impulse responses metadata
    struct impulse_responses_base_metadata
    {
        static const float CONVOLUTION_TIME_MAX         = 10.0; // Max convolution time (in seconds)
        static const size_t CONVOLUTION_BUFFER_SIZE     = 4;    // The size of shift buffer relative to maximum convolution time

        static const float CONVLEN_MIN             =   0.0f;
        static const float CONVLEN_MAX             =   100.0f;
        static const float CONVLEN_DFL             =   100.0f;
        static const float CONVLEN_STEP            =   0.1f;
    };

    struct impulse_responses_metadata: public impulse_responses_base_metadata
    {
        static const plugin_metadata_t metadata;

        enum ports
        {
            // Input Audio
            IN,

            // Output Audio
            OUT,

            // Input controls
            BYPASS, FNAME, CHANNEL, LENGTH, DRY, WET, OUT_GAIN

            // Output controls/meters
        };
    };
#endif
}

#endif /* CORE_PLUGIN_METADATA_H_ */
