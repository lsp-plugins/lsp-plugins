/*
 * metadata.cpp
 *
 *  Created on: 16 окт. 2015 г.
 *      Author: sadko
 */

#include <core/plugin_metadata.h>
#include <stddef.h>

namespace lsp
{
    // Common port name definitions
    const char INPUT_L[]        = "in_l";
    const char INPUT_R[]        = "in_r";
    const char OUTPUT_L[]       = "out_l";
    const char OUTPUT_R[]       = "out_r";

    // Port groups
    const port_group_item_t stereo_out_group_ports[] =
    {
        { OUTPUT_L,     PGR_LEFT    },
        { OUTPUT_R,     PGR_RIGHT   },
        { NULL }
    };

    const port_group_item_t stereo_in_group_ports[] =
    {
        { INPUT_L,      PGR_LEFT    },
        { INPUT_R,      PGR_RIGHT   },
        { NULL }
    };

    const port_group_t stereo_plugin_groups[] =
    {
        { "stereo_in",  "Stereo Input",     GRP_STEREO,     PGF_IN,     stereo_in_group_ports       },
        { "stereo_out", "Stereo Output",    GRP_STEREO,     PGF_OUT,    stereo_out_group_ports      },
        { NULL, NULL }
    };

    // Phase detector
    static const port_t phase_detector_ports[] =
    {
        // Input audio ports
        { "in_a",           "Input A",          U_NONE,         R_AUDIO, F_IN, 0, 0, 0, 0, NULL    },
        { "in_b",           "Input B",          U_NONE,         R_AUDIO, F_IN, 0, 0, 0, 0, NULL    },

        // Output audio ports
        { "out_a",          "Output A",         U_NONE,         R_AUDIO, F_OUT, 0, 0, 0, 0, NULL   },
        { "out_b",          "Output B",         U_NONE,         R_AUDIO, F_OUT, 0, 0, 0, 0, NULL   },

        // Input controls
        { "bypass",         "Bypass",           U_BOOL,         R_CONTROL, F_IN, 0, 0, 0, 0, NULL },
        { "reset",          "Reset",            U_BOOL,         R_CONTROL, F_IN | F_TRG, 0, 0, 0, 0, NULL },
        { "time",           "Time",             U_MSEC,         R_CONTROL, F_IN | F_UPPER | F_LOWER | F_STEP | F_LOG,
                phase_detector_metadata::DETECT_TIME_MIN, phase_detector_metadata::DETECT_TIME_MAX, phase_detector_metadata::DETECT_TIME_DFL, phase_detector_metadata::DETECT_TIME_STEP, NULL },
        { "reactivity",     "Reactivity",       U_SEC,          R_CONTROL, F_IN | F_UPPER | F_LOWER | F_STEP | F_LOG,
                phase_detector_metadata::REACT_TIME_MIN, phase_detector_metadata::REACT_TIME_MAX, phase_detector_metadata::REACT_TIME_DFL, phase_detector_metadata::REACT_TIME_STEP, NULL },
        { "selector",       "Selector",         U_PERCENT,      R_CONTROL, F_IN | F_UPPER | F_LOWER | F_STEP,
                phase_detector_metadata::SELECTOR_MIN, phase_detector_metadata::SELECTOR_MAX, phase_detector_metadata::SELECTOR_DFL, phase_detector_metadata::SELECTOR_STEP, NULL },

        // Output controls
        { "best_time",      "Best time",        U_MSEC,         R_METER, F_OUT | F_UPPER | F_LOWER,
                phase_detector_metadata::TIME_MIN, phase_detector_metadata::TIME_MAX, 0, 0, NULL },
        { "best_samples",   "Best samples",     U_SAMPLES,      R_METER, F_OUT | F_UPPER | F_LOWER,
                phase_detector_metadata::SAMPLES_MIN, phase_detector_metadata::SAMPLES_MAX, 0, 0, NULL },
        { "best_distance",  "Best distance",    U_CM,           R_METER, F_OUT | F_UPPER | F_LOWER,
                phase_detector_metadata::DISTANCE_MIN, phase_detector_metadata::DISTANCE_MAX, 0, 0, NULL },
        { "best_value",     "Best value",       U_NONE,         R_METER, F_OUT | F_UPPER | F_LOWER, -1, 1, 0, 0, NULL },

        { "sel_time",       "Selected time",    U_MSEC,         R_METER, F_OUT | F_UPPER | F_LOWER,
                phase_detector_metadata::TIME_MIN, phase_detector_metadata::TIME_MAX, 0, 0, NULL },
        { "sel_samples",    "Selected samples", U_SAMPLES,      R_METER, F_OUT | F_UPPER | F_LOWER,
                phase_detector_metadata::SAMPLES_MIN, phase_detector_metadata::SAMPLES_MAX, 0, 0, NULL },
        { "sel_distance",   "Selected distance",U_CM,           R_METER, F_OUT | F_UPPER | F_LOWER,
                phase_detector_metadata::DISTANCE_MIN, phase_detector_metadata::DISTANCE_MAX, 0, 0, NULL },
        { "sel_value",      "Selected value",   U_NONE,         R_METER, F_OUT | F_UPPER | F_LOWER, -1, 1, 0, 0, NULL },

        { "worst_time",     "Worst time",       U_MSEC,         R_METER, F_OUT | F_UPPER | F_LOWER,
                phase_detector_metadata::TIME_MIN, phase_detector_metadata::TIME_MAX, 0, 0, NULL },
        { "worst_samples",  "Worst samples",    U_SAMPLES,      R_METER, F_OUT | F_UPPER | F_LOWER,
                phase_detector_metadata::SAMPLES_MIN, phase_detector_metadata::SAMPLES_MAX, 0, 0, NULL },
        { "worst_distance", "Worst distance",   U_CM,           R_METER, F_OUT | F_UPPER | F_LOWER,
                phase_detector_metadata::DISTANCE_MIN, phase_detector_metadata::DISTANCE_MAX, 0, 0, NULL },
        { "worst_value",    "Worst value",      U_NONE,         R_METER, F_OUT | F_UPPER | F_LOWER, -1, 1, 0, 0, NULL },

        { "function",       "Function",         U_NONE,         R_MESH,  F_OUT,
                0.0, 0.0, MAX_SAMPLE_RATE * phase_detector_metadata::DETECT_TIME_MAX * 0.001, 2, NULL },

        { NULL, NULL }
    };

