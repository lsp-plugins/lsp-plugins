/*
 * trigger.cpp
 *
 *  Created on: 04 мая 2016 г.
 *      Author: sadko
 */

#include <metadata/plugins.h>
#include <metadata/ports.h>
#include <metadata/developers.h>

namespace lsp
{
    static const port_item_t trigger_sample_selectors[] =
    {
        { "0", "sampler.samp.0" },
        { "1", "sampler.samp.1" },
        { "2", "sampler.samp.2" },
        { "3", "sampler.samp.3" },
        { "4", "sampler.samp.4" },
        { "5", "sampler.samp.5" },
        { "6", "sampler.samp.6" },
        { "7", "sampler.samp.7" },
        { NULL, NULL }
    };

    static const port_item_t trigger_modes[] =
    {
        { "Peak",       "sidechain.peak"           },
        { "RMS",        "sidechain.rms"            },
        { "Low-Pass",   "sidechain.lowpass"        },
        { "Uniform",    "sidechain.uniform"        },
        { NULL, NULL }
    };

    static const port_item_t trigger_sources[] =
    {
        { "Middle",     "sidechain.middle" },
        { "Side",       "sidechain.side" },
        { "Left",       "sidechain.left" },
        { "Right",      "sidechain.right" },
        { NULL, NULL }
    };

    static const port_item_t trigger_areas[] =
    {
        { "Trigger",    "trigger.trig" },
        { "Instrument", "trigger.inst" },
        { NULL, NULL }
    };

    //-------------------------------------------------------------------------
    // Trigger
    #define T_FILE_GAIN_MONO \
        AMP_GAIN10("mx", "Sample mix gain", 1.0f)
    #define T_FILE_GAIN_STEREO \
        PAN_CTL("pl", "Sample left channel panorama", -100.0f), \
        PAN_CTL("pr", "Sample right channel panorama", 100.0f)

    #define T_SAMPLE_FILE(gain)         \
        PATH("sf", "Sample file"),      \
        CONTROL("hc", "Sample head cut", U_MSEC, sampler_kernel_metadata::SAMPLE_LENGTH), \
        CONTROL("tc", "Sample tail cut", U_MSEC, sampler_kernel_metadata::SAMPLE_LENGTH), \
        CONTROL("fi", "Sample fade in", U_MSEC, sampler_kernel_metadata::SAMPLE_LENGTH), \
        CONTROL("fo", "Sample fade out", U_MSEC, sampler_kernel_metadata::SAMPLE_LENGTH), \
        AMP_GAIN10("mk", "Sample makeup gain", 1.0f), \
        { "vl", "Sample velocity max",  U_PERCENT, R_CONTROL, F_IN | F_LOWER | F_UPPER | F_STEP | F_LOWERING, 0.0f, 100.0f, 0.0f, 0.25, NULL }, \
        CONTROL("pd", "Sample pre-delay", U_MSEC, sampler_kernel_metadata::PREDELAY), \
        { "on", "Sample enabled", U_BOOL, R_CONTROL, F_IN, 0, 0, 1.0f, 0, NULL }, \
        TRIGGER("ls", "Sample listen"), \
        gain, \
        BLINK("ac", "Sample activity"), \
        BLINK("no", "Sample note on event"), \
        { "fl", "Sample length", U_MSEC, R_METER, F_OUT | F_LOWER | F_UPPER | F_STEP, \
                sampler_kernel_metadata::SAMPLE_LENGTH_MIN, sampler_kernel_metadata::SAMPLE_LENGTH_MAX, 0, sampler_kernel_metadata::SAMPLE_LENGTH_STEP, NULL }, \
        STATUS("fs", "Sample load status"), \
        MESH("fd", "Sample file contents", sampler_kernel_metadata::TRACKS_MAX, sampler_kernel_metadata::MESH_SIZE)

    #define T_METERS_MONO                   \
        MESH("isg", "Input signal graph", trigger_base_metadata::TRACKS_MAX, trigger_base_metadata::HISTORY_MESH_SIZE), \
        METER_GAIN20("ism", "Input signal meter"), \
        SWITCH("isv", "Input signal display", 1.0f)

    #define T_METERS_STEREO                 \
        COMBO("ssrc", "Signal source", 0, trigger_sources), \
        MESH("isgl", "Input signal graph left", trigger_base_metadata::TRACKS_MAX, trigger_base_metadata::HISTORY_MESH_SIZE), \
        MESH("isgr", "Input signal graph right", trigger_base_metadata::TRACKS_MAX, trigger_base_metadata::HISTORY_MESH_SIZE), \
        METER_GAIN20("isml", "Input signal meter left"), \
        METER_GAIN20("ismr", "Input signal meter right"), \
        SWITCH("isvl", "Input signal left display", 1.0f), \
        SWITCH("isvr", "Input signal right display", 1.0f)

