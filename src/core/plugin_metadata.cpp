/*
 * metadata.cpp
 *
 *  Created on: 16 окт. 2015 г.
 *      Author: sadko
 */

#include <core/plugin_metadata.h>
#include <core/windows.h>
#include <stddef.h>

namespace lsp
{
    //-------------------------------------------------------------------------
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

    static const char *file_channels[] =
    {
        "1",
        "2",
        "3",
        "4",
        "5",
        "6",
        "7",
        "8",
        NULL
    };

    // Port pre-definitions
    #define AUDIO_INPUT(id, label) \
        { id, label, U_NONE, R_AUDIO, F_IN, 0, 0, 0, 0, NULL    }
    #define AUDIO_OUTPUT(id, label) \
        { id, label, U_NONE, R_AUDIO, F_OUT, 0, 0, 0, 0, NULL    }
    #define FILE_CHANNEL(id, label) \
        { id, label, U_ENUM, R_CONTROL, F_IN | F_INT, 0, 0, 0, 0, file_channels }
    #define AMP_GAIN(id, label, dfl, max) \
        { id, label, U_GAIN_AMP, R_CONTROL, F_IN | F_LOG | F_UPPER | F_LOWER | F_STEP, 0, max, dfl, 0.1, NULL }
    #define AMP_GAIN10(id, label, dfl)  AMP_GAIN(id, label, dfl, 10.0f)
    #define PORTS_END   \
        { NULL, NULL }

    // Reduced ports
    #define AUDIO_INPUT_MONO    AUDIO_INPUT("in", "Input")
    #define AUDIO_INPUT_LEFT    AUDIO_INPUT("in_l", "Input L")
    #define AUDIO_INPUT_RIGHT   AUDIO_INPUT("in_r", "Input R")
    #define AUDIO_INPUT_A       AUDIO_INPUT("in_a", "Input A")
    #define AUDIO_INPUT_B       AUDIO_INPUT("in_b", "Input B")
    #define AUDIO_INPUT_N(n)    AUDIO_INPUT("in" #n, "Input " #n)

    #define AUDIO_OUTPUT_MONO   AUDIO_OUTPUT("out", "Output")
    #define AUDIO_OUTPUT_LEFT   AUDIO_OUTPUT("out_l", "Output L")
    #define AUDIO_OUTPUT_RIGHT  AUDIO_OUTPUT("out_r", "Output R")
    #define AUDIO_OUTPUT_A      AUDIO_OUTPUT("out_a", "Output A")
    #define AUDIO_OUTPUT_B      AUDIO_OUTPUT("out_b", "Output B")
    #define AUDIO_OUTPUT_N(n)   AUDIO_OUTPUT("out" #n, "Output " #n)

    #define OUT_GAIN            AMP_GAIN10("g_out", "Output gain", 1.0f)

    #define DRY_GAIN            AMP_GAIN10("dry", "Dry amount", 0.0f)
    #define DRY_GAIN_L          AMP_GAIN10("dry_l", "Dry amount L", 0.0f)
    #define DRY_GAIN_R          AMP_GAIN10("dry_r", "Dry amount R", 0.0f)

    #define WET_GAIN            AMP_GAIN10("wet", "Wet amount", 1.0f)
    #define WET_GAIN_L          AMP_GAIN10("wet_l", "Wet amount L", 1.0f)
    #define WET_GAIN_R          AMP_GAIN10("wet_r", "Wet amount R", 1.0f)

    #define BYPASS              { "bypass",         "Bypass",           U_BOOL,         R_CONTROL, F_IN, 0, 0, 0, 0, NULL }

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
        { "reset",          "Reset",            U_BOOL,         R_CONTROL, F_IN | F_TRG, 0, 0, 0, 0, NULL },
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

        { "f",              "Function",         U_NONE,         R_MESH,  F_OUT,
                0.0, 0.0, phase_detector_metadata::MESH_POINTS, 2, NULL },

