/*
 * slap_delay.cpp
 *
 *  Created on: 30 янв. 2017 г.
 *      Author: sadko
 */

#include <metadata/plugins.h>
#include <metadata/ports.h>
#include <metadata/developers.h>

namespace lsp
{
    //-------------------------------------------------------------------------
    // Slap delay
    static const port_item_t slap_delay_modes[] =
    {
        { "Off",        "slap_delay.modes.off" },
        { "Time",       "slap_delay.modes.time" },
        { "Distance",   "slap_delay.modes.distance" },
        { "Notes",      "slap_delay.modes.notes" },
        { NULL, NULL }
    };

    static const port_item_t slap_delay_lines[] =
    {
        { "0-3",        "slap_delay.lines_0:3" },
        { "4-7",        "slap_delay.lines_4:7" },
        { "8-11",       "slap_delay.lines_8:11" },
        { "12-15",      "slap_delay.lines_12:15" },
        { NULL, NULL }
    };

    #define SLAP_PAN_MONO(id, label) \
        PAN_CTL("p" id, label " panorama", 0.0f)

    #define SLAP_PAN_STEREO(id, label) \
        PAN_CTL("pl" id, label " left channel panorama", -100.0f), \
        PAN_CTL("pr" id, label " right channel panorama", 100.0f)

    #define SLAP_DELAY_COMMON(pan)  \
        BYPASS, \
        COMBO("lsel", "Delay line selector", 0, slap_delay_lines), \
        CONTROL("temp", "Tem", U_DEG_CEL, slap_delay_base_metadata::TEMPERATURE), \
        CONTROL("pred", "Pre-delay", U_MSEC, slap_delay_base_metadata::PRED_TIME), \
        CONTROL("strch", "Stretch time", U_PERCENT, slap_delay_base_metadata::STRETCH), \
        CONTROL("tempo", "Tempo", U_BPM, slap_delay_base_metadata::TEMPO), \
        SWITCH("sync", "Tempo sync", 0.0f), \
        SWITCH("ramp", "Ramping delay", 0.0f), \
        pan("_in", "Input"), \
        DRY_GAIN(GAIN_AMP_0_DB), \
        SWITCH("dm", "Dry mute", 0.0f), \
        WET_GAIN(GAIN_AMP_0_DB), \
        SWITCH("wm", "Wet mute", 0.0f), \
        SWITCH("mono", "Mono output", 0.0f), \
        OUT_GAIN