    #define T_PORTS_GLOBAL(sample)  \
        COMBO("asel", "Area selector", 0, trigger_areas), \
        BYPASS,                 \
        DRY_GAIN(1.0f),         \
        WET_GAIN(1.0f),         \
        OUT_GAIN, \
        COMBO("mode", "Detection mode", trigger_base_metadata::MODE_DFL, trigger_modes), \
        SWITCH("pause", "Pause graph analysis", 0.0f), \
        TRIGGER("clear", "Clear graph analysis"), \
        AMP_GAIN100("preamp", "Signal pre-amplification", 1.0f), \
        AMP_GAIN10("dl", "Detect level", trigger_base_metadata::DETECT_LEVEL_DFL), \
        CONTROL("dt", "Detect time", U_MSEC, trigger_base_metadata::DETECT_TIME), \
        AMP_GAIN1("rrl", "Relative release level", trigger_base_metadata::RELEASE_LEVEL_DFL), \
        CONTROL("rt", "Release time", U_MSEC, trigger_base_metadata::RELEASE_TIME), \
        CONTROL("dyna", "Dynamics", U_PERCENT, trigger_base_metadata::DYNAMICS), \
        AMP_GAIN("dtr1", "Dynamics range 1", GAIN_AMP_P_6_DB, 20.0f), \
        AMP_GAIN("dtr2", "Dynamics range 2", GAIN_AMP_M_36_DB, 20.0f), \
        CONTROL("react", "Reactivity", U_MSEC, trigger_base_metadata::REACTIVITY), \
        METER_OUT_GAIN("rl", "Release level", 20.0f), \
        MESH("tfg", "Trigger function graph", trigger_base_metadata::TRACKS_MAX, trigger_base_metadata::HISTORY_MESH_SIZE), \
        METER_GAIN20("tfm", "Trigger function meter"), \
        SWITCH("tfv", "Trigger function display", 1.0f), \
        BLINK("tla", "Trigger activity"), \
        MESH("tlg", "Trigger level graph", trigger_base_metadata::TRACKS_MAX, trigger_base_metadata::HISTORY_MESH_SIZE), \
        METER_GAIN20("tlm", "Trigger level meter"), \
        SWITCH("tlv", "Trigger level display", 1.0f), \
        TRIGGER("lstn", "Trigger listen"), \
        PORT_SET("ssel", "Sample selector", trigger_sample_selectors, sample)

    #define T_MIDI_PORTS                    \
        COMBO("chan", "Channel", trigger_midi_metadata::CHANNEL_DFL, midi_channels), \
        COMBO("note", "Note", trigger_midi_metadata::NOTE_DFL, notes), \
        COMBO("oct", "Octave", trigger_midi_metadata::OCTAVE_DFL, octaves), \
        { "mn", "MIDI Note #", U_NONE, R_METER, F_OUT | F_LOWER | F_UPPER | F_INT, 0, 127, 0, 0, NULL }

    static const port_t sample_file_mono_ports[] =
    {
        T_SAMPLE_FILE(T_FILE_GAIN_MONO),
        PORTS_END
    };

    static const port_t sample_file_stereo_ports[] =
    {
        T_SAMPLE_FILE(T_FILE_GAIN_STEREO),
        PORTS_END
    };

    static const port_t trigger_mono_ports[] =
    {
        PORTS_MONO_PLUGIN,
        T_METERS_MONO,
        T_PORTS_GLOBAL(sample_file_mono_ports),

        PORTS_END
    };

    static const port_t trigger_stereo_ports[] =
    {
        PORTS_STEREO_PLUGIN,
        T_METERS_STEREO,
        T_PORTS_GLOBAL(sample_file_stereo_ports),

        PORTS_END
    };

    static const port_t trigger_mono_midi_ports[] =
    {
        PORTS_MONO_PLUGIN,
        T_METERS_MONO,
        PORTS_MIDI_CHANNEL,
        T_MIDI_PORTS,
        T_PORTS_GLOBAL(sample_file_mono_ports),

        PORTS_END
    };

    static const port_t trigger_stereo_midi_ports[] =
    {
        PORTS_STEREO_PLUGIN,
        T_METERS_STEREO,
        PORTS_MIDI_CHANNEL,
        T_MIDI_PORTS,
        T_PORTS_GLOBAL(sample_file_stereo_ports),

        PORTS_END
    };

    static const int trigger_classes[] = { C_DYNAMICS, -1 };

    //-------------------------------------------------------------------------
    // Define plugin metadata
    const plugin_metadata_t  trigger_mono_metadata::metadata =
    {
        "Triggersensor Mono",
        "Trigger Mono",
        "TS1M",
        &developers::v_sadovnikov,
        "trigger_mono",
        "zghv",
        0,
        LSP_VERSION(1, 0, 1),
        trigger_classes,
        E_INLINE_DISPLAY,
        trigger_mono_ports,
        "trigger/single/mono.xml",
        NULL,
        mono_plugin_port_groups
    };

    const plugin_metadata_t  trigger_stereo_metadata::metadata =
    {
        "Triggersensor Stereo",
        "Trigger Stereo",
        "TS1S",
        &developers::v_sadovnikov,
        "trigger_stereo",
        "zika",
        0,
        LSP_VERSION(1, 0, 1),
        trigger_classes,
        E_INLINE_DISPLAY,
        trigger_stereo_ports,
        "trigger/single/stereo.xml",
        NULL,
        stereo_plugin_port_groups
    };

    const plugin_metadata_t  trigger_midi_mono_metadata::metadata =
    {
        "Triggersensor MIDI Mono",
        "Trigger MIDI Mono",
        "TSM1M",
        &developers::v_sadovnikov,
        "trigger_midi_mono",
        "t4yz",
        0,
        LSP_VERSION(1, 0, 1),
        trigger_classes,
        E_INLINE_DISPLAY,
        trigger_mono_midi_ports,
        "trigger/single/mono.xml",
        NULL,
        mono_plugin_port_groups
    };

    const plugin_metadata_t  trigger_midi_stereo_metadata::metadata =
    {
        "Triggersensor MIDI Stereo",
        "Trigger MIDI Stereo",
        "TSM1S",
        &developers::v_sadovnikov,
        "trigger_midi_stereo",
        "9cqf",
        0,
        LSP_VERSION(1, 0, 1),
        trigger_classes,
        E_INLINE_DISPLAY,
        trigger_stereo_midi_ports,
        "trigger/single/stereo.xml",
        NULL,
        stereo_plugin_port_groups
    };
}
