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

        static const float TIME_DIVISION_MAX = 0.02f;
        static const float TIME_DIVISION_MIN = 1.0f / 192000.0f;
        static const float TIME_DIVISION_DFL = 1e-3f;
        static const float TIME_DIVISION_STEP = 1e-3f;

        static const float TIME_POSITION_MAX = 100.0f;
        static const float TIME_POSITION_MIN = -100.0f;
        static const float TIME_POSITION_DFL = 0.0f;
        static const float TIME_POSITION_STEP = 1.0f;

        static const float VERTICAL_DIVISION_MAX = 1.0e-3f;
        static const float VERTICAL_DIVISION_MIN = 10.0f;
        static const float VERTICAL_DIVISION_DFL = 1.0;
        static const float VERTICAL_DIVISION_STEP = 1e-3;

        static const float VERTICAL_POSITION_MAX = 100.0f;
        static const float VERTICAL_POSITION_MIN = -100.0f;
        static const float VERTICAL_POSITION_DFL = 0.0f;
        static const float VERTICAL_POSITION_STEP = 1.0f;

        enum trigger_mode_selector_t
        {
            TRIGGER_MODE_NONE,
            TRIGGER_MODE_SINGLE,
            TRIGGER_MODE_REPEAT,

            TRIGGER_MODE_DFL = TRIGGER_MODE_REPEAT
        };

        enum trigger_type_selector_t
        {
            TRIGGER_TYPE_RISING_EDGE,
            TRIGGER_TYPE_FALLING_EDGE,

            TRIGGER_TYPE_DFL = TRIGGER_TYPE_RISING_EDGE
        };

        static const float TRIGGER_HYSTERESIS_MAX = 100.0f;
        static const float TRIGGER_HYSTERESIS_MIN = 0.0f;
        static const float TRIGGER_HYSTERESIS_DFL = 1.0f;
        static const float TRIGGER_HYSTERESIS_STEP = 1.0f;

        static const float TRIGGER_LEVEL_MAX = 10.0f;
        static const float TRIGGER_LEVEL_MIN = -10.0f;
        static const float TRIGGER_LEVEL_DFL = 0.0f;
        static const float TRIGGER_LEVEL_STEP = 1.0f;

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

//    struct oscilloscope_x4_metadata: public oscilloscope_base_metadata
//    {
//        static const plugin_metadata_t metadata;
//    };
}

#endif /* METADATA_OSCILLOSCOPE_H_ */