    static const int phase_detector_classes[] = { C_UTILITY, C_ANALYSER, -1 };

    const plugin_metadata_t  phase_detector_metadata::metadata =
    {
        "Phasendetektor",
        "Phase Detector",
        "PD1",
        "Vladimir Sadovnikov",
        "jffz",
        phase_detector_classes,
        phase_detector_ports,
        NULL
    };

    // Compensation delay
    static const char *comp_delay_modes[] =
    {
        "Samples",
        "Distance",
        "Time",
        NULL
    };

    static const port_t comp_delay_mono_ports[] =
    {
        // Input audio ports
        { "in",             "Input",            U_NONE,         R_AUDIO, F_IN, 0, 0, 0, 0, NULL    },

        // Output audio ports
        { "out",            "Output",           U_NONE,         R_AUDIO, F_OUT, 0, 0, 0, 0, NULL   },

        // Input controls
        { "bypass",         "Bypass",           U_BOOL,         R_CONTROL, F_IN, 0, 0, 0, 0, NULL },
        { "mode",           "Mode",             U_ENUM,         R_CONTROL, F_IN | F_INT, 0, 0, 0, 0, comp_delay_modes },
        { "samples",        "Samples",          U_SAMPLES,      R_CONTROL, F_IN | F_INT | F_UPPER | F_LOWER | F_STEP,
                0, comp_delay_base_metadata::SAMPLES_MAX, 0, 1, NULL },
        { "meters",         "Meters",           U_M,            R_CONTROL, F_IN | F_INT | F_UPPER | F_LOWER | F_STEP,
                0, comp_delay_base_metadata::METERS_MAX, 0, 1, NULL },
        { "centimeters",    "Centimeters",      U_CM,           R_CONTROL, F_IN | F_UPPER | F_LOWER | F_STEP,
                0, comp_delay_base_metadata::CENTIMETERS_MAX, 0, comp_delay_base_metadata::CENTIMETERS_STEP, NULL },
        { "temperature",    "Temperature",      U_DEG_CEL,      R_CONTROL, F_IN | F_UPPER | F_LOWER | F_STEP,
                comp_delay_base_metadata::TEMPERATURE_MIN, comp_delay_base_metadata::TEMPERATURE_MAX, comp_delay_base_metadata::TEMPERATURE_DFL, comp_delay_base_metadata::CENTIMETERS_STEP, NULL },
        { "time",           "Time",             U_MSEC,         R_CONTROL, F_IN | F_LOG | F_UPPER | F_LOWER | F_STEP,
                0, comp_delay_base_metadata::TIME_MAX, 0, comp_delay_base_metadata::TIME_STEP, NULL },
        { "dry",            "Dry amount",       U_GAIN_AMP,     R_CONTROL, F_IN | F_LOG | F_UPPER | F_LOWER | F_STEP, 0, 10, 0, 0.1, NULL },
        { "wet",            "Wet amount",       U_GAIN_AMP,     R_CONTROL, F_IN | F_LOG | F_UPPER | F_LOWER | F_STEP, 0, 10, 1.0, 0.1, NULL },
        { "out_gain",       "Output gain",      U_GAIN_AMP,     R_CONTROL, F_IN | F_LOG | F_UPPER | F_LOWER | F_STEP, 0, 10, 1.0, 0.1, NULL },

        // Output controls
        { "del_time",       "Delay time",       U_MSEC,         R_METER, F_OUT | F_UPPER | F_LOWER,
                0, comp_delay_base_metadata::DELAY_OUT_MAX_TIME, 0, 0, NULL     },
        { "del_samples",    "Delay samples",    U_SAMPLES,      R_METER, F_OUT | F_UPPER | F_LOWER,
                0, comp_delay_base_metadata::DELAY_OUT_MAX_SAMPLES, 0, 0, NULL  },
        { "del_distance",   "Delay distance",   U_CM,           R_METER, F_OUT | F_UPPER | F_LOWER,
                0, comp_delay_base_metadata::DELAY_OUT_MAX_DISTANCE, 0, 0, NULL },

        { NULL, NULL }
    };

