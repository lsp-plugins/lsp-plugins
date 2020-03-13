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
    static const port_item_t comp_delay_modes[] =
    {
        { "Samples",    "comp_delay.samples"    },
        { "Distance",   "comp_delay.distance"   },
        { "Time",       "comp_delay.time"       },
        { NULL, NULL }
    };

    static const port_t comp_delay_mono_ports[] =
    {
        // Input audio ports
        AUDIO_INPUT_MONO,

        // Output audio ports
        AUDIO_OUTPUT_MONO,

        // Input controls
        BYPASS,

        COMBO("mode", "Mode", 0, comp_delay_modes),
        SWITCH("ramp", "Ramping", 0.0f),
        INT_CONTROL("samp", "Samples", U_SAMPLES, comp_delay_base_metadata::SAMPLES),
        INT_CONTROL("m", "Meters", U_M, comp_delay_base_metadata::METERS),
        CONTROL("cm", "Centimeters", U_CM, comp_delay_base_metadata::CENTIMETERS),
        CONTROL("t", "Temperature", U_DEG_CEL, comp_delay_base_metadata::TEMPERATURE),
        CONTROL("time", "Time", U_MSEC, comp_delay_base_metadata::TIME),
        DRY_GAIN(0.0f),
        WET_GAIN(1.0f),

        OUT_GAIN,

        // Output controls
        METER_MINMAX("d_t", "Delay time", U_HZ, 0.0f, comp_delay_base_metadata::DELAY_OUT_MAX_TIME),
        METER_MINMAX("d_s", "Delay samples", U_SAMPLES, 0.0f, comp_delay_base_metadata::DELAY_OUT_MAX_SAMPLES),
        METER_MINMAX("d_d", "Delay distance", U_CM, 0.0f, comp_delay_base_metadata::DELAY_OUT_MAX_DISTANCE),

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

        COMBO("mode", "Mode", 0, comp_delay_modes),
        SWITCH("ramp", "Ramping", 0.0f),
        INT_CONTROL("samp", "Samples", U_SAMPLES, comp_delay_base_metadata::SAMPLES),
        INT_CONTROL("m", "Meters", U_M, comp_delay_base_metadata::METERS),
        CONTROL("cm", "Centimeters", U_CM, comp_delay_base_metadata::CENTIMETERS),
        CONTROL("t", "Temperature", U_DEG_CEL, comp_delay_base_metadata::TEMPERATURE),
        CONTROL("time", "Time", U_MSEC, comp_delay_base_metadata::TIME),
        DRY_GAIN(0.0f),
        WET_GAIN(1.0f),

        OUT_GAIN,

        // Output controls
        METER_MINMAX("d_t", "Delay time", U_HZ, 0.0f, comp_delay_base_metadata::DELAY_OUT_MAX_TIME),
        METER_MINMAX("d_s", "Delay samples", U_SAMPLES, 0.0f, comp_delay_base_metadata::DELAY_OUT_MAX_SAMPLES),
        METER_MINMAX("d_d", "Delay distance", U_CM, 0.0f, comp_delay_base_metadata::DELAY_OUT_MAX_DISTANCE),

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

        COMBO("mode_l", "Mode L", 0, comp_delay_modes),
        SWITCH("ramp_l", "Ramping L", 0.0f),
        INT_CONTROL("samp_l", "Samples L", U_SAMPLES, comp_delay_base_metadata::SAMPLES),
        INT_CONTROL("m_l", "Meters L", U_M, comp_delay_base_metadata::METERS),
        CONTROL("cm_l", "Centimeters L", U_CM, comp_delay_base_metadata::CENTIMETERS),
        CONTROL("t_l", "Temperature L", U_DEG_CEL, comp_delay_base_metadata::TEMPERATURE),
        CONTROL("time_l", "Time L", U_MSEC, comp_delay_base_metadata::TIME),
        DRY_GAIN_L(0.0f),
        WET_GAIN_L(1.0f),

        COMBO("mode_r", "Mode R", 0, comp_delay_modes),
        SWITCH("ramp_r", "Ramping R", 0.0f),
        INT_CONTROL("samp_r", "Samples R", U_SAMPLES, comp_delay_base_metadata::SAMPLES),
        INT_CONTROL("m_r", "Meters R", U_M, comp_delay_base_metadata::METERS),
        CONTROL("cm_r", "Centimeters R", U_CM, comp_delay_base_metadata::CENTIMETERS),
        CONTROL("t_r", "Temperature R", U_DEG_CEL, comp_delay_base_metadata::TEMPERATURE),
        CONTROL("time_r", "Time R", U_MSEC, comp_delay_base_metadata::TIME),
        DRY_GAIN_R(0.0f),
        WET_GAIN_R(1.0f),

        OUT_GAIN,

        // Output controls
        METER_MINMAX("d_t_l", "Delay time L", U_HZ, 0.0f, comp_delay_base_metadata::DELAY_OUT_MAX_TIME),
        METER_MINMAX("d_s_l", "Delay samples L", U_SAMPLES, 0.0f, comp_delay_base_metadata::DELAY_OUT_MAX_SAMPLES),
        METER_MINMAX("d_d_l", "Delay distance L", U_CM, 0.0f, comp_delay_base_metadata::DELAY_OUT_MAX_DISTANCE),

        METER_MINMAX("d_t_r", "Delay time R", U_HZ, 0.0f, comp_delay_base_metadata::DELAY_OUT_MAX_TIME),
        METER_MINMAX("d_s_r", "Delay samples R", U_SAMPLES, 0.0f, comp_delay_base_metadata::DELAY_OUT_MAX_SAMPLES),
        METER_MINMAX("d_d_r", "Delay distance R", U_CM, 0.0f, comp_delay_base_metadata::DELAY_OUT_MAX_DISTANCE),

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
        LSP_COMP_DELAY_BASE + 0,
        LSP_VERSION(1, 0, 2),
        comp_delay_classes,
        E_NONE,
        comp_delay_mono_ports,
        "delay/comp/mono.xml",
        NULL,
        mono_plugin_port_groups
    };

    const plugin_metadata_t  comp_delay_stereo_metadata::metadata =
    {
        "Verzögerungsausgleicher Stereo",
        "Delay Compensator Stereo",
        "VA1S",
        &developers::v_sadovnikov,
        "comp_delay_stereo",
        "qpwr",
        LSP_COMP_DELAY_BASE + 1,
        LSP_VERSION(1, 0, 2),
        comp_delay_classes,
        E_NONE,
        comp_delay_stereo_ports,
        "delay/comp/stereo.xml",
        NULL,
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
        LSP_COMP_DELAY_BASE + 2,
        LSP_VERSION(1, 0, 2),
        comp_delay_classes,
        E_NONE,
        comp_delay_x2_stereo_ports,
        "delay/comp/x2_stereo.xml",
        NULL,
        stereo_plugin_port_groups
    };

}