        PORTS_END
    };

    static const int phase_detector_classes[] = { C_ANALYSER, C_UTILITY, -1 };

    const plugin_metadata_t  phase_detector_metadata::metadata =
    {
        "Phasendetektor",
        "Phase Detector",
        "PD1",
        "Vladimir Sadovnikov",
        "jffz",
        LSP_VERSION(1, 0, 0),
        phase_detector_classes,
        phase_detector_ports,
        NULL
    };

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
        DRY_GAIN,
        WET_GAIN,

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
        DRY_GAIN,
        WET_GAIN,

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
        DRY_GAIN_L,
        WET_GAIN_L,

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
        DRY_GAIN_R,
        WET_GAIN_R,

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
        "Vladimir Sadovnikov",
        "jav8",
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
        "Vladimir Sadovnikov",
        "qpwr",
        LSP_VERSION(1, 0, 0),
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
        LSP_VERSION(1, 0, 0),
        comp_delay_classes,
        comp_delay_x2_stereo_ports,
        stereo_plugin_groups
    };

    //-------------------------------------------------------------------------
    // Spectral analyzer: x1, x8, x12, x16, x24, x32
    static const int spectrum_analyzer_classes[] = { C_ANALYSER, C_UTILITY, C_SPECTRAL, -1 };

    static const char *fft_tolerance[] =
    {
        "1024",
        "2048",
        "4096",
        "8192",
        "16384",
        NULL
    };

    static const char *spectrum_analyzer_x1_channels[]=
    {
        "0",
        NULL
    };

    static const char *spectrum_analyzer_x2_channels[]=
    {
        "0",
        "1",
        NULL
    };

    static const char *spectrum_analyzer_x4_channels[]=
    {
        "0", "1", "2", "3",
        NULL
    };

    static const char *spectrum_analyzer_x8_channels[]=
    {
        "0", "1", "2", "3", "4", "5", "6", "7",
        NULL
    };

    static const char *spectrum_analyzer_x12_channels[]=
    {
        "0", "1", "2", "3", "4", "5", "6", "7",
        "8", "9", "10", "11",
        NULL
    };

    static const char *spectrum_analyzer_x16_channels[]=
    {
        "0", "1", "2", "3", "4", "5", "6", "7",
        "8", "9", "10", "11", "12", "13", "14", "15",
        NULL
    };

    #define SA_INPUT(x, total) \
            AUDIO_INPUT_N(x), \
            AUDIO_OUTPUT_N(x), \
            { "on_" #x, "Analyse " #x, U_BOOL, R_CONTROL, F_IN, 0, 0, (x == 0) ? 1.0f : 0.0f, 0, NULL    }, \
            { "solo_" #x, "Solo " #x, U_BOOL, R_CONTROL, F_IN, 0, 0, 0, 0, NULL    }, \
            { "frz_" #x, "Freeze " #x, U_BOOL, R_CONTROL, F_IN, 0, 0, 0, 0, NULL    }, \
            { "hue_" #x, "Hue " #x, U_NONE, R_CONTROL, F_IN | F_UPPER | F_LOWER | F_STEP, 0.0f, 1.0f, (float(x) / float(total)), 0.25f/360.0f, NULL     }, \
            AMP_GAIN("sh_" #x, "Shift gain " #x, 1.0f, 1000.0f), \
            { "spc_" #x, "Spectrum " #x, U_NONE, R_MESH, F_OUT, 0.0, 0.0, spectrum_analyzer_base_metadata::MESH_POINTS, 2, NULL }

    #define SA_COMMON(c) \
            BYPASS, \
            { "tol", "FFT Tolerance", U_ENUM, R_CONTROL, F_IN, 0, 0, spectrum_analyzer_base_metadata::RANK_DFL - spectrum_analyzer_base_metadata::RANK_MIN, 0, fft_tolerance }, \
            { "wnd", "FFT Window", U_ENUM, R_CONTROL, F_IN, 0, 0, spectrum_analyzer_base_metadata::WND_DFL, 0, windows::windows }, \
            { "env", "FFT Envelope", U_ENUM, R_CONTROL, F_IN, 0, 0, spectrum_analyzer_base_metadata::ENV_DFL, 0, envelope::envelopes }, \
            AMP_GAIN("pamp", "Preamp gain", spectrum_analyzer_base_metadata::PREAMP_DFL, 1000.0f), \
            { "react",          "Reactivity",       U_SEC,          R_CONTROL, F_IN | F_UPPER | F_LOWER | F_STEP | F_LOG, \
                 spectrum_analyzer_base_metadata::REACT_TIME_MIN, spectrum_analyzer_base_metadata::REACT_TIME_MAX, spectrum_analyzer_base_metadata::REACT_TIME_DFL, spectrum_analyzer_base_metadata::REACT_TIME_STEP, NULL }, \
            { "chn", "Channel", U_ENUM, R_CONTROL, F_IN, 0, 0, 0, 0, spectrum_analyzer_x ## c ## _channels }, \
            { "sel", "Selector", U_PERCENT, R_CONTROL, F_IN | F_UPPER | F_LOWER | F_STEP | F_LOG, \
                 spectrum_analyzer_base_metadata::SELECTOR_MIN, spectrum_analyzer_base_metadata::SELECTOR_MAX, spectrum_analyzer_base_metadata::SELECTOR_DFL, spectrum_analyzer_base_metadata::SELECTOR_STEP, NULL }, \
            { "freq", "Frequency", U_HZ, R_METER, F_OUT | F_UPPER | F_LOWER, \
                spectrum_analyzer_base_metadata::FREQ_MIN, spectrum_analyzer_base_metadata::FREQ_MAX, spectrum_analyzer_base_metadata::FREQ_DFL, 0, NULL }, \
            { "lvl", "Level", U_GAIN_AMP, R_METER, F_OUT | F_UPPER | F_LOWER, 0, 10000, 0, 0, NULL }

    static const port_t spectrum_analyzer_x1_ports[] =
    {
        SA_INPUT(0, 1),
        SA_COMMON(1),
        PORTS_END
    };

    static const port_t spectrum_analyzer_x2_ports[] =
    {
        SA_INPUT(0, 2),
        SA_INPUT(1, 2),
        SA_COMMON(2),
        PORTS_END
    };

    static const port_t spectrum_analyzer_x4_ports[] =
    {
        SA_INPUT(0, 4),
        SA_INPUT(1, 4),
        SA_INPUT(2, 4),
        SA_INPUT(3, 4),
        SA_COMMON(4),
        PORTS_END
    };

    static const port_t spectrum_analyzer_x8_ports[] =
    {
        SA_INPUT(0, 8),
        SA_INPUT(1, 8),
        SA_INPUT(2, 8),
        SA_INPUT(3, 8),
        SA_INPUT(4, 8),
        SA_INPUT(5, 8),
        SA_INPUT(6, 8),
        SA_INPUT(7, 8),
        SA_COMMON(8),
        PORTS_END
    };

    static const port_t spectrum_analyzer_x12_ports[] =
    {
        SA_INPUT(0, 12),
        SA_INPUT(1, 12),
        SA_INPUT(2, 12),
        SA_INPUT(3, 12),
        SA_INPUT(4, 12),
        SA_INPUT(5, 12),
        SA_INPUT(6, 12),
        SA_INPUT(7, 12),
        SA_INPUT(8, 12),
        SA_INPUT(9, 12),
        SA_INPUT(10, 12),
        SA_INPUT(11, 12),
        SA_COMMON(12),
        PORTS_END
    };

    static const port_t spectrum_analyzer_x16_ports[] =
    {
        SA_INPUT(0, 16),
        SA_INPUT(1, 16),
        SA_INPUT(2, 16),
        SA_INPUT(3, 16),
        SA_INPUT(4, 16),
        SA_INPUT(5, 16),
        SA_INPUT(6, 16),
        SA_INPUT(7, 16),
        SA_INPUT(8, 16),
        SA_INPUT(9, 16),
        SA_INPUT(10, 16),
        SA_INPUT(11, 16),
        SA_INPUT(12, 16),
        SA_INPUT(13, 16),
        SA_INPUT(14, 16),
        SA_INPUT(15, 16),
        SA_COMMON(16),
        PORTS_END
    };

    #undef SA_INPUT
    #undef SA_COMMON

    const plugin_metadata_t  spectrum_analyzer_x1_metadata::metadata =
    {
        "Spektrumanalysator x1",
        "Spectrum Analyzer",
        "SA1",
        "Vladimir Sadovnikov",
        "qtez",
        LSP_VERSION(1, 0, 0),
        spectrum_analyzer_classes,
        spectrum_analyzer_x1_ports,
        NULL
    };

    const plugin_metadata_t  spectrum_analyzer_x2_metadata::metadata =
    {
        "Spektrumanalysator x2",
        "Spectrum Analyzer",
        "SA2",
        "Vladimir Sadovnikov",
        "aw7r",
        LSP_VERSION(1, 0, 0),
        spectrum_analyzer_classes,
        spectrum_analyzer_x2_ports,
        NULL
    };

    const plugin_metadata_t  spectrum_analyzer_x4_metadata::metadata =
    {
        "Spektrumanalysator x4",
        "Spectrum Analyzer",
        "SA4",
        "Vladimir Sadovnikov",
        "xzgo",
        LSP_VERSION(1, 0, 0),
        spectrum_analyzer_classes,
        spectrum_analyzer_x4_ports,
        NULL
    };

    const plugin_metadata_t  spectrum_analyzer_x8_metadata::metadata =
    {
        "Spektrumanalysator x8",
        "Spectrum Analyzer",
        "SA8",
        "Vladimir Sadovnikov",
        "e5hb",
        LSP_VERSION(1, 0, 0),
        spectrum_analyzer_classes,
        spectrum_analyzer_x8_ports,
        NULL
    };

    const plugin_metadata_t  spectrum_analyzer_x12_metadata::metadata =
    {
        "Spektrumanalysator x12",
        "Spectrum Analyzer",
        "SA12",
        "Vladimir Sadovnikov",
        "tj3l",
        LSP_VERSION(1, 0, 0),
        spectrum_analyzer_classes,
        spectrum_analyzer_x12_ports,
        NULL
    };

    const plugin_metadata_t  spectrum_analyzer_x16_metadata::metadata =
    {
        "Spektrumanalysator x16",
        "Spectrum Analyzer",
        "SA16",
        "Vladimir Sadovnikov",
        "nuzi",
        LSP_VERSION(1, 0, 0),
        spectrum_analyzer_classes,
        spectrum_analyzer_x16_ports,
        NULL
    };

#ifndef LSP_NO_EXPERMIENTAL
    //-------------------------------------------------------------------------
    // Impulse responses
    static const port_t impulse_responses_mono_ports[] =
    {
        // Input audio ports
        AUDIO_INPUT_MONO,

        // Output audio ports
        AUDIO_OUTPUT_MONO,

        // Input controls
        BYPASS,
        { "ifn",            "Impulse file",     U_STRING,       R_PATH,     F_IN, 0, 0, 0, 0, NULL },
        { "ifc",            "Channel",          U_ENUM,         R_CONTROL,  F_IN | F_INT, 0, 0, 0, 0, file_channels },
        { "ifl",            "Length",           U_PERCENT,      R_CONTROL,  F_IN | F_UPPER | F_LOWER | F_STEP,
            impulse_responses_base_metadata::CONVLEN_MIN, impulse_responses_base_metadata::CONVLEN_MAX, impulse_responses_base_metadata::CONVLEN_DFL, impulse_responses_base_metadata::CONVLEN_STEP, NULL },
        DRY_GAIN,
        WET_GAIN,

        OUT_GAIN,

        // Output controls

        PORTS_END
    };

    static const int impulse_responses_classes[] = { C_CONVERTER, C_SPECTRAL, -1 };

    const plugin_metadata_t  impulse_responses_metadata::metadata =
    {
        "Impulsantworten Mono",
        "Impulse Responses Mono",
        "IR1M",
        "Vladimir Sadovnikov",
        "xxxx",
        LSP_VERSION(1, 0, 0),
        impulse_responses_classes,
        impulse_responses_mono_ports,
        NULL
    };
#endif /* LSP_NO_EXPERMIENTAL */

}



