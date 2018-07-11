/*
 * comp_delay.cpp
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
        AUDIO_INPUT_MONO,

        // Output audio ports
        AUDIO_OUTPUT_MONO,

        // Input controls
        BYPASS,

        { "mode",           "Mode",             U_ENUM,         R_CONTROL, F_IN | F_INT, 0, 0, 0, 0, comp_delay_modes },
        { "samp",           "Samples",          U_SAMPLES,      R_CONTROL, F_IN | F_INT | F_UPPER | F_LOWER | F_STEP,
                0, comp_delay_base_metadata::SAMPLES_MAX, 0, 1, NULL },
        { "m",              "Meters",           U_M,            R_CONTROL, F_IN | F_INT | F_UPPER | F_LOWER | F_STEP,
                0, comp_delay_base_metadata::METERS_MAX, 0, 1, NULL },
        { "cm",             "Centimeters",      U_CM,           R_CONTROL, F_IN | F_UPPER | F_LOWER | F_STEP,
                0, comp_delay_base_metadata::CENTIMETERS_MAX, 0, comp_delay_base_metadata::CENTIMETERS_STEP, NULL },
        { "t",              "Temperature",      U_DEG_CEL,      R_CONTROL, F_IN | F_UPPER | F_LOWER | F_STEP,
                comp_delay_base_metadata::TEMPERATURE_MIN, comp_delay_base_metadata::TEMPERATURE_MAX, comp_delay_base_metadata::TEMPERATURE_DFL, comp_delay_base_metadata::CENTIMETERS_STEP, NULL },
        { "time",           "Time",             U_MSEC,         R_CONTROL, F_IN | F_LOG | F_UPPER | F_LOWER | F_STEP,
                0, comp_delay_base_metadata::TIME_MAX, 0, comp_delay_base_metadata::TIME_STEP, NULL },
        DRY_GAIN(0.0f),
        WET_GAIN(1.0f),

        OUT_GAIN,

        // Output controls
        { "d_t",            "Delay time",       U_MSEC,         R_METER, F_OUT | F_UPPER | F_LOWER,
                0, comp_delay_base_metadata::DELAY_OUT_MAX_TIME, 0, 0, NULL     },
        { "d_s",            "Delay samples",    U_SAMPLES,      R_METER, F_OUT | F_UPPER | F_LOWER,
                0, comp_delay_base_metadata::DELAY_OUT_MAX_SAMPLES, 0, 0, NULL  },
        { "d_d",            "Delay distance",   U_CM,           R_METER, F_OUT | F_UPPER | F_LOWER,
                0, comp_delay_base_metadata::DELAY_OUT_MAX_DISTANCE, 0, 0, NULL },

        PORTS_END
    };

    static const port_t comp_delay_stereo_ports[] =
    {
        // Input audio ports
        AUDIO_INPUT_LEFT,
        AUDIO_INPUT_RIGHT,

        // Output audio ports
        AUDIO_OUTPUT_LEFT,
        AUDIO_OUTPUT_RIGHT,

        // Input controls
        BYPASS,

        { "mode",           "Mode",             U_ENUM,         R_CONTROL, F_IN | F_INT, 0, 0, 0, 0, comp_delay_modes },
        { "samp",           "Samples",          U_SAMPLES,      R_CONTROL, F_IN | F_INT | F_UPPER | F_LOWER | F_STEP,
                0, comp_delay_base_metadata::SAMPLES_MAX, 0, 1, NULL },
        { "m",              "Meters",           U_M,            R_CONTROL, F_IN | F_INT | F_UPPER | F_LOWER | F_STEP,
                0, comp_delay_base_metadata::METERS_MAX, 0, 1, NULL },
        { "cm",             "Centimeters",      U_CM,           R_CONTROL, F_IN | F_UPPER | F_LOWER | F_STEP,
                0, comp_delay_base_metadata::CENTIMETERS_MAX, 0, comp_delay_base_metadata::CENTIMETERS_STEP, NULL },
        { "t",              "Temperature",      U_DEG_CEL,      R_CONTROL, F_IN | F_UPPER | F_LOWER | F_STEP,
                comp_delay_base_metadata::TEMPERATURE_MIN, comp_delay_base_metadata::TEMPERATURE_MAX, comp_delay_base_metadata::TEMPERATURE_DFL, comp_delay_base_metadata::CENTIMETERS_STEP, NULL },
        { "time",           "Time",             U_MSEC,         R_CONTROL, F_IN | F_LOG | F_UPPER | F_LOWER | F_STEP,
                0, comp_delay_base_metadata::TIME_MAX, 0, comp_delay_base_metadata::TIME_STEP, NULL },
        DRY_GAIN(0.0f),
        WET_GAIN(1.0f),

        OUT_GAIN,

        // Output controls
        { "d_t",            "Delay time",       U_MSEC,         R_METER, F_OUT | F_UPPER | F_LOWER,
                0, comp_delay_base_metadata::DELAY_OUT_MAX_TIME, 0, 0, NULL     },
        { "d_s",            "Delay samples",    U_SAMPLES,      R_METER, F_OUT | F_UPPER | F_LOWER,
                0, comp_delay_base_metadata::DELAY_OUT_MAX_SAMPLES, 0, 0, NULL  },
        { "d_d",            "Delay distance",   U_CM,           R_METER, F_OUT | F_UPPER | F_LOWER,
                0, comp_delay_base_metadata::DELAY_OUT_MAX_DISTANCE, 0, 0, NULL },

        PORTS_END
    };

    static const port_t comp_delay_x2_stereo_ports[] =
    {
        // Input audio ports
        AUDIO_INPUT_LEFT,
        AUDIO_INPUT_RIGHT,

        // Output audio ports
        AUDIO_OUTPUT_LEFT,
        AUDIO_OUTPUT_RIGHT,

        // Input controls
        BYPASS,

        { "mode_l",         "Mode L",           U_ENUM,         R_CONTROL, F_IN | F_INT, 0, 0, 0, 0, comp_delay_modes },
        { "samp_l",         "Samples L",        U_SAMPLES,      R_CONTROL, F_IN | F_INT | F_UPPER | F_LOWER | F_STEP,
                0, comp_delay_base_metadata::SAMPLES_MAX, 0, 1, NULL },
        { "m_l",            "Meters L",         U_M,            R_CONTROL, F_IN | F_INT | F_UPPER | F_LOWER | F_STEP,
                0, comp_delay_base_metadata::METERS_MAX, 0, 1, NULL },
        { "cm_l",           "Centimeters L",    U_CM,           R_CONTROL, F_IN | F_UPPER | F_LOWER | F_STEP,
                0, comp_delay_base_metadata::CENTIMETERS_MAX, 0, comp_delay_base_metadata::CENTIMETERS_STEP, NULL },
        { "t_l",            "Temperature L",    U_DEG_CEL,      R_CONTROL, F_IN | F_UPPER | F_LOWER | F_STEP,
                comp_delay_base_metadata::TEMPERATURE_MIN, comp_delay_base_metadata::TEMPERATURE_MAX, comp_delay_base_metadata::TEMPERATURE_DFL, comp_delay_base_metadata::CENTIMETERS_STEP, NULL },
        { "time_l",         "Time L",           U_MSEC,         R_CONTROL, F_IN | F_LOG | F_UPPER | F_LOWER | F_STEP,
                0, comp_delay_base_metadata::TIME_MAX, 0, comp_delay_base_metadata::TIME_STEP, NULL },
        DRY_GAIN_L(0.0f),
        WET_GAIN_L(1.0f),

        { "mode_r",         "Mode R",           U_ENUM,         R_CONTROL, F_IN | F_INT, 0, 0, 0, 0, comp_delay_modes },
        { "samp_r",         "Samples R",        U_SAMPLES,      R_CONTROL, F_IN | F_INT | F_UPPER | F_LOWER | F_STEP,
                0, comp_delay_base_metadata::SAMPLES_MAX, 0, 1, NULL },
        { "m_r",            "Meters R",         U_M,            R_CONTROL, F_IN | F_INT | F_UPPER | F_LOWER | F_STEP,
                0, comp_delay_base_metadata::METERS_MAX, 0, 1, NULL },
        { "cm_r",           "Centimeters R",    U_CM,           R_CONTROL, F_IN | F_UPPER | F_LOWER | F_STEP,
                0, comp_delay_base_metadata::CENTIMETERS_MAX, 0, comp_delay_base_metadata::CENTIMETERS_STEP, NULL },
        { "t_r",            "Temperature R",    U_DEG_CEL,      R_CONTROL, F_IN | F_UPPER | F_LOWER | F_STEP,
                comp_delay_base_metadata::TEMPERATURE_MIN, comp_delay_base_metadata::TEMPERATURE_MAX, comp_delay_base_metadata::TEMPERATURE_DFL, comp_delay_base_metadata::CENTIMETERS_STEP, NULL },
        { "time_r",         "Time R",           U_MSEC,         R_CONTROL, F_IN | F_LOG | F_UPPER | F_LOWER | F_STEP,
                0, comp_delay_base_metadata::TIME_MAX, 0, comp_delay_base_metadata::TIME_STEP, NULL },
        DRY_GAIN_R(0.0f),
        WET_GAIN_R(1.0f),

        OUT_GAIN,

        // Output controls
        { "d_t_l",          "Delay time L",     U_MSEC,         R_METER, F_OUT | F_UPPER | F_LOWER,
                0, comp_delay_base_metadata::DELAY_OUT_MAX_TIME, 0, 0, NULL     },
        { "d_s_l",          "Delay samples L",  U_SAMPLES,      R_METER, F_OUT | F_UPPER | F_LOWER,
                0, comp_delay_base_metadata::DELAY_OUT_MAX_SAMPLES, 0, 0, NULL  },
        { "d_d_l",          "Delay distance L", U_CM,           R_METER, F_OUT | F_UPPER | F_LOWER,
                0, comp_delay_base_metadata::DELAY_OUT_MAX_DISTANCE, 0, 0, NULL },

        { "d_t_r",          "Delay time R",     U_MSEC,         R_METER, F_OUT | F_UPPER | F_LOWER,
                0, comp_delay_base_metadata::DELAY_OUT_MAX_TIME, 0, 0, NULL },
        { "d_s_r",          "Delay samples R",  U_SAMPLES,      R_METER, F_OUT | F_UPPER | F_LOWER,
                0, comp_delay_base_metadata::DELAY_OUT_MAX_SAMPLES, 0, 0, NULL },
        { "d_d_r",          "Delay distance R", U_CM,           R_METER, F_OUT | F_UPPER | F_LOWER,
                0, comp_delay_base_metadata::DELAY_OUT_MAX_DISTANCE, 0, 0, NULL },

        PORTS_END
    };

    static const int comp_delay_classes[] = { C_DELAY, -1 };

    const plugin_metadata_t  comp_delay_mono_metadata::metadata =
    {
        "Verzögerungsausgleicher Mono",
        "Delay Compensator Mono",
        "VA1M",
        &developers::v_sadovnikov,
        "comp_delay_mono",
        "jav8",
        LSP_LADSPA_BASE + 1,
        LSP_VERSION(1, 0, 0),
        comp_delay_classes,
        comp_delay_mono_ports,
        NULL
    };

    const plugin_metadata_t  comp_delay_stereo_metadata::metadata =
    {
        "Verzögerungsausgleicher Stereo",
        "Delay Compensator Stereo",
        "VA1S",
        &developers::v_sadovnikov,
        "comp_delay_stereo",
        "qpwr",
        LSP_LADSPA_BASE + 2,
        LSP_VERSION(1, 0, 0),
        comp_delay_classes,
        comp_delay_stereo_ports,
        stereo_plugin_port_groups
    };

    const plugin_metadata_t  comp_delay_x2_stereo_metadata::metadata =
    {
        "Verzögerungsausgleicher x2 Stereo",
        "Delay Compensator x2 Stereo",
        "VA2S",
        &developers::v_sadovnikov,
        "comp_delay_x2_stereo",
        "fwd3",
        LSP_LADSPA_BASE + 3,
        LSP_VERSION(1, 0, 0),
        comp_delay_classes,
        comp_delay_x2_stereo_ports,
        stereo_plugin_port_groups
    };

}