    #define SLAP_DELAY_PROCESSOR(id, pan) \
        COMBO("dm" #id, "Delay " #id " mode", 0, slap_delay_modes), \
        pan(#id, "Delay " #id), \
        SWITCH("s" #id, "Delay " #id " solo", 0.0f), \
        SWITCH("m" #id, "Delay " #id " mute", 0.0f), \
        SWITCH("ph" #id, "Delay " #id " phase", 0.0f), \
        CONTROL("dt" #id, "Delay " #id " time", U_MSEC, slap_delay_base_metadata::TIME), \
        CONTROL("dd" #id, "Delay " #id " distance", U_M, slap_delay_base_metadata::DISTANCE), \
        CONTROL("df" #id, "Delay " #id " fraction", U_BAR, slap_delay_base_metadata::FRACTION), \
        INT_CONTROL("ds" #id, "Delay " #id " denominator", U_BEAT, slap_delay_base_metadata::DENOMINATOR), \
        SWITCH("eq" #id, "Equalizer " #id " on", 0.0f), \
        SWITCH("lfc" #id, "Delay " #id " low-cut", 0.0f), \
        LOG_CONTROL("flc" #id, "Delay " #id " low-cut frequency", U_HZ, slap_delay_base_metadata::LOW_CUT), \
        SWITCH("hfc" #id, "Delay " #id " high-cut", 0.0f), \
        LOG_CONTROL("fhc" #id, "Delay " #id " high-cut frequency", U_HZ, slap_delay_base_metadata::HIGH_CUT), \
        LOG_CONTROL("fbs" #id, "Delay " #id " sub-bass", U_GAIN_AMP, slap_delay_base_metadata::BAND_GAIN), \
        LOG_CONTROL("fbb" #id, "Delay " #id " bass", U_GAIN_AMP, slap_delay_base_metadata::BAND_GAIN), \
        LOG_CONTROL("fbm" #id, "Delay " #id " middle", U_GAIN_AMP, slap_delay_base_metadata::BAND_GAIN), \
        LOG_CONTROL("fbp" #id, "Delay " #id " presence", U_GAIN_AMP, slap_delay_base_metadata::BAND_GAIN), \
        LOG_CONTROL("fbt" #id, "Delay " #id " treble", U_GAIN_AMP, slap_delay_base_metadata::BAND_GAIN), \
        AMP_GAIN10("dg" #id, "Delay " #id " gain", GAIN_AMP_0_DB)

    #define SLAP_DELAY_PROCESSORS(pan) \
        SLAP_DELAY_PROCESSOR(0, pan), \
        SLAP_DELAY_PROCESSOR(1, pan), \
        SLAP_DELAY_PROCESSOR(2, pan), \
        SLAP_DELAY_PROCESSOR(3, pan), \
        SLAP_DELAY_PROCESSOR(4, pan), \
        SLAP_DELAY_PROCESSOR(5, pan), \
        SLAP_DELAY_PROCESSOR(6, pan), \
        SLAP_DELAY_PROCESSOR(7, pan), \
        SLAP_DELAY_PROCESSOR(8, pan), \
        SLAP_DELAY_PROCESSOR(9, pan), \
        SLAP_DELAY_PROCESSOR(10, pan), \
        SLAP_DELAY_PROCESSOR(11, pan), \
        SLAP_DELAY_PROCESSOR(12, pan), \
        SLAP_DELAY_PROCESSOR(13, pan), \
        SLAP_DELAY_PROCESSOR(14, pan), \
        SLAP_DELAY_PROCESSOR(15, pan)

    static const port_t slap_delay_mono_ports[] =
    {
        // Input audio ports
        AUDIO_INPUT_MONO,
        AUDIO_OUTPUT_LEFT,
        AUDIO_OUTPUT_RIGHT,
        SLAP_DELAY_COMMON(SLAP_PAN_MONO),
        SLAP_DELAY_PROCESSORS(SLAP_PAN_MONO),

        PORTS_END
    };

    static const port_t slap_delay_stereo_ports[] =
    {
        // Input audio ports
        PORTS_STEREO_PLUGIN,
        SLAP_DELAY_COMMON(SLAP_PAN_STEREO),
        SLAP_DELAY_PROCESSORS(SLAP_PAN_STEREO),

        PORTS_END
    };

    static const int slap_delay_classes[] = { C_DELAY, -1 };

    const plugin_metadata_t  slap_delay_mono_metadata::metadata =
    {
        "Slapback-Delay Mono",
        "Slapback Delay Mono",
        "SD16M",
        &developers::v_sadovnikov,
        "slap_delay_mono",
        "gt0d",
        LSP_SLAP_DELAY_BASE + 0,
        LSP_VERSION(1, 0, 1),
        slap_delay_classes,
        E_NONE,
        slap_delay_mono_ports,
        "delay/slap_delay/mono.xml",
        NULL,
        mono_to_stereo_plugin_port_groups
    };

    const plugin_metadata_t  slap_delay_stereo_metadata::metadata =
    {
        "Slapback-Delay Stereo",
        "Slapback Delay Stereo",
        "SD16S",
        &developers::v_sadovnikov,
        "slap_delay_stereo",
        "0xxj",
        LSP_SLAP_DELAY_BASE + 1,
        LSP_VERSION(1, 0, 1),
        slap_delay_classes,
        E_NONE,
        slap_delay_stereo_ports,
        "delay/slap_delay/stereo.xml",
        NULL,
        stereo_plugin_port_groups
    };

}

