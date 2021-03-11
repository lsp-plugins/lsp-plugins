/*
 * oscilloscope.h
 *
 *  Created on: 26 Jan 2020
 *      Author: crocoduck
 */

#ifndef METADATA_OSCILLOSCOPE_H_
#define METADATA_OSCILLOSCOPE_H_

namespace lsp
{
    struct oscilloscope_base_metadata
    {
        static const plugin_metadata_t metadata;

        static const float HORIZONTAL_DIVISION_MAX      = 10.0f;
        static const float HORIZONTAL_DIVISION_MIN      = 1.0e-3f;
        static const float HORIZONTAL_DIVISION_DFL      = 0.5;
        static const float HORIZONTAL_DIVISION_STEP     = 1e-3f;

        static const float TIME_DIVISION_MAX            = 50.0f;
        static const float TIME_DIVISION_MIN            = 0.05f;
        static const float TIME_DIVISION_DFL            = 1.0f;
        static const float TIME_DIVISION_STEP           = 0.01f;

        static const float TIME_POSITION_MAX            = 100.0f;
        static const float TIME_POSITION_MIN            = -100.0f;
        static const float TIME_POSITION_DFL            = 0.0f;
        static const float TIME_POSITION_STEP           = 0.1f;

        static const float VERTICAL_DIVISION_MAX        = 10.0f;
        static const float VERTICAL_DIVISION_MIN        = 1.0e-3f;
        static const float VERTICAL_DIVISION_DFL        = 0.5;
        static const float VERTICAL_DIVISION_STEP       = 1e-3f;

        static const float VERTICAL_POSITION_MAX        = 100.0f;
        static const float VERTICAL_POSITION_MIN        = -100.0f;
        static const float VERTICAL_POSITION_DFL        = 0.0f;
        static const float VERTICAL_POSITION_STEP       = 0.1f;

        static const size_t STROBE_HISTORY_MAX          = 10;
        static const size_t STROBE_HISTORY_MIN          = 0;
        static const size_t STROBE_HISTORY_DFL          = 0;
        static const size_t STROBE_HISTORY_STEP         = 1;

        static const float XY_RECORD_TIME_MAX           = 50.0f;
        static const float XY_RECORD_TIME_MIN           = 1.0f;
        static const float XY_RECORD_TIME_DFL           = 10.0f;
        static const float XY_RECORD_TIME_STEP          = 0.01f;

        static const float MAXDOTS_MAX                  = 16384.0f;
        static const float MAXDOTS_MIN                  = 512.0f;
        static const float MAXDOTS_DFL                  = 8192.0f;
        static const float MAXDOTS_STEP                 = 0.01f;

        enum oversampler_mode_selector_t
        {
            OSC_OVS_NONE,
            OSC_OVS_2X,
            OSC_OVS_3X,
            OSC_OVS_4X,
            OSC_OVS_6X,
            OSC_OVS_8X,

            OSC_OVS_DFL = OSC_OVS_8X
        };

        enum mode_selector_t
        {
            MODE_XY,
            MODE_TRIGGERED,
            MODE_GONIOMETER,

            MODE_DFL = MODE_TRIGGERED
        };

        enum sweep_type_selector_t
        {
            SWEEP_TYPE_SAWTOOTH,
            SWEEP_TYPE_TRIANGULAR,
            SWEEP_TYPE_SINE,

            SWEEP_TYPE_DFL = SWEEP_TYPE_SAWTOOTH
        };

        enum trigger_input_selector_t
        {
            TRIGGER_INPUT_Y,
            TRIGGER_INPUT_EXT,

            TRIGGER_INPUT_DFL = TRIGGER_INPUT_Y
        };

        enum trigger_mode_selector_t
        {
            TRIGGER_MODE_SINGLE,
            TRIGGER_MODE_MANUAL,
            TRIGGER_MODE_REPEAT,

            TRIGGER_MODE_DFL = TRIGGER_MODE_REPEAT
        };

        enum trigger_type_selector_t
        {
            TRIGGER_TYPE_NONE,
            TRIGGER_TYPE_SIMPLE_RISING_EDGE,
            TRIGGER_TYPE_SIMPE_FALLING_EDGE,
            TRIGGER_TYPE_ADVANCED_RISING_EDGE,
            TRIGGER_TYPE_ADVANCED_FALLING_EDGE,

            TRIGGER_TYPE_DFL = TRIGGER_TYPE_ADVANCED_RISING_EDGE
        };

        static const float TRIGGER_HYSTERESIS_MAX = 100.0f;
        static const float TRIGGER_HYSTERESIS_MIN = 0.0f;
        static const float TRIGGER_HYSTERESIS_DFL = 1.0f;
        static const float TRIGGER_HYSTERESIS_STEP = 0.01f;

        static const float TRIGGER_LEVEL_MAX = 100.0f;
        static const float TRIGGER_LEVEL_MIN = -100.0f;
        static const float TRIGGER_LEVEL_DFL = 0.0f;
        static const float TRIGGER_LEVEL_STEP = 0.01f;

        static const float TRIGGER_HOLD_TIME_MAX = 60.0f;
        static const float TRIGGER_HOLD_TIME_MIN = 0.0f;
        static const float TRIGGER_HOLD_TIME_DFL = 0.0f;
        static const float TRIGGER_HOLD_TIME_STEP = 0.001f;

        enum coupling_type_t
        {
            COUPLING_AC,
            COUPLING_DC,

            COUPLING_DFL = COUPLING_DC
        };

        static const size_t SCOPE_MESH_SIZE = 512;
    };

    struct oscilloscope_x1_metadata: public oscilloscope_base_metadata
    {
        static const plugin_metadata_t metadata;
    };

    struct oscilloscope_x2_metadata: public oscilloscope_base_metadata
    {
        static const plugin_metadata_t metadata;
    };

    struct oscilloscope_x4_metadata: public oscilloscope_base_metadata
    {
        static const plugin_metadata_t metadata;
    };
}

#endif /* METADATA_OSCILLOSCOPE_H_ */
