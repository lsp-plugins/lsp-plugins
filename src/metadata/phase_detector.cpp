/*
 * phase_detector.cpp
 *
 *  Created on: 08 апр. 2016 г.
 *      Author: sadko
 */

#include <metadata/plugins.h>
#include <metadata/ports.h>
#include <metadata/developers.h>

namespace lsp
{
    //-------------------------------------------------------------------------
    // Phase detector
    static const port_t phase_detector_ports[] =
    {
        // Input audio ports
        AUDIO_INPUT_A,
        AUDIO_INPUT_B,

        // Output audio ports
        AUDIO_OUTPUT_A,
        AUDIO_OUTPUT_B,

        // Input controls
        BYPASS,
        TRIGGER("reset", "Reset"),
        { "time",           "Time",             U_MSEC,         R_CONTROL, F_IN | F_UPPER | F_LOWER | F_STEP | F_LOG,
                phase_detector_metadata::DETECT_TIME_MIN, phase_detector_metadata::DETECT_TIME_MAX, phase_detector_metadata::DETECT_TIME_DFL, phase_detector_metadata::DETECT_TIME_STEP, NULL },
        { "react",          "Reactivity",       U_SEC,          R_CONTROL, F_IN | F_UPPER | F_LOWER | F_STEP | F_LOG,
                phase_detector_metadata::REACT_TIME_MIN, phase_detector_metadata::REACT_TIME_MAX, phase_detector_metadata::REACT_TIME_DFL, phase_detector_metadata::REACT_TIME_STEP, NULL },
        { "sel",            "Selector",         U_PERCENT,      R_CONTROL, F_IN | F_UPPER | F_LOWER | F_STEP,
                phase_detector_metadata::SELECTOR_MIN, phase_detector_metadata::SELECTOR_MAX, phase_detector_metadata::SELECTOR_DFL, phase_detector_metadata::SELECTOR_STEP, NULL },

        // Output controls
        { "b_t",            "Best time",        U_MSEC,         R_METER, F_OUT | F_UPPER | F_LOWER,
                phase_detector_metadata::TIME_MIN, phase_detector_metadata::TIME_MAX, 0, 0, NULL },
        { "b_s",            "Best samples",     U_SAMPLES,      R_METER, F_OUT | F_UPPER | F_LOWER,
                phase_detector_metadata::SAMPLES_MIN, phase_detector_metadata::SAMPLES_MAX, 0, 0, NULL },
        { "b_d",            "Best distance",    U_CM,           R_METER, F_OUT | F_UPPER | F_LOWER,
                phase_detector_metadata::DISTANCE_MIN, phase_detector_metadata::DISTANCE_MAX, 0, 0, NULL },
        { "b_v",            "Best value",       U_NONE,         R_METER, F_OUT | F_UPPER | F_LOWER, -1, 1, 0, 0, NULL },

        { "s_t",            "Selected time",    U_MSEC,         R_METER, F_OUT | F_UPPER | F_LOWER,
                phase_detector_metadata::TIME_MIN, phase_detector_metadata::TIME_MAX, 0, 0, NULL },
        { "s_s",            "Selected samples", U_SAMPLES,      R_METER, F_OUT | F_UPPER | F_LOWER,
                phase_detector_metadata::SAMPLES_MIN, phase_detector_metadata::SAMPLES_MAX, 0, 0, NULL },
        { "s_d",            "Selected distance",U_CM,           R_METER, F_OUT | F_UPPER | F_LOWER,
                phase_detector_metadata::DISTANCE_MIN, phase_detector_metadata::DISTANCE_MAX, 0, 0, NULL },
        { "s_v",            "Selected value",   U_NONE,         R_METER, F_OUT | F_UPPER | F_LOWER, -1, 1, 0, 0, NULL },

        { "w_t",            "Worst time",       U_MSEC,         R_METER, F_OUT | F_UPPER | F_LOWER,
                phase_detector_metadata::TIME_MIN, phase_detector_metadata::TIME_MAX, 0, 0, NULL },
        { "w_s",            "Worst samples",    U_SAMPLES,      R_METER, F_OUT | F_UPPER | F_LOWER,
                phase_detector_metadata::SAMPLES_MIN, phase_detector_metadata::SAMPLES_MAX, 0, 0, NULL },
        { "w_d",            "Worst distance",   U_CM,           R_METER, F_OUT | F_UPPER | F_LOWER,
                phase_detector_metadata::DISTANCE_MIN, phase_detector_metadata::DISTANCE_MAX, 0, 0, NULL },
        { "w_v",            "Worst value",      U_NONE,         R_METER, F_OUT | F_UPPER | F_LOWER, -1, 1, 0, 0, NULL },
        MESH("f", "Function", 2, phase_detector_metadata::MESH_POINTS),
        PORTS_END
    };

    static const int phase_detector_classes[] = { C_ANALYSER, -1 };

    const plugin_metadata_t  phase_detector_metadata::metadata =
    {
        "Phasendetektor",
        "Phase Detector",
        "PD1",
        &developers::v_sadovnikov,
        "phase_detector",
        "jffz",
        LSP_PHASE_DETECTOR_BASE,
        LSP_VERSION(1, 0, 0),
        phase_detector_classes,
        E_INLINE_DISPLAY,
        phase_detector_ports,
        "util/phase_detector.xml",
        NULL,
        NULL
    };

}