    static const port_t comp_delay_stereo_ports[] =
    {
        // Input audio ports
        { INPUT_L,          "Input L",          U_NONE,         R_AUDIO, F_IN, 0, 0, 0, 0, NULL    },
        { INPUT_R,          "Input R",          U_NONE,         R_AUDIO, F_IN, 0, 0, 0, 0, NULL    },

        // Output audio ports
        { OUTPUT_L,         "Output L",         U_NONE,         R_AUDIO, F_OUT, 0, 0, 0, 0, NULL   },
        { OUTPUT_R,         "Output R",         U_NONE,         R_AUDIO, F_OUT, 0, 0, 0, 0, NULL   },

        // Input controls
        { "bypass",         "Bypass",           U_BOOL,         R_CONTROL, F_IN, 0, 0, 0, 0, NULL },
        { "mode",           "Mode",             U_ENUM,         R_CONTROL, F_IN | F_INT, 0, 0, 0, 0, comp_delay_modes },
        { "samples",        "Samples",          U_SAMPLES,      R_CONTROL, F_IN | F_INT | F_UPPER | F_LOWER | F_STEP,
                0, comp_delay_base_metadata::SAMPLES_MAX, 0, 1, NULL },
        { "meters",         "Meters",           U_M,            R_CONTROL, F_IN | F_INT | F_UPPER | F_LOWER | F_STEP,
                0, comp_delay_base_metadata::METERS_MAX, 0, 1, NULL },
        { "centimeters",    "Centimeters",      U_CM,           R_CONTROL, F_IN | F_UPPER | F_LOWER | F_STEP,
                0, comp_delay_base_metadata::CENTIMETERS_MAX, 0, comp_delay_base_metadata::CENTIMETERS_STEP, NULL },
        { "temperature",    "Temperature",      U_DEG_CEL,      R_CONTROL, F_IN | F_UPPER | F_LOWER | F_STEP,
                comp_delay_base_metadata::TEMPERATURE_MIN, comp_delay_base_metadata::TEMPERATURE_MAX, comp_delay_base_metadata::TEMPERATURE_DFL, comp_delay_base_metadata::CENTIMETERS_STEP, NULL },
        { "time",           "Time",             U_MSEC,         R_CONTROL, F_IN | F_LOG | F_UPPER | F_LOWER | F_STEP,
                0, comp_delay_base_metadata::TIME_MAX, 0, comp_delay_base_metadata::TIME_STEP, NULL },
        { "dry",            "Dry amount",       U_GAIN_AMP,     R_CONTROL, F_IN | F_LOG | F_UPPER | F_LOWER | F_STEP, 0, 10, 0, 0.1, NULL },
        { "wet",            "Wet amount",       U_GAIN_AMP,     R_CONTROL, F_IN | F_LOG | F_UPPER | F_LOWER | F_STEP, 0, 10, 1.0, 0.1, NULL },
        { "out_gain",       "Output gain",      U_GAIN_AMP,     R_CONTROL, F_IN | F_LOG | F_UPPER | F_LOWER | F_STEP, 0, 10, 1.0, 0.1, NULL },

        // Output controls
        { "del_time",       "Delay time",       U_MSEC,         R_METER, F_OUT | F_UPPER | F_LOWER,
                0, comp_delay_base_metadata::DELAY_OUT_MAX_TIME, 0, 0, NULL     },
        { "del_samples",    "Delay samples",    U_SAMPLES,      R_METER, F_OUT | F_UPPER | F_LOWER,
                0, comp_delay_base_metadata::DELAY_OUT_MAX_SAMPLES, 0, 0, NULL  },
        { "del_distance",   "Delay distance",   U_CM,           R_METER, F_OUT | F_UPPER | F_LOWER,
                0, comp_delay_base_metadata::DELAY_OUT_MAX_DISTANCE, 0, 0, NULL },

        { NULL, NULL }
    };

    static const port_t comp_delay_x2_stereo_ports[] =
    {
        // Input audio ports
        { INPUT_L,          "Input L",          U_NONE,         R_AUDIO, F_IN, 0, 0, 0, 0, NULL    },
        { INPUT_R,          "Input R",          U_NONE,         R_AUDIO, F_IN, 0, 0, 0, 0, NULL    },

        // Output audio ports
        { OUTPUT_L,         "Output L",         U_NONE,         R_AUDIO, F_OUT, 0, 0, 0, 0, NULL   },
        { OUTPUT_R,         "Output R",         U_NONE,         R_AUDIO, F_OUT, 0, 0, 0, 0, NULL   },

        // Input controls
        { "bypass",         "Bypass",           U_BOOL,         R_CONTROL, F_IN, 0, 0, 0, 0, NULL },

        { "mode_l",         "Mode L",           U_ENUM,         R_CONTROL, F_IN | F_INT, 0, 0, 0, 0, comp_delay_modes },
        { "samples_l",      "Samples L",        U_SAMPLES,      R_CONTROL, F_IN | F_INT | F_UPPER | F_LOWER | F_STEP,
                0, comp_delay_base_metadata::SAMPLES_MAX, 0, 1, NULL },
        { "meters_l",       "Meters L",         U_M,            R_CONTROL, F_IN | F_INT | F_UPPER | F_LOWER | F_STEP,
                0, comp_delay_base_metadata::METERS_MAX, 0, 1, NULL },
        { "centimeters_l",  "Centimeters L",    U_CM,           R_CONTROL, F_IN | F_UPPER | F_LOWER | F_STEP,
                0, comp_delay_base_metadata::CENTIMETERS_MAX, 0, comp_delay_base_metadata::CENTIMETERS_STEP, NULL },
        { "temperature_l",  "Temperature L",    U_DEG_CEL,      R_CONTROL, F_IN | F_UPPER | F_LOWER | F_STEP,
                comp_delay_base_metadata::TEMPERATURE_MIN, comp_delay_base_metadata::TEMPERATURE_MAX, comp_delay_base_metadata::TEMPERATURE_DFL, comp_delay_base_metadata::CENTIMETERS_STEP, NULL },
        { "time_l",         "Time L",           U_MSEC,         R_CONTROL, F_IN | F_LOG | F_UPPER | F_LOWER | F_STEP,
                0, comp_delay_base_metadata::TIME_MAX, 0, comp_delay_base_metadata::TIME_STEP, NULL },
        { "dry_l",          "Dry amount L",     U_GAIN_AMP,     R_CONTROL, F_IN | F_LOG | F_UPPER | F_LOWER | F_STEP, 0, 10, 0, 0.1, NULL },
        { "wet_l",          "Wet amount L",     U_GAIN_AMP,     R_CONTROL, F_IN | F_LOG | F_UPPER | F_LOWER | F_STEP, 0, 10, 1.0, 0.1, NULL },

        { "mode_r",         "Mode R",           U_ENUM,         R_CONTROL, F_IN | F_INT, 0, 0, 0, 0, comp_delay_modes },
        { "samples_r",      "Samples R",        U_SAMPLES,      R_CONTROL, F_IN | F_INT | F_UPPER | F_LOWER | F_STEP,
                0, comp_delay_base_metadata::SAMPLES_MAX, 0, 1, NULL },
        { "meters_r",       "Meters R",         U_M,            R_CONTROL, F_IN | F_INT | F_UPPER | F_LOWER | F_STEP,
                0, comp_delay_base_metadata::METERS_MAX, 0, 1, NULL },
        { "centimeters_r",  "Centimeters R",    U_CM,           R_CONTROL, F_IN | F_UPPER | F_LOWER | F_STEP,
                0, comp_delay_base_metadata::CENTIMETERS_MAX, 0, comp_delay_base_metadata::CENTIMETERS_STEP, NULL },
        { "temperature_r",  "Temperature R",    U_DEG_CEL,      R_CONTROL, F_IN | F_UPPER | F_LOWER | F_STEP,
                comp_delay_base_metadata::TEMPERATURE_MIN, comp_delay_base_metadata::TEMPERATURE_MAX, comp_delay_base_metadata::TEMPERATURE_DFL, comp_delay_base_metadata::CENTIMETERS_STEP, NULL },
        { "time_r",         "Time R",           U_MSEC,         R_CONTROL, F_IN | F_LOG | F_UPPER | F_LOWER | F_STEP,
                0, comp_delay_base_metadata::TIME_MAX, 0, comp_delay_base_metadata::TIME_STEP, NULL },
        { "dry_r",          "Dry amount R",     U_GAIN_AMP,     R_CONTROL, F_IN | F_LOG | F_UPPER | F_LOWER | F_STEP, 0, 10, 0, 0.1, NULL },
        { "wet_r",          "Wet amount R",     U_GAIN_AMP,     R_CONTROL, F_IN | F_LOG | F_UPPER | F_LOWER | F_STEP, 0, 10, 1.0, 0.1, NULL },

        { "out_gain",       "Output gain",      U_GAIN_AMP,     R_CONTROL, F_IN | F_LOG | F_UPPER | F_LOWER | F_STEP, 0, 10, 1.0, 0.1, NULL },

        // Output controls
        { "del_time_l",     "Delay time L",     U_MSEC,         R_METER, F_OUT | F_UPPER | F_LOWER,
                0, comp_delay_base_metadata::DELAY_OUT_MAX_TIME, 0, 0, NULL     },
        { "del_samples_l",  "Delay samples L",  U_SAMPLES,      R_METER, F_OUT | F_UPPER | F_LOWER,
                0, comp_delay_base_metadata::DELAY_OUT_MAX_SAMPLES, 0, 0, NULL  },
        { "del_distance_l", "Delay distance L", U_CM,           R_METER, F_OUT | F_UPPER | F_LOWER,
                0, comp_delay_base_metadata::DELAY_OUT_MAX_DISTANCE, 0, 0, NULL },

        { "del_time_r",     "Delay time R",     U_MSEC,         R_METER, F_OUT | F_UPPER | F_LOWER,
                0, comp_delay_base_metadata::DELAY_OUT_MAX_TIME, 0, 0, NULL },
        { "del_samples_r",  "Delay samples R",  U_SAMPLES,      R_METER, F_OUT | F_UPPER | F_LOWER,
                0, comp_delay_base_metadata::DELAY_OUT_MAX_SAMPLES, 0, 0, NULL },
        { "del_distance_r", "Delay distance R", U_CM,           R_METER, F_OUT | F_UPPER | F_LOWER,
                0, comp_delay_base_metadata::DELAY_OUT_MAX_DISTANCE, 0, 0, NULL },

        { NULL, NULL }
    };

    static const int comp_delay_classes[] = { C_DELAY, -1 };

    const plugin_metadata_t  comp_delay_mono_metadata::metadata =
    {
        "Verzögerungsausgleicher Mono",
        "Delay Compensator Mono",
        "VA1M",
        "Vladimir Sadovnikov",
        "jav8",
        comp_delay_classes,
        comp_delay_mono_ports,
        NULL
    };

    const plugin_metadata_t  comp_delay_stereo_metadata::metadata =
    {
        "Verzögerungsausgleicher Stereo",
        "Delay Compensator Stereo",
        "VA1S",
        "Vladimir Sadovnikov",
        "qpwr",
        comp_delay_classes,
        comp_delay_stereo_ports,
        stereo_plugin_groups
    };

    const plugin_metadata_t  comp_delay_x2_stereo_metadata::metadata =
    {
        "Verzögerungsausgleicher X2 Stereo",
        "Delay Compensator X2 Stereo",
        "VA2S",
        "Vladimir Sadovnikov",
        "fwd3",
        comp_delay_classes,
        comp_delay_x2_stereo_ports,
        stereo_plugin_groups
    };
}



